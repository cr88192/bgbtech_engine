#include <bgbbtj.h>

extern float pdjpg_exptab[256];
extern float pdjpg_scltab[256];

BGBBTJ_API byte *BGBBTJ_JPG_DecodeScanForComponentLayer(
	byte *buf, int sz, char *name)
{
	byte *cs, *cs2, *cse;
	int i;
	
	cs=buf; cse=buf+sz;
	while(cs<cse)
	{
		if((cs[0]==0xFF) && (cs[1]==JPG_APP11))
		{
			i=(cs[2]<<8)|cs[3];
			cs2=cs+i+2;

			cs+=4;
			if(!strcmp((char *)cs, "CompLayer"))
			{
				cs+=strlen((char *)cs)+1;
				if(!strcmp((char *)cs, name))
				{
//					printf("Found %s\n", name);
					cs+=strlen((char *)cs)+1;
					return(cs2);
				}

				cs+=strlen((char *)cs)+1;
				continue;
			}

			if(!strcmp((char *)cs, "TagLayer"))
				break;

			cs+=i-2;
			continue;
		}
		if((cs[0]==0xFF) && (cs[1]>=JPG_APP0) && (cs[1]<=JPG_APP15))
		{
			i=(cs[2]<<8)|cs[3];
			cs+=i+2;
			continue;
		}

		cs++;
	}
	
	return(NULL);
}

BGBBTJ_API byte *BGBBTJ_JPG_DecodeScanForTagLayer(
	byte *buf, int sz, char *name)
{
	byte *cs, *cs2, *cse;
	int i;
	
	cs=buf; cse=buf+sz;
	while(cs<cse)
	{
		if((cs[0]==0xFF) && (cs[1]==JPG_APP11))
		{
			i=(cs[2]<<8)|cs[3];
			cs2=cs+i+2;

			cs+=4;
			if(!strcmp((char *)cs, "TagLayer"))
			{
				cs+=strlen((char *)cs)+1;
				if(!strcmp((char *)cs, name))
				{
//					printf("Found %s\n", name);
					cs+=strlen((char *)cs)+1;
					return(cs2);
				}

				cs+=strlen((char *)cs)+1;
				continue;
			}
			cs+=i-2;
			continue;
		}
		if((cs[0]==0xFF) && (cs[1]>=JPG_APP0) && (cs[1]<=JPG_APP15))
		{
			i=(cs[2]<<8)|cs[3];
			cs+=i+2;
			continue;
		}

		cs++;
	}
	
	return(NULL);
}

BGBBTJ_API byte *BGBBTJ_JPG_DecodeScanNextTagLayer(
	byte *buf, int sz, char **rname)
{
	byte *cs, *cs2, *cse;
	int i;
	
	cs=buf; cse=buf+sz;
	while(cs<cse)
	{
		if((cs[0]==0xFF) && (cs[1]==JPG_APP11))
		{
			i=(cs[2]<<8)|cs[3];
			cs2=cs+i+2;

			cs+=4;
			if(!strcmp((char *)cs, "TagLayer"))
			{
				*rname=dystrdup((char *)cs);
				return(cs2);
			}
			cs+=i-2;
			continue;
		}
		if((cs[0]==0xFF) && (cs[1]>=JPG_APP0) && (cs[1]<=JPG_APP15))
		{
			i=(cs[2]<<8)|cs[3];
			cs+=i+2;
			continue;
		}

		cs++;
	}
	
	return(NULL);
}

BGBBTJ_API byte *BGBBTJ_JPG_Decode(byte *buf, int sz, int *xs, int *ys)
{
	BGBBTJ_JPG_Context *ctx;
	byte *obuf;
	byte *otbuf;
	byte *csl;
	int i, n, sz1;
	
	ctx=BGBBTJ_JPG_AllocContext();
	obuf=BGBBTJ_JPG_DecodeCtx(ctx, buf, sz, xs, ys);
	ctx->jpg_imgbuf=NULL;
	
	csl=BGBBTJ_JPG_DecodeScanForComponentLayer(buf, sz, "Alpha");
	if(csl)
	{
		sz1=sz-(csl-buf);
//		otbuf=BGBBTJ_JPG_DecodeLDatCtx(ctx, csl, sz1, xs, ys);
		otbuf=BGBBTJ_JPG_DecodeLDatCtxClrs(ctx, csl, sz1,
			BGBBTJ_JPG_YYYA, xs, ys);
		
		n=ctx->xs*ctx->ys;
		for(i=0; i<n; i++)
		{
			obuf[i*4+3]=otbuf[i*4+1];
		}
	}

	BGBBTJ_JPG_FreeContext(ctx);
	return(obuf);
}

BGBBTJ_API int BGBBTJ_JPG_DecodeBufferClrs(
	byte *ibuf, int isz, byte *obuf, int clrs, int *xs, int *ys)
{
	BGBBTJ_JPG_Context *ctx;
	int i;

	ctx=BGBBTJ_JPG_AllocContext();
	i=BGBBTJ_JPG_DecodeBufferClrsCtx(ctx, ibuf, isz, obuf, clrs, xs, ys);
	BGBBTJ_JPG_FreeContext(ctx);
	return(i);
}

BGBBTJ_API int BGBBTJ_JPG_DecodeBufferClrsCtx(
	BGBBTJ_JPG_Context *ctx,
	byte *ibuf, int isz, byte *obuf, int clrs, int *xs, int *ys)
{
	byte tbb[16];
//	BGBBTJ_JPG_Context *ctx;
	byte *ocbuf;
	byte *otbuf;
	byte *cs, *ct, *csl;
	int n, sz1, xs1, ys1;
	int i, j, k, l;

	if(clrs==BGBBTJ_JPG_DXT1)
	{
//		ctx=BGBBTJ_JPG_AllocContext();
		ocbuf=BGBBTJ_JPG_DecodeCtxClrs(ctx, ibuf, isz, clrs, xs, ys);
		xs1=(*xs+3)/4;
		ys1=(*ys+3)/4;
		memcpy(obuf, ocbuf, xs1*ys1*8);
		if(ocbuf)free(ocbuf);
//		BGBBTJ_JPG_FreeContext(ctx);
		return(isz);
	}

	if((clrs==BGBBTJ_JPG_BC7) || (clrs==BGBBTJ_JPG_BC7_VF) ||
		(clrs==BGBBTJ_JPG_BC7_VF2))
	{
		ocbuf=NULL;
		otbuf=NULL;

//		ctx=BGBBTJ_JPG_AllocContext();
		ocbuf=BGBBTJ_JPG_DecodeCtxClrs(ctx, ibuf, isz, clrs, xs, ys);
		ctx->jpg_imgbuf=NULL;

		xs1=(*xs+3)/4;
		ys1=(*ys+3)/4;

		if((clrs==BGBBTJ_JPG_BC7_VF) ||
			(clrs==BGBBTJ_JPG_BC7_VF2))
		{
//			BGBBTJ_BC7_FlipImageBC7(ocbuf, obuf, xs1*4, ys1*4);
			BGBBTJ_BC7_FlipImageBlocksBC7(ocbuf, obuf, xs1*4, ys1*4);
		}else
		{
			memcpy(obuf, ocbuf, xs1*ys1*16);
		}

		if(ocbuf)free(ocbuf);
		if(otbuf)free(otbuf);

//		BGBBTJ_JPG_FreeContext(ctx);
		return(isz);
	}

	if((clrs==BGBBTJ_JPG_BC3) ||
		(clrs==BGBBTJ_JPG_BC3F) ||
		(clrs==BGBBTJ_JPG_BC3_VF))
	{
		ocbuf=NULL;
		otbuf=NULL;

//		ctx=BGBBTJ_JPG_AllocContext();
		ocbuf=BGBBTJ_JPG_DecodeCtxClrs(ctx, ibuf, isz, clrs, xs, ys);
		ctx->jpg_imgbuf=NULL;

		xs1=(*xs+3)/4;
		ys1=(*ys+3)/4;

		memcpy(obuf, ocbuf, xs1*ys1*16);
		
		csl=BGBBTJ_JPG_DecodeScanForComponentLayer(ibuf, isz, "Alpha");
		if(!csl)
			csl=BGBBTJ_JPG_DecodeScanForComponentLayer(ibuf, isz, "DASe");
		if(!csl)
			csl=BGBBTJ_JPG_DecodeScanForComponentLayer(ibuf, isz, "DABe");
		if(csl)
//		if(0)
		{
			sz1=isz-(csl-ibuf);
//			otbuf=BGBBTJ_JPG_DecodeLDatCtx(ctx, csl, sz1, xs, ys);
			otbuf=BGBBTJ_JPG_DecodeLDatCtxClrs(ctx, csl, sz1,
				BGBBTJ_JPG_BC4, xs, ys);
		
//			n=ctx->xs*ctx->ys;
			n=xs1*ys1;
			for(i=0; i<n; i++)
			{
				bgbbtj_bt1c_memcpy8(obuf+i*16, otbuf+i*8);
//				obuf[i*4+3]=otbuf[i*4+1];
			}
		}else
		{
			tbb[0]=255;		tbb[1]=255;
			tbb[2]=0;		tbb[3]=0;
			tbb[4]=0;		tbb[5]=0;
			tbb[6]=0;		tbb[7]=0;

			n=xs1*ys1;
			for(i=0; i<n; i++)
			{
				bgbbtj_bt1c_memcpy8(obuf+i*16, tbb);
//				obuf[i*4+3]=otbuf[i*4+1];
			}
		}

		if(clrs==BGBBTJ_JPG_BC3_VF)
		{
			n=xs1*ys1;
			memcpy(ocbuf, obuf, n*16);
			BGBBTJ_BCn_FlipImageDXT5(ocbuf, obuf, *xs, *ys);
		}

		if(ocbuf)free(ocbuf);
		if(otbuf)free(otbuf);

//		BGBBTJ_JPG_FreeContext(ctx);
		return(isz);
	}

	if((clrs==BGBBTJ_JPG_MB5C) ||
		(clrs==BGBBTJ_JPG_MB5C_VF))
	{
		ocbuf=NULL;
		otbuf=NULL;

//		ctx=BGBBTJ_JPG_AllocContext();
		ocbuf=BGBBTJ_JPG_DecodeCtxClrs(ctx, ibuf, isz, clrs, xs, ys);
		ctx->jpg_imgbuf=NULL;

		xs1=(*xs+3)/4;
		ys1=(*ys+3)/4;

//		memcpy(obuf, ocbuf, xs1*ys1*32);
		n=xs1*ys1;
		for(i=0; i<n; i++)
		{
			bgbbtj_bt1c_memcpy8(obuf+i*32+8, ocbuf+i*32+8);
			cs=ocbuf+i*32+8;
			ct=obuf+i*32+8;
			j=cs[0]|(cs[1]<<8);
			k=cs[2]|(cs[3]<<8);
			j=BGBBTJ_BT1C_ConvRGB16ToRGB24(NULL, j);
			k=BGBBTJ_BT1C_ConvRGB16ToRGB24(NULL, k);
			j=BGBBTJ_BT1C_SwapRGB24_RB(j);
			k=BGBBTJ_BT1C_SwapRGB24_RB(k);
			bgbbtj_bt1c_setule32(ct+8, j);
			bgbbtj_bt1c_setule32(ct+12, k);
//			obuf[i*4+3]=otbuf[i*4+1];
		}
		
		csl=BGBBTJ_JPG_DecodeScanForComponentLayer(ibuf, isz, "Alpha");
		if(!csl)
			csl=BGBBTJ_JPG_DecodeScanForComponentLayer(ibuf, isz, "DASe");
		if(!csl)
			csl=BGBBTJ_JPG_DecodeScanForComponentLayer(ibuf, isz, "DABe");
		if(csl)
//		if(0)
		{
			sz1=isz-(csl-ibuf);
//			otbuf=BGBBTJ_JPG_DecodeLDatCtx(ctx, csl, sz1, xs, ys);
			otbuf=BGBBTJ_JPG_DecodeLDatCtxClrs(ctx, csl, sz1,
				BGBBTJ_JPG_BC4, xs, ys);
		
//			n=ctx->xs*ctx->ys;
			n=xs1*ys1;
			for(i=0; i<n; i++)
			{
				bgbbtj_bt1c_memcpy8(obuf+i*32, otbuf+i*8);
//				obuf[i*4+3]=otbuf[i*4+1];
			}
		}else
		{
			tbb[0]=255;		tbb[1]=255;
			tbb[2]=0;		tbb[3]=0;
			tbb[4]=0;		tbb[5]=0;
			tbb[6]=0;		tbb[7]=0;

			n=xs1*ys1;
			for(i=0; i<n; i++)
			{
				bgbbtj_bt1c_memcpy8(obuf+i*32, tbb);
//				obuf[i*4+3]=otbuf[i*4+1];
			}
		}

		if(clrs==BGBBTJ_JPG_MB5C_VF)
		{
			n=xs1*ys1;
			memcpy(ocbuf, obuf, n*32);
//			BGBBTJ_BCn_FlipImageDXT5(ocbuf, obuf, *xs, *ys);
			BGBBTJ_BCn_FlipImageMB5C(ocbuf, obuf, *xs, *ys);
		}

		if(ocbuf)free(ocbuf);
		if(otbuf)free(otbuf);

//		BGBBTJ_JPG_FreeContext(ctx);
		return(isz);
	}
	
//	ctx=BGBBTJ_JPG_AllocContext();
	ocbuf=BGBBTJ_JPG_DecodeCtxClrs(ctx, ibuf, isz, clrs, xs, ys);
	ctx->jpg_imgbuf=NULL;
		
	csl=BGBBTJ_JPG_DecodeScanForComponentLayer(ibuf, isz, "Alpha");
	if(!csl)
		csl=BGBBTJ_JPG_DecodeScanForComponentLayer(ibuf, isz, "DASe");
	if(!csl)
		csl=BGBBTJ_JPG_DecodeScanForComponentLayer(ibuf, isz, "DABe");
	if(csl)
	{
		sz1=isz-(csl-ibuf);
//		otbuf=BGBBTJ_JPG_DecodeLDatCtx(ctx, csl, sz1, xs, ys);
		otbuf=BGBBTJ_JPG_DecodeLDatCtxClrs(ctx, csl, sz1,
			BGBBTJ_JPG_YYYA, xs, ys);
		
		n=ctx->xs*ctx->ys;
		for(i=0; i<n; i++)
		{
			obuf[i*4+3]=otbuf[i*4+1];
		}
	}

//	BGBBTJ_JPG_FreeContext(ctx);
	return(isz);
}

BGBBTJ_API int BGBBTJ_JPG_DecodeComponentType(byte *buf, int sz)
{
	byte *csl;
	int sz1;

//	sz1=(sz>1024)?1024:sz;
//	csl=BGBBTJ_JPG_DecodeScanForComponentLayer(buf, sz1, "RGB");
//	if(!csl)return(0);

	csl=BGBBTJ_JPG_DecodeScanForComponentLayer(buf, sz, "DASe");
	if(csl)
	{
		csl=BGBBTJ_JPG_DecodeScanForComponentLayer(buf, sz, "LuRGB");
		if(csl)return(16);
		csl=BGBBTJ_JPG_DecodeScanForComponentLayer(buf, sz, "SpRGB");
		if(csl)return(12);
		return(8);
	}else
	{
		csl=BGBBTJ_JPG_DecodeScanForComponentLayer(buf, sz, "LuRGB");
		if(csl)return(7);

		csl=BGBBTJ_JPG_DecodeScanForComponentLayer(buf, sz, "Alpha");
		if(csl)return(4);

		return(3);
	}
}

BGBBTJ_API byte *BGBBTJ_JPG_DecodeLDatCtx(BGBBTJ_JPG_Context *ctx,
	byte *buf, int sz, int *xs, int *ys)
{
	return(BGBBTJ_JPG_DecodeLDatCtxClrs(ctx, buf, sz,
		BGBBTJ_JPG_RGBA, xs, ys));
}

BGBBTJ_API byte *BGBBTJ_JPG_DecodeLDatCtxClrs(BGBBTJ_JPG_Context *ctx,
	byte *buf, int sz, int clrs, int *xs, int *ys)
{
	byte *tbuf, *tbuf2, *cs, *ct;
	int i;

	if((buf[0]!=0xFF) || (buf[1]!=JPG_APP12) ||
		(buf[4]!='L') || (buf[5]!='D') ||
		(buf[6]!='A') || (buf[7]!='T'))
	{
		tbuf=BGBBTJ_JPG_DecodeCtxClrs(ctx, buf, sz, clrs, xs, ys);
		return(tbuf);
	}
	
	tbuf=malloc(1<<20);
	cs=buf; ct=tbuf;

	while((cs[0]==0xFF) && (cs[1]==JPG_APP12) &&
		(cs[4]=='L') && (cs[5]=='D') &&
		(cs[6]=='A') && (cs[7]=='T'))
	{
		i=(cs[2]<<8)|cs[3];
		memcpy(ct, cs+8, i-6);
		cs+=i+2; ct+=(i-6);
	}

	i=ct-tbuf;
	i=BGBBTJ_JPG_EscapeDecodeSingleBuffer(tbuf, i);

	tbuf2=BGBBTJ_JPG_DecodeCtxClrs(ctx, tbuf, i, clrs, xs, ys);
	free(tbuf);
	return(tbuf2);
}

BGBBTJ_API int BGBBTJ_JPG_DummyDecodeLDatCtx(BGBBTJ_JPG_Context *ctx,
	byte *buf, int sz, int *xs, int *ys)
{
	byte *tbuf, *tbuf2, *cs, *ct;
	int i, j;

	if((buf[0]!=0xFF) || (buf[1]!=JPG_APP12) ||
		(buf[4]!='L') || (buf[5]!='D') ||
		(buf[6]!='A') || (buf[7]!='T'))
	{
		i=BGBBTJ_JPG_DummyDecodeCtx(ctx, buf, sz, xs, ys);
		return(i);
	}
	
	tbuf=malloc(1<<20);
	cs=buf; ct=tbuf;

	while((cs[0]==0xFF) && (cs[1]==JPG_APP12) &&
		(cs[4]=='L') && (cs[5]=='D') &&
		(cs[6]=='A') && (cs[7]=='T'))
	{
		i=(cs[2]<<8)|cs[3];
		memcpy(ct, cs+8, i-6);
		cs+=i+2; ct+=(i-6);
	}

	i=ct-tbuf;
	i=BGBBTJ_JPG_EscapeDecodeSingleBuffer(tbuf, i);

	j=BGBBTJ_JPG_DummyDecodeCtx(ctx, tbuf, i, xs, ys);
	free(tbuf);
	return(j);
}

BGBBTJ_API int BGBBTJ_JPG_DecodeComponentCtx(BGBBTJ_JPG_Context *ctx,
	byte *buf, int sz,
	byte *rgba, byte *norm, byte *spec, byte *luma,
	int *xs, int *ys)
{
	return(BGBBTJ_JPG_DecodeComponentCtxI(ctx, buf, sz,
		BGBBTJ_JPG_RGBA,
		rgba, norm, spec, luma, NULL,
		xs, ys));
}

BGBBTJ_API int BGBBTJ_JPG_DecodeComponentCtxClrs(BGBBTJ_JPG_Context *ctx,
	byte *buf, int sz, int clrs,
	byte *rgba, byte *norm, byte *spec, byte *luma,
	int *xs, int *ys)
{
	return(BGBBTJ_JPG_DecodeComponentCtxI(ctx, buf, sz, clrs,
		rgba, norm, spec, luma, NULL,
		xs, ys));
}

BGBBTJ_API int BGBBTJ_JPG_DecodeFloatComponentCtx(BGBBTJ_JPG_Context *ctx,
	byte *buf, int sz,
	float *rgba, float *norm, float *spec, float *luma,
	int *xs, int *ys)
{
	byte *trgba, *tnorm, *tspec, *tluma, *tlebe;
	int i, n;
	float f, g;

	i=BGBBTJ_JPG_DummyDecodeCtx(ctx, buf, sz, xs, ys);
	if(i<0)return(-1);

	n=ctx->xs*ctx->ys;
	if(!ctx->imgbuf_rgba)
		ctx->imgbuf_rgba=malloc(4*n);
	if(!ctx->imgbuf_norm)
		ctx->imgbuf_norm=malloc(4*n);
	if(!ctx->imgbuf_spec)
		ctx->imgbuf_spec=malloc(4*n);
	if(!ctx->imgbuf_luma)
		ctx->imgbuf_luma=malloc(4*n);

	if(!ctx->imgbuf_lebe)
	{
		ctx->imgbuf_lebe=malloc(4*n);
		memset(ctx->imgbuf_lebe, 192, 4*n);
	}

	i=BGBBTJ_JPG_DecodeComponentCtxI(ctx, buf, sz,
		BGBBTJ_JPG_RGBA,
		ctx->imgbuf_rgba, ctx->imgbuf_norm,
		ctx->imgbuf_spec, ctx->imgbuf_luma,
		ctx->imgbuf_lebe,
		xs, ys);
	if(i<0)
	{
		return(-1);
	}
	
	trgba=ctx->imgbuf_rgba;
	tnorm=ctx->imgbuf_norm;
	tspec=ctx->imgbuf_spec;
	tluma=ctx->imgbuf_luma;
	tlebe=ctx->imgbuf_lebe;
	
	if(rgba)
	{
		for(i=0; i<n; i++)
		{
			f=(1.0/256)*pdjpg_scltab[tlebe[i*4+0]];
			rgba[i*4+0]=trgba[i*4+0]*f;
			rgba[i*4+1]=trgba[i*4+1]*f;
			rgba[i*4+2]=trgba[i*4+2]*f;
			rgba[i*4+3]=trgba[i*4+3]*(1.0/256);
		}
	}

	if(norm)
	{
		for(i=0; i<n; i++)
		{
//			f=(1.0/256)*pdjpg_scltab[tlebe[i*4+0]];
			f=1.0/256;
			g=(1.0/256)*pdjpg_scltab[tlebe[i*4+2]];
			norm[i*4+0]=tnorm[i*4+0]*f;
			norm[i*4+1]=tnorm[i*4+1]*f;
			norm[i*4+2]=tnorm[i*4+2]*f;
			norm[i*4+3]=tnorm[i*4+3]*g;
		}
	}

	if(spec)
	{
		for(i=0; i<n; i++)
		{
//			f=(1.0/256)*pdjpg_scltab[tlebe[i*4+0]];
			f=1.0/256;
			spec[i*4+0]=tspec[i*4+0]*f;
			spec[i*4+1]=tspec[i*4+1]*f;
			spec[i*4+2]=tspec[i*4+2]*f;
			spec[i*4+3]=tspec[i*4+3]*f;
		}
	}

	if(luma)
	{
		for(i=0; i<n; i++)
		{
			f=(1.0/256)*pdjpg_scltab[tlebe[i*4+1]];
			luma[i*4+0]=tluma[i*4+0]*f;
			luma[i*4+1]=tluma[i*4+1]*f;
			luma[i*4+2]=tluma[i*4+2]*f;
			luma[i*4+3]=tluma[i*4+3]*(1.0/256);
		}
	}
	return(0);
}

BGBBTJ_API int BGBBTJ_JPG_ExtractComponentCtx(BGBBTJ_JPG_Context *ctx,
	byte *rgba, byte *norm, byte *spec, byte *luma, byte *lebe)
{
	int n;
	
	n=ctx->xs*ctx->ys;
	if(rgba && ctx->imgbuf_rgba)
		memcpy(rgba, ctx->imgbuf_rgba, n*4);
	if(norm && ctx->imgbuf_norm)
		memcpy(norm, ctx->imgbuf_norm, n*4);
	if(spec && ctx->imgbuf_spec)
		memcpy(spec, ctx->imgbuf_spec, n*4);
	if(luma && ctx->imgbuf_luma)
		memcpy(luma, ctx->imgbuf_luma, n*4);
	if(lebe && ctx->imgbuf_lebe)
		memcpy(lebe, ctx->imgbuf_lebe, n*4);
	return(0);
}

int BGBBTJ_JPG_DecodeComponentCtxI_DXT5(BGBBTJ_JPG_Context *ctx,
	byte *buf, int sz, int clrs,
	byte *rgba, byte *norm, byte *spec, byte *luma, byte *lebe,
	int *xs, int *ys)
{
	byte *tbuf;
	byte *csl;
	int i, n, sz1, fbsz, fbn;

	csl=BGBBTJ_JPG_DecodeScanForComponentLayer(buf, sz, "RGB");
	if(csl)
	{
		sz1=sz-(csl-buf);
		tbuf=BGBBTJ_JPG_DecodeLDatCtxClrs(ctx, csl, sz1, clrs, xs, ys);
		if(!tbuf) return(-1);
	}
	else
	{
		tbuf=BGBBTJ_JPG_DecodeCtxClrs(ctx, buf, sz, clrs, xs, ys);
		if(!tbuf) return(-1);
	}
	
//	fbn=((*xs)/4)*((*ys)/4);
	fbn=(ctx->xs/4)*(ctx->ys/4);
	fbsz=fbn*16;
	memcpy(rgba, tbuf, fbsz);

	csl=BGBBTJ_JPG_DecodeScanForComponentLayer(buf, sz, "Alpha");
	if(csl)
	{
		sz1=sz-(csl-buf);
		tbuf=BGBBTJ_JPG_DecodeLDatCtx(ctx, csl, sz1, xs, ys);
		BGBBTJ_BCn_EncodeIdxImageAlphaDXT5(rgba, tbuf, *xs, *ys, 4, 1);
	}else
	{
		BGBBTJ_BCn_FillImageAlphaDXT5(rgba, 255, *xs, *ys);
	}

	return(0);
}

int BGBBTJ_JPG_DecodeComponentCtxI_BC7(BGBBTJ_JPG_Context *ctx,
	byte *buf, int sz, int clrs,
	byte *rgba, byte *norm, byte *spec, byte *luma, byte *lebe,
	int *xs, int *ys)
{
	byte *tbuf;
	byte *csl;
	int i, n, sz1, fbsz, fbn;

	csl=BGBBTJ_JPG_DecodeScanForComponentLayer(buf, sz, "RGB");
	if(csl)
	{
		sz1=sz-(csl-buf);
		tbuf=BGBBTJ_JPG_DecodeLDatCtxClrs(ctx, csl, sz1, clrs, xs, ys);
		if(!tbuf) return(-1);
	}
	else
	{
		tbuf=BGBBTJ_JPG_DecodeCtxClrs(ctx, buf, sz, clrs, xs, ys);
		if(!tbuf) return(-1);
	}
	
//	fbn=((*xs)/4)*((*ys)/4);
	fbn=(ctx->xs/4)*(ctx->ys/4);
	fbsz=fbn*16;
	memcpy(rgba, tbuf, fbsz);

#if 0
	csl=BGBBTJ_JPG_DecodeScanForComponentLayer(buf, sz, "Alpha");
	if(csl)
	{
		sz1=sz-(csl-buf);
		tbuf=BGBBTJ_JPG_DecodeLDatCtx(ctx, csl, sz1, xs, ys);
		BGBBTJ_BCn_EncodeIdxImageAlphaDXT5(rgba, tbuf, *xs, *ys, 4, 1);
	}else
	{
		BGBBTJ_BCn_FillImageAlphaDXT5(rgba, 255, *xs, *ys);
	}
#endif

	return(0);
}

int BGBBTJ_JPG_DecodeComponentCtxI(BGBBTJ_JPG_Context *ctx,
	byte *buf, int sz, int clrs,
	byte *rgba, byte *norm, byte *spec, byte *luma, byte *lebe,
	int *xs, int *ys)
{
	byte *tbuf;
	byte *csl;
	int i, n, sz1;

	if((clrs==BGBBTJ_JPG_BC3) || (clrs==BGBBTJ_JPG_BC3F))
	{
		i=BGBBTJ_JPG_DecodeComponentCtxI_DXT5(ctx, buf, sz, clrs,
			rgba, norm, spec, luma, lebe, xs, ys);
		return(i);
	}

	if((clrs==BGBBTJ_JPG_BC7) || (clrs==BGBBTJ_JPG_BC7_VF))
	{
		i=BGBBTJ_JPG_DecodeComponentCtxI_BC7(ctx, buf, sz, clrs,
			rgba, norm, spec, luma, lebe, xs, ys);
		return(i);
	}

//	tbuf=BGBBTJ_JPG_DecodeCtx(ctx, buf, sz, xs, ys);
//	if(!tbuf)return(-1);

	csl=BGBBTJ_JPG_DecodeScanForComponentLayer(buf, sz, "RGB");
	if(csl)
	{
		sz1=sz-(csl-buf);
		tbuf=BGBBTJ_JPG_DecodeLDatCtx(ctx, csl, sz1, xs, ys);
		if(!tbuf) return(-1);
	}
	else
	{
		tbuf=BGBBTJ_JPG_DecodeCtx(ctx, buf, sz, xs, ys);
		if(!tbuf) return(-1);
	}

	n=ctx->xs*ctx->ys;

	for(i=0; i<n; i++)
	{
		rgba[i*4+0]=tbuf[i*4+0];
		rgba[i*4+1]=tbuf[i*4+1];
		rgba[i*4+2]=tbuf[i*4+2];
		rgba[i*4+3]=255;
//		rgba[i*4+3]=tbuf[i*4+3];
	}

	if(norm)
	{
		csl=BGBBTJ_JPG_DecodeScanForComponentLayer(buf, sz, "XYZ");
		if(csl)
		{
			sz1=sz-(csl-buf);
			tbuf=BGBBTJ_JPG_DecodeLDatCtx(ctx, csl, sz1, xs, ys);
			for(i=0; i<n; i++)
			{
				norm[i*4+0]=tbuf[i*4+0];
				norm[i*4+1]=tbuf[i*4+2];
				norm[i*4+2]=tbuf[i*4+1];
//				norm[i*4+3]=tbuf[i*4+3];
			}
		}
	}

	if(spec)
	{
		csl=BGBBTJ_JPG_DecodeScanForComponentLayer(buf, sz, "SpRGB");
		if(csl)
		{
			sz1=sz-(csl-buf);
			tbuf=BGBBTJ_JPG_DecodeLDatCtx(ctx, csl, sz1, xs, ys);
			for(i=0; i<n; i++)
			{
				spec[i*4+0]=tbuf[i*4+0];
				spec[i*4+1]=tbuf[i*4+1];
				spec[i*4+2]=tbuf[i*4+2];
//				spec[i*4+3]=tbuf[i*4+3];
			}
		}
	}

	csl=BGBBTJ_JPG_DecodeScanForComponentLayer(buf, sz, "DASe");
	if(csl && (norm || spec))
	{
		sz1=sz-(csl-buf);
		tbuf=BGBBTJ_JPG_DecodeLDatCtx(ctx, csl, sz1, xs, ys);
		for(i=0; i<n; i++)
		{
			rgba[i*4+3]=tbuf[i*4+1];
			if(norm) { norm[i*4+3]=tbuf[i*4+0]; }
			if(spec) { spec[i*4+3]=tbuf[i*4+2]; }
		}
	}

	csl=BGBBTJ_JPG_DecodeScanForComponentLayer(buf, sz, "DABe");
	if(csl)
	{
		sz1=sz-(csl-buf);
		tbuf=BGBBTJ_JPG_DecodeLDatCtx(ctx, csl, sz1, xs, ys);
		for(i=0; i<n; i++)
		{
			rgba[i*4+3]=tbuf[i*4+1];
			if(norm) { norm[i*4+3]=tbuf[i*4+0]; }
			if(lebe) { lebe[i*4+1]=tbuf[i*4+2]; }
		}
	}

	csl=BGBBTJ_JPG_DecodeScanForComponentLayer(buf, sz, "Alpha");
	if(csl)
	{
		sz1=sz-(csl-buf);
		tbuf=BGBBTJ_JPG_DecodeLDatCtx(ctx, csl, sz1, xs, ys);
		for(i=0; i<n; i++)
			{ rgba[i*4+3]=tbuf[i*4+1]; }
	}
		
	if(norm)
	{
		csl=BGBBTJ_JPG_DecodeScanForComponentLayer(buf, sz, "Depth");
		if(csl)
		{
			sz1=sz-(csl-buf);
			tbuf=BGBBTJ_JPG_DecodeLDatCtx(ctx, csl, sz1, xs, ys);
			for(i=0; i<n; i++)
				{ norm[i*4+3]=tbuf[i*4+1]; }
		}
	}
	
	if(luma)
	{
		csl=BGBBTJ_JPG_DecodeScanForComponentLayer(buf, sz, "LuRGB");
		if(csl)
		{
			sz1=sz-(csl-buf);
			tbuf=BGBBTJ_JPG_DecodeLDatCtx(ctx, csl, sz1, xs, ys);
			for(i=0; i<n; i++)
			{
				luma[i*4+0]=tbuf[i*4+0];
				luma[i*4+1]=tbuf[i*4+1];
				luma[i*4+2]=tbuf[i*4+2];
				luma[i*4+3]=255;
			}
		}
	}

	if(lebe)
	{
		csl=BGBBTJ_JPG_DecodeScanForComponentLayer(buf, sz, "LeBe");
		if(csl)
		{
			sz1=sz-(csl-buf);
			tbuf=BGBBTJ_JPG_DecodeLDatCtx(ctx, csl, sz1, xs, ys);
			for(i=0; i<n; i++)
			{
				lebe[i*4+0]=tbuf[i*4+0];
				lebe[i*4+1]=tbuf[i*4+1];
				lebe[i*4+2]=tbuf[i*4+2];
			}
		}
	}

	return(0);
}

BGBBTJ_API int BGBBTJ_JPG_DecodeComponent(byte *buf, int sz,
	byte *rgba, byte *norm, byte *spec, byte *luma,
	int *xs, int *ys)
{
	BGBBTJ_JPG_Context *ctx;
	int i;
	
	ctx=BGBBTJ_JPG_AllocContext();
	i=BGBBTJ_JPG_DecodeComponentCtx(ctx, buf, sz,
		rgba, norm, spec, luma, xs, ys);
//	ctx->jpg_imgbuf=NULL;
	BGBBTJ_JPG_FreeContext(ctx);
	return(i);
}

BGBBTJ_API int BGBBTJ_JPG_DecodeComponentClrs(
	byte *buf, int sz, int clrs,
	byte *rgba, byte *norm, byte *spec, byte *luma,
	int *xs, int *ys)
{
	BGBBTJ_JPG_Context *ctx;
	int i;
	
	ctx=BGBBTJ_JPG_AllocContext();
	i=BGBBTJ_JPG_DecodeComponentCtxI(ctx, buf, sz, clrs,
		rgba, norm, spec, luma, NULL, xs, ys);
//	ctx->jpg_imgbuf=NULL;
	BGBBTJ_JPG_FreeContext(ctx);
	return(i);
}

BGBBTJ_API int BGBBTJ_JPG_DummyDecodeLayerInfo(byte *buf, int sz,
	BGBBTJ_JPG_LayerInfo *linf)
{
	BGBBTJ_JPG_Context *ctx;
	byte *csl;
	int xs, ys, sz1;
	int i;
	
	ctx=BGBBTJ_JPG_AllocContext();
//	i=BGBBTJ_JPG_DecodeComponentCtx(ctx, buf, sz,
//		rgba, norm, spec, luma, xs, ys);
//	ctx->jpg_imgbuf=NULL;

	csl=BGBBTJ_JPG_DecodeScanForComponentLayer(buf, sz, "RGB");
	if(csl)
	{
		sz1=sz-(csl-buf);
		i=BGBBTJ_JPG_DummyDecodeLDatCtx(ctx, csl, sz1, &xs, &ys);
		if(i<0) return(i);
	}
	else
	{
		i=BGBBTJ_JPG_DummyDecodeCtx(ctx, buf, sz, &xs, &ys);
		if(i<0) return(i);
	}
	
	if((xs<=0) || (ys<=0))
		return(-1);

	linf->flags=ctx->bcst_flags;
	linf->alpha=ctx->bcst_alpha;
	linf->blend=ctx->bcst_blend;
	linf->orgx=ctx->bcst_orgx;
	linf->orgy=ctx->bcst_orgy;
	linf->xs=xs;
	linf->ys=ys;

	linf->minx=ctx->bcst_minx;
	linf->miny=ctx->bcst_miny;
	linf->maxx=ctx->bcst_maxx;
	linf->maxy=ctx->bcst_maxy;

	BGBBTJ_JPG_FreeContext(ctx);
	return(i);
}
