#include <btgesv.h>

BTGE_SEntity *btge_world_varstate;

BTSV_API char *btGlobalVarGet(char *name)
{
	char *s;
	if(!name || !btge_world_varstate)
		return(NULL);
	s=BTGE_SEnt_GetStr(btge_world_varstate, name);
	return(s);
}

BTSV_API void btGlobalVarSet(char *name, char *val)
{
	if(!name)return;
		
	if(!btge_world_varstate)
	{
		btge_world_varstate=BTGE_SEnt_NewEntity(
			NULL, "info_varstate_saved");
	}
	
	BTGE_SEnt_SetStr(btge_world_varstate, name, val);
}

BTSV_API char *btQualifyLocalVarName(char *name)
{
	char tb[256];
	char *s, *t;
	char *map;
	
	if(!name)return(NULL);
	map=btCvarGet(name);
//	if(!map)return(name);
	
	t=tb;
	if(map)
	{
		s=map;
		while(*s)
		{
			if(*s=='/')
				{ s++; *t++='_'; continue; }
			*t++=*s++;
		}
		*t++='_';
	}

	s=name;
	while(*s)
	{
		if(*s=='/')
			{ s++; *t++='_'; continue; }
		*t++=*s++;
	}
	*t++=0;
	
	return(dyllStrdup(tb));
}

BTSV_API char *btLocalVarGet(char *name)
{
	char *s0, *s1;
	s0=btQualifyLocalVarName(name);
	s1=btGlobalVarGet(s0);
	return(s1);
}

BTSV_API void btLocalVarSet(char *name, char *val)
{
	char *s;
	s=btQualifyLocalVarName(name);
	btGlobalVarSet(s, val);
	return;
}

BTSV_API btEntity BtSp_info_varstate_saved(btSEntity args)
{
	btge_world_varstate=args;
	return(NULL);
}

BTSV_API BTGE_SEntity *BTGE_SaveGame_FlattenEntity(
	BTGE_SEntity **wrl, btEntity ent)
{
	static char *fields[]=
	{
	"origin",	"angles",		"velocity",		// "angle",
	"movedir",	"classname",	"model",		"targetname",
	"target",	"killtarget",	"message",		"map",
	"sound",	"noise",		"landmark",		"anim",
	"oldanim",	"use_eval",		"spawnflags",	"health",
	"speed",	"wait",			"delay",		"damage",
	"lip",		"height",		"light",		"count",
	"pos1",		"pos2",			"state",		"flags",
	NULL};

	BTGE_SEntity *sent;
	char *s;
	int i;

	//skip invalid entities
	if(!ent || !ent->classname)
		return(NULL);

	//don't save old savepoints
	if(!strcmp(ent->classname, "info_player_saved"))
		return(NULL);

	if(ent->flags&BT_FL_CLIENT)
	{
		//save player state specially
		sent=BTGE_SEnt_NewEntity(wrl, "info_player_saved");
		BT_FlattenClientState(ent, sent);
		return(sent);
	}

	sent=BTGE_SEnt_NewEntity(wrl, ent->classname);

	for(i=0; fields[i]; i++)
	{
		s=btGetFieldAsString(ent, fields[i]);
		if(!s)continue;
		if(!strcmp(s, ""))continue;
		if(!strcmp(s, "0"))continue;
		if(!strcmp(s, "0 0"))continue;
		if(!strcmp(s, "0 0 0"))continue;
		if(!strcmp(s, "0 0 0 0"))continue;
		BTGE_SEnt_SetStr(sent, fields[i], s);
	}

	if(ent->light)
	{
		BTGE_SEnt_SetVec3(sent, "_color", ent->light->clr);
		BTGE_SEnt_SetNum(sent, "style", ent->light->style);
		BTGE_SEnt_SetNum(sent, "_lightflags", ent->light->flags);
		BTGE_SEnt_SetNum(sent, "light", ent->light->val[0]);
		if(ent->light->val[1]!=0)
			BTGE_SEnt_SetNum(sent, "hilight", ent->light->val[1]);

		if(ent->light->flags&BTGE_LFL_BOXCULL)
		{
			BTGE_SEnt_SetVec3(sent, "light_radius", ent->light->boxval);
			BTGE_SEnt_SetVec3(sent, "light_mins", ent->light->mins);
			BTGE_SEnt_SetVec3(sent, "light_maxs", ent->light->maxs);
		}
	}

	if(btGetBModel(ent))
	{
		sent->brush=btGetBModel(ent)->brush;

		BTGE_SEnt_SetStr(sent, "origin", btVec3ToStr(ent->spawnOrigin));
		BTGE_SEnt_SetStr(sent, "angles", btVec3ToStr(ent->spawnAngles));
	}

	return(sent);
}

BTSV_API BTGE_SEntity *BTGE_SaveGame_FlattenWorld(BTGE_BrushWorld *world)
{
	BTGE_BrushWorld *wrl0;
	BTGE_SEntity *sent;
	int i;
	
//	BTGE_BrushPhys_UnlinkWorld(world);
	
//	wrl0=btge_world;
//	btge_world=world;
	
	sent=NULL;
	for(i=1; i<world->n_ents; i++)
		BTGE_SaveGame_FlattenEntity(&sent, world->ents[i]);
	
	if(btge_world_varstate)
		BTGE_SEnt_NewCloneEntity(&sent, NULL, btge_world_varstate);
	
//	world->n_ents=0;
//	world->m_ents=0;
//	btge_world=wrl0;
	
	return(sent);
}

BTSV_API void BTGE_SaveGame_SaveWorld(
	BTGE_BrushWorld *world, char *name)
{
	BTGE_BrushWorld *wtmp;

	if(!name)
		return;

	wtmp=BTGE_Brush_NewWorld();
	wtmp->entity=BTGE_SaveGame_FlattenWorld(world);
	BTGE_BrushMap_SaveFile(wtmp, name);
}
