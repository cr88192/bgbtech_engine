#include <btgecm.h>

static byte btgecm_misc_colors[64][3]=
{
	//Basic 16 colors
	{0x00,0x00,0x00},	//0, Black
	{0x80,0x00,0x00},	//1, Dark Red
	{0x00,0x80,0x00},	//2, Dark Green
	{0x80,0x80,0x00},	//3, Dark Yellow
	{0x00,0x00,0x80},	//4, Dark Blue
	{0x80,0x00,0x80},	//5, Dark Purple
	{0x00,0x80,0x80},	//6, Dark Cyan
	{0x80,0x80,0x80},	//7, Light Grey
	{0x40,0x40,0x40},	//8, Dark Grey
	{0xFF,0x00,0x00},	//9, Bright Red
	{0x00,0xFF,0x00},	//10, Bright Green
	{0xFF,0xFF,0x00},	//11, Bright Yellow
	{0x00,0x00,0xFF},	//12, Bright Blue
	{0xFF,0x00,0xFF},	//13, Bright Purple
	{0x00,0xFF,0xFF},	//14, Bright Cyan
	{0xFF,0xFF,0xFF},	//15, Bright White

	//Reduced Staturation Base Colors
	{0x20,0x20,0x20},	//16
	{0xFF,0x80,0x80},	//17
	{0x80,0xFF,0x80},	//18
	{0xFF,0xFF,0x80},	//19
	{0x80,0x80,0xFF},	//20
	{0xFF,0x80,0xFF},	//21
	{0x80,0xFF,0xFF},	//22
	{0xDF,0xDF,0xDF},	//23
	{0xA0,0xA0,0xA0},	//24
	{0x80,0x40,0x40},	//25
	{0x40,0x80,0x40},	//26
	{0x80,0x80,0x40},	//27
	{0x40,0x40,0x80},	//28
	{0x80,0x40,0x80},	//29
	{0x40,0x80,0x80},	//30
	{0xC0,0xC0,0xC0},	//31

	//PDN colors
	{0x00,0x00,0x00},	//32, Top L0
	{0xFF,0xFF,0xFF},	//33, Bot L0
	{0x40,0x40,0x40},	//34, Top L1
	{0x80,0x80,0x80},	//35, Bot L1
	{0xFF,0x00,0x00},	//36
	{0x7F,0x00,0x00},	//37
	{0xFF,0x6A,0x00},	//38
	{0x7F,0x33,0x00},	//39
	{0xFF,0xD8,0x00},	//40
	{0x7F,0x6A,0x00},	//41
	{0xB6,0xFF,0x00},	//42
	{0x5B,0x7F,0x00},	//43
	{0x4C,0xFF,0x00},	//44
	{0x26,0x7F,0x00},	//45
	{0x00,0xFF,0x21},	//46
	{0x00,0x7F,0x0E},	//47
	{0x00,0xFF,0x90},	//48
	{0x00,0x7F,0x46},	//49
	{0x00,0xFF,0xFF},	//50
	{0x00,0x7F,0x7F},	//51
	{0x00,0x94,0xFF},	//52
	{0x00,0x4A,0x7F},	//53
	{0x00,0x26,0xFF},	//54
	{0x00,0x13,0x7F},	//55
	{0x48,0x00,0xFF},	//56
	{0x21,0x00,0x7F},	//57
	{0xB2,0x00,0xFF},	//58
	{0x57,0x00,0x7F},	//59
	{0xFF,0x00,0xDC},	//60
	{0x7F,0x00,0x6E},	//61
	{0xFF,0x00,0x6E},	//62
	{0x7F,0x00,0x37},	//63

//	{0x7F,0x00,0x00},
//	{0x00,0x7F,0x00},
//	{0x7F,0x7F,0x00},
//	{0x00,0x00,0x7F},
};

BTGE_API int BTGECM_Misc_GetColorIndexPDN(byte *clr)
{
	return(BTGECM_ColorKey_GetColorPaletteIndex(
		clr, (byte *)btgecm_misc_colors, 64));

#if 0
	int i;
	
	for(i=0; i<64; i++)
	{
		if((clr[0]==colors[i][0]) &&
			(clr[1]==colors[i][1]) &&
			(clr[2]==colors[i][2]))
				return(i);
	}
	
	return(-1);
#endif
}

BTGE_API int BTGECM_ColorKey_GetColorPaletteIndex(
	byte *clr, byte *colors, int ncolors)
{
	int i;
	
	for(i=0; i<ncolors; i++)
	{
		if((clr[0]==colors[i*3+0]) &&
			(clr[1]==colors[i*3+1]) &&
			(clr[2]==colors[i*3+2]))
				return(i);
	}
	
	return(-1);
}

BTGE_API int BTGECM_ColorKey_LoadPaletteDefault(
	byte *clrbuf, int maxcolors)
{
	int i;
	
	for(i=0; i<64; i++)
	{
		clrbuf[i*3+0]=btgecm_misc_colors[i][0];
		clrbuf[i*3+1]=btgecm_misc_colors[i][1];
		clrbuf[i*3+2]=btgecm_misc_colors[i][2];
	}
	return(64);
}

BTGE_API int BTGECM_ColorKey_LoadPalette(
	char *name, byte *clrbuf, int maxcolors)
{
	char buf[256];
	VFILE *fd;
	char **a;
	char *s;
	int i, n;

	fd=vffopen(name, "rt");
	if(!fd)
	{
		return(-1);
	}

	memset(clrbuf, 0, 3*maxcolors);
	
	n=0;
	while(!vfeof(fd))
	{
		vfgets(buf, 255, fd);
		a=gcrsplit(buf);

		if(!a[0])continue;

		if(*(a[0])=='/')continue;	//comment
		if(*(a[0])==';')continue;	//comment

		if(!strcmp(a[0], "color"))
		{
			i=gcratoi(a[1]);
			if((i<0) || (i>=maxcolors))
				continue;
			clrbuf[i*3+0]=gcratoi(a[2]);
			clrbuf[i*3+1]=gcratoi(a[3]);
			clrbuf[i*3+2]=gcratoi(a[4]);
			if(i>=n)n=i+1;
		}
	}
	
	vfclose(fd);
	
	return(n);
}

BTGE_API int BTGECM_ColorKey_LoadNames(
	char *name, char *key, char **namebuf, int maxnames)
{
	char buf[256];
	VFILE *fd;
	char **a;
	char *s;
	int i, n;

	fd=vffopen(name, "rt");
	if(!fd)
	{
		return(-1);
	}

	for(i=0; i<maxnames; i++)
		namebuf[i]=NULL;

	n=0;
	while(!vfeof(fd))
	{
		vfgets(buf, 255, fd);
		a=gcrsplit(buf);

		if(!a[0])continue;

		if(*(a[0])=='/')continue;	//comment
		if(*(a[0])==';')continue;	//comment

		if(!strcmp(a[0], key))
		{
			i=gcratoi(a[1]);
			if((i<0) || (i>=maxnames))
				continue;
			namebuf[i]=dystrdup(a[2]);
			if(i>=n)n=i+1;
		}
	}

	vfclose(fd);

	return(n);
}
