#include <bttile2d.h>

Tile2D_Entity *tile2d_entity;
Tile2D_Entity *tile2d_player;

PDGLUI_InputState *bt2d_state;
PDGLUI_Camera *bt2d_cam;

void Tile2D_AnimTick_Player(Tile2D_Entity *player)
{
	Tile2D_Entity *tmp;
	vec2 org1, dir, ofs;
	float f, g;
	char *s, *s1;
	float x, y;
	int i, j;

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
	
	tile2d_player->spr_tex=LBXGL_Texture_LoadImage(s);

#if 1
	if(tile2d_player->impulse&BT_IMPULSE_FIRE)
	{
		i=rand()&3;
		j=0;
		if(i==1)j=3;
		if(i==2)j=-3;
		if(v2x(tile2d_player->vel)<0)
		{
			dir=vec2(-10, j);
			ofs=vec2(0, 1.0);
		}else
		{
			dir=vec2(10, j);
			ofs=vec2(2.0, 1.0);
		}

//		i=((int)floor(bt2d_state->time_f*16))&15;
//		if(!i)
		if(1)
		{
			tmp=Tile2D_SpawnEntity_Bullet(
				v2add(tile2d_player->org, ofs),
				dir, 2);
			tmp->next=tile2d_entity;
			tile2d_entity=tmp;

			tmp->owner=tile2d_player;
		}
	}
#endif
}

void Tile2D_Update_Player(Tile2D_Entity *player, float dt)
{
	Tile2D_Entity *tmp, *lst;
	vec2 org1, dir;
	float f, g;
	char *s, *s1;
	float x, y;
	int i, j;


	bt2d_cam->org[0]=v2x(tile2d_player->org);
	bt2d_cam->org[2]=v2y(tile2d_player->org)+6;
	
	s=Tile2D_TexNameForBox(
		v2x(tile2d_player->org)+0.5,
		v2y(tile2d_player->org),
		1, 2);
	if(!s)
	{
		tile2d_player->vel=v2add(tile2d_player->vel,
			vec2(0, -dt*10));
//		tile2d_player_y-=bt2d_state->dt_f*10;
	}else
	{
		f=1.0-4*dt;
		if(f<0)f=0;
	
		if(v2y(tile2d_player->vel)<0)
		{
			tile2d_player->vel=
				vec2(v2x(tile2d_player->vel)*f, 0);
		}else
		{
			tile2d_player->vel=
				vec2(v2x(tile2d_player->vel)*f,
				v2y(tile2d_player->vel));
		}
	}
	
	org1=v2addscale(tile2d_player->org,
		tile2d_player->vel, dt);
	s1=Tile2D_TexNameForBox(
		v2x(org1)+0.5, v2y(org1)+0.2, 1, 1.75);
	if(s1)
	{
		tile2d_player->vel=
			vec2(0, v2y(tile2d_player->vel));
			
		if(v2y(tile2d_player->vel)<0)
		{
			tile2d_player->vel=
				vec2(v2x(tile2d_player->vel), 0);
		}
	}

	lst=Tile2D_Entity_QueryMoveCollide(tile2d_player,
		org1, TILE2D_QFL_SKIPOWNER|TILE2D_QFL_SKIPNONSOLID);
	if(lst)
	{
		tile2d_player->vel=vec2(0, 0);
	}

	tile2d_player->org=v2addscale(tile2d_player->org,
		tile2d_player->vel, dt);
}

BT2D_API int Tile2D_HandleInput()
{
	char buf[64];
	float mins[3], maxs[3];
	float io[3], iv[4], fw[2], pt[4], f, g, v;
	float im[16], im1[16], *imv;
	int imfl;

	int i, j, k;
	unsigned short *kcur;

	Tile2D_Init();

//	if(Con_IsDown())
//		return(0);

//	UI_Camera_Update();

	if(tile2d_player->health<=0)
	{
		if(PDGL_KeyDown(K_ATTACK))
		{
			if(bt2dTime()>tile2d_player->timer_attackFinished)
				{ Tile2D_Reload(); }
			return(0);
		}
		return(0);
	}

	if(isnan(v2x(tile2d_player->vel)) ||
		isinf(v2x(tile2d_player->vel)) ||
		isnan(v2y(tile2d_player->vel)) ||
		isinf(v2y(tile2d_player->vel)))
	{
		tile2d_player->vel=vec2(0, 0);
	}

	imfl=0;
	kcur=bt2d_state->keys;
	while(*kcur)
	{
//		if(PDGL_KeyDown(K_CTRL))
//			imfl|=BT_IMPULSE_FIRE;
//		if(PDGL_KeyDown(K_ENTER))
//			imfl|=BT_IMPULSE_USE;

		if(PDGL_KeyDown(K_NEXTWEAPON))
			imfl|=BT_IMPULSE_NEXTWEAP;
		if(PDGL_KeyDown(K_PREVWEAPON))
			imfl|=BT_IMPULSE_PREVWEAP;

//		if(bt2d_state->mb&1)imfl|=BT_IMPULSE_FIRE;
//		if(bt2d_state->mb&2)imfl|=BT_IMPULSE_ALTFIRE;

		if((*kcur>='0') && (*kcur<='9') && !PDGL_KeyDown(K_ALT))
		{
			imfl|=BT_IMPULSE_0<<((*kcur)-'0');
			kcur++;
			continue;
		}

		if(*kcur=='+')
		{
			f=btCvarGetf("r_split_offset");
			f=f+0.001;
			btCvarSetf("r_split_offset", f);
			printf("r_split_offset %f\n", f);
		}

		if(*kcur=='-')
		{
			f=btCvarGetf("r_split_offset");
			f=f-0.001;
			btCvarSetf("r_split_offset", f);
			printf("r_split_offset %f\n", f);
		}

		kcur++;
	}
	
	if(PDGL_KeyDown(K_MOVELEFT))
	{
//		tile2d_player_x-=bt2d_state->dt_f*1;
		if(v2x(tile2d_player->vel)>-10)
		{
			tile2d_player->vel=v2add(tile2d_player->vel,
				vec2(-50*bt2d_state->dt_f, 0));
		}
	}
	if(PDGL_KeyDown(K_MOVERIGHT))
	{
//		tile2d_player_x+=bt2d_state->dt_f*1;
		if(v2x(tile2d_player->vel)<10)
		{
			tile2d_player->vel=v2add(tile2d_player->vel,
				vec2(50*bt2d_state->dt_f, 0));
		}
	}

	if(PDGL_KeyDown(K_MOVEUP))
	{
		f=1.0-(v2y(tile2d_player->vel)/5);
		if(f<0)f=0;
		if(f>1)f=1;

		tile2d_player->vel=v2add(tile2d_player->vel,
			vec2(0, f*50*bt2d_state->dt_f));
		
//		if(v2y(tile2d_player->vel)<25)
//		{
//			tile2d_player->vel=v2add(tile2d_player->vel,
//				vec2(0, 25*bt2d_state->dt_f));
//		}
//		tile2d_player_x+=bt2d_state->dt_f*1;
	}

	if(PDGL_KeyDown(K_ATTACK))
		imfl|=BT_IMPULSE_FIRE;
	if(PDGL_KeyDown(K_USE))
		imfl|=BT_IMPULSE_USE;
	if(PDGL_KeyDown(K_ATTACK2))
		imfl|=BT_IMPULSE_ALTFIRE;
	if(PDGL_KeyDown(K_ATTACK3))
		imfl|=BT_IMPULSE_ALTFIRE2;

	tile2d_player->impulse=imfl;
}

void Tile2D_SpawnPlayer(vec2 org)
{
	tile2d_player=Tile2D_AllocEntity();

	tile2d_player->next=tile2d_entity;
	tile2d_entity=tile2d_player;

	tile2d_player->mins=vec2(0.5, 0);
	tile2d_player->maxs=vec2(1.5, 2);
//	tile2d_player->org=vec2(0, 5);
	tile2d_player->org=org;
	tile2d_player->vel=vec2(0, 0);

	tile2d_player->spr_size=vec2(2, 2);
//	tile2d_player->spr_center=vec2(1, 0);

	tile2d_player->health=100;
	tile2d_player->flags|=TILE2D_EFL_TAKEDAMAGE;
	
	tile2d_player->solidtype=TILE2D_SOLIDTYPE_SLIDEBOX;
	tile2d_player->movetype=TILE2D_MOVETYPE_WALK;
	
	tile2d_player->Draw=Tile2D_DrawEntity_Sprite;
	tile2d_player->AnimTick=Tile2D_AnimTick_Player;
	tile2d_player->Update=Tile2D_Update_Player;
	tile2d_player->Die=Tile2D_Entity_DieGeneric;
}
