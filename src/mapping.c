#include "mapping.h"



void mappings_free(mappings_setup_t *map) {
  
  for(uint32_t i=0;i<map->mapping_count;i+){
    free( (*(map->maps))[i] );
  }

  free(map->maps);
}
