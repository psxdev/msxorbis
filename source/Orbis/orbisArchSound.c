/*****************************************************************************
** $Source: /cygdrive/d/Private/_SVNROOT/bluemsx/blueMSX/Src/Sdl/SdlSound.c,v $
**
** $Revision: 1.6 $
**
** $Date: 2008-03-30 18:38:45 $
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
#include "ArchSound.h"
#include <orbisAudio.h>
#include <debugnet.h>
#include <stdlib.h>

typedef struct OrbisSound {
    Mixer* mixer;
    int started;
    UInt32 readPtr;
    UInt32 writePtr;
    UInt32 bytesPerSample;
    UInt32 bufferMask;
    UInt32 bufferSize;
    UInt32 skipCount;
    UInt8 channels;
    UInt8 *buffer;
} OrbisSound;

OrbisSound orbisSound;
int oldLen = 0;
//

void soundCallback(short *buf, unsigned int samples)
{
    UInt32 avail = (orbisSound.readPtr - orbisSound.writePtr);// & sdlSound.bufferMask;
    int size = samples*orbisSound.channels*orbisSound.bytesPerSample;
   // if ((UInt32)samples > avail) {
   //     orbisSound.readPtr = (orbisSound.readPtr - orbisSound.bufferSize / 2) & orbisSound.bufferMask;
   //     memset((UInt8*)buf + avail, 0, samples - avail);
   //     length = avail;
   // }

    memcpy(buf, orbisSound.buffer+orbisSound.readPtr, size);
    orbisSound.readPtr = (orbisSound.readPtr + size) & orbisSound.bufferMask;
}
static void AudioCallback(OrbisAudioSample *buf, unsigned int samples, void *userdata)
{
    soundCallback((short *)buf,samples);
}
static Int32 soundWrite(void* dummy, Int16 *buffer, UInt32 count)
{
   UInt32 avail;

    if (!orbisSound.started) {
        return 0;
    }

    count *= orbisSound.bytesPerSample*orbisSound.channels;

    /*if (orbisSound.skipCount > 0) {
        if (count <= orbisSound.skipCount) {
            orbisSound.skipCount -= count;
            return 0;
        }
        count -= orbisSound.skipCount;
        orbisSound.skipCount = 0;
    }*/

    //SDL_LockAudio(); fix to avoid problem writing in orbis callback

  //  avail = (sdlSound.writePtr - sdlSound.readPtr) & sdlSound.bufferMask;
 //   if (count < avail && 0) {
   //     sdlSound.skipCount = sdlSound.bufferSize / 2;
   // }
   // else {
        if (orbisSound.writePtr + count > orbisSound.bufferSize) {
            UInt32 count1 = orbisSound.bufferSize - orbisSound.writePtr;
            UInt32 count2 = count - count1;
            memcpy(orbisSound.buffer + orbisSound.writePtr, buffer, count1);
            memcpy(orbisSound.buffer, buffer, count2);
            orbisSound.writePtr = count2;
        }
        else {
            memcpy(orbisSound.buffer + orbisSound.writePtr, buffer, count);
            orbisSound.writePtr += count;
        }
    //}

    //SDL_UnlockAudio();
    return 0;
}

void archSoundCreate(Mixer* mixer, UInt32 sampleRate, UInt32 bufferSize, Int16 channels) 
{
	//SDL_AudioSpec desired;
	//SDL_AudioSpec audioSpec;
	UInt16 samples = channels;

    memset(&orbisSound, 0, sizeof(orbisSound));

 /*   bufferSize = bufferSize * sampleRate / 1000 * sizeof(Int16) / 4;

    while (samples < bufferSize) samples *= 2;

	desired.freq     = sampleRate;
	desired.samples  = samples;
	desired.channels = (UInt8)channels;
#ifdef LSB_FIRST
	desired.format   = AUDIO_S16LSB;
#else
    desired.format   = AUDIO_S16MSB;
#endif
	desired.callback = soundCallback;
	desired.userdata = NULL;
    
	if (SDL_InitSubSystem(SDL_INIT_AUDIO) != 0) {
        return;
    }

	char driver_name[1024];
	SDL_AudioDriverName(driver_name, 1024);
	printf("Audio driver: %s\n", driver_name);*/
/*	
	int i;
	for (i = 0; i < SDL_GetNumAudioDrivers(); ++i) {
		const char* driver_name = SDL_GetAudioDriver(i);
		if (SDL_AudioInit(driver_name)) {
			printf("Audio driver failed to initialize: %s\n", driver_name);
			continue;
		}
    }
*/
	/*if (SDL_OpenAudio(&desired, &audioSpec) != 0) {
		SDL_QuitSubSystem(SDL_INIT_AUDIO);
        return;
    }
	
	printf ("freq:%d(%d)\n", desired.freq, audioSpec.freq);
	printf ("samples:%d(%d)\n", desired.samples, audioSpec.samples);
	printf ("channels:%d(%d)\n", desired.channels, audioSpec.channels);
	printf ("format:%d(%d)\n", desired.format, audioSpec.format);
	printf ("size:%d(%d)\n", desired.size, audioSpec.size);
	
    sdlSound.bufferSize = 5;
    while (sdlSound.bufferSize < 4 * audioSpec.size) sdlSound.bufferSize *= 2;*/
	
    orbisSound.bufferSize = 2*1*512* 4;
	debugNetPrintf(3,"[MSXORBIS] %s  orbisSound.bufferSize %d \n",__FUNCTION__,orbisSound.bufferSize);
    orbisSound.bufferMask = orbisSound.bufferSize - 1;
    orbisSound.buffer = (UInt8*)malloc(orbisSound.bufferSize);
    orbisSound.started = 1;
    orbisSound.mixer = mixer;
    orbisSound.bytesPerSample = 2;
    orbisSound.channels = channels;
    if(orbisSound.buffer!=NULL)
    {
         orbisAudioSetCallback(0, AudioCallback, 0);
    }
    
    mixerSetStereo(mixer, channels == 2);
    mixerSetWriteCallback(mixer, soundWrite, NULL, 2*channels*512 / orbisSound.bytesPerSample);
    
	archSoundResume();
}

void archSoundDestroy(void) 
{
   // if (sdlSound.started) {
     //   mixerSetWriteCallback(sdlSound.mixer, NULL, NULL, 0);
	//	SDL_QuitSubSystem(SDL_INIT_AUDIO);
    //}
    orbisSound.started = 0;
    orbisAudioPause(0);

}
void archSoundResume(void) 
{
	orbisAudioResume(0);
}

void archSoundSuspend(void) 
{
	orbisAudioPause(0);
}
