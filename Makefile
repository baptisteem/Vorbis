CC        = gcc
LD        = gcc

SRCDIR    = src
INCDIR    = include
OBJDIR    = ref_objs
OBJ				= objs

PROG      = vorbis_decoder

CPPFLAGS  = 
CFLAGS    = -std=c99 -g -I $(INCDIR)
LDFLAGS   = -lm


OBJECTS   = $(OBJDIR)/main.o                                                  \
            $(OBJDIR)/error.o                                                 \
            $(OBJDIR)/pcm_handler.o                                           \
            $(OBJDIR)/ogg_core.o $(OBJDIR)/ogg_packet.o                       \
            $(OBJDIR)/floor.o $(OBJDIR)/floor0.o $(OBJDIR)/floor1.o           \
            $(OBJDIR)/vorbis_main.o                                           \
            $(OBJDIR)/header1.o                                           \
            $(OBJDIR)/header2.o                                           \
            $(OBJDIR)/common_header.o                                           \
            $(OBJDIR)/vorbis_packet.o                                         \
            $(OBJDIR)/vorbis_io.o                                             \
            $(OBJDIR)/header3.o                                               \
            $(OBJDIR)/residue.o                                               \
            $(OBJDIR)/helpers.o                                               \
            $(OBJDIR)/mapping.o                                               \
            $(OBJDIR)/codebook.o                                              \
            $(OBJDIR)/codebook_read.o                                         \
            $(OBJDIR)/time_domain_transform.o                                 \
            $(OBJDIR)/envelope.o                                              \
	    $(OBJDIR)/dot_product.o                                           \

MY_OBJECTS = 


quiet-command = $(if $(VERB),$1,$(if $(2),@echo $2 && $1, @$1))

all     : $(OBJDIR) $(PROG)

## Start - dot_product ##
dot_product : $(PROG)_dot_product $(OBJDIR)

$(PROG)_dot_product : $(filter-out $(OBJDIR)/dot_product.o,$(OBJECTS)) $(OBJ)/dot_product.o
	$(call quiet-command, $(LD) $^ $(LDFLAGS) -o $@, "  LD       $@" $(LDFLAGS))
## End - dot_product ##

$(OBJ)/%.o: $(SRCDIR)/%.c $(INCDIR)
	$(LD) $(CFLAGS) $(LDFLAGS) -c $< -o $@ 

$(PROG) : $(OBJECTS) $(MY_OBJECTS)
	$(call quiet-command, $(LD) $^ $(LDFLAGS) -o $@, "  LD       $@" $(LDFLAGS))

$(OBJDIR):
	$(call quiet-command, mkdir -p $(OBJDIR),)


clean    :
	$(call quiet-command, rm -f $(MY_OBJECTS) $(PROG) *~, "  CLEAN    ")1
