#include "time_domain.h"
#include <math.h>
#include "sample.h"
#include "vorbis_io.h"
#include <stdio.h>
#include <stdlib.h>
#include "fast_imdc.h"


struct time_domain_transform1 {

  time_domain_transform_t base;
  uint16_t *blocksize;
  ensemble_matrice_t *Ens;
};

typedef struct time_domain_transform1 time_domain_transform1_t;

status_t time_domain_transforms_setup_init(vorbis_stream_t *stream, time_domain_transform_t **ptdt){

  //forward in the flux
  status_t stat;
  uint32_t p_count, dst, time_count;
  uint16_t *block;
  time_domain_transform1_t *ptdt1;

  stat = vorbis_read_nbits(6, &dst, stream->io_desc, &p_count);
  if (stat==VBS_BADSTREAM || p_count!=6) return VBS_BADSTREAM;

  time_count = dst;
  time_count ++;

  
  for (uint32_t i=0; i<time_count; i++){

    stat = vorbis_read_nbits(16, &dst, stream->io_desc, &p_count);
    if (stat==VBS_BADSTREAM || p_count!=16 || dst != 0) 
      return VBS_BADSTREAM;
  }	

  //creation of structure time_domain_transform_t
  block=calloc(2,sizeof(uint16_t));

  for (uint32_t i=0; i<2; i++){
    block[i]=(stream->codec->blocksize)[i];
  }

  ptdt1=malloc(sizeof(time_domain_transform1_t));
  ptdt1->base.type=0;
  ptdt1->blocksize=block;
  initialisation_matrices(&ptdt1->Ens, stream);
  
  *ptdt=(time_domain_transform_t *)(ptdt1);

  return VBS_SUCCESS;	
}


void time_domain_transforms_free(time_domain_transform_t *tdt){
  time_domain_transform1_t *tdt1=(time_domain_transform1_t *) tdt;
  
  matrices_free(tdt1->Ens);
  free(tdt1->blocksize);
  free(tdt1);
}


status_t time_domain_transform_process(time_domain_transform_t *tdt, sample_t *fsamp, sample_t *tsamp, sample_t *filter, int mode){
  
  time_domain_transform1_t *tdt1=(time_domain_transform1_t *)tdt;
  uint32_t n=(tdt1->blocksize)[mode];
  status_t stat;
  sample_t *tsamp_temp = calloc(n/2, sizeof(sample_t));

  switch(mode){
  case 0 :
    stat = multiplication_classique(tdt1->Ens->M_blocsize_0, fsamp, tsamp_temp);
    stat = multiplication_P(tdt1->Ens->M_blocsize_0, tsamp_temp, tsamp);
    break;
  case 1 : 
    stat = multiplication_classique(tdt1->Ens->M_blocsize_1, fsamp, tsamp_temp);
    stat = multiplication_P(tdt1->Ens->M_blocsize_1, tsamp_temp, tsamp);
    break;
  }

  if(stat != VBS_SUCCESS)
    return stat;

  for (uint32_t i=0; i<n; i++){
   tsamp[i] *= filter[i];
  }

  free(tsamp_temp);

  return VBS_SUCCESS;	
}
