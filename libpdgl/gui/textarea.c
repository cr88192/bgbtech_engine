#include <pdgl.h>
#include <pdglui.h>

struct PDGLUI_WidgetMethods_s *pdglui_textarea_methods=NULL;

char *PDGLUI_TextArea_Flatten(PDGLUI_Widget *obj)
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

	b=ObjType_New("string_t", j);
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

char *PDGLUI_TextArea_FlattenSelection(PDGLUI_Widget *obj)
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

	b=ObjType_New("string_t", j);
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

//	kprint("copy '%s'\n", b);

	return(b);
}

int PDGLUI_TextArea_DeleteSelection(PDGLUI_Widget *obj)
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

	b=kalloc(512);
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

	return(b);
}

int PDGLUI_TextArea_Insert(PDGLUI_Widget *obj, int *px, int *py, char *ks)
{
	int cx, cy;
	int i, j, k;
	char *s, *s2, *b, *t;

	cx=*px;
	cy=*py;

	kprint("insert %p\n", ks);

	while(*ks)
	{
		if(((*ks<127) && (*ks>=' ')) || (*ks=='\t'))
		{
			s=obj->bufptrs[cy];
			if(!s)
			{
				s=kalloc(512);
				obj->bufptrs[cy]=s;
			}
			if(cx>strlen(s))cx=strlen(s);
			s+=cx;
			b=kralloc(512);
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
				s=kalloc(512);
				obj->bufptrs[cy]=s;
			}
			if(cx>strlen(s))cx=strlen(s);
			s+=cx;
			b=kralloc(512);
			strcpy(b, s);
			*s=0;

			for(i=(obj->nbufptrs-1); i>(cy+1); i--)
				obj->bufptrs[i]=obj->bufptrs[i-1];

			s2=kalloc(512);
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

int PDGLUI_TextArea_HandleSelect(PDGLUI_Widget *obj)
{
	if(PDGLUI_KeyDown(K_SHIFT))
	{
//		kprint("sel\n");
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

int PDGLUI_TextArea_HandleInput2(int ox, int oy,
	PDGLUI_InputState *state, PDGLUI_Form *form, PDGLUI_Widget *obj)
{
	int cx, cy;
	int i, j, k;
	unsigned short *ks;
	char *s, *s2, *b, *t;
	void *p;
	int ch;

//	if(PDGLUI_KeyDown(K_SHIFT))kprint("shift a1\n");

	ch=0;
	ks=state->keys;
	while(*ks)
	{
		if((*ks<127) && (*ks>=' ')  && !PDGLUI_KeyDown(K_CTRL))
		{
			ch=1;
			if(obj->sxpos>=0)
				PDGLUI_TextArea_DeleteSelection(obj);
			s=obj->bufptrs[obj->cypos];
			if(!s)
			{
				s=kalloc(512);
				obj->bufptrs[obj->cypos]=s;
			}
			if(obj->cxpos>strlen(s))
				obj->cxpos=strlen(s);
			s+=obj->cxpos;
			b=kralloc(512);
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
				s=kalloc(512);
				obj->bufptrs[obj->cypos]=s;
			}
			if(obj->cxpos>strlen(s))
				obj->cxpos=strlen(s);
			if(obj->cxpos>0)
			{
				s+=obj->cxpos;
				b=kralloc(512);
				strcpy(b, s);
				s--;
				strcpy(s, b);
				obj->cxpos--;
			}else if(obj->cypos>0)
			{
				s2=obj->bufptrs[obj->cypos-1];
				if(!s2)
				{
					s2=kalloc(512);
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
				s=kalloc(512);
				obj->bufptrs[obj->cypos]=s;
			}
			if(obj->cxpos>strlen(s))
				obj->cxpos=strlen(s);
			if(obj->cxpos<strlen(s))
			{
				s+=obj->cxpos;
				b=kralloc(512);
				strcpy(b, s+1);
				strcpy(s, b);
			}else
			{
				s2=obj->bufptrs[obj->cypos+1];
				if(!s2)
				{
					s2=kalloc(512);
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
			PDGLUI_TextArea_DeleteSelection(obj);
		}
		if(*ks=='\r')
		{
			ch=1;
			if(obj->sxpos>=0)
				PDGLUI_TextArea_DeleteSelection(obj);
			s=obj->bufptrs[obj->cypos];
			if(!s)
			{
				s=kalloc(512);
				obj->bufptrs[obj->cypos]=s;
			}
			if(obj->cxpos>strlen(s))
				obj->cxpos=strlen(s);
			s+=obj->cxpos;
			b=kralloc(512);
			strcpy(b, s);
			*s=0;

			for(i=(obj->nbufptrs-1); i>(obj->cypos+1); i--)
				obj->bufptrs[i]=obj->bufptrs[i-1];

			s2=kalloc(512);
			obj->bufptrs[obj->cypos+1]=s2;
			strcpy(s2, b);

			obj->cxpos=0;
			obj->cypos++;
		}
		if(*ks=='\t')
		{
			ch=1;
			if(obj->sxpos>=0)
				PDGLUI_TextArea_DeleteSelection(obj);
			s=obj->bufptrs[obj->cypos];
			if(!s)
			{
				s=kalloc(512);
				obj->bufptrs[obj->cypos]=s;
			}
			if(obj->cxpos>strlen(s))
				obj->cxpos=strlen(s);
			s+=obj->cxpos;
			b=kralloc(512);
			strcpy(b, s);
			*s++='\t';
			strcpy(s, b);

			obj->cxpos++;
		}
		if(*ks==K_HOME)
		{
//			PDGLUI_TextArea_HandleSelect(obj);
			obj->cxpos=0;
			obj->wxpos=0;
		}
		if(*ks==K_END)
		{
//			PDGLUI_TextArea_HandleSelect(obj);
			s=obj->bufptrs[obj->cypos];
			obj->cxpos=0;
			if(s)obj->cxpos=strlen(s);
		}
		if(*ks==K_UPARROW)
		{
			PDGLUI_TextArea_HandleSelect(obj);
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
			PDGLUI_TextArea_HandleSelect(obj);
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
			PDGLUI_TextArea_HandleSelect(obj);
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
			PDGLUI_TextArea_HandleSelect(obj);
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
			PDGLUI_TextArea_HandleSelect(obj);
			obj->cypos-=obj->rows;
			if(obj->cypos<0)obj->cypos=0;
		}
		if(*ks==K_PGDN)
		{
			PDGLUI_TextArea_HandleSelect(obj);
			obj->cypos+=obj->rows;
		}

		if((*ks=='x') && PDGLUI_KeyDown(K_CTRL))
		{
			ch=1;
			b=PDGLUI_TextArea_FlattenSelection(obj);
			PDGLUI_TextArea_DeleteSelection(obj);
//			PDGLUI_ClipBoard_Export(b);
		}
		if((*ks=='c') && PDGLUI_KeyDown(K_CTRL))
		{
			b=PDGLUI_TextArea_FlattenSelection(obj);
//			PDGLUI_ClipBoard_Export(b);
		}
		if(((*ks=='v') && PDGLUI_KeyDown(K_CTRL)) ||
			((*ks==K_INS) && PDGLUI_KeyDown(K_SHIFT)))
		{
			ch=1;
//			b=PDGLUI_ClipBoard_Import("string_t");
			if(b)PDGLUI_TextArea_Insert(obj, &obj->cxpos,
				&obj->cypos, b);
			if(obj->cxpos<0)obj->cxpos=0;
			if(obj->cypos<0)obj->cypos=0;
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

	p=NULL;
	if(ch)PDGLUI_Forms_Message(obj->form, obj->id, "changed", 0, &p);
	return(0);
}

int PDGLUI_TextArea_HandleInput(PDGLUI_Widget *obj,
	PDGLUI_InputState *state, int ox, int oy)
{
	if(!strcmp(obj->type, "textarea"))
	{
		if(state->mb&1)obj->state&=~PDGLUI_GUISTATE_HASFOCUS;

		if(PDGLUI_Widget_CheckClick(ox, oy, state, obj))
			obj->state|=PDGLUI_GUISTATE_HASFOCUS;

		if(obj->state&PDGLUI_GUISTATE_HASFOCUS)
			PDGLUI_TextArea_HandleInput2(ox, oy, state,
				obj->form, obj);

		return(0);
	}
}

int PDGLUI_TextArea_Render(PDGLUI_Widget *obj, int ox, int oy)
{
	PDGLUI_Widget *cur;
	int cx, cy;
	int r, g, b;
	int i, j, k;
	int szo;
	char *s;

	szo=(obj->state&65536)?12:0;

	PDGLUI_Forms_CalcColor(obj->bgcolor, &r, &g, &b);
	Draw_FillSquareRGBA(ox+obj->ox, oy+obj->oy+szo,
		obj->xs-szo, obj->ys-szo, r, g, b, 255);

	GfxFont_SetFont("fixed", 0);
//	PDGLUI_Forms_CalcColor(obj->fgcolor, &r, &g, &b);
//	GfxFont_DrawString(obj->buffer, ox+obj->ox+2, oy+obj->oy+2,
//		8, 12, r, g, b, 255);

	if(szo)
	{
		s=kralloc(64);
		kprints(s, "L%d, C%d", obj->cypos+1, obj->cxpos+1);
		PDGLUI_Forms_CalcColor(obj->fgcolor, &r, &g, &b);
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
				PDGLUI_Forms_CalcColor(obj->bgcolor,
					&r, &g, &b);
				GfxFont_DrawChar(k, cx, cy-12, 8, 12,
					r, g, b, 255);
			}else
			{
				PDGLUI_Forms_CalcColor(obj->fgcolor,
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
	if((obj->state&PDGLUI_GUISTATE_HASFOCUS) &&
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

int PDGLUI_TextArea_SendMessage(PDGLUI_Widget *obj, char *msg,
	int nargs, void *args)
{
	char *s;

	if(!strcmp(msg, "flatten_text"))
	{
		s=PDGLUI_TextArea_Flatten(obj);
		PDGLUI_Forms_Message(obj->form, obj->id, "text", 1, &s);
	}
}

PDGLUI_Widget *PDGLUI_TextArea_BuildWidget(NetParse_Node *node,
	PDGLUI_Form *form, PDGLUI_Forms_StyleState *style)
{
	PDGLUI_Widget *tmp, *tmp2;
	char *str;
	int fg, bg, wbg;
	int i;

	if(!strcmp(node->key, "textarea"))
	{
		tmp=kalloc(sizeof(PDGLUI_Widget));
		tmp->type=kstrdup("textarea");
		tmp->form=form;
		tmp->funcs=pdglui_textarea_methods;

		str=NetParse_GetNodeAttr(node, "id");
		if(str)tmp->id=kstrdup(str);

		str=NetParse_GetNodeAttr(node, "rows");
		if(str)tmp->rows=katoi(str);
		str=NetParse_GetNodeAttr(node, "cols");
		if(str)tmp->cols=katoi(str);
//		str=NetParse_GetNodeAttr(node, "maxlength");
//		if(str)tmp->maxlength=katoi(str);

		tmp->fgcolor=style->fgcolor;
		tmp->bgcolor=style->bgcolor;
		tmp->wbgcolor=style->wbgcolor;

		str=NetParse_GetNodeAttr(node, "fgcolor");
		if(str)tmp->fgcolor=PDGLUI_Forms_DecodeColor(str);
		str=NetParse_GetNodeAttr(node, "bgcolor");
		if(str)tmp->bgcolor=PDGLUI_Forms_DecodeColor(str);
		str=NetParse_GetNodeAttr(node, "wbgcolor");
		if(str)tmp->wbgcolor=PDGLUI_Forms_DecodeColor(str);

		str=NetParse_GetNodeAttr(node, "status");
		if(str)tmp->state|=65536;

//		if(!tmp->maxlength)tmp->maxlength=65536;
//		tmp->buffer=kalloc(tmp->maxlength);
//		if(node->first)if(node->first->text)
//			strcpy(tmp->buffer, node->first->text);

		tmp->nbufptrs=4096;
		tmp->bufptrs=kalloc(tmp->nbufptrs*sizeof(char *));
		tmp->sxpos=-1;
		tmp->sypos=-1;

		i=(tmp->state&65536)?12:0;
		tmp->xs=8*tmp->cols+i;
		tmp->ys=12*tmp->rows+i;

		return(tmp);
	}
}

int PDGLUI_TextArea_Init()
{
	PDGLUI_Widget_RegisterType("textarea", PDGLUI_TextArea_BuildWidget);

	pdglui_textarea_methods=kalloc(sizeof(struct PDGLUI_WidgetMethods_s));
	pdglui_textarea_methods->render=&PDGLUI_TextArea_Render;
	pdglui_textarea_methods->handle_input=&PDGLUI_TextArea_HandleInput;
//	pdglui_textarea_methods->size=&PDGLUI_TextArea_SizeWidget;
	pdglui_textarea_methods->message=&PDGLUI_TextArea_SendMessage;
}
