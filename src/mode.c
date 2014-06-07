#include "mode.h"
#include "error.h"
#include "helpers.h"
#include <stdio.h>

status_t window_modes_setup_init(vorbis_stream_t *stream, window_modes_setup_t **pset){

  status_t return_status = VBS_SUCCESS;

  (*pset) = malloc(sizeof(window_modes_setup_t));

  uint32_t tmp = 0;
  uint32_t null = 0;
  
  //We read the number of mode and check if we have valid data
  if(vorbis_read_nbits(MODE_COUNT_BITS, &tmp, stream->io_desc, &null) == VBS_SUCCESS) {
   
    (*pset)->mode_count = (uint8_t) tmp + 1;
    (*pset)->mode_code_nbbits = ilog((*pset)->mode_count - 1);

    (*pset)->modes = malloc((*pset)->mode_count * sizeof(window_mode_t));
    if((*pset)->modes != NULL){
      
      for(uint32_t i=0;i<(*pset)->mode_count;i++){
      
        //Mode blockflag 
        vorbis_read_nbits(MODE_FLAG_BITS, &tmp, stream->io_desc, &null);
        ((*pset)->modes[i]).blockflag = (uint8_t) tmp;
        
        //Mode windowtype
        vorbis_read_nbits(MODE_WINDOW_BITS, &tmp, stream->io_desc, &null);
        ((*pset)->modes[i]).window_type = (uint16_t) tmp;
        
        //Mode transformtype
        vorbis_read_nbits(MODE_TRANSFORM_BITS, &tmp, stream->io_desc, &null);
        ((*pset)->modes[i]).transform_type = (uint16_t) tmp;
        
        //Mode mapping
        vorbis_read_nbits(MODE_MAPPING_BITS, &tmp, stream->io_desc, &null);
       
        uint8_t index = (uint8_t) tmp;
        ((*pset)->modes[i]).mapping = (stream->codec->mappings_desc)->maps[index];

        //TO DO : Confirme that the last condition id ok
        if(((*pset)->modes[i]).window_type != 0 ||
           ((*pset)->modes[i]).transform_type != 0  ||
           ((*pset)->modes[i]).mapping->id >= (*pset)->mode_count){
          
          return_status = VBS_BADSTREAM;
        }
      }
    }
    else
      ;//TO DO : Add error
  }
  else
    ;//TO DO : Add error

  return return_status;
}

void window_modes_free(window_modes_setup_t *set){
  free(set->modes);
  free(set);
}
