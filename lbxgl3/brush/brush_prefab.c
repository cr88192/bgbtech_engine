#include <lbxgl2.h>

LBXGL_BrushWorld *lbxgl_prefab_worlds[256];
int lbxgl_n_prefab_worlds=0;

LBXGL_API LBXGL_BrushWorld *LBXGL_BrushPrefab_LookupName(char *name)
{
	LBXGL_BrushWorld *cur;
	int i;
	
	for(i=0; i<lbxgl_n_prefab_worlds; i++)
	{
		cur=lbxgl_prefab_worlds[i];
		if(!strcmp(cur->map_name, name))
			return(cur);
	}
	return(NULL);
}

LBXGL_API LBXGL_BrushWorld *LBXGL_BrushPrefab_GetName(char *name)
{
	LBXGL_BrushWorld *tmp;
	int i;

	tmp=LBXGL_BrushPrefab_LookupName(name);
	if(tmp)return(tmp);
	
	tmp=LBXGL_Brush_NewWorld();
	LBXGL_BrushMap_LoadFile(tmp, name);
	if(!tmp->map_name)
	{
		LBXGL_Brush_FreeWorld(tmp);
		return(NULL);
	}
	
	LBXGL_BrushWorld_PushWorld(tmp);
	LBXGL_BrushWorld_DrawBuildFast(tmp);
	LBXGL_BrushWorld_PopWorld();

	i=lbxgl_n_prefab_worlds++;
	lbxgl_prefab_worlds[i]=tmp;
	return(tmp);
}


/**
  * Get the bmodel for a qualified prefab.
  * Names are given as "mapname:bmdl", such as:
  * "maps/prefabs/wall1a.map:1".
  */
LBXGL_API LBXGL_BrushModel *LBXGL_BrushPrefab_GetBModelName(char *name)
{
	char tb[256];
	LBXGL_BrushWorld *wrl;
	LBXGL_BrushModel *mcur;
	char *s, *t;
	int i, bnum;

//	printf("LBXGL_BrushPrefab_GetBModelName: '%s'\n", name);

	s=name+strlen(name);
	while((s>name) && (*s!=':'))s--;
	if(*s!=':')
	{
		printf("LBXGL_BrushPrefab_GetBModelName: no suffix\n");
		return(NULL);
	}
	bnum=atoi(s+1);
	if(bnum<=0)
	{
		printf("LBXGL_BrushPrefab_GetBModelName: Bad Mdlnum %d\n", bnum);
		return(NULL);
	}
	
	s=name; t=tb;
	while(*s && (*s!=':'))
		*t++=*s++;
	*t++=0;
		
	wrl=LBXGL_BrushPrefab_GetName(tb);
	if(!wrl)
	{
		printf("LBXGL_BrushPrefab_GetBModelName: Fail Load '%s'\n", tb);
		return(NULL);
	}
	
	mcur=wrl->bmdl;
	while(mcur)
	{
		if(mcur->mdlnum==bnum)
			return(mcur);
		mcur=mcur->next;
	}

	printf("LBXGL_BrushPrefab_GetBModelName: No BModel %s, %d\n", name, bnum);
	return(NULL);
}
