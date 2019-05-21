#ifndef _BROWSER_H_
#define _BROWSER_H_

#define BROWSER_BACKGROUND_FILE_PATH "host0:app/msxorbis/system/textures/msxorbis.png"
#define FILE_ICON_PATH "host0:app/msxorbis/system/textures/file_icon.png"
#define FOLDER_ICON_PATH "host0:app/msxorbis/system/textures/folder_icon.png"
#define SETTINGS_BACKGROUND_FILE_PATH "host0:app/msxorbis/system/textures/settings.png"
#define CREDITS_BACKGROUND_FILE_PATH "host0:app/msxorbis/system/textures/credits.png"
enum SCREEN_STATUS 
{
	SCREEN_EMU,
	SCREEN_BROWSER,
	SCREEN_SETTINGS,
	SCREEN_CREDITS,
	
};

void showBrowser();

#endif