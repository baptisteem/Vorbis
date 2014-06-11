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

status_t vorbis_read_nbits(uint32_t nb_bits, uint32_t *dst, vorbis_io_t *io, uint32_t *p_count){

  //if we have enough bits in reste
  if(io->nb_reste >= nb_bits){

    uint8_t mask = 0;
    for(uint8_t i=0;i<nb_bits;i++){
      mask += pow(2,i);
    }
    fprintf(stderr,"Dans retenue = nb_bits, nb_reste : %d\n", io->nb_reste);
    fprintf(stderr,"Nb_bits : %d\n", nb_bits);
    *dst = 0;
    *dst = io->reste & mask;
    fprintf(stderr,"Avant : Mask : %d, io->reste : %d, dst : %d\n", mask, io->reste, *dst);
    io->reste = io->reste >> nb_bits;
    io->nb_reste -= nb_bits;

    *p_count = nb_bits;
    fprintf(stderr,"Apres : Nb_reste : %d, io->reste : %d, dst : %d\n", io->nb_reste, io->reste, *dst);
  }
  else{

    uint32_t nb_bytes = 0;
    if( (nb_bits - io->nb_reste) %8 == 0)
      nb_bytes = (nb_bits-io->nb_reste) / 8 ;
    else
      nb_bytes = (nb_bits - io->nb_reste) / 8 + 1;
    uint8_t reste_in_bits = nb_bytes*8 + io->nb_reste - nb_bits;
    uint8_t *buf = malloc(nb_bytes * sizeof(uint8_t));
    uint32_t bytes_read = 0;

    ogg_status_t ret = ogg_packet_read(io->stream,buf,nb_bytes,&bytes_read); 
    
    fprintf(stderr,"############################\n");
    fprintf(stderr,"Nb_bits : %d, Nb_bytes : %d\n", nb_bits, nb_bytes);
    fprintf(stderr,"Reste in bits : %d\n", reste_in_bits);
    for(uint8_t k=0;k<nb_bytes;k++)
      fprintf(stderr,"Raw data [%d] : %d\n", k,buf[k]);
    fprintf(stderr,"Nb_reste : %d, reste : %d\n", io->nb_reste, io->reste);
    fprintf(stderr,"############################\n");
    

    if(ret == OGG_END)
      return VBS_EOP;
    if(ret != OGG_OK)
      return VBS_BADSTREAM;

    *dst = 0;
    //We have the same amount of byte read
    if(nb_bytes == bytes_read){

      fprintf(stderr,"Dans nb_bytes = bytes_read\n");

      //Fill dst with bytes read
      //*dst |= io->reste << (nb_bytes*8 + (8-reste_in_bits));
      uint8_t mask = 0;
      for(uint8_t k=0;k<io->nb_reste;k++)
        mask += pow(2,k);
      *dst |= io->reste & mask;
      for(int8_t i=0;i<nb_bytes;i++){
        if(i != nb_bytes-1)
          *dst |= (buf[i] << (i*8 + io->nb_reste));
        else{
          //*dst |= buf[0] << (nb_bytes*8 + (8-io->nb_reste));
          uint8_t mask = 0;
          for(uint8_t j=0;j<reste_in_bits;j++)
            mask += pow(2,j);
          *dst |= (buf[nb_bytes-1] & mask) << (i*8 + io->nb_reste);
          // *dst |= buf[nb_bytes-1] & mask;
        }
      }
      /*
      //Fill dst with bytes read
      *dst = io->reste << (nb_bytes*8 + (8-reste_in_bits));
      for(int8_t i=nb_bytes-1;i>=0;i--){
        if(i != 0)
          *dst |= (buf[i] << (i*8 - reste_in_bits));
          //*dst |= (buf[nb_bytes-i] << (i*8 - reste_in_bits));
        else{
          uint8_t mask = 0;
          for(uint8_t j=0;j<8-reste_in_bits;j++)
            mask += pow(2,j);
          *dst |= buf[0] & mask;
          //*dst |= buf[nb_bytes-1] & mask;
        }
      }
      */
      io->reste = 0;
      if(reste_in_bits != 0)
        //io->reste = buf[nb_bytes-1] >> (8-reste_in_bits);
        io->reste = buf[0] >> (reste_in_bits);
      io->nb_reste = reste_in_bits;
      *p_count = nb_bits;
    }
    else{

      fprintf(stderr,"Dans nb_bytes != bytes_read\n");
      //Fill dst with bytes read
      *dst = io->reste << (bytes_read*8 + reste_in_bits);
      for(int8_t i=bytes_read-1;i>=0;i--){
        if(i != 0)
          *dst |= (buf[i] << (i*8 - reste_in_bits));
        else{
          uint8_t mask = 0;
          for(uint8_t j=0;j<8-reste_in_bits;j++)
            mask += pow(2,j);
          *dst |= buf[0] & mask;
        }
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
  //  io->stream = io->stream->next;

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
