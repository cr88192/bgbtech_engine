#include <pdgl.h>

#include <bgbmid.h>

#define PDGL_RIFF_RIFF 0x46464952
#define PDGL_RIFF_WAVE 0x45564157
#define PDGL_RIFF_fmt  0x20746D66
#define PDGL_RIFF_data 0x61746164

PDGL_Sample *PDGL_Sound_LoadWAV(char *name)
{
	VFILE *fd;
	PDGL_Sample *tmp;

	int i, j;
	unsigned int tag, len, rsrc;

	byte *buf;
	int chan, rate, bits;

	printf("PDGL_Sound_LoadWAV: Loading sample '%s'\n", name);

	fd=vffopen(name, "rb");

	if(!fd)
	{
		printf("PDGL_Sound_LoadWAV: Failed open sample '%s'\n", name);
		return(NULL);
	}

	tag=0;
	len=0;
	rsrc=0;
	for(i=0; i<4; i++)
		tag+=vfgetc(fd)<<(i*8);
	for(i=0; i<4; i++)
		len+=vfgetc(fd)<<(i*8);
	for(i=0; i<4; i++)
		rsrc+=vfgetc(fd)<<(i*8);

	if(tag!=PDGL_RIFF_RIFF)
	{
		printf("PDGL_Sound_LoadWAV: File Tag not RIFF\n");
		return(NULL);
	}
	if(rsrc!=PDGL_RIFF_WAVE)
	{
		printf("PDGL_Sound_LoadWAV: Resource type not WAVE\n");
		return(NULL);
	}

	//fmt
	tag=0;
	len=0;
	for(i=0; i<4; i++)
		tag+=vfgetc(fd)<<(i*8);
	for(i=0; i<4; i++)
		len+=vfgetc(fd)<<(i*8);

	buf=gcralloc(len);
	vfread(buf, 1, len, fd);

	chan=buf[2];
	rate=buf[4]+(buf[5]<<8);
	bits=buf[14];

	//data
	tag=0;
	len=0;
	for(i=0; i<4; i++)
		tag+=vfgetc(fd)<<(i*8);
	for(i=0; i<4; i++)
		len+=vfgetc(fd)<<(i*8);


	buf=gcatomic(len);
	vfread(buf, 1, len, fd);

	len/=chan*(bits/8);
	tmp=PDGL_Sound_SampleFromBuf(name, buf, chan, rate, bits, len);
	gcfree(buf);

	return(tmp);
}


PDGL_Sample *PDGL_Sound_LoadMIDI(char *name)
{
	BGBMID_Context *ctx;
	PDGL_Sample *tmp;
	short *obuf;
	byte *buf, *cs, *ce;
	char *ifn, *ofn;
	float ratesc;
	int lv, dt, tg, d0, d1, op, ch, sz;
	int i, j, k, noteticks, ntrk;

	ratesc=1;
	ifn=name;
	ofn=NULL;
	
	i=BGBMID_ReadMidiMerge(ifn, &buf, &sz, &noteticks);
	if(i<0)return(NULL);

	ctx=BGBMID_NewContext();
	BGBMID_SetNoteTicks(ctx, noteticks);
	BGBMID_SetRateScale(ctx, ratesc);
	BGBMID_SetNoteLen(ctx, 1.0);

	BGBMID_SetOutputSampleRate(ctx, 44100);
	BGBMID_SetOutputBuffer(ctx, NULL, 1<<24);

	i=BGBMID_SynthStream(ctx, buf, sz);
	printf("Stream End %d\n", i);

//	BGBMID_DumpMidi0(ofn, buf, sz, noteticks);

	obuf=BGBMID_GetOutputBuffer(ctx);
	i=BGBMID_GetOutputBufferPos(ctx);
	j=BGBMID_GetOutputSampleRate(ctx);
//	BGBMID_StoreWAV(ofn, (byte *)obuf, 2, j, 16, i);

	//coerce to mono
	for(k=0; k<i; k++)
	{
		lv=(obuf[k*2+0]+obuf[k*2+1])/2;
//		lv/=16;
		if(lv<(-32767))lv=-32767;
		if(lv>32767)lv=32767;
		obuf[k]=lv;
	}

	tmp=PDGL_Sound_SampleFromBuf(name, (byte *)obuf, 1, j, 16, i);

	BGBMID_DestroyContext(ctx);

	return(tmp);
}

PDGL_Sample *PDGL_Sound_LoadStringMIDI(char *name)
{
	BGBMID_Context *ctx;
	PDGL_Sample *tmp;
	short *obuf;
	byte *buf, *cs, *ce;
	char *ifn, *ofn;
	float ratesc;
	int lv, dt, tg, d0, d1, op, ch, sz;
	int i, j, k, noteticks, ntrk;

	ratesc=1;
	ifn=name;
	ofn=NULL;
	
	if(!strncmp(ifn, "midi:", 5))ifn+=5;

	buf=bgbmid_malloc(1<<16);

//	i=BGBMID_ReadMidiMerge(ifn, &buf, &sz, &noteticks);
	BGBMID_Ascii2Midi(buf, ifn, &sz, &noteticks);
	if(i<0)return(NULL);

	ctx=BGBMID_NewContext();
	BGBMID_SetNoteTicks(ctx, noteticks);
	BGBMID_SetRateScale(ctx, ratesc);
	BGBMID_SetNoteLen(ctx, 1.0);

	BGBMID_SetOutputSampleRate(ctx, 44100);
	BGBMID_SetOutputBuffer(ctx, NULL, 1<<24);

	i=BGBMID_SynthStream(ctx, buf, sz);
	printf("Stream End %d\n", i);

//	BGBMID_DumpMidi0(ofn, buf, sz, noteticks);

	obuf=BGBMID_GetOutputBuffer(ctx);
	i=BGBMID_GetOutputBufferPos(ctx);
	j=BGBMID_GetOutputSampleRate(ctx);
//	BGBMID_StoreWAV(ofn, (byte *)obuf, 2, j, 16, i);

	//coerce to mono
	for(k=0; k<i; k++)
	{
		lv=(obuf[k*2+0]+obuf[k*2+1])/2;
//		lv/=16;
		if(lv<(-32767))lv=-32767;
		if(lv>32767)lv=32767;
		obuf[k]=lv;
	}

	tmp=PDGL_Sound_SampleFromBuf(name, (byte *)obuf, 1, j, 16, i);

	BGBMID_DestroyContext(ctx);

	bgbmid_free(buf);

	return(tmp);
}

#if 0
PDGL_API int PDGL_Sound_WaveCalcDCT(int *dct, int dctsz,
	short *bp, int len, int step)
{
	int i, j, k, l, n, bi;
	float f, g, bf;
	
	n=dctsz;
	l=len; bi=0; bf=0;
	if(l>n)l=n;
	for(i=0; i<n; i++)
	{
		f=0;
		for(j=0; j<l; j++)
		{
			g=cos((M_PI/n)*(j+0.5)*i);
			f+=bp[j*step]*g;
		}
		dct[i]=f;
	}
	
	return(0);
}
#endif

#if 1
PDGL_API int PDGL_Sound_WaveCalcDCT(int *dct, int dctsz,
	short *bp, int len, int step)
{
	static short *pcdct=NULL;
	static int pcdctsz=0;
	int i, j, k, l, n;
	float f, g;
	
	if(!pcdct || (pcdctsz!=dctsz))
	{
		n=dctsz;
		pcdct=gcrealloc(pcdct, n*n*sizeof(short));
		for(i=0; i<n; i++)
		{
			for(j=0; j<n; j++)
			{
				g=cos((M_PI/n)*(j+0.5)*i);
				pcdct[i*n+j]=256*g;
			}
		}
		pcdctsz=dctsz;
	}
	
	n=dctsz;
	l=len;
	if(l>n)l=n;
	for(i=0; i<n; i++)
	{
		k=0;
		for(j=0; j<l; j++)
		{
			k+=bp[j*step]*pcdct[i*n+j];
		}
		dct[i]=k>>8;
	}
	
	return(0);
}
#endif

PDGL_API int PDGL_Sound_MatchCompareDCT(int *dct1, int *dct2, int dctsz)
{
	double f, g, h;
	int i, j;
	
	h=0;
	for(i=0; i<dctsz; i++)
	{
		f=fabs(dct1[i]);
		g=fabs(dct2[i]);
		f=f-g;
		h+=f*f;
	}
	j=sqrt(h/dctsz);
//	j/=dctsz;
	return(j);
}
