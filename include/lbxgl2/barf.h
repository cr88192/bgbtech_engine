/*
Big Array Render Fill
 */
 
typedef struct LBXGL_BARF_RenderArray_s LBXGL_BARF_RenderArray;
typedef struct LBXGL_BARF_RenderContext_s LBXGL_BARF_RenderContext;

typedef float lbxgl_barf_float;
typedef u16 lbxgl_barf_hfloat;

typedef float lbxgl_barf_xyzfloat;
// typedef u16 lbxgl_barf_stfloat;
typedef float lbxgl_barf_stfloat;
//typedef u16 lbxgl_barf_nvfloat;
// typedef u16 lbxgl_barf_clrfloat;
typedef u32 lbxgl_barf_nvfloat;
typedef byte lbxgl_barf_clrfloat;

struct LBXGL_BARF_RenderArray_s {
int tex;
int texflags;
int texflags2;

int n_tris, m_tris;
lbxgl_barf_xyzfloat *xyz;		//XYZ
lbxgl_barf_stfloat *st;			//ST
lbxgl_barf_nvfloat *norm;		//XYZ (Plane)
lbxgl_barf_nvfloat *xnorm;		//XYZ (S-Vector)
lbxgl_barf_nvfloat *ynorm;		//XYZ (T-Vector)
lbxgl_barf_clrfloat *rgba;		//RGBA (base color)
lbxgl_barf_clrfloat *srgba;		//RGBA (sun-light)
lbxgl_barf_clrfloat *vrgba;		//RGBA (voxel-light)
lbxgl_barf_clrfloat *trgba;		//RGBA (temp-light, combined)

//VBOs
int vbo;			//VBO ID
int sz_vbo;			//VBO size

//solid face VBO
int vbo_xyz;		//XYZ
int vbo_st;			//ST
int vbo_norm;		//normals (XYZ)
int vbo_xnorm;		//X normals (XYZ)
int vbo_ynorm;		//Y normals (XYZ)
int vbo_rgba;		//base color (RGBA)
int vbo_srgba;		//sun-light color (RGBA)
int vbo_vrgba;		//vox-light color (RGBA)
int vbo_trgba;		//temp light color (RGBA)
};

struct LBXGL_BARF_RenderContext_s {
int n_array, m_array;
LBXGL_BARF_RenderArray **array;
};
