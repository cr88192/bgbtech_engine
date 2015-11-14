#include <lbxgl2.h>

#define LWO_FOURCC(a, b, c, d)	((a)|((b)<<8)|((c)<<16)|((d)<<24))

#define FCC_FORM	LWO_FOURCC('F', 'O', 'R', 'M')
#define FCC_LWOB	LWO_FOURCC('L', 'W', 'O', 'B')


static u16 GetUInt16BE(byte *buf)
{
	return((buf[0]<<8)|buf[1]);
}

static s16 GetSInt16BE(byte *buf)
{
	return((s16)((buf[0]<<8)|buf[1]));
}

static u32 GetUInt32BE(byte *buf)
{
	return((buf[0]<<24)|(buf[1]<<16)|(buf[2]<<8)|buf[3]);
}

static s32 GetSInt32BE(byte *buf)
{
	return((s32)((buf[0]<<24)|(buf[1]<<16)|(buf[2]<<8)|buf[3]));
}

static u32 GetUInt32LE(byte *buf)
{
	return((buf[3]<<24)|(buf[2]<<16)|(buf[1]<<8)|buf[0]);
}

static void GetLumpHead(byte *buf, u32 *rfcc, u32 *rsz)
{
	*rfcc=GetUInt32LE(buf+0);
	*rsz=GetUInt32BE(buf+4);
}

static void GetFormHead(byte *buf, u32 *rfcc, u32 *rsz, u32 *rfcc2)
{
	*rfcc=GetUInt32LE(buf+0);
	*rsz=GetUInt32BE(buf+4);
	*rfc2=GetUInt32LE(buf+8);
}

LBXGL_MeshGroup *LBXGL_LWO_LoadModel(char *name)
{
	byte *buf;
	u32 fcca, fccb, fsz;
	int sz;
	
	buf=vf_loadfile(name, &sz);
	if(!buf)return(NULL);
	
	GetFormHead(buf, &fcca, &fsz, &fccb);
	
	if((fcca!=FCC_FORM) || (fccb!=FCC_LWOB))
	{
		return(NULL)
	}
	
	return(NULL);
}
