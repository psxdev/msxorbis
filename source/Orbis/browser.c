/*
 * msxorbis for PlayStation 4 
 * Copyright (C) 2017 Antonio Jose Ramos Marquez (aka bigboss) @psxdev on twitter
 * Repository https://github.com/psxdev/msxorbis
 */

#include <orbisPad.h>
#include <orbis2d.h>
#include <orbisFileBrowser.h>
#include <ps4link.h>
#include <debugnet.h>
#include <freetype-gl.h>

//#include "menu.h"
#include "browser.h"
#include "orbisGlTextureManager.h"


int notSelected=1;
int comeBack=0;
int displayNumber=0;
int extension;
int slot=0;
int drive=0;
char path[256];
int posy=0;
int posx=0;
vec4 color;
extern OrbisGlTextureState *browserTexture;
extern OrbisGlTextureState *folderTexture;
extern OrbisGlTextureState *fileTexture;
extern int screenStatus;


int flagfolder=0;
OrbisFileBrowserListEntry *currentEntry;

void browserUpdateController()
{
	int ret;
	unsigned int buttons=0;
	OrbisFileBrowserListEntry *entry;
	ret=orbisPadUpdate();
	if(ret==0)
	{
		if(orbisPadGetButtonPressed(ORBISPAD_UP) || orbisPadGetButtonHold(ORBISPAD_UP))
		{
			debugNetPrintf(DEBUG,"Up pressed\n");
			orbisFileBrowserEntryUp();
			currentEntry=orbisFileBrowserListGetNthEntry(orbisFileBrowserGetBasePos()+orbisFileBrowserGetRelPos());
			if(currentEntry!=NULL)
			{
				debugNetPrintf(INFO,"current entry %s\n",currentEntry->dir->name);
			}
		}
		if(orbisPadGetButtonPressed(ORBISPAD_DOWN)|| orbisPadGetButtonHold(ORBISPAD_DOWN))
		{
			
			debugNetPrintf(DEBUG,"Down pressed\n");
			debugNetPrintf(DEBUG," before entry down level=%d base=%d rel=%d\n",orbisFileBrowserGetDirLevel(),orbisFileBrowserGetBasePos(),orbisFileBrowserGetRelPos());
			
			orbisFileBrowserEntryDown();
			debugNetPrintf(DEBUG," after entry down level=%d base=%d rel=%d\n",orbisFileBrowserGetDirLevel(),orbisFileBrowserGetBasePos(),orbisFileBrowserGetRelPos());
			
			currentEntry=orbisFileBrowserListGetNthEntry(orbisFileBrowserGetBasePos()+orbisFileBrowserGetRelPos());
			if(currentEntry!=NULL)
			{
				debugNetPrintf(INFO,"current entry %s\n",currentEntry->dir->name);
			}
			else
			{
				debugNetPrintf(INFO,"current entry chunga\n");
				
			}
		}
		if(orbisPadGetButtonPressed(ORBISPAD_R1))
		{
						posy++;

			debugNetPrintf(DEBUG,"r1 pressed %d\n",posy);
		}
		if(orbisPadGetButtonPressed(ORBISPAD_L1))
		{
						posy--;

			debugNetPrintf(DEBUG,"r1 pressed %d\n",posy);
		}	
		if(orbisPadGetButtonPressed(ORBISPAD_R2))
		{
						posx++;

			debugNetPrintf(DEBUG,"r1 pressed %d\n",posx);
		}
		if(orbisPadGetButtonPressed(ORBISPAD_L2))
		{
						posx--;

			debugNetPrintf(DEBUG,"r1 pressed %d\n",posx);
		}						
		if(orbisPadGetButtonPressed(ORBISPAD_CIRCLE))
		{
			debugNetPrintf(DEBUG,"Circle pressed come back to msx\n");
			comeBack=1;
		}
		if(orbisPadGetButtonPressed(ORBISPAD_TRIANGLE))
 
		{
			debugNetPrintf(DEBUG,"Triangle pressed posy=%d\n",++posy);
			screenStatus=SCREEN_CREDITS;

			showCredits();
		}
		if(orbisPadGetButtonPressed(ORBISPAD_SQUARE))
		{
			debugNetPrintf(DEBUG,"Square pressed\n");
		}
		if(orbisPadGetButtonPressed(ORBISPAD_CROSS))
		{
			debugNetPrintf(DEBUG,"Cross pressed\n");
			notSelected=0;
			debugNetPrintf(DEBUG,"cross level=%d base=%d rel=%d\n",orbisFileBrowserGetDirLevel(),orbisFileBrowserGetBasePos(),orbisFileBrowserGetRelPos());
			
			//entry=orbisFileBrowserListGetNthEntry(orbisFileBrowserGetBasePos()+orbisFileBrowserGetRelPos());					
			currentEntry=orbisFileBrowserListGetNthEntry(orbisFileBrowserGetBasePos()+orbisFileBrowserGetRelPos());

			if(currentEntry!=NULL)
			{
				
				debugNetPrintf(INFO,"cross current entry %s customtype=%d\n",currentEntry->dir->name,currentEntry->dir->customtype);
				
				switch(currentEntry->dir->customtype)
				{	
					case FILE_TYPE_FOLDER:
						notSelected=1;
						//if(strcmp(currentEntry->dir->name, ".") == 0)
						//{
						//	flagfolder=1;
						//}
						//else
						//{
							//if(strcmp(currentEntry->dir->name, ".")!=0)
							//{
								debugNetPrintf(DEBUG,"cross selected folder level=%d base=%d rel=%d\n",orbisFileBrowserGetDirLevel(),orbisFileBrowserGetBasePos(),orbisFileBrowserGetRelPos());
								
								//orbisFileBrowserDirLevelUp(currentEntry->dir->name);
								//debugNetPrintf(DEBUG,"cross selected folder level=%d base=%d rel=%d\n",orbisFileBrowserGetDirLevel(),orbisFileBrowserGetBasePos(),orbisFileBrowserGetRelPos());
								flagfolder=1;
								//}
						//}
						break;
					case FILE_TYPE_GAME_ROM:
						
						//if(extension==FILE_TYPE_GAME_ROM)
						//{
							sprintf(path,"%s/%s",orbisFileBrowserGetListPath(),currentEntry->dir->name);
							debugNetPrintf(DEBUG,"cross selected entry game %s\n",path);
							
							debugNetPrintf(DEBUG,"change cart\n");
							actionCartInsertFromHost(path,0);

							screenStatus=SCREEN_EMU;
							//LoadCart(path,slot,MAP_GUESS);
							//}
						//else
						//{
						//	debugNetPrintf(INFO,"wrong extension choose the right one\n");
							//}
						break;
					case FILE_TYPE_GAME_DSK:
						//if(extension==FILE_TYPE_GAME_DSK)
						//{
						
							sprintf(path,"%s/%s",orbisFileBrowserGetListPath(),currentEntry->dir->name);
							//LoadFileDrive(path,drive);
							debugNetPrintf(DEBUG,"change disk\n");
							actionDiskInsertFromHost(path,0);
							screenStatus=SCREEN_EMU;

							//}
						//else
						//{
						//	debugNetPrintf(INFO,"wrong extension choose the right one\n");
							//}
						break;
					case FILE_TYPE_CAS:
					//	if(extension==FILE_TYPE_CAS)
					//	{
							sprintf(path,"%s/%s",orbisFileBrowserGetListPath(),currentEntry->dir->name);
							//ChangeTape(path);
							debugNetPrintf(DEBUG,"change cas\n");
							
					//	}
					//	else
					//	{
					//		debugNetPrintf(INFO,"wrong extension choose the right one\n");
					//	}
						break;
					default:
						debugNetPrintf(DEBUG,"wrong extension come back\n");
						comeBack=1;
						break;
				}
			}
		}	
	}
}
void getSizeString(char string[16], uint64_t size) 
{
	double double_size = (double)size;

	int i = 0;
	static char *units[] = { "B", "KB", "MB", "GB", "TB", "PB", "EB", "ZB", "YB" };
	while (double_size >= 1024.0) {
		double_size /= 1024.0;
		i++;
	}

	snprintf(string, 16, "%.*f %s", (i == 0) ? 0 : 2, double_size, units[i]);
}
void browserDrawText()
{
	vec4 color;
	color.r=255.0;
	color.g=255.0;
	color.b=255.0;
	color.a=1.0;

	orbisGlTextDraw("Browser",30+posx,52+15+posy,color);
	orbisGlTextDraw("Back",90-22+posx,1000+34+posy,color);
	orbisGlTextDraw("Browser",100+90-22+64,1000+34+posy,color);
	orbisGlTextDraw("Credits",220+90-22+64+79,1000+34+posy,color);
	orbisGlTextDraw("Select",340+90-22+64+79+79,1000+34+posy,color);

}
void browserDraw()
{
	int i=0;
	uint32_t f1 ,f2;
	char dateString[20]; 
	char sizeString[16];
	browserDrawText();
	if(flagfolder==1 && currentEntry)
	{
		if(strcmp(currentEntry->dir->name,".")!=0)
		{
			debugNetPrintf(DEBUG,"go to new directory %s\n",currentEntry->dir->name);
			//char rootpath[256];
			//sprintf(rootpath,"%s/%s",orbisFileBrowserGetListPath(),currentEntry->dir->name);
			//debugNetPrintf(DEBUG,"go to new directory %s\n",rootpath);
			if(strcmp(currentEntry->dir->name,"..")!=0)
			{
			orbisFileBrowserDirLevelUp(currentEntry->dir->name);
			}
			else
			{
				orbisFileBrowserDirLevelDown();
			}
			debugNetPrintf(DEBUG,"after orbisFileBrowserDirLevelUp\n");
		}
		else
		{
			orbisFileBrowserListRefresh();
		}
		flagfolder=0;
		
	}
	OrbisFileBrowserListEntry *entry=orbisFileBrowserListGetNthEntry(orbisFileBrowserGetBasePos());
	
	
		
		while(entry && i<MAX_ENTRIES)
		{
			
			if(entry->dir->customtype==FILE_TYPE_FOLDER)
			{
				if(folderTexture)
				{
					//orbisGlDraw(programTextureId,folderTextureId,30,90+i*20);
					orbisGlDrawTextureSpecial(folderTexture,30,90+i*30+1+67);
	
				}
				sprintf(sizeString,"%s","FOLDER");
				
			}
			else
			{
				if(fileTexture)
				{
					//orbisGlDraw(programTextureId,fileTextureId,30,90+i*20+2);	
					orbisGlDrawTextureSpecial(folderTexture,30,90+i*30+1+67);

				}
				getSizeString(sizeString,entry->dir->size);
			}
			//debugNetPrintf(DEBUG,("%s %d\n",entry->name,entry->type);
			if(i==orbisFileBrowserGetRelPos())
			{
				
				/*f1 = 0xFF24FFBD;
				f2 = 0xFF24FFBD;
				update_gradient(&f1, &f2);*/
				color.r=36.0/255.0;
				color.g=1.0;
				color.b=189.0/255.0;
				color.a=1.0;
								
			}
			else
			{
				/*f1 = 0x80FFFFFF;
				f2 = 0x80FFFFFF;
				update_gradient(&f1, &f2);*/
				color.r=1.0;
				color.g=1.0;
				color.b=1.0;
				color.a=1.0;
			}
			//print_text(50+posx,90+i*20+posy,entry->dir->name);
			orbisGlTextDraw(entry->dir->name,60,90+i*30+1+67,color);
		
			sprintf(dateString,"%02d/%02d/%04d %02d:%02d %s",
			entry->dir->mtime.day,
			entry->dir->mtime.month,
			entry->dir->mtime.year,
			entry->dir->mtime.hour>12?entry->dir->mtime.hour-12:entry->dir->mtime.hour,
			entry->dir->mtime.minute,
			entry->dir->mtime.hour>=12? "PM" : "AM");	
			orbisGlTextDraw(dateString,740+960+8-15-35,90+i*30+1+67,color);
			orbisGlTextDraw(sizeString,740+960-8-15-35-17*strlen(sizeString),90+i*30+1+67,color);
				
			entry=entry->next;	
			i++;			
			
		}
		//f1 = 0xFF24FFBD,f2 = 0xFF24FFBD;
		//update_gradient(&f1, &f2);
		color.r=36.0/255.0;
				color.g=1.0;
				color.b=189.0/255.0;
				color.a=1.0;
		orbisGlTextDraw(orbisFileBrowserGetListPath(),30,52+76,color);
}
void showBrowser()
{
	//orbisAudioPause(0);
	
	while(notSelected==1 && comeBack==0 && screenStatus!=SCREEN_EMU)
	{
				//debugNetPrintf(3,"jur1\n");

		browserUpdateController();
			//	debugNetPrintf(3,"jur2\n");
		if(browserTexture!=NULL)
		{
		
        //orbisGlTextureAdd(browserTexture,0,0,1920,1080);
        
			orbisGlDrawTexture(browserTexture);
		}
		if(screenStatus==SCREEN_BROWSER)
		browserDraw();	
			//debugNetPrintf(3,"jur3\n");

		orbisGlSwapBuffers();
			//	debugNetPrintf(3,"jur4\n");

	}
	notSelected=1;
	comeBack=0;
	//msx go go
	screenStatus=SCREEN_EMU;
}

    
