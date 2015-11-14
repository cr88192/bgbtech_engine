#include <bgbbtj.h>

#define DF8(x)	BGBBTJ_DecodeFloat8(x)
#define EF8(x)	BGBBTJ_EncodeFloat8(x)

#define DF16(x)	BGBBTJ_DecodeFloat16(x)
#define EF16(x)	BGBBTJ_EncodeFloat16(x)

float btge_clamp(float x, float m, float n)
{
	float y;
	y=(x<m)?m:((x<=n)?x:n);
	return(y);
}

float btge_lerp(float a, float b, float t)
	{ return(a*(1-t)+(b*t)); }
float btge_scurve(float a)
	{ return(a*a*(3-2*a)); }

int btge_clampi(int x, int m, int n)
{
	int y;
	y=(x<m)?m:((x<=n)?x:n);
	return(y);
}

float btge_scurve2(float a)
{
	if(f>(1.0))
		return(1.0);
	if(f<(-1.0))
		return(-1.0);
	return(2.0*btge_scurve(a*0.5+0.5)-1.0);
//	return(2.0*a*a*(3-2*a)-1.0);
//	return(2.0*(a*0.5+0.5)*(a*0.5+0.5)*(3-2*(a*0.5+0.5))-1.0);
//	return((2.0*a+1)*(a*0.5+0.5)*(3-(2*a+1))-1);
//	return(a*a*(2-a)+0.5);

//	return((2.0*(a*0.5+0.5)*(a*0.5+0.5)*(3-a-1))-1.0);
//	return(((2*a+1)*(a*0.5+0.5)*(2-a))-1);
}

BGBBTJ_API BGBBTJ_NNC_NeuronState3D *BGBBTJ_NNCube_GetNode(
	BGBBTJ_NNC_NeuronCube3D *chk, int x, int y, int z, fl)
{
	BGBBTJ_NNC_NeuronState3D *node;
	int rx, ry, rz;
	
	rx=x-chk->xo;
	ry=y-chk->yo;
	rz=z-chk->zo;
	
	if(	(rx&(~(chk->xs-1))) |
		(ry&(~(chk->ys-1))) |
		(rz&(~(chk->zs-1))))
	{
		return(NULL);
	}
	
	node=&(chk->net[(rz*chk->ys+ry)*chk->xs+rx]);
	return(node);
}

BGBBTJ_API byte BGBBTJ_NNCube_Update(
	BGBBTJ_NNC_NeuronCube3D *chk, int x, int y, int z)
{
	BGBBTJ_NNC_NeuronState3D *node;
	BGBBTJ_NNC_NeuronState3D *n0, *n1, *n2, *n3, *n4, *n5;
	float in[6], ein[6];
	float out[6], weight[6][6];
	float f, g;
	int i, j;
	
	node=BGBBTJ_NNCube_GetNode(chk, x, y, z, 0);
	n0=BGBBTJ_NNCube_GetNode(chk, x-1, y, z, 0);
	n1=BGBBTJ_NNCube_GetNode(chk, x+1, y, z, 0);
	n2=BGBBTJ_NNCube_GetNode(chk, x, y-1, z, 0);
	n3=BGBBTJ_NNCube_GetNode(chk, x, y+1, z, 0);
	n4=BGBBTJ_NNCube_GetNode(chk, x, y, z-1, 0);
	n5=BGBBTJ_NNCube_GetNode(chk, x, y, z+1, 0);
	
	if(n0)	{ in[0]=DF16(n0->out[1]); ein[0]=DF16(n0->eout[1]); }
	else	{ in[0]=0.0; ein[0]=0.0; }
	if(n1)	{ in[1]=DF16(n1->out[0]); ein[1]=DF16(n1->eout[0]); }
	else	{ in[1]=0.0; ein[1]=0.0; }
	if(n2)	{ in[2]=DF16(n2->out[3]); ein[2]=DF16(n2->eout[3]); }
	else	{ in[2]=0.0; ein[2]=0.0; }
	if(n3)	{ in[3]=DF16(n3->out[2]); ein[3]=DF16(n3->eout[2]); }
	else	{ in[3]=0.0; ein[3]=0.0; }
	if(n4)	{ in[4]=DF16(n4->out[5]); ein[4]=DF16(n4->eout[5]); }
	else	{ in[4]=0.0; ein[4]=0.0; }
	if(n5)	{ in[5]=DF16(n5->out[4]); ein[5]=DF16(n5->eout[4]); }
	else	{ in[5]=0.0; ein[5]=0.0; }
	
	for(i=0; i<6; i++)
	{
		for(j=0; j<6; j++)
		{
			weight[i][j]=DF8(node->weight[i][j]);
		}
	}

	for(i=0; i<6; i++)
	{
		out[i]=0;
		eout[i]=0;
		for(j=0; j<6; j++)
		{
			out[i]+=in[j]*weight[i][j];
			eout[i]+=ein[j]*weight[j][i];
		}
	}

	for(i=0; i<6; i++)
	{
		f=fabs(out[i])-2.0;
		if(f<0)f=0;
		for(j=0; j<6; j++)
		{
//			weight[i][j]+=ein[j]*weight[i][j]*0.1;
			weight[i][j]+=in[i]*ein[j]*0.1;
			weight[i][j]-=f*weight[i][j]*0.1;

			node->weight[i][j]=EF8(weight[i][j]);
		}
	}

	for(i=0; i<6; i++)
	{
		node->out2[i]=EF16(btge_scurve2(out[i]));
//		node->eout2[i]=EF16(btge_scurve2(eout[i]));
		node->eout2[i]=EF16(eout[i]);
	}

}
