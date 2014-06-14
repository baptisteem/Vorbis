#include "ogg_core.h"
#include "ogg_internal.h"
#include "vorbis_packet.h"
#include <stdio.h>

#define END_OF_STREAM 4
#define BIT_4 0x0004

/*
 * Extend internal_ogg_logical_stream to keep FILE pointer
 */
typedef struct internal_ogg_logical_stream_file intern_ogg_log_file_t;
struct internal_ogg_logical_stream_file{
  internal_ogg_logical_stream_t base;
  FILE *file;
  uint8_t eof;
};

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

ogg_status_t ogg_read_segments(FILE* file, internal_ogg_logical_stream_t *l_stream){

  //Read segment table size
  for(uint32_t i=0;i<l_stream->header->nb_segs;i++)
    fread(&l_stream->table[i], sizeof(uint8_t), 1, file);

  uint32_t size_segs = 0;
  //Compute size for data malloc
  for(uint32_t i=0;i<l_stream->header->nb_segs;i++)
    size_segs += l_stream->table[i];


  //Init segment data
  l_stream->data = calloc(size_segs,sizeof(uint8_t));
  if(l_stream->data == NULL)
    return OGG_ERR_CORRUPT;

  uint32_t cpt = 0;
  for(uint32_t i=0;i<l_stream->header->nb_segs;i++){
    
    if(i == 0)
      fread(&l_stream->data[0], l_stream->table[i] * sizeof(uint8_t), 1, file);
    else
      fread(&l_stream->data[cpt], l_stream->table[i] * sizeof(uint8_t), 1, file);
    cpt += l_stream->table[i];
  }


  return OGG_OK;
}

ogg_status_t ogg_init(FILE* file, ogg_physical_stream_t **ppstream){

  //init the physical stream
  (*ppstream) = malloc(sizeof(ogg_physical_stream_t));
  (*ppstream)->nb_streams = 1;

  //logical stream with file
  intern_ogg_log_file_t *l_stream = malloc(sizeof(intern_ogg_log_file_t));  
  l_stream->eof = 0;

  //Attach the file to extended internal structure
  l_stream->file = file;
  //Attach physical stream to internal stream
  l_stream->base.phy = (*ppstream);
  l_stream->base.packet = NULL;
 
  // Attach the logical stream to the physical one 
  (*ppstream)->first = (ogg_logical_stream_t*)((internal_ogg_logical_stream_t*)l_stream);

  //Decode header
  l_stream->base.header = malloc(sizeof(ogg_page_hdr_t));
  ogg_status_t ret = ogg_decode_hdr(file, l_stream->base.header);
  if(ret != OGG_OK)
    return ret;

  //Configure logical stream
  l_stream->base.base.codec = OGG_VORBIS;
  l_stream->base.base.stream_id = l_stream->base.header->stream_id;
  l_stream->base.base.next = NULL;

  //Read segment table
  l_stream->base.table = calloc(l_stream->base.header->nb_segs,sizeof(uint8_t));
  if(l_stream->base.table == NULL)
    return OGG_ERR_CORRUPT;
  
    ogg_read_segments(file, (internal_ogg_logical_stream_t*)l_stream);
  
  return OGG_OK;
}

ogg_status_t ogg_term(ogg_physical_stream_t *pstream) {

  //Detach packet
  ogg_packet_detach((internal_ogg_logical_stream_t*)pstream->first);
/*
  //Free header, data and table
  free(((internal_ogg_logical_stream_t*)pstream->first)->header);
  free(((internal_ogg_logical_stream_t*)pstream->first)->data);
  free(((internal_ogg_logical_stream_t*)pstream->first)->table);
*/
  //Free extented internal logical stream
  free((intern_ogg_log_file_t*)pstream->first);

  //Free physical stream
  free(pstream);

  return OGG_OK;
}

ogg_status_t ogg_decode(ogg_logical_stream_t *lstream, pcm_handler_t *pcm_hdler){

  ogg_status_t ogg_ret = ogg_packet_attach((internal_ogg_logical_stream_t*)lstream);
  if(ogg_ret != OGG_OK)
    return ogg_ret;

  status_t ret = decode_stream(lstream, pcm_hdler);
  if(ret != VBS_SUCCESS)
    return OGG_ERR_CORRUPT;
  
  return OGG_OK;
}

ogg_status_t ogg_get_next_page(internal_ogg_logical_stream_t *lstream){

  intern_ogg_log_file_t *stream = (intern_ogg_log_file_t*)lstream;

  //Free the last data and table
  free(stream->base.data);
  free(stream->base.table);
  
  if(stream->eof == 1) 
  {
    free(lstream->header);
    lstream->header = NULL;
    lstream->data = NULL;
    lstream->table = NULL;
    return OGG_END;
  }
  

  ogg_status_t ret = ogg_decode_hdr(stream->file, stream->base.header);
   
  if((lstream->header->type & BIT_4) == END_OF_STREAM) 
    stream->eof = 1; 

  //Init segment table
  stream->base.table = calloc(stream->base.header->nb_segs,sizeof(uint8_t));
  if(stream->base.table == NULL)
    return OGG_ERR_CORRUPT;
    
  ret = ogg_read_segments(stream->file, lstream);
  if(ret != OGG_OK)
    return ret;
  
  return OGG_OK;  
}
