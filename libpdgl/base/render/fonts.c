// #include <GL/gl.h>

#include <pdgl.h>

#if 0
typedef struct fontfrag_s {
struct fontfrag_s *next, *prev; //in parent font
struct fontinfo_s *parent;
int base;	//base character

// byte usemap[8];		//cells which have been drawn
// byte badmap[8];		//cells which are not valid

u16 usemap[16];		//cells which have been drawn
u16 badmap[16];		//cells which are not valid
byte *buffer;
int texnum;
}fontfrag;

typedef struct fontinfo_s {
struct fontinfo_s *next;	//next in the list of fonts
struct fontinfo_s *chain;	//links for font fragments
char *name;

byte *buf;
int w, h;
int s, e;
fontfrag *frag;

struct fontinfo_s *alts[16];	//alternate fonts to check
int n_alts;
}fontinfo;
#endif

fontinfo *font_root, *font_cur;
fontfrag *font_freefrags;

int font_mode, font_time, font_size;
char *font_name;

//extern unsigned char font_8x8_32_96[96*8];
//extern unsigned char koi8r_8x16[];
//extern unsigned char cp866_8x16[];

byte mouse_16x16[32]={
0x00, 0x07,		/* 0000000000000111 */
0x00, 0x0F,		/* 0000000000001111 */
0x00, 0x1F,		/* 0000000000011111 */
0x00, 0x3E,		/* 0000000000111110 */
0x80, 0x7C,		/* 1000000001111100 */
0xC0, 0xF8,		/* 1100000011111000 */
0xE1, 0xF0,		/* 1110000111110000 */
0xF3, 0xE0,		/* 1111001111100000 */
0xFF, 0xC0,		/* 1111111111000000 */
0xFF, 0x80,		/* 1111111110000000 */
0xFF, 0x00,		/* 1111111100000000 */
0xFF, 0x00,		/* 1111111100000000 */
0xFF, 0x80,		/* 1111111110000000 */
0xFF, 0xC0,		/* 1111111111000000 */
0xFF, 0xE0,		/* 1111111111100000 */
0xFF, 0xF0,		/* 1111111111110000 */

};

PDGL_API int GfxFont_Register(char *name, char *buf, int w, int h, int s, int e)
{
	fontinfo *tmp;

	tmp=gcalloc(sizeof(fontinfo));
	tmp->name=dystrdup(name);
	tmp->buf=buf;
	tmp->w=w;
	tmp->h=h;
	tmp->s=s;
	tmp->e=e;

	tmp->next=font_root;
	font_root=tmp;

	return(0);
}

PDGL_API PDGL_FontInfo *GfxFont_FindFont(char *name)
{
	PDGL_FontInfo *cur;

	cur=font_root;
	while(cur)
	{
		if(!strcmp(cur->name, name))return(cur);
		cur=cur->next;
	}
	return(NULL);
}

PDGL_API byte *GfxFont_FetchChar(PDGL_FontInfo *font, int c, int *w, int *h)
{
	fontinfo *cur;
	int i;

	cur=font;
	while(cur)
	{
		if((c>=cur->s) && (c<=cur->e))
		{
			*w=cur->w;
			*h=cur->h;
			i=c-cur->s;
			i=i*((cur->w*cur->h)/8);

//			printf("fetched char %s, %d, %d\n", cur->name, c, i);

			return(cur->buf+i);
		}
//		printf("didn't find char %d\n", c);
		cur=cur->chain;
	}
	return(NULL);
}

PDGL_API int GfxFont_GetCellBit(byte *cell, int w, int h, int x, int y, int rw, int rh)
{
	int i, j, k;

	x=(float)x*((float)w)/((float)rw);
	y=(float)y*((float)h)/((float)rh);

	if(x>=w)x=w-1;
	if(y>=h)y=h-1;

	i=(y*w)+x;
	j=i&7;
	i>>=3;

	k=cell[i];

#if 0
	if((rh<16) && (h==16))
	{
		i&=0xe;
		k=cell[i]|cell[i+1];
	}else k=cell[i];
#endif

	return((k>>(7-j))&1);
}

PDGL_API int GfxFont_RenderCell(PDGL_FontInfo *font,
	PDGL_FontFrag *frag, int c)
{
	byte *cell;
	int f, y, x;
	int i, j, k;
	int cw, ch;
	int bw, bh;
	int p;

//	f=c/64;
//	y=(c/8)&7;
//	x=c&7;

	f=c/256;
	y=(c>>4)&15;
	x=c&15;

	cell=GfxFont_FetchChar(font, c, &cw, &ch);
	if(!cell)
	{
//		printf("render cell: fail %d\n", c);
		frag->usemap[y]|=(1<<x);
		frag->badmap[y]|=(1<<x);
		return(-1);
	}

	if(!frag->buffer)
	{
		frag->buffer=gcalloc(font->w*font->h*256*4);
//		frag->buffer=gcalloc(font->w*font->h*64*4);
//		frag->buffer=gcalloc(16*16*64*4);
//		frag->base=c/64;
		frag->texnum=Tex_AllocTexnum();

		memset(frag->buffer, 0, font->w*font->h*256*4);
//		memset(frag->buffer, 0, font->w*font->h*64*4);
//		memset(frag->buffer, 0, 16*16*64*4);
	}

	bw=font->w;
	bh=font->h;
//	bw=16;
//	bh=16;

//	printf("render cell: %d %d:%d (%d %d) (%d %d)\n", c, x, y, bw, bh, cw, ch);

//	memset(frag->buffer, 255, bw*bh*64*4);

	for(i=0; i<bh; i++)for(j=0; j<bw; j++)
	{
//		p=(((y*bh)+(bh-(i+1)))*(bw*8))+((x*bw)+j);
		p=(((y*bh)+(bh-(i+1)))*(bw*16))+((x*bw)+j);
		k=GfxFont_GetCellBit(cell, cw, ch, j, i, bw, bh);
//		k=1;
		if(k)
		{
			frag->buffer[(p*4)+0]=255;
			frag->buffer[(p*4)+1]=255;
			frag->buffer[(p*4)+2]=255;
			frag->buffer[(p*4)+3]=255;
		}else
		{
			frag->buffer[(p*4)+0]=0;
			frag->buffer[(p*4)+1]=0;
			frag->buffer[(p*4)+2]=0;
			frag->buffer[(p*4)+3]=0;
		}
	}

	frag->usemap[y]|=(1<<x);
	return(0);
}

PDGL_API PDGL_FontFrag *GfxFont_TryFetchFrag(PDGL_FontInfo *font, int c)
{
	fontfrag *cur;
	int f, y, x;
	int i;

	if(!font)return(NULL);

//	f=c/64;
//	y=(c/8)&7;
//	x=c&7;

	f=c/256;
	y=(c>>4)&15;
	x=c&15;

	cur=font->frag;
	while(cur)
	{
		if(f==cur->base)break;
		cur=cur->next;
	}
	if(!cur)return(NULL);
	if(!(cur->usemap[y]&(1<<x)))
		return(NULL);
	if(cur->badmap[y]&(1<<x))
		return(NULL);
	return(cur);
}

PDGL_API PDGL_FontFrag *GfxFont_FetchFrag(PDGL_FontInfo *font, int c)
{
	fontfrag *cur;
	int f, y, x;
	int i;

	if(!font)
		return(NULL);

//	f=c/64;
//	y=(c/8)&7;
//	x=c&7;

	f=c/256;
	y=(c>>4)&15;
	x=c&15;

	cur=font->frag;
	while(cur)
	{
		if(f==cur->base)break;
		cur=cur->next;
	}

	if(!cur)
	{
		printf("new frag\n");
		cur=gcalloc(sizeof(fontfrag));
//		cur->buffer=gcalloc(font->w*font->h*64*4);
//		cur->buffer=gcalloc(16*16*64*4);
//		cur->base=c/64;
		cur->base=c/256;
//		cur->texnum=Tex_AllocTexnum();

//		memset(cur->buffer, 0, font->w*font->h*64*4);
//		memset(cur->buffer, 0, 16*16*64*4);
		memset(cur->usemap, 0, 8);

		cur->parent=font;
		cur->next=font->frag;
		font->frag=cur;
	}

	if(!(cur->usemap[y]&(1<<x)))
	{
		GfxFont_RenderCell(font, cur, c);

#if 1
//		for(i=0; i<64; i++)
		for(i=0; i<256; i++)
		{
//			x=i&7;
//			y=i>>3;

			x=i&15;
			y=i>>4;

			if(!(cur->usemap[y]&(1<<x)))
				GfxFont_RenderCell(font, cur, (cur->base<<6)+i /* c */);
		}
#endif

		if(cur->buffer)
		{
//			Tex_LoadTexture2(8*font->w, 8*font->h, cur->buffer,
//				2, cur->texnum, 0);

			Tex_LoadTexture2(16*font->w, 16*font->h, cur->buffer,
				2, cur->texnum, 0);
		}
	}

	if(cur->badmap[y]&(1<<x))
	{
		for(i=0; i<font->n_alts; i++)
		{
			cur=GfxFont_FetchFrag(font->alts[i], c);
			if(cur)return(cur);
		}
		return(NULL);
	}

	return(cur);
}

PDGL_API int GfxFont_DrawChar(int c, int x, int y, int w, int h, int r, int g, int b, int a)
{
	byte *cell;
	int cw, ch, n;
	float s1, t1, s2, t2;
	PDGL_FontFrag *frag;

//	printf("draw char %d (%d %d) (%d %d)\n", c, x, y, w, h);

	if(!c || (c==' '))return(0);

	if(font_mode&PDGL_FONT_BLINK)
	{
		if((font_time%1000)>=500)
			return(0);
	}

	frag=GfxFont_FetchFrag(font_cur, c);
	if(!frag)return(-1);

	pdglDisable (GL_CULL_FACE);
//	pdglEnable (GL_TEXTURE_2D);
	pdglEnableTexture2D();
	pdglBindTexture(GL_TEXTURE_2D, frag->texnum);

//	pdglEnable (GL_BLEND);
//	pdglBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

//	pdglShadeModel (GL_SMOOTH);

// #ifndef GLES
//	pdglTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
// #endif

//	n=c&63;

//	s1=(n%8)/8.0;
//	t1=(n/8)/8.0;
//	s2=(n%8+1)/8.0;
//	t2=(n/8+1)/8.0;

	n=c&255;

	s1=(n&15)/16.0;
	t1=(n>>4)/16.0;
	s2=((n&15)+1)/16.0;
	t2=((n>>4)+1)/16.0;

//	s1=0;
//	t1=1;
//	s2=1;
//	t2=0;

//	pdglColor4f(1, 0, 0, 1);
	pdglColor4f(r/255.0, g/255.0, b/255.0, a/255.0);

	if(font_mode&PDGL_FONT_ITALIC)
	{
#if 1
		pdglBegin(PDGL_TRIANGLES);
		pdglTexCoord2f(s1, t1);
		pdglVertex2f(x-(w*0.5), y);

		pdglTexCoord2f(s1, t2);
		pdglVertex2f(x+(w*0.5), y+h);

		pdglTexCoord2f(s2, t2);
		pdglVertex2f(x+w+(w*0.5), y+h);


		pdglTexCoord2f(s1, t1);
		pdglVertex2f(x-(w*0.5), y);

		pdglTexCoord2f(s2, t2);
		pdglVertex2f(x+w+(w*0.5), y+h);

		pdglTexCoord2f(s2, t1);
		pdglVertex2f(x+w-(w*0.5), y);
		pdglEnd();
#endif

#if 0
		pdglBegin(PDGL_QUADS);
		pdglTexCoord2f(s1, t1);
		pdglVertex2f(x-(w*0.5), y);

		pdglTexCoord2f(s1, t2);
		pdglVertex2f(x+(w*0.5), y+h);

		pdglTexCoord2f(s2, t2);
		pdglVertex2f(x+w+(w*0.5), y+h);

		pdglTexCoord2f(s2, t1);
		pdglVertex2f(x+w-(w*0.5), y);
		pdglEnd();
#endif
	}else
	{
#if 1
		pdglBegin(PDGL_TRIANGLES);
		pdglTexCoord2f(s1, t1);
		pdglVertex2f(x, y);

		pdglTexCoord2f(s1, t2);
		pdglVertex2f(x, y+h);

		pdglTexCoord2f(s2, t2);
		pdglVertex2f(x+w, y+h);


		pdglTexCoord2f(s1, t1);
		pdglVertex2f(x, y);

		pdglTexCoord2f(s2, t2);
		pdglVertex2f(x+w, y+h);

		pdglTexCoord2f(s2, t1);
		pdglVertex2f(x+w, y);
		pdglEnd();
#endif

#if 0
		pdglBegin(PDGL_QUADS);
		pdglTexCoord2f(s1, t1);
		pdglVertex2f(x, y);

		pdglTexCoord2f(s1, t2);
		pdglVertex2f(x, y+h);

		pdglTexCoord2f(s2, t2);
		pdglVertex2f(x+w, y+h);

		pdglTexCoord2f(s2, t1);
		pdglVertex2f(x+w, y);
		pdglEnd();
#endif
	}

	if((font_mode&PDGL_FONT_UNDERLINE) ||
		(font_mode&PDGL_FONT_OVERLINE) ||
		(font_mode&PDGL_FONT_STRIKEOUT))
	{
//		pdglDisable (GL_TEXTURE_2D);
		pdglDisableTexture2D();

		pdglBegin(GL_LINES);

		if(font_mode&PDGL_FONT_UNDERLINE)
		{
			pdglVertex2f(x, y);
			pdglVertex2f(x+w, y);
		}
		if(font_mode&PDGL_FONT_OVERLINE)
		{
			pdglVertex2f(x, y+h);
			pdglVertex2f(x+w, y+h);
		}
		if(font_mode&PDGL_FONT_STRIKEOUT)
		{
			pdglVertex2f(x, y+(h*0.5));
			pdglVertex2f(x+w, y+(h*0.5));
		}

		pdglEnd();
	}

//	printf("draw char %d\n", c);

//	cell=GfxFont_FetchChar(font_cur, c, &cw, &ch);
//	if(cell)GfxDrv_DrawCell(cell, cw, ch, x, y, w, h, r, g, b, a);

	return(0);
}

PDGL_API int GfxFont_DrawCharMode(int c,
	int x, int y, int w, int h, int r, int g, int b, int a, int m)
{
	int tm;
	
	tm=font_mode;
	font_mode=m;
	GfxFont_DrawChar(c, x, y, w, h, r, g, b, a);
	font_mode=tm;
}

PDGL_API int GfxFont_DrawCharModeQI(int c,
	int x, int y, int w, int h, int r, int g, int b, int a, int m)
{
	static int texn;
	static int qb;
	int cw, ch, n, tm;
	float s1, t1, s2, t2;
	fontfrag *frag;

	if(!c || (c==' '))return(0);

	if(c==-1)
	{
		texn=-1; qb=0;
		return(0);
	}

	if(c==-2)
	{
		if(qb) { pdglEnd(); qb=0; }
		return(0);
	}

//	if(font_mode&PDGL_FONT_BLINK)
	if(m&PDGL_FONT_BLINK)
	{
		if((font_time%1000)>=500)
			return(0);
	}

	tm=font_mode;
	font_mode=m;

	GfxFont_SetFontSize(font_name, font_mode, h);

	frag=GfxFont_TryFetchFrag(font_cur, c);

	if(!frag || (frag->texnum!=texn))
	{
		if(qb) { pdglEnd(); qb=0; }
		frag=GfxFont_FetchFrag(font_cur, c);
		if(!frag)
			return(-1);
		texn=frag->texnum;
	}

	if(!qb)
	{
		pdglDisable (GL_CULL_FACE);
//		pdglEnable(GL_TEXTURE_2D);
		pdglEnableTexture2D();
		pdglBindTexture(GL_TEXTURE_2D, texn);

//#ifndef GLES
//		pdglTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
//#endif

//		pdglBegin(PDGL_QUADS);
		pdglBegin(PDGL_TRIANGLES);
		qb=1;
	}

//	pdglDisable (GL_CULL_FACE);
//	pdglEnable (GL_TEXTURE_2D);
//	pdglBindTexture(GL_TEXTURE_2D, frag->texnum);

//	pdglEnable (GL_BLEND);
//	pdglBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
//	pdglShadeModel (GL_SMOOTH);
//	pdglTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);

//	n=c&63;

//	s1=(n%8)/8.0;
//	t1=(n/8)/8.0;
//	s2=(n%8+1)/8.0;
//	t2=(n/8+1)/8.0;

	n=c&255;

	s1=(n&15)/16.0;
	t1=(n>>4)/16.0;
	s2=((n&15)+1)/16.0;
	t2=((n>>4)+1)/16.0;

	pdglColor4f(r/255.0, g/255.0, b/255.0, a/255.0);

	if(font_mode&PDGL_FONT_ITALIC)
	{
#if 1
		pdglTexCoord2f(s1, t1);
		pdglVertex2f(x-(w*0.5), y);

		pdglTexCoord2f(s1, t2);
		pdglVertex2f(x+(w*0.5), y+h);

		pdglTexCoord2f(s2, t2);
		pdglVertex2f(x+w+(w*0.5), y+h);


		pdglTexCoord2f(s1, t1);
		pdglVertex2f(x-(w*0.5), y);

		pdglTexCoord2f(s2, t2);
		pdglVertex2f(x+w+(w*0.5), y+h);

		pdglTexCoord2f(s2, t1);
		pdglVertex2f(x+w-(w*0.5), y);
#endif

#if 0
		pdglTexCoord2f(s1, t1);
		pdglVertex2f(x-(w*0.5), y);

		pdglTexCoord2f(s1, t2);
		pdglVertex2f(x+(w*0.5), y+h);

		pdglTexCoord2f(s2, t2);
		pdglVertex2f(x+w+(w*0.5), y+h);

		pdglTexCoord2f(s2, t1);
		pdglVertex2f(x+w-(w*0.5), y);
#endif
	}else
	{
#if 1
		pdglTexCoord2f(s1, t1);
		pdglVertex2f(x, y);

		pdglTexCoord2f(s1, t2);
		pdglVertex2f(x, y+h);

		pdglTexCoord2f(s2, t2);
		pdglVertex2f(x+w, y+h);


		pdglTexCoord2f(s1, t1);
		pdglVertex2f(x, y);

		pdglTexCoord2f(s2, t2);
		pdglVertex2f(x+w, y+h);

		pdglTexCoord2f(s2, t1);
		pdglVertex2f(x+w, y);
#endif

#if 0
		pdglTexCoord2f(s1, t1);
		pdglVertex2f(x, y);

		pdglTexCoord2f(s1, t2);
		pdglVertex2f(x, y+h);

		pdglTexCoord2f(s2, t2);
		pdglVertex2f(x+w, y+h);

		pdglTexCoord2f(s2, t1);
		pdglVertex2f(x+w, y);
#endif
	}

	font_mode=tm;

	return(0);
}

PDGL_API int GfxFont_DrawString(char *str, int x, int y, int w, int h,
	int r, int g, int b, int a)
{
	unsigned char *s;
	int i;

	GfxFont_SetFontSize(font_name, font_mode, h);

	s=(unsigned char *)str;
	while(*s)
	{
		i=*s++;
		if(i>=0xF0)
		{
			i=(i&7)<<18;
			i+=((*s++)&63)<<12;
			i+=((*s++)&63)<<6;
			i+=(*s++)&63;
		}else if(i>=0xE0)
		{
			i=(i&15)<<12;
			i+=((*s++)&63)<<6;
			i+=(*s++)&63;
		}else if(i>=0xC0)
		{
			i=(i&15)<<6;
			i+=(*s++)&63;
		}else if(i>=0x80)
		{
			continue;
		}

		GfxFont_DrawChar(i, x, y, w, h, r, g, b, a);
		x+=w;
	}
	return(0);
}

PDGL_API int GfxFont_DrawString2(char *str, int x, int y, int w, int h,
	int r, int g, int b, int a)
{
	unsigned char *s;
	int cx, cy;
	int m, i;

	s=(unsigned char *)str;

	GfxFont_SetFontSize(font_name, font_mode, h);

	cx=x;
	cy=y;

	while(*s)
	{
		if(*s<' ')
		{
			switch(*s)
			{
			case '\r':
				cx=x;
				break;
			case '\n':
				cx=x;
				cy-=h;
				break;
			case '\t':
				i=(cx-x)/w;
				cx=x+((i+8)&(~7))*w;
				break;

			default:
				break;
			}

			s++;
			continue;
		}

		i=*s++;
		if(i>=0xF0)
		{
			i=(i&7)<<18;
			i+=((*s++)&63)<<12;
			i+=((*s++)&63)<<6;
			i+=(*s++)&63;
		}else if(i>=0xE0)
		{
			i=(i&15)<<12;
			i+=((*s++)&63)<<6;
			i+=(*s++)&63;
		}else if(i>=0xC0)
		{
			i=(i&15)<<6;
			i+=(*s++)&63;
		}else if(i>=0x80)
		{
			continue;
		}

		GfxFont_DrawChar(i, cx, cy, w, h, r, g, b, a);
		cx+=w;
	}

	return(0);
}

PDGL_API int GfxFont_DrawFormatString(char *str, int x, int y, int w, int h,
	int r, int g, int b, int a)
{
	unsigned char *s;
	int cx, cy, lu;
	int m, i;

	s=(unsigned char *)str;

	cx=x;
	cy=y;
	lu=0;

	m=font_mode;
	GfxFont_SetFontSize(font_name, font_mode, h);

	while(*s)
	{
		if(*s<' ')
		{
			switch(*s)
			{
			case '\r':
				cx=x;
				break;
			case '\n':
				cx=x;
				cy-=h;
				break;
			case '\t':
				i=(cx-x)/w;
				cx=x+((i+8)&(~7))*w;
				break;

			default:
				break;
			}

			s++;
			continue;
		}

		if(s[0]!=s[1])
		{
			i=1;
			switch(*s)
			{
			case '_':
				font_mode^=PDGL_FONT_UNDERLINE;
				break;
			case '*':
				font_mode^=PDGL_FONT_BOLD;
//				GfxFont_SetFont(font_name, font_mode);
				GfxFont_SetFontSize(font_name, font_mode, h);
				break;
			case '/':
				font_mode^=PDGL_FONT_ITALIC;
				break;
			case '-':
				font_mode^=PDGL_FONT_STRIKEOUT;
				break;

			case '&':
				if(!(font_mode&PDGL_FONT_UNDERLINE))
				{
					font_mode|=PDGL_FONT_UNDERLINE;
					lu=1;
				}
				break;

			default:
				i=0;
				break;
			}

			if(i)
			{
				s++;
				continue;
			}
		}else
		{
			switch(*s)
			{
			case '_':
			case '*':
			case '/':
			case '-':
			case '&':
				s++;
				break;
			default:
				break;
			}
		}

		i=*s++;
		if(i>=0xF0)
		{
			i=(i&7)<<18;
			i+=((*s++)&63)<<12;
			i+=((*s++)&63)<<6;
			i+=(*s++)&63;
		}else if(i>=0xE0)
		{
			i=(i&15)<<12;
			i+=((*s++)&63)<<6;
			i+=(*s++)&63;
		}else if(i>=0xC0)
		{
			i=(i&15)<<6;
			i+=(*s++)&63;
		}else if(i>=0x80)
		{
			continue;
		}

		GfxFont_DrawChar(i, cx, cy, w, h, r, g, b, a);
		cx+=w;

		if(lu)
		{
			font_mode&=~PDGL_FONT_UNDERLINE;
			lu=0;
		}
	}
	font_mode=m;
	GfxFont_SetFont(font_name, font_mode);
//	GfxFont_SetFontSize(font_name, font_mode, h);

	return(0);
}

PDGL_API int GfxFont_SetTime(int time)
{
	font_time=time;
	return(0);
}

PDGL_API int GfxFont_SetFont(char *name, int mode)
{
	return(GfxFont_SetFontSize(name, mode, 0));
}

PDGL_API int GfxFont_SetFontSize(char *name, int mode, int size)
{
	char tb[64];
	fontinfo *font;
	int sz;

	if(!name)
	{
		return(-1);
	}

	if(font_name && !strcmp(name, font_name))
	{
		if((font_mode==mode) && (font_size==size))
			return(0);
	}

	font_name=dystrdup(name);
	font_mode=mode;
	font_size=size;

	font=NULL;

	if(!font && (size>0))
	{
		sz=size;
		if(!font && (mode&PDGL_FONT_BOLD))
		{
			sprintf(tb, "%s_%dpx_B", name, sz);
			font=GfxFont_FindFont(tb);
		}else
		{
			sprintf(tb, "%s_%dpx", name, sz);
			font=GfxFont_FindFont(tb);
		}
	}
	
	if(!font && (size>0))
	{
		sz=1;
//		while(sz<size)sz=sz<<1;
//		while((sz<<1)<size)sz=sz<<1;
//		while(((sz*3)>>1)<size)sz=sz<<1;
//		while(((sz*5)>>2)<size)sz=sz<<1;
		while(((sz*23)>>4)<size)sz=sz<<1;
	
		if(!font && (mode&PDGL_FONT_BOLD))
		{
			sprintf(tb, "%s_%dpx_B", name, sz);
			font=GfxFont_FindFont(tb);
		}else
		{
			sprintf(tb, "%s_%dpx", name, sz);
			font=GfxFont_FindFont(tb);
		}
	}
	
	if(!font && (mode&PDGL_FONT_BOLD))
	{
		sprintf(tb, "%s_B", name);
		font=GfxFont_FindFont(tb);
	}
	if(!font)font=GfxFont_FindFont(name);

	if(font)font_cur=font;
		else printf("no font '%s'\n", name);
	return(0);
}

PDGL_API int GfxFont_LoadFont(char *name, char *fname)
{
	return(GfxFont_LoadFontBase(name, fname, 0));
}

PDGL_API int GfxFont_LoadFontBase(char *name, char *fname, int base)
{
	VFILE *fd;
	fontinfo *tmp, *cur;
	byte *buf;
	int s, e;
	int cw, ch;

	printf("loading font '%s':'%s'\n", name, fname);

	fd=vffopen(fname, "rb");
	if(!fd)
	{
		printf("Failed load font '%s':'%s'\n", name, fname);
		return(-1);
	}
	cur=NULL;

	s=vfgetc(fd);
	s+=vfgetc(fd)<<8;

	e=vfgetc(fd);
	e+=vfgetc(fd)<<8;

	if(s==0xBF00)
	{
		buf=gcralloc(16);
		vfread(buf, 1, e, fd);
		cw=buf[1];
		ch=buf[2];
	}else
	{
		printf("no header\n");
		return(-1);
	}

	while(1)
	{
		s=vfgetc(fd);
		s+=vfgetc(fd)<<8;

		e=vfgetc(fd);
		e+=vfgetc(fd)<<8;

		if(!s && !e)break;

//		printf("chunk %d-%d\n", s, e);

		buf=malloc(((e-s)+1)*((cw*ch+7)/8));
		vfread(buf, (e-s)+1, ((cw*ch+7)/8), fd);

		tmp=gcalloc(sizeof(fontinfo));
		tmp->name=dystrdup(name);
		tmp->buf=buf;
		tmp->w=cw;
		tmp->h=ch;
		tmp->s=s+base;
		tmp->e=e+base;

		tmp->chain=cur;
		cur=tmp;
	}
	if(cur)
	{
		cur->next=font_root;
		font_root=cur;
	}
	return(0);
}

PDGL_API int GfxFont_AddFontSearch(char *name, char *cname)
{
	fontinfo *f, *f2;
	int i;

	f=GfxFont_FindFont(name);
	f2=GfxFont_FindFont(cname);

	if(!f || !f2)
		return(-1);

	i=f->n_alts++;
	f->alts[i]=f2;

	return(0);

//	return(GfxFont_LoadFontBase(name, fname, 0));
}

PDGL_API int GfxFont_Init()
{
	fontinfo *f, *f2;

	font_root=NULL;
	font_cur=NULL;

//	GfxFont_Register("fixed", font_8x8_32_96, 8, 8,  32, 127);
//	GfxFont_Register("koi8-r", koi8r_8x16, 8, 16, 32, 255);
//	GfxFont_Register("cp866", cp866_8x16, 8, 16, 32, 255);

	GfxFont_Register("mouse", mouse_16x16, 16, 16, 32, 32);

//	GfxFont_LoadFont("unicode", "etl16.bfn");
//	GfxFont_LoadFont("bgb0", "fonts/bgb0.bfn");
	GfxFont_LoadFont("unicode", "fonts/unifont.bfn");

//	GfxFont_LoadFont("fixed", "fonts/fixed.bfn");
	GfxFont_LoadFont("fixed_B", "fonts/fixed_b.bfn");

//	GfxFont_LoadFont("fixed_12px", "fonts/fixed_12px.bfn");

//	GfxFont_LoadFont("fixed_16px", "fonts/unifont.bfn");
//	GfxFont_LoadFont("fixed_16px", "fonts/fixed_32px.bfn");
//	GfxFont_LoadFont("fixed_16px", "fonts/fixed_16px.bfn");
	GfxFont_LoadFont("fixed_16px", "fonts/fixed_cp437_16px.bfn");
	GfxFont_LoadFont("fixed_32px", "fonts/fixed_32px.bfn");
	GfxFont_LoadFont("fixed_64px", "fonts/fixed_64px.bfn");

	GfxFont_LoadFont("fixed",			"fonts/fixed_cp437_8px.bfn");
	GfxFont_LoadFont("fixed_cp437_8px", "fonts/fixed_cp437_8px.bfn");

	GfxFont_LoadFont("gfx", "fonts/gfx.bfn");

//	GfxFont_AddFontSearch("fixed", "fixed_cp437_8px");
	GfxFont_AddFontSearch("fixed", "unicode");
	GfxFont_AddFontSearch("fixed_16px", "unicode");
	GfxFont_AddFontSearch("fixed_32px", "unicode");
	GfxFont_AddFontSearch("fixed_64px", "unicode");

#if 0
	f=GfxFont_FindFont("fixed");
	f2=GfxFont_FindFont("unicode");
	if(f)f->chain=f2;

	f=GfxFont_FindFont("fixed_16px");
	if(f)f->chain=f2;
	f=GfxFont_FindFont("fixed_32px");
	if(f)f->chain=f2;
	f=GfxFont_FindFont("fixed_64px");
	if(f)f->chain=f2;
#endif

	return(0);
}
