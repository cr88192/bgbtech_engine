#include <bgbfrir2.h>

FR2A_Object *BGBFR2A_AllocObject(void)
{
	FR2A_Object *tmp;
	
	tmp=gctalloc("fr2a_object_t", sizeof(FR2A_Object));
	return(tmp);
}

char *BGBFR2A_Intern(FR2A_Object *obj, char *str)
{
	char *cs, *cse;
	int i;
	
	for(i=0; i<obj->n_strtab; i++)
	{
		cs=obj->strtab_buf[i];
		cse=obj->strtab_end[i];
		while(cs<cse)
		{
			if(!strcmp(cs, str))
				{ return(cs); }
			cs+=strlen(cs);
		}
	}
	
	if(!obj->str_s || ((obj->str_c+(strlen(str)+1))>obj->str_e))
	{
		i=obj->n_strtab++;
		obj->strtab_buf[i]=malloc(1<<16);
		obj->strtab_end[i]=obj->strtab_buf[i];
		
		obj->str_s=obj->strtab_buf[i];
		obj->str_e=obj->str_s+(1<<16);
		obj->str_c=obj->str_s;
	}
	
	cs=obj->str_c;
	strcpy(obj->str_c, str);
	obj->str_c+=strlen(str)+1;
	obj->strtab_end[obj->n_strtab-1]=obj->str_c;
	
	return(cs);
}

int BGBFR2A_LookupCTabInt(FR2A_Object *obj, s32 val)
{
	int i, j, k;

	for(i=1; i<obj->n_ctab; i++)
	{
		if((obj->ctab_tag[i]==BGBFR2_TWOCC_I4) &&
			(obj->ctab_val[i].i==val))
				return(i);
	}
	return(-1);
}

int BGBFR2A_LookupCTabLong(FR2A_Object *obj, s64 val)
{
	int i, j, k;

	for(i=1; i<obj->n_ctab; i++)
	{
		if((obj->ctab_tag[i]==BGBFR2_TWOCC_I8) &&
			(obj->ctab_val[i].l==val))
				return(i);
	}
	return(-1);
}

int BGBFR2A_LookupCTabFloat(FR2A_Object *obj, f32 val)
{
	int i, j, k;

	for(i=1; i<obj->n_ctab; i++)
	{
		if((obj->ctab_tag[i]==BGBFR2_TWOCC_F4) &&
			(obj->ctab_val[i].f==val))
				return(i);
	}
	return(-1);
}

int BGBFR2A_LookupCTabDouble(FR2A_Object *obj, f64 val)
{
	int i, j, k;

	for(i=1; i<obj->n_ctab; i++)
	{
		if((obj->ctab_tag[i]==BGBFR2_TWOCC_F8) &&
			(obj->ctab_val[i].d==val))
				return(i);
	}
	return(-1);
}

void BGBFR2A_CheckExpandCtab(FR2A_Object *obj)
{
	int i, j, k;

	if(!obj->ctab_val)
	{
		i=64;
		obj->ctab_tag=gcalloc(i*sizeof(int));
		obj->ctab_val=gcalloc(i*sizeof(FR2_Value));
		obj->n_ctab=1;
		obj->m_ctab=i;
	}
	
	if((obj->n_ctab+1)>=obj->m_ctab)
	{
		i=obj->m_ctab; i=i+(i>>1);
		obj->ctab_tag=gcrealloc(obj->ctab_tag, i*sizeof(int));
		obj->ctab_val=gcrealloc(obj->ctab_val, i*sizeof(FR2_Value));
		obj->m_ctab=i;
	}
}

int BGBFR2A_IndexCTabInt(FR2A_Object *obj, s32 val)
{
	int i, j, k;

	i=BGBFR2A_LookupCTabInt(obj, val);
	if(i>=0)return(i);
	
	BGBFR2A_CheckExpandCtab(obj);
	i=obj->n_ctab++;
	obj->ctab_tag[i]=BGBFR2_TWOCC_I4;
	obj->ctab_val[i].i=val;
	return(i);
}

int BGBFR2A_IndexCTabLong(FR2A_Object *obj, s64 val)
{
	int i, j, k;

	i=BGBFR2A_LookupCTabLong(obj, val);
	if(i>=0)return(i);
	
	BGBFR2A_CheckExpandCtab(obj);
	i=obj->n_ctab++;
	obj->ctab_tag[i]=BGBFR2_TWOCC_I8;
	obj->ctab_val[i].l=val;
	return(i);
}

int BGBFR2A_IndexCTabFloat(FR2A_Object *obj, f32 val)
{
	int i, j, k;

	i=BGBFR2A_LookupCTabFloat(obj, val);
	if(i>=0)return(i);
	
	BGBFR2A_CheckExpandCtab(obj);
	i=obj->n_ctab++;
	obj->ctab_tag[i]=BGBFR2_TWOCC_F4;
	obj->ctab_val[i].f=val;
	return(i);
}

int BGBFR2A_IndexCTabDouble(FR2A_Object *obj, f64 val)
{
	int i, j, k;

	i=BGBFR2A_LookupCTabDouble(obj, val);
	if(i>=0)return(i);
	
	BGBFR2A_CheckExpandCtab(obj);
	i=obj->n_ctab++;
	obj->ctab_tag[i]=BGBFR2_TWOCC_F8;
	obj->ctab_val[i].d=val;
	return(i);
}

int BGBFR2A_LookupCTabString(FR2A_Object *obj, char *str)
{
	int i, j, k;

	for(i=1; i<obj->n_ctab; i++)
	{
		if((obj->ctab_tag[i]==BGBFR2_TWOCC_SI) &&
			!strcmp(obj->ctab_val[i].a, str))
				return(i);
	}
	return(-1);
}

int BGBFR2A_IndexCTabString(FR2A_Object *obj, char *str)
{
	int i, j, k;

	i=BGBFR2A_LookupCTabString(obj, str);
	if(i>=0)return(i);
	
	BGBFR2A_CheckExpandCtab(obj);
	i=obj->n_ctab++;
	obj->ctab_tag[i]=BGBFR2_TWOCC_SI;
	obj->ctab_val[i].a=BGBFR2A_Intern(obj, str);
	return(i);
}

int BGBFR2A_ParseDeclVar(FR2A_Object *obj, char **rcs,
	char **rname, char **rsig)
{
	char tb1[256], tb2[256];
	char *ns, *ss;
	char *cs, *cs1, *cse;
	int ty1, ty2;
	
	cs=*rcs;
	cs1=BGBFR2_AsmParse_Token(cs, tb1, &ty1);
	if(ty1!=BGBFR2_TOKEN_NAME)
	{
		*rname=NULL;
		*rsig=NULL;
		return(-1);
	}

	ns=BGBFR2A_Intern(obj, tb1);
	ss=NULL;

	cs=cs1;
	cs1=BGBFR2_AsmParse_Token(cs, tb2, &ty2);
	if(!strcmp(tb2, ":"))
	{
		cs1=BGBFR2_AsmParse_Token(cs1, tb2, &ty2);
		ss=BGBFR2A_Intern(obj, tb1);
		cs=cs1;
	}
	
	*rname=ns;
	*rsig=ss;
	return(0);
}

int BGBFR2A_ParseInstrName(FR2A_Object *obj, char **rcs,
	char **rname, char **rsig)
{
	char tb1[256], tb2[256];
	char *ns, *ss;
	char *cs, *cs1, *cse;
	int ty1, ty2;
	
	cs=*rcs;
	cs1=BGBFR2_AsmParse_Token(cs, tb1, &ty1);
	if(ty1!=BGBFR2_TOKEN_NAME)
	{
		*rname=NULL;
		*rsig=NULL;
		return(-1);
	}

	ns=dyllStrdup(tb1);
	ss=NULL;

	cs=cs1;
	cs1=BGBFR2_AsmParse_Token(cs, tb2, &ty2);
	if(!strcmp(tb2, "."))
	{
		cs1=BGBFR2_AsmParse_Token(cs1, tb2, &ty2);
		ss=dyllStrdup(tb1);
		cs=cs1;
	}
	
	*rname=ns;
	*rsig=ss;
	return(0);
}

int BGBFR2A_IndexNameList(char *str, char **lst)
{
	int i;
	
	for(i=0; lst[i]; i++)
		if(!strcmp(str, lst[i]))
			return(i);
	return(-1);
}

static char *bgbfr2a_jop_names[]={
"jmp_eq",		"jmp_ne",		"jmp_lt",	"jmp_gt",	//24-27
"jmp_le",		"jmp_ge",		"jmp_un",	"jmp_tr",	//28-31
"jmp2_eq",		"jmp2_ne",		"jmp2_lt",	"jmp2_gt",	//24-27
"jmp2_le",		"jmp2_ge",		"jmp2_un",	"jmp2_tr",	//28-31
NULL};

static char *bgbfr2a_uop_names[]={
"mov", "neg", 	"not", 		"lnot",
"inc", "dec", "defer", 	"setdefer",
NULL};

static char *bgbfr2a_bop_names[]={
"add",			"sub",			"mul",		"div",		//0-3
"mod",			"and",			"or",		"xor",		//4-7
"shl",			"shr",			"shrr",		"cmpl",		//8-11
"cmpg",			"land",			"lor",		"lxor",		//12-15
"cmp_eq",		"cmp_ne",		"cmp_lt",	"cmp_gt",	//16-19
"cmp_le",		"cmp_ge",		"cmp_un",	"cmp_tr",	//20-23
"jmp_eq",		"jmp_ne",		"jmp_lt",	"jmp_gt",	//24-27
"jmp_le",		"jmp_ge",		"jmp_un",	"jmp_tr",	//28-31
"loadindex",	"storeindex",
NULL};

static char *bgbfr2a_pfx_names[]={
"z",	"i",	"l",	"f",	"d",	"q",	"p",	"c",
"sb",	"ub",	"ss",	"us",	"v",	"s2",	"s3",	"s4",
"v2f",	"v3f",	"v4f",	"v2d",	"v3d",	"v4d",	"v2i",	"v3i",
"v4i",	"v2b",	"v3b",	"v4b",	"v2s",	"v3s",	"v4s",	"-",
"ic",	"lc",	"fc",	"dc",
NULL};

int BGBFR2A_AssembleDecodeRegisterName(FR2A_Object *obj, char *str)
{
	int i;

	if((*str>='0') && (*str<='9'))
	{
		return(bgbgc_atoi(str));
	}
	
	for(i=0; i<obj->cur_func->n_arg; i++)
		if(!strcmp(str, obj->cur_func->arg_name[i]))
			{ return(i); }
	for(i=0; i<obj->cur_func->n_var; i++)
		if(!strcmp(str, obj->cur_func->var_name[i]))
			{ return(obj->cur_func->n_arg+i); }

	if(*str=='R')
		{ return(bgbgc_atoi(str+1)); }
	if(*str=='A')
		{ return(bgbgc_atoi(str+1)); }
	if(*str=='V')
		{ return(obj->cur_func->n_arg+bgbgc_atoi(str+1)); }

	if(*str=='T')
	{
		return(obj->cur_func->n_arg+obj->cur_func->n_var+
			bgbgc_atoi(str+1));
	}
	return(-1);
}

int BGBFR2A_AssembleDecodeLiteralPfx(FR2A_Object *obj, int pfx, char *str)
{
	if((pfx==FR2_PFX_I) || (pfx==FR2_PFX_L) ||
		(pfx==FR2_PFX_IC) || (pfx==FR2_PFX_LC))
	{
		return(bgbgc_atoi(str));
	}
	
	if(pfx==FR2_PFX_F)
	{
		i=BGBFR2A_IndexCTabFloat(obj, atof(str));
		return(i);
	}

	if(pfx==FR2_PFX_D)
	{
		i=BGBFR2A_IndexCTabDouble(obj, atof(str));
		return(i);
	}
	
	return(-1);
}

int BGBFR2A_AssembleDecodeLiteralSigCtab(FR2A_Object *obj,
	char *sig, char *str)
{
	int i, j, k;

	if((*sig=='i') || (*sig=='j'))
	{
		i=BGBFR2A_IndexCTabInt(obj, bgbgc_atoi(str));
		return(i);
	}

	if((*sig=='l') || (*sig=='x') || (*sig=='y'))
	{
		i=BGBFR2A_IndexCTabLong(obj, bgbgc_atoi(str));
		return(i);
	}

	if(*sig=='f')
	{
		i=BGBFR2A_IndexCTabFloat(obj, atof(str));
		return(i);
	}

	if(*sig=='d')
	{
		i=BGBFR2A_IndexCTabDouble(obj, atof(str));
		return(i);
	}

	if(sig[0]=='P')
	{
		if(sig[1]=='c')
		{
			i=BGBFR2A_IndexCTabString(obj, str);
			return(i);
		}
	}
	
	return(-1);
}

int BGBFR2A_EmitTextByte(FR2A_Object *obj, int val)
{
	int i, j, k;

	if(!obj->cur_func->text)
	{
		obj->cur_func->text=gcatomic(256);
		obj->cur_func->ct=obj->cur_func->text;
		obj->cur_func->cte=obj->cur_func->text+256;
	}

	if((obj->cur_func->ct+1)>=obj->cur_func->cte)
	{
		i=(obj->cur_func->cte)-(obj->cur_func->text);
		j=(obj->cur_func->ct)-(obj->cur_func->text);
		i=i+(i>>1);
		obj->cur_func->text=gcrealloc(obj->cur_func->text, i);
		obj->cur_func->cte=obj->cur_func->text+i;
		obj->cur_func->ct=obj->cur_func->text+j;
	}
	
	*obj->cur_func->ct++=val;
	return(0);
}

int BGBFR2A_EmitJumpReloc(FR2A_Object *obj, char *lbl, int tag)
{
	int i;

	i=obj->cur_func->n_jmp++;
	obj->cur_func->jmp_name[i]=dyllStrdup(lbl);
	obj->cur_func->jmp_offs[i]=obj->cur_func->ct-obj->cur_func->text;
	obj->cur_func->jmp_tag[i]=tag;
}

int BGBFR2A_EmitLabel(FR2A_Object *obj, char *lbl)
{
	int i;

	i=obj->cur_func->n_lbl++;
	obj->cur_func->lbl_name[i]=dyllStrdup(lbl);
	obj->cur_func->lbl_offs[i]=obj->cur_func->ct-obj->cur_func->text;
	obj->cur_func->lbl_tag[i]=tag;
}

int BGBFR2A_EmitJumpAddr16(FR2A_Object *obj, char *lbl)
{
	BGBFR2A_EmitTextByte(obj, 0);
	BGBFR2A_EmitTextByte(obj, 0);
	BGBFR2A_EmitJumpReloc(obj, lbl, BGBFR2_TWOCC_I2);
}

int BGBFR2A_EmitJumpAddr24(FR2A_Object *obj, char *lbl)
{
	BGBFR2A_EmitTextByte(obj, 0);
	BGBFR2A_EmitTextByte(obj, 0);
	BGBFR2A_EmitTextByte(obj, 0);
	BGBFR2A_EmitJumpReloc(obj, lbl, BGBFR2_TWOCC_I3);
}

int BGBFR2A_EmitJumpAddr24(FR2A_Object *obj, char *lbl)
{
	BGBFR2A_EmitTextByte(obj, 0);
	BGBFR2A_EmitTextByte(obj, 0);
	BGBFR2A_EmitTextByte(obj, 0);
	BGBFR2A_EmitTextByte(obj, 0);
	BGBFR2A_EmitJumpReloc(obj, lbl, BGBFR2_TWOCC_I4);
}

void BGBFR2A_AssembleStatementBlock(FR2A_Object *obj, char **rcs)
{
	char *args[64];
	char tb1[256], tb2[256];
	char *cs, *cs1, *cse;
	char *name, *sig;
	int ty1, ty2;
	int op, pfx, nargs;
	int rd, rs, rt, rc;

	cs=*rcs;
	BGBFR2A_ParseInstrName(obj, &cs, &name, &sig);
	
	if(!name)return;
	
	pfx=0;
	if(sig)
	{
		pfx=BGBFR2A_IndexNameList(sig, bgbfr2a_pfx_names);
		if(pfx<0)pfx=0;
	}

	nargs=0;
	while(*cs)
	{
		cs1=BGBFR2_AsmParse_Token(cs, tb1, &ty1);
		if(!strcmp(tb1, ";"))
			{ cs=cs1; break; }
		if(!strcmp(tb1, ","))
			{ cs=cs1; continue; }
		args[nargs++]=BGBFR2A_Intern(obj, tb1);
	}
	args[nargs]=NULL;

	if(!strcmp(name, "jmp"))
	{
		BGBFR2A_EmitTextByte(obj, FR2_OPFX_UJMP4);
		BGBFR2A_EmitJumpAddr24(obj, args[0]);
		return;
	}

	if(!strcmp(name, "ret"))
	{
		if(pfx)
		{
			if(pfx<16)
			{
				rs=BGBFR2A_AssembleDecodeRegisterName(obj, args[0]);
				BGBFR2A_EmitTextByte(obj, FR2_OPFX_ESCG1+pfx);
				BGBFR2A_EmitTextByte(obj, FR2_OPG_RET);
				BGBFR2A_EmitTextByte(obj, rs);
				return;
			}else
			{
				rs=BGBFR2A_AssembleDecodeRegisterName(obj, args[0]);
				BGBFR2A_EmitTextByte(obj, FR2_OPFX_ESCG2+((pfx>>4)&15));
				BGBFR2A_EmitTextByte(obj, (pfx&15)<<4);
				BGBFR2A_EmitTextByte(obj, FR2_OPG_RET);
				BGBFR2A_EmitTextByte(obj, rs);
				return;
			}
		}else
		{
			BGBFR2A_EmitTextByte(obj, FR2_OPFX_ESCG1);
			BGBFR2A_EmitTextByte(obj, FR2_OPG_RET);
			return;
		}
	}

	if(!strcmp(name, "call"))
	{
		if(pfx<16)
		{
			BGBFR2A_EmitTextByte(obj, FR2_OPFX_ESCG1+pfx);
			BGBFR2A_EmitTextByte(obj, FR2_OPG_CALL);
		}else
		{
			BGBFR2A_EmitTextByte(obj, FR2_OPFX_ESCG2+((pfx>>4)&15));
			BGBFR2A_EmitTextByte(obj, (pfx&15)<<4);
			BGBFR2A_EmitTextByte(obj, FR2_OPG_CALL);
		}

		rd=BGBFR2A_AssembleDecodeRegisterName(obj, args[0]);
		rs=BGBFR2A_AssembleDecodeRegisterName(obj, args[1]);

		BGBFR2A_EmitTextByte(obj, rd);
		BGBFR2A_EmitTextByte(obj, rs);
		BGBFR2A_EmitTextByte(obj, nargs-2);
		
		for(i=2; i<nargs; i++)
		{
			rt=BGBFR2A_AssembleDecodeRegisterName(obj, args[i]);
			BGBFR2A_EmitTextByte(obj, rt);
		}
		
		return;
	}

	if(!strcmp(name, "sload"))
	{
		if(pfx<16)
		{
			BGBFR2A_EmitTextByte(obj, FR2_OPFX_ESCG1+pfx);
			BGBFR2A_EmitTextByte(obj, FR2_OPG_SLOAD);
		}else
		{
			BGBFR2A_EmitTextByte(obj, FR2_OPFX_ESCG2+((pfx>>4)&15));
			BGBFR2A_EmitTextByte(obj, (pfx&15)<<4);
			BGBFR2A_EmitTextByte(obj, FR2_OPG_SLOAD);
		}

		rd=BGBFR2A_AssembleDecodeRegisterName(obj, args[0]);
//		rs=BGBFR2A_AssembleDecodeRegisterName(obj, args[1]);
		rc=BGBFR2A_LookupTopVar(obj, args[1]);

		BGBFR2A_EmitTextByte(obj, rd);
		BGBFR2A_EmitTextByte(obj, (rc>>8)&255);
		BGBFR2A_EmitTextByte(obj, (rc   )&255);
		return;
	}

	if(!strcmp(name, "sstore"))
	{
		if(pfx<16)
		{
			BGBFR2A_EmitTextByte(obj, FR2_OPFX_ESCG1+pfx);
			BGBFR2A_EmitTextByte(obj, FR2_OPG_SSTORE);
		}else
		{
			BGBFR2A_EmitTextByte(obj, FR2_OPFX_ESCG2+((pfx>>4)&15));
			BGBFR2A_EmitTextByte(obj, (pfx&15)<<4);
			BGBFR2A_EmitTextByte(obj, FR2_OPG_SSTORE);
		}

//		rd=BGBFR2A_AssembleDecodeRegisterName(obj, args[0]);
		rs=BGBFR2A_AssembleDecodeRegisterName(obj, args[1]);
		rc=BGBFR2A_LookupTopVar(obj, args[0]);

		BGBFR2A_EmitTextByte(obj, rd);
		BGBFR2A_EmitTextByte(obj, (rc>>8)&255);
		BGBFR2A_EmitTextByte(obj, (rc   )&255);
		return;
	}

	if(!strcmp(name, "scall"))
	{
		if(pfx<16)
		{
			BGBFR2A_EmitTextByte(obj, FR2_OPFX_ESCG1+pfx);
			BGBFR2A_EmitTextByte(obj, FR2_OPG_SCALL);
		}else
		{
			BGBFR2A_EmitTextByte(obj, FR2_OPFX_ESCG2+((pfx>>4)&15));
			BGBFR2A_EmitTextByte(obj, (pfx&15)<<4);
			BGBFR2A_EmitTextByte(obj, FR2_OPG_SCALL);
		}

		rd=BGBFR2A_AssembleDecodeRegisterName(obj, args[0]);
//		rs=BGBFR2A_AssembleDecodeRegisterName(obj, args[1]);
		rc=BGBFR2A_LookupTopVar(obj, args[1]);

		BGBFR2A_EmitTextByte(obj, rd);
		BGBFR2A_EmitTextByte(obj, (rc>>8)&255);
		BGBFR2A_EmitTextByte(obj, (rc   )&255);
		BGBFR2A_EmitTextByte(obj, nargs-2);
		
		for(i=2; i<nargs; i++)
		{
			rt=BGBFR2A_AssembleDecodeRegisterName(obj, args[i]);
			BGBFR2A_EmitTextByte(obj, rt);
		}
		
		return;
	}

	op=BGBFR2A_IndexNameList(name, bgbfr2a_jop_names);
	if(op>=0)
	{
		if((pfx==FR2_PFX_I) || (pfx==FR2_PFX_L))
		{
			rs=BGBFR2A_AssembleDecodeRegisterName(obj, args[0]);
			rt=BGBFR2A_AssembleDecodeRegisterName(obj, args[1]);

			if((rs<16) && (rt<16))
			{
				i=FR2_OPFX_BJMP4IL+op;
				if(pfx==FR2_PFX_L)
					{ i+=8; }
				j=(rs<<4)|rt;
				BGBFR2A_EmitTextByte(obj, i|op);
				BGBFR2A_EmitTextByte(obj, j);
				BGBFR2A_EmitJumpAddr16(obj, args[2]);
				return;
			}
		}

		if((pfx==FR2_PFX_F) || (pfx==FR2_PFX_D))
		{
			rs=BGBFR2A_AssembleDecodeRegisterName(obj, args[0]);
			rt=BGBFR2A_AssembleDecodeRegisterName(obj, args[1]);

			if((rs<16) && (rt<16))
			{
				i=FR2_OPFX_BJMP4IL+op;
				if(pfx==FR2_PFX_D)
					{ i+=8; }
				j=(rs<<4)|rt;
				BGBFR2A_EmitTextByte(obj, i|op);
				BGBFR2A_EmitTextByte(obj, j);
				BGBFR2A_EmitJumpAddr16(obj, args[2]);
				return;
			}
		}

		if((pfx>=FR2_PFX_IC) && (pfx<=FR2_PFX_DC))
		{
			rs=BGBFR2A_AssembleDecodeRegisterName(obj, args[0]);
			rc=BGBFR2A_AssembleDecodeLiteralPfx(obj, pfx, args[1]);
//			rc=bgbgc_atoi(args[1]);
			if(rs<0)rs=0;
			
			i=FR2_OPFX_ESCG1+((pfx-FR2_PFX_IC)+FR2_PFX_I);
			BGBFR2A_EmitTextByte(obj, i);
			BGBFR2A_EmitTextByte(obj, FR2_OPG_BINARYJMPC);
			BGBFR2A_EmitTextByte(obj, op);
			BGBFR2A_EmitTextByte(obj, rs);
			BGBFR2A_EmitTextByte(obj, (rc>>24)&255);
			BGBFR2A_EmitTextByte(obj, (rc>>16)&255);
			BGBFR2A_EmitTextByte(obj, (rc>> 8)&255);
			BGBFR2A_EmitTextByte(obj, (rc    )&255);
			BGBFR2A_EmitJumpAddr32(obj, args[2]);
			return;
		}

		rs=BGBFR2A_AssembleDecodeRegisterName(obj, args[0]);
		rt=BGBFR2A_AssembleDecodeRegisterName(obj, args[1]);

		if(pfx<16)
		{
			i=FR2_OPFX_ESCG1+pfx;
			BGBFR2A_EmitTextByte(obj, i);
			BGBFR2A_EmitTextByte(obj, FR2_OPG_BINARYJMP);
		}else
		{
			i=FR2_OPFX_ESCG2+(pfx>>4)&15;
			BGBFR2A_EmitTextByte(obj, i);
			BGBFR2A_EmitTextByte(obj, ((pfx&15)<<4));
			BGBFR2A_EmitTextByte(obj, FR2_OPG_BINARYJMP);
		}
		BGBFR2A_EmitTextByte(obj, op);
		BGBFR2A_EmitTextByte(obj, rs);
		BGBFR2A_EmitTextByte(obj, rt);
		BGBFR2A_EmitJumpAddr32(obj, args[2]);
		return;
	}

	op=BGBFR2A_IndexNameList(name, bgbfr2a_uop_names);
	if(op>=0)
	{
		if((pfx==FR2_PFX_I) ||
			(pfx==FR2_PFX_L) ||
			(pfx==FR2_PFX_F) ||
			(pfx==FR2_PFX_D))
		{
			rd=BGBFR2A_AssembleDecodeRegisterName(obj, args[0]);
			rs=BGBFR2A_AssembleDecodeRegisterName(obj, args[1]);
			if(rd<0)rd=0;
			if(rs<0)rs=0;
			
			if((op<16) && (rd<16) && (rs<16))
			{
				i=FR2_OPFX_UNARY4I+((pfx-FR2_PFX_I)<<4);
				j=(rd<<4)|rs;
				BGBFR2A_EmitTextByte(obj, i|op);
				BGBFR2A_EmitTextByte(obj, j);
				return;
			}else
			{
				i=FR2_OPFX_ESCG1+pfx;
				BGBFR2A_EmitTextByte(obj, i);
				BGBFR2A_EmitTextByte(obj, FR2_OPG_UNARY);
				BGBFR2A_EmitTextByte(obj, op);
				BGBFR2A_EmitTextByte(obj, rd);
				BGBFR2A_EmitTextByte(obj, rs);
				return;
			}
		}
		
		if(pfx==FR2_PFX_IC)
		{
			rd=BGBFR2A_AssembleDecodeRegisterName(obj, args[0]);
//			rc=BGBFR2A_AssembleDecodeLiteralInt(obj, args[1]);
			rc=BGBFR2A_AssembleDecodeLiteralPfx(obj, pfx, args[1]);
//			rc=bgbgc_atoi(args[1]);
			if(rd<0)rd=0;
			
			if((op<16) && (rd<16) && (rc>=(-2048)) && (rc<2048))
			{
				j=(rd<<4)|((rc>>8)&15);
				BGBFR2A_EmitTextByte(obj, FR2_OPFX_UNARY4IC+op);
				BGBFR2A_EmitTextByte(obj, j);
				BGBFR2A_EmitTextByte(obj, rc&255);
				return;
			}
		}

		if((pfx>=FR2_PFX_IC) && (pfx<=FR2_PFX_DC))
		{
			rd=BGBFR2A_AssembleDecodeRegisterName(obj, args[0]);
			rc=BGBFR2A_AssembleDecodeLiteralPfx(obj, pfx, args[1]);
//			rc=BGBFR2A_AssembleDecodeLiteralInt(obj, args[1]);
//			rc=bgbgc_atoi(args[1]);
			if(rd<0)rd=0;
			
			i=FR2_OPFX_ESCG1+((pfx-FR2_PFX_IC)+FR2_PFX_I);
			BGBFR2A_EmitTextByte(obj, i);
			BGBFR2A_EmitTextByte(obj, FR2_OPG_UNARYC);
			BGBFR2A_EmitTextByte(obj, op);
			BGBFR2A_EmitTextByte(obj, rd);
			BGBFR2A_EmitTextByte(obj, (rc>>24)&255);
			BGBFR2A_EmitTextByte(obj, (rc>>16)&255);
			BGBFR2A_EmitTextByte(obj, (rc>> 8)&255);
			BGBFR2A_EmitTextByte(obj, (rc    )&255);
			return;
		}

		if(pfx<16)
		{
			i=FR2_OPFX_ESCG1+pfx;
			BGBFR2A_EmitTextByte(obj, i);
			BGBFR2A_EmitTextByte(obj, FR2_OPG_UNARY);
		}else
		{
			i=FR2_OPFX_ESCG2+(pfx>>4)&15;
			BGBFR2A_EmitTextByte(obj, i);
			BGBFR2A_EmitTextByte(obj, ((pfx&15)<<4));
			BGBFR2A_EmitTextByte(obj, FR2_OPG_UNARY);
		}
		BGBFR2A_EmitTextByte(obj, op);
		BGBFR2A_EmitTextByte(obj, rd);
		BGBFR2A_EmitTextByte(obj, rs);
		return;
	}


	op=BGBFR2A_IndexNameList(name, bgbfr2a_bop_names);
	if(op>=0)
	{
		if((pfx==FR2_PFX_I) ||
			(pfx==FR2_PFX_L) ||
			(pfx==FR2_PFX_F) ||
			(pfx==FR2_PFX_D))
		{
			rd=BGBFR2A_AssembleDecodeRegisterName(obj, args[0]);
			rs=BGBFR2A_AssembleDecodeRegisterName(obj, args[1]);
			rt=BGBFR2A_AssembleDecodeRegisterName(obj, args[2]);
			if(rd<0)rd=0;
			if(rs<0)rs=0;
			if(rt<0)rt=0;
			
			if((op<256) && (rd<16) && (rs<16) && (rt<16))
			{
				i=FR2_OPFX_BINARY4I+((pfx-FR2_PFX_I)<<4)+((op>>4)&15);
				j=((op&15)<<4)|rd;
				k=(rs<<4)|rt;
				BGBFR2A_EmitTextByte(obj, i);
				BGBFR2A_EmitTextByte(obj, j);
				BGBFR2A_EmitTextByte(obj, k);
				return;
			}else
			{
				i=FR2_OPFX_ESCG1+pfx;
				BGBFR2A_EmitTextByte(obj, i);
				BGBFR2A_EmitTextByte(obj, FR2_OPG_BINARY);
				BGBFR2A_EmitTextByte(obj, op);
				BGBFR2A_EmitTextByte(obj, rd);
				BGBFR2A_EmitTextByte(obj, rs);
				BGBFR2A_EmitTextByte(obj, rt);
				return;
			}
		}
		
		if(pfx==FR2_PFX_IC)
		{
			rd=BGBFR2A_AssembleDecodeRegisterName(obj, args[0]);
			rs=BGBFR2A_AssembleDecodeRegisterName(obj, args[1]);
			rc=BGBFR2A_AssembleDecodeLiteralPfx(obj, pfx, args[2]);
//			rc=BGBFR2A_AssembleDecodeLiteralInt(obj, args[1]);
//			rc=bgbgc_atoi(args[2]);
			if(rd<0)rd=0;
			
			if((op<256) && (rd<16) && (rs<16) &&
				(rc>=(-2048)) && (rc<2048))
			{
				i=FR2_OPFX_BINARY4IC+((op>>4)&15);
				j=((op&15)<<4)|rd;
				k=(rs<<4)|((rc>>8)&15);
				BGBFR2A_EmitTextByte(obj, i);
				BGBFR2A_EmitTextByte(obj, j);
				BGBFR2A_EmitTextByte(obj, k);
				BGBFR2A_EmitTextByte(obj, rc&255);
				return;
			}
		}

		if((pfx>=FR2_PFX_IC) && (pfx<=FR2_PFX_DC))
		{
			rd=BGBFR2A_AssembleDecodeRegisterName(obj, args[0]);
			rs=BGBFR2A_AssembleDecodeRegisterName(obj, args[1]);
			rc=BGBFR2A_AssembleDecodeLiteralPfx(obj, pfx, args[2]);
//			rc=BGBFR2A_AssembleDecodeLiteralInt(obj, args[1]);
//			rc=bgbgc_atoi(args[2]);
			if(rd<0)rd=0;
			if(rs<0)rd=0;
			
			i=FR2_OPFX_ESCG1+((pfx-FR2_PFX_IC)+FR2_PFX_I);
			BGBFR2A_EmitTextByte(obj, i);
			BGBFR2A_EmitTextByte(obj, FR2_OPG_BINARYC);
			BGBFR2A_EmitTextByte(obj, op);
			BGBFR2A_EmitTextByte(obj, rd);
			BGBFR2A_EmitTextByte(obj, rs);
			BGBFR2A_EmitTextByte(obj, (rc>>24)&255);
			BGBFR2A_EmitTextByte(obj, (rc>>16)&255);
			BGBFR2A_EmitTextByte(obj, (rc>> 8)&255);
			BGBFR2A_EmitTextByte(obj, (rc    )&255);
			return;
		}

		rd=BGBFR2A_AssembleDecodeRegisterName(obj, args[0]);
		rs=BGBFR2A_AssembleDecodeRegisterName(obj, args[1]);
		rt=BGBFR2A_AssembleDecodeRegisterName(obj, args[2]);
		if(rd<0)rd=0;
		if(rs<0)rs=0;
		if(rt<0)rt=0;

		if(pfx<16)
		{
			i=FR2_OPFX_ESCG1+pfx;
			BGBFR2A_EmitTextByte(obj, i);
			BGBFR2A_EmitTextByte(obj, FR2_OPG_BINARY);
		}else
		{
			i=FR2_OPFX_ESCG2+(pfx>>4)&15;
			BGBFR2A_EmitTextByte(obj, i);
			BGBFR2A_EmitTextByte(obj, ((pfx&15)<<4));
			BGBFR2A_EmitTextByte(obj, FR2_OPG_BINARY);
		}

		BGBFR2A_EmitTextByte(obj, op);
		BGBFR2A_EmitTextByte(obj, rd);
		BGBFR2A_EmitTextByte(obj, rs);
		BGBFR2A_EmitTextByte(obj, rt);
		return;
	}
}

void BGBFR2A_AssembleBlockVars(FR2A_Object *obj, char **rcs)
{
	char tb1[256], tb2[256];
	char *cs, *cs1, *cs2, *cse;
	char *name, *sig;
	int val;
	int ty1, ty2;

	cs=*rcs;
	while(*cs)
	{
		BGBFR2A_ParseDeclVar(obj, &cs, &name, &sig);
		val=0;

		cs1=BGBFR2_AsmParse_Token(cs, tb1, &ty1);
		if(!strcmp(tb1, "="))
		{
			cs1=BGBFR2_AsmParse_Token(cs1, tb2, &ty2);
			cs=cs1;

			val=BGBFR2A_AssembleDecodeLiteralSigCtab(obj, sig, tb2);

			cs1=BGBFR2_AsmParse_Token(cs, tb1, &ty1);
		}

		i=ctx->cur_func->n_var++;
		ctx->cur_func->var_name[i]=name;
		ctx->cur_func->var_sig[i]=sig;
		ctx->cur_func->var_val[i]=val;

		if(!strcmp(tb1, ","))
			{ cs=cs1; continue; }
		if(!strcmp(tb1, ";"))
			{ cs=cs1; break; }
	}
}

void BGBFR2A_ApplyBlockRelocs(FR2A_Object *obj)
{
	FR2A_Function *func;
	byte *cs;
	int i, j, k, l;
	
	func=obj->cur_func;
	
	l=0;
	for(i=0; i<func->n_jmp; i++)
	{
		for(j=0; j<func->n_lbl; j++)
		{
			if(!strcmp(func->jmp_name[i], func->lbl_name[j]))
				break;
		}

		if(j<func->n_lbl)
		{
			k=func->lbl_offs[j]-func->jmp_offs[i];
			switch(func->jmp_tag[i])
			{
			case BGBFR2_TWOCC_I2:
				cs=func->text+(func->jmp_offs[i]-2);
				cs[0]=(k>>8)&255;
				cs[1]=(k   )&255;
				break;
			case BGBFR2_TWOCC_I3:
				cs=func->text+(func->jmp_offs[i]-3);
				cs[0]=(k>>16)&255;
				cs[1]=(k>> 8)&255;
				cs[2]=(k    )&255;
				break;
			case BGBFR2_TWOCC_I4:
				cs=func->text+(func->jmp_offs[i]-4);
				cs[0]=(k>>24)&255;
				cs[1]=(k>>16)&255;
				cs[2]=(k>> 8)&255;
				cs[3]=(k    )&255;
				break;
			default:
				break;
			}
		}else
		{
			func->jmp_name[l]=func->jmp_name[i];
			func->jmp_offs[l]=func->jmp_offs[i];
			func->jmp_tag[l]=func->jmp_tag[i];
			l++;
		}
	}
	func->n_jmp=l;
}

void BGBFR2A_AssembleBlockStatements(FR2A_Object *obj, char **rcs)
{
	char tb1[256], tb2[256];
	char *cs, *cs1, *cs2, *cse;
	int ty1, ty2;

	cs=*rcs;
	while(*cs)
	{
		cs1=BGBFR2_AsmParse_Token(cs, tb1, &ty1);
		if(!strcmp(tb1, "}"))
			{ cs=cs1; break; }

		cs2=BGBFR2_AsmParse_Token(cs1, tb2, &ty2);

		if((ty1==BGBFR2_TOKEN_NAME) && !strcmp(tb2, ":"))
		{
			BGBFR2A_EmitLabel(obj, tb1);
			cs=cs2;
			continue;
		}

		if(!strcmp(tb1, "var"))
		{
			cs=cs1;
			BGBFR2A_AssembleBlockVars(obj, &cs);
			continue;
		}

		BGBFR2A_AssembleStatementBlock(obj, &cs);
	}
	
	BGBFR2A_ApplyBlockRelocs(obj);
	
	*rcs=cs;
}

void BGBFR2A_CheckExpandFuncs(FR2A_Object *obj)
{
	int i, j, k;

	if(!obj->func)
	{
		i=16;
		obj->func=gcalloc(i*sizeof(FR2A_Function *));
		obj->n_func=0;
		obj->m_func=i;
	}

	if((obj->n_func+1)>=obj->m_func)
	{
		i=obj->m_func; i=i+(i>>1);
		obj->func=gcrealloc(obj->func, i*sizeof(FR2A_Function *));
		obj->m_func=i;
	}
}

void BGBFR2A_CheckExpandVars(FR2A_Object *obj)
{
	int i, j, k;

	if(!obj->func)
	{
		i=16;
		obj->var=gcalloc(i*sizeof(FR2A_VarInfo *));
		obj->n_var=0;
		obj->m_var=i;
	}

	if((obj->n_var+1)>=obj->m_var)
	{
		i=obj->m_var; i=i+(i>>1);
		obj->var=gcrealloc(obj->var, i*sizeof(FR2A_VarInfo *));
		obj->m_var=i;
	}
}

int BGBFR2A_LookupTopVar(FR2A_Object *obj, char *str)
{
	int i, j, k;

	for(i=0; i<obj->n_var; i++)
	{
		if(!strcmp(obj->var[i]->name, str))
			return(i);
	}
	return(-1);
}

int BGBFR2A_LookupTopVarSig(FR2A_Object *obj, char *name, char *sig)
{
	int i, j, k;

	for(i=0; i<obj->n_var; i++)
	{
		if(!strcmp(obj->var[i]->name, name) &&
			!strcmp(obj->var[i]->sig, sig))
			return(i);
	}
	return(-1);
}

int BGBFR2A_IndexTopVarSig(FR2A_Object *obj, char *name, char *sig)
{
	FR2A_VarInfo *tvar;
	int i, j, k;

	i=BGBFR2A_LookupTopVarSig(obj, name, sig);
	if(i>=0)
		{ return(i); }

	BGBFR2A_CheckExpandVars(obj);
		
	tvar=gctalloc("fr2a_varinfo_t", sizeof(FR2A_VarInfo));
	tvar->name=BGBFR2A_Intern(obj, name);
	tvar->sig=BGBFR2A_Intern(obj, sig);
//	tvar->val=val;

	i=obj->n_var++;
	obj->var[i]=tvar;
	
	return(i);
}

void BGBFR2A_AssembleFunction(FR2A_Object *obj, char **rcs)
{
	char tb1[256], tb2[256];
	FR2A_Function *func, *ofunc;
	char *cs, *cs1, *cse;
	char *name, *sig;
	char *name1, *sig1;
	int ty1, ty2;
	int i, j, k;
	
	BGBFR2A_CheckExpandFuncs(obj);
	func=gctalloc("fr2a_function_t", sizeof(FR2A_Function));
	
	cs=*rcs;
	BGBFR2A_ParseDeclVar(obj, &cs, &name, &sig);
	func->name=name;
	func->sig=sig;
	
	i=obj->n_func++;
	obj->func[i]=func;

	cs=BGBFR2_AsmParse_Token(cs, tb1, &ty1);
	if(!strcmp(tb1, "("))
	{
		while(1)
		{
			cs1=BGBFR2_AsmParse_Token(cs, tb1, &ty1);
			if(!strcmp(tb1, ")"))
				{ cs=cs1; break; }

			if(!strcmp(tb1, ","))
				{ cs=cs1; break; }

			BGBFR2A_ParseDeclVar(obj, &cs, &name1, &sig1);
			if(!name1)break;
			
			i=func->n_arg++;
			func->arg_name[i]=name1;
			func->arg_sig[i]=sig1;
		}

		cs1=BGBFR2_AsmParse_Token(cs, tb1, &ty1);
		if(!strcmp(tb1, ")"))
			{ cs=cs1; break; }
	}

	cs1=BGBFR2_AsmParse_Token(cs, tb1, &ty1);
	if(!strcmp(tb1, "{"))
	{
		ofunc=obj->cur_func;
		obj->cur_func=func;
		
		cs=cs1;
		BGBFR2A_AssembleBlockStatements(obj, &cs);

		obj->cur_func=ofunc;
	}
	
	*rcs=cs;
}

void BGBFR2A_AssembleTopVars(FR2A_Object *obj, char **rcs)
{
	char tb1[256], tb2[256];
	FR2A_VarInfo *tvar;
	char *cs, *cs1, *cs2, *cse;
	char *name, *sig;
	int val;
	int ty1, ty2;

	cs=*rcs;
	while(*cs)
	{
		BGBFR2A_ParseDeclVar(obj, &cs, &name, &sig);
		val=0;

		cs1=BGBFR2_AsmParse_Token(cs, tb1, &ty1);
		if(!strcmp(tb1, "="))
		{
			cs1=BGBFR2_AsmParse_Token(cs1, tb2, &ty2);
			cs=cs1;

			val=BGBFR2A_AssembleDecodeLiteralSigCtab(obj, sig, tb2);

			cs1=BGBFR2_AsmParse_Token(cs, tb1, &ty1);
		}

		BGBFR2A_CheckExpandVars(obj);
		
		tvar=gctalloc("fr2a_varinfo_t", sizeof(FR2A_VarInfo));
		tvar->name=name;
		tvar->sig=sig;
		tvar->val=val;

		i=obj->n_var++;
		obj->var[i]=tvar;

		if(!strcmp(tb1, ","))
			{ cs=cs1; continue; }
		if(!strcmp(tb1, ";"))
			{ cs=cs1; break; }
	}
}

void BGBFR2A_AssembleStatementTop(FR2A_Object *obj, char **rcs)
{
	char tb1[256], tb2[256];
	char *cs, *cs1, *cse;
	int ty1, ty2;
	
	cs=*rcs;
	cs1=BGBFR2_AsmParse_Token(cs, tb1, &ty1);
	
	if(!strcmp(tb1, "function"))
	{
		cs=cs1;
		BGBFR2A_AssembleFunction(obj, &cs);
		*rcs=cs;
		return;
	}

	if(!strcmp(tb1, "var"))
	{
		cs=cs1;
		BGBFR2A_AssembleTopVars(obj, &cs);
		*rcs=cs;
		return;
	}
}

FR2A_Object *BGBFR2A_AssembleBuffer(char *buf, int szBuf)
{
	FR2A_Object *obj;
	char *cs, *cse;
	
	obj=BGBFR2A_AllocObject();
	cs=buf; cse=buf+szBuf;
	while(cs<cse)
	{
		BGBFR2A_AssembleStatementTop(obj, &cs);
	}
}