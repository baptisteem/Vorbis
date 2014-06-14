#include <stdio.h>
#include "helpers.h"
#include "codebook.h"



typedef struct codebook_tree codebook_tree_t;
typedef struct codebook_node codebook_node_t;
typedef struct codebook_1 codebook_1_t;

struct codebook_tree{
  codebook_node_t *head; 
  uint32_t type;               // 0 si mode scalaire, 1 si mode VQ
  uint32_t codebook_dimensions; // 0 si mode scalaire
  uint32_t codebook_entries;   // nombre de mots
  sample_t **vq_vector;        // tableau de vq, indicé par scalar (NULL si mode scalaire)
}codebook_tree;

struct codebook_node{
  codebook_node_t *fils_gauche;
  codebook_node_t *fils_droit;
  
  uint32_t noeud_sature;     // 1 si noeud sature (toute les feuilles en dessous remplies)
  uint32_t est_une_feuille;  // 0 si noeud, 1 si feuille
  uint32_t scalar;           // Feuille 
}codebook_node;

struct codebook_1{
  codebook_t base;
  codebook_tree_t *tree;  // Pointeur sur l'arbre
};




sample_t * construction_VQ_type1(uint32_t entry, uint32_t codebook_dimensions, uint32_t *codebook_multiplicands, uint32_t codebook_lookup_values, sample_t codebook_delta_value, sample_t codebook_minimum_value, uint32_t codebook_sequence_p){
  sample_t last = 0;
  uint32_t index_divisor = 1;
  uint32_t index;
  sample_t *vq_vector = calloc(codebook_dimensions, sizeof(sample_t));
  
  for(uint32_t i = 0; i<codebook_dimensions; i++){

    index = (entry/index_divisor) % codebook_lookup_values;
    vq_vector[i] = codebook_minimum_value + last + codebook_delta_value * (sample_t)(codebook_multiplicands[index]);
    if(codebook_sequence_p == 1){
      last = vq_vector[i];
    }
    index_divisor = index_divisor * codebook_lookup_values;

  }
  return vq_vector;
}



sample_t * construction_VQ_type2(uint32_t entry, uint32_t codebook_dimensions, uint32_t *codebook_multiplicands, uint32_t codebook_delta_value, uint32_t codebook_minimum_value, uint32_t codebook_sequence_p){
  sample_t last = 0;
  uint32_t index = entry * codebook_dimensions;
  sample_t *vq_vector = calloc(codebook_dimensions, sizeof(sample_t));
  
  for(uint32_t i = 0; i<codebook_dimensions; i++){
    
    vq_vector[i] = codebook_minimum_value + last + codebook_delta_value * (sample_t)(codebook_multiplicands[index]);
    if(codebook_sequence_p == 1){
      last = vq_vector[i];
    }
    index++;
    
  }
  return vq_vector;
}




status_t remplir_arbre_rec(codebook_node_t **node, uint32_t length, uint32_t entry){
  status_t ret = VBS_BADSTREAM;


  /**************     Si noeud innexistant on le crée     ***************/
  
  if(*node == NULL){                      
    (*node) = malloc(sizeof(codebook_node_t));
    (*node)->fils_droit = NULL;
    (*node)->fils_gauche = NULL;
    
    if(length == 0){
      
      (*node)->noeud_sature = 1;
      (*node)->est_une_feuille = 1;
      (*node)->scalar = entry;
      ret = VBS_SUCCESS;
    }else{
      (*node)->noeud_sature = 0;
      (*node)->est_une_feuille = 0;
      (*node)->scalar = 0;
      ret = remplir_arbre_rec(&(*node)->fils_gauche, length-1, entry);
      
    }
    
  /**************       si le noeud existe on descend      ***************/    

  }else if(length != 0){   
    
    if((*node)->fils_gauche!=NULL && (*node)->fils_droit!=NULL){
      
      if((*node)->fils_gauche->noeud_sature == 0)
	ret = remplir_arbre_rec(&(*node)->fils_gauche, length-1, entry);
      
      if((*node)->fils_droit->noeud_sature==0 && ret==VBS_BADSTREAM)
	ret = remplir_arbre_rec(&(*node)->fils_droit, length-1, entry);
      
    }else if((*node)->fils_gauche == NULL){
      ret = remplir_arbre_rec(&(*node)->fils_gauche, length-1, entry);
      
    }else{
      
      if((*node)->fils_gauche->noeud_sature == 0)
	ret = remplir_arbre_rec(&(*node)->fils_gauche, length-1, entry);
      if(ret == VBS_BADSTREAM)
	ret = remplir_arbre_rec(&(*node)->fils_droit, length-1, entry);
    }
    
  }
  
  if((*node)->fils_gauche!=NULL && (*node)->fils_droit!=NULL){
    if((*node)->fils_gauche->noeud_sature==1 && (*node)->fils_droit->noeud_sature==1)
      (*node)->noeud_sature = 1;
  }
  
  return ret;
}




void affiche_arbre(codebook_node_t *node){
  if (node==NULL){
  }else{
    if(node->est_une_feuille == 1){
      fprintf(stderr, " %u ", node->scalar);
    }else{
      affiche_arbre(node->fils_gauche);
      affiche_arbre(node->fils_droit);
    }
  }
}



status_t codebook_decode(vorbis_stream_t * stream, codebook_t ** codebook, uint32_t codebook_id){

  /*****   codebook temporaire   *****/
  codebook_1_t *codebook_temp = malloc(sizeof(codebook_1_t));

  /*****   variables initialisation   *****/
  uint32_t p_count, vorbis_sync_pattern, codebook_dimensions, codebook_entries;
  status_t ret;

  /*****   variables on recupere codebook_length   *****/
  uint32_t entry, ordered, sparse, used, length, entry_number, num_bits;
  uint32_t *codeword_lengths = NULL;

  /*****   recuperation des parametres   *****/
  uint32_t codebook_lookup_type, codebook_value_bits, codebook_sequence_p, codebook_lookup_values, dst;
  sample_t codebook_minimum_value, codebook_delta_value;
  uint32_t *codebook_multiplicands;

  

/*
                   ####################################################
		   |                                                  |
                   |      initialisation pour chaque codebook         |
	           |                                                  |
                   ####################################################
*/


  codebook_temp->base.index = codebook_id;

  ret = vorbis_read_nbits(24, &vorbis_sync_pattern, stream->io_desc, &p_count);
  if(p_count!=24 || ret!=VBS_SUCCESS || vorbis_sync_pattern!=0x564342)
    return VBS_BADSTREAM;
  
  ret = vorbis_read_nbits(16, &codebook_dimensions, stream->io_desc, &p_count);
  if(p_count!=16 || ret!=VBS_SUCCESS)
    return VBS_BADSTREAM;
  
  ret = vorbis_read_nbits(24, &codebook_entries, stream->io_desc, &p_count);
  if(p_count!=24 || ret!=VBS_SUCCESS)
    return VBS_BADSTREAM;
  


/*                 ####################################################
		   |                                                  |
                   |           on recupere codeword_length            |
	           |                                                  |
                   ####################################################
*/
  
    
  codeword_lengths = calloc(codebook_entries, sizeof(uint32_t));
    
  ret = vorbis_read_nbits(1, &ordered, stream->io_desc, &p_count);
  if(p_count!=1 || ret!=VBS_SUCCESS)
    return VBS_BADSTREAM;
  
  
  if(ordered == 0){
    
    ret = vorbis_read_nbits(1, &sparse, stream->io_desc, &p_count);
    if(p_count!=1 || ret!=VBS_SUCCESS)
      return VBS_BADSTREAM;
    
    for(entry = 0; entry<codebook_entries; entry++){
      if(sparse==1){
	
	ret = vorbis_read_nbits(1, &used, stream->io_desc, &p_count);
	if(p_count!=1 || ret!=VBS_SUCCESS)
	  return VBS_BADSTREAM;
	
	if(used == 1){
	  ret = vorbis_read_nbits(5, &length, stream->io_desc, &p_count);
	  if(p_count!=5 || ret!=VBS_SUCCESS)
	    return VBS_BADSTREAM;
	  codeword_lengths[entry] = length + 1;
	}else{
	  codeword_lengths[entry] = -1;             // convention pour <unused>  
	}
	
	 }else{
	
	ret = vorbis_read_nbits(5, &length, stream->io_desc, &p_count);
	if(p_count!=5 || ret!=VBS_SUCCESS)
	  return VBS_BADSTREAM;
	codeword_lengths[entry] = length + 1;
	
      }	 
    }
  }else{
    
    entry = 0;
    ret = vorbis_read_nbits(5, &length, stream->io_desc, &p_count);
    if((p_count!=5)||(ret!=VBS_SUCCESS))
      return VBS_BADSTREAM;
    
    while(entry < codebook_entries){    
      length++;
      num_bits = ilog(codebook_entries - entry);
      ret = vorbis_read_nbits(num_bits, &entry_number, stream->io_desc, &p_count);
      if(p_count!=num_bits || ret!=VBS_SUCCESS)
	return VBS_BADSTREAM;
      
      for(uint32_t i = entry; i<(entry+entry_number); i++){
	codeword_lengths[i] = length;
      }
	
      entry = entry + entry_number;
    }
    
    if(entry>codebook_entries)
      return VBS_BADSTREAM;
  }


/*                 ####################################################
		   |                                                  |
                   |             construction de l'arbre              |
	           |                                                  |
                   ####################################################
*/


  codebook_temp->tree = malloc(sizeof(codebook_tree));
  codebook_temp->tree->head = NULL;
  codebook_temp->tree->codebook_entries = codebook_entries;
  codebook_temp->tree->codebook_dimensions = codebook_dimensions;
  
  for(entry = 0; entry<codebook_entries; entry++){
    if(codeword_lengths[entry] != (uint32_t)-1){                // suit la convention pout <unused>

	ret = remplir_arbre_rec(&codebook_temp->tree->head ,codeword_lengths[entry], entry);
	if(ret != VBS_SUCCESS)
	  return VBS_BADSTREAM;

    }
  }


/*                 ####################################################
		   |                                                  |
                   |              representation des VQ               |
	           |                                                  |
                   ####################################################
*/
    

  /*******************         recuperation des parametres        ********************/

  ret = vorbis_read_nbits(4, &codebook_lookup_type, stream->io_desc, &p_count);
  if(p_count!=4 || ret!=VBS_SUCCESS)
    return VBS_BADSTREAM;  
  
  codebook_temp->tree->type = 0;
  
  if(codebook_lookup_type>0){
    
    codebook_temp->tree->type = 1;
    
    ret = vorbis_read_nbits(32, &dst, stream->io_desc, &p_count);
    if(p_count!=32 || ret!=VBS_SUCCESS)
      return VBS_BADSTREAM;    
    codebook_minimum_value = float32_unpack(dst);

    ret = vorbis_read_nbits(32, &dst, stream->io_desc, &p_count);
    if((p_count!=32)||(ret!=VBS_SUCCESS))
      return VBS_BADSTREAM;    
    codebook_delta_value = float32_unpack(dst);
    
    ret = vorbis_read_nbits(4, &codebook_value_bits, stream->io_desc, &p_count);
    if(p_count!=4 || ret!=VBS_SUCCESS)
      return VBS_BADSTREAM;    
    codebook_value_bits++;

    ret = vorbis_read_nbits(1, &codebook_sequence_p, stream->io_desc, &p_count);
    if(p_count!=1 || ret!=VBS_SUCCESS)
      return VBS_BADSTREAM;   

    if(codebook_lookup_type == 1)
      codebook_lookup_values = lookup1_values(codebook_entries, codebook_dimensions);
    else
      codebook_lookup_values = codebook_entries*codebook_dimensions;
    
    codebook_multiplicands = calloc(codebook_lookup_values, sizeof(uint32_t));
    
    for(uint32_t value = 0; value<codebook_lookup_values; value++){
      ret = vorbis_read_nbits(codebook_value_bits, &codebook_multiplicands[value], stream->io_desc, &p_count);
      if(p_count!=codebook_value_bits || ret!=VBS_SUCCESS)
	return VBS_BADSTREAM;  
    }

  }
      

  /***************      construction des VQ associés a chaque feuille        ***************/
  
  if(codebook_lookup_type == 1){
    codebook_temp->tree->vq_vector = calloc(codebook_entries, sizeof(sample_t*));
    for(entry = 0; entry<codebook_entries; entry++)
      codebook_temp->tree->vq_vector[entry] = construction_VQ_type1(entry, codebook_dimensions, codebook_multiplicands, codebook_lookup_values, codebook_delta_value, codebook_minimum_value, codebook_sequence_p); 
    free(codebook_multiplicands);
    
  }else if(codebook_lookup_type == 2){
    codebook_temp->tree->vq_vector = calloc(codebook_entries, sizeof(sample_t*));
    for(entry = 0; entry<codebook_entries; entry++)
      codebook_temp->tree->vq_vector[entry] = construction_VQ_type2(entry, codebook_dimensions, codebook_multiplicands, codebook_delta_value, codebook_minimum_value, codebook_sequence_p);
    free(codebook_multiplicands);
    
  }else if(codebook_lookup_type == 0){
      codebook_temp->tree->vq_vector = NULL;
  }

  
  free(codeword_lengths);
  
  *codebook = (codebook_t*)codebook_temp;
  return VBS_SUCCESS;
}




status_t codebook_setup_init(vorbis_stream_t * stream, codebook_setup_t ** pset){

  /***** variables premiere initialisation *****/
  uint32_t p_count, vorbis_codebook_count;
  status_t ret;

  /****************      premiere initialisation       ****************/
  
  ret = vorbis_read_nbits(8, &vorbis_codebook_count, stream->io_desc, &p_count);
  if(p_count!=8 || ret!=VBS_SUCCESS) 
    return VBS_BADSTREAM;
  vorbis_codebook_count++;
  
  *pset = NULL;
  *pset = malloc(sizeof(codebook_setup_t));
  if(*pset == NULL)
    return VBS_OUTOFMEMORY;
  
  (*pset)->codebooks = NULL;
  (*pset)->codebooks = calloc(vorbis_codebook_count, sizeof(codebook_1_t*));
  if((*pset)->codebooks == NULL)
    return VBS_OUTOFMEMORY;

  (*pset)->nb_cb = vorbis_codebook_count;
  
  
    
/*                 ####################################################
		   |                                                  |
                   |            lecture de chaque codebook            |
	           |                                                  |
                   ####################################################
*/
  
  for(uint32_t codebook_id = 0; codebook_id<vorbis_codebook_count; codebook_id++){
    
    ret = codebook_decode(stream, &(*pset)->codebooks[codebook_id], codebook_id);
    if(ret == VBS_BADSTREAM)
      return VBS_BADSTREAM;
    
  }
  
  return VBS_SUCCESS;
}




void free_arbre(codebook_node_t *node){
  
  if (node != NULL){
    if(node->fils_droit==NULL && node->fils_gauche==NULL){
      free(node);
    }else{ 
      free_arbre(node->fils_gauche);
      free_arbre(node->fils_droit);
      free(node);
    }
  }else{
    free(node);
  }
}



void codebooks_free(codebook_setup_t * cb_desc){
  codebook_1_t *codebook_temp;

  for(uint32_t i = 0; i<cb_desc->nb_cb; i++){
    codebook_temp = (codebook_1_t*)cb_desc->codebooks[i];
    
    free_arbre(codebook_temp->tree->head);

    if(codebook_temp->tree->type == 1){
      for(uint32_t j = 0; j<codebook_temp->tree->codebook_entries; j++){
	free(codebook_temp->tree->vq_vector[j]);
      }
    }
    
    free(codebook_temp->tree->vq_vector);
    free(codebook_temp->tree);
    free(codebook_temp);
  }

  free(cb_desc->codebooks);
  free(cb_desc);
}



uint32_t codebook_translate_scalar(vorbis_stream_t * stream, codebook_t * book, uint32_t *scalar){
  uint32_t valeur, ret, p_count, bit;
  codebook_1_t *book_temp = (codebook_1_t*)book;
  codebook_node_t *noeud_courant = book_temp->tree->head;
  valeur = 0;
  
  while(noeud_courant->est_une_feuille == 0){
    ret = vorbis_read_nbits(1, &bit, stream->io_desc, &p_count);
    if(p_count!=1 || ret!=VBS_SUCCESS) 
      return VBS_BADSTREAM;
    
    if(bit == 0){
      noeud_courant = noeud_courant->fils_gauche;    
    }else{
      noeud_courant = noeud_courant->fils_droit;
    }
    valeur++;
  }
  
  *scalar = noeud_courant->scalar;
  return valeur;
}




uint32_t codebook_translate_vq(vorbis_stream_t * stream, codebook_t * book, int *sz, sample_t **vector){
  uint32_t valeur, scalar;
  codebook_1_t *book_temp = (codebook_1_t*)book;
  
  valeur = codebook_translate_scalar(stream, book, &scalar);
  if(valeur == (uint32_t)VBS_BADSTREAM)
    return VBS_BADSTREAM;
  
  *sz = book_temp->tree->codebook_dimensions;
  *vector = book_temp->tree->vq_vector[scalar];
  
  return valeur;
}




uint32_t codebook_get_dimension(codebook_t * book){
  codebook_1_t *book_temp = (codebook_1_t*)book;
  return book_temp->tree->codebook_dimensions;
}
