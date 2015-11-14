#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#ifdef _MSC_VER
#include <float.h>
#endif

#include <bgbdy.h>
#include <bgbsvm.h>

#include <bgbnet.h>

// #include <libbsde.h>
#include <libvecmath.h>

#ifndef BTGECM_H
#define BTGECM_H

#ifndef BTGE_METERS_PER_INCH
#define BTGE_METERS_PER_INCH	0.0375
#define BTGE_DENSITY_MG_M3		1.0
#endif

#define BTGE_SEFL_DIRTY			1
#define BTGE_SEFL_NEW			2

#define BT_SEQEV_UNLINK			1		//sequenced event unlinks itself

#define BTGE_DO2D_SKY			1		//draw in skybox
#define BTGE_DO2D_HIDDEN		2		//do not draw

#define BTGE_DO2D_MODE_BONES	1
#define BTGE_DO2D_MODE_ANIMATE	2
#define BTGE_DO2D_MODE_MESH		3
#define BTGE_DO2D_MODE_SOLID	4
#define BTGE_DO2D_MODE_OBJECT	5

#define BTGE_DO2D_MODE_FIRST	1
#define BTGE_DO2D_MODE_LAST		5

#ifdef __cplusplus
extern "C" {
#endif

typedef struct BTGE_SEntPair_s BTGE_SEntPair;
typedef struct BTGE_SEntity_s BTGE_SEntity;

typedef struct BTGE_ItemInfo_s BTGE_ItemInfo;
typedef struct BTGE_Inventory_s BTGE_Inventory;

typedef struct BTGE_Entity_s BTGE_Entity;
typedef struct BTGE_EntityNode_s BTGE_EntityNode;

// typedef struct BTGE_Entity_Iface_s BTGE_Entity_Iface;
// typedef struct BTGE_Actor_Iface_s BTGE_Actor_Iface;

typedef struct BTGE_Brush_s BTGE_Brush;
typedef struct BTGE_BrushModel_s BTGE_BrushModel;
typedef struct BTGE_BrushWorld_s BTGE_BrushWorld;

typedef struct BTGE_Light_s BTGE_Light;
typedef struct BTGE_LightNode_s BTGE_LightNode;

typedef struct BTGE_ModelState_s BTGE_ModelState;

// typedef struct BTGE_MoveFrame_s BTGE_MoveFrame;
// typedef struct BTGE_MoveInfo_s BTGE_MoveInfo;

typedef struct BTGE_VoxelInfo_s BTGE_VoxelInfo;
typedef struct BTGE_VoxelChunk_s BTGE_VoxelChunk;
typedef struct BTGE_VoxelChunkMesh_s BTGE_VoxelChunkMesh;
typedef struct BTGE_VoxelRegion_s BTGE_VoxelRegion;
typedef struct BTGE_VoxelData_s BTGE_VoxelData;

typedef struct BTGE_VoxelElasticNode_s BTGE_VoxelElasticNode;
typedef struct BTGE_VoxelElasticVertex_s BTGE_VoxelElasticVertex;
typedef struct BTGE_VoxelElasticFace_s BTGE_VoxelElasticFace;

typedef struct BTGE_VoxelBiomeInfo_s BTGE_VoxelBiomeInfo;
typedef struct BTGE_VoxelOreInfo_s BTGE_VoxelOreInfo;
typedef struct BTGE_VoxelTerrGen_s BTGE_VoxelTerrGen;
typedef struct BTGE_NoiseContext_s BTGE_NoiseContext;

typedef struct BTCMUI_Context_s BTCMUI_Context;
typedef struct BTCMUI_Event_s BTCMUI_Event;
typedef struct BTCMUI_Form_s BTCMUI_Form;
typedef struct BTCMUI_WidgetType_s BTCMUI_WidgetType;
typedef struct BTCMUI_Widget_s BTCMUI_Widget;

typedef struct BTGE_SequencedEvent_s BTGE_SequencedEvent;
typedef struct BTGE_DrawableObject2D_s BTGE_DrawableObject2D;


typedef void *BTCL_World_Dummy;
typedef void *BTCL_BrushWorld_Dummy;

typedef dytname("btge_iteminfo_t") as_variant
	BTGE_ItemInfo *btItem;
typedef dytname("btge_entity_t") as_variant
	BTGE_Entity *btEntity;
typedef dytname("btge_entnode_t") as_variant
	BTGE_EntityNode *btEntityNode;
typedef dytname("btge_sentity_t") as_variant
	BTGE_SEntity *btSEntity;
typedef dytname("btge_light_t") as_variant
	BTGE_Light *btLight;
typedef dytname("btge_brush_world_t") as_variant
	BTGE_BrushWorld *btWorld;

typedef dytname("btcmui_context_t") as_variant
	BTCMUI_Context *btguiContext;
typedef dytname("btcmui_event_t") as_variant
	BTCMUI_Event *btguiEvent;
typedef dytname("btcmui_form_t") as_variant
	BTCMUI_Form *btguiForm;
typedef dytname("btcmui_widgettype_t") as_variant
	BTCMUI_WidgetType *btguiWidgetType;
typedef dytname("btcmui_widget_t") as_variant
	BTCMUI_Widget *btguiWidget;

typedef dytname("btge_drawableobject2d_t") as_variant
	BTGE_DrawableObject2D *btDrawableObject2D;


typedef struct BTGE_DrawSequence2D_FramePos_s BTGE_DrawSequence2D_FramePos;
typedef struct BTGE_DrawSequence2D_Frame_s BTGE_DrawSequence2D_Frame;
typedef struct BTGE_DrawSequence2D_s BTGE_DrawSequence2D;

typedef dytname("btge_sequence2d_framepos_t") as_variant
	BTGE_DrawSequence2D_FramePos *btSequence2DFramePos;
typedef dytname("btge_sequence2d_frame_t") as_variant
	BTGE_DrawSequence2D_Frame *btSequence2DFrame;
typedef dytname("btge_sequence2d_t") as_variant
	BTGE_DrawSequence2D *btSequence2D;


struct BTGE_SEntPair_s {
BTGE_SEntPair *next;
char *name;
char *value;
int flags;
};

struct BTGE_SEntity_s {
BTGE_SEntity *next;	//physical next link
BTGE_SEntPair *keys;
int flags;
int idnum;

BTGE_Brush *brush;
BTGE_Brush *brush_last;
BTGE_ModelState *mdl;
BTGE_BrushModel *bmdl;
BTGE_BrushWorld *wrl;		//owning world
BTGE_Light *light;
BTGE_VoxelRegion *rgn;		//owning region
};

struct BTGE_SequencedEvent_s
{
BTGE_SequencedEvent *next;
BTGE_SequencedEvent *parent;
float wait;
char **cmds;
int n_cmds;
int m_cmds;
char **vars_var;
char **vars_val;
int n_vars;
int m_vars;
int flags;
char *wait_eval;
int wait_sound;
//dyt wait_cb;
};

struct BTGE_DrawableObject2D_s
{
BTGE_BrushWorld *world;
BTGE_DrawableObject2D *next;
BTGE_DrawableObject2D *chain;
BTGE_DrawableObject2D *seqnext;
BTGE_DrawSequence2D *seq;
int entid;
// float ox, oy;
float ord;
// float xs, ys;
int tex;
int flags;
int stateflags;		//current state flags
void *data;
vec2 org;			//origin
vec2 vel;			//velocity
vec2 size;			//size
vec2 dir;			//direction
vec2 coffs;			//center offset
char *imgname;
float rate;
float accDt;
float angle;		//current angle
float angvel;		//angular velocity
float scale;
dyt dy_this;		//object used if available.
int sound_chan;		//sound channel

void (*Draw)(BTGE_DrawableObject2D *self);
void (*Destroy)(BTGE_DrawableObject2D *self);
void (*Update)(BTGE_DrawableObject2D *self, float dt);
int (*HandleInput)(BTGE_DrawableObject2D *self);
};

struct BTGE_DrawSequence2D_FramePos_s
{
vec2 org;
float angle;
float scale;
int statefl;
};

struct BTGE_DrawSequence2D_Frame_s
{
int n_pos, m_pos;
BTGE_DrawSequence2D_FramePos **entpos;
};

struct BTGE_DrawSequence2D_s
{
BTGE_DrawableObject2D *entity;
int n_frame, m_frame;
BTGE_DrawSequence2D_Frame **frame;

double fps, fdt;
double adt;
int cur_frame;
double frame_time;

int n_entity, m_entity;

//UI state
int mode;
int sel_entid;
};

#include <btgecm_conf.h>

#include <btge_mesh.h>
#include <btge_brush.h>
#include <btge_voxel.h>
#include <btge_voxdung.h>
#include <btgecm_concmd.h>
#include <btgecm_keys.h>

#include <btgecm_gui.h>
#include <btgecm_bsglue.h>

#include <btgecm_filecache.h>

#include <btgecm_auto.h>

#ifdef __cplusplus
}
#endif

#endif
