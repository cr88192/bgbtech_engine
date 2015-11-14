//texture flags
#define BTGE_TXFL_NONE			0
#define BTGE_TXFL_ALPHA			1		//image uses alpha channel
#define BTGE_TXFL_VALPHA		2		//object uses alpha value
#define BTGE_TXFL_NONSOLID		4		//objects should be nonsolid
#define BTGE_TXFL_DETAIL		8		//objects should be detail
#define BTGE_TXFL_NOCSG			16		//object should not clip others
#define BTGE_TXFL_FULLBRIGHT	32		//don't lightmap surface
#define BTGE_TXFL_WINDOW		64		//object is window-like
#define BTGE_TXFL_WARPST		128		//warp st coords
#define BTGE_TXFL_GLOSS			256		//use surface gloss
#define BTGE_TXFL_NODRAW		512		//don't render
#define BTGE_TXFL_NEWSCREEN		1024	//GLSL: use new screen texture
#define BTGE_TXFL_KEEPSCREEN	2048	//GLSL: keep visible
#define BTGE_TXFL_FLUID			4096	//object is fluid-like
#define BTGE_TXFL_FLUID0		8192	//fluid-type bit 0
#define BTGE_TXFL_FLUID1		16384	//fluid-type bit 1
#define BTGE_TXFL_FLUID2		32768	//fluid-type bit 2
#define BTGE_TXFL_FLUID3		65536	//fluid-type bit 3
#define BTGE_TXFL_EFR0			131072	//effect reserved 0
#define BTGE_TXFL_EFR1			262144	//effect reserved 1
#define BTGE_TXFL_EFFECT		524288	//special effect
#define BTGE_TXFL_NOSHADOW		1048576	//don't render shadow

#define BTGE_TXFL_MATERIAL_MASK		0xFF000		//mask
#define BTGE_TXFL_MATERIAL_NONE		0x00000		//generic solid
#define BTGE_TXFL_MATERIAL_SKY		0x02000		//sky
#define BTGE_TXFL_MATERIAL_GLASS	0x04000		//glass
#define BTGE_TXFL_MATERIAL_WOOD		0x06000		//wood
#define BTGE_TXFL_MATERIAL_METAL	0x08000		//metal
#define BTGE_TXFL_MATERIAL_FLESH	0x0A000		//flesh
#define BTGE_TXFL_MATERIAL_CONCRETE	0x0C000		//concrete
#define BTGE_TXFL_MATERIAL_FOAM		0x0E000		//solid foam-like
#define BTGE_TXFL_MATERIAL_COMPUTER	0x10000		//computer/machine
#define BTGE_TXFL_MATERIAL_UBGLASS	0x12000		//unbreakable glass
#define BTGE_TXFL_MATERIAL_STONE	0x14000		//stone/rocks
#define BTGE_TXFL_MATERIAL_SOLID	0x7E000		//generic solid

#define BTGE_TXFL_FLUID_MASK		0xFF000		//mask
#define BTGE_TXFL_FLUID_WATER		0x01000		//slime
#define BTGE_TXFL_FLUID_SLIME		0x03000		//slime
#define BTGE_TXFL_FLUID_LAVA		0x05000		//lava
#define BTGE_TXFL_FLUID_SEWER		0x07000		//dirty water

#define BTGE_TXFL_EFFECT_MASK		0xFF000		//mask
#define BTGE_TXFL_EFFECT_LVOL		0x82000		//light volume
#define BTGE_TXFL_EFFECT_FOG		0x84000		//fog volume
#define BTGE_TXFL_EFFECT_ORIGIN		0x86000		//origin
#define BTGE_TXFL_EFFECT_ANTIPORTAL	0x88000		//antiportal

//face flags
#define BTGE_TXFL_NOPLANE	(1<<24)	//don't use face as plane

#define BTGE_TEX_PLANAR			0
#define BTGE_TEX_BOX			1
#define BTGE_TEX_SPHERICAL		2
#define BTGE_TEX_CYLINDRICAL	3
#define BTGE_TEX_MULTIPLANAR	4


#define BTGE_BRFL_SEL		1	//selected
#define BTGE_BRFL_OPSEL		2	//operator selected
#define BTGE_BRFL_DEL		4	//deleted
#define BTGE_BRFL_HIDDEN	8	//hidden
#define BTGE_BRFL_ALPHA		16	//brush is translucent
#define BTGE_BRFL_NODRAW	32	//don't render
#define BTGE_BRFL_NONSOLID	64	//objects should be nonsolid
#define BTGE_BRFL_ORIGIN	128	//brush is an origin brush
#define BTGE_BRFL_SKY		256	//brush is a sky brush

#define BTGE_BRFL_FLUID		4096	//brush is fluid
#define BTGE_BRFL_FLUID0	8192	//fluid-type bit 0
#define BTGE_BRFL_FLUID1	16384	//fluid-type bit 1
#define BTGE_BRFL_FLUID2	32768	//fluid-type bit 2
#define BTGE_BRFL_FLUID3	65536	//fluid-type bit 3
#define BTGE_BRFL_EFR0		131072	//effect reserved 0
#define BTGE_BRFL_EFR1		262144	//effect reserved 1
#define BTGE_BRFL_EFFECT	524288	//special effect
#define BTGE_BRFL_NOSHADOW	1048576	//don't render shadows


// typedef struct BTGE_SEntity_s BTGE_SEntity;
typedef struct BTGE_BrushNode_s BTGE_BrushNode;
// typedef struct BTGE_BrushModel_s BTGE_BrushModel;

typedef struct BTGE_BrushPatch_s BTGE_BrushPatch;
typedef struct BTGE_BrushPatchPoint_s BTGE_BrushPatchPoint;

// typedef struct BTGE_Brush_s BTGE_Brush;
typedef struct BTGE_BrushGroup_s BTGE_BrushGroup;

typedef struct BTGE_BrushTempPortal_s BTGE_BrushTempPortal;
typedef struct BTGE_BrushTempFace_s BTGE_BrushTempFace;
typedef struct BTGE_BrushTempFace_s BTGE_GroundPanel;

// typedef struct BTGE_BrushWorld_s BTGE_BrushWorld;

// typedef struct BTGE_Light_s BTGE_Light;
typedef struct BTGE_TexImage_s BTGE_TexImage;

struct BTGE_BrushNode_s {
BTGE_BrushNode *lnode;		//left sub-node
BTGE_BrushNode *rnode;		//right sub-node
BTGE_BrushNode *pnode;		//parent node
BTGE_Brush *mid;			//brushes (crossing plane)
float norm[4];				//plane normal

float mins[4];				//AABB mins of contents
float maxs[4];				//AABB maxs of contents
int id;						//linearly assigned node id
byte *vis;					//vis list
BTGE_BrushTempPortal *prt;
BTGE_BrushTempFace *face;

//
BTGE_Entity *ents;			//linked entities
};

struct BTGE_BrushModel_s {
BTGE_BrushModel *next;
BTGE_Brush *brush;
BTGE_BrushNode *bsp;
int mdlnum;
int flags;					//bmodel flags

BTGE_SEntity *sent;			//owning S-Entity
// BTGE_ModelState *mesh;	//owning model state
btEntity ent;				//owning entity

float mdl_org[4];
float rot[4];
float vel[4];
};

struct BTGE_BrushPatchPoint_s {
float xyz[3];
float uv[2];
float norm[4];
};

struct BTGE_BrushPatch_s {
char *tex;
float sdir[4];
float tdir[4];
int texnum;
int texfl;

float xoffs, yoffs, rot, xscale, yscale;

BTGE_BrushPatchPoint ***pts;
int nx_pts;
int ny_pts;
int m_pts;		//max y points

//built patch vertices
BTGE_BrushPatchPoint *vec;		//xyz
int vec_xs;			//x size
int vec_ys;			//y size
};

struct BTGE_Brush_s {
BTGE_Brush *next, *prev;	//next/prev, linear linking
BTGE_BrushGroup *grp;		//owning brush group
BTGE_SEntity *se_owner;		//owning S-Entity
BTGE_BrushNode *node;		//owner node
BTGE_BrushModel *model;		//owner bmodel

BTGE_Brush *enext;		//next in entity
BTGE_Brush *nnext;		//next in node
BTGE_Brush *mnext;		//next in bmodel
BTGE_Brush *chain;		//next in temp-chain

char *name;	//brush name
int flags;	//brush flags

//geometry
float *norm;	//face normals		[x y z d]
float *sdir;	//face s-vectors	[x y z d]
float *tdir;	//face t-vectors	[x y z d]
char **tex;		//textures
int n_face;		//number of faces
int m_face;		//max number of faces

//vecs
float *xyz;	//xyz coords (all poly vecs)
float *st;	//st coords (all poly vecs)
int n_xyz;	//number of xyz vertices
int n_st;	//number of st vertices

//poly faces
byte *pf_plane;	//poly face/plane num
short *pf_ivec;	//poly vertex index
short *pf_nvec;	//poly vertex count
int *pf_flag;	//poly vertex index
int *pf_tex;	//poly tex num
int n_poly;	//number of polygons

short *vec;	//poly vertices [xyz st - -]
int n_vec;	//number of vertices

short *edge;	//poly edges [v0 v1 p0 p1]
int n_edge;

//cached RGB
//byte *rgb;		//cached combined light values

//temp
float org[3], rad;	//sphere org and radius
float *norg;		//face origins [x y z]
float mins[3];		//AABB mins
float maxs[3];		//AABB maxs

//physics
int solid;

BTGE_BrushPatch *patch;	//bezier patch
};

struct BTGE_BrushGroup_s {
BTGE_BrushGroup *next, *prev;
BTGE_BrushGroup *first;

BTGE_Brush *brush;
char *name;	//group name
int flags;	//group flags
};

#if 1
struct BTGE_BrushTempPortal_s {
BTGE_BrushTempPortal *next;
BTGE_BrushTempPortal *chain;
float pts[16*3];
int n_pts;

float norm[4];
BTGE_BrushNode *lnode;		//left target node
BTGE_BrushNode *rnode;		//right target node
BTGE_BrushTempPortal *lnext;	//next in left node
BTGE_BrushTempPortal *rnext;	//next in right node
};
#endif

#if 1
struct BTGE_BrushTempFace_s {
BTGE_BrushTempFace *next;	//next face in list
BTGE_BrushTempFace *chain;	//next face in chain
BTGE_BrushTempFace *nnext;	//next face in node
BTGE_Brush *brush;		//originating brush

float norm[4];

float *xyz;
float *st;
float *evec;			//face edge vertices
int n_vec;

BTGE_BrushTempFace **elink;	//adjacent faces
BTGE_BrushTempFace **link;	//reachable faces
int n_link;

int tex;
int flags;
};
#endif

struct BTGE_BrushWorld_s {
BTGE_Brush *brush;
BTGE_Light *light;
BTGE_BrushNode *bsp;
// BTGE_ModelState *mesh;
BTGE_BrushTempPortal *portal;
BTGE_BrushModel *bmdl;

//byte autovis[4096];
//byte autovis_shadow[4096];
//byte mdlvis[4096];

// float autovis_mins[3];
// float autovis_maxs[3];

int lighting;
int shadows;
int flashlight;

byte shadowcam;		//is camera in shadow?
byte fluidcam;		//is camera submerged?

//mapper
BTGE_SEntity *entity;
BTGE_SEntity *entity_psel;

//simulation
// dyt object_list;	//list of world objects
btEntity *ents;		//entities in world
int n_ents;			//num entities in world
int m_ents;			//max entities in world
int phys_id;		//BSDE world id

int phys_cnt;		//count physics objects
// BTGE_BrushTempFace *groundfaces;

btEntityNode entsNodeRoot;
btEntityNode entsNodeFirst;

//misc
char *map_name;
int map_stat_brushes;
int map_stat_ents;
int map_stat_lights;

//terrain
int terr_xorg;
int terr_yorg;
int terr_xs;
int terr_ys;

byte *terr_hmap;

int terr_tex;
int terr_tex_xs;
int terr_tex_ys;

//lights
byte *light_mask;
int light_rov;
};


struct BTGE_Light_s {
BTGE_Light *next;	//next (in list)
BTGE_Light *tnext;	//next (temp/query list)
BTGE_Light *chain;	//temp chain
BTGE_Light *nnext;	//next (light node)
float org[3];
float dir[3];
float clr[3];
float val[2];
float angle;
int flags;	//light flags
int num;	//conceptual light number
int leaf;	//leafnum for light
int style;	//lightstyle

float tval;	//temp value (used in sorting)
float curval;	//current brightness value
};


struct BTGE_TexImage_s {
BTGE_TexImage *next;	//next teximage
char *name;				//image name
int num;				//raw texnum
int width, height;		//image width/height
int flags;				//flags
float gloss;			//gloss value
float alpha;			//alpha value
float xscale, yscale;	//texture scale
};
