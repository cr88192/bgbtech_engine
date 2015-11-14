#define BGBGC_USECONS

#ifndef BGBGC_SHAVEMEM
#define BGBGC_CBLK_BITS		18
#define BGBGC_CBLK_CELLS	(1<<18)
//#define BGBGC_INITCONS		4	//initial cons heap in chunks
#define BGBGC_INITCONS		1	//initial cons heap in chunks
#else
#define BGBGC_CBLK_BITS		16
#define BGBGC_CBLK_CELLS	(1<<16)
#define BGBGC_INITCONS		1	//initial cons heap in chunks
#endif

#define BGBGC_USECONS64			//use 64-bit cons cells

// #define BGBGC_USEPRECISE
#define BGBGC_BLKSHIFT	16	//block shift (precise refs)

#define BGBGC_TINYLIST	64	//free list for tiny objects

#define BGBGC_CELLSHIFT	4	//shift for number of bytes per cell
#define BGBGC_MAPSHIFT	4	//shift for relation between maps and cells
#define BGBGC_WORDSHIFT	2	//shift for bytes per cpu word (2=32 bit).
#define BGBGC_CELLSIZE	(1<<BGBGC_CELLSHIFT)
#define BGBGC_CELLPAD	((1<<BGBGC_CELLSHIFT)-1)


#define BGBGC_BIGCELL_LIST	2048	//free list for tiny objects

#define BGBGC_BIGCELLSHIFT	9	//shift for number of bytes per cell
#define BGBGC_BIGMAPSHIFT	9	//shift for relation between maps and cells
#define BGBGC_BIGCELLSIZE	(1<<BGBGC_BIGCELLSHIFT)
#define BGBGC_BIGCELLPAD	((1<<BGBGC_BIGCELLSHIFT)-1)


#ifndef BGBGC_SHAVEMEM

// #define BGBGC_CHKSIZE	22	//shift of default chunk size
#define BGBGC_CHKSIZE	24	//shift of default chunk size
// #define BGBGC_INITHEAP	16	//initial heap in chunks
#define BGBGC_INITHEAP	4	//initial heap in chunks

// #define BGBGC_MARKSIZE	(1<<16)	//initial mark stack size
// #define BGBGC_MARKSIZE	(1<<18)	//initial mark stack size
#define BGBGC_MARKSIZE	(1<<20)	//initial mark stack size
#define BGBGC_LOBJSIZE	(1<<28)	//initial limit for large objects
#define BGBGC_HEAPLIM	(1<<28)	//initial limit for small objects
#define BGBGC_CONSLIM	(1<<24)	//initial limit for cons cells

#define BGBGC_BIGCHKSIZE	24	//shift of default chunk size
#define BGBGC_BIGINITHEAP	1	//initial heap in chunks
#define BGBGC_BIGHEAPLIM	(1<<28)	//initial limit for big objects

#else

#define BGBGC_CHKSIZE	20	//shift of default chunk size
#define BGBGC_INITHEAP	4	//initial heap in chunks
#define BGBGC_MARKSIZE	(1<<16)	//initial mark stack size
#define BGBGC_LOBJSIZE	(1<<24)	//initial limit for large objects
#define BGBGC_HEAPLIM	(1<<26)	//initial limit for large objects
#define BGBGC_CONSLIM	(1<<20)	//initial limit for cons cells

#define BGBGC_BIGCHKSIZE	22	//shift of default chunk size
#define BGBGC_BIGINITHEAP	1	//initial heap in chunks
#define BGBGC_BIGHEAPLIM	(1<<24)	//initial limit for big objects

#endif


#define BGBGC_KIB(n)	(((n)+1023)>>10)
#define BGBGC_CELLKIB(n)	((((n)<<BGBGC_CELLSHIFT)+1023)>>10)
#define BGBGC_CONSKIB(n)	((((n)<<3)+1023)>>10)

#define	BGBGC_MAGIC		0x87654321
#define	BGBGC_XMAGIC	0x98765432

#define	BGBGC_BIGMAGIC		0xA9876543
#define	BGBGC_BIGXMAGIC		0xBA987654

//map bits
#define	BGBGC_MAP_TYPEMASK	0x03
#define	BGBGC_MAP_MARKMASK	0x0C

#define	BGBGC_MAP_BASEMASK	0x0F

#define	BGBGC_MAP_TYPE_FREE		0x00
#define	BGBGC_MAP_TYPE_HEAD		0x01
#define	BGBGC_MAP_TYPE_BODY		0x02
#define	BGBGC_MAP_TYPE_RESV		0x03
#define	BGBGC_MAP_TYPE_MASK		0x03

#define	BGBGC_MAP_MARK_WHITE	0x00
#define	BGBGC_MAP_MARK_BLACK	0x04
#define	BGBGC_MAP_MARK_EXBLACK	0x08
#define	BGBGC_MAP_MARK_LOCK		0x0C
#define	BGBGC_MAP_MARK_MASK		0x0C

#define	BGBGC_MAP_TRC_MASK		0x70
#define	BGBGC_MAP_TRC_GENERIC	0x00
#define	BGBGC_MAP_TRC_ATOMIC	0x10
#define	BGBGC_MAP_TRC_DEFILED	0x20
#define	BGBGC_MAP_TRC_PRECISE0	0x30
#define	BGBGC_MAP_TRC_PRECISE1	0x40
#define	BGBGC_MAP_TRC_PRECISE2	0x50
#define	BGBGC_MAP_TRC_PRECISE3	0x60
#define	BGBGC_MAP_TRC_PRECISEM	0x70

#define	BGBGC_MAP_CONSV_MARK	0x80	//was marked conservatively

#define	BGBGC_MAP_NEW_HEAD		(BGBGC_MAP_TYPE_HEAD|BGBGC_MAP_MARK_EXBLACK)
#define	BGBGC_MAP_NEW_BODY		(BGBGC_MAP_TYPE_BODY|BGBGC_MAP_MARK_EXBLACK)

#define	BGBGC_MAP_LOCK_HEAD		(BGBGC_MAP_TYPE_HEAD|BGBGC_MAP_MARK_LOCK)
#define	BGBGC_MAP_LOCK_BODY		(BGBGC_MAP_TYPE_BODY|BGBGC_MAP_MARK_LOCK)

#ifdef BGBGC_USECONS64
typedef s64 bgbgc_consref;
#else
typedef nlint bgbgc_consref;
#endif

typedef struct BGBGC_Chunk_s BGBGC_Chunk;
typedef struct BGBGC_BlockInfo_s BGBGC_BlockInfo;
typedef struct BGBGC_LocalHeapInfo_s BGBGC_LocalHeapInfo;

struct BGBGC_Chunk_s {
	BGBGC_Chunk *next;
	int magic;
	int size;
	int rover;
	int cells;
	byte *map;
	byte *data;
};

struct BGBGC_BlockInfo_s {
	int next;	//next in list
	int rnext;	//next in physical memory (splits/merges)
	int rprev;	//prev in physical memory (splits/merges)
	void *ptr;	//pointer to physical address
	int size;	//physical block size
	int flags;	//flags, 1&=free
};

struct BGBGC_LocalHeapInfo_s {
	BGBGC_LocalHeapInfo *next;
	char *name;

	void *block[256];
	byte *block_map[256];
	int nblock;

	byte *rov;		//rover pos
	byte *srov;		//rover start
	byte *erov;		//rover end
	void *list;		//allocation list
};

extern BGBGC_Chunk *bgbgc_initchain, *bgbgc_curchunk, *bgbgc_curxchunk;

#ifdef WIN32
extern void *_data_start__, *_data_end__;
extern void *_bss_start__, *_bss_end__;
#endif

#ifdef linux
extern void *__data_start, *__data_end;
extern void *__bss_start, *__bss_end;
#endif

extern void *bgbgc_stackbase;

extern volatile int bgbgc_used_cells;
extern volatile int bgbgc_free_cells;
extern volatile int bgbgc_freed_cells;
extern volatile int bgbgc_manual_cells;
extern volatile int bgbgc_total_cells;

extern volatile int bgbgc_last_gc;
extern volatile int bgbgc_est_used_cells, bgbgc_est_free_cells;
extern volatile int bgbgc_num_lobjs, bgbgc_size_lobjs;
extern volatile int bgbgc_freed_lobjs, bgbgc_freed_size_lobjs, bgbgc_size_lim_lobjs;

extern volatile int bgbgc_lastgc;

extern volatile void **bgbgc_markstack;
extern volatile int bgbgc_markstackpos;
extern volatile int bgbgc_markstacklim;

extern void **bgbgc_lobj;
extern int *bgbgc_lobj_size;
extern int *bgbgc_lobj_flag;
extern int bgbgc_lobj_num;
extern int bgbgc_lobj_max;

extern void (*bgbgc_markhdlr[1024])();
extern int bgbgc_nmarkhdlr;
