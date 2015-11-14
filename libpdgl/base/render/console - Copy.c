#include <pdgl.h>

#include <time.h>
#include <stdarg.h>

int con_down=0;

static char conbuf[75][100];
static short conclr[75][100];
static char promptbuf[100], promptpos;
static char histbuf[16][100], histpos;
static char completebuf[100];

static int (*con_handler[16])(char *s);
static char *(*con_complete[16])(char *s);
static char con_mchar[16];
static int con_modes=0;

static int con_curclr=0x0007;

static char *con_stuffcmd[1024];
static int con_nstuffcmd=0;

BTGE_ConsoleInfo *con_info;		//current active console

PDGL_API int GfxFont_SetFont(char *name, int mode);
PDGL_API int GfxFont_DrawChar(int c, int x, int y, int w, int h,
	int r, int g, int b, int a);

PDGL_API int Con_HandleKey(int num, int down);

PDGL_API int Con_Update(float dt)
{
	int i;

	if(con_nstuffcmd>0)
	{
//		ConCmds_Handler(con_stuffcmd[0]);
//		for(i=0; i<con_nstuffcmd; i++)
//			con_stuffcmd[i]=con_stuffcmd[i+1];
		con_nstuffcmd--;
		ConCmds_Handler(con_stuffcmd[con_nstuffcmd]);
	}
	return(0);
}

PDGL_API int Con_StuffCmd(char *str)
{
	con_stuffcmd[con_nstuffcmd++]=
		dystrdup(str);
	return(0);
}

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
	int i;
	for(i=0; i<75; i++)
	{
		memset(&conbuf[i][0], 0, 100);
		memset(&conclr[i][0], 0, 100*sizeof(short));
	}
	return(0);
}

PDGL_API int Con_Init()
{
	PDGL_AddKeyHandler(&Con_HandleKey);
	gcAddLogHook(Con_WriteString);

	con_info=BTGE_Console_AllocConsoleInfo();
	con_info->WriteString=Con_WriteString2;

	con_modes=0;
	return(0);
}

PDGL_API int Con_RenderBackground()
{
	int x, y;
	int i, j, k, l;

	glBegin(GL_QUADS); l=-1;
	for(i=0; i<(con_down?75:15); i++)
	{
		for(j=0; j<100; j++)
		{
//			if(!conbuf[74-i][j])break;
			k=conclr[74-i][j];

			if(k!=l)
			{
				glColor4f(
					((k&16)?0.75:0)+((k&128)?0.25:0),
					((k&32)?0.75:0)+((k&128)?0.25:0),
					((k&64)?0.75:0)+((k&128)?0.25:0),
					0.75);
				l=k;
			}
//			Draw_Square((j*8)-400, (i*8)-300, 8, 8);

			x=(j*8)-400;
			y=(i*8)-300;
//			glBegin(GL_QUADS);
			glVertex2f(x,	y);
			glVertex2f(x,	y+8);
			glVertex2f(x+8,	y+8);
			glVertex2f(x+8,	y);
//			glEnd();
		}
	}
	glEnd();
}

PDGL_API int Con_Render()
{
	int cr, cg, cb, ca, cm;
	int i, j, k;

//	if(!con_down)return(0);

	GfxFont_SetFont("fixed", 0);

	glDisable(GL_ALPHA_TEST);
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
	for(i=0; i<(con_down?75:15); i++)
	{
		for(j=0; j<100; j++)
		{
			if(!conbuf[74-i][j])break;
			k=conclr[74-i][j];

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
			ca=(k&0x1000)?192:255;	//faint
			cm=0;
			
			if(k&0x100)cm|=PDGL_FONT_ITALIC;
			if(k&0x200)cm|=PDGL_FONT_UNDERLINE;
			if(k&0x400)cm|=PDGL_FONT_BLINK;
			if(k&0x800)cm|=PDGL_FONT_STRIKEOUT;
			
//			GfxFont_DrawCharMode(conbuf[74-i][j],
//				(j*8)-400, (i*8)-300, 8, 8,
//				cr, cg, cb, ca, cm);

			GfxFont_DrawCharModeQI(conbuf[74-i][j],
				(j*8)-400, (i*8)-300, 8, 8,
				cr, cg, cb, ca, cm);
		}
	}
	GfxFont_DrawCharModeQI(-2, 0,0,0,0, 0,0,0,0, 0);

//	Draw_Character(-1000, -750, 25, 25, '~');
	if(con_down)
		GfxFont_DrawChar(con_mchar[con_down],
			-400, -300, 8, 8, 255, 255, 255, 255);
		else GfxFont_DrawChar('X', -400, -300, 8, 8, 255, 0, 0, 255);

	j=strlen(promptbuf);
	for(i=j; i<80; i++)
		GfxFont_DrawChar(completebuf[i],
			-400+16+(i*8), -300, 8, 8,
			127, 127, 127, 255);

	for(i=0; i<80; i++)
		GfxFont_DrawChar(promptbuf[i],
			-400+16+(i*8), -300, 8, 8,
			255, 255, 255, 255);

//	for(i=0; i<80; i++)
//		Draw_Character(i*25-1000+25, -750, 25, 25, promptbuf[i]);

	i=(clock()*1000)/CLOCKS_PER_SEC;
	i=(i/250)&1;
	j=promptpos;

	if(i)GfxFont_DrawChar('_',
		-400+16+(j*8), -300, 8, 8,
		255, 255, 255, 255);

//	if(i)Draw_Character(-1000+(promptpos+1)*25, -750, 25, 25, '_');

	return(0);
}

void cb_scrollup()
{
	int i;
	for(i=0; i<74; i++)
	{
		memcpy(&conbuf[i][0], &conbuf[i+1][0], 100);
		memcpy(&conclr[i][0], &conclr[i+1][0], 100*sizeof(short));
	}
	memset(&conbuf[74][0], 0, 100);
	memset(&conclr[74][0], 0, 100*sizeof(short));
}

void Con_HandleAnsi(char **rstr)
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
				con_curclr=0x0007;
				break;
			}
			for(i=0; i<ni; i++)
			{
				switch(iarr[i])
				{
				case 0: con_curclr=0x0007; break;
				case 1: con_curclr|=0x0008; break;	//bright
				case 2: con_curclr|=0x1000; break;	//faint

				case 3: con_curclr|=0x0100; break;	//italic
				case 4: con_curclr|=0x0200; break;	//underline
				case 5: con_curclr|=0x0400; break;	//blink (slow)
				case 6: con_curclr|=0x0400; break;	//blink (fast)

				case 9: con_curclr|=0x0800; break;	//crossed-out

				case 22: con_curclr&=~0x1008; break;	//disable bright/faint
				case 23: con_curclr&=~0x0100; break;	//disable italic
				case 24: con_curclr&=~0x0200; break;	//disable underline
				case 25: con_curclr&=~0x0400; break;	//disable blink

				case 29: con_curclr&=~0x0800; break;	//disable cross-out
				
				case 30: case 31: case 32: case 33:
				case 34: case 35: case 36: case 37:
					j=iarr[i]-30;
					con_curclr&=~0x0007;
					con_curclr|=j;
					break;
				case 39:
					con_curclr&=~0x0007;
					con_curclr|=7;
					break;
				case 40: case 41: case 42: case 43:
				case 44: case 45: case 46: case 47:
					j=iarr[i]-40;
					con_curclr&=~0x0070;
					con_curclr|=j<<4;
					break;
				case 49:
					con_curclr&=~0x0070;
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

PDGL_API void Con_WriteString2(BTGE_ConsoleInfo *con, char *str)
{
	Con_WriteString(str);
}

PDGL_API void Con_WriteString(char *s)
{
	static int x;
	int i, j;
//	x=0;
	while(*s)
	{
		if(*s<' ')switch(*s)
		{
		case '\r':
			x=0;
			break;
		case '\n':
			cb_scrollup();
			x=0;
			break;
		case '\t':
			if(x>(100-16))
			{
				cb_scrollup();
				x=0;
				break;
			}
			j=(x+8)&(~7);
			for(i=x; i<j; i++)
			{
				conbuf[74][i]=' ';
				conclr[74][i]=con_curclr;
			}
			x=j;
			break;
		case '\x1B':
			Con_HandleAnsi(&s);
			break;
		default:
			break;
		}else
		{
			conbuf[74][x]=*s;
			conclr[74][x]=con_curclr;
			x++;
		}
		if(x>=100)
		{
			cb_scrollup();
			x=0;
		}
		s++;
	}
}

PDGL_API int Con_HandleKey(int num, int down)
{
	char *s;
	void *p;
	int i;

	if(!down)return(0);

//	if(((num=='~') || (num=='`')) && PDGL_KeyDown(K_CTRL))
	if(((num=='~') || (num=='`')) && PDGL_KeyDown(K_ALT))
	{
		con_down++;
		con_down%=con_modes+1;
		return(0);
	}

	if(con_down)
	{
		if(num<' ')switch(num)
		{
		case '\r':
			if(con_handler[con_down])
				con_handler[con_down](promptbuf);

			memcpy(histbuf[0], promptbuf, 100);
			for(i=15; i>0; i--)
				memcpy(histbuf[i], histbuf[i-1], 100);
			histpos=0;

			promptpos=0;
			memset(promptbuf, 0, 100);
			memset(completebuf, 0, 100);
			break;

		case '\t':
			for(i=promptpos; i<100; i++)
				promptbuf[i]=completebuf[i];
			promptpos=strlen(promptbuf);
			break;

		case '\b':
			histpos=0;
			if(promptpos>0)
			{
				promptpos--;
				for(i=promptpos; promptbuf[i]; i++)
					promptbuf[i]=promptbuf[i+1];
			}

			memset(completebuf, 0, 100);
			if(con_complete[con_down])
			{
				s=con_complete[con_down](promptbuf);
				if(s)strcpy(completebuf, s);
			}
			break;
		default:
			break;
		}else if(num<127)
		{
			histpos=0;
			for(i=promptpos; promptbuf[i]; i++);
			for(; i>promptpos; i--)
				promptbuf[i]=promptbuf[i-1];
			promptbuf[promptpos++]=num;

			memset(completebuf, 0, 100);
			if(con_complete[con_down])
			{
				s=con_complete[con_down](promptbuf);
				if(s)strcpy(completebuf, s);
			}
		}else switch(num)
		{
		case K_LEFTARROW:
			histpos=0;
			promptpos--;
			if(promptpos<0)promptpos=0;
			break;
		case K_RIGHTARROW:
			histpos=0;
			if(promptbuf[promptpos])
				promptpos++;
			break;

		case K_UPARROW:
			if(!histpos)
				memcpy(histbuf[histpos], promptbuf, 100);

			histpos++;
			if(histpos>15)histpos=15;
			memcpy(promptbuf, histbuf[histpos], 100);
			promptpos=strlen(promptbuf);
			break;
		case K_DOWNARROW:
			histpos--;
			if(histpos<0)histpos=0;
			memcpy(promptbuf, histbuf[histpos], 100);
			promptpos=strlen(promptbuf);
			break;

		case K_HOME:
			histpos=0;
			promptpos=0;
			break;
		case K_END:
			histpos=0;
			promptpos=strlen(promptbuf);
			break;

		default:
			break;
		}
	}
	return(0);
}

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
