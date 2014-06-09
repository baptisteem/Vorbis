#include "pcm_handler.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>


#define NB_FORMAT 2

//demander à bap comment on free un pointeur de fonction
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

//Fonction qui permet de convertir un entier en un string 
//correspondant à sa valeur binaire//fonctionne pas, j ai besoin des 0 inutiles
//aussi
char * bin (unsigned long int i)
{
    static char buffer [1+sizeof (unsigned long int)*8] = { 0 };
    char *p=buffer-1+sizeof (unsigned long int)*8;
    do { *--p = '0' + (i & 1); i >>= 1; } while (i);
    return p;
}

//permet d'afficher en binaire little endian un string
void affichage_string(char *s){
	printf("%s",bin((unsigned long int)s));
}

void affichage_entier(uint8_t u){
	printf("%s",bin(u));
}


int process_raw (pcm_handler_t * hdlr, unsigned int num,int16_t ** samples){
	
}

int process_wav (pcm_handler_t * hdlr, unsigned int num,
			int16_t ** samples) {

	//printf("entree dans process_wav \n");
	pcm_handler1_t *hdlr1=(pcm_handler1_t *)hdlr;
	FILE *fp=fopen(hdlr1->arg,"w");


	//bloc de declaration d'un fichier wav



	//bloc decrivant le format audio


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
		//printing the first parameters
		
		FILE *fp=fopen(hdlr1->arg,"w");
		affichage_string("RIFF");


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
