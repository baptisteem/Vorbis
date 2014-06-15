#include "ogg_packet.h"
#include "ogg_internal.h"
#include <stdio.h>
#include <inttypes.h>


ogg_status_t ogg_packet_attach(internal_ogg_logical_stream_t *lstream){

	ogg_packet_handler_t *hdlr=malloc(sizeof(ogg_packet_handler_t));
	hdlr->seg_num=0;
	hdlr->seg_ind=0;
	hdlr->data_ind=0;
	hdlr->packet_ind=1;
	hdlr->end=0;
	
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

	//on se situe toujours là ou on va lire
	
	internal_ogg_logical_stream_t *lstream1=(internal_ogg_logical_stream_t *)lstream;
	ogg_status_t ret;
	*nbytes_read=0;
	uint32_t remind_packet_ind=0;
	uint32_t rest=0;//nb d'octets possible de lire dans paquet courant

	if (lstream1->packet->end == 1){
		printf("refus debut \n");
		return OGG_END;
	}

/* Première partie: depart quelconque dans un premier segment quelconque */

	rest=lstream1->table[lstream1->packet->seg_num];
	rest -=lstream1->packet->seg_ind;

	if ( nbytes < rest){

		for (uint32_t i=0; i< nbytes; i++){
			buf[i]=lstream1->data[i+lstream1->packet->data_ind];
		}
		lstream1->packet->packet_ind +=nbytes;
		lstream1->packet->data_ind +=nbytes;
		lstream1->packet->seg_ind +=nbytes;
		*nbytes_read= nbytes;
		return OGG_OK;

	}
	else{

		for (uint32_t i=0; i< rest; i++){
			buf[i]=lstream1->data[i+lstream1->packet->data_ind];
		}
		*nbytes_read +=rest;
		nbytes -=rest;

		if (lstream1->table[lstream1->packet->seg_num] !=255){
			//on est à la fin d'un paquet: on se place bien
			lstream1->packet->seg_ind +=(rest-1);
			lstream1->packet->data_ind +=(rest-1);
			lstream1->packet->packet_ind +=(rest-1);
			lstream1->packet->end =1;
			return OGG_END;
		}
		else{
			//Nous ne sommes pas à la fin d'un paquet: on continue

			if ((lstream1->packet->seg_num+1) !=(lstream1->header->nb_segs)){
				//Il y a des segments après
				lstream1->packet->packet_ind +=rest;
				lstream1->packet->data_ind += rest;
				lstream1->packet->seg_ind =0;
				lstream1->packet->seg_num ++;		
			}
			else{	//On doit changer de page
				lstream1->packet->packet_ind +=rest;
				lstream1->packet->data_ind += rest;
				lstream1->packet->seg_ind =0;
				lstream1->packet->seg_num ++;				
				remind_packet_ind =lstream1->packet->packet_ind;
				
				ret=ogg_get_next_page(lstream1);


				if (ret == OGG_END){
					printf("oggend dans 1ere partie bizarre \n");
					return OGG_ERR_UNEXP_EOF;
				}
				ogg_packet_detach(lstream1);
				ogg_packet_attach(lstream1);
				lstream1->packet->packet_ind=remind_packet_ind;
			}
		}
	}


	/* Deuxième partie: je suis au debut d'un segment
	 * je passe des segments jusqu'à avoir celui ou 
	 * je vais m'arreter de lire (soit le packet se termine, soit le nbytes est
	 * inferieur ou egale à la taille du packet */

	
	while ((lstream1->table[lstream1->packet->seg_num]==255) && (nbytes > lstream1->table[lstream1->packet->seg_num])){

		printf("deuxieme partie \n");
		for (uint32_t i=0; i< 255; i++){
			buf[i+(*nbytes_read)]=lstream1->data[i+lstream1->packet->data_ind];
		}
		*nbytes_read +=255;
		nbytes -=255;

		if ((lstream1->packet->seg_num+1) !=(lstream1->header->nb_segs)){
		//Il reste des packets après
			lstream1->packet->packet_ind +=255;
			lstream1->packet->data_ind += 255;
			lstream1->packet->seg_ind =0;
			lstream1->packet->seg_num ++;	

		}
		else{
		//Il faut changer de page
				lstream1->packet->packet_ind +=rest;
				lstream1->packet->data_ind += rest;
				lstream1->packet->seg_ind =0;
				lstream1->packet->seg_num ++;				
				remind_packet_ind =lstream1->packet->packet_ind;


			ret=ogg_get_next_page(lstream1);
			if (ret == OGG_END){
				printf("oggend dans 2eme partie bizarre \n");
				return OGG_ERR_UNEXP_EOF;
			}
			ogg_packet_detach(lstream1);
			ogg_packet_attach(lstream1);
			lstream1->packet->packet_ind=remind_packet_ind;
		}
	}

	/* Troisième partie:au debut du dernier segment que je vais lire
	 * en sachant que j ai au moins passé le premier segment */
	

	if ( nbytes < lstream1->table[lstream1->packet->seg_num])
	{
		//Je ne lis pas toute la page
		for (uint32_t i=0; i< nbytes; i++){
			buf[i+(*nbytes_read)]=lstream1->data[i+lstream1->packet->data_ind];
		}
		lstream1->packet->packet_ind +=nbytes;
		lstream1->packet->data_ind +=nbytes;
		lstream1->packet->seg_ind +=nbytes;
		*nbytes_read += nbytes;
		return OGG_OK;
	}
	else{
		uint32_t taille=lstream1->table[lstream1->packet->seg_num];

		for (uint32_t i=0; i< taille; i++){
			buf[i+(*nbytes_read)]=lstream1->data[i+lstream1->packet->data_ind];
		}

		if (taille != 255){
			//on se place bien
			lstream1->packet->seg_ind +=(taille-1);
			lstream1->packet->data_ind +=(taille-1);
			lstream1->packet->packet_ind +=(taille-1);
			lstream1->packet->end =1;
			*nbytes_read +=taille;
			return OGG_END;
		}
		else{
		 	//on lit 255; on est pas dans le dernier packet
			printf("error rien à faire ici \n");
			return OGG_ERR_UNEXP_EOF;
		}
	}
	
}









ogg_status_t ogg_packet_next(ogg_logical_stream_t *lstream){

	internal_ogg_logical_stream_t *lstream1=(internal_ogg_logical_stream_t *)lstream;

	ogg_status_t ret;

	//On avance jusqu'àu dernier segment du packet
	while(lstream1->table[lstream1->packet->seg_num] == 255){
		if ((lstream1->packet->seg_num+1) !=(lstream1->header->nb_segs)){
			//soit il reste des segments derrière
			lstream1->packet->data_ind +=lstream1->table[lstream1->packet->seg_num];
			lstream1->packet->data_ind -=lstream1->packet->seg_ind;
			lstream1->packet->packet_ind +=lstream1->table[lstream1->packet->seg_num];
			lstream1->packet->packet_ind -=lstream1->packet->seg_ind;
			lstream1->packet->seg_num ++;
			lstream1->packet->seg_ind=0;		
		}
		else{
			//soit on doit changer de page
			ogg_packet_detach(lstream1);
			ret=ogg_get_next_page(lstream1);
			if (ret ==OGG_END){

			       	printf("fin fichier bizarre dans next packet \n");
				return OGG_ERR_UNEXP_EOF;
			}
			ogg_packet_attach(lstream1);
		}
	}

	//On est sur le dernier segment d'un paquet
	if ((lstream1->packet->seg_num+1) !=(lstream1->header->nb_segs)){
		//Il reste des segments donc des paquets à lire sur cette page
		lstream1->packet->data_ind +=lstream1->table[lstream1->packet->seg_num];
		lstream1->packet->data_ind -=lstream1->packet->seg_ind;
		lstream1->packet->seg_ind=0;
		lstream1->packet->seg_num ++;
		lstream1->packet->packet_ind=1;
		lstream1->packet->end=0;
		return OGG_OK;
	}
	else{
		//le packet est le dernier de la page et se finit sur cette page
		lstream1->packet->packet_ind +=lstream1->table[lstream1->packet->seg_num];
		lstream1->packet->packet_ind -=lstream1->packet->seg_ind;
		ogg_packet_detach(lstream1);
		ret=ogg_get_next_page(lstream1);
		if (ret ==OGG_END) 
		{
			ogg_packet_attach(lstream1);
			printf("oggend dans next packet");
			return OGG_END;
		}
		ogg_packet_attach(lstream1);
		return OGG_OK;
	}
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
