#ifndef __MRPT_022_H__
#define __MRPT_022_H__

#define MRPT_MAX_TAGS		1024

#define MRPT_FLAG_COMPOUND	1
#define MRPT_FLAG_FIXEDLENGTH	2

#define MRPT_CLASS_DEFAULT	0
#define MRPT_CLASS_RAW		0
#define MRPT_CLASS_STRING	1
#define MRPT_CLASS_INTEGER	2
#define MRPT_CLASS_FLOAT	3

typedef struct MRPT_TagInfo_s MRPT_TagInfo;
typedef struct MRPT_ConInfo_s MRPT_ConInfo;
typedef struct MRPT_TLV_s MRPT_TLV;

struct MRPT_TagInfo_s {
char *name;

short flags;
short tag;
byte len;
byte class;
};

struct MRPT_ConInfo_s {
MRPT_TagInfo **tags;

int flags;

int rtag;
int rlen;
byte *rbuf, *erbuf;
};

struct MRPT_TLV_s {
MRPT_TLV *next;

MRPT_TagInfo *tag;
MRPT_TLV *first;

int len;
byte *data;
};

#endif
