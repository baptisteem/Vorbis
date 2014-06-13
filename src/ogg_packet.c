#include "ogg_packet.h"
#include "ogg_internal.h"


//ce dont je peux avoir besoin dans la struct ogg_packet
//
//fonction permettant de se situer au debut du prochain packet

#define JUMP 1000

ogg_status_t ogg_packet_attach(internal_ogg_logical_stream_t *lstream){
	
	ogg_packet_handler *hdlr=malloc(sizeof(ogg_packet_handler_t));
	hdlr->seg_num=0;
	hdlr->seg_ind=0;
	hdlr->data_ind=0;
	hdlr->
	hdlr->packet_curr_size=0;

	lstream->packet=hdlr;	
}

ogg_status_t ogg_packet_detach(internal_ogg_logical_stream_t *lstream){

	free(lstream->packet);
}



ogg_status_t ogg_packet_read(ogg_logical_stream_t *lstream, 
                             uint8_t *buf, uint32_t nbytes,
                             uint32_t *nbytes_read){
	
	internal_ogg_logical_stream_t *lstream1=(internal_ogg_logical_stream_t *)lstream;
	uint32_t nbr=0;

	while ((lstream1->table[lstream1->packet->seg_num] ==255) && (nbytes>255) ){
		
		for (uint32_t i=0; i< 255; i++){
			buf[i+nbr]=lstream1->data[i+lstream1->packet->data_ind];
		}
		nbytes -=255;
		nbr +=255;

	
		if (lstream1->packet->seg_num !=(lstream1->header->nb_segs-1))
		{
			//on est dans un packet interne
			printf("packet interne \n");
			lstream1->packet->seg_num ++;
			lstream1->packet->seg_ind=0;
			lstream1->packet->data_ind +=255;
		}
		else{
			//on est dans le dernier packet d'une page
			printf("dernier packet \n");
			lstream1->packet->seg_num=0;
			lstream1->packet->data_ind=0;
			lstream1->packet->seg_ind=0;
			lstream1= (internal_ogg_logical_stream_t *)lstream1->base.next;
			if (lstream1 ==NULL) return OGG_END;
		}
	}

	if (lstream1->table[lstream1->packet->seg_num] < nbytes){

		//le nombre de bytes à lire nous ferais sortir du packet
		printf("on ne peut pas lire tous les bytes \n");
		uint32_t limite=lstream1->table[lstream1->packet->seg_num];
		limite =-lstream1->packet->seg_ind;
		for (uint32_t i=0; i< limite; i++){
			buf[i+nbr]=lstream1->data[i+lstream1->packet->data_ind];
		}
		nbr +=limite;		
	
	}
	else{
		//on peut lire tous les bytes dans le packet
		printf("on peut lire tous les bytes \n");
		for (uint32_t i=0; i< nbytes; i++){
			buf[i+nbr]=lstream->data[i+lstream1->packet->data_ind];
		}
		nbr += nbytes;
	}
	//faire un test pour voir si on etait pas dans le dernier packet?

	*nbytes_read=nbr;
	return OGG_OK;
}

ogg_status_t ogg_packet_next(ogg_logical_stream_t *lstream){
//On fait des petits sauts en utilisant la fonction ogg_packet_read

	uint8_t buf_useless[JUMP];
	uint32_t *nbytes_read;
	*nbytes_read=JUMP;
	ogg_status ret=OGG_OK;

	while ((ret==OGG_OK) && (nbytes_read ==JUMP)){

		ret=ogg_packet_read(lstream,buf_useless,JUMP,*nbytes_read);
	}

	return ret;
}

uint32_t ogg_packet_size(ogg_logical_stream_t *lstream){

	return 0;
}

ogg_status_t ogg_packet_position(ogg_logical_stream_t *lstream,
                                 int64_t *position){

	return OGG_OK;
}


