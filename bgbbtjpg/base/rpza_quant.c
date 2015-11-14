#include <bgbbtj.h>

BGBBTJ_API int BGBBTJ_RPZAQ_DecodeBlock(
	byte *iblk, byte *orgba)
{
	BGBBTJ_BCn_DecodeBlockDXT1(
		iblk, orgba, 4, 16, 4);
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
	byte blkwin[256*8];
	byte tblk[8], tblk2[8], tlblk[8];
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

			e=BGBBTJ_RPZAQ_CalcBlockRMSE(cblk, clblk);
			if(e<=rmse)
			{
				BGBBTJ_RPZAQ_ThrashBlock(tlblk);
				memcpy(coblk, clblk, 8);
				continue;
			}
		}

#if 1
		//can block be replaced with a flat color?
		BGBBTJ_RPZAQ_CalcBlockAsFlat(cblk, tblk);
		e=BGBBTJ_RPZAQ_CalcBlockRMSE(cblk, tblk);
		if(e<=rmse)
		{
			//can we reuse last block's color
			BGBBTJ_RPZAQ_CalcBlockAsPriorFlatColor(tlblk, tblk, tblk2);
			e=BGBBTJ_RPZAQ_CalcBlockRMSE(cblk, tblk2);
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

		//can we reuse last block's color
		BGBBTJ_RPZAQ_CalcBlockAsPriorColor(tlblk, cblk, tblk);
		e=BGBBTJ_RPZAQ_CalcBlockRMSE(cblk, tblk);
		if(e<=rmse)
		{
			memcpy(coblk, tblk, 8);
			continue;
		}
		
		//can we replace the block with another recently seen block?
		if(qfl&BGBBTJ_QFL_BLOCKINDEX)
		{
			for(k=0; k<256; k++)
			{
				cwblk=blkwin+((blkrov+k)&255)*8;
				e=BGBBTJ_RPZAQ_CalcBlockRMSE(cblk, cwblk);
				if(e<=rmse)break;
			}
			
			if(k<256)
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
