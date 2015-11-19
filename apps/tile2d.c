// #include <windows.h>
// #include <GL/gl.h>

#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#ifndef M_PI
#define M_PI		3.1415926535897932384626433832795
#endif

#define PDGL_BASEHEAP		(64*1024*1024)
#define PDGL_BASEHEAP_LIMIT	(256*1024*1024)
#define PDGL_BASEHEAP_LOBJ	(1024*1024*1024)
#define PDGL_BASEHEAP_CONS	(64*1024*1024)

#define PDGL_PRESTART


// #include <pdlib.h>

#include <pdgl.h>
#include <pdglui.h>
#include <pdgl_stub.h>

#include <lbxgl2.h>
#include <libvecmath.h>

#include <btgesv.h>

#include <bttile2d.h>

Tile2D_Entity *Tile2D_SpawnEntity_Bullet(vec2 org, vec2 vel, int style);

int ConCmds_Load(char **a)
{
}

int ConCmds_Save(char **a)
{
}


char *games[16], *start_map;
int n_games;

//called before main window creation
int pdgl_main_prestart(int argc, char *argv[])
{
	char tb[256];
	char *opts[4];
	char *s;
	void *p;
	int xs, ys;
	int i, j, k, l;

	xs=800; ys=600;

#if 1
	n_games=0; start_map=NULL;
	for(i=1; i<argc; i++)
	{
		dyPrintf("%d: %s\n", i, argv[i]);
		
		if(!strcmp(argv[i], "-game"))
			games[n_games++]=argv[i+1];
		if(!strcmp(argv[i], "+map"))
			start_map=argv[i+1];
		if(!strcmp(argv[i], "+devmap"))
			start_map=argv[i+1];

		if(!strcmp(argv[i], "-width"))
			xs=atoi(argv[i+1]);
		if(!strcmp(argv[i], "-height"))
			ys=atoi(argv[i+1]);

		if(!strcmp(argv[i], "+set") && !strcmp(argv[i+1], "game"))
			games[n_games++]=argv[i+2];
		if(!strcmp(argv[i], "+set") && !strcmp(argv[i+1], "fs_game"))
			games[n_games++]=argv[i+2];
	}
#endif

#if 1
	dyllAddLibraryPath("resource");
	dyllLoadLibrary("game_base");

	for(i=0; i<n_games; i++)
	{
		dyPrintf("add gamedir: %s\n", games[i]);
		vf_mount(games[i], "/", "dir", NULL);
		dyllAddLibraryPathFront(games[i]);
		
		sprintf(tb, "game_%s", games[i]);
		dyllLoadLibrary(tb);
	}
#endif

	Con_Init();
	ConCmds_Init();

#if 1
	btCvarSetf("deathmatch", 0);
	btCvarSetf("coop", 0);

	btCvarSetf("sv_cheats", 0);
	btCvarSetf("sv_nomonsters", 0);
	btCvarSetf("sv_gravity", 800);

	btCvarSetf("cl_showentids", 0);
	btCvarSetf("r_lightbudget", 0);
	btCvarSetf("r_noearlyout", 0);
	btCvarSetf("r_nohires", 0);
	btCvarSetf("r_skipfinal", 0);
	btCvarSetf("r_skipdark", 0);
	btCvarSetf("r_skiplight", 0);
	btCvarSetf("r_skipalpha", 0);

	btCvarSet("r_title", "BGBTech");
	btCvarSetf("r_width", xs);
	btCvarSetf("r_height", ys);

	btCvarSetf("r_usefbo", 1);
	btCvarSetf("r_usehdr", 1);
	btCvarSetf("r_usebloom", 0);
	btCvarSetf("r_parallax", 1);
	btCvarSetf("r_cellshade", 0);
	btCvarSetf("r_anaglyph", 0);
#endif

	btCvarSetf("r_exposure_force", 0);
	btCvarSetf("r_exposure_scale", 1);

	btCvarSetf("cl_showmenu", 1);


	bsvmAddScriptPath("game2d");

	Con_ExecCmdBuf("game2d/default.cfg");
	Con_DoExecCmds();

	Con_ExecCmdBuf("game2d/config.cfg");
	Con_DoExecCmds();

//	Con_ExecCmdBuf("game2d/autoexec.cfg");

	s=btCvarGet("r_title");
	xs=btCvarGetf("r_width");
	ys=btCvarGetf("r_height");

	GfxDrv_SetDefaults(s, xs, ys);
//	Con_ReInit();

	return(1);
}

int pdgl_main_startup(int argc, char *argv[])
{
	int xs1, ys1;

	GfxFont_Init();
//	Con_Init();
//	ConCmds_Init();

	Con_ReInit();

	PDGLUI_Init();
	UI_Camera_Init();

	PDGL_Sound_Init();
	LBXGL_Particle_Init();

#if 1
	cam->raw=0;				//direct control
	cam->mgrab=0;			//mouse grab
//	cam->speed=25*12;		//25 ft/s / ~ 17MPH
	cam->speed=25*8;		//25 ft/s / ~ 17MPH
//	cam->speed=19*8;		//19 ft/s / ~ 13MPH
	cam->lockcfg=1;			//lock camera config
//	cam->org[3]=-40;		//move along W axis
#endif

#if 0
//	tile2d_player_x=0;
//	tile2d_player_y=5;
//	tile2d_player->org=vec2(0, 5);

	tile2d_map=Tex_LoadFileRaw("tile2d/map0.png",
		&tile2d_map_xs, &tile2d_map_ys);
	tile2d_map_ents=Tex_LoadFileRaw("tile2d/map0_ents.png",
		&xs1, &ys1);
	Tile2D_SpawnEntities();

	tile2d_player=Tile2D_AllocEntity();

	tile2d_player->next=tile2d_entity;
	tile2d_entity=tile2d_player;

	tile2d_player->mins=vec2(0.5, 0);
	tile2d_player->maxs=vec2(1.5, 2);
	tile2d_player->org=vec2(0, 5);

	tile2d_player->health=25;
	tile2d_player->flags|=TILE2D_EFL_TAKEDAMAGE;
	
	tile2d_player->solidtype=TILE2D_SOLIDTYPE_SLIDEBOX;
	tile2d_player->movetype=TILE2D_MOVETYPE_WALK;
	
//	tile2d_player->Draw=Tile2D_DrawEntity_Sprite;
//	tile2d_player->AnimTick=Tile2D_AnimTick_Bear;
//	tile2d_player->Update=Tile2D_Update_EntityMob;
	tile2d_player->Die=Tile2D_Entity_DieGeneric;
#endif

	return(1);
}

int main_handle_input()
{
	char buf[64];
	float mins[3], maxs[3];
	float io[3], iv[4], fw[2], pt[4], f, g, v;
	float im[16], im1[16], *imv;
	int imfl;

	int i, j, k;
	unsigned short *kcur;

	if(Con_IsDown())
		return(0);

	UI_Camera_Update();

	bt2dHandleInput();

#if 0
	imfl=0;
	kcur=state->keys;
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

//		if(state->mb&1)imfl|=BT_IMPULSE_FIRE;
//		if(state->mb&2)imfl|=BT_IMPULSE_ALTFIRE;

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
//		tile2d_player_x-=state->dt_f*1;
		if(v2x(tile2d_player->vel)>-10)
		{
			tile2d_player->vel=v2add(tile2d_player->vel,
				vec2(-50*state->dt_f, 0));
		}
	}
	if(PDGL_KeyDown(K_MOVERIGHT))
	{
//		tile2d_player_x+=state->dt_f*1;
		if(v2x(tile2d_player->vel)<10)
		{
			tile2d_player->vel=v2add(tile2d_player->vel,
				vec2(50*state->dt_f, 0));
		}
	}

	if(PDGL_KeyDown(K_MOVEUP))
	{
		f=1.0-(v2y(tile2d_player->vel)/5);
		if(f<0)f=0;
		if(f>1)f=1;

		tile2d_player->vel=v2add(tile2d_player->vel,
			vec2(0, f*50*state->dt_f));
		
//		if(v2y(tile2d_player->vel)<25)
//		{
//			tile2d_player->vel=v2add(tile2d_player->vel,
//				vec2(0, 25*state->dt_f));
//		}
//		tile2d_player_x+=state->dt_f*1;
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
#endif
}

int pdgl_main_body()
{
	static int t;
	char buf[64];

	int i, j, k;
	char *s;

	main_handle_input();

//	Tile2D_UpdateWorld();
	bt2dUpdateWorld(state->dt_f);

	UI_Camera_Setup3D();

	pdglDisable(GL_CULL_FACE);
	pdglDisable(GL_TEXTURE_2D);
	PD3D_DrawGrid(0, 0, 16, 1);

//	Tile2D_DrawWorld();
	bt2dDrawWorld();

#if 0
	pdglBegin(GL_LINES);

	pdglColor4f(1, 0, 0, 1);
	pdglVertex3f(0, 0, 0);
	pdglVertex3f(1000, 0, 0);

	pdglColor4f(0, 1, 0, 1);
	pdglVertex3f(0, 0, 0);
	pdglVertex3f(0, 1000, 0);

	pdglColor4f(0, 0, 1, 1);
	pdglVertex3f(0, 0, 0);
	pdglVertex3f(0, 0, 1000);

	pdglEnd();
#endif


	UI_Camera_Setup2D();
	Con_Render();

	pdglDisableTexture2D();
	pdglColor4f(1, 0, 0, 1);

	pdglBegin(GL_LINES);
	pdglVertex2f(state->mx-10, -state->my);
	pdglVertex2f(state->mx+10, -state->my);
	pdglVertex2f(state->mx, -state->my-10);
	pdglVertex2f(state->mx, -state->my+10);
	pdglEnd();

	return(0);
}

int pdgl_main_shutdown()
{
	printf("shutdown\n");
	return(0);
}
