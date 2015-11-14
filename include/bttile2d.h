#include <btgecm.h>
#include <btgesv.h>

#include <pdgl.h>
#include <pdglui.h>

#include <lbxgl2.h>
#include <libvecmath.h>

#ifdef WIN32
#ifdef BT2D_DLL
#define BT2D_API __declspec(dllexport)
#else
#define BT2D_API __declspec(dllimport)
#endif
#else
#define BT2D_API
#endif

#ifndef BTTILE2D_H
#define BTTILE2D_H

#define TILE2D_EFL_BLOCKED			1
#define TILE2D_EFL_ONGROUND			2
#define TILE2D_EFL_TAKEDAMAGE		4

#define TILE2D_QFL_SKIPNONSOLID		1
#define TILE2D_QFL_SKIPOWNER		2

#define TILE2D_MOD_BULLET			1

#define TILE2D_SOLIDTYPE_NOT		0
#define TILE2D_SOLIDTYPE_BBOX		1
#define TILE2D_SOLIDTYPE_SLIDEBOX	2
#define TILE2D_SOLIDTYPE_TRIGGER	3

#define TILE2D_MOVETYPE_NONE		0
#define TILE2D_MOVETYPE_WALK		1

typedef struct Tile2D_Entity_s Tile2D_Entity;

struct Tile2D_Entity_s
{
Tile2D_Entity *next;
Tile2D_Entity *chain;
Tile2D_Entity *owner;
Tile2D_Entity *other;

vec2 org;		//origin
vec2 vel;		//velocity
vec2 mins;		//bbox mins
vec2 maxs;		//bbox maxs

int solidtype;
int movetype;
int flags;
int impulse;

//misc
float damage;
float health;
int frame;

dycObject obj;

//timers
float timer_die;				//timer before despawning
float timer_attackFinished;

//sprite state
int spr_tex;		//sprite frame texture
vec2 spr_size;		//sprite x size
vec2 spr_center;	//sprite center
byte spr_angle;		//sprite angle (256 degrees)

void (*Draw)(Tile2D_Entity *self);				//redraw
void (*Update)(Tile2D_Entity *self, float dt);	//dynamic update
void (*AnimTick)(Tile2D_Entity *self);			//10Hz animation tick
void (*Think)(Tile2D_Entity *self);				//Think event (timer based)
void (*Touch)(Tile2D_Entity *self,
	Tile2D_Entity *other);						//Touched an entity
void (*Blocked)(Tile2D_Entity *self,
	Tile2D_Entity *other);						//Blocked by entity
void (*Pain)(Tile2D_Entity *self,
	Tile2D_Entity *other,
	float damage, int mod);					//Took damage
void (*Die)(Tile2D_Entity *self,
	Tile2D_Entity *other,
	float damage, int mod);					//Died
};

#include <bt2d_auto.h>

#endif
