#include "mapping.h"
#include "helpers.h"
#include <stdio.h>

#define FLAG_BITS 1
#define FLAG_SQUARE_BITS 1
#define MAPPING_SUBMAP_BITS 4
#define MAPPING_STEPS_BITS 8
#define MAPPING_MUX 4
#define RESERVED_BITS 2
#define DISCARD_BITS 8 
#define FLOOR_BITS 8
#define RESIDUE_BITS 8


static void mappings_type0_free(mapping_type0_t *map);
typedef struct mapping_type0 mapping_type0_t;

struct mapping_type0 {
  mapping_t *mapping;

  uint32_t submap_count;
  uint32_t coupling_steps;

  uint8_t *magnitude;
  uint8_t *angle;

  uint8_t *mux;

  uint8_t *submap_floor;
  uint8_t *submap_residue;
};

static status_t mapping_type0_decode(vorbis_stream_t *stream, mapping_t *map, vorbis_packet_t *data){

  
  fprintf(stderr, "Start decode\n");

  status_t return_status = VBS_SUCCESS;
  mapping_type0_t *map0 = (mapping_type0_t *)map;
  uint8_t submap_number = 0;

  /* Floor */

  for(uint32_t i=0;data->nb_chan;i++){
  
    submap_number = map0->mux[i];
    uint8_t floor_nb = map0->submap_floor[submap_number];

    floor_t *floor = stream->codec->floors_desc->floors[floor_nb];
    
    status_t ret = floor->decode(stream, floor, *data->spectral, stream->codec->blocksize[1]/2);
  }

  /* Non-zero propagate */
  for(uint32_t i=0;i<map0->coupling_steps;i++){
      uint8_t magnitude_channel = map0->magnitude[i];
      uint8_t angle_channel = map0->angle[i];

      if(data->no_residue[magnitude_channel] == 0 ||
         data->no_residue[angle_channel] == 0){

        data->no_residue[magnitude_channel] = 0;
        data->no_residue[angle_channel] = 0;
      }
  }

  /* Residues */
  for(uint32_t i=0;i<map0->submap_count;i++){
    uint32_t ch = 0;

    for(uint32_t j=0;j<data->nb_chan;j++){
      
      if(map0->mux[j] == i){
        
        if(data->no_residue[j] == 1)
          data->do_not_decode[ch] = 1;
        else
          data->do_not_decode[ch] = 0;
      }
      ch++;
    }

    uint8_t residue_nb = map0->submap_residue[i];
    residue_t *residue = stream->codec->residues_desc->residues[residue_nb];

    status_t ret = residue->decode(stream, residue, ch, stream->codec->blocksize[1], data->dec_residues, data->do_not_decode);

    ch = 0;
    for(uint32_t j=0;j<data->nb_chan;j++){
      if(map0->mux[j] == i){
        data->residues[j] = data->dec_residues[ch];
        ch++;
      }
    }
  }

  /* Inverse coupling */ 
  for(uint32_t i=0;i<map0->coupling_steps;i++){
    sample_t *magnitude_vector = data->residues[map0->magnitude[i]];
    sample_t *angle_vector = data->residues[map0->angle[i]];
  
    for(uint32_t j=0;j< (sizeof(*magnitude_vector)/sizeof(sample_t));j++){
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

  fprintf(stderr, "End decode\n");
  return return_status ;
}

status_t mapping_decode(vorbis_stream_t *stream, mapping_t *map, vorbis_packet_t *data){

  return map->decode(stream, map, data);
}

status_t mapping_type0_header_decode(vorbis_stream_t *stream, mapping_type0_t *map){

  status_t return_status = VBS_SUCCESS;

  uint32_t tmp = 0;
  uint32_t null = 0;
  uint32_t flag = 0;
  uint32_t reserved = 0;
  uint32_t flag_square_polar = 0;

  //Read flag
  vorbis_read_nbits(FLAG_BITS, &flag, stream->io_desc, &null);
  if(flag == 1){
    //Read mapping submaps bit
    vorbis_read_nbits(MAPPING_SUBMAP_BITS, &tmp, stream->io_desc, &null);
    map->submap_count = tmp;
  }
  else
    map->submap_count = 1;

  //Read flag square polar
  vorbis_read_nbits(FLAG_SQUARE_BITS, &flag_square_polar, stream->io_desc, &null);
  if(flag_square_polar == 1){
    //Read mapping coupling steps
    vorbis_read_nbits(MAPPING_STEPS_BITS, &tmp, stream->io_desc, &null);
    map->coupling_steps = tmp + 1;      

    map->magnitude = malloc(map->coupling_steps * sizeof(uint8_t));
    map->angle = malloc(map->coupling_steps * sizeof(uint8_t));
    for(uint32_t i=0;i<map->coupling_steps;i++){

      uint32_t nb_bits = ilog(stream->codec->audio_channels - 1);
      //Magnitude
      vorbis_read_nbits(nb_bits, &tmp, stream->io_desc, &null);
      map->magnitude[i] = tmp;
      //Angle
      vorbis_read_nbits(nb_bits, &tmp, stream->io_desc, &null);
      map->angle[i] = tmp;

      if(map->magnitude[i] > (stream->codec->audio_channels-1)
          || map->angle[i] > (stream->codec->audio_channels-1)){
        return_status = VBS_BADSTREAM;
      }
    }
  }
  else
    map->coupling_steps = 0;

  //Reserved flag
  vorbis_read_nbits(RESERVED_BITS, &reserved, stream->io_desc, &null);
  if(reserved != 0){
    return_status = VBS_BADSTREAM;
  }

  //Mux part
  map->mux = malloc(stream->codec->audio_channels * sizeof(uint8_t)); 
  if(map->submap_count > 1){
    for(uint32_t i=0;i<stream->codec->audio_channels;i++){
      
      vorbis_read_nbits(MAPPING_MUX, &tmp, stream->io_desc, &null);
      map->mux[i] = (uint8_t) tmp;
      if(map->mux[i] > map->submap_count-1)
        return_status = VBS_BADSTREAM;
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
    vorbis_read_nbits(DISCARD_BITS, &tmp, stream->io_desc, &null);
    
    vorbis_read_nbits(FLOOR_BITS, &tmp, stream->io_desc, &null);
    map->submap_floor[i] = (uint8_t) tmp;
    if(map->submap_floor[i] > stream->codec->floors_desc->floor_count-1){
      return_status = VBS_BADSTREAM;
    }

    vorbis_read_nbits(RESIDUE_BITS, &tmp, stream->io_desc, &null);
    map->submap_residue[i] = (uint8_t) tmp;
    if(map->submap_residue[i] > stream->codec->residues_desc->residue_count-1){
      return_status = VBS_BADSTREAM;
    }
  }

  return return_status;
}

status_t mappings_setup_init(vorbis_stream_t *stream, mappings_setup_t **pmap){

  uint32_t tmp = 0;
  uint32_t null = 0;

  status_t return_status = VBS_SUCCESS;

  //Init pmap
  (*pmap) = malloc(sizeof(mappings_setup_t));

  //Read mapping count bits
  vorbis_read_nbits(MAPPING_COUNT_BITS, &tmp, stream->io_desc, &null);
  (*pmap)->mapping_count = (uint8_t) tmp + 1;

  (*pmap)->maps = malloc( (*pmap)->mapping_count * sizeof(mapping_t*));

  for(uint32_t i=0;i<(*pmap)->mapping_count;i++){

    vorbis_read_nbits(MAPPING_TYPE_BITS, &tmp, stream->io_desc, &null);    
    if(tmp == MAPPING_TYPE0){
      (*pmap)->maps[i] = malloc(sizeof(mapping_type0_t));
      ((*pmap)->maps[i])->type = MAPPING_TYPE0;
      ((*pmap)->maps[i])->id = (uint8_t)i;
      ((*pmap)->maps[i])->decode = mapping_type0_decode;
      ((*pmap)->maps[i])->free = mappings_type0_free;

      //Decode header
      mapping_type0_header_decode(stream,(mapping_type0_t *) (*pmap)->maps[i]);
    }
  }
  
  return return_status;
}

static void mappings_type0_free(mapping_type0_t *map) {
  
  map->mapping->free(map->mapping);
  
  free(map->submap_count);
  free(map->coupling_steps);
  free(map->magnitude);
  free(map->angle);
  free(map->mux);
  free(map->submap_floor);
  free(map->submap_residue);
  
  free(map);
}

void mappings_free(mappings_setup_t *map) {

  for(uint32_t i=0;i<map->mapping_count;i++){
    map->maps[i]->free(map->maps[i]);
  }

  free(map->maps);
  free(map);
}

