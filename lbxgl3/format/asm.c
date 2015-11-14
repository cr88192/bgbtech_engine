#include <lbxgl2.h>
#include <bgbasm.h>

int lbxgl_asm_sse;

int LBXGL_ASM_Init()
{
	static int init;
	void *p;
	void (*fcn)();
	int i;

	if(init)return(0);
	init=1;

	BASM_Init();

#if 0

	LBXGL_ASM_RegisterPtr("kprint", &kprint);
	LBXGL_ASM_RegisterPtr("kalloc", &kalloc);
	LBXGL_ASM_RegisterPtr("kfree", &kfree);
	LBXGL_ASM_RegisterPtr("kralloc", &kralloc);

	lbxgl_asm_sse=0;

	LBXGL_ASM_LoadFile("asm/test.as");
	p=LBXGL_ASM_LookupPtr("test");
	if(p)
	{
		fcn=p;
		fcn();

		i=LBXGL_ASM_GetVarIntIdx("cpuid_std", 3);
		if(i&0x02000000)lbxgl_asm_sse=1;
		if(i&0x04000000)lbxgl_asm_sse=2;

		kprint("SSE %d\n", lbxgl_asm_sse);
	}
#endif

//	*(int *)-1=-1;
	return(0);
}

int LBXGL_ASM_LoadFile(char *name)
{
	VFILE *fd;
	char *buf;
	int sz;

	LBXGL_ASM_Init();

	printf("Attempt Open ASM %s\n", name);

	fd=vffopen(name, "rb");
	if(!fd)return(-1);

	vfseek(fd, 0, 2);
	sz=vftell(fd);

	vfseek(fd, 0, 0);
	buf=gcatomic(sz+1);
	vfread(buf, 1, sz, fd);
	buf[sz]=0;

	BASM_BeginAssembly(NULL);
//	asm_print_raw(buf);
	basm_print("%s", buf);
	BASM_EndAssembly();

	gcfree(buf);

	printf("Loaded ASM %s\n", name);

	return(0);
}

void LBXGL_ASM_RegisterPtr(char *name, void *ptr)
{
	BASM_RegisterPtr(name, ptr);
}

void *LBXGL_ASM_LookupPtr(char *name)
{
	void *p;
	p=BASM_GetPtr(name);
	return(p);
}


int LBXGL_ASM_GetVarByteIdx(char *name, int i)
	{ byte *p; p=LBXGL_ASM_LookupPtr(name); if(!p)return(0); return(p[i]); }
int LBXGL_ASM_GetVarShortIdx(char *name, int i)
	{ short *p; p=LBXGL_ASM_LookupPtr(name); if(!p)return(0); return(p[i]); }
int LBXGL_ASM_GetVarIntIdx(char *name, int i)
	{ int *p; p=LBXGL_ASM_LookupPtr(name); if(!p)return(0); return(p[i]); }

void LBXGL_ASM_SetVarByteIdx(char *name, int i, int val)
	{ byte *p; p=LBXGL_ASM_LookupPtr(name); if(p)p[i]=val; }
void LBXGL_ASM_SetVarShortIdx(char *name, int i, int val)
	{ short *p; p=LBXGL_ASM_LookupPtr(name); if(p)p[i]=val; }
void LBXGL_ASM_SetVarIntIdx(char *name, int i, int val)
	{ int *p; p=LBXGL_ASM_LookupPtr(name); if(p)p[i]=val; }

int LBXGL_ASM_GetVarByte(char *name)
	{ return(LBXGL_ASM_GetVarByteIdx(name, 0)); }
int LBXGL_ASM_GetVarShort(char *name)
	{ return(LBXGL_ASM_GetVarShortIdx(name, 0)); }
int LBXGL_ASM_GetVarInt(char *name)
	{ return(LBXGL_ASM_GetVarIntIdx(name, 0)); }

void LBXGL_ASM_SetVarByte(char *name, int val)
	{ LBXGL_ASM_SetVarByteIdx(name, 0, val); }
void LBXGL_ASM_SetVarShort(char *name, int val)
	{ LBXGL_ASM_SetVarShortIdx(name, 0, val); }
void LBXGL_ASM_SetVarInt(char *name, int val)
	{ LBXGL_ASM_SetVarIntIdx(name, 0, val); }
