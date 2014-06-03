#include "mapping.h"
#include "error.h"

typedef struct mapping_type0_t mapping_type0_t;

struct mapping_type0 {
  mapping_t *mapping;
};



status_t mapping_setup_init(vorbis_stream_t *stream, mappings_setup_t **pmap){

  uint32_t tmp = 0;
  uint32_t null = 0;

  status_t return_status = VBS_SUCCESS;

  //Init pmap
  (*pmap) = malloc(sizeof(mappings_setup_t));

  //Read mapping count bits
  vorbis_read_nbits(MAPPING_COUNT_BITS, &tmp, stream->io_desc, &null);
  (*pmap)->mapping_count = (uint8_t) tmp + 1;

  for(uint32_t i=0;i<(*pmap)->mapping_count;i++){

    mapping_type0_t mapping_0 = malloc(sizeof(mapping_type0_t));
    mapping_0->mapping = malloc(sizeof(mapping_t));

    vorbis_read_nbits(MAPPING_TYPE_BITS, &tmp, stream->io_desc, &null);    
    mapping_0->mapping->type = tmp;

    if(mapping_0->mapping->type = MAPPING_TYPE0)
      mapping_0->mapping->decode(stream, mapping_0->mapping);
  }

  return return_status;
}

void mappings_free(mappings_setup_t *map) {

  for(uint32_t i=0;i<map->mapping_count;i+){
    free( (*(map->maps))[i] );
  }

  free(map->maps);
}
