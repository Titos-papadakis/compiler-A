#ifndef __ALPHA_UTILITIES_H__
/**
 * @brief Utilities for
 * alpha compiler such
 * as more descriptive types
 */
#define __ALPHA_UTILITIES_H__

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <math.h>

/* Yakk-Bison declarations */
extern FILE *yyin;                  /* Input to Yakk */
extern char *yytext;                /* Yakk text */
extern int yylineno;                /* Line number */
int yylex();                        /* Yakk function */

typedef unsigned char Uint8;            /* Unsigned 1 byte integer */
typedef signed char Sint8;              /* Signed 1 byte integer */
typedef unsigned short int Uint16;      /* Unsigned 2 byte integer */
typedef signed short int Sint16;        /* Signed 2 byte integer */
typedef unsigned int Uint32;            /* Unsigned 4 byte integer */ 
typedef signed int Sint32;              /* Signed 4 byte integer */

/* Null type overload */
#define ALPHA_NIL NULL

/* Failure macro */
#define ALPHA_FAIL 1

/* Success macro */
#define ALPHA_SUCCESS 0

/* Debug mode via flag */
extern Uint8 debugMode;

/* Global debug macro ,activate and deactivate this for more info */
#define ALPHA_CONSOLE

/* Global debug macro ,activate and deactivate this for more info */
#ifdef ALPHA_CONSOLE
    /* Error print function */
    #define AERR(...) fprintf(stderr, "AERR: " __VA_ARGS__)
    /* Debug print function */
    #define ADEB(...) fprintf(stdout, "ADEB: " __VA_ARGS__)
#endif
#ifndef ALPHA_CONSOLE
    #define AERR(...) /* Disabled */
    #define ADEB(...) /* Disabled */
#endif

#endif
