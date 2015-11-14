#include <bgbbtj.h>

BGBBTJ_API int BGBBTJ_RPZAQ_DecodeBlock(
	byte *iblk, byte *orgba)
{
	int l0, l1, l2, l3;
	int p0, p1, p2, p3;
	int i, j, k;

	BGBBTJ_BCn_DecodeBlockDXT1(
		iblk, orgba, 4, 16, 4);
	
	for(i=0; i<3; i++)
		for(j=0; j<3; j++)
	{
		l0=(i*4+j+0)*4;	l1=(i*4+j+1)*4;
		l2=(i*4+j+4)*4;	l3=(i*4+j+5)*4;
//		p0=(32*orgba[l0+0])+(12*orgba[l1+0])+(12*orgba[l2+0])+(8*orgba[l3+0]);
//		p1=(32*orgba[l0+1])+(12*orgba[l1+1])+(12*orgba[l2+1])+(8*orgba[l3+1]);
//		p2=(32*orgba[l0+2])+(12*orgba[l1+2])+(12*orgba[l2+2])+(8*orgba[l3+2]);
//		p3=(32*orgba[l0+3])+(12*orgba[l1+3])+(12*orgba[l2+3])+(8*orgba[l3+3]);
		p0= (26*orgba[l0+0])+(14*orgba[l1+0])+
			(14*orgba[l2+0])+(10*orgba[l3+0]);
		p1= (26*orgba[l0+1])+(14*orgba[l1+1])+
			(14*orgba[l2+1])+(10*orgba[l3+1]);
		p2= (26*orgba[l0+2])+(14*orgba[l1+2])+
			(14*orgba[l2+2])+(10*orgba[l3+2]);
		p3= (26*orgba[l0+3])+(14*orgba[l1+3])+
			(14*orgba[l2+3])+(10*orgba[l3+3]);
		orgba[l0+0]=p0>>6;
		orgba[l0+1]=p1>>6;
		orgba[l0+2]=p2>>6;
		orgba[l0+3]=p3>>6;
	}
	
	return(0);
}

BGBBTJ_API int BGBBTJ_RPZAQ_CalcColorRMSE(
	byte *rgb0, byte *rgb1)
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

BGBBTJ_API double BGBBTJ_RPZAQ_CalcBlockRMSE(
	byte *blka, byte *blkb)
{
	byte rgba0[16*4];
	byte rgba1[16*4];
	int e;
	int i;
	
	if((blkb[0]==0x55) && (blkb[1]==0xAA))
	{
		e=16*1024;
		return(e);
	}
	
	BGBBTJ_RPZAQ_DecodeBlock(blka, rgba0);
	BGBBTJ_RPZAQ_DecodeBlock(blkb, rgba1);
	
	e=0;
	for(i=0; i<16; i++)
	{
		e+=BGBBTJ_RPZAQ_CalcColorRMSE(rgba0+i*4, rgba1+i*4);
	}
//	e=sqrt(e/(5*16));
	e=sqrt(e/5.0);
	return(e);
}

BGBBTJ_API void BGBBTJ_RPZAQ_CalcBlockAsFlat(
	byte *blka, byte *blkb)
{
	byte rgba0[16*4];
//	byte rgba1[16*4];
	int cr, cg, cb, ca;
	int i;

	if((blka[0]==blka[2]) && (blka[1]==blka[3]))
//	if(1)
	{
		memcpy(blkb, blka, 8);
		return;
	}

	BGBBTJ_RPZAQ_DecodeBlock(blka, rgba0);
	
	cr=0; cg=0; cb=0; ca=0;
	for(i=0; i<16; i++)
	{
		cr+=rgba0[i*4+0];	cg+=rgba0[i*4+1];
		cb+=rgba0[i*4+2];	ca+=rgba0[i*4+3];
	}
	
	cr=cr/16;	cg=cg/16;
	cb=cb/16;	ca=ca/16;
	
	if(ca<128)
	{
		i=((cr<<8)&0xF800) | ((cg<<3)&0x07E0) | ((cb>>3)&0x001F);
		blkb[0]=i;		blkb[1]=i>>8;
		blkb[2]=i;		blkb[3]=i>>8;
		blkb[4]=255;	blkb[5]=255;
		blkb[6]=255;	blkb[7]=255;
	}else
	{
		i=((cr<<8)&0xF800) | ((cg<<3)&0x07E0) | ((cb>>3)&0x001F);
		blkb[0]=i;	blkb[1]=i>>8;
		blkb[2]=i;	blkb[3]=i>>8;
		blkb[4]=0;	blkb[5]=0;
		blkb[6]=0;	blkb[7]=0;
	}
}

void BGBBTJ_RPZAQ_CalcBlockAs2x2(
	byte *blka, byte *blkb)
{
//	static byte halfidx[16]=
//	{ 0, 2, 2, 2,
//	  2, 1, 3, 3,
//	  2, 3, 2, 2,
//	  2, 3, 2, 3};
	static byte halfidx[16]=
	{ 0x0, 0xA, 0xA, 0xA,
	  0xA, 0x5, 0xF, 0xF,
	  0xA, 0xF, 0xA, 0xA,
	  0xA, 0xF, 0xA, 0xF};
	
	blkb[0]=blka[0]; blkb[1]=blka[1];
	blkb[2]=blka[2]; blkb[3]=blka[3];
	
	blkb[4]=halfidx[blka[4]&15]|(halfidx[blka[4]>>4]<<4);
	blkb[5]=halfidx[blka[5]&15]|(halfidx[blka[5]>>4]<<4);
	blkb[5]=(blkb[4]&0xCC)|(blkb[5]&0x33);
	blkb[4]=halfidx[blkb[5]&15]|(halfidx[blkb[5]>>4]<<4);
	blkb[5]=blkb[4];

	blkb[6]=halfidx[blka[6]&15]|(halfidx[blka[6]>>4]<<4);
	blkb[7]=halfidx[blka[7]&15]|(halfidx[blka[7]>>4]<<4);
	blkb[7]=(blkb[6]&0xCC)|(blkb[7]&0x33);
	blkb[6]=halfidx[blkb[7]&15]|(halfidx[blkb[7]>>4]<<4);
	blkb[7]=blkb[6];
	
//	blkb[4]=halfidx[blka[4]&15]|(halfidx[blka[4]>>4]<<4);
//	blkb[5]=halfidx[blka[5]&15]|(halfidx[blka[5]>>4]<<4);
//	blkb[6]=halfidx[blka[6]&15]|(halfidx[blka[6]>>4]<<4);
//	blkb[7]=halfidx[blka[7]&15]|(halfidx[blka[7]>>4]<<4);
//	blkb[5]=blkb[4];
//	blkb[7]=blkb[6];
}

void BGBBTJ_RPZAQ_CalcBlockAs1bpp(
	byte *blka, byte *blkb)
{
//	static byte halfidx[16]=
//	{ 0, 2, 2, 2,
//	  2, 1, 3, 3,
//	  2, 3, 2, 2,
//	  2, 3, 2, 3};
	static byte halfidx[16]=
	{ 0x0, 0x1, 0x0, 0x1,
	  0x4, 0x5, 0x4, 0x5,
	  0x0, 0x1, 0x0, 0x1,
	  0x4, 0x5, 0x4, 0x5};
	
	blkb[0]=blka[0]; blkb[1]=blka[1];
	blkb[2]=blka[2]; blkb[3]=blka[3];
	
	blkb[4]=halfidx[blka[4]&15]|(halfidx[blka[4]>>4]<<4);
	blkb[5]=halfidx[blka[5]&15]|(halfidx[blka[5]>>4]<<4);
	blkb[6]=halfidx[blka[6]&15]|(halfidx[blka[6]>>4]<<4);
	blkb[7]=halfidx[blka[7]&15]|(halfidx[blka[7]>>4]<<4);
}

BGBBTJ_API void BGBBTJ_RPZAQ_CalcBlockAsPriorColor(
	byte *blka, byte *blkb, byte *blkc)
{
	blkc[0]=blka[0];	blkc[1]=blka[1];
	blkc[2]=blka[2];	blkc[3]=blka[3];
	blkc[4]=blkb[4];	blkc[5]=blkb[5];
	blkc[6]=blkb[6];	blkc[7]=blkb[7];
}

BGBBTJ_API void BGBBTJ_RPZAQ_CalcBlockAsPriorFlatColor(
	byte *blka, byte *blkb, byte *blkc)
{
	blkc[0]=blka[0];	blkc[1]=blka[1];
	blkc[2]=blka[0];	blkc[3]=blka[1];
	blkc[4]=blkb[4];	blkc[5]=blkb[5];
	blkc[6]=blkb[6];	blkc[7]=blkb[7];
}

BGBBTJ_API void BGBBTJ_RPZAQ_CalcBlockAsDiffScale(
	byte *oblks, byte *ct, int xs1, int ys1,
	byte *blka, byte *blkb, int fl, int stride, int sc)
{
	int ca, cb;

	ct[0]=blka[0];	ct[1]=blka[1];
	ct[2]=blka[2];	ct[3]=blka[3];

	ca=BGBBTJ_BT1C_EncodePredictDiffColorA(
		oblks, ct, xs1, ys1,
		fl, stride, sc);
	cb=BGBBTJ_BT1C_EncodePredictDiffColorB(
		oblks, ct, xs1, ys1,
		fl, stride, sc);

	blkb[0]=ca&255;		blkb[1]=(ca>>8)&255;
	blkb[2]=cb&255;		blkb[3]=(cb>>8)&255;
//	blkb[0]=blka[0];	blkb[1]=blka[1];
//	blkb[2]=blka[0];	blkb[3]=blka[1];
	blkb[4]=blka[4];	blkb[5]=blka[5];
	blkb[6]=blka[6];	blkb[7]=blka[7];
}

#if 0
// BGBBTJ_API double BGBBTJ_RPZAQ_CalcBlockAsFlatRMSE(
//	byte *blka, byte *blkb)
{
	byte rgba0[16*4];
//	byte rgba1[16*4];
	int e;
	int i;
	
	BGBBTJ_RPZAQ_DecodeBlock(blka, rgba0);
//	BGBBTJ_RPZAQ_DecodeBlock(blkb, rgba1);
	
	e=0;
	for(i=0; i<16; i++)
	{
		e+=BGBBTJ_RPZAQ_CalcColorRMSE(rgba0+i*4, rgba1+i*4);
	}
	e=sqrt(e/(5*16));
	return(e);
}
#endif

BGBBTJ_API void BGBBTJ_RPZAQ_ThrashBlock(byte *blk)
{
	blk[0]=0x55; blk[1]=0xAA;
	blk[2]=0xAA; blk[3]=0x55;
	blk[4]=0x55; blk[5]=0xAA;
	blk[6]=0xAA; blk[7]=0x55;
}

BGBBTJ_API int BGBBTJ_RPZA_QuantizeBlocksDXTn(
	byte *oblks, byte *blks, byte *lblks,
	int xs, int ys, int qfl, int stride)
{
	static const int mvtab[32]=
	{
	-1, -1,	 0, -1,	 1, -1,  1,  0,
	 1,  1,  0,  1,	-1,  1, -1,  0,
	-2,  0,  0, -2,	 2,  0,  0,  2,
	-3,  0,  0, -3,	 3,  0,  0,  3,
	};

	byte blkwin[256*8];
	byte tblk[8], tblk2[8], tlblk[8];
	byte aiblk[8], ablk[8];
	byte *cblk, *ciblk, *clblk, *coblk, *cwblk, *cwblk2;
	byte *csl, *csle, *cs1, *bcs1;
	double e, be;
	double rmse;
	byte blkrov;
	int xs1, ys1;
	int xs2, ys2;
	int xs3, ys3;
	int bx, by;
	int i, j, k, l, n, csc;
	
	rmse=100-(qfl&255);
//	rmse=(rmse*rmse)/100;
//	rmse=(rmse*rmse)/80.0;
//	rmse=pow(rmse, 1.5)/10.0;
	rmse=pow(rmse, 1.5)/20.0;
//	rmse=1;
	blkrov=0;
	
	if(rmse<4.0)
	{
		if(oblks!=blks)
		{
			xs1=(xs+3)>>2; ys1=(ys+3)>>2;
			for(i=0; i<ys1; i++)
				for(j=0; j<xs1; j++)
			{
				cblk=blks+(i*xs1+j)*stride;
				coblk=oblks+(i*xs1+j)*stride;
				memcpy(coblk, cblk, 8);
			}
		}
		return(0);
	}
	
	for(i=0; i<256; i++)
		{ BGBBTJ_RPZAQ_ThrashBlock(blkwin+i*8); }
	
	BGBBTJ_RPZAQ_ThrashBlock(tlblk);
	xs1=(xs+3)>>2; ys1=(ys+3)>>2;
	n=xs1*ys1; csc=0;
	
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

	csl=lblks; csle=csl+n*stride;

//	for(i=0; i<ys1; i++)
//		for(j=0; j<xs1; j++)
	for(i=0; i<n; i++)
	{
//		cblk=blks+(i*xs1+j)*stride;
//		coblk=oblks+(i*xs1+j)*stride;
		cblk=blks+i*stride;
		ciblk=cblk;
		coblk=oblks+i*stride;

		memcpy(aiblk, ciblk, 8);
		memcpy(ablk, ciblk, 8);
		cblk=aiblk;

		//is block sufficiently similar to last-frame block?
		if(lblks)
		{
//			clblk=lblks+(i*xs1+j)*stride;
			clblk=lblks+i*stride;

			e=BGBBTJ_RPZAQ_CalcBlockRMSE(cblk, clblk);
			if(e<=rmse)
			{
				BGBBTJ_RPZAQ_ThrashBlock(tlblk);
				memcpy(coblk, clblk, 8);
				continue;
			}
		}

		if(lblks && (qfl&BGBBTJ_BTIC1C_QFL_MOTION))
		{
			clblk=lblks+i*stride;
			bcs1=clblk;

			bx=0; by=0; be=999999999.0;
#if 0
			for(j=-2; j<=2; j++)
			{
				for(k=-2; k<=2; k++)
				{
					cs1=clblk+(j*xs1*stride)+k*stride;
					if(cs1<lblks)continue;
					if(cs1>=csle)continue;

					e=BGBBTJ_RPZAQ_CalcBlockRMSE(cblk, cs1);
					if(e<=be)
						{ bx=k; by=j; be=e; bcs1=cs1; }
				}
			}
#endif

#if 1
			for(j=0; j<16; j++)
			{
				cs1=clblk+
					(mvtab[j*2+1]*xs1*stride)+
					mvtab[j*2+0]*stride;
				if(cs1<lblks)continue;
				if(cs1>=csle)continue;

				e=BGBBTJ_RPZAQ_CalcBlockRMSE(cblk, cs1);
				if(e<=be)
					{ bx=mvtab[j*2+0]; by=mvtab[j*2+1]; be=e; bcs1=cs1; }
			}
#endif

			if(be<=rmse)
			{
				BGBBTJ_RPZAQ_ThrashBlock(tlblk);
				memcpy(coblk, bcs1, 8);
				continue;
			}
		}

		if(qfl&BGBBTJ_BTIC1C_QFL_DIFFCLR)
		{
			BGBBTJ_RPZAQ_CalcBlockAsDiffScale(oblks, coblk, xs1, ys1,
				cblk, tblk, qfl, stride, csc);
			e=BGBBTJ_RPZAQ_CalcBlockRMSE(cblk, tblk);
			if(e<=rmse)
			{
				memcpy(ablk, tblk, 8);
				cblk=ablk;
//				continue;
			}
			
			if(cblk!=ablk)
			{
				for(j=0; j<8; j++)
				{
					BGBBTJ_RPZAQ_CalcBlockAsDiffScale(oblks, coblk, xs1, ys1,
						cblk, tblk, qfl, stride, 0);
					e=BGBBTJ_RPZAQ_CalcBlockRMSE(cblk, tblk);
					if(e<=rmse)
					{
						memcpy(ablk, tblk, 8);
						cblk=ablk; csc=0;
						break;
					}
				}
			}
			
#if 0
			if(cblk!=ablk)
			{
				BGBBTJ_RPZAQ_CalcBlockAsDiffScale(oblks, coblk, xs1, ys1,
					cblk, tblk, qfl, stride, 0);
				e=BGBBTJ_RPZAQ_CalcBlockRMSE(cblk, tblk);
				if(e<=rmse)
				{
					memcpy(ablk, tblk, 8);
					cblk=ablk; csc=0;
//					continue;
				}
			}

			if(cblk!=ablk)
			{
				BGBBTJ_RPZAQ_CalcBlockAsDiffScale(oblks, coblk, xs1, ys1,
					cblk, tblk, qfl, stride, 1);
				e=BGBBTJ_RPZAQ_CalcBlockRMSE(cblk, tblk);
				if(e<=rmse)
				{
					memcpy(ablk, tblk, 8);
					cblk=ablk; csc=1;
//					continue;
				}
			}
#endif

#if 0
			if(cblk!=ablk)
			{
				BGBBTJ_RPZAQ_CalcBlockAsDiffScale(oblks, coblk, xs1, ys1,
					cblk, tblk, qfl, stride, 2);
				e=BGBBTJ_RPZAQ_CalcBlockRMSE(cblk, tblk);
				if(e<=rmse)
				{
					memcpy(ablk, tblk, 8);
					cblk=ablk; csc=2;
//					continue;
				}
			}

			if(cblk!=ablk)
			{
				BGBBTJ_RPZAQ_CalcBlockAsDiffScale(oblks, coblk, xs1, ys1,
					cblk, tblk, qfl, stride, 3);
				e=BGBBTJ_RPZAQ_CalcBlockRMSE(cblk, tblk);
				if(e<=rmse)
//				if(1)
				{
					memcpy(ablk, tblk, 8);
					cblk=ablk; csc=3;
//					continue;
				}
			}
#endif
		}
		
#if 1
		//can block be replaced with a flat color?
		if(qfl&BGBBTJ_BTIC1C_QFL_DIFFCLR)
//		if(0)
		{
			//are we using differential colors?
			//try to quantize to a differential color
			BGBBTJ_RPZAQ_CalcBlockAsFlat(ciblk, tblk2);
			BGBBTJ_RPZAQ_CalcBlockAsDiffScale(oblks, coblk, xs1, ys1,
				tblk2, tblk, qfl, stride, csc);
				
//			tblk[4]=0; tblk[5]=0;
//			tblk[6]=0; tblk[7]=0;
//			e=BGBBTJ_RPZAQ_CalcBlockRMSE(ciblk, tblk);
//			if(e<=rmse)
//			{
//				memcpy(ablk, tblk2, 8);
//				cblk=ablk;
//			}
		}else
		{
			BGBBTJ_RPZAQ_CalcBlockAsFlat(cblk, tblk);
		}
		e=BGBBTJ_RPZAQ_CalcBlockRMSE(cblk, tblk);
//		e=BGBBTJ_RPZAQ_CalcBlockRMSE(aiblk, tblk);
		if(e<=rmse)
		{
			//can we reuse last block's color
			BGBBTJ_RPZAQ_CalcBlockAsPriorFlatColor(tlblk, tblk, tblk2);
			e=BGBBTJ_RPZAQ_CalcBlockRMSE(cblk, tblk2);
//			e=BGBBTJ_RPZAQ_CalcBlockRMSE(aiblk, tblk2);
			if(e<=rmse)
			{
				memcpy(coblk, tblk2, 8);
				continue;
			}

//			BGBBTJ_RPZAQ_ThrashBlock(tlblk);
			memcpy(tlblk, tblk, 8);
			memcpy(coblk, tblk, 8);
			continue;
		}
#endif

#if 1
		if(qfl&BGBBTJ_BTIC1C_QFL_EXTMODES)
		{
			BGBBTJ_RPZAQ_CalcBlockAs2x2(cblk, tblk);
			e=BGBBTJ_RPZAQ_CalcBlockRMSE(cblk, tblk);
			if(e<=rmse)
//			if(1)
			{
				//can we reuse last block's color
//				BGBBTJ_RPZAQ_CalcBlockAsPriorFlatColor(tlblk, tblk, tblk2);
				BGBBTJ_RPZAQ_CalcBlockAsPriorColor(tlblk, tblk, tblk2);
				e=BGBBTJ_RPZAQ_CalcBlockRMSE(cblk, tblk2);
//				e=BGBBTJ_RPZAQ_CalcBlockRMSE(aiblk, tblk2);
				if(e<=rmse)
				{
					memcpy(ablk, tblk2, 8);
					cblk=ablk;
				}else
				{
					memcpy(ablk, tblk, 8);
					cblk=ablk;
				}
			}

#if 1
			BGBBTJ_RPZAQ_CalcBlockAs1bpp(cblk, tblk);
			e=BGBBTJ_RPZAQ_CalcBlockRMSE(cblk, tblk);
			if(e<=rmse)
			{
				//can we reuse last block's color
//				BGBBTJ_RPZAQ_CalcBlockAsPriorFlatColor(tlblk, tblk, tblk2);
				BGBBTJ_RPZAQ_CalcBlockAsPriorColor(tlblk, tblk, tblk2);
				e=BGBBTJ_RPZAQ_CalcBlockRMSE(cblk, tblk2);
//				e=BGBBTJ_RPZAQ_CalcBlockRMSE(aiblk, tblk2);
				if(e<=rmse)
				{
//					memcpy(coblk, tblk2, 8);
//					continue;
					memcpy(ablk, tblk2, 8);
					cblk=ablk;
				}else
				{
					memcpy(ablk, tblk, 8);
					cblk=ablk;
				}

//				BGBBTJ_RPZAQ_ThrashBlock(tlblk);
//				memcpy(tlblk, tblk, 8);
//				memcpy(coblk, tblk, 8);
//				continue;
			}
#endif
		}
#endif

		//can we reuse last block's color
		BGBBTJ_RPZAQ_CalcBlockAsPriorColor(tlblk, cblk, tblk);
		e=BGBBTJ_RPZAQ_CalcBlockRMSE(cblk, tblk);
//		e=BGBBTJ_RPZAQ_CalcBlockRMSE(aiblk, tblk);
		if(e<=rmse)
		{
			memcpy(coblk, tblk, 8);
			continue;
		}
		
		//can we replace the block with another recently seen block?
		if(qfl&BGBBTJ_QFL_BLOCKINDEX)
		{
//			for(k=0; k<256; k++)
			for(k=0; k<4; k++)
			{
				cwblk=blkwin+((blkrov+k)&255)*8;
				e=BGBBTJ_RPZAQ_CalcBlockRMSE(cblk, cwblk);
//				e=BGBBTJ_RPZAQ_CalcBlockRMSE(aiblk, cwblk);
				if(e<=rmse)break;
			}
			
//			if(k<256)
			if(k<4)
			{
				blkrov=(blkrov-1)&255;
				cwblk2=blkwin+blkrov*8;
				memcpy(cwblk2, cwblk, 8);
				
				memcpy(coblk, cwblk, 8);
				continue;
			}

			blkrov=(blkrov-1)&255;
			cwblk2=blkwin+blkrov*8;
			memcpy(cwblk2, cblk, 8);
		}

		memcpy(tlblk, cblk, 8);
		memcpy(coblk, cblk, 8);
	}
	return(0);
}


BGBBTJ_API int BGBBTJ_RPZA_QuantizeBlocksDXT5(
	byte *oblks, byte *blks, byte *lblks,
	int xs, int ys, int qfl)
{
	return(BGBBTJ_RPZA_QuantizeBlocksDXTn(
		oblks+8, blks+8, lblks?(lblks+8):NULL, xs, ys, qfl, 16));
}
