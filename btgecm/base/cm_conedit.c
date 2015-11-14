/**
 * Console Based Editor
 */

#include <btgecm.h>

#ifndef MIN
#define MIN(x, y) (((x)<(y))?(x):(y))
#endif

#ifndef MAX
#define MAX(x, y) (((x)>(y))?(x):(y))
#endif

BTGE_ConEdit *btge_conedit_modroot;
BTGE_ConEdit *btge_conedit_curmod;

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
	u16 *s;
	char *t;
	int i;

//	for(i=0; src[i]; i++)
//		dst[i]=src[i];
//	dst[i]=0;

	s=(u16 *)src; t=dst;
	while(*s)
	{
		i=*s++;
		if(i=='\x1F')	//editor internal char
			continue;
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

short *con_strdup(short *src)
{
	short *dst;
	int i;
	
	i=con_strlen(src)+1;
	dst=gcalloc(i*sizeof(short));
	
	for(i=0; src[i]; i++)
		dst[i]=src[i];
	dst[i]=0;
	return(dst);
}

short *con_strdup8(char *src)
{
	short *dst;
	int i;
	
	i=strlen(src)+1;
	dst=gcalloc(i*sizeof(short));
	con_strcpy8to16(dst, src);
//	for(i=0; src[i]; i++)
//		dst[i]=src[i];
//	dst[i]=0;
	return(dst);
}

void con_strcat(short *dst, short *src)
{
	short *s, *t;
	int i;
	
	s=src; t=dst;
	while(*t)t++;
	while(*s)*t++=*s++;
	*t++=0;
}

BTGE_API BTGE_ConsoleInfo *BTGE_Console_AllocConsoleInfo()
{
	BTGE_ConsoleInfo *tmp;
	tmp=gctalloc("btge_consoleinfo_t", sizeof(BTGE_ConsoleInfo));
	return(tmp);
}

BTGE_API void BTGE_Console_WriteString(BTGE_ConsoleInfo *con, char *str)
{
	if(con->WriteString)
		con->WriteString(con, str);
}

BTGE_API int BTGE_Console_Printf(BTGE_ConsoleInfo *con, char *s, ...)
{
	va_list lst;
	char *buf;

	buf=malloc(4096);

	va_start(lst, s);
	vsprintf(buf, s, lst);
	BTGE_Console_WriteString(con, buf);
	va_end(lst);

	free(buf);

	return(0);
}

BTGE_API int BTGE_Console_KeyDown(BTGE_ConsoleInfo *con, int key)
{
	if(con->KeyDown)
		return(con->KeyDown(con, key));
	return(0);
}

BTGE_ConEdit *BTGE_ConEdit_LookupModule(char *name)
{
	BTGE_ConEdit *cur;
	
	cur=btge_conedit_modroot;
	while(cur)
	{
		if(!strcmp(cur->name, name))
			return(cur);
		cur=cur->next;
	}
	
	return(NULL);
}

BTGE_ConEdit *BTGE_ConEdit_GetModule(char *name)
{
	BTGE_ConEdit *cur;

	cur=BTGE_ConEdit_LookupModule(name);
	if(cur)return(cur);
	
	cur=gctalloc("btge_conedit_t", sizeof(BTGE_ConEdit));
	cur->name=dystrdup(name);
	cur->next=btge_conedit_modroot;
	btge_conedit_modroot=cur;
	
	return(cur);
}

BTGE_ConEdit *BTGE_ConEdit_SetActiveModule(
	BTGE_ConsoleInfo *con, char *name)
{
	BTGE_ConEdit *edit;
	edit=BTGE_ConEdit_GetModule(name);
	btge_conedit_curmod=edit;
	con->edit=edit;
	return(edit);
}

/**
  * Line Editing Stuff
  */

void BTGE_ConEdit_CheckEditLine(BTGE_ConEdit *edit, int idx)
{
	int i, j;

	if(!edit->linebuf)
	{
		edit->linebuf=gcalloc(256*sizeof(short *));
		edit->n_lines=0;
		edit->m_lines=256;
	}

	if(idx>=edit->m_lines)
	{
		j=edit->m_lines;
		while(j<=idx)j=j+(j>>1);
		edit->linebuf=gcrealloc(edit->linebuf, j*sizeof(char *));
		edit->m_lines=j;
	}
}

void BTGE_ConEdit_LineInsert(BTGE_ConEdit *edit, int i, int j)
{
	int k;

	k=edit->n_lines+j;
	BTGE_ConEdit_CheckEditLine(edit, k);
	while(j>0)
	{
		for(k=edit->n_lines; k>i; k--)
			edit->linebuf[k]=edit->linebuf[k-1];
		edit->n_lines++;
		edit->linebuf[i]=NULL;
		j--;
	}
}

void BTGE_ConEdit_LineDelete(BTGE_ConEdit *edit, int i, int j)
{
	int k;

	k=edit->n_lines+j;
	BTGE_ConEdit_CheckEditLine(edit, k);
	while(j>0)
	{
		for(k=i; k<edit->n_lines; k++)
			edit->linebuf[k]=edit->linebuf[k+1];
		edit->n_lines--;
		edit->linebuf[k]=NULL;
		j--;
	}
}

void BTGE_ConEdit_LineUpdate8(BTGE_ConEdit *edit, int i, char *str)
{
	short tb[256], tb2[256];

	con_strcpy8to16(tb2, str);
	BTGE_ConEdit_AdjustLineTabs(NULL, tb, tb2);
	edit->linebuf[i]=con_strdup(tb);

//	edit->linebuf[i]=con_strdup8(str);
	if(i>=edit->n_lines)
		edit->n_lines=i+1;
//	BTGE_Console_Printf(con, "Set Line %d: '%s'\n", i, str);
}

void BTGE_ConEdit_LineEval(BTGE_ConEdit *edit, int i, int j)
{
//	char tb[4096];
//	char tb[65536];
//	char tb[262144];
	char *tb;
	u16 *s;
	char *t;
	dyt p;
	int k;

	tb=malloc(262144);

	t=tb;
	for(k=i; k<=j; k++)
	{
		s=edit->linebuf[k];
		if(!s)s=L"";
		while(*s)*t++=*s++;
		*t++='\n';
//		sprintf(t, "%s\n", s);
//		t+=strlen(t);
	}
	*t=0;

//	printf("Eval: %s\n", tb);
	p=BSVM_EvalString(tb);
//	dyPrint(p);
//	printf("\n");

	free(tb);
}

void BTGE_ConEdit_LoadFile(BTGE_ConsoleInfo *con, char *name)
{
	char tb[256];
	byte *buf;
	char *s, *fn;
	int i, sz;
	
	fn=name;
	if(!fn)fn=con->edit->filename;
	if(!fn)
	{
		BTGE_Console_Printf(con, "No filename given\n");
		return;
	}
	buf=vf_loadfile(fn, &sz);
	if(!fn)
	{
		BTGE_Console_Printf(con, "Load failed\n");
		return;
	}

	con->edit->filename=dystrdup(fn);
	con->edit->n_lines=0;
	
	s=(char *)buf; i=1;
	while(*s)
	{
		s=gcrsgets(tb, 255, s);
		BTGE_ConEdit_LineUpdate8(con->edit, i, tb);
		i++;
	}
	
	gcfree(buf);
}

void BTGE_ConEdit_SaveFile(BTGE_ConsoleInfo *con, char *name)
{
	char tb[256], tb2[256];
	short *s2;
	char *s, *buf;
	char *t, *fn;
	int k;

	buf=gcalloc(65536);
	t=buf;
	for(k=1; k<con->edit->n_lines; k++)
	{
		memset(tb2, 0, 256);
		s2=con->edit->linebuf[k];
		con_strcpy16to8(tb2, s2);
		s=tb2;
		while(*s)*t++=*s++;
		*t++='\r';
		*t++='\n';
	}
	*t=0;

	fn=name;
	if(!fn)fn=con->edit->filename;
	if(!fn)
	{
		BTGE_Console_Printf(con, "No filename given\n");
		return;
	}
	
	con->edit->filename=dystrdup(fn);
	
	vf_storefile(fn, buf, strlen(buf));
	gcfree(buf);
}

BTGE_API int BTGE_ConEdit_Handler(BTGE_ConsoleInfo *con, char *str)
{
	char tb[1024];
	BTGE_ConEdit *edit;
	char **a;
	char *s, *t;
	int i, j, k;

	if(!con)return(-1);

	if(!str || !*str)
		return(0);
	if(*str!='*')
		return(-1);

	a=gcrsplit(str);
	if(!a[0])return(0);
	
	if(!strcmp(a[0], "*module"))
	{
		edit=BTGE_ConEdit_GetModule(a[1]);
		btge_conedit_curmod=edit;
		con->edit=edit;
		return(0);
	}

	if(!strcmp(a[0], "*help"))
	{
		BTGE_Console_Printf(con,
		"  *help                    This message\n"
		"  *module <name>           Set the current module\n"
		"  *clear                   Clear contents of module\n"
		"  *insert <line> [count]   Insert blank line(s)\n"
		"  *delete <line> [count]   Delete line(s)\n"
		"  *list [start [end]]      Print line contents\n"
		"  *<line> <text...>        Replace line contents with text\n"
		"  *eval [start [end]]      Evaluate module contents\n"
		"  *load <name>             Load module contents from file\n"
		"  *save <name>             Save module contents to file\n"
		"  *start_edit              Start Text Editor\n"
		);
		return(0);
	}
	
	edit=btge_conedit_curmod;
	if(!edit)
	{
		edit=BTGE_ConEdit_GetModule("A");
		btge_conedit_curmod=edit;
		con->edit=edit;

//		BTGE_Console_Printf(con,
//			"Editor commands need an active module\n");
//		return(0);
	}
	
	if(!edit->linebuf)
	{
		edit->linebuf=gcalloc(256*sizeof(char *));
		edit->n_lines=0;
		edit->m_lines=256;
	}

	if(!strcmp(a[0], "*clear"))
	{
		edit->n_lines=0;
		return(0);
	}

	if(!strcmp(a[0], "*load"))
	{
		BTGE_ConEdit_LoadFile(con, a[1]);
		return(0);
	}

	if(!strcmp(a[0], "*save"))
	{
		BTGE_ConEdit_SaveFile(con, a[1]);
		return(0);
	}

	if(!strcmp(a[0], "*insert"))
	{
		i=atoi(a[1]);
		if(a[2])j=atoi(a[2]);
			else j=1;
		BTGE_ConEdit_LineInsert(edit, i, j);
		return(0);
	}

	if(!strcmp(a[0], "*delete"))
	{
		i=atoi(a[1]);
		if(a[2])j=atoi(a[2]);
			else j=1;
		BTGE_ConEdit_LineDelete(edit, i, j);
		return(0);
	}

	if(!strcmp(a[0], "*list"))
	{
		i=1; j=edit->n_lines-1;
		if(a[1])
		{
			i=atoi(a[1]);
			if(a[2])j=atoi(a[2]);
		}
		
		for(k=i; k<=j; k++)
		{
//			s=edit->linebuf[k];
//			if(!s)s="";
//			BTGE_Console_Printf(con,
//				"  %d: %s\n", k, s);

			con_strcpy16to8(tb, edit->linebuf[k]);
			BTGE_Console_Printf(con,
				"  %d: %s\n", k, tb);
		}
		return(0);
	}

	if(!strcmp(a[0], "*eval"))
	{
		i=1; j=edit->n_lines-1;
		if(a[1])
		{
			i=atoi(a[1]);
			if(a[2])j=atoi(a[2]);
		}

		BTGE_ConEdit_LineEval(edit, i, j);

		return(0);
	}

	if((str[1]>='0') && (str[1]<='9'))
	{
		i=atoi(str+1);
		s=str+1;
		while(*s && (*s>' '))s++;
		while(*s && (*s<=' '))s++;

		BTGE_ConEdit_CheckEditLine(edit, i);
		BTGE_ConEdit_LineUpdate8(edit, i, s);
		return(0);
	}

	if(!strcmp(a[0], "*start_edit"))
	{
		BTGE_ConEdit_InitUI(con);
		return(0);
	}

	BTGE_Console_Printf(con,
		"Bad Editor Command %s\n", a[0]);
	return(-1);
}


/**
  *	Interactive Editing Stuff
  */

static short *btge_conedit_clipboard=NULL;

BTGE_API short *BTGE_ConEdit_GetClipboardString16(
	BTGE_ConsoleInfo *con)
{
	return(btge_conedit_clipboard);
}

BTGE_API void BTGE_ConEdit_SetClipboardString16(
	BTGE_ConsoleInfo *con, short *str)
{
	btge_conedit_clipboard=str;
}

BTGE_API short *BTGE_ConEdit_GetSelectionString16(
	BTGE_ConsoleInfo *con)
{
	short *buf, *s, *t, *cs, *ce;
	int sx, sy, ex, ey, tx, ty;
	int i, j, k, l;
	
	sx=con->edit->sel_sx;
	sy=con->edit->sel_sy;
	ex=con->edit->sel_ex;
	ey=con->edit->sel_ey;

	if((sx==ex) && (sy==ey))
		return(NULL);

	if(sy!=ey)
	{
		if(ey<sy) { tx=sx; sx=ex; ex=tx; ty=sy; sy=ey; ey=ty; }

		buf=gcalloc(4096*sizeof(short));

		t=buf;
		for(i=sy; i<=ey; i++)
		{
			s=con->edit->linebuf[i+1];
			if(!s) { *t++='\n'; continue; }
			cs=s; ce=s+con_strlen(s);
			if(i==sy)cs=s+sx;
			if(i==ey)ce=s+ex;
			while(*cs && (cs<ce))
				*t++=*cs++;
			if(!(*cs))*t++='\n';
		}
		
		s=con_strdup(buf);
		gcfree(buf);
		return(s);
	}else
	{
		if(ex<sx) { tx=sx; sx=ex; ex=tx; ty=sy; sy=ey; ey=ty; }

		s=con->edit->linebuf[sy+1];
		if(!s)return(NULL);
		
		l=ex-sx;
		buf=gcalloc((l+1)*sizeof(short));
		for(i=0; i<l; i++)
			buf[i]=s[sx+i];
		buf[i]=0;
		return(buf);
	}
}

BTGE_API int BTGE_ConEdit_RedrawCheckSel(
	BTGE_ConsoleInfo *con, int x, int y)
{
	int mx, my, nx, ny;
	
	mx=MIN(con->edit->sel_sx, con->edit->sel_ex);
	my=MIN(con->edit->sel_sy, con->edit->sel_ey);
	nx=MAX(con->edit->sel_sx, con->edit->sel_ex);
	ny=MAX(con->edit->sel_sy, con->edit->sel_ey);
	
	if((mx==nx) && (my==ny))
		return(0);
	
	if((x<mx) && (y==my))return(0);
	if((x>nx) && (y==ny))return(0);
	if(y<my)return(0);
	if(y>ny)return(0);
	return(1);
}

BTGE_API int BTGE_ConEdit_RedrawUI(BTGE_ConsoleInfo *con)
{
	char tb[256];
	BTGE_ConEdit *cur;
	char *s, *s0, *s1;
	short *t;
	int x, y, xo, yo;
	int i, j, k;

	for(i=0; i<60; i++)
	{
		for(j=0; j<100; j++)
		{
			con->conbuf[i][j]=0;
			con->conclr[i][j]=0x0007;
		}
	}
	
	xo=1; yo=7;

	for(i=0; i<50; i++)
	{
		con->conbuf[i+yo][-1+xo]='|';
		con->conclr[i+yo][-1+xo]=0x0047;
		con->conbuf[i+yo][80+xo]='|';
		con->conclr[i+yo][80+xo]=0x0047;
	}

	for(j=0; j<80; j++)
	{
		con->conbuf[-1+yo][j+xo]='-';
		con->conclr[-1+yo][j+xo]=0x0047;
		con->conbuf[50+yo][j+xo]='-';
		con->conclr[50+yo][j+xo]=0x0047;
	}
	con->conbuf[-1+yo][-1+xo]='+';
	con->conclr[-1+yo][-1+xo]=0x0047;
	con->conbuf[-1+yo][80+xo]='+';
	con->conclr[-1+yo][80+xo]=0x0047;
	con->conbuf[50+yo][-1+xo]='+';
	con->conclr[50+yo][-1+xo]=0x0047;
	con->conbuf[50+yo][80+xo]='+';
	con->conclr[50+yo][80+xo]=0x0047;

	if(!con->edit)
		return(0);

	for(i=0; i<50; i++)
	{
		for(j=0; j<80; j++)
		{
			k=0x0047;
			
			x=con->edit->win_x+j;
			y=con->edit->win_y+i;
			if(BTGE_ConEdit_RedrawCheckSel(con, x, y))
				k=0x00E7;
		
			con->conbuf[i+yo][j+xo]=0;
			con->conclr[i+yo][j+xo]=k;
		}
	}

	for(i=0; i<50; i++)
	{
		for(j=0; j<80; j++)
		{
			x=con->edit->win_x+j;
			y=con->edit->win_y+i+1;
			
			t=NULL;
			if((y>0) && (y<con->edit->n_lines))
				t=con->edit->linebuf[y];
			
			if(t)
			{
				while(*t && (x>0)) { t++; x--; }
				con->conbuf[i+yo][j+xo]=*t;
			}
		}
	}

	x=con->edit->cur_x-con->edit->win_x;
	y=con->edit->cur_y-con->edit->win_y;
	
	if((x>=0) && (y>=0) && (x<80) && (y<50))
	{
		k=0x0074;
		if(con->edit->flags&1)
			k=0x0017;
		con->conclr[y+yo][x+xo]=k;		
	}


	sprintf(tb, "Line=%d/%d, Col=%d    ",
		con->edit->cur_y+1, con->edit->n_lines,
		con->edit->cur_x+1);

	s=tb; i=50; j=0;
	while(*s)
	{
		con->conclr[i+yo][j+xo]=0x00C7;
		con->conbuf[i+yo][j+xo]=*s++;
		j++;
	}

	cur=btge_conedit_modroot; i=0;
	while(cur)
	{
		s0=cur->name;
		s1=cur->filename;
		if(!s0) { cur=cur->next; continue; }
		if(!s1) s1="";
		
		s=s1+strlen(s1);
		while((s>s1) && (*(s-1)!='/') && (*(s-1)!='\\'))s--;
		s1=s;

		sprintf(tb, "%s:%s", s0, s1);
		if((81+xo+strlen(tb))>100)
		{
			k=100-3-(81+xo);
			strcpy(tb+k, "...");
		}

		k=(cur==con->edit)?0x00C7:0x0047;
		s=tb; j=81;
		while(*s)
		{
			con->conclr[i+yo][j+xo]=k;
			con->conbuf[i+yo][j+xo]=*s++;
			j++;
		}

		i++;
		cur=cur->next;
	}

	return(0);
}

void BTGE_ConEditUI_WriteString(BTGE_ConsoleInfo *con, char *str)
{
}

int BTGE_ConEditUI_Update(BTGE_ConsoleInfo *con, float dt)
{
	BTGE_ConEdit_RedrawUI(con);
	return(0);
}

short *BTGE_ConEdit_GetLine(BTGE_ConsoleInfo *con, int i)
{
	u16 *s;
	BTGE_ConEdit_CheckEditLine(con->edit, i);
	s=con->edit->linebuf[i];
	if(!s)s=L"";
	return(s);
}

void BTGE_ConEdit_UpdateLine(BTGE_ConsoleInfo *con, int i, short *str)
{
	if(con->edit->linebuf[i])
		gcfree(con->edit->linebuf[i]);
	con->edit->linebuf[i]=con_strdup(str);
	if(i>=con->edit->n_lines)
		con->edit->n_lines=i+1;
//	BTGE_Console_Printf(con, "Set Line %d: '%s'\n", i, str);
}

void BTGE_ConEditUI_AdjustEditWindow(BTGE_ConsoleInfo *con)
{
	if(con->edit->cur_y<con->edit->win_y)
		{ con->edit->win_y=con->edit->cur_y; }
	if(con->edit->cur_x<con->edit->win_x)
		{ con->edit->win_x=con->edit->cur_x; }
	while(con->edit->cur_y>=(con->edit->win_y+50))
		{ con->edit->win_y++; }
	while(con->edit->cur_x>=(con->edit->win_x+80))
		{ con->edit->win_x++; }
}

#if 1
/** Adjust line to make sure tabs are correct */
void BTGE_ConEdit_AdjustLineTabs(BTGE_ConsoleInfo *con,
	short *dst, short *src)
{
	short *s, *t;
	int i, j, k, ni;
	
	s=src; t=dst; ni=0;
	while(*s)
	{
		//real tab character
		if(*s=='\t')
		{
			s++;
			i=s-src;
			j=t-dst;
//			k=3-(i&3);
//			while(k-- && (*s=='\x1F'))s++;			

			while(*s && (*s=='\x1F'))s++;			

			*t++='\t';
			k=3-(j&3);
			while(k--)*t++='\x1F';
			continue;
		}
		//lingering tab spacer
		if(*s=='\x1F')
		{
			s++;
			continue;
		}

		*t++=*s++;
	}
	*t++=0;
}

/** Adjust line tabs following a deletion */
void BTGE_ConEdit_AdjustLineTabsDel(BTGE_ConsoleInfo *con,
	short *dst, short *src)
{
	short *s, *t;
	int i, j, k, ni;
	
	s=src; t=dst; ni=0;
	while(*s)
	{
		if(*s=='\t')
		{
			s++;
			i=s-src;
			j=t-dst;
			k=3-((i+ni)&3);
			while(k-- && (*s=='\x1F'))s++;
			if(k<0)
			{
				*t++='\t';
				k=3-(j&3);
				while(k--)*t++='\x1F';
			}else
			{
				ni+=k+1;
			}
			continue;
		}
		//lingering tab spacer
		if(*s=='\x1F')
		{
			s++;
			continue;
		}
		*t++=*s++;
	}
	*t++=0;
}
#endif

/** Align Cursor to be on tab character */
void BTGE_ConEdit_AdjustCursorTabAlign(BTGE_ConsoleInfo *con)
{
	short *s;
	int i;

	i=con->edit->cur_y+1;
	s=BTGE_ConEdit_GetLine(con, i);

	if(con->edit->cur_x>con_strlen(s))
		con->edit->cur_x=con_strlen(s);
	while((con->edit->cur_x>0) &&
		(s[con->edit->cur_x]=='\x1F'))
			con->edit->cur_x--;
}

/** Adjust cursor to be 1 char after tab character */
void BTGE_ConEdit_AdjustCursorTabAlignB(BTGE_ConsoleInfo *con)
{
	short *s;
	int i;

	i=con->edit->cur_y+1;
	s=BTGE_ConEdit_GetLine(con, i);

	if(con->edit->cur_x>con_strlen(s))
		con->edit->cur_x=con_strlen(s);
	while((con->edit->cur_x>0) &&
		(s[con->edit->cur_x-1]=='\x1F'))
			con->edit->cur_x--;
}

/** Align Cursor to be on character following a tab */
void BTGE_ConEdit_AdjustCursorTabAlignAfter(BTGE_ConsoleInfo *con)
{
	short *s;
	int i;

	i=con->edit->cur_y+1;
	s=BTGE_ConEdit_GetLine(con, i);

	if(con->edit->cur_x>con_strlen(s))
		con->edit->cur_x=con_strlen(s);
	while((s[con->edit->cur_x]=='\x1F'))
			con->edit->cur_x++;
}

void BTGE_ConEdit_AdjustCursorLineEnd(BTGE_ConsoleInfo *con)
{
	short *s;
	int i;

	i=con->edit->cur_y+1;
	s=BTGE_ConEdit_GetLine(con, i);
	con->edit->cur_x=con_strlen(s);
	BTGE_ConEditUI_AdjustEditWindow(con);
}

bool BTGE_ConEdit_CheckCursorPastEOL(BTGE_ConsoleInfo *con)
{
	short *s;
	int i;

	i=con->edit->cur_y+1;
	s=BTGE_ConEdit_GetLine(con, i);
	return(con->edit->cur_x>con_strlen(s));
}

void BTGE_ConEdit_AdjustCursorClampEOL(BTGE_ConsoleInfo *con)
{
	short *s;
	int i;

	i=con->edit->cur_y+1;
	s=BTGE_ConEdit_GetLine(con, i);
	if(con->edit->cur_x>con_strlen(s))
		con->edit->cur_x=con_strlen(s);
	//align to next tab
	while((s[con->edit->cur_x]=='\x1F'))
			con->edit->cur_x++;
	BTGE_ConEditUI_AdjustEditWindow(con);
}

void BTGE_ConEdit_UpdateStartSelection(BTGE_ConsoleInfo *con)
{
	if(BTGE_Console_KeyDown(con, K_SHIFT))
	{
		if(!(con->edit->flags&2))
		{
			con->edit->sel_sx=con->edit->cur_x;
			con->edit->sel_sy=con->edit->cur_y;
			con->edit->sel_ex=con->edit->cur_x;
			con->edit->sel_ey=con->edit->cur_y;
			con->edit->flags|=2;
		}else
		{
			con->edit->sel_ex=con->edit->cur_x;
			con->edit->sel_ey=con->edit->cur_y;
		}
	}else
	{
	}
}

void BTGE_ConEdit_UpdateEndSelection(BTGE_ConsoleInfo *con)
{
	if(BTGE_Console_KeyDown(con, K_SHIFT))
	{
		con->edit->sel_ex=con->edit->cur_x;
		con->edit->sel_ey=con->edit->cur_y;
	}else
	{
		BTGE_ConEdit_UpdateClearSelection(con);
	}
}

void BTGE_ConEdit_UpdateClearSelection(BTGE_ConsoleInfo *con)
{
	con->edit->sel_sx=0;
	con->edit->sel_sy=0;
	con->edit->sel_ex=0;
	con->edit->sel_ey=0;
	con->edit->flags&=~2;
}

int BTGE_ConEditUI_HandleKey(BTGE_ConsoleInfo *con, int num, int down)
{
	short tb[256], tb2[256];
	char tbc[256];
	short *s, *t, *s0, *s1;
	int i, j, k;

	if(!down)
		return(0);
	
	if(!con->edit)
	{
		if(con->OldHandleKey)
			return(con->OldHandleKey(con, num, down));
		return(0);
	}
	
	if(BTGE_Console_KeyDown(con, K_ALT))
	{
		if((num>='A') && (num<='Z'))
		{
			tbc[0]=num; tbc[1]=0;
			BTGE_ConEdit_SetActiveModule(con, tbc);
			return(0);
		}
		
		if((num==';') || (num==':'))
			{ con->edit->flags^=1; }
		
		return(0);
	}
	
	if(con->edit->flags&1)
	{
		if(con->OldHandleKey)
			return(con->OldHandleKey(con, num, down));
		return(0);
	}
	
	if(num==K_F5)
	{
		i=1; j=con->edit->n_lines-1;
		
		if(con->edit->sel_sy != con->edit->sel_ey)
		{
			i=MIN(con->edit->sel_sy, con->edit->sel_ey)+1;
			j=MAX(con->edit->sel_sy, con->edit->sel_ey);
		}
		
		BTGE_ConEdit_LineEval(con->edit, i, j);
		BTGE_ConEdit_UpdateClearSelection(con);
		return(0);
	}
	
	BTGE_ConEdit_UpdateStartSelection(con);

	if(num==K_UPARROW)
	{
		if(con->edit->cur_y>0)
			{ con->edit->cur_y--; }
			
		BTGE_ConEdit_AdjustCursorClampEOL(con);
//		if(con->edit->cur_y<con->edit->win_y)
//			{ con->edit->win_y=con->edit->cur_y; }

		BTGE_ConEdit_UpdateEndSelection(con);
		return(0);
	}

	if(num==K_LEFTARROW)
	{
		if(con->edit->cur_x<=0)
		{
			if(con->edit->cur_y>0)
			{
				con->edit->cur_y--;
				BTGE_ConEdit_AdjustCursorLineEnd(con);
			}
			BTGE_ConEdit_UpdateEndSelection(con);
			return(0);
		}

		if(con->edit->cur_x>0)
			{ con->edit->cur_x--; }
		BTGE_ConEdit_AdjustCursorTabAlign(con);
//		if(con->edit->cur_x<con->edit->win_x)
//			{ con->edit->win_x=con->edit->cur_x; }
		BTGE_ConEdit_UpdateEndSelection(con);
		return(0);
	}

	if(num==K_DOWNARROW)
	{
		if((con->edit->cur_y+1)<con->edit->n_lines)
			{ con->edit->cur_y++; }
		BTGE_ConEdit_AdjustCursorClampEOL(con);
//		while(con->edit->cur_y>=(con->edit->win_y+50))
//			{ con->edit->win_y++; }
		BTGE_ConEdit_UpdateEndSelection(con);
		return(0);
	}

	if(num==K_RIGHTARROW)
	{
		con->edit->cur_x++;

		if(BTGE_ConEdit_CheckCursorPastEOL(con))
		{
			if((con->edit->cur_y+1)<con->edit->n_lines)
				{ con->edit->cur_y++; }
			con->edit->cur_x=0;
			return(0);
		}

		BTGE_ConEdit_AdjustCursorClampEOL(con);		
//		BTGE_ConEditUI_AdjustEditWindow(con);
		BTGE_ConEdit_UpdateEndSelection(con);
		return(0);
	}

	if(num==K_HOME)
	{
		con->edit->cur_x=0;
		BTGE_ConEditUI_AdjustEditWindow(con);
		BTGE_ConEdit_UpdateEndSelection(con);
		return(0);
	}

	if(num==K_END)
	{
		i=con->edit->cur_y+1;
		s=BTGE_ConEdit_GetLine(con, i);
		con->edit->cur_x=con_strlen(s);
		BTGE_ConEditUI_AdjustEditWindow(con);
		BTGE_ConEdit_UpdateEndSelection(con);
		return(0);
	}

	if(num==K_PGUP)
	{
		con->edit->cur_y-=50;
		con->edit->cur_x=0;

		if(con->edit->cur_y<0)
			{ con->edit->cur_y=0; }
	
		BTGE_ConEditUI_AdjustEditWindow(con);
		BTGE_ConEdit_UpdateEndSelection(con);
		return(0);
	}

	if(num==K_PGDN)
	{
		con->edit->cur_y+=50;
		con->edit->cur_x=0;

		if((con->edit->cur_y+1)>=con->edit->n_lines)
			{ con->edit->cur_y=con->edit->n_lines-1; }
		if(con->edit->cur_y<0)
			{ con->edit->cur_y=0; }

		BTGE_ConEditUI_AdjustEditWindow(con);
		BTGE_ConEdit_UpdateEndSelection(con);
		return(0);
	}

	if(num==K_BACKSPACE)
	{
		BTGE_ConEdit_UpdateClearSelection(con);

		if(con->edit->cur_x>0)
		{
			i=con->edit->cur_y+1;
			s=BTGE_ConEdit_GetLine(con, i);

			BTGE_ConEdit_AdjustCursorTabAlignB(con);

			t=tb; s0=s+con->edit->cur_x-1;
			while(*s && (s<s0))
				*t++=*s++;
			if(*s)s++;
			while(*s)
				*t++=*s++;
			*t++=0;
		
			BTGE_ConEdit_AdjustLineTabsDel(con, tb2, tb);
			BTGE_ConEdit_UpdateLine(con, i, tb2);

			if(con->edit->cur_x>0)
				con->edit->cur_x--;
			if(con->edit->cur_x>con_strlen(tb))
				con->edit->cur_x=con_strlen(tb);
			BTGE_ConEditUI_AdjustEditWindow(con);

			return(0);
		}else if(con->edit->cur_y>0)
		{
			i=con->edit->cur_y;
			s0=BTGE_ConEdit_GetLine(con, i);
			j=con->edit->cur_y+1;
			s1=BTGE_ConEdit_GetLine(con, j);

			con_strcpy(tb, s0);
			con_strcat(tb, s1);
			BTGE_ConEdit_AdjustLineTabs(con, tb2, tb);
			BTGE_ConEdit_UpdateLine(con, i, tb2);
			BTGE_ConEdit_LineDelete(con->edit, j, 1);
			
			con->edit->cur_y--;
			con->edit->cur_x=con_strlen(s0);
			BTGE_ConEditUI_AdjustEditWindow(con);

			return(0);
		}

		return(0);
	}

	if(num==K_DEL)
	{
		BTGE_ConEdit_UpdateClearSelection(con);

		i=con->edit->cur_y+1;
		s=BTGE_ConEdit_GetLine(con, i);

		if(con->edit->cur_x>=con_strlen(s))
		{
			i=con->edit->cur_y+1;
			s0=BTGE_ConEdit_GetLine(con, i);
			j=con->edit->cur_y+2;
			s1=BTGE_ConEdit_GetLine(con, j);

			con_strcpy(tb, s0);
			con_strcat(tb, s1);
			BTGE_ConEdit_AdjustLineTabs(con, tb2, tb);
			BTGE_ConEdit_UpdateLine(con, i, tb2);
			BTGE_ConEdit_LineDelete(con->edit, j, 1);
			
//			con->edit->cur_y--;
			con->edit->cur_x=con_strlen(s0);
			BTGE_ConEditUI_AdjustEditWindow(con);
			return(0);
		}

		BTGE_ConEdit_AdjustCursorTabAlign(con);

		t=tb; s0=s+con->edit->cur_x;
		while(*s && (s<s0))
			*t++=*s++;
		if(*s)s++;
		while(*s)
			*t++=*s++;
		*t++=0;
		
		BTGE_ConEdit_AdjustLineTabsDel(con, tb2, tb);
		BTGE_ConEdit_UpdateLine(con, i, tb2);
		if(con->edit->cur_x>con_strlen(tb))
			con->edit->cur_x=con_strlen(tb);
		BTGE_ConEditUI_AdjustEditWindow(con);

		return(0);
	}

	if(num==K_ENTER)
	{
		BTGE_ConEdit_UpdateClearSelection(con);

		i=con->edit->cur_y+1;
		s=BTGE_ConEdit_GetLine(con, i);

		BTGE_ConEdit_AdjustCursorTabAlign(con);

		t=tb; s0=s+con->edit->cur_x;
		while(*s && (s<s0))
			*t++=*s++;
//		*t++=num;
		*t++=0;

		t=tb2;
		while(*s)
			*t++=*s++;
		*t++=0;
		
		BTGE_ConEdit_UpdateLine(con, i, tb);

		BTGE_ConEdit_LineInsert(con->edit, i+1, 1);
		BTGE_ConEdit_AdjustLineTabs(con, tb, tb2);
		BTGE_ConEdit_UpdateLine(con, i+1, tb);

		con->edit->cur_x=0;
		con->edit->cur_y++;
//		if(con->edit->cur_x>strlen(tb))
//			con->edit->cur_x=strlen(tb);
		BTGE_ConEditUI_AdjustEditWindow(con);
		return(0);
	}

	if(num==K_TAB)
	{
		BTGE_ConEdit_UpdateClearSelection(con);

		i=con->edit->cur_y+1;
		s=BTGE_ConEdit_GetLine(con, i);

		if(con->edit->cur_x>con_strlen(s))
			con->edit->cur_x=con_strlen(s);

		j=4-(con->edit->cur_x&3);

		t=tb; s0=s+con->edit->cur_x;
		while(*s && (s<s0))
			*t++=*s++;
//		*t++=num;
//		while(j--)*t++='\t';
//		*t++='\x1F';
		*t++='\t';
		while(*s)
			*t++=*s++;
		*t++=0;
		
		BTGE_ConEdit_AdjustLineTabs(con, tb2, tb);
		BTGE_ConEdit_UpdateLine(con, i, tb2);

		con->edit->cur_x=
			(con->edit->cur_x+4)&(~3);

		BTGE_ConEdit_AdjustCursorTabAlignAfter(con);

//		con->edit->cur_x++;
//		if(con->edit->cur_x>strlen(tb))
//			con->edit->cur_x=strlen(tb);
		BTGE_ConEditUI_AdjustEditWindow(con);
		return(0);
	}

	if((num>=' ') && (num<='~'))
	{
		BTGE_ConEdit_UpdateClearSelection(con);

		i=con->edit->cur_y+1;
		s=BTGE_ConEdit_GetLine(con, i);

		t=tb; s0=s+con->edit->cur_x;
		while(*s && (s<s0))
			*t++=*s++;
		*t++=num;
		while(*s)
			*t++=*s++;
		*t++=0;
		
		BTGE_ConEdit_AdjustLineTabs(con, tb2, tb);
		BTGE_ConEdit_UpdateLine(con, i, tb2);

		con->edit->cur_x++;
		if(con->edit->cur_x>con_strlen(tb))
			con->edit->cur_x=con_strlen(tb);

		BTGE_ConEdit_AdjustCursorTabAlignAfter(con);

		BTGE_ConEditUI_AdjustEditWindow(con);
		return(0);
	}

	return(0);
}

int BTGE_ConEditUI_Clear(BTGE_ConsoleInfo *con)
	{ return(0); }
// int BTGE_ConEditUI_Render(BTGE_ConsoleInfo *con)
//	{ }

int BTGE_ConEdit_InitUI(BTGE_ConsoleInfo *con)
{
	con->OldHandleKey=con->HandleKey;

//	con->WriteString=BTGE_ConEditUI_WriteString;
	con->Update=BTGE_ConEditUI_Update;
	con->HandleKey=BTGE_ConEditUI_HandleKey;
	con->Clear=BTGE_ConEditUI_Clear;
	return(0);
}
