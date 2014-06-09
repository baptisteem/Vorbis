#include "pcm_handler.h"
#include <stdio.h>
#include <stdlib.h>


#define NB_FORMAT 2

void pcm_handler_list(const char *prefix){

	char * format[NB_FORMAT]={"wav","raw"};
	
	for (uint32_t i=0; i<NB_FORMAT;i++){
		printf("%s%s \n",prefix,format[i]);	
	}
}

pcm_handler_t *pcm_handler_create(const char *format, const char *arg){
	// checking if the format is correct 
	pcm_handler_t *hdlr=malloc(sizeof(pcm_handler_t));
	//avec comparaison de chaine, un booleen, une while ptete
	//lancer un message d erreur si besoin
	//Is it important to know exactly the format
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
	

	//initialization of pcm_handler



}

void pcm_handler_delete(pcm_handler_t * hdlr){

	free(hdlr->init);
	free(hdlr->process);
	free(hdlr->finalize);
	free(hdlr);
}
