#include <frbc2ci.h>

#ifdef _WIN32
#include <windows.h>
#endif

#ifndef FRBCIGL_H
#define FRBCIGL_H

#ifndef frgl_state
#define frgl_state	((FRGL_State *)FRGL_GetState())
#endif


typedef struct FRGL_State_s FRGL_State;

struct FRGL_State_s
{
int mx, my;		//current mouse pos
int lmx, lmy;	//mouse pos last frame
int omx, omy;	//last non-dragging pos
int dmx, dmy;	//delta mouse pos
int dx, dy;		//drag from lmx
int odx, ody;	//drag from omx

int rmx, rmy;	//"real" mouse pos
int rlmx, rlmy;	//"real" mouse pos last frame

int mb, lmb;
unsigned short *keys;

int orgx, orgy;
int zoom;
int time;

int xs, ys;		//screen x and y size
int lxs, lys;	//screen last x and y size
double dt_f;		//current frame delta time
double adt_f;	//avg frame time
double adt2_f;	//avg frame time 2
double time_f;	//current time

int doshot;
int kill;
int maxhz;

byte keymap[32];
};

#include <frbcigl_keys.h>

#endif
