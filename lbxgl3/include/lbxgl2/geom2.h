#ifndef LBXGL_GEOM2_H
#define LBXGL_GEOM2_H

#define LBXGL_SHADER2_FLAT		0x1000
#define LBXGL_SHADER2_DARK		0x1001
#define LBXGL_SHADER2_LIGHT		0x1002
#define LBXGL_SHADER2_SHADOW	0x1003
#define LBXGL_SHADER2_FINAL		0x1004
#define LBXGL_SHADER2_BASIC		0x1005

typedef struct LBXGL_Shader2_Context_s LBXGL_Shader2_Context;
typedef struct LBXGL_Shader2_RenderMode_s LBXGL_Shader2_RenderMode;
typedef struct LBXGL_Shader2_RenderShader_s LBXGL_Shader2_RenderShader;

struct LBXGL_Shader2_Context_s {

LBXGL_Shader2_RenderMode *mode;		//rendering mode
LBXGL_Shader2_RenderShader *draw;	//shader draw methods

LBXGL_TexImage *texture;			//bound texture
LBXGL_Light *light;					//active light source

float tmp_st[2];
float tmp_color[4];
float tmp_normal[4];

float *vec_xyz;
float *vec_st;
float *vec_norm;
float *vec_clr;
int n_vec;
int m_vec;
};

struct LBXGL_Shader2_RenderMode_s {
void (*bind)(LBXGL_Shader2_Context *ctx, int tex);
void (*begin)(LBXGL_Shader2_Context *ctx, int prim);
void (*end)(LBXGL_Shader2_Context *ctx);
void (*vertex3fv)(LBXGL_Shader2_Context *ctx, float *fv);
void (*normal3fv)(LBXGL_Shader2_Context *ctx, float *fv);
void (*texCoord2fv)(LBXGL_Shader2_Context *ctx, float *fv);
void (*color3fv)(LBXGL_Shader2_Context *ctx, float *fv);
void (*color4fv)(LBXGL_Shader2_Context *ctx, float *fv);
};

struct LBXGL_Shader2_RenderShader_s {
void (*draw_flat)(LBXGL_Shader2_Context *ctx);
void (*draw_dark)(LBXGL_Shader2_Context *ctx);
void (*draw_light)(LBXGL_Shader2_Context *ctx);
void (*draw_shadow)(LBXGL_Shader2_Context *ctx, float *org, float rad);
void (*draw_final)(LBXGL_Shader2_Context *ctx);
void (*draw_basic)(LBXGL_Shader2_Context *ctx);
};

#endif
