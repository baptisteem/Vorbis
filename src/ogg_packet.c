#include "ogg_packet.h"
#include "ogg_internal.h"
#include <stdio.h>
#include <inttypes.h>

int c1=0;//1ère manière de compter le nb de bytes parcourus
int c2=0;//2ème manière de compter le nb de bytes parcourus

int total_taille(internal_ogg_logical_stream_t *lstream1){
		int aux=0;
		for(uint32_t i=0; i<lstream1->header->nb_segs; i++){
			aux += lstream1->table[i];	
		}
		return aux;
}

ogg_status_t ogg_packet_attach(internal_ogg_logical_stream_t *lstream){
	
	ogg_packet_handler_t *hdlr=malloc(sizeof(ogg_packet_handler_t));
	hdlr->seg_num=0;
	hdlr->seg_ind=0;
	hdlr->data_ind=0;
	hdlr->packet_ind=1;
	hdlr->end=0;
	hdlr->position=0;
	
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
	uint32_t rest=0;






	//Tant que le champ end de packet vaut 1:
	// cela signifie que l'on a atteint la fin d'une page et on
	// attend un next page
	if (lstream1->packet->end == 1){
		return OGG_END;
	}

/********** Première partie: depart quelconque dans un premier segment quelconque ******/

	//On calcule le nb de bytes qu'il reste dans le segment
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
		c2 +=nbytes;
		return OGG_OK;

	}
	else{

		for (uint32_t i=0; i< rest; i++){
			buf[i]=lstream1->data[i+lstream1->packet->data_ind];
		}
		*nbytes_read +=rest;
		c2 += rest;
		nbytes -=rest;

		if (lstream1->table[lstream1->packet->seg_num] !=OGG_PAGE_SEG_MAX_SIZE){
			//on est à la fin d'un paquet: on se place bien
			//pour pouvoir ensuite changer de packet
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
				c1 +=total_taille(lstream1);
				ogg_packet_detach(lstream1);
				ogg_packet_attach(lstream1);
				ret=ogg_get_next_page(lstream1);

				if (ret == OGG_END){
					return OGG_ERR_UNEXP_EOF;
				}
				lstream1->packet->packet_ind=remind_packet_ind;
			}
		}
	}

/********** Deuxième partie: je suis au debut d'un segment
	 * je passe des segments jusqu'à avoir celui ou 
	 * je vais m'arreter de lire (dans le segment où on s'arrete
	 * soit le packet se termine, soit le nbytes est
	 * inferieur ou egal à la taille du packet ********************************/

	
	while ((lstream1->table[lstream1->packet->seg_num]==OGG_PAGE_SEG_MAX_SIZE) && (nbytes > lstream1->table[lstream1->packet->seg_num])){


		for (uint32_t i=0; i< OGG_PAGE_SEG_MAX_SIZE ; i++){
			buf[i+(*nbytes_read)]=lstream1->data[i+lstream1->packet->data_ind];
		}
		*nbytes_read +=OGG_PAGE_SEG_MAX_SIZE ;
		c2 += OGG_PAGE_SEG_MAX_SIZE ;
		nbytes -=OGG_PAGE_SEG_MAX_SIZE ;

		if ((lstream1->packet->seg_num+1) !=(lstream1->header->nb_segs)){
			//Il reste des packets après
			lstream1->packet->packet_ind +=OGG_PAGE_SEG_MAX_SIZE ;
			lstream1->packet->data_ind += OGG_PAGE_SEG_MAX_SIZE ;
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

			c1 +=total_taille(lstream1);
			ogg_packet_detach(lstream1);
			ogg_packet_attach(lstream1);
			ret=ogg_get_next_page(lstream1);
			if (ret == OGG_END){
				return OGG_ERR_UNEXP_EOF;
			}
			lstream1->packet->packet_ind=remind_packet_ind;
		}
	}

/********* Troisième partie:au debut du dernier segment que je vais lire
	 * en sachant que j ai au moins passé le premier segment **********************/
	
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
		c2 +=nbytes;
		return OGG_OK;
	}
	else{

		uint32_t taille=lstream1->table[lstream1->packet->seg_num];

		for (uint32_t i=0; i< taille; i++){
			buf[i+(*nbytes_read)]=lstream1->data[i+lstream1->packet->data_ind];
		}
	
		//on se place bien
		lstream1->packet->seg_ind +=(taille-1);
		lstream1->packet->data_ind +=(taille-1);
		lstream1->packet->packet_ind +=(taille-1);
		lstream1->packet->end =1;
		*nbytes_read +=taille;
		c2 += taille;

		return OGG_END;
	}
	
	
}




/*Remarques:
 * Je suis bien conscient que les parties 1 et 2 auraient pu être regroupées
 * (ce qui était le cas dans mon algorithme initial). Cependant pour un soucis 
 * de clarté lors de la tentative de debeugage de l'erreur obtenu avec Tiersen,
 * j'ai preféré bien pouvoir identifier les étapes de la lecture.
 *
 * Il n'y a ni fuites, ni erreurs et un rms=0 pour windows, mioumiou et tromboon.
 * Cependant avec le fichier Tiersen, il n'y a ni fuite, ni erreurs lors de l'execution.
 * Le fichier généré possède 32octets de plus que l'original (ces 32 octets supplémentaires
 * sont à la fin du fichier généré). Par conséquent les champs dans les entetes,
 * où est stocké la taille du fichier -X (ou X vaut 8 ou 44), diffèrent.Le reste 
 * du fichier généré est identhique à l'original. En effet en modifiant manuellement
 * l'entete  et supprimant les 32 octets finaux du fichier généré manuellement,
 * on obtient un rms=0. 
 */




ogg_status_t ogg_packet_next(ogg_logical_stream_t *lstream){
	

	internal_ogg_logical_stream_t *lstream1=(internal_ogg_logical_stream_t *)lstream;
	
	ogg_status_t ret;

	//On avance jusqu'àu dernier segment du packet
	while(lstream1->table[lstream1->packet->seg_num] == OGG_PAGE_SEG_MAX_SIZE){
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
			c1 +=total_taille(lstream1);


			ogg_packet_detach(lstream1);
			ogg_packet_attach(lstream1);

			ret=ogg_get_next_page(lstream1);



			if (ret ==OGG_END){
				

			       	printf("fin fichier bizarre dans next packet \n");
				return OGG_ERR_UNEXP_EOF;
			}
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
	
		c1 +=total_taille(lstream1);

		ogg_packet_detach(lstream1);
		ogg_packet_attach(lstream1);

		ret=ogg_get_next_page(lstream1);
		
	 		
		if (ret ==OGG_END) 
		{

			printf("gran pos %d \n",lstream1->header->gran_pos);
			printf(" Nb à lire de bytes: %d Nb de bytes lus: %d \n",c1,c2);
			if (c1==c2){
			printf("L'integralité des bytes disponibles a été lu \n");
			}
			else{
			printf("Tous les bytes n'ont pas été lus\n");
			}
			return OGG_END;
		}

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

		if ((lstream1->packet->seg_num+1 == lstream1->header->nb_segs) && (lstream1->table[lstream1->packet->seg_num] != OGG_PAGE_SEG_MAX_SIZE )){
			*position=(int64_t)lstream1->header->gran_pos;
		}
		else{
			*position=-1;
		}
	
	return OGG_OK;
}






