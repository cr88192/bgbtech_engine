/*
BGBMID Speech Recognizer
 */

#define BGBMID_BMSR_BLOCKSZ			64
#define BGBMID_BMSR_BLOCKSAMP8K		64

// typedef struct BMSR_Context_s BMSR_Context;
// typedef struct BMSR_Sample_s BMSR_Sample;

struct BGBMID_BMSR_Context_s {
BGBMID_Context *bmctx;		//main context
BGBMID_BMSR_Sample *samp;	//word-samples

char *base;					//base path
int blksamp;				//block samples (8kHz)
};

struct BGBMID_BMSR_Sample_s {
BGBMID_BMSR_Sample *next;	//next
char *name;					//filename
char *word;					//associated word
short *buf;					//sample data
int *blks;					//as blocks
int len;					//length (samples)
int nblks;					//num blocks
int blksamp;				//block samples (8kHz)
};
