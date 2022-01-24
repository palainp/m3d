#ifndef _LIB_SCENE3D_H_
#define _LIB_SCENE3D_H_

#include "lib_surface.h"
#include "lib_objet3d.h"

typedef struct _node {
	double montant[4][4], descendant[4][4];

	t_objet3d* pt_objet;

	struct _node* pt_pere;
	struct _node* pt_fils;
	struct _node* pt_frere;
} t_node;
typedef t_node* t_scene3d;

t_scene3d creerScene3d(t_objet3d* pt_objet);
void lierScene3d(t_scene3d pt_pere, t_scene3d pt_fils);
void afficherScene3d(t_surface *s, t_scene3d scene);

void libererScene3d(t_scene3d scene);

#endif
