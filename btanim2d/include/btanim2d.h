#include <bgbdy.h>
#include <pdgl.h>
#include <pdglui.h>

#ifndef BTANIM2D_H
#define BTANIM2D_H

#define BTA2D_TOOL_SELECT		1
#define BTA2D_TOOL_MOVEPIXELS	2
#define BTA2D_TOOL_MOVESELECT	3
#define BTA2D_TOOL_PENCIL		4
#define BTA2D_TOOL_PAINTBRUSH	5
#define BTA2D_TOOL_BUCKET		6
#define BTA2D_TOOL_ERASER		7


typedef struct BTA2D_EditTool_s BTA2D_EditTool;
typedef struct BTA2D_EditLayer_s BTA2D_EditLayer;

struct BTA2D_EditTool_s
{
// int ox, oy;
// int xs, ys;
int id;
int tex;
char *img;
};

struct BTA2D_EditLayer_s
{
int ox, oy;
int xs, ys;
s16 *buf;
};

#include <bta2d_conf.h>
#include <bta2d_auto.h>

#endif
