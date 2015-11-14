#include <bgbbtj.h>

extern byte rpza_blkmap1[256];
extern byte rpza_blkmap2[256];
extern byte rpza_blkmap3[256];
extern byte rpza_blkmap4[256];

extern byte rpza_blkalphap[256];
extern byte rpza_blkalphamape1[4096];
extern byte rpza_blkalphamape2[4096];

BGBBTJ_API int BGBBTJ_RPZA_EncodeBlocksDXT1_LookupRun(
	byte *csc, byte *css, byte *cse,
	int *rlen, int *rdist, int fl)
{
	byte *cs, *cs1;
	int bi, bl, ml;
	int i, j, k;

	if(!(fl&BGBBTJ_BTIC1C_EFL_BLOCKLZ))
		{ *rlen=0; *rdist=0; return(0); }
		
	bi=0; bl=0; ml=256;
	cs=csc-8;
	while(cs>css)
	{
		i=(csc-cs)/8;
		if(i>=65536) { break; }
		else if(i>=4096) { ml=1; }
		else if(i>=1024) { ml=8; }
		else if(i>=256) { ml=32; }
		else if(i>=64) { ml=64; }
		else if(i>=16) { ml=128; }

		for(j=0; j<256; j++)
			if(memcmp(cs+j*8, csc+j*8, 8))
				break;
		if(j>bl) { bi=i; bl=j; }
		if(bl>=ml)break;
		cs-=8;
	}
	
	if(bl<16)
	{
//		if(bi>256)
//			return(0);
	}
	
	*rlen=bl;
	*rdist=bi;
	return(bl>0);
}

BGBBTJ_API int BGBBTJ_RPZA_EncodeBlocksDXT1(
	byte *obuf, int osz, byte *blks, byte *lblks,
	int xs, int ys, int fl)
{
	return(BGBBTJ_RPZA_EncodeBlocksDXTn(
		obuf, osz, blks, lblks, xs, ys, fl, 8));
}

BGBBTJ_API int BGBBTJ_RPZA_EncodeBlocksDXT5(
	byte *obuf, int osz, byte *blks, byte *lblks,
	int xs, int ys, int fl)
{
	int i, j;
	
	i=BGBBTJ_RPZA_EncodeBlocksDXTn(
		obuf, osz, blks+8, lblks?(lblks+8):NULL, xs, ys, fl, 16);
	j=BGBBTJ_RPZA_EncodeBlocksBC4(
		obuf+i, osz-i, blks, lblks, xs, ys,
		fl, 16);
//		fl|BGBBTJ_BTIC1C_EFL_BLOCKLZ, 16);
	return(i+j);
}

BGBBTJ_API int BGBBTJ_RPZA_EncodeBlocksDXTn(
	byte *obuf, int osz, byte *blks, byte *lblks,
	int xs, int ys, int fl, int stride)
{
	byte tb[16];
	byte *cs, *cse, *csm, *csl;
	byte *ct, *cte, *ctb;
	int xs1, ys1, n, n1;
	int xs2, ys2, xs3, ys3;
	int i0, i1, i2, i3;
	int bl, bd;
	int i, j, k, l;
	
	BGBBTJ_RPZA_Init();
	
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
	
	cs=blks; cse=cs+n*stride; csl=lblks;
	ct=obuf;
	
	*ct++=0xE1;		*ct++=0x00;
	*ct++=0x00;		*ct++=0x00;
	
	while(cs<cse)
	{
#if 1
		if(lblks && !memcmp(cs, csl, 8))
		{
			for(i=1; (i<32) && ((cs+i*stride)<cse); i++)
			{
				if(memcmp(cs+i*stride, csl+i*stride, 8))
					break;
			}

			*ct++=0x80+(i-1);
			cs+=i*stride; csl+=i*stride;
			continue;
		}
#endif

		i0=cs[0]|(cs[1]<<8);
		i1=cs[2]|(cs[3]<<8);

		if(BGBBTJ_RPZA_EncodeBlocksDXT1_LookupRun(
			cs, blks, cse, &bl, &bd, fl))
//		if((i0!=i1) && BGBBTJ_RPZA_EncodeBlocksDXT1_LookupRun(
//			cs, blks, cse, &bl, &bd, fl))
		{
			if((bl==1) && (bd==1))
				{ *ct++=0xE8; cs+=bl*stride; csl+=bl*stride; continue; }
			if((bl<=256) && (bd==1))
				{ *ct++=0xE9; *ct++=bl-1;
				cs+=bl*stride; csl+=bl*stride; continue; }
			if((bl==1) && (bd<=256))
				{ *ct++=0xEA; *ct++=bd-1;
				cs+=bl*stride; csl+=bl*stride; continue; }
			if((bl==1) && (bd<=65536))
			{
				j=bd-1; *ct++=0xEB;
				*ct++=(j>>8)&255; *ct++=j&255;
				cs+=bl*stride; csl+=bl*stride; continue;
			}
			if((bl<=256) && (bd<=256))
				{ *ct++=0xEC; *ct++=bl-1; *ct++=bd-1;
				cs+=bl*stride; csl+=bl*stride; continue; }
			if((bl<=256) && (bd<=65536))
			{
				*ct++=0xED; *ct++=bl-1;
				j=bd-1; *ct++=(j>>8)&255; *ct++=j&255;
				cs+=bl*stride; csl+=bl*stride; continue;
			}
		}

		if(i0<=i1)
		{
			//fully transparent blocks
			if(	(cs[4]==0xFF) && (cs[5]==0xFF) &&
				(cs[6]==0xFF) && (cs[7]==0xFF))
			{
				for(i=1; (i<32) && ((cs+i*stride)<cse); i++)
				{
					if(memcmp(cs+i*stride, cs, 8))
						break;
					//i2=cs[i*8*0]|(cs[i*8+1]<<8);
					//i3=cs[i*8+2]|(cs[i*8+3]<<8);
					//if((i2!=i0) || (i2!=i3))
					//	break;
				}
			
				*ct++=0xA0+(i-1);

				j=((i0&0xFFC0)>>1)|(i0&0x1F);
				j|=0x8000;
				ct[0]=(j>>8)&0xFF;	ct[1]=j&0xFF;
				ct+=2;
				cs+=i*stride; csl+=i*stride;
				continue;
			}
		}

#if 1
		if(i0==i1)
//		if(0)
		{
			for(i=0; (i<32) && ((cs+i*stride)<cse); i++)
			{
//				if(memcmp(cs+i*8, cs, 8))
//					break;
				i2=cs[i*stride+0]|(cs[i*stride+1]<<8);
				i3=cs[i*stride+2]|(cs[i*stride+3]<<8);
				if((i2!=i0) || (i2!=i3))
					break;
				if(	rpza_blkalphap[cs[i*stride+4]] |
					rpza_blkalphap[cs[i*stride+5]] |
					rpza_blkalphap[cs[i*stride+6]] |
					rpza_blkalphap[cs[i*stride+7]])
						break;
			}
			
			if(i>0)
			{
				*ct++=0xA0+(i-1);

				j=((i0&0xFFC0)>>1)|(i0&0x1F);
				ct[0]=(j>>8)&0xFF;	ct[1]=j&0xFF;
				ct+=2;
			
				cs+=i*stride; csl+=i*stride;
				continue;
			}
		}
#endif

		if(i0>i1)
//		if(1)
		{
			csm=rpza_blkmap3;
//			csm=rpza_blkmap4;

			for(i=1; (i<32) && ((cs+i*stride)<cse); i++)
			{
				i2=cs[i*stride+0]|(cs[i*stride+1]<<8);
				i3=cs[i*stride+2]|(cs[i*stride+3]<<8);
				if((i2!=i0) || (i3!=i1))
					break;
			}
			
			if(i>1)
//			if(0)
			{
				n1=i;
				*ct++=0xC0+(i-1);

				j=((i0&0xFFC0)>>1)|(i0&0x1F);
				k=((i1&0xFFC0)>>1)|(i1&0x1F);
		
				ct[0]=(j>>8)&0xFF;	ct[1]=j&0xFF;
				ct[2]=(k>>8)&0xFF;	ct[3]=k&0xFF;
				ct+=4;
				
				for(i=0; i<n1; i++)
				{
					ct[0]=csm[cs[4]]; ct[1]=csm[cs[5]];
					ct[2]=csm[cs[6]]; ct[3]=csm[cs[7]];
					cs+=stride; csl+=stride; ct+=4;
				}
				continue;
			}
			
			j=((i0&0xFFC0)>>1)|(i0&0x1F);
			k=((i1&0xFFC0)>>1)|(i1&0x1F);
			k|=0x8000;
		
			ct[0]=(j>>8)&0xFF;	ct[1]=j&0xFF;
			ct[2]=(k>>8)&0xFF;	ct[3]=k&0xFF;
			ct[4]=csm[cs[4]];	ct[5]=csm[cs[5]];
			ct[6]=csm[cs[6]];	ct[7]=csm[cs[7]];
			cs+=stride; csl+=stride; ct+=8;
			continue;
		}

		if(i0<=i1)
		{
			csm=rpza_blkmap4;
//			csm=rpza_blkmap3;

			for(i=1; (i<32) && ((cs+i*stride)<cse); i++)
			{
				i2=cs[i*stride+0]|(cs[i*stride+1]<<8);
				i3=cs[i*stride+2]|(cs[i*stride+3]<<8);
				if((i2!=i0) || (i3!=i1))
					break;
			}

#if 1
			if(i>1)
			{
				n1=i;
				*ct++=0xC0+(i-1);

				j=((i0&0xFFC0)>>1)|(i0&0x1F);
				k=((i1&0xFFC0)>>1)|(i1&0x1F);
				k|=0x8000;

				ct[0]=(j>>8)&0xFF;	ct[1]=j&0xFF;
				ct[2]=(k>>8)&0xFF;	ct[3]=k&0xFF;
				ct+=4;
				
				for(i=0; i<n1; i++)
				{
					ct[0]=csm[cs[4]]; ct[1]=csm[cs[5]];
					ct[2]=csm[cs[6]]; ct[3]=csm[cs[7]];
					cs+=stride; csl+=stride; ct+=4;
				}
				continue;
			}
#endif

			*ct++=0xC0;
			j=((i0&0xFFC0)>>1)|(i0&0x1F);
			k=((i1&0xFFC0)>>1)|(i1&0x1F);
			k|=0x8000;
		
			ct[0]=(j>>8)&0xFF;	ct[1]=j&0xFF;
			ct[2]=(k>>8)&0xFF;	ct[3]=k&0xFF;
			ct[4]=csm[cs[4]];	ct[5]=csm[cs[5]];
			ct[6]=csm[cs[6]];	ct[7]=csm[cs[7]];
			cs+=stride; csl+=stride; ct+=8;
			continue;
		}
	}

	i=ct-obuf;
	obuf[1]=(i>>16)&0xFF;
	obuf[2]=(i>>8)&0xFF;
	obuf[3]=(i>>0)&0xFF;

	return(i);
}

BGBBTJ_API int BGBBTJ_RPZA_EncodeBlocksBC4(
	byte *obuf, int osz, byte *blks, byte *lblks,
	int xs, int ys, int fl, int stride)
{
	byte tb[16];
	byte *cs, *cse, *csm, *csl;
	byte *ct, *cte, *ctb;
	int xs1, ys1, n, n1;
	int xs2, ys2, xs3, ys3;
	int i0, i1, i2, i3;
	int bl, bd;
	int i, j, k, l;
	
	BGBBTJ_RPZA_Init();
	
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
	
	cs=blks; cse=cs+n*stride; csl=lblks;
	ct=obuf;
	
	*ct++=0xE3;		*ct++=0x00;
	*ct++=0x00;		*ct++=0x00;
	*ct++='A';		*ct++='X';
	
	while(cs<cse)
	{
		if(lblks && !memcmp(cs, csl, 8))
		{
			for(i=1; (i<32) && ((cs+i*stride)<cse); i++)
			{
				if(memcmp(cs+i*stride, csl+i*stride, 8))
					break;
			}

			*ct++=0x80+(i-1);
			cs+=i*stride; csl+=i*stride;
			continue;
		}

		i0=cs[0];
		i1=cs[1];

		if(BGBBTJ_RPZA_EncodeBlocksDXT1_LookupRun(
			cs, blks, cse, &bl, &bd, fl))
		{
			if((bl==1) && (bd==1))
				{ *ct++=0xE8; cs+=bl*stride; csl+=bl*stride; continue; }
			if((bl<=256) && (bd==1))
				{ *ct++=0xE9; *ct++=bl-1;
				cs+=bl*stride; csl+=bl*stride; continue; }
			if((bl==1) && (bd<=256))
				{ *ct++=0xEA; *ct++=bd-1;
				cs+=bl*stride; csl+=bl*stride; continue; }
			if((bl==1) && (bd<=65536))
			{
				j=bd-1; *ct++=0xEB;
				*ct++=(j>>8)&255; *ct++=j&255;
				cs+=bl*stride; csl+=bl*stride; continue;
			}
			if((bl<=256) && (bd<=256))
				{ *ct++=0xEC; *ct++=bl-1; *ct++=bd-1;
				cs+=bl*stride; csl+=bl*stride; continue; }
			if((bl<=256) && (bd<=65536))
			{
				*ct++=0xED; *ct++=bl-1;
				j=bd-1; *ct++=(j>>8)&255; *ct++=j&255;
				cs+=bl*stride; csl+=bl*stride; continue;
			}
		}

		if(i0==i1)
		{
			for(i=0; (i<32) && ((cs+i*stride)<cse); i++)
			{
				if(memcmp(cs+i*stride, cs, 8))
					break;
			}
			
			if(i>0)
			{
				*ct++=0xA0+(i-1);

//				j=((i0&0xFFC0)>>1)|(i0&0x1F);
				j=i0;
				*ct++=j>>1;

//				ct[0]=(j>>8)&0xFF;	ct[1]=j&0xFF;
//				ct+=2;
			
				cs+=i*stride; csl+=i*stride;
				continue;
			}
		}

//		if(i0>=i1)
		if(1)
		{
			if(i0>i1)
				{ csm=rpza_blkalphamape2; }
			else
				{ csm=rpza_blkalphamape1; }

			for(i=1; (i<32) && ((cs+i*stride)<cse); i++)
			{
				i2=cs[i*stride+0];
				i3=cs[i*stride+1];
				if((i2!=i0) || (i3!=i1))
					break;
			}
			
			if(i>1)
//			if(0)
			{
				n1=i;
				*ct++=0xC0+(i-1);

				j=i0; k=i1;
//				ct[0]=(j>>8)&0xFF;	ct[1]=j&0xFF;
//				ct[2]=(k>>8)&0xFF;	ct[3]=k&0xFF;
//				ct+=4;

				ct[0]=(j>>1)&0x7F;
				ct[1]=(k>>1)&0x7F;
				ct+=2;

//				ct[0]=(j>>8)&0xFF;	ct[1]=j&0xFF; ct[2]=k;
//				ct+=3;
				
				for(i=0; i<n1; i++)
				{
					j=cs[2]|(cs[3]<<8)|(cs[4]<<16);
					k=cs[5]|(cs[6]<<8)|(cs[7]<<16);
					ct[0]=csm[j&0xFFF]; ct[1]=csm[(j>>12)&0xFFF];
					ct[2]=csm[k&0xFFF]; ct[3]=csm[(k>>12)&0xFFF];
					cs+=stride; csl+=stride; ct+=4;
				}
				continue;
			}
			
			j=i0; k=i1;
//			k|=0x8000;
		
//			ct[0]=(j>>8)&0xFF;	ct[1]=j&0xFF;
//			ct[2]=(k>>8)&0xFF;	ct[3]=k&0xFF;
//			ct[0]=(j>>8)&0xFF;	ct[1]=j&0xFF; ct[2]=k;
//			ct+=3;

			ct[0]=(j>>1)&0x7F;
			ct[1]=(k>>1)&0x7F;
			ct+=2;

			j=cs[2]|(cs[3]<<8)|(cs[4]<<16);
			k=cs[5]|(cs[6]<<8)|(cs[7]<<16);
			ct[0]=csm[j&0xFFF];	ct[1]=csm[(j>>12)&0xFFF];
			ct[2]=csm[k&0xFFF];	ct[3]=csm[(k>>12)&0xFFF];
			cs+=stride; csl+=stride; ct+=4;
			continue;
		}
	}

	i=ct-obuf;
	obuf[1]=(i>>16)&0xFF;
	obuf[2]=(i>>8)&0xFF;
	obuf[3]=(i>>0)&0xFF;

	return(i);
}

#if 0
// BGBBTJ_API BGBBTJ_BTIC1C_Context *BGBBTJ_RPZA_AllocContext()
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
#endif

BGBBTJ_API int BGBBTJ_RPZA_EncodeImageFast(
	BGBBTJ_BTIC1C_Context *ctx, byte *rgba, byte *obuf, int osz,
	int xs, int ys, int qf, int clrs)
{
	return(BGBBTJ_RPZA_EncodeImage(
		ctx, rgba, obuf, osz, xs, ys, qf, clrs));
}

BGBBTJ_API int BGBBTJ_RPZA_EncodeImage(
	BGBBTJ_BTIC1C_Context *ctx, byte *rgba, byte *obuf, int osz,
	int xs, int ys, int qf, int clrs)
{
	BGBBTJ_BTIC1C_ImageInfo *hdr;
	int sz;

	hdr=ctx->image[0];
	
	sz=BGBBTJ_RPZA_EncodeImage2(ctx, hdr,
		rgba, obuf, osz, xs, ys, qf, clrs);
	return(sz);
}

BGBBTJ_API int BGBBTJ_RPZA_EncodeImage2(
	BGBBTJ_BTIC1C_Context *ctx, BGBBTJ_BTIC1C_ImageInfo *hdr,
	byte *rgba, byte *obuf, int osz,
	int xs, int ys, int qf, int clrs)
{
	s16 *rgba2;
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

	if(!hdr->rgba)
	{
//		i=(4*hdr->width2)*(4*hdr->height2)*4;
//		if(qf&BGBBTJ_QFL_MIPMAP)i*=2;
		i=hdr->nblocksmip*16*4;
		hdr->rgba=gcatomic(i);
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
		
//		BGBBTJ_Tex_Dither15(hdr->rgba, hdr->rgba, xs1, ys1);
	}
#endif

#if 0
	if(clrs==BGBBTJ_JPG_RGBA)
	{
		for(i=0; i<ys; i++)
			for(j=0; j<xs; j++)
		{
			BGBBTJ_BlendMagentaEncodeColor(
				rgba+((i*xs+j)*4),
				hdr->rgba+((i*xs1+j)*4));
		
#if 0
			cr=rgba[(i*xs+j)*4+0];		cg=rgba[(i*xs+j)*4+1];
			cb=rgba[(i*xs+j)*4+2];		ca=rgba[(i*xs+j)*4+3];
		
			l=255-ca;
			k=255-((l*l)>>8);
//			k=(ca*ca)>>8;
		
//			if(ca<224)
//			if(k<224)
			if(k<252)
			{
//				cr=((cr*k)+(0*(256-k)))>>8;
//				cg=((cg*k)+(255*(256-k)))>>8;
//				cb=((cb*k)+(255*(256-k)))>>8;

				cr=((cr*k)+(255*(256-k)))>>8;
				cg=((cg*k)+(0*(256-k)))>>8;
				cb=((cb*k)+(255*(256-k)))>>8;

#if 0
				if(((i>>6)^(j>>6))&1)
				{
					cr=((cr*k)+(255*(256-k)))>>8;
					cg=((cg*k)+(0*(256-k)))>>8;
					cb=((cb*k)+(255*(256-k)))>>8;
				}else
				{
					cr=((cr*k)+(0*(256-k)))>>8;
					cg=((cg*k)+(255*(256-k)))>>8;
					cb=((cb*k)+(255*(256-k)))>>8;
				}
#endif
			}

			hdr->rgba[(i*xs+j)*4+0]=cr;
			hdr->rgba[(i*xs+j)*4+1]=cg;
			hdr->rgba[(i*xs+j)*4+2]=cb;
			hdr->rgba[(i*xs+j)*4+3]=ca;
#endif
		}
	}
#endif

#if 1
	if(clrs==BGBBTJ_JPG_RGBA_Q11_4)
	{
		if(!hdr->tmm_rgba)
		{
//			i=(4*hdr->width2)*(4*hdr->height2)*4;
//			if(qf&BGBBTJ_QFL_MIPMAP)i*=2;
			i=hdr->nblocksmip*4*sizeof(s16);
			hdr->tmm_rgba=gcatomic(i);
			hdr->tmn_rgba=gcatomic(i);
			hdr->sz_tmrgba=i;
		}

		BGBBTJ_SplitHdrBlockToneMap_FixedQ3_11(
			(s16 *)rgba, hdr->tmm_rgba, hdr->tmn_rgba,
			hdr->rgba, xs, ys, qf);
	}
#endif

#if 1
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

//	ldat=hdr->ldata;
//	if(qf&BGBBTJ_QFL_IFRAME)
//		ldat=NULL;

	ldat=NULL;
	if(qf&BGBBTJ_QFL_PFRAME)
		ldat=hdr->ldata;

	switch(clrs)
	{
	case BGBBTJ_JPG_RGBA:
		if(qf&BGBBTJ_QFL_MIPMAP)
		{
			BGBBTJ_BCn_EncodeImageDXT5B_Mip(hdr->fpdata,
				hdr->rgba, xs1, ys1, 4);
			BGBBTJ_BCn_FlipImageDXT5_Mip(hdr->fpdata, hdr->data, xs1, ys1);
			BGBBTJ_RPZA_QuantizeBlocksDXT5(hdr->data, hdr->data, ldat,
				xs1, ys1, qf);
//			sz=BGBBTJ_RPZA_EncodeBlocksDXT5(obuf, osz,
//				hdr->data, ldat, xs, ys);
			sz=BGBBTJ_RPZA_EncodeBlocksDXT5(obuf, osz,
				hdr->data, ldat, xs1, ys1, BGBBTJ_BTIC1C_EFL_MIPMAP);
			bsz=16;
		}else
		{
#if 0
			BGBBTJ_BCn_EncodeImageDXT1A(hdr->fpdata, rgba, xs, ys, 4);
//			BGBBTJ_BCn_EncodeImageDXT1F(hdr->fpdata, rgba, xs, ys, 4);
			BGBBTJ_BCn_FlipImageDXT1(hdr->fpdata, hdr->data, xs, ys, 8);
			sz=BGBBTJ_RPZA_EncodeBlocksDXT1(obuf, osz,
				hdr->data, ldat, xs, ys);
			bsz=8;
#endif
#if 1
			BGBBTJ_BCn_EncodeImageDXT5B(hdr->fpdata, hdr->rgba, xs1, ys1, 4);
//			BGBBTJ_BCn_EncodeImageDXT5(hdr->fpdata, hdr->rgba, xs, ys, 4);
//			BGBBTJ_BCn_EncodeImageDXT5(hdr->fpdata, rgba, xs, ys, 4);
//			BGBBTJ_BCn_EncodeImageDXT1F(hdr->fpdata, rgba, xs, ys, 4);
			BGBBTJ_BCn_FlipImageDXT5(hdr->fpdata, hdr->data, xs1, ys1);
			BGBBTJ_RPZA_QuantizeBlocksDXT5(hdr->data, hdr->data, ldat,
				xs1, ys1, qf);
//			sz=BGBBTJ_RPZA_EncodeBlocksDXT5(obuf, osz,
//				hdr->data, ldat, xs, ys);
			sz=BGBBTJ_RPZA_EncodeBlocksDXT5(obuf, osz,
				hdr->data, ldat, xs1, ys1, 0);
			bsz=16;
#endif
		}
		break;
	case BGBBTJ_JPG_RGB:
		BGBBTJ_BCn_EncodeImageDXT1A(hdr->fpdata, rgba, xs, ys, 3);
//		BGBBTJ_BCn_EncodeImageDXT1F(hdr->fpdata, rgba, xs, ys, 3);
		BGBBTJ_BCn_FlipImageDXT1(hdr->fpdata, hdr->data, xs, ys, 8);
		sz=BGBBTJ_RPZA_EncodeBlocksDXT1(obuf, osz,
			hdr->data, ldat, xs, ys, 0);
		bsz=8;
		break;
	case BGBBTJ_JPG_BGRA:
		BGBBTJ_BCn_EncodeImageDXT1A_BGRA(hdr->fpdata, rgba, xs, ys, 4);
		BGBBTJ_BCn_FlipImageDXT1(hdr->fpdata, hdr->data, xs, ys, 8);
		sz=BGBBTJ_RPZA_EncodeBlocksDXT1(obuf, osz,
			hdr->data, ldat, xs, ys, 0);
		bsz=8;
		break;
	case BGBBTJ_JPG_BGR:
//		BGBBTJ_BCn_EncodeImageDXT1F_BGRA(hdr->fpdata, rgba, xs, ys, 3);
		BGBBTJ_BCn_EncodeImageDXT1A_BGRA(hdr->fpdata, rgba, xs, ys, 3);
		BGBBTJ_BCn_FlipImageDXT1(hdr->fpdata, hdr->data, xs, ys, 8);
		sz=BGBBTJ_RPZA_EncodeBlocksDXT1(obuf, osz,
			hdr->data, ldat, xs, ys, 0);
		bsz=8;
		break;
#if 0
	case BGBBTJ_JPG_RGBA_Q11_4:
		BGBBTJ_BCn_EncodeImageDXT5B(hdr->fpdata, hdr->rgba, xs1, ys1, 4);
		BGBBTJ_BCn_FlipImageDXT5(hdr->fpdata, hdr->data, xs1, ys1);
		BGBBTJ_RPZA_QuantizeBlocksDXT5(hdr->data, hdr->data, ldat,
			xs1, ys1, qf);
		sz=BGBBTJ_RPZA_EncodeBlocksDXT5(obuf, osz,
			hdr->data, ldat, xs1, ys1, 0);
//		i=BGBBTJ_RPZA_EncodeToneMap(obuf+sz, osz-sz,
//			hdr->tmm_rgba, hdr->tmn_rgba, xs1, ys1, qf, 4);
		if(i>0)sz+=i;
		bsz=16;
		break;
#endif

	default:
		break;
	}
	
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
	memcpy(hdr->ldata, hdr->data, hdr->nblocksmip*bsz);
#endif

	return(sz);
}

byte *BGBBTJ_RPZA_EmitCompLayerHeader(
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

byte *BGBBTJ_RPZA_EmitTagLayerHeader(
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

BGBBTJ_API byte *BGBBTJ_RPZA_EmitLayerImage(
	BGBBTJ_BTIC1C_Context *ctx, BGBBTJ_BTIC1C_ImageInfo *img,
	byte *ctt, byte *cte, int qfl)
{
	byte *ldat;
	byte *ct;
	int i, bsz;

	ldat=NULL;
	if(qfl&BGBBTJ_QFL_PFRAME)
		ldat=img->ldata;

	ct=BGBBTJ_RPZA_EmitCompLayerHeader(ctx, img, ctt, cte);
	switch(img->imgtype)
	{
	case BGBBTJ_JPG_DXT1:
	case BGBBTJ_JPG_DXT1F:
	case BGBBTJ_JPG_DXT1A:
		i=BGBBTJ_RPZA_EncodeBlocksDXT1(ct, cte-ct, img->data, ldat,
			img->width, img->height, 0);
		ct+=i; bsz=8;
		break;
	case BGBBTJ_JPG_DXT5:
	case BGBBTJ_JPG_DXT5F:
		i=BGBBTJ_RPZA_EncodeBlocksDXT5(ct, cte-ct, img->data, ldat,
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
//	i=BGBBTJ_RPZA_EncodeImage(img, norm, ct, cte-ct, xs, ys, qf, clrs);
}

BGBBTJ_API byte *BGBBTJ_RPZA_EmitTagLayer(
	BGBBTJ_BTIC1C_Context *ctx, BGBBTJ_BTIC1C_LayerInfo *layer,
	byte *ctt, byte *cte, int qfl)
{
	byte *ct;
	int i;
	
	ct=ctt;
	ct=BGBBTJ_RPZA_EmitTagLayerHeader(ctx, layer, ctt, cte);
	for(i=0; i<layer->n_image; i++)
	{
		ct=BGBBTJ_RPZA_EmitLayerImage(ctx,
			layer->image[i], ct, cte, qfl);
	}
	return(ct);
}
