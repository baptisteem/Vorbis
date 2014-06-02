#ifndef __VORBIS_MAIN_H
#define __VORBIS_MAIN_H

/**
 * @addtogroup vorbis_main
 * @{
 * 
 * @brief Ce module regroupe la définition des structures transversales
 * utilisées dans le décodeur, ainsi que les fonctions liées au
 * décodage des en-têtes et du flux principal Vorbis.
 */


/*
 * Type definitions
 */
typedef struct vorbis_stream vorbis_stream_t;

typedef struct vorbis_codec vorbis_codec_t;

/*
 * Includes
 */
#include <stdint.h>

#include <ogg_packet.h>
#include <pcm_handler.h>
#include <vorbis_io.h>
#include <vorbis_packet.h>

#include <codebook.h>
#include <time_domain.h>
#include <floor.h>
#include <residue.h>
#include <mapping.h>
#include <mode.h>

#include <error.h>

/*
 * Structure definitions
 */

/**
 * @brief La structure vorbis_codec rassemble les informations de 
 * configuration glanées lors de la lecture des en-têtes.
 * 
 * Les champs de cette structure
 * peuvent être rassemblés en deux groupes, le premier correspondant
 * aux informations récupérées lors de la lecture de l'en-tête 1, le
 * second correspondant à celles de l'en-tête 3. On notera que les
 * informations de l'en-tête 2 ne sont pas stockées par notre
 * décodeur, mais directement affichées sur le terminal.
 * 
 */
struct vorbis_codec {

	/*
	 * Identification header content
	 */
	uint32_t vorbis_version;    /**< Version de la norme Vorbis utilisée */
	uint8_t audio_channels;     /**< Nombre de canaux audio */
	uint32_t audio_sample_rate; /**< Fréquence d'échantillonnage de ces canaux */
	uint32_t bitrate_maximum;   /**< Débit maximum */
	uint32_t bitrate_nominal;   /**< Débit nominal */
	uint32_t bitrate_minimum;   /**< Débit minimum */
	uint16_t blocksize[2];      /**< Taille des fenêtres (petite et grande) */

	/*
	 * Comment header infos
	 */
	/* Skipped */

	/*
	 * Setup header content
	 */
	codebook_setup_t *codebooks_desc;   /**< Informations de configuration
	                                         des codebooks */
	floors_setup_t *floors_desc;        /**< Informations de configuration
	                                         des floors  */
	residues_setup_t *residues_desc;    /**< Informations de configuration
	                                         des residues */
	mappings_setup_t *mappings_desc;    /**< Informations de configuration
	                                         des mappings  */
	window_modes_setup_t *modes_desc;   /**< Informations de configuration
	                                         des modes  */
        time_domain_transform_t *tdt_desc;  /**< Informations de configuration
                                                de la TDT */
};

/**
 * @brief La structure vorbis_stream est la structure principale utilisée
 * transversalement dans tout le décodeur Vorbis.
 * 
 * Elle contient à la fois le
 * point d'entrée permettant de lire le flux à décoder, mais aussi la
 * configuration du décodeur et enfin les échantillons PCM produits
 * par notre décodeur. Cette structure n'est en aucun cas à étendre:
 * elle garantit, avec vorbis_codec, la compatibilité des modules
 * fournis avec ceux que vous développerez.
 * 
 */
struct vorbis_stream {

	vorbis_codec_t *codec;    /**< Configuration du décodeur, remplie lors
	                               de la lecture des différents en-têtes. */
	                               
	pcm_handler_t *pcm_hdler; /**< Echantillons PCM produits après
	                               décodage du flux */
	                               
	vorbis_io_t *io_desc;     /**< Point d'entrée pour la lecture du flux. */
};


/* 
 * Function declarations
 */

/** 
 * @brief Fonction permettant d'allouer une structure vorbis_codec pour
 * stocker les informations lues dans les entêtes.
 *
 * Les champs de la structure ne sont pas initialisés; ils le seront au fur
 * et à mesure de la lecture (des entêtes) du flux Ogg.
 *
 * @return un pointeur sur la structure nouvellement allouée
 *         ou \c NULL en cas de problème d'allocation.
 */
vorbis_codec_t *vorbis_codec_new(void);


/** 
 * @brief Fonction qui permet de libérer la mémoire d'une structure
 * vorbis_codec.
 *
 * @param[in] codec est un pointeur sur la zone mémoire à libérer.
 */
void vorbis_codec_free(vorbis_codec_t *codec);


/** 
 * @brief Fonction permettant d'allouer et d'initaliser la structure
 * principale vorbis_stream du décodeur.
 *
 * @param[in] ogg le flux Ogg dans lequel est lu le flux Vorbis (seul le
 *            premier flux logique du container est lu dans ce projet).
 * @param[in] pcm_hdler le module de gestion des échantillons PCM décodés
 *            dans le flux Vorbis.
 * 
 * @return un pointeur sur la structure nouvellement allouée
 *         ou \c NULL en cas de problème d'allocation.
 */
vorbis_stream_t *vorbis_new(ogg_logical_stream_t *ogg, pcm_handler_t *pcm_hdler);


/** 
 * @brief Fonction qui permet de libérer la mémoire d'une structure
 * vorbis_stream_t.
 *
 * @param[in] vorbis est un pointeur sur la zone mémoire à libérer.
 */
void vorbis_free(vorbis_stream_t *vorbis);



/**
 * @}
 */
#endif				/* __VORBIS_MAIN_H */
