#include <bgbmid.h>

BGBMID_BMSR_Context *BGBMID_BMSR_AllocContext()
{
	BGBMID_BMSR_Context *ctx;
	
	ctx=bgbmid_malloc(sizeof(BGBMID_BMSR_Context));
	return(ctx);
}

double BGBMID_BMSR_MatchCompareBlock(int *dct1, int *dct2, int blksz)
{
	double e;
	int i, j, k;
	
	e=0;
	for(i=0; i<blksz; i++)
	{
		j=dct1[i]-dct2[i];
		e+=((double)j)*j;
	}
	e=sqrt(e);
	return(e);
}

int BGBMID_BMSR_MatchCompareSample(
	BGBMID_BMSR_Context *ctx, BGBMID_BMSR_Sample *samp)
{
	int tblk[BGBMID_BMSR_BLOCKSZ];
	double e, f, g;
	int i, j, k;

	e=0;
	for(i=0; i<samp->nblks; i++)
	{
		BGBMID_GetPriorRecordFreqBlock(
			ctx->bmctx, samp->nblks-i-1, tblk,
			BGBMID_BMSR_BLOCKSZ);
		f=BGBMID_BMSR_MatchCompareBlock(
			samp->blks+i*BGBMID_BMSR_BLOCKSZ,
			tblk, BGBMID_BMSR_BLOCKSZ);
		e=e+f*f;
	}
	e=e/samp->nblks;
	e=sqrt(e);
	return(e);
}

BGBMID_BMSR_Sample *BGBMID_BMSR_LookupMatch(
	BGBMID_BMSR_Context *ctx)
{
	BGBMID_BMSR_Sample *cur, *best;
	int e, be;
	
	cur=ctx->samp; best=NULL; be=2048*BGBMID_BMSR_BLOCKSZ;
	while(cur)
	{
		e=BGBMID_BMSR_MatchCompareSample(ctx, cur);
		if(e<be) { best=cur; be=e; }
		cur=cur->next;
	}
	return(best);
}

char *BGBMID_BMSR_LookupMatchWord(
	BGBMID_BMSR_Context *ctx)
{
	BGBMID_BMSR_Sample *cur;
	
	cur=BGBMID_BMSR_LookupMatch(ctx);
	if(cur)return(cur->word);
	return(NULL);
}

BGBMID_BMSR_Sample *BGBMID_BMSR_LookupWordPatch(
	BGBMID_BMSR_Context *ctx, char *word)
{
	BGBMID_BMSR_Sample *cur;
	
	cur=ctx->samp;
	while(cur)
	{
		if(!strcmp(cur->word, word))
			return(cur);
		cur=cur->next;
	}
	return(NULL);
}

BGBMID_BMSR_Sample *BGBMID_BMSR_GetWordPatch(
	BGBMID_BMSR_Context *ctx, char *word)
{
	BGBMID_BMSR_Sample *cur;
	
	cur=BGBMID_BMSR_LookupWordPatch(ctx, word);
	if(cur)return(cur);

	cur=bgbmid_malloc(sizeof(BGBMID_BMSR_Sample));
	cur->word=bgbmid_strdup(word);
	
	cur->next=ctx->samp;
	ctx->samp=cur;
	
	return(cur);
}

int BGBMID_BMSR_LoadWordPatch(BGBMID_BMSR_Context *ctx,
	char *word, char *patch)
{
	char tb[256];
	BGBMID_BMSR_Sample *samp;
	short *bp;
	float f, g;
	int ch, rt, bts, len, isz;
	int i, j, k, n, nb, bs;

	if(ctx->base)sprintf(tb, "%s/%s", ctx->base, patch);
		else strcpy(tb, patch);

	bp=(short *)BGBMID_LoadWAV_8Mono16(tb, &len);
	if(!bp)return(-1);

	samp=BGBMID_BMSR_GetWordPatch(ctx, word);
	samp->buf=bp;
	samp->len=len;
	
	bs=ctx->blksamp;
//	nb=(len+bs-1)/bs;
	nb=len/bs;
	samp->blks=bgbmid_malloc(nb*BGBMID_BMSR_BLOCKSZ*sizeof(int));
	samp->nblks=nb;
	samp->blksamp=bs;
	
	k=0;
	for(i=0; i<len; i++)
	{
		j=abs(bp[i]);
		if(j>k)
			k=j;
	}
	
	f=4096.0/(k+1);
	if(f<1.0)f=1.0;
	if(f>8.0)f=8.0;
	for(i=0; i<len; i++)
		{ bp[i]=bp[i]*f; }
	
	for(i=0; i<nb; i++)
	{
		BGBMID_TransformSamplesToMatchBlock(
			samp->blks+i*BGBMID_BMSR_BLOCKSZ,
			bp+i*bs, bs, bs);
	}
	
	return(1);
}

int BGBMID_BMSR_LoadProfile(BGBMID_BMSR_Context *ctx, char *lst)
{
//	void *fd, *fd2;
	char tb[256];
	char *ibuf;
	char **a, *s, *t, *cs;
//	BSG_DictEnt *ts;
	short *bp;
	int ch, rt, bts, len, isz;
	int i, j, k, n;

	if(ctx->base)sprintf(tb, "%s/%s", ctx->base, lst);
		else strcpy(tb, lst);
//	fd=bgbmid_fopen(buf, "rt");
	ibuf=BGBMID_LoadFile(tb, &isz);
	if(!ibuf)return(-1);

//	printf("Load Dict %s\n", tb);

	n=0; cs=ibuf;
	while(*cs)
	{
		s=bgbmid_rgets(&cs);
		if(!(s[0]))continue;
		if(s[0]=='\n')continue;
		if(s[0]=='#')continue;
		if(s[0]==';')continue;

		a=bgbmid_split(s);
		if(!a[0])continue;

//		printf("W %s\n", a[0]);

		if(!strcmp(a[0], "wordpatch"))
		{
			BGBMID_BMSR_LoadWordPatch(ctx, a[1], a[2]);
			continue;
		}

	}

	bgbmid_free(ibuf);
	return(0);
}
