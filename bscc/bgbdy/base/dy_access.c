#include <bgbdy.h>


BGBDY_API int dyGetAccessModeUID(dyAccessMode acc)
	{ return((acc>>12)&1023); }
BGBDY_API int dyGetAccessModeGID(dyAccessMode acc)
	{ return((acc>>22)&1023); }
BGBDY_API int dyGetAccessModeBits(dyAccessMode acc)
	{ return(acc&4095); }

BGBDY_API int dyCheckObjectAccess(dyAccessMode obj, dyAccessMode usr, int req)
{
//	return(1);

	//UID 0 is root, so has full access to everything
	if(!usr ||
		!dyGetAccessModeUID(usr) ||
		!dyGetAccessModeGID(usr))
	{
		return(1);
	}

//	*(int *)-1=-1;

	if(!obj)
	{
		//access=0 indicates defaults (system-level objects)
		//by default, a user account has read/exec access
		if((req&BGBDY_ACCESS_READEXEC_ANY)==req)
			return(1);
		//otherwise, no access
		return(0);
	}

	if(dyGetAccessModeUID(obj)==dyGetAccessModeUID(usr))
	{
		if(((dyGetAccessModeBits(obj)>>8)&req)==req)
			return(1);
	}

	if(dyGetAccessModeGID(obj)==dyGetAccessModeGID(usr))
	{
		if(((dyGetAccessModeBits(obj)>>4)&req)==req)
			return(1);
	}

	if((dyGetAccessModeBits(obj)&req)==req)
		return(1);
		
	return(0);
}

BGBDY_API int dyCheckObjectCurrentAccess(dyAccessMode obj, int req)
	{ return(dyCheckObjectAccess(obj, dyGetAccessMode(), req)); }

BGBDY_API int dyCheckCurrentAccessRootOnly()
{
	dyAccessMode acc;
	acc=dyGetAccessMode();
	if(!acc || !dyGetAccessModeUID(acc))
		return(1);
	return(0);
}


BGBDY_API dyAccessMode dyGetAccessModeDefault()
{
	dyAccessMode acc;
	acc=dyGetAccessMode();
//	if(!acc || !dyGetAccessModeUID(acc))
//		return(1);
	acc&=~BGBDY_ACCESS_MODEMASK;
	acc|=BGBDY_ACCESS_DEFAULT;
	return(acc);
}
