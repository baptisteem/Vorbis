CC        = gcc
LD        = gcc

SRCDIR    = src
INCDIR    = include
OBJDIR    = ref_objs
OBJ		  = objs

PROG      = vorbis_decoder

CPPFLAGS  = 
CFLAGS    = -std=c99 -g -Wall -Wextra -I $(INCDIR)
LDFLAGS   = -lm

OBJECTS   = $(OBJDIR)/main.o                                                  \
						$(OBJDIR)/error.o                                                 \
						$(OBJDIR)/pcm_handler.o                                           \
						$(OBJDIR)/ogg_core.o $(OBJDIR)/ogg_packet.o                       \
						$(OBJDIR)/floor.o $(OBJDIR)/floor0.o            \
						$(OBJDIR)/vorbis_main.o                                           \
						$(OBJDIR)/header1.o                                           \
						$(OBJDIR)/header2.o                                           \
						$(OBJDIR)/common_header.o                                           \
						$(OBJDIR)/vorbis_packet.o                                         \
						$(OBJDIR)/vorbis_io.o                                             \
						$(OBJDIR)/header3.o                                               \
						$(OBJDIR)/header1.o                                               \
						$(OBJDIR)/header2.o                                               \
						$(OBJDIR)/floor1.o                                               \
						$(OBJDIR)/common_header.o                                               \
						$(OBJDIR)/residue.o                                               \
						$(OBJDIR)/helpers.o                                               \
						$(OBJDIR)/mapping.o                                               \
						$(OBJDIR)/codebook.o                                              \
						$(OBJDIR)/codebook_read.o                                         \
						$(OBJDIR)/time_domain_transform.o                                 \
						$(OBJDIR)/envelope.o                                              \
						$(OBJDIR)/dot_product.o                                           \
						$(OBJDIR)/mode.o       

MY_OBJECTS = $(OBJ)/main.o	\
						$(OBJDIR)/error.o                                                 \
						$(OBJ)/pcm_handler.o                                           \
						$(OBJDIR)/ogg_core.o $(OBJDIR)/ogg_packet.o                       \
						$(OBJ)/floor.o $(OBJDIR)/floor0.o $(OBJ)/floor1.o           \
						$(OBJ)/vorbis_main.o                                           \
						$(OBJ)/vorbis_headers.o                                           \
						$(OBJ)/vorbis_packet.o                                         \
						$(OBJ)/vorbis_io.o                                             \
						$(OBJ)/residue.o                                               \
						$(OBJ)/helpers.o                                               \
						$(OBJ)/mapping.o                                               \
						$(OBJDIR)/codebook.o                                              \
						$(OBJDIR)/codebook_read.o                                         \
						$(OBJDIR)/time_domain_transform.o                                 \
						$(OBJ)/envelope.o                                              \
						$(OBJ)/dot_product.o                                           \
						$(OBJ)/mode.o       

quiet-command = $(if $(VERB),$1,$(if $(2),@echo $2 && $1, @$1))

all     : $(OBJDIR) $(OBJ) $(PROG)

## Reference - Seulement les libs de départ ##
ref : $(OBJDIR) $(OBJ) $(PROG)_ref

$(PROG)_ref : $(OBJECTS)
	$(call quiet-command, $(LD) $^ $(LDFLAGS) -o $@, "  LD       $@" $(LDFLAGS))
## End - reference ##


## Start - dot_product ##
dot_product : $(OBJDIR) $(OBJ) $(PROG)_dot_product 

$(PROG)_dot_product : $(filter-out $(OBJDIR)/dot_product.o,$(OBJECTS)) $(OBJ)/dot_product.o
	$(call quiet-command, $(LD) $^ $(LDFLAGS) -o $@, "  LD       $@" $(LDFLAGS))
## End - dot_product ##


## Start - residue ##
residue : $(OBJDIR) $(OBJ) $(PROG)_residue 

$(PROG)_residue : $(filter-out $(OBJDIR)/residue.o,$(OBJECTS)) $(OBJ)/residue.o
	$(call quiet-command, $(LD) $^ $(LDFLAGS) -o $@, "  LD       $@" $(LDFLAGS))
## End - residue ##


## Start - ogg_core ##
ogg_core : $(OBJDIR) $(OBJ) $(PROG)_ogg_core

$(PROG)_ogg_core : $(filter-out $(OBJDIR)/ogg_core.o,$(OBJECTS)) $(OBJ)/ogg_core.o 
	$(call quiet-command, $(LD) $^ $(LDFLAGS) -o $@, "  LD       $@" $(LDFLAGS))
## End - ogg_core ##


## Start - vorbis_io ##
vorbis_io : $(OBJDIR) $(OBJ) $(PROG)_vorbis_io 

$(PROG)_vorbis_io : $(filter-out $(OBJDIR)/vorbis_io.o,$(OBJECTS)) $(OBJ)/vorbis_io.o	
	$(call quiet-command, $(LD) $^ $(LDFLAGS) -o $@, "  LD       $@" $(LDFLAGS))
## End - vorbis_io ##

## Start - pcm_handler ##
pcm_handler : $(OBJDIR) $(OBJ) $(PROG)_pcm_handler 

$(PROG)_pcm_handler : $(filter-out $(OBJDIR)/pcm_handler.o,$(OBJECTS)) $(OBJ)/pcm_handler.o
	$(call quiet-command, $(LD) $^ $(LDFLAGS) -o $@, "  LD       $@" $(LDFLAGS))
## End - pcm_handler ##


## Start - envelope ##
envelope : $(OBJDIR) $(OBJ) $(PROG)_envelope 

$(PROG)_envelope : $(filter-out $(OBJDIR)/envelope.o,$(OBJECTS)) $(OBJ)/envelope.o
	$(call quiet-command, $(LD) $^ $(LDFLAGS) -o $@, "  LD       $@" $(LDFLAGS))
## End - envelope ##


## Start - vorbis_packet ##
vorbis_packet : $(PROG)_vorbis_packet $(OBJDIR)

$(PROG)_vorbis_packet : $(filter-out $(OBJDIR)/vorbis_packet.o,$(OBJECTS)) $(OBJ)/vorbis_packet.o
	$(call quiet-command, $(LD) $^ $(LDFLAGS) -o $@, "  LD       $@" $(LDFLAGS))
## End - vorbis_packet ##


## Start - floor ##
floor : $(OBJDIR) $(OBJ) $(PROG)_floor

$(PROG)_floor : $(filter-out $(OBJDIR)/floor.o,$(OBJECTS)) $(OBJ)/floor.o
	$(call quiet-command, $(LD) $^ $(LDFLAGS) -o $@, "  LD       $@" $(LDFLAGS))
## End - floor ##

## Start - floor1 ##
floor1 : $(OBJDIR) $(OBJ) $(PROG)_floor1 

$(PROG)_floor1 : $(filter-out $(OBJDIR)/floor1.o,$(OBJECTS)) $(OBJ)/floor1.o
	$(call quiet-command, $(LD) $^ $(LDFLAGS) -o $@, "  LD       $@" $(LDFLAGS))
## End - floor1 ##


## Start - headers ##
headers : $(OBJDIR) $(OBJ) $(PROG)_headers 

$(OBJ)/vorbis_headers.o: $(SRCDIR)/vorbis_headers.c $(INCDIR)
	$(LD) $(CFLAGS) $(LDFLAGS) -c $< -o $@ 

$(PROG)_headers : $(filter-out $(OBJDIR)/header1.o $(OBJDIR)/header2.o $(OBJDIR)/header3.o $(OBJDIR)/common_header.o,$(OBJECTS)) $(OBJ)/vorbis_headers.o
	$(call quiet-command, $(LD) $^ $(LDFLAGS) -o $@, "  LD       $@" $(LDFLAGS))
## End - headers##


## Start - helpers ##
helpers : $(OBJDIR) $(OBJ) $(PROG)_helpers 

$(PROG)_helpers : $(filter-out $(OBJDIR)/helpers.o,$(OBJECTS)) $(OBJ)/helpers.o
	$(call quiet-command, $(LD) $^ $(LDFLAGS) -o $@, "  LD       $@" $(LDFLAGS))
## End - helpers ##

## Start - mode ##
mode : $(OBJDIR) $(OBJ) $(PROG)_mode

$(PROG)_mode : $(filter-out $(OBJDIR)/mode.o,$(OBJECTS)) $(OBJ)/mode.o
	$(call quiet-command, $(LD) $^ $(LDFLAGS) -o $@, "  LD       $@" $(LDFLAGS))
## End - mode ##


## Start - time_domain_transform ##
time_domain : $(OBJDIR) $(OBJ) $(PROG)_time_domain 

#Rule for time_domain, because not the same name
$(OBJ)/time_domain_transform.o: $(SRCDIR)/time_domain.c $(INCDIR)
	$(LD) $(CFLAGS) $(LDFLAGS) -c $< -o $@ 

$(PROG)_time_domain : $(filter-out $(OBJDIR)/time_domain_transform.o,$(OBJECTS)) $(OBJ)/time_domain_transform.o
	$(call quiet-command, $(LD) $^ $(LDFLAGS) -o $@, "  LD       $@" $(LDFLAGS))
## End - time_domain_transform ##


## Start - mapping ##
mapping : $(OBJDIR) $(OBJ) $(PROG)_mapping

$(PROG)_mapping : $(filter-out $(OBJDIR)/mapping.o ,$(OBJECTS)) $(OBJ)/mapping.o 
	$(call quiet-command, $(LD) $^ $(LDFLAGS) -o $@, "  LD       $@" $(LDFLAGS))
## End - mapping ##


## Start - main ##
main : $(OBJDIR) $(OBJ) $(PROG)_main 

$(PROG)_main : $(filter-out $(OBJDIR)/main.o,$(OBJECTS)) $(OBJ)/main.o
	$(call quiet-command, $(LD) $^ $(LDFLAGS) -o $@, "  LD       $@" $(LDFLAGS))
## End - mode ##


## Start - vorbis_main ##
vorbis_main : $(OBJDIR) $(OBJ) $(PROG)_vorbis_main 

$(PROG)_vorbis_main : $(filter-out $(OBJDIR)/vorbis_main.o,$(OBJECTS)) $(OBJ)/vorbis_main.o
	$(call quiet-command, $(LD) $^ $(LDFLAGS) -o $@, "  LD       $@" $(LDFLAGS))
## End - vorbis_main ##


$(OBJ)/%.o: $(SRCDIR)/%.c $(INCDIR)
	$(LD) $(CFLAGS) $(LDFLAGS) -c $< -o $@ 

$(PROG) : $(MY_OBJECTS)
	$(call quiet-command, $(LD) $^ $(LDFLAGS) -o $@, "  LD       $@" $(LDFLAGS))

$(OBJDIR):
	$(call quiet-command, mkdir -p $(OBJDIR),)

$(OBJ):
	$(call quiet-command, mkdir -p $(OBJ),)

clean    :
	$(call quiet-command, rm -f $(OBJ)/* $(PROG)* *~, "  CLEAN    ")1
