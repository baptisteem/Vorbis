#include "floor.h"

void floors_free(floors_setup_t *set){

  free(set->data0);
  free(set->data1);

  for(uint32_t i=0;i<set->floor_count;i++){
    free(set->floors[i]);
  }

  free(set->floors);
  free(set)
}
