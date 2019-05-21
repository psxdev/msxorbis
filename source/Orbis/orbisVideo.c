/*****************************************************************************
**
** blueberryMSX
** https://github.com/pokebyte/blueberryMSX
**
** An MSX Emulator for Raspberry Pi based on blueMSX
**
** Copyright (C) 2003-2006 Daniel Vik
** Copyright (C) 2014 Akop Karapetyan
**
** GLES code is based on
** https://sourceforge.net/projects/atari800/ and
** https://code.google.com/p/pisnes/
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

#include <stdlib.h>
#include <stdio.h>

#include "Properties.h"
#include "VideoRender.h"

//#include <bcm_host.h>
//#include <interface/vchiq_arm/vchiq_if.h>
//#include <EGL/egl.h>
//#include <GLES2/gl2.h>
#include <orbisGl.h>
#include <debugnet.h>

typedef	struct ShaderInfo {
	GLuint program;
	GLint a_position;
	GLint a_texcoord;
	GLint u_vp_matrix;
	GLint u_texture;
	GLboolean scanline;
} ShaderInfo;

#define	TEX_WIDTH  600
#define	TEX_HEIGHT 240

#define BIT_DEPTH       16
#define BYTES_PER_PIXEL (BIT_DEPTH >> 3)
#define ZOOM            1
#define	WIDTH           544
#define	HEIGHT          240

#define	minU 0.0f
#define	maxU ((float)WIDTH / TEX_WIDTH)	
#define	minV 0.0f
#define	maxV ((float)HEIGHT / TEX_HEIGHT)

extern Video *video;
extern Properties *properties;

static void drawQuad(const ShaderInfo *sh);
static GLuint createShader(GLenum type, const char *shaderSrc);
static GLuint createProgram(const char *vertexShaderSrc, const char *fragmentShaderSrc);
static void setOrtho(float m[4][4],
	float left, float right, float bottom, float top,
	float near, float far, float scaleX, float scaleY);

//static EGL_DISPMANX_WINDOW_T nativeWindow;
//static EGLDisplay display = NULL;
//static EGLSurface surface = NULL;
//static EGLContext context = NULL;

uint32_t screenWidth = ATTR_ORBISGL_WIDTH;
uint32_t screenHeight = ATTR_ORBISGL_HEIGHT;

static ShaderInfo shader;
static GLuint buffers[3];
static GLuint textures[2];

//static SDL_Surface *sdlScreen;

char *msxScreen = NULL;
int msxScreenPitch;
int height;

static const char* vertexShaderSrc =
	"uniform mat4 u_vp_matrix;\n"
	"uniform bool scanline;\n"
	"attribute vec4 a_position;\n"
	"attribute vec2 a_texcoord;\n"
	"attribute vec4 in_Colour;\n"
	"varying vec4 v_vColour;\n"
	"varying mediump vec2 v_texcoord;\n"
	"varying vec4 TEX0;\n"
	"void main() {\n"
	"	v_texcoord = a_texcoord;\n"
	"	v_vColour = in_Colour;\n"
	"   if (scanline)\n"
	"	{\n"
	"   	gl_Position = a_position.x*u_vp_matrix[0] + a_position.y*u_vp_matrix[1] + a_position.z*u_vp_matrix[2] + a_position.w*u_vp_matrix[3];\n"
	"   	TEX0.xy = a_position.xy;\n"
	"	} else {"
	"		gl_Position = u_vp_matrix * a_position;\n"
	"	}\n"
	"}\n";
	
static const char* fragmentShaderSrc =
	"precision mediump float;\n"
	"varying  vec2 v_texcoord;\n"
	"uniform bool scanline;\n"
	"uniform sampler2D u_texture;\n"
	"varying vec4 TEX0;\n"
	"uniform vec2 TextureSize;\n"
	"void main() {\n"
	"   if (scanline)\n"
	"	{\n"
	"  		vec3 col;\n"
	"  		float x = TEX0.x * TextureSize.x;\n"
	"  		float y = floor(gl_FragCoord.y / 3.0) + 0.5;\n"
	"  		float ymod = mod(gl_FragCoord.y, 3.0);\n"
	"  		vec2 f0 = vec2(x, y);\n"
	"  		vec2 uv0 = f0 / TextureSize.xy;\n"
 	"  		vec3 t0 = texture2D(u_texture, v_texcoord).xyz;\n"
	"  		if (ymod > 2.0) {\n"
	"    		vec2 f1 = vec2(x, y + 1.0);\n"
	"    		vec2 uv1 = f1 / TextureSize.xy;\n"
	"    		vec3 t1 = texture2D(u_texture, uv1).xyz * 0.1;\n"
	"    		col = (t0 + t1) / 1.6;\n"
	"  		} else {\n"
	"    		col = t0;\n"
	"  		} \n"
	"  		gl_FragColor = vec4(col, 1.0);\n"
	"	} else {"
	"		gl_FragColor = texture2D(u_texture, v_texcoord);\n"
	"	}\n"
	"}\n";

static const GLfloat uvs[] = {
	minU, minV,
	maxU, minV,
	maxU, maxV,
	minU, maxV,
};

static const GLushort indices[] = {
	0, 1, 2,
	0, 2, 3,
};

static const int kVertexCount = 4;
static const int kIndexCount = 6;

static float projection[4][4];

static const GLfloat vertices[] = {
	-0.5, -0.5f, 0.0f,
	+0.5f, -0.5f, 0.0f,
	+0.5f, +0.5f, 0.0f,
	-0.5, +0.5f, 0.0f,
};

int orbisInitVideo()
{

	

	debugNetPrintf(DEBUG,"[MSXORBIS] %s Initializing shaders...\n",__FUNCTION__);

	// Init shader resources
	memset(&shader, 0, sizeof(ShaderInfo));
	shader.program = createProgram(vertexShaderSrc, fragmentShaderSrc);
	if (!shader.program) {
		fprintf(stderr, "createProgram() failed\n");
		return 0;
	}

	debugNetPrintf(DEBUG,"[MSXORBIS] %s Initializing textures/buffers...\n",__FUNCTION__);

	shader.a_position	= glGetAttribLocation(shader.program,	"a_position");
	shader.a_texcoord	= glGetAttribLocation(shader.program,	"a_texcoord");
	shader.u_vp_matrix	= glGetUniformLocation(shader.program,	"u_vp_matrix");
	shader.u_texture	= glGetUniformLocation(shader.program,	"u_texture");
	shader.scanline		= glGetUniformLocation(shader.program,  "scanline");
	
	glGenTextures(1, textures);
	glBindTexture(GL_TEXTURE_2D, textures[0]);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, TEX_WIDTH, TEX_HEIGHT, 0, GL_RGB, GL_UNSIGNED_SHORT_5_6_5, NULL);

	glGenBuffers(3, buffers);
	glBindBuffer(GL_ARRAY_BUFFER, buffers[0]);
	glBufferData(GL_ARRAY_BUFFER, kVertexCount * sizeof(GLfloat) * 3, vertices, GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, buffers[1]);
	glBufferData(GL_ARRAY_BUFFER, kVertexCount * sizeof(GLfloat) * 2, uvs, GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, buffers[2]);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, kIndexCount * sizeof(GL_UNSIGNED_SHORT), indices, GL_STATIC_DRAW);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	glDisable(GL_DEPTH_TEST);
	glDisable(GL_DITHER);

	// float sx = 1.0f;
	// float sy = 1.0f;
	// float zoom = (float)ZOOM;

	// // Screen aspect ratio adjustment // ratio
	// float a = (float)(screenWidth) / screenHeight * (float)(properties->video.screenRatio) / 100;
	// float a0 = (float)WIDTH / (float)HEIGHT;

	// if (a > a0) {
		// sx = a0/a;
	// } else {
		// sy = a/a0;
	// }

	// setOrtho(projection, -0.5f, +0.5f, +0.5f, -0.5f, -1.0f, 1.0f, sx * zoom * 1.1, sy * zoom);

	debugNetPrintf(DEBUG,"[MSXORBIS] %s Setting up screen...\n",__FUNCTION__);

//	msxScreenPitch = WIDTH * BIT_DEPTH / 8;
	msxScreen = (char*)calloc(1, BIT_DEPTH / 8 * TEX_WIDTH * TEX_HEIGHT);
	if (msxScreen==NULL) {
		debugNetPrintf(ERROR,"[MSXORBIS] %s Error allocating screen texture\n",__FUNCTION__);
		return 0;
	}

	//fprintf(stderr, "Initializing SDL video...\n");

	// We're doing our own video rendering - this is just so SDL-based keyboard
	// can work
    //SDL_VideoInit("DISPMANX", 0);
	//sdlScreen = SDL_SetVideoMode(0, 0, 0, SDL_ASYNCBLIT);
//    SDL_ShowCursor(SDL_DISABLE);
	return 1;
}

void orbisDestroyVideo()
{
	//if (sdlScreen) {
	//	SDL_FreeSurface(sdlScreen);
	//}
	if (msxScreen) {
		free(msxScreen);
	}

	// Destroy shader resources
	if (shader.program) {
		glDeleteProgram(shader.program);
		glDeleteBuffers(3, buffers);
		glDeleteTextures(1, textures);
	}
	
	// Release OpenGL resources
	//if (display) {
	//	eglMakeCurrent(display, EGL_NO_SURFACE, EGL_NO_SURFACE, EGL_NO_CONTEXT);
	//	eglDestroySurface(display, surface);
	//	eglDestroyContext(display, context);
	//	eglTerminate(display);
	//}

}

int width = -1;
int lines = -1;
int interlace = -1;
void orbisDebugDisplay()
{
	int w;
	if (properties->video.force4x3ratio)
	{
		w = (screenWidth - (screenHeight*4/3.0f));
	}
	if (w < 0) w = 0;
	debugNetPrintf(DEBUG,"[MSXORBIS] %s screenWidth %d screenHeight %d w %d\n",__FUNCTION__,screenWidth,screenHeight,w);
	debugNetPrintf(DEBUG,"[MSXORBIS] %s  syncMethod %d \n",__FUNCTION__,properties->emulation.syncMethod == P_EMU_SYNCTOVBLANK
);
}
void orbisUpdateEmuDisplay()
{
	int w = 0;
	if (!shader.program) 
	{
		debugNetPrintf(ERROR,"[MSXORBIS]orbisUpdateEmuDisplay Shader not initialized\n");
		return;
	}

	glClear(GL_COLOR_BUFFER_BIT);
	if (properties->video.force4x3ratio)
		w = (screenWidth - (screenHeight*4/3.0f));
	if (w < 0) w = 0;
	//glViewport(w/2, 0, screenWidth-w, screenHeight);
       glViewport(0, 0, ATTR_ORBISGL_WIDTH, ATTR_ORBISGL_HEIGHT);

	ShaderInfo *sh = &shader;

	glDisable(GL_BLEND);
	glUseProgram(sh->program);

	FrameBuffer* frameBuffer;
	frameBuffer = frameBufferFlipViewFrame(properties->emulation.syncMethod == P_EMU_SYNCTOVBLANKASYNC);
	if (frameBuffer == NULL) {
		frameBuffer = frameBufferGetWhiteNoiseFrame();
	}
	if (frameBufferGetDoubleWidth(frameBuffer, 0) != width)
	{
		width = frameBufferGetDoubleWidth(frameBuffer, 0);
		lines = frameBuffer->lines;
		msxScreenPitch = (256+16)*(width+1);
		height = frameBuffer->lines;
		float sx = 1.0f;
		float sy = 1.0f;
		debugNetPrintf(DEBUG,"[MSXORBIS] %s screen = %x, width = %d, height = %d, double = %d, interfaced = %d, ",__FUNCTION__, msxScreen, msxScreenPitch, frameBuffer->lines, width, interlace);
		sx = sx * msxScreenPitch/WIDTH;
		debugNetPrintf(DEBUG,"[MSXORBIS] %s sx=%f,sy=%f\n",__FUNCTION__,sx, sy);
		setOrtho(projection, -sx/2, sx/2, sy/2, -sy/2, -0.5f, +0.5f,1,1);		
		glUniformMatrix4fv(sh->u_vp_matrix, 1, GL_FALSE, &projection[0][0]);
	}
	int borderWidth = ((int)((WIDTH - frameBuffer->maxWidth)  * ZOOM)) >> 1;
	if (borderWidth < 0)
		borderWidth = 0;

	glUniform1i(shader.scanline, properties->video.scanlinesEnable);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, textures[0]);
	
//	videoRender(video, frameBuffer, BIT_DEPTH, 1,
//				msxScreen + borderWidth * BYTES_PER_PIXEL, 0, msxScreenPitch, -1);

	videoRender(video, 	frameBuffer, BIT_DEPTH, 1,
				msxScreen, 0, msxScreenPitch*2, -1);

	// if (borderWidth > 0) {
	// 	int h = height;
	// 	while (h--) {
	// 		memset(dpyData, 0, borderWidth * BYTES_PER_PIXEL);
	// 		memset(dpyData + (width - borderWidth) * BYTES_PER_PIXEL, 0, borderWidth * BYTES_PER_PIXEL);
	// 		dpyData += msxScreenPitch;
	// 	}
	// }

//	glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, WIDTH, HEIGHT,
//					GL_RGB, GL_UNSIGNED_SHORT_5_6_5, msxScreen);

	glTexSubImage2D(GL_TEXTURE_2D, 0, (WIDTH-msxScreenPitch)/2, 0, msxScreenPitch, frameBuffer->lines,
					GL_RGB, GL_UNSIGNED_SHORT_5_6_5, msxScreen);
					
	drawQuad(sh);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

	//eglSwapBuffers(display, surface);
	orbisGlSwapBuffers();
}

static GLuint createShader(GLenum type, const char *shaderSrc)
{
	
}

static GLuint createProgram(const char *vertexShaderSrc, const char *fragmentShaderSrc)
{
	/*GLuint vertexShader = createShader(GL_VERTEX_SHADER, vertexShaderSrc);
	if (!vertexShader) {
		fprintf(stderr, "createShader(GL_VERTEX_SHADER) failed\n");
		return 0;
	}

	GLuint fragmentShader = createShader(GL_FRAGMENT_SHADER, fragmentShaderSrc);
	if (!fragmentShader) {
		fprintf(stderr, "createShader(GL_FRAGMENT_SHADER) failed\n");
		glDeleteShader(vertexShader);
		return 0;
	}

	GLuint programObject = glCreateProgram();
	if (!programObject) {
		fprintf(stderr, "glCreateProgram() failed: %d\n", glGetError());
		return 0;
	}

	glAttachShader(programObject, vertexShader);
	glAttachShader(programObject, fragmentShader);

	// Link the program
	glLinkProgram(programObject);

	// Check the link status
	GLint linked = 0;
	glGetProgramiv(programObject, GL_LINK_STATUS, &linked);
	if (!linked) {
		GLint infoLen = 0;
		glGetProgramiv(programObject, GL_INFO_LOG_LENGTH, &infoLen);
		if (infoLen > 1) {
			char* infoLog = (char *)malloc(infoLen);
			glGetProgramInfoLog(programObject, infoLen, NULL, infoLog);
			fprintf(stderr, "Error linking program: %s\n", infoLog);
			free(infoLog);
		}

		glDeleteProgram(programObject);
		return 0;
	}

	// Delete these here because they are attached to the program object.
	glDeleteShader(vertexShader);
	glDeleteShader(fragmentShader);

	return programObject;*/
	if(vertexShaderSrc==NULL)
	{
	    debugNetPrintf(ERROR,"[MSXORBIS] %s can't open vertex shader\n",__FUNCTION__);
		return 0;
	}
	GLuint vs=orbisGlCompileShader(GL_VERTEX_SHADER,vertexShaderSrc);
	if(vs==0)
	{
	    debugNetPrintf(ERROR,"[MSXORBIS] %s can't compile vertex shader at %s\n",__FUNCTION__,vertexShaderSrc);
		return 0;
	}
	if(fragmentShaderSrc==NULL)
	{
	    debugNetPrintf(ERROR,"[MSXORBIS] %s can't open fragment shader\n",__FUNCTION__);
		return 0;
	}
	GLuint fs=orbisGlCompileShader(GL_FRAGMENT_SHADER,fragmentShaderSrc);
	if(fs==0)
	{
	    debugNetPrintf(ERROR,"[MSXORBIS] %s can't compile fragment shader at %s\n",__FUNCTION__,fragmentShaderSrc);
	}
	GLuint po=orbisGlLinkProgram(vs,fs);
	if(po==0)
	{
	    debugNetPrintf(ERROR,"[MSXORBIS] %s can't link program with vertex shader %d and fragment shader %d\n",__FUNCTION__,vs,fs);
		return 0;
	}
	glDeleteShader(vs);
	glDeleteShader(fs);
	
	return po;
}

static void setOrtho(float m[4][4],
	float left, float right, float bottom, float top,
	float near, float far, float scaleX, float scaleY)
{
	memset(m, 0, 4 * 4 * sizeof(float));
	m[0][0] = 2.0f / (right - left) * scaleX;
	m[1][1] = 2.0f / (top - bottom) * scaleY;
	m[2][2] = 0;//-2.0f / (far - near);
	m[3][0] = -(right + left) / (right - left);
	m[3][1] = -(top + bottom) / (top - bottom);
	m[3][2] = 0;//-(far + near) / (far - near);
	m[3][3] = 1;
}

static void drawQuad(const ShaderInfo *sh)
{
	glUniform1i(sh->u_texture, 0);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

	glBindBuffer(GL_ARRAY_BUFFER, buffers[0]);
	glVertexAttribPointer(sh->a_position, 3, GL_FLOAT,
		GL_FALSE, 3 * sizeof(GLfloat), NULL);
	glEnableVertexAttribArray(sh->a_position);

	glBindBuffer(GL_ARRAY_BUFFER, buffers[1]);
	glVertexAttribPointer(sh->a_texcoord, 2, GL_FLOAT,
		GL_FALSE, 2 * sizeof(GLfloat), NULL);
	glEnableVertexAttribArray(sh->a_texcoord);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, buffers[2]);

	glDrawElements(GL_TRIANGLES, kIndexCount, GL_UNSIGNED_SHORT, 0);
}
