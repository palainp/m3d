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
	libererPoint2d(t->abc[0]);
	libererPoint2d(t->abc[1]);
	libererPoint2d(t->abc[2]);
	free(t);
}

void _echangerPoint2d(t_triangle2d *t, size_t i, size_t j)
{
	int tmp = t->abc[i]->x;
	t->abc[i]->x = t->abc[j]->x;
	t->abc[j]->x = tmp;

	tmp = t->abc[i]->y;
	t->abc[i]->y = t->abc[j]->y;
	t->abc[j]->y = tmp;
}

void _trierSommets(t_triangle2d *t)
{
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
	assert(i<j);
	if (t->abc[i]->y < t->abc[j]->y)
	{
		double dx = ((double)(t->abc[i]->x - t->abc[j]->x))/(t->abc[i]->y - t->abc[j]->y);
		for(int y=t->abc[i]->y; y<=t->abc[j]->y; ++y)
		{
			int x = (int)(t->abc[i]->x+(y-t->abc[i]->y)*dx);

			if (t_surface_xmin(s)[y] > x) t_surface_xmin(s)[y] = x;
			if (t_surface_xmax(s)[y] < x) t_surface_xmax(s)[y] = x;

		}
	} else { // the > case won't occurs => == case
		t_surface_xmin(s)[t->abc[j]->y] = MIN(t->abc[i]->x, t->abc[j]->x);
		t_surface_xmax(s)[t->abc[j]->y] = MAX(t->abc[i]->x, t->abc[j]->x);
	}
}

void afficherTriangle2d(t_surface *s, t_triangle2d *t, Uint32 couleur)
{
	memset(t_surface_xmin(s), RX, RY);
	memset(t_surface_xmax(s), 0, RY);

	_trierSommets(t);
#if 0
	for (size_t i=0; i<3; ++i)
	{
		printf("(%lu) %d %d\t", i, t->abc[i]->x, t->abc[i]->y);
	}
	printf("\n");
#endif
	_majMinMax(s, t, 0, 1);
	_majMinMax(s, t, 0, 2);
	_majMinMax(s, t, 1, 2);

	for(int y=t->abc[0]->y; y<=t->abc[2]->y; ++y)
	{
		remplirLigneHorizontale(s, t_surface_xmin(s)[y], y, t_surface_xmax(s)[y]-t_surface_xmin(s)[y], couleur);
	}
}
