#include <pdgl.h>

#include <time.h>
#include <stdarg.h>

int con_down=0;
int con_screen_xs;
int con_screen_ys;

// static char conbuf[75][100];
// static short conclr[75][100];
// static char promptbuf[100], promptpos;
// static char histbuf[16][100], histpos;
// static char completebuf[100];

static int (*con_handler[16])(char *s);
static char *(*con_complete[16])(char *s);
static char con_mchar[16];
static int con_modes=0;

// static int con_curclr=0x0007;
// static char *con_stuffcmd[1024];
// static int con_nstuffcmd=0;

BTGE_ConsoleInfo *con_info;			//current active console
BTGE_ConsoleInfo *con_base;			//base console (console 1)
BTGE_ConsoleInfo *con_context[9];	//console contexts

PDGL_API int GfxFont_SetFont(char *name, int mode);
PDGL_API int GfxFont_DrawChar(int c, int x, int y, int w, int h,
	int r, int g, int b, int a);

PDGL_API int Con_HandleKey(int num, int down);

PDGL_API int Con_DoExecCmds(void)
{
	while(con_base->nstuffcmd>0)
	{
//		ConCmds_Handler(con_stuffcmd[0]);
//		for(i=0; i<con_nstuffcmd; i++)
//			con_stuffcmd[i]=con_stuffcmd[i+1];
		con_base->nstuffcmd--;
		ConCmds_Handler(con_base->stuffcmd[con_base->nstuffcmd]);
	}
	return(0);
}

PDGL_API int Con_Update(float dt)
{
	int i;

//	Con_UpdateInfo(con_base, dt);

	for(i=0; i<9; i++)
	{
		if(con_context[i]->Update)
			con_context[i]->Update(con_context[i], dt);
	}
	
	return(0);
}

PDGL_API int Con_UpdateInfo(BTGE_ConsoleInfo *con, float dt)
{
	int i;

	if(con->nstuffcmd>0)
	{
//		ConCmds_Handler(con_stuffcmd[0]);
//		for(i=0; i<con_nstuffcmd; i++)
//			con_stuffcmd[i]=con_stuffcmd[i+1];
		con->nstuffcmd--;
		ConCmds_Handler(con->stuffcmd[con->nstuffcmd]);
	}
	return(0);
}

PDGL_API int Con_StuffCmdInfo(BTGE_ConsoleInfo *con, char *str)
{
	con->stuffcmd[con->nstuffcmd++]=dystrdup(str);
	return(0);
}

PDGL_API int Con_StuffCmd(char *str)
	{ return(Con_StuffCmdInfo(con_base, str)); }

PDGL_API int Con_StuffCmdBuf(char *buf)
{
	char *ta[768];
	char tb[256];
	char *s, *t;
	int i, n;
	
	s=buf; t=tb; n=0;
	while(*s)
	{
		if(*s=='\"')
		{
			*t++=*s++;
			while(*s)
			{
				if(*s=='\r')break;
				if(*s=='\n')break;
				if(*s=='\\')
				{
					*t++=*s++;
					*t++=*s++;
					continue;
				}
				if(*s=='\"')
				{
					*t++=*s++;
					break;
				}
				*t++=*s++;
			}
			continue;
		}
	
		if((*s=='\r') || (*s=='\n') || ((s[0]==';') && (s[1]==' ')))
		{
			*t++=0;
			if(tb[0] && (tb[0]!='#') && (tb[0]!='/'))
				ta[n++]=dystrdup(tb);

			if((s[0]==';') && (s[1]==' '))
				s+=2;

//			if(*s=='\r')s++;
//			if(*s=='\n')s++;

			while(*s && (*s<=' '))s++;

			t=tb;
			continue;
		}
		
		*t++=*s++;
	}

	*t++=0;
	if(tb[0] && (tb[0]!='#') && (tb[0]!='/'))
		ta[n++]=dystrdup(tb);

	for(i=n-1; i>=0; i--)
		Con_StuffCmd(ta[i]);
	return(0);
}

PDGL_API int Con_ExecCmdBuf(char *name)
{
	char *buf;
	int sz;
	
	buf=vf_loadfile(name, &sz);
	if(buf)
	{
		Con_StuffCmdBuf(buf);
		gcfree(buf);
	}

	return(0);
}

PDGL_API int Con_IsDown()
{
	return(con_down);
}

PDGL_API int Con_SetHandler(char ch, int (*handler)(char *s), int down)
{
	con_handler[down]=handler;
	con_mchar[down]=ch;
	if(down>con_modes)con_modes=down;
	return(0);
}

PDGL_API int Con_AddMode(char ch, int (*handler)(char *s))
{
	con_modes++;
	con_handler[con_modes]=handler;
	con_mchar[con_modes]=ch;
	return(0);
}

PDGL_API int Con_AddModeComplete(char ch, int (*handler)(char *s),
	char *(*complete)(char *s))
{
	con_modes++;
	con_handler[con_modes]=handler;
	con_complete[con_modes]=complete;
	con_mchar[con_modes]=ch;
	return(0);
}

PDGL_API int Con_SetDown(int down)
{
	con_down=down;
	con_down%=con_modes+1;
	return(0);
}

PDGL_API int Con_Clear()
{
//	Con_ClearInfo(con_info);
	if(con_info->Clear)
		return(con_info->Clear(con_info));
	return(-1);
}

PDGL_API int Con_ClearInfo(BTGE_ConsoleInfo *con)
{
	int i;
	for(i=0; i<con->ys; i++)
	{
		con_memset(&con->conbuf[i][0], 0, con->xs);
		con_memset(&con->conclr[i][0], 0, con->xs);
	}
	return(0);
}

PDGL_API int Con_Init()
{
	static int init=0;
	int i;
	
	if(init)return(1);
	init=1;

	//create consoles...
	
	for(i=0; i<9; i++)
	{
		con_info=BTGE_Console_AllocConsoleInfo();
		con_context[i]=con_info;

		con_info->curclr=0x0007;
		con_info->x=0;
		con_info->y=74;
		con_info->xs=100;
		con_info->ys=75;
		con_info->WriteString=Con_WriteString2;
		con_info->Update=Con_UpdateInfo;
		con_info->HandleKey=Con_HandleKeyInfo;
		con_info->Clear=Con_ClearInfo;
//		con_info->Render=Con_RenderInfo;
		con_info->KeyDown=Con_KeyDown;

		con_info->Render=Con_RenderInfo2;
	}

//	con_info=BTGE_Console_AllocConsoleInfo();
//	con_info->curclr=0x0007;
//	con_info->x=0;
//	con_info->y=74;
//	con_info->WriteString=Con_WriteString2;

	con_info=con_context[0];
	con_base=con_info;


	PDGL_AddKeyHandler(&Con_HandleKey);
	gcAddLogHook(Con_WriteStringBase);

	con_modes=0;
	return(0);
}

PDGL_API int Con_ReInit()
{
	BTGE_ConsoleInfo *con;
	int wxs, wys;
	int i;

	GfxDrv_GetWindowSize(&con_screen_xs, &con_screen_ys);
	wxs=con_screen_xs; wys=con_screen_ys;

	for(i=0; i<9; i++)
	{
		con=con_context[i];
		con->x=0;
		con->y=(wys/8)-1;
		con->xs=wxs/8;
		con->ys=wys/8;
	}
}

PDGL_API int Con_RenderBackground()
	{ return(Con_RenderBackgroundInfo(con_info)); }

PDGL_API int Con_RenderBackgroundInfo(BTGE_ConsoleInfo *con)
{
	int x, y, wxs, wys, wxs2, wys2;
	int i, j, k, l;

	wxs=con_screen_xs; wys=con_screen_ys;
	wxs2=wxs/2; wys2=wys/2;

//	pdglBegin(PDGL_QUADS);
	pdglBegin(PDGL_TRIANGLES);
	l=-1;
//	for(i=0; i<(con_down?75:15); i++)
	for(i=0; i<(con_down?con->ys:15); i++)
	{
		for(j=0; j<con->xs; j++)
		{
//			if(!conbuf[74-i][j])break;
//			k=con->conclr[74-i][j];
			k=con->conclr[(con->ys-1)-i][j];

			if((k!=l) && !(k&CON_EFF_CHARESC))
			{
				pdglColor4f(
					((k&16)?0.75:0)+((k&128)?0.25:0),
					((k&32)?0.75:0)+((k&128)?0.25:0),
					((k&64)?0.75:0)+((k&128)?0.25:0),
					0.75);
				l=k;
			}
//			Draw_Square((j*8)-400, (i*8)-300, 8, 8);

			x=(j*8)-wxs2;
			y=(i*8)-wys2;
#if 0
//			glBegin(GL_QUADS);
			pdglVertex2f(x,	y);
			pdglVertex2f(x,	y+8);
			pdglVertex2f(x+8,	y+8);
			pdglVertex2f(x+8,	y);
//			glEnd();
#endif

			pdglVertex2f(x,	y);
			pdglVertex2f(x,	y+8);
			pdglVertex2f(x+8,	y+8);

			pdglVertex2f(x,	y);
			pdglVertex2f(x+8,	y+8);
			pdglVertex2f(x+8,	y);
		}
	}
	pdglEnd();
}

PDGL_API int Con_Render()
{
	if(con_info->Render)
		return(con_info->Render(con_info));
	return(-1);

//	return(Con_RenderInfo(con_info));
}

PDGL_API int Con_RenderInfo(BTGE_ConsoleInfo *con)
{
	int cr, cg, cb, ca, cm, wxs, wys, wxs2, wys2;
	int c;
	int i, j, k;

//	if(!con_down)return(0);

	GfxDrv_GetWindowSize(&con_screen_xs, &con_screen_ys);
	wxs=con_screen_xs; wys=con_screen_ys;
	wxs2=wxs/2; wys2=wys/2;

//	GfxFont_SetFont("fixed", 0);
	GfxFont_SetFontSize("fixed", 0, 8);

//	glDisable(GL_ALPHA_TEST);
	glDisable(GL_CULL_FACE);
	glDisable(GL_TEXTURE_2D);

	if(con_down)
	{
//		glColor4f(0.25, 0.25, 0.25, 0.75);
//		Draw_Square(-400, -300, 800, 600);
		Con_RenderBackground();
	}

//	glEnable(GL_TEXTURE_2D);
//	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);

	GfxFont_DrawCharModeQI(-1, 0,0,0,0, 0,0,0,0, 0);
//	for(i=0; i<(con_down?75:15); i++)
	for(i=0; i<(con_down?con->ys:15); i++)
	{
//		for(j=0; j<100; j++)
		for(j=0; j<con->xs; j++)
		{
//			if(!con->conbuf[74-i][j])break;
			if(!con->conbuf[(con->ys-1)-i][j])
				continue;
			if(con->conbuf[(con->ys-1)-i][j]==' ')
				continue;
			k=con->conclr[(con->ys-1)-i][j];

#if 0
			if(k&0x400)		//blink
			{
				cr=PDGL_TimeMS()/250;
				if(cr&1)continue;
			}
#endif

			cr=((k&1)?191:0)+((k&8)?64:0);
			cg=((k&2)?191:0)+((k&8)?64:0);
			cb=((k&4)?191:0)+((k&8)?64:0);
			ca=(k&CON_EFF_FAINT)?192:255;	//faint
			cm=0;
			
			if(k&CON_EFF_ITALIC)cm|=PDGL_FONT_ITALIC;
			if(k&CON_EFF_UNDERLINE)cm|=PDGL_FONT_UNDERLINE;
			if(k&CON_EFF_BLINK)cm|=PDGL_FONT_BLINK;
//			if(k&CON_EFF_STRIKEOUT)cm|=PDGL_FONT_STRIKEOUT;
			if(k&CON_EFF_BOLD)cm|=PDGL_FONT_BOLD;
			
			if(k&CON_EFF_SUBSUPER)
			{
				if(k&CON_EFF_STRIKEOUT)
					{ cm|=PDGL_FONT_SUBSCRIPT; }
				else
					{ cm|=PDGL_FONT_SUPERSCRIPT; }
			}else
			{
				if(k&CON_EFF_STRIKEOUT)
					cm|=PDGL_FONT_STRIKEOUT;
			}
			
//			GfxFont_DrawCharMode(conbuf[74-i][j],
//				(j*8)-400, (i*8)-300, 8, 8,
//				cr, cg, cb, ca, cm);

			c=con->conbuf[(con->ys-1)-i][j];
			if(k&CON_EFF_CHARESC)
			{
				c|=((k>>4)&15)<<20;
			}

			GfxFont_DrawCharModeQI(c,
				(j*8)-wxs2, (i*8)-wys2, 8, 8,
				cr, cg, cb, ca, cm);
		}
	}
	GfxFont_DrawCharModeQI(-2, 0,0,0,0, 0,0,0,0, 0);

//	Draw_Character(-1000, -750, 25, 25, '~');
	if(con_down)
	{
		GfxFont_DrawChar(con_mchar[con_down],
			-wxs2, -wys2, 8, 8, 255, 255, 255, 255);
	}else
	{
		GfxFont_DrawChar('X',
			-wxs2, -wys2, 8, 8, 255, 0, 0, 255);
	}

	j=con_strlen(con->promptbuf);
	for(i=j; i<con->xs; i++)
		GfxFont_DrawChar(con->completebuf[i],
			-wxs2+16+(i*8), -wys2, 8, 8,
			127, 127, 127, 255);

	for(i=0; i<con->xs; i++)
		GfxFont_DrawChar(con->promptbuf[i],
			-wxs2+16+(i*8), -wys2, 8, 8,
			255, 255, 255, 255);

//	for(i=0; i<80; i++)
//		Draw_Character(i*25-1000+25, -750, 25, 25, promptbuf[i]);

	i=(clock()*1000)/CLOCKS_PER_SEC;
	i=(i/250)&1;
	j=con->promptpos;

	if(i)GfxFont_DrawChar('_',
		-wxs2+16+(j*8), -wys2, 8, 8,
		255, 255, 255, 255);

//	if(i)Draw_Character(-1000+(promptpos+1)*25, -750, 25, 25, '_');

	return(0);
}

PDGL_API int Con_RenderBackgroundVBO(
	BTGE_ConsoleInfo *con, PDGL_TextVBO *vbo)
{
	int x, y, wxs, wys, wxs2, wys2;
	int i, j, k, l;

	wxs=con_screen_xs; wys=con_screen_ys;
	wxs2=wxs/2; wys2=wys/2;

	PDGL_TextVBO_Begin(vbo, PDGL_TRIANGLES);
	l=-1;
	for(i=0; i<(con_down?con->ys:15); i++)
	{
		for(j=0; j<con->xs; j++)
		{
			k=con->conclr[(con->ys-1)-i][j];

			if((k!=l) && !(k&CON_EFF_CHARESC))
			{
				PDGL_TextVBO_Color4f(vbo,
					((k&16)?0.75:0)+((k&128)?0.25:0),
					((k&32)?0.75:0)+((k&128)?0.25:0),
					((k&64)?0.75:0)+((k&128)?0.25:0),
					0.75);
				l=k;
			}
//			Draw_Square((j*8)-400, (i*8)-300, 8, 8);

			x=(j*8)-wxs2;
			y=(i*8)-wys2;

			PDGL_TextVBO_Vertex2f(vbo, x,	y);
			PDGL_TextVBO_Vertex2f(vbo, x,	y+8);
			PDGL_TextVBO_Vertex2f(vbo, x+8,	y+8);

			PDGL_TextVBO_Vertex2f(vbo, x,	y);
			PDGL_TextVBO_Vertex2f(vbo, x+8,	y+8);
			PDGL_TextVBO_Vertex2f(vbo, x+8,	y);
		}
	}
	PDGL_TextVBO_End(vbo);
}

PDGL_API int Con_RenderTextVBO(
	BTGE_ConsoleInfo *con, PDGL_TextVBO *vbo)
{
	int cr, cg, cb, ca, cm, wxs, wys, wxs2, wys2;
	int c;
	int i, j, k;

//	if(!con_down)return(0);

	GfxDrv_GetWindowSize(&con_screen_xs, &con_screen_ys);
	wxs=con_screen_xs; wys=con_screen_ys;
	wxs2=wxs/2; wys2=wys/2;

//	GfxFont_SetFont("fixed", 0);
	GfxFont_SetFontSize("fixed", 0, 8);

	if(con_down)
	{
		i=Tex_LoadFile("textures/effects/white", NULL, NULL);
		PDGL_TextVBO_BindTexture(vbo, i);
		Con_RenderBackgroundVBO(con, vbo);
	}

	PDGL_TextVBO_DrawCharModeQI2(vbo, -1, 0,0,0,0, 0,0,0,0, 0);
	for(i=0; i<(con_down?con->ys:15); i++)
	{
		for(j=0; j<con->xs; j++)
		{
			if(!con->conbuf[(con->ys-1)-i][j])
				continue;
			if(con->conbuf[(con->ys-1)-i][j]==' ')
				continue;
			k=con->conclr[(con->ys-1)-i][j];

			cr=((k&1)?191:0)+((k&8)?64:0);
			cg=((k&2)?191:0)+((k&8)?64:0);
			cb=((k&4)?191:0)+((k&8)?64:0);
			ca=(k&CON_EFF_FAINT)?192:255;	//faint
			cm=0;
			
			if(k&CON_EFF_ITALIC)cm|=PDGL_FONT_ITALIC;
			if(k&CON_EFF_UNDERLINE)cm|=PDGL_FONT_UNDERLINE;
			if(k&CON_EFF_BLINK)cm|=PDGL_FONT_BLINK;
//			if(k&CON_EFF_STRIKEOUT)cm|=PDGL_FONT_STRIKEOUT;
			if(k&CON_EFF_BOLD)cm|=PDGL_FONT_BOLD;
			
			if(k&CON_EFF_SUBSUPER)
			{
				if(k&CON_EFF_STRIKEOUT)
					{ cm|=PDGL_FONT_SUBSCRIPT; }
				else
					{ cm|=PDGL_FONT_SUPERSCRIPT; }
			}else
			{
				if(k&CON_EFF_STRIKEOUT)
					cm|=PDGL_FONT_STRIKEOUT;
			}
			
			c=con->conbuf[(con->ys-1)-i][j];
			if(k&CON_EFF_CHARESC)
			{
				c|=((k>>4)&15)<<20;
			}

			PDGL_TextVBO_DrawCharModeQI2(vbo, c,
				(j*8)-wxs2, (i*8)-wys2, 8, 8,
				cr, cg, cb, ca, cm);
		}
	}
//	PDGL_TextVBO_DrawCharModeQI2(vbo, -2, 0,0,0,0, 0,0,0,0, 0);

//	Draw_Character(-1000, -750, 25, 25, '~');
	if(con_down)
	{
		PDGL_TextVBO_DrawCharModeQI2(vbo, con_mchar[con_down],
			-wxs2, -wys2, 8, 8, 255, 255, 255, 255, 0);
	}else
	{
		PDGL_TextVBO_DrawCharModeQI2(vbo, 'X',
			-wxs2, -wys2, 8, 8, 255, 0, 0, 255, 0);
	}

	j=con_strlen(con->promptbuf);
	for(i=j; i<con->xs; i++)
		PDGL_TextVBO_DrawCharModeQI2(vbo, con->completebuf[i],
			-wxs2+16+(i*8), -wys2, 8, 8,
			127, 127, 127, 255, 0);

	for(i=0; i<con->xs; i++)
		PDGL_TextVBO_DrawCharModeQI2(vbo, con->promptbuf[i],
			-wxs2+16+(i*8), -wys2, 8, 8,
			255, 255, 255, 255, 0);

//	for(i=0; i<80; i++)
//		Draw_Character(i*25-1000+25, -750, 25, 25, promptbuf[i]);

	i=(clock()*1000)/CLOCKS_PER_SEC;
	i=(i/250)&1;
	j=con->promptpos;

	if(i)PDGL_TextVBO_DrawCharModeQI2(vbo, '_',
		-wxs2+16+(j*8), -wys2, 8, 8,
		255, 255, 255, 255, 0);

	PDGL_TextVBO_DrawCharModeQI2(vbo, -2, 0,0,0,0, 0,0,0,0, 0);

	return(0);
}

PDGL_API int Con_RenderInfo2(BTGE_ConsoleInfo *con)
{
	PDGL_TextVBO *vbo;
	int dirty;
	int i, j;

	vbo=con->vbo_info; dirty=0;
	if(!vbo)
	{
		vbo=PDGL_TextVBO_Create();
		con->vbo_info=vbo;
		dirty=1;
	}

	i=(clock()*1000)/CLOCKS_PER_SEC;
	i=(i/250)&1;

	if(i && !(con->flags&CON_FLAG_BLINKSTATE))
		{ dirty=1; con->flags|=CON_FLAG_BLINKSTATE; }
	if(!i && (con->flags&CON_FLAG_BLINKSTATE))
		{ dirty=1; con->flags&=~CON_FLAG_BLINKSTATE; }

	if(con->flags&CON_FLAG_DIRTY)
	{
		dirty=1;
		con->flags&=~CON_FLAG_DIRTY;
	}

	if(dirty)
	{
		PDGL_TextVBO_Reset(vbo);
		Con_RenderTextVBO(con, vbo);
		PDGL_TextVBO_Upload(vbo);
		dirty=0;
	}

	PDGL_TextVBO_Draw(vbo);
}

void cb_scrollup()
	{ Con_ScrollUpInfo(con_info); }

void con_memcpy(short *dst, short *src, int cnt)
{
	int i;
	for(i=0; i<cnt; i++)
		dst[i]=src[i];
}

void con_memset(short *dst, int val, int cnt)
{
	int i;
	for(i=0; i<cnt; i++)
		dst[i]=val;
}

void con_strcpy(short *dst, short *src)
{
	short *s, *t;
	int i;
	
	s=src; t=dst;
	while(*s)*t++=*s++;
	*t++=0;
}

int con_strlen(short *src)
{
	int i;
	for(i=0; src[i]; i++);
	return(i);
}

void con_strcpy16to8(char *dst, short *src)
{
	short *s;
	char *t;
	int i;

//	for(i=0; src[i]; i++)
//		dst[i]=src[i];
//	dst[i]=0;

	s=src; t=dst;
	while(*s)
	{
//		i=*((u16 *)s)++;
		i=*((u16 *)s);
		s++;
		BGBDY_EmitChar(&t, i);
	}
	*t++=0;
}

void con_strcpy8to16(short *dst, char *src)
{
	char *s;
	short *t;
	int i;
//	for(i=0; src[i]; i++)
//		dst[i]=src[i];
//	dst[i]=0;

	s=src; t=dst;
	while(*s)
	{
		i=BGBDY_ParseChar(&s);
		*t++=i;
	}
	*t++=0;
}

void Con_ScrollUpInfo(BTGE_ConsoleInfo *con)
{
	int i;
//	for(i=0; i<74; i++)
	for(i=0; i<(con->ys-1); i++)
	{
//		con_memcpy(&con->conbuf[i][0], &con->conbuf[i+1][0], 100);
//		con_memcpy(&con->conclr[i][0], &con->conclr[i+1][0], 100);
		con_memcpy(&con->conbuf[i][0], &con->conbuf[i+1][0], con->xs);
		con_memcpy(&con->conclr[i][0], &con->conclr[i+1][0], con->xs);
	}
//	con_memset(&con->conbuf[74][0], 0, 100);
//	con_memset(&con->conclr[74][0], 0, 100);
	con_memset(&con->conbuf[con->ys-1][0], 0, con->xs);
	con_memset(&con->conclr[con->ys-1][0], 0, con->xs);
}

void Con_HandleAnsi(char **rstr)
	{ Con_HandleAnsiInfo(con_info, rstr); }

void Con_HandleAnsiInfo(BTGE_ConsoleInfo *con, char **rstr)
{
	int iarr[64];
	int i, j, k, ni, cmd;
	char *s;

	s=*rstr; ni=0;
	if(*s=='[')
	{
		s++; i=0;
		while(*s)
		{
			j=*s++;
			if((j>='0') && (j<='9'))
				{ i=(i*10)+(j-'0'); continue; }
			if(j==';')
				{ iarr[ni++]=i; i=0; continue; }
			if((j>='A') && (j<='Z'))
				{ cmd=j; break; }
			if((j>='a') && (j<='z'))
				{ cmd=j; break; }
		}
		
		switch(cmd)
		{
		case 'm':
			if(!ni)
			{
				con->curclr=0x0007;
				break;
			}
			for(i=0; i<ni; i++)
			{
				switch(iarr[i])
				{
				case 0: con->curclr=0x0007; break;
				case 1: con->curclr|=0x0008; break;	//bright
				case 2: con->curclr|=0x1000; break;	//faint

				case 3: con->curclr|=0x0100; break;	//italic
				case 4: con->curclr|=0x0200; break;	//underline
				case 5: con->curclr|=0x0400; break;	//blink (slow)
				case 6: con->curclr|=0x0400; break;	//blink (fast)

				case 9: con->curclr|=0x0800; break;	//crossed-out

				case 22: con->curclr&=~0x1008; break;	//disable bright/faint
				case 23: con->curclr&=~0x0100; break;	//disable italic
				case 24: con->curclr&=~0x0200; break;	//disable underline
				case 25: con->curclr&=~0x0400; break;	//disable blink

				case 29: con->curclr&=~0x0800; break;	//disable cross-out
				
				case 30: case 31: case 32: case 33:
				case 34: case 35: case 36: case 37:
					j=iarr[i]-30;
					con->curclr&=~0x0007;
					con->curclr|=j;
					break;
				case 39:
					con->curclr&=~0x0007;
					con->curclr|=7;
					break;
				case 40: case 41: case 42: case 43:
				case 44: case 45: case 46: case 47:
					j=iarr[i]-40;
					con->curclr&=~0x0070;
					con->curclr|=j<<4;
					break;
				case 49:
					con->curclr&=~0x0070;
					break;
				default:
					break;
				}
			}
			break;
		default:
			break;
		}
		
		*rstr=s;
		return;
	}
	
	s++;
	*rstr=s;
	return;
}

PDGL_API void Con_WriteStringBase(char *str)
	{ Con_WriteString2(con_base, str); }
PDGL_API void Con_WriteString(char *str)
	{ Con_WriteString2(con_info, str); }

PDGL_API void Con_WriteString2(BTGE_ConsoleInfo *con, char *str)
{
//	static int x;
	int i, j, ch;
	char *s;
	
	con->flags|=CON_FLAG_DIRTY;
	
	s=str;
//	x=0;
	while(*s)
	{
		ch=BGBDY_PeekChar(&s);
		if(ch<' ')switch(ch)
		{
		case '\r':
			con->x=0;
			break;
		case '\n':
			Con_ScrollUpInfo(con);
			con->x=0;
			break;
		case '\t':
			if(con->x>(con->xs-16))
			{
				Con_ScrollUpInfo(con);
				con->x=0;
				break;
			}
			j=(con->x+8)&(~7);
			for(i=con->x; i<j; i++)
			{
				con->conbuf[con->ys-1][i]=' ';
				con->conclr[con->ys-1][i]=con->curclr;
			}
			con->x=j;
			break;
		case '\x1B':
			Con_HandleAnsi(&s);
			break;
		default:
			break;
		}else if(ch>=65536)
		{
			j=con->curclr;
			j=(j|CON_EFF_CHARESC)&0xFF0F;
			j|=(ch>>12)&0x00F0;
			con->conbuf[con->ys-1][con->x]=ch&0x00FFFF;
			con->conclr[con->ys-1][con->x]=j;
			con->x++;
		}else
		{
			con->conbuf[con->ys-1][con->x]=ch;
			con->conclr[con->ys-1][con->x]=con->curclr;
			con->x++;
		}
		if(con->x>=con->xs)
		{
			Con_ScrollUpInfo(con);
			con->x=0;
		}
		BGBDY_ParseChar(&s);
//		s++;
	}
}

PDGL_API int Con_HandleKey(int num, int down)
{
	if(!down)return(0);

//	if(((num=='~') || (num=='`')) && PDGL_KeyDown(K_CTRL))
	if(((num=='~') || (num=='`')) && PDGL_KeyDown(K_ALT))
	{
		con_down++;
		con_down%=con_modes+1;
		con_info->flags|=CON_FLAG_DIRTY;
		return(0);
	}

	if(PDGL_KeyDown(K_ALT))
	{
		if((num>=K_F1) && (num<=K_F9))
		{
			//set active console
			con_info=con_context[num-K_F1];
			con_info->flags|=CON_FLAG_DIRTY;
			return(0);
		}

		if((num>='1') && (num<='9'))
		{
			//set active console
			con_info=con_context[num-'1'];
			con_info->flags|=CON_FLAG_DIRTY;
			return(0);
		}
	}

	if(con_info->HandleKey)
		con_info->HandleKey(con_info, num, down);
//	return(Con_HandleKeyInfo(con_info, num, down));
}

PDGL_API int Con_HandleKeyInfo(BTGE_ConsoleInfo *con, int num, int down)
{
	char tbc[256];
	char *s;
	void *p;
	int i;

	if(!down)return(0);

	if(con_down)
	{
		con->flags|=CON_FLAG_DIRTY;

		if(num<' ')switch(num)
		{
		case '\r':
			if(con_handler[con_down])
			{
				con_strcpy16to8(tbc, con->promptbuf);
				con_handler[con_down](tbc);
//				con_handler[con_down](con->promptbuf);
			}

			con_memcpy(con->histbuf[0], con->promptbuf, con->xs);
			for(i=15; i>0; i--)
				con_memcpy(con->histbuf[i], con->histbuf[i-1], con->xs);
//			memcpy(con->histbuf[0], con->promptbuf, 100);
			con->histpos=0;

			con->promptpos=0;
			con_memset(con->promptbuf, 0, con->xs);
			con_memset(con->completebuf, 0, con->xs);
			break;

		case '\t':
			for(i=con->promptpos; i<con->xs; i++)
				con->promptbuf[i]=con->completebuf[i];
			con->promptpos=con_strlen(con->promptbuf);
			break;

		case '\b':
			con->histpos=0;
			if(con->promptpos>0)
			{
				con->promptpos--;
				for(i=con->promptpos; con->promptbuf[i]; i++)
					con->promptbuf[i]=con->promptbuf[i+1];
			}

			con_memset(con->completebuf, 0, con->xs);
			if(con_complete[con_down])
			{
				con_strcpy16to8(tbc, con->promptbuf);
				s=con_complete[con_down](tbc);
				if(s)con_strcpy8to16(con->completebuf, s);

//				s=con_complete[con_down](con->promptbuf);
//				if(s)con_strcpy(con->completebuf, s);
			}
			break;
		default:
			break;
		}else if(num<127)
		{
			con->histpos=0;
			for(i=con->promptpos; con->promptbuf[i]; i++);
			for(; i>con->promptpos; i--)
				con->promptbuf[i]=con->promptbuf[i-1];
			con->promptbuf[con->promptpos++]=num;

			con_memset(con->completebuf, 0, con->xs);
			if(con_complete[con_down])
			{
				con_strcpy16to8(tbc, con->promptbuf);
				s=con_complete[con_down](tbc);
				if(s)con_strcpy8to16(con->completebuf, s);

//				s=con_complete[con_down](con->promptbuf);
//				if(s)con_strcpy(con->completebuf, s);
			}
		}else switch(num)
		{
		case K_LEFTARROW:
			con->histpos=0;
			con->promptpos--;
			if(con->promptpos<0)con->promptpos=0;
			break;
		case K_RIGHTARROW:
			con->histpos=0;
			if(con->promptbuf[con->promptpos])
				con->promptpos++;
			break;

		case K_UPARROW:
			if(!con->histpos)
				con_memcpy(con->histbuf[con->histpos],
					con->promptbuf, con->xs);

			con->histpos++;
			if(con->histpos>15)con->histpos=15;
			con_memcpy(con->promptbuf,
				con->histbuf[con->histpos], con->xs);
			con->promptpos=con_strlen(con->promptbuf);
			break;
		case K_DOWNARROW:
			con->histpos--;
			if(con->histpos<0)con->histpos=0;
			con_memcpy(con->promptbuf,
				con->histbuf[con->histpos], con->xs);
			con->promptpos=con_strlen(con->promptbuf);
			break;

		case K_HOME:
			con->histpos=0;
			con->promptpos=0;
			break;
		case K_END:
			con->histpos=0;
			con->promptpos=con_strlen(con->promptbuf);
			break;

		default:
			break;
		}
	}
	return(0);
}

PDGL_API int Con_KeyDown(BTGE_ConsoleInfo *con, int key)
	{ return(PDGL_KeyDown(key)); }

PDGL_API int Con_Printf(char *s, ...)
{
	va_list lst;
	char *buf;

	buf=malloc(4096);

	va_start(lst, s);
	vsprintf(buf, s, lst);
	Con_WriteString(buf);
	va_end(lst);

	free(buf);

	return(0);
}
