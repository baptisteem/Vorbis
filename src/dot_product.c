#include <stdint.h>
#include "dot_product.h"


status_t dot_product(sample_t **fsamp, sample_t **residues, uint32_t nb_chan,
                     uint32_t n2){

  for (int i = 0; i<nb_chan; i++){
    for (int j = 0; j<n2; j++){
      *(*(sample_t + i) + j) = (*(*(sample_t + i) + j)) * (*(*(residues + i) + j));
    } 
  }

  return VBS_SUCCESS;
}
