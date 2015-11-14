#include <btgecm.h>

BTGECM_Pack2State *btgecm_pack_root=NULL;

int btgecm_strcmp(const char *stra, const char *strb)
{
	const char *sa, *sb;
	
	sa=stra; sb=strb;
	while(*sa && *sb)
	{
		if(*sa!=*sb)break;
		sa++; sb++;
	}
	if(!(*sa) && !(*sb))
		return(0);
	return(((*sa)>(*sb))?(-1):1);
}

int btgecm_stricmp(const char *stra, const char *strb)
{
	const char *sa, *sb;
	int a, b;
	
	sa=stra; sb=strb;
	while(*sa && *sb)
	{
		a=*sa; b=*sb;
		if((a>='A') && (a<='Z'))a=(a-'A')+'a';
		if((b>='A') && (b<='Z'))b=(b-'A')+'a';
		if(a!=b)break;
		sa++; sb++;
	}
	if(!(*sa) && !(*sb))
		return(0);
	return(((*sa)>(*sb))?(-1):1);
}

s32 btgecm_get_le16(byte *b)
{
	return(b[0]|(b[1]<<8));
}

s32 btgecm_get_le32(byte *b)
{
	return(b[0]|(b[1]<<8)|(b[2]<<16)|(b[3]<<24));
}

s64 btgecm_get_le64(byte *b)
{
	s64 t;
	t=btgecm_get_le32(b);
	t+=((s64)btgecm_get_le32(b+4))<<32;
	return(t);
}

void btgecm_set_le16(byte *b, s16 v)
{
	b[0]=v&255;
	b[1]=(v>>8)&255;
}

void btgecm_set_le32(byte *b, s32 v)
{
	b[0]=v&255;
	b[1]=(v>>8)&255;
	b[2]=(v>>16)&255;
	b[3]=(v>>24)&255;
}

void btgecm_set_le64(byte *b, s64 v)
{
	b[0]=v&255;
	b[1]=(v>>8)&255;
	b[2]=(v>>16)&255;
	b[3]=(v>>24)&255;
	b[4]=(v>>32)&255;
	b[5]=(v>>40)&255;
	b[6]=(v>>48)&255;
	b[7]=(v>>56)&255;
}

int btgecm_get_parsehex(char *str)
{
	char *s;
	int i, j, k;
	s=str; k=0;

	while(*s)
	{
		j=*s++;
		if((j>='0') && (j<='9'))
			{ k=k*16+(j-'0'); continue; }
		if((j>='A') && (j<='F'))
			{ k=k*16+((j-'A')+10); continue; }
		if((j>='a') && (j<='f'))
			{ k=k*16+((j-'a')+10); continue; }
		break;
	}
	return(k);
}

BTGECM_Pack2State *BTGECM_LookupPack2(char *name)
{
	BTGECM_Pack2State *cur;

	cur=btgecm_pack_root;
	while(cur)
	{
		if(!btgecm_stricmp(cur->name, name))
			return(cur);
		cur=cur->next;
	}
	return(NULL);
}

int BTGECM_Pack2FRead(BTGECM_Pack2State *ctx,
	void *buf, s64 base, int sz)
{
	char tb[256];
	int fdn, fdn2;
	int i, j, k, l;
	
	fdn=(base>>30);
	fdn2=((base+sz)>>30);
	
	if(!ctx->fda[fdn])
	{
		sprintf(tb, "%s.S%d.pak2", ctx->name, fdn);
		if(ctx->readwrite)
			{ tmp->fda[fdn]=btgecm_fopen(tb, "r+b"); }
		else
			{ tmp->fda[fdn]=btgecm_fopen(tb, "rb"); }

		if(!ctx->fda[fdn])
		{
			return(0);
		}
	}

	if(fdn==fdn2)
	{
		btgecm_fseek(ctx->fda[fdn], (int)base, 0);
		i=btgecm_fread(buf, 1, sz, tmp->fda[fdn]);
		return(i);
	}
	
	j=(int)(base-(((s64)fdn)<<30));
	k=(1<<30)-j;

	btgecm_fseek(ctx->fda[fdn], (int)base, 0);
	i=btgecm_fread(buf, 1, k, tmp->fda[fdn]);
	j=BTGECM_Pack2FRead(ctx, ((byte *)buf)+k, base+k, sz-k);
	return(i);
}

int BTGECM_Pack2FWrite(BTGECM_Pack2State *ctx,
	void *buf, s64 base, int sz)
{
	char tb[256];
	int fdn, fdn2;
	int i, j, k, l;
	
	if(!ctx->readwrite)
		{ return(-1); }

	fdn=(base>>30);
	fdn2=((base+sz)>>30);
	
	if(!ctx->fda[fdn])
	{
		sprintf(tb, "%s.S%d.pak2", ctx->name, fdn);
		tmp->fda[fdn]=btgecm_fopen(tb, "r+b");
		if(!ctx->fda[fdn])
			{ tmp->fda[fdn]=btgecm_fopen(tb, "w+b"); }
		if(!ctx->fda[fdn])
			{ return(0); }
	}

	if(fdn==fdn2)
	{
		btgecm_fseek(ctx->fda[fdn], (int)base, 0);
		i=btgecm_fwrite(buf, 1, sz, tmp->fda[fdn]);
		return(i);
	}
	
	j=(int)(base-(((s64)fdn)<<30));
	k=(1<<30)-j;

	btgecm_fseek(ctx->fda[fdn], (int)base, 0);
	i=btgecm_fwrite(buf, 1, k, tmp->fda[fdn]);
	j=BTGECM_Pack2FWrite(ctx, ((byte *)buf)+k, base+k, sz-k);
	return(i);
}

BTGECM_Pack2State *BTGECM_OpenPack2RO(char *name)
{
	char tb[256];
	BTGECM_Pack2State *tmp;

	tmp=BTGECM_LookupPack2(name);
	if(tmp)return(tmp);

	tmp=btgecm_malloc(sizeof(BTGECM_Pack2State));
	memset(tmp, 0, sizeof(BTGECM_Pack2State));

	tmp->name=btgecm_strdup(name);
	tmp->readwrite=false;
	tmp->dirty=false;

	sprintf(tb, "%s.pak2", name);
	tmp->fda[0]=btgecm_fopen(tb, "rb");
	if(!tmp->fda[0])
	{
		btgecm_free(tmp);
		return(NULL);
	}

	btgecm_fread(&tmp->hdr, 1, sizeof(BTGECM_Pack2Head), tmp->fda[0]);

	tmp->doffs=btgecm_get_le64(tmp->hdr.offs);
	tmp->dents=btgecm_get_le32(tmp->hdr.ents);
	tmp->mdents=btgecm_get_le32(tmp->hdr.maxents);

	tmp->dir=btgecm_malloc(tmp->mdents*sizeof(BTGECM_Pack2DirEnt));
	BTGECM_Pack2FRead(tmp, tmp->dir, tmp->doffs,
		tmp->dents*sizeof(BTGECM_Pack2DirEnt));

//	btgecm_fseek(tmp->fd, tmp->doffs, 0);
//	btgecm_fread(tmp->dir, 64, tmp->dents, tmp->fd);

	tmp->next=btgecm_pack_root;
	btgecm_pack_root=tmp;

	return(tmp);
}

BTGECM_Pack2State *BTGECM_OpenPack2RW(char *name)
{
	char tb[256];
	BTGECM_Pack2State *tmp;

	tmp=BTGECM_LookupPack2(name);
	if(tmp)return(tmp);

	tmp=btgecm_malloc(sizeof(BTGECM_Pack2State));
	memset(tmp, 0, sizeof(BTGECM_Pack2State));

	tmp->name=btgecm_strdup(name);
	tmp->readwrite=true;
	tmp->dirty=false;

	sprintf(tb, "%s.pak2", name);
	tmp->fda[0]=btgecm_fopen(tb, "r+b");

	if(!tmp->fda[0])
	{
		tmp->fda[0]=btgecm_fopen(tb, "w+b");
		tmp->dirty=true;
	}

	if(!tmp->fda[0])
	{
		btgecm_free(tmp);
		return(NULL);
	}

	if(tmp->dirty)
	{
		strcpy(tmp->hdr.fcc, "PAK2");
		tmp->doffs=4096;
		tmp->dents=0;
		tmp->mdents=256;
		tmp->dir=btgecm_malloc(tmp->mdents*sizeof(BTGECM_Pack2DirEnt));
	}else
	{
//		btgecm_fread(&tmp->hdr, 1, sizeof(BTGECM_Pack2Head), tmp->fda[0]);
		BTGECM_Pack2FRead(tmp, &tmp->hdr, 0, sizeof(BTGECM_Pack2Head));

		tmp->doffs=btgecm_get_le64(tmp->hdr.offs);
		tmp->dents=btgecm_get_le32(tmp->hdr.ents);
		tmp->mdents=btgecm_get_le32(tmp->hdr.maxents);

		tmp->dir=btgecm_malloc(tmp->mdents*sizeof(BTGECM_Pack2DirEnt));
		BTGECM_Pack2FRead(tmp, tmp->dir, tmp->doffs,
			tmp->dents*sizeof(BTGECM_Pack2DirEnt));
	}

//	btgecm_fseek(tmp->fd, tmp->doffs, 0);
//	btgecm_fread(tmp->dir, 64, tmp->dents, tmp->fd);

	tmp->next=btgecm_pack_root;
	btgecm_pack_root=tmp;

	return(tmp);
}

int BTGECM_LookupPack2File(BTGECM_Pack2State *ctx, char *name,
	int *roffs, int *rsize)
{
	char tb[256];
	char *s;
	int i, j, k;

//	printf("Check Pack %s for %s\n", ctx->name, name);

	for(i=0; i<ctx->dents; i++)
	{
		s=ctx->dir[i].name;

		if((s[0]=='@') && (s[1]=='@') && (s[2]=='0') && (s[3]=='x'))
		{
			k=btgecm_get_parsehex(s+4);
			btgecm_fseek(ctx->fd, k, 0);
			btgecm_fread(tb, 1, 256, ctx->fd);
			s=tb;
		}

		if(btgecm_stricmp(s, name))
			continue;

//		if(btgecm_stricmp(ctx->dir[i].name, name))
//			continue;

//		j=*(int *)(ctx->dir[i].offs);
//		k=*(int *)(ctx->dir[i].size);
		j=btgecm_get_le32(ctx->dir[i].offs);
		k=btgecm_get_le32(ctx->dir[i].size);

		*roffs=j; *rsize=k;
		return(1);
	}

//	printf("Not Found %s for %s\n", ctx->name, name);

	return(0);
}

void *BTGECM_Pack2LookupFile(char *name, int *roffs, int *rsize)
{
	BTGECM_Pack2State *cur;
	int i;

	cur=btgecm_pack_root;
	while(cur)
	{
		i=BTGECM_LookupPack2File(cur, name, roffs, rsize);
		if(i>0)return(cur->fd);

		cur=cur->next;
	}
	return(NULL);
}

byte *BTGECM_Pack2ReadFile(char *name, int *rsize)
{
	void *fd;
	byte *buf;
	int offs, size;

	fd=BTGECM_Pack2LookupFile(name, &offs, &size);
	if(!fd)return(NULL);

//	printf("read %s %10d %d\n", name, offs, size);

	buf=btgecm_malloc(size+1);
	buf[size]=0;

	btgecm_fseek(fd, offs, 0);
	btgecm_fread(buf, 1, size, fd);

//	printf("%c%c%c%c\n", buf[0], buf[1], buf[2], buf[3]);

	if(rsize)*rsize=size;
	return(buf);
}

