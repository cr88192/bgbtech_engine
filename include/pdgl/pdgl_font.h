typedef struct PDGL_FontFrag_s PDGL_FontFrag;
typedef struct PDGL_FontInfo_s PDGL_FontInfo;

#ifdef PDGL_DLL
typedef struct PDGL_FontFrag_s fontfrag;
typedef struct PDGL_FontInfo_s fontinfo;
#endif

struct PDGL_FontFrag_s {
PDGL_FontFrag *next, *prev; //in parent font
PDGL_FontInfo *parent;
int base;	//base character

// byte usemap[8];		//cells which have been drawn
// byte badmap[8];		//cells which are not valid

u16 usemap[16];		//cells which have been drawn
u16 badmap[16];		//cells which are not valid
byte *buffer;
int texnum;
};

struct PDGL_FontInfo_s {
PDGL_FontInfo *next;	//next in the list of fonts
PDGL_FontInfo *chain;	//links for font fragments
char *name;

byte *buf;
int w, h;
int s, e;
PDGL_FontFrag *frag;

PDGL_FontInfo *alts[16];	//alternate fonts to check
int n_alts;
};


typedef struct {
byte cur_clr[4];
float cur_st[2];
int cur_tex;
int cur_prim;
int cur_basexyz;

float *xyz;		//XYZ vertex coords
float *st;		//ST vertex coords
byte *rgba;		//RGBA vertex color
int n_xyz;		//num vertices
int m_xyz;		//max vertices

int ofs_xyz;	//XYZ offset
int ofs_st;		//ST offset
int ofs_rgba;	//RGBA offset
int ofs_end;	//offset of end
int sz_xyz;		//XYZ offset
int sz_st;		//ST offset
int sz_rgba;	//RGBA offset

int vbo;		//VBO number
byte *vbo_buf;	//VBO buffer
int sz_vbo;		//size of VBO

int *prim;		//(start, count, prim, tex)
int n_prim;
int m_prim;
}PDGL_TextVBO;
