#include <lbxgl2.h>
#include <btgecl.h>

BTCL_API void BTCLUI_DrawFormWidget(BTCLUI_Context *uictx,
	BTCLUI_Form *form, BTCLUI_Widget *widget, int ox, int oy)
{
	BTCLUI_Widget *cur;

	if(widget->flags&BTCLUI_FL_HIDDEN)
		return;

	if(widget->vt && widget->vt->Render)
	{
		widget->vt->Render(widget, ox, oy);
		return;
	}

#if 0
	cur=widget->wchild;
	while(cur)
	{
		BTCLUI_DrawFormWidget(uictx, form, cur, ox, oy);
		cur=cur->wnext;
	}
#endif
}

BTCL_API void BTCLUI_DrawForm(BTCLUI_Context *uictx,
	BTCLUI_Form *form)
{
	BTCLUI_Widget *cur;

	if(form->flags&BTCLUI_FL_HIDDEN)
		return;
		
	cur=form->root;
	while(cur)
	{
		BTCLUI_DrawFormWidget(uictx, form, cur,
			form->ox, form->oy-form->ys);
		cur=cur->wnext;
	}
}

BTCL_API void BTCLUI_DrawContext(BTCLUI_Context *uictx)
{
	BTCLUI_Form *cur;
	
	cur=uictx->forms;
	while(cur)
	{
		BTCLUI_DrawForm(uictx, cur);
		cur=cur->next;
	}
}
