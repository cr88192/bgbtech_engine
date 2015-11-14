#ifndef LBXGL2_BSP_H
#define LBXGL2_BSP_H

#define BSP_FOURCC(a, b, c, d)	((a)|((b)<<8)|((c)<<16)|((d)<<24))

#define BSP_UPPER(c)		((((c)>='A') && ((c)<='Z')) || ((c)=='~'))
#define BSP_UPPER_A(fcc)	BSP_UPPER((fcc)&0xFF)
#define BSP_UPPER_B(fcc)	BSP_UPPER(((fcc)>>8)&0xFF)
#define BSP_UPPER_C(fcc)	BSP_UPPER(((fcc)>>16)&0xFF)
#define BSP_UPPER_D(fcc)	BSP_UPPER(((fcc)>>24)&0xFF)

#define BSPFACE_FL_ONNODE	1	//face fell on a node plane
#define BSPFACE_FL_INSIDE	2	//CSG: face on inside edge of a plane
#define BSPFACE_FL_OUTSIDE	4	//CSG: face on outside edge of a plane
#define BSPFACE_FL_OUTSIDE2	8	//CSG: face is not inside anything
#define BSPFACE_FL_WINDOW	16	//GEN: may be translucent
#define BSPFACE_FL_NOCSG	32	//CSG: don't clip things by this
#define BSPFACE_FL_NOSUB	64	//LM: don't subsample lighting
#define BSPFACE_FL_PORTAL	128	//GEN: face is a portal
#define BSPFACE_FL_BACKSIDE	256	//GEN: face backside of a face

#define BSPNODE_FL_SEEN		1	//VIS: leaf was visible

#define BSPLM_FL_MONO		1	//LM: lightmap is mono

typedef struct BSP_Brush_s BSP_Brush;
typedef struct BSP_Face_s BSP_Face;
typedef struct BSP_LM_s BSP_LM;
typedef struct BSP_Model_s BSP_Model;
typedef struct BSP_Node_s BSP_Node;

struct BSP_Brush_s {
BSP_Brush *next;
BSP_Face *face;
int flags;
};

struct BSP_Face_s {
BSP_Face *next;
BSP_Face *lmnext;
BSP_Face *nnext;	//next in node
BSP_LM *lm;
BSP_Node *node;		//node containing face
BSP_Node *leaf;		//leaf containing face

float norm[4], sdir[4], tdir[4];
int nvecs;
float *vecs;
float *evecs;
float *st;
float *uv;
int *vidx;		//lm vertex indices
byte *rgb;		//cached lighting values
int num, lmnum, flags;
char *tex;

BSP_Node *lleaf;	//left leaf
BSP_Node *rleaf;	//right leaf
BSP_Face *llnext;	//leaf leaf next
BSP_Face *rlnext;	//right leaf next
};

struct BSP_Node_s {
BSP_Node *lnode, *rnode;
BSP_Face *face;
BSP_Face *prt;
BSP_Face *tsurf;
float norm[4];
float mins[3], maxs[3];
float org[3];
float radius;
int num, flags;
int szvis;
byte *vis;

struct LBXGL_Entity_s *ents;
};

struct BSP_LM_s {
BSP_LM *next;
BSP_Face *face;

int xs, ys, texnum, num;
byte *lmbuf;	//lightmap data
byte *txbuf;	//crude texture

float norm[4];
float udir[3], vdir[3];		//uv vectors
float min[2], max[2];		//uv extents
float usz, vsz;

int dxs, dys, dn, fl;
byte *dlmbuf[16];	//dynamic lightmap data
int dlmstyle[16];	//lightstyle values
int dlmnum[16];		//lightstyle texnum

int num_vecs;
float *vec_xyz;		//merged vertices
float *vec_uv;		//merged uv (matches with xyz)
byte *vec_rgb;		//cached lighting values (dlights)

byte *vec_srgb;		//cached static lighting values
byte *vec_drgb[16];	//cached dynamic lighting values
};


struct BSP_Model_s {
BSP_Model *next;

BSP_Brush *brush;
BSP_Node *root;
BSP_LM *lmroot;

int num_tex;
char **texnames;
int *tex_xs;
int *tex_ys;
int *tex_fl;
int *tex_num;
int *tex_num2;

int mdlnum;
int solid;	//solid id
};

#endif
