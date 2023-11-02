#ifndef _GLOBALS_H_
#define _GLOBALS_H_

#include <string.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>

extern void fatal(const char* s, ...);
extern int yylex(void);
#define yyerror(s) fatal(s)

extern uint32_t entier;
extern double flottant;
extern char texte[256];

#endif
