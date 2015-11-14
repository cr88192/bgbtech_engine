#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>

#if 0
#ifndef BYTE_T
#define BYTE_T
typedef unsigned char byte;
#endif

#ifndef S64_T
#define S64_T
typedef unsigned char s64;
#endif

#ifndef CRUN_TIME_T
#define CRUN_TIME_T
typedef long long crun_time_t;
#endif

#include "crun_auto.h"

#endif

#include "crun_core.c"
#include "crun_cortxt.c"


// extern char *crun_coretext;

int crun_init()
{
#ifdef _WIN32
	return(0);
#endif
	return(-1);
}


int crun_prologue(char **rt)
{
	char *t;

	t=*rt;

//	crun_printsf(&t, "#include <stdio.h>\n");
//	crun_printsf(&t, "#include <stdlib.h>\n");
//	crun_printsf(&t, "#include <string.h>\n");
//	crun_printsf(&t, "#include <stdarg.h>\n");

	crun_sputs(&t, crun_coretext);

	crun_printsf(&t, "\n\n/* End Prologue */\n\n\n");

	*rt=t;
	return(0);
}

int main(int argc, char *argv[])
{
	char *targs[64];
	static char tbuf[65536];
	static char obuf[65536];
	FILE *fd;
	char *s, *t, *fn;
	int i, sz, nta;

	i=crun_init();
	if(i<0)return(EXIT_FAILURE);

	nta=1; fn=NULL;
	for(i=1; i<argc; i++)
	{
		s=argv[i];
		if((s[0]=='/')&&(s[1]=='f'))
			{ fn=argv[i]+2; continue; }
		if((s[0]=='-')&&(s[1]=='f'))
			{ fn=argv[i]+2; continue; }
		targs[nta++]=s;
	}

	if(!fn)fn="crun_info.c";

	targs[0]=fn;
	targs[nta]=NULL;


	fd=fopen(fn, "rb");
	if(!fd)return(EXIT_FAILURE);

	t=obuf;

	crun_prologue(&t);

	sz=fread(tbuf, 1, 65536, fd);
	fclose(fd);
	tbuf[sz]=0;

	crun_sputs(&t, tbuf);
//	strcpy(t, tbuf);
//	t+=strlen(t);

	fd=fopen("_crun_tmp_.c", "wb");
	if(!fd)return(EXIT_FAILURE);
	fwrite(obuf, 1, strlen(obuf), fd);
	fclose(fd);

	system("cl /Fe_crun_tmp_.exe _crun_tmp_.c /nologo");

	i=crun_call(".\\_crun_tmp_.exe", targs+1);

//	t=tbuf;
//	crun_sputs(&t, ".\\_crun_tmp_.exe");
//	for(i=1; i<nta; i++)
//		crun_printsf(&t, " \"%s\"", targs[i]);
//	*t++=0;
//	i=system(tbuf);

//	i=system(".\\crun_tmp_.exe");

	// remove("_crun_tmp_.c");
	// remove("_crun_tmp_.exe");

//	printf("Ret=%d\n", i);
	return(i);

//	return(EXIT_SUCCESS);
}
