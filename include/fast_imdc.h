#include <sample.h>
#include "vorbis_io.h"
#include "vorbis_main.h"


typedef sample_t **matrice_t;
typedef sample_t *vecteur_t;

typedef struct gp_matrice gp_matrice_t;
typedef struct ensemble_matrice ensemble_matrice_t;


struct gp_matrice {
  uint32_t size_N;
  matrice_t M;       // Matrice taille N/2^2
}gp_matrice;

struct ensemble_matrice{
  gp_matrice_t *M_blocsize_0;  
  gp_matrice_t *M_blocsize_1;
}ensemble_matrice;


status_t multiplication_classique(gp_matrice_t *gp, vecteur_t B, vecteur_t v);

status_t multiplication_P(gp_matrice_t *gp, vecteur_t B, vecteur_t v);

status_t initialisation_matrices(ensemble_matrice_t **Ens, vorbis_stream_t *stream);

void matrices_free(ensemble_matrice_t *Ens);


