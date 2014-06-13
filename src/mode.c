#include "mode.h"
#include "error.h"
#include "helpers.h"
#include <stdio.h>

#define MODE_COUNT_BITS 6
#define MODE_FLAG_BITS 1
#define MODE_WINDOW_BITS 16
#define MODE_TRANSFORM_BITS 16
#define MODE_MAPPING_BITS 8
    

status_t window_modes_setup_init(vorbis_stream_t *stream, window_modes_setup_t **pset){

  status_t ret;

  (*pset) = malloc(sizeof(window_modes_setup_t));

  uint32_t tmp = 0;
  uint32_t null = 0;

  //We read the number of mode and check if we have valid data
  ret = vorbis_read_nbits(MODE_COUNT_BITS, &tmp, stream->io_desc, &null);
  if(ret != VBS_SUCCESS)
    return ret;

  (*pset)->mode_count = (uint8_t) tmp + 1;
  (*pset)->mode_code_nbbits = ilog((*pset)->mode_count - 1);

  (*pset)->modes = malloc((*pset)->mode_count * sizeof(window_mode_t));
  if((*pset)->modes != NULL){

    for(uint32_t i=0;i<(*pset)->mode_count;i++){

      //Mode blockflag 
      ret = vorbis_read_nbits(MODE_FLAG_BITS, &tmp, stream->io_desc, &null);
      if(ret != VBS_SUCCESS)
        return ret;

      ((*pset)->modes[i]).blockflag = (uint8_t) tmp;

      //Mode windowtype
      ret = vorbis_read_nbits(MODE_WINDOW_BITS, &tmp, stream->io_desc, &null);
      if(ret != VBS_SUCCESS)
        return ret;
      ((*pset)->modes[i]).window_type = (uint16_t) tmp;

      //Mode transformtype
      ret = vorbis_read_nbits(MODE_TRANSFORM_BITS, &tmp, stream->io_desc, &null);
      if(ret != VBS_SUCCESS)
        return ret;
      ((*pset)->modes[i]).transform_type = (uint16_t) tmp;

      //Mode mapping
      ret = vorbis_read_nbits(MODE_MAPPING_BITS, &tmp, stream->io_desc, &null);
      if(ret != VBS_SUCCESS)
        return ret;

      uint8_t index = (uint8_t) tmp;
      ((*pset)->modes[i]).mapping = (stream->codec->mappings_desc)->maps[index];

      //TO DO : Confirme that the last condition id ok
      if(((*pset)->modes[i]).window_type != 0 ||
          ((*pset)->modes[i]).transform_type != 0  ||
          ((*pset)->modes[i]).mapping->id >= (*pset)->mode_count)
        return VBS_BADSTREAM;
    }
  }
  else
    return VBS_BADSTREAM;

  return VBS_SUCCESS;
}

void window_modes_free(window_modes_setup_t *set){
  free(set->modes);
  free(set);
}
