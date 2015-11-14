/*
load/store pcx files
 */

#include <bgbbtj.h>

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

byte *BGBBTJ_PCX_Load(VFILE *fd, int *w, int *h, char *pal)
{
	struct PcxHead_s head;
	unsigned char *buf, *s;
	int i, j, sz;
	unsigned char t;
	int width, height;

	head.tag=vfgetc(fd);

	if(head.tag!=0x0A)
	{
		printf("PCX_LoadPCX: invalid tag\n");
		return(NULL);
	}

	head.version=vfgetc(fd);
	head.encoding=vfgetc(fd);
	if((head.encoding!=0) && (head.encoding!=1))
	{
		printf("PCX_LoadPCX: bad encoding\n");
		return(NULL);
	}

	head.bpp=vfgetc(fd);

	if(head.bpp!=8)
	{
		printf("PCX_LoadPCX: bpp is not 8 bits\n");
		return(NULL);
	}

	head.minx=vfgetc(fd);
	head.minx+=vfgetc(fd)<<8;
	head.miny=vfgetc(fd);
	head.miny+=vfgetc(fd)<<8;
	head.maxx=vfgetc(fd);
	head.maxx+=vfgetc(fd)<<8;
	head.maxy=vfgetc(fd);
	head.maxy+=vfgetc(fd)<<8;

	head.hres=vfgetc(fd);
	head.hres+=vfgetc(fd)<<8;
	head.vres=vfgetc(fd);
	head.vres+=vfgetc(fd)<<8;

	vfread(head.map, 1, 48, fd);

	head.resv=vfgetc(fd);
	head.planes=vfgetc(fd);

	head.linesz=vfgetc(fd);
	head.linesz+=vfgetc(fd)<<8;

	head.paltype=vfgetc(fd);
	head.paltype+=vfgetc(fd)<<8;

	vfread(head.resv2, 1, 58, fd);


	width=(head.maxx-head.minx)+1;
	height=(head.maxy-head.miny)+1;

	if(w)*w=width;
	if(h)*h=height;

	sz=width*height;
	buf=malloc(sz+256);

	if(!head.encoding)
	{
		vfread(buf, 1, sz, fd);
		return(buf);
	}

	s=buf;
	while((s-buf)<sz)
	{
		t=vfgetc(fd);
		if(t>=0xC0)
		{
			j=t&0x3F;
			t=vfgetc(fd);
			while(j--)*s++=t;
		}else *s++=t;
	}

	if(pal)
	{
		vfseek(fd, -769, 2);
		t=vfgetc(fd);
		if(t==12)
			vfread(pal, 1, 768, fd);
	}
	return(buf);
}

byte *BGBBTJ_PCX_Load32(VFILE *fd, int *w, int *h)
{
	unsigned char *buf, *buf2;
	unsigned char pal[768];
	int cw, ch;
	int i;

	buf=BGBBTJ_PCX_Load(fd, &cw, &ch, pal);
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

unsigned char *BGBBTJ_PCX_LoadRaw(char *name, int *w, int *h, char *pal)
{
	VFILE *fd;
	unsigned char *tmp;

	fd=vffopen(name, "rb");
	if(!fd)
	{
		printf("PCX_LoadRaw: can't open '%s'\n", name);
		return(NULL);
	}

	tmp=BGBBTJ_PCX_Load(fd, w, h, pal);
	vfclose(fd);

	return(tmp);
}

int BGBBTJ_PCX_Store(VFILE *fd, unsigned char *img,
	int width, int height, char *pal)
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

	vfputc(head.tag, fd);
	vfputc(head.version, fd);
	vfputc(head.encoding, fd);
	vfputc(head.bpp, fd);

	vfputc(head.minx&0xff, fd);
	vfputc((head.minx>>8)&0xff, fd);
	vfputc(head.miny&0xff, fd);
	vfputc((head.miny>>8)&0xff, fd);

	vfputc(head.maxx&0xff, fd);
	vfputc((head.maxx>>8)&0xff, fd);
	vfputc(head.maxy&0xff, fd);
	vfputc((head.maxy>>8)&0xff, fd);

	vfputc(head.hres&0xff, fd);
	vfputc((head.hres>>8)&0xff, fd);
	vfputc(head.vres&0xff, fd);
	vfputc((head.vres>>8)&0xff, fd);

	vfwrite(head.map, 1, 48, fd);

	vfputc(head.resv, fd);
	vfputc(head.planes, fd);

	vfputc(head.linesz&0xff, fd);
	vfputc((head.linesz>>8)&0xff, fd);

	vfputc(head.paltype&0xff, fd);
	vfputc((head.paltype>>8)&0xff, fd);

	vfwrite(head.resv2, 1, 58, fd);

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
				vfputc(t, fd);
			}else if(l>0)
			{
				vfputc(0xC0+l, fd);
				vfputc(t, fd);
			}
			l=1;
			t=*s;
			s++;
			continue;
		}
		if(l>=15)
		{
			vfputc(0xC0+l, fd);
			vfputc(t, fd);

			l=1;
			s++;
			continue;
		}
		s++;
		l++;
	}

	if((l==1) && (t<0xC0))
	{
		vfputc(t, fd);
	}else if(l>0)
	{
		vfputc(0xC0+l, fd);
		vfputc(t, fd);
	}

	if(pal)
	{
		vfputc(12, fd);
		vfwrite(pal, 1, 768, fd);
	}else
	{
		vfputc(12, fd);
		for(i=0; i<256; i++)
		{
			vfputc(i, fd);
			vfputc(i, fd);
			vfputc(i, fd);
		}
	}

	return(0);
}

int BGBBTJ_PCX_StoreRaw(char *name, unsigned char *buf,
	int w, int h, char *pal)
{
	VFILE *fd;

	fd=vffopen(name, "wb");
	if(!fd)
	{
		printf("PCX_StoreRaw: can't open '%s'\n", name);
		return(-1);
	}

	BGBBTJ_PCX_Store(fd, buf, w, h, pal);
	vfclose(fd);

	return(0);
}
