#include <stdio.h>
#include <unistd.h>
#include <stdbool.h>
#include <assert.h>
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
t_node* camera_active;
static t_point3d origine;

t_scene3d charger_scene3d(const char* fn);

bool handle_events()
{

	SDL_PumpEvents();

	if (event.type == SDL_QUIT || currentKeyStates[SDL_SCANCODE_Q]) {
		return true;
	} else if (event.type == SDL_MOUSEMOTION && is_button_down) {
		rotationScene3d(camera_active, &origine, y-event.motion.y, x-event.motion.x, 0);
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

int main(int argc, char* argv[])
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

	if (argc-1!=1)
	{
		fprintf(stderr, "Usage: %s <scene>\n", argv[0]);
		exit(EXIT_FAILURE);
	}

	origine = definirPoint3d(0,0,0);
	printf("Loading scene '%s'...", argv[1]); fflush(stdout);
	t_scene3d scn = charger_scene3d(argv[1]);
	printf("done\n");
	assert(scn.n_cam>0);
	camera_active = scn.cam[0];

	while (++i < DUREE * 60 && !quit_requested)	// DUREE " * 60FPS
	{
		effacerEcran(surface);

		if (SDL_PollEvent(&event)) {
			quit_requested = handle_events();
		}

		afficherScene3d(surface, camera_active);
		majEcran(surface);

		/* Framerate fixe a env 60 FPS max */
		SDL_Delay((1000 / 60) - MIN((1000 / 60), (SDL_GetTicks() - oldtime)));
		oldtime = SDL_GetTicks();

		cpt++;
		snprintf(buf, sizeof(buf), "%d FPS", (int)(cpt * 1000.0 / (oldtime - timestart)));
		definirTitreFenetre(surface, buf);
	}

	libererFenetre(surface);
	libererScene3d(camera_active);

	return 0;
}
