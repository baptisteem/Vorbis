#ifndef __FLOOR1_H
#define __FLOOR1_H

/** \addtogroup floor1
 * @{
 * 
 * @brief  Module de gestion des floor de type 1
 *
 * Comme illustré dans le fichier floor1_example.c , nous vous
 * conseillons d'étendre les types génériques floor_t et floor_data_t
 * pour contenir en plus respectivement l'ensemble des informations de
 * configurations spécifiques au floor de type 1, et l'ensemble des
 * zones de travail utiles aux floors de type 1 pour décoder un paquet audio
 * et faire la synthèse de la courbe gros grain.
 * 
 */

/*
 * Type definitions
 */

/*
 * Includes
 */
#include <stdint.h>
#include <vorbis.h>
#include <sample.h>

#include <floor.h>
#include <error.h>

/*
 * Structure definitions
 *
 */
#define FLOOR_TYPE1 1

/*
 * Function declarations
 */

/** 
 * Fonction qui lit les informations de configuration d'un floor dans l'en-tête
 * 3 (c.f. section Floor/décodage d'entête)
 * 
 * @param[in] stream est un pointeur sur le flux Vorbis en cours de décodage
 * @param[in] id est l'identifiant de la configuration de floor qui va être lue
 * @param[out] floor est un pointeur sur la structure allouée et initialisée
 * @param[in] data est la zone de travail du type de floor correspondant
 * 
 * @return VBS_SUCCESS en cas de succès, VBS_BADSTREAM en cas d'erreur de
 * lecture dans le flux, VBS_OUTOFMEMORY en cas d'erreur d'allocation,
 * + erreurs issues de vorbis_read_nbits 
 */
status_t floor_type1_hdr_decode(vorbis_stream_t * stream, uint8_t id,
			   floor_t ** floor, floor_data_t * data);

/** 
 * Fonction qui permet le décodage d'un floor dans le flux Vorbis.
 * (c.f. section Floor/décodage du paquet et suivantes)
 * 
 * @param[in] stream est un pointeur sur le flux Vorbis en coues de décodage
 * @param[in] floor_cfg est un pointeur sur le flux à utiliser pour la lecture
 * @param[out] v est le vecteur des données de sortie
 * @param[in] v_size est la taille du vecteur de sortie
 * 
 * @return VBS_SUCCESS en cas de succès, VBS_UNUSED si le floor n'est pas
 * utilisé pour ce paquet, VBS_BADSTREAM en cas d'erreur de lecture dans le flux
 * + erreurs issues de vorbis_read_nbits 
 */
status_t floor_type1_decode(vorbis_stream_t * stream, floor_t * floor_cfg,
		       sample_t * v, uint16_t v_size);

/** 
 * Fonction d'allocation d'une zone de travail spécifique à ce type de floor.
 * 
 * @param[out] pfl_data est un pointeur sur la zone nouvellement allouée 
 * @param[in] blocksize est le vecteur des tailles de bloc
 * 
 * @return VBS_SUCCESS en cas de succès ou VBS_OUTOFMEMORY en cas d'erreur
 * d'allocation
 */
status_t floor_type1_data_new(floor_data_t ** pfl_data, uint16_t * blocksize);

/** 
 * Fonction d'allocation des tampons internes de la zone de travail
 * A utiliser évidemment après avoir décodé l'ensemble des configurations de 
 * floor présentes dans l'entête 3 du flux.
 * 
 * @param[in] fl_data est le pointeur sur la zone de travail
 * 
 * @return VBS_SUCCESS en cas de succès ou VBS_OUTOFMEMORY en cas d'erreur
 * d'allocation
 */
status_t floor_type1_data_allocate(floor_data_t * fl_data);

/** 
 * Fonction de libération de la zone de travail. Cette fonction désalloue la
 * zone de travail et ses tampons interne. Elle doit être appelée après la
 * lecture de l'ensemble des description de floor type 1.
 * 
 * @param[in] fl_data est le pointeur sur la zone de travail
 */
void floor_type1_data_free(floor_data_t * fl_data);

/**
 * @}
 */
#endif				/* __FLOOR1_H */
