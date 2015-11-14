#include <pdgl.h>

extern PDGL_Sample *pdgl_sample_root;

PDGL_API PDGL_Sample *PDGL_BTAC_CreateSample(
	int chan, int rate, int bits, int len, int flags)
{
	PDGL_Sample *head;
	int bsz, bsmp, n;

	head=gctalloc("pdgl_sample_t", sizeof(PDGL_Sample));
	head->chan=chan;
	head->rate=rate;
	head->bits=bits;
	head->len=len;
	head->flags=flags;
	
	head->blk_size=5;		//32 bytes (256 bits)
	head->blk_samples=6;	//64 samples
	
	if(head->flags&PDGL_BTAC_EFL_HALFMONO)
	{
		head->blk_size++;
	}
	
	if(len)
	{
		bsz=1<<head->blk_size;
		bsmp=1<<head->blk_samples;
		n=(len+bsmp-1)/bsmp;

		head->data=gctatomic("pdgl_snd_samples", n*bsz+256);
	}

	return(head);
}

PDGL_API int PDGL_BTAC_FlattenSample(
	PDGL_Sample *head, byte *obuf, int osz)
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

	obuf[24]=(head->flags>> 0) & 0xFF;
	obuf[25]=(head->flags>> 8) & 0xFF;
	obuf[26]=(head->flags>>16) & 0xFF;
	obuf[27]=(head->flags>>24) & 0xFF;
	
	bsz=1<<head->blk_size;
	bsmp=1<<head->blk_samples;
	n=(head->len+bsmp-1)/bsmp;
	memcpy(obuf+256, head->data, n*bsz);
	
	return(256+n*bsz);
}

PDGL_API int PDGL_BTAC_SizeFlattenSample(
	PDGL_Sample *head)
{
	int bsz, bsmp, n;

	bsz=1<<head->blk_size;
	bsmp=1<<head->blk_samples;
	n=(head->len+bsmp-1)/bsmp;
//	memcpy(obuf+256, head->data, n*bsz);
	
	return(256+n*bsz);
}

PDGL_API int PDGL_BTAC_StoreSample(char *name,
	PDGL_Sample *head)
{
	VFILE *fd;
	byte *buf;
	int sz;
	int i;

	fd=vffopen(name, "wb");
	if(!fd)return(-1);

	sz=PDGL_BTAC_SizeFlattenSample(head);
	buf=malloc(sz);

	sz=PDGL_BTAC_FlattenSample(head, buf, sz);

	vfwrite(buf, 1, sz, fd);
	vfclose(fd);
	
	free(buf);
	
	return(0);
}

PDGL_API PDGL_Sample *PDGL_BTAC_LoadSample(char *name)
{
	PDGL_Sample *head;
	int bsz, bsmp, n;
	VFILE *fd;
	byte *buf;
	int fsz;

	fd=vffopen(name, "rb");
	if(!fd)return(NULL);
	
	vfseek(fd, 0, 2);
	fsz=vftell(fd);
	vfseek(fd, 0, 0);
	
	buf=malloc(fsz);
	vfread(buf, 1, fsz, fd);
	vfclose(fd);
	
	if(memcmp(buf, "BTAC00", 6))
	{
		printf("BTAC_LoadSample: Magic Fail\n");
		free(buf);
		return(NULL);
	}
	
	head=PDGL_BTAC_CreateSample(0, 0, 0, 0, 0);

	head->chan=buf[12];			//0=mono, 1=stereo
	head->bits=buf[13];			//virtual bits/sample (8/16)
	head->blk_size=buf[14];		//log2 block size (5=32 bytes)
	head->blk_samples=buf[15];	//log2 block samples (6=64 samples)
	
	head->rate=buf[16]|(buf[17]<<8)|(buf[18]<<16)|(buf[19]<<24);
	head->len=buf[20]|(buf[21]<<8)|(buf[22]<<16)|(buf[23]<<24);
	head->flags=buf[24]|(buf[25]<<8)|(buf[26]<<16)|(buf[27]<<24);

	head->data_offs=buf[8]|(buf[9]<<8)|(buf[10]<<16)|(buf[11]<<24);

	if(head->len)
	{
		bsz=1<<head->blk_size;
		bsmp=1<<head->blk_samples;
		n=(head->len+bsmp-1)/bsmp;

		head->data=gctatomic("pdgl_snd_samples", n*bsz+256);
		
		memcpy(head->data, buf+head->data_offs, n*bsz);
	}
	
	return(head);
}

PDGL_Sample *PDGL_Sound_LoadBTAC(char *name)
{
	PDGL_Sample *head;
	
	head=PDGL_BTAC_LoadSample(name);
	if(!head)return(NULL);

	head->name=dystrdup(name);

	head->next=pdgl_sample_root;
	pdgl_sample_root=head;
	return(head);
}

static PDGL_Sample **pdgl_btac_blockcache_sample=NULL;
static s16 **pdgl_btac_blockcache_buffer=NULL;
static int *pdgl_btac_blockcache_bufsz=NULL;
static int *pdgl_btac_blockcache_bufidx=NULL;

PDGL_API int PDGL_BTAC_GetSamplePoint_IndexBlock(PDGL_Sample *head, int idx)
{
	char *s;
	int bi, bs;
	int i;

	if(!pdgl_btac_blockcache_sample)
	{
		pdgl_btac_blockcache_sample=gctalloc(
			"pdgl_btac_blockcahe_sample", 4096*sizeof(PDGL_Sample *));
		pdgl_btac_blockcache_buffer=gctalloc(
			"pdgl_btac_blockcahe_buffer", 4096*sizeof(s16 *));
		pdgl_btac_blockcache_bufsz=gctalloc(
			"pdgl_btac_blockcahe_bufsz", 4096*sizeof(int));
		pdgl_btac_blockcache_bufidx=gctalloc(
			"pdgl_btac_blockcahe_bufsz", 4096*sizeof(int));
	}
	
	if(!head->name_hash)
	{
		s=head->name; i=0;
		while(*s) { i=i*4093+(*s++)+1; }
		head->name_hash=i;
	}
	
	bi=idx>>head->blk_samples;
	
	i=(head->name_hash*4093+bi)*4093;
	i=(i>>12)&4095;
	
	if((pdgl_btac_blockcache_sample[i]==head) &&
		(pdgl_btac_blockcache_bufidx[i]==bi))
	{
		return(i);
	}
	
	bs=(1<<head->blk_samples)*head->chan;
	if(!pdgl_btac_blockcache_buffer[i])
	{
		pdgl_btac_blockcache_buffer[i]=gctatomic(
			"pdgl_btac_blockcahe_buffer_data", bs*sizeof(s16));
		pdgl_btac_blockcache_bufsz[i]=bs;
	}

	if(pdgl_btac_blockcache_bufsz[i]<bs)
	{
		pdgl_btac_blockcache_buffer[i]=gcrealloc(
			pdgl_btac_blockcache_buffer[i], bs*sizeof(s16));
		pdgl_btac_blockcache_bufsz[i]=bs;
	}
	
	PDGL_BTAC_DecodeBlockTagS16(
		head->data+(bi<<head->blk_size),
		pdgl_btac_blockcache_buffer[i],
		head->flags);
	pdgl_btac_blockcache_sample[i]=head;
	pdgl_btac_blockcache_bufidx[i]=bi;
	
	return(i);
}

PDGL_API int PDGL_BTAC_GetSamplePoint(PDGL_Sample *head, int idx)
{
	int i, j, k;

	if(!head->data)
		return(0);

	i=PDGL_BTAC_GetSamplePoint_IndexBlock(head, idx);
	j=idx&((1<<head->blk_samples)-1);
	k=pdgl_btac_blockcache_buffer[i][j];
	return(k);
}
