#define PDGL_SOUND_LOOP			1	//loop the sample
#define PDGL_SOUND_POINT		2	//point source
#define PDGL_SOUND_NOATTN		4	//disable attenuation
#define PDGL_SOUND_HIGHATTN		8	//rapid falloff
#define PDGL_SOUND_LOWATTN		16	//gradual falloff
#define PDGL_SOUND_CALCCURVOL	32	//calculate current volume
#define PDGL_SOUND_CALCCURDCT	64	//calculate current DCT (low frequency)

#define PDGL_SOUND_UNLINK	256		//remove sound from mixer

#define PDGL_SOUND_C		331	//speed of sound in m/s

#define PDGL_BTAC_EFL_STEREO		1	//samples are stereo
#define PDGL_BTAC_EFL_CENTERMONO	2	//encode as centered-mono
#define PDGL_BTAC_EFL_HALFMONO		4	//dealing with a single channel
#define PDGL_BTAC_EFL_MIDSTREAM		8	//we are in the middle-part of a stream

typedef struct PDGL_Sample_s PDGL_Sample;
typedef struct PDGL_MixChannel_s PDGL_MixChannel;

typedef struct PDGL_VoiceState_s PDGL_VoiceState;

struct PDGL_Sample_s {
PDGL_Sample *next;

char *name;
int name_hash;

byte chan;
byte bits;
byte blk_size;		//log2 block-size (bytes)
byte blk_samples;	//log2 of block-samples
int rate;
int len;
int flags;
int data_offs;

short *buf;			//raw PCM samples
byte *data;			//BTAC samples
};

struct PDGL_MixChannel_s {
PDGL_MixChannel *next;
char *name;	//sample/stream name
int id;		//channel id

int flags;
float vol;
int delay;		//delay until sample start

int curvol;		//volume at last point mixed
int currvol;	//current absolute volume
//float scale;
//float org[3]; //vol/(scale*|org-cam|)

//multipoint
int num_points;
int max_points;	//buffer size
float *points;	//x y z scale
float *vel;	//vx vy vz vw

//sound
PDGL_Sample *samp;
int offs, len;

//stream
void *data;
int (*getsamples)(short *mixbuf, int cnt, void *data);

int *curdct;	//current mixed DCT block
};

struct PDGL_VoiceState_s {
void *sgctx;		//speechgen context
int chan;
};
