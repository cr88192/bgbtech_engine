#include <bgbtac.h>

BTAC_API BTAC_SampleHeader *BTAC_CreateSample(
	int chan, int rate, int bits, int len, int flags)
{
	BTAC_SampleHeader *head;
	int bsz, bsmp, n;

	head=bgbtac_malloc(sizeof(BTAC_SampleHeader));
	head->chan=chan;
	head->rate=rate;
	head->bits=bits;
	head->length=len;
	head->flags=flags;
	
	head->blk_size=5;		//32 bytes (256 bits)
	head->blk_samples=6;	//64 samples
	
	if(head->flags&BGBTAC_EFL_HALFMONO)
	{
		head->blk_size++;
	}
	
	if(len)
	{
		bsz=1<<head->blk_size;
		bsmp=1<<head->blk_samples;
		n=(len+bsmp-1)/bsmp;

		head->data=bgbtac_malloc(n*bsz+256);
	}

	return(head);
}

BTAC_API int BTAC_FlattenSample(BTAC_SampleHeader *head, byte *obuf, int osz)
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
	obuf[20]=(head->length>> 0) & 0xFF;
	obuf[21]=(head->length>> 8) & 0xFF;
	obuf[22]=(head->length>>16) & 0xFF;
	obuf[23]=(head->length>>24) & 0xFF;

	obuf[24]=(head->flags>> 0) & 0xFF;
	obuf[25]=(head->flags>> 8) & 0xFF;
	obuf[26]=(head->flags>>16) & 0xFF;
	obuf[27]=(head->flags>>24) & 0xFF;
	
	bsz=1<<head->blk_size;
	bsmp=1<<head->blk_samples;
	n=(head->length+bsmp-1)/bsmp;
	memcpy(obuf+256, head->data, n*bsz);
	
	return(256+n*bsz);
}

BTAC_API int BTAC_SizeFlattenSample(BTAC_SampleHeader *head)
{
	int bsz, bsmp, n;

	bsz=1<<head->blk_size;
	bsmp=1<<head->blk_samples;
	n=(head->length+bsmp-1)/bsmp;
//	memcpy(obuf+256, head->data, n*bsz);
	
	return(256+n*bsz);
}

BTAC_API int BTAC_StoreSample(char *name, BTAC_SampleHeader *head)
{
	void *fd;
	byte *buf;
	int sz;
	int i;

	fd=bgbtac_fopen(name, "wb");
	if(!fd)return(-1);

	sz=BTAC_SizeFlattenSample(head);
	buf=malloc(sz);

	sz=BTAC_FlattenSample(head, buf, sz);

	bgbtac_fwrite(buf, 1, sz, fd);
	bgbtac_fclose(fd);
	
	free(buf);
	
	return(0);
}

BTAC_API BTAC_SampleHeader *BTAC_LoadSample(char *name)
{
	BTAC_SampleHeader *head;
	int bsz, bsmp, n;
	void *fd;
	byte *buf;
	int fsz;

	fd=bgbtac_fopen(name, "rb");
	if(!fd)return(NULL);
	
	bgbtac_fseek(fd, 0, 2);
	fsz=bgbtac_ftell(fd);
	bgbtac_fseek(fd, 0, 0);
	
	buf=malloc(fsz);
	bgbtac_fread(buf, 1, fsz, fd);
	bgbtac_fclose(fd);
	
	if(memcmp(buf, "BTAC00", 6))
	{
		printf("BTAC_LoadSample: Magic Fail\n");
		free(buf);
		return(NULL);
	}
	
	head=BTAC_CreateSample(0, 0, 0, 0, 0);

	head->chan=buf[12];			//0=mono, 1=stereo
	head->bits=buf[13];			//virtual bits/sample (8/16)
	head->blk_size=buf[14];		//log2 block size (5=32 bytes)
	head->blk_samples=buf[15];	//log2 block samples (6=64 samples)
	
	head->rate=buf[16]|(buf[17]<<8)|(buf[18]<<16)|(buf[19]<<24);
	head->length=buf[20]|(buf[21]<<8)|(buf[22]<<16)|(buf[23]<<24);
	head->flags=buf[24]|(buf[25]<<8)|(buf[26]<<16)|(buf[27]<<24);

	head->data_offs=buf[8]|(buf[9]<<8)|(buf[10]<<16)|(buf[11]<<24);

	if(head->length)
	{
		bsz=1<<head->blk_size;
		bsmp=1<<head->blk_samples;
		n=(head->length+bsmp-1)/bsmp;

		head->data=bgbtac_malloc(n*bsz+256);
		
		memcpy(head->data, buf+head->data_offs, n*bsz);
	}
	
	return(head);
}
