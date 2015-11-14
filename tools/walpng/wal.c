/*
load/store wal files
 */

#include <walpng.h>

uint WAL_ReadInt32LE(FILE *fd)
{
	uint i;

	i=fgetc(fd);
	i|=fgetc(fd)<<8;
	i|=fgetc(fd)<<16;
	i|=fgetc(fd)<<24;
	return(i);
}

uint WAL_ReadFourcc(FILE *fd)
{
	uint i;

	i=fgetc(fd);
	i|=fgetc(fd)<<8;
	i|=fgetc(fd)<<16;
	i|=fgetc(fd)<<24;
	return(i);
}

void WAL_Set32LE(byte *t, int val)
{
	t[0]=val;
	t[1]=val>>8;
	t[2]=val>>16;
	t[3]=val>>24;
}

int WAL_Write(FILE *fd, WalInfo_t *inf)
{
	byte tb[256];
	int i;

	memset(tb, 0, 256);
	if(inf->name)
		strcpy(tb+ 0, inf->name);
	if(inf->animname)
		strcpy(tb+56, inf->animname);

	WAL_Set32LE(tb+32, inf->width);
	WAL_Set32LE(tb+36, inf->height);
	WAL_Set32LE(tb+88, inf->flags);
	WAL_Set32LE(tb+92, inf->contents);
	WAL_Set32LE(tb+96, inf->value);

	i=100;
	WAL_Set32LE(tb+40, i);
	i+=inf->width*inf->height;

	WAL_Set32LE(tb+44, i);
	i+=(inf->width/2)*(inf->height/2);

	WAL_Set32LE(tb+48, i);
	i+=(inf->width/4)*(inf->height/4);

	WAL_Set32LE(tb+52, i);
	i+=(inf->width/8)*(inf->height/8);

	fwrite(tb, 1, 100, fd);

	i=inf->width*inf->height;
	fwrite(inf->data[0], 1, i, fd);

	i=(inf->width/2)*(inf->height/2);
	fwrite(inf->data[1], 1, i, fd);

	i=(inf->width/4)*(inf->height/4);
	fwrite(inf->data[2], 1, i, fd);

	i=(inf->width/8)*(inf->height/8);
	fwrite(inf->data[3], 1, i, fd);

	return(0);
}

char *WAL_Q1Name(char *name)
{
	char tb[64];
	char *s, *t;

	s=name+strlen(name);
	while((s>name) && (*s!='/'))s--;
	if(*s=='/')s++;

	t=tb;
	while(*s)
	{
		*t=*s++;
		if(*t=='#')*t='*';
		if((*t>='a') && (*t<='z'))
			*t=(*t-'a')+'A';
		t++;
	}
	*t++=0;

	return(strdup(tb));
}

int WAL_WriteQ1(FILE *fd, WalInfo_t *inf)
{
	byte tb[256];
	int i;

	memset(tb, 0, 256);
	if(inf->name)
		strcpy(tb+ 0, WAL_Q1Name(inf->name));

	WAL_Set32LE(tb+16, inf->width);
	WAL_Set32LE(tb+20, inf->height);

	i=40;
	WAL_Set32LE(tb+24, i);
	i+=inf->width*inf->height;

	WAL_Set32LE(tb+28, i);
	i+=(inf->width/2)*(inf->height/2);

	WAL_Set32LE(tb+32, i);
	i+=(inf->width/4)*(inf->height/4);

	WAL_Set32LE(tb+36, i);
	i+=(inf->width/8)*(inf->height/8);

	fwrite(tb, 1, 40, fd);

	i=inf->width*inf->height;
	fwrite(inf->data[0], 1, i, fd);

	i=(inf->width/2)*(inf->height/2);
	fwrite(inf->data[1], 1, i, fd);

	i=(inf->width/4)*(inf->height/4);
	fwrite(inf->data[2], 1, i, fd);

	i=(inf->width/8)*(inf->height/8);
	fwrite(inf->data[3], 1, i, fd);

	return(0);
}

int WAL_WriteFile(char *name, WalInfo_t *inf)
{
	FILE *fd;

	fd=fopen(name, "wb");
	if(!fd)return(-1);
	WAL_Write(fd, inf);
	fclose(fd);
	return(-1);
}

int WAL_WriteFileQ1(char *name, WalInfo_t *inf)
{
	FILE *fd;

	fd=fopen(name, "wb");
	if(!fd)return(-1);
	WAL_WriteQ1(fd, inf);
	fclose(fd);
	return(-1);
}

void WAL_Conv32to8(byte *ibuf, byte *obuf, int width, int height,
	char *pal)
{
	byte *cs, *ct;
	int i, j, k, n;
	int dr, dg, db;
	int bi, bd;

	cs=ibuf; ct=obuf;
	n=width*height;
	for(i=0; i<n; i++)
	{
		bi=0; bd=(1<<30);
		for(j=0; j<256; j++)
		{
			dr=((int)pal[j*3+0])-cs[0];
			dg=((int)pal[j*3+1])-cs[1];
			db=((int)pal[j*3+2])-cs[2];
			k=dr*dr+dg*dg+db*db;
			if(k<bd) { bi=j; bd=k; }
		}

		cs+=4;
		*ct++=bi;
	}
}

int WAL_HalfSample(byte *src, int w, int h)
{
	int i, j, w2, h2, i2, j2;

	w2=w>>1;
	h2=h>>1;
	for(i=0;i<h2; i++)
		for(j=0; j<w2; j++)
	{
		i2=i<<1;
		j2=j<<1;
		src[((i*w2+j)<<2)+0]=
			(src[((i2*w+j2)<<2)+0]+		src[((i2*w+(j2+1))<<2)+0]+
			src[(((i2+1)*w+j2)<<2)+0]+	src[(((i2+1)*w+(j2+1))<<2)+0])>>2;

		src[((i*w2+j)<<2)+1]=
			(src[((i2*w+j2)<<2)+1]+		src[((i2*w+(j2+1))<<2)+1]+
			src[(((i2+1)*w+j2)<<2)+1]+	src[(((i2+1)*w+(j2+1))<<2)+1])>>2;

		src[((i*w2+j)<<2)+2]=
			(src[((i2*w+j2)<<2)+2]+		src[((i2*w+(j2+1))<<2)+2]+
			src[(((i2+1)*w+j2)<<2)+2]+	src[(((i2+1)*w+(j2+1))<<2)+2])>>2;

		src[((i*w2+j)<<2)+3]=
			(src[((i2*w+j2)<<2)+3]+		src[((i2*w+(j2+1))<<2)+3]+
			src[(((i2+1)*w+j2)<<2)+3]+	src[(((i2+1)*w+(j2+1))<<2)+3])>>2;
	}
}

WalInfo_t *WAL_CreateWal(byte *img, int width, int height, char *pal)
{
	WalInfo_t *tmp;
	byte *buf0, *buf1;
	int i, j, i1;

	tmp=malloc(sizeof(WalInfo_t));
	memset(tmp, 0, sizeof(WalInfo_t));

	tmp->width=width;
	tmp->height=height;

	buf0=malloc(width*height*4);
	buf1=malloc(width*height);

//	memcpy(buf0, img, width*height*4);
	for(i=0; i<height; i++)
	{
		i1=height-i-1;
		for(j=0; j<width; j++)
		{
			buf0[(i*width+j)*4+0]=img[(i1*width+j)*4+0];
			buf0[(i*width+j)*4+1]=img[(i1*width+j)*4+1];
			buf0[(i*width+j)*4+2]=img[(i1*width+j)*4+2];
			buf0[(i*width+j)*4+3]=img[(i1*width+j)*4+3];
		}
	}

	WAL_Conv32to8(buf0, buf1, width, height, pal);
	i=width*height; tmp->data[0]=malloc(i);
	memcpy(tmp->data[0], buf1, i);

	WAL_HalfSample(buf0, width, height);
	WAL_Conv32to8(buf0, buf1, width/2, height/2, pal);
	i=(width/2)*(height/2); tmp->data[1]=malloc(i);
	memcpy(tmp->data[1], buf1, i);

	WAL_HalfSample(buf0, width/2, height/2);
	WAL_Conv32to8(buf0, buf1, width/4, height/4, pal);
	i=(width/4)*(height/4); tmp->data[2]=malloc(i);
	memcpy(tmp->data[2], buf1, i);

	WAL_HalfSample(buf0, width/4, height/4);
	WAL_Conv32to8(buf0, buf1, width/8, height/8, pal);
	i=(width/8)*(height/8); tmp->data[3]=malloc(i);
	memcpy(tmp->data[3], buf1, i);

	free(buf0); free(buf1);

	tmp->pal=malloc(768);
	memcpy(tmp->pal, pal, 768);

	return(tmp);
}
