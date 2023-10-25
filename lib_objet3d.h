#ifndef _OBJET_3D_H_
#define _OBJET_3D_H_

#include <stdbool.h>
#include "lib_surface.h"
#include "lib_3d.h"

typedef struct {
	size_t t[3];
	Uint32 c;
} t_face;

typedef struct {
	bool est_trie, est_camera;
	t_point3d** p; // p est un tableau de ptr sur des points
	size_t np;
	t_face* faces; // face est un tableau d'indices de points
	size_t nfaces;
} t_objet3d;

t_objet3d* objet_vide();
t_objet3d* camera();
t_objet3d* cube(int l);
t_objet3d* fichierObjet3d(const char* fn, Uint32 c1, Uint32 c2);

void libererObjet3d(t_objet3d *o);
void afficherObjet3d(t_surface* s, t_objet3d* o);
t_objet3d* copierObjet3d(t_objet3d* o);
void concatenerObjet3d(t_objet3d *a, t_objet3d *b); // a=a+b (b n'aura plus de sens ensuite)

void trierObjet3d(t_objet3d *o);

void translationObjet3d(t_objet3d *o, t_vecteur3d* v);
void rotationObjet3d(t_objet3d *o, t_point3d *c, double x, double y, double z);
void homothetieObjet3d(t_objet3d *o, double x, double y, double z);
void transformationObjet3d(t_objet3d *o, double mat[4][4]);

#endif
