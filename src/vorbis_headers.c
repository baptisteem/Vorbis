#include "vorbis_io.h"
#include "vorbis_headers.h"
#include <stdlib.h>
#include <stdio.h>


status_t vorbis_common_header(vorbis_stream_t *stream, uint8_t *header_type){
  char vorbis[6];
  status_t bool;
  uint32_t p_count, dst; 

  if (stream->io_desc==NULL) return VBS_BADSTREAM;

  bool = vorbis_read_nbits(1, &dst, stream->io_desc, &p_count);
  if ((bool==VBS_SUCCESS)&&(p_count==1)){
    if (dst != 1) return VBS_BADSTREAM;
  }else{
    return VBS_BADSTREAM;
  }
  
  bool = vorbis_read_nbits(7, &dst, stream->io_desc, &p_count);
  if ((bool==VBS_SUCCESS)&&(p_count==7)){
    *header_type = (uint8_t)dst;
  }else{
    return VBS_BADSTREAM;
  }
  
  
  for(uint32_t i=0; i<6; i++){
    bool = vorbis_read_nbits(8, &dst, stream->io_desc, &p_count);
    vorbis[i] = dst;
  }
  
  if ((bool==VBS_SUCCESS)&&(p_count==8)){
    char c[6] = "vorbis";
    if (strcmp(vorbis, c) != 0) return VBS_BADSTREAM;
  }else{
    return VBS_BADSTREAM; 
  }
  
  return VBS_SUCCESS;
}


status_t vorbis_header1_decode(vorbis_stream_t *stream){
  uint8_t *header_type =  malloc(sizeof(uint8_t));
  status_t bool;

  bool = vorbis_common_header(stream, header_type);
  if ((*header_type != 0) || (bool==VBS_BADSTREAM)) return VBS_BADSTREAM; 

  uint32_t p_count, dst, dst_bis; 
  
  bool = vorbis_read_nbits(32, &dst, stream->io_desc, &p_count); // vorbis_version
  if ((bool==VBS_BADSTREAM) || (dst!=0)) return VBS_BADSTREAM;
  stream->codec->vorbis_version = dst;
  
  bool = vorbis_read_nbits(8, &dst, stream->io_desc, &p_count); // audio_channels
  if ((bool==VBS_BADSTREAM) || (dst=0)) return VBS_BADSTREAM;
  stream->codec->audio_channels = (uint8_t)dst;
  
  bool = vorbis_read_nbits(32, &dst, stream->io_desc, &p_count); // audio_sample_rate
  if ((bool==VBS_BADSTREAM) || (dst=0)) return VBS_BADSTREAM;
  stream->codec->audio_sample_rate = dst;  
  
  bool = vorbis_read_nbits(32, &dst, stream->io_desc, &p_count); // bitrate_maximum
  if (bool==VBS_BADSTREAM) return VBS_BADSTREAM;
  stream->codec->bitrate_maximum = (int32_t)dst;   

  bool = vorbis_read_nbits(32, &dst, stream->io_desc, &p_count); // bitrate_nominal
  if (bool==VBS_BADSTREAM) return VBS_BADSTREAM; 
  stream->codec->bitrate_nominal = (int32_t)dst;
  
  bool = vorbis_read_nbits(32, &dst, stream->io_desc, &p_count); // bitrate_minimum
  if (bool==VBS_BADSTREAM) return VBS_BADSTREAM;
  stream->codec->bitrate_minimum = (int32_t)dst;
  
  bool = vorbis_read_nbits(4, &dst, stream->io_desc, &p_count); // blocksize[0]
  dst = pow(2, dst);
  if ((bool==VBS_BADSTREAM) || (dst<64) || (dst>8192)) return VBS_BADSTREAM;
  stream->codec->blocksize[0] = (uint16_t)dst;
  
  bool = vorbis_read_nbits(4, &dst_bis, stream->io_desc, &p_count); // blocksize[1]
  dst_bis = pow(2, dst_bis);
  if ((bool==VBS_BADSTREAM)||(dst>dst_bis)||(dst_bis>8192)) return VBS_BADSTREAM;
  stream->codec->blocksize[1] = (uint16_t)dst;
  
  bool = vorbis_read_nbits(1, &dst, stream->io_desc, &p_count); // framming_flag
  if ((bool==VBS_BADSTREAM) || (dst!=1)) return VBS_BADSTREAM;  
  

  free(header_type);
  return VBS_SUCCESS;
}


status_t vorbis_header2_decode(vorbis_stream_t *stream){
  uint8_t *header_type =  malloc(sizeof(uint8_t));
  status_t bool;
  
  bool = vorbis_common_header(stream, header_type);
  if ((*header_type != 1) || (bool==VBS_BADSTREAM)) return VBS_BADSTREAM; 
  
  uint32_t p_count, dst, dst_bis1, dst_bis2; 

  // vendor_length
  bool = vorbis_read_nbits(32, &dst, stream->io_desc, &p_count);
  if (bool==VBS_BADSTREAM) return VBS_BADSTREAM;

  // vendor_string
  char *chaine = calloc(dst, 8);

  for (uint32_t i = 0; i<dst; i++){
    bool = vorbis_read_nbits(8, &dst_bis1, stream->io_desc, &p_count);
    if (bool==VBS_BADSTREAM) return VBS_BADSTREAM;
    chaine[i] = dst_bis1;
  }
  printf("%s\n\n", chaine);
  fflush(stdout);
  free(chaine);

  // user_comment_list_length
  bool = vorbis_read_nbits(32, &dst_bis2, stream->io_desc, &p_count);
  if (bool==VBS_BADSTREAM) return VBS_BADSTREAM;
    
  for(uint32_t i = 0; i<dst_bis2; i++){

    // length_i
    bool = vorbis_read_nbits(32, &dst, stream->io_desc, &p_count);
    if (bool==VBS_BADSTREAM) return VBS_BADSTREAM;
    char *chaine = calloc(dst+3, 8);

    // user_comment_i
    for (uint32_t i = 0; i<(dst+3); i++){
      bool = vorbis_read_nbits(8, &dst_bis1, stream->io_desc, &p_count);
      if (bool==VBS_BADSTREAM) return VBS_BADSTREAM;
      
      if (dst_bis1!=61){          // si le caractere est different de '=' (0x3D)
	chaine[i] = dst_bis1;
      }else{
	chaine[i] = ' ';
	chaine[i+1] = ':';
	chaine[i+2] = ' ';
	chaine[i+3] = ' ';
	i = i+3;
      }
    }
    printf("%s\n", chaine);
    fflush(stdout);
    free(chaine);
  }
  printf("\n");
  fflush(stdout);
  
  bool = vorbis_read_nbits(1, &dst, stream->io_desc, &p_count); // framing_bit
  if ((bool==VBS_BADSTREAM) || (dst!=1)) return VBS_BADSTREAM;  
  
  free(header_type);
  return VBS_SUCCESS;
}
