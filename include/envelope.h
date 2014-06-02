#ifndef __ENVELOPE_H
#define __ENVELOPE_H

/**
 * \addtogroup envelope
 * @{
 * 
 * @brief  Module de gestion des enveloppes
 * 
 * Une enveloppe correspond à la fonction de fenêtrage \a w d'un paquet
 * Vorbis, soit le filtre appliqué au résultat de l'iMDCT.
 * Sa forme dépend de la taille de la fenêtre courante (petite ou grande)
 * et des tailles des fenêtres précédente et suivante. Le calcul de cette
 * fonction, en fait les pentes à gauche et à droite, est détaillé dans la
 * section 3.4 du sujet. 
 */

/*
 * Type definitions
 */
typedef struct envelope envelope_t;

/*
 * Includes
 */
#include <stdint.h>

#include <sample.h>
#include <error.h>


/**
 * @brief Structure de travail pour la gestion des enveloppes.
 *
 * Elle contient en particulier trois tailles de fenêtre: celle de la
 * fenêtre courante dans le flux, mais aussi celle de la fenêtre précédente
 * et celle de la suivante. Ces tailles sont codées comme une valeur entière
 * valant 0 si une fenêtre est petite ou 1 si elle est grande. Il s'agit en
 * fait d'un indice dans le tableau \a blocksize des tailles de fenêtre.
 *
 * Le champ \a initialized est relatif au démarrage du flux. En effet, la
 * première moitié de la toute première fenêtre du flux doit être jetée.
 * \a initialized doit donc être égal à 0 (faux) au début du décodage, puis
 * valoir 1 (vrai) à partir de la fenêtre suivante.
 *
 * Cette structure pourra être étendue si vous souhaitez stocker
 * d'autres informations.
 */
struct envelope {
	int initialized;       /**< Première moitié de fenêtre initialisée? */

	uint16_t *blocksize;   /**< Tableau de 2 élements contenant les
	                            tailles de fenêtre du flux courant. */

	uint8_t prev_window;   /**< Taille de la fenêtre précédente (0 ou 1) */
	                            
	uint8_t curr_window;   /**< Taille de la fenêtre courante (0 ou 1) */   
	uint8_t next_window;   /**< Taille de la fenêtre suivante (0 ou 1) */
};


/* 
 * Function declarations
 */

/** 
 * Fonction permettant d'allouer et d'initialiser une enveloppe.
 * 
 * @param[in] blocksize est un tableau contenant les tailles des deux types
 *            de fenêtres.
 * 
 * @return un pointeur sur la structure nouvellement allouée et initialisée
 *         ou \c NULL en cas de problème d'allocation.
 */
envelope_t *envelope_init(uint16_t *blocksize);


/** 
 * Fonction qui permet de libérer la mémoire de la structure d'enveloppe.
 * 
 * @param[in] env est un pointeur sur la zone mémoire à libérer.
 */
void envelope_free(envelope_t *env);


/** 
 * Cette fonction permet de générer le filtre à appliquer au résultat de
 * l'iMDCT. 
 * Elle travaille à partir d'une configuration \a env préalablement remplie
 * (les indices des tailles devront être mises à jour pour chaque paquet).
 *
 * Cette fonction ne lit aucune donnée dans le flux Vorbis. La lecture de
 * toutes les informations nécessaires devra donc être faite avant.
 * 
 * @param[in] env est un pointeur sur la structure d'enveloppe courante
 * @param[out] filter est un vecteur qui reçoit les coefficient du filtre
 * 
 * @return VBS_SUCCESS en cas de succes ou VBS_FATAL en cas d'erreur.
 */
status_t envelope_prepare(envelope_t *env, sample_t *filter);


/** 
 * Cette fonction réalise l'opération de recouvrement entre la fenêtre
 * précédente et la fenêtre courante. Il s'agit simplement d'ajouter les
 * signaux temporels se recouvrant, pour générer un ensemble d'échantillons
 * temporels complètement reconstruits (voir section 3.4).
 * Le recouvrement s'étend de la moitié de la fenêtre précédente à la moitié
 * de la fenêtre courante.
 * Les données non utilisées d'une fenêtre (sa seconde moitié) devront être
 * conservées dans un cache, pour le recouvrement avec la fenêtre suivante.
 * 
 * En entrée de cette fonction:
 * - \a in contient les échantillons temporels de la fenêtre courante, décodés
 *   à l'aide de l'iMDCT et de l'enveloppe courante;
 * - le vecteur \a cache contient la seconde moitié de la fenêtre précédente.
 *
 * En sortie:
 * - \a out contient les échantillons complètement produits.
 * - \a cache contient la seconde moitié de la fenêtre \a in, non utilisée car
 *   elle n'était pas dans le recouvrement.
 *
 * Le champ \a initialized de la structure \c envelope_t est utilisé par cette
 * fonction pour savoir si on est en régime constant ou en initialisation.
 * Lors du traitement de la première fenêtre, le cache ne contient aucune
 * valeur et aucune donnée ne peut être produite. Par contre, le cache devra
 * être rempli pour le prochain appel.
 * 
 * @param[in] env est un pointeur sur la structure d'enveloppe courante
 * @param[in] in est le vecteur de données d'entrée
 * @param[in,out] cache est un vecteur de données cachées
 * @param[out] out est le vecteur de données de sortie
 * 
 * @return le nombre d'échantillons produits
 */
uint16_t envelope_overlap_add(envelope_t *env, sample_t *in,
                              sample_t *cache, sample_t *out);

/** @} */
#endif				/* __ENVELOPE_H */
