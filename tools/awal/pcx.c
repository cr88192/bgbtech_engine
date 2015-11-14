/*
load/store pcx files
 */

#include <walpng.h>

struct PcxHead_s {
unsigned char tag;	//0x0A
unsigned char version;	//typically 5
unsigned char encoding; //1=RLE
unsigned char bpp;

unsigned short minx;
unsigned short miny;
unsigned short maxx;
unsigned short maxy;

unsigned short hres;
unsigned short vres;

unsigned char map[48];	//16 color palette

unsigned char resv;	//0
unsigned char planes;	//1
unsigned short linesz;	//bytes per line
unsigned short paltype;	//0=color/bw, 1=greyscale

unsigned char resv2[58];
};

byte *PCX_Load(FILE *fd, int *w, int *h, char *pal)
{
	struct PcxHead_s head;
	unsigned char *buf, *s;
	int i, j, sz;
	unsigned char t;
	int width, height;

	head.tag=fgetc(fd);

	if(head.tag!=0x0A)
	{
		printf("PCX_LoadPCX: invalid tag\n");
		return(NULL);
	}

	head.version=fgetc(fd);
	head.encoding=fgetc(fd);
	if((head.encoding!=0) && (head.encoding!=1))
	{
		printf("PCX_LoadPCX: bad encoding\n");
		return(NULL);
	}

	head.bpp=fgetc(fd);

	if(head.bpp!=8)
	{
		printf("PCX_LoadPCX: bpp is not 8 bits\n");
		return(NULL);
	}

	head.minx=fgetc(fd);
	head.minx+=fgetc(fd)<<8;
	head.miny=fgetc(fd);
	head.miny+=fgetc(fd)<<8;
	head.maxx=fgetc(fd);
	head.maxx+=fgetc(fd)<<8;
	head.maxy=fgetc(fd);
	head.maxy+=fgetc(fd)<<8;

	head.hres=fgetc(fd);
	head.hres+=fgetc(fd)<<8;
	head.vres=fgetc(fd);
	head.vres+=fgetc(fd)<<8;

	fread(head.map, 1, 48, fd);

	head.resv=fgetc(fd);
	head.planes=fgetc(fd);

	head.linesz=fgetc(fd);
	head.linesz+=fgetc(fd)<<8;

	head.paltype=fgetc(fd);
	head.paltype+=fgetc(fd)<<8;

	fread(head.resv2, 1, 58, fd);


	width=(head.maxx-head.minx)+1;
	height=(head.maxy-head.miny)+1;

	if(w)*w=width;
	if(h)*h=height;

	sz=width*height;
	buf=malloc(sz+256);

	if(!head.encoding)
	{
		fread(buf, 1, sz, fd);
		return(buf);
	}

	s=buf;
	while((s-buf)<sz)
	{
		t=fgetc(fd);
		if(t>=0xC0)
		{
			j=t&0x3F;
			t=fgetc(fd);
			while(j--)*s++=t;
		}else *s++=t;
	}

	if(pal)
	{
		fseek(fd, -769, 2);
		t=fgetc(fd);
		if(t==12)
			fread(pal, 1, 768, fd);
	}
	return(buf);
}

byte *PCX_Load32(FILE *fd, int *w, int *h)
{
	unsigned char *buf, *buf2;
	unsigned char pal[768];
	int cw, ch;
	int i;

	buf=PCX_Load(fd, &cw, &ch, pal);
	if(!buf)return(NULL);

	if(w)*w=cw;
	if(h)*h=ch;

	buf2=malloc(cw*ch*4);
	for(i=0; i<(cw*ch); i++)
	{
		buf2[(i*4)+0]=pal[(buf[i]*3)+0];
		buf2[(i*4)+1]=pal[(buf[i]*3)+1];
		buf2[(i*4)+2]=pal[(buf[i]*3)+2];
		buf2[(i*4)+3]=255;
	}
	free(buf);

	return(buf2);
}

unsigned char *PCX_LoadRaw(char *name, int *w, int *h, char *pal)
{
	FILE *fd;
	unsigned char *tmp;

	fd=fopen(name, "rb");
	if(!fd)
	{
		printf("PCX_LoadRaw: can't open '%s'\n", name);
		return(NULL);
	}

	tmp=PCX_Load(fd, w, h, pal);
	fclose(fd);

	return(tmp);
}

int PCX_Store(FILE *fd, unsigned char *img, int width, int height, char *pal)
{
	struct PcxHead_s head;
	unsigned char *buf, *s;
	int i, j, l, sz;
	unsigned char t;

	memset(&head, 0, sizeof(head));

	head.tag=0x0A;
	head.version=5;
	head.encoding=1;
	head.bpp=8;

	head.minx=0;
	head.miny=0;
	head.maxx=width-1;
	head.maxy=height-1;

	head.hres=0x500;
	head.vres=0x400;

	head.planes=1;
	head.linesz=width;

	fputc(head.tag, fd);
	fputc(head.version, fd);
	fputc(head.encoding, fd);
	fputc(head.bpp, fd);

	fputc(head.minx&0xff, fd);
	fputc((head.minx>>8)&0xff, fd);
	fputc(head.miny&0xff, fd);
	fputc((head.miny>>8)&0xff, fd);

	fputc(head.maxx&0xff, fd);
	fputc((head.maxx>>8)&0xff, fd);
	fputc(head.maxy&0xff, fd);
	fputc((head.maxy>>8)&0xff, fd);

	fputc(head.hres&0xff, fd);
	fputc((head.hres>>8)&0xff, fd);
	fputc(head.vres&0xff, fd);
	fputc((head.vres>>8)&0xff, fd);

	fwrite(head.map, 1, 48, fd);

	fputc(head.resv, fd);
	fputc(head.planes, fd);

	fputc(head.linesz&0xff, fd);
	fputc((head.linesz>>8)&0xff, fd);

	fputc(head.paltype&0xff, fd);
	fputc((head.paltype>>8)&0xff, fd);

	fwrite(head.resv2, 1, 58, fd);

	sz=width*height;

	buf=img;
	s=buf;
	t=0;
	l=0;

	while((s-buf)<sz)
	{
		if(*s!=t)
		{
			if((l==1) && (t<0xC0))
			{
				fputc(t, fd);
			}else if(l>0)
			{
				fputc(0xC0+l, fd);
				fputc(t, fd);
			}
			l=1;
			t=*s;
			s++;
			continue;
		}
		if(l>=15)
		{
			fputc(0xC0+l, fd);
			fputc(t, fd);

			l=1;
			s++;
			continue;
		}
		s++;
		l++;
	}

	if((l==1) && (t<0xC0))
	{
		fputc(t, fd);
	}else if(l>0)
	{
		fputc(0xC0+l, fd);
		fputc(t, fd);
	}

	if(pal)
	{
		fputc(12, fd);
		fwrite(pal, 1, 768, fd);
	}else
	{
		fputc(12, fd);
		for(i=0; i<256; i++)
		{
			fputc(i, fd);
			fputc(i, fd);
			fputc(i, fd);
		}
	}

	return(0);
}

int PCX_StoreRaw(char *name, unsigned char *buf, int w, int h, char *pal)
{
	FILE *fd;

	fd=fopen(name, "wb");
	if(!fd)
	{
		printf("PCX_StoreRaw: can't open '%s'\n", name);
		return(-1);
	}

	PCX_Store(fd, buf, w, h, pal);
	fclose(fd);

	return(0);
}
