#include <pdgl.h>

#include <bsgen.h>

#if 0
extern void *(*bsg_fopen_fp)(char *name, char *mode);
extern void (*bsg_fclose_fp)(void *fd);
extern int (*bsg_fread_fp)(void *buf, int i, int j, void *fd);
extern int (*bsg_fwrite_fp)(void *buf, int i, int j, void *fd);
extern void (*bsg_fseek_fp)(void *buf, int i, int j);
extern int (*bsg_fgetc_fp)(void *fd);
extern int (*bsg_fputc_fp)(int i, void *fd);
extern int (*bsg_ftell_fp)(void *fd);
extern int (*bsg_feof_fp)(void *fd);
extern int (*bsg_fgets_fp)(void *buf, int sz, void *fd);
#endif


int pdgl_voice_getsamples(short *samples, int cnt, void *data)
{
	PDGL_VoiceState *vs;
	BSG_State *ctx;
	int i;

	vs=data;
	ctx=vs->sgctx;

	i=BSG_ReadSamples(ctx, samples, cnt);
	for(; i<cnt; i++)samples[i]=0;
	return(cnt);
}

int pdgl_voice_getsamples2(short *samples, int cnt, void *data)
{
	PDGL_VoiceState *vs;
	BSG_State *ctx;
	int i;

	vs=data;
	ctx=vs->sgctx;

	i=BSG_ReadSamples(ctx, samples, cnt);
	if(i>0)
	{
		for(; i<cnt; i++)samples[i]=0;
		return(cnt);
	}

	return(-1);
}

static void *bsg_fopen_i(char *name, char *mode)
	{ return(vffopen(name, mode)); }
static void bsg_fclose_i(void *fd)
	{ vfclose(fd); }
static int bsg_fread_i(void *buf, int i, int j, void *fd)
	{ return(vfread(buf, i, j, fd)); }
static int bsg_fwrite_i(void *buf, int i, int j, void *fd)
	{ return(vfwrite(buf, i, j, fd)); }
static void bsg_fseek_i(void *buf, int i, int j)
	{ vfseek(buf, i, j); }
static int bsg_fgetc_i(void *fd)
	{ return(vfgetc(fd)); }
static int bsg_fputc_i(int i, void *fd)
	{ return(vfputc(i, fd)); }
static int bsg_ftell_i(void *fd)
	{ return(vftell(fd)); }
static int bsg_feof_i(void *fd)
	{ return(vfeof(fd)); }
static int bsg_fgets_i(void *buf, int sz, void *fd)
	{ vfgets(buf, sz, fd); }

PDGL_API void PDGL_Voice_Init()
{
	static int init=0;
	BSG_IO *io;

	if(init)return;

	io=bsg_file_getio();

	io->fopen_fp=bsg_fopen_i;
	io->fclose_fp=bsg_fclose_i;
	io->fread_fp=bsg_fread_i;
	io->fwrite_fp=bsg_fwrite_i;
	io->fseek_fp=bsg_fseek_i;
	io->fgetc_fp=bsg_fgetc_i;
	io->fputc_fp=bsg_fputc_i;
	io->ftell_fp=bsg_ftell_i;
	io->feof_fp=bsg_feof_i;
	io->fgets_fp=bsg_fgets_i;

	BSG_LoadVoice("common", "voice");
	BSG_LoadVoice("male0", "voice");
	BSG_LoadVoice("male1", "voice");
	BSG_LoadVoice("comp0", "voice");
	BSG_LoadVoice("female0", "voice");
}

PDGL_API PDGL_VoiceState *PDGL_Voice_New(char *voice,
	int flags, float vol,
	float scale, float *org, float *vel)
{
	PDGL_VoiceState *vs;
	BSG_State *ctx;
	int i;

	PDGL_Voice_Init();

	ctx=BSG_NewContext(voice);

	vs=gctalloc("_pdgl_voicestate_t", sizeof(PDGL_VoiceState));
	vs->sgctx=ctx;

	i=PDGL_Sound_PlayPointStream(NULL, pdgl_voice_getsamples, vs,
		flags,  vol, scale, org, vel);
	vs->chan=i;
	return(vs);
}

PDGL_API int PDGL_Voice_Speak(PDGL_VoiceState *vs, char *text)
{
	BSG_SendText(vs->sgctx, text);
	return(0);
}


PDGL_API int PDGL_Voice_PointSpeak(char *voice, char *text,
	int fl, float vol, float scale, float *org, float *vel)
{
	PDGL_VoiceState *vs;
	BSG_State *ctx;
	int i;

	PDGL_Voice_Init();

	ctx=BSG_NewContext(voice);
	BSG_SendText(ctx, text);

	vs=gctalloc("_pdgl_voicestate_t", sizeof(PDGL_VoiceState));
	vs->sgctx=ctx;

	i=PDGL_Sound_PlayPointStream(NULL, pdgl_voice_getsamples2, vs,
		fl, vol, scale, org, vel);
	vs->chan=i;

	return(i);
}
