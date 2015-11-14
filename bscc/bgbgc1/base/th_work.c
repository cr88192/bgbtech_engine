#include <bgbgc.h>

BGBGC_Job *bgbgc_workqueue_first=NULL;
BGBGC_Job *bgbgc_workqueue_last=NULL;

BGBGC_Job *bgbgc_workqueue_delay=NULL;
BGBGC_Job *bgbgc_workqueue_blocked=NULL;

void *bgbgc_workqueue_mutex=NULL;

volatile int bgbgc_workqueue_workers=0;
volatile int bgbgc_workqueue_pendingworkers=0;

int bgbgc_workqueue_lastspawn=0;

BGBGC_WorkerInfo *(*btge_spawnworker[256])(void *data);
int btge_n_spawnworker=0;

BGBGC_API int BGBGC_RegisterSpawnWorker(
	BGBGC_WorkerInfo *(*spawn)(void *data))
{
	int i;
	
	i=btge_n_spawnworker++;
	btge_spawnworker[i]=spawn;
	return(i+1);
}

int BGBGC_TimeMS()
{
	return(clock()*(1000.0/CLOCKS_PER_SEC));
}

int BGBGC_DoWorkMS(int ms)
{
	BGBGC_Job *job;
	int i, t, st;

	return(0);

	if(!bgbgc_workqueue_workers)
		bgbgc_workqueue_workers++;

	st=BGBGC_TimeMS();
	while(1)
	{
		job=NULL;
		thLockFastMutex(bgbgc_workqueue_mutex);
		if(bgbgc_workqueue_first)
		{
			job=bgbgc_workqueue_first;
			bgbgc_workqueue_first=job->next;
			if(!bgbgc_workqueue_first)
				bgbgc_workqueue_last=NULL;
		}
		thUnlockFastMutex(bgbgc_workqueue_mutex);
		
		if(!job)break;
		
		job->val=job->func(job->data);
		job->done=1;
		
		if(job->val==BGBGC_NULLEND)
			gcfree(job);

		t=BGBGC_TimeMS();
		i=t-st;
		if((i<0) || (i>ms))break;
	}
	
	return(t-st);
}

BGBGC_Job *BGBGC_WorkerGetJob(BGBGC_WorkerInfo *info)
{
	BGBGC_Job *job, *jcur, *jprv;

	job=NULL;
	thLockFastMutex(bgbgc_workqueue_mutex);
	if(bgbgc_workqueue_first)
	{
		if(info && info->workerId)
		{
			jcur=bgbgc_workqueue_first; jprv=NULL;
			while(jcur)
			{
				if(jcur->workerId==info->workerId)
					break;
				jprv=jcur;
				jcur=jcur->next;
			}
		}else
		{
			jcur=bgbgc_workqueue_first; jprv=NULL;
			while(jcur)
			{
				if(!jcur->workerId)
					break;
				jprv=jcur;
				jcur=jcur->next;
			}
		}

		job=jcur;
		if(job)
		{
			if(jprv)
			{
				jprv->next=jcur->next;
				if(!jprv->next)
					bgbgc_workqueue_last=jprv;
			}else
			{
				bgbgc_workqueue_first=job->next;
				if(!bgbgc_workqueue_first)
					bgbgc_workqueue_last=NULL;
			}
		}

#if 0
		job=bgbgc_workqueue_first;
		bgbgc_workqueue_first=job->next;
		if(!bgbgc_workqueue_first)
			bgbgc_workqueue_last=NULL;
#endif
	}
	thUnlockFastMutex(bgbgc_workqueue_mutex);
		
	return(job);
}

int BGBGC_WorkerLoop(void *p)
{
	static int lastjob;
	BGBGC_WorkerInfo *info;
	BGBGC_Job *job, *jcur, *jprv;
	int i, t;

	info=p;

	if(info && info->begin)
		info->begin(info);

	bgbgc_workqueue_pendingworkers--;
	bgbgc_workqueue_workers++;
	lastjob=BGBGC_TimeMS();
	while(1)
	{
		job=BGBGC_WorkerGetJob(info);
		
		if(!job)
		{
			t=BGBGC_TimeMS();
			i=t-lastjob;
//			if(i>1000)
			if(i>(30*1000))
				break;
			thSleep(10);
			continue;
		}
		
		lastjob=BGBGC_TimeMS();
		job->val=job->func(job->data);
		job->done=1;
		
		if(job->val==BGBGC_NULLEND)
			gcfree(job);
	}
	
	if(info && info->end)
		info->end(info);
	
	bgbgc_workqueue_workers--;
	return(0);
}

void BGBGC_AddJobEnd(BGBGC_Job *job)
{
	if(bgbgc_workqueue_last)
	{
		bgbgc_workqueue_last->next=job;
		bgbgc_workqueue_last=job;
	}else
	{
		bgbgc_workqueue_first=job;
		bgbgc_workqueue_last=job;
	}
}

void BGBGC_DispatchDelayWork(int dt)
{
	BGBGC_Job *cur, *prv, *tmp;
	
	cur=bgbgc_workqueue_delay; prv=NULL;
	while(cur)
	{
		cur->time-=dt;
		if(cur->time<=0)
		{
			if(prv)
			{
				tmp=cur;
				prv->next=cur->next;
				cur=cur->next;
			}else
			{
				tmp=cur;
				bgbgc_workqueue_delay=cur->next;
				cur=cur->next;
			}
			
			tmp->next=NULL;
			BGBGC_AddJobEnd(tmp);
			continue;
		}
	
		prv=cur;
		cur=cur->next;
	}
}

void BGBGC_DispatchBlockedWork(int dt)
{
	BGBGC_Job *cur, *prv, *tmp;
	int i;
	
	cur=bgbgc_workqueue_blocked; prv=NULL;
	while(cur)
	{
		if(!(cur->blocked(cur->data)))
		{
			if(prv)
			{
				tmp=cur;
				prv->next=cur->next;
				cur=cur->next;
			}else
			{
				tmp=cur;
				bgbgc_workqueue_blocked=cur->next;
				cur=cur->next;
			}
			
			tmp->next=NULL;
			BGBGC_AddJobEnd(tmp);
			continue;
		}
	
		prv=cur;
		cur=cur->next;
	}
}

void BGBGC_DispatchWork()
{
	static int lastcheck;
	BGBGC_Job *job;
	int i, t, t1, ts;

	t=BGBGC_TimeMS();
	i=t-lastcheck;
	if(i<0)
	{
		lastcheck=t;
		return;
	}
	if(i<10)
		return;

	t1=t;
	thLockFastMutex(bgbgc_workqueue_mutex);	
	BGBGC_DispatchDelayWork(i);
	BGBGC_DispatchBlockedWork(i);
	if(bgbgc_workqueue_first)
		{ job=bgbgc_workqueue_first; t1=job->time; }
	thUnlockFastMutex(bgbgc_workqueue_mutex);

//	ts=t-bgbgc_workqueue_lastspawn;

//	if(job)
	if(job)
	{
		if(bgbgc_workqueue_workers>=16)
			return;
		if(bgbgc_workqueue_pendingworkers>=4)
			return;
	
		ts=t-bgbgc_workqueue_lastspawn;
		i=t-t1;
		if(!bgbgc_workqueue_workers || ((i>100) && (ts>500)))
		{
			bgbgc_workqueue_pendingworkers++;
//			thThread(BGBGC_WorkerLoop, NULL);
			BGBGC_SpawnWorker(job->workerId);
			bgbgc_workqueue_lastspawn=t;			
		}
	}

	lastcheck=t;
	return;
}

void BGBGC_SpawnWorker(int workId)
{
	BGBGC_WorkerInfo *inf;

	inf=NULL;
	if(workId>0)
	{
		inf=btge_spawnworker[workId-1](NULL);

		if(inf==UNDEFINED)
			return;

		if(!inf)inf=gcalloc(sizeof(BGBGC_WorkerInfo));
		inf->workerId=workId;
	}
	
	thThread(BGBGC_WorkerLoop, inf);
//	thThread(BGBGC_WorkerLoop, NULL);

//	bgbgc_workqueue_lastspawn=t;
}

void BGBGC_DispatchWork2(int workId)
{
	int i, t, t1, ts;

	t=BGBGC_TimeMS();
	t1=t;
	thLockFastMutex(bgbgc_workqueue_mutex);
	if(bgbgc_workqueue_first)
		t1=bgbgc_workqueue_first->time;
	thUnlockFastMutex(bgbgc_workqueue_mutex);

	if(bgbgc_workqueue_workers>=16)
		return;
	if(bgbgc_workqueue_pendingworkers>=4)
		return;

	ts=t-bgbgc_workqueue_lastspawn;
	i=t-t1;
	if(!bgbgc_workqueue_workers || ((i>100) && (ts>500)))
	{
		bgbgc_workqueue_pendingworkers++;
		BGBGC_SpawnWorker(workId);
		bgbgc_workqueue_lastspawn=t;
	}
}

void BGBGC_WorkInit()
{
	static int init=0;
	
	if(init)return;
	init=1;
	
	bgbgc_workqueue_mutex=thFastMutex();
}

BGBGC_API BGBGC_Job *thAddJob(void *(*fcn)(void *data), void *data)
{
	BGBGC_Job *job;
	
	BGBGC_WorkInit();
	
	job=gctalloc("_bgbgc_job_t", sizeof(BGBGC_Job));
	job->func=fcn;
	job->data=data;
	job->time=BGBGC_TimeMS();

	thLockFastMutex(bgbgc_workqueue_mutex);
	BGBGC_AddJobEnd(job);
	thUnlockFastMutex(bgbgc_workqueue_mutex);
	BGBGC_DispatchWork2(0);
	return(job);
}

BGBGC_API BGBGC_Job *thAddJobDelay(void *(*fcn)(void *data), void *data, int ms)
{
	BGBGC_Job *job;

	BGBGC_WorkInit();

	job=gctalloc("_bgbgc_job_t", sizeof(BGBGC_Job));
	job->func=fcn;
	job->data=data;
	job->time=ms;

	thLockFastMutex(bgbgc_workqueue_mutex);
	job->next=bgbgc_workqueue_delay;
	bgbgc_workqueue_delay=job;
	thUnlockFastMutex(bgbgc_workqueue_mutex);
	return(job);
}

BGBGC_API BGBGC_Job *thAddJobBlocked(
	void *(*fcn)(void *data),
	int (*blocked)(void *data),
	void *data)
{
	BGBGC_Job *job;

	BGBGC_WorkInit();

	job=gctalloc("_bgbgc_job_t", sizeof(BGBGC_Job));
	job->func=fcn;
	job->blocked=blocked;
	job->data=data;

	thLockFastMutex(bgbgc_workqueue_mutex);
	job->next=bgbgc_workqueue_blocked;
	bgbgc_workqueue_blocked=job;
	thUnlockFastMutex(bgbgc_workqueue_mutex);
	return(job);
}

BGBGC_API BGBGC_Job *thAddJobId(
	void *(*fcn)(void *data), void *data, int workId)
{
	BGBGC_Job *job;

	BGBGC_WorkInit();

	job=gctalloc("_bgbgc_job_t", sizeof(BGBGC_Job));
	job->func=fcn;
	job->data=data;
	job->time=BGBGC_TimeMS();
	job->workerId=workId;

	thLockFastMutex(bgbgc_workqueue_mutex);
	BGBGC_AddJobEnd(job);
	thUnlockFastMutex(bgbgc_workqueue_mutex);
	BGBGC_DispatchWork2(workId);
	return(job);
}

BGBGC_API void thFreeJob(BGBGC_Job *job)
	{ gcfree(job); }
BGBGC_API void *thJobGetData(BGBGC_Job *job)
	{ return(job->data); }
BGBGC_API void *thJobGetValue(BGBGC_Job *job)
	{ return(job->val); }
BGBGC_API int thJobGetDone(BGBGC_Job *job)
	{ return(job->done); }

BGBGC_API int thDoWorkMS(int ms)
	{ return(BGBGC_DoWorkMS(ms)); }
