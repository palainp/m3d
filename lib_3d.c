#include <assert.h>
#include <string.h>
#include "lib_3d.h"
#include "lib_mat.h"

t_point3d* definirPoint3d(double x, double y, double z)
{
	t_point3d *p = (t_point3d*)malloc(sizeof(t_point3d));
	if (p!=NULL)
	{
		p->xyzt[0] = x;
		p->xyzt[1] = y;
		p->xyzt[2] = z;
		p->xyzt[3] = 1;
	}
	return p;
}

void libererPoint3d(t_point3d *p)
{
	free(p);
}

t_point3d* copierPoint3d(t_point3d* p)
{
	assert(p!=NULL);

	return definirPoint3d(p->xyzt[0], p->xyzt[1], p->xyzt[2]);
}

static void _copierPoint3d(t_point3d* dest, t_point3d *src)
{
	assert(dest!=NULL);
	assert(src!=NULL);

	memcpy(dest->xyzt, src->xyzt, 4*sizeof(double));
}

t_vecteur3d* definirVecteur3d(double x, double y, double z)
{
	t_vecteur3d *p = (t_vecteur3d*)malloc(sizeof(t_vecteur3d));
	if (p!=NULL)
	{
		p->xyzt[0] = x;
		p->xyzt[1] = y;
		p->xyzt[2] = z;
		p->xyzt[3] = 0;
	}
	return p;
}

void libererVecteur3d(t_vecteur3d *p)
{
	free(p);
}

void transformationPoint3d(t_point3d *p, double mat[4][4], t_point3d* tmp)
{
	_copierPoint3d(tmp, p);
	mult_vecteur(p, mat, tmp);
}

t_triangle3d* definirTriangle3d(t_point3d* a, t_point3d* b, t_point3d* c)
{
	assert(a!=NULL);
	assert(b!=NULL);
	assert(c!=NULL);

	t_triangle3d *t = (t_triangle3d*)malloc(sizeof(t_triangle3d));
	if (t!=NULL)
	{
		t->abc[0] = copierPoint3d(a);
		t->abc[1] = copierPoint3d(b);
		t->abc[2] = copierPoint3d(c);
	}
	return t;
}

void libererTriangle3d(t_triangle3d *t)
{
	assert(t!=NULL);

	libererPoint3d(t->abc[0]);
	libererPoint3d(t->abc[1]);
	libererPoint3d(t->abc[2]);
	free(t);
}

t_triangle3d* copierTriangle3d(t_triangle3d* t)
{
	assert(t!=NULL);
	return definirTriangle3d(t->abc[0], t->abc[1], t->abc[2]);
}

void afficherTriangle3d(t_surface *s, t_triangle3d *t, Uint32 couleur)
{
	assert(s!=NULL);
	assert(t!=NULL);

	t_point2d p[3];
	p[0] = definirPoint2d((int)(t->abc[0]->xyzt[0]+t_surface_x(s)/2), (int)(-t->abc[0]->xyzt[1]+t_surface_y(s)/2));
	p[1] = definirPoint2d((int)(t->abc[1]->xyzt[0]+t_surface_x(s)/2), (int)(-t->abc[1]->xyzt[1]+t_surface_y(s)/2));
	p[2] = definirPoint2d((int)(t->abc[2]->xyzt[0]+t_surface_x(s)/2), (int)(-t->abc[2]->xyzt[1]+t_surface_y(s)/2));

	t_triangle2d t2d = definirTriangle2d(&p[0], &p[1], &p[2]);
	afficherTriangle2d(s, &t2d, couleur);
}

void transformationTriangle3d(t_triangle3d *t, double mat[4][4])
{
	assert(t!=NULL);

	t_point3d p;
	for (size_t i=0; i<3; ++i)
	{
		transformationPoint3d(t->abc[i], mat, &p);
	}
}
