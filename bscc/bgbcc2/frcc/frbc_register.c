#include <bgbccc.h>

BGBCC_FrCC_RegisterInfo *BGBCC_FrBC_AllocRegisterInfo(
	BGBCC_FrCC_State *ctx)
{
	BGBCC_FrCC_RegisterInfo *rtmp;

	if(ctx->reginfo_free)
	{
		rtmp=ctx->reginfo_free;
		ctx->reginfo_free=rtmp->next;
		memset(rtmp, 0, sizeof(BGBCC_FrCC_RegisterInfo));
		return(rtmp);
	}
	
	rtmp=bgbcc_malloc(sizeof(BGBCC_FrCC_RegisterInfo));
	return(rtmp);
}

bool BGBCC_FrBC_FreeRegisterInfo(BGBCC_FrCC_State *ctx,
	BGBCC_FrCC_RegisterInfo *rinf)
{
	rinf->next=ctx->reginfo_free;
	ctx->reginfo_free=rinf;
	return(true);
}
	
frbc_status BGBCC_FrBC_RegisterAllocTemporaryLLn(
	BGBCC_FrCC_State *ctx, frbc_type bty,
	frbc_register *rtreg, char *fn, int ln)
{
	BGBCC_FrCC_RegisterInfo *ri;
	frbc_register treg;
	int i;

	if(!ctx->cur_func->regs)
	{
		ctx->cur_func->regs=bgbcc_malloc(
			256*sizeof(BGBCC_FrCC_RegisterInfo *));
		ctx->cur_func->n_regs=0;
		ctx->cur_func->m_regs=256;
	}
	
	if(ctx->cur_func->n_regs>=16)
	{
		printf("RegAllocDbg:\n");
		for(i=0; i<ctx->cur_func->n_regs; i++)
		{
			ri=ctx->cur_func->regs[i];
			if(ri)
			{
				printf("    R%d: %s:%d\n", i,
					ri->alc_fn, ri->alc_ln);
			}
		}
	}
	
	for(i=0; i<256; i++)
	{
		if(!ctx->cur_func->regs[i])
		{
			if(i>=ctx->cur_func->n_regs)
				ctx->cur_func->n_regs=i+1;
		
			ri=BGBCC_FrBC_AllocRegisterInfo(ctx);
			ri->alc_fn=fn;
			ri->alc_ln=ln;

			ri->type=bty;
			ri->ucnt=1;
			ctx->cur_func->regs[i]=ri;
			treg.val=FR2C_REGTY_REG|
				(bty.val<<FR2C_REGID_TYPESHIFT)|
				i;
			*rtreg=treg;
			return(i);
		}
	}

	BGBCC_FrBC_TagError(ctx,
		FR2C_TERR_STATUS(FR2C_STATUS_ERR_CANTACQUIRE));
	return(-1);
}

frbc_status BGBCC_FrBC_RegisterAllocTemporaryInt(
	BGBCC_FrCC_State *ctx, frbc_register *rtreg)
{
	return(BGBCC_FrBC_RegisterAllocTemporary(
		ctx, BGBCC_FrBC_TypeWrapBasicType(FR2C_TY_I), rtreg));
}

frbc_status BGBCC_FrBC_RegisterAllocTemporaryLong(
	BGBCC_FrCC_State *ctx, frbc_register *rtreg)
{
	return(BGBCC_FrBC_RegisterAllocTemporary(
		ctx, BGBCC_FrBC_TypeWrapBasicType(FR2C_TY_L), rtreg));
}

frbc_status BGBCC_FrBC_RegisterAllocTemporaryFloat(
	BGBCC_FrCC_State *ctx, frbc_register *rtreg)
{
	return(BGBCC_FrBC_RegisterAllocTemporary(
		ctx, BGBCC_FrBC_TypeWrapBasicType(FR2C_TY_F), rtreg));
}

frbc_status BGBCC_FrBC_RegisterAllocTemporaryDouble(
	BGBCC_FrCC_State *ctx, frbc_register *rtreg)
{
	return(BGBCC_FrBC_RegisterAllocTemporary(
		ctx, BGBCC_FrBC_TypeWrapBasicType(FR2C_TY_D), rtreg));
}

frbc_status BGBCC_FrBC_RegisterAllocTemporaryPointer(
	BGBCC_FrCC_State *ctx, frbc_register *rtreg)
{
	return(BGBCC_FrBC_RegisterAllocTemporary(
		ctx, BGBCC_FrBC_TypeWrapBasicType(FR2C_TY_P), rtreg));
}

frbc_status BGBCC_FrBC_RegisterCheckRelease(
	BGBCC_FrCC_State *ctx, frbc_register reg)
{
	BGBCC_FrCC_RegisterInfo *ri;
	int i;

	if((reg.val&FR2C_REGTY_REGMASK)==FR2C_REGTY_REG)
	{
		i=(reg.val&FR2C_REGID_REGMASK);
		if((i<0) || (i>=256))
			return(FR2C_STATUS_ERR_BADVALUE);
		ri=ctx->cur_func->regs[i];
		ri->ucnt--;
		if(ri->ucnt>0)
			return(FR2C_STATUS_NO);
		ctx->cur_func->regs[i]=NULL;
		BGBCC_FrBC_FreeRegisterInfo(ctx, ri);
		return(FR2C_STATUS_YES);
	}

	if((reg.val&FR2C_REGTY_REGMASK)==FR2C_REGTY_ARG)
		{ return(FR2C_STATUS_NO); }
	if((reg.val&FR2C_REGTY_REGMASK)==FR2C_REGTY_LOCAL)
		{ return(FR2C_STATUS_NO); }
	if((reg.val&FR2C_REGTY_REGMASK)==FR2C_REGTY_GLOBAL)
		{ return(FR2C_STATUS_NO); }

	if((reg.val&FR2C_REGTY_REGMASK)==FR2C_REGTY_IMM_INT)
		{ return(FR2C_STATUS_NO); }
	if((reg.val&FR2C_REGTY_REGMASK)==FR2C_REGTY_IMM_LONG)
		{ return(FR2C_STATUS_NO); }
	if((reg.val&FR2C_REGTY_REGMASK)==FR2C_REGTY_IMM_FLOAT)
		{ return(FR2C_STATUS_NO); }
	if((reg.val&FR2C_REGTY_REGMASK)==FR2C_REGTY_IMM_DOUBLE)
		{ return(FR2C_STATUS_NO); }
	if((reg.val&FR2C_REGTY_REGMASK)==FR2C_REGTY_IMM_STRING)
		{ return(FR2C_STATUS_NO); }

	BGBCC_FrBC_TagError(ctx,
		FR2C_TERR_STATUS(FR2C_STATUS_ERR_CANTRELEASE));
	return(FR2C_STATUS_ERR_CANTRELEASE);
}

frbc_status BGBCC_FrBC_RegisterCheckAcquire(
	BGBCC_FrCC_State *ctx, frbc_register reg)
{
	BGBCC_FrCC_RegisterInfo *ri;
	int i;

	if((reg.val&FR2C_REGTY_REGMASK)==FR2C_REGTY_REG)
	{
		i=(reg.val&FR2C_REGID_REGMASK);
		if((i<0) || (i>=256))
			return(FR2C_STATUS_ERR_BADVALUE);
		ri=ctx->cur_func->regs[i];
		ri->ucnt++;
		return(FR2C_STATUS_YES);
	}
	
	return(FR2C_STATUS_NO);
//	return(FR2C_STATUS_ERR_CANTACQUIRE);
}
