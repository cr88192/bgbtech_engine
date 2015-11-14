#ifndef __MRPT02_H__
#define __MRPT02_H__

typedef struct {
void **framestack;
int *posstack;
int *tagstack;
int stackpos;

int flag;
int rtag;

int rlen;
byte *rbuf, *erbuf;
}MRPT_ConInfo;

#endif
