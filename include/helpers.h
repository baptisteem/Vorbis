#ifndef __HELPERS_H
#define __HELPERS_H

/**
 * \addtogroup   helpers
 * @{
 *
 * @brief Boîte à outils de notre décodeur. Elle contient des
 * fonctions utilisées transversalement par plusieurs modules.
 * 
 */

/*
 * Includes
 */
#include <stdint.h>

#include <sample.h>


/*
 * Function declarations
 */

/** 
 * Fonction retournant la position (commençant à 1) du bit de poids 
 * fort de l'entier passé en paramètre.  Ceci correspond aussi au 
 * nombre de bits nécessaires au codage binaire de la valeur val.
 *
 * @param[in] val est un entier quelconque
 * 
 * @return la position du bit de poids fort de l'entier passé en
 * paramètre, 0 si val est négatif ou nul.
 */
uint32_t ilog(int32_t val);

/** 
 * Fonction renvoyant le plus grand entier r tel que r à la puissance
 * b est inférieur ou égal à a. Cette fonction est mentionnée dans la section
 * 5.2.3.a du sujet.
 * 
 * @param[in] a est l'entier a
 * @param[in] b est l'entier b
 * 
 * @return la valeur de r
 */
uint32_t lookup1_values(uint32_t a, uint32_t b);

/** 
 * Fonction effectuant la conversion d'un nombre réel, depuis sa
 * représentation entière dans le flux Vorbis vers un format flottant
 * compris par la machine.  L'algorithme est donné dans le sujet, dans
 * la partie décrivant les "codebook" de la section "Header 3"
 * (section 5.2.3.a).
 * 
 * @param[in] packed est le réel à convertir
 * 
 * @return le réel au format de la machine 
 */
sample_t float32_unpack(uint32_t packed);

#endif				/* __HELPERS_H */

/** @} */
