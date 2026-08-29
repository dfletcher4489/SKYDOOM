IRX_DIR := ./irx
OBJS_DIR := ./objs
SRC_DIR := ./ps2
TARGET_DIR := ./bin
WIN_DIR ?= $(PS2_ISO_WIN_DIR)
PS2_LIB_DIR := ./inc
AUDSRVX := ./audsrvx
AUDSRVX_EE := $(AUDSRVX)/ee/audsrvx/
AUDSRVX_IOP := $(AUDSRVX)/iop/audsrvx/
AUDSRVX_LIB := $(AUDSRVX_EE)/lib/
AUDSRVX_IRX := $(AUDSRVX_IOP)/irx/

OBJEXT    := o
CEXT      := c

EE_BIN = doom_00012.elf
IOP_MOD = sio2man.irx padman.irx libsd.irx mcman.irx mcserv.irx

EE_SRC_C += $(shell find $(SRC_DIR) -type f -name *.$(CEXT))
EE_OBJS += $(patsubst $(SRC_DIR)/%, $(OBJS_DIR)/%, $(EE_SRC_C:.$(CEXT)=.$(OBJEXT)))

EE_DVP = dvp-as

EE_LIBS= -lps2 -ldma -lgraph -ldraw -lkernel -lpacket -lpad -lcdvd -lpng -lz -lunzip -laudsrvx -lmc

LOG_LEVEL?=3

EE_CFLAGS += -DTML_STATIC -DTSF_STATIC -DTSF_IMPLEMENTATION -DTML_IMPLEMENTATION -DPS_LOG_LVL=$(LOG_LEVEL) -Wall -Wno-char-subscripts --std=gnu99 -I$(PS2_LIB_DIR) -I$(AUDSRVX_EE)/include/
EE_LDFLAGS = -L./lib -L$(AUDSRVX_LIB) -L$(PS2SDK)/ee/common/lib -L$(PS2SDK)/ee/lib -L$(PS2SDK)/ports/lib 

ISO_TGT=doom.iso

include ./Makefile.eeglobal
include $(PS2SDK)/samples/Makefile.pref

.PHONY : all clean compile directories disc irx update_win

all: compile disc update_win

compile:
	make all -C $(AUDSRVX_IOP)
	make all -C $(AUDSRVX_EE)
	make

directories:
	if [ ! -d $(IRX_DIR) ]; then mkdir -p $(IRX_DIR) ; fi

	if [ ! -d $(OBJS_DIR) ]; then mkdir -p $(OBJS_DIR) ; fi

	if [ ! -d $(TARGET_DIR) ]; then mkdir -p $(TARGET_DIR) ; fi

update_win:
	if [ ! -z "$(WIN_DIR)" ]; then cp $(ISO_TGT) $(WIN_DIR) ; fi

disc:
	mkisofs -l -o $(ISO_TGT) $(TARGET_DIR)/$(EE_BIN) SYSTEM.CNF $(IRX_DIR) $(AUDSRVX_IRX)audsrvx.irx *.wad gzdoom.sf2

irx:
	mv $(IOP_MOD) "$(IRX_DIR)"

clean:
	make clean -C $(AUDSRVX_EE)
	make clean -C $(AUDSRVX_IOP)
	rm -rf $(ISO_TGT) $(TARGET_DIR)/$(EE_BIN) $(EE_OBJS)

	
		
