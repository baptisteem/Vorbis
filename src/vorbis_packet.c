#include <stdio.h>
#include "vorbis_packet.h"
#include "helpers.h"
#include "dot_product.h"
#include "mapping.h"
#include "mode.h"
 
#define PACKET_TYPE_BITS 1
#define INT_16_MAX 32767
#define INT_16_MIN -32678

//Extend to keep cache in packet
typedef struct vorbis_pkt_cache vorbis_pkt_cache_t;
struct vorbis_pkt_cache{
  vorbis_packet_t base;
  sample_t *filter;
  sample_t **cache;
  envelope_t *envelope;
  uint8_t init;
};

status_t vorbis_packet_decode(vorbis_stream_t *stream, vorbis_packet_t *pkt, uint16_t *nb_samp){

  vorbis_pkt_cache_t *pkt_cache = (vorbis_pkt_cache_t *)pkt;

  uint32_t tmp = 0;
  uint32_t p_count = 0;
  status_t ret;

  //Read packet type
  ret = vorbis_read_nbits(PACKET_TYPE_BITS,&tmp, stream->io_desc, &p_count);
  if(ret != VBS_SUCCESS)
    return ret;

  if(tmp != 0)
    return VBS_BADSTREAM;

  //Read mode number
  ret = vorbis_read_nbits(ilog(stream->codec->modes_desc->mode_count-1), &tmp, stream->io_desc, &p_count);
  if(ret != VBS_SUCCESS)
    return ret;

  uint8_t mode_number = tmp;
  window_mode_t mode = stream->codec->modes_desc->modes[mode_number];

  //Define window's size
  pkt_cache->base.size = stream->codec->blocksize[mode.blockflag];

  if(mode.blockflag == 1){
    ret = vorbis_read_nbits(1, &tmp, stream->io_desc, &p_count);
    if(ret != VBS_SUCCESS)
      return ret; 
    uint32_t previous = tmp;

    ret = vorbis_read_nbits(1, &tmp, stream->io_desc, &p_count);
    if(ret != VBS_SUCCESS)
      return ret;
    uint32_t next = tmp;

    pkt_cache->envelope->curr_window = 1;
    if(previous == 0)
        pkt_cache->envelope->prev_window = 0;
    else
        pkt_cache->envelope->prev_window = 1; 
    if(next == 0)
        pkt_cache->envelope->next_window = 0;
    else    
        pkt_cache->envelope->next_window = 1;
  }
  else
    pkt_cache->envelope->curr_window = 0;

  //Filter envelop and init
  if(pkt_cache->init == 1)
    pkt_cache->envelope->initialized = 1;
  
  ret = envelope_prepare(pkt_cache->envelope, pkt_cache->filter);
  if(ret != VBS_SUCCESS)
    return ret;

  //Init dec_res to 0
  for(uint32_t i=0;i<pkt_cache->base.nb_chan;i++){
    if(i==0){
      for(uint32_t j=0;j<pkt_cache->base.nb_chan*(pkt_cache->base.size/2);j++){
        pkt_cache->base.dec_residues[0][j] = 0;
      }
    }
    else{
      for(uint32_t j=0;j<pkt_cache->base.size/2;j++)
        pkt_cache->base.dec_residues[i][j] = 0;
    }
  }
  
  for(uint32_t i=0;i<pkt_cache->base.nb_chan;i++){
    for(uint32_t j=0;j<pkt_cache->base.size/2;j++)
      pkt_cache->base.spectral[i][j] = 0;
  }
  

  //Mapping decode
  ret = mapping_decode(stream, mode.mapping, (vorbis_packet_t*)pkt_cache);
  if(ret != VBS_SUCCESS)
    return ret;

  //Dot product
  ret = dot_product(pkt_cache->base.spectral, pkt_cache->base.residues, pkt_cache->base.nb_chan, pkt_cache->base.size/2);
  if(ret != VBS_SUCCESS)
    return ret;

  for(uint8_t i=0;i<pkt_cache->base.nb_chan;i++){

    //Spectral to temporal
    ret = time_domain_transform_process(stream->codec->tdt_desc,pkt_cache->base.spectral[i], pkt_cache->base.temporal[i], pkt_cache->filter, mode.blockflag);
    if(ret != VBS_SUCCESS)
      return ret;

    sample_t *tmp = calloc(stream->codec->blocksize[1],sizeof(sample_t));

    //Overlap add
    *nb_samp = envelope_overlap_add(pkt_cache->envelope, pkt_cache->base.temporal[i], pkt_cache->cache[i], tmp);

    //Convert sample_t to int16_t    
    for(uint32_t j=0;j<*nb_samp;j++){
      int32_t s = 0;
      s = tmp[j] * (pow(2,15)-1);
      if(s > INT_16_MAX)
        pkt->pcm[i][j] = INT_16_MAX;
      else if(s < INT_16_MIN)
        pkt->pcm[i][j] = INT_16_MIN;
      else
        pkt->pcm[i][j] = (int16_t) s;
    }

    free(tmp);
  }

  //Set init to 1 after the first packet
  if(pkt_cache->init == 0){
    pkt_cache->init = 1;
  }

  pkt_cache->envelope->prev_window = pkt_cache->envelope->curr_window;
  pkt_cache->envelope->curr_window = pkt_cache->envelope->next_window;

  return VBS_SUCCESS;
}

vorbis_packet_t *vorbis_packet_init(uint16_t *blocksize, uint8_t nb_chan){

  vorbis_pkt_cache_t *packet = malloc(sizeof(vorbis_pkt_cache_t));

  if(packet == NULL)
    return NULL;

  packet->base.nb_chan = nb_chan;
  packet->base.size = blocksize[1];
  packet->init = 0;

  //Init filter
  packet->filter = calloc(blocksize[1],sizeof(sample_t));

  //Init envelope
  packet->envelope = envelope_init(blocksize);
  
  //Init packet cache
  packet->cache = malloc(nb_chan * sizeof(sample_t*));
  if(packet->cache == NULL)
    return NULL;
  for(uint32_t i=0;i<nb_chan;i++){
    packet->cache[i] = malloc(blocksize[1] * sizeof(sample_t));
    if(packet->cache[i] == NULL)
      return NULL;
    for(uint32_t j=0;j<blocksize[1];j++)
      packet->cache[i][j] = 0;
  }

  //Dec_residues
  packet->base.dec_residues = malloc(nb_chan * sizeof(sample_t*));
  if(packet->base.dec_residues == NULL)
    return NULL;

  packet->base.dec_residues[0] = malloc(nb_chan * (blocksize[1]/2) * sizeof(sample_t));
  if(packet->base.dec_residues[0] == NULL)
    return NULL;
  for(uint8_t i=1;i<nb_chan;i++){
    packet->base.dec_residues[i] = malloc( blocksize[1]/2 * sizeof(sample_t));
    if(packet->base.dec_residues[i] == NULL)
      return NULL;
  }

  //do not decode
  packet->base.do_not_decode = malloc(nb_chan * sizeof(uint8_t));
  if(packet->base.do_not_decode == NULL)
    return NULL;

  //no residue
  packet->base.no_residue = malloc(nb_chan * sizeof(uint8_t));
  if(packet->base.no_residue == NULL)
    return NULL;

  //pcm
  packet->base.pcm = malloc(nb_chan * sizeof(int16_t*));
  if(packet->base.pcm == NULL)
    return NULL;
  for(uint8_t i=0;i<nb_chan;i++){
    packet->base.pcm[i] = malloc( blocksize[1] * sizeof(int16_t));
    if(packet->base.pcm[i] == NULL)
      return NULL;
  }

  //residues
  packet->base.residues = malloc(nb_chan * sizeof(sample_t*));
  if(packet->base.residues == NULL)
    return NULL;

  //spectral
  packet->base.spectral = malloc(nb_chan * sizeof(sample_t*));
  if(packet->base.spectral == NULL)
    return NULL;
  
  for(uint8_t i=0;i<nb_chan;i++){
    packet->base.spectral[i] = malloc( blocksize[1]/2 * sizeof(sample_t));
    if(packet->base.spectral[i] == NULL)
      return NULL;
  }

  //temporal
  packet->base.temporal = malloc(nb_chan * sizeof(sample_t*));
  if(packet->base.temporal == NULL)
    return NULL;
  for(uint8_t i=0;i<nb_chan;i++){
    packet->base.temporal[i] = malloc( blocksize[1] * sizeof(sample_t));
    if(packet->base.temporal[i] == NULL)
      return NULL;
  }

  return (vorbis_packet_t*)packet;
}

void vorbis_packet_free(vorbis_packet_t *pkt){
  
  vorbis_pkt_cache_t *pkt_cache = (vorbis_pkt_cache_t*) pkt;

  //Free filter
  free(pkt_cache->filter);

  //Free cache
  for(uint32_t i=0;i<pkt_cache->base.nb_chan;i++)
    free(pkt_cache->cache[i]);
  free(pkt_cache->cache);

  //Free envelope
  envelope_free(pkt_cache->envelope);
  
  //Free dec_residues
  for(uint32_t i=0;i<pkt_cache->base.nb_chan;i++)
    free(pkt_cache->base.dec_residues[i]);
  free(pkt_cache->base.dec_residues);

  //do not decode
  free(pkt_cache->base.do_not_decode);

  //no residue
  free(pkt_cache->base.no_residue);

  //pcm
  for(uint32_t i=0;i<pkt_cache->base.nb_chan;i++)
    free(pkt_cache->base.pcm[i]);
  free(pkt_cache->base.pcm);

  //free residues
  free(pkt_cache->base.residues);

  //free spectral
  for(uint32_t i=0;i<pkt_cache->base.nb_chan;i++)
    free(pkt_cache->base.spectral[i]);
  free(pkt_cache->base.spectral);

  //free spectral
  for(uint32_t i=0;i<pkt_cache->base.nb_chan;i++)
    free(pkt_cache->base.temporal[i]);
  free(pkt_cache->base.temporal);
  
  free(pkt_cache);
}
