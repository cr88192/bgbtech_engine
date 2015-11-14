#include <bgbgal.h>

BGBDY_API bgalMember *bgalBreed(bgalMember *obja, bgalMember *objb)
{
	if(obja->ctx->breed)
		return(obja->ctx->breed(obja->ctx, obja, objb));
	return(NULL);
}

BGBDY_API double bgalTest(bgalMember *obj, void *data)
{
	if(obj->ctx->test)
		return(obj->ctx->test(obj->ctx, obj, data));
	return(0);
}

BGBDY_API void bgalStepTest(bgalContext *ctx, void *data)
{
	if(ctx->step_test)
		ctx->step_test(ctx, data);
}

BGBDY_API void bgalStepSelect(bgalContext *ctx)
{
	if(ctx->step_select)
		ctx->step_select(ctx);
}

BGBDY_API void bgalStepBreed(bgalContext *ctx)
{
	if(ctx->step_breed)
		ctx->step_breed(ctx);
}

BGBDY_API void bgalStepStatus(bgalContext *ctx)
{
	if(ctx->step_status)
		ctx->step_status(ctx);
}

BGBDY_API void bgalStep(bgalContext *ctx)
{
	if(ctx->step)
		ctx->step(ctx);
}

BGBDY_API void bgalSetup(bgalContext *ctx, void *data)
{
	if(ctx->setup)
		ctx->setup(ctx, data);
}

BGBDY_API void bgalEvolve(bgalContext *ctx, void *data)
{
	if(ctx->evolve)
		ctx->evolve(ctx, data);
}

BGBDY_API bgalMember *bgalAllocMember(bgalContext *ctx)
{
	bgalMember *tmp;

	if(ctx->free)
	{
		tmp=ctx->free;
		ctx->free=tmp->lnext;
		tmp->lnext=NULL;
		return(tmp);
	}

	if(ctx->alloc_member)
	{
		tmp=ctx->alloc_member(ctx);
		tmp->next=ctx->member;
		ctx->member=tmp;
		return(tmp);
	}

	return(NULL);
}

BGBDY_API void bgalFreeMember(bgalMember *obj)
{
	if(obj->ctx->free_member)
		obj->ctx->free_member(obj->ctx, obj);
}

BGBDY_API void bgalSterilize(bgalContext *ctx)
{
	bgalMember *cur, *nxt;

	cur=ctx->live;
	ctx->live=NULL;
	ctx->cnt_live=0;

	while(cur)
	{
		nxt=cur->lnext;
		bgalFreeMember(cur);
		cur=nxt;
	}
}

BGBDY_API bgalMember *bgalPickMember(bgalContext *ctx, bgalMember *skip)
{
	bgalMember *tmp;

	if(ctx->pick_member)
	{
		tmp=ctx->pick_member(ctx, skip);
		return(tmp);
	}

	return(NULL);
}

BGBDY_API void bgalAddData(bgalContext *ctx, void *data)
{
	int i;

	if(!ctx->data)
	{
		i=16;
		ctx->data=gcalloc(i*sizeof(void *));
		ctx->n_data=0; ctx->m_data=i;
	}

	if((ctx->n_data+1)>=ctx->m_data)
	{
		i=ctx->m_data+(ctx->m_data>>1);
		ctx->data=gcrealloc(ctx->data, i*sizeof(void *));
		ctx->n_data=0; ctx->m_data=i;
	}

	i=ctx->n_data++;
	ctx->data[i]=data;
}

BGBDY_API bgalMember *bgal_alloc_default(bgalContext *ctx)
{
	bgalMember *tmp;

	tmp=gcalloc(sizeof(bgalMember));
	tmp->ctx=ctx;

	tmp->tune=gcalloc(ctx->sz_tune*sizeof(double));

	if(ctx->sz_vec)
	{
		tmp->vec=gcalloc(ctx->sz_vec*sizeof(double));
		tmp->vel=gcalloc(ctx->sz_vec*sizeof(double));
	}

	if(ctx->sz_bits)
		tmp->bits=gcalloc((ctx->sz_bits+7)/8);

	if(ctx->sz_prog)
		tmp->prog=gcalloc(ctx->sz_prog*sizeof(byte *));

	return(tmp);
}

void bgal_free_default(bgalContext *ctx, bgalMember *obj)
{
	int i;

	if(obj->prog)
	{
		for(i=0; i<ctx->sz_prog; i++)
			if(obj->prog[i])
		{
			gcfree(obj->prog[i]);
			obj->prog[i]=NULL;
		}
	}

	obj->lnext=ctx->free;
	ctx->free=obj;
}

bgalMember *bgal_pick_default(bgalContext *ctx, bgalMember *skip)
{
	bgalMember *cur;
	int i;

	if(!ctx->live)return(NULL);

	while(1)
	{
		i=bgbrng_sqrrand()*ctx->cnt_live;
		cur=ctx->live;
		while(i && cur->lnext)
			{ i--; cur=cur->lnext; }
//		if(cur!=skip)break;
		break;
	}

	return(cur);
}

int bgal_noisebytemask(float m)
{
	float f;
	int i, j;

	j=0;
	for(i=0; i<8; i++)
	{
		f=bgbrng_ssqrrand()*m;
		if(f>0.5)j|=1<<i;
	}
	return(j);
}

byte *bgal_breedprogbytes(bgalMember *tmp, byte *pa, byte *pb)
{
	byte buf[256];
	byte *pc;
	float f, g, m;
	int i, j, k;

	g=tmp->tune[3]; m=1.0-(g*g);
	pc=buf;

	if(pa && pb)
	{
		while(*pa && *pb)
		{
			if((pc-buf)>240)break;

#if 1
			f=bgbrng_ssqrrand()*m;
			if(f>0.25) { pa++; pb++; continue; }

			j=bgbrng_genvalue_fast()&255;
			j=((*pa)&j) | ((*pb)&(~j));
			j^=bgal_noisebytemask(m);
			if(!j)j++;
			*pc++=j;

			f=bgbrng_ssqrrand()*m*0.6;
			if(f<0.5)pa++;
			f=bgbrng_ssqrrand()*m*0.6;
			if(f<0.5)pb++;

			f=bgbrng_ssqrrand()*m;
			if(f>0.5)*pc++=bgbrng_genvalue_fast()&255;
#endif

#if 0
			i=bgbrng_genvalue_fast()&15;
			switch(i)
			{
			case 0: case 1: case 2: case 3:
				*pc++=*pa++; pb++; break;
			case 4: case 5: case 6: case 7:
				*pc++=*pb++; pa++; break;
			case 8: case 9: case 10: case 11:
				j=bgbrng_genvalue_fast()&255;
				j=((*pa)&j) | ((*pb)&(~j));
				j^=bgal_noisebytemask(m);
				if(!j)j++;
				*pc++=j;
				pa++; pb++;
				break;

			case 12:
			case 13:
				j=bgbrng_genvalue_fast()&255;
				j=((*pa)&j) | ((*pb)&(~j));
				j^=bgal_noisebytemask(m);
				if(!j)j++;
				*pc++=j;

				f=bgbrng_ssqrrand()*m*0.75;
				if(f<0.5)pa++;
				f=bgbrng_ssqrrand()*m*0.75;
				if(f<0.5)pb++;
				break;

			case 14:
			case 15:
				f=bgbrng_ssqrrand()*m*0.75;
				if(f>0.5)*pc++=bgbrng_genvalue_fast()&255;
				break;
			}
#endif
		}

		if(!(*pa))pa=pb;
	}else if(!pa)pa=pb;

	if(pa)while(*pa)
	{
		if((pc-buf)>240)break;

#if 1
		f=bgbrng_ssqrrand()*m;
		if(f>0.35) { pa++; pb++; continue; }

		j=*pa++;
		j^=bgal_noisebytemask(m);
		if(!j)j++;
		*pc++=j;

		f=bgbrng_ssqrrand()*m;
		if(f>0.5)*pc++=bgbrng_genvalue_fast()&255;
#endif

#if 0
		i=bgbrng_genvalue_fast()&7;
		switch(i)
		{
		case 0: case 1: case 2: case 3: *pc++=*pa++; break;
		case 4: case 5:
			j=*pa++;
			j^=bgal_noisebytemask(m);
			if(!j)j++;
			*pc++=j;
			break;
		case 6:
			f=bgbrng_ssqrrand()*m*0.75;
			if(f>0.5) pa++;
			break;
		case 7:
			f=bgbrng_ssqrrand()*m*0.75;
			if(f>0.5)*pc++=bgbrng_genvalue_fast()&255;
			break;
		}
#endif
	}

	f=bgbrng_ssqrrand()*m*0.75;
	if(f>0.5)*pc++=bgbrng_genvalue_fast()&255;

	*pc++=0;

	buf[15]=0;

	i=strlen(buf);
	pc=gcatomic(i);
	memcpy(pc, buf, i);

	return(pc);
}

bgalMember *bgal_breed_default(bgalContext *ctx,
	bgalMember *obja, bgalMember *objb)
{
	bgalMember *tmp;
	float f, g, h, m;
	int i, j, k, n;

	tmp=bgalAllocMember(ctx);

	f=bgbrng_nfrand();
	g=obja->tune[1]*(1-f) + objb->tune[1]*f;
	m=1.0-(g*g);

	for(i=0; i<ctx->sz_tune; i++)
	{
		f=bgbrng_nfrand();
		g=obja->tune[i]*(1-f) + objb->tune[i]*f;

		g+=bgbrng_ssqrrand()*(0.1*m);
		f=bgbrng_nfrand();
		if(f<(0.05*m))g*=bgbrng_ssqrrand()*1.5*m + (1-m);

		if(g<0)g=0; if(g>1)g=1;
		tmp->tune[i]=g;
	}

	g=tmp->tune[0]; m=1.0-(g*g);

	for(i=0; i<ctx->sz_vec; i++)
	{
		f=bgbrng_nfrand();
		g=obja->vel[i]*(1-f) + objb->vel[i]*f;

		g+=bgbrng_ssqrrand()*(0.1*m);

		f=bgbrng_nfrand();
		if(f<(0.05*m))g*=bgbrng_ssqrrand()*1.5*m + (1-m);

		tmp->vel[i]=g;
	}

	for(i=0; i<ctx->sz_vec; i++)
	{
		f=bgbrng_nfrand();
		g=obja->vec[i]*(1-f) + objb->vec[i]*f;

		g+=tmp->vel[i];

		g+=bgbrng_ssqrrand()*(0.1*m);

		f=bgbrng_nfrand();
		if(f<(0.05*m))g*=bgbrng_ssqrrand()*1.5*m + (1-m);

		tmp->vec[i]=g;
	}

	if(tmp->bits)
	{
		g=tmp->tune[2]; m=1.0-(g*g);

		n=(ctx->sz_bits+7)/8;
		for(i=0; i<n; i++)
		{
			j=bgbrng_genvalue_fast()&255;
			j=(obja->bits[i]&j) | (objb->bits[i]&(~j));
			j^=bgal_noisebytemask(m);
			tmp->bits[i]=j;
		}

#if 0
		for(i=0; i<ctx->sz_bits; i++)
		{
			j=obja->bits[i>>3]&(1<<(i&7));
			k=objb->bits[i>>3]&(1<<(i&7));
			f=bgbrng_nfrand();
			g=j*(1-f) + k*f;
			g+=bgbrng_ssqrrand()*m*0.5;
			if(g<0.5)tmp->bits[i>>3]&=~(1<<(i&7));
			if(g>=0.5)tmp->bits[i>>3]|=(1<<(i&7));
		}
#endif
	}

	if(tmp->prog)
	{
		for(i=0; i<ctx->sz_prog; i++)
		{
			tmp->prog[i]=bgal_breedprogbytes(tmp,
				obja->prog[i], objb->prog[i]);
		}
	}

	return(tmp);
}

void bgal_step_test_default(bgalContext *ctx, void *data)
{
	bgalMember *cur;

	cur=ctx->live;
	while(cur)
	{
		cur->weight=bgalTest(cur, data);
		cur=cur->lnext;
	}
}

void bgal_step_select_min_default(bgalContext *ctx)
{
	bgalMember *cur, *cur1, *lst, *nxt, *prv;
	bgalMember *tmp, *tmpa, *tmpb;
	int i, n;

//	return;

	cur=ctx->live; lst=NULL;
	while(cur)
	{
		nxt=cur->lnext;
		cur->lnext=NULL;

		cur1=lst; prv=NULL;
		while(cur1 && (cur1->weight<=cur->weight))
			{ prv=cur1; cur1=cur1->lnext; }

		if(prv)
		{
			cur->lnext=cur1;
			prv->lnext=cur;
		}else
		{
			cur->lnext=lst;
			lst=cur;
		}

		cur=nxt;
	}

	cur=lst; prv=NULL; i=ctx->n_keep; n=0;
	while(cur && i) { n++; i--; prv=cur; cur=cur->lnext; }

	if(cur)
	{
		if(prv)prv->lnext=NULL;

		while(cur)
		{
			nxt=cur->lnext;
			bgalFreeMember(cur);
			cur=nxt;
		}

//		cur1=cur;
//		while(cur1->lnext)cur1=cur1->lnext;
//		cur1->lnext=ctx->free;
//		ctx->free=cur;
	}

	ctx->cnt_live=n;
	ctx->live=lst;
}

void bgal_step_select_max_default(bgalContext *ctx)
{
	bgalMember *cur, *cur1, *lst, *nxt, *prv;
	bgalMember *tmp, *tmpa, *tmpb;
	int i, n;

	cur=ctx->live; lst=NULL;
	while(cur)
	{
		nxt=cur->lnext;
		cur->lnext=NULL;

		cur1=lst; prv=NULL;
		while(cur1 && (cur1->weight>=cur->weight))
			{ prv=cur1; cur1=cur1->lnext; }

		if(prv)
		{
			cur->lnext=cur1;
			prv->lnext=cur;
		}else
		{
			cur->lnext=lst;
			lst=cur;
		}

		cur=nxt;
	}

	cur=lst; prv=NULL; i=ctx->n_keep; n=0;
	while(cur && i) { n++; i--; prv=cur; cur=cur->lnext; }

	if(cur)
	{
		if(prv)prv->lnext=NULL;

		while(cur)
		{
			nxt=cur->lnext;
			bgalFreeMember(cur);
			cur=nxt;
		}

//		cur1=cur;
//		while(cur1->lnext)cur1=cur1->lnext;
//		cur1->lnext=ctx->free;
//		ctx->free=cur;
	}

	ctx->cnt_live=n;
	ctx->live=lst;
}

void bgal_step_breed_default(bgalContext *ctx)
{
	bgalMember *cur, *cur1, *lst, *nxt, *prv;
	bgalMember *tmp, *tmpa, *tmpb;
	int i, n;

	cur=ctx->live; lst=cur;
	while(cur && cur->lnext)
		cur=cur->lnext;

	i=ctx->n_breed; n=ctx->cnt_live;
	while(i)
	{
		tmpa=bgalPickMember(ctx, NULL);
		tmpb=bgalPickMember(ctx, tmpa);
		tmp=bgalBreed(tmpa, tmpb);

		if(cur) { cur->lnext=tmp; cur=tmp; }
			else { cur=tmp; lst=tmp; }
		i--; n++;
	}

	ctx->cnt_live=n;
	ctx->live=lst;
}

void bgal_step_default(bgalContext *ctx)
{
	void *p;
	int i;

	p=NULL;
	if(ctx->n_data)
	{
		i=bgbrng_genvalue_fast()&0xFFFFFF;
		i%=ctx->n_data;
		p=ctx->data[i];
	}

	bgalStepTest(ctx, p);
	bgalStepSelect(ctx);
	bgalStepBreed(ctx);
	bgalStepStatus(ctx);
}

void bgal_setup_default(bgalContext *ctx, void *data)
{
	bgalMember *tmp, *lst;
	int i, j, n;

	lst=ctx->live; n=ctx->cnt_live;
	i=ctx->n_keep + ctx->n_breed - n;
	while(i--)
	{
		tmp=bgalAllocMember(ctx);

		for(j=0; j<ctx->sz_vec; j++)
		{
			tmp->vec[j]=bgbrng_ssqrrand()*10;
			tmp->vel[j]=bgbrng_ssqrrand()*10;
		}

		n++;
		tmp->lnext=lst; lst=tmp;
	}

	ctx->cnt_live=n;
	ctx->live=lst;
}

void bgal_evolve_default(bgalContext *ctx, void *data)
{
	int i;

	bgalSterilize(ctx);
	bgalSetup(ctx, data);

	for(i=0; i<ctx->n_gen; i++)
		bgalStep(ctx);
}


BGBDY_API bgalContext *bgalNewGenCtx(int n_gen, int sz_pop, int n_keep)
{
	bgalContext *ctx;

	BGAL_InterpInit();

	ctx=gctalloc("bgal_context_t", sizeof(bgalContext));

	ctx->n_gen=n_gen;
	ctx->n_keep=n_keep;
	ctx->n_breed=sz_pop-n_keep;

	ctx->alloc_member=bgal_alloc_default;
	ctx->free_member=bgal_free_default;
	ctx->pick_member=bgal_pick_default;
	ctx->breed=bgal_breed_default;

	ctx->step_test=bgal_step_test_default;
	ctx->step_select=bgal_step_select_min_default;
	ctx->step_breed=bgal_step_breed_default;
	ctx->step_status=NULL;
	ctx->step=bgal_step_default;
	ctx->setup=bgal_setup_default;
	ctx->evolve=bgal_evolve_default;

	return(ctx);
}

BGBDY_API bgalContext *bgalNewGenMinCtx(int sz_vec, int n_gen, int sz_pop, int n_keep)
{
	bgalContext *ctx;

	ctx=bgalNewGenCtx(n_gen, sz_pop, n_keep);

	ctx->sz_vec=sz_vec;
	ctx->sz_tune=8;
	ctx->sz_prog=0;

	return(ctx);
}

BGBDY_API bgalContext *bgalNewGenMaxCtx(int sz_vec, int n_gen, int sz_pop, int n_keep)
{
	bgalContext *ctx;

	ctx=bgalNewGenMinCtx(sz_vec, n_gen, sz_pop, n_keep);
	ctx->step_select=bgal_step_select_max_default;

	return(ctx);
}

BGBDY_API bgalContext *bgalNewGenBitsMinCtx(int sz_bits,
	int n_gen, int sz_pop, int n_keep)
{
	bgalContext *ctx;

	ctx=bgalNewGenCtx(n_gen, sz_pop, n_keep);

	ctx->sz_bits=sz_bits;
	ctx->sz_tune=8;
	ctx->sz_prog=0;

	return(ctx);
}

BGBDY_API bgalContext *bgalNewGenBitsMaxCtx(int sz_bits,
	int n_gen, int sz_pop, int n_keep)
{
	bgalContext *ctx;

	ctx=bgalNewGenBitsMinCtx(sz_bits, n_gen, sz_pop, n_keep);
	ctx->step_select=bgal_step_select_max_default;

	return(ctx);
}


BGBDY_API bgalContext *bgalNewGenProgMinCtx(int sz_vec, int sz_bits, int sz_prog,
	int n_gen, int sz_pop, int n_keep)
{
	bgalContext *ctx;

	ctx=bgalNewGenCtx(n_gen, sz_pop, n_keep);

	ctx->sz_vec=sz_vec;
	ctx->sz_bits=sz_bits;
	ctx->sz_prog=sz_prog;
	ctx->sz_tune=8;

	return(ctx);
}

BGBDY_API bgalContext *bgalNewGenProgMaxCtx(int sz_vec, int sz_bits, int sz_prog,
	int n_gen, int sz_pop, int n_keep)
{
	bgalContext *ctx;

	ctx=bgalNewGenProgMinCtx(sz_vec, sz_bits, sz_prog,
		n_gen, sz_pop, n_keep);
	ctx->step_select=bgal_step_select_max_default;

	return(ctx);
}
