#if 0
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
// #define BTGE_TXFL_NOSHADOW		1048576	//don't render shadow

#define BTGE_TXFL_NOSHADOW		0x00100000	//don't render shadow
#define BTGE_TXFL_WARPXYZ		0x00200000	//warp xyz coords
#define BTGE_TXFL_SCROLLST		0x00400000	//transform ST coords
#define BTGE_TXFL_LIGHTMAT		0x00800000	//use lighting material

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
#define BTGE_TXFL_MATERIAL_SLUDGE	0x16000		//burning sludge
#define BTGE_TXFL_MATERIAL_SOLID	0x7E000		//generic solid

#define BTGE_TXFL_FLUID_MASK		0xFF000		//mask
#define BTGE_TXFL_FLUID_WATER		0x01000		//slime
#define BTGE_TXFL_FLUID_SLIME		0x03000		//slime
#define BTGE_TXFL_FLUID_LAVA		0x05000		//lava
#define BTGE_TXFL_FLUID_SEWER		0x07000		//dirty water

#define BTGE_TXFL_EFFECT_MASK			0xFF000		//mask
#define BTGE_TXFL_EFFECT_LVOL			0x82000		//light volume
#define BTGE_TXFL_EFFECT_FOG			0x84000		//fog volume
#define BTGE_TXFL_EFFECT_ORIGIN			0x86000		//origin
#define BTGE_TXFL_EFFECT_ANTIPORTAL		0x88000		//antiportal
#define BTGE_TXFL_EFFECT_DECAL			0x8A000		//decal
#define BTGE_TXFL_EFFECT_VOXEL			0x8C000		//texture is a voxel type
#define BTGE_TXFL_EFFECT_DECAL2			0x8E000		//decal (positive)
#define BTGE_TXFL_EFFECT_DECAL_ALPHA	0x90000		//decal (alpha-blend)
#define BTGE_TXFL_EFFECT_PORTAL			0x92000		//portal

#define BTGE_TXFL_ALPHA_LSHIFT			13		//fluid-type bit 0
#define BTGE_TXFL_ALPHA_MASK			0x1E000	//mask

//face flags
#define BTGE_TXFL_NOPLANE	(1<<24)	//don't use face as plane
#endif


#if 1
//texture flags
#define BTGE_TXFL_NONE				0x00000000
#define BTGE_TXFL_ALPHA				0x00000001	//image uses alpha channel
#define BTGE_TXFL_VALPHA			0x00000002	//object uses alpha value
#define BTGE_TXFL_NONSOLID			0x00000004	//objects should be nonsolid
#define BTGE_TXFL_DETAIL			0x00000008	//objects should be detail
#define BTGE_TXFL_NOCSG				0x00000010	//object should not clip others
#define BTGE_TXFL_FULLBRIGHT		0x00000020	//don't lightmap surface
#define BTGE_TXFL_WINDOW			0x00000040	//object is window-like
// #define BTGE_TXFL_WARPST			0x00000080	//warp st coords
#define BTGE_TXFL_GLOSS				0x00000100	//use surface gloss
#define BTGE_TXFL_NODRAW			0x00000200	//don't render
#define BTGE_TXFL_NEWSCREEN			0x00000400	//GLSL: use new screen texture
#define BTGE_TXFL_KEEPSCREEN		0x00000800	//GLSL: keep visible
#define BTGE_TXFL_FLUID0			0x00001000	//fluid-type bit 0
#define BTGE_TXFL_FLUID1			0x00002000	//fluid-type bit 1
#define BTGE_TXFL_FLUID2			0x00004000	//fluid-type bit 2
#define BTGE_TXFL_FLUID3			0x00008000	//fluid-type bit 3
#define BTGE_TXFL_EFR0				0x00010000	//effect reserved 0
#define BTGE_TXFL_EFR1				0x00020000	//effect reserved 1
#define BTGE_TXFL_FLUID				0x00040000	//object is fluid-like
#define BTGE_TXFL_EFFECT			0x00080000	//special effect
#define BTGE_TXFL_NOSHADOW			0x00100000	//don't render shadow
// #define BTGE_TXFL_WARPXYZ		0x00200000	//warp xyz coords
// #define BTGE_TXFL_SCROLLST		0x00400000	//transform ST coords

#define BTGE_TXFL_NOPLANE			0x00200000	//don't use face as plane
#define BTGE_TXFL_COLOR_UVAY		0x00400000	//warp st coords

#define BTGE_TXFL_LIGHTMAT			0x00800000	//use lighting material
//#define BTGE_TXFL_NOPLANE			0x01000000	//don't use face as plane

#define BTGE_TXFL_WARP_MASK			0x0F000000	//warp mask
#define BTGE_TXFL_WARP_NONE			0x00000000	//no warp
#define BTGE_TXFL_WARP_ST			0x01000000	//warp st coords
#define BTGE_TXFL_WARP_XYZ			0x02000000	//warp xyz coords
#define BTGE_TXFL_WARP_SCROLLST		0x03000000	//transform ST coords
#define BTGE_TXFL_WARP_NBULGE		0x04000000	//bulge along normal
#define BTGE_TXFL_WARP_JIGGLE		0x05000000	//inertial jiggle

#define BTGE_TXFL_MATERIAL_MASK			0x000FF000		//mask
#define BTGE_TXFL_MATERIAL_NONE			0x00000000		//generic solid
#define BTGE_TXFL_MATERIAL_SKY			0x00001000		//sky
#define BTGE_TXFL_MATERIAL_GLASS		0x00002000		//glass
#define BTGE_TXFL_MATERIAL_WOOD			0x00003000		//wood
#define BTGE_TXFL_MATERIAL_METAL		0x00004000		//metal
#define BTGE_TXFL_MATERIAL_FLESH		0x00005000		//flesh
#define BTGE_TXFL_MATERIAL_CONCRETE		0x00006000		//concrete
#define BTGE_TXFL_MATERIAL_FOAM			0x00007000		//solid foam-like
#define BTGE_TXFL_MATERIAL_COMPUTER		0x00008000		//computer/machine
#define BTGE_TXFL_MATERIAL_UBGLASS		0x00009000		//unbreakable glass
#define BTGE_TXFL_MATERIAL_STONE		0x0000A000		//stone/rocks
#define BTGE_TXFL_MATERIAL_SLUDGE		0x0000B000		//burning sludge
// #define BTGE_TXFL_MATERIAL_LADDER		0x0000C000		//ladder

#define BTGE_TXFL_MATERIAL_SOLID		0x0003F000		//generic solid

#define BTGE_TXFL_FLUID_MASK			0x000FF000		//mask
#define BTGE_TXFL_FLUID_NONE			0x00040000		//generic fluid
#define BTGE_TXFL_FLUID_WATER			0x00041000		//water
#define BTGE_TXFL_FLUID_SLIME			0x00042000		//slime
#define BTGE_TXFL_FLUID_LAVA			0x00043000		//lava
#define BTGE_TXFL_FLUID_SEWER			0x00044000		//dirty water
#define BTGE_TXFL_FLUID_PORTAL			0x00045000		//portal block
#define BTGE_TXFL_FLUID_ELASTIC			0x00046000		//elastic fluid

#define BTGE_TXFL_EFFECT_MASK			0x000FF000		//mask
#define BTGE_TXFL_EFFECT_LVOL			0x00081000		//light volume
#define BTGE_TXFL_EFFECT_FOG			0x00082000		//fog volume
#define BTGE_TXFL_EFFECT_ORIGIN			0x00083000		//origin
#define BTGE_TXFL_EFFECT_ANTIPORTAL		0x00084000		//antiportal
#define BTGE_TXFL_EFFECT_DECAL			0x00085000		//decal
#define BTGE_TXFL_EFFECT_VOXEL			0x00086000		//texture is a voxel type
#define BTGE_TXFL_EFFECT_DECAL2			0x00087000		//decal (positive)
#define BTGE_TXFL_EFFECT_DECAL_ALPHA	0x00088000		//decal (alpha-blend)
#define BTGE_TXFL_EFFECT_PORTAL			0x00089000		//portal
// #define BTGE_TXFL_EFFECT_LADDER			0x0008A000		//ladder

#define BTGE_TXFL_ALPHA_LSHIFT			12			//fluid-type bit 0
#define BTGE_TXFL_ALPHA_MASK			0x0000F000	//alpha mask

#define BTGE_TXFL_SCALE_LSHIFT			24			//fluid-type bit 0
#define BTGE_TXFL_SCALE_MASK			0xFF000000	//alpha mask
#endif

#define BTGE_TXFL2_UNBREAKABLE				0x00000001	//unbreakable 
#define BTGE_TXFL2_VOXEL					0x00000002	//voxel 
#define BTGE_TXFL2_NODEPTHWRITE				0x00000004	//no depth writes
#define BTGE_TXFL2_PARTIALBLOCK				0x00000008	//partial block 
#define BTGE_TXFL2_NOWATEROVERLAY			0x00000010	//no water overlay 
#define BTGE_TXFL2_ANIMMATCHSOUND			0x00000020	//anim match sound
#define BTGE_TXFL2_TOUCHPAIN				0x00000040	//touching causes pain
#define BTGE_TXFL2_LADDER					0x00000080	//is a ladder
#define BTGE_TXFL2_AUXVARIANT				0x00000100	//aux selects variants
#define BTGE_TXFL2_EMITSSOUND				0x00000200	//is a sound source
#define BTGE_TXFL2_ELASTIC					0x00000400	//things bounce off
#define BTGE_TXFL2_NOFLEXFORM				0x00000800	//don't use flexform

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

#define BTGE_BRFL_QUADS		32		//all faces are quads
#define BTGE_BRFL_PTLIGHT		64		//fast point-lighting
#define BTGE_BRFL_CHKDRAW		128		//check faces for draw effects
#define BTGE_BRFL_VSHCHK		256		//view shadow check
#define BTGE_BRFL_VISCLIP		512		//not visible in frustum
#define BTGE_BRFL_FULLBRIGHT	1024	//brush is full brightness
#define BTGE_BRFL_STATICSHADOW	2048	//casts static shadow

#define BTGE_BRFL_NODRAW	0x00100000	//don't render
#define BTGE_BRFL_NONSOLID	0x00200000	//objects should be nonsolid
#define BTGE_BRFL_ORIGIN	0x00400000	//brush is an origin brush
#define BTGE_BRFL_SKY		0x00800000	//brush is a sky brush
#define BTGE_BRFL_TRANSIENT	0x01000000	//brush is transient (non-saved)
#define BTGE_BRFL_DIRTY		0x02000000	//brush is marked dirty
#define BTGE_BRFL_NEW		0x04000000	//brush was newly created

#if 0
#define BTGE_BRFL_FLUID		4096	//brush is fluid
#define BTGE_BRFL_FLUID0	8192	//fluid-type bit 0
#define BTGE_BRFL_FLUID1	16384	//fluid-type bit 1
#define BTGE_BRFL_FLUID2	32768	//fluid-type bit 2
#define BTGE_BRFL_FLUID3	65536	//fluid-type bit 3
#define BTGE_BRFL_EFR0		131072	//effect reserved 0
#define BTGE_BRFL_EFR1		262144	//effect reserved 1
#define BTGE_BRFL_EFFECT	524288	//special effect
#define BTGE_BRFL_NOSHADOW	1048576	//don't render shadows
#endif

#if 1
#define BTGE_BRFL_FLUID0	BTGE_TXFL_FLUID0	//fluid-type bit 0
#define BTGE_BRFL_FLUID1	BTGE_TXFL_FLUID1	//fluid-type bit 1
#define BTGE_BRFL_FLUID2	BTGE_TXFL_FLUID2	//fluid-type bit 2
#define BTGE_BRFL_FLUID3	BTGE_TXFL_FLUID3	//fluid-type bit 3
#define BTGE_BRFL_EFR0		BTGE_TXFL_EFR0		//effect reserved 0
#define BTGE_BRFL_EFR1		BTGE_TXFL_EFR1		//effect reserved 1
#define BTGE_BRFL_FLUID		BTGE_TXFL_FLUID		//brush is fluid
#define BTGE_BRFL_EFFECT	BTGE_TXFL_EFFECT	//special effect
#define BTGE_BRFL_NOSHADOW	BTGE_TXFL_NOSHADOW	//don't render shadows
#endif

#define BTGE_MDL_SAVEREL_NUM		(2*4)
#define BTGE_MDL_SAVEREL_DT			0.25

#define BTGE_WFL_LIGHTBUDGET			1	//use economical lighting
#define BTGE_WFL_EARLYOUT				2	//use early-out optimization
#define BTGE_WFL_OVERBUDGET				4	//rendering is taking too long
#define BTGE_WFL_ALPHABUDGET			8	//use economical shaders
#define BTGE_WFL_PARALLAX				16	//use parallax mapping

#define BTGE_WFL_VOXFLUID				256	//voxel fluid visible
#define BTGE_WFL_UPDATING				512	//world is running update
#define BTGE_WFL_UPDATED				1024	//world has updated

#define BTGE_WFL_NOEXPLOSIVEDAMAGE		2048	//disable explosive damage
#define BTGE_WFL_NOSAVEVOXELWORLD		4096	//disable saving voxel world

#define BTGE_LFL_CHANGED		1		//light has changed since last frame
#define BTGE_LFL_LINEAR			2		//light uses linear falloff
#define BTGE_LFL_SPOTLIGHT		4		//light is a spotlight
#define BTGE_LFL_NOSHADOW		8		//light does not cast shadows
#define BTGE_LFL_VISTEST		16		//light does not cast shadows
#define BTGE_LFL_LIGHTVOL		32		//light can make light volumes
#define BTGE_LFL_DISABLE		64		//light is currently disabled/off
#define BTGE_LFL_LASER			128		//light is a laser
#define BTGE_LFL_SUN			256		//light is the sun
#define BTGE_LFL_STATIC			512		//light is static / non-moving
#define BTGE_LFL_ENVGLOSS		1024	//used as gloss envmap
#define BTGE_LFL_ENVRENDER		2048	//render envmap from this spot

#define BTGE_LFL_DETAIL			4096	//light is a detail light
#define BTGE_LFL_BOXCULL		8192	//light uses bbox for culling
#define BTGE_LFL_SCENERY		16384	//light applies to scenery
#define BTGE_LFL_CAMREL			32768	//light is camera relative

#define BTGE_LFL_EFFECT_MASK	0x00FF0000
#define BTGE_LFL_EFFECT_NONE	0x00000000

#define BTGE_LFL_SHADOWMAP		0x01000000	//use shadow mapping

#define BT_LNUM_GLOW				0x00010000	//mask for glowing entities
#define BT_LNUM_SERVER				0x00020000	//mask for server-managed lights


//WARNING: These constants are magic (Align with those in OpenGL)
#define BTGE_POINTS						0
#define BTGE_LINES						1
#define BTGE_LINE_LOOP					2
#define BTGE_LINE_STRIP					3
#define BTGE_TRIANGLES					4
#define BTGE_TRIANGLE_STRIP				5
#define BTGE_TRIANGLE_FAN				6
#define BTGE_QUADS						7
#define BTGE_QUAD_STRIP					8
#define BTGE_POLYGON					9


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
typedef struct BTGE_BrushTempFace_s BTGE_BrushDrawFace;

// typedef struct BTGE_BrushWorld_s BTGE_BrushWorld;

// typedef struct BTGE_Light_s BTGE_Light;
typedef struct BTGE_TexImage_s BTGE_TexImage;
// typedef struct BTGE_ModelState_s BTGE_ModelState;

typedef struct LBXGL_RenderMethods_s BTGE_RenderMethods;	//hack...

struct BTGE_BrushNode_s {
BTGE_BrushNode *lnode;		//left sub-node
BTGE_BrushNode *rnode;		//right sub-node
BTGE_BrushNode *pnode;		//parent node
BTGE_Brush *mid;			//brushes (crossing plane)
float norm[4];				//plane normal

float mins[4];				//AABB mins of contents
float maxs[4];				//AABB maxs of contents
float org[3];				//origin of contents
float rad;					//radius of contents
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
BTGE_ModelState *mesh;		//owning model state
btEntity ent;				//owning entity

float mdl_org[4];
float rot[4];
float vel[4];

float com_org[4];		//center of mass origin
float com_rad;			//radius from center of mass
};

struct BTGE_BrushPatchPoint_s {
float xyz[3];
float uv[2];
float norm[4];
float rgba[4];
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
BTGE_Brush *knext;		//next in special list

char *name;		//brush name
char *gname;	//brush group name
int flags;		//brush flags
int texfl;		//texture flags

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
float *rgb;		//cached combined light values

//temp
float org[3], rad;	//sphere org and radius
float *norg;		//face origins [x y z]
float mins[3];		//AABB mins
float maxs[3];		//AABB maxs
float ptrgb[4];		//point RGB

//physics
int solid;
int idnum;			//brush ID number

BTGE_BrushPatch *patch;		//bezier patch
BTGE_MeshPrim *mesh;		//polygon mesh
BTGE_SEntity *se_child;		//child entity (generator)

// BTGE_BrushTempFace *face;	//brush faces
BTGE_BrushDrawFace *face;	//brush faces
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
BTGE_BrushTempFace *bnext;	//next face in brush
BTGE_Brush *brush;		//originating brush

float norm[4];
float sdir[4];
float tdir[4];

float *xyz;
float *st;
float *evec;			//face edge vertices

short *vec;
int n_vec;

BTGE_BrushTempFace **elink;	//adjacent faces
BTGE_BrushTempFace **link;	//reachable faces
int n_link;

int tex;				//texture
int flags;				//flags
int prim;				//primitive type
};
#endif

struct BTGE_BrushWorld_s {
BTGE_Brush *brush;
BTGE_Light *light;
BTGE_BrushNode *bsp;
BTGE_ModelState *mesh;
BTGE_BrushTempPortal *portal;
BTGE_BrushModel *bmdl;

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

float basegamma;
float curgamma;
int flags;

//mapper
BTGE_SEntity *entity;

BTGE_SEntity *entity_psel;
BTGE_SEntity *entity_sel;
BTGE_Brush *brush_sel;			//last brush
int brush_sel_face;
char *entity_sel_key;

char *default_tex;
int grid_snap;

int edit_mode;
int edit_cmd;

//simulation
dyt object_list;	//list of world objects
btEntity *ents;		//entities in world
int n_ents;			//num entities in world
int m_ents;			//max entities in world
int phys_id;		//BSDE world id

int phys_cnt;		//count physics objects
BTGE_BrushTempFace *groundfaces;

btEntityNode entsNodeRoot;
btEntityNode entsNodeFirst;
btguiContext guis;	//GUI Context

//misc
char *map_name;
int map_stat_brushes;
int map_stat_ents;
int map_stat_lights;

BTGE_Light *light_sun;
BTGE_Brush *brush_antiportal;
BTGE_Brush *brush_last;			//last brush

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

//voxels
BTGE_VoxelRegion *vox_region;
BTGE_VoxelTerrGen *vox_terr;

BTGE_VoxelChunk *vox_vischk;	//visible chunk list
BTGE_VoxelChunk *vox_avischk;	//alpha visible chunk list

//rendering stuff
float cam_light;		//light-level at camera
float cam_expose;		//camera exposure
s64 cam_reforg[3];		//refernce point origin
float bsp_origin[3];	//world BSP origin

float time_setactive;		//timer: set active chunks
float time_autovis;			//timer: autovis
float time_rebuildvis;		//timer: rebuild visible
float time_turtle;			//timer: bad framerate
float time_daynight;		//day/night clock

BTGE_DrawableObject2D *drawobj2d;

void *barf_voxel;			//render-array for voxel terrain
void *barf_model;			//render-array for models

//methods
byte *(*EncodeChunkContents)(BTGE_BrushWorld *world,
	BTGE_VoxelChunk *chk, byte *obuf, int osz);
byte *(*DecodeChunkContents)(BTGE_BrushWorld *world,
	BTGE_VoxelChunk *chk, byte *ibuf, int isz);
byte *(*EncodeRegionContents)(BTGE_BrushWorld *world,
	BTGE_VoxelRegion *rgn, byte *obuf, int osz);
byte *(*DecodeRegionContents)(BTGE_BrushWorld *world,
	BTGE_VoxelRegion *rgn, byte *ibuf, int isz);

int (*HandleLoadRegion)(BTGE_BrushWorld *world, BTGE_VoxelRegion *rgn);
int (*HandleSaveRegion)(BTGE_BrushWorld *world, BTGE_VoxelRegion *rgn);
};


struct BTGE_LightNode_s {
BTGE_LightNode *lnode;
BTGE_LightNode *rnode;
BTGE_Light *mid;
float norm[4];
};

struct BTGE_Light_s {
BTGE_Light *next;	//next (in list)
BTGE_Light *tnext;	//next (temp/query list)
BTGE_Light *chain;	//temp chain
BTGE_Light *nnext;	//next (light node)
BTGE_Light *oldval;	//old values (delta)
float org[3];
float dir[3];
float clr[3];
float val[2];				//base value, highlight
float coeff[3];				//ambient, specular, diffuse
float angle;
int flags;					//light flags
int num;					//conceptual light number
int leaf;					//leafnum for light
int style;					//lightstyle
int envtex;					//environment map texture (cubemap)
int envdepthtex;			//environment map depth texture (cubemap)

char *texname;				//texture name
int tex_falloff;			//falloff texture
int tex_project;			//falloff texture

float mins[3];				//light bbox mins
float maxs[3];				//light bbox maxs
float amins[3];				//absolute mins
float amaxs[3];				//absolute maxs
float boxval[3];			//light box value
float rel_org[3];			//relative base origin (camrel)
float rel_dir[3];			//relative base direction (camrel)

float tval;					//temp value (used in sorting)
float curval;				//current brightness value
float cur_mcsc;

BTGE_Brush **visbrush;		//list of potentially visible brushes (static)

BTGE_Brush **litbrush;		//brushes being lit by light (temp)
BTGE_Brush **shadbrush;		//brushes casting shadow from light (temp)
BTGE_ModelState **litmdl;	//models being lit by light (temp)
BTGE_ModelState **shadmdl;	//models casting shadow from light (temp)
int sz_litbrush;			//size of lit brush array
int sz_shadbrush;			//size of shadow-brush array
int sz_litmdl;				//size of lit model array
int sz_shadmdl;				//size of shadow-model array

float env_time;				//environment timer
float env_org[3];			//relative base origin (camrel)
};


struct BTGE_TexImage_s {
BTGE_TexImage *next;	//next teximage
char *name;				//image name
int num;				//raw texnum
int width, height;		//image width/height
int flags;				//flags
int flags2;				//flags2
int flags3;				//flags3
int flags4;				//flags4
float gloss;			//gloss value
float alpha;			//alpha value
float xscale, yscale;	//texture scale

int altnum;				//alternate number (renderer texture number)
};

struct BTGE_ModelState_s {
BTGE_ModelState *next;		//free for client use
BTGE_ModelState *chain;	//temporary chains, ...
BTGE_RenderMethods *mvt;

float org[3];
float rot[9];
void *mdl;
void *state;
char *mdltype;

char *modelname;	//modelname (props)

int entnum;

int frame;			//frame-based models
int skin;			//models with multiple skins
int flags;			//same as texture flags
int effects;		//model effects flags
int effects2;		//model effects2 flags

float mins[3];		//AABB mins
float maxs[3];		//AABB maxs
float rel_org[3];	//relative origin

BTGE_BrushDrawFace *tmp_drawface;

//saved relative positions
float lr_org[BTGE_MDL_SAVEREL_NUM*3];
float lr_rot[BTGE_MDL_SAVEREL_NUM*4];
float lr_time[BTGE_MDL_SAVEREL_NUM];
};
