#ifndef LBXGL_VID_CODEC_H
#define LBXGL_VID_CODEC_H

typedef struct LBXGL_VidCodec_s LBXGL_VidCodec;
typedef struct LBXGL_VidCodecCTX_s LBXGL_VidCodecCTX;

typedef struct {
int width;
int height;
float frametime;
int num_frames;
int vid_fcc, vid_cmpr;
int aud_fcc, aud_cmpr;
}LBXGL_Video_Stats;

struct LBXGL_VidCodec_s {
LBXGL_VidCodec *next;
int *fcc;	//list of handlers, NULL -> try any

LBXGL_VidCodecCTX *(*begin_decompress)(int fcc,
	PDGL_BMPInfoHeader *in, PDGL_BMPInfoHeader *out);
};

struct LBXGL_VidCodecCTX_s {
void *data;

int (*decompress_frame)(LBXGL_VidCodecCTX *ctx,
	void *src, void *dst, int ssz, int dsz);
};

#endif
