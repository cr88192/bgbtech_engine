//Quake MDL

#ifndef __LBXGL_FMT_QMDL_H__
#define __LBXGL_FMT_QMDL_H__

#define QMDL_ID			(('O'<<24)+('P'<<16)+('D'<<8)+'I')
#define QMDL_VERSION		6
#define QMDL_ONSEAM		0x20
#define QMDL_DT_FACES_FRONT	0x10
#define QMDL_ST_SYNC		0
#define QMDL_ST_RAND		1

#define QMD2_ID			(('2'<<24)+('P'<<16)+('D'<<8)+'I')
#define QMD2_VERSION		8

#define QMD3_ID			(('3'<<24)+('P'<<16)+('D'<<8)+'I')
#define QMD3_VERSION		15

#define QMD4_ID			(('4'<<24)+('P'<<16)+('D'<<8)+'I')
#define QMD4_VERSION		1

//#define QMDL_SCALE		(1.0/36.0)
//#define QMD2_SCALE		(1.0/36.0)
//#define	QMD3_SCALE		(1.0/36.0)
//#define	QMD4_SCALE		(1.0/36.0)

#define QMDL_SCALE		1.0
#define QMD2_SCALE		1.0
#define	QMD3_SCALE		1.0
#define	QMD4_SCALE		1.0

typedef struct {
int	ident;
int	version;
float	scale[3];
float	scale_origin[3];
float	boundingradius;
float	eyeposition[3];
int	numskins;
int	skinwidth;
int	skinheight;
int	numverts;
int	numtris;
int	numframes;
int	synctype;
int	flags;
float	size;
}quake_mdl_t;

typedef struct
{
int	ident;
int	version;
int	skinwidth;
int	skinheight;
int	framesize;
int	num_skins;
int	num_xyz;
int	num_st;
int	num_tris;
int	num_glcmds;
int	num_frames;
int	ofs_skins;
int	ofs_st;
int	ofs_tris;
int	ofs_frames;
int	ofs_glcmds;	
int	ofs_end;
}quake_md2_t;

typedef struct {
int	ident;			//MDL, MD2, MD3, MD4
int	version;		//MDL, MD2, MD3, MD4
int	num_frames;		//MDL, MD2, MD3, MD4

int	num_skins;		//MDL, MD2, MD3
int	skinwidth;		//MDL, MD2
int	skinheight;		//MDL, MD2
int	num_tris;		//MDL, MD2
int	flags;			//MDL, MD3

float	scale[3];		//MDL
float	scale_origin[3];	//MDL
float	boundingradius;		//MDL
float	eyeposition[3];		//MDL
int	numverts;		//MDL
int	synctype;		//MDL
float	size;			//MDL

int	ofs_skins;		//MD2, MD3
int	ofs_frames;		//MD2, MD3, MD4

int	framesize;		//MD2
int	num_xyz;		//MD2
int	num_st;			//MD2
int	num_glcmds;		//MD2
int	ofs_st;			//MD2
int	ofs_tris;		//MD2
int	ofs_glcmds;		//MD2
int	ofs_end;		//MD2

int	num_tags;		//MD3
int	num_surfaces;		//MD3
int	ofs_tags;		//MD3
int	ofs_surfaces;		//MD3

int	num_bones;		//MD4
int	num_lods;		//MD4
int	ofs_lods;		//MD4
}quake_qmdl_t;

typedef struct {
float vertex[3];
float normal[3];
float st[2];
int num_weights;

int *bone_indices;	//MD4, index into bonerefs
float *bone_weights;	//MD4, relative power of bone weights
}quake_md4vertex_t;

typedef struct {
int	ident;		//MD3, MD4
char	name[64];	//MD3, MD4
int	num_verts;	//MD3, MD4
int	num_triangles;	//MD3, MD4
int	ofs_triangles;	//MD3, MD4
int	ofs_end;	//MD3, MD4

int	flags;		//MD3
int	num_frames;	//MD3
int	num_shaders;	//MD3
int	ofs_shaders;	//MD3
int	ofs_st;		//MD3
int	ofs_xyz;	//MD3

int *tris;		//MD3, v1 v2 v3
char **shaders;		//MD3
float *st;		//MD3
float **frames;		//MD3
int *sdrnums;		//MD3: gl texture numbers

char	shader[64];	//MD4
int	ofs_header;	//MD4
int	ofs_verts;	//MD4
int	num_bonerefs;	//MD4
int	ofs_bonerefs;	//MD4

quake_md4vertex_t **md4verts;	//MD4
int *bonerefs;			//MD4
}quake_md3surf_t;

typedef struct {
int	num_surfaces;		//MD4
int	ofs_surfaces;		//MD4
int	ofs_end;		//MD4
quake_md3surf_t **surfs;	//MD4
}quake_md4lod_t;

typedef struct LBXGL_QMDL_s LBXGL_QMDL;

struct LBXGL_QMDL_s {
LBXGL_QMDL *next;
char *name;

quake_qmdl_t head;

byte *palette;	//MDL

byte **skins;	//MDL, MD2: stored RGBA
int *skinnums;	//MDL, MD2: gl texture numbers
char **skinnames; //skin names (q2)

float *st_vecs;	//MDL, MD2
int *st_seam;	//MDL: seam values (q1)
int *polys;	//MDL, MD2: v1 v2 v3 front(q1) st1 st2 st3 pad

float **frames;	//MDL, MD2, MD4
char **framenames;	//MDL, MD2, MD3, MD4

float *tags;			//MD3
char **tagnames;		//MD3
quake_md3surf_t **surfs;	//MD3

quake_md4lod_t	**lods;		//MD4
};

#endif
