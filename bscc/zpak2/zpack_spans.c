#include <zpack.h>

int ZPACK_AllocSpansNode(ZPACK_Context *ctx)
{
	ZPACK_SpansNode *tn;
	int tmp;
	int i, j;

	if(!ctx->span)
	{
		ctx->n_spans=65536;
//		ctx->n_spans=4096;
//		ctx->n_spans=256;
		ctx->spans_free=0;

		ctx->span=malloc(ctx->n_spans*sizeof(ZPACK_SpansNode));
		for(i=(ctx->n_spans-1); i>0; i--)
		{
			ctx->span[i].lnode=-1;
			ctx->span[i].rnode=-1;
			ctx->span[i].offs=-1;
			ctx->span[i].size=ctx->spans_free;
			ctx->spans_free=i;
		}
	}

	if(!ctx->spans_free)
	{
		//all spans are used, so expand spans table by 50%
		j=ctx->n_spans;
		ctx->n_spans=j+(j>>1);
//		ctx->span=realloc(ctx->span,
//			ctx->n_spans*sizeof(ZPACK_SpansNode));

		tn=malloc(ctx->n_spans*sizeof(ZPACK_SpansNode));
		memset(tn, 0xFF, ctx->n_spans*sizeof(ZPACK_SpansNode));
		memcpy(tn, ctx->span, j*sizeof(ZPACK_SpansNode));
		free(ctx->span);
		ctx->span=tn;

		for(i=(ctx->n_spans-1); i>=j; i--)
		{
			ctx->span[i].lnode=-1;
			ctx->span[i].rnode=-1;
			ctx->span[i].offs=-1;
			ctx->span[i].size=ctx->spans_free;
			ctx->spans_free=i;
		}

		printf("Expand: %d->%d\n", j, ctx->n_spans);
	}

	tmp=ctx->spans_free;

	if((tmp<=0) || (tmp>=ctx->n_spans))
	{
		printf("ZPACK_AllocSpansNode: Range sanity failed\n");
		*(int *)-1=-1;
	}

	if((ctx->span[tmp].lnode!=-1) || (ctx->span[tmp].rnode!=-1))
	{
		printf("ZPACK_AllocSpansNode: Purity sanity failed\n");
		*(int *)-1=-1;
	}

	ctx->spans_free=ctx->span[tmp].size;

	memset(&(ctx->span[tmp]), 0, sizeof(ZPACK_SpansNode));
	return(tmp);
}

void ZPACK_FreeSpansNode(ZPACK_Context *ctx, int node)
{
	if((node<=0) || (node>=ctx->n_spans))
	{
		printf("ZPACK_FreeSpansNode: Range sanity failed\n");
		*(int *)-1=-1;
	}

	ctx->span[node].lnode=-1;
	ctx->span[node].rnode=-1;
	ctx->span[node].offs=-1;
	ctx->span[node].size=ctx->spans_free;
	ctx->spans_free=node;
}


int ZPACK_LookupSpan_R(ZPACK_Context *ctx, int node, u64 offs, u32 *sz)
{
	int i;

	if(!ctx->span[node].lnode)
	{
		if(ctx->span[node].offs==offs)
		{
			*sz=ctx->span[node].size;
			return(1);
		}
		return(0);
	}

	if(offs<ctx->span[node].offs)
		return(ZPACK_LookupSpan_R(ctx, ctx->span[node].lnode,
			offs, sz));
	return(ZPACK_LookupSpan_R(ctx, ctx->span[node].rnode, offs, sz));
}

int ZPACK_LookupSpan(ZPACK_Context *ctx, u64 offs, u32 *sz)
{
	if(!ctx->spans_root)return(0);
	return(ZPACK_LookupSpan_R(ctx, ctx->spans_root, offs, sz));
}


u64 ZPACK_SpanNodeGetMin(ZPACK_Context *ctx, int node)
{
	if(ctx->span[node].lnode)
		return(ZPACK_SpanNodeGetMin(ctx, ctx->span[node].lnode));
	return(ctx->span[node].offs);
}

u64 ZPACK_SpanNodeGetMax(ZPACK_Context *ctx, int node)
{
	if(ctx->span[node].lnode)
		return(ZPACK_SpanNodeGetMax(ctx, ctx->span[node].rnode));
	return(ctx->span[node].offs);
}

int ZPACK_SpanNodeGetH(ZPACK_Context *ctx, int node)
	{ return(ctx->span[node].lnode?ctx->span[node].size:0); }

int ZPACK_SpanNodeCalcH(ZPACK_Context *ctx, int node)
{
	int i, j;

	if(!ctx->span[node].lnode)return(0);
	i=ZPACK_SpanNodeGetH(ctx, ctx->span[node].lnode);
	j=ZPACK_SpanNodeGetH(ctx, ctx->span[node].rnode);
	ctx->span[node].size=((i>j)?i:j)+1;
	return(ctx->span[node].size);
}

u64 ZPACK_SpanNodeCalcMid(ZPACK_Context *ctx, int node)
{
	int i, j;

	if(!ctx->span[node].lnode)return(0);

	ctx->span[node].offs=ZPACK_SpanNodeGetMin(ctx,
		ctx->span[node].rnode);

	i=ZPACK_SpanNodeGetMax(ctx, ctx->span[node].lnode);

	if(i>=ctx->span[node].offs)
	{
		printf("ZPACK_SpanNodeCalcMid: Overlap sanity failed\n");
		*(int *)-1=-1;
	}

	return(ctx->span[node].offs);
}

void ZPACK_BalanceSpansNode(ZPACK_Context *ctx, int node)
{
	int tnode;
	int i, j, k;

//	return(0);

	i=ZPACK_SpanNodeCalcH(ctx, ctx->span[node].lnode);
	j=ZPACK_SpanNodeCalcH(ctx, ctx->span[node].rnode);

	if(i>(j+1))
	{
		tnode=ctx->span[ctx->span[node].lnode].lnode;
		k=ctx->span[ctx->span[node].lnode].offs;

		ctx->span[ctx->span[node].lnode].lnode=
			ctx->span[ctx->span[node].lnode].rnode;
		ctx->span[ctx->span[node].lnode].rnode=ctx->span[node].rnode;
		ctx->span[ctx->span[node].lnode].offs=ctx->span[node].offs;

		ctx->span[node].rnode=ctx->span[node].lnode;
		ctx->span[node].lnode=tnode;
		ctx->span[node].offs=k;

#if 1
		ZPACK_SpanNodeCalcMid(ctx, ctx->span[node].lnode);
		ZPACK_SpanNodeCalcMid(ctx, ctx->span[node].rnode);
#endif

		ZPACK_SpanNodeCalcH(ctx, ctx->span[node].lnode);
		ZPACK_SpanNodeCalcH(ctx, ctx->span[node].rnode);
	}

	if(j>(i+1))
	{
		tnode=ctx->span[ctx->span[node].rnode].rnode;
		k=ctx->span[ctx->span[node].rnode].offs;

		ctx->span[ctx->span[node].rnode].rnode=
			ctx->span[ctx->span[node].rnode].lnode;
		ctx->span[ctx->span[node].rnode].lnode=ctx->span[node].lnode;
		ctx->span[ctx->span[node].rnode].offs=ctx->span[node].offs;

		ctx->span[node].lnode=ctx->span[node].rnode;
		ctx->span[node].rnode=tnode;
		ctx->span[node].offs=k;

#if 1
		ZPACK_SpanNodeCalcMid(ctx, ctx->span[node].lnode);
		ZPACK_SpanNodeCalcMid(ctx, ctx->span[node].rnode);
#endif

		ZPACK_SpanNodeCalcH(ctx, ctx->span[node].lnode);
		ZPACK_SpanNodeCalcH(ctx, ctx->span[node].rnode);
	}

	ZPACK_SpanNodeCalcH(ctx, node);
	ZPACK_SpanNodeCalcMid(ctx, node);
}

void ZPACK_InsertSpan_R(ZPACK_Context *ctx, int node, u64 roffs,
	u64 offs, u32 size, char *name)
{
	if(!ctx->span[node].lnode)
	{
		//hit a leaf

		if(ctx->span[node].offs==offs)
		{
			//same node, so update
			if(roffs && ((offs+size)>roffs))
			{
				printf("ZPACK_InsertSpan_R: Bad insert "
					"(overlap next node)\n");
				*(int *)-1=-1;
			}

			ctx->span[node].size=size;
			ctx->span[node].name=name;

			return;
		}

		if((offs>=ctx->span[node].offs) &&
			(offs<(ctx->span[node].offs+ctx->span[node].size)))
		{
			printf("ZPACK_InsertSpan_R: Bad insert 2 "
				"(overlap prev node %d)\n",
				(ctx->span[node].offs+ctx->span[node].size)-offs);
			*(int *)-1=-1;
		}

		//different node, so split
		ctx->span[node].lnode=ZPACK_AllocSpansNode(ctx);
		ctx->span[node].rnode=ZPACK_AllocSpansNode(ctx);

		if(offs<ctx->span[node].offs)
		{
			printf("ZPACK_InsertSpan_R: Left Insert\n");
			*(int *)-1=-1;

			ctx->span[ctx->span[node].lnode].offs=offs;
			ctx->span[ctx->span[node].lnode].size=size;
			ctx->span[ctx->span[node].lnode].name=name;
			ctx->span[ctx->span[node].rnode].offs=
				ctx->span[node].offs;
			ctx->span[ctx->span[node].rnode].size=
				ctx->span[node].size;
			ctx->span[ctx->span[node].rnode].name=
				ctx->span[node].name;
			ctx->span[node].size=1;
		}else
		{
			ctx->span[ctx->span[node].lnode].offs=
				ctx->span[node].offs;
			ctx->span[ctx->span[node].lnode].size=
				ctx->span[node].size;
			ctx->span[ctx->span[node].lnode].name=
				ctx->span[node].name;
			ctx->span[ctx->span[node].rnode].offs=offs;
			ctx->span[ctx->span[node].rnode].size=size;
			ctx->span[ctx->span[node].rnode].name=name;
			ctx->span[node].offs=offs;
			ctx->span[node].size=1;
		}
		return;
	}

	if(offs<ctx->span[node].offs)
	{
		ZPACK_InsertSpan_R(ctx, ctx->span[node].lnode,
			ctx->span[node].offs, offs, size, name);
	}else
	{
		ZPACK_InsertSpan_R(ctx, ctx->span[node].rnode, roffs,
			offs, size, name);
	}
	ZPACK_BalanceSpansNode(ctx, node);
}

void ZPACK_InsertSpan(ZPACK_Context *ctx, u64 offs, u32 size, char *name)
{
	if(!size)
	{
		//0 length spans don't exist in the image...

		if(!offs)return;		//0,0 is special (no data)

		//the span is being resized, to nothing...
		ZPACK_DeleteSpan(ctx, offs);
		return;
	}

	if(!ctx->spans_root)
	{
		ctx->spans_root=ZPACK_AllocSpansNode(ctx);
		ctx->span[ctx->spans_root].offs=offs;
		ctx->span[ctx->spans_root].size=size;
		ctx->span[ctx->spans_root].name=name;
		return;
	}
	ZPACK_InsertSpan_R(ctx, ctx->spans_root, 0, offs, size, name);
}

int ZPACK_DeleteSpan_R(ZPACK_Context *ctx, int node, u64 offs)
{
	int tnode;
	int i;

	if(!ctx->span[node].lnode)return(0);

	if(!ctx->span[ctx->span[node].lnode].lnode &&
		(ctx->span[ctx->span[node].lnode].offs==offs))
	{
		ZPACK_FreeSpansNode(ctx, ctx->span[node].lnode);
		tnode=ctx->span[node].rnode;
		ctx->span[node].lnode=ctx->span[tnode].lnode;
		ctx->span[node].rnode=ctx->span[tnode].rnode;
		ctx->span[node].offs=ctx->span[tnode].offs;
		ctx->span[node].size=ctx->span[tnode].size;
		ctx->span[node].name=ctx->span[tnode].name;
		ZPACK_FreeSpansNode(ctx, tnode);
		return(1);
	}

	if(!ctx->span[ctx->span[node].rnode].lnode &&
		(ctx->span[ctx->span[node].rnode].offs==offs))
	{
		ZPACK_FreeSpansNode(ctx, ctx->span[node].rnode);
		tnode=ctx->span[node].lnode;
		ctx->span[node].lnode=ctx->span[tnode].lnode;
		ctx->span[node].rnode=ctx->span[tnode].rnode;
		ctx->span[node].offs=ctx->span[tnode].offs;
		ctx->span[node].size=ctx->span[tnode].size;
		ctx->span[node].name=ctx->span[tnode].name;
		ZPACK_FreeSpansNode(ctx, tnode);
		return(1);
	}

	if(offs<ctx->span[node].offs)
	{
		i=ZPACK_DeleteSpan_R(ctx, ctx->span[node].lnode, offs);
	}else{
		i=ZPACK_DeleteSpan_R(ctx, ctx->span[node].rnode, offs);
		ctx->span[node].offs=ZPACK_SpanNodeGetMin(ctx,
			ctx->span[node].rnode);
	}
	ZPACK_BalanceSpansNode(ctx, node);
	return(i);
}

int ZPACK_DeleteSpan(ZPACK_Context *ctx, u64 offs)
{
	int i;

//	return(0);

	if(!offs)return(0);
	if(!ctx->spans_root)return(0);
	if(!ctx->span[ctx->spans_root].lnode &&
		(ctx->span[ctx->spans_root].offs==offs))
	{
		ZPACK_FreeSpansNode(ctx, ctx->spans_root);
		ctx->spans_root=0;
		return(1);
	}
	i=ZPACK_DeleteSpan_R(ctx, ctx->spans_root, offs);
	return(i);
}

//find the next span, roffs is the first span following this node
u64 ZPACK_NextSpan_R(ZPACK_Context *ctx, int node, u64 offs, u64 roffs)
{
	int i;

	if(!ctx->span[node].lnode)
	{
		if(offs<ctx->span[node].offs)	//inexact match before node
		{
			printf("ZPACK_NextSpan_R: Prior Node\n");
			*(int *)-1=-1;

			return(ctx->span[node].offs);
		}

#if 1
		if(offs!=ctx->span[node].offs)
		{
			printf("ZPACK_NextSpan_R: Missing Node\n");
			*(int *)-1=-1;
		}
#endif

		return(roffs);
	}

	if(offs<ctx->span[node].offs)
		return(ZPACK_NextSpan_R(ctx, ctx->span[node].lnode,
			offs, ctx->span[node].offs));
	return(ZPACK_NextSpan_R(ctx, ctx->span[node].rnode, offs, roffs));
}

//find the next span, 0, in this case, means this was the last span
int ZPACK_NextSpan(ZPACK_Context *ctx, u64 offs)
{
	if(!ctx->spans_root)return(0);
	return(ZPACK_NextSpan_R(ctx, ctx->spans_root, offs, 0));
}

int ZPACK_PrevSpan_R(ZPACK_Context *ctx, int node, u64 offs)
{
	int i;

	if(!ctx->span[node].lnode)
		return(ctx->span[node].offs);

	if(offs<=ctx->span[node].offs)
		return(ZPACK_PrevSpan_R(ctx, ctx->span[node].lnode,
			offs));
	return(ZPACK_PrevSpan_R(ctx, ctx->span[node].rnode, offs));
}

//find the next span, 0, in this case, means this was the last span
int ZPACK_PrevSpan(ZPACK_Context *ctx, u64 offs)
{
	if(!ctx->spans_root)return(0);
	return(ZPACK_PrevSpan_R(ctx, ctx->spans_root, offs));
}

int ZPACK_PrevSpanEnd_R(ZPACK_Context *ctx, int node, u64 offs)
{
	int i;

	if(!ctx->span[node].lnode)
		return(ctx->span[node].offs+ctx->span[node].size);

	if(offs<=ctx->span[node].offs)
		return(ZPACK_PrevSpan_R(ctx, ctx->span[node].lnode,
			offs));
	return(ZPACK_PrevSpan_R(ctx, ctx->span[node].rnode, offs));
}

int ZPACK_PrevSpanEnd(ZPACK_Context *ctx, u64 offs)
{
	if(!ctx->spans_root)return(0);
	if(!offs)return(0);
	return(ZPACK_PrevSpanEnd_R(ctx, ctx->spans_root, offs));
}

int ZPACK_FinalSpanEnd(ZPACK_Context *ctx)
{
	int node;

	if(!ctx->spans_root)return(0);

	node=ctx->spans_root;
	while(ctx->span[node].lnode)
		node=ctx->span[node].rnode;

	return(ctx->span[node].offs+ctx->span[node].size);
}

int ZPACK_CheckSpan(ZPACK_Context *ctx, u64 offs, u32 size)
{
	u64 nxt;

	if(!offs)return(size?0:1);

	nxt=ZPACK_NextSpan(ctx, offs);
	if(!nxt)return(1);
	if((offs+size)<=nxt)return(1);
	return(0);
}

int ZPACK_CheckPadSpan(ZPACK_Context *ctx, u64 offs, u32 size)
{
	u64 nxt;

	if(!offs)return(size?0:1);

	nxt=ZPACK_NextSpan(ctx, offs);
	if(!nxt)return(size);
	if((offs+size)<=nxt)return(size);
	return(nxt-offs);
}


void ZPACK_FindMinSpan_R(ZPACK_Context *ctx, int node, u32 size,
	u64 *offs, u32 *sz, u64 roffs)
{
	int i;

	if(!ctx->span[node].lnode)
	{
		if(!roffs)
		{
			//end of image, check if nothing better was found
			if(!(*sz))
			{
				//no good spaces left, so allocate after
				//last span
				*offs=ctx->span[node].offs+
					ctx->span[node].size;
				*sz=size;
			}
			return;
		}

		i=roffs-(ctx->span[node].offs+ctx->span[node].size);
		if(roffs && (i<0))
		{
			printf("ZPACK_FindMinSpan_R: Sequence sanity fail\n");
			*(int *)-1=-1;
		}

		if((i>=size) && (!(*sz) || (i<(*sz))))
		{
			*offs=ctx->span[node].offs+ctx->span[node].size;
			*sz=i;
		}
		return;
	}

	ZPACK_FindMinSpan_R(ctx, ctx->span[node].lnode, size,
		offs, sz, ctx->span[node].offs);
	ZPACK_FindMinSpan_R(ctx, ctx->span[node].rnode, size,
		offs, sz, roffs);
}

//find the smallest span at least size bytes
u64 ZPACK_FindMinSpan(ZPACK_Context *ctx, u32 size)
{
	u64 bo;
	u32 bsz;

	if(!size)return(0);	//0 length spans get 0 for an offset

	if(!ctx->spans_root)
	{
		return(4);
	}

	bo=0; bsz=0;
	ZPACK_FindMinSpan_R(ctx, ctx->spans_root, size, &bo, &bsz, 0);
	return(bo);
}


void ZPACK_PrintFreeSpans_R(ZPACK_Context *ctx, int node, u64 roffs)
{
	int i, j;

	if(!ctx->span[node].lnode)
	{
		i=ctx->span[node].offs+ctx->span[node].size;
//		printf("%08X..%08X (%d bytes)\t\t%s\n",
//			(u32)ctx->span[node].offs, i-1, ctx->span[node].size,
//			ctx->span[node].name?ctx->span[node].name:"");

		if(!roffs)
			return;

		i=ctx->span[node].offs+ctx->span[node].size;
		j=roffs-i;

		if(!j)return;

		printf("Free: %08X..%08X (%d bytes)\n", i, i+j-1, j);

		return;
	}

	ZPACK_PrintFreeSpans_R(ctx, ctx->span[node].lnode,
		ctx->span[node].offs);
	ZPACK_PrintFreeSpans_R(ctx, ctx->span[node].rnode, roffs);
}

ZPACK_API void ZPACK_PrintFreeSpans(ZPACK_Context *ctx)
{
	ZPACK_PrintFreeSpans_R(ctx, ctx->spans_root, 0);
}

void ZPACK_PrintSpans_R(ZPACK_Context *ctx, int node, u64 roffs)
{
	int i, j;

	if(!ctx->span[node].lnode)
	{
		i=ctx->span[node].offs+ctx->span[node].size;
		printf("%08X..%08X (%d bytes)\t\t%s\n",
			(u32)ctx->span[node].offs, i-1, ctx->span[node].size,
			ctx->span[node].name?ctx->span[node].name:"");

		if(!roffs)
			return;

		i=ctx->span[node].offs+ctx->span[node].size;
		j=roffs-i;

		if(!j)return;

		printf("Free: %08X..%08X (%d bytes)\n", i, i+j-1, j);

		return;
	}

	ZPACK_PrintSpans_R(ctx, ctx->span[node].lnode,
		ctx->span[node].offs);
	ZPACK_PrintSpans_R(ctx, ctx->span[node].rnode, roffs);
}

ZPACK_API void ZPACK_PrintSpans(ZPACK_Context *ctx)
{
	ZPACK_PrintSpans_R(ctx, ctx->spans_root, 0);
}
