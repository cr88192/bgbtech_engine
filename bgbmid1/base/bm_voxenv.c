/*
Attempt to model the reverb and filter effects for points in a voxel environment.

Will use 2 levels of voxels:
Fine, for areas within a smaller radius;
Coarse, for dealing with larger areas.

Fine will work per-voxel in a 32x32x16 area;
Coarse will work in a larger 512x512x256 area, but in terms of 16x16x16 chunk coordinates.
 */

#include <bgbmid.h>

BMID_API BGBMID_EnvVoxel (*BGBMID_VoxEnv_GetWorldFineVoxel_f)(
	BGBMID_Context *ctx, int x, int y, int z, int flags);
BMID_API BGBMID_EnvVoxel (*BGBMID_VoxEnv_GetWorldCoarseVoxel_f)(
	BGBMID_Context *ctx, int x, int y, int z, int flags);

BMID_API void BGBMID_VoxEnv_RegisterGetWorldFineVoxel(
	BGBMID_EnvVoxel (*fcn)(BGBMID_Context *ctx,
		int x, int y, int z, int flags))
{
	BGBMID_VoxEnv_GetWorldFineVoxel_f=fcn;
}

BMID_API void BGBMID_VoxEnv_RegisterGetWorldCoarseVoxel(
	BGBMID_EnvVoxel (*fcn)(BGBMID_Context *ctx,
		int x, int y, int z, int flags))
{
	BGBMID_VoxEnv_GetWorldCoarseVoxel_f=fcn;
}

BMID_API BGBMID_EnvVoxel BGBMID_VoxEnv_GetWorldFineVoxel(
	BGBMID_Context *ctx,
	int x, int y, int z, int flags)
{
	BGBMID_EnvVoxel tmp;
	
	if(BGBMID_VoxEnv_GetWorldFineVoxel_f)
		return(BGBMID_VoxEnv_GetWorldFineVoxel_f(ctx, x, y, z, flags));
	
	tmp.type=BGBMID_VOXTY_AIR;
	tmp.aux=0;
	
#if 0
	if((x<-12) || (x>12))
//		tmp.type=BGBMID_VOXTY_STONE;
		tmp.type=BGBMID_VOXTY_DIRT;
	if((y<-12) || (y>12))
//		tmp.type=BGBMID_VOXTY_STONE;
		tmp.type=BGBMID_VOXTY_DIRT;
	if((z<-6) || (z>6))
//	if(z<-6)
//		tmp.type=BGBMID_VOXTY_STONE;
		tmp.type=BGBMID_VOXTY_DIRT;
#endif

	return(tmp);
}

BMID_API BGBMID_EnvVoxel BGBMID_VoxEnv_GetWorldCoarseVoxel(
	BGBMID_Context *ctx,
	int x, int y, int z, int flags)
{
	BGBMID_EnvVoxel tmp;

	if(BGBMID_VoxEnv_GetWorldCoarseVoxel_f)
		return(BGBMID_VoxEnv_GetWorldCoarseVoxel_f(ctx, x, y, z, flags));

	tmp.type=BGBMID_VOXTY_AIR;
	tmp.aux=0;

#if 1
	if((x<-12) || (x>12))
		tmp.type=BGBMID_VOXTY_STONE;
//		tmp.type=BGBMID_VOXTY_DIRT;
	if((y<-12) || (y>12))
		tmp.type=BGBMID_VOXTY_STONE;
//		tmp.type=BGBMID_VOXTY_DIRT;
	if((z<-6) || (z>6))
		tmp.type=BGBMID_VOXTY_STONE;
//		tmp.type=BGBMID_VOXTY_DIRT;
#endif

	return(tmp);
}

BMID_API int BGBMID_VoxEnv_VoxelPassSound(BGBMID_Context *ctx,
	BGBMID_EnvVoxel vox)
{
	int i;
	i=((vox.aux>>4)&15)-1;
	if(vox.type)i-=7;
	return((i>=0)?i:0);
}

BMID_API int BGBMID_VoxEnv_VoxelDelaySound(BGBMID_Context *ctx,
	BGBMID_EnvVoxel vox)
{
	int i;
	i=(vox.aux&15)-1;
	return((i>=0)?i:0);
}

BMID_API BGBMID_EnvVoxel BGBMID_VoxEnv_VoxelPropagateSound(
	BGBMID_Context *ctx,
	BGBMID_EnvVoxel vox, BGBMID_EnvVoxel vox1)
{
	int i, j, k;

	i=BGBMID_VoxEnv_VoxelPassSound(ctx, vox);
	j=(vox1.aux>>4)&15;
	if(i>j)		{ vox1.aux=(vox1.aux&(~0xF0))|(i<<4); }
	i=BGBMID_VoxEnv_VoxelDelaySound(ctx, vox);
	j=vox1.aux&15;
	if(i>j)		{ vox1.aux=(vox1.aux&(~0x0F))|i; }
	
	return(vox1);
}

BMID_API void BGBMID_VoxEnv_PropagateValuesX(BGBMID_Context *ctx,
	BGBMID_EnvFilter *env, BGBMID_EnvVoxel *voxarr, int flags)
{
	BGBMID_EnvVoxel vox, vox1;
	int x, y, z;
	int i, j, k;
	
	for(y=0; y<32; y++)
		for(z=0; z<16; z++)
	{
		x=0;
		vox=voxarr[z*1024+y*32+x];
		for(x=1; x<32; x++)
		{
			vox1=voxarr[z*1024+y*32+x];
			vox1=BGBMID_VoxEnv_VoxelPropagateSound(ctx, vox, vox1);
			voxarr[z*1024+y*32+x]=vox1;
			vox=vox1;
		}

		for(x=30; x>=0; x--)
		{
			vox1=voxarr[z*1024+y*32+x];
			vox1=BGBMID_VoxEnv_VoxelPropagateSound(ctx, vox, vox1);
			voxarr[z*1024+y*32+x]=vox1;
			vox=vox1;
		}
	}
}

BMID_API void BGBMID_VoxEnv_PropagateValuesY(BGBMID_Context *ctx,
	BGBMID_EnvFilter *env, BGBMID_EnvVoxel *voxarr, int flags)
{
	BGBMID_EnvVoxel vox, vox1;
	int x, y, z;
	int i, j, k;
	
	for(x=0; x<32; x++)
		for(z=0; z<16; z++)
	{
		y=0;
		vox=voxarr[z*1024+y*32+x];
		for(y=1; y<32; y++)
		{
			vox1=voxarr[z*1024+y*32+x];
			vox1=BGBMID_VoxEnv_VoxelPropagateSound(ctx, vox, vox1);
			voxarr[z*1024+y*32+x]=vox1;
			vox=vox1;
		}

		for(y=30; y>=0; y--)
		{
			vox1=voxarr[z*1024+y*32+x];
			vox1=BGBMID_VoxEnv_VoxelPropagateSound(ctx, vox, vox1);
			voxarr[z*1024+y*32+x]=vox1;
			vox=vox1;
		}
	}
}

BMID_API void BGBMID_VoxEnv_PropagateValuesZ(BGBMID_Context *ctx,
	BGBMID_EnvFilter *env, BGBMID_EnvVoxel *voxarr, int flags)
{
	BGBMID_EnvVoxel vox, vox1;
	int x, y, z;
	int i, j, k;
	
	for(x=0; x<32; x++)
		for(y=0; y<32; y++)
	{
		z=0;
		vox=voxarr[z*1024+y*32+x];
		for(z=1; z<16; z++)
		{
			vox1=voxarr[z*1024+y*32+x];
			vox1=BGBMID_VoxEnv_VoxelPropagateSound(ctx, vox, vox1);
			voxarr[z*1024+y*32+x]=vox1;
			vox=vox1;
		}

		for(z=14; z>=0; z--)
		{
			vox1=voxarr[z*1024+y*32+x];
			vox1=BGBMID_VoxEnv_VoxelPropagateSound(ctx, vox, vox1);
			voxarr[z*1024+y*32+x]=vox1;
			vox=vox1;
		}
	}
}

BMID_API void BGBMID_VoxEnv_PropagateValues(BGBMID_Context *ctx,
	BGBMID_EnvFilter *env, BGBMID_EnvVoxel *voxarr, int flags)
{
	BGBMID_VoxEnv_PropagateValuesX(ctx, env, voxarr, flags);
	BGBMID_VoxEnv_PropagateValuesY(ctx, env, voxarr, flags);
	BGBMID_VoxEnv_PropagateValuesZ(ctx, env, voxarr, flags);

	BGBMID_VoxEnv_PropagateValuesX(ctx, env, voxarr, flags);
	BGBMID_VoxEnv_PropagateValuesY(ctx, env, voxarr, flags);
	BGBMID_VoxEnv_PropagateValuesZ(ctx, env, voxarr, flags);
}

BMID_API void BGBMID_VoxEnv_AddModelVoxel(BGBMID_Context *ctx,
	BGBMID_EnvFilter *env, BGBMID_EnvVoxel vox,
		vec3 lorg, vec3 org, float scale, int flags)
{
	static float rv[16]=
	{
		0.000, 0.100, 0.010, 0.060,
		1.000, 0.040, 0.040, 0.006,
		0.000, 
	};

	float f, g, d, pd, d1, v;
	int i, j;
	
	if(!vox.type)return;
//	if(!vox.aux)return;
	if(!(vox.aux&0xF0))return;
	if(!(vox.aux&0x0F))return;
	
	d=v3dist(org, lorg);
	i=(vox.aux>>4)&15;
	pd=i*scale;
	v=(pd>0)?(1.0/pd):0.0;
//	v=(pd>0)?(1.0/(pd*pd)):0.0;

	i=vox.aux&15;
	d1=(15-i)*scale;
	d1=(d+d1)*0.5;
	
	d1*=2;
	
	f=rv[vox.type];
	// f=f*f;
	// f*=scale/4.0;
	j=sqrt(d1*(44100.0/BGBMID_SOUND_C));
	env->rvMat[j]+=v*f;
}

BMID_API int BGBMID_VoxEnv_ModelPoint(BGBMID_Context *ctx,
	BGBMID_EnvFilter *env, vec3 org, int flags)
{
	BGBMID_EnvVoxel vox;
	vec3 vorg;
	float f, g;
	int x, y, z, bx, by, bz;
	int i, j, k;

	if(ctx->cnTime<env->voxTime)
		return(0);
	env->voxTime=ctx->cnTime+0.25;

	env->org[0]=floor(v3x(org));
	env->org[1]=floor(v3y(org));
	env->org[2]=floor(v3z(org));
	
	if(!env->vox_fine)
		{ env->vox_fine=bgbmid_malloc(32*32*16*sizeof(BGBMID_EnvVoxel)); }
	if(!env->vox_coarse)
		{ env->vox_coarse=bgbmid_malloc(32*32*16*sizeof(BGBMID_EnvVoxel)); }
	
	bx=env->org[0];
	by=env->org[1];
	bz=env->org[2];
	for(x=0; x<32; x++)
		for(y=0; y<32; y++)
			for(z=0; z<16; z++)
	{
		vox=BGBMID_VoxEnv_GetWorldFineVoxel(ctx,
			bx+x-16, by+y-16, bz+z-8, flags);
		env->vox_fine[z*1024+y*32+x]=vox;
	}

	bx=env->org[0]>>4;
	by=env->org[1]>>4;
	bz=env->org[2]>>4;
	for(x=0; x<32; x++)
		for(y=0; y<32; y++)
			for(z=0; z<16; z++)
	{
		vox=BGBMID_VoxEnv_GetWorldCoarseVoxel(ctx,
			bx+x-16, by+y-16, bz+z-8, flags);
		env->vox_coarse[z*1024+y*32+x]=vox;
	}
	
	if((env->vox_fine[8*1024+16*32+16].type==BGBMID_VOXTY_WATER) ||
		(env->vox_fine[8*1024+16*32+16].type==BGBMID_VOXTY_LAVA))
	{
		BGBMID_SetReverbBaseEffect(ctx, 1.0, 2.0);
		BGBMID_SetupReverbSinc(ctx, 2.0);
		return(0);
	}
	
	env->vox_fine[8*1024+16*32+16].aux=255;
	env->vox_coarse[8*1024+16*32+16].aux=255;
	
	BGBMID_VoxEnv_PropagateValues(ctx, env, env->vox_fine, flags);
	BGBMID_VoxEnv_PropagateValues(ctx, env, env->vox_coarse, flags);
	
	for(i=0; i<256; i++)
	{
		env->rvMat[i]=0;
		env->efMat[i]=0;
	}
	
	bx=env->org[0];
	by=env->org[1];
	bz=env->org[2];
	for(x=0; x<32; x++)
		for(y=0; y<32; y++)
			for(z=0; z<16; z++)
	{
		vorg=vec3(bx+x-16+0.5, by+y-16+0.5, bz+z-8+0.5);
		vox=env->vox_fine[z*1024+y*32+x];
		BGBMID_VoxEnv_AddModelVoxel(ctx, env, vox, org, vorg, 1, flags);
	}

	bx=env->org[0]>>4;
	by=env->org[1]>>4;
	bz=env->org[2]>>4;
	for(x=0; x<32; x++)
		for(y=0; y<32; y++)
			for(z=0; z<16; z++)
	{
		if(((x-16)==0) && ((y-16)==0) && ((z-8)==0))
			continue;
	
		vorg=vec3((bx+x-16+0.5)*16, (by+y-16+0.5)*16, (bz+z-8+0.5)*16);
		vox=env->vox_coarse[z*1024+y*32+x];
		BGBMID_VoxEnv_AddModelVoxel(ctx, env, vox, org, vorg, 16, flags);
	}

#if 0
	//suppress high-frequency effects
	for(i=0; i<32; i++)
	{
		f=0.5+(i/32.0)*0.5;
		f=f*f;
		env->rvMat[i]*=f;
	}
#endif

#if 0
	env->rvMat[11]*=0.75;
	env->rvMat[12]*=0.5;
	env->rvMat[13]*=0.25;
	env->rvMat[14]*=0.1;
	env->rvMat[15]*=0.1;
	env->rvMat[16]*=0.5;
#endif

	BGBMID_VoxEnv_NormalizeEnvReverb(ctx, env);
	BGBMID_VoxEnv_NormalizeEnvEffect(ctx, env);

#if 0
	printf("\n");
	for(i=0; i<16; i++)
	{
		for(j=0; j<16; j++)
		{
			printf("%.2f ", (env->rvMat[i*16+j]*255));
		}
		printf("\n");
	}
	printf("\n");
#endif

	return(0);
}

BMID_API int BGBMID_VoxEnv_UpdateModelListener(
	BGBMID_Context *ctx, int flags)
{
	int i;

	i=BGBMID_VoxEnv_ModelPoint(ctx, ctx->listen->env,
		vec3vf(ctx->listen->org), 0);
	return(i);
}

BMID_API int BGBMID_VoxEnv_NormalizeEnvReverb(
	BGBMID_Context *ctx, BGBMID_EnvFilter *env)
{
	float f, g;
	int i;
	
	g=0;
	for(i=0; i<256; i++)
	{
		f=env->rvMat[i];
		g=g+f*f;
	}

	if(g>1.0)
	{
//		BGBMID_SetReverbBaseEffect(ctx,
//			1.0, 1.0);
		BGBMID_SetReverbBaseEffect(ctx,
			1.0, pow(g, 0.25));

//		printf("%.2f\n", sqrt(g));
		g=1.0/sqrt(g);

		for(i=0; i<256; i++)
			{ env->rvMat[i]*=g; }
	}else
	{
//		printf("%.2f\n", sqrt(g));

		BGBMID_SetReverbBaseEffect(ctx,
			1.0, sqrt(g));
	}

#if 0	
	if(g>1.0)
		{ g=1.0/sqrt(g); }
	else { g=1.0; }

	for(i=0; i<256; i++)
		{ env->rvMat[i]*=g; }
#endif
	return(0);
}

BMID_API int BGBMID_VoxEnv_NormalizeEnvEffect(
	BGBMID_Context *ctx, BGBMID_EnvFilter *env)
{
	float f, g;
	int i;
	
	g=0;
	for(i=0; i<256; i++)
	{
		f=env->efMat[i];
		g=g+f*f;
	}
	
	if(g>1.0)
		{ g=1.0/sqrt(g); }
	else { g=1.0; }

	for(i=0; i<256; i++)
		{ env->efMat[i]*=g; }
	return(0);
}
