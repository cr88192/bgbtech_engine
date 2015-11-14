#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <math.h>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

#ifndef M_E
#define M_E 2.7182818284590452354
#endif

#ifndef BGBTAC_H
#define BGBTAC_H

#include <bgbtac_conf.h>

#define BGBMID_SFL_16BIT		1
#define BGBMID_SFL_UNSIGNED		2
#define BGBMID_SFL_LOOP			4
#define BGBMID_SFL_BIDIRLOOP		8
#define BGBMID_SFL_REVERSE		16
#define BGBMID_SFL_ENVELOPESUST		32
#define BGBMID_SFL_ENVELOPEFILT		64

#define BGBTAC_EFL_STEREO		1		//samples are stereo
#define BGBTAC_EFL_CENTERMONO	2		//encode as centered-mono
#define BGBTAC_EFL_HALFMONO		4		//dealing with a single channel
#define BGBTAC_EFL_MIDSTREAM	8		//we are in the middle-part of a stream

#define BGBMID_FCC_MThd	0x4D546864
#define BGBMID_FCC_MTrk	0x4D54726B

#ifndef BYTE_T
#define BYTE_T
typedef unsigned char byte;
#endif

#ifndef U8_T
#define U8_T
typedef unsigned char u8;
#endif

#ifndef S8_T
#define S8_T
typedef signed char s8;
#endif

typedef struct BTAC_SampleHeader_s BTAC_SampleHeader;

struct BTAC_SampleHeader_s
{
byte chan;
byte bits;
byte blk_size;		//log2 block-size (bytes)
byte blk_samples;	//log2 of block-samples
u32 rate;
u32 length;
u32 flags;
u32 data_offs;

byte *data;			//block data
};

#ifdef __cplusplus
extern "C" {
#endif

#include <bgbtac_auto.h>


#ifdef __cplusplus
}
#endif

#endif
