/*****************************************************************************
** $Source: /cygdrive/d/Private/_SVNROOT/bluemsx/blueMSX/Src/Sdl/SdlMouse.c,v $
**
** $Revision: 1.9 $
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

#include "MsxTypes.h"
#include "ArchInput.h"




static char* arrow[] = {
    "X                               ",
    "XX                              ",
    "X.X                             ",
    "X..X                            ",
    "X...X                           ",
    "X....X                          ",
    "X.....X                         ",
    "X......X                        ",
    "X.......X                       ",
    "X........X                      ",
    "X.........X                     ",
    "X..... XXXXX                    ",
    "X..X...X                        ",
    "X.X X...X                       ",
    "XX  X...X                       ",
    "X    X...X                      ",
    "     X...X                      ",
    "      X...X                     ",
    "      X...X                     ",
    "       XXX                      ",
    "                                ",
    "                                ",
    "                                ",
    "                                ",
    "                                ",
    "                                ",
    "                                ",
    "                                ",
    "                                ",
    "                                ",
    "                                ",
    "                                "
};

static char* cross[] = {
    "             XXXXX              ",
    "            X.....X             ",
    "            X.....X             ",
    "            X.....X             ",
    "            X.....X             ",
    "             X...X              ",
    "             X...X              ",
    "             X...X              ",
    "              X.X               ",
    "              X.X               ",
    "              X.X               ",
    "               X                ",
    " XXXX          X          XXXX  ",
    "X....XXX               XXX....X ",
    "X.......XXX    X    XXX.......X ",
    "X..........XX X X XX..........X ",
    "X.......XXX    X    XXX.......X ",
    "X....XXX               XXX....X ",
    " XXXX          X          XXXX  ",
    "               X                ",
    "              X.X               ",
    "              X.X               ",
    "              X.X               ",
    "             X...X              ",
    "             X...X              ",
    "             X...X              ",
    "            X.....X             ",
    "            X.....X             ",
    "            X.....X             ",
    "            X.....X             ",
    "             XXXXX              ",
    "                                "
};
 


void archMouseGetState(int* dx, int* dy) 
{ 
   
}

int  archMouseGetButtonState(int checkAlways) 
{ 
    
        return 0;
    
}

void  archMouseEmuEnable(AmEnableMode mode) 
{ 
   //mouse.mode = mode;
}
