#ifndef _LIB_2D_H_
#define _LIB_2D_H_

#include "lib_surface.h"

typedef struct {
	int x,y;
} t_point2d;

typedef struct {
	t_point2d* abc[3];
} t_triangle2d;

t_point2d* definirPoint2d(int x, int y);
void libererPoint2d(t_point2d *p);

t_triangle2d* definirTriangle2d(t_point2d* a, t_point2d* b, t_point2d* c);
void libererTriangle2d(t_triangle2d *t);
void afficherCoordonneesTriangle2d(t_triangle2d *t);

void afficherTriangle2d(t_surface *s, t_triangle2d *t, Uint32 couleur);
#endif
