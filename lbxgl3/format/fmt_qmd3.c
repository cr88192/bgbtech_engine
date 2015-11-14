#include <lbxgl2.h>

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

LBXGL_QMDL *LBXGL_QuakeMD3_Load(char *name)
{
	char namebuf[65];
	LBXGL_QMDL *tmp;
	quake_qmdl_t *head;
	byte *skinbuf;
	float *vecbuf;
	quake_md3surf_t *surf;
	VFILE *fd;
	char *s, *t;
	int i, j, k, base;
	int w, h;

	fd=vffopen(name, "rb");
	if(!fd)return(NULL);

	printf("loading md3 %s\n", name);

	tmp=gctalloc("lbxgl_quakemdl_t", sizeof(LBXGL_QMDL));
	head=&tmp->head;

	head->ident=ReadInt32LE(fd);
	head->version=ReadInt32LE(fd);

	if(head->ident==QMD4_ID)
	{
		tmp->name=dystrdup(name);
		LBXGL_QuakeMD4_StageLoad(tmp, fd);
		return(tmp);
	}

	if(head->ident!=QMD3_ID)
	{
		printf("LBXGL_QuakeMD3_Load(%s): Invalid Ident\n", name);
		return(NULL);
	}
	if(head->version!=QMD3_VERSION)
	{
		printf("LBXGL_QuakeMD3_Load(%s): Invalid Version\n", name);
		return(NULL);
	}
	vfread(namebuf, 1, 64, fd);
	head->flags=ReadInt32LE(fd);

	head->num_frames=ReadInt32LE(fd);
	head->num_tags=ReadInt32LE(fd);
	head->num_surfaces=ReadInt32LE(fd);
	head->num_skins=ReadInt32LE(fd);

	head->ofs_frames=ReadInt32LE(fd);
	head->ofs_tags=ReadInt32LE(fd);
	head->ofs_surfaces=ReadInt32LE(fd);
	head->ofs_end=ReadInt32LE(fd);

	tmp->tags=gcalloc(12*head->num_tags*sizeof(float));
	tmp->tagnames=gcalloc(head->num_tags*sizeof(char *));

	tmp->surfs=gcalloc(head->num_surfaces*sizeof(quake_md3surf_t *));

	vfseek(fd, head->ofs_frames, 0);
	for(i=0; i<head->num_frames; i++)
	{
		for(j=0; j<10; j++)ReadFloat32LE(fd);
		vfread(namebuf, 1, 16, fd);
		printf("frame name: '%s'\n", namebuf);
	}

	vfseek(fd, head->ofs_tags, 0);
	for(i=0; i<head->num_tags; i++)
	{
		vfread(namebuf, 1, 64, fd);
		printf("tag name: '%s'\n", namebuf);
		printf("tag values:");
		for(j=0; j<12; j++)
		{
			tmp->tags[(i*12)+j]=ReadFloat32LE(fd);
			printf(" %f", tmp->tags[(i*12)+j]);
		}
		printf("\n");
	}

	base=head->ofs_surfaces;
	for(i=0; i<head->num_surfaces; i++)
	{
		vfseek(fd, base, 0);

		surf=gcalloc(sizeof(quake_md3surf_t));
		tmp->surfs[i]=surf;

		surf->ident=ReadInt32LE(fd);
		vfread(surf->name, 1, 64, fd);
		surf->flags=ReadInt32LE(fd);
		surf->num_frames=ReadInt32LE(fd);
		surf->num_shaders=ReadInt32LE(fd);
		surf->num_verts=ReadInt32LE(fd);
		surf->num_triangles=ReadInt32LE(fd);
		surf->ofs_triangles=ReadInt32LE(fd);
		surf->ofs_shaders=ReadInt32LE(fd);
		surf->ofs_st=ReadInt32LE(fd);
		surf->ofs_xyz=ReadInt32LE(fd);
		surf->ofs_end=ReadInt32LE(fd);

		printf("surface name: '%s'\n", surf->name);
		printf("surface frames: %d shaders: %d verts: %d tris: %d\n",
			surf->num_frames, surf->num_shaders, surf->num_verts,
			surf->num_triangles);

		surf->tris=gcalloc(3*surf->num_triangles*sizeof(int));
		surf->shaders=gcalloc(surf->num_shaders*sizeof(char *));
		surf->st=gcalloc(2*surf->num_verts*sizeof(int));
		surf->frames=gcalloc(surf->num_frames*sizeof(float *));
		surf->sdrnums=gcalloc(surf->num_shaders*sizeof(int));

		vfseek(fd, base+surf->ofs_triangles, 0);
		for(j=0; j<surf->num_triangles; j++)
		{
			surf->tris[(j*3)+0]=ReadInt32LE(fd);
			surf->tris[(j*3)+1]=ReadInt32LE(fd);
			surf->tris[(j*3)+2]=ReadInt32LE(fd);
		}

		vfseek(fd, base+surf->ofs_shaders, 0);
		for(j=0; j<surf->num_shaders; j++)
		{
			vfread(namebuf, 1, 64, fd);
			surf->shaders[j]=dystrdup(namebuf);
			ReadInt32LE(fd);

			s=namebuf+strlen(namebuf);
			while((s>namebuf) && (*s!='/') && (*s!='.'))s--;
			if(*s=='.')*s=0;

			printf("shader name: '%s'\n", namebuf);

			surf->sdrnums[j]=LBXGL_Texture_LoadImage(namebuf);
			if(surf->sdrnums[j]<0)
			{
				printf("missing skin %s\n", namebuf);
				surf->sdrnums[j]=0;
			}
#if 0
			skinbuf=Tex_LoadFileRaw(namebuf, &w, &h);
			if(skinbuf)
			{
				surf->sdrnums[j]=Tex_LoadTexture(w, h,
					skinbuf, 1);
				free(skinbuf);
			}else
			{
				printf("missing skin %s\n", namebuf);
			}
#endif
		}

		vfseek(fd, base+surf->ofs_st, 0);
		for(j=0; j<surf->num_verts; j++)
		{
			surf->st[(j*2)+0]=ReadFloat32LE(fd);
			surf->st[(j*2)+1]=ReadFloat32LE(fd);

//			printf("st(%d): %f %f\n", j,
//				surf->st[(j*2)+0], surf->st[(j*2)+1]);
		}

		vfseek(fd, base+surf->ofs_xyz, 0);
		for(j=0; j<surf->num_frames; j++)
		{
			vecbuf=gcalloc(3*surf->num_verts*sizeof(float));
			surf->frames[j]=vecbuf;
			for(k=0; k<surf->num_verts; k++)
			{
				vecbuf[(k*3)+0]=ReadInt16LE(fd)/64.0;
				vecbuf[(k*3)+1]=ReadInt16LE(fd)/64.0;
				vecbuf[(k*3)+2]=ReadInt16LE(fd)/64.0;
				ReadInt16LE(fd);

				vecbuf[(k*3)+0]*=QMD3_SCALE;
				vecbuf[(k*3)+1]*=QMD3_SCALE;
				vecbuf[(k*3)+2]*=QMD3_SCALE;
			}
		}

		base=base+surf->ofs_end;
	}

	printf("\n");

	return(tmp);
}

int LBXGL_QuakeMD4_LoadSurfs(LBXGL_QMDL *tmp,
	quake_md4lod_t *lod, VFILE *fd, int base)
{
	quake_qmdl_t *head;
	quake_md3surf_t *surf;
	quake_md4vertex_t *vert;
	int i, j, k;
	char namebuf[65];

	head=&tmp->head;

	lod->surfs=gcalloc(lod->num_surfaces*sizeof(quake_md3surf_t *));
	base+=lod->ofs_surfaces;

	for(i=0; i<lod->num_surfaces; i++)
	{
		vfseek(fd, base, 0);

		surf=gcalloc(sizeof(quake_md3surf_t));
		tmp->surfs[i]=surf;

		surf->ident=ReadInt32LE(fd);
		vfread(surf->name, 1, 64, fd);
		vfread(surf->shader, 1, 64, fd);
		ReadInt32LE(fd);
		surf->ofs_header=ReadInt32LE(fd);

		surf->num_verts=ReadInt32LE(fd);
		surf->ofs_verts=ReadInt32LE(fd);

		surf->num_triangles=ReadInt32LE(fd);
		surf->ofs_triangles=ReadInt32LE(fd);

		surf->num_bonerefs=ReadInt32LE(fd);
		surf->ofs_bonerefs=ReadInt32LE(fd);
		surf->ofs_end=ReadInt32LE(fd);

		printf("surface name: '%s'\n", surf->name);
		printf("surface shader: '%s'\n", surf->shader);
		printf("surface verts: %d tris: %d bonerefs: %d\n",
			surf->num_verts, surf->num_triangles,
			surf->num_bonerefs);

		surf->tris=gcalloc(3*surf->num_triangles*sizeof(int));
		surf->md4verts=gcalloc(surf->num_verts*sizeof(quake_md4vertex_t *));
		surf->bonerefs=gcalloc(surf->num_bonerefs*sizeof(int));

		vfseek(fd, base+surf->ofs_verts, 0);
		for(j=0; j<surf->num_verts; j++)
		{
			vert=gcalloc(sizeof(quake_md4vertex_t));
			surf->md4verts[j]=vert;

			for(k=0; k<3; k++)
				vert->vertex[k]=ReadFloat32LE(fd);
			for(k=0; k<3; k++)
				vert->normal[k]=ReadFloat32LE(fd);
			for(k=0; k<2; k++)
				vert->st[k]=ReadFloat32LE(fd);

			vert->num_weights=ReadInt32LE(fd);
			vert->bone_indices=gcalloc(vert->num_weights*sizeof(int));
			vert->bone_weights=gcalloc(vert->num_weights*sizeof(float));

			for(k=0; k<vert->num_weights; k++)
			{
				vert->bone_indices[k]=ReadInt32LE(fd);
				vert->bone_weights[k]=ReadFloat32LE(fd);
			}
		}

		vfseek(fd, base+surf->ofs_triangles, 0);
		for(j=0; j<surf->num_triangles; j++)
		{
			surf->tris[(j*3)+0]=ReadInt32LE(fd);
			surf->tris[(j*3)+1]=ReadInt32LE(fd);
			surf->tris[(j*3)+2]=ReadInt32LE(fd);
		}

		vfseek(fd, base+surf->ofs_bonerefs, 0);
		for(j=0; j<surf->num_bonerefs; j++)
			surf->bonerefs[i]=ReadInt32LE(fd);

		base=base+surf->ofs_end;
	}
	return(0);
}

LBXGL_QMDL *LBXGL_QuakeMD4_StageLoad(LBXGL_QMDL *tmp, VFILE *fd)
{
	quake_qmdl_t *head;
	float *vecbuf;
	quake_md3surf_t *surf;
	quake_md4lod_t *lod;
	int i, j, k, base;
	char namebuf[65];

	printf("loading md4 %s\n", tmp->name);
	head=&tmp->head;

//	head->ident=ReadInt32LE(fd);
//	head->version=ReadInt32LE(fd);

	if(head->ident!=QMD4_ID)
	{
		printf("LBXGL_QuakeMD4_Load(%s): Invalid Ident\n", tmp->name);
		return(NULL);
	}
	if(head->version!=QMD4_VERSION)
	{
		printf("LBXGL_QuakeMD4_Load(%s): Invalid Version\n", tmp->name);
		return(NULL);
	}
	vfread(namebuf, 1, 64, fd);

	head->num_frames=ReadInt32LE(fd);
	head->num_bones=ReadInt32LE(fd);
	head->ofs_frames=ReadInt32LE(fd);

	head->num_lods=ReadInt32LE(fd);
	head->ofs_lods=ReadInt32LE(fd);

	head->ofs_end=ReadInt32LE(fd);

	tmp->frames=gcalloc(head->num_frames*sizeof(float *));
	tmp->framenames=gcalloc(head->num_frames*sizeof(char *));

	tmp->lods=gcalloc(head->num_lods*sizeof(quake_md4lod_t *));

	vfseek(fd, head->ofs_frames, 0);
	for(i=0; i<head->num_frames; i++)
	{
		for(j=0; j<10; j++)ReadFloat32LE(fd);
		vfread(namebuf, 1, 16, fd);
		printf("frame name: '%s'\n", namebuf);

		tmp->frames[i]=gcalloc(12*head->num_bones*sizeof(float));
		vecbuf=tmp->frames[i];

		for(j=0; j<head->num_bones*12; j++)
			vecbuf[j]=ReadFloat32LE(fd);
	}

	base=head->ofs_lods;
	for(i=0; i<head->num_lods; i++)
	{
		vfseek(fd, base, 0);

		lod=gcalloc(sizeof(quake_md4lod_t));
		tmp->lods[i]=lod;

		lod->num_surfaces=ReadInt32LE(fd);
		lod->ofs_surfaces=ReadInt32LE(fd);
		lod->ofs_end=ReadInt32LE(fd);

		printf("lod: %d surfs: %d\n", i, lod->num_surfaces);

		LBXGL_QuakeMD4_LoadSurfs(tmp, lod, fd, base);

		base+=lod->ofs_end;
	}

	return(0);
}

int LBXGL_QuakeMD3_Render(LBXGL_QMDL *mdl, int frame, int skin)
{
	int i, j, k;
	quake_md3surf_t *surf;
	float *vecs, *v;
	float s1, t1, s2, t2, s3, t3;

//	printf("render mdl %d %d\n", frame, skin);

//	return(0);

	if(frame<0)frame=0;
	while(frame>=mdl->head.num_frames)
		frame-=mdl->head.num_frames;

//	vecs=mdl->frames[frame];
	pdglColor4f(1, 1, 1, 1);

//	if(skin<256)
//	{
//		if(skin<0)skin=0;
//		if(skin>=mdl->head.num_skins)skin=0;
//		j=mdl->skinnums[skin];
//	}else j=skin-256;
//	Draw_Bind(j);

//	glDisable(GL_TEXTURE_2D);

	for(i=0; i<mdl->head.num_surfaces; i++)
	{
		surf=mdl->surfs[i];
		vecs=surf->frames[frame];

		if(skin<256)
		{
			if(skin<0)skin=0;
			if(skin>=surf->num_shaders)skin=0;
			j=surf->sdrnums[skin];
		}else j=skin-256;
		Draw_Bind(j);

		pdglBegin(PDGL_TRIANGLES);
		for(j=0; j<surf->num_triangles; j++)
		{
//			printf("poly %d\n", i);

			k=surf->tris[(j*3)+2];
			s1=surf->st[(k*2)+0];
			t1=1-surf->st[(k*2)+1];

			k=surf->tris[(j*3)+1];
			s2=surf->st[(k*2)+0];
			t2=1-surf->st[(k*2)+1];

			k=surf->tris[(j*3)+0];
			s3=surf->st[(k*2)+0];
			t3=1-surf->st[(k*2)+1];

			pdglTexCoord2f(s1, t1);
			k=surf->tris[(j*3)+2];
			v=vecs+(k*3);
//			printf("(%f %f %f) (%f %f)\n",
//				v[0], v[1], v[2], s1, t1);
			pdglVertex3f(v[0], v[1], v[2]);

			pdglTexCoord2f(s2, t2);
			k=surf->tris[(j*3)+1];
			v=vecs+(k*3);
//			printf("(%f %f %f) (%f %f)\n",
//				v[0], v[1], v[2], s2, t2);
			pdglVertex3f(v[0], v[1], v[2]);

			pdglTexCoord2f(s3, t3);
			k=surf->tris[(j*3)+0];
			v=vecs+(k*3);
//			printf("(%f %f %f) (%f %f)\n",
//				v[0], v[1], v[2], s3, t3);
			pdglVertex3f(v[0], v[1], v[2]);
		}
		pdglEnd();
	}

//	glEnable(GL_TEXTURE_2D);

	return(0);
}


int LBXGL_QuakeMD3_RenderFlat(LBXGL_QMDL *mdl, int frame, int skin)
{
	float tv0[3], tv1[3], tv2[3];
	quake_md3surf_t *surf;
	float *vecs, *v0, *v1, *v2;
	float s1, t1, s2, t2, s3, t3;
	int i, j, k;

	if(frame<0)frame=0;
	while(frame>=mdl->head.num_frames)
		frame-=mdl->head.num_frames;

//	pdglColor4f(1, 1, 1, 1);

	for(i=0; i<mdl->head.num_surfaces; i++)
	{
		surf=mdl->surfs[i];
		vecs=surf->frames[frame];

		pdglBegin(PDGL_TRIANGLES);
		for(j=0; j<surf->num_triangles; j++)
		{
			v0=vecs+(surf->tris[(j*3)+2]*3);
			v1=vecs+(surf->tris[(j*3)+1]*3);
			v2=vecs+(surf->tris[(j*3)+0]*3);

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
	}

//	glEnable(GL_TEXTURE_2D);

	return(0);
}
