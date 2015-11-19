#include <lbxgl2.h>

LBXGL_QMDL *lbxgl_qmdl_cache=NULL;

static short ReadInt16LE(VFILE *fd)
{
	short i;

	i=vfgetc(fd);
	i|=vfgetc(fd)<<8;

	return(i);
}

static int ReadInt32LE(VFILE *fd)
{
	int i;

	i=vfgetc(fd);
	i|=vfgetc(fd)<<8;
	i|=vfgetc(fd)<<16;
	i|=vfgetc(fd)<<24;

	return(i);
}

static float ReadFloat32LE(VFILE *fd)
{
	int i;

	i=vfgetc(fd);
	i|=vfgetc(fd)<<8;
	i|=vfgetc(fd)<<16;
	i|=vfgetc(fd)<<24;

	return(*(float *)(&i));
}

LBXGL_QMDL *LBXGL_QuakeMDL_Load(char *name, char *pal)
{
	LBXGL_QMDL *tmp;
	quake_qmdl_t *head;
	byte *skinbuf, *palbuf, *skin2;
	float *vecbuf;
	VFILE *fd;
	int i, j;
	char namebuf[17];

	palbuf=gcalloc(768);
	for(i=0; i<256; i++)
	{
		palbuf[(i*3)+0]=i;
		palbuf[(i*3)+1]=i;
		palbuf[(i*3)+2]=i;
	}

	if(pal)
	{
		fd=vffopen(pal, "rb");
		if(fd)
		{
			vfread(palbuf, 1, 768, fd);
			vfclose(fd);
		}
	}

	fd=vffopen(name, "rb");
	if(!fd)return(NULL);

	printf("loading mdl %s\n", name);

	tmp=gctalloc("lbxgl_quakemdl_t", sizeof(LBXGL_QMDL));
	head=&tmp->head;

	head->ident=ReadInt32LE(fd);
	head->version=ReadInt32LE(fd);

	if(head->ident!=QMDL_ID)
	{
		printf("LBXGL_QuakeMDL_Load(%s): Invalid Ident\n", name);
		return(NULL);
	}
	if(head->version!=QMDL_VERSION)
	{
		printf("LBXGL_QuakeMDL_Load(%s): Invalid Version\n", name);
		return(NULL);
	}

	for(i=0; i<3; i++)
		head->scale[i]=ReadFloat32LE(fd);
	for(i=0; i<3; i++)
		head->scale_origin[i]=ReadFloat32LE(fd);
	head->boundingradius=ReadFloat32LE(fd);
	for(i=0; i<3; i++)
		head->eyeposition[i]=ReadFloat32LE(fd);
	head->num_skins=ReadInt32LE(fd);
	head->skinwidth=ReadInt32LE(fd);
	head->skinheight=ReadInt32LE(fd);
	head->numverts=ReadInt32LE(fd);
	head->num_tris=ReadInt32LE(fd);
	head->num_frames=ReadInt32LE(fd);
	head->synctype=ReadInt32LE(fd);
	head->flags=ReadInt32LE(fd);
	head->size=ReadFloat32LE(fd);

	head->num_xyz=head->numverts;
	head->num_st=head->numverts;

	tmp->skins=gcalloc(head->num_skins*sizeof(byte *));
	tmp->skinnums=gcalloc(head->num_skins*sizeof(int));
	tmp->skinnames=gcalloc(head->num_skins*sizeof(int));

	tmp->st_vecs=gcalloc(2*head->numverts*sizeof(float));
	tmp->st_seam=gcalloc(head->numverts*sizeof(int));
	tmp->polys=gcalloc(8*head->num_tris*sizeof(int));

	tmp->frames=gcalloc(head->num_frames*sizeof(float *));
	tmp->framenames=gcalloc(head->num_frames*sizeof(char *));

	printf("skins %d\n", head->num_skins);
	printf("skin %d %d\n", head->skinwidth, head->skinheight);
	printf("vecs %d\n", head->numverts);
	printf("tris %d\n", head->num_tris);
	printf("frames %d\n", head->num_frames);
	printf("scale %f %f %f\n", head->scale[0],
		head->scale[1], head->scale[2]);
	printf("scale_origin %f %f %f\n", head->scale[0],
		head->scale_origin[1], head->scale_origin[2]);

	skinbuf=gcalloc(head->skinwidth*head->skinheight);
	for(i=0; i<head->num_skins; i++)
	{
		ReadInt32LE(fd);
		vfread(skinbuf, 1, head->skinwidth*head->skinheight, fd);

		tmp->skins[i]=malloc(head->skinwidth*head->skinheight*4);
		skin2=tmp->skins[i];
		for(j=0; j<(head->skinwidth*head->skinheight); j++)
		{
			skin2[(j*4)+0]=palbuf[(skinbuf[j]*3)+0];
			skin2[(j*4)+1]=palbuf[(skinbuf[j]*3)+1];
			skin2[(j*4)+2]=palbuf[(skinbuf[j]*3)+2];
			skin2[(j*4)+3]=255;
		}
		tmp->skinnums[i]=Tex_LoadTexture(head->skinwidth,
			head->skinheight, skin2, 1);
	}
	gcfree(skinbuf);

	for(i=0; i<head->numverts; i++)
	{
		tmp->st_seam[i]=ReadInt32LE(fd);
		tmp->st_vecs[(i*2)+0]=ReadInt32LE(fd);
		tmp->st_vecs[(i*2)+1]=ReadInt32LE(fd);
	}

	for(i=0; i<head->num_tris; i++)
	{
		tmp->polys[(i*8)+3]=ReadInt32LE(fd);
		tmp->polys[(i*8)+0]=ReadInt32LE(fd);
		tmp->polys[(i*8)+1]=ReadInt32LE(fd);
		tmp->polys[(i*8)+2]=ReadInt32LE(fd);

		tmp->polys[(i*8)+4]=tmp->polys[(i*8)+0];
		tmp->polys[(i*8)+5]=tmp->polys[(i*8)+1];
		tmp->polys[(i*8)+6]=tmp->polys[(i*8)+2];
	}

	for(i=0; i<head->num_frames; i++)
	{
		ReadInt32LE(fd);
		ReadInt32LE(fd);
		ReadInt32LE(fd);

		memset(namebuf, 0, 17);
		vfread(namebuf, 1, 16, fd);

		tmp->framenames[i]=dystrdup(namebuf);
		tmp->frames[i]=gcalloc(3*head->numverts*sizeof(float));
		vecbuf=tmp->frames[i];

//		printf("frame %s\n", namebuf);

		for(j=0; j<head->numverts; j++)
		{
			vecbuf[(j*3)+0]=(vfgetc(fd)*head->scale[0])+
				head->scale_origin[0];
			vecbuf[(j*3)+1]=(vfgetc(fd)*head->scale[1])+
				head->scale_origin[1];
			vecbuf[(j*3)+2]=(vfgetc(fd)*head->scale[2])+
				head->scale_origin[2];
			vfgetc(fd);

			vecbuf[(j*3)+0]*=QMDL_SCALE;
			vecbuf[(j*3)+1]*=QMDL_SCALE;
			vecbuf[(j*3)+2]*=QMDL_SCALE;
		}
	}

	return(tmp);
}

LBXGL_QMDL *LBXGL_QuakeMD2_Load(char *name)
{
	LBXGL_QMDL *tmp;
	quake_qmdl_t *head;
	byte *skinbuf;
	float *vecbuf;
	VFILE *fd;
	int i, j, w, h;
	char namebuf[65];
	float trans[3], scale[3];

	fd=vffopen(name, "rb");
	if(!fd)return(NULL);

	printf("loading md2 %s\n", name);

	tmp=gctalloc("lbxgl_quakemdl_t", sizeof(LBXGL_QMDL));
	head=&tmp->head;

	head->ident=ReadInt32LE(fd);
	head->version=ReadInt32LE(fd);

	if(head->ident!=QMD2_ID)
	{
		printf("LBXGL_QuakeMD2_Load(%s): Invalid Ident\n", name);
		return(NULL);
	}
	if(head->version!=QMD2_VERSION)
	{
		printf("LBXGL_QuakeMD2_Load(%s): Invalid Version\n", name);
		return(NULL);
	}

	head->skinwidth=ReadInt32LE(fd);
	head->skinheight=ReadInt32LE(fd);
	head->framesize=ReadInt32LE(fd);
	head->num_skins=ReadInt32LE(fd);

	head->num_xyz=ReadInt32LE(fd);
	head->num_st=ReadInt32LE(fd);

	head->num_tris=ReadInt32LE(fd);
	head->num_glcmds=ReadInt32LE(fd);
	head->num_frames=ReadInt32LE(fd);

	head->ofs_skins=ReadInt32LE(fd);
	head->ofs_st=ReadInt32LE(fd);
	head->ofs_tris=ReadInt32LE(fd);
	head->ofs_frames=ReadInt32LE(fd);
	head->ofs_glcmds=ReadInt32LE(fd);
	head->ofs_end=ReadInt32LE(fd);

	tmp->skins=gcalloc((head->num_skins+1)*sizeof(byte *));
	tmp->skinnums=gcalloc(head->num_skins*sizeof(int));
	tmp->skinnames=gcalloc(head->num_skins*sizeof(int));

	tmp->st_vecs=gcalloc(2*head->num_st*sizeof(float));
	tmp->st_seam=gcalloc(head->num_st*sizeof(int));
	tmp->polys=gcalloc(8*head->num_tris*sizeof(int));

	tmp->frames=gcalloc(head->num_frames*sizeof(float *));
	tmp->framenames=gcalloc(head->num_frames*sizeof(char *));

	printf("skins %d\n", head->num_skins);
	printf("skin %d %d\n", head->skinwidth, head->skinheight);
	printf("vecs %d\n", head->numverts);
	printf("tris %d\n", head->num_tris);
	printf("frames %d\n", head->num_frames);
	printf("scale %f %f %f\n", head->scale[0],
		head->scale[1], head->scale[2]);
	printf("scale_origin %f %f %f\n", head->scale[0],
		head->scale_origin[1], head->scale_origin[2]);

	vfseek(fd, head->ofs_skins, 0);
	for(i=0; i<head->num_skins; i++)
	{
		memset(namebuf, 0, 65);
		vfread(namebuf, 1, 64, fd);
		tmp->skinnames[i]=dystrdup(namebuf);

//		printf("skin %s\n", namebuf);

		skinbuf=Tex_LoadFileRaw(namebuf, &w, &h);
		if(skinbuf)
		{
			tmp->skins[i]=skinbuf;
			tmp->skinnums[i]=Tex_LoadTexture(head->skinwidth,
				head->skinheight, skinbuf, 1);
		}else
		{
			printf("missing skin %s\n", namebuf);
		}
	}

	vfseek(fd, head->ofs_st, 0);
	for(i=0; i<head->num_st; i++)
	{
		tmp->st_seam[i]=0;
		tmp->st_vecs[(i*2)+0]=ReadInt16LE(fd);
		tmp->st_vecs[(i*2)+1]=ReadInt16LE(fd);
	}

	vfseek(fd, head->ofs_tris, 0);
	for(i=0; i<head->num_tris; i++)
	{
		tmp->polys[(i*8)+0]=ReadInt16LE(fd);
		tmp->polys[(i*8)+1]=ReadInt16LE(fd);
		tmp->polys[(i*8)+2]=ReadInt16LE(fd);
		tmp->polys[(i*8)+3]=0;

		tmp->polys[(i*8)+4]=ReadInt16LE(fd);
		tmp->polys[(i*8)+5]=ReadInt16LE(fd);
		tmp->polys[(i*8)+6]=ReadInt16LE(fd);
	}

	vfseek(fd, head->ofs_frames, 0);
	for(i=0; i<head->num_frames; i++)
	{
		for(j=0; j<3; j++)scale[j]=ReadFloat32LE(fd);
		for(j=0; j<3; j++)trans[j]=ReadFloat32LE(fd);

		memset(namebuf, 0, 17);
		vfread(namebuf, 1, 16, fd);

//		printf("frame %s\n", namebuf);

		tmp->framenames[i]=dystrdup(namebuf);
		tmp->frames[i]=gcalloc(3*head->num_xyz*sizeof(float));
		vecbuf=tmp->frames[i];

		for(j=0; j<head->num_xyz; j++)
		{
			vecbuf[(j*3)+0]=(vfgetc(fd)*scale[0])+trans[0];
			vecbuf[(j*3)+1]=(vfgetc(fd)*scale[1])+trans[1];
			vecbuf[(j*3)+2]=(vfgetc(fd)*scale[2])+trans[2];
			vfgetc(fd);

			vecbuf[(j*3)+0]*=QMD2_SCALE;
			vecbuf[(j*3)+1]*=QMD2_SCALE;
			vecbuf[(j*3)+2]*=QMD2_SCALE;
		}
	}

	return(tmp);
}

int LBXGL_QuakeMDL_Render(LBXGL_QMDL *mdl, int frame, int skin)
{
	int i, j;
	float *vecs, *v;
	float s1, t1, s2, t2, s3, t3;

//	printf("render mdl %d %d\n", frame, skin);

	if(mdl->head.ident==QMD3_ID)
	{
		LBXGL_QuakeMD3_Render(mdl, frame, skin);
		return(0);
	}

	if(frame<0)frame=0;
	while(frame>=mdl->head.num_frames)
		frame-=mdl->head.num_frames;

	vecs=mdl->frames[frame];
	pdglColor4f(1, 1, 1, 1);

	if(skin<256)
	{
		if(skin<0)skin=0;
		if(skin>=mdl->head.num_skins)skin=0;
		j=mdl->skinnums[skin];
	}else j=skin-256;
	Draw_Bind(j);

	pdglBegin(PDGL_TRIANGLES);
	for(i=0; i<mdl->head.num_tris; i++)
	{
//		printf("poly %d\n", i);
		if(mdl->head.ident==QMD2_ID)
		{
			j=mdl->polys[(i*8)+4];
			s1=mdl->st_vecs[(j*2)+0]/mdl->head.skinwidth;
			t1=mdl->st_vecs[(j*2)+1]/mdl->head.skinheight;
			j=mdl->polys[(i*8)+5];
			s2=mdl->st_vecs[(j*2)+0]/mdl->head.skinwidth;
			t2=mdl->st_vecs[(j*2)+1]/mdl->head.skinheight;
			j=mdl->polys[(i*8)+6];
			s3=mdl->st_vecs[(j*2)+0]/mdl->head.skinwidth;
			t3=mdl->st_vecs[(j*2)+1]/mdl->head.skinheight;
		}else
		{
			j=mdl->polys[(i*8)+4];
			s1=mdl->st_vecs[(j*2)+0]/mdl->head.skinwidth;
			t1=mdl->st_vecs[(j*2)+1]/mdl->head.skinheight;
			if(!mdl->polys[(i*8)+3] && mdl->st_seam[j])
				s1+=0.5;

			j=mdl->polys[(i*8)+5];
			s2=mdl->st_vecs[(j*2)+0]/mdl->head.skinwidth;
			t2=mdl->st_vecs[(j*2)+1]/mdl->head.skinheight;
			if(!mdl->polys[(i*8)+3] && mdl->st_seam[j])
				s2+=0.5;

			j=mdl->polys[(i*8)+6];
			s3=mdl->st_vecs[(j*2)+0]/mdl->head.skinwidth;
			t3=mdl->st_vecs[(j*2)+1]/mdl->head.skinheight;
			if(!mdl->polys[(i*8)+3] && mdl->st_seam[j])
				s3+=0.5;

			if(!mdl->polys[(i*8)+3] && !mdl->st_seam[i])
			{
//				s1+=0.5;
//				s2+=0.5;
//				s3+=0.5;
			}
		}

		pdglTexCoord2f(s1, t1);
		j=mdl->polys[(i*8)+0];
		v=vecs+(j*3);
//		printf("(%f %f %f) (%f %f)\n",
//			v[0], v[1], v[2], s1, t1);
		pdglVertex3f(v[0], v[1], v[2]);

		pdglTexCoord2f(s2, t2);
		j=mdl->polys[(i*8)+1];
		v=vecs+(j*3);
//		printf("(%f %f %f) (%f %f)\n",
//			v[0], v[1], v[2], s2, t2);
		pdglVertex3f(v[0], v[1], v[2]);

		pdglTexCoord2f(s3, t3);
		j=mdl->polys[(i*8)+2];
		v=vecs+(j*3);
//		printf("(%f %f %f) (%f %f)\n",
//			v[0], v[1], v[2], s3, t3);
		pdglVertex3f(v[0], v[1], v[2]);
	}
	pdglEnd();
	return(0);
}

int LBXGL_QuakeMDL_RenderFlat(LBXGL_QMDL *mdl, int frame, int skin)
{
	float tv0[3], tv1[3], tv2[3];
	float *vecs, *v0, *v1, *v2;
	float s1, t1, s2, t2, s3, t3;
	int i, j;

	if(mdl->head.ident==QMD3_ID)
	{
		LBXGL_QuakeMD3_RenderFlat(mdl, frame, skin);
		return(0);
	}

	if(frame<0)frame=0;
	while(frame>=mdl->head.num_frames)
		frame-=mdl->head.num_frames;

	vecs=mdl->frames[frame];
	pdglBegin(PDGL_TRIANGLES);
	for(i=0; i<mdl->head.num_tris; i++)
	{
		v0=vecs+(mdl->polys[(i*8)+0]*3);
		v1=vecs+(mdl->polys[(i*8)+1]*3);
		v2=vecs+(mdl->polys[(i*8)+2]*3);

		V3F_SUB(v1, v0, tv0);
		V3F_SUB(v2, v0, tv1);
		V3F_CROSS(tv0, tv1, tv2);
		V3F_FRNORMALIZE(tv2, tv2);

		pdglNormal3fv(tv2);

		pdglVertex3fv(v0);
		pdglVertex3fv(v1);
		pdglVertex3fv(v2);
	}
	pdglEnd();
	return(0);
}


LBXGL_QMDL *LBXGL_QuakeMDL_LoadModel(char *name)
{
	LBXGL_QMDL *tmp;
	char *t;

	tmp=lbxgl_qmdl_cache;
	while(tmp)
	{
		if(!strcmp(tmp->name, name))
			return(tmp);
		tmp=tmp->next;
	}

	t=name+strlen(name);
	while((t>name) && (*t!='.'))t--;
	if(*t=='.')t++;

	tmp=NULL;

	printf("LBXGL_QuakeMDL_LoadModel: %s %s\n", name, t);

	if(!strcmp(t, "mdl"))
		tmp=LBXGL_QuakeMDL_Load(name, "models/q1_palette.lmp");

	if(!strcmp(t, "md2"))
		tmp=LBXGL_QuakeMD2_Load(name);

	if(!strcmp(t, "md3"))
		tmp=LBXGL_QuakeMD3_Load(name);

	if(tmp)
	{
		tmp->name=dystrdup(name);
		tmp->next=lbxgl_qmdl_cache;
		lbxgl_qmdl_cache=tmp;
	}
	return(tmp);
}

int LBXGL_QuakeMDL_IndexFrame(LBXGL_QMDL *mdl, char *name)
{
	int i;

	for(i=0; i<mdl->head.num_frames; i++)
		if(!strcmp(mdl->framenames[i], name))return(i);
	return(-1);
}

int LBXGL_QuakeMDL_GenPartVerts(LBXGL_QMDL *mdl, int *parts)
{
	return(0);
}

int LBXGL_QuakeMDL_RenderState(LBXGL_ModelState *ctx)
{
	LBXGL_QuakeMDL_Render(ctx->mdl, ctx->frame, ctx->skin);
	return(0);
}

int LBXGL_QuakeMDL_RenderStateFlat(LBXGL_ModelState *ctx)
{
	LBXGL_QuakeMDL_RenderFlat(ctx->mdl, ctx->frame, ctx->skin);
	return(0);
}

int LBXGL_QuakeMDL_RenderStateDark(LBXGL_ModelState *ctx)
{
	pdglColor4f(0, 0, 0, 1);
	LBXGL_QuakeMDL_RenderFlat(ctx->mdl, ctx->frame, ctx->skin);
	return(0);
}

int LBXGL_QuakeMDL_RenderStateLight(LBXGL_ModelState *ctx)
{
	LBXGL_QuakeMDL_RenderFlat(ctx->mdl, ctx->frame, ctx->skin);
	return(0);
}

int LBXGL_QuakeMDL_RenderStateFinal(LBXGL_ModelState *ctx)
{
	pdglBlendFunc(GL_DST_COLOR, GL_ZERO);
	LBXGL_QuakeMDL_Render(ctx->mdl, ctx->frame, ctx->skin);
	pdglBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	return(0);
}

int LBXGL_QuakeMDL_RenderStateShadow(LBXGL_ModelState *ctx,
	float *org, float rad)
{
//	LBXGL_QuakeMDL_RenderFlat(ctx->mdl, ctx->frame, ctx->skin);
	return(0);
}
