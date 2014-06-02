#ifndef __ERROR_H
#define __ERROR_H

#include <stdlib.h>

/*
 * Error codes
 */

typedef enum status status_t;
typedef enum ogg_status ogg_status_t;

/* Codes liés au flux Vorbis */
enum status {
	VBS_EOS          =     1,
	VBS_SUCCESS      =     0,
	VBS_OUTOFMEMORY  =  -127,
	VBS_BADSTREAM    =  -128,
	VBS_EOP          =  -129,
	VBS_FATAL        =  -130,
	VBS_BADVERSION   =  -131,
	VBS_UNUSED       =  -132,
};

/* Codes liés au conteneur Ogg */
enum ogg_status {
	OGG_END             = 1,   /* Fin de (fichier, flux, page, ...): 
	                              pas un code d'erreur. */
	OGG_OK              = 0,   /* Ok! */
	OGG_ERR_CORRUPT     = -1,  /* Flux corrompu */
	OGG_ERR_MEM_ERROR   = -2,  /* Problème d'allocation */
	OGG_ERR_IO_ERROR    = -3,  /* Problème d'accès au fichier */
	OGG_ERR_UNEXP_EOF   = -4,  /* Fin de fichier non attendue */
	OGG_ERR_ILL_ARG     = -5,  /* Argument illégal */
	OGG_ERR_ILL_OP      = -6,  /* Opération illégale */
	OGG_ERR_UNIMPLEM    = -7,  /* Opération non implémtentée */
	OGG_ERR_DECODER     = -8,  /* Erreur au décodage */
	OGG_ERR_INTERNAL    = -9,  /* Erreur interne */
};

/* 
 * Function declarations
 */


#endif				/* __VORBIS_PACKET_H */
