#include <pdgl.h>

int pdgl_text_ox, pdgl_text_oy;	//text origin
int pdgl_text_cx, pdgl_text_cy;	//text position at present
int pdgl_text_w, pdgl_text_h;
int pdgl_text_r, pdgl_text_g, pdgl_text_b, pdgl_text_a;

/*--
Cat lbxgl;C Interface
Form
	int PDGL_SetTextOrigin(int x, int y);
Description
	Sets the current text origin.
	This is used when rendering text to know where to render to.
	Text is rendered aligned on the XY plane.
--*/
PDGL_API int PDGL_SetTextOrigin(int x, int y)
{
	pdgl_text_ox=x;
	pdgl_text_oy=y;
	pdgl_text_cx=pdgl_text_ox;
	pdgl_text_cy=pdgl_text_oy;
	return(0);
}

/*--
Cat lbxgl;C Interface
Form
	int PDGL_SetTextSize(int sz);
Description
	Set the size of the text.
--*/
PDGL_API int PDGL_SetTextSize(int sz)
{
	pdgl_text_w=sz*(3.0/4.0);
	pdgl_text_h=sz;
	return(0);
}

/*--
Cat lbxgl;C Interface
Form
	int PDGL_SetTextSize2(int w, int h);
Description
	Set the size of the text.
--*/
PDGL_API int PDGL_SetTextSize2(int w, int h)
{
	pdgl_text_w=w;
	pdgl_text_h=h;
	return(0);
}

/*--
Cat lbxgl;C Interface
Form
	int PDGL_SetTextColor(float *c);
Description
	Set the text color, <R G B A>, [0..1].
--*/
PDGL_API int PDGL_SetTextColor(float *c)
{
	pdgl_text_r=c[0];
	pdgl_text_g=c[1];
	pdgl_text_b=c[2];
	pdgl_text_a=c[3];
	return(0);
}

/*--
Cat lbxgl;C Interface
Form
	int PDGL_SetTextColor2(int r, int g, int b, int a);
Description
	Set the text color, <R G B A>, [0..255].
--*/
PDGL_API int PDGL_SetTextColor2(int r, int g, int b, int a)
{
	pdgl_text_r=r;
	pdgl_text_g=g;
	pdgl_text_b=b;
	pdgl_text_a=a;
	return(0);
}
/*--
Cat lbxgl;C Interface
Form
	int PDGL_DrawText(char *str);
Description
	Draw text at the specified origin, size, and color.
	Linebreaks and other control characters are handled.
	The current position is moved to reflect the printed text, \
	however, the origin is used as the upper left corner of the \
	text, thus serving as a left margin for linebreaks and such.
--*/
PDGL_API int PDGL_DrawText(char *str)
{
	char *s;
	int i;

	s=str;

	while(*s)
	{
		if(*s<' ')
		{
			switch(*s)
			{
			case '\r':
				pdgl_text_cx=pdgl_text_ox;
				break;
			case '\n':
				pdgl_text_cx=pdgl_text_ox;
				pdgl_text_cy-=pdgl_text_h;
				break;
			case '\t':
				i=pdgl_text_cx-pdgl_text_ox;
				i+=pdgl_text_w*8;
				i-=i%(pdgl_text_w*8);
				pdgl_text_cx=i;
				break;
			default:
				break;
			}
		}else
		{
			GfxFont_DrawChar(*s,
				pdgl_text_cx, pdgl_text_cy,
				pdgl_text_w, pdgl_text_h,
				pdgl_text_r, pdgl_text_g,
				pdgl_text_b, pdgl_text_a);
			pdgl_text_cx+=pdgl_text_w;
		}
		s++;
	}
	return(0);
}
