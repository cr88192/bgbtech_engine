#include <pdlib.h>

#ifndef FOURCC
#define FOURCC(a, b, c, d) ((a)|((b)<<8)|((c)<<16)|((d)<<24))
#endif


#define FCC_PNG_LOW	FOURCC(0x89, 'P', 'N', 'G')
#define FCC_PNG_HIGH	FOURCC(0x0D, 0x0A, 0x1A, 0x0A)

#define FCC_IHDR	FOURCC('I', 'H', 'D', 'R')
#define FCC_IEND	FOURCC('I', 'E', 'N', 'D')
#define FCC_IDAT	FOURCC('I', 'D', 'A', 'T')

int read_cb(byte *buf, int sz, void *str)
{
	byte **bufp;

	bufp=(byte **)str;
	memcpy(buf, *bufp, sz);
	*bufp=*bufp+sz;
	return(sz);
}

int write_cb(byte *buf, int sz, void *str)
{
	byte **bufp;

	bufp=(byte **)str;
	memcpy(*bufp, buf, sz);
	*bufp=*bufp+sz;
	return(sz);
}

char *PDPNG_Fourcc2String(uint fcc)
{
	static char buf[64], pos;
	int i, j;

	if(pos>56)pos=0;

	for(i=0; i<4; i++)
	{
		j=(fcc>>(i*8))&0xFF;
		if((j<' ') || (j>'~'))j='~';
		buf[pos+i]=j;
	}
	buf[pos+4]=0;
	return(buf+pos);
}

int PDPNG_Paeth(int a, int b, int c)
{
	int p, pa, pb, pc;

	p=a+b-c;
	pa=(p>a)?(p-a):(a-p);
	pb=(p>b)?(p-b):(b-p);
	pc=(p>c)?(p-c):(c-p);

	p=(pa<=pb)?((pa<=pc)?a:c):((pb<=pc)?b:c);
	return(p);
}


byte *PDPNG_Load(VFILE *ifd, int *w, int *h)
{
	PDZ_State *ctx;
	uint fcc0, fcc1, len, crc, end;
	int xs, ys, bpp, clr, cm, fm, im, ssz;
	int i, j, k, pa, pb, pc;
	byte *buf, *bufe;
	byte *buf2, *buf2e;
	byte *s, *t;

	fcc0=IFF_ReadFourcc(ifd);
	fcc1=IFF_ReadFourcc(ifd);
	if((fcc0!=FCC_PNG_LOW) || (fcc1!=FCC_PNG_HIGH))
	{
		printf("damaged fourcc or non-png\n");
		return(NULL);
	}

	while(!vfeof(ifd))
	{
		len=IFF_ReadInt32BE(ifd);
		fcc0=IFF_ReadFourcc(ifd);

		if(fcc0==FCC_IEND)break;

		end=vftell(ifd)+len;
		if(fcc0==FCC_IHDR)
		{
			xs=IFF_ReadInt32BE(ifd);
			ys=IFF_ReadInt32BE(ifd);

			bpp=vfgetc(ifd);
			clr=vfgetc(ifd);
			cm=vfgetc(ifd);
			fm=vfgetc(ifd);
			im=vfgetc(ifd);

			if(bpp!=8)
			{
				kprint("only 8 bits/channel supported\n");
				return(NULL);
			}
			if((clr!=2) && (clr!=6))
			{
				kprint("only RGB(A) images supported\n");
				return(NULL);
			}
			if(cm!=0)
			{
				kprint("only deflated images supported\n");
				return(NULL);
			}
			if(fm!=0)
			{
				kprint("unknown filter method\n");
				return(NULL);
			}
			if(im!=0)
			{
				kprint("interlacing not supported\n");
				return(NULL);
			}

			buf=malloc(xs*ys*8);
			bufe=buf;

			ssz=0;
			if(clr==2)ssz=3;
			if(clr==6)ssz=4;

//			kprint("%dx%dx%dbpp\n", xs, ys, ssz*bpp);
//			kprint("clr %d cm %d fm %d im %d\n",
//				clr, cm, fm, im);
		}

		if(fcc0==FCC_IDAT)
		{
			vfread(bufe, 1, len, ifd);
			bufe+=len;
		}

		vfseek(ifd, end, 0);
		crc=IFF_ReadInt32BE(ifd);
	}

	buf2=malloc(xs*ys*8);
	buf2e=buf2;

	bufe=buf+2;

	ctx=PDZ_NewState(262144, 262144);
	ctx->read_f=&read_cb;
	ctx->write_f=&write_cb;
	ctx->istr=&bufe;
	ctx->ostr=&buf2e;
	PDZ_DecodeStream(ctx);
	free(buf);

	buf=malloc(xs*ys*4);
	s=buf2;
	t=buf;

	for(i=0; i<ys; i++)
	{
		fm=*s++;

		for(j=0; j<xs; j++)
		{
			for(k=0; k<ssz; k++)
			{
				if(i && j)
				{
//					pa=(t-xs*4-4)[k];
//					pb=(t-xs*4)[k];
//					pc=(t-4)[k];

					pa=buf[(((ys-i)*xs)+j-1)*4+k];
					pb=buf[(((ys-i)*xs)+j)*4+k];
					pc=buf[(((ys-i-1)*xs)+j-1)*4+k];
				}else if(i)
				{
					pa=0;
//					pb=(t-xs*4)[k];
					pb=buf[(((ys-i)*xs)+j)*4+k];
					pc=0;
				}else if(j)
				{
					pa=0;
					pb=0;
//					pc=(t-4)[k];
					pc=buf[(((ys-i-1)*xs)+j-1)*4+k];
				}else
				{
					pa=0;
					pb=0;
					pc=0;
				}

				switch(fm)
				{
				case 0:
//					t[k]=s[k];
					buf[(((ys-i-1)*xs)+j)*4+k]=s[k];
					break;
				case 1:
//					t[k]=s[k]+pc;
					buf[(((ys-i-1)*xs)+j)*4+k]=s[k]+pc;
					break;
				case 2:
//					t[k]=s[k]+pb;
					buf[(((ys-i-1)*xs)+j)*4+k]=s[k]+pb;
					break;
				case 3:
//					t[k]=s[k]+((pc+pb)>>1);
					buf[(((ys-i-1)*xs)+j)*4+k]=
						s[k]+((pc+pb)>>1);
					break;
				case 4:
//					t[k]=s[k]+PDPNG_Paeth(pc, pb, pa);
					buf[(((ys-i-1)*xs)+j)*4+k]=
						s[k]+PDPNG_Paeth(pc, pb, pa);
					break;
				default:
					break;
				}
			}
//			if(ssz==3)t[3]=255;
			if(ssz==3)buf[(((ys-i-1)*xs)+j)*4+3]=255;

			s+=ssz;
//			t+=4;
		}
	}
	free(buf2);

	*w=xs;
	*h=ys;
	return(buf);
}
