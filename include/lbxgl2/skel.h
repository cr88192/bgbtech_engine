/*
Skeletal geometries will be handled specially.
*/

#ifndef __LBXGL_SKELGEOM_H__
#define __LBXGL_SKELGEOM_H__

#define LBXGL_SKELMESH_DEFMAXBONES	256
#define LBXGL_SKELMESH_DEFMAXVERTS	16384
#define LBXGL_SKELMESH_DEFMAXTRIS	4096
#define LBXGL_SKELMESH_DEFMAXANIMS	256
#define LBXGL_SKELANIM_DEFMAXFRAMES	256

typedef struct LBXGL_SkelBone_s LBXGL_SkelBone;
typedef struct LBXGL_SkelAnim_s LBXGL_SkelAnim;
typedef struct LBXGL_SkelVertex_s LBXGL_SkelVertex;
typedef struct LBXGL_SkelFace_s LBXGL_SkelFace;
typedef struct LBXGL_SkelMesh_s LBXGL_SkelMesh;

typedef struct LBXGL_SkelEvent_s LBXGL_SkelEvent;
typedef struct LBXGL_SkelState_s LBXGL_SkelState;


struct LBXGL_SkelBone_s {
char *name;
int parent;
float pos[3], rot[3];	//position in base model

float min[3], max[3];	//bounding box
float com[3], vol[3];	//center of mass, volume cube
float weight;		//bone weight (total volume)
};


struct LBXGL_SkelAnim_s {
char *name;
char *next;

int num_rnext;
char *rnext[8];
byte rnext_weight[8];

LBXGL_SkelEvent *events;
float fps;

int num_bones;	//number of bones referenced in this anim
int num_frames;	//number of frames in this anim
char **boneref;	//[num_bones], name of each bone in this anim
float **bonepos;	//[num_frames][num_bones*6]
};


struct LBXGL_SkelVertex_s {
float xyz[3];
float norm[3];
float st[2];
int bone;
};


struct LBXGL_SkelFace_s {
int xyz[3];
int st[3];
int norm[3];

int skinnum;
};


struct LBXGL_SkelMesh_s {
int num_bones, num_anims, num_tris;
//int num_verts;
LBXGL_SkelBone **bone;
LBXGL_SkelAnim **anim;
//LBXGL_SkelVertex **vertex;

int num_xyz, num_st, num_norm;
short	*v_xyzbone;	//vertex xyz bones [num_xyz]
float	*v_xyz;		//xyz vertices [num_xyz*3]
short	*v_normbone;	//vertex normal bones [num_norm]
float	*v_norm;	//vertex normals [num_norm*3]
float	*v_st;		//st vertices [num_st*2]

int num_skin;
char **skin_name;
int *skin_texnum;
char *base;

LBXGL_SkelFace **tris;
//int *tris;


float unit;		//base unit of measurement relative to the meter
};


struct LBXGL_SkelEvent_s {
LBXGL_SkelEvent *next;

int frame;
char *target;	//"SELF" for message
char *event;
void *args[16];
};


struct LBXGL_SkelState_s {
LBXGL_SkelMesh *mesh;
char *name;			//local name for model
int seq, frame;
float frametime;		//time remaining for current frame

float mins[3], maxs[3];		//current bounding box
float nmins[3], nmaxs[3];	//next bounding box
float light[4];			//current light level

float *basepos;			//current set of base positions
float *relpos;			//current set of relative positions
float *nrelpos;			//next set of relative positions

float *vel;			//current set of velocities (worldspace vecs)
float *xforms;			//current set of transforms
float *nxforms;			//next set of transforms

LBXGL_SkelState **attach;	//attached models, per bone
};

#endif
