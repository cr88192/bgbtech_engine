#include <btgecm.h>

//float BTGE_Texture_GetImageScaleX(int num)
//	{ return(1); }
//float BTGE_Texture_GetImageScaleY(int num)
//	{ return(1); }
//int BTGE_Texture_GetImageFlags(int num)
//	{ return(0); }
//int BTGE_Texture_LoadImageSize(char *name, int *rxs, int *rys)
//	{ return(1); }

BTGE_API int Hull_LinePlaneIntersect(float *start, float *end,
	float *norm, float *point)
{
	float dir[3], x;

	//calc direction
	dir[0]=end[0]-start[0];
	dir[1]=end[1]-start[1];
	dir[2]=end[2]-start[2];

	x=V3F_DOT(dir, norm);
	if(x==0)return(-1);

	x=1.0/((x<0)?-x:x);
	dir[0]*=x;
	dir[1]*=x;
	dir[2]*=x;

	//calc intersection
	x=V3F_NDOT(start, norm);

	x=(x<0)?-x:x;
	point[0]=start[0]+dir[0]*x;
	point[1]=start[1]+dir[1]*x;
	point[2]=start[2]+dir[2]*x;

	return(0);
}

BTGE_API int Hull_LinePlaneIntersectST(float *sv, float *sst,
	float *ev, float *est,
	float *norm, float *ov, float *ost)
{
	float dir[3], stdir[2], x;

	//calc direction
	dir[0]=ev[0]-sv[0];
	dir[1]=ev[1]-sv[1];
	dir[2]=ev[2]-sv[2];

	stdir[0]=est[0]-sst[0];
	stdir[1]=est[1]-sst[1];


	x=V3F_DOT(dir, norm);
	if(x==0)
	{
		printf("Hull_LinePlaneIntersectST: Parallel Clip\n");
		V3F_COPY(sv, ov);
		V2F_COPY(sst, ost);
		return(-1);
	}

	x=1.0/((x<0)?-x:x);
	dir[0]*=x;
	dir[1]*=x;
	dir[2]*=x;

	stdir[0]*=x;
	stdir[1]*=x;

	//calc intersection
	x=V3F_NDOT(sv, norm);

	x=(x<0)?-x:x;
	ov[0]=sv[0]+dir[0]*x;
	ov[1]=sv[1]+dir[1]*x;
	ov[2]=sv[2]+dir[2]*x;

	ost[0]=sst[0]+stdir[0]*x;
	ost[1]=sst[1]+stdir[1]*x;

	return(0);
}

BTGE_API void Hull_AdjacentNormals(float *norm, float *udir, float *vdir)
{
	float f;

	V3F_ZERO(udir);
	V3F_ZERO(vdir);

	//UxV=N

	if(fabs(norm[0])>fabs(norm[1]))
	{
		if(fabs(norm[0])>fabs(norm[2]))
		{
			udir[(norm[0]>0)?1:2]=1; 
			vdir[(norm[0]>0)?2:1]=1;
		}else
		{
			udir[(norm[2]>0)?0:1]=1; 
			vdir[(norm[2]>0)?1:0]=1;
		}
	}else
	{
		if(fabs(norm[1])>fabs(norm[2]))
		{
			udir[(norm[1]>0)?2:0]=1; 
			vdir[(norm[1]>0)?0:2]=1;
		}else
		{
			udir[(norm[2]>0)?0:1]=1; 
			vdir[(norm[2]>0)?1:0]=1;
		}
	}

	f=V3F_DOT(udir, norm);
	V3F_ADDSCALE(udir, norm, -f, udir);
	f=V3F_DOT(vdir, norm);
	V3F_ADDSCALE(vdir, norm, -f, vdir);

	V3F_NORMALIZE(udir, udir);
	V3F_NORMALIZE(vdir, vdir);
}

BTGE_API void Hull_AdjacentNormals2(float *norm, float *udir, float *vdir)
{
	float f;

	V3F_ZERO(udir);
	V3F_ZERO(vdir);

	//UxV=N

	if(fabs(norm[0])>fabs(norm[1]))
	{
		if(fabs(norm[0])>fabs(norm[2]))
		{
			udir[1]=(norm[0]>0)?1:-1; 
			vdir[2]=1;
		}else
		{
			udir[0]=(norm[2]>0)?1:-1; 
			vdir[1]=1;
		}
	}else
	{
		if(fabs(norm[1])>fabs(norm[2]))
		{
			udir[0]=(norm[1]>0)?-1:1;
			vdir[2]=1; 
		}else
		{
			udir[0]=(norm[2]>0)?1:-1; 
			vdir[1]=1;
		}
	}

	f=V3F_DOT(udir, norm);
	V3F_ADDSCALE(udir, norm, -f, udir);
	f=V3F_DOT(vdir, norm);
	V3F_ADDSCALE(vdir, norm, -f, vdir);

	V3F_NORMALIZE(udir, udir);
	V3F_NORMALIZE(vdir, vdir);
}

BTGE_API void Hull_AdjacentNormals21(float *norm, float *udir, float *vdir)
{
	float f;

	V3F_ZERO(udir);
	V3F_ZERO(vdir);

	//UxV=N

	if(fabs(norm[0])>fabs(norm[1]))
	{
		if(fabs(norm[0])>fabs(norm[2]))
		{
			udir[1]=1; 
			vdir[2]=1;
		}else
		{
			udir[0]=1; 
			vdir[1]=1;
		}
	}else
	{
		if(fabs(norm[1])>fabs(norm[2]))
		{
			udir[0]=1;
			vdir[2]=1; 
		}else
		{
			udir[0]=1; 
			vdir[1]=1;
		}
	}

//	f=V3F_DOT(udir, norm);
//	V3F_ADDSCALE(udir, norm, -f, udir);
//	f=V3F_DOT(vdir, norm);
//	V3F_ADDSCALE(vdir, norm, -f, vdir);

	V3F_NORMALIZE(udir, udir);
	V3F_NORMALIZE(vdir, vdir);
}

BTGE_API void Hull_AdjacentNormals22(float *norm, float *udir, float *vdir)
{
	float f, ax, ay, az;

	V3F_ZERO(udir);
	V3F_ZERO(vdir);

	//UxV=N

	ax=fabs(norm[0]); ay=fabs(norm[1]); az=fabs(norm[2]);

	if(ax>=ay && ax>=az) { udir[1]=1; vdir[2]=1; }
		else if(ay>=ax && ay>=az) { udir[0]=1; vdir[2]=1; }
		else { udir[0]=1; vdir[1]=1; }

	V3F_NORMALIZE(udir, udir);
	V3F_NORMALIZE(vdir, vdir);
}

BTGE_API void Hull_AdjacentNormals23(float *norm, float *udir, float *vdir)
{
	float tn[3];
	float f, ax, ay, az;

	V3F_ZERO(udir);
	V3F_ZERO(vdir);

	//UxV=N

	ax=fabs(norm[0]); ay=fabs(norm[1]); az=fabs(norm[2]);

	if(ax>=ay && ax>=az)
	{
		udir[1]=1; vdir[2]=1;
		if(norm[0]<0)
			udir[1]=-1;
	}else if(ay>=ax && ay>=az)
	{
		udir[0]=1; vdir[2]=1;
		if(norm[1]<0)
			udir[0]=-1;
	}else
	{
		udir[1]=1; vdir[0]=1;
	}

//	V3F_CROSS(udir, vdir, tn);
//	if(V3F_DOT(tn, norm)<0)
//		{ V3F_SCALE(udir, -1, udir); }

//	if(((ax*norm[0])+(ay*norm[1])+(az*norm[2]))<0)
//		{ V3F_SCALE(udir, -1, udir); }

	f=V3F_DOT(udir, norm);
	V3F_ADDSCALE(udir, norm, -f, udir);
	f=V3F_DOT(vdir, norm);
	V3F_ADDSCALE(vdir, norm, -f, vdir);

	V3F_NORMALIZE(udir, udir);
	V3F_NORMALIZE(vdir, vdir);
}

BTGE_API void Hull_MakePlaneFace(float *norm, float *pts)
{
	float v0[3], v1[3], v2[3];

	Hull_AdjacentNormals(norm, v0, v1);

	V3F_SCALEADDSCALE(v0, -99999, v1, -99999, v2);
	V3F_ADDSCALE(v2, norm, norm[3], pts+(0*3));

	V3F_SCALEADDSCALE(v0,  99999, v1, -99999, v2);
	V3F_ADDSCALE(v2, norm, norm[3], pts+(1*3));

	V3F_SCALEADDSCALE(v0,  99999, v1,  99999, v2);
	V3F_ADDSCALE(v2, norm, norm[3], pts+(2*3));

	V3F_SCALEADDSCALE(v0, -99999, v1,  99999, v2);
	V3F_ADDSCALE(v2, norm, norm[3], pts+(3*3));
}

BTGE_API int Hull_ClipFace(float *norm,
	float *ipts, float *opts, int num)
{
	int i, j, k, l;

	//first outside
	for(i=0; i<num; i++)if(V3F_NDOT(ipts+(i*3), norm)>0)break;

	if(i==num)	//nothing to clip
	{
		for(i=0; i<num*3; i++)opts[i]=ipts[i];
		return(num);	//nothing to clip
	}

	//first inside
	j=i;
	while(1)
	{
		if(V3F_NDOT(ipts+(j*3), norm)<=0)break;
		j=(j+1)%num;
		if(j==i)return(0);	//everything clipped
	}

	//copy inside
	i=j;
	l=0;
	while(1)
	{
		V3F_COPY(ipts+(j*3), opts+(l*3));
		l++;

		k=(j+1)%num;
		if(V3F_NDOT(ipts+(k*3), norm)>0)break;
		j=k;
	}

	//exit point
	if(V3F_NDOT(ipts+(j*3), norm)<0)
	{
		Hull_LinePlaneIntersect(
			ipts+(j*3), ipts+(k*3), norm, opts+(l*3));
		l++;
	}

	j=k;
	while(1)
	{
		k=(j+1)%num;
		if(V3F_NDOT(ipts+(k*3), norm)<=0)break;
		j=k;
	}

	//entry point
	if(V3D_NDOT(ipts+(k*3), norm)<0)
	{
		Hull_LinePlaneIntersect(
			ipts+(j*3), ipts+(k*3), norm, opts+(l*3));
		l++;
	}

	return(l);
}

BTGE_API int Hull_ClipFaceST(float *norm,
	float *ipts, float *ist, float *opts, float *ost, int num)
{
	float f, g;
	int i, j, k, l;

	//first outside
	for(i=0; i<num; i++)if(V3F_NDOT(ipts+(i*3), norm)>0)break;

	if(i==num)	//nothing to clip
	{
		for(i=0; i<num*3; i++)opts[i]=ipts[i];
		for(i=0; i<num*2; i++)ost[i]=ist[i];
		return(num);	//nothing to clip
	}

	//first inside
	j=i;
	while(1)
	{
		if(V3F_NDOT(ipts+(j*3), norm)<=0)break;
		j=(j+1)%num;
		if(j==i)return(0);	//everything clipped
	}

	//copy inside
	i=j;
	l=0;
	while(1)
	{
		V3F_COPY(ipts+(j*3), opts+(l*3));
		V2F_COPY(ist+(j*2), ost+(l*2));
		l++;

		k=(j+1)%num;
		if(V3F_NDOT(ipts+(k*3), norm)>0)break;
		j=k;
	}

	//exit point
	f=V3F_NDOT(ipts+(j*3), norm);
//	g=V3F_NDOT(ipts+(k*3), norm);
//	if((f<0) && (g>0))
	if(f<0)
	{
		Hull_LinePlaneIntersectST(
			ipts+(j*3), ist+(j*2),
			ipts+(k*3), ist+(k*2),
			norm, opts+(l*3), ost+(l*2));
		l++;
	}
#if 0
	else if(g==0)
	{
		V3F_COPY(ipts+(k*3), opts+(l*3));
		V2F_COPY(ist+(k*2), ost+(l*2));
		l++;
		
//		k=(k+1)%num;
	}
#endif

	j=k;
	while(1)
	{
		k=(j+1)%num;
		if(V3F_NDOT(ipts+(k*3), norm)<=0)break;
		j=k;
	}

	//entry point
	f=V3D_NDOT(ipts+(k*3), norm);
	if(f<0)
	{
		Hull_LinePlaneIntersectST(
			ipts+(j*3), ist+(j*2),
			ipts+(k*3), ist+(k*2),
			norm, opts+(l*3), ost+(l*2));
		l++;
	}
#if 0
	else if(f==0)
	{
		V3F_COPY(ipts+(k*3), opts+(l*3));
		V2F_COPY(ist+(k*2), ost+(l*2));
		l++;
	}
#endif
	return(l);
}

BTGE_API void Hull_BoxPlaneExtents(float *mins, float *maxs, float *norm,
	float *rm, float *rn)
{
	float d0, d1;
	int i;

	d0=0; d1=0;
	for(i=0; i<3; i++)
	{
		if(norm[i]>0)
		{
			d0+=maxs[i]*norm[i];
			d1+=mins[i]*norm[i];
		}else
		{
			d0+=mins[i]*norm[i];
			d1+=maxs[i]*norm[i];
		}
	}
	*rm=d1-norm[3]; *rn=d0-norm[3];
}

BTGE_API void Hull_SpherePlaneExtents(float *org, float rad, float *norm,
	float *rm, float *rn)
{
	float f;
	
	f=V3F_NDOT(org, norm);
	*rm=f-rad; *rn=f+rad;
}

int BTGE_Brush_CombineTextureFlags(int fl0, int fl1)
{
	int flags, mat;
	int i, j;

	flags=
		(fl0&(~BTGE_TXFL_MATERIAL_MASK))|
		(fl1&(~BTGE_TXFL_MATERIAL_MASK));

	i=fl0&BTGE_TXFL_MATERIAL_MASK;
	j=fl1&BTGE_TXFL_MATERIAL_MASK;

//	if(i==BTGE_TXFL_EFFECT_ORIGIN)returm(fl1);
//	if(j==BTGE_TXFL_EFFECT_ORIGIN)returm(fl0);
	
	if(i)
	{
		if(!j || (i==j))
		{
			mat=i;
		}else
		{
			mat=BTGE_TXFL_MATERIAL_SOLID;
		}
	}else mat=j;
	
	return(flags|mat);
}

int BTGE_Brush_BuildFace(BTGE_Brush *brush, int id,
	float *xyz, float *st, int *rtn)
{
	static float pts0[16*3];
	static float pts1[16*3];
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

	if(!l)return(0);

	sprintf(buf, "textures/%s", brush->tex[id]);
	tn=BTGE_Texture_LoadImageSize(buf, &xs, &ys);
	*rtn=tn;

	i=BTGE_Texture_GetImageFlags(tn);
	if(i&BTGE_TXFL_ALPHA)brush->flags|=BTGE_BRFL_ALPHA;
	if(i&BTGE_TXFL_VALPHA)brush->flags|=BTGE_BRFL_ALPHA;
	if(i&BTGE_TXFL_WINDOW)brush->flags|=BTGE_BRFL_ALPHA;
	if(i&BTGE_TXFL_NODRAW)brush->flags|=BTGE_BRFL_ALPHA|BTGE_BRFL_NODRAW;

	if(i&BTGE_TXFL_NOSHADOW)brush->flags|=BTGE_BRFL_NOSHADOW;
	if(i&BTGE_TXFL_NONSOLID)brush->flags|=BTGE_BRFL_NONSOLID;

	brush->flags=BTGE_Brush_CombineTextureFlags(brush->flags,
		i&BTGE_TXFL_MATERIAL_MASK);

#if 0
	if(i&BTGE_TXFL_FLUID)
	{
		brush->flags|=BTGE_BRFL_ALPHA|BTGE_BRFL_FLUID;
		brush->flags|=i&BTGE_TXFL_FLUID_MASK;
	}

	if(i&BTGE_TXFL_EFFECT)
	{
		brush->flags|=i&BTGE_TXFL_EFFECT_MASK;
	}
#endif

	if(tn<0)
	{
		xs=256;
		ys=256;
	}

	sx=BTGE_Texture_GetImageScaleX(tn);
	sy=BTGE_Texture_GetImageScaleY(tn);

	for(i=0; i<l; i++)
	{
		V3F_COPY(pts0+i*3, xyz+i*3);
		st[i*2+0]=V3F_NDOT(pts0+i*3, brush->sdir+id*4)/(xs*sx);
		st[i*2+1]=V3F_NDOT(pts0+i*3, brush->tdir+id*4)/(ys*sy);
	}
	return(l);
}

BTGE_API void BTGE_Brush_BuildFaces(BTGE_Brush *brush)
{
	static float xyz[256*3];
	static float st[256*3];
	static float xyz2[256*3];
	static float st2[256*3];

	static int iv[64];
	static int nv[64];
	static int nt[64];
	static int np[64];
	static int ev[64*4];
	float org[3], tv0[3];
	double f, g;
	int nxyz, nst, npoly, nvec, nev;
	int i, j, k, l, i0, i1;

	if(brush->patch)
	{
		BTGE_BrushPatch_BuildFaces(brush);
		return;
	}

	if(brush->mesh)
	{
		BTGE_BrushMesh_BuildFaces(brush);
		return;
	}


	if(brush->n_face<=0)
		return;

	brush->flags&=~(BTGE_BRFL_ALPHA|BTGE_TXFL_FLUID_MASK|
		BTGE_TXFL_EFFECT_MASK|BTGE_BRFL_NOSHADOW);

	nvec=0; npoly=0;
	for(i=0; i<brush->n_face; i++)
	{
		np[npoly]=i;
		nv[npoly]=BTGE_Brush_BuildFace(brush, i,
			xyz+nvec*3, st+nvec*2, &nt[npoly]);
		if(!nv[npoly])continue;
		iv[npoly]=nvec;
		nvec+=nv[npoly];
		npoly++;
	}
	brush->n_vec=nvec;
	brush->n_poly=npoly;

//	brush->plnum=gcatomic(brush->n_poly*sizeof(int));
//	brush->texnum=gcatomic(brush->n_poly*sizeof(int));
//	brush->numvec=gcatomic(brush->n_poly*sizeof(int));
//	brush->xyz=gcatomic(brush->n_vec*3*sizeof(float));
//	brush->st=gcatomic(brush->n_vec*2*sizeof(float));

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

		j=BTGE_Texture_GetImageFlags(nt[i]);
		brush->pf_flag[i]=j;
	}

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

	for(i=0; i<brush->n_xyz; i++) { V3F_COPY(xyz2+i*3, brush->xyz+i*3); }
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


	brush->mins[0]=999999; brush->mins[1]=999999; brush->mins[2]=999999;
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
}


BTGE_API void BTGE_Brush_DestroyFaces(BTGE_Brush *brush)
{
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
//	if(brush->rgb)gcfree(brush->rgb);
//	brush->dlrgb=NULL;
//	brush->srgb=NULL;
//	brush->rgb=NULL;
}

BTGE_API void BTGE_Brush_DestroyBrush(BTGE_Brush *brush)
{
	if(!brush)return;

	BTGE_Brush_DestroyFaces(brush);
	
	if(brush->norm)gcfree(brush->norm);
	if(brush->sdir)gcfree(brush->sdir);
	if(brush->tdir)gcfree(brush->tdir);
	if(brush->tex)gcfree(brush->tex);
	gcfree(brush);
}

BTGE_API int BTGE_Brush_CheckPointBrush(BTGE_Brush *brush, float *pt)
{
	float tv0[3], tv1[3], tv2[3];
	float f, g;
	int i;

	if(brush->patch)
		{ return(BTGE_BrushPatch_CheckPointBrush(brush, pt)); }

	if(brush->mesh)
		{ return(BTGE_BrushMesh_CheckPointBrush(brush, pt)); }

	if(brush->xyz)
		{ f=V3F_DIST(pt, brush->org); if(f>=brush->rad)return(0); }
	for(i=0; i<brush->n_face; i++)
		{ f=V3F_NDOT(pt, brush->norm+i*4); if(f>0)return(0); }
	return(1);
}

BTGE_API int BTGE_Brush_CheckLineBrushClip(BTGE_Brush *brush,
	float *sv, float *ev, float *csv, float *cev)
{
	float tv0[3], tv1[3], tv2[3];
	float f, g;
	int i;

	if(brush->patch)
	{
		return(BTGE_BrushPatch_CheckLineBrushClip(
			brush, sv, ev, csv, cev));
	}

	if(brush->mesh)
	{
		return(BTGE_BrushMesh_CheckLineBrushClip(
			brush, sv, ev, csv, cev));
	}

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

BTGE_API int BTGE_Brush_CheckLineBrush(BTGE_Brush *brush,
	float *sv, float *ev)
{
	if(brush->mesh)
		{ return(BTGE_BrushMesh_CheckLineBrush(brush, sv, ev)); }

	return(BTGE_Brush_CheckLineBrushClip(brush, sv, ev, NULL, NULL));
}

BTGE_API int BTGE_Brush_CheckBoxBrush(BTGE_Brush *brush,
	float *mins, float *maxs)
{
	float m, n;
	int i;

	if(brush->patch)
		{ return(BTGE_BrushPatch_CheckBoxBrush(brush, mins, maxs)); }

	if(brush->mesh)
		{ return(BTGE_BrushMesh_CheckBoxBrush(brush, mins, maxs)); }

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

BTGE_API int BTGE_Brush_CheckSphereBrush(BTGE_Brush *brush,
	float *org, float rad)
{
	float dir[4];
	float *fv;
	float f, g;
	int i;

	if(brush->patch)
		{ return(BTGE_BrushPatch_CheckSphereBrush(brush, org, rad)); }

	if(brush->mesh)
		{ return(BTGE_BrushMesh_CheckSphereBrush(brush, org, rad)); }

	if(!brush->xyz)
		{ BTGE_Brush_BuildFaces(brush); }

	for(i=0; i<brush->n_face; i++)
	{
		fv=brush->norm+i*4;
		f=V3F_NDOT(org, fv);
		if(f>=rad)return(0);
	}
	
#if 1
	V3F_SUB(brush->org, org, dir);
	f=V3F_NORMALIZE(dir, dir);
//	g=V3F_FRNORMALIZE(dir, dir);
//	f=(g>0)?(1.0/g):0;
	if(f>(brush->rad+rad)) return(0);

#if 1
	dir[3]=V3F_DOT(org, dir);
	BTGE_Brush_GetPlaneExtents(brush, dir, &f, &g);
//	BTGE_Brush_GetBBoxPlaneExtents(cur, dir, &f, &g);
	if(f>=rad) return(0);
#endif
#endif

	return(1);
}

/** Get the extents of a brush along an axis */
BTGE_API int BTGE_Brush_GetExtents(BTGE_Brush *brush, float *vec,
	float *rm, float *rn)
{
	float tv0[3], tv1[3], tv2[3];
	float f, g, m, n;
	int i;

	if(brush->patch)
		{ return(BTGE_BrushPatch_GetExtents(brush, vec, rm, rn)); }

	if(brush->mesh)
		{ return(BTGE_BrushMesh_GetExtents(brush, vec, rm, rn)); }

	m=999999; n=-999999;
	for(i=0; i<brush->n_xyz; i++)
	{
		f=V3F_DOT(brush->xyz+i*3, vec);
		if(f<m)m=f; if(f>n)n=f;
	}

	if(rm)*rm=m; if(rn)*rn=n;
	return(0);
}

/** Get the extents of a brush relative to a plane */
BTGE_API int BTGE_Brush_GetPlaneExtents(BTGE_Brush *brush, float *norm,
	float *rm, float *rn)
{
	float tv0[3], tv1[3], tv2[3];
	float f, g, m, n;
	int i;

	if(brush->patch)
		{ return(BTGE_BrushPatch_GetPlaneExtents(brush, norm, rm, rn)); }

	if(brush->mesh)
		{ return(BTGE_BrushMesh_GetPlaneExtents(brush, norm, rm, rn)); }

	m=999999; n=-999999;
	for(i=0; i<brush->n_xyz; i++)
	{
		f=V3F_NDOT(brush->xyz+i*3, norm);
		if(f<m)m=f; if(f>n)n=f;
	}
	if(rm)*rm=m; if(rn)*rn=n;
	return(0);
}

/** Get the bbox for a brush */
BTGE_API void BTGE_Brush_GetBBoxExtents(BTGE_Brush *brush, float *vec,
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

/** Get the extents of a brush's BBox along relative to a plane */
BTGE_API void BTGE_Brush_GetBBoxPlaneExtents(BTGE_Brush *brush,
	float *norm, float *rm, float *rn)
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

/** Get the extents of a brush's BBox along relative to a plane */
BTGE_API void BTGE_Brush_GetBrushFacePlaneExtents(
	BTGE_Brush *brush, int fn,
	float *norm, float *rm, float *rn)
{
	float tv0[3], tv1[3], tv2[3];
	float f, g, m, n;
	int i, j, k;

//	if(brush->patch)
//		{ return(BTGE_BrushPatch_GetPlaneExtents(brush, norm, rm, rn)); }
//	if(brush->mesh)
//		{ return(BTGE_BrushMesh_GetPlaneExtents(brush, norm, rm, rn)); }

	m=999999; n=-999999;
	for(i=0; i<brush->pf_nvec[fn]; i++)
	{
		j=brush->pf_ivec[fn];
		k=brush->vec[(j+i)*4+0];
		f=V3F_NDOT(brush->xyz+k*3, norm);
		if(f<m)m=f; if(f>n)n=f;
	}
	if(rm)*rm=m; if(rn)*rn=n;
//	return(0);
}

BTGE_API BTGE_Brush *BTGE_Brush_New(int num)
{
	BTGE_Brush *tmp;

	BTGE_Brush_Init();

	tmp=gctalloc("btge_brush_t", sizeof(BTGE_Brush));
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

BTGE_API BTGE_Brush *BTGE_Brush_MakeCubeExtentsTexPos(
	float mx, float my, float mz,
	float nx, float ny, float nz, char *tex,
	float tx, float ty, float tz, int tp)
{
	float torg[3];
	BTGE_Brush *tmp;
	int i;

	tmp=BTGE_Brush_New(6);

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

//	for(i=0; i<6; i++)
//		Hull_AdjacentNormals2(tmp->norm+i*4,
//			tmp->sdir+i*4, tmp->tdir+i*4);

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

BTGE_API BTGE_Brush *BTGE_Brush_MakeCubeExtents(
	float mx, float my, float mz,
	float nx, float ny, float nz, char *tex)
{
	BTGE_Brush *tmp;

	tmp=BTGE_Brush_MakeCubeExtentsTexPos(
		mx, my, mz, nx, ny, nz, tex, 0, 0, 0, 0);
	return(tmp);
}

BTGE_API BTGE_Brush *BTGE_Brush_MakeCube(float x, float y, float z,
	float xs, float ys, float zs, char *tex)
{
	BTGE_Brush *tmp;

	tmp=BTGE_Brush_MakeCubeExtents(
		x-xs, y-ys, z-zs, x+xs, y+ys, z+zs, tex);
	return(tmp);
}

BTGE_API void BTGE_Brush_CheckExpandFaces(BTGE_Brush *brush, int num)
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

BTGE_API BTGE_Brush *BTGE_Brush_CloneBrush(BTGE_Brush *brush)
{
	BTGE_Brush *tmp;
	int n;

	BTGE_Brush_Init();

	if(brush->mesh)
	{
		tmp=gctalloc("lbxgl_brush_t", sizeof(BTGE_Brush));
		tmp->mesh=BTGE_Mesh_ClonePrim(brush->mesh);
		return(tmp);
	}

	tmp=gctalloc("lbxgl_brush_t", sizeof(BTGE_Brush));
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

#if 0
BTGE_API void BTGE_Brush_DestroyBrush(BTGE_Brush *brush)
{
	BTGE_Brush_DestroyFaces(brush);
	
	gcfree(brush->norm);
	gcfree(brush->sdir);
	gcfree(brush->tdir);
	gcfree(brush->norg);
	gcfree(brush->tex);
	gcfree(brush);
}
#endif

BTGE_API BTGE_Brush *BTGE_Brush_CloneBrushList(BTGE_Brush *brush)
{
	BTGE_Brush *tmp;
	
	if(!brush)return(NULL);
	
	tmp=BTGE_Brush_CloneBrush(brush);
	tmp->next=BTGE_Brush_CloneBrushList(brush->next);
	return(tmp);
}

BTGE_API void BTGE_Brush_DestroyBrushList(BTGE_Brush *brush)
{
	BTGE_Brush *tmp;
	
	if(!brush)return;
	BTGE_Brush_DestroyBrushList(brush->next);
	BTGE_Brush_DestroyBrush(brush);
}

BTGE_API BTGE_Brush *BTGE_Brush_CloneBrushEntList(BTGE_Brush *brush)
{
	BTGE_Brush *tmp;
	
	if(!brush)return(NULL);
	
	tmp=BTGE_Brush_CloneBrush(brush);
	tmp->enext=BTGE_Brush_CloneBrushEntList(brush->enext);
	return(tmp);
}

BTGE_API BTGE_Brush *BTGE_Brush_CloneBrushChainList(BTGE_Brush *brush)
{
	BTGE_Brush *tmp;
	
	if(!brush)return(NULL);
	
	tmp=BTGE_Brush_CloneBrush(brush);
	tmp->chain=BTGE_Brush_CloneBrushChainList(brush->chain);
	return(tmp);
}

BTGE_API void BTGE_Brush_DestroyBrushEntList(BTGE_Brush *brush)
{
	BTGE_Brush *tmp;
	
	if(!brush)return;
	BTGE_Brush_DestroyBrushEntList(brush->enext);
	BTGE_Brush_DestroyBrush(brush);
}

BTGE_API void BTGE_Brush_TranslateBrush(BTGE_Brush *brush, float *offs)
{
	int i;
	
	for(i=0; i<brush->n_face; i++)
		{ brush->norm[i*4+3]+=V3F_DOT(offs, brush->norm+i*4); }
}

BTGE_API void BTGE_Brush_TranslateBrushChainList(
	BTGE_Brush *brush, float *offs)
{
	BTGE_Brush *tmp;
	
	if(!brush)return;
	BTGE_Brush_TranslateBrushChainList(brush->chain, offs);
	BTGE_Brush_TranslateBrush(brush, offs);
}

BTGE_API void BTGE_Brush_Init()
{
	static int init=0;

	if(init)return;
	init=1;
}
