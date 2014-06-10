#include "pcm_handler.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>


#define NB_FORMAT 2

//demander à bap comment on free un pointeur de fonction, le mettre À null il ne 
//prend pas de place
//trouver une fonction qui affiche en binaire sans perte à l'avant ou arriere
//trouver une fonction qui permet de donner la conversion en little endian
//tenter de regrouper les fonctions raw 
//tenter d utiliser la fonction affichage list pour verifier le format

typedef struct pcm_handler1 pcm_handler1_t;


struct pcm_handler1 {

	pcm_handler_t base;
	unsigned int sampl;
	unsigned int nchan;
	const char *format;
	const char *arg;
};


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

//Fonction qui permet de passer en little_endian des int16_t
int16_t swap_int16( int16_t val ) 
{
    //return (val << 8) | ((val >> 8) & 0xFF);
    return ((val & 0xFF) << 8) | (val >> 8);
}


void print_little_endian (uint32_t size,uint32_t n){
	//size peut valoir 16 ou 32 (2 ou 4 octets à afficher)
	
	char c[size];
	for (uint32_t i=0; i<size; i++){
		c[i]=n % mod 2;
		c /= 2;
	}


}




int process_raw (pcm_handler_t * hdlr, unsigned int num,int16_t ** samples){
	
}

int process_wav (pcm_handler_t * hdlr, unsigned int num,
			int16_t ** samples) {

	//printf("entree dans process_wav \n");
	pcm_handler1_t *hdlr1=(pcm_handler1_t *)hdlr;
	FILE *fp=fopen(hdlr1->arg,"w");

	//bloc des données
	

	//le mieux serait ici d'appeller process_raw pas possible vu que ca renvoie 0
	
	for (uint32_t n=0; n<num; n++){
		for (uint32_t ch=0; ch<(hdlr1->nchan); ch ++){
		//	printf("%s ",bin((samples[ch])[n]));
		//	printf("%s ",bin(swap_int16((samples[ch])[n])));
		//	printf("2 affichages\n");
			//il faut afficher en little endian	
		}
	}

	
	return 0;

}


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

		if (!strcmp(format,"raw")){

			hdlr1->base.process=process_raw;
			return (pcm_handler_t *)hdlr1;
		}
		else{
			hdlr1->base.process=process_wav;
			FILE *fp=fopen(hdlr1->arg,"w");

			//bloc de declaration d'un fichier wav
			
			affichage_string("RIFF");


			//bloc decrivant le format audio

			return (pcm_handler_t *)hdlr1;
		}
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
