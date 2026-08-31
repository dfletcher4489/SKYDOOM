// Emacs style mode select   -*- C++ -*- 
//-----------------------------------------------------------------------------
//
// $Id:$
//
// Copyright (C) 1993-1996 by id Software, Inc.
//
// This source is available for distribution and/or modification
// only under the terms of the DOOM Source Code License as
// published by id Software. All rights reserved.
//
// The source is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// FITNESS FOR A PARTICULAR PURPOSE. See the DOOM Source Code License
// for more details.
//
// $Log:$
//
// DESCRIPTION:
//	Main program, simply calls D_DoomMain high level loop.
//
//-----------------------------------------------------------------------------

static const char
rcsid[] = "$Id: i_main.c,v 1.4 1997/02/03 22:45:10 b1 Exp $";

#include "audsrvx.h"
#include <loadfile.h>
#include <iopheap.h>
#include <libmc.h>

#include "doomdef.h"
#include "i_video.h"

#include "m_argv.h"
#include "d_main.h"
#include "gamemanager/ps_manager.h"
#include "gs/ps_gs.h"
#include "log/ps_log.h"
#include "io/ps_file_io.h"
#include "io/ps_memcard.h"
#include "pad/ps_pad.h"

#include "tsf.h"
u32 SKYDOOM_HEIGHT = 448;
u32 SKYDOOM_WIDTH = 640;

tsf* gTsfInstance = NULL;

char *audioBuffer1 = NULL;
char *audioBuffer2 = NULL;
s32 sifTransferID = -1;

int memCardType, memCardFree, memCardFormat;
extern char doomdir[35];
extern sceMcTblGetDir saveentries[6];
boolean useMemCard = false;

Controller mainController;

#define BUFFERSIZE (44100 * 5) + 400
int
main
( int		argc,
  char**	argv ) 
{ 

    ManagerInfo info;

    memset(&info, 0, sizeof(ManagerInfo));

    info.doublebuffered = true;
    info.zenable = false;
    info.width = SKYDOOM_WIDTH;
    info.height = SKYDOOM_HEIGHT;
    info.psm = GS_PSM_24;
    info.drawBufferSize = 1 << 8;

    InitializeSystem(&info);
    I_InitGraphics();
    int ret;
    LoadMCMANModules(); 

    if (mcInit(1) < 0)
    {
        ERRORLOG("Memcard init didn't work");
        return -1;
    }

    int memCardRet = mcGetInfo(0 , 0,  &memCardType, &memCardFree, &memCardFormat);
    mcSync(0, NULL, &ret);

    memCardRet = mcGetInfo(0 , 0,  &memCardType, &memCardFree, &memCardFormat);
    mcSync(0, NULL, &ret);

    if (!ret)
    {
        useMemCard = true;
    }

    DEBUGLOG("ret from memcard is %d %d", ret, memCardRet);
    DEBUGLOG("%d type %d free %d format", memCardType, memCardFree, memCardFormat);

    if (useMemCard)
    {
        mcGetDir(0, 0, doomdir, 0, 6, saveentries);

        mcSync(0, NULL, &ret);

        if (ret == -4)
        {
            mcMkDir(0, 0, doomdir);
            mcSync(0, NULL, &ret);
        }
    }

    ret = audsrv_init();

    if (ret < 0)
    {
        ERRORLOG("audsrv init didn't work");
        return -1;
    }

    audsrv_adpcm_init();

    struct audsrv_fmt_t audio;
    audio.freq = 22050;
    audio.bits = 16;
    audio.channels = 1;
    
    //audsrv_set_format(&audio);

    //audsrv_set_volume(50);

    audioBuffer1 = SifAllocIopHeap(BUFFERSIZE);
    audioBuffer2 = SifAllocIopHeap(BUFFERSIZE);

    if (!audioBuffer1 || !audioBuffer2)
    {
        ERRORLOG("Cannot allocate audio buffers");
    }

   // audsrv_set_buffers(audioBuffer1, audioBuffer2, BUFFERSIZE, BUFFERSIZE);

    u32 sf2Size = 0;

    char buffer[25];

    Pathify("gzdoom.sf2", buffer);

    unsigned char *gzsf2 = ReadFileInFull(buffer, &sf2Size);
    
    DEBUGLOG("size of gzdoom %d", sf2Size);

    gTsfInstance = tsf_load_memory(gzsf2, sf2Size);

    tsf_set_output(gTsfInstance, TSF_MONO, 22050, 0.0f);

    InitializeController(&mainController, 0, 0);

    myargc = argc; 
    myargv = argv; 
 
    D_DoomMain (); 

    return 0;
} 
