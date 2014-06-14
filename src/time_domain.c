#include "time_domain.h"
#include <math.h>
#include "sample.h"
#include "vorbis_io.h"
#include <stdio.h>
#include <stdlib.h>



struct time_domain_transform1 {

  time_domain_transform_t base;
  uint16_t *blocksize;
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
  *ptdt=(time_domain_transform_t *)(ptdt1);

  return VBS_SUCCESS;	
}


void time_domain_transforms_free(time_domain_transform_t *tdt){
  time_domain_transform1_t *tdt1=(time_domain_transform1_t *) tdt;
  
  free(tdt1->blocksize);
  free(tdt1);
}


status_t time_domain_transform_process(time_domain_transform_t *tdt, sample_t *fsamp,
    sample_t *tsamp, sample_t *filter, int mode){

  time_domain_transform1_t *tdt1=(time_domain_transform1_t *)tdt;
  uint32_t n=(tdt1->blocksize)[mode];

  sample_t temp, aux;

  for (uint32_t i=0; i<n; i++){
    temp=(sample_t)0;
    for (uint32_t j=0; j<(n/2); j++){

      aux  = (sample_t)(M_PI_2*(2*j+1)*((2*i +1) + 0.5*n))/n;
      aux =cos(aux);
      aux *=fsamp[j];
      //add aux to the previous sum 
      temp +=aux;

    }
    temp *=filter[i];
    tsamp[i]=temp;
  }
  return VBS_SUCCESS;	
}
