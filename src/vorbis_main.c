#include "vorbis_main.h"
#include "vorbis.h"
#include "pcm_handler.h"
#include "mode.h"
#include "vorbis_headers.h"
#include <stdio.h>

status_t decode_stream(ogg_logical_stream_t *ogg_stream, pcm_handler_t *pcm_hdler){

  status_t ret;

  vorbis_stream_t *vorbis = vorbis_new(ogg_stream,pcm_hdler);

  //Decode header 1
  ret = vorbis_header1_decode(vorbis);
  if(ret != VBS_SUCCESS)
    return ret;
  
  ret = vorbis_io_next_packet(vorbis->io_desc);
  if(ret != VBS_SUCCESS)
    return ret;
  
  //Decode header 2
  ret = vorbis_header2_decode(vorbis);
  if(ret != VBS_SUCCESS)
    return ret;
  
  ret = vorbis_io_next_packet(vorbis->io_desc);
  if(ret != VBS_SUCCESS)
    return ret;
  
  //Decode header 3
  ret = vorbis_header3_decode(vorbis);
  if(ret != VBS_SUCCESS)
    return ret;

  vorbis_packet_t *packet = vorbis_packet_init(vorbis->codec->blocksize,vorbis->codec->audio_channels);

  pcm_hdler->init(pcm_hdler,vorbis->codec->audio_sample_rate,vorbis->codec->audio_channels);

  int64_t cpt = 0;
  int64_t limit = 0;
  while(vorbis_io_next_packet(vorbis->io_desc) == VBS_SUCCESS){
    
    uint16_t nb_samp = 0;
    ret = vorbis_packet_decode(vorbis,packet,&nb_samp);
    if(ret != VBS_SUCCESS)
      return ret;

    limit = vorbis_io_limit(vorbis->io_desc);
    //We check if we reached the limit    
    if(limit != -1 && limit < cpt+nb_samp)
      break;
    
    cpt += nb_samp;
      
    pcm_hdler->process(pcm_hdler,nb_samp,packet->pcm);
  }

  pcm_hdler->process(pcm_hdler,limit-cpt,packet->pcm);
  pcm_hdler->finalize(pcm_hdler);
  vorbis_packet_free(packet);
  vorbis_free(vorbis);

  return VBS_SUCCESS;
}

vorbis_stream_t *vorbis_new(ogg_logical_stream_t *ogg, pcm_handler_t *pcm_hdler){

  vorbis_stream_t *stream = malloc(sizeof(vorbis_stream_t));

  stream->codec = vorbis_codec_new();
  stream->io_desc = vorbis_io_init(ogg);
  stream->pcm_hdler = pcm_hdler;

  return stream;
}

vorbis_codec_t *vorbis_codec_new(void){
  return malloc(sizeof(vorbis_codec_t));
}

void vorbis_codec_free(vorbis_codec_t *codec){

  codebooks_free(codec->codebooks_desc);
  floors_free(codec->floors_desc);
  mappings_free(codec->mappings_desc);
  window_modes_free(codec->modes_desc);
  residues_free(codec->residues_desc);
  time_domain_transforms_free(codec->tdt_desc);

  free(codec);
}

void vorbis_free(vorbis_stream_t *vorbis){

  vorbis_codec_free(vorbis->codec);
  vorbis_io_free(vorbis->io_desc);

  free(vorbis);
}
