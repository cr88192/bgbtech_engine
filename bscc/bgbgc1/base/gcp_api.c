#include <bgbgc.h>

#ifdef BGBGC_USEPRECISE

void **bgbgc_gcp_root[4096];
int bgbgc_gcp_nroot;

void gcpMark(gcp obj)
	{ BGBGC_MarkPrecise((void *)obj); }

void gcpInc(gcp obj)
	{ BGBGC_IncRef((void *)obj); }

void gcpDec(gcp obj)
	{ BGBGC_DecRef((void *)obj); }

void gcpSafeDec(gcp obj)
	{ BGBGC_SafeDecRef((void *)obj); }

void gcpSet(gcp *ref, gcp obj)
	{ BGBGC_CollectAssignPrecise((void **)ref, (void *)obj); }

void gcpClear(gcp *ref)
	{ BGBGC_CollectAssignPrecise((void **)ref, NULL); }


void gcpPushFrame()
	{ BGBGC_PushRootFrame(); }

void gcpPopFrame()
	{ BGBGC_PopRootFrame(); }

void gcpRoot(gcp *ref)
	{ BGBGC_PushFrameRoot((void **)ref); }

void gcpGlobalRoot(gcp *ref)
	{ bgbgc_gcp_root[bgbgc_gcp_nroot++]=(void **)ref; }


void *gcpDefile(gcp obj)
	{ return(BGBGC_Defile((void *)obj)); }

gcp gcpUndefile(void *obj)
	{ return((gcp)BGBGC_UnDefile(obj)); }

void *gcpGrabPtr(gcp obj)
	{ return(BGBGC_PreciseGetPtr((void *)obj)); }

void gcpDropPtr(gcp obj)
	{ /* NO-OP at this point */ }

gcp gcpAlloc(char *type, int sz)
	{ return((gcp)BGBGC_NewPrecise(type, sz)); }

#endif
