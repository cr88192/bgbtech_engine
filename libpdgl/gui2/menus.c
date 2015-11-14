#include <pdgl.h>
#include <pdglui.h>

extern int pdglui_forms_hasfocus;

PDGLUI_Menu *pdglui_menu_root=NULL;
PDGLUI_Menu *pdglui_menu_draw=NULL;

PDGLUI_Menu *pdglui_menu_stack[64];
int pdglui_menu_stackpos;

PDGLUI_API int PDGLUI_BeginMenu(char *name)
{
	PDGLUI_Menu *tmp;

	tmp=gcalloc(sizeof(PDGLUI_Menu));
	tmp->name=dystrdup(name);

	tmp->ys=8;
	tmp->ys_item=12;

	pdglui_menu_stack[0]=tmp;
	pdglui_menu_stackpos=1;
}

PDGLUI_API void PDGLUI_EndMenu()
{
	PDGLUI_Menu *tmp;

	tmp=pdglui_menu_stack[0];
	PDGLUI_MenuUpdateItems(tmp);

	tmp->next=pdglui_menu_root;
	pdglui_menu_root=tmp;
	pdglui_menu_stackpos=0;
}

PDGLUI_API int PDGLUI_BeginSubMenu(char *name)
{
	PDGLUI_Menu *tmp, *up;
	PDGLUI_MenuItemT *it, *cur;
	int i;

	up=pdglui_menu_stack[pdglui_menu_stackpos-1];

	tmp=gcalloc(sizeof(PDGLUI_Menu));
	tmp->name=dystrdup(up->name);
	tmp->ys=8;

	pdglui_menu_stack[pdglui_menu_stackpos]=tmp;
	pdglui_menu_stackpos++;

	it=gcalloc(sizeof(PDGLUI_MenuItemT));
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

PDGLUI_API void PDGLUI_EndSubMenu()
{
	PDGLUI_Menu *tmp, *up;

	up=pdglui_menu_stack[pdglui_menu_stackpos-1];
	PDGLUI_MenuUpdateItems(up);

	pdglui_menu_stackpos--;
}

PDGLUI_API void PDGLUI_MenuItem(char *id, char *name)
{
	PDGLUI_Menu *up;
	PDGLUI_MenuItemT *it, *cur;
	int i;

	up=pdglui_menu_stack[pdglui_menu_stackpos-1];

	it=gcalloc(sizeof(PDGLUI_MenuItemT));
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

	PDGLUI_MenuUpdateItems(up);
}

PDGLUI_API void PDGLUI_MenuUpdateItems(PDGLUI_Menu *menu)
{
	PDGLUI_MenuItemT *cur;
	int xs_ico, xs_name, xs_key, xs_widget;
	int ys_item;
	int i, j, k, n;

	xs_ico=0;	xs_name=0;
	xs_key=0;	xs_widget=0;
	ys_item=12;

	cur=menu->item; n=0;
	while(cur)
	{
		if(cur->name)
		{
			i=strlen(cur->name)*8;
			if(i>xs_name)xs_name=i;
		}

		if(cur->keycmd)
		{
			i=strlen(cur->keycmd)*8;
			if(i>xs_key)xs_key=i;
		}
		
		if(cur->ico_tex)
		{
			i=16;
			if(i>xs_ico)xs_ico=i;
			if(i>ys_item)ys_item=i;
		}

		n++;
		cur=cur->next;
	}

	menu->xs_ico=xs_ico;
	menu->xs_name=xs_name;
	menu->xs_key=xs_key;
	menu->xs_widget=xs_widget;
	menu->ys_item=ys_item;

	i=xs_ico+xs_name+xs_key+xs_widget + 4*8;
	menu->xs=i;
	menu->ys=n*ys_item+8;
}

PDGLUI_API PDGLUI_MenuItemT *PDGLUI_MenuAddItem(
	PDGLUI_Menu *menu, char *id, char *name)
{
//	PDGLUI_Menu *up;
	PDGLUI_MenuItemT *it, *cur;
	int i;

//	up=pdglui_menu_stack[pdglui_menu_stackpos-1];

	it=gcalloc(sizeof(PDGLUI_MenuItemT));
	it->id=dystrdup(id);
	it->name=dystrdup(name);

	if(menu->item)
	{
		cur=menu->item;
		while(cur->next)cur=cur->next;
		cur->next=it;
	}else
	{
		menu->item=it;
	}

	if(name)
	{
		i=strlen(name)*8 + 4*8;
		if(i>menu->xs)menu->xs=i;
	}
	menu->ys+=12;
	
	PDGLUI_MenuUpdateItems(menu);
}

PDGLUI_API void PDGLUI_Menus_ShowSubMenu(PDGLUI_Menu *tmp, int ox, int oy)
{
	PDGLUI_Menu *cur;

	cur=pdglui_menu_draw;
	while(cur)
	{
		if(cur==tmp)break;
		cur=cur->dnext;
	}
	if(!cur)
	{
		tmp->dnext=pdglui_menu_draw;
		pdglui_menu_draw=tmp;
	}

	tmp->ox=ox;
	tmp->oy=oy;
}

PDGLUI_API void PDGLUI_ShowMenu(char *name, int ox, int oy)
{
	PDGLUI_Menu *cur;

	cur=pdglui_menu_root;
	while(cur)
	{
		if(!strcmp(cur->name, name))
		{
			PDGLUI_Menus_ShowSubMenu(cur, ox, oy);
			cur->sel=NULL;
		}
		cur=cur->next;
	}
}

PDGLUI_API int PDGLUI_Menus_IsSubMenu(PDGLUI_Menu *tmp, PDGLUI_Menu *tmp2)
{
	PDGLUI_MenuItemT *icur;
	int i;

	if(!tmp)return(0);
	if(!tmp2)return(0);
	if(tmp==tmp2)return(1);

	icur=tmp->item;
	while(icur)
	{
		i=PDGLUI_Menus_IsSubMenu(icur->sub, tmp2);
		if(i)return(i);
		icur=icur->next;
	}
	return(0);
}

PDGLUI_API int PDGLUI_Menus_IsMenuShown(PDGLUI_Menu *tmp)
{
	PDGLUI_Menu *cur;

	cur=pdglui_menu_draw;
	while(cur)
	{
		if(cur==tmp)return(1);
		cur=cur->dnext;
	}
	return(0);
}

PDGLUI_API int PDGLUI_MenuShowP(char *name)
{
	PDGLUI_Menu *cur;

	if(!name)
	{
		if(pdglui_menu_draw)
			return(1);
		return(0);
	}

	cur=pdglui_menu_draw;
	while(cur)
	{
		if(!strcmp(cur->name, name))
			return(1);
		cur=cur->dnext;
	}
	return(0);
}

PDGLUI_API void PDGLUI_SetMenuSelection(char *name, char *id)
{
	PDGLUI_Menu *cur;

	cur=pdglui_menu_root;
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

PDGLUI_API char *PDGLUI_GetMenuSelection(char *name)
{
	PDGLUI_Menu *cur;
	char *s;

	cur=pdglui_menu_root;
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

PDGLUI_API void PDGLUI_MenuMakeHidden(PDGLUI_Menu *menu)
{
	PDGLUI_Menu *cur, *lst;

	if(!menu)
	{
		pdglui_menu_draw=NULL;
		return;
	}

	cur=pdglui_menu_draw; lst=NULL;
	while(cur)
	{
		if(cur==menu)
		{
			if(lst)lst->dnext=cur->dnext;
				else pdglui_menu_draw=cur->dnext;
			cur=cur->dnext;
			continue;
		}

		lst=cur;
		cur=cur->next;
	}
}

PDGLUI_API void PDGLUI_HideMenu(char *name)
{
	PDGLUI_Menu *cur, *lst;

	if(!name)
	{
		pdglui_menu_draw=NULL;
		return;
	}

	cur=pdglui_menu_draw; lst=NULL;
	while(cur)
	{
		if(!strcmp(cur->name, name))
		{
			if(lst)lst->dnext=cur->dnext;
				else pdglui_menu_draw=cur->dnext;
			cur=cur->dnext;
			continue;
		}

		lst=cur;
		cur=cur->next;
	}
}

PDGLUI_API void PDGLUI_SetMenuItemName(
	char *name, char *id, char *title)
{
	PDGLUI_Menu *cur;
	PDGLUI_MenuItemT *icur;
	int i;

	cur=pdglui_menu_root;
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

PDGLUI_API void PDGLUI_Menus_Render(PDGLUI_InputState *state)
{
	PDGLUI_Menu *cur, *lst, *focus;
	PDGLUI_MenuItemT *icur;
	int yp, cs, ysi;

	focus=NULL;
	cur=pdglui_menu_draw;
	while(cur)
	{
		if(	(state->mx>cur->ox) &&
			(state->mx<(cur->ox+cur->xs)) &&
			(state->my>(cur->oy-cur->ys)) &&
			(state->my<cur->oy))
		{
			focus=cur;
			pdglui_forms_hasfocus=2;
		}
		cur=cur->dnext;
	}

	if(state->mb&1)
	{
		cur=pdglui_menu_draw; lst=NULL;
		while(cur)
		{
			if(!PDGLUI_Menus_IsSubMenu(cur, focus))
			{
				if(lst)lst->dnext=cur->dnext;
					else pdglui_menu_draw=cur->dnext;
			}

			cur=cur->dnext;
		}
	}

	cur=pdglui_menu_draw;
	while(cur)
	{
		Draw_FillSquareRGBA(cur->ox, cur->oy-cur->ys,
			cur->xs, cur->ys, 192, 192, 192, 255);
		PDGLUI_Widget_Render3DBorder(cur->ox, cur->oy-cur->ys,
			cur->xs, cur->ys, 0xC0C0C0, 0);

		ysi=cur->ys_item;
//		ysi=12;

//		icur=cur->item; yp=cur->oy-12;
		icur=cur->item; yp=cur->oy-ysi;
		while(icur)
		{
			if(!icur->name)
			{
				PDGLUI_Widget_RenderLight3DBorder(
					cur->ox+4, yp+4, cur->xs-8, 2,
					0xC0C0C0, 1);
//				yp-=12;
				yp-=ysi;
				icur=icur->next;
				continue;
			}

			GfxFont_SetFont("fixed", 0);
//			GfxFont_DrawFormatString(icur->name,
//				cur->ox+2*8, yp,
//				8, 8, 0, 0, 0, 255);

			GfxFont_DrawFormatString(icur->name,
				cur->ox+2*8+cur->xs_ico, yp,
				8, 8, 0, 0, 0, 255);

			if(icur->keycmd)
			{
				GfxFont_DrawString(icur->keycmd,
					cur->ox+2*8+cur->xs_ico+cur->xs_name, yp,
					8, 8, 0, 0, 0, 255);
			}

			cs=0;

			if(	(state->mx>cur->ox) &&
				(state->mx<(cur->ox+cur->xs)) &&
				(state->my>yp) && (state->my<yp+8))
			{
//				PDGLUI_Widget_RenderLight3DBorder(
//					cur->ox+4, yp-2, cur->xs-8, 12,
//					0xC0C0C0, state->mb&1);
				PDGLUI_Widget_RenderLight3DBorder(
					cur->ox+4, yp-2, cur->xs-8, ysi,
					0xC0C0C0, state->mb&1);
					cs=1;
			}

			if(PDGLUI_Menus_IsMenuShown(icur->sub))
			{
//				PDGLUI_Widget_RenderLight3DBorder(
//					cur->ox+4, yp-2, cur->xs-8, 12,
//					0xC0C0C0, 1);

				PDGLUI_Widget_RenderLight3DBorder(
					cur->ox+4, yp-2, cur->xs-8, ysi,
					0xC0C0C0, 1);
			}

			if(icur->sub)
			{
				GfxFont_SetFont("gfx", 0);

				GfxFont_DrawChar(0x29,
					cur->ox+cur->xs-12, yp,
					8, 8, 0, 0, 0, 255);

//				GfxFont_DrawChar(0x29,
//					cur->ox+cur->xs-ysi, yp,
//					8, 8, 0, 0, 0, 255);

				if(cs && (state->mb&1))
					PDGLUI_Menus_ShowSubMenu(icur->sub,
						cur->ox+cur->xs, yp+8);
			}else if(cs && (state->mb&1) && !(state->lmb&1))
			{
				if(cur->Select)
				{
					cur->Select(cur, icur->id);
					PDGLUI_HideMenu(cur->name);
				}else
				{
					PDGLUI_SetMenuSelection(cur->name, icur->id);
					PDGLUI_HideMenu(cur->name);
				}
			}

//			yp-=12;
			yp-=ysi;
			icur=icur->next;
		}

		cur=cur->dnext;
	}
}
