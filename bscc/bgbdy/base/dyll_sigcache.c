#include <bgbasm.h>
#include <bgbgc.h>
#include <bgbdy.h>

char **bgbgc_sig_name;
char **bgbgc_sig_value;
int bgbgc_sig_hash[4096];
int bgbgc_n_sig, bgbgc_m_sig;

void BGBGC_InitSig()
{
	static int init=0;
	char tb[256];
	VFILE *fd;
	char **a;

	if(init)return;
	init=1;

	DYLL_MetaPath_LoadAll();

#if 0
	bgbgc_sig_name=malloc(4096*sizeof(char *));
	bgbgc_sig_value=malloc(4096*sizeof(char *));
	bgbgc_n_sig=0; bgbgc_m_sig=4096;

	fd=vffopen("cc_usr/sigcache.txt", "rb");
	if(!fd)return;

	while(!vfeof(fd))
	{
		vfgets(tb, 255, fd);
		a=gcrsplit(tb);
		if(!a[0])continue;
		if(a[0][0]==';')continue;
		if(a[0][0]=='#')continue;
		if(a[0][0]=='/')continue;

		BGBGC_BindSig(a[0], a[1]);
	}
	vfclose(fd);
#endif

}

void BGBGC_SaveSig()
{
	VFILE *fd;
	int i;

	DYLL_MetaPath_SaveAll();

#if 0
//	printf("BGBGC_SaveSig: A %d\n", bgbgc_n_sig);

	if(!bgbgc_n_sig)return;

#if 1
	fd=vffopen("cc_usr/sigcache.txt", "wt");
	if(!fd)return;

//	printf("BGBGC_SaveSig: B\n");

	for(i=0; i<bgbgc_n_sig; i++)
	{
		if(!bgbgc_sig_value[i])
			continue;

		vfprint(fd, "\"%s\" \"%s\"\n",
			bgbgc_sig_name[i], bgbgc_sig_value[i]);
	}

	vfclose(fd);
#endif
#endif
}

int BGBGC_HashSigName(char *str)
{
	char *s;
	int i;

	s=str; i=0;
	while(*s)i=(i*251)+(*s++);
	i=(i>>8)&4095;
	return(i);
}

char *BGBGC_LookupSig(char *name)
{
	int i, hi;

	BGBGC_InitSig();

	return(DYLL_MetaPath_LookupKey(name));

#if 0
	hi=BGBGC_HashSigName(name);
	i=bgbgc_sig_hash[hi];
	if((i>=0) && (i<bgbgc_n_sig))
		if(!strcmp(bgbgc_sig_name[i], name))
			return(bgbgc_sig_value[i]);

	for(i=0; i<bgbgc_n_sig; i++)
		if(!strcmp(bgbgc_sig_name[i], name))
			break;
	if(i<bgbgc_n_sig)
	{
		bgbgc_sig_hash[hi]=i;
		return(bgbgc_sig_value[i]);
	}
#endif

	return(NULL);
}

int BGBGC_BindSig(char *name, char *value)
{
	int i, hi;

	BGBGC_InitSig();

	if(!name)return(-1);

	DYLL_MetaPath_BindKey(name, value);
	return(0);

#if 0
	if(!value)
	{
		for(i=0; i<bgbgc_n_sig; i++)
			if(!strcmp(bgbgc_sig_name[i], name))
				break;
		if(i<bgbgc_n_sig)
			bgbgc_sig_value[i]=NULL;
		return;
	}

	hi=BGBGC_HashSigName(name);

#if 1
	i=bgbgc_sig_hash[hi];
	if((i>=0) && (i<bgbgc_n_sig))
		if(!strcmp(bgbgc_sig_name[i], name))
	{
		if(!bgbgc_sig_value[i])
		{
			bgbgc_sig_value[i]=basm_strdup(value);
			return(i);
		}

		if(strcmp(bgbgc_sig_value[i], value))
		{
			printf("BGBGC_BindSig: Redefined '%s' as "
				"different type: %s->%s\n",
				name, bgbgc_sig_value[i], value);
			bgbgc_sig_value[i]=basm_strdup(value);
//			*(int *)-1=-1;
			return(i);
		}
		return(i);
	}
#endif

	for(i=0; i<bgbgc_n_sig; i++)
		if(!strcmp(bgbgc_sig_name[i], name))
			break;
	if(i<bgbgc_n_sig)
	{
		if(!bgbgc_sig_value[i])
		{
			bgbgc_sig_value[i]=basm_strdup(value);
			bgbgc_sig_hash[hi]=i;
			return(i);
		}

		if(!strcmp(bgbgc_sig_value[i], value))
		{
			bgbgc_sig_hash[hi]=i;
			return(i);
		}

		printf("BGBGC_BindSig: Redefined %s as "
			"different type: %s->%s\n",
			name, bgbgc_sig_value[i], value);
		bgbgc_sig_value[i]=basm_strdup(value);
		bgbgc_sig_hash[hi]=i;
		return(i);
	}

	if(bgbgc_n_sig>=bgbgc_m_sig)
	{
		i=bgbgc_m_sig+(bgbgc_m_sig>>1);
		bgbgc_sig_name=realloc(bgbgc_sig_name, i*sizeof(char *));
		bgbgc_sig_value=realloc(bgbgc_sig_value, i*sizeof(char *));
		bgbgc_m_sig=i;
	}

	i=bgbgc_n_sig++;
	bgbgc_sig_name[i]=basm_strdup(name);
	bgbgc_sig_value[i]=basm_strdup(value);
	bgbgc_sig_hash[hi]=i;
	return(i);
#endif
}
