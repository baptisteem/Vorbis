#include "envelope.h"
#include <stdio.h>

envelope_t *envelope_init(uint16_t *blocksize){

}

void envelope_free(envelope_t *env){

}

status_t envelope_prepare(envelope_t *env, sample_t *filter){

}

uint16_t envelope_overlap_add(envelope_t *env, sample_t *in,
                              sample_t *cache, sample_t *out){

}
