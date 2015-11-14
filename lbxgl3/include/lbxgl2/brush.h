#if 0
#define LBXGL_BRFL_SEL			1		//selected
#define LBXGL_BRFL_OPSEL		2		//operator selected
#define LBXGL_BRFL_DEL			4		//deleted
#define LBXGL_BRFL_HIDDEN		8		//hidden
#define LBXGL_BRFL_ALPHA		16		//brush is translucent
#define LBXGL_BRFL_QUADS		32		//all faces are quads
#define LBXGL_BRFL_PTLIGHT		64		//fast point-lighting
#define LBXGL_BRFL_CHKDRAW		128		//check faces for draw effects
#define LBXGL_BRFL_VSHCHK		256		//view shadow check
#define LBXGL_BRFL_VISCLIP		512		//not visible in frustum
#define LBXGL_BRFL_FULLBRIGHT	1024	//brush is full brightness
#define LBXGL_BRFL_STATICSHADOW	2048	//casts static shadow
#endif

#if 1
#define LBXGL_BRFL_SEL			BTGE_BRFL_SEL		//selected
#define LBXGL_BRFL_OPSEL		BTGE_BRFL_OPSEL		//operator selected
#define LBXGL_BRFL_DEL			BTGE_BRFL_DEL		//deleted
#define LBXGL_BRFL_HIDDEN		BTGE_BRFL_HIDDEN	//hidden
#define LBXGL_BRFL_ALPHA		BTGE_BRFL_ALPHA		//brush is translucent
#define LBXGL_BRFL_QUADS		BTGE_BRFL_QUADS		//all faces are quads
#define LBXGL_BRFL_PTLIGHT		BTGE_BRFL_PTLIGHT	//fast point-lighting
#define LBXGL_BRFL_CHKDRAW		BTGE_BRFL_CHKDRAW	//check for draw effects
#define LBXGL_BRFL_VSHCHK		BTGE_BRFL_VSHCHK	//view shadow check
#define LBXGL_BRFL_VISCLIP		BTGE_BRFL_VISCLIP	//not visible in frustum
#define LBXGL_BRFL_FULLBRIGHT	BTGE_BRFL_FULLBRIGHT	//brush is fullbright
#define LBXGL_BRFL_STATICSHADOW	BTGE_BRFL_STATICSHADOW	//casts static shadow

#define LBXGL_BRFL_NODRAW		BTGE_BRFL_NODRAW	//don't render
#define LBXGL_BRFL_NONSOLID		BTGE_BRFL_NONSOLID	//should be nonsolid
#define LBXGL_BRFL_ORIGIN		BTGE_BRFL_ORIGIN	//is an origin brush
#define LBXGL_BRFL_SKY			BTGE_BRFL_SKY		//is a sky brush
#define LBXGL_BRFL_TRANSIENT	BTGE_BRFL_TRANSIENT	//is not saved
#endif

#if 0
#define LBXGL_BRFL_FLUID		4096	//brush is fluid
#define LBXGL_BRFL_FLUID0		8192	//fluid-type bit 0
#define LBXGL_BRFL_FLUID1		16384	//fluid-type bit 1
#define LBXGL_BRFL_FLUID2		32768	//fluid-type bit 2
#define LBXGL_BRFL_FLUID3		65536	//fluid-type bit 3
#define LBXGL_BRFL_EFR0			131072	//effect reserved 0
#define LBXGL_BRFL_EFR1			262144	//effect reserved 1
#define LBXGL_BRFL_EFFECT		524288	//special effect
#define LBXGL_BRFL_NOSHADOW		1048576	//don't render shadow
#endif

#if 1
#define LBXGL_BRFL_FLUID0		LBXGL_TXFL_FLUID0	//fluid-type bit 0
#define LBXGL_BRFL_FLUID1		LBXGL_TXFL_FLUID1	//fluid-type bit 1
#define LBXGL_BRFL_FLUID2		LBXGL_TXFL_FLUID2	//fluid-type bit 2
#define LBXGL_BRFL_FLUID3		LBXGL_TXFL_FLUID3	//fluid-type bit 3
#define LBXGL_BRFL_EFR0			LBXGL_TXFL_EFR0		//effect reserved 0
#define LBXGL_BRFL_EFR1			LBXGL_TXFL_EFR1		//effect reserved 1
#define LBXGL_BRFL_FLUID		LBXGL_TXFL_FLUID	//brush is fluid
#define LBXGL_BRFL_EFFECT		LBXGL_TXFL_EFFECT	//special effect
#define LBXGL_BRFL_NOSHADOW		LBXGL_TXFL_NOSHADOW	//don't render shadow
#endif

#define LBXGL_EDMODE_GROUP		0
#define LBXGL_EDMODE_OBJECT		1
#define LBXGL_EDMODE_FACE		2
#define LBXGL_EDMODE_VERTEX		3
#define LBXGL_EDMODE_TEXTURE	4
#define LBXGL_EDMODE_BRUSH		5

#define LBXGL_EDCMD_SEL			0
#define LBXGL_EDCMD_TRANS		1
#define LBXGL_EDCMD_ROT			2
#define LBXGL_EDCMD_SCALE		3
#define LBXGL_EDCMD_SCALE2		4

#define LBXGL_EDCMD_DELETE		16
#define LBXGL_EDCMD_EXTRUDE		17
#define LBXGL_EDCMD_SUBDIVIDE	18
#define LBXGL_EDCMD_MERGE		19
#define LBXGL_EDCMD_CUT			20
#define LBXGL_EDCMD_COPY		21
#define LBXGL_EDCMD_DUPLICATE	22
#define LBXGL_EDCMD_INVERT		23

#define LBXGL_SEFL_SEL		1	//selected
#define LBXGL_SEFL_OPSEL	2	//operator selected
#define LBXGL_SEFL_DEL		4	//deleted
#define LBXGL_SEFL_HIDDEN	8	//hidden

#if 0
typedef struct LBXGL_SEntity_s LBXGL_SEntity;
typedef struct LBXGL_SEntPair_s LBXGL_SEntPair;

// typedef struct LBXGL_BrushNode_s LBXGL_BrushNode;
typedef struct LBXGL_BrushModel_s LBXGL_BrushModel;

typedef struct LBXGL_BrushPatch_s LBXGL_BrushPatch;
typedef struct LBXGL_BrushPatchPoint_s LBXGL_BrushPatchPoint;

// typedef struct LBXGL_Brush_s LBXGL_Brush;
typedef struct LBXGL_BrushGroup_s LBXGL_BrushGroup;
typedef struct LBXGL_BrushTempPortal_s LBXGL_BrushTempPortal;
typedef struct LBXGL_BrushTempFace_s LBXGL_BrushTempFace;
typedef struct LBXGL_BrushTempFace_s LBXGL_BrushDrawFace;

typedef struct LBXGL_BrushTempFace_s LBXGL_GroundPanel;

typedef struct LBXGL_BrushWorld_s LBXGL_BrushWorld;
#endif

#if 1
typedef struct BTGE_SEntity_s LBXGL_SEntity;
typedef struct BTGE_SEntPair_s LBXGL_SEntPair;

// typedef struct BTGE_BrushNode_s LBXGL_BrushNode;
typedef struct BTGE_BrushModel_s LBXGL_BrushModel;

typedef struct BTGE_BrushPatch_s LBXGL_BrushPatch;
typedef struct BTGE_BrushPatchPoint_s LBXGL_BrushPatchPoint;

// typedef struct BTGE_Brush_s LBXGL_Brush;
typedef struct BTGE_BrushGroup_s LBXGL_BrushGroup;
typedef struct BTGE_BrushTempPortal_s LBXGL_BrushTempPortal;
typedef struct BTGE_BrushTempFace_s LBXGL_BrushTempFace;
typedef struct BTGE_BrushTempFace_s LBXGL_BrushDrawFace;

typedef struct BTGE_BrushTempFace_s LBXGL_GroundPanel;

typedef struct BTGE_BrushWorld_s LBXGL_BrushWorld;

typedef struct BTGE_BrushWorld_s lbxWorld;

#endif

#if 0
struct LBXGL_BrushNode_s {
LBXGL_BrushNode *lnode;		//left sub-node
LBXGL_BrushNode *rnode;		//right sub-node
LBXGL_BrushNode *pnode;		//parent node
LBXGL_Brush *mid;		//brushes
float norm[4];			//plane normal

float mins[4];			//AABB mins of contents
float maxs[4];			//AABB maxs of contents
float org[3];			//origin of contents
float rad;				//radius of contents
int id;				//linearly assigned node id
byte *vis;			//vis list
LBXGL_BrushTempPortal *prt;
LBXGL_BrushTempFace *face;
};

struct LBXGL_BrushModel_s {
LBXGL_BrushModel *next;
LBXGL_Brush *brush;
LBXGL_BrushNode *bsp;

LBXGL_SEntity *sent;	//owning S-Entity
LBXGL_ModelState *mesh;	//owning model state

int mdlnum;				//model number

float mdl_org[4];
float rot[4];
float vel[4];

float com_org[4];		//center of mass origin
float com_rad;			//radius from center of mass
};

struct LBXGL_BrushPatchPoint_s {
float xyz[3];
float uv[2];
float norm[4];
float rgba[4];
};

struct LBXGL_BrushPatch_s {
char *tex;
float sdir[4];
float tdir[4];
int texnum;
int texfl;

float xoffs, yoffs, rot, xscale, yscale;

LBXGL_BrushPatchPoint ***pts;
int nx_pts;
int ny_pts;
int m_pts;		//max y points

//built patch vertices
LBXGL_BrushPatchPoint *vec;		//xyz
int vec_xs;			//x size
int vec_ys;			//y size
};

struct LBXGL_Brush_s {
LBXGL_Brush *next, *prev;	//next/prev, linear linking (in map)
LBXGL_BrushGroup *grp;		//owning brush group
LBXGL_SEntity *se_owner;	//owning S-Entity
LBXGL_BrushNode *node;		//owner node
LBXGL_BrushModel *model;	//owner bmodel

LBXGL_Brush *enext;		//next in entity
LBXGL_Brush *nnext;		//next in node
LBXGL_Brush *mnext;		//next in bmodel
LBXGL_Brush *chain;		//next in temp-chain
LBXGL_Brush *knext;		//next in next is special list (antiportal/...)

char *name;	//brush name
int flags;	//brush flags
int texfl;	//brush texture flags

//geometry
float *norm;	//face normals		[x y z d]
float *sdir;	//face s-vectors	[x y z d]
float *tdir;	//face t-vectors	[x y z d]
char **tex;		//textures
int n_face;		//number of faces
int m_face;		//max number of faces

//cache
//int *plnum;	//poly face/plane num
//int *texnum;	//poly tex num
//int *numvec;	//poly vertex count
//float *xyz;	//xyz coords (all poly vecs)
//float *st;	//st coords (all poly vecs)
//int n_vec;	//number of vertices
//int n_poly;	//number of polygons

//vecs
float *xyz;	//xyz coords (all poly vecs)
float *st;	//st coords (all poly vecs)
int n_xyz;	//number of xyz vertices
int n_st;	//number of st vertices

//poly faces
byte *pf_plane;	//poly face/plane num
short *pf_ivec;	//poly vertex index
short *pf_nvec;	//poly vertex count
int *pf_flag;	//poly texture flags
int *pf_tex;	//poly texture num
int n_poly;	//number of polygons

short *vec;	//poly vertices [xyz st - -]
int n_vec;	//number of vertices

short *edge;	//poly edges [v0 v1 p0 p1]
int n_edge;

//cached RGB
//byte *dlrgb;		//cached dlight values
//byte *srgb;		//cached static lighting values
//byte *drgb[16];		//cached dynamic lighting values
float *rgb;		//cached combined light values

//temp
float org[3], rad;	//sphere org and radius
float *norg;		//face origins [x y z]
float mins[3];		//AABB mins
float maxs[3];		//AABB maxs
float ptrgb[4];		//point RGB

//physics
int solid;

LBXGL_BrushPatch *patch;	//bezier patch
LBXGL_MeshPrim *mesh;		//polygonal mesh
LBXGL_SEntity *se_child;	//child entity (generator)
};

struct LBXGL_BrushGroup_s {
LBXGL_BrushGroup *next, *prev;
LBXGL_BrushGroup *first;

LBXGL_Brush *brush;
char *name;	//group name
int flags;	//group flags
};


struct LBXGL_BrushTempPortal_s {
LBXGL_BrushTempPortal *next;
LBXGL_BrushTempPortal *chain;
float pts[16*3];
int n_pts;

float norm[4];
LBXGL_BrushNode *lnode;		//left target node
LBXGL_BrushNode *rnode;		//right target node
LBXGL_BrushTempPortal *lnext;	//next in left node
LBXGL_BrushTempPortal *rnext;	//next in right node
};


struct LBXGL_BrushTempFace_s {
LBXGL_BrushTempFace *next;	//next face in list
LBXGL_BrushTempFace *chain;	//next face in chain
LBXGL_BrushTempFace *nnext;	//next face in node
LBXGL_Brush *brush;		//originating brush

float norm[4];

float *xyz;
float *st;
float *evec;			//face edge vertices
int n_vec;

LBXGL_BrushTempFace **elink;	//adjacent faces


LBXGL_BrushTempFace **link;	//reachable faces
int n_link;

int tex;
int flags;
};


struct LBXGL_BrushWorld_s {
LBXGL_Brush *brush;
LBXGL_Light *light;
LBXGL_BrushNode *bsp;
LBXGL_ModelState *mesh;
LBXGL_BrushTempPortal *portal;
LBXGL_BrushModel *bmdl;

byte autovis[4096];
byte autovis_shadow[4096];
byte mdlvis[4096];

float autovis_mins[3];
float autovis_maxs[3];
float skycam_org[3];

byte lighting;
byte flashlight;
byte worldscale;	//0=1x, 1=12x
byte skycam;		//3D skybox in use
short shadows;

byte shadowcam;		//is camera in shadow?
byte fluidcam;		//is camera submerged?

float basegamma;	//base gamma scale
float curgamma;		//current gamma scale

//mapper
LBXGL_SEntity *entity;

LBXGL_SEntity *entity_psel;	//parent of new brushes
LBXGL_SEntity *entity_sel;
LBXGL_Brush *brush_sel;
int brush_sel_face;
char *entity_sel_key;

char *default_tex;
int grid_snap;

int edit_mode;
int edit_cmd;

//simulation
dyt object_list;	//list of world objects
lbxEntity *ents;	//entities in world
int phys_id;		//BSDE world id

LBXGL_BrushTempFace *groundfaces;

//misc
char *map_name;
int map_stat_brushes;
int map_stat_ents;
int map_stat_lights;

LBXGL_Light *light_sun;
LBXGL_Brush *brush_antiportal;
LBXGL_Brush *brush_last;			//last brush

//terrain (height map)
int terr_xorg;
int terr_yorg;
int terr_xs;
int terr_ys;

byte *terr_hmap;

int terr_tex;
int terr_tex_xs;
int terr_tex_ys;

//voxels
LBXGL_VoxelRegion *vox_region;
};

#endif

#if 1

typedef struct
{
LBXGL_BrushWorld *world;

LBXGL_GroundPanel **blst;	//"best" list
LBXGL_GroundPanel **lst;	//current list
int blpos;			//best list pos
int lpos;			//current list pos

float *sorg;			//source origin
float *dorg;			//dest origin
LBXGL_GroundPanel *spanel;	//source panel
LBXGL_GroundPanel *dpanel;	//dest panel

}LBXGL_BrushPath_FindPathPanelList_State;

#endif
