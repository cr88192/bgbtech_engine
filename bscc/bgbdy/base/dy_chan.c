#include <bgbgc.h>
#include <bgbdy.h>

static void *dy_chan_mutex=NULL;

static void chn_lock()
{
	thLockMutex(dy_chan_mutex);
}

static void chn_unlock()
{
	thUnlockMutex(dy_chan_mutex);
}

BGBDY_API dyt chnNew()
{
	BGBGC_Chan *chn;

	if(!dy_chan_mutex)
		dy_chan_mutex=thMutex();

	chn=gctalloc("_chan_t", sizeof(BGBGC_Chan));
	return((dyt)chn);
}

BGBDY_API void chnGetBuf(dyt chan, void *buf, int sz)
{
	BGBGC_Chan *chn;

	chn=(BGBGC_Chan *)chan;

	chn_lock();
	if((chn->set>0) && (chn->get<0))
	{
		chn->get++;
		memcpy(buf, (void *)chn->buf, sz);
		chn->set--;
		chn_unlock();
		return;
	}
	chn_unlock();

//	while(chn->get)
//		thSleep(0);

	while(1)
	{
		chn_lock();
		if(!chn->get)break;
		chn_unlock();
		thSleep(0);
	}
	chn->get++;
	chn_unlock();

//	while(!chn->set)
//		thSleep(0);

	while(1)
	{
		chn_lock();
		if(chn->set)break;
		chn_unlock();
		thSleep(0);
	}
	memcpy(buf, (void *)chn->buf, sz);
	chn->set--;
	chn->gh=0;
	chn_unlock();
}

BGBDY_API void chnPutBuf(dyt chan, void *buf, int sz)
{
	BGBGC_Chan *chn;

	chn=(BGBGC_Chan *)chan;

//	while(chn->set)
//		thSleep(0);

	while(1)
	{
		chn_lock();
		if(!chn->set)break;
		chn_unlock();
		thSleep(0);
	}
	memcpy((void *)chn->buf, buf, sz);
	chn->set++;
	chn_unlock();

//	while(!chn->get)
//		thSleep(0);
//	chn->get--;

	while(1)
	{
		chn_lock();
		if(chn->get)break;
		chn_unlock();
		thSleep(0);
	}
	chn->get--;
	chn->sh=0;
	chn_unlock();
}

BGBDY_API void chnSendBuf(dyt chan, void *buf, int sz)
{
	BGBGC_Chan *chn;

	chn=(BGBGC_Chan *)chan;

//	while(chn->set)
//		thSleep(0);

	while(1)
	{
		chn_lock();
		if(!chn->set)break;
		chn_unlock();
		thSleep(0);
	}

	if(chn->get>0)
	{
		chn_unlock();

		chnPutBuf(chan, buf, sz);
		return;
	}

	chn->get=-1;
	memcpy((void *)chn->buf, buf, sz);
	chn->set++;
	chn->sh=0;
	chn_unlock();
}

BGBDY_API int chnPollGetP(dyt chan)
{
	BGBGC_Chan *chn;

	chn=(BGBGC_Chan *)chan;
	if(chn->gh)return(0);
	if(chn->get>0)return(0);
	if(chn->set>0)return(1);
	return(0);
}

BGBDY_API int chnPollPutP(dyt chan)
{
	BGBGC_Chan *chn;

	chn=(BGBGC_Chan *)chan;
	if(chn->sh)return(0);
	if(chn->get>0)return(1);
	return(0);
}

BGBDY_API int chnGetP(dyt chan)
{
	BGBGC_Chan *chn;

	chn=(BGBGC_Chan *)chan;
	chn_lock();
	if(chn->gh)	{ chn_unlock(); return(0); }
	if(chn->set>0)	{ chn_unlock(); return(1); }
	chn->gh=1;
	chn_unlock();
	return(0);
}

BGBDY_API int chnPutP(dyt chan)
{
	BGBGC_Chan *chn;

	chn=(BGBGC_Chan *)chan;
	chn_lock();
	if(chn->sh)	{ chn_unlock(); return(0); }
	if(chn->get>0)	{ chn_unlock(); return(1); }
	chn->sh=1;
	chn_unlock();
	return(0);
}



BGBDY_API int chnGetInt(dyt chan)
	{ int v; chnGetBuf(chan, &v, sizeof(int)); return(v); }
BGBDY_API s64 chnGetLong(dyt chan)
	{ s64 v; chnGetBuf(chan, &v, sizeof(s64)); return(v); }
BGBDY_API float chnGetFloat(dyt chan)
	{ float v; chnGetBuf(chan, &v, sizeof(float)); return(v); }
BGBDY_API double chnGetDouble(dyt chan)
	{ double v; chnGetBuf(chan, &v, sizeof(double)); return(v); }
BGBDY_API void *chnGetPtr(dyt chan)
	{ void *v; chnGetBuf(chan, &v, sizeof(void *)); return(v); }

BGBDY_API char *chnGetString(dyt chan)	{ return(chnGetPtr(chan)); }
BGBDY_API dyt chanGetDyt(dyt chan)	{ return(chnGetPtr(chan)); }


BGBDY_API void chnPutInt(dyt chan, int val)
	{ chnPutBuf(chan, &val, sizeof(int)); }
BGBDY_API void chnPutLong(dyt chan, s64 val)
	{ chnPutBuf(chan, &val, sizeof(s64)); }
BGBDY_API void chnPutFloat(dyt chan, float val)
	{ chnPutBuf(chan, &val, sizeof(float)); }
BGBDY_API void chnPutDouble(dyt chan, double val)
	{ chnPutBuf(chan, &val, sizeof(double)); }
BGBDY_API void chnPutPtr(dyt chan, void *val)
	{ chnPutBuf(chan, &val, sizeof(void *)); }

BGBDY_API void chnPutString(dyt chan, char *val)
	{ chnPutPtr(chan, val); }
BGBDY_API void chnPutDyt(dyt chan, dyt val)
	{ chnPutPtr(chan, val); }

BGBDY_API void chnSendInt(dyt chan, int val)
	{ chnSendBuf(chan, &val, sizeof(int)); }
BGBDY_API void chnSendLong(dyt chan, s64 val)
	{ chnSendBuf(chan, &val, sizeof(s64)); }
BGBDY_API void chnSendFloat(dyt chan, float val)
	{ chnSendBuf(chan, &val, sizeof(float)); }
BGBDY_API void chnSendDouble(dyt chan, double val)
	{ chnSendBuf(chan, &val, sizeof(double)); }
BGBDY_API void chnSendPtr(dyt chan, void *val)
	{ chnSendBuf(chan, &val, sizeof(void *)); }

BGBDY_API void chnSendString(dyt chan, char *val)
	{ chnSendPtr(chan, val); }
BGBDY_API void chnSendDyt(dyt chan, dyt val)
	{ chnSendPtr(chan, val); }


