#ifndef __MAPPING_H
#define __MAPPING_H

/**
 * \addtogroup mapping
 * @{
 * 
 * @brief  Module de gestion des Mappings
 * 
 */

/*
 * Type definitions
 */
typedef struct mappings_setup mappings_setup_t;
typedef struct mapping mapping_t;


/*
 * Includes
 */
#include <stdint.h>

#include <vorbis.h>
#include <vorbis_packet.h>
#include <floor.h>
#include <residue.h>
#include <error.h>

/*
 * Structure definitions
 */

/**
 * @brief Structure regroupant l'ensemble des descripteurs de
 * mapping utilisés dans le flux Vorbis en cours de décodage.
 *
 * maps est un pointeur vers un tableau de mapping_count 
 * pointeurs vers les mappings.
 * Ceci permet l'extension du type mapping_t (en mapping_type0_t 
 * actuellement, mais on pourrait faire une extension différente
 * si d'autres types de mapping sont autorisés).
 */
struct mappings_setup {
        uint8_t mapping_count;  /**< Nombre de mappings */
        mapping_t **maps;       /**< Tableau des pointeurs vers les mapping */
};


#define MAPPING_TYPE0 0


/**
 * @brief Structure générique décrivant le mapping, indépendemment de son type.
 *
 * Même si la norme Vorbis ne compte qu'un type de mapping à l'heure
 * actuelle (le type 0), la norme reste extensible de ce point de vue, et
 * la représentation doit donc respecter cette possibilité. 
 * On y retrouve donc un type (\a type), qui doit actuellement valoir 
 * MAPPING_TYPE0, et un identifiant (\a id) correspondant
 * à l'indice où ce mapping est référencé dans la configuration mappings_setup.
 *
 * Les deux autres champs sont des pointeurs sur les fonctions spécifiques du 
 * type du mapping:
 * - \a decode pour décoder le paquet courant (au regard des informations du 
 *   mapping courant). Pour plus de détails, voir les commentaires de 
 *   \a mapping_type0_decode (qui sera la fonction pointée par \a decode puisque 
 *   les mapping sont forcément de type 0).
 * - \a free libère la mémoire de la structure de mapping pointée par
     son argument \a map.
 */
struct mapping {
	int type;		/**< Type de mapping */
	uint8_t id;             /**< Identifiant de ce mapping */

	status_t (*decode) (vorbis_stream_t *stream, mapping_t *map,
			    vorbis_packet_t *data);
	void (*free) (mapping_t *map);
};


/* 
 * Function declarations
 */

/** 
 * Cette fonction effectue l'allocation de la structure de type 
 * mappings_setup_t et l'initialise en lisant les informations de 
 * configuration de l'entête 3 (voir la section 5.2.3.e du sujet).
 *
 * Cette fonction prend en argument un stream qui permettra les
 * opérations de lecture du paquet d'entête, et d'avoir accès aux 
 * descripteurs de floor et de residue (via 
 * stream->codec->floors_desc et stream->codec->residues_desc, qui
 * doivent donc avoir été préalablement initialisés).
 * 
 * @param[in] stream pointeur sur le flux Vorbis en cours de lecture.
 * @param[out] pmap adresse du pointeur sur la structure allouée et initialisée.
 * 
 * @return VBS_SUCCESS en cas de success, VBS_OUTOFMEMORY en cas d'erreur
 *         d'allocation ou VBS_BADSTREAM en cas d'erreur de lecture dans le flux
 */
status_t mappings_setup_init(vorbis_stream_t *stream, mappings_setup_t **pmap);


/** 
 * Cette fonction permet, à partir d'un mapping donné par
 * l'argument \a map, de décoder le flux Vorbis d'un paquet.
 *
 * Cette fonction est une fonction générique, indépendante du type de
 * mapping. Elle ne fait qu'appeler la bonne fonction de décodage,
 * en fonction du type de \a map. Pour les utilisateurs du module \c decode,
 * elle est une alternative à la fonction pointée par le champ map->decode.
 *
 * @param[in] stream pointeur sur le flux Vorbis en cours de lecture.
 * @param[in] map pointeur vers le mapping courant.
 * @param[in,out] data pointeur vers une structure qui contient les tampons de 
 *             décodage.
 * 
 * @return VBS_SUCCESS
 */
status_t mapping_decode(vorbis_stream_t *stream, mapping_t *map,
			vorbis_packet_t *data);


/** 
 * Cette fonction libère la mémoire allouée pour le stockage des
 * descripteurs de mappings.
 * 
 * @param[in] map est le pointeur sur la structure à libérer.
 */
void mappings_free(mappings_setup_t *map);


/* @} */
#endif				/* __MAPPING_H */
