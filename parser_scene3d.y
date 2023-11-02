%{
#include <stdarg.h>
#include <assert.h>
#include "globals.h"
#include "lib_3d.h"
#include "lib_objet3d.h"
#include "lib_scene3d.h"

#define YYDEBUG 1
extern FILE* yyin;

uint32_t entier;
double flottant;
char texte[256];

enum {
	POINT=0,
	VECTEUR,
	OBJET,
	CAMERA,
	SCENE,
	SCENECAMERA,
};

char* type_str[] = {"point", "vecteur", "objet", "camera", "scene_objet", "scene_camera"};

struct lst {
	char* nom;
	size_t type;
	union {
		t_point3d   p;
		t_vecteur3d v;
		t_objet3d*  o;
		t_node*     s;
	} u;
	struct lst* suiv;
};
static struct lst* l;

static void insere_element(void* p, size_t type, const char* nom);
static struct lst* cherche_element(const char* nom);
static void liberer_elements(void);

%}

%token T_COULEUR
%token T_FLOTTANT
%token T_ID
%token T_CHEMIN
%token T_SCENE
%token T_OBJET
%token T_CAMERA
%token T_POINT
%token T_VECTEUR
%token T_TRANSLATION
%token T_ROTATION
%token T_HOMOTHETIE
%token T_LIAISON

%union {
	char* str;
	Uint32 coul;
	double coord;
}

%type <str> sid
%type <str> schemin
%type <coul> couleur
%type <coord> coordonnee

%start elements

%%

elements
	: /* vide */
	| elements nouv_scene
	| elements nouv_objet
	| elements nouv_point
	| elements nouv_vecteur
	| elements operation
	;

sid
	: T_ID { $$ = strdup(texte); }
	;

schemin
	: T_CHEMIN { $$ = strdup(texte); }
	;

couleur
	: T_COULEUR { $$ = (Uint32)entier; }
	;

coordonnee
	: T_FLOTTANT { $$ = flottant; }
	;

nouv_scene
	: T_SCENE sid sid {
		char* nom = $2;
		char* obj = $3;

		struct lst* o = cherche_element(obj);
		if (o==NULL || (o->type!=OBJET && o->type!=CAMERA)) fatal("'%s' is not an object or a camera");

		t_node* s = creerScene3d(o->u.o);

		if (o->type==OBJET)
			insere_element(s, SCENE, nom);
		else /* o->type==CAMERA */
			insere_element(s, SCENECAMERA, nom);

		free(nom);
		free(obj);
	}
	;

nouv_objet
	: T_OBJET sid schemin couleur couleur {
		char* nom = $2;
		char* chemin = $3;
		insere_element(fichierObjet3d(chemin, $4, $5), OBJET, nom);
		free(nom);
		free(chemin);
	}
	| T_CAMERA sid {
		char* nom = $2;
		insere_element(camera(), CAMERA, nom);
		free(nom);
	}
	;

nouv_point
	: T_POINT sid coordonnee coordonnee coordonnee {
		char* nom = $2;

		t_point3d *p = (t_point3d*)malloc(sizeof(t_point3d));
		p->xyzt[0] = $3;
		p->xyzt[1] = $4;
		p->xyzt[2] = $5;
		p->xyzt[3] = 1;
		insere_element(p, POINT, nom);

		free(nom);
	}
	;

nouv_vecteur
	: T_VECTEUR sid coordonnee coordonnee coordonnee {
		char* nom = $2;

		t_vecteur3d *p = (t_vecteur3d*)malloc(sizeof(t_vecteur3d));
		p->xyzt[0] = $3;
		p->xyzt[1] = $4;
		p->xyzt[2] = $5;
		p->xyzt[3] = 0;
		insere_element(p, VECTEUR, nom);

		free(nom);
	}
	;

operation
	: T_TRANSLATION sid sid {
		char* obj = $2;
		char* vect = $3;

		struct lst* o = cherche_element(obj);
		if (o==NULL || (o->type!=OBJET && o->type!=CAMERA)) fatal("'%s' is not an object or a camera", obj);

		struct lst* v = cherche_element(vect);
		if (v==NULL || v->type!=VECTEUR) fatal("'%s' is not a vector", vect);

		translationObjet3d(o->u.o, &o->u.v);

		free(obj);
		free(vect);
	}
	| T_ROTATION sid sid coordonnee coordonnee coordonnee {
		char* obj = $2;
		char* ctre = $3;

		struct lst* o = cherche_element(obj);
		if (o==NULL || (o->type!=OBJET && o->type!=CAMERA)) fatal("'%s' is not an object or a camera", obj);

		struct lst* c = cherche_element(ctre);
		if (c==NULL || c->type!=POINT) fatal("'%s' is not a point", ctre);

		rotationObjet3d(o->u.o, &o->u.p, $4, $5, $6);

		free(obj);
		free(ctre);
	}
	| T_HOMOTHETIE sid coordonnee coordonnee coordonnee {
		char* obj = $2;

		struct lst* o = cherche_element(obj);
		if (o==NULL || (o->type!=OBJET && o->type!=CAMERA)) fatal("'%s' is not an object or a camera", obj);

		homothetieObjet3d(o->u.o, $3, $4, $5);

		free(obj);
	}
	| T_LIAISON sid sid {
		char* pere = $2;
		char* enfant = $3;

		struct lst* p = cherche_element(pere);
		if (p==NULL || (p->type!=SCENE && p->type!=SCENECAMERA)) fatal("'%s' is not a scene", pere);

		struct lst* e = cherche_element(enfant);
		if (e==NULL || (e->type!=SCENE && e->type!=SCENECAMERA)) fatal("'%s' is not a scene", enfant);

		lierScene3d(p->u.s, e->u.s);

		free(pere);
		free(enfant);
	}
	;
		
%%

    
static void insere_element(void* p, size_t type, const char* nom)
{
	struct lst* tmp = l;

	while (!(tmp==NULL || strcmp(nom, tmp->nom)==0))
		tmp = tmp->suiv;

	if (tmp!=NULL) fatal("%s '%s' is already defined as %s", type_str[type], nom, type_str[tmp->type]);

	/* si tmp==NULL l'élément n'est pas déjà présent dans la liste on insère en tête */
	tmp = (struct lst*) malloc(sizeof(struct lst));
	tmp->nom = strdup(nom);
	tmp->type = type;
	switch (type)
	{
		case POINT:
			memcpy(&tmp->u.p, p, sizeof(t_point3d));
			break;
		case VECTEUR:
			memcpy(&tmp->u.v, p, sizeof(t_vecteur3d));
			break;
		case OBJET:
		case CAMERA:
			tmp->u.o = (t_objet3d*)p;
			break;
		case SCENE:
		case SCENECAMERA:
			tmp->u.s = (t_node*)p;
			break;
		default:
			fatal("type '%s' is not defined", type);
	}
	tmp->suiv = l;
	l = tmp;
}

static struct lst* cherche_element(const char* nom)
{
	struct lst* tmp = l;
	while (!(tmp==NULL || strcmp(nom, tmp->nom)==0))
		tmp = tmp->suiv;

	return tmp;
}

static void liberer_elements(void)
{
	struct lst* tmp = l;

	while (tmp!=NULL)
	{
		struct lst* sauv = tmp->suiv;
		free(tmp->nom);

		/* objet et scene sont utilisés par le programme appelant => pas de libération ici */

		free(tmp);
		tmp = sauv;
	}
}

void fatal(const char* s, ...)
{
	va_list ap;
	va_start(ap, s);
	fprintf(stderr, "Error: ");
	vfprintf(stderr, s, ap);
	fprintf(stderr, "\n");
	va_end(ap);
	exit(EXIT_FAILURE);
}

void parse(FILE* fn)
{
	yyin= fn;
	while(feof(yyin)==0)
	{
		yyparse();
	}
}

t_scene3d charger_scene3d(const char* fn)
{
	assert(fn!=NULL);

	FILE* fd = fopen(fn, "r");
	assert(fd!=NULL);

	yydebug = 0;
	parse(fd);
	fclose(fd);

	t_scene3d scn;
	scn.n_cam = 0;
	struct lst* tmp = l;
	while (tmp!=NULL)
	{
		if (tmp->type == SCENECAMERA)
		{
			scn.cam[scn.n_cam] = tmp->u.s;
			scn.n_cam++;
		}
		tmp = tmp->suiv;
	}

	return scn;
}
