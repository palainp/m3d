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

static void _construction_rec(t_surface *s, t_node* pt_node, double mat[4][4], t_objet3d *o)
{
	if (pt_node==NULL)
		return;

	assert(pt_node->pt_objet != NULL);

	double tmp_mat[4][4];
	mult_mat(tmp_mat, mat, pt_node->descendant);

	if (!(pt_node->pt_objet->est_camera))
	{
		t_objet3d *tmp_obj = copierObjet3d(pt_node->pt_objet);
		transformationObjet3d(tmp_obj, tmp_mat);
		concatenerObjet3d(o, tmp_obj);
		libererObjet3d(tmp_obj);
	}

	_construction_rec(s, pt_node->pt_fils, tmp_mat, o);
	_construction_rec(s, pt_node->pt_frere, mat, o);
}


void afficherScene3d(t_surface *s, t_scene3d scene)
{
	assert(s!=NULL);
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

	t_objet3d* o = objet_vide();

	assert(prec_node!=NULL);
	_construction_rec(s, prec_node, mat, o);

	trierObjet3d(o);
	afficherObjet3d(s, o);
	libererObjet3d(o);
}

void libererScene3d(t_scene3d scene)
{
	if (scene==NULL)
		return;
	
	libererScene3d(scene->pt_fils);
	libererScene3d(scene->pt_frere);
	libererObjet3d(scene->pt_objet);
	free(scene);
}

void translationScene3d(t_scene3d scn, t_vecteur3d* v)
{
	assert(scn!=NULL);
	assert(v!=NULL);

	double translation[4][4] = {\
		{1, 0, 0, v->xyzt[0]}, \
		{0, 1, 0, v->xyzt[1]}, \
		{0, 0, 1, v->xyzt[2]}, \
		{0, 0, 0, 1}\
	};
	double translation_inverse[4][4] = {\
		{1, 0, 0, -v->xyzt[0]}, \
		{0, 1, 0, -v->xyzt[1]}, \
		{0, 0, 1, -v->xyzt[2]}, \
		{0, 0, 0, 1}\
	};

	transformationScene3d(scn, translation_inverse, translation);
}

void rotationScene3d(t_scene3d scn, t_point3d *c, double x, double y, double z)
{
	assert(scn!=NULL);
	assert(c!=NULL);

	double tra1[4][4] = {\
		{1, 0, 0, -c->xyzt[0]}, \
		{0, 1, 0, -c->xyzt[1]}, \
		{0, 0, 1, -c->xyzt[2]}, \
		{0, 0, 0, 1}\
	};
	double tra2[4][4] = {\
		{1, 0, 0, c->xyzt[0]}, \
		{0, 1, 0, c->xyzt[1]}, \
		{0, 0, 1, c->xyzt[2]}, \
		{0, 0, 0, 1}\
	};
	double radX = x*M_PI/180, radY = y*M_PI/180, radZ = z*M_PI/180;
	double rotX[4][4] = {\
		{1, 0,     0,      0}, \
		{0, cos(radX), -sin(radX), 0}, \
		{0, sin(radX), cos(radX),  0}, \
		{0, 0,     0,      1}\
	};
	double rotY[4][4] = {\
		{cos(radY), 0, -sin(radY), 0}, \
		{0,     1, 0,      0}, \
		{sin(radY), 0, cos(radY),  0}, \
		{0,     0, 0,      1}\
	};
	double rotZ[4][4] = {\
		{cos(radZ), -sin(radZ), 0, 0}, \
		{sin(radZ), cos(radZ),  0, 0}, \
		{0,     0,      1, 0}, \
		{0,     0,      0, 1}\
	};

	double rotXinv[4][4] = {\
		{1, 0,     0,      0}, \
		{0, cos(-radX), -sin(-radX), 0}, \
		{0, sin(-radX), cos(-radX),  0}, \
		{0, 0,     0,      1}\
	};
	double rotYinv[4][4] = {\
		{cos(-radY), 0, -sin(-radY), 0}, \
		{0,     1, 0,      0}, \
		{sin(-radY), 0, cos(-radY),  0}, \
		{0,     0, 0,      1}\
	};
	double rotZinv[4][4] = {\
		{cos(-radZ), -sin(-radZ), 0, 0}, \
		{sin(-radZ), cos(-radZ),  0, 0}, \
		{0,     0,      1, 0}, \
		{0,     0,      0, 1}\
	};

	double tmp[4][4], tmp2[4][4];
	double rotation[4][4], rotation_inverse[4][4];
	mult_mat(tmp, rotX, tra1);
	mult_mat(tmp2, rotY, tmp);
	mult_mat(tmp, rotZ, tmp2);
	mult_mat(rotation, tra2, tmp);

	mult_mat(tmp, rotZinv, tra1);
	mult_mat(tmp2, rotYinv, tmp);
	mult_mat(tmp, rotXinv, tmp2);
	mult_mat(rotation_inverse, tra2, tmp);

	transformationScene3d(scn, rotation_inverse, rotation);
}

void transformationScene3d(t_scene3d scn, double montant[4][4], double descendant[4][4])
{
	assert(scn!=NULL);
	double tmp_mat[4][4];
	mult_mat(tmp_mat, montant, scn->montant);
	copier_mat(scn->montant, tmp_mat);
	mult_mat(tmp_mat, scn->descendant, descendant);
	copier_mat(scn->descendant, tmp_mat);
}
