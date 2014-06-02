#ifndef __SAMPLE_H
#define __SAMPLE_H

/**
 * @file   sample.h
 * 
 * @brief  Fichier définissant quelques types, constantes utiles.
 * 
 */


/*
 * Type definitions
 */
#ifdef FIXED_POINT

/* to be defined */

#else

/**
 * @brief Type de base du domain temporel
 */
typedef double sample_t;
#endif

/*
 * Includes
 */
#ifdef FIXED_POINT

/* to be defined */

#else
#include <math.h>

#undef M_PI
#define M_PI		(3.14159265358979323846)	/* pi */

#undef M_PI_2
#define M_PI_2		(1.57079632679489661923)	/* pi/2 */
#endif

#endif				/* __SAMPLE_H */
