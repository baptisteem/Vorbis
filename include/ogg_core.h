#ifndef __OGG_CORE_H
#define __OGG_CORE_H

/**
 * \addtogroup ogg_core
 * @{
 *
 * @brief  Module de gestion du container Ogg.
 *
 * Ce module gère essentiellement le désentrelacement des pages des
 * différents flux logiques.
 *
 * Il regroupe des structures et fonctions :
 * - publiques, dans \c ogg.h, utilisables dans le projet
 * - et privées, dans \c ogg_internal.h, destinées à l'interface entre
 *   les deux modules ogg_core et ogg_packet.
 */

/**
 * @file   ogg_core.h
 */


/*
 * Includes
 */

#include <stdio.h>
#include <ogg.h>
#include <pcm_handler.h>
#include <error.h>

/* 
 * Function declarations
 */

/** 
 * @brief Alloue et initialise un flux physique à partir d'un fichier,
 * en remplissant la liste des flux logiques qu'il contient.
 *
 * Comme expliqué au chapitre 4 du sujet, les premières pages des flux
 * logiques (1 seule page par flux, celle qui a le flag BOS) sont les unes
 * à la suite des autres au début du fichier Ogg. Cette fonction lit donc
 * les premières pages afin de lister tous les flux logiques que contient le
 * flux physique. 
 * 
 * @param[in]  file descripteur de fichier ouvert en lecture
 * @param[out] ppstream pointeur pour le retour de l'adresse de la structure
 *             allouée et initialisée dans cette fonction.
 * 
 * @return OGG_OK en cas de succès, OGG_END si le flux physique ne
 *         contient aucun flux logique, un statut d'erreur adéquat sinon.
 */
ogg_status_t ogg_init(FILE *file, ogg_physical_stream_t **ppstream);


/** 
 * @brief Decode un flux logique au format Vorbis.
 * 
 * Le flux logique à décoder doit faire partie de la liste des flux logiques
 * du flux physique, construite par la fonction ogg_init.
 *(ou n'est pas appelée, c'est à la discrétion du module main). 
 
 * Le décodage proprement dit est délégué à la fonction \ref decode_stream du
 * module vorbis_main. Celle-ci doit pouvoir faire appel à \ref ogg_packet_read
 * et récupérer ainsi les premiers octets du premier paquet du flux logique.
 * Pour celà, un ogg_packet_handler aura été préalablement attaché
 * au flux \a lstream. Il sera détaché une fois le décodage terminé.
 * Ces opérations d'attacher/détacher sont réalisées par le module ogg_packet.
 *
 * @note Cette fonction ne peut être appelée qu’une seule fois par
 * physical_stream. Dans ce projet, il n’est en effet pas possible
 * de décoder plusieurs flux logiques d’un même flux physique.
 *
 * @note Elle peut aussi ne jamais être appelée, c'est à la discrétion du
 * module main. 
 * 
 * @param[in] lstream flux logique à décoder
 * @param[in] pcm_hdler module de traitement des échantillons PCM décodés
 *            dans le flux Vorbis.
 * 
 * @return OGG_OK en cas de succès, OGG_ERR_ILL_OP si la fonction a déjà été
 *         appelée sur le flux \a lstream, un statut d'erreur adéquat sinon.
 */
ogg_status_t ogg_decode(ogg_logical_stream_t *lstream,
                        pcm_handler_t *pcm_hdler);


/** 
 * @brief Libérer la mémoire d'une structure de flux physique Ogg.
 *
 * Cette fonction peut être appelée directement après l’appel à la 
 * fonction ogg_decode, ou directement après ogg_init si on ne veut
 * décoder aucun flux.
 *
 * @param[in] pstream pointeur sur la zone mémoire à libérer.
 * 
 * @return OGG_OK en cas de succès, un statut d'erreur adéquat sinon.
 */
ogg_status_t ogg_term(ogg_physical_stream_t *pstream);

/** @} */

#endif				/* __OGG_CORE_H */
