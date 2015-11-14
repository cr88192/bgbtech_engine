#include <stdio.h>

#include <pdgl.h>


// #ifdef linux
#if defined(linux) || defined(NATIVECLIENT) || defined(ANDROID)

PDGL_API int SoundDev_DeInit()
{
}

PDGL_API int SoundDev_Init()
{
}

PDGL_API int SoundDev_GetDMAPos()
{
}

PDGL_API void SoundDev_Submit()
{
}

PDGL_API int SoundDev_PaintSamples(short *buffer, int cnt)
{
}

PDGL_API int SoundDev_WriteRawSamples(short *buffer, int cnt)
{
}

PDGL_API int SoundDev_WriteStereoSamples(short *buffer, int cnt)
{
	return(SoundDev_WriteRawSamples(buffer, cnt*2));
}

PDGL_API int SoundDev_WriteSamples(short *buffer, int cnt)
{
	static short *tbuf=NULL;
	static int tbcnt=0;
	int i;
	
	if(!tbuf)
	{
		i=16384;
		while(cnt>i)i=i+(i>>1);
		tbuf=malloc(i*2*sizeof(short));
		tbcnt=i;
	}
	
	if(cnt>tbcnt)
	{
		i=tbcnt;
		while(cnt>i)i=i+(i>>1);
		tbuf=realloc(tbuf, i*2*sizeof(short));
		tbcnt=i;
	}
	
	for(i=0; i<cnt; i++)
	{
		tbuf[i*2+0]=buffer[i];
		tbuf[i*2+1]=buffer[i];
	}
	
	i=SoundDev_WriteRawSamples(tbuf, cnt*2);
	return(i);
}

#endif

