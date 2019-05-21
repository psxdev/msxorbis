
/*****************************************************************************
**
** msxorbis
** https://github.com/psxdev/msxorbis
**
** An MSX Emulator for PlayStation 4 based on blueMSX
**
** Copyright (C) 2003-2006 Daniel Vik
** Copyright (C) 2014 Akop Karapetyan
** Copyright (C) 2019 Antonio Jose Ramos Marquez
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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <kernel.h>
#include <systemservice.h>
#include <orbis2d.h>
#include <orbisPad.h>
#include <orbisKeyboard.h>
#include <orbisAudio.h>
#include <modplayer.h>
#include <ps4link.h>
#include <debugnet.h>
#include <orbisFile.h>
#include <orbisGl.h>
#include <unistd.h>

#include "orbisGlTextureManager.h"
#include "browser.h"

#include "CommandLine.h"
#include "Properties.h"
#include "ArchFile.h"
#include "VideoRender.h"
#include "AudioMixer.h"
#include "Casette.h"
#include "Led.h"
#include "PrinterIO.h"
#include "UartIO.h"
#include "MidiIO.h"
#include "Machine.h"
#include "Board.h"
#include "Emulator.h"
#include "FileHistory.h"
#include "InputEvent.h"
#include "Actions.h"
#include "Language.h"
#include "LaunchFile.h"
#include "ArchEvent.h"
#include "ArchSound.h"
#include "ArchNotifications.h"
#include "JoystickPort.h"
//#include "PiShortcuts.h"
//#include "PiVideo.h"
//#include "PiUdev.h"
#include "InputEvent.h"
#include "Disk.h"
#define EVENT_UPDATE_DISPLAY 2

int screenStatus=SCREEN_EMU;

OrbisGlTextureState *browserTexture=NULL;
OrbisGlTextureState *folderTexture=NULL;
OrbisGlTextureState *fileTexture=NULL;
OrbisGlTextureState *settingsTexture=NULL;
OrbisGlTextureState *creditsTexture=NULL;
OrbisGlProgram *programTexture=NULL;
     


bool flag=true;

Orbis2dConfig  *conf;
OrbisPadConfig *confPad;

typedef struct OrbisGlobalConf
{
    Orbis2dConfig *conf;
    OrbisPadConfig *confPad;
    OrbisAudioConfig *confAudio;
    OrbisKeyboardConfig *confKeyboard;
    ps4LinkConfiguration *confLink;
    int orbisLinkFlag;
}OrbisGlobalConf;

OrbisGlobalConf *myConf;


//msxorbis
//static Shortcuts* shortcuts;
Video *video;
Properties *properties;
static Mixer *mixer;

//static Shortcuts* shortcuts;


static int doQuit = 0;

static long displayUpdates = 0;
static int pendingDisplayEvents = 0;
static void *dpyUpdateAckEvent = NULL;

#define JOYSTICK_COUNT 2

extern MediaDb *romdb;
extern EmuState emuState;

void updateController()
{
    int ret;
    unsigned int buttons=0;
    ret=orbisPadUpdate();
    if(ret==0)
    {
        if(orbisPadGetButtonPressed(ORBISPAD_L2|ORBISPAD_R2) || orbisPadGetButtonHold(ORBISPAD_L2|ORBISPAD_R2))
        {
            debugNetPrintf(DEBUG,"Combo L2R2 pressed\n");
            buttons=orbisPadGetCurrentButtonsPressed();
            buttons&= ~(ORBISPAD_L2|ORBISPAD_R2);
            orbisPadSetCurrentButtonsPressed(buttons);
        }
        if(orbisPadGetButtonPressed(ORBISPAD_L1|ORBISPAD_R1) )
        {
            debugNetPrintf(DEBUG,"Combo L1R1 pressed\n");
            buttons=orbisPadGetCurrentButtonsPressed();
            buttons&= ~(ORBISPAD_L1|ORBISPAD_R1);
            orbisPadSetCurrentButtonsPressed(buttons);
        }
        if(orbisPadGetButtonPressed(ORBISPAD_L1|ORBISPAD_R2) || orbisPadGetButtonHold(ORBISPAD_L1|ORBISPAD_R2))
        {
            debugNetPrintf(DEBUG,"Combo L1R2 pressed\n");
            buttons=orbisPadGetCurrentButtonsPressed();
            buttons&= ~(ORBISPAD_L1|ORBISPAD_R2);
            orbisPadSetCurrentButtonsPressed(buttons);
        }
        if(orbisPadGetButtonPressed(ORBISPAD_L2|ORBISPAD_R1) || orbisPadGetButtonHold(ORBISPAD_L2|ORBISPAD_R1) )
        {
            debugNetPrintf(DEBUG,"Combo L2R1 pressed\n");
            buttons=orbisPadGetCurrentButtonsPressed();
            buttons&= ~(ORBISPAD_L2|ORBISPAD_R1);
            orbisPadSetCurrentButtonsPressed(buttons);
        }
        if(orbisPadGetButtonPressed(ORBISPAD_UP) || orbisPadGetButtonHold(ORBISPAD_UP))
        {
            debugNetPrintf(DEBUG,"Up pressed\n");
        }
        if(orbisPadGetButtonPressed(ORBISPAD_DOWN) || orbisPadGetButtonHold(ORBISPAD_DOWN))
        {
            debugNetPrintf(DEBUG,"Down pressed\n");
        }
        if(orbisPadGetButtonPressed(ORBISPAD_RIGHT) || orbisPadGetButtonHold(ORBISPAD_RIGHT))
        {
            debugNetPrintf(DEBUG,"Right pressed\n");
        }
        if(orbisPadGetButtonPressed(ORBISPAD_LEFT) || orbisPadGetButtonHold(ORBISPAD_LEFT))
        {
            debugNetPrintf(DEBUG,"Left pressed\n");
        }
        if(orbisPadGetButtonPressed(ORBISPAD_TRIANGLE))
        {
            debugNetPrintf(DEBUG,"Triangle pressed exit\n");
            flag=0;
        }
        if(orbisPadGetButtonPressed(ORBISPAD_CIRCLE))
        {
            debugNetPrintf(DEBUG,"Circle pressed\n");
            //orbisAudioResume(0);
        }
        if(orbisPadGetButtonPressed(ORBISPAD_CROSS))
        {
            debugNetPrintf(DEBUG,"Cross pressed rand color\n");
            debugNetPrintf(DEBUG,"[MSXORBIS] %s EMUSTATE %d\n",__FUNCTION__,emuState);

            //orbisAudioStop();
        }
        if(orbisPadGetButtonPressed(ORBISPAD_SQUARE))
        {
            debugNetPrintf(DEBUG,"Square pressed\n");
            //orbisAudioPause(0);
        }
        if(orbisPadGetButtonPressed(ORBISPAD_L1))
        {
            debugNetPrintf(DEBUG,"L1 pressed\n");
          
        }
        if(orbisPadGetButtonPressed(ORBISPAD_L2))
        {
            debugNetPrintf(DEBUG,"L2 pressed\n");
            orbisDebugDisplay();
        }
        if(orbisPadGetButtonPressed(ORBISPAD_R1))
        {
            debugNetPrintf(DEBUG,"R1 pressed\n");
        }
        if(orbisPadGetButtonPressed(ORBISPAD_R2))
        {
            debugNetPrintf(DEBUG,"R2 pressed\n");
        }
    }
}


void finishApp()
{
    orbisAudioFinish();
    orbisKeyboardFinish();
    orbisGlFinish();
    orbisPadFinish();
    orbisFileFinish();
    ps4LinkFinish();
}


static bool initAppGl()
{
    int ret;
    ret=orbisGlInit(ATTR_ORBISGL_WIDTH,ATTR_ORBISGL_HEIGHT);
    if(ret>0)
    {
        glViewport(0, 0, ATTR_ORBISGL_WIDTH, ATTR_ORBISGL_HEIGHT);
        ret=glGetError();
        if(ret)
        {
            debugNetPrintf(ERROR,"glViewport failed: 0x%08X\n",ret);
            return false;
        }
        glClearColor(0.0f, 0.0f, 0.0f, 1.0f); //blue RGBA
        ret=glGetError();
        if(ret)
        {
            debugNetPrintf(ERROR,"glClearColor failed: 0x%08X\n",ret);
            return false;
        }
        return true;
    }
    return false;
}


bool initApp()
{
    int ret;
    sceSystemServiceHideSplashScreen();
    //more library initialiazation here pad,filebroser,audio,keyboard, etc
    //....
    orbisFileInit();
    orbisFileBrowserInit("host0:app/msxorbis/GAMES");
    
    ret=orbisPadInitWithConf(myConf->confPad);
    if(ret)
    {
        confPad=orbisPadGetConf();
        ret=orbisAudioInitWithConf(myConf->confAudio);
        if(ret==1)
        {
            //ret=orbisKeyboardInitWithConf(myConf->confKeyboard);
			ret=orbisKeyboardInit();
			debugNetPrintf(DEBUG,"orbisKeyboardInit %d\n",ret);
			if(ret==1)
			{
				myConf->confKeyboard=OrbisKeyboardGetConf();
				ret=orbisKeyboardOpen();
				debugNetPrintf(DEBUG,"orbisKeyboardOpen %d\n",ret);
			}
            //ret=orbisKeyboardInit();
            //debugNetPrintf(DEBUG,"orbisKeyboardInit %d\n",ret);
			    //if(ret==1)
                //{
              //      myConf->confKeyboard=OrbisKeyboardGetConf();
                //    ret=orbisKeyboardOpen();
                  //  debugNetPrintf(DEBUG,"orbisKeyboardOpen %d\n",ret);
                //}
            //if(ret!=1)
              //  return false;
        }
    }
    else
    {
        return false;
    }
    if(!initAppGl())
        return false;

    return true;
}


void msxOrbisSetDefaultDirectories(const char *rootDir)
{
    char buffer[512]; // FIXME

    debugNetPrintf(DEBUG,"[MSXORBIS] %s %s before propertiesSetDirectory\n",__FUNCTION__,rootDir);

    propertiesSetDirectory(rootDir, rootDir);

    debugNetPrintf(DEBUG,"[MSXORBIS] %s after propertiesSetDirectory\n",__FUNCTION__);


   // sprintf(buffer, "%s/Audio Capture", rootDir);
   // archCreateDirectory(buffer);
   // actionSetAudioCaptureSetDirectory(buffer, "");

   // sprintf(buffer, "%s/Video Capture", rootDir);
   // archCreateDirectory(buffer);
   // actionSetAudioCaptureSetDirectory(buffer, "");

   // sprintf(buffer, "%s/QuickSave", rootDir);
   // archCreateDirectory(buffer);
   // actionSetQuickSaveSetDirectory(buffer, "");

    sprintf(buffer, "%s/SRAM", rootDir);
    //archCreateDirectory(buffer);
    boardSetDirectory(buffer);

    sprintf(buffer, "%s/Casinfo", rootDir);
    //archCreateDirectory(buffer);
    tapeSetDirectory(buffer, "");

    sprintf(buffer, "%s/Databases", rootDir);
    //archCreateDirectory(buffer);
    debugNetPrintf(DEBUG,"[MSXORBIS] %s before mediaDbLoad\n",__FUNCTION__);

    mediaDbLoad();
    debugNetPrintf(DEBUG,"[MSXORBIS] %s after mediaDbLoad\n",__FUNCTION__);

    MediaType *it=mediaDbLookup(romdb,"2ae97f3c8152f56695c3d3a2dedaa781bf0a15f1",0);
    if(it!=NULL)
    {
        debugNetPrintf(DEBUG,"[MSXORBIS] %s digest=%s\n",__FUNCTION__,"2ae97f3c8152f56695c3d3a2dedaa781bf0a15f1");
        debugNetPrintf(DEBUG,"[MSXORBIS] %s genmsxid=%d\n",__FUNCTION__,it->genmsxid);
        debugNetPrintf(DEBUG,"[MSXORBIS] %s romType=%d\n",__FUNCTION__,it->romType);
        debugNetPrintf(DEBUG,"[MSXORBIS] %s media=%d\n",__FUNCTION__,it->media);
        debugNetPrintf(DEBUG,"[MSXORBIS] %s original=%d\n",__FUNCTION__,it->original);
        debugNetPrintf(DEBUG,"[MSXORBIS] %s system=%s\n",__FUNCTION__,it->system);
        debugNetPrintf(DEBUG,"[MSXORBIS] %s title=%s\n",__FUNCTION__,it->title);
        debugNetPrintf(DEBUG,"[MSXORBIS] %s company=%s\n",__FUNCTION__,it->company);
        debugNetPrintf(DEBUG,"[MSXORBIS] %s country=%s\n",__FUNCTION__,it->country);
        debugNetPrintf(DEBUG,"[MSXORBIS] %s year=%s\n",__FUNCTION__,it->year);
        debugNetPrintf(DEBUG,"[MSXORBIS] %s remark=%s\n",__FUNCTION__,it->remark);
        debugNetPrintf(DEBUG,"[MSXORBIS] %s originalData=%s\n",__FUNCTION__,it->originalData);
        debugNetPrintf(DEBUG,"[MSXORBIS] %s startData=%s\n",__FUNCTION__,it->startData);
        debugNetPrintf(DEBUG,"[MSXORBIS] %s ---------------------------\n",__FUNCTION__);
    }

    //sprintf(buffer, "%s/Shortcut Profiles", rootDir);
    //archCreateDirectory(buffer);
    //shortcutsSetDirectory(buffer);

    sprintf(buffer, "%s/Machines", rootDir);
    //archCreateDirectory(buffer);
    machineSetDirectory(buffer);

    //sprintf(buffer, "%s/Media", rootDir);
    //archCreateDirectory(buffer);
}

int flagdisplay=0;
void handleEvent(int event)
{
    switch(event)
    {
        case EVENT_UPDATE_DISPLAY:
            if (displayUpdates++ == 0) {
                //piMouseInit();
                //piMouseSetCaptureRect(0, 0, screenWidth, screenHeight);
                
                // Prevent joystick sticking
                inputEventReset();
            }
            //flagdisplay=0;
            orbisUpdateEmuDisplay();
            //orbisDebugDisplay();
            archEventSet(dpyUpdateAckEvent);
            pendingDisplayEvents--;
            flagdisplay=EVENT_UPDATE_DISPLAY;
            break;
        default:
            break;
    }    
}

int archUpdateEmuDisplay(int syncMode)
{
   // SDL_Event event;
    //debugNetPrintf(INFO,"[MSXORBIS] %s pendingDisplayEvents %d syncMode %d\n",__FUNCTION__,pendingDisplayEvents,syncMode);

    if (pendingDisplayEvents > 1) {
            
        return 1;
    }

    pendingDisplayEvents++;
    //flagdisplay=1;
    flagdisplay=EVENT_UPDATE_DISPLAY;

   /*event.type = SDL_USEREVENT;
    event.user.code = EVENT_UPDATE_DISPLAY;
    event.user.data1 = NULL;
    event.user.data2 = NULL;

    SDL_PushEvent(&event);*/
    if (properties->emulation.syncMethod == P_EMU_SYNCFRAMES) {
      //  debugNetPrintf(INFO,"[MSXORBIS] %s properties->emulation.syncMethod %d pendingDisplayEvents %d\n",__FUNCTION__,properties->emulation.syncMethod,pendingDisplayEvents);

      archEventWait(dpyUpdateAckEvent, 500);
    }
    handleEvent(flagdisplay);
    return 1;
}
void archUpdateWindow()
{
}

void archEmulationStartNotification()
{
}

void archEmulationStopNotification()
{
}

void archEmulationStartFailure()
{
}

void archTrap(uint8_t value)
{
}

void archQuit()
{
    doQuit = 1;
    //system("aconnect -x");
}

void textureRegister()
{
    programTexture=orbisGlTextureManagerInit();
    debugNetPrintf(DEBUG,"[MSXORBIS] %s linked shader program_id=%d (0x%08x)\n",__FUNCTION__,programTexture->programId, programTexture->programId);

    if(programTexture!=NULL)
    {
        browserTexture=orbisGlInitTexture("browser",BROWSER_BACKGROUND_FILE_PATH,programTexture,ATTR_ORBISGL_WIDTH,ATTR_ORBISGL_HEIGHT);
        //folderTexture=orbisGlInitTexture("folder_icon",FOLDER_ICON_PATH,programTexture,ATTR_ORBISGL_WIDTH,ATTR_ORBISGL_HEIGHT);
        //fileTexture=orbisGlInitTexture("file_icon",FILE_ICON_PATH,programTexture,ATTR_ORBISGL_WIDTH,ATTR_ORBISGL_HEIGHT);
        //settingsTexture=orbisGlInitTexture("settings",SETTINGS_BACKGROUND_FILE_PATH,programTexture,ATTR_ORBISGL_WIDTH,ATTR_ORBISGL_HEIGHT);
        //creditsTexture=orbisGlInitTexture("credits",CREDITS_BACKGROUND_FILE_PATH,programTexture,ATTR_ORBISGL_WIDTH,ATTR_ORBISGL_HEIGHT);

    }
    orbisGlTextInit();
    debugNetPrintf(DEBUG,"[MSXORBIS] %s registered\n",__FUNCTION__);

    
}

int main(int argc, char *argv[])
{
    int ret;
    int i;
    char szLine[8192] = "";
    uintptr_t intptr=0;
    sscanf(argv[1],"%p",&intptr);
    myConf=(OrbisGlobalConf *)intptr;
    ret=ps4LinkInitWithConf(myConf->confLink);
    if(!ret)
    {
        ps4LinkFinish();
        return 0;
    }
    debugNetPrintf(INFO,"[MSXORBIS] fucking yeah!!!\n");
    sleep(1);

    // init libraries
    flag=initApp();

    textureRegister();
    debugNetPrintf(INFO,"[ORBIS_GL] %d\n",screenStatus);

   
    //Mod_Init(0);
    //ret = Mod_Load("host0:main.mod");
    //if(ret)
      //  Mod_Play();
    debugNetPrintf(INFO,"[MSXORBIS] %s before orbisAudioPause\n",__FUNCTION__);
    orbisAudioPause(0);
    //orbisAudioResume(0);
    debugNetPrintf(INFO,"[MSXORBIS] %s before msxOrbisSetDefaultDirectories\n",__FUNCTION__);

    msxOrbisSetDefaultDirectories("host0:app/msxorbis");
    debugNetPrintf(INFO,"[MSXORBIS] %s after msxOrbisSetDefaultDirectories\n",__FUNCTION__);

    properties = propCreate(0, 0, P_KBD_EUROPEAN, 0, "");
    if (!orbisInitVideo()) 
    {
        debugNetPrintf(ERROR,"[MSXORBIS] %s orbisInitVideo() failed\n",__FUNCTION__);
        goto err;
    }
    video = videoCreate();
    videoSetPalMode(video, VIDEO_PAL_FAST);
    videoSetColors(video, properties->video.saturation,
        properties->video.brightness,
        properties->video.contrast, properties->video.gamma);
    videoSetScanLines(video, properties->video.scanlinesEnable,
        properties->video.scanlinesPct);
    videoSetColorSaturation(video, properties->video.colorSaturationEnable,
        properties->video.colorSaturationWidth);
    dpyUpdateAckEvent = archEventCreate(0);
    keyboardInit(properties);

    // Larger buffers cause sound delay
    properties->sound.bufSize = 40;

    mixer = mixerCreate();
    for (i = 0; i < MIXER_CHANNEL_TYPE_COUNT; i++) {
        mixerSetChannelTypeVolume(mixer, i,
            properties->sound.mixerChannel[i].volume);
        mixerSetChannelTypePan(mixer, i,
            properties->sound.mixerChannel[i].pan);
        mixerEnableChannelType(mixer, i,
            properties->sound.mixerChannel[i].enable);
    }
    mixerSetMasterVolume(mixer, properties->sound.masterVolume);
    mixerEnableMaster(mixer, properties->sound.masterEnable);

    emulatorInit(properties, mixer);
    actionInit(video, properties, mixer);
    tapeSetReadOnly(properties->cassette.readOnly);

    langInit();
    langSetLanguage(properties->language);

    joystickPortSetType(0, properties->joy1.typeId);
    joystickPortSetType(1, properties->joy2.typeId);

    printerIoSetType(properties->ports.Lpt.type,
        properties->ports.Lpt.fileName);
    printerIoSetType(properties->ports.Lpt.type,
        properties->ports.Lpt.fileName);
    uartIoSetType(properties->ports.Com.type,
        properties->ports.Com.fileName);
    midiIoSetMidiOutType(properties->sound.MidiOut.type,
        properties->sound.MidiOut.fileName);
    midiIoSetMidiInType(properties->sound.MidiIn.type,
        properties->sound.MidiIn.fileName);
    ykIoSetMidiInType(properties->sound.YkIn.type,
        properties->sound.YkIn.fileName);

    emulatorRestartSound();

    videoUpdateAll(video, properties);

    //shortcuts = shortcutsCreate();

    mediaDbSetDefaultRomType(properties->cartridge.defaultType);

    for (i = 0; i < PROP_MAX_CARTS; i++) {
        if (properties->media.carts[i].fileName[0]) {
            insertCartridge(properties, i,
                properties->media.carts[i].fileName,
                properties->media.carts[i].fileNameInZip,
                properties->media.carts[i].type, -1);
        }
        updateExtendedRomName(i,
            properties->media.carts[i].fileName,
            properties->media.carts[i].fileNameInZip);
    }
    //strcpy(properties->media.carts[0].fileName, "host0:app/GRADIUS2.ROM");
   // strcpy(properties->media.carts[0].fileName, "host0:app/msxorbis/GAMES/nightk.rom");
   // MediaType *it=mediaDbLookup(romdb,"af567ea6e27912d6d5bf1c8e9bc18e9b393fd1ab",0);
    //MediaType *it=mediaDbLookup(romdb,"2ae97f3c8152f56695c3d3a2dedaa781bf0a15f1",0);

   // if(it!=NULL)
    //{
        //properties->media.carts[0].type = ROM_UNKNOWN;//it->romType;
    //}
    insertCartridge(properties, 0,
                properties->media.carts[0].fileName,
                properties->media.carts[0].fileNameInZip,
                properties->media.carts[0].type, -1);

    //strcpy(properties->media.disks[18].fileName, "host0:app/msxorbis/msxorbishdd.dsk");

    for (i = 0; i < PROP_MAX_DISKS; i++) {
        if (properties->media.disks[i].fileName[0]) {
            insertDiskette(properties, i,
                properties->media.disks[i].fileName,
                properties->media.disks[i].fileNameInZip, -1);
        }
        updateExtendedDiskName(i,
            properties->media.disks[i].fileName,
            properties->media.disks[i].fileNameInZip);
    }
    

    for (i = 0; i < PROP_MAX_TAPES; i++) {
        if (properties->media.tapes[i].fileName[0]) {
            insertCassette(properties, i,
                properties->media.tapes[i].fileName,
                properties->media.tapes[i].fileNameInZip, 0);
        }
        updateExtendedCasName(i,
            properties->media.tapes[i].fileName,
            properties->media.tapes[i].fileNameInZip);
    }
    {
        Machine* machine = machineCreate(properties->emulation.machineName);
        if (machine != NULL) 
        {
		    debugNetPrintf(DEBUG,"[MSXORBIS] %s  machine is not null \n",__FUNCTION__);
			
            boardSetMachine(machine);
            machineDestroy(machine);
        } 
        else 
        {
            debugNetPrintf(ERROR,"[MSXORBIS] %s Error creating machine\n",__FUNCTION__);
            orbisDestroyVideo();
            goto err;
        }
    }

    boardSetFdcTimingEnable(properties->emulation.enableFdcTiming);
    boardSetY8950Enable(properties->sound.chip.enableY8950);
    boardSetYm2413Enable(properties->sound.chip.enableYM2413);
    boardSetMoonsoundEnable(properties->sound.chip.enableMoonsound);
    boardSetVideoAutodetect(properties->video.detectActiveMonitor);
	
    /*emulatorSuspend();
	
    strcpy(properties->media.disks[2].fileName, "host0:app/msxorbis/msxorbishdd.dsk");
    insertDiskette(properties, 2,
        properties->media.disks[2].fileName,
        properties->media.disks[2].fileNameInZip, -1);
     updateExtendedDiskName(2,
            properties->media.disks[2].fileName,
            properties->media.disks[2].fileNameInZip);
	emulatorResume();*/
	//strcpy(szLine,"/ide1primary host0:app/msxorbis/msxorbishdd.dsk");
    i = emuTryStartWithArguments(properties, szLine, NULL);
    debugNetPrintf(DEBUG,"[MSXORBIS] %s  emuTryStartWithArguments return %d \n",__FUNCTION__,i);

    if (i < 0) 
    {
        debugNetPrintf(ERROR,"[MSXORBIS] %s Failed %d to parse command line\n",__FUNCTION__,i);
        

        goto err;
    }
    //emulatorSuspend();
  
   // emulatorResume();
    if (i == 0) 
    {
        emulatorStart(NULL);
    }
  //insertDiskette(properties, diskGetHdDriveId(0, 0), "host0:app/msxorbis/msxorbishdd.dsk", NULL, -1);
   // boardChangeDiskette(diskGetHdDriveId(0, 0), "host0:app/msxorbis/msxorbishdd.dsk", NULL);
  
    keyboardInit(NULL);
	keyboardUpdate();
	
   //  inputEventSet(EC_Y);
   //     inputEventSet(EC_P);

    debugNetPrintf(DEBUG,"[MSXORBIS] %s Powering on\n",__FUNCTION__);

    while(flag)
    {
       // updateController();
      //  keyboardUpdate();

        //if (displayUpdates++ == 0) 
        //{
               // piMouseInit();
               // piMouseSetCaptureRect(0, 0, screenWidth, screenHeight);
                                //keyboardInit();

                // Prevent joystick sticking
          //      inputEventReset();
        //}
        if(flagdisplay)
        {
            if (displayUpdates++ == 0) {
                //piMouseInit();
                //piMouseSetCaptureRect(0, 0, screenWidth, screenHeight);
                
                // Prevent joystick sticking
                //inputEventReset();
            }
            //flagdisplay=0;
            //handleEvent(flagdisplay);
          //  flagdisplay=0;
            
           // orbisUpdateEmuDisplay();
           archEventSet(dpyUpdateAckEvent);
            
            if(screenStatus==SCREEN_BROWSER)
            {
                showBrowser();
                emulatorResume();

                //ret=0;
            }
            //pendingDisplayEvents--;
        } 
    }



    err:
    videoDestroy(video);
    propDestroy(properties);
    archSoundDestroy();
    mixerDestroy(mixer);
    orbisDestroyVideo();
    orbisAudioPause(0);
    //Mod_End();

    // finish libraries
    finishApp();

    exit(EXIT_SUCCESS);
}



