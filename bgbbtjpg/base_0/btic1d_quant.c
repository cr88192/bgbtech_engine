#include <bgbbtj.h>

BGBBTJ_API int BGBBTJ_BT1DQ_DecodeBlock(
	byte *iblk, short *orgba)
{
//	BGBBTJ_BCn_DecodeBlockDXT1(
//		iblk, orgba, 4, 16, 4);
//	BGBBTJ_BT1D_DecodeBlockBX1A_Q11_4(
//		iblk, orgba, 4, 16, 4);
//	BGBBTJ_BT1D_DecodeBlockBX1A_LS16(
//		iblk, orgba, 4, 16, 4);
	BGBBTJ_BT1D_DecodeBlockBX1A_S16(
		iblk, orgba, 4, 16);
	return(0);
}

BGBBTJ_API int BGBBTJ_BT1DQ_EncodeBlock(
	byte *oblk, short *rgba)
{
	BGBBTJ_BT1D_EncodeBlockBX1A_S16(oblk, rgba, 4, 4*4);
	return(0);
}

BGBBTJ_API int BGBBTJ_BT1DQ_EncodeBlockHalf(
	byte *oblk, short *rgba)
{
	BGBBTJ_BT1D_EncodeHalfBlockBX1A_S16(oblk, rgba, 4, 4*4);
	return(0);
}

BGBBTJ_API int BGBBTJ_BT1DQ_CalcColorRMSE(
	short *rgb0, short *rgb1)
{
//	int y0, y1, dy;
	int dr, dg, db, da;
	int d;

//	y0=(rgb0[0]+2*rgb0[1]+rgb0[2])>>2;
//	y1=(rgb1[0]+2*rgb1[1]+rgb1[2])>>2;
	dr=rgb0[0]-rgb1[0];
	dg=rgb0[1]-rgb1[1];
	db=rgb0[2]-rgb1[2];
	da=rgb0[3]-rgb1[3];
//	dy=y0-y1;
	
//	d=(dr*dr)+(dg*dg)+(db*db)+(da*da)+(dy*dy);
	d=(dr*dr)+2*(dg*dg)+(db*db)+(da*da);
//	d=d/5;
	return(d);
}

BGBBTJ_API double BGBBTJ_BT1DQ_CalcBlockRMSE(
	byte *blka, byte *blkb)
{
	short rgba0[16*4];
	short rgba1[16*4];
	int e;
	int i;
	
	if((blkb[0]==0x55) && (blkb[1]==0xAA))
	{
		e=16*65536;
		return(e);
	}
	
	BGBBTJ_BT1DQ_DecodeBlock(blka, rgba0);
	BGBBTJ_BT1DQ_DecodeBlock(blkb, rgba1);
	
	e=0;
	for(i=0; i<16; i++)
	{
		e+=BGBBTJ_BT1DQ_CalcColorRMSE(rgba0+i*4, rgba1+i*4);
	}
//	e=sqrt(e/(5*16));
	e=sqrt(e/5.0);
	return(e);
}

BGBBTJ_API void BGBBTJ_BT1DQ_SetDWordBE(
	byte *blk, int v)
{
	blk[0]=(v>>24)&255;
	blk[1]=(v>>16)&255;
	blk[2]=(v>> 8)&255;
	blk[3]=(v>> 0)&255;
}

BGBBTJ_API void BGBBTJ_BT1DQ_CalcBlockAsFlat(
	byte *blka, byte *blkb)
{
	short rgba0[16*4];
	short rgba1[16*4];
	int cr, cg, cb, ca;
	int i0, i1, i2, i3;
	int j0, j1, j2, j3;
	int bm;
	int i, j;

	i0=(blka[ 0]<<24)|(blka[ 1]<<16)|(blka[ 2]<<8)|(blka[ 3]);
	i1=(blka[ 4]<<24)|(blka[ 5]<<16)|(blka[ 6]<<8)|(blka[ 7]);
	i2=(blka[ 8]<<24)|(blka[ 9]<<16)|(blka[10]<<8)|(blka[11]);
	i3=(blka[12]<<24)|(blka[13]<<16)|(blka[14]<<8)|(blka[15]);

	j0=(blka[ 8]<<8)|(blka[ 9]);
	j1=(blka[10]<<8)|(blka[11]);
	j2=(blka[12]<<8)|(blka[13]);
	j3=(blka[14]<<8)|(blka[15]);

	bm=(((i0>>29)&7)<<3)|((i1>>29)&3);

	if((bm>=48) && (bm<56))
	{
		memcpy(blkb, blka, 16);
		return;
	}

#if 1
	if((i0&0x1FFFFFFF)==(i1&0x1FFFFFFF))
	{
		if((bm==0) || (bm==1))
		{
			BGBBTJ_BT1DQ_SetDWordBE(blkb+ 0, 0xC0000000);
			BGBBTJ_BT1DQ_SetDWordBE(blkb+ 4, i1);
			BGBBTJ_BT1DQ_SetDWordBE(blkb+ 8, 0);
			BGBBTJ_BT1DQ_SetDWordBE(blkb+12, 0);
			return;
		}
		
		if(bm==2)
		{
			j=((j0&15)+(j1&15)+(j2&15)+(j3&15)+2)>>1;
					
			i2=	(2<<29)|
				(j<<24)|
				((i0&0x1FE00000)>>5)|
				((i0&0x0003FC00)>>2)|
				((i0&0x000001FE)>>1);
			BGBBTJ_BT1DQ_SetDWordBE(blkb+ 0, 0xC0000000);
			BGBBTJ_BT1DQ_SetDWordBE(blkb+ 4, i2);
			BGBBTJ_BT1DQ_SetDWordBE(blkb+ 8, 0);
			BGBBTJ_BT1DQ_SetDWordBE(blkb+12, 0);
			return;
		}

		if(bm==3)
		{
			j=((j0&15)+(j1&15)+(j2&15)+(j3&15)+2)>>1;
					
			i2=	(i0&0xFFFE0000)|
				((i0&0x0000FC00)<<1)|
				((i0&0x000000FC)<<3)|
				j;
			BGBBTJ_BT1DQ_SetDWordBE(blkb+ 0, 0xC0000000);
			BGBBTJ_BT1DQ_SetDWordBE(blkb+ 4, i2);
			BGBBTJ_BT1DQ_SetDWordBE(blkb+ 8, 0);
			BGBBTJ_BT1DQ_SetDWordBE(blkb+12, 0);
			return;
		}
	}
#endif

//	if((blka[0]==blka[4]) && (blka[1]==blka[5]) && (blka[1]==blka[5]))
//	{
//		memcpy(blkb, blka, 8);
//		return;
//	}

	BGBBTJ_BT1DQ_DecodeBlock(blka, rgba0);
	
	cr=0; cg=0; cb=0; ca=0;
	for(i=0; i<16; i++)
	{
		cr+=rgba0[i*4+0];	cg+=rgba0[i*4+1];
		cb+=rgba0[i*4+2];	ca+=rgba0[i*4+3];
	}
	
	cr=cr/16;	cg=cg/16;
	cb=cb/16;	ca=ca/16;

//	BGBBTJ_BT1D_EncodeFlatBlockBX1A(blkb, cr, cg, cb, ca);
	BGBBTJ_BT1D_EncodeFlatBlockBX1A_S16(blkb, cr, cg, cb, ca);

#if 0
	for(i=0; i<16; i++)
	{
		rgba1[i*4+0]=cr;
		rgba1[i*4+1]=cg;
		rgba1[i*4+2]=cb;
		rgba1[i*4+3]=ca;
	}
	
	BGBBTJ_BT1DQ_EncodeBlock(blkb, rgba1);
#endif

#if 0
	if(ca<128)
	{
//		i=((cr<<8)&0xF800) | ((cg<<3)&0x07E0) | ((cb>>3)&0x001F);
//		blkb[0]=i;		blkb[1]=i>>8;
//		blkb[2]=i;		blkb[3]=i>>8;
//		blkb[4]=255;	blkb[5]=255;
//		blkb[6]=255;	blkb[7]=255;
	}else
	{
//		i=((cr<<8)&0xF800) | ((cg<<3)&0x07E0) | ((cb>>3)&0x001F);
//		blkb[0]=i;	blkb[1]=i>>8;
//		blkb[2]=i;	blkb[3]=i>>8;
//		blkb[4]=0;	blkb[5]=0;
//		blkb[6]=0;	blkb[7]=0;
	}
#endif
}

BGBBTJ_API void BGBBTJ_BT1DQ_CalcBlockAsHalf(
	byte *blka, byte *blkb)
{
	short rgba0[16*4];
	short rgba1[16*4];

	BGBBTJ_BT1DQ_DecodeBlock(blka, rgba0);
	BGBBTJ_BT1DQ_EncodeBlockHalf(blkb, rgba0);
}

BGBBTJ_API void BGBBTJ_BT1DQ_CalcBlockAsFlatHalf(
	byte *blka, byte *blkb)
{
	short rgba0[16*4];
	short rgba1[16*4];
	int cr, cg, cb, ca;
	int i0, i1, i2, i3;
	int j0, j1, j2, j3;
	int bm;
	int i, j;

	i0=(blka[ 0]<<24)|(blka[ 1]<<16)|(blka[ 2]<<8)|(blka[ 3]);
	i1=(blka[ 4]<<24)|(blka[ 5]<<16)|(blka[ 6]<<8)|(blka[ 7]);
	i2=(blka[ 8]<<24)|(blka[ 9]<<16)|(blka[10]<<8)|(blka[11]);
	i3=(blka[12]<<24)|(blka[13]<<16)|(blka[14]<<8)|(blka[15]);

	j0=(blka[ 8]<<8)|(blka[ 9]);
	j1=(blka[10]<<8)|(blka[11]);
	j2=(blka[12]<<8)|(blka[13]);
	j3=(blka[14]<<8)|(blka[15]);

	bm=(((i0>>29)&7)<<3)|((i1>>29)&3);

//	if((bm>=48) && (bm<56))
	if((bm>=52) && (bm<56))
	{
		memcpy(blkb, blka, 16);
		return;
	}

	BGBBTJ_BT1DQ_DecodeBlock(blka, rgba0);
	
	cr=0; cg=0; cb=0; ca=0;
	for(i=0; i<16; i++)
	{
		cr+=rgba0[i*4+0];	cg+=rgba0[i*4+1];
		cb+=rgba0[i*4+2];	ca+=rgba0[i*4+3];
	}
	
	cr=cr/16;	cg=cg/16;
	cb=cb/16;	ca=ca/16;

//	BGBBTJ_BT1D_EncodeFlatBlockBX1A(blkb, cr, cg, cb, ca);
//	BGBBTJ_BT1D_EncodeFlatBlockBX1A_S16(blkb, cr, cg, cb, ca);
	BGBBTJ_BT1D_EncodeFlatHalfBlockBX1A_S16(blkb, cr, cg, cb, ca);
}

BGBBTJ_API void BGBBTJ_BT1DQ_CalcBlockAsPriorColor(
	byte *blka, byte *blkb, byte *blkc)
{
	blkc[ 0]=blka[ 0];	blkc[ 1]=blka[ 1];
	blkc[ 2]=blka[ 2];	blkc[ 3]=blka[ 3];
	blkc[ 4]=blka[ 4];	blkc[ 5]=blka[ 5];
	blkc[ 6]=blka[ 6];	blkc[ 7]=blka[ 7];
	blkc[ 8]=blkb[ 8];	blkc[ 9]=blkb[ 9];
	blkc[10]=blkb[10];	blkc[11]=blkb[11];
	blkc[12]=blkb[12];	blkc[13]=blkb[13];
	blkc[14]=blkb[14];	blkc[15]=blkb[15];
}

BGBBTJ_API void BGBBTJ_BT1DQ_CalcBlockAsPriorColorHalf(
	byte *blka, byte *blkb, byte *blkc)
{
	byte tblk0[16], tblk1[16];
	short rgba0[16*4];
	short rgba1[16*4];

	BGBBTJ_BT1DQ_DecodeBlock(blka, rgba0);
	BGBBTJ_BT1DQ_DecodeBlock(blkb, rgba1);
	BGBBTJ_BT1DQ_EncodeBlockHalf(tblk0, rgba0);
	BGBBTJ_BT1DQ_EncodeBlockHalf(tblk1, rgba1);

	blkc[ 0]=tblk0[ 0];	blkc[ 1]=tblk0[ 1];
	blkc[ 2]=tblk0[ 2];	blkc[ 3]=tblk0[ 3];
	blkc[ 4]=tblk0[ 4];	blkc[ 5]=tblk0[ 5];
	blkc[ 6]=tblk0[ 6];	blkc[ 7]=tblk0[ 7];
	blkc[ 8]=tblk1[ 8];	blkc[ 9]=tblk1[ 9];
	blkc[10]=tblk1[10];	blkc[11]=tblk1[11];
	blkc[12]=tblk1[12];	blkc[13]=tblk1[13];
	blkc[14]=tblk1[14];	blkc[15]=tblk1[15];
}

BGBBTJ_API void BGBBTJ_BT1DQ_CalcBlockAsPriorFlatColor(
	byte *blka, byte *blkb, byte *blkc)
{
	BGBBTJ_BT1DQ_CalcBlockAsFlat(blka, blkc);

//	blkc[0]=blka[0];	blkc[1]=blka[1];
//	blkc[2]=blka[0];	blkc[3]=blka[1];
//	blkc[4]=blkb[4];	blkc[5]=blkb[5];
//	blkc[6]=blkb[6];	blkc[7]=blkb[7];
}

BGBBTJ_API void BGBBTJ_BT1DQ_CalcBlockAsPriorFlatColorHalf(
	byte *blka, byte *blkb, byte *blkc)
{
	BGBBTJ_BT1DQ_CalcBlockAsFlatHalf(blka, blkc);
}

BGBBTJ_API void BGBBTJ_BT1DQ_ThrashBlock(byte *blk)
{
	blk[0]=0x55; blk[1]=0xAA;
	blk[2]=0xAA; blk[3]=0x55;
	blk[4]=0x55; blk[5]=0xAA;
	blk[6]=0xAA; blk[7]=0x55;
}

BGBBTJ_API int BGBBTJ_BT1D_QuantizeBlocksN(
	byte *oblks, byte *blks, byte *lblks,
	int xs, int ys, int qfl, int stride)
{
	byte blkwin[256*16];
	byte tblk[16], tblk2[16], tlblk[16];
	byte *cblk, *clblk, *coblk, *cwblk, *cwblk2;
	double e;
	double rmse;
	byte blkrov;
	int xs1, ys1;
	int xs2, ys2;
	int xs3, ys3;
	int i, j, k, l, n;
	
	rmse=100-(qfl&255);
//	rmse=(rmse*rmse)/100;
//	rmse=(rmse*rmse)/80.0;
//	rmse=pow(rmse, 1.5)/10.0;
	rmse=pow(rmse, 1.5)/20.0;
//	rmse=1;
	blkrov=0;
	
	if(rmse<4.0)
//	if(1)
	{
		if(oblks!=blks)
		{
			xs1=(xs+3)>>2; ys1=(ys+3)>>2;
			for(i=0; i<ys1; i++)
				for(j=0; j<xs1; j++)
			{
				cblk=blks+(i*xs1+j)*stride;
				coblk=oblks+(i*xs1+j)*stride;
				memcpy(coblk, cblk, 16);
			}
		}
		return(0);
	}
	
	for(i=0; i<256; i++)
		{ BGBBTJ_BT1DQ_ThrashBlock(blkwin+i*16); }
	
	BGBBTJ_BT1DQ_ThrashBlock(tlblk);
	xs1=(xs+3)>>2; ys1=(ys+3)>>2;
	n=xs1*ys1;
	
	if(qfl&BGBBTJ_QFL_MIPMAP)
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
	}

//	for(i=0; i<ys1; i++)
//		for(j=0; j<xs1; j++)
	for(i=0; i<n; i++)
	{
//		cblk=blks+(i*xs1+j)*stride;
//		coblk=oblks+(i*xs1+j)*stride;
		cblk=blks+i*stride;
		coblk=oblks+i*stride;

		//is block sufficiently similar to last-frame block?
		if(lblks)
		{
//			clblk=lblks+(i*xs1+j)*stride;
			clblk=lblks+i*stride;

			e=BGBBTJ_BT1DQ_CalcBlockRMSE(cblk, clblk);
			if(e<=rmse)
			{
				BGBBTJ_BT1DQ_ThrashBlock(tlblk);
				memcpy(coblk, clblk, 16);
				continue;
			}
		}

#if 1
		//can block be replaced with a flat color? (Half-Block)
		BGBBTJ_BT1DQ_CalcBlockAsFlatHalf(cblk, tblk);
		e=BGBBTJ_BT1DQ_CalcBlockRMSE(cblk, tblk);
		if(e<=rmse)
//		if(1)
		{
#if 1
			//can we reuse last block's color (Half-Block)
			BGBBTJ_BT1DQ_CalcBlockAsPriorFlatColorHalf(tlblk, tblk, tblk2);
			e=BGBBTJ_BT1DQ_CalcBlockRMSE(cblk, tblk2);
			if(e<=rmse)
			{
				memcpy(coblk, tblk2, 16);
				continue;
			}
#endif

//			BGBBTJ_BT1DQ_ThrashBlock(tlblk);
			memcpy(tlblk, tblk, 16);
			memcpy(coblk, tblk, 16);
			continue;
		}
#endif

#if 1
		//can block be replaced with a flat color?
		BGBBTJ_BT1DQ_CalcBlockAsFlat(cblk, tblk);
		e=BGBBTJ_BT1DQ_CalcBlockRMSE(cblk, tblk);
		if(e<=rmse)
		{
			//can we reuse last block's color
			BGBBTJ_BT1DQ_CalcBlockAsPriorFlatColor(tlblk, tblk, tblk2);
			e=BGBBTJ_BT1DQ_CalcBlockRMSE(cblk, tblk2);
			if(e<=rmse)
			{
				memcpy(coblk, tblk2, 16);
				continue;
			}

//			BGBBTJ_BT1DQ_ThrashBlock(tlblk);
			memcpy(tlblk, tblk, 16);
			memcpy(coblk, tblk, 16);
			continue;
		}
#endif

#if 1
		//can we reuse last block's color (half-block)
		BGBBTJ_BT1DQ_CalcBlockAsPriorColorHalf(tlblk, cblk, tblk);
		e=BGBBTJ_BT1DQ_CalcBlockRMSE(cblk, tblk);
		if(e<=rmse)
		{
			memcpy(coblk, tblk, 16);
			continue;
		}
#endif

#if 1
		//can we reuse last block's color
		BGBBTJ_BT1DQ_CalcBlockAsPriorColor(tlblk, cblk, tblk);
		e=BGBBTJ_BT1DQ_CalcBlockRMSE(cblk, tblk);
		if(e<=rmse)
		{
			memcpy(coblk, tblk, 16);
			continue;
		}
#endif
		
		//can we replace the block with another recently seen block?
		if(qfl&BGBBTJ_QFL_BLOCKINDEX)
//		if(1)
		{
//			for(k=0; k<256; k++)
			for(k=0; k<64; k++)
			{
				cwblk=blkwin+((blkrov+k)&255)*16;
				e=BGBBTJ_BT1DQ_CalcBlockRMSE(cblk, cwblk);
				if(e<=rmse)break;
			}
			
//			if(k<256)
			if(e<=rmse)
			{
				blkrov=(blkrov-1)&255;
				cwblk2=blkwin+blkrov*16;
				memcpy(cwblk2, cwblk, 16);
				
				memcpy(coblk, cwblk, 16);
				continue;
			}

			blkrov=(blkrov-1)&255;
			cwblk2=blkwin+blkrov*16;
			memcpy(cwblk2, cblk, 16);
		}

#if 1
		//can we reuse last block's color (half-block)
		BGBBTJ_BT1DQ_CalcBlockAsHalf(cblk, tblk);
		e=BGBBTJ_BT1DQ_CalcBlockRMSE(cblk, tblk);
		if(e<=rmse)
//		if(1)
		{
			memcpy(tlblk, tblk, 16);
			memcpy(coblk, tblk, 16);
			continue;
		}
#endif

		memcpy(tlblk, cblk, 16);
		memcpy(coblk, cblk, 16);
	}
	return(0);
}


BGBBTJ_API int BGBBTJ_BT1D_QuantizeBlocks(
	byte *oblks, byte *blks, byte *lblks,
	int xs, int ys, int qfl)
{
	return(BGBBTJ_BT1D_QuantizeBlocksN(
		oblks, blks, lblks?(lblks):NULL, xs, ys, qfl, 16));
}
