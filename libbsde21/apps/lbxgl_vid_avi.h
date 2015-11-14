#ifndef __LBXGL_VID_AVI__
#define __LBXGL_VID_AVI__

#include <mpg123.h>
#include <mpglib.h>

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

typedef int		LONG;
typedef unsigned short	WORD;
typedef unsigned int	DWORD;
typedef DWORD		FOURCC;

typedef char		CHAR;
typedef unsigned char	BYTE;
typedef unsigned short	WCHAR;

typedef void		*LPVOID;

typedef struct tagRECT {
	LONG left;
	LONG top;
	LONG right;
	LONG bottom;
} RECT;

typedef struct tagPALETTEENTRY {
	BYTE peRed;
	BYTE peGreen;
	BYTE peBlue;
	BYTE peFlags;
} PALETTEENTRY;

typedef struct _MainAVIHeader
{
	DWORD dwMicroSecPerFrame;
	DWORD dwMaxBytesPerSec;
	DWORD dwPaddingGranularity;
	DWORD dwFlags;
	DWORD dwTotalFrames;
	DWORD dwInitialFrames;
	DWORD dwStreams;
	DWORD dwSuggestedBufferSize;
	DWORD dwWidth;
	DWORD dwHeight;
	DWORD dwReserved[4];
} MainAVIHeader;
typedef struct {
	FOURCC fccType;
	FOURCC fccHandler;
	DWORD dwFlags;
	WORD wPriority;
	WORD wLanguage;
	DWORD dwInitialFrames;
	DWORD dwScale;
	DWORD dwRate;
	DWORD dwStart;
	DWORD dwLength;
	DWORD dwSuggestedBufferSize;
	DWORD dwQuality;
	DWORD dwSampleSize;
	RECT rcFrame;
} AVIStreamHeader;
typedef struct _AVIINDEXENTRY {
	DWORD ckid;
	DWORD dwFlags;
	DWORD dwChunkOffset;
	DWORD dwChunkLength;
} AVIINDEXENTRY;
typedef struct _AVIPALCHANGE {
	BYTE bFirstEntry;
	BYTE bNumEntries;
	WORD wFlags;
	PALETTEENTRY peNew[1];
} AVIPALCHANGE;

typedef struct tWAVEFORMATEX {
	WORD wFormatTag;
	WORD nChannels;
	DWORD nSamplesPerSec;
	DWORD nAvgBytesPerSec;
	WORD nBlockAlign;
	WORD wBitsPerSample;
	WORD cbSize;
} WAVEFORMATEX;

typedef struct tagBITMAPINFOHEADER{
	DWORD	biSize;
	LONG	biWidth;
	LONG	biHeight;
	WORD	biPlanes;
	WORD	biBitCount;
	DWORD	biCompression;
	DWORD	biSizeImage;
	LONG	biXPelsPerMeter;
	LONG	biYPelsPerMeter;
	DWORD	biClrUsed;
	DWORD	biClrImportant;
} BITMAPINFOHEADER;
typedef struct tagRGBQUAD {
	BYTE	rgbBlue;
	BYTE	rgbGreen;
	BYTE	rgbRed;
	BYTE	rgbReserved;
} RGBQUAD;
typedef struct tagBITMAPINFO {
	BITMAPINFOHEADER bmiHeader;
	RGBQUAD bmiColors[1];
} BITMAPINFO;


typedef struct {
VFILE *fd;
int ofs_hdrl, ofs_movi, ofs_cur;
MainAVIHeader *avihead;
AVIStreamHeader *strhead, *vidhead, *audhead;
AVIINDEXENTRY *avi_index;
WAVEFORMATEX *wavefmt;
BITMAPINFOHEADER *bmihead;
void *fcbuf, *fdbuf;
int str_vid, str_aud;
int idxsz, frnum, cindex;

float frame_time;
int texnum;

#ifdef USE_XVIDCORE
xvid_dec_create_t xparam;
xvid_dec_frame_t xframe;
#endif
#ifdef USE_MPGLIB
struct mpstr mp;
#endif

LBXGL_VidCodecCTX *codec_ctx;

int drain;
void *fadbuf;
short *sampbuf, *esampbuf;
}LBXGL_AVI_Context;

#endif
