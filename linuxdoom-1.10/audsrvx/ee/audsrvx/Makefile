# _____     ___ ____     ___ ____
#  ____|   |    ____|   |        | |____|
# |     ___|   |____ ___|    ____| |    \    PS2DEV Open Source Project.
#-----------------------------------------------------------------------
# Copyright 2005, ps2dev - http://www.ps2dev.org
# Licenced under GNU Library General Public License version 2
# Review ps2sdk README & LICENSE files for further details.



EE_LIB = libaudsrvx.a
EE_ERL = libaudsrvx.erl
EE_OBJS_DIR = ./obj
EE_BIN_DIR = ./lib
EE_OBJS = $(EE_OBJS_DIR)/audsrvx_rpc.o $(EE_OBJS_DIR)/erl-support.o
EE_INC_DIR = ./include/
SRC_DIR = ./src

include $(PS2SDK)/samples/Makefile.pref
include ./Makefile.eeglobal

all:: $(EE_BIN_DIR) $(EE_OBJS_DIR) $(EE_LIB) $(EE_ERL)

$(EE_BIN_DIR):
	mkdir -p $@

$(EE_OBJS_DIR):
	mkdir -p $@

clean::
	rm -f -r $(EE_OBJS_DIR) $(EE_BIN_DIR)

clean::
	rm -f -r $(EE_OBJS_DIR) $(EE_BIN_DIR)