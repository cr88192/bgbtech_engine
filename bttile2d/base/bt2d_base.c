#include <bttile2d.h>

PDGLUI_InputState *bt2d_state;
PDGLUI_Camera *bt2d_cam;

byte *tile2d_map;
byte *tile2d_map_ents;
int tile2d_map_xs;
int tile2d_map_ys;

double tile2d_maptime;

// float tile2d_player_x;
// float tile2d_player_y;
//vec2 tile2d_player->org;
//vec2 tile2d_player->vel;
//int tile2d_player->impulse;

Tile2D_Entity *tile2d_entity=NULL;
Tile2D_Entity *tile2d_player=NULL;

void Tile2D_Init(void)
{
	static int init=0;
	int xs1, ys1;

	if(init)return;
	init=1;

	if(!bt2d_state)bt2d_state=PDGLUI_GetState();
	if(!bt2d_cam)bt2d_cam=PDGLUI_GetCamera();

#if 1
//	tile2d_player_x=0;
//	tile2d_player_y=5;
//	tile2d_player->org=vec2(0, 5);

	tile2d_map=Tex_LoadFileRaw("tile2d/map0.png",
		&tile2d_map_xs, &tile2d_map_ys);
	tile2d_map_ents=Tex_LoadFileRaw("tile2d/map0_ents.png",
		&xs1, &ys1);
	Tile2D_SpawnEntities();

	Tile2D_SpawnPlayer(vec2(0, 5));
#endif
	
	BT2D_ScriptInit();
}

double bt2dTime(void)
{
	return(tile2d_maptime);
}

void Tile2D_Reload(void)
{
	tile2d_maptime=0;
	tile2d_entity=NULL;
	tile2d_player=NULL;

	Tile2D_SpawnEntities();
	Tile2D_SpawnPlayer(vec2(0, 5));
}

void Tile2D_DrawEntity_Sprite(Tile2D_Entity *self)
{
	float pt[8];
	float x, y, xs, ys, xc, yc;
	float f, g, h;
	int i, j, k;

	if(self->spr_tex<=0)
		return;

	x=v2x(self->org);
	y=v2y(self->org);

	xs=v2x(self->spr_size);
	ys=v2y(self->spr_size);

	xc=v2x(self->spr_center);
	yc=v2y(self->spr_center);

	LBXGL_Shader_BindTexture(self->spr_tex);

	x=x-xc; y=y-yc;

	pt[0]=x;		pt[1]=y;
	pt[2]=x+xs;		pt[3]=y;
	pt[4]=x+xs;		pt[5]=y+ys;
	pt[6]=x;		pt[7]=y+ys;

	h=self->spr_angle*(128/M_PI);
	for(i=0; i<4; i++)
	{
		f=cos(h)*pt[i*2+0]+sin(h)*pt[i*2+1];
		g=-sin(h)*pt[i*2+0]+cos(h)*pt[i*2+1];
		pt[i*2+0]=f;
		pt[i*2+1]=g;
	}

//	x=0; y=5;

	if(v2x(self->vel)>=0)
	{
		LBXGL_Shader_Begin(PDGL_QUADS);
		LBXGL_Shader_TexCoord2f(1, 0);
		LBXGL_Shader_Vertex3f(pt[0], 0, pt[1]);
		LBXGL_Shader_TexCoord2f(0, 0);
		LBXGL_Shader_Vertex3f(pt[2], 0, pt[3]);
		LBXGL_Shader_TexCoord2f(0, 1);
		LBXGL_Shader_Vertex3f(pt[4], 0, pt[5]);
		LBXGL_Shader_TexCoord2f(1, 1);
		LBXGL_Shader_Vertex3f(pt[6], 0, pt[7]);
		LBXGL_Shader_End();
	}else
	{
		LBXGL_Shader_Begin(PDGL_QUADS);
		LBXGL_Shader_TexCoord2f(0, 0);
		LBXGL_Shader_Vertex3f(pt[0], 0, pt[1]);
		LBXGL_Shader_TexCoord2f(1, 0);
		LBXGL_Shader_Vertex3f(pt[2], 0, pt[3]);
		LBXGL_Shader_TexCoord2f(1, 1);
		LBXGL_Shader_Vertex3f(pt[4], 0, pt[5]);
		LBXGL_Shader_TexCoord2f(0, 1);
		LBXGL_Shader_Vertex3f(pt[6], 0, pt[7]);
		LBXGL_Shader_End();
	}
}

bool Tile2D_CheckBoxBox(
	vec2 amins, vec2 amaxs, vec2 bmins, vec2 bmaxs)
{
	if(v2x(amaxs)<v2x(bmins))
		return(false);
	if(v2x(bmaxs)<v2x(amins))
		return(false);
	
	if(v2y(amaxs)<v2y(bmins))
		return(false);
	if(v2y(bmaxs)<v2y(amins))
		return(false);
		
	return(true);
}

void Tile2D_Update_EntityMob(Tile2D_Entity *self, float dt)
{
	self->org=Tile2D_Update_CheckMove(self, vec2(0, 0), dt);
}

void Tile2D_Update_EntityBullet(Tile2D_Entity *self, float dt)
{
	Tile2D_Entity *cur, *nxt;

	self->org=Tile2D_Update_CheckMoveFly(self, dt);
	
	if(self->other)
	{
		cur=self->other;
		while(cur)
		{
			nxt=cur->chain;
			Tile2D_Entity_Damage(cur, self,
				self->damage, TILE2D_MOD_BULLET);
			cur=nxt;
		}
	}
	
	if(self->flags&TILE2D_EFL_BLOCKED)
	{
		Tile2D_Entity_Delete(self);
		return;
	}
	
	if((self->timer_die>0) && (bt2d_state->time_f>self->timer_die))
	{
		Tile2D_Entity_Delete(self);
	}
}

void Tile2D_AnimTick_Bear(Tile2D_Entity *self)
{
	Tile2D_Entity *tmp;
	vec2 dir;
	float h;
	char *s;
	int i, j;

	i=((int)floor(bt2d_state->time_f*4))&1;
	switch(i)
	{
	case 0: s="sprites/tile2d/pedobear_l0"; break;
	case 1: s="sprites/tile2d/pedobear_l1"; break;
	}
	self->spr_tex=LBXGL_Texture_LoadImage(s);

	if(!(rand()&31) && (v2dist(tile2d_player->org, self->org)<64))
	{
		dir=v2sub(tile2d_player->org, self->org);
		dir=v2norm(dir);
		dir=v2scale(dir, 10);
//		dir=vec2(1*sin(h), 1*cos(h));
	
//		j=((int)floor(bt2d_state->time_f*2))&3;
		j=rand()&3;
//		h=(bt2d_state->time_f)*(M_PI/0.1665);
		tmp=Tile2D_SpawnEntity_Bullet(
			v2add(self->org, vec2(0, 1)),
			dir, 0+j);
		tmp->next=tile2d_entity;
		tile2d_entity=tmp;
		tmp->owner=self;
	}
}

void Tile2D_AnimTick_GiantBear(Tile2D_Entity *self)
{
	Tile2D_Entity *tmp;
	char *s;
	float h;
	int i, j, k;

	i=((int)floor(bt2d_state->time_f*4))&1;
	switch(i)
	{
	case 0: s="sprites/tile2d/pedobear_l0"; break;
	case 1: s="sprites/tile2d/pedobear_l1"; break;
	}
	self->spr_tex=LBXGL_Texture_LoadImage(s);

	i=((int)floor(bt2d_state->time_f*0.25))&3;
	if(!i && (v2dist(tile2d_player->org, self->org)<64))
	{
		j=((int)floor(bt2d_state->time_f*2))&3;

		if(!(rand()&3))
		{
			h=(bt2d_state->time_f)*(M_PI/0.1665);
			tmp=Tile2D_SpawnEntity_Bullet(
				v2add(self->org, vec2(0, 10)),
				vec2(1*sin(h), 1*cos(h)), 8+j);
			tmp->next=tile2d_entity;
			tile2d_entity=tmp;
			tmp->owner=self;
		}else
		{
//			h=bt2d_state->time_f*(M_PI/0.3333);
//			h=bt2d_state->time_f*(M_PI/0.111);
			for(i=0; i<4; i++)
			{
				h=(bt2d_state->time_f+i/4.0)*(M_PI/0.1665);
				tmp=Tile2D_SpawnEntity_Bullet(
					v2add(self->org, vec2(0, 10)),
					vec2(2*sin(h), 2*cos(h)), 4+j);
				tmp->next=tile2d_entity;
				tile2d_entity=tmp;
				tmp->owner=self;
			}
		}
	}
}

void Tile2D_AnimTick_Gib(Tile2D_Entity *self)
{
	char *s;
	int i;

	if(self->frame>=8)
		return;

	i=self->frame++;
	switch(i)
	{
	case 0: s="sprites/tile2d/player/play_15"; break;
	case 1: s="sprites/tile2d/player/play_16"; break;
	case 2: s="sprites/tile2d/player/play_17"; break;
	case 3: s="sprites/tile2d/player/play_18"; break;
	case 4: s="sprites/tile2d/player/play_19"; break;
	case 5: s="sprites/tile2d/player/play_20"; break;
	case 6: s="sprites/tile2d/player/play_21"; break;
	case 7: s="sprites/tile2d/player/play_22"; break;
	default: s="sprites/tile2d/player/play_22"; break;
	}
	self->spr_tex=LBXGL_Texture_LoadImage(s);
}

void Tile2D_Entity_DieGeneric(Tile2D_Entity *self,
	Tile2D_Entity *other, float dmg, int mod)
{
	self->flags&=~TILE2D_EFL_TAKEDAMAGE;
	self->frame=0;

	self->solidtype=TILE2D_SOLIDTYPE_NOT;
	tile2d_player->timer_attackFinished=bt2dTime()+10;

	self->AnimTick=Tile2D_AnimTick_Gib;
}

Tile2D_Entity *Tile2D_SpawnEntity_Bear(vec2 org)
{
	Tile2D_Entity *tmp;

	tmp=Tile2D_AllocEntity();
	tmp->org=org;
	tmp->mins=vec2(-0.5, 0);
	tmp->maxs=vec2( 0.5, 2);
	
	tmp->spr_size=vec2(2, 2);
	tmp->spr_center=vec2(1, 0);
	
	tmp->health=25;
	tmp->flags|=TILE2D_EFL_TAKEDAMAGE;
	
	tmp->solidtype=TILE2D_SOLIDTYPE_SLIDEBOX;
	tmp->movetype=TILE2D_MOVETYPE_WALK;
	
	tmp->Draw=Tile2D_DrawEntity_Sprite;
	tmp->AnimTick=Tile2D_AnimTick_Bear;
	tmp->Update=Tile2D_Update_EntityMob;
	tmp->Die=Tile2D_Entity_DieGeneric;
	
	return(tmp);
}

Tile2D_Entity *Tile2D_SpawnEntity_GiantBear(vec2 org)
{
	Tile2D_Entity *tmp;

	tmp=Tile2D_AllocEntity();
	tmp->org=org;
	tmp->mins=vec2(-4, 0);
	tmp->maxs=vec2( 4, 15);
	
	tmp->spr_size=vec2(16, 16);
	tmp->spr_center=vec2(8, 0);

	tmp->health=250;
	tmp->flags|=TILE2D_EFL_TAKEDAMAGE;

	tmp->solidtype=TILE2D_SOLIDTYPE_SLIDEBOX;
	tmp->movetype=TILE2D_MOVETYPE_WALK;
	
	tmp->Draw=Tile2D_DrawEntity_Sprite;
	tmp->AnimTick=Tile2D_AnimTick_GiantBear;
	tmp->Update=Tile2D_Update_EntityMob;
	tmp->Die=Tile2D_Entity_DieGeneric;
	
	return(tmp);
}

Tile2D_Entity *Tile2D_SpawnEntity_Bullet(vec2 org, vec2 vel, int style)
{
	Tile2D_Entity *tmp;
	char *s;

	tmp=Tile2D_AllocEntity();
	tmp->org=org;
	tmp->vel=vel;
	tmp->mins=vec2(-0.25, -0.25);
	tmp->maxs=vec2( 0.25,  0.25);
	
	tmp->spr_size=vec2(0.5, 0.5);
	tmp->spr_center=vec2(0.25, 0.25);
	
	tmp->damage=5;
	
	tmp->Draw=Tile2D_DrawEntity_Sprite;
//	tmp->AnimTick=Tile2D_AnimTick_Bear;
	tmp->Update=Tile2D_Update_EntityBullet;

	switch(style)
	{
	case 0:
		s="sprites/plasma/s_plasma_0";
		break;
	case 1:
		s="sprites/plasma/s_plasma2_0";
		break;
	case 2:
		s="sprites/plasma/s_plasma3_0";
		break;
	case 3:
		s="sprites/bfg/s_bfg1_0";
		break;
	case 4:
		s="sprites/plasma/s_plasma_0";
		tmp->spr_size=vec2(1, 1);
		tmp->spr_center=vec2(0.5, 0.5);
		tmp->damage=10;
		break;
	case 5:
		s="sprites/plasma/s_plasma2_0";
		tmp->spr_size=vec2(1, 1);
		tmp->spr_center=vec2(0.5, 0.5);
		tmp->damage=10;
		break;
	case 6:
		s="sprites/plasma/s_plasma3_0";
		tmp->spr_size=vec2(1, 1);
		tmp->spr_center=vec2(0.5, 0.5);
		tmp->damage=10;
		break;
	case 7:
		s="sprites/bfg/s_bfg1_0";
		tmp->spr_size=vec2(1, 1);
		tmp->spr_center=vec2(0.5, 0.5);
		tmp->damage=10;
		break;
	case 8:
		s="sprites/plasma/s_plasma_0";
		tmp->spr_size=vec2(2, 2);
		tmp->spr_center=vec2(1, 1);
		tmp->damage=20;
		break;
	case 9:
		s="sprites/plasma/s_plasma2_0";
		tmp->spr_size=vec2(2, 2);
		tmp->spr_center=vec2(1, 1);
		tmp->damage=20;
		break;
	case 10:
		s="sprites/plasma/s_plasma3_0";
		tmp->spr_size=vec2(2, 2);
		tmp->spr_center=vec2(1, 1);
		tmp->damage=20;
		break;
	case 11:
		s="sprites/bfg/s_bfg1_0";
		tmp->spr_size=vec2(2, 2);
		tmp->spr_center=vec2(1, 1);
		tmp->damage=20;
		break;
	default:
		s="sprites/plasma/s_plasma3_0";
		break;
	}

//	s="sprites/plasma/s_plasma3_0";
	tmp->spr_tex=LBXGL_Texture_LoadImage(s);

	return(tmp);
}


void Tile2D_DrawEntities()
{
	Tile2D_Entity *cur;
	
	cur=tile2d_entity;
	while(cur)
	{
		if(cur->Draw)
			{ cur->Draw(cur); }
		cur=cur->next;
	}
}

BT2D_API void Tile2D_DrawWorld()
{
	Tile2D_Entity *tmp;
	char *s, *s0, *s1;
	int tex;
	float x, y;
	int i, j;

	LBXGL_Sprite_SetCameraOrgRot(bt2d_cam->org, bt2d_cam->rot);

	glEnable(GL_ALPHA_TEST);

	tex=LBXGL_Texture_LoadImage("textures/voxel/dirt");
	LBXGL_Shader_BindTexture(tex);
	
	s0=NULL;
//	LBXGL_Shader_Begin(PDGL_QUADS);
	for(i=0; i<tile2d_map_xs; i++)
	{
		for(j=0; j<tile2d_map_ys; j++)
		{
			x=i-(tile2d_map_xs/2);
			y=j-(tile2d_map_ys/2);
		
			s1=Tile2D_TexNameForColor(tile2d_map+(j*tile2d_map_xs+i)*4);
			if(s1!=s0)
			{
				if(s0)
				{
					LBXGL_Shader_End();
				}
				
				if(s1)
				{
					tex=LBXGL_Texture_LoadImage(s1);
					LBXGL_Shader_BindTexture(tex);
					LBXGL_Shader_Begin(PDGL_QUADS);
				}
				
				s0=s1;
			}
		
			if(!s0)
				continue;
			
			LBXGL_Shader_TexCoord2f(0, 0);
			LBXGL_Shader_Vertex3f(x, 0, y);

			LBXGL_Shader_TexCoord2f(1, 0);
			LBXGL_Shader_Vertex3f(x+1, 0, y);

			LBXGL_Shader_TexCoord2f(1, 1);
			LBXGL_Shader_Vertex3f(x+1, 0, y+1);

			LBXGL_Shader_TexCoord2f(0, 1);
			LBXGL_Shader_Vertex3f(x, 0, y+1);
		}
	}
	if(s0)
		LBXGL_Shader_End();

	Tile2D_DrawEntities();

#if 0
	if(tile2d_player->impulse&BT_IMPULSE_FIRE)
	{
		i=((int)floor(bt2d_state->time_f*8))&1;
		switch(i)
		{
		case 0: s="sprites/tile2d/player/play_fire1"; break;
		case 1: s="sprites/tile2d/player/play_fire2"; break;
		}

#if 0
		i=((int)floor(bt2d_state->time_f*16))&15;
		if(!i)
		{
			tmp=Tile2D_SpawnEntity_Bullet(
				v2add(tile2d_player->org, vec2(0, 1.0)),
				vec2(10, 0));
			tmp->next=tile2d_entity;
			tile2d_entity=tmp;
		}
#endif
	}else if(fabs(v2y(tile2d_player->vel))>1.0)
	{
		i=((int)floor(bt2d_state->time_f*8))&1;
		switch(i)
		{
		case 0: s="sprites/tile2d/player/play_fly1"; break;
		case 1: s="sprites/tile2d/player/play_fly2"; break;
		}
	}else if((fabs(v2x(tile2d_player->vel))>=0.2) &&
		(fabs(v2x(tile2d_player->vel))>fabs(v2y(tile2d_player->vel))))
	{
		if(fabs(v2x(tile2d_player->vel))>=2.0)
			{ i=((int)floor(bt2d_state->time_f*8))&3; }
		else
			{ i=((int)floor(bt2d_state->time_f*4))&3; }
//		i=((int)floor(bt2d_state->time_f*v2len(tile2d_player->vel)))&3;
		switch(i)
		{
		case 0: s="sprites/tile2d/player/play_0"; break;
		case 1: s="sprites/tile2d/player/play_1"; break;
		case 2: s="sprites/tile2d/player/play_2"; break;
		case 3: s="sprites/tile2d/player/play_3"; break;
		}
	}else
	{
		s="sprites/tile2d/player/play_idle1";
	}
	
	tex=LBXGL_Texture_LoadImage(s);
	LBXGL_Shader_BindTexture(tex);

	x=v2x(tile2d_player->org);
	y=v2y(tile2d_player->org);

//	x=0; y=5;

	if(v2x(tile2d_player->vel)>=0)
	{
		LBXGL_Shader_Begin(PDGL_QUADS);
		LBXGL_Shader_TexCoord2f(1, 0);
		LBXGL_Shader_Vertex3f(x, 0, y);

		LBXGL_Shader_TexCoord2f(0, 0);
		LBXGL_Shader_Vertex3f(x+2, 0, y);

		LBXGL_Shader_TexCoord2f(0, 1);
		LBXGL_Shader_Vertex3f(x+2, 0, y+2);

		LBXGL_Shader_TexCoord2f(1, 1);
		LBXGL_Shader_Vertex3f(x, 0, y+2);
		LBXGL_Shader_End();
	}else
	{
		LBXGL_Shader_Begin(PDGL_QUADS);
		LBXGL_Shader_TexCoord2f(0, 0);
		LBXGL_Shader_Vertex3f(x, 0, y);

		LBXGL_Shader_TexCoord2f(1, 0);
		LBXGL_Shader_Vertex3f(x+2, 0, y);

		LBXGL_Shader_TexCoord2f(1, 1);
		LBXGL_Shader_Vertex3f(x+2, 0, y+2);

		LBXGL_Shader_TexCoord2f(0, 1);
		LBXGL_Shader_Vertex3f(x, 0, y+2);
		LBXGL_Shader_End();
	}
#endif

	LBXGL_Shader_BindTexture(-1);
	pdglDisableTexture2D();
}

BT2D_API void Tile2D_UpdateWorld(float dta)
{
	static float acdt=0;
	static float acdt2=0;
	Tile2D_Entity *cur, *nxt;
	
	vec2 org1;
	float f, g, dt;
	char *s, *s1;

	Tile2D_Init();

	if(bt2d_state->dt_f>1)return;

	tile2d_maptime+=dta;

//	Tile2D_Update_Player(bt2d_state->dt_f);

	acdt+=dta;
	while(acdt>=0.1)
	{
		cur=tile2d_entity;
		while(cur)
		{
			nxt=cur->next;
			if(cur->AnimTick)
				{ cur->AnimTick(cur); }
			cur=nxt;
		}
		
//		Tile2D_AnimTick_Player();
		
//		Tile2D_Update_Player(0.1);
		acdt-=0.1;
	}
	
//	dt=bt2d_state->dt_f;
//	if(dt>0.2)dt=0.2;

	acdt2+=dta;
	while(acdt2>=0.1)
	{
		cur=tile2d_entity;
		while(cur)
		{
			nxt=cur->next;
			if(cur->Update)
				{ cur->Update(cur, 0.1); }
			cur=nxt;
		}
		acdt2-=0.1;
	}

	cur=tile2d_entity;
	while(cur)
	{
		nxt=cur->next;
		if(cur->Update)
			{ cur->Update(cur, acdt2); }
		cur=nxt;
	}

	acdt2=0;
	
//	Tile2D_Update_Player(acdt);
}
