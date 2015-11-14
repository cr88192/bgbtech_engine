#include <bgbdy.h>
#include <bgbsvm.h>
#include <libvecmath.h>

#include <btgecm.h>

#ifndef BTGESV_H
#define BTGESV_H

// #define BTGE_API
// #define BTGE_SPAWN

#include <btgesv_conf.h>
#include <btge_shared.h>

#ifdef __cplusplus
extern "C" {
#endif

#define BT_SKILL_EASY		0
#define BT_SKILL_MEDIUM		1
#define BT_SKILL_HARD		2
#define BT_SKILL_NIGHTMARE	3

#define BT_SKILL_DEATHMATCH	16
#define BT_SKILL_COOP		32

#define BTGE_METERS_PER_INCH	0.0375
#define BTGE_DENSITY_MG_M3		1.0

#define BT_SPAWNFLAG_ITEM_TRIGGER_SPAWN		0x00000001
#define BT_SPAWNFLAG_ITEM_NO_TOUCH			0x00000002
#define	BT_SPAWNFLAG_NOTEASY				0x00000100
#define	BT_SPAWNFLAG_NOTMEDIUM				0x00000200
#define	BT_SPAWNFLAG_NOTHARD				0x00000400
#define	BT_SPAWNFLAG_NOTDEATHMATCH			0x00000800
#define	BT_SPAWNFLAG_NOTCOOP				0x00001000
#define BT_SPAWNFLAG_DROPPED_ITEM			0x00010000
#define	BT_SPAWNFLAG_DROPPED_PLAYER_ITEM	0x00020000
#define BT_SPAWNFLAG_ITEM_TARGETS_USED		0x00040000

#define BT_SPAWNFLAG_DOOR_START_OPEN		0x00000001
#define BT_SPAWNFLAG_DOOR_REVERSE			0x00000002
#define BT_SPAWNFLAG_DOOR_DONT_LINK			0x00000004
#define BT_SPAWNFLAG_DOOR_GOLD_KEY			0x00000008
#define BT_SPAWNFLAG_DOOR_SILVER_KEY		0x00000010
#define BT_SPAWNFLAG_DOOR_TOGGLE			0x00000020
#define BT_SPAWNFLAG_DOOR_X_AXIS			0x00000040
#define BT_SPAWNFLAG_DOOR_Y_AXIS			0x00000080

#define BT_SPAWNFLAG_DOOR_CRUSHER			0x00010000
#define BT_SPAWNFLAG_DOOR_NOMONSTER			0x00020000
//#define BT_SPAWNFLAG_DOOR_TOGGLE			0x00040000

//note: func_button, uses same spawnflags as door

#define BT_SPAWNFLAG_SECRET_OPEN_ONCE		0x00000001
#define BT_SPAWNFLAG_SECRET_FIRST_LEFT		0x00000002
#define BT_SPAWNFLAG_SECRET_FIRST_DOWN		0x00000004
#define BT_SPAWNFLAG_SECRET_NOT_SHOOT		0x00000008
#define BT_SPAWNFLAG_SECRET_ALWAYS_SHOOT	0x00000010

#define BT_SPAWNFLAG_TRAIN_START_ON			0x00000001
#define BT_SPAWNFLAG_TRAIN_TOGGLE			0x00000002
#define BT_SPAWNFLAG_TRAIN_BLOCK_STOPS		0x00000004

#define BT_SPAWNFLAG_LIGHT_START_OFF		0x00000001

#define BT_SPAWNFLAG_TRIGGER_SILENT			0x00000001
#define BT_SPAWNFLAG_TRIGGER_ENABLE			0x00000004

#define BT_SPAWNFLAG_TRIGGER_MONSTER		0x00000001
#define BT_SPAWNFLAG_TRIGGER_NO_CLIENT		0x00000002
#define BT_SPAWNFLAG_TRIGGER_TRIGGERED		0x00000004

#define BT_SPAWNFLAG_TELEPORT_ONLYCLIENT	0x00000001
#define BT_SPAWNFLAG_TELEPORT_SILENT		0x00000002

#define BT_SPAWNFLAG_PUSH_ONCE				0x00000001

#define BT_SPAWNFLAG_ROTATING_START_ON		0x00000001
#define BT_SPAWNFLAG_ROTATING_REVERSE		0x00000002
#define BT_SPAWNFLAG_ROTATING_X_AXIS		0x00000004
#define BT_SPAWNFLAG_ROTATING_Y_AXIS		0x00000008
#define BT_SPAWNFLAG_ROTATING_ACCEL			0x00000010
#define BT_SPAWNFLAG_ROTATING_PAIN			0x00000020
#define BT_SPAWNFLAG_ROTATING_NONSOLID		0x00000040

#define BT_SPAWNFLAG_DOOR_ROTATING_REVERSE		0x00000002
#define BT_SPAWNFLAG_DOOR_ROTATING_ONE_WAY		0x00000010
#define BT_SPAWNFLAG_DOOR_ROTATING_X_AXIS		0x00000040
#define BT_SPAWNFLAG_DOOR_ROTATING_Y_AXIS		0x00000080

#define BT_SPAWNFLAG_SPEAKER_LOOP_ON		0x00000001
#define BT_SPAWNFLAG_SPEAKER_LOOP_OFF		0x00000002
#define BT_SPAWNFLAG_SPEAKER_RELIABLE		0x00000004


// #define BT_SPAWNFLAG_DOOR_CRUSHER		4	//Q2
// #define BT_SPAWNFLAG_DOOR_NOMONSTER		8	//Q2
// #define BT_SPAWNFLAG_DOOR_TOGGLE			32	//Q2

//#define BT_SPAWNFLAG_DOOR_DONT_LINK		0x10000
//#define BT_SPAWNFLAG_DOOR_GOLD_KEY		0x20000
//#define BT_SPAWNFLAG_DOOR_SILVER_KEY	0x40000


#define BT_ITFL_WEAPON1			1		//weapon 1
#define BT_ITFL_WEAPON2			2		//weapon 2
#define BT_ITFL_WEAPON3			4		//weapon 3
#define BT_ITFL_WEAPON4			8		//weapon 4
#define BT_ITFL_WEAPON5			16		//weapon 5
#define BT_ITFL_WEAPON6			32		//weapon 6
#define BT_ITFL_WEAPON7			64		//weapon 7
#define BT_ITFL_WEAPON8			128		//weapon 8
#define BT_ITFL_WEAPON9			256		//weapon 9
#define BT_ITFL_WEAPON10		512		//weapon 10

#define BT_ITFL_KEY1			1024	//key 1
#define BT_ITFL_KEY2			2048	//key 2
#define BT_ITFL_KEY3			4096	//key 3
#define BT_ITFL_KEY4			8192	//key 4

#define BT_ITFL_HAMMERDRILL		1		//hammer-drill / jackhammer
#define BT_ITFL_BLASTER			2		//blaster
#define BT_ITFL_SHOTGUN			4		//shotgun
#define BT_ITFL_CHAINGUN		8		//chaingun
#define BT_ITFL_ROCKETLAUNCHER	16		//rocket launcher
#define BT_ITFL_PLASMAGUN		32		//plasma gun
#define BT_ITFL_PLASMACANNON	64		//plasma-cannon
#define BT_ITFL_LASERGUN		128		//lasergun
#define BT_ITFL_ROCKETSHOVEL	256		//lasergun

#define BT_ITFL_GOLDKEY			1024	//key 1
#define BT_ITFL_SILVERKEY		2048	//key 2

#define BT_ITFL_GIVEALL			(BT_ITFL_WEAPON1|BT_ITFL_WEAPON2| \
	BT_ITFL_WEAPON3|BT_ITFL_WEAPON4|BT_ITFL_WEAPON5|BT_ITFL_WEAPON6| \
	BT_ITFL_WEAPON7|BT_ITFL_WEAPON8|BT_ITFL_WEAPON9|BT_ITFL_WEAPON10| \
	BT_ITFL_KEY1|BT_ITFL_KEY2|BT_ITFL_KEY3|BT_ITFL_KEY4)

#define BT_ITFL_CONSUMABLE		0x01000000	//key 1


#define BT_FL_FLY				1		//flying ent, no gravity, ignore onground
#define BT_FL_SWIM				2		//swim ent, no gravity in water
#define BT_FL_CLIENT			4		//entity is a player
#define BT_FL_INWATER			8		//currently in water
#define BT_FL_MONSTER			16		//is an NPC/monster
#define BT_FL_INVULNERABLE		32		//ignore damage
#define BT_FL_NOTARGET			64		//ignored by enemies
#define BT_FL_ITEM				128		//is an item
#define BT_FL_ONGROUND			256		//is sitting on the ground
#define BT_FL_PARTIALGROUND		512		//is hanging off a ledge
#define BT_FL_WATERJUMP			1024	//jumped in water
#define BT_FL_JUMPRELEASED		2048
#define BT_FL_NOCLIP			4096	//has noclip set
#define BT_FL_SCRIPTED			8192	//locked into a scripted sequence
#define BT_FL_CREATIVE			16384	//has world-editing abilities
#define BT_FL_FASTMP			32768	//rapid MP regeneration

#define BT_FL_SIMPLEMOVE		65536	//simple/direct movement
#define BT_FL_OFFGROUND			131072	//walked off a ledge
#define BT_FL_JUMPMOVE			262144	//would require jumping
#define BT_FL_STARTSOLID		524288	//would require jumping

#define BT_DTFL_SENTID			1		//SEntity ID

#define BT_DTFL_STATICMASK		1		//SEntity ID


#define BT_FL_AI_PASSIVE		(1<<20)		//does not attack anyone
#define BT_FL_AI_NEUTRAL		(1<<21)		//will attack if angered
#define BT_FL_AI_GOODGUY		(1<<22)		//aligned with player
#define BT_FL_AI_FOLLOWER		(1<<23)		//follows player around
#define BT_FL_AI_ROAMING		(1<<24)		//roam when idle (nodes)
#define BT_FL_AI_NOCHASE		(1<<25)		//don't pursue enemies
#define BT_FL_AI_FREEROAM		(1<<26)		//roam freely (random directions)
#define BT_FL_AI_DESPAWN		(1<<27)		//despawn at a certain distance

#define BT_FL_SET_SOLID			(1<<28)		//solid was modified
#define BT_FL_SET_POSITION		(1<<29)		//position has been set manually

#define BT_FL2_RAILMOVE			1			//moves along rails
#define BT_FL2_PUSHABLE_XY		2			//pushable in XY direction
#define BT_FL2_PUSHABLE_Z		4			//pushable in Z direction
#define BT_FL2_STOPPORTAL		8			//temporily disable portal
#define BT_FL2_GUIMODE			16			//guimode
#define BT_FL2_CLIENTMOVE		32			//client-side movement
#define BT_FL2_VIEWHFLIP		64			//flip view horizontal
#define BT_FL2_VIEWVFLIP		128			//flip view horizontal

#define BT_MOVE_NONE			0	//no behavior
#define BT_MOVE_WALK			1	//NPC movement
#define BT_MOVE_STEP			2	//player movement
#define BT_MOVE_FLY				3	//flying along, no gravity
#define BT_MOVE_TOSS			4	//subject to gravity, lands on things
#define BT_MOVE_PUSH			5	//push things, used for SOLID_BSP
#define BT_MOVE_NOCLIP			6	//move along, ignore world
#define BT_MOVE_FLYMISSILE		7	//flying entity, expanded trigger box
#define BT_MOVE_BOUNCE			8	//subject to gravity, bounces on impact
#define BT_MOVE_BOUNCEMISSILE	9	//like bounce, expanded trigger box
#define BT_MOVE_RBD_PHYSICS		10	//rigid body physics
#define BT_MOVE_SLIDE			11	//slides along, subject to gravity

#define BT_SOLID_NOT				0	//non-solid, non-reactive
#define BT_SOLID_TRIGGER			1	//non-solid, responds to touch
#define BT_SOLID_BBOX				2	//solid AABB (usually stationary)
#define BT_SOLID_SLIDEBOX			3	//solid AABB (player/NPC)
#define BT_SOLID_BSP				4	//brushmodel entity

//the solids below may be orientated.
//note that 'rot' will be used for orientation, not angles
#define BT_SOLID_OBB				5	//solid OBB (rotates freely)
#define BT_SOLID_OBB_YAW			6	//solid OBB (yaw only)
#define BT_SOLID_SPHERE				7	//solid sphere (radius)
#define BT_SOLID_CYLINDER			8	//solid cylinder (radius)
#define BT_SOLID_CAPSULE			9	//solid capsule (radius)
#define BT_SOLID_HULL				10	//solid hull (polyhedron)
#define BT_SOLID_TRIMESH			11	//solid triangle mesh
#define BT_SOLID_QUADMESH			12	//solid quad mesh

#define BT_RANGE_MELEE				1
#define BT_RANGE_NEAR				2
#define BT_RANGE_MID				4
#define BT_RANGE_FAR				8

#define BT_DAMAGE_NO				0		//doesn't take damage
#define BT_DAMAGE_YES				1		//takes damage
#define BT_DAMAGE_AIM				2		//takes damage, show as target

//TRACE flags effect how traceline behaves
#define BT_TRACE_NOMONSTERS			0x0000	//trace ignores monsters
#define BT_TRACE_MONSTERS			0x0001	//trace finds monsters
#define BT_TRACE_CONT_NOSOLID		0x0002	//trace ignores solid geometry
#define BT_TRACE_CONT_NOWINDOW		0x0004	//trace ignores windows
#define BT_TRACE_CONT_NOFLUID		0x0008	//trace ignores fluids
#define BT_TRACE_CLIP				0x0010	//calculate trace ending position
#define BT_TRACE_TRIGGER			0x0020	//trace finds triggers
#define BT_TRACE_NONSOLID			0x0040	//trace finds non-solid ents
#define BT_TRACE_BSPSOLID			0x0080	//trace finds brush ents
#define BT_TRACE_STARTSOLID			0x0100

#define BT_CONTENTS_EMPTY			0x00000000
#define BT_CONTENTS_SOLID			0x00000001	//solid geometry
#define BT_CONTENTS_WINDOW			0x00000002	//translucent+solid windows
#define BT_CONTENTS_SKY				0x00000004	//sky view, solid/no-draw
#define BT_CONTENTS_WATER			0x00000008	//water-like fluid
#define BT_CONTENTS_SLIME			0x00000010	//burning slime
#define BT_CONTENTS_LAVA			0x00000020	//burning lava
#define BT_CONTENTS_MIST			0x00000040	//non-solid haze or effects
#define	BT_CONTENTS_ORIGIN			0x00000100	//origin brush/bmodel
#define	BT_CONTENTS_ALPHA			0x00000200	//alpha surfaces (translucent)
#define	BT_CONTENTS_PORTAL			0x00000400
#define	BT_CONTENTS_DETAIL			0x00000800	//cosmetic only (no BSP)
#define BT_CONTENTS_FLUID_FL		0x00001000	//object is fluid-like
#define BT_CONTENTS_FLUID_MASK		0x0000F000	//mask
#define BT_CONTENTS_FLUID_WATER		0x00001000	//slime
#define BT_CONTENTS_FLUID_SLIME		0x00003000	//slime
#define BT_CONTENTS_FLUID_LAVA		0x00005000	//lava
#define BT_CONTENTS_FLUID_SEWER		0x00007000	//dirty water
#define BT_CONTENTS_FLUID_PORTAL	0x00009000	//portal
#define BT_CONTENTS_FLUID_ELASTIC	0x0000B000	//elastic
#define BT_CONTENTS_EFFECT_MASK		0x000FF000	//mask
#define BT_CONTENTS_EFFECT_LVOL		0x00082000	//light volume
#define BT_CONTENTS_EFFECT_FOG		0x00084000	//fog volume
#define BT_CONTENTS_EFFECT			0x00080000	//special effect
#define	BT_CONTENTS_PLAYERCLIP		0x00100000
#define	BT_CONTENTS_MONSTERCLIP		0x00200000
#define BT_CONTENTS_NONSOLID		0x00400000	//non-solid objects
#define	BT_CONTENTS_MONSTER			0x00800000
#define	BT_CONTENTS_DEADMONSTER		0x01000000
#define	BT_CONTENTS_LADDER			0x02000000
#define	BT_CONTENTS_MOVE_0			0x04000000
#define	BT_CONTENTS_MOVE_90			0x08000000
#define	BT_CONTENTS_MOVE_180		0x10000000
#define	BT_CONTENTS_MOVE_270		0x20000000
#define	BT_CONTENTS_MOVE_UP			0x40000000
#define	BT_CONTENTS_MOVE_DOWN		0x80000000

#define BT_CONTENTS_PLAYERSOLID		(BT_CONTENTS_SOLID | \
									BT_CONTENTS_WINDOW | \
									BT_CONTENTS_SKY)

#define BT_CONTENTS_FLUID			(BT_CONTENTS_WATER | \
									BT_CONTENTS_SLIME | \
									BT_CONTENTS_LAVA | \
									BT_CONTENTS_FLUID_FL)

#define BT_CONTENTS_MONSTERSOLID	BT_CONTENTS_PLAYERSOLID
#define BT_CONTENTS_ITEMSOLID		BT_CONTENTS_PLAYERSOLID

#define BT_CONTENTS_CHECKSOLID		(BT_CONTENTS_PLAYERSOLID | \
									BT_CONTENTS_FLUID)

#define BT_STATE_NONE				0
#define BT_STATE_TOP				1
#define BT_STATE_BOTTOM				2
#define BT_STATE_UP					3
#define BT_STATE_DOWN				4
#define BT_STATE_ON					5
#define BT_STATE_OFF				6
#define BT_STATE_SPINUP				7
#define BT_STATE_SPINDOWN			8

#define BT_STATE_USER_A				16
#define BT_STATE_USER_B				17
#define BT_STATE_USER_C				18
#define BT_STATE_USER_D				19
#define BT_STATE_USER_E				20
#define BT_STATE_USER_F				21
#define BT_STATE_USER_G				22
#define BT_STATE_USER_H				23
#define BT_STATE_USER_I				24
#define BT_STATE_USER_J				25
#define BT_STATE_USER_K				26
#define BT_STATE_USER_L				27
#define BT_STATE_USER_M				28
#define BT_STATE_USER_N				29
#define BT_STATE_USER_O				30
#define BT_STATE_USER_P				31


#define BT_AS_STRAIGHT				1
#define BT_AS_SLIDING				2
#define BT_AS_MELEE					3
#define BT_AS_MISSILE				4

#define BT_MOD_UNKNOWN				0		//unknown means
#define BT_MOD_MELEE				1		//melee weapon
#define BT_MOD_DAMAGEGUN			2		//test weapon
#define BT_MOD_EXPLOSION			3		//generic explosion
#define BT_MOD_ROCKET				4		//rocket explosion
#define BT_MOD_GRENADE				5		//grenade explosion
#define BT_MOD_HURT					6		//trigger_hurt
#define BT_MOD_BULLET				7		//shot with bullets
#define BT_MOD_PLASMA				8		//burned with plasma
#define BT_MOD_BLASTER				9		//shot with blaster
#define BT_MOD_BIGPLASMA			10		//plasma-cannon / BFG
#define BT_MOD_DOGBITE				11		//mauled by dog
#define BT_MOD_LASER				12		//burned by laser
#define BT_MOD_DEMONBITE			13		//mauled by demon
#define BT_MOD_CRUSH				14		//crushed by a door/train
#define BT_MOD_LAVA					15		//burned by lava
#define BT_MOD_SLIME				16		//burned by slime
#define BT_MOD_WATER				17		//drowning
#define BT_MOD_IMPACT				18		//falling and similar
#define BT_MOD_TELEFRAG				19		//telefragged
#define BT_MOD_CHANGELEVEL			20		//tried to escape
#define BT_MOD_ROCKETSHOVEL			21		//rocket shovel

// typedef struct BTGE_SEntPair_s BTGE_SEntPair;
// typedef struct BTGE_SEntity_s BTGE_SEntity;

// typedef struct BTGE_ItemInfo_s BTGE_ItemInfo;
// typedef struct BTGE_Inventory_s BTGE_Inventory;

// typedef struct BTGE_Entity_s BTGE_Entity;
// typedef struct BTGE_EntityNode_s BTGE_EntityNode;

typedef struct BTGE_ClientInfo_s BTGE_ClientInfo;
typedef struct BTGE_MessageHandler_s BTGE_MessageHandler;

typedef struct BTGE_EntityDialog_s BTGE_EntityDialog;
typedef struct BTGE_DialogNode_s BTGE_DialogNode;
typedef struct BTGE_DialogInfo_s BTGE_DialogInfo;

// typedef struct BTGE_EntityActor_s BTGE_EntityActor;
// typedef struct BTGE_EntityTrigger_s BTGE_EntityTrigger;
// typedef struct BTGE_EntityPlayer_s BTGE_EntityPlayer;

typedef struct BTGE_Entity_Iface_s BTGE_Entity_Iface;
typedef struct BTGE_Actor_Iface_s BTGE_Actor_Iface;

// typedef struct BTGE_Brush_s BTGE_Brush;
// typedef struct BTGE_BrushModel_s BTGE_BrushModel;
// typedef struct BTGE_BrushWorld_s BTGE_BrushWorld;

// typedef struct BTGE_Light_s BTGE_Light;

typedef struct BTGE_MoveFrame_s BTGE_MoveFrame;
typedef struct BTGE_MoveInfo_s BTGE_MoveInfo;
typedef struct BTGE_MoveFrame_DyInfo_s BTGE_MoveFrame_DyInfo;

#if 1
typedef dytname("btge_moveframe_t") as_variant
	BTGE_MoveFrame *btMoveFrame;
typedef dytname("btge_moveinfo_t") as_variant
	BTGE_MoveInfo *btMoveInfo;
typedef dytname("btge_moveframe_dyinfo_t") as_variant
	BTGE_MoveFrame_DyInfo *btMoveFrameDyInfo;
#endif

#if 0
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
#endif

#if 1
typedef dytname("btge_dialoginfo_t") as_variant
	BTGE_DialogInfo *btDialogInfo;
typedef dytname("btge_dialognode_t") as_variant
	BTGE_DialogNode *btDialogNode;
#endif

typedef void (*btge_think)(btEntity self);
typedef void (*btge_use)(btEntity self, btEntity other);

#if 0
struct BTGE_SEntPair_s {
BTGE_SEntPair *next;
char *name;
char *value;
int flag;
};

struct BTGE_SEntity_s {
BTGE_SEntity *next;	//physical next link
BTGE_SEntPair *keys;
int flags;

BTGE_Brush *brush;
// BTGE_ModelState *mdl;
BTGE_BrushModel *bmdl;
BTGE_BrushWorld *wrl;
BTGE_Light *light;
};
#endif

struct BTGE_ItemInfo_s {
BTGE_ItemInfo *next;
BTGE_ItemInfo *chain;
char *classname;
int (*canPickup)(btEntity self, btEntity item);
void (*use)(btEntity self, btItem item);
void (*drop)(btEntity self, btItem item);
void (*heldThink)(btEntity self, btItem item);
void (*touchThink)(btEntity self, btItem item);
char *worldModel;	//what this looks like in-world
char *viewModel;	//what this looks like in the HUD
char *icon;			//image representing item
char *itemName;		//user-readable item name
char *ammoName;		//ammo type (weapons), base type (ammo/items)
char *touchSound;	//sound made when touched
int flags;			//item flags
int effects;		//effects flags for model
int amount;			//amount held for a given item
int maxAmount;		//maximum allowed in inventory
int dropWeight;		//relative weight of dropping this item
int dropChance;		//chance that this item will actually drop
int usageValue;		//value on use (HP/MP/AP/...)
dyt dy_this;		//object used if available.
};

struct BTGE_Inventory_s {
BTGE_Inventory *next;	//next inventory entry
BTGE_Inventory *chain;	//chained inventory entry (query)
char *classname;		//classname of inventory entry
char *subname;			//sub-name
int count;
int slot;
};

struct BTGE_EntityNode_s
{
btEntityNode next;
btEntityNode lnode;
btEntityNode rnode;
btEntity mid;
vec4d plane;
};

struct BTGE_EntityDialog_s
{
btEntity client;		//client entity
btEntity npc;			//npc entity
char *msg;				//current message
char *opt_msg[10];		//current option strings
char *opt_tgt[10];		//option targets
char *cur_img;			//current portrait image
char *cur_name;			//current portrait name
int state;				//state value
int impulse;			//impulse flags
BTGE_DialogInfo *info;	//dialogue info
};

struct BTGE_DialogNode_s
{
char *name;				//node name
char *message;			//node message string
char *image;			//node portrait image
char *opt_msg[10];		//node option names
char *opt_tgt[10];		//node option targets

void (*fcn_f)(btEntity self, BTGE_EntityDialog *info);
dyt fcn_dy;
};

struct BTGE_DialogInfo_s
{
BTGE_DialogNode **node;
int n_node;			//num nodes
int m_node;			//max nodes (0=fixed array)
};


struct BTGE_ClientInfo_s
{
BTGE_ClientInfo *next;	//next client
int clnum;				//client number
BTGE_Entity *entity;	//player entity

s64 reforg[3];			//refernce point origin
s64 oldreforg[3];		//old refernce point origin

byte *deltamap_ent;		//delta bitmap for entities (1=sync)
byte *deltamap_brush;	//delta bitmap for brushes
byte *deltamap_light;	//delta bitmap for lights
};

struct BTGE_MessageHandler_s {
BTGE_MessageHandler *next;	//next (in list)
dyt name;
void (*Handler)(btEntity self, dyt msg);
dyt dyHandler;
};


//NOTE: Be careful when updating this structure
//It is used by reflection and thus needs metadata to be up-to-date
struct BTGE_Entity_s
{
BTGE_Entity_Iface *vt;
BTGE_Actor_Iface *avt;

//BTGE_EntityTrigger *eTrig;
//BTGE_EntityActor *eAct;
//BTGE_EntityPlayer *ePlayer;

// BTGE_BrushModel *bmdl2;

//solid information
int entnum;				//entity number
int solidnum;			//physics solid number
int movetype;			//movetype
int solidtype;			//solidtype
int clientnum;			//client number
vec3d absmin, absmax;	//absolute bounding box (all solids)
vec3d origin;			//current origin
vec3d oldorigin;		//prior origin (deltas)
vec3 velocity;			//current velocity
vec3 oldvelocity;		//prior origin (deltas)
vec3 angles;			//angles (view/movement)
vec3 angles2;			//angles2 (brushmodel orientation)
vec3 oldangles;			//prior angles (delta)
vec3 oldangles2;		//prior angles2 (delta)
vec3 angvel;			//angular velocity
vec3 angvel2;			//angular velocity (angles2)
vec3 mins;				//bbox mins (AABB/OBB)
vec3 maxs;				//bbox maxs (AABB/OBB)
vec3 oldmins;			//old mins (deltas)
vec3 oldmaxs;			//old maxs (deltas)
quat rot;				//rotation quaternion
quat rotvel;			//rotation velocity
quat oldrot;			//rotation quaternion
quat oldrotvel;			//rotation velocity

float radius;			//radius (sphere, cylinder, capsule)

//movement and client
vec3 movedir;			//movement direction
vec3 impulseVelocity;	//client movement impulse
vec3 vieworg;			//view origin (offset of camera relative to origin)
vec3 oldvieworg;		//old view origin
vec3d morg, mend;		//mouse origin and end-point
vec3 mdir;				//mouse direction vector
vec3d spawnOrigin;		//for brush entities
vec3 spawnAngles;		//for brush entities
vec3 viewMins;			//view bbox mins (2D)
vec3 viewMaxs;			//view bbox maxs (2D)

vec3d origin2;			//alternate origin
vec3d oldorigin2;		//prior alternate origin (deltas)
quat rot2;				//rotation quaternion
quat rotvel2;			//rotation velocity
quat oldrot2;			//rotation quaternion
quat oldrotvel2;		//rotation velocity

char *classname;
char *model;
char *weaponmodel;
char *target;
char *targetname;
char *killtarget;
char *message;
char *map;
char *anim;
char *weaponanim;

char *oldmodel;
char *oldweaponmodel;
char *oldanim;
char *oldweaponanim;

char *sound;
char *landmark;

int frame;
int weaponframe;
int skin;
int effects;
int effects2;
int flags;
int flags2;
int spawnflags;
int itemflags;
int aiflags;
int team;
int oldframe;
int oldweaponframe;
int oldeffects;
int oldeffects2;

btge_think f_think;
float nextthink;
btge_use f_use;
btge_think f_think1;
dyt dy_think;
dyt dy_think1;
btge_think f_movethink;

int waterlevel;
int watertype;

float health;
float maxHealth;
int takedamage;
float mass;				//approx mass for entity (pounds)
float scale;			//model scale

float curMP;
float maxMP;
int weaponnum;

float curArmor;
float maxArmor;

float idealYaw;
float yawSpeed;
float speed;
float viewFOV;			//FOV for clients, visibility FOV for NPCs

float attackFinished;
float painFinished;
float airFinished;
float showHostile;
float portalFinished;

float lightlevel;		//for light sources
int lightstyle;			//light style for light sources
vec4 color;

btLight light;			//attached lightsource
btItem item;			//represented item for drops, held item for player

btEntity groundEntity;
btEntity chain;
btEntity enemy;
btEntity owner;
btEntity goalEntity;
btEntity moveTarget;
btEntity oldEnemy;

btEntity linkNext;
btEntity linkFirst;

btEntity entNextNode;
btEntityNode entNode;

BTGE_Inventory *items;
BTGE_MoveInfo *move;
BTGE_SEntity *sent;
BTGE_ClientInfo *client;
BTGE_MoveInfo *weaponmove;

dyt dy_this;			//this for BS code
dyt msg_queue;			//client-specific message queue
dyt dy_proto;			//prototype object (dynamic fields)

//trigger and similar
vec3d dest1;
vec3d dest2;
vec3d dest3;
vec3d finalDest;
vec3d pos1;
vec3d pos2;

int count;
int state;

float wait;
float delay;
float damage;
float lip;
float height;
float moveSpeed;
btEntity triggerEntity;

//sound related
char *snd_start;		//starting movement
char *snd_stop;			//ending movement
char *snd_sight;		//upon seeing enemy
char *snd_idle;			//upon standing idly
char *snd_missile;		//upon using a ranged attack
char *snd_melee;		//upon using a melee attack

char *ttsVoice;			//voice name for text-to-speech
char *ttsControl;		//voice control sequence for text-to-speech

float volStat;			//current volume status (0..1)

//actor
int attackState;
float pauseTime;
float searchTime;
float wanderTime;
float despawnTime;

btEntity teamLeader;

int teamColor[4];
int teamSkin[4];
int oldTeamColor[4];
int oldTeamSkin[4];

//player
// int walkframe;
float invincibleFinished;
float invisibleFinished;
float quadFinished;
float suitFinished;

int impulseFlags;	//client impulse flags
int deltaFlags;		//sent static fields

float moveDist;		//how far has player moved?
float walkDist;		//how far has player walked?
float stepDist;		//accumulated distance for footstep sound.

//misc
float strength;
int sounds;			//sounds/"theme"
char *material;		//material type
char *gibmodel;
char *use_eval;		//eval this string entity fires
float client_time;	//client-side time (as per last update)
int placer_vox;		//placer voxel type
int placer_aux;		//placer voxel aux
dyt placer_value;	//placer dynamic value

BTGE_EntityDialog *dlg;
BTGE_DialogInfo *dlg_info;
btguiContext gui_ctx;

int num_in;
int num_on;

char *in_event_name[4];
char *in_event_targ[4];
char *on_event_name[4];
char *on_event_targ[4];
float xform[16];		//current transformation matrix (rot, origin)
};

struct BTGE_Entity_Iface_s
{
void (*init)(btEntity self);
void (*deinit)(btEntity self);

void (*prethink)(btEntity self);
void (*think)(btEntity self);
void (*blocked)(btEntity self, btEntity other);
void (*touch)(btEntity self, btEntity other);
void (*use)(btEntity self, btEntity other);
void (*pain)(btEntity self, btEntity other, float damage);
void (*die)(btEntity self, btEntity other, float damage);
void (*cmdmsg)(btEntity self, btEntity other, char *str);
};

struct BTGE_Actor_Iface_s
{
void (*stand)(btEntity self);
void (*walk)(btEntity self);
void (*run)(btEntity self);
void (*missile)(btEntity self);
void (*melee)(btEntity self);
};

#if 0
struct BTGE_EntityTrigger_s
{
vec3 dest1;
vec3 dest2;
vec3 dest3;
vec3 finalDest;
vec3 pos1;
vec3 pos2;

int count;
int state;

float wait;
float delay;
float damage;
float lip;
float height;
btEntity triggerEntity;

char *snd_start;
char *snd_stop;
};

struct BTGE_EntityActor_s
{
int attackState;
float pauseTime;
float searchTime;
float wanderTime;
};

struct BTGE_EntityPlayer_s
{
int walkframe;
float invincibleFinished;
float invisibleFinished;
float quadFinished;
float suitFinished;
};
#endif

typedef struct BTGE_Trace_s BTGE_Trace;

struct BTGE_Trace_s
{
BTGE_Brush *brush;
btEntity ent;
vec3d org;
float fraction;
int contents;
int flags;
};

struct BTGE_MoveFrame_s
{
void (*f_ai)(btEntity self, float dist);
float dist;
void (*f_think)(btEntity self);
void *data;
};

struct BTGE_MoveInfo_s
{
char *anim;						//anim sequence in use
int startFrame;					//starting frame (in anim)
int endFrame;					//ending frame (last frame is endFrame-1)
BTGE_MoveFrame *frame;			//frames
void (*f_end)(btEntity self);	//called at end of sequence
void *data;
};

struct BTGE_MoveFrame_DyInfo_s
{
dyt dyf_ai;
dyt dyf_think;
dyt dyf_end;
};

// #include <btge_brush.h>
#include <btgesv_bsglue.h>

#include <btge_auto.h>

#ifdef __cplusplus
}
#endif

#endif
