#include <string.h>
#include <assert.h>
#include "lib_mat.h"
#include "lib_scene3d.h"

#define IDENTITE (double[4][4]){\
	{1, 0, 0, 0}, \
	{0, 1, 0, 0}, \
	{0, 0, 1, 0}, \
	{0, 0, 0, 1} \
}

t_scene3d creerScene3d(t_objet3d* pt_objet)
{
	assert(pt_objet != NULL);

	t_node *s = (t_node*)malloc(sizeof(t_node));
	if (s!=NULL)
	{
		copier_mat(s->montant, IDENTITE);
		copier_mat(s->descendant, IDENTITE);
		s->pt_objet = pt_objet;
		s->pt_pere = NULL;
		s->pt_fils = NULL;
		s->pt_frere = NULL;
	}
	return s;

}

void lierScene3d(t_scene3d pt_pere, t_scene3d pt_fils)
{
	assert(pt_pere != NULL);
	assert(pt_fils != NULL);

	pt_fils->pt_pere = pt_pere;
	pt_fils->pt_frere = pt_pere->pt_fils;
	pt_pere->pt_fils = pt_fils;
}

static void _aff_rec(t_surface *s, t_node* pt_node, double mat[4][4])
{
	if (pt_node==NULL)
		return;

	double tmp_mat[4][4];
	mult_mat(tmp_mat, mat, pt_node->descendant);

	transformationObjet3d(pt_node->pt_objet, tmp_mat);
	afficherObjet3d(s, pt_node->pt_objet);

	_aff_rec(s, pt_node->pt_fils, tmp_mat);

	_aff_rec(s, pt_node->pt_frere, mat);
}


void afficherScene3d(t_surface *s, t_scene3d scene)
{
	assert(scene!=NULL);
	assert(scene->pt_objet!=NULL);
	assert(scene->pt_objet->est_camera);

	double mat[4][4];
	t_node *pt_node = scene;
	t_node *prec_node = NULL;
	copier_mat(mat, IDENTITE);
	while (pt_node!=NULL)
	{
		double tmp_mat[4][4];
		mult_mat(tmp_mat, mat, pt_node->montant);
		copier_mat(mat, tmp_mat);
		prec_node = pt_node;
		pt_node = pt_node->pt_pere;
	}

	if (prec_node != NULL) // camera is not the root
		_aff_rec(s, prec_node, mat);
	else                   // camera IS the root
		_aff_rec(s, scene->pt_fils, mat); 
}

void libererScene3d(t_scene3d scene)
{
	if (scene==NULL)
		return;
	
	libererScene3d(scene->pt_fils);
	libererScene3d(scene->pt_frere);
	libererObjet3d(scene->pt_objet);
}
