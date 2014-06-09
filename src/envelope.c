#include "envelope.h"
#include <stdio.h>
#include <math.h>
#include "sample.h"



envelope_t *envelope_init(uint16_t *blocksize){

	printf("\n \n \nstarting envelope_init \n");
	//initialization
	envelope_t *env=malloc(sizeof(envelope_t));
	uint16_t *block=malloc(2*sizeof(uint16_t));
	for (uint32_t i=0; i<2; i++){
		block[i]=blocksize[i];
	}
	
	env->initialized=0;
	env->blocksize=block;
	env->prev_window=0;
	env->curr_window=0;
	env->next_window=0;
	printf("fin de envelope init \n");
	return (env);
	
}

void envelope_free(envelope_t *env){

	printf(" starting envelope_free \n");
	free(env->blocksize);
	free(env);
	printf(" envelope_free_done \n");
}



status_t envelope_prepare(envelope_t *env, sample_t *filter){


	if (env->next_window != 0){
		env->next_window=1;
	}
	
	//calculating the i0,i1,i2,i3
	uint32_t i0,i1,i2,i3,left_n,right_n;
	uint32_t nc=(env->blocksize)[env->curr_window];
	uint32_t b0=(env->blocksize)[0];

	if (env->prev_window != env->curr_window){

		i0=(nc-b0)/4;
		i1=(nc+b0)/4;
		left_n=b0/2;
	}
	else{
		i0=0;
		i1=nc/2;
		left_n=nc/2;	
	}

	if (env->curr_window != env->next_window){

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

	uint32_t nc=(env->blocksize)[env->curr_window];
	uint32_t np=(env->blocksize)[env->prev_window];

	if (env->initialized ==0){

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



