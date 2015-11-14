#include <btgesv.h>

BTSV_API BTGE_Inventory *BT_EntityLookupInventory(
	btEntity self, char *classname, char *subname)
{
	BTGE_Inventory *cur, *lst;
	
	cur=self->items; lst=NULL;
	while(cur)
	{
		if(!strcmp(cur->classname, classname))
		{
			if(!subname && !cur->subname)
			{
				cur->chain=lst;
				lst=cur;
				cur=cur->next;
				continue;
			}
			if(subname && cur->subname &&
				!strcmp(cur->subname, subname))
			{
				cur->chain=lst;
				lst=cur;
				cur=cur->next;
				continue;
			}
		}
		cur=cur->next;
	}
	
	return(lst);
}

BTSV_API BTGE_Inventory *BT_EntityGetInventory(
	btEntity self, char *classname, char *subname)
{
	BTGE_Inventory *tmp;
	
	tmp=BT_EntityLookupInventory(self,
		classname, subname);
	if(tmp)return(tmp);
	
	tmp=gctalloc("btge_inventory_t", sizeof(BTGE_Inventory));
	tmp->classname=dystrdup(classname);
	if(subname)
		tmp->subname=dystrdup(subname);
	tmp->next=self->items;
	self->items=tmp;
	
	return(tmp);
}

BTSV_API int BT_EntityGetInventoryCount(
	btEntity self, char *classname, char *subname)
{
	BTGE_Inventory *inv;
	inv=BT_EntityGetInventory(self, classname, subname);
	return(inv->count);
}

BTSV_API bool BT_EntityTryInventoryTakeCount(
	btEntity self, char *classname, char *subname, int take)
{
	BTGE_Inventory *inv;

	inv=BT_EntityGetInventory(self, classname, subname);
	if(inv->count>=take)
	{
		inv->count-=take;
		return(true);
	}
	return(false);
}

BTSV_API void BT_EntityGiveVoxel(btEntity self,
	BTGE_VoxelData *vox, int cnt)
{
	BTGE_Inventory *inv;
	char *s;

	s=BTGE_Voxel_GetNameForVoxelIndex(vox->type&VOX_TY_MASK);
	
	if(s)
	{
		inv=BT_EntityGetInventory(self, "misc_voxel", s);
		inv->count+=cnt;
	}
}

BTSV_API bool BT_EntityTryInventoryVoxTyTakeCount(
	btEntity self, int voxty, int take)
{
	BTGE_Inventory *inv;
	char *s;

	s=BTGE_Voxel_GetNameForVoxelIndex(voxty&VOX_TY_MASK);
	inv=BT_EntityGetInventory(self, "misc_voxel", s);

	if(inv->count>=take)
	{
		inv->count-=take;
		return(true);
	}
	return(false);
}
