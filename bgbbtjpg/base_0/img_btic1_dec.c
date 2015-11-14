#include <bgbbtj.h>

u16 BTIC1_UnwrapWordUBE(btj_word_ube val)
{
	int v;
	v=(val.v[0]<<8)|val.v[1];
	return((u16)v);
}

s16 BTIC1_UnwrapWordSBE(btj_word_sbe val)
{
	int v;
	v=(val.v[0]<<8)|val.v[1];
	return((s16)v);
}

u32 BTIC1_UnwrapDWordUBE(btj_dword_ube val)
{
	u32 v;
	v=(val.v[0]<<24)|(val.v[1]<<16)|(val.v[2]<<8)|val.v[3];
	return(v);
}

s32 BTIC1_UnwrapDWordSBE(btj_dword_sbe val)
{
	s32 v;
	v=(val.v[0]<<24)|(val.v[1]<<16)|(val.v[2]<<8)|val.v[3];
	return(v);
}

BGBBTJ_API int BTIC1_EscapeDecodeBuffer(byte *ibuf, int isz,
	byte *obuf, int osz)
{
	byte *cs, *ct, *cse, *cte;
	
	cs=ibuf; cse=ibuf+isz;
	ct=obuf; cte=obuf+osz;
	
//	while((cs<cse) && (ct<cte))
	while(cs<cse)
	{
		if(cs[0]==0xFF)
		{
			if(cs[1]==0x00)
				{ cs+=2; *ct++=0xFF; continue; }
			if((cs[1]>=0x01) && (cs[1]<=0x0F))
				{ *ct++=*cs++; *ct++=(*cs++)-1; continue; }
			if((cs[1]>=0x20) && (cs[1]<=0x3F))
				{ *ct++=*cs++; *ct++=(*cs++)^0xC0; continue; }
			if((cs[1]>=0x40) && (cs[1]<=0x7F))
				{ *ct++=*cs++; *ct++=(*cs++)-0x20; continue; }
			break;
		}
		*ct++=*cs++;
	}
	if((ct>=cte) && (cs<cse))
		return(-1);
	return(ct-obuf);
}

BGBBTJ_API int BTIC1_EscapeDecodeSingleBuffer(byte *buf, int sz)
{
	byte *cs, *ct, *cse, *cte;
	
	cs=buf; cse=buf+sz;
	ct=buf; cte=buf+sz;
	
//	while((cs<cse) && (ct<cte))
	while(cs<cse)
	{
		if(cs[0]==0xFF)
		{
			if(cs[1]==0x00)
				{ cs+=2; *ct++=0xFF; continue; }
			if((cs[1]>=0x01) && (cs[1]<=0x1F))
				{ *ct++=*cs++; *ct++=(*cs++)-1; continue; }
			if((cs[1]>=0x20) && (cs[1]<=0x3F))
				{ *ct++=*cs++; *ct++=(*cs++)^0xC0; continue; }
			if((cs[1]>=0x40) && (cs[1]<=0x7F))
				{ *ct++=*cs++; *ct++=(*cs++)-0x20; continue; }
			break;
		}
		*ct++=*cs++;
	}
	if((ct>=cte) && (cs<cse))
		return(-1);
	return(ct-buf);
}

int BTIC1_CalcUnpackImageSize(
	BGBBTJ_BTIC1_Context *ctx,
	BGBBTJ_BTIC1_ImageHeader *thdr)
{
	int xs, ys;
	int sz, n;
	int i;

	switch(thdr->imgtype)
	{
	case BGBBTJ_JPG_BC1:
	case BGBBTJ_JPG_BC1F:
	case BGBBTJ_JPG_BC1A:
		xs=thdr->width; ys=thdr->height; n=0;
		for(i=thdr->mip_start; i<=thdr->mip_end; i++)
		{
			n+=((xs+3)>>2)*((ys+3)>>2);
			xs>>=1; ys>>=1;
		}
		if((thdr->flags&BGBBTJ_BTIC1_LFL_IFRAME) ||
			(thdr->flags&BGBBTJ_BTIC1_LFL_PFRAME))
		{
			i=(thdr->width+3)/4;
			n=((n+(i-1))/i)*i;
			sz=2*n*8;
		}else
		{
			sz=n*8;
		}
		break;
	case BGBBTJ_JPG_BC3:
	case BGBBTJ_JPG_BC3F:
	case BGBBTJ_JPG_BC3_UVAY:
		xs=thdr->width; ys=thdr->height; n=0;
		for(i=thdr->mip_start; i<=thdr->mip_end; i++)
		{
			n+=((xs+3)>>2)*((ys+3)>>2);
			xs>>=1; ys>>=1;
		}
		if((thdr->flags&BGBBTJ_BTIC1_LFL_IFRAME) ||
			(thdr->flags&BGBBTJ_BTIC1_LFL_PFRAME))
		{
			i=(thdr->width+3)/4;
			n=((n+(i-1))/i)*i;
			sz=2*n*16;
		}else
		{
			sz=n*16;
		}
//		sz=n*16;
		break;
	default:
		sz=-1;
		break;
	}
	
	return(sz);
}

void BTIC1_FilterUnpackImage(
	BGBBTJ_BTIC1_Context *ctx,
	BGBBTJ_BTIC1_ImageHeader *thdr)
{
	byte *tbuf;
	int sz, n;

	switch(thdr->imgtype)
	{
	case BGBBTJ_JPG_BC1:
	case BGBBTJ_JPG_BC1F:
	case BGBBTJ_JPG_BC1A:
		switch(thdr->filtmode)
		{
		case BGBBTJ_BTIC1_FILT_NONE:
			sz=BTIC1_CalcUnpackImageSize(ctx, thdr);
			BTIC1_CheckBufferImageData(ctx, thdr, sz);
			memcpy(thdr->data, thdr->fpdata, thdr->sz_data);
			break;
		case BGBBTJ_BTIC1_FILT_BLOCKPACK:
			sz=BTIC1_CalcUnpackImageSize(ctx, thdr);
			n=sz/8;
			if(	(thdr->flags&BGBBTJ_BTIC1_LFL_IFRAME) ||
				(thdr->flags&BGBBTJ_BTIC1_LFL_PFRAME))
					{ n=n/2; }
			BTIC1_CheckBufferImageData(ctx, thdr, sz);
			if(thdr->flags&BGBBTJ_BTIC1_LFL_PFRAME)
				{ BGBBTJ_PackBCn_DecodeBlocksDXT1(
					thdr->fpdata, thdr->data+thdr->ofs_pframe, n); }
			else
				{ BGBBTJ_PackBCn_DecodeBlocksDXT1(
					thdr->fpdata, thdr->data, n); }
			break;
		case BGBBTJ_BTIC1_FILT_BLOCKPACK2:
			sz=BTIC1_CalcUnpackImageSize(ctx, thdr);
			n=sz/8;
			if(	(thdr->flags&BGBBTJ_BTIC1_LFL_IFRAME) ||
				(thdr->flags&BGBBTJ_BTIC1_LFL_PFRAME))
					{ n=n/2; }
			BTIC1_CheckBufferImageData(ctx, thdr, sz);
			if(thdr->flags&BGBBTJ_BTIC1_LFL_PFRAME)
			{
				BGBBTJ_PackBCn_DecodeBlocks2DXT1(
					thdr->fpdata, thdr->fpldata,
					thdr->data+thdr->ofs_pframe,
					n, ((thdr->width+3)/4));
			}else
			{
				BGBBTJ_PackBCn_DecodeBlocks2DXT1(
					thdr->fpdata, thdr->fpldata, thdr->data,
					n, ((thdr->width+3)/4));
			}
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
			sz=BTIC1_CalcUnpackImageSize(ctx, thdr);
			BTIC1_CheckBufferImageData(ctx, thdr, sz);
			memcpy(thdr->data, thdr->fpdata, thdr->sz_data);
			break;
		case BGBBTJ_BTIC1_FILT_BLOCKPACK:
			sz=BTIC1_CalcUnpackImageSize(ctx, thdr);
			n=sz/16;
			if(	(thdr->flags&BGBBTJ_BTIC1_LFL_IFRAME) ||
				(thdr->flags&BGBBTJ_BTIC1_LFL_PFRAME))
					{ n=n/2; }
			BTIC1_CheckBufferImageData(ctx, thdr, sz);
			if(thdr->flags&BGBBTJ_BTIC1_LFL_PFRAME)
			{
				BGBBTJ_PackBCn_DecodeBlocksDXT5(
					thdr->fpdata, thdr->data+thdr->ofs_pframe, n);
			}else
			{
				BGBBTJ_PackBCn_DecodeBlocksDXT5(
					thdr->fpdata, thdr->data, n);
			}
			break;
		case BGBBTJ_BTIC1_FILT_BLOCKPACK2:
			sz=BTIC1_CalcUnpackImageSize(ctx, thdr);
			n=sz/16;
			if(	(thdr->flags&BGBBTJ_BTIC1_LFL_IFRAME) ||
				(thdr->flags&BGBBTJ_BTIC1_LFL_PFRAME))
					{ n=n/2; }
			BTIC1_CheckBufferImageData(ctx, thdr, sz);
			if(thdr->flags&BGBBTJ_BTIC1_LFL_PFRAME)
			{
				BGBBTJ_PackBCn_DecodeBlocks2DXT5(
					thdr->fpdata, thdr->fpldata,
					thdr->data+thdr->ofs_pframe,
					n, ((thdr->width+3)/4));
			}else
			{
				BGBBTJ_PackBCn_DecodeBlocks2DXT5(
					thdr->fpdata, thdr->fpldata, thdr->data,
					n, ((thdr->width+3)/4));
			}
			break;
		default:
			break;
		}
		break;
	default:
		break;
	}
}

void BTIC1_DecodeImageHeader(
	BGBBTJ_BTIC1_Context *ctx, byte *cs, int len)
{
	byte lhbuf[256];
	struct BGBBTJ_BTIC1_ImageHeader_Ds lh;
	BGBBTJ_BTIC1_ImageHeader *thdr;
	int i;

	memset(lhbuf, 0, 256);
	BTIC1_EscapeDecodeBuffer(cs, len, lhbuf, 256);
	memcpy(&lh, lhbuf, sizeof(struct BGBBTJ_BTIC1_ImageHeader_Ds));

	thdr=ctx->cur_img;
	thdr->width=BTIC1_UnwrapDWordSBE(lh.width);
	thdr->height=BTIC1_UnwrapDWordSBE(lh.height);
	thdr->imgtype=BTIC1_UnwrapWordUBE(lh.imgtype);
	thdr->mip_start=lh.mip_start;
	thdr->mip_end=lh.mip_end;
	thdr->filtmode=lh.filtmode;
	thdr->clrtype=lh.clrtype;
	thdr->pixtype=lh.pixtype;
}

void BTIC1_DecodeImageData(
	BGBBTJ_BTIC1_Context *ctx, byte *cs, int len)
{
	BGBBTJ_BTIC1_ImageHeader *thdr;
	byte *tbuf;
	int sz;
	
	tbuf=btic_malloc(len);
	sz=BTIC1_EscapeDecodeBuffer(cs, len, tbuf, len);
	if(sz<0) { btic_free(tbuf); return; }
	
	thdr=ctx->cur_img;
	BTIC1_CheckBufferImageFPData(ctx, thdr, sz);
	memcpy(thdr->fpdata, tbuf, sz);
	btic_free(tbuf);
	
	BTIC1_FilterUnpackImage(ctx, thdr);
}

void BTIC1_DecodeImageLiteralData(
	BGBBTJ_BTIC1_Context *ctx, byte *cs, int len)
{
	BGBBTJ_BTIC1_ImageHeader *thdr;
	byte *tbuf;
	int sz;
	
	tbuf=btic_malloc(len);
	sz=BTIC1_EscapeDecodeBuffer(cs, len, tbuf, len);
	if(sz<0) { btic_free(tbuf); return; }
	
	thdr=ctx->cur_img;
	BTIC1_CheckBufferImageFPLData(ctx, thdr, sz);
	memcpy(thdr->fpldata, tbuf, sz);
	btic_free(tbuf);
	
//	BTIC1_FilterUnpackImage(ctx, thdr);
}

void BTIC1_DecodeImageDataZ(
	BGBBTJ_BTIC1_Context *ctx, byte *cs, int len)
{
	BGBBTJ_BTIC1_ImageHeader *thdr;
	byte *tbuf, *tbuf1;
	int sz, sz1;

#if 0
	tbuf=btic_malloc(len);
	sz=BTIC1_EscapeDecodeBuffer(cs, len, tbuf, len);
	if(sz<0) { btic_free(tbuf); return; }
	
	tbuf1=btic_malloc(1<<20);
//	sz1=PDUNZ_DecodeStream(tbuf+2, tbuf1, sz, 1<<20);
	PDUNZ_DecodeStreamSz(tbuf+2, tbuf1, sz, 1<<20, &sz1, 0);
#endif

#if 1
	tbuf1=btic_malloc(1<<20);
//	sz1=PDUNZ_DecodeStream(tbuf+2, tbuf1, sz, 1<<20);
	PDUNZ_DecodeStreamSz(cs+2, tbuf1, len, 1<<20, &sz1,
		BGBBTJ_ZFL_ESCAPE_FF);
#endif

	thdr=ctx->cur_img;
	BTIC1_CheckBufferImageFPData(ctx, thdr, sz1);
	memcpy(thdr->fpdata, tbuf1, sz1);
	
	BTIC1_FilterUnpackImage(ctx, thdr);

//	btic_free(tbuf);
	btic_free(tbuf1);
}

void BTIC1_DecodeImageLiteralDataZ(
	BGBBTJ_BTIC1_Context *ctx, byte *cs, int len)
{
	BGBBTJ_BTIC1_ImageHeader *thdr;
	byte *tbuf, *tbuf1;
	int sz, sz1;
	
#if 0
	tbuf=btic_malloc(len);
	sz=BTIC1_EscapeDecodeBuffer(cs, len, tbuf, len);
	if(sz<0) { btic_free(tbuf); return; }
	
	tbuf1=btic_malloc(1<<20);
//	sz1=PDUNZ_DecodeStream(tbuf+2, tbuf1, sz, 1<<20);
	PDUNZ_DecodeStreamSz(tbuf+2, tbuf1, sz, 1<<20, &sz1, 0);
#endif

#if 1
	tbuf1=btic_malloc(1<<20);
//	sz1=PDUNZ_DecodeStream(tbuf+2, tbuf1, sz, 1<<20);
	PDUNZ_DecodeStreamSz(cs+2, tbuf1, len, 1<<20, &sz1,
		BGBBTJ_ZFL_ESCAPE_FF);
#endif

	thdr=ctx->cur_img;
	BTIC1_CheckBufferImageFPLData(ctx, thdr, sz1);
	memcpy(thdr->fpldata, tbuf1, sz1);

//	btic_free(tbuf);
	btic_free(tbuf1);
	
//	BTIC1_FilterUnpackImage(ctx, thdr);
}

byte *BTIC1_SyncMarker(byte *cs, byte *cse)
{
	while(cs<cse)
	{
		if((*cs==0xFF) && cs[1]>0x10)
			break;
		cs++;
	}
	return(cs);
}

byte *BTIC1_DecodeMarkerGeneric(byte *cs, byte *cse, int *rlen)
{
	int l;
	l=(cs[2]<<8)|cs[3];
	switch(l)
	{
	case 0:
		l=(cs[4]<<24)|(cs[5]<<16)|(cs[6]<<8)|cs[7];
		cs+=8; l-=10; break;
	case 1:
		l=(cs[8]<<24)|(cs[9]<<16)|(cs[10]<<8)|cs[11];
		cs+=12; l-=14; break;
	default:
		cs+=4; l-=6; break;
	}
	
	*rlen=l;
	return(cs);
}

byte *BTIC1_DecodeMarkerFMT12(byte *cs, byte *cse, int *rlen, u32 *rfcc)
{
	u32 fcc;
	int l;
	l=(cs[2]<<8)|cs[3];
	switch(l)
	{
	case 0:
		l=(cs[4]<<24)|(cs[5]<<16)|(cs[6]<<8)|cs[7];
		cs+=8; l-=10; break;
	case 1:
		l=(cs[8]<<24)|(cs[9]<<16)|(cs[10]<<8)|cs[11];
		cs+=12; l-=14; break;
	default:
		cs+=4; l-=6; break;
	}
	
	*rlen=l;
	*rfcc=FOURCC(cs[0], cs[1], cs[2], cs[3]);
	return(cs+4);
}

byte *BTIC1_DecodeMarkerAPP12(byte *cs, byte *cse, int *rlen, u32 *rfcc)
	{ return(BTIC1_DecodeMarkerFMT12(cs, cse, rlen, rfcc)); }

byte *BTIC1_DecodeMarkerFMT13(byte *cs, byte *cse, int *rlen, char **rtag)
{
	char *s;
	int l, tl;
	l=(cs[2]<<8)|cs[3];
	switch(l)
	{
	case 0:
		l=(cs[4]<<24)|(cs[5]<<16)|(cs[6]<<8)|cs[7];
		cs+=8; l-=6; break;
	case 1:
		l=(cs[8]<<24)|(cs[9]<<16)|(cs[10]<<8)|cs[11];
		cs+=12; l-=10; break;
	default:
		cs+=4; l-=2; break;
	}
	
	s=(char *)cs;
	tl=strlen(s)+1;
	
	*rlen=l-tl;
	*rtag=s;
	cs+=tl;
	return(cs);
}

byte *BTIC1_DecodeMarkerAPP13(byte *cs, byte *cse, int *rlen, char **rtag)
	{ return(BTIC1_DecodeMarkerFMT13(cs, cse, rlen, rtag)); }

byte *BTIC1_DecodeImage(BGBBTJ_BTIC1_Context *ctx, byte *cs, byte *cse)
{
	byte *cs1;
	int len;
	u32 fcc;
	int i;
	
	while(cs<cse)
	{
		if(*cs!=0xFF)
		{
			printf("BTIC1_DecodeImage: Resync\n");
			for(i=0; i<8; i++)
				printf("%02X ", cs[i]);
			printf("\n");
			cs=BTIC1_SyncMarker(cs, cse);
			continue;
		}
		
		if(cs[1]==BGBBTJ_BTIC1_MARK_FMT12)
		{
			cs1=BTIC1_DecodeMarkerFMT12(cs, cse, &len, &fcc);
			if(fcc==BTIC1_FCC_THDR)
			{
				BTIC1_DecodeImageHeader(ctx, cs1, len);
				cs=cs1+len;
				continue;
			}
			if(fcc==BTIC1_FCC_TDAT)
			{
				BTIC1_DecodeImageData(ctx, cs1, len);
				cs=cs1+len;
				continue;
			}

			if(fcc==BTIC1_FCC_TLDA)
			{
				BTIC1_DecodeImageLiteralData(ctx, cs1, len);
				cs=cs1+len;
				continue;
			}

			if(fcc==BTIC1_FCC_TDAZ)
			{
				BTIC1_DecodeImageDataZ(ctx, cs1, len);
				cs=cs1+len;
				continue;
			}

			if(fcc==BTIC1_FCC_TLDZ)
			{
				BTIC1_DecodeImageLiteralDataZ(ctx, cs1, len);
				cs=cs1+len;
				continue;
			}

			if(fcc==BTIC1_FCC_TEND)
				{ cs=cs1+len; break; }

			printf("BTIC1_DecodeImage: BAD Marker FMT12 %04s\n", &fcc);
			break;
		}

		if((cs[1]>=BGBBTJ_BTIC1_MARK_FMT0) &&
			(cs[1]<=BGBBTJ_BTIC1_MARK_FMT13))
		{
			printf("BTIC1_DecodeImage: BAD Marker FMT%d %s\n",
				cs[1]-BGBBTJ_BTIC1_MARK_FMT0);
			break;
		}

		if((cs[1]>=BGBBTJ_BTIC1_MARK_APP0) &&
			(cs[1]<=BGBBTJ_BTIC1_MARK_APP15))
		{
			cs1=BTIC1_DecodeMarkerGeneric(cs, cse, &len);
			cs=cs1+len;
			continue;
		}

		printf("BTIC1_DecodeImage: BAD Marker %02X\n %04s\n", cs[1]);
		break;
	}
	return(cs);
}

void BTIC1_DecodeCompLayerHeader(
	BGBBTJ_BTIC1_Context *ctx, byte *cs, int len)
{
	byte lhbuf[256];
	struct BGBBTJ_BTIC1_CompLayerHeader_Ds lh;
	BGBBTJ_BTIC1_ImageHeader *thdr;
	int id;
	int i;

	memset(lhbuf, 0, 256);
	BTIC1_EscapeDecodeBuffer(cs, len, lhbuf, 256);
	memcpy(&lh, lhbuf, sizeof(struct BGBBTJ_BTIC1_CompLayerHeader_Ds));

	id=BTIC1_UnwrapDWordSBE(lh.layerid);
	thdr=ctx->image[id-1];
	
	if(!thdr)
	{
		i=sizeof(BGBBTJ_BTIC1_ImageHeader);
		thdr=btic_malloc(i);
		memset(thdr, 0, i);
		thdr->layerid=id;
//		thdr->name=strdup(lh.name);
//		thdr->name[8]=0;

		strncpy(lhbuf, lh.name, 8);
		thdr->name=btic_strdup(lhbuf);

		ctx->image[id-1]=thdr;
		if(id>=ctx->n_image)
			ctx->n_image=id;

		i=ctx->cur_layer->n_image++;
		ctx->cur_layer->image[i]=thdr;
	}

	ctx->cur_img=thdr;

	thdr->flags=BTIC1_UnwrapDWordUBE(lh.flags);

//	thdr=ctx->cur_img;
//	thdr->width=BTIC1_UnwrapDWordSBE(lh.width);
//	thdr->height=BTIC1_UnwrapDWordSBE(lh.height);
//	thdr->imgtype=BTIC1_UnwrapWordUBE(lh.imgtype);
//	thdr->mip_start=lh.mip_start;
//	thdr->mip_end=lh.mip_end;
//	thdr->filtmode=lh.filtmode;
//	thdr->clrtype=lh.clrtype;
//	thdr->pixtype=lh.pixtype;
}

void BTIC1_DecodeCompLayerID(
	BGBBTJ_BTIC1_Context *ctx, byte *cs, int len)
{
	byte lhbuf[256];
	struct BGBBTJ_BTIC1_CompLayerID_Ds lh;
	BGBBTJ_BTIC1_ImageHeader *thdr;
	int id;
	int i;

	memset(lhbuf, 0, 256);
	BTIC1_EscapeDecodeBuffer(cs, len, lhbuf, 256);
	memcpy(&lh, lhbuf, sizeof(struct BGBBTJ_BTIC1_CompLayerID_Ds));

	id=BTIC1_UnwrapDWordSBE(lh.layerid);
	thdr=ctx->image[id-1];
	
	if(!thdr)
	{
		i=sizeof(BGBBTJ_BTIC1_ImageHeader);
		thdr=btic_malloc(i);
		memset(thdr, 0, i);
		thdr->layerid=id;

		ctx->image[id-1]=thdr;
		if(id>=ctx->n_image)
			ctx->n_image=id;

//		i=ctx->cur_layer->n_image++;
//		ctx->cur_layer->image[i]=thdr;
	}

	ctx->cur_img=thdr;

	thdr->flags=BTIC1_UnwrapDWordUBE(lh.flags);
}

void BTIC1_DecodeDumpBytes(BGBBTJ_BTIC1_Context *ctx, byte *cs, byte *cse)
{
	int i, j;

	printf("%08X ", cs-ctx->frm_css);

	for(i=0; i<16; i++)
		printf("%02X ", cs[i]);
	for(i=0; i<16; i++)
	{
		j=cs[i];
		if((j<' ') || (j>'~'))
			j='~';
		printf("%c", j);
	}
	printf("\n");
}

byte *BTIC1_DecodeLayer(BGBBTJ_BTIC1_Context *ctx, byte *cs, byte *cse)
{
	byte *cs1;
	char *tag;
	int len;
	u32 fcc;
	int i;
	
	while(cs<cse)
	{
		if(*cs!=0xFF)
		{
			printf("BTIC1_DecodeLayer: Resync\n");
			BTIC1_DecodeDumpBytes(ctx, cs, cse);
			cs=BTIC1_SyncMarker(cs, cse);
			continue;
		}
		
		if(cs[1]==BGBBTJ_BTIC1_MARK_FMT12)
		{
			cs1=BTIC1_DecodeMarkerFMT12(cs, cse, &len, &fcc);

#if 0
			if(fcc==BTIC1_FCC_THDR)
			{
				BTIC1_DecodeImageHeader(ctx, cs1, len);
				cs=cs1+len;
				continue;
			}
			if(fcc==BTIC1_FCC_TDAT)
			{
				BTIC1_DecodeImageData(ctx, cs1, len);
				cs=cs1+len;
				continue;
			}
#endif

			if(fcc==BTIC1_FCC_LEND)
				{ cs=cs1+len; break; }

			printf("BTIC1_DecodeLayer: BAD Marker FMT12 %04s\n", &fcc);
			break;
		}

		if(cs[1]==BGBBTJ_BTIC1_MARK_FMT13)
		{
			cs1=BTIC1_DecodeMarkerFMT13(cs, cse, &len, &tag);
			if(!strcmp(tag, "CompLayer"))
			{
				BTIC1_DecodeCompLayerHeader(ctx, cs1, len);
				cs=cs1+len;
				cs=BTIC1_DecodeImage(ctx, cs, cse);
				continue;
			}
			printf("BTIC1_DecodeLayer: BAD Marker FMT13 %s\n", tag);
			break;
		}

		if((cs[1]>=BGBBTJ_BTIC1_MARK_FMT0) &&
			(cs[1]<=BGBBTJ_BTIC1_MARK_FMT13))
		{
			printf("BTIC1_DecodeLayer: BAD Marker FMT%d %s\n",
				cs[1]-BGBBTJ_BTIC1_MARK_FMT0);
			break;
		}
		
		if((cs[1]>=BGBBTJ_BTIC1_MARK_APP0) &&
			(cs[1]<=BGBBTJ_BTIC1_MARK_APP15))
		{
			cs1=BTIC1_DecodeMarkerGeneric(cs, cse, &len);
			cs=cs1+len;
			continue;
		}

		printf("BTIC1_DecodeLayer: BAD Marker %02X\n %04s\n", cs[1]);
		break;
	}
	return(cs);
}

void BTIC1_DecodeTagLayerHeader(
	BGBBTJ_BTIC1_Context *ctx, byte *cs, int len)
{
	byte lhbuf[256];
	struct BGBBTJ_BTIC1_TagLayerHeader_Ds lh;
	BGBBTJ_BTIC1_LayerHeader *lhdr;
	int id;
	int i;

	memset(lhbuf, 0, 256);
	BTIC1_EscapeDecodeBuffer(cs, len, lhbuf, 256);
	memcpy(&lh, lhbuf, sizeof(struct BGBBTJ_BTIC1_TagLayerHeader_Ds));

	id=BTIC1_UnwrapDWordSBE(lh.layerid);
	lhdr=ctx->layer[id-1];
	
	if(!lhdr)
	{
		i=sizeof(BGBBTJ_BTIC1_LayerHeader);
		lhdr=btic_malloc(i);
		memset(lhdr, 0, i);
		lhdr->layerid=id;
		strncpy(lhbuf, lh.name, 32);
		lhdr->name=btic_strdup(lhbuf);
		ctx->layer[id-1]=lhdr;
		if(id>ctx->n_layer)
			ctx->n_layer=id;
	}

	lhdr->xorg=BTIC1_UnwrapDWordSBE(lh.xorg);
	lhdr->yorg=BTIC1_UnwrapDWordSBE(lh.yorg);
	lhdr->xsize=BTIC1_UnwrapDWordSBE(lh.xsize);
	lhdr->ysize=BTIC1_UnwrapDWordSBE(lh.ysize);
	lhdr->xcenter=BTIC1_UnwrapDWordSBE(lh.xcenter);
	lhdr->ycenter=BTIC1_UnwrapDWordSBE(lh.ycenter);
	lhdr->flags=BTIC1_UnwrapDWordUBE(lh.flags);

	ctx->cur_layer=lhdr;
}

byte *BTIC1_DecodeImageContents(BGBBTJ_BTIC1_Context *ctx,
	byte *cs, byte *cse)
{
	byte *cs1;
	char *tag;
	int len;
	u32 fcc;
	
	while(cs<cse)
	{
		if(*cs!=0xFF)
		{
			printf("BTIC1_DecodeImageContents: Resync\n");
			cs=BTIC1_SyncMarker(cs, cse);
			continue;
		}
		
		if(cs[1]==BGBBTJ_BTIC1_MARK_FMT12)
		{
			cs1=BTIC1_DecodeMarkerFMT12(cs, cse, &len, &fcc);

//			if(fcc==BTIC1_FCC_LEND)
//				{ cs=cs1+len; break; }

			if(fcc==BTIC1_FCC_CLID)
			{
				BTIC1_DecodeCompLayerID(ctx, cs1, len);
				cs=cs1+len;
				cs=BTIC1_DecodeImage(ctx, cs, cse);
				continue;
			}

			printf("BTIC1_DecodeImagePlainBuffer: "
				"BAD Marker FMT12 %04s\n", &fcc);
			break;
		}

		if(cs[1]==BGBBTJ_BTIC1_MARK_FMT13)
		{
			cs1=BTIC1_DecodeMarkerFMT13(cs, cse, &len, &tag);

#if 0
			if(!strcmp(tag, "CompLayer"))
			{
				BTIC1_DecodeCompLayerHeader(ctx, cs1, len);
				cs=cs1+len;
				cs=BTIC1_DecodeImage(ctx, cs, cse);
				continue;
			}
#endif

			if(!strcmp(tag, "TagLayer"))
			{
				BTIC1_DecodeTagLayerHeader(ctx, cs1, len);
				cs=cs1+len;
				cs=BTIC1_DecodeLayer(ctx, cs, cse);
				continue;
			}
			printf("BTIC1_DecodeImagePlainBuffer: "
				"BAD Marker FMT13 %s\n", tag);
			break;
		}

		if((cs[1]>=BGBBTJ_BTIC1_MARK_FMT0) &&
			(cs[1]<=BGBBTJ_BTIC1_MARK_FMT13))
		{
			printf("BTIC1_DecodeImagePlainBuffer: "
				"BAD Marker FMT%d %s\n",
				cs[1]-BGBBTJ_BTIC1_MARK_FMT0);
			break;
		}
		
		if((cs[1]>=BGBBTJ_BTIC1_MARK_APP0) &&
			(cs[1]<=BGBBTJ_BTIC1_MARK_APP15))
		{
			cs1=BTIC1_DecodeMarkerGeneric(cs, cse, &len);
			cs=cs1+len;
			continue;
		}

		printf("BTIC1_DecodeLayer: BAD Marker %02X\n %04s\n", cs[1]);
		break;
	}
	
	return(cs);
}

void BTIC1_DecodeImagePlainBuffer(
	BGBBTJ_BTIC1_Context *ctx, byte *ibuf, int len)
{
	byte *tbuf;
	byte *cs, *cse;
	int sz;
	
	tbuf=btic_malloc(len);
	sz=BTIC1_EscapeDecodeBuffer(ibuf, len, tbuf, len);
	if(sz<0) { btic_free(tbuf); return; }
	
	cs=tbuf; cse=tbuf+sz;
	ctx->frm_css=cs;
	ctx->frm_cse=cse;
	
	cs=BTIC1_DecodeImageContents(ctx, cs, cse);

	btic_free(tbuf);
}

void BTIC1_DecodeImageArithBuffer(
	BGBBTJ_BTIC1_Context *ctx, byte *ibuf, int len)
{
	byte *tbuf, *tbuf1;
	byte *cs, *cse;
	int sz, sz1;
	
	tbuf=btic_malloc(len);
	sz=BTIC1_EscapeDecodeBuffer(ibuf, len, tbuf, len);
	if(sz<0) { btic_free(tbuf); return; }
	
	tbuf1=btic_malloc(1<<24);
	sz1=BGBBTJ_BitArith_DecodeData(tbuf, sz, tbuf1, 1<<24);
	
//	cs=tbuf; cse=tbuf+sz;
	cs=tbuf1; cse=tbuf1+sz1;
	ctx->frm_css=cs;
	ctx->frm_cse=cse;
	
	cs=BTIC1_DecodeImageContents(ctx, cs, cse);
	
	btic_free(tbuf);
	btic_free(tbuf1);
}

void BTIC1_DecodeImageDeflateBuffer(
	BGBBTJ_BTIC1_Context *ctx, byte *ibuf, int len)
{
	byte *tbuf, *tbuf1;
	byte *cs, *cse;
	int sz, sz1;
	
	tbuf=btic_malloc(len);
	sz=BTIC1_EscapeDecodeBuffer(ibuf, len, tbuf, len);
	if(sz<0) { btic_free(tbuf); return; }
	
	tbuf1=btic_malloc(1<<24);
//	sz1=BGBBTJ_BitArith_DecodeData(tbuf, sz, tbuf1, 1<<24);
	sz1=PDUNZ_DecodeStream(tbuf+2, tbuf1, sz, 1<<24);

//	cs=tbuf; cse=tbuf+sz;
	cs=tbuf1; cse=tbuf1+sz1;
	ctx->frm_css=cs;
	ctx->frm_cse=cse;
	
	cs=BTIC1_DecodeImageContents(ctx, cs, cse);
	
	btic_free(tbuf);
	btic_free(tbuf1);
}

void BTIC1_DecodeImageArithDeflateBuffer(
	BGBBTJ_BTIC1_Context *ctx, byte *ibuf, int len)
{
	byte *tbuf, *tbuf1;
	byte *cs, *cse;
	int sz, sz1;
	
	tbuf=btic_malloc(len);
	sz=BTIC1_EscapeDecodeBuffer(ibuf, len, tbuf, len);
	if(sz<0) { btic_free(tbuf); return; }
	
	tbuf1=btic_malloc(1<<24);
	sz1=BGBBTJ_BitArith_DecodeData(tbuf, sz, tbuf1, 1<<24);

	btic_free(tbuf);
	tbuf=tbuf1;
	tbuf1=btic_malloc(1<<24);
	sz=sz1;

//	sz1=PDUNZ_DecodeStream(tbuf, tbuf1, sz, 1<<24);
	PDUNZ_DecodeStreamSz(tbuf, tbuf1, sz, 1<<24, &sz1, 0);

//	cs=tbuf; cse=tbuf+sz;
	cs=tbuf1; cse=tbuf1+sz1;
	ctx->frm_css=cs;
	ctx->frm_cse=cse;
	
	cs=BTIC1_DecodeImageContents(ctx, cs, cse);
	
	btic_free(tbuf);
	btic_free(tbuf1);
}

int BTIC1_DecodeContext(
	BGBBTJ_BTIC1_Context *ctx, byte *ibuf, int sz)
{
	byte *cs1;
	char *tag;
	int len;

	if((ibuf[0]!=0xFF) || (ibuf[1]!=BGBBTJ_BTIC1_MARK_FMT13))
		return(-1);
	cs1=BTIC1_DecodeMarkerFMT13(ibuf, ibuf+sz, &len, &tag);
	
	if(!strcmp(tag, "BTIC1"))
	{
		BTIC1_DecodeImagePlainBuffer(ctx, cs1, len);
		cs1+=len;
		return(cs1-ibuf);
	}

	if(!strcmp(tag, "BTIC1A"))
	{
		BTIC1_DecodeImageArithBuffer(ctx, cs1, len);
		cs1+=len;
		return(cs1-ibuf);
	}

	if(!strcmp(tag, "BTIC1Z"))
	{
		BTIC1_DecodeImageDeflateBuffer(ctx, cs1, len);
		cs1+=len;
		return(cs1-ibuf);
	}

	if(!strcmp(tag, "BTIC1AZ"))
	{
		BTIC1_DecodeImageArithDeflateBuffer(ctx, cs1, len);
		cs1+=len;
		return(cs1-ibuf);
	}
	
	return(-1);
}

void BTIC1_InverseColorConvertImage(
	BGBBTJ_BTIC1_Context *ctx,
	BGBBTJ_BTIC1_ImageHeader *thdr, int pf)
{
	byte *tbuf;
	int sz, sz1;

//	if(!thdr->rgba)
//		return;

	switch(thdr->imgtype)
	{
	case BGBBTJ_JPG_BC1:
	case BGBBTJ_JPG_BC1F:
	case BGBBTJ_JPG_BC1A:
		sz=((thdr->width+3)/4)*((thdr->height+3)/4)*8;
		sz1=thdr->width*thdr->height*4;
//		BTIC1_CheckBufferImageData(ctx, thdr, sz);
		BTIC1_CheckBufferImageRGBA(ctx, thdr, sz1);
		if(thdr->flags&BGBBTJ_BTIC1_LFL_PFRAME)
		{
			BGBBTJ_BCn_DecodeImageDXTn(
				thdr->data+thdr->ofs_pframe,
				thdr->rgba,
				thdr->width, thdr->height,
				4, thdr->imgtype);
		}else
		{
			BGBBTJ_BCn_DecodeImageDXTn(thdr->data, thdr->rgba,
				thdr->width, thdr->height,
				4, thdr->imgtype);
		}
		break;
	case BGBBTJ_JPG_BC3:
	case BGBBTJ_JPG_BC3F:
	case BGBBTJ_JPG_BC3_UVAY:
		sz=((thdr->width+3)/4)*((thdr->height+3)/4)*16;
		sz1=thdr->width*thdr->height*4;
//		BTIC1_CheckBufferImageData(ctx, thdr, sz);
		BTIC1_CheckBufferImageRGBA(ctx, thdr, sz1);
		if(thdr->flags&BGBBTJ_BTIC1_LFL_PFRAME)
		{
			BGBBTJ_BCn_DecodeImageDXTn(
				thdr->data+thdr->ofs_pframe,
				thdr->rgba,
				thdr->width, thdr->height,
				4, thdr->imgtype);
		}else
		{
			BGBBTJ_BCn_DecodeImageDXTn(
				thdr->data, thdr->rgba,
				thdr->width, thdr->height,
				4, thdr->imgtype);
		}
		break;
	default:
		break;
	}
}

bool BTIC1_CheckFormatEquiv(int fmt1, int fmt2)
{
	if(fmt1==fmt2)return(true);
	
	if((fmt1==BGBBTJ_JPG_BC1) || (fmt1==BGBBTJ_JPG_BC1F) ||
		(fmt1==BGBBTJ_JPG_BC1A))
	{
		if((fmt2==BGBBTJ_JPG_BC1) || (fmt2==BGBBTJ_JPG_BC1F) ||
			(fmt2==BGBBTJ_JPG_BC1A))
		{
			return(true);
		}
	}

	if((fmt1==BGBBTJ_JPG_BC3) || (fmt1==BGBBTJ_JPG_BC3F))
	{
		if((fmt2==BGBBTJ_JPG_BC3) || (fmt2==BGBBTJ_JPG_BC3F))
		{
			return(true);
		}
	}
	
	return(0);
}

void BTIC1_GetImageBuffer(
	BGBBTJ_BTIC1_Context *ctx, BGBBTJ_BTIC1_ImageHeader *img,
	byte *rgba, int xs, int ys, int pf)
{
	int sz;

	if((img->width!=xs) || (img->height!=ys))
	{
		return;
	}
	
	//if possible, copy image directly
	if(BTIC1_CheckFormatEquiv(pf, img->imgtype))
	{
		switch(img->imgtype)
		{
		case BGBBTJ_JPG_BC1:
		case BGBBTJ_JPG_BC1F:
		case BGBBTJ_JPG_BC1A:
			sz=((img->width+3)/4)*((img->height+3)/4)*8;
			if(img->flags&BGBBTJ_BTIC1_LFL_PFRAME)
				{ memcpy(rgba, img->data+img->ofs_pframe, sz); }
			else
				{ memcpy(rgba, img->data, sz); }
			break;
		case BGBBTJ_JPG_BC3:
		case BGBBTJ_JPG_BC3F:
		case BGBBTJ_JPG_BC3_UVAY:
			sz=((img->width+3)/4)*((img->height+3)/4)*16;
			if(img->flags&BGBBTJ_BTIC1_LFL_PFRAME)
				{ memcpy(rgba, img->data+img->ofs_pframe, sz); }
			else
				{ memcpy(rgba, img->data, sz); }
			break;
		default:
			break;
		}
		
		return;
	}

	if((pf==BGBBTJ_JPG_BC3) || (pf==BGBBTJ_JPG_BC3F))
	{
		if(	(img->imgtype==BGBBTJ_JPG_BC1) ||
			(img->imgtype==BGBBTJ_JPG_BC1F) ||
			(img->imgtype==BGBBTJ_JPG_BC1A))
		{
			if(img->flags&BGBBTJ_BTIC1_LFL_PFRAME)
			{
				BGBBTJ_BCn_TranscodeBC1ToBC3(
					img->data+img->ofs_pframe, rgba, xs, ys);
			}else
			{
				BGBBTJ_BCn_TranscodeBC1ToBC3(
					img->data, rgba, xs, ys);
			}
			return;
		}
	}

	//otherwise, convert to RGBA then into the target format
	
	BTIC1_InverseColorConvertImage(ctx, img, pf);

	switch(pf)
	{
	case BGBBTJ_JPG_BC1:
	case BGBBTJ_JPG_BC1F:
	case BGBBTJ_JPG_BC1A:
		BGBBTJ_BCn_EncodeImageDXTn(rgba, img->rgba,
			img->width, img->height, 4, pf);
		break;
	case BGBBTJ_JPG_BC3:
	case BGBBTJ_JPG_BC3F:
	case BGBBTJ_JPG_BC3_UVAY:
		BGBBTJ_BCn_EncodeImageDXTn(rgba, img->rgba,
			img->width, img->height, 4, pf);
		break;
	default:
		memcpy(rgba, img->rgba, xs*ys*4);
		break;
	}
	
//	memcpy(rgba, img->rgba, xs*ys*4);
}

BGBBTJ_API int BTIC1_GetLayerComponentCtx(
	BGBBTJ_BTIC1_Context *ctx, char *lname,
	byte *rgba, byte *norm, byte *spec, byte *luma,
	int *rxs, int *rys, int pf)
{
	BGBBTJ_BTIC1_LayerHeader *lhdr;
	BGBBTJ_BTIC1_ImageHeader *ihdr;
	int i;

	lhdr=BTIC1_LookupContextLayer(ctx, lname);
	if(!lhdr)return(-1);
	
	*rxs=lhdr->xsize;
	*rys=lhdr->ysize;
	
	if(rgba)
	{
		ihdr=BTIC1_LookupLayerImage(ctx, lhdr, "RGBA");
		BTIC1_GetImageBuffer(ctx, ihdr, rgba, *rxs, *rys, pf);
	}

	if(norm)
	{
		ihdr=BTIC1_LookupLayerImage(ctx, lhdr, "XYZD");
		BTIC1_GetImageBuffer(ctx, ihdr, norm, *rxs, *rys, pf);
	}

	if(spec)
	{
		ihdr=BTIC1_LookupLayerImage(ctx, lhdr, "SpRGBS");
		BTIC1_GetImageBuffer(ctx, ihdr, spec, *rxs, *rys, pf);
	}

	if(luma)
	{
		ihdr=BTIC1_LookupLayerImage(ctx, lhdr, "LuRGBS");
		BTIC1_GetImageBuffer(ctx, ihdr, luma, *rxs, *rys, pf);
	}

	return(0);
}

BGBBTJ_API int BTIC1_DecodeComponentCtx(BGBBTJ_BTIC1_Context *ctx,
	byte *buf, int sz,
	byte *rgba, byte *norm, byte *spec, byte *luma,
	int *xs, int *ys, int pf)
{
	int i;

	i=BTIC1_DecodeContext(ctx, buf, sz);
	BTIC1_GetLayerComponentCtx(ctx, NULL,
		rgba, norm, spec, luma,
		xs, ys, pf);
	return(i);
}

BGBBTJ_API int BTIC1_DecodeComponent(
	byte *buf, int sz,
	byte *rgba, byte *norm, byte *spec, byte *luma,
	int *rxs, int *rys, int pf)
{
	int i;

	BGBBTJ_BTIC1_Context *ctx;
	
	ctx=BTIC1_AllocContext();
	i=BTIC1_DecodeComponentCtx(ctx,
		buf, sz,
		rgba, norm, spec, luma,
		rxs, rys, pf);
	BTIC1_FreeContext(ctx);
	return(i);
}
