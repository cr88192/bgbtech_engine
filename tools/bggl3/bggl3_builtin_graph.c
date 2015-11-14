#include "bggl3.h"
#include <math.h>

int Img_StoreTGA(FILE *fd, unsigned char *buf, int w, int h)
{
	int i, flip;
	unsigned char *s, *t, *obuf;

	if(h<0)
	{
		h=-h;
		flip=1;
	}else flip=0;

	fputc(0, fd);			//id length
	fputc(0, fd);			//ctype
	fputc(2, fd);			//itype
	fputc(0, fd); fputc(0, fd);	//cindex
	fputc(0, fd); fputc(0, fd);	//clength
	fputc(0, fd);			//csize
	fputc(0, fd); fputc(0, fd);	//x origin
	fputc(0, fd); fputc(0, fd);	//y origin
	fputc(w, fd); fputc(w>>8, fd);	//width
	fputc(h, fd); fputc(h>>8, fd);	//height
	fputc(32, fd);			//bpp
	fputc(8|(flip?32:0), fd);	//attributes

	obuf=malloc(w*h*4);
	s=buf; t=obuf;
	for(i=0; i<w*h; i++)
	{
		t[0]=s[2]; t[1]=s[1]; t[2]=s[0]; t[3]=s[3];
		s+=4; t+=4;
	}
	fwrite(obuf, w*4, h, fd);
	free(obuf);

	return(0);
}

elem bggl3_graph(BGGL3_State *ctx, elem args)
{
	float min[3], max[3];
	byte clr[4];
	FILE *fd;
	byte *buf;
	char *name;
	elem fcn, a, fa[16];
	float x, y;
	int xs, ys, xo, yo;
	int i, j, k, l, nf;
	int lk[16], ll[16];

	name=BGGL3_CAR(args);
	xs=BGGL3_TOINT(BGGL3_CADR(args));
	ys=BGGL3_TOINT(BGGL3_CADDR(args));

	BGGL3_ToVector(BGGL3_CADDDR(args), min);
	BGGL3_ToVector(BGGL3_CADDDDR(args), max);
	fcn=BGGL3_CADDDDDR(args);

	if(BGGL3_CONSP(fcn))
	{
		a=fcn;
		for(i=0; a && i<16; i++)
		{
			fa[i]=BGGL3_CAR(a);
			a=BGGL3_CDR(a);
		}
		nf=i;
	}else
	{
		fa[0]=fcn;
		nf=1;
	}

	printf("graph '%s' %dx%d {%g %g} to {%g %g}\n",
		name, xs, ys, min[0], min[1], max[0], max[1]);

	buf=malloc(4*xs*ys);
	memset(buf, 0, 4*xs*ys);
	for(i=0; i<(xs*ys); i++)buf[i*4+3]=255;


	xo=(xs*(0-min[0]))/(max[0]-min[0]);
	yo=(ys*(0-min[1]))/(max[1]-min[1]);

	if((yo>=3) && (yo<(ys-3)))
	{
		for(i=0; i<xs; i++)
		{
			buf[(yo*xs+i)*4+0]=255;
			buf[(yo*xs+i)*4+1]=255;
			buf[(yo*xs+i)*4+2]=255;
		}

		x=floor(min[0]);
		while(x<max[0])
		{
			i=(xs*(x-min[0]))/(max[0]-min[0]);
			if((i<0) || (i>=xs)){x++; continue;}

			for(j=(yo-3); j<=(yo+3); j++)
			{
				buf[(j*xs+i)*4+0]=255;
				buf[(j*xs+i)*4+1]=255;
				buf[(j*xs+i)*4+2]=255;
			}
			x++;
		}
	}

	if((xo>=0) && (xo<xs))
	{
		for(i=0; i<ys; i++)
		{
			buf[(i*xs+xo)*4+0]=255;
			buf[(i*xs+xo)*4+1]=255;
			buf[(i*xs+xo)*4+2]=255;
		}

		y=floor(min[1]);
		while(y<max[1])
		{
			i=(ys*(y-min[1]))/(max[1]-min[1]);
			if((i<0) || (i>=xs)){y++; continue;}

			for(j=(xo-3); j<=(xo+3); j++)
			{
				buf[(i*xs+j)*4+0]=255;
				buf[(i*xs+j)*4+1]=255;
				buf[(i*xs+j)*4+2]=255;
			}
			y++;
		}
	}

	for(i=0; i<16; i++)
	{
		lk[i]=-10;
		ll[i]=-10;
	}

	k=0; l=0;
	for(i=0; i<xs; i++)
	{
		x=(i*(max[0]-min[0]))/xs+min[0];

		for(k=0; k<nf; k++)
		{
			l=k+1;

			clr[0]=(l&1)?255:((l&8)?128:0);
			clr[1]=(l&2)?255:((l&8)?128:0);
			clr[2]=(l&4)?255:((l&8)?128:0);

			a=BGGL3_ApplyArg(ctx, fa[k], BGGL3_FLOAT(x));
			y=BGGL3_GETREAL(a);
			j=(ys*(y-min[1]))/(max[1]-min[1]);

			if(lk[k]==-10)lk[k]=j;
			if(lk[k]<0)lk[k]=0;
			if(lk[k]>=ys)lk[k]=ys-1;

			while(1)
			{
				if(BGGL3_COMPLEXP(a) && (y==0))
				{
					lk[k]=-10;
					break;
				}

				buf[(lk[k]*xs+i)*4+0]=clr[0];
				buf[(lk[k]*xs+i)*4+1]=clr[1];
				buf[(lk[k]*xs+i)*4+2]=clr[2];

				if(lk[k]<j)
				{ lk[k]++; if(lk[k]>=ys)break;
				}else if(lk[k]>j)
				{ lk[k]--; if(lk[k]<0)break;
				}else break;
			}

			if(!BGGL3_COMPLEXP(a))
			{
				ll[k]=-10;
				continue;
			}

			if(ll[k]==-10)ll[k]=j;
			if(ll[k]<0)ll[k]=0;
			if(ll[k]>=ys)ll[k]=ys-1;

			y=BGGL3_GETIMAG(a);
			j=(ys*(y-min[1]))/(max[1]-min[1]);
			while(1)
			{
				buf[(ll[k]*xs+i)*4+0]=255-clr[0];
				buf[(ll[k]*xs+i)*4+1]=255-clr[1];
				buf[(ll[k]*xs+i)*4+2]=255-clr[2];

				if(ll[k]<j)
				{ ll[k]++; if(ll[k]>=ys)break;
				}else if(ll[k]>j)
				{ ll[k]--; if(ll[k]<0)break;
				}else break;
			}
		}
	}

	fd=fopen(name, "wb");
	if(!fd)return(NULL);

	Img_StoreTGA(fd, buf, xs, ys);

	return(NULL);
}
