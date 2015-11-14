#include <btgesv.h>

BTGE_BrushWorld *btge_prefab_worlds[256];
int btge_n_prefab_worlds=0;

BTGE_API BTGE_BrushWorld *BTGE_BrushPrefab_LookupName(char *name)
{
	BTGE_BrushWorld *cur;
	int i;
	
	for(i=0; i<btge_n_prefab_worlds; i++)
	{
		cur=btge_prefab_worlds[i];
		if(!strcmp(cur->map_name, name))
			return(cur);
	}
	return(NULL);
}

BTGE_API BTGE_BrushWorld *BTGE_BrushPrefab_GetName(char *name)
{
	BTGE_BrushWorld *tmp;
	int i;

	tmp=BTGE_BrushPrefab_LookupName(name);
	if(tmp)return(tmp);
	
	tmp=BTGE_Brush_NewWorld();
	BTGE_BrushMap_LoadFile(tmp, name);
	if(!tmp->map_name)
	{
		BTGE_Brush_FreeWorld(tmp);
		return(NULL);
	}
	
	BTGE_BrushWorld_DrawBuildFast(tmp);

	i=btge_n_prefab_worlds++;
	btge_prefab_worlds[i]=tmp;
	return(tmp);
}


/**
  * Get the bmodel for a qualified prefab.
  * Names are given as "mapname:bmdl", such as:
  * "maps/prefabs/wall1a.map:1".
  */
BTGE_API BTGE_BrushModel *BTGE_BrushPrefab_GetBModelName(char *name)
{
	char tb[256];
	BTGE_BrushWorld *wrl;
	BTGE_BrushModel *mcur;
	char *s, *t;
	int i, bnum;

	s=name+strlen(name);
	while((s>name) && (*s!=':'))s--;
	if(!s)return(NULL);
	bnum=atoi(s+1);
	if(bnum<=0)return(NULL);
	
	s=name; t=tb;
	while(*s && (*s!=':'))
		*t++=*s++;
	*t++=0;
		
	wrl=BTGE_BrushPrefab_GetName(tb);
	if(!wrl)return(NULL);
	
	mcur=wrl->bmdl;
	while(mcur)
	{
		if(mcur->mdlnum==bnum)
			return(mcur);
		mcur=mcur->next;
	}
	return(NULL);
}
