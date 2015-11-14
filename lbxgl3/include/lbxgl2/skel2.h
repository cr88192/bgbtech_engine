#define BONE_SOLID_BBOX		1
#define BONE_SOLID_CAPSULE	2
#define BONE_SOLID_SPHERE	3


typedef struct LBXGL_Skel2MeshFace_s LBXGL_Skel2MeshFace;
typedef struct LBXGL_Skel2Mesh_s LBXGL_Skel2Mesh;
typedef struct LBXGL_Skel2MeshGroup_s LBXGL_Skel2MeshGroup;

typedef struct LBXGL_Skel2Solid_s LBXGL_Skel2Solid;
typedef struct LBXGL_Skel2Anim_s LBXGL_Skel2Anim;
typedef struct LBXGL_Skel2Model_s LBXGL_Skel2Model;
typedef struct LBXGL_Skel2ModelVBO_s LBXGL_Skel2ModelVBO;

typedef struct LBXGL_Skel2MeshState_s LBXGL_Skel2MeshState;
typedef struct LBXGL_Skel2MeshStateGroup_s LBXGL_Skel2MeshStateGroup;
typedef struct LBXGL_Skel2State_s LBXGL_Skel2State;

struct LBXGL_Skel2MeshFace_s {
LBXGL_Skel2MeshFace *next;	//next face (in general)
int tnext;	//next face (with same texture)
int vecs[3];
int stvecs[3];
float norm[3];
float nvecs[3*3];
};

struct LBXGL_Skel2Mesh_s {
LBXGL_Skel2Mesh *next;
LBXGL_Skel2MeshFace *faces;	//first face
char *name;

int n_tex;
char *texname[16];
int texnum[16];
int texface[16];	//first face with a given texture

LBXGL_Skel2MeshFace **face;
int n_faces;	//number of faces
int b_faces;	//base index of faces

float *vecs;	//xyz vertices
float *stvecs;	//st vertices
int n_vecs;		//number of vertices
int n_stvecs;	//number of st vertices
int b_vecs;		//base index of xyz vertices
int b_stvecs;	//base index of st vertices

int *vfidx;		//vertex first face index
int *vfnum;		//vertex num faces
int *vfids;		//vertex face ids

int *vidx;		//vertex first weight
int *vnum;		//vertex num weights

int n_weights;	//number of weights
float *wxyz;	//weight xyz vecs
float *wvnv;	//weight normal vecs
int *widx;		//weight bone indices
float *wval;	//weight values

int *edge;		//shiluette edges
int n_edge;		//number of edges

float com_mins[3];
float com_maxs[3];
float com_org[3];
float com_rad;
};

struct LBXGL_Skel2MeshGroup_s {
LBXGL_Skel2MeshGroup *next;
LBXGL_Skel2MeshGroup *first;
LBXGL_Skel2Mesh *prim;
char *name;
};


struct LBXGL_Skel2Anim_s {
LBXGL_Skel2Anim *next;
char *name, *next_anim;

int num_frames;
float *frame_org;	//org in each frame
float *frame_rot;	//rot in each frame
byte *frame_mask;	//which keyframes are set

float frame_time;
int frame_base;		//base for animating via frame-numbers
};


struct LBXGL_Skel2Solid_s {
int type;
int bone;
float m[3];	//point 0 for capsules, min for bbox
float n[3];	//point 1 for capsules, max for bbox
float r0, r1;	//radius inner and outer
float w;
};

struct LBXGL_Skel2Model_s {
int num_bones;
int max_bones;		//number of bones to leave space for

char **bone_name;
int *bone_up;
float *bone_org;	//org in base frame
float *bone_rot;	//rot in base frame
float *bone_ang;	//angles in base frame

//hitbox
float *bone_mins;	//estimated mins in base frame
float *bone_maxs;	//estimated maxs in base frame

int num_solids;
LBXGL_Skel2Solid **solid;

LBXGL_Skel2MeshGroup *mesh_grp[64];	//attached meshes
LBXGL_Skel2MeshGroup *mesh;	//vertex-skinned meshes
LBXGL_Skel2Anim *anim;		//anims
LBXGL_Skel2ModelVBO *mesh_vbos;		//LOD VBO layouts

LBXGL_Skel2MeshGroup *mesh_lod[8];	//LOD meshes
LBXGL_Skel2ModelVBO *mesh_lod_vbos[8];	//LOD VBO layouts

float mdlorg[3];	//model origin
float mdlscale[3];		//final scale factors
float mdlxlat[3];		//translation

float radius;			//estimated radius (for culling)

int n_xyz;			//number of vertices in model
int n_st;			//number of st vertices in model
int n_face;			//number of faces in model

int num_poses;
int max_poses;
LBXGL_Skel2State **pose;
};

//info about VBO layout for model
struct LBXGL_Skel2ModelVBO_s {
float *arr_st;				//ST values
byte *arr_rgba;				//RGBA values
int n_st, m_st;				//num/max ST values
int n_rgba, m_rgba;			//num/max RGBA values

int *xyz_idx;				//indices (XYZ, ST, Norm, RGBA)
int n_xyz;					//num vertices
int m_xyz;					//max vertices

//offsets within VBO
int ofs_xyz;				//XYZ offset
int ofs_st;					//ST offset
int ofs_norm;				//vertex normals (XYZ)
int ofs_rgba;				//base color (RGBA)
int ofs_vlrgba;				//dark color (RGBA)

int sz_xyz;					//XYZ size
int sz_st;					//ST size
int sz_norm;				//normals size (XYZ)
int sz_rgba;				//base color (RGBA)
int sz_vert;				//size of vertex VBO

//VBO for elements
int *elems;					//element indices
int n_elem;					//number of elements
int m_elem;					//max number of elements

int vbo_elem;				//base VBO for elements

//per-texture element segments
int *seg_idx;			//elem indices (base, count, tex, texfl)
int n_seg;				//number of texures
int m_seg;				//number of texures
};

struct LBXGL_Skel2MeshState_s {
LBXGL_Skel2MeshState *next;
LBXGL_Skel2Mesh *prim;	//actual mesh
float *curvecs;		//vertices, current frame
float *curnorms;	//face normals, current frame
float *curvnorms;	//face vectex normals, current frame
};

struct LBXGL_Skel2MeshStateGroup_s {
LBXGL_Skel2MeshStateGroup *next;
LBXGL_Skel2MeshStateGroup *first;
LBXGL_Skel2MeshState *prim;
char *name;
};

struct LBXGL_Skel2State_s {
LBXGL_Skel2Model *mdl;
LBXGL_Skel2Anim *anim;
int frame_cur;
float frame_time;
int bone_sel;
int solid_sel;

int frame_calc;		//last frame still good

float *cur_xyz;		//vertices, current frame
float *cur_norm;	//face normals, current frame
float *cur_vnorm;	//face vectex normals, current frame


LBXGL_Skel2Anim *last_anim;		//anim for last frame
int last_frame;					//last frame
float backlerp;					//lerp towards last frame
int lod;						//draw at lod level

float *bone_rorg;	//org current frame
float *bone_rmat;	//rot current frame
int *bone_flag;		//bone flags
float *bone_uorg;	//user-defined org
float *bone_umat;	//user-defined rot

LBXGL_Skel2MeshStateGroup *mesh;

//VBOs
int vbo_vert;						//VBO for vertices
int vbo_elem;						//VBO for elements
LBXGL_Skel2ModelVBO *vbo_info;		//current VBO layout

byte *vbo_vertbuf;
};
