#include <lbxgl2.h>

int bsp_tex_on;
int bsp_num_nodes=0;
int bsp_num_leaves=0;
int bsp_num_glow=0;

int BSP_FacePlaneSide(BSP_Face *face, float *norm)
{
	float d;
	int i, j;

	i=0;
	for(j=0; j<face->nvecs; j++)
	{
		d=V3F_NDOT(face->vecs+j*3, norm);
		if(d<=0)i|=1;
		if(d>0)i|=2;
	}

	return(i);
}

int BSP_FacePlaneSide2(BSP_Face *face, float *norm)
{
	float d;
	int i, j;

	i=0;
	for(j=0; j<face->nvecs; j++)
	{
		d=V3F_NDOT(face->vecs+j*3, norm);
		if(d<0)i|=1;
		if(d>0)i|=2;
	}
	return(i);
}

void BSP_InitBBox(float *mins, float *maxs)
{
	mins[0]=9999; mins[1]=9999; mins[2]=9999;
	maxs[0]=-9999; maxs[1]=-9999; maxs[2]=-9999;
}

void BSP_BoundFace(BSP_Face *face,
	float *mins, float *maxs)
{
	float *v;
	int i;

	for(i=0; i<face->nvecs; i++)
	{
		v=face->vecs+i*3;
		if(v[0]<mins[0])mins[0]=v[0];
		if(v[0]>maxs[0])maxs[0]=v[0];
		if(v[1]<mins[1])mins[1]=v[1];
		if(v[1]>maxs[1])maxs[1]=v[1];
		if(v[2]<mins[2])mins[2]=v[2];
		if(v[2]>maxs[2])maxs[2]=v[2];
	}
}

void BSP_BoundFaces(BSP_Face *faces,
	float *mins, float *maxs)
{
	BSP_Face *cur;

	cur=faces;
	while(cur)
	{
		BSP_BoundFace(cur, mins, maxs);
		cur=cur->next;
	}
}

float BSP_BoundFaceSphere(BSP_Face *face, float *org)
{
	float *v;
	float r, d;
	int i;

	r=0;
	for(i=0; i<face->nvecs; i++)
	{
		v=face->vecs+i*3;
		d=V3F_DIST(v, org);
		if(d>r)r=d;
	}
	return(r);
}

float BSP_BoundFacesSphere(BSP_Face *faces, float *org)
{
	BSP_Face *cur;
	float r, d;

	r=0;
	cur=faces;
	while(cur)
	{
		d=BSP_BoundFaceSphere(cur, org);
		if(d>r)r=d;
		cur=cur->next;
	}
	return(r);
}

#define BSP_SUBDIVIDE	512

BSP_Face *BSP_SubdivideFaces(BSP_Face *faces)
{
	float mins[3], maxs[3], nv[4];
	BSP_Face *cur, *nxt, *fst, *tmp;
	float f;
	int i;

	while(1)
	{
		i=0;
		cur=faces;
		fst=NULL;
		while(cur)
		{
			nxt=cur->next;

			mins[0]=999999;
			mins[1]=999999;
			mins[2]=999999;

			maxs[0]=-999999;
			maxs[1]=-999999;
			maxs[2]=-999999;

			BSP_BoundFace(cur, mins, maxs);

			if((maxs[0]-mins[0])>BSP_SUBDIVIDE)
			{
				f=mins[0]+(maxs[0]-mins[0])*0.5;
				V4F_SET(nv, 1, 0, 0, f);

				tmp=BSP_FrontPoly(cur, nv);
				tmp->next=fst;
				fst=tmp;

				tmp=BSP_BackPoly(cur, nv);
				tmp->next=fst;
				fst=tmp;

				i=1;
				cur=nxt;
				continue;
			}

			if((maxs[1]-mins[1])>BSP_SUBDIVIDE)
			{
				f=mins[1]+(maxs[1]-mins[1])*0.5;
				V4F_SET(nv, 0, 1, 0, f);

				tmp=BSP_FrontPoly(cur, nv);
				tmp->next=fst;
				fst=tmp;

				tmp=BSP_BackPoly(cur, nv);
				tmp->next=fst;
				fst=tmp;

				i=1;
				cur=nxt;
				continue;
			}

			if((maxs[2]-mins[2])>BSP_SUBDIVIDE)
			{
				f=mins[2]+(maxs[2]-mins[2])*0.5;
				V4F_SET(nv, 0, 0, 1, f);

				tmp=BSP_FrontPoly(cur, nv);
				tmp->next=fst;
				fst=tmp;

				tmp=BSP_BackPoly(cur, nv);
				tmp->next=fst;
				fst=tmp;

				i=1;
				cur=nxt;
				continue;
			}

			cur->next=fst;
			fst=cur;

			cur=nxt;
		}
		faces=fst;

		if(!i)break;
	}

	return(faces);
}

BSP_Face *BSP_ChoosePlaneSlow(BSP_Face *faces)
{
	BSP_Face *cur1, *cur2, *best;
	float nw, bw;
	int nl, nr, ns, nd, nf, i;
	int bd, bs;

	best=NULL;

	cur1=faces;
	while(cur1)
	{
		if(cur1->flags&BSPFACE_FL_ONNODE)
		{
			cur1=cur1->next;
			continue;
		}

		nl=0;
		nr=0;
		ns=0;
		nf=0;

		cur2=faces;
		while(cur2)
		{
			if(cur2==cur1)
			{
				cur2=cur2->next;
				continue;
			}

			i=BSP_FacePlaneSide2(cur2, cur1->norm);
			if(i==0)nf++;
			if(i==1)nl++;
			if(i==2)nr++;
			if(i==3)
			{
				nl++;
				nr++;
				ns++;
			}
			cur2=cur2->next;
		}

		nd=(nl<nr)?(nr-nl):(nl-nr);
		nw=nd*0.5+(ns*0.25)+nf;

		if(!best || (nw<bw))
		{
			best=cur1;
			bd=nd;
			bs=ns;
			bw=nw;

			cur1=cur1->next;
			continue;
		}

		cur1=cur1->next;
	}

	return(best);
}

BSP_Face *BSP_ChoosePlaneFast(BSP_Face *faces)
{
	float org[3], dir[4];
	BSP_Face *cur, *best;
	float f, g, bv;
	int i, j, k;

	V3F_ZERO(org); cur=faces; j=0;
	while(cur)
	{
		if(cur->flags&BSPFACE_FL_ONNODE)
		{
			cur=cur->next;
			continue;
		}

		for(i=0; i<cur->nvecs; i++)
			{ V3F_ADD(org, cur->vecs+i*3, org); j++; }
		cur=cur->next;
	}

	V3F_SCALE(org, 1.0/j, org);

	V3F_ZERO(dir); cur=faces;
	while(cur)
	{
		if(cur->flags&BSPFACE_FL_ONNODE)
		{
			cur=cur->next;
			continue;
		}

		for(i=0; i<cur->nvecs; i++)
		{
			dir[0]+=fabs(cur->vecs[i*3+0]-org[0]);
			dir[1]+=fabs(cur->vecs[i*3+1]-org[1]);
			dir[2]+=fabs(cur->vecs[i*3+2]-org[2]);
		}
		cur=cur->next;
	}

	V3F_NORMALIZE(dir, dir);
	dir[4]=V3F_DOT(org, dir);

	best=NULL; bv=1000000000;

	cur=faces;
	while(cur)
	{
		if(cur->flags&BSPFACE_FL_ONNODE)
		{
			cur=cur->next;
			continue;
		}

		f=fabs(V3F_DOT(cur->norm, dir));
		g=fabs(dir[3]*f-cur->norm[3]*f);		

		f=f*f;
		if(isnan(f) || (f<0.00000001))f=0.00000001;
		f=(1.0+g)/f;

		if(!best || (f<bv))
		{
			best=cur;
			bv=f;
		}

		cur=cur->next;
	}
	return(best);
}

BSP_Face *BSP_ChoosePlane(BSP_Face *faces)
{
	return(BSP_ChoosePlaneFast(faces));
}

BSP_Face *BSP_DupPoly(BSP_Face *face)
{
	BSP_Face *tmp;
	int i;

	tmp=kalloc(sizeof(BSP_Face));
	memset(tmp, 0, sizeof(BSP_Face));

	tmp->node=face->node;

	V4F_COPY(face->norm, tmp->norm);
	V4F_COPY(face->sdir, tmp->sdir);
	V4F_COPY(face->tdir, tmp->tdir);
	tmp->tex=face->tex;
	tmp->num=face->num;
	tmp->flags=face->flags;

	tmp->lleaf=face->lleaf;
	tmp->rleaf=face->rleaf;
	tmp->llnext=face->llnext;
	tmp->rlnext=face->rlnext;

	tmp->nvecs=face->nvecs;
	tmp->vecs=kalloc(3*tmp->nvecs*sizeof(float));
	tmp->st=kalloc(2*tmp->nvecs*sizeof(float));

	for(i=0; i<tmp->nvecs; i++)
	{
		V3F_COPY(face->vecs+i*3, tmp->vecs+i*3);
		V2F_COPY(face->st+i*2, tmp->st+i*2);
	}

	return(tmp);
}

void BSP_FreePoly(BSP_Face *face)
{
	if(face->vecs)kfree(face->vecs);
	if(face->evecs)kfree(face->evecs);
	if(face->st)kfree(face->st);	
	kfree(face);
}

BSP_Face *BSP_FrontPoly(BSP_Face *face,
	float *norm)
{
	static float pts[16*3], stv[16*2];
	BSP_Face *tmp;
	float rn[4];
	int i, j, k, l;

	tmp=kalloc(sizeof(BSP_Face));
	memset(tmp, 0, sizeof(BSP_Face));

	tmp->node=face->node;

	V4F_COPY(face->norm, tmp->norm);
	V4F_COPY(face->sdir, tmp->sdir);
	V4F_COPY(face->tdir, tmp->tdir);
	tmp->tex=face->tex;
	tmp->num=face->num;
	tmp->flags=face->flags;

	tmp->lleaf=face->lleaf;
	tmp->rleaf=face->rleaf;
	tmp->llnext=face->llnext;
	tmp->rlnext=face->rlnext;

	V4F_SCALE(norm, -1, rn);
	if(face->st)
	{
		l=Hull_ClipFaceST(rn, face->vecs, face->st,
			pts, stv, face->nvecs);
	}else
	{
		l=Hull_ClipFace(rn, face->vecs, pts, face->nvecs);
	}

	tmp->nvecs=l;
	tmp->vecs=kalloc(3*l*sizeof(float));

	if(face->st)tmp->st=kalloc(2*l*sizeof(float));

	for(j=0; j<l; j++)
	{
		V3F_COPY(pts+j*3, tmp->vecs+j*3);
		if(face->st)V2F_COPY(stv+j*2, tmp->st+j*2);
	}

	return(tmp);
}

BSP_Face *BSP_BackPoly(BSP_Face *face,
	float *norm)
{
	float rnorm[4];

	rnorm[0]=-norm[0];
	rnorm[1]=-norm[1];
	rnorm[2]=-norm[2];
	rnorm[3]=-norm[3];

	return(BSP_FrontPoly(face, rnorm));
}

BSP_Node *BSP_BuildLeaf(BSP_Face *faces)
{
	BSP_Node *tmp;
	BSP_Face *cur, *fst, *lst, *ftmp;
	int i, j;

	fst=NULL; lst=NULL;
	cur=faces;
	while(cur)
	{
		ftmp=BSP_DupPoly(cur);
		if(fst) {lst->next=ftmp; lst=ftmp; }
			else {fst=ftmp; lst=ftmp; }
		cur=cur->next;
	}

	faces=BSP_SubdivideFaces(faces);

	tmp=kalloc(sizeof(BSP_Node));
	memset(tmp, 0, sizeof(BSP_Node));
	tmp->face=faces;
	tmp->tsurf=fst;

	BSP_InitBBox(tmp->mins, tmp->maxs);
	BSP_BoundFaces(faces, tmp->mins, tmp->maxs);

	V3F_SCALEADDSCALE(tmp->mins, 0.5, tmp->maxs, 0.5, tmp->org);
	tmp->radius=BSP_BoundFacesSphere(faces, tmp->org);

	tmp->num=bsp_num_leaves;
	bsp_num_leaves++;

	return(tmp);
}

#if 1
BSP_Node *BSP_BuildNode(BSP_Face *faces, float *norm)
{
	BSP_Node *tmp, *ntmp;
	BSP_Face *cur, *nxt, *ftmp;
	BSP_Face *lfst, *rfst, *mfst;
	int i, j, lf, rf, mf;
	float f, g;

	lf=0;
	rf=0;
	mf=0;

	tmp=kalloc(sizeof(BSP_Node));
	memset(tmp, 0, sizeof(BSP_Node));
	tmp->num=bsp_num_nodes;
	bsp_num_nodes++;

	lfst=NULL;
	rfst=NULL;
	mfst=NULL;
	cur=faces;
	while(cur)
	{
		nxt=cur->next;

		i=BSP_FacePlaneSide2(cur, norm);
		f=V3F_DOT(cur->norm, norm);

		switch(i)
		{
		case 0:
			if(fabs(f)<0.999)break;
			j=f>0;

#if 0
#if 1
			ftmp=BSP_DupPoly(cur);
			ftmp->flags|=BSPFACE_FL_ONNODE;
			ftmp->flags|=BSPFACE_FL_BACKSIDE;
			if(!ftmp->node)ftmp->node=tmp;
			ftmp->next=rfst;
			rfst=ftmp;
			rf++;

#endif

			ftmp=BSP_DupPoly(cur);
			ftmp->flags|=BSPFACE_FL_ONNODE;
			if(!ftmp->node)ftmp->node=tmp;
			ftmp->next=lfst;
			lfst=ftmp;
			lf++;
#endif

			cur->flags|=BSPFACE_FL_PORTAL;
			if(f<0)cur->flags|=BSPFACE_FL_BACKSIDE;
				else cur->flags&=~BSPFACE_FL_BACKSIDE;
			cur->next=mfst;
			mfst=cur;
			mf++;

			break;
		case 1:
			cur->next=lfst;
			lfst=cur;
			lf++;
			break;
		case 2:
			cur->next=rfst;
			rfst=cur;
			rf++;
			break;
		case 3:
			ftmp=BSP_FrontPoly(cur, norm);
//			ftmp=BSP_BackPoly(cur, norm);
			ftmp->next=rfst;
			rfst=ftmp;

			ftmp=BSP_BackPoly(cur, norm);
//			ftmp=BSP_FrontPoly(cur, norm);
			ftmp->next=lfst;
			lfst=ftmp;

//			BSP_FreePoly(cur);

			lf++;
			rf++;
			break;
		default:
			break;
		}

		cur=nxt;
	}


//	mfst=BSP_SubdivideFaces(mfst);
//	tmp->face=mfst;

#if 0
	cur=mfst;
	while(cur)
	{
		BSP_BuildEvecs(cur);
		cur=cur->next;
	}
#endif

	tmp->tsurf=mfst;

	V4F_COPY(norm, tmp->norm);

	BSP_InitBBox(tmp->mins, tmp->maxs);
	BSP_BoundFaces(mfst, tmp->mins, tmp->maxs);

	ftmp=BSP_ChoosePlane(lfst);
	if(ftmp)
	{
		ntmp=BSP_BuildNode(lfst, ftmp->norm);
		tmp->lnode=ntmp;
//		ntmp->up=tmp;
	}else
	{
		ntmp=BSP_BuildLeaf(lfst);
		tmp->lnode=ntmp;
//		ntmp->up=tmp;
	}

	ftmp=BSP_ChoosePlane(rfst);
	if(ftmp)
	{
		ntmp=BSP_BuildNode(rfst, ftmp->norm);
		tmp->rnode=ntmp;
//		ntmp->up=tmp;
	}else
	{
		ntmp=BSP_BuildLeaf(rfst);
		tmp->rnode=ntmp;
//		ntmp->up=tmp;
	}

	for(i=0; i<3; i++)
	{
		if(tmp->lnode->mins[i]<tmp->mins[i])
			tmp->mins[i]=tmp->lnode->mins[i];
		if(tmp->rnode->mins[i]<tmp->mins[i])
			tmp->mins[i]=tmp->rnode->mins[i];
		if(tmp->lnode->maxs[i]>tmp->maxs[i])
			tmp->maxs[i]=tmp->lnode->maxs[i];
		if(tmp->rnode->maxs[i]>tmp->maxs[i])
			tmp->maxs[i]=tmp->rnode->maxs[i];
	}

	return(tmp);
}
#endif

int BSP_ProcessTextures(BSP_Model *mdl,
	BSP_Face *faces)
{
	BSP_Face *cur;
	char *s;
	int i, j, xs, ys, fl, tn;

	printf("BSP_ProcessTextures\n");

	mdl->texnames=kalloc(256*sizeof(char *));
	mdl->tex_xs=kalloc(256*sizeof(int));
	mdl->tex_ys=kalloc(256*sizeof(int));
	mdl->tex_fl=kalloc(256*sizeof(int));
	mdl->tex_num=kalloc(256*sizeof(int));
	mdl->num_tex=0;

	cur=faces;
	while(cur)
	{
		for(i=0; i<mdl->num_tex; i++)
			if(!strcmp(mdl->texnames[i], cur->tex))
				break;
		if(i==mdl->num_tex)
		{
			mdl->texnames[i]=strdup(cur->tex);
			mdl->tex_xs[i]=128;
			mdl->tex_ys[i]=128;
			mdl->tex_fl[i]=0;

//			buf=BSP_GetTexture(cur->tex, &xs, &ys, &fl);
			tn=LBXGL_Texture_LoadImageSize(cur->tex, &xs, &ys);
			if(tn>=0)
			{
				fl=LBXGL_Texture_GetImageFlags(tn);

				mdl->tex_xs[i]=xs;
				mdl->tex_ys[i]=ys;
				mdl->tex_fl[i]=fl;
				mdl->tex_num[i]=tn;
			}

			mdl->num_tex++;
		}
		cur->num=i;

		for(j=0; j<cur->nvecs; j++)
		{
			cur->st[j*2+0]=V3F_NDOT(cur->vecs+j*3, cur->sdir)/
				mdl->tex_xs[i];
			cur->st[j*2+1]=V3F_NDOT(cur->vecs+j*3, cur->tdir)/
				mdl->tex_ys[i];
		}

		cur=cur->next;
	}
	printf("BSP_ProcessTextures: OK, %d Tex\n", mdl->num_tex);

	return(0);
}


//destrictively flatten brushes
BSP_Face *BSP_FlattenBrushes(BSP_Brush *brush)
{
	BSP_Brush *bcur;
	BSP_Face *fst, *lst, *fcur;

	printf("BSP_FlattenBrushes\n");

	fst=NULL;
	lst=NULL;
	bcur=brush;
	while(bcur)
	{
//		printf("BSP_FlattenBrushes: brush %p\n", bcur);

		if(fst)
		{
			lst->next=bcur->face;
			while(lst->next)lst=lst->next;
		}else
		{
			fst=bcur->face;
			lst=fst;
			while(lst && lst->next)lst=lst->next;
		}

		bcur=bcur->next;
	}

	printf("BSP_FlattenBrushes: OK\n");

	return(fst);
}

BSP_Model *BSP_BuildModel(BSP_Brush *brush)
{
	BSP_Model *tmp;
	BSP_Brush *blst, *bcur;
	BSP_Face *faces;

	BSP_Node *ntmp;
	BSP_Face *cur, *nxt, *ftmp;
	int i;

	printf("BSP_BuildModel\n");

	bsp_tex_on=0;

	blst=BSP_ClipBrushes(brush);
	faces=BSP_FlattenBrushes(blst);

	tmp=kalloc(sizeof(BSP_Model));
	memset(tmp, 0, sizeof(BSP_Model));

	BSP_ProcessTextures(tmp, faces);
	tmp->brush=brush;

	bsp_tex_on=1;
	bsp_num_nodes=0;
	bsp_num_leaves=0;
	bsp_num_glow=0;

#if 0
	//collect any glowing faces
	nxt=NULL;
	cur=faces;
	while(cur)
	{
		if(cur->flags&BSPFACE_FL_GLOW)
		{
			ftmp=BSP_DupPoly(cur);
			BSP_BuildEvecs(ftmp);

			ftmp->next=nxt;
			nxt=ftmp;

			bsp_num_glow++;
		}
		cur=cur->next;
	}
	tmp->glow=nxt;
	tmp->num_glow=bsp_num_glow;
#endif

	ftmp=BSP_ChoosePlane(faces);
	if(ftmp)
	{
		printf("BSP_BuildModel: Node\n");
		ntmp=BSP_BuildNode(faces, ftmp->norm);
		tmp->root=ntmp;
	}else
	{
		printf("BSP_BuildModel: Leaf\n");
		ntmp=BSP_BuildLeaf(faces);
		tmp->root=ntmp;
	}

	printf("BSP_BuildModel: %d nodes %d leaves, %d glowsurfs\n",
		bsp_num_nodes, bsp_num_leaves, bsp_num_glow);

//	tmp->num_nodes=bsp_num_nodes;
//	tmp->num_leaves=bsp_num_leaves;

	printf("BSP_BuildModel: OK\n");

	return(tmp);
}

