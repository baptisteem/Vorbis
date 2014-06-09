#include "floor.h"
#include "floor0.h"
#include "floor1.h"
#include <stdio.h>

#define FLOOR_COUNT_BITS 6
#define FLOOR_TYPE_BITS 16

status_t floor_decode(vorbis_stream_t *stream, floor_t *floor, sample_t *v, uint16_t v_size){

  return floor->decode(stream, floor, v, v_size);

}

status_t floors_setup_init(vorbis_stream_t *stream, floors_setup_t **pset){
  uint32_t tmp = 0;
  uint32_t null = 0;
  //Return var
  status_t ret = VBS_SUCCESS;

  //Init pset
  (*pset) = malloc(sizeof(floors_setup_t));

  //Read floor count
  ret = vorbis_read_nbits(FLOOR_COUNT_BITS, &tmp, stream->io_desc, &null);
  if(ret != VBS_SUCCESS)
    return ret;

  (*pset)->floor_count = (uint8_t)tmp + 1;

  //Init floor data0
  floor_type0_data_new(&(*pset)->data0,stream->codec->blocksize);

  //Init floor data1
  floor_type1_data_new(&(*pset)->data1,stream->codec->blocksize);

  //Malloc for floor array
  (*pset)->floors = malloc( (*pset)->floor_count * sizeof(floor_t*));

  for(uint32_t i=0;i<(*pset)->floor_count;i++){

    //Read floor type
    ret = vorbis_read_nbits(FLOOR_TYPE_BITS, &tmp, stream->io_desc, &null);
    if(ret != VBS_SUCCESS)
      return ret;

    //If it's a type 0 floor
    if(tmp == FLOOR_TYPE0){
      floor_type0_hdr_decode(stream, (uint8_t)i, (*pset)->floors ,(*pset)->data0);
      ((*pset)->floors[i])->id = (uint8_t)i;
      ((*pset)->floors[i])->type = FLOOR_TYPE0;
      ((*pset)->floors[i])->decode = floor_type0_decode;
    }
    else if(tmp == FLOOR_TYPE1){
      floor_type1_hdr_decode(stream, (uint8_t)i, (*pset)->floors ,(*pset)->data1);
      ((*pset)->floors[i])->id = (uint8_t)i;
      ((*pset)->floors[i])->type = FLOOR_TYPE1;
      ((*pset)->floors[i])->decode = (void *)floor_type1_decode;
    }
  }  
  floor_type1_data_allocate((*pset)->data1);
  floor_type0_data_allocate((*pset)->data0);


  return VBS_SUCCESS;
}

void floors_free(floors_setup_t *set){

  floor_type0_data_free(set->data0);
  floor_type1_data_free(set->data1);

  for(uint32_t i=0;i<set->floor_count;i++){
    set->floors[i]->free(set->floors[i]);
  }
  free(set->floors);
  free(set);
}
