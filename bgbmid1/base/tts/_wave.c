#include <bsgen.h>

#define PDGL_RIFF_RIFF 0x46464952
#define PDGL_RIFF_WAVE 0x45564157
#define PDGL_RIFF_fmt  0x20746D66
#define PDGL_RIFF_data 0x61746164

BSG_API int BSWV_ScaleSample(short *d, int dl, short *s, int sl);

BSG_API byte *BSWV_LoadWAV(char *name, int *rch, int *rr, int *rbits, int *rlen)
{
	static byte tbuf[4096];
	void *fd;
	byte *buf;

	unsigned int tag, len, rsrc, fend, fstrt;
	int chan, rate, bits, slen;
	int i, j;

	fd=bsg_fopen(name, "rb");

	if(!fd)
	{
//		printf("BSWV_LoadWAV: Failed open sample '%s'\n", name);
		return(NULL);
	}

	tag=0; len=0; rsrc=0;
	for(i=0; i<4; i++)tag+=bsg_fgetc(fd)<<(i*8);
	for(i=0; i<4; i++)len+=bsg_fgetc(fd)<<(i*8);
	for(i=0; i<4; i++)rsrc+=bsg_fgetc(fd)<<(i*8);
	fstrt=bsg_ftell(fd);
	fend=fstrt+len;

	if(tag!=PDGL_RIFF_RIFF)
	{
		printf("PDGL_Sound_LoadWAV: File Tag not RIFF (%s)\n", name);
		bsg_fclose(fd);
		return(NULL);
	}
	if(rsrc!=PDGL_RIFF_WAVE)
	{
		printf("PDGL_Sound_LoadWAV: Resource type not WAVE (%s)\n", name);
		bsg_fclose(fd);
		return(NULL);
	}

	//fmt
	while(bsg_ftell(fd)<fend)
	{
		tag=0; len=0;
		for(i=0; i<4; i++)tag+=bsg_fgetc(fd)<<(i*8);
		for(i=0; i<4; i++)len+=bsg_fgetc(fd)<<(i*8);

		if(tag!=PDGL_RIFF_fmt)
		{
			len=(len+1)&~1;
			bsg_fseek(fd, len, 1);
			continue;
		}

		bsg_fread(tbuf, 1, len, fd);

		if((tbuf[0]!=1) || (tbuf[1]!=0))
		{
			printf("PDGL_Sound_LoadWAV: WAVE not PCM (%s)\n",
				name);
			bsg_fclose(fd);
			return(NULL);
		}

		chan=tbuf[2];
		rate=tbuf[4]+(tbuf[5]<<8);
		bits=tbuf[14];

		break;
	}


	//data
	buf=NULL;
	bsg_fseek(fd, fstrt, 0);
	while(bsg_ftell(fd)<fend)
	{
		tag=0; len=0;
		for(i=0; i<4; i++)tag+=bsg_fgetc(fd)<<(i*8);
		for(i=0; i<4; i++)len+=bsg_fgetc(fd)<<(i*8);

		if(tag!=PDGL_RIFF_data)
		{
			len=(len+1)&~1;
			bsg_fseek(fd, len, 1);
			continue;
		}

		buf=malloc(len);
		bsg_fread(buf, 1, len, fd);

		i=chan*(bits/8);
		slen=len/i;

		break;
	}

	bsg_fclose(fd);

	*rch=chan;
	*rr=rate;
	*rbits=bits;
	*rlen=slen;
	return(buf);
}

BSG_API void BSWV_StoreWAV(char *name, byte *buf, int ch, int rt, int bits, int len)
{
	void *fd;
	int i;

	fd=bsg_fopen(name, "wb");
	bsg_fputc('R', fd);	bsg_fputc('I', fd);	bsg_fputc('F', fd);	bsg_fputc('F', fd);
	bsg_fputc(0, fd);	bsg_fputc(0, fd);	bsg_fputc(0, fd);	bsg_fputc(0, fd);
	bsg_fputc('W', fd);	bsg_fputc('A', fd);	bsg_fputc('V', fd);	bsg_fputc('E', fd);

	//format header
	bsg_fputc('f', fd);	bsg_fputc('m', fd);	bsg_fputc('t', fd);	bsg_fputc(' ', fd);
	bsg_fputc(16, fd);	bsg_fputc(0, fd);	bsg_fputc(0, fd);	bsg_fputc(0, fd);

	bsg_fputc(1, fd);		bsg_fputc(0, fd);
	bsg_fputc(ch, fd);		bsg_fputc(0, fd);

	bsg_fputc(rt&0xFF, fd);	bsg_fputc((rt>>8)&0xFF, fd);
	bsg_fputc(0, fd);		bsg_fputc(0, fd);

	i=rt*ch*(bits/8);
	bsg_fputc(i&0xFF, fd);		bsg_fputc((i>>8)&0xFF, fd);
	bsg_fputc((i>>16)&0xFF, fd);	bsg_fputc((i>>24)&0xFF, fd);

	bsg_fputc(ch*(bits/8), fd);		bsg_fputc(0, fd);
	bsg_fputc(bits, fd);		bsg_fputc(0, fd);


	//data
	bsg_fputc('d', fd);	bsg_fputc('a', fd);	bsg_fputc('t', fd);	bsg_fputc('a', fd);

	i=len*ch*(bits/8);
	bsg_fputc(i&0xFF, fd);		bsg_fputc((i>>8)&0xFF, fd);
	bsg_fputc((i>>16)&0xFF, fd);	bsg_fputc((i>>24)&0xFF, fd);

	bsg_fwrite(buf, 1, i, fd);
	if(i)bsg_fputc(0, fd);

	i=bsg_ftell(fd);
	bsg_fseek(fd, 4, 0);

	i-=8;	
	bsg_fputc(i&0xFF, fd);		bsg_fputc((i>>8)&0xFF, fd);
	bsg_fputc((i>>16)&0xFF, fd);	bsg_fputc((i>>24)&0xFF, fd);

	bsg_fclose(fd);
}

BSG_API short *BSWV_LoadWAV_Mono16(char *name, int *rrt, int *rlen)
{
	byte *bpb;
	short *bp, *bp1;
	int ch, rt, bts, len;
	int i, j, k;

	bpb=BSWV_LoadWAV(name, &ch, &rt, &bts, &len);
	if(!bpb)return(NULL);

	bp=NULL;
	if(bts==8)
	{
		bp=malloc(ch*len*sizeof(short));

		for(i=0; i<(ch*len); i++)
		{
			j=bpb[i]; j=(j-128)*256;
			bp[i]=j;
		}

		free(bpb);
	}
	if(bts==16)bp=(short *)bpb;

	if(!bp)
	{
		free(bpb);
		printf("PDGL_Sound_LoadWAV_44Mono16: "
			"WAVE not 8 or 16 bits (%s)\n", name);
		return(NULL);
	}

	if(ch<1)
	{
		free(bpb);
		printf("PDGL_Sound_LoadWAV_44Mono16: "
			"WAVE bad number of channels (%s)\n", name);
		return(NULL);
	}

	if(ch>1)
	{
		bp1=malloc(len*sizeof(short));
		for(i=0; i<len; i++)
		{
			k=0;
			for(j=0; j<ch; j++)
				k+=bp[i*ch+j];
			if(k<-32767)k=-32767;
			if(k>32767)k=32767;
			bp1[i]=k;
		}

		free(bp);
		bp=bp1;
	}

	*rrt=rt;
	*rlen=len;
	return(bp);
}

BSG_API short *BSWV_LoadWAV_44Mono16(char *name, int *rlen)
{
	short *bp, *bp1;
	int rt, len;
	int i, j, k;

	bp=BSWV_LoadWAV_Mono16(name, &rt, &len);
	if(!bp)return(NULL);

	if(rt!=44100)
	{
		i=len*(44100.0/rt);
		bp1=malloc(i*sizeof(short));

		BSWV_ScaleSample(bp1, i, bp, len);

		free(bp);
		bp=bp1;
		len=i;
	}

	*rlen=len;
	return(bp);
}

BSG_API short *BSWV_LoadWAV_16Mono16(char *name, int *rlen)
{
	short *bp, *bp1;
	int rt, len;
	int i, j, k;

	bp=BSWV_LoadWAV_Mono16(name, &rt, &len);
	if(!bp)return(NULL);

	if(rt!=16000)
	{
		i=len*(16000.0/rt);
		bp1=malloc(i*sizeof(short));

		BSWV_ScaleSample(bp1, i, bp, len);

		free(bp);
		bp=bp1;
		len=i;
	}

	*rlen=len;
	return(bp);
}


BSG_API int BSWV_NormalizeSample(short *dst, short *src, int len)
{
	float c, r;
	int i, j, k;

	c=0;
	for(i=0; i<len; i++)
		c+=src[i];
	c/=len;

	r=0;
	for(i=0; i<len; i++)
		r+=fabs(src[i]-c);
	r/=len;

	if(r<1)r=1;

	for(i=0; i<len; i++)
	{
		j=(src[i]-c)*(4096.0/r);
		if(j>32767)j=32767;
		if(j<-32767)j=-32767;
		dst[i]=j;
	}

	return(0);
}

int BSWV_ScaleSampleTrilinear(short *d, int dl, short *s, int sl)
{
	float *tb, *tb1, *tb2;
	float s1, s2, s3;
	float r, r1, p1, p2, f, g;
	int v, tl;
	int i, j, k;

	//first: upscale sample to a power of 2 size
	tl=1;
	while(sl>tl)tl<<=1;

	tb=malloc(tl*sizeof(float));
	tb1=malloc(tl*sizeof(float));

//	for(i=0; i<sl; i++)
//		tb[i]=s[i];
//	BSWV_ScaleSample(tb, tl, s, sl);

	r=(((float)(sl-1))/((float)tl));
	for(i=0; i<tl; i++)
	{
		p1=i*r; j=floor(p1);
		p2=p1-j;

		s1=s[j]; s2=s[j+1];
		f=(s1*(1-p2))+(s2*p2);
		tb[i]=f;
	}


	//second: downscale by powers of 2

	while(tl>dl)
	{
		tl>>=1;
		for(j=0; j<tl; j++)
		{
			f=(tb[j*2+0]+tb[j*2+1])/2;
			tb1[j]=f;
		}

		tb2=tb; tb=tb1; tb1=tb2;
	}

	if(tl==dl)
	{
		//temp len == dest len
		for(i=0; i<dl; i++)
		{
			v=tb[i];
			if(v<(-32767))v=-32767;
			if(v>32767)v=32767;
			d[i]=v;
		}
		return(0);
	}

	//use trilinear upsample filter
	r=(((float)tl)/((float)dl));
	r1=(((float)sl)/((float)dl));
	for(i=0; i<dl; i++)
	{
		p1=i*r;
		j=floor(p1);
		p2=p1-j;

		k=floor(i*r1+0.5);

		s1=tb[j];
		s2=tb[j+1];
		s3=s[k];
//		v=((s1*(1-p2))+(s2*p2))*0.666666+(s3*0.333333);
		v=((s1*(1-p2))+(s2*p2))*0.5+(s3*0.5);

		if(v<(-32767))v=-32767;
		if(v>32767)v=32767;
		d[i]=v;
	}

	free(tb);
	free(tb1);

	return(0);
}

BSG_API int BSWV_ScaleSample(short *d, int dl, short *s, int sl)
{
	int i, j, k, s1, s2, v;
	float r, p1, p2;

	if(dl==sl)
	{
		for(i=0; i<sl; i++)d[i]=s[i];
		return(0);
	}

	//upsample, linear
	if(dl>sl)
	{
		r=(((float)(sl-1))/((float)dl));
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

#if 0
	r=(((float)sl)/((float)dl));
	for(i=0; i<dl; i++)
	{
		j=floor(i*r);
		d[i]=s[j];
	}
	return(0);
#endif

	//downsample, trilinear
	BSWV_ScaleSampleTrilinear(d, dl, s, sl);
}


int BSWV_SampleCompressor(short *db, int dl, short *sb, int sl,
	int ivl, int isl, int irl)
{
//	static short *buf1=NULL;
//	static short *buf2=NULL;
	short *buf1, *buf2, *buf3;
	int sp, dp;
	int i, j, k, l;

	i=irl*3*sizeof(short);
	buf1=malloc(i);
	buf2=malloc(i);
	memset(buf1, 0, i);
	memset(buf2, 0, i);

	buf3=malloc(ivl*sizeof(short));

	sp=0; dp=0;
	while((sp<sl) && (dp<dl))
	{
		for(i=0; i<ivl; i++)
			buf3[i]=sb[sp+(i%(sl-sp))];

		j=isl; k=0;
		while(k<irl)
		{
			BSWV_ScaleSample(buf2+k, isl, buf3, ivl);

			if(k)
			{
				l=(buf2[k-1]+buf2[k])/2;
				buf2[k-1]=(buf2[k-1]+l)/2;
				buf2[k-2]=(buf2[k-2]+buf2[k-1])/2;
				buf2[k-3]=(buf2[k-3]+buf2[k-2])/2;
				buf2[k]=(buf2[k]+l)/2;
				buf2[k+1]=(buf2[k+1]+buf2[k])/2;
				buf2[k+2]=(buf2[k+2]+buf2[k+1])/2;
				buf2[k+3]=(buf2[k+3]+buf2[k+2])/2;
			}

			k+=isl;
		}

		j=irl*2;
		for(i=0; i<j; i++)
		{
			k=buf1[i+irl];
			k+=buf2[i];
			if(k<(-32767))k=-32767;
			if(k>32767)k=32767;
			buf1[i]=k;
		}

		for(i=0; i<irl; i++)db[dp++]=buf1[i];
		sp+=ivl;
	}

	free(buf1);
	free(buf2);
	free(buf3);

	return(0);
}
