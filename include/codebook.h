#ifndef __CODEBOOK_H
#define __CODEBOOK_H

/** \addtogroup codebook
 * @{
 * 
 * @brief  En-tête commune des modules de gestion des codebooks : \c codebook et
 * \c codebook_read.
 *
 * Ces deux modules vous faciliteront le déverminage. Il est encouragé
 * de fusionner ces deux modules dans votre version finale.
 */


/* Types */
typedef struct codebook codebook_t;
typedef struct codebook_setup codebook_setup_t;

#include <stdint.h>
#include <vorbis.h>
#include <error.h>
#include <sample.h>

/* Stuctures */

/**
 * @brief Structure de description d'un codebook
 * 
 */
struct codebook {
	uint32_t index;           /**< Rang */
};

/**
 * @brief La structure globale du module Codebook qui comporte l'ensemble des
 * codebooks du flux.
 */
struct codebook_setup {
	uint8_t nb_cb;		/**< Nombre de codebooks */
	codebook_t **codebooks;	/**< tableau de pointeurs des codebooks */
};

/* Functions */

/** 
 * Fonction du module \c codebook qui récupère les dictionnaires stockés dans le flux Vorbis \a
 * stream (cf module \c vorbis_io), pour les stocker dans la structure des dictionnaires \a pset.  
 * La fonction stocke chacun des dictionnaires ainsi
 * construit dans le tableau contenu dans la structure (en utilise les
 * algorithmes présentés dans la section Headers3 Codebook de la
 * documentation). Les arbres de Huffman qui représentent les dictionnaires sont
 * stockés en utilisant la structure d'arbre \c codebook_tree_t
 * 
 * @param[in] stream est un pointeur sur le flux Vorbis en cours de décodage
 * @param[out] pset est un pointeur qui renvoie l'adresse de la structure
 *      nouvellement allouée et contenant la configuration lue
 * 
 * @return VBS_SUCCESS en cas de succes, VBS_BADSTREAM en cas d'erreur de
 * lecture dans le flux ou VBS_OUTOFMEMORY en cas d'erreur d'allocation mémoire
 */
status_t codebook_setup_init(vorbis_stream_t * stream,
			     codebook_setup_t ** pset);

/** 
 * Fonction du module \c codebook qui permet de libérer la structure pointée par \a cb_desc. En sortie
 * de fonction, le descripteur des dictionnaires doit être complètement libéré,
 * c'est à dire que le descripteur est libéré ainsi que l'ensemble des
 * structures allouées dans le descripteur.
 * 
 * @param cb_desc est le pointeur sur la structure à libérer
 */
void codebooks_free(codebook_setup_t * cb_desc);

/** 
 * Fonction du module \c codebook qui permet d'utiliser les dictionnaires sur le flux Vorbis.  \a
 * Elle travaille avec le dictionnaire \a book, et
 * lit le flux \c stream jusqu'à trouver un mot de code valide pour ce
 * dictionnaire. Une fois trouvé, elle renseigne \a scalar avec l'entrée dans l'arbre de huffman
 * correspondante au mot de code.  
 * 
 * @param[in] stream est le flux en cours de décodage
 * @param[in] book est le pointeur sur le codebook à utiliser
 * @param[out] scalar est un pointeur permettant de renvoyer le pointeur sur
 *        l'entrée résultante
 * 
 * @return le nombre de bits lus dans le flux Vorbis en cas de succès,
 * VBS_BADSTREAM en cas d'erreur de lecture dans le flux.
 */
uint32_t codebook_translate_scalar(vorbis_stream_t * stream, codebook_t * book,
								   uint32_t *scalar);

/** 
 * Fonction du module \c codebook qui permet d'utiliser les dictionnaires sur le flux Vorbis.  \a
 * Elle travaille avec le dictionnaire \a book, et
 * lit le flux \c stream jusqu'à trouver un mot de code valide pour ce
 * dictionnaire. Une fois trouvé, elle renvoie dans \a vector le vecteur des données 
 * quantifiées (VQ) correspondant au mot de code. Attention l'allocation et la
 * libération de ce vecteur sont à la charge du module codebook.  
 * 
 * @param[in] stream est le flux en cours de décodage
 * @param[in] book est le pointeur sur le codebook à utiliser
 * @param[out] sz est un pointeur permettant de renvoyer la taille du vecteur
 * @param[out] vector est un pointeur sur permettant de renvoyer un pointeur sur 
 *         le vecteur de donnée 
 * 
 * @return le nombre de bits lus dans le flux Vorbis en cas de succès,
 * VBS_BADSTREAM en cas d'erreur de lecture dans le flux.
 */
uint32_t codebook_translate_vq(vorbis_stream_t * stream, codebook_t * book,
							   int *sz, sample_t **vector);


/** 
 * Fonction du module \c codebook qui permet de récuperer la dimension des
 * vecteurs de quantification d'un codebook \a book
 * 
 * @param[in] book est le pointeur sur le codebook à utiliser
 * 
 * @return la dimension des vecteurs de quantification du codebook.
 */
uint32_t codebook_get_dimension(codebook_t * book);


#endif				/** @} __CODEBOOK_H */
