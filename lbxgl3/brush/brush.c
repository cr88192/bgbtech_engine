#include <lbxgl2.h>

extern LBXGL_BrushWorld *lbxgl_world;

void LBXGL_Hull_BBoxSphere(float *org, float rad,
	float *mins, float *maxs)
{
	mins[0]=org[0]-rad;
	mins[1]=org[1]-rad;
	mins[2]=org[2]-rad;

	maxs[0]=org[0]+rad;
	maxs[1]=org[1]+rad;
	maxs[2]=org[2]+rad;
}

int LBXGL_Hull_CheckSphereBBox(float *org, float rad,
	float *mins, float *maxs)
{
	float mins2[3], maxs2[3];
	
	LBXGL_Hull_BBoxSphere(org, rad, mins2, maxs2);

	if(!SolidAABB_BoxCollideP(mins, maxs, mins2, maxs2))
		return(0);
	return(1);
}

int LBXGL_Brush_CombineTextureFlags(int fl0, int fl1)
{
	int flags, mat;
	int i, j;

	flags=
		(fl0&(~LBXGL_TXFL_MATERIAL_MASK))|
		(fl1&(~LBXGL_TXFL_MATERIAL_MASK));

	i=fl0&LBXGL_TXFL_MATERIAL_MASK;
	j=fl1&LBXGL_TXFL_MATERIAL_MASK;

	if(i)
	{
		if(!j || (i==j))
		{
			mat=i;
		}else
		{
			mat=LBXGL_TXFL_MATERIAL_SOLID;
		}
	}else mat=j;
	
	return(flags|mat);
}

int LBXGL_Brush_BuildFace(LBXGL_Brush *brush, int id,
	float *xyz, float *st, int *rtn)
{
	float pts0[64*3];
	float pts1[64*3];
	char buf[256];

	float tv0[3], tv1[3], tv2[3];
	float org[3];
	float sx, sy, f, g;
	int tn, xs, ys;
	int i, j, k, l, t;

	Hull_MakePlaneFace(brush->norm+id*4, pts0);
	l=4;

	for(i=0; i<brush->n_face; i++)
	{
		if(i==id)continue;
		l=Hull_ClipFace(brush->norm+i*4, pts0, pts1, l);
//		for(j=0; j<(l*3); j++)pts0[j]=pts1[j];

		//copy and clamp to face
		for(j=0; j<l; j++)
		{
			f=V3F_NDOT(pts1+(j*3), brush->norm+id*4);
			V3F_ADDSCALE(pts1+(j*3), brush->norm+id*4, -f, pts0+(j*3));
		}
	}

	if(!l)
	{
		printf("LBXGL_Brush_BuildFace: Bad Brush, Face clipped away\n");
		return(0);
	}

	sprintf(buf, "textures/%s", brush->tex[id]);
//	tn=Tex_LoadFile(buf, &xs, &ys);
	tn=LBXGL_Texture_LoadImageSize(buf, &xs, &ys);
	
	if(tn<0)
	{
		tn=LBXGL_Texture_LoadImageSize(
			"textures/effects/missingtex", &xs, &ys);
	}
	
	*rtn=tn;

	i=LBXGL_Texture_GetImageFlags(tn);
	if(i&LBXGL_TXFL_ALPHA)brush->flags|=LBXGL_BRFL_ALPHA;
	if(i&LBXGL_TXFL_VALPHA)brush->flags|=LBXGL_BRFL_ALPHA;
	if(i&LBXGL_TXFL_WINDOW)brush->flags|=LBXGL_BRFL_ALPHA;

//	if(i&LBXGL_TXFL_NODRAW)brush->flags|=LBXGL_BRFL_ALPHA;
	if(i&LBXGL_TXFL_NODRAW)brush->flags|=LBXGL_BRFL_CHKDRAW;

	if(i&LBXGL_TXFL_NOSHADOW)brush->flags|=LBXGL_BRFL_NOSHADOW;

	brush->flags=LBXGL_Brush_CombineTextureFlags(
		brush->flags, i&LBXGL_TXFL_MATERIAL_MASK);

	brush->texfl=LBXGL_Brush_CombineTextureFlags(brush->texfl, i);

#if 1
	if(i&LBXGL_TXFL_FLUID)
	{
		brush->flags|=LBXGL_BRFL_ALPHA|LBXGL_BRFL_FLUID;
		brush->flags|=i&LBXGL_TXFL_FLUID_MASK;
	}

	if(i&LBXGL_TXFL_EFFECT)
	{
		brush->flags|=i&LBXGL_TXFL_EFFECT_MASK;
	}
#endif

	if(tn<0)
	{
		xs=256;
		ys=256;
	}

	sx=LBXGL_Texture_GetImageScaleX(tn);
	sy=LBXGL_Texture_GetImageScaleY(tn);

	for(i=0; i<l; i++)
	{
		V3F_COPY(pts0+i*3, xyz+i*3);
		st[i*2+0]=V3F_NDOT(pts0+i*3, brush->sdir+id*4)/(xs*sx);
		st[i*2+1]=V3F_NDOT(pts0+i*3, brush->tdir+id*4)/(ys*sy);
	}
	return(l);
}

LBXGL_API void LBXGL_Brush_BuildFaces(LBXGL_Brush *brush)
{
	float xyz[256*3];
	float st[256*3];
	float xyz2[256*3];
	float st2[256*3];

	int iv[64];
	int nv[64];
	int nt[64];
	int np[64];
	int ev[64*4];

	float org[3], tv0[3], tv1[3];
	LBXGL_Brush *tlst;
	double f, g;
	int nxyz, nst, npoly, nvec, nev;
	int i, j, k, l, i0, i1;

	if(brush->patch)
	{
		LBXGL_BrushPatch_BuildFaces(brush);
		return;
	}

	if(brush->mesh)
	{
		LBXGL_BrushMesh_BuildFaces(brush);
		return;
	}

	brush->flags&=~(LBXGL_BRFL_ALPHA|LBXGL_TXFL_FLUID_MASK|
		LBXGL_TXFL_EFFECT_MASK|LBXGL_BRFL_NOSHADOW|LBXGL_BRFL_QUADS);
	brush->texfl=0;

	nvec=0; npoly=0;
	for(i=0; i<brush->n_face; i++)
	{
		np[npoly]=i;
		nv[npoly]=LBXGL_Brush_BuildFace(brush, i,
			xyz+nvec*3, st+nvec*2, &nt[npoly]);
		if(!nv[npoly])continue;
		iv[npoly]=nvec;
		nvec+=nv[npoly];
		npoly++;
	}
	brush->n_vec=nvec;
	brush->n_poly=npoly;

	for(i=0; i<npoly; i++)
		if(nv[i]!=4)
			break;
	if(i>=npoly)
		brush->flags|=LBXGL_BRFL_QUADS;

	brush->pf_plane=gcatomic(brush->n_poly*sizeof(byte));
	brush->pf_ivec=gcatomic(brush->n_poly*sizeof(short));
	brush->pf_nvec=gcatomic(brush->n_poly*sizeof(short));

	brush->pf_flag=gcatomic(brush->n_poly*sizeof(int));
	brush->pf_tex=gcatomic(brush->n_poly*sizeof(int));

	brush->vec=gcatomic(brush->n_vec*4*sizeof(short));

	for(i=0; i<brush->n_poly; i++)
	{
		brush->pf_plane[i]=np[i];
		brush->pf_ivec[i]=iv[i];
		brush->pf_nvec[i]=nv[i];
		brush->pf_tex[i]=nt[i];

		j=LBXGL_Texture_GetImageFlags(nt[i]);
		brush->pf_flag[i]=j;
	}

#if 1
	for(i=0; i<brush->n_poly; i++)
	{
//		if(!(brush->pf_flag[i]&(LBXGL_TXFL_ALPHA|LBXGL_TXFL_NODRAW)))
//			break;

		if(!(brush->pf_flag[i]&LBXGL_TXFL_ALPHA))
			break;
	}
	if(i>=brush->n_poly)
		brush->flags|=LBXGL_BRFL_ALPHA;
#endif

	nxyz=0; nst=0;
	for(i=0; i<brush->n_vec; i++)
	{
		for(j=0; j<nxyz; j++)if(V3F_DIST(xyz+i*3, xyz2+j*3)<0.1)break;
		if(j>=nxyz) { j=nxyz++; V3F_COPY(xyz+i*3, xyz2+j*3); }
		brush->vec[i*4+0]=j;

		for(j=0; j<nst; j++)if(V2F_DIST(st+i*2, st2+j*2)<0.01)break;
		if(j>=nst) { j=nst++; V2F_COPY(st+i*2, st2+j*2); }
		brush->vec[i*4+1]=j;
	}

	brush->n_xyz=nxyz;
	brush->n_st=nst;
	brush->xyz=gcatomic(brush->n_xyz*3*sizeof(float));
	brush->st=gcatomic(brush->n_st*2*sizeof(float));

	for(i=0; i<brush->n_xyz; i++)
	{
		//copy, but round vertices
		for(j=0; j<3; j++)
		{
			k=xyz2[i*3+j]*256;
			brush->xyz[i*3+j]=k/256.0;
		}

//		V3F_COPY(xyz2+i*3, brush->xyz+i*3);
	}

	for(i=0; i<brush->n_st; i++) { V2F_COPY(st2+i*2, brush->st+i*2); }

#if 0
	nev=0;
	for(i=0; i<npoly; i++)
	{
		k=iv[i];
		for(j=0; j<(nv[i]-1); j++)
		{
			ev[nev*4+0]=brush->vec[(j+k)*4+0];
			ev[nev*4+1]=brush->vec[(j+k+1)*4+0];
			ev[nev*4+3]=i; ev[nev*4+4]=i;
			nev++;
		}
		ev[nev*4+0]=brush->vec[(j+nv[i]-1)*4+0];
		ev[nev*4+1]=brush->vec[(j+0)*4+0];
		ev[nev*4+3]=i; ev[nev*4+4]=i;
		nev++;
	}

	for(i=0; i<npoly; i++)
	{
		k=iv[i];
		for(j=0; j<(nv[i]-1); j++)
		{
			i0=brush->vec[(j+k)*4+0];
			i1=brush->vec[(j+k+1)*4+0];

			for(l=0; l<nev; l++)
				if((ev[l*4+0]==i1) && (ev[l*4+1]==i0))
					break;
			if(l<nev) { ev[l*4+3]=i; continue; }
			//?...
		}

		i0=brush->vec[(j+nv[i]-1)*4+0]; i1=brush->vec[(j+0)*4+0];
		for(l=0; l<nev; l++)if((ev[l*4+0]==i1) && (ev[l*4+1]==i0))
			break;
		if(l<nev) { ev[l*4+3]=i; continue; }
		//?...
	}
#endif

	nev=0;
	for(i=0; i<npoly; i++)
	{
		k=iv[i];
		for(j=0; j<(nv[i]-1); j++)
		{
			i0=brush->vec[(j+k)*4+0];
			i1=brush->vec[(j+k+1)*4+0];

			for(l=0; l<nev; l++)
				if((ev[l*4+0]==i1) && (ev[l*4+1]==i0))
					break;
			if(l<nev) { ev[l*4+3]=i; continue; }

			ev[nev*4+0]=i0; ev[nev*4+1]=i1;
			ev[nev*4+2]=i; ev[nev*4+3]=i;
			nev++;
		}

		i0=brush->vec[(k+nv[i]-1)*4+0]; i1=brush->vec[(k+0)*4+0];
		for(l=0; l<nev; l++)if((ev[l*4+0]==i1) && (ev[l*4+1]==i0))
			break;
		if(l<nev) { ev[l*4+3]=i; continue; }

		ev[nev*4+0]=i0; ev[nev*4+1]=i1;
		ev[nev*4+2]=i; ev[nev*4+3]=i;
		nev++;
	}

	brush->n_edge=nev;
	brush->edge=gcatomic(brush->n_edge*4*sizeof(short));
	for(i=0; i<(brush->n_edge*4); i++)brush->edge[i]=ev[i];



	if(!brush->norg)brush->norg=gcatomic(brush->n_face*3*sizeof(float));

	for(i=0; i<brush->n_face; i++)
		{ V3F_ZERO(brush->norg+i*3); }

	for(i=0; i<npoly; i++)
	{
		V3F_ZERO(org);
		k=iv[i];
		for(j=0; j<nv[i]; j++)
			{ V3F_ADD(org, xyz+(j+k)*3, org); }
		V3F_SCALE(org, 1.0/nv[i], org);
		V3F_COPY(org, brush->norg+np[i]*3);
	}

	V3F_ZERO(org);
	for(i=0; i<brush->n_face; i++)
		{ V3F_ADD(org, brush->norg+i*3, org); }
	V3F_SCALE(org, 1.0/brush->n_face, brush->org);

	g=0;
	for(i=0; i<nvec; i++)
	{
		V3F_SUB(xyz+i*3, brush->org, tv0);
		f=V3F_LEN(tv0);
		if(f>g)g=f;
	}
	brush->rad=g;


	brush->mins[0]= 999999; brush->mins[1]= 999999; brush->mins[2]= 999999;
	brush->maxs[0]=-999999; brush->maxs[1]=-999999; brush->maxs[2]=-999999;
	for(i=0; i<nvec; i++)
	{
		if(xyz[i*3+0]<brush->mins[0])brush->mins[0]=xyz[i*3+0];
		if(xyz[i*3+1]<brush->mins[1])brush->mins[1]=xyz[i*3+1];
		if(xyz[i*3+2]<brush->mins[2])brush->mins[2]=xyz[i*3+2];

		if(xyz[i*3+0]>brush->maxs[0])brush->maxs[0]=xyz[i*3+0];
		if(xyz[i*3+1]>brush->maxs[1])brush->maxs[1]=xyz[i*3+1];
		if(xyz[i*3+2]>brush->maxs[2])brush->maxs[2]=xyz[i*3+2];
	}

	if((brush->mins[0]<-524288) ||
		(brush->mins[1]<-524288) ||
		(brush->mins[2]<-524288) ||
		(brush->maxs[0]>524288) ||
		(brush->maxs[1]>524288) ||
		(brush->maxs[2]>524288))
	{
		printf("LBXGL_Brush_BuildFaces: "
			"Brush BBox Sanity Check: Bad Size\n");
	}

#if 0
	//determine if certain faces face outside the map.
	for(i=0; i<brush->n_poly; i++)
	{
		if(!lbxgl_world)
		{
			printf("check brush outside: no world\n");
			break;
		}
		V3F_ADDSCALE(brush->org,
			brush->norm+(brush->pf_plane[i]*4), 99999, tv0);
		tlst=LBXGL_BrushWorld_TraceLineBrush(
			lbxgl_world, brush->org, tv0, brush);

		if(!tlst)
		{
//			brush->pf_flag[i]|=LBXGL_TXFL_NODRAW;
//			brush->flags|=LBXGL_BRFL_NOSHADOW;
		}

#if 1
//		tlst=NULL;
		for(j=0; (!tlst) && (j<nv[i]); j++)
		{
			V3F_ADDSCALE(xyz+(j+iv[i])*3,
				brush->norm+(brush->pf_plane[i]*4), 2, tv1);

			tlst=LBXGL_BrushWorld_TraceLineBrush(
				lbxgl_world, tv1, tv0, brush);

//			tlst=LBXGL_BrushWorld_TraceLineBrush(
//				lbxgl_world, xyz+(j+iv[i])*3, tv0, brush);
		}

		if(!tlst)
		{
			brush->pf_flag[i]|=LBXGL_TXFL_NODRAW;
//			brush->flags|=LBXGL_BRFL_NOSHADOW;
		}
#endif

#if 0
		f=V3F_NDOT(brush->org, brush->norm+(brush->pf_plane[i]*4));
		V3F_ADDSCALE(brush->org,
			brush->norm+(brush->pf_plane[i]*4), (-f)+4, tv0);
		tlst=LBXGL_BrushWorld_TraceLineBrush(
			lbxgl_world, brush->org, tv0, brush);

		for(j=0; tlst && (j<nv[i]); j++)
		{
			V3F_ADDSCALE(xyz+(j+iv[i])*3,
				brush->norm+(brush->pf_plane[i]*4), 0, tv0);

			V3F_ADDSCALE(xyz+(j+iv[i])*3,
				brush->norm+(brush->pf_plane[i]*4), 4, tv1);

			tlst=LBXGL_BrushWorld_TraceLineBrush(
				lbxgl_world, tv0, tv1, brush);
			if(!tlst)break;

//			tlst=LBXGL_BrushWorld_TraceLineBrush(
//				lbxgl_world, xyz+(j+iv[i])*3, tv0, brush);
		}

		if(tlst)
		{
			brush->pf_flag[i]|=LBXGL_TXFL_NODRAW;
//			brush->flags|=LBXGL_BRFL_NOSHADOW;
		}
#endif
	}
#endif
}


LBXGL_API void LBXGL_Brush_DestroyFaces(LBXGL_Brush *brush)
{
	if(brush->patch)
	{
		LBXGL_BrushPatch_DestroyFaces(brush);
		return;
	}

	if(brush->mesh)
	{
		LBXGL_BrushMesh_DestroyFaces(brush);
		return;
	}

	BTGE_BrushCSG_DestroyBrushFaces(brush);

	if(brush->xyz)gcfree(brush->xyz);
	if(brush->st)gcfree(brush->st);
	brush->xyz=NULL;
	brush->st=NULL;
	brush->n_xyz=0;
	brush->n_st=0;

	if(brush->pf_plane)gcfree(brush->pf_plane);
	if(brush->pf_ivec)gcfree(brush->pf_ivec);
	if(brush->pf_nvec)gcfree(brush->pf_nvec);
	if(brush->pf_flag)gcfree(brush->pf_flag);
	if(brush->pf_tex)gcfree(brush->pf_tex);

	brush->pf_plane=NULL;
	brush->pf_ivec=NULL;
	brush->pf_nvec=NULL;
	brush->pf_flag=NULL;
	brush->pf_tex=NULL;
	brush->n_poly=0;

	if(brush->vec)gcfree(brush->vec);
	brush->vec=NULL;
	brush->n_vec=0;

//	if(brush->dlrgb)gcfree(brush->dlrgb);
//	if(brush->srgb)gcfree(brush->srgb);
	if(brush->rgb)gcfree(brush->rgb);
//	brush->dlrgb=NULL;
//	brush->srgb=NULL;
	brush->rgb=NULL;
}

LBXGL_API int LBXGL_Brush_CheckPointBrush(LBXGL_Brush *brush, float *pt)
{
	float tv0[3], tv1[3], tv2[3];
	float f, g;
	int i;

	if(brush->patch)
		{ return(LBXGL_BrushPatch_CheckPointBrush(brush, pt)); }

	if(brush->mesh)
		{ return(LBXGL_BrushMesh_CheckPointBrush(brush, pt)); }

	if(brush->xyz)
		{ f=V3F_DIST(pt, brush->org); if(f>=brush->rad)return(0); }
	for(i=0; i<brush->n_face; i++)
		{ f=V3F_NDOT(pt, brush->norm+i*4); if(f>0)return(0); }
	return(1);
}

LBXGL_API float LBXGL_Brush_DistancePointBrush(
	LBXGL_Brush *brush, float *pt)
{
	float tv0[3], tv1[3], tv2[3];
	float f, g;
	int i;

	if(brush->patch)
	{
		f=V3F_DIST(pt, brush->org);
		if(f<=brush->rad)return(0);
		return(f-brush->rad);
//		return(LBXGL_BrushPatch_CheckPointBrush(brush, pt));
	}

	if(brush->mesh)
	{
		f=V3F_DIST(pt, brush->org);
		if(f<=brush->rad)return(0);
		return(f-brush->rad);
//		return(LBXGL_BrushPatch_CheckPointBrush(brush, pt));
	}

//	if(brush->xyz)
//		{ f=V3F_DIST(pt, brush->org); if(f>=brush->rad)return(0); }
	g=0;
	for(i=0; i<brush->n_face; i++)
	{
		f=V3F_NDOT(pt, brush->norm+i*4);
		if(f>0)g+=f*f;
	}
	return(sqrt(g));
}

LBXGL_API int LBXGL_Brush_CheckLineBrushClip(LBXGL_Brush *brush,
	float *sv, float *ev, float *csv, float *cev)
{
	float tv0[3], tv1[3], tv2[3];
	float f, g;
	int i;

	if(brush->patch)
		return(LBXGL_BrushPatch_CheckLineBrushClip(
			brush, sv, ev, csv, cev));

	if(brush->mesh)
		return(LBXGL_BrushMesh_CheckLineBrushClip(
			brush, sv, ev, csv, cev));

	if(csv) { V3F_COPY(sv, csv); }
	if(cev) { V3F_COPY(ev, cev); }

#if 0
	if(brush->xyz)
	{
		V3F_SUB(ev, sv, tv0);
		V3F_NORMALIZE(tv0, tv0);
		f=V3F_DOT(brush->org, tv0)-V3F_DOT(sv, tv0);
		V3F_ADDSCALE(sv, tv0, f, tv1);
		f=V3F_DIST(tv1, brush->org);
		if(f>=brush->rad)return(0);
	}
#endif

	V3F_COPY(sv, tv0);
	V3F_COPY(ev, tv1);

	for(i=0; i<brush->n_face; i++)
	{
		f=V3F_NDOT(tv0, brush->norm+i*4);
		g=V3F_NDOT(tv1, brush->norm+i*4);
		if((f>0) && (g>0))return(0);

		if((f<=0) && (g<=0))continue;

		Hull_LinePlaneIntersect(tv0, tv1, brush->norm+i*4, tv2);

		if(f>0) { V3F_COPY(tv2, tv0); }
			else { V3F_COPY(tv2, tv1); }
	}

	if(csv) { V3F_COPY(tv0, csv); }
	if(cev) { V3F_COPY(tv1, cev); }

	return(1);
}

LBXGL_API int LBXGL_Brush_CheckLineBrush(LBXGL_Brush *brush, float *sv, float *ev)
{
	return(LBXGL_Brush_CheckLineBrushClip(brush, sv, ev, NULL, NULL));
}

LBXGL_API int LBXGL_Brush_CheckBoxBrush(LBXGL_Brush *brush,
	float *mins, float *maxs)
{
	float m, n;
	int i;

	if(brush->patch)
		return(LBXGL_BrushPatch_CheckBoxBrush(brush, mins, maxs));

	if(brush->mesh)
		return(LBXGL_BrushMesh_CheckBoxBrush(brush, mins, maxs));

	if(brush->xyz)
	{
		if(!SolidAABB_BoxCollideP(mins, maxs, brush->mins, brush->maxs))
			return(0);
	}

	for(i=0; i<brush->n_face; i++)
	{
		Hull_BoxPlaneExtents(mins, maxs, brush->norm+i*4, &m, &n);
		if(m>=0)return(0);
	}
	return(1);
}

LBXGL_API int LBXGL_Brush_CheckSphereBrush(LBXGL_Brush *brush,
	float *org, float rad)
{
	float dir[4];
	float *fv;
	float f, g;
	int i;

//	if(brush->patch)
//		return(LBXGL_BrushPatch_CheckSphereBrush(brush, org, rad));

	for(i=0; i<brush->n_face; i++)
	{
		fv=brush->norm+i*4;
		f=V3F_NDOT(org, fv);
		if(f>=rad)return(0);
	}

	if(brush->patch)
		return(LBXGL_BrushPatch_CheckSphereBrush(brush, org, rad));

	if(brush->mesh)
		return(LBXGL_BrushMesh_CheckSphereBrush(brush, org, rad));

#if 1
	V3F_SUB(brush->org, org, dir);
	f=V3F_NORMALIZE(dir, dir);
//	g=V3F_FRNORMALIZE(dir, dir);
//	f=(g>0)?(1.0/g):0;
	if(f>(brush->rad+rad)) return(0);

#if 1
	dir[3]=V3F_DOT(org, dir);
	LBXGL_Brush_GetPlaneExtents(brush, dir, &f, &g);
//	LBXGL_Brush_GetBBoxPlaneExtents(cur, dir, &f, &g);
	if(f>=rad) return(0);
#endif
#endif

	return(1);
}

LBXGL_API int LBXGL_Brush_GetExtents(LBXGL_Brush *brush, float *vec,
	float *rm, float *rn)
{
	float tv0[3], tv1[3], tv2[3];
	float f, g, m, n;
	int i;

	if(brush->patch)
		return(LBXGL_BrushPatch_GetExtents(brush, vec, rm, rn));

	if(brush->mesh)
		return(LBXGL_BrushMesh_GetExtents(brush, vec, rm, rn));

	m=999999; n=-999999;
	for(i=0; i<brush->n_xyz; i++)
	{
		f=V3F_DOT(brush->xyz+i*3, vec);
		if(f<m)m=f; if(f>n)n=f;
	}

	if(rm)*rm=m; if(rn)*rn=n;
	return(0);
}

LBXGL_API int LBXGL_Brush_GetPlaneExtents(LBXGL_Brush *brush, float *norm,
	float *rm, float *rn)
{
	float tv0[3], tv1[3], tv2[3];
	float f, g, m, n;
	int i;

	if(brush->patch)
		return(LBXGL_BrushPatch_GetPlaneExtents(brush, norm, rm, rn));

	if(brush->mesh)
		return(LBXGL_BrushMesh_GetPlaneExtents(brush, norm, rm, rn));

	m=999999; n=-999999;
	for(i=0; i<brush->n_xyz; i++)
	{
		f=V3F_NDOT(brush->xyz+i*3, norm);
		if(f<m)m=f; if(f>n)n=f;
	}
	if(rm)*rm=m; if(rn)*rn=n;
	return(0);
}

LBXGL_API void LBXGL_Brush_GetBBoxExtents(LBXGL_Brush *brush, float *vec,
	float *rm, float *rn)
{
	float d0, d1;
	int i;

	d0=0; d1=0;
	for(i=0; i<3; i++)
	{
		if(vec[i]>0)
		{
			d0+=brush->maxs[i]*vec[i];
			d1+=brush->mins[i]*vec[i];
		}else
		{
			d0+=brush->mins[i]*vec[i];
			d1+=brush->maxs[i]*vec[i];
		}
	}

	*rm=d1;
	*rn=d0;
}

LBXGL_API void LBXGL_Brush_GetBBoxPlaneExtents(LBXGL_Brush *brush, float *norm,
	float *rm, float *rn)
{
	float d0, d1;
	int i;

	d0=0; d1=0;
	for(i=0; i<3; i++)
	{
		if(norm[i]>0)
		{
			d0+=brush->maxs[i]*norm[i];
			d1+=brush->mins[i]*norm[i];
		}else
		{
			d0+=brush->mins[i]*norm[i];
			d1+=brush->maxs[i]*norm[i];
		}
	}
	*rm=d1-norm[3]; *rn=d0-norm[3];
}

LBXGL_API LBXGL_Brush *LBXGL_Brush_New(int num)
{
	LBXGL_Brush *tmp;

	LBXGL_Brush_Init();

	tmp=gctalloc("lbxgl_brush_t", sizeof(LBXGL_Brush));
	tmp->n_face=num;
	tmp->m_face=num;

	if(num>0)
	{
		tmp->norm=gcatomic(num*4*sizeof(float));
		tmp->sdir=gcatomic(num*4*sizeof(float));
		tmp->tdir=gcatomic(num*4*sizeof(float));
		tmp->norg=gcatomic(num*3*sizeof(float));
		tmp->tex=gcalloc(num*sizeof(char *));
	}

	return(tmp);
}

LBXGL_API LBXGL_Brush *LBXGL_Brush_MakeCubeExtentsTexPos(
	float mx, float my, float mz,
	float nx, float ny, float nz, char *tex,
	float tx, float ty, float tz, int tp)
{
	float torg[3];
	LBXGL_Brush *tmp;
	int i;

	tmp=LBXGL_Brush_New(6);

	tmp->tex[0]=tex;
	tmp->tex[1]=tex;
	tmp->tex[2]=tex;
	tmp->tex[3]=tex;
	tmp->tex[4]=tex;
	tmp->tex[5]=tex;

	tmp->norm[0*4+0]=-1;
	tmp->norm[0*4+1]=0;
	tmp->norm[0*4+2]=0;
	tmp->norm[0*4+3]=-mx;

	tmp->norm[1*4+0]=1;
	tmp->norm[1*4+1]=0;
	tmp->norm[1*4+2]=0;
	tmp->norm[1*4+3]=nx;

	tmp->norm[2*4+0]=0;
	tmp->norm[2*4+1]=-1;
	tmp->norm[2*4+2]=0;
	tmp->norm[2*4+3]=-my;

	tmp->norm[3*4+0]=0;
	tmp->norm[3*4+1]=1;
	tmp->norm[3*4+2]=0;
	tmp->norm[3*4+3]=ny;

	tmp->norm[4*4+0]=0;
	tmp->norm[4*4+1]=0;
	tmp->norm[4*4+2]=-1;
	tmp->norm[4*4+3]=-mz;

	tmp->norm[5*4+0]=0;
	tmp->norm[5*4+1]=0;
	tmp->norm[5*4+2]=1;
	tmp->norm[5*4+3]=nz;

	torg[0]=tx; torg[1]=ty; torg[2]=tz;
	for(i=0; i<6; i++)
	{
		if(tp==2)
		{
			Hull_AdjacentNormals22(tmp->norm+i*4,
				tmp->sdir+i*4, tmp->tdir+i*4);
		}else
		{
			Hull_AdjacentNormals2(tmp->norm+i*4,
				tmp->sdir+i*4, tmp->tdir+i*4);
		}
		tmp->sdir[i*4+3]=V3F_DOT(torg, tmp->sdir+i*4);
		tmp->tdir[i*4+3]=V3F_DOT(torg, tmp->tdir+i*4);
	}

	return(tmp);
}

LBXGL_API LBXGL_Brush *LBXGL_Brush_MakeCubeExtents(
	float mx, float my, float mz,
	float nx, float ny, float nz, char *tex)
{
	LBXGL_Brush *tmp;

	tmp=LBXGL_Brush_MakeCubeExtentsTexPos(
		mx, my, mz, nx, ny, nz, tex, 0, 0, 0, 0);
	return(tmp);
}

LBXGL_API LBXGL_Brush *LBXGL_Brush_MakeCube(float x, float y, float z,
	float xs, float ys, float zs, char *tex)
{
	LBXGL_Brush *tmp;

	tmp=LBXGL_Brush_MakeCubeExtents(
		x-xs, y-ys, z-zs, x+xs, y+ys, z+zs, tex);
	return(tmp);
}

LBXGL_API void LBXGL_Brush_MakeRoomFromBrush(
	LBXGL_BrushWorld *world, LBXGL_Brush *brush)
{
	LBXGL_Brush *btmp;

	if(!brush->xyz)LBXGL_Brush_BuildFaces(brush);

	btmp=LBXGL_Brush_MakeCubeExtents(
		brush->mins[0]-8,
		brush->mins[1]-0,
		brush->mins[2]-0,
		brush->mins[0]+8,
		brush->maxs[1]+0,
		brush->maxs[2]+0,
		brush->tex[0]);
	LBXGL_BrushWorld_AddBrush(world, btmp);

	btmp=LBXGL_Brush_MakeCubeExtents(
		brush->maxs[0]-8,
		brush->mins[1]-0,
		brush->mins[2]-0,
		brush->maxs[0]+8,
		brush->maxs[1]+0,
		brush->maxs[2]+0,
		brush->tex[1]);
	LBXGL_BrushWorld_AddBrush(world, btmp);

	btmp=LBXGL_Brush_MakeCubeExtents(
		brush->mins[0]-0,
		brush->mins[1]-8,
		brush->mins[2]-0,
		brush->maxs[0]+0,
		brush->mins[1]+8,
		brush->maxs[2]+0,
		brush->tex[2]);
	LBXGL_BrushWorld_AddBrush(world, btmp);

	btmp=LBXGL_Brush_MakeCubeExtents(
		brush->mins[0]-0,
		brush->maxs[1]-8,
		brush->mins[2]-0,
		brush->maxs[0]+0,
		brush->maxs[1]+8,
		brush->maxs[2]+0,
		brush->tex[3]);
	LBXGL_BrushWorld_AddBrush(world, btmp);

	btmp=LBXGL_Brush_MakeCubeExtents(
		brush->mins[0]-0,
		brush->mins[1]-0,
		brush->mins[2]-8,
		brush->maxs[0]+0,
		brush->maxs[1]+0,
		brush->mins[2]+8,
		brush->tex[4]);
	LBXGL_BrushWorld_AddBrush(world, btmp);

	btmp=LBXGL_Brush_MakeCubeExtents(
		brush->mins[0]-0,
		brush->mins[1]-0,
		brush->maxs[2]-8,
		brush->maxs[0]+0,
		brush->maxs[1]+0,
		brush->maxs[2]+8,
		brush->tex[5]);
	LBXGL_BrushWorld_AddBrush(world, btmp);
}

LBXGL_API void LBXGL_Brush_MakeRoomExtents(
	LBXGL_BrushWorld *world,
	float mx, float my, float mz,
	float nx, float ny, float nz,
	char *floortex, char *walltex, char *ceiltex)
{
	LBXGL_Brush *btmp;

	btmp=LBXGL_Brush_MakeCubeExtents(
		mx-8, my-0, mz-0, mx+8, ny+0, nz+0, walltex);
	LBXGL_BrushWorld_AddBrush(world, btmp);
	btmp=LBXGL_Brush_MakeCubeExtents(
		nx-8, my-0, mz-0, nx+8, ny+0, nz+0, walltex);
	LBXGL_BrushWorld_AddBrush(world, btmp);
	btmp=LBXGL_Brush_MakeCubeExtents(
		mx-0, my-8, mz-0, nx+0, my+8, nz+0, walltex);
	LBXGL_BrushWorld_AddBrush(world, btmp);
	btmp=LBXGL_Brush_MakeCubeExtents(
		my-0, ny-8, mz-0, nx+0, ny+8, nz+0, walltex);
	LBXGL_BrushWorld_AddBrush(world, btmp);
	btmp=LBXGL_Brush_MakeCubeExtents(
		mx-0, my-0, mz-8, nx+0, ny+0, mz+8, floortex);
	LBXGL_BrushWorld_AddBrush(world, btmp);
	btmp=LBXGL_Brush_MakeCubeExtents(
		mx-0, my-0, nz-8, nx+0, ny+0, nz+8, ceiltex);
	LBXGL_BrushWorld_AddBrush(world, btmp);
}

LBXGL_API int LBXGL_Brush_SubtractBrushAB(
	LBXGL_BrushWorld *world, LBXGL_Brush **rlst,
	LBXGL_Brush *lbrush, LBXGL_Brush *rbrush)
{
	float cmins[3], cmaxs[3];
	LBXGL_Brush *btmp;

	if(lbrush==rbrush)
		return(0);

	if(!lbrush->xyz)LBXGL_Brush_BuildFaces(lbrush);
	if(!rbrush->xyz)LBXGL_Brush_BuildFaces(rbrush);

	if(!SolidAABB_BoxCollideP(
		lbrush->mins, lbrush->maxs,
		rbrush->mins, rbrush->maxs))
			return(0);
	
	V3F_MAX(lbrush->mins, rbrush->mins, cmins);
	V3F_MIN(lbrush->maxs, rbrush->maxs, cmaxs);
	
	if(lbrush->mins[0]<rbrush->mins[0])
	{
		btmp=LBXGL_Brush_MakeCubeExtents(
			lbrush->mins[0], cmins[1], cmins[2],
			rbrush->mins[0], cmaxs[1], cmaxs[2],
			lbrush->tex[0]);

//		LBXGL_BrushWorld_AddBrush(world, btmp);
		btmp->chain=*rlst; *rlst=btmp;
	}

	if(lbrush->maxs[0]>rbrush->maxs[0])
	{
		btmp=LBXGL_Brush_MakeCubeExtents(
			rbrush->maxs[0], cmins[1], cmins[2],
			lbrush->maxs[0], cmaxs[1], cmaxs[2],
			lbrush->tex[1]);
//		LBXGL_BrushWorld_AddBrush(world, btmp);
		btmp->chain=*rlst; *rlst=btmp;
	}


	if(lbrush->mins[1]<rbrush->mins[1])
	{
		btmp=LBXGL_Brush_MakeCubeExtents(
			lbrush->mins[0], lbrush->mins[1], cmins[2],
			lbrush->maxs[0], rbrush->mins[1], cmaxs[2],
			lbrush->tex[2]);
//		LBXGL_BrushWorld_AddBrush(world, btmp);
		btmp->chain=*rlst; *rlst=btmp;
	}

	if(lbrush->maxs[1]>rbrush->maxs[1])
	{
		btmp=LBXGL_Brush_MakeCubeExtents(
			lbrush->mins[0], rbrush->maxs[1], cmins[2],
			lbrush->maxs[0], lbrush->maxs[1], cmaxs[2],
			lbrush->tex[3]);
//		LBXGL_BrushWorld_AddBrush(world, btmp);
		btmp->chain=*rlst; *rlst=btmp;
	}


	if(lbrush->mins[2]<rbrush->mins[2])
	{
		btmp=LBXGL_Brush_MakeCubeExtents(
			lbrush->mins[0], lbrush->mins[1], lbrush->mins[2],
			lbrush->maxs[0], lbrush->maxs[1], rbrush->mins[2],
			lbrush->tex[4]);
//		LBXGL_BrushWorld_AddBrush(world, btmp);
		btmp->chain=*rlst; *rlst=btmp;
	}

	if(lbrush->maxs[2]>rbrush->maxs[2])
	{
		btmp=LBXGL_Brush_MakeCubeExtents(
			lbrush->mins[0], lbrush->mins[1], rbrush->maxs[2],
			lbrush->maxs[0], lbrush->maxs[1], lbrush->maxs[2],
			lbrush->tex[5]);
//		LBXGL_BrushWorld_AddBrush(world, btmp);
		btmp->chain=*rlst; *rlst=btmp;
	}
	
	return(1);
}

LBXGL_API void LBXGL_Brush_SubtractBrush(
	LBXGL_BrushWorld *world, LBXGL_Brush *brush)
{
	LBXGL_Brush *cur, *lst;
	LBXGL_SEntity *ecur, *opsel;
	char *s;
	int i;

	opsel=world->entity_psel;

	ecur=world->entity;
	while(ecur)
	{
		if(ecur->brush)
		{
			world->entity_psel=ecur;
			cur=ecur->brush; lst=NULL;
			while(cur)
			{
				if(cur->flags&LBXGL_BRFL_DEL)
					{ cur=cur->enext; continue; }
//				if(!cur->xyz)
//					LBXGL_Brush_BuildFaces(cur);

				i=LBXGL_Brush_SubtractBrushAB(world, &lst, cur, brush);
				if(i)cur->flags|=LBXGL_BRFL_DEL|BTGE_BRFL_DIRTY;
				
				cur=cur->enext;
			}
			
			cur=lst;
			while(cur)
			{
				LBXGL_BrushWorld_AddBrush(world, cur);
				cur=cur->chain;
			}
		}

		ecur=ecur->next;
	}

	world->entity_psel=opsel;
}


LBXGL_API int LBXGL_Brush_SubtractBrushLstBBox(
	LBXGL_BrushWorld *world, LBXGL_Brush **rlst,
	LBXGL_Brush *lbrush, float *rmins, float *rmaxs)
{
	float cmins[3], cmaxs[3];
	float lmins[3], lmaxs[3];
	LBXGL_Brush *btmp;

//	if(lbrush==rbrush)
//		return(0);

	if(!lbrush->xyz)LBXGL_Brush_BuildFaces(lbrush);
//	if(!rbrush->xyz)LBXGL_Brush_BuildFaces(rbrush);

	if(!SolidAABB_BoxCollideP(
		lbrush->mins, lbrush->maxs, rmins, rmaxs))
			return(0);
	
	V3F_COPY(lbrush->mins, lmins);
	V3F_COPY(lbrush->maxs, lmaxs);
	
	V3F_MAX(lbrush->mins, rmins, cmins);
	V3F_MIN(lbrush->maxs, rmaxs, cmaxs);
	
	if(lbrush->mins[0]<rmins[0])
	{
		btmp=LBXGL_Brush_MakeCubeExtents(
			lmins[0], cmins[1], cmins[2],
			rmins[0], cmaxs[1], cmaxs[2],
			lbrush->tex[0]);

//		LBXGL_BrushWorld_AddBrush(world, btmp);
		btmp->chain=*rlst; *rlst=btmp;
	}

	if(lbrush->maxs[0]>rmaxs[0])
	{
		btmp=LBXGL_Brush_MakeCubeExtents(
			rmaxs[0], cmins[1], cmins[2],
			lmaxs[0], cmaxs[1], cmaxs[2],
			lbrush->tex[1]);
//		LBXGL_BrushWorld_AddBrush(world, btmp);
		btmp->chain=*rlst; *rlst=btmp;
	}


	if(lbrush->mins[1]<rmins[1])
	{
		btmp=LBXGL_Brush_MakeCubeExtents(
			lmins[0], lmins[1], cmins[2],
			lmaxs[0], rmins[1], cmaxs[2],
			lbrush->tex[2]);
//		LBXGL_BrushWorld_AddBrush(world, btmp);
		btmp->chain=*rlst; *rlst=btmp;
	}

	if(lbrush->maxs[1]>rmaxs[1])
	{
		btmp=LBXGL_Brush_MakeCubeExtents(
			lmins[0], rmaxs[1], cmins[2],
			lmaxs[0], lmaxs[1], cmaxs[2],
			lbrush->tex[3]);
//		LBXGL_BrushWorld_AddBrush(world, btmp);
		btmp->chain=*rlst; *rlst=btmp;
	}


	if(lbrush->mins[2]<rmins[2])
	{
		btmp=LBXGL_Brush_MakeCubeExtents(
			lmins[0], lmins[1], lmins[2],
			lmaxs[0], lmaxs[1], rmins[2],
			lbrush->tex[4]);
//		LBXGL_BrushWorld_AddBrush(world, btmp);
		btmp->chain=*rlst; *rlst=btmp;
	}

	if(lbrush->maxs[2]>rmaxs[2])
	{
		btmp=LBXGL_Brush_MakeCubeExtents(
			lmins[0], lmins[1], rmaxs[2],
			lmaxs[0], lmaxs[1], lmaxs[2],
			lbrush->tex[5]);
//		LBXGL_BrushWorld_AddBrush(world, btmp);
		btmp->chain=*rlst; *rlst=btmp;
	}
	
	return(1);
}

LBXGL_API void LBXGL_Brush_SubtractBrushBBox(
	LBXGL_BrushWorld *world, float *mins, float *maxs)
{
	LBXGL_Brush *cur, *lst;
	LBXGL_SEntity *ecur, *opsel;
	char *s;
	int i;

	opsel=world->entity_psel;

	ecur=world->entity;
	while(ecur)
	{
		if(ecur->brush)
		{
			world->entity_psel=ecur;
			cur=ecur->brush; lst=NULL;
			while(cur)
			{
				if(cur->flags&LBXGL_BRFL_DEL)
					{ cur=cur->enext; continue; }
//				if(!cur->xyz)
//					LBXGL_Brush_BuildFaces(cur);

				i=LBXGL_Brush_SubtractBrushLstBBox(
					world, &lst, cur, mins, maxs);
				if(i)cur->flags|=LBXGL_BRFL_DEL|BTGE_BRFL_DIRTY;
				
				cur=cur->enext;
			}
			
			cur=lst;
			while(cur)
			{
				LBXGL_BrushWorld_AddBrush(world, cur);
				cur=cur->chain;
			}
		}

		ecur=ecur->next;
	}

	world->entity_psel=opsel;
}


LBXGL_API void LBXGL_Brush_CheckExpandFaces(LBXGL_Brush *brush, int num)
{
	int n;

	if(brush->m_face>=num)
		return;
		
	n=brush->m_face;
	if(!n)n=16;
	while(n<num)n=n+(n>>1);

	brush->norm=gcrealloc(brush->norm, n*4*sizeof(float));
	brush->sdir=gcrealloc(brush->sdir, n*4*sizeof(float));
	brush->tdir=gcrealloc(brush->tdir, n*4*sizeof(float));
	brush->norg=gcrealloc(brush->norg, n*3*sizeof(float));
	brush->tex =gcrealloc(brush->tex , n*sizeof(char *));
	brush->m_face=n;
}

LBXGL_API LBXGL_Brush *LBXGL_Brush_CloneBrush(LBXGL_Brush *brush)
{
	LBXGL_Brush *tmp;
	int n;

	LBXGL_Brush_Init();

	if(brush->mesh)
	{
		tmp=gctalloc("lbxgl_brush_t", sizeof(LBXGL_Brush));
		tmp->mesh=LBXGL_Mesh_ClonePrim(brush->mesh);
		return(tmp);
	}

	tmp=gctalloc("lbxgl_brush_t", sizeof(LBXGL_Brush));
	tmp->n_face=brush->n_face;
	tmp->m_face=brush->m_face;
	tmp->flags=brush->flags;

	n=brush->m_face;
	tmp->norm=gcatomic(n*4*sizeof(float));
	tmp->sdir=gcatomic(n*4*sizeof(float));
	tmp->tdir=gcatomic(n*4*sizeof(float));
	tmp->norg=gcatomic(n*3*sizeof(float));
	tmp->tex=gcalloc(n*1*sizeof(char *));

	memcpy(tmp->norm, brush->norm, n*4*sizeof(float));
	memcpy(tmp->sdir, brush->sdir, n*4*sizeof(float));
	memcpy(tmp->tdir, brush->tdir, n*4*sizeof(float));
	memcpy(tmp->norg, brush->norg, n*3*sizeof(float));
	memcpy(tmp->tex, brush->tex, n*1*sizeof(float));

	return(tmp);
}

LBXGL_API void LBXGL_Brush_DestroyBrush(LBXGL_Brush *brush)
{
	LBXGL_Brush_DestroyFaces(brush);
	
	gcfree(brush->norm);
	gcfree(brush->sdir);
	gcfree(brush->tdir);
	gcfree(brush->norg);
	gcfree(brush->tex);
	gcfree(brush);
}

LBXGL_API LBXGL_Brush *LBXGL_Brush_CloneBrushList(LBXGL_Brush *brush)
{
	LBXGL_Brush *tmp;
	
	if(!brush)return(NULL);
	
	tmp=LBXGL_Brush_CloneBrush(brush);
	tmp->next=LBXGL_Brush_CloneBrushList(brush->next);
	return(tmp);
}

LBXGL_API void LBXGL_Brush_DestroyBrushList(LBXGL_Brush *brush)
{
	LBXGL_Brush *tmp;
	
	if(!brush)return;
	LBXGL_Brush_DestroyBrushList(brush->next);
	LBXGL_Brush_DestroyBrush(brush);
}

LBXGL_API LBXGL_Brush *LBXGL_Brush_CloneBrushEntList(LBXGL_Brush *brush)
{
	LBXGL_Brush *tmp;
	
	if(!brush)return(NULL);
	
	tmp=LBXGL_Brush_CloneBrush(brush);
	tmp->enext=LBXGL_Brush_CloneBrushEntList(brush->enext);
	return(tmp);
}

LBXGL_API void LBXGL_Brush_DestroyBrushEntList(LBXGL_Brush *brush)
{
	LBXGL_Brush *tmp;
	
	if(!brush)return;
	LBXGL_Brush_DestroyBrushEntList(brush->enext);
	LBXGL_Brush_DestroyBrush(brush);
}

int lbxgl_brush_renderdark(dyt obj)
{
	LBXGL_Brush_DrawBrushDark((LBXGL_Brush *)obj);
	return(0);
}

int lbxgl_brush_renderlight(dyt obj)
{
	LBXGL_Brush_DrawBrushLight2((LBXGL_Brush *)obj);
	return(0);
}

int lbxgl_brush_rendershadow(dyt obj, float *org)
{
//	LBXGL_Brush_DrawShadowVolume((LBXGL_Brush *)obj, org);
	return(0);
}

int lbxgl_brush_renderfinal(dyt obj)
{
	LBXGL_Brush_DrawFacesShadow((LBXGL_Brush *)obj);
	return(0);
}

int lbxgl_brush_getsphere(dyt obj, float *org, float *rad)
{
	LBXGL_Brush *tmp;

	tmp=(LBXGL_Brush *)obj;
	V3F_COPY(tmp->org, org);
	*rad=tmp->rad;
	return(0);
}

LBXGL_API void LBXGL_Brush_Init()
{
	static int init=0;
	LBXGL_RenderMethods *tmp;

	if(init)return;
	init=1;

	tmp=LBXGL_Method_GetRenderMethods("lbxgl_brush_t");
	tmp->render_dark=lbxgl_brush_renderdark;
	tmp->render_light=lbxgl_brush_renderlight;
	tmp->render_shadow=lbxgl_brush_rendershadow;
	tmp->render_final=lbxgl_brush_renderfinal;
	tmp->get_sphere=lbxgl_brush_getsphere;
}
