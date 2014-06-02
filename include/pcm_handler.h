#ifndef __PCM_HANDLER_H
#define __PCM_HANDLER_H

/**
 * @addtogroup pcm_handler
 * @{
 * 
 * @brief  Module de gestion des sorties PCM
 * 
 * Ce module sert à traiter les échantillons audios décodés par
 * Vorbis. Plusieurs sorties sont possibles pour les échantillons: par exemple
 * les stocker dans un fichier, ou les envoyer sur les hauts parleurs.  Ce
 * module permet de gérer plusieurs sorties.
 * Notre version en propose 2:
 * -# "wav": met les échantillons dans un fichier au format WAV,
 * -# "raw": met les échantillons directement dans un fichier sans traitement.
 */

/*
 * Includes
 */
#include <stdint.h>

/*
 * Type definitions
 */
typedef struct pcm_handler pcm_handler_t;

/*
 * Pcm handler structure
 */
/**
 * @brief Cette structure représente un traitant d'échantillons qui est utilisé
 * par les modules vorbis.
 *
 * Elle contient 3 champs qui sont des pointeurs sur des fonctions qui sont
 * spécifiées ci-dessous.
 *
 * Cette fonction permet d'initialiser le traitant avec les paramètres du flux
 * audio (fréquence d'échantillonage et nombre de canaux). La taille d'un
 * échantillon n'est pas paramétrable, seuls des échantillons signés sur 16 bits
 * sont utilisés dans ce projet. Cette fonction doit être appelée une seule fois
 * avant tout appel à la fonction \a process.
 *
 * \a init prend en argument le traitant d'échantillon, la fréquence
 * d'échantillonage (le nombre d'échantillon qui doivent lus par seconde pour
 * chaque canal audio) et le nombre de canaux audios (1 pour du mono, 2 pour de
 * la stéréo, etc).
 *
 * \a init retourne 0 en cas de succès et un nombre négatif en cas
 * d'erreur.
 *
 * La fonction \a process permet de traiter des échantillons audios. En plus du
 * traitant, elle prend en argument le nombre d'échantillons par canal, ainsi
 * que les échantillons. Cette fonction peut être appelée autant de fois que
 * nécessaire
 * 
 * Les échantillons sont donnés dans un tableau de tableaux. La première dimension
 * est le canal, la deuxième est le numéro de l'échantillon. Les échantillons sont
 * ordonnés par ordre croissant dans le temps.
 * 
 * \a process retourne 0 en cas de succès et un nombre négatif en cas
 * d'erreur.
 * 
 * La fonction \a finalize permet de finaliser le traitement des
 * échantillons. Une fois appelée, il n'est plus possible de traiter des
 * échantillons avec \a process.
 * 
 * \a finalisze retourne 0 en cas de succès et un nombre négatif en cas
 * d'erreur.
 * 
 */
struct pcm_handler {
	int (*init) (pcm_handler_t * hdlr, unsigned int sampl,
		     unsigned int nchan);
                            /**< Pointeur sur la fonction init correspondante */
	int (*process) (pcm_handler_t * hdlr, unsigned int num,
			int16_t ** samples);
                            /**< Pointeur sur la fonction process correspondante */
	int (*finalize) (pcm_handler_t * hdlr);
                            /**< Pointeur sur la fonction finalize correspondante */
};

/* 
 * Functions
 */

/** 
 * Cette fonction permet de lister tous les formats de sortie disponibles. Elle
 * affiche sur la sortie standard la liste de ces formats. Chaque nom de format
 * est affiché sur une ligne différente précédé de la chaîne de caractères
 * \a  prefix passée en argument.  Dans le cas d'un module ne gérant que le
 * format de sortie \b "wav", un appel à cette fonction avec comme argument la 
 * chaîne vide \b "" doit uniquement imprimer \b "wav\n" sur la sortie standard.
 * 
 * @param[in] prefix est une chaîne de caractère contenant le préfixe d'affichage.
 */
void pcm_handler_list(const char *prefix);

/** 
 * Cette fonction alloue et renvoie un \c pcm_handler_t du format \a format
 * demandé. Le deuxième argument est un argument pour le gestionnaire
 * d'échantillon. Les chaînes de caractère \a format correctes sont celles
 * listées par la fonction \a pcm_handler_list.  Dans le cas d'un gestionnaire
 * stockant les échantillons dans un fichier (par exemple \b "wav"), c'est le nom
 * de ce fichier qui est mis dans cet argument.
 * 
 * Les trois champs du \c pcm_handler_t renvoyé doivent être valides.  Ainsi il
 * sera nécessaire d'avoir aussi implémenté dans ce module les 3 fonctions à
 * mettre dans les champs du \c pcm_handler_t pour pouvoir les
 * initialiser.
 * 
 * @param[in] format est le nom du format souhaité.
 * @param[in] arg est un argument passé au gestionnaire d'échantillons.
 * 
 * @return un pointeur vers le \c pcm_handler_t créé en cas de succès ou NULL en
 * cas d'erreur.
 */
pcm_handler_t *pcm_handler_create(const char *format, const char *arg);

/** 
 * Cette fonction supprime toute zone mémoire allouée pour le
 * \c pcm_handler_t dont le pointeur est passé en argument.
 * 
 * @param[in] hdlr pointe la zone à libérer.
 */
void pcm_handler_delete(pcm_handler_t * hdlr);

/**
 * @}
 */
#endif				/* __PCM_HANDLER_H */
