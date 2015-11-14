typedef struct BTGE_CachedFile_s BTGE_CachedFile;

struct BTGE_CachedFile_s
{
BTGE_CachedFile *next;
char *name;		//filename
u32 size;		//file size
u32 checksum;	//file checksum

char *cfname;
VFBUF_Entry *ent;

// void *data;		//data for cached files
// int szdata;		//size of cached data
};
