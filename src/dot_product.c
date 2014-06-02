#include <stdint.h>
#include "dot_product.h"


status_t dot_product(sample_t **fsamp, sample_t **residues, uint32_t nb_chan,
                     uint32_t n2){
  double f, r;
  double *addr_f;
  
  for (uint32_t i = 0; i<nb_chan; i++){
    for (uint32_t j = 0; j<n2; j++){
      addr_f = *(fsamp +i) + j;
      f = *addr_f;
      r = *(*(residues +i) + j);
      *addr_f= f*r;
    } 
  }

  return VBS_SUCCESS;
}
