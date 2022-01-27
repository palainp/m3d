#include <assert.h>
#include <stdlib.h>
#include <stdio.h>
#include "lib_objet3d.h"
#include "lib_mat.h"

t_maillon* creer_maillon(t_triangle3d* t, Uint32 c)
{
	t_maillon *m = (t_maillon*)malloc(sizeof(t_maillon));
	if (m!=NULL)
	{
		m->t = t;
		m->c = c;
		m->pt_suiv = NULL;
	}
	return m;
}

void liberer_maillon(t_maillon *m)
{
	assert(m!=NULL);
	libererTriangle3d(m->t);
	free(m);
}

void inserer_tete(t_liste* l, t_maillon* m)
{
	assert(l!=NULL);
	assert(m!=NULL);
	m->pt_suiv = *l;
	*l = m;
}

void inserer_apres(t_maillon* x, t_maillon* m)
{
	assert(x!=NULL);
	assert(m!=NULL);
	m->pt_suiv = x->pt_suiv;
	x->pt_suiv = m;
}

t_objet3d* objet_vide()
{
	t_objet3d* o = (t_objet3d*)malloc(sizeof(t_objet3d));
	if (o!=NULL)
	{
		o->est_trie = true;
		o->est_camera = false;
		o->faces = NULL;
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

	t_point3d* t[8];
	t[0] = definirPoint3d(xm, ym, zm);
	t[1] = definirPoint3d(xM, ym, zm);
	t[2] = definirPoint3d(xM, yM, zm);
	t[3] = definirPoint3d(xm, yM, zm);
	t[4] = definirPoint3d(xm, ym, zM);
	t[5] = definirPoint3d(xM, ym, zM);
	t[6] = definirPoint3d(xM, yM, zM);
	t[7] = definirPoint3d(xm, yM, zM);

	// face arrière
	inserer_tete(&(o->faces), creer_maillon(\
		definirTriangle3d(t[0], t[1], t[2]), ROUGEC));
	inserer_tete(&(o->faces), creer_maillon(\
		definirTriangle3d(t[0], t[2], t[3]), BLANC));
	// face avant
	inserer_tete(&(o->faces), creer_maillon(\
		definirTriangle3d(t[4], t[5], t[6]), VERTC));
	inserer_tete(&(o->faces), creer_maillon(\
		definirTriangle3d(t[4], t[6], t[7]), BLEUC));
	// face gauche
	inserer_tete(&(o->faces), creer_maillon(\
		definirTriangle3d(t[0], t[4], t[7]), JAUNEF));
	inserer_tete(&(o->faces), creer_maillon(\
		definirTriangle3d(t[0], t[7], t[3]), ROSEF));
	// face doite
	inserer_tete(&(o->faces), creer_maillon(\
		definirTriangle3d(t[1], t[5], t[6]), MAUVE));
	inserer_tete(&(o->faces), creer_maillon(\
		definirTriangle3d(t[1], t[6], t[2]), ORANGE));
	// face haut
	inserer_tete(&(o->faces), creer_maillon(\
		definirTriangle3d(t[3], t[2], t[6]), MARRON1));
	inserer_tete(&(o->faces), creer_maillon(\
		definirTriangle3d(t[3], t[6], t[7]), MARRON3));
	// face bas
	inserer_tete(&(o->faces), creer_maillon(\
		definirTriangle3d(t[0], t[1], t[5]), PALEC));
	inserer_tete(&(o->faces), creer_maillon(\
		definirTriangle3d(t[0], t[5], t[4]), GRISC));

	libererPoint3d(t[0]);
	libererPoint3d(t[1]);
	libererPoint3d(t[2]);
	libererPoint3d(t[3]);
	libererPoint3d(t[4]);
	libererPoint3d(t[5]);
	libererPoint3d(t[6]);
	libererPoint3d(t[7]);

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

	t_point3d** t;
	size_t l=0, max=100;
	t = (t_point3d**)malloc(max*sizeof(t_point3d*));
	assert(t!=NULL);

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

			if (l>=max)
			{
				max = (size_t)((double)max*1.5);
				t = realloc(t, max*sizeof(t_point3d*));
				assert(t!=NULL);
			}

//			printf("lecture point (%lu/%lu): %f %f %f\n", l, max, x, y, z);

			t[l] = definirPoint3d(x,y,z);
			l++;
		} else if (ligne[0]=='f') {
			int i,j,k;
			int in,jn,kn;
			int it,jt,kt;

			size_t z=1;
			while (!(z==sizeof(ligne) || ligne[z]!=' ' || ligne[z]!='\t')) ++z;
			assert(z!=sizeof(ligne));
			assert(ligne[z]!='\n');

			if (!(sscanf(&(ligne[z]), "%d// %d// %d//\n", &i, &j, &k)==3 || \
				sscanf(&(ligne[z]), "%d %d %d\n", &i, &j, &k)==3 || \
				sscanf(&(ligne[z]), "%d//%d %d//%d %d//%d\n", &i, &in, &j, &jn, &k, &kn)==6 || \
				sscanf(&(ligne[z]), "%d/%d/%d %d/%d/%d %d/%d/%d\n", &i, &it, &in, &j, &jt, &jn, &k, &kt, &kn)==9))
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
			t_triangle3d *tr = definirTriangle3d(t[i], t[j], t[k]);
			inserer_tete(&(o->faces), creer_maillon(\
					tr, couleur_entre(rand()%100, c1, c2)));
		}
	}
	for(size_t i=0; i<l; ++i)
	{
		libererPoint3d(t[i]);
	}
	free(t);
	return o;
}

void libererObjet3d(t_objet3d *o)
{
	assert(o!=NULL);

	t_maillon *m = o->faces;
	while (m!=NULL)
	{
		t_maillon *s = m->pt_suiv;
		liberer_maillon(m);
		m = s;
	}
	free(o);
}

void afficherObjet3d(t_surface* s, t_objet3d* o)
{
	assert(s!=NULL);
	assert(o!=NULL);

	if (!o->est_trie)
		trierObjet3d(o);

	t_maillon *m = o->faces;
	while (m!=NULL)
	{
		afficherTriangle3d(s, m->t, m->c);
		m = m->pt_suiv;
	}
}

t_objet3d* copierObjet3d(t_objet3d* o)
{
	assert(o!=NULL);

	t_objet3d* cpy = objet_vide();
	assert(cpy!=NULL);

	t_maillon *m = o->faces;
	while (m!=NULL)
	{
		inserer_tete(&(cpy->faces), creer_maillon(\
				copierTriangle3d(m->t), m->c));
		m = m->pt_suiv;
	}

	cpy->est_trie = false;
	return cpy;
}

void concatenerObjet3d(t_objet3d *a, t_objet3d *b)
{
	assert(a!=NULL);
	assert(b!=NULL);

	t_maillon *m = b->faces;
	while (m!=NULL)
	{
		t_maillon *suiv_m = m->pt_suiv;
		inserer_tete(&(a->faces), m);
		m = suiv_m;
	}

	a->est_trie = false;
	b->faces = NULL;
}

#if 0
static size_t nbFacesObjet3d(t_objet3d* o)
{
	assert(o!=NULL);

	t_maillon *m = o->faces;
	size_t n=0;
	while (m!=NULL)
	{
		++n;
		m = m->pt_suiv;
	}
	return n;
}
#endif

#if 1
void decoupeListe(t_liste *a, t_liste *b) // O(n)
{
	t_maillon *rapide, *lent, *prec_lent;

	prec_lent = NULL;
	lent = rapide = *a;
	while (!(rapide==NULL || rapide->pt_suiv==NULL))
	{
		rapide = rapide->pt_suiv;
		rapide = rapide->pt_suiv;
		prec_lent = lent;
		lent = lent->pt_suiv;
	}
	if (prec_lent!=NULL)
	{
		prec_lent->pt_suiv = NULL;
	}
	*b = lent;
}

void fusionListe(t_liste *a, t_liste b)
{
	t_maillon *res, *ptr_fin, *ptr_a, *ptr_b;

	res = NULL;
	ptr_fin = NULL;
	ptr_a = *a;
	ptr_b = b;
	while (!(ptr_a==NULL || ptr_b==NULL))
	{
		if (zmoyen(ptr_a->t)<zmoyen(ptr_b->t))
		{
			t_maillon *tmp = ptr_a->pt_suiv;
			if (ptr_fin!=NULL)
			{
				inserer_apres(ptr_fin, ptr_a);
			} else {
				inserer_tete(&res, ptr_a);
			}
			ptr_fin = ptr_a;
			ptr_a = tmp;
		} else {
			t_maillon *tmp = ptr_b->pt_suiv;
			if (ptr_fin!=NULL)
			{
				inserer_apres(ptr_fin, ptr_b);
			} else {
				inserer_tete(&res, ptr_b);
			}
			ptr_fin = ptr_b;
			ptr_b = tmp;
		}
	}
	while (!(ptr_a==NULL))
	{
		t_maillon *tmp = ptr_a->pt_suiv;
		if (ptr_fin!=NULL)
		{
			inserer_apres(ptr_fin, ptr_a);
		} else {
			inserer_tete(&res, ptr_a);
		}
		ptr_fin = ptr_a;
		ptr_a = tmp;
	}
	while (!(ptr_b==NULL))
	{
		t_maillon *tmp = ptr_b->pt_suiv;
		if (ptr_fin!=NULL)
		{
			inserer_apres(ptr_fin, ptr_b);
		} else {
			inserer_tete(&res, ptr_b);
		}
		ptr_fin = ptr_b;
		ptr_b = tmp;
	}

	*a = res;
}

void trierListe_rec(t_liste *a)
{
	if (*a==NULL || (*a)->pt_suiv==NULL)
	{
		return;
	} else {
		t_liste b;
		decoupeListe(a, &b);
		trierListe_rec(a);
		trierListe_rec(&b);
		fusionListe(a, b);
	}
}

void trierObjet3d(t_objet3d *o)			// O(n*log(n))
{
	assert(o!=NULL);

	if (o->est_trie)
		return;

	trierListe_rec(&(o->faces));
	o->est_trie = true;
}
#else
void trierObjet3d(t_objet3d *o)			// O(n^2)
{
	assert(o!=NULL);

	if (o->est_trie)
		return;

	t_maillon *i = o->faces;
	t_maillon *prec_i = NULL;
	while (i!=NULL)						// O(n)
	{
		t_maillon *j = i->pt_suiv;
		t_maillon *prec_j = i;
		t_maillon *m = i;
		t_maillon *prec_m = prec_i;
		double zm = zmoyen(m->t);
		while (j!=NULL)					// O(n)
		{
			double zj = zmoyen(j->t);
			if (zj<zm)
			{
				m = j;
				prec_m = prec_j;
				zm = zj;
			}
			prec_j = j;
			j = j->pt_suiv;
		}

		if (m != i)
		{
			// extraction de m de la liste non triée
			// si m!=i => prec_m!=NULL
			prec_m->pt_suiv = m->pt_suiv;
	
			// insertion de m à la fin de la liste triée
			if (prec_i==NULL)
			{
				inserer_tete(&(o->faces), m);
			} else {
				inserer_apres(prec_i, m);
			}
			prec_i = m;
		} else {
			prec_i = i;
			i = i->pt_suiv;
		}
	}
	o->est_trie = true;
}
#endif

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

	t_maillon *m = o->faces;
	while (m!=NULL)
	{
		transformationTriangle3d(m->t, mat);
		m = m->pt_suiv;
	}
}
