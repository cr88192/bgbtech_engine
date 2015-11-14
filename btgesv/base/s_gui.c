#include <btgesv.h>

BTSV_API btguiContext BT_GetEntityUIContext(btEntity self)
{
	btguiContext tmp;
	btWorld wrl;

	wrl=btCurrentWorld();

	tmp=self->gui_ctx;
	if(!tmp)
	{
//		tmp=BTCMUI_WorldNewUIContext(wrl, self->entnum);
		tmp=BTCMUI_WorldGetUIContext(wrl, self->entnum);
		self->gui_ctx=tmp;
	}
	
	return(tmp);
}

BTSV_API void BT_EntityGUIUse(btEntity self, btEntity other)
{
	if(!self || !self->gui_ctx)
		return;
		
	btSetClientEntityGUI(other, self);
}
