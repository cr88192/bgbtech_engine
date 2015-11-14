#include <btgecl.h>

// extern int btgeclui_context->fgcolor;	//text fg color
// extern int btgeclui_context->bgcolor;	//text bg color
// extern int btgeclui_context->wbgcolor;	//widget bg color

char *BTGECLUI_TextArea_Flatten(BTGECLUI_Widget *obj)
{
	int cx, cy;
	int i, j, k;
	unsigned short *ks;
	char *s, *s2, *b, *t;

	i=0;
	j=0;
	for(i=0; i<obj->nbufptrs; i++)
	{
		s=obj->bufptrs[i];
		if(s)j+=strlen(s)+2;
	}
	j++;

	b=gctalloc("_string_t", j);
	t=b;
	for(i=0; i<obj->nbufptrs; i++)
	{
		s=obj->bufptrs[i];
		if(s)
		{
			strcpy(t, s);
			t+=strlen(t);
			*t++='\r';
			*t++='\n';
			*t=0;
		}
	}

	return(b);
}

char *BTGECLUI_TextArea_FlattenSelection(BTGECLUI_Widget *obj)
{
	int cx, cy;
	int i, j, k;
	unsigned short *ks;
	char *s, *s2, *b, *t;

	int sx, sy, ex, ey;

	sx=obj->sxpos;
	sy=obj->sypos;
	ex=obj->cxpos;
	ey=obj->cypos;

	if((ey<sy) || (ey==sy) && (ex<sx))
	{
		cx=sx;
		cy=sy;
		sx=ex;
		sy=ey;
		ex=cx;
		ey=cy;
	}

	j=0;
	for(i=sy; i<=ey; i++)
	{
		s=obj->bufptrs[i];
		if(i==sy)s+=sx;
		if(i!=ey)
		{
			if(s)j+=strlen(s)+2;
		}else j+=ex;
	}
	j++;

	b=gctalloc("_string_t", j);
	t=b;
	for(i=sy; i<=ey; i++)
	{
		s=obj->bufptrs[i];
		if(i==sy)s+=sx;
		if(i!=ey)
		{
			if(s)
			{
				strcpy(t, s);
				t+=strlen(t);
				*t++='\r';
				*t++='\n';
				*t=0;
			}
		}else
		{
			if(i==sy)j=ex-sx;
				else j=ex;
			strncpy(t, s, j);
		}
	}

//	printf("copy '%s'\n", b);

	return(b);
}

int BTGECLUI_TextArea_DeleteSelection(BTGECLUI_Widget *obj)
{
	int cx, cy;
	int i, j, k;
	unsigned short *ks;
	char *s, *s2, *b, *t;

	int sx, sy, ex, ey;

	sx=obj->sxpos;
	sy=obj->sypos;
	ex=obj->cxpos;
	ey=obj->cypos;

	if((ey<sy) || (ey==sy) && (ex<sx))
	{
		cx=sx;
		cy=sy;
		sx=ex;
		sy=ey;
		ex=cx;
		ey=cy;
	}

	b=gcalloc(512);
	s=obj->bufptrs[sy];
	if(s)strncpy(b, s, sx);
	s=obj->bufptrs[ey];
	if(s)strcat(b, s+ex);
	obj->bufptrs[sy]=b;

	j=ey-sy;
	if(j)for(i=sy+1; i<=((obj->nbufptrs-1)-ey); i++)
		obj->bufptrs[i]=obj->bufptrs[i+j];

	obj->sxpos=-1;
	obj->sypos=-1;
	obj->cxpos=sx;
	obj->cypos=sy;

	return(0);
}

int BTGECLUI_TextArea_Insert(BTGECLUI_Widget *obj, int *px, int *py, char *ks)
{
	int cx, cy;
	int i, j, k;
	char *s, *s2, *b, *t;

	cx=*px;
	cy=*py;

	printf("insert %p\n", ks);

	while(*ks)
	{
		if(((*ks<127) && (*ks>=' ')) || (*ks=='\t'))
		{
			s=obj->bufptrs[cy];
			if(!s)
			{
				s=gcalloc(512);
				obj->bufptrs[cy]=s;
			}
			if(cx>strlen(s))cx=strlen(s);
			s+=cx;
			b=gcralloc(512);
			strcpy(b, s);
			*s++=*ks;
			strcpy(s, b);

			cx++;
		}
		if(*ks=='\n')
		{
			s=obj->bufptrs[cy];
			if(!s)
			{
				s=gcalloc(512);
				obj->bufptrs[cy]=s;
			}
			if(cx>strlen(s))cx=strlen(s);
			s+=cx;
			b=gcralloc(512);
			strcpy(b, s);
			*s=0;

			for(i=(obj->nbufptrs-1); i>(cy+1); i--)
				obj->bufptrs[i]=obj->bufptrs[i-1];

			s2=gcalloc(512);
			obj->bufptrs[cy+1]=s2;
			strcpy(s2, b);

			cx=0;
			cy++;
		}

		ks++;
	}
	*px=cx;
	*py=cy;

	return(0);
}

int BTGECLUI_TextArea_HandleSelect(BTGECLUI_Widget *obj)
{
	if(BTGECLUI_KeyDown(K_SHIFT))
	{
//		printf("sel\n");
		if(obj->sxpos<0)
		{
			obj->sxpos=obj->cxpos;
			obj->sypos=obj->cypos;
		}
	}else
	{
		obj->sxpos=-1;
		obj->sypos=-1;
	}
	return(0);
}

int BTGECLUI_TextArea_HandleInput2(int ox, int oy,
	BTGECLUI_InputState *state, BTGECLUI_Form *form, BTGECLUI_Widget *obj)
{
	int cx, cy;
	int i, j, k;
	unsigned short *ks;
	char *s, *s2, *b, *t;
	void *p;
	int ch;

//	if(BTGECLUI_KeyDown(K_SHIFT))printf("shift a1\n");

	ch=0;
	ks=state->keys;
	while(*ks)
	{
		if((*ks=='x') && BTGECLUI_KeyDown(K_CTRL))
		{
			ch=1;
			b=BTGECLUI_TextArea_FlattenSelection(obj);
			BTGECLUI_TextArea_DeleteSelection(obj);
//			BTGECLUI_ClipBoard_Export(b);
		}
		if((*ks=='c') && BTGECLUI_KeyDown(K_CTRL))
		{
			b=BTGECLUI_TextArea_FlattenSelection(obj);
//			BTGECLUI_ClipBoard_Export(b);
		}
		if(((*ks=='v') && BTGECLUI_KeyDown(K_CTRL)) ||
			((*ks==K_INS) && BTGECLUI_KeyDown(K_SHIFT)))
		{
			ch=1;
//			b=BTGECLUI_ClipBoard_Import("string_t");
			if(b)BTGECLUI_TextArea_Insert(obj, &obj->cxpos,
				&obj->cypos, b);
			if(obj->cxpos<0)obj->cxpos=0;
			if(obj->cypos<0)obj->cypos=0;
		}

		if(BTGECLUI_KeyDown(K_CTRL) || BTGECLUI_KeyDown(K_ALT))
		{
			if(obj->cxpos<obj->wxpos)
				obj->wxpos=obj->cxpos;
			while(obj->cxpos>=(obj->wxpos+obj->cols))
				obj->wxpos++;
			if(obj->cypos<obj->wypos)
				obj->wypos=obj->cypos;
			while(obj->cypos>=(obj->wypos+obj->rows))
				obj->wypos++;

			ks++;
			continue;
		}


		if((*ks<127) && (*ks>=' '))
		{
			ch=1;
			if(obj->sxpos>=0)
				BTGECLUI_TextArea_DeleteSelection(obj);
			s=obj->bufptrs[obj->cypos];
			if(!s)
			{
				s=gcalloc(512);
				obj->bufptrs[obj->cypos]=s;
			}
			if(obj->cxpos>strlen(s))
				obj->cxpos=strlen(s);
			s+=obj->cxpos;
			b=gcralloc(512);
			strcpy(b, s);
			*s++=*ks;
			strcpy(s, b);

			obj->cxpos++;
		}
		if((*ks=='\b') && (obj->sxpos<0))
		{
			ch=1;
			s=obj->bufptrs[obj->cypos];
			if(!s)
			{
				s=gcalloc(512);
				obj->bufptrs[obj->cypos]=s;
			}
			if(obj->cxpos>strlen(s))
				obj->cxpos=strlen(s);
			if(obj->cxpos>0)
			{
				s+=obj->cxpos;
				b=gcralloc(512);
				strcpy(b, s);
				s--;
				strcpy(s, b);
				obj->cxpos--;
			}else if(obj->cypos>0)
			{
				s2=obj->bufptrs[obj->cypos-1];
				if(!s2)
				{
					s2=gcalloc(512);
					obj->bufptrs[obj->cypos-1]=s2;
				}

				obj->cxpos=strlen(s2);
				obj->cypos--;
				strcat(s2, s);

				for(i=obj->cypos+1; i<(obj->nbufptrs-1); i++)
					obj->bufptrs[i]=obj->bufptrs[i+1];
			}
		}
		if((*ks==K_DEL) && (obj->sxpos<0))
		{
			ch=1;
			s=obj->bufptrs[obj->cypos];
			if(!s)
			{
				s=gcalloc(512);
				obj->bufptrs[obj->cypos]=s;
			}
			if(obj->cxpos>strlen(s))
				obj->cxpos=strlen(s);
			if(obj->cxpos<strlen(s))
			{
				s+=obj->cxpos;
				b=gcralloc(512);
				strcpy(b, s+1);
				strcpy(s, b);
			}else
			{
				s2=obj->bufptrs[obj->cypos+1];
				if(!s2)
				{
					s2=gcalloc(512);
					obj->bufptrs[obj->cypos+1]=s2;
				}

				strcat(s, s2);

				for(i=obj->cypos+1; i<(obj->nbufptrs-1); i++)
					obj->bufptrs[i]=obj->bufptrs[i+1];
			}
		}
		if(((*ks=='\b') || (*ks==K_DEL)) && (obj->sxpos>=0))
		{
			ch=1;
			BTGECLUI_TextArea_DeleteSelection(obj);
		}
		if(*ks=='\r')
		{
			ch=1;
			if(obj->sxpos>=0)
				BTGECLUI_TextArea_DeleteSelection(obj);
			s=obj->bufptrs[obj->cypos];
			if(!s)
			{
				s=gcalloc(512);
				obj->bufptrs[obj->cypos]=s;
			}
			if(obj->cxpos>strlen(s))
				obj->cxpos=strlen(s);
			s+=obj->cxpos;
			b=gcralloc(512);
			strcpy(b, s);
			*s=0;

			for(i=(obj->nbufptrs-1); i>(obj->cypos+1); i--)
				obj->bufptrs[i]=obj->bufptrs[i-1];

			s2=gcalloc(512);
			obj->bufptrs[obj->cypos+1]=s2;
			strcpy(s2, b);

			obj->cxpos=0;
			obj->cypos++;
		}
		if(*ks=='\t')
		{
			ch=1;
			if(obj->sxpos>=0)
				BTGECLUI_TextArea_DeleteSelection(obj);
			s=obj->bufptrs[obj->cypos];
			if(!s)
			{
				s=gcalloc(512);
				obj->bufptrs[obj->cypos]=s;
			}
			if(obj->cxpos>strlen(s))
				obj->cxpos=strlen(s);
			s+=obj->cxpos;
			b=gcralloc(512);
			strcpy(b, s);
			*s++='\t';
			strcpy(s, b);

			obj->cxpos++;
		}
		if(*ks==K_HOME)
		{
//			BTGECLUI_TextArea_HandleSelect(obj);
			obj->cxpos=0;
			obj->wxpos=0;
		}
		if(*ks==K_END)
		{
//			BTGECLUI_TextArea_HandleSelect(obj);
			s=obj->bufptrs[obj->cypos];
			obj->cxpos=0;
			if(s)obj->cxpos=strlen(s);
		}
		if(*ks==K_UPARROW)
		{
			BTGECLUI_TextArea_HandleSelect(obj);
			obj->cypos--;
			if(obj->cypos<0)obj->cypos=0;

			s=obj->bufptrs[obj->cypos];
			if(s)
			{
				if(obj->cxpos>strlen(s))
					obj->cxpos=strlen(s);
			}else obj->cxpos=0;
		}
		if(*ks==K_DOWNARROW)
		{
			BTGECLUI_TextArea_HandleSelect(obj);
			obj->cypos++;

			s=obj->bufptrs[obj->cypos];
			if(s)
			{
				if(obj->cxpos>strlen(s))
					obj->cxpos=strlen(s);
			}else obj->cxpos=0;
		}
		if(*ks==K_LEFTARROW)
		{
			BTGECLUI_TextArea_HandleSelect(obj);
			obj->cxpos--;
			if(obj->cxpos<0)
			{
				obj->cypos--;
				obj->cxpos=0;
				if(obj->cypos>=0)
				{
					s=obj->bufptrs[obj->cypos];
					if(s)obj->cxpos=strlen(s);
				}else
				{
					obj->cypos=0;
					obj->cxpos=0;
				}
			}
		}
		if(*ks==K_RIGHTARROW)
		{
			BTGECLUI_TextArea_HandleSelect(obj);
			s=obj->bufptrs[obj->cypos];
			obj->cxpos++;
			if(s)
			{
				if(obj->cxpos>strlen(s))
				{
					obj->cypos++;
					obj->cxpos=0;
				}
			}else
			{
				obj->cypos++;
				obj->cxpos=0;
			}
		}
		if(*ks==K_PGUP)
		{
			BTGECLUI_TextArea_HandleSelect(obj);
			obj->cypos-=obj->rows;
			if(obj->cypos<0)obj->cypos=0;
		}
		if(*ks==K_PGDN)
		{
			BTGECLUI_TextArea_HandleSelect(obj);
			obj->cypos+=obj->rows;
		}

		if(obj->cxpos<obj->wxpos)
			obj->wxpos=obj->cxpos;
		while(obj->cxpos>=(obj->wxpos+obj->cols))
			obj->wxpos++;
		if(obj->cypos<obj->wypos)
			obj->wypos=obj->cypos;
		while(obj->cypos>=(obj->wypos+obj->rows))
			obj->wypos++;

		ks++;
	}

//	p=NULL;
//	if(ch)BTGECLUI_Forms_Message(obj->form, obj->id, "changed", 0, &p);

	if(ch)BTGECLUI_Widget_AddEvent(obj, "changed");
	return(0);
}

int BTGECLUI_TextArea_HandleInput(BTGECLUI_Widget *obj,
	BTGECLUI_InputState *state, int ox, int oy)
{
	if(!strcmp(obj->type, "textarea"))
	{
		if(state->mb&1)obj->state&=~BTGECLUI_GUISTATE_HASFOCUS;

		if(BTGECLUI_Widget_CheckClick(ox, oy, state, obj))
			obj->state|=BTGECLUI_GUISTATE_HASFOCUS;

		if(obj->state&BTGECLUI_GUISTATE_HASFOCUS)
			BTGECLUI_TextArea_HandleInput2(ox, oy, state,
				obj->form, obj);

		if(BTGECLUI_Widget_CheckMClicked(ox, oy, state, obj))
			BTGECLUI_Widget_AddEvent(obj, "mclicked");
		if(BTGECLUI_Widget_CheckRClicked(ox, oy, state, obj))
			BTGECLUI_Widget_AddEvent(obj, "rclicked");

		return(0);
	}
}

int BTGECLUI_TextArea_Render(BTGECLUI_Widget *obj, int ox, int oy)
{
	BTGECLUI_Widget *cur;
	int cx, cy;
	int r, g, b;
	int i, j, k;
	int szo;
	char *s;

	szo=(obj->state&65536)?12:0;

	BTGECLUI_Forms_CalcColor(obj->bgcolor, &r, &g, &b);
	Draw_FillSquareRGBA(ox+obj->ox, oy+obj->oy+szo,
		obj->xs-szo, obj->ys-szo, r, g, b, 255);

	GfxFont_SetFont("fixed", 0);
//	BTGECLUI_Forms_CalcColor(obj->fgcolor, &r, &g, &b);
//	GfxFont_DrawString(obj->buffer, ox+obj->ox+2, oy+obj->oy+2,
//		8, 12, r, g, b, 255);

	if(szo)
	{
		s=gcralloc(64);
		sprintf(s, "L%d, C%d", obj->cypos+1, obj->cxpos+1);
		BTGECLUI_Forms_CalcColor(obj->fgcolor, &r, &g, &b);
		GfxFont_DrawString(s, ox+obj->ox, oy+obj->oy,
			8, 12, r, g, b, 255);
	}

	for(i=0; i<obj->rows; i++)
	{
		cy=oy+obj->oy+obj->ys-(i*12);
		if(!obj->bufptrs[obj->wypos+i])continue;
		s=obj->bufptrs[obj->wypos+i];
		s+=obj->wxpos;
		for(j=0; j<obj->cols; j++)
		{
			cx=ox+obj->ox+(j*8);
//			k=obj->bufptrs[obj->wypos+i][obj->wxpos+j];
			k=*s++;
			if(k=='\t')
			{
				j=((j+8)&(~7));
				continue;
			}
			if(!k)break;
			if(((obj->sxpos>=0) && (obj->sypos>=0)) &&
				((( ((obj->wxpos+j)>=obj->sxpos) &&
				((obj->wypos+i)>=obj->sypos) ) ||
				((obj->wypos+i)>obj->sypos) ) &&

				( ((obj->wypos+i)<obj->cypos) ||
				( ((obj->wxpos+j)<obj->cxpos) &&
				((obj->wypos+i)<=obj->cypos) ))) ||

				((( ((obj->wxpos+j)<obj->sxpos) &&
				((obj->wypos+i)<=obj->sypos) ) ||
				((obj->wypos+i)<obj->sypos) ) &&

				( ((obj->wypos+i)>obj->cypos) ||
				( ((obj->wxpos+j)>=obj->cxpos) &&
				((obj->wypos+i)>=obj->cypos) )))
				)
			{
				Draw_FillSquareRGBA(cx, cy-12,
					8, 12, 0, 0, 128, 255);
				BTGECLUI_Forms_CalcColor(obj->bgcolor,
					&r, &g, &b);
				GfxFont_DrawChar(k, cx, cy-12, 8, 12,
					r, g, b, 255);
			}else
			{
				BTGECLUI_Forms_CalcColor(obj->fgcolor,
					&r, &g, &b);
				GfxFont_DrawChar(k, cx, cy-12, 8, 12,
					r, g, b, 255);
			}
		}
	}

//	i=obj->cxpos-obj->wxpos;

	s=obj->bufptrs[obj->cypos];
	if(s)
	{
		s+=obj->wxpos;
		i=0;
		for(j=obj->wxpos; j<obj->cxpos; j++)
		{
			if(*s=='\t')i=((i+8)&(~7));
			i++;
			s++;
		}
	}else i=obj->cxpos-obj->wxpos;

	j=obj->cypos-obj->wypos;
	if((obj->state&BTGECLUI_GUISTATE_HASFOCUS) &&
		(i>=0) && (i<=obj->cols) && (j>=0) && (j<=obj->cols))
	{
		GfxFont_SetFont("gfx", PDGL_FONT_BLINK);

		cx=ox+obj->ox+(i*8);
		cy=oy+obj->oy+obj->ys-(j*12);

		GfxFont_DrawChar(0x21, cx, cy-12,
			8, 12, 0, 0, 0, 255);

//		GfxFont_DrawChar('I', cx, cy-12,
//			8, 12, 0, 0, 0, 255);

//		GfxFont_DrawChar('I', cx, cy-12,
//			8, 12, 127, 127, 127, 255);
	}

	glDisable(GL_TEXTURE_2D);

	glBegin(GL_LINES);
	glColor4f(0.3, 0.3, 0.3, 1);
	glVertex2f(ox+obj->ox,			oy+obj->oy+szo);
	glVertex2f(ox+obj->ox,			oy+obj->oy+obj->ys);
	glVertex2f(ox+obj->ox,			oy+obj->oy+obj->ys);
	glVertex2f(ox+obj->ox+obj->xs-szo,	oy+obj->oy+obj->ys);

	glColor4f(0.7, 0.7, 0.7, 1);
	glVertex2f(ox+obj->ox,			oy+obj->oy+szo);
	glVertex2f(ox+obj->ox+obj->xs-szo,	oy+obj->oy+szo);
	glVertex2f(ox+obj->ox+obj->xs-szo,	oy+obj->oy+szo);
	glVertex2f(ox+obj->ox+obj->xs-szo,	oy+obj->oy+obj->ys);
	glEnd();

	return(0);
}

#if 0
//BTGECLUI_Widget *BTGECLUI_TextArea_BuildWidget(NetParse_Node *node,
	BTGECLUI_Form *form, BTGECLUI_Forms_StyleState *style)
{
	BTGECLUI_Widget *tmp, *tmp2;
	char *str;
	int fg, bg, wbg;
	int i;

	if(!strcmp(node->key, "textarea"))
	{
		tmp=gcalloc(sizeof(BTGECLUI_Widget));
		tmp->type=dystrdup("textarea");
		tmp->form=form;
		tmp->funcs=btgeclui_context->textarea_methods;

		str=NetParse_GetNodeAttr(node, "id");
		if(str)tmp->id=dystrdup(str);

		str=NetParse_GetNodeAttr(node, "rows");
		if(str)tmp->rows=katoi(str);
		str=NetParse_GetNodeAttr(node, "cols");
		if(str)tmp->cols=katoi(str);

		tmp->fgcolor=style->fgcolor;
		tmp->bgcolor=style->bgcolor;
		tmp->wbgcolor=style->wbgcolor;

		str=NetParse_GetNodeAttr(node, "fgcolor");
		if(str)tmp->fgcolor=BTGECLUI_Forms_DecodeColor(str);
		str=NetParse_GetNodeAttr(node, "bgcolor");
		if(str)tmp->bgcolor=BTGECLUI_Forms_DecodeColor(str);
		str=NetParse_GetNodeAttr(node, "wbgcolor");
		if(str)tmp->wbgcolor=BTGECLUI_Forms_DecodeColor(str);

		str=NetParse_GetNodeAttr(node, "status");
		if(str)tmp->state|=65536;

		tmp->nbufptrs=4096;
		tmp->bufptrs=gcalloc(tmp->nbufptrs*sizeof(char *));
		tmp->sxpos=-1;
		tmp->sypos=-1;

		i=(tmp->state&65536)?12:0;
		tmp->xs=8*tmp->cols+i;
		tmp->ys=12*tmp->rows+i;

		return(tmp);
	}
}
#endif

BTGECLUI_Widget *BTGECLUI_TextArea_Begin(char *type, char *id)
{
	BTGECLUI_Widget *tmp, *tmp2;
	char *str;
	int fg, bg, wbg;
	int i;

	if(!strcmp(type, "textarea"))
	{
		tmp=gcalloc(sizeof(BTGECLUI_Widget));
		tmp->type=dystrdup("textarea");

		tmp->id=dystrdup(id);
		tmp->fgcolor=btgeclui_context->fgcolor;
		tmp->bgcolor=btgeclui_context->bgcolor;
		tmp->wbgcolor=btgeclui_context->wbgcolor;

//		str=NetParse_GetNodeAttr(node, "status");
//		if(str)tmp->state|=65536;

		tmp->rows=25;
		tmp->cols=80;

		return(tmp);
	}

	return(NULL);
}

int BTGECLUI_TextArea_End(BTGECLUI_Widget *tmp)
{
	int i;

	tmp->nbufptrs=4096;
	tmp->bufptrs=gcalloc(tmp->nbufptrs*sizeof(char *));
	tmp->sxpos=-1;
	tmp->sypos=-1;

	i=(tmp->state&65536)?12:0;
	tmp->xs=8*tmp->cols+i;
	tmp->ys=12*tmp->rows+i;

	return(0);
}

char *BTGECLUI_TextArea_GetPropertyS(BTGECLUI_Widget *obj, char *var)
{
	return(NULL);
}

int BTGECLUI_TextArea_SetPropertyS(BTGECLUI_Widget *obj, char *var, char *val)
{
	return(0);
}

int BTGECLUI_TextArea_GetPropertyI(BTGECLUI_Widget *obj, char *var)
{
	return(0);
}

int BTGECLUI_TextArea_SetPropertyI(BTGECLUI_Widget *obj, char *var, int val)
{
	if(!strcmp(var, "rows"))obj->rows=val;
	if(!strcmp(var, "cols"))obj->cols=val;
	if(!strcmp(var, "status"))
	{
		if(val)obj->state|=65536;
			else obj->state&=~65536;
	}
	return(0);
}

BTGECLUI_WidgetType btgeclui_context->textarea_methods=
{
NULL,
"textarea",

BTGECLUI_TextArea_Render,
BTGECLUI_TextArea_HandleInput,
NULL,
BTGECLUI_TextArea_Begin,
BTGECLUI_TextArea_End,
NULL,
BTGECLUI_TextArea_GetPropertyS,
BTGECLUI_TextArea_SetPropertyS,
BTGECLUI_TextArea_GetPropertyI,
BTGECLUI_TextArea_SetPropertyI
};

int BTGECLUI_TextArea_Init()
{
	BTGECLUI_Widget_RegisterType(&btgeclui_context->textarea_methods);
	return(0);
}
