#ifndef MATNF_H
#define MATNF_H

#include <bsde_vecnf.h>

typedef struct {
float v[4];
}mat2;

typedef struct {
float v[12];
}mat3;

typedef struct {
float v[16];
}mat4;


#define mat2vf(v)	dtyMat2vf(v)
#define mat3vf(v)	dtyMat3vf(v)
#define mat4vf(v)	dtyMat4vf(v)

#define mat2(a,b,c,d)	dtyMat2f(a,b,c,d)
#define mat3(a,b,c,d, e,f,g,h, i) \
	dtyMat3f(a,b,c,d, e,f,g,h, i)
#define mat4(a,b,c,d, e,f,g,h, i,j,k,l, m,n,o,p) \
	dtyMat4f(a,b,c,d, e,f,g,h, i,j,k,l, m,n,o,p)


#endif
