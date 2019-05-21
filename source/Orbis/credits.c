/*
 * msxorbis for PlayStation 4 
 * Copyright (C) 2017 Antonio Jose Ramos Marquez (aka bigboss) @psxdev on twitter
 * Repository https://github.com/psxdev/msxorbis
 */

#include <orbisPad.h>
#include <orbis2d.h>
#include <orbisFile.h>
#include <debugnet.h>
#include <freetype-gl.h>  // links against libfreetype-gl

#include "browser.h"
#include "orbisGlTextureManager.h"
//extern OrbisGlTextureState *creditsTexture;
extern OrbisGlTextureState *browserTexture;

extern int screenStatus;
void creditsUpdateController()
{
	int ret;
	unsigned int buttons=0;
	ret=orbisPadUpdate();
	if(ret==0)
	{
		
		if(orbisPadGetButtonPressed(ORBISPAD_UP))
		{
			debugNetPrintf(DEBUG,"Up pressed\n");
			screenStatus=SCREEN_BROWSER;
		}
		if(orbisPadGetButtonPressed(ORBISPAD_DOWN))
		{
			debugNetPrintf(DEBUG,"Down pressed\n");
			screenStatus=SCREEN_BROWSER;
		}						
		if(orbisPadGetButtonPressed(ORBISPAD_RIGHT))
		{
			debugNetPrintf(DEBUG,"Right pressed\n");
			screenStatus=SCREEN_BROWSER;
		}
		if(orbisPadGetButtonPressed(ORBISPAD_LEFT))
		{
			debugNetPrintf(DEBUG,"Left pressed\n");
			screenStatus=SCREEN_BROWSER;
		}
		if(orbisPadGetButtonPressed(ORBISPAD_TRIANGLE))
		{
			debugNetPrintf(DEBUG,"Triangle pressed exit\n");
			screenStatus=SCREEN_CREDITS;
		}
		if(orbisPadGetButtonPressed(ORBISPAD_CIRCLE))
		{
			debugNetPrintf(DEBUG,"Circle pressed come back to browser\n");
			screenStatus=SCREEN_EMU;
		}
		if(orbisPadGetButtonPressed(ORBISPAD_CROSS))
		{
			debugNetPrintf(DEBUG,"Cross pressed\n");
			screenStatus=SCREEN_CREDITS;
		}
		if(orbisPadGetButtonPressed(ORBISPAD_SQUARE))
		{
			debugNetPrintf(DEBUG,"Square pressed\n");
			screenStatus=SCREEN_BROWSER;
		}
	}
}
void creditsDrawText()
{
	vec4 color;
	color.r=255.0;
	color.g=255.0;
	color.b=255.0;
	color.a=1.0;

	orbisGlTextDraw("Credits",30,52+15,color);
	orbisGlTextDraw("Back",90-22,1000+34,color);
	orbisGlTextDraw("Browser",100+90-22+64,1000+34,color);
	orbisGlTextDraw("Credits",220+90-22+64+79,1000+34,color);
	orbisGlTextDraw("Select",340+90-22+64+79+79,1000+34,color);
	int i=0;
	orbisGlTextDraw("MSXORBIS is a MSX emulator for PlayStation 4",60,90+i*30+1+67,color);i++;
	orbisGlTextDraw("It is based on bluemsx",60,90+i*30+1+67,color);i++;
	orbisGlTextDraw("http://bluemsx.com",60,90+i*30+1+67,color);i++;
	orbisGlTextDraw("Source code of msxorbis is available at:",60,90+i*30+1+67,color);i++;
	orbisGlTextDraw("https://github.com/psxdev/msxorbis",60,90+i*30+1+67,color);i++;
	orbisGlTextDraw("This software have been done using the following open source tools:",60,90+i*30+1+67,color);i++;
	orbisGlTextDraw("- Hitodama's PS4SDK with lasted addons check:",60,90+i*30+1+67,color);i++;
	orbisGlTextDraw("https://github.com/psxdev/ps4sdk/tree/firmware505",60,90+i*30+1+67,color);i++;
	orbisGlTextDraw("- Toolchain based on LLVM/Clang and gnu binutils compiled on MacOS",60,90+i*30+1+67,color);i++;
	orbisGlTextDraw("- Liborbis",60,90+i*30+1+67,color);i++;
	orbisGlTextDraw("https://github.com/orbisdev/liborbis",60,90+i*30+1+67,color);i++;
	orbisGlTextDraw("- Gimp for all graphic stuff",60,90+i*30+1+67,color);i++;
	orbisGlTextDraw("Special thanks goes to:",60,90+i*30+1+67,color);i++;
	orbisGlTextDraw("- Daniel Vik and all people who wrote the original bluemsx emulator",60,90+i*30+1+67,color);i++;
	orbisGlTextDraw("- Hitodama for his incredible work with ps4sdk the one and only",60,90+i*30+1+67,color);i++;
	orbisGlTextDraw("- flatz to bring a lot of fun with gl stuff",60,90+i*30+1+67,color);i++;
	orbisGlTextDraw("- @frangar, @xerpi @theflow0 and rest of people involved in vitasdk for their incredible work on Vita, it helped me a lot :P",60,90+i*30+1+67,color);i++;
	orbisGlTextDraw("Some parts of liborbis are based on their work",60,90+i*30+1+67,color);i++;
	orbisGlTextDraw("- @masterzorag for freetype and orbisfreetype-gl port and all samples for orbisgl and liborbis",60,90+i*30+1+67,color);i++;
	orbisGlTextDraw("- All people who have been using liborbis",60,90+i*30+1+67,color);i++;
	orbisGlTextDraw("- All ps3dev and ps2dev old comrades",60,90+i*30+1+67,color);i++;
	orbisGlTextDraw("- @ZiL0G80 and @notzecoxao for sharing some stuff with me",60,90+i*30+1+67,color);i++;
	orbisGlTextDraw("- All msx users, special mention to people in the 6th Msx's Users Meeting at Sevilla who must be testing this on site. @konamiman You should show this to Mr Nishi :P",60,90+i*30+1+67,color);i++;
	orbisGlTextDraw("- Hideo Kojima and Sony source of inspiration and dreams and for more than 35 years of playing and coding with Sony devices",60,90+i*30+1+67,color);i++;
	orbisGlTextDraw("If you want contact with me you can do at @psxdev on twitter",60,90+i*30+1+67,color);i++;i++;
	orbisGlTextDraw("Antonio Jose Ramos Marquez aka bigboss",60,90+i*30+1+67,color);i++;
	orbisGlTextDraw("The best is yet to come...",60,90+i*30+1+67,color);i++;i++;
}
void creditsDraw()
{
	glClear(GL_COLOR_BUFFER_BIT);

	if(browserTexture!=NULL)
	{
		
       // orbisGlTextureAdd(creditsTexture,0,0,creditsTexture->texture->width,creditsTexture->texture->height);
        
		orbisGlDrawTexture(browserTexture);
		creditsDrawText();
	}
}
void showCredits()
{
	//orbisAudioPause(0);
	screenStatus=SCREEN_CREDITS;
	while(screenStatus==SCREEN_CREDITS)
	{
		creditsUpdateController();
		creditsDraw();	
		orbisGlSwapBuffers();
	}
	//comeBack=0;
	//msx go go
	//displayNumber=0;
}