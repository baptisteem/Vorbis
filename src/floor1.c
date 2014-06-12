#include "helpers.h"
#include <floor.h>
#include "floor1.h"
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "vorbis_io.h"
#include "sample.h"
#include "codebook.h"

#define MAX(x, y) (((x) > (y)) ? (x) : (y))
#define MIN(x, y) (((x) < (y)) ? (x) : (y))

typedef struct floor_type1 floor_type1_t;
typedef struct floor_type1_data floor_type1_data_t;
typedef struct line_subclass_books line_subclass_books_t;
typedef struct qsort_struct qsort_struct_t;

struct floor_type1_data {

	floor_data_t base;
	uint32_t size; //Max des floor1_Size_X_List
	uint32_t *floor1_Y;
	uint32_t *floor1_final_Y;
	uint32_t *floor1_final_Y_o;
	uint32_t *floor1_step2_flag;
	uint32_t *floor1_step2_flag_o;
	uint32_t *floor1_X_list_o;
	qsort_struct_t * tri;
};


struct line_subclass_books{
	int32_t *tab;
};

struct floor_type1 {

	floor_t base;
	uint32_t floor1_partitions;
	uint32_t *floor1_partition_class_list;
	uint32_t *floor1_class_dimensions;
	uint32_t *floor1_class_subclasses;
	uint32_t *floor1_class_masterbooks;
	line_subclass_books_t *floor1_subclass_books;
	uint32_t floor1_multiplier;
	uint32_t floor1_values;
	uint32_t floor1_maximum_classplusun;
	uint32_t floor1_size_X_list;
	uint32_t *floor1_X_list;
};

struct qsort_struct {
	uint32_t elm;	
	uint32_t ind;
};


static const sample_t inverse_dB_table[256] = {
	1.0649863e-07, 1.1341951e-07, 1.2079015e-07, 1.2863978e-07,
	1.3699951e-07, 1.4590251e-07, 1.5538408e-07, 1.6548181e-07,
	1.7623575e-07, 1.8768855e-07, 1.9988561e-07, 2.1287530e-07,
	2.2670913e-07, 2.4144197e-07, 2.5713223e-07, 2.7384213e-07,
	2.9163793e-07, 3.1059021e-07, 3.3077411e-07, 3.5226968e-07,
	3.7516214e-07, 3.9954229e-07, 4.2550680e-07, 4.5315863e-07,
	4.8260743e-07, 5.1396998e-07, 5.4737065e-07, 5.8294187e-07,
	6.2082472e-07, 6.6116941e-07, 7.0413592e-07, 7.4989464e-07,
	7.9862701e-07, 8.5052630e-07, 9.0579828e-07, 9.6466216e-07,
	1.0273513e-06, 1.0941144e-06, 1.1652161e-06, 1.2409384e-06,
	1.3215816e-06, 1.4074654e-06, 1.4989305e-06, 1.5963394e-06,
	1.7000785e-06, 1.8105592e-06, 1.9282195e-06, 2.0535261e-06,
	2.1869758e-06, 2.3290978e-06, 2.4804557e-06, 2.6416497e-06,
	2.8133190e-06, 2.9961443e-06, 3.1908506e-06, 3.3982101e-06,
	3.6190449e-06, 3.8542308e-06, 4.1047004e-06, 4.3714470e-06,
	4.6555282e-06, 4.9580707e-06, 5.2802740e-06, 5.6234160e-06,
	5.9888572e-06, 6.3780469e-06, 6.7925283e-06, 7.2339451e-06,
	7.7040476e-06, 8.2047000e-06, 8.7378876e-06, 9.3057248e-06,
	9.9104632e-06, 1.0554501e-05, 1.1240392e-05, 1.1970856e-05,
	1.2748789e-05, 1.3577278e-05, 1.4459606e-05, 1.5399272e-05,
	1.6400004e-05, 1.7465768e-05, 1.8600792e-05, 1.9809576e-05,
	2.1096914e-05, 2.2467911e-05, 2.3928002e-05, 2.5482978e-05,
	2.7139006e-05, 2.8902651e-05, 3.0780908e-05, 3.2781225e-05,
	3.4911534e-05, 3.7180282e-05, 3.9596466e-05, 4.2169667e-05,
	4.4910090e-05, 4.7828601e-05, 5.0936773e-05, 5.4246931e-05,
	5.7772202e-05, 6.1526565e-05, 6.5524908e-05, 6.9783085e-05,
	7.4317983e-05, 7.9147585e-05, 8.4291040e-05, 8.9768747e-05,
	9.5602426e-05, 0.00010181521, 0.00010843174, 0.00011547824,
	0.00012298267, 0.00013097477, 0.00013948625, 0.00014855085,
	0.00015820453, 0.00016848555, 0.00017943469, 0.00019109536,
	0.00020351382, 0.00021673929, 0.00023082423, 0.00024582449,
	0.00026179955, 0.00027881276, 0.00029693158, 0.00031622787,
	0.00033677814, 0.00035866388, 0.00038197188, 0.00040679456,
	0.00043323036, 0.00046138411, 0.00049136745, 0.00052329927,
	0.00055730621, 0.00059352311, 0.00063209358, 0.00067317058,
	0.00071691700, 0.00076350630, 0.00081312324, 0.00086596457,
	0.00092223983, 0.00098217216, 0.0010459992, 0.0011139742,
	0.0011863665, 0.0012634633, 0.0013455702, 0.0014330129,
	0.0015261382, 0.0016253153, 0.0017309374, 0.0018434235,
	0.0019632195, 0.0020908006, 0.0022266726, 0.0023713743,
	0.0025254795, 0.0026895994, 0.0028643847, 0.0030505286,
	0.0032487691, 0.0034598925, 0.0036847358, 0.0039241906,
	0.0041792066, 0.0044507950, 0.0047400328, 0.0050480668,
	0.0053761186, 0.0057254891, 0.0060975636, 0.0064938176,
	0.0069158225, 0.0073652516, 0.0078438871, 0.0083536271,
	0.0088964928, 0.009474637, 0.010090352, 0.010746080,
	0.011444421, 0.012188144, 0.012980198, 0.013823725,
	0.014722068, 0.015678791, 0.016697687, 0.017782797,
	0.018938423, 0.020169149, 0.021479854, 0.022875735,
	0.024362330, 0.025945531, 0.027631618, 0.029427276,
	0.031339626, 0.033376252, 0.035545228, 0.037855157,
	0.040315199, 0.042935108, 0.045725273, 0.048696758,
	0.051861348, 0.055231591, 0.058820850, 0.062643361,
	0.066714279, 0.071049749, 0.075666962, 0.080584227,
	0.085821044, 0.091398179, 0.097337747, 0.10366330,
	0.11039993, 0.11757434, 0.12521498, 0.13335215,
	0.14201813, 0.15124727, 0.16107617, 0.17154380,
	0.18269168, 0.19456402, 0.20720788, 0.22067342,
	0.23501402, 0.25028656, 0.26655159, 0.28387361,
	0.30232132, 0.32196786, 0.34289114, 0.36517414,
	0.38890521, 0.41417847, 0.44109412, 0.46975890,
	0.50028648, 0.53279791, 0.56742212, 0.60429640,
	0.64356699, 0.68538959, 0.72993007, 0.77736504,
	0.82788260, 0.88168307, 0.9389798, 1
};

void floor_type1_free(floor_t *fl)
{
     	floor_type1_t *floor1 = (floor_type1_t *) fl;

	for (uint32_t i=0; i<floor1->floor1_maximum_classplusun; i++){
		free(((floor1->floor1_subclass_books)[i]).tab);
	}

	free(floor1->floor1_subclass_books);
	free(floor1->floor1_partition_class_list);
	free(floor1->floor1_class_dimensions);
	free(floor1->floor1_class_subclasses);
	free(floor1->floor1_class_masterbooks);
	free(floor1->floor1_X_list);

	free(floor1);
	return;
}

status_t floor_type1_hdr_decode(vorbis_stream_t *stream, uint8_t id,
				floor_t **floor, floor_data_t *data)
{
	floor_type1_t *floor1 = calloc(1, sizeof(floor_type1_t));
	floor1->base.type=1;
	floor1->base.id=id;
	floor1->base.decode=floor_type1_decode;
	floor1->base.free=floor_type1_free;

	floor_type1_data_t *data1=(floor_type1_data_t *) data;

	uint32_t p_count, dst;
	status_t stat;

	//Floor1_partition
	stat = vorbis_read_nbits(5, &dst, stream->io_desc, &p_count);
	if ((stat==VBS_BADSTREAM) || (p_count != 5)) return VBS_BADSTREAM;

	floor1->floor1_partitions=(uint32_t)dst;
	int32_t maximum_class=-1;
	floor1->floor1_partition_class_list=calloc(floor1->floor1_partitions,sizeof(uint32_t));

	for (uint32_t i=0; i<floor1->floor1_partitions; i++){
		stat = vorbis_read_nbits(4, &dst, stream->io_desc, &p_count);
		if ((stat==VBS_BADSTREAM) || (p_count != 4)) return VBS_BADSTREAM;
		floor1->floor1_partition_class_list[i]=(uint32_t)dst;
		maximum_class=MAX(maximum_class,(int32_t)floor1->floor1_partition_class_list[i]);
	}
		
	floor1->floor1_maximum_classplusun=maximum_class+1;

	//loop on maximum class
	floor1->floor1_class_dimensions=calloc(maximum_class+1,sizeof(uint32_t));
	floor1->floor1_class_subclasses=calloc(maximum_class+1,sizeof(uint32_t));
	floor1->floor1_class_masterbooks=calloc(maximum_class+1,sizeof(uint32_t));
	floor1->floor1_subclass_books=calloc(maximum_class+1,sizeof(line_subclass_books_t));

	for (uint32_t i=0; i<=(uint32_t)maximum_class; i++){

		stat = vorbis_read_nbits(3, &dst, stream->io_desc, &p_count);
		if ((stat==VBS_BADSTREAM) || (p_count != 3)) return VBS_BADSTREAM;
		floor1->floor1_class_dimensions[i]=1+(uint32_t)dst;

		stat = vorbis_read_nbits(2, &dst, stream->io_desc, &p_count);
		if ((stat==VBS_BADSTREAM) || (p_count != 2)) return VBS_BADSTREAM;
		floor1->floor1_class_subclasses[i]=(uint32_t)dst;
		
		if (floor1->floor1_class_subclasses[i] != 0){
			stat = vorbis_read_nbits(8, &dst, stream->io_desc, &p_count);
			if ((stat==VBS_BADSTREAM) || (p_count != 8)) return VBS_BADSTREAM;
			floor1->floor1_class_masterbooks[i]=(uint32_t)dst;
		}
		
		uint32_t limite_i=(uint32_t)pow(2.0,(sample_t)(floor1->floor1_class_subclasses[i]));
		floor1->floor1_subclass_books[i].tab=calloc(limite_i,sizeof(int32_t));
		
		for (uint32_t j=0; j<limite_i; j++){

			stat = vorbis_read_nbits(8, &dst, stream->io_desc, &p_count);
			if ((stat==VBS_BADSTREAM) || (p_count != 8)) return VBS_BADSTREAM;
			(floor1->floor1_subclass_books[i]).tab[j]=((uint32_t)dst) -1;
		}
	}

	//Calculating the size_x_list of floor1_x_list
	uint32_t current_class_number;

	uint32_t size_x_list=2;
	for (uint32_t i=0; i<floor1->floor1_partitions; i++){
		current_class_number=floor1->floor1_partition_class_list[i];
		size_x_list += floor1->floor1_class_dimensions[current_class_number];
	}

	floor1->floor1_size_X_list=size_x_list;
	floor1->floor1_X_list=calloc(floor1->floor1_size_X_list,sizeof(uint32_t));
		
	//advancing again in the flux	
	stat = vorbis_read_nbits(2, &dst, stream->io_desc, &p_count);
	if ((stat==VBS_BADSTREAM) || (p_count != 2)) return VBS_BADSTREAM;
	floor1->floor1_multiplier=1+(uint32_t)dst ;

	stat = vorbis_read_nbits(4, &dst, stream->io_desc, &p_count);
	if ((stat==VBS_BADSTREAM) || (p_count != 4)) return VBS_BADSTREAM;
	uint32_t rangebits=(uint32_t)dst;
	
	floor1->floor1_X_list[0]=0;
	floor1->floor1_X_list[1]=(uint32_t)pow(2.0,(sample_t)rangebits);
	floor1->floor1_values= 2;

	for (uint32_t i=0; i<floor1->floor1_partitions;i++){

		current_class_number=floor1->floor1_partition_class_list[i];
		for (uint32_t j=0; j< floor1->floor1_class_dimensions[current_class_number]; j++){

			stat = vorbis_read_nbits(rangebits, &dst, stream->io_desc, &p_count);
			if ((stat==VBS_BADSTREAM) || (p_count != rangebits)) return VBS_BADSTREAM;
			floor1->floor1_X_list[floor1->floor1_values]=(uint32_t)dst;
			floor1->floor1_values ++;			
		}
	}

	data1->base.occ ++;
	data1->size=MAX(data1->size,floor1->floor1_size_X_list);

	//uptating the floor value
	floor_t *pfloor = (floor_t *)floor1;
	*floor=pfloor;
	return VBS_SUCCESS;
}





/**********************************************************************************/



uint32_t low_neighbor(uint32_t *v, uint32_t x);
uint32_t high_neighbor(uint32_t *v, uint32_t x);
uint32_t render_point(uint32_t x0,uint32_t y0,uint32_t x1,uint32_t y1,uint32_t xi);
void render_line(uint32_t x0, uint32_t y0, uint32_t x1, uint32_t y1,uint32_t *tab_indice);
static uint32_t compare_qsort (qsort_struct_t const *a,qsort_struct_t const *b);


/*********************************************************************************/

status_t floor_type1_decode(vorbis_stream_t *stream, floor_t *floor_cfg,
				   sample_t *v, uint16_t v_size){

	floor_type1_t *floor1 = (floor_type1_t *) floor_cfg;
	floor_type1_data_t *data1=(floor_type1_data_t *)(stream->codec->floors_desc->data1);

	uint32_t p_count, dst;
	status_t stat;
	
	/**********************Premiere partie **************************/
	

	stat = vorbis_read_nbits(1, &dst, stream->io_desc, &p_count);
	if ((stat==VBS_BADSTREAM) || (p_count != 1)) return VBS_BADSTREAM;
	if ((uint32_t)dst==0) return VBS_UNUSED;

	uint32_t vector[4]={256,128,86,64};
	uint32_t range=vector[floor1->floor1_multiplier-1];
	uint32_t nb_bits=ilog(range-1);

	stat = vorbis_read_nbits(nb_bits, &dst, stream->io_desc, &p_count);
	if ((stat==VBS_BADSTREAM) || (p_count != nb_bits)) return VBS_BADSTREAM;
	data1->floor1_Y[0]=(uint32_t)dst;

	stat = vorbis_read_nbits(nb_bits, &dst, stream->io_desc, &p_count);
	if ((stat==VBS_BADSTREAM) || (p_count != nb_bits)) return VBS_BADSTREAM;
	data1->floor1_Y[1]=(uint32_t)dst;

	uint32_t offset=2;
	
 	for (uint32_t i=0; i<floor1->floor1_partitions; i++){
		uint32_t class=floor1->floor1_partition_class_list[i];
		uint32_t cdim=floor1->floor1_class_dimensions[class];
		uint32_t cbits=floor1->floor1_class_subclasses[class];
		uint32_t csub=-1+(uint32_t)pow(2.0,(sample_t)cbits);
		uint32_t cval=0;

		if (cbits > 0){
			codebook_translate_scalar(stream,(stream->codec->codebooks_desc->codebooks)[floor1->floor1_class_masterbooks[class]],&cval);
		}

		for(uint32_t j=0; j<cdim; j++){
			int32_t book=(floor1->floor1_subclass_books[class]).tab[cval & csub];
			cval=cval >> cbits;
			if (book >= 0){
				codebook_translate_scalar(stream,(stream->codec->codebooks_desc->codebooks)[book],&(dst));
				data1->floor1_Y[j+offset]=(uint32_t)dst;
			}
			else{
				data1->floor1_Y[j+offset]=0;
			}			
		}

		offset=offset+cdim;
	}

	/******************Synthese de la courbe*********************/


	range=vector[floor1->floor1_multiplier-1];
	data1->floor1_step2_flag[0]=1;
	data1->floor1_step2_flag[1]=1;
	data1->floor1_final_Y[0]=data1->floor1_Y[0];
	data1->floor1_final_Y[1]=data1->floor1_Y[1];


	for (uint32_t i=2; i<floor1->floor1_values; i++){
	
		uint32_t low_neighbor_offset=low_neighbor(floor1->floor1_X_list,i);
		uint32_t high_neighbor_offset=high_neighbor(floor1->floor1_X_list,i);

		uint32_t predicted=render_point(floor1->floor1_X_list[low_neighbor_offset],data1->floor1_final_Y[low_neighbor_offset],floor1->floor1_X_list[high_neighbor_offset],data1->floor1_final_Y[high_neighbor_offset],floor1->floor1_X_list[i]);

	
		uint32_t val=data1->floor1_Y[i];
		uint32_t highroom=range - predicted;
		uint32_t lowroom=predicted;
		uint32_t room;

		if (highroom < lowroom ){
			room=highroom*2;	
		}
		else{
			room=lowroom*2;
		}

		if (val != 0){
			data1->floor1_step2_flag[low_neighbor_offset]=1;
			data1->floor1_step2_flag[high_neighbor_offset]=1;
			data1->floor1_step2_flag[i]=1;

			if (val >= room){
				if (highroom > lowroom){
					data1->floor1_final_Y[i]=val-lowroom+predicted;
				}
				else{
					data1->floor1_final_Y[i]=predicted-val+highroom-1;
				}
			}
			else if (val % 2 ==1){
				data1->floor1_final_Y[i]=predicted -((val+1)/2);
			}
			else{
				data1->floor1_final_Y[i]=predicted +(val/2);
				
			}
		}
		else{
			data1->floor1_step2_flag[i]=0;
			data1->floor1_final_Y[i]=predicted;
		}
	}


	/****************************Qsort ***************************/


	for (uint32_t i=0; i<floor1->floor1_size_X_list; i++){
		data1->tri[i].elm=floor1->floor1_X_list[i];
		data1->tri[i].ind=i;
	}

	qsort(data1->tri, floor1->floor1_size_X_list, sizeof(qsort_struct_t),(__compar_fn_t)compare_qsort);
	uint32_t ind;

	for (uint32_t i=0; i<floor1->floor1_size_X_list; i++){

		ind=data1->tri[i].ind;		
		data1->floor1_final_Y_o[i]=data1->floor1_final_Y[ind];
		data1->floor1_X_list_o[i]=floor1->floor1_X_list[ind];
		data1->floor1_step2_flag_o[i]=data1->floor1_step2_flag[ind];
	}

	/**************Tri simpliste des vecteurs de manière simultannée************/

/*	uint32_t jmin;
	uint32_t min;
        uint32_t aux;
	uint32_t auxb; 

	
	for (uint32_t i=0; i< floor1->floor1_size_X_list; i++){
		data1->floor1_final_Y_o[i]=data1->floor1_final_Y[i];
		data1->floor1_X_list_o[i]=floor1->floor1_X_list[i];
		data1->floor1_step2_flag_o[i]=data1->floor1_step2_flag[i];
	}

	for (uint32_t i=0; i<floor1->floor1_size_X_list; i++){
	
		jmin=i;
		min=data1->floor1_X_list_o[i];
		for (uint32_t j=i+1; j<floor1->floor1_size_X_list; j++){

			if (min > data1->floor1_X_list_o[j] ){
				min=data1->floor1_X_list_o[j];
				jmin=j;
			}
		}
		aux=data1->floor1_X_list_o[i];
		data1->floor1_X_list_o[i]=data1->floor1_X_list_o[jmin];
		data1->floor1_X_list_o[jmin]=aux;

		aux=data1->floor1_step2_flag_o[i];
		data1->floor1_step2_flag_o[i]=data1->floor1_step2_flag_o[jmin];
		data1->floor1_step2_flag_o[jmin]=aux;

		auxb=data1->floor1_final_Y_o[i];
		data1->floor1_final_Y_o[i]=data1->floor1_final_Y_o[jmin];
		data1->floor1_final_Y_o[jmin]=auxb;
	}
*/


	/*****************Tronquage et calcul du résultat final *******************/

	uint32_t hx=0;
	uint32_t hy;
	uint32_t lx=0;
	uint32_t ly=(data1->floor1_final_Y_o[0])*(floor1->floor1_multiplier);
			
	uint32_t * tab_indice=calloc(v_size *2, sizeof(uint32_t));
	//La longueur de Tab_indice est initialisée
	//à une grande valeur pour qu'il puisse tout contenir

	for (uint32_t i=1; i<floor1->floor1_values;i++){
		
		if (data1->floor1_step2_flag_o[i] == 1){
			hy=data1->floor1_final_Y_o[i] * floor1->floor1_multiplier;
			hx=data1->floor1_X_list_o[i];
			render_line(lx,ly,hx,hy,tab_indice); 
			lx=hx;
			ly=hy;
		}
	}

	if (hx < v_size){
		render_line(hx,hy,v_size,hy,tab_indice);	
	}
	
	//Tronquer à n2 éléments revient à ne rien faire car
	// ensuite on lit tab_indice de 0 à n2

	for (uint16_t i=0; i< v_size; i++){
		v[i]=inverse_dB_table[tab_indice[i]];
	}

	free(tab_indice);
	return VBS_SUCCESS;
}



/*****************************************************************************************/



status_t floor_type1_data_new(floor_data_t **pfl_data, uint16_t *blocksize)
{
	blocksize[0]=blocksize[0];
    	floor_type1_data_t *data1= malloc(sizeof(floor_type1_data_t ));
	data1->base.type=1;
	data1->base.occ=0;
	data1->size=0;
	*pfl_data=(floor_data_t *)data1;

	return VBS_SUCCESS;
}

status_t floor_type1_data_allocate(floor_data_t *fl_data)
{
	
	floor_type1_data_t *data1=(floor_type1_data_t *) fl_data;

	data1->floor1_Y=calloc(data1->size,sizeof(uint32_t));
	data1->floor1_final_Y=calloc(data1->size,sizeof(uint32_t));
	data1->floor1_final_Y_o=calloc(data1->size,sizeof(uint32_t));
	data1->floor1_step2_flag=calloc(data1->size,sizeof(uint32_t));
	data1->floor1_step2_flag_o=calloc(data1->size,sizeof(uint32_t));
	data1->floor1_X_list_o=calloc(data1->size,sizeof(uint32_t));
	data1->tri=calloc(data1->size,sizeof(qsort_struct_t));
	return VBS_SUCCESS;
}

void floor_type1_data_free(floor_data_t *fl_data)
{
	floor_type1_data_t *data1=(floor_type1_data_t *)fl_data;

	free(data1->floor1_Y);
	free(data1->floor1_final_Y);
	free(data1->floor1_final_Y_o);
	free(data1->floor1_step2_flag_o);
	free(data1->floor1_step2_flag);
	free(data1->floor1_X_list_o);
	free(data1->tri);

	free(data1);
	return;
}


uint32_t low_neighbor(uint32_t *v, uint32_t x){
	
	int32_t n=-1;
	for (uint32_t i=0; i<x; i++){
		if (n==-1){
			if (v[i]<=v[x]){
				n=i;
			}
		}
		else{
			if (v[i]<=v[x] && v[n]<v[i]){
				n=i;
			}	
		}
	}
	return n;	
}

uint32_t high_neighbor(uint32_t *v, uint32_t x){
	
	int32_t n=-1;
	for (uint32_t i=0; i<x; i++){
		if (n==-1){
			if (v[i]>=v[x]){
				n=i;
			}
		}
		else{
			if (v[i]>=v[x] && v[n]>v[i]){
				n=i;
			}	
		}
	}
	return n;
}

uint32_t render_point(uint32_t x0,uint32_t y0,uint32_t x1,uint32_t y1,uint32_t xi){

	int32_t dy=y1-y0;
	uint32_t adx=x1-x0;
	uint32_t ady=abs(dy);
	uint32_t err=ady*(xi-x0);
	uint32_t off=err/adx;
	if (dy<0){
		return(y0-off);
	}
	else{
		return(y0+off);
	}
}

void render_line(uint32_t x0, uint32_t y0, uint32_t x1, uint32_t y1,uint32_t *tab_indice){
	int32_t dy=y1-y0;
	int32_t adx=x1-x0;
	uint32_t ady=abs(dy);
	int32_t base=dy/adx;
	uint32_t x=x0;
	uint32_t y=y0;
	int32_t err=0;
	int32_t sy;

	if (dy <0){
		sy=base-1;
	}
	else{
		sy=base+1;
	}
	ady -= adx*abs(base);
	printf(" premier x %d y %d \n", x ,y);
	tab_indice[x]=y;
	
	for (uint32_t i=x0+1; i<=x1-1;i++){
		err += ady;
		if (err >=  adx){
			err -= adx;
			y +=sy;
		}
		else{
			y += base;
		}
		printf(" x %i y %d \n",i,y);

		tab_indice[i]=y;
	}	

}


static uint32_t compare_qsort (qsort_struct_t const *a,qsort_struct_t const *b){

   qsort_struct_t const *pa = a;
   qsort_struct_t const *pb = b;

   /* on retourne l'etat de l'evaluation (tri croissant) */
   return ((pa->elm) - (pb->elm));
}


