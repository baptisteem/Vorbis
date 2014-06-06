#include "envelope.h"
#include <stdio.h>
#include <math.h>
#include "sample.h"

//Realisation d'une extension qui finalement est inutile
//Faut il l'enlever ?

struct envelope1 {
	envelope_t base;
	uint8_t ancient_next_window;
	uint8_t ancient_curr_window;
};

typedef struct envelope1 envelope1_t;

envelope_t *envelope_init(uint16_t *blocksize){

	printf("\n \n \nstarting envelope_init \n");
	//initialization
	envelope1_t *env=malloc(sizeof(envelope1_t));
	uint16_t *block=malloc(2*sizeof(uint16_t));
	for (uint32_t i=0; i<2; i++){
		block[i]=blocksize[i];
	}
	
	env->base.initialized=0;
	env->base.blocksize=block;
	env->base.prev_window=0;
	env->base.curr_window=0;
	env->base.next_window=0;
	env->ancient_next_window=0;
	env->ancient_curr_window=0;
	printf("fin de envelope init \n");
	return (envelope_t *)(env);
	
}

void envelope_free(envelope_t *env){

	printf(" starting envelope_free \n");
	envelope1_t *env1=(envelope1_t *)env;
	free(env1->base.blocksize);
	free(env1);
	printf(" envelope_free_done \n");
}



status_t envelope_prepare(envelope_t *env, sample_t *filter){


	envelope1_t *env1=(envelope1_t *) env;


	if (env1->base.next_window != 0){
		env1->base.next_window=1;
	}
	/*
	if (env1->base.initialized == 1){
		env1->base.prev_window=env1->ancient_curr_window;
		env1->base.curr_window=env1->ancient_next_window;

	}
	env1->ancient_next_window=env1->base.next_window;
	env1->ancient_curr_window=env1->base.curr_window;*/

	//	printf("prepare: prec %d, cour%d, next%d,ancient_next%d les tailles %d %d initialized %d \n",env1->base.prev_window,env1->base.curr_window,env1->base.next_window,env1->ancient_next_window, env1->base.blocksize[0],env1->base.blocksize[1],env1->base.initialized);

	//calculating the i0,i1,i2,i3
	uint32_t i0,i1,i2,i3,left_n,right_n;
	uint32_t nc=(env1->base.blocksize)[env1->base.curr_window];
	uint32_t b0=(env1->base.blocksize)[0];

	if (env1->base.prev_window != env1->base.curr_window){

		i0=(nc-b0)/4;
		i1=(nc+b0)/4;
		left_n=b0/2;
	}
	else{
		i0=0;
		i1=nc/2;
		left_n=nc/2;	
	}

	if (env1->base.curr_window != env1->base.next_window){

		i2=(3*nc-b0)/4;
		i3=(3*nc+b0)/4;
		right_n=b0/2;
	}
	else{
		i2=nc/2;
		i3=nc;
		right_n=nc/2;	
	}

	//defining the filter
	sample_t aux;

	for (uint32_t i=0; i<i0; i++){
		filter[i]=(sample_t)0;
	}

	for (uint32_t i=i0; i<i1; i++){
		aux=0.5;
		aux +=(sample_t)(i-i0);
		aux /=(sample_t)left_n;
		aux= sin(M_PI_2*aux);
		aux = aux*aux;
		aux= sin(M_PI_2*aux);
		filter[i]=aux;
	}

	for (uint32_t i=i1; i<i2; i++){
		filter[i]=(sample_t)1;
	}

	for (uint32_t i=i2; i<i3; i++){
		aux =0.5;
		aux +=(sample_t)(i-i2);
		aux /=(sample_t)right_n;
		aux= sin(M_PI_2*aux +M_PI_2);
		aux = aux*aux;
		aux= sin(M_PI_2*aux);
		filter[i]=aux;
	}

	for (uint32_t i=i3; i<nc; i++){
		filter[i]=(sample_t)0;
	}
	
	return VBS_SUCCESS;
}


uint16_t envelope_overlap_add(envelope_t *env, sample_t *in,
                              sample_t *cache, sample_t *out){

	envelope1_t * env1=(envelope1_t *)env;
	uint32_t nc=(env1->base.blocksize)[env1->base.curr_window];
	uint32_t np=(env1->base.blocksize)[env1->base.prev_window];

	if (env1->base.initialized ==0){

		for (uint32_t i=0; i<nc/2; i++){
			cache[i]=in[i+nc/2];
		}
		printf("done overlap initialisation\n");
		return 0;
	}
	else if (nc == np){
	//Les 2 fenetres sont de meme taille
		for (uint32_t i=0; i<nc/2; i++){
			out[i]=cache[i]+in[i];
		}
		for (uint32_t i=0; i<nc/2; i++){
			cache[i]=in[i+nc/2];
		}
		return (nc/2);
	}
	else if (np < nc){
	//La fenetre precedente est plus petite 
		for (uint32_t i=0; i<(nc+np)/4;i++){
			out[i]=in[i+(nc-np)/4];
		}

		for (uint32_t i=0; i<np/2; i++){
			out[i]=cache[i]+out[i];
		}

		for (uint32_t i=0; i<nc/2; i++){
			cache[i]=in[i+nc/2];
		}
		return((np+nc)/4); 
	}
	else{
	//La fenetre precedente est plus grande
		for (uint32_t i=0; i<(nc+np)/4; i++){
			out[i]=cache[i];
		}
		for (uint32_t i=0; i<nc/2; i++){
			out[i+(np-nc)/4]=out[i+(np-nc)/4]+in[i];
		}
		for (uint32_t i=0; i<nc/2; i++){
			cache[i]=in[i+nc/2];
		}
		return((np+nc)/4); 

	}
}



