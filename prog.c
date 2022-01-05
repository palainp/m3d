#include <stdio.h>
#include <unistd.h>
#include <stdbool.h>
#include "lib_surface.h"

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

	while (++i < DUREE * 60 && !quit_requested)	// DUREE " * 60FPS
	{
		effacerEcran(surface);

		if (SDL_PollEvent(&event)) {
			quit_requested = handle_events();
		}

		majEcran(surface);

		/* Framerate fixe a env 60 FPS max */
		SDL_Delay((1000 / 60) - MIN((1000 / 60), (SDL_GetTicks() - oldtime)));
		oldtime = SDL_GetTicks();

		cpt++;
		snprintf(buf, sizeof(buf), "%d FPS", (int)(cpt * 1000.0 / (oldtime - timestart)));
		definirTitreFenetre(surface, buf);
	}

	libererFenetre(surface);

	return 0;
}
