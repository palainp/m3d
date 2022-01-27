#include <assert.h>
#include "lib_surface.h"

/**
 * L'utilisation de _Generic est moins importante car ces #define n'ont de portee que ce fichier .c : undef en fin de .c
 */
#define t_surface_ecran(this) ((this)->ecran)
#define t_surface_texture(this) ((this)->texture)
#define t_surface_renderer(this) ((this)->renderer)
#define t_surface_window(this) ((this)->window)

t_surface *creerFenetre(int x, int y)
{
	t_surface *S;

	S = (t_surface *) malloc(sizeof(t_surface));

	if (S != NULL) {
		t_surface_x(S) = x;
		t_surface_y(S) = y;

		SDL_CreateWindowAndRenderer(t_surface_x(S), t_surface_y(S),
					    SDL_WINDOW_RESIZABLE , &t_surface_window(S), &t_surface_renderer(S));
		if (t_surface_window(S) == NULL || t_surface_renderer(S) == NULL) {
			libererFenetre(S);
			S = NULL;
		}

		SDL_SetRenderDrawColor(t_surface_renderer(S), 0, 0, 0, 0xFF);
		SDL_RenderClear(t_surface_renderer(S));
		SDL_RenderPresent(t_surface_renderer(S));

		SDL_RenderSetLogicalSize(t_surface_renderer(S), t_surface_x(S), t_surface_y(S));

		t_surface_ecran(S) = (Uint32 *) malloc((size_t)(t_surface_x(S) * t_surface_y(S)) * sizeof(Uint32));	// le tableau de Uint32 qu'on modifie pour dessiner l'image
		t_surface_texture(S) = SDL_CreateTexture(t_surface_renderer(S),
							 SDL_PIXELFORMAT_ARGB8888, SDL_TEXTUREACCESS_STREAMING, t_surface_x(S), t_surface_y(S));

		t_surface_xmin(S) = (int *)malloc((size_t)t_surface_y(S) * sizeof(int));
		t_surface_xmax(S) = (int *)malloc((size_t)t_surface_y(S) * sizeof(int));

		if (t_surface_xmin(S) == NULL || t_surface_xmax(S) == NULL) {
			libererFenetre(S);
			S = NULL;
		} else {
			for (size_t i=0; i<(size_t)t_surface_y(S); ++i)
			{
				t_surface_xmax(S)[i] = 0;
				t_surface_xmin(S)[i] = t_surface_x(S) - 1;
			}
		}
	}
	return S;
}

void libererFenetre(t_surface * surface)
{
	assert(surface!=NULL);

	free(t_surface_xmin(surface));
	free(t_surface_xmax(surface));
	free(t_surface_ecran(surface));
	SDL_DestroyTexture(t_surface_texture(surface));
	SDL_DestroyRenderer(t_surface_renderer(surface));
	SDL_DestroyWindow(t_surface_window(surface));
	free(surface);
}

void effacerEcran(t_surface * surface)
{
	assert(surface!=NULL);

	memset(t_surface_ecran(surface), 0, (size_t)(t_surface_x(surface) * t_surface_y(surface)) * sizeof(Uint32));
}

void majEcran(t_surface * surface)
{
	assert(surface!=NULL);

	SDL_UpdateTexture(t_surface_texture(surface), NULL, t_surface_ecran(surface), t_surface_x(surface) * (int)sizeof(Uint32));	// Ecran -> Texture

	SDL_RenderClear(t_surface_renderer(surface));
	SDL_RenderCopy(t_surface_renderer(surface), t_surface_texture(surface), NULL, NULL);	// Texture -> Renderer

	SDL_RenderPresent(t_surface_renderer(surface));	// Renderer -> Window
}

void definirTitreFenetre(t_surface * surface, const char *titre)
{
	assert(surface!=NULL);
	assert(titre!=NULL);

	SDL_SetWindowTitle(t_surface_window(surface), titre);
}

void remplirPixel(t_surface * surface, int x, int y, Uint32 pixel)
{
	assert(surface!=NULL);

	unsigned char *p = (unsigned char *)t_surface_ecran(surface) + (y * t_surface_x(surface) + x) * (int)sizeof(Uint32);
	*(Uint32 *) p = pixel;
}

#if 0
void remplirLigneHorizontale(t_surface * surface, int x, int y, int l, Uint32 pixel)
{
	assert(surface!=NULL);

	int i;
	for (i = 0; i < l; i++) {
		remplirPixel(surface, x + i, y, pixel);
	}
}
#else
void remplirLigneHorizontale(t_surface * surface, int x, int y, int l, Uint32 pixel)
{
	assert(surface!=NULL);
	assert(x>=0);
	assert(x<t_surface_x(surface));
	assert(y>=0);
	assert(y<t_surface_y(surface));

	int i;
	// il s'agit d'un ptr sur un Uint32, l'arithmetique des pointeurs fait que l'operation +1 decale directement de sizeof(Uint32) octets, on n'a donc pas le même pb que pour la fonction precedente
	Uint32 *ptr_ecran = t_surface_ecran(surface) + (y * t_surface_x(surface) + x);

#ifdef __UINT64_TYPE__
	Uint64 pixel64 = pixel;
	Uint64 double_pixel = pixel64 << 32 | pixel64;

	// copie par mots de 64b, il restera eventuellemet un Uint32 a copier a la fin
	for (i = 0; i < l / 2; i++) {
		memcpy(ptr_ecran, &double_pixel, sizeof(Uint64));
		ptr_ecran += 2;
	}
	if (l % 2 == 1) {
		memcpy(ptr_ecran, &pixel, sizeof(Uint32));
	}
#else
	// copie par mots de 32b
	for (i = 0; i < l; i++) {
		memcpy(ptr_ecran, &pixel, sizeof(Uint32));
		ptr_ecran += 1;
	}
#endif

}
#endif

Uint32 arc_en_ciel(int z)	// z \in [0, 1275]
{
	if (z < 255) {
		return couleur_entre(100 * z / 255, ROUGEC, JAUNEC);
	} else if (z < 510) {
		return couleur_entre(100 * z / 255, JAUNEC, VERTC);
	} else if (z < 765) {
		return couleur_entre(100 * z / 255, VERTC, PALEC);
	} else if (z < 1020) {
		return couleur_entre(100 * z / 255, PALEC, BLEUC);
	} else {
		return couleur_entre(100 * z / 255, JAUNEC, VERTC);
	}
}

Uint32 couleur_entre(int z, Uint32 a, Uint32 b)	// z \in [0, 100]
{
	Uint8 ra = COULEUR_Uint32_R(a), ga = COULEUR_Uint32_G(a), ba = COULEUR_Uint32_B(a);
	Uint8 rb = COULEUR_Uint32_R(b), gb = COULEUR_Uint32_G(b), bb = COULEUR_Uint32_B(b);
	Uint8 rc = (Uint8) (ra + (z / 100.0) * (rb - ra)), gc = (Uint8) (ga + (z / 100.0) * (gb - ga)), bc = (Uint8) (ba + (z / 100.0) * (bb - ba));
	Uint32 c = COULEUR_RGB_Uint32(rc, gc, bc);
	return c;
}

void affiche_hexa(Uint32 c)
{				/* affiche la chaine 0x.... correspondant à c */
	Uint8 a = (Uint8) ((c >> 24) & 0xFF), r = COULEUR_Uint32_R(c), g = COULEUR_Uint32_G(c), b = COULEUR_Uint32_B(c);
	printf("|0x|%02x|%02x|%02x|%02x|", a, r, g, b);
}

#undef t_surface_ecran
#undef t_surface_texture
#undef t_surface_renderer
#undef t_surface_window
