#include <lbxgl2.h>

extern int lbxgl_shader_gloss;

extern PDGLUI_InputState *lbxgl_state;
extern PDGLUI_Camera *lbxgl_cam;
LBXGL_BrushWorld *lbxgl_world;

#if 1
void LBXGL_BrushWorld_DrawRenderView(
	LBXGL_BrushWorld *world, float *org, float *rot,
	int xs, int ys, float aspect, float fov, 
	LBXGL_Light *light)
{
	static int skip=0;
	float tv0[4], tv1[4], org2[3];
	LBXGL_Light *lcur;
	LBXGL_ModelState *mcur;
	int i, j, k, l;
	float f, g, d;

	if(skip)
		{ return; }

	LBXGL_BrushWorld_DrawBuildFast(world);

	V3F_COPY(lbxgl_cam->org, org2);
	V3F_COPY(org, lbxgl_cam->org);

//	LBXGL_BrushWorld_PreLightWorldFast(world, org);

//	UI_Camera_SetupView3D(0, 0, xs, ys);
	UI_Camera_SetupViewAtPersp3D(0, 0, xs, ys, aspect, fov, org, rot);

//	glCullFace(GL_BACK); glEnable(GL_CULL_FACE);
	glDepthFunc(GL_LEQUAL);

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	if(light)
	{
		LBXGL_BrushWorld_DrawWorldLightViewFast(world, light);
	}else
	{
//		LBXGL_BrushWorld_DrawBrushes(world, world->brush);
		LBXGL_BrushWorld_DrawWorldFast(world);
	}

	glFinish();

//	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	V3F_COPY(org2, lbxgl_cam->org);
}

LBXGL_API void LBXGL_BrushWorld_DrawRenderCubeFace(
	LBXGL_BrushWorld *world, float *org, int face,
	int xs, int ys, LBXGL_Light *light)
{
	float rot[9];

	switch(face)
	{
	case 10:
		V3F_SET(rot+0,  0,  1,  0);
		V3F_SET(rot+3,  1,  0,  0);
		V3F_SET(rot+6,  0,  0,  1);
		break;
	case 11:
		V3F_SET(rot+0, -1,  0,  0);
		V3F_SET(rot+3,  0,  1,  0);
		V3F_SET(rot+6,  0,  0,  1);
		break;
	case 12:
		V3F_SET(rot+0,  0,  1,  0);
		V3F_SET(rot+3,  0,  0,  1);
		V3F_SET(rot+6, -1,  0,  0);
		break;
	case 13:
		V3F_SET(rot+0,  0, -1,  0);
		V3F_SET(rot+3, -1,  0,  0);
		V3F_SET(rot+6,  0,  0,  1);
		break;
	case 14:
		V3F_SET(rot+0,  1,  0,  0);
		V3F_SET(rot+3,  0, -1,  0);
		V3F_SET(rot+6,  0,  0,  1);
		break;
	case 15:
		V3F_SET(rot+0,  0,  1,  0);
		V3F_SET(rot+3,  0,  0, -1);
		V3F_SET(rot+6,  1,  0,  0);
		break;

	case GL_TEXTURE_CUBE_MAP_POSITIVE_X:
		V3F_SET(rot+0,  0,  0, -1);
		V3F_SET(rot+3,  1,  0,  0);
		V3F_SET(rot+6,  0, -1,  0);
		break;
	case GL_TEXTURE_CUBE_MAP_NEGATIVE_X:
		V3F_SET(rot+0,  0,  0,  1);
		V3F_SET(rot+3, -1,  0,  0);
		V3F_SET(rot+6,  0, -1,  0);
		break;
	case GL_TEXTURE_CUBE_MAP_POSITIVE_Y:
		V3F_SET(rot+0,  1,  0,  0);
		V3F_SET(rot+3,  0,  1,  0);
		V3F_SET(rot+6,  0,  0,  1);
		break;
	case GL_TEXTURE_CUBE_MAP_NEGATIVE_Y:
		V3F_SET(rot+0,  1,  0,  0);
		V3F_SET(rot+3,  0, -1,  0);
		V3F_SET(rot+6,  0,  0, -1);
		break;
	case GL_TEXTURE_CUBE_MAP_POSITIVE_Z:
		V3F_SET(rot+0,  1,  0,  0);
		V3F_SET(rot+3,  0,  0,  1);
		V3F_SET(rot+6,  0, -1,  0);
		break;
	case GL_TEXTURE_CUBE_MAP_NEGATIVE_Z:
		V3F_SET(rot+0, -1,  0,  0);
		V3F_SET(rot+3,  0,  0, -1);
		V3F_SET(rot+6,  0, -1,  0);
		break;
	default:
		V3F_SET(rot+0,  1,  0,  0);
		V3F_SET(rot+3,  0,  1,  0);
		V3F_SET(rot+6,  0,  0,  1);
		break;
	}

	LBXGL_BrushWorld_DrawRenderView(world, org, rot,
		xs, ys, 1.0, 90, light);
}
#endif

void LBXGL_BrushWorld_DrawRenderViewBuffer(
	LBXGL_BrushWorld *world, float *org, float *rot,
	int *stbuf, int *zbuf, int xs, int ys, float aspect, float fov, 
	LBXGL_Light *light)
{
	float tv0[4], tv1[4], org2[3];
	LBXGL_Light *lcur;
	LBXGL_ModelState *mcur;
	int i, j, k, l;
	float f, g, d;

	LBXGL_BrushWorld_DrawBuildFast(world);

	V3F_COPY(lbxgl_cam->org, org2);
	V3F_COPY(org, lbxgl_cam->org);

//	LBXGL_BrushWorld_PreLightWorldFast(world, org);

//	UI_Camera_SetupView3D(0, 0, xs, ys);
	UI_Camera_SetupViewAtPersp3D(0, 0, xs, ys, aspect, fov, org, rot);

//	glCullFace(GL_BACK); glEnable(GL_CULL_FACE);
	glDepthFunc(GL_LEQUAL);

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	if(light)
	{
		LBXGL_BrushWorld_DrawWorldLightViewFast(world, light);
	}else
	{
//		LBXGL_BrushWorld_DrawBrushes(world, world->brush);
		LBXGL_BrushWorld_DrawWorldFast(world);
	}

	glFinish();
	if(stbuf)
	{
//		glReadPixels(0, 0, xs, ys, GL_RGBA, GL_UNSIGNED_BYTE, stbuf);
		glReadPixels(0, 0, xs, ys, GL_BGRA, GL_UNSIGNED_INT_8_8_8_8, stbuf);
	}

	if(zbuf)
	{
		glReadPixels(0, 0, xs, ys,
			GL_DEPTH_STENCIL_EXT, GL_UNSIGNED_INT_24_8_EXT, zbuf);

#if 0
		k=zbuf[0]; l=zbuf[0];
		for(i=0; i<xs*ys; i++)
		{
			j=zbuf[i];
			if(j<k)k=j;
			if(j>l)l=j;
		}
		printf("DrawRenderViewBuffer: Depth %08X/%08X\n", k, l);
#endif
	}

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
//	glClear(GL_COLOR_BUFFER_BIT);

	V3F_COPY(org2, lbxgl_cam->org);
}

LBXGL_API void LBXGL_BrushWorld_DrawRenderCubeFaceBuffer(
	LBXGL_BrushWorld *world, float *org, int face,
	int *stbuf, int *zbuf, int xs, int ys, LBXGL_Light *light)
{
	float rot[9];

	switch(face)
	{
	case 10:
		V3F_SET(rot+0,  0,  1,  0);
		V3F_SET(rot+3,  1,  0,  0);
		V3F_SET(rot+6,  0,  0,  1);
		break;
	case 11:
		V3F_SET(rot+0, -1,  0,  0);
		V3F_SET(rot+3,  0,  1,  0);
		V3F_SET(rot+6,  0,  0,  1);
		break;
	case 12:
		V3F_SET(rot+0,  0,  1,  0);
		V3F_SET(rot+3,  0,  0,  1);
		V3F_SET(rot+6, -1,  0,  0);
		break;
	case 13:
		V3F_SET(rot+0,  0, -1,  0);
		V3F_SET(rot+3, -1,  0,  0);
		V3F_SET(rot+6,  0,  0,  1);
		break;
	case 14:
		V3F_SET(rot+0,  1,  0,  0);
		V3F_SET(rot+3,  0, -1,  0);
		V3F_SET(rot+6,  0,  0,  1);
		break;
	case 15:
		V3F_SET(rot+0,  0,  1,  0);
		V3F_SET(rot+3,  0,  0, -1);
		V3F_SET(rot+6,  1,  0,  0);
		break;

	case GL_TEXTURE_CUBE_MAP_POSITIVE_X:
		V3F_SET(rot+0,  0,  0, -1);
		V3F_SET(rot+3,  1,  0,  0);
		V3F_SET(rot+6,  0, -1,  0);
		break;
	case GL_TEXTURE_CUBE_MAP_NEGATIVE_X:
		V3F_SET(rot+0,  0,  0,  1);
		V3F_SET(rot+3, -1,  0,  0);
		V3F_SET(rot+6,  0, -1,  0);
		break;
	case GL_TEXTURE_CUBE_MAP_POSITIVE_Y:
		V3F_SET(rot+0,  1,  0,  0);
		V3F_SET(rot+3,  0,  1,  0);
		V3F_SET(rot+6,  0,  0,  1);
		break;
	case GL_TEXTURE_CUBE_MAP_NEGATIVE_Y:
		V3F_SET(rot+0,  1,  0,  0);
		V3F_SET(rot+3,  0, -1,  0);
		V3F_SET(rot+6,  0,  0, -1);
		break;
	case GL_TEXTURE_CUBE_MAP_POSITIVE_Z:
		V3F_SET(rot+0,  1,  0,  0);
		V3F_SET(rot+3,  0,  0,  1);
		V3F_SET(rot+6,  0, -1,  0);
		break;
	case GL_TEXTURE_CUBE_MAP_NEGATIVE_Z:
		V3F_SET(rot+0, -1,  0,  0);
		V3F_SET(rot+3,  0,  0, -1);
		V3F_SET(rot+6,  0, -1,  0);
		break;
	default:
		V3F_SET(rot+0,  1,  0,  0);
		V3F_SET(rot+3,  0,  1,  0);
		V3F_SET(rot+6,  0,  0,  1);
		break;
	}

	LBXGL_BrushWorld_DrawRenderViewBuffer(world, org, rot,
		stbuf, zbuf, xs, ys, 1.0, 90, light);

//	LBXGL_BrushWorld_DrawRenderViewBuffer(world, org, rot,
//		stbuf, zbuf, xs, ys, 1.0, 45);
}


#if 1
LBXGL_API void LBXGL_BrushWorld_DrawRenderToCubeMap(
	LBXGL_BrushWorld *world, float *org, int texnum, int ztexnum,
	int flags, LBXGL_Light *light)
{
#ifndef GLES
	static int cube_fbo=0;

	byte oldautovis[4096];
	byte oldautovis_shadow[4096];
	byte oldmdlvis[4096];
	int ia[4];

	float org2[3];
	int i, sv0, size;


#if 1
	memcpy(oldautovis, world->autovis, 4096);
	memcpy(oldautovis_shadow, world->autovis_shadow, 4096);
	memcpy(oldmdlvis, world->mdlvis, 4096);

	//make everything visible
	memset(world->autovis, 255, 4096);
	memset(world->autovis_shadow, 255, 4096);
	memset(world->mdlvis, 255, 4096);

	V3F_SET(world->autovis_mins, -9999, -9999, -9999);
	V3F_SET(world->autovis_maxs,  9999,  9999,  9999);
#endif

	V3F_COPY(lbxgl_cam->org, org2);
	V3F_COPY(org, lbxgl_cam->org);

	LBXGL_BrushWorld_PreLightWorldFast(world, org);

	if(flags&LBXGL_LFL_SUN)
	{
//		size=512;
		size=1024;
	}else
	{
		size=256;
//		size=128;
	}


#if 0
	sv0=world->shadows;
	world->shadows=-1;
	for(i=0; i<6; i++)
	{
		if(flags&LBXGL_LFL_SUN)
		{
			if((i!=5) && !(flags&LBXGL_LFL_CHANGED))
			{
				stbuf[i]=NULL;
				zbuf[i]=NULL;
				continue;
			}
		}

		glDisable(GL_CULL_FACE);
//		glEnable(GL_CULL_FACE);
//		glCullFace(GL_FRONT);
		glCullFace(GL_BACK);

		stbuf[i]=(texnum>0)?malloc(size*size*4):NULL;
		zbuf[i]=(ztexnum>0)?malloc(size*size*4):NULL;
		LBXGL_BrushWorld_DrawRenderCubeFaceBuffer(world, org,
			GL_TEXTURE_CUBE_MAP_POSITIVE_X+i,
			stbuf[i], zbuf[i], size, size, light);
	}
	world->shadows=sv0;
#endif

	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);

	glEnable(GL_TEXTURE_2D);
	
	glEnable(GL_TEXTURE_CUBE_MAP);
	
	if((texnum>0) && (flags&LBXGL_LFL_CHANGED))
	{
		glBindTexture(GL_TEXTURE_CUBE_MAP, texnum);

		for(i=0; i<6; i++)
		{
			glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X+i,
				0, GL_RGBA8, size, size, 0,
				GL_BGRA, GL_UNSIGNED_INT_8_8_8_8, NULL);
		}

		glTexParameteri(GL_TEXTURE_CUBE_MAP,
			GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_CUBE_MAP,
			GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_CUBE_MAP,
			GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

		glTexParameterf(GL_TEXTURE_CUBE_MAP,
			GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameterf(GL_TEXTURE_CUBE_MAP,
			GL_TEXTURE_MAG_FILTER, GL_LINEAR);
			
		glFinish();
	}

	if((ztexnum>0) && (flags&LBXGL_LFL_CHANGED))
	{
		glBindTexture(GL_TEXTURE_CUBE_MAP, ztexnum);

		for(i=0; i<6; i++)
		{
			glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X+i,
				0, GL_DEPTH_STENCIL_EXT, size, size, 0,
				GL_DEPTH_STENCIL_EXT, GL_UNSIGNED_INT_24_8_EXT, NULL);

//			glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X+i,
//				0, GL_DEPTH, size, size, 0,
//				GL_DEPTH, GL_UNSIGNED_INT, NULL);
		}

		glTexParameteri(GL_TEXTURE_CUBE_MAP,
			GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_CUBE_MAP,
			GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_CUBE_MAP,
			GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

		glTexParameterf(GL_TEXTURE_CUBE_MAP,
			GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameterf(GL_TEXTURE_CUBE_MAP,
			GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		glFinish();
	}

	glCullFace(GL_FRONT);
	glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
	glDisable(GL_TEXTURE_CUBE_MAP);


	// framebuffer object
	if(cube_fbo<=0)
		{ pdglGenFramebuffers(1, &cube_fbo); }

	pdglBindFramebuffer(GL_FRAMEBUFFER_EXT, cube_fbo);

//	status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
//	printf("%d\"\n", status);
//	printf("%d\n", GL_FRAMEBUFFER_COMPLETE);

    glViewport(0, 0, size, size);

    for(i=0; i<6; i++)
    {
		if((i!=5) && !(flags&LBXGL_LFL_CHANGED))
			{ continue; }

		pdglFramebufferTexture2D(GL_FRAMEBUFFER_EXT, 
			GL_DEPTH_ATTACHMENT_EXT,
			GL_TEXTURE_CUBE_MAP_POSITIVE_X+i,
			ztexnum, 0);

		pdglFramebufferTexture2D(GL_FRAMEBUFFER_EXT, 
			GL_STENCIL_ATTACHMENT_EXT,
			GL_TEXTURE_CUBE_MAP_POSITIVE_X+i,
			ztexnum, 0);

		pdglFramebufferTexture2D(GL_FRAMEBUFFER_EXT,
			GL_COLOR_ATTACHMENT0_EXT,
			GL_TEXTURE_CUBE_MAP_POSITIVE_X+i, texnum, 0);

		ia[0]=GL_COLOR_ATTACHMENT0_EXT;
		ia[1]=GL_DEPTH_ATTACHMENT_EXT;
		pdglDrawBuffers(1, ia);

		LBXGL_BrushWorld_DrawRenderCubeFace(world, org,
			GL_TEXTURE_CUBE_MAP_POSITIVE_X+i,
			size, size, light);
    }

	pdglBindFramebuffer(GL_FRAMEBUFFER_EXT, 0);

	V3F_COPY(org2, lbxgl_cam->org);

	memcpy(world->autovis, oldautovis, 4096);
	memcpy(world->autovis_shadow, oldautovis_shadow, 4096);
	memcpy(world->mdlvis, oldmdlvis, 4096);
#endif
}
#endif


#if 0
LBXGL_API void LBXGL_BrushWorld_DrawRenderToCubeMap(
	LBXGL_BrushWorld *world, float *org, int texnum, int ztexnum,
	int flags, LBXGL_Light *light)
{
#ifndef GLES

	byte oldautovis[4096];
	byte oldautovis_shadow[4096];
	byte oldmdlvis[4096];

	int *stbuf[6];
	int *zbuf[6];
	float org2[3];
	int i, sv0, size;


#if 1
	memcpy(oldautovis, world->autovis, 4096);
	memcpy(oldautovis_shadow, world->autovis_shadow, 4096);
	memcpy(oldmdlvis, world->mdlvis, 4096);

	//make everything visible
	memset(world->autovis, 255, 4096);
	memset(world->autovis_shadow, 255, 4096);
	memset(world->mdlvis, 255, 4096);

	V3F_SET(world->autovis_mins, -9999, -9999, -9999);
	V3F_SET(world->autovis_maxs,  9999,  9999,  9999);
#endif

	V3F_COPY(lbxgl_cam->org, org2);
	V3F_COPY(org, lbxgl_cam->org);

	LBXGL_BrushWorld_PreLightWorldFast(world, org);

	if(flags&LBXGL_LFL_SUN)
	{
		size=512;
//		size=1024;
	}else
	{
//		size=256;
		size=128;
	}


	sv0=world->shadows;
	world->shadows=-1;
	for(i=0; i<6; i++)
	{
		if(flags&LBXGL_LFL_SUN)
		{
			if((i!=5) && !(flags&LBXGL_LFL_CHANGED))
			{
				stbuf[i]=NULL;
				zbuf[i]=NULL;
				continue;
			}
		}

		glDisable(GL_CULL_FACE);
//		glEnable(GL_CULL_FACE);
//		glCullFace(GL_FRONT);
		glCullFace(GL_BACK);

		stbuf[i]=(texnum>0)?malloc(size*size*4):NULL;
		zbuf[i]=(ztexnum>0)?malloc(size*size*4):NULL;
		LBXGL_BrushWorld_DrawRenderCubeFaceBuffer(world, org,
			GL_TEXTURE_CUBE_MAP_POSITIVE_X+i,
			stbuf[i], zbuf[i], size, size, light);
	}
	world->shadows=sv0;

	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);

	glEnable(GL_TEXTURE_2D);
	
	glEnable(GL_TEXTURE_CUBE_MAP);
	
	if(texnum>0)
	{
		glBindTexture(GL_TEXTURE_CUBE_MAP, texnum);

		for(i=0; i<6; i++)
		{
			if(!stbuf[i])
				continue;
		
//			LBXGL_BrushWorld_DrawRenderCubeFaceBuffer(world, org,
//				GL_TEXTURE_CUBE_MAP_POSITIVE_X+i,
//				stbuf, 256, 256);

//			glBindTexture(GL_TEXTURE_CUBE_MAP, texnum);
//			glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X+i,
//				0, GL_RGBA8, size, size, 0,
//				GL_RGBA, GL_UNSIGNED_BYTE, stbuf[i]);
			glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X+i,
				0, GL_RGBA8, size, size, 0,
				GL_BGRA, GL_UNSIGNED_INT_8_8_8_8, stbuf[i]);
			free(stbuf[i]);
		}

		glTexParameteri(GL_TEXTURE_CUBE_MAP,
			GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_CUBE_MAP,
			GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_CUBE_MAP,
			GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

		glTexParameterf(GL_TEXTURE_CUBE_MAP,
			GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameterf(GL_TEXTURE_CUBE_MAP,
			GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		glFinish();
	}

	if(ztexnum>0)
	{
		glBindTexture(GL_TEXTURE_CUBE_MAP, ztexnum);

		for(i=0; i<6; i++)
		{
			if(!zbuf[i])
				continue;

//			LBXGL_BrushWorld_DrawRenderCubeFaceBuffer(world, org,
//				GL_TEXTURE_CUBE_MAP_POSITIVE_X+i,
//				stbuf, 256, 256);

//			glBindTexture(GL_TEXTURE_CUBE_MAP, texnum);
//			glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X+i,
//				0, GL_DEPTH24_STENCIL8_EXT, 256, 256, 0,
//				GL_DEPTH_STENCIL_EXT, GL_UNSIGNED_INT_24_8_EXT, zbuf[i]);

			glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X+i,
				0, GL_DEPTH_STENCIL_EXT, size, size, 0,
				GL_DEPTH_STENCIL_EXT, GL_UNSIGNED_INT_24_8_EXT, zbuf[i]);
			free(zbuf[i]);
		}

		glTexParameteri(GL_TEXTURE_CUBE_MAP,
			GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_CUBE_MAP,
			GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_CUBE_MAP,
			GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

		glTexParameterf(GL_TEXTURE_CUBE_MAP,
			GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameterf(GL_TEXTURE_CUBE_MAP,
			GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		glFinish();
	}

	glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
	glDisable(GL_TEXTURE_CUBE_MAP);

	V3F_COPY(org2, lbxgl_cam->org);


	memcpy(world->autovis, oldautovis, 4096);
	memcpy(world->autovis_shadow, oldautovis_shadow, 4096);
	memcpy(world->mdlvis, oldmdlvis, 4096);

//	free(stbuf);
#endif
}
#endif

LBXGL_API void LBXGL_BrushWorld_DrawGlossCubeMap(
	LBXGL_BrushWorld *world, int texnum)
{
	byte *stbuf;
	int i;
	
#ifndef GLES

	LBXGL_Shader_Init();
	
	stbuf=malloc(256*256*4);
//	LBXGL_BrushWorld_DrawRenderCubeFaceBuffer(world, org,
//		GL_TEXTURE_CUBE_MAP_POSITIVE_X+i,
//		stbuf[i], 256, 256);

	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, lbxgl_shader_gloss);
	glGetTexImage(GL_TEXTURE_2D, 0, GL_RGBA, GL_UNSIGNED_BYTE, stbuf);
	
	glEnable(GL_TEXTURE_CUBE_MAP);
	glBindTexture(GL_TEXTURE_CUBE_MAP, texnum);

	for(i=0; i<6; i++)
	{
		glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X+i,
			0, GL_RGBA8, 256, 256, 0,
			GL_RGBA, GL_UNSIGNED_BYTE, stbuf);
	}
	free(stbuf);

	glTexParameteri(GL_TEXTURE_CUBE_MAP,
		GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP,
		GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP,
		GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

	glTexParameterf(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameterf(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	glDisable(GL_TEXTURE_CUBE_MAP);
#endif

}

LBXGL_API void LBXGL_BrushWorld_DrawRenderToCubeFaces(
	LBXGL_BrushWorld *world, float *org, int *rtex)
{
	int *stbuf[6];
	float org2[3];
	int i, xs, sv0;

#ifndef GLES

#if 1
	//make everything visible
	memset(world->autovis, 255, 4096);
	memset(world->autovis_shadow, 255, 4096);
	memset(world->mdlvis, 255, 4096);

	V3F_SET(world->autovis_mins, -9999, -9999, -9999);
	V3F_SET(world->autovis_maxs,  9999,  9999,  9999);
#endif

	V3F_COPY(lbxgl_cam->org, org2);
	V3F_COPY(org, lbxgl_cam->org);

//	LBXGL_BrushWorld_PreLightWorldFast(world, org);

	xs=256;

	sv0=world->shadows;
	world->shadows=-1;
	for(i=0; i<6; i++)
	{
		stbuf[i]=malloc(xs*xs*4);
		LBXGL_BrushWorld_DrawRenderCubeFaceBuffer(world, org,
//			GL_TEXTURE_CUBE_MAP_POSITIVE_X+i,
			10+i,
			stbuf[i], NULL, xs, xs, NULL);
	}
	world->shadows=sv0;

	glEnable(GL_TEXTURE_2D);
	
//	glEnable(GL_TEXTURE_CUBE_MAP);
//	glBindTexture(GL_TEXTURE_CUBE_MAP, texnum);

	for(i=0; i<6; i++)
	{
//		LBXGL_BrushWorld_DrawRenderCubeFaceBuffer(world, org,
//			GL_TEXTURE_CUBE_MAP_POSITIVE_X+i,
//			stbuf, 256, 256);

		if(rtex[i]<=0)
			rtex[i]=Tex_AllocTexnum();

		glBindTexture(GL_TEXTURE_2D, rtex[i]);
//		glTexImage2D(GL_TEXTURE_2D,
//			0, GL_RGBA8, xs, xs, 0,
//			GL_RGBA, GL_UNSIGNED_BYTE, stbuf[i]);

		glTexImage2D(GL_TEXTURE_2D,
			0, GL_RGBA8, xs, xs, 0,
			GL_BGRA, GL_UNSIGNED_INT_8_8_8_8, stbuf[i]);

		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		free(stbuf[i]);
	}

	V3F_COPY(org2, lbxgl_cam->org);
#endif
}


LBXGL_API void LBXGL_BrushWorld_FlushLightEnvMap(
	LBXGL_BrushWorld *world, LBXGL_Light *light)
{
	if(light->envtex>0)
	{
		Tex_DeleteTexture(light->envtex);
		light->envtex=0;
	}
	if(light->envdepthtex>0)
	{
		Tex_DeleteTexture(light->envdepthtex);
		light->envdepthtex=0;
	}
}

LBXGL_API void LBXGL_BrushWorld_UpdateLightEnvMap_Delay(
	LBXGL_BrushWorld *world, LBXGL_Light *light)
{
	if(lbxgl_state->doshot&32)
	{
		light->env_time=lbxgl_state->time_f+0.25*(1.0+bgbrng_randg()*0.1);
		return;
	}

	if(light->flags&LBXGL_LFL_SUN)
	{
		light->env_time=lbxgl_state->time_f+2.0*(1.0+bgbrng_randg()*0.1);
//		light->env_time=lbxgl_state->time_f+1.0*(1.0+bgbrng_randg()*0.1);
	}else
	{
//		light->env_time=lbxgl_state->time_f+8.0*(1.0+bgbrng_randg()*0.1);
		light->env_time=lbxgl_state->time_f+4.0*(1.0+bgbrng_randg()*0.1);
//		light->env_time=lbxgl_state->time_f+2.0*(1.0+bgbrng_randg()*0.1);
//		light->env_time=lbxgl_state->time_f+4.0+(bgbrng_randg()*0.1);
	}
//		light->env_time=lbxgl_state->time_f+4.0+(bgbrng_randg()*0.1);
}

LBXGL_API void LBXGL_BrushWorld_UpdateLightEnvMap(
	LBXGL_BrushWorld *world, LBXGL_Light *light)
{
	float f;

	if(lbxgl_state->time_f <= light->env_time)
		return;

	if(!(light->flags&LBXGL_LFL_ENVRENDER))
		return;

#if 0
	if(light->flags&LBXGL_LFL_SUN)
	{
		f=LBXGL_Voxel_GetCameraSunGamma(world);
		if(f<0.001)
		{
			LBXGL_BrushWorld_UpdateLightEnvMap_Delay(world, light);
			V3F_COPY(light->org, light->env_org);
			return;
		}
	}
#endif


	if(light->envtex<=0)
	{
		light->envtex=Tex_AllocTexnum();
		light->envdepthtex=Tex_AllocTexnum();
		LBXGL_BrushWorld_DrawRenderToCubeMap(
			world, light->org,
			light->envtex, light->envdepthtex,
			light->flags|LBXGL_LFL_CHANGED, light);
//		light->env_time=lbxgl_state->time_f+4.0+(bgbrng_randg()*0.1);
		LBXGL_BrushWorld_UpdateLightEnvMap_Delay(world, light);
		V3F_COPY(light->org, light->env_org);
	}else
	{
		if(lbxgl_state->time_f >= light->env_time)
		{
			LBXGL_BrushWorld_DrawRenderToCubeMap(
				world, light->org,
				light->envtex, light->envdepthtex, light->flags, light);
//			light->env_time=lbxgl_state->time_f+4.0+(bgbrng_randg()*0.1);
			LBXGL_BrushWorld_UpdateLightEnvMap_Delay(world, light);
			V3F_COPY(light->org, light->env_org);
		}
	}
}
