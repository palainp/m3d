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
	t_point3d* abc[3];
} t_triangle3d;

t_point3d* definirPoint3d(double x, double y, double z);
void libererPoint3d(t_point3d *p);
t_point3d* copierPoint3d(t_point3d* p);

t_vecteur3d* definirVecteur3d(double x, double y, double z);
void libererVecteur3d(t_vecteur3d *p);

t_triangle3d* definirTriangle3d(t_point3d* a, t_point3d* b, t_point3d* c);
void libererTriangle3d(t_triangle3d *t);
t_triangle3d* copierTriangle3d(t_triangle3d* t);
double zmoyen(t_triangle3d* t);

void afficherTriangle3d(t_surface *s, t_triangle3d *t, Uint32 couleur);

void translationTriangle3d(t_triangle3d *t, t_vecteur3d* v);
void rotationTriangle3d(t_triangle3d *t, t_point3d *c, double x, double y, double z);
void homothetieTriangle3d(t_triangle3d *t, double x, double y, double z);
void transformationTriangle3d(t_triangle3d *t, double mat[4][4]);

#endif
