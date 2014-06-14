#include "ogg_core.h"
#include "ogg_internal.h"
#include <stdio.h>

ogg_status_t ogg_decode_hdr(FILE *file, ogg_page_hdr_t *hdr){

  //Capture pattern
  fread(hdr->magic,4 * sizeof(uint8_t),1,file);

  //Test if magic == "OggS"
  if(hdr->magic[0] != 'O' || hdr->magic[1] != 'g' || hdr->magic[2] != 'g' || hdr->magic[3] != 'S')
    return OGG_ERR_CORRUPT;

  //Version
  fread(&hdr->version,sizeof(uint8_t),1,file);

  //Header type flag
  fread(&hdr->type,sizeof(uint8_t),1,file);

  //Granule pos
  fread(&hdr->gran_pos,sizeof(int64_t),1,file);

  //Bitstream serial number
  fread(&hdr->stream_id,sizeof(uint32_t),1,file);

  //Page sequence number
  fread(&hdr->page_id,sizeof(uint32_t),1,file);

  //Crc checksum
  fread(&hdr->crc,sizeof(uint32_t),1,file);

  //Page segment
  fread(&hdr->nb_segs,sizeof(uint8_t),1,file);

  return OGG_OK;
}

ogg_status ogg_read_segments(FILE* file, internal_ogg_logical_stream_t *l_stream){

  for(uint32_t i=0;i<l_stream->header->nb_segs;i++){
    
  }

  return OGG_OK;
}

ogg_status_t ogg_init(FILE* file, ogg_physical_stream_t **ppstream){

  internal_ogg_logical_stream_t *l_stream = malloc(sizeof(internal_ogg_logical_stream_t));  

  //Decode header
  l_stream->header = malloc(sizeof(ogg_page_hdr_t));
  ogg_status ret = ogg_decode_hdr(file, l_stream->header);
  if(ret != OGG_OK)
    return ret;

  //Read segment table
  l_stream->table = malloc(l_stream->header->nb_segs * sizeof(uint8_t));
  if(l_stream->table == NULL)
    return OGG_ERR_CORRUPT;
  

  


  return OGG_OK;
}

ogg_status_t ogg_term (ogg_physical_stream_t *pstream) {

  return OGG_OK;
}

ogg_status_t ogg_decode(ogg_logical_stream_t *lstream, pcm_handler_t *pcm_hdler){

  return OGG_OK;
}

ogg_status_t ogg_get_next_page(internal_ogg_logical_stream_t *lstream){

  return OGG_OK;  

}
