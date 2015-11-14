#include <lbxgl2.h>
#include <math.h>

int SkelVBO_IndexTexCoord(
	LBXGL_Skel2Model *mdl, LBXGL_Skel2ModelVBO *vbo,
	float *st)
{
	int i;

	for(i=0; i<vbo->n_st; i++)
	{
		if(	(vbo->arr_st[i*2+0]==st[0]) &&
			(vbo->arr_st[i*2+1]==st[1]))
		{
			return(i);
		}
	}

	if(vbo->n_st>=vbo->m_st)
	{
		i=vbo->m_st+(vbo->m_st>>1);
		if(!i)i=256;
		vbo->arr_st=gcrealloc(vbo->arr_st, i*2*sizeof(float));
		vbo->m_st=i;
	}
	
	i=vbo->n_st++;
	vbo->arr_st[i*2+0]=st[0];
	vbo->arr_st[i*2+1]=st[1];
	return(i);
}

int SkelVBO_IndexRGBA(
	LBXGL_Skel2Model *mdl, LBXGL_Skel2ModelVBO *vbo,
	float *rgba)
{
	byte rgb[4];
	int i;

	i=rgba[0]*255; i=((i<0)?0:((i>255)?255:i)); rgb[0]=i;
	i=rgba[1]*255; i=((i<0)?0:((i>255)?255:i)); rgb[1]=i;
	i=rgba[2]*255; i=((i<0)?0:((i>255)?255:i)); rgb[2]=i;
	i=rgba[3]*255; i=((i<0)?0:((i>255)?255:i)); rgb[3]=i;

	for(i=0; i<vbo->n_rgba; i++)
	{
		if(	(vbo->arr_rgba[i*4+0]==rgb[0]) &&
			(vbo->arr_rgba[i*4+1]==rgb[1]) &&
			(vbo->arr_rgba[i*4+1]==rgb[2]) &&
			(vbo->arr_rgba[i*4+1]==rgb[3]))
		{
			return(i);
		}
	}

	if(vbo->n_rgba>=vbo->m_rgba)
	{
		i=vbo->m_rgba+(vbo->m_rgba>>1);
		if(!i)i=8;
		vbo->arr_rgba=gcrealloc(vbo->arr_rgba, i*4*sizeof(byte));
		vbo->m_rgba=i;
	}
	
	i=vbo->n_rgba++;
	vbo->arr_rgba[i*4+0]=rgb[0];
	vbo->arr_rgba[i*4+1]=rgb[1];
	vbo->arr_rgba[i*4+2]=rgb[2];
	vbo->arr_rgba[i*4+3]=rgb[3];
	return(i);
}

int SkelVBO_IndexColor4f(
	LBXGL_Skel2Model *mdl, LBXGL_Skel2ModelVBO *vbo,
	float r, float g, float b, float a)
{
	float tv[4];
	tv[0]=r; tv[1]=g; tv[2]=b; tv[3]=a;
	return(SkelVBO_IndexRGBA(mdl, vbo, tv));
}

int SkelVBO_IndexVertex(
	LBXGL_Skel2Model *mdl, LBXGL_Skel2ModelVBO *vbo,
	int xyz, int st, int norm, int rgba)
{
	int i;

#if 1
	for(i=0; i<vbo->n_xyz; i++)
	{
		if(	(vbo->xyz_idx[i*4+0]==xyz) &&
			(vbo->xyz_idx[i*4+1]==st) &&
			(vbo->xyz_idx[i*4+2]==norm) &&
			(vbo->xyz_idx[i*4+3]==rgba))
		{
			return(i);
		}
	}
#endif

	if(vbo->n_xyz>=vbo->m_xyz)
	{
		i=vbo->m_xyz+(vbo->m_xyz>>1);
		if(!i)i=256;
		vbo->xyz_idx=gcrealloc(vbo->xyz_idx, i*4*sizeof(int));
		vbo->m_xyz=i;
	}
	
	i=vbo->n_xyz++;
	vbo->xyz_idx[i*4+0]=xyz;
	vbo->xyz_idx[i*4+1]=st;
	vbo->xyz_idx[i*4+2]=norm;
	vbo->xyz_idx[i*4+3]=rgba;
	return(i);
}

int SkelVBO_IndexEmitElem(
	LBXGL_Skel2Model *mdl, LBXGL_Skel2ModelVBO *vbo,
	int xyz, int st, int norm, int rgba)
{
	int i, j;

	if(vbo->n_elem>=vbo->m_elem)
	{
		i=vbo->m_elem+(vbo->m_elem>>1);
		if(!i)i=256;
		vbo->elems=gcrealloc(vbo->elems, i*sizeof(int));
		vbo->m_elem=i;
	}
	
	i=SkelVBO_IndexVertex(mdl, vbo, xyz, st, norm, rgba);
	j=vbo->n_elem++;
	vbo->elems[j]=i;
	return(j);
}

int SkelVBO_IndexBeginSegment(
	LBXGL_Skel2Model *mdl, LBXGL_Skel2ModelVBO *vbo,
	int tex)
{
	int txfl;
	int i;

	if(vbo->n_seg>=vbo->m_seg)
	{
		i=vbo->m_seg+(vbo->m_seg>>1);
		if(!i)i=256;
		vbo->seg_idx=gcrealloc(vbo->seg_idx, i*4*sizeof(int));
		vbo->m_seg=i;
	}
	
	txfl=LBXGL_Texture_GetImageFlags(tex);
	
	i=vbo->n_seg++;
	vbo->seg_idx[i*4+0]=vbo->n_elem;
	vbo->seg_idx[i*4+1]=0;
	vbo->seg_idx[i*4+2]=tex;
	vbo->seg_idx[i*4+3]=txfl;
	return(i);
}

void SkelVBO_IndexEndSegment(
	LBXGL_Skel2Model *mdl, LBXGL_Skel2ModelVBO *vbo)
{
	int i, j;

	i=vbo->n_seg-1;
	j=vbo->n_elem-vbo->seg_idx[i*4+0];

	if(j<=0)
	{
		vbo->n_seg--;
		return;
	}
	
//	vbo->seg_idx[i*4+0]=vbo->n_elem;
	vbo->seg_idx[i*4+1]=j;
//	vbo->seg_idx[i*4+2]=tex;
//	vbo->seg_idx[i*4+3]=0;
}

void SkelVBO_IndexMesh(
	LBXGL_Skel2Model *mdl, LBXGL_Skel2ModelVBO *vbo,
	LBXGL_Skel2Mesh *mesh)
{
	LBXGL_Skel2MeshFace *cur, *cur2;
//	float im[16];
//	float tv[3], tv0[3], tv1[3], tv2[3];
//	float *txyz, *tnorm, *tvnorm;
//	float *v0, *v1, *imv;
	int i, j, k, l, l0, l1, l2, l3, tn;
//	float f, g;

	if(!mesh)return;

	if(!mesh->n_vecs || !mesh->n_faces)
		return;

	if(!mesh->vidx)
	{
		return;
	}

	for(tn=0; tn<mesh->n_tex; tn++)
	{
		SkelVBO_IndexBeginSegment(mdl, vbo, mesh->texnum[tn]);

		i=mesh->texface[tn];
		while(1)
		{
			cur=mesh->face[i];

			for(j=0; j<3; j++)
			{
				l0=mesh->b_vecs+cur->vecs[j];
//				l0=cur->vecs[j];
				l1=SkelVBO_IndexTexCoord(mdl, vbo,
					mesh->stvecs+(2*cur->stvecs[j]));
				l2=mesh->b_faces*3+i*3+j;
				l3=SkelVBO_IndexColor4f(mdl, vbo, 1, 1, 1, 1);

				SkelVBO_IndexEmitElem(mdl, vbo,
					l0, l1, l2, l3);
			}

			i=cur->tnext;
			if(i<=0)break;
		}
		
		SkelVBO_IndexEndSegment(mdl, vbo);
	}
}

void SkelVBO_IndexMeshGroup(
	LBXGL_Skel2Model *mdl, LBXGL_Skel2ModelVBO *vbo,
	LBXGL_Skel2MeshGroup *grp)
{
	LBXGL_Skel2MeshGroup *gcur;
	LBXGL_Skel2Mesh *mcur;

	if(!grp)return;

	gcur=grp->first;
	while(gcur)
	{
		SkelVBO_IndexMeshGroup(mdl, vbo, gcur);
		gcur=gcur->next;
	}

	mcur=grp->prim;
	while(mcur)
	{
		SkelVBO_IndexMesh(mdl, vbo, mcur);
		mcur=mcur->next;
	}
}

void SkelVBO_ModelRebuildLodVBO(
	LBXGL_Skel2Model *mdl, int lvl)
{
	LBXGL_Skel2ModelVBO *vbo;
	LBXGL_Skel2MeshGroup *grp;
	int *elems1;
	int id, sz;
	int i, j, k, l, n;

	if(lvl>=0)
	{
		grp=mdl->mesh_lod[lvl];
		if(!grp)return;

		vbo=mdl->mesh_lod_vbos[lvl];
		if(!vbo)
		{
			vbo=gctalloc("lbxgl_skel2modelvbo_t",
				sizeof(LBXGL_Skel2ModelVBO));
			mdl->mesh_lod_vbos[lvl]=vbo;
		}
	}else
	{
		grp=mdl->mesh;
		if(!grp)return;

		vbo=mdl->mesh_vbos;
		if(!vbo)
		{
			vbo=gctalloc("lbxgl_skel2modelvbo_t",
				sizeof(LBXGL_Skel2ModelVBO));
			mdl->mesh_vbos=vbo;
		}
	}

	vbo->n_xyz=0;
	vbo->n_elem=0;
	vbo->n_seg=0;
	vbo->sz_vert=0;

	SkelVBO_IndexMeshGroup(mdl, vbo, grp);

#if 1
	//sort segments by texture
	for(i=0; i<vbo->n_seg; i++)
	{
		for(j=i+1; j<vbo->n_seg; j++)
		{
			if(vbo->seg_idx[j*4+2]<vbo->seg_idx[i*4+2])
			{
				for(k=0; k<4; k++)
				{
					l=vbo->seg_idx[i*4+k];
					vbo->seg_idx[i*4+k]=
						vbo->seg_idx[j*4+k];
					vbo->seg_idx[j*4+k]=l;
				}
			}
		}
	}
	
	//repack elements into sorted order
	elems1=gcalloc(vbo->n_elem*sizeof(int)); n=0;
	for(i=0; i<vbo->n_seg; i++)
	{
		k=vbo->seg_idx[i*4+0];
		l=vbo->seg_idx[i*4+1];
		for(j=0; j<l; j++)
			{ elems1[n+j]=vbo->elems[k+j]; }
		vbo->seg_idx[i*4+0]=n;
		n+=l;
		if(n>vbo->n_elem)
			{ *(int *)-1=-1; }
	}
	for(i=0; i<n; i++)
		{ vbo->elems[i]=elems1[i]; }
	gcfree(elems1);
#endif

#if 1
	n=0;
	for(i=0; i<vbo->n_seg; i++)
	{
		l=vbo->seg_idx[i*4+1];
		for(j=i+1; j<vbo->n_seg; j++)
		{
			if(vbo->seg_idx[j*4+2]!=vbo->seg_idx[i*4+2])
				break;
			l+=vbo->seg_idx[j*4+1];
		}
		vbo->seg_idx[n*4+0]=
			vbo->seg_idx[i*4+0];
		vbo->seg_idx[n*4+1]=l;
		vbo->seg_idx[n*4+2]=
			vbo->seg_idx[i*4+2];
		vbo->seg_idx[n*4+3]=
			vbo->seg_idx[i*4+3];
		n++; i=j-1;
	}
	vbo->n_seg=n;
#endif

	sz=vbo->n_elem*sizeof(int);
	pdglGenBuffers(1, &id);
	pdglBindBuffer(GL_ELEMENT_ARRAY_BUFFER, id);
	pdglBufferData(GL_ELEMENT_ARRAY_BUFFER,
		sz, vbo->elems, GL_STATIC_DRAW);
	vbo->vbo_elem=id;
	pdglBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

#if 0
	pdglGenBuffers(1, &id);
	pdglBindBuffer(GL_ARRAY_BUFFER, id);
	pdglBufferData(GL_ARRAY_BUFFER, sz, buf, GL_STATIC_DRAW);
	vbo->vbo_face=id;

	free(buf);

	pdglBindBuffer(GL_ARRAY_BUFFER, 0);
#endif

	vbo->sz_xyz=vbo->n_xyz*3*sizeof(float);
	vbo->sz_st=vbo->n_xyz*2*sizeof(float);
	vbo->sz_norm=vbo->n_xyz*3*sizeof(s8);
	vbo->sz_rgba=vbo->n_xyz*4*sizeof(u8);

//	vbo->sz_vert=vbo->sz_xyz+vbo->sz_st+
//		vbo->sz_norm+vbo->sz_rgba;
		
	i=0;
	vbo->ofs_xyz=i;
	i+=vbo->sz_xyz;
	i=(i+15)&(~15);

	vbo->ofs_st=i;
	i+=vbo->sz_st;
	i=(i+15)&(~15);

	vbo->ofs_norm=i;
	i+=vbo->sz_norm;
	i=(i+15)&(~15);

	vbo->ofs_rgba=i;
	i+=vbo->sz_rgba;
	i=(i+15)&(~15);

	vbo->ofs_vlrgba=i;
	i+=vbo->sz_rgba;
	i=(i+15)&(~15);

	vbo->sz_vert=i+64;
}

LBXGL_Skel2ModelVBO *SkelVBO_VBOForLod(LBXGL_Skel2State *ctx)
{
	LBXGL_Skel2ModelVBO *vbo;

	if((ctx->lod>=8) && ctx->mdl->mesh_lod[7])
		return(ctx->mdl->mesh_lod_vbos[7]);
	if((ctx->lod>=7) && ctx->mdl->mesh_lod[6])
		return(ctx->mdl->mesh_lod_vbos[6]);
	if((ctx->lod>=6) && ctx->mdl->mesh_lod[5])
		return(ctx->mdl->mesh_lod_vbos[5]);
	if((ctx->lod>=5) && ctx->mdl->mesh_lod[4])
		return(ctx->mdl->mesh_lod_vbos[4]);
	if((ctx->lod>=4) && ctx->mdl->mesh_lod[3])
		return(ctx->mdl->mesh_lod_vbos[3]);
	if((ctx->lod>=3) && ctx->mdl->mesh_lod[2])
		return(ctx->mdl->mesh_lod_vbos[2]);
	if((ctx->lod>=2) && ctx->mdl->mesh_lod[1])
		return(ctx->mdl->mesh_lod_vbos[1]);
	if((ctx->lod>=1) && ctx->mdl->mesh_lod[0])
		return(ctx->mdl->mesh_lod_vbos[0]);
	return(ctx->mdl->mesh_vbos);
}

void SkelVBO_RebuildVBO(LBXGL_Skel2State *ctx)
{
	LBXGL_Skel2ModelVBO *vbo;
	LBXGL_Skel2ModelVBO *vbo_base;

	float *xyz, *st;
	s8 *norm;
	u8 *rgba, *vlrgba;
	int id;
	int i, j;

	vbo=SkelVBO_VBOForLod(ctx);
	ctx->vbo_info=vbo;
	vbo_base=ctx->mdl->mesh_vbos;

//	ctx->vbo_info=ctx->mdl->mesh_lod_vbos[ctx->lod];
	if(!ctx->vbo_info)
		return;
	
	ctx->vbo_elem=ctx->vbo_info->vbo_elem;
	
	if(!ctx->vbo_vertbuf)
	{
		i=vbo_base->sz_vert;
		ctx->vbo_vertbuf=gcalloc(i);

		if(!i)
			{ *(int *)-1=-1; }
	}
	
	if(!ctx->vbo_info->sz_vert)
		{ *(int *)-1=-1; }

	i=vbo_base->sz_vert;
	if(i<ctx->vbo_info->sz_vert)
		{ *(int *)-1=-1; }
	
	xyz=(float *)(ctx->vbo_vertbuf+vbo->ofs_xyz);
	st=(float *)(ctx->vbo_vertbuf+vbo->ofs_st);
	norm=(s8 *)(ctx->vbo_vertbuf+vbo->ofs_norm);
	rgba=(u8 *)(ctx->vbo_vertbuf+vbo->ofs_rgba);
	vlrgba=(u8 *)(ctx->vbo_vertbuf+vbo->ofs_vlrgba);

//	memset(vlrgba, 0, vbo->sz_rgba);

	if((vbo->ofs_rgba+(vbo->n_xyz*4))>=
		vbo_base->sz_vert)
	{
		*(int *)-1=-1;
	}

#if 1
	for(i=0; i<ctx->vbo_info->n_xyz; i++)
	{
		j=ctx->vbo_info->xyz_idx[i*4+0];
		V3F_COPY(ctx->cur_xyz+j*3, xyz+i*3);

		j=ctx->vbo_info->xyz_idx[i*4+1];
		V2F_COPY(ctx->vbo_info->arr_st+j*2, st+i*2);

		j=ctx->vbo_info->xyz_idx[i*4+2];
//		V3F_COPY(ctx->cur_vnorm+j*3, norm+i*3);
		V3F_SCALE(ctx->cur_vnorm+j*4, 126, norm+i*3);

		j=ctx->vbo_info->xyz_idx[i*4+3];
		V4F_COPY(ctx->vbo_info->arr_rgba+j*4, rgba+i*4);
		
		vlrgba[i*4+0]=0;
		vlrgba[i*4+1]=0;
		vlrgba[i*4+2]=0;
		vlrgba[i*4+3]=255;
	}
#endif

#if 1
	id=ctx->vbo_vert;
	if(id<=0)
	{
		pdglGenBuffers(1, &id);
		ctx->vbo_vert=id;

		pdglBindBuffer(GL_ARRAY_BUFFER, id);
		pdglBufferData(GL_ARRAY_BUFFER,
			vbo_base->sz_vert,
			ctx->vbo_vertbuf,
			GL_DYNAMIC_DRAW);
//			GL_STATIC_DRAW);
		pdglBindBuffer(GL_ARRAY_BUFFER, 0);
	}else
	{
		pdglBindBuffer(GL_ARRAY_BUFFER, id);

		pdglBufferSubData(GL_ARRAY_BUFFER, 0, 
			vbo_base->sz_vert, ctx->vbo_vertbuf);

//		pdglBufferData(GL_ARRAY_BUFFER,
//			vbo_base->sz_vert,
//			ctx->vbo_vertbuf,
//			GL_STATIC_DRAW);
		pdglBindBuffer(GL_ARRAY_BUFFER, 0);
	}
#endif
}

void SkelVBO_DestroyVBO(LBXGL_Skel2State *ctx)
{
	if(ctx->vbo_vert>0)
	{
		pdglDeleteBuffers(1, &(ctx->vbo_vert));
		ctx->vbo_vert=0;
	}

	if(ctx->vbo_vertbuf)
	{
		gcfree(ctx->vbo_vertbuf);
		ctx->vbo_vertbuf=NULL;
	}
}

LBXGL_API void SkelVBO_DrawModelFlatVL(LBXGL_Skel2State *ctx)
{
	LBXGL_Skel2ModelVBO *vbo;
	int tn, b, l;
	int i;

	vbo=ctx->vbo_info;
	if(!vbo)return;

	pdglBindBuffer(GL_ARRAY_BUFFER, ctx->vbo_vert);
	pdglBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ctx->vbo_elem);

	for(i=0; i<vbo->n_seg; i++)
	{
		b=vbo->seg_idx[i*4+0];
		l=vbo->seg_idx[i*4+1];
		tn=vbo->seg_idx[i*4+2];
		LBXGL_Shader_BindTexture(tn);

#if 1
		LBXGL_Shader_DrawElementsNormalTexRGB(
			PDGL_TRIANGLES, l, GL_UNSIGNED_INT, (byte *)(b*sizeof(int)),
			3, GL_FLOAT, 0, (byte *)vbo->ofs_xyz,
			2, GL_FLOAT, 0, (byte *)vbo->ofs_st,
			3, GL_BYTE, 3, (byte *)vbo->ofs_norm,
			4, GL_UNSIGNED_BYTE, 0, (byte *)vbo->ofs_vlrgba);
#endif
	}

	pdglBindBuffer(GL_ARRAY_BUFFER, 0);
	pdglBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

LBXGL_API void SkelVBO_DrawModelFlat(LBXGL_Skel2State *ctx)
{
	LBXGL_Skel2ModelVBO *vbo;
	int tn, b, l;
	int i;

	vbo=ctx->vbo_info;
	if(!vbo)return;

	pdglBindBuffer(GL_ARRAY_BUFFER, ctx->vbo_vert);
	pdglBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ctx->vbo_elem);

	for(i=0; i<vbo->n_seg; i++)
	{
		b=vbo->seg_idx[i*4+0];
		l=vbo->seg_idx[i*4+1];
		tn=vbo->seg_idx[i*4+2];
		LBXGL_Shader_BindTexture(tn);

#if 0
		LBXGL_Shader_DrawArraysNormalTexRGB(
			PDGL_TRIANGLES, b, l,
			3, GL_FLOAT, 0, (byte *)vbo->ofs_xyz,
			2, GL_FLOAT, 0, (byte *)vbo->ofs_st,
			3, GL_BYTE, 3, (byte *)vbo->ofs_norm,
			4, GL_UNSIGNED_BYTE, 0, (byte *)vbo->ofs_rgba);
#endif

#if 0
		LBXGL_Shader_DrawElementsNormalTexRGB(
			PDGL_TRIANGLES, l, GL_UNSIGNED_INT, (byte *)(b*sizeof(int)),
			3, GL_FLOAT, 0, (byte *)vbo->ofs_xyz,
			2, GL_FLOAT, 0, (byte *)vbo->ofs_st,
			3, GL_BYTE, 3, (byte *)vbo->ofs_norm,
			4, GL_UNSIGNED_BYTE, 0, (byte *)vbo->ofs_rgba);
#endif

#if 1
		LBXGL_Shader_DrawElementsNormalTexRGB(
			PDGL_TRIANGLES, l, GL_UNSIGNED_INT, (byte *)(b*sizeof(int)),
			3, GL_FLOAT, 0, (byte *)vbo->ofs_xyz,
			2, GL_FLOAT, 0, (byte *)vbo->ofs_st,
			3, GL_BYTE, 3, (byte *)vbo->ofs_norm,
			4, GL_UNSIGNED_BYTE, 0, (byte *)vbo->ofs_rgba);
#endif
	}

	pdglBindBuffer(GL_ARRAY_BUFFER, 0);
	pdglBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

LBXGL_API void SkelVBO_DrawModelFinal(LBXGL_Skel2State *ctx)
{
	LBXGL_Skel2ModelVBO *vbo;
	int tn, b, l;
	int i;

	vbo=ctx->vbo_info;
	if(!vbo)return;

	pdglBindBuffer(GL_ARRAY_BUFFER, ctx->vbo_vert);
	pdglBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ctx->vbo_elem);

	for(i=0; i<vbo->n_seg; i++)
	{
		b=vbo->seg_idx[i*4+0];
		l=vbo->seg_idx[i*4+1];
		tn=vbo->seg_idx[i*4+2];
		LBXGL_Shader_BindTexture(tn);

#if 0
		LBXGL_Shader_DrawArraysNormalTexRGB(
			PDGL_TRIANGLES, b, l,
			3, GL_FLOAT, 0, (byte *)vbo->ofs_xyz,
			2, GL_FLOAT, 0, (byte *)vbo->ofs_st,
			3, GL_BYTE, 3, (byte *)vbo->ofs_norm,
			4, GL_UNSIGNED_BYTE, 0, (byte *)vbo->ofs_rgba);
#endif

#if 1
		LBXGL_Shader_DrawElementsNormalTexRGB(
			PDGL_TRIANGLES, l, GL_UNSIGNED_INT, (byte *)(b*sizeof(int)),
			3, GL_FLOAT, 0, (byte *)vbo->ofs_xyz,
			2, GL_FLOAT, 0, (byte *)vbo->ofs_st,
			3, GL_BYTE, 3, (byte *)vbo->ofs_norm,
			4, GL_UNSIGNED_BYTE, 0, (byte *)vbo->ofs_rgba);
#endif
	}

	pdglBindBuffer(GL_ARRAY_BUFFER, 0);
	pdglBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}
