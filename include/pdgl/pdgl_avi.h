typedef struct {
u32 dwMicroSecPerFrame;
u32 dwMaxBytesPerSec;
u32 dwReserved1;
u32 dwFlags;
u32 dwTotalFrames;
u32 dwInitialFrames;
u32 dwStreams;
u32 dwSuggestedBufferSize;
u32 dwWidth;
u32 dwHeight;
u32 dwScale;
u32 dwRate;
u32 dwStart;
u32 dwLength;
}PDGL_MainAVIHeader;

typedef struct {
u32 fccType;
u32 fccHandler;
u32 dwFlags;
u32 dwReserved1;
u32 dwInitialFrames;
u32 dwScale;
u32 dwRate;
u32 dwStart;
u32 dwLength;
u32 dwSuggestedBufferSize;
u32 dwQuality;
u32 dwSampleSize;
}PDGL_AVIStreamHeader;

typedef struct {
unsigned int biSize;
unsigned int biWidth;
unsigned int biHeight;
unsigned short biPlanes;
unsigned short biBitCount;
unsigned int biCompression;
unsigned int biSizeImage;
unsigned int biXPelsPerMeter;
unsigned int biYPelsPerMeter;
unsigned int biClrUsed;
unsigned int biClrImportant;
}PDGL_BMPInfoHeader;

typedef struct
{
unsigned short wFormatTag;
unsigned short nChannels;
unsigned int nSamplesPerSec;
unsigned int nAvgBytesPerSec;
unsigned short nBlockAlign;
unsigned short wBitsPerSample;
unsigned short cbSize;
}PDGL_WaveFmt;


typedef struct {
VFILE *fd;

int offs_avih;

int offs_vidh;
int offs_vidf;

int offs_audh;
int offs_audf;

int osz_avi;
int osz_hdrl;
int osz_strl;
int osz_movi;

PDGL_MainAVIHeader *avih;
PDGL_AVIStreamHeader *vidh;
PDGL_AVIStreamHeader *audh;
PDGL_BMPInfoHeader *vidf;
PDGL_WaveFmt *audf;

uint *index;
int n_index, m_index;
int fps;
int xs, ys;
int frnum;

volatile double accDt;
volatile double accDt2;

volatile void *vidCtx;
volatile void *audCtx;

// void *lock;

volatile byte *capFrameBuf;
volatile byte *capFrameBuf2;
volatile int capFrameIdx;
volatile int capFrameLastIdx;
volatile byte capReqClose;
volatile byte capReqBreak;

volatile byte *capEncodeBuf;
volatile byte *capEncodeAuBuf;

volatile short *audSampleBuf;
volatile int audSampleBufSz;
volatile int audSampleBufEndPos;
volatile int audSampleBufStartPos;

}PDGL_AVICtx;

