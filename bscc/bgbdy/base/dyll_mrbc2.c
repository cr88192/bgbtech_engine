#include <bgbdy.h>

static int hexval(int i)
{
	if((i>='0') && (i<='9'))return(i-'0');
	if((i>='A') && (i<='F'))return(i-'A'+10);
	if((i>='a') && (i<='f'))return(i-'a'+10);
	return(-1);
}

static int hexbyte(char *s)
{
	int i, j, k;

	k=s[0];
	if((k>='0') && (k<='9')) { i=k-'0'; }
		else if((k>='A') && (k<='F')) { i=k-'A'+10; }
		else if((k>='a') && (k<='f')) { i=k-'a'+10; }
		else return(-1);
	k=s[1];
	if((k>='0') && (k<='9')) { j=k-'0'; }
		else if((k>='A') && (k<='F')) { j=k-'A'+10; }
		else if((k>='a') && (k<='f')) { j=k-'a'+10; }
		else return(-1);
	return((i<<4)+j);
}

int DYLL_MRBC2_ParseHexData(char **rcs, byte **rct, int *rsz)
{
	char *cs;
	byte *ct, *ict, *cte;
	int i, j, k;

	cs=*rcs;

	ict=*rct; cte=ict+(*rsz);
	if(!ict) { ict=malloc(256); cte=ict+256; }
	ct=ict;

	while(*cs)
	{
		if(*cs<=' ')
		{
			while(*cs && *cs<=' ')cs++;
			continue;
		}
		i=hexbyte(cs);
		if(i<0)break;

		if((ct+1)>cte)
		{
			j=(cte-ict); j=j+(j>>1);
			k=ct-ict;
			ict=realloc(ict, j);
			cte=ict+j; ct=ict+k;
		}

		cs+=2; *ct++=i;
	}

	*rcs=cs; *rct=ict; *rsz=cte-ict;
	return(ct-ict);
}


static int base64val(int i)
{
	if((i>='A') && (i<='Z'))return(i-'A'+0);
	if((i>='a') && (i<='z'))return(i-'a'+26);
	if((i>='0') && (i<='9'))return(i-'0'+52);
	if(i=='+')return(62);
	if(i=='/')return(63);
	return(-1);
}

static int base64int(char *s)
{
	int i, j, k;

	i=base64val(s[0]); if(i<0)return(-1);
	j=base64val(s[1]); if(j<0)return(-1);
	k=base64val(s[2]); if(k<0)return(-1);
	k=(i<<12)|(j<<6)|k;
	return(k);
}

static int base85val(int i)
{
	if((i>='!') && (i<='u'))return(i-'!');
	return(-1);
}

static s64 base85int(char **rcs)
{
	char *s;
	int i, j;
	s64 l;

	s=*rcs; l=0;

	while(*s && (*s<=' '))s++;
	if(*s=='z')
	{
		s++;
		*rcs=s;
		return(l);
	}

	for(i=0; i<5; i++)
	{
		while(*s && (*s<=' '))s++;
		j=base85val(*s);
		if(j<0)break;
		s++; l=l*85+j;
	}

	if(i<2)return(-1);
	for(; i<5; i++)l=l*85+84;
	*rcs=s;
	return(l);
}

int DYLL_MRBC2_ParseBase64Data(char **rcs, byte **rct, int *rsz)
{
	char *cs;
	byte *ct, *ict, *cte;
	int i, j, k;

	cs=*rcs;

	ict=*rct; cte=ict+(*rsz);
	if(!ict) { ict=malloc(256); cte=ict+256; }
	ct=ict;

	while(*cs)
	{
		if(*cs<=' ')
		{
			while(*cs && *cs<=' ')cs++;
			continue;
		}
		i=base64int(cs);
		if(i<0)break;

		if((ct+3)>cte)
		{
			j=(cte-ict); j=j+(j>>1);
			k=ct-ict;
			ict=realloc(ict, j);
			cte=ict+j; ct=ict+k;
		}

		cs+=4;
		*ct++=(i>>16)&0xFF;
		*ct++=(i>>8)&0xFF;
		*ct++=(i>>0)&0xFF;
	}

	*rcs=cs; *rct=ict; *rsz=cte-ict;
	return(ct-ict);
}

int DYLL_MRBC2_ParseBase85Data(char **rcs, byte **rct, int *rsz)
{
	char *cs;
	byte *ct, *ict, *cte;
	int i, j, k;
	s64 l;

	cs=*rcs;

	ict=*rct; cte=ict+(*rsz);
	if(!ict) { ict=malloc(256); cte=ict+256; }
	ct=ict;

	while(*cs)
	{
		l=base85int(&cs);
		if(l<0)break;

		if((ct+4)>cte)
		{
			j=(cte-ict); j=j+(j>>1);
			k=ct-ict;
			ict=realloc(ict, j);
			cte=ict+j; ct=ict+k;
		}

		*ct++=(l>>24)&0xFF;
		*ct++=(l>>16)&0xFF;
		*ct++=(l>>8)&0xFF;
		*ct++=(l>>0)&0xFF;
	}

	*rcs=cs; *rct=ict; *rsz=cte-ict;
	return(ct-ict);
}


DYLL_MRBC2_Context *DYLL_MRBC2_AllocContext()
{
	DYLL_MRBC2_Context *tmp;

	tmp=gctalloc("_dyll_mrbc2_context_t", sizeof(DYLL_MRBC2_Context));
	tmp->global=gcalloc(4096*sizeof(dytf));
	tmp->stack=gcalloc(4096*sizeof(dytf));
	tmp->markstack=gcalloc(256*sizeof(int));
	tmp->ipstack=gcalloc(256*sizeof(char *));

	return(tmp);
}

int DYLL_MRBC2_FreeContext(DYLL_MRBC2_Context *tmp)
{
	gcfree(tmp->global);
	gcfree(tmp->stack);
	gcfree(tmp->markstack);
	gcfree(tmp->ipstack);
	gcfree(tmp);
	return(0);
}

void DYLL_MRBC2_Push(DYLL_MRBC2_Context *ctx, dytf val)
{
	ctx->stack[ctx->stackpos++]=val;
}

dytf DYLL_MRBC2_Pop(DYLL_MRBC2_Context *ctx)
{
	ctx->stackpos--;
	return(ctx->stack[ctx->stackpos]);
}

void DYLL_MRBC2_PushInt(DYLL_MRBC2_Context *ctx, s64 val)
{
	ctx->stack[ctx->stackpos++]=dytfInt(val);
}

s64 DYLL_MRBC2_PopInt(DYLL_MRBC2_Context *ctx)
{
	dytf p;
	ctx->stackpos--;
	p=ctx->stack[ctx->stackpos];
	return(dytfIntv(p));
}

void DYLL_MRBC2_PushFloat(DYLL_MRBC2_Context *ctx, double val)
{
	ctx->stack[ctx->stackpos++]=dytfReal(val);
}

double DYLL_MRBC2_PopFloat(DYLL_MRBC2_Context *ctx)
{
	dytf p;
	ctx->stackpos--;
	p=ctx->stack[ctx->stackpos];
	return(dytfRealv(p));
}

void DYLL_MRBC2_PushMark(DYLL_MRBC2_Context *ctx)
{
	ctx->markstack[ctx->markstackpos++]=ctx->stackpos;
}

void DYLL_MRBC2_PopMark(DYLL_MRBC2_Context *ctx)
{
	ctx->markstackpos--;
	ctx->stackpos=ctx->markstack[ctx->markstackpos];
}

void DYLL_MRBC2_CallBlock(DYLL_MRBC2_Context *ctx, char *ip)
{
	ctx->ipstack[ctx->ipstackpos++]=ctx->ip;
	ctx->ip=ip;
}

int DYLL_MRBC2_RetBlock(DYLL_MRBC2_Context *ctx)
{
	if(ctx->ipstackpos<=0)
		return(-1);
	ctx->ipstackpos--;
	ctx->ip=ctx->ipstack[ctx->ipstackpos];
	return(0);
}

char *DYLL_MRBC2_ReadString(DYLL_MRBC2_Context *ctx)
{
	char tb[1024];
	char *s, *t;
	int i, j, k;

	s=ctx->ip; t=tb; i=1;
	s++;	//eat '('
	while(*s)
	{
		if(*s==')')
		{
			i--;
			if(!i) { s++; break; }
			*t++=*s++; continue;
		}

		if(*s=='(') { i++; *t++=*s++; continue; }

		if(*s=='~')
		{
			s++;
			j=*s++;
			switch(j)
			{
			case '~': *t++=j; break;
			case '(': *t++=j; break;
			case ')': *t++=j; break;
			case 'n': *t++='\n'; break;
			case 'r': *t++='\r'; break;
			case 't': *t++='\t'; break;
			default: *t++=j; break;
			}
			continue;
		}

		*t++=*s++;
	}
	ctx->ip=s;

	s=dystrdup(tb);
	return(s);
}

char *DYLL_MRBC2_BlockString(DYLL_MRBC2_Context *ctx, char *str)
{
	char *t;

	t=gctalloc("_dyll_mrbc_block_t", strlen(str)+1);
	strcpy(t, str);
	return(t);
}

char *DYLL_MRBC2_ReadBlock(DYLL_MRBC2_Context *ctx)
{
	char tb[1024];
	char *s, *t;
	int i, j, k;

	s=ctx->ip; t=tb; i=1;
	s++;	//eat '{'
	while(*s)
	{
		if(*s=='}')
		{
			i--;
			if(!i) { s++; break; }
			*t++=*s++; continue;
		}

		if(*s=='{') { i++; *t++=*s++; continue; }
		*t++=*s++;
	}
	ctx->ip=s;

	s=DYLL_MRBC2_BlockString(ctx, tb);
	return(s);
}

dytf DYLL_MRBC2_ReadData(DYLL_MRBC2_Context *ctx)
{
	byte *buf, *buf1;
	char *s, *t;
	int i, j, k, sz;

	s=ctx->ip; i=1;
	s++;	//eat '<'

	if(*s=='~')
	{
		s++;
		buf=NULL; sz=0;
		i=DYLL_MRBC2_ParseBase85Data(&s, &buf, &sz);
		buf1=(byte *)gcalloc(i);
		memcpy(buf1, buf, i);
		free(buf);

		while(*s && *s<=' ')s++;
		if(*s=='~')s++;
		if(*s=='>')s++;

		ctx->ip=s;
		return(dytfWrap((dyt)buf1));
	}

	if(*s=='|')
	{
		s++;
		buf=NULL; sz=0;
		i=DYLL_MRBC2_ParseBase64Data(&s, &buf, &sz);
		buf1=(byte *)gcalloc(i);
		memcpy(buf1, buf, i);
		free(buf);

		while(*s && *s<=' ')s++;
		if(*s=='|')s++;
		if(*s=='>')s++;

		ctx->ip=s;
		return(dytfWrap((dyt)buf1));
	}

	s++;
	buf=NULL; sz=0;
	i=DYLL_MRBC2_ParseHexData(&s, &buf, &sz);
	buf1=(byte *)gcalloc(i);
	memcpy(buf1, buf, i);
	free(buf);

	while(*s && *s<=' ')s++;
	if(*s=='>')s++;

	ctx->ip=s;
	return(dytfWrap((dyt)buf1));
}

int DYLL_MRBC2_ReadInt(DYLL_MRBC2_Context *ctx)
{
	char *ip;
	int i, j, k;

	ip=ctx->ip; i=0;
	if(*ip=='-') { ip++; i=-1; }
	while(*ip)
	{
		if((*ip<'0') || (*ip>'9'))
			break;
		i=(i*10)+((*ip++)-'0');
	}
	ctx->ip=ip;
	return(i);
}

int DYLL_MRBC2_StepInt(DYLL_MRBC2_Context *ctx)
{
	int i, j, k;

	switch(*ctx->ip++)
	{
	case '0': case '1': case '2': case '3':
	case '4': case '5': case '6': case '7':
	case '8': case '9':
		ctx->ip--;
		i=DYLL_MRBC2_ReadInt(ctx);
		DYLL_MRBC2_PushInt(ctx, i);
		break;

	case '+':
		j=DYLL_MRBC2_PopInt(ctx); i=DYLL_MRBC2_PopInt(ctx);
		DYLL_MRBC2_PushInt(ctx, i+j);
		break;

	case '-':
		if((*ctx->ip>='0') && (*ctx->ip<='9'))
		{
			ctx->ip--;
			i=DYLL_MRBC2_ReadInt(ctx);
			DYLL_MRBC2_PushInt(ctx, i);
			break;
		}
		j=DYLL_MRBC2_PopInt(ctx); i=DYLL_MRBC2_PopInt(ctx);
		DYLL_MRBC2_PushInt(ctx, i-j);
		break;

	case '*':
		j=DYLL_MRBC2_PopInt(ctx); i=DYLL_MRBC2_PopInt(ctx);
		DYLL_MRBC2_PushInt(ctx, i*j);
		break;
	case '/':
		j=DYLL_MRBC2_PopInt(ctx); i=DYLL_MRBC2_PopInt(ctx);
		if(!j) { i=0; j=1; }
		DYLL_MRBC2_PushInt(ctx, i/j);
		break;
	case '%':
		j=DYLL_MRBC2_PopInt(ctx); i=DYLL_MRBC2_PopInt(ctx);
		if(!j) { i=0; j=1; }
		DYLL_MRBC2_PushInt(ctx, i%j);
		break;

	case '&':
		j=DYLL_MRBC2_PopInt(ctx); i=DYLL_MRBC2_PopInt(ctx);
		DYLL_MRBC2_PushInt(ctx, i&j);
		break;
	case '|':
		j=DYLL_MRBC2_PopInt(ctx); i=DYLL_MRBC2_PopInt(ctx);
		DYLL_MRBC2_PushInt(ctx, i|j);
		break;
	case '^':
		j=DYLL_MRBC2_PopInt(ctx); i=DYLL_MRBC2_PopInt(ctx);
		DYLL_MRBC2_PushInt(ctx, i^j);
		break;
	case '~':
		i=DYLL_MRBC2_PopInt(ctx);
		DYLL_MRBC2_PushInt(ctx, ~i);
		break;
	case '_':
		i=DYLL_MRBC2_PopInt(ctx);
		DYLL_MRBC2_PushInt(ctx, -i);
		break;

	case '<':
		j=DYLL_MRBC2_PopInt(ctx); i=DYLL_MRBC2_PopInt(ctx);
		DYLL_MRBC2_PushInt(ctx, i<j);
		break;
	case '>':
		j=DYLL_MRBC2_PopInt(ctx); i=DYLL_MRBC2_PopInt(ctx);
		DYLL_MRBC2_PushInt(ctx, i>j);
		break;
	case '=':
		j=DYLL_MRBC2_PopInt(ctx); i=DYLL_MRBC2_PopInt(ctx);
		DYLL_MRBC2_PushInt(ctx, i==j);
		break;
	case '!':
		switch(*ctx->ip++)
		{
		case '<':
			j=DYLL_MRBC2_PopInt(ctx); i=DYLL_MRBC2_PopInt(ctx);
			DYLL_MRBC2_PushInt(ctx, i>=j);
			break;
		case '>':
			j=DYLL_MRBC2_PopInt(ctx); i=DYLL_MRBC2_PopInt(ctx);
			DYLL_MRBC2_PushInt(ctx, i<=j);
			break;
		case '=':
			j=DYLL_MRBC2_PopInt(ctx); i=DYLL_MRBC2_PopInt(ctx);
			DYLL_MRBC2_PushInt(ctx, i!=j);
			break;
		default:
			break;
		}
		break;

	case 'a':
		i=DYLL_MRBC2_PopInt(ctx);
		if(i<0)i=-i;
		DYLL_MRBC2_PushInt(ctx, i);
		break;
	case 'f':
		i=DYLL_MRBC2_PopInt(ctx);
		DYLL_MRBC2_PushFloat(ctx, i);
		break;
	case 'z':
		i=DYLL_MRBC2_PopInt(ctx);
		DYLL_MRBC2_PushInt(ctx, !i);
		break;

	default:
		break;
	}
	return(0);
}

double DYLL_MRBC2_ReadFloat(DYLL_MRBC2_Context *ctx)
{
	char *ip;
	double f, g;
	int i, j, k;

	ip=ctx->ip; f=0;
	if(*ip=='-') { ip++; f=-1; }
	while(*ip)
	{
		if((*ip<'0') || (*ip>'9'))
			break;
		f=(f*10)+((*ip++)-'0');
	}
	if(*ip=='.')
	{
		ip++; g=0.1;
		while(*ip)
		{
			if((*ip<'0') || (*ip>'9'))
				break;
			f+=g*((*ip++)-'0'); g*=0.1;
		}
	}
	ctx->ip=ip;
	return(f);
}

dytf DYLL_MRBC2_ReadNumber(DYLL_MRBC2_Context *ctx)
{
	char *ip;
	double f, g;
	int i, j, k;

	ip=ctx->ip; f=0;
	if(*ip=='-') { ip++; f=-1; }
	while(*ip)
	{
		if((*ip<'0') || (*ip>'9'))
			break;
		f=(f*10)+((*ip++)-'0');
	}
	if(*ip=='.')
	{
		ip++; g=0.1;
		while(*ip)
		{
			if((*ip<'0') || (*ip>'9'))
				break;
			f+=g*((*ip++)-'0'); g*=0.1;
		}
		ctx->ip=ip;
		return(dytfReal(f));
	}
	ctx->ip=ip;
	return(dytfInt((s64)f));
}

int DYLL_MRBC2_StepFloat(DYLL_MRBC2_Context *ctx)
{
	double f, g, h;
	int i, j, k;

	switch(*ctx->ip++)
	{
	case '0': case '1': case '2': case '3':
	case '4': case '5': case '6': case '7':
	case '8': case '9':
		ctx->ip--;
		f=DYLL_MRBC2_ReadFloat(ctx);
		DYLL_MRBC2_PushFloat(ctx, f);
		break;

	case '+':
		g=DYLL_MRBC2_PopFloat(ctx); f=DYLL_MRBC2_PopFloat(ctx);
		DYLL_MRBC2_PushFloat(ctx, f+g);
		break;

	case '-':
		if((*ctx->ip>='0') && (*ctx->ip<='9'))
		{
			ctx->ip--;
			f=DYLL_MRBC2_ReadInt(ctx);
			DYLL_MRBC2_PushFloat(ctx, f);
			break;
		}
		g=DYLL_MRBC2_PopFloat(ctx); f=DYLL_MRBC2_PopFloat(ctx);
		DYLL_MRBC2_PushFloat(ctx, f-g);
		break;

	case '*':
		g=DYLL_MRBC2_PopFloat(ctx); f=DYLL_MRBC2_PopFloat(ctx);
		DYLL_MRBC2_PushFloat(ctx, f*g);
		break;
	case '/':
		g=DYLL_MRBC2_PopFloat(ctx); f=DYLL_MRBC2_PopFloat(ctx);
		if(!g) { f=0; g=1; }
		DYLL_MRBC2_PushFloat(ctx, f/g);
		break;
	case '%':
		g=DYLL_MRBC2_PopFloat(ctx); f=DYLL_MRBC2_PopFloat(ctx);
		if(!g) { f=0; g=1; }
		DYLL_MRBC2_PushFloat(ctx, fmod(f, g));
		break;
	case '^':
		g=DYLL_MRBC2_PopFloat(ctx); f=DYLL_MRBC2_PopFloat(ctx);
		DYLL_MRBC2_PushFloat(ctx, pow(f, g));
		break;

#if 0
	case '&':
		j=DYLL_MRBC2_PopInt(ctx); i=DYLL_MRBC2_PopInt(ctx);
		DYLL_MRBC2_PushInt(ctx, i&j);
		break;
	case '|':
		j=DYLL_MRBC2_PopInt(ctx); i=DYLL_MRBC2_PopInt(ctx);
		DYLL_MRBC2_PushInt(ctx, i|j);
		break;
	case '^':
		j=DYLL_MRBC2_PopInt(ctx); i=DYLL_MRBC2_PopInt(ctx);
		DYLL_MRBC2_PushInt(ctx, i^j);
		break;
	case '~':
		i=DYLL_MRBC2_PopInt(ctx);
		DYLL_MRBC2_PushInt(ctx, ~i);
		break;
#endif

	case '_':
		f=DYLL_MRBC2_PopFloat(ctx);
		DYLL_MRBC2_PushFloat(ctx, -f);
		break;

	case '<':
		g=DYLL_MRBC2_PopFloat(ctx); f=DYLL_MRBC2_PopFloat(ctx);
		DYLL_MRBC2_PushInt(ctx, f<g);
		break;
	case '>':
		g=DYLL_MRBC2_PopFloat(ctx); f=DYLL_MRBC2_PopFloat(ctx);
		DYLL_MRBC2_PushInt(ctx, f>g);
		break;
	case '=':
		g=DYLL_MRBC2_PopFloat(ctx); f=DYLL_MRBC2_PopFloat(ctx);
		DYLL_MRBC2_PushInt(ctx, f==g);
		break;
	case '!':
		switch(*ctx->ip++)
		{
		case '<':
			g=DYLL_MRBC2_PopFloat(ctx); f=DYLL_MRBC2_PopFloat(ctx);
			DYLL_MRBC2_PushInt(ctx, f>=g);
			break;
		case '>':
			g=DYLL_MRBC2_PopFloat(ctx); f=DYLL_MRBC2_PopFloat(ctx);
			DYLL_MRBC2_PushInt(ctx, f<=g);
			break;
		case '=':
			g=DYLL_MRBC2_PopFloat(ctx); f=DYLL_MRBC2_PopFloat(ctx);
			DYLL_MRBC2_PushInt(ctx, f!=g);
			break;
		default:
			break;
		}
		break;

	case 'a':
		f=DYLL_MRBC2_PopFloat(ctx);
		if(f<0)f=-f;
		DYLL_MRBC2_PushFloat(ctx, f);
		break;
	case 'i':
		i=DYLL_MRBC2_PopFloat(ctx);
		DYLL_MRBC2_PushInt(ctx, i);
		break;
	case 'z':
		f=DYLL_MRBC2_PopFloat(ctx);
		DYLL_MRBC2_PushInt(ctx, f==0);
		break;

	default:
		break;
	}
	return(0);
}

int DYLL_MRBC2_Step(DYLL_MRBC2_Context *ctx)
{
	dytf p, q, r;
	int i, j, k;
	int op;

	if(!(*ctx->ip))
	{
		i=DYLL_MRBC2_RetBlock(ctx);
		return(i);
	}

	op=*ctx->ip++;
	switch(op)
	{
	case ' ': break;

	case '0': case '1': case '2': case '3':
	case '4': case '5': case '6': case '7':
	case '8': case '9':
		ctx->ip--;
		p=DYLL_MRBC2_ReadNumber(ctx);
		DYLL_MRBC2_Push(ctx, p);
		break;

	case '+':
		q=DYLL_MRBC2_Pop(ctx); p=DYLL_MRBC2_Pop(ctx);
		r=dytfAdd(p, q); DYLL_MRBC2_Push(ctx, r);
		break;
	case '-':
		if((*ctx->ip>='0') && (*ctx->ip<='9'))
		{
			ctx->ip--;
			p=DYLL_MRBC2_ReadNumber(ctx);
			DYLL_MRBC2_Push(ctx, p);
			break;
		}
		q=DYLL_MRBC2_Pop(ctx); p=DYLL_MRBC2_Pop(ctx);
		r=dytfSub(p, q); DYLL_MRBC2_Push(ctx, r);
		break;
	case '*':
		q=DYLL_MRBC2_Pop(ctx); p=DYLL_MRBC2_Pop(ctx);
		r=dytfMul(p, q); DYLL_MRBC2_Push(ctx, r);
		break;
	case '/':
		q=DYLL_MRBC2_Pop(ctx); p=DYLL_MRBC2_Pop(ctx);
		r=dytfDiv(p, q); DYLL_MRBC2_Push(ctx, r);
		break;
	case '%':
		q=DYLL_MRBC2_Pop(ctx); p=DYLL_MRBC2_Pop(ctx);
		r=dytfMod(p, q); DYLL_MRBC2_Push(ctx, r);
		break;

	case '&':
		q=DYLL_MRBC2_Pop(ctx); p=DYLL_MRBC2_Pop(ctx);
		r=dytfAnd(p, q); DYLL_MRBC2_Push(ctx, r);
		break;
	case '|':
		q=DYLL_MRBC2_Pop(ctx); p=DYLL_MRBC2_Pop(ctx);
		r=dytfOr(p, q); DYLL_MRBC2_Push(ctx, r);
		break;
	case '^':
		q=DYLL_MRBC2_Pop(ctx); p=DYLL_MRBC2_Pop(ctx);
		if(dytfRealP(p) || dytfRealP(q))
			{ r=dytfExp(p, q); }
		else	{ r=dytfXor(p, q); }
		DYLL_MRBC2_Push(ctx, r);
		break;

	case '~':
		p=DYLL_MRBC2_Pop(ctx);
		DYLL_MRBC2_Push(ctx, dytfNeg(p));
		break;
	case '<':
		if(*ctx->ip=='=')
		{
			ctx->ip++;
			q=DYLL_MRBC2_Pop(ctx); p=DYLL_MRBC2_Pop(ctx);
			DYLL_MRBC2_PushInt(ctx, dytfLeP(p, q));
			break;
		}

		q=DYLL_MRBC2_Pop(ctx); p=DYLL_MRBC2_Pop(ctx);
		DYLL_MRBC2_PushInt(ctx, dytfLtP(p, q));
		break;
	case '>':
		if(*ctx->ip=='=')
		{
			ctx->ip++;
			q=DYLL_MRBC2_Pop(ctx); p=DYLL_MRBC2_Pop(ctx);
			DYLL_MRBC2_PushInt(ctx, dytfGeP(p, q));
			break;
		}
		q=DYLL_MRBC2_Pop(ctx); p=DYLL_MRBC2_Pop(ctx);
		DYLL_MRBC2_PushInt(ctx, dytfGtP(p, q));
		break;
	case '=':
		switch(*ctx->ip++)
		{
		case '=':
			q=DYLL_MRBC2_Pop(ctx); p=DYLL_MRBC2_Pop(ctx);
			DYLL_MRBC2_PushInt(ctx, dytfEqP(p, q));
			break;
		case '<':
			q=DYLL_MRBC2_Pop(ctx); p=DYLL_MRBC2_Pop(ctx);
			DYLL_MRBC2_PushInt(ctx, dytfLeP(p, q));
			break;
		case '>':
			q=DYLL_MRBC2_Pop(ctx); p=DYLL_MRBC2_Pop(ctx);
			DYLL_MRBC2_PushInt(ctx, dytfGeP(p, q));
			break;

		case '~':
			p=DYLL_MRBC2_Pop(ctx);
			DYLL_MRBC2_Push(ctx, dytfNot(p));
			break;
		case '!':
			p=DYLL_MRBC2_Pop(ctx);
			DYLL_MRBC2_Push(ctx, dytfLNot(p));
			break;
		default:
			break;
		}
		break;
	case '!':
		switch(*ctx->ip)
		{
		case '<':
			ctx->ip++;
			q=DYLL_MRBC2_Pop(ctx); p=DYLL_MRBC2_Pop(ctx);
			DYLL_MRBC2_PushInt(ctx, dytfGeP(p, q));
			break;
		case '>':
			ctx->ip++;
			q=DYLL_MRBC2_Pop(ctx); p=DYLL_MRBC2_Pop(ctx);
			DYLL_MRBC2_PushInt(ctx, dytfLeP(p, q));
			break;
		case '=':
			ctx->ip++;
			q=DYLL_MRBC2_Pop(ctx); p=DYLL_MRBC2_Pop(ctx);
			DYLL_MRBC2_PushInt(ctx, dytfNEqP(p, q));
			break;
		default:
			p=DYLL_MRBC2_Pop(ctx);
			DYLL_MRBC2_Push(ctx, dytfLNot(p));
			break;
		}
		break;

	case 'I':
		DYLL_MRBC2_StepInt(ctx);
		break;
	case 'F':
		DYLL_MRBC2_StepFloat(ctx);
		break;

	case 'b':
		j=ctx->markstack[ctx->markstackpos-1];
		i=ctx->stackpos-j;
		DYLL_MRBC2_PushInt(ctx, i);
		break;
	case 'c':
		DYLL_MRBC2_PopMark(ctx);
		break;
	case 'd':
		p=DYLL_MRBC2_Pop(ctx);
		DYLL_MRBC2_Push(ctx, p);
		DYLL_MRBC2_Push(ctx, p);
		break;

	case 'i':
		i=DYLL_MRBC2_PopInt(ctx);
		p=ctx->stack[ctx->stackpos-i-1];
		DYLL_MRBC2_Push(ctx, p);
		break;
	case 'j':
		i=DYLL_MRBC2_PopInt(ctx);
		j=ctx->markstack[ctx->markstackpos-1];
		p=ctx->stack[j+i];
		DYLL_MRBC2_Push(ctx, p);
		break;

	case 'p':
		p=DYLL_MRBC2_Pop(ctx);
		break;
	case 't':
		DYLL_MRBC2_PushInt(ctx, 1);
		break;
	case 'x':
		p=DYLL_MRBC2_Pop(ctx);
		q=DYLL_MRBC2_Pop(ctx);
		DYLL_MRBC2_Push(ctx, p);
		DYLL_MRBC2_Push(ctx, q);
		break;
	case 'z':
		DYLL_MRBC2_PushInt(ctx, 0);
		break;

	case '_':
		if(*ctx->ip=='<')
		{
			p=DYLL_MRBC2_ReadData(ctx);
			DYLL_MRBC2_Push(ctx, p);
			break;
		}

		DYLL_MRBC2_PushMark(ctx);
		break;
	case '{':
		ctx->ip--;
		p=dytfWrap((dyt)DYLL_MRBC2_ReadBlock(ctx));
		DYLL_MRBC2_Push(ctx, p);
		break;
	case '(':
		ctx->ip--;
		p=dytfWrap((dyt)DYLL_MRBC2_ReadString(ctx));
		DYLL_MRBC2_Push(ctx, p);
		break;

	default:
		break;
	}
	return(0);
}



BGBDY_API int DYLL_MRBC2_EvalVoid(char *str)
{
	DYLL_MRBC2_Context *ctx;
	int i;

	ctx=DYLL_MRBC2_AllocContext();
	ctx->ip=str;
	while(1)
	{
		i=DYLL_MRBC2_Step(ctx);
		if(i)break;
	}
	DYLL_MRBC2_FreeContext(ctx);
	return(0);
}

BGBDY_API void *DYLL_MRBC2_EvalPVoid(char *str)
{
	DYLL_MRBC2_Context *ctx;
	dytf p;
	int i;

	ctx=DYLL_MRBC2_AllocContext();
	ctx->ip=str;
	while(1)
	{
		i=DYLL_MRBC2_Step(ctx);
		if(i)break;
	}
	p=DYLL_MRBC2_Pop(ctx);
	DYLL_MRBC2_FreeContext(ctx);
	return((void *)dytfUnwrap(p));
}

BGBDY_API int DYLL_MRBC2_EvalInt(char *str)
{
	DYLL_MRBC2_Context *ctx;
	dytf p;
	int i;

	ctx=DYLL_MRBC2_AllocContext();
	ctx->ip=str;
	while(1)
	{
		i=DYLL_MRBC2_Step(ctx);
		if(i)break;
	}
	i=DYLL_MRBC2_PopInt(ctx);
	DYLL_MRBC2_FreeContext(ctx);
	return(i);
}

BGBDY_API double DYLL_MRBC2_EvalFloat(char *str)
{
	DYLL_MRBC2_Context *ctx;
	double f;
	int i;

	ctx=DYLL_MRBC2_AllocContext();
	ctx->ip=str;
	while(1)
	{
		i=DYLL_MRBC2_Step(ctx);
		if(i)break;
	}
	f=DYLL_MRBC2_PopFloat(ctx);
	DYLL_MRBC2_FreeContext(ctx);
	return(f);
}

