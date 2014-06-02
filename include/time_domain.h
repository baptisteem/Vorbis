#ifndef __TIME_DOMAIN_TRANSFORM_H
#define __TIME_DOMAIN_TRANSFORM_H

/**
 * \addtogroup time_domain
 * @{
 *
 * @brief  Module de gestion des transformations de domaine de temps.
 * 
 * Ce module est responsable de la gestion des transformées en domaine temporel.
 * Cette partie de la norme Vorbis est une extension possible pour les futures
 * versions de la norme, mais n'est pas utilisée aujourd'hui, puisque la MDCT est
 * utilisée systématiquement. En revanche, plusieurs algorithmes existent pour
 * effectuer l'iMDCT. Dans un premier temps, vous implanterez l'algorithme
 * décrit au paragraphe 3.4 du sujet.
 * NB: Le module \c time_domain.o qui vous est fourni implante une iMDCT 
 * rapide (fast_imdct), il est donc normal que votre version soit 
 * (beaucoup) plus lente.
 */

typedef struct time_domain_transform time_domain_transform_t;

#include <vorbis_main.h>
#include <sample.h>
#include <error.h>

/**
 * @brief Structure de base de Time Domain Transform
 * 
 * Cette structure devra être étendue pour stocker les informations
 * nécessaires à l'implantation de l'iMDCT choisie. Par exemple pour
 * l'implantation initiale, vous aurez besoin des tailles des fenêtres
 * (stream->codec->blocksize).
 * Rappel: vous ne devez PAS modifier ce fichier \c time_domain.h ! 
 * L'extension de type se fera dans vos fichiers .c, comme illustré
 * dans \c floor1_example.c .
 */
struct time_domain_transform {
	int type;		/**< Type de Time Domain Transform, utile si
				   vous tentez diverses implantations de l'iMDCT */
};

/* 
 * Function declarations
 */

/** 
 * Fonction responsable de la lecture de la partie de l'entête 3 qui est prévue
 * pour cette extension (time_domain_transform), et l'initialisation de la 
 * TDT. Voir le paragraphe 5.2.3.b du sujet, ainsi que les commentaires
 * ci-dessus concernant l'extension du type \c time_domain_transform.
 * 
 * @param[in] stream est un pointeur sur le flux Vorbis en cours de lecture.
 * @param[out] ptdt est un pointeur pour le retour de l'adresse de la structure
 *          allouée et initialisée dans cette fonction.
 * 
 * @return VBS_SUCCESS en cas de succes, VBS_OUTOFMEMORY en cas d'erreur
 *      d'allocation ou VBS_BADSTREAM en cas d'erreur de lecture dans le flux.
 */
status_t time_domain_transforms_setup_init(vorbis_stream_t *stream,
					   time_domain_transform_t **ptdt);

/** 
 * Fonction réalisant la libération de la structure de time_domain_transform.
 * 
 * @param[in] tdt est un pointeur sur la structure à libérer.
 */
void time_domain_transforms_free(time_domain_transform_t *tdt);

/** 
 * Fonction réalisant l' opération de changement de domaine (fréquentiel -> temporel) 
 * incluant l'iMDCT et le filtre. Dans un premier temps, vous implanterez l'algorithme
 * iMDCT décrit au paragraphe 5.3.4 du sujet. Cet algorithme nécessite de connaître
 * la taille N de la fenêtre courante, il faut donc avoir étendu le type
 * time_domain_transform_t afin de stocker cette information, comme discuté 
 * ci-dessus.
 * 
 * @param[in] tdt est un pointeur sur la structure définissant la TDT
 * @param[in] fsamp est le vecteur des échantillons fréquentiels (de taille N/2)
 * @param[out] tsamp est le vecteur de sortie des échantillons temporels, il
            doit avoir été préalablement alloué (de taille N)
 * @param[in] filter est le vecteur des coefficients du filtre de l'iMDCT dans 
 *          notre cas ils correspondent à la fonction de fenêtrage (de taille N)
 * @param[in] mode définit le type de fenêtre (petite 0 ou grande 1)
 * 
 * @return  VBS_SUCCESS 
 */
status_t time_domain_transform_process(time_domain_transform_t *tdt, sample_t *fsamp,
				       sample_t *tsamp, sample_t *filter, int mode);

/* @} */
#endif				/* __TIME_DOMAIN_TRANSFORM_H */
