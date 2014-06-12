#include "pcm_handler.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>


#define NB_FORMAT 2
 
typedef struct pcm_handler1 pcm_handler1_t;

struct pcm_handler1 {

	pcm_handler_t base;
	unsigned int sampl;
	unsigned int nchan;
	const char *format;
	const char *arg;
	int32_t size;
	FILE *fp;
};


void pcm_handler_list(const char *prefix){

	char * format[NB_FORMAT]={"wav","raw"};
	
	for (uint32_t i=0; i<NB_FORMAT;i++){
		printf("%s%s \n",prefix,format[i]);	
	}
}

int init_wav(pcm_handler_t * hdlr, unsigned int sampl,unsigned int nchan);
int init_raw(pcm_handler_t * hdlr, unsigned int sampl,unsigned int nchan);
int process (pcm_handler_t * hdlr, unsigned int num,int16_t ** samples);
int finalize_wav (pcm_handler_t * hdlr);
int finalize_raw (pcm_handler_t * hdlr);

pcm_handler_t *pcm_handler_create(const char *format, const char *arg){
	
	if (strcmp(format,"wav") && strcmp(format,"raw")){
		
			printf("mauvais format \n");
			return (pcm_handler_t *)NULL;
	}
	else{
		//initializing the parameters
		pcm_handler1_t *hdlr1=malloc(sizeof(pcm_handler1_t));

		hdlr1->format=format;
		hdlr1->arg=arg;
		hdlr1->size=0;
		hdlr1->base.process=process;
		hdlr1->fp=fopen(hdlr1->arg,"wb");


		if (!strcmp(format,"raw")){

			hdlr1->base.init=init_raw;
			hdlr1->base.finalize=finalize_raw;
			return (pcm_handler_t *)hdlr1;
		}
		else{
			hdlr1->base.init=init_wav;
			hdlr1->base.finalize=finalize_wav;
			return (pcm_handler_t *)hdlr1;
		}


	}

}


/*************************************************************/

int init_raw(pcm_handler_t * hdlr, unsigned int sampl,
		     unsigned int nchan){
	pcm_handler1_t *hdlr1=(pcm_handler1_t *)hdlr;
	hdlr1->nchan=nchan;
	hdlr1->base.init=NULL;
	return 0;
}


int init_wav(pcm_handler_t * hdlr, unsigned int sampl,
		     unsigned int nchan){

	pcm_handler1_t *hdlr1=(pcm_handler1_t *)hdlr;
	hdlr1->nchan=nchan;
	hdlr1->sampl=sampl;
	hdlr1->base.init=NULL;
 	//seek  

	uint32_t p;
	p=0x46464952;//"RIFF" en bigendian	
	fwrite(&p,4,1,hdlr1->fp);
	p=0;//on reserve juste la place necessaire pour la suite
	fwrite(&p,4,1,hdlr1->fp);
	p=0x45564157;//"WAVE" en bigendian
	fwrite(&p,4,1,hdlr1->fp);
	hdlr1->size +=12;

	//bloc decrivant le format audio
	p=0x20746d66; //"fmt " en bigendian
	fwrite(&p,4,1,hdlr1->fp);
	p=16;
	fwrite(&p,4,1,hdlr1->fp);
	p=1;
	fwrite(&p,2,1,hdlr1->fp);
	p=hdlr1->nchan;
	fwrite(&p,2,1,hdlr1->fp);
	p=hdlr1->sampl;
	fwrite(&p,4,1,hdlr1->fp);
	p=2* hdlr1->sampl * hdlr1->nchan;
	fwrite(&p,4,1,hdlr1->fp);
	p=2*hdlr1->nchan;
	fwrite(&p,2,1,hdlr1->fp);
	p=16;
	fwrite(&p,2,1,hdlr1->fp);
	hdlr1->size +=24;
		

	//bloc donnant les échantillons
	p=0x61746164; //"data" en bigendian
	fwrite(&p,4,1,hdlr1->fp);
	p=0; //on reserve juste la place necessaire pour la suite
	fwrite(&p,4,1,hdlr1->fp);
	hdlr1->size +=8;

	return 0;

};

/**************************************************************/

int process (pcm_handler_t * hdlr, unsigned int num,
			int16_t ** samples) {

	pcm_handler1_t *hdlr1=(pcm_handler1_t *)hdlr;
	int16_t p;
	
	for (uint32_t n=0; n<num; n++){
		for (uint32_t ch=0; ch<(hdlr1->nchan); ch ++){
			
		p=(samples[ch])[n];
		fwrite(&p,2,1,hdlr1->fp);
		hdlr1->size += 2;
		}
	}	
	return 0;
}

int finalize_raw (pcm_handler_t * hdlr) {

	pcm_handler1_t *hdlr1=(pcm_handler1_t *)hdlr;
	hdlr1->base.process=NULL; 
  	fclose(hdlr1->fp);

	return 0;
}

int finalize_wav (pcm_handler_t * hdlr) {

	pcm_handler1_t *hdlr1=(pcm_handler1_t *)hdlr;
	uint32_t p;
	fseek(hdlr1->fp,4,SEEK_SET);
	p=hdlr1->size-8;
	fwrite(&p,4,1,hdlr1->fp);
	fseek(hdlr1->fp,40,SEEK_SET);
	p=hdlr1->size-44;
	fwrite(&p,4,1,hdlr1->fp);

	hdlr1->base.process=NULL;
	fclose(hdlr1->fp);
	return 0;
};



void pcm_handler_delete(pcm_handler_t * hdlr){

	pcm_handler1_t *hdlr1=(pcm_handler1_t *)hdlr;
	free(hdlr1);
}
