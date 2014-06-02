#ifndef __MODE_H
#define __MODE_H

/**
 * \addtogroup mode
 * @{
 * 
 * @brief  Module de gestion des Modes
 * 
 */

/*
 * Type definitions
 */
typedef struct window_modes_setup window_modes_setup_t;
typedef struct window_mode window_mode_t;

/*
 * Includes
 */
#include <stdint.h>

#include <vorbis.h>
#include <mapping.h>
#include <error.h>

/*
 * Structure definitions
 */

/**
 * @brief Structure regroupant l'ensemble des descripteurs de
 * mode utilisés dans le flux Vorbis en cours de décodage.
 * 
 * Cette structure contient un tableau de modes \a modes de
 * \a mode_count cases. Cette taille n'étant connue qu'à la lecture
 * du flux, le tableau est alloué dynamiquement.
 *
 * Le champ \a mode_code_nbbits représente le nombre de bits nécessaires
 * pour coder (dans les paquets audio) l'identifiant du mode utilisé
 * (cf. \a ilog). On parle bien ici de l'indice maximal du mode dans le
 * tableau \a modes, il ne s'agit donc pas de \a mode_count, mais de
 * \a mode_count - 1.
 */
struct window_modes_setup {
	uint8_t mode_count;        /**< Nombre de modes */
	window_mode_t *modes;      /**< Tableau des modes */
	uint8_t mode_code_nbbits;  /**< Nombre de bits nécessaire pour coder
	                                l'indice d'un mode */
};

/**
 * @brief Structure décrivant le mode de la fenêtre courante.
 *
 * Le champ \a blockflag définit la taille de la fenêtre: 0 pour une petite
 * fenêtre, 1 pour une grande.
 * Le champ \a mapping donne le mapping du mode courant.
 *
 * Les deux champs \a window_type et \a transform_type sont réservés pour
 * des extensions futures de Vorbis. Ils doivent donc toujours valoir zéro,
 * sinon le descripteur de mode n'est pas valide.
 */
struct window_mode {
	uint8_t blockflag;       /**< Taille de la fenêtre courante, 0 ou 1 */
	uint16_t window_type;    /**< Extension, doit valoir 0 */
	uint16_t transform_type; /**< Extension, doit valoir 0 */
	mapping_t *mapping;      /**< Mapping associé à ce mode */
};

/* 
 * Function declarations
 */

 
/** 
 * Fonction de lecture des modes dans l'entête 3.
 * 
 * Cette fonction est en charge de l'allocation et de l'initialisation de
 * la structure window_modes_setup_t regroupant les différents modes
 * (voir la section 5.2.3.f du sujet).
 * 
 * Remarque: \a stream->codec->modes_desc ne contient pas d'information à 
 * l'appel de cette fonction. La structure \a *pset sera donc initialisée
 * en lisant dans \a stream->io_desc.
 * Par contre \a stream->codec->mappings_desc
 * doit avoir été préalablement initialisée: cette fonction ne construit 
 * pas les mapping associés aux modes, elle se contente de pointer vers
 * les mapping appropriés pré-existants dans \a stream->codec->mappings_desc.
 * 
 * @param[in] stream pointeur sur le flux Vorbis en cours de lecture.
 * @param[out] pset adresse du pointeur sur la structure allouée et initialisée.
 * 
 * @return VBS_SUCCESS en cas de success, VBS_OUTOFMEMORY en cas d'erreur
 *         d'allocation ou VBS_BADSTREAM en cas d'erreur de lecture dans le flux
 */
status_t window_modes_setup_init(vorbis_stream_t *stream,
                                 window_modes_setup_t **pset);

/** 
 * Cette fonction libère la mémoire allouée pour le stockage des
 * descripteurs de modes.
 * 
 * @param[in] set est le pointeur sur la structure à libérer.
 */
void window_modes_free(window_modes_setup_t *set);

/* @} */
#endif				/* __MODE_H */

