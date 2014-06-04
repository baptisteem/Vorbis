#include "pcm_handler.h"
#include <stdio.h>

#define NB_FORMAT 2

void pcm_handler_list(const char *prefix){

	char * format[NB_FORMAT]={"wav","raw"};
	
	for (uint32_t i=0; i<NB_FORMAT;i++){
		printf("%s%s \n",prefix,format[i]);	
	}
}

pcm_handler_t *pcm_handler_create(const char *format, const char *arg){
	// checking if the format is correct 

	//avec comparaison de chaine, un booleen, une while ptete
	//lancer un message d erreur si besoin
	//Is it important to know exactly the format
	

	//initialization of pcm_handler



}

void pcm_handler_delete(pcm_handler_t * hdlr){

}
