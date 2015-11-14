#include <zpack.h>

ZPACK_API void ZPACK_CreateImage(char *name)
{
	FILE *fd;

	fd=fopen(name, "wb");

	ZPACK_WritePad(fd, 4096);

	ZPACK_WriteU32(fd, FCC_PK56);	//0, FCC
	ZPACK_WriteU16(fd, 0);	//4, disk number
	ZPACK_WriteU16(fd, 0);	//6, disk with start of central directory
	ZPACK_WriteU16(fd, 0);	//8, dir ents on disk
	ZPACK_WriteU16(fd, 0);	//10, dir ents in total

	ZPACK_WriteU32(fd, 0);		//12, dir size
	ZPACK_WriteU32(fd, 4096);	//16, dir offset
	ZPACK_WriteU16(fd, 0);		//20, comment length

	fclose(fd);
}

void ZPACK_ProcessDir_R(ZPACK_Context *ctx, int pos, char *path)
{
	char buf[256];
	ZPACK_CacheFile *ctmp;
	char *s, *t;
	int i, j, k, l;

	i=pos;
	while(1)
	{
		if(!ctx->root[i].name[0])
		{
			i=ctx->root[i].chain;
			if(!i)break;
			continue;
		}

		strcpy(buf, ctx->root[i].name);
		if(*path)sprintf(buf, "%s/%s", path, ctx->root[i].name);

		j=ctx->root[i].flags;

		if(j&2)		//dir
		{
			j=ctx->root[i].crc32;
			if(j)ZPACK_ProcessDir_R(ctx, j, buf);

			i=ctx->root[i].chain;
			if(!i)break;
			continue;
		}

		l=ctx->root[i].hoffs;
		k=ctx->root[i].hfsize;
		ZPACK_InsertSpan(ctx, l, k, ctx->root[i].name);

		i=ctx->root[i].chain;
		if(!i)break;
	}
}

int ZPACK_ReadHeader(ZPACK_Context *ctx)
{
	static byte buf[4096];
	int i, j, k;

	fseek(ctx->fd, 0, 2);
	ctx->eof=ftell(ctx->fd);

	if(ctx->eof>=4096)
	{
		fseek(ctx->fd, ctx->eof-4096, 0);
		fread(buf, 1, 4096, ctx->fd);
	}else
	{
		memset(buf, 0, 4096);
		fseek(ctx->fd, 0, 0);
		fread(buf+(4096-ctx->eof), 1, ctx->eof, ctx->fd);
	}

	for(i=4096-22; i>=0; i--)
	{
		j=buf[i]|(buf[i+1]<<8)|(buf[i+2]<<16)|(buf[i+3]<<24);
		if(j==FCC_PK56)break;
	}

	if(i<0)
	{
		printf("ZPACK: End Of Central Directory Record Not Found\n");
		return(-1);
	}

	j=ZPACK_GetU16(buf+i+4);
	k=ZPACK_GetU16(buf+i+6);

	if(j || k)
	{
		printf("ZPACK: Segmented archives not supported\n");
		return(-2);
	}

	j=ZPACK_GetU16(buf+i+8);
	k=ZPACK_GetU16(buf+i+10);
	if(j!=k)return(-2);

//	printf("End Of Central Directory Record @ %X\n", ctx->eof-4096+i);

	j=ZPACK_GetU16(buf+i+20);

	ctx->hoffs=ctx->eof-4096+i;
	ctx->hsz=22+j;

	ctx->doffs=ZPACK_GetU32(buf+i+16);
	ctx->dsize=ZPACK_GetU32(buf+i+12);
	ctx->dents=ZPACK_GetU16(buf+i+10);

	if(ctx->doffs>=ctx->eof)return(-1);
	if((ctx->doffs+ctx->dsize)>=ctx->eof)return(-1);

	printf("End Of Central Directory Record @ %X\n", ctx->hoffs);

	if((ctx->doffs+ctx->dsize)<ctx->hoffs)
		printf("Warn: %d bytes Between CDir and EOCD Marker\n",
			ctx->hoffs-(ctx->doffs+ctx->dsize));

	printf("Dir @ %X %d bytes, %d ents\n",
		ctx->doffs, ctx->dsize, ctx->dents);

	return(0);
}

int ZPACK_ReadDirectory(ZPACK_Context *ctx)
{
	char name[256], nbuf[256];

	int id, os, ver, fl, cm, crc, md, mt;
	int csize, usize, offs, doffs, toffs;
	int dn, dfn, dents, dsz;
	int nlen, elen, clen;
	int i, j;

	fseek(ctx->fd, ctx->doffs, 0);

	while(ftell(ctx->fd)<(ctx->doffs+ctx->dsize))
	{
		offs=ftell(ctx->fd);
		id=ZPACK_ReadU32(ctx->fd);		//0, sig

		if(id==FCC_PK12)
		{
			os=ZPACK_ReadU16(ctx->fd);	//4, ver
			ver=ZPACK_ReadU16(ctx->fd);	//6, ver
			fl=ZPACK_ReadU16(ctx->fd);	//8, flags
			cm=ZPACK_ReadU16(ctx->fd);	//10, method
			mt=ZPACK_ReadU16(ctx->fd);	//12, time
			md=ZPACK_ReadU16(ctx->fd);	//14, date
			crc=ZPACK_ReadU32(ctx->fd);	//16, CRC
			csize=ZPACK_ReadU32(ctx->fd);	//20, compressed size
			usize=ZPACK_ReadU32(ctx->fd);	//24, uncompressed size
			nlen=ZPACK_ReadU16(ctx->fd);	//28, name length
			elen=ZPACK_ReadU16(ctx->fd);	//30, extra length
			clen=ZPACK_ReadU16(ctx->fd);	//32, comment length

			ZPACK_ReadU16(ctx->fd);		//34, disk number
			ZPACK_ReadU16(ctx->fd);		//36, internal attr
			ZPACK_ReadU32(ctx->fd);		//38, external attr
			offs=ZPACK_ReadU32(ctx->fd);	//42, header offset

			memset(name, 0, 256);
			fread(name, 1, nlen, ctx->fd);
			fseek(ctx->fd, elen, 1);
			fseek(ctx->fd, clen, 1);

//			printf("cdir file: %s @ %08X\n", name, offs);

			if(name[strlen(name)-1]=='/')
				continue;

			i=ZPACK_CreateFile(ctx, name);

			ctx->root[i].csize=csize;
			ctx->root[i].usize=usize;
			ctx->root[i].method=cm;
			ctx->root[i].crc32=crc;
			ctx->root[i].hoffs=offs;

			ctx->root[i].date=md;
			ctx->root[i].time=mt;

			toffs=ftell(ctx->fd);

			fseek(ctx->fd, offs, 0);
			id=ZPACK_ReadU32(ctx->fd);	//sig
			fseek(ctx->fd, 22, 1);

			nlen=ZPACK_ReadU16(ctx->fd);	//name length
			elen=ZPACK_ReadU16(ctx->fd);	//extra length
			fseek(ctx->fd, nlen, 1);
			fseek(ctx->fd, elen, 1);

			ctx->root[i].hfsize=30+nlen+elen+csize;
			ctx->root[i].offset=ftell(ctx->fd);
			fseek(ctx->fd, toffs, 0);
			continue;
		}

//		printf("cdir sync @ %X\n", ftell(ctx->fd));
//		fseek(ctx->fd, -3, 1);

		printf("cdir bad @ %X\n", ftell(ctx->fd));
		break;
	}

}


int ZPACK_ReadDirBroken(ZPACK_Context *ctx)
{
	static char buf[4096];
	unsigned int l;
	int i, j, k;

	fseek(ctx->fd, 0, 0);
	ctx->dents=0;

	printf("ReadDirBroken\n");

	l=fgetc(ctx->fd);
	l|=fgetc(ctx->fd)<<8;
	l|=fgetc(ctx->fd)<<16;
	l|=fgetc(ctx->fd)<<24;
	while(!feof(ctx->fd))
	{
		if(l==0x04034b50)
		{
//			printf("local @ %X\n", ftell(ctx->fd));

//			fseek(ctx->fd, -4, 1);

			k=ftell(ctx->fd);

//			fseek(ctx->fd, 22, 1);

			j=ZPACK_ReadU16(ctx->fd);	//ver
			if(j>21)continue;

			j=ZPACK_ReadU16(ctx->fd);	//flag
			if(j&0xEFE9)continue;
			j=ZPACK_ReadU16(ctx->fd);	//cm
			if((j!=0) && (j!=8) && (j!=9))
				continue;
			j=ZPACK_ReadU16(ctx->fd);	//time
			j=ZPACK_ReadU16(ctx->fd);	//date
			j=ZPACK_ReadU32(ctx->fd);	//crc
			j=ZPACK_ReadU32(ctx->fd);	//csize
			j=ZPACK_ReadU32(ctx->fd);	//usize

			i=fgetc(ctx->fd);
			i|=fgetc(ctx->fd)<<8;

			if(!i)continue;
			if(i>=4096)continue;

			fgetc(ctx->fd);
			fgetc(ctx->fd);

			memset(buf, 0, 4096);
			fread(buf, 1, i, ctx->fd);
//			printf("%s\n", buf);

			for(j=0; j<i; j++)
				if(buf[j]<' ')break;
			if(j<i)continue;

			fseek(ctx->fd, k, 0);


			if(buf[strlen(buf)-1]=='/')
				continue;

			i=ZPACK_CreateFile(ctx, buf);

			ctx->root[i].hoffs=k-4;

//			ZPACK_ReadU32(ctx->fd);	//sig
//			fseek(ctx->fd, 22, 1);

			ZPACK_ReadU16(ctx->fd);		//ver
			ZPACK_ReadU16(ctx->fd);		//flag
			j=ZPACK_ReadU16(ctx->fd);	//cm
			ctx->root[i].method=j;
			j=ZPACK_ReadU16(ctx->fd);	//time
			ctx->root[i].time=j;
			j=ZPACK_ReadU16(ctx->fd);	//date
			ctx->root[i].date=j;
			j=ZPACK_ReadU32(ctx->fd);	//crc
			ctx->root[i].crc32=j;
			j=ZPACK_ReadU32(ctx->fd);	//csize
			ctx->root[i].csize=j;
			j=ZPACK_ReadU32(ctx->fd);	//usize
			ctx->root[i].usize=j;

			j=ZPACK_ReadU16(ctx->fd);	//name length
			k=ZPACK_ReadU16(ctx->fd);	//extra length
			fseek(ctx->fd, j, 1);
			fseek(ctx->fd, k, 1);

			ctx->root[i].hfsize=30+j+k+ctx->root[i].csize;
			ctx->root[i].offset=ftell(ctx->fd);

			fseek(ctx->fd, ctx->root[i].csize, 1);

			ctx->dents++;
			l=ZPACK_ReadU32(ctx->fd);
			continue;

		}
		l=(l>>8)|(fgetc(ctx->fd)<<24);
	}

	printf("ReadDirBroken: OK\n");
}


ZPACK_API ZPACK_Context *ZPACK_OpenImage(char *name, int fl)
{
	FILE *fd;
	ZPACK_Context *ctx;
	u64 li;
	int i, j, k;

	fd=fopen(name, (fl&1)?"rb":"r+b");
	if(!fd)
	{
		if(!(fl&1))
		{
			ZPACK_CreateImage(name);
			fd=fopen(name, (fl&1)?"rb":"r+b");
		}
		if(!fd)
			return(NULL);
	}

	ctx=malloc(sizeof(ZPACK_Context));
	memset(ctx, 0, sizeof(ZPACK_Context));

	ctx->name=strdup(name);
	ctx->fd=fd;
	ctx->flags=fl;


	printf("Open Image %s\n", (fl&1)?"R/O":"R/W");

	i=ZPACK_ReadHeader(ctx);

#if 1
	if(i<-1)	//non-recoverable (eg: segmented file, ...)
	{
		fclose(ctx->fd);
		free(ctx);
		return(NULL);
	}
#endif

	if(i<0)
	{
		ctx->ents=4096;
		ctx->root=malloc(ctx->ents*sizeof(ZPACK_DirEntry));
		memset(ctx->root, 0, ctx->ents*sizeof(ZPACK_DirEntry));

		ZPACK_ReadDirBroken(ctx);

		if(!ctx->dents)
		{
			printf("No ZIP Local Headers\n");
			free(ctx->root);
			fclose(ctx->fd);
			free(ctx);
			return(NULL);
		}

		ZPACK_InsertSpan(ctx, 0, 4, "$special");
		ZPACK_ProcessDir_R(ctx, 0, "");

		return(ctx);
	}


	ctx->ents=4096;
	while(ctx->ents<(2*ctx->dents))
		ctx->ents=ctx->ents+(ctx->ents>>1);

	ctx->root=malloc(ctx->ents*sizeof(ZPACK_DirEntry));
	memset(ctx->root, 0, ctx->ents*sizeof(ZPACK_DirEntry));

//	fseek(fd, ctx->offs, 0);
//	fread(ctx->root, sizeof(struct ZPACK_DirEntry_ds), ctx->ents, fd);

	ZPACK_ReadDirectory(ctx);

	ZPACK_InsertSpan(ctx, 0, 4, "$special");

	ZPACK_ProcessDir_R(ctx, 0, "");

	i=4096;
	while(i<=ctx->dsize)i=i+(i>>1);
	j=ZPACK_FinalSpanEnd(ctx);
	k=ctx->hoffs-i;
	if(k>j)j=k;

	ctx->doffs2=j;
	ctx->dsize2=ctx->hoffs-j;

//	ZPACK_InsertSpan(ctx, ctx->doffs2, ctx->dsize2, "$root");
//	ZPACK_InsertSpan(ctx, ctx->hoffs, ctx->hsz, "$head");

	//trash old header
	if(!(ctx->flags&1))
	{
		fseek(fd, ctx->hoffs, 0);
		ZPACK_WriteU32(fd, 0);
	}

#if 0
	//spans for header and root
	ZPACK_InsertSpan(ctx, 0, 4, "$special");

	ZPACK_InsertSpan(ctx, ctx->doffs, ctx->dsize, "$root");
	ZPACK_InsertSpan(ctx, ctx->hoffs, ctx->hsz, "$head");

	//process root to rebuild spans and such
	ZPACK_ProcessDir_R(ctx, 0, "");

	i=4096;
	while(i<=ctx->dsize)i=i+(i>>1);
//	ctx->dsize2=ZPACK_CheckPadSpan(ctx, ctx->doffs, i);

	ctx->doffs2=(ctx->doffs+ctx->dsize)-i;
	j=ZPACK_PrevSpanEnd(ctx, ctx->doffs);
	if(j>ctx->doffs2)
	{
		ctx->doffs2=j;
		ctx->dsize2=ZPACK_CheckPadSpan(ctx, ctx->doffs2, i);
	}

//	ZPACK_DeleteSpan(ctx, ctx->doffs);
	ZPACK_InsertSpan(ctx, ctx->doffs2, ctx->dsize2, "$root");
#endif

	return(ctx);
}


int ZPACK_SizeDirectory_R(ZPACK_Context *ctx, int pos, char *path)
{
	char buf[256];
	char *s, *t;
	int i, j, k, l, sz;

	i=pos; sz=0;
	while(1)
	{
		if(!ctx->root[i].name[0])
		{
			i=ctx->root[i].chain;
			if(!i)break;
			continue;
		}

		strcpy(buf, ctx->root[i].name);
		if(*path)sprintf(buf, "%s/%s", path, ctx->root[i].name);

		j=ctx->root[i].flags;

		if(j&2)		//dir
		{
			j=ctx->root[i].crc32;
			if(j)sz+=ZPACK_SizeDirectory_R(ctx, j, buf);

			i=ctx->root[i].chain;
			if(!i)break;
			continue;
		}

		sz+=46;
		sz+=strlen(buf);

		i=ctx->root[i].chain;
		if(!i)break;
	}
	return(sz);
}

int ZPACK_WriteDirectory_R(ZPACK_Context *ctx, int pos, char *path)
{
	char buf[256];
	char *s, *t;
	int i, j, k, l;

	i=pos; l=0;
	while(1)
	{
		if(!ctx->root[i].name[0])
		{
			i=ctx->root[i].chain;
			if(!i)break;
			continue;
		}

		strcpy(buf, ctx->root[i].name);
		if(*path)sprintf(buf, "%s/%s", path, ctx->root[i].name);

		j=ctx->root[i].flags;

		if(j&2)		//dir
		{
			j=ctx->root[i].crc32;
			if(j)l+=ZPACK_WriteDirectory_R(ctx, j, buf);

			i=ctx->root[i].chain;
			if(!i)break;
			continue;
		}

		j=strlen(buf);
		k=10;
		if(ctx->root[i].method==8)k=20;
		if(ctx->root[i].method==9)k=21;

		ZPACK_WriteU32(ctx->fd, FCC_PK12);
		ZPACK_WriteU16(ctx->fd, 21);	//ver made by
		ZPACK_WriteU16(ctx->fd, k);	//ver extract
		ZPACK_WriteU16(ctx->fd, 0);	//flags
		ZPACK_WriteU16(ctx->fd, ctx->root[i].method);
		ZPACK_WriteU16(ctx->fd, ctx->root[i].time);	//time
		ZPACK_WriteU16(ctx->fd, ctx->root[i].date);	//date
		ZPACK_WriteU32(ctx->fd, ctx->root[i].crc32);
		ZPACK_WriteU32(ctx->fd, ctx->root[i].csize);
		ZPACK_WriteU32(ctx->fd, ctx->root[i].usize);
		ZPACK_WriteU16(ctx->fd, j);	//name len
		ZPACK_WriteU16(ctx->fd, 0);	//extra len
		ZPACK_WriteU16(ctx->fd, 0);	//comment len
		ZPACK_WriteU16(ctx->fd, 0);	//start disk number
		ZPACK_WriteU16(ctx->fd, 0);	//internal attr
		ZPACK_WriteU32(ctx->fd, 0);	//external attr
		ZPACK_WriteU32(ctx->fd, ctx->root[i].hoffs);

		fwrite(buf, 1, j, ctx->fd);

		l++;
		i=ctx->root[i].chain;
		if(!i)break;
	}
	return(l);
}

#if 0
int ZPACK_CommitRoot(ZPACK_Context *ctx)
{
	int i, j;

	i=ZPACK_SizeDirectory_R(ctx, 0, "");
//	j=4096;
//	while(j<=i)j=j+(j>>1);

	ZPACK_DeleteSpan(ctx, ctx->hoffs);
	ZPACK_WriteZeroSpan(ctx->fd, ctx->hoffs, ctx->hsz);

	if(!ZPACK_CheckSpan(ctx, ctx->doffs, i))
	{
		ZPACK_WriteZeroSpan(ctx->fd, ctx->doffs, ctx->dsize);

		ZPACK_DeleteSpan(ctx, ctx->doffs);
		ctx->doffs=ZPACK_FindMinSpan(ctx, i);
		if((ctx->doffs+i)>ctx->eof)ctx->eof=ctx->doffs+i;
	}
	ctx->dsize=i;
//	ctx->dsize2=j;

	ZPACK_InsertSpan(ctx, ctx->doffs, ctx->dsize, "$root");

	fseek(ctx->fd, ctx->doffs, 0);
	i=ZPACK_WriteDirectory_R(ctx, 0, "");
	ctx->dents=i;

	if(ctx->eof>(ctx->hoffs+ctx->hsz))
//	if(1)
	{
//		ZPACK_WriteZeroSpan(ctx->fd, ctx->hoffs, ctx->hsz);
//		ZPACK_DeleteSpan(ctx, ctx->hoffs);
		ctx->hoffs=ctx->eof;
		ctx->hsz=22;
	}
	ZPACK_InsertSpan(ctx, ctx->hoffs, ctx->hsz, "$head");

	fseek(ctx->fd, ctx->hoffs, 0);
	ZPACK_WriteU32(ctx->fd, FCC_PK56);
	ZPACK_WriteU16(ctx->fd, 0);	//disk num
	ZPACK_WriteU16(ctx->fd, 0);	//disk num (dir start)
	ZPACK_WriteU16(ctx->fd, ctx->dents);	//ents on disk
	ZPACK_WriteU16(ctx->fd, ctx->dents);	//ents total
	ZPACK_WriteU32(ctx->fd, ctx->dsize);	//dir size
	ZPACK_WriteU32(ctx->fd, ctx->doffs);	//dir offset
	ZPACK_WriteU16(ctx->fd, 0);	//comment len


	return(0);
}
#endif

int ZPACK_CommitRoot(ZPACK_Context *ctx)
{
	int i, j, k;

	i=ZPACK_SizeDirectory_R(ctx, 0, "")+22;
	j=4096;
	while(j<=i)j=j+(j>>1);

//	ZPACK_WriteZeroSpan(ctx->fd, ctx->hoffs, ctx->hsz);
//	ZPACK_DeleteSpan(ctx, ctx->doffs);
//	ZPACK_DeleteSpan(ctx, ctx->doffs2);
//	ZPACK_DeleteSpan(ctx, ctx->hoffs);

//	k=ZPACK_PrevSpanEnd(ctx, ctx->eof+1);
	k=ZPACK_FinalSpanEnd(ctx);

	printf("EOF %X last %X\n", ctx->eof, k);

	if(ctx->eof<k)ctx->eof=k;
	if((ctx->eof-k)<i)
		ctx->eof=k+j;

	ZPACK_WritePadFile(ctx->fd, ctx->eof);
	fflush(ctx->fd);

	ctx->hoffs=ctx->eof-22;
	ctx->hsz=22;

	ctx->doffs=ctx->hoffs-(i-22);
	ctx->dsize=i-22;

	ctx->dsize2=ctx->hoffs-k;
	if(j<ctx->dsize2)ctx->dsize2=j;
	ctx->doffs2=ctx->hoffs-ctx->dsize2;


	printf("CDIR @ %X %d bytes\n", ctx->doffs, ctx->dsize);

//	ZPACK_InsertSpan(ctx, ctx->doffs2, ctx->dsize2, "$root");
//	ZPACK_InsertSpan(ctx, ctx->hoffs, ctx->hsz, "$head");


	fseek(ctx->fd, ctx->doffs, 0);
	i=ZPACK_WriteDirectory_R(ctx, 0, "");
	ctx->dents=i;

	fseek(ctx->fd, ctx->hoffs, 0);
	ZPACK_WriteU32(ctx->fd, FCC_PK56);
	ZPACK_WriteU16(ctx->fd, 0);	//disk num
	ZPACK_WriteU16(ctx->fd, 0);	//disk num (dir start)
	ZPACK_WriteU16(ctx->fd, ctx->dents);	//ents on disk
	ZPACK_WriteU16(ctx->fd, ctx->dents);	//ents total
	ZPACK_WriteU32(ctx->fd, ctx->dsize);	//dir size
	ZPACK_WriteU32(ctx->fd, ctx->doffs);	//dir offset
	ZPACK_WriteU16(ctx->fd, 0);	//comment len

	return(0);
}

ZPACK_API int ZPACK_CommitImage(ZPACK_Context *ctx)
{
	if(ctx->flags&1)return;
	ZPACK_CommitCacheAll(ctx);
//	ZPACK_CommitRoot(ctx);
	fflush(ctx->fd);
	return(0);
}

ZPACK_API int ZPACK_CloseImage(ZPACK_Context *ctx)
{
	if(!(ctx->flags&1))
	{
		ZPACK_CommitCacheAll(ctx);
		ZPACK_CommitRoot(ctx);
	}
	fclose(ctx->fd);
	return(0);
}

int ZPACK_FindFreeEntry(ZPACK_Context *ctx, int last)
{
	int i, j;

	for(i=last; i<ctx->ents; i++)
	{
		if(!ctx->root[i].name[0])
			return(i);
	}
	for(i=1; i<last; i++)
	{
		if(!ctx->root[i].name[0])
			return(i);
	}

	i=ctx->ents;
//	ctx->ents=(ctx->ents+96)&(~63);
	ctx->ents=ctx->ents+(ctx->ents>>1);

	printf("Expand Root %d->%d\n", i, ctx->ents);

	ctx->root=realloc(ctx->root,
		ctx->ents*sizeof(ZPACK_DirEntry));
	memset(ctx->root+i, 0,
		(ctx->ents-i)*sizeof(ZPACK_DirEntry));
	return(i);
}

char *ZPACK_ParseName(char *s, char *obuf)
{
	static char *b36="0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ";
	char buf[256];
	char *t, *s2, *t2;
	int i, h;

	t=buf;
	while(*s && (*s!='/') && (*s!=':'))*t++=*s++;
	*t++=0;
//	while(*s && (*s=='/'))s++;

//	if(strlen(buf)<32)
	if(strlen(buf)<64)
	{
		strcpy(obuf, buf);
		return(s);
	}

	//name exceeds limit, so mangle
	s2=buf; h=0;
	while(*s2)
	{
		i=*s2++;
		if((i>='A') && (i<='Z'))i=i-'A'+'a';
		h=(h>>20)^(h<<3)^i;
	}
	h&=0xFFFFF;

	s2=buf+strlen(buf);
	while((s2>buf) && (*s2!='.'))s2--;
	if((s2<=buf) || (strlen(s2)>8))s2=NULL;

//	i=32-6;
	i=64-6;
	if(s2)i-=strlen(s2);
	t=buf; t2=obuf;
	while(i--)*t2++=*t++;

	*t2++='~';
	*t2++=b36[(h/46656)%36];
	*t2++=b36[(h/1296)%36];
	*t2++=b36[(h/36)%36];
	*t2++=b36[h%36];

	if(s2)strcpy(t2, s2);

	return(s);
}

int ZPACK_LocateFile_R(ZPACK_Context *ctx, int pos, char *name)
{
	char buf[64];
	char *s, *t;
	int i, j;

//	printf("LookupR %s\n", name);

	s=ZPACK_ParseName(name, buf);
	while(*s && (*s=='/'))s++;

	i=pos;
	while(1)
	{
//		printf("LStep %d: %s\n", i, ctx->root[i].name);

		if(zpack_stricmp(ctx->root[i].name, buf))
		{
			i=ctx->root[i].chain;
			if(!i) { i=-1; break; }
			continue;
		}
		break;
	}

	if(i<0)return(i);	//not found

//	printf("Check %d: %s\n", i, ctx->root[i].name);

	if(!(*s))return(i);	//found

	j=ctx->root[i].flags;
	if(!(j&2))return(-1);

	j=ctx->root[i].crc32;
	if(!j)return(-1);	//dir is empty

	j=ZPACK_LocateFile_R(ctx, j, s);
	return(j);
}

int ZPACK_LocateFile(ZPACK_Context *ctx, char *name)
{
	char *s;
	int i;

//	printf("Locate %s\n", name);

	s=name;
	while(*s && (*s=='/'))s++;
	if(!(*s))return(-1);
	i=ZPACK_LocateFile_R(ctx, 0, s);
	return(i);
}

int ZPACK_CreateFile_R(ZPACK_Context *ctx, int pos, char *name)
{
	char buf[64];
	char *s, *t;
	int i, j, k, sd;

//	printf("CreateR %s\n", name);

	s=ZPACK_ParseName(name, buf);

	sd=(*s=='/')?1:0;
	while(*s && (*s=='/'))s++;

	i=pos; j=0;
	while(1)
	{
//		printf("Step %d: %s\n", i, ctx->root[i].name);
		if(zpack_stricmp(ctx->root[i].name, buf))
		{
			j=ctx->root[i].chain;
			if(!j) { j=-1; break; }
			i=j;
			continue;
		}
		break;
	}

	if(j<0)	//not found, create
	{
//		printf("Create %s\n", buf);

		j=ZPACK_FindFreeEntry(ctx, i?i:1);
		memset(&ctx->root[j], 0, sizeof(ZPACK_DirEntry));
		strcpy(ctx->root[j].name, buf);

		if(i!=j)ctx->root[i].chain=j;

		if(*s || sd)ctx->root[j].flags=2;
		if(*s)
		{
			k=ZPACK_FindFreeEntry(ctx, j);
			ctx->root[j].crc32=k;

			k=ZPACK_CreateFile_R(ctx, k, s);
			return(k);
		}

//		printf("Create %s: OK\n", buf);

		return(j);
	}

	if(!(*s))	//found, pre-existing
	{
		//need to destroy any caches and truncate file

		k=ctx->root[i].offset;
		ZPACK_DeleteSpan(ctx, k);

		ctx->root[i].flags=0;
		ctx->root[i].method=0;
		ctx->root[i].crc32=0;
		ctx->root[i].usize=0;
		ctx->root[i].csize=0;
		ctx->root[i].offset=0;

		ctx->root[i].date=0;
		ctx->root[i].time=0;

		return(i);
	}

	//something was found, verify if it is a directory
	j=ctx->root[i].flags;
	if(!(j&2))return(-1);

	//sub directory
	j=ctx->root[i].crc32;
	if(!j)
	{
		//dir empty, create contents
		j=ZPACK_FindFreeEntry(ctx, i);
		ctx->root[i].crc32=j;
	}

	j=ZPACK_CreateFile_R(ctx, j, s);
	return(j);
}

int ZPACK_CreateFile(ZPACK_Context *ctx, char *name)
{
	char *s;
	int i;

	s=name;
	while(*s && (*s=='/'))s++;
	if(!(*s))return(-1);
	i=ZPACK_CreateFile_R(ctx, 0, s);
	return(i);
}

int ZPACK_LocateFork(ZPACK_Context *ctx, int pos, char *name)
{
	char *s, *t;
	int i;

	s=name;
	i=ctx->root[pos].chain;
	while(1)
	{
		t=ctx->root[i].name;
		if((t[0]!='$') || (t[1]!=':'))
			break;
		if(!zpack_stricmp(t+2, s))break;

		i=ctx->root[i].chain;
		if(!i) { i=-1; break; }
	}
	return(i);
}

int ZPACK_CreateFork(ZPACK_Context *ctx, int pos, char *name)
{
	char *s, *t;
	int i, j;

	i=ZPACK_LocateFork(ctx, pos, name);
	if(i<=0)
	{
		i=ZPACK_FindFreeEntry(ctx, pos);
		memset(&ctx->root[i], 0, sizeof(ZPACK_DirEntry));

		j=ctx->root[pos].chain;
		ctx->root[i].chain=j;
		ctx->root[pos].chain=i;
	}else
	{
		j=ctx->root[i].offset;
		ZPACK_DeleteSpan(ctx, j);
	}

//	memset(ctx->root[i].name, 0, 32);
	memset(ctx->root[i].name, 0, 64);
	sprintf(ctx->root[i].name, "$:%s", name);

	ctx->root[i].flags=0;
	ctx->root[i].method=0;
	ctx->root[i].crc32=0;
	ctx->root[i].usize=0;
	ctx->root[i].csize=0;
	ctx->root[i].offset=0;

	return(i);
}

int ZPACK_DeleteFile_R(ZPACK_Context *ctx, int pos, int up, char *name)
{
	char buf[64];
	char *s, *t;
	int i, j, k;

	s=ZPACK_ParseName(name, buf);
	while(*s && (*s=='/'))s++;

	i=pos; j=-1;
	while(1)
	{
		if(zpack_stricmp(ctx->root[i].name, buf))
		{
			j=i;
			i=ctx->root[i].chain;
			if(!i) { i=-1; break; }
			continue;
		}
		break;
	}

	if(i<0)return(-1);	//not found

	if(!(*s))	//found
	{
		k=ctx->root[i].offset;
		ZPACK_DeleteSpan(ctx, k);

		if(j>=0)
		{
			//not first item, so unlink
			k=ctx->root[i].chain;
			ctx->root[j].chain=k;
			memset(&ctx->root[i], 0,
				sizeof(ZPACK_DirEntry));
		}else if(up>=0)
		{
			//first item, but a parent exists
			k=ctx->root[i].chain;
			ctx->root[up].crc32=k;
			memset(&ctx->root[i], 0,
				sizeof(ZPACK_DirEntry));
		}else
		{
			//first entry of root, be more careful
//			memset(&ctx->root[i].name, 0, 32);
			memset(&ctx->root[i].name, 0, 64);

			ctx->root[i].flags=0;
			ctx->root[i].method=0;
			ctx->root[i].crc32=0;
			ctx->root[i].usize=0;
			ctx->root[i].csize=0;
			ctx->root[i].offset=0;
		}

		return(i);
	}

	//something was found, verify if it is a directory
	j=ctx->root[i].flags;
	if(!(j&2))return(-1);

	//step into sub-directory
	j=ctx->root[i].crc32;
	if(!j)return(-1);

	j=ZPACK_DeleteFile_R(ctx, j, i, s);
	return(j);
}

ZPACK_API int ZPACK_DeleteFile(ZPACK_Context *ctx, char *name)
{
	char *s;
	int i;

	s=name;
	while(*s && (*s=='/'))s++;
	if(!(*s))return(-1);

	ZPACK_DropCache(ctx, s);

	i=ZPACK_DeleteFile_R(ctx, 0, -1, s);
	return(i);
}


ZPACK_API int ZPACK_StatFile(ZPACK_Context *ctx, char *name, ZPACK_STAT *stat)
{
	ZPACK_CacheFile *cur;
	int i, j, k;

	i=ZPACK_LocateFile(ctx, name);
	if(i<0)return(-1);

	j=ctx->root[i].date;
	k=ctx->root[i].time;

	stat->date_time=(j<<16)+k;
	stat->flags=ctx->root[i].flags;
	stat->method=ctx->root[i].method;
	stat->crc32=ctx->root[i].crc32;
	stat->usize=ctx->root[i].usize;
	stat->csize=ctx->root[i].csize;
	stat->offset=ctx->root[i].offset;

	return(0);
}

ZPACK_API int ZPACK_SetFileTime(ZPACK_Context *ctx, char *name, u32 t)
{
	int i;

	i=ZPACK_LocateFile(ctx, name);
	if(i<0)return(-1);
	ctx->root[i].date=(t>>16);
	ctx->root[i].time=t&0xFFFF;
	return(0);
}
