#include "vorbis_headers.h"
#include <stdlib.h>

// verifier ce qu'est io
status_t vorbis_common_header(vorbis_stream_t *stream, uint8_t *header_type){
  char vorbis[6];
  status_t bool;
  uint32_t *p_count; 

  bool = vorbis_read_nbits(1, dst, io, p_count);
  if ((bool == VBS_SUCESS)&&(*p_count==1)){
    if (dst != 1) return VBS_BADSTREAM;
  }else{
    return VBS_BADSTREAM;
  }
  
  bool = vorbis_read_nbits(7, dst, io, p_count);
  if ((bool == VBS_SUCESS)&&(*p_count==7)){
    if (dst != uint32_t(header_type)) return VBS_BADSTREAM;
  }else{
    return VBS_BADSTREAM;
  }
  

  for(int i = 0; i<6; i++){
    bool = vorbis_read_nbits(8, dst, io, p_count)
    vorbis[i] = char(dst);
      }
  
  if ((bool == VBS_SUCCESS)&&(*p_count==1))
    if (vorbis != "vorbis") return VBS_BADSTREAM;
  }else{
    return VBS_BADSTREAM; 
  }
      
return VBS_SUCCESS
}


status_t vorbis_header1_decode(vorbis_stream_t *stream){
  vorbis_common_header(vorbis_stream_t *stream, 0);

  
  }
