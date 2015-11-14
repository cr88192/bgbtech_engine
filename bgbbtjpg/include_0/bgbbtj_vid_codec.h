#ifndef BGBBTJ_VID_CODEC_H
#define BGBBTJ_VID_CODEC_H

#define BGBBTJ_VIFL_FASTENCODE		1
#define BGBBTJ_VIFL_FASTDECODE		2

typedef struct BGBBTJ_VidCodec_s BGBBTJ_VidCodec;
typedef struct BGBBTJ_VidCodecCTX_s BGBBTJ_VidCodecCTX;

typedef struct {
int width;
int height;
float frametime;
int num_frames;
int vid_fcc, vid_cmpr;
int aud_fcc, aud_cmpr;
}BGBBTJ_Video_Stats;

struct BGBBTJ_VidCodec_s {
BGBBTJ_VidCodec *next;
int *fcc;	//list of handlers, NULL -> try any

BGBBTJ_VidCodecCTX *(*begin_decompress)(int fcc,
	BGBBTJ_BMPInfoHeader *in, BGBBTJ_BMPInfoHeader *out);
BGBBTJ_VidCodecCTX *(*begin_compress)(int fcc,
	BGBBTJ_BMPInfoHeader *in, BGBBTJ_BMPInfoHeader *out);
int (*decompress_query)(int fcc,
	BGBBTJ_BMPInfoHeader *in, BGBBTJ_BMPInfoHeader *out);
int (*compress_query)(int fcc,
	BGBBTJ_BMPInfoHeader *in, BGBBTJ_BMPInfoHeader *out);
};

struct BGBBTJ_VidCodecCTX_s {
void *data;

byte *vidStrd;
byte *audStrd;
int sz_vidStrd;
int sz_audStrd;
int viQuality;
int viNextIFrame;
int viFlags;

int (*decompress_frame)(BGBBTJ_VidCodecCTX *ctx,
	void *src, void *dst, int ssz, int dsz);
int (*compress_frame)(BGBBTJ_VidCodecCTX *ctx,
	void *src, void *dst, int dsz, int qfl, int clrs, int *rfl);
int (*end_decompress)(BGBBTJ_VidCodecCTX *ctx);
int (*end_compress)(BGBBTJ_VidCodecCTX *ctx);
int (*decompress_frame_clrs)(BGBBTJ_VidCodecCTX *ctx,
	void *src, void *dst, int ssz, int dsz, int clrs);
};

struct BGBBTJ_VidLayer_s {
char *tagLayerName;
char *compLayerName;
void *data;
};

#endif
