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
//	DOOM graphics stuff for X11, UNIX.
//
//-----------------------------------------------------------------------------

static const char
	rcsid[] = "$Id: i_x.c,v 1.6 1997/02/03 22:45:10 b1 Exp $";

#include <stdlib.h>
#include <unistd.h>

#include <stdarg.h>
#include <sys/time.h>
#include <sys/types.h>

#include <signal.h>

#include "doomstat.h"
#include "i_system.h"
//#include "v_video.h"
#include "m_argv.h"
#include "d_main.h"
#include "z_zone.h"
#include "doomdef.h"
#include "i_video.h"
#include "i_sound.h"
#include <graph.h>
#include "ps_global.h"
#include "pad/ps_pad.h"
#include "dma/ps_dma.h"
#include "gs/ps_gs.h"
#include "textures/ps_texture.h"
#include "gamemanager/ps_manager.h"
#include "system/ps_timer.h"
#include "graphics/ps_drawing.h"
#include "log/ps_log.h"

#define PS2_TEXTURE_SIZE_X 512
#define PS2_TEXTURE_SIZE_Y (PS2_TEXTURE_SIZE_X >> 1)

typedef struct ColorPaletted
{
	u8 r;
	u8 g;
	u8 b;
	u8 a;
} ColorPaletted;

extern	byte*		screens[5];
extern  int	dirtybox[4];
extern	byte	gammatable[5][256];
extern	int	usegamma;
extern u32 SKYDOOM_HEIGHT;
extern u32 SKYDOOM_WIDTH;
extern Controller mainController;

__attribute__((aligned(128))) static ColorPaletted colors[256];
u32 SKYDOOM_HEIGHT_HALF;
u32 SKYDOOM_WIDTH_HALF;
Texture image;

// Fake mouse handling.
// This cannot work properly w/o DGA.
// Needs an invisible mouse cursor at least.

float timestart, timeend;


static u32 events_id[16] = {KEY_ESCAPE, KEY_SPEED, 0, KEY_PAUSE, 
							KEY_UPARROW, KEY_RIGHTARROW, KEY_DOWNARROW, KEY_LEFTARROW, 
							0, KEY_FIRE, KEY_CYCLE_LEFT, KEY_CYCLE_RIGHT, 
							KEY_BACKSPACE, KEY_SELECT, KEY_ENTER, KEY_TAB};
static u8 JoyRHPv = 127;
static u8 JoyLVPv = 127;
static u8 JoyLHPv = 127;
static u8 JoyRVPv = 127;
static u32 lower = 50;
static u32 upper = 200;

static void AddJoyStickEvent(int keySpoof, evtype_t eventType)
{
	event_t event;
	event.type = eventType;
	event.data1 = keySpoof;
	D_PostEvent(&event);
}

static void CheckLowerAxisDown(u32 joyVal, u32 joyValPrev, u32 threshold, int keySpoof)
{
	if (joyVal <= threshold && joyValPrev > threshold)
	{
		AddJoyStickEvent(keySpoof, ev_keydown);
	}
}

static void CheckLowerAxisUp(u32 joyVal, u32 joyValPrev, u32 threshold, int keySpoof)
{
	if (joyVal > threshold && joyValPrev <= threshold)
	{
		AddJoyStickEvent(keySpoof, ev_keyup);
	}
}

static void CheckUpperAxisUp(u32 joyVal, u32 joyValPrev, u32 threshold, int keySpoof)
{
	if (joyVal < threshold && joyValPrev >= threshold)
	{
		AddJoyStickEvent(keySpoof, ev_keyup);
	}
}

static void CheckUpperAxisDown(u32 joyVal, u32 joyValPrev, u32 threshold, int keySpoof)
{
	if (joyVal >= threshold && joyValPrev < threshold)
	{
		AddJoyStickEvent(keySpoof, ev_keydown);
	}
}

void I_UpdatePad()
{
	static u32 old_pad = 0;

	int state = GetPadWhenReady(&mainController);

    if (state < 0)
    {
        ERRORLOG("Pad(%d, %d) is disconnected", mainController.port, mainController.slot);
        return;
    }

    state = ReadPad(&mainController);

	event_t event;

	if (!state)
	{
		u32 currData = 0xffff ^ mainController.buttons.btns;

		u32 new_pad = currData & ~old_pad;

		CheckLowerAxisDown(mainController.buttons.rjoy_h, JoyRHPv, lower, KEY_LOOK_LEFT);
		CheckLowerAxisUp(mainController.buttons.rjoy_h, JoyRHPv, lower, KEY_LOOK_LEFT);

		CheckUpperAxisDown(mainController.buttons.rjoy_h, JoyRHPv, upper, KEY_LOOK_RIGHT);
		CheckUpperAxisUp(mainController.buttons.rjoy_h, JoyRHPv, upper, KEY_LOOK_RIGHT);

		CheckLowerAxisDown(mainController.buttons.rjoy_v, JoyRVPv, lower, KEY_LOOK_UP);
		CheckLowerAxisUp(mainController.buttons.rjoy_v, JoyRVPv, lower, KEY_LOOK_UP);

		CheckUpperAxisDown(mainController.buttons.rjoy_v, JoyRVPv, upper, KEY_LOOK_DOWN);
		CheckUpperAxisUp(mainController.buttons.rjoy_v, JoyRVPv, upper, KEY_LOOK_DOWN);

		CheckLowerAxisDown(mainController.buttons.ljoy_h, JoyLHPv, lower, KEY_MOVE_LEFT);
		CheckLowerAxisUp(mainController.buttons.ljoy_h, JoyLHPv, lower, KEY_MOVE_LEFT);

		CheckUpperAxisDown(mainController.buttons.ljoy_h, JoyLHPv, upper, KEY_MOVE_RIGHT);
		CheckUpperAxisUp(mainController.buttons.ljoy_h, JoyLHPv, upper, KEY_MOVE_RIGHT);

		CheckLowerAxisDown(mainController.buttons.ljoy_v, JoyLVPv, lower, KEY_UPARROW);
		CheckLowerAxisUp(mainController.buttons.ljoy_v, JoyLVPv, lower, KEY_UPARROW);

		CheckUpperAxisDown(mainController.buttons.ljoy_v, JoyLVPv, upper, KEY_DOWNARROW);
		CheckUpperAxisUp(mainController.buttons.ljoy_v, JoyLVPv, upper, KEY_DOWNARROW);

		JoyRVPv = mainController.buttons.rjoy_v;
		JoyRHPv = mainController.buttons.rjoy_h;
		JoyLHPv = mainController.buttons.ljoy_h;
		JoyLVPv = mainController.buttons.ljoy_v;

		int padType = 0x0001;
		for (int i = 0; padType != 0; i++)
		{
			if (new_pad & padType)
			{
				event.type = ev_keydown;
				event.data1 = events_id[i];
				D_PostEvent(&event);
			}
			
			if (!(currData & padType) && (old_pad & padType))
			{
				event.type = ev_keyup;
				event.data1 = events_id[i];
				D_PostEvent(&event);
			}

			padType <<= 1;
		}

		old_pad = currData;
	}
}

//
// I_StartFrame
//
void I_StartFrame(void)
{
	// er?
}
void I_ShutdownGraphics(void)
{
}

// I_StartTic
//
void I_StartTic(void)
{
	I_UpdatePad();
}

//
// I_UpdateNoBlit
//
void I_UpdateNoBlit(void)
{
	// what is this?
}

//
// I_FinishUpdate
//

void I_FinishUpdate(void)
{
	image.pixels = screens[0];
	ClearScreen(g_Manager.targetBack, g_Manager.gs_context, 0xFF, 0xFF, 0x00, 0x00);
	DrawFullScreenQuad(SKYDOOM_HEIGHT_HALF, SKYDOOM_WIDTH_HALF, &image);
	StitchDrawBuffer(true);
    DispatchDrawBuffers();
	EndFrame(0);
	graph_start_vsync();
	while (!(graph_check_vsync()))
	{
		I_UpdateMusic();
	}
}

//
// I_ReadScreen
//
void I_ReadScreen(byte *scr)
{
	memcpy(scr, screens[0], SCREENWIDTH * SCREENHEIGHT);
}

//
// Palette stuff.
//

void UploadNewPalette(byte *palette)
{
}

//
// I_SetPalette
//
void I_SetPalette(byte *palette)
{
	int i;

	byte *clut_palette = palette;

	for (i = 0; i < 256; i++)
	{
		int swizzledIndex = (i & 231) + ((i & 8) << 1) + ((i & 16) >> 1);
		int c = gammatable[usegamma][*clut_palette++];
		colors[swizzledIndex].r = c;
		c = gammatable[usegamma][*clut_palette++];
		colors[swizzledIndex].g = c;
		c = gammatable[usegamma][*clut_palette++];
		colors[swizzledIndex].b = c;
		colors[swizzledIndex].a = 0x80;
	}
}

void DrawFullScreenQuad(int screenHeight, int screenWidth, Texture *_image)
{
	BeginCommand();
	BindTexture(_image, true);
	DepthTest(true, 1);
    SourceAlphaTest(ATEST_KEEP_FRAMEBUFFER, ATEST_METHOD_NOTEQUAL, 0);
	PrimitiveType(GS_SET_PRIM(PRIM_TRIANGLE_STRIP, PRIM_SHADE_GOURAUD, DRAW_ENABLE, DRAW_DISABLE, DRAW_DISABLE, DRAW_DISABLE, PRIM_MAP_UV, g_Manager.gs_context, PRIM_UNFIXED));
	
	u32 regCount = 3;

	u64 regFlag = ((u64)GIF_REG_RGBAQ) << 0 | ((u64)GIF_REG_UV) << 4 | ((u64)GIF_REG_XYZ2) << 8;

	SetRegSizeAndType(3, regFlag);

	DrawCountDirectRegList(4);

	int u0 = 0;
	int v0 = 0;

	int u1 = (_image->width  << 4);
	int v1 = (_image->height << 4);

	u8 red, green, blue, alpha;

    red = green = blue = 0xFF;

    alpha = 0x80;

	DrawPairU64(GIF_SET_RGBAQ(red, green, blue, alpha, 1), GIF_SET_UV(u0, v0));
	DrawPairU64(GIF_SET_XYZ(CreateGSScreenCoordinates(screenWidth, -), CreateGSScreenCoordinates(screenHeight, -), 0xFFFFFF), GIF_SET_RGBAQ(red, green, blue, alpha, 1));
	DrawPairU64(GIF_SET_UV(u0, v1), GIF_SET_XYZ(CreateGSScreenCoordinates(screenWidth, -), CreateGSScreenCoordinates(screenHeight, +), 0xFFFFFF));
	DrawPairU64(GIF_SET_RGBAQ(red, green, blue, alpha, 1), GIF_SET_UV(u1, v0));
	DrawPairU64(GIF_SET_XYZ(CreateGSScreenCoordinates(screenWidth, +), CreateGSScreenCoordinates(screenHeight, -), 0xFFFFFF), GIF_SET_RGBAQ(red, green, blue, alpha, 1));
	DrawPairU64(GIF_SET_UV(u1, v1), GIF_SET_XYZ(CreateGSScreenCoordinates(screenWidth, +), CreateGSScreenCoordinates(screenHeight, +), 0xFFFFFF));

	SubmitCommand(false);
}

void I_InitGraphics(void)
{
	SKYDOOM_HEIGHT_HALF = SKYDOOM_HEIGHT >> 1;
	SKYDOOM_WIDTH_HALF = SKYDOOM_WIDTH >> 1;

	image.width = SCREENWIDTH;
	image.height = SCREENHEIGHT;
	image.psm = GS_PSM_8;
	image.mipLevels = 0;
	image.mipMaps = NULL;
	image.clut_buffer = (u8*)colors;
	image.mipLevels = 0;
    image.mipMaps = NULL;
    image.mode = 0;
    image.type = PS_TEX_MEMORY;

    CreateClutStructs(&image, GS_PSM_32);

    u32 components = TEXTURE_COMPONENTS_RGBA;

    if (image.psm == GS_PSM_24)
    {
        components = TEXTURE_COMPONENTS_RGB;
    } 

    CreateTexStructs(&image, PS2_TEXTURE_SIZE_X, PS2_TEXTURE_SIZE_Y, image.psm, components, TEXTURE_FUNCTION_MODULATE, PS_FILTER_BILINEAR);

	timestart = timeend = getTicks(g_Manager.timer);
}
