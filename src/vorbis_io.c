#include "vorbis_io.h"
#include "ogg.h"
#include "ogg_packet.h"
#include "helpers.h"

struct vorbis_io{

  ogg_logical_stream_t *stream;
  int8_t reste;

};

/* Big endian --> Little endian ???
*/
status_t vorbis_read_nbits(uint32_t nb_bits, uint32_t *dst, vorbis_io_t *io, uint32_t *p_count){

  uint8_t bits_reste = ilog(io->reste);

  //if we have enough bits in reste
  if(bits_reste >= nb_bits){

    //Create the mask to select bits in reste
    uint8_t mask = 0;
    for(uint8_t i=bits_reste;i>bits_reste-nb_bits;i--){
      mask += pow(2,i);
    }

    *dst = 0;
    *dst = io->reste & mask;
    *p_count = nb_bits;
  }
  else{

    uint32_t nb_bytes = (io->reste + nb_bits) / 8;
    uint8_t reste_in_bits = (io->reste + nb_bits) % 8;
    uint8_t *buf = malloc(nb_bytes * sizeof(uint8_t));
    uint32_t bytes_read = 0;

    ogg_status_t ret = ogg_packet_read(io->stream,buf,nb_bytes,&bytes_read);
    if(ret != OGG_OK)
      return VBS_BADSTREAM;

    *dst = 0;
    //We have the same amount of byte read
    if(nb_bytes == bytes_read){

      //Fill dst with bytes read
      *dst = io->reste << (32-bits_reste);
      for(uint8_t i=1;i<=nb_bytes;i++){
        *dst |= buf[i-1] << (32-bits_reste-i*8);
      }

      io->reste = 0;
      io->reste = buf[nb_bytes-1] << (8-reste_in_bits);
    }
  }

  return VBS_SUCCESS;
}

vorbis_io_t *vorbis_io_init(ogg_logical_stream_t *ogg_desc){

  vorbis_io_t *vorbis = malloc(sizeof(vorbis_io_t));

  if(ogg_packet_attach(ogg_desc) != OGG_OK)
    return VBS_BADSTREAM;

  vorbis->stream = ogg_desc;

  return VBS_SUCCESS;
}

status_t vorbis_io_next_packet(vorbis_io_t *io){

  //Set reste to zero
  io->reste = 0;

  ogg_status_t ret = ogg_packet_next(io->stream);

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

  ogg_packet_detach(io->stream);

  free(io);
}
