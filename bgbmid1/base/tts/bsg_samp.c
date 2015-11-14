#include <bgbmid.h>

int bsg_stricmp(char *s0, char *s1)
{
	int a, b;
	while(*s0 && *s1)
	{
		a=*s0++; b=*s1++;
		if((a>='A') && (a<='Z'))
			a=a-'A'+'a';
		if((b>='A') && (b<='Z'))
			b=b-'A'+'a';
		if(a>b)return(1);
		if(b>a)return(-1);
	}
	if(*s0)return(1);
	if(*s1)return(-1);
	return(0);
}

short *BSG_LoadSamp_44Mono16(BSG_Voice *ctx, char *name, int *rlen)
{
	char tb[256];
	byte *buf;
	int sz;
	short *bp;
	char *s;
	int len;

	if(ctx->base)sprintf(tb, "%s/%s", ctx->base, name);
		else strcpy(tb, name);

	bp=(short *)BGBMID_LoadWAV_44Mono16(tb, rlen);
	if(bp)return(bp);

	strcpy(tb, name);
	s=tb+strlen(tb);
	while((s>tb) && (*s!='.'))s--;
	if(*s=='.')strcpy(s, ".wz");

	buf=BGBMID_PackReadFile(tb, &sz);
	if(buf)
	{
		bp=SGWZ_Decode_44Mono16(buf, rlen);

		if(!bp)printf("Decode Fail %s\n", name);

		free(buf);
		return(bp);
	}

	printf("No Sample %s (%s)\n", name, tb);

	return(NULL);
}

BSG_Samp *BSG_FindSamp(BSG_Voice *ctx, char *name)
{
	BSG_Samp *cur;
	int h;

	h=BSG_DictHash(name);
	cur=ctx->samp[h];
	while(cur)
	{
		if(!strcmp(name, cur->name))
			return(cur);
		cur=cur->next;
	}
	return(NULL);
}

BSG_Samp *BSG_GetSamp(BSG_Voice *ctx, char *name)
{
	char tb[256];
	BSG_Samp *tmp;
	short *bp;
	int len, h;

	tmp=BSG_FindSamp(ctx, name);
	if(tmp)return(tmp);

//	if(ctx->base)sprintf(tb, "%s/%s", ctx->base, name);
//		else strcpy(tb, name);
//	bp=(short *)BSWV_LoadWAV_44Mono16(tb, &len);

//	BSWV_NormalizeSample(bp, bp, len);

	tmp=malloc(sizeof(BSG_Samp));
	memset(tmp, 0, sizeof(BSG_Samp));
	tmp->name=bgbmid_strdup(name);

#if 0
	bp=BSG_LoadSamp_44Mono16(ctx, name, &len);
//	if(!bp)return(NULL);

	if(bp)
	{
		tmp->len=len;
		tmp->buf=bp;
	}
#endif

	h=BSG_DictHash(name);
	tmp->next=ctx->samp[h];
	ctx->samp[h]=tmp;

//	tmp->next=ctx->samp;
//	ctx->samp=tmp;

	return(tmp);
}

int BSG_DictHash(char *str)
{
	char *s;
	int i, j;

	s=str; i=0;
	while(*s)
	{
		j=*s++;
		if((j>='A') && (j<='Z'))j=j-'A'+'a';
		i=(i*251)+j;
	}
	i=((i*251)>>8)&0xFF;
	return(i);
}

BSG_DictEnt *BSG_DictFirst(BSG_Voice *ctx, char *str)
{
	int i;

	if(*str=='.')return(ctx->phon);

	i=BSG_DictHash(str);
	return(ctx->dict[i]);
}

void BSG_LinkDict(BSG_Voice *ctx, BSG_DictEnt *ent)
{
	char *s;
	int i;

	s=ent->name;
	if(*s=='.')
	{
		ent->next=ctx->phon;
		ctx->phon=ent;
		return;
	}

	i=BSG_DictHash(s);
	ent->next=ctx->dict[i];
	ctx->dict[i]=ent;
}

BSG_DictEnt *BSG_FindDict(BSG_Voice *ctx, char *str)
{
	BSG_DictEnt *cur, *lst;

	cur=BSG_DictFirst(ctx, str); lst=NULL;
	while(cur)
	{
		if(!bsg_stricmp(str, cur->name))
		{
			if(lst)
			{
				lst->next=cur->next;
				BSG_LinkDict(ctx, cur);
			}
			return(cur);
		}
		lst=cur; cur=cur->next;
	}

	if(ctx->parent)
		return(BSG_FindDict(ctx->parent, str));

	return(NULL);
}

BSG_DictEnt *BSG_FindDict2(BSG_Voice *ctx, char *str)
{
	BSG_DictEnt *cur, *lst;

	cur=BSG_DictFirst(ctx, str); lst=NULL;
	while(cur)
	{
		if(!strcmp(str, cur->name))
		{
			if(lst)
			{
				lst->next=cur->next;
				BSG_LinkDict(ctx, cur);
			}
			return(cur);
		}
		lst=cur; cur=cur->next;
	}

	if(ctx->parent)
		return(BSG_FindDict2(ctx->parent, str));

	return(NULL);
}

BSG_DictEnt *BSG_FindDictPhon(BSG_Voice *ctx, char *str)
{
	BSG_DictEnt *cur, *best;
	char *s, *t;
	int i, l;

	best=NULL; l=0;
//	cur=ctx->dict;
	cur=ctx->phon;
	while(cur)
	{
		if(cur->name[0]!='.')
		{
			cur=cur->next;
			continue;
		}

		s=str;
		t=cur->name+1;
		i=0;
		while(*s && *t && (*s==*t))
			{ s++; t++; i++; }

		if((*t && (*t!='_')) || ((*t=='_') && t[1]))
		{
			cur=cur->next;
			continue;
		}

		if(i>l)
		{
			best=cur;
			l=i;
		}

		cur=cur->next;
	}

	if(ctx->parent)
	{
		cur=BSG_FindDictPhon(ctx->parent, str);
		if(cur)
		{
			s=str; t=cur->name+1; i=0;
			while(*s && *t && (*s==*t))
				{ s++; t++; i++; }
			if(i>l) { best=cur; l=i; }
		}
	}

	return(best);
}

#if 0
int BSG_LoadDict(BSG_Voice *ctx, char *lst)
{
	void *fd, *fd2;
	char buf[256];
	char **a, *s, *t;
	BSG_DictEnt *ts, *cs;
	short *bp;
	int ch, rt, bts, len;
	int i, j, k, n;

	if(ctx->base)sprintf(buf, "%s/%s", ctx->base, lst);
		else strcpy(buf, lst);
	fd=bgbmid_fopen(buf, "rt");

	if(!fd)return(-1);

	printf("Load Dict %s\n", buf);

	n=0;
	while(!bgbmid_feof(fd))
	{
		memset(buf, 0, 256);
		bgbmid_fgets(buf, 255, fd);
		if(buf[0]=='\n')continue;
		if(buf[0]=='#')continue;
		if(buf[0]==';')continue;

		a=bgbmid_split(buf);
		if(!a[0])continue;

//		printf("W %s\n", a[0]);

//		n++;
//		if(n>=1000)
//		{
//			printf(".");
//			n-=1000;
//		}

		s=a[1];
		while(*s && (*s!='/'))s++;
		if(a[2] || !(*s))
		{
			ts=malloc(sizeof(BSG_DictEnt));
			memset(ts, 0, sizeof(BSG_DictEnt));

			ts->name=strdup(a[0]);

			ts->pat=malloc(16*sizeof(char *));
			for(i=0; a[i+1]; i++)
				ts->pat[i]=strdup(a[i+1]);
			ts->pat[i]=0;

			BSG_LinkDict(ctx, ts);
			continue;
		}

//		bp=(short *)BSWV_LoadWAV(a[1], &ch, &rt, &bts, &len);
//		bp=(short *)BSWV_LoadWAV_44Mono16(a[1], &len);
//		if(!bp)continue;

//		BSWV_NormalizeSample(bp, bp, len);

		ts=malloc(sizeof(BSG_DictEnt));
		memset(ts, 0, sizeof(BSG_DictEnt));

		ts->name=strdup(a[0]);

		ts->samp=BSG_GetSamp(ctx, a[1]);

//		ts->len=len;
//		ts->buf=bp;

//		ts->next=ctx->dict;
//		ctx->dict=ts;

		BSG_LinkDict(ctx, ts);
	}
	bgbmid_fclose(fd);

	printf("Load Dict: OK\n");

	return(0);
}
#endif

#if 1
int BSG_LoadDict(BSG_Voice *ctx, char *lst)
{
//	void *fd, *fd2;
	char tb[256];
	char *ibuf;
	char **a, *s, *t, *cs;
	BSG_DictEnt *ts;
	short *bp;
	int ch, rt, bts, len, isz;
	int i, j, k, n;

	if(ctx->base)sprintf(tb, "%s/%s", ctx->base, lst);
		else strcpy(tb, lst);
//	fd=bgbmid_fopen(buf, "rt");
	ibuf=BGBMID_LoadFile(tb, &isz);
	if(!ibuf)return(-1);

	printf("Load Dict %s\n", tb);

	n=0; cs=ibuf;
	while(*cs)
	{
//		memset(buf, 0, 256);
//		bgbmid_fgets(buf, 255, fd);
		s=bgbmid_rgets(&cs);
		if(!(s[0]))continue;
		if(s[0]=='\n')continue;
		if(s[0]=='#')continue;
		if(s[0]==';')continue;

		a=bgbmid_split(s);
		if(!a[0])continue;

//		printf("W %s\n", a[0]);

//		n++;
//		if(n>=1000)
//		{
//			printf(".");
//			n-=1000;
//		}

		s=a[1];
		while(*s && (*s!='/'))s++;
		if(a[2] || !(*s))
		{
			ts=bgbmid_malloc(sizeof(BSG_DictEnt));
//			memset(ts, 0, sizeof(BSG_DictEnt));

//			ts->name=bgbmid_strdup(a[0]);
			ts->name=strdup(a[0]);

			if(a[2])
			{
				for(i=0; a[i+1]; i++);
				if(i>3)
				{
					ts->pat=bgbmid_malloc(16*sizeof(char *));
					for(i=0; a[i+1]; i++)
						ts->pat[i]=bgbmid_strdup(a[i+1]);
					ts->pat[i]=0;
				}else
				{
					ts->pat=ts->tpat;
					for(i=0; a[i+1]; i++)
						ts->pat[i]=bgbmid_strdup(a[i+1]);
					ts->pat[i]=0;
				}
			}else
			{
				ts->pat=ts->tpat;
//				ts->pat[0]=bgbmid_strdup(a[1]);
				ts->pat[0]=strdup(a[1]);
				ts->pat[1]=0;
			}

			BSG_LinkDict(ctx, ts);
			continue;
		}

//		bp=(short *)BSWV_LoadWAV(a[1], &ch, &rt, &bts, &len);
//		bp=(short *)BSWV_LoadWAV_44Mono16(a[1], &len);
//		if(!bp)continue;

//		BSWV_NormalizeSample(bp, bp, len);

		ts=bgbmid_malloc(sizeof(BSG_DictEnt));
//		memset(ts, 0, sizeof(BSG_DictEnt));

//		ts->name=bgbmid_strdup(a[0]);
		ts->name=strdup(a[0]);

		ts->samp=BSG_GetSamp(ctx, a[1]);

//		ts->len=len;
//		ts->buf=bp;

//		ts->next=ctx->dict;
//		ctx->dict=ts;

		BSG_LinkDict(ctx, ts);
	}

//	bgbmid_fclose(fd);
	bgbmid_free(ibuf);

	printf("Load Dict: OK\n");

	return(0);
}
#endif

