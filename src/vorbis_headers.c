#include "vorbis_io.h"
#include "vorbis_headers.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>

status_t vorbis_common_header(vorbis_stream_t *stream, uint8_t *header_type){
  char vorbis[7] = "";
  status_t return_status;
  uint32_t p_count, dst; 
  char *c = "vorbis";
  
  fprintf(stderr,"Trace 0\n");
  return_status = vorbis_read_nbits(1, &dst, stream->io_desc, &p_count);
  
  if ((return_status==VBS_SUCCESS)&&(p_count==1)){
    if (dst != 1) {
      fprintf(stderr,"Trace 0.1. Dst : %d\n", dst);
      return VBS_BADSTREAM;
    }
  }else{
    return VBS_BADSTREAM;
  }

  fprintf(stderr,"---------Trace 1-----------\n");

  return_status = vorbis_read_nbits(7, &dst, stream->io_desc, &p_count);
  if (return_status==VBS_SUCCESS && p_count==7){
    *header_type = (uint8_t)dst;
  }else{
    return VBS_BADSTREAM;
  }

  fprintf(stderr,"header type : %d\n", *header_type);
  fprintf(stderr,"-------------Trace 2------------\n");

  for(uint32_t i=0; i<6; i++){
    return_status = vorbis_read_nbits(8, &dst, stream->io_desc, &p_count);
    vorbis[i] = dst;
    fprintf(stderr,"Lettre : %c\n",dst);
  }
  vorbis[6] = '\0';

  fprintf(stderr,"BIG VORBIS -------------> %s\n", vorbis);

  if (return_status==VBS_SUCCESS && p_count==8){
    if (strcmp(vorbis, c) != 0) 
      return VBS_BADSTREAM;
  }else{
    return VBS_BADSTREAM; 
  }

  return VBS_SUCCESS;
}


status_t vorbis_header1_decode(vorbis_stream_t *stream){
  uint8_t header_type = 0;
  uint32_t p_count, dst, dst_bis; 
  status_t return_status;

  fprintf(stderr,"Header 1\n");

  return_status = vorbis_common_header(stream, &header_type);
  if (header_type != 0 || return_status==VBS_BADSTREAM) 
    return VBS_BADSTREAM; 
  
  fprintf(stderr,"End common header\n");

  return_status = vorbis_read_nbits(32, &dst, stream->io_desc, &p_count); // vorbis_version
  if ((return_status==VBS_BADSTREAM) || (dst!=0)) 
    return VBS_BADSTREAM;
  stream->codec->vorbis_version = dst;
  dst = 0;

  return_status = vorbis_read_nbits(8, &dst, stream->io_desc, &p_count); // audio_channels
  if (return_status==VBS_BADSTREAM || dst <= 0)                  // strictement superieur a 0??
    return VBS_BADSTREAM;
  stream->codec->audio_channels = (uint8_t)dst;
  dst = 0;

  return_status = vorbis_read_nbits(32, &dst, stream->io_desc, &p_count); // audio_sample_rate
  if (return_status==VBS_BADSTREAM || dst <= 0)                   // strictement superieur a 0?? 
    return VBS_BADSTREAM;
  stream->codec->audio_sample_rate = dst;  
  dst = 0;

  return_status = vorbis_read_nbits(32, &dst, stream->io_desc, &p_count); // bitrate_maximum
  if (return_status==VBS_BADSTREAM) 
    return VBS_BADSTREAM;
  stream->codec->bitrate_maximum = dst; 
  dst = 0;  

  return_status = vorbis_read_nbits(32, &dst, stream->io_desc, &p_count); // bitrate_nominal
  if (return_status==VBS_BADSTREAM) 
    return VBS_BADSTREAM; 
  stream->codec->bitrate_nominal = dst;
  dst = 0;

  return_status = vorbis_read_nbits(32, &dst, stream->io_desc, &p_count); // bitrate_minimum
  if (return_status==VBS_BADSTREAM) 
    return VBS_BADSTREAM;
  stream->codec->bitrate_minimum = dst;
  dst = 0;

  return_status = vorbis_read_nbits(4, &dst, stream->io_desc, &p_count); // blocksize[0]
  fprintf(stderr,"Trace 3 - dst : %d, status : %d------------------------\n", dst, return_status);
  dst = pow(2, dst);
  if (return_status==VBS_BADSTREAM || dst < 64 || dst > 8192) 
    return VBS_BADSTREAM;
  stream->codec->blocksize[0] = (uint16_t)dst;
  dst = 0;

  return_status = vorbis_read_nbits(4, &dst_bis, stream->io_desc, &p_count); // blocksize[1]
  dst_bis = pow(2, dst_bis);
  if ((return_status==VBS_BADSTREAM)||(dst>dst_bis)||(dst_bis>8192)) 
    return VBS_BADSTREAM;
  stream->codec->blocksize[1] = (uint16_t)dst_bis;
  dst = 0;

  return_status = vorbis_read_nbits(1, &dst, stream->io_desc, &p_count); // framming_flag
  if ((return_status==VBS_BADSTREAM) || (dst!=1)) 
    return VBS_BADSTREAM;  

  fprintf(stderr,"End header 1##############################\n");

  return VBS_SUCCESS;
}


status_t vorbis_header2_decode(vorbis_stream_t *stream){
  uint8_t header_type = 0;
  status_t return_status;
  uint32_t p_count, dst, dst_bis1, dst_bis2; 

  fprintf(stderr,"Header 2\n");
  
  return_status = vorbis_common_header(stream, &header_type);
  fprintf(stderr,"Header type : %d\n", header_type);
  if (header_type != 1 || return_status==VBS_BADSTREAM) 
    return VBS_BADSTREAM; 

  // vendor_length
  return_status = vorbis_read_nbits(32, &dst, stream->io_desc, &p_count);
  if (return_status==VBS_BADSTREAM) 
    return VBS_BADSTREAM;

  // vendor_string
  char *chaine = calloc(dst+1, 8);

  for (uint32_t i = 0; i<dst; i++){
    return_status = vorbis_read_nbits(8, &dst_bis1, stream->io_desc, &p_count);
    if (return_status==VBS_BADSTREAM) 
      return VBS_BADSTREAM;
    chaine[i] = dst_bis1;
  }
  chaine[dst] = '\0';
  printf("%s\n\n", chaine);
  fflush(stdout);
  free(chaine);

  // user_comment_list_length
  return_status = vorbis_read_nbits(32, &dst_bis2, stream->io_desc, &p_count);
  if (return_status==VBS_BADSTREAM) 
    return VBS_BADSTREAM;

  for(uint32_t i = 0; i<dst_bis2; i++){

    // length_i
    return_status = vorbis_read_nbits(32, &dst, stream->io_desc, &p_count);
    if (return_status==VBS_BADSTREAM) 
      return VBS_BADSTREAM;
    char *chaine = calloc(dst+4, 8);

    // user_comment_i
    for (uint32_t i = 0; i<(dst+3); i++){
      return_status = vorbis_read_nbits(8, &dst_bis1, stream->io_desc, &p_count);
      if (return_status==VBS_BADSTREAM) 
        return VBS_BADSTREAM;

      // si le caractere est different de '=' (0x3D)
      if (dst_bis1!='='){ 
        chaine[i] = dst_bis1;
      }
      else{
        chaine[i] = ' ';
        chaine[i+1] = ':';
        chaine[i+2] = ' ';
        chaine[i+3] = ' ';
        i = i+3;
      }
    }
    chaine[dst+3] = '\0';
    printf("%s\n", chaine);
    fflush(stdout);
    free(chaine);
  }
  printf("\n");
  fflush(stdout);

  // framing_bit
  return_status = vorbis_read_nbits(1, &dst, stream->io_desc, &p_count); 
  if ((return_status==VBS_BADSTREAM) || (dst!=1)) 
    return VBS_BADSTREAM;  
  
  fprintf(stderr,"End header 1##############################\n");
  return VBS_SUCCESS;
}

status_t vorbis_header3_decode(vorbis_stream_t *stream){

  uint8_t header_type = 0;
  uint32_t p_count = 0; 
  uint32_t dst = 0; 

  status_t return_status = VBS_SUCCESS;
  
  return_status = vorbis_common_header(stream, &header_type);
  if ((header_type != 2) || (return_status==VBS_BADSTREAM)) 
    return VBS_BADSTREAM; 

  fprintf(stderr,"Codebook\n");
  return_status = codebook_setup_init(stream, &stream->codec->codebooks_desc);
  if(return_status != VBS_SUCCESS)
    return return_status;

  fprintf(stderr,"Time domain\n");
  return_status = time_domain_transforms_setup_init(stream, &stream->codec->tdt_desc);
  if(return_status != VBS_SUCCESS)
    return return_status;

  fprintf(stderr,"Floor\n");
  return_status = floors_setup_init(stream, &stream->codec->floors_desc);
  if(return_status != VBS_SUCCESS)
    return return_status;

  fprintf(stderr,"Residue\n");
  return_status = residues_setup_init(stream, &stream->codec->residues_desc);
  if(return_status != VBS_SUCCESS)
    return return_status;

  fprintf(stderr,"Mapping\n");
  return_status = mappings_setup_init(stream, &stream->codec->mappings_desc);
  if(return_status != VBS_SUCCESS)
    return return_status;

  fprintf(stderr,"Windows\n");
  return_status = window_modes_setup_init(stream, &stream->codec->modes_desc);
  if(return_status != VBS_SUCCESS)
    return return_status;

  //Framing bit
  dst = 0;
  return_status = vorbis_read_nbits(1, &dst, stream->io_desc, &p_count); 
  if(return_status==VBS_BADSTREAM || dst!=1) 
    return VBS_BADSTREAM;  

  return return_status;
}
