#ifndef __OGG_PACKET_H
#define __OGG_PACKET_H

/**
 * \addtogroup ogg_packet
 * @{
 * 
 * @brief  Module de gestion des paquets Ogg.
 * 
 * Ce module permet la reconstitution des paquets d'un flux logique
 * à partir de ses pages. Il contient des fonctions utilisables par
 * le module ogg_core et d'autres par les modules Vorbis.
 *
 * Toutes les fonctions travaillent à partir d'un objet 
 * ogg_logical_stream, qui est en réalité du type étendu
 * internal_ogg_logical_stream 
 */                              

/**
 * @file   ogg_packet.h
 */

/*
 * Includes
 */

#include <stdint.h>
#include <ogg.h>
#include <error.h>



/* 
 * Function declarations
 */

/** 
 * @brief Fonction permettant de lyre des octets du paquet courant
 * d'un flux logique.
 * 
 * Le nombre d'octets lus par cette fonction est normalement égal à
 * \a nbytes, mais il peut être inférieur dans le cas où le paquet
 * ne contient plus assez d’octets. 
 * Demander plus d’octets qu’il n’en reste dans le paquet courant n’est
 * pas une erreur. En particulier l'appel de \ref ogg_packet_read sur un
 * paquet entièrement lu (il ne reste plus d’octets dedans), doit
 * simplement retourner 0 puisqu’elle n’a pu lire aucun octet.
 * 
 * Une erreur est par contre d’appeler cette fonction si le dernier
 * paquet du flux a été sauté avec la fonction ogg_packet_next.
 *
 * @param[in] lstream flux logique en cours de lecture
 * @param[in] buf buffer de sortie contenant les octets lus, qui doit
 *            être déjà alloué avec une capacité au moins égale à
 *            nbytes.
 * @param[in] nbytes nombre de bytes à lire dans lstream
 * @param[out] nbytes_read le nombre d'octets réellement lus
 * 
 * @return OGG_OK en cas de succès, un statut d'erreur adéquat sinon.
 */
ogg_status_t ogg_packet_read(ogg_logical_stream_t *lstream, 
                             uint8_t *buf, uint32_t nbytes,
                             uint32_t *nbytes_read);
	  
/** 
 * Fonction permettant d'avancer au paquet suivant.
 * 
 * Tous les octets éventuellement restant dans le paquet courant sont
 * sautés. L’appel suivant de \ref ogg_packet_read copiera les premiers octets
 * du paquet nouvellement sélectionné.
 * 
 * Un appel à cette fonction sur le dernier paquet n'est pas une erreur, mais
 * le devient une fois que le dernier paquet a été sauté.
 * 
 * @param[in] lstream flux logique en cours de lecture
 * 
 * @return OGG_OK en cas de succès, OGG_END si le paquet courant est le
 *         dernier paquet du flux, un statut d'erreur adéquat sinon.
 */
ogg_status_t ogg_packet_next(ogg_logical_stream_t *lstream);


/** 
 * Fonction estimant la taille en octets du paquet courant.
 * 
 * Cette taille peut être estimée à partir d'informations sur le paquet
 * courant (à rajouter dans votre structure ogg_packet_handler): par exemple 
 * le segment courant et le nombre d'octets déjà lus.
 * Si le paquet est terminé, la fonction doit retourner la taille réelle.
 * Sinon, la valeur retournée doit être strictement suérieure au nombre
 * d’octets lus jusqu’à présent dans le paquet.
 *
 * Cette fonction est surtout utile pour le débogage, afin de vérifier
 * si le paquet entier a été lu.
 * 
 * @param[in] lstream flux logique en cours de lecture.
 * 
 * @return taille en octets du paquet courant.
 */
uint32_t ogg_packet_size(ogg_logical_stream_t *lstream);


/** 
 * @brief Fonction permettant d'accéder à la position absolue dans le
 * flux logique.
 *
 * Cette position est définie dans la section 4.4.1 sur les en-tête de page.
 * Si le paquet courant est le dernier de la page et qu'il se termine sur
 * cette page, alors la position rechercher est la valeur \a granule
 * \a position de l'en-tête.
 * Sinon, la position du paquet courant n'est pas (encore) connue, et vaut -1.
 * Quand le dernier paquet est sauté, la position indiquée doit être celle
 * du dernier paquet.
 *
 * @param lstream[in] flux logique en cours de lecture.
 * @param position[out] la position abosule du paquet courant, -1 si elle
 *        ne peut pas être déterminée
 * 
 * @return OGG_OK en cas de succès, un statut d'erreur adéquat sinon.
 */
ogg_status_t ogg_packet_position(ogg_logical_stream_t *lstream,
                                 int64_t *position);


/** @} */

#endif				/* __OGG_PACKET_H */
