#include "envelope.h"
#include <stdio.h>

envelope_t *envelope_init(uint16_t *blocksize){

	//reading in the flux
	//2 malloc et classique
	//initialization
	
}

void envelope_free(envelope_t *env){
	free(env->blocksize);
	free(env);
}

status_t envelope_prepare(envelope_t *env, sample_t *filter){
//comment obtient on a nouvelle taille ?
//calcul des ij et on definit en fonction des zones,
//on renvoie la meme enveloppe
//


}

//cache sera toujours de la taille de la fenetre maximum
//sample_t sera de la taille de fenetre maximum qui correpond
//à la superposition maximum

uint16_t envelope_overlap_add(envelope_t *env, sample_t *in,
                              sample_t *cache, sample_t *out){


//prendre le milieu de la 	
	}

	
	
	}else{
	//we are not at the beginning
	


	}

	uint32_t nc=
	uint32_t np=
	
	

	//add the first part of the current window
	for (uint32_t i=0; i<
	
	//add the second part of the previous if it exists
	else{
		for (uint32_t i=0
	}
	//mettre la condition voir si on est à la premiere fenetre ou non

	//preparing the cache
	free(cache); //est ce que ce garde l adresse ?
	//on sinon on fait un cache de la taille de la plus grande fenetre


	
}
