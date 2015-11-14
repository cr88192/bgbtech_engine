#include <bgbfrir2.h>

int BGBFR2_LoadObj_DecodeStringTable(
	FR2_ObjectModule *obj, byte *buf, int szBuf)
{
	obj->strtab=gcalloc(szBuf+1);
	memcpy(obj->strtab, buf, szBuf);
	obj->sz_strtab=szBuf;
	obj->msz_strtab=szBuf;
	return(0);
}

int BGBFR2_LoadObj_DecodeBlobTable(
	FR2_ObjectModule *obj, byte *buf, int szBuf)
{
	obj->blobtab=gcalloc(szBuf);
	memcpy(obj->blobtab, buf, szBuf);
	obj->sz_blobtab=szBuf;
	obj->msz_blobtab=szBuf;
	return(0);
}

int BGBFR2_LoadObj_DecodeConstantTable(
	FR2_ObjectModule *obj, byte *buf, int szBuf)
{
	int *ctb;
	FR2_Value **ctv;
	byte *cs, *cse;
	int i, j, k, l, n, m;
	
	obj->ctab_buf=gcalloc(szBuf);
	memcpy(obj->ctab_buf, buf, szBuf);
	obj->sz_ctab=szBuf;
	obj->msz_ctab=szBuf;

	n=0; m=64;
	ctb=gcalloc(m*sizeof(int));
	ctv=gcalloc(m*sizeof(FR2_Value *));
	ctv[n]=NULL;
	ctb[n++]=0;
	
	cs=buf; cse=buf+szBuf;
	while(cs<cse)
	{
		if((n+1)>=m)
		{
			m=m+(m>>1);
			ctb=gcrealloc(ctb, m*sizeof(int));
			ctv=gcrealloc(ctv, m*sizeof(FR2_Value *));
		}
		
		if(*cs==0xE0)
			break;

		if(*cs==0xE1)
		{
			l=(cs[1]<<16)|(cs[2]<<8)|(cs[3]);
			cs+=l; continue;
		}	

		if(*cs==0xE3)
		{
			l=(cs[1]<<16)|(cs[2]<<8)|(cs[3]);
			k=cs[4]|(cs[5]<<8);
			ctv[n]=BGBFR2_LoadObj_DecodeValueTwocc(obj, k, cs+6, l-6);
			ctb[n++]=cs-buf;
			cs+=l; continue;
		}

		if(*cs==0xE4)
		{
			l=(cs[1]<<16)|(cs[2]<<8)|(cs[3]);
			ctb[n++]=cs-buf;
			cs+=l; continue;
		}

		if(*cs==0xE5)
		{
			l=cs[1];
			k=cs[2]|(cs[3]<<8);
			ctv[n]=BGBFR2_LoadObj_DecodeValueTwocc(obj, k, cs+4, l-4);
			ctb[n++]=cs-buf;
			cs+=l; continue;
		}
	}
	obj->ctab_idx=ctb;
	obj->ctab_val=ctv;
	obj->n_ctab=n;
	obj->m_ctab=m;

	return(0);
}

int BGBFR2_LoadObj_CheckExpandVar(FR2_ObjectModule *obj)
{
	int i, j, k, n;

	if(!obj->svar)
	{
		n=64;
		obj->svar=gcalloc(n*sizeof(FR2_StaticVarInfo *));
		obj->n_svar=0;
		obj->m_svar=n;
	}
	
	if((obj->n_svar+1)>=obj->m_svar)
	{
		n=obj->m_svar; n=n+(n>>1);
		obj->svar=gcrealloc(obj->svar, n*sizeof(FR2_StaticVarInfo *));
		obj->m_svar=n;
	}
	
	return(0);
}

int BGBFR2_LoadObj_CheckExpandFunc(FR2_ObjectModule *obj)
{
	int i, j, k, n;

	if(!obj->svar)
	{
		n=64;
		obj->sfunc=gcalloc(n*sizeof(FR2_Function *));
		obj->n_sfunc=0;
		obj->m_sfunc=n;
	}
	
	if((obj->n_sfunc+1)>=obj->m_sfunc)
	{
		n=obj->m_sfunc; n=n+(n>>1);
		obj->sfunc=gcrealloc(obj->sfunc, n*sizeof(FR2_Function *));
		obj->m_sfunc=n;
	}
	
	return(0);
}

FR2_Value *BGBFR2_LoadObj_DecodeValueTwocc(FR2_ObjectModule *obj,
	int tcc, byte *buf, int szBuf)
{
	FR2_Value *val;
	int i, j, k;

	if(tcc==BGBFR2_TWOCC('I', '4'))
	{
		val=gcalloc(sizeof(FR2_Value));
		val->i=(buf[0]<<24)|(buf[1]<<16)|(buf[2]<<8)|(buf[3]);
	}else if(tcc==BGBFR2_TWOCC('I', '8'))
	{
		val=gcalloc(sizeof(FR2_Value));
		val->l=
			(((s64)buf[0])<<56)|(((s64)buf[1])<<48)|
			(((s64)buf[2])<<40)|(((s64)buf[3])<<32)|
			(((s64)buf[4])<<24)|(((s64)buf[5])<<16)|
			(((s64)buf[6])<< 8)|(((s64)buf[7])    );
	}else if(tcc==BGBFR2_TWOCC('F', '4'))
	{
		val=gcalloc(sizeof(FR2_Value));
		val->i=(buf[0]<<24)|(buf[1]<<16)|(buf[2]<<8)|(buf[3]);
	}else if(tcc==BGBFR2_TWOCC('F', '8'))
	{
		val=gcalloc(sizeof(FR2_Value));
		val->l=
			(((s64)buf[0])<<56)|(((s64)buf[1])<<48)|
			(((s64)buf[2])<<40)|(((s64)buf[3])<<32)|
			(((s64)buf[4])<<24)|(((s64)buf[5])<<16)|
			(((s64)buf[6])<< 8)|(((s64)buf[7])    );
	}else if(tcc==BGBFR2_TWOCC('S', 'I'))
	{
		val=gcalloc(sizeof(FR2_Value));
		i=(buf[0]<<24)|(buf[1]<<16)|(buf[2]<<8)|(buf[3]);
		tvar->a=dystrdup(obj->strtab+i);
	}
	
	return(val);
}

FR2_Value *BGBFR2_LoadObj_ValueForCtabIndex(
	FR2_ObjectModule *obj, int idx)
{
	byte *cs, *cs1;
	FR2_Value *val;
	int i, j, k;
	
	cs=obj->ctab_buf+obj->ctab_idx[idx];
	val=NULL;

	if(*cs==0xE5)
	{
		l=cs[1];
		k=cs[2]|(cs[3]<<8);
		val=BGBFR2_LoadObj_DecodeValueTwocc(obj, k, cs+4, l-4);
	}
}

FR2_Value *BGBFR2_LoadObj_CachedValueForCtabIndex(
	FR2_ObjectModule *obj, int idx)
{
	FR2_Value *val;

	if(obj->ctab_val[idx])
		return(obj->ctab_val[idx]);
		
	val=BGBFR2_LoadObj_ValueForCtabIndex(obj, idx);
	obj->ctab_val[idx]=val;
	return(val);
}

int BGBFR2_LoadObj_DecodeMarkerVarTwocc(
	FR2_ObjectModule *obj, FR2_StaticVarInfo *tvar, int tcc,
	byte *buf, int szBuf)
{
//	if(tcc==BGBFR2_TWOCC('S', 'T'))
//		{ return(BGBFR2_LoadObj_DecodeStringTable(obj, buf, szBuf)); }
//	if(tcc==BGBFR2_TWOCC('B', 'T'))
//		{ return(BGBFR2_LoadObj_DecodeBlobTable(obj, buf, szBuf)); }
//	if(tcc==BGBFR2_TWOCC('C', 'T'))
//		{ return(BGBFR2_LoadObj_DecodeConstantTable(obj, buf, szBuf)); }
	
	return(-1);
}

int BGBFR2_LoadObj_DecodeMarkerFuncTwocc(
	FR2_ObjectModule *obj, FR2_Function *tfn, int tcc,
	byte *buf, int szBuf)
{
//	if(tcc==BGBFR2_TWOCC('S', 'T'))
//		{ return(BGBFR2_LoadObj_DecodeStringTable(obj, buf, szBuf)); }
//	if(tcc==BGBFR2_TWOCC('B', 'T'))
//		{ return(BGBFR2_LoadObj_DecodeBlobTable(obj, buf, szBuf)); }
//	if(tcc==BGBFR2_TWOCC('C', 'T'))
//		{ return(BGBFR2_LoadObj_DecodeConstantTable(obj, buf, szBuf)); }
	
	return(-1);
}

int BGBFR2_LoadObj_DecodeDefVar(
	FR2_ObjectModule *obj, byte *buf, int szBuf)
{
	FR2_StaticVarInfo *tvar;
	int i, j, k, n;
	
	BGBFR2_LoadObj_CheckExpandVar(obj);
	
	tvar=gcalloc(sizeof(FR2_StaticVarInfo));
	
	i=obj->n_svar++;
	obj->svar[i]=tvar;
	
	i=(buf[0]<<24)|(buf[1]<<16)|(buf[2]<<8)|(buf[3]);
	tvar->name=obj->strtab+i;

	i=(buf[4]<<24)|(buf[5]<<16)|(buf[6]<<8)|(buf[7]);
	tvar->sig=obj->strtab+i;

	i=(buf[8]<<24)|(buf[9]<<16)|(buf[10]<<8)|(buf[11]);
	tvar->flags=obj->strtab+i;

	i=(buf[12]<<24)|(buf[13]<<16)|(buf[14]<<8)|(buf[15]);
	if(i)
		{ tvar->ptr=BGBFR2_LoadObj_ValueForCtabIndex(obj, i); }

	cs=buf+16; cse=buf+szBuf;
	while(cs<cse)
	{
		if(*cs==0xE0)
			break;

		if(*cs==0xE1)
		{
			l=(cs[1]<<16)|(cs[2]<<8)|(cs[3]);
			cs+=l; continue;
		}	

		if(*cs==0xE2)
		{
			k=cs[1];
			l=cs[2]|(cs[3]<<8);
			cs+=l; continue;
		}

		if(*cs==0xE3)
		{
			l=(cs[1]<<16)|(cs[2]<<8)|(cs[3]);
			k=cs[4]|(cs[5]<<8);
			BGBFR2_LoadObj_DecodeMarkerVarTwocc(obj, tvar, k, cs+6, l-6);
			cs+=l; continue;
		}

		if(*cs==0xE4)
		{
			l=(cs[1]<<16)|(cs[2]<<8)|(cs[3]);
			cs+=l; continue;
		}

		if(*cs==0xE5)
		{
			l=cs[1];
			k=cs[2]|(cs[3]<<8);
			BGBFR2_LoadObj_DecodeMarkerVarTwocc(obj, tvar, k, cs+4, l-4);
			cs+=l; continue;
		}
	}
}

int BGBFR2_LoadObj_DecodeDefFunc(
	FR2_ObjectModule *obj, byte *buf, int szBuf)
{
	FR2_Function *tfn;
	int i, j, k, n;
	
	BGBFR2_LoadObj_CheckExpandFunc(obj);
	
	tvar=gcalloc(sizeof(FR2_Function));
	
	i=obj->n_sfunc++;
	obj->sfunc[i]=tfn;
	
	i=(buf[0]<<24)|(buf[1]<<16)|(buf[2]<<8)|(buf[3]);
	tfn->name=obj->strtab+i;

	i=(buf[4]<<24)|(buf[5]<<16)|(buf[6]<<8)|(buf[7]);
	tfn->sig=obj->strtab+i;

	i=(buf[8]<<24)|(buf[9]<<16)|(buf[10]<<8)|(buf[11]);
	tfn->flags=obj->strtab+i;

	i=(buf[12]<<24)|(buf[13]<<16)|(buf[14]<<8)|(buf[15]);

	cs=obj->blobtab+i;
	if(*cs==0xE5)
	{
		l=cs[1];
		k=cs[2]|(cs[3]<<8);

		cs1=cs+4;		

		if(k==BGBFR2_TWOCC('B', 'C'))
		{
			tfn->ip=cs1;
			tfn->sz_ip=l-4;
		}
	}else if(*cs==0xE3)
	{
		l=(cs[1]<<16)|(cs[2]<<8)|(cs[3]);
		k=cs[4]|(cs[5]<<8);

		cs1=cs+6;

		if(k==BGBFR2_TWOCC('B', 'C'))
		{
			tfn->ip=cs1;
			tfn->sz_ip=l-6;
		}
	}

	cs=buf+16; cse=buf+szBuf;
	while(cs<cse)
	{
		if(*cs==0xE0)
			break;

		if(*cs==0xE1)
		{
			l=(cs[1]<<16)|(cs[2]<<8)|(cs[3]);
			cs+=l; continue;
		}	

		if(*cs==0xE2)
		{
			k=cs[1];
			l=cs[2]|(cs[3]<<8);
			cs+=l; continue;
		}

		if(*cs==0xE3)
		{
			l=(cs[1]<<16)|(cs[2]<<8)|(cs[3]);
			k=cs[4]|(cs[5]<<8);
			
			BGBFR2_LoadObj_DecodeMarkerFuncTwocc(obj, tfn, k, cs+6, l-6);
			cs+=l; continue;
		}

		if(*cs==0xE4)
		{
			l=(cs[1]<<16)|(cs[2]<<8)|(cs[3]);
			cs+=l; continue;
		}

		if(*cs==0xE5)
		{
			l=cs[1];
			k=cs[2]|(cs[3]<<8);

			BGBFR2_LoadObj_DecodeMarkerFuncTwocc(obj, tfn, k, cs+4, l-4);
			cs+=l; continue;
		}
	}

	return(0);
}

int BGBFR2_LoadObj_DecodeMarkerTwocc(
	FR2_ObjectModule *obj, int tcc,
	byte *buf, int szBuf)
{
	if(tcc==BGBFR2_TWOCC('S', 'T'))
		{ return(BGBFR2_LoadObj_DecodeStringTable(obj, buf, szBuf)); }
	if(tcc==BGBFR2_TWOCC('B', 'T'))
		{ return(BGBFR2_LoadObj_DecodeBlobTable(obj, buf, szBuf)); }
	if(tcc==BGBFR2_TWOCC('C', 'T'))
		{ return(BGBFR2_LoadObj_DecodeConstantTable(obj, buf, szBuf)); }
	
	return(-1);
}

FR2_ObjectModule *BGBFR2_LoadObj_DecodeObjectBuffer(byte *buf, int szBuf)
{
	FR2_ObjectModule *obj;
	byte *cs, *cse;
	int i, j, k, l;
	
	obj=gctalloc("bgbfr2_objectmodule_t", sizeof(FR2_ObjectModule));
	
	cs=buf; cse=buf+szBuf;
	while(cs<cse)
	{
		if(*cs==0xE0)
			break;

		if(*cs==0xE1)
		{
			l=(cs[1]<<16)|(cs[2]<<8)|(cs[3]);
//			BTIC2C_MarkerImgData(ctx, cs+4, l-4, obuf, clrs);
			cs+=l; continue;
		}	

		if(*cs==0xE3)
		{
			l=(cs[1]<<16)|(cs[2]<<8)|(cs[3]);
			k=cs[4]|(cs[5]<<8);
			
			BGBFR2_LoadObj_DecodeMarkerTwocc(obj, k, cs+6, l-6);
			
//			if((cs[4]=='Q') && (cs[5]>='0') && (cs[5]<='9'))
//			{	BTIC2C_MarkerDQT(ctx, cs+6, cs[5]-'0', l-6);
//				cs+=l; continue; }
			cs+=l; continue;
		}

		if(*cs==0xE4)
		{
			l=(cs[1]<<16)|(cs[2]<<8)|(cs[3]);
			cs+=l; continue;
		}

		if(*cs==0xE5)
		{
			l=cs[1];
			k=cs[2]|(cs[3]<<8);

			BGBFR2_LoadObj_DecodeMarkerTwocc(obj, k, cs+4, l-4);
			
//			if((cs[2]=='Q') && (cs[3]>='0') && (cs[3]<='9'))
//			{	BTIC2C_MarkerDQT(ctx, cs+4, cs[3]-'0', l-4);
//				cs+=l; continue; }
			cs+=l; continue;
		}
	}
}