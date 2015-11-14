#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef unsigned char byte;
typedef unsigned short u16;
typedef unsigned int u32;
typedef unsigned long long u64;


typedef struct CipherCtx_s CipherCtx;

struct CipherCtx_s {
byte order[256];
byte order2[256];
u64 seed;
};

void Cipher_UpdateChar(CipherCtx *ctx, byte v)
{
	byte cv, v1;
	
	cv=ctx->order[v];
	v1=(ctx->seed>>56)&255;
	ctx->order[v]=ctx->order[v1];
	ctx->order[v1]=cv;
	ctx->order2[ctx->order[v]]=v;
	ctx->order2[ctx->order[v1]]=v1;
	ctx->seed=ctx->seed*72057594037927931LL+v;
}

byte Cipher_EncodeChar(CipherCtx *ctx, byte pv)
{
	byte cv, v1;
	
	cv=ctx->order[pv];
	Cipher_UpdateChar(ctx, pv);
	return(cv);
}

byte Cipher_DecodeChar(CipherCtx *ctx, byte cv)
{
	byte pv, v1;
	
	pv=ctx->order2[cv];
	Cipher_UpdateChar(ctx, pv);
	return(pv);
}

void Cipher_InitKey(CipherCtx *ctx, byte *key, int sz)
{
	int i, j;

	for(i=0; i<256; i++)
	{
		ctx->order[i]=i;
		ctx->order2[i]=i;
	}
	ctx->seed=0;

	if(!sz)return;
	for(i=0; i<4096; i++)
		Cipher_UpdateChar(ctx, key[i%sz]);
}

void Cipher_EncodeBuffer(CipherCtx *ctx, byte *obuf, byte *ibuf, int sz)
{
	int i;	
	for(i=0; i<sz; i++)
		obuf[i]=Cipher_EncodeChar(ctx, ibuf[i]);
}

void Cipher_DecodeBuffer(CipherCtx *ctx, byte *obuf, byte *ibuf, int sz)
{
	int i;	
	for(i=0; i<sz; i++)
		obuf[i]=Cipher_DecodeChar(ctx, ibuf[i]);
}

void Cipher_Dump(CipherCtx *ctx)
{
	int i, j, k, l;
	
	for(i=0; i<16; i++)
	{
		for(j=0; j<16; j++)
		{
			k=ctx->order[i*16+j];
			l=ctx->order2[k];
			printf("%02X ", k);
			if(l!=(i*16+j))
				printf("!");
		}
		printf("\n");
	}

	printf("\n");

	for(i=0; i<16; i++)
	{
		for(j=0; j<16; j++)
		{
			printf("%02X ", ctx->order2[i*16+j]);
		}
		printf("\n");
	}

	printf("%08X:%08X\n", (u32)(ctx->seed>>32), (u32)ctx->seed);
}

void *readfile(char *name, int *rsz)
{
	FILE *fd;
	void *buf;
	int sz;

	fd=fopen(name, "rb");
	if(!fd)return(NULL);
	
	fseek(fd, 0, 2);
	sz=ftell(fd);

	buf=malloc(sz);
	fseek(fd, 0, 0);
	fread(buf, 1, sz, fd);
	fclose(fd);
	
	if(rsz)*rsz=sz;
	return(buf);
}

u32 Cipher_HashName(char *name)
{
	char *s;
	u32 h;
	
	s=name; h=0;
	while(*s)h=(h*251)+(*s++);
	return(h);
}

int Cipher_EncodeFile(char *name, char *key)
{
	char tb[256];
	CipherCtx ctx;
	FILE *fd;
	byte *ibuf;
	byte *obuf;
	u32 h;
	int sz;
	
	ibuf=readfile(name, &sz);
	if(!ibuf)return(-1);
	h=Cipher_HashName(name);

	obuf=malloc(sz);

	strcpy(tb, name);
	if(key)strcat(tb, key);
	Cipher_InitKey(&ctx, tb, strlen(tb));
	Cipher_EncodeBuffer(&ctx, obuf, ibuf, sz);

	sprintf(tb, "%08X.dat", h);
	fd=fopen(tb, "wb");
	fwrite(obuf, 1, sz, fd);
	fclose(fd);
	free(ibuf);
	free(obuf);
	
	printf("%s -> %s\n", name, tb);
}

int main(int argc, char *argv[])
{
	byte buf0[256];
	byte buf1[256];
	CipherCtx ctx;
	char *key, *msg;
	int i, sz;

	msg="Some Encodable Test Message";

	key="Test Key 0";
	Cipher_InitKey(&ctx, key, strlen(key));
	Cipher_Dump(&ctx);

	sz=strlen(msg);

	for(i=0; i<sz; i++)
		printf("%02X ", msg[i]);
	printf("\n");

	Cipher_EncodeBuffer(&ctx, buf0, msg, sz);

	for(i=0; i<sz; i++)
		printf("%02X ", buf0[i]);
	printf("\n");

	Cipher_InitKey(&ctx, key, strlen(key));
	Cipher_DecodeBuffer(&ctx, buf1, buf0, sz);

	for(i=0; i<sz; i++)
		printf("%02X ", buf1[i]);
	printf("\n");

	for(i=1; i<argc; i++)
		Cipher_EncodeFile(argv[i], key);
	return(0);
}

