#include <stdint.h>
#include <vorbis.h>
#include <sample.h>
#include <error.h>
#incluse "residue.h"



status_t residues_setup_init(vorbis_stream_t * stream, residues_setup_t ** pres){

}



//void residues_free(residues_setup_t * set){

}


status_t residue_decode(vorbis_stream_t * stream, residue_t * residue, int ch,
			int64_t N2, sample_t ** v, uint8_t * do_not_decode){
  uint32_t p_count;
  status_t bool;
  uint32_t residue_begin, residue_end, residue_partition_size, residu_classification, residu_classbook;
  
  bool = vorbis_read_nbits(24, &residue_begin, stream->io_desc, &p_count);
  if((p_coun!=24)||(bool!=VBS_SUCCES)) return VBS_BADSTREAM;

  bool = vorbis_read_nbits(24, &residue_end, stream->io_desc, &p_count);
  if((p_coun!=24)||(bool!=VBS_SUCCES)) return VBS_BADSTREAM;

  bool = vorbis_read_nbits(24, &residue_partition_size, stream->io_desc, &p_count);
  if((p_coun!=24)||(bool!=VBS_SUCCES)) return VBS_BADSTREAM;
  residu_partition_size ++;

  bool = vorbis_read_nbits(6, &residue_classification, stream->io_desc, &p_count);
  if((p_coun!=6)||(bool!=VBS_SUCCES)) return VBS_BADSTREAM;
  residue_classification ++;

  bool = vorbis_read_nbits(8, &residue_begin, stream->io_desc, &p_count);
  if((p_coun!=8)||(bool!=VBS_SUCCES)) return VBS_BADSTREAM;

  
  
}
