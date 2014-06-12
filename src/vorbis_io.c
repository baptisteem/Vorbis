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

/*
 * Create a mask to get the n first bits
 */
static uint8_t create_mask(uint8_t n){

  uint8_t mask = 0;
  for(uint32_t i=0;i<n;i++)
    mask += pow(2,i);

  return mask;
}

status_t vorbis_read_nbits(uint32_t nb_bits, uint32_t *dst, vorbis_io_t *io, uint32_t *p_count){


  //First case, wa have enough bits in reste
  if(nb_bits <= io->nb_reste){
    
    *dst = 0;
    //Select the nb_bits first bits with an and mask
    *dst = io->reste & create_mask(nb_bits);
    //Change reste size
    io->nb_reste -= nb_bits;
    //Shift the reste
    io->reste = io->reste >> nb_bits;
    *p_count = nb_bits;
  }
  //If we have to ask for new bytes
  else{
    
    //Compute how many bytes we nead to read
    uint8_t bytes_to_read = (nb_bits - io->nb_reste)/8;
    //If we don't have a multiple of 8
    if((nb_bits - io->nb_reste)%8 != 0)
     bytes_to_read += 1;
    //Compute how many bits the will be in reste at the end
    uint8_t next_nb_reste = (io->nb_reste + bytes_to_read*8) - nb_bits;

    uint8_t *buf = malloc(bytes_to_read * sizeof(uint8_t));
    if(buf == NULL)
      return VBS_FATAL;

    //Bytes actually read by ogg_packet_read
    uint32_t nbytes_read = 0;
    ogg_status_t ret_ogg = ogg_packet_read(io->stream, buf, bytes_to_read, &nbytes_read);
    if(ret_ogg != OGG_OK)
      return VBS_FATAL;
    
    //If we read the same number of bytes we wanted to
    if(nbytes_read == bytes_to_read){
    
      //First we add the previous reste
      *dst = io->reste;
      //Now we add each bytes 
      for(uint32_t i=0;i<nbytes_read-1;i++){
        *dst |= (buf[i] << (io->nb_reste + i*8));
      }
      //If we have a multiple of 8 we add the entire last byte
      *dst |= ((buf[nbytes_read-1] & create_mask(8-next_nb_reste)) << (io->nb_reste + (nbytes_read-1)*8));
      
      *p_count = nb_bits;
      //We update the new reste
      io->nb_reste = next_nb_reste;
      io->reste = buf[nbytes_read-1] >> (8-next_nb_reste) ;
    }
    else{
      
      //First we add the previous reste
      *dst = io->reste;
      //Now we add each bytes 
      for(uint32_t i=0;i<nbytes_read;i++){
        *dst |= (buf[i] << (io->nb_reste + i*8));
      }
      
      *p_count = nbytes_read*8 + io->nb_reste;
      //We update the new reste
      io->nb_reste = 0;
      io->reste = 0;
      
      return VBS_EOP;
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
