
// #include <pdlib.h>

#include <pdgl.h>
#include <pdglui.h>
#include <pdgl_stub.h>

#ifdef _WIN32
#include <windows.h>
//#include <GL/gl.h>
#endif

#include <stdio.h>
#include <stdlib.h>
#include <math.h>

float main_time;
float main_ltime;

int pdgl_main_startup(int argc, char *argv[])
{
	float v0[3], v1[3], v2[3], f, g, h;
	char *opts[4];
	int i, j, k, l;

	GfxFont_Init();
	Con_Init();
	ConCmds_Init();

//	PDSCR_Interp_Init();
//	PDGLUE_Init();

	PDGLUI_Init();

	PDGLUI_BeginForm("window", "tst0");
	PDGLUI_FormPropertyS("title", "Test Window 0");

	PDGLUI_BeginWidget("vbox", NULL);

/*
	PDGLUI_BeginWidget("textarea", NULL);
	PDGLUI_WidgetPropertyI("status", 1);
	PDGLUI_EndWidget();
*/

	PDGLUI_BeginWidget("button", "btn0");
	PDGLUI_WidgetPropertyS("label", "&A Test");
	PDGLUI_EndWidget();

	PDGLUI_BeginWidget("checkbox", "chk0");
	PDGLUI_WidgetPropertyS("label", "&B Test1");
	PDGLUI_EndWidget();

	PDGLUI_BeginWidget("checkbox", "chk1");
	PDGLUI_WidgetPropertyS("label", "&C Test2");
	PDGLUI_EndWidget();

	PDGLUI_BeginWidget("textbox", "txt0");
//	PDGLUI_WidgetPropertyS("label", "&D Test2");
	PDGLUI_EndWidget();

	PDGLUI_BeginWidget("radio", NULL);
	PDGLUI_WidgetPropertyS("label", "&E Radio 0");
	PDGLUI_WidgetPropertyS("name", "rad");
	PDGLUI_WidgetPropertyS("value", "rad0");
	PDGLUI_EndWidget();

	PDGLUI_BeginWidget("radio", NULL);
	PDGLUI_WidgetPropertyS("label", "&F Radio 1");
	PDGLUI_WidgetPropertyS("name", "rad");
	PDGLUI_WidgetPropertyS("value", "rad1");
	PDGLUI_EndWidget();

	PDGLUI_BeginWidget("radio", NULL);
	PDGLUI_WidgetPropertyS("label", "&G Radio 2");
	PDGLUI_WidgetPropertyS("name", "rad");
	PDGLUI_WidgetPropertyS("value", "rad2");
	PDGLUI_EndWidget();

	PDGLUI_BeginWidget("label", NULL);
	PDGLUI_WidgetPropertyS("label", "Label 0");
	PDGLUI_EndWidget();

	PDGLUI_BeginWidget("list", "lst0");
	PDGLUI_WidgetPropertyI("rows", 8);
	PDGLUI_WidgetPropertyI("cols", 16);

	PDGLUI_Item("it0", "Item 0");
	PDGLUI_Item("it1", "Item 1");
	PDGLUI_Item("it2", "Item 2");
	PDGLUI_EndWidget();

	PDGLUI_EndWidget();
	PDGLUI_EndForm();




	PDGLUI_BeginForm("fixedbox", "tst1");
	PDGLUI_FormProperty2I("origin", -400, 300);

	PDGLUI_BeginWidget("vbox", NULL);

	PDGLUI_BeginWidget("button", "btn0");
	PDGLUI_WidgetPropertyS("label", "Test");
	PDGLUI_EndWidget();

	PDGLUI_BeginWidget("hbox", NULL);

	PDGLUI_BeginWidget("button", "btn_a");
	PDGLUI_WidgetPropertyS("label", "A");
	PDGLUI_EndWidget();

	PDGLUI_BeginWidget("button", "btn_b");
	PDGLUI_WidgetPropertyS("label", "B");
	PDGLUI_EndWidget();

	PDGLUI_BeginWidget("button", "btn_c");
	PDGLUI_WidgetPropertyS("label", "C");
	PDGLUI_EndWidget();

	PDGLUI_EndWidget();

	PDGLUI_BeginWidget("union", "un_0");
	PDGLUI_WidgetPropertyS("value", "lbl_a");

	PDGLUI_BeginWidget("label", "lbl_a");
	PDGLUI_WidgetPropertyS("label", "Alpha");
	PDGLUI_EndWidget();

	PDGLUI_BeginWidget("label", "lbl_b");
	PDGLUI_WidgetPropertyS("label", "Beta");
	PDGLUI_EndWidget();

	PDGLUI_BeginWidget("label", "lbl_c");
	PDGLUI_WidgetPropertyS("label", "Gamma");
	PDGLUI_EndWidget();

	PDGLUI_EndWidget();
	

	PDGLUI_EndWidget();
	PDGLUI_EndForm();


	PDGLUI_BeginMenu("menu");
	PDGLUI_MenuItem("foo", "&Foo");
	PDGLUI_MenuItem("bar", "&Bar");
	PDGLUI_MenuItem("baz", "&Baz");

	PDGLUI_MenuItem(NULL, NULL);

	PDGLUI_BeginSubMenu("&FooBar");
	PDGLUI_MenuItem("foo", "&FooFoo");
	PDGLUI_MenuItem("bar", "&FooBar");
	PDGLUI_MenuItem("baz", "&FooBaz");
	PDGLUI_EndSubMenu();

	PDGLUI_BeginSubMenu("&FooBaz");
	PDGLUI_MenuItem("foo", "&BarFoo");
	PDGLUI_MenuItem("bar", "&BarBar");
	PDGLUI_MenuItem("baz", "&BarBaz");
	PDGLUI_EndSubMenu();

	PDGLUI_EndMenu();

//	PDGLUI_ShowMenu("menu", -300, 200);

	return(1);
}

int main_handle_input()
{
	char buf[64];

	int i, j, k;
	unsigned short *kcur;

	if(Con_IsDown())
		return(0);

	kcur=pdgl_keybuf;
	while(*kcur)
	{
		if(*kcur==K_ESCAPE)
		{
			main_kill=1;
			return(-1);
		}

		if(*kcur==K_F3)
			PDGL_Sound_Pop();

		kcur++;
	}

	if(state->mb&4)
		PDGLUI_ShowMenu("menu", state->mx, -state->my);
}

int pdgl_main_body()
{
	static int t;
	char buf[64];

	int i, j, k;
	unsigned short *kcur;
	char *s0, *s1, *s2;

//	printf("frame\n");

	main_handle_input();

	while(PDGLUI_HasEvent("tst0"))
	{
		s0=PDGLUI_GetEventWidget("tst0");
		s1=PDGLUI_GetEventName("tst0");
		i=PDGLUI_GetEventIVal("tst0");
		s2=PDGLUI_GetEventSVal("tst0");
		PDGLUI_NextEvent("tst0");

//		kprint("%s %s %d %s\n", s0, s1, i, s2?s2:"");
		if(!s0)continue;

		if(!strcmp(s0, "btn0"))
			PDGLUI_ClearItems("tst0", "lst0");

		if(!strcmp(s0, "txt0") && !strcmp(s1, "submit"))
		{
			PDGLUI_AddItem("tst0", "lst0", s2, s2);
			PDGLUI_SetWidgetPropertyS(
				"tst0", "txt0", "text", "");
		}
	}

	while(PDGLUI_HasEvent("tst1"))
	{
		s0=PDGLUI_GetEventWidget("tst1");
		s1=PDGLUI_GetEventName("tst1");
		i=PDGLUI_GetEventIVal("tst1");
		s2=PDGLUI_GetEventSVal("tst1");
		PDGLUI_NextEvent("tst1");

//		kprint("%s %s %d %s\n", s0, s1, i, s2?s2:"");
		if(!s0)continue;

		if(!strcmp(s0, "btn_a"))
			PDGLUI_SetWidgetPropertyS(
				"tst1", "un_0", "value", "lbl_a");
		if(!strcmp(s0, "btn_b"))
			PDGLUI_SetWidgetPropertyS(
				"tst1", "un_0", "value", "lbl_b");
		if(!strcmp(s0, "btn_c"))
			PDGLUI_SetWidgetPropertyS(
				"tst1", "un_0", "value", "lbl_c");
	}

	Draw_SetSolid2_2D(4.0/3.0, 400,
		VEC3(0, 0, 0), VEC3(0, 0, 0), 0, 0, pdgl_xs, pdgl_ys);

	Con_Render();

	GfxFont_SetFont("fixed", 0);

	PDGLUI_Frame();

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
