#include <btgecl.h>

// extern int btgeclui_context->forms_hasfocus;

// BTGECLUI_Menu *btgeclui_context->menu_root=NULL;
// BTGECLUI_Menu *btgeclui_context->menu_draw=NULL;

// BTGECLUI_Menu *btgeclui_context->menu_stack[64];
// int btgeclui_context->menu_stackpos;

BTGECLUI_API int BTGECLUI_BeginMenu(char *name)
{
	BTGECLUI_Menu *tmp;

	tmp=gcalloc(sizeof(BTGECLUI_Menu));
	tmp->name=dystrdup(name);

	tmp->ys=8;

	btgeclui_context->menu_stack[0]=tmp;
	btgeclui_context->menu_stackpos=1;
}

BTGECLUI_API void BTGECLUI_EndMenu()
{
	BTGECLUI_Menu *tmp;

	tmp=btgeclui_context->menu_stack[0];
	tmp->next=btgeclui_context->menu_root;
	btgeclui_context->menu_root=tmp;
	btgeclui_context->menu_stackpos=0;
}

BTGECLUI_API int BTGECLUI_BeginSubMenu(char *name)
{
	BTGECLUI_Menu *tmp, *up;
	BTGECLUI_MenuItemT *it, *cur;
	int i;

	up=btgeclui_context->menu_stack[btgeclui_context->menu_stackpos-1];

	tmp=gcalloc(sizeof(BTGECLUI_Menu));
	tmp->name=dystrdup(up->name);
	tmp->ys=8;

	btgeclui_context->menu_stack[btgeclui_context->menu_stackpos]=tmp;
	btgeclui_context->menu_stackpos++;

	it=gcalloc(sizeof(BTGECLUI_MenuItemT));
	it->name=dystrdup(name);
	it->sub=tmp;

	if(up->item)
	{
		cur=up->item;
		while(cur->next)cur=cur->next;
		cur->next=it;
	}else
	{
		up->item=it;
	}

	i=strlen(name)*8 + 4*8;
	if(i>up->xs)up->xs=i;
	up->ys+=12;

	return(0);
}

BTGECLUI_API void BTGECLUI_EndSubMenu()
{
	btgeclui_context->menu_stackpos--;
}

BTGECLUI_API void BTGECLUI_MenuItem(char *id, char *name)
{
	BTGECLUI_Menu *up;
	BTGECLUI_MenuItemT *it, *cur;
	int i;

	up=btgeclui_context->menu_stack[btgeclui_context->menu_stackpos-1];

	it=gcalloc(sizeof(BTGECLUI_MenuItemT));
	it->id=dystrdup(id);
	it->name=dystrdup(name);

	if(up->item)
	{
		cur=up->item;
		while(cur->next)cur=cur->next;
		cur->next=it;
	}else
	{
		up->item=it;
	}

	if(name)
	{
		i=strlen(name)*8 + 4*8;
		if(i>up->xs)up->xs=i;
	}
	up->ys+=12;
}

BTGECLUI_API void BTGECLUI_Menus_ShowSubMenu(BTGECLUI_Menu *tmp, int ox, int oy)
{
	BTGECLUI_Menu *cur;

	cur=btgeclui_context->menu_draw;
	while(cur)
	{
		if(cur==tmp)break;
		cur=cur->dnext;
	}
	if(!cur)
	{
		tmp->dnext=btgeclui_context->menu_draw;
		btgeclui_context->menu_draw=tmp;
	}

	tmp->ox=ox;
	tmp->oy=oy;
}

BTGECLUI_API void BTGECLUI_ShowMenu(char *name, int ox, int oy)
{
	BTGECLUI_Menu *cur;

	cur=btgeclui_context->menu_root;
	while(cur)
	{
		if(!strcmp(cur->name, name))
		{
			BTGECLUI_Menus_ShowSubMenu(cur, ox, oy);
			cur->sel=NULL;
		}
		cur=cur->next;
	}
}

BTGECLUI_API int BTGECLUI_Menus_IsSubMenu(BTGECLUI_Menu *tmp, BTGECLUI_Menu *tmp2)
{
	BTGECLUI_MenuItemT *icur;
	int i;

	if(!tmp)return(0);
	if(!tmp2)return(0);
	if(tmp==tmp2)return(1);

	icur=tmp->item;
	while(icur)
	{
		i=BTGECLUI_Menus_IsSubMenu(icur->sub, tmp2);
		if(i)return(i);
		icur=icur->next;
	}
	return(0);
}

BTGECLUI_API int BTGECLUI_Menus_IsMenuShown(BTGECLUI_Menu *tmp)
{
	BTGECLUI_Menu *cur;

	cur=btgeclui_context->menu_draw;
	while(cur)
	{
		if(cur==tmp)return(1);
		cur=cur->dnext;
	}
	return(0);
}

BTGECLUI_API int BTGECLUI_MenuShowP(char *name)
{
	BTGECLUI_Menu *cur;

	if(!name)
	{
		if(btgeclui_context->menu_draw)
			return(1);
		return(0);
	}

	cur=btgeclui_context->menu_draw;
	while(cur)
	{
		if(!strcmp(cur->name, name))
			return(1);
		cur=cur->dnext;
	}
	return(0);
}

BTGECLUI_API void BTGECLUI_SetMenuSelection(char *name, char *id)
{
	BTGECLUI_Menu *cur;

	cur=btgeclui_context->menu_root;
	while(cur)
	{
		if(!strcmp(cur->name, name))
		{
			cur->sel=id;
			return;
		}
		cur=cur->next;
	}
}

BTGECLUI_API char *BTGECLUI_GetMenuSelection(char *name)
{
	BTGECLUI_Menu *cur;
	char *s;

	cur=btgeclui_context->menu_root;
	while(cur)
	{
		if(!strcmp(cur->name, name))
		{
			s=cur->sel;
			cur->sel=NULL;
			return(s);
		}
		cur=cur->next;
	}
	return(NULL);
}

BTGECLUI_API void BTGECLUI_HideMenu(char *name)
{
	BTGECLUI_Menu *cur, *lst;

	if(!name)
	{
		btgeclui_context->menu_draw=NULL;
		return;
	}

	cur=btgeclui_context->menu_draw; lst=NULL;
	while(cur)
	{
		if(!strcmp(cur->name, name))
		{
			if(lst)lst->dnext=cur->dnext;
				else btgeclui_context->menu_draw=cur->dnext;
			cur=cur->dnext;
			continue;
		}

		lst=cur;
		cur=cur->next;
	}
}

BTGECLUI_API void BTGECLUI_SetMenuItemName(
	char *name, char *id, char *title)
{
	BTGECLUI_Menu *cur;
	BTGECLUI_MenuItemT *icur;
	int i;

	cur=btgeclui_context->menu_root;
	while(cur)
	{
		if(!strcmp(cur->name, name))
		{
			icur=cur->item;
			while(icur)
			{
				if(icur->id && !strcmp(icur->id, id))
				{
					icur->name=dystrdup(title);

					if(title)
					{
						i=strlen(title)*8 + 4*8;
						if(i>cur->xs)cur->xs=i;
					}
					return;
				}
				icur=icur->next;
			}

//			cur->sel=id;
//			return;
		}
		cur=cur->next;
	}
}

BTGECLUI_API void BTGECLUI_Menus_Render(BTGECLUI_InputState *state)
{
	BTGECLUI_Menu *cur, *lst, *focus;
	BTGECLUI_MenuItemT *icur;
	int yp, cs;

	focus=NULL;
	cur=btgeclui_context->menu_draw;
	while(cur)
	{
		if(	(state->mx>cur->ox) &&
			(state->mx<(cur->ox+cur->xs)) &&
			(state->my>(cur->oy-cur->ys)) &&
			(state->my<cur->oy))
		{
			focus=cur;
			btgeclui_context->forms_hasfocus=2;
		}
		cur=cur->dnext;
	}

	if(state->mb&1)
	{
		cur=btgeclui_context->menu_draw; lst=NULL;
		while(cur)
		{
			if(!BTGECLUI_Menus_IsSubMenu(cur, focus))
			{
				if(lst)lst->dnext=cur->dnext;
					else btgeclui_context->menu_draw=cur->dnext;
			}

			cur=cur->dnext;
		}
	}

	cur=btgeclui_context->menu_draw;
	while(cur)
	{
		Draw_FillSquareRGBA(cur->ox, cur->oy-cur->ys,
			cur->xs, cur->ys, 192, 192, 192, 255);
		BTGECLUI_Widget_Render3DBorder(cur->ox, cur->oy-cur->ys,
			cur->xs, cur->ys, 0xC0C0C0, 0);

		icur=cur->item; yp=cur->oy-12;
		while(icur)
		{
			if(!icur->name)
			{
				BTGECLUI_Widget_RenderLight3DBorder(
					cur->ox+4, yp+4, cur->xs-8, 2,
					0xC0C0C0, 1);
				yp-=12;
				icur=icur->next;
				continue;
			}

			GfxFont_SetFont("fixed", 0);
			GfxFont_DrawFormatString(icur->name,
				cur->ox+2*8, yp,
				8, 8, 0, 0, 0, 255);

			cs=0;

			if(	(state->mx>cur->ox) &&
				(state->mx<(cur->ox+cur->xs)) &&
				(state->my>yp) && (state->my<yp+8))
			{
				BTGECLUI_Widget_RenderLight3DBorder(
					cur->ox+4, yp-2, cur->xs-8, 12,
					0xC0C0C0, state->mb&1);
					cs=1;
			}

			if(BTGECLUI_Menus_IsMenuShown(icur->sub))
			{
				BTGECLUI_Widget_RenderLight3DBorder(
					cur->ox+4, yp-2, cur->xs-8, 12,
					0xC0C0C0, 1);
			}

			if(icur->sub)
			{
				GfxFont_SetFont("gfx", 0);

				GfxFont_DrawChar(0x29,
					cur->ox+cur->xs-12, yp,
					8, 8, 0, 0, 0, 255);

				if(cs && (state->mb&1))
					BTGECLUI_Menus_ShowSubMenu(icur->sub,
						cur->ox+cur->xs, yp+8);
			}else if(cs && (state->mb&1) && !(state->lmb&1))
			{
				BTGECLUI_SetMenuSelection(cur->name, icur->id);
				BTGECLUI_HideMenu(cur->name);
			}

			yp-=12;
			icur=icur->next;
		}

		cur=cur->dnext;
	}
}
