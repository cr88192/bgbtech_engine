#include <stdio.h>
#include <stdlib.h>
#include <math.h>
// #include <unistd.h>

#ifndef M_PI
#define M_PI		3.1415926535897932384626433832795
#endif

#define PDGL_BASEHEAP		(64*1024*1024)
#define PDGL_BASEHEAP_LIMIT	(256*1024*1024)
#define PDGL_BASEHEAP_LOBJ	(1024*1024*1024)

#define PDGL_PRESTART

// #include <pdlib.h>

#include <pdgl.h>
#include <pdglui.h>
#include <pdgl_stub.h>

// #ifdef _WIN32
// #include <windows.h>
// #include <GL/gl.h>
// #endif

#include <lbxgl2.h>
#include <libvecmath.h>

#include <bgbasm.h>
#include <bgbccc.h>
#include <bgbsvm.h>


float main_cam_org[3];
float main_cam_ang[3];
float main_cam_vel[3];
int main_cam_id;
int main_cam_raw=1;

float main_time;
float main_speed=12;

float main_cam_fw[3];
float main_cam_rt[3];
float main_cam_up[3];

float main_morg[3];
float main_mdir[3];


LBXGL_Skel2State *main_skel;

char *bones_file;
char *model_file;
char *frame_file;

int main_loadsave;
int main_mode;
int main_lmode;
int main_flushui;

//int state->doshot;		//do a screen shot
int main_showbones=1;		//render bones
int main_showsolids=0;		//render solids

char *games[16], *start_map;
int n_games;

#define MODE_BONES	1
#define MODE_ANIMATE	2
#define MODE_MESH	3
#define MODE_SOLID	4

#define MODE_FIRST	1
#define MODE_LAST	4


int main_load_rootmesh(char *name)
{
//	Skel_FlushInstance(main_skel);

//	main_skel->mdl->mesh=AC3D_LoadModel(name);
	Skel_LoadMesh(main_skel->mdl, name);
//	if(main_skel->mdl->num_bones)
//		Skel_BindMeshGroup(main_skel, main_skel->mdl->mesh);
}

int main_draw_rootmesh()
{
	main_skel->frame_calc=0;

	if(!main_skel->mdl->mesh)return(0);
//	Skel_DrawMeshGroup(main_skel, main_skel->mdl->mesh);
	Skel_DrawModel(main_skel);
}


int main_load_mesh(int num, char *name, float *pos)
{
	float tv[16];

	if(!stricmp(vfgetext(name), "ac"))
	{
		Mat4F_Copy(pos, tv);
		V4F_COPY(pos+0*4, tv+1*4);
		V4F_COPY(pos+1*4, tv+0*4);

		main_skel->mdl->mesh_grp[num]=
			AC3D_LoadModelPos(name, tv);
		return(0);
	}
}

void main_draw_star(float *org)
{
	float v[3];
	int i;

	for(i=1; i<27; i++)
	{
		v[0]=((i/1)%3)?((((i/1)%3)==1)?1:-1):0;
		v[1]=((i/3)%3)?((((i/3)%3)==1)?1:-1):0;
		v[2]=((i/9)%3)?((((i/9)%3)==1)?1:-1):0;
		V3F_NORMALIZE(v, v);

		pdglVertex3f(org[0]-v[0]*0.5, org[1]-v[1]*0.5, org[2]-v[2]*0.5);
		pdglVertex3f(org[0]+v[0]*0.5, org[1]+v[1]*0.5, org[2]+v[2]*0.5);
	}
}


void main_draw_mesh(int num, float *org, float *rot)
{
	float im[16];
	float tv[3], tv1[3], f;
	int i, j, k;

	if(main_skel->mdl->mesh_grp[num])
	{
		Mat4F_Identity(im);
		im[0]=rot[0];  im[1]=rot[1];  im[2]=rot[2];
		im[4]=rot[3];  im[5]=rot[4];  im[6]=rot[5];
		im[8]=rot[6];  im[9]=rot[7];  im[10]=rot[8];
		im[12]=org[0]; im[13]=org[1]; im[14]=org[2];

		f=V3F_DIST(org, main_cam_org)/12.0;
		if(f<1)f=1;

		pdglColor4f(1.0/f, 1.0/f, 1.0/f, 1);

		if((main_mode==MODE_MESH) && (num==main_skel->bone_sel))
			pdglColor4f(0.5/f, 1.0/f, 0.5/f, 1);

		AC3D_DrawMeshGroup(main_skel->mdl->mesh_grp[num], im);

		return;
	}
}

char *Path_GenAbs(char *name)
{
	char buf[256], buf1[256];
	char *s;

	s=name;
	if(*s=='/')return(name);
	if(*s=='\\')return(name);

	while(*s && (*s!='/') && (*s!='\\'))s++;
	if(*s==':')return(name);

	getcwd(buf, 255);
	sprintf(buf1, "%s/%s", buf, name);
	name=strdup(buf1);

	return(name);
}

int ConCmds_RenameBone(char **a)
{
	int i;

	for(i=0; i<main_skel->mdl->num_bones; i++)
		if(main_skel->mdl->bone_name[i] && !strcmp(main_skel->mdl->bone_name[i], a[1]))
			break;
	if(i>=main_skel->mdl->num_bones)
	{
		Con_Printf("Unknown Bone '%s'\n", a[1]);
		return(0);
	}

	main_skel->mdl->bone_name[i]=strdup(a[2]);

	return(0);
}


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

	btCvarSet("r_title", "BGBTech BoneTool");
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
	float v[3];
	char *opts[4];
	int i, j, k, l;
	void *p;
	char *s;

	GfxFont_Init();
//	Con_Init();
//	ConCmds_Init();

	Con_ReInit();

	PDGLUI_Init();

	ConCmds_Register("rename-bone", "Rename a Bone", ConCmds_RenameBone);

	ConCmds_Register("memlog", "Set Memlog", ConCmds_MemLog);

	ConCmds_Register("eval", "Eval Expression", ConCmds_Eval);
	ConCmds_Register("bs", "Eval Expression", ConCmds_Eval);
	ConCmds_Register(";", "Eval Expression", ConCmds_Eval);

	p=BASM_GetPtr("bonetool_init");
//	dyPrintf("Got %p\n", p);

	if(p)
	{
		((int(*)(int argc, char **argv))p)(argc, argv);
	}

	for(i=0; i<n_games; i++)
	{
		sprintf(tb, "bonetool_%s_init", games[i]);
		p=BASM_GetPtr(tb);
		if(p) ((int (*)())p)();
	}

	UI_LoadSave_CheckDrives();
	UI_LoadSave_SetTitle("Test", NULL);

	main_cam_org[0]=0;
	main_cam_org[1]=-10;
	main_cam_org[2]=5;

	main_cam_ang[0]=90;
	main_cam_ang[1]=0;
	main_cam_ang[2]=0;


	bones_file=NULL;
	model_file=NULL;
	frame_file=NULL;

	main_skel=gctalloc("lbxgl_skel2state_t", sizeof(LBXGL_Skel2State));
	main_skel->bone_rorg=gcalloc(64*3*sizeof(float));
	main_skel->bone_rmat=gcalloc(64*9*sizeof(float));

	for(i=0; i<64; i++)
		Mat3F_Identity(main_skel->bone_rmat+i*9);

	main_skel->mdl=gctalloc("lbxgl_skel2model_t",
		sizeof(LBXGL_Skel2Model));

	main_skel->anim=gcalloc(sizeof(LBXGL_Skel2Anim));
	main_skel->anim->frame_org=gcalloc(64*256*3*sizeof(float));
	main_skel->anim->frame_rot=gcalloc(64*256*9*sizeof(float));
	main_skel->anim->frame_mask=gcalloc(64*256/8);

#if 1
	main_skel->mdl->bone_name=gcalloc(64*sizeof(char *));
	main_skel->mdl->bone_up=gcalloc(64*sizeof(int));
	main_skel->mdl->bone_org=gcalloc(64*3*sizeof(float));
	main_skel->mdl->bone_rot=gcalloc(64*9*sizeof(float));
	main_skel->mdl->bone_ang=gcalloc(64*3*sizeof(float));

	main_skel->mdl->bone_mins=gcalloc(64*3*sizeof(float));
	main_skel->mdl->bone_maxs=gcalloc(64*3*sizeof(float));

	main_skel->mdl->max_bones=64;
#endif

	main_skel->cur_xyz=gcalloc(65536*3*sizeof(float));
	main_skel->cur_norm=gcalloc(65536*4*sizeof(float));
	main_skel->cur_vnorm=gcalloc(65536*3*4*sizeof(float));

	main_skel->mdl->mdlscale[0]=1;
	main_skel->mdl->mdlscale[1]=1;
	main_skel->mdl->mdlscale[2]=1;

	main_skel->mdl->radius = 96;

	PDGLUI_BeginMenu("menu");

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

	PDGLUI_EndMenu();


	s=PDGLUI_BeginForm("fixedbox", "bones");
	PDGLUI_FormProperty2I("origin", 400-(16*8), 300);

	PDGLUI_BeginWidget("vbox", NULL);

	PDGLUI_BeginWidget("hbox", NULL);

	PDGLUI_BeginWidget("label", NULL);
	PDGLUI_WidgetPropertyS("label", "Mode: ");
	PDGLUI_EndWidget();

	PDGLUI_BeginWidget("label", "mode");
	PDGLUI_WidgetPropertyS("label", "");
	PDGLUI_EndWidget();

	PDGLUI_EndWidget();


	PDGLUI_BeginWidget("list", "lst");
	PDGLUI_WidgetPropertyI("rows", 24);
	PDGLUI_WidgetPropertyI("cols", 16);
	PDGLUI_EndWidget();

//	PDGLUI_BeginWidget("hbox", NULL);

	PDGLUI_BeginWidget("label", NULL);
	PDGLUI_WidgetPropertyS("label", "Rename: ");
	PDGLUI_EndWidget();

	PDGLUI_BeginWidget("textbox", "rename");
	PDGLUI_WidgetPropertyI("size", 16);
	PDGLUI_EndWidget();

//	PDGLUI_EndWidget();

	PDGLUI_BeginWidget("checkbox", "show_bones");
	PDGLUI_WidgetPropertyS("label", "Show Bones");
	PDGLUI_WidgetPropertyI("checked", 1);
	PDGLUI_EndWidget();

	PDGLUI_BeginWidget("checkbox", "show_solids");
	PDGLUI_WidgetPropertyS("label", "Show Solids");
	PDGLUI_WidgetPropertyI("checked", 0);
	PDGLUI_EndWidget();

	PDGLUI_EndWidget();

	PDGLUI_EndForm();

	Con_ExecCmdBuf("game/bonetool.cfg");

	main_skel->bone_sel=-1;
	main_mode=MODE_BONES;

	return(1);
}

void main_set_keyframe()
{
	float pt[3];
	int i, j, k, l;

	if(main_mode!=MODE_ANIMATE)return;

	i=main_skel->bone_sel;
	j=main_skel->mdl->bone_up[i];
	k=main_skel->frame_cur*64+i;
	if(!(main_skel->anim->frame_mask[k>>3]&(1<<(k&7))))
	{
		main_skel->anim->frame_mask[k>>3]|=(1<<(k&7));
		if(j<0)
		{
			V3F_COPY(main_skel->bone_rorg+i*3, main_skel->anim->frame_org+k*3);
			for(l=0; l<9; l++)main_skel->anim->frame_rot[k*9+l]=
				main_skel->bone_rmat[i*9+l];
		}else
		{
			Mat3F_Mat3MultT(main_skel->bone_rmat+i*9,
				main_skel->bone_rmat+j*9, main_skel->anim->frame_rot+k*9);
			V3F_COPY(main_skel->bone_rorg+i*3, pt);
			V3F_SUB(pt, main_skel->bone_rorg+j*3, pt);
			main_skel->anim->frame_org[k*3+0]=V3F_DOT(pt, main_skel->bone_rmat+j*9+0);
			main_skel->anim->frame_org[k*3+1]=V3F_DOT(pt, main_skel->bone_rmat+j*9+3);
			main_skel->anim->frame_org[k*3+2]=V3F_DOT(pt, main_skel->bone_rmat+j*9+6);
		}
	}

}

void main_delete_bone(int num)
{
	int i, j;

	main_skel->mdl->bone_name[num]=NULL;
	for(i=0; i<main_skel->anim->num_frames; i++)
	{
		j=i*64+num;
		main_skel->anim->frame_mask[j>>3]&=~(1<<(j&7));
	}

	for(i=0; i<main_skel->mdl->num_bones; i++)
	{
		if(!main_skel->mdl->bone_name[i])continue;
		if(main_skel->mdl->bone_up[i]==num)
			main_delete_bone(i);
	}
}


void main_show_help(char *mode)
{
	static char *helpstr_generic=
"*Controls*\n"
"\n"
"General:\n"
"	Left//Right Arrows, Move left//right\n"
"	Up//Down Arrows, Move forwards//backwards\n"
"	Shift+Up//Down Arrows, Move up//down\n"
"	Shift+Home, Rotate to nearest 45 degrees\n"
"	CMB+Drag, Rotate Camera\n"
"	Tab, Change mode\n"
"Bones:\n"
"	LMB+Drag, Rotate object\n"
"	Shift+LMB+Drag, Translate object\n"
"	Delete(Anim Mode), Delete keyframe\n"
"	Shift+Delete(Anim Mode), Delete all bone keyframes\n"
"	Delete(Bone Mode), Delete bone and sub bones\n"
"	Insert(Bone Mode), Create a new bone attached to the current bone\n"
"Anim:\n"
"	+, Next Frame\n"
"	--, Prev Frame\n"
"	**, New Frame\n"
"	//, Delete Frame\n"
"\n\n";

	static char *helpstr_about=
"BGB Skeletal Animating Tool\n"
"2005--2006 By Brendan G Bohannon\n"
"";

	char *helpstr;
	char *s;

	helpstr=helpstr_generic;
	if(mode && !strcmp(mode, "about"))helpstr=helpstr_about;

	s=PDGLUI_BeginForm("window", "help");
	if(!s)return;

	PDGLUI_FormPropertyS("title", "Help");

	PDGLUI_BeginWidget("vbox", NULL);

	PDGLUI_BeginWidget("text", NULL);
	PDGLUI_WidgetPropertyS("value", helpstr);
	PDGLUI_EndWidget();

	PDGLUI_BeginWidget("button", "btn_ok");
	PDGLUI_WidgetPropertyS("label", "   &OK   ");
	PDGLUI_EndWidget();

	PDGLUI_EndWidget();

	PDGLUI_EndForm();
}

void main_handle_form(char *frm)
{
	char *s0, *s1, *s2, *s;
	int i;

	while(PDGLUI_HasEvent(frm))
	{
		s0=PDGLUI_GetEventWidget(frm);
		s1=PDGLUI_GetEventName(frm);
		i=PDGLUI_GetEventIVal(frm);
		s2=PDGLUI_GetEventSVal(frm);
		PDGLUI_NextEvent(frm);

		if(!s0)continue;

		if(!strcmp(frm, "help") && !strcmp(s0, "btn_ok"))
		{
			PDGLUI_Forms_Unlink(frm);
		}

		if(!strcmp(frm, "bones") &&
			!strcmp(s0, "rename") &&
			!strcmp(s1, "submit"))
		{
			i=main_skel->bone_sel;
			if(i>=0)
			{
				s=main_skel->mdl->bone_name[i];
				main_skel->mdl->bone_name[i]=dystrdup(s2);

				dyPrintf("Rename Bone %s -> %s", s, s2);
			}
			PDGLUI_SetWidgetPropertyS(
				"bones", "rename", "text", "");
		}
		if(!strcmp(frm, "bones") &&
			!strcmp(s0, "lst") && !strcmp(s1, "select"))
		{
			main_skel->bone_sel=i;
		}

		if(!strcmp(frm, "bones") && !strcmp(s0, "show_bones"))
		{
			if(!strcmp(s1, "clicked"))main_showbones=1;
			if(!strcmp(s1, "unclicked"))main_showbones=0;
		}
		if(!strcmp(frm, "bones") && !strcmp(s0, "show_solids"))
		{
			if(!strcmp(s1, "clicked"))main_showsolids=1;
			if(!strcmp(s1, "unclicked"))main_showsolids=0;
		}
	}
}

void main_handle_events()
{
	main_handle_form("help");
	main_handle_form("bones");
}

void main_handle_input_bones()
{
	float io[3], iv[4], fw[2], pt[4], tv[3], f, g, v;
	float im[16], im1[16], *imv;
	int i, j, k, l;

	if(PDGL_KeyDown(K_ENTER))
		main_skel->bone_sel=-1;

	for(i=0; i<main_skel->mdl->num_bones; i++)
	{
		V3F_COPY(main_skel->bone_rorg+i*3, io);

		f=V3F_DOT(io, main_mdir)-V3F_DOT(main_morg, main_mdir);
		V3F_ADDSCALE(main_morg, main_mdir, f, pt);
		f=V3F_DIST(pt, io);
		if(f<0.5)
		{
			if(PDGL_KeyDown(K_ENTER))
				main_skel->bone_sel=i;
		}
	}

	if((state->mb&1) && (main_skel->bone_sel>=0) && PDGL_KeyDown(K_SHIFT))
	{
		i=main_skel->bone_sel;
		V3F_COPY(main_skel->bone_rorg+i*3, io);
		v=V3F_DOT(io, main_cam_fw)-V3F_DOT(main_cam_org, main_cam_fw);

		V3F_SCALEADDSCALE(main_cam_up, -state->dmy*v/300,
			main_cam_rt, state->dmx*v/300, iv);

		j=main_skel->mdl->bone_up[i];
		if(j<0)
		{
			V3F_ADD(main_skel->bone_rorg+i*3, iv, pt);
			V3F_COPY(pt, main_skel->mdl->bone_org+i*3);
		}else
		{
			V3F_ADD(main_skel->bone_rorg+i*3, iv, pt);
			V3F_SUB(pt, main_skel->bone_rorg+j*3, pt);

			imv=main_skel->bone_rmat+j*9;
			main_skel->mdl->bone_org[i*3+0]=V3F_DOT(pt, imv+0);
			main_skel->mdl->bone_org[i*3+1]=V3F_DOT(pt, imv+3);
			main_skel->mdl->bone_org[i*3+2]=V3F_DOT(pt, imv+6);
		}
	}

	if((state->mb&1) && (main_skel->bone_sel>=0) && !PDGL_KeyDown(K_SHIFT))
	{
		i=main_skel->bone_sel;
		V3F_COPY(main_skel->bone_rorg+i*3, io);

		pt[0]=V3F_DOT(io, main_cam_rt)-
			V3F_DOT(main_cam_org, main_cam_rt);
		pt[1]=V3F_DOT(io, main_cam_up)-
			V3F_DOT(main_cam_org, main_cam_up);
		pt[2]=V3F_DOT(io, main_cam_fw)-
			V3F_DOT(main_cam_org, main_cam_fw);

		f=atan2((-state->my/400.0)-(pt[1]/pt[2]),
			(state->mx/400.0)-(pt[0]/pt[2]));
		g=atan2((-state->lmy/400.0)-(pt[1]/pt[2]),
			(state->lmx/400.0)-(pt[0]/pt[2]));

		Mat3F_Rotate3Matrix(main_skel->bone_rmat+i*9,
			main_cam_fw, -(f-g), main_skel->bone_rmat+i*9);

		j=main_skel->mdl->bone_up[i];
		if(j<0)
		{
			for(j=0; j<9; j++)main_skel->mdl->bone_rot[i*9+j]=
				main_skel->bone_rmat[i*9+j];
		}else
		{
			imv=main_skel->bone_rmat+j*9;
			Mat3F_Mat3MultT(main_skel->bone_rmat+i*9, imv,
				main_skel->mdl->bone_rot+i*9);
		}
	}
}

void main_handle_input_animate()
{
	float io[3], iv[4], fw[2], pt[4], tv[3], f, g, v;
	float im[16], im1[16], *imv;
	int i, j, k, l;

	if(PDGL_KeyDown(K_ENTER))
		main_skel->bone_sel=-1;

	for(i=0; i<main_skel->mdl->num_bones; i++)
	{
		V3F_COPY(main_skel->bone_rorg+i*3, io);

		f=V3F_DOT(io, main_mdir)-V3F_DOT(main_morg, main_mdir);
		V3F_ADDSCALE(main_morg, main_mdir, f, pt);
		f=V3F_DIST(pt, io);
		if(f<0.5)
		{
			if(PDGL_KeyDown(K_ENTER))
				main_skel->bone_sel=i;
		}
	}

	if((state->mb&1) && (main_skel->bone_sel>=0) && PDGL_KeyDown(K_SHIFT))
	{
		main_set_keyframe();

		i=main_skel->bone_sel;
		V3F_COPY(main_skel->bone_rorg+i*3, io);
		v=V3F_DOT(io, main_cam_fw)-V3F_DOT(main_cam_org, main_cam_fw);

		V3F_SCALEADDSCALE(main_cam_up, -state->dmy*v/300,
			main_cam_rt, state->dmx*v/300, iv);

		j=main_skel->mdl->bone_up[i];
		if(j<0)
		{
			V3F_ADD(main_skel->bone_rorg+i*3, iv, pt);

			k=main_skel->frame_cur*64+i;
			main_skel->anim->frame_org[k*3+0]=pt[0];
			main_skel->anim->frame_org[k*3+1]=pt[1];
			main_skel->anim->frame_org[k*3+2]=pt[2];
		}else
		{
			V3F_ADD(main_skel->bone_rorg+i*3, iv, pt);
			V3F_SUB(pt, main_skel->bone_rorg+j*3, pt);

			imv=main_skel->bone_rmat+j*9;

			k=main_skel->frame_cur*64+i;
			main_skel->anim->frame_org[k*3+0]=V3F_DOT(pt, imv+0);
			main_skel->anim->frame_org[k*3+1]=V3F_DOT(pt, imv+3);
			main_skel->anim->frame_org[k*3+2]=V3F_DOT(pt, imv+6);
		}
	}

	if((state->mb&1) && (main_skel->bone_sel>=0) && !PDGL_KeyDown(K_SHIFT))
	{
		main_set_keyframe();

		i=main_skel->bone_sel;
		V3F_COPY(main_skel->bone_rorg+i*3, io);

		pt[0]=V3F_DOT(io, main_cam_rt)-
			V3F_DOT(main_cam_org, main_cam_rt);
		pt[1]=V3F_DOT(io, main_cam_up)-
			V3F_DOT(main_cam_org, main_cam_up);
		pt[2]=V3F_DOT(io, main_cam_fw)-
			V3F_DOT(main_cam_org, main_cam_fw);

		f=atan2((-state->my/400.0)-(pt[1]/pt[2]),
			(state->mx/400.0)-(pt[0]/pt[2]));
		g=atan2((-state->lmy/400.0)-(pt[1]/pt[2]),
			(state->lmx/400.0)-(pt[0]/pt[2]));

		Mat3F_Rotate3Matrix(main_skel->bone_rmat+i*9,
			main_cam_fw, -(f-g), main_skel->bone_rmat+i*9);

		j=main_skel->mdl->bone_up[i];
		if(j<0)
		{
			k=main_skel->frame_cur*64+i;
			for(j=0; j<9; j++)main_skel->anim->frame_rot[k*9+j]=
				main_skel->bone_rmat[i*9+j];
		}else
		{
			imv=main_skel->bone_rmat+j*9;

			k=main_skel->frame_cur*64+i;
			Mat3F_Mat3MultT(main_skel->bone_rmat+i*9, imv,
				main_skel->anim->frame_rot+k*9);
		}
	}
}

void main_handle_input_solid()
{
	LBXGL_Skel2Solid *solid;
	float tv0[3], tv1[3], iv[3];
	float f, g, v, d;
	int i, j, k, l;

	if(!main_skel)return;

	if(PDGL_KeyDown(K_ENTER))
	{
		main_skel->solid_sel=-1;

		for(i=0; i<main_skel->mdl->num_solids; i++)
		{
			j=SkelUtil_CheckLineSolid(main_skel, i,
				main_morg, main_mdir);
			k=main_skel->mdl->solid[i]->bone;

			if(j && PDGL_KeyDown(K_ENTER))
			{
				main_skel->solid_sel=i;
				main_skel->bone_sel=k;
			}
		}
	}

	if(main_skel->solid_sel<0)return;

	i=main_skel->solid_sel;
	if(!main_skel->mdl || !main_skel->mdl->solid)
		return;
	solid=main_skel->mdl->solid[i];
	if(!solid)return;
	j=solid->bone;

	if(state->mb&1)
	{
		V3F_ADD(solid->m, main_skel->bone_rorg+j*3, tv0);
		v=V3F_DOT(tv0, main_mdir)-V3F_DOT(main_morg, main_mdir);
		V3F_ADDSCALE(main_morg, main_mdir, v, tv1);
		V3F_SCALEADDSCALE(main_cam_up, -state->dmy*v/300,
			main_cam_rt, state->dmx*v/300, iv);
		if(V3F_DIST(tv0, tv1)<1)
		{
			V3F_ADD(solid->m, iv, solid->m);
		}


		V3F_ADD(solid->n, main_skel->bone_rorg+j*3, tv0);
		v=V3F_DOT(tv0, main_mdir)-V3F_DOT(main_morg, main_mdir);
		V3F_ADDSCALE(main_morg, main_mdir, v, tv1);
		V3F_SCALEADDSCALE(main_cam_up, -state->dmy*v/300,
			main_cam_rt, state->dmx*v/300, iv);
		if(V3F_DIST(tv0, tv1)<1)
		{
			V3F_ADD(solid->n, iv, solid->n);
		}



		V3F_ADD(solid->m, main_skel->bone_rorg+j*3, tv0);
		tv0[2]-=solid->r0;

		v=V3F_DOT(tv0, main_mdir)-V3F_DOT(main_morg, main_mdir);
		V3F_ADDSCALE(main_morg, main_mdir, v, tv1);
		V3F_SCALEADDSCALE(main_cam_up, -state->dmy*v/300,
			main_cam_rt, state->dmx*v/300, iv);
		if(V3F_DIST(tv0, tv1)<1)
			solid->r0-=iv[2];


		V3F_ADD(solid->m, main_skel->bone_rorg+j*3, tv0);
		tv0[2]+=solid->r1;

		v=V3F_DOT(tv0, main_mdir)-V3F_DOT(main_morg, main_mdir);
		V3F_ADDSCALE(main_morg, main_mdir, v, tv1);
		V3F_SCALEADDSCALE(main_cam_up, -state->dmy*v/300,
			main_cam_rt, state->dmx*v/300, iv);
		if(V3F_DIST(tv0, tv1)<1)
			solid->r1+=iv[2];
	}
}

void main_handle_input()
{
	char buf[64];
	float io[3], iv[4], fw[2], pt[4], tv[3], f, g, v;
	float im[16], im1[16], *imv;
	char *s;
	int i, j, k, l;
	unsigned short *kcur;

	if(Con_IsDown() || main_loadsave || PDGLUI_HasFocus())
		return;

	if(state->mb&4)
		PDGLUI_ShowMenu("menu", state->mx, -state->my);

	s=PDGLUI_GetMenuSelection("menu");
	if(s)
	{
		if(!strcmp(s, "clear_all"))
		{
			main_skel->mdl->num_bones=0;
			main_skel->mdl->num_solids=0;
			main_skel->anim->num_frames=0;
			main_skel->mdl->mesh=NULL;
			for(i=0; i<main_skel->mdl->num_bones; i++)
				main_skel->mdl->mesh_grp[i]=NULL;
			main_skel->frame_cur=0;

			main_flushui=1;
		}
		if(!strcmp(s, "clear_bones"))
		{
			main_skel->mdl->num_bones=0;
			main_skel->mdl->num_solids=0;
			main_flushui=1;
		}
		if(!strcmp(s, "clear_anim"))
		{
			main_skel->anim->num_frames=0;
			main_flushui=1;
		}
		if(!strcmp(s, "clear_mesh"))
		{
			main_skel->mdl->mesh=NULL;
			for(i=0; i<main_skel->mdl->num_bones; i++)
				main_skel->mdl->mesh_grp[i]=NULL;
			main_flushui=1;
		}


		if(!strcmp(s, "load_bones"))
		{
			UI_LoadSave_SetTitle("Load Bones", "bone");
			main_loadsave=1;
		}
		if(!strcmp(s, "save_bones"))
		{
			UI_LoadSave_SetTitle("Save Bones", "bone");
			main_loadsave=2;
		}
		if(!strcmp(s, "load_anim"))
		{
			UI_LoadSave_SetTitle("Load Anim", "anim");
			main_loadsave=3;
		}
		if(!strcmp(s, "save_anim"))
		{
			UI_LoadSave_SetTitle("Save Anim", "anim");
			main_loadsave=4;
		}
		if(!strcmp(s, "load_model"))
		{
			UI_LoadSave_SetTitle("Load Model", "model");
			main_loadsave=5;
		}
		if(!strcmp(s, "save_model"))
		{
			UI_LoadSave_SetTitle("Save Model", "model");
			main_loadsave=6;
		}

		if(!strcmp(s, "load_mesh"))
		{
			UI_LoadSave_SetTitle("Load Mesh", "mesh");
			main_loadsave=7;
		}
		if(!strcmp(s, "save_mesh"))
		{
			UI_LoadSave_SetTitle("Save Mesh", "mesh");
			main_loadsave=8;
		}

		if(!strcmp(s, "load_ac3d"))
		{
			UI_LoadSave_SetTitle("Import AC3D", "ac");
			main_loadsave=9;
		}

		if(!strcmp(s, "load_smdref"))
		{
			UI_LoadSave_SetTitle("Import SMD Reference", "smd");
			main_loadsave=10;
		}
		if(!strcmp(s, "load_smdanim"))
		{
			UI_LoadSave_SetTitle("Import SMD Anim", "smd");
			main_loadsave=11;
		}

		if(!strcmp(s, "cmd_rebind"))
		{
			Skel_BindMeshGroup(main_skel, main_skel->mdl->mesh);
			main_flushui=1;
		}
		if(!strcmp(s, "cmd_unbind"))
		{
			Skel_UnbindMeshGroup(main_skel, main_skel->mdl->mesh);
			main_flushui=1;
		}

		if(!strcmp(s, "help_generic"))
			main_show_help("general");
		if(!strcmp(s, "help_about"))
			main_show_help("about");
	}


	kcur=state->keys;
	while(*kcur)
	{
		if(*kcur==K_ESCAPE)
		{
			state->kill=1;
			return;
		}

		if(*kcur==K_MWHEELUP)
		{
			main_speed*=1.5;
			Con_Printf("Move Speed %g ft/s\n", main_speed/12);
		}
		if(*kcur==K_MWHEELDOWN)
		{
			main_speed*=1.0/1.5;
			Con_Printf("Move Speed %g ft/s\n", main_speed/12);
		}

		if(*kcur==K_F1)
		{
			main_show_help("general");
		}

		if(*kcur==K_F10)
			state->doshot=1;

		if(*kcur==K_TAB)
		{
			main_mode++;
			if(main_mode>MODE_LAST)main_mode=MODE_FIRST;
		}

		if(*kcur=='S')
		{
//			main_save_anim(frame_file);
			Skel_SaveAnim(main_skel->mdl, main_skel->anim,
				frame_file);
		}

		if((*kcur==K_DEL) && (main_skel->bone_sel>=0) &&
			(main_mode==MODE_ANIMATE))
		{
			if(PDGL_KeyDown(K_SHIFT))
			{
				for(i=0; i<main_skel->anim->num_frames; i++)
				{
					j=i*64+main_skel->bone_sel;
					main_skel->anim->frame_mask[j>>3]&=~(1<<(j&7));
				}
			}else
			{
				i=main_skel->frame_cur*64+main_skel->bone_sel;
				main_skel->anim->frame_mask[i>>3]&=~(1<<(i&7));
			}
			main_flushui=1;
		}

		if((*kcur==K_DEL) && (main_skel->bone_sel>=0) &&
			(main_mode==MODE_BONES))
		{
			main_delete_bone(main_skel->bone_sel);
			main_flushui=1;
		}

		if((*kcur==K_DEL) && (main_skel->bone_sel>=0) &&
			(main_mode==MODE_MESH))
		{
			main_skel->mdl->mesh_grp[main_skel->bone_sel]=NULL;
			main_flushui=1;
		}

		if((*kcur==K_INS) && (main_mode==MODE_BONES))
		{
			for(i=0; i<main_skel->mdl->num_bones; i++)
				if(!main_skel->mdl->bone_name[i])
					break;
			if(i>=main_skel->mdl->num_bones)main_skel->mdl->num_bones=i+1;

			sprintf(buf, "auto-%d", i);
			main_skel->mdl->bone_name[i]=strdup(buf);
			main_skel->mdl->bone_up[i]=main_skel->bone_sel;
			Mat3F_Identity(main_skel->mdl->bone_rot+i*9);
			Mat3F_Identity(main_skel->bone_rmat+i*9);

			j=main_skel->bone_sel;
			V3F_ZERO(tv);
			if(j>=0) { V3F_COPY(main_skel->bone_rorg+j*3, tv); }

			pt[0]=V3F_DOT(tv, main_cam_rt)-
				V3F_DOT(main_cam_org, main_cam_rt);
			pt[1]=V3F_DOT(tv, main_cam_up)-
				V3F_DOT(main_cam_org, main_cam_up);
			pt[2]=V3F_DOT(tv, main_cam_fw)-
				V3F_DOT(main_cam_org, main_cam_fw);

			V3F_ZERO(io);
			V3F_ADDSCALE(io, main_cam_rt,
				(state->mx/400.0)*pt[2]-pt[0], io);
			V3F_ADDSCALE(io, main_cam_up,
				(-state->my/400.0)*pt[2]-pt[1], io);

			main_skel->mdl->bone_org[i*3+0]=io[0];
			main_skel->mdl->bone_org[i*3+1]=io[1];
			main_skel->mdl->bone_org[i*3+2]=io[2];

			main_skel->bone_rorg[i*3+0]=io[0]+tv[0];
			main_skel->bone_rorg[i*3+1]=io[1]+tv[1];
			main_skel->bone_rorg[i*3+2]=io[2]+tv[2];

			main_flushui=1;
			return;
		}

		if(((*kcur=='+') || (*kcur=='=')) && (main_skel->frame_time>=(1.0/15.0)))
		{
			main_mode=MODE_ANIMATE;

			main_skel->frame_time-=1.0/15.0;
			main_skel->frame_cur++;
			if(main_skel->anim->num_frames)main_skel->frame_cur%=main_skel->anim->num_frames;
				else main_skel->frame_cur=0;
		}
		if(((*kcur=='-') || (*kcur=='_')) && (main_skel->frame_time>=(1.0/15.0)))
		{
			main_mode=MODE_ANIMATE;

			main_skel->frame_time-=1.0/15.0;
			main_skel->frame_cur--;
			if(main_skel->frame_cur<0)main_skel->frame_cur+=main_skel->anim->num_frames;
			if(main_skel->anim->num_frames)main_skel->frame_cur%=main_skel->anim->num_frames;
				else main_skel->frame_cur=0;
		}
		if(*kcur=='*')
		{
			main_mode=MODE_ANIMATE;

			for(i=main_skel->anim->num_frames; i>main_skel->frame_cur; i--)
			{
				for(j=0; j<64*3; j++)
					main_skel->anim->frame_org[i*64*3+j]=
						main_skel->anim->frame_org[(i-1)*64*3+j];
				for(j=0; j<64*9; j++)
					main_skel->anim->frame_rot[i*64*9+j]=
						main_skel->anim->frame_rot[(i-1)*64*9+j];
				for(j=0; j<64/8; j++)
					main_skel->anim->frame_mask[i*64/8+j]=
						main_skel->anim->frame_mask[(i-1)*64/8+j];
			}
			if(main_skel->anim->num_frames)main_skel->frame_cur++;
			for(j=0; j<64/8; j++)main_skel->anim->frame_mask[i*64/8+j]=0;
			main_skel->anim->num_frames++;

			main_flushui=1;
		}

		if((main_skel->anim->num_frames>0) && (*kcur=='/'))
		{
			main_mode=MODE_ANIMATE;

			for(i=main_skel->frame_cur; i<main_skel->anim->num_frames; i++)
			{
				for(j=0; j<64*3; j++)
					main_skel->anim->frame_org[i*64*3+j]=
						main_skel->anim->frame_org[(i+1)*64*3+j];
				for(j=0; j<64*9; j++)
					main_skel->anim->frame_rot[i*64*9+j]=
						main_skel->anim->frame_rot[(i+1)*64*9+j];
				for(j=0; j<64/8; j++)
					main_skel->anim->frame_mask[i*64/8+j]=
						main_skel->anim->frame_mask[(i+1)*64/8+j];
			}
			if(main_skel->frame_cur>0)main_skel->frame_cur--;
			main_skel->anim->num_frames--;

			main_flushui=1;
		}

		kcur++;
	}

	V3F_ZERO(iv);
	if(PDGL_KeyDown(K_SHIFT) || PDGL_KeyDown(K_CTRL))
	{
		if(PDGL_KeyDown(K_LEFTARROW))
		{
			V3F_ADDSCALE(iv, main_cam_rt,
				-state->dt_f*main_speed, iv);
		}
		if(PDGL_KeyDown(K_RIGHTARROW))
		{
			V3F_ADDSCALE(iv, main_cam_rt,
				state->dt_f*main_speed, iv);
		}

		if(PDGL_KeyDown(K_UPARROW))
		{
			V3F_ADDSCALE(iv, main_cam_up,
				state->dt_f*main_speed, iv);
		}
		if(PDGL_KeyDown(K_DOWNARROW))
		{
			V3F_ADDSCALE(iv, main_cam_up,
				-state->dt_f*main_speed, iv);
		}

		if(PDGL_KeyDown(K_HOME))
		{
			i=(int)((main_cam_ang[0]+22.5)/45);
			main_cam_ang[0]=(i%8)*45;

			i=(int)((main_cam_ang[1]+22.5)/45);
			main_cam_ang[1]=(i%8)*45;

			i=(int)((main_cam_ang[2]+22.5)/45);
			main_cam_ang[2]=(i%8)*45;
		}
	}else
	{
		if(PDGL_KeyDown(K_LEFTARROW))
		{
			V3F_ADDSCALE(iv, main_cam_rt,
				-state->dt_f*main_speed, iv);
		}
		if(PDGL_KeyDown(K_RIGHTARROW))
		{
			V3F_ADDSCALE(iv, main_cam_rt,
				state->dt_f*main_speed, iv);
		}

		if(PDGL_KeyDown(K_UPARROW))
		{
			V3F_ADDSCALE(iv, main_cam_fw,
				state->dt_f*main_speed, iv);
		}
		if(PDGL_KeyDown(K_DOWNARROW))
		{
			V3F_ADDSCALE(iv, main_cam_fw,
				-state->dt_f*main_speed, iv);
		}
	}

	if(state->mb&2)
	{
		if(state->lmb&2)
		{
			main_cam_ang[0]-=state->my;
			main_cam_ang[2]-=state->mx;

			if(main_cam_ang[0]<0)main_cam_ang[0]=0;
			if(main_cam_ang[0]>180)main_cam_ang[0]=180;
		}

		Mouse_SetPos(0, 0);
	}

	V3F_ADD(main_cam_org, iv, main_cam_org);
	main_cam_ang[0]=floor(main_cam_ang[0]+0.5);
	main_cam_ang[1]=floor(main_cam_ang[1]+0.5);
	main_cam_ang[2]=floor(main_cam_ang[2]+0.5);

	if(main_cam_ang[0]<0)main_cam_ang[0]+=360;
	if(main_cam_ang[0]>=360)main_cam_ang[0]-=360;
	if(main_cam_ang[1]<0)main_cam_ang[1]+=360;
	if(main_cam_ang[1]>=360)main_cam_ang[1]-=360;
	if(main_cam_ang[2]<0)main_cam_ang[2]+=360;
	if(main_cam_ang[2]>=360)main_cam_ang[2]-=360;

	V3F_ZERO(iv);

	if(main_mode==MODE_BONES)
		main_handle_input_bones();
	if(main_mode==MODE_ANIMATE)
		main_handle_input_animate();
	if(main_mode==MODE_SOLID)
		main_handle_input_solid();
}

void main_draw_solids()
{
	float io[3], iv[4], fw[2], pt[4], pt1[3];
	float f, g, v;
	float im[16], *imv, *v0, *v1, *v2;
	int i, j, k;

	if((main_mode!=MODE_BONES) && (main_mode!=MODE_SOLID) &&
		!main_showsolids)
		return;

	pdglDisable(GL_DEPTH_TEST);

	for(i=0; i<main_skel->mdl->num_solids; i++)
	{
		pdglModelviewMatrix();
		pdglPushMatrix();

		j=main_skel->mdl->solid[i]->bone;
		v0=main_skel->bone_rorg+3*j;
		imv=main_skel->bone_rmat+9*j;

		pdglTranslatef(v0[0], v0[1], v0[2]);

		Mat4F_Identity(im);
		for(j=0; j<3; j++)for(k=0; k<3; k++)
			im[j*4+k]=imv[j*3+k];
		pdglMultMatrixf(im);

		v1=main_skel->mdl->solid[i]->m;
		v2=main_skel->mdl->solid[i]->n;

		V3F_COPY(v1, pt);
		pt[2]-=main_skel->mdl->solid[i]->r0;

		V3F_COPY(v1, pt1);
		pt1[2]+=main_skel->mdl->solid[i]->r1;

		j=0;
		if(main_mode==MODE_SOLID)
			j= i == main_skel->solid_sel;
		switch(main_skel->mdl->solid[i]->type)
		{
		case BONE_SOLID_BBOX:
			if(j)
			{
				pdglBegin(PDGL_LINES);
				pdglColor4f(1, 1, 0, 1);
				main_draw_star(v1);
				main_draw_star(v2);
				pdglEnd();
			}
			break;
		case BONE_SOLID_SPHERE:
			if(j)
			{
				pdglBegin(PDGL_LINES);
				pdglColor4f(1, 1, 0, 1);
				main_draw_star(v1);

				pdglColor4f(0, 0, 1, 1);
				pdglVertex3fv(v1);
				pdglVertex3fv(pt);
				main_draw_star(pt);

				pdglVertex3fv(v1);
				pdglVertex3fv(pt1);
				main_draw_star(pt1);
				pdglEnd();
			}
			break;
		case BONE_SOLID_CAPSULE:
			if(j)
			{
				pdglBegin(PDGL_LINES);
				pdglColor4f(1, 1, 0, 1);
				pdglVertex3fv(v1);
				pdglVertex3fv(v2);
				main_draw_star(v1);
				main_draw_star(v2);

				pdglColor4f(0, 0, 1, 1);
				pdglVertex3fv(v1);
				pdglVertex3fv(pt);
				main_draw_star(pt);

				pdglVertex3fv(v1);
				pdglVertex3fv(pt1);
				main_draw_star(pt1);
				pdglEnd();
			}
			break;
		default:
			break;
		}

		pdglPopMatrix();
	}

	pdglEnable(GL_DEPTH_TEST);

	for(i=0; i<main_skel->mdl->num_solids; i++)
	{
		pdglModelviewMatrix();
		pdglPushMatrix();

		j=main_skel->mdl->solid[i]->bone;
		v0=main_skel->bone_rorg+3*j;
		imv=main_skel->bone_rmat+9*j;

		pdglTranslatef(v0[0], v0[1], v0[2]);

		for(j=0; j<3; j++)for(k=0; k<3; k++)
			im[j*4+k]=imv[j*3+k];
		im[0*4+3]=0; im[1*4+3]=0; im[2*4+3]=0;
		im[3*4+0]=0; im[3*4+1]=0; im[3*4+2]=0;
		im[3*4+3]=1;
		pdglMultMatrixf(im);

		v1=main_skel->mdl->solid[i]->m;
		v2=main_skel->mdl->solid[i]->n;

		k=0;
		if(main_mode==MODE_SOLID)
			k=SkelUtil_CheckLineSolid(main_skel, i,
				main_morg, main_mdir);

		if(main_mode==MODE_BONES)
			j=main_skel->mdl->solid[i]->bone ==
				main_skel->bone_sel;
		if(main_mode==MODE_SOLID)
			j= i == main_skel->solid_sel;
		switch(main_skel->mdl->solid[i]->type)
		{
		case BONE_SOLID_BBOX:
			PD3D_DrawFlatCube(v1[0], v1[1], v1[2],
				v2[0], v2[1], v2[2],
				j?0:1, j?1:0, k?1:0, 0.25);
			break;
		case BONE_SOLID_SPHERE:
			PD3D_DrawFlatSphere(v1[0], v1[1], v1[2],
				main_skel->mdl->solid[i]->r0,
				j?0:1, j?1:0, k?1:0, 0.25);
			if(main_skel->mdl->solid[i]->r1>
				main_skel->mdl->solid[i]->r0)
				PD3D_DrawFlatSphere(v1[0], v1[1], v1[2],
					main_skel->mdl->solid[i]->r1,
					j?0:1, j?1:0, k?0:1, 0.25);
			break;
		case BONE_SOLID_CAPSULE:
			PD3D_DrawFlatCapsule(v1, v2, main_skel->mdl->solid[i]->r0,
				j?0:1, j?1:0, k?1:0, 0.25);
			if(main_skel->mdl->solid[i]->r1>main_skel->mdl->solid[i]->r0)
				PD3D_DrawFlatCapsule(v1, v2, main_skel->mdl->solid[i]->r1,
					j?0:1, j?1:0, k?0:1, 0.25);
			break;
		default:
			break;
		}

		pdglPopMatrix();
	}

}

int pdgl_main_body()
{
	static int t;
	char buf[64];
	float mins[3], maxs[3];
	float io[3], iv[4], fw[2], pt[4], f, g, v;
	float im[16], im1[16], *imv, *v0, *v1, *v2;
	char *s;
	int i, j, k;
	unsigned short *kcur;
	LBXGL_Skel2MeshGroup *gtmp, *gcur;
	LBXGL_Skel2Mesh *mcur;


	f=main_cam_ang[2]*(M_PI/180);
	g=(main_cam_ang[0]-90)*(M_PI/180);
	main_cam_fw[0]=-sin(f)*cos(g);
	main_cam_fw[1]=cos(f)*cos(g);
	main_cam_fw[2]=sin(g);

	main_cam_rt[0]=cos(f);
	main_cam_rt[1]=sin(f);
	main_cam_rt[2]=0;

	main_cam_up[0]=sin(f)*sin(g);
	main_cam_up[1]=-cos(f)*sin(g);
	main_cam_up[2]=cos(g);

	iv[0]=(state->mx/400.0)*main_cam_rt[0]+(-state->my/400.0)*main_cam_up[0];
	iv[1]=(state->mx/400.0)*main_cam_rt[1]+(-state->my/400.0)*main_cam_up[1];
	iv[2]=(state->mx/400.0)*main_cam_rt[2]+(-state->my/400.0)*main_cam_up[2];
	main_morg[0]=main_cam_org[0]+iv[0]+main_cam_fw[0];
	main_morg[1]=main_cam_org[1]+iv[1]+main_cam_fw[1];
	main_morg[2]=main_cam_org[2]+iv[2]+main_cam_fw[2];

	main_mdir[0]=main_cam_fw[0]+iv[0];
	main_mdir[1]=main_cam_fw[1]+iv[1];
	main_mdir[2]=main_cam_fw[2]+iv[2];
	V3F_NORMALIZE(main_mdir, main_mdir);


	f=main_cam_ang[2]*(M_PI/180);
	fw[0]=-sin(f);
	fw[1]=cos(f);

	io[0]=main_cam_org[0]-10*sin(f);
	io[1]=main_cam_org[1]+10*cos(f);
	io[2]=main_cam_org[2];

	f=(((rand()&0xFFF)/2048.0)-1)*0.1;
	g=(((rand()&0xFFF)/2048.0)-1)*0.1;
	io[0]+=f;
	io[1]+=g;

	v=1000;

	main_time+=state->dt_f;

	main_skel->frame_time+=state->dt_f;
	if(main_skel->frame_time>=(2.0/15.0))main_skel->frame_time=2.0/15.0;

	Con_Update(state->dt_f);
	main_handle_input();
	thDoWorkMS(10);

	Draw_SetPerspective_3D(4.0/3.0, 90, 0,
		main_cam_org, main_cam_ang, 0, 0, state->xs, state->ys);

	pdglDisable(GL_CULL_FACE);
	pdglDisableTexture2D();
	pdglEnable(GL_DEPTH_TEST);


//	PD3D_DrawGrid(0, 0, 16, 1);

//	pdglDisable(GL_DEPTH_TEST);
	pdglDisable (GL_TEXTURE_2D);

	pdglColor4f(0.5, 0.5, 0.5, 1);
	PD3D_DrawGrid2(48, 1);

	pdglColor4f(0.75, 0.75, 0.75, 1);
	PD3D_DrawGrid2(256, 12);

//	pdglDisable(GL_DEPTH_TEST);

	pdglColor4f(1, 0, 0, 1);

	pdglBegin(PDGL_LINES);
	V3F_ADD(main_cam_org, main_cam_fw, iv);
	pdglVertex3fv(main_cam_org);
	pdglVertex3fv(iv);
	V3F_ADD(main_cam_org, main_cam_rt, iv);
	pdglVertex3fv(main_cam_org);
	pdglVertex3fv(iv);

	V3F_ADD(main_cam_org, main_cam_up, iv);
	pdglVertex3fv(main_cam_org);
	pdglVertex3fv(iv);
	pdglEnd();

	pdglColor4f(0, 1, 1, 1);

	pdglDisable(GL_DEPTH_TEST);

	for(i=0; i<main_skel->mdl->num_bones; i++)
	{
		j=main_skel->mdl->bone_up[i];
		if(j<0)
		{
			V3F_COPY(main_skel->mdl->bone_org+i*3, main_skel->bone_rorg+i*3);
			Mat3F_Copy(main_skel->mdl->bone_rot+i*9, main_skel->bone_rmat+i*9);

			if(main_mode==MODE_ANIMATE)
			{
				Skel_LerpOrg(main_skel, i, main_skel->bone_rorg+i*3);
				Skel_LerpRot(main_skel, i, main_skel->bone_rmat+i*9);
			}
		}else
		{
			if(main_mode==MODE_ANIMATE)
			{
				Skel_LerpOrg(main_skel, i, pt);
				Skel_LerpRot(main_skel, i, im);
			}else
			{
				V3F_COPY(main_skel->mdl->bone_org+i*3, pt);
				Mat3F_Copy(main_skel->mdl->bone_rot+i*9, im);
			}

			imv=main_skel->bone_rmat+j*9;
			Mat3F_Mat3Mult(im, imv, main_skel->bone_rmat+i*9);
			io[0]=pt[0]*imv[0]+pt[1]*imv[3]+pt[2]*imv[6];
			io[1]=pt[0]*imv[1]+pt[1]*imv[4]+pt[2]*imv[7];
			io[2]=pt[0]*imv[2]+pt[1]*imv[5]+pt[2]*imv[8];
			V3F_ADD(io, main_skel->bone_rorg+j*3, io);
			V3F_COPY(io, main_skel->bone_rorg+i*3);
		}
	}

	pdglDisable(GL_CULL_FACE);
	pdglEnable(GL_DEPTH_TEST);
	pdglColor4f(1, 1, 1, 1);

#ifndef GLES
	pdglEnableLighting();
	pdglEnable(GL_NORMALIZE);

	pdglLightModeli(GL_LIGHT_MODEL_TWO_SIDE, GL_TRUE);
	pt[0]=1; pt[1]=1; pt[2]=1; pt[3]=1;
	pdglMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, pt);
	pt[0]=main_cam_org[0];
	pt[1]=main_cam_org[1];
	pt[2]=main_cam_org[2];
	pt[3]=1;
	pdglLightfv(GL_LIGHT0, GL_POSITION, pt);
	pdglLightf(GL_LIGHT0, GL_QUADRATIC_ATTENUATION, 1.0/1000.0);
	pdglEnable(GL_LIGHT0);

	main_draw_rootmesh();

	for(i=0; i<main_skel->mdl->num_bones; i++)
	{
		if(!main_skel->mdl->bone_name[i])continue;
		main_draw_mesh(i, main_skel->bone_rorg+i*3, main_skel->bone_rmat+i*9);
	}


	pdglDisableLighting();
#endif

	pdglDisableTexture2D();


	pdglDisable(GL_DEPTH_TEST);
	pdglBegin(PDGL_LINES);
	if((main_mode==MODE_BONES) || main_showbones)
		for(i=0; i<main_skel->mdl->num_bones; i++)
	{
		if(!main_skel->mdl->bone_name[i])continue;

		V3F_COPY(main_skel->bone_rorg+i*3, io);

		pdglColor4f(1, 1, 0, 1);

		j=main_skel->frame_cur*64+i;
		if(main_skel->anim->frame_mask[j>>3]&(1<<(j&7)))
			pdglColor4f(0, 1, 1, 1);

		f=V3F_DOT(io, main_mdir)-V3F_DOT(main_morg, main_mdir);
		V3F_ADDSCALE(main_morg, main_mdir, f, pt);
		f=V3F_DIST(pt, io);
		if(f<0.5)
		{
			pdglColor4f(1, 0, 1, 1);
//			if(PDGL_KeyDown(K_ENTER))
//				main_skel->bone_sel=i;
		}
		if(main_skel->bone_sel==i)
			pdglColor4f(0, 0.75, 0, 1);

		main_draw_star(io);

		j=main_skel->mdl->bone_up[i];
		if(j<0)continue;

		pdglColor4f(0, 0, 1, 1);
		pdglVertex3fv(io);

		pdglColor4f(0, 1, 1, 1);
		pdglVertex3fv(main_skel->bone_rorg+j*3);

	}
	pdglEnd();

	pdglEnable(GL_DEPTH_TEST);

	main_draw_solids();


	Draw_SetSolid2_2D(4.0/3.0, 400,
		VEC3(0, 0, 0), VEC3(0, 0, 0), 0, 0, state->xs, state->ys);

	pdglDisableTexture2D();
	pdglColor4f(0, 1, 0, 1);
	pdglBegin(PDGL_LINES);
	pdglVertex2f(-10, -10);
	pdglVertex2f(10, 10);
	pdglVertex2f(-10, 10);
	pdglVertex2f(10, -10);
	pdglEnd();

	Con_Render();

	GfxFont_SetFont("fixed", 0);

	sprintf(buf, "(%g %g %g) (%g %g %g)",
		main_cam_org[0], main_cam_org[1], main_cam_org[2],
		main_cam_ang[0], main_cam_ang[1], main_cam_ang[2]);
	GfxFont_DrawString(buf, -400, 300-12,
		8, 8,  0, 255, 0, 255);

	if(main_skel->anim)
	{
		sprintf(buf, "%d/%d",
			(main_skel->frame_cur+1),
			main_skel->anim->num_frames);
		GfxFont_DrawString(buf, -400, 300-12-8,
			8, 8,  0, 255, 0, 255);
	}

	strcpy(buf, "");
	if(main_mode==MODE_BONES)strcpy(buf, "Bones");
	if(main_mode==MODE_ANIMATE)strcpy(buf, "Animate");
	if(main_mode==MODE_MESH)strcpy(buf, "Mesh");
	if(main_mode==MODE_SOLID)strcpy(buf, "Solid");
//	GfxFont_DrawString(buf, 400-(16*8), 300-12,
//		8, 8,  0, 255, 0, 255);

	if(main_mode!=main_lmode)
	{
		PDGLUI_SetWidgetPropertyS("bones", "mode",
			"label", buf);
		main_lmode=main_mode;
	}

	if(main_flushui)
	{
		PDGLUI_ClearItems("bones", "lst");
		for(i=0; i<main_skel->mdl->num_bones; i++)
		{
			if(!main_skel->mdl->bone_name[i])sprintf(buf, "<none>");
				else sprintf(buf, "%s", main_skel->mdl->bone_name[i]);
			PDGLUI_AddItem("bones", "lst", buf, buf);
		}
		PDGLUI_SetWidgetPropertyI("bones", "lst",
			"value", main_skel->bone_sel);

		main_flushui=0;
	}


	if(main_loadsave)
	{
		UI_LoadSave_Draw();
		s=UI_LoadSave_GetName();

		if(s)
		{
			if(*s && (main_loadsave==1))
			{
				bones_file=s;
				Skel_LoadBones(main_skel->mdl, s);
			}
			if(*s && (main_loadsave==2))
			{
				bones_file=s;
//				Skel_SaveBones(main_skel->mdl, s);
				Skel_SaveStateBones(main_skel, s);
			}
			if(*s && (main_loadsave==3))
			{
				frame_file=s;
				main_skel->anim=Skel_LoadAnim(
					main_skel->mdl, s);
			}
			if(*s && (main_loadsave==4))
			{
				frame_file=s;
//				main_save_anim(s);
				Skel_SaveAnim(main_skel->mdl, main_skel->anim,
					s);
			}
			if(*s && (main_loadsave==5))
			{
				model_file=s;
//				main_load_model(s);
			}
			if(*s && (main_loadsave==6))
			{
				model_file=s;
//				main_save_model(s);
			}

			if(*s && (main_loadsave==7))
			{
//				main_load_model(s);
			}
			if(*s && (main_loadsave==8))
			{
//				main_save_model(s);
			}

			if(*s && (main_loadsave==9))
			{
				gtmp=AC3D_LoadModel(s);

				if(main_skel->bone_sel>=0)
				{
					main_skel->mdl->mesh_grp[main_skel->bone_sel]=gtmp;
				}else
				{
					main_load_rootmesh(s);
				}
			}

			if(*s && (main_loadsave==10))
			{
				LBXGL_SkelSmd_LoadReference(
					main_skel->mdl, s);
			}
			if(*s && (main_loadsave==11))
			{
				main_skel->anim=LBXGL_SkelSmd_LoadAnim(
					main_skel->mdl, s);
			}

			main_loadsave=0;
		}
	}

	PDGLUI_Frame();

	main_handle_events();


	pdglDisableTexture2D();
	pdglColor4f(1, 0, 0, 1);
	pdglBegin(PDGL_LINES);
	pdglVertex2f(state->mx-10, -state->my);
	pdglVertex2f(state->mx+10, -state->my);
	pdglVertex2f(state->mx, -state->my-10);
	pdglVertex2f(state->mx, -state->my+10);
	pdglEnd();

/*
	if(state->doshot)
	{
		Tex_DoScreenshot();
		state->doshot=0;
	}
*/

	return(0);
}

int pdgl_main_shutdown()
{
	dyPrintf("app1 shutdown\n");
	return(0);
}
