#ifndef __VORBIS_IO_H
#define __VORBIS_IO_H

/**
 * \addtogroup   vorbis_io
 * @{
 * 
 * @brief  Module de gestion des lectures binaires dans le flux.
 * 
 */

/*
 * Type definitions
 */
typedef struct vorbis_io vorbis_io_t;

/*
 * Includes
 */
#include <stdint.h>
#include <ogg.h>
#include <error.h>

/* 
 * Function declaration
 */

/** 
 * Fonction qui permet d'allouer et initialiser une structure de
 * travail de lecture binaire.
 * 
 * @param[in] ogg_desc est un pointeur sur le flux OGG correspondant.
 * 
 * @return un pointeur sur la zone nouvellement allouée et initialisée, 
 *         NULL en cas d'erreur.
 */
vorbis_io_t *vorbis_io_init(ogg_logical_stream_t *ogg_desc);

/** 
 * Fonction qui permet de libérer une structure de travail de lecture
 * binaire.
 * 
 * @param[in,out] io est le pointeur sur la structure à libérer
 */
void vorbis_io_free(vorbis_io_t *io);

/** 
 * Fonction principale du module, elle permet à l'ensemble des autres modules de
 * vorbis d'effectuer des opérations de lecture au bit, \a nb_bits à
 *  partir du container OGG.
 * 
 * @param[in] nb_bits est le nombre de bits à lire (max: 32)
 * @param[out] dst est un pointeur sur le mots mémoire de 32bits où placer 
 *            les données lues
 * @param[in] io est le pointeur sur la structure de travail
 * @param[out] p_count est un pointeur sur un mot de 32 bits où sera
 *            placé le nombre de bits effectivement lus
 * 
 * @return VBS_SUCCESS en cas de success, VBS_EOP en cas de fin
 *         de paquet ou VBS_FATAL en cas d'erreur. 
 */
status_t vorbis_read_nbits(uint32_t nb_bits, uint32_t *dst,
			   vorbis_io_t *io, uint32_t *p_count);


/** 
 * Fonction qui permet de passer au paquet OGG suivant, pour celà il
 * interagit avec le flux OGG. 
 * 
 * @param[in] io est le pointeur sur la structure de travail
 * 
 * @return VBS_SUCCESS en cas de succes, VBS_EOS si la fin du flux est
 *         atteinte, VBS_FATAL en cas d'erreur.
 */
status_t vorbis_io_next_packet(vorbis_io_t *io);

/** 
 * Fonction permet de récupérer une information mise à la disposition du 
 * codec par le conteneur (OGG en l'occurence). Cette information est la
 * limite du nombre d'échantillon à produire grâce au 
 * \c ogg_packet_position
 *
 * @param[in] io est le pointeur sur la structure de travail
 * 
 * @return la limite du flux courant en terme d'échantillons
 */
int64_t vorbis_io_limit(vorbis_io_t *io);

#endif				/* __VORBIS_IO_H */

/** @} */
