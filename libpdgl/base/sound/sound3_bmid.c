// #define USE_MP3

#include <pdgl.h>

#include <bgbmid.h>

// PDGL_Sample *pdgl_sample_root=NULL;
// short *pdgl_sound_mixbuf;
// PDGL_MixChannel *pdgl_sound_channels=NULL;

BGBMID_Context *pdgl_bgbmid_ctx;
BTGE_BrushWorld *pdgl_bgbmid_world;

float pdgl_sound_vol=1;
float pdgl_sound_camorigin[4];
float pdgl_sound_camorigin2[4];
float pdgl_sound_camvelocity[4];
float pdgl_sound_camrotation[9];

// int pdgl_sound_lmixpos;
// int pdgl_sound_mixpos;
float pdgl_sound_time;		//accumulated time

// short *pdgl_sound_samplebuf;	//temporary buffer for holding samples
// int pdgl_sound_seq=1;

static void *bgbmid_fopen_i(char *name, char *mode)
	{ return(vffopen(name, mode)); }
static void bgbmid_fclose_i(void *fd)
	{ vfclose((VFILE *)fd); }
static int bgbmid_fread_i(void *buf, int i, int j, void *fd)
	{ return(vfread(buf, i, j, (VFILE *)fd)); }
static int bgbmid_fwrite_i(void *buf, int i, int j, void *fd)
	{ return(vfwrite(buf, i, j, (VFILE *)fd)); }
static void bgbmid_fseek_i(void *fd, int i, int j)
	{ vfseek((VFILE *)fd, i, j); }
static int bgbmid_fgetc_i(void *fd)
	{ return(vfgetc((VFILE *)fd)); }
static int bgbmid_fputc_i(int i, void *fd)
	{ return(vfputc(i, (VFILE *)fd)); }
static int bgbmid_ftell_i(void *fd)
	{ return(vftell((VFILE *)fd)); }
static int bgbmid_feof_i(void *fd)
	{ return(vfeof((VFILE *)fd)); }
static int bgbmid_fgets_i(void *buf, int sz, void *fd)
	{ vfgets(buf, sz, (VFILE *)fd); }

BGBMID_EnvVoxel PDGL_Sound_GetWorldFineVoxel_i(
	BGBMID_Context *ctx, int x, int y, int z, int flags) //AH:ignore
{
	BGBMID_EnvVoxel tmp;
	BTGE_VoxelData vox;
	
	if(!pdgl_bgbmid_world)
	{
		tmp.type=BGBMID_VOXTY_AIR;
		tmp.aux=0;
		return(tmp);
	}
	
//	vox=BTGE_Voxel_GetWorldVoxelData(
//		pdgl_bgbmid_world, x, y, z);

	vox=BTGE_Voxel_GetWorldVoxelDataFlags(
		pdgl_bgbmid_world, x, y, z, VOX_GFL_SKIP_LOAD);
		
	switch(vox.type&VOX_TY_MASK)
	{
	case VOX_TY_AIR: tmp.type=BGBMID_VOXTY_AIR; break;
	case VOX_TY_GRASS: tmp.type=BGBMID_VOXTY_DIRT; break;
	case VOX_TY_DIRT: tmp.type=BGBMID_VOXTY_DIRT; break;
	case VOX_TY_SAND: tmp.type=BGBMID_VOXTY_SAND; break;
	case VOX_TY_GRAVEL: tmp.type=BGBMID_VOXTY_SAND; break;
	case VOX_TY_CLOTH: tmp.type=BGBMID_VOXTY_CLOTH; break;
	case VOX_TY_MYCELIUM: tmp.type=BGBMID_VOXTY_DIRT; break;

	case VOX_TY_WOOD: case VOX_TY_LOG:
		tmp.type=BGBMID_VOXTY_WOOD; break;

	case VOX_TY_SHROOM_BROWN_HUGE:
	case VOX_TY_SHROOM_RED_HUGE:
		tmp.type=BGBMID_VOXTY_WOOD; break;

	case VOX_TY_STONE: case VOX_TY_COBBLESTONE:
	case VOX_TY_BRICK: case VOX_TY_NETHERBRICK:
		tmp.type=BGBMID_VOXTY_STONE;
		break;

	case VOX_TY_WATER: case VOX_TY_WATER2:
		tmp.type=BGBMID_VOXTY_WATER;
		break;
	case VOX_TY_LAVA: case VOX_TY_LAVA2:
		tmp.type=BGBMID_VOXTY_LAVA;
		break;

	default:
		if(BTGE_Voxel_VoxelFluidP(&vox))
			{ tmp.type=BGBMID_VOXTY_WATER; }
		else if(BTGE_Voxel_VoxelOpaqueP(&vox))
			{ tmp.type=BGBMID_VOXTY_STONE; }
		else
			{ tmp.type=BGBMID_VOXTY_AIR; }
		break;
	}

	tmp.aux=0;
	return(tmp);
}

BGBMID_EnvVoxel PDGL_Sound_GetWorldCoarseVoxel_i(
	BGBMID_Context *ctx, int x, int y, int z, int flags) //AH:ignore
{
	BTGE_VoxelChunk *chk;
	BGBMID_EnvVoxel tmp;
	BTGE_VoxelData vox;
	
//	chk=BTGE_Voxel_GetWorldChunkFlags(
//		pdgl_bgbmid_world, x*16, y*16, z*16,
//		VOX_GFL_DELTA_LOAD);
	chk=NULL;
	
	if(chk)
	{
		tmp.type=BGBMID_VOXTY_STONE;
		tmp.aux=0;
		return(tmp);
	}
	
	tmp.type=BGBMID_VOXTY_AIR;
	tmp.aux=0;
	return(tmp);
}

PDGL_API PDGL_Sample *PDGL_Sound_SampleFromBuf(char *name,
	byte *buf, int chan, int rate, int bits,
	int len)
{
	return(NULL);
}

PDGL_API PDGL_Sample *PDGL_Sound_LoadDecodeSampleBuffer(
	char *name, char *type, byte *ibuf, int isz)
{
	return(NULL);
}

PDGL_API PDGL_Sample *PDGL_Sound_LoadSample(char *name)
{
	return(NULL);
}

PDGL_API PDGL_Sample *PDGL_Sound_FindSample(char *name)
{
	return(NULL);
}

PDGL_API PDGL_Sample *PDGL_Sound_GetSample(char *name)
{
//	return(tmp);
}

PDGL_API int PDGL_Sound_PlaySample(char *name, int flags, float vol)
{
	int id;

	id=BGBMID_PlaySampleName(pdgl_bgbmid_ctx, name);
	return(id);

//	return(chan->id);
}

PDGL_API int PDGL_Sound_PlayPointSample(char *name, int flags, float vol,
	float scale, float *org, float *vel)
{
	int id;

	if(!(flags&PDGL_SOUND_NOATTN))
		flags|=PDGL_SOUND_POINT;

	id=BGBMID_PlaySampleName(pdgl_bgbmid_ctx, name);

	BGBMID_Controller3fv(pdgl_bgbmid_ctx, id,
		BGBMID_CONTROL_ORIGIN_3D, org);
	BGBMID_Controller3fv(pdgl_bgbmid_ctx, id,
		BGBMID_CONTROL_VELOCITY_3D, vel);

	BGBMID_Controller1f(pdgl_bgbmid_ctx, id,
		BGBMID_CONTROL_VOLUME, vol);

	BGBMID_Controller1i(pdgl_bgbmid_ctx, id,
		BGBMID_CONTROL_FLAGS, flags);

	return(id);

//	return(chan->id);
}

PDGL_API int PDGL_Sound_PlayStream(char *name,
	int (*getsamples)(short *samples, int cnt, void *data), void *data,
	int flags, float vol)
{
	return(-1);

//	return(chan->id);
}

PDGL_API int PDGL_Sound_PlayPointStream(char *name,
	int (*getsamples)(short *samples, int cnt, void *data), void *data,
	int flags, float vol,
	float scale, float *org, float *vel)
{
	return(-1);

//	return(chan->id);
}

PDGL_API PDGL_MixChannel *PDGL_Sound_LookupPlaying(int id)
{
	return(NULL);
}

/** Kill off a playing sound effect, or kill all sounds if passed -1 */
PDGL_API void PDGL_Sound_StopPlaying(int id)
{
	BGBMID_StopChannel(pdgl_bgbmid_ctx, id);
}

PDGL_API int PDGL_Sound_StillPlayingP(int id)
{
	return(BGBMID_ChannelStillPlayingP(pdgl_bgbmid_ctx, id));
}

PDGL_API void PDGL_Sound_SetPlayingOrigin(int id,
	float *org, float *vel)
{
	if(org)
	{
		BGBMID_Controller3fv(pdgl_bgbmid_ctx, id,
			BGBMID_CONTROL_ORIGIN_3D, org);
	}
	if(vel)
	{
		BGBMID_Controller3fv(pdgl_bgbmid_ctx, id,
			BGBMID_CONTROL_VELOCITY_3D, vel);
	}
}

PDGL_API void PDGL_Sound_CreateStreamName(
	char *name, int rate, int len)
{
	BGBMID_CreateStreamName(pdgl_bgbmid_ctx, name, rate, len);
}

PDGL_API void PDGL_Sound_WriteStreamSamples(
	char *name, short *pcm, int len)
{
	int prg;
	
	prg=BGBMID_LookupSampleName(pdgl_bgbmid_ctx, name);
	if(prg<0)return;
	
//	BGBMID_CreateStreamName(pdgl_bgbmid_ctx, name, rate, len);
	BGBMID_WriteStreamSamples(pdgl_bgbmid_ctx, prg, pcm, len);
}

PDGL_API void PDGL_Sound_WriteStreamStereoSamples(
	char *name, short *pcm, int len)
{
	short *pcm2;
	int prg;
	int i, j, k;
	
	if(len<=0)
		return;
	
	prg=BGBMID_LookupSampleName(pdgl_bgbmid_ctx, name);
	if(prg<0)return;
	
	pcm2=malloc(len*sizeof(short));
	for(i=0; i<len; i++)
	{
		j=(pcm[i*2+0]+pcm[i*2+1])/2;
		pcm2[i]=j;
	}
	
//	BGBMID_CreateStreamName(pdgl_bgbmid_ctx, name, rate, len);
	BGBMID_WriteStreamSamples(pdgl_bgbmid_ctx, prg, pcm2, len);
	
	free(pcm2);
}

PDGL_API int PDGL_Sound_PointSpeak(char *voice, char *text,
	int flags, float vol, float scale, float *org, float *vel)
{
//	int i;
	int id, chan, prg;

	if(!(flags&PDGL_SOUND_NOATTN))
		flags|=PDGL_SOUND_POINT;

	chan=BGBMID_AllocChannel(pdgl_bgbmid_ctx);
	prg=BGBMID_CreateProgramSpeak(pdgl_bgbmid_ctx, voice, text);
	if((chan<0) || (prg<0))
	{
		printf("PDGL_Voice_PointSpeak: Failed Alloc Chan=%d Prog=%d\n",
			chan, prg);
		return(-1);
	}

	id=BGBMID_PlayMixSample(pdgl_bgbmid_ctx, chan, prg);

//	id=BGBMID_PlaySampleName(pdgl_bgbmid_ctx, name);

	BGBMID_Controller3fv(pdgl_bgbmid_ctx, id,
		BGBMID_CONTROL_ORIGIN_3D, org);
	BGBMID_Controller3fv(pdgl_bgbmid_ctx, id,
		BGBMID_CONTROL_VELOCITY_3D, vel);

	BGBMID_Controller1f(pdgl_bgbmid_ctx, id,
		BGBMID_CONTROL_VOLUME, vol);

	BGBMID_Controller1i(pdgl_bgbmid_ctx, id,
		BGBMID_CONTROL_FLAGS, flags);

	return(id);
}

PDGL_API int PDGL_Sound_Speak(char *voice, char *text,
	int flags, float vol, float scale)
{
//	int i;
	int id, chan, prg;

//	if(!(flags&PDGL_SOUND_NOATTN))
//		flags|=PDGL_SOUND_POINT;

	chan=BGBMID_AllocChannel(pdgl_bgbmid_ctx);
	prg=BGBMID_CreateProgramSpeak(pdgl_bgbmid_ctx, voice, text);
	if((chan<0) || (prg<0))
	{
		printf("PDGL_Voice_PointSpeak: Failed Alloc Chan=%d Prog=%d\n",
			chan, prg);
		return(-1);
	}

	id=BGBMID_PlayMixSample(pdgl_bgbmid_ctx, chan, prg);

//	id=BGBMID_PlaySampleName(pdgl_bgbmid_ctx, name);

//	BGBMID_Controller3fv(pdgl_bgbmid_ctx, id,
//		BGBMID_CONTROL_ORIGIN_3D, org);
//	BGBMID_Controller3fv(pdgl_bgbmid_ctx, id,
//		BGBMID_CONTROL_VELOCITY_3D, vel);

	BGBMID_Controller1f(pdgl_bgbmid_ctx, id,
		BGBMID_CONTROL_VOLUME, vol);
	BGBMID_Controller1i(pdgl_bgbmid_ctx, id,
		BGBMID_CONTROL_FLAGS, flags);

	return(id);
}

/** Get current output volume of a currently playing sound source.
  * This is calculated from the samples being mixed.
  * The volume is that at the source.
  */
PDGL_API int PDGL_Sound_GetPlayingCurVol(int id)
{
	float tv[4];
	int i, j, k;
	
	BGBMID_GetChannelVectorNfv(pdgl_bgbmid_ctx, id,
		BGBMID_VECTOR_CURRENT_VOLUME,
		tv, 4);
	return(tv[0]);

//	return(cur->curvol);
}

/** Get current relative volume of a currently playing sound source.
  * This returns the volume as heard from the camera.
  * The global engine volume is ignored.
  */
PDGL_API int PDGL_Sound_GetPlayingRelCurVol(int id)
{
	float tv[4];
	int i, j, k;
	
	BGBMID_GetChannelVectorNfv(pdgl_bgbmid_ctx, id,
		BGBMID_VECTOR_CURRENT_VOLUME,
		tv, 4);
	return((tv[1]+tv[2])*0.5);

//	return(cur->currvol);
}

/** Get current output DCT of a currently playing sound source.
  * This is calculated from the samples being mixed.
  */
PDGL_API int PDGL_Sound_GetPlayingCurDCT(int id, int *dct)
{
	float tv[16];
	int i, j, k;
	
	BGBMID_GetChannelVectorNfv(pdgl_bgbmid_ctx, id,
		BGBMID_VECTOR_HISTOGRAM, tv, 16);
	for(i=0; i<16; i++)
		{ dct[i]=tv[i]; }

//	return(v);
}

/** Get current output DCT of a currently playing sound source.
  * This is calculated from the samples being mixed.
  */
PDGL_API int PDGL_Sound_GetSampleMatchDCT(char *name, int *dct)
{
	float tv[16];
	int id, chan, prg;
	int i, j, k;

	prg=BGBMID_LookupSampleName(pdgl_bgbmid_ctx, name);
	if(prg<0)return;

	BGBMID_GetProgramVectorNfv(pdgl_bgbmid_ctx, id,
		BGBMID_VECTOR_HISTOGRAM, tv, 16);
	for(i=0; i<16; i++)
		{ dct[i]=tv[i]; }

//	PDGL_Sample *samp;
//	samp=PDGL_Sound_GetSample(name);
//	if(!samp)return(-1);

//	PDGL_Sound_WaveCalcDCT(dct, 64,
//		samp->buf, samp->len/4, 4);
	return(0);
}

PDGL_API int PDGL_Sound_MatchCompareDCT(int *dct1, int *dct2, int dctsz)
{
	double e;
	int i, j, k;
	
	e=0;
	for(i=0; i<16; i++)
	{
		j=dct1[i]-dct2[i];
		e+=((double)j)*j;
	}
	e=sqrt(e);

	return(e);
}

PDGL_API int PDGL_Sound_GetRecordSpectrum(int *dct, int msz)
{
	return(BGBMID_GetRecordFreqBlock(pdgl_bgbmid_ctx, dct, msz));
}

PDGL_API int PDGL_Sound_GetPriorRecordSpectrum(int idx, int *dct, int msz)
{
	return(BGBMID_GetPriorRecordFreqBlock(pdgl_bgbmid_ctx, idx, dct, msz));
}

PDGL_API int PDGL_Sound_WriteSamples(int cnt)
{
	return(0);
}

int PDGL_Sound_WriteStereoSamples2_i(short *buffer, int cnt, int ovcnt)
{
	PDGL_AVI_WriteStereoSamples(buffer, cnt);
}

PDGL_API int PDGL_Sound_Pop()
{
	short s[4];
	s[0]=-16384;
	s[1]=-16384;
	s[2]=16384;
	s[3]=16384;
//	SoundDev_WriteSamples(s, 4);
//	SoundDev_WriteStereoSamples(s, 2);
	return(0);
}

PDGL_API int PDGL_Sound_SetWorld(BTGE_BrushWorld *world)
{
	pdgl_bgbmid_world=world;
}

PDGL_API int PDGL_Sound_CamOrigin(float x, float y, float z)
{
	pdgl_sound_camorigin[0]=x;
	pdgl_sound_camorigin[1]=y;
	pdgl_sound_camorigin[2]=z;
	return(0);
}

PDGL_API int PDGL_Sound_CamVelocity(float x, float y, float z)
{
	pdgl_sound_camvelocity[0]=x;
	pdgl_sound_camvelocity[1]=y;
	pdgl_sound_camvelocity[2]=z;
	return(0);
}

PDGL_API int PDGL_Sound_CamOriginFV(float *v)
{
	pdgl_sound_camorigin[0]=v[0];
	pdgl_sound_camorigin[1]=v[1];
	pdgl_sound_camorigin[2]=v[2];
	return(0);
}

PDGL_API int PDGL_Sound_CamVelocityFV(float *v)
{
	pdgl_sound_camvelocity[0]=v[0];
	pdgl_sound_camvelocity[1]=v[1];
	pdgl_sound_camvelocity[2]=v[2];
	return(0);
}

PDGL_API int PDGL_Sound_CamRotationFV(float *v)
{
	pdgl_sound_camrotation[0]=v[0];
	pdgl_sound_camrotation[1]=v[1];
	pdgl_sound_camrotation[2]=v[2];
	pdgl_sound_camrotation[3]=v[3];
	pdgl_sound_camrotation[4]=v[4];
	pdgl_sound_camrotation[5]=v[5];
	pdgl_sound_camrotation[6]=v[6];
	pdgl_sound_camrotation[7]=v[7];
	pdgl_sound_camrotation[8]=v[8];
	return(0);
}

PDGL_API int PDGL_Sound_Frame(double dt)
{
	int i, c;

	if(dt>0.5)
		return(-1);

	pdgl_sound_camorigin2[0]=
		pdgl_sound_camorigin[0]+pdgl_sound_camvelocity[0]*dt;
	pdgl_sound_camorigin2[1]=
		pdgl_sound_camorigin[1]+pdgl_sound_camvelocity[1]*dt;
	pdgl_sound_camorigin2[2]=
		pdgl_sound_camorigin[2]+pdgl_sound_camvelocity[2]*dt;

	BGBMID_SetListenerOrigin(pdgl_bgbmid_ctx, 0, pdgl_sound_camorigin);
	BGBMID_SetListenerVelocity(pdgl_bgbmid_ctx, 0, pdgl_sound_camvelocity);
	BGBMID_SetListenerRotation(pdgl_bgbmid_ctx, 0, pdgl_sound_camrotation);

//	c=0.5*44100;
//	pdgl_sound_lmixpos=pdgl_sound_mixpos;
//	if((pdgl_sound_mixpos+(c*2))>=(1<<19))
//		pdgl_sound_mixpos=0;

	i=btCvarGetf("snd_envmodel");

	if(i>0)
		{ BGBMID_VoxEnv_UpdateModelListener(pdgl_bgbmid_ctx, 0); }

	BGBMID_PlayMixTime(pdgl_bgbmid_ctx, dt);
	BGBMID_RecordMixTime(pdgl_bgbmid_ctx, dt);

#if 0
	while(dt>0.25)
	{
		c=0.25*44100;
		PDGL_Sound_MixSamples(c, dt);
		dt-=0.25;
	}

	c=dt*44100;
	PDGL_Sound_MixSamples(c, dt);
	PDGL_Sound_WriteSamples(c);
	pdgl_sound_mixpos+=c*2;
#endif

	return(0);
}

PDGL_API int PDGL_Sound_DeInit()
{
	BGBMID_SoundDev_DeInit();
	BGBMID_SoundDev_RecDeInit();
//	SoundDev_DeInit();
	return(0);
}

PDGL_API int PDGL_Sound_Init()
{
	BGBMID_RegisterWriteStereoSamples2(
		PDGL_Sound_WriteStereoSamples2_i);
	BGBMID_VoxEnv_RegisterGetWorldFineVoxel(
		PDGL_Sound_GetWorldFineVoxel_i);
	BGBMID_VoxEnv_RegisterGetWorldCoarseVoxel(
		PDGL_Sound_GetWorldCoarseVoxel_i);

	bgbmid_set_fopen(bgbmid_fopen_i);
	bgbmid_set_fclose(bgbmid_fclose_i);
	bgbmid_set_fread(bgbmid_fread_i);
	bgbmid_set_fwrite(bgbmid_fwrite_i);
	bgbmid_set_fseek(bgbmid_fseek_i);
	bgbmid_set_fgetc(bgbmid_fgetc_i);
	bgbmid_set_fputc(bgbmid_fputc_i);
	bgbmid_set_ftell(bgbmid_ftell_i);
	bgbmid_set_feof(bgbmid_feof_i);
	bgbmid_set_fgets(bgbmid_fgets_i);

	BGBMID_SoundDev_Init();
	BGBMID_SoundDev_RecInit();

	BGBMID_LoadPackGlobal("instruments.pak");
	BGBMID_LoadPackGlobal("sound.pak");

	pdgl_bgbmid_ctx=BGBMID_NewContext();

	BGBMID_SetNoteTicks(pdgl_bgbmid_ctx, 172);
	BGBMID_SetRateScale(pdgl_bgbmid_ctx, 1.0);
	BGBMID_SetFreqScale(pdgl_bgbmid_ctx, 1.0);
	BGBMID_SetNoteLen(pdgl_bgbmid_ctx, 1.0);

	BGBMID_SetOutputBuffer(pdgl_bgbmid_ctx, NULL, 1<<20);

	BGBMID_InitChannels(pdgl_bgbmid_ctx);

//	BGBMID_SetReverbBaseEffect(pdgl_bgbmid_ctx, 1.0, 1.0);
	BGBMID_SetupReverbSinc(pdgl_bgbmid_ctx, 2.0);

	return(0);
}
