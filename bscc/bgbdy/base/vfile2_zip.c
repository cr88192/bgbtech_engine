#include <bgbgc.h>
#include <bgbdy.h>

int VFZIP_ReadUInt16(FILE *fd)
{
	int i;

	i=fgetc(fd);
	i|=fgetc(fd)<<8;
	return(i);
}

int VFZIP_ReadUInt32(FILE *fd)
{
	int i;

	i=fgetc(fd);
	i|=fgetc(fd)<<8;
	i|=fgetc(fd)<<16;
	i|=fgetc(fd)<<24;
	return(i);
}

void VFZIP_WriteUInt16(FILE *fd, int v)
{
	fputc(v&0xFF, fd);
	fputc((v>>8)&0xFF, fd);
}

void VFZIP_WriteUInt32(FILE *fd, int v)
{
	fputc(v&0xFF, fd);
	fputc((v>>8)&0xFF, fd);
	fputc((v>>16)&0xFF, fd);
	fputc((v>>24)&0xFF, fd);
}

u32 VFZIP_CRC32(void *buf, int sz, u32 lcrc)
{
	static u32 crctab[256];
	static int init=0;

	byte *s;
	u32 c;
	int i, j;

	if(!init)
	{
		init=1;

		for(i=0; i<256; i++)
		{
			c=i;
			for(j=0; j<8; j++)
				if(c&1)c=0xedb88320^(c>>1);
					else c>>=1;
			crctab[i]=c;
		}
	}

	c=lcrc; s=buf; i=sz;
	while(i--)c=crctab[(c^(*s++))&0xFF]^(c>>8);
	return(c);
}

int VFZIP_IsText(byte *buf, int sz)
{
	int i, j, k;

	k=1;
	for(i=0; i<sz; i++)
	{
		j=buf[i];

		if((j>=' ') && (j<='~'))continue;

		if(j=='\r')continue;
		if(j=='\n')continue;
		if(j=='\t')continue;

		if(j=='\a')continue;
		if(j=='\b')continue;
		if(j=='\v')continue;
		if(j=='\f')continue;

		k=0;
		break;
	}

	return(k);
}


VFZIP_Context *VFZIP_OpenZip(char *fname, int rrw)
{
	char name[256], nbuf[256];
	FILE *fd;
	VFZIP_Context *ctx;
	VFZIP_Entry *tmp, *lst, *cur, *root;
	VFZIP_Entry *dfst[16], *dstk[16];
	char *s, *t, *s2, *t2;

	int id, os, ver, fl, cm, crc;
	int csize, usize, offs, doffs, toffs;
	int nlen, elen, clen;
	int i, j;

	fd=fopen(fname, "rb");
	if(!fd)return(NULL);

	printf("reading zip %s %s\n", fname,
		rrw?"read-rewrite":"read-only");

	ctx=malloc(sizeof(VFZIP_Context));
	memset(ctx, 0, sizeof(VFZIP_Context));
	ctx->fd=fd;
	ctx->name=strdup(fname);

	lst=NULL;
	while(1)
	{
		offs=ftell(fd);
		id=VFZIP_ReadUInt32(fd);		//sig

		if(id==FCC_PK_FILE)
		{
			ver=VFZIP_ReadUInt16(fd);	//ver
			fl=VFZIP_ReadUInt16(fd);	//flags
			cm=VFZIP_ReadUInt16(fd);	//method
			VFZIP_ReadUInt16(fd);		//time
			VFZIP_ReadUInt16(fd);		//date
			crc=VFZIP_ReadUInt32(fd);	//CRC
			csize=VFZIP_ReadUInt32(fd);	//compressed size
			usize=VFZIP_ReadUInt32(fd);	//uncompressed size
			nlen=VFZIP_ReadUInt16(fd);	//name length
			elen=VFZIP_ReadUInt16(fd);	//extra length

//			printf("%d %d %d %d %d\n", ver, fl, cm, nlen, elen);

			memset(name, 0, 256);
			fread(name, 1, nlen, fd);
//			fseek(fd, nlen, 1);
			fseek(fd, elen, 1);

			doffs=ftell(fd);
			fseek(fd, csize, 1);

//			printf("%08X %s %d(%d)\n", offs, name, usize, csize);

			if(!(fl&8))continue;

			crc=VFZIP_ReadUInt32(fd);	//CRC
			csize=VFZIP_ReadUInt32(fd);	//compressed size
			usize=VFZIP_ReadUInt32(fd);	//uncompressed size
//			printf("desc %08X %d %d\n", crc, csize, usize);
			continue;
		}

		if(id==FCC_PK_DIRFILE)
		{
			os=VFZIP_ReadUInt16(fd);	//ver
			ver=VFZIP_ReadUInt16(fd);	//ver
			fl=VFZIP_ReadUInt16(fd);	//flags
			cm=VFZIP_ReadUInt16(fd);	//method
			VFZIP_ReadUInt16(fd);		//time
			VFZIP_ReadUInt16(fd);		//date
			crc=VFZIP_ReadUInt32(fd);	//CRC
			csize=VFZIP_ReadUInt32(fd);	//compressed size
			usize=VFZIP_ReadUInt32(fd);	//uncompressed size
			nlen=VFZIP_ReadUInt16(fd);	//name length
			elen=VFZIP_ReadUInt16(fd);	//extra length
			clen=VFZIP_ReadUInt16(fd);	//comment length

			VFZIP_ReadUInt16(fd);		//disk number
			VFZIP_ReadUInt16(fd);		//internal attr
			VFZIP_ReadUInt32(fd);		//external attr
			offs=VFZIP_ReadUInt32(fd);	//header offset

			memset(name, 0, 256);
			fread(name, 1, nlen, fd);
			fseek(fd, elen, 1);
			fseek(fd, clen, 1);

			s=name;
			while(*s)
			{
				if((*s>='A') && (*s<='Z'))
					*s=*s-'A'+'a';
				s++;
			}

//			printf("cdir file: %s @ %08X\n", name, offs);


			tmp=malloc(sizeof(VFZIP_Entry));
			memset(tmp, 0, sizeof(VFZIP_Entry));

			tmp->name=strdup(name);
			tmp->csize=csize;
			tmp->usize=usize;
			tmp->method=cm;


			toffs=ftell(fd);
			fseek(fd, offs, 0);
			id=VFZIP_ReadUInt32(fd);	//sig
			fseek(fd, 22, 1);

			nlen=VFZIP_ReadUInt16(fd);	//name length
			elen=VFZIP_ReadUInt16(fd);	//extra length
			fseek(fd, nlen, 1);
			fseek(fd, elen, 1);
			tmp->doffs=ftell(fd);

			if(rrw)
			{
//				printf("\treading: zip %s\n", tmp->name);
				VFZIP_ReadInFile(ctx, tmp);
			}

			if(lst && (strcmp(tmp->name, lst->name)>0))
			{
				cur=lst;
				while(cur->next &&
					(strcmp(tmp->name, cur->next->name)>0))
						cur=cur->next;
				tmp->next=cur->next;
				cur->next=tmp;
			}else
			{
				tmp->next=lst;
				lst=tmp;
			}

			i=0;
			s=tmp->name;
			while(*s)i=(i>>12)^(i<<3)^(*s++);
			i&=4095;
			tmp->hnext=ctx->hash[i];
			ctx->hash[i]=tmp;


			fseek(fd, toffs, 0);
			continue;
		}

		if(id==FCC_PK_DIREND)
		{
//			printf("end of central directory\n");

			VFZIP_ReadUInt16(fd);	//disk number
			VFZIP_ReadUInt16(fd);	//first disk of dir
			VFZIP_ReadUInt16(fd);	//files on disk
			VFZIP_ReadUInt16(fd);	//files total
			VFZIP_ReadUInt32(fd);	//size of directory
			VFZIP_ReadUInt32(fd);	//disk offset

			clen=VFZIP_ReadUInt16(fd);	//comment length
			fseek(fd, clen, 1);

			break;
		}

		printf("unknown record %08X\n", id);
		break;
	}


	memset(name, 0, 256);
	t=name;
	dfst[0]=NULL;
	dstk[0]=NULL;
	i=0;

	cur=lst;
	while(cur)
	{
		j=strlen(name);
		if(j && (strncmp(cur->name, name, j) || (cur->name[j]!='/')))
		{
//			printf("}\n");

			while((t>name) && (*t!='/'))t--;
			*t=0;
			i--;
			continue;
		}

		s=cur->name+j;
		while(*s=='/')s++;
		s2=s;
		while(*s && (*s!='/'))s++;
		if(*s=='/')
		{
			tmp=malloc(sizeof(VFZIP_Entry));
			memset(tmp, 0, sizeof(VFZIP_Entry));

			memset(nbuf, 0, 256);
			strncpy(nbuf, s2, s-s2);
			tmp->rname=strdup(nbuf);

			j=s-cur->name;
			memset(name, 0, 256);
			memcpy(name, cur->name, j);
			t=name+j;

			tmp->name=strdup(name);

			if(dfst[i])
			{
				dstk[i]->dnext=tmp;
				dstk[i]=tmp;
			}else
			{
				if(i)dstk[i-1]->dfirst=tmp;
				dfst[i]=tmp;
				dstk[i]=tmp;
			}

			i++;
			dfst[i]=NULL;
			dstk[i]=NULL;

//			printf("%s {\n", name);

			continue;
		}

		memset(nbuf, 0, 256);
		strncpy(nbuf, s2, s-s2);
		if(!strlen(nbuf)){cur=cur->next; continue;}

		cur->rname=strdup(nbuf);

//		printf("%s %d (@ %08X %d)\n", cur->rname,
//			cur->usize, cur->doffs, cur->csize);

		if(dfst[i])
		{
			dstk[i]->dnext=cur;
			dstk[i]=cur;
		}else
		{
			if(i)dstk[i-1]->dfirst=cur;
			dfst[i]=cur;
			dstk[i]=cur;
		}

		cur=cur->next;
	}

	root=dfst[0];

	ctx->first=lst;
	ctx->root=root;

	if(rrw)
	{
		fclose(fd);
		ctx->fd=NULL;
	}

	printf("reading zip %s: OK\n", fname);

	return(ctx);

#if 0
	i=0;
	cur=root;
	while(cur)
	{
		if(cur->dfirst)
		{
			for(j=0; j<i; j++)fputc('\t', stdout);
			printf("%s/\n", cur->rname);

			dstk[i]=cur->dnext;
			i++;

			cur=cur->dfirst;
			continue;
		}

		for(j=0; j<i; j++)fputc('\t', stdout);
		printf("%s\n", cur->rname);

		cur=cur->dnext;
		while(!cur && i)
		{
			i--;
			cur=dstk[i];
		}
	}
#endif
}

int VFZIP_WriteZip(VFZIP_Context *ctx, char *name)
{
	VFZIP_Entry *cur;
	FILE *fd;
	byte *cbuf;
	int doffs, dsize, dents;
	int i, j, k;

	printf("writing zip %s\n", name);

	fd=fopen(name, "wb");
	if(!fd)return(-1);

	//first pass: write out files
	cur=ctx->first;
	while(cur)
	{
		printf("	writing: %s\n", cur->name);

		cur->crc=~VFZIP_CRC32(cur->buffer, cur->usize, 0xFFFFFFFF);
//		cur->method=0;

		if(cur->method==0)
		{
			cbuf=malloc(cur->usize);
			memcpy(cbuf, cur->buffer, cur->usize);
			cur->csize=cur->usize;
		}
		if(cur->method==8)
		{
			cbuf=malloc(2*cur->usize);
			cur->csize=PDZ2_EncodeStream(cur->buffer, cbuf,
				cur->usize, 2*cur->usize);
		}

		cur->offs=ftell(fd);

		VFZIP_WriteUInt32(fd, FCC_PK_FILE);

		VFZIP_WriteUInt16(fd, 20);	//ver needed to extract
		VFZIP_WriteUInt16(fd, 0);		//flags
		VFZIP_WriteUInt16(fd, cur->method);	//method

		VFZIP_WriteUInt16(fd, 0);		//time
		VFZIP_WriteUInt16(fd, 0);		//date

		VFZIP_WriteUInt32(fd, cur->crc);	//CRC32
		VFZIP_WriteUInt32(fd, cur->csize);	//compressed size
		VFZIP_WriteUInt32(fd, cur->usize);	//uncompressed size

		i=strlen(cur->name);
		VFZIP_WriteUInt16(fd, i);		//name len
		VFZIP_WriteUInt16(fd, 0);		//extra len
		fwrite(cur->name, 1, i, fd);		//name

		cur->doffs=ftell(fd);
		fwrite(cbuf, 1, cur->csize, fd);	//data
		free(cbuf);

		cur=cur->next;
	}


	//second pass: write out central directory
	doffs=ftell(fd); dents=0;
	cur=ctx->first;
	while(cur)
	{
		VFZIP_WriteUInt32(fd, FCC_PK_DIRFILE);

		VFZIP_WriteUInt16(fd, 20);	//ver made by
		VFZIP_WriteUInt16(fd, 20);	//ver needed to extract
		VFZIP_WriteUInt16(fd, 0);		//flags
		VFZIP_WriteUInt16(fd, cur->method);	//method

		VFZIP_WriteUInt16(fd, 0);		//time
		VFZIP_WriteUInt16(fd, 0);		//date

		VFZIP_WriteUInt32(fd, cur->crc);	//CRC32
		VFZIP_WriteUInt32(fd, cur->csize);	//compressed size
		VFZIP_WriteUInt32(fd, cur->usize);	//uncompressed size

		i=strlen(cur->name);
		VFZIP_WriteUInt16(fd, i);		//name len
		VFZIP_WriteUInt16(fd, 0);		//extra len
		VFZIP_WriteUInt16(fd, 0);		//comment len

		j=(cur->name[i-1]=='/')?16:0;
		k=VFZIP_IsText(cur->buffer, cur->usize);
		VFZIP_WriteUInt16(fd, 0);		//disk start
		VFZIP_WriteUInt16(fd, k);		//internal attr
		VFZIP_WriteUInt32(fd, j);		//external attr

		VFZIP_WriteUInt32(fd, cur->offs);	//local header offs

		fwrite(cur->name, 1, i, fd);		//name

		dents++;
		cur=cur->next;
	}

	dsize=ftell(fd)-doffs;

	VFZIP_WriteUInt32(fd, FCC_PK_DIREND);
	VFZIP_WriteUInt16(fd, 0);		//this disk
	VFZIP_WriteUInt16(fd, 0);		//disk with dir start
	VFZIP_WriteUInt16(fd, dents);		//entries on disk
	VFZIP_WriteUInt16(fd, dents);		//entries total
	VFZIP_WriteUInt32(fd, dsize);		//directory size
	VFZIP_WriteUInt32(fd, doffs);		//directory offset

	VFZIP_WriteUInt16(fd, 0);		//comment length

	printf("writing zip %s: OK\n", name);
	return(0);
}


VFZIP_Entry *VFZIP_LocateFile(VFZIP_Context *ctx, char *name)
{
	VFZIP_Entry *cur, *lcur;
	char *s;
	int i;

	i=0;
	s=name;
	while(*s)i=(i>>12)^(i<<3)^(*s++);
	i&=4095;

	lcur=NULL;
	cur=ctx->hash[i];
	while(cur)
	{
		if(!strcmp(cur->name, name))
		{
			if(lcur)
			{
				lcur->hnext=cur->hnext;
				cur->hnext=ctx->hash[i];
				ctx->hash[i]=cur;
			}
			return(cur);
		}
		cur=cur->hnext;
	}
	return(NULL);
}

VFZIP_Entry *VFZIP_CreateFile(VFZIP_Context *ctx, char *name)
{
	VFZIP_Entry *cur, *lcur, *tmp;
	char *s;
	int i;

	tmp=malloc(sizeof(VFZIP_Entry));
	memset(tmp, 0, sizeof(VFZIP_Entry));

	tmp->name=strdup(name);
	tmp->csize=0;
	tmp->usize=0;
	tmp->method=8;

	cur=ctx->first; lcur=NULL;
	while(cur)
	{
		if(strcmp(cur->name, name)>0)
		{
			tmp->next=cur;
			if(lcur)lcur->next=tmp;
				else ctx->first=tmp;
			break;
		}
		if(!strcmp(cur->name, name))
		{
			tmp->next=cur->next;
			if(lcur)lcur->next=tmp;
				else ctx->first=tmp;
			break;
		}
		lcur=cur;
		cur=cur->next;
	}
	if(!cur)
	{
		if(lcur)lcur->next=tmp;
			else ctx->first=tmp;
	}

	i=0;
	s=name;
	while(*s)i=(i>>12)^(i<<3)^(*s++);
	i&=4095;

	lcur=NULL;
	cur=ctx->hash[i];
	while(cur)
	{
		if(!strcmp(cur->name, name))
		{
			if(lcur)lcur->hnext=cur->hnext;
				else ctx->hash[i]=cur->hnext;
			break;
		}
		cur=cur->hnext;
	}
	tmp->hnext=ctx->hash[i];
	ctx->hash[i]=tmp;

	return(tmp);
}

void VFZIP_ReadInFile(VFZIP_Context *ctx, VFZIP_Entry *tmp)
{
	byte *ibuf, *obuf;
	int i;

	if(tmp->buffer)return;

	fseek(ctx->fd, tmp->doffs, 0);
	ibuf=NULL; obuf=NULL;

	//store
	if(tmp->method==0)
	{
		obuf=malloc(tmp->usize);
		fread(obuf, 1, tmp->usize, ctx->fd);
	}

	//deflate and deflate64
	if((tmp->method==8) || (tmp->method==9))
	{
		ibuf=malloc(tmp->csize);
		obuf=malloc(tmp->usize);

		fread(ibuf, 1, tmp->csize, ctx->fd);
		i=PDUNZ_DecodeStream(ibuf, obuf, tmp->csize, tmp->usize);

		free(ibuf);
		if(i<0)
		{
			free(obuf);
			obuf=NULL;
		}
	}

	tmp->buffer=obuf;
	tmp->bsize=tmp->usize;
}

int VFZIP_UncacheFile(VFZIP_Context *ctx)
{
	VFZIP_Entry *cur, *lst, *best, *blst;


	//significantly idle files
	cur=ctx->croot; lst=NULL; best=NULL;
	while(cur)
	{
		if((cur->iocnt<64) && (!best || (cur->iocnt<=best->iocnt)))
			{ best=cur; blst=lst; }
		lst=cur; cur=cur->cnext;
	}

	if(!best)	//most idle closed file
	{
		cur=ctx->croot; lst=NULL;
		while(cur)
		{
			if(!cur->ucnt && (!best || (cur->iocnt<=best->iocnt)))
				{ best=cur; blst=lst; }
			lst=cur; cur=cur->cnext;
		}
	}
	if(!best)	//largest closed file
	{
		cur=ctx->croot; lst=NULL;
		while(cur)
		{
			if(!cur->ucnt && (!best || (cur->usize>=best->usize)))
				{ best=cur; blst=lst; }
			lst=cur; cur=cur->cnext;
		}
	}
	if(!best)	//largest file in general, ignoring huge ones
	{
		cur=ctx->croot; lst=NULL;
		while(cur)
		{
			if((cur->usize<VFZIP_CACHE_MAXCACHE) &&
				(!best || (cur->usize>=best->usize)))
					{ best=cur; blst=lst; }
			lst=cur; cur=cur->cnext;
		}
	}
	if(!best)	//find whatever is left
	{
		cur=ctx->croot; lst=NULL;
		while(cur)
		{
			if(!best || (cur->usize>=best->usize))
				{ best=cur; blst=lst; }
			lst=cur; cur=cur->cnext;
		}
	}

	if(best)
	{
		if(blst)blst->cnext=best->cnext;
			else ctx->croot=best->cnext;
		free(best->buffer);
		best->buffer=NULL;

		ctx->cache-=best->usize;

		//decrement io counts for remaining files
		cur=ctx->croot;
		while(cur)
		{
			cur->iocnt--;
			if(cur->iocnt<0)cur->iocnt=0;
			cur=cur->next;
		}

		return(0);
	}

	//by this point, there is nothing to free
	return(-1);
}

void VFZIP_CacheFile(VFZIP_Context *ctx, char *name)
{
	byte *ibuf, *obuf;
	VFZIP_Entry *tmp;
	int i;

	if(!ctx->fd)return;	//rrw mode

	tmp=VFZIP_LocateFile(ctx, name);
	if(!tmp || tmp->buffer)return;

	printf("ZIP: Cache File %s method %d\n", name, tmp->method);

	//large files are ignored as they will try to clear out
	//everything else
	if(tmp->usize<VFZIP_CACHE_MAXCACHE)
	{
		while((ctx->cache+tmp->usize)>=VFZIP_CACHE_MAX)
		{
			i=VFZIP_UncacheFile(ctx);
			if(i<0)break;
		}
	}
	ctx->cache+=tmp->usize;

	VFZIP_ReadInFile(ctx, tmp);

	if(tmp->buffer)
	{
		tmp->cnext=ctx->croot;
		ctx->croot=tmp;
	}
}

void VFZIP_ReCacheFile(VFZIP_Context *ctx, VFZIP_Entry *ent)
{
	if(ent->buffer)return;
	VFZIP_CacheFile(ctx, ent->name);
}


int vfzip_vfread(void *buf, int s1, int s2, VFILE *fd)
{
	VFZIP_FileCtx *ctx;
	int i;

	ctx=fd->data;
	i=s1*s2;
	if((ctx->offs+i)>ctx->ent->usize)
		i=ctx->ent->usize-ctx->offs;
	i/=s1;

	VFZIP_ReCacheFile(ctx->ctx, ctx->ent);
	ctx->ent->iocnt=256;

	memcpy(buf, ctx->ent->buffer+ctx->offs, i*s1);
	ctx->offs+=i*s1;
	return(i);
}

int vfzip_vfwrite(void *buf, int s1, int s2, VFILE *fd)
{
	VFZIP_FileCtx *ctx;
	int i;

	ctx=fd->data;
	ctx->ctx->dirty=1;

	i=s1*s2;
	if((ctx->offs+i)>ctx->ent->bsize)
	{
		ctx->ent->bsize=ctx->offs+i+4096;
		ctx->ent->bsize&=~4095;
		ctx->ent->buffer=realloc(ctx->ent->buffer, ctx->ent->bsize);
	}
	if((ctx->offs+i)>ctx->ent->usize)
		ctx->ent->usize=ctx->offs+i;

	memcpy(ctx->ent->buffer+ctx->offs, buf, i);
	ctx->offs+=i;
	return(i/s1);
}

int vfzip_vfgetc(VFILE *fd)
{
	VFZIP_FileCtx *ctx;
	int i;

	ctx=fd->data;

	if(ctx->offs>=ctx->ent->usize)
		return(-1);

	VFZIP_ReCacheFile(ctx->ctx, ctx->ent);
	ctx->ent->iocnt=256;

	i=ctx->ent->buffer[ctx->offs];
	ctx->offs++;
	return(i);
}

int vfzip_vfputc(int c, VFILE *fd)
{
	int i;
	byte b;

	b=c;
	vfzip_vfwrite(&b, 1, 1, fd);
	return(0);
}

int vfzip_vfflush(VFILE *fd)
{
	VFZIP_FileCtx *ctx;
	ctx=fd->data;

	//flushing means: rewrite archive...
	if(!ctx->ctx->fd && ctx->ctx->dirty)
	{
		VFZIP_WriteZip(ctx->ctx, ctx->ctx->name);
		ctx->ctx->dirty=0;
	}

	return(0);
}

vfoffs_t vfzip_vfseek(VFILE *fd, vfoffs_t rel, int pos)
{
	VFZIP_FileCtx *ctx;
	int i;

	ctx=fd->data;

	switch(pos)
	{
	case 0:
		ctx->offs=rel;
		break;
	case 1:
		ctx->offs+=rel;
		break;
	case 2:
		ctx->offs+=ctx->ent->usize+rel;
		break;
	default:
		break;
	}
	return(ctx->offs);
}

vfoffs_t vfzip_vftell(VFILE *fd)
{
	VFZIP_FileCtx *ctx;
	ctx=fd->data;
	return(ctx->offs);
}

int vfzip_vfeof(VFILE *fd)
{
	VFZIP_FileCtx *ctx;
	ctx=fd->data;
	if(ctx->offs>=ctx->ent->usize)
		return(1);
	return(0);
}

int vfzip_vfclose(VFILE *fd)
{
	VFZIP_FileCtx *ctx;
	ctx=fd->data;

	ctx->ent->ucnt--;
	if(ctx->ent->ucnt<0)
		ctx->ent->ucnt=0;
	gcfree(ctx);

	//inefficient: a file was modified, rewrite archive
	if(!ctx->ctx->fd && ctx->ctx->dirty)
	{
		VFZIP_WriteZip(ctx->ctx, ctx->ctx->name);
		ctx->ctx->dirty=0;
	}

	return(0);
}

VFILE_Interface vfzip_iface={
NULL,
"zipped_file",
vfzip_vfread,	//read
vfzip_vfwrite,	//write
vfzip_vfgetc,	//getc
vfzip_vfputc,	//putc
vfzip_vfflush,	//flush
vfzip_vfseek,	//seek
vfzip_vftell,	//tell
vfzip_vfeof,	//eof
vfzip_vfclose,	//close
NULL		//ioctl
};

VFILE *vfzip_vfopen_w(VFILE_Mount *mnt, char *name, char *mode)
{
	char buf[256];
	char *s;
	VFILE *tmp;
	VFZIP_Entry *ent;
	VFZIP_FileCtx *ctx;
	VFZIP_Context *cctx;
	int i;

	ent=NULL;	//fix warning

	ctx=gcalloc(sizeof(VFZIP_FileCtx));
	ctx->ctx=mnt->data;
	ctx->ent=ent;
	ctx->offs=0;

	cctx=mnt->data;
	cctx->dirty=1;
	ent=VFZIP_CreateFile(cctx, name);
	ent->ucnt++;

	ctx->ent=ent;	//? (fixing warnings, forgot what code does)

	tmp=vf_new();
	tmp->iface=&vfzip_iface;
	tmp->data=ctx;
	return(tmp);
}

VFILE *vfzip_vfopen(VFILE_Mount *mnt, char *name, char *mode)
{
	char buf[256];
	char *s;
	VFILE *tmp;
	VFZIP_Entry *ent;
	VFZIP_FileCtx *ctx;
	VFZIP_Context *cctx;
	int i;

	strcpy(buf, name);
	s=buf;
	while(*s)
	{
		if(*s=='\\')*s='/';
		if((*s>='A') && (*s<='Z'))*s=*s-'A'+'a';
		s++;
	}

	if(mode[0]=='w')
	{
		cctx=mnt->data;
		if(cctx->fd)return(NULL);	//not rrw
		tmp=vfzip_vfopen_w(mnt, buf, mode);
		return(tmp);
	}

	if(mode[1]=='+')
	{
		cctx=mnt->data;
		if(cctx->fd)return(NULL);	//not rrw
	}


	ent=VFZIP_LocateFile(mnt->data, name);
	VFZIP_CacheFile(mnt->data, name);
	if(!ent || !ent->buffer)return(NULL);

	ctx=gcalloc(sizeof(VFZIP_FileCtx));
	ctx->ctx=mnt->data;
	ctx->ent=ent;
	ctx->offs=0;

	ent->ucnt++;

	tmp=vf_new();
	tmp->iface=&vfzip_iface;
	tmp->data=ctx;
	return(tmp);
}

VFILE_Mount *vfzip_vfmount(VFILE_Mount_Interface *iface,
	char *src, char *dst, char **ops)
{
	VFILE_Mount *tmp;
	VFZIP_Context *ctx;
	int i, j, rrw;

	rrw=0;
	if(ops)
	{
		for(i=0; ops[i]; i++)
			if(!strcmp(ops[i], "rrw"))rrw=1;
	}


//	ctx=VFZIP_OpenZip(src, 1);
//	if(!ctx)return(NULL);
//	VFZIP_WriteZip(ctx, "resource/tst0.zip");

	ctx=VFZIP_OpenZip(src, rrw);
	if(!ctx)return(NULL);

	tmp=vf_new_mount();

	tmp->iface=iface;
	tmp->src=strdup(src);
	tmp->dst=strdup(dst);

	tmp->data=ctx;

	return(tmp);
}

VFILE_Mount_Interface vfzip_mnt_iface={
NULL,		//chain
"zip",		//name
vfzip_vfopen,	//open
vfzip_vfmount,	//mount
NULL,		//umount

NULL,		//opendir
NULL,		//closedir
NULL,		//readdir
NULL,		//statflags
NULL		//mkdir
};

void vfzip_init()
{
	vf_register_fs(&vfzip_mnt_iface);
}
