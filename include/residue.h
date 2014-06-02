#ifndef __RESIDUE_H
#define __RESIDUE_H

/** \addtogroup residue
 * @{
 * @brief  Module de gestion des residues
 * 
 * 
 */

/*
 * Type definitions
 */
typedef struct residues_setup residues_setup_t;
typedef struct residue residue_t;

/*
 * Includes
 */
#include <stdint.h>

#include <vorbis.h>
#include <sample.h>
#include <error.h>

/*
 * Structure definitions
 */
/**
 * @brief Structure contenant la configuration lue dans l'entête 3.
 * 
 * Cette structure rassemble l'ensemble des définitions de residues du flux
 * vorbis lu, dans le tableau \a residues. Ce tableau est de dimension \a
 * residue_count et est constitué de pointeur sur des structures de type \c
 * residue_t.
 */
struct residues_setup {
	uint8_t residue_count;	/**< Nombre de residues */
	residue_t **residues;	/**< Tableau des residues */
};

/**
 * @brief Structure de description d'un residue
 * 
 * Cette structure permet d'identifier les différents residues utilisés dans le
 * flux vorbis décodé.
 */
struct residue {
	int type; 
        /**< Type de residue. Dans la norme actuelle, il existe 3 types de residues
         * (0, 1 et 2), mais cette norme est extensible, et ce nombre n'est pas
         * figé. [0-2] */
	status_t (*decode) (vorbis_stream_t * stream, residue_t * resid, uint8_t ch,
		       uint16_t N2, sample_t ** v, uint8_t * do_not_decode);
        /**< Pointeur vers une fonction permettant la lecture du residue dans
         *  le paquet audio vorbis.  */

	void (*free) (residue_t * res); /**< Fonction de libération associée */
        /**< Pointeur vers une fonction permettant la libération de la structure
         * courante */
};

/* 
 * Function declarations
 */

/** 
 * Cette fonction alloue et initialise la structure de type \c residues_setup_t,
 * peuple le champ \a residues et la renvoie par le biais du pointeur de
 * pointeur \a pres. Cette fonction lit la partie de l'entête 3 relative aux
 * residues (c.f. la section Header3/residue de la documentation et la section
 * Residue/décodage).
 * NB: Cette fonction est largement similaire à la fonction \c floors_setup_init.
 * 
 * @param[in] stream est un pointeur sur le flux Vorbis en cours de lecture.
 * @param[out] pres est un pointeur permettant de renvoyer un pointeur sur la
 *        zone allouée et initialisée.
 * 
 * @return VBS_SUCCESS en cas de success, VBS_OUTOFMEMORY en cas d'erreur
 *        d'allocation ou VBS_BADSTREAM en cas d'erreur de lecture dans le flux.
 */
status_t residues_setup_init(vorbis_stream_t * stream, residues_setup_t ** pres);

/** 
 * Cette fonction libère la mémoire allouée pour le stockage des configurations
 * de residues.
 * 
 * @param[in] set est le pointeur sur la structure à libérer.
 */
void residues_free(residues_setup_t * set);

/** 
 * Cette fonction est responsable du décodage dans les paquets audio
 * residues. 
 * 
 * @param[in] stream est un pointeur sur le flux Vorbis en cours de lecture.
 * @param[in] residue est un pointeur sur la configuration de residue à
 * utiliser pour obtenir les coefficients permettant de calculer la courbe de
 * grain fin.
 * @param[in] ch donne le nombre de canaux à décoder (il peut être inférieur au
 * nombre réel de canaux).
 * @param[in] N2 donne la taille en nombre de coefficient de chaque canal (tous les
 * canaux ont la même taille).
 * @param[out] v correspond aux vecteurs qui permettront de conserver les residues
 * décodés dans le paquet.
 * @param[in] do_not_decode correspond au vecteur de fanions correspondant aux
 * canaux n'ayant pas de residue à décoder.
 * 
 * @return VBS_SUCCESS en cas de success, VBS_BADSTREAM en cas d'erreur de
 *  lecture dans le flux, ou VBS_EOP en cas de fin de paquet impromptu.
 */
status_t residue_decode(vorbis_stream_t * stream, residue_t * residue, int ch,
		   int64_t N2, sample_t ** v, uint8_t * do_not_decode);

#endif				/** @} __RESIDUE_H */
