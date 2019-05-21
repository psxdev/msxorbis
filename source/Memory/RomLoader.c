/*****************************************************************************
** $Source: /cygdrive/d/Private/_SVNROOT/bluemsx/blueMSX/Src/Memory/RomLoader.c,v $
**
** $Revision: 1.6 $
**
** $Date: 2008-03-30 18:38:42 $
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
#include "RomLoader.h"
#include "ziphelper.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <debugnet.h>
#include <orbisFile.h>
#include <sys/fcntl.h>
// PacketFileSystem.h Need to be included after all other includes
#include "PacketFileSystem.h"


UInt8* romLoad(const char *fileName, const char *fileInZipFile, int* size)
{
    UInt8* buf = NULL;
    int file;

    if (fileName == NULL || strlen(fileName) == 0) {
        return NULL;
    }

    if (fileInZipFile != NULL && strlen(fileInZipFile) == 0) {
        fileInZipFile = NULL;
    }

    if (fileInZipFile != NULL) {
        buf = zipLoadFile(fileName, fileInZipFile, size);
        return buf;
    }

    file = orbisOpen(fileName, O_RDONLY, 0);
    if (file <=0) {
        return NULL;
    }

    *size=orbisLseek(file, 0, SEEK_END);
    if (*size <= 0) {
        orbisClose(file);
        return malloc(1);
    }

    orbisLseek(file, 0, SEEK_SET);

    buf = malloc(*size);
    if(buf==NULL)
    {
        debugNetPrintf(ERROR,"[MSXORBIS] %s error malloc size=%d too high for malloc \n",__FUNCTION__,*size);
        buf=mmap(NULL,*size,0x01|0x02,0x1000|0x0002,-1,0);
        debugNetPrintf(DEBUG,"[MSXORBIS] %s trying mmap hard way fix size=%d buf=%x\n",__FUNCTION__,*size,buf);

    }
    debugNetPrintf(DEBUG,"[MSXORBIS] %s trying mmap hard way fix size=%d buf=%x\n",__FUNCTION__,*size,buf);

    debugNetPrintf(DEBUG,"[MSXORBIS] %s readed size=%d\n",__FUNCTION__,orbisRead(file, buf, *size));

    orbisClose(file);

    return buf;
}

