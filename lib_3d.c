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
	return definirPoint3d(p->xyzt[0], p->xyzt[1], p->xyzt[2]);
}

static void _copierPoint3d(t_point3d* dest, t_point3d *src)
{
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

t_triangle3d* definirTriangle3d(t_point3d* a, t_point3d* b, t_point3d* c)
{
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
	libererPoint3d(t->abc[0]);
	libererPoint3d(t->abc[1]);
	libererPoint3d(t->abc[2]);
	free(t);
}

void afficherTriangle3d(t_surface *s, t_triangle3d *t, Uint32 couleur)
{
	t_triangle2d *t2d = definirTriangle2d(\
		definirPoint2d((int)(t->abc[0]->xyzt[0]+t_surface_x(s)/2), (int)(-t->abc[0]->xyzt[1]+t_surface_y(s)/2)), \
		definirPoint2d((int)(t->abc[1]->xyzt[0]+t_surface_x(s)/2), (int)(-t->abc[1]->xyzt[1]+t_surface_y(s)/2)), \
		definirPoint2d((int)(t->abc[2]->xyzt[0]+t_surface_x(s)/2), (int)(-t->abc[2]->xyzt[1]+t_surface_y(s)/2))
		);
	afficherCoordonneesTriangle2d(t2d);
	afficherTriangle2d(s, t2d, couleur);
	libererTriangle2d(t2d);
}

void translationTriangle3d(t_triangle3d *t, t_vecteur3d* v)
{
	double mat[4][4] = {\
		{1, 0, 0, v->xyzt[0]}, \
		{0, 1, 0, v->xyzt[1]}, \
		{0, 0, 1, v->xyzt[2]}, \
		{0, 0, 0, 1}\
	};
	transformationTriangle3d(t, mat);
}

void rotationTriangle3d(t_triangle3d *t, t_point3d *c, double x, double y, double z)
{
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
#if 0
	transformationTriangle3d(t, tra1);
	transformationTriangle3d(t, rotX);
	transformationTriangle3d(t, rotY);
	transformationTriangle3d(t, rotZ);
	transformationTriangle3d(t, tra2);
#else
	double tmp[4][4], tmp2[4][4];
	mult_mat(tmp, rotX, tra1);
	mult_mat(tmp2, rotY, tmp);
	mult_mat(tmp, rotZ, tmp2);
	mult_mat(tmp2, tra2, tmp);
	transformationTriangle3d(t, tmp2);
#endif
}

void homothetieTriangle3d(t_triangle3d *t, double x, double y, double z)
{
	double mat[4][4] = {\
		{x, 0, 0, 0}, \
		{0, y, 0, 0}, \
		{0, 0, z, 0}, \
		{0, 0, 0, 1}\
	};
	transformationTriangle3d(t, mat);
}

void transformationTriangle3d(t_triangle3d *t, double mat[4][4])
{
	t_point3d p;
	for (size_t i=0; i<3; ++i)
	{
		_copierPoint3d(&p, t->abc[i]);
		mult_vecteur(t->abc[i], mat, &p);
	}
}
