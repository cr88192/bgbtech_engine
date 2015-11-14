#include <bgbmid.h>

extern BGBMID_Patch *bgbmid_patchroot;

static BGBMID_Sample **bgbmid_btac_blockcache_sample=NULL;
static s16 **bgbmid_btac_blockcache_buffer=NULL;
static int *bgbmid_btac_blockcache_bufsz=NULL;
static int *bgbmid_btac_blockcache_bufidx=NULL;


BMID_API BGBMID_Sample *BGBMID_BTAC_CreateSample(
	int chan, int rate, int bits, int len, int flags)
{
	BGBMID_Sample *head;
	int bsz, bsmp, n;

	head=bgbmid_malloc(sizeof(BGBMID_Sample));
	head->chan=chan;
	head->rate=rate;
	head->bits=bits;
	head->len=len;
	head->flags=flags;

	if((flags&BGBMID_BTAC_EBMODE_MASK)==BGBMID_BTAC_EBMODE_BLK64)
	{
		head->blk_size=5;		//32 bytes (256 bits)
		head->blk_samples=6;	//64 samples
	}else if((flags&BGBMID_BTAC_EBMODE_MASK)==BGBMID_BTAC_EBMODE_BLK256)
	{
		head->blk_size=7;		//128 bytes (1024 bits)
		head->blk_samples=8;	//256 samples
	}else if((flags&BGBMID_BTAC_EBMODE_MASK)==BGBMID_BTAC_EBMODE_BLK256LQ)
	{
		head->blk_size=6;		//64 bytes (512 bits)
		head->blk_samples=8;	//256 samples
	}else if((flags&BGBMID_BTAC_EBMODE_MASK)==BGBMID_BTAC_EBMODE_BLK256HQ)
	{
		head->blk_size=8;		//256 bytes (2048 bits)
		head->blk_samples=8;	//256 samples
	}

	
	if(head->flags&BGBMID_BTAC_EFL_HALFMONO)
	{
		head->blk_size++;
	}
	
	if(len)
	{
		bsz=1<<head->blk_size;
		bsmp=1<<head->blk_samples;
		n=(len+bsmp-1)/bsmp;

		head->btac=bgbmid_malloc(n*bsz+256);
	}

	return(head);
}

BMID_API int BGBMID_BTAC_FlattenSample(BGBMID_Sample *head, byte *obuf, int osz)
{
	int bsz, bsmp, n;
	byte *ct;
	
	memset(obuf, 0, 64);
	
	//magic bytes
	obuf[0]='B';
	obuf[1]='T';
	obuf[2]='A';
	obuf[3]='C';
	obuf[4]='0';
	obuf[5]='0';
	obuf[6]=0x8F;
	obuf[7]=0x0D;

	//header offset
	obuf[ 8]=0x00;
	obuf[ 9]=0x01;
	obuf[10]=0x00;
	obuf[11]=0x00;

	obuf[12]=head->chan;		//0=mono, 1=stereo
	obuf[13]=head->bits;		//virtual bits/sample (8/16)
	obuf[14]=head->blk_size;	//log2 block size (5=32 bytes)
	obuf[15]=head->blk_samples;	//log2 block samples (6=64 samples)

	//sample rate
	obuf[16]=(head->rate>> 0) & 0xFF;
	obuf[17]=(head->rate>> 8) & 0xFF;
	obuf[18]=(head->rate>>16) & 0xFF;
	obuf[19]=(head->rate>>24) & 0xFF;

	//sound length (samples)
	obuf[20]=(head->len>> 0) & 0xFF;
	obuf[21]=(head->len>> 8) & 0xFF;
	obuf[22]=(head->len>>16) & 0xFF;
	obuf[23]=(head->len>>24) & 0xFF;

	obuf[25]=(head->flags>> 0) & 0xFF;
	obuf[24]=(head->flags>> 8) & 0xFF;
//	obuf[24]=(head->flags>> 0) & 0xFF;
//	obuf[25]=(head->flags>> 8) & 0xFF;
	obuf[26]=(head->flags>>16) & 0xFF;
	obuf[27]=(head->flags>>24) & 0xFF;
	
	bsz=1<<head->blk_size;
	bsmp=1<<head->blk_samples;
	n=(head->len+bsmp-1)/bsmp;
	memcpy(obuf+256, head->btac, n*bsz);
	
	return(256+n*bsz);
}

BMID_API int BGBMID_BTAC_SizeFlattenSample(BGBMID_Sample *head)
{
	int bsz, bsmp, n;

	bsz=1<<head->blk_size;
	bsmp=1<<head->blk_samples;
	n=(head->len+bsmp-1)/bsmp;
//	memcpy(obuf+256, head->btac, n*bsz);
	
	return(256+n*bsz);
}

BMID_API int BGBMID_BTAC_Encode(short *ibuf,
	int ch, int rt, int len, int flags,
	byte *obuf, int obsz)
{
	BGBMID_Sample *head;
	int sz;

	if(ch==2)
	{
		flags|=BGBMID_BTAC_EFL_STEREO;
	}

	head=BGBMID_BTAC_CreateSample(ch, rt, 16, len, flags);
	BGBMID_BTAC_EncodeStreamS16(ibuf, head->btac, len, flags);
	sz=BGBMID_BTAC_FlattenSample(head, obuf, obsz);
	return(sz);
}

BMID_API int BGBMID_BTAC_StoreSample(char *name, BGBMID_Sample *head)
{
	void *fd;
	byte *buf;
	int sz;
	int i;

	fd=bgbmid_fopen(name, "wb");
	if(!fd)return(-1);

	sz=BGBMID_BTAC_SizeFlattenSample(head);
	buf=malloc(sz);

	sz=BGBMID_BTAC_FlattenSample(head, buf, sz);

	bgbmid_fwrite(buf, 1, sz, fd);
	bgbmid_fclose(fd);
	
	free(buf);
	
	return(0);
}

BMID_API byte *BGBMID_LoadFile(char *name, int *rsz)
{
	void *fd;
	byte *buf;
	int fsz;

	fd=bgbmid_fopen(name, "rb");
	if(!fd)
	{
		buf=BGBMID_PackReadFile(name, &fsz);
		if(buf)
		{
			if(rsz)*rsz=fsz;
			return(buf);
		}
	
		return(NULL);
	}
	
	bgbmid_fseek(fd, 0, 2);
	fsz=bgbmid_ftell(fd);
	bgbmid_fseek(fd, 0, 0);
	
	buf=bgbmid_malloc(fsz+16);
	bgbmid_fread(buf, 1, fsz, fd);
	bgbmid_fclose(fd);
	
	if(rsz)*rsz=fsz;
	return(buf);
}

BMID_API int BGBMID_StoreFile(char *name, void *buf, int sz)
{
	void *fd;
	int i;

	fd=bgbmid_fopen(name, "wb");
	if(!fd)return(-1);

	bgbmid_fwrite(buf, 1, sz, fd);
	bgbmid_fclose(fd);
		
	return(0);
}

BMID_API BGBMID_Sample *BGBMID_BTAC_LoadSample(char *name)
{
	BGBMID_Sample *head;
	int bsz, bsmp, n;
	void *fd;
	byte *buf;
	int fsz;

	buf=BGBMID_LoadFile(name, &fsz);
	if(!buf)return(NULL);

	if(!memcmp(buf, "BTAC2A", 6))
	{
		head=BGBMID_BTAC2_DecodeSample(buf, fsz);
		bgbmid_free(buf);
		return(head);
	}
	
	if(memcmp(buf, "BTAC00", 6))
	{
		printf("BGBMID_BTAC_LoadSample: Magic Fail\n");
		free(buf);
		return(NULL);
	}
	
	head=BGBMID_BTAC_CreateSample(0, 0, 0, 0, 0);

	head->chan=buf[12];			//0=mono, 1=stereo
	head->bits=buf[13];			//virtual bits/sample (8/16)
	head->blk_size=buf[14];		//log2 block size (5=32 bytes)
	head->blk_samples=buf[15];	//log2 block samples (6=64 samples)
	
	head->rate=buf[16]|(buf[17]<<8)|(buf[18]<<16)|(buf[19]<<24);
	head->len=buf[20]|(buf[21]<<8)|(buf[22]<<16)|(buf[23]<<24);
//	head->flags=buf[24]|(buf[25]<<8)|(buf[26]<<16)|(buf[27]<<24);
	head->flags=buf[25]|(buf[24]<<8)|(buf[26]<<16)|(buf[27]<<24);

	head->btac_offs=buf[8]|(buf[9]<<8)|(buf[10]<<16)|(buf[11]<<24);

	if(head->len)
	{
		bsz=1<<head->blk_size;
		bsmp=1<<head->blk_samples;
		n=(head->len+bsmp-1)/bsmp;

		head->btac=bgbmid_malloc(n*bsz+256);
		
		memcpy(head->btac, buf+head->btac_offs, n*bsz);
	}
	
	bgbmid_free(buf);
	
	return(head);
}

BGBMID_Sample *BGBMID_Sound_LoadBTAC(char *name)
{
	BGBMID_Sample *head;
	
	head=BGBMID_BTAC_LoadSample(name);
	if(!head)return(NULL);

//	head->name=dystrdup(name);

//	head->next=bgbmid_sample_root;
//	bgbmid_sample_root=head;
	return(head);
}

BGBMID_Patch *BGBMID_LoadPatchRawBTAC(char *name)
{
	char tb[256];
	BGBMID_Patch *tmp;
	BGBMID_Sample *cur, *lst, *nxt;
	void *fd;
	short *bp;
	char **a;
	char *s;
	int rt, len;
	int i, j, k;

	lst=NULL;

	sprintf(tb, "%s.btac", name);
	cur=BGBMID_BTAC_LoadSample(tb);
	if(!cur)return(NULL);
	lst=cur;

	cur=lst; lst=NULL;
	while(cur)
	{
		nxt=cur->next;
		cur->next=lst;
		lst=cur; cur=nxt;
	}

	tmp=bgbmid_malloc(sizeof(BGBMID_Patch));
	memset(tmp, 0, sizeof(BGBMID_Patch));
	tmp->samp=lst;

	tmp->name=bgbmid_strdup(name);
	tmp->next=bgbmid_patchroot;
	bgbmid_patchroot=tmp;

	return(tmp);
}

BMID_API int BGBMID_BTAC_GetSamplePoint_IndexBlock(
	BGBMID_Sample *head, int idx)
{
	char *s;
	int bi, bs, bc, fl;
	int i;

	if(!bgbmid_btac_blockcache_sample)
	{
		bgbmid_btac_blockcache_sample=bgbmid_tyalloc(
			"bgbmid_btac_blockcahe_sample", 4096*sizeof(BGBMID_Sample *));
		bgbmid_btac_blockcache_buffer=bgbmid_tyalloc(
			"bgbmid_btac_blockcahe_buffer", 4096*sizeof(s16 *));
		bgbmid_btac_blockcache_bufsz=bgbmid_tyalloc(
			"bgbmid_btac_blockcahe_bufsz", 4096*sizeof(int));
		bgbmid_btac_blockcache_bufidx=bgbmid_tyalloc(
			"bgbmid_btac_blockcahe_bufsz", 4096*sizeof(int));
	}
	
//	if(!head->name_hash)
//	{
//		s=head->name; i=0;
//		while(*s) { i=i*4093+(*s++)+1; }
//		head->name_hash=i;
//	}
	
	bi=idx>>head->blk_samples;
	bc=head->len>>head->blk_samples;
	
//	i=(head->name_hash*4093+bi)*4093;
	i=(((int)head)*4093+bi)*4093;
	i=(i>>12)&4095;
	
	if((bgbmid_btac_blockcache_sample[i]==head) &&
		(bgbmid_btac_blockcache_bufidx[i]==bi))
	{
		return(i);
	}
	
	bs=2*(1<<head->blk_samples)*head->chan;
	if(!bgbmid_btac_blockcache_buffer[i])
	{
		bgbmid_btac_blockcache_buffer[i]=bgbmid_tyatomic(
			"bgbmid_btac_blockcahe_buffer_data", bs*sizeof(s16));
		bgbmid_btac_blockcache_bufsz[i]=bs;
	}

	if(bgbmid_btac_blockcache_bufsz[i]<bs)
	{
		bgbmid_btac_blockcache_buffer[i]=gcrealloc(
			bgbmid_btac_blockcache_buffer[i], bs*sizeof(s16));
		bgbmid_btac_blockcache_bufsz[i]=bs;
	}
	
	fl=head->flags;
	if((bi>0) && ((bi+1)<bc))
		fl|=BGBMID_BTAC_EFL_MIDSTREAM;
	
	if(head->DecodeBlock)
	{
		head->DecodeBlock(
			head, bi,
			bgbmid_btac_blockcache_buffer[i],
			fl);
	}else
	{
//		BGBMID_BTAC_Decode64BlockTagS16(
//		BGBMID_BTAC_DecodeBlockTagS16(
		BGBMID_BTAC_DecodeBlockTagS16DB(
			head->btac+(bi<<head->blk_size),
			bgbmid_btac_blockcache_buffer[i],
			fl);
	}
	bgbmid_btac_blockcache_sample[i]=head;
	bgbmid_btac_blockcache_bufidx[i]=bi;
	
	BGBMID_BufferRebuildMip(
		bgbmid_btac_blockcache_buffer[i],
		head->chan, (1<<head->blk_samples));
	
	return(i);
}

BMID_API int BGBMID_BTAC_GetSamplePoint(BGBMID_Sample *head,
	int idx, int depth, int ch)
{
	static int mipbase[18]=
	{
	     0,  65536,  98304, 114688,	//   0, 65536, 32768, 16384
	122880, 126976, 129024, 130048, //8192,  4096,  2048,  1024
	130560, 130816, 130944, 131008, // 512,   256,   128,    64
	131040, 131056, 131064, 131068, //  32,    16,     8,     4
	131070, 131071
	};
	int b;
	int i, j, k;

//	if(!head->btac)
//		return(0);
	if(!head->btac && !head->DecodeBlock)
		return(0);

//	if((idx<0) || (idx>=head->len))
//		return(0);

	idx=bgbmid_mod(idx, (head->len>>depth));

	if(depth)
	{
		b=mipbase[depth]>>(16-head->blk_samples);
		i=BGBMID_BTAC_GetSamplePoint_IndexBlock(head, idx<<depth);
		j=idx&((1<<(head->blk_samples-depth))-1);
		k=bgbmid_btac_blockcache_buffer[i][(b+j)*head->chan+ch];
		return(k);
	}else
	{
		i=BGBMID_BTAC_GetSamplePoint_IndexBlock(head, idx);
		j=idx&((1<<head->blk_samples)-1);
		k=bgbmid_btac_blockcache_buffer[i][j*head->chan+ch];
		return(k);
	}
}
