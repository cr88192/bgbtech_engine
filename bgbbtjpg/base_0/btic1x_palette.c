/*
Generate Optimized Palettes
Use an octree.

Palette Index:
RGB555, Using bits 8-13 from half-float images.

0-16383: explicit single color.
16384-32767: nearest color.
32768-65535: chain (multiple colors).

Chain Entries:
bits 0-14: Next Entry or Color
bit 15:
	0: End Of Chain, Holds Color
	1: Chain Continues, Holds Entry
Bits 16-30: Color

*/

#include <bgbbtj.h>

BGBBTJ_BTIC1C_TmpPalColor *BGBBTJ_BT1X_AllocPalColor(void)
{
	BGBBTJ_BTIC1C_TmpPalColor *tmp;
	tmp=gctalloc("bt1c_tmppalcolor_t", sizeof(BGBBTJ_BTIC1C_TmpPalColor));
	return(tmp);
}

BGBBTJ_BTIC1C_TmpPalNode *BGBBTJ_BT1X_AllocPalNode(void)
{
	BGBBTJ_BTIC1C_TmpPalNode *tmp;
	tmp=gctalloc("bt1c_tmppalnode_t", sizeof(BGBBTJ_BTIC1C_TmpPalNode));
	return(tmp);
}

void BGBBTJ_BT1X_FreePalColor(BGBBTJ_BTIC1C_TmpPalColor *tmp)
{
	gcfree(tmp);
}

void BGBBTJ_BT1X_FreePalNode(BGBBTJ_BTIC1C_TmpPalNode *tmp)
{
	gcfree(tmp);
}

void BGBBTJ_BT1X_RebuildSplitColorPalNode(
	BGBBTJ_BTIC1C_TmpPalNode *tmp)
{
	BGBBTJ_BTIC1C_TmpPalColor *ccur;
	
	tmp->trgb[0]=0;
	tmp->trgb[1]=0;
	tmp->trgb[2]=0;
	tmp->rgbcnt=0;
	tmp->nclrs=0;
	
	ccur=tmp->clrs;
	while(ccur)
	{
		tmp->trgb[0]+=((float)ccur->rgb[0])*ccur->cnt;
		tmp->trgb[1]+=((float)ccur->rgb[1])*ccur->cnt;
		tmp->trgb[2]+=((float)ccur->rgb[2])*ccur->cnt;
		tmp->rgbcnt+=ccur->cnt;
		tmp->nclrs++;
		
		ccur=ccur->next;
	}
}

void BGBBTJ_BT1X_SplitColorPalNode(
	BGBBTJ_BTIC1C_TmpPalNode *tmp)
{
	BGBBTJ_BTIC1C_TmpPalColor *ccur, *cnxt;
	int tr, tg, tb;
	int i, j, k;

	if(tmp->child[0])
		return;

	tr=tmp->trgb[0]/tmp->rgbcnt;
	tg=tmp->trgb[1]/tmp->rgbcnt;
	tb=tmp->trgb[2]/tmp->rgbcnt;
	tmp->crgb[0]=tr;
	tmp->crgb[1]=tg;
	tmp->crgb[2]=tb;
	
	for(i=0; i<8; i++)
		{ tmp->child[i]=BGBBTJ_BT1X_AllocPalNode(); }
		
	ccur=tmp->clrs;
	tmp->clrs=NULL;
	while(ccur)
	{
		i=0;
		if(ccur->rgb[0]>=tr)i|=1;
		if(ccur->rgb[1]>=tg)i|=2;
		if(ccur->rgb[2]>=tb)i|=4;
		cnxt=ccur->next;
		ccur->next=tmp->child[i]->clrs;
		tmp->child[i]->clrs=ccur;
		ccur=cnxt;
	}

	for(i=0; i<8; i++)
		{ BGBBTJ_BT1X_RebuildSplitColorPalNode(tmp->child[i]); }
}

void BGBBTJ_BT1X_InsertColorPalNode(
	BGBBTJ_BTIC1C_TmpPalNode *tmp, int cr, int cg, int cb)
{
	static int rec=0;
	BGBBTJ_BTIC1C_TmpPalColor *ccur;
	
	int tr, tg, tb;
	int i, j, k;

	//already split?
	if(tmp->child[0])
	{
//		tr=tmp->trgb[0]/tmp->rgbcnt;
//		tg=tmp->trgb[1]/tmp->rgbcnt;
//		tb=tmp->trgb[2]/tmp->rgbcnt;
		tr=tmp->crgb[0];
		tg=tmp->crgb[1];
		tb=tmp->crgb[2];
		i=0;
		if(cr>=tr)i|=1;
		if(cg>=tg)i|=2;
		if(cb>=tb)i|=4;
		rec++;
		BGBBTJ_BT1X_InsertColorPalNode(tmp->child[i], cr, cg, cb);
		rec--;
		
		//add color to node average
		tmp->trgb[0]+=cr;
		tmp->trgb[1]+=cg;
		tmp->trgb[2]+=cb;
		tmp->rgbcnt++;
//		tmp->tcnt++;
		return;
	}
	
	//need to split?
//	if((tmp->rgbcnt>=256) || (tmp->nclrs>=16))
	if((tmp->nclrs>=16) && (rec<64))
	{
		//split node and try again
		BGBBTJ_BT1X_SplitColorPalNode(tmp);
		rec++;
		BGBBTJ_BT1X_InsertColorPalNode(tmp, cr, cg, cb);
		rec--;
		return;
	}
	
	ccur=tmp->clrs;
	while(ccur)
	{
		if(	(ccur->rgb[0]==cr) &&
			(ccur->rgb[1]==cg) &&
			(ccur->rgb[2]==cb))
				break;
		ccur=ccur->next;
	}
	
	if(ccur)
	{
		ccur->cnt++;
	}else if(tmp->nclrs<256)
	{
		ccur=BGBBTJ_BT1X_AllocPalColor();
		ccur->rgb[0]=cr;
		ccur->rgb[1]=cg;
		ccur->rgb[2]=cb;
		ccur->cnt=1;
		ccur->next=tmp->clrs;
		tmp->clrs=ccur;
		tmp->nclrs++;
	}

	tmp->trgb[0]+=cr;
	tmp->trgb[1]+=cg;
	tmp->trgb[2]+=cb;
	tmp->rgbcnt++;
	return;
}

void BGBBTJ_BT1X_DestroyPalNodeTree(BGBBTJ_BTIC1C_TmpPalNode *tmp)
{
	BGBBTJ_BTIC1C_TmpPalColor *ccur, *cnxt;
	int i, j, k;
	
	if(!tmp)return;
	
	for(i=0; i<8; i++)
		{ BGBBTJ_BT1X_DestroyPalNodeTree(tmp->child[i]); }

	ccur=tmp->clrs;
	while(ccur)
	{
		cnxt=ccur->next;
		BGBBTJ_BT1X_FreePalColor(ccur);
		ccur=cnxt;
	}

	BGBBTJ_BT1X_FreePalNode(tmp);
}

BGBBTJ_BTIC1C_TmpPalNode *BGBBTJ_BT1X_InsertPalNodeListByCount(
	BGBBTJ_BTIC1C_TmpPalNode *lst,
	BGBBTJ_BTIC1C_TmpPalNode *tmp)
{
	BGBBTJ_BTIC1C_TmpPalNode *cur, *prv;
	
	if(!tmp)return(lst);
	
	cur=lst; prv=NULL;
	while(cur && (tmp->rgbcnt<=cur->rgbcnt))
		{ prv=cur; cur=cur->next; }
	if(prv)
		{ tmp->next=cur; prv->next=tmp; }
	else
		{ tmp->next=lst; lst=tmp; }
	return(lst);
}

int BGBBTJ_BT1X_GeneratePaletteRGB24(
	byte *src, int npix, int stride,
	byte *pal, int maxclr, int pstride)
{
	BGBBTJ_BTIC1C_TmpPalNode *tree, *tlst, *tcur;
	int nclr;
	int tr, tg, tb;
	int i, j, k;
	
	tree=BGBBTJ_BT1X_AllocPalNode();
	for(i=0; i<npix; i++)
	{
		BGBBTJ_BT1X_InsertColorPalNode(tree,
			src[i*stride+0], src[i*stride+1], src[i*stride+2]);
	}
	
	tlst=tree;
	tlst->next=NULL;
	
	nclr=0;
	while(tlst && (nclr<maxclr))
	{
		tcur=tlst;
		tlst=tcur->next;
		
		tr=tcur->trgb[0]/tcur->rgbcnt;
		tg=tcur->trgb[1]/tcur->rgbcnt;
		tb=tcur->trgb[2]/tcur->rgbcnt;
		
		i=nclr++;
		pal[i*pstride+0]=tr;
		pal[i*pstride+1]=tg;
		pal[i*pstride+2]=tb;
		
		for(i=0; i<8; i++)
		{
			tlst=BGBBTJ_BT1X_InsertPalNodeListByCount(
				tlst, tcur->child[i]);
		}
	}
	
	BGBBTJ_BT1X_DestroyPalNodeTree(tree);
	return(nclr);
}

int BGBBTJ_BT1X_GeneratePaletteRGB48(
	u16 *src, int npix, int stride,
	u16 *pal, int maxclr, int pstride)
{
	BGBBTJ_BTIC1C_TmpPalNode *tree, *tlst, *tcur;
	int nclr;
	int tr, tg, tb;
	int i, j, k;
	
	tree=BGBBTJ_BT1X_AllocPalNode();
	for(i=0; i<npix; i++)
	{
		BGBBTJ_BT1X_InsertColorPalNode(tree,
			src[i*stride+0], src[i*stride+1], src[i*stride+2]);
	}
	
	tlst=tree;
	tlst->next=NULL;
	
	nclr=0;
	while(tlst && (nclr<maxclr))
	{
		tcur=tlst;
		tlst=tcur->next;
		
		tr=tcur->trgb[0]/tcur->rgbcnt;
		tg=tcur->trgb[1]/tcur->rgbcnt;
		tb=tcur->trgb[2]/tcur->rgbcnt;
		
		i=nclr++;
		pal[i*pstride+0]=tr;
		pal[i*pstride+1]=tg;
		pal[i*pstride+2]=tb;
		
		for(i=0; i<8; i++)
		{
			tlst=BGBBTJ_BT1X_InsertPalNodeListByCount(
				tlst, tcur->child[i]);
		}
	}
	
	BGBBTJ_BT1X_DestroyPalNodeTree(tree);
	return(nclr);
}

int BGBBTJ_BT1X_GeneratePaletteBGR24(
	byte *src, int npix, int stride,
	byte *pal, int maxclr, int pstride)
{
	BGBBTJ_BTIC1C_TmpPalNode *tree, *tlst, *tcur;
	int nclr;
	int tr, tg, tb;
	int i, j, k;
	
	tree=BGBBTJ_BT1X_AllocPalNode();
	for(i=0; i<npix; i++)
	{
		BGBBTJ_BT1X_InsertColorPalNode(tree,
			src[i*stride+2], src[i*stride+1], src[i*stride+0]);
	}
	
	tlst=tree;
	tlst->next=NULL;
	
	nclr=0;
	while(tlst && (nclr<maxclr))
	{
		tcur=tlst;
		tlst=tcur->next;
		
		tr=tcur->trgb[0]/tcur->rgbcnt;
		tg=tcur->trgb[1]/tcur->rgbcnt;
		tb=tcur->trgb[2]/tcur->rgbcnt;
		
		i=nclr++;
		pal[i*pstride+0]=tr;
		pal[i*pstride+1]=tg;
		pal[i*pstride+2]=tb;
		
		for(i=0; i<8; i++)
		{
			tlst=BGBBTJ_BT1X_InsertPalNodeListByCount(
				tlst, tcur->child[i]);
		}
	}
	
	BGBBTJ_BT1X_DestroyPalNodeTree(tree);
	return(nclr);
}

int BGBBTJ_BT1X_CalcIndex555_RGB24(byte *clr)
{
	int i;
	i=  ((clr[0]&0xF8)<<7) |
		((clr[1]&0xF8)<<2) |
		((clr[2]&0xF8)>>3);
	return(i);
}

int BGBBTJ_BT1X_CalcIndex555_RGB48(u16 *clr)
{
	int i;
	i=  ((clr[0]&0x1F00)<<2) |
		((clr[1]&0x1F00)>>3) |
		((clr[2]&0x1F00)>>8);
	return(i);
}

int BGBBTJ_BT1X_CalcDistance_RGB24(byte *clra, byte *clrb)
{
	int dr, dg, db, d;
	
	dr=clra[0]-clrb[0];
	dg=clra[1]-clrb[1];
	db=clra[2]-clrb[2];
	d=dr*dr+dg*dg+db*db;
	return(d);
}

int BGBBTJ_BT1X_CalcDistance_RGB48(u16 *clra, u16 *clrb)
{
	s64 dr, dg, db, d;
	
	dr=clra[0]-clrb[0];
	dg=clra[1]-clrb[1];
	db=clra[2]-clrb[2];
	d=dr*dr+dg*dg+db*db;
	return((int)(d>>16));
}

void BGBBTJ_BT1X_UnpackColor555_RGB24(u16 clra, byte *clrb)
{
	clrb[0]=(clra>>10)&0x1F;
	clrb[1]=(clra>>5)&0x1F;
	clrb[2]=(clra>>0)&0x1F;
	clrb[0]=(clrb[0]<<3)|(clrb[0]>>2);
	clrb[1]=(clrb[1]<<3)|(clrb[1]>>2);
	clrb[2]=(clrb[2]<<3)|(clrb[2]>>2);
}

void BGBBTJ_BT1X_UnpackColor565_RGB24(u16 clra, byte *clrb)
{
	clrb[0]=(clra>>11)&0x1F;
	clrb[1]=(clra>>5)&0x3F;
	clrb[2]=(clra>>0)&0x1F;
	clrb[0]=(clrb[0]<<3)|(clrb[0]>>2);
	clrb[1]=(clrb[1]<<2)|(clrb[1]>>4);
	clrb[2]=(clrb[2]<<3)|(clrb[2]>>2);
}

void BGBBTJ_BT1X_UnpackColor24_RGB24(u32 clra, byte *clrb)
{
	clrb[0]=(clra>>16)&0xFF;
	clrb[1]=(clra>> 8)&0xFF;
	clrb[2]=(clra>> 0)&0xFF;
}

int BGBBTJ_BT1X_FillHolesPaletteIndex555_RGB24(
	byte *pal, int nclr, int stride,
	u16 *index, u32 *chain)
{
	static int stab[]={
	-1, 1, -1024, 1024, -32, 32,
	-1025, -1023, 1023, 1025, 
	-33, -31, 31, 33,
	-1056, -992, 992, 1056,
	-1057, -993, 991, 1055,
	-1055, -991, 993, 1057,
	};
	byte tclr[4];
	int bi, bd, d;
	int i, j, k, l;

	//try to fill holes in the index
	for(i=0; i<32768; i++)
	{
		//if not a null-entry, do nothing
		if(index[i]!=16384)
			continue;

		BGBBTJ_BT1X_UnpackColor555_RGB24(i, tclr);

		bi=0; bd=999999999;
		for(j=0; j<nclr; j++)
		{
			d=BGBBTJ_BT1X_CalcDistance_RGB24(tclr, pal+j*stride);
			if(d<bd) { bi=j; bd=d; }
		}
		index[i]=bi|16384;

#if 0
		//check if adjacent entries have colors.
		for(j=0; j<24; j++)
		{
			k=i+stab[j];
//			k=(j&1)?(k+   1):(k-   1);
//			k=(j&2)?(k+1024):(k-1024);
//			k=(j&4)?(k+  32):(k-  32);
			if((k>=0) && (k<32768))
			{
				l=index[k];
				if(l<16384)
				{
					//single color index
					index[i]=l|16384;
					break;
				}
				if(l&32768)
				{
					//first color in chain
					l=chain[l&32767];
					index[i]=(l>>16)|16384;
					break;				
				}
			}
		}
		
		//did we find something?
		if(index[i]!=16384)
			continue;
		if(i>0)
		{
			//ok, just copy prior color
			index[i]=index[i-1];
			continue;
		}
#endif
	}

	return(0);
}

int BGBBTJ_BT1X_GeneratePaletteIndex555_RGB24(
	byte *pal, int nclr, int stride,
	u16 *index, u32 *chain, int maxchain)
{
	int nchn;
	int i, j, k, l;
	
	for(i=0; i<32768; i++)
	{
		index[i]=16384;
	}

	nchn=0;
	for(i=0; i<nclr; i++)
	{
		j=BGBBTJ_BT1X_CalcIndex555_RGB24(pal+i*stride);
		k=index[j];
		if(k==16384)
			{ index[j]=i; continue; }
		l=nchn++;
		chain[l]=(i<<16)|k;
		index[j]=0x8000|l;
	}

	BGBBTJ_BT1X_FillHolesPaletteIndex555_RGB24(pal, nclr, stride,
		index, chain);

	return(nchn);
}

int BGBBTJ_BT1X_GeneratePaletteIndex555_RGB48(
	u16 *pal, int nclr, int stride,
	u16 *index, u32 *chain, int maxchain)
{
	int nchn;
	int i, j, k, l;
	
	for(i=0; i<32768; i++)
	{
		index[i]=16384;
	}

	nchn=0;
	for(i=0; i<nclr; i++)
	{
		j=BGBBTJ_BT1X_CalcIndex555_RGB48(pal+i*stride);
		k=index[j];
		if(k==16384)
			{ index[j]=i; continue; }
		l=nchn++;
		chain[l]=(i<<16)|k;
		index[j]=0x8000|l;
	}

	BGBBTJ_BT1X_FillHolesPaletteIndex555_RGB24(NULL, nclr, stride,
		index, chain);

	return(nchn);
}

int BGBBTJ_BT1X_LookupColorIndex555_RGB24(
	byte *clr,
	byte *pal, int nclr, int pstr,
	u16 *index, u32 *chain)
{
	int bi, bd, d;
	int i, j, k;

	i=BGBBTJ_BT1X_CalcIndex555_RGB24(clr);
	j=index[i];
	if(j<32768)
		return(j&16383);

	i=j&32767;
	bi=(chain[i]>>16)&16383; bd=999999999;
	while(1)
	{
		j=chain[i];
		k=(j>>16)&16383;
		d=BGBBTJ_BT1X_CalcDistance_RGB24(clr, pal+k*pstr);
		if(d<bd) { bi=k; bd=d; }
		if(j&32768)
			{ i=j&32767; continue; }
		k=j&16383;
		d=BGBBTJ_BT1X_CalcDistance_RGB24(clr, pal+k*pstr);
		if(d<bd) { bi=k; bd=d; }
		break;
	}
	return(bi);
}

int BGBBTJ_BT1X_LookupColorIndex555_RGB48(
	u16 *clr,
	u16 *pal, int nclr, int pstr,
	u16 *index, u32 *chain)
{
	int bi, bd, d;
	int i, j, k;

	i=BGBBTJ_BT1X_CalcIndex555_RGB48(clr);
	j=index[i];
	if(j<32768)
		return(j&16383);

	i=j&32767;
	bi=(chain[i]>>16)&16383; bd=999999999;
	while(1)
	{
		j=chain[i];
		k=(j>>16)&16383;
		d=BGBBTJ_BT1X_CalcDistance_RGB48(clr, pal+k*pstr);
		if(d<bd) { bi=k; bd=d; }
		if(j&32768)
			{ i=j&32767; continue; }
		k=j&16383;
		d=BGBBTJ_BT1X_CalcDistance_RGB48(clr, pal+k*pstr);
		if(d<bd) { bi=k; bd=d; }
		break;
	}
	return(bi);
}

int BGBBTJ_BT1X_LookupColorIndex555_RGB565P24(
	u16 clra, u32 *pal, int nclr,
	u16 *index, u32 *chain)
{
	byte clr[4], pclr[4];
	int bi, bd, d;
	int i, j, k;

	BGBBTJ_BT1X_UnpackColor565_RGB24(clra, clr);

	i=BGBBTJ_BT1X_CalcIndex555_RGB24(clr);
	j=index[i];
	if(j<32768)
		return(j&16383);

	i=j&32767;
	bi=(chain[i]>>16)&16383; bd=999999999;
	while(1)
	{
		j=chain[i];
		k=(j>>16)&16383;
		BGBBTJ_BT1X_UnpackColor24_RGB24(pal[k], pclr);
		d=BGBBTJ_BT1X_CalcDistance_RGB24(clr, pclr);
		if(d<bd) { bi=k; bd=d; }
		if(j&32768)
			{ i=j&32767; continue; }
		k=j&16383;
		BGBBTJ_BT1X_UnpackColor24_RGB24(pal[k], pclr);
		d=BGBBTJ_BT1X_CalcDistance_RGB24(clr, pclr);
		if(d<bd) { bi=k; bd=d; }
		break;
	}
	return(bi);
}
