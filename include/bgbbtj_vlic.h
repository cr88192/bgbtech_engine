#define BTVLIC_BLEND_NORMAL					0
#define BTVLIC_BLEND_ADD					1
#define BTVLIC_BLEND_SUBTRACT				2
#define BTVLIC_BLEND_MULTIPLY				3
#define BTVLIC_BLEND_DIVIDE					4
#define BTVLIC_BLEND_SCREEN					5
#define BTVLIC_BLEND_OVERLAY				6
#define BTVLIC_BLEND_DIFFERENCE				7
#define BTVLIC_BLEND_DARKEN					8
#define BTVLIC_BLEND_LIGHTEN				9
#define BTVLIC_BLEND_COLOR_DODGE			10
#define BTVLIC_BLEND_COLOR_BURN				11
#define BTVLIC_BLEND_INVERT					12

#define BTVLIC_BLEND_ZERO					32
#define BTVLIC_BLEND_ONE					33
#define BTVLIC_BLEND_SRC_COLOR				34
#define BTVLIC_BLEND_DST_COLOR				35
#define BTVLIC_BLEND_SRC_ALPHA				36
#define BTVLIC_BLEND_DST_ALPHA				37
#define BTVLIC_BLEND_ONE_MINUS_SRC_COLOR	38
#define BTVLIC_BLEND_ONE_MINUS_DST_COLOR	39
#define BTVLIC_BLEND_ONE_MINUS_SRC_ALPHA	40
#define BTVLIC_BLEND_ONE_MINUS_DST_ALPHA	41

#define BTVLIC_BLEND_SRC_RED				42
#define BTVLIC_BLEND_DST_RED				43
#define BTVLIC_BLEND_SRC_GREEN				44
#define BTVLIC_BLEND_DST_GREEN				45
#define BTVLIC_BLEND_SRC_BLUE				46
#define BTVLIC_BLEND_DST_BLUE				47
#define BTVLIC_BLEND_ONE_MINUS_SRC_RED		48
#define BTVLIC_BLEND_ONE_MINUS_DST_RED		49
#define BTVLIC_BLEND_ONE_MINUS_SRC_GREEN	50
#define BTVLIC_BLEND_ONE_MINUS_DST_GREEN	51
#define BTVLIC_BLEND_ONE_MINUS_SRC_BLUE		52
#define BTVLIC_BLEND_ONE_MINUS_DST_BLUE		53

typedef struct BTVLIC_PixelValue_s BTVLIC_PixelValue;
typedef struct BTVLIC_PixelValueF_s BTVLIC_PixelValueF;
typedef struct BTVLIC_Mat3x3_s BTVLIC_Mat3x3;

typedef struct BTVLIC_Texture2D_s BTVLIC_Texture2D;

typedef struct BTVLIC_DrawLayer_s BTVLIC_DrawLayer;
typedef struct BTVLIC_DrawContext_s BTVLIC_DrawContext;

struct BTVLIC_PixelValue_s
	{ s16 r, g, b, a; };

struct BTVLIC_PixelValueF_s
	{ float r, g, b, a; };

struct BTVLIC_Mat3x3_s
{
//float v[9];
float v00, v01, v02;
float v10, v11, v12;
float v20, v21, v22;
};

struct BTVLIC_Texture2D_s
{
// int ox, oy;
int xs, ys;
byte npot;
s16 *buf;		//RGBA, Q11.4, Normal Range = 0..255
byte *rgba;

char *name;
BGBBTJ_AVI_Context *avictx;
float startDelay;
float playSpeed;
bool isPaused;
bool reqRestart;
};

struct BTVLIC_DrawLayer_s
{
BTVLIC_Mat3x3 xform_base;	//transform of the layer in world space
BTVLIC_Mat3x3 xform_tex;	//transform of the layer in texture space
// BTVLIC_Mat3x3 xfvel_base;	//transform of the layer in world space
// BTVLIC_Mat3x3 xfvel_tex;	//transform of the layer in texture space
int ox, oy;
int xs, ys;
byte blend;
byte blend2;
byte maskblend;
s16 alpha;
char *name;
BTVLIC_Texture2D *tex;		//image texture
BTVLIC_Texture2D *masktex;	//mask texture

float xftex_xvel, xftex_yvel, xftex_avel;
float xfbase_xvel, xfbase_yvel, xfbase_avel;
};

struct BTVLIC_DrawContext_s
{
BTVLIC_Texture2D *tgt;

BTVLIC_Texture2D *tex[256];
int n_tex;

BTVLIC_DrawLayer *layer[256];
int n_layer;
};
