#include <bs2asm.h>

BS2ASM_ImageLump *BS2ASM_AllocLump(BS2ASM_Context *ctx)
{
	BS2ASM_ImageLump *tmp;
	
	tmp=bs2asm_cmalloc(ctx, sizeof(BS2ASM_ImageLump));
	tmp->ctx=ctx;

	return(tmp);
}

void BS2ASM_ResetLump(BS2ASM_ImageLump *lmp)
{
	if(!lmp->data)
	{
		lmp->data=bs2asm_cmalloc(lmp->ctx, 64);
		lmp->msz_data=64;
	}

	lmp->dp=lmp->data;
}

BGBSVM_API int BS2ASM_CheckEmitLump(BS2ASM_ImageLump *lmp, int sz)
{
	int i, j;

	if(!lmp->data)
	{
		lmp->data=bs2asm_cmalloc(lmp->ctx, 64);
		lmp->dp=lmp->data;
		lmp->msz_data=64;
	}

	if((lmp->dp+16)>=(lmp->data+lmp->msz_data))
	{
		i=lmp->msz_data+(lmp->msz_data>>1);
		j=lmp->dp-lmp->data;
		lmp->data=bs2asm_realloc(lmp->data, i);
		lmp->dp=lmp->data+j;
		lmp->msz_data=i;
	}

	return(0);
}

BGBSVM_API int BS2ASM_EmitLumpOpcode(BS2ASM_ImageLump *lmp, int op)
{
	BS2ASM_CheckEmitLump(lmp, 16);
	lmp->dp=BS2ASM_EncodeOpcode(lmp->dp, op);
	return(0);
}

BGBSVM_API int BS2ASM_EmitLumpVLI(BS2ASM_ImageLump *lmp, s64 val)
{
	BS2ASM_CheckEmitLump(lmp, 16);
	lmp->dp=BS2ASM_EncVLI(lmp->dp, val);
	return(0);
}

BGBSVM_API int BS2ASM_EmitLumpSVLI(BS2ASM_ImageLump *lmp, s64 val)
{
	BS2ASM_CheckEmitLump(lmp, 16);
	lmp->dp=BS2ASM_EncSVLI(lmp->dp, val);
	return(0);
}

BGBSVM_API int BS2ASM_EmitLumpSym(BS2ASM_ImageLump *lmp, char *str)
{
	int i, j;

	BS2ASM_CheckEmitLump(lmp, 16);
	i=BS2ASM_LitIndexSym(lmp->ctx, str);
	lmp->dp=BS2ASM_EncVLI(lmp->dp, i);
	return(0);
}

BGBSVM_API int BS2ASM_EmitLumpString(BS2ASM_ImageLump *lmp, char *str)
{
	int i, j;

	BS2ASM_CheckEmitLump(lmp, 16);
	i=BS2ASM_LitIndexString(lmp->ctx, str);
	lmp->dp=BS2ASM_EncVLI(lmp->dp, i);
	return(0);
}

BGBSVM_API int BS2ASM_EmitLumpUTF8(BS2ASM_ImageLump *lmp, char *str)
{
	int i, j;

	BS2ASM_CheckEmitLump(lmp, 16);
	i=BS2ASM_LitIndexUTF8(lmp->ctx, str);
	lmp->dp=BS2ASM_EncVLI(lmp->dp, i);
	return(0);
}

BGBSVM_API int BS2ASM_EmitLumpNameType(BS2ASM_ImageLump *lmp,
	char *name, char *type)
{
	int i, j;

	BS2ASM_CheckEmitLump(lmp, 16);
	i=BS2ASM_LitIndexNameType(lmp->ctx, name, type);
	lmp->dp=BS2ASM_EncVLI(lmp->dp, i);
	return(0);
}

BGBSVM_API int BS2ASM_EmitLumpEntity(BS2ASM_ImageLump *lmp,
	BS2ASM_TagEntity *ent)
{
	int i, j;

	BS2ASM_CheckEmitLump(lmp, 16);
	i=BS2ASM_LitIndexTagEnt(lmp->ctx, ent);
	lmp->dp=BS2ASM_EncVLI(lmp->dp, i);
	return(0);
}

#if 0
// BGBSVM_API int BS2ASM_EmitLumpClassRef(BS2ASM_ImageLump *lmp,
	char *name)
{
	int i, j;

	BS2ASM_CheckEmitLump(lmp, 16);
	i=BS2ASM_LitIndexClassRef(lmp->ctx, name);
	lmp->dp=BS2ASM_EncVLI(lmp->dp, i);
	return(0);
}
#endif

BGBSVM_API int BS2ASM_EmitLumpInt(BS2ASM_ImageLump *lmp, s64 val)
{
	int i, j;

	BS2ASM_CheckEmitLump(lmp, 16);
	i=BS2ASM_LitIndexInt(lmp->ctx, val);
	lmp->dp=BS2ASM_EncVLI(lmp->dp, i);
	return(0);
}

BGBSVM_API int BS2ASM_EmitLumpLong(BS2ASM_ImageLump *lmp, s64 val)
{
	int i, j;

	BS2ASM_CheckEmitLump(lmp, 16);
	i=BS2ASM_LitIndexLong(lmp->ctx, val);
	lmp->dp=BS2ASM_EncVLI(lmp->dp, i);
	return(0);
}

BGBSVM_API int BS2ASM_EmitLumpFloat(BS2ASM_ImageLump *lmp, double val)
{
	int i, j;

	BS2ASM_CheckEmitLump(lmp, 16);
	i=BS2ASM_LitIndexFloat(lmp->ctx, val);
	lmp->dp=BS2ASM_EncVLI(lmp->dp, i);
	return(0);
}

BGBSVM_API int BS2ASM_EmitLumpDouble(BS2ASM_ImageLump *lmp, double val)
{
	int i, j;

	BS2ASM_CheckEmitLump(lmp, 16);
	i=BS2ASM_LitIndexDouble(lmp->ctx, val);
	lmp->dp=BS2ASM_EncVLI(lmp->dp, i);
	return(0);
}

void BS2ASM_EmitLumpByte(BS2ASM_ImageLump *lmp, int val)
{
	BS2ASM_CheckEmitLump(lmp, 16);
	*lmp->dp++=val;
}

void BS2ASM_EmitLumpBytes(BS2ASM_ImageLump *lmp, byte *buf, int sz)
{
	int i;
	
	for(i=0; i<sz; i++)
		BS2ASM_EmitLumpByte(lmp, buf[i]);
}

BGBSVM_API void BS2ASM_EmitLumpInt16(BS2ASM_ImageLump *lmp, int val)
{
	BS2ASM_CheckEmitLump(lmp, 16);
	lmp->dp=BS2ASM_EncS16(lmp->dp, val);
}

BGBSVM_API void BS2ASM_EmitLumpInt32(BS2ASM_ImageLump *lmp, s32 val)
{
	BS2ASM_CheckEmitLump(lmp, 16);
	lmp->dp=BS2ASM_EncS32(lmp->dp, val);
}

BGBSVM_API void BS2ASM_EmitLumpInt64(BS2ASM_ImageLump *lmp, s64 val)
{
	BS2ASM_CheckEmitLump(lmp, 16);
	lmp->dp=BS2ASM_EncS64(lmp->dp, val);
}

BGBSVM_API void BS2ASM_EmitLumpFloat32(BS2ASM_ImageLump *lmp, f32 val)
{
	BS2ASM_CheckEmitLump(lmp, 16);
	lmp->dp=BS2ASM_EncF32(lmp->dp, val);
}

BGBSVM_API void BS2ASM_EmitLumpFloat64(BS2ASM_ImageLump *lmp, f64 val)
{
	BS2ASM_CheckEmitLump(lmp, 16);
	lmp->dp=BS2ASM_EncF64(lmp->dp, val);
}
