#include "fast_imdc.h"
#include <math.h>
#include <sample.h>
#include <stdio.h>




status_t multiplication_classique(gp_matrice_t *gp, vecteur_t B, vecteur_t v){
  uint32_t N = gp->size_N/2;
  sample_t temp = 0;
  matrice_t M_temp = gp->M;

  /**********   on fait une multiplication pour une matrice pleine N/2^2   *********/
  for(uint32_t i = 0; i<N; i++){
    for(uint32_t j = 0; j<N; j++){
      temp += M_temp[i][j]*B[j];
    }
    v[i] = temp;
    temp = 0;
  }

 return VBS_SUCCESS;
}


status_t multiplication_P(gp_matrice_t *gp, vecteur_t B, vecteur_t v){
  uint32_t N = gp->size_N/4;

  /*************{    Deuxieme etape : On repasse en dimmension N      }*************\
  |                                                                                 |
  |	   [ 0   I ]    La matrice est de cette forme avec I et J de M_N/4 et       |
  |	   [ 0  -J ]    Elle gere le passage de dimmenssion N/2 a N.                |
  |	   [ J   0 ]       avec I = [1 0 . 0]   J = [0 . 0 1]                       |
  |        [ I   0 ]                [0 1 . .]       [. . 1 0]                       |
  |                                 [. . . 0] ,     [0 . . .]                       |
  |                                 [0 . 0 1]       [1 0 . 0]                       |
  |                                                                                 |
  \*********************************************************************************/
 
  for(uint32_t i = 0; i<N; i++){

    v[i]     =  B[i+N];
    v[i+N]   = -B[2*N-1-i];
    v[i+2*N] =  B[N-1-i];
    v[i+3*N] =  B[i];  

  }
 return VBS_SUCCESS;
}



status_t initialisation_matrices(ensemble_matrice_t **Ens, vorbis_stream_t *stream){
  uint32_t N_0 = stream->codec->blocksize[0];
  uint32_t N_1 = stream->codec->blocksize[1];
  (*Ens) = malloc(sizeof(ensemble_matrice_t)); 
  (*Ens)->M_blocsize_0 = malloc(sizeof(gp_matrice_t));
  (*Ens)->M_blocsize_1 = malloc(sizeof(gp_matrice_t));
  (*Ens)->M_blocsize_0->M = calloc((N_0/2) ,sizeof(sample_t*));
  (*Ens)->M_blocsize_1->M = calloc((N_1/2) ,sizeof(sample_t*));
  (*Ens)->M_blocsize_0->size_N = N_0;
  (*Ens)->M_blocsize_1->size_N = N_1;


  /************{    Premiere etape : On travaille en dimmension N/2      }**********\
  |                                                                                 |
  |     [ M_11  M_12 ]    La matrice est de cette forme avec I et J de M_N/4 et     |
  |     [ M_21  M_22 ]    Elle gere le passage de dimmenssion N/2 a N.              |
  |	                                                                            |   
  |                                                                                 |
  \*********************************************************************************/

  
  for(uint32_t i = 0; i<(N_0/4); i++){
    (*Ens)->M_blocsize_0->M[i]       = calloc((N_0/2) ,sizeof(sample_t));
    (*Ens)->M_blocsize_0->M[i+N_0/4] = calloc((N_0/2) ,sizeof(sample_t));
    for(uint32_t j = 0; j<(N_0/4); j++){
      (*Ens)->M_blocsize_0->M[i][j]             = -cos(M_PI_2*(2*i+1)*(2*j+1)/N_0);
      (*Ens)->M_blocsize_0->M[i][j+N_0/4]       = -cos(M_PI_2*(2*i+1)*(2*j+1+N_0/2)/N_0);
      (*Ens)->M_blocsize_0->M[i+N_0/4][j]       =  cos(M_PI_2*(2*i+1+N_0/2)*(2*j+1)/N_0);
      (*Ens)->M_blocsize_0->M[i+N_0/4][j+N_0/4] =  cos(M_PI_2*(2*i+1+N_0/2)*(2*j+1+N_0/2)/N_0);
      
    }
  }
  for(uint32_t i = 0; i<(N_1/4); i++){
    (*Ens)->M_blocsize_1->M[i]       = calloc(N_1/2 ,sizeof(sample_t));
    (*Ens)->M_blocsize_1->M[i+N_1/4] = calloc(N_1/2 ,sizeof(sample_t));
    for(uint32_t j = 0; j<(N_1/4); j++){
      (*Ens)->M_blocsize_1->M[i][j]             = -cos(M_PI_2*(2*i+1)*(2*j+1)/N_1);
      (*Ens)->M_blocsize_1->M[i][j+N_1/4]       = -cos(M_PI_2*(2*i+1)*(2*j+1+N_1/2)/N_1);
      (*Ens)->M_blocsize_1->M[i+N_1/4][j]       =  cos(M_PI_2*(2*i+1+N_1/2)*(2*j+1)/N_1);
      (*Ens)->M_blocsize_1->M[i+N_1/4][j+N_1/4] =  cos(M_PI_2*(2*i+1+N_1/2)*(2*j+1+N_1/2)/N_1);
    }
  }
  return VBS_SUCCESS;
}


void matrices_free(ensemble_matrice_t *Ens){
  
  for(uint32_t i = 0; i<Ens->M_blocsize_0->size_N/2; i++){
    free(Ens->M_blocsize_0->M[i]);
  }
  for(uint32_t i = 0; i<Ens->M_blocsize_1->size_N/2; i++){
    free(Ens->M_blocsize_1->M[i]);
  }
  
  free(Ens->M_blocsize_0->M);  
  free(Ens->M_blocsize_1->M);
  
  free(Ens->M_blocsize_0);
  free(Ens->M_blocsize_1);  
  free(Ens);
}
