#ifndef BGBBTJ_H
#define BGBBTJ_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <stdarg.h>
#include <math.h>

#if !defined(BGBBTJ_DRV) && !defined(BGBBTJ_NODY)
#include <bgbdy.h>
#else

#ifndef BGBBTJ_NODY
#include <bgbdy.h>
#endif

#ifdef printf
#undef printf
#endif

#ifdef BGBBTJ_DRV
#define printf	btjpg_printf
#endif

#define VFILE		FILE
#define vffopen		fopen
#define vfclose		fclose
#define vfread		fread
#define vfwrite		fwrite
#define vfseek		fseek
#define vftell		ftell
#define vfgets		fgets
#define vfeof		feof
#define vfgetc		fgetc
#define vfputc		fputc

#undef gcalloc
#undef gctalloc
#undef gcatomic
#undef gctatomic

#define gcalloc		btjpg_gcalloc
#define gctalloc	btjpg_gctalloc
#define gcatomic	btjpg_gcalloc
#define gctatomic	btjpg_gctalloc
#define gcfree		btjpg_gcfree

#define dystrdup	btjpg_strdup
#define dyllStrdup	btjpg_strdup

#ifndef min
#define min(x, y) (((x)<(y))?(x):(y))
#endif

#ifndef max
#define max(x, y) (((x)>(y))?(x):(y))
#endif

#endif

#include <bgbbtj_conf.h>

#ifndef BYTE_T
#define BYTE_T
typedef unsigned char byte;
#endif

#ifndef PDLIB_INT_BITS_T
#define PDLIB_INT_BITS_T
typedef unsigned short u16;
typedef signed short s16;
typedef unsigned int u32;
typedef signed int s32;
typedef unsigned long long u64;
typedef signed long long s64;
#endif

// typedef unsigned short u16;
// typedef unsigned int u32;

typedef unsigned short ushort;
typedef unsigned int uint;

typedef struct { byte v[2]; } btj_word;
typedef struct { byte v[2]; } btj_word_ube;
typedef struct { byte v[2]; } btj_word_sbe;
typedef struct { byte v[2]; } btj_word_ule;
typedef struct { byte v[2]; } btj_word_sle;

typedef struct { byte v[4]; } btj_dword;
typedef struct { byte v[4]; } btj_dword_ube;
typedef struct { byte v[4]; } btj_dword_sbe;
typedef struct { byte v[4]; } btj_dword_ule;
typedef struct { byte v[4]; } btj_dword_sle;

#define BGBBTJ_ZFL_ESCAPE_FF	1
#define BGBBTJ_ZFL_DEFLATE64	2
#define BGBBTJ_ZFL_ESCBTLZA		4
#define BGBBTJ_ZFL_ESCBTLZH		8
#define BGBBTJ_ZFL_ISBTLZH		16
#define BGBBTJ_ZFL_ARITH		32

// typedef signed short s16;
// typedef signed int s32;

#define	CONTENTS_SOLID			1		// an eye is never valid in a solid
#define	CONTENTS_WINDOW			2		// translucent, but not watery
#define	CONTENTS_AUX			4
#define	CONTENTS_LAVA			8
#define	CONTENTS_SLIME			16
#define	CONTENTS_WATER			32
#define	CONTENTS_MIST			64
#define	LAST_VISIBLE_CONTENTS	64

#define	CONTENTS_AREAPORTAL		0x8000

#define	CONTENTS_PLAYERCLIP		0x10000
#define	CONTENTS_MONSTERCLIP	0x20000

// currents can be added to any other contents, and may be mixed
#define	CONTENTS_CURRENT_0		0x40000
#define	CONTENTS_CURRENT_90		0x80000
#define	CONTENTS_CURRENT_180	0x100000
#define	CONTENTS_CURRENT_270	0x200000
#define	CONTENTS_CURRENT_UP		0x400000
#define	CONTENTS_CURRENT_DOWN	0x800000

#define	CONTENTS_ORIGIN			0x1000000	// removed before bsping an entity

#define	CONTENTS_MONSTER		0x2000000	// should never be on a brush, only in game
#define	CONTENTS_DEADMONSTER	0x4000000
#define	CONTENTS_DETAIL			0x8000000	// brushes to be added after vis leafs
#define	CONTENTS_TRANSLUCENT	0x10000000	// auto set if any surface has trans
#define	CONTENTS_LADDER			0x20000000



#define	SURF_LIGHT		0x1		// value will hold the light strength

#define	SURF_SLICK		0x2		// effects game physics

#define	SURF_SKY		0x4		// don't draw, but add to skybox
#define	SURF_WARP		0x8		// turbulent water warp
#define	SURF_TRANS33	0x10
#define	SURF_TRANS66	0x20
#define	SURF_FLOWING	0x40	// scroll towards angle
#define	SURF_NODRAW		0x80	// don't bother referencing the texture

#define	SURF_HINT		0x100	// make a primary bsp splitter
#define	SURF_SKIP		0x200	// completely ignore, allowing non-closed brushes

//BGB: added, match Q3 values
#define	SURF_HINT		0x100	// make a primary bsp splitter
#define	SURF_SKIP		0x200	// completely ignore, allowing non-closed brushes
#define	SURF_NOLIGHTMAP		0x400	// surface doesn't need a lightmap
#define	SURF_POINTLIGHT		0x800	// generate lighting info at vertexes
#define	SURF_METALSTEPS		0x1000	// clanking footsteps
#define	SURF_NOSTEPS		0x2000	// no footstep sounds
#define	SURF_NONSOLID		0x4000	// don't collide against curves with this set
#define SURF_LIGHTFILTER	0x8000	// act as a light filter during q3map -light
#define	SURF_ALPHASHADOW	0x10000	// do per-pixel light shadow casting in q3map
#define	SURF_NODLIGHT		0x20000	// never add dynamic lights

//BGB: remap, from Q3
#define	SURF_NODAMAGE		0x040000	// never give falling damage
#define	SURF_LADDER		0x080000
#define	SURF_NOIMPACT		0x100000	// don't make missile explosions
#define	SURF_NOMARKS		0x200000	// don't leave missile marks
#define	SURF_FLESH		0x400000	// make flesh sounds and effects

//BGB: custom flags
#define	SURF2_GLOSS		0x00000001	// make glossy


typedef struct
{
char		name[32];	//0
unsigned int	width;		//32
unsigned int	height;		//36
unsigned int	offsets[4];	//40, four mip maps stored
char		animname[32];	//56, next frame in animation chain
int		flags;		//88
int		contents;	//92
int		value;		//96
//100
} miptex_t;

typedef struct
{
char	*name;
int	width, height;
byte	*data[4];		// four mip maps stored
char	*animname;		// next frame in animation chain
int	flags;
int	contents;
int	value;
byte	*pal;
byte	**frames;
int		*frames_sz;
int n_frames;
}WalInfo_t;

typedef struct
{
char	name[32];	//0
int		width;		//32
int		height;		//36
int		offset;		//40
int 	size;		//44
int 	fcc;		//48
int		flags;		//52
int		contents;	//56
int		value;		//60
//64
} AwalTex_t;

typedef struct {
byte *cs, *cse;
byte *ct, *cte;
u32 win;
int pos;
}BGBBTJ_BitStream;

// #define VFILE		FILE
// #define vffopen		fopen
// #define vfread		fread
// #define vfwrite		fwrite
// #define vfseek		fseek
// #define vftell		ftell
// #define vfclose		fclose

// #define gcalloc		pdgl_avi_malloc
// #define gcfree		free

typedef struct BTIC2C_Context_s BTIC2C_Context;

#include <bgbbtj_vecnf.h>

#include <bgbbtj_jpeg.h>
#include <bgbbtj_avi.h>

#include <bgbbtj_vid_codec.h>
#include <bgbbtj_vid_avi.h>

#include <bgbbtj_btic1.h>
#include <bgbbtj_btic1c.h>
#include <bgbbtj_btic2b.h>
#include <bgbbtj_btic2c.h>

#include <bgbbtj_btlza.h>

#include <bgbbtj_vlic.h>
#include <bgbbtj_nncube.h>

#include <bgbbtj_auto.h>

#endif
