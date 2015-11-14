#include <frbc2ci.h>

void *fr2ci_malloc(int sz)
{
	void *ptr;
	ptr=malloc(sz);
	memset(ptr, 0, sz);
	return(ptr);
}

void fr2ci_free(void *ptr)
{
	free(ptr);
}

char *fr2ci_strdup(char *str)
{
	return(strdup(str));
}

FR2CI_Image *FR2CI_AllocImage(void)
{
	FR2CI_Image *img;
	
	img=fr2ci_malloc(sizeof(FR2CI_Image));
	memset(img, 0, sizeof(FR2CI_Image));
	return(img);
}

FR2CI_Image *FR2CI_DecodeImage(byte *data, int szData, int flags)
{
	FR2CI_Image *img;
	u32 m0, m1, msz;
	u32 tsz, tfcc;
	byte *cs, *cse, *cs1;
	u16 *wct, *wct1;
	void *ptr;
	int n, nl;
	int i, j, k;
	
	m0=fr2ci_getUInt32BE(data+0);
	m1=fr2ci_getUInt32BE(data+4);
	msz=fr2ci_getUInt32BE(data+8);
	if(m0!=FR2C_FOURCC_FRBC)
		return(NULL);
	if(m1!=FR2C_FOURCC_2C00)
		return(NULL);
	if(((s64)msz)>szData)
		return(NULL);
	
	if(flags&FR2C_LOADFL_NOCOPY)
	{
		img=FR2CI_AllocImage();
		img->imgbuf=data;
		img->sz_imgbuf=szData;
	}else
	{
		img=FR2CI_AllocImage();
		img->imgbuf=fr2ci_malloc(msz);
		img->sz_imgbuf=msz;
		memcpy(img->imgbuf, data, msz);
	}

	cs=img->imgbuf+12;
	cse=img->imgbuf+msz;
	
	while(cs<cse)
	{
		if(*cs==0xE3)
		{
			tsz=fr2ci_getUInt24BE(cs+1);
			if((cs+tsz)>cse)
				break;
			tfcc=fr2ci_getUInt16BE(cs+4);
			FR2CI_DecodeMarkerTWOCC(img, (u16)tfcc, cs+6, cs+tsz);
			cs+=tsz;
			continue;
		}

		if(*cs==0xE4)
		{
			tsz=fr2ci_getUInt24BE(cs+1);
			if((cs+tsz)>cse)
				break;
			tfcc=fr2ci_getUInt32BE(cs+4);
			FR2CI_DecodeMarkerFOURCC(img, tfcc, cs+8, cs+tsz);
			cs+=tsz;
			continue;
		}

		if(*cs==0xE5)
		{
			tsz=cs[1];
			if((cs+tsz)>cse)
				break;
			tfcc=fr2ci_getUInt16BE(cs+2);
			FR2CI_DecodeMarkerTWOCC(img, (u16)tfcc, cs+4, cs+tsz);
			cs+=tsz;
			continue;
		}

		if(*cs==0xE6)
		{
			tsz=(u32)fr2ci_getUInt56BE(cs+1);
			if((cs+tsz)>cse)
				break;
			tfcc=fr2ci_getUInt32BE(cs+8);
			FR2CI_DecodeMarkerFOURCC(img, tfcc, cs+12, cs+tsz);
			cs+=tsz;
			continue;
		}

		if(*cs==0xE7)
		{
			tsz=fr2ci_getUInt24BE(cs+1);
			if((cs+tsz)>cse)
				break;
//			tfcc=fr2ci_getUInt32BE(cs+4);
//			FR2CI_DecodeMarkerFOURCC(img, tfcc, cs+8, cs+tsz);
			cs+=tsz;
			continue;
		}

		if(*cs==0xE8)
		{
			tsz=fr2ci_getUInt24BE(cs+1);
			if((cs+tsz)>cse)
				break;
//			tfcc=fr2ci_getUInt32BE(cs+4);
//			FR2CI_DecodeMarkerFOURCC(img, tfcc, cs+8, cs+tsz);
			cs+=tsz;
			continue;
		}

		if((*cs==0xE9) || (*cs==0xEA))
		{
			tsz=(u32)fr2ci_getUInt56BE(cs+1);
			if((cs+tsz)>cse)
				break;
//			tfcc=fr2ci_getUInt32BE(cs+8);
//			FR2CI_DecodeMarkerFOURCC(img, tfcc, cs+12, cs+tsz);
			cs+=tsz;
			continue;
		}
		
		break;
	}
	
	if(img->wstrtab)
	{
		img->wstrtab2=fr2ci_malloc(img->sz_wstrtab*sizeof(u16));
		cs=img->wstrtab; cse=cs+img->sz_wstrtab;
		wct=img->wstrtab2;
		while(cs<cse)
		{
			cs1=cs;
			wct1=wct;
			while(*cs1)
			{
				i=FR2CI_DecodeCharUTF8(&cs1);
				*wct1++=i;
			}
			*wct1=0;
			i=strlen(cs)+1;
			cs+=i; wct+=i;
		}
	}

//	nl=img->sz_lidx/4;
	nl=img->sz_lidx/img->sr_lidx;
	img->litv=fr2ci_malloc(nl*sizeof(FR2CI_Value));
	img->litab=fr2ci_malloc(nl*sizeof(FR2CI_LiteralInfo *));
	img->n_lit=nl;
	for(i=0; i<nl; i++)
		{ FR2CI_DecodeLiteralTableEntry(img, i); }

//	n=img->sz_gidx/4;
	n=img->sz_gidx/img->sr_gidx;
	img->gbl=fr2ci_malloc(n*sizeof(FR2CI_Value));
	img->gbl_func=fr2ci_malloc(n*sizeof(FR2CI_Function *));
	img->gbl_dvar=fr2ci_malloc(n*sizeof(FR2CI_VarInfo *));
	img->n_gbl=n;
	
	for(i=0; i<img->n_gbl; i++)
	{
//		j=fr2ci_getInt32BE(img->gidx+i*4);
		j=FR2CI_GetGIDX(img, i);
		cs=img->gdef+j;

		if(cs[0]==0xE3)
		{
			tsz=(cs[1]<<16)|(cs[2]<<8)|(cs[3]);
			tfcc=fr2ci_getInt16BE(cs+4);
			if(tfcc==FR2C_TWOCC_FN)
			{
				ptr=FR2CI_DecodeFunction(img, cs+6, tsz-6);
				img->gbl_func[i]=ptr;
				img->gbl[i].p=ptr;
				continue;
			}

			if(tfcc==FR2C_TWOCC_DV)
			{
				ptr=FR2CI_DecodeVarInfo(img, cs+6, tsz-6);
				img->gbl_dvar[i]=ptr;
				continue;
			}
		}

		if(cs[0]==0xE4)
		{
			tsz=(cs[1]<<16)|(cs[2]<<8)|(cs[3]);
			tfcc=fr2ci_getInt32BE(cs+4);
			if(tfcc==FR2C_FOURCC_FUNC)
			{
				ptr=FR2CI_DecodeFunction(img, cs+8, tsz-8);
				img->gbl_func[i]=ptr;
				img->gbl[i].p=ptr;
				continue;
			}

			if(tfcc==FR2C_FOURCC_DVAR)
			{
				ptr=FR2CI_DecodeVarInfo(img, cs+8, tsz-8);
				img->gbl_dvar[i]=ptr;
				continue;
			}
		}

		if(cs[0]==0xE5)
		{
			tsz=cs[1];
			tfcc=fr2ci_getInt16BE(cs+2);
			if(tfcc==FR2C_TWOCC_FN)
			{
				ptr=FR2CI_DecodeFunction(img, cs+4, tsz-4);
				img->gbl_func[i]=ptr;
				img->gbl[i].p=ptr;
				continue;
			}

			if(tfcc==FR2C_TWOCC_DV)
			{
				ptr=FR2CI_DecodeVarInfo(img, cs+4, tsz-4);
				img->gbl_dvar[i]=ptr;
				continue;
			}
		}
	}

	for(j=0; j<2; j++)
	{
		for(i=0; i<img->n_lit; i++)
		{
			FR2CI_InitLiteralInfo(img, img->litab[i], img->litv+i);
		}
		for(i=0; i<img->n_gbl; i++)
		{
			if(img->gbl_dvar[i])
			{
				FR2CI_InitVarInfo(img, img->gbl_dvar[i], img->gbl+i);
			}
		}
	}
	
	return(img);
}

int FR2CI_GetGIDX(
	FR2CI_Image *img, int idx)
{
	int i;
	switch(img->sr_gidx)
	{
	case 2:		i=fr2ci_getInt16BE(img->gidx+i*2); break;
	case 3:		i=fr2ci_getInt24BE(img->gidx+i*3); break;
	case 4:		i=fr2ci_getInt32BE(img->gidx+i*4); break;
	default:	i=0; break;
	}
	return(i);
}

int FR2CI_GetLIDX(
	FR2CI_Image *img, int idx)
{
	int i;
	switch(img->sr_lidx)
	{
	case 2:		i=fr2ci_getInt16BE(img->lidx+i*2); break;
	case 3:		i=fr2ci_getInt24BE(img->lidx+i*3); break;
	case 4:		i=fr2ci_getInt32BE(img->lidx+i*4); break;
	default:	i=0; break;
	}
	return(i);
}

int FR2CI_DecodeMarkerTWOCC(
	FR2CI_Image *img, u16 tcc, byte *css, byte *cse)
{
	return(0);
}

int FR2CI_DecodeMarkerFOURCC(
	FR2CI_Image *img, u32 fcc, byte *css, byte *cse)
{
	switch(fcc)
	{
	case FR2C_FOURCC_GDEF:
		img->gdef=css;		img->sz_gdef=cse-css;
		break;
	case FR2C_FOURCC_LDEF:
		img->ldef=css;		img->sz_ldef=cse-css;
		break;

	case FR2C_FOURCC_GIDX:
		img->gidx=css;	img->sz_gidx=cse-css;	img->sr_gidx=4;
		break;
	case FR2C_FOURCC_LIDX:
		img->lidx=css;	img->sz_lidx=cse-css;	img->sr_gidx=4;
		break;
	case FR2C_FOURCC_GIX2:
		img->gidx=css;	img->sz_gidx=cse-css;	img->sr_gidx=2;
		break;
	case FR2C_FOURCC_LIX2:
		img->lidx=css;	img->sz_lidx=cse-css;	img->sr_gidx=2;
		break;
	case FR2C_FOURCC_GIX3:
		img->gidx=css;	img->sz_gidx=cse-css;	img->sr_gidx=3;
		break;
	case FR2C_FOURCC_LIX3:
		img->lidx=css;	img->sz_lidx=cse-css;	img->sr_gidx=3;
		break;

	case FR2C_FOURCC_LVT4:
		img->lvt4=css;		img->sz_lvt4=cse-css;
		break;
	case FR2C_FOURCC_LVT8:
		img->lvt8=css;		img->sz_lvt8=cse-css;
		break;
	case FR2C_FOURCC_STRS:
		img->strtab=css;	img->sz_strtab=cse-css;
		break;
	case FR2C_FOURCC_WSTR:
		img->wstrtab=css;	img->sz_wstrtab=cse-css;
		break;
	}
	return(0);
}

FR2CI_Opcode *FR2CI_AllocOpcode(FR2CI_Image *img)
{
	FR2CI_OpcodeSlab *slab;
	FR2CI_Opcode *tmp;
	int i;

	if(img->free_opcode)
	{
		tmp=img->free_opcode;
		img->free_opcode=*(FR2CI_Opcode **)tmp;
		memset(tmp, 0, sizeof(FR2CI_Opcode));
		return(tmp);
	}
	
	slab=fr2ci_malloc(sizeof(FR2CI_OpcodeSlab));
	slab->next=img->slab_opcode;
	img->slab_opcode=slab;
	
	for(i=0; i<256; i++)
	{
		tmp=&(slab->op[i]);
		*(FR2CI_Opcode **)tmp=img->free_opcode;
		img->free_opcode=tmp;
	}
	
	tmp=img->free_opcode;
	img->free_opcode=*(FR2CI_Opcode **)tmp;
	memset(tmp, 0, sizeof(FR2CI_Opcode));
	return(tmp);
}

void FR2CI_FreeOpcode(FR2CI_Image *img, FR2CI_Opcode *op)
{
	*(FR2CI_Opcode **)op=img->free_opcode;
	img->free_opcode=op;
}

FR2CI_Trace *FR2CI_AllocTrace(FR2CI_Image *img)
{
	FR2CI_TraceSlab *slab;
	FR2CI_Trace *tmp;
	int i;

	if(img->free_trace)
	{
		tmp=img->free_trace;
		img->free_trace=*(FR2CI_Trace **)tmp;
		memset(tmp, 0, sizeof(FR2CI_Trace));
		return(tmp);
	}
	
	slab=fr2ci_malloc(sizeof(FR2CI_TraceSlab));
	slab->next=img->slab_trace;
	img->slab_trace=slab;
	
	for(i=0; i<256; i++)
	{
		tmp=&(slab->tr[i]);
		*(FR2CI_Trace **)tmp=img->free_trace;
		img->free_trace=tmp;
	}
	
	tmp=img->free_trace;
	img->free_trace=*(FR2CI_Trace **)tmp;
	memset(tmp, 0, sizeof(FR2CI_Trace));
	return(tmp);
}

void FR2CI_FreeTrace(FR2CI_Image *img, FR2CI_Trace *op)
{
	*(FR2CI_Trace **)op=img->free_trace;
	img->free_trace=op;
}

FR2CI_TailOpcode *FR2CI_AllocTailOpcode(FR2CI_Image *img)
{
	FR2CI_TailOpcodeSlab *slab;
	FR2CI_TailOpcode *tmp;
	int i;

	if(img->free_tailopcode)
	{
		tmp=img->free_tailopcode;
		img->free_tailopcode=*(FR2CI_TailOpcode **)tmp;
		memset(tmp, 0, sizeof(FR2CI_TailOpcode));
		return(tmp);
	}
	
	slab=fr2ci_malloc(sizeof(FR2CI_TailOpcodeSlab));
	slab->next=img->slab_tailopcode;
	img->slab_tailopcode=slab;
	
	for(i=0; i<256; i++)
	{
		tmp=&(slab->op[i]);
		*(FR2CI_TailOpcode **)tmp=img->free_tailopcode;
		img->free_tailopcode=tmp;
	}
	
	tmp=img->free_tailopcode;
	img->free_tailopcode=*(FR2CI_TailOpcode **)tmp;
	memset(tmp, 0, sizeof(FR2CI_TailOpcode));
	return(tmp);
}

void FR2CI_FreeTailOpcode(FR2CI_Image *img, FR2CI_TailOpcode *op)
{
	*(FR2CI_TailOpcode **)op=img->free_tailopcode;
	img->free_tailopcode=op;
}

s16 fr2ci_getInt16BE(byte *ptr)
	{ return((ptr[0]<<8)|(ptr[1])); }

u16 fr2ci_getUInt16BE(byte *ptr)
	{ return((ptr[0]<<8)|(ptr[1])); }

s32 fr2ci_getInt24BE(byte *ptr)
{
	s32 t;
	t=(ptr[0]<<16)|(ptr[1]<<8)|(ptr[2]);
	t=(t<<8)>>8;
	return((ptr[0]<<16)|(ptr[1]<<8)|(ptr[2]));
}

u32 fr2ci_getUInt24BE(byte *ptr)
	{ return((ptr[0]<<16)|(ptr[1]<<8)|(ptr[2])); }

s32 fr2ci_getInt32BE(byte *ptr)
	{ return((ptr[0]<<24)|(ptr[1]<<16)|(ptr[2]<<8)|(ptr[3])); }

u32 fr2ci_getUInt32BE(byte *ptr)
	{ return((ptr[0]<<24)|(ptr[1]<<16)|(ptr[2]<<8)|(ptr[3])); }

s64 fr2ci_getInt64BE(byte *ptr)
{
	return((((s64)fr2ci_getInt32BE(ptr+0))<<32)|
		fr2ci_getUInt32BE(ptr+4));
}

u64 fr2ci_getUInt64BE(byte *ptr)
{
	return((((u64)fr2ci_getUInt32BE(ptr+0))<<32)|
		fr2ci_getUInt32BE(ptr+4));
}

s64 fr2ci_getInt56BE(byte *ptr)
{
	return((((s64)fr2ci_getInt24BE(ptr+0))<<32)|
		fr2ci_getUInt32BE(ptr+3));
}

u64 fr2ci_getUInt56BE(byte *ptr)
{
	return((((u64)fr2ci_getUInt24BE(ptr+0))<<32)|
		fr2ci_getUInt32BE(ptr+3));
}

s32 FR2CI_GetImageLiteralInt32(FR2CI_Image *img, int idx)
	{ return(fr2ci_getInt32BE(img->lvt4+idx*4)); }

u32 FR2CI_GetImageLiteralUInt32(FR2CI_Image *img, int idx)
	{ return(fr2ci_getUInt32BE(img->lvt4+idx*4)); }

s64 FR2CI_GetImageLiteralInt64(FR2CI_Image *img, int idx)
	{ return(fr2ci_getInt64BE(img->lvt8+idx*8)); }

u64 FR2CI_GetImageLiteralUInt64(FR2CI_Image *img, int idx)
	{ return(fr2ci_getUInt64BE(img->lvt8+idx*8)); }

f32 FR2CI_GetImageLiteralFloat32(FR2CI_Image *img, int idx)
{
	u32 t;
	t=fr2ci_getUInt32BE(img->lvt4+idx*4);
	return(*(f32 *)(&t));
}

f64 FR2CI_GetImageLiteralFloat64(FR2CI_Image *img, int idx)
{
	u64 t;
	t=fr2ci_getUInt64BE(img->lvt8+idx*8);
	return(*(f64 *)(&t));
}

void FR2CI_SplitIndexValuePrefix(int idx, int *rval, int *rpfx)
{
	int v, pfn, pfeb, pf;
	int i, j, k;
	
//	i=fr2ci_getInt32BE(img->lvt4+idx*4);
	v=idx; pfn=0; pfeb=0;
	//eat 1 bits
	while(v&1)
		{ v=v>>1; pfn++; }
	v=v>>1;		//eat 0 bit
	pfeb=pfn?pfn:1;
	pf=(pfn<<pfeb)|(v&((1<<pfeb)-1));
	v=v>>pfeb;
	
	*rval=v;
	*rpfx=pf;
}

void *FR2CI_GetImageLiteralPointer(FR2CI_Image *img, int idx)
{
	void *ptr;
	int pf, vi;
	
	FR2CI_SplitIndexValuePrefix(idx, &vi, &pf);
	switch(pf)
	{
	case 0:
		ptr=FR2CI_GetImageLiteralTablePointer(img, vi);
		break;
	case 1:
		ptr=img->strtab+vi;
		break;
	case 2:
		ptr=img->wstrtab2+vi;
		break;
	case 3:
		ptr=FR2CI_GetImageGlobalTableObjPointer(img, vi);
		break;
	case 4:
		ptr=img->gbl+vi;
		break;
	default:
		ptr=NULL;
	}
	
	return(ptr);
}

void *FR2CI_GetImageLiteralTablePointer(FR2CI_Image *img, int idx)
{
	FR2CI_LiteralInfo *tmp;
	void *ptr;
	
	ptr=img->litv[idx].p;
	if(ptr)
		{ return(ptr); }

	tmp=FR2CI_DecodeLiteralTableEntry(img, idx);
	ptr=img->litv[idx].p;
	if(ptr)
		{ return(ptr); }
	img->litv[idx].p=tmp;
	return(tmp);
}

FR2CI_LiteralInfo *FR2CI_DecodeLiteralTableEntry(FR2CI_Image *img, int idx)
{
	FR2CI_LiteralInfo *tmp, *tmp2;
	byte *cs, *dcs;
	int offs, sz, fcc, dsz;
	int i, j, k, n;
	
	tmp=img->litab[idx];
	if(tmp)
		{ return(tmp); }

	tmp=NULL;
//	offs=fr2ci_getInt32BE(img->lidx+idx*4);
	offs=FR2CI_GetLIDX(img, idx);
	cs=img->ldef+offs;

	if(cs[0]==0xE3)
	{
		sz=(cs[1]<<16)|(cs[2]<<8)|(cs[3]);
		fcc=fr2ci_getInt16BE(cs+4);
		dsz=sz-6;
		dcs=cs+6;

		if(	(fcc==FR2C_TWOCC_SR) ||
			(fcc==FR2C_TWOCC_SU))
		{
			tmp=FR2CI_DecodeStruct(img, fcc, dcs, dsz);
		}
	}

	if(cs[0]==0xE4)
	{
		sz=(cs[1]<<16)|(cs[2]<<8)|(cs[3]);
		fcc=fr2ci_getInt32BE(cs+4);
		dsz=sz-8;
		dcs=cs+8;
		
		if(fcc==FR2C_FOURCC_ARR1)
		{
			tmp=FR2CI_AllocFR2CI_LiteralInfo(img);
			tmp->tagdata=cs;
			tmp->litdata=FR2CI_ImagePoolAlloc(img, dsz);
			memcpy(tmp->litdata, dcs, dsz);
		}

		if(fcc==FR2C_FOURCC_ARR2)
		{
			n=dsz/2;

			tmp=FR2CI_AllocFR2CI_LiteralInfo(img);
			tmp->tagdata=cs;
			tmp->litdata=FR2CI_ImagePoolAlloc(img, n*sizeof(s16));
			for(i=0; i<n; i++)
			{
				((s16 *)tmp->litdata)[i]=fr2ci_getInt16BE(dcs+i*2);
			}
//			memcpy(tmp->litdata, cs, sz);
		}

		if(fcc==FR2C_FOURCC_ARR4)
		{
			n=dsz/4;
			tmp=FR2CI_AllocFR2CI_LiteralInfo(img);
			tmp->tagdata=cs;
			tmp->litdata=FR2CI_ImagePoolAlloc(img, n*sizeof(s32));
			for(i=0; i<n; i++)
			{
				((s32 *)tmp->litdata)[i]=fr2ci_getInt32BE(dcs+i*4);
			}
//			memcpy(tmp->litdata, cs, sz);
		}

		if(fcc==FR2C_FOURCC_ARR8)
		{
			n=dsz/8;
			tmp=FR2CI_AllocFR2CI_LiteralInfo(img);
			tmp->tagdata=cs;
			tmp->litdata=FR2CI_ImagePoolAlloc(img, n*sizeof(s64));
			for(i=0; i<n; i++)
			{
				((s64 *)tmp->litdata)[i]=fr2ci_getInt64BE(dcs+i*4);
			}
//			memcpy(tmp->litdata, cs, sz);
		}

		if(fcc==FR2C_FOURCC_ARRP)
		{
			n=dsz/4;
			tmp=FR2CI_AllocFR2CI_LiteralInfo(img);
			tmp->tagdata=cs;
			tmp->litdata=FR2CI_ImagePoolAlloc(img, n*sizeof(void *));
			for(i=0; i<n; i++)
			{
//				((s64 *)tmp->litdata)[i]=fr2ci_getInt64BE(cs+i*4);
			}
//			memcpy(tmp->litdata, cs, sz);
		}

		if(fcc==FR2C_FOURCC_ASTD)
		{
			i=fr2ci_getInt32BE(dcs);
			tmp2=FR2CI_DecodeLiteralTableEntry(img, i);
			if(!tmp2)
				{ return(NULL); }
		}

		if(	(fcc==FR2C_FOURCC_DSTR) ||
			(fcc==FR2C_FOURCC_DSTU))
		{
			tmp=FR2CI_DecodeStruct(img, fcc, cs+8, sz-8);
		}
	}

	if(cs[0]==0xE5)
	{
		sz=cs[1];
		fcc=fr2ci_getInt16BE(cs+2);
		dsz=sz-4;
		dcs=cs+4;

		if(	(fcc==FR2C_TWOCC_SR) ||
			(fcc==FR2C_TWOCC_SU))
		{
			tmp=FR2CI_DecodeStruct(img, fcc, dcs, dsz);
		}
	}

	if(tmp)
	{
		img->litab[idx]=tmp;
		img->litv[idx].p=tmp->litdata;
		return(tmp);
	}
	return(NULL);
}

void FR2CI_InitLiteralInfo(FR2CI_Image *img,
	FR2CI_LiteralInfo *tmp, FR2CI_Value *init)
{
	byte *cs, *dcs;
	void *p;
	int offs, sz, fcc, dsz;
	int i, j, k, n;
	
	cs=tmp->tagdata;
	if(cs[0]==0xE4)
	{
		sz=(cs[1]<<16)|(cs[2]<<8)|(cs[3]);
		fcc=fr2ci_getInt32BE(cs+4);
		dsz=sz-8;
		dcs=cs+8;

		if(fcc==FR2C_FOURCC_ARRP)
		{
			n=dsz/4;
			for(i=0; i<n; i++)
			{
				j=fr2ci_getInt32BE(cs+i*4);
				p=FR2CI_GetImageLiteralPointer(img, j);
				((void **)tmp->litdata)[i]=p;
			}
//			memcpy(tmp->litdata, cs, sz);
		}
	}
}

void *FR2CI_GetImageGlobalTableObjPointer(FR2CI_Image *img, int idx)
{
	void *ptr;
	
	ptr=img->gbl[idx].p;
	if(ptr)
		{ return(ptr); }
	return(NULL);
}

void *FR2CI_ImagePoolAlloc(FR2CI_Image *img, int sz)
{
	byte *pb, *pt;
	void *p;
	int xi, xn;
	int i, j;
	
	xi=(sz+15)>>4;
	if(xi<64)
	{
		if(img->free_pool[xi])
		{
			p=img->free_pool[xi];
			img->free_pool[xi]=*(void **)p;
			memset(p, 0, sz);
			return(p);
		}
		
		xn=65536/(xi<<4);
		pb=fr2ci_malloc(xn*xi*16);
		pt=pb;
		for(i=0; i<xn; i++)
		{
			*(void **)pt=img->free_pool[xi];
			img->free_pool[xi]=(void *)pt;
			pt+=xi<<4;
		}

		p=img->free_pool[xi];
		img->free_pool[xi]=*(void **)p;
		memset(p, 0, sz);
		return(p);
	}
	
	return(fr2ci_malloc(sz));
}

void FR2CI_ImagePoolFree(FR2CI_Image *img, void *ptr, int sz)
{
	int xi, xn;

	xi=(sz+15)>>4;
	if(xi<64)
	{
		*(void **)ptr=img->free_pool[xi];
		img->free_pool[xi]=(void *)ptr;
		return;
	}
	
	fr2ci_free(ptr);
}

FR2CI_ArgOpcode *FR2CI_AllocArgOpcode(FR2CI_Image *img)
{
	return(FR2CI_ImagePoolAlloc(img, sizeof(FR2CI_ArgOpcode)));
}

void FR2CI_FreeArgOpcode(FR2CI_Image *img, FR2CI_ArgOpcode *op)
{
	FR2CI_ImagePoolFree(img, op, sizeof(FR2CI_ArgOpcode));
}

FR2CI_CallInfo *FR2CI_AllocCallInfo(FR2CI_Image *img)
{
	return(FR2CI_ImagePoolAlloc(img, sizeof(FR2CI_CallInfo)));
}

void FR2CI_FreeCallInfo(FR2CI_Image *img, FR2CI_CallInfo *op)
{
	FR2CI_ImagePoolFree(img, op, sizeof(FR2CI_CallInfo));
}

FR2CI_Function *FR2CI_AllocFunction(FR2CI_Image *img)
{
	return(FR2CI_ImagePoolAlloc(img, sizeof(FR2CI_Function)));
}

void FR2CI_FreeFunction(FR2CI_Image *img, FR2CI_Function *op)
{
	FR2CI_ImagePoolFree(img, op, sizeof(FR2CI_Function));
}

FR2CI_VarInfo *FR2CI_AllocVarInfo(FR2CI_Image *img)
{
	return(FR2CI_ImagePoolAlloc(img, sizeof(FR2CI_VarInfo)));
}

void FR2CI_FreeVarInfo(FR2CI_Image *img, FR2CI_VarInfo *op)
{
	FR2CI_ImagePoolFree(img, op, sizeof(FR2CI_VarInfo));
}

FR2CI_Function *FR2CI_ImageGetFunction(FR2CI_Image *img, int idx)
{
	FR2CI_Function *tmp;
	byte *cs;
	int offs, sz, fcc;
	
	tmp=img->gbl[idx].p;
	if(tmp)
		{ return(tmp); }

	tmp=NULL;
//	offs=fr2ci_getInt32BE(img->gidx+idx*4);
	offs=FR2CI_GetGIDX(img, idx);
	cs=img->gdef+offs;

	if(cs[0]==0xE3)
	{
		sz=(cs[1]<<16)|(cs[2]<<8)|(cs[3]);
		fcc=fr2ci_getInt16BE(cs+4);
		if(fcc==FR2C_TWOCC_FN)
		{
			tmp=FR2CI_DecodeFunction(img, cs+6, sz-6);
			img->gbl[idx].p=tmp;
		}
	}

	if(cs[0]==0xE4)
	{
		sz=(cs[1]<<16)|(cs[2]<<8)|(cs[3]);
		fcc=fr2ci_getInt32BE(cs+4);
		if(fcc==FR2C_FOURCC_FUNC)
		{
			tmp=FR2CI_DecodeFunction(img, cs+8, sz-8);
			img->gbl[idx].p=tmp;
		}
	}

	if(cs[0]==0xE5)
	{
		sz=cs[1];
		fcc=fr2ci_getInt16BE(cs+2);
		if(fcc==FR2C_TWOCC_FN)
		{
			tmp=FR2CI_DecodeFunction(img, cs+4, sz-4);
			img->gbl[idx].p=tmp;
		}
	}

	return(tmp);
}

FR2CI_Function *FR2CI_ImageGetExportFunction(
	FR2CI_Image *img, char *name, char *sig)
{
	int i;

	for(i=0; i<img->n_gbl; i++)
	{
		if(!img->gbl_func[i])
			continue;
		if(!img->gbl_func[i]->name)
			continue;
		if(	(!img->gbl_func[i]->text ||
				(img->gbl_func[i]->sz_text<=0)) &&
				!img->gbl_func[i]->natfcn)
			continue;
		if(!strcmp(img->gbl_func[i]->name, name))
			return(img->gbl_func[i]);
	}

	return(NULL);
}

FR2CI_LiteralInfo *FR2CI_AllocFR2CI_LiteralInfo(FR2CI_Image *img)
{
	return(FR2CI_ImagePoolAlloc(img, sizeof(FR2CI_LiteralInfo)));
}

void FR2CI_FreeFR2CI_LiteralInfo(FR2CI_Image *img, FR2CI_LiteralInfo *op)
{
	FR2CI_ImagePoolFree(img, op, sizeof(FR2CI_LiteralInfo));
}
