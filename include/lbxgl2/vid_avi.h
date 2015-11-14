#ifndef LBXGL_VID_AVI
#define LBXGL_VID_AVI

#define RIFF_MAKETAG(a, b, c, d)	((a)+((b)<<8)+((c)<<16)+((d)<<24))
#define RIFF_UNHEX_NIBBLE(a)		( \
	(((a)>='0')&&((a)<='9'))?((a)-'0') : \
	(((a)>='A')&&((a)<='F'))?((a)-'A'+10) : \
	(((a)>='a')&&((a)<='f'))?((a)-'a'+10) : 0)
#define RIFF_HEX_NIBBLE(a)		(((a)<10)?((a)+'0'):((a)+'A'))

#define RIFF_MAKEHEXTAG(a)	RIFF_MAKETAG( \
					RIFF_HEX_NIBBLE(((a)>>12)&0xf), \
					RIFF_HEX_NIBBLE(((a)>>8)&0xf), \
					RIFF_HEX_NIBBLE(((a)>>4)&0xf), \
					RIFF_HEX_NIBBLE((a)&0xf) \
					)

#define RIFF_TAG_RIFF	RIFF_MAKETAG('R','I','F','F')
#define RIFF_TAG_AVI	RIFF_MAKETAG('A','V','I',' ')
#define RIFF_TAG_LIST	RIFF_MAKETAG('L','I','S','T')

#define RIFF_TAG_hdrl	RIFF_MAKETAG('h','d','r','l')
#define RIFF_TAG_avih	RIFF_MAKETAG('a','v','i','h')
#define RIFF_TAG_strl	RIFF_MAKETAG('s','t','r','l')
#define RIFF_TAG_strh	RIFF_MAKETAG('s','t','r','h')
#define RIFF_TAG_strf	RIFF_MAKETAG('s','t','r','f')
#define RIFF_TAG_strd	RIFF_MAKETAG('s','t','r','d')
#define RIFF_TAG_movi	RIFF_MAKETAG('m','o','v','i')
#define RIFF_TAG_rec	RIFF_MAKETAG('r','e','c',' ')
#define RIFF_TAG_idx1	RIFF_MAKETAG('i','d','x','1')

#define RIFF_TAG_auds	RIFF_MAKETAG('a','u','d','s')
#define RIFF_TAG_vids	RIFF_MAKETAG('v','i','d','s')
#define RIFF_TAG_divx	RIFF_MAKETAG('d','i','v','x')
#define RIFF_TAG_xvid	RIFF_MAKETAG('x','v','i','d')
#define RIFF_TAG_DIVX	RIFF_MAKETAG('D','I','V','X')
#define RIFF_TAG_XVID	RIFF_MAKETAG('X','V','I','D')

#define RIFF_TAG_MJPG	RIFF_MAKETAG('M','J','P','G')
#define RIFF_TAG_JPEG	RIFF_MAKETAG('J','P','E','G')

#define RIFF_TAG_MBTJ	RIFF_MAKETAG('M','B','T','J')
#define RIFF_TAG_BTJP	RIFF_MAKETAG('B','T','J','P')

#define RIFF_TAG_MBTC	RIFF_MAKETAG('M','B','T','C')
#define RIFF_TAG_BTIC	RIFF_MAKETAG('B','T','I','C')

#define RIFF_TAG_MPNG	RIFF_MAKETAG('M','B','T','C')
#define RIFF_TAG_PNG	RIFF_MAKETAG('P','N','G',' ')

#define RIFF_TAG_rpza	RIFF_MAKETAG('r','p','z','a')
#define RIFF_TAG_azpr	RIFF_MAKETAG('a','z','p','r')

#define RIFF_TAG_bt1c	RIFF_MAKETAG('b','t','1','c')
#define RIFF_TAG_c1tb	RIFF_MAKETAG('c','1','t','b')

#define LBXGL_AVI_FL_UPDATING	1
#define LBXGL_AVI_FL_DELTATICK	2

typedef struct {
	s32 left;
	s32 top;
	s32 right;
	s32 bottom;
} LBXGL_RECT;

typedef struct {
	byte peRed;
	byte peGreen;
	byte peBlue;
	byte peFlags;
} LBXGL_PALETTEENTRY;

typedef struct
{
	u32 dwMicroSecPerFrame;
	u32 dwMaxBytesPerSec;
	u32 dwPaddingGranularity;
	u32 dwFlags;
	u32 dwTotalFrames;
	u32 dwInitialFrames;
	u32 dwStreams;
	u32 dwSuggestedBufferSize;
	u32 dwWidth;
	u32 dwHeight;
	u32 dwReserved[4];
} LBXGL_MainAVIHeader;

typedef struct {
	u32 fccType;
	u32 fccHandler;
	u32 dwFlags;
	u16 wPriority;
	u16 wLanguage;
	u32 dwInitialFrames;
	u32 dwScale;
	u32 dwRate;
	u32 dwStart;
	u32 dwLength;
	u32 dwSuggestedBufferSize;
	u32 dwQuality;
	u32 dwSampleSize;
	LBXGL_RECT rcFrame;
} LBXGL_AVIStreamHeader;

typedef struct {
	u32 ckid;
	u32 dwFlags;
	u32 dwChunkOffset;
	u32 dwChunkLength;
} LBXGL_AVIINDEXENTRY;

typedef struct {
	byte bFirstEntry;
	byte bNumEntries;
	u16 wFlags;
	LBXGL_PALETTEENTRY peNew[1];
} LBXGL_AVIPALCHANGE;

typedef struct {
	u16 wFormatTag;
	u16 nChannels;
	u32 nSamplesPerSec;
	u32 nAvgBytesPerSec;
	u16 nBlockAlign;
	u16 wBitsPerSample;
	u16 cbSize;
} LBXGL_WAVEFORMATEX;

typedef struct {
	u32	biSize;
	s32	biWidth;
	s32	biHeight;
	u16	biPlanes;
	u16	biBitCount;
	u32	biCompression;
	u32	biSizeImage;
	s32	biXPelsPerMeter;
	s32	biYPelsPerMeter;
	u32	biClrUsed;
	u32	biClrImportant;
} LBXGL_BITMAPINFOHEADER;

typedef struct {
	byte	rgbBlue;
	byte	rgbGreen;
	byte	rgbRed;
	byte	rgbReserved;
} LBXGL_RGBQUAD;

typedef struct {
	LBXGL_BITMAPINFOHEADER bmiHeader;
	LBXGL_RGBQUAD bmiColors[1];
} LBXGL_BITMAPINFO;

typedef struct LBXGL_AVI_Context_s LBXGL_AVI_Context;

struct LBXGL_AVI_Context_s {
VFILE *fd;
int ofs_hdrl, ofs_movi, ofs_cur;
LBXGL_MainAVIHeader *avihead;
LBXGL_AVIStreamHeader *strhead, *vidhead, *audhead;
LBXGL_AVIINDEXENTRY *avi_index;
LBXGL_WAVEFORMATEX *wavefmt;
LBXGL_BITMAPINFOHEADER *bmihead;
void *fcbuf, *fdbuf;
int str_vid, str_aud;
int idxsz, frnum, cindex;

void *fdbuf_norm;	//extension: normal map
void *fdbuf_spec;	//extension: specular
void *fdbuf_luma;	//extension: luma

float frame_time;
int texnum;
int chroma_color;	//hack to support chroma-key filtering

int texnum_norm;
int texnum_spec;
int texnum_luma;
int mjpg_cty;		//component type
int mjpg_clrs;		//colorspace

LBXGL_VidCodecCTX *codec_ctx;
void *aud_codec_ctx;

int drain;
void *fadbuf;
short *sampbuf, *esampbuf;

int flags;
char *avi_name;

LBXGL_AVI_Context *avi_next;
};

#endif
