#include <bgbbtj.h>

typedef struct BGBBTJ_BTIC1_Malloc_s BGBBTJ_BTIC1_Malloc;
struct BGBBTJ_BTIC1_Malloc_s {
BGBBTJ_BTIC1_Malloc *prev, *next;
char *fn;
int ln;
int sz;
};

BGBBTJ_BTIC1_Malloc *btic_malloc_list=NULL;

char *btic_strdup(char *str)
{
	return(dyllStrdup(str));
}

void *btic_malloc_lln(int sz, char *fn, int ln)
{
	BGBBTJ_BTIC1_Malloc *tmp;
	
	tmp=malloc(sz+sizeof(BGBBTJ_BTIC1_Malloc));
	tmp->fn=fn;
	tmp->ln=ln;
	tmp->sz=sz;
	
	if(btic_malloc_list)
	{
		tmp->prev=NULL;
		btic_malloc_list->prev=tmp;
		tmp->next=btic_malloc_list;
		btic_malloc_list=tmp;
	}else
	{
		tmp->prev=NULL;
		tmp->next=btic_malloc_list;
		btic_malloc_list=tmp;
	}
	
	return(tmp+1);
}

void btic_free(void *ptr)
{
	BGBBTJ_BTIC1_Malloc *tmp;
	
	tmp=((BGBBTJ_BTIC1_Malloc *)ptr)-1;
	
	if(tmp->prev)
	{
		tmp->prev->next=tmp->next;
		if(tmp->next)
			{ tmp->next->prev=tmp->prev; }
	}else
	{
		if(tmp->next)
			{ tmp->next->prev=NULL; }
		btic_malloc_list=tmp->next;
	}
	
	free(tmp);
}

BGBBTJ_API int BTIC1_EscapeEncodeBuffer(byte *ibuf, int isz,
	byte *obuf, int osz)
{
	byte *cs, *ct, *cse, *cte;
	
	cs=ibuf; cse=ibuf+isz;
	ct=obuf; cte=obuf+osz;
	
	while((cs<cse) && (ct<cte))
	{
		if(*cs==0xFF)
		{
			if((cs[1]>=0) && (cs[1]<=30))
				{ *ct++=*cs++; *ct++=(*cs++)+1; continue; }

			if((cs[1]>=0xE0) && (cs[1]<=0xFF))
				{ *ct++=*cs++; *ct++=(*cs++)^0xC0; continue; }
			if((cs[1]>=0x20) && (cs[1]<=0x5F))
				{ *ct++=*cs++; *ct++=(*cs++)+0x20; continue; }

#if 0
			if(cs[1]==15)
			{
				*ct++=*cs++; cs++;
				*ct++=0x10; *ct++=0x00;
				continue;
			}

			if(cs[1]==16)
			{
				if((cs[2]>=0) && (cs[2]<=14))
					{ *ct++=*cs++; *ct++=*cs++; *ct++=(*cs++)+1; continue; }
				if(cs[1]==16)
				{
					*ct++=*cs++; *ct++=*cs++; cs++;
					*ct++=0x10; *ct++=0x00;
					continue;
				}
			}
#endif

			cs++;
			*ct++=0xFF;
			*ct++=0x00;
			continue;
		}
		
		*ct++=*cs++;
	}
	
	if(ct>=cte)return(-1);
	return(ct-obuf);
}

BGBBTJ_API int BTIC1_EscapeEncodeSingleBuffer(byte *buf, int sz)
{
	byte *tbuf;
	int i;
	
	tbuf=btic_malloc(sz*2);
	i=BTIC1_EscapeEncodeBuffer(buf, sz, tbuf, sz*2);
	if(i<0)return(i);
	memcpy(buf, tbuf, i);
	btic_free(tbuf);
	return(i);
}

BGBBTJ_API int BTIC1_EscapeEncodeBufferSimple(byte *ibuf, int isz,
	byte *obuf, int osz)
{
	byte *cs, *ct, *cse, *cte;
	
	cs=ibuf; cse=ibuf+isz;
	ct=obuf; cte=obuf+osz;
	
	while((cs<cse) && (ct<cte))
	{
		if(*cs==0xFF)
		{
			cs++;
			*ct++=0xFF;
			*ct++=0x00;
			continue;
		}
		
		*ct++=*cs++;
	}
	
	if(ct>=cte)return(-1);
	return(ct-obuf);
}

byte *BTIC1_EmitInt16LE(byte *ct, uint v)
{
	*ct++=v&0xFF;
	*ct++=(v>>8)&0xFF;
	return(ct);
}

byte *BTIC1_EmitInt16BE(byte *ct, uint v)
{
	*ct++=(v>>8)&0xFF;
	*ct++=v&0xFF;
	return(ct);
}

byte *BTIC1_EmitInt32LE(byte *ct, uint v)
{
	*ct++=v&0xFF;
	*ct++=(v>>8)&0xFF;
	*ct++=(v>>16)&0xFF;
	*ct++=(v>>24)&0xFF;
	return(ct);
}

byte *BTIC1_EmitInt32BE(byte *ct, uint v)
{
	*ct++=(v>>24)&0xFF;
	*ct++=(v>>16)&0xFF;
	*ct++=(v>>8)&0xFF;
	*ct++=v&0xFF;
	return(ct);
}

byte *BTIC1_EmitFourcc(byte *ct, uint v)
{
	*ct++=v&0xFF;
	*ct++=(v>>8)&0xFF;
	*ct++=(v>>16)&0xFF;
	*ct++=(v>>24)&0xFF;
	return(ct);
}

byte *BTIC1_EmitString(byte *ct, char *str)
{
	int l;
	l=strlen(str);
	memcpy(ct, str, l+1);
	return(ct+l+1);
}

byte *BTIC1_EmitMarkerLen(byte *ct, byte tag, int len)
{
	*ct++=0xFF;
	*ct++=tag;

	if((len+2)<65536)
	{
		ct=BTIC1_EmitInt16BE(ct, len+2);
	}else
	{
		ct=BTIC1_EmitInt16BE(ct, 0);
		ct=BTIC1_EmitInt32BE(ct, len+6);
	}
	return(ct);
}

byte *BTIC1_EmitLumpFMT12(byte *ct, uint fcc, byte *buf, int len)
{
	byte *tbuf;
	byte tb[4];
	int len1;

	if(!buf)
	{
		ct=BTIC1_EmitMarkerLen(ct, BGBBTJ_BTIC1_MARK_FMT12, len+4);
		ct=BTIC1_EmitFourcc(ct, fcc);
		memset(ct, 0, len);
		ct+=len;
	}else
	{
		tbuf=btic_malloc(2*len);
		len1=BTIC1_EscapeEncodeBuffer(buf, len, tbuf, 2*len);
		ct=BTIC1_EmitMarkerLen(ct, BGBBTJ_BTIC1_MARK_FMT12, len1+4);
		ct=BTIC1_EmitFourcc(ct, fcc);
		memcpy(ct, tbuf, len1);
		ct+=len1;
		btic_free(tbuf);
	}

	return(ct);
}

byte *BTIC1_EmitLumpAPP12(byte *ct, uint fcc, byte *buf, int len)
{
	byte *tbuf;
	byte tb[4];
	int len1;

	if(!buf)
	{
		ct=BTIC1_EmitMarkerLen(ct, BGBBTJ_BTIC1_MARK_APP12, len+4);
		ct=BTIC1_EmitFourcc(ct, fcc);
		memset(ct, 0, len);
		ct+=len;
	}else
	{
		tbuf=btic_malloc(2*len);
		len1=BTIC1_EscapeEncodeBuffer(buf, len, tbuf, 2*len);
		ct=BTIC1_EmitMarkerLen(ct, BGBBTJ_BTIC1_MARK_APP12, len1+4);
		ct=BTIC1_EmitFourcc(ct, fcc);
		memcpy(ct, tbuf, len1);
		ct+=len1;
		btic_free(tbuf);
	}

	return(ct);
}

byte *BTIC1_EmitLumpFMT13(byte *ct, char *tag, byte *buf, int len)
{
	byte *tbuf;
	byte tb[4];
	int len1, lent;

	if(!buf)
	{
		lent=strlen(tag)+1;
		ct=BTIC1_EmitMarkerLen(ct, BGBBTJ_BTIC1_MARK_FMT13, len+lent);
		ct=BTIC1_EmitString(ct, tag);
		memset(ct, 0, len);
		ct+=len;
	}else
	{
		tbuf=btic_malloc(2*len);
		len1=BTIC1_EscapeEncodeBuffer(buf, len, tbuf, 2*len);
		lent=strlen(tag)+1;
		ct=BTIC1_EmitMarkerLen(ct, BGBBTJ_BTIC1_MARK_FMT13, len1+lent);
		ct=BTIC1_EmitString(ct, tag);
		memcpy(ct, tbuf, len1);
		ct+=len1;
		btic_free(tbuf);
	}
	
	return(ct);
}

byte *BTIC1_EmitLumpAPP13(byte *ct, char *tag, byte *buf, int len)
{
	byte *tbuf;
	byte tb[4];
	int len1, lent;

	if(!buf)
	{
		lent=strlen(tag)+1;
		ct=BTIC1_EmitMarkerLen(ct, BGBBTJ_BTIC1_MARK_APP13, len+lent);
		ct=BTIC1_EmitString(ct, tag);
		memset(ct, 0, len);
		ct+=len;
	}else
	{
		tbuf=btic_malloc(2*len);
		len1=BTIC1_EscapeEncodeBuffer(buf, len, tbuf, 2*len);
		lent=strlen(tag)+1;
		ct=BTIC1_EmitMarkerLen(ct, BGBBTJ_BTIC1_MARK_APP13, len1+lent);
		ct=BTIC1_EmitString(ct, tag);
		memcpy(ct, tbuf, len1);
		ct+=len1;
		btic_free(tbuf);
	}
	
	return(ct);
}

#if 1
byte *BTIC1_EmitLumpFMT12Z(byte *ct, uint fcc, byte *buf, int len)
{
	return(BTIC1_EmitLumpMarkFccZ(ct,
		BGBBTJ_BTIC1_MARK_FMT12, fcc, buf, len));
}

byte *BTIC1_EmitLumpAPP12Z(byte *ct, uint fcc, byte *buf, int len)
{
	return(BTIC1_EmitLumpMarkFccZ(ct,
		BGBBTJ_BTIC1_MARK_APP12, fcc, buf, len));
}

byte *BTIC1_EmitLumpMarkFccZ(byte *ct,
	int mark, uint fcc, byte *buf, int len)
{
	byte *tbuf, *tbuf1;
	byte tb[4];
	int len1, len2;

	if(!buf || !len)
	{
		ct=BTIC1_EmitMarkerLen(ct, mark, len+4);
		ct=BTIC1_EmitFourcc(ct, fcc);
		memset(ct, 0, len);
		ct+=len;
	}else
	{
		tbuf=btic_malloc(2*len);
		tbuf1=btic_malloc(2*len);
		len2=PDZ2_EncodeStream64LvlZl(buf, tbuf1, len, 2*len, 9);
		len1=BTIC1_EscapeEncodeBufferSimple(tbuf1, len2, tbuf, 2*len);
		ct=BTIC1_EmitMarkerLen(ct, mark, len1+4);
		ct=BTIC1_EmitFourcc(ct, fcc);
		memcpy(ct, tbuf, len1);
		ct+=len1;
		btic_free(tbuf); btic_free(tbuf1);
	}

	return(ct);
}

byte *BTIC1_EmitLumpFMT13Z(byte *ct, char *tag, byte *buf, int len)
{
	return(BTIC1_EmitLumpMarkStrZ(ct,
		BGBBTJ_BTIC1_MARK_FMT13, tag, buf, len));
}

byte *BTIC1_EmitLumpAPP13Z(byte *ct, char *tag, byte *buf, int len)
{
	return(BTIC1_EmitLumpMarkStrZ(ct,
		BGBBTJ_BTIC1_MARK_APP13, tag, buf, len));
}

byte *BTIC1_EmitLumpMarkStrZ(byte *ct,
	int mark, char *tag, byte *buf, int len)
{
	byte *tbuf, *tbuf1;
	byte tb[4];
	int len1, len2, lent;

	if(!buf || !len)
	{
		lent=strlen(tag)+1;
		ct=BTIC1_EmitMarkerLen(ct, mark, len+lent);
		ct=BTIC1_EmitString(ct, tag);
		memset(ct, 0, len);
		ct+=len;
	}else
	{
		tbuf=btic_malloc(2*len);
		tbuf1=btic_malloc(2*len);
		len2=PDZ2_EncodeStream64LvlZl(buf, tbuf1, len, 2*len, 9);
		len1=BTIC1_EscapeEncodeBufferSimple(tbuf1, len2, tbuf, 2*len);
		lent=strlen(tag)+1;
		ct=BTIC1_EmitMarkerLen(ct, mark, len1+lent);
		ct=BTIC1_EmitString(ct, tag);
		memcpy(ct, tbuf, len1);
		ct+=len1;
		btic_free(tbuf); btic_free(tbuf1);
	}
	
	return(ct);
}
#endif

byte *BTIC1_EmitLumpFMT11(byte *ct, char *tag, char **args)
{
	byte *buf, *ct1;
	byte tb[4];
	int len;
	int i;

	buf=btic_malloc(65536);
	ct1=buf;
	ct1=BTIC1_EmitString(ct1, tag);
	if(args)
	{
		for(i=0; args[i]; i++)
			{ ct1=BTIC1_EmitString(ct1, args[i]); }
	}
	*ct1++=0x00;
	len=ct1-buf;

	ct=BTIC1_EmitMarkerLen(ct, BGBBTJ_BTIC1_MARK_FMT11, len);
	memcpy(ct, buf, len);
	ct+=len;
	
	btic_free(buf);
	
	return(ct);
}

byte *BTIC1_EmitLumpAPP11(byte *ct, char *tag, char **args)
{
	byte *buf, *ct1;
	byte tb[4];
	int len;
	int i;

	buf=btic_malloc(65536);
	ct1=buf;
	ct1=BTIC1_EmitString(ct1, tag);
	if(args)
	{
		for(i=0; args[i]; i++)
			{ ct1=BTIC1_EmitString(ct1, args[i]); }
	}
	*ct1++=0x00;
	len=ct1-buf;

	ct=BTIC1_EmitMarkerLen(ct, BGBBTJ_BTIC1_MARK_APP11, len);
	memcpy(ct, buf, len);
	ct+=len;
	
	btic_free(buf);
	
	return(ct);
}

btj_word_ube BTIC1_WrapWordUBE(u16 val)
{
	btj_word_ube tmp;
	tmp.v[0]=(val>>8)&0xFF;
	tmp.v[1]=val&0xFF;
	return(tmp);
}

btj_word_sbe BTIC1_WrapWordSBE(s16 val)
{
	btj_word_sbe tmp;
	tmp.v[0]=(val>>8)&0xFF;
	tmp.v[1]=val&0xFF;
	return(tmp);
}

btj_dword_ube BTIC1_WrapDWordUBE(u32 val)
{
	btj_dword_ube tmp;
	tmp.v[0]=(val>>24)&0xFF;
	tmp.v[1]=(val>>16)&0xFF;
	tmp.v[2]=(val>>8)&0xFF;
	tmp.v[3]=val&0xFF;
	return(tmp);
}

btj_dword_sbe BTIC1_WrapDWordSBE(s32 val)
{
	btj_dword_sbe tmp;
	tmp.v[0]=(val>>24)&0xFF;
	tmp.v[1]=(val>>16)&0xFF;
	tmp.v[2]=(val>>8)&0xFF;
	tmp.v[3]=val&0xFF;
	return(tmp);
}

byte *BTIC1_EmitLayerHeader(
	BGBBTJ_BTIC1_Context *ctx, byte *ct, BGBBTJ_BTIC1_LayerHeader *lhdr)
{
	struct BGBBTJ_BTIC1_TagLayerHeader_Ds lh;
	
	memset(&lh, 0, sizeof(struct BGBBTJ_BTIC1_TagLayerHeader_Ds));
	if(lhdr->name)
		strcpy(lh.name, lhdr->name);
	lh.layerid=BTIC1_WrapDWordSBE(lhdr->layerid);
	lh.xorg=BTIC1_WrapDWordSBE(lhdr->xorg);
	lh.yorg=BTIC1_WrapDWordSBE(lhdr->yorg);
	lh.xsize=BTIC1_WrapDWordSBE(lhdr->xsize);
	lh.ysize=BTIC1_WrapDWordSBE(lhdr->ysize);
	lh.xcenter=BTIC1_WrapDWordSBE(lhdr->xcenter);
	lh.ycenter=BTIC1_WrapDWordSBE(lhdr->ycenter);
	lh.flags=BTIC1_WrapDWordUBE(lhdr->flags);
	
	ct=BTIC1_EmitLumpFMT13(ct, "TagLayer", (byte *)(&lh),
		sizeof(struct BGBBTJ_BTIC1_TagLayerHeader_Ds));

//	ct=BTIC1_EmitLumpFMT12(ct, FCC_LHDR, (byte *)(&lh),
//		sizeof(struct BGBBTJ_BTIC1_TagLayerHeader_Ds));
	return(ct);
}

byte *BTIC1_EmitComponentLayerID(
	BGBBTJ_BTIC1_Context *ctx, byte *ct,
	BGBBTJ_BTIC1_ImageHeader *thdr)
{
	struct BGBBTJ_BTIC1_CompLayerID_Ds lh;
	
	memset(&lh, 0, sizeof(struct BGBBTJ_BTIC1_CompLayerID_Ds));
	lh.layerid=BTIC1_WrapDWordSBE(thdr->layerid);
	lh.flags=BTIC1_WrapDWordUBE(thdr->flags);
	
	ct=BTIC1_EmitLumpFMT12(ct, BTIC1_FCC_CLID, (byte *)(&lh),
		sizeof(struct BGBBTJ_BTIC1_CompLayerID_Ds));
	return(ct);
}

byte *BTIC1_EmitComponentLayerHeader(
	BGBBTJ_BTIC1_Context *ctx, byte *ct,
	BGBBTJ_BTIC1_ImageHeader *thdr)
{
	struct BGBBTJ_BTIC1_CompLayerHeader_Ds lh;
	
	memset(&lh, 0, sizeof(struct BGBBTJ_BTIC1_CompLayerHeader_Ds));
	strcpy(lh.name, thdr->name);
	lh.layerid=BTIC1_WrapDWordSBE(thdr->layerid);
	lh.flags=BTIC1_WrapDWordUBE(thdr->flags);
	
	ct=BTIC1_EmitLumpFMT13(ct, "CompLayer", (byte *)(&lh),
		sizeof(struct BGBBTJ_BTIC1_CompLayerHeader_Ds));
	return(ct);
}

byte *BTIC1_EmitImageHeader(
	BGBBTJ_BTIC1_Context *ctx, byte *ct,
	BGBBTJ_BTIC1_ImageHeader *thdr)
{
	struct BGBBTJ_BTIC1_ImageHeader_Ds lh;
	
	memset(&lh, 0, sizeof(struct BGBBTJ_BTIC1_ImageHeader_Ds));
	lh.width=BTIC1_WrapDWordSBE(thdr->width);
	lh.height=BTIC1_WrapDWordSBE(thdr->height);
	lh.imgtype=BTIC1_WrapWordUBE(thdr->imgtype);
	lh.mip_start=thdr->mip_start;
	lh.mip_end=thdr->mip_end;
	lh.filtmode=thdr->filtmode;
	lh.clrtype=thdr->clrtype;
	lh.pixtype=thdr->pixtype;
	
	ct=BTIC1_EmitLumpFMT12(ct, BTIC1_FCC_THDR, (byte *)(&lh),
		sizeof(struct BGBBTJ_BTIC1_ImageHeader_Ds));
	return(ct);
}

byte *BTIC1_EmitImageData(
	BGBBTJ_BTIC1_Context *ctx, byte *ct,
	BGBBTJ_BTIC1_ImageHeader *thdr)
{
	if(!thdr->fpdata)
		return(ct);

	if(thdr->sz_fpdata>=256)
	{
		ct=BTIC1_EmitLumpFMT12Z(ct, BTIC1_FCC_TDAZ,
			thdr->fpdata, thdr->sz_fpdata);
	}else
	{
		ct=BTIC1_EmitLumpFMT12(ct, BTIC1_FCC_TDAT,
			thdr->fpdata, thdr->sz_fpdata);
	}

	return(ct);
}

byte *BTIC1_EmitImageLiteralData(
	BGBBTJ_BTIC1_Context *ctx, byte *ct,
	BGBBTJ_BTIC1_ImageHeader *thdr)
{
	if(!thdr->fpldata)
		return(ct);
	if(thdr->sz_fpldata<=0)
		return(ct);

	if(thdr->sz_fpldata>=256)
	{
//		ct=BTIC1_EmitLumpFMT12(ct, BTIC1_FCC_TLDA,
//			thdr->fpldata, thdr->sz_fpldata);
		ct=BTIC1_EmitLumpFMT12Z(ct, BTIC1_FCC_TLDZ,
			thdr->fpldata, thdr->sz_fpldata);
	}else
	{
		ct=BTIC1_EmitLumpFMT12(ct, BTIC1_FCC_TLDA,
			thdr->fpldata, thdr->sz_fpldata);
	}

	return(ct);
}

byte *BTIC1_EmitImage(
	BGBBTJ_BTIC1_Context *ctx, byte *ct,
	BGBBTJ_BTIC1_ImageHeader *thdr)
{
	ct=BTIC1_EmitImageHeader(ctx, ct, thdr);
	ct=BTIC1_EmitImageLiteralData(ctx, ct, thdr);
	ct=BTIC1_EmitImageData(ctx, ct, thdr);
	ct=BTIC1_EmitLumpFMT12(ct, BTIC1_FCC_TEND, NULL, 0);
	return(ct);
}

void BTIC1_FilterPackImage(
	BGBBTJ_BTIC1_Context *ctx,
	BGBBTJ_BTIC1_ImageHeader *thdr)
{
	byte *tbuf, *tlbuf;
	int sz, szl, n;

	switch(thdr->imgtype)
	{
	case BGBBTJ_JPG_BC1:
	case BGBBTJ_JPG_BC1F:
	case BGBBTJ_JPG_BC1A:
		switch(thdr->filtmode)
		{
		case BGBBTJ_BTIC1_FILT_NONE:
			BTIC1_CheckBufferImageFPData(ctx, thdr, thdr->sz_data);
//			thdr->fpdata=btic_malloc(thdr->sz_data);
			memcpy(thdr->fpdata, thdr->data, thdr->sz_data);
//			thdr->sz_fpdata=thdr->sz_data;
			break;
		case BGBBTJ_BTIC1_FILT_BLOCKPACK:
			n=thdr->sz_data/8;
			if((thdr->flags&BGBBTJ_BTIC1_LFL_IFRAME) ||
				(thdr->flags&BGBBTJ_BTIC1_LFL_PFRAME))
					{ n=n/2; }

			tbuf=btic_malloc(2*thdr->sz_data);
			if(thdr->flags&BGBBTJ_BTIC1_LFL_PFRAME)
			{
				sz=BGBBTJ_PackBCn_EncodePatchBlocksDXT1(
					tbuf, thdr->data, n);
			}else
			{
				sz=BGBBTJ_PackBCn_EncodeBlocksDXT1(
					tbuf, thdr->data, n);
			}
			BTIC1_CheckBufferImageFPData(ctx, thdr, sz);
//			thdr->fpdata=btic_malloc(sz);
			memcpy(thdr->fpdata, tbuf, sz);
//			thdr->sz_fpdata=sz;
			btic_free(tbuf);
			break;
		case BGBBTJ_BTIC1_FILT_BLOCKPACK2:
			tbuf=btic_malloc(2*thdr->sz_data);
			tlbuf=btic_malloc(2*thdr->sz_data);

			n=thdr->sz_data/8;
			if((thdr->flags&BGBBTJ_BTIC1_LFL_IFRAME) ||
				(thdr->flags&BGBBTJ_BTIC1_LFL_PFRAME))
					{ n=n/2; }

			if(thdr->flags&BGBBTJ_BTIC1_LFL_PFRAME)
			{
				sz=BGBBTJ_PackBCn_EncodePatchBlocks2DXT1(
					tbuf, tlbuf, thdr->data,
					n, ((thdr->width+3)/4), &szl);
			}else
			{
				sz=BGBBTJ_PackBCn_EncodeBlocks2DXT1(
					tbuf, tlbuf, thdr->data,
					n, ((thdr->width+3)/4), &szl);
			}
			BTIC1_CheckBufferImageFPData(ctx, thdr, sz);
			BTIC1_CheckBufferImageFPLData(ctx, thdr, szl);
			memcpy(thdr->fpdata, tbuf, sz);
			memcpy(thdr->fpldata, tlbuf, szl);
			btic_free(tbuf);
			btic_free(tlbuf);
			break;
		default:
			break;
		}
		break;
	case BGBBTJ_JPG_BC3:
	case BGBBTJ_JPG_BC3F:
	case BGBBTJ_JPG_BC3_UVAY:
		switch(thdr->filtmode)
		{
		case BGBBTJ_BTIC1_FILT_NONE:
			BTIC1_CheckBufferImageFPData(ctx, thdr, thdr->sz_data);
//			thdr->fpdata=btic_malloc(thdr->sz_data);
			memcpy(thdr->fpdata, thdr->data, thdr->sz_data);
//			thdr->sz_fpdata=thdr->sz_data;
			break;
		case BGBBTJ_BTIC1_FILT_BLOCKPACK:
			tbuf=btic_malloc(2*thdr->sz_data);

			n=thdr->sz_data/16;
			if((thdr->flags&BGBBTJ_BTIC1_LFL_IFRAME) ||
				(thdr->flags&BGBBTJ_BTIC1_LFL_PFRAME))
					{ n=n/2; }

			if(thdr->flags&BGBBTJ_BTIC1_LFL_PFRAME)
			{
				sz=BGBBTJ_PackBCn_EncodePatchBlocksDXT5(
					tbuf, thdr->data, n);
			}else
			{
				sz=BGBBTJ_PackBCn_EncodeBlocksDXT5(
					tbuf, thdr->data, n);
			}
			BTIC1_CheckBufferImageFPData(ctx, thdr, sz);
//			thdr->fpdata=btic_malloc(sz);
			memcpy(thdr->fpdata, tbuf, sz);
//			thdr->sz_fpdata=sz;
			btic_free(tbuf);
			break;
		case BGBBTJ_BTIC1_FILT_BLOCKPACK2:
			tbuf=btic_malloc(2*thdr->sz_data);
			tlbuf=btic_malloc(2*thdr->sz_data);

			n=thdr->sz_data/16;
			if((thdr->flags&BGBBTJ_BTIC1_LFL_IFRAME) ||
				(thdr->flags&BGBBTJ_BTIC1_LFL_PFRAME))
					{ n=n/2; }

			if(thdr->flags&BGBBTJ_BTIC1_LFL_PFRAME)
			{
				sz=BGBBTJ_PackBCn_EncodePatchBlocks2DXT5(
					tbuf, tlbuf, thdr->data,
					n, ((thdr->width+3)/4), &szl);
			}else
			{
				sz=BGBBTJ_PackBCn_EncodeBlocks2DXT5(
					tbuf, tlbuf, thdr->data,
					n, ((thdr->width+3)/4), &szl);
			}
			BTIC1_CheckBufferImageFPData(ctx, thdr, sz);
			BTIC1_CheckBufferImageFPLData(ctx, thdr, szl);
			memcpy(thdr->fpdata, tbuf, sz);
			memcpy(thdr->fpldata, tlbuf, szl);
			btic_free(tbuf);
			btic_free(tlbuf);
			break;
		default:
			break;
		}
		break;
	default:
		break;
	}
}

void BTIC1_CheckBufferImageFPData(
	BGBBTJ_BTIC1_Context *ctx,
	BGBBTJ_BTIC1_ImageHeader *thdr,
	int sz)
{
	byte *tbuf;

	if(!thdr->fpdata || (sz!=thdr->sz_fpdata))
	{
		if(thdr->fpdata)
			btic_free(thdr->fpdata);
		tbuf=btic_malloc(sz);
		thdr->fpdata=tbuf;
		thdr->sz_fpdata=sz;
	}
}

void BTIC1_CheckBufferImageFPLData(
	BGBBTJ_BTIC1_Context *ctx,
	BGBBTJ_BTIC1_ImageHeader *thdr,
	int sz)
{
	byte *tbuf;

	if(!thdr->fpldata || (sz!=thdr->sz_fpldata))
	{
		if(thdr->fpldata)
			btic_free(thdr->fpldata);
		tbuf=btic_malloc(sz);
		thdr->fpldata=tbuf;
		thdr->sz_fpldata=sz;
	}
}

void BTIC1_CheckBufferImageData(
	BGBBTJ_BTIC1_Context *ctx,
	BGBBTJ_BTIC1_ImageHeader *thdr,
	int sz)
{
	byte *tbuf;

	if(!thdr->data || (sz!=thdr->sz_data))
	{
		if(thdr->data)
			btic_free(thdr->data);
		tbuf=btic_malloc(sz);
		thdr->data=tbuf;
		thdr->sz_data=sz;
	}

	if((thdr->flags&BGBBTJ_BTIC1_LFL_IFRAME) ||
		(thdr->flags&BGBBTJ_BTIC1_LFL_PFRAME))
	{
		thdr->ofs_pframe=sz/2;
	}
}

void BTIC1_CheckBufferImageRGBA(
	BGBBTJ_BTIC1_Context *ctx,
	BGBBTJ_BTIC1_ImageHeader *thdr,
	int sz)
{
	byte *tbuf;

	if(!thdr->rgba || (sz!=thdr->sz_rgba))
	{
		if(thdr->rgba)
			btic_free(thdr->rgba);
		tbuf=btic_malloc(sz);
		thdr->rgba=tbuf;
		thdr->sz_rgba=sz;
	}
}

void BTIC1_ColorConvertImage(
	BGBBTJ_BTIC1_Context *ctx,
	BGBBTJ_BTIC1_ImageHeader *thdr)
{
	byte *tbuf;
	int sz;

	if(!thdr->rgba)
		return;

	switch(thdr->imgtype)
	{
	case BGBBTJ_JPG_BC1:
	case BGBBTJ_JPG_BC1F:
	case BGBBTJ_JPG_BC1A:
		sz=((thdr->width+3)/4)*((thdr->height+3)/4)*8;
		if((thdr->flags&BGBBTJ_BTIC1_LFL_IFRAME) ||
			(thdr->flags&BGBBTJ_BTIC1_LFL_PFRAME))
				{ thdr->ofs_pframe=sz; sz=2*sz; }
		BTIC1_CheckBufferImageData(ctx, thdr, sz);
		if(thdr->flags&BGBBTJ_BTIC1_LFL_PFRAME)
		{
			BGBBTJ_BCn_EncodeImageDXTn(
				thdr->data+thdr->ofs_pframe, thdr->rgba,
				thdr->width, thdr->height,
				4, thdr->imgtype);
		}else
		{
			BGBBTJ_BCn_EncodeImageDXTn(thdr->data, thdr->rgba,
				thdr->width, thdr->height,
				4, thdr->imgtype);
		}
		break;
	case BGBBTJ_JPG_BC3:
	case BGBBTJ_JPG_BC3F:
	case BGBBTJ_JPG_BC3_UVAY:
		sz=((thdr->width+3)/4)*((thdr->height+3)/4)*16;
		if((thdr->flags&BGBBTJ_BTIC1_LFL_IFRAME) ||
			(thdr->flags&BGBBTJ_BTIC1_LFL_PFRAME))
				{ thdr->ofs_pframe=sz; sz=2*sz; }
		BTIC1_CheckBufferImageData(ctx, thdr, sz);
		if(thdr->flags&BGBBTJ_BTIC1_LFL_PFRAME)
		{
			BGBBTJ_BCn_EncodeImageDXTn(
				thdr->data+thdr->ofs_pframe, thdr->rgba,
				thdr->width, thdr->height,
				4, thdr->imgtype);
		}else
		{
			BGBBTJ_BCn_EncodeImageDXTn(thdr->data, thdr->rgba,
				thdr->width, thdr->height,
				4, thdr->imgtype);
		}
		break;
	default:
		break;
	}
}

byte *BTIC1_EncodeImageBuffer(
	BGBBTJ_BTIC1_Context *ctx, byte *ct,
	BGBBTJ_BTIC1_ImageHeader *thdr)
{
	BTIC1_ColorConvertImage(ctx, thdr);
	BTIC1_FilterPackImage(ctx, thdr);
	ct=BTIC1_EmitImage(ctx, ct, thdr);
	return(ct);
}

byte *BTIC1_EncodeCompLayerBuffer(
	BGBBTJ_BTIC1_Context *ctx, byte *ct,
	BGBBTJ_BTIC1_ImageHeader *thdr)
{
	int i;

	if(!thdr->layerid)
	{
		i=ctx->n_image++;
		thdr->layerid=i+1;
		ctx->image[i]=thdr;
	}

	ct=BTIC1_EmitComponentLayerHeader(ctx, ct, thdr);
	ct=BTIC1_EncodeImageBuffer(ctx, ct, thdr);
	return(ct);
}

byte *BTIC1_EncodeTagLayerBuffer(
	BGBBTJ_BTIC1_Context *ctx, byte *ct,
	BGBBTJ_BTIC1_LayerHeader *lhdr)
{
	int i;

	ct=BTIC1_EmitLayerHeader(ctx, ct, lhdr);
//	ct=BTIC1_EncodeImageBuffer(ctx, ct, thdr);

	for(i=0; i<lhdr->n_image; i++)
	{
		ct=BTIC1_EncodeCompLayerBuffer(ctx, ct, lhdr->image[i]);
	}

	ct=BTIC1_EmitLumpFMT12(ct, BTIC1_FCC_LEND, NULL, 0);

	return(ct);
}

byte *BTIC1_EncodePatchCompLayerBuffer(
	BGBBTJ_BTIC1_Context *ctx, byte *ct,
	BGBBTJ_BTIC1_ImageHeader *thdr)
{
	int i;

	if(!thdr->layerid)
	{
		i=ctx->n_image++;
		thdr->layerid=i+1;
		ctx->image[i]=thdr;
	}

	ct=BTIC1_EmitComponentLayerID(ctx, ct, thdr);
	ct=BTIC1_EncodeImageBuffer(ctx, ct, thdr);
	return(ct);
}

int BTIC1_EncodeContext(
	BGBBTJ_BTIC1_Context *ctx, byte *obuf, int maxsz)
{
	byte *tbuf, *tbuf1, *ct;
	int i, j, k, l;
	
	tbuf=btic_malloc(1<<24);
	ct=tbuf;
	
	if(ctx->qflags&BGBBTJ_QFL_PFRAME)
	{
		for(i=0; i<ctx->n_image; i++)
		{
			ct=BTIC1_EncodePatchCompLayerBuffer(ctx, ct, ctx->image[i]);
		}
	}else
	{
		for(i=0; i<ctx->n_layer; i++)
		{
			ctx->layer[i]->layerid=i+1;
			ct=BTIC1_EncodeTagLayerBuffer(ctx, ct, ctx->layer[i]);
		}
	}

	if(0)
	{
		i=ct-tbuf;

		tbuf1=btic_malloc(1<<24);
		j=BGBBTJ_BitArith_EncodeData(tbuf, i, tbuf1, 1<<24);

		ct=BTIC1_EmitLumpFMT13(obuf, "BTIC1A", tbuf1, j);
		btic_free(tbuf); btic_free(tbuf1);
		return(ct-obuf);
	}

	if(0)
	{
		i=ct-tbuf;

		tbuf1=btic_malloc(1<<24);
//		j=BGBBTJ_BitArith_EncodeData(tbuf, i, tbuf1, 1<<24);
//		j=PDZ2_EncodeStreamLvl(tbuf, tbuf1+2, i, 1<<24, 0);
//		j=PDZ2_EncodeStreamLvl(tbuf, tbuf1+2, i, 1<<24, 9);
//		j=PDZ2_EncodeStream64Lvl(tbuf, tbuf1+2, i, 1<<24, 9);
		j=PDZ2_EncodeStream64LvlZl(tbuf, tbuf1, i, 1<<24, 9);

#if 0
		k=(7<<12)|(8<<8)|(3<<6); l=k%31;
		if(l)k|=31-l;
		tbuf1[0]=k>>8; tbuf1[1]=k&0xFF;

		k=BGBBTJ_BufPNG_DataAdler32(tbuf, i, 1);
		tbuf1[2+j+0]=(k>>24)&0xFF;
		tbuf1[2+j+1]=(k>>16)&0xFF;
		tbuf1[2+j+2]=(k>>8)&0xFF;
		tbuf1[2+j+3]=k&0xFF;

		j+=6;
#endif

		ct=BTIC1_EmitLumpFMT13(obuf, "BTIC1Z", tbuf1, j);
		btic_free(tbuf); btic_free(tbuf1);
		return(ct-obuf);
	}

	if(0)
	{
		i=ct-tbuf;

		tbuf1=btic_malloc(1<<24);
//		j=BGBBTJ_BitArith_EncodeData(tbuf, i, tbuf1, 1<<24);
//		j=PDZ2_EncodeStreamLvl(tbuf, tbuf1+2, i, 1<<24, 0);
//		j=PDZ2_EncodeStreamLvl(tbuf, tbuf1, i, 1<<24, 9);
		j=PDZ2_EncodeStream64Lvl(tbuf, tbuf1, i, 1<<24, 9);
		memcpy(tbuf, tbuf1, j); i=j;
		j=BGBBTJ_BitArith_EncodeData(tbuf, i, tbuf1, 1<<24);

		ct=BTIC1_EmitLumpFMT13(obuf, "BTIC1AZ", tbuf1, j);
		btic_free(tbuf); btic_free(tbuf1);
		return(ct-obuf);
	}

	i=ct-tbuf;
	ct=BTIC1_EmitLumpFMT13(obuf, "BTIC1", tbuf, i);
	btic_free(tbuf);

	return(ct-obuf);
}

BGBBTJ_BTIC1_LayerHeader *BTIC1_AllocLayer(
	BGBBTJ_BTIC1_Context *ctx, char *name)
{
	BGBBTJ_BTIC1_LayerHeader *tmp;

	tmp=btic_malloc(sizeof(BGBBTJ_BTIC1_LayerHeader));
	memset(tmp, 0, sizeof(BGBBTJ_BTIC1_LayerHeader));
	if(name)tmp->name=dyllStrdup(name);
	tmp->ctx=ctx;
	
	return(tmp);
}

BGBBTJ_BTIC1_ImageHeader *BTIC1_AllocImage(
	BGBBTJ_BTIC1_Context *ctx, char *name)
{
	BGBBTJ_BTIC1_ImageHeader *tmp;

	tmp=btic_malloc(sizeof(BGBBTJ_BTIC1_ImageHeader));
	memset(tmp, 0, sizeof(BGBBTJ_BTIC1_ImageHeader));
	if(name)tmp->name=dyllStrdup(name);
	tmp->ctx=ctx;
	
	return(tmp);
}

BGBBTJ_BTIC1_LayerHeader *BTIC1_LookupContextLayer(
	BGBBTJ_BTIC1_Context *ctx, char *name)
{
	int i;

	if(!name)
		{ return(ctx->layer[0]); }
	for(i=1; i<ctx->n_layer; i++)
	{
		if(!strcmp(ctx->layer[i]->name, name))
			return(ctx->layer[i]);
	}
	return(NULL);
}

BGBBTJ_BTIC1_LayerHeader *BTIC1_GetContextLayer(
	BGBBTJ_BTIC1_Context *ctx, char *name)
{
	BGBBTJ_BTIC1_LayerHeader *tmp;
	int i;

	if(!name)
	{
		if(ctx->layer[0])
			return(ctx->layer[0]);
		tmp=BTIC1_AllocLayer(ctx, NULL);
		ctx->layer[0]=tmp;
		if(ctx->n_layer<1)
			ctx->n_layer=1;
		return(tmp);
	}

	tmp=BTIC1_LookupContextLayer(ctx, name);
	if(tmp)return(tmp);

	if(ctx->n_layer<1)
	{
		if(!ctx->layer[0])
		{
			tmp=BTIC1_AllocLayer(ctx, NULL);
			ctx->layer[0]=tmp;
		}
		ctx->n_layer=1;
	}

	i=ctx->n_layer++;

	tmp=BTIC1_AllocLayer(ctx, name);
	ctx->layer[i]=tmp;
	return(tmp);
}

BGBBTJ_BTIC1_ImageHeader *BTIC1_LookupLayerImage(
	BGBBTJ_BTIC1_Context *ctx, BGBBTJ_BTIC1_LayerHeader *layer,
	char *name)
{
	int i;
	
	for(i=0; i<layer->n_image; i++)
	{
		if(!strcmp(layer->image[i]->name, name))
			return(layer->image[i]);
	}
	
	return(NULL);
}

BGBBTJ_BTIC1_ImageHeader *BTIC1_GetLayerImage(
	BGBBTJ_BTIC1_Context *ctx, BGBBTJ_BTIC1_LayerHeader *layer,
	char *name)
{
	BGBBTJ_BTIC1_ImageHeader *tmp;
	int i;

	tmp=BTIC1_LookupLayerImage(ctx, layer, name);
	if(tmp)return(tmp);
	
	i=layer->n_image++;
	tmp=BTIC1_AllocImage(ctx, name);
	tmp->layer=layer;
	layer->image[i]=tmp;
	return(tmp);
}

void BTIC1_SetupImageBuffer(
	BGBBTJ_BTIC1_Context *ctx, BGBBTJ_BTIC1_ImageHeader *img,
	byte *rgba, int xs, int ys, int qf, int ity, int pf)
{
	byte *rsbuf, *ct;
	int sz, xs1, ys1;
	int i, nl, fl;

	rsbuf=btic_malloc(xs*ys*4);
//	memcpy(rsbuf, rgba, xs*ys*4);
	if(qf&BGBBTJ_QFL_PFRAME)
	{
		BGBBTJ_FiltLBR_FilterImage(NULL, img->rgba, xs, ys, qf);
		BGBBTJ_FiltLBR_FilterPatchImage(rsbuf, rgba, xs, ys, qf);
	}else
	{
		BGBBTJ_FiltLBR_FilterImage(rsbuf, rgba, xs, ys, qf);
	}

	fl=0;
	if(qf&BGBBTJ_QFL_IFRAME)fl|=BGBBTJ_BTIC1_LFL_IFRAME;
	if(qf&BGBBTJ_QFL_PFRAME)fl|=BGBBTJ_BTIC1_LFL_PFRAME;

	xs1=xs; ys1=ys; sz=0;
	for(nl=0; (xs1>1) || (ys1>1); nl++)
	{
		sz+=xs1*ys1*4;
		xs1=(xs1+1)>>1;
		ys1=(ys1+1)>>1;
	}
	
	nl=0;
	
	img->width=xs;
	img->height=ys;
	img->flags=fl;
	img->imgtype=ity;
//	img->filtmode=BGBBTJ_BTIC1_FILT_BLOCKPACK;
	img->filtmode=BGBBTJ_BTIC1_FILT_BLOCKPACK2;
	img->mip_start=0;
	img->mip_end=nl;
	
//	sz=xs*ys*4;
	BTIC1_CheckBufferImageRGBA(ctx, img, sz);
//	memcpy(img->rgba, rgba, sz);

	xs1=xs; ys1=ys; ct=img->rgba;
	for(i=0; (xs1>1) || (ys1>1); i++)
	{
		memcpy(ct, rsbuf, xs1*ys1*4);
		ct+=xs1*ys1*4;
		BGBBTJ_Tex_HalfSample(rsbuf, xs1, ys1);
		xs1=(xs1+1)>>1;
		ys1=(ys1+1)>>1;
	}
	
	btic_free(rsbuf);
}

BGBBTJ_API int BTIC1_SetupComponentCtx(
	BGBBTJ_BTIC1_Context *ctx, char *lname,
	byte *rgba, byte *norm, byte *spec, byte *luma,
	int xs, int ys, int qf, int ity, int pf)
{
	BGBBTJ_BTIC1_LayerHeader *lhdr;
	BGBBTJ_BTIC1_ImageHeader *ihdr;
	int i;

	lhdr=BTIC1_GetContextLayer(ctx, lname);
	lhdr->xsize=xs;
	lhdr->ysize=ys;
	
	if(rgba)
	{
		ihdr=BTIC1_GetLayerImage(ctx, lhdr, "RGBA");
		BTIC1_SetupImageBuffer(ctx, ihdr, rgba, xs, ys, qf, ity, pf);
	}

	if(norm)
	{
		ihdr=BTIC1_GetLayerImage(ctx, lhdr, "XYZD");
		BTIC1_SetupImageBuffer(ctx, ihdr, norm, xs, ys, qf, ity, pf);
	}

	if(spec)
	{
		ihdr=BTIC1_GetLayerImage(ctx, lhdr, "SpRGBS");
		BTIC1_SetupImageBuffer(ctx, ihdr, spec, xs, ys, qf, ity, pf);
	}

	if(luma)
	{
		ihdr=BTIC1_GetLayerImage(ctx, lhdr, "LuRGBS");
		BTIC1_SetupImageBuffer(ctx, ihdr, luma, xs, ys, qf, ity, pf);
	}
	
	return(0);
}

BGBBTJ_API int BTIC1_EncodeComponentCtx(
	BGBBTJ_BTIC1_Context *ctx,
	byte *rgba, byte *norm, byte *spec, byte *luma,
	byte *obuf, int xs, int ys, int qf)
{
	int i, pf;

	i=BGBBTJ_BCn_CheckEncodeImageAlpha(rgba, xs, ys, 4);
	if(norm)i|=BGBBTJ_BCn_CheckEncodeImageAlpha(norm, xs, ys, 4);
	if(spec)i|=BGBBTJ_BCn_CheckEncodeImageAlpha(norm, xs, ys, 4);
	if(luma)i|=BGBBTJ_BCn_CheckEncodeImageAlpha(norm, xs, ys, 4);

	if(qf&BGBBTJ_QFL_PFRAME)
	{
		pf=ctx->imgtype;
		ctx->qflags=qf;
	}else
	{
		pf=BGBBTJ_JPG_BC1A;
		if(i)pf=BGBBTJ_JPG_BC3;
		ctx->imgtype=pf;
		ctx->qflags=qf;
	}

	BTIC1_SetupComponentCtx(ctx, NULL,
		rgba, norm, spec, luma,
		xs, ys, qf, pf, BGBBTJ_JPG_RGBA);

#if 0
	if(i)
	{
		BTIC1_SetupComponentCtx(ctx, NULL,
			rgba, norm, spec, luma,
			xs, ys, qf, BGBBTJ_JPG_BC3, BGBBTJ_JPG_RGBA);
	}else
	{
		BTIC1_SetupComponentCtx(ctx, NULL,
			rgba, norm, spec, luma,
			xs, ys, qf, BGBBTJ_JPG_BC1A, BGBBTJ_JPG_RGBA);
	}
#endif

	i=BTIC1_EncodeContext(ctx, obuf, xs*ys*4);
	return(i);
}

BGBBTJ_API BGBBTJ_BTIC1_Context *BTIC1_AllocContext(void)
{
	BGBBTJ_BTIC1_Context *tmp;
	tmp=btic_malloc(sizeof(BGBBTJ_BTIC1_Context));
	memset(tmp, 0, sizeof(BGBBTJ_BTIC1_Context));	
	return(tmp);
}

BGBBTJ_API void BTIC1_FreeContext(BGBBTJ_BTIC1_Context *ctx)
{
	BGBBTJ_BTIC1_Malloc *cur, *nxt;
	int i;
	
	for(i=0; i<ctx->n_image; i++)
	{
		if(!ctx->image[i])
			continue;
	
		if(ctx->image[i]->data)
			{ btic_free(ctx->image[i]->data); }
		if(ctx->image[i]->fpdata)
			{ btic_free(ctx->image[i]->fpdata); }
		if(ctx->image[i]->fpldata)
			{ btic_free(ctx->image[i]->fpldata); }
		if(ctx->image[i]->rgba)
			{ btic_free(ctx->image[i]->rgba); }
		btic_free(ctx->image[i]);
	}

	for(i=0; i<ctx->n_layer; i++)
	{
		btic_free(ctx->layer[i]);
	}

	btic_free(ctx);
	
#if 1
	cur=btic_malloc_list;
	while(cur)
	{
		nxt=cur->next;
		printf("Leak: %s %d\n", cur->fn, cur->ln);
		free(cur);
		cur=nxt;
	}
	btic_malloc_list=NULL;
#endif
}

BGBBTJ_API int BTIC1_EncodeComponent(
	byte *rgba, byte *norm, byte *spec, byte *luma,
	byte *obuf, int xs, int ys, int qf)
{
	int i;

	BGBBTJ_BTIC1_Context *ctx;
	
	ctx=BTIC1_AllocContext();
	i=BTIC1_EncodeComponentCtx(ctx,
		rgba, norm, spec, luma,
		obuf, xs, ys, qf);
	BTIC1_FreeContext(ctx);
	return(i);
}
