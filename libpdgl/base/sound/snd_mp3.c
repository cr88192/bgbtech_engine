#include <pdgl.h>

#include <mpg123.h>
#include <mpglib.h>

PDGL_Sample *PDGL_Sound_LoadMP3(char *name)
{
	VFILE *fd;
	PDGL_Sample *tmp;
	struct mpstr mp;

	int i, j;
	byte *buf, *s, *ibuf, *obuf;
	int chan, rate, bits, bitrate, lty;
	int len, ret, size, bufsize, ofs;

	printf("PDGL_Sound_LoadMP3: Loading sample '%s'\n", name);

	fd=vffopen(name, "rb");

	if(!fd)
	{
		printf("PDGL_Sound_LoadMP3: Failed open sample '%s'\n", name);
		return(NULL);
	}

	InitMP3(&mp);

	buf=malloc(10<<20);
	s=buf;

	ibuf=gcatomic(16384);
	obuf=gcatomic(16384);

	len=vfread(ibuf, 1, 16384, fd);
	ofs=statMP3(ibuf, &rate, &chan, &bitrate, &lty);
	bits=16;
	printf("PDGL_Sound_LoadMP3: %d Hz, %d chan, %d bps, layer %d, ofs %d\n",
		rate, chan, bitrate, lty, ofs);

	vfseek(fd, 0, 2);
	size=vftell(fd);
	len=(size+((bitrate/8)-1))/(bitrate/8);
	bufsize=len*rate*chan*4; //2x est stream len
	printf("PDGL_Sound_LoadMP3: aprox %d seconds, buf %d bytes\n",
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
			printf("PDGL_Sound_LoadMP3: overflow\n");
			break;
		}
	}

	len=s-buf;
	printf("PDGL_Sound_LoadMP3: decoded %f seconds\n",
		(float)len/(rate*chan*2));
	tmp=PDGL_Sound_SampleFromBuf(name, buf, chan, rate, bits, len);

	gcfree(ibuf);
	gcfree(obuf);
	free(buf);

	return(tmp);
}
