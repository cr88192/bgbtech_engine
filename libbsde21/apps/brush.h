#define LBXGL_BRFL_SEL		1	//selected
#define LBXGL_BRFL_OPSEL	2	//operator selected
#define LBXGL_BRFL_DEL		4	//deleted
#define LBXGL_BRFL_HIDDEN	8	//hidden
#define LBXGL_BRFL_ALPHA	16	//brush is translucent

#define LBXGL_BRFL_FLUID	4096	//brush is fluid
#define LBXGL_BRFL_FLUID0	8192	//fluid-type bit 0
#define LBXGL_BRFL_FLUID1	16384	//fluid-type bit 1
#define LBXGL_BRFL_FLUID2	32768	//fluid-type bit 2
#define LBXGL_BRFL_FLUID3	65536	//fluid-type bit 3
#define LBXGL_BRFL_EFR0		131072	//effect reserved 0
#define LBXGL_BRFL_EFR1		262144	//effect reserved 1
#define LBXGL_BRFL_EFFECT	524288	//special effect
#define LBXGL_BRFL_NOSHADOW	1048576	//don't render


typedef struct LBXGL_SEntity_s LBXGL_SEntity;

typedef struct LBXGL_BrushNode_s LBXGL_BrushNode;
typedef struct LBXGL_BrushModel_s LBXGL_BrushModel;

typedef struct LBXGL_Brush_s LBXGL_Brush;
typedef struct LBXGL_BrushGroup_s LBXGL_BrushGroup;
typedef struct LBXGL_BrushTempPortal_s LBXGL_BrushTempPortal;
typedef struct LBXGL_BrushTempFace_s LBXGL_BrushTempFace;

typedef struct LBXGL_BrushWorld_s LBXGL_BrushWorld;

struct LBXGL_BrushNode_s {
LBXGL_BrushNode *lnode;		//left sub-node
LBXGL_BrushNode *rnode;		//right sub-node
LBXGL_BrushNode *pnode;		//parent node
LBXGL_Brush *mid;		//brushes
float norm[4];			//plane normal

float mins[4];			//AABB mins of contents
float maxs[4];			//AABB maxs of contents
int id;				//linearly assigned node id
byte *vis;			//vis list
LBXGL_BrushTempPortal *prt;
LBXGL_BrushTempFace *face;
};

struct LBXGL_BrushModel_s {
LBXGL_BrushModel *next;
LBXGL_Brush *brush;
LBXGL_BrushNode *bsp;

float org[4];
float rot[4];
};

struct LBXGL_Brush_s {
LBXGL_Brush *next, *prev;	//next/prev, linear linking
LBXGL_BrushGroup *grp;		//owning brush group
LBXGL_SEntity *se_owner;	//owning S-Entity
LBXGL_BrushNode *node;		//owner node
LBXGL_BrushModel *model;	//owner bmodel

LBXGL_Brush *enext;		//next in entity
LBXGL_Brush *nnext;		//next in node
LBXGL_Brush *mnext;		//next in bmodel
LBXGL_Brush *chain;		//next in temp-chain

char *name;	//brush name
int flags;	//brush flags

//geometry
float *norm;	//face normals		[x y z d]
float *sdir;	//face s-vectors	[x y z d]
float *tdir;	//face t-vectors	[x y z d]
char **tex;	//textures
int n_face;	//number of faces

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
int *pf_flag;	//poly vertex index
int *pf_tex;	//poly tex num
int n_poly;	//number of polygons

short *vec;	//poly vertices [xyz st - -]
int n_vec;	//number of vertices

short *edge;	//poly edges [v0 v1 p0 p1]
int n_edge;

//cached RGB
//byte *dlrgb;		//cached dlight values
//byte *srgb;		//cached static lighting values
//byte *drgb[16];		//cached dynamic lighting values
byte *rgb;		//cached combined light values

//temp
float org[3], rad;	//sphere org and radius
float *norg;		//face origins [x y z]
float mins[3];		//AABB mins
float maxs[3];		//AABB maxs
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
LBXGL_BrushTempFace *next;
LBXGL_BrushTempFace *chain;
LBXGL_BrushTempFace *nnext;
LBXGL_Brush *brush;

float norm[4];

float *xyz;
float *st;
float *evec;
int n_vec;

int tex;
int flags;
};


struct LBXGL_BrushWorld_s {
LBXGL_Brush *brush;
// LBXGL_Light *light;
LBXGL_BrushNode *bsp;
// LBXGL_ModelState *mesh;
LBXGL_BrushTempPortal *portal;
LBXGL_BrushModel *bmdl;

byte autovis[4096];
byte autovis_shadow[4096];
byte mdlvis[4096];

float autovis_mins[3];
float autovis_maxs[3];

int lighting;
int shadows;
int flashlight;

byte shadowcam;		//is camera in shadow?
byte fluidcam;		//is camera submerged?

//mapper
//LBXGL_SEntity *entity;

//LBXGL_SEntity *entity_psel;	//parent of new brushes
//LBXGL_SEntity *entity_sel;
LBXGL_Brush *brush_sel;
int brush_sel_face;
char *entity_sel_key;

char *default_tex;

//simulation
//dyt object_list;	//list of world objects
//lbxEntity *ents;	//entities in world
};

