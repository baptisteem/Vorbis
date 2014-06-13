#include "ogg_packet.h"
#include "ogg_internal.h"
#include "stdio.h"


//ce dont je peux avoir besoin dans la struct ogg_packet
//fonction permettant de se situer au debut du prochain packet
//
//attention quand on change de page alors le pcm_handler n est plus le meme bien sur
//il faut le reallouer juste faire passer l'adresse
//quand on finit un packet on ne saute pas forcement de page
//
//verifier en priorite qu'à chaque etape on a bien toutes les infos
//
//table et data sont de la meme taille
#define JUMP 1000

ogg_status_t ogg_packet_attach(internal_ogg_logical_stream_t *lstream){
	
	ogg_packet_handler_t *hdlr=malloc(sizeof(ogg_packet_handler_t));
	hdlr->seg_num=0;
	hdlr->seg_ind=0;
	hdlr->data_ind=0;
	hdlr->packet_ind=1;
	hdlr->position=-1;
	
	lstream->packet=hdlr;

	return OGG_OK;	
}

ogg_status_t ogg_packet_detach(internal_ogg_logical_stream_t *lstream){

	free(lstream->packet);
	return OGG_OK;
}


ogg_status_t ogg_packet_read(ogg_logical_stream_t *lstream, 
                             uint8_t *buf, uint32_t nbytes,
                             uint32_t *nbytes_read){
	
	internal_ogg_logical_stream_t *lstream1=(internal_ogg_logical_stream_t *)lstream;
	uint32_t nbr=0;
	ogg_packet_handler_t *recuperation;

	while ((lstream1->table[lstream1->packet->seg_num] ==255) && (nbytes>255) ){
		
		for (uint32_t i=0; i< 255; i++){
			buf[i+nbr]=lstream1->data[i+lstream1->packet->data_ind];
		}
		nbytes -=255;
		nbr +=255;
		lstream1->packet->seg_ind=0;
		lstream1->packet->packet_ind +=255;
		lstream1->packet->position=-1;


	
		if ((lstream1->packet->seg_num+1) !=(lstream1->header->nb_segs))
		{
			//on est dans un segment interne
			printf("segment interne \n");
			lstream1->packet->seg_num ++;
			lstream1->packet->data_ind +=255;
		}
		else{
			//on est dans le dernier segment d'une page
			printf("dernier segment \n");

			lstream1->packet->seg_num=0;
			lstream1->packet->data_ind=0;
			recuperation=lstream1->packet;
			lstream1= (internal_ogg_logical_stream_t *)lstream1->base.next;
			if (lstream1 == NULL) return OGG_ERR_UNEXP_EOF;
			//sinon on passe à la page suivante
			lstream1->packet=recuperation;
		}
	}
	//sur la deuxiemem partie gerer position et packet_ind
	//et bien mettre en place la recuperation

	if (lstream1->table[lstream1->packet->seg_num] <= nbytes){

		//le nombre de bytes permet au moins de lire tout le packet
		//on est donc alors à la fin d'un segment
		printf("on lit moins ou le nombre exacte de bytes \n");

		uint32_t limite=lstream1->table[lstream1->packet->seg_num];
		limite =-lstream1->packet->seg_ind;

		for (uint32_t i=0; i< limite; i++){
			buf[i+nbr]=lstream1->data[i+lstream1->packet->data_ind];
		}
		nbr +=limite;
		*nbytes_read=nbr;

		if ((lstream1->packet->seg_num+1) !=(lstream1->header->nb_segs)){
		//il reste des segments donc des paquets à lire sur cette page

		}
		else{
		//le packet est le dernier de la page et se finit sur cette page
		
		lstream1= (internal_ogg_logical_stream_t *)lstream1->base.next;
		if (lstream1 == NULL) return OGG_END;

		}			
	}
	else{
		//on peut lire tous les bytes demandés-on est au milieu d'un segment
		printf("on peut lire tous les bytes \n");
		for (uint32_t i=0; i< nbytes; i++){
			buf[i+nbr]=lstream1->data[i+lstream1->packet->data_ind];
		}
		nbr += nbytes;
		*nbytes_read=nbr;
		//mettre à jour toutes les infos
	}


	return OGG_OK;
}

ogg_status_t ogg_packet_next(ogg_logical_stream_t *lstream){
//On fait des petits sauts en utilisant la fonction ogg_packet_read

	uint8_t buf_useless[JUMP];
	uint32_t nbytes_read=JUMP;
	ogg_status_t ret=OGG_OK;

	while ((ret==OGG_OK) && (nbytes_read ==JUMP)){

		ret=ogg_packet_read(lstream,buf_useless,JUMP,&nbytes_read);
	}
	return ret;
}

uint32_t ogg_packet_size(ogg_logical_stream_t *lstream){
	
	internal_ogg_logical_stream_t *lstream1=(internal_ogg_logical_stream_t *)lstream;
	return (lstream1->packet->packet_ind);
}

ogg_status_t ogg_packet_position(ogg_logical_stream_t *lstream,
                                 int64_t *position){
	internal_ogg_logical_stream_t *lstream1=(internal_ogg_logical_stream_t *)lstream;
	*position=lstream1->packet->position;
	return OGG_OK;
}


