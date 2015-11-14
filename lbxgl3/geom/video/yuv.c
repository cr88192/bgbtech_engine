#include <lbxgl.h>
#include <lbxgl_vid_avi.h>

//#define YUV_XS	176
//#define YUV_YS	144

#define YUV_XS		720
//#define YUV_YS		576
#define YUV_YS		486
#define YUV_FPS		30
#define YUV_FRAMES	220

typedef struct {
VFILE *fd;
byte *fcbuf, *fdbuf;	//coded and decoded frame
byte *flbuf, *fnbuf;	//last and next frames

int frnum;
float frame_time;
}LBXGL_YUV_Context;

LBXGL_YUV_Context *YUV_LoadYUV(char *name) //AH:ignore
{
	LBXGL_YUV_Context *ctx;

	ctx=ObjType_New("lbxgl_avi_t", sizeof(LBXGL_YUV_Context));

	ctx->fd=vffopen(name, "rb");
	if(!ctx->fd)return(NULL);

	ctx->fcbuf=malloc(YUV_XS*YUV_YS*4);
	ctx->fdbuf=malloc(YUV_XS*YUV_YS*4);

	ctx->flbuf=malloc(YUV_XS*YUV_YS*2);
	ctx->fnbuf=malloc(YUV_XS*YUV_YS*2);

	return(ctx);
}

LBXGL_Video_Stats *YUV_GetStats(LBXGL_YUV_Context *ctx) //AH:ignore
{
	LBXGL_Video_Stats *tmp;
	int i;

	tmp=kalloc(sizeof(LBXGL_Video_Stats));

	tmp->width=YUV_XS;
	tmp->height=YUV_YS;

	tmp->frametime=1.0/YUV_FPS;
	tmp->num_frames=YUV_FRAMES;

	i=RIFF_MAKEHEXTAG(0);
	tmp->vid_fcc=i;
	tmp->vid_cmpr=i;

	i=RIFF_MAKEHEXTAG(0);
	tmp->aud_fcc=i;
	tmp->aud_cmpr=i;

	return(tmp);
}

void *YUV_FrameRaw(LBXGL_YUV_Context *ctx, float dt) //AH:ignore
{
	static int nhf;
	int df, hf;

	int i, j, k, l;
	int y, u, v;
	int y2, u2, v2;
	int y3, u3, v3;
	int r, g, b;
	int c1, c2;
	float f;

	ctx->frame_time-=dt;
	df=0;
	while(ctx->frame_time<=0)
	{
		df=1;

		if(nhf)
		{
			memcpy(ctx->flbuf, ctx->fcbuf, YUV_XS*YUV_YS*2);
			memcpy(ctx->fcbuf, ctx->fnbuf, YUV_XS*YUV_YS*2);

			i=vfread(ctx->fnbuf, 1, YUV_XS*YUV_YS*2, ctx->fd);
			if(i<=0)vfseek(ctx->fd, 0, 0);
			ctx->frnum++;

			ctx->frame_time+=1.0/YUV_FPS;
//			nhf=0;
			continue;
		}

		nhf=1;
		ctx->frame_time+=1.0/YUV_FPS;
	}

	if(!df)return(ctx->fdbuf);

	hf=!nhf;
	for(i=0; i<YUV_YS; i++)
		for(j=0; j<YUV_XS; j++)
	{
		y=ctx->fcbuf[((i*YUV_XS)+j)*2+1];
		u=ctx->fcbuf[((i*YUV_XS)+(j&(~1)))*2+0];
		v=ctx->fcbuf[((i*YUV_XS)+(j&(~1)))*2+2];

#if 0
		if((i&1)==hf)
		{
//			k=(((i&(~1))+hf)*YUV_XS)+j;
//			k=((i&(~1))*YUV_XS)+j;
			k=(i*YUV_XS)+j;

			l=(k&(~1))<<1;
			y=ctx->fcbuf[(k<<1)+1];
			u=ctx->fcbuf[l+0];
			v=ctx->fcbuf[l+2];
		}
		else if(hf)
		{
			k=((i+1)*YUV_XS)+j;
			l=(k&(~1))<<1;
			y=ctx->fcbuf[(k<<1)+1];
			u=ctx->fcbuf[l+0];
			v=ctx->fcbuf[l+2];

			if(i>0)
			{
				k=((i-1)*YUV_XS)+j;
				l=(k&(~1))<<1;
				y2=ctx->fcbuf[(k<<1)+1];
				u2=ctx->fcbuf[l+0];
				v2=ctx->fcbuf[l+2];
				y=(y+y2)/2;
				u=(u+u2)/2;
				v=(v+v2)/2;
			}

			k=(i*YUV_XS)+j;
			l=(k&(~1))<<1;
			y2=ctx->fcbuf[(k<<1)+1];
			u2=ctx->fcbuf[l+0];
			v2=ctx->fcbuf[l+2];

			y3=ctx->fnbuf[(k<<1)+1];
			u3=ctx->fnbuf[l+0];
			v3=ctx->fnbuf[l+2];
			c1=(y<y3)?(y3-y):(y-y3);
//			c1+=(u<u3)?(u3-u):(u-u3);
//			c1+=(v<v3)?(v3-v):(v-v3);
			c2=(y2<y3)?(y3-y2):(y2-y3);
//			c2+=(u2<u3)?(u3-u2):(u2-u3);
//			c2+=(v2<v3)?(v3-v2):(v2-v3);

			if(!c2)f=0;
				else f=1-(((float)c1/(float)c2)*0.10);
//			f=1;
			if(f>1.0)f=1.0;
			if(f<0.0)f=0.0;

			y=(y*f)+(y2*(1.0-f));
			u=(u*f)+(u2*(1.0-f));
			v=(v*f)+(v2*(1.0-f));
		}else
		{
			k=((i-1)*YUV_XS)+j;
			l=(k&(~1))<<1;
			y=ctx->fcbuf[(k<<1)+1];
			u=ctx->fcbuf[l+0];
			v=ctx->fcbuf[l+2];

			if(i<(YUV_YS-2))
			{
				k=((i+1)*YUV_XS)+j;
				l=(k&(~1))<<1;
				y2=ctx->fcbuf[(k<<1)+1];
				u2=ctx->fcbuf[l+0];
				v2=ctx->fcbuf[l+2];

				y=(y+y2)/2;
				u=(u+u2)/2;
				v=(v+v2)/2;
			}

			k=(i*YUV_XS)+j;
			l=(k&(~1))<<1;
			y2=ctx->flbuf[(k<<1)+1];
			u2=ctx->flbuf[l+0];
			v2=ctx->flbuf[l+2];

			y3=ctx->fcbuf[(k<<1)+1];
			u3=ctx->fcbuf[l+0];
			v3=ctx->fcbuf[l+2];
			c1=(y<y3)?(y3-y):(y-y3);
//			c1+=(u<u3)?(u3-u):(u-u3);
//			c1+=(v<v3)?(v3-v):(v-v3);
			c2=(y2<y3)?(y3-y2):(y2-y3);
//			c2+=(u2<u3)?(u3-u2):(u2-u3);
//			c2+=(v2<v3)?(v3-v2):(v2-v3);

			if(!c2)f=0;
				else f=1-(((float)c1/(float)c2)*0.10);
//			f=1;
			if(f>1.0)f=1.0;
			if(f<0.0)f=0.0;

			y=(y*f)+(y2*(1.0-f));
			u=(u*f)+(u2*(1.0-f));
			v=(v*f)+(v2*(1.0-f));
		}
#endif

#if 0
		else if(!hf)
		{
			k=(i*YUV_XS)+j;
			l=(k&(~1))<<1;

			y=ctx->fcbuf[(k<<1)+1];
			u=ctx->fcbuf[l+0];
			v=ctx->fcbuf[l+2];

			y2=ctx->fnbuf[(k<<1)+1];
			u2=ctx->fnbuf[l+0];
			v2=ctx->fnbuf[l+2];
			y=(y+y2)/2;
			u=(u+u2)/2;
			v=(v+v2)/2;
		}else
		{
			k=(i*YUV_XS)+j;
			l=(k&(~1))<<1;

			y=ctx->flbuf[(k<<1)+1];
			u=ctx->flbuf[l+0];
			v=ctx->flbuf[l+2];

			y2=ctx->fcbuf[(k<<1)+1];
			u2=ctx->fcbuf[l+0];
			v2=ctx->fcbuf[l+2];
			y=(y+y2)/2;
			u=(u+u2)/2;
			v=(v+v2)/2;
		}
#endif

#if 0
		else if(hf)
		{
			k=((i+1)*YUV_XS)+j;
			l=(k&(~1))<<1;
			y=ctx->fcbuf[(k<<1)+1];
			u=ctx->fcbuf[l+0];
			v=ctx->fcbuf[l+2];

			if(i>0)
			{
				k=((i-1)*YUV_XS)+j;
				l=(k&(~1))<<1;
				y2=ctx->fcbuf[(k<<1)+1];
				u2=ctx->fcbuf[l+0];
				v2=ctx->fcbuf[l+2];
				y=(y+y2)/2;
				u=(u+u2)/2;
				v=(v+v2)/2;
			}

			k=(i*YUV_XS)+j;
			l=(k&(~1))<<1;
			y2=ctx->flbuf[(k<<1)+1];
			u2=ctx->flbuf[l+0];
			v2=ctx->flbuf[l+2];
			y=(y+y2)/2;
			u=(u+u2)/2;
			v=(v+v2)/2;
		}else
		{
			k=((i-1)*YUV_XS)+j;
			l=(k&(~1))<<1;
			y=ctx->fcbuf[(k<<1)+1];
			u=ctx->fcbuf[l+0];
			v=ctx->fcbuf[l+2];

			if(i<(YUV_YS-2))
			{
				k=((i+1)*YUV_XS)+j;
				l=(k&(~1))<<1;
				y2=ctx->fcbuf[(k<<1)+1];
				u2=ctx->fcbuf[l+0];
				v2=ctx->fcbuf[l+2];

				y=(y+y2)/2;
				u=(u+u2)/2;
				v=(v+v2)/2;
			}

			k=(i*YUV_XS)+j;
			l=(k&(~1))<<1;
			y2=ctx->flbuf[(k<<1)+1];
			u2=ctx->flbuf[l+0];
			v2=ctx->flbuf[l+2];
			y=(y+y2)/2;
			u=(u+u2)/2;
			v=(v+v2)/2;
		}
#endif

//		u=128;
//		v=128;

		b=1.164*(y-16)+2.018*(u-128);
		g=1.164*(y-16)-0.813*(v-128)-0.391*(u-128);
		r=1.164*(y-16)+1.596*(v-128);

		if(b<0)b=0;
		if(b>255)b=255;
		if(g<0)g=0;
		if(g>255)g=255;
		if(r<0)r=0;
		if(r>255)r=255;

		k=(i*YUV_XS)+j;
		ctx->fdbuf[(k<<2)+0]=r;
		ctx->fdbuf[(k<<2)+1]=g;
		ctx->fdbuf[(k<<2)+2]=b;
		ctx->fdbuf[(k<<2)+3]=255;

//		ctx->fdbuf[((i*YUV_XS)+j)*4+0]=r;
//		ctx->fdbuf[((i*YUV_XS)+j)*4+1]=g;
//		ctx->fdbuf[((i*YUV_XS)+j)*4+2]=b;
//		ctx->fdbuf[((i*YUV_XS)+j)*4+3]=255;
	}
//	hf=!hf;

	return(ctx->fdbuf);
}

float YUV_GetPosition(LBXGL_YUV_Context *ctx) //AH:ignore
{
	float t;

	return(0);
}
