#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include "ogg_core.h"

void display_list_stream(ogg_physical_stream_t *ogg_stream);
void display_help();

int main(int argc, char *argv[]) {

  FILE *file = NULL;
  uint32_t c;
  char format[] = "hls:f:o:";
  uint8_t list_stream = 0;

  while( (c = getopt(argc,argv,format)) != -1){
    switch(c){
      case 'h':
        display_help();
        break;
      case 'l':
        list_stream = 1;
        break;
      case 's':
        break;
      case 'f':
        break;
      case 'o':
        break;
      case '?':
        printf("%s: invalid option -- '%c'\n", argv[0], c);
        break;
    }
  }

  file = fopen(argv[1], "r");
  if(file != NULL){
  
    ogg_physical_stream_t *physical_stream = NULL;
    
    if(ogg_init(file,&physical_stream) == OGG_OK){
      
      if(list_stream==1)
        display_list_stream(physical_stream);
    
    }
    else
      printf("Aucun flux physique disponible.");

  }
  else
    printf("Failed to open input file '%s': No such file or directory\n", argv[1]);

  return 0;
}

void display_list_stream(ogg_physical_stream_t *ogg_stream){

  uint8_t cpt = 0;
  ogg_logical_stream_t *tmp = ogg_stream->first;

  printf("%d logical stream(s):\n", ogg_stream->nb_streams);
  while(tmp != NULL)
  {
    if(tmp->codec == OGG_VORBIS)
      printf("%d: codec='vorbis', id=%d", cpt, tmp->stream_id);
    else
      printf("%d: codec='unknown', id=%d", cpt, tmp->stream_id);

    tmp = tmp->next;
    cpt++;
  }

}

void display_help(){

  printf("usage: ./vorbis_decoder_ref [-hl] [-f <format>] [-o <outfile>] <infile>\n\n"
      " Decode the ogg/vorbis file <infile>.\n"
      " Default format is 'wav'.\n"
      " Default output file name is 'basename(<infile>).<format>'.\n"
      " If not stream id is specified, the first logical stream\n"
      " found in the file is decoded\n\n"
      " Options:\n"
      "\t -h : print this help message.\n"
      "\t -l : list the streams of the ogg file, don't decode  anything\n"
      "\t -s <id> : decode the stream of the given id\n"
      "\t -o <outfile> : set the output file name to <outfile>\n"
      "\t -f <format> : set the ouput format to <format>.\n"
      "\t\t\t available formats are:\n"
      " \t\t\t\t\t\twav\n"
      " \t\t\t\t\t\traw\n");
}
