#include <bgbccc.h>

BGBCC_FrCC_LiteralInfo *BGBCC_FrBC_LookupStructureForSig(
	BGBCC_FrCC_State *ctx, char *sig)
{
	char *s, *t;

	s=sig;
	while(*s && (*s=='P'))
		{ s++; }

	return(BGBCC_FrBC_LookupStructureForSig2(ctx, s));
}

BGBCC_FrCC_LiteralInfo *BGBCC_FrBC_LookupStructureForSig2(
	BGBCC_FrCC_State *ctx, char *sig)
{
	char tb[256];
	BGBCC_FrCC_LiteralInfo *cur;
	char *s, *t;
	int i;
	
	s=sig;
	while(*s && (*s=='P'))
		{ s++; }
	if(*s!='X')
		{ return(NULL); }
	s++;
		
	if((*s>='0') && (*s<='9'))
	{
		i=atoi(s);
		cur=ctx->literals[i];
		return(cur);
	}

	t=tb;
	while(*s && (*s!=';'))
		*t++=*s++;
	*t++=0;
	
	cur=BGBCC_FrBC_LookupStructure(ctx, tb);
	return(cur);
}

int BGBCC_FrBC_LookupStructureIDForSig(
	BGBCC_FrCC_State *ctx, char *sig)
{
	BGBCC_FrCC_LiteralInfo *cur;
	
	cur=BGBCC_FrBC_LookupStructureForSig(ctx, sig);
	if(!cur)return(-1);
	return(cur->litid);
}

BGBCC_FrCC_LiteralInfo *BGBCC_FrBC_LookupStructureForType(
	BGBCC_FrCC_State *ctx, frbc_type type)
{
	BGBCC_FrCC_LiteralInfo *st;
	int bt;

	bt=type.val&FR2C_TY_BASEMASK;
	st=ctx->literals[bt-256];
	return(st);
}

int BGBCC_FrBC_LookupStructFieldID(
	BGBCC_FrCC_State *ctx, BGBCC_FrCC_LiteralInfo *st, char *name)
{
	int i;
	
	if(!st)
		return(-1);
	if(!st->decl)
		return(-1);
	
	for(i=0; i<st->decl->n_fields; i++)
	{
		if(!st->decl->fields[i]->name, name)
			return(i);
	}
	return(-1);
}

frbc_status BGBCC_FrBC_LookupStructFieldType(
	BGBCC_FrCC_State *ctx, BGBCC_FrCC_LiteralInfo *st, char *name,
	frbc_type *rty)
{
	frbc_type bty;
	int i;
	
	if(!st)
		return(-1);
	if(!st->decl)
		return(-1);
	
	for(i=0; i<st->decl->n_fields; i++)
	{
		if(!st->decl->fields[i]->name, name)
		{
			BGBCC_FrBC_TypeFromSig(ctx, &bty,
				st->decl->fields[i]->sig);
			*rty=bty;
			return(1);
		}
	}
	return(0);
}

frbc_status BGBCC_FrBC_EmitOffsetOf(
	BGBCC_FrCC_State *ctx, frbc_type type, frbc_register dst,
	BGBCC_FrCC_LiteralInfo *st, char *name)
{
	int y, rt, op, fn;
	int i, j, k;
	
	fn=BGBCC_FrBC_LookupStructFieldID(ctx, st, name);
	if(fn<0)
	{
		BGBCC_FrBC_TagError(ctx,
			FR2C_TERR_STATUS(FR2C_STATUS_ERR_BADOPARGS));
		return(FR2C_STATUS_ERR_BADOPARGS);
	}

	y=BGBCC_FrBC_GetRegOprMode1(ctx, dst);

	if(	BGBCC_FrBC_IsRegBaseP(ctx, dst) &&
		(y>=0) && (y<16))
	{
		BGBCC_FrBC_EmitOpcode(ctx, FR2C_OP_OFFSETOF_I);
		BGBCC_FrBC_EmitByte(ctx, (y<<4)|BGBCC_FrBC_GetRegID(ctx, dst));
		BGBCC_FrBC_EmitWord(ctx, st->litid);
		BGBCC_FrBC_EmitByte(ctx, fn);
		return(FR2C_STATUS_YES);
	}

	BGBCC_FrBC_TagError(ctx,
		FR2C_TERR_STATUS(FR2C_STATUS_ERR_BADOPARGS));
	return(FR2C_STATUS_ERR_BADOPARGS);
}

frbc_status BGBCC_FrBC_EmitLoadSlot(
	BGBCC_FrCC_State *ctx, frbc_type type,
	frbc_register dst, frbc_register src,
	BGBCC_FrCC_LiteralInfo *st, char *name)
{
	int z, y, rt, op, fn;
	int i, j, k;
	
	fn=BGBCC_FrBC_LookupStructFieldID(ctx, st, name);
	if(fn<0)
	{
		BGBCC_FrBC_TagError(ctx,
			FR2C_TERR_STATUS(FR2C_STATUS_ERR_BADOPARGS));
		return(FR2C_STATUS_ERR_BADOPARGS);
	}

	z=BGBCC_FrBC_GetTypeOperationZ(ctx, type);
	y=BGBCC_FrBC_GetRegOprMode2(ctx, dst, src);

	if(	BGBCC_FrBC_IsRegBaseP(ctx, dst) &&
		BGBCC_FrBC_IsRegBaseP(ctx, src) &&
		(z>=0) && (z<16) && (y>=0) && (y<16))
	{
		BGBCC_FrBC_EmitOpcode(ctx, FR2C_OP_LOADSLOT);
		BGBCC_FrBC_EmitByte(ctx, (z<<4)|y);
		BGBCC_FrBC_EmitByte(ctx,
			(BGBCC_FrBC_GetRegID(ctx, dst)<<4)|
			 BGBCC_FrBC_GetRegID(ctx, src));
//		BGBCC_FrBC_EmitByte(ctx, (y<<4)|BGBCC_FrBC_GetRegID(ctx, dst));
		BGBCC_FrBC_EmitWord(ctx, st->litid);
		BGBCC_FrBC_EmitByte(ctx, fn);
		return(FR2C_STATUS_YES);
	}

	BGBCC_FrBC_TagError(ctx,
		FR2C_TERR_STATUS(FR2C_STATUS_ERR_BADOPARGS));
	return(FR2C_STATUS_ERR_BADOPARGS);
}

frbc_status BGBCC_FrBC_EmitStoreSlot(
	BGBCC_FrCC_State *ctx, frbc_type type,
	frbc_register dst, frbc_register src,
	BGBCC_FrCC_LiteralInfo *st, char *name)
{
	int z, y, rt, op, fn;
	int i, j, k;
	
	fn=BGBCC_FrBC_LookupStructFieldID(ctx, st, name);
	if(fn<0)
	{
		BGBCC_FrBC_TagError(ctx,
			FR2C_TERR_STATUS(FR2C_STATUS_ERR_BADOPARGS));
		return(FR2C_STATUS_ERR_BADOPARGS);
	}

	z=BGBCC_FrBC_GetTypeOperationZ(ctx, type);
	y=BGBCC_FrBC_GetRegOprMode2(ctx, dst, src);

	if(	BGBCC_FrBC_IsRegBaseP(ctx, dst) &&
		BGBCC_FrBC_IsRegBaseP(ctx, src) &&
		(z>=0) && (z<16) && (y>=0) && (y<16))
	{
		BGBCC_FrBC_EmitOpcode(ctx, FR2C_OP_STORESLOT);
		BGBCC_FrBC_EmitByte(ctx, (z<<4)|y);
		BGBCC_FrBC_EmitByte(ctx,
			(BGBCC_FrBC_GetRegID(ctx, dst)<<4)|
			 BGBCC_FrBC_GetRegID(ctx, src));
//		BGBCC_FrBC_EmitByte(ctx, (y<<4)|BGBCC_FrBC_GetRegID(ctx, dst));
		BGBCC_FrBC_EmitWord(ctx, st->litid);
		BGBCC_FrBC_EmitByte(ctx, fn);
		return(FR2C_STATUS_YES);
	}

	BGBCC_FrBC_TagError(ctx,
		FR2C_TERR_STATUS(FR2C_STATUS_ERR_BADOPARGS));
	return(FR2C_STATUS_ERR_BADOPARGS);
}


frbc_status BGBCC_FrBC_EmitLoadSlotAddr(
	BGBCC_FrCC_State *ctx, frbc_type type,
	frbc_register dst, frbc_register src,
	BGBCC_FrCC_LiteralInfo *st, char *name)
{
	int z, y, rt, op, fn;
	int i, j, k;
	
	fn=BGBCC_FrBC_LookupStructFieldID(ctx, st, name);
	if(fn<0)
	{
		BGBCC_FrBC_TagError(ctx,
			FR2C_TERR_STATUS(FR2C_STATUS_ERR_BADOPARGS));
		return(FR2C_STATUS_ERR_BADOPARGS);
	}

	z=BGBCC_FrBC_GetTypeOperationZ(ctx, type);
	y=BGBCC_FrBC_GetRegOprMode2(ctx, dst, src);

	if(	BGBCC_FrBC_IsRegBaseExtP(ctx, dst) &&
		BGBCC_FrBC_IsRegBaseP(ctx, src) &&
		(z>=0) && (z<16) && (y>=0) && (y<16))
	{
		BGBCC_FrBC_EmitOpcode(ctx, FR2C_OP_LOADSLOTAW);
		BGBCC_FrBC_EmitByte(ctx, (z<<4)|y);

		BGBCC_FrBC_EmitByte(ctx, BGBCC_FrBC_GetRegID(ctx, dst));
		BGBCC_FrBC_EmitByte(ctx, BGBCC_FrBC_GetRegID(ctx, src));
		BGBCC_FrBC_EmitWord24(ctx, st->litid);
		BGBCC_FrBC_EmitWord(ctx, fn);

//		BGBCC_FrBC_EmitByte(ctx,
//			(BGBCC_FrBC_GetRegID(ctx, dst)<<4)|
//			 BGBCC_FrBC_GetRegID(ctx, src));
//		BGBCC_FrBC_EmitWord(ctx, st->litid);
//		BGBCC_FrBC_EmitByte(ctx, fn);
		return(FR2C_STATUS_YES);
	}

	BGBCC_FrBC_TagError(ctx,
		FR2C_TERR_STATUS(FR2C_STATUS_ERR_BADOPARGS));
	return(FR2C_STATUS_ERR_BADOPARGS);
}


frbc_status BGBCC_FrBC_EmitInitObj(
	BGBCC_FrCC_State *ctx, frbc_type type,
	frbc_register dst, BGBCC_FrCC_LiteralInfo *st)
{
	int z, y, w, rt, op, fn;
	int i, j, k;
	
//	z=BGBCC_FrBC_GetTypeOperationZ(ctx, type);
//	y=BGBCC_FrBC_GetRegOprMode2(ctx, dst, src);
	w=BGBCC_FrBC_GetRegOprModeW(ctx, dst, type);

	if(	BGBCC_FrBC_IsRegBaseExtP(ctx, dst) &&
		(w>=0) && (w<16))
	{
		BGBCC_FrBC_EmitOpcode(ctx, FR2C_OP_INITOBJ);
		BGBCC_FrBC_EmitByte(ctx, (w<<4)|
			(BGBCC_FrBC_GetRegID(ctx, dst)&15));
		BGBCC_FrBC_EmitWord(ctx, st->litid);
		return(FR2C_STATUS_YES);
	}

	BGBCC_FrBC_TagError(ctx,
		FR2C_TERR_STATUS(FR2C_STATUS_ERR_BADOPARGS));
	return(FR2C_STATUS_ERR_BADOPARGS);
}

int BGBCC_FrBC_GetArraySizeForSig(
	BGBCC_FrCC_State *ctx, char *sig)
{
	char *s, *t;
	s=sig;
	while(*s && (*s=='P'))
		{ s++; }
	return(BGBCC_FrBC_GetArraySizeForSig2(ctx, s));
}

int BGBCC_FrBC_GetArraySizeForSig2(
	BGBCC_FrCC_State *ctx, char *sig)
{
	return(BGBCC_FrBC_GetArraySizeForSig2R(ctx, &sig));
}

int BGBCC_FrBC_GetArraySizeForSig2R(
	BGBCC_FrCC_State *ctx, char **rsig)
{
	char *s, *t;
	int i;

	s=*rsig;
	if(*s=='A')
	{
		s++; i=0;
		while(*s && (*s>='0') && (*s<='9'))
			{ i=(i*10)+((*s++)-'0'); }
		if(*s==',')
			return(-1);		
		if(*s==';')
			s++;
		*rsig=s;
		return(i);
	}
	
	return(-1);
}

frbc_status BGBCC_FrBC_EmitInitArr(
	BGBCC_FrCC_State *ctx, frbc_type type,
	frbc_register dst, int sz)
{
	int z, y, w, rt, op, fn;
	int i, j, k;
	
	z=BGBCC_FrBC_GetTypeOperationZ(ctx, type);
//	y=BGBCC_FrBC_GetRegOprMode2(ctx, dst, src);
//	w=BGBCC_FrBC_GetRegOprModeW(ctx, dst, type);

	if(	BGBCC_FrBC_IsRegLocalBasicP(ctx, dst) &&
		(z>=0) && (z<16))
	{
		BGBCC_FrBC_EmitOpcode(ctx, FR2C_OP_INITARR);
		BGBCC_FrBC_EmitByte(ctx, (z<<4)|
			(BGBCC_FrBC_GetRegID(ctx, dst)&15));
		BGBCC_FrBC_EmitWord(ctx, sz);
		return(FR2C_STATUS_YES);
	}

	BGBCC_FrBC_TagError(ctx,
		FR2C_TERR_STATUS(FR2C_STATUS_ERR_BADOPARGS));
	return(FR2C_STATUS_ERR_BADOPARGS);
}

frbc_status BGBCC_FrBC_EmitInitObjArr(
	BGBCC_FrCC_State *ctx, frbc_type type,
	frbc_register dst, BGBCC_FrCC_LiteralInfo *st, int sz)
{
	int z, y, w, rt, op, fn;
	int i, j, k;
	
//	z=BGBCC_FrBC_GetTypeOperationZ(ctx, type);
//	y=BGBCC_FrBC_GetRegOprMode2(ctx, dst, src);
	w=BGBCC_FrBC_GetRegOprModeW(ctx, dst, type);

	if(	BGBCC_FrBC_IsRegBaseExtP(ctx, dst) &&
		(w>=0) && (w<16))
	{
		BGBCC_FrBC_EmitOpcode(ctx, FR2C_OP_INITOBJARR);
		BGBCC_FrBC_EmitByte(ctx, (w<<4)|
			(BGBCC_FrBC_GetRegID(ctx, dst)&15));
		BGBCC_FrBC_EmitWord(ctx, st->litid);
		BGBCC_FrBC_EmitWord(ctx, sz);
		return(FR2C_STATUS_YES);
	}

	BGBCC_FrBC_TagError(ctx,
		FR2C_TERR_STATUS(FR2C_STATUS_ERR_BADOPARGS));
	return(FR2C_STATUS_ERR_BADOPARGS);
}
