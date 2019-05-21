/*****************************************************************************
** $Source: /cygdrive/d/Private/_SVNROOT/bluemsx/blueMSX/Src/Sdl/SdlFile.c,v $
**
** $Revision: 1.5 $
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
#include "ArchFile.h"
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <orbisFile.h>
#include <debugnet.h>
#include <sys/fcntl.h>

int archCreateDirectory(const char* pathname)
{
    int ret=orbisMkdir(pathname, 0775);
    if(ret!=0)
    {
    	return -1;
    }
    return 0;
}

const char* archGetCurrentDirectory()
{
    static char buf[512];
   // return getcwd(buf, sizeof(buf));
    return "host0:app/orbismsx";
}

void archSetCurrentDirectory(const char* pathname)
{
    //chdir(pathname);
}

int archFileExists(const char* fileName)
{
    int fd;
    if(!fileName)
    {
    	return 0;
    }
    fd=orbisOpen(fileName,O_RDONLY,0);
    if(fd<=0)
    {
    	return 0;
    }
    else
    {
    	orbisClose(fd);
    	return 1;
    }
}

int archFileDelete(const char* fileName)
{
   // return remove(fileName) == 0;
	return 1;
}

#define n_array sizeof(array)/sizeof(const char *)
/* Compare the strings. */

static int compare (const void * a, const void * b)
{
    /* The pointers point to offsets into "array", so we need to
       dereference them to get at the strings. */

    return strcmp (*(const char **) a, *(const char **) b);
}

/* File dialogs: */
char* archFilenameGetOpenRom(Properties* properties, int cartSlot, RomType* romType) { return NULL; }
char* archFilenameGetOpenDisk(Properties* properties, int drive, int allowCreate) { debugNetPrintf(DEBUG,"[MSXORBIS] %s %d  \n",__FUNCTION__,drive);
if(drive==18)
{
return "host0:app/msxorbis/msxorbishdd.dsk"; 

} 
}
char* archFilenameGetOpenHarddisk(Properties* properties, int drive, int allowCreate) {             debugNetPrintf(DEBUG,"[MSXORBIS] %s %d  \n",__FUNCTION__,drive);

return "host0:app/msxorbis/msxorbishdd.dsk"; }
char* archFilenameGetOpenCas(Properties* properties) { return NULL; }
char* archFilenameGetSaveCas(Properties* properties, int* type) { return NULL; }
char* archFilenameGetOpenState(Properties* properties) { return NULL; }
char* archFilenameGetOpenCapture(Properties* properties) { return NULL; }
char* archFilenameGetSaveState(Properties* properties) { return NULL; }
char* archDirnameGetOpenDisk(Properties* properties, int drive) { return NULL; }
char* archFilenameGetOpenRomZip(Properties* properties, int cartSlot, const char* fname, const char* fileList, int count, int* autostart, int* romType) { return NULL; }
char* archFilenameGetOpenDiskZip(Properties* properties, int drive, const char* fname, const char* fileList, int count, int* autostart) {return NULL;}
char* archFilenameGetOpenCasZip(Properties* properties, const char* fname, const char* fileList, int count, int* autostart) { return NULL; }
char* archFilenameGetOpenAnyZip(Properties* properties, const char* fname, const char* fileList, int count, int* autostart, int* romType) { return NULL; }

