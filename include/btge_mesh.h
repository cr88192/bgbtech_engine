typedef struct BTGE_MeshPrim_s BTGE_MeshPrim;
typedef struct BTGE_MeshGroup_s BTGE_MeshGroup;

struct BTGE_MeshPrim_s {
BTGE_MeshPrim *next;
BTGE_MeshGroup *up;
BTGE_MeshPrim *bnext;	//next in brush
char *name;
char *tex;
int texnum;

int n_xyz, m_xyz;	//number and max xyz vertices
int n_st, m_st;		//number and max st vertices
int n_pts, m_pts;	//number and max face points
int n_face, m_face;	//number and max faces
int n_ftex, m_ftex;	//number and max face textures

float *xyz;		//[x y z]*
float *st;		//[s t]*
float *norm;	//[x y z]*
int *pts;		//[i_xyz i_st]*
int *face;		//[i_pts n_pts]*

int *pts2;		//[i_xyz i_st i_norm i_rgba]*
int *face2;		//[i_pts n_pts prim i_tex]*
int *ftex2;		//[i_face n_face - -]*
char **ftex2n;

//flags: 1&=sel, 14&=opsel, 128&=del, 64&=hidden
byte *fl_xyz;		//vertex flag mask
byte *fl_st;		//st vertex flag mask
byte *fl_pts;		//face points flag mask
byte *fl_face;		//face flag mask
int fl;			//prim flag mask, &255=same

float tex_org[3];
float tex_sdir[3];
float tex_tdir[3];
int tex_mode;		//texture projection mode
int tex_fl;		//texture flags

//cached render-time stuff
float *norm_face;	//face normals
float *norm_pts;	//face vertex normals
float *norm_xyz;	//vertex normals
int *edge;		//[xyz0 xyz1 face0 face1]*
int n_edge;		//num edges

float com_mins[3];
float com_maxs[3];
float com_org[3];
float com_rad;
};


struct BTGE_MeshGroup_s {
BTGE_MeshGroup *next, *first;
BTGE_MeshPrim *prim;
char *name;
int fl;			//group flags
};
