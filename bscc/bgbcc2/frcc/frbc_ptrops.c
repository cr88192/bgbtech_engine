#include <bgbccc.h>

frbc_status BGBCC_FrBC_EmitLoadIndexImm(
	BGBCC_FrCC_State *ctx, frbc_type type, frbc_register dst,
	frbc_register src, int idx)
{
	int y, rt, op;
	int i, j, k;

//	if(BGBCC_FrBC_IsRegImmZeroP(ctx, srcb))
//	{
//		rt=BGBCC_FrBC_EmitJumpRegZero(ctx, type, opr, srca, name);
//		if(rt>=0)return(rt);
//	}
	
	y=BGBCC_FrBC_GetRegOprMode2(ctx, dst, src);

	if(	BGBCC_FrBC_IsRegBaseP(ctx, dst) &&
		BGBCC_FrBC_IsRegBaseP(ctx, src) &&
		(idx>=(-2048)) && (idx<2048) &&
		(y>=0) && (y<16))
	{
		op=-1;
		if(BGBCC_FrBC_TypeIntP(ctx, type))
			{ op=FR2C_OP_LOADINDEX_IC; }
		if(BGBCC_FrBC_TypeLongP(ctx, type))
			{ op=FR2C_OP_LOADINDEX_LC; }
		if(BGBCC_FrBC_TypeFloatP(ctx, type))
			{ op=FR2C_OP_LOADINDEX_FC; }
		if(BGBCC_FrBC_TypeDoubleP(ctx, type))
			{ op=FR2C_OP_LOADINDEX_DC; }
		if(BGBCC_FrBC_TypePointerP(ctx, type))
			{ op=FR2C_OP_LOADINDEX_PC; }

		if(BGBCC_FrBC_TypeSignedByteP(ctx, type))
			{ op=FR2C_OP_LOADINDEX_SBC; }
		if(BGBCC_FrBC_TypeUnsignedByteP(ctx, type))
			{ op=FR2C_OP_LOADINDEX_UBC; }
		if(BGBCC_FrBC_TypeSignedShortP(ctx, type))
			{ op=FR2C_OP_LOADINDEX_SSC; }
		if(BGBCC_FrBC_TypeUnsignedShortP(ctx, type))
			{ op=FR2C_OP_LOADINDEX_USC; }

		if(op<0)return(-1);

		BGBCC_FrBC_EmitOpcode(ctx, op);
		BGBCC_FrBC_EmitByte(ctx, (y<<4)|BGBCC_FrBC_GetRegID(ctx, dst));
		BGBCC_FrBC_EmitByte(ctx,
			(BGBCC_FrBC_GetRegID(ctx, src)<<4)|(idx>>8)&15);
		BGBCC_FrBC_EmitByte(ctx, idx&255);

		return(FR2C_STATUS_YES);
	}

	BGBCC_FrBC_TagError(ctx,
		FR2C_TERR_STATUS(FR2C_STATUS_ERR_BADOPARGS));
	return(FR2C_STATUS_ERR_BADOPARGS);
}

frbc_status BGBCC_FrBC_EmitStoreIndexImm(
	BGBCC_FrCC_State *ctx, frbc_type type, frbc_register dst,
	frbc_register src, int idx)
{
	frbc_register treg;
	int y, rt, op;
	int i, j, k;

//	if(BGBCC_FrBC_IsRegImmZeroP(ctx, srcb))
//	{
//		rt=BGBCC_FrBC_EmitJumpRegZero(ctx, type, opr, srca, name);
//		if(rt>=0)return(rt);
//	}

	if(BGBCC_FrBC_IsRegImmP(ctx, src))
	{
		BGBCC_FrBC_RegisterAllocTemporary(ctx, type, &treg);
		BGBCC_FrBC_EmitMov(ctx, type, treg, src);
		rt=BGBCC_FrBC_EmitStoreIndexImm(ctx, type, dst, treg, idx);
		BGBCC_FrBC_RegisterCheckRelease(ctx, treg);
		if(rt>=0)return(rt);

		BGBCC_FrBC_TagError(ctx,
			FR2C_TERR_STATUS(FR2C_STATUS_ERR_BADOPARGS));
		return(FR2C_STATUS_ERR_BADOPARGS);
	}
	
	y=BGBCC_FrBC_GetRegOprMode2(ctx, dst, src);

	if(	BGBCC_FrBC_IsRegBaseP(ctx, dst) &&
		BGBCC_FrBC_IsRegBaseP(ctx, src) &&
		(idx>=(-2048)) && (idx<2048) &&
		(y>=0) && (y<16))
	{
		op=-1;
		if(BGBCC_FrBC_TypeIntP(ctx, type))
			{ op=FR2C_OP_STOREINDEX_IC; }
		if(BGBCC_FrBC_TypeLongP(ctx, type))
			{ op=FR2C_OP_STOREINDEX_LC; }
		if(BGBCC_FrBC_TypeFloatP(ctx, type))
			{ op=FR2C_OP_STOREINDEX_FC; }
		if(BGBCC_FrBC_TypeDoubleP(ctx, type))
			{ op=FR2C_OP_STOREINDEX_DC; }
		if(BGBCC_FrBC_TypePointerP(ctx, type))
			{ op=FR2C_OP_STOREINDEX_PC; }

		if(BGBCC_FrBC_TypeSignedByteP(ctx, type))
			{ op=FR2C_OP_STOREINDEX_BC; }
		if(BGBCC_FrBC_TypeUnsignedByteP(ctx, type))
			{ op=FR2C_OP_STOREINDEX_BC; }
		if(BGBCC_FrBC_TypeSignedShortP(ctx, type))
			{ op=FR2C_OP_STOREINDEX_SC; }
		if(BGBCC_FrBC_TypeUnsignedShortP(ctx, type))
			{ op=FR2C_OP_STOREINDEX_SC; }

		if(op<0)return(-1);

		BGBCC_FrBC_EmitOpcode(ctx, op);
		BGBCC_FrBC_EmitByte(ctx, (y<<4)|BGBCC_FrBC_GetRegID(ctx, dst));
		BGBCC_FrBC_EmitByte(ctx,
			(BGBCC_FrBC_GetRegID(ctx, src)<<4)|(idx>>8)&15);
		BGBCC_FrBC_EmitByte(ctx, idx&255);

		return(FR2C_STATUS_YES);
	}

	BGBCC_FrBC_TagError(ctx,
		FR2C_TERR_STATUS(FR2C_STATUS_ERR_BADOPARGS));
	return(FR2C_STATUS_ERR_BADOPARGS);
}

frbc_status BGBCC_FrBC_EmitLoadIndex(
	BGBCC_FrCC_State *ctx, frbc_type type, frbc_register dst,
	frbc_register srca, frbc_register srcb)
{
	int y, rt, op;
	int i, j, k;

//	if(BGBCC_FrBC_IsRegImmZeroP(ctx, srcb))
//	{
//		rt=BGBCC_FrBC_EmitJumpRegZero(ctx, type, opr, srca, name);
//		if(rt>=0)return(rt);
//	}
	
	if(BGBCC_FrBC_IsRegImmIntP(ctx, srcb))
	{
		i=BGBCC_FrBC_GetRegImmIntValue(ctx, srcb);
		rt=BGBCC_FrBC_EmitLoadIndexImm(ctx, type, dst, srca, i);
		return(rt);
	}
	
	y=BGBCC_FrBC_GetRegOprMode3(ctx, dst, srca, srcb);

	if(	BGBCC_FrBC_IsRegBaseP(ctx, dst) &&
		BGBCC_FrBC_IsRegBaseP(ctx, srca) &&
		BGBCC_FrBC_IsRegBaseP(ctx, srcb) &&
		(y>=0) && (y<16))
	{
		op=-1;
		if(BGBCC_FrBC_TypeIntP(ctx, type))
			{ op=FR2C_OP_LOADINDEX_I; }
		if(BGBCC_FrBC_TypeLongP(ctx, type))
			{ op=FR2C_OP_LOADINDEX_L; }
		if(BGBCC_FrBC_TypeFloatP(ctx, type))
			{ op=FR2C_OP_LOADINDEX_F; }
		if(BGBCC_FrBC_TypeDoubleP(ctx, type))
			{ op=FR2C_OP_LOADINDEX_D; }
		if(BGBCC_FrBC_TypePointerP(ctx, type))
			{ op=FR2C_OP_LOADINDEX_P; }

		if(BGBCC_FrBC_TypeSignedByteP(ctx, type))
			{ op=FR2C_OP_LOADINDEX_SB; }
		if(BGBCC_FrBC_TypeUnsignedByteP(ctx, type))
			{ op=FR2C_OP_LOADINDEX_UB; }
		if(BGBCC_FrBC_TypeSignedShortP(ctx, type))
			{ op=FR2C_OP_LOADINDEX_SS; }
		if(BGBCC_FrBC_TypeUnsignedShortP(ctx, type))
			{ op=FR2C_OP_LOADINDEX_US; }

		if(op<0)return(-1);

		BGBCC_FrBC_EmitOpcode(ctx, op);
		BGBCC_FrBC_EmitByte(ctx, (y<<4)|BGBCC_FrBC_GetRegID(ctx, dst));
		BGBCC_FrBC_EmitByte(ctx,
			(BGBCC_FrBC_GetRegID(ctx, srca)<<4)|
			 BGBCC_FrBC_GetRegID(ctx, srcb));

		return(FR2C_STATUS_YES);
	}

	BGBCC_FrBC_TagError(ctx,
		FR2C_TERR_STATUS(FR2C_STATUS_ERR_BADOPARGS));
	return(FR2C_STATUS_ERR_BADOPARGS);
}

frbc_status BGBCC_FrBC_EmitStoreIndex(
	BGBCC_FrCC_State *ctx, frbc_type type, frbc_register dst,
	frbc_register srca, frbc_register srcb)
{
	frbc_register treg;
	int y, rt, op;
	int i, j, k;

//	if(BGBCC_FrBC_IsRegImmZeroP(ctx, srcb))
//	{
//		rt=BGBCC_FrBC_EmitJumpRegZero(ctx, type, opr, srca, name);
//		if(rt>=0)return(rt);
//	}
	
	if(BGBCC_FrBC_IsRegImmP(ctx, srcb))
	{
		BGBCC_FrBC_RegisterAllocTemporary(ctx, type, &treg);
		BGBCC_FrBC_EmitMov(ctx, type, treg, srcb);
		rt=BGBCC_FrBC_EmitStoreIndex(ctx, type, dst, srca, treg);
		BGBCC_FrBC_RegisterCheckRelease(ctx, treg);
		if(rt>=0)return(rt);

		BGBCC_FrBC_TagError(ctx,
			FR2C_TERR_STATUS(FR2C_STATUS_ERR_BADOPARGS));
		return(FR2C_STATUS_ERR_BADOPARGS);
	}

	if(BGBCC_FrBC_IsRegImmIntP(ctx, srca))
	{
		i=BGBCC_FrBC_GetRegImmIntValue(ctx, srca);
		rt=BGBCC_FrBC_EmitStoreIndexImm(ctx, type, dst, srcb, i);
		return(rt);
	}
	
	y=BGBCC_FrBC_GetRegOprMode3(ctx, dst, srca, srcb);

	if(	BGBCC_FrBC_IsRegBaseP(ctx, dst) &&
		BGBCC_FrBC_IsRegBaseP(ctx, srca) &&
		BGBCC_FrBC_IsRegBaseP(ctx, srcb) &&
		(y>=0) && (y<16))
	{
		op=-1;
		if(BGBCC_FrBC_TypeIntP(ctx, type))
			{ op=FR2C_OP_STOREINDEX_I; }
		if(BGBCC_FrBC_TypeLongP(ctx, type))
			{ op=FR2C_OP_STOREINDEX_L; }
		if(BGBCC_FrBC_TypeFloatP(ctx, type))
			{ op=FR2C_OP_STOREINDEX_F; }
		if(BGBCC_FrBC_TypeDoubleP(ctx, type))
			{ op=FR2C_OP_STOREINDEX_D; }
		if(BGBCC_FrBC_TypePointerP(ctx, type))
			{ op=FR2C_OP_STOREINDEX_P; }

		if(BGBCC_FrBC_TypeSignedByteP(ctx, type))
			{ op=FR2C_OP_STOREINDEX_B; }
		if(BGBCC_FrBC_TypeUnsignedByteP(ctx, type))
			{ op=FR2C_OP_STOREINDEX_B; }
		if(BGBCC_FrBC_TypeSignedShortP(ctx, type))
			{ op=FR2C_OP_STOREINDEX_S; }
		if(BGBCC_FrBC_TypeUnsignedShortP(ctx, type))
			{ op=FR2C_OP_STOREINDEX_S; }

		if(op<0)return(-1);

		BGBCC_FrBC_EmitOpcode(ctx, op);
		BGBCC_FrBC_EmitByte(ctx, (y<<4)|BGBCC_FrBC_GetRegID(ctx, dst));
		BGBCC_FrBC_EmitByte(ctx,
			(BGBCC_FrBC_GetRegID(ctx, srca)<<4)|
			 BGBCC_FrBC_GetRegID(ctx, srcb));

		return(FR2C_STATUS_YES);
	}

	BGBCC_FrBC_TagError(ctx,
		FR2C_TERR_STATUS(FR2C_STATUS_ERR_BADOPARGS));
	return(FR2C_STATUS_ERR_BADOPARGS);
}

frbc_status BGBCC_FrBC_EmitLeaImm(
	BGBCC_FrCC_State *ctx, frbc_type type, frbc_register dst,
	frbc_register src, int idx)
{
	int y, rt, op;
	int i, j, k;

//	if(BGBCC_FrBC_IsRegImmZeroP(ctx, srcb))
//	{
//		rt=BGBCC_FrBC_EmitJumpRegZero(ctx, type, opr, srca, name);
//		if(rt>=0)return(rt);
//	}
	
	y=BGBCC_FrBC_GetRegOprMode2(ctx, dst, src);

	if(	BGBCC_FrBC_IsRegBaseP(ctx, dst) &&
		BGBCC_FrBC_IsRegBaseP(ctx, src) &&
		(idx>=(-2048)) && (idx<2048) &&
		(y>=0) && (y<16))
	{
		op=-1;
		if(BGBCC_FrBC_TypeIntP(ctx, type))
			{ op=FR2C_OP_LEA_IC; }
		if(BGBCC_FrBC_TypeLongP(ctx, type))
			{ op=FR2C_OP_LEA_LC; }
		if(BGBCC_FrBC_TypeFloatP(ctx, type))
			{ op=FR2C_OP_LEA_FC; }
		if(BGBCC_FrBC_TypeDoubleP(ctx, type))
			{ op=FR2C_OP_LEA_DC; }
		if(BGBCC_FrBC_TypePointerP(ctx, type))
			{ op=FR2C_OP_LEA_PC; }

		if(BGBCC_FrBC_TypeSignedByteP(ctx, type))
			{ op=FR2C_OP_LEA_BC; }
		if(BGBCC_FrBC_TypeUnsignedByteP(ctx, type))
			{ op=FR2C_OP_LEA_BC; }
		if(BGBCC_FrBC_TypeSignedShortP(ctx, type))
			{ op=FR2C_OP_LEA_SC; }
		if(BGBCC_FrBC_TypeUnsignedShortP(ctx, type))
			{ op=FR2C_OP_LEA_SC; }

		if(op<0)return(-1);

		BGBCC_FrBC_EmitOpcode(ctx, op);
		BGBCC_FrBC_EmitByte(ctx, (y<<4)|BGBCC_FrBC_GetRegID(ctx, dst));
		BGBCC_FrBC_EmitByte(ctx,
			(BGBCC_FrBC_GetRegID(ctx, src)<<4)|(idx>>8)&15);
		BGBCC_FrBC_EmitByte(ctx, idx&255);

		return(FR2C_STATUS_YES);
	}

	BGBCC_FrBC_TagError(ctx,
		FR2C_TERR_STATUS(FR2C_STATUS_ERR_BADOPARGS));
	return(FR2C_STATUS_ERR_BADOPARGS);
}

frbc_status BGBCC_FrBC_EmitLea(
	BGBCC_FrCC_State *ctx, frbc_type type, frbc_register dst,
	frbc_register srca, frbc_register srcb)
{
	int y, rt, op;
	int i, j, k;
	
	if(BGBCC_FrBC_IsRegImmIntP(ctx, srcb))
	{
		i=BGBCC_FrBC_GetRegImmIntValue(ctx, srcb);
		rt=BGBCC_FrBC_EmitLeaImm(ctx, type, dst, srca, i);
		return(rt);
	}
	
	y=BGBCC_FrBC_GetRegOprMode3(ctx, dst, srca, srcb);

	if(	BGBCC_FrBC_IsRegBaseP(ctx, dst) &&
		BGBCC_FrBC_IsRegBaseP(ctx, srca) &&
		BGBCC_FrBC_IsRegBaseP(ctx, srcb) &&
		(y>=0) && (y<16))
	{
		op=-1;
		if(BGBCC_FrBC_TypeIntP(ctx, type))
			{ op=FR2C_OP_LEA_I; }
		if(BGBCC_FrBC_TypeLongP(ctx, type))
			{ op=FR2C_OP_LEA_L; }
		if(BGBCC_FrBC_TypeFloatP(ctx, type))
			{ op=FR2C_OP_LEA_F; }
		if(BGBCC_FrBC_TypeDoubleP(ctx, type))
			{ op=FR2C_OP_LEA_D; }
		if(BGBCC_FrBC_TypePointerP(ctx, type))
			{ op=FR2C_OP_LEA_P; }

		if(BGBCC_FrBC_TypeSignedByteP(ctx, type))
			{ op=FR2C_OP_LEA_B; }
		if(BGBCC_FrBC_TypeUnsignedByteP(ctx, type))
			{ op=FR2C_OP_LEA_B; }
		if(BGBCC_FrBC_TypeSignedShortP(ctx, type))
			{ op=FR2C_OP_LEA_S; }
		if(BGBCC_FrBC_TypeUnsignedShortP(ctx, type))
			{ op=FR2C_OP_LEA_S; }

		if(op<0)
		{
			BGBCC_FrBC_TagError(ctx,
				FR2C_TERR_STATUS(FR2C_STATUS_ERR_UNHANDLEDTYPE));
			return(FR2C_STATUS_ERR_UNHANDLEDTYPE);
		}

		BGBCC_FrBC_EmitOpcode(ctx, op);
		BGBCC_FrBC_EmitByte(ctx, (y<<4)|BGBCC_FrBC_GetRegID(ctx, dst));
		BGBCC_FrBC_EmitByte(ctx,
			(BGBCC_FrBC_GetRegID(ctx, srca)<<4)|
			 BGBCC_FrBC_GetRegID(ctx, srcb));

		return(FR2C_STATUS_YES);
	}

	BGBCC_FrBC_TagError(ctx,
		FR2C_TERR_STATUS(FR2C_STATUS_ERR_BADOPARGS));
	return(FR2C_STATUS_ERR_BADOPARGS);
}


frbc_status BGBCC_FrBC_EmitLdaVar(
	BGBCC_FrCC_State *ctx, frbc_type type,
	frbc_register dst, frbc_register src)
{
	int y, rt, op;
	int i, j, k;
	
	y=BGBCC_FrBC_GetRegOprMode2(ctx, dst, src);

	if(	BGBCC_FrBC_IsRegBaseP(ctx, dst) &&
		BGBCC_FrBC_IsRegBaseExtP(ctx, src) &&
		(y>=0) && (y<16))
	{
		BGBCC_FrBC_EmitOpcode(ctx, FR2C_OP_LDAVAR);
		BGBCC_FrBC_EmitByte(ctx, (y<<4)|BGBCC_FrBC_GetRegID(ctx, dst));
		BGBCC_FrBC_EmitByte(ctx, BGBCC_FrBC_GetRegID(ctx, src));

		return(FR2C_STATUS_YES);
	}

	BGBCC_FrBC_TagError(ctx,
		FR2C_TERR_STATUS(FR2C_STATUS_ERR_BADOPARGS));
	return(FR2C_STATUS_ERR_BADOPARGS);
}

frbc_status BGBCC_FrBC_EmitSizeofVar(
	BGBCC_FrCC_State *ctx, frbc_type type,
	frbc_register dst)
{
	BGBCC_FrCC_LiteralInfo *linf;
	char *s;
	int y;
	int i, j, k;

	y=BGBCC_FrBC_GetRegOprMode1(ctx, dst);

	BGBCC_FrBC_EmitOpcode(ctx, FR2C_OP_SIZEOF_I);
	BGBCC_FrBC_EmitByte(ctx, (y<<4)|BGBCC_FrBC_GetRegID(ctx, dst));

	s=BGBCC_FrBC_TypeGetSig(ctx, type);
	linf=BGBCC_FrBC_GetTypedef(ctx, NULL, s);
	i=linf->litid;
	
	BGBCC_FrBC_EmitWord(ctx, i);
	BGBCC_FrBC_EmitRelocLiteral16(ctx);
	return(FR2C_STATUS_YES);
}
