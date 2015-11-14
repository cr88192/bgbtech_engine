#define SND_ENABLE_OGG
// #define SND_RESAMP_BGBMID

#include <pdgl.h>

#ifdef SND_ENABLE_OGG

#include <ivorbiscodec.h>
#include <ivorbisfile.h>

#ifdef SND_RESAMP_BGBMID
#include <bgbmid.h>
#endif

//HACK: deal with matter of FILE IO not working between DLL's
static FILE *s_ogg_open(char *name)
{
	char tb[256];
	FILE *fd;
	VFILE *vfd;
	byte *buf;
	char *s;
	int hi, sz;
	
//	sprintf(tb, "%s/%s", FS_Gamedir(), name);
//	fd=ov_fopen(tb, "rb");
//	if(fd)return(fd);

//	sprintf(tb, "%s/%s", "baseq2", name);
//	fd=ov_fopen(tb, "rb");
//	if(fd)return(fd);

	sprintf(tb, "%s/%s", "resource", name);
	fd=ov_fopen(tb, "rb");
	if(fd)return(fd);

	fd=ov_fopen(name, "rb");
	if(fd)return(fd);

	//see if this can be resolved to an existent OS file
	s=vfTryFindOSPath(name, "rb");
	if(s)
	{
		fd=ov_fopen(s, "rb");
		if(fd)return(fd);
	}

#if 1
	//hack...
	//try to cache VFS files somewhere accessible to fopen
	s=name; hi=0;
	while(*s)hi=hi*251+(*s++);
	hi=((hi*251)>>8)&0xFFFFFF;

	sprintf(tb, "resource/sound/tmp_%06X", hi);
	fd=ov_fopen(tb, "rb");
	if(fd)return(fd);

	vfd=vffopen(name, "rb");
	if(vfd)
	{
		buf=vf_bufferin_sz(vfd, &sz);
		vfclose(vfd);

		if(buf)
		{
			fd=fopen(tb, "wb");
			if(fd)
			{
				fwrite(buf, 1, sz, fd);
				fclose(fd);
			}

			fd=ov_fopen(tb, "rb");
			if(fd)return(fd);
		}
		return(NULL);
	}
#endif
	
	return(NULL);
}

PDGL_Sample *PDGL_Sound_LoadOGG(char *name)
{
	FILE *fd;
	PDGL_Sample *tmp;
	OggVorbis_File vf;
    vorbis_info *vi;
	int current_section;

	int i, j, k;
	byte *buf, *s, *ibuf, *obuf, *ebuf;
	short *sbuf, *tbuf;
	int chan, rate, bits, bitrate, lty;
	int len, ret, size, bufsize, ofs;

//	printf("PDGL_Sound_LoadMP3: Loading sample '%s'\n", name);

//	fd=fopen(name, "rb");
//	size = FS_FOpenFile (name, &fd);
//	fd=ov_fopen(argv[1], "rb");

	fd=s_ogg_open(name);

//	if(!fd)
	if((size<0) || !fd)
	{
		printf("PDGL_Sound_LoadOGG: Failed open sample '%s'\n", name);
		return(NULL);
	}

//	fgetc(fd);
//	fseek(fd, 0, 0);

	if(ov_open(fd, &vf, NULL, 0) < 0) {
		printf("Input %s does not appear to be an Ogg bitstream.\n", name);
		return(NULL);
	}

	vi=ov_info(&vf,-1);
	chan=vi->channels;
	rate=vi->rate;
	bits=16;

	buf=malloc(1<<20); ebuf=buf+(1<<20);
	s=buf;

//	ibuf=malloc(16384);
	obuf=malloc(16384);
	while(1)
	{
		ret=ov_read(&vf, obuf, 16384, &current_section);
		if (ret<=0)
			break;

		size=ret;
		if((s+size)>=ebuf)
		{
			i=s-buf;
			j=ebuf-buf;
			while((i+size)>=j)j=j+(j>>1);
			buf=realloc(buf, j);
			ebuf=buf+j;
			s=buf+i;
		}

		//fwrite(pcmout,1,i,ofd);
		memcpy(s, obuf, size);
		s+=size;
	}

//	InitMP3(&mp);
//	buf=malloc(10<<20);
//	s=buf;

#if 0
	ibuf=malloc(16384);
	obuf=malloc(16384);

	len=fread(ibuf, 1, 16384, fd);
	ofs=statMP3(ibuf, &rate, &chan, &bitrate, &lty);
	bits=16;
	printf("PDGL_Sound_LoadMP3: %d Hz, %d chan, %d bps, layer %d, ofs %d\n",
		rate, chan, bitrate, lty, ofs);

	fseek(fd, 0, 2);
	size=ftell(fd);
	len=(size+((bitrate/8)-1))/(bitrate/8);
	bufsize=len*rate*chan*4; //2x est stream len
	printf("PDGL_Sound_LoadMP3: aprox %d seconds, buf %d bytes\n",
		len, bufsize);

	fseek(fd, ofs, 0);
	buf=malloc(bufsize);
	s=buf;

	while(!feof(fd))
	{
		len=fread(ibuf, 1, 16384, fd);
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
			Com_DPrintf("PDGL_Sound_LoadMP3: overflow\n");
			break;
		}
	}
#endif

//	fclose(fd);
//	FS_FCloseFile (fd);

	ov_fclose(fd);

	len=s-buf;
	printf("PDGL_Sound_LoadOGG: decoded %f seconds\n",
		(float)len/(rate*chan*2));
//	tmp=PDGL_Sound_SampleFromBuf(name, buf, chan, rate, bits, len);

	len/=chan*2;

	if(chan>1)
	{
		//coerce to mono
		sbuf=(short *)buf;
		for(i=0; i<len; i++)
		{
			j=(sbuf[i*chan+0]+sbuf[i*chan+1])/2;
			if(j<(-32767))j=-32767;
			if(j>32767)j=32767;
			sbuf[i]=j;
		}
	}

#ifdef SND_RESAMP_BGBMID
	if(rate!=44100)
	{
		i=len*(((double)44100)/rate);
		sbuf=(short *)buf;
		tbuf=(short *)malloc(i*sizeof(short));
		BGBMID_ScaleSampleHQ(tbuf, i, sbuf, len);
		free(buf);
		buf=(byte *)tbuf;
		len=i; rate=44100;
	}
#endif

	tmp=PDGL_Sound_SampleFromBuf(name, buf, 1, rate, 16, len);

//	free(ibuf);
	free(obuf);
	free(buf);

	return(tmp);
}

#endif
