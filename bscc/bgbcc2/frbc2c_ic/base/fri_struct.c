#include <frbc2ci.h>

FR2CI_LiteralInfo *FR2CI_DecodeStruct(FR2CI_Image *img,
	u32 fcc, byte *data, int dsz)
{
	FR2CI_VarInfo *t_field[1024];
	FR2CI_LiteralInfo *tmp;
	byte *cs, *cse;
	int sz, al;
	int i, j, k;
	
	tmp=FR2CI_AllocFR2CI_LiteralInfo(img);
	tmp->image=img;
	tmp->tag=fcc;
	tmp->field=t_field;
	tmp->n_field=0;
	
	cs=data; cse=data+dsz;
	while(cs<cse)
	{
		switch(*cs++)
		{
		case FR2C_TAG_NAME:
			tmp->name=FR2CI_DecodeMarkerString(img, &cs, cse);
			break;
		case FR2C_TAG_SIG:
			tmp->sig=FR2CI_DecodeMarkerString(img, &cs, cse);
			break;
		case FR2C_TAG_FLAGS:
			tmp->flagstr=FR2CI_DecodeMarkerString(img, &cs, cse);
			break;

#if 0
//		case FR2C_TAG_LIBNAME:
//			tmp->libname=FR2CI_DecodeMarkerString(img, &cs, cse);
//			break;
//		case FR2C_TAG_NLOCALS:
//			tmp->nlocals=FR2CI_DecodeVLIE(&cs);
//			break;
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
#endif

		case FR2C_TAG_TWOCC_L24:
			i=(cs[0]<<16)|(cs[1]<<8)|cs[2]; cs+=3;
			j=(cs[0]<<8)|(cs[1]); cs+=2;
			FR2CI_DecodeStructMarkerTWOCC(img, tmp, (u16)j, cs+6, cs+i);
			cs+=i-6;
			break;
		case FR2C_TAG_FOURCC_L24:
			i=(cs[0]<<16)|(cs[1]<<8)|cs[2]; cs+=3;
			j=(cs[0]<<24)|(cs[1]<<16)|(cs[2]<<8)|(cs[3]); cs+=4;
			FR2CI_DecodeStructMarkerFOURCC(img, tmp, (u32)j, cs+8, cs+i);
			cs+=i-8;
			break;
		case FR2C_TAG_TWOCC_L8:
			i=*cs++;
			j=(cs[0]<<8)|(cs[1]); cs+=2;
			FR2CI_DecodeStructMarkerTWOCC(img, tmp, (u16)j, cs+4, cs+i);
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

	tmp->st_size=0;
	tmp->st_align=0;
	tmp->field=FR2CI_ImagePoolAlloc(img,
		tmp->n_field*sizeof(FR2CI_VarInfo *));

	for(i=0; i<tmp->n_field; i++)
	{
		tmp->field[i]=t_field[i];
		sz=t_field[i]->vi_size;
		al=t_field[i]->vi_align;
		
		if(al>tmp->st_align)
			tmp->st_align=al;

		if((tmp->tag!=FR2C_FOURCC_DSTU) &&
			(tmp->tag!=FR2C_TWOCC_SU))
		{
			tmp->st_size=(tmp->st_size+(al-1))&(~(al-1));
			t_field[i]->vi_offset=tmp->st_size;
			tmp->st_size+=sz;
		}else
		{
			t_field[i]->vi_offset=0;
			if(sz>tmp->st_size)
				tmp->st_size=sz;
		}
	}

	al=tmp->st_align;
	tmp->st_size=(tmp->st_size+(al-1))&(~(al-1));

	return(tmp);
}

int FR2CI_DecodeStructMarkerTWOCC(
	FR2CI_Image *img, FR2CI_LiteralInfo *str,
	u16 tcc, byte *css, byte *cse)
{
	switch(tcc)
	{
	case FR2C_TWOCC_DF:
		FR2CI_DecodeStructDefField(img, str, css, cse);
		break;
	default:
		break;
	}
	return(0);
}

int FR2CI_DecodeStructMarkerFOURCC(
	FR2CI_Image *img, FR2CI_LiteralInfo *str,
	u32 fcc, byte *css, byte *cse)
{
	switch(fcc)
	{
//	case FR2C_FOURCC_GDEF:
//		img->gdef=css;		img->sz_gdef=cse-css;
//		break;
	default:
		break;
	}
	return(0);
}

int FR2CI_DecodeStructDefField(
	FR2CI_Image *img, FR2CI_LiteralInfo *str,
	byte *css, byte *cse)
{
	FR2CI_VarInfo *dfi;
	byte *cs;
	
//	dfi=FR2CI_AllocVarInfo(img);
	dfi=FR2CI_DecodeVarInfo(img, css, cse-css);
	str->field[str->n_field++]=dfi;
	return(0);
	
//	cs=css;
//	while(cs<cse)
//	{
//	}
}

FR2CI_LiteralInfo *FR2CI_LookupStructName(FR2CI_Image *img, char *name)
{
	return(NULL);
}

int FR2CI_GetStructSigSizeAlign(
	FR2CI_Image *img, char *sig, int *rsz, int *ral)
{
	char tb[256];
	FR2CI_LiteralInfo *str;
	int objid;
	char *s, *t;

	if(!sig || (*sig!='X'))
	{
		*rsz=0; *ral=0;
		return(-1);
	}
	
	if((sig[1]>='0') && (sig[1]<='9'))
	{
		s=sig+1; objid=0;
		while(*s && (*s!=';'))
		{
			if((*s>='0') && (*s<='9'))
				{ objid=(objid*10)+((*s++)-'0'); }
			break;
		}
		
		str=FR2CI_DecodeLiteralTableEntry(img, objid);
		if(str)
		{
			*rsz=str->st_size;
			*ral=str->st_align;
			return(0);
		}
		return(-1);
	}
	
	s=sig+1; t=tb;
	while(*s && (*s!=';'))
		{ *t++=*s++; }
		
	str=FR2CI_LookupStructName(img, tb);
	if(str)
	{
		*rsz=str->st_size;
		*ral=str->st_align;
		return(0);
	}
	return(-1);
}

int FR2CI_GetSigSizeAlign(
	FR2CI_Image *img, char *sig, int *rsz, int *ral)
{
	int asz[16];
	char *s;
	int sz, al, na;
	int i, j, k;

	switch(*sig)
	{
	case 'a':	case 'b':
	case 'c':	case 'h':
		sz=sizeof(byte);
		al=sizeof(byte);
		break;
	case 'i':	case 'j':
		sz=sizeof(s32);
		al=sizeof(s32);
		break;
	case 'l':	case 'm':
		sz=sizeof(long);
		al=sizeof(long);
		break;
	case 'x':	case 'y':
		sz=sizeof(s64);
		al=sizeof(s64);
		break;
	case 'f':
		sz=sizeof(f32);
		al=sizeof(f32);
		break;
	case 'd':
		sz=sizeof(f64);
		al=sizeof(f64);
		break;
	case 'P':
		sz=sizeof(void *);
		al=sizeof(void *);
		break;
	case 'A':
		s=sig+1; na=0; i=0;
		while(*s)
		{
			if((*s>='0') && (*s<='9'))
				{ i=i*10+((*s++)-'0'); continue; }
			if(*s==',')
				{ asz[na++]=i; i=0; s++; continue; }
			if(*s==';')
				{ asz[na++]=i; i=0; s++; break; }
			asz[na++]=i;
			break;
		}
		FR2CI_GetSigSizeAlign(img, s, &sz, &al);
		j=1;
		for(i=0; i<na; i++)
			j*=asz[i];
		if(sz<al)sz=al;
		sz=j*sz;
		break;
	case 'X':
		FR2CI_GetStructSigSizeAlign(img, sig, &sz, &al);
		break;
	}
	
	*rsz=sz;
	*ral=al;
	return(0);
}
