typedef struct BSXRP_MuxBuffer_s BSXRP_MuxBuffer;
typedef struct BSXRP_StreamContext_s BSXRP_StreamContext;
typedef struct BSXRP_Context_s BSXRP_Context;

struct BSXRP_MuxBuffer_s
{
BSXRP_MuxBuffer *next;
int muxid;

byte *buffer;		//buffer for transmit/recieve
int sz_buffer;		//size of buffer
int offset;			//current offset for transmit/recieve
int ack_offset;		//offset known recieved
};

struct BSXRP_StreamContext_s
{
//huffman tables
u16 huff_code[4][384];	//literal codes
u16 huff_mask[4][384];	//literal code masks
u16 huff_next[4][384];	//literal table code chains
byte huff_len[4][384];	//literal code lengths
u16 huff_idx[4][256];	//literal index table

int huff_stat[4][384];	//symbol count stats (temporary)

//IO buffers
byte *ibuf;			//input buffer
byte *obuf;			//output buffer
byte *cs;			//current input pos
byte *cse;			//current input buffer end
// byte *ce;			//end of input buffer
byte *ct;			//current output pos
byte *cte;			//end of output buffer

//bit window
u32 bit_win;		//bit window
int bit_pos;		//bit window position

//LZ77
byte lz_wbuf[65536];	//window buffer
u16 lz_lbuf[65536];		//link buffer
u16 lz_hash[4096];		//hash table
u16 lz_wpos;			//window position
int lz_maxdist;			//maximum distance
int lz_sdepth;			//search depth

//message value encoding
dyt val_mru[65536];
int val_szmru;

u16 *val_cts;
u16 *val_ct;
u16 *val_cte;
};

struct BSXRP_Context_s
{
BSXRP_StreamContext *rx;	//recieving stream context
BSXRP_StreamContext *tx;	//sending stream context
Meta0_Con *con;				//connection
dyt queue_in;				//incomming messages
dyt queue_out;				//outgoing messages

BSXRP_MuxBuffer *mux_rx;	//multiplex recieve
BSXRP_MuxBuffer *mux_tx;	//multiplex transmit
int mux_tx_pending;			//multiplex transmitted-pending
int mux_rover;				//mux ID rover
};
