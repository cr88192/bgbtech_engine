/*

Mode 4
0000 1ttm  rrrr rRRR ( 0/ 1)
RRgg gggG  GGGG bbbb ( 2/ 3)
bBBB BBaa  aaaa AAAA ( 4/ 5)
AAii iiii  iiii iiii ( 6/ 7)
iiii iiii  iiii iiii ( 8/ 9)
ijjj jjjj  jjjj jjjj (10/11)
jjjj jjjj  jjjj jjjj (12/13)
jjjj jjjj  jjjj jjjj (14/15)

Mode 5
0000 01tt  rrrr rrrR ( 0/ 1)
RRRR RRgg  gggg gGGG ( 2/ 3)
GGGG bbbb  bbbB BBBB ( 4/ 5)
BBaa aaaa  aaAA AAAA ( 6/ 7)
AAii iiii  iiii iiii ( 8/ 9)
iiii iiii  iiii iiii (10/11)
ijjj jjjj  jjjj jjjj (12/13)
jjjj jjjj  jjjj jjjj (14/15)

Mode 6
0000 001r  rrrr rrRR ( 0/ 1)
RRRR Rggg  gggg GGGG ( 2/ 3)
GGGb bbbb  bbBB BBBB ( 4/ 5)
Baaa aaaa  AAAA AAAp ( 6/ 7)
Piii iiii  iiii iiii ( 8/ 9)
iiii iiii  iiii iiii (10/11)
iiii iiii  iiii iiii (12/13)
iiii iiii  iiii iiii (14/15)

*/


#include <bgbbtj.h>

int bgbbtj_bc7_weights2[4]=
	{ 0, 21, 43, 64};
int bgbbtj_bc7_weights3[8]=
	{ 0,  9, 18, 27, 37, 46, 55, 64};
int bgbbtj_bc7_weights4[16]= 
	{ 0,  4,  9, 13, 17, 21, 26, 30,
	 34, 38, 43, 47, 51, 55, 60, 64};

byte bgbbtj_bc7_partition2[64*16]=
{   0,0,1,1,0,0,1,1,0,0,1,1,0,0,1,1,    0,0,0,1,0,0,0,1,0,0,0,1,0,0,0,1,
    0,1,1,1,0,1,1,1,0,1,1,1,0,1,1,1,    0,0,0,1,0,0,1,1,0,0,1,1,0,1,1,1,
    0,0,0,0,0,0,0,1,0,0,0,1,0,0,1,1,    0,0,1,1,0,1,1,1,0,1,1,1,1,1,1,1,
    0,0,0,1,0,0,1,1,0,1,1,1,1,1,1,1,    0,0,0,0,0,0,0,1,0,0,1,1,0,1,1,1,
    0,0,0,0,0,0,0,0,0,0,0,1,0,0,1,1,    0,0,1,1,0,1,1,1,1,1,1,1,1,1,1,1,
    0,0,0,0,0,0,0,1,0,1,1,1,1,1,1,1,    0,0,0,0,0,0,0,0,0,0,0,1,0,1,1,1,
    0,0,0,1,0,1,1,1,1,1,1,1,1,1,1,1,    0,0,0,0,0,0,0,0,1,1,1,1,1,1,1,1,
    0,0,0,0,1,1,1,1,1,1,1,1,1,1,1,1,    0,0,0,0,0,0,0,0,0,0,0,0,1,1,1,1,
    0,0,0,0,1,0,0,0,1,1,1,0,1,1,1,1,    0,1,1,1,0,0,0,1,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,1,0,0,0,1,1,1,0,    0,1,1,1,0,0,1,1,0,0,0,1,0,0,0,0,
    0,0,1,1,0,0,0,1,0,0,0,0,0,0,0,0,    0,0,0,0,1,0,0,0,1,1,0,0,1,1,1,0,
    0,0,0,0,0,0,0,0,1,0,0,0,1,1,0,0,    0,1,1,1,0,0,1,1,0,0,1,1,0,0,0,1,
    0,0,1,1,0,0,0,1,0,0,0,1,0,0,0,0,    0,0,0,0,1,0,0,0,1,0,0,0,1,1,0,0,
    0,1,1,0,0,1,1,0,0,1,1,0,0,1,1,0,    0,0,1,1,0,1,1,0,0,1,1,0,1,1,0,0,
    0,0,0,1,0,1,1,1,1,1,1,0,1,0,0,0,    0,0,0,0,1,1,1,1,1,1,1,1,0,0,0,0,
    0,1,1,1,0,0,0,1,1,0,0,0,1,1,1,0,    0,0,1,1,1,0,0,1,1,0,0,1,1,1,0,0,
    0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,    0,0,0,0,1,1,1,1,0,0,0,0,1,1,1,1,
    0,1,0,1,1,0,1,0,0,1,0,1,1,0,1,0,    0,0,1,1,0,0,1,1,1,1,0,0,1,1,0,0,
    0,0,1,1,1,1,0,0,0,0,1,1,1,1,0,0,    0,1,0,1,0,1,0,1,1,0,1,0,1,0,1,0,
    0,1,1,0,1,0,0,1,0,1,1,0,1,0,0,1,    0,1,0,1,1,0,1,0,1,0,1,0,0,1,0,1,
    0,1,1,1,0,0,1,1,1,1,0,0,1,1,1,0,    0,0,0,1,0,0,1,1,1,1,0,0,1,0,0,0,
    0,0,1,1,0,0,1,0,0,1,0,0,1,1,0,0,    0,0,1,1,1,0,1,1,1,1,0,1,1,1,0,0,
    0,1,1,0,1,0,0,1,1,0,0,1,0,1,1,0,    0,0,1,1,1,1,0,0,1,1,0,0,0,0,1,1,
    0,1,1,0,0,1,1,0,1,0,0,1,1,0,0,1,    0,0,0,0,0,1,1,0,0,1,1,0,0,0,0,0,
    0,1,0,0,1,1,1,0,0,1,0,0,0,0,0,0,    0,0,1,0,0,1,1,1,0,0,1,0,0,0,0,0,
    0,0,0,0,0,0,1,0,0,1,1,1,0,0,1,0,    0,0,0,0,0,1,0,0,1,1,1,0,0,1,0,0,
    0,1,1,0,1,1,0,0,1,0,0,1,0,0,1,1,    0,0,1,1,0,1,1,0,1,1,0,0,1,0,0,1,
    0,1,1,0,0,0,1,1,1,0,0,1,1,1,0,0,    0,0,1,1,1,0,0,1,1,1,0,0,0,1,1,0,
    0,1,1,0,1,1,0,0,1,1,0,0,1,0,0,1,    0,1,1,0,0,0,1,1,0,0,1,1,1,0,0,1,
    0,1,1,1,1,1,1,0,1,0,0,0,0,0,0,1,    0,0,0,1,1,0,0,0,1,1,1,0,0,1,1,1,
    0,0,0,0,1,1,1,1,0,0,1,1,0,0,1,1,    0,0,1,1,0,0,1,1,1,1,1,1,0,0,0,0,
    0,0,1,0,0,0,1,0,1,1,1,0,1,1,1,0,    0,1,0,0,0,1,0,0,0,1,1,1,0,1,1,1};

byte bgbbtj_bc7_partition3[64*16]=
{   0,0,1,1,0,0,1,1,0,2,2,1,2,2,2,2,    0,0,0,1,0,0,1,1,2,2,1,1,2,2,2,1,
    0,0,0,0,2,0,0,1,2,2,1,1,2,2,1,1,    0,2,2,2,0,0,2,2,0,0,1,1,0,1,1,1,
    0,0,0,0,0,0,0,0,1,1,2,2,1,1,2,2,    0,0,1,1,0,0,1,1,0,0,2,2,0,0,2,2,
    0,0,2,2,0,0,2,2,1,1,1,1,1,1,1,1,    0,0,1,1,0,0,1,1,2,2,1,1,2,2,1,1,
    0,0,0,0,0,0,0,0,1,1,1,1,2,2,2,2,    0,0,0,0,1,1,1,1,1,1,1,1,2,2,2,2,
    0,0,0,0,1,1,1,1,2,2,2,2,2,2,2,2,    0,0,1,2,0,0,1,2,0,0,1,2,0,0,1,2,
    0,1,1,2,0,1,1,2,0,1,1,2,0,1,1,2,    0,1,2,2,0,1,2,2,0,1,2,2,0,1,2,2,
    0,0,1,1,0,1,1,2,1,1,2,2,1,2,2,2,    0,0,1,1,2,0,0,1,2,2,0,0,2,2,2,0,
    0,0,0,1,0,0,1,1,0,1,1,2,1,1,2,2,    0,1,1,1,0,0,1,1,2,0,0,1,2,2,0,0,
    0,0,0,0,1,1,2,2,1,1,2,2,1,1,2,2,    0,0,2,2,0,0,2,2,0,0,2,2,1,1,1,1,
    0,1,1,1,0,1,1,1,0,2,2,2,0,2,2,2,    0,0,0,1,0,0,0,1,2,2,2,1,2,2,2,1,
    0,0,0,0,0,0,1,1,0,1,2,2,0,1,2,2,    0,0,0,0,1,1,0,0,2,2,1,0,2,2,1,0,
    0,1,2,2,0,1,2,2,0,0,1,1,0,0,0,0,    0,0,1,2,0,0,1,2,1,1,2,2,2,2,2,2,
    0,1,1,0,1,2,2,1,1,2,2,1,0,1,1,0,    0,0,0,0,0,1,1,0,1,2,2,1,1,2,2,1,
    0,0,2,2,1,1,0,2,1,1,0,2,0,0,2,2,    0,1,1,0,0,1,1,0,2,0,0,2,2,2,2,2,
    0,0,1,1,0,1,2,2,0,1,2,2,0,0,1,1,    0,0,0,0,2,0,0,0,2,2,1,1,2,2,2,1,
    0,0,0,0,0,0,0,2,1,1,2,2,1,2,2,2,    0,2,2,2,0,0,2,2,0,0,1,2,0,0,1,1,
    0,0,1,1,0,0,1,2,0,0,2,2,0,2,2,2,    0,1,2,0,0,1,2,0,0,1,2,0,0,1,2,0,
    0,0,0,0,1,1,1,1,2,2,2,2,0,0,0,0,    0,1,2,0,1,2,0,1,2,0,1,2,0,1,2,0,
    0,1,2,0,2,0,1,2,1,2,0,1,0,1,2,0,    0,0,1,1,2,2,0,0,1,1,2,2,0,0,1,1,
    0,0,1,1,1,1,2,2,2,2,0,0,0,0,1,1,    0,1,0,1,0,1,0,1,2,2,2,2,2,2,2,2,
    0,0,0,0,0,0,0,0,2,1,2,1,2,1,2,1,    0,0,2,2,1,1,2,2,0,0,2,2,1,1,2,2,
    0,0,2,2,0,0,1,1,0,0,2,2,0,0,1,1,    0,2,2,0,1,2,2,1,0,2,2,0,1,2,2,1,
    0,1,0,1,2,2,2,2,2,2,2,2,0,1,0,1,    0,0,0,0,2,1,2,1,2,1,2,1,2,1,2,1,
    0,1,0,1,0,1,0,1,0,1,0,1,2,2,2,2,    0,2,2,2,0,1,1,1,0,2,2,2,0,1,1,1,
    0,0,0,2,1,1,1,2,0,0,0,2,1,1,1,2,    0,0,0,0,2,1,1,2,2,1,1,2,2,1,1,2,
    0,2,2,2,0,1,1,1,0,1,1,1,0,2,2,2,    0,0,0,2,1,1,1,2,1,1,1,2,0,0,0,2,
    0,1,1,0,0,1,1,0,0,1,1,0,2,2,2,2,    0,0,0,0,0,0,0,0,2,1,1,2,2,1,1,2,
    0,1,1,0,0,1,1,0,2,2,2,2,2,2,2,2,    0,0,2,2,0,0,1,1,0,0,1,1,0,0,2,2,
    0,0,2,2,1,1,2,2,1,1,2,2,0,0,2,2,    0,0,0,0,0,0,0,0,0,0,0,0,2,1,1,2,
    0,0,0,2,0,0,0,1,0,0,0,2,0,0,0,1,    0,2,2,2,1,2,2,2,0,2,2,2,1,2,2,2,
    0,1,0,1,2,2,2,2,2,2,2,2,2,2,2,2,    0,1,1,1,2,0,1,1,2,2,0,1,2,2,2,0};

byte bgbbtj_bc7_anchor2[64]=
{   15,15,15,15,15,15,15,15,
    15,15,15,15,15,15,15,15,
    15, 2, 8, 2, 2, 8, 8,15,
     2, 8, 2, 2, 8, 8, 2, 2,
    15,15, 6, 8, 2, 8,15,15,
     2, 8, 2, 2, 2,15,15, 6,
     6, 2, 6, 8,15,15, 2, 2,
    15,15,15,15,15, 2, 2,15};

byte bgbbtj_bc7_anchor3a[64]=
{    3, 3,15,15, 8, 3,15,15,
     8, 8, 6, 6, 6, 5, 3, 3,
     3, 3, 8,15, 3, 3, 6,10,
     5, 8, 8, 6, 8, 5,15,15,
     8,15, 3, 5, 6,10, 8,15,
    15, 3,15, 5,15,15,15,15,
     3,15, 5, 5, 5, 8, 5,10,
     5,10, 8,13,15,12, 3, 3};

byte bgbbtj_bc7_anchor3b[64]=
{   15, 8, 8, 3,15,15, 3, 8,
    15,15,15,15,15,15,15, 8,
    15, 8,15, 3,15, 8,15, 8,
     3,15, 6,10,15,15,10, 8,
    15, 3,15,10,10, 8, 9,10,
     6,15, 8,15, 3, 6, 6, 8,
    15, 3,15,15,15,15,15,15,
    15,15,15,15, 3,15,15, 8};

#if 0
void BGBBTJ_BC7_EncodeBlock_SplitMinMaxClrYA(
	byte *pxy, byte *pxa,
	int *min_rgba, int *max_rgba, int *avg_rgba,
	int *rmcy, int *rncy, int *racy,
	int *rmca, int *rnca, int *raca,
	byte *rgba, int xstride, int ystride)
{
	int mcr, mcg, mcb, mca, mcy;
	int ncr, ncg, ncb, nca, ncy;
	int acr, acg, acb, aca, acy;
	int cr, cg, cb, ca, cy;
	int i, j, k, l;

	mcr=255; mcg=255; mcb=255; mca=255; mcy=255;
	ncr=  0; ncg=  0; ncb=  0; nca=  0; ncy=  0;
	acr=  0; acg=  0; acb=  0; aca=  0; acy=  0;
	
	for(i=0; i<4; i++)
		for(j=0; j<4; j++)
	{
		k=(i*ystride)+(j*xstride);
		cr=rgba[k+0];	cg=rgba[k+1];
		cb=rgba[k+2];	ca=rgba[k+3];
		cy=(cr+(2*cg)+cb)>>2;		//Luma

		pxa[i*4+j]=ca;
		pxy[i*4+j]=cy;

//		pxa[j*4+i]=ca;
//		pxy[j*4+i]=cy;

		//Find Mins/Maxs
		if(cy<mcy) { mcr=cr; mcg=cg; mcb=cb; mcy=cy; }
		if(cy>ncy) { ncr=cr; ncg=cg; ncb=cb; ncy=cy; }
		if(ca<mca) { mca=ca; }
		if(ca>nca) { nca=ca; }
		
		acr+=cr; acg+=cg;
		acb+=cb; acy+=cy;
	}
	
	min_rgba[0]=mcr;			min_rgba[1]=mcg;
	min_rgba[2]=mcb;			min_rgba[3]=mca;
	max_rgba[0]=ncr;			max_rgba[1]=ncg;
	max_rgba[2]=ncb;			max_rgba[3]=nca;
	avg_rgba[0]=(mcr+ncr)>>1;	avg_rgba[1]=(mcg+ncg)>>1;
	avg_rgba[2]=(mcb+ncb)>>1;	avg_rgba[3]=(mca+nca)>>1;
	
	*rmcy=mcy; *rncy=ncy; *racy=(mcy+ncy)>>1;
	*rmca=mca; *rnca=nca; *raca=(mca+nca)>>1;
}
#endif

#if 1
void BGBBTJ_BC7_EncodeBlock_SplitMinMaxClrYA(
	byte *pxy, byte *pxa,
	int *min_rgba, int *max_rgba, int *avg_rgba,
	int *rmcy, int *rncy, int *racy,
	int *rmca, int *rnca, int *raca,
	byte *rgba, int xstride, int ystride)
{
	byte pxyy[16], pxyr[16], pxyg[16], pxyb[16];
	byte *tpxy;

	int mcr, mcg, mcb, mca, mcy;
	int ncr, ncg, ncb, nca, ncy;
	int acr, acg, acb, aca, acy;
	int mcr3, mcg3, mcb3, mcy3;
	int ncr3, ncg3, ncb3, ncy3;
	int mcr3r, mcg3r, mcb3r, ncr3r, ncg3r, ncb3r, mcy3r, ncy3r;
	int mcr3g, mcg3g, mcb3g, ncr3g, ncg3g, ncb3g, mcy3g, ncy3g;
	int mcr3b, mcg3b, mcb3b, ncr3b, ncg3b, ncb3b, mcy3b, ncy3b;
	int dr, dg, db, da, dy;
	int cr, cg, cb, ca, cy;
	int cyr, cyg, cyb;
	int i, j, k, l;

	mcr=255; mcg=255; mcb=255; mca=255; mcy=255;
	ncr=  0; ncg=  0; ncb=  0; nca=  0; ncy=  0;
	acr=  0; acg=  0; acb=  0; aca=  0; acy=  0;
	
	mcr3r=255; mcg3r=255; mcb3r=255; mcy3r=255;
	ncr3r=  0; ncg3r=  0; ncb3r=  0; ncy3r=  0;
	mcr3g=255; mcg3g=255; mcb3g=255; mcy3g=255;
	ncr3g=  0; ncg3g=  0; ncb3g=  0; ncy3g=  0;
	mcr3b=255; mcg3b=255; mcb3b=255; mcy3b=255;
	ncr3b=  0; ncg3b=  0; ncb3b=  0; ncy3b=  0;

	for(i=0; i<4; i++)
		for(j=0; j<4; j++)
	{
		k=(i*ystride)+(j*xstride);
		cr=rgba[k+0];	cg=rgba[k+1];
		cb=rgba[k+2];	ca=rgba[k+3];
//		cy=(cr+(2*cg)+cb)>>2;		//Luma

		cy =(5*cr+8*cg+3*cb)>>4;		//Luma (Default)
//		cyr=(8*cr+5*cg+3*cb)>>4;
		cyr=(8*cr+2*cg+6*cb)>>4;
		cyg=(8*cr+6*cg+2*cb)>>4;
		cyb=(2*cr+6*cg+8*cb)>>4;

		pxa[i*4+j]=ca;
//		pxy[i*4+j]=cy;

		pxyy[i*4+j]=cy;
		pxyr[i*4+j]=cyr;
		pxyg[i*4+j]=cyg;
		pxyb[i*4+j]=cyb;

		//Find Mins/Maxs
		if(cy<mcy) { mcr=cr; mcg=cg; mcb=cb; mcy=cy; }
		if(cy>ncy) { ncr=cr; ncg=cg; ncb=cb; ncy=cy; }
		if(ca<mca) { mca=ca; }
		if(ca>nca) { nca=ca; }

		if(cyr<mcy3r) { mcr3r=cr; mcg3r=cg; mcb3r=cb; mcy3r=cyr; }
		if(cyr>ncy3r) { ncr3r=cr; ncg3r=cg; ncb3r=cb; ncy3r=cyr; }
		if(cyg<mcy3g) { mcr3g=cr; mcg3g=cg; mcb3g=cb; mcy3g=cyg; }
		if(cyg>ncy3g) { ncr3g=cr; ncg3g=cg; ncb3g=cb; ncy3g=cyg; }
		if(cyb<mcy3b) { mcr3b=cr; mcg3b=cg; mcb3b=cb; mcy3b=cyb; }
		if(cyb>ncy3b) { ncr3b=cr; ncg3b=cg; ncb3b=cb; ncy3b=cyb; }

		acr+=cr; acg+=cg;
		acb+=cb; acy+=cy;
	}

	tpxy=pxyy;
	
	dy=ncy-mcy;
	dr=ncy3r-mcy3r;
	dg=ncy3g-mcy3g;
	db=ncy3b-mcy3b;
	if(dr>dy)
	{
		mcr=mcr3r; mcg=mcg3r; mcb=mcb3r; mcy=mcy3r;
		ncr=ncr3r; ncg=ncg3r; ncb=ncb3r; ncy=ncy3r;
		tpxy=pxyr; dy=ncy-mcy;
	}
	if(dg>dy)
	{
		mcr=mcr3g; mcg=mcg3g; mcb=mcb3g; mcy=mcy3g;
		ncr=ncr3g; ncg=ncg3g; ncb=ncb3g; ncy=ncy3g;
		tpxy=pxyg; dy=ncy-mcy;
	}
	if(db>dy)
	{
		mcr=mcr3b; mcg=mcg3b; mcb=mcb3b; mcy=mcy3b;
		ncr=ncr3b; ncg=ncg3b; ncb=ncb3b; ncy=ncy3b;
		tpxy=pxyb; dy=ncy-mcy;
	}

#if 1
	acr=(acr+8)>>4;
	acg=(acg+8)>>4;
	acb=(acb+8)>>4;
	
	acy=(acr+(2*acg)+acb)>>2;
	mcy3=(mcr+(2*mcg)+mcb)>>2;
	ncy3=(ncr+(2*ncg)+ncb)>>2;

	l=(256*mcy3)/(acy+1);
	cr=(acr*l+128)>>8;
	cg=(acg*l+128)>>8;
	cb=(acb*l+128)>>8;
	mcr=(3*mcr+cr)>>2;
	mcg=(3*mcg+cg)>>2;
	mcb=(3*mcb+cb)>>2;
	
	l=(256*ncy3)/(acy+1);
	cr=(acr*l+128)>>8;
	cg=(acg*l+128)>>8;
	cb=(acb*l+128)>>8;
	ncr=(3*ncr+cr)>>2;
	ncg=(3*ncg+cg)>>2;
	ncb=(3*ncb+cb)>>2;
#endif

#if 1
	mcr=(mcr<0)?0:((mcr>255)?255:mcr);
	mcg=(mcg<0)?0:((mcg>255)?255:mcg);
	mcb=(mcb<0)?0:((mcb>255)?255:mcb);
	ncr=(ncr<0)?0:((ncr>255)?255:ncr);
	ncg=(ncg<0)?0:((ncg>255)?255:ncg);
	ncb=(ncb<0)?0:((ncb>255)?255:ncb);
#endif

	for(i=0; i<16; i++)
		{ pxy[i]=tpxy[i]; }

	min_rgba[0]=mcr;			min_rgba[1]=mcg;
	min_rgba[2]=mcb;			min_rgba[3]=mca;
	max_rgba[0]=ncr;			max_rgba[1]=ncg;
	max_rgba[2]=ncb;			max_rgba[3]=nca;
	avg_rgba[0]=(mcr+ncr)>>1;	avg_rgba[1]=(mcg+ncg)>>1;
	avg_rgba[2]=(mcb+ncb)>>1;	avg_rgba[3]=(mca+nca)>>1;
	
	*rmcy=mcy; *rncy=ncy; *racy=(mcy+ncy)>>1;
	*rmca=mca; *rnca=nca; *raca=(mca+nca)>>1;
}
#endif


void BGBBTJ_BC7_EncodeBlock_SplitMinMaxClrY(
	byte *pxy,
	int *min_rgba, int *max_rgba, int *avg_rgba,
	int *rmcy, int *rncy, int *racy,
	byte *rgba, int xstride, int ystride)
{
	int mcr, mcg, mcb, mca, mcy;
	int ncr, ncg, ncb, nca, ncy;
	int acr, acg, acb, aca, acy;
	int cr, cg, cb, ca, cy;
	int i, j, k, l;

	mcr=255; mcg=255; mcb=255; mca=255; mcy=255;
	ncr=  0; ncg=  0; ncb=  0; nca=  0; ncy=  0;
	acr=  0; acg=  0; acb=  0; aca=  0; acy=  0;
	
	for(i=0; i<4; i++)
		for(j=0; j<4; j++)
	{
		k=(i*ystride)+(j*xstride);
		cr=rgba[k+0];	cg=rgba[k+1];
		cb=rgba[k+2];	ca=rgba[k+3];
		cy=(cr+(2*cg)+cb)>>2;		//Luma

		pxy[i*4+j]=cy;
//		pxy[j*4+i]=cy;

		//Find Mins/Maxs
		if(cy<mcy) { mcr=cr; mcg=cg; mcb=cb; mcy=cy; }
		if(cy>ncy) { ncr=cr; ncg=cg; ncb=cb; ncy=cy; }
		if(ca<mca) { mca=ca; }
		if(ca>nca) { nca=ca; }
		
		acr+=cr; acg+=cg;
		acb+=cb; acy+=cy;
	}
	
	min_rgba[0]=mcr;			min_rgba[1]=mcg;
	min_rgba[2]=mcb;			min_rgba[3]=mca;
	max_rgba[0]=ncr;			max_rgba[1]=ncg;
	max_rgba[2]=ncb;			max_rgba[3]=nca;
	avg_rgba[0]=(mcr+ncr)>>1;	avg_rgba[1]=(mcg+ncg)>>1;
	avg_rgba[2]=(mcb+ncb)>>1;	avg_rgba[3]=(mca+nca)>>1;
	
	*rmcy=mcy; *rncy=ncy; *racy=(mcy+ncy)>>1;
//	*rmca=mca; *rnca=nca; *raca=(mca+nca)>>1;
}

void BGBBTJ_BC7_EncodeBlock_SplitMinMaxClrY2(
	byte *pxy,
	int *min_rgba, int *max_rgba, int *avg_rgba,
	int *rmcy, int *rncy, int *racy,
	byte *rgba, int xstride, int ystride)
{
	int mcr, mcg, mcb, mca, mcy;
	int ncr, ncg, ncb, nca, ncy;
	int acr, acg, acb, aca, acy;
	int cr, cg, cb, ca, cy;
	int i, j, k, l;

	mcr=255; mcg=255; mcb=255; mca=255; mcy=255;
	ncr=  0; ncg=  0; ncb=  0; nca=  0; ncy=  0;
	acr=  0; acg=  0; acb=  0; aca=  0; acy=  0;
	
	for(i=0; i<4; i++)
		for(j=0; j<4; j++)
	{
		k=(i*ystride)+(j*xstride);
		cr=rgba[k+0];	cg=rgba[k+1];
		cb=rgba[k+2];	ca=rgba[k+3];
		cy=(2*cr+(4*cg)+cb+ca)>>3;		//Luma

		pxy[i*4+j]=cy;
//		pxy[j*4+i]=cy;

		//Find Mins/Maxs
		if(cy<mcy) { mcr=cr; mcg=cg; mcb=cb; mcy=cy; mca=ca; }
		if(cy>ncy) { ncr=cr; ncg=cg; ncb=cb; ncy=cy; nca=ca; }
		acr+=cr; acg+=cg;
		acb+=cb; acy+=cy;
	}
	
	min_rgba[0]=mcr;			min_rgba[1]=mcg;
	min_rgba[2]=mcb;			min_rgba[3]=mca;
	max_rgba[0]=ncr;			max_rgba[1]=ncg;
	max_rgba[2]=ncb;			max_rgba[3]=nca;
	avg_rgba[0]=(mcr+ncr)>>1;	avg_rgba[1]=(mcg+ncg)>>1;
	avg_rgba[2]=(mcb+ncb)>>1;	avg_rgba[3]=(mca+nca)>>1;
	
	*rmcy=mcy; *rncy=ncy; *racy=(mcy+ncy)>>1;
}

void BGBBTJ_BC7_EncodeBlock_VecSwapRGB(int *clra, int *clrb)
{
	int t;
	t=clra[0]; clra[0]=clrb[0]; clrb[0]=t;
	t=clra[1]; clra[1]=clrb[1]; clrb[1]=t;
	t=clra[2]; clra[2]=clrb[2]; clrb[2]=t;
}

void BGBBTJ_BC7_EncodeBlock_VecSwapRGBA(int *clra, int *clrb)
{
	int t;
	t=clra[0]; clra[0]=clrb[0]; clrb[0]=t;
	t=clra[1]; clra[1]=clrb[1]; clrb[1]=t;
	t=clra[2]; clra[2]=clrb[2]; clrb[2]=t;
	t=clra[3]; clra[3]=clrb[3]; clrb[3]=t;
}

void BGBBTJ_BC7_EncodeBlock_VecSwapR(int *clra, int *clrb)
	{ int t; t=clra[0]; clra[0]=clrb[0]; clrb[0]=t; }
void BGBBTJ_BC7_EncodeBlock_VecSwapG(int *clra, int *clrb)
	{ int t; t=clra[1]; clra[1]=clrb[1]; clrb[1]=t; }
void BGBBTJ_BC7_EncodeBlock_VecSwapB(int *clra, int *clrb)
	{ int t; t=clra[2]; clra[2]=clrb[2]; clrb[2]=t; }
void BGBBTJ_BC7_EncodeBlock_VecSwapA(int *clra, int *clrb)
	{ int t; t=clra[3]; clra[3]=clrb[3]; clrb[3]=t; }

void BGBBTJ_BC7_EncodeBlock_VecInvertPixels(
	byte *pxv, int n, int min, int max)
{
	int i, j, k;
	
	for(i=0; i<n; i++)
		{ pxv[i]=max-(pxv[i]-min); }
}

void BGBBTJ_BC7_EncodeBlock_Mode4(byte *block,
	byte *rgba, int xstride, int ystride)
{
//	static const char idxtab[16]=
//		{ 1,1,1,1, 1,1,3,3, 2,2,0,0, 0,0,0,0 };
//	static const char idxtab2[16]=
//		{ 0,0,0,0, 2,2,3,3, 4,4,5,5, 1,1,1,1 };
	static const char idxtab[16]=
		{ 0,0,0,0, 0,0,1,1, 2,2,3,3, 3,3,3,3 };
	static const char idxtab2[16]=
		{ 0,0,0,0, 0,1,2,3, 4,5,6,7, 7,7,7,7 };
	
	BGBBTJ_BitStream bits;

	byte pxa[16], pxy[16];
	int p0, p1, p2, p3, p4, p5, p6, p7;
	int l0, l1, l2, l3a, l3b;
	int min[4], mcy, mca;
	int max[4], ncy, nca;
	int avg[4], acy, aca;
	int acr, acg, acb, ixb;
	int cr, cg, cb, ca, cy;
	int i, j, k, l;

	BGBBTJ_BC7_EncodeBlock_SplitMinMaxClrYA(
		pxy, pxa, min, max, avg,
		&mcy, &ncy, &acy,
		&mca, &nca, &aca,
		rgba, xstride, ystride);

	ixb=1;

	BGBBTJ_BitsLE_ClearSetupWrite(&bits, block, 16);
	BGBBTJ_BitsLE_WriteBits(&bits, 16, 5);
	BGBBTJ_BitsLE_WriteBits(&bits, 0, 2);
	BGBBTJ_BitsLE_WriteBits(&bits, ixb, 1);

	if(pxy[0]>=acy)
	{
		BGBBTJ_BC7_EncodeBlock_VecSwapRGB(min, max);
		BGBBTJ_BC7_EncodeBlock_VecInvertPixels(pxy, 16, mcy, ncy);
	}

	if(pxa[0]>=aca)
	{
		BGBBTJ_BC7_EncodeBlock_VecSwapA(min, max);
		BGBBTJ_BC7_EncodeBlock_VecInvertPixels(pxa, 16, mca, nca);
	}

	BGBBTJ_BitsLE_WriteBits(&bits, (min[0])>>3, 5);
	BGBBTJ_BitsLE_WriteBits(&bits, (max[0])>>3, 5);
	BGBBTJ_BitsLE_WriteBits(&bits, (min[1])>>3, 5);
	BGBBTJ_BitsLE_WriteBits(&bits, (max[1])>>3, 5);
	BGBBTJ_BitsLE_WriteBits(&bits, (min[2])>>3, 5);
	BGBBTJ_BitsLE_WriteBits(&bits, (max[2])>>3, 5);
	BGBBTJ_BitsLE_WriteBits(&bits, (min[3])>>2, 6);
	BGBBTJ_BitsLE_WriteBits(&bits, (max[3])>>2, 6);

//	l0=49152/(nca-aca+1);		//Fix-Point Scale (Alpha)
	l0=32768/(nca-aca+1);		//Fix-Point Scale (Alpha)
	l1=32768/(ncy-acy+1);		//Fix-Point Scale (Luma)
	l2=65536-2048;
	l3a=65536;

	if(ixb)
	{
		p0=idxtab[((pxa[0]-aca)*l0+l3a)>>13];
		BGBBTJ_BitsLE_WriteBit(&bits, p0);
		for(i=1; i<16; i++)
		{
			p0=idxtab[((pxa[i]-aca)*l0+l3a)>>13];
			BGBBTJ_BitsLE_Write2Bits(&bits, p0);
		}

		p0=idxtab2[((pxy[0]-acy)*l1+l3a)>>13];
		BGBBTJ_BitsLE_Write2Bits(&bits, p0);
		for(i=1; i<16; i++)
		{
			p0=idxtab2[((pxy[i]-acy)*l1+l3a)>>13];
			BGBBTJ_BitsLE_Write3Bits(&bits, p0);
		}
	}else
	{
		p0=idxtab[((pxy[0]-acy)*l1+l3a)>>13];
		BGBBTJ_BitsLE_WriteBit(&bits, p0);
		for(i=1; i<16; i++)
		{
			p0=idxtab[((pxy[i]-acy)*l1+l3a)>>13];
			BGBBTJ_BitsLE_Write2Bits(&bits, p0);
		}

		p0=idxtab2[((pxa[0]-aca)*l0+l3a)>>13];
		BGBBTJ_BitsLE_Write2Bits(&bits, p0);
		for(i=1; i<16; i++)
		{
			p0=idxtab2[((pxa[i]-aca)*l0+l3a)>>13];
			BGBBTJ_BitsLE_Write3Bits(&bits, p0);
		}
	}

	BGBBTJ_BitsLE_FlushBits(&bits);
}

void BGBBTJ_BC7_EncodeBlock_Mode5(byte *block,
	byte *rgba, int xstride, int ystride)
{
//	static const char idxtab[16]=
//		{ 1,1,1,1, 1,1,3,3, 2,2,0,0, 0,0,0,0 };
//	static const char idxtab2[16]=
//		{ 0,0,0,0, 2,2,3,3, 4,4,5,5, 1,1,1,1 };
	static const char idxtab[16]=
		{ 0,0,0,0, 0,0,1,1, 2,2,3,3, 3,3,3,3 };
	
	BGBBTJ_BitStream bits;

	byte pxa[16], pxy[16];
	int p0, p1, p2, p3, p4, p5, p6, p7;
	int l0, l1, l2, l3a, l3b;
	int min[4], mcy, mca;
	int max[4], ncy, nca;
	int avg[4], acy, aca;
	int acr, acg, acb;
	int cr, cg, cb, ca, cy;
	int i, j, k, l;

	BGBBTJ_BC7_EncodeBlock_SplitMinMaxClrYA(
		pxy, pxa, min, max, avg,
		&mcy, &ncy, &acy,
		&mca, &nca, &aca,
		rgba, xstride, ystride);

	BGBBTJ_BitsLE_ClearSetupWrite(&bits, block, 16);
	BGBBTJ_BitsLE_WriteBits(&bits, 32, 6);
	BGBBTJ_BitsLE_WriteBits(&bits, 0, 2);

	if(pxy[0]>=acy)
	{
		BGBBTJ_BC7_EncodeBlock_VecSwapRGB(min, max);
		BGBBTJ_BC7_EncodeBlock_VecInvertPixels(pxy, 16, mcy, ncy);
	}

	if(pxa[0]>=aca)
	{
		BGBBTJ_BC7_EncodeBlock_VecSwapA(min, max);
		BGBBTJ_BC7_EncodeBlock_VecInvertPixels(pxa, 16, mca, nca);
	}

	BGBBTJ_BitsLE_WriteBits(&bits, (min[0])>>1, 7);
	BGBBTJ_BitsLE_WriteBits(&bits, (max[0])>>1, 7);
	BGBBTJ_BitsLE_WriteBits(&bits, (min[1])>>1, 7);
	BGBBTJ_BitsLE_WriteBits(&bits, (max[1])>>1, 7);
	BGBBTJ_BitsLE_WriteBits(&bits, (min[2])>>1, 7);
	BGBBTJ_BitsLE_WriteBits(&bits, (max[2])>>1, 7);
	BGBBTJ_BitsLE_WriteBits(&bits, min[3], 8);
	BGBBTJ_BitsLE_WriteBits(&bits, max[3], 8);

//	l0=49152/(nca-aca+1);		//Fix-Point Scale (Alpha)
	l0=32768/(nca-aca+1);		//Fix-Point Scale (Alpha)
	l1=32768/(ncy-acy+1);		//Fix-Point Scale (Luma)
	l2=65536-2048;
	l3a=65536;

	p0=idxtab[((pxy[0]-acy)*l1+l3a)>>13];
	BGBBTJ_BitsLE_WriteBit(&bits, p0);
	for(i=1; i<16; i++)
	{
		p0=idxtab[((pxy[i]-acy)*l1+l3a)>>13];
		BGBBTJ_BitsLE_Write2Bits(&bits, p0);
	}

	p0=idxtab[((pxa[0]-aca)*l0+l3a)>>13];
	BGBBTJ_BitsLE_WriteBit(&bits, p0);
	for(i=1; i<16; i++)
	{
		p0=idxtab[((pxa[i]-aca)*l0+l3a)>>13];
		BGBBTJ_BitsLE_Write2Bits(&bits, p0);
	}

	BGBBTJ_BitsLE_FlushBits(&bits);
}

void BGBBTJ_BC7_EncodeBlock_Mode6(byte *block,
	byte *rgba, int xstride, int ystride)
{
	static const char idxtab[32]=
		{  0,  0,  0,  0,  0,  0,  0,  0,
		   0,  1,  2,  3,  4,  5,  6,  7,
		   8,  9, 10, 11, 12, 13, 14, 15,
		  15, 15, 15, 15, 15, 15, 15, 15};
	
	BGBBTJ_BitStream bits;

	byte pxy[16];
	int p0, p1, p2, p3, p4, p5, p6, p7;
	int l0, l1, l2, l3a, l3b;
	int min[4], mcy, mca;
	int max[4], ncy, nca;
	int avg[4], acy, aca;
	int acr, acg, acb;
	int cr, cg, cb, ca, cy;
	int i, j, k, l;

	BGBBTJ_BC7_EncodeBlock_SplitMinMaxClrY2(
		pxy, min, max, avg,
		&mcy, &ncy, &acy,
		rgba, xstride, ystride);

	BGBBTJ_BitsLE_ClearSetupWrite(&bits, block, 16);
	BGBBTJ_BitsLE_WriteBits(&bits, 64, 7);

	if(pxy[0]>=acy)
	{
		BGBBTJ_BC7_EncodeBlock_VecSwapRGBA(min, max);
		BGBBTJ_BC7_EncodeBlock_VecInvertPixels(pxy, 16, mcy, ncy);
	}

	BGBBTJ_BitsLE_WriteBits(&bits, (min[0])>>1, 7);
	BGBBTJ_BitsLE_WriteBits(&bits, (max[0])>>1, 7);
	BGBBTJ_BitsLE_WriteBits(&bits, (min[1])>>1, 7);
	BGBBTJ_BitsLE_WriteBits(&bits, (max[1])>>1, 7);
	BGBBTJ_BitsLE_WriteBits(&bits, (min[2])>>1, 7);
	BGBBTJ_BitsLE_WriteBits(&bits, (max[2])>>1, 7);
	BGBBTJ_BitsLE_WriteBits(&bits, (min[3])>>1, 7);
	BGBBTJ_BitsLE_WriteBits(&bits, (max[3])>>1, 7);
	BGBBTJ_BitsLE_WriteBits(&bits, min[1], 1);
	BGBBTJ_BitsLE_WriteBits(&bits, max[1], 1);

//	l0=49152/(nca-aca+1);		//Fix-Point Scale (Alpha)
	l1=32768/(ncy-acy+1);		//Fix-Point Scale (Luma)
	l2=65536-2048;
	l3a=65536;

	p0=idxtab[((pxy[0]-acy)*l1+l3a)>>12];
	BGBBTJ_BitsLE_Write3Bits(&bits, p0);
	for(i=1; i<16; i++)
	{
		p0=idxtab[((pxy[i]-acy)*l1+l3a)>>12];
		BGBBTJ_BitsLE_Write4Bits(&bits, p0);
	}

	BGBBTJ_BitsLE_FlushBits(&bits);
}

void BGBBTJ_BC7_EncodeBlock(byte *block,
	byte *rgba, int xstride, int ystride)
{
//	BGBBTJ_BC7_EncodeBlock_Mode4(block, rgba, xstride, ystride);
	BGBBTJ_BC7_EncodeBlock_Mode5(block, rgba, xstride, ystride);
//	BGBBTJ_BC7_EncodeBlock_Mode6(block, rgba, xstride, ystride);
}

BGBBTJ_API void BGBBTJ_BC7_EncodeImageBC7(byte *block,
	byte *rgba, int xs, int ys, int stride)
{
	int xs1, ys1;
	int i, j;
	
//	xs1=xs>>2; ys1=ys>>2;
	xs1=(xs+3)>>2; ys1=(ys+3)>>2;
	for(i=0; i<ys1; i++)
		for(j=0; j<xs1; j++)
	{
		BGBBTJ_BC7_EncodeBlock(
			block+(i*xs1+j)*16,
			rgba+(i*4*xs+j*4)*stride,
			stride, xs*stride);
	}
}

int BGBBTJ_BC7_CalcBlockError(
	byte *rgba0, int xstride0, int ystride0,
	byte *rgba1, int xstride1, int ystride1)
{
	int e, dr, dg, db, da;
	int p0, p1;
	int i, j, k;
	
	e=0;
	for(i=0; i<4; i++)
		for(j=0; j<4; j++)
	{
		p0=i*ystride0+j*xstride0;
		p1=i*ystride1+j*xstride1;
		dr=rgba0[p0+0]-rgba1[p1+0];
		dg=rgba0[p0+1]-rgba1[p1+1];
		db=rgba0[p0+2]-rgba1[p1+2];
		da=rgba0[p0+3]-rgba1[p1+3];
		e+=dr*dr+dg*dg+db*db+da*da;
	}
	return(e);
}

void BGBBTJ_BC7_EncodeBlockBest(byte *block,
	byte *rgba, int xstride, int ystride)
{
	byte tblk[16], tpix[16*4];
	int terr, berr;

	BGBBTJ_BC7_EncodeBlock_Mode5(block, rgba, xstride, ystride);
	BGBBTJ_BC7_DecodeBlock(block, tpix, 4, 16, 0);
	berr=BGBBTJ_BC7_CalcBlockError(rgba, xstride, ystride, tpix, 4, 16);
	if(berr<=0)return;
	
	BGBBTJ_BC7_EncodeBlock_Mode4(tblk, rgba, xstride, ystride);
	BGBBTJ_BC7_DecodeBlock(tblk, tpix, 4, 16, 0);
	terr=BGBBTJ_BC7_CalcBlockError(rgba, xstride, ystride, tpix, 4, 16);
	if(terr<berr)
		{ memcpy(block, tblk, 16); berr=terr; }

	BGBBTJ_BC7_EncodeBlock_Mode6(tblk, rgba, xstride, ystride);
	BGBBTJ_BC7_DecodeBlock(tblk, tpix, 4, 16, 0);
	terr=BGBBTJ_BC7_CalcBlockError(rgba, xstride, ystride, tpix, 4, 16);
	if(terr<berr)
		{ memcpy(block, tblk, 16); berr=terr; }

#if 1
	BGBBTJ_BC7_EncodeBlockBest_Part2(tblk, rgba, xstride, ystride);
	BGBBTJ_BC7_DecodeBlock(tblk, tpix, 4, 16, 0);
	terr=BGBBTJ_BC7_CalcBlockError(rgba, xstride, ystride, tpix, 4, 16);
	if(terr<berr)
		{ memcpy(block, tblk, 16); berr=terr; }
#endif

#if 0
	BGBBTJ_BC7_EncodeBlockBest_Part3(tblk, rgba, xstride, ystride);
	BGBBTJ_BC7_DecodeBlock(tblk, tpix, 4, 16, 0);
	terr=BGBBTJ_BC7_CalcBlockError(rgba, xstride, ystride, tpix, 4, 16);
	if(terr<berr)
		{ memcpy(block, tblk, 16); berr=terr; }
#endif

//	BGBBTJ_BC7_EncodeBlock_Mode4(block, rgba, xstride, ystride);
//	BGBBTJ_BC7_EncodeBlock_Mode5(block, rgba, xstride, ystride);
//	BGBBTJ_BC7_EncodeBlock_Mode6(block, rgba, xstride, ystride);
}

BGBBTJ_API void BGBBTJ_BC7_EncodeImageBestBC7(byte *block,
	byte *rgba, int xs, int ys, int stride)
{
	int stat[8];
	int xs1, ys1;
	int i, j, k;
	
	for(i=0; i<8; i++)
		stat[i]=0;
	
//	xs1=xs>>2; ys1=ys>>2;
	xs1=(xs+3)>>2; ys1=(ys+3)>>2;
	for(i=0; i<ys1; i++)
		for(j=0; j<xs1; j++)
	{
		BGBBTJ_BC7_EncodeBlockBest(
			block+(i*xs1+j)*16,
			rgba+(i*4*xs+j*4)*stride,
			stride, xs*stride);
		
		k=BGBBTJ_BC7_GetBlockMode(block+(i*xs1+j)*16);
		stat[k]++;
	}
	
	printf("BGBBTJ_BC7_EncodeImageBestBC7: "
			"Stats=(%d %d %d %d  %d %d %d %d)\n",
		stat[0], stat[1], stat[2], stat[3],
		stat[4], stat[5], stat[6], stat[7]);
}

void BGBBTJ_BC7_EncodeBlock_YUV420(byte *block,
	short *pxy, short *pxu, short *pxv, int vfl)
{
	byte pxb[16*4];
	int cy, cu, cv;
	int cy0, cy1, cy2, cy3;
	int cr0, cg0, cb0;
	int cr1, cg1, cb1;
	int cr2, cg2, cb2;
	int cr3, cg3, cb3;
	int u1, v1;
	int l0, l1, l2, l3;
	int i, j, k;

	for(i=0; i<2; i++)
		for(j=0; j<2; j++)
	{
		cy0=pxy[((i*2+0)*8)+(j*2+0)];
		cy1=pxy[((i*2+0)*8)+(j*2+1)];
		cy2=pxy[((i*2+1)*8)+(j*2+0)];
		cy3=pxy[((i*2+1)*8)+(j*2+1)];

		cu=pxu[i*8+j];
		cv=pxv[i*8+j];
		u1=cu-128; v1=cv-128;

		l0=359*v1;
		l1=-88*u1-183*v1;
		l2=454*u1;

//		cr0=(256*cy0+l0)>>8;	cg0=(256*cy0+l1)>>8;	cb0=(256*cy0+l2)>>8;
//		cr1=(256*cy1+l0)>>8;	cg1=(256*cy1+l1)>>8;	cb1=(256*cy1+l2)>>8;
//		cr2=(256*cy2+l0)>>8;	cg2=(256*cy2+l1)>>8;	cb2=(256*cy2+l2)>>8;
//		cr3=(256*cy3+l0)>>8;	cg3=(256*cy3+l1)>>8;	cb3=(256*cy3+l2)>>8;

		cr0=(256*cy0+l0+128)>>8;	cg0=(256*cy0+l1+128)>>8;
		cb0=(256*cy0+l2+128)>>8;	cr1=(256*cy1+l0+128)>>8;
		cg1=(256*cy1+l1+128)>>8;	cb1=(256*cy1+l2+128)>>8;
		cr2=(256*cy2+l0+128)>>8;	cg2=(256*cy2+l1+128)>>8;
		cb2=(256*cy2+l2+128)>>8;	cr3=(256*cy3+l0+128)>>8;
		cg3=(256*cy3+l1+128)>>8;	cb3=(256*cy3+l2+128)>>8;

		cr0=(cr0<0)?0:((cr0>255)?255:cr0);
		cg0=(cg0<0)?0:((cg0>255)?255:cg0);
		cb0=(cb0<0)?0:((cb0>255)?255:cb0);
		cr1=(cr1<0)?0:((cr1>255)?255:cr1);
		cg1=(cg1<0)?0:((cg1>255)?255:cg1);
		cb1=(cb1<0)?0:((cb1>255)?255:cb1);
		cr2=(cr2<0)?0:((cr2>255)?255:cr2);
		cg2=(cg2<0)?0:((cg2>255)?255:cg2);
		cb2=(cb2<0)?0:((cb2>255)?255:cb2);
		cr3=(cr3<0)?0:((cr3>255)?255:cr3);
		cg3=(cg3<0)?0:((cg3>255)?255:cg3);
		cb3=(cb3<0)?0:((cb3>255)?255:cb3);

		if(vfl&1)
		{
			l0=(3-(i*2+0))*16+(j*2+0)*4;
			l1=(3-(i*2+0))*16+(j*2+1)*4;
			l2=(3-(i*2+1))*16+(j*2+0)*4;
			l3=(3-(i*2+1))*16+(j*2+1)*4;
		}else
		{
			l0=(i*2+0)*16+(j*2+0)*4;
			l1=(i*2+0)*16+(j*2+1)*4;
			l2=(i*2+1)*16+(j*2+0)*4;
			l3=(i*2+1)*16+(j*2+1)*4;
		}
		pxb[l0+0]=cr0;	pxb[l0+1]=cg0;
		pxb[l0+2]=cb0;	pxb[l0+3]=255;
		pxb[l1+0]=cr1;	pxb[l1+1]=cg1;
		pxb[l1+2]=cb1;	pxb[l1+3]=255;
		pxb[l2+0]=cr2;	pxb[l2+1]=cg2;
		pxb[l2+2]=cb2;	pxb[l2+3]=255;
		pxb[l3+0]=cr3;	pxb[l3+1]=cg3;
		pxb[l3+2]=cb3;	pxb[l3+3]=255;
	}

	BGBBTJ_BC7_EncodeBlock(
		block, pxb, 4, 4*4);
}


int BGBBTJ_BC7_DecodeBlock_ReadMode(BGBBTJ_BitStream *bits)
{
	int i, j;

	i=BGBBTJ_BitsLE_Peek8Bits(bits);
	if(i)
	{
		if(i&15)
		{
			if(i&3)		{ j=(i&1)?0:1; }
			else		{ j=(i&4)?2:3; }
		}else
		{
			if(i&63)	{ j=(i&16)?4:5; }
			else		{ j=(i&64)?6:7; }
		}
		BGBBTJ_BitsLE_ReadBits(bits, j+1);
		return(j);
	}else
	{
		BGBBTJ_BitsLE_Read8Bits(bits);
//		i=BGBBTJ_BitsLE_Peek8Bits(bits);
#if 1
		i=8; j=BGBBTJ_BitsLE_ReadBit(bits);
		while(!j && (i<32))
			{ i++; j=BGBBTJ_BitsLE_ReadBit(bits); }
		return(i);
#endif
	}

#if 0
	i=0; j=BGBBTJ_BitsLE_ReadBit(bits);
	while(!j && (i<32))
		{ i++; j=BGBBTJ_BitsLE_ReadBit(bits); }
	return(i);
#endif
}

int bgbbtj_bc7_interpolate(int e0, int e1, int idx, int prec)
{
	int i, w;

	switch(prec)
	{
	case 2:
		w=bgbbtj_bc7_weights2[idx];
		i=(((64-w)*e0+w*e1+32)>>6);
		break;
	case 3:
		w=bgbbtj_bc7_weights3[idx];
		i=(((64-w)*e0+w*e1+32)>>6);
		break;
	case 4:
		w=bgbbtj_bc7_weights4[idx];
		i=(((64-w)*e0+w*e1+32)>>6);
		break;
	default:
		i=0; break;
	}
	return(i);
}

void BGBBTJ_BC7_DecodeBlock_Mode4(
	BGBBTJ_BitStream *bits,
	byte *rgba, int xstride, int ystride, int flags)
{
	byte blkb[16*4];
	byte clrb[8*4];
	int rot, idxm;
	int r0, r1, g0, g1, b0, b1, a0, a1;
	int p;
	int i, j, k, l;
	
	rot=BGBBTJ_BitsLE_Read2Bits(bits);
	idxm=BGBBTJ_BitsLE_ReadBit(bits);

	r0=BGBBTJ_BitsLE_Read5Bits(bits);
	r1=BGBBTJ_BitsLE_Read5Bits(bits);
	g0=BGBBTJ_BitsLE_Read5Bits(bits);
	g1=BGBBTJ_BitsLE_Read5Bits(bits);
	b0=BGBBTJ_BitsLE_Read5Bits(bits);
	b1=BGBBTJ_BitsLE_Read5Bits(bits);
	a0=BGBBTJ_BitsLE_Read6Bits(bits);
	a1=BGBBTJ_BitsLE_Read6Bits(bits);
	
	r0=(r0<<3)|(r0>>2); g0=(g0<<3)|(g0>>2);
	b0=(b0<<3)|(b0>>2); a0=(a0<<2)|(a0>>4);
	r1=(r1<<3)|(r1>>2); g1=(g1<<3)|(g1>>2);
	b1=(b1<<3)|(b1>>2); a1=(a1<<2)|(a1>>4);
	
	if(idxm==0)
	{
		for(i=0; i<4; i++)
		{
			clrb[i*4+0]=bgbbtj_bc7_interpolate(r0, r1, i, 2);
			clrb[i*4+1]=bgbbtj_bc7_interpolate(g0, g1, i, 2);
			clrb[i*4+2]=bgbbtj_bc7_interpolate(b0, b1, i, 2);
		}

		for(i=0; i<8; i++)
		{
			clrb[i*4+3]=bgbbtj_bc7_interpolate(a0, a1, i, 3);
		}
		
		p=BGBBTJ_BitsLE_ReadBit(bits);
		blkb[0*4+0]=clrb[p*4+0];
		blkb[0*4+1]=clrb[p*4+1];
		blkb[0*4+2]=clrb[p*4+2];
		for(i=1; i<16; i++)
		{
			p=BGBBTJ_BitsLE_Read2Bits(bits);
			blkb[i*4+0]=clrb[p*4+0];
			blkb[i*4+1]=clrb[p*4+1];
			blkb[i*4+2]=clrb[p*4+2];
		}

		p=BGBBTJ_BitsLE_Read2Bits(bits);
		blkb[0*4+3]=clrb[p*4+3];
		for(i=1; i<16; i++)
		{
			p=BGBBTJ_BitsLE_Read3Bits(bits);
			blkb[i*4+3]=clrb[p*4+3];
		}
	}else
	{
		for(i=0; i<8; i++)
		{
			clrb[i*4+0]=bgbbtj_bc7_interpolate(r0, r1, i, 3);
			clrb[i*4+1]=bgbbtj_bc7_interpolate(g0, g1, i, 3);
			clrb[i*4+2]=bgbbtj_bc7_interpolate(b0, b1, i, 3);
		}

		for(i=0; i<4; i++)
		{
			clrb[i*4+3]=bgbbtj_bc7_interpolate(a0, a1, i, 2);
		}
		
		p=BGBBTJ_BitsLE_ReadBit(bits);
		blkb[0*4+3]=clrb[p*4+3];
		for(i=1; i<16; i++)
		{
			p=BGBBTJ_BitsLE_Read2Bits(bits);
			blkb[i*4+3]=clrb[p*4+3];
		}

		p=BGBBTJ_BitsLE_Read2Bits(bits);
		blkb[0*4+0]=clrb[p*4+0];
		blkb[0*4+1]=clrb[p*4+1];
		blkb[0*4+2]=clrb[p*4+2];
		for(i=1; i<16; i++)
		{
			p=BGBBTJ_BitsLE_Read3Bits(bits);
			blkb[i*4+0]=clrb[p*4+0];
			blkb[i*4+1]=clrb[p*4+1];
			blkb[i*4+2]=clrb[p*4+2];
		}
	}
	
	switch(rot)
	{
	case 0: break;
	case 1: case 2: case 3:
		k=rot-1;
		for(i=0; i<16; i++)
			{ j=blkb[i*4+k]; blkb[i*4+k]=blkb[i*4+3]; blkb[i*4+3]=j; }
		break;
	default:
		break;
	}

	if(flags&1)
	{
		for(i=0; i<4; i++)
			for(j=0; j<4; j++)
		{
			k=i*ystride+j*xstride; l=(i*4+j)*4;
			rgba[k+2]=blkb[l+0]; rgba[k+1]=blkb[l+1];
			rgba[k+0]=blkb[l+2]; rgba[k+3]=blkb[l+3];
		}
	}else
	{
		for(i=0; i<4; i++)
			for(j=0; j<4; j++)
		{
			k=i*ystride+j*xstride; l=(i*4+j)*4;
			rgba[k+0]=blkb[l+0]; rgba[k+1]=blkb[l+1];
			rgba[k+2]=blkb[l+2]; rgba[k+3]=blkb[l+3];
		}
	}
}

void BGBBTJ_BC7_DecodeBlock_Mode5(
	BGBBTJ_BitStream *bits,
	byte *rgba, int xstride, int ystride, int flags)
{
	byte blkb[16*4];
	byte clrb[8*4];
	byte *bp0, *bp1;
	int rot, idxm;
	int r0, r1, g0, g1, b0, b1, a0, a1;
	int p, p0, p1, p2, p3;
	int i0, i1, i2, i3;
	int i, j, k, l;
	
	rot=BGBBTJ_BitsLE_Read2Bits(bits);
#if 0
	r0=BGBBTJ_BitsLE_Read7Bits(bits);
	r1=BGBBTJ_BitsLE_Read7Bits(bits);
	g0=BGBBTJ_BitsLE_Read7Bits(bits);
	g1=BGBBTJ_BitsLE_Read7Bits(bits);
	b0=BGBBTJ_BitsLE_Read7Bits(bits);
	b1=BGBBTJ_BitsLE_Read7Bits(bits);
	a0=BGBBTJ_BitsLE_Read8Bits(bits);
	a1=BGBBTJ_BitsLE_Read8Bits(bits);
#endif
#if 1
	p0=BGBBTJ_BitsLE_Read14Bits(bits);
	p1=BGBBTJ_BitsLE_Read14Bits(bits);
	p2=BGBBTJ_BitsLE_Read14Bits(bits);
	p3=BGBBTJ_BitsLE_Read16Bits(bits);
	r0=p0&127; r1=(p0>>7);	g0=p1&127; g1=(p1>>7);
	b0=p2&127; b1=(p2>>7);	a0=p3&255; a1=(p3>>8);
#endif

	r0=(r0<<1)|(r0>>6); r1=(r1<<1)|(r1>>6);
	g0=(g0<<1)|(g0>>6); g1=(g1<<1)|(g1>>6);
	b0=(b0<<1)|(b0>>6); b1=(b1<<1)|(b1>>6);

	if((r0==r1) && (g0==g1) && (b0==b1) && (a0==a1))
	{
		if(flags&1)
		{
#if defined(X86) || defined(X86_64)
			p0=(a0<<24)|(r0<<16)|(g0<<8)|(b0);
			for(i=0; i<4; i++)
			{
				j=i*ystride;
				i0=j+0*xstride;		i1=j+1*xstride;
				i2=j+2*xstride;		i3=j+3*xstride;
				*(u32 *)(rgba+i0)=p0;	*(u32 *)(rgba+i1)=p0;
				*(u32 *)(rgba+i2)=p0;	*(u32 *)(rgba+i3)=p0;
			}
#else
			for(i=0; i<4; i++)
			{
				j=i*ystride;
				i0=j+0*xstride;		i1=j+1*xstride;
				i2=j+2*xstride;		i3=j+3*xstride;
				rgba[i0+2]=r0; rgba[i0+1]=g0;
				rgba[i0+0]=b0; rgba[i0+3]=a0;
				rgba[i1+2]=r0; rgba[i1+1]=g0;
				rgba[i1+0]=b0; rgba[i1+3]=a0;
				rgba[i2+2]=r0; rgba[i2+1]=g0;
				rgba[i2+0]=b0; rgba[i2+3]=a0;
				rgba[i3+2]=r0; rgba[i3+1]=g0;
				rgba[i3+0]=b0; rgba[i3+3]=a0;
			}
#endif
		}else
		{
			for(i=0; i<4; i++)
			{
				j=i*ystride;
				i0=j+0*xstride;		i1=j+1*xstride;
				i2=j+2*xstride;		i3=j+3*xstride;
				rgba[i0+0]=r0; rgba[i0+1]=g0;
				rgba[i0+2]=b0; rgba[i0+3]=a0;
				rgba[i1+0]=r0; rgba[i1+1]=g0;
				rgba[i1+2]=b0; rgba[i1+3]=a0;
				rgba[i2+0]=r0; rgba[i2+1]=g0;
				rgba[i2+2]=b0; rgba[i2+3]=a0;
				rgba[i3+0]=r0; rgba[i3+1]=g0;
				rgba[i3+2]=b0; rgba[i3+3]=a0;
			}
		}
		return;
	}

	for(i=0; i<4; i++)
	{
		clrb[i*4+0]=bgbbtj_bc7_interpolate(r0, r1, i, 2);
		clrb[i*4+1]=bgbbtj_bc7_interpolate(g0, g1, i, 2);
		clrb[i*4+2]=bgbbtj_bc7_interpolate(b0, b1, i, 2);
		clrb[i*4+3]=bgbbtj_bc7_interpolate(a0, a1, i, 2);
	}
	
	if((r0==r1) && (g0==g1) && (b0==b1))
	{
		for(i=0; i<16; i++)
		{
			blkb[i*4+0]=r0;
			blkb[i*4+1]=g0;
			blkb[i*4+2]=b0;
		}
		if(a0!=a1)
			{ p=BGBBTJ_BitsLE_ReadBits(bits, 31); }
	}else
	{
#if 1
		p=BGBBTJ_BitsLE_Read7Bits(bits);
		p0=(p&1)*4;			p1=((p>>1)&3)*4;
		p2=((p>>3)&3)*4;	p3=((p>>5)&3)*4;
		i0=0*4;		i1=1*4;
		i2=2*4;		i3=3*4;
		blkb[i0+0]=clrb[p0+0];	blkb[i0+1]=clrb[p0+1];
		blkb[i0+2]=clrb[p0+2];	blkb[i1+0]=clrb[p1+0];
		blkb[i1+1]=clrb[p1+1];	blkb[i1+2]=clrb[p1+2];
		blkb[i2+0]=clrb[p2+0];	blkb[i2+1]=clrb[p2+1];
		blkb[i2+2]=clrb[p2+2];	blkb[i3+0]=clrb[p3+0];
		blkb[i3+1]=clrb[p3+1];	blkb[i3+2]=clrb[p3+2];
		for(i=1; i<4; i++)
		{
			p=BGBBTJ_BitsLE_Read8Bits(bits);
			p0=(p&3)*4;			p1=((p>>2)&3)*4;
			p2=((p>>4)&3)*4;	p3=((p>>6)&3)*4;
			i0=(i*4+0)*4;		i1=(i*4+1)*4;
			i2=(i*4+2)*4;		i3=(i*4+3)*4;
			blkb[i0+0]=clrb[p0+0];	blkb[i0+1]=clrb[p0+1];
			blkb[i0+2]=clrb[p0+2];	blkb[i1+0]=clrb[p1+0];
			blkb[i1+1]=clrb[p1+1];	blkb[i1+2]=clrb[p1+2];
			blkb[i2+0]=clrb[p2+0];	blkb[i2+1]=clrb[p2+1];
			blkb[i2+2]=clrb[p2+2];	blkb[i3+0]=clrb[p3+0];
			blkb[i3+1]=clrb[p3+1];	blkb[i3+2]=clrb[p3+2];
		}
#endif
#if 0
		p=BGBBTJ_BitsLE_ReadBit(bits);
		blkb[0*4+0]=clrb[p*4+0];
		blkb[0*4+1]=clrb[p*4+1];
		blkb[0*4+2]=clrb[p*4+2];
		for(i=1; i<16; i++)
		{
			p=BGBBTJ_BitsLE_Read2Bits(bits);
			blkb[i*4+0]=clrb[p*4+0];
			blkb[i*4+1]=clrb[p*4+1];
			blkb[i*4+2]=clrb[p*4+2];
		}
#endif
	}

	if(a0==a1)
	{
		for(i=0; i<4; i++)
		{
			blkb[i*16+0*4+3]=a0;
			blkb[i*16+1*4+3]=a0;
			blkb[i*16+2*4+3]=a0;
			blkb[i*16+3*4+3]=a0;
		}

//		for(i=0; i<16; i++)
//		{
//			blkb[i*4+3]=a0;
//		}
//		p=BGBBTJ_BitsLE_ReadBits(bits, 31);
	}else
	{
#if 1
		p=BGBBTJ_BitsLE_Read7Bits(bits);
		p0=(p&1)*4;			p1=((p>>1)&3)*4;
		p2=((p>>3)&3)*4;	p3=((p>>5)&3)*4;
		i0=0*4;		i1=1*4;
		i2=2*4;		i3=3*4;
		blkb[i0+3]=clrb[p0+3];	blkb[i1+3]=clrb[p1+3];
		blkb[i2+3]=clrb[p2+3];	blkb[i3+3]=clrb[p3+3];
		for(i=1; i<4; i++)
		{
			p=BGBBTJ_BitsLE_Read8Bits(bits);
			p0=(p&3)*4;			p1=((p>>2)&3)*4;
			p2=((p>>4)&3)*4;	p3=((p>>6)&3)*4;
			i0=(i*4+0)*4;		i1=(i*4+1)*4;
			i2=(i*4+2)*4;		i3=(i*4+3)*4;
			blkb[i0+3]=clrb[p0+3];	blkb[i1+3]=clrb[p1+3];
			blkb[i2+3]=clrb[p2+3];	blkb[i3+3]=clrb[p3+3];
		}
#endif
#if 0
		p=BGBBTJ_BitsLE_ReadBit(bits);
		blkb[0*4+3]=clrb[p*4+3];
		for(i=1; i<16; i++)
		{
			p=BGBBTJ_BitsLE_Read2Bits(bits);
			blkb[i*4+3]=clrb[p*4+3];
		}
#endif
	}
	
	switch(rot)
	{
	case 0: break;
	case 1: case 2: case 3:
		k=rot-1;
		for(i=0; i<16; i++)
			{ j=blkb[i*4+k]; blkb[i*4+k]=blkb[i*4+3]; blkb[i*4+3]=j; }
		break;
	default:
		break;
	}

	if(flags&1)
	{
#if defined(X86) || defined(X86_64)
		if(xstride==4)
		{
			for(i=0; i<4; i++)
			{
				bp0=blkb+i*16;
				bp1=rgba+(i*ystride);
				p0=((int *)bp0)[0];		p1=((int *)bp0)[1];
				p2=((int *)bp0)[2];		p3=((int *)bp0)[3];
				p0=(p0&0xFF00FF00)|((p0>>16)&255)|((p0&255)<<16);
				p1=(p1&0xFF00FF00)|((p1>>16)&255)|((p1&255)<<16);
				p2=(p2&0xFF00FF00)|((p2>>16)&255)|((p2&255)<<16);
				p3=(p3&0xFF00FF00)|((p3>>16)&255)|((p3&255)<<16);
//				p0=(bp0[ 0]<<16)|(bp[ 1]<<8)|(bp[ 2])|(bp[ 3]<<24);
//				p1=(bp0[ 4]<<16)|(bp[ 5]<<8)|(bp[ 6])|(bp[ 7]<<24);
//				p2=(bp0[ 8]<<16)|(bp[ 9]<<8)|(bp[10])|(bp[11]<<24);
//				p3=(bp0[12]<<16)|(bp[13]<<8)|(bp[14])|(bp[15]<<24);
				((int *)bp1)[0]=p0;		((int *)bp1)[1]=p1;
				((int *)bp1)[2]=p2;		((int *)bp1)[3]=p3;
			}
			return;
		}
#endif

#if 1
		for(i=0; i<4; i++)
		{
			k=i*ystride+0*xstride; l=(i*4+0)*4;
			rgba[k+2]=blkb[l+0]; rgba[k+1]=blkb[l+1];
			rgba[k+0]=blkb[l+2]; rgba[k+3]=blkb[l+3];
			k=i*ystride+1*xstride; l=(i*4+1)*4;
			rgba[k+2]=blkb[l+0]; rgba[k+1]=blkb[l+1];
			rgba[k+0]=blkb[l+2]; rgba[k+3]=blkb[l+3];
			k=i*ystride+2*xstride; l=(i*4+2)*4;
			rgba[k+2]=blkb[l+0]; rgba[k+1]=blkb[l+1];
			rgba[k+0]=blkb[l+2]; rgba[k+3]=blkb[l+3];
			k=i*ystride+3*xstride; l=(i*4+3)*4;
			rgba[k+2]=blkb[l+0]; rgba[k+1]=blkb[l+1];
			rgba[k+0]=blkb[l+2]; rgba[k+3]=blkb[l+3];
		}
#endif

#if 0
		for(i=0; i<4; i++)
			for(j=0; j<4; j++)
		{
			k=i*ystride+j*xstride; l=(i*4+j)*4;
			rgba[k+2]=blkb[l+0]; rgba[k+1]=blkb[l+1];
			rgba[k+0]=blkb[l+2]; rgba[k+3]=blkb[l+3];
		}
#endif
	}else
	{
		for(i=0; i<4; i++)
			for(j=0; j<4; j++)
		{
			k=i*ystride+j*xstride; l=(i*4+j)*4;
//			k=i*ystride+j*xstride; l=(j*4+i)*4;
			rgba[k+0]=blkb[l+0]; rgba[k+1]=blkb[l+1];
			rgba[k+2]=blkb[l+2]; rgba[k+3]=blkb[l+3];
		}
	}
}

void BGBBTJ_BC7_DecodeBlock_Mode6(
	BGBBTJ_BitStream *bits,
	byte *rgba, int xstride, int ystride, int flags)
{
	byte blkb[16*4];
	byte clrb[16*4];
	int rot, idxm;
	int r0, r1, g0, g1, b0, b1, a0, a1;
	int p;
	int i, j, k, l;
	
	r0=BGBBTJ_BitsLE_Read7Bits(bits);
	r1=BGBBTJ_BitsLE_Read7Bits(bits);
	g0=BGBBTJ_BitsLE_Read7Bits(bits);
	g1=BGBBTJ_BitsLE_Read7Bits(bits);
	b0=BGBBTJ_BitsLE_Read7Bits(bits);
	b1=BGBBTJ_BitsLE_Read7Bits(bits);
	a0=BGBBTJ_BitsLE_Read7Bits(bits);
	a1=BGBBTJ_BitsLE_Read7Bits(bits);

	j=BGBBTJ_BitsLE_ReadBit(bits);
	k=BGBBTJ_BitsLE_ReadBit(bits);
	r0=(r0<<1)|j; g0=(g0<<1)|j; b0=(b0<<1)|j; a0=(a0<<1)|j;
	r1=(r1<<1)|k; g1=(g1<<1)|k; b1=(b1<<1)|k; a1=(a1<<1)|k;
	
	for(i=0; i<16; i++)
	{
		clrb[i*4+0]=bgbbtj_bc7_interpolate(r0, r1, i, 4);
		clrb[i*4+1]=bgbbtj_bc7_interpolate(g0, g1, i, 4);
		clrb[i*4+2]=bgbbtj_bc7_interpolate(b0, b1, i, 4);
		clrb[i*4+3]=bgbbtj_bc7_interpolate(a0, a1, i, 4);
	}
		
	p=BGBBTJ_BitsLE_Read3Bits(bits);
	blkb[0*4+0]=clrb[p*4+0]; blkb[0*4+1]=clrb[p*4+1];
	blkb[0*4+2]=clrb[p*4+2]; blkb[0*4+3]=clrb[p*4+3];
	for(i=1; i<16; i++)
	{
		p=BGBBTJ_BitsLE_Read4Bits(bits);
		blkb[i*4+0]=clrb[p*4+0]; blkb[i*4+1]=clrb[p*4+1];
		blkb[i*4+2]=clrb[p*4+2]; blkb[i*4+3]=clrb[p*4+3];
	}
	
	if(flags&1)
	{
		for(i=0; i<4; i++)
			for(j=0; j<4; j++)
		{
			k=i*ystride+j*xstride; l=(i*4+j)*4;
			rgba[k+2]=blkb[l+0]; rgba[k+1]=blkb[l+1];
			rgba[k+0]=blkb[l+2]; rgba[k+3]=blkb[l+3];
		}
	}else
	{
		for(i=0; i<4; i++)
			for(j=0; j<4; j++)
		{
			k=i*ystride+j*xstride; l=(i*4+j)*4;
//			k=i*ystride+j*xstride; l=(j*4+i)*4;
			rgba[k+0]=blkb[l+0]; rgba[k+1]=blkb[l+1];
			rgba[k+2]=blkb[l+2]; rgba[k+3]=blkb[l+3];
		}
	}
}

void BGBBTJ_BC7_DecodeBlock_Mode0(
	BGBBTJ_BitStream *bits,
	byte *rgba, int xstride, int ystride, int flags)
{
	byte blkb[16*4];
	byte clrb[32*4];
	int part, an2, an3;
	int r0, r1, g0, g1, b0, b1, a0, a1;
	int r2, r3, g2, g3, b2, b3, a2, a3;
	int r4, r5, g4, g5, b4, b5, a4, a5;
	int p0, p1, p2, p3, p4, p5;
	int i, j, k, l, p;

	part=BGBBTJ_BitsLE_Read4Bits(bits);
	
	r0=BGBBTJ_BitsLE_Read4Bits(bits);
	r1=BGBBTJ_BitsLE_Read4Bits(bits);
	r2=BGBBTJ_BitsLE_Read4Bits(bits);
	r3=BGBBTJ_BitsLE_Read4Bits(bits);
	r4=BGBBTJ_BitsLE_Read4Bits(bits);
	r5=BGBBTJ_BitsLE_Read4Bits(bits);
	g0=BGBBTJ_BitsLE_Read4Bits(bits);
	g1=BGBBTJ_BitsLE_Read4Bits(bits);
	g2=BGBBTJ_BitsLE_Read4Bits(bits);
	g3=BGBBTJ_BitsLE_Read4Bits(bits);
	g4=BGBBTJ_BitsLE_Read4Bits(bits);
	g5=BGBBTJ_BitsLE_Read4Bits(bits);
	b0=BGBBTJ_BitsLE_Read4Bits(bits);
	b1=BGBBTJ_BitsLE_Read4Bits(bits);
	b2=BGBBTJ_BitsLE_Read4Bits(bits);
	b3=BGBBTJ_BitsLE_Read4Bits(bits);
	b4=BGBBTJ_BitsLE_Read4Bits(bits);
	b5=BGBBTJ_BitsLE_Read4Bits(bits);

	p0=BGBBTJ_BitsLE_ReadBit(bits);
	p1=BGBBTJ_BitsLE_ReadBit(bits);
	p2=BGBBTJ_BitsLE_ReadBit(bits);
	p3=BGBBTJ_BitsLE_ReadBit(bits);
	p4=BGBBTJ_BitsLE_ReadBit(bits);
	p5=BGBBTJ_BitsLE_ReadBit(bits);

	r0=(r0<<1)|p0; g0=(g0<<1)|p0; b0=(b0<<1)|p0;
	r1=(r1<<1)|p1; g1=(g1<<1)|p1; b1=(b1<<1)|p1;
	r2=(r2<<1)|p2; g2=(g2<<1)|p2; b2=(b2<<1)|p2;
	r3=(r3<<1)|p3; g3=(g3<<1)|p3; b3=(b3<<1)|p3;
	r4=(r4<<1)|p4; g4=(g4<<1)|p4; b4=(b4<<1)|p4;
	r5=(r5<<1)|p5; g5=(g5<<1)|p5; b5=(b5<<1)|p5;

	r0=(r0<<3)|(r0>>2); g0=(g0<<3)|(g0>>2); b0=(b0<<3)|(b0>>2);
	r1=(r1<<3)|(r1>>2); g1=(g1<<3)|(g1>>2); b1=(b1<<3)|(b1>>2);
	r2=(r2<<3)|(r2>>2); g2=(g2<<3)|(g2>>2); b2=(b2<<3)|(b2>>2);
	r3=(r3<<3)|(r3>>2); g3=(g3<<3)|(g3>>2); b3=(b3<<3)|(b3>>2);
	r4=(r4<<3)|(r4>>2); g4=(g4<<3)|(g4>>2); b4=(b4<<3)|(b4>>2);
	r5=(r5<<3)|(r5>>2); g5=(g5<<3)|(g5>>2); b5=(b5<<3)|(b5>>2);
	
	for(i=0; i<8; i++)
	{
		clrb[(0+i)*4+0]=bgbbtj_bc7_interpolate(r0, r1, i, 3);
		clrb[(0+i)*4+1]=bgbbtj_bc7_interpolate(g0, g1, i, 3);
		clrb[(0+i)*4+2]=bgbbtj_bc7_interpolate(b0, b1, i, 3);
		clrb[(0+i)*4+3]=255;
		clrb[(8+i)*4+0]=bgbbtj_bc7_interpolate(r2, r3, i, 3);
		clrb[(8+i)*4+1]=bgbbtj_bc7_interpolate(g2, g3, i, 3);
		clrb[(8+i)*4+2]=bgbbtj_bc7_interpolate(b2, b3, i, 3);
		clrb[(8+i)*4+3]=255;
		clrb[(16+i)*4+0]=bgbbtj_bc7_interpolate(r4, r5, i, 3);
		clrb[(16+i)*4+1]=bgbbtj_bc7_interpolate(g4, g5, i, 3);
		clrb[(16+i)*4+2]=bgbbtj_bc7_interpolate(b4, b5, i, 3);
		clrb[(16+i)*4+3]=255;
	}
	
	an2=bgbbtj_bc7_anchor3a[part];
	an3=bgbbtj_bc7_anchor3b[part];
	
	p=BGBBTJ_BitsLE_Read2Bits(bits);
	blkb[0*4+0]=clrb[p*4+0]; blkb[0*4+1]=clrb[p*4+1];
	blkb[0*4+2]=clrb[p*4+2]; blkb[0*4+3]=clrb[p*4+3];
	for(i=1; i<16; i++)
	{
		j=bgbbtj_bc7_partition3[(part<<4)+i]*8;
		if((i==an2) || (i==an3))
			{ p=BGBBTJ_BitsLE_Read2Bits(bits)+j; }
		else
			{ p=BGBBTJ_BitsLE_Read3Bits(bits)+j; }
		blkb[i*4+0]=clrb[p*4+0]; blkb[i*4+1]=clrb[p*4+1];
		blkb[i*4+2]=clrb[p*4+2]; blkb[i*4+3]=clrb[p*4+3];
	}
	
	if(flags&1)
	{
		for(i=0; i<4; i++)
			for(j=0; j<4; j++)
		{
			k=i*ystride+j*xstride; l=(i*4+j)*4;
			rgba[k+2]=blkb[l+0]; rgba[k+1]=blkb[l+1];
			rgba[k+0]=blkb[l+2]; rgba[k+3]=blkb[l+3];
		}
	}else
	{
		for(i=0; i<4; i++)
			for(j=0; j<4; j++)
		{
			k=i*ystride+j*xstride; l=(i*4+j)*4;
//			k=i*ystride+j*xstride; l=(j*4+i)*4;
			rgba[k+0]=blkb[l+0]; rgba[k+1]=blkb[l+1];
			rgba[k+2]=blkb[l+2]; rgba[k+3]=blkb[l+3];
		}
	}
}

void BGBBTJ_BC7_DecodeBlock_Mode1(
	BGBBTJ_BitStream *bits,
	byte *rgba, int xstride, int ystride, int flags)
{
	byte blkb[16*4];
	byte clrb[16*4];
	int part, an2;
	int r0, r1, g0, g1, b0, b1, a0, a1;
	int r2, r3, g2, g3, b2, b3, a2, a3;
	int p0, p1, p2, p3;
	int i, j, k, l, p;

	part=BGBBTJ_BitsLE_Read6Bits(bits);
	
	r0=BGBBTJ_BitsLE_Read6Bits(bits);
	r1=BGBBTJ_BitsLE_Read6Bits(bits);
	r2=BGBBTJ_BitsLE_Read6Bits(bits);
	r3=BGBBTJ_BitsLE_Read6Bits(bits);
	g0=BGBBTJ_BitsLE_Read6Bits(bits);
	g1=BGBBTJ_BitsLE_Read6Bits(bits);
	g2=BGBBTJ_BitsLE_Read6Bits(bits);
	g3=BGBBTJ_BitsLE_Read6Bits(bits);
	b0=BGBBTJ_BitsLE_Read6Bits(bits);
	b1=BGBBTJ_BitsLE_Read6Bits(bits);
	b2=BGBBTJ_BitsLE_Read6Bits(bits);
	b3=BGBBTJ_BitsLE_Read6Bits(bits);

	p0=BGBBTJ_BitsLE_ReadBit(bits);
	p1=BGBBTJ_BitsLE_ReadBit(bits);

	r0=(r0<<1)|p0; g0=(g0<<1)|p0; b0=(b0<<1)|p0;
	r1=(r1<<1)|p0; g1=(g1<<1)|p0; b1=(b1<<1)|p0;
	r2=(r2<<1)|p1; g2=(g2<<1)|p1; b2=(b2<<1)|p1;
	r3=(r3<<1)|p1; g3=(g3<<1)|p1; b3=(b3<<1)|p1;

	r0=(r0<<1)|(r0>>6); g0=(g0<<1)|(g0>>6); b0=(b0<<1)|(b0>>6);
	r1=(r1<<1)|(r1>>6); g1=(g1<<1)|(g1>>6); b1=(b1<<1)|(b1>>6);
	r2=(r2<<1)|(r2>>6); g2=(g2<<1)|(g2>>6); b2=(b2<<1)|(b2>>6);
	r3=(r3<<1)|(r3>>6); g3=(g3<<1)|(g3>>6); b3=(b3<<1)|(b3>>6);
	
	for(i=0; i<8; i++)
	{
		clrb[(0+i)*4+0]=bgbbtj_bc7_interpolate(r0, r1, i, 3);
		clrb[(0+i)*4+1]=bgbbtj_bc7_interpolate(g0, g1, i, 3);
		clrb[(0+i)*4+2]=bgbbtj_bc7_interpolate(b0, b1, i, 3);
		clrb[(0+i)*4+3]=255;
		clrb[(8+i)*4+0]=bgbbtj_bc7_interpolate(r2, r3, i, 3);
		clrb[(8+i)*4+1]=bgbbtj_bc7_interpolate(g2, g3, i, 3);
		clrb[(8+i)*4+2]=bgbbtj_bc7_interpolate(b2, b3, i, 3);
		clrb[(8+i)*4+3]=255;
	}
	
	an2=bgbbtj_bc7_anchor2[part];
	
	p=BGBBTJ_BitsLE_Read2Bits(bits);
	blkb[0*4+0]=clrb[p*4+0]; blkb[0*4+1]=clrb[p*4+1];
	blkb[0*4+2]=clrb[p*4+2]; blkb[0*4+3]=clrb[p*4+3];
	for(i=1; i<16; i++)
	{
		j=bgbbtj_bc7_partition2[(part<<4)+i]*8;
		if(i==an2)
			{ p=BGBBTJ_BitsLE_Read2Bits(bits)+j; }
		else
			{ p=BGBBTJ_BitsLE_Read3Bits(bits)+j; }
		blkb[i*4+0]=clrb[p*4+0]; blkb[i*4+1]=clrb[p*4+1];
		blkb[i*4+2]=clrb[p*4+2]; blkb[i*4+3]=clrb[p*4+3];
	}
	
	if(flags&1)
	{
		for(i=0; i<4; i++)
			for(j=0; j<4; j++)
		{
			k=i*ystride+j*xstride; l=(i*4+j)*4;
			rgba[k+2]=blkb[l+0]; rgba[k+1]=blkb[l+1];
			rgba[k+0]=blkb[l+2]; rgba[k+3]=blkb[l+3];
		}
	}else
	{
		for(i=0; i<4; i++)
			for(j=0; j<4; j++)
		{
			k=i*ystride+j*xstride; l=(i*4+j)*4;
//			k=i*ystride+j*xstride; l=(j*4+i)*4;
			rgba[k+0]=blkb[l+0]; rgba[k+1]=blkb[l+1];
			rgba[k+2]=blkb[l+2]; rgba[k+3]=blkb[l+3];
		}
	}
}

void BGBBTJ_BC7_DecodeBlock_Mode2(
	BGBBTJ_BitStream *bits,
	byte *rgba, int xstride, int ystride, int flags)
{
	byte blkb[16*4];
	byte clrb[16*4];
	int part, an2, an3;
	int r0, r1, g0, g1, b0, b1, a0, a1;
	int r2, r3, g2, g3, b2, b3, a2, a3;
	int r4, r5, g4, g5, b4, b5, a4, a5;
	int p0, p1, p2, p3;
	int i, j, k, l, p;

	part=BGBBTJ_BitsLE_Read6Bits(bits);
	
	r0=BGBBTJ_BitsLE_Read5Bits(bits);
	r1=BGBBTJ_BitsLE_Read5Bits(bits);
	r2=BGBBTJ_BitsLE_Read5Bits(bits);
	r3=BGBBTJ_BitsLE_Read5Bits(bits);
	r4=BGBBTJ_BitsLE_Read5Bits(bits);
	r5=BGBBTJ_BitsLE_Read5Bits(bits);
	g0=BGBBTJ_BitsLE_Read5Bits(bits);
	g1=BGBBTJ_BitsLE_Read5Bits(bits);
	g2=BGBBTJ_BitsLE_Read5Bits(bits);
	g3=BGBBTJ_BitsLE_Read5Bits(bits);
	g4=BGBBTJ_BitsLE_Read5Bits(bits);
	g5=BGBBTJ_BitsLE_Read5Bits(bits);
	b0=BGBBTJ_BitsLE_Read5Bits(bits);
	b1=BGBBTJ_BitsLE_Read5Bits(bits);
	b2=BGBBTJ_BitsLE_Read5Bits(bits);
	b3=BGBBTJ_BitsLE_Read5Bits(bits);
	b4=BGBBTJ_BitsLE_Read5Bits(bits);
	b5=BGBBTJ_BitsLE_Read5Bits(bits);

	r0=(r0<<3)|(r0>>2); g0=(g0<<3)|(g0>>2); b0=(b0<<3)|(b0>>2);
	r1=(r1<<3)|(r1>>2); g1=(g1<<3)|(g1>>2); b1=(b1<<3)|(b1>>2);
	r2=(r2<<3)|(r2>>2); g2=(g2<<3)|(g2>>2); b2=(b2<<3)|(b2>>2);
	r3=(r3<<3)|(r3>>2); g3=(g3<<3)|(g3>>2); b3=(b3<<3)|(b3>>2);
	r4=(r4<<3)|(r4>>2); g4=(g4<<3)|(g4>>2); b4=(b4<<3)|(b4>>2);
	r5=(r5<<3)|(r5>>2); g5=(g5<<3)|(g5>>2); b5=(b5<<3)|(b5>>2);

	for(i=0; i<4; i++)
	{
		clrb[(0+i)*4+0]=bgbbtj_bc7_interpolate(r0, r1, i, 2);
		clrb[(0+i)*4+1]=bgbbtj_bc7_interpolate(g0, g1, i, 2);
		clrb[(0+i)*4+2]=bgbbtj_bc7_interpolate(b0, b1, i, 2);
		clrb[(0+i)*4+3]=255;
		clrb[(4+i)*4+0]=bgbbtj_bc7_interpolate(r2, r3, i, 2);
		clrb[(4+i)*4+1]=bgbbtj_bc7_interpolate(g2, g3, i, 2);
		clrb[(4+i)*4+2]=bgbbtj_bc7_interpolate(b2, b3, i, 2);
		clrb[(4+i)*4+3]=255;
		clrb[(8+i)*4+0]=bgbbtj_bc7_interpolate(r4, r5, i, 2);
		clrb[(8+i)*4+1]=bgbbtj_bc7_interpolate(g4, g5, i, 2);
		clrb[(8+i)*4+2]=bgbbtj_bc7_interpolate(b4, b5, i, 2);
		clrb[(8+i)*4+3]=255;
	}
	
	an2=bgbbtj_bc7_anchor3a[part];
	an3=bgbbtj_bc7_anchor3b[part];
	
	p=BGBBTJ_BitsLE_ReadBit(bits);
	blkb[0*4+0]=clrb[p*4+0]; blkb[0*4+1]=clrb[p*4+1];
	blkb[0*4+2]=clrb[p*4+2]; blkb[0*4+3]=clrb[p*4+3];
	for(i=1; i<16; i++)
	{
		j=bgbbtj_bc7_partition3[(part<<4)+i]*4;
		if((i==an2) || (i==an3))
			{ p=BGBBTJ_BitsLE_ReadBit(bits)+j; }
		else
			{ p=BGBBTJ_BitsLE_Read2Bits(bits)+j; }
		blkb[i*4+0]=clrb[p*4+0]; blkb[i*4+1]=clrb[p*4+1];
		blkb[i*4+2]=clrb[p*4+2]; blkb[i*4+3]=clrb[p*4+3];
	}
	
	if(flags&1)
	{
		for(i=0; i<4; i++)
			for(j=0; j<4; j++)
		{
			k=i*ystride+j*xstride; l=(i*4+j)*4;
			rgba[k+2]=blkb[l+0]; rgba[k+1]=blkb[l+1];
			rgba[k+0]=blkb[l+2]; rgba[k+3]=blkb[l+3];
		}
	}else
	{
		for(i=0; i<4; i++)
			for(j=0; j<4; j++)
		{
			k=i*ystride+j*xstride; l=(i*4+j)*4;
//			k=i*ystride+j*xstride; l=(j*4+i)*4;
			rgba[k+0]=blkb[l+0]; rgba[k+1]=blkb[l+1];
			rgba[k+2]=blkb[l+2]; rgba[k+3]=blkb[l+3];
		}
	}
}

void BGBBTJ_BC7_DecodeBlock_Mode3(
	BGBBTJ_BitStream *bits,
	byte *rgba, int xstride, int ystride, int flags)
{
	byte blkb[16*4];
	byte clrb[16*4];
	int part, an2;
	int r0, r1, g0, g1, b0, b1, a0, a1;
	int r2, r3, g2, g3, b2, b3, a2, a3;
	int p0, p1, p2, p3;
	int i, j, k, l, p;

	part=BGBBTJ_BitsLE_Read6Bits(bits);
	
	r0=BGBBTJ_BitsLE_Read7Bits(bits);
	r1=BGBBTJ_BitsLE_Read7Bits(bits);
	r2=BGBBTJ_BitsLE_Read7Bits(bits);
	r3=BGBBTJ_BitsLE_Read7Bits(bits);
	g0=BGBBTJ_BitsLE_Read7Bits(bits);
	g1=BGBBTJ_BitsLE_Read7Bits(bits);
	g2=BGBBTJ_BitsLE_Read7Bits(bits);
	g3=BGBBTJ_BitsLE_Read7Bits(bits);
	b0=BGBBTJ_BitsLE_Read7Bits(bits);
	b1=BGBBTJ_BitsLE_Read7Bits(bits);
	b2=BGBBTJ_BitsLE_Read7Bits(bits);
	b3=BGBBTJ_BitsLE_Read7Bits(bits);

	p0=BGBBTJ_BitsLE_ReadBit(bits);
	p1=BGBBTJ_BitsLE_ReadBit(bits);
	p2=BGBBTJ_BitsLE_ReadBit(bits);
	p3=BGBBTJ_BitsLE_ReadBit(bits);
	r0=(r0<<1)|p0; g0=(g0<<1)|p0; b0=(b0<<1)|p0;
	r1=(r1<<1)|p1; g1=(g1<<1)|p1; b1=(b1<<1)|p1;
	r2=(r2<<1)|p2; g2=(g2<<1)|p2; b2=(b2<<1)|p2;
	r3=(r3<<1)|p3; g3=(g3<<1)|p3; b3=(b3<<1)|p3;
	
	for(i=0; i<4; i++)
	{
		clrb[(0+i)*4+0]=bgbbtj_bc7_interpolate(r0, r1, i, 2);
		clrb[(0+i)*4+1]=bgbbtj_bc7_interpolate(g0, g1, i, 2);
		clrb[(0+i)*4+2]=bgbbtj_bc7_interpolate(b0, b1, i, 2);
		clrb[(0+i)*4+3]=255;
		clrb[(8+i)*4+0]=bgbbtj_bc7_interpolate(r2, r3, i, 2);
		clrb[(8+i)*4+1]=bgbbtj_bc7_interpolate(g2, g3, i, 2);
		clrb[(8+i)*4+2]=bgbbtj_bc7_interpolate(b2, b3, i, 2);
		clrb[(8+i)*4+3]=255;
	}
	
	an2=bgbbtj_bc7_anchor2[part];
	
	p=BGBBTJ_BitsLE_ReadBit(bits);
	blkb[0*4+0]=clrb[p*4+0]; blkb[0*4+1]=clrb[p*4+1];
	blkb[0*4+2]=clrb[p*4+2]; blkb[0*4+3]=clrb[p*4+3];
	for(i=1; i<16; i++)
	{
		j=bgbbtj_bc7_partition2[(part<<4)+i]*8;
		if(i==an2)
			{ p=BGBBTJ_BitsLE_ReadBit(bits)+j; }
		else
			{ p=BGBBTJ_BitsLE_Read2Bits(bits)+j; }
		blkb[i*4+0]=clrb[p*4+0]; blkb[i*4+1]=clrb[p*4+1];
		blkb[i*4+2]=clrb[p*4+2]; blkb[i*4+3]=clrb[p*4+3];
	}
	
	if(flags&1)
	{
		for(i=0; i<4; i++)
			for(j=0; j<4; j++)
		{
			k=i*ystride+j*xstride; l=(i*4+j)*4;
			rgba[k+2]=blkb[l+0]; rgba[k+1]=blkb[l+1];
			rgba[k+0]=blkb[l+2]; rgba[k+3]=blkb[l+3];
		}
	}else
	{
		for(i=0; i<4; i++)
			for(j=0; j<4; j++)
		{
			k=i*ystride+j*xstride; l=(i*4+j)*4;
//			k=i*ystride+j*xstride; l=(j*4+i)*4;
			rgba[k+0]=blkb[l+0]; rgba[k+1]=blkb[l+1];
			rgba[k+2]=blkb[l+2]; rgba[k+3]=blkb[l+3];
		}
	}
}

void BGBBTJ_BC7_DecodeBlock_Mode7(
	BGBBTJ_BitStream *bits,
	byte *rgba, int xstride, int ystride, int flags)
{
	byte blkb[16*4];
	byte clrb[16*4];
	int part, an2;
	int r0, r1, g0, g1, b0, b1, a0, a1;
	int r2, r3, g2, g3, b2, b3, a2, a3;
	int p0, p1, p2, p3;
	int i, j, k, l, p;

	part=BGBBTJ_BitsLE_Read6Bits(bits);
	
	r0=BGBBTJ_BitsLE_Read5Bits(bits);
	r1=BGBBTJ_BitsLE_Read5Bits(bits);
	r2=BGBBTJ_BitsLE_Read5Bits(bits);
	r3=BGBBTJ_BitsLE_Read5Bits(bits);
	g0=BGBBTJ_BitsLE_Read5Bits(bits);
	g1=BGBBTJ_BitsLE_Read5Bits(bits);
	g2=BGBBTJ_BitsLE_Read5Bits(bits);
	g3=BGBBTJ_BitsLE_Read5Bits(bits);
	b0=BGBBTJ_BitsLE_Read5Bits(bits);
	b1=BGBBTJ_BitsLE_Read5Bits(bits);
	b2=BGBBTJ_BitsLE_Read5Bits(bits);
	b3=BGBBTJ_BitsLE_Read5Bits(bits);
	a0=BGBBTJ_BitsLE_Read5Bits(bits);
	a1=BGBBTJ_BitsLE_Read5Bits(bits);
	a2=BGBBTJ_BitsLE_Read5Bits(bits);
	a3=BGBBTJ_BitsLE_Read5Bits(bits);

	p0=BGBBTJ_BitsLE_ReadBit(bits);
	p1=BGBBTJ_BitsLE_ReadBit(bits);
	p2=BGBBTJ_BitsLE_ReadBit(bits);
	p3=BGBBTJ_BitsLE_ReadBit(bits);

	r0=(r0<<1)|p0; g0=(g0<<1)|p0; b0=(b0<<1)|p0; a0=(a0<<1)|p0;
	r1=(r1<<1)|p1; g1=(g1<<1)|p1; b1=(b1<<1)|p1; a1=(a1<<1)|p1;
	r2=(r2<<1)|p2; g2=(g2<<1)|p2; b2=(b2<<1)|p2; a2=(a2<<1)|p2;
	r3=(r3<<1)|p3; g3=(g3<<1)|p3; b3=(b3<<1)|p3; a3=(a3<<1)|p3;

	r0=(r0<<2)|(r0>>4); g0=(g0<<2)|(g0>>4);
	b0=(b0<<2)|(b0>>4); a0=(a0<<2)|(a0>>4);
	r1=(r1<<2)|(r1>>4); g1=(g1<<2)|(g1>>4);
	b1=(b1<<2)|(b1>>4); a1=(a1<<2)|(a1>>4);
	r2=(r2<<2)|(r2>>4); g2=(g2<<2)|(g2>>4);
	b2=(b2<<2)|(b2>>4); a2=(a2<<2)|(a2>>4);
	r3=(r3<<2)|(r3>>4); g3=(g3<<2)|(g3>>4);
	b3=(b3<<2)|(b3>>4); a3=(a3<<2)|(a3>>4);

	for(i=0; i<4; i++)
	{
		clrb[(0+i)*4+0]=bgbbtj_bc7_interpolate(r0, r1, i, 2);
		clrb[(0+i)*4+1]=bgbbtj_bc7_interpolate(g0, g1, i, 2);
		clrb[(0+i)*4+2]=bgbbtj_bc7_interpolate(b0, b1, i, 2);
		clrb[(0+i)*4+3]=bgbbtj_bc7_interpolate(a0, a1, i, 2);
		clrb[(8+i)*4+0]=bgbbtj_bc7_interpolate(r2, r3, i, 2);
		clrb[(8+i)*4+1]=bgbbtj_bc7_interpolate(g2, g3, i, 2);
		clrb[(8+i)*4+2]=bgbbtj_bc7_interpolate(b2, b3, i, 2);
		clrb[(8+i)*4+3]=bgbbtj_bc7_interpolate(a2, a3, i, 2);
	}
	
	an2=bgbbtj_bc7_anchor2[part];
	
	p=BGBBTJ_BitsLE_ReadBit(bits);
	blkb[0*4+0]=clrb[p*4+0]; blkb[0*4+1]=clrb[p*4+1];
	blkb[0*4+2]=clrb[p*4+2]; blkb[0*4+3]=clrb[p*4+3];
	for(i=1; i<16; i++)
	{
		j=bgbbtj_bc7_partition2[(part<<4)+i]*8;
		if(i==an2)
			{ p=BGBBTJ_BitsLE_ReadBit(bits)+j; }
		else
			{ p=BGBBTJ_BitsLE_Read2Bits(bits)+j; }
		blkb[i*4+0]=clrb[p*4+0]; blkb[i*4+1]=clrb[p*4+1];
		blkb[i*4+2]=clrb[p*4+2]; blkb[i*4+3]=clrb[p*4+3];
	}
	
	if(flags&1)
	{
		for(i=0; i<4; i++)
			for(j=0; j<4; j++)
		{
			k=i*ystride+j*xstride; l=(i*4+j)*4;
			rgba[k+2]=blkb[l+0]; rgba[k+1]=blkb[l+1];
			rgba[k+0]=blkb[l+2]; rgba[k+3]=blkb[l+3];
		}
	}else
	{
		for(i=0; i<4; i++)
			for(j=0; j<4; j++)
		{
			k=i*ystride+j*xstride; l=(i*4+j)*4;
//			k=i*ystride+j*xstride; l=(j*4+i)*4;
			rgba[k+0]=blkb[l+0]; rgba[k+1]=blkb[l+1];
			rgba[k+2]=blkb[l+2]; rgba[k+3]=blkb[l+3];
		}
	}
}

void BGBBTJ_BC7_DecodeBlock_Default(
	BGBBTJ_BitStream *bits,
	byte *rgba, int xstride, int ystride, int flags)
{
	int i, j;
	
	for(i=0; i<4; i++)
		for(j=0; j<4; j++)
	{
		rgba[i*ystride+j*xstride+0]=0;
		rgba[i*ystride+j*xstride+1]=0;
		rgba[i*ystride+j*xstride+2]=0;
		rgba[i*ystride+j*xstride+3]=0;
	}
}

int BGBBTJ_BC7_GetBlockMode(byte *block)
{
	int mode;
	int i;
	
	i=block[0];
	if((i&1)==1)		//Mode 0
	{
		mode=0;
	}else if((i&  3)==  2)		//Mode 1
	{
		mode=1;
	}else if((i&  7)==  4)		//Mode 2
	{
		mode=2;
	}else if((i& 15)==  8)		//Mode 3
	{
		mode=3;
	}else if((i& 31)== 16)		//Mode 4
	{
		mode=4;
	}else if((i& 63)== 32)		//Mode 5
	{
		mode=5;
	}else if((i&127)== 64)		//Mode 6
	{
		mode=6;
	}else if((i&255)==128)		//Mode 7
	{
		mode=7;
	}
	return(mode);
}

void BGBBTJ_BC7_DecodeBlock(byte *block,
	byte *rgba, int xstride, int ystride, int flags)
{
	BGBBTJ_BitStream bits;
	int mode;

	BGBBTJ_BitsLE_ClearSetupRead(&bits, block, 16);
	mode=BGBBTJ_BC7_DecodeBlock_ReadMode(&bits);
	switch(mode)
	{
	case 0:
		BGBBTJ_BC7_DecodeBlock_Mode0(&bits, rgba, xstride, ystride, flags);
		break;
	case 1:
		BGBBTJ_BC7_DecodeBlock_Mode1(&bits, rgba, xstride, ystride, flags);
		break;
	case 2:
		BGBBTJ_BC7_DecodeBlock_Mode2(&bits, rgba, xstride, ystride, flags);
		break;
	case 3:
		BGBBTJ_BC7_DecodeBlock_Mode3(&bits, rgba, xstride, ystride, flags);
		break;
	case 4:
		BGBBTJ_BC7_DecodeBlock_Mode4(&bits, rgba, xstride, ystride, flags);
		break;
	case 5:
		BGBBTJ_BC7_DecodeBlock_Mode5(&bits, rgba, xstride, ystride, flags);
		break;
	case 6:
		BGBBTJ_BC7_DecodeBlock_Mode6(&bits, rgba, xstride, ystride, flags);
		break;
	case 7:
		BGBBTJ_BC7_DecodeBlock_Mode7(&bits, rgba, xstride, ystride, flags);
		break;
	default:
		BGBBTJ_BC7_DecodeBlock_Default(&bits, rgba, xstride, ystride, flags);
		break;
	}
}

BGBBTJ_API void BGBBTJ_BC7_DecodeImageBC7(byte *block,
	byte *rgba, int xs, int ys, int stride)
{
	int xs1, ys1;
	int i, j;
	
	xs1=xs>>2; ys1=ys>>2;
	for(i=0; i<ys1; i++)
		for(j=0; j<xs1; j++)
	{
		BGBBTJ_BC7_DecodeBlock(
			block+(i*xs1+j)*16,
			rgba+(i*4*xs+j*4)*stride,
			stride, xs*stride, 0);
	}
}

BGBBTJ_API void BGBBTJ_BC7_DecodeImageBC7_BGRA(byte *block,
	byte *rgba, int xs, int ys, int stride)
{
	int xs1, ys1;
	int i, j;
	
	xs1=xs>>2; ys1=ys>>2;
	for(i=0; i<ys1; i++)
		for(j=0; j<xs1; j++)
	{
		BGBBTJ_BC7_DecodeBlock(
			block+(i*xs1+j)*16,
			rgba+(i*4*xs+j*4)*stride,
			stride, xs*stride, 1);
	}
}


void BGBBTJ_BC7_FlipBlock_Mode5(byte *iblk, byte *oblk)
{
	u64 l0, l1;
	u32 p0, p1, p2, p3;
	u32 pix, apix;

	bgbbtj_bt1c_memcpy16(oblk, iblk);
	//extract pixels
	p0=iblk[ 8]|(iblk[ 9]<<8)|(iblk[10]<<16)|(iblk[11]<<24);
	p1=iblk[12]|(iblk[13]<<8)|(iblk[14]<<16)|(iblk[15]<<24);
	apix=((p1>>1)&1)|(p1&(~3));
	pix=((p0>>2)&1)|((p0>>1)&(~3))|((p1&1)<<31);
	
	//flat color, skip
	if(!pix && !apix)
		return;
	
	//flip pixels
	p0=pix&255;			p1=(pix>>8)&255;
	p2=(pix>>16)&255;	p3=(pix>>24)&255;
	pix=(p0<<24)|(p1<<16)|(p2<<8)|p3;
	//flip alpha
	p0=apix&255;		p1=(apix>>8)&255;
	p2=(apix>>16)&255;	p3=(apix>>24)&255;
	apix=(p0<<24)|(p1<<16)|(p2<<8)|p3;
		
	//check if we need to invert pixels or alpha
	if((pix&2) || (apix&2))
	{
#if defined(X86) || defined(X86_64)
		l0=*(s64 *)(iblk+1);
#else
		l0=	 ((s64)iblk[1])     |(((s64)iblk[2])<< 8)|
			(((s64)iblk[3])<<16)|(((s64)iblk[4])<<24)|
			(((s64)iblk[5])<<32)|(((s64)iblk[6])<<40)|
			(((s64)iblk[7])<<48)|(((s64)iblk[8])<<56);
#endif
			
		//invert pixels (and swap colors)
		if(pix&2)
		{
			//00000000 00000111 11110000 00011111 11000000 01111111
			//00000011 11111000 00001111 11100000 00111111 10000000
			l0= ((l0&0x00000007F01FC07FULL)<<7)|
				((l0&0x000003F80FE03480ULL)>>7)|
				 (l0&0xFFFFFC0000000000ULL);
			pix=~pix;
		}

		//invert alpha pixels (and swap alpha)
		if(apix&2)
		{
			//00000000 00000111 11110000 00011111 11000000 01111111
			//00000011 11111000 00001111 11100000 00111111 10000000
			l0= ((l0&0x0003FC0000000000ULL)<<8)|
				((l0&0x03FC000000000000ULL)>>8)|
				 (l0&0xFC0003FFFFFFFFFFULL);
			apix=~apix;
		}
		
		//write back
#if defined(X86) || defined(X86_64)
		*(s64 *)(oblk+1)=l0;
#else
		oblk[1]=(l0    )&255;	oblk[2]=(l0>> 8)&255;
		oblk[3]=(l0>>16)&255;	oblk[4]=(l0>>24)&255;
		oblk[5]=(l0>>32)&255;	oblk[6]=(l0>>40)&255;
		oblk[7]=(l0>>48)&255;	oblk[8]=(l0>>56)&255;
#endif
	}

	l0=iblk[8];
//	l0=	 ((s64)iblk[ 8])     |(((s64)iblk[ 9])<< 8)|
//		(((s64)iblk[10])<<16)|(((s64)iblk[11])<<24)|
//		(((s64)iblk[12])<<32)|(((s64)iblk[13])<<40)|
//		(((s64)iblk[14])<<48)|(((s64)iblk[15])<<56);
	l0=l0&3;
	l0|=(pix&1)<<2;
	l0|=((u64)(pix&(~3)))<<3;
	l0|=((u64)(apix&1))<<33;
	l0|=((u64)(apix&(~3)))<<34;
		
#if defined(X86) || defined(X86_64)
	*(s64 *)(oblk+8)=l0;
#else
	oblk[ 8]=(l0    )&255;	oblk[ 9]=(l0>> 8)&255;
	oblk[10]=(l0>>16)&255;	oblk[11]=(l0>>24)&255;
	oblk[12]=(l0>>32)&255;	oblk[13]=(l0>>40)&255;
	oblk[14]=(l0>>48)&255;	oblk[15]=(l0>>56)&255;
#endif
}

void BGBBTJ_BC7_FlipBlock(byte *iblk, byte *oblk)
{
	int i;
	
	i=iblk[0];
	if((i&1)==1)		//Mode 0
	{
	}else if((i&  3)==  2)		//Mode 1
	{
	}else if((i&  7)==  4)		//Mode 2
	{
	}else if((i& 15)==  8)		//Mode 3
	{
	}else if((i& 31)== 16)		//Mode 4
	{
	}else if((i& 63)== 32)		//Mode 5
	{
		BGBBTJ_BC7_FlipBlock_Mode5(iblk, oblk);
	}else if((i&127)== 64)		//Mode 6
	{
	}else if((i&255)==128)		//Mode 7
	{
	}
}

BGBBTJ_API void BGBBTJ_BC7_FlipImageBC7(
	byte *iblk, byte *oblk, int xs, int ys)
{
	byte *cs, *ct;
	int xs1, ys1;
	int i, j;
	
	xs1=xs>>2; ys1=ys>>2;
	for(i=0; i<ys1; i++)
		for(j=0; j<xs1; j++)
	{
		cs=iblk+((ys1-i-1)*xs1+j)*16;
		ct=oblk+(i*xs1+j)*16;
		BGBBTJ_BC7_FlipBlock(cs, ct);
	}
}

BGBBTJ_API void BGBBTJ_BC7_FlipImageBlocksBC7(
	byte *iblk, byte *oblk, int xs, int ys)
{
	byte *cs, *ct;
	int xs1, ys1;
	int i, j;
	
	xs1=xs>>2; ys1=ys>>2;
	for(i=0; i<ys1; i++)
		for(j=0; j<xs1; j++)
	{
		cs=iblk+((ys1-i-1)*xs1+j)*16;
		ct=oblk+(i*xs1+j)*16;
		bgbbtj_bt1c_memcpy16(ct, cs);
//		BGBBTJ_BC7_FlipBlock(cs, ct);
	}
}
