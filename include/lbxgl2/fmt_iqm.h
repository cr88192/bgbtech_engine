// IQM: Inter-Quake Model format
// version 1: April 20, 2010
// version 2: May 31, 2011
//* explicitly store quaternion w to minimize animation jitter
//  modified joint and pose struct to explicitly store quaternion w in new channel 6 (with 10 total channels)

// all data is little endian

typedef struct IQM_Header_s		IQM_Header;
typedef struct IQM_Mesh_s 			IQM_Mesh;
typedef struct IQM_VertexArray_s	IQM_VertexArray;
typedef struct IQM_Triangle_s		IQM_Triangle;
typedef struct IQM_Adjacency_s		IQM_Adjacency;
typedef struct IQM_Joint_s			IQM_Joint;
typedef struct IQM_Pose_s			IQM_Pose;
typedef struct IQM_Anim_s			IQM_Anim;
typedef struct IQM_Bounds_s		IQM_Bounds;
typedef struct IQM_Extension_s		IQM_Extension;
typedef struct IQM_Vertex_s		IQM_Vertex;

#if 0
struct IQM_Model_s
{
IQM_Header head;
char *text;
char *comment;
IQM_Mesh *mesh;
IQM_VertexArray *vertexarray;
IQM_Triangle *triangle;
IQM_Joint *joint;
IQM_Pose *pose;
IQM_Anim *anim;
};
#endif

struct IQM_Header_s
{
char magic[16]; // the string "INTERQUAKEMODEL\0", 0 terminated
u32 version; // must be version 2
u32 filesize;
u32 flags;
u32 num_text, ofs_text;
u32 num_meshes, ofs_meshes;
u32 num_vertexarrays, num_vertexes, ofs_vertexarrays;
u32 num_triangles, ofs_triangles, ofs_adjacency;
u32 num_joints, ofs_joints;
u32 num_poses, ofs_poses;
u32 num_anims, ofs_anims;
u32 num_frames, num_framechannels, ofs_frames, ofs_bounds;
u32 num_comment, ofs_comment;
u32 num_extensions, ofs_extensions; // these are stored as a linked list, not as a contiguous array
};
// ofs_* fields are relative to the beginning of the header
// ofs_* fields must be set to 0 when the particular data is empty
// ofs_* fields must be aligned to at least 4 byte boundaries

struct IQM_Mesh_s
{
u32 name; // unique name for the mesh, if desired
u32 material; // set to a name of a non-unique material or texture
u32 first_vertex, num_vertexes;
u32 first_triangle, num_triangles;
};

// all vertex array entries must ordered as defined below, if present
// i.e. position comes before normal comes before ... comes before custom
// where a format and size is given, this means models intended for portable use should use these
// an IQM implementation is not required to honor any other format/size than those recommended
// however, it may support other format/size combinations for these types if it desires
	
// vertex array type
#define IQM_VTY_POSITION 		0  // float, 3
#define IQM_VTY_TEXCOORD 		1  // float, 2
#define IQM_VTY_NORMAL   		2  // float, 3
#define IQM_VTY_TANGENT  		3  // float, 4
#define IQM_VTY_BLENDINDEXES 	4  // ubyte, 4
#define IQM_VTY_BLENDWEIGHTS 	5  // ubyte, 4
#define IQM_VTY_COLOR			6  // ubyte, 4

// all values up to CUSTOM are reserved for future use
// any value >= CUSTOM is interpreted as CUSTOM type
// the value then defines an offset into the string table, where offset = value - CUSTOM
// this must be a valid string naming the type
#define IQM_VTY_CUSTOM  	 	0x10

// vertex array format
#define IQM_VF_BYTE   	0
#define IQM_VF_UBYTE  	1
#define IQM_VF_SHORT  	2
#define IQM_VF_USHORT 	3
#define IQM_VF_INT		4
#define IQM_VF_UINT   	5
#define IQM_VF_HALF   	6
#define IQM_VF_FLOAT  	7
#define IQM_VF_DOUBLE 	8

struct IQM_VertexArray_s
{
u32 type;   // type or custom name
u32 flags;
u32 format; // component format
u32 size;   // number of components
u32 offset; // offset to array of tightly packed components, with num_vertexes * size total entries
 // offset must be aligned to max(sizeof(format), 4)
};

struct IQM_Triangle_s
{
u32 vertex[3];
};

struct IQM_Adjacency_s
{
u32 triangle[3];
};

struct IQM_Joint_s
{
u32 name;
int parent; // parent < 0 means this is a root bone
float translate[3], rotate[4], scale[3]; 
// translate is translation <Tx, Ty, Tz>, and rotate is quaternion rotation <Qx, Qy, Qz, Qw>
// rotation is in relative/parent local space
// scale is pre-scaling <Sx, Sy, Sz>
// output = (input*scale)*rotation + translation
};

struct IQM_Pose_s
{
int parent; // parent < 0 means this is a root bone
u32 channelmask; // mask of which 10 channels are present for this joint pose
float channeloffset[10], channelscale[10]; 
// channels 0..2 are translation <Tx, Ty, Tz> and channels 3..6 are quaternion rotation <Qx, Qy, Qz, Qw>
// rotation is in relative/parent local space
// channels 7..9 are scale <Sx, Sy, Sz>
// output = (input*scale)*rotation + translation
};

// ushort frames[]; // frames is a big unsigned short array where each group of framechannels components is one frame

struct IQM_Anim_s
{
u32 name;
u32 first_frame, num_frames; 
float framerate;
u32 flags;
};

enum
{
IQM_ANIM_LOOP = 1<<0
};

struct IQM_Bounds_s
{
float bbmins[3], bbmaxs[3]; // the minimum and maximum coordinates of the bounding box for this animation frame
float xyradius, radius; // the circular radius in the X-Y plane, as well as the spherical radius
};

// char text[]; // big array of all strings, each individual string being 0 terminated
// char comment[];

struct IQM_Extension_s
{
u32 name;
u32 num_data, ofs_data;
u32 ofs_extensions; // pointer to next extension
};

// vertex data is not really interleaved, but this just gives examples of standard types of the data arrays
struct IQM_Vertex_s
{
float position[3], texcoord[2], normal[3], tangent[4];
byte blendindices[4], blendweights[4], color[4];
};
