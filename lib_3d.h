#ifndef _LIB_3D_H_
#define _LIB_3D_H_

#include "lib_surface.h"
#include "lib_2d.h"

typedef struct {
	double xyzt[4];
} t_point3d;

typedef struct {
	double xyzt[4];
} t_vecteur3d;

typedef struct {
	t_point3d abc[3];
} t_triangle3d;

t_point3d definirPoint3d(double x, double y, double z);
t_point3d copierPoint3d(t_point3d* p);

t_vecteur3d definirVecteur3d(double x, double y, double z);

void transformationPoint3d(t_point3d *p, double mat[4][4], t_point3d* tmp); // tmp sert pour limiter le nombre d'allocation, optimisation prématurée ?

t_triangle3d definirTriangle3d(t_point3d* a, t_point3d* b, t_point3d* c);
void libererTriangle3d(t_triangle3d *t);
t_triangle3d copierTriangle3d(t_triangle3d* t);

void afficherTriangle3d(t_surface *s, t_triangle3d *t, Uint32 couleur);

void transformationTriangle3d(t_triangle3d *t, double mat[4][4]);

#endif
