#include <lbxgl2.h>

static char *map_name;
static int map_seq=0;

static char **split_str(char *s)
{
	char **a, *t;
	int i;

	a=kralloc(128*sizeof(char *));
	memset(a, 0, 128*sizeof(char *));
	i=0;
	while(*s)
	{
		while(*s && (*s<=' '))s++;
		if(!*s)break;
		t=kralloc(256);
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

LBXGL_MapEdge *LBXGL_MAP_InterpolateEdge(LBXGL_MapEdge *edge, float *norm)
{
	LBXGL_MapEdge *tmp;
	float dir[3], stdir[2], uvdir[2], x;

	tmp=kalloc(sizeof(LBXGL_MapEdge));

	//calc dir vectors
	VecNF_Subtract(edge->link->point, edge->point, dir, 3);
	VecNF_Subtract(edge->link->st, edge->st, stdir, 2);
	VecNF_Subtract(edge->link->uv, edge->uv, uvdir, 2);

	x=VecNF_DotProduct(dir, norm, 3);
	x=(x<0)?-x:x;
	VecNF_Scale(dir, 1/x, dir, 3);
	VecNF_Scale(stdir, 1/x, stdir, 2);
	VecNF_Scale(uvdir, 1/x, uvdir, 2);

	//calc intersections
	x=VecNF_DotProduct(edge->point, norm, 3)-norm[3];
	x=(x<0)?-x:x;

	VecNF_ScaleAdd(dir, x, edge->point, tmp->point, 3);
	VecNF_ScaleAdd(stdir, x, edge->st, tmp->st, 2);
	VecNF_ScaleAdd(uvdir, x, edge->uv, tmp->uv, 2);

	return(tmp);
}

LBXGL_MapFace *LBXGL_MAP_CutPolys(LBXGL_MapFace *poly, float *norm)
{
	LBXGL_MapFace *next;
	LBXGL_MapEdge *cur, *cur2, *tmp;

	if(!poly)return(NULL);
	next=poly->next;

	cur=poly->link;
	while(1)
	{
		if(VecNF_DotProduct(cur->point, norm, 3)>=norm[3])
			break;
		cur=cur->link;

		if(cur==poly->link)
		{
			//entire poly got cut away
			next=LBXGL_MAP_CutPolys(next, norm);
			return(next);
		}
	}

	poly->link=cur;	//this point is known good

	//find a point where an intersection occures
	while(1)
	{
		if(VecNF_DotProduct(cur->link->point, norm, 3)<norm[3])
			break;
		cur=cur->link;

		if(cur==poly->link)
		{
			//entire poly is ok
			next=LBXGL_MAP_CutPolys(next, norm);
			poly->next=next;
			return(poly);
		}
	}

	//find the next intersection
	cur2=cur->link;
	while(1)
	{
		if(VecNF_DotProduct(cur2->link->point, norm, 3)>=norm[3])
			break;
		cur2=cur2->link;
	}

	tmp=LBXGL_MAP_InterpolateEdge(cur, norm);
	tmp->link=LBXGL_MAP_InterpolateEdge(cur2, norm);

	tmp->link->link=cur2->link;
	cur->link=tmp;

	next=LBXGL_MAP_CutPolys(next, norm);
	poly->next=next;
	return(poly);
}

LBXGL_MapFace *LBXGL_MAP_BuildPolys(float *v0, float *v1, float *v2,
	char **tex, float *vs, float *vt, float *stmat, int num, int ty)
{
	LBXGL_MapFace *fst, *lst, *tmp;
	LBXGL_MapEdge *pt0, *pt1, *pt2, *pt3;
	float tv0[3], tv1[3], tv2[3];
	float norm[4];
	float sx, sy, f;
	int i, j, k;

	fst=NULL;
	lst=NULL;
	for(i=0; i<num; i++)
	{
		tmp=kalloc(sizeof(LBXGL_MapFace));
		tmp->tex=tex[i];

		pt0=kalloc(sizeof(LBXGL_MapEdge));
		pt1=kalloc(sizeof(LBXGL_MapEdge));
		pt2=kalloc(sizeof(LBXGL_MapEdge));
		pt3=kalloc(sizeof(LBXGL_MapEdge));
		pt0->link=pt1;
		pt1->link=pt2;
		pt2->link=pt3;
		pt3->link=pt0;

		tmp->link=pt0;
//		pt0->tex=tex[i];

		if(lst)
		{
			lst->next=tmp;
			lst=tmp;
		}else
		{
			fst=tmp;
			lst=tmp;
		}

		VecNF_Subtract(&v1[i*3], &v0[i*3], tv0, 3);
		VecNF_Subtract(&v2[i*3], &v0[i*3], tv1, 3);
		sx=VecNF_Normalize(tv0, tv0, 3)/128;
		sy=VecNF_Normalize(tv1, tv1, 3)/128;

//		Vec3F_CrossProduct(tv0, tv1, norm);
		Vec3F_CrossProduct(tv1, tv0, norm);
		norm[3]=VecNF_DotProduct(&v0[i*3], norm, 3);

		VecNF_Copy(norm, tmp->norm, 4);
		VecNF_Copy(vs+(i*4), tmp->sdir, 4);
		VecNF_Copy(vt+(i*4), tmp->tdir, 4);

		k=-1;
		f=-1;
		for(j=0; j<3; j++)
			if(fabs(norm[j])>f)
		{
			k=j;
			f=fabs(norm[j]);
		}

		VecNF_ScaleAdd(tv0, -MAP_INF, &v0[i*3], tv2, 3);
		VecNF_ScaleAdd(tv1, -MAP_INF, tv2, pt0->point, 3);

		VecNF_ScaleAdd(tv0, MAP_INF, &v0[i*3], tv2, 3);
		VecNF_ScaleAdd(tv1, -MAP_INF, tv2, pt1->point, 3);

		VecNF_ScaleAdd(tv0, MAP_INF, &v0[i*3], tv2, 3);
		VecNF_ScaleAdd(tv1, MAP_INF, tv2, pt2->point, 3);

		VecNF_ScaleAdd(tv0, -MAP_INF, &v0[i*3], tv2, 3);
		VecNF_ScaleAdd(tv1, MAP_INF, tv2, pt3->point, 3);

		if(ty==0)
		{
			VecNF_Zero(tv0, 3);
			VecNF_Zero(tv1, 3);
			switch(k)
			{
			case 0:
				tv0[1]=1;
				tv1[2]=1;
				break;
			case 1:
				tv0[0]=1;
				tv1[2]=1;
				break;
			case 2:
				tv0[0]=1;
				tv1[1]=1;
				break;
			default:
				break;
			}

			pt0->st[0]=VecNF_DotProduct(pt0->point, tv0, 3);
			pt0->st[1]=VecNF_DotProduct(pt0->point, tv1, 3);

			pt1->st[0]=VecNF_DotProduct(pt1->point, tv0, 3);
			pt1->st[1]=VecNF_DotProduct(pt1->point, tv1, 3);

			pt2->st[0]=VecNF_DotProduct(pt2->point, tv0, 3);
			pt2->st[1]=VecNF_DotProduct(pt2->point, tv1, 3);

			pt3->st[0]=VecNF_DotProduct(pt3->point, tv0, 3);
			pt3->st[1]=VecNF_DotProduct(pt3->point, tv1, 3);


			tv2[0]=stmat[i*6+0]*pt0->st[0] +
				stmat[i*6+1]*pt0->st[1] + stmat[i*6+2];
			tv2[1]=stmat[i*6+3]*pt0->st[0] +
				stmat[i*6+4]*pt0->st[1] + stmat[i*6+5];
			VecNF_Copy(tv2, pt0->st, 2);

			tv2[0]=stmat[i*6+0]*pt1->st[0] +
				stmat[i*6+1]*pt1->st[1] + stmat[i*6+2];
			tv2[1]=stmat[i*6+3]*pt1->st[0] +
				stmat[i*6+4]*pt1->st[1] + stmat[i*6+5];
			VecNF_Copy(tv2, pt1->st, 2);

			tv2[0]=stmat[i*6+0]*pt2->st[0] +
				stmat[i*6+1]*pt2->st[1] + stmat[i*6+2];
			tv2[1]=stmat[i*6+3]*pt2->st[0] +
				stmat[i*6+4]*pt2->st[1] + stmat[i*6+5];
			VecNF_Copy(tv2, pt2->st, 2);

			tv2[0]=stmat[i*6+0]*pt3->st[0] +
				stmat[i*6+1]*pt3->st[1] + stmat[i*6+2];
			tv2[1]=stmat[i*6+3]*pt3->st[0] +
				stmat[i*6+4]*pt3->st[1] + stmat[i*6+5];
			VecNF_Copy(tv2, pt3->st, 2);
		}

		if(ty==1)
		{
			pt0->st[0]=-MAP_INF*sx;
			pt0->st[1]=-MAP_INF*sy;
			pt1->st[0]=MAP_INF*sx;
			pt1->st[1]=-MAP_INF*sy;
			pt2->st[0]=MAP_INF*sx;
			pt2->st[1]=MAP_INF*sy;
			pt3->st[0]=-MAP_INF*sx;
			pt3->st[1]=MAP_INF*sy;
		}

		if(ty==2)
		{
			pt0->st[0]=VecNF_DotProduct(pt0->point,
				stmat+(i*12)+0, 3)+stmat[i*12+3];
			pt0->st[1]=VecNF_DotProduct(pt0->point,
				stmat+(i*12)+4, 3)+stmat[i*12+7];

			pt1->st[0]=VecNF_DotProduct(pt1->point,
				stmat+(i*12)+0, 3)+stmat[i*12+3];
			pt1->st[1]=VecNF_DotProduct(pt1->point,
				stmat+(i*12)+4, 3)+stmat[i*12+7];

			pt2->st[0]=VecNF_DotProduct(pt2->point,
				stmat+(i*12)+0, 3)+stmat[i*12+3];
			pt2->st[1]=VecNF_DotProduct(pt2->point,
				stmat+(i*12)+4, 3)+stmat[i*12+7];

			pt3->st[0]=VecNF_DotProduct(pt3->point,
				stmat+(i*12)+0, 3)+stmat[i*12+3];
			pt3->st[1]=VecNF_DotProduct(pt3->point,
				stmat+(i*12)+4, 3)+stmat[i*12+7];
		}
	}

	return(fst);
}

LBXGL_PolyPrim *LBXGL_MAP_BuildMesh(LBXGL_MapFace *face)
{
	LBXGL_PolyPrim *tmp;
	LBXGL_Poly *poly;
	LBXGL_MapFace *fcur;
	LBXGL_MapEdge *cur2;
	char *s;
	int xs, ys;

//	kprint("BuildMesh:\n");

	int i, tx;
	tmp=LBXGL_CreatePolyPrim();
	fcur=face;
	while(fcur)
	{
		cur2=fcur->link;
		for(i=0; !i || cur2!=fcur->link; i++)
			cur2=cur2->link;

		poly=LBXGL_CreatePoly(i);

		s=ksprint("textures/%s", fcur->tex);
		tx=LBXGL_Texture_LoadImage(s);
		if(tx<0)tx=LBXGL_Texture_LoadImage("textures/basic/checker.tga");
		if(tx>=0)
		{
			LBXGL_Texture_GetImageSize(tx, &xs, &ys);

			poly->texnum=tx;
			poly->flags|=LBXGL_FL_HASTEX;
		}else
		{
			xs=128;
			ys=128;
		}

		s=ksprint("textures/%s_s", fcur->tex);
		tx=LBXGL_Texture_LoadImage(s);
		if(tx>=0)
		{
			poly->gtexnum=tx;
			poly->flags|=LBXGL_FL_HASGM;
		}

//		poly
//		kprint("\tVecs(%d):\n", i);

		cur2=fcur->link;
		for(i=0; !i || cur2!=fcur->link; i++)
		{
			poly->vecs[i*LBXGL_VECW+0]=cur2->point[0]*MAP_SCALE;
			poly->vecs[i*LBXGL_VECW+1]=cur2->point[1]*MAP_SCALE;
			poly->vecs[i*LBXGL_VECW+2]=cur2->point[2]*MAP_SCALE;
			poly->vecs[i*LBXGL_VECW+3]=cur2->st[0]*(1.0/xs);
			poly->vecs[i*LBXGL_VECW+4]=cur2->st[1]*(1.0/ys);

//			kprint("\t\tVec(%d): (%.4f %.4f %.4f) (%.4f %.4f)\n", i,
//				cur2->point[0], cur2->point[1], cur2->point[2],
//				cur2->st[0], cur2->st[1]);

			cur2=cur2->link;
		}

		LBXGL_PolyPrim_AddPoly(tmp, poly);

		fcur=fcur->next;
	}

//	kprint("BuildMesh: done %p\n", tmp);

	return(tmp);
}

LBXGL_MapFace *LBXGL_MAP_LoadBrushFaces(VFILE *fd, int *mty)
{
	char lbuf[512];
	char **as, **as2, *s;

	float tv0[3], tv1[3], tv2[3];
	float ang, cx, sx;
	float f, scs, sct;

	static float v0[16*3], v1[16*3], v2[16*3];	//3 points
	static float vs[16*4], vt[16*4];		//st vectors
	static float norm[16*4];

	static char *tex[16];
	static float storg[2], strot, stscale[2];
	static float stmat[16*12];

	int i, j, n, pn;

	LBXGL_MapFace *face;

//	kprint("LoadBrush:\n");

	i=0;
	while(!vfeof(fd))
	{
		vfgets(lbuf, 511, fd);
		as=split_str(lbuf);

		if(!as[0])continue;
		if(!strcmp(as[0], "//"))continue;
		if(!strcmp(as[0], "}"))break;

		if(strcmp(as[0], "("))continue;
		v0[i*3+0]=atof(as[1]);
		v0[i*3+1]=atof(as[2]);
		v0[i*3+2]=atof(as[3]);
		v1[i*3+0]=atof(as[6]);
		v1[i*3+1]=atof(as[7]);
		v1[i*3+2]=atof(as[8]);
		v2[i*3+0]=atof(as[11]);
		v2[i*3+1]=atof(as[12]);
		v2[i*3+2]=atof(as[13]);

		tex[i]=kstrdup(as[15]);

		VecNF_Subtract(&v1[i*3], &v0[i*3], tv0, 3);
		VecNF_Subtract(&v2[i*3], &v0[i*3], tv1, 3);
		VecNF_Normalize(tv0, tv0, 3);
		VecNF_Normalize(tv1, tv1, 3);

//		Vec3F_CrossProduct(tv0, tv1, &norm[i*4]);
		Vec3F_CrossProduct(tv1, tv0, &norm[i*4]);
		VecNF_Normalize(&norm[i*4], &norm[i*4], 3);

		norm[i*4+3]=VecNF_DotProduct(&v0[i*3], &norm[i*4], 3);

		f=-1;
		pn=-1;
		for(j=0; j<3; j++)
			if(fabs(norm[i*4+j])>f)
		{
			f=fabs(norm[i*4+j]);
			pn=j;
		}
		

		if(!strcmp(as[16], "["))
		{
			if(*mty!=2)kprint("LBXGL_MAP_LoadBrush: "
				"Detect Valve 220\n");
			*mty=2;

			vs[i*4+0]=atof(as[17]);
			vs[i*4+1]=atof(as[18]);
			vs[i*4+2]=atof(as[19]);
			vs[i*4+3]=atof(as[20]);

			vt[i*4+0]=atof(as[23]);
			vt[i*4+1]=atof(as[24]);
			vt[i*4+2]=atof(as[25]);
			vt[i*4+3]=atof(as[26]);


			tv0[0]=atof(as[17]);
			tv0[1]=atof(as[18]);
			tv0[2]=atof(as[19]);
			storg[0]=atof(as[20]);

			tv1[0]=atof(as[23]);
			tv1[1]=atof(as[24]);
			tv1[2]=atof(as[25]);
			storg[1]=atof(as[26]);

			strot=atof(as[28]);
			stscale[0]=atof(as[29]);
			stscale[1]=atof(as[30]);

			Vec3F_CrossProduct(tv0, tv1, tv2);
//			Vec3F_CrossProduct(tv1, tv0, tv2);

			stmat[i*12+0]=tv0[0]*stscale[0];
			stmat[i*12+1]=tv0[1]*stscale[0];
			stmat[i*12+2]=tv0[2]*stscale[0];
			stmat[i*12+3]=storg[0];

			stmat[i*12+4]=tv1[0]*stscale[1];
			stmat[i*12+5]=tv1[1]*stscale[1];
			stmat[i*12+6]=tv1[2]*stscale[1];
			stmat[i*12+7]=storg[1];

			stmat[i*12+8]=tv2[0];
			stmat[i*12+9]=tv2[1];
			stmat[i*12+10]=tv2[2];
			stmat[i*12+11]=0;
		}else
		{
			j=0;
			if(as[24] && !strcmp(as[24], "//TX1"))j=1;
			if(as[24] && !strcmp(as[24], "//TX2"))j=-1;

			if(j && *mty!=1)
			{
				kprint("LBXGL_MAP_LoadBrush: "
					"Detect Quark ETP\n");
				*mty=1;
			}

			storg[0]=atof(as[16]);
			storg[1]=atof(as[17]);
			strot=atof(as[18]);
			stscale[0]=atof(as[19]);
			stscale[1]=atof(as[20]);

			ang=strot*(M_PI/180);
			sx=sin(ang);
			cx=cos(ang);

			if(*mty==1)
			{
				VecNF_Subtract(&v1[i*3], &v0[i*3], tv0, 3);
				VecNF_Subtract(&v2[i*3], &v0[i*3], tv1, 3);
				scs=VecNF_Normalize(tv0, tv0, 3);
				sct=VecNF_Normalize(tv1, tv1, 3);

				VecNF_Scale(tv0, scs/128.0, vs+(i*4), 3);
				VecNF_Scale(tv1, sct/128.0, vt+(i*4), 3);

				vs[i*4+3]=VecNF_DotProduct(&v0[i*3], tv0, 3);
				vt[i*4+3]=VecNF_DotProduct(&v0[i*3], tv1, 3);
			}else
			{
				LBXGL_BspUtil_ClosestAxisNormal(
					norm+(i*4), tv0, tv1, tv2);

				VecNF_ScaleAddScale(tv0, cx, tv1, sx,
					vs+(i*4), 3);
				VecNF_ScaleAddScale(tv0, -sx, tv1, cx,
					vt+(i*4), 3);

				vs[i*4+3]=storg[0];
				vt[i*4+3]=storg[1];
			}

			stmat[i*6+0]=stscale[0]*cx;
			stmat[i*6+1]=stscale[0]*sx;
			stmat[i*6+2]=storg[0];

			stmat[i*6+3]=-stscale[1]*sx;
			stmat[i*6+4]=stscale[1]*cx;
			stmat[i*6+5]=storg[1];
		}

		i++;
	}
	n=i;

//	kprint("LoadBrush: %d faces\n", n);

	face=LBXGL_MAP_BuildPolys(v0, v1, v2, tex, vs, vt, stmat, n, *mty);

	for(i=0; i<n; i++)
	{
		tv0[0]=-norm[i*4+0]; tv0[1]=-norm[i*4+1];
		tv0[2]=-norm[i*4+2]; tv0[3]=-norm[i*4+3];
		face=LBXGL_MAP_CutPolys(face, tv0);

//		face=LBXGL_MAP_CutPolys(face, &norm[i*4]);
	}

//	kprint("LoadBrush: done %p\n", tmp);

	return(face);
}

LBXGL_Entity *LBXGL_MAP_LoadDummy(VFILE *fd)
{
	char lbuf[256];
	char **as, **as2, *s;

	kprint("LoadDummy:\n");

	while(!vfeof(fd))
	{
		vfgets(lbuf, 255, fd);
		as=split_str(lbuf);

		if(!as[0])continue;
		if(!strcmp(as[0], "//"))continue;

		if(!strcmp(as[0], "{"))
		{
			LBXGL_MAP_LoadDummy(fd);
			continue;
		}
		if(!strcmp(as[0], "}"))break;
	}

	kprint("LoadDummy: done\n");
	return(0);
}

LBXGL_Entity *LBXGL_MAP_LoadEntity(VFILE *fd, int *mty)
{
//	LBXGL_PolyPrim *fst, *lst, *prim;

	LBXGL_MapFace *face, *ffst, *flst;
	LBXGL_MapBrush *btmp, *bfst, *blst;

	LBXGL_MapFace *orgface;
	LBXGL_MapBrush *orgbrush;

	LBXGL_BSP_Model *bmdl;
	LBXGL_PolyGroup *grp;
	LBXGL_Entity *tmp;
	char lbuf[256];
	float mins[3], maxs[3], org[3], ang[3];
	char **as, **as2, *s, *cname, *mtyn;
	double *fa;
	void *p;
	int i, n;

//	kprint("LoadEntity:\n");

	tmp=LBXGL_Entity_New();

	cname=NULL;

//	fst=NULL;
//	lst=NULL;

	orgface=NULL;
	orgbrush=NULL;

	bfst=NULL;
	blst=NULL;
	ffst=NULL;
	flst=NULL;
	while(!vfeof(fd))
	{
		vfgets(lbuf, 255, fd);
		as=split_str(lbuf);

		if(!as[0])continue;
		if(!strcmp(as[0], "//"))continue;

		if(!strcmp(as[0], "{"))
		{
//			prim=LBXGL_MAP_LoadBrush(fd, mty);
			face=LBXGL_MAP_LoadBrushFaces(fd, mty);

			btmp=LBXGL_MAP_MakeBrush(face);
			if(blst)
			{
				blst->next=btmp;
				blst=btmp;
			}else
			{
				bfst=btmp;
				blst=btmp;
			}

			if(btmp->flags&LBXGL_BRUSH_FL_ORIGIN)
			{
				orgface=face;
				orgbrush=btmp;

				continue;
			}
			if(btmp->flags&LBXGL_BRUSH_FL_NONSOLID)
				continue;

			if(flst)
			{
				flst->next=face;
				while(flst->next)flst=flst->next;
			}else
			{
				ffst=face;
				flst=face;
				while(flst->next)flst=flst->next;
			}

#if 0
			prim=LBXGL_MAP_BuildMesh(face);
			LBXGL_CalcPolyPrimNormals(prim);
			LBXGL_BoundPolyPrim(prim, NULL, NULL);

//			prim=NULL;
//			LBXGL_MAP_LoadDummy(fd);

			if(lst)
			{
				lst->next=prim;
				lst=prim;
			}else
			{
				fst=prim;
				lst=prim;
			}
#endif

			continue;
		}
		if(!strcmp(as[0], "}"))break;

		s=as[0];
		if(!(((*s>='a') && (*s<='z')) || ((*s>='A') && (*s<='Z'))))
			continue;

		//property
		s=as[1];
		while(*s && (*s<=' '))s++;
		if(((*s>='0') && (*s<='9')) || (*s=='-'))
		{
			as2=split_str(as[1]);
			for(n=0; as2[n]; n++);

			if(n>1)
			{
				fa=ObjType_New("bggl2_numarray_t",
					n*sizeof(double));
				for(i=0; i<n; i++)
					fa[i]=atof(as2[i]);
				p=fa;
			}else p=NetVal_WrapFloat(atof(as2[0]));
		}else p=NetVal_WrapString(as[1]);

		if(!strcmp(as[0], "origin") ||
			!strcmp(as[0], "mins") ||
			!strcmp(as[0], "maxs"))
		{
			VecND_Scale(fa, MAP_SCALE, fa, 3);
		}

		if(!strcmp(as[0], "classname"))
			cname=p;

		LBXGL_Entity_SetProperty(tmp, as[0], p);
	}

#if 0
	if(fst)
	{
		grp=LBXGL_CreatePolyGroup();
		grp->prim=fst;

		LBXGL_Entity_SetProperty(tmp, "mdl", grp);

		LBXGL_InitBBox(mins, maxs);
		LBXGL_BoundPolyGroup(grp, mins, maxs);
		LBXGL_Entity_SetPropertyFVector(tmp, "absmin", mins, 3);
		LBXGL_Entity_SetPropertyFVector(tmp, "absmax", maxs, 3);
	}
#endif

#if 1
	if(ffst)
	{
		mtyn=LBXGL_Entity_GetPropertyString(tmp, "modeltype");
		if(!mtyn)mtyn="bsp";

		if(orgbrush)
		{
			
			VecNF_ScaleAddScale(orgbrush->mins, -0.5,
				orgbrush->maxs, -0.5, org, 3);
			LBXGL_BspCache_TranslateFaces(ffst, org);

			VecNF_Scale(org, -MAP_SCALE, org, 3);
			LBXGL_Entity_SetPropertyFVector(tmp, "origin", org, 3);
		}

		LBXGL_InitBBox(mins, maxs);
		LBXGL_BspCache_BoundFaces(ffst, mins, maxs);

		VecNF_Scale(mins, MAP_SCALE, mins, 3);
		VecNF_Scale(maxs, MAP_SCALE, maxs, 3);

		VecNF_ScaleAddScale(mins, 0.5, maxs, 0.5, org, 3);
		VecNF_Zero(ang, 3);

		LBXGL_Entity_SetPropertyFVector(tmp, "absmin", mins, 3);
		LBXGL_Entity_SetPropertyFVector(tmp, "absmax", maxs, 3);
		LBXGL_Entity_SetPropertyFVector(tmp, "absorg", org, 3);

#if 0
		p=LBXGL_Entity_GetProperty(tmp, "solid_origin");
		if(!p)LBXGL_Entity_SetPropertyFVector(tmp,
			"solid_origin", org, 3);

		p=LBXGL_Entity_GetProperty(tmp, "solid_angles");
		if(!p)LBXGL_Entity_SetPropertyFVector(tmp,
			"solid_angles", ang, 3);
#endif

		if(!strcmp(mtyn, "bsp"))
		{
			bmdl=LBXGL_BspCache_BuildModel(ffst, bfst);
			LBXGL_Entity_SetProperty(tmp, "mdl", bmdl);
		}

		if(!strcmp(mtyn, "brushsolid"))
		{
			LBXGL_Entity_SetProperty(tmp, "mdl", bfst);
		}

//		if(!cname)cname="NULL";
//		s=ksprint("%s:#%d", map_name, map_seq++);
//		bmdl->name=kstrdup(s);
	}
#endif

//	kprint("LoadEntity: done %p\n", tmp);

	return(tmp);
}

extern VFILE *lbxgl_light_infofd;

int LBXGL_MAP_LightMapWorld(LBXGL_Entity *ents, char *base, int crc)
{
	VFILE *fd;
	LBXGL_Light *lights, *cur;
	LBXGL_PolyGroup *world;
	LBXGL_Entity *ecur;
	void *p;
	char *s;
	int ocrc;
	char buf[16];
	int i;

	kprint("LightMapWorld:\n");

#if 0
	s=ksprint("%s_lm.lmi", base);
	fd=vffopen(s, "r+b");
	if(fd)
	{
		vfread(buf, 1, 8, fd);
		if(!strcmp(buf, "LMINFO0"))
			vfread(&ocrc, 1, 4, fd);
			else ocrc=-1;
	}else
	{
		fd=vffopen(s, "w+b");
		ocrc=-1;
	}
#endif


	lights=NULL;
	ecur=ents;
	while(ecur)
	{
		s=LBXGL_Entity_GetPropertyString(ecur, "classname");
		if(!strcmp(s, "light"))
			lights=LBXGL_Light_NewLightEnt(lights, ecur);
		ecur=ecur->next;
	}
	LBXGL_Entity_SetProperty(ents, "lights", lights);


	s=ksprint("%s_lights.txt", base);
	fd=vffopen(s, "wt");
	cur=lights;
	while(cur)
	{
		kprintvf(fd, "( %g %g %g ) [ %g %g %g ] %g %g  %d\n",
			cur->org[0], cur->org[1], cur->org[2],
			cur->diffuse[0], cur->diffuse[1], cur->diffuse[2],
			cur->val, cur->hival, cur->flags);
		cur=cur->next;
	}
	vfclose(fd);


	if(1)
	{
		ecur=ents;
		while(ecur)
		{
			p=LBXGL_Entity_GetProperty(ecur, "mdl");
			s=ObjType_GetTypeName(p);
			if(!strcmp(s, "lbxgl_bspmodel_t"))
				LBXGL_BspLMap_LightModel(p, lights);
			ecur=ecur->next;
		}
	}

#if 0
	if(crc!=ocrc)
	{
		vfseek(fd, 0, 0);
		vfwrite("LMINFO0", 1, 8, fd);
		vfwrite(&crc, 1, 4, fd);

		kprint("LightMapWorld: crc changed %X -> %X\n", ocrc, crc);

		if(!lights)
		{
			vfputc(0, fd);
			return(0);
		}
		vfputc(1, fd);
		vfputc(0, fd);
		vfputc(0, fd);
		vfputc(0, fd);

		lbxgl_light_infofd=fd;

		kprint("LightMapWorld: lights\n");

		ecur=ents;
		while(ecur)
		{
			s=LBXGL_Entity_GetPropertyString(ecur, "classname");
			if(!strcmp(s, "worldspawn"))
				world=LBXGL_Entity_GetProperty(ecur, "mdl");
			ecur=ecur->next;
		}

		kprint("LightMapWorld: world\n");

		ecur=ents;
		while(ecur)
		{
			p=LBXGL_Entity_GetProperty(ecur, "mdl");
			s=ObjType_GetTypeName(p);
			if(!strcmp(s, "lbxgl_polygroup_t"))
				LBXGL_Light_LightMapGroup(world, p, lights);
			ecur=ecur->next;
		}
		kprint("LightMapWorld: done\n");

		vfclose(fd);
	}else
	{
		i=vfgetc(fd);
		if(i!=1)return(0);

		vfgetc(fd);
		vfgetc(fd);
		vfgetc(fd);

		kprint("LightMapWorld: load\n");

		ecur=ents;
		while(ecur)
		{
			p=LBXGL_Entity_GetProperty(ecur, "mdl");
			s=ObjType_GetTypeName(p);
			if(!strcmp(s, "lbxgl_polygroup_t"))
			{
				i=LBXGL_Light_LoadLightMapGroup(fd, p);
				if(i<0)kprint("LightMapWorld: "
					"decode fail near %X\n", vftell(fd));
			}
			ecur=ecur->next;
		}
		vfclose(fd);
	}
#endif

	return(0);
}

LBXGL_Entity *LBXGL_MAP_LoadMap(char *name)
{
	LBXGL_Entity *fst, *lst, *tmp;
	LBXGL_Entity *cur, *plst, *pent;

	VFILE *fd;
	char lbuf[256];
	char *s, *t;
	char **as;
	int crc, mty;
	int i, nn;

	kprint("LoadMap: %s\n", name);

	fd=vffopen(name, "rb");
	s=vf_bufferin(fd);
	vfclose(fd);

	crc=ObjType_StrAdler32(s);
	kfree(s);

	kprint("LoadMap: checksum %X\n", crc);

	strcpy(lbuf, name);
	s=lbuf+strlen(lbuf);
	while((s>lbuf) && (*s!='.'))s--;
	if(*s=='.')*s++=0;

	fst=LBXGL_BspCache_LoadWorld(lbuf, crc);
	if(fst)return(fst);

	map_name=name;
//	map_seq=1;
	map_seq=rand()^(rand()<<12);

	fst=NULL;
	lst=NULL;
	fd=vffopen(name, "rt");

	mty=0;
	nn=0;

	while(!vfeof(fd))
	{
		vfgets(lbuf, 255, fd);
		as=split_str(lbuf);

		if(!as[0])continue;
		if(!strcmp(as[0], "//"))
		{
/*
			for(i=1; as[i]; i++)
			{
				if(!strncmp(as[i], "TXQBSP", 6))
				{
					if(mty!=1)kprint(
						"LBXGL_MAP_LoadMap: "
						"Detect Valve 220\n");
					mty=1;
				}
//				if(!strncmp(as[i], "QuArk", 5))
//					mty=1;
			}
*/
			continue;
		}

		if(!strcmp(as[0], "{"))
		{
			tmp=LBXGL_MAP_LoadEntity(fd, &mty);

			//name unnamed entities
			s=LBXGL_Entity_GetPropertyString(tmp, "name");
			if(!s)
			{
				s=LBXGL_Entity_GetPropertyString(tmp,
					"classname");
				t=ksprint("%s__AN%d", s, nn++);
				LBXGL_Entity_SetPropertyString(tmp,
					"name", t);
			}
			tmp->name=s;

			//link into hierarchy
			//default: worldspawn as parent
			pent=fst;

			s=LBXGL_Entity_GetPropertyString(tmp, "parentname");
			if(s)
			{
				cur=fst;
				while(cur)
				{
					t=LBXGL_Entity_GetPropertyString(
						cur, "name");
					if(!strcmp(s, t))break;
					cur=cur->next;
				}
				pent=cur;
			}

			if(pent)
			{
				if(pent->rdown)
				{
					cur=pent->rdown;
					while(cur->rnext)cur=cur->rnext;
					cur->rnext=tmp;

					tmp->rup=pent;
				}else
				{
					tmp->rup=pent;
					pent->rdown=tmp;
				}
			}

			//link to end of flat list
			if(lst)
			{
				lst->next=tmp;
				lst=tmp;
			}else
			{
				fst=tmp;
				lst=tmp;
			}
		}
	}

	vfclose(fd);

	strcpy(lbuf, name);
	s=lbuf+strlen(lbuf);
	while((s>lbuf) && (*s!='.'))s--;
	if(*s=='.')*s++=0;

	LBXGL_MAP_LightMapWorld(fst, lbuf, crc);

	LBXGL_BspCache_SaveWorld(fst, lbuf, crc);

	kprint("LoadMap: done %p\n", fst);

	return(fst);
}


int LBXGL_MAP_FlattenEnt(LBXGL_Entity *ent, VFILE *fd)
{
	LBXGL_EntProperty *cur, *last;
	double fv[16];
	double *fa;
	double f;
	void *p;
	char *ty;
	int i, n, sz;

	kprintvf(fd, "{\n");

	last=NULL;
	cur=ent->attr;
	while(cur)
	{
		p=cur->value;
		ty=ObjType_GetTypeName(p);

		if(!strcmp(ty, "string_t"))
		{
			kprintvf(fd, "\"%s\" \"%s\"\n", cur->name, p);
		}

		if(!strcmp(ty, "float_t"))
		{
			f=NetVal_UnwrapFloat(p);
			if(f==(int)f)
				kprintvf(fd, "\"%s\" \"%d\"\n",
					cur->name, (int)f);
				else kprintvf(fd, "\"%s\" \"%lf\"\n",
					cur->name, f);
		}

		if(!strcmp(ty, "bggl2_numarray_t"))
		{
			sz=ObjType_GetSize(p);

			fa=p;
			n=sz/sizeof(double);
			VecND_Copy(fa, fv, n);

			if(!strcmp(cur->name, "origin") ||
				!strcmp(cur->name, "mins") ||
				!strcmp(cur->name, "maxs") ||
				!strcmp(cur->name, "absmin") ||
				!strcmp(cur->name, "absmax"))
			{
				VecND_Scale(fa, MAP_ISCALE, fv, 3);
			}


			kprintvf(fd, "\"%s\" \"", cur->name);

			for(i=0; i<n; i++)
			{
				if(fv[i]==(int)fv[i])
					kprintvf(fd, "%d", (int)fv[i]);
					else kprintvf(fd, "%lf", fv[i]);
				if((i+1)<n)vfputc(' ', fd);
			}

			kprintvf(fd, "\"\n");
		}

		cur=cur->next;
	}
	kprintvf(fd, "}\n");

	return(0);
}

int LBXGL_MAP_FlattenEnts(LBXGL_Entity *ents, VFILE *fd)
{
	LBXGL_Entity *cur;

	cur=ents;
	while(cur)
	{
		LBXGL_MAP_FlattenEnt(cur, fd);
		cur=cur->next;
	}
	return(0);
}

LBXGL_Entity *LBXGL_MAP_UnFlattenEnt(VFILE *fd)
{
	LBXGL_BSP_Model *bmdl;
	LBXGL_Entity *tmp;
	char lbuf[256];
	float mins[3], maxs[3];
	char **as, **as2, *s;
	double *fa;
	void *p;
	int i, n;

//	kprint("LoadEntity:\n");

	while(!vfeof(fd))
	{
		vfgets(lbuf, 255, fd);
		as=split_str(lbuf);

		if(!as[0])continue;
		if(!strncmp(as[0], "//", 2))continue;
		if(!strcmp(as[0], "{"))break;

		kprint("LBXGL_MAP_UnFlattenEnt: Parse Error\n");
		return(NULL);
	}

	if(vfeof(fd))return(NULL);

	tmp=LBXGL_Entity_New();

	while(!vfeof(fd))
	{
		vfgets(lbuf, 255, fd);
		as=split_str(lbuf);

		if(!as[0])continue;
		if(!strncmp(as[0], "//", 2))continue;

		if(!strcmp(as[0], "}"))break;

		s=as[0];
		if(!(((*s>='a') && (*s<='z')) || ((*s>='A') && (*s<='Z'))))
			continue;

		//property
		s=as[1];
		while(*s && (*s<=' '))s++;
		if(((*s>='0') && (*s<='9')) || (*s=='-'))
		{
			as2=split_str(as[1]);
			for(n=0; as2[n]; n++);

			if(n>1)
			{
				fa=ObjType_New("bggl2_numarray_t",
					n*sizeof(double));
				for(i=0; i<n; i++)
					fa[i]=atof(as2[i]);
				p=fa;
			}else p=NetVal_WrapFloat(atof(as2[0]));
		}else p=NetVal_WrapString(as[1]);

		if(!strcmp(as[0], "origin") ||
			!strcmp(as[0], "mins") ||
			!strcmp(as[0], "maxs") ||
			!strcmp(as[0], "absmin") ||
			!strcmp(as[0], "absmax"))
		{
			VecND_Scale(fa, MAP_SCALE, fa, 3);
		}

		LBXGL_Entity_SetProperty(tmp, as[0], p);
	}

//	kprint("LoadEntity: done %p\n", tmp);

	return(tmp);
}

LBXGL_Entity *LBXGL_MAP_UnFlattenEnts(VFILE *fd)
{
	LBXGL_Entity *cur, *tmp, *fst, *lst, *pent;
	LBXGL_Light *lights;
	char *s, *t;
	void *p;
	int nn;

	nn=0;

	fst=NULL;
	lst=NULL;
	while(!vfeof(fd))
	{
		tmp=LBXGL_MAP_UnFlattenEnt(fd);
		if(!tmp)continue;

		s=LBXGL_Entity_GetPropertyString(tmp, "classname");
		kprint("read ent %s\n", s);

		s=LBXGL_Entity_GetPropertyString(tmp, "model");
		if(s)
		{
			p=LBXGL_BspCache_LookupModel(s);
			if(p)LBXGL_Entity_SetProperty(tmp, "mdl", p);
		}


		//name unnamed entities
		s=LBXGL_Entity_GetPropertyString(tmp, "name");
		if(!s)
		{
			s=LBXGL_Entity_GetPropertyString(tmp,
				"classname");
			t=ksprint("%s__UAN%d", s, nn++);
			LBXGL_Entity_SetPropertyString(tmp,
				"name", t);
		}

		//link into hierarchy
		//default: worldspawn as parent
		pent=fst;

		s=LBXGL_Entity_GetPropertyString(tmp, "parentname");
		if(s)
		{
			cur=fst;
			while(cur)
			{
				t=LBXGL_Entity_GetPropertyString(
					cur, "name");
				if(!strcmp(s, t))break;
				cur=cur->next;
			}
			pent=cur;
		}

		if(pent)
		{
			if(pent->rdown)
			{
				cur=pent->rdown;
				while(cur->rnext)cur=cur->rnext;
				cur->rnext=tmp;

				tmp->rup=pent;
			}else
			{
				tmp->rup=pent;
				pent->rdown=tmp;
			}
		}

		if(lst)lst->next=tmp;
			else fst=tmp;
		lst=tmp;
	}

	lights=NULL;
	cur=fst;
	while(cur)
	{
		s=LBXGL_Entity_GetPropertyString(cur, "classname");
		if(!strcmp(s, "light"))
			lights=LBXGL_Light_NewLightEnt(lights, cur);
		cur=cur->next;
	}
	LBXGL_Entity_SetProperty(fst, "lights", lights);

	return(fst);
}


//different style, hierarchical nesting

int LBXGL_MAP_FlattenEnt2(LBXGL_Entity *ent, VFILE *fd, int ind)
{
	LBXGL_Entity *ecur;
	LBXGL_EntProperty *cur, *last;
	double fv[16];
	double *fa;
	double f;
	void *p;
	char *ty;
	int i, n, sz;

	for(i=0; i<ind; i++)kprintvf(fd, "\t");
	if(ent->name)kprintvf(fd, "\"%s\"\n", ent->name);
	kprintvf(fd, "{\n");

	last=NULL;
	cur=ent->attr;
	while(cur)
	{
		if(!strcmp(cur->name, "name") ||
			!strcmp(cur->name, "parentname"))
		{
			cur=cur->next;
			continue;
		}

		p=cur->value;
		ty=ObjType_GetTypeName(p);

		if(!strcmp(ty, "string_t"))
		{
			for(i=0; i<(ind+1); i++)kprintvf(fd, "\t");
			kprintvf(fd, "\"%s\" \"%s\"\n", cur->name, p);
		}

		if(!strcmp(ty, "float_t"))
		{
			for(i=0; i<(ind+1); i++)kprintvf(fd, "\t");

			f=NetVal_UnwrapFloat(p);
			if(f==(int)f)
				kprintvf(fd, "\"%s\" \"%d\"\n",
					cur->name, (int)f);
				else kprintvf(fd, "\"%s\" \"%lf\"\n",
					cur->name, f);
		}

		if(!strcmp(ty, "bggl2_numarray_t"))
		{
			for(i=0; i<(ind+1); i++)kprintvf(fd, "\t");

			sz=ObjType_GetSize(p);

			fa=p;
			n=sz/sizeof(double);
			VecND_Copy(fa, fv, n);

			if(!strcmp(cur->name, "origin") ||
				!strcmp(cur->name, "mins") ||
				!strcmp(cur->name, "maxs") ||
				!strcmp(cur->name, "absmin") ||
				!strcmp(cur->name, "absmax"))
			{
				VecND_Scale(fa, MAP_ISCALE, fv, 3);
			}


			kprintvf(fd, "\"%s\" \"", cur->name);

			for(i=0; i<n; i++)
			{
				if(fv[i]==(int)fv[i])
					kprintvf(fd, "%d", (int)fv[i]);
					else kprintvf(fd, "%lf", fv[i]);
				if((i+1)<n)vfputc(' ', fd);
			}

			kprintvf(fd, "\"\n");
		}

		cur=cur->next;
	}

	ecur=ent->rdown;
	while(ecur)
	{
		LBXGL_MAP_FlattenEnt2(ecur, fd, ind+1);
		ecur=ecur->next;
	}

	for(i=0; i<ind; i++)kprintvf(fd, "\t");
	kprintvf(fd, "}\n");

	return(0);
}

int LBXGL_MAP_FlattenEnts2(LBXGL_Entity *ents, VFILE *fd)
{
	LBXGL_Entity *cur;

	cur=ents;
	while(cur)
	{
		LBXGL_MAP_FlattenEnt2(cur, fd, 0);
		cur=cur->rnext;
	}
	return(0);
}
