#include <assert.h>
#include "lib_2d.h"

t_point2d* definirPoint2d(int x, int y)
{
	t_point2d *p = (t_point2d*)malloc(sizeof(t_point2d));
	if (p!=NULL)
	{
		p->x = x;
		p->y = y;
	}
	return p;
}

void libererPoint2d(t_point2d *p)
{
	free(p);
}

t_triangle2d* definirTriangle2d(t_point2d* a, t_point2d* b, t_point2d* c)
{
	assert(a!=NULL);
	assert(b!=NULL);
	assert(c!=NULL);

	t_triangle2d *t = (t_triangle2d*)malloc(sizeof(t_triangle2d));
	if (t!=NULL)
	{
		t->abc[0] = a;
		t->abc[1] = b;
		t->abc[2] = c;
	}
	return t;
}

void libererTriangle2d(t_triangle2d *t)
{
	assert(t!=NULL);

	libererPoint2d(t->abc[0]);
	libererPoint2d(t->abc[1]);
	libererPoint2d(t->abc[2]);
	free(t);
}

void _echangerPoint2d(t_triangle2d *t, size_t i, size_t j)
{
	assert(t!=NULL);

	t_point2d* tmp = t->abc[i];
	t->abc[i] = t->abc[j];
	t->abc[j] = tmp;
}

void _trierSommets(t_triangle2d *t)
{
	assert(t!=NULL);

	if (t->abc[0]->y > t->abc[1]->y)
	{
		_echangerPoint2d(t, 0, 1);
	}
	if (t->abc[1]->y > t->abc[2]->y)
	{
		_echangerPoint2d(t, 1, 2);
	}
	if (t->abc[0]->y > t->abc[1]->y)
	{
		_echangerPoint2d(t, 0, 1);
	}
}

void _majMinMax(t_surface *s, t_triangle2d *t, size_t i, size_t j)
{
	assert(s!=NULL);
	assert(t!=NULL);
	assert(i<j);

	if (t->abc[i]->y < t->abc[j]->y)
	{
		double dx = ((double)(t->abc[i]->x - t->abc[j]->x))/(t->abc[i]->y - t->abc[j]->y);
		for(int y=MAX(0, t->abc[i]->y); y<=MIN(t_surface_y(s)-1, t->abc[j]->y); ++y)
		{
			int x = (int)(t->abc[i]->x+(y-t->abc[i]->y)*dx);

			if (t_surface_xmin(s)[y] > x && x>=0) t_surface_xmin(s)[y] = x;
			if (t_surface_xmax(s)[y] < x && x<t_surface_x(s)) t_surface_xmax(s)[y] = x;

		}
	} else  // the > case won't occurs => == case
		if (t->abc[i]->y>=0 && t->abc[i]->y<t_surface_y(s)) {
			int xmin = MIN(t->abc[i]->x, t->abc[j]->x);
			int xmax = MAX(t->abc[i]->x, t->abc[j]->x);
			int y = t->abc[i]->y;

			if (t_surface_xmin(s)[y] > xmin && xmin>=0) t_surface_xmin(s)[t->abc[j]->y] = xmin;
			if (t_surface_xmax(s)[y] < xmax && xmax<t_surface_x(s)) t_surface_xmax(s)[t->abc[j]->y] = xmax;
	}
}

void afficherTriangle2d(t_surface *s, t_triangle2d *t, Uint32 couleur)
{
	assert(s!=NULL);
	assert(t!=NULL);

	_trierSommets(t);

	_majMinMax(s, t, 0, 1);
	_majMinMax(s, t, 0, 2);
	_majMinMax(s, t, 1, 2);

	for(int y=MAX(0, t->abc[0]->y); y<=MIN(t_surface_y(s)-1, t->abc[2]->y); ++y)
	{
		remplirLigneHorizontale(s, t_surface_xmin(s)[y], y, t_surface_xmax(s)[y]-t_surface_xmin(s)[y], couleur);
		t_surface_xmax(s)[y] = 0;
		t_surface_xmin(s)[y] = t_surface_x(s) - 1;
	}
}
