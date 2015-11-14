#include <bgbccc.h>

void BGBCC_FrBC_CheckExpandGlobals(
	BGBCC_FrCC_State *ctx)
{
}

BGBCC_FrCC_RegisterInfo *BGBCC_FrBC_LookupGlobal(
	BGBCC_FrCC_State *ctx, char *name)
{
	BGBCC_FrCC_RegisterInfo *cur;
	int i;
	
	for(i=0; i<ctx->n_reg_globals; i++)
	{
		cur=ctx->reg_globals[i];
		if(!cur)
			continue;
		if(!strcmp(cur->name, name))
			return(cur);
	}
	return(NULL);
}

BGBCC_FrCC_RegisterInfo *BGBCC_FrBC_GetGlobal(
	BGBCC_FrCC_State *ctx, char *name)
{
	BGBCC_FrCC_RegisterInfo *cur;
	int i;
	
	cur=BGBCC_FrBC_LookupGlobal(ctx, name);
	if(cur)return(cur);
	
	BGBCC_FrBC_CheckExpandGlobals(ctx);
	
	cur=bgbcc_malloc(sizeof(BGBCC_FrCC_RegisterInfo));
	cur->name=bgbcc_strdup(name);
	
	i=ctx->n_reg_globals++;
	ctx->reg_globals[i]=cur;
	cur->regid=i;
	
	return(cur);
}

frbc_status BGBCC_FrBC_EmitLoadGlobal(
	BGBCC_FrCC_State *ctx, frbc_type type,
	frbc_register dst, frbc_register src)
{
	int z, y, w, g, rt;

	w=BGBCC_FrBC_GetRegOprModeW(ctx, dst, type);
	g=BGBCC_FrBC_GetRegID(ctx, src);

	if(BGBCC_FrBC_IsRegBaseP(ctx, dst) &&
		(w>=0) && (g<65536))
	{
		BGBCC_FrBC_EmitOpcode(ctx, FR2C_OP_LDGVAR);
		BGBCC_FrBC_EmitByte(ctx, (w<<4)|
			BGBCC_FrBC_GetRegID(ctx, dst));
		BGBCC_FrBC_EmitWord(ctx, g);
		return(FR2C_STATUS_YES);
	}

	BGBCC_FrBC_TagError(ctx,
		FR2C_TERR_STATUS(FR2C_STATUS_ERR_BADOPARGS));
	return(FR2C_STATUS_ERR_BADOPARGS);
}

frbc_status BGBCC_FrBC_EmitStoreGlobal(
	BGBCC_FrCC_State *ctx, frbc_type type,
	frbc_register dst, frbc_register src)
{
	frbc_register treg;
	int z, y, w, g, rt;

	w=BGBCC_FrBC_GetRegOprModeW(ctx, src, type);
	g=BGBCC_FrBC_GetRegID(ctx, dst);

	if(BGBCC_FrBC_IsRegBaseP(ctx, src) &&
		(w>=0) && (g<65536))
	{
		BGBCC_FrBC_EmitOpcode(ctx, FR2C_OP_STGVAR);
		BGBCC_FrBC_EmitByte(ctx, (w<<4)|
			BGBCC_FrBC_GetRegID(ctx, src));
		BGBCC_FrBC_EmitWord(ctx, g);
		return(FR2C_STATUS_YES);
	}

	if(BGBCC_FrBC_IsRegGlobalP(ctx, src) ||
		BGBCC_FrBC_IsRegImmP(ctx, src))
	{
		BGBCC_FrBC_RegisterAllocTemporary(ctx, type, &treg);
		BGBCC_FrBC_EmitMov(ctx, type, treg, src);
		rt=BGBCC_FrBC_EmitStoreGlobal(ctx, type, dst, treg);
		BGBCC_FrBC_RegisterCheckRelease(ctx, treg);
		if(rt>=0)return(rt);
	}

	BGBCC_FrBC_TagError(ctx,
		FR2C_TERR_STATUS(FR2C_STATUS_ERR_BADOPARGS));
	return(FR2C_STATUS_ERR_BADOPARGS);
}
