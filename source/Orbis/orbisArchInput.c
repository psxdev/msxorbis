/*****************************************************************************
** $Source: /cygdrive/d/Private/_SVNROOT/bluemsx/blueMSX/Src/Sdl/SdlInput.c,v $
**
** $Revision: 1.11 $
**
** $Date: 2008-03-31 19:42:23 $
**
** More info: http://www.bluemsx.com
**
** Copyright (C) 2003-2006 Daniel Vik
**
** This program is free software; you can redistribute it and/or modify
** it under the terms of the GNU General Public License as published by
** the Free Software Foundation; either version 2 of the License, or
** (at your option) any later version.
** 
** This program is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
** GNU General Public License for more details.
**
** You should have received a copy of the GNU General Public License
** along with this program; if not, write to the Free Software
** Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
**
******************************************************************************
*/
#include "ArchInput.h"


#include "Language.h"
#include "Properties.h"
#include "InputEvent.h"
#include "JoystickPort.h"
#include <stdio.h>
#include <debugnet.h>
#include <orbisPad.h>
#include <orbisKeyboard.h>
#include <kb.h>
#include "Keyboard.h"
#include "browser.h"
extern int screenStatus;


static int kbdTable[3][256];

static int inputTypeScanStart = 0;
static int inputTypeScanEnd = 1;

extern Properties *properties;

static void initKbdTable()
{
	//memset (kbdTable, 0, sizeof(kbdTable));

	kbdTable[0][0x30] = EC_0;
	kbdTable[0][0x31] = EC_1;
	kbdTable[0][0x32] = EC_2;
	kbdTable[0][0x33] = EC_3;
	kbdTable[0][0x34] = EC_4;
	kbdTable[0][0x35] = EC_5;
	kbdTable[0][0x36] = EC_6;
	kbdTable[0][0x37] = EC_7;
	kbdTable[0][0x38] = EC_8;
	kbdTable[0][0x39] = EC_9;
	kbdTable[0][0x22] = EC_2;
	kbdTable[0][0x27] = EC_NEG;
	kbdTable[0][0x3f] = EC_CIRCFLX;
	kbdTable[0][0x27] = EC_BKSLASH;
	kbdTable[0][0x5b] = EC_AT;
	kbdTable[0][0x5b] = EC_LBRACK;
	kbdTable[0][0x3b] = EC_SEMICOL;
	kbdTable[0][0x3a] = EC_COLON;
	kbdTable[0][0x5d] = EC_RBRACK;
	kbdTable[0][0x2c] = EC_COMMA;
	kbdTable[0][0x2e] = EC_PERIOD;
	kbdTable[0][0x27] = EC_DIV;
	//kbdTable[0][orbisKeyboardNames[0][KEY_]] = EC_UNDSCRE;

	kbdTable[0][0x61] = EC_A;
	kbdTable[0][0x62] = EC_B;
	kbdTable[0][0x63] = EC_C;
	kbdTable[0][0x64] = EC_D;
	kbdTable[0][0x65] = EC_E;
	kbdTable[0][0x66] = EC_F;
	kbdTable[0][0x67] = EC_G;
	kbdTable[0][0x68] = EC_H;
	kbdTable[0][0x69] = EC_I;
	kbdTable[0][0x6a] = EC_J;
	kbdTable[0][0x6b] = EC_K;
	kbdTable[0][0x6c] = EC_L;
	kbdTable[0][0x6d] = EC_M;
	kbdTable[0][0x6e] = EC_N;
	kbdTable[0][0x6f] = EC_O;
	kbdTable[0][0x70] = EC_P;
	kbdTable[0][0x71] = EC_Q;
	kbdTable[0][0x72] = EC_R;
	kbdTable[0][0x73] = EC_S;
	kbdTable[0][0x74] = EC_T;
	kbdTable[0][0x75] = EC_U;
	kbdTable[0][0x76] = EC_V;
	kbdTable[0][0x77] = EC_W;
	kbdTable[0][0x78] = EC_X;
	kbdTable[0][0x79] = EC_Y;
	kbdTable[0][0x7a] = EC_Z;

	kbdTable[0][0x12] = EC_F1;
	kbdTable[0][0x13] = EC_F2;
	kbdTable[0][0x14] = EC_F3;
	kbdTable[0][0x15] = EC_F4;
	kbdTable[0][0x16] = EC_F5;
	kbdTable[0][0x1b] = EC_ESC;
	kbdTable[0][0x9] = EC_TAB;
	kbdTable[0][0x11] = EC_STOP; //F12 STOP
	kbdTable[0][0x8] = EC_BKSPACE;
	//kbdTable[0][SDLK_END      ] = EC_SELECT;
	kbdTable[0][0xd] = EC_RETURN;
	kbdTable[0][0x20] = EC_SPACE;
	//kbdTable[0][] = EC_CLS;
	//kbdTable[0][orbisKeyboardNames[0][KEY_INSERT]] = EC_INS;
	//kbdTable[0][orbisKeyboardNames[0][KEY_DELETE]] = EC_DEL;
	kbdTable[0][0x1] = EC_LEFT;
	kbdTable[0][0x2] = EC_UP;
	kbdTable[0][0x3] = EC_RIGHT;
	kbdTable[0][0x4] = EC_DOWN;

	/*kbdTable[0][SDLK_KP_MULTIPLY] = EC_NUMMUL;
	kbdTable[0][SDLK_KP_PLUS    ] = EC_NUMADD;
	kbdTable[0][SDLK_KP_DIVIDE  ] = EC_NUMDIV;
	kbdTable[0][SDLK_KP_MINUS   ] = EC_NUMSUB;
	kbdTable[0][SDLK_KP_PERIOD  ] = EC_NUMPER;
	kbdTable[0][SDLK_PAGEDOWN   ] = EC_NUMCOM;*/
	/*kbdTable[0][SDLK_KP0] = EC_NUM0;
	kbdTable[0][SDLK_KP1] = EC_NUM1;
	kbdTable[0][SDLK_KP2] = EC_NUM2;
	kbdTable[0][SDLK_KP3] = EC_NUM3;
	kbdTable[0][SDLK_KP4] = EC_NUM4;
	kbdTable[0][SDLK_KP5] = EC_NUM5;
	kbdTable[0][SDLK_KP6] = EC_NUM6;
	kbdTable[0][SDLK_KP7] = EC_NUM7;
	kbdTable[0][SDLK_KP8] = EC_NUM8;
	kbdTable[0][SDLK_KP9] = EC_NUM9;*/
/*
	kbdTable[0][SDLK_LSUPER  ] = EC_TORIKE;
	kbdTable[0][SDLK_RSUPER  ] = EC_JIKKOU;
	kbdTable[0][SDLK_LSHIFT  ] = EC_LSHIFT;
	kbdTable[0][SDLK_RSHIFT  ] = EC_RSHIFT;
	kbdTable[0][SDLK_LCTRL   ] = EC_CTRL;
	kbdTable[0][SDLK_LALT    ] = EC_GRAPH;
	kbdTable[0][SDLK_RALT    ] = EC_CODE;
	kbdTable[0][orbisKeyboardNames[0][]] = EC_CAPS;
	kbdTable[0][SDLK_KP_ENTER] = EC_PAUSE;
	kbdTable[0][SDLK_SYSREQ  ] = EC_PRINT;*/

	/*kbdTable[1][SDLK_SPACE       ] = EC_JOY1_BUTTON1;
	kbdTable[1][SDLK_LCTRL       ] = EC_JOY1_BUTTON2;
	kbdTable[1][SDLK_LEFT        ] = EC_JOY1_LEFT;
	kbdTable[1][SDLK_UP          ] = EC_JOY1_UP;
	kbdTable[1][SDLK_RIGHT       ] = EC_JOY1_RIGHT;
	kbdTable[1][SDLK_DOWN        ] = EC_JOY1_DOWN;*/
	/*kbdTable[1][SDLK_0           ] = EC_COLECO1_0;
	kbdTable[1][SDLK_1           ] = EC_COLECO1_1;
	kbdTable[1][SDLK_2           ] = EC_COLECO1_2;
	kbdTable[1][SDLK_3           ] = EC_COLECO1_3;
	kbdTable[1][SDLK_4           ] = EC_COLECO1_4;
	kbdTable[1][SDLK_5           ] = EC_COLECO1_5;
	kbdTable[1][SDLK_6           ] = EC_COLECO1_6;
	kbdTable[1][SDLK_7           ] = EC_COLECO1_7;
	kbdTable[1][SDLK_8           ] = EC_COLECO1_8;
	kbdTable[1][SDLK_9           ] = EC_COLECO1_9;
	kbdTable[1][SDLK_MINUS       ] = EC_COLECO1_STAR;
	kbdTable[1][SDLK_EQUALS      ] = EC_COLECO1_HASH;

	kbdTable[2][SDLK_KP0     ] = EC_COLECO2_0;
	kbdTable[2][SDLK_KP1     ] = EC_COLECO2_1;
	kbdTable[2][SDLK_KP2     ] = EC_COLECO2_2;
	kbdTable[2][SDLK_KP3     ] = EC_COLECO2_3;
	kbdTable[2][SDLK_KP4     ] = EC_COLECO2_4;
	kbdTable[2][SDLK_KP5     ] = EC_COLECO2_5;
	kbdTable[2][SDLK_KP6     ] = EC_COLECO2_6;
	kbdTable[2][SDLK_KP7     ] = EC_COLECO2_7;
	kbdTable[2][SDLK_KP8     ] = EC_COLECO2_8;
	kbdTable[2][SDLK_KP9     ] = EC_COLECO2_9;
	kbdTable[2][SDLK_KP_MULTIPLY    ] = EC_COLECO2_STAR;
	kbdTable[2][SDLK_KP_DIVIDE      ] = EC_COLECO2_HASH;*/
}

void piInputResetJoysticks()
{
	/*SDL_QuitSubSystem(SDL_INIT_JOYSTICK);
	SDL_InitSubSystem(SDL_INIT_JOYSTICK);
	SDL_JoystickEventState(SDL_ENABLE);
	SDL_JoystickOpen(0);
	SDL_JoystickOpen(1);*/
}

void orbisInputResetMSXDevices(int realMice, int realJoysticks)
{
	/*int port = 0;
	// Any connected joysticks take priority
	while (realJoysticks > 0 && port < 2) {
		if (port == 0) {
			properties->joy1.typeId = JOYSTICK_PORT_JOYSTICK;
			joystickPortSetType(port, JOYSTICK_PORT_JOYSTICK);
		} else if (port == 1) {
			properties->joy2.typeId = JOYSTICK_PORT_JOYSTICK;
			joystickPortSetType(port, JOYSTICK_PORT_JOYSTICK);
		}
		
		realJoysticks--;
		port++;

		debugNetPrintf(ERROR,"[MSXORBIS] Connecting a joystick to port %d\n", port);
	}

	// If there are still open ports and a mouse,
	// connect it to the remaining port 
	if (realMice > 0) {
		if (port == 0) {
			properties->joy1.typeId = JOYSTICK_PORT_MOUSE;
			joystickPortSetType(port++, properties->joy1.typeId);
			debugNetPrintf(ERROR,"[MSXORBIS] Connecting a mouse to port 1\n");
		} else if (port == 1) {
			properties->joy2.typeId = JOYSTICK_PORT_MOUSE;
			joystickPortSetType(port++, properties->joy2.typeId);
			debugNetPrintf(ERROR,"[MSXORBIS] Connecting a mouse to port 2\n");
		}
		
		realMice--;
	}*/
}

void keyboardInit(Properties *properties)
{
	//if (strncmp(properties->emulation.machineName, "COL", 3) == 0) {
	//	inputTypeScanStart = 1;
	//	inputTypeScanEnd = 2;
	//	debugNetPrintf(ERROR,"[MSXORBIS] Initializing ColecoVision input\n");
	//}

	initKbdTable();
	inputEventReset();
}

void keyboardUpdate()
{
	orbisKeyboardUpdate();
	uint8_t key_shift=orbisKeyboardGetMsxKeyShift();
	if(key_shift)
	{
		debugNetPrintf(3,"shift\n");

		inputEventSet(EK_LSHIFT);
	}
	else
	{
		//debugNetPrintf(3,"un shift\n");

		inputEventUnset(EK_LSHIFT);
	}
	uint8_t key_ctrl=orbisKeyboardGetMsxKeyCtrl();
	if(key_ctrl)
	{
				debugNetPrintf(3,"ctrl\n");

		inputEventSet(EK_CTRL);
	}
	else
	{
		//debugNetPrintf(3,"un ctrl\n");

		inputEventUnset(EK_CTRL);
	}
	uint8_t key=orbisKeyboardGetMsxKey();
	if(orbisKeyboardGetMsxKeyStatus()==1)
	{
		debugNetPrintf(3,"[MSXORBIS] %s key %d\n",__FUNCTION__,key);

		inputEventSet(kbdTable[0][key]);
	}
	else
	{		//debugNetPrintf(3,"un key %d\n",key);

		inputEventUnset(kbdTable[0][key]);
	}
	
/** Keyboard() ***********************************************/
/** Modify keyboard matrix.                                 **/

	/*int i;
	for (i = inputTypeScanStart; i <= inputTypeScanEnd; i++) {
		if (event->keysym.scancode == 0x7A) {
			if (event->type == SDL_KEYUP) 
				inputEventUnset(kbdTable[i][SDLK_RALT]);
			else if (event->type == SDL_KEYDOWN)
				inputEventSet(kbdTable[i][SDLK_RALT]);
		} else if (event->type == SDL_KEYUP) {
			if (event->keysym.sym == 0 && event->keysym.scancode == 58)
				inputEventUnset(kbdTable[i][SDLK_CAPSLOCK]);
			else
				inputEventUnset(kbdTable[i][event->keysym.sym]);
		} else if (event->type == SDL_KEYDOWN) {
			if (event->keysym.sym == 0 && event->keysym.scancode == 58)
				inputEventSet(kbdTable[i][SDLK_CAPSLOCK]);
			else
				inputEventSet(kbdTable[i][event->keysym.sym]);
		}
	}*/
}
void joystickUpdate()
{
	int ret;
	unsigned int buttons;
	ret=orbisPadUpdate();
	if(ret==0)
	{
	if(orbisPadGetButtonPressed(ORBISPAD_L2|ORBISPAD_R2))
	{
		debugNetPrintf(3,"combo L2R2\n");
		buttons=orbisPadGetCurrentButtonsPressed();
		buttons&= ~(ORBISPAD_L2|ORBISPAD_R2);
		orbisPadSetCurrentButtonsPressed(buttons);
		emulatorSuspend();
		screenStatus=SCREEN_BROWSER;
		//LoadCart("host0:ROM/GRADIUS2.ROM",0,MAP_GUESS);
		//ResetMSX(Mode,32,4);
		//LoadFile("host0:ROM/ABADIA.DSK");
	}
	if(orbisPadGetButtonPressed(ORBISPAD_L1|ORBISPAD_R1))
	{
		debugNetPrintf(3,"combo L1R1\n");
		buttons=orbisPadGetCurrentButtonsPressed();
		buttons&= ~(ORBISPAD_L1|ORBISPAD_R1);
		orbisPadSetCurrentButtonsPressed(buttons);
		//LoadCart("host0:ROM/GRADIUS2.ROM",0,MAP_KONAMI4);
		//LoadFile("host0:ROM/ABADIA32.DSK");
		inputEventSet(EC_Y);
		inputEventSet(EC_P);

	}
	if(orbisPadGetButtonPressed(ORBISPAD_LEFT) || orbisPadGetButtonHold(ORBISPAD_LEFT))
	{
		//JoyState|=KBD_LEFT;LastKey=CON_LEFT;
		debugNetPrintf(INFO,"press left\n");
		
		inputEventSet(EC_LEFT);
	}
	if(orbisPadGetButtonPressed(ORBISPAD_RIGHT) || orbisPadGetButtonHold(ORBISPAD_RIGHT))
	{
		//JoyState|=KBD_RIGHT;LastKey=CON_RIGHT;
		debugNetPrintf(INFO,"press right\n");
		
		inputEventSet(EC_RIGHT);
		
	}
	if(orbisPadGetButtonPressed(ORBISPAD_UP) || orbisPadGetButtonHold(ORBISPAD_UP))
	{
		//JoyState|=KBD_UP;LastKey=CON_UP;
		debugNetPrintf(INFO,"press up\n");
		
		inputEventSet(EC_UP);
		
	}
	if(orbisPadGetButtonPressed(ORBISPAD_DOWN) || orbisPadGetButtonHold(ORBISPAD_DOWN))
	{
		//JoyState|=KBD_DOWN;LastKey=CON_DOWN;
		debugNetPrintf(INFO,"press down\n");
		
		inputEventSet(EC_DOWN);
		
	}
	if(orbisPadGetButtonPressed(ORBISPAD_CROSS) || orbisPadGetButtonHold(ORBISPAD_CROSS))
	{
		//JoyState|=0x31;
		debugNetPrintf(INFO,"press cross\n");
		inputEventSet(EC_SPACE);
		
		
	}
	if(orbisPadGetButtonPressed(ORBISPAD_CIRCLE))
	{
		//JoyState|=0x4D;
		debugNetPrintf(INFO,"press circle\n");
		
		inputEventSet(EC_M);
		
		
	}
	if(orbisPadGetButtonPressed(ORBISPAD_TRIANGLE))
	{
		//JoyState|=BTN_FIREL;
		debugNetPrintf(INFO,"press triangle\n");
		//inputEventSet(EK_F4);
		
	}
	if(orbisPadGetButtonPressed(ORBISPAD_L2))
	{
		//JoyState|=BTN_FIREL;
		debugNetPrintf(INFO,"press l2\n");
		//inputEventSet(EC_F3);//EK_F3
		 //inputEventSet(EC_Y);
		//inputEventSet(EC_P);
	//actionHarddiskInsert(2);
		actionEmuSpeedDecrease();
		
	}
	if(orbisPadGetButtonPressed(ORBISPAD_R2))
	{
		//JoyState|=BTN_FIREL;
		debugNetPrintf(INFO,"press r2\n");
		//inputEventSet(EK_F4);
		//actionEmuResetHard();
		//XKBD_SET(KBD_CONTROL);
		actionEmuSpeedIncrease();
		
	}
	
	if(orbisPadGetButtonPressed(ORBISPAD_R1))
	{
		//JoyState|=BTN_FIREL;
		debugNetPrintf(INFO,"press r1\n");
		//inputEventSet(EK_F2);
		actionEmuResetSoft();
		
	}
	if(orbisPadGetButtonPressed(ORBISPAD_L1))
	{
		//JoyState|=BTN_FIREL;
		debugNetPrintf(INFO,"press L1\n");
		inputEventSet(EK_F1);
		
	}
	if(orbisPadGetButtonPressed(ORBISPAD_SQUARE))
	{
		//JoyState|=BTN_FIRER;
		debugNetPrintf(3,"press square\n");
		inputEventSet(EK_F5);
		
	}
	if(orbisPadGetButtonReleased(ORBISPAD_LEFT))
	{
		//JoyState|=~BTN_LEFT;
		debugNetPrintf(INFO,"release left\n");
		
		inputEventUnset(EC_LEFT);
		
	}
	if(orbisPadGetButtonReleased(ORBISPAD_RIGHT))
	{
		//JoyState|=~BTN_RIGHT;
		debugNetPrintf(INFO,"release right\n");
		
		inputEventUnset(EC_RIGHT);
		
	}
	if(orbisPadGetButtonReleased(ORBISPAD_UP))
	{
		//JoyState|=~BTN_UP;
		debugNetPrintf(INFO,"release up\n");
		
		inputEventUnset(EC_UP);
		
	}
	if(orbisPadGetButtonReleased(ORBISPAD_DOWN))
	{
		//JoyState|=~BTN_DOWN;
		debugNetPrintf(INFO,"release down\n");
		
		inputEventUnset(EC_DOWN);
		
	}
	if(orbisPadGetButtonReleased(ORBISPAD_CROSS))
	{
	//	JoyState|=~BTN_FIREA;
		debugNetPrintf(INFO,"release cross\n");
		
		inputEventUnset(EC_SPACE);
		
	}
	if(orbisPadGetButtonReleased(ORBISPAD_CIRCLE))
	{
		//JoyState|=~BTN_FIREB;
		debugNetPrintf(INFO,"release circle\n");
		
		inputEventUnset(EC_M);
		
	}
	if(orbisPadGetButtonReleased(ORBISPAD_TRIANGLE))
	{
	//	JoyState|=~BTN_FIREL;
		debugNetPrintf(INFO,"release triangle\n");
		
		//inputEventUnset(EK_F4);
		
	}
	if(orbisPadGetButtonReleased(ORBISPAD_SQUARE))
	{
		//JoyState|=~BTN_FIRER;
		debugNetPrintf(INFO,"release square\n");
		
		inputEventUnset(EK_F5);
		
	}
	if(orbisPadGetButtonReleased(ORBISPAD_L2))
	{
		//JoyState|=BTN_FIREL;
		debugNetPrintf(INFO,"release l2\n");
		//inputEventUnset(EC_F3);//EK_F3
		
	}
	if(orbisPadGetButtonReleased(ORBISPAD_R2))
	{
		//JoyState|=BTN_FIREL;
		debugNetPrintf(INFO,"release triangle\n");
		//inputEventUnset(EK_F4);
		//XKBD_RES(KBD_CONTROL);
		
	}
	if(orbisPadGetButtonReleased(ORBISPAD_R1))
	{
		//JoyState|=BTN_FIREL;
		debugNetPrintf(INFO,"release r1\n");
		//inputEventUnset(EK_F2);
		
	}
	if(orbisPadGetButtonReleased(ORBISPAD_L1))
	{
		//JoyState|=BTN_FIREL;
		debugNetPrintf(3,"release l1\n");
		inputEventUnset(EK_F1);
		
		
	}
 
	
	}
}
void joystickAxisUpdate()
{
	/*if (event->which == 0) {
		if (event->axis == 0) {
			// Left/right
			if (event->value < -3200) {
				inputEventSet(EC_JOY1_LEFT);
				inputEventUnset(EC_JOY1_RIGHT);
			} else if (event->value > 3200) {
				inputEventUnset(EC_JOY1_LEFT);
				inputEventSet(EC_JOY1_RIGHT);
			} else {
				inputEventUnset(EC_JOY1_RIGHT);
				inputEventUnset(EC_JOY1_LEFT);
			}
		} else if (event->axis == 1) {
			// Up/down
			if (event->value < -3200) {
				inputEventSet(EC_JOY1_UP);
				inputEventUnset(EC_JOY1_DOWN);
			} else if (event->value > 3200) {
				inputEventUnset(EC_JOY1_UP);
				inputEventSet(EC_JOY1_DOWN);
			} else {
				inputEventUnset(EC_JOY1_UP);
				inputEventUnset(EC_JOY1_DOWN);
			}
		}
	} else if (event->which == 1) {
		if (event->axis == 0) {
			// Left/right
			if (event->value < -3200) {
				inputEventSet(EC_JOY2_LEFT);
				inputEventUnset(EC_JOY2_RIGHT);
			} else if (event->value > 3200) {
				inputEventUnset(EC_JOY2_LEFT);
				inputEventSet(EC_JOY2_RIGHT);
			} else {
				inputEventUnset(EC_JOY2_RIGHT);
				inputEventUnset(EC_JOY2_LEFT);
			}
		} else if (event->axis == 1) {
			// Up/down
			if (event->value < -3200) {
				inputEventSet(EC_JOY2_UP);
				inputEventUnset(EC_JOY2_DOWN);
			} else if (event->value > 3200) {
				inputEventUnset(EC_JOY2_UP);
				inputEventSet(EC_JOY2_DOWN);
			} else {
				inputEventUnset(EC_JOY2_UP);
				inputEventUnset(EC_JOY2_DOWN);
			}
		}
	}*/
}

void joystickButtonUpdate()
{
	/*if (event->type == SDL_JOYBUTTONDOWN) {
		if (event->which == 0) {
			if (event->button == 0) {
				inputEventSet(EC_JOY1_BUTTON1);
			} else if (event->button == 1) {
				inputEventSet(EC_JOY1_BUTTON2);
			} else if (event->button == 8) {
                inputEventSet(EC_JOY_BUTTONL);
            } else if (event->button == 9) {
                inputEventSet(EC_JOY_BUTTONR);
            }
		} else if (event->which == 1) {
			if (event->button == 0) {
				inputEventSet(EC_JOY2_BUTTON1);
			} else if (event->button == 1) {
				inputEventSet(EC_JOY2_BUTTON2);
			} else if (event->button == 8) {
                inputEventSet(EC_JOY_BUTTONL);
            } else if (event->button == 9) {
                inputEventSet(EC_JOY_BUTTONR);
			}
		}
	} else if (event->type == SDL_JOYBUTTONUP) {
		if (event->which == 0) {
			if (event->button == 0) {
				inputEventUnset(EC_JOY1_BUTTON1);
			} else if (event->button == 1) {
				inputEventUnset(EC_JOY1_BUTTON2);
			} else if (event->button == 8) {
                inputEventUnset(EC_JOY_BUTTONL);
            } else if (event->button == 9) {
                inputEventUnset(EC_JOY_BUTTONR);
			}
		} else if (event->which == 1) {
			if (event->button == 0) {
				inputEventUnset(EC_JOY2_BUTTON1);
			} else if (event->button == 1) {
				inputEventUnset(EC_JOY2_BUTTON2);
			} else if (event->button == 8) {
                inputEventUnset(EC_JOY_BUTTONL);
            } else if (event->button == 9) {
                inputEventUnset(EC_JOY_BUTTONR);
			}
		}
	}*/
}

void  archUpdateJoystick() {}
UInt8 archJoystickGetState(int joystickNo) { return 0; }
int   archJoystickGetCount() { return 0; }
char* archJoystickGetName(int index) { return ""; }
void  archMouseSetForceLock(int lock) { }
void  archPollInput() 
{ 
	keyboardUpdate();
	joystickUpdate();
}
void  archKeyboardSetSelectedKey(int keyCode) {}
char* archGetSelectedKey() { return ""; }
char* archGetMappedKey() { return ""; }
int   archKeyboardIsKeyConfigured(int msxKeyCode) { return 0; }
int   archKeyboardIsKeySelected(int msxKeyCode) { return 0; }
char* archKeyconfigSelectedKeyTitle() { return ""; }
char* archKeyconfigMappedToTitle() { return ""; }
char* archKeyconfigMappingSchemeTitle() { return ""; }

