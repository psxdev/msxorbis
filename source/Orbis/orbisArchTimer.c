/*****************************************************************************
** $Source: /cygdrive/d/Private/_SVNROOT/bluemsx/blueMSX/Src/Sdl/SdlTimer.c,v $
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
#include "ArchTimer.h"
#include "ArchThread.h"
#include <stdlib.h>
#include <kernel.h>
#include <stdint.h>
#include <stdbool.h>
static uint64_t start;
static bool ticks_started = false;
void orbisTicksInit(void)
{
    if (ticks_started) 
    {
        return;
    }
    ticks_started = true;
    start = sceKernelGetProcessTime();
}
void orbisTicksQuit(void)
{
    ticks_started = false;
}

uint32_t orbisGetTicks(void)
{
    if (!ticks_started) {
        orbisTicksInit();
    }

    uint64_t now;
    uint32_t ticks;

    now = sceKernelGetProcessTime();
    ticks = (uint32_t)(now - start)/1000;
    return (ticks);
}


#ifdef NO_TIMERS

// Timer callbacks are not needed. When disabled, there is no need for
// archEvents either.

void* archCreateTimer(int period, int (*timerCallback)(void*)) { return NULL; }
void archTimerDestroy(void* timer) {}


// The only timer that is required is a high res timer. The resolution is
// not super important, the higher the better, but one tick every 10ms is
// good enough. The frequency argument is in Hz and is 1000 or less.
uint32_t archGetSystemUpTime(UInt32 frequency) 
{
    return orbisGetTicks() / (1000 / frequency);
}

// This is just a timer value with a frequency as high as possible. 
// The frequency of the timer is not important.
uint32_t archGetHiresTimer() 
{
    return orbisGetTicks();
}

#else 

static void (*timerCb)(void*) = NULL;
static uint32_t timerFreq;
static uint32_t lastTimeout;
ScePthread timerThid; 

uint32_t timerCalback(uint32_t interval)
{
    if (timerCb) 
    {
        uint32_t currentTime = archGetSystemUpTime(timerFreq);

        while (lastTimeout != currentTime) 
        {
            lastTimeout = currentTime;
            timerCb(timerCb);
        }
    }
    return interval;
}
static void*  emuTimerThread;
static void timerThread() 
{
    uint64_t ticks;
    uint64_t now;
    while(1)
    {
        now=sceKernelGetProcessTime()/1000;
        ticks=10+now;
        do
        {
            now=sceKernelGetProcessTime()/1000;
        }
        while(now<ticks);
        timerCalback(10);
    }
}
void* archCreateTimer(int period, int (*timerCallback)(void*)) 
{ 
    timerFreq = 1000 / period;
    lastTimeout = archGetSystemUpTime(timerFreq);
    debugNetPrintf(3,"[MSXORBIS] %s  lastTimeout %d  timerFreq %d \n",__FUNCTION__,lastTimeout,timerFreq);

    timerCb  = timerCallback;

   // SDL_SetTimer(period, timerCalback);
    emuTimerThread = archThreadCreate(timerThread, THREAD_PRIO_HIGH);


    return timerCallback;
}

void archTimerDestroy(void* timer) 
{
    if (timerCb != timer) {
        return;
    }

  //  SDL_SetTimer(0, NULL);
    archThreadDestroy(emuTimerThread);
    timerCb = NULL;

}

uint32_t archGetSystemUpTime(UInt32 frequency) 
{
    return orbisGetTicks() / (1000 / frequency);
}

uint32_t archGetHiresTimer() 
{
    return orbisGetTicks();
}

#endif