%{
#include "globals.h"
#include "parser_scene3d.tab.c"
%}

%option noyywrap

LF (\r\n)|\r|\n
SP [ \t]+

%%

scene { return T_SCENE; }
couleur { return T_COULEUR; }
objet { return T_OBJET; }
camera { return T_CAMERA; }
point { return T_POINT; }
vecteur { return T_VECTEUR; }
translation { return T_TRANSLATION; }
rotation { return T_ROTATION; }
homothetie { return T_HOMOTHETIE; }
liaison { return T_LIAISON; }

[0-9\-]+\.[0-9]+ {
	flottant = atof(yytext);
	return T_FLOTTANT;
}

0x[0-9A-Fa-f]+ {
	sscanf(yytext, "%X", &entier);
	return T_ENTIER;
}

[A-Za-z]+[A-Za-z_0-9]* {
	strncpy(texte, yytext, sizeof(texte));
	if (texte[sizeof(texte)-1]!='\0')
		fatal("id '%s' is too long", yytext);
	return T_ID;
}

\".+\" {
	/* chemin avec suppression des guillemets */
	strncpy(texte, &yytext[1], sizeof(texte));
	if (texte[sizeof(texte)-1]!='\0')
		fatal("path '%s' is too long", yytext);
	/* ici strlen(yytext)<sizeof(texte) */
	texte[strlen(yytext)-2]='\0';
	return T_CHEMIN;
}


#[^\r\n]* | /* commentaires */
{SP}+     {} /* espaces ignorés */

<<EOF>> { yyterminate(); return 0; }
. { fatal("unparseable character 0x%02x", *yytext); }

%%
