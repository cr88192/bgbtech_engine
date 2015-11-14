#include <bgbmid.h>

BSG_PackState *bsg_pack_root=NULL;

BSG_PackState *BSG_LookupPack(char *name)
{
	BSG_PackState *cur;

	cur=bsg_pack_root;
	while(cur)
	{
		if(!bsg_stricmp(cur->name, name))
			return(cur);
		cur=cur->next;
	}
	return(NULL);
}

BSG_PackState *BSG_OpenPack(char *name)
{
	BSG_PackState *tmp;

	tmp=BSG_LookupPack(name);
	if(tmp)return(tmp);

	tmp=malloc(sizeof(BSG_PackState));
	memset(tmp, 0, sizeof(BSG_PackState));

	tmp->name=strdup(name);

	tmp->fd=bgbmid_fopen(name, "rb");

	bgbmid_fread(&tmp->hdr, 1, 12, tmp->fd);

	tmp->doffs=*(int *)(tmp->hdr.offs);
	tmp->dents=*(int *)(tmp->hdr.ents);

	tmp->dir=malloc(tmp->dents*sizeof(struct BSG_DirEnt_t));

	bgbmid_fseek(tmp->fd, tmp->doffs, 0);
	bgbmid_fread(tmp->dir, 64, tmp->dents, tmp->fd);

	tmp->next=bsg_pack_root;
	bsg_pack_root=tmp;

	return(tmp);
}

int BSG_LookupPackFile(BSG_PackState *ctx, char *name,
	int *roffs, int *rsize)
{
	int i, j, k;

//	printf("Check Pack %s for %s\n", ctx->name, name);

	for(i=0; i<ctx->dents; i++)
	{
		if(bsg_stricmp(ctx->dir[i].name, name))
			continue;

		j=*(int *)(ctx->dir[i].offs);
		k=*(int *)(ctx->dir[i].size);

		*roffs=j; *rsize=k;
		return(1);
	}

//	printf("Not Found %s for %s\n", ctx->name, name);

	return(0);
}

void *BSG_PackLookupFile(char *name, int *roffs, int *rsize)
{
	BSG_PackState *cur;
	int i;

	cur=bsg_pack_root;
	while(cur)
	{
		i=BSG_LookupPackFile(cur, name, roffs, rsize);
		if(i>0)return(cur->fd);

		cur=cur->next;
	}
	return(NULL);
}

byte *BSG_PackReadFile(char *name, int *rsize)
{
	void *fd;
	byte *buf;
	int offs, size;

	fd=BSG_PackLookupFile(name, &offs, &size);
	if(!fd)return(NULL);

//	printf("read %s %10d %d\n", name, offs, size);

	buf=malloc(size+1);
	buf[size]=0;

	bgbmid_fseek(fd, offs, 0);
	bgbmid_fread(buf, 1, size, fd);

//	printf("%c%c%c%c\n", buf[0], buf[1], buf[2], buf[3]);

	*rsize=size;
	return(buf);
}

