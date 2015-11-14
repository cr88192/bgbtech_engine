#define VOX_DTY_UNASSIGNED			0	//unassigned
#define VOX_DTY_NONE				1	//none (left unchanged)

#define VOX_DTY_HALL_X_BRICK		4	//hallway, X-axis, brick
#define VOX_DTY_HALL_Y_BRICK		5	//hallway, Y-axis, brick
#define VOX_DTY_HALL_XY_BRICK		6	//hallway, XY cross, brick

#define VOX_DEDGE_NX				0	//-X
#define VOX_DEDGE_PX				1	//+X
#define VOX_DEDGE_NY				2	//-Y
#define VOX_DEDGE_PY				3	//+Y
#define VOX_DEDGE_NZ				4	//-Z
#define VOX_DEDGE_PZ				5	//+Z

typedef struct BTGE_VoxelDungeonInfo_s BTGE_VoxelDungeonInfo;

struct BTGE_VoxelDungeonInfo_s
{
int type;
char *name;
byte vox_idx[16*16*16];	//voxel indices
short allow_edge[6][16];
short allow_n_edge[6];

short allow_biome[16];
short allow_n_biome;
short deny_biome[16];
short deny_n_biome;

BTGE_VoxelData idx_vox[64];	//index voxels
byte idx_rgba[64*4];		//index RGBA colors
int n_idx;					//number of indices

short min_z;
short max_z;
};