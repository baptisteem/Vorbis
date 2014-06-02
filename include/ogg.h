#ifndef __OGG_H
#define __OGG_H

/**
 * \addtogroup ogg_core
 * @{
 */

/**
 * @file  ogg.h
 * 
 * @brief Structures de flux Ogg physique et logique.
 * 
 * Ces structures sont communes à tous les modules Ogg, et utilisées
 * depuis l'extérieur.
 */

/*
 * Includes
 */
#include <stdint.h>


/*
 * Type definitions
 */
typedef enum ogg_codec ogg_codec_t;
typedef struct ogg_physical_stream ogg_physical_stream_t;
typedef struct ogg_logical_stream ogg_logical_stream_t;

/**
 * @brief Identifiant de format/codec d'un flux logique Ogg.
 */
enum ogg_codec
{
	OGG_UNKNOWN,
	OGG_VORBIS
};


/*
 * Structure definitions
 */

/**
 * @brief Structure représentant un flux physique Ogg.
 *
 * Cette structure contient une liste chaînée des flux logiques présents
 * dans un flux physique.
 * Comme expliqué au chapitre 4 du sujet, les premières pages des flux
 * logiques (1 seule page par flux, celle qui a le flag BOS) sont les unes
 * à la suite des autres au début du fichier Ogg.
 * Il faut donc toutes les lire pour générer la liste. 
 * 
 */
struct ogg_physical_stream
{
	uint32_t nb_streams;          /**< Nombre de flux logiques contenus
	                                   dans le flux physique. */
	ogg_logical_stream_t *first;  /**< Pointeur vers le premier des flux
	                                   logiques (tête de liste chaînée). */
};


/**
 * @brief Structure représentant un flux logique Ogg.
 * 
 */
struct ogg_logical_stream
{
	uint32_t stream_id;          /**< Identifiant du flux logique au sein
	                                   du flux physique. */
	ogg_codec_t codec;           /**< Format/codec du flux logique */
	
	ogg_logical_stream_t *next;  /**< Pointeur vers le prochain élément de
	                                  la liste des flux logiques, NULL si 
	                                  c'est le dernier. */
};

/** @} */

#endif				/* __OGG_H */
