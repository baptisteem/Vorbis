#include "pcm_handler.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>


#define NB_FORMAT 2
//demander à bap comment on free un pointeur de fonction

struct pcm_handler1 {

	pcm_handler_t base;
	unsigned int sampl;
	unsigned int nchan;
	const char *format;
	const char *arg;
};

typedef struct pcm_handler1 pcm_handler1_t;

void pcm_handler_list(const char *prefix){
	printf("entree pcm_handler_list \n");

	char * format[NB_FORMAT]={"wav","raw"};
	
	for (uint32_t i=0; i<NB_FORMAT;i++){
		printf("%s%s \n",prefix,format[i]);	
	}
}

int init(pcm_handler_t * hdlr, unsigned int sampl,
		     unsigned int nchan){

	printf("entree dans init \n");
	pcm_handler1_t *hdlr1=(pcm_handler1_t *)hdlr;
	hdlr1->nchan=nchan;
	hdlr1->sampl=sampl;
	//cette fonction doit etre utilisée une seule fois
	hdlr1->base.init=NULL;
	return 0;
};

int process_raw (pcm_handler_t * hdlr, unsigned int num,int16_t ** samples){
	
	printf("entree dans process_raw \n");	
	pcm_handler1_t *hdlr1=(pcm_handler1_t *)hdlr;
	for (uint32_t n; n<num; n++){
		for (uint32_t ch=0; ch<(hdlr1->nchan); ch ++){
			printf("%d",(samples[ch])[n]);
			//afficher d'une autre facon surement	
		}
		printf("passage à la ligne \n");
	}
	return 0;
}

int process_wav (pcm_handler_t * hdlr, unsigned int num,
			int16_t ** samples) {

	printf("entree dans process_wav \n");
	pcm_handler1_t *hdlr1=(pcm_handler1_t *)hdlr;
	FILE *fp=fopen(hdlr1->arg,"w");

	//bloc de declaration d'un fichier wav


	//bloc decrivant le format audio


	//bloc des données

	//le mieux serait ici d'appeller process_raw pas possible vu que ca renvoie 0
	
	for (uint32_t n=0; n<num; n++){
		for (uint32_t ch=0; ch<(hdlr1->nchan); ch ++){
			//printf("%d",(samples[ch])[n]);
			//afficher d'une autre facon surement	
		}
		//printf("passage à la ligne \n");
	}
	return 0;

}

//La fonction finalize est commune aux deux formats 
int finalize (pcm_handler_t * hdlr) {

	printf("entree finalize \n");
	// on empeche la réutilisation de la fonction process
	pcm_handler1_t *hdlr1=(pcm_handler1_t *)hdlr;
	hdlr1->base.process=NULL;
	return 0;
}


pcm_handler_t *pcm_handler_create(const char *format, const char *arg){
	
	printf("debut de handler_create %s \n",format);

	if (strcmp(format,"wav") && strcmp(format,"raw")){
	//realiser le test plutot avec la fct d affichage
		
			printf("mauvais format \n");
			return (pcm_handler_t *)NULL;
	}
	else{
		printf("entree dans initialization \n");	
		//initializing the parameters
	
		pcm_handler1_t *hdlr1=malloc(sizeof(pcm_handler1_t));

		hdlr1->format=format;
		hdlr1->arg=arg;
		hdlr1->base.init=init;
		hdlr1->base.finalize=finalize;

		if (format="wav"){
			hdlr1->base.process=process_wav;
		}
		else{
			hdlr1->base.process=process_raw;
		}
			
		printf("end handler create \n");
		return (pcm_handler_t *)hdlr1;
	}

}

void pcm_handler_delete(pcm_handler_t * hdlr){

	printf("entree delete \n");
	pcm_handler1_t *hdlr1=(pcm_handler1_t *)hdlr;
	//free(hdlr1->base.init);
	//free(hdlr1->base.process);
	//free(hdlr1->base.finalize);
	free(hdlr1);
}
