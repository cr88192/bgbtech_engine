#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// #include <bgbbtj.h>

typedef unsigned char byte;
typedef unsigned int u32;

typedef struct TargaHeader_s {
	unsigned char 	id_length, ctype, itype;
	unsigned short	cindex, clength;
	unsigned char	csize;
	unsigned short	x_origin, y_origin, width, height;
	unsigned char	bpp, attributes;
} TargaHeader;

int BGBBTJ_Img_StoreTGA(FILE *fd, byte *buf, int w, int h)
{
	TargaHeader head;
	int i;
	byte *s;

	head.id_length=0;
	head.ctype=0;
	head.itype=2;
	head.cindex=0;
	head.clength=0;
	head.csize=0;
	head.x_origin=0;
	head.y_origin=0;
	head.width=w;
	head.height=h;
	head.bpp=24;
	head.attributes=0;

	fputc(head.id_length, fd);
	fputc(head.ctype, fd);
	fputc(head.itype, fd);

	fputc(head.cindex, fd);
	fputc(head.cindex>>8, fd);
	fputc(head.clength, fd);
	fputc(head.clength>>8, fd);

	fputc(head.csize, fd);

	fputc(head.x_origin, fd);
	fputc(head.x_origin>>8, fd);
	fputc(head.y_origin, fd);
	fputc(head.y_origin>>8, fd);

	fputc(head.width, fd);
	fputc(head.width>>8, fd);
	fputc(head.height, fd);
	fputc(head.height>>8, fd);

	fputc(head.bpp, fd);
	fputc(head.attributes, fd);

//	vfwrite(buf, w*4, h, fd);

	s=buf;
	for(i=0; i<w*h; i++)
	{
		fputc(s[2], fd);
		fputc(s[1], fd);
		fputc(s[0], fd);
		s+=4;
	}

	return(0);
}

int BGBBTJ_TGA_SaveRaw(char *name, byte *buf, int w, int h)
{
	FILE *fd;
	
	fd=fopen(name, "wb");
	if(!fd)
	{
		return(-1);
	}
	return(BGBBTJ_Img_StoreTGA(fd, buf, w, h));
}

int main(int argc, char *argv[])
{
	byte patbuf[64*64*4];
	u32 patarr[256];
	byte pat[16], pat1[16];
	int p0, p1, p2, p3, p4, p5, p6, p7, p8;
	float f, g;
	int i, j, k, l;
	
	for(i=0; i<240; i++)
	{
		memset(pat, 0, 16);
		
		if(i&128)
		{
			pat[0]=3;	pat[1]=3;
			pat[4]=3;	pat[5]=3;
		}

		if(i&64)
		{
			pat[2]=3;	pat[3]=3;
			pat[6]=3;	pat[7]=3;
		}

		if(i&32)
		{
			pat[ 8]=3;	pat[ 9]=3;
			pat[12]=3;	pat[13]=3;
		}

		if(i&16)
		{
			pat[10]=3;	pat[11]=3;
			pat[14]=3;	pat[15]=3;
		}
		
		if(i&8)
		{
			pat[4]=3;	pat[5]=3;	pat[ 6]=3;	pat[ 7]=3;
			pat[8]=3;	pat[9]=3;	pat[10]=3;	pat[11]=3;
		}
	
		if(i&4)
		{
			pat[1]=3;	pat[ 2]=3;	pat[ 5]=3;	pat[ 6]=3;
			pat[9]=3;	pat[10]=3;	pat[13]=3;	pat[14]=3;
		}

		for(j=0; j<4; j++)
			for(k=0; k<4; k++)
		{
			if(j>0)
			{
				p0=(k>0)?pat[(j-1)*4+(k-1)]:pat[(j-1)*4+k];
				p1=pat[(j-1)*4+k];
				p2=(k<3)?pat[(j-1)*4+(k+1)]:pat[(j-1)*4+k];
			}else
			{
				p0=(k>0)?pat[j*4+(k-1)]:pat[j*4+k];
				p1=pat[j*4+k];
				p2=(k<3)?pat[j*4+(k+1)]:pat[j*4+k];
			}

			p3=(k>0)?pat[j*4+(k-1)]:pat[j*4+k];
			p4=pat[j*4+k];
			p5=(k<3)?pat[j*4+(k+1)]:pat[j*4+k];

			if(j<3)
			{
				p6=(k>0)?pat[(j+1)*4+(k-1)]:pat[(j+1)*4+k];
				p7=pat[(j+1)*4+k];
				p8=(k<3)?pat[(j+1)*4+(k+1)]:pat[(j+1)*4+k];
			}else
			{
				p6=(k>0)?pat[j*4+(k-1)]:pat[j*4+k];
				p7=pat[j*4+k];
				p8=(k<3)?pat[j*4+(k+1)]:pat[j*4+k];
			}

			switch(i&3)
			{
			case 0:
				l=p4;
				break;
			case 1:
				f=	0.13*p0 + 0.25*p1 + 0.13*p2 +
					0.25*p3 + 1.50*p4 + 0.25*p5 +
					0.13*p6 + 0.25*p7 + 0.13*p8;
				f=f/(0.13+0.25+0.13+
					 0.25+1.50+0.25+
					 0.13+0.25+0.13);
				l=f+0.5;
				break;
			case 2:
				f=	0.13*p0 + 0.25*p1 + 0.13*p2 +
					0.25*p3 + 1.50*p4 + 0.25*p5 +
					0.13*p6 + 0.25*p7 + 0.13*p8;
				f=f/(0.13+0.25+0.13+
					 0.25+1.50+0.25+
					 0.13+0.25+0.13);
				l=f;
				break;
			case 3:
				f=	0.13*p0 + 0.25*p1 + 0.13*p2 +
					0.25*p3 + 1.50*p4 + 0.25*p5 +
					0.13*p6 + 0.25*p7 + 0.13*p8;
				f=f/(0.13+0.25+0.13+
					 0.25+1.50+0.25+
					 0.13+0.25+0.13);
				l=f+0.9999;
				break;
			}

			pat1[j*4+k]=l;
		}


		for(j=0; j<16; j++)
			pat[j]=pat1[j];
			
		p0=	(pat[ 0]<<30)|(pat[ 1]<<28)|(pat[ 2]<<26)|(pat[ 3]<<24)|
			(pat[ 4]<<22)|(pat[ 5]<<20)|(pat[ 6]<<18)|(pat[ 7]<<16)|
			(pat[ 8]<<14)|(pat[ 9]<<12)|(pat[10]<<10)|(pat[11]<< 8)|
			(pat[12]<< 6)|(pat[13]<< 4)|(pat[14]<< 2)|(pat[15]<< 0);

		patarr[i]=p0;

//		if(!(i%4))
//		{
//			printf("//0x%02X\n", i-4);
//		}
//		printf("0x%08X, ", p0);
	}

	for(i=240; i<256; i++)
	{
		memset(pat, 0, 16);

		for(j=0; j<4; j++)
			for(k=0; k<4; k++)
		{
			l=1;
			if((j^k)&1)l=0;
//			if((i&8) && (k&1))l=0;
//			if((i&4) && (j&1))l=0;
//			if((i&8) && ((j^k)&2) && (k&1))l=0;
			if((i&4) && ((j^k)&2) && (j&1))l=0;
			if((i&8) && ((j^k)&2) && (j&2))l=0;

			switch(i&3)
			{
			case 0:
				l=l?3:0;
				break;
			case 1:
				l=l?2:1;
				break;
			case 2:
				l=l?1:0;
				break;
			case 3:
				l=l?2:0;
				break;
			}

			pat[j*4+k]=l;
		}

		p0=	(pat[ 0]<<30)|(pat[ 1]<<28)|(pat[ 2]<<26)|(pat[ 3]<<24)|
			(pat[ 4]<<22)|(pat[ 5]<<20)|(pat[ 6]<<18)|(pat[ 7]<<16)|
			(pat[ 8]<<14)|(pat[ 9]<<12)|(pat[10]<<10)|(pat[11]<< 8)|
			(pat[12]<< 6)|(pat[13]<< 4)|(pat[14]<< 2)|(pat[15]<< 0);

		patarr[i]=p0;

//		if(!(i%4))
//		{
//			printf("//0x%02X\n", i-4);
//		}
//		printf("0x%08X, ", p0);
	}

	patarr[0]=0x00000000;
	patarr[1]=0x003C3C00;
	patarr[2]=0x00282800;
	patarr[3]=0x28BEBE28;
	
	patarr[255]=0xFFFFFFFF;
	
//	printf("//0x%02X\n", i-4);

//	printf("\n");

	for(i=0; i<256; i++)
	{
		for(j=i+1; j<256; j++)
		{
			if(patarr[j]==patarr[i])
			{
//				patarr[j]=0;
				if(j>4)
				{
					printf("%02X ", j);
					k=patarr[j-2]-patarr[j-1];
					l=patarr[j-2]+k;
					patarr[j]=l;
				}
			}
		}
	}

	printf("\n");

	for(i=0; i<(256/4); i++)
	{
		for(j=0; j<4; j++)
		{
			printf("0x%08X, ", patarr[i*4+j]);
		}
		printf("//0x%02X\n", i*4);
	}

	for(i=0; i<16; i++)
		for(j=0; j<16; j++)
	{
		p8=patarr[i*16+j];
		for(k=0; k<4; k++)
			for(l=0; l<4; l++)
		{
			p0=(p8>>(30-(k*4+l)*2))&3;

//			p1=((i*4+k)*64+(j*4+l));
			p1=((63-(i*4+k))*64+(j*4+l));

			patbuf[p1*4+0]=p0*63;
			patbuf[p1*4+1]=p0*63;
			patbuf[p1*4+2]=p0*63;
			patbuf[p1*4+3]=255;

//			patbuf[((i*4+k)*64+(j*4+l))*4+0]=p0*63;
//			patbuf[((i*4+k)*64+(j*4+l))*4+1]=p0*63;
//			patbuf[((i*4+k)*64+(j*4+l))*4+2]=p0*63;
//			patbuf[((i*4+k)*64+(j*4+l))*4+3]=255;
		}
	}

	BGBBTJ_TGA_SaveRaw("pixpat0_dump0.tga", patbuf, 64, 64);
}
