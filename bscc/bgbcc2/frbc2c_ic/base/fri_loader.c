#include <frbc2ci.h>

FRBC2CI_API FR2CI_LoaderCtx *FR2CI_AllocLoader(void)
{
	FR2CI_LoaderCtx *img;
	
	img=malloc(sizeof(FR2CI_LoaderCtx));
	memset(img, 0, sizeof(FR2CI_LoaderCtx));
	return(img);
}

FRBC2CI_API FR2CI_Image *FR2CI_LookupImageName(
	FR2CI_LoaderCtx *ldr, char *name)
{
	FR2CI_Image *cur;
	
	cur=ldr->image;
	while(cur)
	{
		if(!strcmp(cur->name, name))
			return(cur);
		cur=cur->next;
	}
	return(NULL);
}

FRBC2CI_API FR2CI_Image *FR2CI_LoadImageName(
	FR2CI_LoaderCtx *ldr, char *name)
{
	FR2CI_Image *cur;
	byte *buf;
	int sz, fl;

	cur=FR2CI_LookupImageName(ldr, name);
	if(cur)
		{ return(cur); }

	if(ldr->LoadImageFile)
	{
		fl=0;
		buf=ldr->LoadImageFile(name, &sz, &fl);
		if(buf)
		{
			cur=FR2CI_DecodeImage(buf, sz, fl);
			if(ldr->ReleaseImageFileBuffer)
				ldr->ReleaseImageFileBuffer(buf, fl);
			if(cur)
			{
				cur->name=fr2ci_strdup(name);
				cur->next=ldr->image;
				ldr->image=cur;
				return(cur);
			}
		}
	}
	return(NULL);
}

FRBC2CI_API FR2CI_Image *FR2CI_LoadProgramName(
	FR2CI_LoaderCtx *ldr, char *name)
{
	FR2CI_Image *cur;
	
	cur=FR2CI_LoadImageName(ldr, name);
	ldr->img_main=cur;
	return(cur);
}


FRBC2CI_API FR2CI_Function *FR2CI_LookupFunctionNameGeneric(
	FR2CI_LoaderCtx *ldr, char *name, char *sig)
{
	FR2CI_Image *cur;
	FR2CI_Function *func;
	
	cur=ldr->image;
	while(cur)
	{
//		if(!strcmp(cur->name, name))
//			return(cur);
		func=FR2CI_ImageGetExportFunction(cur, name, sig);
		if(func)return(func);
		cur=cur->next;
	}
	return(NULL);
}
