#include <bgbccc.h>

bool BGBCC_FrBC_TypeSmallIntP(
	BGBCC_FrCC_State *ctx, frbc_type ty)
{
	if(BGBCC_FrBC_TypeArrayP(ctx, ty))
		return(false);
	if(BGBCC_FrBC_TypePointerP(ctx, ty))
		return(false);

	if((ty.val&FR2C_TY_BASEMASK)==FR2C_TY_I)
		return(true);
	if((ty.val&FR2C_TY_BASEMASK)==FR2C_TY_UI)
		return(true);

	if((ty.val&FR2C_TY_BASEMASK)==FR2C_TY_SB)
		return(true);
	if((ty.val&FR2C_TY_BASEMASK)==FR2C_TY_UB)
		return(true);
	if((ty.val&FR2C_TY_BASEMASK)==FR2C_TY_SS)
		return(true);
	if((ty.val&FR2C_TY_BASEMASK)==FR2C_TY_US)
		return(true);

	return(false);
}

bool BGBCC_FrBC_TypeSmallLongP(
	BGBCC_FrCC_State *ctx, frbc_type ty)
{
	if(BGBCC_FrBC_TypeArrayP(ctx, ty))
		return(false);
	if(BGBCC_FrBC_TypePointerP(ctx, ty))
		return(false);

	if((ty.val&FR2C_TY_BASEMASK)==FR2C_TY_L)
		return(true);
	if((ty.val&FR2C_TY_BASEMASK)==FR2C_TY_UL)
		return(true);
	if((ty.val&FR2C_TY_BASEMASK)==FR2C_TY_I)
		return(true);
	if((ty.val&FR2C_TY_BASEMASK)==FR2C_TY_UI)
		return(true);
	if((ty.val&FR2C_TY_BASEMASK)==FR2C_TY_NL)
		return(true);
	if((ty.val&FR2C_TY_BASEMASK)==FR2C_TY_UNL)
		return(true);

	if((ty.val&FR2C_TY_BASEMASK)==FR2C_TY_SB)
		return(true);
	if((ty.val&FR2C_TY_BASEMASK)==FR2C_TY_UB)
		return(true);
	if((ty.val&FR2C_TY_BASEMASK)==FR2C_TY_SS)
		return(true);
	if((ty.val&FR2C_TY_BASEMASK)==FR2C_TY_US)
		return(true);

	return(false);
}

bool BGBCC_FrBC_TypeUnsignedP(
	BGBCC_FrCC_State *ctx, frbc_type ty)
{
	if(BGBCC_FrBC_TypeArrayP(ctx, ty))
		return(false);
	if(BGBCC_FrBC_TypePointerP(ctx, ty))
		return(false);

	if((ty.val&FR2C_TY_BASEMASK)==FR2C_TY_UL)
		return(true);
	if((ty.val&FR2C_TY_BASEMASK)==FR2C_TY_UI)
		return(true);
	if((ty.val&FR2C_TY_BASEMASK)==FR2C_TY_US)
		return(true);
	if((ty.val&FR2C_TY_BASEMASK)==FR2C_TY_UB)
		return(true);

	if((ty.val&FR2C_TY_BASEMASK)==FR2C_TY_UNL)
		return(true);

	return(false);
}

bool BGBCC_FrBC_TypeIntP(
	BGBCC_FrCC_State *ctx, frbc_type ty)
{
	if(BGBCC_FrBC_TypeArrayP(ctx, ty))
		return(false);
	if(BGBCC_FrBC_TypePointerP(ctx, ty))
		return(false);

	if((ty.val&FR2C_TY_BASEMASK)==FR2C_TY_I)
		return(true);
	return(false);
}

bool BGBCC_FrBC_TypeLongP(
	BGBCC_FrCC_State *ctx, frbc_type ty)
{
	if(BGBCC_FrBC_TypePointerP(ctx, ty))
		return(false);

	if((ty.val&FR2C_TY_BASEMASK)==FR2C_TY_L)
		return(true);
	return(false);
}

bool BGBCC_FrBC_TypeUnsignedIntP(
	BGBCC_FrCC_State *ctx, frbc_type ty)
{
	if(BGBCC_FrBC_TypeArrayP(ctx, ty))
		return(false);
	if(BGBCC_FrBC_TypePointerP(ctx, ty))
		return(false);

	if((ty.val&FR2C_TY_BASEMASK)==FR2C_TY_UI)
		return(true);
	return(false);
}

bool BGBCC_FrBC_TypeUnsignedLongP(
	BGBCC_FrCC_State *ctx, frbc_type ty)
{
	if(BGBCC_FrBC_TypeArrayP(ctx, ty))
		return(false);
	if(BGBCC_FrBC_TypePointerP(ctx, ty))
		return(false);

	if((ty.val&FR2C_TY_BASEMASK)==FR2C_TY_UL)
		return(true);
	return(false);
}

bool BGBCC_FrBC_TypeFloatP(
	BGBCC_FrCC_State *ctx, frbc_type ty)
{
	if(BGBCC_FrBC_TypeArrayP(ctx, ty))
		return(false);
	if(BGBCC_FrBC_TypePointerP(ctx, ty))
		return(false);

	if((ty.val&FR2C_TY_BASEMASK)==FR2C_TY_F)
		return(true);
	return(false);
}

bool BGBCC_FrBC_TypeDoubleP(
	BGBCC_FrCC_State *ctx, frbc_type ty)
{
	if(BGBCC_FrBC_TypeArrayP(ctx, ty))
		return(false);
	if(BGBCC_FrBC_TypePointerP(ctx, ty))
		return(false);

	if((ty.val&FR2C_TY_BASEMASK)==FR2C_TY_D)
		return(true);
	return(false);
}

bool BGBCC_FrBC_TypeBaseILFD_P(
	BGBCC_FrCC_State *ctx, frbc_type ty)
{
	if(BGBCC_FrBC_TypeArrayP(ctx, ty))
		return(false);
	if(BGBCC_FrBC_TypePointerP(ctx, ty))
		return(false);

	if((ty.val&FR2C_TY_BASEMASK)==FR2C_TY_I)
		return(true);
	if((ty.val&FR2C_TY_BASEMASK)==FR2C_TY_L)
		return(true);
	if((ty.val&FR2C_TY_BASEMASK)==FR2C_TY_F)
		return(true);
	if((ty.val&FR2C_TY_BASEMASK)==FR2C_TY_D)
		return(true);

	if((ty.val&FR2C_TY_BASEMASK)==FR2C_TY_UL)
		return(true);
	if((ty.val&FR2C_TY_BASEMASK)==FR2C_TY_UI)
		return(true);
	if((ty.val&FR2C_TY_BASEMASK)==FR2C_TY_NL)
		return(true);
	if((ty.val&FR2C_TY_BASEMASK)==FR2C_TY_UNL)
		return(true);

	if((ty.val&FR2C_TY_BASEMASK)==FR2C_TY_SB)
		return(true);
	if((ty.val&FR2C_TY_BASEMASK)==FR2C_TY_UB)
		return(true);
	if((ty.val&FR2C_TY_BASEMASK)==FR2C_TY_SS)
		return(true);
	if((ty.val&FR2C_TY_BASEMASK)==FR2C_TY_US)
		return(true);
	return(false);
}

bool BGBCC_FrBC_TypeBaseSmallILFD_P(
	BGBCC_FrCC_State *ctx, frbc_type ty)
{
	if(BGBCC_FrBC_TypeArrayP(ctx, ty))
		return(false);
	if(BGBCC_FrBC_TypePointerP(ctx, ty))
		return(false);

	if((ty.val&FR2C_TY_BASEMASK)==FR2C_TY_I)
		return(true);
	if((ty.val&FR2C_TY_BASEMASK)==FR2C_TY_L)
		return(true);
	if((ty.val&FR2C_TY_BASEMASK)==FR2C_TY_F)
		return(true);
	if((ty.val&FR2C_TY_BASEMASK)==FR2C_TY_D)
		return(true);

	if((ty.val&FR2C_TY_BASEMASK)==FR2C_TY_UL)
		return(true);
	if((ty.val&FR2C_TY_BASEMASK)==FR2C_TY_UI)
		return(true);
	if((ty.val&FR2C_TY_BASEMASK)==FR2C_TY_NL)
		return(true);
	if((ty.val&FR2C_TY_BASEMASK)==FR2C_TY_UNL)
		return(true);

	if((ty.val&FR2C_TY_BASEMASK)==FR2C_TY_SB)
		return(true);
	if((ty.val&FR2C_TY_BASEMASK)==FR2C_TY_UB)
		return(true);
	if((ty.val&FR2C_TY_BASEMASK)==FR2C_TY_SS)
		return(true);
	if((ty.val&FR2C_TY_BASEMASK)==FR2C_TY_US)
		return(true);

	return(false);
}

bool BGBCC_FrBC_TypeBaseSmallILFDP_P(
	BGBCC_FrCC_State *ctx, frbc_type ty)
{
	if(BGBCC_FrBC_TypeArrayP(ctx, ty))
		return(false);
	if(BGBCC_FrBC_TypePointerP(ctx, ty))
		return(true);
	return(BGBCC_FrBC_TypeBaseSmallILFD_P(ctx, ty));
}

bool BGBCC_FrBC_TypeBaseSmallIL_P(
	BGBCC_FrCC_State *ctx, frbc_type ty)
{
	if(BGBCC_FrBC_TypeArrayP(ctx, ty))
		return(false);
	if(BGBCC_FrBC_TypePointerP(ctx, ty))
		return(false);

	if((ty.val&FR2C_TY_BASEMASK)==FR2C_TY_I)
		return(true);
	if((ty.val&FR2C_TY_BASEMASK)==FR2C_TY_L)
		return(true);

	if((ty.val&FR2C_TY_BASEMASK)==FR2C_TY_UL)
		return(true);
	if((ty.val&FR2C_TY_BASEMASK)==FR2C_TY_UI)
		return(true);
	if((ty.val&FR2C_TY_BASEMASK)==FR2C_TY_NL)
		return(true);
	if((ty.val&FR2C_TY_BASEMASK)==FR2C_TY_UNL)
		return(true);

	if((ty.val&FR2C_TY_BASEMASK)==FR2C_TY_SB)
		return(true);
	if((ty.val&FR2C_TY_BASEMASK)==FR2C_TY_UB)
		return(true);
	if((ty.val&FR2C_TY_BASEMASK)==FR2C_TY_SS)
		return(true);
	if((ty.val&FR2C_TY_BASEMASK)==FR2C_TY_US)
		return(true);

	return(false);
}

bool BGBCC_FrBC_TypePointerP(
	BGBCC_FrCC_State *ctx, frbc_type ty)
{
	if((ty.val&FR2C_TY_TYTY_MASK)==FR2C_TY_TYTY_BASIC)
	{
		if((ty.val&FR2C_TY_BASEMASK)==FR2C_TY_P)
			return(true);
		if(ty.val&FR2C_TY_PTRMASK)
			return(true);
	}
	return(false);
}

bool BGBCC_FrBC_TypeArrayP(
	BGBCC_FrCC_State *ctx, frbc_type ty)
{
	if((ty.val&FR2C_TY_TYTY_MASK)==FR2C_TY_TYTY_BASIC)
	{
		if(ty.val&FR2C_TY_ARRMASK)
			return(true);
	}
	return(false);
}

bool BGBCC_FrBC_TypeSignedByteP(
	BGBCC_FrCC_State *ctx, frbc_type ty)
{
	if(BGBCC_FrBC_TypePointerP(ctx, ty))
		return(false);
	if((ty.val&FR2C_TY_BASEMASK)==FR2C_TY_SB)
		return(true);
	return(false);
}

bool BGBCC_FrBC_TypeUnsignedByteP(
	BGBCC_FrCC_State *ctx, frbc_type ty)
{
	if(BGBCC_FrBC_TypePointerP(ctx, ty))
		return(false);
	if((ty.val&FR2C_TY_BASEMASK)==FR2C_TY_UB)
		return(true);
	return(false);
}

bool BGBCC_FrBC_TypeSignedShortP(
	BGBCC_FrCC_State *ctx, frbc_type ty)
{
	if(BGBCC_FrBC_TypePointerP(ctx, ty))
		return(false);
	if((ty.val&FR2C_TY_BASEMASK)==FR2C_TY_SS)
		return(true);
	return(false);
}

bool BGBCC_FrBC_TypeUnsignedShortP(
	BGBCC_FrCC_State *ctx, frbc_type ty)
{
	if(BGBCC_FrBC_TypePointerP(ctx, ty))
		return(false);
	if((ty.val&FR2C_TY_BASEMASK)==FR2C_TY_US)
		return(true);
	return(false);
}

bool BGBCC_FrBC_TypeVoidP(
	BGBCC_FrCC_State *ctx, frbc_type ty)
{
	if(BGBCC_FrBC_TypePointerP(ctx, ty))
		return(false);
	if((ty.val&FR2C_TY_BASEMASK)==FR2C_TY_V)
		return(true);
	return(false);
}

bool BGBCC_FrBC_TypeEqualP(
	BGBCC_FrCC_State *ctx,
	frbc_type dty, frbc_type sty)
{
	return(dty.val==sty.val);
}

bool BGBCC_FrBC_TypeBaseEqualP(
	BGBCC_FrCC_State *ctx,
	frbc_type dty, frbc_type sty)
{
	if(BGBCC_FrBC_TypeIntP(ctx, dty) && BGBCC_FrBC_TypeIntP(ctx, sty))
		return(true);
	if(BGBCC_FrBC_TypeLongP(ctx, dty) && BGBCC_FrBC_TypeLongP(ctx, sty))
		return(true);
	if(BGBCC_FrBC_TypeFloatP(ctx, dty) && BGBCC_FrBC_TypeFloatP(ctx, sty))
		return(true);
	if(BGBCC_FrBC_TypeDoubleP(ctx, dty) && BGBCC_FrBC_TypeDoubleP(ctx, sty))
		return(true);
	if(BGBCC_FrBC_TypePointerP(ctx, dty) &&
			BGBCC_FrBC_TypePointerP(ctx, sty))
		return(true);
	return(false);
}

bool BGBCC_FrBC_TypeOprBasicP(
	BGBCC_FrCC_State *ctx, frbc_type ty)
{
	if(BGBCC_FrBC_TypePointerP(ctx, ty))
		return(true);

	if((ty.val&FR2C_TY_BASEMASK)>=16)
		return(false);

	if((ty.val&FR2C_TY_BASEMASK)==FR2C_TY_S)
		return(false);
	if((ty.val&FR2C_TY_BASEMASK)==FR2C_TY_V)
		return(false);
	if((ty.val&FR2C_TY_BASEMASK)==FR2C_TY_UNDEF_I)
		return(false);

	return(true);
}

int BGBCC_FrBC_TypeAsOprBasic(
	BGBCC_FrCC_State *ctx, frbc_type ty)
{
	if(BGBCC_FrBC_TypePointerP(ctx, ty))
		return(FR2C_TY_P);

	if((ty.val&FR2C_TY_BASEMASK)>=16)
		return(-1);
	if((ty.val&FR2C_TY_BASEMASK)==FR2C_TY_S)
		return(-1);
	if((ty.val&FR2C_TY_BASEMASK)==FR2C_TY_V)
		return(-1);
	if((ty.val&FR2C_TY_BASEMASK)==FR2C_TY_UNDEF_I)
		return(-1);

	return(ty.val&FR2C_TY_BASEMASK);
}

frbc_status BGBCC_FrBC_TypeDerefType(
	BGBCC_FrCC_State *ctx,
	frbc_type sty, frbc_type *rdty)
{
	frbc_type tty;
//	if(!BGBCC_FrBC_TypePointerP(ctx, ty))

	if(BGBCC_FrBC_TypeArrayP(ctx, sty))
	{
		tty.val=sty.val&(FR2C_TY_BASEMASK|FR2C_TY_PTRMASK);
		*rdty=tty;
		return(FR2C_STATUS_YES);
	}

	if(!(sty.val&FR2C_TY_PTRMASK))
	{
		*rdty=sty;
		return(FR2C_STATUS_ERR_UNHANDLEDTYPE);
	}

	tty.val=sty.val-FR2C_TY_PTRIDX1;
	*rdty=tty;
	return(FR2C_STATUS_YES);
}

frbc_status BGBCC_FrBC_TypePointerType(
	BGBCC_FrCC_State *ctx,
	frbc_type sty, frbc_type *rdty)
{
	frbc_type tty;

	if(BGBCC_FrBC_TypeArrayP(ctx, sty))
	{
		tty.val=sty.val&(FR2C_TY_BASEMASK|FR2C_TY_PTRMASK);
		tty.val=tty.val+FR2C_TY_PTRIDX1;
		*rdty=tty;
		return(FR2C_STATUS_YES);
	}

//	if(!BGBCC_FrBC_TypePointerP(ctx, ty))
//	if(!(sty.val&FR2C_TY_PTRMASK))
//		return(FR2C_STATUS_ERR_UNHANDLEDTYPE);
	tty.val=sty.val+FR2C_TY_PTRIDX1;
	*rdty=tty;
	return(FR2C_STATUS_YES);
}

frbc_status BGBCC_FrBC_TypeAutoPromoteType(
	BGBCC_FrCC_State *ctx,
	frbc_type sty, frbc_type *rdty)
{
	frbc_type tty;

	if(BGBCC_FrBC_TypeSmallIntP(ctx, sty) &&
		!BGBCC_FrBC_TypeIntP(ctx, sty))
	{
		tty=BGBCC_FrBC_TypeWrapBasicType(FR2C_TY_I);
		if(rdty)*rdty=tty;
		return(FR2C_STATUS_YES);
	}

#if 0
	if(BGBCC_FrBC_TypeSmallLongP(ctx, sty) &&
		!BGBCC_FrBC_TypeLongP(ctx, sty))
	{
		tty=BGBCC_FrBC_TypeWrapBasicType(FR2C_TY_L);
		*rdty=ty;
		return(FR2C_STATUS_YES);
	}
#endif

	if(rdty)*rdty=sty;
	return(FR2C_STATUS_NO);
}

frbc_status BGBCC_FrBC_TypeFromSig(
	BGBCC_FrCC_State *ctx,
	frbc_type *rty, char *sig)
{
	int asz[16];
	BGBCC_FrCC_LiteralInfo *st;
	frbc_type tty;
	char *s;
	int an, pn, bty;
	int i, j, k;
	
	s=sig; an=0; pn=0; bty=-1;
	
	if((*s=='A') && (s[1]>='0') && (s[1]<='9'))
	{
		s++; i=0;
		while(*s)
		{
			if(*s==';')
			{
				asz[an++]=i;
				s++; break;
			}
			if(*s==',')
			{
				asz[an++]=i;
				i=0; s++;
				continue;
			}
			if((*s>='0') && (*s<='9'))
			{
				i=(i*10)+((*s++)-'0');
				continue;
			}
			asz[an++]=i;
			break;
		}
	}
	
	while(*s=='P')
		{ pn++; s++; }
	switch(*s)
	{
	case 'a': bty=FR2C_TY_SB; break;
	case 'b': bty=FR2C_TY_SB; break;
	case 'c': bty=FR2C_TY_SB; break;
	case 'd': bty=FR2C_TY_D; break;
	case 'e': bty=FR2C_TY_D; break;
	case 'f': bty=FR2C_TY_F; break;
	case 'g': bty=FR2C_TY_F128; break;
	case 'h': bty=FR2C_TY_UB; break;
	case 'i': bty=FR2C_TY_I; break;
	case 'j': bty=FR2C_TY_UI; break;
	case 'k': bty=FR2C_TY_F16; break;
	case 'l': bty=FR2C_TY_NL; break;
	case 'm': bty=FR2C_TY_UNL; break;
	case 'n': bty=FR2C_TY_I128; break;
	case 'o': bty=FR2C_TY_UI128; break;
	case 'p': bty=-1; break;
	case 'q': bty=-1; break;
	case 'r': bty=FR2C_TY_VARIANT; break;
	case 's': bty=FR2C_TY_SS; break;
	case 't': bty=FR2C_TY_US; break;
	case 'u': bty=-1; break;
	case 'v': bty=FR2C_TY_V; break;
	case 'w': bty=FR2C_TY_US; break;
	case 'x': bty=FR2C_TY_L; break;
	case 'y': bty=FR2C_TY_UL; break;
	case 'z': bty=FR2C_TY_VARARGS; break;
	case 'X':
		st=BGBCC_FrBC_LookupStructureForSig(ctx, s);
		bty=st->litid+256;
		break;
	default:  bty=-1; break;
	}
	
	if((bty>=0) && (an==0))
	{
		tty.val=bty|(pn<<FR2C_TY_PTRSHL)|
			FR2C_TY_TYTY_BASIC;
		*rty=tty;
		return(FR2C_STATUS_YES);
	}

	if((bty>=0) && (an==1) && (asz[0]<FR2C_TY_BASEARRMAX))
	{
		tty.val=bty|(pn<<FR2C_TY_PTRSHL)|
			(asz[0]<<FR2C_TY_ARRSHL)|
			FR2C_TY_TYTY_BASIC;
		*rty=tty;
		return(FR2C_STATUS_YES);
	}

	return(FR2C_STATUS_NO);
}

char *BGBCC_FrBC_TypeGetSig(
	BGBCC_FrCC_State *ctx, frbc_type ty)
{
	char tb[256];
	BGBCC_FrCC_LiteralInfo *st;
	char *s, *t;
	int pn, asz, bt;
	int i;

	if((ty.val&FR2C_TY_TYTY_MASK)==FR2C_TY_TYTY_BASIC)
	{
		pn=(ty.val&FR2C_TY_PTRMASK)>>FR2C_TY_PTRSHL;
		asz=(ty.val&FR2C_TY_ARRMASK)>>FR2C_TY_ARRSHL;
		bt=ty.val&FR2C_TY_BASEMASK;
		
		t=tb;
		if(asz)
		{
			sprintf(t, "A%d;", asz);
			t+=strlen(t);
		}
		if(pn)
		{
			i=pn;
			while(i--)*t++='P';
			*t=0;
		}
		
		if(bt<256)
		{
			switch(bt)
			{
			case FR2C_TY_I: *t++='i'; break;
			case FR2C_TY_L: *t++='x'; break;
			case FR2C_TY_F: *t++='f'; break;
			case FR2C_TY_D: *t++='d'; break;
			case FR2C_TY_P:
				*t++='P'; *t++='v'; break;
			case FR2C_TY_V: *t++='v'; break;
			case FR2C_TY_NL: *t++='l'; break;
			case FR2C_TY_SB: *t++='c'; break;
			case FR2C_TY_UB: *t++='h'; break;
			case FR2C_TY_SS: *t++='s'; break;
			case FR2C_TY_US: *t++='t'; break;
			case FR2C_TY_UI: *t++='j'; break;
			case FR2C_TY_UL: *t++='y'; break;
			case FR2C_TY_UNL: *t++='m'; break;

			case FR2C_TY_S: *t++='i'; break;
			default:
				break;
			}
			*t++=0;
		}else
		{
//			st=ctx->literals[bt-256];
			sprintf(t, "X%d", bt);
		}
	}
	
	return(bgbcc_strdup(tb));
}

frbc_type BGBCC_FrBC_TypeWrapBasicType(int ty)
{
	frbc_type tty;
	tty.val=ty;
	return(tty);
}


int BGBCC_FrBC_GetTypeConvMode(
	BGBCC_FrCC_State *ctx,
	frbc_type dty, frbc_type sty)
{
	if(BGBCC_FrBC_TypeIntP(ctx, sty))
	{
		if(BGBCC_FrBC_TypeLongP(ctx, dty))
			{ return(FR2C_OPCONV_I2L); }
		if(BGBCC_FrBC_TypeFloatP(ctx, dty))
			{ return(FR2C_OPCONV_I2F); }
		if(BGBCC_FrBC_TypeDoubleP(ctx, dty))
			{ return(FR2C_OPCONV_I2D); }
		if(BGBCC_FrBC_TypePointerP(ctx, dty))
			{ return(FR2C_OPCONV_I2P); }
		return(FR2C_STATUS_ERR_UNHANDLEDTYPE);
	}

	if(BGBCC_FrBC_TypeLongP(ctx, sty))
	{
		if(BGBCC_FrBC_TypeIntP(ctx, dty))
			{ return(FR2C_OPCONV_L2I); }
		if(BGBCC_FrBC_TypeFloatP(ctx, dty))
			{ return(FR2C_OPCONV_L2F); }
		if(BGBCC_FrBC_TypeDoubleP(ctx, dty))
			{ return(FR2C_OPCONV_L2D); }
		if(BGBCC_FrBC_TypePointerP(ctx, dty))
			{ return(FR2C_OPCONV_L2P); }
		return(FR2C_STATUS_ERR_UNHANDLEDTYPE);
	}

	if(BGBCC_FrBC_TypeFloatP(ctx, sty))
	{
		if(BGBCC_FrBC_TypeIntP(ctx, dty))
			{ return(FR2C_OPCONV_F2I); }
		if(BGBCC_FrBC_TypeLongP(ctx, dty))
			{ return(FR2C_OPCONV_F2L); }
		if(BGBCC_FrBC_TypeDoubleP(ctx, dty))
			{ return(FR2C_OPCONV_F2D); }
		return(FR2C_STATUS_ERR_UNHANDLEDTYPE);
	}

	if(BGBCC_FrBC_TypeDoubleP(ctx, sty))
	{
		if(BGBCC_FrBC_TypeIntP(ctx, dty))
			{ return(FR2C_OPCONV_D2I); }
		if(BGBCC_FrBC_TypeLongP(ctx, dty))
			{ return(FR2C_OPCONV_D2L); }
		if(BGBCC_FrBC_TypeFloatP(ctx, dty))
			{ return(FR2C_OPCONV_D2F); }
		return(FR2C_STATUS_ERR_UNHANDLEDTYPE);
	}

	if(BGBCC_FrBC_TypePointerP(ctx, sty))
	{
		if(BGBCC_FrBC_TypeIntP(ctx, dty))
			{ return(FR2C_OPCONV_P2I); }
		if(BGBCC_FrBC_TypeLongP(ctx, dty))
			{ return(FR2C_OPCONV_P2L); }
		return(FR2C_STATUS_ERR_UNHANDLEDTYPE);
	}

	return(FR2C_STATUS_ERR_UNHANDLEDTYPE);
}

int BGBCC_FrBC_GetTypeConvIMode(
	BGBCC_FrCC_State *ctx,
	frbc_type dty, frbc_type sty)
{
	int dt, st;

	if(!BGBCC_FrBC_TypeSmallIntP(ctx, dty))
		return(FR2C_STATUS_ERR_UNHANDLEDTYPE);
	if(!BGBCC_FrBC_TypeSmallIntP(ctx, sty))
		return(FR2C_STATUS_ERR_UNHANDLEDTYPE);
	return(BGBCC_FrBC_GetTypeConvILMode(ctx, dty, sty));
}		

int BGBCC_FrBC_GetTypeConvLMode(
	BGBCC_FrCC_State *ctx,
	frbc_type dty, frbc_type sty)
{
	int dt, st;

	if(!BGBCC_FrBC_TypeSmallLongP(ctx, dty))
		return(FR2C_STATUS_ERR_UNHANDLEDTYPE);
	if(!BGBCC_FrBC_TypeSmallLongP(ctx, sty))
		return(FR2C_STATUS_ERR_UNHANDLEDTYPE);
	return(BGBCC_FrBC_GetTypeConvILMode(ctx, dty, sty));
}		

int BGBCC_FrBC_GetTypeConvILMode(
	BGBCC_FrCC_State *ctx,
	frbc_type dty, frbc_type sty)
{
	int dt, st;

//	if(!BGBCC_FrBC_TypeSmallIntP(ctx, dty))
//		return(FR2C_STATUS_ERR_UNHANDLEDTYPE);
//	if(!BGBCC_FrBC_TypeSmallIntP(ctx, sty))
//		return(FR2C_STATUS_ERR_UNHANDLEDTYPE);
		
	dt=dty.val; st=sty.val;
	
	if(dt==FR2C_TY_I)
	{
		if(st==FR2C_TY_SB)
			{ return(FR2C_OPCONV2_SXB); }
		if(st==FR2C_TY_UB)
			{ return(FR2C_OPCONV2_ZXB); }
		if(st==FR2C_TY_SS)
			{ return(FR2C_OPCONV2_SXS); }
		if(st==FR2C_TY_US)
			{ return(FR2C_OPCONV2_ZXS); }
		if(st==FR2C_TY_I)
			{ return(FR2C_OPCONV2_SXI); }
		if(st==FR2C_TY_UI)
			{ return(FR2C_OPCONV2_ZXI); }
	}

	if(dt==FR2C_TY_UI)
	{
		if(st==FR2C_TY_SB)
			{ return(FR2C_OPCONV2_ZXB); }
		if(st==FR2C_TY_SS)
			{ return(FR2C_OPCONV2_ZXS); }
		if(st==FR2C_TY_I)
			{ return(FR2C_OPCONV2_ZXI); }

		if(st==FR2C_TY_UB)
			{ return(FR2C_OPCONV2_ZXB); }
		if(st==FR2C_TY_US)
			{ return(FR2C_OPCONV2_ZXS); }
		if(st==FR2C_TY_UI)
			{ return(FR2C_OPCONV2_ZXI); }
	}

	if((dt==FR2C_TY_L) || (dt==FR2C_TY_NL))
	{
		if(st==FR2C_TY_SB)
			{ return(FR2C_OPCONV2_SXB); }
		if(st==FR2C_TY_UB)
			{ return(FR2C_OPCONV2_ZXB); }
		if(st==FR2C_TY_SS)
			{ return(FR2C_OPCONV2_SXS); }
		if(st==FR2C_TY_US)
			{ return(FR2C_OPCONV2_ZXS); }
		if(st==FR2C_TY_I)
			{ return(FR2C_OPCONV2_SXI); }
		if(st==FR2C_TY_UI)
			{ return(FR2C_OPCONV2_ZXI); }
	}
	
	if(dt==FR2C_TY_SB)
		{ return(FR2C_OPCONV2_SXB); }
	if(dt==FR2C_TY_UB)
		{ return(FR2C_OPCONV2_ZXB); }

	if(dt==FR2C_TY_SS)
	{
		if(st==FR2C_TY_SB)
			{ return(FR2C_OPCONV2_SXB); }
		if(st==FR2C_TY_UB)
			{ return(FR2C_OPCONV2_ZXB); }
		return(FR2C_OPCONV2_SXS);
	}

	if(dt==FR2C_TY_US)
	{
		if(st==FR2C_TY_SB)
			{ return(FR2C_OPCONV2_ZXS); }
		if(st==FR2C_TY_UB)
			{ return(FR2C_OPCONV2_ZXB); }
		return(FR2C_OPCONV2_ZXS);
	}

	return(FR2C_STATUS_ERR_UNHANDLEDTYPE);
}

bool BGBCC_FrBC_TypeSupportsOperatorP(
	BGBCC_FrCC_State *ctx, frbc_type ty, int opr)
{
	if(BGBCC_FrBC_TypeSmallIntP(ctx, ty))
	{
		return(true);
	}

	if(BGBCC_FrBC_TypeLongP(ctx, ty))
	{
		return(true);
	}

	if(BGBCC_FrBC_TypeFloatP(ctx, ty))
	{
		if(opr==FR2C_BINOP_ADD)
			return(true);
		if(opr==FR2C_BINOP_SUB)
			return(true);
		if(opr==FR2C_BINOP_MUL)
			return(true);
		if(opr==FR2C_BINOP_DIV)
			return(true);
		return(false);
	}

	if(BGBCC_FrBC_TypeDoubleP(ctx, ty))
	{
		if(opr==FR2C_BINOP_ADD)
			return(true);
		if(opr==FR2C_BINOP_SUB)
			return(true);
		if(opr==FR2C_BINOP_MUL)
			return(true);
		if(opr==FR2C_BINOP_DIV)
			return(true);
		return(false);
	}

	if(BGBCC_FrBC_TypePointerP(ctx, ty))
	{
		if(opr==FR2C_BINOP_ADD)
			return(true);
		if(opr==FR2C_BINOP_SUB)
			return(true);
		return(false);
	}
	
	return(false);
}

frbc_status BGBCC_FrBC_GetTypeBinaryDest(
	BGBCC_FrCC_State *ctx, int opr,
	frbc_type lty, frbc_type rty,
	frbc_type *rdty)
{
	int ltyb, rtyb, dtyb;

	if(BGBCC_FrBC_TypeSmallIntP(ctx, lty) &&
		BGBCC_FrBC_TypeSmallIntP(ctx, rty))
	{
		ltyb=lty.val&FR2C_TY_BASEMASK;
		rtyb=rty.val&FR2C_TY_BASEMASK;
		dtyb=FR2C_TY_I;
		
		switch(ltyb)
		{
		case FR2C_TY_I:
			switch(rtyb)
			{
			case FR2C_TY_I:		dtyb=FR2C_TY_I; break;
			case FR2C_TY_UI:	dtyb=FR2C_TY_UI; break;
			default:			dtyb=FR2C_TY_I; break;
			}
			break;
		case FR2C_TY_UI:
			dtyb=FR2C_TY_UI; break;

		case FR2C_TY_SB:
			switch(rtyb)
			{
			case FR2C_TY_SB:	dtyb=FR2C_TY_SB; break;
			case FR2C_TY_UB:	dtyb=FR2C_TY_UB; break;
			case FR2C_TY_SS:	dtyb=FR2C_TY_SS; break;
			case FR2C_TY_US:	dtyb=FR2C_TY_US; break;
			case FR2C_TY_I:		dtyb=FR2C_TY_I; break;
			case FR2C_TY_UI:	dtyb=FR2C_TY_UI; break;
			default:			dtyb=FR2C_TY_I; break;
			}
			break;
		case FR2C_TY_UB:
			switch(rtyb)
			{
			case FR2C_TY_SB:	case FR2C_TY_UB:
				dtyb=FR2C_TY_UB; break;
			case FR2C_TY_SS:	dtyb=FR2C_TY_SS; break;
			case FR2C_TY_US:	dtyb=FR2C_TY_US; break;
			case FR2C_TY_I:		dtyb=FR2C_TY_I; break;
			case FR2C_TY_UI:	dtyb=FR2C_TY_UI; break;
			default:			dtyb=FR2C_TY_I; break;
			}
			break;

		case FR2C_TY_SS:
			switch(rtyb)
			{
			case FR2C_TY_SB: case FR2C_TY_UB:
			case FR2C_TY_SS:
				dtyb=FR2C_TY_SS; break;
			case FR2C_TY_US:	dtyb=FR2C_TY_US; break;
			case FR2C_TY_I:		dtyb=FR2C_TY_I; break;
			case FR2C_TY_UI:	dtyb=FR2C_TY_UI; break;
			default:			dtyb=FR2C_TY_I; break;
			}
			break;
		case FR2C_TY_US:
			switch(rtyb)
			{
			case FR2C_TY_SB:	case FR2C_TY_UB:
			case FR2C_TY_SS:	case FR2C_TY_US:
				dtyb=FR2C_TY_US; break;
			case FR2C_TY_I:		dtyb=FR2C_TY_I; break;
			case FR2C_TY_UI:	dtyb=FR2C_TY_UI; break;
			default:			dtyb=FR2C_TY_I; break;
			}
			break;
		default:			dtyb=FR2C_TY_I; break;
		}

		*rdty=BGBCC_FrBC_MakeTypeID(ctx, dtyb);
		return(BGBCC_FrBC_TypeSupportsOperatorP(ctx, *rdty, opr)?
			FR2C_STATUS_YES:FR2C_STATUS_NO);
	}

	if(BGBCC_FrBC_TypeSmallLongP(ctx, lty) &&
		BGBCC_FrBC_TypeSmallLongP(ctx, rty))
	{
		if(BGBCC_FrBC_TypeSmallIntP(ctx, lty))
		{
			*rdty=rty;
			return(BGBCC_FrBC_TypeSupportsOperatorP(ctx, *rdty, opr)?
				FR2C_STATUS_YES:FR2C_STATUS_NO);
		}

		if(BGBCC_FrBC_TypeSmallIntP(ctx, rty))
		{
			*rdty=lty;
			return(BGBCC_FrBC_TypeSupportsOperatorP(ctx, *rdty, opr)?
				FR2C_STATUS_YES:FR2C_STATUS_NO);
		}
		
		if(BGBCC_FrBC_TypeUnsignedLongP(ctx, lty))
		{
			*rdty=lty;
			return(BGBCC_FrBC_TypeSupportsOperatorP(ctx, *rdty, opr)?
				FR2C_STATUS_YES:FR2C_STATUS_NO);
		}

		if(BGBCC_FrBC_TypeUnsignedLongP(ctx, rty))
		{
			*rdty=rty;
			return(BGBCC_FrBC_TypeSupportsOperatorP(ctx, *rdty, opr)?
				FR2C_STATUS_YES:FR2C_STATUS_NO);
		}

		*rdty=BGBCC_FrBC_MakeTypeID(ctx, FR2C_TY_L);
		return(BGBCC_FrBC_TypeSupportsOperatorP(ctx, *rdty, opr)?
			FR2C_STATUS_YES:FR2C_STATUS_NO);
	}

	if(BGBCC_FrBC_TypeSmallIntP(ctx, lty))
	{
		if(BGBCC_FrBC_TypeSmallIntP(ctx, rty))
		{
			*rdty=BGBCC_FrBC_MakeTypeID(ctx, FR2C_TY_I);
			return(BGBCC_FrBC_TypeSupportsOperatorP(ctx, *rdty, opr)?
				FR2C_STATUS_YES:FR2C_STATUS_NO);
//			return(FR2C_STATUS_YES);
		}

		if(BGBCC_FrBC_TypeSmallLongP(ctx, rty))
		{
			*rdty=BGBCC_FrBC_MakeTypeID(ctx, FR2C_TY_L);
			return(BGBCC_FrBC_TypeSupportsOperatorP(ctx, *rdty, opr)?
				FR2C_STATUS_YES:FR2C_STATUS_NO);
//			return(FR2C_STATUS_YES);
		}

		if(BGBCC_FrBC_TypeFloatP(ctx, rty))
		{
			*rdty=BGBCC_FrBC_MakeTypeID(ctx, FR2C_TY_D);
			return(BGBCC_FrBC_TypeSupportsOperatorP(ctx, *rdty, opr)?
				FR2C_STATUS_YES:FR2C_STATUS_NO);
//			return(FR2C_STATUS_YES);
		}

		if(BGBCC_FrBC_TypeDoubleP(ctx, rty))
		{
			*rdty=BGBCC_FrBC_MakeTypeID(ctx, FR2C_TY_D);
			return(BGBCC_FrBC_TypeSupportsOperatorP(ctx, *rdty, opr)?
				FR2C_STATUS_YES:FR2C_STATUS_NO);
//			return(FR2C_STATUS_YES);
		}

//		if(BGBCC_FrBC_TypePointerP(ctx, rty))
//		{
//			*rdty=BGBCC_FrBC_MakeTypeID(ctx, FR2C_TY_L);
//			return(FR2C_STATUS_YES);
//		}

		BGBCC_FrBC_TagError(ctx,
			FR2C_TERR_STATUS(FR2C_STATUS_ERR_UNHANDLEDTYPE));
		return(FR2C_STATUS_ERR_UNHANDLEDTYPE);
	}

	if(BGBCC_FrBC_TypeSmallLongP(ctx, lty))
	{
		if(BGBCC_FrBC_TypeSmallIntP(ctx, rty))
		{
			*rdty=BGBCC_FrBC_MakeTypeID(ctx, FR2C_TY_L);
			return(BGBCC_FrBC_TypeSupportsOperatorP(ctx, *rdty, opr)?
				FR2C_STATUS_YES:FR2C_STATUS_NO);
//			return(FR2C_STATUS_YES);
		}

		if(BGBCC_FrBC_TypeSmallLongP(ctx, rty))
		{
			*rdty=BGBCC_FrBC_MakeTypeID(ctx, FR2C_TY_L);
			return(BGBCC_FrBC_TypeSupportsOperatorP(ctx, *rdty, opr)?
				FR2C_STATUS_YES:FR2C_STATUS_NO);
//			return(FR2C_STATUS_YES);
		}

		if(BGBCC_FrBC_TypeFloatP(ctx, rty))
		{
			*rdty=BGBCC_FrBC_MakeTypeID(ctx, FR2C_TY_D);
			return(BGBCC_FrBC_TypeSupportsOperatorP(ctx, *rdty, opr)?
				FR2C_STATUS_YES:FR2C_STATUS_NO);
//			return(FR2C_STATUS_YES);
		}

		if(BGBCC_FrBC_TypeDoubleP(ctx, rty))
		{
			*rdty=BGBCC_FrBC_MakeTypeID(ctx, FR2C_TY_D);
			return(BGBCC_FrBC_TypeSupportsOperatorP(ctx, *rdty, opr)?
				FR2C_STATUS_YES:FR2C_STATUS_NO);
//			return(FR2C_STATUS_YES);
		}

//		if(BGBCC_FrBC_TypePointerP(ctx, dty))
//			{ return(FR2C_OPCONV_L2P); }
		BGBCC_FrBC_TagError(ctx,
			FR2C_TERR_STATUS(FR2C_STATUS_ERR_UNHANDLEDTYPE));
		return(FR2C_STATUS_ERR_UNHANDLEDTYPE);
	}

	if(BGBCC_FrBC_TypeFloatP(ctx, lty))
	{
		if(BGBCC_FrBC_TypeSmallIntP(ctx, rty))
		{
			*rdty=BGBCC_FrBC_MakeTypeID(ctx, FR2C_TY_D);
			return(BGBCC_FrBC_TypeSupportsOperatorP(ctx, *rdty, opr)?
				FR2C_STATUS_YES:FR2C_STATUS_NO);
//			return(FR2C_STATUS_YES);
		}

		if(BGBCC_FrBC_TypeSmallLongP(ctx, rty))
		{
			*rdty=BGBCC_FrBC_MakeTypeID(ctx, FR2C_TY_D);
			return(BGBCC_FrBC_TypeSupportsOperatorP(ctx, *rdty, opr)?
				FR2C_STATUS_YES:FR2C_STATUS_NO);
//			return(FR2C_STATUS_YES);
		}

		if(BGBCC_FrBC_TypeFloatP(ctx, rty))
		{
			*rdty=BGBCC_FrBC_MakeTypeID(ctx, FR2C_TY_F);
			return(BGBCC_FrBC_TypeSupportsOperatorP(ctx, *rdty, opr)?
				FR2C_STATUS_YES:FR2C_STATUS_NO);
//			return(FR2C_STATUS_YES);
		}

		if(BGBCC_FrBC_TypeDoubleP(ctx, rty))
		{
			*rdty=BGBCC_FrBC_MakeTypeID(ctx, FR2C_TY_D);
			return(BGBCC_FrBC_TypeSupportsOperatorP(ctx, *rdty, opr)?
				FR2C_STATUS_YES:FR2C_STATUS_NO);
//			return(FR2C_STATUS_YES);
		}

		BGBCC_FrBC_TagError(ctx,
			FR2C_TERR_STATUS(FR2C_STATUS_ERR_UNHANDLEDTYPE));
		return(FR2C_STATUS_ERR_UNHANDLEDTYPE);
	}

	if(BGBCC_FrBC_TypeDoubleP(ctx, lty))
	{
		if(BGBCC_FrBC_TypeSmallIntP(ctx, rty))
		{
			*rdty=BGBCC_FrBC_MakeTypeID(ctx, FR2C_TY_D);
			return(BGBCC_FrBC_TypeSupportsOperatorP(ctx, *rdty, opr)?
				FR2C_STATUS_YES:FR2C_STATUS_NO);
//			return(FR2C_STATUS_YES);
		}

		if(BGBCC_FrBC_TypeSmallLongP(ctx, rty))
		{
			*rdty=BGBCC_FrBC_MakeTypeID(ctx, FR2C_TY_D);
			return(BGBCC_FrBC_TypeSupportsOperatorP(ctx, *rdty, opr)?
				FR2C_STATUS_YES:FR2C_STATUS_NO);
//			return(FR2C_STATUS_YES);
		}

		if(BGBCC_FrBC_TypeFloatP(ctx, rty))
		{
			*rdty=BGBCC_FrBC_MakeTypeID(ctx, FR2C_TY_D);
			return(BGBCC_FrBC_TypeSupportsOperatorP(ctx, *rdty, opr)?
				FR2C_STATUS_YES:FR2C_STATUS_NO);
//			return(FR2C_STATUS_YES);
		}

		if(BGBCC_FrBC_TypeDoubleP(ctx, rty))
		{
			*rdty=BGBCC_FrBC_MakeTypeID(ctx, FR2C_TY_D);
			return(BGBCC_FrBC_TypeSupportsOperatorP(ctx, *rdty, opr)?
				FR2C_STATUS_YES:FR2C_STATUS_NO);
//			return(FR2C_STATUS_YES);
		}

		BGBCC_FrBC_TagError(ctx,
			FR2C_TERR_STATUS(FR2C_STATUS_ERR_UNHANDLEDTYPE));
		return(FR2C_STATUS_ERR_UNHANDLEDTYPE);
	}

	if(BGBCC_FrBC_TypePointerP(ctx, lty))
	{
		if(BGBCC_FrBC_TypeSmallIntP(ctx, rty))
		{
			*rdty=lty;
			return(BGBCC_FrBC_TypeSupportsOperatorP(ctx, *rdty, opr)?
				FR2C_STATUS_YES:FR2C_STATUS_NO);
//			return(FR2C_STATUS_YES);
		}

		if(BGBCC_FrBC_TypeSmallLongP(ctx, rty))
		{
			*rdty=lty;
			return(BGBCC_FrBC_TypeSupportsOperatorP(ctx, *rdty, opr)?
				FR2C_STATUS_YES:FR2C_STATUS_NO);
//			return(FR2C_STATUS_YES);
		}

		BGBCC_FrBC_TagError(ctx,
			FR2C_TERR_STATUS(FR2C_STATUS_ERR_UNHANDLEDTYPE));
		return(FR2C_STATUS_ERR_UNHANDLEDTYPE);
	}

	BGBCC_FrBC_TagError(ctx,
		FR2C_TERR_STATUS(FR2C_STATUS_ERR_UNHANDLEDTYPE));
	return(FR2C_STATUS_ERR_UNHANDLEDTYPE);
}
