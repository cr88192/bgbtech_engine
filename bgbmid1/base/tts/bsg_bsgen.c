#include <bgbmid.h>

BSG_Voice *bsg_voices=NULL;

BMID_API void BSG_Init(void)
{
	static int init=0;
	
	if(init)return;
	init=1;

	BSG_LoadVoice("common", "voice");
	BSG_LoadVoice("male0", "voice");
	BSG_LoadVoice("male1", "voice");
	BSG_LoadVoice("comp0", "voice");
	BSG_LoadVoice("female0", "voice");
}

BMID_API BSG_Voice *BSG_LookupVoice(char *name)
{
	BSG_Voice *cur;

	cur=bsg_voices;
	while(cur)
	{
		if(!strcmp(cur->name, name))
			return(cur);
		cur=cur->next;
	}
	return(NULL);
}

BMID_API BSG_Voice *BSG_NewVoice(char *name, char *base)
{
	BSG_Voice *tmp;

	tmp=malloc(sizeof(BSG_Voice));
	memset(tmp, 0, sizeof(BSG_Voice));

	tmp->name=strdup(name);
	tmp->base=strdup(base);

	tmp->next=bsg_voices;
	bsg_voices=tmp;

	return(tmp);
}

BMID_API BSG_Voice *BSG_LoadVoice(char *name, char *base)
{
	char tb[256];
	BSG_Voice *tmp;
	void *fd;
	char **a;

	tmp=BSG_LookupVoice(name);
	if(tmp)return(tmp);

	tmp=BSG_NewVoice(name, base);

	sprintf(tb, "%s/vi_%s.txt", base, name);
	fd=bgbmid_fopen(tb, "rt");
	if(fd)
	{
		while(!bgbmid_feof(fd))
		{
			memset(tb, 0, 256);
			bgbmid_fgets(tb, 255, fd);
			a=bgbmid_split(tb);

			if(!a[0])continue;
			if(a[0][0]==';')continue;
			if(a[0][0]=='#')continue;
			if(a[0][0]=='/')continue;

			if(!strcmp(a[0], "parent"))
				tmp->parent=BSG_LookupVoice(a[1]);
			if(!strcmp(a[0], "pack"))
			{
				sprintf(tb, "%s/%s", base, a[1]);
				BGBMID_OpenPack(tb);
			}
			if(!strcmp(a[0], "dict"))
				BSG_LoadDict(tmp, a[1]);
		}
		bgbmid_fclose(fd);
	}

	return(tmp);
}


BMID_API void BSG_PushWord(BSG_State *ctx, char *s)
{
	ctx->wordstack[ctx->wordstackpos++]=bgbmid_strdup(s);
}

BMID_API void BSG_PushWordEnd(BSG_State *ctx, char *s)
{
	int i;

	i=ctx->wordstackpos++;
	for(; i>0; i--)ctx->wordstack[i]=ctx->wordstack[i-1];
	ctx->wordstack[0]=bgbmid_strdup(s);
}

BMID_API void BSG_GenPhon(BSG_State *ctx, char *str)
{
	char *sbuf[16];
	char tb[16];
	BSG_DictEnt *smp, *smp1;
	char *s, *t;
	int i, si, ti;

	si=0; ti=0;
	s=str;

//	if((*s!='T') && (*s!='D') && (*s!='r') && (*s!='b') &&
//		(*s!='C') && (*s!='d') && (*s!='j') && (*s!='k') &&
//		(*s!='n') && (*s!='p') && (*s!='t') && (*s!='Z'))
	if(1)
	{
		i=ctx->lphone;
		if(!i)i='_';
		sprintf(tb, ".%c%c", i, *s);

//		sprintf(tb, "._%c", *s);
		smp=BSG_FindDict2(ctx->voice, tb);
		if(smp)sbuf[si++]=smp->name;

		if(!smp)
		{
			sprintf(tb, ".%c_", i);
			smp=BSG_FindDict2(ctx->voice, tb);
			if(smp)sbuf[si++]=smp->name;

			sprintf(tb, "._%c", *s);
			smp=BSG_FindDict2(ctx->voice, tb);
			if(smp)sbuf[si++]=smp->name;
		}

		ctx->lphone=0;
	}

	while(*s)
	{
		smp=BSG_FindDictPhon(ctx->voice, s);

		if(!smp)
		{
//			printf("lacking diphone for '%s'\n", s);
			s++;
			continue;
		}

		if((!s[1] || !s[2]) && (smp->name[2]=='_'))
			ti=1;

		t=smp->name+1; i=0;
		while(*s && *t && (*s==*t)) { s++; t++; i++; }
		if(*s && !*t && (i>1) && (s[-1]!='_'))
		{
			smp1=BSG_FindDictPhon(ctx->voice, s-1);
			if(smp1 && smp1->name[2] && (smp1->name[2]!='_' || *s=='_'))
				s--;
		}

		sbuf[si++]=smp->name;
	}

#if 0
	if(!ti && (s>(str+1)) && !*s)
	{
		smp=BSG_FindDictPhon(ctx->voice, s-1);
		if(smp && (smp->name[2]=='_'))
			sbuf[si++]=smp->name;
	}
#endif

	while(si)
	{
		si--;
		BSG_PushWord(ctx, sbuf[si]);
	}
}



BMID_API void BSG_OutSamples2(BSG_State *ctx, short *buf, int cnt)
{
	int i;
//	if(ctx->afd)write(ctx->afd, buf, cnt*2);
//	if(ctx->ofd)fwrite(buf, 1, cnt*2, ctx->ofd);

	if(ctx->out_cb)
	{
		i=ctx->out_cb(ctx, buf, cnt);
		return;
	}

	if(ctx->obuf1)
	{
		if((ctx->obuf1pos+cnt)>=ctx->obuf1sz)
		{
			i=ctx->obuf1sz;
			while((ctx->obuf1pos+cnt)>=i)
				i=i+(i/2);
			ctx->obuf1=realloc(ctx->obuf1, i*sizeof(short));
			ctx->obuf1sz=i;
		}

		memcpy(ctx->obuf1+ctx->obuf1pos, buf, cnt*2);
		ctx->obuf1pos+=cnt;
	}
}

BMID_API void BSG_OutStep(BSG_State *ctx, short lv, short iv)
{
	while(((lv+512)<iv) && ((lv+512)>lv))
	{
		lv+=256;
		BSG_OutSamples2(ctx, &lv, 1);
	}

	while(((lv-512)>iv) && ((lv-512)<lv))
	{
		lv-=256;
		BSG_OutSamples2(ctx, &lv, 1);
	}

#if 1
	while((lv+32)<iv)
	{
		lv+=16;
		BSG_OutSamples2(ctx, &lv, 1);
	}

	while((lv-32)>iv)
	{
		lv-=16;
		BSG_OutSamples2(ctx, &lv, 1);
	}
#endif
}

BMID_API void BSG_OutSamples21(BSG_State *ctx, short *buf, int cnt)
{
	static short lv;
	short iv;
	int i;

	iv=buf[0];
	BSG_OutStep(ctx, lv, iv);
	BSG_OutSamples2(ctx, buf, cnt);
	lv=buf[cnt-1];
}

BMID_API void BSG_OutSamples(BSG_State *ctx, short *buf, int cnt)
{
//	static short *buf1=NULL;
//	static short *buf2=NULL;
	short *buf1;
	int i, j, k, l, n;
	float f, g;

	if((ctx->ivl==ctx->isl) && (ctx->ivl==ctx->irl))
	{
		BSG_OutSamples21(ctx, buf, cnt);
		return;
	}

//	printf("phase\n");

	memcpy(ctx->obuf+ctx->obufpos, buf, cnt*2);
	ctx->obufpos+=cnt;

	n=ctx->obufpos/ctx->ivl;
	if(n)
	{
		buf1=malloc((n+1)*ctx->irl*sizeof(short));

		BGBMID_SampleCompressor(buf1, n*ctx->irl, ctx->obuf, n*ctx->ivl,
			ctx->ivl, ctx->isl, ctx->irl);

		BSG_OutSamples21(ctx, buf1, n*ctx->irl);

		k=n*ctx->ivl;
		j=ctx->obufpos-k;
		for(i=0; i<j; i++)ctx->obuf[i]=ctx->obuf[i+k];
		ctx->obufpos-=k;

		free(buf1);
	}

#if 0
	if(!buf1)
	{
		buf1=malloc(1<<16);
		buf2=malloc(1<<16);
		memset(buf1, 0, 65536);
		memset(buf2, 0, 65536);
	}

	while(ctx->obufpos>ctx->ivl)
	{
#if 0
		j=ctx->isl; k=0;
//		BSWV_ScaleSample(buf2, j, ctx->obuf, ctx->ivl);
//		BSWV_ScaleSample(buf2+j, j, ctx->obuf, ctx->ivl);

		while(k<ctx->irl)
		{
			BGBMID_ScaleSample(buf2+k, j, ctx->obuf, ctx->ivl);

			if(k)
			{
				l=(buf2[k-1]+buf2[k])/2;
				buf2[k-1]=(buf2[k-1]+l)/2;
				buf2[k-2]=(buf2[k-2]+buf2[k-1])/2;
				buf2[k-3]=(buf2[k-3]+buf2[k-2])/2;
				buf2[k]=(buf2[k]+l)/2;
				buf2[k+1]=(buf2[k+1]+buf2[k])/2;
				buf2[k+2]=(buf2[k+2]+buf2[k+1])/2;
				buf2[k+3]=(buf2[k+3]+buf2[k+2])/2;
			}

			k+=j;
		}

		j=ctx->irl*2;
		g=ctx->irl;
		for(i=0; i<j; i++)
		{
			k=buf1[i+ctx->irl];
			k+=buf2[i];
			if(k<(-32767))k=-32767;
			if(k>32767)k=32767;
			buf1[i]=k;
		}
#endif

		BGBMID_SampleCompressor(buf1, ctx->irl, ctx->obuf, ctx->ivl,
			ctx->ivl, ctx->isl, ctx->irl);

		BSG_OutSamples21(ctx, buf1, ctx->irl);

		j=ctx->obufpos-ctx->ivl;
		for(i=0; i<j; i++)
			ctx->obuf[i]=ctx->obuf[i+ctx->ivl];
		ctx->obufpos-=ctx->ivl;
	}
#endif
}


BMID_API void BSG_GenDelay(BSG_State *ctx, int ms)
{
	short buf[256];
	int i, j;

	i=(ms*44100)/1000;

	memset(buf, 0, 512);

	for(j=0; j<(i/256); j++)
		BSG_OutSamples(ctx, buf, 256);
}

BMID_API void BSG_OutDict(BSG_State *ctx, BSG_DictEnt *smp)
{
	static short *buf=NULL;
	short *bp;
	int len;
	int j;

	if(!buf)buf=malloc(1<<20);

	if(!smp->samp)
	{
		if(!smp->pat)return;

		for(j=0; smp->pat[j]; j++);
		j--;
		for(; j>=0; j--)
			BSG_PushWord(ctx, smp->pat[j]);
		return;
	}

	if(!smp->samp->buf && !smp->samp->len)
	{
		bp=BSG_LoadSamp_44Mono16(ctx->voice, smp->samp->name, &len);
		if(bp)
		{
			smp->samp->len=len;
			smp->samp->buf=bp;
		}else smp->samp->len=-1;
	}

	if(!smp->samp->buf)
		return;

	memcpy(buf, smp->samp->buf, smp->samp->len*2);

	j=smp->samp->len;
	j=(smp->samp->len*ctx->tone)/100;

	BGBMID_ScaleSample(buf, j, smp->samp->buf, smp->samp->len);
	BSG_OutSamples(ctx, buf, j);

	ctx->lphone=0;
	if(smp->name[0]=='.')
		ctx->lphone=smp->name[strlen(smp->name)-1];
}


BMID_API void BSG_Command(BSG_State *ctx, char *str)
{
	BSG_Voice *vtmp;

	if(!strcmp(str, "^0")) { ctx->tone=100; return; }
	if(!strcmp(str, "_0")) { ctx->tone=100; return; }

	if(!strcmp(str, "^0a")) { ctx->isl=ctx->ivl; return; }
	if(!strcmp(str, "_0a")) { ctx->isl=ctx->ivl; return; }
	if(!strcmp(str, "^0r")) { ctx->irl=ctx->ivl; return; }
	if(!strcmp(str, "_0r")) { ctx->irl=ctx->ivl; return; }

	if(!strcmp(str, "_1")) { ctx->tone=110; return; }
	if(!strcmp(str, "_2")) { ctx->tone=120; return; }
	if(!strcmp(str, "_3")) { ctx->tone=130; return; }
	if(!strcmp(str, "_4")) { ctx->tone=140; return; }
	if(!strcmp(str, "_5")) { ctx->tone=150; return; }
	if(!strcmp(str, "_6")) { ctx->tone=160; return; }
	if(!strcmp(str, "_7")) { ctx->tone=170; return; }
	if(!strcmp(str, "_8")) { ctx->tone=180; return; }
	if(!strcmp(str, "_9")) { ctx->tone=190; return; }

	if(!strcmp(str, "^1")) { ctx->tone=90; return; }
	if(!strcmp(str, "^2")) { ctx->tone=80; return; }
	if(!strcmp(str, "^3")) { ctx->tone=70; return; }
	if(!strcmp(str, "^4")) { ctx->tone=60; return; }
	if(!strcmp(str, "^5")) { ctx->tone=50; return; }
	if(!strcmp(str, "^6")) { ctx->tone=40; return; }
	if(!strcmp(str, "^7")) { ctx->tone=30; return; }
	if(!strcmp(str, "^8")) { ctx->tone=20; return; }
	if(!strcmp(str, "^9")) { ctx->tone=10; return; }

	if(!strcmp(str, "_1a")) { ctx->isl=ctx->ivl*1.10; return; }
	if(!strcmp(str, "_2a")) { ctx->isl=ctx->ivl*1.20; return; }
	if(!strcmp(str, "_3a")) { ctx->isl=ctx->ivl*1.30; return; }
	if(!strcmp(str, "_4a")) { ctx->isl=ctx->ivl*1.40; return; }
	if(!strcmp(str, "_5a")) { ctx->isl=ctx->ivl*1.50; return; }
	if(!strcmp(str, "_6a")) { ctx->isl=ctx->ivl*1.60; return; }
	if(!strcmp(str, "_7a")) { ctx->isl=ctx->ivl*1.70; return; }
	if(!strcmp(str, "_8a")) { ctx->isl=ctx->ivl*1.80; return; }
	if(!strcmp(str, "_9a")) { ctx->isl=ctx->ivl*1.90; return; }

	if(!strcmp(str, "_10a")) { ctx->isl=ctx->ivl*2.00; return; }
	if(!strcmp(str, "_11a")) { ctx->isl=ctx->ivl*2.10; return; }
	if(!strcmp(str, "_12a")) { ctx->isl=ctx->ivl*2.20; return; }
	if(!strcmp(str, "_13a")) { ctx->isl=ctx->ivl*2.30; return; }
	if(!strcmp(str, "_14a")) { ctx->isl=ctx->ivl*2.40; return; }
	if(!strcmp(str, "_15a")) { ctx->isl=ctx->ivl*2.50; return; }
	if(!strcmp(str, "_16a")) { ctx->isl=ctx->ivl*2.60; return; }
	if(!strcmp(str, "_17a")) { ctx->isl=ctx->ivl*2.70; return; }
	if(!strcmp(str, "_18a")) { ctx->isl=ctx->ivl*2.80; return; }
	if(!strcmp(str, "_19a")) { ctx->isl=ctx->ivl*2.90; return; }

	if(!strcmp(str, "^1a")) { ctx->isl=ctx->ivl*0.90; return; }
	if(!strcmp(str, "^2a")) { ctx->isl=ctx->ivl*0.80; return; }
	if(!strcmp(str, "^3a")) { ctx->isl=ctx->ivl*0.70; return; }
	if(!strcmp(str, "^4a")) { ctx->isl=ctx->ivl*0.60; return; }
	if(!strcmp(str, "^5a")) { ctx->isl=ctx->ivl*0.50; return; }
	if(!strcmp(str, "^6a")) { ctx->isl=ctx->ivl*0.40; return; }
	if(!strcmp(str, "^7a")) { ctx->isl=ctx->ivl*0.30; return; }
	if(!strcmp(str, "^8a")) { ctx->isl=ctx->ivl*0.20; return; }
	if(!strcmp(str, "^9a")) { ctx->isl=ctx->ivl*0.10; return; }

	if(!strcmp(str, "_1r")) { ctx->irl=ctx->ivl*1.10; return; }
	if(!strcmp(str, "_2r")) { ctx->irl=ctx->ivl*1.20; return; }
	if(!strcmp(str, "_3r")) { ctx->irl=ctx->ivl*1.30; return; }
	if(!strcmp(str, "_4r")) { ctx->irl=ctx->ivl*1.40; return; }
	if(!strcmp(str, "_5r")) { ctx->irl=ctx->ivl*1.50; return; }
	if(!strcmp(str, "_6r")) { ctx->irl=ctx->ivl*1.60; return; }
	if(!strcmp(str, "_7r")) { ctx->irl=ctx->ivl*1.70; return; }
	if(!strcmp(str, "_8r")) { ctx->irl=ctx->ivl*1.80; return; }
	if(!strcmp(str, "_9r")) { ctx->irl=ctx->ivl*1.90; return; }

	if(!strcmp(str, "^1r")) { ctx->irl=ctx->ivl*0.90; return; }
	if(!strcmp(str, "^2r")) { ctx->irl=ctx->ivl*0.80; return; }
	if(!strcmp(str, "^3r")) { ctx->irl=ctx->ivl*0.70; return; }
	if(!strcmp(str, "^4r")) { ctx->irl=ctx->ivl*0.60; return; }
	if(!strcmp(str, "^5r")) { ctx->irl=ctx->ivl*0.50; return; }
	if(!strcmp(str, "^6r")) { ctx->irl=ctx->ivl*0.40; return; }
	if(!strcmp(str, "^7r")) { ctx->irl=ctx->ivl*0.30; return; }
	if(!strcmp(str, "^8r")) { ctx->irl=ctx->ivl*0.20; return; }
	if(!strcmp(str, "^9r")) { ctx->irl=ctx->ivl*0.10; return; }

	if(*str=='_')
	{
		if(str[1]=='.')
		{
			BSG_GenDelay(ctx, atoi(str+2));
			return;
		}

		vtmp=BSG_LookupVoice(str+1);
		if(vtmp)ctx->voice=vtmp;
		return;
	}

//	if(!strcmp(str, "_"))
//	{
//		gen_delay(ctx, 100);
//		return;
//	}
}

BMID_API void BSG_ProcessItem(BSG_State *ctx)
{
	char *s, *t;
	BSG_DictEnt *smp;
	int i, j, k;

	ctx->wordstackpos--;
	s=ctx->wordstack[ctx->wordstackpos];

//	printf("\n");
//	printf("%s ", s);

	if(((*s=='_') || (*s=='^')) && s[1])
	{
		BSG_Command(ctx, s);
		return;
	}

#if 0
	i=strlen(s)-1;
	if((s[i]==',') || (s[i]=='.' || (s[i]==':')))
	{
		s[i]=0;
		BSG_PushWord(ctx, "_.250");
		BSG_PushWord(ctx, "_");
	}
#endif

	smp=BSG_FindDict2(ctx->voice, s);
	if(smp)
	{
		BSG_OutDict(ctx, smp);
		return;
	}

	if(((*s>='0') && (*s<='9')) ||
		((*s=='-') && (s[1]>='0') && (s[1]<='9')))
	{
		BSG_GenNumber(ctx, s);
		return;
	}

	if(*s=='*')
	{
		BSG_GenPhon(ctx, s+1);
		return;
	}

	if(!smp)smp=BSG_FindDict(ctx->voice, s);
	if(!smp)
	{
		BSG_GenDecipher(ctx, s);
//		BSG_GenAcron(ctx, s);
		return;
	}

	BSG_OutDict(ctx, smp);
}

BMID_API void BSG_ProcessItems(BSG_State *ctx)
{
	while(ctx->wordstackpos)
		BSG_ProcessItem(ctx);
}

BMID_API void BSG_GenWord(BSG_State *ctx, char *str)
{
	ctx->wordstack[ctx->wordstackpos++]=str;

	while(ctx->wordstackpos)
		BSG_ProcessItem(ctx);
}

BMID_API int BSG_ReadSamples(BSG_State *ctx, short *buf, int len)
{
	int i, j;

	while((ctx->obuf1pos<len) && ctx->wordstackpos)
		BSG_ProcessItem(ctx);

	i=ctx->obuf1pos;
	if(i<len)
	{
		memcpy(buf, ctx->obuf1, i*sizeof(short));
		ctx->obuf1pos=0;
		return(i);
	}

	memcpy(buf, ctx->obuf1, len*sizeof(short));

	i-=len;
	for(j=0; j<i; j++)
		ctx->obuf1[j]=ctx->obuf1[j+len];
	ctx->obuf1pos-=len;
	return(len);
}


BMID_API BSG_State *BSG_NewContext(char *voice)
{
	BSG_State *ctx;
	BSG_Voice *vc;

	ctx=malloc(sizeof(BSG_State));
	memset(ctx, 0, sizeof(BSG_State));

	vc=BSG_LookupVoice(voice);

	ctx->voice=vc;

	ctx->wordstack=malloc(4096*sizeof(char *));
	ctx->wordstackpos=0;

	ctx->obuf=malloc(1<<20);
	ctx->obufpos=0;

	ctx->obuf1=malloc((1<<16)*sizeof(short));
	ctx->obuf1pos=0;
	ctx->obuf1sz=1<<16;

	ctx->tone=100;

//	ctx->ivl=1470;
	ctx->ivl=735;
	ctx->isl=ctx->ivl;
	ctx->irl=ctx->ivl;

//	ctx->isl=926;

	return(ctx);
}

BMID_API void BSG_FreeContext(BSG_State *ctx)
{
	if(ctx->wordstack)
		free(ctx->wordstack);
	if(ctx->obuf)
		free(ctx->obuf);
	if(ctx->obuf1)
		free(ctx->obuf1);
}

char *BSG_EatWhite(char *s)
{
	char *fn;
	int ln;
	int i;

	while(*s && (*s<=' '))s++;

	if((s[0]=='/') && (s[1]=='/'))
	{
		while(*s && (*s!='\n'))s++;
		s=BSG_EatWhite(s);
		return(s);
	}
	if((s[0]=='/') && (s[1]=='*'))
	{
		s+=2;
		i=1;
		while(*s && i)
		{
			if((s[0]=='/') && (s[1]=='*'))
			{
				s+=2;
				i++;
				continue;
			}
			if((s[0]=='*') && (s[1]=='/'))
			{
				s+=2;
				i--;
				continue;
			}
			s++;
		}
		s=BSG_EatWhite(s);
		return(s);
	}

	return(s);
}


int BSG_OpChar(int c)
{
	int i;

	switch(c)
	{
	case '~': case '!':	case '@': case '#':
	case '$': case '%':	case '^': case '&':
	case '*': case '-':	case '=': case '+':
	case '\\': case '|':	case ':': case '<':
	case '>': case '?':	case '/': case '`':
	case '.':
		i=1;
		break;

	default:
		i=0;
		break;
	}
	return(i);
}

char *BSG_Token(char *s, char *b)
{
	char *t;
	int i, j, k;

	*b=0;
	s=BSG_EatWhite(s);
	if(!*s)return(s);
	t=b;

	if((*s=='_') || (*s=='^'))
	{
		while(*s && (*s>' '))*t++=*s++;
		*t++=0;
		return(s);
	}

	if(((*s>='a') && (*s<='z')) || ((*s>='A') && (*s<='Z')))
	{
		while(	((*s>='a') && (*s<='z')) ||
			((*s>='A') && (*s<='Z')))
			*t++=*s++;
		*t++=0;
		return(s);
	}

	if((*s>='0') && (*s<='9'))
	{
		if((s[0]=='0') && (s[1]=='x'))
		{
			*t++=*s++;
			*t++=*s++;
			while(((*s>='0') && (*s<='9')) ||
				((*s>='A') && (*s<='F')) ||
				((*s>='a') && (*s<='f')))
			{
				*t++=*s++;
			}
			*t++=0;
			return(s);
		}

		while((*s>='0') && (*s<='9'))
			*t++=*s++;
		if(*s=='.')*t++=*s++;
		while((*s>='0') && (*s<='9'))
			*t++=*s++;
		if(*s=='e')
		{
			*t++=*s++;
			if((*s=='+') || (*s=='-'))*t++=*s++;
			while((*s>='0') && (*s<='9'))
				*t++=*s++;
		}
		*t++=0;
		return(s);
	}

	if((s[0]=='.') && (s[1]>='0') && (s[1]<='9'))
	{
		*t++='0';
		*t++='.';

		while((*s>='0') && (*s<='9'))
			*t++=*s++;
		if(*s=='e')
		{
			*t++=*s++;
			if((*s=='+') || (*s=='-'))*t++=*s++;
			while((*s>='0') && (*s<='9'))
				*t++=*s++;
		}
		*t++=0;
		return(s);
	}

	if(!strncmp(s, "...", 3))
	{
		*t++=*s++; *t++=*s++; *t++=*s++; *t++=0;
		return(s);
	}

	*t++=*s++; *t++=0;
	return(s);
}

BMID_API void BSG_SendTextSentence(BSG_State *ctx, char *str, char *lt)
{
	char tb[64];
	char *s, *s1;

//	printf("Sentence %s\n", str);

	s=str;
	while(*s)
	{
		s=BSG_Token(s, tb);
		if(!tb[0])continue;

		if((tb[0]=='_') || (tb[0]=='^'))
		{
			BSG_PushWordEnd(ctx, tb);
			continue;
		}

		if(!strcmp(tb, ".") || !strcmp(tb, ",") ||
			!strcmp(tb, ":") || !strcmp(tb, "!") ||
			!strcmp(tb, "?") || !strcmp(tb, "..."))
		{
			continue;
		}


		s1=NULL;
		switch(tb[0])
		{
		case '~': s1="tilde"; break;
		case '@': s1="at"; break;
		case '#': s1="pound"; break;
		case '$': s1="dollar"; break;
		case '%': s1="percent"; break;
		case '^': s1="exp"; break;
		case '&': s1="and"; break;
		case '*': s1="times"; break;
		case '-': s1="minus"; break;
		case '=': s1="equals"; break;
		case '+': s1="plus"; break;
		case '\\': s1="backslash"; break;
		case '|': s1="or"; break;
		case '<': s1="lessthan"; break;
		case '>': s1="greaterthan"; break;
		case '/': s1="slash"; break;
		case '`': s1="backquote"; break;
		default: break;
		}

		if(s1)
		{
			BSG_PushWordEnd(ctx, tb);
			continue;
		}

		s1=tb;
		if(((*s1>='a') && (*s1<='z')) || ((*s1>='A') && (*s1<='Z')))
		{
			BSG_PushWordEnd(ctx, tb);
			continue;
		}

		if(((*s1>='0') && (*s1<='9')) ||
			((*s1=='-') && (s1[1]>='0') && (s1[1]<='9')))
		{
			BSG_PushWordEnd(ctx, tb);
			continue;
		}
	}

	BSG_PushWordEnd(ctx, "_");
	BSG_PushWordEnd(ctx, "_.250");
}

BMID_API void BSG_SendText(BSG_State *ctx, char *str)
{
	char tb[64], tb1[256];
	char *s, *s1, *t;

	s=str; t=tb1;
	while(*s)
	{
		s=BSG_Token(s, tb);
		if(!tb[0])continue;

		strcpy(t, tb);
		t+=strlen(tb);
		*t++=' ';

		if(!strcmp(tb, ".") || !strcmp(tb, "!") ||
			!strcmp(tb, "?") || !strcmp(tb, "..."))
		{
			*t++=0;

			BSG_SendTextSentence(ctx, tb1, tb);
			t=tb1;
		}
	}

	if(t>tb1)
	{
		*t++=0;
		BSG_SendTextSentence(ctx, tb1, NULL);
	}
}

