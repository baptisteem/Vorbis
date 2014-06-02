#ifndef __VORBIS_PACKET_H
#define __VORBIS_PACKET_H

/**
 * @addtogroup vorbis_packet
 * @{
 * 
 * @brief  Module de gestion des paquets Vorbis.
 *
 */

/*
 * Type definitions
 */
typedef struct vorbis_packet vorbis_packet_t;

/*
 * Includes
 */
#include <vorbis.h>
#include <sample.h>
#include <envelope.h>
#include <error.h>

/*
 * Structure definitions
 */
/**
 * @brief Structure de travail pour le décodage des paquets Vorbis.
 *
 * Cette structure contient plusieurs types d'informations:
 * - générales, mises à jour au fur et à mesure du décodage: \a nb_chan,
 *   \a pcm_offset
 * - spécifiques au paquet courant (en cours de décodage): la taille de
 *   fenêtre \a N du paquet, et des buffers de travail \a spectral,
 *   \a dec_residues, \a residues, \a temporal ainsi que \a no_residue et
 *   \a do_not_decode.
 *   Ces espaces mémoires sont partagés pour éviter de les allouer puis
 *   les désallouer à chaque fois qu'un nouveau paquet est décodé.
 *   Ils sont alloués une seule fois lors de l'initialisation de la
 *   structure, avec une taille n_max correspondant à la plus grande des
 *   tailles de fenêtre (taille de la "grande fenêtre").
 */
struct vorbis_packet {

/* Champs communs a tous les paquets */
	uint8_t nb_chan;          /**< Nombre de canaux */

/* Champs spécifiques au paquet courant */
	uint16_t size;           /**< Taille du paquet courant (i.e. de la
	                              fenêtre correspondante) */
	
/* Buffers de la représentation spectrale */
	sample_t **spectral;     /**< Buffers de la représentation spectrale:
	                              les courbes gros grain après le décodage
	                              des \a floor, puis les courbes spectrales
	                              issues du produit scalaire.
	                              Taille: nombre_de_canaux x (n_max/2) */
	sample_t **dec_residues; /**< Buffers pour les residues décodés du
	                              paquet (dans l'ordre des submaps).
	                              Taille: nombre_de_canaux x (n_max/2) 
								  Attention: le vecteur pour le canal 0 
								  doit pouvoir contenir un residue de type 2.
								  Sa taille est donc de nb_chan*n/2. */
	sample_t **residues;     /**< Buffers des residues réordonnés après
	                              l'étape de découplage.
	                              Taille: nombre_de_canaux x (n_max/2) */
	
/* Buffers utiles au décodage (floor et surtout residues) */
	uint8_t *no_residue;    /**< Tableau des fanions d'absence de residue.
	                             Taille: nombre_de_canaux */
	uint8_t *do_not_decode; /**< Tableau des fanions de non décodage.
	                             Taille: nombre_de_canaux */
	
/* Buffers de la représentation temporelle */
	sample_t **temporal;    /**< Buffers de représentation temporelle.
	                             Taille: nombre_de_canaux x (n_max) */

/* Buffers des PCMs décodés */
	int16_t **pcm;         /**< Buffers de sortie des PCM décodés dans
                                     un paquet.
                                     Taille: nombre_de_canaux x (n_max) */
};


/* 
 * Function declarations
 */

/** 
 * Fonction qui permet d'allouer et d'initialiser la structure
 * de travail de type \c vorbis_packet_t.
 * 
 * @param[in] blocksize est un tableau contenant les tailles des deux types
 *            de fenêtres.
 * @param[in] nb_chan est le nombre de canaux présents dans le flux en cours
 *            de décodage.
 * 
 * @return un pointeur sur la structure fraichement allouée et initialisée,
 *         \c NULL en cas d'erreur.
 */
vorbis_packet_t *vorbis_packet_init(uint16_t *blocksize, uint8_t nb_chan);


/** 
 * Fonction qui permet de libérer la structure de travail de type
 * \c vorbis_packet_t
 * 
 * @param[in] pkt est un pointeur sur la structure de travail à libérer, 
 *            il doit bien sûr être non nul.
 */
void vorbis_packet_free(vorbis_packet_t *pkt);


/** 
 * Fonction qui effectue le décodage d'un paquet audio du flux Vorbis,
 * comme décrit en section 5.3. Elle regroupe différentes opérations:
 * - lecture du \a mode;
 * - décodage des informations dans le flux (floors, residues, ...) puis 
 *   couplage inverse des residues. Ces étapes sont en fait déléguées au
 *   \a mapping associé au paquet courant.
 * - génération du signal, du \a dot_product à la production des
 *   échantillons PCM.
 * 
 * Remarques importantes:
 * -  La chaîne de traitement (décodage) fonctionne sur des \c sample_t, or
 *    les PCM produits devront être de type \c int16_t. Une conversion est
 *    donc nécessaire pour produire les échantillons, mais vous devez faire
 *    attention à la saturation.
 * - La fonction \c vorbis_packet_decode est à l'image des fonctions 
 *    \c vorbis_header1_decode, \c vorbis_header2_decode et
 *    \c vorbis_header3_decode : elle ne s'occupe que de la lecture d'un
 *    paquet, et ne prend pas en charge le changement de paquet qui doit être
 *    géré au niveau supérieur.
 *
 * @param[in] stream est le pointeur sur le flux Vorbis en cours de décodage
 * @param[in] pkt est le pointeur la structure de travail de décodage de
 *            paquets Vorbis
 * @param[out] nb_samp renvoie le nombre d'échantillons produits lors de la
 *            lecture de ce paquet
 * 
 * @return VBS_SUCCESS dans le cas où le paquet a été décodé avec succès,
 *         VBS_BADSTREAM en cas d'erreur lors de la lecture dans le flux.
 */
status_t vorbis_packet_decode(vorbis_stream_t *stream, vorbis_packet_t *pkt,
                              uint16_t *nb_samp);

/**
 * @}
 */
#endif				/* __VORBIS_PACKET_H */
