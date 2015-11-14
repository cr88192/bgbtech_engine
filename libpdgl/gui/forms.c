#include <pdgl.h>
#include <pdglui.h>

PDGLUI_Form *pdglui_forms_root=NULL;
PDGLUI_Form *pdglui_forms_focus=NULL;
int pdglui_forms_index=1;

char *pdglui_forms_colors[]=
{
"aqua",		"#00FFFF", "black",	"#000000", "blue",	"#0000FF",
"fuchsia",	"#FF00FF", "gray",	"#808080", "green",	"#008000",
"lime",		"#00FF00", "maroon",	"#800000", "navy",	"#000080",
"olive",	"#808000", "purple",	"#800080", "red",	"#FF0000", 
"silver",	"#C0C0C0", "teal",	"#008080", "white",	"#FFFFFF",
"yellow",	"#FFFF00", NULL,	NULL
};


PDGLUI_Form *PDGLUI_Forms_Lookup(char *name)
{
	PDGLUI_Form *cur;

	cur=pdglui_forms_root;
	while(cur)
	{
		if(!strcmp(cur->name, name))return(cur);
		cur=cur->next;
	}
	return(NULL);
}

int PDGLUI_Forms_HandleInput(PDGLUI_InputState *state)
{
	PDGLUI_Form *cur;

	if(state->mb)
		pdglui_forms_focus=NULL;

	cur=pdglui_forms_root;
	while(cur)
	{
		cur->xs=cur->root->xs;
		cur->ys=cur->root->ys;

		if(
			(state->lmx>=cur->ox) &&
			(state->lmy>=(cur->oy-cur->ys)) &&
			(state->lmx<=(cur->ox+cur->xs)) &&
			(state->lmy<=cur->oy) &&
			state->mb)
		{
			//changing focus to current form
			pdglui_forms_focus=cur;

			//dragging current form
//			cur->ox+=state->dx;
//			cur->oy+=state->dy;
		}
#if 1
		if(
			(state->lmx>=cur->ox) &&
			(state->lmy>=cur->oy) &&
			(state->lmx<=(cur->ox+cur->xs)) &&
			(state->lmy<=(cur->oy+16)) &&
			(state->mb&1))
		{
			//changing focus to current form
			pdglui_forms_focus=cur;

			//dragging current form
			cur->ox+=state->dx;
			cur->oy+=state->dy;
		}
#endif

		if(pdglui_forms_focus==cur)
		{
			PDGLUI_Widget_HandleInput(cur->root, state,
				cur->ox, cur->oy-cur->ys);
		}
		cur=cur->next;
	}
	return(0);
}

int PDGLUI_Forms_RenderColor(int c, int a)
{
	float r, g, b;

	r=((c>>16)&0xff)/255.0;
	g=((c>>8)&0xff)/255.0;
	b=(c&0xff)/255.0;
	glColor4f(r, g, b, a/255.0);
	return(0);
}

int PDGLUI_Forms_RenderDarkColor(int c, int a)
{
	float r, g, b;

	r=((c>>16)&0xff)/255.0;
	g=((c>>8)&0xff)/255.0;
	b=(c&0xff)/255.0;
	r*=2.0/3.0;
	g*=2.0/3.0;
	b*=2.0/3.0;
	glColor4f(r, g, b, a/255.0);
	return(0);
}

int PDGLUI_Forms_RenderLightColor(int c, int a)
{
	float r, g, b;

	r=((c>>16)&0xff)/255.0;
	g=((c>>8)&0xff)/255.0;
	b=(c&0xff)/255.0;
	r*=4.0/3.0;
	g*=4.0/3.0;
	b*=4.0/3.0;
	glColor4f(r, g, b, a/255.0);
	return(0);
}

int PDGLUI_Forms_CalcColor(int c, int *rr, int *gr, int *br)
{
	*rr=((c>>16)&0xff);
	*gr=((c>>8)&0xff);
	*br=(c&0xff);
	return(0);
}

int PDGLUI_Forms_CalcDarkColor(int c, int *rr, int *gr, int *br)
{
	*rr=((c>>16)&0xff)-64;
	*gr=((c>>8)&0xff)-64;
	*br=(c&0xff)-64;
	if(*rr<0)*rr=0;
	if(*gr<0)*gr=0;
	if(*br<0)*br=0;
	return(0);
}

int PDGLUI_Forms_CalcLightColor(int c, int *rr, int *gr, int *br)
{
	*rr=((c>>16)&0xff);
	*gr=((c>>8)&0xff);
	*br=(c&0xff);
	if(*rr>255)*rr=255;
	if(*gr>255)*gr=255;
	if(*br>255)*br=255;
	return(0);
}

int PDGLUI_Forms_Render(PDGLUI_InputState *state)
{
	PDGLUI_Form *cur;
	int r, g, b;
	int i;

	glMatrixMode(GL_MODELVIEW_MATRIX);
	glPushMatrix();

	i=(state->zoom>0)?(1024<<state->zoom):(1024>>(-state->zoom));
	glScalef(1024.0/i, 1024.0/i, 1.0);
	glTranslatef(-state->orgx, -state->orgy, 0);

	GfxFont_SetFont("fixed", 0);

	cur=pdglui_forms_root;
	while(cur)
	{
		if(!strcmp(cur->style, "window"))
		{
			if(pdglui_forms_focus==cur)
			{
				Draw_FillSquareRGBA(cur->ox, cur->oy,
					cur->xs, 16,
					0, 0, 127, 255);
			}else
			{
				Draw_FillSquareRGBA(cur->ox, cur->oy,
					cur->xs, 16, 127, 127, 127, 255);
			}
			PDGLUI_Forms_CalcColor(cur->wbgcolor, &r, &g, &b);
			Draw_FillSquareRGBA(cur->ox, cur->oy-cur->ys,
				cur->xs, cur->ys, r, g, b, 255);
		}
		if(!strcmp(cur->style, "box"))
		{
			PDGLUI_Forms_CalcColor(cur->wbgcolor, &r, &g, &b);
			Draw_FillSquareRGBA(cur->ox, cur->oy-cur->ys,
				cur->xs, cur->ys, r, g, b, 255);
		}
		if(!strcmp(cur->style, "none"))
		{
		}

		PDGLUI_Widget_Render(cur->root, cur->ox, cur->oy-cur->ys);
		cur=cur->next;
	}

	glPopMatrix();
	return(0);
}

int PDGLUI_Forms_DecodeColor(char *s)
{
	int i, j;
	char *t;

	if(*s=='#')
	{
		s++;
		j=0;
		for(i=0; i<6; i++)
		{
			j<<=4;
			if((*s>='0') && (*s<='9'))j+=(*s-'0');
			if((*s>='A') && (*s<='F'))j+=(*s-'A')+10;
			if((*s>='a') && (*s<='f'))j+=(*s-'a')+10;
			s++;
		}
		return(j);
	}else
	{
		for(i=0; pdglui_forms_colors[i*2]; i++)
			if(!kstricmp(pdglui_forms_colors[i*2], s))
		{
			t=pdglui_forms_colors[(i*2)+1];
			j=PDGLUI_Forms_DecodeColor(t);
			return(j);
		}
	}

	return(0x808080);
}

char *PDGLUI_Forms_ShowForm(int ox, int oy, char *base,
	int (*callback)(char *form, void *ptr, char *widget,
		char *event, int nargs, void **args),
	void *data,
	NetParse_Node *form)
{
	PDGLUI_Form *tmp;
	PDGLUI_Forms_StyleState *style;
	char *b, *str;

	tmp=kalloc(sizeof(PDGLUI_Form));

	b=kralloc(strlen(base)+10);
	kprints(b, "%s:%d", base, pdglui_forms_index++);
	tmp->name=kstrdup(b);

	tmp->ox=ox;
	tmp->oy=oy;

	tmp->callback=callback;
	tmp->data=data;


	style=kalloc(sizeof(PDGLUI_Forms_StyleState));
	style->fgcolor=0x000000;
	style->bgcolor=0xFFFFFF;
	style->wbgcolor=0x808080;

	str=NetParse_GetNodeAttr(form, "fgcolor");
	if(str)style->fgcolor=PDGLUI_Forms_DecodeColor(str);
	str=NetParse_GetNodeAttr(form, "bgcolor");
	if(str)style->bgcolor=PDGLUI_Forms_DecodeColor(str);
	str=NetParse_GetNodeAttr(form, "wbgcolor");
	if(str)style->wbgcolor=PDGLUI_Forms_DecodeColor(str);

	tmp->style="window";
	str=NetParse_GetNodeAttr(form, "style");
	if(str)tmp->style=str;

	tmp->fgcolor=style->fgcolor;
	tmp->bgcolor=style->bgcolor;
	tmp->wbgcolor=style->wbgcolor;

	tmp->root=PDGLUI_Widget_BuildWidget(form->first, tmp, style);
	tmp->xs=tmp->root->xs;
	tmp->ys=tmp->root->ys;

	tmp->next=pdglui_forms_root;
	pdglui_forms_root=tmp;

	return(tmp->name);
}

int PDGLUI_Forms_UpdateState(PDGLUI_InputState *state,
	int mx, int my, int mb, unsigned short *keys)
{
	unsigned short *ks;
	int rmx, rmy, dx, dy, rdx, rdy;
	int i;

	state->rlmx=state->rmx;
	state->rlmy=state->rmy;
	state->rmx=mx;
	state->rmy=my;

	dx=state->rmx-state->rlmx;
	dy=state->rmy-state->rlmy;

	rmx=(state->zoom>=0)?(mx<<state->zoom):(mx>>(-state->zoom));
	rmy=(state->zoom>=0)?(my<<state->zoom):(my>>(-state->zoom));

	rdx=(state->zoom>=0)?(dx<<state->zoom):(dx>>(-state->zoom));
	rdy=(state->zoom>=0)?(dy<<state->zoom):(dy>>(-state->zoom));

	state->lmx=state->mx;
	state->lmy=state->my;
	state->lmb=state->mb;

	state->mx=rmx+state->orgx;
	state->my=rmy+state->orgy;
	state->mb=mb;

	if((state->mb&1) && !(state->lmb&1))
	{
		state->omx=state->lmx;
		state->omy=state->lmy;
	}

	state->dx=state->mx-state->lmx;
	state->dy=state->my-state->lmy;

	state->odx=state->mx-state->omx;
	state->ody=state->mx-state->omy;

	state->keys=keys;

	if(PDGLUI_KeyDown(K_ALT))
	{
		i=(state->zoom>0)?(64<<state->zoom):(64>>(-state->zoom));
		if(!i)i=1;

		ks=state->keys;
		while(*ks)
		{
			if(*ks==K_LEFTARROW)
				state->orgx-=i;
			if(*ks==K_RIGHTARROW)
				state->orgx+=i;

			if(*ks==K_UPARROW)
				state->orgy+=i;
			if(*ks==K_DOWNARROW)
				state->orgy-=i;

			if(*ks==K_MWHEELUP)
			{
				state->orgx-=rmx>>1;
				state->orgy-=rmy>>1;
				state->zoom--;
			}
			if(*ks==K_MWHEELDOWN)
			{
				state->orgx+=rmx>>1;
				state->orgy+=rmy>>1;
				state->zoom++;
			}

			ks++;
		}

		if(state->mb&1)
		{
			state->orgx-=rdx;
			state->orgy-=rdy;
		}
	}

	return(0);
}

int PDGLUI_KeyDown(int key)
{
	return(PDGL_KeyDown(key));
}

int PDGLUI_Forms_Init()
{
	PDGLUI_Widget_Init();
}
