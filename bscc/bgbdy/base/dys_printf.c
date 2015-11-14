#include <bgbdy.h>

char *BGBDY_Printf_BasicInt(char *ct, char *cte, s64 val)
{
	char tb[64];
	char *t;
	s64 v;
	int sg;
	
	if(val<0) { v=-val; sg=1; }
		else { v=val; sg=0; }
	t=tb+64; *(--t)=0;
	if(!v) { *(--t)='0'; }
	while(v) { *(--t)=(v%10)+'0'; v/=10; }
	if(sg) { *(--t)='-'; }
	
	while(*t && (ct<cte)) { *ct++=*t++; }
	return(ct);
}

char *BGBDY_Printf_FormatInt(char *ct, char *cte, s64 val,
	int tfl, int tw, int tpr)
{
	char tb[64];
	char *t;
	s64 v;
	int sg, cnt;
	
	if(val<0) { v=-val; sg=1; }
		else { v=val; sg=0; }
	t=tb+64; *(--t)=0; cnt=0;
	while(v || !cnt)
	{
		*(--t)=(v%10)+'0'; v/=10;
		cnt++;
		if(tpr && (cnt>=tpr))break;
	}
	
	if(tw && !(tfl&1) && (cnt<tw))
	{
		if(tfl&8)
		{
			while(cnt<tw) { *(--t)='0'; cnt++; }

			if(sg) { *(--t)='-'; }
			else if(tfl&2) { *(--t)='+'; }
		}else
		{
			if(sg) { *(--t)='-'; }
			else if(tfl&2) { *(--t)='+'; }

			while(cnt<tw) { *(--t)=' '; cnt++; }
		}
	}else
	{
		if(sg) { *(--t)='-'; }
		else if(tfl&2) { *(--t)='+'; }
	}
	
	while(*t && (ct<cte)) { *ct++=*t++; }

	if(tw && (tfl&1) && (cnt<tw))
	{
		while((cnt<tw) && (ct<cte)) { *ct++=' '; cnt++; }
	}

	return(ct);
}

char *BGBDY_Printf_FormatHex(char *ct, char *cte,
	s64 val, int tfl, int tw, int tpr)
{
	return(BGBDY_Printf_FormatBase(ct, cte, val, 16, tfl, tw, tpr));
}

char *BGBDY_Printf_FormatBase(char *ct, char *cte,
	s64 val, int base,
	int tfl, int tw, int tpr)
{
	char tb[64];
	char *t;
	s64 v;
	int i, sg, cnt;
	
	if(val<0) { v=-val; sg=1; }
		else { v=val; sg=0; }
	t=tb+64; *(--t)=0; cnt=0;
	while(v || !cnt)
	{
		i=v%base; v/=base;
		*(--t)=(i<10)?(i+'0'):((i-10)+((tfl&16)?'A':'a'));
		cnt++;
		if(tpr && (cnt>=tpr))break;
	}
	
	if(tw && !(tfl&1) && (cnt<tw))
	{
		if(tfl&8)
		{
			while(cnt<tw) { *(--t)='0'; cnt++; }

			if(sg) { *(--t)='-'; }
			else if(tfl&2) { *(--t)='+'; }
		}else
		{
			if(sg) { *(--t)='-'; }
			else if(tfl&2) { *(--t)='+'; }

			while(cnt<tw) { *(--t)=' '; cnt++; }
		}
	}else
	{
		if(sg) { *(--t)='-'; }
		else if(tfl&2) { *(--t)='+'; }
	}
	
	while(*t && (ct<cte)) { *ct++=*t++; }

	if(tw && (tfl&1) && (cnt<tw))
	{
		while((cnt<tw) && (ct<cte)) { *ct++=' '; cnt++; }
	}

	return(ct);
}

char *BGBDY_ParseFormat(char *cs,
	int *rtfl, int *rtw, int *rtpr, int *rtsz, int *rtty)
{
	int tfl, tw, tpr, tsz, tty;

	tfl=0; tw=0; tpr=0; tsz=0; tty=0;
			
	if(*cs=='-') { tfl|=1; cs++; }
	if(*cs=='+') { tfl|=2; cs++; }
	if(*cs==' ') { tfl|=4; cs++; }
	if(*cs=='0') { tfl|=8; cs++; }

	//width
	while((*cs>='0') && (*cs<='9'))
		{ tw=(tw*10)+((*cs++)-'0'); }

	//precission
	if(*cs=='.')
	{
		cs++;
		while((*cs>='0') && (*cs<='9'))
			{ tpr=(tpr*10)+((*cs++)-'0'); }
	}
			
	//type
	if(*cs=='h') { tsz=1; cs++; }
	if(*cs=='l') { tsz=2; cs++; }
	if(*cs=='l') { tsz=3; cs++; }
	if(*cs=='L') { tsz=4; cs++; }
			
	tty=*cs++;
	
	if((tty=='f') || (tty=='g') || (tty=='e') ||
		(tty=='G') || (tty=='E'))
	{
		if(tsz==0)tsz=5;	//double
		if(tsz==4)tsz=6;	//long double
	}

	if(tty=='p')
	{
		tsz=7;
	}
	
	*rtfl=tfl;
	*rtw=tw;
	*rtpr=tpr;
	*rtsz=tsz;
	*rtty=tty;
	
	return(cs);
}

char *BGBDY_VSnPrintf(char *obuf, int sz, char *fmt, va_list lst)
{
	char *cs, *ct, *cte, *s, *t;
	int tfl, tw, tpr, tsz, tty;
	s64 li;
	double lf;
	void *lp;
	int i;
	
	cs=fmt; ct=obuf;
	cte=obuf+(sz-1);
	
	while(*cs && (ct<cte))
	{
		if(*cs=='%')
		{
			cs++;

			//simple cases, handled early
			if(*cs=='%')
				{ *ct++=*cs++; continue; }
			if((*cs=='d') || (*cs=='i'))
			{
				cs++;
				i=va_arg(lst, int);
				ct=BGBDY_Printf_BasicInt(ct, cte, i);
				continue;
			}
			if(*cs=='s')
			{
				cs++;
				s=va_arg(lst, char *);
				while(*s && (ct<cte))
					{ *ct++=*s++; }
				continue;
			}
			if(*cs=='c')
			{
				cs++;
				i=va_arg(lst, char);
				*ct++=i;
				continue;
			}

			BGBDY_ParseFormat(cs, &tfl, &tw, &tpr, &tsz, &tty);

			switch(tsz)
			{
			case 0:		li=va_arg(lst, int); break;
			case 1:		li=va_arg(lst, short); break;
			case 2:		li=va_arg(lst, long); break;
			case 3:		li=va_arg(lst, long long); break;
			case 4:		li=va_arg(lst, long long); break;
			case 5:		lf=va_arg(lst, double); break;
			case 6:		lf=va_arg(lst, long double); break;
			case 7:		lp=va_arg(lst, void *); break;
			default:	li=va_arg(lst, int); break;
			}

			if((tty=='d') || (tty=='i') || (tty=='u'))
			{
				if(tty=='u')li&=(tsz==3)?0x7FFFFFFFFFFFFFFFLL:0xFFFFFFFFLL;
				ct=BGBDY_Printf_FormatInt(ct, cte, li, tfl, tw, tpr);
				continue;
			}

			if((tty=='x') || (tty=='X'))
			{
				if(tty=='X')tfl|=16;
				ct=BGBDY_Printf_FormatHex(ct, cte, li, tfl, tw, tpr);
				continue;
			}

			if(tty=='o')
			{
				ct=BGBDY_Printf_FormatBase(ct, cte, li, 8, tfl, tw, tpr);
				continue;
			}
			
			continue;
		}
		
		*ct++=*cs++;
	}
	
	*ct=0;
	return(ct);
}

char *BGBDY_VSnPrintfA(char *obuf, int sz, char *fmt,
	dyte *args, int nargs)
{
	char *cs, *ct, *cte, *s, *t;
	int tfl, tw, tpr, tsz, tty;
	s64 li;
	double lf;
	void *lp;
	int i, an;
	
	cs=fmt; ct=obuf;
	cte=obuf+(sz-1); an=0;
	
	while(*cs && (ct<cte))
	{
		if(*cs=='%')
		{
			cs++;

			//simple cases, handled early
			if(*cs=='%')
				{ *ct++=*cs++; continue; }
			if((*cs=='d') || (*cs=='i'))
			{
				cs++;
				i=va_arg(lst, int);
				ct=BGBDY_Printf_BasicInt(ct, cte, i);
				continue;
			}
			if(*cs=='s')
			{
				cs++;
				s=va_arg(lst, char *);
				while(*s && (ct<cte))
					{ *ct++=*s++; }
				continue;
			}
			if(*cs=='c')
			{
				cs++;
				i=va_arg(lst, char);
				*ct++=i;
				continue;
			}

			BGBDY_ParseFormat(cs, &tfl, &tw, &tpr, &tsz, &tty);

			switch(tsz)
			{
			case 0:		li=dyintv(args[an++]); break;
			case 1:		li=dyintv(args[an++]); break;
			case 2:		li=dyintv(args[an++]); break;
			case 3:		li=dyintv(args[an++]); break;
			case 4:		li=dyintv(args[an++]); break;
			case 5:		lf=va_arg(lst, double); break;
			case 6:		lf=va_arg(lst, long double); break;
			case 7:		lp=va_arg(lst, void *); break;
			default:	li=va_arg(lst, int); break;
			}

			if((tty=='d') || (tty=='i') || (tty=='u'))
			{
				if(tty=='u')li&=(tsz==3)?0x7FFFFFFFFFFFFFFFLL:0xFFFFFFFFLL;
				ct=BGBDY_Printf_FormatInt(ct, cte, li, tfl, tw, tpr);
				continue;
			}

			if((tty=='x') || (tty=='X'))
			{
				if(tty=='X')tfl|=16;
				ct=BGBDY_Printf_FormatHex(ct, cte, li, tfl, tw, tpr);
				continue;
			}

			if(tty=='o')
			{
				ct=BGBDY_Printf_FormatBase(ct, cte, li, 8, tfl, tw, tpr);
				continue;
			}
			
			continue;
		}
		
		*ct++=*cs++;
	}
	
	*ct=0;
	return(ct);
}
