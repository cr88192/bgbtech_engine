#include <bgbbtj.h>

#ifdef BGBBTJ_VLCDRV

#ifndef MODULE_STRING
#define	MODULE_STRING "bgbtjvlc"
#endif

#define DOMAIN_  "vlc-bgbtjvlc"
#define _(str)  dgettext(DOMAIN_, str)
#define N_(str) (str)


#ifndef inline
#define inline
#endif

#include <vlc_common.h>
#include <vlc_plugin.h>
#include <vlc_codec.h>

struct decoder_sys_t
{
	BGBBTJ_VidCodecCTX *ctx;
    bool b_error;
};

static int  OpenDecoder   ( vlc_object_t * );
static void CloseDecoder  ( vlc_object_t * );

static picture_t *DecodeBlock  ( decoder_t *, block_t ** );

vlc_module_begin ()
    set_category( CAT_INPUT )
    set_subcategory( SUBCAT_INPUT_VCODEC )
    set_description( N_("BGBTech Image Codec") )
    set_capability( "decoder", 100 )
    set_callbacks( OpenDecoder, CloseDecoder )
    add_shortcut( "btic" )
vlc_module_end ()

static int OpenDecoder( vlc_object_t *p_this )
{
    decoder_t *p_dec = (decoder_t*)p_this;
    decoder_sys_t *p_sys;
	BGBBTJ_VidCodecCTX *ctx;
	BGBBTJ_BMPInfoHeader bmpin, bmpout;

	if((p_dec->fmt_in.i_codec != VLC_FOURCC('B','T','I','C')) &&
		(p_dec->fmt_in.i_codec != VLC_FOURCC('b','t','1','c')) &&
		(p_dec->fmt_in.i_codec != VLC_FOURCC('b','t','1','d')))
	{
		return VLC_EGENERIC;
	}

	//allocte state
	p_sys = malloc(sizeof(decoder_sys_t));
    if(!p_sys)
		{ return(VLC_ENOMEM); }
	memset(p_sys, 0, sizeof(decoder_sys_t));
	p_dec->p_sys = p_sys;

	memset(&bmpin, 0, sizeof(BGBBTJ_BMPInfoHeader));
	memset(&bmpout, 0, sizeof(BGBBTJ_BMPInfoHeader));
	
	bmpin.biCompression=p_dec->fmt_in.i_codec;
	bmpin.biWidth=p_dec->fmt_in.video.i_width;
	bmpin.biHeight=p_dec->fmt_in.video.i_height;
	bmpin.biPlanes=1;
	bmpin.biBitCount=32;

	bmpout.biWidth=p_dec->fmt_in.video.i_width;
	bmpout.biHeight=-p_dec->fmt_in.video.i_height;
	bmpin.biPlanes=1;
	bmpin.biBitCount=32;

	ctx=BGBBTJ_Codec_BeginDecompress(RIFF_TAG_BTIC, &bmpin, &bmpout);
	if(!ctx) { return(VLC_EGENERIC); }
	p_sys->ctx=ctx;

    p_dec->fmt_out.i_cat = VIDEO_ES;
    p_dec->fmt_out.i_codec = VLC_CODEC_RGBA;

	p_dec->fmt_out.video.i_width = bmpout.biWidth;
    p_dec->fmt_out.video.i_height = bmpin.biHeight;
    p_dec->fmt_out.video.i_sar_num = 1;
    p_dec->fmt_out.video.i_sar_den = 1;
    p_dec->fmt_out.video.i_rmask = 0x000000ff;
    p_dec->fmt_out.video.i_gmask = 0x0000ff00;
    p_dec->fmt_out.video.i_bmask = 0x00ff0000;

    p_dec->pf_decode_video = DecodeBlock;
    
    return(VLC_SUCCESS);
}

static picture_t *DecodeBlock( decoder_t *p_dec, block_t **pp_block )
{
    decoder_sys_t *p_sys = p_dec->p_sys;
    block_t *p_block;
    picture_t *p_pic = 0;
    int ret;
    int i, j, k;

    if( !pp_block || !*pp_block )
		{ return(NULL); }

    p_block = *pp_block;
    p_sys->b_error = false;

    if( p_block->i_flags & BLOCK_FLAG_DISCONTINUITY )
    {
        block_Release(p_block); *pp_block = NULL;
        return(NULL);
    }

    p_pic = decoder_NewPicture( p_dec );
    if(!p_pic)
    {
		block_Release(p_block); *pp_block = NULL;
		return(NULL);
    }

	ret=BGBBTJ_Codec_DecompressFrame(p_sys->ctx,
		p_block->p_buffer, p_pic->p->p_pixels,
		p_block->i_buffer,
		1<<24);

    p_pic->date = p_block->i_pts > VLC_TS_INVALID ? p_block->i_pts : p_block->i_dts;

    block_Release(p_block); *pp_block = NULL;
    return(p_pic);
}

static void CloseDecoder( vlc_object_t *p_this )
{
    decoder_t *p_dec = (decoder_t *)p_this;
    decoder_sys_t *p_sys = p_dec->p_sys;

	if(p_sys->ctx)
	{
		BGBBTJ_VidCodecCTX_EndDecompress(p_sys->ctx);
		BGBBTJ_VidCodecCTX_Destroy(p_sys->ctx);
		p_sys->ctx=NULL;
	}

    free(p_sys);
}

#endif
