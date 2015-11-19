#include <lbxgl2.h>

#include <bgbbtj.h>

extern PDGLUI_InputState *lbxgl_state;
extern PDGLUI_Camera *lbxgl_cam;

LBXGL_TexImage *lbxgl_teximage_root=NULL;
LBXGL_TexImage *lbxgl_teximage_sync=NULL;

LBXGL_TexImage *lbxgl_teximage_tab[4096];

LBXGL_TexImage *lbxgl_shader_cur;


LBXGL_API LBXGL_TexImage *LBXGL_Texture_LookupImage(char *name)
{
	LBXGL_TexImage *cur;

	cur=lbxgl_teximage_root;
	while(cur)
	{
		if(!strcmp(cur->name, name))
			return(cur);
		cur=cur->next;
	}
	return(NULL);
}

LBXGL_API LBXGL_TexImage *LBXGL_Texture_LookupNum(int num)
{
	LBXGL_TexImage *cur;

//	return(NULL);

	if(num<=0)return(NULL);

	cur=lbxgl_teximage_tab[num&4095];
	if(cur && cur->num==num)
		return(cur);

	cur=lbxgl_teximage_root;
	while(cur)
	{
		if(cur->num==num)
		{
			lbxgl_teximage_tab[num&4095]=cur;
			return(cur);
		}
		cur=cur->next;
	}
	return(NULL);
}

LBXGL_API int LBXGL_Texture_LoadInfoCommand(
	LBXGL_TexImage *tmp, char **a)
{
	float tv[4];
	void *p;
	char *s, *s0, *s1;
	float f;
	int i, j;

		if(!strcmp(a[0], "$flags"))
		{
			for(i=1; a[i]; i++)
			{
				if(!strcmp(a[i], "alpha"))
					tmp->flags|=LBXGL_TXFL_ALPHA;
				if(!strcmp(a[i], "valpha"))
					tmp->flags|=LBXGL_TXFL_VALPHA;
				if(!strcmp(a[i], "nonsolid"))
					tmp->flags|=LBXGL_TXFL_NONSOLID;
				if(!strcmp(a[i], "detail"))
					tmp->flags|=LBXGL_TXFL_DETAIL;
				if(!strcmp(a[i], "nocsg"))
					tmp->flags|=LBXGL_TXFL_NOCSG;
				if(!strcmp(a[i], "fullbright"))
					tmp->flags|=LBXGL_TXFL_FULLBRIGHT;
				if(!strcmp(a[i], "window"))
					tmp->flags|=LBXGL_TXFL_WINDOW;
//				if(!strcmp(a[i], "warpst"))
//					tmp->flags|=LBXGL_TXFL_WARPST;
//				if(!strcmp(a[i], "warpxyz"))
//					tmp->flags|=LBXGL_TXFL_WARPXYZ;
				if(!strcmp(a[i], "nodraw"))
					tmp->flags|=LBXGL_TXFL_NODRAW;
				if(!strcmp(a[i], "fluid"))
					tmp->flags|=LBXGL_TXFL_FLUID;

				if(!strcmp(a[i], "warpst"))
					tmp->flags|=LBXGL_TXFL_WARP_ST;
				if(!strcmp(a[i], "warpxyz"))
					tmp->flags|=LBXGL_TXFL_WARP_XYZ;
				if(!strcmp(a[i], "scrollst"))
					tmp->flags|=LBXGL_TXFL_WARP_SCROLLST;
				if(!strcmp(a[i], "nbulge"))
					tmp->flags|=LBXGL_TXFL_WARP_NBULGE;
				if(!strcmp(a[i], "jiggle"))
					tmp->flags|=LBXGL_TXFL_WARP_JIGGLE;

				if(!strcmp(a[i], "water"))
					tmp->flags|=LBXGL_TXFL_FLUID_WATER;
				if(!strcmp(a[i], "slime"))
					tmp->flags|=LBXGL_TXFL_FLUID_SLIME;
				if(!strcmp(a[i], "lava"))
					tmp->flags|=LBXGL_TXFL_FLUID_LAVA;
				if(!strcmp(a[i], "sewer"))
					tmp->flags|=LBXGL_TXFL_FLUID_SEWER;
				if(!strcmp(a[i], "portal"))
					tmp->flags|=LBXGL_TXFL_FLUID_PORTAL;

				if(!strcmp(a[i], "noshadow"))
					tmp->flags|=LBXGL_TXFL_NOSHADOW;

				if(!strcmp(a[i], "lightvol"))
				{
					tmp->flags|=LBXGL_TXFL_EFFECT_LVOL;
					tmp->alpha=0.25;
				}

				if(!strcmp(a[i], "newscreen"))
					tmp->flags|=LBXGL_TXFL_NEWSCREEN;
				if(!strcmp(a[i], "keepscreen"))
					tmp->flags|=LBXGL_TXFL_KEEPSCREEN;

				if(!strcmp(a[i], "lightmat"))
					tmp->flags|=LBXGL_TXFL_LIGHTMAT;

				if(!strcmp(a[i], "gloss"))
				{
					tmp->flags|=LBXGL_TXFL_GLOSS;
					tmp->gloss=0.25;
				}

				if(!strcmp(a[i], "fog"))
				{
					tmp->flags|=LBXGL_TXFL_EFFECT_FOG;
					tmp->uf4v[0]=0.25;
					tmp->uf4v[1]=0.25;
					tmp->uf4v[2]=0.25;
					tmp->uf4v[3]=1.0/256;
				}

				if(!strcmp(a[i], "origin"))
					{ tmp->flags|=LBXGL_TXFL_EFFECT_ORIGIN; }
				if(!strcmp(a[i], "antiportal"))
					{ tmp->flags|=LBXGL_TXFL_EFFECT_ANTIPORTAL; }
				if(!strcmp(a[i], "decal"))
					{ tmp->flags|=LBXGL_TXFL_EFFECT_DECAL; }
				if(!strcmp(a[i], "voxel"))
					{ tmp->flags|=LBXGL_TXFL_EFFECT_VOXEL; }
				if(!strcmp(a[i], "portal"))
					{ tmp->flags|=LBXGL_TXFL_EFFECT_PORTAL; }

				if(!strcmp(a[i], "decal_pos"))
					{ tmp->flags|=LBXGL_TXFL_EFFECT_DECAL2; }
				if(!strcmp(a[i], "decal_neg"))
					{ tmp->flags|=LBXGL_TXFL_EFFECT_DECAL; }
				if(!strcmp(a[i], "decal_alpha"))
					{ tmp->flags|=LBXGL_TXFL_EFFECT_DECAL_ALPHA; }

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
			}
		}

		if(!strcmp(a[0], "$material"))
		{
			i=1;
			if(!strcmp(a[i], "solid"))
				{ tmp->flags|=LBXGL_TXFL_MATERIAL_SOLID; }
			if(!strcmp(a[i], "sky"))
				{ tmp->flags|=LBXGL_TXFL_MATERIAL_SKY; }
			if(!strcmp(a[i], "glass"))
				{ tmp->flags|=LBXGL_TXFL_MATERIAL_GLASS; }
			if(!strcmp(a[i], "wood"))
				{ tmp->flags|=LBXGL_TXFL_MATERIAL_WOOD; }
			if(!strcmp(a[i], "metal"))
				{ tmp->flags|=LBXGL_TXFL_MATERIAL_METAL; }
			if(!strcmp(a[i], "flesh"))
				{ tmp->flags|=LBXGL_TXFL_MATERIAL_FLESH; }
			if(!strcmp(a[i], "concrete"))
				{ tmp->flags|=LBXGL_TXFL_MATERIAL_CONCRETE; }
			if(!strcmp(a[i], "foam"))
				{ tmp->flags|=LBXGL_TXFL_MATERIAL_FOAM; }
			if(!strcmp(a[i], "computer"))
				{ tmp->flags|=LBXGL_TXFL_MATERIAL_COMPUTER; }
			if(!strcmp(a[i], "unbreakable_glass"))
				{ tmp->flags|=LBXGL_TXFL_MATERIAL_UBGLASS; }
			if(!strcmp(a[i], "stone"))
				{ tmp->flags|=LBXGL_TXFL_MATERIAL_STONE; }
			if(!strcmp(a[i], "sludge"))
				{ tmp->flags|=LBXGL_TXFL_MATERIAL_SLUDGE; }
		}

		if(!strcmp(a[0], "$lightvol"))
		{
			tmp->flags|=LBXGL_TXFL_EFFECT_LVOL;
			tmp->alpha=atof(a[1]);
		}

		if(!strcmp(a[0], "$fog"))
		{
			tmp->flags|=LBXGL_TXFL_EFFECT_FOG;
			tmp->uf4v[0]=atof(a[1]);
			tmp->uf4v[1]=atof(a[2]);
			tmp->uf4v[2]=atof(a[3]);
			tmp->uf4v[3]=atof(a[4]);
		}

		if(!strcmp(a[0], "$alpha"))
		{
			tmp->flags|=LBXGL_TXFL_VALPHA;
			tmp->alpha=atof(a[1]);
		}

		if(!strcmp(a[0], "$gloss"))
		{
			tmp->flags|=LBXGL_TXFL_GLOSS;
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

		if(!strcmp(a[0], "$size"))
		{
			if(a[2])
			{
				tmp->width=atoi(a[1]);
				tmp->height=atoi(a[2]);
			}else
			{
				tmp->width=atoi(a[1]);
				tmp->height=atoi(a[1]);
			}
		}

		if(!strcmp(a[0], "$scale_size"))
		{
			if(a[2])
			{
				tmp->width*=atof(a[1]);
				tmp->height*=atof(a[2]);
			}else
			{
				tmp->width*=atof(a[1]);
				tmp->height*=atof(a[1]);
			}
			if(tmp->width<1)tmp->width=1;
			if(tmp->height<1)tmp->height=1;
		}

		if(!strcmp(a[0], "$script"))
		{
			PDGL_LoadModule(a[1]);
		}

		if(!strcmp(a[0], "$shader"))
		{
			i=PDGL_LoadShader(a[1]);
			tmp->glsdr=i;
		}

		if(!strcmp(a[0], "$sky"))
		{
			tmp->flags|=LBXGL_TXFL_NODRAW;
			tmp->flags|=LBXGL_TXFL_MATERIAL_SKY;
			LBXGL_Sky_LoadSky(a[1]);
		}

		if(!strcmp(a[0], "$sun_origin"))
		{
			tv[0]=atof(a[1]);
			tv[1]=atof(a[2]);
			tv[2]=atof(a[3]);
//			tv[3]=atof(a[4]);
			LBXGL_Sky_SetSunOrigin(tv);
		}

		if(!strcmp(a[0], "$sun_color"))
		{
			tv[0]=atof(a[1]);
			tv[1]=atof(a[2]);
			tv[2]=atof(a[3]);
			tv[3]=1;
//			tv[3]=atof(a[4]);
			LBXGL_Sky_SetSunColor(tv);
		}

		if(!strcmp(a[0], "$sun_light"))
		{
			f=atof(a[1]);
			LBXGL_Sky_SetSunLight(f);
		}

		if(!strcmp(a[0], "$video"))
		{
			s0=dystrdup(a[1]);
			tmp->anim_video=AVI_LoadAVI(s0);
			if(tmp->anim_video)
			{
				tmp->tex_alt=AVI_FrameTexnum(tmp->anim_video, 0);
				tmp->tex_norm=AVI_FrameTexnumNorm(tmp->anim_video, 0);
//				tmp->tex_spec=AVI_FrameTexnumSpec(tmp->anim_video, 0);
				tmp->tex_gloss=AVI_FrameTexnumSpec(tmp->anim_video, 0);
				tmp->tex_glow=AVI_FrameTexnumLuma(tmp->anim_video, 0);

				tmp->flags&=~LBXGL_TXFL_COLOR_UVAY;
				
//				if(tmp->anim_video->audhead && tmp->anim_video->wavefmt)
				if(AVI_HasAudioP(tmp->anim_video))
				{
					PDGL_Sound_CreateStreamName(s0,
						AVI_GetAudioSampleRate(tmp->anim_video),
						2*AVI_GetAudioSampleRate(tmp->anim_video));

//					PDGL_Sound_CreateStreamName(a[1],
//						tmp->anim_video->wavefmt->nSamplesPerSec,
//						2*tmp->anim_video->wavefmt->nSamplesPerSec);
				}
			}
		}

		if(!strcmp(a[0], "$chromakey"))
		{
			if(tmp->anim_video)
			{
				AVI_SetChromaKey(tmp->anim_video,
					atoi(a[1]), atoi(a[2]), atoi(a[3]), atoi(a[4]));
			}
		}

		if(!strcmp(a[0], "$drawfunc"))
		{
			p=PDGL_GetPtr(a[1]);
			if(p)tmp->draw[tmp->ndraw++]=p;
				else printf("LBXGL_Texture_LoadInfo: "
					"no shader function '%s'\n", a[1]);
		}

		if(!strcmp(a[0], "$texgen"))
		{
			p=PDGL_GetPtr(a[1]);
			if(p)tmp->texGen=p;
				else printf("LBXGL_Texture_LoadInfo: "
					"no tex-gen function '%s'\n", a[1]);
		}

		if(!strcmp(a[0], "$warp_scale"))
		{
			tmp->warp_scale=atof(a[1]);
		}

		if(!strcmp(a[0], "$warp_speed"))
		{
			tmp->warp_speed=atof(a[1]);
		}

		if(!strcmp(a[0], "$warp_trans"))
		{
//			tmp->flags|=LBXGL_TXFL_SCROLLST;
//			tmp->flags|=LBXGL_TXFL_WARP_SCROLLST;
			tmp->warp_trans[0]=atof(a[1]);
			tmp->warp_trans[1]=atof(a[2]);
			tmp->warp_trans[2]=atof(a[3]);
		}

		if(!strcmp(a[0], "$fparm"))
		{
			i=atoi(a[1]);
			tmp->ufv[i]=atof(a[2]);
			if(i>=tmp->ufn)tmp->ufn=i+1;
		}

		if(!strcmp(a[0], "$vparm"))
		{
			i=atoi(a[1]);
			if(a[2])tmp->uf4v[i*4+0]=atof(a[2]);
				else tmp->uf4v[i*4+0]=0;
			if(a[3])tmp->uf4v[i*4+1]=atof(a[3]);
				else tmp->uf4v[i*4+1]=0;
			if(a[4])tmp->uf4v[i*4+2]=atof(a[4]);
				else tmp->uf4v[i*4+2]=0;
			if(a[5])tmp->uf4v[i*4+3]=atof(a[5]);
				else tmp->uf4v[i*4+3]=0;
			if(i>=tmp->ufvn)tmp->ufvn=i+1;
		}

		if(!strcmp(a[0], "$fpidx"))
			tmp->ufi[atoi(a[1])]=atoi(a[2]);
		if(!strcmp(a[0], "$vpidx"))
			tmp->uvi[atoi(a[1])]=atoi(a[2]);
		if(!strcmp(a[0], "$tpidx"))
			tmp->uti[atoi(a[1])]=atoi(a[2]);

		if(!strcmp(a[0], "$texparm"))
		{
			i=atoi(a[1]);
			tmp->utex[i]=Tex_LoadFile(a[2], NULL, NULL);
			if(i>=tmp->utn)tmp->utn=i+1;
		}

#if 1
		if(!strcmp(a[0], "$fparm_name"))
		{
			i=atoi(a[1]);
			tmp->uf1vn[i]=dystrdup(a[2]);
			tmp->ufv[i]=atof(a[3]);
			if(i>=tmp->ufn)tmp->ufn=i+1;
		}

		if(!strcmp(a[0], "$vparm_name"))
		{
			i=atoi(a[1]);
			tmp->uf4vn[i]=dystrdup(a[2]);

			if(a[3])tmp->uf4v[i*4+0]=atof(a[3]);
				else tmp->uf4v[i*4+0]=0;
			if(a[4])tmp->uf4v[i*4+1]=atof(a[4]);
				else tmp->uf4v[i*4+1]=0;
			if(a[5])tmp->uf4v[i*4+2]=atof(a[5]);
				else tmp->uf4v[i*4+2]=0;
			if(a[6])tmp->uf4v[i*4+3]=atof(a[6]);
				else tmp->uf4v[i*4+3]=0;
			if(i>=tmp->ufvn)tmp->ufvn=i+1;
		}

		if(!strcmp(a[0], "$texparm_name"))
		{
			i=atoi(a[1]);
			tmp->utexvn[i]=dystrdup(a[2]);

			tmp->utex[i]=Tex_LoadFile(a[3], NULL, NULL);
			if(i>=tmp->utn)tmp->utn=i+1;
		}

		if(!strcmp(a[0], "$fparm_cvar"))
		{
			i=atoi(a[1]);
			tmp->uf1vn[i]=dystrdup(a[2]);
			tmp->uf1vcn[i]=dystrdup(a[3]);
			if(i>=tmp->ufn)tmp->ufn=i+1;
		}

		if(!strcmp(a[0], "$vparm_cvar"))
		{
			i=atoi(a[1]);
			tmp->uf4vn[i]=dystrdup(a[2]);
			tmp->uf4vcn[i]=dystrdup(a[3]);
			if(i>=tmp->ufvn)tmp->ufvn=i+1;
		}
#endif

		if(!strcmp(a[0], "$tex_glow"))
		{
			tmp->tex_glow=Tex_LoadFile(a[1], NULL, NULL);
			tmp->flags|=LBXGL_TXFL_LIGHTMAT;
		}

		if(!strcmp(a[0], "$tex_normal"))
		{
			tmp->tex_norm=Tex_LoadFile(a[1], NULL, NULL);
			tmp->flags|=LBXGL_TXFL_LIGHTMAT;
		}

		if(!strcmp(a[0], "$tex_specular"))
		{
			tmp->tex_spec=Tex_LoadFile(a[1], NULL, NULL);
			tmp->flags|=LBXGL_TXFL_LIGHTMAT;
		}

		if(!strcmp(a[0], "$tex_gloss"))
		{
			tmp->tex_gloss=Tex_LoadFile(a[1], NULL, NULL);
			tmp->flags|=LBXGL_TXFL_LIGHTMAT;
		}

		if(!strcmp(a[0], "$ambient"))
		{
			if(a[1])tmp->mat_ambient[0]=atof(a[1]);
			if(a[2])tmp->mat_ambient[1]=atof(a[2]);
			if(a[3])tmp->mat_ambient[2]=atof(a[3]);
			if(a[4])tmp->mat_ambient[3]=atof(a[4]);
			tmp->flags|=LBXGL_TXFL_LIGHTMAT;
		}

		if(!strcmp(a[0], "$diffuse"))
		{
			if(a[1])tmp->mat_diffuse[0]=atof(a[1]);
			if(a[2])tmp->mat_diffuse[1]=atof(a[2]);
			if(a[3])tmp->mat_diffuse[2]=atof(a[3]);
			if(a[4])tmp->mat_diffuse[3]=atof(a[4]);
			tmp->flags|=LBXGL_TXFL_LIGHTMAT;
		}

		if(!strcmp(a[0], "$specular"))
		{
			if(a[1])tmp->mat_specular[0]=atof(a[1]);
			if(a[2])tmp->mat_specular[1]=atof(a[2]);
			if(a[3])tmp->mat_specular[2]=atof(a[3]);
			if(a[4])tmp->mat_specular[3]=atof(a[4]);
			tmp->flags|=LBXGL_TXFL_LIGHTMAT;
		}

		if(!strcmp(a[0], "$emission"))
		{
			if(a[1])tmp->mat_emission[0]=atof(a[1]);
			if(a[2])tmp->mat_emission[1]=atof(a[2]);
			if(a[3])tmp->mat_emission[2]=atof(a[3]);
			if(a[4])tmp->mat_emission[3]=atof(a[4]);
			tmp->flags|=LBXGL_TXFL_LIGHTMAT;
		}

		if(!strcmp(a[0], "$shininess"))
		{
			if(a[1])tmp->mat_shininess[0]=atof(a[1]);
			if(a[2])tmp->mat_shininess[1]=atof(a[2]);
			if(a[3])tmp->mat_shininess[2]=atof(a[3]);
			if(a[4])tmp->mat_shininess[3]=atof(a[4]);
			tmp->flags|=LBXGL_TXFL_LIGHTMAT;
		}

		if(!strcmp(a[0], "$ambient_u"))
		{
			f=atof(a[1]);
			V3F_CONST(tmp->mat_ambient, f);
			tmp->flags|=LBXGL_TXFL_LIGHTMAT;
		}
		if(!strcmp(a[0], "$diffuse_u"))
		{
			f=atof(a[1]);
			V3F_CONST(tmp->mat_diffuse, f);
			tmp->flags|=LBXGL_TXFL_LIGHTMAT;
		}
		if(!strcmp(a[0], "$specular_u"))
		{
			f=atof(a[1]);
			V3F_CONST(tmp->mat_specular, f);
			tmp->flags|=LBXGL_TXFL_LIGHTMAT;
		}
		if(!strcmp(a[0], "$emission_u"))
		{
			f=atof(a[1]);
			V3F_CONST(tmp->mat_emission, f);
			tmp->flags|=LBXGL_TXFL_LIGHTMAT;
		}
		if(!strcmp(a[0], "$shininess_u"))
		{
			f=atof(a[1]);
			V3F_CONST(tmp->mat_shininess, f);
			tmp->flags|=LBXGL_TXFL_LIGHTMAT;
		}


		if(!strcmp(a[0], "$blend"))
		{
			i=0; j=0;
			if(a[1])
			{
				if(!strcmp(a[1], "add"))
					{ i=GL_SRC_COLOR; j=GL_ONE; }
				if(!strcmp(a[1], "sub"))
					{ i=GL_ZERO; j=GL_ONE_MINUS_SRC_COLOR; }
				if(!strcmp(a[1], "filter"))
					{ i=GL_DST_COLOR; j=GL_ONE; }
				if(!strcmp(a[1], "blend"))
					{ i=GL_SRC_ALPHA; j=GL_ONE_MINUS_SRC_ALPHA; }
				if(!strcmp(a[1], "add_alpha"))
					{ i=GL_SRC_ALPHA; j=GL_ONE; }
			}

			if(a[1] && a[2])
			{
				//source
				if(!stricmp(a[1], "GL_ONE"))
					i=GL_ONE;
				if(!stricmp(a[1], "GL_ZERO"))
					i=GL_ZERO;
				if(!stricmp(a[1], "GL_SRC_COLOR"))
					i=GL_SRC_COLOR;
				if(!stricmp(a[1], "GL_DST_COLOR"))
					i=GL_DST_COLOR;
				if(!stricmp(a[1], "GL_ONE_MINUS_SRC_COLOR"))
					i=GL_ONE_MINUS_DST_COLOR;
				if(!stricmp(a[1], "GL_ONE_MINUS_DST_COLOR"))
					i=GL_ONE_MINUS_DST_COLOR;
				if(!stricmp(a[1], "GL_SRC_ALPHA"))
					i=GL_SRC_ALPHA;
				if(!stricmp(a[1], "GL_DST_ALPHA"))
					i=GL_DST_ALPHA;
				if(!stricmp(a[1], "GL_ONE_MINUS_SRC_ALPHA"))
					i=GL_ONE_MINUS_SRC_ALPHA;
				if(!stricmp(a[1], "GL_ONE_MINUS_DST_ALPHA"))
					i=GL_ONE_MINUS_SRC_ALPHA;

				//dest
				if(!stricmp(a[2], "GL_ONE"))
					j=GL_ONE;
				if(!stricmp(a[2], "GL_ZERO"))
					j=GL_ZERO;
				if(!stricmp(a[2], "GL_SRC_COLOR"))
					j=GL_SRC_COLOR;
				if(!stricmp(a[2], "GL_DST_COLOR"))
					j=GL_DST_COLOR;
				if(!stricmp(a[2], "GL_ONE_MINUS_SRC_COLOR"))
					j=GL_ONE_MINUS_DST_COLOR;
				if(!stricmp(a[2], "GL_ONE_MINUS_DST_COLOR"))
					j=GL_ONE_MINUS_DST_COLOR;
				if(!stricmp(a[2], "GL_SRC_ALPHA"))
					j=GL_SRC_ALPHA;
				if(!stricmp(a[2], "GL_DST_ALPHA"))
					j=GL_DST_ALPHA;
				if(!stricmp(a[2], "GL_ONE_MINUS_SRC_ALPHA"))
					j=GL_ONE_MINUS_SRC_ALPHA;
				if(!stricmp(a[2], "GL_ONE_MINUS_DST_ALPHA"))
					j=GL_ONE_MINUS_SRC_ALPHA;
			}
			
			if(i || j)
			{
				//got a valid pair
				tmp->blend_src=i;
				tmp->blend_dst=j;
			}
		}

		if(!strcmp(a[0], "$anim_next"))
		{
			i=LBXGL_Texture_LoadImage(a[1]);
			tmp->anim_next=i;
		}

		if(!strcmp(a[0], "$anim_matchsound"))
		{
			tmp->flags2|=BTGE_TXFL2_ANIMMATCHSOUND;
			
			i=tmp->snd_n_dct++;
			tmp->snd_dct=gcrealloc(tmp->snd_dct,
				(i+1)*64*sizeof(int));
			PDGL_Sound_GetSampleMatchDCT(a[1], tmp->snd_dct+i*64);
//			i=LBXGL_Texture_LoadImage(a[1]);
//			tmp->anim_next=i;
		}

#if 1
		if(!strcmp(a[0], "$layer"))
		{
			i=tmp->n_layer++;
			j=LBXGL_Texture_LoadImage(a[1]);
			tmp->layer_link[i]=j;
		}
#endif

	return(0);
}

LBXGL_API int LBXGL_Texture_LoadInfo(LBXGL_TexImage *tmp)
{
	LBXGL_TexImage *imgstack[64];
	LBXGL_TexImage *img_prev;
	LBXGL_TexImage *img_base;
	VFILE *fd;
	void *p;
	char buf[256];
	float tv[4];
	char **a, *s;
	float f;
	int imgstackpos;
	int i, j;

//	tmp->flags|=LBXGL_TXFL_COLOR_UVAY;
	strcpy(buf, tmp->name);
	
	s=buf;
	while(*s && ((s[0]!=':')||(s[1]!=':')))s++;
	if((s[0]==':') && (s[1]==':'))s=0;
	
	s=buf+strlen(buf);
	while((s>buf) && (*s!='.'))s--;
	if(*s!='.')s=buf+strlen(buf);
	strcpy(s, ".txt");

//	fd=vffopen(buf, "rt");
	fd=btcl_vffopen(buf, "rt");
	if(!fd)return(-1);
	imgstackpos=0;
	img_prev=tmp;
	img_base=tmp;

//	tmp->flags|=LBXGL_TXFL_COLOR_UVAY;

	while(!vfeof(fd))
	{
		vfgets(buf, 255, fd);
		a=gcrsplit(buf);
		if(!a[0])continue;

		if(!strcmp(a[0], "$begin_frame"))
		{
			i=LBXGL_Texture_LoadImage(a[1]);
			img_prev->anim_next=i;
			
			imgstack[imgstackpos++]=tmp;
			tmp=LBXGL_Texture_LookupNum(i);
			continue;
		}

		if(!strcmp(a[0], "$end_frame"))
		{
			img_prev=tmp;
			imgstackpos--;
			if(imgstackpos<0)
				break;
			tmp=imgstack[imgstackpos];
			continue;
		}

		if(!strcmp(a[0], "$end_frame_loop"))
		{
			img_prev=tmp;
			imgstackpos--;
			if(imgstackpos<0)
				break;
			tmp=imgstack[imgstackpos];
			img_prev->anim_next=tmp->num;			
			continue;
		}

		if(!strcmp(a[0], "$flags_prev"))
		{
			tmp->flags=img_prev->flags;
			tmp->flags2=img_prev->flags2;
			tmp->flags3=img_prev->flags3;
			tmp->flags4=img_prev->flags4;
		}

		if(!strcmp(a[0], "$begin_layer"))
		{
			i=img_base->n_layer++;
			j=LBXGL_Texture_LoadImage(a[1]);
			img_base->layer_link[i]=j;

			imgstack[imgstackpos++]=tmp;
			tmp=LBXGL_Texture_LookupNum(j);
			continue;
		}

		if(!strcmp(a[0], "$end_layer"))
		{
			imgstackpos--;
			if(imgstackpos<0)
				break;
			tmp=imgstack[imgstackpos];
			continue;
		}

		LBXGL_Texture_LoadInfoCommand(tmp, a);
	}

	vfclose(fd);
	return(0);
}

LBXGL_API int LBXGL_Texture_LoadImageBTJ(LBXGL_TexImage *tmp,
	char *name, int *rxs, int *rys)
{
	char tb[256], tnb[256], tnl[64];
	BGBBTJ_JPG_LayerInfo linf;

	VFILE *fd;
	byte *ibuf, *csl, *csl2;
	byte *rgba, *norm, *spec, *luma;
	char *s, *t;
	int xs, ys, sz, szl;
	int i;

	s=name; t=tnb;
	while(*s && ((s[0]!=':')||(s[1]!=':')) && (*s!='#'))
		*t++=*s++;
	*t++=0;
	
	t=tnl;
	if((s[0]==':') && (s[1]==':'))
		{ s+=2; while(*s && *s!=':')*t++=*s++; }
	if(s[0]=='#')
		{ s++; while(*s && *s!=':')*t++=*s++; }
	*t++=0;

//	sprintf(tb, "%s.btj", name);
	sprintf(tb, "%s.btj", tnb);
//	fd=vffopen(tb, "rb");
	fd=btcl_vffopen(tb, "rb");
	if(!fd)return(-1);

	ibuf=vf_bufferin_sz(fd, &sz);
	rgba=NULL; norm=NULL;
	spec=NULL; luma=NULL;
	xs=0; ys=0;
	
	vfclose(fd);
	
#if 1
	if(tnl[0])
	{
		csl=BGBBTJ_JPG_DecodeScanForTagLayer(ibuf, sz, tnl);
		if(!csl)return(-1);
		
		szl=sz-(csl-ibuf);
	}else
	{
		csl=ibuf; szl=sz;
	}

	i=BGBBTJ_JPG_DummyDecodeLayerInfo(csl, szl, &linf);
	if(i<0) { return(-1); }

	xs=linf.xs; ys=linf.ys;
	rgba=malloc(xs*ys*4);

	csl2=BGBBTJ_JPG_DecodeScanForComponentLayer(csl, szl, "XYZ");
	if(csl2) { norm=malloc(xs*ys*4); }
	csl2=BGBBTJ_JPG_DecodeScanForComponentLayer(csl, szl, "SpRGB");
	if(csl2) { spec=malloc(xs*ys*4); }
	csl2=BGBBTJ_JPG_DecodeScanForComponentLayer(csl, szl, "LuRGB");
	if(csl2) { luma=malloc(xs*ys*4); }
	
	BGBBTJ_JPG_DecodeComponent(csl, szl,
		rgba, norm, spec, luma, &xs, &ys);
#endif

	if(rgba)
		{ tmp->num=Tex_LoadTexture(xs, ys, rgba, 1); }
	if(norm)
		{ tmp->tex_norm=Tex_LoadTexture(xs, ys, norm, 1); }
	if(spec)
		{ tmp->tex_gloss=Tex_LoadTexture(xs, ys, spec, 1); }
	if(luma)
		{ tmp->tex_glow=Tex_LoadTexture(xs, ys, luma, 1); }

	//default material values
	V4F_SET(tmp->mat_ambient, 1, 1, 1, 1);
	V4F_SET(tmp->mat_diffuse, 1, 1, 1, 1);
	V4F_SET(tmp->mat_specular, 0.2, 0.2, 0.2, 1);
	V4F_SET(tmp->mat_emission, 0, 0, 0, 1);
	V4F_SET(tmp->mat_shininess, 1, 1, 1, 1);

	tmp->warp_scale=1;
	tmp->warp_speed=1;

	LBXGL_Texture_LoadInfo(tmp);

	if(rgba)free(rgba);
	if(norm)free(norm);
	if(spec)free(spec);
	if(luma)free(luma);

	if(rxs)*rxs=xs;
	if(rys)*rys=ys;
	return(tmp->num);
}

int LBXGL_Texture_LoadImageAltImg(LBXGL_TexImage *tmp)
{
	char tb[256];
	char *name;
	char *s, *t;
	int i, j;
	
	name=tmp->name;

#if 1
//	if(!nohires)
	if(1)
	{
		sprintf(tb, "%s_alt", name);
		tmp->tex_alt=Tex_LoadFile(tb, NULL, NULL);
	}

	if(tmp->tex_glow<=0)
	{
		sprintf(tb, "%s_add", name);
		tmp->tex_glow=Tex_LoadFile(tb, NULL, NULL);
	}

	if(tmp->tex_glow<=0)
	{
		sprintf(tb, "%s_luma", name);
		tmp->tex_glow=Tex_LoadFile(tb, NULL, NULL);
	}

	if(tmp->tex_glow<=0)
	{
		sprintf(tb, "%s_glow", name);
		tmp->tex_glow=Tex_LoadFile(tb, NULL, NULL);
	}

//	sprintf(tb, "%s_local", name);
//	tmp->tex_norm=Tex_LoadFile(tb, NULL, NULL);

//	if(!nohires)
	if(1)
	{
		if(tmp->tex_norm<=0)
		{
//			sprintf(tb, "%s_local", name);
//			tmp->tex_norm=Tex_LoadFile(tb, NULL, NULL);
			tmp->tex_norm=Tex_LoadFileBaseSuffix(name,
				"_local", "_bump", NULL, NULL);
			if(tmp->tex_norm>0)
				tmp->flags|=LBXGL_TXFL_LIGHTMAT;
		}

		if(tmp->tex_norm<=0)
		{
//			sprintf(tb, "%s_norm", name);
//			tmp->tex_norm=Tex_LoadFile(tb, NULL, NULL);
			tmp->tex_norm=Tex_LoadFileBaseSuffix(name,
				"_norm", "_bump", NULL, NULL);
			if(tmp->tex_norm>0)
				tmp->flags|=LBXGL_TXFL_LIGHTMAT;
		}

#if 0
		if(tmp->tex_spec<=0)
		{
//			sprintf(tb, "%s_spec", name);
			sprintf(tb, "%s_gloss", name);
			tmp->tex_spec=Tex_LoadFile(tb, NULL, NULL);
			if(tmp->tex_spec>0)
				tmp->flags|=LBXGL_TXFL_LIGHTMAT;
		}
#endif

#if 1
		if(tmp->tex_gloss<=0)
		{
			sprintf(tb, "%s_gloss", name);
//			sprintf(tb, "%s_gloss_alpha", name);
			tmp->tex_gloss=Tex_LoadFile(tb, NULL, NULL);
			if(tmp->tex_gloss>0)
				tmp->flags|=LBXGL_TXFL_LIGHTMAT;
		}

		if(tmp->tex_gloss<=0)
		{
			sprintf(tb, "%s_spec", name);
//			sprintf(tb, "%s_gloss", name);
//			sprintf(tb, "%s_gloss_alpha", name);
			tmp->tex_gloss=Tex_LoadFile(tb, NULL, NULL);
			if(tmp->tex_gloss>0)
				tmp->flags|=LBXGL_TXFL_LIGHTMAT;
		}
#endif

#if 0
		if(tmp->tex_gloss<=0)
		{
			sprintf(tb, "%s_s", name);
			tmp->tex_gloss=Tex_LoadFile(tb, NULL, NULL);
			if(tmp->tex_gloss>0)
				tmp->flags|=LBXGL_TXFL_LIGHTMAT;
		}
#endif
	}

	Tex_SetUseColorFormat(-1);

	//animated texture magic
	if(!tmp->anim_next)
	{
		s=name+strlen(name);
		while((s>name) && (*s!='/'))s--;
		if(*s=='/')s++;
		if((*s=='+') && (s[1]>='0') && (s[1]<='9'))
		{
			strcpy(tb, name);
			j=(s-name)+1;

			tb[j]=tb[j]+1;

			i=LBXGL_Texture_LoadImage(tb);
			if(i<=0)
			{
				tb[j]='0';
				i=LBXGL_Texture_LoadImage(tb);
			}
			tmp->anim_next=i;

			tb[j]='a';
			i=LBXGL_Texture_LoadImage(tb);
			tmp->anim_alt=i;
		}
	}
#endif

	pdglFinish();

	return(tmp->num);
}

void *LBXGL_Texture_LoadImageAltImg_cb(void *data)
{
	LBXGL_Texture_LoadImageAltImg(data);
	return(BGBGC_NULLEND);
}

LBXGL_API int LBXGL_Texture_LoadImageSize(char *name, int *rxs, int *rys)
{
	char tb[256], tnb[256], tnl[64];
	LBXGL_TexImage *tmp;
	VFILE *fd;
	char *s, *t;
	int x, y, x1, y1, nohires, fmt;
	int i, j;

//	tmp=NULL;
	tmp=LBXGL_Texture_LookupImage(name);
	if(tmp)
	{
		while(tmp->link)
			tmp=tmp->link;
		if(rxs)*rxs=tmp->width;
		if(rys)*rys=tmp->height;
		return(tmp->num);
	}

	x=0;
	y=0;

	nohires=BTGE_CvarGetNum("r_nohires");

	tmp=gctalloc("lbxgl_teximage_t", sizeof(LBXGL_TexImage));
	tmp->num=-1;
	tmp->name=dystrdup(name);
	
	//first, see if this texture is a link
	sprintf(tb, "%s_link.txt", name);
//	fd=vffopen(tb, "rt");
	fd=btcl_vffopen(tb, "rt");
	if(fd)
	{
		vfgets(tb, 255, fd);
		vfclose(fd);
		
		s=tb;
		while(*s && (*s>' '))s++;
		*s=0;
		
		i=LBXGL_Texture_LoadImageSize(tb, rxs, rys);
		tmp->link=LBXGL_Texture_LookupNum(i);

		if(i<0)
			printf("Texture Link(%s): %s->%s\n",
				(i>0)?"OK":"BAD", name, tb);

		tmp->next=lbxgl_teximage_root;
		lbxgl_teximage_root=tmp;

		return(i);
	}

	i=LBXGL_Texture_LoadImageBTJ(tmp, name, &x, &y);
	if(i>0)
	{
		if(rxs)*rxs=x;
		if(rys)*rys=y;
		return(tmp->num);
	}

	Tex_SetUseColorFormat(-1);

	tmp->num=Tex_LoadFile(name, &x, &y);
	tmp->width=x;
	tmp->height=y;

	//check if UVAY is being used
	fmt=Tex_GetLastColorFormat();
	if(fmt==BGBBTJ_JPG_BC3_UVAY)
	{
		tmp->flags|=LBXGL_TXFL_COLOR_UVAY;
		Tex_SetUseColorFormat(fmt);
	}

	if(tmp->num<0)
		printf("Texture: Missing-Base: %s\n", name);

	//default material values
	V4F_SET(tmp->mat_ambient, 1, 1, 1, 1);
	V4F_SET(tmp->mat_diffuse, 1, 1, 1, 1);
	V4F_SET(tmp->mat_specular, 0.2, 0.2, 0.2, 1);
	V4F_SET(tmp->mat_emission, 0, 0, 0, 1);
	V4F_SET(tmp->mat_shininess, 1, 1, 1, 1);

	tmp->warp_scale=1;
	tmp->warp_speed=1;

#if 0
	if(!nohires)
	{
		sprintf(tb, "%s_alt", name);
		tmp->tex_alt=Tex_LoadFile(tb, NULL, NULL);
	}
#endif

	sprintf(tb, "%s_size", name);
	i=Tex_LoadFile(tb, &x1, &y1);
	if(i>=0)
	{
		x=x1; y=y1;
		tmp->width=x;
		tmp->height=y;
	}


	if(rxs)*rxs=x;
	if(rys)*rys=y;

//	return(-1);

	tmp->next=lbxgl_teximage_root;
	lbxgl_teximage_root=tmp;

	LBXGL_Texture_LoadInfo(tmp);

	if(tmp->texGen)
		tmp->tex_alt=tmp->texGen(tmp);

	if(tmp->anim_video && AVI_HasAudioP(tmp->anim_video))
	{
		tmp->syncnext=lbxgl_teximage_sync;
		lbxgl_teximage_sync=tmp;
	}

	//see about loading secondary maps
	//if none were given in info
	if(!nohires)
	{
//		pdglAddJob(LBXGL_Texture_LoadImageAltImg_cb, tmp);
		LBXGL_Texture_LoadImageAltImg(tmp);
	}else
	{
		tmp->flags&=~LBXGL_TXFL_LIGHTMAT;
	}

#if 0
#if 1
	if(!nohires)
	{
		sprintf(tb, "%s_alt", name);
		tmp->tex_alt=Tex_LoadFile(tb, NULL, NULL);
	}
#endif

	if(tmp->tex_glow<=0)
	{
		sprintf(tb, "%s_add", name);
		tmp->tex_glow=Tex_LoadFile(tb, NULL, NULL);
	}

	if(tmp->tex_glow<=0)
	{
		sprintf(tb, "%s_luma", name);
		tmp->tex_glow=Tex_LoadFile(tb, NULL, NULL);
	}

	if(tmp->tex_glow<=0)
	{
		sprintf(tb, "%s_glow", name);
		tmp->tex_glow=Tex_LoadFile(tb, NULL, NULL);
	}

//	sprintf(tb, "%s_local", name);
//	tmp->tex_norm=Tex_LoadFile(tb, NULL, NULL);

	if(!nohires)
	{
		if(tmp->tex_norm<=0)
		{
//			sprintf(tb, "%s_local", name);
//			tmp->tex_norm=Tex_LoadFile(tb, NULL, NULL);
			tmp->tex_norm=Tex_LoadFileBaseSuffix(name,
				"_local", "_bump", NULL, NULL);
			if(tmp->tex_norm>0)
				tmp->flags|=LBXGL_TXFL_LIGHTMAT;
		}

		if(tmp->tex_norm<=0)
		{
//			sprintf(tb, "%s_norm", name);
//			tmp->tex_norm=Tex_LoadFile(tb, NULL, NULL);
			tmp->tex_norm=Tex_LoadFileBaseSuffix(name,
				"_norm", "_bump", NULL, NULL);
			if(tmp->tex_norm>0)
				tmp->flags|=LBXGL_TXFL_LIGHTMAT;
		}

#if 0
		if(tmp->tex_spec<=0)
		{
//			sprintf(tb, "%s_spec", name);
			sprintf(tb, "%s_gloss", name);
			tmp->tex_spec=Tex_LoadFile(tb, NULL, NULL);
			if(tmp->tex_spec>0)
				tmp->flags|=LBXGL_TXFL_LIGHTMAT;
		}
#endif

#if 1
		if(tmp->tex_gloss<=0)
		{
			sprintf(tb, "%s_gloss", name);
//			sprintf(tb, "%s_gloss_alpha", name);
			tmp->tex_gloss=Tex_LoadFile(tb, NULL, NULL);
			if(tmp->tex_gloss>0)
				tmp->flags|=LBXGL_TXFL_LIGHTMAT;
		}

		if(tmp->tex_gloss<=0)
		{
			sprintf(tb, "%s_spec", name);
//			sprintf(tb, "%s_gloss", name);
//			sprintf(tb, "%s_gloss_alpha", name);
			tmp->tex_gloss=Tex_LoadFile(tb, NULL, NULL);
			if(tmp->tex_gloss>0)
				tmp->flags|=LBXGL_TXFL_LIGHTMAT;
		}
#endif

#if 0
		if(tmp->tex_gloss<=0)
		{
			sprintf(tb, "%s_s", name);
			tmp->tex_gloss=Tex_LoadFile(tb, NULL, NULL);
			if(tmp->tex_gloss>0)
				tmp->flags|=LBXGL_TXFL_LIGHTMAT;
		}
#endif
	}

	Tex_SetUseColorFormat(-1);

	//animated texture magic
	if(!tmp->anim_next)
	{
		s=name+strlen(name);
		while((s>name) && (*s!='/'))s--;
		if(*s=='/')s++;
		if((*s=='+') && (s[1]>='0') && (s[1]<='9'))
		{
			strcpy(tb, name);
			j=(s-name)+1;

			tb[j]=tb[j]+1;

			i=LBXGL_Texture_LoadImage(tb);
			if(i<=0)
			{
				tb[j]='0';
				i=LBXGL_Texture_LoadImage(tb);
			}
			tmp->anim_next=i;

			tb[j]='a';
			i=LBXGL_Texture_LoadImage(tb);
			tmp->anim_alt=i;
		}
	}
#endif

	return(tmp->num);
}

LBXGL_API int LBXGL_Texture_LoadImage(char *name)
{
	int i;

	i=LBXGL_Texture_LoadImageSize(name, NULL, NULL);
	return(i);
}

LBXGL_API int LBXGL_Texture_LoadImageBufferSize(char *name, char *ty,
	byte *ibuf, int isz, int *rxs, int *rys)
{
	LBXGL_TexImage *tmp;
//	VFILE *fd;
	byte *buf;
	char *s;
	int xs, ys, xs1, ys1, nohires;
	int i, j;

	tmp=LBXGL_Texture_LookupImage(name);
	if(tmp)
	{
		buf=Tex_LoadImageBufferRaw(name, ty, ibuf, isz, &xs, &ys);
		if(buf)
		{
			tmp->num=Tex_LoadTexture2(xs, ys, buf, 1, tmp->num, 0);
			free(buf);
		}

		tmp->width=xs;
		tmp->height=ys;

		if(rxs)*rxs=xs;
		if(rys)*rys=ys;
		return(tmp->num);
	}

	tmp=gctalloc("lbxgl_teximage_t", sizeof(LBXGL_TexImage));
	tmp->num=-1;
	tmp->name=dystrdup(name);

	tmp->next=lbxgl_teximage_root;
	lbxgl_teximage_root=tmp;

	buf=Tex_LoadImageBufferRaw(name, ty, ibuf, isz, &xs, &ys);
	if(buf)
	{
		tmp->num=Tex_LoadTexture2(xs, ys, buf, 1, 0, 0);
		free(buf);
	}

	tmp->width=xs;
	tmp->height=ys;

	if(rxs)*rxs=xs;
	if(rys)*rys=ys;

	return(tmp->num);
}

LBXGL_API int LBXGL_Texture_GetImageSize(int num, int *xs, int *ys)
{
	LBXGL_TexImage *tmp;

//	tmp=NULL;
	tmp=LBXGL_Texture_LookupNum(num);
	if(!tmp)return(-1);

	if(xs)*xs=tmp->width;
	if(ys)*ys=tmp->height;
	return(0);
}

LBXGL_API byte *LBXGL_Texture_GetImageBuffer(int num, int *xs, int *ys)
{
	LBXGL_TexImage *tmp;
	byte *buf;

//	tmp=NULL;
	tmp=LBXGL_Texture_LookupNum(num);
	if(!tmp)return(NULL);

	if(xs)*xs=tmp->width;
	if(ys)*ys=tmp->height;

	buf=Tex_GetBuffer(num);
	return(buf);
}

LBXGL_API int LBXGL_Texture_GetImageFlags(int num)
{
	LBXGL_TexImage *tmp;
	tmp=LBXGL_Texture_LookupNum(num);
	if(!tmp)return(0);
	return(tmp->flags);
}

LBXGL_API int LBXGL_Texture_GetImageFlags2(int num)
{
	LBXGL_TexImage *tmp;
	tmp=LBXGL_Texture_LookupNum(num);
	if(!tmp)return(0);
	return(tmp->flags2);
}

LBXGL_API float LBXGL_Texture_GetImageScaleX(int num)
{
	LBXGL_TexImage *tmp;
	tmp=LBXGL_Texture_LookupNum(num);
	if(!tmp)return(1.0);
	if(tmp->xscale==0)return(1.0);
	return(tmp->xscale);
}

LBXGL_API float LBXGL_Texture_GetImageScaleY(int num)
{
	LBXGL_TexImage *tmp;
	tmp=LBXGL_Texture_LookupNum(num);
	if(!tmp)return(1.0);
	if(tmp->yscale==0)return(1.0);
	return(tmp->yscale);
}

LBXGL_API int LBXGL_Texture_GetImageGlow(int num)
{
	LBXGL_TexImage *tmp;
	tmp=LBXGL_Texture_LookupNum(num);
	if(!tmp)return(0);
	return(tmp->tex_glow);
}

LBXGL_API float LBXGL_Texture_GetImageAlpha(int num)
{
	LBXGL_TexImage *tmp;
	tmp=LBXGL_Texture_LookupNum(num);
	if(!tmp)return(1.0);
	return(tmp->alpha);
}

LBXGL_API float LBXGL_Texture_GetImageFParm(int num, int idx)
{
	LBXGL_TexImage *tmp;
	tmp=LBXGL_Texture_LookupNum(num);
	if(!tmp)return(0.0);
	return(tmp->ufv[idx]);
}

LBXGL_API void LBXGL_Texture_GetImageVParm(int num, int idx, float *tv)
{
	LBXGL_TexImage *tmp;
	tmp=LBXGL_Texture_LookupNum(num);
	if(!tmp)
	{
		V4F_ZERO(tv);
		return;
	}
	V4F_COPY(tmp->uf4v+idx*4, tv);
	return;
}

LBXGL_API void LBXGL_Texture_BindImage(int num)
{
	LBXGL_Shader_BindTexture(num);

	pdglEnable(GL_TEXTURE_2D);
	pdglBindTexture(GL_TEXTURE_2D, lbxgl_shader_cur->num);
}

void LBXGL_Texture_SetImageMaterial(int tex)
{
	LBXGL_TexImage *img;
	img=LBXGL_Texture_LookupNum(tex);
	LBXGL_Texture_SetImageMaterialImg(img);
}

void LBXGL_Texture_SetImageMaterialImg(LBXGL_TexImage *img)
{
	float pt[4];

#ifndef GLES
	if(!img)
	{
		pt[0]=1; pt[1]=1; pt[2]=1; pt[3]=1;
		pdglMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, pt);
		pdglMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, pt);
		pt[0]=0.2; pt[1]=0.2; pt[2]=0.2; pt[3]=1;
		pdglMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, pt);
		pt[0]=0; pt[1]=0; pt[2]=0; pt[3]=1;
		pdglMaterialfv(GL_FRONT_AND_BACK, GL_EMISSION, pt);
		pt[0]=1;
		pdglMaterialfv(GL_FRONT_AND_BACK, GL_SHININESS, pt);
		return;
	}

	pdglMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, img->mat_ambient);
	pdglMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, img->mat_diffuse);
	pdglMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, img->mat_specular);
	pdglMaterialfv(GL_FRONT_AND_BACK, GL_EMISSION, img->mat_emission);
	pdglMaterialfv(GL_FRONT_AND_BACK, GL_SHININESS, img->mat_shininess);
#endif
}

void LBXGL_Texture_Update(float dt)
{
	LBXGL_TexImage *cur;
	short *pcm;
	char *s;
	float f, g, dt2;
	int i, j;

#if 0
	cur=lbxgl_teximage_sync;
	while(cur)
	{
		if(cur->anim_video)
			{ AVI_ClearDeltaTick(cur->anim_video); }
		cur=cur->syncnext;
	}
#endif

	cur=lbxgl_teximage_sync;
	while(cur)
	{
		if(cur->anim_video && !AVI_GetDeltaTick(cur->anim_video))
		{
			AVI_SetDeltaTick(cur->anim_video);
		
			f=lbxgl_state->time_f;
			g=cur->anim_time;
			dt2=(g>0)?(f-g):0;
//			if(dt2>0.25)dt2=0;
			if(dt2>0.5)dt2=0;
			cur->anim_time=f;
			cur->tex_alt=
				AVI_FrameTexnum(cur->anim_video, dt2);

			if(AVI_HasAudioP(cur->anim_video));
			{
				AVI_GetAudioSamples(cur->anim_video,
					&pcm, &j);
				s=AVI_GetVideoName(cur->anim_video);
				PDGL_Sound_WriteStreamStereoSamples(s, pcm, j);
				AVI_ClearAudioSamples(cur->anim_video);
			}
		}

		cur=cur->syncnext;
	}

	AVI_ClearAllDeltaTicks();

#if 0
	cur=lbxgl_teximage_sync;
	while(cur)
	{
		if(cur->anim_video)
			{ AVI_ClearDeltaTick(cur->anim_video); }
		cur=cur->syncnext;
	}
#endif
}
