#include <lbxgl2.h>

extern PDGLUI_InputState *lbxgl_state;
extern PDGLUI_Camera *lbxgl_cam;
extern LBXGL_BrushWorld *lbxgl_world;

int lbxgl_voxel_chkleft;
int lbxgl_voxel_chkstart;

int lbxgl_voxel_reforg[3];
bool lbxgl_voxel_disablevbo=false;
float lbxgl_voxel_drawdist=4096;

void LBXGL_Voxel_DrawChunkSimpleFlat(LBXGL_VoxelChunk *chk)
{
	if(!chk->mesh->n_face)
		return;

#if 1
	if(chk->mesh->vbo_face>0)
	{
		pdglBindBuffer(GL_ARRAY_BUFFER, chk->mesh->vbo_face);
		LBXGL_Shader_DrawArraysSimpleFlat(
			PDGL_TRIANGLES, 0, chk->mesh->n_face*3,
			3, GL_FLOAT, 0, (byte *)chk->mesh->vbo_face_xyz);
		pdglBindBuffer(GL_ARRAY_BUFFER, 0);
	}else
	{
		LBXGL_Shader_DrawArraysSimpleFlat(PDGL_TRIANGLES, 0, chk->mesh->n_face*3,
			3, GL_FLOAT, 0, chk->mesh->face_xyz);
	}
#endif
}

void LBXGL_Voxel_DrawChunkFlat(LBXGL_VoxelChunk *chk)
{
	if(!chk->mesh->n_face)
		return;

	if(chk->flags&VOX_CHFL_VISCLIP)
		return;

#if 1
	if(chk->mesh->vbo_face>0)
	{
		pdglBindBuffer(GL_ARRAY_BUFFER, chk->mesh->vbo_face);
		LBXGL_Shader_DrawArraysNormalFlat(
			PDGL_TRIANGLES, 0, chk->mesh->n_face*3,
			3, GL_FLOAT, 0, (byte *)chk->mesh->vbo_face_xyz,
			3, GL_BYTE, 3, (byte *)chk->mesh->vbo_face_norm);
		pdglBindBuffer(GL_ARRAY_BUFFER, 0);
	}else
	{
		LBXGL_Shader_DrawArraysNormalFlat(PDGL_TRIANGLES, 0, chk->mesh->n_face*3,
			3, GL_FLOAT, 0, chk->mesh->face_xyz,
			3, GL_BYTE, 3, chk->mesh->face_norm);
	}
#endif
}

void LBXGL_Voxel_DrawChunkFlatRGB(LBXGL_VoxelChunk *chk)
{
	if(!chk->mesh->n_face)
		return;

	if(chk->flags&VOX_CHFL_VISCLIP)
		return;

#if 1
	if(chk->mesh->vbo_face>0)
	{
		pdglBindBuffer(GL_ARRAY_BUFFER, chk->mesh->vbo_face);
		LBXGL_Shader_DrawArraysNormalFlatRGB(
			PDGL_TRIANGLES, 0, chk->mesh->n_face*3,
			3, GL_FLOAT, 0, (byte *)chk->mesh->vbo_face_xyz,
			3, GL_BYTE, 3, (byte *)chk->mesh->vbo_face_norm,
			4, GL_UNSIGNED_BYTE, 0, (byte *)chk->mesh->vbo_face_rgba);
		pdglBindBuffer(GL_ARRAY_BUFFER, 0);
	}else
	{
		LBXGL_Shader_DrawArraysNormalFlatRGB(PDGL_TRIANGLES, 0, chk->mesh->n_face*3,
			3, GL_FLOAT, 0, chk->mesh->face_xyz,
			3, GL_BYTE, 3, chk->mesh->face_norm,
			4, GL_UNSIGNED_BYTE, 0, chk->mesh->face_rgba);
	}
#endif
}

void LBXGL_Voxel_DrawChunkFlatVLRGB(LBXGL_VoxelChunk *chk)
{
	int i, j, k, tn, fl;

	if(!chk->mesh->n_face)
		return;

	if(chk->flags&VOX_CHFL_VISCLIP)
		return;

#if 0
//	if(chk->flags&VOX_CHFL_USESHADER)
	if((chk->flags&VOX_CHFL_USESHADER) && !(chk->flags&VOX_CHFL_DISTANT))
	{
		LBXGL_Shader_NormalX3f(0, 0, 0);
		LBXGL_Shader_NormalY3f(0, 0, 0);

//		LBXGL_Shader_BindTexture(-1);
		pdglBindBuffer(GL_ARRAY_BUFFER, chk->mesh->vbo_face);

		for(i=0; i<chk->mesh->n_face; i++)
		{
			for(j=i; j<chk->mesh->n_face; j++)
				if(chk->mesh->face_tex[j]!=chk->mesh->face_tex[i])
					break;

			k=j-i;

			tn=LBXGL_Voxel_MapTexture(chk, chk->mesh->face_tex[i]);
			LBXGL_Shader_BindTexture(tn);

			LBXGL_Shader_DrawArraysNormalTexRGB(
				PDGL_TRIANGLES, i*3, k*3,
				3, GL_FLOAT, 0, (byte *)chk->mesh->vbo_face_xyz,
				2, GL_FLOAT, 0, (byte *)chk->mesh->vbo_face_st,
				3, GL_BYTE, 3, (byte *)chk->mesh->vbo_face_norm,
				4, GL_UNSIGNED_BYTE, 0, (byte *)chk->mesh->vbo_face_vlrgba);

			i+=k-1;
		}

		pdglBindBuffer(GL_ARRAY_BUFFER, 0);
		return;
	}
#endif

	LBXGL_Shader_BindTexture(-1);

#if 1
	if(chk->mesh->vbo_face>0)
	{
		pdglBindBuffer(GL_ARRAY_BUFFER, chk->mesh->vbo_face);
		LBXGL_Shader_DrawArraysNormalTexRGB(
			PDGL_TRIANGLES, 0, chk->mesh->n_face*3,
			3, GL_FLOAT, 0, (byte *)chk->mesh->vbo_face_xyz,
			2, GL_FLOAT, 0, (byte *)chk->mesh->vbo_face_st,
			3, GL_BYTE, 3, (byte *)chk->mesh->vbo_face_norm,
			4, GL_UNSIGNED_BYTE, 0, (byte *)chk->mesh->vbo_face_vlrgba);
		pdglBindBuffer(GL_ARRAY_BUFFER, 0);
	}else
	{
		LBXGL_Shader_DrawArraysNormalFlatRGB(PDGL_TRIANGLES, 0, chk->mesh->n_face*3,
			3, GL_FLOAT, 0, chk->mesh->face_xyz,
			3, GL_BYTE, 3, chk->mesh->face_norm,
			4, GL_UNSIGNED_BYTE, 0, chk->mesh->face_vlrgba);
	}
#endif

#if 0
	pdglBindBuffer(GL_ARRAY_BUFFER, chk->mesh->vbo_face);
	LBXGL_Shader_DrawArraysNormalFlatRGB(
		PDGL_TRIANGLES, 0, chk->mesh->n_face*3,
		3, GL_FLOAT, 0, (byte *)chk->mesh->vbo_face_xyz,
		3, GL_BYTE, 3, (byte *)chk->mesh->vbo_face_norm,
		4, GL_UNSIGNED_BYTE, 0, (byte *)chk->mesh->vbo_face_vlrgba);
	pdglBindBuffer(GL_ARRAY_BUFFER, 0);
#endif

#if 0
	LBXGL_Shader_DrawArraysNormalFlatRGB(PDGL_TRIANGLES, 0, chk->mesh->n_face*3,
		3, GL_FLOAT, 0, chk->mesh->face_xyz,
		3, GL_BYTE, 3, chk->mesh->face_norm,
		4, GL_UNSIGNED_BYTE, 0, chk->mesh->face_vlrgba);
#endif
}

void LBXGL_Voxel_DrawChunkFlatVLRGB_AlphaLightMat(LBXGL_VoxelChunk *chk)
{
	int i, j, k, tn, fl;

	if(!chk->mesh->n_aface)
		return;

	if(lbxgl_world->flags&
			(BTGE_WFL_LIGHTBUDGET|BTGE_WFL_ALPHABUDGET))
		return;

	if(chk->flags&VOX_CHFL_ALPHAVISCLIP)
		return;
	if(!(chk->flags&VOX_CHFL_ALPHALIGHTMAT))
		return;
	if(chk->flags&VOX_CHFL_DISTANT)
		return;

	LBXGL_Shader_NormalX3f(0, 0, 0);
	LBXGL_Shader_NormalY3f(0, 0, 0);

//	LBXGL_Shader_BindTexture(-1);
	if(chk->mesh->vbo_aface>0)
		{ pdglBindBuffer(GL_ARRAY_BUFFER, chk->mesh->vbo_aface); }

	for(i=0; i<chk->mesh->n_aface; i++)
	{
		for(j=i; j<chk->mesh->n_aface; j++)
			if(chk->mesh->aface_tex[j]!=chk->mesh->aface_tex[i])
				break;
		fl=BTGE_Texture_GetImageFlags(chk->mesh->aface_tex[i]);
		if(!(fl&BTGE_TXFL_LIGHTMAT)) { i=j-1; continue; }

		k=j-i;

		tn=LBXGL_Voxel_MapTexture(chk, chk->mesh->aface_tex[i]);
		LBXGL_Shader_BindTexture(tn);

		if(chk->mesh->vbo_aface>0)
		{
			LBXGL_Shader_DrawArraysNormalTexRGB(
				PDGL_TRIANGLES, i*3, k*3,
				3, GL_FLOAT, 0, (byte *)chk->mesh->vbo_aface_xyz,
				2, GL_FLOAT, 0, (byte *)chk->mesh->vbo_aface_st,
				3, GL_BYTE, 3, (byte *)chk->mesh->vbo_aface_norm,
				4, GL_UNSIGNED_BYTE, 0, (byte *)chk->mesh->vbo_aface_vlrgba);
		}else
		{
			LBXGL_Shader_DrawArraysNormalTexRGB(
				PDGL_TRIANGLES, i*3, k*3,
				3, GL_FLOAT, 0, (byte *)chk->mesh->aface_xyz,
				2, GL_FLOAT, 0, (byte *)chk->mesh->aface_st,
				3, GL_BYTE, 3, (byte *)chk->mesh->aface_norm,
				4, GL_UNSIGNED_BYTE, 0, (byte *)chk->mesh->aface_vlrgba);
		}

		i+=k-1;
	}

	if(chk->mesh->vbo_aface>0)
		{ pdglBindBuffer(GL_ARRAY_BUFFER, 0); }
}

void LBXGL_Voxel_DrawChunkLight_AlphaLightMat(
	LBXGL_VoxelChunk *chk, LBXGL_Light *light)
{
	byte *rgba;
	byte *rgba2;
	bool useva;
	int i, j, k, tn, fl;

	if(!chk->mesh->n_aface)
		return;

	if(lbxgl_world->flags&
			(BTGE_WFL_LIGHTBUDGET|BTGE_WFL_ALPHABUDGET))
		return;

	if(chk->flags&VOX_CHFL_ALPHAVISCLIP)
		return;
	if(!(chk->flags&VOX_CHFL_ALPHALIGHTMAT))
		return;
	if(chk->flags&VOX_CHFL_DISTANT)
		return;

	if(!LBXGL_Voxel_CheckChunkLight(chk, light))
		return;

//	LBXGL_Shader_BlendFunc(GL_SRC_COLOR, GL_ONE);
//	LBXGL_Shader_Color4f(1, 1, 1, 1);
//	LBXGL_Shader_NormalX3f(0, 0, 0);
//	LBXGL_Shader_NormalY3f(0, 0, 0);

	LBXGL_Shader_NormalX3f(0, 0, 0);
	LBXGL_Shader_NormalY3f(0, 0, 0);

	if(light->flags&LBXGL_LFL_SUN)
		rgba=chk->mesh->aface_slrgba;
	else rgba=chk->mesh->aface_rgba;

	if(light->flags&LBXGL_LFL_SUN)
		rgba2=(byte *)chk->mesh->vbo_aface_slrgba;
	else rgba2=(byte *)chk->mesh->vbo_aface_rgba;

//	LBXGL_Shader_BindTexture(-1);

	if(chk->mesh->vbo_aface>0)
		{ pdglBindBuffer(GL_ARRAY_BUFFER, chk->mesh->vbo_aface); }

	for(i=0; i<chk->mesh->n_aface; i++)
	{
		for(j=i; j<chk->mesh->n_aface; j++)
			if(chk->mesh->aface_tex[j]!=chk->mesh->aface_tex[i])
				break;
		fl=BTGE_Texture_GetImageFlags(chk->mesh->aface_tex[i]);
		if(!(fl&BTGE_TXFL_LIGHTMAT)) { i=j-1; continue; }

		k=j-i;

		tn=LBXGL_Voxel_MapTexture(chk, chk->mesh->aface_tex[i]);
		LBXGL_Shader_BindTexture(tn);

#if 1
		if(chk->mesh->vbo_aface>0)
		{
			LBXGL_Shader_DrawArraysNormalTexRGB(
				PDGL_TRIANGLES, i*3, k*3,
				3, GL_FLOAT, 0, (byte *)chk->mesh->vbo_aface_xyz,
				2, GL_FLOAT, 0, (byte *)chk->mesh->vbo_aface_st,
				3, GL_BYTE, 3, (byte *)chk->mesh->vbo_aface_norm,
				4, GL_UNSIGNED_BYTE, 0, rgba2);
		}else
		{
			LBXGL_Shader_DrawArraysNormalTexRGB(
				PDGL_TRIANGLES, i*3, k*3,
				3, GL_FLOAT, 0, (byte *)chk->mesh->aface_xyz,
				2, GL_FLOAT, 0, (byte *)chk->mesh->aface_st,
				3, GL_BYTE, 3, (byte *)chk->mesh->aface_norm,
				4, GL_UNSIGNED_BYTE, 0, rgba);
		}
#endif

		i+=k-1;
	}

	if(chk->mesh->vbo_aface>0)
		{ pdglBindBuffer(GL_ARRAY_BUFFER, 0); }
}

void LBXGL_Voxel_DrawChunkLight(
	LBXGL_VoxelChunk *chk, LBXGL_Light *light)
{
#ifndef BTGE_VOXEL_BYTERGBA
	float *rgba;
#else
	byte *rgba;
	byte *rgba2;
#endif
	bool useva;
	int i, j, k, tn, fl;

	if(!chk->mesh->n_face)
		return;
	if(chk->flags&VOX_CHFL_VISCLIP)
		return;

	if(!LBXGL_Voxel_CheckChunkLight(chk, light))
		return;

//	LBXGL_Shader_BlendFunc(GL_SRC_COLOR, GL_ONE);
//	LBXGL_Shader_Color4f(1, 1, 1, 1);
//	LBXGL_Shader_NormalX3f(0, 0, 0);
//	LBXGL_Shader_NormalY3f(0, 0, 0);

#if 1
	if((chk->flags&VOX_CHFL_USESHADER) && !(chk->flags&VOX_CHFL_DISTANT))
//	if(1)
//	if(0)
	{
		LBXGL_Shader_NormalX3f(0, 0, 0);
		LBXGL_Shader_NormalY3f(0, 0, 0);

		if(light->flags&LBXGL_LFL_SUN)
//		if(1)
			rgba=chk->mesh->face_slrgba;
		else rgba=chk->mesh->face_rgba;
//		useva=false;

		if(light->flags&LBXGL_LFL_SUN)
			rgba2=(byte *)chk->mesh->vbo_face_slrgba;
		else rgba2=(byte *)chk->mesh->vbo_face_rgba;

//		LBXGL_Shader_BindTexture(-1);

		if(chk->mesh->vbo_face>0)
			{ pdglBindBuffer(GL_ARRAY_BUFFER, chk->mesh->vbo_face); }

		for(i=0; i<chk->mesh->n_face; i++)
		{
			for(j=i; j<chk->mesh->n_face; j++)
				if(chk->mesh->face_tex[j]!=chk->mesh->face_tex[i])
					break;

			k=j-i;

			tn=LBXGL_Voxel_MapTexture(chk, chk->mesh->face_tex[i]);
			LBXGL_Shader_BindTexture(tn);

#if 1
			if(chk->mesh->vbo_face>0)
			{
//				pdglBindBuffer(GL_ARRAY_BUFFER, chk->mesh->vbo_face);
				LBXGL_Shader_DrawArraysNormalTexRGB(
					PDGL_TRIANGLES, i*3, k*3,
					3, GL_FLOAT, 0, (byte *)chk->mesh->vbo_face_xyz,
					2, GL_FLOAT, 0, (byte *)chk->mesh->vbo_face_st,
					3, GL_BYTE, 3, (byte *)chk->mesh->vbo_face_norm,
					4, GL_UNSIGNED_BYTE, 0, rgba2);
			}else
			{
				LBXGL_Shader_DrawArraysNormalTexRGB(
					PDGL_TRIANGLES, i*3, k*3,
					3, GL_FLOAT, 0, chk->mesh->face_xyz,
					2, GL_FLOAT, 0, chk->mesh->face_st,
					3, GL_BYTE, 3, chk->mesh->face_norm,
					4, GL_UNSIGNED_BYTE, 0, rgba);
			}
#endif
			
			i+=k-1;
		}

		if(chk->mesh->vbo_face>0)
			{ pdglBindBuffer(GL_ARRAY_BUFFER, 0); }
		
//		if(chk->flags&VOX_CHFL_ALPHALIGHTMAT)
//			LBXGL_Voxel_DrawChunkLight_AlphaLightMat(chk, light);
		
		return;
	}
#endif


#if 0
	if(chk->flags&VOX_CHFL_USESHADER)
//	if(true)
	{
//		LBXGL_Shader_BlendFunc(GL_SRC_COLOR, GL_ONE);
//		LBXGL_Shader_BlendFunc(GL_DST_COLOR, GL_ZERO);
		LBXGL_Shader_NormalX3f(0, 0, 0);
		LBXGL_Shader_NormalY3f(0, 0, 0);

		if(light->flags&LBXGL_LFL_SUN)
//		if(1)
			rgba=chk->mesh->face_slrgba;
		else rgba=chk->mesh->face_rgba;
		useva=false;

		//LBXGL_Shader_Color4f(1,1,1,1);
		for(i=0; i<chk->mesh->n_face; i++)
		{
			fl=BTGE_Texture_GetImageFlags(chk->mesh->face_tex[i]);
			if(!(fl&BTGE_TXFL_LIGHTMAT))
			{
				for(j=i; j<chk->mesh->n_face; j++)
					if(chk->mesh->face_tex[j]!=chk->mesh->face_tex[i])
						break;
				i=j-1;
				useva=true;
				continue;
			}

			tn=LBXGL_Voxel_MapTexture(chk, chk->mesh->face_tex[i]);
			LBXGL_Shader_BindTexture(tn);
//			LBXGL_Shader_BindTexture(chk->mesh->face_tex[i]);

			LBXGL_Shader_BeginLight(PDGL_TRIANGLES);
			for(j=i; j<chk->mesh->n_face; j++)
			{
				if(chk->mesh->face_tex[j]!=chk->mesh->face_tex[i])
					break;

				for(k=0; k<3; k++)
				{
#ifndef BTGE_VOXEL_BYTERGBA
					LBXGL_Shader_Normal3fv(chk->mesh->face_norm+(j*3+k)*3);
					LBXGL_Shader_Color4fv(rgba+(j*3+k)*4);
#else
					LBXGL_Shader_Normal3sbv(chk->mesh->face_norm+(j*3+k)*3);
					LBXGL_Shader_Color4ubv(rgba+(j*3+k)*4);
#endif

//					LBXGL_Shader_Normal3fv(chk->mesh->face_norm+(j*4+k)*3);
					LBXGL_Shader_TexCoord2fv(chk->mesh->face_st+(j*3+k)*2);
					LBXGL_Shader_Vertex3fv(chk->mesh->face_xyz+(j*3+k)*3);
				}
			}
			LBXGL_Shader_EndLight();
			i=j-1;
		}

#if 1
		if(useva)
		{
			LBXGL_Shader_BindTexture(-1);

			for(i=0; i<chk->mesh->n_face; i++)
			{
				fl=BTGE_Texture_GetImageFlags(chk->mesh->face_tex[i]);
				if(fl&BTGE_TXFL_LIGHTMAT)
				{
					for(j=i; j<chk->mesh->n_face; j++)
						if(chk->mesh->face_tex[j]!=chk->mesh->face_tex[i])
							break;
					i=j-1;
					continue;
				}

				for(j=i; j<chk->mesh->n_face; j++)
					if(chk->mesh->face_tex[j]!=chk->mesh->face_tex[i])
						break;

				k=j-i;

				LBXGL_Shader_DrawArraysNormalFlatRGB(
					PDGL_TRIANGLES, i*3, k*3,
					3, GL_FLOAT, 0, chk->mesh->face_xyz,
					3, GL_BYTE, 3, chk->mesh->face_norm,
					4, GL_UNSIGNED_BYTE, 0, rgba);

				i+=k-1;
			}
		}
#endif

		return;
	}
#endif

	LBXGL_Shader_BindTexture(-1);

	if(light->flags&LBXGL_LFL_SUN)
	{
#if 1
		if(chk->mesh->vbo_face>0)
		{
			pdglBindBuffer(GL_ARRAY_BUFFER, chk->mesh->vbo_face);
			LBXGL_Shader_DrawArraysNormalFlatRGB(
				PDGL_TRIANGLES, 0, chk->mesh->n_face*3,
				3, GL_FLOAT, 0, (byte *)chk->mesh->vbo_face_xyz,
				3, GL_BYTE, 3, (byte *)chk->mesh->vbo_face_norm,
				4, GL_UNSIGNED_BYTE, 0, (byte *)chk->mesh->vbo_face_slrgba);
			pdglBindBuffer(GL_ARRAY_BUFFER, 0);
		}else
		{
			LBXGL_Shader_DrawArraysNormalFlatRGB(
				PDGL_TRIANGLES, 0, chk->mesh->n_face*3,
				3, GL_FLOAT, 0, chk->mesh->face_xyz,
				3, GL_BYTE, 3, chk->mesh->face_norm,
				4, GL_UNSIGNED_BYTE, 0, chk->mesh->face_slrgba);
		}
#endif
		return;
	}

#if 1
	if(chk->mesh->vbo_face>0)
	{
		pdglBindBuffer(GL_ARRAY_BUFFER, chk->mesh->vbo_face);
		LBXGL_Shader_DrawArraysNormalFlatRGB(
			PDGL_TRIANGLES, 0, chk->mesh->n_face*3,
			3, GL_FLOAT, 0, (byte *)chk->mesh->vbo_face_xyz,
			3, GL_BYTE, 3, (byte *)chk->mesh->vbo_face_norm,
			4, GL_UNSIGNED_BYTE, 0, (byte *)chk->mesh->vbo_face_rgba);
		pdglBindBuffer(GL_ARRAY_BUFFER, 0);
	}else
	{
		LBXGL_Shader_DrawArraysNormalFlatRGB(
			PDGL_TRIANGLES, 0, chk->mesh->n_face*3,
			3, GL_FLOAT, 0, chk->mesh->face_xyz,
			3, GL_BYTE, 3, chk->mesh->face_norm,
			4, GL_UNSIGNED_BYTE, 0, chk->mesh->face_rgba);
	}
#endif
}

void LBXGL_Voxel_DrawChunkShadow(
	LBXGL_VoxelChunk *chk, LBXGL_Light *light)
{
	static float *xyzarr=NULL;
	static int nxyz=0;

	float txyza[4*3], txyzb[4*3];
	float tv0[3], tv1[3], tv2[3], tv3[3];
	float dv0[3], dv1[3], lorg[3];
	float *xyz, *v0;

	float f, g, h;
	int i, j, k, l, n, i0, i1, nva;

	if(lbxgl_voxel_disablevbo)
		return;

//	i=btCvarGetf("r_disablevbo");

	return;

//	if(light->flags&LBXGL_LFL_SUN)
	if(light->flags&LBXGL_LFL_SHADOWMAP)
		return;

	if(!chk->mesh->n_face)
		return;

	if(chk->flags&VOX_CHFL_VISCLIP)
		return;

	if(!LBXGL_Voxel_CheckChunkLight(chk, light))
		return;

//	if(!LBXGL_Brush_CheckBBoxFrustumDistance(mins, maxs, 1024))
//		return;
	if(LBXGL_Voxel_GetChunkFrustumDistance(chk)>=1024)
		return;

//	n=chk->mesh->n_face*6*4;
	n=chk->mesh->n_face*(6+6*4);
	if(!xyzarr || (n>nxyz))
	{
		xyzarr=realloc(xyzarr, n*3*sizeof(float));
		nxyz=n;
	}

	h=0.01*(256.0/1024);
	if(h>0.1)h=0.1;

	lorg[0]=light->org[0]+lbxgl_voxel_reforg[0];
	lorg[1]=light->org[1]+lbxgl_voxel_reforg[1];
	lorg[2]=light->org[2]+lbxgl_voxel_reforg[2];
	g=LBXGL_Shadow_LightGammaRadius(light);
//	g=99999;

	xyz=xyzarr;
	for(i=0; i<chk->mesh->n_face; i++)
	{
		for(j=0; j<3; j++)
		{
			v0=chk->mesh->face_xyz+(i*3+j)*3;
			V3F_COPY(v0, tv0);
//			V3F_SCALEADDSCALE(tv0, (1.0-h), brush->org, h, tv0);
//			V3F_SUB(tv0, light->org, dv0);
			V3F_SUB(tv0, lorg, dv0);
			V3F_FRNORMALIZE(dv0, dv0);
			V3F_ADD(tv0, lbxgl_cam->fw, tv0);
			V3F_ADDSCALE(tv0, dv0, 1, txyza+j*3);
			V3F_ADDSCALE(tv0, dv0, 2*g, txyzb+j*3);
		}

#ifndef BTGE_VOXEL_BYTERGBA
//		v0=chk->mesh->face_norm+(i*4)*3;
		V3F_COPY(chk->mesh->face_norm+(i*3)*3, tv0);
		f=V3F_DOT(light->org, tv0)-V3F_DOT(txyza, tv0);
//		f=-f;
#else
		tv0[0]=(chk->mesh->face_norm+(i*3)*3)[0]*(1.0/127.0);
		tv0[1]=(chk->mesh->face_norm+(i*3)*3)[1]*(1.0/127.0);
		tv0[2]=(chk->mesh->face_norm+(i*3)*3)[2]*(1.0/127.0);
		f=V3F_DOT(light->org, tv0)-V3F_DOT(txyza, tv0);
#endif

		for(j=0; j<3; j++)
		{
			k=(j+1)%3;
			
			if(f>10)
			{
				V3F_COPY(txyza+k*3, xyz+0*3);
				V3F_COPY(txyza+j*3, xyz+1*3);
				V3F_COPY(txyzb+j*3, xyz+2*3);

				V3F_COPY(txyzb+j*3, xyz+3*3);
				V3F_COPY(txyzb+k*3, xyz+4*3);
				V3F_COPY(txyza+k*3, xyz+5*3);
				xyz+=6*3;
			}else if(f<10)
			{
				V3F_COPY(txyza+k*3, xyz+5*3);
				V3F_COPY(txyza+j*3, xyz+4*3);
				V3F_COPY(txyzb+j*3, xyz+3*3);

				V3F_COPY(txyzb+j*3, xyz+2*3);
				V3F_COPY(txyzb+k*3, xyz+1*3);
				V3F_COPY(txyza+k*3, xyz+0*3);

				xyz+=6*3;
			}
		}
	}

	i=(xyz-xyzarr)/3;
	if(i>n)
	{
		*(int *)-1=-1;
	}

	LBXGL_Shader_DrawArraysSimpleFlat(
		PDGL_TRIANGLES, 0, i,
		3, GL_FLOAT, 0, xyzarr);
}

void LBXGL_Voxel_DrawChunkForLight(
	LBXGL_VoxelChunk *chk, LBXGL_Light *light)
{
	byte *rgba;
	byte *rgba2;
	bool useva;
	int i, j, k, tn, fl;

	if(!chk->mesh->n_face)
		return;
//	if(chk->flags&VOX_CHFL_VISCLIP)
//		return;

	if(!LBXGL_Voxel_CheckChunkLight(chk, light))
		return;

#if 1
//	if(chk->flags&VOX_CHFL_USESHADER)
	if((chk->flags&VOX_CHFL_USESHADER) && !(chk->flags&VOX_CHFL_DISTANT))
	{
		LBXGL_Shader_NormalX3f(0, 0, 0);
		LBXGL_Shader_NormalY3f(0, 0, 0);

		if(light->flags&LBXGL_LFL_SUN)
			rgba=chk->mesh->face_slrgba;
		else rgba=chk->mesh->face_rgba;

		if(light->flags&LBXGL_LFL_SUN)
			rgba2=(byte *)chk->mesh->vbo_face_slrgba;
		else rgba2=(byte *)chk->mesh->vbo_face_rgba;

		if(chk->mesh->vbo_face>0)
			{ pdglBindBuffer(GL_ARRAY_BUFFER, chk->mesh->vbo_face); }

		for(i=0; i<chk->mesh->n_face; i++)
		{
#if 1
//			if(BTGE_Voxel_VoxelAlphaP(&(chk->mesh->face_vox[i])))
			if(BTGE_Voxel_VoxelNoShadowP(&(chk->mesh->face_vox[i])))
			{
				for(j=i; j<chk->mesh->n_face; j++)
					if(chk->mesh->face_tex[j]!=chk->mesh->face_tex[i])
						break;
				i=j;
				continue;
			}
#endif

			for(j=i; j<chk->mesh->n_face; j++)
				if(chk->mesh->face_tex[j]!=chk->mesh->face_tex[i])
					break;

			k=j-i;

			tn=LBXGL_Voxel_MapTexture(chk, chk->mesh->face_tex[i]);
			LBXGL_Shader_BindTexture(tn);

			if(chk->mesh->vbo_face>0)
			{
				LBXGL_Shader_DrawArraysNormalTexRGB(
					PDGL_TRIANGLES, i*3, k*3,
					3, GL_FLOAT, 0, (byte *)chk->mesh->vbo_face_xyz,
					2, GL_FLOAT, 0, (byte *)chk->mesh->vbo_face_st,
					3, GL_BYTE, 3, (byte *)chk->mesh->vbo_face_norm,
					4, GL_UNSIGNED_BYTE, 0, rgba2);
			}else
			{
				LBXGL_Shader_DrawArraysNormalTexRGB(
					PDGL_TRIANGLES, i*3, k*3,
					3, GL_FLOAT, 0, (byte *)chk->mesh->face_xyz,
					2, GL_FLOAT, 0, (byte *)chk->mesh->face_st,
					3, GL_BYTE, 3, (byte *)chk->mesh->face_norm,
					4, GL_UNSIGNED_BYTE, 0, rgba);
			}

			i+=k-1;
		}

		if(chk->mesh->vbo_face>0)
			{ pdglBindBuffer(GL_ARRAY_BUFFER, 0); }
		return;
	}
#endif

	LBXGL_Shader_BindTexture(-1);

	if(light->flags&LBXGL_LFL_SUN)
	{
		if(chk->mesh->vbo_face>0)
		{
			pdglBindBuffer(GL_ARRAY_BUFFER, chk->mesh->vbo_face);
			LBXGL_Shader_DrawArraysNormalFlatRGB(
				PDGL_TRIANGLES, 0, chk->mesh->n_face*3,
				3, GL_FLOAT, 0, (byte *)chk->mesh->vbo_face_xyz,
				3, GL_BYTE, 3, (byte *)chk->mesh->vbo_face_norm,
				4, GL_UNSIGNED_BYTE, 0, (byte *)chk->mesh->vbo_face_slrgba);
			pdglBindBuffer(GL_ARRAY_BUFFER, 0);
		}else
		{
			LBXGL_Shader_DrawArraysNormalFlatRGB(
				PDGL_TRIANGLES, 0, chk->mesh->n_face*3,
				3, GL_FLOAT, 0, (byte *)chk->mesh->face_xyz,
				3, GL_BYTE, 3, (byte *)chk->mesh->face_norm,
				4, GL_UNSIGNED_BYTE, 0, (byte *)chk->mesh->face_slrgba);
		}
		return;
	}

	if(chk->mesh->vbo_face>0)
	{
		pdglBindBuffer(GL_ARRAY_BUFFER, chk->mesh->vbo_face);
		LBXGL_Shader_DrawArraysNormalFlatRGB(
			PDGL_TRIANGLES, 0, chk->mesh->n_face*3,
			3, GL_FLOAT, 0, (byte *)chk->mesh->vbo_face_xyz,
			3, GL_BYTE, 3, (byte *)chk->mesh->vbo_face_norm,
			4, GL_UNSIGNED_BYTE, 0, (byte *)chk->mesh->vbo_face_rgba);
		pdglBindBuffer(GL_ARRAY_BUFFER, 0);
	}else
	{
		LBXGL_Shader_DrawArraysNormalFlatRGB(
			PDGL_TRIANGLES, 0, chk->mesh->n_face*3,
			3, GL_FLOAT, 0, (byte *)chk->mesh->face_xyz,
			3, GL_BYTE, 3, (byte *)chk->mesh->face_norm,
			4, GL_UNSIGNED_BYTE, 0, (byte *)chk->mesh->face_rgba);
	}
}

void LBXGL_Voxel_DrawChunkStaticLight(
	LBXGL_VoxelChunk *chk)
{
	byte *rgba;
	byte *rgba2;
	bool useva;
	int i, j, k, tn, fl;

	if(!chk->mesh->n_face)
		return;

#if 1
	if(1)
	{
		LBXGL_Shader_NormalX3f(0, 0, 0);
		LBXGL_Shader_NormalY3f(0, 0, 0);

		rgba=chk->mesh->face_tlrgba;
		rgba2=(byte *)chk->mesh->vbo_face_tlrgba;

		if(chk->mesh->vbo_face>0)
			{ pdglBindBuffer(GL_ARRAY_BUFFER, chk->mesh->vbo_face); }

		for(i=0; i<chk->mesh->n_face; i++)
		{
			for(j=i; j<chk->mesh->n_face; j++)
				if(chk->mesh->face_tex[j]!=chk->mesh->face_tex[i])
					break;

			k=j-i;

			tn=LBXGL_Voxel_MapTexture(chk, chk->mesh->face_tex[i]);
			LBXGL_Shader_BindTexture(tn);

			if(chk->mesh->vbo_face>0)
			{
				LBXGL_Shader_DrawArraysNormalTexRGB(
					PDGL_TRIANGLES, i*3, k*3,
					3, GL_FLOAT, 0, (byte *)chk->mesh->vbo_face_xyz,
					2, GL_FLOAT, 0, (byte *)chk->mesh->vbo_face_st,
					3, GL_BYTE, 3, (byte *)chk->mesh->vbo_face_norm,
					4, GL_UNSIGNED_BYTE, 0, rgba2);
			}else
			{
				LBXGL_Shader_DrawArraysNormalTexRGB(
					PDGL_TRIANGLES, i*3, k*3,
					3, GL_FLOAT, 0, (byte *)chk->mesh->face_xyz,
					2, GL_FLOAT, 0, (byte *)chk->mesh->face_st,
					3, GL_BYTE, 3, (byte *)chk->mesh->face_norm,
					4, GL_UNSIGNED_BYTE, 0, rgba);
			}

			i+=k-1;
		}

		if(chk->mesh->vbo_face>0)
			{ pdglBindBuffer(GL_ARRAY_BUFFER, 0); }
		return;
	}
#endif
}

void LBXGL_Voxel_DrawChunkFinal(LBXGL_VoxelChunk *chk)
{
	int i, j, k, tn;

	if(!chk->mesh->n_face)
		return;
	if(chk->flags&VOX_CHFL_VISCLIP)
		return;

//	LBXGL_Shader_BlendFunc(GL_DST_COLOR, GL_ZERO);
//	LBXGL_Shader_BlendFunc(GL_DST_COLOR, GL_ONE);
//	LBXGL_Shader_BlendFunc(GL_SRC_COLOR, GL_ONE);

	LBXGL_Shader_BlendFunc(GL_DST_COLOR, GL_ONE_MINUS_SRC_ALPHA);
	LBXGL_Shader_Color4f(1,1,1,1);

#if 0
	if(chk->flags&VOX_CHFL_USESHADER)
	{
//		LBXGL_Shader_BlendFunc(GL_SRC_COLOR, GL_ONE);
//		LBXGL_Shader_BlendFunc(GL_DST_COLOR, GL_ZERO);

		//LBXGL_Shader_Color4f(1,1,1,1);
		for(i=0; i<chk->mesh->n_face; i++)
		{
			tn=LBXGL_Voxel_MapTexture(chk, chk->mesh->face_tex[i]);
			LBXGL_Shader_BindTexture(tn);
//			LBXGL_Shader_BindTexture(chk->mesh->face_tex[i]);

			LBXGL_Shader_Begin(PDGL_TRIANGLES);
			for(j=i; j<chk->mesh->n_face; j++)
			{
				if(chk->mesh->face_tex[j]!=chk->mesh->face_tex[i])
					break;

				for(k=0; k<3; k++)
				{
#ifndef BTGE_VOXEL_BYTERGBA
					LBXGL_Shader_Normal3fv(chk->mesh->face_norm+(j*3+k)*3);
#else
					LBXGL_Shader_Normal3sbv(chk->mesh->face_norm+(j*3+k)*3);
#endif

//					LBXGL_Shader_Normal3fv(chk->mesh->face_norm+(j*3+k)*3);
					LBXGL_Shader_TexCoord2fv(chk->mesh->face_st+(j*3+k)*2);
					LBXGL_Shader_Vertex3fv(chk->mesh->face_xyz+(j*3+k)*3);
				}
			}
			LBXGL_Shader_End();
			i=j-1;
		}

//		LBXGL_Shader_BlendFunc(GL_SRC_COLOR, GL_ONE);
//		LBXGL_Shader_BlendFunc(GL_DST_COLOR, GL_ZERO);
		LBXGL_Shader_Color4f(1,1,1,1);
		return;
	}
#endif

#if 1
	if(chk->mesh->vbo_face>0)
		{ pdglBindBuffer(GL_ARRAY_BUFFER, chk->mesh->vbo_face); }
	for(i=0; i<chk->mesh->n_face; i++)
	{
		for(j=i; j<chk->mesh->n_face; j++)
			if(chk->mesh->face_tex[j]!=chk->mesh->face_tex[i])
				break;

		k=j-i;

//		tn=LBXGL_Voxel_MapTexture2(chk, chk->mesh->face_tex[i]);
		tn=LBXGL_Voxel_MapTexture(chk, chk->mesh->face_tex[i]);
		LBXGL_Shader_BindTexture(tn);

#if 1
		if(chk->mesh->vbo_face>0)
		{
//			pdglBindBuffer(GL_ARRAY_BUFFER, chk->mesh->vbo_face);
			LBXGL_Shader_DrawArraysNormalTex(
				PDGL_TRIANGLES, i*3, k*3,
				3, GL_FLOAT, 0, (byte *)chk->mesh->vbo_face_xyz,
				2, GL_FLOAT, 0, (byte *)chk->mesh->vbo_face_st,
				3, GL_BYTE, 3, (byte *)chk->mesh->vbo_face_norm);
//			pdglBindBuffer(GL_ARRAY_BUFFER, 0);
		}else
		{
			LBXGL_Shader_DrawArraysNormalTex(PDGL_TRIANGLES, i*3, k*3,
				3, GL_FLOAT, 0, chk->mesh->face_xyz,
				2, GL_FLOAT, 0, chk->mesh->face_st,
				3, GL_BYTE, 3, chk->mesh->face_norm);
		}
#endif

		i+=k-1;
	}
	if(chk->mesh->vbo_face>0)
		{ pdglBindBuffer(GL_ARRAY_BUFFER, 0); }
#endif
}

void LBXGL_Voxel_DrawChunkFinalRGB(LBXGL_VoxelChunk *chk)
{
	int i, j, k, tn;

	if(!chk->mesh->n_face)
		return;
	if(chk->flags&VOX_CHFL_VISCLIP)
		return;

//	if(!LBXGL_Voxel_CheckChunkFrustum(chk))
//		return;

#if 1
	if(chk->mesh->vbo_face>0)
		{ pdglBindBuffer(GL_ARRAY_BUFFER, chk->mesh->vbo_face); }
	for(i=0; i<chk->mesh->n_face; i++)
	{
		for(j=i; j<chk->mesh->n_face; j++)
			if(chk->mesh->face_tex[j]!=chk->mesh->face_tex[i])
				break;

		k=j-i;

//		tn=LBXGL_Voxel_MapTexture2(chk, chk->mesh->face_tex[i]);
		tn=LBXGL_Voxel_MapTexture(chk, chk->mesh->face_tex[i]);
		LBXGL_Shader_BindTexture(tn);

#if 1
		if(chk->mesh->vbo_face>0)
		{
//			pdglBindBuffer(GL_ARRAY_BUFFER, chk->mesh->vbo_face);
			LBXGL_Shader_DrawArraysNormalTexRGB(
				PDGL_TRIANGLES, i*3, k*3,
				3, GL_FLOAT, 0, (byte *)chk->mesh->vbo_face_xyz,
				2, GL_FLOAT, 0, (byte *)chk->mesh->vbo_face_st,
				3, GL_BYTE, 3, (byte *)chk->mesh->vbo_face_norm,
				4, GL_UNSIGNED_BYTE, 0, (byte *)chk->mesh->vbo_face_rgba);
		}else
		{
			LBXGL_Shader_DrawArraysNormalTexRGB(PDGL_TRIANGLES, i*3, k*3,
				3, GL_FLOAT, 0, chk->mesh->face_xyz,
				2, GL_FLOAT, 0, chk->mesh->face_st,
				3, GL_BYTE, 3, chk->mesh->face_norm,
				4, GL_UNSIGNED_BYTE, 0, chk->mesh->face_rgba);
		}
#endif

		i+=k-1;
	}

	if(chk->mesh->vbo_face>0)
		{ pdglBindBuffer(GL_ARRAY_BUFFER, 0); }
#endif
}

void LBXGL_Voxel_DrawChunkFinal_AlphaLightMat(LBXGL_VoxelChunk *chk)
{
	int i, j, k, tn, fl;

	if(!chk->mesh->n_aface)
		return;

	if(lbxgl_world->flags&
			(BTGE_WFL_LIGHTBUDGET|BTGE_WFL_ALPHABUDGET))
		return;

	if(chk->flags&VOX_CHFL_ALPHAVISCLIP)
		return;
	if(!(chk->flags&VOX_CHFL_ALPHALIGHTMAT))
		return;
	if(chk->flags&VOX_CHFL_DISTANT)
		return;

//	LBXGL_Shader_BlendFunc(GL_DST_COLOR, GL_ZERO);
//	LBXGL_Shader_BlendFunc(GL_DST_COLOR, GL_ONE);
//	LBXGL_Shader_BlendFunc(GL_SRC_COLOR, GL_ONE);

	LBXGL_Shader_BlendFunc(GL_DST_COLOR, GL_ONE_MINUS_SRC_ALPHA);
	LBXGL_Shader_Color4f(1,1,1,1);

#if 1
	if(chk->mesh->vbo_aface>0)
		{ pdglBindBuffer(GL_ARRAY_BUFFER, chk->mesh->vbo_aface); }
	for(i=0; i<chk->mesh->n_aface; i++)
	{
		for(j=i; j<chk->mesh->n_aface; j++)
			if(chk->mesh->aface_tex[j]!=chk->mesh->aface_tex[i])
				break;
		fl=BTGE_Texture_GetImageFlags(chk->mesh->aface_tex[i]);
		if(!(fl&BTGE_TXFL_LIGHTMAT)) { i=j-1; continue; }

		k=j-i;

//		tn=LBXGL_Voxel_MapTexture2(chk, chk->mesh->aface_tex[i]);
		tn=LBXGL_Voxel_MapTexture(chk, chk->mesh->aface_tex[i]);
		LBXGL_Shader_BindTexture(tn);

		if(chk->mesh->vbo_aface>0)
		{
			LBXGL_Shader_DrawArraysNormalTex(
				PDGL_TRIANGLES, i*3, k*3,
				3, GL_FLOAT, 0, (byte *)chk->mesh->vbo_aface_xyz,
				2, GL_FLOAT, 0, (byte *)chk->mesh->vbo_aface_st,
				3, GL_BYTE, 3, (byte *)chk->mesh->vbo_aface_norm);
		}else
		{
			LBXGL_Shader_DrawArraysNormalTex(
				PDGL_TRIANGLES, i*3, k*3,
				3, GL_FLOAT, 0, (byte *)chk->mesh->aface_xyz,
				2, GL_FLOAT, 0, (byte *)chk->mesh->aface_st,
				3, GL_BYTE, 3, (byte *)chk->mesh->aface_norm);
		}

		i+=k-1;
	}
	if(chk->mesh->vbo_aface>0)
		{ pdglBindBuffer(GL_ARRAY_BUFFER, 0); }
#endif
}

void LBXGL_Voxel_DrawChunkBasic(LBXGL_VoxelChunk *chk)
{
	int i, j, k, tn;

	if(!chk->mesh->n_face)
		return;
	if(chk->flags&VOX_CHFL_VISCLIP)
		return;

	if(chk->mesh->vbo_face>0)
		{ pdglBindBuffer(GL_ARRAY_BUFFER, chk->mesh->vbo_face); }

	for(i=0; i<chk->mesh->n_face; i++)
	{
		for(j=i; j<chk->mesh->n_face; j++)
			if(chk->mesh->face_tex[j]!=chk->mesh->face_tex[i])
				break;

		k=j-i;

//		tn=LBXGL_Voxel_MapTexture2(chk, chk->mesh->face_tex[i]);
		tn=LBXGL_Voxel_MapTexture(chk, chk->mesh->face_tex[i]);
		LBXGL_Shader_BindTexture(tn);

		if(chk->mesh->vbo_face>0)
		{
//			pdglBindBuffer(GL_ARRAY_BUFFER, chk->mesh->vbo_face);
			LBXGL_Shader_DrawArraysNormalTex(
				PDGL_TRIANGLES, i*3, k*3,
				3, GL_FLOAT, 0, (byte *)chk->mesh->vbo_face_xyz,
				2, GL_FLOAT, 0, (byte *)chk->mesh->vbo_face_st,
				3, GL_BYTE, 3, (byte *)chk->mesh->vbo_face_norm);
		}else
		{
//			pdglBindBuffer(GL_ARRAY_BUFFER, chk->mesh->vbo_face);
			LBXGL_Shader_DrawArraysNormalTex(
				PDGL_TRIANGLES, i*3, k*3,
				3, GL_FLOAT, 0, (byte *)chk->mesh->face_xyz,
				2, GL_FLOAT, 0, (byte *)chk->mesh->face_st,
				3, GL_BYTE, 3, (byte *)chk->mesh->face_norm);
		}

		i+=k-1;
	}

	if(chk->mesh->vbo_face>0)
		{ pdglBindBuffer(GL_ARRAY_BUFFER, 0); }
}

void LBXGL_Voxel_DrawChunkAlphaRGB(LBXGL_VoxelChunk *chk)
{
	int i, j, k, tn, fl;

//	if(!chk->mesh->n_face && !chk->mesh->n_aface)
	if(!chk->mesh->n_aface)
		return;
	if(chk->flags&VOX_CHFL_ALPHAVISCLIP)
		return;

	if(!LBXGL_Voxel_CheckChunkFrustum(chk))
		return;

#if 0
	for(i=0; i<chk->mesh->n_aface; i++)
	{
		if(BTGE_Voxel_VoxelFluidP(chk->mesh->aface_vox[i]))
			continue;

		tn=LBXGL_Voxel_MapTexture(chk, chk->mesh->aface_tex[i]);
		LBXGL_Shader_BindTexture(tn);

		LBXGL_Shader_Begin(PDGL_TRIANGLES);
		for(j=i; j<chk->mesh->n_aface; j++)
		{
			if(chk->mesh->aface_tex[j]!=chk->mesh->aface_tex[i])
				break;

			for(k=0; k<3; k++)
			{
#ifndef BTGE_VOXEL_BYTERGBA
				LBXGL_Shader_Color4fv(chk->mesh->aface_rgba+(j*3+k)*4);
				LBXGL_Shader_Normal3fv(chk->mesh->aface_norm+(j*3+k)*3);
#else
				LBXGL_Shader_Color4ubv(chk->mesh->aface_rgba+(j*3+k)*4);
				LBXGL_Shader_Normal3sbv(chk->mesh->aface_norm+(j*3+k)*3);
#endif
				LBXGL_Shader_TexCoord2fv(chk->mesh->aface_st+(j*3+k)*2);
				LBXGL_Shader_Vertex3fv(chk->mesh->aface_xyz+(j*3+k)*3);
			}
		}
		LBXGL_Shader_End();
		i=j-1;
	}
#endif

#if 1
	if(chk->mesh->vbo_aface>0)
		{ pdglBindBuffer(GL_ARRAY_BUFFER, chk->mesh->vbo_aface); }
	for(i=0; i<chk->mesh->n_aface; i++)
	{
//		if(BTGE_Voxel_VoxelFluidP(chk->mesh->aface_vox[i]))
		if(BTGE_Voxel_VoxelFluidP(&(chk->mesh->aface_vox[i])))
		{
			for(j=i; j<chk->mesh->n_aface; j++)
				if(chk->mesh->aface_tex[j]!=chk->mesh->aface_tex[i])
					break;
			i=j-1;
			continue;
		}

		for(j=i; j<chk->mesh->n_aface; j++)
			if(chk->mesh->aface_tex[j]!=chk->mesh->aface_tex[i])
				break;

#if 1
		fl=BTGE_Texture_GetImageFlags(chk->mesh->aface_tex[i]);
		if((fl&BTGE_TXFL_LIGHTMAT) && !(chk->flags&VOX_CHFL_DISTANT) &&
			!(lbxgl_world->flags&
				(BTGE_WFL_LIGHTBUDGET|BTGE_WFL_ALPHABUDGET)))
		{
			if(lbxgl_world->lighting==1)
				{ i=j-1; continue; }
		}
#endif
		k=j-i;

//		tn=LBXGL_Voxel_MapTexture2(chk, chk->mesh->face_tex[i]);
		tn=LBXGL_Voxel_MapTexture(chk, chk->mesh->aface_tex[i]);
		LBXGL_Shader_BindTexture(tn);

#if 1
		if(chk->mesh->vbo_aface>0)
		{
			pdglBindBuffer(GL_ARRAY_BUFFER, chk->mesh->vbo_aface);
			LBXGL_Shader_DrawArraysNormalTexRGB(
				PDGL_TRIANGLES, i*3, k*3,
				3, GL_FLOAT, 0, (byte *)chk->mesh->vbo_aface_xyz,
				2, GL_FLOAT, 0, (byte *)chk->mesh->vbo_aface_st,
				3, GL_BYTE, 3, (byte *)chk->mesh->vbo_aface_norm,
				4, GL_UNSIGNED_BYTE, 0, (byte *)chk->mesh->vbo_aface_rgba);
		}else
		{
			LBXGL_Shader_DrawArraysNormalTexRGB(PDGL_TRIANGLES, i*3, k*3,
				3, GL_FLOAT, 0, chk->mesh->aface_xyz,
				2, GL_FLOAT, 0, chk->mesh->aface_st,
				3, GL_BYTE, 3, chk->mesh->aface_norm,
				4, GL_UNSIGNED_BYTE, 0, chk->mesh->aface_rgba);
		}
#endif

		i+=k-1;
	}

	if(chk->mesh->vbo_aface>0)
		{ pdglBindBuffer(GL_ARRAY_BUFFER, 0); }
#endif
}

void LBXGL_Voxel_DrawChunkSimpleAlphaFlat(LBXGL_VoxelChunk *chk)
{
	if(!chk->mesh->n_aface)
		return;
	LBXGL_Shader_DrawArraysSimpleFlat(PDGL_TRIANGLES,
		0, chk->mesh->n_aface*3,
		3, GL_FLOAT, 0, chk->mesh->aface_xyz);
}

void LBXGL_Voxel_DrawChunkAlphaFlat(LBXGL_VoxelChunk *chk)
{
//	if(!chk->mesh->n_face && !chk->mesh->n_aface)
	if(!chk->mesh->n_aface)
		return;
	if(chk->flags&VOX_CHFL_ALPHAVISCLIP)
		return;

	LBXGL_Shader_DrawArraysNormalFlat(PDGL_TRIANGLES,
		0, chk->mesh->n_aface*3,
		3, GL_FLOAT, 0, chk->mesh->aface_xyz,
		3, GL_BYTE, 3, chk->mesh->aface_norm);
}

void LBXGL_Voxel_DrawChunkAlphaFluidFlat(LBXGL_VoxelChunk *chk)
{
	int i, j, k, tn;

	if(!chk->mesh->n_aface)return;
	if(chk->flags&VOX_CHFL_ALPHAVISCLIP)
		return;
	if(!LBXGL_Voxel_CheckChunkFrustum(chk))return;

#if 1
	if(chk->mesh->vbo_aface>0)
		{ pdglBindBuffer(GL_ARRAY_BUFFER, chk->mesh->vbo_aface); }
	for(i=0; i<chk->mesh->n_aface; i++)
	{
//		if(!BTGE_Voxel_VoxelFluidP(chk->mesh->aface_vox[i]))
		if(!BTGE_Voxel_VoxelFluidP(&(chk->mesh->aface_vox[i])))
		{
			for(j=i; j<chk->mesh->n_aface; j++)
				if(chk->mesh->aface_tex[j]!=chk->mesh->aface_tex[i])
					break;
			i=j-1;
			continue;
		}

		for(j=i; j<chk->mesh->n_aface; j++)
			if(chk->mesh->aface_tex[j]!=chk->mesh->aface_tex[i])
				break;

		k=j-i;

//		tn=LBXGL_Voxel_MapTexture2(chk, chk->mesh->face_tex[i]);
//		tn=LBXGL_Voxel_MapTexture(chk, chk->mesh->aface_tex[i]);
//		LBXGL_Shader_BindTexture(tn);

#if 1
		if(chk->mesh->vbo_aface>0)
		{
			LBXGL_Shader_DrawArraysNormalFlat(
				PDGL_TRIANGLES, i*3, k*3,
				3, GL_FLOAT, 0, (byte *)chk->mesh->vbo_aface_xyz,
				3, GL_BYTE, 3, (byte *)chk->mesh->vbo_aface_norm);
		}else
		{
			LBXGL_Shader_DrawArraysNormalFlat(
				PDGL_TRIANGLES, i*3, k*3,
				3, GL_FLOAT, 0, (byte *)chk->mesh->aface_xyz,
				3, GL_BYTE, 3, (byte *)chk->mesh->aface_norm);
		}
#endif

		i+=k-1;
	}
	if(chk->mesh->vbo_aface>0)
		{ pdglBindBuffer(GL_ARRAY_BUFFER, 0); }
#endif

#if 0
	for(i=0; i<chk->mesh->n_aface; i++)
	{
		if(!BTGE_Voxel_VoxelFluidP(chk->mesh->aface_vox[i]))
			continue;
	
		pdglBegin(PDGL_TRIANGLES);
		for(j=i; j<chk->mesh->n_aface; j++)
		{
			if(chk->mesh->aface_tex[j]!=chk->mesh->aface_tex[i])
				break;

			for(k=0; k<3; k++)
			{
#ifndef BTGE_VOXEL_BYTERGBA
				pdglNormal3fv(chk->mesh->aface_norm+(j*3+k)*3);
#else
				pdglNormal3bv(chk->mesh->aface_norm+(j*3+k)*3);
#endif
				pdglVertex3fv(chk->mesh->aface_xyz+(j*3+k)*3);
			}
		}
		pdglEnd();
		i=j-1;
	}
#endif
}

void LBXGL_Voxel_DrawChunkAlphaFluid(LBXGL_VoxelChunk *chk)
{
	int i, j, k, tn;

	if(!chk->mesh->n_aface)
		return;
	if(chk->flags&VOX_CHFL_ALPHAVISCLIP)
		return;

	if(!LBXGL_Voxel_CheckChunkFrustum(chk))
		return;

#if 1
	if(chk->mesh->vbo_aface>0)
		{ pdglBindBuffer(GL_ARRAY_BUFFER, chk->mesh->vbo_aface); }
	for(i=0; i<chk->mesh->n_aface; i++)
	{
//		if(!BTGE_Voxel_VoxelFluidP(chk->mesh->aface_vox[i]))
		if(!BTGE_Voxel_VoxelFluidP(&(chk->mesh->aface_vox[i])))
		{
			for(j=i; j<chk->mesh->n_aface; j++)
				if(chk->mesh->aface_tex[j]!=chk->mesh->aface_tex[i])
					break;
			i=j-1;
			continue;
		}

		for(j=i; j<chk->mesh->n_aface; j++)
			if(chk->mesh->aface_tex[j]!=chk->mesh->aface_tex[i])
				break;

		k=j-i;

//		tn=LBXGL_Voxel_MapTexture2(chk, chk->mesh->face_tex[i]);
		tn=LBXGL_Voxel_MapTexture(chk, chk->mesh->aface_tex[i]);
		LBXGL_Shader_BindTexture(tn);

#if 1
		if(chk->mesh->vbo_aface>0)
		{
			LBXGL_Shader_DrawArraysNormalTexRGB(
				PDGL_TRIANGLES, i*3, k*3,
				3, GL_FLOAT, 0, (byte *)chk->mesh->vbo_aface_xyz,
				2, GL_FLOAT, 0, (byte *)chk->mesh->vbo_aface_st,
				3, GL_BYTE, 3, (byte *)chk->mesh->vbo_aface_norm,
				4, GL_UNSIGNED_BYTE, 0, (byte *)chk->mesh->vbo_aface_rgba);
		}else
		{
			LBXGL_Shader_DrawArraysNormalTexRGB(
				PDGL_TRIANGLES, i*3, k*3,
				3, GL_FLOAT, 0, (byte *)chk->mesh->aface_xyz,
				2, GL_FLOAT, 0, (byte *)chk->mesh->aface_st,
				3, GL_BYTE, 3, (byte *)chk->mesh->aface_norm,
				4, GL_UNSIGNED_BYTE, 0, (byte *)chk->mesh->aface_rgba);
		}
#endif

		i+=k-1;
	}
	if(chk->mesh->vbo_aface>0)
		{ pdglBindBuffer(GL_ARRAY_BUFFER, 0); }
#endif

#if 0
	for(i=0; i<chk->mesh->n_aface; i++)
	{
		if(!BTGE_Voxel_VoxelFluidP(chk->mesh->aface_vox[i]))
			continue;
	
		tn=LBXGL_Voxel_MapTexture(chk, chk->mesh->aface_tex[i]);
		LBXGL_Shader_BindTexture(tn);
//		LBXGL_Shader_BindTexture(chk->mesh->face_tex[i]);

		glDisable(GL_CULL_FACE);

//		tn=LBXGL_Texture_LoadImage("textures/shader/watery_base");
//		LBXGL_Shader_BindTexture(tn);
		LBXGL_Shader_Color4f(1, 1, 1, 1);

		LBXGL_Shader_Begin(PDGL_TRIANGLES);
		for(j=i; j<chk->mesh->n_aface; j++)
		{
			if(chk->mesh->aface_tex[j]!=chk->mesh->aface_tex[i])
				break;

			for(k=0; k<3; k++)
			{
//				LBXGL_Shader_Color4fv(chk->mesh->aface_rgba+(j*3+k)*4);
//				LBXGL_Shader_Normal3fv(chk->mesh->aface_norm+(j*3+k)*3);
#ifndef BTGE_VOXEL_BYTERGBA
				LBXGL_Shader_Color4fv(chk->mesh->aface_rgba+(j*3+k)*4);
				LBXGL_Shader_Normal3fv(chk->mesh->aface_norm+(j*3+k)*3);
#else
				LBXGL_Shader_Color4ubv(chk->mesh->aface_rgba+(j*3+k)*4);
				LBXGL_Shader_Normal3sbv(chk->mesh->aface_norm+(j*3+k)*3);
#endif

				LBXGL_Shader_TexCoord2fv(chk->mesh->aface_st+(j*3+k)*2);
				LBXGL_Shader_Vertex3fv(chk->mesh->aface_xyz+(j*3+k)*3);
			}
		}
		LBXGL_Shader_End();
		i=j-1;
	}
#endif
}

void LBXGL_Voxel_DrawChunkAlphaFluid2(LBXGL_VoxelChunk *chk)
{
	int i, j, k, tn;

	if(!chk->mesh->n_aface)
		return;
	if(chk->flags&VOX_CHFL_ALPHAVISCLIP)
		return;

	if(!LBXGL_Voxel_CheckChunkFrustum(chk))
		return;

#if 1
	tn=LBXGL_Texture_LoadImage("textures/shader/watery_base");
	LBXGL_Shader_BindTexture(tn);

	if(chk->mesh->vbo_aface>0)
		{ pdglBindBuffer(GL_ARRAY_BUFFER, chk->mesh->vbo_aface); }
	for(i=0; i<chk->mesh->n_aface; i++)
	{
//		if(!BTGE_Voxel_VoxelFluidOverlayP(chk->mesh->aface_vox[i]))
		if(!BTGE_Voxel_VoxelFluidOverlayP(&(chk->mesh->aface_vox[i])))
		{
			for(j=i; j<chk->mesh->n_aface; j++)
				if(chk->mesh->aface_tex[j]!=chk->mesh->aface_tex[i])
					break;
			i=j-1;
			continue;
		}

		for(j=i; j<chk->mesh->n_aface; j++)
			if(chk->mesh->aface_tex[j]!=chk->mesh->aface_tex[i])
				break;

		k=j-i;

//		tn=LBXGL_Voxel_MapTexture(chk, chk->mesh->aface_tex[i]);
//		tn=LBXGL_Texture_LoadImage("textures/shader/watery_base");
//		LBXGL_Shader_BindTexture(tn);

#if 1
		if(chk->mesh->vbo_aface>0)
		{
			LBXGL_Shader_DrawArraysNormalTexRGB(
				PDGL_TRIANGLES, i*3, k*3,
				3, GL_FLOAT, 0, (byte *)chk->mesh->vbo_aface_xyz,
				2, GL_FLOAT, 0, (byte *)chk->mesh->vbo_aface_st,
				3, GL_BYTE, 3, (byte *)chk->mesh->vbo_aface_norm,
				4, GL_UNSIGNED_BYTE, 0, (byte *)chk->mesh->vbo_aface_rgba);
		}else
		{
			LBXGL_Shader_DrawArraysNormalTexRGB(
				PDGL_TRIANGLES, i*3, k*3,
				3, GL_FLOAT, 0, (byte *)chk->mesh->aface_xyz,
				2, GL_FLOAT, 0, (byte *)chk->mesh->aface_st,
				3, GL_BYTE, 3, (byte *)chk->mesh->aface_norm,
				4, GL_UNSIGNED_BYTE, 0, (byte *)chk->mesh->aface_rgba);
		}
#endif

		i+=k-1;
	}
	if(chk->mesh->vbo_aface>0)
		{ pdglBindBuffer(GL_ARRAY_BUFFER, 0); }
#endif

#if 0
	for(i=0; i<chk->mesh->n_aface; i++)
	{
//		tn=LBXGL_Voxel_MapTexture(chk, chk->mesh->aface_tex[i]);
//		LBXGL_Shader_BindTexture(tn);
//		LBXGL_Shader_BindTexture(chk->mesh->face_tex[i]);

		if(!BTGE_Voxel_VoxelFluidOverlayP(chk->mesh->aface_vox[i]))
			continue;

		tn=LBXGL_Texture_LoadImage("textures/shader/watery_base");
		LBXGL_Shader_BindTexture(tn);
		LBXGL_Shader_Color4f(1, 1, 1, 1);

		glDisable(GL_CULL_FACE);

		LBXGL_Shader_Begin(PDGL_TRIANGLES);
		for(j=i; j<chk->mesh->n_aface; j++)
		{
			if(chk->mesh->aface_tex[j]!=chk->mesh->aface_tex[i])
				break;

			for(k=0; k<3; k++)
			{
//				LBXGL_Shader_Color4fv(chk->mesh->aface_rgba+(j*4+k)*4);
//				LBXGL_Shader_Normal3fv(chk->mesh->aface_norm+(j*4+k)*3);
#ifndef BTGE_VOXEL_BYTERGBA
				LBXGL_Shader_Color4fv(chk->mesh->aface_rgba+(j*3+k)*4);
				LBXGL_Shader_Normal3fv(chk->mesh->aface_norm+(j*3+k)*3);
#else
				LBXGL_Shader_Color4ubv(chk->mesh->aface_rgba+(j*3+k)*4);
				LBXGL_Shader_Normal3sbv(chk->mesh->aface_norm+(j*3+k)*3);
#endif
				LBXGL_Shader_TexCoord2fv(chk->mesh->aface_st+(j*3+k)*2);
				LBXGL_Shader_Vertex3fv(chk->mesh->aface_xyz+(j*3+k)*3);
			}
		}
		LBXGL_Shader_End();
		i=j-1;
	}
#endif
}

void LBXGL_Voxel_DrawBeginTranslateForRegion(LBXGL_VoxelRegion *rgn)
{
	float tv[3];

#if 1
	tv[0]=(rgn->ox*rgn->voxsz)-lbxgl_voxel_reforg[0];
	tv[1]=(rgn->oy*rgn->voxsz)-lbxgl_voxel_reforg[1];
	tv[2]=(rgn->oz*rgn->voxsz)-lbxgl_voxel_reforg[2];
	LBXGL_Shader_Translatef(tv[0], tv[1], tv[2]);
#endif

#if 0
	tv[0]=-lbxgl_voxel_reforg[0];
	tv[1]=-lbxgl_voxel_reforg[1];
	tv[2]=-lbxgl_voxel_reforg[2];
	LBXGL_Shader_Translatef(tv[0], tv[1], tv[2]);
#endif
}

void LBXGL_Voxel_DrawEndTranslateForRegion(LBXGL_VoxelRegion *rgn)
{
	float tv[3];

#if 1
	tv[0]=(rgn->ox*rgn->voxsz)-lbxgl_voxel_reforg[0];
	tv[1]=(rgn->oy*rgn->voxsz)-lbxgl_voxel_reforg[1];
	tv[2]=(rgn->oz*rgn->voxsz)-lbxgl_voxel_reforg[2];
	LBXGL_Shader_Translatef(-tv[0], -tv[1], -tv[2]);
#endif

#if 0
	tv[0]=-lbxgl_voxel_reforg[0];
	tv[1]=-lbxgl_voxel_reforg[1];
	tv[2]=-lbxgl_voxel_reforg[2];
	LBXGL_Shader_Translatef(-tv[0], -tv[1], -tv[2]);
#endif
}

void LBXGL_Voxel_DrawRegionLight(
	LBXGL_VoxelRegion *rgn, LBXGL_Light *light)
{
	LBXGL_VoxelChunk *cur;

	if(!LBXGL_Voxel_CheckRegionFrustum(rgn))return;
	if(!LBXGL_Voxel_CheckRegionLight(rgn, light))return;

	LBXGL_Voxel_DrawBeginTranslateForRegion(rgn);

	cur=rgn->vischk;
	while(cur)
	{
		LBXGL_Voxel_DrawChunkLight(cur, light);
		cur=cur->mesh->vnext;
	}

#if 1
	cur=rgn->avischk;
	while(cur)
	{
		LBXGL_Voxel_DrawChunkLight_AlphaLightMat(cur, light);
		cur=cur->mesh->avnext;
	}
#endif

	LBXGL_Voxel_DrawEndTranslateForRegion(rgn);
}

void LBXGL_Voxel_DrawRegionShadow(
	LBXGL_VoxelRegion *rgn, LBXGL_Light *light)
{
	LBXGL_VoxelChunk *cur;

	if(!LBXGL_Voxel_CheckRegionFrustum(rgn))return;
	if(!LBXGL_Voxel_CheckRegionLight(rgn, light))return;

	LBXGL_Voxel_DrawBeginTranslateForRegion(rgn);

	cur=rgn->svischk;
	while(cur)
	{
		LBXGL_Voxel_DrawChunkShadow(cur, light);
		cur=cur->mesh->svnext;
	}

	LBXGL_Voxel_DrawEndTranslateForRegion(rgn);
}

void LBXGL_Voxel_DrawRegionForLight(
	LBXGL_VoxelRegion *rgn, LBXGL_Light *light)
{
	LBXGL_VoxelChunk *cur;

//	if(!LBXGL_Voxel_CheckRegionFrustum(rgn))return;
	if(!LBXGL_Voxel_CheckRegionLight(rgn, light))return;

	LBXGL_Voxel_DrawBeginTranslateForRegion(rgn);

	cur=rgn->svischk;
	while(cur)
	{
		LBXGL_Voxel_DrawChunkForLight(cur, light);
		cur=cur->mesh->svnext;
	}

	LBXGL_Voxel_DrawEndTranslateForRegion(rgn);
}

void LBXGL_Voxel_DrawRegionFlat(LBXGL_VoxelRegion *rgn)
{
	LBXGL_VoxelChunk *cur;

	if(!LBXGL_Voxel_CheckRegionFrustum(rgn))
		return;

	LBXGL_Voxel_DrawBeginTranslateForRegion(rgn);

	cur=rgn->vischk;
	while(cur)
	{
		LBXGL_Voxel_DrawChunkFlat(cur);
		cur=cur->mesh->vnext;
	}

	LBXGL_Voxel_DrawEndTranslateForRegion(rgn);
}

void LBXGL_Voxel_DrawRegionFlatRGB(LBXGL_VoxelRegion *rgn)
{
	LBXGL_VoxelChunk *cur;

	if(!LBXGL_Voxel_CheckRegionFrustum(rgn))
		return;

	LBXGL_Voxel_DrawBeginTranslateForRegion(rgn);

	cur=rgn->vischk;
	while(cur)
	{
		LBXGL_Voxel_DrawChunkFlatRGB(cur);
		cur=cur->mesh->vnext;
	}

	LBXGL_Voxel_DrawEndTranslateForRegion(rgn);
}

void LBXGL_Voxel_DrawRegionFlatVLRGB(LBXGL_VoxelRegion *rgn)
{
	LBXGL_VoxelChunk *cur;

	if(!LBXGL_Voxel_CheckRegionFrustum(rgn))
		return;

	LBXGL_Voxel_DrawBeginTranslateForRegion(rgn);

	cur=rgn->vischk;
	while(cur)
	{
		LBXGL_Voxel_DrawChunkFlatVLRGB(cur);
		cur=cur->mesh->vnext;
	}

#if 1
	cur=rgn->avischk;
	while(cur)
	{
		LBXGL_Voxel_DrawChunkFlatVLRGB_AlphaLightMat(cur);
		cur=cur->mesh->avnext;
	}
#endif

	LBXGL_Voxel_DrawEndTranslateForRegion(rgn);
}

void LBXGL_Voxel_DrawRegionStaticLight(LBXGL_VoxelRegion *rgn)
{
	LBXGL_VoxelChunk *cur;

	if(!LBXGL_Voxel_CheckRegionFrustum(rgn))
		return;

	LBXGL_Voxel_DrawBeginTranslateForRegion(rgn);

	cur=rgn->vischk;
	while(cur)
	{
		LBXGL_Voxel_DrawChunkStaticLight(cur);
		cur=cur->mesh->vnext;
	}

	LBXGL_Voxel_DrawEndTranslateForRegion(rgn);
}

void LBXGL_Voxel_DrawRegionFinal(LBXGL_VoxelRegion *rgn)
{
	LBXGL_VoxelChunk *cur;

	if(!LBXGL_Voxel_CheckRegionFrustum(rgn))
		return;

	LBXGL_Voxel_DrawBeginTranslateForRegion(rgn);

	cur=rgn->vischk;
	while(cur)
	{
		LBXGL_Voxel_DrawChunkFinal(cur);
		cur=cur->mesh->vnext;
	}

#if 1
	cur=rgn->avischk;
	while(cur)
	{
		LBXGL_Voxel_DrawChunkFinal_AlphaLightMat(cur);
		cur=cur->mesh->avnext;
	}
#endif

	LBXGL_Voxel_DrawEndTranslateForRegion(rgn);
}

void LBXGL_Voxel_DrawRegionFinalRGB(LBXGL_VoxelRegion *rgn)
{
	LBXGL_VoxelChunk *cur;

	if(!LBXGL_Voxel_CheckRegionFrustum(rgn))
		return;

	LBXGL_Voxel_DrawBeginTranslateForRegion(rgn);

	cur=rgn->vischk;
	while(cur)
	{
		LBXGL_Voxel_DrawChunkFinalRGB(cur);
		cur=cur->mesh->vnext;
	}

	LBXGL_Voxel_DrawEndTranslateForRegion(rgn);
}

void LBXGL_Voxel_DrawRegionBasic(LBXGL_VoxelRegion *rgn)
{
	LBXGL_VoxelChunk *cur;

	if(!LBXGL_Voxel_CheckRegionFrustum(rgn))
		return;

	LBXGL_Voxel_DrawBeginTranslateForRegion(rgn);

	cur=rgn->vischk;
	while(cur)
	{
		LBXGL_Voxel_DrawChunkBasic(cur);
		cur=cur->mesh->vnext;
	}

	LBXGL_Voxel_DrawEndTranslateForRegion(rgn);
}

void LBXGL_Voxel_DrawRegionAlphaRGB(LBXGL_VoxelRegion *rgn)
{
	LBXGL_VoxelChunk *cur;

	if(!LBXGL_Voxel_CheckRegionFrustum(rgn))
		return;

	LBXGL_Voxel_DrawBeginTranslateForRegion(rgn);

	cur=rgn->avischk;
	while(cur)
	{
		LBXGL_Voxel_DrawChunkAlphaRGB(cur);
		cur=cur->mesh->avnext;
	}

	LBXGL_Voxel_DrawEndTranslateForRegion(rgn);
}

void LBXGL_Voxel_DrawRegionAlphaFlat(LBXGL_VoxelRegion *rgn)
{
	LBXGL_VoxelChunk *cur;

	if(!LBXGL_Voxel_CheckRegionFrustum(rgn))
		return;

	LBXGL_Voxel_DrawBeginTranslateForRegion(rgn);

	cur=rgn->avischk;
	while(cur)
	{
		LBXGL_Voxel_DrawChunkAlphaFlat(cur);
		cur=cur->mesh->avnext;
	}

	LBXGL_Voxel_DrawEndTranslateForRegion(rgn);
}

void LBXGL_Voxel_DrawRegionAlphaFluidFlat(LBXGL_VoxelRegion *rgn)
{
	LBXGL_VoxelChunk *cur;

	if(!LBXGL_Voxel_CheckRegionFrustum(rgn))
		return;

	LBXGL_Voxel_DrawBeginTranslateForRegion(rgn);

	cur=rgn->avischk;
	while(cur)
	{
		LBXGL_Voxel_DrawChunkAlphaFluidFlat(cur);
		cur=cur->mesh->avnext;
	}

	LBXGL_Voxel_DrawEndTranslateForRegion(rgn);
}

void LBXGL_Voxel_DrawRegionAlphaFluid(LBXGL_VoxelRegion *rgn)
{
	LBXGL_VoxelChunk *cur;

	if(!LBXGL_Voxel_CheckRegionFrustum(rgn))
		return;

	LBXGL_Voxel_DrawBeginTranslateForRegion(rgn);

	cur=rgn->avischk;
	while(cur)
	{
		LBXGL_Voxel_DrawChunkAlphaFluid(cur);
		cur=cur->mesh->avnext;
	}

	LBXGL_Voxel_DrawEndTranslateForRegion(rgn);
}

void LBXGL_Voxel_DrawRegionAlphaFluid2(LBXGL_VoxelRegion *rgn)
{
	LBXGL_VoxelChunk *cur;

	if(!LBXGL_Voxel_CheckRegionFrustum(rgn))
		return;

	LBXGL_Voxel_DrawBeginTranslateForRegion(rgn);

	cur=rgn->avischk;
	while(cur)
	{
		LBXGL_Voxel_DrawChunkAlphaFluid2(cur);
		cur=cur->mesh->avnext;
	}

	LBXGL_Voxel_DrawEndTranslateForRegion(rgn);
}

void LBXGL_Voxel_DrawRegionListFlat(LBXGL_VoxelRegion *lst)
{
	LBXGL_VoxelRegion *cur;

	cur=lst;
	while(cur)
	{
		LBXGL_Voxel_DrawRegionFlat(cur);
		cur=cur->next;
	}
}

void LBXGL_Voxel_DrawRegionListLight(
	LBXGL_VoxelRegion *lst, LBXGL_Light *light)
{
	LBXGL_VoxelRegion *cur;

	cur=lst;
	while(cur)
	{
		LBXGL_Voxel_DrawRegionLight(cur, light);
		cur=cur->next;
	}
}

void LBXGL_Voxel_DrawRegionListShadow(
	LBXGL_VoxelRegion *lst, LBXGL_Light *light)
{
	LBXGL_VoxelRegion *cur;

	cur=lst;
	while(cur)
	{
		LBXGL_Voxel_DrawRegionShadow(cur, light);
		cur=cur->next;
	}
}

void LBXGL_Voxel_DrawRegionListForLight(
	LBXGL_VoxelRegion *lst, LBXGL_Light *light)
{
	LBXGL_VoxelRegion *cur;

	cur=lst;
	while(cur)
	{
		LBXGL_Voxel_DrawRegionForLight(cur, light);
		cur=cur->next;
	}
}

void LBXGL_Voxel_DrawRegionListFlatRGB(LBXGL_VoxelRegion *lst)
{
	LBXGL_VoxelRegion *cur;

	cur=lst;
	while(cur)
	{
		LBXGL_Voxel_DrawRegionFlatRGB(cur);
		cur=cur->next;
	}
}

void LBXGL_Voxel_DrawRegionListFlatVLRGB(LBXGL_VoxelRegion *lst)
{
	LBXGL_VoxelRegion *cur;

	cur=lst;
	while(cur)
	{
		LBXGL_Voxel_DrawRegionFlatVLRGB(cur);
		cur=cur->next;
	}
}

void LBXGL_Voxel_DrawRegionListStaticLight(LBXGL_VoxelRegion *lst)
{
	LBXGL_VoxelRegion *cur;

	cur=lst;
	while(cur)
	{
		LBXGL_Voxel_DrawRegionStaticLight(cur);
		cur=cur->next;
	}
}

void LBXGL_Voxel_DrawRegionListFinal(LBXGL_VoxelRegion *lst)
{
	LBXGL_VoxelRegion *cur;

	cur=lst;
	while(cur)
	{
		LBXGL_Voxel_DrawRegionFinal(cur);
		cur=cur->next;
	}
}

void LBXGL_Voxel_DrawRegionListFinalRGB(LBXGL_VoxelRegion *lst)
{
	LBXGL_VoxelRegion *cur;

	cur=lst;
	while(cur)
	{
		LBXGL_Voxel_DrawRegionFinalRGB(cur);
		cur=cur->next;
	}
}

void LBXGL_Voxel_DrawRegionListBasic(LBXGL_VoxelRegion *lst)
{
	LBXGL_VoxelRegion *cur;

	cur=lst;
	while(cur)
	{
		LBXGL_Voxel_DrawRegionBasic(cur);
		cur=cur->next;
	}
}

void LBXGL_Voxel_DrawRegionListAlphaRGB(LBXGL_VoxelRegion *lst)
{
	LBXGL_VoxelRegion *cur;

	cur=lst;
	while(cur)
	{
		LBXGL_Voxel_DrawRegionAlphaRGB(cur);
		cur=cur->next;
	}
}

void LBXGL_Voxel_DrawRegionListAlphaFlat(LBXGL_VoxelRegion *lst)
{
	LBXGL_VoxelRegion *cur;

	cur=lst;
	while(cur)
	{
		LBXGL_Voxel_DrawRegionAlphaFlat(cur);
		cur=cur->next;
	}
}

void LBXGL_Voxel_DrawRegionListAlphaFluidFlat(LBXGL_VoxelRegion *lst)
{
	LBXGL_VoxelRegion *cur;

	cur=lst;
	while(cur)
	{
		LBXGL_Voxel_DrawRegionAlphaFluidFlat(cur);
		cur=cur->next;
	}
}

void LBXGL_Voxel_DrawRegionListAlphaFluid(LBXGL_VoxelRegion *lst)
{
	LBXGL_VoxelRegion *cur;

	cur=lst;
	while(cur)
	{
		LBXGL_Voxel_DrawRegionAlphaFluid(cur);
		cur=cur->next;
	}
}

void LBXGL_Voxel_DrawRegionListAlphaFluid2(LBXGL_VoxelRegion *lst)
{
	LBXGL_VoxelRegion *cur;

	cur=lst;
	while(cur)
	{
		LBXGL_Voxel_DrawRegionAlphaFluid2(cur);
		cur=cur->next;
	}
}

void LBXGL_Voxel_DrawWorldChunksAlphaRGB(LBXGL_BrushWorld *world)
{
	LBXGL_VoxelChunk *cur;

//	if(!LBXGL_Voxel_CheckRegionFrustum(rgn))
//		return;

	cur=world->vox_avischk;
	while(cur)
	{
		LBXGL_Voxel_DrawChunkAlphaRGB(cur);
		cur=cur->mesh->wavnext;
	}
}

void LBXGL_Voxel_BeginTranslateForWorld(LBXGL_BrushWorld *world)
{
	float tv[3];

//	glMatrixMode(GL_MODELVIEW);
//	glPushMatrix();
	LBXGL_Shader_PushMatrix();

	lbxgl_voxel_reforg[0]=world->cam_reforg[0];
	lbxgl_voxel_reforg[1]=world->cam_reforg[1];
	lbxgl_voxel_reforg[2]=world->cam_reforg[2];

#if 0
	tv[0]=-world->cam_reforg[0];
	tv[1]=-world->cam_reforg[1];
	tv[2]=-world->cam_reforg[2];
//	glTranslatef(tv[0], tv[1], tv[2]);
	LBXGL_Shader_Translatef(tv[0], tv[1], tv[2]);
#endif
}

void LBXGL_Voxel_EndTranslateForWorld(LBXGL_BrushWorld *world)
{
	LBXGL_Shader_PopMatrix();
//	glMatrixMode(GL_MODELVIEW);
//	glPopMatrix();
}

void LBXGL_Voxel_DrawWorldDark(LBXGL_BrushWorld *world)
{
	BTGE_ModelState *mdls;

//	lbxgl_voxel_chkleft=16;
	lbxgl_voxel_chkleft=64;
	lbxgl_voxel_chkstart=PDGL_TimeMS();

//	pdglColor4f(0, 0, 0, 1);
	pdglColor4f(1, 1, 1, 1);

	world->flags&=~BTGE_WFL_VOXFLUID;

	LBXGL_Voxel_BeginTranslateForWorld(world);

//	LBXGL_Voxel_DrawRegionListFlatVLRGB(world->vox_region);
	
	mdls=LBXGL_Voxel_QueryModelsVisibleWorld(world);
	LBXGL_BrushWorld_DrawModelListDark(world, mdls);
	
	LBXGL_Voxel_EndTranslateForWorld(world);

	LBXGL_BARF_DrawContextDark(world->barf_voxel);
}

void LBXGL_Voxel_DrawWorldLight(LBXGL_BrushWorld *world,
	LBXGL_Light *light)
{
	BTGE_ModelState *mdls;

//	if(light->flags&LBXGL_LFL_SUN)
	if(1)
	{
		LBXGL_Shader_BindTexture(0);

		LBXGL_Voxel_BeginTranslateForWorld(world);

		pdglColor4f(1, 1, 1, 1);
//		LBXGL_Voxel_DrawRegionListLight(world->vox_region, light);

		mdls=LBXGL_Voxel_QueryModelsVisibleWorld(world);
		LBXGL_BrushWorld_DrawModelListLight(world, mdls, light);

		LBXGL_Voxel_EndTranslateForWorld(world);

		LBXGL_BARF_DrawContextLight(world->barf_voxel, light);
	}else
	{
		LBXGL_Shader_BindTexture(0);

		LBXGL_Voxel_BeginTranslateForWorld(world);

		pdglColor4f(1, 1, 1, 1);
		LBXGL_Voxel_DrawRegionListLight(world->vox_region, light);

		mdls=LBXGL_Voxel_QueryModelsVisibleWorld(world);
		LBXGL_BrushWorld_DrawModelListLight(world, mdls, light);

		LBXGL_Voxel_EndTranslateForWorld(world);

//		LBXGL_BARF_DrawContextLight(world->barf_voxel, light);
	}

#if 0
	LBXGL_Shader_BindTexture(0);

	LBXGL_Voxel_BeginTranslateForWorld(world);

	pdglColor4f(1, 1, 1, 1);
//	LBXGL_Voxel_DrawRegionListLight(world->vox_region, light);

	mdls=LBXGL_Voxel_QueryModelsVisibleWorld(world);
	LBXGL_BrushWorld_DrawModelListLight(world, mdls, light);

	LBXGL_Voxel_EndTranslateForWorld(world);

	LBXGL_BARF_DrawContextLight(world->barf_voxel, light);
#endif
}

void LBXGL_Voxel_DrawWorldShadows(LBXGL_BrushWorld *world,
	LBXGL_Light *light)
{
	BTGE_ModelState *mdls;

	//for now...
	if(light->flags&LBXGL_LFL_SUN)
		return;

	LBXGL_Voxel_BeginTranslateForWorld(world);

//	LBXGL_Voxel_DrawRegionListFlat(world->vox_region);
	LBXGL_Voxel_DrawRegionListShadow(world->vox_region, light);

	mdls=LBXGL_Voxel_QueryModelsVisibleWorld(world);
	LBXGL_BrushWorld_DrawModelListShadow(world, mdls, light);

	LBXGL_Voxel_EndTranslateForWorld(world);

	LBXGL_BARF_DrawContextShadows(world->barf_voxel, light);
}

void LBXGL_Voxel_DrawWorldForLight(LBXGL_BrushWorld *world,
	LBXGL_Light *light)
{
	BTGE_ModelState *mdls;

	if(light->flags&LBXGL_LFL_SUN)
	{
		LBXGL_BARF_DrawContextForLight(world->barf_voxel, light);
	}else
	{
		LBXGL_Voxel_BeginTranslateForWorld(world);
//		LBXGL_Voxel_DrawRegionListFlat(world->vox_region);
		LBXGL_Voxel_DrawRegionListForLight(world->vox_region, light);
		LBXGL_Voxel_EndTranslateForWorld(world);
	}

#if 0
	//for now...
//	if(light->flags&LBXGL_LFL_SUN)
//		return;

	LBXGL_Voxel_BeginTranslateForWorld(world);

//	LBXGL_Voxel_DrawRegionListFlat(world->vox_region);
//	LBXGL_Voxel_DrawRegionListForLight(world->vox_region, light);

//	mdls=LBXGL_Voxel_QueryModelsVisibleWorld(world);
//	LBXGL_BrushWorld_DrawModelListShadow(world, mdls, light);

	LBXGL_Voxel_EndTranslateForWorld(world);

	LBXGL_BARF_DrawContextForLight(world->barf_voxel, light);
#endif
}

void LBXGL_Voxel_DrawWorldPointLight(LBXGL_BrushWorld *world)
{
//	LBXGL_Voxel_DrawRegionListFlatRGB(world->vox_region);
}

void LBXGL_Voxel_DrawWorldFinal(LBXGL_BrushWorld *world)
{
	BTGE_ModelState *mdls;

	LBXGL_Voxel_BeginTranslateForWorld(world);

	pdglColor4f(1, 1, 1, 1);
//	LBXGL_Voxel_DrawRegionListFinal(world->vox_region);

	mdls=LBXGL_Voxel_QueryModelsVisibleWorld(world);
	LBXGL_BrushWorld_DrawModelListFinal(world, mdls);

	LBXGL_Voxel_EndTranslateForWorld(world);

	LBXGL_BARF_DrawContextFinal(world->barf_voxel);
}

void LBXGL_Voxel_DrawWorldStaticLight(LBXGL_BrushWorld *world)
{
	BTGE_ModelState *mdls;

	LBXGL_Voxel_BeginTranslateForWorld(world);

	pdglColor4f(1, 1, 1, 1);
//	LBXGL_Voxel_DrawRegionListStaticLight(world->vox_region);

	mdls=LBXGL_Voxel_QueryModelsVisibleWorld(world);
	LBXGL_BrushWorld_DrawModelList(world, mdls);

	LBXGL_Voxel_EndTranslateForWorld(world);

	LBXGL_BARF_DrawContextStaticLight(world->barf_voxel);
}

void LBXGL_Voxel_DrawWorldFlat(LBXGL_BrushWorld *world)
{
	BTGE_ModelState *mdls;

	LBXGL_Voxel_BeginTranslateForWorld(world);

//	pdglColor4f(1, 1, 1, 1);
	LBXGL_Voxel_DrawRegionListFlat(world->vox_region);

	mdls=LBXGL_Voxel_QueryModelsVisibleWorld(world);
	LBXGL_BrushWorld_DrawModelListFlat(world, mdls);

	LBXGL_Voxel_EndTranslateForWorld(world);

//	LBXGL_BARF_DrawContextFlat(world->barf_voxel);
}

void LBXGL_Voxel_DrawWorld(LBXGL_BrushWorld *world)
{
	BTGE_ModelState *mdls;

	LBXGL_Shader_BlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	LBXGL_Shader_Color4f(1, 1, 1, 1);

	LBXGL_Voxel_BeginTranslateForWorld(world);

	pdglColor4f(1, 1, 1, 1);
	LBXGL_Voxel_DrawRegionListBasic(world->vox_region);

	mdls=LBXGL_Voxel_QueryModelsVisibleWorld(world);
	LBXGL_BrushWorld_DrawModelList(world, mdls);

	LBXGL_Voxel_EndTranslateForWorld(world);

//	LBXGL_BARF_DrawContext(world->barf_voxel);
}

void LBXGL_Voxel_DrawWorldAlpha(LBXGL_BrushWorld *world)
{
	BTGE_ModelState *mdls;

	LBXGL_Voxel_BeginTranslateForWorld(world);

//	LBXGL_Voxel_DrawRegionListAlphaRGB(world->vox_region);

	mdls=LBXGL_Voxel_QueryModelsVisibleWorld(world);
	LBXGL_BrushWorld_DrawModelListAlpha(world, mdls);

	LBXGL_Voxel_EndTranslateForWorld(world);

	LBXGL_Shader_BlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	LBXGL_Shader_Color4f(1, 1, 1, 1);

	LBXGL_BARF_DrawContextAlpha(world->barf_voxel);
}

void LBXGL_Voxel_DrawWorldFluidFlat(LBXGL_BrushWorld *world)
{
#if 0
	LBXGL_Voxel_BeginTranslateForWorld(world);

//	LBXGL_Voxel_DrawRegionListAlphaFlat(world->vox_region);
	LBXGL_Voxel_DrawRegionListAlphaFluidFlat(world->vox_region);

	LBXGL_Voxel_EndTranslateForWorld(world);
#endif

	LBXGL_BARF_DrawContextFluidFlat(world->barf_voxel);
}

void LBXGL_Voxel_DrawWorldFluid(LBXGL_BrushWorld *world)
{
#if 0
	LBXGL_Voxel_BeginTranslateForWorld(world);

//	LBXGL_Voxel_DrawRegionListAlphaRGB(world->vox_region);
	LBXGL_Voxel_DrawRegionListAlphaFluid(world->vox_region);

	LBXGL_Voxel_EndTranslateForWorld(world);
#endif

	LBXGL_BARF_DrawContextFluid(world->barf_voxel);
}

void LBXGL_Voxel_DrawWorldFluid2(LBXGL_BrushWorld *world)
{
#if 0
	LBXGL_Voxel_BeginTranslateForWorld(world);
	LBXGL_Voxel_DrawRegionListAlphaFluid2(world->vox_region);
	LBXGL_Voxel_EndTranslateForWorld(world);
#endif

	LBXGL_BARF_DrawContextFluid2(world->barf_voxel);
}

LBXGL_API void LBXGL_Voxel_DrawFluidFogVolume(
	LBXGL_BrushWorld *world, float *rgba)
{
	float s;

	if(!world->vox_region)
		return;
	if(!(world->flags&BTGE_WFL_VOXFLUID))
		return;

	glDepthMask(1);
	glColorMask(0, 0, 0, 0);

	glEnable(GL_CULL_FACE);
	glCullFace(GL_FRONT);
//	glCullFace(GL_BACK);

	LBXGL_Voxel_DrawWorldFluidFlat(world);

	glCullFace(GL_BACK);

	glDepthMask(0);
	glColorMask(1, 1, 1, 1);
	
	s=0.0625/rgba[3];
	LBXGL_Brush_DrawLayerFog(rgba, 2, s, 128);

	glDepthMask(1);
}

LBXGL_API void LBXGL_Voxel_DrawFluidSurfaceFogVolume(
	LBXGL_BrushWorld *world)
{
	float pt[4];
	float s;

	if(!world->vox_region)
		return;
	if(!(world->flags&BTGE_WFL_VOXFLUID))
		return;

	glDisable(GL_TEXTURE_2D);
	glClear(GL_STENCIL_BUFFER_BIT);
	glEnable(GL_STENCIL_TEST);

	glStencilFunc(GL_ALWAYS, 0, 255);
	glStencilOp(GL_KEEP, GL_KEEP, GL_INCR);

	glDepthMask(0);
	glColorMask(0, 0, 0, 0);

	glEnable(GL_CULL_FACE);
//	glCullFace(GL_FRONT);
	glCullFace(GL_BACK);

	LBXGL_Voxel_DrawWorldFluidFlat(world);

	glCullFace(GL_BACK);

	glDepthMask(0);
	glColorMask(1, 1, 1, 1);

	glStencilFunc(GL_NOTEQUAL, 0, 255);

	pt[0]=0.04; pt[1]=0.18; pt[2]=0.20;
//	pt[3]=1.0/384;
//	pt[3]=1.0/512;
	pt[3]=1.0/768;

	s=0.0625/pt[3];
	LBXGL_Brush_DrawLayerFog(pt, 2, s, 32);

	glDepthMask(1);

	glDepthFunc(GL_LEQUAL);
	glDisable(GL_STENCIL_TEST);
}
