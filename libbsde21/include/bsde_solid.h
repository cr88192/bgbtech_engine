#ifndef BSDE_SOLID_H
#define BSDE_SOLID_H

typedef struct BSDE_Solid_s BSDE_Solid;
typedef struct BSDE_SoftBody_Vertex_s BSDE_SoftBody_Vertex;
typedef struct BSDE_SoftBody_Element_s BSDE_SoftBody_Element;
typedef struct BSDE_SoftBody_Face_s BSDE_SoftBody_Face;

typedef struct BSDE_Contact_s BSDE_Contact;
typedef struct BSDE_Joint_s BSDE_Joint;
typedef struct BSDE_World_s BSDE_World;
typedef struct BSDE_QuadNode_s BSDE_QuadNode;
typedef struct BSDE_BinaryNode_s BSDE_BinaryNode;
typedef struct BSDE_ThreadState_s BSDE_ThreadState;

struct BSDE_Solid_s {
//solid management
BSDE_Solid *qnext;	//next in quad/oct tree
BSDE_Solid *chain;	//chained solid
BSDE_BinaryNode *node;	//linked in node

BSDE_World *world;	//world solid belongs to
int handle;		//handle allocated to this solid

BSDE_Joint *joint;	//first joint
void *data;		//solid specific data

//solid

int solidtype;	//solid type
int moveflags;	//flags effecting the movement of an object
int stateflags;	//internally managed flags
bsde_real idletime;

//general data
bsde_real mins[3], maxs[3];	//AABB or OBB extents
bsde_real height, radius;	//height and radius (depend on type)
bsde_real dir[4];		//direction (planes)

//general static properties
bsde_real mass;
bsde_real volume;		//internal volume
bsde_real mass_offs[3];	//XYZ, mass offset
bsde_real mass_imat[9];	//inertial matrix
bsde_real mass_tmat[9];	//inverse inertial matrix
bsde_real elastic;		//surface elastic
bsde_real friction;	//surface friction
bsde_real viscosity;	//fluid viscosity
bsde_real moment;		//inertial scalar

//temp state
bsde_real mmins[3];	//move min extents
bsde_real mmaxs[3];	//move max extents
bsde_real ivel[3];		//impulse velocity
bsde_real itvel[3];	//impulse torque
bsde_real avel[3];		//angular velocity
bsde_real wtmat[9];	//world inverse inertial tensor, I^-1
bsde_real wimat[9];	//world inertial tensor, I

//general state
bsde_real org[4];		//XYZ0
bsde_real rot[4];		//quaternion
bsde_real vel[4];		//XYZ0
bsde_real tvel[4];		//XYZ0, torque/inertia
bsde_real xform[16];	//current transform

//predicted ending state
bsde_real e_org[4];		//XYZ0
bsde_real e_rot[4];		//quaternion
bsde_real e_vel[4];		//XYZ0
bsde_real e_tvel[4];	//XYZ0, torque/inertia
bsde_real e_xform[16];	//predicted transform

//special state (user forces, ...)
bsde_real u_wvel[3];	//desired contact velocity
bsde_real u_acc[3];	//acceleration
bsde_real u_racc[3];	//localspace acceleration (thrust)

bsde_real u_wantorg[3];	//desired origin
bsde_real u_wantvel[3];	//desired velocity
bsde_real u_wantrot[4];	//desired velocity

bsde_real u_resv[5];	//space for more vectors
bsde_real u_wpow;		//contact velocity power

//solidtype related and similar
int n_vecs;
int n_faces;
bsde_real *vecs;	//vertices
bsde_real *norms;	//face normals (x y z d)
int *faces;	//faces/triangle vertex indices
int *facevecs;	//vertices/face (hulls)
bsde_real *evecs;	//edge normals

//bsde_real *ovecs;	//outer vertices
//bsde_real n_ovecs;	//num outer vertices

bsde_real *e_vecs;		//predicted vertices (trimesh)
bsde_real *e_evecs;	//predicted edge vectors (trimesh)

//Soft-Body / FEM
BSDE_SoftBody_Vertex **fem_vtx;
BSDE_SoftBody_Element **fem_elem;
BSDE_SoftBody_Face **fem_face;
int fem_n_vtx;
int fem_n_elem;
int fem_n_face;

//Voxels
int vox_xs, vox_ys, vox_zs;		//Chunk Size (Voxels)
bsde_real vox_vsz;				//Voxel Size

int vox_type_size;				//Voxel Type Size
int vox_type_type;				//Voxel Type Type
int vox_type_step;				//Voxel Type Stride/Step
int vox_type_num;				//Voxel Type Count
void *vox_type_data;			//Voxel Type Data

int vox_meta_size;				//Voxel Meta Size
int vox_meta_type;				//Voxel Meta Type
int vox_meta_step;				//Voxel Meta Stride/Step
int vox_meta_num;				//Voxel Meta Count
void *vox_meta_data;
};

struct BSDE_SoftBody_Vertex_s {
int type;
bsde_real org[3];
bsde_real vel[3];
bsde_real acc[3];
bsde_real mass;
};

struct BSDE_SoftBody_Element_s {
int type, flags;
int vecs[8];
bsde_real lens[12];
//int *vertex;	//vertex array
//bsde_real *length;	//length array
bsde_real vol;	//area or volume
};

struct BSDE_SoftBody_Face_s {
int type;
int vecs[4];
};


struct BSDE_Contact_s {
BSDE_Solid *obja;	//first colliding object
BSDE_Solid *objb;	//second colliding object

bsde_real org[3];		//intersection point
bsde_real norm[3];		//collision normal
bsde_real depth;		//penetration depth
//norm and depth define the amount obja should move
//norm and -depth define the amount objb should move

int npts;		//number of contact points
bsde_real pts[16*4];	//points
};

struct BSDE_Joint_s {
BSDE_Solid *obja;	//first object
BSDE_Solid *objb;	//second object
BSDE_Joint *linka;	//next joint for obja
BSDE_Joint *linkb;	//next joint for objb

int type;
int flags;

bsde_real orga[3];		//joint position 0 (a's local space)
bsde_real orgb[3];		//joint position 0 (b's local space)
bsde_real dira[3];		//joint direction 0 (a's local space)
bsde_real dirb[3];		//joint direction 0 (b's local space)
bsde_real dir1a[3];	//joint direction 1 (a's local space)
bsde_real dir1b[3];	//joint direction 1 (b's local space)

bsde_real org1a[3];	//joint position 1 (a's local space)
bsde_real org1b[3];	//joint position 1 (b's local space)
bsde_real org2a[3];	//joint position 2 (a's local space)
bsde_real org2b[3];	//joint position 2 (b's local space)
bsde_real org3a[3];	//joint position 3 (a's local space)
bsde_real org3b[3];	//joint position 3 (b's local space)

bsde_real str[4];		//strength factors
bsde_real attn[4];		//attenuation factors (a, b, c, d), ax^3+bx^2+cx+d
bsde_real radius;
};



struct BSDE_World_s {
BSDE_Solid **solids;
int n_solids;

BSDE_Joint **joints;
int n_joints;

BSDE_Solid **stack;
int stackpos, stacksize;

BSDE_Contact **contact;
//BSDE_QuadNode *quad;
void *root;		//root of partitioning tree

bsde_real gravity[3];	//gravity vector
bsde_real maxstep;		//maximum step size
bsde_real minstep;		//minimum step size
bsde_real maxframe;	//maximum frame size

bsde_real worldtime;	//accumulated world time
bsde_real steptime;	//remaining step time
bsde_real realtime;	//approximate "realtime"

bsde_real sc_len;
bsde_real sc_vol;
bsde_real sc_mass;
bsde_real sc_force;

int flags;		//world-related flags
int passes;		//passes per frame

int *tr_arr;	//trigger array [self, other]
int tr_num;	//size of trigger array
int tr_cur;	//current spot in trigger array

void *t_arr_hs;	//self handles
void *t_arr_ho;	//other handles
void *t_arr_o;	//(x, y, z) origins
void *t_arr_n;	//(x, y, z, d) normals
int t_str_hs;	//self handle stride
int t_str_ho;	//other handle stride
int t_str_o;	//origin stride
int t_str_n;	//normal stride
int t_ty_hs;	//self handle type
int t_ty_ho;	//other handle type
int t_ty_o;	//origin type
int t_ty_n;	//normal type

int t_num;	//max size of contact array
int t_cur;	//current index in contact array

unsigned int rng2_table[64];
int rng2_i;
long long rng3_state;
};

struct BSDE_QuadNode_s {
BSDE_QuadNode *node[4];
bsde_real center[4];
BSDE_Solid *first;	//first solid in node
};

struct BSDE_BinaryNode_s {
BSDE_BinaryNode *node[2], *up;
bsde_real norm[4];
BSDE_Solid *first;	//first solid in node
};

struct BSDE_ThreadState_s {
int id;		//client ID
int msg_id;	//message target ID
int world;	//world id

int *build_array;
bsde_real *vector_array;
bsde_real *scalar_array;
int build_array_pos;
int vector_array_pos;
int scalar_array_pos;
int build_array_sz;
int vector_array_sz;
int scalar_array_sz;

int build_level;
int build_toplevel;
};

#endif
