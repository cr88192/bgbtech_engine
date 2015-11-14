#ifndef MATNF_H
#define MATNF_H

#include <vecnf.h>
#include <bgbdy.h>

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

#define m2add(a,b)		dtyMat2Add(a,b)
#define m3add(a,b)		dtyMat3Add(a,b)
#define m4add(a,b)		dtyMat4Add(a,b)

#define m2sub(a,b)		dtyMat2Sub(a,b)
#define m3sub(a,b)		dtyMat3Sub(a,b)
#define m4sub(a,b)		dtyMat4Sub(a,b)

#define m2mul(a,b)		dtyMat2Mul(a,b)
#define m3mul(a,b)		dtyMat3Mul(a,b)
#define m4mul(a,b)		dtyMat4Mul(a,b)

#define m2inv(a)		dtyMat2Inv(a)
#define m3inv(a)		dtyMat3Inv(a)
#define m4inv(a)		dtyMat4Inv(a)

#define m2trans(a)		dtyMat2Trans(a)
#define m3trans(a)		dtyMat3Trans(a)
#define m4trans(a)		dtyMat4Trans(a)

#define m2mulI(a,b)		dtyMat2MulI(a,b)
#define m3mulI(a,b)		dtyMat3MulI(a,b)
#define m4mulI(a,b)		dtyMat4MulI(a,b)

#define m2mulT(a,b)		dtyMat2MulT(a,b)
#define m3mulT(a,b)		dtyMat3MulT(a,b)
#define m4mulT(a,b)		dtyMat4MulT(a,b)

#endif
