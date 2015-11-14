#ifndef __MRPT021_H__
#define __MRPT021_H__

typedef struct MRPT_ConInfo_s MRPT_ConInfo;
typedef struct MRPT_TLV_s MRPT_TLV;

struct MRPT_ConInfo_s {
int flag;

int rtag; //8.24
int rlen;
byte *rbuf, *erbuf;
};

struct MRPT_TLV_s {
MRPT_TLV *next;

int tag; //8.24
MRPT_TLV *first;

int len;
byte *data;
};

#endif
