#include <getopt.h>
#include <libgen.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include "ogg_core.h"
#include "pcm_handler.h"

void display_list_stream(ogg_physical_stream_t *ogg_stream);
void display_help();
ogg_status_t decode_ogg_id_stream(long id, ogg_physical_stream_t *ogg_stream, pcm_handler_t *pcm);

int main(int argc, char *argv[]) {

  FILE *file = NULL;
  uint32_t c;
  char format[] = "hls:f:o:";
  uint8_t list_stream = 0;
  long id = -1;
  char *outformat = "wav";
  char outfile[255] = "\0";
  ogg_status_t ret;

  while( (c = getopt(argc,argv,format)) != -1){
    switch(c){
      case 'h':
        display_help();
        return 0;
        break;
      case 'l':
        list_stream = 1;
        break;
      case 's':
        id = (long) atoi(optarg);
        break;
      case 'f':
        outformat = optarg;
        break;
      case 'o':
        strcpy(outfile,optarg);
        break;
      case ':':       /* -f or -o without operand */
        printf("%s: option require an argument -- '%c'\n", argv[0], optopt);
        break;
      case '?':
        return 0;
        break;
    }
  }

  //If we don't have a filename
  if(strcmp(outfile,"")==0){
    strcpy(outfile,basename(argv[optind]));
    strcat(outfile,".");
    strcat(outfile,outformat);
  }

  file = fopen(argv[optind], "r");
  if(file != NULL){

    ogg_physical_stream_t *physical_stream = NULL;

    ret = ogg_init(file,&physical_stream);
    if(ret != OGG_OK){
      printf("Aucun flux physique disponible.\n");
      return 0;
    }

    //If we want to list all logical streams
    if(list_stream==1){
      display_list_stream(physical_stream);
      return 0;
    }

    pcm_handler_t *pcm_handler = pcm_handler_create(outformat,outfile);

    //If we don't ask for a specific stream to decode
    if(id == -1){
      ret = ogg_decode(physical_stream->first, pcm_handler);
      if(ret != OGG_OK)
        return ret;
    }
    else{
      ret = decode_ogg_id_stream(id, physical_stream, pcm_handler);
      if(ret != OGG_OK)
        return ret;
    }

    pcm_handler_delete(pcm_handler);

    ret = ogg_term(physical_stream);
    if(ret != OGG_OK)
      return ret;
  }
  else
    printf("Failed to open input file '%s': No such file or directory\n", argv[1]);

  return 0;
}

ogg_status_t decode_ogg_id_stream(long id, ogg_physical_stream_t *ogg_stream, pcm_handler_t *pcm){

  ogg_status_t ret;
  ogg_logical_stream_t *tmp = ogg_stream->first;

  //We are searching for the appropriate logical stream
  while(tmp != NULL){

    if(tmp->stream_id == id){

      ret = ogg_decode(ogg_stream->first, pcm);
      if(ret != OGG_OK)
        return ret;

      return OGG_OK;
    }
    tmp = tmp->next;
  }

  printf("Stream with id %ld not found\n", id);

  return OGG_ERR_ILL_OP;
}

/* Display list of all logical stream in a physical stream */
void display_list_stream(ogg_physical_stream_t *ogg_stream){

  uint8_t cpt = 0;
  ogg_logical_stream_t *tmp = ogg_stream->first;

  printf("%d logical stream(s):\n", ogg_stream->nb_streams);
  while(tmp != NULL){

    if(tmp->codec == OGG_VORBIS)
      printf("\t%d: codec='vorbis', id=%d\n", cpt, tmp->stream_id);
    else
      printf("\t%d: codec='unknown', id=%d\n", cpt, tmp->stream_id);

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
      "\t\t\t available formats are:\n");
  pcm_handler_list("");
}
