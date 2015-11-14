//#define USE_MPGLIB

#include <pdgl.h>

#include <unistd.h>
#include <fcntl.h>
#include <sys/soundcard.h>

#ifdef USE_MPGLIB
#include <mpg123.h>
#include <mpglib.h>
#endif
//AH:skip

#define PDGL_RIFF_RIFF 0x46464952
#define PDGL_RIFF_WAVE 0x45564157
#define PDGL_RIFF_fmt  0x20746D66
#define PDGL_RIFF_data 0x61746164

#define PDGL_SOUND_LOOP	1
#define PDGL_SOUND_POINT	2

#define PDGL_SOUND_UNLINK	256

typedef struct PDGL_Sample_s PDGL_Sample;
typedef struct PDGL_MixChannel_s PDGL_MixChannel;

struct PDGL_Sample_s {
PDGL_Sample *next;

char *name;

int len;
short *buf;
};

struct PDGL_MixChannel_s {
PDGL_MixChannel *next;
char *name;	//sample/stream name

int flags;
float vol;

//float scale;
//float org[3]; //vol/(scale*|org-cam|)

//multipoint
int num_points;
int max_points;	//buffer size
float *points;	//x y z scale
float *vel;	//vx vy vz vw

//sound
PDGL_Sample *samp;
int offs, len;

//stream
void *data;
int (*getsamples)(short *mixbuf, int cnt, void *data);
};

PDGL_Sample *pdgl_sample_root=NULL;
int pdgl_sound_afd;

short *pdgl_sound_mixbuf;
PDGL_MixChannel *pdgl_sound_channels=NULL;
float pdgl_sound_vol;
float pdgl_sound_camorigin[3];
float pdgl_sound_camorigin2[3];
float pdgl_sound_camvelocity[3];

short *pdgl_sound_samplebuf;	//temporary buffer for holding samples

PDGL_Sample *PDGL_Sound_SampleFromBuf(char *name,
	byte *buf, int chan, int rate, int bits,
	int len)
{
	PDGL_Sample *tmp;

	int slen, scnt, scnt2;
	short *buf2, *buf3, *t;
	int i, j, k, l;

	byte *s, *s2;

	slen=chan*(bits/8);
	scnt=len/slen;
	buf2=kalloc(scnt*2);

	kprint("%d %d %d\n", chan, rate, bits);

	s=buf;
	t=buf2;
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
			}else l=((*s++)-128)*256;

			k+=l;
		}
		if(k<-32767)k=-32767;
		if(k>32767)k=32767;
		*t++=k;
	}

	if(rate!=44100)
	{
		scnt2=scnt*(44100.0/rate);
		buf3=kalloc(scnt2*2);

		PDGL_Sound_ScaleSample(buf3, scnt2, buf2, scnt);

		scnt=scnt2;
		kfree(buf2);
		buf2=buf3;
	}

#if 0
	buf3=kalloc(scnt*2);
	PDGL_SoundFilter_FilterHalfHaar16(buf2, buf3, scnt);

	s=kalloc(scnt*2);
	s2=kalloc(scnt*2);
	for(i=0; i<scnt; i++)
	{
		j=(buf2[i]/255.0)+127;
		if(j<0)j=0;
		if(j>255)j=255;
		s[i]=j;
	}

	i=scnt;
	PDGL_SoundFilter_FilterEncode(s, i);

	j=arith3_compress(s, s2, i);
	kprint("comp: %d samples %d->%d bytes\n", scnt, i, j);

//	memcpy(buf2, buf3, scnt*2);
	PDGL_SoundFilter_UnfilterHalfHaar16(buf3, buf2, scnt);
#endif


	tmp=kalloc(sizeof(PDGL_Sample));
	tmp->name=kstrdup(name);
	tmp->len=scnt;
	tmp->buf=buf2;

	tmp->next=pdgl_sample_root;
	pdgl_sample_root=tmp;

	return(tmp);
}

PDGL_Sample *PDGL_Sound_LoadWAV(char *name)
{
	VFILE *fd;
	PDGL_Sample *tmp;

	int i, j;
	unsigned int tag, len, rsrc;

	byte *buf;
	int chan, rate, bits;

	kprint("PDGL_Sound_LoadWAV: Loading sample '%s'\n", name);

	fd=vffopen(name, "rb");

	if(!fd)
	{
		kprint("PDGL_Sound_LoadWAV: Failed open sample '%s'\n", name);
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
		kprint("PDGL_Sound_LoadWAV: File Tag not RIFF\n");
		return(NULL);
	}
	if(rsrc!=PDGL_RIFF_WAVE)
	{
		kprint("PDGL_Sound_LoadWAV: Resource type not WAVE\n");
		return(NULL);
	}

	//fmt
	tag=0;
	len=0;
	for(i=0; i<4; i++)
		tag+=vfgetc(fd)<<(i*8);
	for(i=0; i<4; i++)
		len+=vfgetc(fd)<<(i*8);

	buf=kralloc(len);
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


	buf=kalloc(len);
	vfread(buf, 1, len, fd);

	//fill in sample
//	tmp->len=len/2;
//	tmp->buf=buf;

	tmp=PDGL_Sound_SampleFromBuf(name, buf, chan, rate, bits, len);
	kfree(buf);

//	kprint("Loaded sample '%s', %d samples\n", name, tmp->len);

	return(tmp);
}

#ifdef USE_MPGLIB
PDGL_Sample *PDGL_Sound_LoadMP3(char *name)
{
	VFILE *fd;
	PDGL_Sample *tmp;
	struct mpstr mp;

	int i, j;
	byte *buf, *s, *ibuf, *obuf;
	int chan, rate, bits, bitrate, lty;
	int len, ret, size, bufsize, ofs;

	kprint("PDGL_Sound_LoadMP3: Loading sample '%s'\n", name);

	fd=vffopen(name, "rb");

	if(!fd)
	{
		kprint("PDGL_Sound_LoadMP3: Failed open sample '%s'\n", name);
		return(NULL);
	}

	InitMP3(&mp);

	buf=malloc(10<<20);
	s=buf;

	ibuf=kalloc(16384);
	obuf=kalloc(16384);

	len=vfread(ibuf, 1, 16384, fd);
	ofs=statMP3(ibuf, &rate, &chan, &bitrate, &lty);
	bits=16;
	kprint("PDGL_Sound_LoadMP3: %d Hz, %d chan, %d bps, layer %d, ofs %d\n",
		rate, chan, bitrate, lty, ofs);

	vfseek(fd, 0, 2);
	size=vftell(fd);
	len=(size+((bitrate/8)-1))/(bitrate/8);
	bufsize=len*rate*chan*4; //2x est stream len
	kprint("PDGL_Sound_LoadMP3: aprox %d seconds, buf %d bytes\n",
		len, bufsize);

	vfseek(fd, ofs, 0);
	buf=malloc(bufsize);
	s=buf;

	while(!vfeof(fd))
	{
		len=vfread(ibuf, 1, 16384, fd);
		if(len<=0)break;

		ret = decodeMP3(&mp,ibuf,len,obuf,16384,&size);
		while(ret == MP3_OK)
		{
			memcpy(s, obuf, size);
			s+=size;

			ret = decodeMP3(&mp,NULL,0,obuf,16384,&size);
		}

		if((s-buf)>((bufsize*2)/3))
		{
			kprint("PDGL_Sound_LoadMP3: overflow\n");
			break;
		}
	}

	len=s-buf;
	kprint("PDGL_Sound_LoadMP3: decoded %f seconds\n",
		(float)len/(rate*chan*2));
	tmp=PDGL_Sound_SampleFromBuf(name, buf, chan, rate, bits, len);

	kfree(ibuf);
	kfree(obuf);
	free(buf);

	return(tmp);
}
#endif

PDGL_Sample *PDGL_Sound_LoadSample(char *name)
{
	char *s;

	s=name+strlen(name);
	while((s>name) && (*s!='.'))s--;

	if(*s=='.')s++;

	if(!strcmp(s, "wav"))return(PDGL_Sound_LoadWAV(name));
#ifdef USE_MPGLIB
	if(!strcmp(s, "mp3"))return(PDGL_Sound_LoadMP3(name));
#endif

	return(NULL);
}

PDGL_Sample *PDGL_Sound_FindSample(char *name)
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

PDGL_Sample *PDGL_Sound_GetSample(char *name)
{
	PDGL_Sample *tmp;

	tmp=PDGL_Sound_FindSample(name);
	if(tmp)return(tmp);

	tmp=PDGL_Sound_LoadSample(name);
	if(tmp)return(tmp);

	//fake sample
	tmp=kalloc(sizeof(PDGL_Sample));
	tmp->name=kstrdup(name);
	tmp->len=0;
	tmp->buf="";

	tmp->next=pdgl_sample_root;
	pdgl_sample_root=tmp;

	return(tmp);
}

int PDGL_Sound_ScaleSample(short *d, int dl, short *s, int sl)
{
	int i, j, s1, s2, v;
	float r, p1, p2;

	r=(((float)sl)/((float)dl));
	for(i=0; i<dl; i++)
	{
		p1=i*r;
		j=floor(p1);
		p2=p1-j;

		s1=s[j];
		s2=s[j+1];
		v=(s1*(1-p2))+(s2*p2);

		d[i]=v;
	}
	return(0);
}

int PDGL_Sound_PlaySample(char *name, int flags, float vol)
{
	PDGL_Sample *samp;
	PDGL_MixChannel *chan;

	samp=PDGL_Sound_GetSample(name);

	chan=kalloc(sizeof(PDGL_MixChannel));
	chan->name=kstrdup(name);

	chan->samp=samp;
	chan->offs=0;
	chan->len=samp->len;

	chan->flags=flags;
	chan->vol=vol;

	chan->next=pdgl_sound_channels;
	pdgl_sound_channels=chan;

	return(0);
}

int PDGL_Sound_PlayPointSample(char *name, int flags, float vol,
	float scale, float *org, float *vel)
{
	PDGL_Sample *samp;
	PDGL_MixChannel *chan;

	samp=PDGL_Sound_GetSample(name);

	chan=kalloc(sizeof(PDGL_MixChannel));
	chan->name=kstrdup(name);

	chan->samp=samp;
	chan->offs=0;
	chan->len=samp->len;

	chan->flags=flags|PDGL_SOUND_POINT;
	chan->vol=vol;

//	chan->scale=scale;
//	chan->org[0]=org[0];
//	chan->org[1]=org[1];
//	chan->org[2]=org[2];

	chan->max_points=4;
	chan->points=kalloc(chan->max_points*4*sizeof(float));
	chan->vel=kalloc(chan->max_points*4*sizeof(float));

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

	return(0);
}

int PDGL_Sound_PlayStream(char *name,
	int (*getsamples)(short *samples, int cnt, void *data), void *data,
	int flags, float vol)
{
	PDGL_MixChannel *chan;

	chan=kalloc(sizeof(PDGL_MixChannel));
	chan->name=kstrdup(name);

	chan->getsamples=getsamples;
	chan->data=data;

	chan->flags=flags;
	chan->vol=vol;

	chan->next=pdgl_sound_channels;
	pdgl_sound_channels=chan;

	return(0);
}

int PDGL_Sound_PlayPointStream(char *name,
	int (*getsamples)(short *samples, int cnt, void *data), void *data,
	int flags, float vol,
	float scale, float *org, float *vel)
{
	PDGL_MixChannel *chan;

	chan=kalloc(sizeof(PDGL_MixChannel));
	chan->name=kstrdup(name);

	chan->getsamples=getsamples;
	chan->data=data;

	chan->flags=flags|PDGL_SOUND_POINT;
	chan->vol=vol;

//	chan->scale=scale;
//	chan->org[0]=org[0];
//	chan->org[1]=org[1];
//	chan->org[2]=org[2];

	chan->max_points=4;
	chan->points=kalloc(chan->max_points*4*sizeof(float));
	chan->vel=kalloc(chan->max_points*4*sizeof(float));

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

	return(0);
}

int PDGL_Sound_CalcDoppler(
	float *lorg, float *lvel, float *sorg, float *svel, int cnt)
{
	float vl, vs, r, c;
	float nv[3];
	int i;

//	vl=Vl*N(Os-Ol)
//	vs=Vs*N(Ol-Os)

//	kprint("gen doppler (%f %f %f)\n", lorg[0], lorg[1], lorg[2]);
//	kprint("gen doppler (%f %f %f)\n", lvel[0], lvel[1], lvel[2]);

//	kprint("gen doppler (%f %f %f)\n", sorg[0], sorg[1], sorg[2]);
//	kprint("gen doppler (%f %f %f)\n", svel[0], svel[1], svel[2]);

	VecNF_Subtract(sorg, lorg, nv, 3);
	VecNF_Normalize(nv, nv, 3);
	vl=VecNF_DotProduct(lvel, nv, 3);
	vs=VecNF_DotProduct(svel, nv, 3);

//	r=((c+vl)/(c-vs))
//	Fl=Fs*r
//	Fs=Fl/r

	c=331;
	r=((c+vl)/(c-vs));

//	Ll=Ls/r
//	Ls=Ll*r

	i=cnt*r;
//	kprint("gen doppler %d %d\n", cnt, i);

	return(i);
}

int PDGL_Sound_CalcMoveOrg(
	float *iorg, float *ivel, float *forg, float dt)
{
	forg[0]=iorg[0]+ivel[0]*dt;
	forg[1]=iorg[1]+ivel[1]*dt;
	forg[2]=iorg[2]+ivel[2]*dt;

	return(0);
}

int PDGL_Sound_MixSamples(int cnt, float dt)
{
	int i, j, l, rcnt;
	PDGL_MixChannel *cur, *lst, *nxt;
	float vs, vsf, vt, vtf;
	float pf[3];

//	kprint("Mixing %d samples\n", cnt);

	if(cnt<0)cnt=0;
	if(cnt>(1<<19))cnt=(1<<19);

	for(i=0; i<cnt; i++)
		pdgl_sound_mixbuf[i]=0;

	if(!pdgl_sound_samplebuf)
		pdgl_sound_samplebuf=malloc(1<<20);

	cur=pdgl_sound_channels;
	lst=NULL;
	while(cur)
	{
		rcnt=cnt;

		vs=1;
		vsf=1;
		if(cur->flags&2)
		{
			vs=0;
			for(i=0; i<cur->num_points; i++)
				if(cur->points[(i*4)+3]>0)
			{
				vt=VecNF_Distance(cur->points+(i*4),
					pdgl_sound_camorigin, 3);
				vt*=cur->points[(i*4)+3];
				if(vt<1)vt=1;
				vt=1/vt;

				PDGL_Sound_CalcMoveOrg(
					cur->points+(i*4), cur->vel+(i*4), pf, dt);
				vtf=VecNF_Distance(pf, pdgl_sound_camorigin2, 3);
				vtf*=cur->points[(i*4)+3];
				if(vtf<1)vtf=1;
				vtf=1/vtf;

				if(vt>vs)
				{
					rcnt=PDGL_Sound_CalcDoppler(
						pdgl_sound_camorigin,
						pdgl_sound_camvelocity,
						cur->points+(i*4),
						cur->vel+(i*4),
						cnt);

					vs=vt;
					vsf=vtf;
				}
			}
		}

		if((rcnt<=0) || (rcnt>(1<<18)) || (vs<0.001))
		{
			lst=cur;
			cur=cur->next;
			continue;
		}

		for(i=0; i<rcnt; i++)pdgl_sound_samplebuf[i]=0;

		if(cur->getsamples)
		{
			i=cur->getsamples(pdgl_sound_samplebuf,
				rcnt, cur->data);
			if(i<0)cur->flags|=PDGL_SOUND_UNLINK;

			if(i<cnt)kprintd("Sound_MixSamples: Sample drain on "
				"stream '%s'\n", cur->name);
		}

		if(cur->samp)
		{
			if(cur->flags&1)
			{
				j=0;
				while(j<rcnt)
				{
					l=cur->len-cur->offs;
					if((rcnt-j)<l)l=(rcnt-j);
					for(i=0; i<l; i++)
						pdgl_sound_samplebuf[j+i]=
						cur->samp->buf[cur->offs+i];
					cur->offs+=l;
					if(cur->offs>=cur->len)cur->offs=0;

					j+=l;

//					kprint("loop %d %d\n", rcnt, j);
				}
			}else
			{
				l=cur->len-cur->offs;
				if(rcnt<l)l=rcnt;

				for(i=0; i<l; i++)
					pdgl_sound_samplebuf[i]=
						cur->samp->buf[cur->offs+i];
				cur->offs+=l;

				for(; i<rcnt; i++)
					pdgl_sound_samplebuf[i]=0;

				if(cur->offs>=cur->len)
					cur->flags|=PDGL_SOUND_UNLINK;
			}
#if 0
//			kprint("SFX %s\n", cur->samp->name);
			l=cur->len-cur->offs;
			if(rcnt<l)l=rcnt;

			for(i=0; i<l; i++)
				pdgl_sound_samplebuf[i]=
					cur->samp->buf[cur->offs+i];
			cur->offs+=l;

			if((cur->offs>=cur->len) && (cur->flags&1))
			{
				cur->offs=0;

				for(; i<rcnt; i++)
					pdgl_sound_samplebuf[i]=
						cur->samp->buf[cur->offs+(i-l)];
				cur->offs+=rcnt-l;
			}

			if((cur->offs>=cur->len) && !(cur->flags&1))
			{
				for(; i<rcnt; i++)
					pdgl_sound_samplebuf[i]=0;
				cur->flags|=PDGL_SOUND_UNLINK;
			}
#endif
		}

		if(rcnt!=cnt)
		{
//			kprint("doppler %d %d\n", cnt, rcnt);

			PDGL_Sound_ScaleSample(
				pdgl_sound_samplebuf+rcnt, cnt,
				pdgl_sound_samplebuf, rcnt);
			for(i=0; i<cnt; i++)
				pdgl_sound_samplebuf[i]=
					pdgl_sound_samplebuf[i+rcnt];
		}

		vt=(vsf-vs)/cnt;
		for(i=0; i<cnt; i++)
		{
			j=pdgl_sound_mixbuf[i]+
				(pdgl_sound_samplebuf[i]*cur->vol*
				pdgl_sound_vol*vs);
//			vs+=vt;

			if(j<-32767)j=-32767;
			if(j>32767)j=32767;
			pdgl_sound_mixbuf[i]=j;
		}

		if(cur->flags&PDGL_SOUND_UNLINK)
		{
			if(lst)lst->next=cur->next;
				else pdgl_sound_channels=cur->next;
			nxt=cur->next;
			kfree(cur);
			cur=nxt;
		}else
		{
			lst=cur;
			cur=cur->next;
		}
	}

	return(0);
}

int PDGL_Sound_WriteSamples(int cnt)
{
//	kprint("Writing %d samples\n", cnt);
//	write(pdgl_sound_afd, pdgl_sound_mixbuf, cnt*2);
	SoundDev_WriteSamples(pdgl_sound_mixbuf, cnt);
	return(0);
}

int PDGL_Sound_Pop()
{
	short s[2];

//	kprint("Writing %d samples\n", cnt);
//	write(pdgl_sound_afd, pdgl_sound_mixbuf, cnt*2);

	s[0]=-16384;
	s[1]=16384;
	SoundDev_WriteSamples(s, 2);
//	write(pdgl_sound_afd, s, 2*2);

	return(0);
}
int PDGL_Sound_CamOrigin(float x, float y, float z)
{
	pdgl_sound_camorigin[0]=x;
	pdgl_sound_camorigin[1]=y;
	pdgl_sound_camorigin[2]=z;
	return(0);
}

int PDGL_Sound_CamVelocity(float x, float y, float z)
{
	pdgl_sound_camvelocity[0]=x;
	pdgl_sound_camvelocity[1]=y;
	pdgl_sound_camvelocity[2]=z;
	return(0);
}

int PDGL_Sound_Frame(float dt)
{
	int c;

//	if(!pdgl_sound_channels)return(0);

	pdgl_sound_camorigin2[0]=
		pdgl_sound_camorigin[0]+pdgl_sound_camvelocity[0]*dt;
	pdgl_sound_camorigin2[1]=
		pdgl_sound_camorigin[1]+pdgl_sound_camvelocity[1]*dt;
	pdgl_sound_camorigin2[2]=
		pdgl_sound_camorigin[2]+pdgl_sound_camvelocity[2]*dt;

	while(dt>0.25)
	{
		c=0.25*44100;
		PDGL_Sound_MixSamples(c, dt);
		dt-=0.25;
	}

	c=dt*44100;
	PDGL_Sound_MixSamples(c, dt);
	PDGL_Sound_WriteSamples(c);
	return(0);
}

int PDGL_Sound_DeInit()
{
//	close(pdgl_sound_afd);
	SoundDev_DeInit();
	return(0);
}

void *pdgl_sound_playsound(PDSCR0_Context *ctx, void **args, int n)
{
	char *samp;
	float vol;

	samp=args[0];
	vol=NetVal_UnwrapFloat(args[1]);
	PDGL_Sound_PlaySample(samp, 0, vol);

	return(NULL);
}

void *pdgl_sound_playloop(PDSCR0_Context *ctx, void **args, int n)
{
	char *samp;
	float vol;

	samp=args[0];
	vol=NetVal_UnwrapFloat(args[1]);
	PDGL_Sound_PlaySample(samp, 1, vol);

	return(NULL);
}

void *pdgl_sound_playpointsound(PDSCR0_Context *ctx, void **args, int n)
{
	char *samp;
	float vol;
	float *org, *vel, scale;

	samp=args[0];
	vol=NetVal_UnwrapFloat(args[1]);
	org=args[2];
	vel=args[3];
	scale=NetVal_UnwrapFloat(args[4]);

	PDGL_Sound_PlayPointSample(samp, 0, vol, scale, org, vel);
	return(NULL);
}

void *pdgl_sound_playpointloop(PDSCR0_Context *ctx, void **args, int n)
{
	char *samp;
	float vol;
	float *org, *vel, scale;

	samp=args[0];
	vol=NetVal_UnwrapFloat(args[1]);
	org=args[2];
	vel=args[3];
	scale=NetVal_UnwrapFloat(args[4]);

	PDGL_Sound_PlayPointSample(samp, 1, vol, scale, org, vel);
	return(NULL);
}

int PDGL_Sound_Init()
{
	short *s;
	char *t;
	int rc;

	int i, j;

	kprint("PDGL_Sound: Init\n");

	pdgl_sound_mixbuf=malloc(1<<20);

#if 0
//	pdgl_sound_afd = open("/dev/dsp", O_WRONLY | O_NONBLOCK);
	pdgl_sound_afd = open("/dev/dsp", O_WRONLY);

	rc = ioctl(pdgl_sound_afd, SNDCTL_DSP_RESET, 0);
	if(rc<0)
		printf("fail reset\n");

	j=0; //0=mono, 1=stereo
	rc = ioctl(pdgl_sound_afd, SNDCTL_DSP_STEREO, &j);
	if(rc<0)
		printf("fail stereo\n");

	j=44100;
	rc = ioctl(pdgl_sound_afd, SNDCTL_DSP_SPEED, &j);
	if(rc<0)
		printf("fail speed\n");

	j=AFMT_S16_LE;	
        rc = ioctl(pdgl_sound_afd, SNDCTL_DSP_SETFMT, &j);
	if(rc<0)
		printf("fail fmt\n");

	PDGL_Sound_WriteSamples(10);
#endif

	SoundDev_Init();

	pdgl_sound_vol=1.0;

	PDSCR_Builtin_AddBuiltin("PlaySound",
		"(sample, volume), plays a sample",
		&pdgl_sound_playsound, 2);
	PDSCR_Builtin_AddBuiltin("PlayLoop",
		"(sample, volume), plays a sample in a loop",
		&pdgl_sound_playloop, 2);
	PDSCR_Builtin_AddBuiltin("PlayPointSound",
		"(sample, volume, origin, velocity, scale), plays a sample from a "
		"given point, with the distance scaled by scale",
		&pdgl_sound_playpointsound, 5);
	PDSCR_Builtin_AddBuiltin("PlayPointLoop",
		"(sample, volume, origin, velocity, scale), plays a looped sample "
		"from a given point, with the distance scaled by scale",
		&pdgl_sound_playpointloop, 5);

	return(0);
}
