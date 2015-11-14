/*
Frame Based allocator...

- Frame Sizes (refers to args):
- 0, 1, 2, 3, 4, 6, 9, 13, 19, 28, 42, 63, 94, 141, 211, 316
- A constant is added to this size, currently 5.

First frame starts at 5, and each next frame has:
size=size+(size>>1);

 */

#include <bgbgc.h>
#include <bgbdy.h>

#if 0
int dyll_frame_sizes[24]=
	{  0,   1,    2,    3,    4,    6,    9,   13,
	  19,  28,   42,   63,   94,  141,  211,  316,
	 474, 711, 1066, 1599, 2398, 3597, 5395, 8092
	};
int dyll_frame_counts[24]=
	{512, 512, 512, 512, 256, 256, 256, 128,
	 128,  64,  64,  32,  32,  16,  16,  16,
	   8,   8,   4,   4,   2,   2,   1,   1};

int dyll_frame_sizes_pad[16]=
	{  6,   6,    8,    8,   10,   12,   14,   18,
	  24,  32,   48,   68,  100,  146,  216,  322,
	 480, 716, 1072, 1604, 2404, 3602, 5400, 8098};
#endif

#if 0
int dyll_frame_sizes[24]=
	{  5,    7,   10,   15,    22,    33,    49,    73,
	 109,  163,  244,  366,   549,   823,  1234,  1851,
	2776, 4164, 6246, 9369, 14053, 21079, 31618, 47427};
int dyll_frame_sizes_pad[24]=
	{  6,    8,   10,   16,    22,    34,    50,    74,
	 110,  164,  244,  366,   550,   824,  1234,  1852,
	2776, 4164, 6246, 9370, 14054, 21080, 31618, 47428};

int dyll_frame_counts[24]=
	{512, 512, 256, 256, 128,  64,  64,  32,
	  32,  16,   8,   8,   4,   4,   2,   2,
	   1,   1,   1,   1,   1,   1,   1,   1};
#endif

int dyll_frame_sizes[48];
int dyll_frame_sizes_pad[48];
int dyll_frame_counts[48];


void *dyll_frame_list[16];

void *dyll_frame_mtx=NULL;
int dyll_frame_lock=0;

#if defined(X86) || defined(X86_64)

void BGBDY_InitFrames()
{
	int i, j, k;

//	dyll_frame_lock=thFastMutex();

	dyllAsmBegin();

	j=5;
	for(i=0; i<48; i++)
	{
		if(sizeof(void *)==4) { k=(j+3)&(~3); }
			else if(sizeof(void *)==8) { k=(j+1)&(~1); }
			else k=j;

		dyll_frame_sizes[i]=j;
		dyll_frame_sizes_pad[i]=k;

		dyll_frame_counts[i]=(8192/(k*sizeof(void *)))+1;

		j=j+(j>>2);
	}

	for(i=0; i<48; i++)
	{
		BGBDY_Frames_EmitAlloc(i);
		BGBDY_Frames_EmitFree(i);
		BGBDY_Frames_EmitAllocLocals(i);
		BGBDY_Frames_EmitReturn(i);
		BGBDY_Frames_EmitCall(i);
	}

	BGBDY_Frames_EmitReturnZ();
	BGBDY_Frames_EmitCallN();

	dyllAsmEnd();
}

// #ifdef __x86_64__
#else /* __i386__ || __x86_64__ */

void BGBDY_InitFrames()
{
	dyll_frame_mtx=thFastMutex();
}

void *dyll_AllocFrameN(int n)
{
	void *p, **pp;
	int i, j;

	thLockFastMutex(dyll_frame_mtx);
	if(dyll_frame_list[n])
	{
		p=dyll_frame_list[n];
		dyll_frame_list[n]=*(void **)p;
		thUnlockFastMutex(dyll_frame_mtx);
		return(p);
	}

	pp=malloc(256*dyll_frame_sizes_pad[n]);
	j=0; p=NULL;
	for(i=0; i<256; i++)
		{ pp[j]=p; p=(void *)pp; j+=dyll_frame_sizes_pad[n]; }
	dyll_frame_list[n]=*(void **)p;

	thUnlockFastMutex(dyll_frame_mtx);
	return(p);
}

void dyll_FreeFrameN(int n, void *p)
{
	thLockFastMutex(dyll_frame_mtx);
	*(void **)p=dyll_frame_list[n];
	dyll_frame_list[n]=p;
	thUnlockFastMutex(dyll_frame_mtx);
}

void *dyll_AllocFrame0() { return(dyll_AllocFrameN(0)); }
void *dyll_AllocFrame1() { return(dyll_AllocFrameN(1)); }
void *dyll_AllocFrame2() { return(dyll_AllocFrameN(2)); }
void *dyll_AllocFrame3() { return(dyll_AllocFrameN(3)); }
void *dyll_AllocFrame4() { return(dyll_AllocFrameN(4)); }
void *dyll_AllocFrame5() { return(dyll_AllocFrameN(5)); }
void *dyll_AllocFrame6() { return(dyll_AllocFrameN(6)); }
void *dyll_AllocFrame7() { return(dyll_AllocFrameN(7)); }
void *dyll_AllocFrame8() { return(dyll_AllocFrameN(8)); }
void *dyll_AllocFrame9() { return(dyll_AllocFrameN(9)); }
void *dyll_AllocFrame10() { return(dyll_AllocFrameN(10)); }
void *dyll_AllocFrame11() { return(dyll_AllocFrameN(11)); }
void *dyll_AllocFrame12() { return(dyll_AllocFrameN(12)); }
void *dyll_AllocFrame13() { return(dyll_AllocFrameN(13)); }
void *dyll_AllocFrame14() { return(dyll_AllocFrameN(14)); }
void *dyll_AllocFrame15() { return(dyll_AllocFrameN(15)); }

void *dyll_FreeFrame0(void *p) { return(dyll_FreeFrameN(0, p)); }
void *dyll_FreeFrame1(void *p) { return(dyll_FreeFrameN(1, p)); }
void *dyll_FreeFrame2(void *p) { return(dyll_FreeFrameN(2, p)); }
void *dyll_FreeFrame3(void *p) { return(dyll_FreeFrameN(3, p)); }
void *dyll_FreeFrame4(void *p) { return(dyll_FreeFrameN(4, p)); }
void *dyll_FreeFrame5(void *p) { return(dyll_FreeFrameN(5, p)); }
void *dyll_FreeFrame6(void *p) { return(dyll_FreeFrameN(6, p)); }
void *dyll_FreeFrame7(void *p) { return(dyll_FreeFrameN(7, p)); }
void *dyll_FreeFrame8(void *p) { return(dyll_FreeFrameN(8, p)); }
void *dyll_FreeFrame9(void *p) { return(dyll_FreeFrameN(9, p)); }
void *dyll_FreeFrame10(void *p) { return(dyll_FreeFrameN(10, p)); }
void *dyll_FreeFrame11(void *p) { return(dyll_FreeFrameN(11, p)); }
void *dyll_FreeFrame12(void *p) { return(dyll_FreeFrameN(12, p)); }
void *dyll_FreeFrame13(void *p) { return(dyll_FreeFrameN(13, p)); }
void *dyll_FreeFrame14(void *p) { return(dyll_FreeFrameN(14, p)); }
void *dyll_FreeFrame15(void *p) { return(dyll_FreeFrameN(15, p)); }

#endif
