#include <bgbfrir2.h>

FR2_RunFrame *BGBFR2_EnvCtx_AllocFrame(FR2_EnvCtx *ctx)
{
	FR2_RunFrame *tmp;

	if(ctx->slab_frame)
	{
		tmp=ctx->slab_frame;
		ctx->slab_frame=tmp->next;
		return(tmp);
	}
	
	tmp=gctalloc("bgbfr2_runframe_t", sizeof(FR2_RunFrame));
	tmp->env=ctx;
	return(tmp);
}

void BGBFR2_EnvCtx_FreeFrame(FR2_EnvCtx *ctx, FR2_RunFrame *frm)
{
	frm->next=ctx->slab_frame;
	ctx->slab_frame=frm;
}

FR2_EnvCtx *BGBFR2_EnvCtx_AllocContext(void)
{
	FR2_EnvCtx *tmp;
	
	tmp=gctalloc("bgbfr2_envctx_t", sizeof(FR2_EnvCtx));
	return(tmp);
}

int BGBFR2_EnvCtx_LinkObject(FR2_EnvCtx *ctx, FR2_ObjectModule *obj)
{
	FR2_Function *fcur;
	FR2_Value *treg;
	int i, j, k, n;

	if(obj->envctx)
	{
		if(obj->envctx==ctx)
		{
			printf("BGBFR2_EnvCtx_LinkObject: "
				"Warning: Multiple Link Attempts\n");
			return(0);
		}
	
		printf("BGBFR2_EnvCtx_LinkObject: Error: "
			"Object Already Linked To Another Context\n");
		return(-1);
	}

	if(!ctx->objs)
	{
		n=16;
		ctx->objs=gcalloc(n*sizeof(FR2_ObjectModule *));
		ctx->m_objs=n;
	}
	
	if((ctx->n_objs+1)>=ctx->m_objs)
	{
		n=ctx->m_objs; n=n+(n>>1);
		ctx->objs=gcrealloc(ctx->objs, n*sizeof(FR2_ObjectModule *));
		ctx->m_objs=n;
	}

	if(!ctx->svar)
	{
		n=16;
		ctx->svar=gcalloc(n*sizeof(FR2_StaticVarInfo *));
		ctx->m_svar=n;
	}
	
	i=ctx->n_objs++;
	ctx->objs[i]=obj;
	obj->envctx=ctx;
	
	for(i=0; i<obj->n_svar; i++)
	{
		for(j=0; j<ctx->n_svar; j++)
		{
			if(obj->svar[i]==ctx->svar[j])
				break;
			if(!strcmp(obj->svar[i]->name, ctx->svar[j]->name) &&
				!strcmp(obj->svar[i]->sig, ctx->svar[j]->sig))
			{
				gcfree(obj->svar[i]);
				obj->svar[i]=ctx->svar[j];
				break;
			}
		}
		if(j<ctx->n_svar)
			continue;

		if((ctx->n_svar+1)>=ctx->m_svar)
		{
			n=ctx->m_svar; n=n+(n>>1);
			ctx->svar=gcrealloc(ctx->svar, n*sizeof(FR2_StaticVarInfo *));
			ctx->m_svar=n;
		}
		
		j=ctx->n_svar++;
		ctx->svar[j]=obj->svar[i];
	}

	for(i=0; i<obj->n_svar; i++)
	{
		treg=ctx->svar[i]->ptr;
		if(!treg)
		{		
			treg=gcalloc(sizeof(FR2_Value));
			ctx->svar[i]->ptr=treg;
		}
		
		if(ctx->svar[i]->sig[0]=='(')
		{
			fcur=ctx->func;
			while(fcur)
			{
				if(!strcmp(fcur->name, ctx->svar[i]->name) &&
					!strcmp(fcur->sig, ctx->svar[i]->sig))
				{
					treg=ctx->svar[j]->ptr;
					treg->a=fcur;
					break;
				}
				fcur=fcur->evnext;
			}
		}
	}
	
	fcur=obj->func;
	while(fcur)
	{
		fcur->envctx=ctx;
		fcur->evnext=ctx->func;
		ctx->func=ctx;
	
		for(j=0; j<ctx->n_svar; j++)
		{
			if(!strcmp(fcur->name, ctx->svar[j]->name) &&
				!strcmp(fcur->sig, ctx->svar[j]->sig))
			{
				treg=ctx->svar[j]->ptr;
				treg->a=fcur;
				break;
			}
		}
		fcur=fcur->next;
	}

	return(0);
}
