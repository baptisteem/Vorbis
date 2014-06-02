#include "mode.h"
#include "error.h"
#include <stdio.h>

status_t window_modes_setup_init(vorbis_stream_t *stream, window_modes_setup_t **pset){

  status_t return_status = VBS_SUCCESS;

  (*pset) = malloc(sizeof(window_modes_setup_t));

  uint32_t *tmp = malloc(sizeof(uint32_t));
  uint32_t null = 0;
  
  //We read the number of mode and check if we have valid data
  if(vorbis_read_nbits(MODE_COUNT_BITS, tmp, stream->io_desc, &null) == VBS_SUCCESS) {
   
    (*pset)->mode_count = (uint8_t) *tmp + 1;

    (*pset)->modes = malloc((*pset)->mode_count * sizeof(window_mode_t));
    if((*pset)->modes != NULL){
      
      for(uint32_t i=0;i<(*pset)->mode_count;i++){
      
        //Mode blockflag 
        vorbis_read_nbits(MODE_FLAG_BITS, tmp, stream->io_desc, &null);
        ((*pset)->modes[i]).blockflag = (uint8_t) *tmp;
        
        //Mode windowtype
        vorbis_read_nbits(MODE_WINDOW_BITS, tmp, stream->io_desc, &null);
        ((*pset)->modes[i]).window_type = (uint16_t) *tmp;
        
        //Mode transformtype
        vorbis_read_nbits(MODE_TRANSFORM_BITS, tmp, stream->io_desc, &null);
        ((*pset)->modes[i]).transform_type = (uint16_t) *tmp;
        
        //Mode mapping
        vorbis_read_nbits(MODE_MAPPING_BITS, tmp, stream->io_desc, &null);
        ((*pset)->modes[i]).mapping = malloc(sizeof(mapping_t));
        ((*pset)->modes[i]).mapping->type = MAPPING_TYPE0;
        ((*pset)->modes[i]).mapping->id = (uint8_t) *tmp;
        
        printf("Tmp : %d\n", (uint8_t) *tmp);
        fflush(stdout);
        
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
  free(set);
}
