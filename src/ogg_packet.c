#include "ogg_packet.h"
#include "ogg_internal.h"
#include <stdio.h>
#include <inttypes.h>


//ce dont je peux avoir besoin dans la struct ogg_packet
//fonction permettant de se situer au debut du prochain packet
//
//attention quand on change de page alors le pcm_handler n est plus le meme bien sur
//il faut le reallouer juste faire passer l'adresse
//quand on finit un packet on ne saute pas forcement de page
//
//verifier en priorite qu'à chaque etape on a bien toutes les infos
//
//remplacer tous les 255 selon leur sens par les define !!!
//
//table et data sont de la meme taille
#define JUMP 255

ogg_status_t ogg_packet_attach(internal_ogg_logical_stream_t *lstream){

	//printf("attach \n");
	ogg_packet_handler_t *hdlr=malloc(sizeof(ogg_packet_handler_t));
	hdlr->seg_num=0;
	hdlr->seg_ind=0;
	hdlr->data_ind=0;
	hdlr->packet_ind=1;
	
	lstream->packet=hdlr;
	return OGG_OK;	
}

ogg_status_t ogg_packet_detach(internal_ogg_logical_stream_t *lstream){
	//printf("detach \n");
	free(lstream->packet);
	return OGG_OK;
}

void printf_handler(internal_ogg_logical_stream_t *lstream1){
/*
 	ogg_packet_handler_t *hdlr=lstream1->packet;
	uint8_t *table=lstream1->table;

	printf(" \n seg_num %d seg_ind %d data_ind %d packet_ind %d \n",hdlr->seg_num,hdlr->seg_ind,hdlr->data_ind,hdlr->packet_ind);
	printf("taille du segment courant %d nb de segments dans la page %d position", table[hdlr->seg_num],lstream1->header->nb_segs);

*/
}





ogg_status_t ogg_packet_read(ogg_logical_stream_t *lstream, 
                             uint8_t *buf, uint32_t nbytes,
                             uint32_t *nbytes_read){

	internal_ogg_logical_stream_t *lstream1=(internal_ogg_logical_stream_t *)lstream;
	uint32_t nbr=0;
	ogg_status_t ret;
	uint32_t sz=255-lstream1->packet->seg_ind;
	uint32_t index_packet=0;

	while ((lstream1->table[lstream1->packet->seg_num] ==255) && (nbytes>=sz) ){

		//on va arriver à la fin d'un segment (sz bytes à parcourir)
		//car pour le segment courant on peut etre au debut, comme on
		//peut etre situé au milieu
		printf("entree dans la boucle qui mange la fin des segments en entier \n");
		for (uint32_t i=0; i< sz; i++){
			buf[i+nbr]=lstream1->data[i+lstream1->packet->data_ind];
		}

		index_packet=lstream1->packet->packet_ind +sz;

		if ((lstream1->packet->seg_num+1) !=(lstream1->header->nb_segs))
		{
			//on est dans un segment interne
			printf("segment interne \n");
			lstream1->packet->seg_num ++;
			lstream1->packet->data_ind +=sz;
		}
		else{
			//on est dans le dernier segment d'une page
			printf("dernier segment \n");
			ogg_packet_detach(lstream1);
			ret=ogg_get_next_page(lstream1);
			if (ret ==OGG_END) return OGG_ERR_UNEXP_EOF;
			ogg_packet_attach(lstream1);

		}

		nbytes -=sz;
		nbr +=sz;
		lstream1->packet->seg_ind=0;
		lstream1->packet->packet_ind =index_packet;
		sz=255;//forcement les fois suivantes on sera au debut d'un segment
	}

	//on est dans le dernier segment du packet
	sz=lstream1->table[lstream1->packet->seg_num];
	sz -=lstream1->packet->seg_ind;
	//printf("sz %d \n",sz);
	if (sz <= nbytes){
		
		//On est placé de manière quelconque sur le segment
		//On arrive à la fin d'un segment, donc on a lu tout le packet
		//printf("on va etre à la fin du dernier segment, config avant avancer \n");
		printf_handler(lstream1);
		//printf("nbytes %d sz %d \n", nbytes, sz);

		for (uint32_t i=0; i< sz; i++){
			buf[i+nbr]=lstream1->data[i+lstream1->packet->data_ind];
		}
		nbr +=sz;
		lstream1->packet->seg_ind  +=sz;
		lstream1->packet->data_ind +=sz;
		*nbytes_read=nbr;
		return OGG_END;
	}
	else{
		//On est placé de manière quelconque sur le segment
		//On arrive au milieu d'un segment-le packet n est pas fini
		//printf("on peut lire tous les bytes, config avant avancer ");
		printf_handler(lstream1);
		for (uint32_t i=0; i< nbytes; i++){
			buf[i+nbr]=lstream1->data[i+lstream1->packet->data_ind];
		}
		nbr += nbytes;
		*nbytes_read=nbr;
		lstream1->packet->packet_ind +=nbytes;
		lstream1->packet->data_ind +=nbytes;
		lstream1->packet->seg_ind +=nbytes;
		return OGG_OK;

	}
}

//Necessaire:il faut que data_ind correpondent au debut du paquet 
ogg_status_t ogg_packet_next(ogg_logical_stream_t *lstream){

	internal_ogg_logical_stream_t *lstream1=(internal_ogg_logical_stream_t *)lstream;

	ogg_status_t ret;
//	printf("ogg_packet_next \n");

	//On avance jusqu'àu dernier segment du packet
	while(lstream1->table[lstream1->packet->seg_num] == 255){
		if ((lstream1->packet->seg_num+1) !=(lstream1->header->nb_segs)){
			//soit il reste des segments derrière
			lstream1->packet->data_ind +=lstream1->table[lstream1->packet->seg_num];
			lstream1->packet->seg_num ++;
			lstream1->packet->seg_ind=0;		
		}
		else{
			//soit on doit changer de page
			ogg_packet_detach(lstream1);
			ret=ogg_get_next_page(lstream1);
			if (ret ==OGG_END) return OGG_ERR_UNEXP_EOF;
			ogg_packet_attach(lstream1);
		}
	}

	//On est sur le dernier segment d'un paquet
	if ((lstream1->packet->seg_num+1) !=(lstream1->header->nb_segs)){
		//Il reste des segments donc des paquets à lire sur cette page
		printf("reste des segments sur la page \n");
		lstream1->packet->seg_ind=0;
		lstream1->packet->seg_num ++;
		lstream1->packet->packet_ind=0;
	}
	else{
		//le packet est le dernier de la page et se finit sur cette page
//		printf("dernier segment de la page \n");
		ogg_packet_detach(lstream1);
		ret=ogg_get_next_page(lstream1);
		ogg_packet_attach(lstream1);
		if (ret ==OGG_END) return OGG_END;//on est alors à la fin
	}
	return OGG_OK;
}

uint32_t ogg_packet_size(ogg_logical_stream_t *lstream){
	
	internal_ogg_logical_stream_t *lstream1=(internal_ogg_logical_stream_t *)lstream;
	return (lstream1->packet->packet_ind);
}

ogg_status_t ogg_packet_position(ogg_logical_stream_t *lstream,
                                 int64_t *position){
	
	internal_ogg_logical_stream_t *lstream1=(internal_ogg_logical_stream_t *)lstream;

	if ((lstream1->packet->seg_num+1 == lstream1->header->nb_segs) && (lstream1->table[lstream1->packet->seg_num] != 255)){
		*position=lstream1->header->gran_pos;
	}
	else{
		*position=-1;
	}
	return OGG_OK;
}


