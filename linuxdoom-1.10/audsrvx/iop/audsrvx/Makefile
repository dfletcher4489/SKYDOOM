# _____     ___ ____     ___ ____
#  ____|   |    ____|   |        | |____|
# |     ___|   |____ ___|    ____| |    \    PS2DEV Open Source Project.
#-----------------------------------------------------------------------
# Copyright 2005, ps2dev - http://www.ps2dev.org
# Licenced under GNU Library General Public License version 2
# Review ps2sdk README & LICENSE files for further details.


IOP_OBJS_DIR = ./obj

IOP_OBJS = $(IOP_OBJS_DIR)/audsrvx.o $(IOP_OBJS_DIR)/upsamplers.o $(IOP_OBJS_DIR)/hw.o $(IOP_OBJS_DIR)/rpc_server.o $(IOP_OBJS_DIR)/rpc_client.o $(IOP_OBJS_DIR)/common.o $(IOP_OBJS_DIR)/cdrom.o $(IOP_OBJS_DIR)/imports.o $(IOP_OBJS_DIR)/exports.o $(IOP_OBJS_DIR)/adpcm.o

IOP_BIN_DIR = ./irx
IOP_BIN = audsrvx.irx
IOP_SRC_DIR = ./src
IOP_INC_DIR = ./include

include $(PS2SDK)/samples/Makefile.pref
include ./Makefile.iopglobal


all:: $(IOP_BIN_DIR) $(IOP_OBJS_DIR) $(IOP_BIN)

$(IOP_BIN_DIR):
	mkdir -p $@

$(IOP_OBJS_DIR):
	mkdir -p $@

clean::
	rm -f -r $(IOP_OBJS_DIR) $(IOP_BIN_DIR)