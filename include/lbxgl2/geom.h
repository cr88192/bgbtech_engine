#ifndef LBXGL_GEOM_H
#define LBXGL_GEOM_H

#if 0
//texture flags
#define LBXGL_TXFL_NONE			0
#define LBXGL_TXFL_ALPHA		1		//image uses alpha channel
#define LBXGL_TXFL_VALPHA		2		//object uses alpha value
#define LBXGL_TXFL_NONSOLID		4		//objects should be nonsolid
#define LBXGL_TXFL_DETAIL		8		//objects should be detail
#define LBXGL_TXFL_NOCSG		16		//object should not clip others
#define LBXGL_TXFL_FULLBRIGHT	32		//don't lightmap surface
#define LBXGL_TXFL_WINDOW		64		//object is window-like
#define LBXGL_TXFL_WARPST		128		//warp st coords
#define LBXGL_TXFL_GLOSS		256		//use surface gloss
#define LBXGL_TXFL_NODRAW		512		//don't render
#define LBXGL_TXFL_NEWSCREEN	1024	//GLSL: use new screen texture
#define LBXGL_TXFL_KEEPSCREEN	2048	//GLSL: keep visible
#define LBXGL_TXFL_FLUID		4096	//object is fluid-like
#define LBXGL_TXFL_FLUID0		8192	//fluid-type bit 0
#define LBXGL_TXFL_FLUID1		16384	//fluid-type bit 1
#define LBXGL_TXFL_FLUID2		32768	//fluid-type bit 2
#define LBXGL_TXFL_FLUID3		65536	//fluid-type bit 3
#define LBXGL_TXFL_EFR0			131072	//effect reserved 0
#define LBXGL_TXFL_EFR1			262144	//effect reserved 1
#define LBXGL_TXFL_EFFECT		524288	//special effect
// #define LBXGL_TXFL_NOSHADOW		1048576	//don't render shadow
// #define LBXGL_TXFL_WARPXYZ		2097152	//warp xyz coords
// #define LBXGL_TXFL_SCROLLST		(1<<22)	//transform ST coords

#define LBXGL_TXFL_NOSHADOW		0x00100000	//don't render shadow
#define LBXGL_TXFL_WARPXYZ		0x00200000	//warp xyz coords
#define LBXGL_TXFL_SCROLLST		0x00400000	//transform ST coords
#define LBXGL_TXFL_LIGHTMAT		0x00800000	//use lighting material

#define LBXGL_TXFL_MATERIAL_MASK		0xFF000		//mask
#define LBXGL_TXFL_MATERIAL_NONE		0x00000		//generic solid
#define LBXGL_TXFL_MATERIAL_SKY			0x02000		//sky
#define LBXGL_TXFL_MATERIAL_GLASS		0x04000		//glass
#define LBXGL_TXFL_MATERIAL_WOOD		0x06000		//wood
#define LBXGL_TXFL_MATERIAL_METAL		0x08000		//metal
#define LBXGL_TXFL_MATERIAL_FLESH		0x0A000		//flesh
#define LBXGL_TXFL_MATERIAL_CONCRETE	0x0C000		//concrete
#define LBXGL_TXFL_MATERIAL_FOAM		0x0E000		//solid foam-like
#define LBXGL_TXFL_MATERIAL_COMPUTER	0x10000		//computer/machine
#define LBXGL_TXFL_MATERIAL_UBGLASS		0x12000		//unbreakable glass
#define LBXGL_TXFL_MATERIAL_STONE		0x14000		//stone/rocks
#define LBXGL_TXFL_MATERIAL_SLUDGE		0x16000		//burning sludge
#define LBXGL_TXFL_MATERIAL_SOLID		0x7E000		//generic solid

#define LBXGL_TXFL_FLUID_MASK			0x1F000	//mask
#define LBXGL_TXFL_FLUID_WATER			0x01000	//water
#define LBXGL_TXFL_FLUID_SLIME			0x03000	//slime
#define LBXGL_TXFL_FLUID_LAVA			0x05000	//lava
#define LBXGL_TXFL_FLUID_SEWER			0x07000	//dirty water

#define LBXGL_TXFL_ALPHA_LSHIFT			13		//fluid-type bit 0
#define LBXGL_TXFL_ALPHA_MASK			0x1E000	//mask

#define LBXGL_TXFL_EFFECT_MASK			0xFF000	//mask
#define LBXGL_TXFL_EFFECT_LVOL			0x82000	//light volume
#define LBXGL_TXFL_EFFECT_FOG			0x84000	//fog volume
#define LBXGL_TXFL_EFFECT_ORIGIN		0x86000	//origin
#define LBXGL_TXFL_EFFECT_ANTIPORTAL	0x88000	//antiportal
#define LBXGL_TXFL_EFFECT_DECAL			0x8A000	//decal (negative)
#define LBXGL_TXFL_EFFECT_VOXEL			0x8C000	//texture is a voxel type
#define LBXGL_TXFL_EFFECT_DECAL2		0x8E000	//decal (positive)
#define LBXGL_TXFL_EFFECT_DECAL_ALPHA	0x90000	//decal (alpha-blend)
#define LBXGL_TXFL_EFFECT_PORTAL		0x92000	//portal

//face flags
#define LBXGL_TXFL_NOPLANE				(1<<24)	//don't use face as plane
#endif


#if 1
//texture flags
#define LBXGL_TXFL_NONE					BTGE_TXFL_NONE
#define LBXGL_TXFL_ALPHA				BTGE_TXFL_ALPHA			//image uses alpha channel
#define LBXGL_TXFL_VALPHA				BTGE_TXFL_VALPHA		//object uses alpha value
#define LBXGL_TXFL_NONSOLID				BTGE_TXFL_NONSOLID		//objects should be nonsolid
#define LBXGL_TXFL_DETAIL				BTGE_TXFL_DETAIL		//objects should be detail
#define LBXGL_TXFL_NOCSG				BTGE_TXFL_NOCSG			//object should not clip others
#define LBXGL_TXFL_FULLBRIGHT			BTGE_TXFL_FULLBRIGHT	//don't lightmap surface
#define LBXGL_TXFL_WINDOW				BTGE_TXFL_WINDOW		//object is window-like
// #define LBXGL_TXFL_WARPST				BTGE_TXFL_WARPST		//warp st coords
#define LBXGL_TXFL_GLOSS				BTGE_TXFL_GLOSS			//use surface gloss
#define LBXGL_TXFL_NODRAW				BTGE_TXFL_NODRAW		//don't render
#define LBXGL_TXFL_NEWSCREEN			BTGE_TXFL_NEWSCREEN		//GLSL: use new screen texture
#define LBXGL_TXFL_KEEPSCREEN			BTGE_TXFL_KEEPSCREEN	//GLSL: keep visible
#define LBXGL_TXFL_FLUID				BTGE_TXFL_FLUID			//object is fluid-like
#define LBXGL_TXFL_FLUID0				BTGE_TXFL_FLUID0		//fluid-type bit 0
#define LBXGL_TXFL_FLUID1				BTGE_TXFL_FLUID1		//fluid-type bit 1
#define LBXGL_TXFL_FLUID2				BTGE_TXFL_FLUID2		//fluid-type bit 2
#define LBXGL_TXFL_FLUID3				BTGE_TXFL_FLUID2		//fluid-type bit 3
#define LBXGL_TXFL_EFR0					BTGE_TXFL_EFR0			//effect reserved 0
#define LBXGL_TXFL_EFR1					BTGE_TXFL_EFR1			//effect reserved 1
#define LBXGL_TXFL_EFFECT				BTGE_TXFL_EFFECT		//special effect

#define LBXGL_TXFL_NOSHADOW				BTGE_TXFL_NOSHADOW		//don't render shadow
// #define LBXGL_TXFL_WARPXYZ				BTGE_TXFL_WARPXYZ		//warp xyz coords
// #define LBXGL_TXFL_SCROLLST				BTGE_TXFL_SCROLLST		//transform ST coords

#define LBXGL_TXFL_COLOR_UVAY			BTGE_TXFL_COLOR_UVAY

#define LBXGL_TXFL_LIGHTMAT				BTGE_TXFL_LIGHTMAT		//use lighting material

#define LBXGL_TXFL_WARP_MASK			BTGE_TXFL_WARP_MASK			//warp xyz coords
#define LBXGL_TXFL_WARP_NONE			BTGE_TXFL_WARP_NONE			//no warp
#define LBXGL_TXFL_WARP_ST				BTGE_TXFL_WARP_ST			//warp xyz coords
#define LBXGL_TXFL_WARP_XYZ				BTGE_TXFL_WARP_XYZ			//warp xyz coords
#define LBXGL_TXFL_WARP_SCROLLST		BTGE_TXFL_WARP_SCROLLST		//transform ST coords
#define LBXGL_TXFL_WARP_NBULGE			BTGE_TXFL_WARP_NBULGE		//bulge along normal
#define LBXGL_TXFL_WARP_JIGGLE			BTGE_TXFL_WARP_JIGGLE		//inertial jiggle

#define LBXGL_TXFL_MATERIAL_MASK		BTGE_TXFL_MATERIAL_MASK		//mask
#define LBXGL_TXFL_MATERIAL_NONE		BTGE_TXFL_MATERIAL_NONE		//generic solid
#define LBXGL_TXFL_MATERIAL_SKY			BTGE_TXFL_MATERIAL_SKY		//sky
#define LBXGL_TXFL_MATERIAL_GLASS		BTGE_TXFL_MATERIAL_GLASS	//glass
#define LBXGL_TXFL_MATERIAL_WOOD		BTGE_TXFL_MATERIAL_WOOD		//wood
#define LBXGL_TXFL_MATERIAL_METAL		BTGE_TXFL_MATERIAL_METAL	//metal
#define LBXGL_TXFL_MATERIAL_FLESH		BTGE_TXFL_MATERIAL_FLESH	//flesh
#define LBXGL_TXFL_MATERIAL_CONCRETE	BTGE_TXFL_MATERIAL_CONCRETE	//concrete
#define LBXGL_TXFL_MATERIAL_FOAM		BTGE_TXFL_MATERIAL_FOAM		//solid foam-like
#define LBXGL_TXFL_MATERIAL_COMPUTER	BTGE_TXFL_MATERIAL_COMPUTER	//computer/machine
#define LBXGL_TXFL_MATERIAL_UBGLASS		BTGE_TXFL_MATERIAL_UBGLASS	//unbreakable glass
#define LBXGL_TXFL_MATERIAL_STONE		BTGE_TXFL_MATERIAL_STONE	//stone/rocks
#define LBXGL_TXFL_MATERIAL_SLUDGE		BTGE_TXFL_MATERIAL_SLUDGE	//burning sludge
#define LBXGL_TXFL_MATERIAL_SOLID		BTGE_TXFL_MATERIAL_SOLID	//generic solid

#define LBXGL_TXFL_FLUID_MASK			BTGE_TXFL_FLUID_MASK		//mask
#define LBXGL_TXFL_FLUID_WATER			BTGE_TXFL_FLUID_WATER		//water
#define LBXGL_TXFL_FLUID_SLIME			BTGE_TXFL_FLUID_SLIME		//slime
#define LBXGL_TXFL_FLUID_LAVA			BTGE_TXFL_FLUID_LAVA		//lava
#define LBXGL_TXFL_FLUID_SEWER			BTGE_TXFL_FLUID_SEWER		//dirty water

#define LBXGL_TXFL_FLUID_PORTAL			BTGE_TXFL_FLUID_PORTAL		//portal

#define LBXGL_TXFL_NOPLANE				BTGE_TXFL_NOPLANE			//don't use face as plane

#define LBXGL_TXFL_ALPHA_LSHIFT			BTGE_TXFL_ALPHA_LSHIFT		//fluid-type bit 0
#define LBXGL_TXFL_ALPHA_MASK			BTGE_TXFL_ALPHA_MASK		//mask

#define LBXGL_TXFL_EFFECT_MASK			BTGE_TXFL_EFFECT_MASK		//mask
#define LBXGL_TXFL_EFFECT_LVOL			BTGE_TXFL_EFFECT_LVOL		//light volume
#define LBXGL_TXFL_EFFECT_FOG			BTGE_TXFL_EFFECT_FOG		//fog volume
#define LBXGL_TXFL_EFFECT_ORIGIN		BTGE_TXFL_EFFECT_ORIGIN		//origin
#define LBXGL_TXFL_EFFECT_ANTIPORTAL	BTGE_TXFL_EFFECT_ANTIPORTAL	//antiportal
#define LBXGL_TXFL_EFFECT_DECAL			BTGE_TXFL_EFFECT_DECAL		//decal (negative)
#define LBXGL_TXFL_EFFECT_VOXEL			BTGE_TXFL_EFFECT_VOXEL		//texture is a voxel type
#define LBXGL_TXFL_EFFECT_DECAL2		BTGE_TXFL_EFFECT_DECAL2		//decal (positive)
#define LBXGL_TXFL_EFFECT_DECAL_ALPHA	BTGE_TXFL_EFFECT_DECAL_ALPHA	//decal (alpha-blend)
#define LBXGL_TXFL_EFFECT_PORTAL		BTGE_TXFL_EFFECT_PORTAL		//portal

#define LBXGL_TXFL_SCALE_LSHIFT			BTGE_TXFL_SCALE_LSHIFT		//
#define LBXGL_TXFL_SCALE_MASK			BTGE_TXFL_SCALE_MASK		//mask

#endif


#define LBXGL_TEX_PLANAR				0
#define LBXGL_TEX_BOX					1
#define LBXGL_TEX_SPHERICAL				2
#define LBXGL_TEX_CYLINDRICAL			3
#define LBXGL_TEX_MULTIPLANAR			4

#define LBXGL_LFL_CHANGED				1		//light has changed since last frame
#define LBXGL_LFL_LINEAR				2		//light uses linear falloff
#define LBXGL_LFL_SPOTLIGHT				4		//light is a spotlight
#define LBXGL_LFL_NOSHADOW				8		//light does not cast shadows
#define LBXGL_LFL_VISTEST				16		//light does not cast shadows
#define LBXGL_LFL_LIGHTVOL				32		//light can make light volumes
#define LBXGL_LFL_DISABLE				64		//light is off/disabled
#define LBXGL_LFL_LASER					128		//light is a laser
#define LBXGL_LFL_SUN					256		//light is the sun
#define LBXGL_LFL_STATIC				512		//light is static / non-moving
#define LBXGL_LFL_ENVGLOSS				1024	//used as gloss envmap
#define LBXGL_LFL_ENVRENDER				2048	//render envmap from this spot
#define LBXGL_LFL_DETAIL				4096	//light is a detail light
#define LBXGL_LFL_BOXCULL				8192	//light uses bbox for culling
#define LBXGL_LFL_SCENERY				16384	//light applies to scenery
#define LBXGL_LFL_CAMREL				32768	//light is camera-relative

#define LBXGL_LFL_EFFECT_MASK			0x00FF0000
#define LBXGL_LFL_EFFECT_NONE			0x00000000

#define LBXGL_LFL_SHADOWMAP				0x01000000	//use shadow mapping

#define LBXGL_SFL_VIEWCHK				1	//check view in shadow
#define LBXGL_SFL_INFINITE				2	//shadow has infinite extents
#define LBXGL_SFL_NOENDCAPS				4	//skip end caps
#define LBXGL_SFL_XYZSTEP4				8	//XYZ stepping of 4
#define LBXGL_SFL_VIEWCAP				16	//draw view cap

#define LBXGL_MDL_SAVEREL_NUM			(2*4)
#define LBXGL_MDL_SAVEREL_DT			0.25

typedef struct LBXGL_TexImage_s LBXGL_TexImage;

typedef struct LBXGL_BinaryNode_s LBXGL_BinaryNode;
//typedef struct LBXGL_BrushNode_s LBXGL_BrushNode;
//typedef struct LBXGL_Brush_s LBXGL_Brush;

typedef struct BTGE_BrushNode_s LBXGL_BrushNode;
typedef struct BTGE_Brush_s LBXGL_Brush;

typedef struct BTGE_MeshPrim_s LBXGL_MeshPrim;
typedef struct BTGE_MeshGroup_s LBXGL_MeshGroup;

typedef struct lbxEntity_s lbxEntity;	//declared early

//typedef struct LBXGL_Light_s LBXGL_Light;
//typedef struct LBXGL_LightNode_s LBXGL_LightNode;

typedef struct BTGE_Light_s LBXGL_Light;
typedef struct BTGE_LightNode_s LBXGL_LightNode;

typedef struct LBXGL_Particle_s LBXGL_Particle;
typedef struct LBXGL_ParticleInfo_s LBXGL_ParticleInfo;
typedef struct LBXGL_ParticleNode_s LBXGL_ParticleNode;

//typedef struct LBXGL_ModelState_s LBXGL_ModelState;
typedef struct LBXGL_RenderMethods_s LBXGL_RenderMethods;

typedef struct BTGE_ModelState_s LBXGL_ModelState;


struct LBXGL_TexImage_s {
LBXGL_TexImage *next;	//next existing texture
char *name;				//texture image name
int num;				//raw texnum
int width, height;		//image width/height
int flags, glsdr;		//flags and shader
float gloss;			//gloss value
float alpha;			//alpha value
float xscale, yscale;	//texture scale
int tex_glow;			//glow map
int tex_norm;			//normal map
int tex_spec;			//specular (color) map
int tex_gloss;			//gloss (exponent) map
int tex_alt;			//alternate texture

int ndraw;
int (*draw[8])(LBXGL_TexImage *sdr, int lnum, int nvec,
	float *xyz, float *st, float *vn, float *clr);
char ufi[16], uvi[16], uti[16];
float ufv[16];
float uf4v[64];
int utex[16];
int ufn, ufvn, utn;

char *utexvn[16];
char *uf1vn[16];
char *uf4vn[16];

// char *utexvcn[16];
char *uf1vcn[16];
char *uf4vcn[16];

int anim_next;
int anim_alt;
float anim_time;
void *anim_video;		//videoMap

//texture color materials
float mat_ambient[4];
float mat_diffuse[4];
float mat_specular[4];
float mat_emission[4];
float mat_shininess[4];

//misc
LBXGL_TexImage *link;		//texture link (name alias)
LBXGL_TexImage *chain;		//temporary chain
LBXGL_TexImage *syncnext;	//next synchronized texture
LBXGL_Brush *tmp_brush;	//temporary brush list
int blend_src;
int blend_dst;
// int layer_next;
int flags2, flags3, flags4;

int layer_link[8];
int n_layer;

float warp_scale;
float warp_speed;
float warp_trans[3];	//xvel, yvel, avel

int (*texGen)(LBXGL_TexImage *sdr);

int *snd_dct;			//sound-DCT
int snd_n_dct;			//sound num DCT
};

struct LBXGL_Light_s {
LBXGL_Light *next;			//next (in list)
LBXGL_Light *tnext;			//next (temp/query list)
LBXGL_Light *chain;			//temp chain
LBXGL_Light *nnext;			//next (light node)
float org[3];				//light origin
float dir[3];				//light direction (spotlights)
float clr[3];				//light color
float val[2];				//light value (base, highlight)
float angle;				//light angle (spotlight)
int flags;					//light flags
int num;					//conceptual light number
int leaf;					//leafnum for light
int style;					//lightstyle
int envtex;					//environment map texture (cubemap)

char *texname;				//texture name
int tex_falloff;			//falloff texture

float mins[3];				//light bbox mins
float maxs[3];				//light bbox maxs
float amins[3];				//absolute mins
float amaxs[3];				//absolute maxs
float boxval[3];			//light box value

float tval;					//temp value (used in sorting)
float curval;				//current brightness value
LBXGL_Brush **visbrush;		//list of potentially visible brushes (static)

LBXGL_Brush **litbrush;		//brushes being lit by light (temp)
LBXGL_Brush **shadbrush;	//brushes casting shadow from light (temp)
LBXGL_ModelState **litmdl;	//models being lit by light (temp)
LBXGL_ModelState **shadmdl;	//models casting shadow from light (temp)
int sz_litbrush;			//size of lit brush array
int sz_shadbrush;			//size of shadow-brush array
int sz_litmdl;				//size of lit model array
int sz_shadmdl;				//size of shadow-model array
};

struct LBXGL_LightNode_s {
LBXGL_LightNode *lnode;
LBXGL_LightNode *rnode;
LBXGL_Light *mid;
float norm[4];
};

struct LBXGL_BinaryNode_s {
LBXGL_BinaryNode *lnode;
LBXGL_BinaryNode *rnode;
void *mid;
float norm[4];
};

#if 1
struct LBXGL_Particle_s {
LBXGL_Particle *next, *prev;
LBXGL_Particle *chain;
LBXGL_Particle *nnext;
float org[4];
float vel[4];
float time, lim;
float rad;	//sprite radius
int type;	//lightstyle
int tex;	//sprite-based particles
int clr;	//color for particles

float psi_lin;	//linear pressure coeff
float psi_quad;	//quadratic pressure coeff
float psi_cube;	//cubic pressure coeff

double buf[32];
};

struct LBXGL_ParticleInfo_s {
char *name;
void (*step)(LBXGL_Particle *self, float dt);
void (*draw)(LBXGL_Particle *self);
void (*spawn)(LBXGL_Particle *self, float *org, float *vel);
LBXGL_Particle *(*drawMulti)(LBXGL_Particle *self);
};

struct LBXGL_ParticleNode_s {
LBXGL_ParticleNode *left;
LBXGL_ParticleNode *right;
LBXGL_Particle *mid;
float norm[4];
};

#endif


struct LBXGL_ModelState_s {
LBXGL_ModelState *next;		//free for client use
LBXGL_ModelState *chain;	//temporary chains, ...
LBXGL_RenderMethods *mvt;

float org[3];
float rot[9];
void *mdl;
void *state;
char *mdltype;

int entnum;

int frame;	//frame-based models
int skin;	//models with multiple skins
int flags;	//same as texture flags
int effects;
int effects2;

float mins[3];		//AABB mins
float maxs[3];		//AABB maxs

//saved relative positions
float lr_org[LBXGL_MDL_SAVEREL_NUM*3];
float lr_rot[LBXGL_MDL_SAVEREL_NUM*4];
float lr_time[LBXGL_MDL_SAVEREL_NUM];
};

struct LBXGL_RenderMethods_s {
LBXGL_RenderMethods *next;
char *type;

int (*render_dark)(dyt obj);
int (*render_light)(dyt obj);
int (*render_shadow)(dyt obj, float *org);
int (*render_final)(dyt obj);
int (*render_draw)(dyt obj);

int (*get_sphere)(dyt obj, float *org, float *rag);

int (*drawmdl_dark)(LBXGL_ModelState *ctx);
int (*drawmdl_light)(LBXGL_ModelState *ctx);
int (*drawmdl_shadow)(LBXGL_ModelState *ctx, float *org, float rad);
int (*drawmdl_final)(LBXGL_ModelState *ctx);
int (*drawmdl_draw)(LBXGL_ModelState *ctx);
int (*drawmdl_flat)(LBXGL_ModelState *ctx);
int (*drawmdl_getSphere)(LBXGL_ModelState *ctx, float *org, float *rad);
int (*drawmdl_getBox)(LBXGL_ModelState *ctx, float *mins, float *maxs);

BTGE_BrushDrawFace *(*drawmdl_queryDrawFaces)(LBXGL_ModelState *ctx);
};

#endif
