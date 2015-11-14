#include <bgbbtj.h>

BGBBTJ_API int BGBBTJ_BT1D_EncodeBlocks_LookupRun(
	byte *csc, byte *css, byte *cse,
	int *rlen, int *rdist, int fl)
{
	byte *cs, *cs1;
	int bi, bl, ml;
	int i, j, k;

	if(!(fl&BGBBTJ_BTIC1C_EFL_BLOCKLZ))
		{ *rlen=0; *rdist=0; return(0); }
		
	bi=0; bl=0; ml=256;
	cs=csc-16;
	while(cs>css)
	{
		i=(csc-cs)/16;
		if(i>=65536) { break; }
		else if(i>=4096) { ml=1; }
		else if(i>=1024) { ml=8; }
		else if(i>=256) { ml=32; }
		else if(i>=64) { ml=64; }
		else if(i>=16) { ml=128; }

		for(j=0; j<256; j++)
			if(memcmp(cs+j*16, csc+j*16, 16))
				break;
		if(j>bl) { bi=i; bl=j; }
		if(bl>=ml)break;
		cs-=16;
	}
	
	*rlen=bl;
	*rdist=bi;
	return(bl>0);
}

BGBBTJ_API int BGBBTJ_BT1D_EncodeBlocks(
	byte *obuf, int osz, byte *blks, byte *lblks,
	int xs, int ys, int fl)
{
	byte tb[16];
	byte *cs, *cse, *csm, *csl;
	byte *ct, *cte, *ctb;
	int xs1, ys1, n, n1;
	int xs2, ys2, xs3, ys3;
	int i0, i1, i2, i3;
	int j0, j1, j2, j3;
	int bl, bd, bm;
	int i, j, k, l;
	
	BGBBTJ_BT1D_Init();
	
	xs1=(xs+3)/4;
	ys1=(ys+3)/4;
	n=xs1*ys1;
	
	if(fl&BGBBTJ_BTIC1C_EFL_MIPMAP)
	{
		xs2=xs; ys2=ys; n=0;
		while((xs2>1) || (ys2>1))
		{
			if(!xs2)xs2++;
			if(!ys2)ys2++;
			xs3=(xs2+3)/4;
			ys3=(ys2+3)/4;
			n+=xs3*ys3;
			xs2>>=1; ys2>>=1;
		}
	}
	
	cs=blks; cse=cs+n*16; csl=lblks;
	ct=obuf;
	
	*ct++=0xE1;		*ct++=0x00;
	*ct++=0x00;		*ct++=0x00;
	
	while(cs<cse)
	{
#if 1
		if(lblks && !memcmp(cs, csl, 16))
		{
			for(i=1; (i<16) && ((cs+i*16)<cse); i++)
			{
				if(memcmp(cs+i*16, csl+i*16, 16))
					break;
			}

			*ct++=0x80+(i-1);
			cs+=i*16; csl+=i*16;
			continue;
		}
#endif

//		i0=cs[0]|(cs[1]<<8);
//		i1=cs[2]|(cs[3]<<8);

		i0=(cs[0]<<24)|(cs[1]<<16)|(cs[2]<<8)|(cs[3]);
		i1=(cs[4]<<24)|(cs[5]<<16)|(cs[6]<<8)|(cs[7]);
		bm=((cs[0]>>2)&0x38)|((cs[4]>>5)&0x07);
//		bm=((i0>>26)&0x38)|((i1>>29)&0x07);
		j0=(cs[ 8]<<8)|(cs[ 9]);
		j1=(cs[10]<<8)|(cs[11]);
		j2=(cs[12]<<8)|(cs[13]);
		j3=(cs[14]<<8)|(cs[15]);

		if(BGBBTJ_BT1D_EncodeBlocks_LookupRun(
			cs, blks, cse, &bl, &bd, fl))
		{
			if((bl<=16) && (bd==1))
				{ *ct++=0xC0+(bl-1);
				cs+=bl*16; csl+=bl*16; continue; }
			if((bl<=16) && (bd<128))
				{ *ct++=0xD0+(bl-1); *ct++=bd;
				cs+=bl*16; csl+=bl*16; continue; }
			if((bl<=16) && (bd<16384))
				{ *ct++=0xD0+(bl-1);
				*ct++=0x80|((bd>>8)&0x3F);
				*ct++=bd&0xFF;
				cs+=bl*16; csl+=bl*16; continue; }
			if((bl<=16) && (bd<2097152))
				{ *ct++=0xD0+(bl-1);
				*ct++=0xC0|((bd>>16)&0x1F);
				*ct++=(bd>>8)&0xFF;
				*ct++=bd&0xFF;
				cs+=bl*16; csl+=bl*16; continue; }

			if((bl==1) && (bd==1))
				{ *ct++=0xE8; cs+=bl*16; csl+=bl*16; continue; }
			if((bl<=256) && (bd==1))
				{ *ct++=0xE9; *ct++=bl-1;
				cs+=bl*16; csl+=bl*16; continue; }
			if((bl==1) && (bd<=256))
				{ *ct++=0xEA; *ct++=bd-1;
				cs+=bl*16; csl+=bl*16; continue; }
			if((bl==1) && (bd<=65536))
			{
				j=bd-1; *ct++=0xEB;
				*ct++=(j>>8)&255; *ct++=j&255;
				cs+=bl*16; csl+=bl*16; continue;
			}
			if((bl<=256) && (bd<=256))
				{ *ct++=0xEC; *ct++=bl-1; *ct++=bd-1;
				cs+=bl*16; csl+=bl*16; continue; }
			if((bl<=256) && (bd<=65536))
			{
				*ct++=0xED; *ct++=bl-1;
				j=bd-1; *ct++=(j>>8)&255; *ct++=j&255;
				cs+=bl*16; csl+=bl*16; continue;
			}
		}

#if 0
		if((i0&0x1FFFFFFF)==(i1&0x1FFFFFFF))
//		if(0)
		{
			for(i=0; (i<16) && ((cs+i*16)<cse); i++)
			{
				if(memcmp(cs+i*16, cs, 16))
					break;
			}
			
			if(i>0)
			{
				*ct++=0xA0+(i-1);
				switch(bm)
				{
				case 0:		case 1:
					ct[0]=(i0>>24)&0xFF;
					ct[1]=(i0>>16)&0xFF;
					ct[2]=(i0>>8)&0xFF;
					ct[3]=(i0)&0xFF;
					ct+=4;
					break;
				case 2:
//					j=31;
					j=((j0&15)+(j1&15)+(j2&15)+(j3&15)+2)>>1;
					
					i2=	(2<<29)|
						(j<<24)|
						((i0&0x1FE00000)>>5)|
						((i0&0x0003FC00)>>2)|
						((i0&0x000001FE)>>1);
					ct[0]=(i2>>24)&0xFF;
					ct[1]=(i2>>16)&0xFF;
					ct[2]=(i2>>8)&0xFF;
					ct[3]=(i2)&0xFF;
					ct+=4;
					break;
				case 3:
//					j=31;
					j=((j0&15)+(j1&15)+(j2&15)+(j3&15)+2)>>1;
					
					i2=	(i0&0xFFFE0000)|
						((i0&0x0000FC00)<<1)|
						((i0&0x000000FC)<<3)|
						j;
					ct[0]=(i2>>24)&0xFF;
					ct[1]=(i2>>16)&0xFF;
					ct[2]=(i2>>8)&0xFF;
					ct[3]=(i2)&0xFF;
					ct+=4;
					break;
				default:
					break;
				}

				cs+=i*16; csl+=i*16;
				continue;
			}
		}
#endif

#if 1
//		if((i0&0x1FFFFFFF)==(i1&0x1FFFFFFF))
//		if(0)
//		if((cs[0]==0xE1) || (cs[0]==0xE2))
		if((bm>=48) && (bm<56))
		{
			for(i=0; (i<16) && ((cs+i*16)<cse); i++)
			{
				if(memcmp(cs+i*16, cs, 16))
					break;
			}
			
			if(i>0)
			{
				*ct++=0xA0+(i-1);

//				if(cs[0]==0xE1)
				if(bm>=52)
				{
					ct[0]=cs[4];
					ct[1]=cs[5];
					ct+=2;
				}else
				{
					ct[0]=cs[4];
					ct[1]=cs[5];
					ct[2]=cs[6];
					ct[3]=cs[7];
					ct+=4;
				}

				cs+=i*16; csl+=i*16;
				continue;
			}
		}
#endif

		if(1)
		{
			for(i=1; (i<16) && ((cs+i*16)<cse); i++)
			{
//				i2=cs[i*16+0]|(cs[i*16+1]<<8);
//				i3=cs[i*16+2]|(cs[i*16+3]<<8);
				i2=(cs[i*16+0]<<24)|(cs[i*16+1]<<16)|
					(cs[i*16+2]<<8)|(cs[i*16+3]);
				i3=(cs[i*16+4]<<24)|(cs[i*16+5]<<16)|
					(cs[i*16+6]<<8)|(cs[i*16+7]);
				if((i2!=i0) || (i3!=i1))
					break;
			}
			
			if((i>1) || (i0&0x80000000))
			{
				n1=i;
				*ct++=0xB0+(i-1);

//				if(bm==56)
//				if(bm==63)
				if((cs[0]&0xE0)==0xE0)
				{
					//Half-Width
					ct[0]=(i1>>24)&0xFF;
					ct[1]=(i1>>16)&0xFF;
					ct[2]=(i1>> 8)&0xFF;
					ct[3]=(i1    )&0xFF;
					ct+=4;
				
					for(i=0; i<n1; i++)
					{
						ct[0]=cs[ 8];	ct[1]=cs[ 9];
						ct[2]=cs[10];	ct[3]=cs[11];
						cs+=16; csl+=16; ct+=4;
					}
					continue;
				}

				ct[0]=(i0>>24)&0xFF;	ct[1]=(i0>>16)&0xFF;
				ct[2]=(i0>> 8)&0xFF;	ct[3]=(i0    )&0xFF;
				ct[4]=(i1>>24)&0xFF;	ct[5]=(i1>>16)&0xFF;
				ct[6]=(i1>> 8)&0xFF;	ct[7]=(i1    )&0xFF;
				ct+=8;
				
				for(i=0; i<n1; i++)
				{
					ct[0]=cs[ 8];	ct[1]=cs[ 9];
					ct[2]=cs[10];	ct[3]=cs[11];
					ct[4]=cs[12];	ct[5]=cs[13];
					ct[6]=cs[14];	ct[7]=cs[15];
					cs+=16; csl+=16; ct+=8;
				}
				continue;
			}

			ct[0]=(i0>>24)&0xFF;	ct[1]=(i0>>16)&0xFF;
			ct[2]=(i0>>8)&0xFF;		ct[3]=(i0)&0xFF;
			ct[4]=(i1>>24)&0xFF;	ct[5]=(i1>>16)&0xFF;
			ct[6]=(i1>>8)&0xFF;		ct[7]=(i1)&0xFF;
			ct+=8;
			
			ct[0]=cs[ 8];	ct[1]=cs[ 9];
			ct[2]=cs[10];	ct[3]=cs[11];
			ct[4]=cs[12];	ct[5]=cs[13];
			ct[6]=cs[14];	ct[7]=cs[15];
			cs+=16; csl+=16; ct+=8;
			continue;
		}
	}

	i=ct-obuf;
	obuf[1]=(i>>16)&0xFF;
	obuf[2]=(i>>8)&0xFF;
	obuf[3]=(i>>0)&0xFF;

	return(i);
}

BGBBTJ_API int BGBBTJ_BT1D_EncodeBlocksZ(
	byte *obuf, int osz, byte *blks, byte *lblks,
	int xs, int ys, int fl)
{
	byte *tbuf1, *tbuf2;
	int sz1, sz2;	
	tbuf1=malloc(osz);
	tbuf2=malloc(osz);
	sz1=BGBBTJ_BT1D_EncodeBlocks(tbuf1, osz, blks, lblks, xs, ys, fl);
	
	if(sz1<256)
//	if(1)
	{
		memcpy(obuf, tbuf1, sz1);
		free(tbuf1);
		free(tbuf2);
		return(sz1);
	}
	
//	sz2=PDZ2_EncodeStream64LvlZl(tbuf1, tbuf2+6, sz1, osz-6, 9);
	sz2=PDZ2_EncodeStream64LvlZl(tbuf1, tbuf2+6, sz1, osz-6, 5);
	sz2+=6;
	if(sz2<(0.85*sz1))
	{
		memcpy(obuf, tbuf2, sz2);
		obuf[0]=0xE3;			obuf[1]=(sz2>>16)&255;
		obuf[2]=(sz2>>8)&255;	obuf[3]=(sz2>>0)&255;
		obuf[4]='Z';			obuf[5]='I';
		free(tbuf1);
		free(tbuf2);
		return(sz2);
	}else
	{
		memcpy(obuf, tbuf1, sz1);
		free(tbuf1);
		free(tbuf2);
		return(sz1);
	}
}

BGBBTJ_API BGBBTJ_BTIC1C_Context *BGBBTJ_BT1D_AllocContext()
{
	BGBBTJ_BTIC1C_Context *ctx;
	BGBBTJ_BTIC1C_LayerInfo *lyr;
	BGBBTJ_BTIC1C_ImageInfo *hdr;

	ctx=gctalloc("btic1c_context_t", sizeof(BGBBTJ_BTIC1C_Context));
	lyr=gctalloc("btic1c_layerinfo_t", sizeof(BGBBTJ_BTIC1C_LayerInfo));
	hdr=gctalloc("btic1c_imageinfo_t", sizeof(BGBBTJ_BTIC1C_ImageInfo));

	hdr->ctx=ctx;
	hdr->layer=lyr;

	lyr->image[0]=hdr;
	lyr->n_image=1;
	lyr->ctx=ctx;

	ctx->layer[0]=lyr;
	ctx->image[0]=hdr;
	ctx->n_layer=1;
	ctx->n_image=1;
	
	return(ctx);
}

BGBBTJ_API int BGBBTJ_BT1D_EncodeImageFast(
	BGBBTJ_BTIC1C_Context *ctx, byte *rgba, byte *obuf, int osz,
	int xs, int ys, int qf, int clrs)
{
	return(BGBBTJ_BT1D_EncodeImage(
		ctx, rgba, obuf, osz, xs, ys, qf, clrs));
}

BGBBTJ_API int BGBBTJ_BT1D_EncodeImage(
	BGBBTJ_BTIC1C_Context *ctx, byte *rgba, byte *obuf, int osz,
	int xs, int ys, int qf, int clrs)
{
	BGBBTJ_BTIC1C_ImageInfo *hdr;
	int sz;

	hdr=ctx->image[0];
	
	sz=BGBBTJ_BT1D_EncodeImage2(ctx, hdr,
		rgba, obuf, osz, xs, ys, qf, clrs);
	return(sz);
}

BGBBTJ_API int BGBBTJ_BT1D_EncodeImage2(
	BGBBTJ_BTIC1C_Context *ctx, BGBBTJ_BTIC1C_ImageInfo *hdr,
	byte *rgba, byte *obuf, int osz,
	int xs, int ys, int qf, int clrs)
{
	float *flrgba;
	s16 *fprgba;
	byte *ldat, *ct, *ct0;
	int xs1, ys1;
	int xs2, ys2;
	int xs3, ys3;
	int cr, cg, cb, ca;
	int sz, bsz, efl;
	int x1, y1, x2, y2;
	int i, j, k, l, n;
	
	hdr->width=xs;
	hdr->height=ys;
	hdr->flags=qf;
//	hdr->clrs=clrs;

	hdr->width2=(xs+3)/4;
	hdr->height2=(ys+3)/4;
	hdr->nblocks=hdr->width2*hdr->height2;
	hdr->nblocksmip=hdr->nblocks;

	if(qf&BGBBTJ_QFL_MIPMAP)
	{
		xs2=xs; ys2=ys; n=0;
		while((xs2>0) || (ys2>0))
		{
			if(!xs2)xs2++;
			if(!ys2)ys2++;
			xs3=(xs2+3)/4;
			ys3=(ys2+3)/4;
			n+=xs3*ys3;
			xs2>>=1; ys2>>=1;
		}
		hdr->nblocksmip=n;
	}

	if(!hdr->flrgba)
	{
//		i=(4*hdr->width2)*(4*hdr->height2)*4;
//		if(qf&BGBBTJ_QFL_MIPMAP)i*=2;
		i=hdr->nblocksmip*16*4*sizeof(float);
		hdr->flrgba=gcatomic(i);
		hdr->sz_rgba=i;
	}
	
	if(!hdr->data)
	{
//		i=hdr->nblocks*16;
		i=hdr->nblocksmip*16;
		hdr->data=gcatomic(i);
		hdr->sz_data=i;
	}

#if 1
	if(!hdr->fpdata)
	{
//		i=hdr->nblocks*16;
		i=hdr->nblocksmip*16;
		hdr->fpdata=gcatomic(i);
		hdr->sz_fpdata=i;
	}
#endif

	xs1=hdr->width2*4;
	ys1=hdr->height2*4;

#if 1
	if(clrs==BGBBTJ_JPG_RGBA)
	{
		n=xs*ys*4;
		for(i=0; i<n; i++)
			{ hdr->flrgba[i]=rgba[i]/255.0; }
	}

	if(clrs==BGBBTJ_JPG_RGBA_Q11_4)
	{
		n=xs*ys*4; fprgba=(short *)rgba;
		for(i=0; i<n; i++)
			{ hdr->flrgba[i]=fprgba[i]/4080.0; }
	}

	if(clrs==BGBBTJ_JPG_RGBA_F32)
	{
		n=xs*ys*4; flrgba=(float *)rgba;
		for(i=0; i<n; i++)
			{ hdr->flrgba[i]=flrgba[i]; }
	}
#endif

#if 0
	if(clrs==BGBBTJ_JPG_RGBA)
	{
		xs2=hdr->width2;
		ys2=hdr->height2;
		for(y1=0; y1<ys2; y1++)
			for(x1=0; x1<xs2; x1++)
		{
			for(k=0; k<4; k++)
			{
				for(l=0; l<4; l++)
				{
					ca=rgba[((y1*4+k)*xs+(x1*4+l))*4+3];
					if(ca>0)break;
				}
				if(l<4)break;
			}
			if(k<4)
			{
				for(y2=0; y2<4; y2++)
				{
					for(x2=0; x2<4; x2++)
					{
						j=((y1*4+y2)*xs+(x1*4+x2))*4;
						k=((y1*4+y2)*xs1+(x1*4+x2))*4;
						cr=rgba[j+0]; cg=rgba[j+1];
						cb=rgba[j+2]; ca=rgba[j+3];
						hdr->rgba[k+0]=cr; hdr->rgba[k+1]=cg;
						hdr->rgba[k+2]=cb; hdr->rgba[k+3]=ca;
					}
				}
			}else
			{
				for(y2=0; y2<4; y2++)
				{
					for(x2=0; x2<4; x2++)
					{
						k=((y1*4+y2)*xs1+(x1*4+x2))*4;
						cr=255; cg=0; cb=255; ca=0;
						hdr->rgba[k+0]=cr; hdr->rgba[k+1]=cg;
						hdr->rgba[k+2]=cb; hdr->rgba[k+3]=ca;
					}
				}
			}
		}
		
		BGBBTJ_Tex_Dither15(hdr->rgba, hdr->rgba, xs1, ys1);
	}
#endif

#if 0
	if(qf&BGBBTJ_QFL_MIPMAP)
	{
		xs2=xs1; ys2=ys1; ct0=hdr->rgba;
		while((xs2>1) || (ys2>1))
		{
			if(!xs2)xs2++;
			if(!ys2)ys2++;
			xs3=(xs2+3)/4;
			ys3=(ys2+3)/4;
//			n+=xs3*ys3;
//			ct=ct0+(xs3*ys3)*4;
			ct=ct0+(xs2*ys2)*4;
			BGBBTJ_Tex_HalfSampleDest(ct0, ct, xs2, ys2);
			xs2>>=1; ys2>>=1;
			ct0=ct;
		}
	}
#endif

	ldat=NULL;
	if(qf&BGBBTJ_QFL_PFRAME)
		ldat=hdr->ldata;

#if 1
	switch(clrs)
	{
	case BGBBTJ_JPG_RGBA:
	case BGBBTJ_JPG_RGBA_Q11_4:
	case BGBBTJ_JPG_RGBA_F32:
		if(qf&BGBBTJ_QFL_MIPMAP)
		{
			BGBBTJ_BT1D_EncodeImageBX1A_Mip(hdr->fpdata,
				hdr->flrgba, xs1, ys1, 4);
//			BGBBTJ_BCn_FlipImageDXT5_Mip(hdr->fpdata, hdr->data, xs1, ys1);
			BGBBTJ_BT1D_QuantizeBlocks(hdr->data, hdr->data, ldat,
				xs1, ys1, qf);
//			sz=BGBBTJ_BT1D_EncodeBlocksDXT5(obuf, osz,
//				hdr->data, ldat, xs, ys);
			sz=BGBBTJ_BT1D_EncodeBlocks(obuf, osz,
				hdr->data, ldat, xs1, ys1, BGBBTJ_BTIC1C_EFL_MIPMAP);
			bsz=16;
		}else
		{
			BGBBTJ_BT1D_EncodeImageBX1A(
				hdr->data, hdr->flrgba, xs1, ys1, 4);
//			BGBBTJ_BCn_FlipImageDXT5(hdr->fpdata, hdr->data, xs1, ys1);
			BGBBTJ_BT1D_QuantizeBlocks(hdr->data, hdr->data, ldat,
				xs1, ys1, qf);
//			sz=BGBBTJ_BT1D_EncodeBlocks(obuf, osz,
//				hdr->data, ldat, xs, ys);
//			sz=BGBBTJ_BT1D_EncodeBlocks(obuf, osz,
//				hdr->data, ldat, xs1, ys1, 0);
			sz=BGBBTJ_BT1D_EncodeBlocksZ(obuf, osz,
				hdr->data, ldat, xs1, ys1, 0);
			bsz=16;
		}
		break;
	default:
		break;
	}
#endif

#if 0
	switch(clrs)
	{
	case BGBBTJ_JPG_RGBA:
		if(qf&BGBBTJ_QFL_MIPMAP)
		{
			BGBBTJ_BCn_EncodeImageDXT5B_Mip(hdr->fpdata,
				hdr->rgba, xs1, ys1, 4);
			BGBBTJ_BCn_FlipImageDXT5_Mip(hdr->fpdata, hdr->data, xs1, ys1);
			BGBBTJ_BT1D_QuantizeBlocksDXT5(hdr->data, hdr->data, ldat,
				xs1, ys1, qf);
//			sz=BGBBTJ_BT1D_EncodeBlocksDXT5(obuf, osz,
//				hdr->data, ldat, xs, ys);
			sz=BGBBTJ_BT1D_EncodeBlocksDXT5(obuf, osz,
				hdr->data, ldat, xs1, ys1, BGBBTJ_BTIC1C_EFL_MIPMAP);
			bsz=16;
		}else
		{
#if 0
			BGBBTJ_BCn_EncodeImageDXT1A(hdr->fpdata, rgba, xs, ys, 4);
//			BGBBTJ_BCn_EncodeImageDXT1F(hdr->fpdata, rgba, xs, ys, 4);
			BGBBTJ_BCn_FlipImageDXT1(hdr->fpdata, hdr->data, xs, ys, 8);
			sz=BGBBTJ_BT1D_EncodeBlocks(obuf, osz,
				hdr->data, ldat, xs, ys);
			bsz=8;
#endif
#if 1
			BGBBTJ_BCn_EncodeImageDXT5B(hdr->fpdata, hdr->rgba, xs1, ys1, 4);
//			BGBBTJ_BCn_EncodeImageDXT5(hdr->fpdata, hdr->rgba, xs, ys, 4);
//			BGBBTJ_BCn_EncodeImageDXT5(hdr->fpdata, rgba, xs, ys, 4);
//			BGBBTJ_BCn_EncodeImageDXT1F(hdr->fpdata, rgba, xs, ys, 4);
			BGBBTJ_BCn_FlipImageDXT5(hdr->fpdata, hdr->data, xs1, ys1);
			BGBBTJ_BT1D_QuantizeBlocksDXT5(hdr->data, hdr->data, ldat,
				xs1, ys1, qf);
//			sz=BGBBTJ_BT1D_EncodeBlocksDXT5(obuf, osz,
//				hdr->data, ldat, xs, ys);
			sz=BGBBTJ_BT1D_EncodeBlocksDXT5(obuf, osz,
				hdr->data, ldat, xs1, ys1, 0);
			bsz=16;
#endif
		}
		break;
	case BGBBTJ_JPG_RGB:
		BGBBTJ_BCn_EncodeImageDXT1A(hdr->fpdata, rgba, xs, ys, 3);
//		BGBBTJ_BCn_EncodeImageDXT1F(hdr->fpdata, rgba, xs, ys, 3);
		BGBBTJ_BCn_FlipImageDXT1(hdr->fpdata, hdr->data, xs, ys, 8);
		sz=BGBBTJ_BT1D_EncodeBlocks(obuf, osz, hdr->data, ldat, xs, ys);
		bsz=8;
		break;
	case BGBBTJ_JPG_BGRA:
		BGBBTJ_BCn_EncodeImageDXT1A_BGRA(hdr->fpdata, rgba, xs, ys, 4);
		BGBBTJ_BCn_FlipImageDXT1(hdr->fpdata, hdr->data, xs, ys, 8);
		sz=BGBBTJ_BT1D_EncodeBlocks(obuf, osz, hdr->data, ldat, xs, ys);
		bsz=8;
		break;
	case BGBBTJ_JPG_BGR:
		BGBBTJ_BCn_EncodeImageDXT1F_BGRA(hdr->fpdata, rgba, xs, ys, 3);
		BGBBTJ_BCn_FlipImageDXT1(hdr->fpdata, hdr->data, xs, ys, 8);
		sz=BGBBTJ_BT1D_EncodeBlocks(obuf, osz, hdr->data, ldat, xs, ys);
		bsz=8;
		break;
	case BGBBTJ_JPG_RGBA_Q11_4:
		BGBBTJ_BCn_EncodeImageDXT5B(hdr->fpdata, hdr->rgba, xs1, ys1, 4);
		BGBBTJ_BCn_FlipImageDXT5(hdr->fpdata, hdr->data, xs1, ys1);
		BGBBTJ_BT1D_QuantizeBlocksDXT5(hdr->data, hdr->data, ldat,
			xs1, ys1, qf);
		sz=BGBBTJ_BT1D_EncodeBlocksDXT5(obuf, osz,
			hdr->data, ldat, xs1, ys1, 0);
		i=BGBBTJ_BT1D_EncodeToneMap(obuf+sz, osz-sz,
			hdr->tmm_rgba, hdr->tmn_rgba, xs1, ys1, qf, 4);
		if(i>0)sz+=i;
		bsz=16;
		break;
	default:
		break;
	}
#endif
	
#if 1
	if(!ldat)
	{
		ct=obuf+sz;

		ct0=ct;
		*ct++=0xE5;		*ct++=0x00;
		*ct++='H';		*ct++='D';

		*ct++=(xs>>8)&255;	*ct++=(xs>>0)&255;
		*ct++=(ys>>8)&255;	*ct++=(ys>>0)&255;
		*ct++=0x18;			*ct++=clrs;
		*ct++=0x00;			*ct++=0x00;
		*ct++=(qf>>24)&255;	*ct++=(qf>>16)&255;
		*ct++=(qf>>8)&255;	*ct++=(qf>>0)&255;
		
		ct0[1]=ct-ct0;
	
		sz=ct-obuf;
	}
#endif

#if 1
	if(!hdr->ldata)
	{
//		i=hdr->nblocks*16;
		i=hdr->nblocksmip*16;
		hdr->ldata=gcatomic(i);
		hdr->sz_ldata=i;
	}
//	memcpy(hdr->ldata, hdr->data, hdr->nblocks*bsz);
	memcpy(hdr->ldata, hdr->data, hdr->nblocksmip*16);
#endif

	return(sz);
}

byte *BGBBTJ_BT1D_EmitCompLayerHeader(
	BGBBTJ_BTIC1C_Context *ctx, BGBBTJ_BTIC1C_ImageInfo *img,
	byte *cts, byte *cte)
{
	byte *ct;
	char *s;
	
	ct=cts;
	*ct++=0xE5;		*ct++=0x00;
	*ct++='C';		*ct++='L';

	s=img->name;
	while(*s) { *ct++=*s++; }
	*ct++=0;
	*ct++=0;
	cts[1]=ct-cts;
	
	return(ct);
}

byte *BGBBTJ_BT1D_EmitTagLayerHeader(
	BGBBTJ_BTIC1C_Context *ctx, BGBBTJ_BTIC1C_LayerInfo *img,
	byte *cts, byte *cte)
{
	byte *ct;
	char *s;
	
	ct=cts;
	*ct++=0xE5;		*ct++=0x00;
	*ct++='T';		*ct++='L';

	s=img->name;
	while(*s) { *ct++=*s++; }
	*ct++=0;
	*ct++=0;
	cts[1]=ct-cts;
	
	return(ct);
}

BGBBTJ_API byte *BGBBTJ_BT1D_EmitLayerImage(
	BGBBTJ_BTIC1C_Context *ctx, BGBBTJ_BTIC1C_ImageInfo *img,
	byte *ctt, byte *cte, int qfl)
{
	byte *ldat;
	byte *ct;
	int i, bsz;

	ldat=NULL;
	if(qfl&BGBBTJ_QFL_PFRAME)
		ldat=img->ldata;

	ct=BGBBTJ_BT1D_EmitCompLayerHeader(ctx, img, ctt, cte);
	switch(img->imgtype)
	{
	case BGBBTJ_JPG_DXT1:
	case BGBBTJ_JPG_DXT1F:
	case BGBBTJ_JPG_DXT1A:
		i=BGBBTJ_BT1D_EncodeBlocks(ct, cte-ct, img->data, ldat,
			img->width, img->height, 0);
		ct+=i; bsz=8;
		break;
	case BGBBTJ_JPG_DXT5:
	case BGBBTJ_JPG_DXT5F:
		i=BGBBTJ_BT1D_EncodeBlocks(ct, cte-ct, img->data, ldat,
			img->width, img->height, 0);
		ct+=i; bsz=16;
		break;
	default:
		break;
	}

#if 1
	if(!img->ldata)
	{
		i=img->nblocks*16;
		img->ldata=gcatomic(i);
		img->sz_ldata=i;
	}
	memcpy(img->ldata, img->data, img->nblocks*bsz);
#endif
	
	return(ct);
//	i=BGBBTJ_BT1D_EncodeImage(img, norm, ct, cte-ct, xs, ys, qf, clrs);
}

BGBBTJ_API byte *BGBBTJ_BT1D_EmitTagLayer(
	BGBBTJ_BTIC1C_Context *ctx, BGBBTJ_BTIC1C_LayerInfo *layer,
	byte *ctt, byte *cte, int qfl)
{
	byte *ct;
	int i;
	
	ct=ctt;
	ct=BGBBTJ_BT1D_EmitTagLayerHeader(ctx, layer, ctt, cte);
	for(i=0; i<layer->n_image; i++)
	{
		ct=BGBBTJ_BT1D_EmitLayerImage(ctx,
			layer->image[i], ct, cte, qfl);
	}
	return(ct);
}
