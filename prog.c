#include <stdio.h>
#include <unistd.h>
#include <stdbool.h>
#include "lib_surface.h"
#include "lib_2d.h"
#include "lib_3d.h"
#include "lib_objet3d.h"
#include "lib_scene3d.h"

#define DUREE 10*60

SDL_Event event;
int x = 0, y = 0;
bool is_button_down = false;
const Uint8 *currentKeyStates = NULL;
t_scene3d cam;
t_point3d* origine;


bool handle_events()
{

	SDL_PumpEvents();

	if (event.type == SDL_QUIT || currentKeyStates[SDL_SCANCODE_Q]) {
		return true;
	} else if (event.type == SDL_MOUSEMOTION && is_button_down) {
		rotationScene3d(cam, origine, y-event.motion.y, x-event.motion.x, 0);
		x = event.motion.x;
		y = event.motion.y;
	} else if (event.type == SDL_MOUSEBUTTONDOWN) {
		is_button_down = true;
		x = event.motion.x;
		y = event.motion.y;
	} else if (event.type == SDL_MOUSEBUTTONUP) {
		is_button_down = false;
	}

	return false;
}

int main(int argc, char **argv)
{
	t_surface *surface = NULL;
	int cpt = 0;
	Uint32 timestart, oldtime;
	char buf[50];
	int i = 0;
	bool quit_requested = false;

	currentKeyStates = SDL_GetKeyboardState(NULL);
	surface = creerFenetre(RX, RY);
	timestart = SDL_GetTicks();
	oldtime = timestart;

	origine = definirPoint3d(0,0,0);
	t_vecteur3d *v1 = definirVecteur3d(0,80,0);
	t_vecteur3d *v2 = definirVecteur3d(0,-80,0);
	t_vecteur3d *v3 = definirVecteur3d(0,0,100);

	t_objet3d* cb = fichierObjet3d("cube.obj", BLANC, ROUGEC);
	homothetieObjet3d(cb, 60, 150, 150);
	rotationObjet3d(cb, origine, 180, 0, 0);
	translationObjet3d(cb, v2);

	t_objet3d* cow = fichierObjet3d("cow-nonormals.obj", BLANC, NOIR);
	homothetieObjet3d(cow, 50, 50, 50);
	translationObjet3d(cow, v1);

	t_objet3d* cmr = camera();

	cam = creerScene3d(cmr);
	translationScene3d(cam, v3);
	t_scene3d root = creerScene3d(cow);
	lierScene3d(root, cam);
	lierScene3d(root, creerScene3d(cb));

	while (++i < DUREE * 60 && !quit_requested)	// DUREE " * 60FPS
	{
		effacerEcran(surface);

		if (SDL_PollEvent(&event)) {
			quit_requested = handle_events();
		}

		afficherScene3d(surface, cam);
		majEcran(surface);

		/* Framerate fixe a env 60 FPS max */
		SDL_Delay((1000 / 60) - MIN((1000 / 60), (SDL_GetTicks() - oldtime)));
		oldtime = SDL_GetTicks();

		cpt++;
		snprintf(buf, sizeof(buf), "%d FPS", (int)(cpt * 1000.0 / (oldtime - timestart)));
		definirTitreFenetre(surface, buf);
	}

	libererFenetre(surface);
	libererScene3d(root);
	libererVecteur3d(v1);
	libererVecteur3d(v2);
	libererVecteur3d(v3);
	libererPoint3d(origine);

	return 0;
}
