#include <bgbbtj.h>

#ifndef FOURCC
#define FOURCC(a, b, c, d) ((a)|((b)<<8)|((c)<<16)|((d)<<24))
#endif


#define FCC_PNG_LOW	FOURCC(0x89, 'P', 'N', 'G')
#define FCC_PNG_HIGH	FOURCC(0x0D, 0x0A, 0x1A, 0x0A)

#define FCC_IHDR	FOURCC('I', 'H', 'D', 'R')
#define FCC_IEND	FOURCC('I', 'E', 'N', 'D')
#define FCC_IDAT	FOURCC('I', 'D', 'A', 'T')
#define FCC_PLTE	FOURCC('P', 'L', 'T', 'E')
#define FCC_tRNS	FOURCC('t', 'R', 'N', 'S')

uint BGBBTJ_PNG_DataAdler32(void *buf, int sz, uint lcrc)
{
	byte *s;
	int i, c, s1, s2;

	s=buf;
	s1=lcrc&0xFFFF;
	s2=(lcrc>>16)&0xFFFF;
	for(i=0; i<sz; i++)
	{
		s1=(s1+(s[i]))%65521;
		s2=(s1+s2)%65521;
	}
	return((s2<<16)+s1);
}

uint BGBBTJ_PNG_CRC32(void *buf, int sz, uint lcrc)
{
	static uint crctab[256];
	static int init=0;

	byte *s;
	uint c;
	int i, j;

	if(!init)
	{
		init=1;

		for(i=0; i<256; i++)
		{
			c=i;
			for(j=0; j<8; j++)
				if(c&1)c=0xedb88320^(c>>1);
					else c>>=1;
			crctab[i]=c;
		}
	}

	c=lcrc; s=buf; i=sz;
	while(i--)c=crctab[(c^(*s++))&0xFF]^(c>>8);
	return(c);
}

char *BGBBTJ_PNG_Fourcc2String(uint fcc)
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

int BGBBTJ_PNG_Paeth(int a, int b, int c)
{
	int p, pa, pb, pc;

	p=a+b-c;
	pa=(p>a)?(p-a):(a-p);
	pb=(p>b)?(p-b):(b-p);
	pc=(p>c)?(p-c):(c-p);

//	if((pa<=pb) && (pa<=pc))return(a);
//	if(pb<=pc)return(b);
//	return(c);

	p=(pa<=pb)?((pa<=pc)?a:c):((pb<=pc)?b:c);
	return(p);
}

uint BGBBTJ_PNG_ReadInt32BE(VFILE *fd)
{
	uint i;

	i=vfgetc(fd)<<24;
	i|=vfgetc(fd)<<16;
	i|=vfgetc(fd)<<8;
	i|=vfgetc(fd);
	return(i);
}

uint BGBBTJ_PNG_ReadFourcc(VFILE *fd)
{
	uint i;

	i=vfgetc(fd);
	i|=vfgetc(fd)<<8;
	i|=vfgetc(fd)<<16;
	i|=vfgetc(fd)<<24;
	return(i);
}

BGBBTJ_API byte *BGBBTJ_PNG_Load(VFILE *ifd, int *w, int *h)
{
	static byte plte[768], trns[256];
	uint fcc0, fcc1, len, crc, end;
	int xs, ys, bpp, clr, cm, fm, im, ssz;
	int i, j, k, pa, pb, pc;
	byte *buf, *bufe;
	byte *buf2, *buf2e;
	byte *s, *t;

	fcc0=BGBBTJ_PNG_ReadFourcc(ifd);
	fcc1=BGBBTJ_PNG_ReadFourcc(ifd);
	if((fcc0!=FCC_PNG_LOW) || (fcc1!=FCC_PNG_HIGH))
	{
		printf("damaged fourcc or non-png\n");
		return(NULL);
	}

	memset(plte, 0, 768);
	memset(trns, 255, 256);

	while(!vfeof(ifd))
	{
		len=BGBBTJ_PNG_ReadInt32BE(ifd);
		fcc0=BGBBTJ_PNG_ReadFourcc(ifd);

		if(fcc0==FCC_IEND)
		{
			crc=BGBBTJ_PNG_ReadInt32BE(ifd);
//			printf("%s %d %X\n", BGBBTJ_PNG_Fourcc2String(fcc0),
//				len, crc);

			break;
		}

		end=vftell(ifd)+len;
		if(fcc0==FCC_IHDR)
		{
			xs=BGBBTJ_PNG_ReadInt32BE(ifd);
			ys=BGBBTJ_PNG_ReadInt32BE(ifd);

			bpp=vfgetc(ifd);
			clr=vfgetc(ifd);
			cm=vfgetc(ifd);
			fm=vfgetc(ifd);
			im=vfgetc(ifd);

			if(bpp!=8)
			{
				printf("only 8 bits/channel supported\n");
				return(NULL);
			}
			if((clr!=3) && (clr!=0) && (clr!=4) &&
				(clr!=2) && (clr!=6))
			{
//				printf("only RGB(A) or Y(A) images supported\n");
				printf("unsupported color type %d\n", clr);
				return(NULL);
			}
			if(cm!=0)
			{
				printf("only deflated images supported\n");
				return(NULL);
			}
			if(fm!=0)
			{
				printf("unknown filter method\n");
				return(NULL);
			}
			if(im!=0)
			{
				printf("interlacing not supported\n");
				return(NULL);
			}

			buf=malloc(xs*ys*8);
			bufe=buf;

			ssz=0;
			if(clr==2)ssz=3;
			if(clr==6)ssz=4;

			if(clr==0)ssz=1;
			if(clr==4)ssz=2;
			if(clr==3)ssz=1;

//			printf("%dx%dx%dbpp\n", xs, ys, ssz*bpp);
//			printf("clr %d cm %d fm %d im %d\n",
//				clr, cm, fm, im);
		}

		if(fcc0==FCC_IDAT)
		{
			vfread(bufe, 1, len, ifd);
			bufe+=len;
		}

		if(fcc0==FCC_PLTE)
		{
			i=(len<=768)?len:768;
			vfread(plte, 1, i, ifd);
		}
		if(fcc0==FCC_tRNS)
		{
			i=(len<=256)?len:256;
			vfread(plte, 1, i, ifd);
		}

		vfseek(ifd, end, 0);
		crc=BGBBTJ_PNG_ReadInt32BE(ifd);

//		printf("%s %d %X\n", BGBBTJ_PNG_Fourcc2String(fcc0), len, crc);
	}

	buf2=malloc(xs*ys*8);

	i=bufe-(buf+2);
	i=PDUNZ_DecodeStream(buf+2, buf2, i, xs*ys*ssz+ys);
//	i=vfInflateBuffer(buf2, buf+2, xs*ys*ssz+ys, i);
//	i=puff(buf2, &k, buf+2, &j);

	if(i<0)	//this isn't good
	{
		printf("Inflater Error %d\n", i);

		free(buf);
		free(buf2);
		return(NULL);
	}
//	i=j;

	if(i>(xs*ys*4))printf("Inflated %d expected %d\n", i, xs*ys*4);

	j=(buf[0]<<8)|buf[1];
//	printf("Zlib %04X\n", j);

	j=(buf[i+2+0]<<24)|(buf[i+2+1]<<16)|(buf[i+2+2]<<8)|buf[i+2+3];
	k=BGBBTJ_PNG_DataAdler32(buf2, xs*ys*ssz+ys, 1);

	if(j!=k)printf("Adler32: %s  %08X %08X\n", (j==k)?"Pass":"Fail", j, k);

//	PDZ2_EncodeStream(buf2, buf, xs*ys*ssz+ys, xs*ys*8);
//	PDUNZ_DecodeStream(buf, buf2, i, xs*ys*ssz+ys);
	free(buf);

	buf=malloc((xs+1)*(ys+1)*4);
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
					pa=buf[(((ys-i)*xs)+j-1)*4+k];
					pb=buf[(((ys-i)*xs)+j)*4+k];
					pc=buf[(((ys-i-1)*xs)+j-1)*4+k];
				}else if(i)
				{
					pa=0;
					pb=buf[(((ys-i)*xs)+j)*4+k];
					pc=0;
				}else if(j)
				{
					pa=0;
					pb=0;
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
					buf[(((ys-i-1)*xs)+j)*4+k]=s[k];
					break;
				case 1:
					buf[(((ys-i-1)*xs)+j)*4+k]=s[k]+pc;
					break;
				case 2:
					buf[(((ys-i-1)*xs)+j)*4+k]=s[k]+pb;
					break;
				case 3:
					buf[(((ys-i-1)*xs)+j)*4+k]=
						s[k]+((pc+pb)>>1);
					break;
				case 4:
					buf[(((ys-i-1)*xs)+j)*4+k]=
						s[k]+BGBBTJ_PNG_Paeth(pc, pb, pa);
					break;
				default:
					break;
				}
			}
			if(ssz==3)buf[(((ys-i-1)*xs)+j)*4+3]=255;

			s+=ssz;
		}
	}
	free(buf2);

	//correct for grayscale images...
	if(clr==0)
	{
		for(i=0; i<(xs*ys); i++)
		{
			buf[i*4+1]=buf[i*4+0];
			buf[i*4+2]=buf[i*4+0];
			buf[i*4+3]=255;
		}
	}
	if(clr==4)
	{
		for(i=0; i<(xs*ys); i++)
		{
			buf[i*4+3]=buf[i*4+1];
			buf[i*4+1]=buf[i*4+0];
			buf[i*4+2]=buf[i*4+0];
		}
	}

	//correct for paletized images
	if(clr==3)
	{
		for(i=0; i<xs*ys; i++)
		{
			j=buf[i*4+0];

			buf[i*4+0]=plte[j*3+0];
			buf[i*4+1]=plte[j*3+1];
			buf[i*4+2]=plte[j*3+2];
			buf[i*4+3]=trns[j];
		}
	}


	*w=xs;
	*h=ys;
	return(buf);
}

BGBBTJ_API byte *BGBBTJ_PNG_LoadRaw(char *name, int *w, int *h)
{
	VFILE *fd;
	byte *tmp;

	fd=vffopen(name, "rb");
	if(!fd)
	{
		printf("PCX_LoadRaw: can't open '%s'\n", name);
		return(NULL);
	}

	tmp=BGBBTJ_PNG_Load(fd, w, h);
	vfclose(fd);

	return(tmp);
}

BGBBTJ_API void BGBBTJ_PNG_Free(byte *buf)
{
	free(buf);
}

void BGBBTJ_PNG_WriteInt32LE(VFILE *fd, uint v)
{
	vfputc(v&0xFF, fd);
	vfputc((v>>8)&0xFF, fd);
	vfputc((v>>16)&0xFF, fd);
	vfputc((v>>24)&0xFF, fd);
}

void BGBBTJ_PNG_WriteInt32BE(VFILE *fd, uint v)
{
	vfputc((v>>24)&0xFF, fd);
	vfputc((v>>16)&0xFF, fd);
	vfputc((v>>8)&0xFF, fd);
	vfputc(v&0xFF, fd);
}

void BGBBTJ_PNG_WriteFourcc(VFILE *fd, uint v)
{
	vfputc(v&0xFF, fd);
	vfputc((v>>8)&0xFF, fd);
	vfputc((v>>16)&0xFF, fd);
	vfputc((v>>24)&0xFF, fd);
}

void BGBBTJ_PNG_WriteChunk(VFILE *fd, uint fcc, byte *buf, int len)
{
	byte tb[4];
	uint crc;

	BGBBTJ_PNG_WriteInt32BE(fd, len);
	BGBBTJ_PNG_WriteFourcc(fd, fcc);
	vfwrite(buf, 1, len, fd);

	tb[0]=fcc&0xFF; tb[1]=(fcc>>8)&0xFF;
	tb[2]=(fcc>>16)&0xFF; tb[3]=(fcc>>24)&0xFF;
	crc=BGBBTJ_PNG_CRC32(tb, 4, (uint)-1);
	crc=BGBBTJ_PNG_CRC32(buf, len, crc);

//	printf("%s %d %X\n", BGBBTJ_PNG_Fourcc2String(fcc), len, ~crc);

	BGBBTJ_PNG_WriteInt32BE(fd, ~crc);
}

#ifndef ABS
#define ABS(v)	(((v)<0)?(-(v)):(v))
#endif

BGBBTJ_API void BGBBTJ_PNG_Store(VFILE *fd, byte *buf, int xs, int ys)
{
	byte hbuf[64];
	byte *buf2, *buf3, *s, *t;
	int fm, sz, pa, pb, pc, pd;
	int e, be, bf, am, ssz;
	int i, j, k, l;

	am=0;
	for(i=0; i<(xs*ys); i++)
		if(buf[i*4+3]<255)am=1;
	ssz=am?4:3;

	buf2=malloc(xs*ys*ssz+ys);

	t=buf2;
	for(i=0; i<ys; i++)
	{
		be=ssz*xs*255;
		bf=0;

		for(l=0; l<5; l++)
		{
//			if(l==4)continue;

			e=0;
			for(j=0; j<xs; j++)
			{
				for(k=0; k<ssz; k++)
				{
					pd=buf[(((ys-i-1)*xs)+j)*4+k];

					if(i && j)
//					if(((i+1)<xs) && j)
					{
						pa=buf[(((ys-i)*xs)+j-1)*4+k];
						pb=buf[(((ys-i)*xs)+j)*4+k];
						pc=buf[(((ys-i-1)*xs)+j-1)*4+k];
					}else if(i)
//					}else if((i+1)<xs)
					{
						pa=0;
						pb=buf[(((ys-i)*xs)+j)*4+k];
						pc=0;
					}else if(j)
					{
						pa=0;
						pb=0;
						pc=buf[(((ys-i-1)*xs)+j-1)*4+k];
					}else
					{
						pa=0;
						pb=0;
						pc=0;
					}

					switch(l)
					{
					case 0:
						e+=ABS(pd);
						break;
					case 1:
						e+=ABS(pd-pc);
						break;
					case 2:
						e+=ABS(pd-pb);
						break;
					case 3:
						e+=ABS(pd-((pc+pb)>>1));
						break;
					case 4:
						e+=ABS(pd-BGBBTJ_PNG_Paeth(pc, pb, pa));
						break;
					default:
						break;
					}
				}
			}

//			if(l==4)continue;

			if(e<be)
			{
				be=e;
				bf=l;
			}
		}

//		printf("%d ", bf);

		fm=bf;
		*t++=fm;

		for(j=0; j<xs; j++)
		{
			for(k=0; k<ssz; k++)
			{
				pd=buf[(((ys-i-1)*xs)+j)*4+k];

				if(i && j)
//				if(((i+1)<xs) && j)
				{
					pa=buf[(((ys-i)*xs)+j-1)*4+k];
					pb=buf[(((ys-i)*xs)+j)*4+k];
					pc=buf[(((ys-i-1)*xs)+j-1)*4+k];
				}else if(i)
//				}else if((i+1)<xs)
				{
					pa=0;
					pb=buf[(((ys-i)*xs)+j)*4+k];
					pc=0;
				}else if(j)
				{
					pa=0;
					pb=0;
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
					*t++=pd;
					break;
				case 1:
					*t++=pd-pc;
					break;
				case 2:
					*t++=pd-pb;
					break;
				case 3:
					*t++=pd-((pc+pb)>>1);
					break;
				case 4:
					*t++=pd-BGBBTJ_PNG_Paeth(pc, pb, pa);
					break;
				default:
					break;
				}
			}
		}
	}
//	printf("\n");

	buf=malloc(xs*ys*8);

	//zlib header
	i=(7<<12)|(8<<8)|(3<<6);
	j=i%31;
	if(j)i|=31-j;
	buf[0]=i>>8;
	buf[1]=i&0xFF;
//	printf("Zlib %04X\n", i);

	i=PDZ2_EncodeStreamLvl(buf2, buf+2, xs*ys*ssz+ys, xs*ys*8, 1);
//	i=vfDeflateBufferLvl(buf+2, buf2, xs*ys*8, xs*ys*ssz+ys, 9);

	if(i<0)
	{
		printf("PNG: Deflate Error %d\n", i);
	}

	//checksum
	j=BGBBTJ_PNG_DataAdler32(buf2, xs*ys*ssz+ys, 1);
	buf[2+i+0]=(j>>24)&0xFF;
	buf[2+i+1]=(j>>16)&0xFF;
	buf[2+i+2]=(j>>8)&0xFF;
	buf[2+i+3]=j&0xFF;

//	printf("Adler32: Wrote %08X\n", j);

#if 1
	k=xs*ys*ssz+ys;
	buf3=malloc(xs*ys*ssz+ys);
	PDUNZ_DecodeStreamSz(buf+2, buf3, i, k, NULL, 0);
	for(j=0; j<k; j++)
		{ if(buf2[j]!=buf3[j])break; }
	if(j<k)
	{
		printf("PNG: Check Fail: Offs=%d %d->%d\n", j, buf2[j], buf3[j]);
	}
	free(buf3);
#endif

	sz=i+6;
	free(buf2);

	BGBBTJ_PNG_WriteFourcc(fd, FCC_PNG_LOW);
	BGBBTJ_PNG_WriteFourcc(fd, FCC_PNG_HIGH);

	hbuf[0]=xs>>24; hbuf[1]=xs>>16; hbuf[2]=xs>>8; hbuf[3]=xs;
	hbuf[4]=ys>>24; hbuf[5]=ys>>16; hbuf[6]=ys>>8; hbuf[7]=ys;
	hbuf[8]=8;	//8 bits/sample
	hbuf[9]=am?6:2;	//RGB(A)
	hbuf[10]=0;	//0=deflate
	hbuf[11]=0;	//no filter
	hbuf[12]=0;	//no interlace

	BGBBTJ_PNG_WriteChunk(fd, FCC_IHDR, hbuf, 13);

//	BGBBTJ_PNG_WriteChunk(fd, FCC_IDAT, buf, sz);

	s=buf; i=sz;
	while(i>65536)
	{
		BGBBTJ_PNG_WriteChunk(fd, FCC_IDAT, s, 65536);
		s+=65536;
		i-=65536;
	}
	BGBBTJ_PNG_WriteChunk(fd, FCC_IDAT, s, i);

	BGBBTJ_PNG_WriteChunk(fd, FCC_IEND, NULL, 0);
}


BGBBTJ_API int BGBBTJ_PNG_SaveRaw(char *name, byte *buf, int xs, int ys)
{
	VFILE *fd;
	int i;

	fd=vffopen(name, "wb");
	if(!fd)return(-1);

	BGBBTJ_PNG_Store(fd, buf, xs, ys);
	vfclose(fd);

	return(0);
}

BGBBTJ_API int BGBBTJ_PNG_SaveRawFlip(char *name, byte *buf, int xs, int ys)
{
	VFILE *fd;
	byte *buf1;
	int i, j, k, w, h, w2;

	fd=vffopen(name, "wb");
	if(!fd)return(-1);

	buf1=malloc(xs*ys*4); w2=4*xs;
	for(i=0; i<ys; i++) for(j=0; j<w2; j++)
		buf1[i*w2+j]=buf[(ys-i-1)*w2+j];

	BGBBTJ_PNG_Store(fd, buf1, xs, ys);
	free(buf1); vfclose(fd);
	return(0);
}