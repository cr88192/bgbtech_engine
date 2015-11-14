//#include <windows.h>

//#include <GL/gl.h>

#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#define PDGL_BASEHEAP		(64*1024*1024)
#define PDGL_BASEHEAP_LIMIT	(256*1024*1024)
#define PDGL_BASEHEAP_LOBJ	(1024*1024*1024)

#define PDGL_PRESTART

// #include <pdlib.h>

#include <pdgl.h>
#include <pdglui.h>
#include <pdgl_stub.h>

#include <lbxgl2.h>
#include <libvecmath.h>

// #include <bgbasm.h>
// #include <bgbscr1.h>

#include <bgbasm.h>
#include <bgbccc.h>
#include <bgbsvm.h>

#ifndef MESH_API
#ifdef MSVC
#define MESH_API __declspec(dllexport)
#else
#define MESH_API
#endif
#endif

#define VEC_SZ			0.1

//script interface functions
int (*meshtool_handle_input_fp)();
int (*meshtool_draw3d_fp)();
int (*meshtool_draw2d_fp)();

int (*meshtool_menu_event_fp)(char *str);

LBXGL_API LBXGL_MeshGroup *lbxgl_mesh_mdl;
LBXGL_API LBXGL_MeshGroup *lbxgl_meshsel_grp;
LBXGL_API LBXGL_MeshPrim *lbxgl_meshsel_prim;
LBXGL_API int lbxgl_meshsel_face, lbxgl_meshsel_vec;
LBXGL_API int lbxgl_meshsel_multi;

LBXGL_API int lbxgl_mesh_mode;
LBXGL_API int lbxgl_mesh_cmd;
// LBXGL_API int lbxgl_mesh_loadsave;
// LBXGL_API char *lbxgl_mesh_file;

LBXGL_API LBXGL_MeshGroup *lbxgl_mesh_clip;

int main_loadsave;
char *main_file;

int ConCmds_Sky(char **a)
{
	LBXGL_Sky_LoadSky(a[1]);
	return(0);
}

char *games[16], *start_map;
int n_games;


int ConCmds_MemLog(char **a)
{
	if(!a[1])
	{
//		Con_Printf("memlog=%d\n", main_timestat);
		return(0);
	}

	gcDelayGC(atoi(a[1])?4:0);
	return(0);
}

int ConCmds_Eval(char **a)
{
	char tb[1024];
	char **ap;
	dyt p;
	char *s, *t;
	int i;
	
	//reverse the 'split' operation
	t=tb; ap=a+1;
	while(*ap)
	{
		s=*ap++;
		if(s[strlen(s)+1]=='\"')
		{
			//quoted strings don't transform escapes
			//so just re-emit the quoted string
			*t++=' ';
			*t++='\"';
			while(*s)*t++=*s++;
			*t++='\"';
			continue;
		}
		
		*t++=' ';
		while(*s)*t++=*s++;
	}
	*t++=';';	//for good measure
	*t++=';';
	*t++=0;

	dyPrintf("Eval: %s\n", tb);
	
	p=BSVM_EvalString(tb);
	dyPrint(p);
	dyPrintf("\n");
}


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
		
		sprintf(tb, "tool_%s", games[i]);
		dyllLoadLibrary(tb);
	}
#endif

	Con_Init();
	ConCmds_Init();

#if 1
	btCvarSetf("deathmatch", 0);
	btCvarSetf("coop", 0);
	btCvarSetf("sv_cheats", 0);
	btCvarSetf("cl_showentids", 0);
	btCvarSetf("r_lightbudget", 0);
	btCvarSetf("r_noearlyout", 0);
	btCvarSetf("r_nohires", 0);

	btCvarSet("r_title", "BGBTech MeshTool");
	btCvarSetf("r_width", xs);
	btCvarSetf("r_height", ys);

	btCvarSetf("r_usefbo", 1);
	btCvarSetf("r_usehdr", 1);
	btCvarSetf("r_usebloom", 0);
	btCvarSetf("r_parallax", 1);
	btCvarSetf("r_cellshade", 0);
#endif

	bsvmAddScriptPath("game");

	Con_ExecCmdBuf("game/default.cfg");
	Con_DoExecCmds();

	Con_ExecCmdBuf("game/config.cfg");
	Con_DoExecCmds();

//	Con_ExecCmdBuf("game/autoexec.cfg");

	s=btCvarGet("r_title");
	xs=btCvarGetf("r_width");
	ys=btCvarGetf("r_height");

	GfxDrv_SetDefaults(s, xs, ys);
//	Con_ReInit();

	return(1);
}

int pdgl_main_startup(int argc, char *argv[])
{
	char tb[256];
	void *p;
	int i, j, k, l;

//	PDGL_InitASM(argc, argv);
//	ccLoadModule("scripts/e_meshtool.c");

	GfxFont_Init();
//	Con_Init();
//	ConCmds_Init();

	Con_ReInit();

	PDGLUI_Init();
	UI_Camera_Init();

	ConCmds_Register("sky", "Set sky", ConCmds_Sky);

	ConCmds_Register("memlog", "Set Memlog", ConCmds_MemLog);

	ConCmds_Register("eval", "Eval Expression", ConCmds_Eval);
	ConCmds_Register("bs", "Eval Expression", ConCmds_Eval);
	ConCmds_Register(";", "Eval Expression", ConCmds_Eval);

	p=BASM_GetPtr("meshtool_init");
//	dyPrintf("Got %p\n", p);

	if(p)
	{
		((int(*)(int argc, char **argv))p)(argc, argv);
	}

	for(i=0; i<n_games; i++)
	{
		sprintf(tb, "meshtool_%s_init", games[i]);
		p=BASM_GetPtr(tb);
		if(p) ((int (*)())p)();
	}

	p=BASM_GetPtr("meshtool_handle_input");
	meshtool_handle_input_fp=(int (*)())p;

	p=BASM_GetPtr("meshtool_draw_3d");
	meshtool_draw3d_fp=(int (*)())p;

	p=BASM_GetPtr("meshtool_draw_2d");
	meshtool_draw2d_fp=(int (*)())p;

	p=BASM_GetPtr("meshtool_menu_event");
	meshtool_menu_event_fp=(int (*)(char *))p;



	PDGLUI_BeginMenu("menu");

	PDGLUI_BeginSubMenu("&File");
	PDGLUI_MenuItem("new",		"&New           (CTRL--N)");
	PDGLUI_MenuItem("open",		"&Open          (CTRL--O)");
	PDGLUI_MenuItem("save",		"&Save          (CTRL--S)");
	PDGLUI_MenuItem("save_as",	"Save &As");
	PDGLUI_EndSubMenu();

	PDGLUI_BeginSubMenu("&Edit");
	PDGLUI_MenuItem("edit_undo",	"Undo           (CTRL--Z)");
	PDGLUI_MenuItem("edit_redo",	"Redo           (CTRL--R)");
	PDGLUI_MenuItem(NULL, NULL);
	PDGLUI_MenuItem("edit_cut",	"Cut            (CTRL--X)");
	PDGLUI_MenuItem("edit_copy",	"Copy           (CTRL--C)");
	PDGLUI_MenuItem("edit_paste",	"Paste          (CTRL--V)");
	PDGLUI_MenuItem("edit_dup",	"Duplicate      (CTRL--D)");
	PDGLUI_EndSubMenu();

	PDGLUI_BeginSubMenu("Mode");
	PDGLUI_MenuItem("mode_group",	"Group          (G)");
	PDGLUI_MenuItem("mode_object",	"Object         (O)");
	PDGLUI_MenuItem("mode_face",	"Face           (F)");
	PDGLUI_MenuItem("mode_vertex",	"Vertex         (V)");
	PDGLUI_MenuItem("mode_texture",	"Texture        (SHIFT--T)");

	PDGLUI_MenuItem(NULL, NULL);

	PDGLUI_MenuItem("mode_persp",	"Perspective 3D (SHIFT--P)");
	PDGLUI_MenuItem("mode_ortho",	"Ortho 3D       (SHIFT--O)");
	PDGLUI_MenuItem("mode_orth_xy",	"Ortho XY       (SHIFT--X)");
	PDGLUI_MenuItem("mode_orth_xz",	"Ortho XZ       (SHIFT--Z)");
	PDGLUI_MenuItem("mode_orth_yz",	"Ortho YZ       (SHIFT--Y)");
	PDGLUI_MenuItem("mode_cad4v",	"4 Views        (SHIFT--Q)");
	PDGLUI_EndSubMenu();

	PDGLUI_BeginSubMenu("Tool");
	PDGLUI_MenuItem("tool_sel",	"Select         (S)");
	PDGLUI_MenuItem("tool_trans",	"Translate      (T)");
	PDGLUI_MenuItem("tool_rot",	"Rotate         (R)");
	PDGLUI_MenuItem("tool_scale",	"Scale          (SHIFT--S)");
	PDGLUI_MenuItem("tool_scale2",	"Scale HV       (SHIFT--A)");
	PDGLUI_EndSubMenu();

	PDGLUI_BeginSubMenu("Create");
	PDGLUI_MenuItem("create_cube",	"Cube");
	p=BASM_GetPtr("meshtool_menu_create");
	if(p) ((int (*)())p)();
	PDGLUI_EndSubMenu();

	PDGLUI_BeginSubMenu("Command");
	PDGLUI_MenuItem("cmd_delete", 	"Delete Selection        (DEL)");
	PDGLUI_MenuItem("cmd_subdiv",	"Subdivide Object//Faces  (SHIFT--D)");
	PDGLUI_MenuItem("cmd_extrude", 	"Extrude Face            (SHIFT--E)");
	PDGLUI_MenuItem("cmd_merge", 	"Merge Vertices//Objects  (SHIFT--M)");
	PDGLUI_MenuItem("cmd_invert", 	"Invert//Flip Normal      (SHIFT--N)");
	PDGLUI_MenuItem("cmd_settex",	"Set Texture             (CTRL--T)");

	p=BASM_GetPtr("meshtool_menu_command");
	if(p) ((int (*)())p)();
	PDGLUI_EndSubMenu();

	PDGLUI_BeginSubMenu("Texture");
//	PDGLUI_MenuItem("tex_set",	"Set Texture             (CTRL--T)");
	PDGLUI_MenuItem("cmd_settex",	"Set Texture             (CTRL--T)");
	PDGLUI_MenuItem("tex_plane",	"Planar Projection       (CTRL--SHIFT--P)");
	PDGLUI_MenuItem("tex_box",	"Box Projection          (CTRL--SHIFT--B)");
	PDGLUI_MenuItem("tex_sphere",	"Spherical Projection    (CTRL--SHIFT--S)");
	PDGLUI_MenuItem("tex_cylinder",	"Cylindrical Projection  (CTRL--SHIFT--C)");

	PDGLUI_MenuItem(NULL, NULL);

	PDGLUI_MenuItem("tex_proj_xy",	"Project Texture XY      ()");
	PDGLUI_MenuItem("tex_proj_xz",	"Project Texture XZ      ()");

	PDGLUI_MenuItem(NULL, NULL);
	p=BASM_GetPtr("meshtool_menu_texture");
	if(p) ((int (*)())p)();
	PDGLUI_EndSubMenu();

	p=BASM_GetPtr("meshtool_menu_top");
	if(p) ((int (*)())p)();

	PDGLUI_BeginSubMenu("Help");
	PDGLUI_MenuItem("help_generic", "General");
	PDGLUI_MenuItem("help_about", "About");
	PDGLUI_EndSubMenu();

#if 0
	PDGLUI_BeginSubMenu("Clear");
	PDGLUI_MenuItem("clear_all", "Clear All");
	PDGLUI_MenuItem("clear_bones", "Clear Bones");
	PDGLUI_MenuItem("clear_anim", "Clear Anim");
	PDGLUI_MenuItem("clear_mesh", "Clear Meshes");
	PDGLUI_EndSubMenu();

	PDGLUI_BeginSubMenu("Load");
	PDGLUI_MenuItem("load_bones", "Load &Bones");
	PDGLUI_MenuItem("load_anim", "Load &Anim");
	PDGLUI_MenuItem("load_model", "Load &Model");
	PDGLUI_MenuItem(NULL, NULL);
	PDGLUI_MenuItem("load_ac3d", "Import &AC3D");
	PDGLUI_MenuItem("load_smdref", "Import SMD Reference");
	PDGLUI_MenuItem("load_smdanim", "Import SMD Anim");
	PDGLUI_EndSubMenu();

	PDGLUI_BeginSubMenu("Save");
	PDGLUI_MenuItem("save_bones", "Save &Bones");
	PDGLUI_MenuItem("save_anim", "Save &Anim");
//	PDGLUI_MenuItem("save_model", "Save &Model");
//	PDGLUI_MenuItem("save_ac3d", "Save &AC3D");
	PDGLUI_EndSubMenu();

	PDGLUI_BeginSubMenu("Command");
	PDGLUI_MenuItem("cmd_rebind", "Rebind Meshes");
	PDGLUI_MenuItem("cmd_unbind", "Unbind Meshes");
	PDGLUI_EndSubMenu();

	PDGLUI_BeginSubMenu("Help");
	PDGLUI_MenuItem("help_generic", "General");
	PDGLUI_MenuItem("help_about", "About");
	PDGLUI_EndSubMenu();
#endif

	PDGLUI_EndMenu();

//	lbxgl_mesh_mdl=LBXGL_AC3D_LoadModel("model/monitor3.ac");
	lbxgl_mesh_mdl=gctalloc("lbxgl_meshgroup_t", sizeof(LBXGL_MeshGroup));

	UI_TexFinder_SetTitle("Textures");

	Con_ExecCmdBuf("game/meshtool.cfg");

//	cam->mode=PDGLUI_CAM_PERSP;
	cam->mode=PDGLUI_CAM_CAD4V;

	return(1);
}

int main_handle_input()
{
	char buf[64];
	float mins[3], maxs[3];
	float io[3], iv[4], fw[2], pt[4], f, g, v;
	float im[16], im1[16], *imv;

	int i, j, k;
	unsigned short *kcur;
	char *s;

	if(Con_IsDown() || main_loadsave)
		return(0);

	if(PDGLUI_HasFocus())
		return(0);

//	if(main_dirty && (pdgl_time>(main_lutime+500)))
//		LBXGL_Mesh_AddUndo();

//	i=((int (*)())BASM_GetFPtrDummy("meshtool_handle_input"))();
//	if(i>0)return(0);

	if(meshtool_handle_input_fp)
	{
		i=meshtool_handle_input_fp();
		if(i>0)return(0);
	}

	UI_Camera_Update();

	if(state->mb&4)
	{
		PDGLUI_ShowMenu("menu", state->mx, -state->my);
		return(0);
	}

	s=PDGLUI_GetMenuSelection("menu");
	if(s)
	{
		if(meshtool_menu_event_fp)
		{
			i=meshtool_menu_event_fp(s);
			if(i>0)return(0);
		}

		if(!strcmp(s, "new"))
		{
			lbxgl_mesh_mdl=gctalloc("lbxgl_meshgroup_t",
				sizeof(LBXGL_MeshGroup));
		}

		if(!strcmp(s, "open"))
		{
			UI_LoadSave_SetTitle("Open", "ac;acx");
			main_loadsave=1;
		}
		if(!strcmp(s, "save_as"))
		{
			UI_LoadSave_SetTitle("Save As", "ac;acx");
			main_loadsave=2;
		}
		if(!strcmp(s, "cmd_settex"))
		{
			UI_TexFinder_SetTitle("Textures");
			main_loadsave=16;
		}

		if(!strcmp(s, "edit_undo"))LBXGL_Mesh_Undo();
		if(!strcmp(s, "edit_redo"))LBXGL_Mesh_Redo();

		if(!strcmp(s, "edit_cut"))LBXGL_Mesh_CmdSel(LBXGL_EDCMD_CUT);
		if(!strcmp(s, "edit_copy"))LBXGL_Mesh_CmdSel(LBXGL_EDCMD_COPY);
		if(!strcmp(s, "edit_paste"))LBXGL_Mesh_Paste();
		if(!strcmp(s, "edit_dup"))LBXGL_Mesh_CmdSel(LBXGL_EDCMD_DUPLICATE);

		if(!strcmp(s, "mode_group"))
			{ lbxgl_mesh_mode=LBXGL_EDMODE_GROUP; LBXGL_Mesh_ClearSel(); }
//		if(!strcmp(s, "mode_object"))
//			{ lbxgl_mesh_mode=LBXGL_EDMODE_OBJECT; LBXGL_Mesh_ClearSel(); }
		if(!strcmp(s, "mode_face"))
			{ lbxgl_mesh_mode=LBXGL_EDMODE_FACE; LBXGL_Mesh_ClearSel(); }
		if(!strcmp(s, "mode_vertex"))
			{ lbxgl_mesh_mode=LBXGL_EDMODE_VERTEX; LBXGL_Mesh_ClearSel(); }

		if(!strcmp(s, "mode_object"))
		{
			if((lbxgl_mesh_mode!=LBXGL_EDMODE_OBJECT) &&
				(lbxgl_mesh_mode!=LBXGL_EDMODE_TEXTURE))
					LBXGL_Mesh_ClearSel();
			lbxgl_mesh_mode=LBXGL_EDMODE_OBJECT;
		}
		if(!strcmp(s, "mode_texture"))
		{
			if((lbxgl_mesh_mode!=LBXGL_EDMODE_OBJECT) &&
				(lbxgl_mesh_mode!=LBXGL_EDMODE_TEXTURE))
					LBXGL_Mesh_ClearSel();
			lbxgl_mesh_mode=LBXGL_EDMODE_TEXTURE;
		}

		if(!strcmp(s, "mode_persp"))cam->mode=PDGLUI_CAM_PERSP;
		if(!strcmp(s, "mode_ortho"))cam->mode=PDGLUI_CAM_ORTHO;
		if(!strcmp(s, "mode_orth_xy"))cam->mode=PDGLUI_CAM_XY;
		if(!strcmp(s, "mode_orth_xz"))cam->mode=PDGLUI_CAM_XZ;
		if(!strcmp(s, "mode_orth_yz"))cam->mode=PDGLUI_CAM_YZ;
		if(!strcmp(s, "mode_cad4v"))cam->mode=PDGLUI_CAM_CAD4V;

		if(!strcmp(s, "tool_sel"))lbxgl_mesh_cmd=LBXGL_EDCMD_SEL;
		if(!strcmp(s, "tool_trans"))lbxgl_mesh_cmd=LBXGL_EDCMD_TRANS;
		if(!strcmp(s, "tool_rot"))lbxgl_mesh_cmd=LBXGL_EDCMD_ROT;
		if(!strcmp(s, "tool_scale"))lbxgl_mesh_cmd=LBXGL_EDCMD_SCALE;
		if(!strcmp(s, "tool_scale2"))lbxgl_mesh_cmd=LBXGL_EDCMD_SCALE2;

		if(!strcmp(s, "create_cube"))
		{
			LBXGL_Mesh_AddUndo();
			LBXGL_Mesh_CreateCube();
		}

		if(!strcmp(s, "cmd_delete"))LBXGL_Mesh_CmdSel(LBXGL_EDCMD_DELETE);
		if(!strcmp(s, "cmd_extrude"))LBXGL_Mesh_CmdSel(LBXGL_EDCMD_EXTRUDE);
		if(!strcmp(s, "cmd_subdiv"))LBXGL_Mesh_CmdSel(LBXGL_EDCMD_SUBDIVIDE);
		if(!strcmp(s, "cmd_merge"))LBXGL_Mesh_CmdSel(LBXGL_EDCMD_MERGE);
		if(!strcmp(s, "cmd_invert"))LBXGL_Mesh_CmdSel(LBXGL_EDCMD_INVERT);

		if(!strcmp(s, "tex_plane") && lbxgl_meshsel_prim)
			lbxgl_meshsel_prim->tex_mode=LBXGL_TEX_PLANAR;
		if(!strcmp(s, "tex_box") && lbxgl_meshsel_prim)
			lbxgl_meshsel_prim->tex_mode=LBXGL_TEX_BOX;
		if(!strcmp(s, "tex_sphere") && lbxgl_meshsel_prim)
			lbxgl_meshsel_prim->tex_mode=LBXGL_TEX_SPHERICAL;
		if(!strcmp(s, "tex_cylinder") && lbxgl_meshsel_prim)
			lbxgl_meshsel_prim->tex_mode=LBXGL_TEX_CYLINDRICAL;

		if(!strcmp(s, "tex_proj_xy") && lbxgl_meshsel_prim)
			LBXGL_Mesh_ReprojectPrimXY(lbxgl_meshsel_prim);
		if(!strcmp(s, "tex_proj_xz") && lbxgl_meshsel_prim)
			LBXGL_Mesh_ReprojectPrimXZ(lbxgl_meshsel_prim);

		return(0);
	}


	kcur=state->keys;
	while(*kcur)
	{
		if(PDGL_KeyDown(K_CTRL))
		{
			if(*kcur=='z')LBXGL_Mesh_Undo();
			if(*kcur=='r')LBXGL_Mesh_Redo();

			if(*kcur=='c')LBXGL_Mesh_CmdSel(LBXGL_EDCMD_COPY);
			if(*kcur=='x')LBXGL_Mesh_CmdSel(LBXGL_EDCMD_CUT);
			if(*kcur=='d')LBXGL_Mesh_CmdSel(LBXGL_EDCMD_DUPLICATE);
			if(*kcur=='v')LBXGL_Mesh_Paste();

			if(*kcur=='o')
			{
				UI_LoadSave_SetTitle("Open", "ac;acx");
				main_loadsave=1;
			}

			if(*kcur=='s')
			{
				if(!main_file)
				{
					UI_LoadSave_SetTitle("Save As", "ac;acx");
					main_loadsave=2;
				}else
				{
//					LBXGL_AC3D_SaveModel(lbxgl_mesh_mdl, main_file);
					LBXGL_Mesh_SaveModel(main_file, lbxgl_mesh_mdl);
				}
			}

			if(*kcur=='t')
			{
				UI_TexFinder_SetTitle("Textures");
				main_loadsave=16;
			}

			if(*kcur=='P' && lbxgl_meshsel_prim)
				lbxgl_meshsel_prim->tex_mode=LBXGL_TEX_PLANAR;
			if(*kcur=='B' && lbxgl_meshsel_prim)
				lbxgl_meshsel_prim->tex_mode=LBXGL_TEX_BOX;
			if(*kcur=='S' && lbxgl_meshsel_prim)
				lbxgl_meshsel_prim->tex_mode=LBXGL_TEX_SPHERICAL;
			if(*kcur=='C' && lbxgl_meshsel_prim)
				lbxgl_meshsel_prim->tex_mode=LBXGL_TEX_CYLINDRICAL;

			kcur++;
			continue;
		}

		if(*kcur=='g') { lbxgl_mesh_mode=LBXGL_EDMODE_GROUP; LBXGL_Mesh_ClearSel(); }
		if(*kcur=='f') { lbxgl_mesh_mode=LBXGL_EDMODE_FACE; LBXGL_Mesh_ClearSel(); }
		if(*kcur=='v') { lbxgl_mesh_mode=LBXGL_EDMODE_VERTEX; LBXGL_Mesh_ClearSel(); }

		if(*kcur=='o')
		{
			if((lbxgl_mesh_mode!=LBXGL_EDMODE_OBJECT) &&
				(lbxgl_mesh_mode!=LBXGL_EDMODE_TEXTURE))
					LBXGL_Mesh_ClearSel();
			lbxgl_mesh_mode=LBXGL_EDMODE_OBJECT;
		}
		if(*kcur=='T')
		{
			if((lbxgl_mesh_mode!=LBXGL_EDMODE_OBJECT) &&
				(lbxgl_mesh_mode!=LBXGL_EDMODE_TEXTURE))
					LBXGL_Mesh_ClearSel();
			lbxgl_mesh_mode=LBXGL_EDMODE_TEXTURE;
		}

		if(*kcur=='s')lbxgl_mesh_cmd=LBXGL_EDCMD_SEL;
		if(*kcur=='t')lbxgl_mesh_cmd=LBXGL_EDCMD_TRANS;
		if(*kcur=='r')lbxgl_mesh_cmd=LBXGL_EDCMD_ROT;
		if(*kcur=='S')lbxgl_mesh_cmd=LBXGL_EDCMD_SCALE;
		if(*kcur=='A')lbxgl_mesh_cmd=LBXGL_EDCMD_SCALE2;

		if(*kcur=='P')cam->mode=PDGLUI_CAM_PERSP;
		if(*kcur=='O')cam->mode=PDGLUI_CAM_ORTHO;
		if(*kcur=='X')cam->mode=PDGLUI_CAM_XY;
		if(*kcur=='Z')cam->mode=PDGLUI_CAM_XZ;
		if(*kcur=='Y')cam->mode=PDGLUI_CAM_YZ;
		if(*kcur=='Q')cam->mode=PDGLUI_CAM_CAD4V;


		if(*kcur==K_ENTER)
		{
			if(!PDGL_KeyDown(K_SHIFT) && lbxgl_meshsel_multi)
				LBXGL_Mesh_ClearSel();
			LBXGL_Mesh_SelGroup(lbxgl_mesh_mdl);
		}

		if(*kcur==K_INS)
		{
			if(PDGL_KeyDown(K_SHIFT))
				LBXGL_Mesh_Paste();
		}

		if(*kcur==K_DEL)LBXGL_Mesh_CmdSel(LBXGL_EDCMD_DELETE);
		if(*kcur=='E')LBXGL_Mesh_CmdSel(LBXGL_EDCMD_EXTRUDE);
		if(*kcur=='D')LBXGL_Mesh_CmdSel(LBXGL_EDCMD_SUBDIVIDE);
		if(*kcur=='M')LBXGL_Mesh_CmdSel(LBXGL_EDCMD_MERGE);
		if(*kcur=='N')LBXGL_Mesh_CmdSel(LBXGL_EDCMD_INVERT);

		if((lbxgl_mesh_mode==LBXGL_EDMODE_VERTEX) && (*kcur=='F'))
			LBXGL_Mesh_CreateFace(lbxgl_meshsel_prim);

		kcur++;
	}

	if(PDGL_KeyDown(K_CTRL))
	{
		if(!(state->mb&1) && (state->lmb&1))
		{
			i=(state->mx>state->omx)?(state->mx-state->omx):(state->omx-state->mx);
			j=(state->my>state->omy)?(state->my-state->omy):(state->omy-state->my);

			if((i>5) && (j>5))
			{
//				dyPrintf("Sel Box\n");
				if(!PDGL_KeyDown(K_SHIFT))
					LBXGL_Mesh_ClearSel();
				LBXGL_Mesh_SelGroupBox(lbxgl_mesh_mdl);
				return(0);
			}
		}

		return(0);
	}

	if((lbxgl_mesh_cmd==LBXGL_EDCMD_SEL) && (state->mb&1) && !(state->lmb&1))
	{
		if(!PDGL_KeyDown(K_SHIFT) && lbxgl_meshsel_multi)
			LBXGL_Mesh_ClearSel();
		LBXGL_Mesh_SelGroup(lbxgl_mesh_mdl);
		return(0);
	}

	if((lbxgl_mesh_cmd==LBXGL_EDCMD_SEL) && !(state->mb&1) && (state->lmb&1))
	{
		i=(state->mx>state->omx)?(state->mx-state->omx):(state->omx-state->mx);
		j=(state->my>state->omy)?(state->my-state->omy):(state->omy-state->my);

		if((i>5) && (j>5))
		{
//			dyPrintf("Sel Box\n");
			if(!PDGL_KeyDown(K_SHIFT))
				LBXGL_Mesh_ClearSel();
			LBXGL_Mesh_SelGroupBox(lbxgl_mesh_mdl);
			return(0);
		}
	}

	if((lbxgl_mesh_cmd!=LBXGL_EDCMD_SEL) && (state->mb&1))
	{
//		LBXGL_Mesh_CmdSelGroup(lbxgl_mesh_mdl, lbxgl_mesh_cmd);
		LBXGL_Mesh_CmdSel(lbxgl_mesh_cmd);
		return(0);
	}

	return(0);
}

// void PD3D_DrawXYGrid2(float w, float s);
// void PD3D_DrawXZGrid2(float w, float s);
// void PD3D_DrawYZGrid2(float w, float s);

int LBXGL_Mesh_DrawScene()
{
	static int t;
	float pt[4];
	char buf[64];

	int i, j, k;
	char *s;

	if(!cam->ortho)
		LBXGL_Sky_DrawSky();

#ifndef GLES
	glDisable(GL_CULL_FACE);
	glDisable(GL_TEXTURE_2D);
	glDisable(GL_LIGHTING);
	glDisable(GL_COLOR_MATERIAL);

//	i=1;
//	if(cam->width>=(4*12))i=12;

	i=0;
	if(cam->width<=(4*12))i=1;
	if(cam->width<=4)i=2;

	glDisable(GL_TEXTURE_2D);
//	glDisable(GL_DEPTH_TEST);

//	glDepthFunc(GL_LEQUAL);
	glDepthFunc(GL_ALWAYS);

//	switch(cam->mode)
	switch(cam->ortho)
	{
	case 1:
	case 2:
		glColor4f(0.25, 0.25, 0.25, 1);
		if(i>1)PD3D_DrawXYGrid2(256, 1.0/16);
		glColor4f(0.5, 0.5, 0.5, 1);
		if(i)PD3D_DrawXYGrid2(256, 1);
		glColor4f(0.75, 0.75, 0.75, 1);
		PD3D_DrawXYGrid2(256, 12);
		break;
	case 3:
		glColor4f(0.25, 0.25, 0.25, 1);
		if(i>1)PD3D_DrawXZGrid2(256, 1.0/16);
		glColor4f(0.5, 0.5, 0.5, 1);
		if(i)PD3D_DrawXZGrid2(256, 1);
		glColor4f(0.75, 0.75, 0.75, 1);
		PD3D_DrawXZGrid2(256, 12);
		break;
	case 4:
		glColor4f(0.25, 0.25, 0.25, 1);
		if(i>1)PD3D_DrawYZGrid2(256, 1.0/16);
		glColor4f(0.5, 0.5, 0.5, 1);
		if(i)PD3D_DrawYZGrid2(256, 1);
		glColor4f(0.75, 0.75, 0.75, 1);
		PD3D_DrawYZGrid2(256, 12);
		break;

	default:
#if 1
//		PD3D_DrawGrid(0, 0, 16, 1);

		if(fabs(cam->org[2])<50)
		{
			glColor4f(0.5, 0.5, 0.5, 1);
			PD3D_DrawXYGrid2(256, 1);
		}

		glColor4f(0.75, 0.75, 0.75, 1);
		PD3D_DrawXYGrid2(256, 12);

//		glColor4f(0.75, 0.5, 0.5, 1);
//		PD3D_DrawXYGrid2(256, 10);
#endif
		break;
	}

	glDepthFunc(GL_LEQUAL);
//	glEnable(GL_DEPTH_TEST);


	glColor4f(1, 1, 1, 1);
	glDisable(GL_CULL_FACE);

	glEnable(GL_LIGHTING);
	glEnable(GL_NORMALIZE);

	glLightModeli(GL_LIGHT_MODEL_TWO_SIDE, GL_TRUE);

	if(lbxgl_mesh_mode!=LBXGL_EDMODE_FACE)
	{
		pt[0]=1; pt[1]=1; pt[2]=1; pt[3]=1;
		glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, pt);

//		glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, pt);
	}else
	{
		pt[0]=1; pt[1]=1; pt[2]=1; pt[3]=1;
		glMaterialfv(GL_FRONT, GL_DIFFUSE, pt);

//		glMaterialfv(GL_FRONT, GL_SPECULAR, pt);

		pt[0]=0.5; pt[1]=0.5; pt[2]=0.25; pt[3]=1;
		glMaterialfv(GL_BACK, GL_DIFFUSE, pt);
//		glMaterialfv(GL_BACK, GL_SPECULAR, pt);
	}

	UI_Camera_GetOrgFV(pt);
	pt[3]=1;
	glLightfv(GL_LIGHT0, GL_POSITION, pt);
	glLightf(GL_LIGHT0, GL_QUADRATIC_ATTENUATION, 1.0/1000.0);
	glEnable(GL_LIGHT0);

	LBXGL_Mesh_DrawGroupEdit(lbxgl_mesh_mdl);

	if(meshtool_draw3d_fp)
		meshtool_draw3d_fp();

	glDisable(GL_LIGHTING);

	LBXGL_Particle_Step(state->dt_f);
	LBXGL_Particle_Draw();

	LBXGL_Shader_FlushState();
#endif

//	return(-1);
}

int pdgl_main_body()
{
	static int t;
	float pt[4];
	char buf[64];

	int i, j, k;
	char *s;

	Con_Update(state->dt_f);
	main_handle_input();
	thDoWorkMS(10);

	if(cam->mode==PDGLUI_CAM_PERSP2)
		cam->mode=PDGLUI_CAM_PERSP;

	if(UI_Camera_4ViewsP())
	{
		UI_Camera_Setup3D_A();
		LBXGL_Mesh_DrawScene();
		UI_Camera_Setup3D_B();
		LBXGL_Mesh_DrawScene();
		UI_Camera_Setup3D_C();
		LBXGL_Mesh_DrawScene();
		UI_Camera_Setup3D_D();
		LBXGL_Mesh_DrawScene();
	}else
	{
		UI_Camera_Setup3D();
		LBXGL_Mesh_DrawScene();
	}

	UI_Camera_Setup2D();

	Con_Render();

	GfxFont_SetFont("fixed", 0);

#if 1
	GfxFont_SetFont("fixed", 0);

	sprintf(buf, "(%.3f %.3f %.3f) (%g %g %g)",
		cam->org[0], cam->org[1], cam->org[2],
		cam->ang[0], cam->ang[1], cam->ang[2]);
	GfxFont_DrawString(buf, -400, 300-12,
		8, 8,  0, 255, 0, 255);

	sprintf(buf, "(%.3f %.3f %.3f) (%.3f %.3f %.3f)",
		cam->morg[0], cam->morg[1], cam->morg[2],
		cam->mdir[0], cam->mdir[1], cam->mdir[2]);
	GfxFont_DrawString(buf, -400, 300-12-8,
		8, 8,  0, 255, 0, 255);

	strcpy(buf, "");
	if(lbxgl_mesh_mode==LBXGL_EDMODE_GROUP)strcpy(buf, "Group");
	if(lbxgl_mesh_mode==LBXGL_EDMODE_OBJECT)strcpy(buf, "Object");
	if(lbxgl_mesh_mode==LBXGL_EDMODE_FACE)strcpy(buf, "Face");
	if(lbxgl_mesh_mode==LBXGL_EDMODE_VERTEX)strcpy(buf, "Vertex");
	if(lbxgl_mesh_mode==LBXGL_EDMODE_TEXTURE)strcpy(buf, "Texture");
	GfxFont_DrawString(buf, 400-(16*8), 300-12,
		8, 8,  0, 255, 0, 255);

	strcpy(buf, "");
	if(lbxgl_mesh_cmd==LBXGL_EDCMD_SEL)strcpy(buf, "Select");
	if(lbxgl_mesh_cmd==LBXGL_EDCMD_TRANS)strcpy(buf, "Translate");
	if(lbxgl_mesh_cmd==LBXGL_EDCMD_ROT)strcpy(buf, "Rotate");
	if(lbxgl_mesh_cmd==LBXGL_EDCMD_SCALE)strcpy(buf, "Scale");
	if(lbxgl_mesh_cmd==LBXGL_EDCMD_SCALE2)strcpy(buf, "Scale-HV");
	GfxFont_DrawString(buf, 400-(16*8), 300-12-8,
		8, 8,  0, 255, 0, 255);

	if(cam->mode==0)strcpy(buf, "Perspective 3D");
	if(cam->mode==1)strcpy(buf, "Ortho 3D");
	if(cam->mode==2)strcpy(buf, "Ortho XY");
	if(cam->mode==3)strcpy(buf, "Ortho XZ");
	if(cam->mode==4)strcpy(buf, "Ortho YZ");
	if(cam->mode==5)strcpy(buf, "Perspective 2");
	if(cam->mode==6)strcpy(buf, "4D");
	if(cam->mode==7)strcpy(buf, "4 Views");
	GfxFont_DrawString(buf, 400-(16*8), 300-12-16,
		8, 8,  0, 255, 0, 255);
#endif


	if(meshtool_draw2d_fp)
		meshtool_draw2d_fp();

	if(main_loadsave && (main_loadsave<16))
	{
		UI_LoadSave_Draw();
		s=UI_LoadSave_GetName();

		if(s)
		{
			if(*s && (main_loadsave==1))
			{
				main_file=dystrdup(s);
//				lbxgl_mesh_mdl=LBXGL_AC3D_LoadModel(s);
				lbxgl_mesh_mdl=LBXGL_Mesh_LoadModel(s);
			}
			if(*s && (main_loadsave==2))
			{
				main_file=dystrdup(s);
//				LBXGL_AC3D_SaveModel(lbxgl_mesh_mdl, s);
				LBXGL_Mesh_SaveModel(s, lbxgl_mesh_mdl);
			}
			main_loadsave=0;
		}
	}

	if(main_loadsave && (main_loadsave>=16))
	{
		UI_TexFinder_Draw();

		s=UI_TexFinder_GetName();
		if(s)
		{
			main_loadsave=0;
			if(lbxgl_meshsel_prim)
			{
				sprintf(buf, "textures/%s", s);
//				dyPrintf("Set Tex Prim %s\n", buf);

//				lbxgl_meshsel_prim->tex=dystring(buf);
//				lbxgl_meshsel_prim->texnum=Tex_LoadFile(buf, NULL, NULL);

				lbxgl_meshsel_prim->tex=dystrdup(buf);
				lbxgl_meshsel_prim->texnum=LBXGL_Texture_LoadImage(buf);

				dyPrintf("Set Tex Prim %s %d\n", buf, lbxgl_meshsel_prim->texnum);
			}
		}
	}


	PDGLUI_Frame();

	pdglDisableTexture2D();
	pdglColor4f(1, 0, 0, 1);

	pdglBegin(GL_LINES);
	pdglVertex2f(state->mx-10, -state->my);
	pdglVertex2f(state->mx+10, -state->my);
	pdglVertex2f(state->mx, -state->my-10);
	pdglVertex2f(state->mx, -state->my+10);
	pdglEnd();

	if(state->mb&1)
	{
		if((lbxgl_mesh_cmd==LBXGL_EDCMD_SEL) || PDGL_KeyDown(K_CTRL))
		{
			pdglColor4f(0.85, 0.85, 0.85, 1);

			pdglBegin(PDGL_LINES);
			pdglVertex2f(state->omx, -state->omy);
			pdglVertex2f(state->mx, -state->omy);

			pdglVertex2f(state->omx, -state->my);
			pdglVertex2f(state->mx, -state->my);

			pdglVertex2f(state->omx, -state->omy);
			pdglVertex2f(state->omx, -state->my);

			pdglVertex2f(state->mx, -state->omy);
			pdglVertex2f(state->mx, -state->my);
			pdglEnd();
		}else
		{
			pdglColor4f(0.85, 0.85, 0.85, 1);

			pdglBegin(PDGL_LINES);
			pdglVertex2f(state->omx, -state->omy);
			pdglVertex2f(state->mx, -state->my);
			pdglEnd();
		}
	}

	return(0);
}

int pdgl_main_shutdown()
{
	dyPrintf("shutdown\n");
	return(0);
}
