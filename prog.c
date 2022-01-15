#include <stdio.h>
#include <unistd.h>
#include <stdbool.h>
#include "lib_surface.h"
#include "lib_2d.h"
#include "lib_3d.h"
#include "lib_objet3d.h"

#define DUREE 10*60

SDL_Event event;
int x = 0, y = 0;
bool is_button_down = false;
const Uint8 *currentKeyStates = NULL;

bool handle_events()
{

	SDL_PumpEvents();

	if (event.type == SDL_QUIT || currentKeyStates[SDL_SCANCODE_Q]) {
		return true;
	} else if (event.type == SDL_MOUSEMOTION && is_button_down) {
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

	t_point3d *ctr = definirPoint3d(10, 20, 30);
	t_objet3d *cb = cube(50);
	homothetieObjet3d(cb, 5, 10, 5);

	while (++i < DUREE * 60 && !quit_requested)	// DUREE " * 60FPS
	{
		effacerEcran(surface);

		if (SDL_PollEvent(&event)) {
			quit_requested = handle_events();
		}

		rotationObjet3d(cb, ctr, 1, 2, 3);
		afficherObjet3d(surface, cb);
		majEcran(surface);

		/* Framerate fixe a env 60 FPS max */
		SDL_Delay((1000 / 60) - MIN((1000 / 60), (SDL_GetTicks() - oldtime)));
		oldtime = SDL_GetTicks();

		cpt++;
		snprintf(buf, sizeof(buf), "%d FPS", (int)(cpt * 1000.0 / (oldtime - timestart)));
		definirTitreFenetre(surface, buf);
	}

	libererFenetre(surface);
	libererObjet3d(cb);
	libererPoint3d(ctr);

	return 0;
}
