#ifndef __FLOOR_H
#define __FLOOR_H

/**
 * \addtogroup floor
 * @{
 *
 * @brief  Module de gestion des floors
 * 
 */

/*
 * Type definitions
 */
typedef struct floors_setup floors_setup_t;
typedef struct floor_data floor_data_t;
typedef struct floor floor_t;

/*
 * Includes
 */
#include <stdint.h>

#include <vorbis.h>
#include <sample.h>
#include <error.h>

/*
 * Structure definitions
 */

/**
 * @brief Cette structure rassemble l'ensemble des informations concernant la
 * configuration des différents floors du flux vorbis lu.
 * 
 * Le champ \a floor_count donne donc le nombre de configurations de floor
 * présentes dans le tableau \a floors. Ce tableau est donc constitué de
 * \a floor_count pointeurs sur des structures de type \c floor_t.
 * 
 * Les éléments \a data0 et \a data1 sont des pointeurs sur les zones de
 * travail partagées de type \c floor_data_t, utilisables par les floors de
 * type respectifs 0 et 1, pour les opérations de décodage des packets audio
 * et la génération de la courbe gros grain. Ce type pourra être étendu
 * (voir ci-dessous la définition de \c floor_data), on utilise donc des
 * pointeurs vers des floor_data_t.
 * Il faut noter qu'un tel usage n'est pas obligatoire, vous pourrez opter pour
 * l'allocation en pile (c'est à dire sans utiliser de pré-allocation), mais
 * ceux qui optent pour l'utilisation de zones de travail partagées (plus
 * compliquée, mais plus propre et plus efficace) ont la possibilité de le
 * faire.
 */
struct floors_setup {
	uint8_t floor_count;  /**< Nombre de floor */
	floor_t **floors;     /**< Tableau des descriptions de floor */
	floor_data_t *data0;  /**< Données de travail pour les floor de type 0 */
	floor_data_t *data1;  /**< Données de travail pour les floor de type 1 */
};

/**
 * @brief Cette structure contient les informations basiques génériques
 * (indépendantes du type de floor). 
 *
 * On y retrouve un type (\a type) et un identifiant (\a id) correspondant
 * à l'indice où ce floor est référencé dans la configuration floors_setup.
 * 
 * Les deux derniers champs sont des pointeurs de fonctions:
 * - \a decode effectue le décodage d'un paquet audio et la synthèse de la
 *    courbe à gros grain. Cette fonction utilise un paramètre
 *   \a stream pour réaliser les opérations de lecture dans le paquet, un
 *   paramètre \a floor_cfg correspondant à la configuration du floor courant
 *   et enfin un vecteur de sortie \a v et le nombre de valeurs 
 *   à décoder \a v_size (correspondant à \f$ \frac{N}{2} \f$).
 * - l'opération \a free réalise elle la libération de la mémoire de
 *   la structure de floor pointée par l'argument \a floor, une fois
 *   l'ensemble du décodage du flux terminé.
 * 
 * Ces fonctions \a decode et \a free seront en fait des références aux
 * fonctions adéquates spécifiques à chaque type de floor. Selon \a type,
 * \a decode sera ainsi l'adresse de la fonction \a floor_type0_decode ou 
 * celle de \a floor_type1_decode. Ces fonctions devront être définies de
 * manière static dans \c floor0.c et \c floor1.c. 
 * Vous pouvez étudier le fichier \c floor1_example.c qui est fourni 
 * pour vous guider.
 */
struct floor  {
	int type;                           /**< Type de floor */
	uint8_t id;                         /**< Identifiant de ce floor */
	status_t (*decode) (vorbis_stream_t *stream, floor_t *floor_cfg,
	                    sample_t *v, uint16_t v_size);
                                            /**< Operation de décodage */
	void (*free) (floor_t *floor);      /**< Opération de libération */
};


/**
 * @brief Cette structure sert de base pour les structures de travail des
 * différents types de floor.
 * 
 * En effet, chaque type de floor requiert des
 * éléments différents lors de sa manipulation, et l'allocation de ces
 * éléments peut vite se révéler compliquée. Cette structure permet d'allouer
 * les éléments une bonne fois pour toutes pour chaque type, indépendamment du
 * nombre de configurations de floor. On y retrouve juste un identifiant de type
 * (\a type) et un nombre d'occurrences (\a occ). Ce dernier champ permet de
 * recenser le nombre de floor du type correspondant (i.e. le nombre de
 * configurations qui utilisent ce type de floor). On imaginera aisément que dans
 * le cas où cette valeur est nulle, aucune allocation de zone de travail
 * supplémentaire n'est nécessaire. Ce type pourra être étendu de manière
 * spécifique pour chaque type de floor (voir floor1.h et floor1_example.c).
 */
struct floor_data {
	int type;		/**< Type de floor */
	int occ;		/**< Nombre d'occurences de ce type */
};


/*
 * Function declarations
 */

/** 
 * Cette fonction permet d'initialiser un \c floors_setup_t par la lecture de la
 * section correspondante dans l'en-tête 3 du flux Vorbis (voir le paragraphe 
 * 5.2.3.c du sujet). La structure \c floors_setup_t 
 * est allouée et initialisée par cette fonction, et un pointeur vers cette
 * structure sera renvoyé dans \a *pset. Le premier argument \a stream 
 * permet de lire le flux (via stream->io_desc) et donne accès au vecteur
 * des blocksize utilisé. La configuration des différents floor ainsi que
 * la creation et l'allocation des structures \c floor_data_t seront 
 * réalisés par les fonctions appropriées spécifiques de chaque type de floor 
 * (voir floor0.h et floor1.h).
 * 
 * @param[in] stream est un pointeur sur le flux Vorbis en cours de lecture.
 * @param[out] pset est un pointeur pour le retour de l'adresse de la structure
 *          allouée et initialisée dans cette fonction.  
 * 
 * @return VBS_SUCCESS en cas de succes, VBS_OUTOFMEMORY en cas d'erreur
 *      d'allocation ou VBS_BADSTREAM en cas d'erreur de lecture dans le flux
 */
status_t floors_setup_init(vorbis_stream_t *stream, floors_setup_t **pset);

/** 
 * Cette fonction permet de libérer l'ensemble des structures allouées à
 * l'initialisation du \a set. Chaque floor_t sera libéré grâce à son 
 * pointeur de fonction (qui est spécifique de son type).
 * @param set est un pointeur sur la structure à libérer.
 */
void floors_free(floors_setup_t *set);

/** 
 * Cette fonction est responsable du décodage de la partie floor des
 * paquets audio Vorbis. Elle fera appel à la fonction spécifique au
 * type de floor courant grâce au pointeur de fonction \a decode 
 * de la structure \c floor_t.
 * 
 * @param[in] stream est un pointeur sur le flux Vorbis en cours de décodage.
 * @param[in] floor est le floor à utiliser pour le paquet courant
 * @param[out] v est le vecteur des données produites
 * @param[in] v_size est la taille du vecteur de données
 * 
 * @return VBS_SUCCESS en cas de succes, VBS_UNUSED dans le cas où le floor 
 *         n'est pas utilisé, VBS_BADSTREAM en cas d'erreur de lecture dans
 *         le flux ou d'erreur avec \c read_nbits.
 */
status_t floor_decode(vorbis_stream_t *stream, floor_t *floor,
                      sample_t *v, uint16_t v_size);

/* @} */
#endif				/* __FLOOR_H */
