typedef struct dyWRefCtx_s dyWRefCtx;

struct dyWRefCtx_s {
dyWRefCtx *next;
void *data;
u64 seg;

u64 (*alloc_mem)(dyWRefCtx *ctx, u64 sz);
int (*free_mem)(dyWRefCtx *ctx, u64 offs);
u64 (*realloc_mem)(dyWRefCtx *ctx, u64 offs, u64 sz);
int (*copy_mem)(dyWRefCtx *ctx, u64 dst, u64 src, u64 sz);
char *(*gettype_mem)(dyWRefCtx *ctx, u64 offs);

int (*read_mem)(void *buf, int sz, dyWRefCtx *ctx, u64 offs);
int (*write_mem)(void *buf, int sz, dyWRefCtx *ctx, u64 offs);
};
