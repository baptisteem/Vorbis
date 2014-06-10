#include "vorbis_io.h"
#include "ogg.h"
#include "ogg_packet.h"
#include "helpers.h"
#include <stdio.h>

struct vorbis_io{

  ogg_logical_stream_t *stream;
  uint8_t reste;
  uint8_t nb_reste;

};

/* Big endian --> Little endian ???
*/
status_t vorbis_read_nbits(uint32_t nb_bits, uint32_t *dst, vorbis_io_t *io, uint32_t *p_count){

  //if we have enough bits in reste
  if(io->nb_reste >= nb_bits){

    *dst = 0;
    *dst = io->reste << nb_bits;
    io->reste = io->reste >> nb_bits;
    io->nb_reste -= nb_bits;

    *p_count = nb_bits;
  }
  else{

    uint32_t nb_bytes = (io->nb_reste + nb_bits) / 8 + 1;
    uint8_t reste_in_bits = nb_bytes*8 - io->nb_reste - nb_bits;
    uint8_t *buf = malloc(nb_bytes * sizeof(uint8_t));
    uint32_t bytes_read = 0;

    ogg_status_t ret = ogg_packet_read(io->stream,buf,nb_bytes,&bytes_read);
    if(ret != OGG_OK)
      return VBS_BADSTREAM;

    *dst = 0;
    //We have the same amount of byte read
    if(nb_bytes == bytes_read){
      
      //Fill dst with bytes read
      *dst = io->reste << (nb_bytes*8 + reste_in_bits);
      for(uint8_t i=0;i<nb_bytes;i++){
        fprintf(stderr,"Dst : %d\n", (nb_bytes-i-1)*8+io->nb_reste);
        *dst |= buf[i] << ((nb_bytes-i-1)*8 + io->nb_reste);
      }

      uint8_t mask = 0;
      for(uint8_t i=0;i<reste_in_bits;i++){
          mask += pow(2,i);
      }
      io->reste = 0;
      fprintf(stderr,"buf[nb_bytes-1] : %d\n, mask : %d\n", buf[nb_bytes-1], mask);
      io->reste = buf[nb_bytes-1] & mask;
      io->nb_reste = reste_in_bits;
      *p_count = nb_bits;
    }
    else{
      
      //Fill dst with bytes read
      *dst = io->reste << (bytes_read*8-io->nb_reste);
      for(uint8_t i=1;i<=bytes_read;i++){
        *dst |= buf[i] << ((nb_bytes-i-1)*8 + io->nb_reste);
      }

      *p_count = io->nb_reste + 8*bytes_read;
      io->reste = 0;
      io->nb_reste = 0;
    }
    free(buf);
  }

  return VBS_SUCCESS;
}

vorbis_io_t *vorbis_io_init(ogg_logical_stream_t *ogg_desc){

  vorbis_io_t *vorbis = malloc(sizeof(vorbis_io_t));

  vorbis->stream = ogg_desc;
  vorbis->reste = 0;
  vorbis->nb_reste = 0;

  return vorbis;
}

status_t vorbis_io_next_packet(vorbis_io_t *io){

  //Set reste to zero
  io->reste = 0;
  io->nb_reste = 0;

  ogg_status_t ret = ogg_packet_next(io->stream);
  io->stream = io->stream->next;

  if(ret == OGG_OK)
    return VBS_SUCCESS;
  if(ret == OGG_END)
    return VBS_EOS;
 
  return VBS_FATAL;
}

int64_t vorbis_io_limit(vorbis_io_t *io){

  int64_t limit = 0;

  ogg_packet_position(io->stream,&limit);

  return limit;
}

void vorbis_io_free(vorbis_io_t *io){

  free(io);
}
