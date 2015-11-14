#include <pdgl.h>

#ifndef FOURCC
#define FOURCC(a, b, c, d) ((a)|((b)<<8)|((c)<<16)|((d)<<24))
#endif

#define FCC_lqac	FOURCC('l', 'q', 'a', 'c')

uint BSLAC_ReadInt32LE(VFILE *fd)
{
	uint i;

	i=vfgetc(fd);
	i|=vfgetc(fd)<<8;
	i|=vfgetc(fd)<<16;
	i|=vfgetc(fd)<<24;
	return(i);
}

uint BSLAC_ReadFourcc(VFILE *fd)
{
	uint i;

	i=vfgetc(fd);
	i|=vfgetc(fd)<<8;
	i|=vfgetc(fd)<<16;
	i|=vfgetc(fd)<<24;
	return(i);
}

int BSLAC_UnFilterBlockPred(byte *ibuf, byte *obuf, int pred)
{
	int i, j, k, l, e;
	int a, b, c, p;

	a=ibuf[0];
	b=ibuf[1];
	c=ibuf[2];

	obuf[0]=a;
	obuf[1]=b;
	obuf[2]=c;

	e=0;
	for(i=3; i<256; i++)
	{
		k=ibuf[i];
		e+=abs(k);

		switch(pred)
		{
			case 0: p=0; break;
			case 1: p=c; break;
			case 2: p=c+0.5*(c-b); break;
			case 3: p=c+(c-b); break;
			case 4: p=(c+(2*(c-b)-(b-a))); break;
			default: p=0; break;
		}
		if(p>255)p=255;
		if(p<0)p=0;

		l=(byte)(p+k);

		k=(signed char)k;
		if(k>=0)l=p+k*k; else l=p-k*k;
		l=(byte)l;

		if(obuf)obuf[i]=l;

		a=b; b=c; c=l;
	}
	return(e);
}

int BSLAC_UnFilterBlock(byte *ibuf, byte *obuf)
{
	int i;
	i=BSLAC_UnFilterBlockPred(ibuf+1, obuf, ibuf[0]);
	return(i);
}

int BSLAC_UnFilterStream(byte *ibuf, byte *obuf, int len)
{
	byte *s, *t;
	int i, j, e;

	s=ibuf; t=obuf;
	i=len; e=0;
	while(i>256)
	{
		e+=BSLAC_UnFilterBlock(s, t);
		s+=257;
		t+=256;
		i-=256;
	}
	e+=BSLAC_UnFilterBlock(s, t);

	return(e);
}

PDGL_Sample *PDGL_Sound_LoadLQAC(char *name)
{
	VFILE *fd;
	PDGL_Sample *tmp;
	int i, j, len;
	byte *buf, *s, *ibuf, *obuf;

	printf("PDGL_Sound_LoadLQAC: Loading sample '%s'\n", name);

	fd=vffopen(name, "rb");

	if(!fd)
	{
		printf("PDGL_Sound_LoadLQAC: Failed open sample '%s'\n", name);
		return(NULL);
	}

	i=BSLAC_ReadFourcc(fd);
	len=BSLAC_ReadInt32LE(fd);

	if(i!=FCC_lqac)return(NULL);

	i=(len+(255))/256;
	ibuf=malloc(i*512);
	obuf=malloc(i*512);

	vfread(ibuf, 1, i*512, fd);
	vfclose(fd);

//	j=PDUNZ_DecodeStream(ibuf, obuf, i*512, i*257);
	j=vfInflateBuffer(obuf, ibuf, i*257, i*512);
	BSLAC_UnFilterStream(obuf, ibuf, len);

	tmp=PDGL_Sound_SampleFromBuf(name, ibuf, 1, 8000, 8, len);

	free(ibuf);
	free(obuf);

	return(tmp);
}
