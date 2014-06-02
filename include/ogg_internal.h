#ifndef __OGG_INTERNAL_H
#define __OGG_INTERNAL_H

/**
 * \ingroup ogg_core
 * 
 * @file  ogg_internal.h
 * 
 * @brief Structures et fonctions internes permettant l'interface entre
 * les deux modules Ogg.
 */

/*
 * Includes
 */
#include <stdint.h>
#include <ogg.h>
#include <error.h>

/*
 * Type declarations
 */
typedef struct ogg_page_hdr ogg_page_hdr_t;
typedef struct ogg_packet_handler ogg_packet_handler_t;
typedef struct internal_ogg_logical_stream internal_ogg_logical_stream_t;


/*
 * Structure definitions
 */

/**
 * \ingroup ogg_core
 * 
 * @brief En-tête de page Ogg, sans sa table des segments.
 *
 * Sa définition (ordre et taille des champs, voir section 4.4.1 du sujet)
 * correspond exactement à l'en-tête présent dans le fichier.
 * Ceci permet, dans le cas où l’architecture utilisée est little-endian,
 * une copie directe du contenu du fichier vers cette structure.
 * Dans le cas contraire big-endian, il faut en plus intervertir les octets
 * des champs multi-octets.
 *
 */
struct ogg_page_hdr
{
	uint8_t  magic[4];
	uint8_t  version;
	uint8_t  type;
	int64_t  gran_pos;
	uint32_t stream_id;
	uint32_t page_id;
	uint32_t crc;
	uint8_t  nb_segs;
} __attribute__ ((__packed__));
/* 
 * @note L’attribut __attribute__((__packed__)) permet de spécifier au
 * compilateur qu’aucun espace libre ne doit être laissé entre les champs
 * (aucun alignement ne sera fait en mémoire).
 */



/**
 * \ingroup ogg_packet
 *
 * @brief Structure interne de gestion d'un packet Ogg.
 *
 * Cette structure est simplement une structure "de typage" pour le champ
 * \a packet d'un internal_ogg_logical_stream.
 *
 * Conceptuellement, elle devrait être vide et ne contenir aucun champ.
 * Cependant, les structures vides sont inscrites dans la norme C comme étant
 * "compiler dependant" (pour des raisons de taille de type principalement).
 * GCC les gère bien, mais ce n'est pas le cas de tous les compilateurs.
 * La structure est donc ici définie avec un champ inutile, le plus petit
 * possible.
 *
 * Naturellement vous devez étendre cette structure en y ajoutant les
 * informations nécessaires à votre implémentation des modules ogg.
 */
struct ogg_packet_handler
{
	uint8_t dummy;    /**< Champ inutile, pour respecter la norme C sur
	                       les structures vides. */
};


/**
 * \ingroup ogg_core
 *
 * @brief Represention interne d'un flux logique.
 * 
 * Cette structure est une extension de la structure ogg_logical_stream.
 * Elle contient des champs supplémentaires nécessaires pour la lecture du
 * flux mais qui n’ont pas besoin d’être vus par les autres modules.
 *
 * Les trois champs \a header, \a table et \a data doivent être cohérents
 * entre eux: \a table et \a data doivent être de même taille, et valent
 * NULL si la page courante ne contient aucun segment.
 */
struct internal_ogg_logical_stream
{
	ogg_logical_stream_t base;    /**< Partie publique de cette structure */
	ogg_physical_stream_t *phy;   /**< Flux physique auquel appartient le
	                                   flux logique */
	ogg_packet_handler_t *packet; /**< Pointeur vers une structure de gestion
	                                   de packet */
	ogg_page_hdr_t *header;       /**< Pointeur vers l'entête (sans la page
	                                   des segments) du flux logique. */
	uint8_t *table;               /**< Table des segments: un tableau
	                                   contenant la taille de chacun des
	                                   segments de la page. */
	uint8_t *data;                /**< Pointeur vers les segments de donnée
	                                   de la page. */
};


/*
 * Ogg page size numbers
 */
/* max number of segment in a page */
#define OGG_PAGE_MAX_SEGS 255
/* max size of a segment in bytes */
#define OGG_PAGE_SEG_MAX_SIZE 255
/* max size of page data part */
#define OGG_PAGE_DAT_MAX_SIZE (OGG_PAGE_MAX_SEGS * OGG_PAGE_SEG_MAX_SIZE)

/*
 * Ogg 'magic' pattern elements
 */
#define OGG_MAGIC_0 'O'
#define OGG_MAGIC_1 'g'
#define OGG_MAGIC_2 'g'
#define OGG_MAGIC_3 'S'

/* 
 * Ogg CRC polynomial
 */
#define OGG_CRC_POLY 0x04c11db7

/*
 * Ogg page 'type' field flags
 */
#define OGG_FLAG_CONT 0x1
#define OGG_FLAG_BOS  0x2
#define OGG_FLAG_EOS  0x4


/* 
 * Function declarations
 */

/** 
 * \ingroup ogg_packet
 *
 * @brief Fonction permettant d'associer un gestionaire de paquet
 * ogg_packet_handler à un flux logique.
 * 
 * Elle doit:
 * - créer un ogg_packet_handler puis l'initialiser de manière à ce que
 *   le paquet courant soit le premier paquet du flux logique.
 *   Un appel à la fonction \ref ogg_packet_read doit renvoyer les premiers
 *   octets du premier paquet.
 * - attacher cet objet au flux logique passé en paramètre, via son champs
 *   \a packet (\a lstream est en réalité du type étendu 
 *   \ref internal_ogg_logical_stream), initialement NULL.
 *
 * Lors de l’appel de cette fonction la première page du flux logique doit
 * avoir été mise dans les champs \a header, \a table et \a data du flux.
 *
 * @param stream le flux logique auquel on attache un gestionnaire de paquet.
 * 
 * @return OGG_OK en cas de succès, un statut d'erreur adéquat sinon.
 */
ogg_status_t ogg_packet_attach(internal_ogg_logical_stream_t *lstream);
   



/** 
 * \ingroup ogg_packet
 *
 *  @brief Fonction libérant le champ \a packet du flux passé en argument,
 *  qui est ensuite mis à NULL.
 * 
 * @param stream le flux logique duquel on détache un gestionnaire de paquet.
 * 
 * @return OGG_OK en cas de succès, un statut d'erreur adéquat sinon.
 */
ogg_status_t ogg_packet_detach(internal_ogg_logical_stream_t *lstream);


/** 
 * \ingroup ogg_core
 * 
 * @brief Fonction permettant d'avancer à la page suivante d'un flux logique.
 *
 * Seuls les champs \a header, \a table, \a data du flux sont modifiés.
 * S'il n’y a plus de page dans le flux logique, la fonction retourne OGG_END
 * et mets les 3 champs à NULL.
 *
 * @param lstream le flux logique en cours de décodage
 * 
 * @return OGG_OK en cas de succès, OGG_END si le flux était déjà sur la
 *         dernière page, un statut d'erreur adéquat sinon.
 */
ogg_status_t ogg_get_next_page(internal_ogg_logical_stream_t *lstream);


#endif				/* __OGG_INTERNAL_H */
