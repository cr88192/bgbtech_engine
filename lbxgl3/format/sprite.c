/*
text file based format

line driven.
// comment
<cmd> <args*>
<cmd> <args*> {
...
}

space <ox> <oy> <sx> <sy>
ox and oy are the offset of the center relative to the origin
sx and sy are the size relative to the origin.

Change:
make xml based.
<sprite>
	<sequence name="foo"
		frames="16" angles="8" fps="10"
		ox="ox" oy="oy" sx="sx" sy="sy"/>
	...
</sprite>
*/

#include <lbxgl2.h>

LBXGL_Sprite *lbxgl_sprite_root;
LBXGL_LensFlare *lbxgl_sprite_flareroot;

float lbxgl_sprite_camorg[3];

float lbxgl_sprite_camang[3];
float lbxgl_sprite_camfw[3];
float lbxgl_sprite_camrt[3];
float lbxgl_sprite_camup[3];

LBXGL_API int LBXGL_Sprite_SetCamera(float x, float y, float z)
{
	lbxgl_sprite_camorg[0]=x;
	lbxgl_sprite_camorg[1]=y;
	lbxgl_sprite_camorg[2]=z;

	return(0);
}

LBXGL_API int LBXGL_Sprite_SetCameraAngles(float x, float y, float z)
{
	lbxgl_sprite_camang[0]=x;
	lbxgl_sprite_camang[1]=y;
	lbxgl_sprite_camang[2]=z;

	Vec3F_CalcAngleVectors(lbxgl_sprite_camang,
		lbxgl_sprite_camfw,
		lbxgl_sprite_camrt,
		lbxgl_sprite_camup);

	return(0);
}

LBXGL_API int LBXGL_Sprite_SetCameraOrgRot(float *org, float *rot)
{
	lbxgl_sprite_camorg[0]=org[0];
	lbxgl_sprite_camorg[1]=org[1];
	lbxgl_sprite_camorg[2]=org[2];


	lbxgl_sprite_camrt[0]=rot[0];
	lbxgl_sprite_camrt[1]=rot[1];
	lbxgl_sprite_camrt[2]=rot[2];

	lbxgl_sprite_camfw[0]=rot[3];
	lbxgl_sprite_camfw[1]=rot[4];
	lbxgl_sprite_camfw[2]=rot[5];

	lbxgl_sprite_camup[0]=rot[6];
	lbxgl_sprite_camup[1]=rot[7];
	lbxgl_sprite_camup[2]=rot[8];

//	printf("sprint_cam org=(%g %g %g) rt=(%g %g %g) up=(%g %g %g)\n",
//		lbxgl_sprite_camorg[0], lbxgl_sprite_camorg[1], lbxgl_sprite_camorg[2],
//		lbxgl_sprite_camrt[0], lbxgl_sprite_camrt[1], lbxgl_sprite_camrt[2],
//		lbxgl_sprite_camup[0], lbxgl_sprite_camup[1], lbxgl_sprite_camup[2]);

	return(0);
}

LBXGL_API int LBXGL_Sprite_Render(LBXGL_Sprite *spr,
	float *org, float ang, char *seq, int fr, float scale)
{
	LBXGL_SpriteSeq *scur;
	LBXGL_SpriteFrame *fcur;
	float pt0[3], pt1[3], pt2[3], pt3[3];
	int rang;

	if(!spr)return(-1);

	scur=NULL;
	if(seq)
	{
		scur=spr->seqs;
		while(scur)
		{
			if(!strcmp(scur->name, seq))
				break;
			scur=scur->next;
		}
	}
	
	if(scur)fcur=scur->frames;
		else fcur=spr->frames;
		
	while(fcur)
	{
		if(fcur->num==fr)
			break;
		fcur=fcur->next;
	}

	if(!fcur)return(-1);

#if 0
	rang=ang-lbxgl_sprite_camang[2];
	rang%=360;
	rang/=360/scur->n_angles;

	glEnable(GL_TEXTURE_2D);
	Draw_Bind(scur->texnums[fr*scur->n_angles+rang]);
#endif

//	glEnable(GL_ALPHA_TEST);
	glDepthMask(GL_FALSE);

//	glEnable(GL_TEXTURE_2D);
	pdglEnableTexture2D();
	Draw_Bind(fcur->texnum);

#if 0
	VecNF_ScaleAddScale(
		lbxgl_sprite_camrt, -fcur->xs-fcur->ox,
		lbxgl_sprite_camup, fcur->ys-fcur->oy,
		pt0, 3);
	VecNF_ScaleAddScale(
		lbxgl_sprite_camrt, -fcur->xs-fcur->ox,
		lbxgl_sprite_camup, -fcur->ys-fcur->oy,
		pt1, 3);
	VecNF_ScaleAddScale(
		lbxgl_sprite_camrt, fcur->xs-fcur->ox,
		lbxgl_sprite_camup, -fcur->ys-fcur->oy,
		pt2, 3);
	VecNF_ScaleAddScale(
		lbxgl_sprite_camrt, fcur->xs-fcur->ox,
		lbxgl_sprite_camup, fcur->ys-fcur->oy,
		pt3, 3);
#endif

#if 1
	VecNF_ScaleAddScale(
		lbxgl_sprite_camrt, (0-fcur->ox)*scale,
		lbxgl_sprite_camup, (fcur->ys-fcur->oy)*scale,
		pt0, 3);
	VecNF_ScaleAddScale(
		lbxgl_sprite_camrt, (0-fcur->ox)*scale,
		lbxgl_sprite_camup, (0-fcur->oy)*scale,
		pt1, 3);
	VecNF_ScaleAddScale(
		lbxgl_sprite_camrt, (fcur->xs-fcur->ox)*scale,
		lbxgl_sprite_camup, (0-fcur->oy)*scale,
		pt2, 3);
	VecNF_ScaleAddScale(
		lbxgl_sprite_camrt, (fcur->xs-fcur->ox)*scale,
		lbxgl_sprite_camup, (fcur->ys-fcur->oy)*scale,
		pt3, 3);
#endif

	VecNF_Add(pt0, org, pt0, 3);
	VecNF_Add(pt1, org, pt1, 3);
	VecNF_Add(pt2, org, pt2, 3);
	VecNF_Add(pt3, org, pt3, 3);

	pdglBegin(PDGL_QUADS);
	pdglTexCoord2f(0, 1);
	pdglVertex3fv(pt0);
	pdglTexCoord2f(0, 0);
	pdglVertex3fv(pt1);
	pdglTexCoord2f(1, 0);
	pdglVertex3fv(pt2);
	pdglTexCoord2f(1, 1);
	pdglVertex3fv(pt3);
	pdglEnd();

//	glDisable(GL_ALPHA_TEST);
	glDepthMask(GL_TRUE);

	return(0);
}

LBXGL_API int LBXGL_Sprite_RenderState(LBXGL_SpriteState *ctx, float *org)
{
	if(!ctx)return(-1);
	return(LBXGL_Sprite_Render(ctx->sprite, org,
		ctx->angle, ctx->anim, ctx->frame, 1.0));
}

LBXGL_API int LBXGL_Sprite_RenderStateScale(
	LBXGL_SpriteState *ctx, float *org, float scale)
{
	if(!ctx)return(-1);
	return(LBXGL_Sprite_Render(ctx->sprite, org,
		ctx->angle, ctx->anim, ctx->frame, scale));
}

LBXGL_API int LBXGL_Sprite_BoxState(LBXGL_SpriteState *ctx,
	float *mins, float *maxs)
{
	LBXGL_Sprite *spr;
	LBXGL_SpriteSeq *scur;
	LBXGL_SpriteFrame *fcur;
	float pt0[3], pt1[3], pt2[3], pt3[3];
	int rang;

	V3F_SET(mins, -16, -16, -16);
	V3F_SET(maxs,  16,  16,  16);

	if(!ctx)return(-1);

	spr=ctx->sprite;

	if(!spr)return(-1);

	scur=NULL;
	if(ctx->anim)
	{
		scur=spr->seqs;
		while(scur)
		{
			if(!strcmp(scur->name, ctx->anim))break;
			scur=scur->next;
		}
	}
	
	if(scur)fcur=scur->frames;
		else fcur=spr->frames;
		
	while(fcur)
	{
		if(fcur->num==ctx->frame)break;
		fcur=fcur->next;
	}

	if(!fcur)return(-1);

	V3F_SET(mins, -fcur->xs, -fcur->xs, -fcur->ys);
	V3F_SET(maxs,  fcur->xs,  fcur->xs,  fcur->ys);
	
	return(0);
}

LBXGL_SpriteState *LBXGL_Sprite_NewStateInstance(LBXGL_Sprite *spr)
{
	LBXGL_SpriteState *tmp;
	
	tmp=gctalloc("lbxgl_spritestate_t", sizeof(LBXGL_SpriteState));
	tmp->sprite=spr;
	return(tmp);
}

LBXGL_API void LBXGL_Sprite_SetFrame(LBXGL_SpriteState *ctx, int frame)
{
	ctx->frame=frame;
}

LBXGL_API int LBXGL_Sprite_RenderFlareFrag(int tnum, float xsz, float ysz, float *org)
{
	float pt0[3], pt1[3], pt2[3], pt3[3];
	float pt[3];
	float d0, d1, ds;

	ds=VecNF_Distance(lbxgl_sprite_camorg, org, 3);
//	ds=1;

	Draw_Bind(tnum);

	VecNF_ScaleAddScale(
		lbxgl_sprite_camrt, -xsz*ds,
		lbxgl_sprite_camup, ysz*ds,
		pt0, 3);
	VecNF_ScaleAddScale(
		lbxgl_sprite_camrt, -xsz*ds,
		lbxgl_sprite_camup, -ysz*ds,
		pt1, 3);
	VecNF_ScaleAddScale(
		lbxgl_sprite_camrt, xsz*ds,
		lbxgl_sprite_camup, -ysz*ds,
		pt2, 3);
	VecNF_ScaleAddScale(
		lbxgl_sprite_camrt, xsz*ds,
		lbxgl_sprite_camup, ysz*ds,
		pt3, 3);

	pdglBegin(PDGL_QUADS);
	pdglTexCoord2f(0, 1);
	VecNF_Add(pt0, org, pt, 3);
	pdglVertex3fv(pt);

	pdglTexCoord2f(0, 0);
	VecNF_Add(pt1, org, pt, 3);
	pdglVertex3fv(pt);

	pdglTexCoord2f(1, 0);
	VecNF_Add(pt2, org, pt, 3);
	pdglVertex3fv(pt);

	pdglTexCoord2f(1, 1);
	VecNF_Add(pt3, org, pt, 3);
	pdglVertex3fv(pt);
	pdglEnd();

	return(0);
}

LBXGL_API int LBXGL_Sprite_RenderFlare(LBXGL_LensFlare *flare, float *org, float ra)
{
	LBXGL_SpriteSeq *scur;
	float pt0[3], pt1[3], pt2[3], pt3[3];
	float pt[3], ept[3], epts[3], rorg[3];
	int i;

	float d0, d1, ds, da, da0, da1;

	d0=VecNF_DotProduct(lbxgl_sprite_camorg, lbxgl_sprite_camfw, 3);
	d1=VecNF_DotProduct(org, lbxgl_sprite_camfw, 3);
	ds=d1-d0;
	if(ds<=0)return(-1);

	VecNF_Subtract(org, lbxgl_sprite_camorg, pt, 3);
	ds=VecNF_Normalize(pt, pt, 3);

	da=VecNF_DotProduct(pt, lbxgl_sprite_camfw, 3);
	da0=ra/(ds*ds/(36*36));
	da1=ra/(ds*ds/(36*36));

	if(da0>1)da0=1;
	if(da1>1)da1=1;
	da1*=da*da;

	VecNF_AddScale(lbxgl_sprite_camorg, lbxgl_sprite_camfw, flare->fdist*ds,
		ept, 3);

//	ept[0]=0;
//	ept[1]=0;
//	ept[2]=-100;

	VecNF_Subtract(ept, org, epts, 3);
	VecNF_Scale(epts, 1.0/(flare->fcnt+1), epts, 3);

	pdglColor4f(da0, da0, da0, 1);

	glBlendFunc(GL_SRC_COLOR, GL_ONE);

	glDisable(GL_DEPTH_TEST);
	glDisable(GL_CULL_FACE);

	glEnable(GL_TEXTURE_2D);
	Draw_Bind(flare->bnum);

	VecNF_ScaleAddScale(
		lbxgl_sprite_camrt, -flare->xs*ds,
		lbxgl_sprite_camup, flare->ys*ds,
		pt0, 3);
	VecNF_ScaleAddScale(
		lbxgl_sprite_camrt, -flare->xs*ds,
		lbxgl_sprite_camup, -flare->ys*ds,
		pt1, 3);
	VecNF_ScaleAddScale(
		lbxgl_sprite_camrt, flare->xs*ds,
		lbxgl_sprite_camup, -flare->ys*ds,
		pt2, 3);
	VecNF_ScaleAddScale(
		lbxgl_sprite_camrt, flare->xs*ds,
		lbxgl_sprite_camup, flare->ys*ds,
		pt3, 3);

	pdglBegin(PDGL_QUADS);
	pdglTexCoord2f(0, 1);
	VecNF_Add(pt0, org, pt, 3);
	pdglVertex3fv(pt);

	pdglTexCoord2f(0, 0);
	VecNF_Add(pt1, org, pt, 3);
	pdglVertex3fv(pt);

	pdglTexCoord2f(1, 0);
	VecNF_Add(pt2, org, pt, 3);
	pdglVertex3fv(pt);

	pdglTexCoord2f(1, 1);
	VecNF_Add(pt3, org, pt, 3);
	pdglVertex3fv(pt);
	pdglEnd();

	pdglColor4f(da1, da1, da1, 1);

	VecNF_Copy(org, rorg, 3);
	VecNF_Add(rorg, epts, rorg, 3);
	for(i=0; i<flare->fcnt; i++)
	{
		LBXGL_Sprite_RenderFlareFrag(flare->fnum,
			flare->xs2+(i*0.05), flare->ys2+(i*0.05),
			rorg);
		VecNF_Add(rorg, epts, rorg, 3);
	}

//	LBXGL_Sprite_RenderFlareFrag(flare->fnum,
//		flare->xs2, flare->ys2,
//		ept);

	glEnable(GL_DEPTH_TEST);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	return(0);
}

LBXGL_API int LBXGL_Sprite_RenderFlareName(char *name, float *org, float ra)
{
	LBXGL_LensFlare *cur;

	LBXGL_Sprite_Init();

//	printf("flare %s (%f %f %f)\n", name, org[0], org[1], org[2]);

	cur=lbxgl_sprite_flareroot;
	while(cur)
	{
		if(!strcmp(cur->name, name))
			return(LBXGL_Sprite_RenderFlare(cur, org, ra));
		cur=cur->next;
	}
//	printf("unknown flare %s\n", name);
	return(-1);
}

LBXGL_API LBXGL_LensFlare *LBXGL_Sprite_LookupFlare(char *name)
{
	LBXGL_LensFlare *cur;

	LBXGL_Sprite_Init();

//	printf("flare %s (%f %f %f)\n", name, org[0], org[1], org[2]);

	cur=lbxgl_sprite_flareroot;
	while(cur)
	{
		if(!strcmp(cur->name, name))
			return(cur);
		cur=cur->next;
	}
//	printf("unknown flare %s\n", name);
	return(NULL);
}

LBXGL_API LBXGL_LensFlare *LBXGL_Sprite_CreateFlare(char *name,
	char *bname, float bsz, char *fname, float fsz, float fdist, int fcnt)
{
	LBXGL_LensFlare *tmp;

	tmp=gctalloc("lbxgl_lensflare_t", sizeof(LBXGL_LensFlare));
	tmp->name=dystrdup(name);

	tmp->bnum=LBXGL_Texture_LoadImage(bname);
	tmp->fnum=LBXGL_Texture_LoadImage(fname);

	tmp->xs=bsz;
	tmp->ys=bsz;

	tmp->xs2=fsz;
	tmp->ys2=fsz;

	tmp->fdist=fdist;
	tmp->fcnt=fcnt;

	tmp->next=lbxgl_sprite_flareroot;
	lbxgl_sprite_flareroot=tmp;

	return(tmp);
}

LBXGL_API int LBXGL_Sprite_Init()
{
	static int init=0;

	if(init)return(0);
	init=1;

//	LBXGL_Sprite_CreateFlare("flare_cir",
//		"textures/effects/flare_center.tga", 0.1,
//		"textures/effects/flare_grcir.tga", 0.1,
//		0.25, 5);

//	LBXGL_Sprite_CreateFlare("flare_hex",
//		"textures/effects/flare1_base.tga", 0.25,
//		"textures/effects/flare1_grhex.tga", 0.1,
//		0.25, 5);

	LBXGL_Sprite_CreateFlare("flare_cir",
		"textures/effects/flare_center.png", 0.1,
		"textures/effects/flare_grcir.png", 0.1,
		0.25, 5);

	LBXGL_Sprite_CreateFlare("flare_pink",
		"textures/effects/flare_pink.png", 0.1,
		"textures/effects/flare_picir.png", 0.1,
		0.25, 5);

	LBXGL_Sprite_CreateFlare("flare_hex",
		"textures/effects/flare1_base.png", 0.25,
		"textures/effects/flare1_grhex.png", 0.1,
		0.25, 5);

	return(1);
}

static char **split_str(char *s)
{
	char **a, *t;
	int i;

	a=gcralloc(64*sizeof(char *));
	i=0;
	while(*s)
	{
		while(*s && (*s<=' '))s++;
		if(!*s)break;
		t=gcralloc(256);
		a[i++]=t;

		if(*s=='"')
		{
			s++;
			while(*s && (*s!='"'))*t++=*s++;
			if(*s=='"')s++;

			*t++=0;
			continue;
		}

		while(*s && (*s>' '))*t++=*s++;
		*t++=0;
	}
	a[i]=NULL;

	return(a);
}

#if 0
LBXGL_Sprite *LBXGL_Sprite_LoadSprites(char *inf)
{
	LBXGL_Sprite *tmp;
	LBXGL_SpriteSeq *seqf, *seql, *seqt;
	VFILE *fd;
	char base[256];
	char *buf;
	char *s, *t;
	char **a;

	strcpy(base, inf);
	s=base;
	t=s+strlen(s)-1;
	while((t>s) && (*t!='/'))t--;
	if(*t=='/')*t=0;

	tmp=gctalloc("lbxgl_sprite_t", sizeof(LBXGL_Sprite));
//	fd=vffopen(inf, "rt");
	fd=btcl_vffopen(inf, "rt");

	while(!vfeof(fd))
	{
		buf=gcralloc(256);
		vfgets(buf, 255, fd);

		a=split_str(buf);

		if(!strcmp(a[0], "space"))
		{
			tmp->ox=atof(a[1]);
			tmp->oy=atof(a[2]);
			tmp->xs=atof(a[3]);
			tmp->ys=atof(a[4]);
		}
	}
}
#endif

#if 0
LBXGL_SpriteSeq *
	LBXGL_Sprite_LoadSequence(LBXGL_Sprite *spr, NetParse_Node *sn)
{
	LBXGL_SpriteSeq *tmp;
	char *img;
	char *s;
	int i, j;

	tmp=kalloc(sizeof(LBXGL_SpriteSeq));

	s=NetParse_GetNodeAttr(sn, "name");
	tmp->name=dystring(s);

/*
	tmp->ox=0;
	tmp->oy=0;
	tmp->sx=1;
	tmp->sy=1;
*/

	tmp->fps=10;

	tmp->n_frames=1;
	tmp->n_angles=1;

/*
	s=NetParse_GetNodeAttr(sn, "ox");
	if(s)tmp->ox=katoi(s);
	s=NetParse_GetNodeAttr(sn, "oy");
	if(s)tmp->oy=katoi(s);

	s=NetParse_GetNodeAttr(sn, "sx");
	if(s)tmp->sx=katoi(s);
	s=NetParse_GetNodeAttr(sn, "sy");
	if(s)tmp->sy=katoi(s);
*/

	s=NetParse_GetNodeAttr(sn, "fps");
	if(s)tmp->fps=atof(s);

	s=NetParse_GetNodeAttr(sn, "frames");
	if(s)tmp->n_frames=katoi(s);
	s=NetParse_GetNodeAttr(sn, "angles");
	if(s)tmp->n_angles=katoi(s);

	tmp->texnums=kalloc(tmp->n_frames*tmp->n_angles*sizeof(int));
	for(i=0; i<tmp->n_frames; i++)
		for(j=0; j<tmp->n_angles; j++)
	{
		img=ksprint("%s/%s_%d-%d", spr->base, tmp->name, i, j);
		tmp->texnums[i*tmp->n_angles+j]=
			LBXGL_Texture_LoadImage(img);
	}

	return(tmp);
}
#endif

#if 0
LBXGL_Sprite *LBXGL_Sprite_LoadSprites(char *inf)
{
	NetParse_Node *nbase, *ncur;

	LBXGL_Sprite *tmp;
	LBXGL_SpriteSeq *seqf, *seql, *seqt;
	VFILE *fd;
	char base[256];
	char *buf;
	char *s, *t;
	char **a;

	strcpy(base, inf);
	s=base;
	t=s+strlen(s)-1;
	while((t>s) && (*t!='/'))t--;
	if(*t=='/')*t=0;

	tmp=gctalloc("lbxgl_sprite_t", sizeof(LBXGL_Sprite));
	tmp->name=dystring(inf);
	tmp->base=dystring(base);

	seql=NULL;

	nbase=NetParse_XML_LoadFile(inf);
	ncur=nbase->first;
	while(ncur)
	{
		if(!strcmp(ncur->key, "sequence"))
		{
			seqt=LBXGL_Sprite_LoadSequence(tmp, ncur);
			if(seql)seql->next=seqt;
				else tmp->seqs=seqt;
			seql=seqt;
		}
		ncur=ncur->next;
	}
}
#endif



#if 1
LBXGL_Sprite *LBXGL_Sprite_LoadSprites(char *inf)
{
	LBXGL_Sprite *tmp;
	LBXGL_SpriteFrame *frm;
	VFILE *fd;
	char base[256];
	char *buf;
	char *s, *t;
	char **a;

//	fd=vffopen(inf, "rt");
	fd=btcl_vffopen(inf, "rt");
	if(!fd)return(NULL);

	strcpy(base, inf);
	s=base;
	t=s+strlen(s)-1;
	while((t>s) && (*t!='/'))t--;
	if(*t=='/')*t=0;

	tmp=gctalloc("lbxgl_sprite_t", sizeof(LBXGL_Sprite));

	while(!vfeof(fd))
	{
		buf=gcralloc(256);
		vfgets(buf, 255, fd);

		a=split_str(buf);

		if(!a[0])continue;

		if(!strcmp(a[0], "$sprite"))
		{
			tmp->n_frames=atoi(a[1]);
			continue;
		}

		if(!strcmp(a[0], "$spr_flags"))
		{
			continue;
		}

		if(!strcmp(a[0], "$spr_frame"))
		{
			frm=gctalloc("lbxgl_spriteframe_t", sizeof(LBXGL_SpriteFrame));
			frm->num=atoi(a[1]);
			frm->ox=atoi(a[2]);
			frm->oy=atoi(a[3]);
			frm->xs=atoi(a[4]);
			frm->ys=atoi(a[5]);
			frm->texnum=LBXGL_Texture_LoadImage(a[6]);
			
			frm->next=tmp->frames;
			tmp->frames=frm;
			continue;
		}
	}
	
	return(tmp);
}

#endif
