#ifndef __DOT_PRODUCT_H
#define __DOT_PRODUCT_H

/**
 * \addtogroup dot_product
 * @{
 * 
 * @brief  Module de gestion des dot_product
 * 
 */

/*
 * Includes
 */
#include <sample.h>
#include <error.h>
#include <stdint.h>

/*
 * Structure definitions
 */

/* 
 * Function declarations
 */

/** 
 * Cette fonction effectue l'opération de produit case à case des échantillons
 * en fréquence produits grâce au floor par les échantillons obtenus grâce aux
 * residues. Les deux tableaux (\a fsamp et \a residues) sont de tailles 
 * \a nb_chan lignes par \a n2 colonnes. Plus précisemment, chaque "tableau"
 * est un pointeur vers un tableau de \a nb_chan pointeurs vers des tableaux
 * de \a n2 echantillons (de type sample_t). Attention, cette opération doit être 
 * faite en place, le tableau \a fsamp sert à la fois d'entrée et de sortie.
 * 
 * @param[in,out] fsamp est le tableau des échantillons fréquentiels; en entrée
 *                il contient les floors et en sortie le résultat (produit
 *                floors par residues)
 * @param[in] residues est le tableau des échantillons fréquentiels des residues
 * @param[in] nb_chan est le nombre de canaux
 * @param[in] n2 est le nombre d'échantillons par canal
 * 
 * @return VBS_SUCCESS en cas de succes ou VBS_FATAL en cas d'erreur
 */
status_t dot_product(sample_t **fsamp, sample_t **residues, uint32_t nb_chan,
                     uint32_t n2);

/* @} */
#endif				/* __DOT_PRODUCT_H */
