#include "mapping.h"
#include "helpers.h"
#include <stdio.h>


#define MAPPING_COUNT_BITS 6
#define MAPPING_TYPE_BITS 16
#define FLAG_BITS 1
#define FLAG_SQUARE_BITS 1
#define MAPPING_SUBMAP_BITS 4
#define MAPPING_STEPS_BITS 8
#define MAPPING_MUX 4
#define RESERVED_BITS 2
#define DISCARD_BITS 8 
#define FLOOR_BITS 8
#define RESIDUE_BITS 8


typedef struct mapping_type0 mapping_type0_t;

struct mapping_type0 {
  mapping_t mapping;

  uint32_t submap_count;
  uint32_t coupling_steps;

  uint8_t *magnitude;
  uint8_t *angle;

  uint8_t *mux;

  uint8_t *submap_floor;
  uint8_t *submap_residue;
};

/*
 * Free the memory for a type 0 mapping
 */
static void mapping_type0_free(mapping_type0_t *map) {

  free(map->magnitude);
  free(map->angle);
  free(map->mux);
  free(map->submap_floor);
  free(map->submap_residue);
  
  free(map);
}

/* 
 * Decode floor part in type0 decode
 */
static status_t mapping_type0_decode_floor(vorbis_stream_t *stream, mapping_type0_t *map, vorbis_packet_t *data){

  status_t ret;
  uint8_t submap_number = 0;

  for(uint32_t i=0;i<stream->codec->audio_channels;i++){
  
    submap_number = map->mux[i];
    uint8_t floor_nb = map->submap_floor[submap_number];

    floor_t *floor = stream->codec->floors_desc->floors[floor_nb];
  
    ret = floor->decode(stream, floor, data->spectral[i], stream->codec->blocksize[1]/2);
  
    if(ret == VBS_UNUSED){
      data->no_residue[i] = 1;
      ret = VBS_SUCCESS;
    }
    else
      data->no_residue[i] = 0;
  }

  return ret;
}

/*
 * Decode non zero propagate in type0 decode
 */
static status_t mapping_type0_decode_non_zero_propagate(mapping_type0_t *map, vorbis_packet_t *data){

  for(uint32_t i=0;i<map->coupling_steps;i++){
      uint8_t magnitude_channel = map->magnitude[i];
      uint8_t angle_channel = map->angle[i];

      if(data->no_residue[magnitude_channel] == 0 ||
         data->no_residue[angle_channel] == 0){

        data->no_residue[magnitude_channel] = 0;
        data->no_residue[angle_channel] = 0;
      }
  }

  return VBS_SUCCESS;
}

/*
 * Decode residues in type0 decode
 */
static status_t mapping_type0_decode_residues(vorbis_stream_t *stream, mapping_type0_t *map, vorbis_packet_t *data){
  
  status_t ret;

  for(uint32_t i=0;i<map->submap_count;i++){
    uint32_t ch = 0;

    for(uint32_t j=0;j<stream->codec->audio_channels;j++){
      
      if(map->mux[j] == i){
        
        if(data->no_residue[j] == 1)
          data->do_not_decode[ch] = 1;
        else
          data->do_not_decode[ch] = 0;
      }
      ch++;
    }

    uint8_t residue_nb = map->submap_residue[i];
    residue_t *residue = stream->codec->residues_desc->residues[residue_nb];

    ret = residue->decode(stream, residue, ch, data->size/2, data->dec_residues, data->do_not_decode);

    if(ret != VBS_SUCCESS)
      return ret;

    ch = 0;
    for(uint32_t j=0;j<stream->codec->audio_channels;j++){
      if(map->mux[j] == i){
        data->residues[j] = data->dec_residues[ch];
        ch++;
      }
    }
  }

  return VBS_SUCCESS;
}

/*
 * Decode inverse coupling in type0 decode
 */
static status_t mapping_type0_decode_inverse_coupling(mapping_type0_t *map, vorbis_packet_t *data){
  
  for(uint32_t i=0;i<map->coupling_steps;i++){
    sample_t *magnitude_vector = data->residues[map->magnitude[i]];
    sample_t *angle_vector = data->residues[map->angle[i]];
  
    for(uint32_t j=0;j<data->size/2;j++){
      sample_t m = magnitude_vector[j];
      sample_t a = angle_vector[j];

      if(m>0){
        if(a>0)
          angle_vector[j] = m - a; 
        else{
          angle_vector[j] = m;
          magnitude_vector[j] = m + a;
        }
      }
      else{
        if(a>0)
          angle_vector[j] = m + a; 
        else{
          angle_vector[j] = m;
          magnitude_vector[j] = m - a;
        }
      }
    }
  }

  return VBS_SUCCESS;
}

/*
 * Decode a type0 mapping
 */
static status_t mapping_type0_decode(vorbis_stream_t *stream, mapping_t *map, vorbis_packet_t *data){

  status_t ret;
  mapping_type0_t *map0 = (mapping_type0_t *)map;

  /* Decode floor part */
  ret = mapping_type0_decode_floor(stream, map0, data);
  if(ret != VBS_SUCCESS)
    return ret;

  /* Non-zero propagate */
  ret = mapping_type0_decode_non_zero_propagate(map0, data);
  if(ret != VBS_SUCCESS)
    return ret;

  /* Residues */
  ret = mapping_type0_decode_residues(stream, map0, data);
  if(ret != VBS_SUCCESS)
    return ret;
  
  /* Inverse coupling */ 
  ret = mapping_type0_decode_inverse_coupling(map0, data);
  if(ret != VBS_SUCCESS)
    return ret;

  return VBS_SUCCESS;
}

status_t mapping_decode(vorbis_stream_t *stream, mapping_t *map, vorbis_packet_t *data){

  status_t ret =  ((mapping_type0_t*)map)->mapping.decode(stream, map, data);

  return ret;
}

status_t mapping_type0_header_decode(vorbis_stream_t *stream, mapping_type0_t *map){

  status_t ret = VBS_SUCCESS;

  uint32_t tmp = 0;
  uint32_t null = 0;
  uint32_t flag = 0;
  uint32_t reserved = 0;
  uint32_t flag_square_polar = 0;

  //Read flag
  ret = vorbis_read_nbits(FLAG_BITS, &flag, stream->io_desc, &null);
  if(ret != VBS_SUCCESS)
    return ret;

  if(flag == 1){
    //Read mapping submaps bit
    ret = vorbis_read_nbits(MAPPING_SUBMAP_BITS, &tmp, stream->io_desc, &null);
    if(ret != VBS_SUCCESS)
      return ret;
    map->submap_count = tmp;
  }
  else
    map->submap_count = 1;

  //Read flag square polar
  ret = vorbis_read_nbits(FLAG_SQUARE_BITS, &flag_square_polar, stream->io_desc, &null);
  if(ret != VBS_SUCCESS)
    return ret;
  
  if(flag_square_polar == 1){
    //Read mapping coupling steps
    ret = vorbis_read_nbits(MAPPING_STEPS_BITS, &tmp, stream->io_desc, &null);
    if(ret != VBS_SUCCESS)
      return ret;
    
    map->coupling_steps = tmp + 1;      

    map->magnitude = malloc(map->coupling_steps * sizeof(uint8_t));
    map->angle = malloc(map->coupling_steps * sizeof(uint8_t));
    for(uint32_t i=0;i<map->coupling_steps;i++){

      uint32_t nb_bits = ilog(stream->codec->audio_channels - 1);
      //Magnitude
      ret = vorbis_read_nbits(nb_bits, &tmp, stream->io_desc, &null);
      if(ret != VBS_SUCCESS)
        return ret;
      
      map->magnitude[i] = tmp;
      //Angle
      ret = vorbis_read_nbits(nb_bits, &tmp, stream->io_desc, &null);
      if(ret != VBS_SUCCESS)
        return ret;
      map->angle[i] = tmp;

      if(map->magnitude[i] > (stream->codec->audio_channels-1)
          || map->angle[i] > (stream->codec->audio_channels-1)){
        return VBS_BADSTREAM;
      }
    }
  }
  else
    map->coupling_steps = 0;

  //Reserved flag
  ret = vorbis_read_nbits(RESERVED_BITS, &reserved, stream->io_desc, &null);
  if(ret != VBS_SUCCESS)
    return ret;
  
  if(reserved != 0){
     return VBS_BADSTREAM;
  }

  //Mux part
  map->mux = malloc(stream->codec->audio_channels * sizeof(uint8_t)); 
  if(map->submap_count > 1){
    for(uint32_t i=0;i<stream->codec->audio_channels;i++){
      
      ret = vorbis_read_nbits(MAPPING_MUX, &tmp, stream->io_desc, &null);
      if(ret != VBS_SUCCESS)
        return ret;
      
      map->mux[i] = (uint8_t) tmp;
      if(map->mux[i] > map->submap_count-1)
        return VBS_BADSTREAM;
    }
  }
  else{
    for(uint32_t i=0;i<stream->codec->audio_channels;i++){
      map->mux[i] = 0;
    }
  }

  //Floor and residue
  map->submap_floor = malloc(map->submap_count * sizeof(uint8_t));
  map->submap_residue = malloc(map->submap_count * sizeof(uint8_t));
  for(uint32_t i=0;i<map->submap_count;i++){
    ret = vorbis_read_nbits(DISCARD_BITS, &tmp, stream->io_desc, &null);
    if(ret != VBS_SUCCESS)
      return ret;
    
    ret = vorbis_read_nbits(FLOOR_BITS, &tmp, stream->io_desc, &null);
    if(ret != VBS_SUCCESS)
      return ret;
    
    map->submap_floor[i] = (uint8_t) tmp;
    if(map->submap_floor[i] > stream->codec->floors_desc->floor_count-1){
      return VBS_BADSTREAM;
    }

    ret = vorbis_read_nbits(RESIDUE_BITS, &tmp, stream->io_desc, &null);
    if(ret != VBS_SUCCESS)
      return ret;
    
    map->submap_residue[i] = (uint8_t) tmp;
    if(map->submap_residue[i] > stream->codec->residues_desc->residue_count-1){
      return VBS_BADSTREAM;
    }
  }

  return VBS_SUCCESS;
}

status_t mappings_setup_init(vorbis_stream_t *stream, mappings_setup_t **pmap){

  uint32_t tmp = 0;
  uint32_t null = 0;

  status_t ret = VBS_SUCCESS;

  //Init pmap
  (*pmap) = malloc(sizeof(mappings_setup_t));

  //Read mapping count bits
  ret = vorbis_read_nbits(MAPPING_COUNT_BITS, &tmp, stream->io_desc, &null);
  if(ret != VBS_SUCCESS)
    return ret;
  
  (*pmap)->mapping_count = (uint8_t) tmp + 1;

  (*pmap)->maps = malloc( (*pmap)->mapping_count * sizeof(mapping_t*));

  for(uint32_t i=0;i<(*pmap)->mapping_count;i++){

    ret = vorbis_read_nbits(MAPPING_TYPE_BITS, &tmp, stream->io_desc, &null);    
    if(ret != VBS_SUCCESS)
      return ret;
    
    if(tmp == MAPPING_TYPE0){

      mapping_type0_t *map0 = malloc(sizeof(mapping_type0_t));
      map0->mapping.id = (uint8_t)i;
      map0->mapping.type = MAPPING_TYPE0;
      map0->mapping.decode = (void *)mapping_type0_decode;
      map0->mapping.free = (void *)mapping_type0_free;
      (*pmap)->maps[i] = (mapping_t *)map0;
      
      //Decode header
      mapping_type0_header_decode(stream,(mapping_type0_t *) (*pmap)->maps[i]);
    }
  }
  
  return VBS_SUCCESS;
}


void mappings_free(mappings_setup_t *map) {

  for(uint32_t i=0;i<map->mapping_count;i++){
    map->maps[i]->free(map->maps[i]);
  }

  free(map->maps);
  free(map);
}
 
