/*
Written 2007-2010 by Brendan G Bohannon
This code is re-assigned into the public domain.
*/

#include <bs2asm.h>

BGBSVM_API BS2ASM_Block *BS2ASM_AllocBlock(BS2ASM_Context *ctx)
{
	BS2ASM_TagEntity *tmp;
	int i;

	tmp=BS2ASM_NewTagEntity(ctx, "Block");
	i=BS2ASM_LitIndexTagEnt(ctx, tmp);
	tmp->litidx=i;
	
	return(tmp);
}

BGBSVM_API void BS2ASM_FinishBlock(
	BS2ASM_Context *ctx, BS2ASM_Block *blk)
{
	BS2ASM_TagEntity *tmp;

	blk->validx=BS2ASM_LitIndexBlob(ctx,
		blk->body, blk->ip-blk->body);
		
	tmp=BS2ASM_CopyList(ctx, blk);
	BS2ASM_SetEntityValueEnt(blk, "labels", tmp);
}

BGBSVM_API int BS2ASM_LookupBlockLabel(BS2ASM_Block *blk, char *name)
{
	int i;

	for(i=0; i<blk->nlbl; i++)
	{
		if(!strcmp(blk->lbl[i]->tagtype, "Label"))
		{
			if(!strcmp(blk->lbl[i]->name, name))
				return(i);
		}
	}
	return(-1);
}

BGBSVM_API void BS2ASM_CheckExpandBlockLabel(BS2ASM_Block *blk)
{
	int i;

	if(!blk->lbl)
	{
		i=16;
		while(i<=blk->nlbl)i=i+(i>>1);
		blk->lbl=bs2asm_cmalloc(blk->ctx,
			i*sizeof(BS2ASM_TagEntity *));
		blk->mlbl=i;
	}

	if((blk->nlbl+1)>=blk->mlbl)
	{
		i=blk->mlbl+(blk->mlbl>>1);
		blk->lbl=bs2asm_realloc(blk->lbl,
			i*sizeof(BS2ASM_TagEntity *));
		blk->mlbl=i;
	}
}

BGBSVM_API int BS2ASM_AddBlockLabel(BS2ASM_Block *blk, char *name)
{
	int i;

	BS2ASM_CheckExpandBlockLabel(blk);

	i=blk->nlbl++;
//	blk->lbl[i]=bs2asm_cmalloc(blk->ctx, sizeof(BS2ASM_TagEntityLabel));
	blk->lbl[i]=BS2ASM_NewTagEntity(blk->ctx, "Label");
	blk->lbl[i]->name=bs2asm_strdup(name);
	blk->lbl[i]->offs=blk->ip - blk->body;
	
	BS2ASM_LitIndexTagEnt(blk->ctx, blk->lbl[i]);

	return(i);
}

BGBSVM_API int BS2ASM_AddBlockReloc(BS2ASM_Block *blk, char *name)
{
	int i;

	BS2ASM_CheckExpandBlockLabel(blk);

	i=blk->nlbl++;
//	blk->lbl[i]=bs2asm_cmalloc(blk->ctx, sizeof(BS2ASM_TagEntityLabel));
	blk->lbl[i]=BS2ASM_NewTagEntity(blk->ctx, "Reloc");
	blk->lbl[i]->name=bs2asm_strdup(name);
	blk->lbl[i]->offs=blk->ip - blk->body;
	
	BS2ASM_LitIndexTagEnt(blk->ctx, blk->lbl[i]);

	return(i);
}

BGBSVM_API int BS2ASM_CheckEmitBlock(BS2ASM_Block *blk)
{
	int i, j;

	if(!blk->body)
	{
		blk->body=bs2asm_cmalloc(blk->ctx, 64);
		blk->ip=blk->body;
		blk->maxbody=64;
	}

	if((blk->ip+16)>=(blk->body+blk->maxbody))
	{
		i=blk->maxbody+(blk->maxbody>>1);
		j=blk->ip-blk->body;
		blk->body=bs2asm_realloc(blk->body, i);
		blk->ip=blk->body+j;
		blk->maxbody=i;
	}

	return(0);
}

BGBSVM_API int BS2ASM_EmitBlockOpcode(BS2ASM_Block *blk, int op)
{
	BS2ASM_CheckEmitBlock(blk);
	blk->ip=BS2ASM_EncodeOpcode(blk->ip, op);
	return(0);
}

BGBSVM_API int BS2ASM_EmitBlockOpcodeName(BS2ASM_Block *blk, char *op)
{
	int i;

	i=BS2ASM_LookupOpcode2(op);
	if(i<0)return(-1);
	BS2ASM_EmitBlockOpcode(blk, i);
	return(0);
}

BGBSVM_API int BS2ASM_EmitBlockVLI(BS2ASM_Block *blk, s64 val)
{
	BS2ASM_CheckEmitBlock(blk);
	blk->ip=BS2ASM_EncVLI(blk->ip, val);
	return(0);
}

BGBSVM_API int BS2ASM_EmitBlockSVLI(BS2ASM_Block *blk, s64 val)
{
	BS2ASM_CheckEmitBlock(blk);
	blk->ip=BS2ASM_EncSVLI(blk->ip, val);
	return(0);
}

BGBSVM_API int BS2ASM_EmitBlockSym(BS2ASM_Block *blk, char *str)
{
	int i, j;

	BS2ASM_CheckEmitBlock(blk);
	i=BS2ASM_LitIndexSym(blk->ctx, str);
	blk->ip=BS2ASM_EncVLI(blk->ip, i);
	return(0);
}

BGBSVM_API int BS2ASM_EmitBlockString(BS2ASM_Block *blk, char *str)
{
	int i, j;

	BS2ASM_CheckEmitBlock(blk);
	i=BS2ASM_LitIndexString(blk->ctx, str);
	blk->ip=BS2ASM_EncVLI(blk->ip, i);
	return(0);
}

BGBSVM_API int BS2ASM_EmitBlockInt(BS2ASM_Block *blk, s64 val)
{
	int i, j;

	BS2ASM_CheckEmitBlock(blk);
	i=BS2ASM_LitIndexInt(blk->ctx, val);
	blk->ip=BS2ASM_EncVLI(blk->ip, i);
	return(0);
}

BGBSVM_API int BS2ASM_EmitBlockLong(BS2ASM_Block *blk, s64 val)
{
	int i, j;

	BS2ASM_CheckEmitBlock(blk);
	i=BS2ASM_LitIndexLong(blk->ctx, val);
	blk->ip=BS2ASM_EncVLI(blk->ip, i);
	return(0);
}

BGBSVM_API int BS2ASM_EmitBlockFloat(BS2ASM_Block *blk, double val)
{
	int i, j;

	BS2ASM_CheckEmitBlock(blk);
	i=BS2ASM_LitIndexFloat(blk->ctx, val);
	blk->ip=BS2ASM_EncVLI(blk->ip, i);
	return(0);
}

BGBSVM_API int BS2ASM_EmitBlockDouble(BS2ASM_Block *blk, double val)
{
	int i, j;

	BS2ASM_CheckEmitBlock(blk);
	i=BS2ASM_LitIndexDouble(blk->ctx, val);
	blk->ip=BS2ASM_EncVLI(blk->ip, i);
	return(0);
}

BGBSVM_API int BS2ASM_EmitBlockJmpAddr(BS2ASM_Block *blk, char *str)
{
	int i, j;

	BS2ASM_CheckEmitBlock(blk);
	
	blk->ip=BS2ASM_EncS16(blk->ip, 0);
	BS2ASM_AddBlockReloc(blk, str);
//	i=BS2ASM_LitIndexSym(blk->ctx, str);
//	blk->ip=BS2ASM_EncVLI(blk->ip, i);
	return(0);
}
