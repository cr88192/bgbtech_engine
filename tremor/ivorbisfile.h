/********************************************************************
 *                                                                  *
 * THIS FILE IS PART OF THE OggVorbis 'TREMOR' CODEC SOURCE CODE.   *
 *                                                                  *
 * USE, DISTRIBUTION AND REPRODUCTION OF THIS LIBRARY SOURCE IS     *
 * GOVERNED BY A BSD-STYLE SOURCE LICENSE INCLUDED WITH THIS SOURCE *
 * IN 'COPYING'. PLEASE READ THESE TERMS BEFORE DISTRIBUTING.       *
 *                                                                  *
 * THE OggVorbis 'TREMOR' SOURCE CODE IS (C) COPYRIGHT 1994-2003    *
 * BY THE Xiph.Org FOUNDATION http://www.xiph.org/                  *
 *                                                                  *
 ********************************************************************

 function: stdio-based convenience library for opening/seeking/decoding

 ********************************************************************/

#ifndef _OV_FILE_H_
#define _OV_FILE_H_

#ifdef __cplusplus
extern "C"
{
#endif /* __cplusplus */

#include <stdio.h>
#include "ivorbiscodec.h"

/* The function prototypes for the callbacks are basically the same as for
 * the stdio functions fread, fseek, fclose, ftell. 
 * The one difference is that the FILE * arguments have been replaced with
 * a void * - this is to be used as a pointer to whatever internal data these
 * functions might need. In the stdio case, it's just a FILE * cast to a void *
 * 
 * If you use other functions, check the docs for these functions and return
 * the right values. For seek_func(), you *MUST* return -1 if the stream is
 * unseekable
 */
typedef struct {
  size_t (*read_func)  (void *ptr, size_t size, size_t nmemb, void *datasource);
  int    (*seek_func)  (void *datasource, ogg_int64_t offset, int whence);
  int    (*close_func) (void *datasource);
  long   (*tell_func)  (void *datasource);
} ov_callbacks;

typedef struct OggVorbis_File {
  void            *datasource; /* Pointer to a FILE *, etc. */
  int              seekable;
  ogg_int64_t      offset;
  ogg_int64_t      end;
  ogg_sync_state   *oy; 

  /* If the FILE handle isn't seekable (eg, a pipe), only the current
     stream appears */
  int              links;
  ogg_int64_t     *offsets;
  ogg_int64_t     *dataoffsets;
  ogg_uint32_t    *serialnos;
  ogg_int64_t     *pcmlengths;
  vorbis_info     vi;
  vorbis_comment  vc;

  /* Decoding working state local storage */
  ogg_int64_t      pcm_offset;
  int              ready_state;
  ogg_uint32_t     current_serialno;
  int              current_link;

  ogg_int64_t      bittrack;
  ogg_int64_t      samptrack;

  ogg_stream_state *os; /* take physical pages, weld into a logical
                          stream of packets */
  vorbis_dsp_state *vd; /* central working state for the packet->PCM decoder */

  ov_callbacks callbacks;

} OggVorbis_File;

TREM_API int ov_clear(OggVorbis_File *vf);
TREM_API int ov_open(FILE *f,OggVorbis_File *vf,char *initial,long ibytes);
TREM_API int ov_open_callbacks(void *datasource, OggVorbis_File *vf,
		char *initial, long ibytes, ov_callbacks callbacks);

TREM_API int ov_test(FILE *f,OggVorbis_File *vf,char *initial,long ibytes);
TREM_API int ov_test_callbacks(void *datasource, OggVorbis_File *vf,
		char *initial, long ibytes, ov_callbacks callbacks);
TREM_API int ov_test_open(OggVorbis_File *vf);

TREM_API long ov_bitrate(OggVorbis_File *vf,int i);
TREM_API long ov_bitrate_instant(OggVorbis_File *vf);
TREM_API long ov_streams(OggVorbis_File *vf);
TREM_API long ov_seekable(OggVorbis_File *vf);
TREM_API long ov_serialnumber(OggVorbis_File *vf,int i);

TREM_API ogg_int64_t ov_raw_total(OggVorbis_File *vf,int i);
TREM_API ogg_int64_t ov_pcm_total(OggVorbis_File *vf,int i);
TREM_API ogg_int64_t ov_time_total(OggVorbis_File *vf,int i);

TREM_API int ov_raw_seek(OggVorbis_File *vf,ogg_int64_t pos);
TREM_API int ov_pcm_seek(OggVorbis_File *vf,ogg_int64_t pos);
TREM_API int ov_pcm_seek_page(OggVorbis_File *vf,ogg_int64_t pos);
TREM_API int ov_time_seek(OggVorbis_File *vf,ogg_int64_t pos);
TREM_API int ov_time_seek_page(OggVorbis_File *vf,ogg_int64_t pos);

TREM_API ogg_int64_t ov_raw_tell(OggVorbis_File *vf);
TREM_API ogg_int64_t ov_pcm_tell(OggVorbis_File *vf);
TREM_API ogg_int64_t ov_time_tell(OggVorbis_File *vf);

TREM_API vorbis_info *ov_info(OggVorbis_File *vf,int link);
TREM_API vorbis_comment *ov_comment(OggVorbis_File *vf,int link);

TREM_API long ov_read(OggVorbis_File *vf,void *buffer,int length,
		    int *bitstream);

TREM_API FILE *ov_fopen(char *name, char *mode);
TREM_API void ov_fclose(FILE *fd);
TREM_API void *ov_malloc(size_t sz);
TREM_API void ov_free(void *p);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif


