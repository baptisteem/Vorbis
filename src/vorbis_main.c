#include "vorbis_main.h"
#include "vorbis.h"
#include "pcm_handler.h"
#include "mode.h"
#include "vorbis_headers.h"
#include <stdio.h>

status_t decode_stream(ogg_logical_stream_t *ogg_stream, pcm_handler_t *pcm_hdler){

    vorbis_stream_t *vorbis = vorbis_new(ogg_stream,pcm_hdler);
    
    vorbis_header1_decode(vorbis);
    vorbis_io_next_packet(vorbis->io_desc);
    vorbis_header2_decode(vorbis);
    vorbis_io_next_packet(vorbis->io_desc);

    vorbis_header3_decode(vorbis);

      vorbis_packet_t *packet = vorbis_packet_init(vorbis->codec->blocksize,vorbis->codec->audio_channels);

      pcm_hdler->init(pcm_hdler,vorbis->codec->audio_sample_rate,vorbis->codec->audio_channels);
      while(vorbis_io_next_packet(vorbis->io_desc) == VBS_SUCCESS){
      
      uint16_t nb_samp = 0;
      vorbis_packet_decode(vorbis,packet,&nb_samp);

      pcm_hdler->process(pcm_hdler,nb_samp,packet->pcm);
    }

    pcm_hdler->finalize(pcm_hdler);
    vorbis_packet_free(packet);
    vorbis_free(vorbis);
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
  pcm_handler_delete(vorbis->pcm_hdler);

  free(vorbis);
}
