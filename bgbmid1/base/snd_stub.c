#include <stdio.h>

#include <bgbmid.h>


// #ifdef linux
#if defined(linux) || defined(NATIVECLIENT) || defined(ANDROID)

BMID_API int BGBMID_SoundDev_DeInit()
{
}

BMID_API int BGBMID_SoundDev_Init()
{
}

BMID_API int BGBMID_SoundDev_GetDMAPos()
{
}

BMID_API void BGBMID_SoundDev_Submit()
{
}

BMID_API int BGBMID_SoundDev_PaintSamples(short *buffer, int cnt)
{
}

BMID_API int BGBMID_SoundDev_WriteRawSamples(
	short *buffer, int cnt, int ovcnt)
{
}

BMID_API int BGBMID_SoundDev_WriteStereoSamples(short *buffer, int cnt)
{
	return(BGBMID_SoundDev_WriteRawSamples(buffer, cnt*2, cnt*2));
}

BMID_API int BGBMID_SoundDev_WriteStereoSamples2(
	short *buffer, int cnt, int ovcnt)
{
	return(BGBMID_SoundDev_WriteRawSamples(buffer, cnt*2, ovcnt*2));
}

BMID_API int BGBMID_SoundDev_WriteSamples(short *buffer, int cnt)
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
	
	i=BGBMID_SoundDev_WriteRawSamples(tbuf, cnt*2, cnt*2);
	return(i);
}

#endif

