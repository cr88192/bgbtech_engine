#define VOX_TY_AIR			0
#define VOX_TY_STONE		1
#define VOX_TY_GRASS		2
#define VOX_TY_DIRT			3
#define VOX_TY_COBBLESTONE	4
#define VOX_TY_WOOD			5
#define VOX_TY_SAPLING		6
#define VOX_TY_BEDROCK		7
#define VOX_TY_WATER		8
#define VOX_TY_WATER2		9
#define VOX_TY_LAVA			10
#define VOX_TY_LAVA2		11
#define VOX_TY_SAND			12
#define VOX_TY_GRAVEL		13
#define VOX_TY_GOLDORE		14
#define VOX_TY_IRONORE		15
#define VOX_TY_COAL			16
#define VOX_TY_LOG			17
#define VOX_TY_LEAVES		18
#define VOX_TY_SPONGE		19
#define VOX_TY_GLASS		20

#define VOX_FL_NX			1
#define VOX_FL_PX			2
#define VOX_FL_NY			4
#define VOX_FL_PY			8
#define VOX_FL_NZ			16
#define VOX_FL_PZ			32

#define VOX_FL_NXPZ			64
#define VOX_FL_PXPZ			128
#define VOX_FL_NYPZ			256
#define VOX_FL_PYPZ			512

#define VOX_FL_NXPY			1024
#define VOX_FL_PXNY			2048
#define VOX_FL_PXPY			4096
#define VOX_FL_NXNY			8192

#define VOX_FL_NXPYPZ		16384
#define VOX_FL_PXNYPZ		32768
#define VOX_FL_PXPYPZ		65536
#define VOX_FL_NXNYPZ		131072

typedef struct LBXGL_VoxelInfo_s LBXGL_VoxelInfo;
typedef struct LBXGL_VoxelChunk_s LBXGL_VoxelChunk;
typedef struct LBXGL_VoxelRegion_s LBXGL_VoxelRegion;

struct LBXGL_VoxelInfo_s {
char *name;				//name of voxel type
int img_top;		//top texture
int img_side;		//side texture
int img_bot;		//bottom texture
int flags;		//combined flags
};

typedef struct {
byte type;	//basic voxel type
byte aux;	//high 4: light, low 4: special
}LBXGL_VoxelData;

struct LBXGL_VoxelChunk_s {
LBXGL_VoxelChunk *vnext;	//visible next
LBXGL_VoxelRegion *rgn;		//owning region
LBXGL_VoxelData *vox;	//voxel array
int ox, oy, oz;			//origin (voxel space)
int xs, ys, zs;			//size (voxels)
byte clean;

//visible faces (quads)
float *face_xyz;	//XYZ
float *face_st;		//ST
float *face_norm;	//normals (XYZ)
float *face_rgba;	//color (RGBA)
int *face_tex;		//texture
int n_face, m_face;
};

struct LBXGL_VoxelRegion_s {
LBXGL_VoxelRegion *next;	//next region (in world)
LBXGL_VoxelChunk **chk;		//voxel chunk array
LBXGL_VoxelChunk *vischk;	//visible chunk list
int ox, oy, oz;				//origin (voxel space)
int xs, ys, zs;				//region size (chunks)
int xst, yst, zst;			//chunk size
float voxsz;				//voxel size (units)
};


typedef struct LBXGL_NoiseContext_s LBXGL_NoiseContext;

struct LBXGL_NoiseContext_s {
float *arr[4];	//value arrays
int *parr[4];		//permutation array
int ord;		//order of context
int sz;			//size of array
s64 seed;
};
