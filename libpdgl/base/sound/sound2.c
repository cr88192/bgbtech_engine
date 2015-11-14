#define USE_MP3

#include <pdgl.h>

PDGL_Sample *pdgl_sample_root=NULL;

short *pdgl_sound_mixbuf;
PDGL_MixChannel *pdgl_sound_channels=NULL;
float pdgl_sound_vol=1;
float pdgl_sound_camorigin[4];
float pdgl_sound_camorigin2[4];
float pdgl_sound_camvelocity[4];
float pdgl_sound_camrotation[9];
int pdgl_sound_lmixpos;
int pdgl_sound_mixpos;
float pdgl_sound_time;		//accumulated time

short *pdgl_sound_samplebuf;	//temporary buffer for holding samples
int pdgl_sound_seq=1;

PDGL_API PDGL_Sample *PDGL_Sound_SampleFromBuf(char *name,
	byte *buf, int chan, int rate, int bits,
	int len)
{
	PDGL_Sample *tmp;

	int slen, scnt, scnt2;
	short *buf2, *buf3, *t;
	float e;
	int i, j, k, l;

	byte *s, *s2;

	printf("PDGL_Sound_SampleFromBuf: name=%s buf=%p "
			"chan=%d rate=%d bits=%d len=%d\n",
		name, buf, chan, rate, bits, len);

	slen=chan*(bits/8);
//	scnt=len/slen;
	scnt=len;
	buf2=gctatomic("pdgl_snd_samples", (scnt+256)*sizeof(short));

	s=buf; t=buf2; e=0;
	for(i=0; i<scnt; i++)
	{
		k=0;
		for(j=0; j<chan; j++)
		{
			if(bits==16)
			{
				l=*s++;
				l+=(*s++)<<8;
				l=(short)l;
			}else
			{
				l=*s++;
				l=(l-128)*256;
			}

			k+=l;
		}
		k/=chan;
		if(k<-32767)k=-32767;
		if(k>32767)k=32767;
		*t++=k;
		e+=fabs(k);
	}
	
	e/=scnt;
	printf("PDGL_Sound_SampleFromBuf: Vol=%.2f\n", e);

	if(rate!=44100)
	{
		scnt2=scnt*(44100.0/rate);
		buf3=gctatomic("pdgl_snd_samples", (scnt2+256)*sizeof(short));

		PDGL_Sound_ScaleSample(buf3, scnt2, buf2, scnt);

		scnt=scnt2;
		gcfree(buf2);
		buf2=buf3;
	}

	tmp=gctalloc("pdgl_sample_t", sizeof(PDGL_Sample));
	tmp->name=dystrdup(name);
	tmp->len=scnt;
	tmp->buf=buf2;

	tmp->next=pdgl_sample_root;
	pdgl_sample_root=tmp;

	return(tmp);
}

PDGL_API PDGL_Sample *PDGL_Sound_LoadDecodeSampleBuffer(
	char *name, char *type, byte *ibuf, int isz)
{
	PDGL_Sample *tmp;
	byte *obuf;
	int len;

	if(!strcmp(type, "flac"))
	{
		obuf=(byte *)BFLAC_DecStream_44Mono16(ibuf, isz, &len);
		if(!obuf)
		{
			printf("PDGL_Sound_LoadFLAC: Failed decode sample '%s'\n", name);
			return(NULL);
		}

		tmp=PDGL_Sound_SampleFromBuf(name, obuf, 1, 44100, 16, len);
		free(obuf);
		return(tmp);
	}
	
	return(NULL);
}

PDGL_API PDGL_Sample *PDGL_Sound_LoadSample(char *name)
{
	PDGL_Sample *tmp;
	char buf[64];
	char *s;

	if(!strncmp(name, "midi:", 5))
	{
		tmp=PDGL_Sound_LoadStringMIDI(name);
		return(tmp);
	}

	s=name+strlen(name);
	while((s>name) && (*s!='/') && (*s!='.'))s--;

	if((s==name) || (*s!='.'))
	{
		sprintf(buf, "%s.wav", name);
		tmp=PDGL_Sound_LoadSample(buf);
		if(tmp) {tmp->name=dystrdup(name); return(tmp); }

		sprintf(buf, "%s.btac", name);
		tmp=PDGL_Sound_LoadSample(buf);
		if(tmp) {tmp->name=dystrdup(name); return(tmp); }

		sprintf(buf, "%s.flac", name);
		tmp=PDGL_Sound_LoadSample(buf);
		if(tmp) {tmp->name=dystrdup(name); return(tmp); }

		sprintf(buf, "%s.ogg", name);
		tmp=PDGL_Sound_LoadSample(buf);
		if(tmp) {tmp->name=dystrdup(name); return(tmp); }

		sprintf(buf, "%s.mp3", name);
		tmp=PDGL_Sound_LoadSample(buf);
		if(tmp) {tmp->name=dystrdup(name); return(tmp); }

		sprintf(buf, "%s.mid", name);
		tmp=PDGL_Sound_LoadSample(buf);
		if(tmp) {tmp->name=dystrdup(name); return(tmp); }

		sprintf(buf, "%s.lqac", name);
		tmp=PDGL_Sound_LoadSample(buf);
		if(tmp) {tmp->name=dystrdup(name); return(tmp); }

		return(NULL);
	}

	s++;
	if(!strcmp(s, "wav"))return(PDGL_Sound_LoadWAV(name));
	if(!strcmp(s, "flac"))return(PDGL_Sound_LoadFLAC(name));

	if(!strcmp(s, "btac"))return(PDGL_Sound_LoadBTAC(name));

	if(!strcmp(s, "mid"))return(PDGL_Sound_LoadMIDI(name));
	if(!strcmp(s, "ogg"))return(PDGL_Sound_LoadOGG(name));

	if(!strcmp(s, "lqac"))return(PDGL_Sound_LoadLQAC(name));
#ifdef USE_MP3
	if(!strcmp(s, "mp3"))return(PDGL_Sound_LoadMP3(name));
#endif

	return(NULL);
}

PDGL_API PDGL_Sample *PDGL_Sound_FindSample(char *name)
{
	PDGL_Sample *cur;

	cur=pdgl_sample_root;
	while(cur)
	{
		if(!strcmp(name, cur->name))
			return(cur);
		cur=cur->next;
	}
	return(NULL);
}

PDGL_API PDGL_Sample *PDGL_Sound_GetSample(char *name)
{
	static short sa[4]={0,0,0,0};
	char tb[256];
	PDGL_Sample *tmp;
	char *s, *t;

	tmp=PDGL_Sound_FindSample(name);
	if(tmp)return(tmp);

	tmp=PDGL_Sound_LoadSample(name);
	if(tmp)return(tmp);

	//try ripping off extension and see if this works.
	strncpy(tb, name, 255);
	s=tb+strlen(tb);
	while((s>tb) && (*s!='.') && (*s!='/'))s--;
	if(*s=='.')
	{
		*s=0;
		tmp=PDGL_Sound_FindSample(tb);
		if(tmp)return(tmp);

		tmp=PDGL_Sound_LoadSample(tb);
		if(tmp)return(tmp);
	}

	printf("PDGL_Sound_GetSample: Failed Load Sample %s\n", name);

	//fake sample
	tmp=gctalloc("pdgl_sample_t", sizeof(PDGL_Sample));
	tmp->name=dystrdup(name);
	tmp->len=0;
	tmp->buf=sa;

	tmp->next=pdgl_sample_root;
	pdgl_sample_root=tmp;

	return(tmp);
}

#if 1
PDGL_API void PDGL_Sound_UpScaleSample(short *d, int dl, short *s, int sl)
{
	int i, j, s1, s2, v;
	float r, p1, p2;

	r=(((float)sl)/((float)dl));
	for(i=0; i<dl; i++)
	{
		p1=i*r;
		j=floor(p1);
		p2=p1-j;

		s1=s[j]; s2=s[j+1];
		v=(s1*(1-p2))+(s2*p2);

		d[i]=v;
	}
}
#endif

#if 0
PDGL_API void PDGL_Sound_UpScaleSample(short *d, int dl, short *s, int sl)
{
	int i, j, s1, s2, v;
	float r, p1, p2, q1, q2;

	s[0]=s[0];

	r=(((float)(sl-2))/((float)dl));
	for(i=1; i<dl; i++)
	{
		p1=i*r;
		j=floor(p1);
		p2=p1-j;

		q1=s[j]+((s[j]-s[j-1])*p2);
		q2=s[j+1]+((s[j+1]-s[j+2])*(1-p2));
		v=(q1*(1-p2))+(q2*p2);

		if(v<-32767)v=-32767;
		if(v>32767)v=32767;

		d[i]=v;
	}
}
#endif

PDGL_API void PDGL_Sound_DownScaleSample(short *db, int dl, short *sb, int sl)
{
	int v, x, s, c;
	int i, j;

	s=16777216*((float)dl/sl);

	x=0; i=0; j=0;
	while(i<dl)
	{
		v=0; c=0;
		while((x<16777216) && (j<sl))
		{
			v+=sb[j++];
			x+=s;
			c++;
		}
		if(!c)c++;
		db[i++]=v/c;
		x-=16777216;
	}
}

PDGL_API void PDGL_Sound_ScaleSample(short *db, int dl, short *sb, int sl)
{
	if(dl>sl)
	{
		PDGL_Sound_UpScaleSample(db, dl, sb, sl);
	}else if(dl<sl)
	{
		PDGL_Sound_DownScaleSample(db, dl, sb, sl);
	}else
	{
		memcpy(db, sb, dl*sizeof(short));
	}
}

PDGL_API int PDGL_Sound_PlaySample(char *name, int flags, float vol)
{
	PDGL_Sample *samp;
	PDGL_MixChannel *chan;

	samp=PDGL_Sound_GetSample(name);

	chan=gctalloc("pdgl_mixchannel_t", sizeof(PDGL_MixChannel));
	chan->name=dystrdup(name);
	chan->id=pdgl_sound_seq++;

	chan->samp=samp;
	chan->offs=0;
	chan->len=samp->len;

	chan->flags=flags;
	chan->vol=vol;

	chan->next=pdgl_sound_channels;
	pdgl_sound_channels=chan;

	return(chan->id);
}

PDGL_API int PDGL_Sound_PlayPointSample(char *name, int flags, float vol,
	float scale, float *org, float *vel)
{
	PDGL_Sample *samp;
	PDGL_MixChannel *chan;
	float d;

#if 0
	printf("PDGL_Sound_PlayPointSample: %s %d %.2f %.2f "
		"(%.2f %.2f %.2f) (%.2f %.2f %.2f)\n",
		name, flags, vol, scale,
		org[0], org[1], org[2],
		vel[0], vel[1], vel[2]);
#endif

	samp=PDGL_Sound_GetSample(name);

	chan=gctalloc("pdgl_mixchannel_t", sizeof(PDGL_MixChannel));
	chan->name=dystrdup(name);
	chan->id=pdgl_sound_seq++;

	chan->samp=samp;
	chan->offs=0;
	chan->len=samp->len;

	if(!(flags&PDGL_SOUND_NOATTN))
		flags|=PDGL_SOUND_POINT;

	chan->flags=flags;
	chan->vol=vol;

	d=V3F_DIST(org, pdgl_sound_camorigin)/PDGL_SOUND_C;
	chan->delay=44100*d;

//	chan->scale=scale;
//	chan->org[0]=org[0];
//	chan->org[1]=org[1];
//	chan->org[2]=org[2];

	chan->max_points=1;
	chan->points=gctatomic("pdgl_snd_pts", chan->max_points*4*sizeof(float));
	chan->vel=gctatomic("pdgl_snd_vels", chan->max_points*4*sizeof(float));

	chan->points[0]=org[0];
	chan->points[1]=org[1];
	chan->points[2]=org[2];
	chan->points[3]=scale;

	chan->vel[0]=vel[0];
	chan->vel[1]=vel[1];
	chan->vel[2]=vel[2];
	chan->vel[3]=0;

	chan->num_points=1;

	chan->next=pdgl_sound_channels;
	pdgl_sound_channels=chan;

	return(chan->id);
}

PDGL_API int PDGL_Sound_PlayStream(char *name,
	int (*getsamples)(short *samples, int cnt, void *data), void *data,
	int flags, float vol)
{
	PDGL_MixChannel *chan;

	chan=gctalloc("pdgl_mixchannel_t", sizeof(PDGL_MixChannel));
	chan->name=dystrdup(name);
	chan->id=pdgl_sound_seq++;

	chan->getsamples=getsamples;
	chan->data=data;

	chan->flags=flags;
	chan->vol=vol;

	chan->next=pdgl_sound_channels;
	pdgl_sound_channels=chan;

	return(chan->id);
}

PDGL_API int PDGL_Sound_PlayPointStream(char *name,
	int (*getsamples)(short *samples, int cnt, void *data), void *data,
	int flags, float vol,
	float scale, float *org, float *vel)
{
	PDGL_MixChannel *chan;

	chan=gctalloc("pdgl_mixchannel_t", sizeof(PDGL_MixChannel));
	chan->name=dystrdup(name);
	chan->id=pdgl_sound_seq++;

	chan->getsamples=getsamples;
	chan->data=data;

	chan->flags=flags|PDGL_SOUND_POINT;
	chan->vol=vol;

//	chan->scale=scale;
//	chan->org[0]=org[0];
//	chan->org[1]=org[1];
//	chan->org[2]=org[2];

	chan->max_points=1;
	chan->points=gctatomic("pdgl_snd_pts", chan->max_points*4*sizeof(float));
	chan->vel=gctatomic("pdgl_snd_vels", chan->max_points*4*sizeof(float));

	chan->points[0]=org[0];
	chan->points[1]=org[1];
	chan->points[2]=org[2];
	chan->points[3]=scale;

	chan->vel[0]=vel[0];
	chan->vel[1]=vel[1];
	chan->vel[2]=vel[2];
	chan->vel[3]=0;

	chan->num_points=1;

	chan->next=pdgl_sound_channels;
	pdgl_sound_channels=chan;

	return(chan->id);
}

PDGL_API PDGL_MixChannel *PDGL_Sound_LookupPlaying(int id)
{
	PDGL_MixChannel *cur;
	
	cur=pdgl_sound_channels;
	while(cur)
	{
		if(cur->id==id)
			return(cur);
		cur=cur->next;
	}
	return(NULL);
}

/** Kill off a playing sound effect, or kill all sounds if passed -1 */
PDGL_API void PDGL_Sound_StopPlaying(int id)
{
	PDGL_MixChannel *cur;
	
	if(id==-1)
	{
		cur=pdgl_sound_channels;
		while(cur)
		{
			if(cur->id==id)
				cur->flags|=PDGL_SOUND_UNLINK;
			cur=cur->next;
		}
		return;
	}
	
	cur=PDGL_Sound_LookupPlaying(id);
	if(!cur)return;
	
	cur->flags|=PDGL_SOUND_UNLINK;
}

/** Get current output volume of a currently playing sound source.
  * This is calculated from the samples being mixed.
  * The volume is that at the source.
  */
PDGL_API int PDGL_Sound_GetPlayingCurVol(int id)
{
	PDGL_MixChannel *cur;
	cur=PDGL_Sound_LookupPlaying(id);
	if(!cur)return(0);
	cur->flags|=PDGL_SOUND_CALCCURVOL;
	return(cur->curvol);
}

/** Get current relative volume of a currently playing sound source.
  * This returns the volume as heard from the camera.
  * The global engine volume is ignored.
  */
PDGL_API int PDGL_Sound_GetPlayingRelCurVol(int id)
{
	PDGL_MixChannel *cur;
	cur=PDGL_Sound_LookupPlaying(id);
	if(!cur)return(0);
	cur->flags|=PDGL_SOUND_CALCCURVOL;
	return(cur->currvol);
}

/** Get current output DCT of a currently playing sound source.
  * This is calculated from the samples being mixed.
  */
PDGL_API int PDGL_Sound_GetPlayingCurDCT(int id, int *dct)
{
	PDGL_MixChannel *cur;
	int i, v;

	cur=PDGL_Sound_LookupPlaying(id);
	if(!cur)
	{
		if(dct)
		{
			for(i=0; i<64; i++)
				dct[i]=0;
		}
		return(0);
	}
	cur->flags|=PDGL_SOUND_CALCCURDCT;
	
	v=0;
	if(dct && cur->curdct)
	{
		for(i=0; i<64; i++)
			dct[i]=cur->curdct[i];
		v=cur->curdct[0]/64;
	}else
	{
		if(dct)
		{
			for(i=0; i<64; i++)
				dct[i]=0;
		}
	}
	
	return(v);
}

/** Get current output DCT of a currently playing sound source.
  * This is calculated from the samples being mixed.
  */
PDGL_API int PDGL_Sound_GetSampleMatchDCT(char *name, int *dct)
{
	PDGL_Sample *samp;

	samp=PDGL_Sound_GetSample(name);
	if(!samp)return(-1);

	PDGL_Sound_WaveCalcDCT(dct, 64,
		samp->buf, samp->len/4, 4);
	return(0);
}

PDGL_API int PDGL_Sound_WriteSamples(int cnt)
{
//	SoundDev_WriteSamples(
//		pdgl_sound_mixbuf+pdgl_sound_mixpos, cnt);
	SoundDev_WriteStereoSamples(
		pdgl_sound_mixbuf+pdgl_sound_mixpos, cnt);
	PDGL_AVI_WriteStereoSamples(
		pdgl_sound_mixbuf+pdgl_sound_mixpos, cnt);
	return(0);
}

PDGL_API int PDGL_Sound_Pop()
{
	short s[4];
	s[0]=-16384;
	s[1]=-16384;
	s[2]=16384;
	s[3]=16384;
//	SoundDev_WriteSamples(s, 4);
	SoundDev_WriteStereoSamples(s, 2);
	return(0);
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

PDGL_API int PDGL_Sound_Frame(float dt)
{
	int c;

	pdgl_sound_camorigin2[0]=
		pdgl_sound_camorigin[0]+pdgl_sound_camvelocity[0]*dt;
	pdgl_sound_camorigin2[1]=
		pdgl_sound_camorigin[1]+pdgl_sound_camvelocity[1]*dt;
	pdgl_sound_camorigin2[2]=
		pdgl_sound_camorigin[2]+pdgl_sound_camvelocity[2]*dt;

	c=0.5*44100;
	pdgl_sound_lmixpos=pdgl_sound_mixpos;
	if((pdgl_sound_mixpos+(c*2))>=(1<<19))
		pdgl_sound_mixpos=0;

#if 1
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
	SoundDev_DeInit();
	return(0);
}

PDGL_API int PDGL_Sound_Init()
{
	short *s;
	char *t;
	int rc;
	int i, j;

	printf("PDGL_Sound: Init\n");

	SoundDev_Init();

	pdgl_sound_mixbuf=malloc(1<<20);
	pdgl_sound_lmixpos=0;
	pdgl_sound_mixpos=0;
	pdgl_sound_vol=1.0;

	return(0);
}
