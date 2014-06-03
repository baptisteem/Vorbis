#ifndef __VORBIS_HEADERS_H
#define __VORBIS_HEADERS_H

#include "vorbis_main.h"

/*
 *
 * \addtogroup common_header
 * @{
 *
 * @brief Module transverse de lecture des en-têtes.
 * 
 */

/** 
 * Fonction effectuant l'opération de décodage commune aux trois
 * en-têtes, à savoir les 7 octets contenant l'identification du type
 * de paquet, du type de header et du flux ("vorbis"), comme indiqué
 * en section 5.2 du sujet. Elle doit donc être appelée au début de
 * chaque fonction décodant un type d'en-tête particulier
 * (header1_decode, header2_decode, header3_decode).
 * 
 * @param[in] stream est le pointeur sur le flux Vorbis à decoder
 * @param[out] header_type contient le type de header décodé par cette fonction
 * 
 * @return VBS_SUCCESS en cas de succès, VBS_BADSTREAM en cas
 * d'erreur de lecture dans le flux
 */
status_t vorbis_common_header(vorbis_stream_t *stream, uint8_t *header_type);

/* *
 * @}
 */

/* *
 * \addtogroup header1
 * @{
 *
 * @brief Module de lecture de l'en-tête 1.
 * 
 */

/** 
 * Fonction effectuant l'opération de décodage de l'en-tête 1 du flux
 * Vorbis, en-tête d'identification. En pratique, cette fonction lit
 * sur le flux, effectue des vérifications sur les informations lues
 * comme détaillé en section 5.2.1 du sujet, et remplit les champs
 * du codec (inclus dans stream) concernés.
 * 
 * @param[in,out] stream est le pointeur sur le flux Vorbis à
 * decoder. La structure codec qu'il contient est déjà allouée avant
 * l'appel à vorbis_header1_decode et est remplie en fonction des
 * informations lues par cette fonction.
 * 
 * @return VBS_SUCCESS en cas de succès, VBS_BADSTREAM en cas d'erreur de
 * lecture dans le flux
 */
status_t vorbis_header1_decode(vorbis_stream_t *stream);

/* *
 * @}
 */

/* *
 * \addtogroup header2
 * @{
 *
 * @brief Module de lecture de l'en-tête 2.
 * 
 */

/** 
 * Fonction effectuant l'opération de décodage de l'en-tête 2 du flux
 * Vorbis, en-tête de commentaires. En pratique, cette fonction ne
 * fait que lire sur le flux. Les informations lues ne sont pas
 * stockées, mais affichées sur le terminal de façon
 * synthétique. Cette fonction implémente le pseudo-code présenté en
 * section 5.2.2 du sujet.
 * 
 * @param[in] stream est le pointeur sur le flux Vorbis à decoder
 * 
 * @return VBS_SUCCESS en cas de succès, VBS_BADSTREAM en cas d'erreur de
 * lecture dans le flux
 */
status_t vorbis_header2_decode(vorbis_stream_t *stream);

/* *
 * @}
 */

/* *
 * \addtogroup header3
 * @{
 *
 * @brief Module de lecture de l'en-tête 3.
 * 
 */

/** 
 * Fonction effectuant l'opération de décodage de l'en-tête 3 du flux
 * Vorbis, en-tête de configuration. En pratique, cette fonction est
 * en charge d'appeler les fonctions d'allocation/initialisation des
 * structures de données de chacun des éléments du Vorbis (mode,
 * mapping, codebook, residue, floor et transformée). Ces fonctions,
 * comprenant la lecture du flux pour mettre à jour les champs des
 * structures codebooks_desc, floors_desc, residues_desc,
 * mappings_desc et modes_desc, sont quant à elles implémentées
 * indépendamment, dans chacun des modules concernés. La section 5.2.3
 * du sujet détaille dans quel ordre les fonctions
 * d'allocation/initialisation de ces modules doivent être appelées.
 * 
 * @param[in,out] stream est le pointeur sur le flux Vorbis à
 * decoder. Elle contient les structures xxx_setup_t (via l'objet
 * codec) qui sont remplies par cette fonction.
 * 
 * @return VBS_SUCCESS en cas de succès, VBS_BADSTREAM en cas d'erreur de
 * lecture dans le flux
 */
status_t vorbis_header3_decode(vorbis_stream_t *stream);

#endif				/* __VORBIS_HEADERS_H */

/* *
 * @}
 */
