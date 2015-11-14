#include <lbxgl2.h>


#define FCC_ItCF	BSP_FOURCC('I', 't', 'C', 'F')
#define FCC_enD_	BSP_FOURCC('e', 'n', 'D', '~')

#define FCC_bsp		BSP_FOURCC('b', 's', 'p', ' ')
#define FCC_eNts	BSP_FOURCC('e', 'N', 't', 's')
#define FCC_fAce	BSP_FOURCC('f', 'A', 'c', 'e')
#define FCC_fAc1	BSP_FOURCC('f', 'A', 'c', '1')
#define FCC_Node	BSP_FOURCC('N', 'o', 'd', 'e')
#define FCC_nOde	BSP_FOURCC('n', 'O', 'd', 'e')
#define FCC_Leaf	BSP_FOURCC('L', 'e', 'a', 'f')
#define FCC_lEaf	BSP_FOURCC('l', 'E', 'a', 'f')
#define FCC_lMi		BSP_FOURCC('l', 'M', 'i', ' ')
#define FCC_tExi	BSP_FOURCC('t', 'E', 'x', 'i')
#define FCC_Mdl 	BSP_FOURCC('M', 'd', 'l', ' ')
#define FCC_hEad	BSP_FOURCC('h', 'E', 'a', 'd')
#define FCC_vIs		BSP_FOURCC('v', 'I', 's', ' ')
#define FCC_pRt		BSP_FOURCC('p', 'R', 't', ' ')
#define FCC_tSrf	BSP_FOURCC('t', 'S', 'r', 'f')
#define FCC_sStr	BSP_FOURCC('s', 'S', 't', 'r')

BSP_Node *lbxgl_bsp_nodestack[256];
int lbxgl_bsp_nodestackpos;

int BSP_GetFccMode(int fcc)
{
	int i, j;

	i=0;
	j=fcc&0xFF;
	if(((j>='A') && (j<='Z')) || (j=='~'))i|=1;
	j=(fcc>>8)&0xFF;
	if(((j>='A') && (j<='Z')) || (j=='~'))i|=2;
	j=(fcc>>16)&0xFF;
	if(((j>='A') && (j<='Z')) || (j=='~'))i|=4;
	j=(fcc>>24)&0xFF;
	if(((j>='A') && (j<='Z')) || (j=='~'))i|=8;

	return(i);
}

void BSP_SkipTag(VFILE *fd, int fcc)
{
	int i, j;

	switch(BSP_GetFccMode(fcc)&3)
	{
	case 0:
		break;

	case 1:
		while(1)
		{
			i=BSP_ReadFourcc(fd);
			if(i==FCC_enD_)break;
			BSP_SkipTag(fd, i);
		}
		break;

	case 2:
		j=BSP_ReadInt32(fd);
		vfseek(fd, j, 1);
		break;

	case 3:
		j=BSP_ReadInt32(fd);
		vfseek(fd, j, 1);
		break;
	}
}

BSP_Face *BSP_ReadLeafFace(VFILE *fd)
{
	float pt[3], dv[3], dn[4];
	BSP_Face *tmp;
	int i, j;

	tmp=gcalloc(sizeof(BSP_Face));
	memset(tmp, 0, sizeof(BSP_Face));

//	BSP_ReadFourcc(fd);
	BSP_ReadInt32(fd);

	BSP_ReadNormal(fd, tmp->norm);
	tmp->nvecs=BSP_ReadInt16(fd);
	tmp->flags=BSP_ReadInt16(fd);

	tmp->vecs=gcatomic(tmp->nvecs*3*sizeof(float));
	for(i=0; i<tmp->nvecs; i++)
		BSP_ReadVector(fd, tmp->vecs+i*3);

	tmp->evecs=gcatomic(4*tmp->nvecs*sizeof(float));
	for(i=0; i<tmp->nvecs; i++)
	{
		j=(i+1)%tmp->nvecs;
		V3F_SUB(tmp->vecs+j*3, tmp->vecs+i*3, dv);
		V3F_CROSS(tmp->norm, dv, dn);
		V3F_NORMALIZE(dn, dn);
		dn[3]=V3F_DOT(tmp->vecs+i*3, dn);
		V4F_COPY(dn, tmp->evecs+i*4);
	}

	return(tmp);
}

BSP_Node *BSP_FindLeaf(BSP_Node *node, int num)
{
	BSP_Node *ntmp;

	if(!node->lnode)
	{
		if(node->num==num)return(node);
		return(NULL);
	}

	ntmp=BSP_FindLeaf(node->lnode, num);
	if(ntmp)return(ntmp);

	ntmp=BSP_FindLeaf(node->rnode, num);
	return(ntmp);
}

BSP_Face *BSP_ReadPortal(VFILE *fd, BSP_Node *node)
{
	float pt[3], dv[3], dn[4];
	BSP_Face *tmp;
	int i, j;

	tmp=gcalloc(sizeof(BSP_Face));
	memset(tmp, 0, sizeof(BSP_Face));

//	BSP_ReadFourcc(fd);
	BSP_ReadInt32(fd);

	V4F_COPY(node->norm, tmp->norm);

	tmp->nvecs=BSP_ReadInt16(fd);
	tmp->flags=BSP_ReadInt16(fd);
	i=BSP_ReadInt16(fd);
	j=BSP_ReadInt16(fd);

	tmp->lleaf=BSP_FindLeaf(node, i);
	tmp->rleaf=BSP_FindLeaf(node, j);

	tmp->llnext=tmp->lleaf->prt;
	tmp->lleaf->prt=tmp;

	tmp->rlnext=tmp->rleaf->prt;
	tmp->rleaf->prt=tmp;


	tmp->vecs=gcatomic(tmp->nvecs*3*sizeof(float));
	for(i=0; i<tmp->nvecs; i++)
		BSP_ReadVector(fd, tmp->vecs+i*3);

	tmp->evecs=gcatomic(4*tmp->nvecs*sizeof(float));
	for(i=0; i<tmp->nvecs; i++)
	{
		j=(i+1)%tmp->nvecs;
		V3F_SUB(tmp->vecs+j*3, tmp->vecs+i*3, dv);
		V3F_CROSS(tmp->norm, dv, dn);
		V3F_NORMALIZE(dn, dn);
		dn[3]=V3F_DOT(tmp->vecs+i*3, dn);
		V4F_COPY(dn, tmp->evecs+i*4);
	}

	return(tmp);
}

BSP_Face *BSP_ReadTraceSurf(VFILE *fd, BSP_Node *node)
{
	float pt[3], dv[3], dn[4];
	BSP_Face *tmp;
	int i, j;

	tmp=gcalloc(sizeof(BSP_Face));
	memset(tmp, 0, sizeof(BSP_Face));

	BSP_ReadInt32(fd);

	tmp->nvecs=BSP_ReadInt16(fd);
	tmp->flags=BSP_ReadInt16(fd);

	V4F_COPY(node->norm, tmp->norm);
	if(tmp->flags&BSPFACE_FL_BACKSIDE)
		{ V4F_SCALE(node->norm, -1, tmp->norm); }

	tmp->vecs=gcatomic(tmp->nvecs*3*sizeof(float));
	for(i=0; i<tmp->nvecs; i++)
		BSP_ReadVector(fd, tmp->vecs+i*3);

	tmp->evecs=gcatomic(4*tmp->nvecs*sizeof(float));
	for(i=0; i<tmp->nvecs; i++)
	{
		j=(i+1)%tmp->nvecs;
		V3F_SUB(tmp->vecs+j*3, tmp->vecs+i*3, dv);
		V3F_CROSS(tmp->norm, dv, dn);
		V3F_NORMALIZE(dn, dn);
		dn[3]=V3F_DOT(tmp->vecs+i*3, dn);
		V4F_COPY(dn, tmp->evecs+i*4);
	}

	return(tmp);
}

BSP_Face *BSP_ReadNodeFace(VFILE *fd, BSP_Node *leaf, BSP_Model *mdl)
{
	float pt[3], dv[3], dn[4];
	BSP_Face *tmp;
	BSP_Node *ncur;
	BSP_LM *lcur;
	int i, j;

	tmp=gcalloc(sizeof(BSP_Face));
	memset(tmp, 0, sizeof(BSP_Face));

//	BSP_ReadFourcc(fd);
	BSP_ReadInt32(fd);

	BSP_ReadNormal(fd, tmp->norm);
	BSP_ReadNormal(fd, tmp->sdir);
	BSP_ReadNormal(fd, tmp->tdir);

	tmp->nvecs=BSP_ReadInt16(fd);
	tmp->num=BSP_ReadInt16(fd);
	tmp->lmnum=BSP_ReadInt16(fd);
	tmp->flags=BSP_ReadInt16(fd);

	j=BSP_ReadInt16(fd);
	for(i=lbxgl_bsp_nodestackpos-1; i>=0; i--)
	{
		ncur=lbxgl_bsp_nodestack[i];
		if(ncur->num==j)
		{
			tmp->nnext=ncur->face;
			ncur->face=tmp;

			tmp->node=ncur;
			tmp->leaf=leaf;
			break;
		}
	}

	if(tmp->lmnum!=0xFFFF)
	{
		lcur=mdl->lmroot;
		while(lcur)
		{
			if(lcur->num==tmp->lmnum)
			{
				tmp->lm=lcur;
				break;
			}
			lcur=lcur->next;
		}
	}

	BSP_ReadInt16(fd);
	BSP_ReadInt16(fd);
	BSP_ReadInt16(fd);

	tmp->vecs=gcatomic(tmp->nvecs*3*sizeof(float));
	tmp->st=gcatomic(tmp->nvecs*2*sizeof(float));
	tmp->uv=gcatomic(tmp->nvecs*2*sizeof(float));
	if(tmp->lm)tmp->vidx=gcatomic(tmp->nvecs*sizeof(int));
	for(i=0; i<tmp->nvecs; i++)
	{
		BSP_ReadVector(fd, tmp->vecs+i*3);
		BSP_ReadVector2D(fd, tmp->st+i*2);
//		BSP_ReadVector2D(fd, tmp->uv+i*2);

		j=BSP_ReadInt16(fd);
		if(tmp->lm)
		{
			tmp->vidx[i]=j;
			tmp->uv[i*2+0]=tmp->lm->vec_uv[j*2+0];
			tmp->uv[i*2+1]=tmp->lm->vec_uv[j*2+1];
		}
	}


	tmp->evecs=gcatomic(4*tmp->nvecs*sizeof(float));
	for(i=0; i<tmp->nvecs; i++)
	{
		j=(i+1)%tmp->nvecs;
		V3F_SUB(tmp->vecs+j*3, tmp->vecs+i*3, dv);
		V3F_CROSS(tmp->norm, dv, dn);
		V3F_NORMALIZE(dn, dn);
		dn[3]=V3F_DOT(tmp->vecs+i*3, dn);
		V4F_COPY(dn, tmp->evecs+i*4);
	}



	return(tmp);
}

BSP_Node *BSP_ReadLeaf(VFILE *fd, BSP_Model *mdl)
{
	BSP_Node *tmp;
	BSP_Face *ftmp, *ffst, *flst, *tfst, *tlst;
	int i, j, k;

	tmp=gcalloc(sizeof(BSP_Node));
	memset(tmp, 0, sizeof(BSP_Node));

//	BSP_ReadFourcc(fd, FCC_Leaf);

	ffst=NULL; flst=NULL;
	tfst=NULL; tlst=NULL;
	while(1)
	{
		i=BSP_ReadFourcc(fd);
		if(i==FCC_enD_)break;

		if(i==FCC_lEaf)
		{
			j=BSP_ReadInt32(fd);
			k=vftell(fd)+j;

			BSP_ReadVectorI(fd, tmp->mins);
			BSP_ReadVectorI(fd, tmp->maxs);
			tmp->flags=BSP_ReadInt16(fd);
			tmp->num=BSP_ReadInt16(fd);

			vfseek(fd, k, 0);
			continue;
		}

		if(i==FCC_vIs)
		{
			j=BSP_ReadInt32(fd);

			tmp->szvis=j;
			tmp->vis=gcatomic(j);
			vfread(tmp->vis, 1, j, fd);

			continue;
		}

		if(i==FCC_fAc1)
		{
			ftmp=BSP_ReadLeafFace(fd);
			if(tfst)
			{
				tlst->next=ftmp;
				tlst=ftmp;
			}else
			{
				tfst=ftmp;
				tlst=ftmp;
			}
			continue;
		}

		if(i==FCC_fAce)
		{
			ftmp=BSP_ReadNodeFace(fd, tmp, mdl);
			if(ffst)
			{
				flst->next=ftmp;
				flst=ftmp;
			}else
			{
				ffst=ftmp;
				flst=ftmp;
			}
			continue;
		}


		BSP_SkipTag(fd, i);
	}

	tmp->face=ffst;
	tmp->tsurf=tfst;

	V3F_SCALEADDSCALE(tmp->mins, 0.5, tmp->maxs, 0.5, tmp->org);
	tmp->radius=BSP_BoundNodeSphere(tmp, tmp->org);

	return(tmp);
}

BSP_Node *BSP_ReadNode(VFILE *fd, BSP_Model *mdl)
{
	BSP_Node *tmp, *ntmp;
	BSP_Face *ftmp, *ffst, *flst;
	BSP_Face *ptmp, *pfst, *plst;
	BSP_Face *ttmp, *tfst, *tlst;
	int i, j, k;

	tmp=gcalloc(sizeof(BSP_Node));
	memset(tmp, 0, sizeof(BSP_Node));

//	BSP_ReadFourcc(fd, FCC_Node);

	lbxgl_bsp_nodestack[lbxgl_bsp_nodestackpos++]=tmp;

	ffst=NULL; flst=NULL;
	pfst=NULL; plst=NULL;
	tfst=NULL; tlst=NULL;
	while(1)
	{
		i=BSP_ReadFourcc(fd);
		if(i==FCC_enD_)break;

		if(i==FCC_nOde)
		{
			j=BSP_ReadInt32(fd);
			k=vftell(fd)+j;

			BSP_ReadNormal(fd, tmp->norm);
			BSP_ReadVectorI(fd, tmp->mins);
			BSP_ReadVectorI(fd, tmp->maxs);
			tmp->flags=BSP_ReadInt16(fd);
			tmp->num=BSP_ReadInt16(fd);

			vfseek(fd, k, 0);
			continue;
		}

		if(i==FCC_fAce)
		{
			ftmp=BSP_ReadNodeFace(fd, NULL, mdl);
			if(ffst) { flst->next=ftmp; flst=ftmp; }
				else { ffst=ftmp; flst=ftmp; }
			continue;
		}

		if(i==FCC_pRt)
		{
			ptmp=BSP_ReadPortal(fd, tmp);
			if(pfst) { plst->next=ptmp; plst=ptmp; }
				else { pfst=ptmp; plst=ptmp; }
			continue;
		}

		if(i==FCC_tSrf)
		{
			ttmp=BSP_ReadTraceSurf(fd, tmp);
			if(tfst) { tlst->next=ttmp; tlst=ttmp; }
				else { tfst=ttmp; tlst=ttmp; }
			continue;
		}

		if(i==FCC_Node)
		{
			ntmp=BSP_ReadNode(fd, mdl);
			if(!tmp->lnode)
			{
				tmp->lnode=ntmp;
			}else if(!tmp->rnode)
			{
				tmp->rnode=ntmp;
			}
			continue;
		}

		if(i==FCC_Leaf)
		{
			ntmp=BSP_ReadLeaf(fd, mdl);
			if(!tmp->lnode)
			{
				tmp->lnode=ntmp;
			}else if(!tmp->rnode)
			{
				tmp->rnode=ntmp;
			}
			continue;
		}

		BSP_SkipTag(fd, i);
	}

//	tmp->face=ffst;
	tmp->prt=pfst;
	tmp->tsurf=tfst;

	V3F_SCALEADDSCALE(tmp->mins, 0.5, tmp->maxs, 0.5, tmp->org);
	tmp->radius=BSP_BoundNodeSphere(tmp, tmp->org);

	lbxgl_bsp_nodestackpos--;
	return(tmp);
}

BSP_LM *BSP_ReadLM(VFILE *fd)
{
	static byte *buf2=NULL;
	BSP_LM *tmp;
	byte *buf;
//	byte *buf, *buf2;
	int i, j, k, l;

	tmp=gcalloc(sizeof(BSP_LM));
	memset(tmp, 0, sizeof(BSP_LM));

//	BSP_ReadFourcc(fd);
	i=BSP_ReadInt32(fd)+vftell(fd);

	BSP_ReadNormal(fd, tmp->norm);

	tmp->xs=BSP_ReadInt16(fd);
	tmp->ys=BSP_ReadInt16(fd);
	tmp->fl=BSP_ReadInt16(fd);
	tmp->dn=BSP_ReadInt16(fd);

	tmp->dxs=tmp->xs>>2;
	tmp->dys=tmp->ys>>2;
	if(tmp->dxs<1)tmp->dxs=1;
	if(tmp->dys<1)tmp->dys=1;

	BSP_ReadVector2_14(fd, tmp->udir);
	BSP_ReadVector2_14(fd, tmp->vdir);

	tmp->min[0]=BSP_ReadFixed(fd, 65536);
	tmp->min[1]=BSP_ReadFixed(fd, 65536);
	tmp->max[0]=BSP_ReadFixed(fd, 65536);
	tmp->max[1]=BSP_ReadFixed(fd, 65536);

	tmp->num_vecs=BSP_ReadInt16(fd);
	tmp->vec_xyz=gcatomic(tmp->num_vecs*3*sizeof(float));
	tmp->vec_uv=gcatomic(tmp->num_vecs*2*sizeof(float));

	for(j=0; j<tmp->num_vecs; j++)
	{
		BSP_ReadVector(fd, tmp->vec_xyz+j*3);
		BSP_ReadVector2D(fd, tmp->vec_uv+j*2);
	}

//	vfseek(fd, i, 0);
//	return(tmp);


//	printf("Read LM %dx%d\n", tmp->xs, tmp->ys);

	if(tmp->fl&BSPLM_FL_MONO)
	{
		tmp->lmbuf=gcatomic((tmp->xs+1)*(tmp->ys+1));
		BSP_ReadImgMono(fd, tmp->lmbuf, tmp->xs, tmp->ys);
		tmp->texnum=Tex_LoadTextureMono(tmp->xs, tmp->ys, tmp->lmbuf);

		for(j=0; j<tmp->dn; j++)
		{
			tmp->dlmstyle[j]=vfgetc(fd);
			tmp->dlmnum[j]==-1;

			tmp->dlmbuf[j]=gcatomic((tmp->dxs+1)*(tmp->dys+1));
			BSP_ReadImgMono(fd, tmp->dlmbuf[j], tmp->dxs, tmp->dys);
			tmp->dlmnum[j]=Tex_LoadTextureMono(tmp->dxs, tmp->dys,
				tmp->dlmbuf[j]);
		}
	}else
	{
		tmp->lmbuf=gcatomic((tmp->xs+1)*(tmp->ys+1)*4);
		BSP_ReadImg(fd, tmp->lmbuf, tmp->xs, tmp->ys);
		tmp->texnum=Tex_LoadTexture(tmp->xs, tmp->ys, tmp->lmbuf, 1);

		for(j=0; j<tmp->dn; j++)
		{
			tmp->dlmstyle[j]=vfgetc(fd);
			tmp->dlmnum[j]==-1;

			tmp->dlmbuf[j]=gcatomic((tmp->dxs+1)*(tmp->dys+1)*4);
			BSP_ReadImg(fd, tmp->dlmbuf[j], tmp->dxs, tmp->dys);
			tmp->dlmnum[j]=Tex_LoadTexture(tmp->dxs, tmp->dys,
				tmp->dlmbuf[j], 1);
		}
	}

#if 0
	tmp->lmbuf=gcatomic(tmp->xs*tmp->ys*4);
	if(tmp->fl&1)BSP_ReadImgMono(fd, tmp->lmbuf, tmp->xs, tmp->ys);
		else BSP_ReadImg(fd, tmp->lmbuf, tmp->xs, tmp->ys);

	tmp->texnum=-1;

	buf=tmp->lmbuf;
//	buf2=gcatomic(tmp->xs*tmp->ys);
	if(!buf2)buf2=gcatomic(2048*2048);

#if 0
	l=0;
	for(j=0; j<(tmp->xs*tmp->ys); j++)
	{
		k=(buf[j*4+0]+buf[j*4+1]*2+buf[j*4+2])/4;
//		k=(buf[j*4+0]+buf[j*4+1]+buf[j*4+2])/3;
		buf2[j]=k;

		if(abs(buf[j*4+0]-k)>8)break;
		if(abs(buf[j*4+1]-k)>8)break;
		if(abs(buf[j*4+2]-k)>8)break;

//		if(abs(buf[j*4+0]-k)>8)l=1;
//		if(abs(buf[j*4+1]-k)>8)l=1;
//		if(abs(buf[j*4+2]-k)>8)l=1;
	}
#endif

	if(l)
//	if(1)
	{
		tmp->texnum=Tex_LoadTexture(tmp->xs, tmp->ys, tmp->lmbuf, 1);
	}else
	{
		tmp->texnum=Tex_LoadTextureMono(tmp->xs, tmp->ys, buf2);
	}
//	gcfree(buf2);

//	tmp->texnum=Tex_LoadTexture(tmp->xs, tmp->ys, tmp->lmbuf, 1);
//	tmp->texnum=Tex_LoadTextureMono(tmp->xs, tmp->ys, tmp->lmbuf);

	for(j=0; j<tmp->dn; j++)
	{
		tmp->dlmstyle[j]=vfgetc(fd);
		tmp->dlmnum[j]==-1;

		tmp->dlmbuf[j]=gcatomic(tmp->dxs*tmp->dys*4);
		if(tmp->fl&1)
			BSP_ReadImgMono(fd, tmp->dlmbuf[j], tmp->dxs, tmp->dys);
			else BSP_ReadImg(fd, tmp->dlmbuf[j], tmp->dxs, tmp->dys);
		tmp->dlmnum[j]=Tex_LoadTexture(tmp->dxs, tmp->dys,
			tmp->dlmbuf[j], 1);
	}
#endif


	vfseek(fd, i, 0);
	return(tmp);
}

BSP_Model *BSP_ReadModel(VFILE *fd)
{
	char buf[64], buf2[256];
	BSP_Model *tmp;
	BSP_LM *ltmp, *llst;
	float org[3];
	int i, j, k, l;

	tmp=gctalloc("bsp_model_t", sizeof(BSP_Model));
//	memset(tmp, 0, sizeof(BSP_Model));


//	BSP_ReadFourcc(fd, FCC_Mdl);
//	BSP_ReadFourcc(fd, FCC_hEad);
//	BSP_ReadInt32(fd, 18);

	l=0;
	llst=NULL;
	while(1)
	{
		i=BSP_ReadFourcc(fd);
		if(i==FCC_enD_)break;

		if(i==FCC_hEad)
		{
			j=BSP_ReadInt32(fd);
			k=vftell(fd)+j;

			BSP_ReadVectorI(fd, org);	//mins
			BSP_ReadVectorI(fd, org);	//maxs
			BSP_ReadVectorI(fd, org);	//org

			vfseek(fd, k, 0);
			continue;
		}

		if(i==FCC_tExi)
		{
			j=BSP_ReadInt32(fd);
			k=vftell(fd)+j;

			tmp->num_tex=j/64;

			tmp->texnames=gcalloc(tmp->num_tex*sizeof(char *));
			tmp->tex_xs=gcatomic(tmp->num_tex*sizeof(int));
			tmp->tex_ys=gcatomic(tmp->num_tex*sizeof(int));
			tmp->tex_fl=gcatomic(tmp->num_tex*sizeof(int));
			tmp->tex_num=gcatomic(tmp->num_tex*sizeof(int));
			tmp->tex_num2=gcatomic(tmp->num_tex*sizeof(int));

			for(j=0; j<tmp->num_tex; j++)
			{
				memset(buf, 0, 64);
				vfread(buf, 1, 56, fd);

				tmp->texnames[j]=kstrdup(buf);
				tmp->tex_xs[j]=BSP_ReadInt16(fd);
				tmp->tex_ys[j]=BSP_ReadInt16(fd);
				BSP_ReadInt16(fd);
				BSP_ReadInt16(fd);

				tmp->tex_num[j]=-1;
				tmp->tex_num2[j]=-1;

				sprintf(buf2, "textures/%s", buf);
				tmp->tex_num[j]=LBXGL_Texture_LoadImage(buf2);

				sprintf(buf2, "textures/%s_s", buf);
				tmp->tex_num2[j]=LBXGL_Texture_LoadImage(buf2);

				tmp->tex_fl[j]=
					LBXGL_Texture_GetImageFlags(
						tmp->tex_num[j]);
			}

			vfseek(fd, k, 0);
			continue;
		}

		if(i==FCC_lMi)
		{
			ltmp=BSP_ReadLM(fd);
			ltmp->num=l++;

			if(llst)
			{
				llst->next=ltmp;
				llst=ltmp;
			}else
			{
				tmp->lmroot=ltmp;
				llst=ltmp;
			}
			continue;
		}

		if(i==FCC_Node)
		{
			lbxgl_bsp_nodestackpos=0;
			tmp->root=BSP_ReadNode(fd, tmp);
			continue;
		}

		if(i==FCC_Leaf)
		{
			lbxgl_bsp_nodestackpos=0;
			tmp->root=BSP_ReadLeaf(fd, tmp);
			continue;
		}

		BSP_SkipTag(fd, i);
	}

	return(tmp);
}


LBXGL_Entity *BSP_ParseEntity(char **str)
{
	LBXGL_Entity *tmp;
	char lbuf[256];
	char *kbuf[64], *vbuf[64];
	char *cn;
	char **as, **as2, *s, *t;
	double *fa;
	void *p;
	int i, n;

	n=0;
	s=*str;
	while(*s)
	{
		t=lbuf;
		while(*s && (*s<=' '))s++;
		while(*s && *s!='\n')*t++=*s++;
		while(*s && (*s<=' '))s++;
		*t++=0;

		as=ksplit(lbuf);

		if(!as[0])continue;
		if(!strcmp(as[0], "//"))continue;
		if(!strcmp(as[0], "}"))break;

		t=as[0];
		if(!(((*t>='a') && (*t<='z')) || ((*t>='A') && (*t<='Z')) ||
			(*t=='_')))
			continue;

		if(!strcmp(as[0], "classname"))
			cn=kstrdup(as[1]);

		kbuf[n]=kstrdup(as[0]);
		vbuf[n]=kstrdup(as[1]);
		n++;
	}
	*str=s;

	tmp=LBXGL_Entity_NewClass(cn);
	if(!tmp)return(NULL);

	for(i=0; i<n; i++)
	{
		LBXGL_Entity_SetPropertyAsString(tmp, kbuf[i], vbuf[i]);
		gcfree(kbuf[i]);
		gcfree(vbuf[i]);
	}

	return(tmp);
}

#if 0
LBXGL_Entity *BSP_ParseEntity(char **str)
{
	LBXGL_Entity *tmp;
	char lbuf[256];
	char **as, **as2, *s, *t;
	double *fa;
	void *p;
	int i, n;

	tmp=LBXGL_Entity_New();

	s=*str;
	while(*s)
	{
		t=lbuf;
		while(*s && (*s<=' '))s++;
		while(*s && *s!='\n')*t++=*s++;
		while(*s && (*s<=' '))s++;
		*t++=0;

		as=ksplit(lbuf);

		if(!as[0])continue;
		if(!strcmp(as[0], "//"))continue;
		if(!strcmp(as[0], "}"))break;

		t=as[0];
		if(!(((*t>='a') && (*t<='z')) || ((*t>='A') && (*t<='Z')) ||
			(*t=='_')))
			continue;

		//property
		t=as[1];
		while(*t && (*t<=' '))t++;
		if(((*t>='0') && (*t<='9')) || (*t=='-'))
		{
			as2=ksplit(as[1]);
			for(n=0; as2[n]; n++);

			if(n>1)
			{
				fa=gctalloc("bggl2_numarray_t",
					n*sizeof(double));
				for(i=0; i<n; i++)
					fa[i]=atof(as2[i]);
				p=fa;
			}else p=NetVal_WrapFloat(atof(as2[0]));
		}else p=NetVal_WrapString(as[1]);

		LBXGL_Entity_SetProperty(tmp, as[0], p);
	}

	*str=s;
	return(tmp);
}
#endif

LBXGL_Entity *BSP_ReadEnts(VFILE *fd)
{
	char buf[256];
	char *tbuf;
	LBXGL_Entity *ents, *etmp, *elst;
	char *str, *s, *t, **a;
	int i, j;

//	BSP_ReadFourcc(fd, FCC_eNts);
	i=BSP_ReadInt32(fd);

	tbuf=gcatomic(i+1);
	memset(tbuf, 0, i+1);
	vfread(tbuf, 1, i, fd);


	ents=NULL; elst=NULL;
	s=tbuf;
	while(*s)
	{
		t=buf;
		while(*s && (*s<=' '))s++;
		while(*s && *s!='\n')*t++=*s++;
		while(*s && (*s<=' '))s++;
		*t++=0;

		a=ksplit(buf);
		if(!a[0])continue;

		if(!strcmp(a[0], "{"))
		{
			etmp=BSP_ParseEntity(&s);
			if(elst)
			{
				elst->next=etmp;
				elst=etmp;
			}else
			{
				ents=etmp;
				elst=etmp;
			}
		}
	}
	gcfree(tbuf);

	return(ents);

}

LBXGL_World *BSP_ReadBSP(char *name)
{
	VFILE *fd;
	LBXGL_World *tmp;
	LBXGL_Entity *cur, *ents;
	BSP_Model *mtmp, *mfst, *mlst, *mcur;
	LBXGL_Light *lfst, *llst, *ltmp;
	char *s, *t;
	int i, j;

	fd=vffopen(name, "rb");

	tmp=gctalloc("lbxgl_world_t", sizeof(LBXGL_World));

	i=BSP_ReadFourcc(fd);
	j=BSP_ReadFourcc(fd);
	if((i!=FCC_ItCF) || (j!=FCC_bsp))
	{
		kprint("BSP_ReadBSP: Invalid 'ItCF' or 'bsp ' magic\n");
		return(NULL);
	}

	mfst=NULL;
	mlst=NULL;
	while(1)
	{
		i=BSP_ReadFourcc(fd);
		if(i==FCC_enD_)break;

		if(i==FCC_Mdl)
		{
			mtmp=BSP_ReadModel(fd);
			if(mlst)
			{
				mlst->next=mtmp;
				mlst=mtmp;
			}else
			{
				mfst=mtmp;
				mlst=mtmp;
			}
			continue;
		}

		if(i==FCC_eNts)
		{
			ents=BSP_ReadEnts(fd);
			continue;
		}

		if(i==FCC_sStr)
		{
			j=BSP_ReadInt32(fd);
			tmp->styles[tmp->num_styles]=gcatomic(j+1);
			vfread(tmp->styles[tmp->num_styles], 1, j, fd);

			tmp->style_gamma[tmp->num_styles]=1.0;
			tmp->style_flags[tmp->num_styles]=1;

			tmp->num_styles++;
			continue;
		}

		BSP_SkipTag(fd, i);
	}

	lfst=NULL; llst=NULL;

	cur=ents;
	while(cur)
	{
		cur->world=tmp;

		s=LBXGL_Entity_GetProperty(cur, "model");
		if(s && (*s=='*'))
		{
			i=atoi(s+1);
			mcur=mfst;
			while(mcur && i--)mcur=mcur->next;
			LBXGL_Entity_SetProperty(cur, "mdl", mcur);
			cur->bmdl=mcur;
//			mcur->ent=cur;
		}

		s=LBXGL_Entity_GetProperty(cur, "classname");
		if(s && !strncmp(s, "light", 5))
		{
			ltmp=gcalloc(sizeof(LBXGL_Light));
			LBXGL_Entity_GetPropertyFVector(cur, "origin",
				ltmp->org, 3);
			LBXGL_Entity_GetPropertyFVector(cur, "_color",
				ltmp->clr, 3);
			ltmp->val[0]=LBXGL_Entity_GetPropertyFloat(
				cur, "light");
			ltmp->val[1]=LBXGL_Entity_GetPropertyFloat(
				cur, "hilight");
			ltmp->flags=LBXGL_Entity_GetPropertyFloat(
				cur, "spawnflags");

			ltmp->style=-1;
			t=LBXGL_Entity_GetProperty(cur, "style");
			if(t)
			{
				for(i=0; i<tmp->num_styles; i++)
					if(!strcmp(tmp->styles[i], t))
						break;
				if(i<tmp->num_styles)ltmp->style=i;
			}

			if(Vec3F_Length(ltmp->clr)<0.1)
			{
				ltmp->clr[0]=1;
				ltmp->clr[1]=1;
				ltmp->clr[2]=1;
			}
			Vec3F_Normalize(ltmp->clr, ltmp->clr);

			if(llst)
			{
				llst->next=ltmp;
				llst=ltmp;
			}else
			{
				lfst=ltmp;
				llst=ltmp;
			}
		}

		cur=cur->next;
	}

	tmp->bsp=mfst->root;
	tmp->mdls=mfst;
	tmp->ents=ents;
	tmp->lights=lfst;

	tmp->id=-1;

	return(tmp);
}
