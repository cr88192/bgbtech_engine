#include <bgbbtj.h>

typedef struct TargaHeader_s {
	unsigned char 	id_length, ctype, itype;
	unsigned short	cindex, clength;
	unsigned char	csize;
	unsigned short	x_origin, y_origin, width, height;
	unsigned char	bpp, attributes;
} TargaHeader;

byte *BGBBTJ_Img_LoadTGA(VFILE *fd, int *w, int *h)
{
	TargaHeader head;
	int pixlin, i, j;
	byte *dest, *src, *buf, *obuf, r, g, b, a;

	head.id_length=vfgetc(fd);
	head.ctype=vfgetc(fd);
	head.itype=vfgetc(fd);
	head.cindex=vfgetc(fd)+(vfgetc(fd)<<8);
	head.clength=vfgetc(fd)+(vfgetc(fd)<<8);
	head.csize=vfgetc(fd);
	head.x_origin=vfgetc(fd)+(vfgetc(fd)<<8);
	head.y_origin=vfgetc(fd)+(vfgetc(fd)<<8);
	head.width=vfgetc(fd)+(vfgetc(fd)<<8);
	head.height=vfgetc(fd)+(vfgetc(fd)<<8);
	head.bpp=vfgetc(fd);
	head.attributes=vfgetc(fd);

	if(((head.itype!=2) && (head.itype!=10)) ||
			((head.bpp!=32) && (head.bpp!=24)) || (head.ctype!=0))
	{
		printf("Img_LoadTGA: only RGB(24)/RGBA(32) uncompressed "
			"images supported.\n");
		printf("   bpp: %d, type: %d\n", head.bpp, head.ctype);
		return(NULL);
	}

	vfseek(fd, head.id_length, 1); /* skip comment */

	pixlin=head.width*head.height;
	obuf=malloc(pixlin*4);
	dest=obuf;

	buf=malloc(2*pixlin*(head.bpp>>3));
	vfread(buf, 1, 2*pixlin*(head.bpp>>3), fd);
	src=buf;

	if(head.itype==2)
	{
		for(i=0; i<pixlin; i++)
		{
			b=*src++;
			g=*src++;
			r=*src++;
			if(head.bpp==32)
				a=*src++;
				else a=255;
			*dest++=r;
			*dest++=g;
			*dest++=b;
			*dest++=a;
		}
	}

	if(head.itype==10)
	{
		i=0;
		while(i<pixlin)
		{
			j=*src++;
			if(j<128)
			{
				j++;
				while(j)
				{
					b=*src++;
					g=*src++;
					r=*src++;
					if(head.bpp==32)
						a=*src++;
						else a=255;
					*dest++=r;
					*dest++=g;
					*dest++=b;
					*dest++=a;

					j--;
					i++;
				}
			}else
			{
				b=*src++;
				g=*src++;
				r=*src++;
				if(head.bpp==32)
					a=*src++;
					else a=255;

				j-=127;
				while(j)
				{
					*dest++=r;
					*dest++=g;
					*dest++=b;
					*dest++=a;

					j--;
					i++;
				}
			}
		}
	}

	free(buf);

	if(head.attributes&32)
	{
		dest=malloc(pixlin*4);
		for(i=0; i<head.height; i++)
			memcpy(&dest[i*head.width*4], &obuf[(head.height-(i+1))*head.width*4], head.width*4);
		memcpy(obuf, dest, pixlin*4);
		free(dest);
	}

	*w=head.width;
	*h=head.height;
	return(obuf);
}

int BGBBTJ_Img_StoreTGA(VFILE *fd, byte *buf, int w, int h)
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

	vfputc(head.id_length, fd);
	vfputc(head.ctype, fd);
	vfputc(head.itype, fd);

	vfputc(head.cindex, fd);
	vfputc(head.cindex>>8, fd);
	vfputc(head.clength, fd);
	vfputc(head.clength>>8, fd);

	vfputc(head.csize, fd);

	vfputc(head.x_origin, fd);
	vfputc(head.x_origin>>8, fd);
	vfputc(head.y_origin, fd);
	vfputc(head.y_origin>>8, fd);

	vfputc(head.width, fd);
	vfputc(head.width>>8, fd);
	vfputc(head.height, fd);
	vfputc(head.height>>8, fd);

	vfputc(head.bpp, fd);
	vfputc(head.attributes, fd);

//	vfwrite(buf, w*4, h, fd);

	s=buf;
	for(i=0; i<w*h; i++)
	{
		vfputc(s[2], fd);
		vfputc(s[1], fd);
		vfputc(s[0], fd);
		s+=4;
	}

	return(0);
}


BGBBTJ_API byte *BGBBTJ_TGA_LoadRaw(char *name, int *w, int *h)
{
	VFILE *fd;
	byte *tmp;

	fd=vffopen(name, "rb");
	if(!fd)
	{
		printf("PCX_LoadRaw: can't open '%s'\n", name);
		return(NULL);
	}

	tmp=BGBBTJ_Img_LoadTGA(fd, w, h);
	vfclose(fd);

	return(tmp);
}

BGBBTJ_API int BGBBTJ_TGA_SaveRaw(char *name, byte *buf, int w, int h)
{
	VFILE *fd;
	
	fd=vffopen(name, "wb");
	if(!fd)
	{
		return(-1);
	}
	return(BGBBTJ_Img_StoreTGA(fd, buf, w, h));
}
