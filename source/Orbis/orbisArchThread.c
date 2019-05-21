/*****************************************************************************
** $Source: /cygdrive/d/Private/_SVNROOT/bluemsx/blueMSX/Src/Sdl/SdlThread.c,v $
**
** $Revision: 1.5 $
**
** $Date: 2007-03-21 22:26:25 $
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
#include "ArchThread.h"
#include <kernel.h>
#include <stdlib.h>
#include <debugnet.h>

void *threadEntry(void* data) 
{
    void (*entryPoint)() = data;

    entryPoint();
    
    return NULL;
}
ScePthread new_thid; 

void* archThreadCreate(void (*entryPoint)(), int priority)
{ 

    ScePthread thid; 
    int ret;
    thid = scePthreadSelf();
    if (scePthreadGetprio(thid, &priority) == 0 && priority>1)
    {
        ret=scePthreadCreate(&new_thid, NULL, threadEntry, entryPoint, "emuthread");
        if(ret==0)
        {
            debugNetPrintf(DEBUG,"[ORBISMSX] emulator thread UID: 0x%08X priority %d\n", new_thid,priority);
            scePthreadSetprio(new_thid, priority-1);
            return &new_thid;
        }   
    }
    return NULL;
}

void archThreadJoin(void* thread, int timeout) 
{
    ScePthread *thid = (ScePthread *)thread;

    scePthreadJoin(*thid, NULL);
}

void  archThreadDestroy(void* thread) 
{
    ScePthread *thid = (ScePthread *)thread;

    scePthreadCancel(*thid);
}

void archThreadSleep(int milliseconds) 
{
    const uint32_t max_delay = 0xffffffffUL / 1000;
    if(milliseconds > max_delay)
        milliseconds = max_delay;
    sceKernelUsleep(milliseconds * 1000);
}
