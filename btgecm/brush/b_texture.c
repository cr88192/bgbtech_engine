#include <btgecm.h>

BTGE_TexImage *btge_teximage_root=NULL;
BTGE_TexImage *btge_teximage_tab[4096];
int btge_teximage_rov=1;

BTGE_API BTGE_TexImage *BTGE_Texture_LookupImage(char *name)
{
	BTGE_TexImage *cur;

	cur=btge_teximage_root;
	while(cur)
	{
		if(!strcmp(cur->name, name))
			return(cur);
		cur=cur->next;
	}
	return(NULL);
}

BTGE_API BTGE_TexImage *BTGE_Texture_LookupNum(int num)
{
	BTGE_TexImage *cur;

	cur=btge_teximage_tab[num&4095];
	if(cur && cur->num==num)
		return(cur);

	cur=btge_teximage_root;
	while(cur)
	{
		if(cur->num==num)
		{
			btge_teximage_tab[num&4095]=cur;
			return(cur);
		}
		cur=cur->next;
	}
	return(NULL);
}

BTGE_API int BTGE_Texture_LoadInfo(BTGE_TexImage *tmp)
{
	VFILE *fd;
	void *p;
	char buf[256];
	char **a, *s;
	int i;

	strcpy(buf, tmp->name);
	s=buf+strlen(buf);
	while((s>buf) && (*s!='.'))s--;
	if(*s!='.')s=buf+strlen(buf);
	strcpy(s, ".txt");

	fd=vffopen(buf, "rt");
	if(!fd)return(-1);

	while(!vfeof(fd))
	{
		vfgets(buf, 255, fd);
		a=gcrsplit(buf);
		if(!a[0])continue;

		if(!strcmp(a[0], "$flags"))
		{
			for(i=1; a[i]; i++)
			{
				if(!strcmp(a[i], "alpha"))
					tmp->flags|=BTGE_TXFL_ALPHA;
				if(!strcmp(a[i], "valpha"))
					tmp->flags|=BTGE_TXFL_VALPHA;
				if(!strcmp(a[i], "nonsolid"))
					tmp->flags|=BTGE_TXFL_NONSOLID;
				if(!strcmp(a[i], "detail"))
					tmp->flags|=BTGE_TXFL_DETAIL;
				if(!strcmp(a[i], "nocsg"))
					tmp->flags|=BTGE_TXFL_NOCSG;
				if(!strcmp(a[i], "fullbright"))
					tmp->flags|=BTGE_TXFL_FULLBRIGHT;
				if(!strcmp(a[i], "window"))
					tmp->flags|=BTGE_TXFL_WINDOW;
//				if(!strcmp(a[i], "warpst"))
//					tmp->flags|=BTGE_TXFL_WARPST;
				if(!strcmp(a[i], "nodraw"))
					tmp->flags|=BTGE_TXFL_NODRAW;
				if(!strcmp(a[i], "fluid"))
					tmp->flags|=BTGE_TXFL_FLUID;

				if(!strcmp(a[i], "warpst"))
					tmp->flags|=BTGE_TXFL_WARP_ST;

				if(!strcmp(a[i], "water"))
					tmp->flags|=BTGE_TXFL_FLUID_WATER;
				if(!strcmp(a[i], "slime"))
					tmp->flags|=BTGE_TXFL_FLUID_SLIME;
				if(!strcmp(a[i], "lava"))
					tmp->flags|=BTGE_TXFL_FLUID_LAVA;
				if(!strcmp(a[i], "sewer"))
					tmp->flags|=BTGE_TXFL_FLUID_SEWER;
				if(!strcmp(a[i], "fluid_portal"))
					tmp->flags|=BTGE_TXFL_FLUID_PORTAL;
				if(!strcmp(a[i], "fluid_elastic"))
					tmp->flags|=BTGE_TXFL_FLUID_ELASTIC;

				if(!strcmp(a[i], "noshadow"))
					tmp->flags|=BTGE_TXFL_NOSHADOW;
				if(!strcmp(a[i], "noplane"))
					tmp->flags|=BTGE_TXFL_NOPLANE;
				if(!strcmp(a[i], "lightmat"))
					tmp->flags|=BTGE_TXFL_LIGHTMAT;

				if(!strcmp(a[i], "lightvol"))
				{
					tmp->flags|=BTGE_TXFL_EFFECT_LVOL;
					tmp->alpha=0.25;
				}

				if(!strcmp(a[i], "newscreen"))
					tmp->flags|=BTGE_TXFL_NEWSCREEN;
				if(!strcmp(a[i], "keepscreen"))
					tmp->flags|=BTGE_TXFL_KEEPSCREEN;

				if(!strcmp(a[i], "gloss"))
				{
					tmp->flags|=BTGE_TXFL_GLOSS;
					tmp->gloss=0.25;
				}

				if(!strcmp(a[i], "fog"))
				{
					tmp->flags|=BTGE_TXFL_EFFECT_FOG;
				}

				if(!strcmp(a[i], "origin"))
					{ tmp->flags|=BTGE_TXFL_EFFECT_ORIGIN; }
				if(!strcmp(a[i], "antiportal"))
					{ tmp->flags|=BTGE_TXFL_EFFECT_ANTIPORTAL; }
//				if(!strcmp(a[i], "voxel"))
//					{ tmp->flags|=BTGE_TXFL_EFFECT_VOXEL; }
				if(!strcmp(a[i], "portal"))
					{ tmp->flags|=BTGE_TXFL_EFFECT_PORTAL; }

				if(!strcmp(a[i], "decal"))
					{ tmp->flags|=BTGE_TXFL_EFFECT_DECAL; }
				if(!strcmp(a[i], "decal_pos"))
					{ tmp->flags|=BTGE_TXFL_EFFECT_DECAL2; }
				if(!strcmp(a[i], "decal_neg"))
					{ tmp->flags|=BTGE_TXFL_EFFECT_DECAL; }
				if(!strcmp(a[i], "decal_alpha"))
					{ tmp->flags|=BTGE_TXFL_EFFECT_DECAL_ALPHA; }

//				if(!strcmp(a[i], "ladder"))
//					{ tmp->flags|=BTGE_TXFL_EFFECT_LADDER; }
//				if(!strcmp(a[i], "ladder"))
//					{ tmp->flags|=BTGE_TXFL_MATERIAL_LADDER; }

				if(!strcmp(a[i], "sky"))
				{
					tmp->flags|=BTGE_TXFL_NODRAW;
					tmp->flags|=BTGE_TXFL_MATERIAL_SKY;
				}


				if(!strcmp(a[i], "voxel"))
					{ tmp->flags2|=BTGE_TXFL2_VOXEL; }
				if(!strcmp(a[i], "unbreakable"))
					{ tmp->flags2|=BTGE_TXFL2_UNBREAKABLE; }
				if(!strcmp(a[i], "partialblock"))
					{ tmp->flags2|=BTGE_TXFL2_PARTIALBLOCK; }
				if(!strcmp(a[i], "nodepthwrite"))
					{ tmp->flags2|=BTGE_TXFL2_NODEPTHWRITE; }
				if(!strcmp(a[i], "nowateroverlay"))
					{ tmp->flags2|=BTGE_TXFL2_NOWATEROVERLAY; }

				if(!strcmp(a[i], "touchpain"))
					{ tmp->flags2|=BTGE_TXFL2_TOUCHPAIN; }
				if(!strcmp(a[i], "ladder"))
					{ tmp->flags2|=BTGE_TXFL2_LADDER; }

				if(!strcmp(a[i], "auxvariant"))
					{ tmp->flags2|=BTGE_TXFL2_AUXVARIANT; }
				if(!strcmp(a[i], "noflexform"))
					{ tmp->flags2|=BTGE_TXFL2_NOFLEXFORM; }
			}
		}

		if(!strcmp(a[0], "$material"))
		{
			i=1;
			if(!strcmp(a[i], "solid"))
				{ tmp->flags|=BTGE_TXFL_MATERIAL_SOLID; }
			if(!strcmp(a[i], "sky"))
				{ tmp->flags|=BTGE_TXFL_MATERIAL_SKY; }
			if(!strcmp(a[i], "glass"))
				{ tmp->flags|=BTGE_TXFL_MATERIAL_GLASS; }
			if(!strcmp(a[i], "wood"))
				{ tmp->flags|=BTGE_TXFL_MATERIAL_WOOD; }
			if(!strcmp(a[i], "metal"))
				{ tmp->flags|=BTGE_TXFL_MATERIAL_METAL; }
			if(!strcmp(a[i], "flesh"))
				{ tmp->flags|=BTGE_TXFL_MATERIAL_FLESH; }
			if(!strcmp(a[i], "concrete"))
				{ tmp->flags|=BTGE_TXFL_MATERIAL_CONCRETE; }
			if(!strcmp(a[i], "foam"))
				{ tmp->flags|=BTGE_TXFL_MATERIAL_FOAM; }
			if(!strcmp(a[i], "computer"))
				{ tmp->flags|=BTGE_TXFL_MATERIAL_COMPUTER; }
			if(!strcmp(a[i], "unbreakable_glass"))
				{ tmp->flags|=BTGE_TXFL_MATERIAL_UBGLASS; }
			if(!strcmp(a[i], "stone"))
				{ tmp->flags|=BTGE_TXFL_MATERIAL_STONE; }
			if(!strcmp(a[i], "sludge"))
				{ tmp->flags|=BTGE_TXFL_MATERIAL_SLUDGE; }
		}

		if(!strcmp(a[0], "$lightvol"))
		{
			tmp->flags|=BTGE_TXFL_EFFECT_LVOL;
			tmp->alpha=atof(a[1]);
		}

		if(!strcmp(a[0], "$fog"))
		{
			tmp->flags|=BTGE_TXFL_EFFECT_FOG;
		}

		if(!strcmp(a[0], "$alpha"))
		{
			tmp->flags|=BTGE_TXFL_VALPHA;
			tmp->alpha=atof(a[1]);
		}

		if(!strcmp(a[0], "$gloss"))
		{
			tmp->flags|=BTGE_TXFL_GLOSS;
			tmp->gloss=atof(a[1]);
		}

		if(!strcmp(a[0], "$scale"))
		{
			if(a[2])
			{
				tmp->xscale=atof(a[1]);
				tmp->yscale=atof(a[2]);
			}else
			{
				tmp->xscale=atof(a[1]);
				tmp->yscale=atof(a[1]);
			}
		}

		if(!strcmp(a[0], "$script"))
			{ }
		if(!strcmp(a[0], "$shader"))
			{ }

		if(!strcmp(a[0], "$sky"))
		{
			tmp->flags|=BTGE_TXFL_NODRAW;
			tmp->flags|=BTGE_TXFL_MATERIAL_SKY;
//			BTGE_Sky_LoadSky(a[1]);
		}

		if(!strcmp(a[0], "$video"))
			{ }
		if(!strcmp(a[0], "$chromakey"))
			{ }
		if(!strcmp(a[0], "$drawfunc"))
			{ }
		if(!strcmp(a[0], "$fparm"))
			{ }
		if(!strcmp(a[0], "$vparm"))
			{ }
		if(!strcmp(a[0], "$texparm"))
			{ }
	}

	vfclose(fd);
	return(0);
}

BTGE_API int BTGE_Texture_LoadImageSize(char *name, int *rxs, int *rys)
{
	char tb[256];
	BTGE_TexImage *tmp;
	char *s;
	int x, y, x1, y1;
	int i, j;

//	tmp=NULL;
	tmp=BTGE_Texture_LookupImage(name);
	if(tmp)
	{
		if(rxs)*rxs=tmp->width;
		if(rys)*rys=tmp->height;
		return(tmp->num);
	}

	x=256;
	y=256;

	tmp=gctalloc("btge_teximage_t", sizeof(BTGE_TexImage));
	
	tmp->num=btge_teximage_rov++;

	tmp->name=dystrdup(name);
//	tmp->num=Tex_LoadFile(name, &x, &y);
	tmp->width=x;
	tmp->height=y;

	if(rxs)*rxs=x;
	if(rys)*rys=y;

	tmp->next=btge_teximage_root;
	btge_teximage_root=tmp;

	BTGE_Texture_LoadInfo(tmp);

	return(tmp->num);
}

BTGE_API int BTGE_Texture_LoadImage(char *name)
{
	return(BTGE_Texture_LoadImageSize(name, NULL, NULL));
}

BTGE_API int BTGE_Texture_GetImageSize(int num, int *xs, int *ys)
{
	BTGE_TexImage *tmp;

	tmp=BTGE_Texture_LookupNum(num);
	if(!tmp)return(-1);

	if(xs)*xs=tmp->width;
	if(ys)*ys=tmp->height;
	return(0);
}

BTGE_API byte *BTGE_Texture_GetImageBuffer(int num, int *xs, int *ys)
{
	BTGE_TexImage *tmp;
	byte *buf;

//	tmp=NULL;
	tmp=BTGE_Texture_LookupNum(num);
	if(!tmp)return(NULL);

	if(xs)*xs=tmp->width;
	if(ys)*ys=tmp->height;

	buf=NULL;
//	buf=Tex_GetBuffer(num);
	return(buf);
}

BTGE_API int BTGE_Texture_GetImageFlags(int num)
{
	BTGE_TexImage *tmp;
	tmp=BTGE_Texture_LookupNum(num);
	if(!tmp)return(0);
	return(tmp->flags);
}

BTGE_API int BTGE_Texture_GetImageFlags2(int num)
{
	BTGE_TexImage *tmp;
	tmp=BTGE_Texture_LookupNum(num);
	if(!tmp)return(0);
	return(tmp->flags2);
}

BTGE_API int BTGE_Texture_GetImageFlags3(int num)
{
	BTGE_TexImage *tmp;
	tmp=BTGE_Texture_LookupNum(num);
	if(!tmp)return(0);
	return(tmp->flags3);
}

BTGE_API int BTGE_Texture_GetImageFlags4(int num)
{
	BTGE_TexImage *tmp;
	tmp=BTGE_Texture_LookupNum(num);
	if(!tmp)return(0);
	return(tmp->flags4);
}

BTGE_API float BTGE_Texture_GetImageScaleX(int num)
{
	BTGE_TexImage *tmp;
	tmp=BTGE_Texture_LookupNum(num);
	if(!tmp)return(1.0);
	if(tmp->xscale==0)return(1.0);
	return(tmp->xscale);
}

BTGE_API float BTGE_Texture_GetImageScaleY(int num)
{
	BTGE_TexImage *tmp;
	tmp=BTGE_Texture_LookupNum(num);
	if(!tmp)return(1.0);
	if(tmp->yscale==0)return(1.0);
	return(tmp->yscale);
}
