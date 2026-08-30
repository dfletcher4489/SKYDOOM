#include "m_launcher.h"
#include "doomtype.h"
#include "doomdef.h"
#include "i_video.h"
#include "ps_global.h"
#include "gs/ps_gs.h"
#include "textures/ps_texture.h"
#include "textures/ps_font.h"
#include "gamemanager/ps_manager.h"
#include "log/ps_log.h"
#include "pad/ps_pad.h"

#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include <libmc.h>

#include "m_launcherBKGD.h"

extern u32 SKYDOOM_HEIGHT;
extern u32 SKYDOOM_WIDTH;

boolean runninglauncher = false;

Texture *background = NULL;
Font *fontimage = NULL;

u32 wadlistsize = 0;
u32 wadselect = 0;

char **wadlist = NULL;
char **dirlist = NULL;

GameMode_t *gamemodes = NULL;

extern GameMode_t gamemode;
extern char mainwad[25];
extern char maindir[25];

extern boolean useMemCard;
extern char doomdir[35];
extern sceMcTblGetDir saveentries[6];

//pad stuff

static u32 old_pad = 0;
extern Controller mainController;

void M_LauncherInit(void)
{
    runninglauncher = true;

    background = AddAndCreateTextureFromBuffer(backgroundpng, sizeof(backgroundpng), "BACKGROUND", READ_PNG, 0, 0, TEX_ADDRESS_CLAMP); 
    //AddAndCreateTexture("BACKGROUND.PNG", READ_PNG, 0, 0, TEX_ADDRESS_CLAMP, 1);

    fontimage = CreateFontStructFromBuffer("DEFAULT", DefaultFontbmp, DefaultFontDatadat, READ_BMP, sizeof(DefaultFontbmp), sizeof(DefaultFontDatadat)); 
    //CreateFontStruct("DEFAULTFONT.BMP", "DEFAULTFONTDATA.DAT", READ_BMP);

    fontimage->color.r = 0x00;
    fontimage->color.g = 0x00;
    fontimage->color.b = 0x00;

    wadlist = malloc(sizeof(char *) * 10);
    dirlist = malloc(sizeof(char *) * 10);
    gamemodes = (GameMode_t *)malloc(sizeof(GameMode_t) * 10);
}

static void UpdatePad()
{
    struct padButtonStatus buttons;

    u32 new_pad;
    u32 currData;

    int state = GetPadWhenReady(&mainController);

    if (state < 0)
    {
        ERRORLOG("Pad(%d, %d) is disconnected", mainController.port, mainController.slot);
        return;
    }

    state = ReadPad(&mainController);

    buttons = mainController.buttons;

    currData = 0xffff ^ buttons.btns;

    new_pad = currData & ~old_pad;

    if (new_pad & PAD_UP)
    {
        if (wadselect > 0)
        {
            wadselect--;
        }
    }

    if (new_pad & PAD_DOWN)
    {
        if (wadselect < wadlistsize - 1)
        {
            wadselect++;
        }
    }

    if (new_pad & PAD_CROSS)
    {
        runninglauncher = false;
        strcpy(mainwad, wadlist[wadselect]);
        strcpy(maindir, dirlist[wadselect]);
        gamemode = gamemodes[wadselect];

        if (useMemCard)
        {
            char all[40];
            sprintf(doomdir, "%s%s%s", doomdir, "/", maindir);
            sprintf(all, "%s/*", doomdir);

            int ret = mcGetDir(0, 0, all, 0, 6, saveentries);

            mcSync(0, NULL, &ret);

            if (ret == -4)
            {
                mcMkDir(0, 0, doomdir);
                mcSync(0, NULL, &ret);
            }

        }
    }

    old_pad = currData;
}

static void UpperWAD(char *output, char *input)
{
    int len = strlen(input);

    for (int i = 0; i < len; i++)
    {
        output[i] = toupper(input[i]);
    }
}

void M_LauncherRun(void)
{
    for (int i = 0; i<wadlistsize; i++)
        UpperWAD(wadlist[i], wadlist[i]);

     
    u32 halfh = SKYDOOM_HEIGHT >> 1;
    u32 halfw = SKYDOOM_WIDTH >> 1;

    while (runninglauncher)
    {
        UpdatePad();
        ClearScreen(g_Manager.targetBack, g_Manager.gs_context, 0xFF, 0, 0, 255);
        DrawFullScreenQuad(halfh, halfw, background->width, background->height, background);
        PrintText(fontimage, wadlist[wadselect], 50, 200, LEFT);
        StitchDrawBuffer(true);
        DispatchDrawBuffers();
        EndFrame(1);
    }

    ClearScreen(g_Manager.targetBack, g_Manager.gs_context, 0, 0, 0, 255);
    StitchDrawBuffer(true);
    DispatchDrawBuffers();
    EndFrame(1);
}

void M_LauncherDeinit(void)
{
    //ClearManagerTexList(&g_Manager);
  //  CleanFontStruct(fontimage);
   // CleanTextureStruct(background);
    //ClearManagerTexList(&g_Manager);
    background = NULL;
    fontimage = NULL;

    for (int i = 0; i<wadlistsize; i++)
    {
        free(wadlist[i]);
        free(dirlist[i]);
    }
    free(dirlist);
    free(wadlist);
    free(gamemodes);

    wadlist = NULL;
    dirlist = NULL;
    gamemodes = NULL;
}
