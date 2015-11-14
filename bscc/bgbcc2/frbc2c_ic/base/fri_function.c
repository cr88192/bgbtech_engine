#include <frbc2ci.h>

int FR2CI_DecodeCharUTF8(byte **rcs)
{
	byte *cs;
	int i, j, k;

	cs=*rcs;
	i=*cs++;
	if(i<0x80)
		{ j=i; }
	else if(i<0xC0)
		{ j=-(i&63); }
	else if(i<0xE0)
	{
		//110x-xxxx 10xx-xxxx
		j=((i&31)<<6)|((*cs++)&63);
	}
	else if(i<0xF0)
	{
		//1110-xxxx 10xx-xxxx 10xx-xxxx
		j=((i&15)<<12)|((cs[0]&63)<<6)|(cs[1]&63); cs+=2;
	}
	else if(i<0xF8)
	{
		//1111-0xxx 10xx-xxxx 10xx-xxxx 10xx-xxxx
		j=((i&7)<<18)|((cs[0]&63)<<12)|((cs[1]&63)<<6)|(cs[2]&63); cs+=3;
	}
	
	*rcs=cs;
	return(j);
}

s64 FR2CI_DecodeVLIE(byte **rcs)
{
	byte *cs;
	s64 li;
	int i, j, k;

	cs=*rcs;
	i=*cs++;
	if(i<0x80)
		{ li=i; }
	else if(i<0xC0)
		{ li=-(i&63); }
	else if(i<0xE0)
	{
		//110x-xxxx xxxx-xxxx
		li=((i&31)<<8)|(*cs++);
	}
	else if(i<0xF0)
	{
		//1110-xxxx xxxx-xxxx xxxx-xxxx
		li=((i&15)<<16)|(cs[0]<<8)|(cs[1]); cs+=2;
	}
	else if(i<0xF8)
	{
		//1111-0xxx xxxx-xxxx xxxx-xxxx xxxx-xxxx
		li=((i&7)<<24)|(cs[0]<<16)|(cs[1]<<16)|(cs[2]); cs+=3;
	}
	else if(i<0xFC)
	{
		//1111-10xx xxxx-xxxx xxxx-xxxx xxxx-xxxx xxxx-xxxx
		j=(i&3);
		k=(cs[0]<<24)|(cs[1]<<16)|(cs[2]<<16)|(cs[3]);
		cs+=4;
		li=(((s64)j)<<32)|k;
	}
	else if(i<0xFE)
	{
		//1111-110x xxxx-xxxx(x5)
		j=((i&1)<<8)|(cs[0]);
		cs+=1;
		k=(cs[0]<<24)|(cs[1]<<16)|(cs[2]<<16)|(cs[3]);
		cs+=4;
		li=(((s64)j)<<32)|k;
	}
	else if(i<0xFF)
	{
		//1111-1110 xxxx-xxxx(x6)
		j=((i&1)<<16)|(cs[0]<<8)|(cs[1]);
		cs+=2;
		k=(cs[0]<<24)|(cs[1]<<16)|(cs[2]<<16)|(cs[3]);
		cs+=4;
		li=(((s64)j)<<32)|k;
	}else
	{
		i=*cs++;
		if(i<0x80)
		{
			//1111-1111 0xxx-xxxx xxxx-xxxx(x6)
//			j=((i&127)<<24)|(cs[0]<<16)|(cs[1]<<8)|(cs[2]);
//			cs+=3;

			j=((i&127)<<16)|(cs[0]<<8)|(cs[1]);
			cs+=2;
			k=(cs[0]<<24)|(cs[1]<<16)|(cs[2]<<16)|(cs[3]);
			cs+=4;
			li=(((s64)j)<<32)|k;
		}else if(i<0xC0)
		{
			//1111-1111 10xx-xxxx xxxx-xxxx(x7)
			j=((i&63)<<24)|(cs[0]<<16)|(cs[1]<<8)|(cs[2]);
			cs+=3;
			k=(cs[0]<<24)|(cs[1]<<16)|(cs[2]<<16)|(cs[3]);
			cs+=4;
			li=(((s64)j)<<32)|k;
		}else if(i<0xE0)
		{
			//1111-1111 110x-xxxx xxxx-xxxx(x8)
//			j=((i&63)<<24)|(cs[0]<<16)|(cs[1]<<8)|(cs[2]);
//			cs+=3;
			j=(cs[0]<<24)|(cs[1]<<16)|(cs[2]<<16)|(cs[3]);
			cs+=4;
			k=(cs[0]<<24)|(cs[1]<<16)|(cs[2]<<16)|(cs[3]);
			cs+=4;
			li=(((s64)j)<<32)|k;
		}else
		{
			li=-1;
		}
	}
	
	*rcs=cs;
	return(li);
}

s64 FR2CI_DecodeSVLIE(byte **rcs)
{
	s64 li, lj;
	li=FR2CI_DecodeVLIE(rcs);
	lj=(li>>1)^((li<<63)>>63);
	return(lj);
}

char *FR2CI_DecodeMarkerString(FR2CI_Image *img,
	byte **rcs, byte *cse)
{
	char *cst;
	byte *cs;
	int i, j, k;
	
	cs=*rcs;
	switch(*cs++)
	{
	case FR2C_TAG_STRING:
	case FR2C_TAG_KEYWORD:
		cst=(char *)cs;
		cs+=strlen(cs)+1;
		break;
	case FR2C_TAG_STRINGREF:
	case FR2C_TAG_KEYWORDREF:
		i=FR2CI_DecodeVLIE(&cs);
		cst=(char *)(img->strtab+i);
		break;
	default:
		cst=NULL;
		break;
	}
	
	*rcs=cs;
	return(cst);
}

char *FR2CI_DecodeIndexString(FR2CI_Image *img,
	byte **rcs, byte *cse)
{
	char *cst;
	byte *cs;
	int i;

	i=FR2CI_DecodeVLIE(rcs);
	cst=(char *)(img->strtab+i);
	return(cst);
}

FR2CI_Function *FR2CI_DecodeFunction(FR2CI_Image *img,
	byte *data, int sz)
{
	FR2CI_Function *tmp;
	byte *cs, *cse;
	int i, j, k;
	
//	tmp=fr2ci_malloc(sizeof(FR2CI_Function));
	tmp=FR2CI_AllocFunction(img);
	tmp->image=img;
	
	cs=data; cse=data+sz;
	while(cs<cse)
	{
		switch(*cs++)
		{
		case FR2C_TAG_NAME:
//			tmp->name=FR2CI_DecodeMarkerString(img, &cs, cse);
			tmp->name=FR2CI_DecodeIndexString(img, &cs, cse);
			break;
		case FR2C_TAG_SIG:
//			tmp->sig=FR2CI_DecodeMarkerString(img, &cs, cse);
			tmp->sig=FR2CI_DecodeIndexString(img, &cs, cse);
			break;
		case FR2C_TAG_FLAGS:
//			tmp->flagstr=FR2CI_DecodeMarkerString(img, &cs, cse);
			tmp->flagstr=FR2CI_DecodeIndexString(img, &cs, cse);
			break;
		case FR2C_TAG_LIBNAME:
//			tmp->libname=FR2CI_DecodeMarkerString(img, &cs, cse);
			tmp->libname=FR2CI_DecodeIndexString(img, &cs, cse);
			break;
		case FR2C_TAG_NLOCALS:
			tmp->nlocals=FR2CI_DecodeVLIE(&cs);
			break;
		case FR2C_TAG_NREGS:
			tmp->nregs=FR2CI_DecodeVLIE(&cs);
			break;
		case FR2C_TAG_NARGS:
			tmp->nargs=FR2CI_DecodeVLIE(&cs);
			break;
		case FR2C_TAG_NCARGS:
			tmp->ncargs=FR2CI_DecodeVLIE(&cs);
			break;
		case FR2C_TAG_VALUE:
			tmp->value=FR2CI_DecodeVLIE(&cs);
			break;
		case FR2C_TAG_IMGDATA_L24:
			i=(cs[0]<<16)|(cs[1]<<8)|cs[2]; cs+=3;
			tmp->text=cs; tmp->sz_text=i-4;
			cs+=i-4;
			break;
		case FR2C_TAG_IMGDATA_L8:
			i=*cs++;
			tmp->text=cs; tmp->sz_text=i-2;
			cs+=i-2;
			break;
		case FR2C_TAG_TWOCC_L24:
			i=(cs[0]<<16)|(cs[1]<<8)|cs[2]; cs+=3;
			j=(cs[0]<<8)|(cs[1]); cs+=2;
			cs+=i-6;
			break;
		case FR2C_TAG_FOURCC_L24:
			i=(cs[0]<<16)|(cs[1]<<8)|cs[2]; cs+=3;
			j=(cs[0]<<24)|(cs[1]<<16)|(cs[2]<<8)|(cs[3]); cs+=4;
			cs+=i-8;
			break;
		case FR2C_TAG_TWOCC_L8:
			i=*cs++;
			j=(cs[0]<<8)|(cs[1]); cs+=2;
			cs+=i-4;
			break;
		case FR2C_TAG_EIGHTCC_L24:
			i=(cs[0]<<16)|(cs[1]<<8)|cs[2]; cs+=3;
			j=(cs[0]<<24)|(cs[1]<<16)|(cs[2]<<8)|(cs[3]);
			k=(cs[4]<<24)|(cs[5]<<16)|(cs[6]<<8)|(cs[7]);
			cs+=8;
			cs+=i-12;
			break;
		default:
			break;
		}
	}
	
	if(tmp->text)
	{
		tmp->tag=FR2C_FOURCC_FUNC;
	}else
	{
		tmp->tag=FR2C_FOURCC_DVAR;
	}
	
	return(tmp);
}

FR2CI_VarInfo *FR2CI_DecodeVarInfo(FR2CI_Image *img,
	byte *data, int dsz)
{
	FR2CI_VarInfo *tmp;
	int sz, al;

	tmp=FR2CI_DecodeFunction(img, data, dsz);

	if(tmp->sig)
	{
		FR2CI_GetSigSizeAlign(img, tmp->sig, &sz, &al);
		tmp->vi_size=sz;
		tmp->vi_align=al;
	}

	return(tmp);
}

void FR2CI_InitVarInfo(FR2CI_Image *img,
	FR2CI_VarInfo *tmp, FR2CI_Value *init)
{
	if(init && tmp->sig)
	{
		switch(*tmp->sig)
		{
		case 'a':	case 'b':
		case 'c':	case 'h':
		case 'i':	case 'j':
			init->i=FR2CI_GetImageLiteralInt32(img, tmp->value);
			break;
		case 'l':	case 'm':
		case 'x':	case 'y':
			init->l=FR2CI_GetImageLiteralInt64(img, tmp->value);
			break;
		case 'f':
			init->f=FR2CI_GetImageLiteralFloat32(img, tmp->value);
			break;
		case 'd':
			init->d=FR2CI_GetImageLiteralFloat64(img, tmp->value);
			break;
		case 'P':
			init->p=FR2CI_GetImageLiteralPointer(img, tmp->value);
			break;
		case 'A':
			init->p=FR2CI_GetImageLiteralPointer(img, tmp->value);
			break;
		}
	}
}

FR2CI_Trace *FR2CI_GetFunctionEntryTrace(FR2CI_Function *func)
{
	if(func->entry)
		return(func->entry);
	if(func->libname)
		{ return(FR2CI_GetFunctionImportEntryTrace(func)); }
	return(FR2CI_DecodeFunctionTraces(func));
}

FR2CI_Trace *FR2CI_GetFunctionImportEntryTrace(FR2CI_Function *func)
{
	FR2CI_Image *img2;
	FR2CI_Function *func2;
	FR2CI_Trace *tr;
	
	img2=FR2CI_LoadImageName(func->image->loader, func->libname);
	if(!img2)
		return(NULL);
	func2=FR2CI_ImageGetExportFunction(img2, func->name, func->sig);
	if(!func2)
		return(NULL);
		
	tr=FR2CI_GetFunctionEntryTrace(func2);
	if(!tr)
		return(NULL);
	
	func->entry=tr;
	return(tr);
}

FR2CI_Trace *FR2CI_LookupFunctionTrace(FR2CI_Function *func, byte *ip)
{
	int i;
	
	for(i=0; i<func->n_traces; i++)
	{
		if(func->traces[i]->ip==ip)
			return(func->traces[i]);
	}
	return(NULL);
}

FR2CI_Trace *FR2CI_DecodeFunctionTraces(FR2CI_Function *func)
{
	FR2CI_Opcode *t_ops[4096];
	FR2CI_TailOpcode *t_tops[1024];
	FR2CI_Trace *t_traces[1024];
	FR2CI_Function *func2;
	FR2CI_Opcode *op;
	FR2CI_TailOpcode *top;
	FR2CI_Trace *trace;
	byte *cs, *cse, *csip;
	int nops, ntops, ntraces, opsn, topsn, tracesn;
	int i, j, k;
	
	/* Check Incomplete */
	if(!func->text)
	{
		func2=FR2CI_LookupFunctionNameGeneric(
			func->image->loader, func->name, func->sig);
		if(func2)
		{
			trace=FR2CI_GetFunctionEntryTrace(func2);
			if(trace)
			{
				func->entry=trace;
				return(trace);
			}
		}
		
		trace=FR2CI_GetFunctionNativeCall(func->image,
			func, func->name, func->sig);
		if(trace)
			return(trace);

		return(NULL);
	}
	
	nops=0; ntops=0; ntraces=0;
	cs=func->text; cse=cs+func->sz_text;
	while(cs<cse)
	{
		csip=cs;
		opsn=nops; topsn=ntops;
		while(cs<cse)
		{
			op=FR2CI_DecodeOpcode(func->image, &cs, cse);
			if(!op)break;
			t_ops[nops++]=op;
		}
		
		top=FR2CI_DecodeTailOpcode(func->image, &cs, cse);
		if(!top)break;
		t_tops[ntops++]=top;

		trace=FR2CI_CreateTrace(func->image, t_ops+opsn, top, nops-opsn);
		t_traces[ntraces++]=trace;
		trace->ip=csip;
	}
	t_traces[ntraces]=NULL;

	if(cs<cse)
	{
		/* Decode Error */
		return(NULL);
	}
	
	func->ops=fr2ci_malloc(nops*sizeof(FR2CI_Opcode *));
	func->tops=fr2ci_malloc(ntops*sizeof(FR2CI_TailOpcode *));
	func->traces=fr2ci_malloc(ntraces*sizeof(FR2CI_Trace *));
	func->n_ops=nops;
	func->n_tops=ntops;
	func->n_traces=ntraces;
	
	for(i=0; i<nops; i++)
		{ func->ops[i]=t_ops[i]; }
	for(i=0; i<ntops; i++)
		{ func->tops[i]=t_tops[i]; }
	for(i=0; i<ntraces; i++)
	{
		trace=t_traces[i];
		func->traces[i]=trace;
		trace->op=func->ops+(trace->op-t_ops);
	}

	for(i=0; i<ntraces; i++)
	{
		trace=t_traces[i];
		trace->top->nexttrace=t_traces[i+1];		
		if(trace->top->jmpip)
		{
			trace->top->jmptrace=FR2CI_LookupFunctionTrace(
				func, trace->top->jmpip);
		}
	}
	
	func->entry=func->traces[0];
	return(func->entry);
}
