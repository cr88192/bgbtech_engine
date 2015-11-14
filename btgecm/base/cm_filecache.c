/*
Client Side File Caching
*/

#include <btgecm.h>

BTGE_CachedFile *btgecm_filecache_general=NULL;
BTGE_CachedFile *btgecm_filecache_client=NULL;
char *btgecm_filecache_cl_servername;

#if 1
void BTGECM_DataAdler32_Step16(byte *buf, int *rs1, int *rs2)
{
	byte *cs;
	int s1, s2;

#if 0
	s1=*rs1; s2=*rs2; cs=buf;
	s1+=*cs++; s2+=s1;	s1+=*cs++; s2+=s1;
	s1+=*cs++; s2+=s1;	s1+=*cs++; s2+=s1;
	s1+=*cs++; s2+=s1;	s1+=*cs++; s2+=s1;
	s1+=*cs++; s2+=s1;	s1+=*cs++; s2+=s1;
	s1+=*cs++; s2+=s1;	s1+=*cs++; s2+=s1;
	s1+=*cs++; s2+=s1;	s1+=*cs++; s2+=s1;
	s1+=*cs++; s2+=s1;	s1+=*cs++; s2+=s1;
	s1+=*cs++; s2+=s1;	s1+=*cs++; s2+=s1;
	*rs1=s1; *rs2=s2;
#endif

#if 1
	s1=*rs1; s2=*rs2;
	s1+=buf[ 0]; s2+=s1;	s1+=buf[ 1]; s2+=s1;
	s1+=buf[ 2]; s2+=s1;	s1+=buf[ 3]; s2+=s1;
	s1+=buf[ 4]; s2+=s1;	s1+=buf[ 5]; s2+=s1;
	s1+=buf[ 6]; s2+=s1;	s1+=buf[ 7]; s2+=s1;
	s1+=buf[ 8]; s2+=s1;	s1+=buf[ 9]; s2+=s1;
	s1+=buf[10]; s2+=s1;	s1+=buf[11]; s2+=s1;
	s1+=buf[12]; s2+=s1;	s1+=buf[13]; s2+=s1;
	s1+=buf[14]; s2+=s1;	s1+=buf[15]; s2+=s1;
	*rs1=s1; *rs2=s2;
#endif
}

BTGE_API u32 BTGECM_DataAdler32(void *buf, int sz, u32 lcrc)
{
	byte *s;
	u32 s1, s2;
	int n, l;

	s=(byte *)buf;
	l=sz;
	s1=lcrc&0xFFFF;
	s2=(lcrc>>16)&0xFFFF;

	while(l>=5552)
	{
		n=5552/16;
		while(n--)
		{
			BTGECM_DataAdler32_Step16(s, &s1, &s2);
			s+=16;
		}
		s1=s1%65521;
		s2=s2%65521;
		l-=5552;
	}

	if(l>0)
	{
		while(l>=16)
		{
			BTGECM_DataAdler32_Step16(s, &s1, &s2);
			s+=16; l-=16;
		}
		while(l--)
			{ s1+=*s++; s2+=s1; }
		s1=s1%65521;
		s2=s2%65521;
	}

	return(s1|(s2<<16));
}
#endif

BTGE_API u32 BTGECM_CRC32(void *buf, int sz, u32 lcrc)
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

BTGE_API u64 BTGECM_CRC64(void *buf, int sz, u64 lcrc)
{
	static u64 crctab[256];
	static int init=0;

	byte *s;
	u64 c;
	int i, j;

	if(!init)
	{
		init=1;

		for(i=0; i<256; i++)
		{
			c=i;
			for(j=0; j<8; j++)
				if(c&1)c=0xC96C5795D7870F42LL^(c>>1);
					else c>>=1;
			crctab[i]=c;
		}
	}

	c=lcrc; s=buf; i=sz;
	while(i--)c=crctab[(c^(*s++))&0xFF]^(c>>8);
	return(c);
}

u64 BTGECM_FileCache_HashName(char *name)
{
	char tb[256];
//	u32 h1, h2;
	u64 h;
	char *s;
	
//	s=name; h1=0; h2=0;
//	while(*s)
//	{
//		h1=(h1*65521)+(*s++);
//		h2=(h2*65521)+h1;
//	}
	
//	sprintf(tb, "%08x%08x%08x%08x",
//		(u32)(h2>>32), (u32)h2,
//		(u32)(h1>>32), (u32)h1);
//	sprintf(tb, "%08x%08x",
//		(u32)h2, (u32)h1);

	h=BTGECM_CRC64(name, strlen(name), (u64)-1);

	return(h);
}

char *BTGECM_FileCache_StrHashName(char *name)
{
	char tb[256];
//	u32 h1, h2;
	u64 h;
	char *s;
	
//	s=name; h1=0; h2=0;
//	while(*s)
//	{
//		h1=(h1*65521)+(*s++);
//		h2=(h2*65521)+h1;
//	}
	
//	sprintf(tb, "%08x%08x%08x%08x",
//		(u32)(h2>>32), (u32)h2,
//		(u32)(h1>>32), (u32)h1);
//	sprintf(tb, "%08x%08x",
//		(u32)h2, (u32)h1);
//	return(dyllStrdup(tb));

	h=BTGECM_CRC64(name, strlen(name), (u64)-1);
	sprintf(tb, "%08x%08x",
		(u32)(h>>32), (u32)h);
	return(dyllStrdup(tb));
}

void BTGECM_FileCache_LoadManifest(char *svname)
{
	char tb[256];
	BTGE_CachedFile *ent;
	VFILE *fd;
	char **a;
	char *s;
	
	sprintf(tb, "svcache/%s.dat", BTGECM_FileCache_StrHashName(svname));
	fd=vffopen(tb, "rt");
	if(!fd)return;
	
	while(!vfeof(fd))
	{
		vfgets(tb, 255, fd);
		a=gcrsplit(tb);
		if(!a[0])continue;
		
		ent=BTGECM_FileCache_ClientGetFile(a[0]);
		ent->size=atoi(a[1]);
		ent->checksum=atoi(a[2]);

		s=BTGECM_FileCache_StrHashName(a[0]);
		sprintf(tb, "svcache/%s_%08X_%08X.dat",
			s, ent->size, ent->checksum);
		ent->cfname=dyllStrdup(tb);
		
//		sprintf(tb, "%s/%s", svname, a[0]);
//		s=BTGECM_FileCache_StrHashName(tb);
//		sprintf(tb, "svcache/%s.dat", s);
//		ent->cfname=dyllStrdup(tb);
	}
	
	vfclose(fd);
}

void BTGECM_FileCache_SaveManifest(char *svname)
{
	char tb[256];
	BTGE_CachedFile *cur;
	VFILE *fd;
	char **a;
	char *s;
	
	cur=btgecm_filecache_client;
	while(cur)
	{
		if(cur->ent)
		{
			if(cur->name[0]=='.')
				{ cur=cur->next; continue; }

			cur->size=cur->ent->bsize;
			cur->checksum=BTGECM_DataAdler32(
				cur->ent->buffer, cur->size, 1);

			s=BTGECM_FileCache_StrHashName(cur->name);
			sprintf(tb, "svcache/%s_%08X_%08X.dat",
				s, cur->size, cur->checksum);
			cur->cfname=dyllStrdup(tb);
			
			fd=vffopen(cur->cfname, "wb");
			if(!fd) { cur=cur->next; continue; }
			vfwrite(cur->ent->buffer, 1, cur->size, fd);
			vfclose(fd);
		}
		cur=cur->next;
	}
	
	sprintf(tb, "svcache/%s.dat", BTGECM_FileCache_StrHashName(svname));
	fd=vffopen(tb, "rt");
	if(!fd)return;
	
	cur=btgecm_filecache_client;
	while(cur)
	{
		sprintf(tb, "%s %d %d\n", cur->name, cur->size, cur->checksum);
		vfputs(tb, fd);
		cur=cur->next;
	}
	vfclose(fd);
}

BTGE_CachedFile *BTGECM_FileCache_GeneralLookupFile(char *name)
{
	BTGE_CachedFile *cur;
	
	cur=btgecm_filecache_general;
	while(cur)
	{
		if(!strcmp(cur->name, name))
			return(cur);
		cur=cur->next;
	}
	
	return(NULL);
}

BTGE_CachedFile *BTGECM_FileCache_GeneralGetFile(char *name)
{
	BTGE_CachedFile *tmp;
	
	tmp=BTGECM_FileCache_GeneralLookupFile(name);
	if(tmp)return(tmp);
	
	tmp=gctalloc("btgecm_cachedfile_t", sizeof(BTGE_CachedFile));
	tmp->name=dyllStrdup(name);
	
	tmp->next=btgecm_filecache_general;
	btgecm_filecache_general=tmp;
	
	return(tmp);
}


BTGE_CachedFile *BTGECM_FileCache_ClientLookupFile(char *name)
{
	BTGE_CachedFile *cur;
	
	cur=btgecm_filecache_client;
	while(cur)
	{
		if(!strcmp(cur->name, name))
			return(cur);
		cur=cur->next;
	}
	
	return(NULL);
}

BTGE_CachedFile *BTGECM_FileCache_ClientGetFile(char *name)
{
	BTGE_CachedFile *tmp;
	
	tmp=BTGECM_FileCache_ClientLookupFile(name);
	if(tmp)return(tmp);
	
	tmp=gctalloc("btgecm_cachedfile_t", sizeof(BTGE_CachedFile));
	tmp->name=dyllStrdup(name);
	
	tmp->next=btgecm_filecache_client;
	btgecm_filecache_client=tmp;
	
	return(tmp);
}


BTGE_API void BTGE_FileCache_ClientDecodePushFile(
	BTGE_BrushWorld *world, dyt msg)
{
	BTGE_CachedFile *tmp;
	byte *buf;
	char *s;
	dyt n, t, ty;
	int sz;
	
	n=dycadr(msg);
	ty=dycaddr(msg);
	t=dycadddr(msg);

	if(!dyByteArrayP(t))
	{
		printf("BTGE_FileCache_WorldDecodePushFile: Not Byte Array\n");
		return;
	}

	s=dystringv(n);
	tmp=BTGECM_FileCache_ClientGetFile(s);

	buf=dyByteArrayData(t);
	sz=dyByteArraySize(t);

	tmp->size=sz;
	tmp->checksum=BTGECM_DataAdler32(buf, sz, 1);

//	if(tmp->data)
//		gcfree(tmp->data);

//	tmp->data=gcatomic(sz);
//	tmp->szdata=sz;
//	memcpy(tmp->data, buf, sz);

	tmp->ent=VFBUF_EntryWrapBuffer(s, buf, sz);

//	BTGE_Voxel_GlobalThink(world);
//	BTGE_Voxel_DecodeDeltaBuffer(world, buf, sz);
}

BTGE_API VFILE *BTGE_FileCache_ClientOpenFile(
	char *name, char *mode)
{
	BTGE_CachedFile *tmp;
	VFILE *fd;
	
	tmp=BTGECM_FileCache_ClientLookupFile(name);
	if(!tmp)return(NULL);
	
	if(tmp->ent)
	{
		fd=VFBUF_VfWrapEntry(tmp->ent);
		return(fd);
	}

	if(tmp->cfname)
	{
		fd=vffopen(tmp->cfname, mode);
		if(fd)return(fd);
	}

	return(NULL);
}


BTGE_API VFILE *btcl_vffopen(char *name, char *mode)
{
	VFILE *fd;

	fd=BTGE_FileCache_ClientOpenFile(name, mode);
	if(fd)return(fd);
	
	fd=vffopen(name, mode);
	return(fd);
}
