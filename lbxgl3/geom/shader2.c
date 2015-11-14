#include <lbxgl2.h>

LBXGL_Shader2_Context *lbxgl_shader2_ctx;
int lbxgl_shader2_init=0;

LBXGL_Shader2_RenderMode *lbxgl_shader2_oldsdr;
LBXGL_Shader2_RenderMode *lbxgl_shader2_oldsdr_flat;

static void lbxgl_oldsdr_bind(LBXGL_Shader2_Context *ctx, int tex)
	{ LBXGL_Shader_BindTexture(tex); }
static void lbxgl_oldsdr_begin(LBXGL_Shader2_Context *ctx, int prim)
	{ LBXGL_Shader_Begin(prim); }
static void lbxgl_oldsdr_end(LBXGL_Shader2_Context *ctx)
	{ LBXGL_Shader_End(); }
static void lbxgl_oldsdr_end_flat(LBXGL_Shader2_Context *ctx)
	{ LBXGL_Shader_EndFlat(); }
static void lbxgl_oldsdr_vertex3fv(LBXGL_Shader2_Context *ctx, float *fv)
	{ LBXGL_Shader_Vertex3fv(fv); }
static void lbxgl_oldsdr_normal3fv(LBXGL_Shader2_Context *ctx, float *fv)
	{ LBXGL_Shader_Normal3fv(fv); }
static void lbxgl_oldsdr_texCoord2fv(LBXGL_Shader2_Context *ctx, float *fv)
	{ LBXGL_Shader_TexCoord2fv(fv); }
static void lbxgl_oldsdr_color3fv(LBXGL_Shader2_Context *ctx, float *fv)
	{ LBXGL_Shader_Color3fv(fv); }
static void lbxgl_oldsdr_color4fv(LBXGL_Shader2_Context *ctx, float *fv)
	{ LBXGL_Shader_Color4fv(fv); }

LBXGL_API void LBXGL_Shader2_Init()
{
	int i;

	if(lbxgl_shader2_init)return;
	lbxgl_shader2_init=1;
	
	LBXGL_Shader_Init();

	lbxgl_shader2_ctx=gcalloc(sizeof(LBXGL_Shader2_Context));

	i=256;
	lbxgl_shader2_ctx->vec_xyz=gcatomic(i*4*sizeof(float));
	lbxgl_shader2_ctx->vec_st=gcatomic(i*2*sizeof(float));
	lbxgl_shader2_ctx->vec_norm=gcatomic(i*4*sizeof(float));
	lbxgl_shader2_ctx->vec_clr=gcatomic(i*4*sizeof(float));
	lbxgl_shader2_ctx->m_vec=i;
	
	lbxgl_shader2_oldsdr=gcalloc(sizeof(LBXGL_Shader2_RenderMode));
	lbxgl_shader2_oldsdr->bind=lbxgl_oldsdr_bind;
	lbxgl_shader2_oldsdr->begin=lbxgl_oldsdr_begin;
	lbxgl_shader2_oldsdr->end=lbxgl_oldsdr_end;
	lbxgl_shader2_oldsdr->vertex3fv=lbxgl_oldsdr_vertex3fv;
	lbxgl_shader2_oldsdr->normal3fv=lbxgl_oldsdr_normal3fv;
	lbxgl_shader2_oldsdr->texCoord2fv=lbxgl_oldsdr_texCoord2fv;
	lbxgl_shader2_oldsdr->color3fv=lbxgl_oldsdr_color3fv;
	lbxgl_shader2_oldsdr->color4fv=lbxgl_oldsdr_color4fv;

	lbxgl_shader2_oldsdr_flat=gcalloc(sizeof(LBXGL_Shader2_RenderMode));
	*lbxgl_shader2_oldsdr_flat=*lbxgl_shader2_oldsdr;
	lbxgl_shader2_oldsdr_flat->end=lbxgl_oldsdr_end_flat;
	
	
	lbxgl_shader2_ctx->mode=lbxgl_shader2_oldsdr;
}

LBXGL_API void lbxShaderSetMode(int mode)
{
	switch(mode)
	{
	case LBXGL_SHADER2_FLAT:
		lbxgl_shader2_ctx->mode=lbxgl_shader2_oldsdr_flat; break;
	case LBXGL_SHADER2_DARK:
		lbxgl_shader2_ctx->mode=lbxgl_shader2_oldsdr_flat; break;
	case LBXGL_SHADER2_LIGHT:
		lbxgl_shader2_ctx->mode=lbxgl_shader2_oldsdr_flat; break;
	case LBXGL_SHADER2_SHADOW:
		lbxgl_shader2_ctx->mode=lbxgl_shader2_oldsdr_flat; break;

	case LBXGL_SHADER2_FINAL:
		lbxgl_shader2_ctx->mode=lbxgl_shader2_oldsdr; break;
	case LBXGL_SHADER2_BASIC:
		lbxgl_shader2_ctx->mode=lbxgl_shader2_oldsdr; break;

	default:
		lbxgl_shader2_ctx->mode=lbxgl_shader2_oldsdr; break;
	}
}

LBXGL_API void lbxBindTexture(int tex)
	{ lbxgl_shader2_ctx->mode->bind(lbxgl_shader2_ctx, tex); }

LBXGL_API void lbxBegin(int prim)
	{ lbxgl_shader2_ctx->mode->begin(lbxgl_shader2_ctx, prim); }
LBXGL_API void lbxEnd(void)
	{ lbxgl_shader2_ctx->mode->end(lbxgl_shader2_ctx); }

LBXGL_API void lbxVertex3fv(float *fv)
	{ lbxgl_shader2_ctx->mode->vertex3fv(lbxgl_shader2_ctx, fv); }
LBXGL_API void lbxNormal3fv(float *fv)
	{ lbxgl_shader2_ctx->mode->normal3fv(lbxgl_shader2_ctx, fv); }
LBXGL_API void lbxTexCoord2fv(float *fv)
	{ lbxgl_shader2_ctx->mode->texCoord2fv(lbxgl_shader2_ctx, fv); }

LBXGL_API void lbxColor3fv(float *fv)
	{ lbxgl_shader2_ctx->mode->color3fv(lbxgl_shader2_ctx, fv); }
LBXGL_API void lbxColor4fv(float *fv)
	{ lbxgl_shader2_ctx->mode->color4fv(lbxgl_shader2_ctx, fv); }

LBXGL_API void lbxColor3f(float r, float g, float b)
	{ float tv[4]; tv[0]=r; tv[1]=g; tv[2]=b; lbxColor3fv(tv); }
LBXGL_API void lbxColor4f(float r, float g, float b, float a)
	{ float tv[4]; tv[0]=r; tv[1]=g; tv[2]=b; tv[3]=a; lbxColor4fv(tv); }
LBXGL_API void lbxTexCoord2f(float s, float t)
	{ float tv[2]; tv[0]=s; tv[1]=t; lbxTexCoord2fv(tv); }
LBXGL_API void lbxVertex3f(float x, float y, float z)
	{ float tv[3]; tv[0]=x; tv[1]=y; tv[2]=z; lbxVertex3fv(tv); }
LBXGL_API void lbxNormal3f(float x, float y, float z)
	{ float tv[3]; tv[0]=x; tv[1]=y; tv[2]=z; lbxNormal3fv(tv); }
