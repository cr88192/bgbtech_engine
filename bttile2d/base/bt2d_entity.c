#include <bttile2d.h>

byte *tile2d_map;
byte *tile2d_map_ents;
int tile2d_map_xs;
int tile2d_map_ys;

Tile2D_Entity *tile2d_entity;

bool Tile2D_Entity_CheckBox(
	Tile2D_Entity *self, vec2 mins, vec2 maxs)
{
	vec2 mins2, maxs2;
	
	mins2=v2add(self->org, self->mins);
	maxs2=v2add(self->org, self->maxs);
	
	return(Tile2D_CheckBoxBox(mins, maxs, mins2, maxs2));
}

Tile2D_Entity *Tile2D_Entity_QueryBox(
	vec2 mins, vec2 maxs, Tile2D_Entity *skip, int flags)
{
	Tile2D_Entity *cur, *lst;
	
	cur=tile2d_entity; lst=NULL;
	while(cur)
	{
		if((flags&TILE2D_QFL_SKIPOWNER) && skip)
		{
			if(cur->owner==skip)
				{ cur=cur->next; continue; }
			if(skip->owner==cur)
				{ cur=cur->next; continue; }
			if(cur->owner && (cur->owner==skip->owner))
				{ cur=cur->next; continue; }
		}

		if(flags&TILE2D_QFL_SKIPNONSOLID)
		{
			if((cur->solidtype==TILE2D_SOLIDTYPE_NOT) ||
				(cur->solidtype==TILE2D_SOLIDTYPE_TRIGGER))
					{ cur=cur->next; continue; }
		}
	
		if((cur!=skip) && Tile2D_Entity_CheckBox(cur, mins, maxs))
			{ cur->chain=lst; lst=cur; }
		cur=cur->next;
	}
	return(lst);
}

Tile2D_Entity *Tile2D_Entity_QueryCollide(
	Tile2D_Entity *self, int flags)
{
	vec2 mins, maxs;
	
	mins=v2add(self->org, self->mins);
	maxs=v2add(self->org, self->maxs);
	return(Tile2D_Entity_QueryBox(mins, maxs, self, flags));
}

Tile2D_Entity *Tile2D_Entity_QueryMoveCollide(
	Tile2D_Entity *self, vec2 org, int flags)
{
	vec2 mins, maxs;
	
	mins=v2add(org, self->mins);
	maxs=v2add(org, self->maxs);
	return(Tile2D_Entity_QueryBox(mins, maxs, self, flags));
}

void Tile2D_Entity_Unlink(Tile2D_Entity *self)
{
	Tile2D_Entity *cur, *prv;
	
	cur=tile2d_entity; prv=NULL;
	while(cur)
	{
		if(cur==self)
		{
			if(prv)
			{
				prv->next=cur->next;
			}else
			{
				tile2d_entity=cur->next;
			}
			break;
		}
		prv=cur; cur=cur->next;
	}
}

void Tile2D_Entity_Delete(Tile2D_Entity *self)
{
	Tile2D_Entity_Unlink(self);
	gcfree(self);
}

void Tile2D_Entity_Damage(Tile2D_Entity *self,
	Tile2D_Entity *other, float dmg, int mod)
{
	if(!(self->flags&TILE2D_EFL_TAKEDAMAGE))
		return;

	self->health-=dmg;

	if(self->Pain)
		{ self->Pain(self, other, dmg, mod); }
	
	if(self->health<=0)
	{
		if(self->Die)
			{ self->Die(self, other, dmg, mod); }
	}
}

Tile2D_Entity *Tile2D_AllocEntity(void)
{
	Tile2D_Entity *tmp;
	tmp=gcalloc(sizeof(Tile2D_Entity));
	return(tmp);
}

void *Tile2D_SpawnFuncForName(char *name)
{
	void *fcn;
	
	fcn=NULL;
	if(!strcmp(name, "monster_pedobear"))
		fcn=Tile2D_SpawnEntity_Bear;
	if(!strcmp(name, "monster_giantbear"))
		fcn=Tile2D_SpawnEntity_GiantBear;
	return(fcn);
}

void Tile2D_SpawnEntities()
{
	Tile2D_Entity *(*spawn_f)(vec2 org);
	Tile2D_Entity *tmp;
	char *s;
	float x, y;
	int i, j;
	
	tile2d_entity=NULL;
	
	if(!tile2d_map_ents)
		return;
	
	for(i=0; i<tile2d_map_ys; i++)
		for(j=0; j<tile2d_map_xs; j++)
	{
		x=j-(tile2d_map_xs/2);
		y=i-(tile2d_map_ys/2);
		
		s=Tile2D_EntityNameForColor(tile2d_map_ents+(i*tile2d_map_xs+j)*4);
		
		if(s)
		{
			spawn_f=Tile2D_SpawnFuncForName(s);
			if(spawn_f)
			{
				tmp=spawn_f(vec2(x+0.5, y+0.5));
				
				tmp->next=tile2d_entity;
				tile2d_entity=tmp;
			}
		}
	}
}
