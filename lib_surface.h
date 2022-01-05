#ifndef _LIB_SURFACE_H_
#define _LIB_SURFACE_H_

#include <stdlib.h>

/*
 * GENERALITES
 */

#define MIN(a,b) ((a)<(b)?(a):(b))
#define MAX(a,b) ((a)>(b)?(a):(b))

#define RX 800
#define RY 600

/*
 * DEFINITIONS, DEPENDANT DE LA BIBLIOTHEQUE D'AFFICHAGE
 */

#include <SDL2/SDL.h>

typedef struct {
	int x, y;
	int *xmin;		// pour eviter un malloc dans remplirTriangle2d
	int *xmax;		// pour eviter un malloc dans remplirTriangle2d

	Uint32 *ecran;
	SDL_Texture *texture;
	SDL_Renderer *renderer;
	SDL_Window *window;

} t_surface;

#if __STDC_VERSION__ == 201112L	/* C11 version */
 /* si ce n'est pas un t_surface*, ne pourra pas etre utilise comme lvalue */
#define t_surface_x(this) _Generic(this, t_surface*:((this)->x), default:0)
#define t_surface_y(this) _Generic(this, t_surface*:((this)->y), default:0)
#define t_surface_xmin(this) _Generic(this, t_surface*:((this)->xmin), default:NULL)
#define t_surface_xmax(this) _Generic(this, t_surface*:((this)->xmax), default:NULL)
#else
#define t_surface_x(this) ((this)->x)
#define t_surface_y(this) ((this)->y)
#define t_surface_xmin(this) ((this)->xmin)
#define t_surface_xmax(this) ((this)->xmax)
#endif

t_surface *creerFenetre(int x, int y);
void libererFenetre(t_surface * surface);

void effacerEcran(t_surface * surface);

void remplirPixel(t_surface * surface, int x, int y, Uint32 pixel);
void remplirLigneHorizontale(t_surface * surface, int x, int y, int l, Uint32 pixel);
void majEcran(t_surface * surface);

void definirTitreFenetre(t_surface * surface, const char *titre);

/*
 * DIVERS, GESTION DES COULEURS
 */

#define BLANC  0x00ffffff
#define GRISC  0x00aaaaaa
#define GRISF  0x00555555
#define NOIR   0x00000000

#define ROUGEC 0x00ff0000
#define ROUGEF 0x00880000
#define VERTC  0x0000ff00
#define VERTF  0x00008800
#define BLEUC  0x000000ff
#define BLEUF  0x00000088

#define JAUNEC 0x00ffff00
#define JAUNEF 0x00888800
#define PALEC  0x0000ffff
#define PALEF  0x00008888
#define ROSEC  0x00ff00ff
#define ROSEF  0x00880088

#define MARRON1 0x00AD4F09
#define MARRON2 0x00A76726
#define MARRON3 0x0088421D
#define MARRON4 0x00AE642D

#define MAUVE   0x007F29DA
#define ORANGE	0x00ED7F10

/* pour affichage avec %s dans printf, exemple :
 * printf("couleur = %s\n", COULEUR(valeur_numerique));
 */
#define COULEUR(c) ((c==BLANC)?"BLANC":(\
(c==GRISC)?"GRISC":(\
(c==GRISF)?"GRISF":(\
(c==NOIR)?"NOIR":(\
(c==ROUGEC)?"ROUGEC":(\
(c==ROUGEF)?"ROUGEF":(\
(c==VERTC)?"VERTC":(\
(c==VERTF)?"VERTF":(\
(c==BLEUC)?"BLEUC":(\
(c==BLEUF)?"BLEUF":(\
(c==JAUNEC)?"JAUNEC":(\
(c==JAUNEF)?"JAUNEF":(\
(c==PALEC)?"PALEC":(\
(c==PALEF)?"PALEF":(\
(c==ROSEC?"ROSEC":(\
(c==ROSEF)?"ROSEF":(\
"AUTRE"))))))))))))))))))

#define COULEUR_Uint32_R(c) (((c)>>16) & 0xFF)
#define COULEUR_Uint32_G(c) (((c)>>8) & 0xFF)
#define COULEUR_Uint32_B(c) ((c) & 0xFF)
#define COULEUR_RGB_Uint32(r,g,b) ((Uint32)(((r)<<16) + ((g)<<8) + (b)))

/* donne le code couleur dans l'arc en ciel (z dans l'intervalle [0,1275]) */
Uint32 arc_en_ciel(int z);

/* donne le code couleur (z dans l'intervalle [0,100]) dans le dégradé de a à b */
Uint32 couleur_entre(int z, Uint32 a, Uint32 b);

void affiche_hexa(Uint32 c);	/* affiche la chaine 0x.... correspondant à c */

#endif
