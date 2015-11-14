#include <bgbgc.h>
#include <bgbdy.h>

BGBDY_API int dyxParseEatWhite(dysReadStream strm)
{
	int i, r;

	i=0;
	while((dysPeekChar(strm)>0) && (dysPeekChar(strm)<=' '))
	{
		i=1;
		dysReadChar(strm);
	}

	if((dysPeekCharIdx(strm, 0)=='<') &&
		(dysPeekCharIdx(strm, 1)=='!') &&
		(dysPeekCharIdx(strm, 2)=='-') &&
		(dysPeekCharIdx(strm, 3)=='-'))
	{
		i=1;

		dysReadChar(strm);
		dysReadChar(strm);
		dysReadChar(strm);
		dysReadChar(strm);

		while(dysPeekChar(strm)>0)
		{
			if((dysPeekCharIdx(strm, 0)=='-') &&
				(dysPeekCharIdx(strm, 1)=='-') &&
				(dysPeekCharIdx(strm, 2)=='>'))
			{
				dysReadChar(strm);
				dysReadChar(strm);
				dysReadChar(strm);
				break;
			}
			dysReadChar(strm);
		}
	}

	if(i)dyxParseEatWhite(strm);

	if(dysPeekChar(strm)<0)
		return(-1);
	return(0);
}

BGBDY_API int dyxParseSpecialP(dysReadStream strm)
{
	switch(dysPeekChar(strm))
	{
	case '<': case '>':
	case '/': case '=':
	case '?': case ':':
	case 0: case -1:
		return(1);
		break;

	default:
		return(0);
		break;
	}
	return(0);
}

BGBDY_API int dyxParseContSpecialP(dysReadStream strm)
{
	switch(dysPeekChar(strm))
	{
	case '<': case '&':
	case 0: case -1:
		return(1);
		break;

	default:
		return(0);
		break;
	}
	return(0);
}

BGBDY_API char *dyxParseToken(dysReadStream strm, int *ty)
{
	char buf[16];
	char *t, *t2, *b;
	int i, j;

	b=gcralloc(256); t=b;
	*b=0;

	if(ty)*ty=0;

	i=dysParseEatWhite(strm);
	if(i<0)return(NULL);

	if(dysParseSpecialP(strm))
	{
		if(ty)*ty=BGBDY_TTY_SPECIAL;
		*t++=dysReadChar(strm);
		*t=0;
	}else if(dysPeekChar(strm)=='"') /* quoted string */
	{
		if(ty)*ty=BGBDY_TTY_STRING;
		dysReadChar(strm);
		while((dysPeekChar(strm)>0) && (dysPeekChar(strm)!='\"'))
		{
			if(dysPeekChar(strm)=='&')
			{
				dysReadChar(strm);
				t2=buf;
				while((dysPeekChar(strm)>0) &&
					(dysPeekChar(strm)!=';'))
						*t2++=dysReadChar(strm);
				if(dysPeekChar(strm)<0)return(NULL);
				*t2++=0;
				dysReadChar(strm);

				if(buf[0]=='#')
				{
					if(buf[1]=='x')
					{
						t2=buf+2;
						i=0;
						while(*t2)
						{
							i<<=4;
							if((*t2>='0') && (*t2<='9'))
								i+=*t2-'0';
							if((*t2>='A') && (*t2<='F'))
								i+=*t2-'A'+10;
							if((*t2>='a') && (*t2<='f'))
								i+=*t2-'a'+10;
							t++;
						}
						*t++=i;
					}else *t++=atoi(buf+1);
				}
				if(!strcmp(buf, "amp"))*t++='&';
				if(!strcmp(buf, "lt"))*t++='<';
				if(!strcmp(buf, "gt"))*t++='>';
				if(!strcmp(buf, "quot"))*t++='"';
				if(!strcmp(buf, "apos"))*t++='\'';
			}else BGBGC_EmitChar(&t, dysReadChar(strm));
		}
		*t++=0;
		dysReadChar(strm);
	}else
	{	/* alphanumeric string that can contain '_' */
		if(ty)*ty=BGBDY_TTY_SYMBOL;

		while(dysPeekChar(strm)>' ')
		{
			if(dyxParseSpecialP(strm))break;
			BGBGC_EmitChar(&t, dysReadChar(strm));
		}
		*t++=0;
	}

	return(b);
}

BGBDY_API char *dyxPeekToken(dysReadStream strm, int *ty)
{
	dysReadPos pos;
	char *b;

	pos=dysGetPos(strm);
	b=dyxParseToken(strm, ty);
	dysSetPos(strm, pos);
	return(b);
}

BGBDY_API char *dyxParseText(dysReadStream strm)
{
	char *b, *t, *t1;
	char buf[16];
	int i, gws, rws;

	b=gcralloc(4096);
	t=b; *t=0;

	i=dyxParseEatWhite(strm);
	if(i<0)return(NULL);

	gws=0; rws=0;
	while(1)
	{
		while(!dyxParseContSpecialP(strm))
		{
			if((dysPeekChar(strm)=='\r') ||
				(dysPeekChar(strm)=='\n'))
			{
				dyxParseEatWhite(strm);
				if(!rws)
				{
					*t++=' ';
					gws++;
				}
				continue;
			}
			gws=0;
			if(dysPeekChar(strm)<=' ')rws++;
				else rws=0;
			*t++=dysReadChar(strm);
		}

		if(dysPeekChar(strm)<=0)return(NULL);

		if(dysPeekChar(strm)=='&')
		{
			dysReadChar(strm);
			t1=buf;
			while(dysPeekChar(strm)!=';')
				*t1++=dysReadChar(strm);
			if(dysPeekChar(strm)<=0)return(NULL);
			*t1++=0;
			dysReadChar(strm);

			if(buf[0]=='#')
			{
				if(buf[1]=='x')
				{
					t1=buf+2;
					i=0;
					while(*t1)
					{
						i<<=4;
						if((*t1>='0') && (*t1<='9'))
							i+=*t1-'0';
						if((*t1>='A') && (*t1<='F'))
							i+=*t1-'A'+10;
						if((*t1>='a') && (*t1<='f'))
							i+=*t1-'a'+10;
						t1++;
					}
					gws=0;
					if(i<=' ')rws++;
						else rws=0;
					*t++=i;
				}else
				{
					i=atoi(buf+1);
					gws=0;
					if(i<=' ')rws++;
						else rws=0;
					*t++=i;
				}
				continue;
			}
			rws=0;
			gws=0;

			if(!strcmp(buf, "amp"))*t++='&';
			if(!strcmp(buf, "lt"))*t++='<';
			if(!strcmp(buf, "gt"))*t++='>';
			if(!strcmp(buf, "apos"))*t++='\'';
			if(!strcmp(buf, "quot"))*t++='"';
		}else break;
	}
	t-=gws;
	*t++=0;

	return(b);
}

BGBDY_API dyxAttr *dyxParseOpts(dysReadStream strm)
{
	char *ns, *var, *eq, *val;
	int ty;
	dyxAttr *lst, *end, *tmp;

	lst=NULL;
	end=NULL;

	while(1)
	{
		var=dyxPeekToken(strm, &ty);
		if(ty==BGBDY_TTY_NULL)return(NULL);

		if((var[0]=='>') && (ty==BGBDY_TTY_SPECIAL)) break;
		if((var[0]=='/') && (ty==BGBDY_TTY_SPECIAL)) break;
		if((var[0]=='?') && (ty==BGBDY_TTY_SPECIAL)) break;

		if(ty==BGBDY_TTY_NULL) return(NULL);
		if(ty!=BGBDY_TTY_SYMBOL)
		{
			printf("parse error (inv attribute).\n");
			*(int *)-1=-1;
			return(NULL);
		}

		ns=NULL;

		var=dyxParseToken(strm, &ty);
		if(ty==BGBDY_TTY_NULL) return(NULL);

		eq=dyxParseToken(strm, &ty);
		if(ty==BGBDY_TTY_NULL) return(NULL);

		if((ty==BGBDY_TTY_SPECIAL) && (eq[0]==':'))
		{
			ns=var;

			var=dyxParseToken(strm, &ty);
			if(ty==BGBDY_TTY_NULL) return(NULL);

			eq=dyxParseToken(strm, &ty);
			if(ty==BGBDY_TTY_NULL) return(NULL);
		}

		if((ty!=BGBDY_TTY_SPECIAL) || (eq[0]!='='))
		{
			printf("parse error (attr equal).\n");
			*(int *)-1=-1;
			return(NULL);
		}

		val=dyxParseToken(strm, &ty);
		if(ty==BGBDY_TTY_NULL) return(NULL);

		if(ty!=BGBDY_TTY_STRING)
		{
			printf("parse error (inv attribute arg).\n");
			*(int *)-1=-1;
			return(NULL);
		}

		tmp=gctalloc("_dyxattr_t", sizeof(dyxAttr));
		tmp->next=NULL;
		if(ns)tmp->ns=dystrdup(ns);
		tmp->var=dystrdup(var);
		tmp->val=dystrdup(val);

		if(end)
		{
			end->next=tmp;
			end=tmp;
		}else
		{
			lst=tmp;
			end=tmp;
		}
	}

	return(lst);
}

BGBDY_API dyxNode *dyxParseTag(dysReadStream strm, int *rgrp)
{
	char *buf, *buf2, *key, *ns;
	int ty, i, j;
	char *s2;

	dyxNode *tmp, *t, *end;

	i=dyxParseEatWhite(strm);
	if(i<0)return(NULL);

	buf=dyxPeekToken(strm, &ty);
	if(ty==BGBDY_TTY_NULL) return(NULL);

	if(rgrp)*rgrp=0;

	if((buf[0]=='<') && (ty==BGBDY_TTY_SPECIAL))
	{
		buf=dyxParseToken(strm, &ty);
		if(ty==BGBDY_TTY_NULL) return(NULL);

		if(dysPeekChar(strm)=='?')
			dysReadChar(strm);
		if(dysPeekChar(strm)=='!')
		{
			i=1;
			while((dysPeekChar(strm)>0) && i)
			{
				j=dysReadChar(strm);
				if(j=='<')i++;
				if(j=='>')i--;
				if(j=='[')i++;
				if(j==']')i--;
			}
			return(dyxParseExpr(strm));
		}

		if(dysPeekChar(strm)=='/')
		{
			dysReadChar(strm);	//'/'

			key=dyxParseToken(strm, &ty);	//tag or NS
			if(ty==BGBDY_TTY_NULL) return(NULL);

			if(dysPeekChar(strm)==':')
			{
				ns=key;
				key=dyxParseToken(strm, &ty);	//tag
				if(ty==BGBDY_TTY_NULL) return(NULL);
			}

			buf=dyxParseToken(strm, &ty); //'>'
			if(ty==BGBDY_TTY_NULL) return(NULL);

			if(rgrp)*rgrp=2;
			return(NULL);
		}


		ns=NULL;

		key=dyxParseToken(strm, &ty);
		if(ty==BGBDY_TTY_NULL)return(NULL);

		if(ty!=BGBDY_TTY_SYMBOL)
		{
			printf("parse error (inv tag).\n");
			*(int *)-1=-1;
			return(NULL);
		}

		if(dysPeekChar(strm)==':')
		{
			dysReadChar(strm);
			ns=key;
			key=dyxParseToken(strm, &ty);

			if(ty==BGBDY_TTY_NULL)
				return(NULL);
		}

		if((dysPeekChar(strm)>' ') &&
			(dysPeekChar(strm)!='>') && (dysPeekChar(strm)!='/'))
		{
			printf("parse error (inv char after tag).\n");
			*(int *)-1=-1;
			return(NULL);
		}

		tmp=gctalloc("_dyxnode_t", sizeof(dyxNode));
		tmp->next=NULL;
		if(ns)tmp->ns=dystrdup(ns);
		tmp->tag=dystrdup(key);
		tmp->attr=dyxParseOpts(strm);

		buf=dyxParseToken(strm, &ty);
		if(ty==BGBDY_TTY_NULL)
			return(NULL);

		if((buf[0]=='/') && (ty==BGBDY_TTY_SPECIAL))
		{
			buf=dyxParseToken(strm, &ty);
			if(ty==BGBDY_TTY_NULL)
				return(NULL);
			return(tmp);
		}
		if((buf[0]=='?') && (ty==BGBDY_TTY_SPECIAL))
		{
			buf=dyxParseToken(strm, &ty);
			if(ty==BGBDY_TTY_NULL)
				return(NULL);

			buf=gcralloc(64);
			strcpy(buf, "?");
			strcat(buf, tmp->tag);
			tmp->tag=dystrdup(buf);
			return(tmp);
		}
		if(buf[0]!='>')
		{
			printf("parse error (expected close '>').\n");
			*(int *)-1=-1;
			return(NULL);
		}

		if(rgrp)*rgrp=1;

		return(tmp);
	}

	s2=dyxParseText(strm);
	if(dysPeekChar(strm)<=0)
		return(NULL);

	tmp=gctalloc("_dyxnode_t", sizeof(dyxNode));
	tmp->text=dystrdup(s2);

	return(tmp);
}

BGBDY_API dyxNode *dyxParseExprR(dysReadStream strm, int *rgrp)
{
	dyxNode *tmp, *tmp1, *end;
	int i;

	if(*rgrp)*rgrp=0;
	tmp=dyxParseTag(strm, &i);

	if(i==1)
	{
		while(1)
		{
			tmp1=dyxParseExprR(strm, &i);
			if(i==2)break;

			if(!tmp1)return(NULL);

			dyxAdd(tmp, tmp1);
		}

		if(*rgrp)*rgrp=0;
		return(tmp);
	}

	if(i==2)
	{
		if(*rgrp)*rgrp=2;
		return(NULL);
	}

	return(tmp);
}

BGBDY_API dyxNode *dyxParseExpr(dysReadStream strm)
	{ return(dyxParseExprR(strm, NULL)); }

BGBDY_API dyxNode *dyxParseExprStr(char *str)
{
	dysReadStream strm;
	dyxNode *lst, *tmp;

	strm=dysNewReadStreamBuf(str);

	lst=NULL;
	while(dysPeekChar(strm)>0)
	{
		tmp=dyxParseExpr(strm);
		if(!tmp)break;
		lst=dyxAddEnd(lst, tmp);
	}
	dysFreeReadStream(strm);
	return(lst);
}

BGBDY_API dyxNode *dyxParseExprSBuf(char **str)
{
	dysReadStream strm;
	dyxNode *lst, *tmp;

	strm=dysNewReadStreamBuf(*str);

	lst=NULL;
	while(dysPeekChar(strm)>0)
	{
		tmp=dyxParseExpr(strm);
		if(!tmp)break;
		lst=dyxAddEnd(lst, tmp);
	}
	dysFreeReadStream(strm);
	*str=(char *)dysGetPos(strm);
	return(lst);
}

BGBDY_API dyxNode *dyxParseTagSBuf(char **str)
{
	dysReadStream strm;
	dyxNode *lst, *tmp;

	strm=dysNewReadStreamBuf(*str);
	tmp=dyxParseTag(strm, NULL);
	*str=(char *)dysGetPos(strm);
	dysFreeReadStream(strm);
	return(tmp);

#if 0
	lst=NULL;
	while(dysPeekChar(strm)>0)
	{
		tmp=dyxParseExpr(strm);
		if(!tmp)break;
		lst=dyxAddEnd(lst, tmp);
	}
	return(lst);
#endif
}

BGBDY_API char *dyxParseEatWhiteStr(char *str)
{
	dysReadStream strm;
	dyxNode *lst, *tmp;

	strm=dysNewReadStreamBuf(str);
	dyxParseEatWhite(strm);
	str=(char *)dysGetPos(strm);
	dysFreeReadStream(strm);
	return(str);
}
