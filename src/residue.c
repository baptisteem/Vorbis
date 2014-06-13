#include <stdint.h>
#include<stdio.h>
#include <vorbis.h>
#include <sample.h>
#include <error.h>
#include "residue.h"

typedef struct residue_1 residue_1_t;

struct residue_1 {
  residue_t base;

  uint32_t residue_begin;
  uint32_t residue_end;
  uint32_t residue_partition_size;
  uint32_t residue_classifications;
  codebook_t *residue_classbook;
  codebook_t ***residue_book;
}residue_1;

status_t residues_setup_init(vorbis_stream_t * stream, residues_setup_t ** pres){
  uint32_t ret, p_count, residue_count_temp, type_temp;

  /*************   variables Decodage de l'entete (partie commune)   ****************/
  residue_1_t *residue_temp = NULL;
  uint32_t hight_bits, low_bits, bitflag, dst;
  uint32_t *residue_cascade = NULL;


  /*
####################################################
|                                                  |
|           Initialisation et allocation           |
|                                                  |
####################################################
*/


  *pres = malloc(sizeof(residues_setup_t));
  if((pres==NULL) || (*pres==NULL))
    return VBS_OUTOFMEMORY;


  ret = vorbis_read_nbits(6, &residue_count_temp, stream->io_desc, &p_count);
  if((p_count!=6)||(ret!=VBS_SUCCESS)) 
    return VBS_BADSTREAM;
  residue_count_temp++;
  (*pres)->residue_count = residue_count_temp;

  (*pres)->residues = calloc(residue_count_temp, sizeof(residue_1_t*));
  if((*pres)->residues==NULL)
    return VBS_OUTOFMEMORY;



  /*
####################################################
|                                                  |
|      Decodage de l'entete (partie commune)       |
|                                                  |
####################################################
*/


  for(uint32_t i = 0; i<(*pres)->residue_count; i++){

    residue_temp = malloc(sizeof(residue_1_t));
    if(residue_temp==NULL)
      return VBS_OUTOFMEMORY;

    ret = vorbis_read_nbits(16, &type_temp, stream->io_desc, &p_count);
    if((p_count!=16)||(ret!=VBS_SUCCESS)) 
      return VBS_BADSTREAM;

    if(type_temp>2)
      return VBS_BADSTREAM;


    residue_temp->base.type = type_temp;
    residue_temp->base.free = (void *)residues_free;
    residue_temp->base.decode = (void *)residue_decode;

    ret = vorbis_read_nbits(24, &residue_temp->residue_begin, stream->io_desc, &p_count);
    if(p_count!=24 || ret!=VBS_SUCCESS) 
      return VBS_BADSTREAM;

    ret = vorbis_read_nbits(24, &residue_temp->residue_end, stream->io_desc, &p_count);
    if((p_count!=24)||(ret!=VBS_SUCCESS)) 
      return VBS_BADSTREAM;

    ret = vorbis_read_nbits(24, &residue_temp->residue_partition_size, stream->io_desc, &p_count);
    if((p_count!=24)||(ret!=VBS_SUCCESS)) 
      return VBS_BADSTREAM;
    residue_temp->residue_partition_size++;

    ret = vorbis_read_nbits(6, &residue_temp->residue_classifications, stream->io_desc, &p_count);
    if((p_count!=6)||(ret!=VBS_SUCCESS))
      return VBS_BADSTREAM;
    residue_temp->residue_classifications++;

    ret = vorbis_read_nbits(8, &dst, stream->io_desc, &p_count);
    residue_temp->residue_classbook = stream->codec->codebooks_desc->codebooks[dst];
    if((p_count!=8)||(ret!=VBS_SUCCESS)) 
      return VBS_BADSTREAM;

    residue_cascade = calloc(residue_temp->residue_classifications, sizeof(uint32_t));

    residue_temp->residue_book = calloc(residue_temp->residue_classifications, sizeof(codebook_t**));
    for(uint32_t j = 0; j<residue_temp->residue_classifications; j++){
      residue_temp->residue_book[j] = calloc(8, sizeof(codebook_t*));
    }

    for(uint32_t k = 0; k<residue_temp->residue_classifications; k++){ 
      hight_bits = 0;
      ret = vorbis_read_nbits(3, &low_bits, stream->io_desc, &p_count);
      if((p_count!=3)||(ret!=VBS_SUCCESS))
        return VBS_BADSTREAM;

      ret = vorbis_read_nbits(1, &bitflag, stream->io_desc, &p_count);
      if((p_count!=1)||(ret!=VBS_SUCCESS)) 
        return VBS_BADSTREAM;

      if (bitflag==1){
        ret = vorbis_read_nbits(5, &hight_bits, stream->io_desc, &p_count);
        if((p_count!=5)||(ret!=VBS_SUCCESS))
          return VBS_BADSTREAM;
      }
      residue_cascade[k] = hight_bits*8 + low_bits;
    }


    for(uint32_t k = 0; k<residue_temp->residue_classifications; k++){
      for(uint32_t j = 0; j<8; j++){     

        if((residue_cascade[k]&(1<<j))!=0){
          ret = vorbis_read_nbits(8, &dst, stream->io_desc, &p_count);
          residue_temp->residue_book[k][j] = stream->codec->codebooks_desc->codebooks[dst];
          if((p_count!=8)||(ret!=VBS_SUCCESS))
            return VBS_BADSTREAM;
        }else{
          residue_temp->residue_book[k][j] = NULL; // convention pour <unused>
        }	  
      } 
    }

    free(residue_cascade);
    (*pres)->residues[i] = (residue_t*)residue_temp;
  } 

  return VBS_SUCCESS; 
}



void residues_free(residues_setup_t * set){
  residue_1_t *residue_1_temp;

  for(uint32_t i = 0; i<set->residue_count; i++){
    residue_1_temp = (residue_1_t*)(set->residues[i]);
    free(*residue_1_temp->residue_book);
    free(residue_1_temp);
  }
  free(set->residues);
  free(set);
}



status_t decode_residue_type0(vorbis_stream_t *stream, codebook_t *book, uint32_t n, sample_t *v, uint32_t offset, uint32_t codebook_dim){
  uint32_t step = n/codebook_dim;  
  sample_t *entry_temp = NULL; 
  int32_t test; 
  int taille;

  for(uint32_t i = 0; i<step; i++){
    test = codebook_translate_vq(stream, book, &taille, &entry_temp);
    if(test==VBS_BADSTREAM)
      return VBS_BADSTREAM;

    for(uint32_t j = 0; j<codebook_dim; j++){
      v[offset+i+j*step] = v[offset+i+j*step] + entry_temp[j];
    }
  }
  return VBS_SUCCESS;
}



status_t decode_residue_type1(vorbis_stream_t *stream, codebook_t *book, uint32_t n, sample_t *v, uint32_t offset, uint32_t codebook_dim){
  sample_t *entry_temp= NULL;
  int32_t test;
  int taille;

  for(uint32_t i = 0; i<n;){ 
    test = codebook_translate_vq(stream, book, &taille, &entry_temp);
    if(test==VBS_BADSTREAM)
      return VBS_BADSTREAM;   

    for(uint32_t j = 0; j<codebook_dim; j++){
      v[offset+i] = v[offset+i] + entry_temp[j] ;
      i++; 
    }
  }
  return VBS_SUCCESS;
}




status_t residue_decode(vorbis_stream_t * stream, residue_t * residue, int ch, int64_t N2, sample_t ** v, uint8_t * do_not_decode){


  residue_1_t *residue_1 = (residue_1_t*)residue;

  /**************   variables decodage du paquet audio (partie commune)   ***************/
  status_t ret;
  uint32_t actual_size = (stream->codec->blocksize[stream->codec->modes_desc->modes->blockflag])/2;
  uint32_t limit_residue_begin, limit_residue_end, partition_to_read, n_to_read, classwords_per_codeword;

  /**************     variables Fin initialisation, debut des boucles     ***************/
  uint32_t vqclass, offset, codebook_dim, temp = 0;
  codebook_t *vqbook;
  uint32_t **classification = calloc(ch, sizeof(uint32_t*));
  sample_t *vbis;
  int chbis = ch;
  int64_t N2bis = N2;



  /*
##################################################
|                                                |
|    decodage du paquet audio (partie commune)   |
|                                                |
##################################################
*/

  if(residue_1->base.type==2){
    actual_size = N2*ch;
    vbis = calloc(N2*ch, sizeof(sample_t));
    N2 = N2*ch;
    ch = 1;
  }

  if(residue_1->residue_begin<actual_size)
    limit_residue_begin = residue_1->residue_begin;
  else
    limit_residue_begin = actual_size;

  if(residue_1->residue_end<actual_size)
    limit_residue_end = residue_1->residue_end;
  else
    limit_residue_end = actual_size; 

  classwords_per_codeword = codebook_get_dimension(residue_1->residue_classbook);
  n_to_read = limit_residue_end - limit_residue_begin;
  partition_to_read = (n_to_read/residue_1->residue_partition_size); 

  for(int32_t i = 0; i<ch; i++){
    classification[i] = calloc((partition_to_read + classwords_per_codeword), sizeof(uint32_t));
  }


  /*
###################################################
|                                                 |
|    fin des initialisation debut des boucles     |
|                                                 |
###################################################
*/



  if(n_to_read==0)
    return VBS_SUCCESS;



  /**************        Premiere partie initialisation          ************/

  for(uint32_t pass = 0; pass<8; pass++){ 
    uint32_t partition_count = 0;
    while(partition_count<partition_to_read){


      if(pass==0){
        for(int32_t j = 0; j<ch; j++){
          if(do_not_decode[j]==0){
            ret = codebook_translate_scalar(stream, residue_1->residue_classbook, &temp);
            if(ret==VBS_BADSTREAM)
              return VBS_BADSTREAM;	    
            for(int32_t i = (classwords_per_codeword-1); i!=(-1); i--){
              classification[j][i + partition_count] = (temp % residue_1->residue_classifications);
              temp = temp/residue_1->residue_classifications;
            }
          }
        }
      }


      /**************        deuxieme partie decodage          ************/


      for(uint32_t i = 0; (i<classwords_per_codeword)&&(partition_count<partition_to_read);i++){
        for(int32_t j = 0; j<ch; j++){

          if(do_not_decode[j]==0){
            vqclass = classification[j][partition_count];
            vqbook = residue_1->residue_book[vqclass][pass];
            if(vqbook!=NULL){  // convention pour <unused>
              offset = limit_residue_begin + partition_count*residue_1->residue_partition_size;
              codebook_dim = codebook_get_dimension(vqbook);

              switch(residue_1->base.type){
                case 0 : 
                  ret = decode_residue_type0(stream, vqbook, residue_1->residue_partition_size, *(v+j), offset, codebook_dim);
                  break;
                case 1 : 
                  ret = decode_residue_type1(stream, vqbook, residue_1->residue_partition_size, *(v+j), offset, codebook_dim);
                  break;
                case 2 : 
                  ret = decode_residue_type1(stream, vqbook, residue_1->residue_partition_size, vbis, offset, codebook_dim);
                  break;
                default :
                  return VBS_BADSTREAM;
              }
              if(ret==VBS_BADSTREAM)
                return VBS_BADSTREAM;
            }
          }

        }
        partition_count++;
      }   


    }
  }


  /*
###################################################
|                                                 |
|     finitions et liberation de la memoire       |
|                                                 |
###################################################
*/




  if(residue_1->base.type==2){
    for(uint32_t i = 0; i<N2bis; i++){
      for(int32_t j = 0; j<chbis; j++){
        v[j][i] = vbis[i*chbis+j];
      }
    }
  }

  for(int32_t i = 0; i<ch; i++){
    free(classification[i]);
  } 
  free(vbis);

  return VBS_SUCCESS;
}


