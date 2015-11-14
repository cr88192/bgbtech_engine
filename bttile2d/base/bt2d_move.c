#include <bttile2d.h>

Tile2D_Entity *tile2d_entity;
Tile2D_Entity *tile2d_player;

vec2 Tile2D_Update_CheckMove(Tile2D_Entity *self, vec2 dir, float dt)
{
	Tile2D_Entity *lst;
	vec2 org, org1;
	float f, g;
	char *s, *s1;

//	bt2d_cam->org[0]=v2x(self->org);
//	bt2d_cam->org[2]=v2y(self->org);
	
	s=Tile2D_TexNameForBox(
		v2x(self->org)+0.5,
		v2y(self->org),
		1, 2);
	if(!s)
	{
		self->vel=v2add(self->vel,
			vec2(0, -dt*10));
//		tile2d_player_y-=bt2d_state->dt_f*10;
	}else
	{
		f=1.0-dt;
		if(f<0)f=0;
	
		if(v2y(self->vel)<0)
		{
			self->vel=
				vec2(v2x(self->vel)*f, 0);
		}else
		{
			self->vel=
				vec2(v2x(self->vel)*f,
				v2y(self->vel));
		}
	}
	
	org1=v2addscale(self->org, self->vel, dt);
	s1=Tile2D_TexNameForBox(
		v2x(org1)+0.5, v2y(org1)+0.2, 1, 1.75);
	if(s1)
	{
		self->vel=vec2(0, v2y(self->vel));
			
		if(v2y(tile2d_player->vel)<0)
		{
			self->vel=vec2(v2x(self->vel), 0);
		}
	}

	lst=Tile2D_Entity_QueryMoveCollide(self, org1,
		TILE2D_QFL_SKIPNONSOLID);
	if(lst)
	{
		self->vel=vec2(0, 0);
	}

	org=v2addscale(
		self->org, self->vel, dt);
	return(org);
}

vec2 Tile2D_Update_CheckMoveFly(Tile2D_Entity *self, float dt)
{
	Tile2D_Entity *lst;
	vec2 org, org1;
	float f, g;
	char *s, *s1;

	self->other=NULL;

	s=Tile2D_TexNameForBox(
		v2x(self->org)+0.5,
		v2y(self->org),
		1, 2);
	if(s)
	{
		self->flags|=TILE2D_EFL_BLOCKED;
		return(self->org);
	}

	org1=v2addscale(
		self->org, self->vel, dt);

	lst=Tile2D_Entity_QueryMoveCollide(self,
		org1, TILE2D_QFL_SKIPOWNER|TILE2D_QFL_SKIPNONSOLID);
	if(lst)
	{
		self->flags|=TILE2D_EFL_BLOCKED;
		self->other=lst;
		return(self->org);
	}

	org=v2addscale(
		self->org, self->vel, dt);
	return(org);
}
