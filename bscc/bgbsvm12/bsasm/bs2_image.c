#include <bs2vm.h>

BS2VM_ImageInfo *BS2VM_AllocImageInfo()
{
	BS2VM_ImageInfo *tmp;
	tmp=bs2vm_malloc2(sizeof(BS2VM_ImageInfo));
	return(tmp);
}

void *BS2VM_ImageMalloc(BS2VM_ImageInfo *img, int sz)
{
	void *p;
	p=bs2vm_malloc2(sz);
	return(p);
}

void *BS2VM_ImageRealloc(BS2VM_ImageInfo *img, void *ptr, int sz)
{
	void *p;
	p=bs2vm_realloc2(ptr, sz);
	return(p);
}

BS2VM_ImageLump *BS2VM_AllocImageLump(BS2VM_ImageInfo *img)
{
	BS2VM_ImageLump *tmp;
	tmp=bs2vm_malloc2(sizeof(BS2VM_ImageLump));
	return(tmp);
}

void BS2VM_ReserveForLumpIndex(BS2VM_ImageInfo *img, int idx)
{
	int i;

	if(!img->lump)
	{
		i=256;
		while(i<=idx)i=i+(i>>1);
		img->lump=bs2vm_malloc2(i*sizeof(BS2VM_ImageLump *));
		tmp->m_lump=i;
		tmp->n_lump=1;
		if(idx>=tmp->n_lump)
			tmp->n_lump=idx+1;
		return;
	}

	i=tmp->m_lump;
	while(i<=idx)i=i+(i>>1);
	img->lump=bs2vm_realloc2(img->lump, i*sizeof(BS2VM_ImageLump *));
	tmp->m_lump=i;
	if(idx>=tmp->n_lump)
		tmp->n_lump=idx+1;
}

BS2VM_ImageLump *BS2VM_ReadImageLump(
	BS2VM_ImageInfo *img, byte **rdata, byte *lim)
{
	BS2VM_ImageLump *tmp;
	byte *ip;
	
	ip=*rdata;
	tmp=BS2VM_AllocImageLump(img);
	tmp->ty_idx=BS2VM_DecVLI(&ip);
	tmp->sz_data=BS2VM_DecVLI(&ip);
	tmp->data=ip;
	ip+=tmp->sz_data;
	
	*rdata=ip;
	return(tmp);
}

char *BS2VM_GetImageUTF8(BS2VM_ImageInfo *img, int idx)
{
	char *tb1, *s;
	char tb[256];
	int i, sz;

	if(idx<=0)return(NULL);
	if(idx>=img->n_lump)return(NULL);
	if(!img->lump[idx])return(NULL);

	sz=img->lump[idx]->sz_data;
	if(sz<256)
	{
		memcpy(tb, img->lump[idx]->data, sz);
		tb[sz]=0;
		return(bs2vm_strdup(tb));
	}
	
	tb1=malloc(sz+1);
	memcpy(tb1, img->lump[idx]->data, sz);
	tb1[sz]=0;
	s=bs2vm_strdup(tb1);
	free(tb1);
	return(s);
}

void BS2VM_PreDecodeImageLump(
	BS2VM_ImageInfo *img, BS2VM_ImageLump *lmp, int cidx)
{
	BS2VM_Literal *lit;
	byte *ip;
	s64 l;
	f32 ff;
	f64 fd;
	f128 lf;

	if(!lmp->ty_str)return;

	if(!strcmp(lmp->ty_str, "UTF8"))
	{
		lmp->value=BS2VM_GetImageUTF8(img, cidx);
		return;
	}

	if(!strcmp(lmp->ty_str, "Int32"))
	{
		ip=lmp->data;
		l=BS2VM_DecS32(&ip);
		lmp->value=BS2VM_ImageMalloc(img, sizeof(s32));
		*(s32 *)(lmp->value)=l;
		return;
	}

	if(!strcmp(lmp->ty_str, "Int64"))
	{
		ip=lmp->data;
		l=BS2VM_DecS64(&ip);
		lmp->value=BS2VM_ImageMalloc(img, sizeof(s64));
		*(s64 *)(lmp->value)=l;
		return;
	}

	if(!strcmp(lmp->ty_str, "Int128"))
	{
		ip=lmp->data;
		l=BS2VM_DecS128(&ip);
		lmp->value=BS2VM_ImageMalloc(img, sizeof(s128));
		*(s128 *)(lmp->value)=l;
		return;
	}

	if(!strcmp(lmp->ty_str, "Float32"))
	{
		ip=lmp->data;
		ff=BS2VM_DecFloat32(&ip);
		lmp->value=BS2VM_ImageMalloc(img, sizeof(f32));
		*(f32 *)(lmp->value)=ff;
		return;
	}

	if(!strcmp(lmp->ty_str, "Float64"))
	{
		ip=lmp->data;
		fd=BS2VM_DecFloat64(&ip);
		lmp->value=BS2VM_ImageMalloc(img, sizeof(f64));
		*(f64 *)(lmp->value)=fd;
		return;
	}

	if(!strcmp(lmp->ty_str, "Float128"))
	{
		ip=lmp->data;
		lf=BS2VM_DecF128(&ip);
		lmp->value=BS2VM_ImageMalloc(img, sizeof(f128));
		*(f128 *)(lmp->value)=lf;
		return;
	}

	if(!strcmp(lmp->ty_str, "String"))
	{
		lit=BS2VM_ImageMalloc(img, sizeof(BS2VM_Literal));
		lmp->value=lit;
	}

	if(!strcmp(lmp->ty_str, "Symbol"))
	{
		lit=BS2VM_ImageMalloc(img, sizeof(BS2VM_Literal));
		lmp->value=lit;
	}

	if(!strcmp(lmp->ty_str, "Keyword"))
	{
		lit=BS2VM_ImageMalloc(img, sizeof(BS2VM_Literal));
		lmp->value=lit;
	}

}

void BS2VM_PostDecodeImageLump(
	BS2VM_ImageInfo *img, BS2VM_ImageLump *lmp, int cidx)
{
}

BS2VM_ImageInfo *BS2VM_DecodeImageBuffer(byte *data, int sz)
{
	BS2VM_ImageInfo *tmp;
	BS2VM_ImageLump *lmp;
	byte *ip, *ipe;
	int i, n;

	ip=data;
	ipe=data+sz;

	tmp=BS2VM_AllocImageInfo();
	n=1;
	while(1)
	{
		lmp=BS2VM_ReadImageLump(tmp, &ip, ipe);
		if(!lmp)break;
		
		//NullLump terminates file
		if(!lmp->ty_idx && !lmp->sz_data)
			break;
		BS2VM_ReserveForLumpIndex(ctx, n);
		tmp->lump[n]=lmp;
		n++;
	}
	
	for(i=1; i<tmp->n_lump; i++)
	{
		if(!img->lump[i])continue;
		img->lump[i]->ty_str=
			BS2VM_GetImageUTF8(ctx, img->lump[i]->ty_idx);
	}

	for(i=1; i<tmp->n_lump; i++)
	{
		if(!img->lump[i])continue;
		BS2VM_PreDecodeImageLump(tmp, img->lump[i], i);
	}

	for(i=1; i<tmp->n_lump; i++)
	{
		if(!img->lump[i])continue;
		BS2VM_PostDecodeImageLump(tmp, img->lump[i], i);
	}
	
	return(tmp);
}
