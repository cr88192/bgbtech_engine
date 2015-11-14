#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#include <bgbgc.h>
#include <bgbgal.h>

float main_test_0(bgalContext *ctx, bgalMember *obj, void *data)
{
	float f, g;
	int i;

	g=0;
	for(i=0; i<8; i++)
	{
		f=sin(M_PI/4*i);
		g+=fabs(obj->vec[i]-f);
	}

	return(g);
}

void main_status_0(bgalContext *ctx)
{
	bgalMember *cur;
	int i, j, k;

	cur=ctx->live; i=0;
	while(cur && (i<4))
	{
		printf("%d %f: ", i, cur->weight);
		for(j=0; j<8; j++)printf("%1.3f ", cur->vec[j]);
		printf("\n");
		i++; cur=cur->lnext;
	}

	printf("\n");
}

float main_test_1(bgalContext *ctx, bgalMember *obj, void *data)
{
	float arr[256];
	float f, g;
	int i, j, k;

//	i=bgalEval(obj, &f, 1, arr, &j);

#if 1
	g=0;
	for(i=0; i<8; i++)
	{
		j=rand();
		arr[0]=j; f=sin(M_PI/4*j);
//		arr[0]=j; f=sin(j);

		k=bgalEval(obj, arr, 1, arr, &j);
		if((k<0) || (j<1))return(999999.0);

		f=fabs(arr[j-1]-f);
		g+=pow(1+f, 3);
	}
#endif

	g+=g*0.01*strlen(obj->prog[0]);

	return(g);
}

void main_status_1(bgalContext *ctx)
{
	bgalMember *cur;
	int i, j, k;

	cur=ctx->live; i=0;
	while(cur && (i<4))
	{
		printf("%d %f: ", i, cur->weight);
//		for(j=0; j<8; j++)printf("%1.3f ", cur->vec[j]);

		if(cur->prog[0])
		{
			for(j=0; j<16; j++)
			{
				k=cur->prog[0][j];
				if(!k)break;
				printf("%3d ", BGAL_UnGray(k));
			}

			cur->prog[0][12]=0;
		}

		printf("\n");
		i++; cur=cur->lnext;
	}

	printf("\n");
}

int main(int argc, char *argv[])
{
	bgalContext *ctx;
	int i, j;

	gc_init(&argc);

	thSleep(0);

//	ctx=bgalNewGenMinCtx(8, 4096, 256, 16);
//	ctx->test=main_test_0;
//	ctx->step_status=main_status_0;

	ctx=bgalNewGenProgMinCtx(8, 0, 1, 4096, 1024, 64);
	ctx->test=main_test_1;
	ctx->step_status=main_status_1;

	bgalEvolve(ctx, NULL);

#if 0
	thSleep(16);
	printf("    ", j);
	for(j=0; j<16; j++)printf("%2X ", j);
	printf("\n\n");

	for(i=0; i<16; i++)
	{
		printf("%2X  ", i);
		for(j=0; j<16; j++)
//			printf("%3d ", BGAL_UnGray((i<<4)|j));
			printf("%2X ", BGAL_Gray((i<<4)|j));
		printf("\n");
	}
#endif

	return(0);
}
