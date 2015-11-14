#include <stdio.h>
#include <stdlib.h>
#include <string.h>
// #include <stdint.h>
#include <math.h>

#include <bgbgc.h>
#include <bgbdy.h>

#if defined(ANDROID)||defined(NATIVECLIENT)
#define GLES		//GLES, generic
#define GLES2		//GLES 2.0
#endif	//ANDROID||NATIVECLIENT

#ifndef _BGBMETA	//skip for metadata tool

#if defined(WIN32) || defined(_WIN32)
#include <windows.h>
#endif

#ifdef GLES2
#include <GLES2/gl2.h>
#include <GLES2/gl2ext.h>
#else
#include <GL/gl.h>
// #include <GL_2/glext.h>
#include <GL_3/glext.h>
#endif

#endif	//BGBMETA

#include <libvecmath.h>
#include <btgecm.h>

#ifndef UINT_T
#define UINT_T
typedef unsigned int uint;
#endif

#ifndef USHORT_T
#define USHORT_T
typedef unsigned short ushort;
#endif

#ifndef M_PI
#define M_PI		3.1415926535897932384626433832795
#endif

#ifndef PDGL_H
#define PDGL_H

#ifdef __cplusplus
extern "C" {
#endif

#define PDGL_FONT_BOLD			1
#define PDGL_FONT_ITALIC		2
#define PDGL_FONT_UNDERLINE		4
#define PDGL_FONT_OVERLINE		8
#define PDGL_FONT_STRIKEOUT		16
#define PDGL_FONT_BLINK			32
#define PDGL_FONT_SUBSCRIPT		64
#define PDGL_FONT_SUPERSCRIPT	128

//WARNING: These constants are magic (Align with those in OpenGL)
#define PDGL_POINTS						0
#define PDGL_LINES						1
#define PDGL_LINE_LOOP					2
#define PDGL_LINE_STRIP					3
#define PDGL_TRIANGLES					4
#define PDGL_TRIANGLE_STRIP				5
#define PDGL_TRIANGLE_FAN				6
#define PDGL_QUADS						7
#define PDGL_QUAD_STRIP					8
#define PDGL_POLYGON					9


#include <pdgl/pdgl_conf.h>

#include <pdgl/keys.h>
#include <pdgl/sound.h>
#include <pdgl/jpeg.h>

#include <pdgl/pdgl_avi.h>
#include <pdgl/pdgl_font.h>

#include <pdgl/auto.h>
#include <pdgl/tyauto.h>

#include <pdgl/global.h>


#ifdef __cplusplus
}
#endif

#endif
