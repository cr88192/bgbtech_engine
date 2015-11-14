#include <pdgl.h>

#ifdef _WIN32

#include <windows.h>


HMODULE pdgl_hmodapp=NULL;

PDGL_API void *PDGL_LookupName(char *name)
{
	void *p;
	if(!pdgl_hmodapp)
		pdgl_hmodapp=GetModuleHandle(NULL);
	if(!pdgl_hmodapp)
		return(NULL);
	p=GetProcAddress(pdgl_hmodapp, name);
	return(p);
}

#endif

