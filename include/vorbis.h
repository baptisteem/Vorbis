#ifndef __VORBIS_H
#define __VORBIS_H

/**
 * @addtogroup vorbis_main
 * @{
 */

#include <vorbis_main.h>
#include <error.h>

/* 
 * Function declarations
 */

/** 
 * @brief Fonction de décodage principal du flux Vorbis.
 *
 * Cette fonction gère la machine d'état globale du décodage du flux Vorbis.
 * Elle est chargée en particulier:
 * - des initialisations nécessaires aux moments adéquats (avant ou après
 *   la lecture des entêtes);
 * - de la lecture des entêtes;
 * - de la lecture de l'ensemble des paquets audio:
 *   - avancement dans les paquets à l'aide du module vorbis_io;
 *   - lancement du décodage des paquets, via le module vorbis_packet;
 *   - traitement des PCM décodés via le module pcm_handler.
 *   - Attention cependant à ne pas en produire trop! Utiliser la fonction
 *    \c vorbis_io_limit pour connaître la limite d'échantillons à produire
 *    et tronquer la fin du flux le cas échéant.
 * - libérations nécessaires 
 * 
 *
 * @param ogg_stream est le pointeur sur le flux logique OGG contenant le flux
 * Vorbis
 * @param pcm_hdler est le pointeur sur le gestionnaire d'échantillons PCM à
 * utiliser.
 * 
 * @return VBS_SUCCESS en cas de succès, VBS_FATAL en cas de passage de mauvais
 * arguments ou VBS_BADSTREAM en cas d'erreur de lecture dans le flux.
 */
status_t decode_stream(ogg_logical_stream_t *ogg_stream,
                       pcm_handler_t *pcm_hdler);

/**
 * @}
 */
#endif				/* __VORBIS_H */
