/*
 * liborbis 
 * Copyright (C) 2015,2016,2017,2018 Antonio Jose Ramos Marquez (aka bigboss) @psxdev on twitter
 * Repository https://github.com/orbisdev/liborbis
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <kernel.h>
#include <stdbool.h>
#include <sys/fcntl.h>
#include <orbisFile.h>
#include <debugnet.h>
#include <orbis2d.h>  // reuse orbis2d png wrappers and Orbis2dTexture type
/*typedef struct Orbis2dTexture
{
    uint32_t *datap;
    unsigned int width;
    unsigned int height;
    unsigned int depth;
}Orbis2dTexture;*/
#include "orbisGlTextureManager.h"




int orbisGlTextureCounter = 0;
OrbisGlTexture orbisGlTextures[10];

int orbisGlProgramCounter = 0;
OrbisGlProgram orbisGlPrograms[10];


GLuint s_xyz_loc;
GLuint s_uv_loc;
GLuint s_sampler_loc;

GLuint orbisGlGetProgramId(OrbisGlProgram *po)
{
        //debugNetPrintf(DEBUG,"[MSXORBIS] %s ...\n",__FUNCTION__);

    if(po!=NULL)
    {
        //debugNetPrintf(DEBUG,"[MSXORBIS] %s program %d skip 2 ...\n",__FUNCTION__,po->programId);

        return po->programId;
    }
        debugNetPrintf(DEBUG,"[MSXORBIS] %s program is NULL, skip 2 ...\n",__FUNCTION__);

    return 0;
}
GLuint orbisGlGetTextureId(OrbisGlTexture *text)
{
    if(text!=NULL)
    {
        return text->textureId;
    }
    return 0;
}
OrbisGlProgram * orbisGlGetProgram(char *name)
{
	if(strlen(name)>255)
	{
        return NULL;	
	}

    for(int i=0;i<orbisGlProgramCounter;i++)
	{
        if(strcmp(name,orbisGlPrograms[i].name)==0)
		{
            return &orbisGlPrograms[i];
		}
    }
    return NULL;
}
OrbisGlTexture * orbisGlGetTexture(char* name) 
{
	if(strlen(name)>255)
	{
        return NULL;	
	}

    for(int i=0;i<orbisGlTextureCounter;i++)
	{
        if(strcmp(name,orbisGlTextures[i].name)==0)
		{
            return &orbisGlTextures[i];
		}
    }
    return NULL;
}
OrbisGlTexture * orbistGlTextureManagerRegisterTextureFromBuffer(char* name,int width,int height,int format,void *buf)
{
    GLuint textureId;
    
    if(!buf)
    {
        return NULL;
    }

    textureId=orbisGlCreateTexture(width,height,format,buf);
    if(textureId<=0)
    {
        debugNetPrintf(DEBUG,"[MSXORBIS] %s Texture can't be loaded %s, skip 2 ...\n",__FUNCTION__,name);
        return NULL;   
    }
    
   
    orbisGlTextures[orbisGlTextureCounter].textureId=textureId;
    orbisGlTextures[orbisGlTextureCounter].width=width;
    orbisGlTextures[orbisGlTextureCounter].height=height;
    strcpy(orbisGlTextures[orbisGlTextureCounter].name, name);
    debugNetPrintf(DEBUG,"[MSXORBIS]%s Texture registered name=%s id=%d !\n",__FUNCTION__,name,textureId);
    
    orbisGlTextureCounter++;
    return &orbisGlTextures[orbisGlTextureCounter-1];
}
	
OrbisGlTexture * orbisGlTextureManagerRegisterPngTexture(char* name, char* path) 
{
    debugNetPrintf(DEBUG,"[MSXORBIS] %s registering texture: %s (path: %s)\n",__FUNCTION__,name,path);
	/*char *raw;
    if (orbisGlGetTexture(name))
	{
        debugNetPrintf(DEBUG,"[MSXORBIS] %s Texture already exist %s, skip ...\n",__FUNCTION__,name);
        return NULL;
    }
	int pFile=orbisOpen(path, O_RDONLY,0);
	if (pFile<=0)
	{
		debugNetPrintf(DEBUG,"[MSXORBIS] %s Couldn't open %s\n",__FUNCTION__,path);
		return NULL;
	} 
    int32_t fileSize=orbisLseek(pFile,0,SEEK_END);
	orbisLseek(pFile,0,SEEK_SET);  // Seek back to start
	if(fileSize<0)
	{
        debugNetPrintf(DEBUG,"[MSXORBIS] %s Failed to read size of file %s\n",__FUNCTION__,path);
        orbisClose(pFile);
		return NULL;
	}
	
    raw=malloc(sizeof(char)*fileSize);
	if(raw==NULL)
	{
        debugNetPrintf(DEBUG,"[MSXORBIS] %s malloc error %s\n",__FUNCTION__,path);
        orbisClose(pFile);
		return NULL;
	}
    if(orbisRead(pFile,raw,fileSize)!=fileSize)
    {
        debugNetPrintf(DEBUG,"[MSXORBIS] %s Failed to read content of file %s\n",__FUNCTION__,path);
        orbisClose(pFile);
        free(raw);
        return NULL;
	}
	orbisClose(pFile);*/
    Orbis2dTexture* texture_raw = orbis2dLoadPngFromHost_v2(path);
    if (!texture_raw)
	{
        debugNetPrintf(DEBUG,"[MSXORBIS] %s Texture can't be loaded %s, skip ...\n",__FUNCTION__,path);
        //free(raw);
		return NULL;
    }
    debugNetPrintf(DEBUG,"[MSXORBIS] %s png loaded %s, ...\n",__FUNCTION__,path);

    GLenum format = GL_RGBA;
    if (texture_raw->depth==8)
	{
        format = GL_RGB;
    }
    debugNetPrintf(DEBUG,"[MSXORBIS] %s rgba=%d rgb=%d\n",__FUNCTION__,format==GL_RGBA,format==GL_RGB);



    OrbisGlTexture *t=orbistGlTextureManagerRegisterTextureFromBuffer(name,texture_raw->width,texture_raw->height,format,texture_raw->datap);
	
	//orbis2dDestroyTexture(texture_raw);
   // debugNetPrintf(DEBUG,"[MSXORBIS] %s texture destroyed\n",__FUNCTION__);


    return t;
}
void orbisGlTextureManagerFinish()
{
    int i;
    for(i=0;i<orbisGlProgramCounter;i++)
    {
        orbisGlDestroyProgram(orbisGlPrograms[i].programId);
    }
    for(i=0;i<orbisGlTextureCounter;i++)
    {
        orbisGlDestroyProgram(orbisGlTextures[i].textureId);
    }
}
OrbisGlProgram *orbisGlTextureManagerInit()
{
    OrbisGlProgram *p;
    GLuint programId = 0;
    programId=orbisGlCreateProgram(TEXTURE_VERTEX_SHADER,TEXTURE_FRAGMENT_SHADER);
    if(!programId)
    {
        debugNetPrintf(DEBUG,"[MSXORBIS] %s Error during linking shader ! program_id=%d (0x%08x)\n",__FUNCTION__,programId, programId);
        return 0;
    }
        debugNetPrintf(DEBUG,"[MSXORBIS] %s linked shader program_id=%d (0x%08x)\n",__FUNCTION__,programId, programId);

    orbisGlPrograms[orbisGlProgramCounter].programId=programId;
    p=&orbisGlPrograms[orbisGlProgramCounter];
    debugNetPrintf(DEBUG,"[MSXORBIS] %s linked shader program_id=%d (0x%08x)\n",__FUNCTION__,p->programId, p->programId);

    orbisGlProgramCounter++;
    return p;

	
   /* s_xyz_loc     = glGetAttribLocation(programID,  "a_xyz");
    s_uv_loc      = glGetAttribLocation(programID,  "a_uv");
    s_sampler_loc = glGetUniformLocation(programID, "s_texture");
    */
    /*glUseProgram(programID);
    ret = glGetError();
    if (ret) {
        debugNetPrintf(ERROR,"[ORBIS_GL] glUseProgram failed: 0x%08X\n", ret);
    }*/

    // set viewport
    //on_GLES2_Size(view_w, view_h);
}
/*OrbisGlTextureState * orbisGlInitTexture1(char *name,char *path,OrbisGlProgram *program,int x, int y)
{
    OrbisGlTextureState *p;
    p=(OrbisGlTextureState*)malloc(sizeof(OrbisGlTextureState));
    p->screenWidth=x;
    p->screenHeight=y;
    p->texture=orbisGlTextureManagerRegisterPngTexture(name,path);
    debugNetPrintf(DEBUG,"[MSXORBIS] %s after texture register program=%d\n",__FUNCTION__,program?1:0);

    if(p->texture==NULL || program==NULL)
    {
       if(p->texture==NULL)
        {
                    debugNetPrintf(DEBUG,"[MSXORBIS] %s texture is NULL 0\n",__FUNCTION__);

        }
        else
        {
        debugNetPrintf(DEBUG,"[MSXORBIS] %s texture is NULL 1\n",__FUNCTION__);
        }
        //free(p);
        return NULL;
    }
    debugNetPrintf(DEBUG,"[MSXORBIS] %s after test program=%d %d %d\n",__FUNCTION__,program?1:0,p->screenWidth,p->screenHeight);

    p->program=program;
    debugNetPrintf(DEBUG,"[MSXORBIS] %s after test program=%d\n",__FUNCTION__,p->program?1:0);

    GLuint programId=orbisGlGetProgramId(p->program);
    debugNetPrintf(DEBUG,"[MSXORBIS] %s programId %d\n",__FUNCTION__, programId);

    glGenBuffers(1,&p->bufferId);
    
    // get location of shader parameters
    p->screenHalfSizeId = glGetUniformLocation(programId, "screenHalfSize");
    debugNetPrintf(DEBUG,"[MSXORBIS] %s screenHalfSizeId %d\n",__FUNCTION__, p->screenHalfSizeId);
    
    p->positionId = glGetAttribLocation(programId, "vertPosition_screenspace");
    debugNetPrintf(DEBUG,"[MSXORBIS] %s positionId %d\n",__FUNCTION__, p->positionId);
   
    p->uvId = glGetAttribLocation(programId, "texCoord0");
    debugNetPrintf(DEBUG,"[MSXORBIS] %s uvId %d\n",__FUNCTION__, p->uvId);
    
    p->samplerId = glGetUniformLocation(programId, "textureSampler");
    debugNetPrintf(DEBUG,"[MSXORBIS] %s samplerId %d\n",__FUNCTION__, p->samplerId);
    
    
    // set invariant uniforms
    glUseProgram(programId);
    glUniform1i(p->samplerId, 0); // sample from texture unit #0
    glUniform2f(p->screenHalfSizeId, (float)p->screenWidth/2, (float)p->screenHeight/2);
    glUseProgram(0);

    // allocate a buffer for the  vertex data
    p->bufferData = (OrbisGlVertexInfo*)malloc(sizeof(OrbisGlVertexInfo)*6);
    p->count = 0;
        debugNetPrintf(DEBUG,"[MSXORBIS] %s samplerId %d\n",__FUNCTION__, p->samplerId);

    return p;
}
*/
OrbisGlTextureState * orbisGlInitTexture(char *name,char *path,OrbisGlProgram *program,int x, int y)
{
    OrbisGlTextureState *p;
    p=(OrbisGlTextureState*)malloc(sizeof(OrbisGlTextureState));
  //  p->screenWidth=x;
  //  p->screenHeight=y;
    p->texture=orbisGlTextureManagerRegisterPngTexture(name,path);
    debugNetPrintf(DEBUG,"[MSXORBIS] %s after texture register program=%d\n",__FUNCTION__,program?1:0);

    if(p->texture==NULL || program==NULL)
    {
       if(p->texture==NULL)
        {
                    debugNetPrintf(DEBUG,"[MSXORBIS] %s texture is NULL 0\n",__FUNCTION__);

        }
        else
        {
        debugNetPrintf(DEBUG,"[MSXORBIS] %s texture is NULL 1\n",__FUNCTION__);
        }
        //free(p);
        return NULL;
    }
    //debugNetPrintf(DEBUG,"[MSXORBIS] %s after test program=%d %d %d\n",__FUNCTION__,program?1:0,p->screenWidth,p->screenHeight);

    p->program=program;
    debugNetPrintf(DEBUG,"[MSXORBIS] %s after test program=%d\n",__FUNCTION__,p->program?1:0);

    GLuint programId=orbisGlGetProgramId(p->program);
    debugNetPrintf(DEBUG,"[MSXORBIS] %s programId %d\n",__FUNCTION__, programId);

    p->s_xyz_loc     = glGetAttribLocation(programId,  "a_xyz");
    p->s_uv_loc      = glGetAttribLocation(programId,  "a_uv");
    p->s_sampler_loc = glGetUniformLocation(programId, "s_texture");

    // set invariant uniforms
   // glUseProgram(programId);
   // glUniform1i(p->samplerId, 0); // sample from texture unit #0
   // glUniform2f(p->screenHalfSizeId, (float)p->screenWidth/2, (float)p->screenHeight/2);
   // glUseProgram(0);

    // allocate a buffer for the  vertex data
   // p->bufferData = (OrbisGlVertexInfo*)malloc(sizeof(OrbisGlVertexInfo)*6);
    p->count = 0;
      //  debugNetPrintf(DEBUG,"[MSXORBIS] %s samplerId %d\n",__FUNCTION__, p->samplerId);

    return p;
}

void orbisGlFinishTexture(OrbisGlTextureState *p)
{
    glDeleteBuffers(1, &p->bufferId);
    glDeleteTextures(1, &p->texture->textureId);
    free(p->bufferData);
}
void orbisGlTextureAdd(OrbisGlTextureState *p, int x, int y,int w,int h)
{
    float fx=(float)x;
    float fy=(float)y;
    float fw=(float)w;
    float fh=(float)h;

    // fill vertex/uv buffer 
    OrbisGlVertexInfo* pBuffer = &p->bufferData[p->count*6];  
    
        *pBuffer++ = (OrbisGlVertexInfo) { fx, fy, 0.0, 0.0 };
        *pBuffer++ = (OrbisGlVertexInfo) { fx, fy+fh, 0.0, 1.0 };
        *pBuffer++ = (OrbisGlVertexInfo) { fx+fw, fy, 1.0, 0.0 };
        *pBuffer++ = (OrbisGlVertexInfo) { fx+fw, fy+fh, 1.0, 1.0 };
        *pBuffer++ = (OrbisGlVertexInfo) { fx+fw, fy, 1.0, 0.0 };
        *pBuffer++ = (OrbisGlVertexInfo) { fx, fy+fh, 0.0, 1.0 };
    p->count++;
}
void orbisGlDrawTextureSpecial(OrbisGlTextureState *p, int x, int y) {
  //  float fx = x;
  //  float fy = y;

    if (!p)
        return;

float w=1920.0;
float h=1080.0;
int x1=x+p->texture->width;
int y1=y+p->texture->height;

 float fx=2.0*(x/w+0.5)-1.0;
 float fy=2.0*((1080-y)/h+0.5)-1.0;
 float fx1=2.0*(x1/w+0.5)-1.0;
 float fy1=2.0*((1080-y1)/h+0.5)-1.0;
    /*const GLfloat vertexArray[] = 
    {
        fx,  fy, 0.0f, // Position 0
        fx, fy+0,1, 0.0f, // Position 1
         fx+0.1,fy+0.1, 0.0f, // Position 2
         fx+0.1,  fy, 0.0f, // Position 3
    };*/
const GLfloat vertexArray[] = 
    {
        -0.01f,  0.01f, 0.0f, // Position 0
        -0.01f, -0.01f, 0.0f, // Position 1
         0.01f, -0.01f, 0.0f, // Position 2
         0.01f,  0.01f, 0.0f, // Position 3
    };

    const GLfloat textureArray[] = 
    {
        0.0f,  0.0f, // Position 0
        0.0f,  1.0f, // Position 1
        1.0f,  1.0f, // Position 2
        1.0f,  0.0f, // Position 3
    };

    const uint16_t drawList[] = { 0, 1, 2, 0, 2, 3 };

    int ret = 0;
    GLuint programId=orbisGlGetProgramId(p->program);
    GLuint textureId=orbisGlGetTextureId(p->texture);
    
    glUseProgram(programId);

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    glVertexAttribPointer(p->s_xyz_loc, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (void*)vertexArray);
    ret = glGetError();
    if (ret) {
        debugNetPrintf(ERROR,"[ORBIS_GL] glVertexAttribPointer failed: 0x%08X\n", ret);
    }

    glVertexAttribPointer(p->s_uv_loc, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(GLfloat), (void*)textureArray);
    ret = glGetError();
    if (ret) {
        debugNetPrintf(ERROR,"[ORBIS_GL] glVertexAttribPointer failed: 0x%08X\n", ret);
    }

    glEnableVertexAttribArray(p->s_xyz_loc);
    ret = glGetError();
    if (ret) {
        debugNetPrintf(ERROR,"[ORBIS_GL] glEnableVertexAttribArray (1) failed: 0x%08X\n", ret);
    }

    glEnableVertexAttribArray(p->s_uv_loc);
    ret = glGetError();
    if (ret) {
        debugNetPrintf(ERROR,"[ORBIS_GL] glEnableVertexAttribArray (2) failed: 0x%08X\n", ret);
    }

    glActiveTexture(GL_TEXTURE0);
    ret = glGetError();
    if (ret) {
        debugNetPrintf(ERROR,"[PKGLoader] glActiveTexture failed: 0x%08X\n", ret);
    }

    glBindTexture(GL_TEXTURE_2D, textureId);
    ret = glGetError();
    if (ret) {
        debugNetPrintf(ERROR,"[PKGLoader] glBindTexture failed: 0x%08X\n", ret);
    }

    glUniform1i(p->s_sampler_loc, 0);
    ret = glGetError();
    if (ret) {
        debugNetPrintf(ERROR,"[PKGLoader] glUniform1i failed: 0x%08X\n", ret);
    }

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glGenerateMipmap(GL_TEXTURE_2D);

    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_SHORT, drawList);
    ret = glGetError();
    if (ret) {
        debugNetPrintf(ERROR,"[PKGLoader] glDrawElements failed: 0x%08X\n", ret);
    }
    // disconnect slots from shader
    glDisableVertexAttribArray(p->s_xyz_loc);
    glDisableVertexAttribArray(p->s_uv_loc);
     glUseProgram(0);
    glBindTexture(GL_TEXTURE_2D, 0);
    glDisable(GL_BLEND);
}


void orbisGlDrawTexture(OrbisGlTextureState *p) {
  //  float fx = x;
  //  float fy = y;

    if (!p)
        return;

    const GLfloat vertexArray[] = 
    {
        -1.0f,  1.0f, 0.0f, // Position 0
        -1.0f, -1.0f, 0.0f, // Position 1
         1.0f, -1.0f, 0.0f, // Position 2
         1.0f,  1.0f, 0.0f, // Position 3
    };

    const GLfloat textureArray[] = 
    {
        0.0f,  0.0f, // Position 0
        0.0f,  1.0f, // Position 1
        1.0f,  1.0f, // Position 2
        1.0f,  0.0f, // Position 3
    };

    const uint16_t drawList[] = { 0, 1, 2, 0, 2, 3 };

    int ret = 0;
    GLuint programId=orbisGlGetProgramId(p->program);
    GLuint textureId=orbisGlGetTextureId(p->texture);
    
    glUseProgram(programId);

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    glVertexAttribPointer(p->s_xyz_loc, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (void*)vertexArray);
    ret = glGetError();
    if (ret) {
        debugNetPrintf(ERROR,"[ORBIS_GL] glVertexAttribPointer failed: 0x%08X\n", ret);
    }

    glVertexAttribPointer(p->s_uv_loc, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(GLfloat), (void*)textureArray);
    ret = glGetError();
    if (ret) {
        debugNetPrintf(ERROR,"[ORBIS_GL] glVertexAttribPointer failed: 0x%08X\n", ret);
    }

    glEnableVertexAttribArray(p->s_xyz_loc);
    ret = glGetError();
    if (ret) {
        debugNetPrintf(ERROR,"[ORBIS_GL] glEnableVertexAttribArray (1) failed: 0x%08X\n", ret);
    }

    glEnableVertexAttribArray(p->s_uv_loc);
    ret = glGetError();
    if (ret) {
        debugNetPrintf(ERROR,"[ORBIS_GL] glEnableVertexAttribArray (2) failed: 0x%08X\n", ret);
    }

    glActiveTexture(GL_TEXTURE0);
    ret = glGetError();
    if (ret) {
        debugNetPrintf(ERROR,"[PKGLoader] glActiveTexture failed: 0x%08X\n", ret);
    }

    glBindTexture(GL_TEXTURE_2D, textureId);
    ret = glGetError();
    if (ret) {
        debugNetPrintf(ERROR,"[PKGLoader] glBindTexture failed: 0x%08X\n", ret);
    }

    glUniform1i(p->s_sampler_loc, 0);
    ret = glGetError();
    if (ret) {
        debugNetPrintf(ERROR,"[PKGLoader] glUniform1i failed: 0x%08X\n", ret);
    }

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glGenerateMipmap(GL_TEXTURE_2D);

    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_SHORT, drawList);
    ret = glGetError();
    if (ret) {
        debugNetPrintf(ERROR,"[PKGLoader] glDrawElements failed: 0x%08X\n", ret);
    }
    // disconnect slots from shader
    glDisableVertexAttribArray(p->s_xyz_loc);
    glDisableVertexAttribArray(p->s_uv_loc);
     glUseProgram(0);
    glBindTexture(GL_TEXTURE_2D, 0);
    glDisable(GL_BLEND);
}

/*void orbisGlDrawTexture1(OrbisGlTextureState *p)
{

    
    glBindBuffer(GL_ARRAY_BUFFER, p->bufferId);
    glBufferData(GL_ARRAY_BUFFER, 
        sizeof(OrbisGlVertexInfo)*6*p->count, p->bufferData, GL_STATIC_DRAW);
    GLuint programId=orbisGlGetProgramId(p->program);
    GLuint textureId=orbisGlGetTextureId(p->texture);
    // bind the shader program 
    glUseProgram(programId);

        glClearColor(0.0f, 1.0f, 0.0f, 1.0f); //blue RGBA

    glClear(GL_COLOR_BUFFER_BIT);

    // bind the model's texture to texture unit #0
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, textureId);

    // set OpenGL state
   // glDisable(GL_CULL_FACE);
   // glDisable(GL_DEPTH_TEST);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    // set vertex data pointers
    glEnableVertexAttribArray(p->positionId);
    glVertexAttribPointer(p->positionId, 2, GL_FLOAT, false, sizeof(OrbisGlVertexInfo), 
        (const void*)0);

    glEnableVertexAttribArray(p->uvId);
    glVertexAttribPointer(p->uvId, 2, GL_FLOAT, false, sizeof(OrbisGlVertexInfo), 
        (const void*)(sizeof(float)*2));

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glGenerateMipmap(GL_TEXTURE_2D);

    // draw!
    glDrawArrays(GL_TRIANGLES, 0, p->count * 6);
    p->count = 0;

    // clean up state - unbind OpenGL resources
    glDisableVertexAttribArray(p->positionId);
    glDisableVertexAttribArray(p->uvId);
    glUseProgram(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindTexture(GL_TEXTURE_2D, 0);
    glDisable(GL_BLEND);
}*/



