#include <assert.h>
#include <stdlib.h>
#include <stdio.h>
#include "lib_objet3d.h"
#include "lib_mat.h"

static t_face definirFace(size_t a, size_t b, size_t c, Uint32 couleur)
{
	t_face f;
	f.t[0] = a;
	f.t[1] = b;
	f.t[2] = c;
	f.c = couleur;
	return f;
}

t_objet3d* objet_vide()
{
	t_objet3d* o = (t_objet3d*)malloc(sizeof(t_objet3d));
	if (o!=NULL)
	{
		o->est_trie = true;
		o->est_camera = false;
		o->p = NULL;
		o->np = 0;
		o->faces = NULL;
		o->nfaces = 0;
	}
	return o;
}

t_objet3d* camera()
{
	t_objet3d *o = objet_vide();
	assert(o!=NULL);
	o->est_camera = true;
	return o;
}

t_objet3d* cube(int l)
{
	assert(l>0);
	double xm = -l/2, xM = l/2;
	double ym = -l/2, yM = l/2;
	double zm = -l/2, zM = l/2;
	t_objet3d *o = objet_vide();
	assert(o!=NULL);

	o->np = 8;
	o->p = (t_point3d*)malloc(o->np*sizeof(t_point3d));
	assert(o->p!=NULL);
	o->p[0] = definirPoint3d(xm, ym, zm);
	o->p[1] = definirPoint3d(xM, ym, zm);
	o->p[2] = definirPoint3d(xM, yM, zm);
	o->p[3] = definirPoint3d(xm, yM, zm);
	o->p[4] = definirPoint3d(xm, ym, zM);
	o->p[5] = definirPoint3d(xM, ym, zM);
	o->p[6] = definirPoint3d(xM, yM, zM);
	o->p[7] = definirPoint3d(xm, yM, zM);

	o->nfaces = 12;
	o->faces = (t_face*)malloc(o->nfaces*sizeof(t_face));
	assert(o->faces!=NULL);

	// face arrière
	o->faces[0] = definirFace(0, 1, 2, ROUGEC);
	o->faces[1] = definirFace(0, 2, 3, BLANC);
	// face avant
	o->faces[2] = definirFace(4, 5, 6, VERTC);
	o->faces[3] = definirFace(4, 6, 7, BLEUC);
	// face gauche
	o->faces[4] = definirFace(0, 4, 7, JAUNEF);
	o->faces[5] = definirFace(0, 7, 3, ROSEF);
	// face doite
	o->faces[6] = definirFace(1, 5, 6, MAUVE);
	o->faces[7] = definirFace(1, 6, 2, ORANGE);
	// face haut
	o->faces[8] = definirFace(3, 2, 6, MARRON1);
	o->faces[9] = definirFace(3, 6, 7, MARRON3);
	// face bas
	o->faces[10] = definirFace(0, 1, 5, PALEC);
	o->faces[11] = definirFace(0, 5, 4, GRISC);

	o->est_trie = false;
	return o;
}

t_objet3d* fichierObjet3d(const char* fn, Uint32 c1, Uint32 c2)
{
	assert(fn!=NULL);

	FILE* fd = fopen(fn, "r");
	assert(fd!=NULL);

	t_objet3d *o = objet_vide();
	assert(o!=NULL);

	size_t l=0;
	o->np = 100;
	o->p = (t_point3d*)malloc(o->np*sizeof(t_point3d));
	assert(o->p!=NULL);

	size_t lf=0;
	o->nfaces = 100;
	o->faces = (t_face*)malloc(o->nfaces*sizeof(t_face));
	assert(o->faces!=NULL);

	size_t nl = 0;
	char ligne[256];
	while (fgets(ligne, sizeof(ligne), fd)!=NULL)
	{

		++nl;
		if (ligne[0]=='v' && (ligne[1]==' ' || ligne[1]=='\t')) // définition d'un point
		{
			
			float x,y,z;
			size_t j=1;
			while (!(j==sizeof(ligne) || ligne[j]!=' ' || ligne[j]!='\t')) ++j;
			assert(j!=sizeof(ligne));
			assert(ligne[j]!='\n');

			int r = sscanf(&(ligne[j]), "%f %f %f\n", &x, &y, &z);
			if (r==EOF)
			{
				fprintf(stderr, "%s:%d Syntax Error %s:%lu\n", __FILE__, __LINE__, fn, nl);
			}

			if (l>=o->np)
			{
				o->np = (size_t)((double)o->np*1.5);
				o->p = realloc(o->p, o->np*sizeof(t_point3d));
				assert(o->p!=NULL);
			}

//			printf("lecture point (%lu/%lu): %f %f %f\n", l, max, x, y, z);

			o->p[l] = definirPoint3d(x,y,z);
			l++;
		} else if (ligne[0]=='f') {
			size_t i,j,k;
			size_t in,jn,kn;
			size_t it,jt,kt;

			size_t z=1;
			while (!(z==sizeof(ligne) || ligne[z]!=' ' || ligne[z]!='\t')) ++z;
			assert(z!=sizeof(ligne));
			assert(ligne[z]!='\n');

			if (!(sscanf(&(ligne[z]), "%lu// %lu// %lu//\n", &i, &j, &k)==3 || \
				sscanf(&(ligne[z]), "%lu %lu %lu\n", &i, &j, &k)==3 || \
				sscanf(&(ligne[z]), "%lu//%lu %lu//%lu %lu//%lu\n", &i, &in, &j, &jn, &k, &kn)==6 || \
				sscanf(&(ligne[z]), "%lu/%lu/%lu %lu/%lu/%lu %lu/%lu/%lu\n", &i, &it, &in, &j, &jt, &jn, &k, &kt, &kn)==9))
			{
				fprintf(stderr, "%s:%d Syntax Error %s:%lu\n", __FILE__, __LINE__, fn, nl);
			}

			--i;
			--j;
			--k;
//			printf("lecture face : %d %d %d\n", i, j, k);

			assert((size_t)i<l);
			assert((size_t)j<l);
			assert((size_t)k<l);

			if (lf>=o->nfaces)
			{
				o->nfaces = (size_t)((double)o->nfaces*1.5);
				o->faces = realloc(o->faces, o->nfaces*sizeof(t_face));
				assert(o->faces!=NULL);
			}

			o->faces[lf] = definirFace(i, j, k, couleur_entre(rand()%100, c1, c2));
			lf++;
		}
	}

	// réduction des tableaux à leur taille réelle
	o->np = l;
	o->p = realloc(o->p, o->np*sizeof(t_point3d));
	assert(o->p!=NULL);
	o->nfaces = lf;
	o->faces = realloc(o->faces, o->nfaces*sizeof(t_face));
	assert(o->faces!=NULL);

	return o;
}

void libererObjet3d(t_objet3d *o)
{
	assert(o!=NULL);

	free(o->p);
	free(o->faces);
	free(o);
}

void afficherObjet3d(t_surface* s, t_objet3d* o)
{
	assert(s!=NULL);
	assert(o!=NULL);

	for(size_t i=0; i<o->nfaces; ++i)
	{
		t_triangle3d t = definirTriangle3d(&(o->p[o->faces[i].t[0]]), &(o->p[o->faces[i].t[1]]), &(o->p[o->faces[i].t[2]]));
		afficherTriangle3d(s, &t, o->faces[i].c);
	}
}

t_objet3d* copierObjet3d(t_objet3d* o)
{
	assert(o!=NULL);

	t_objet3d* cpy = objet_vide();
	assert(cpy!=NULL);

	cpy->est_trie = o->est_trie;
	cpy->est_camera = o->est_camera;
	cpy->np = o->np;
	cpy->nfaces = o->nfaces;

	cpy->p = (t_point3d*)malloc(cpy->np*sizeof(t_point3d));
	assert(cpy->p!=NULL);
	cpy->faces = (t_face*)malloc(cpy->nfaces*sizeof(t_face));
	assert(cpy->faces!=NULL);

	for (size_t i=0; i<cpy->np; ++i)
	{
		cpy->p[i] = o->p[i];
	}

	for (size_t i=0; i<cpy->nfaces; ++i)
	{
		cpy->faces[i] = o->faces[i];
	}

	return cpy;
}

void fusionnerObjet3d(t_objet3d *a, t_objet3d *b)
{
	assert(a!=NULL);
	assert(b!=NULL);

	a->p = realloc(a->p, (a->np+b->np)*sizeof(t_point3d));
	assert(a->p!=NULL);
	a->faces = realloc(a->faces, (a->nfaces+b->nfaces)*sizeof(t_face));
	assert(a->faces!=NULL);

	for (size_t i=0; i<b->np; ++i)
	{
		a->p[a->np + i] = b->p[i];
	}

	for (size_t i=0; i<b->nfaces; ++i)
	{
		a->faces[a->nfaces + i] = definirFace(\
			a->np + b->faces[i].t[0], \
			a->np + b->faces[i].t[1], \
			a->np + b->faces[i].t[2], \
			b->faces[i].c);
	}

	a->np += b->np;
	a->nfaces += b->nfaces;
	a->est_trie = false;

	libererObjet3d(b);
}

// Function to swap the the position of two elements
static void echange(t_face *a, t_face *b) {
	t_face temp = *a;
	*a = *b;
	*b = temp;
}

static double val(t_point3d tp[], t_face* f)
{
	return (tp[f->t[0]].xyzt[2]+tp[f->t[0]].xyzt[2]+tp[f->t[0]].xyzt[2])/3;
}
static void tamiser(t_objet3d *o, int n, int i) {
	// cherche le max entre la racine et ses deux enfants
	int max = i;
	int fg = 2*i+1;
	int fd = 2*i+2;

	if (fg<n && val(o->p, &o->faces[fg])>val(o->p, &o->faces[max]))
		max = fg;
	if (fd<n && val(o->p, &o->faces[fd])>val(o->p, &o->faces[max]))
		max = fd;

	// echange et continue si la racine n'est pas le max
	if (max!=i) {
		echange(&o->faces[i], &o->faces[max]);
		tamiser(o, n, max);
	}
}

void trierObjet3d(t_objet3d *o)			// Tri par tas : O(n*log(n))
{
	int n = (int)o->nfaces;
	// construction du tas
	for (int i=n/2-1; i>=0; --i)
		tamiser(o, n, i);

	// tri
	for (int i=n-1; i>=0; --i) {
		echange(&o->faces[0], &o->faces[i]);
		tamiser(o, i, 0);
	}
	o->est_trie = true;
}

void translationObjet3d(t_objet3d *o, t_vecteur3d* v)
{
	assert(o!=NULL);
	assert(v!=NULL);

	double mat[4][4] = {\
		{1, 0, 0, v->xyzt[0]}, \
		{0, 1, 0, v->xyzt[1]}, \
		{0, 0, 1, v->xyzt[2]}, \
		{0, 0, 0, 1}\
	};

	transformationObjet3d(o, mat);
}

void rotationObjet3d(t_objet3d *o, t_point3d *c, double x, double y, double z)
{
	assert(o!=NULL);
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

	double tmp[4][4], tmp2[4][4];
	mult_mat(tmp, rotX, tra1);
	mult_mat(tmp2, rotY, tmp);
	mult_mat(tmp, rotZ, tmp2);
	mult_mat(tmp2, tra2, tmp);

	o->est_trie = false;
	transformationObjet3d(o, tmp2);
}

void homothetieObjet3d(t_objet3d *o, double x, double y, double z)
{
	assert(o!=NULL);

	double mat[4][4] = {\
		{x, 0, 0, 0}, \
		{0, y, 0, 0}, \
		{0, 0, z, 0}, \
		{0, 0, 0, 1}\
	};
	transformationObjet3d(o, mat);
}

void transformationObjet3d(t_objet3d *o, double mat[4][4])
{
	assert(o!=NULL);

	t_point3d p;
	for (size_t i=0; i<o->np; ++i)
	{
		transformationPoint3d(&o->p[i], mat, &p);
	}
}
