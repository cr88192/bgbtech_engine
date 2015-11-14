#include <lbxgl2.h>

int Hull_LinePlaneIntersect(float *start, float *end,
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

int Hull_LinePlaneIntersectST(float *sv, float *sst,
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
	if(x==0)return(-1);

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

void Hull_AdjacentNormals(float *norm, float *udir, float *vdir)
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

void Hull_AdjacentNormals2(float *norm, float *udir, float *vdir)
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

void Hull_AdjacentNormals21(float *norm, float *udir, float *vdir)
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

void Hull_MakePlaneFace(float *norm, float *pts)
{
	float v0[3], v1[3], v2[3];

	Hull_AdjacentNormals(norm, v0, v1);

	V3F_SCALEADDSCALE(v0, -999999, v1, -999999, v2);
	V3F_ADDSCALE(v2, norm, norm[3], pts+(0*3));

	V3F_SCALEADDSCALE(v0,  999999, v1, -999999, v2);
	V3F_ADDSCALE(v2, norm, norm[3], pts+(1*3));

	V3F_SCALEADDSCALE(v0,  999999, v1,  999999, v2);
	V3F_ADDSCALE(v2, norm, norm[3], pts+(2*3));

	V3F_SCALEADDSCALE(v0, -999999, v1,  999999, v2);
	V3F_ADDSCALE(v2, norm, norm[3], pts+(3*3));
}

int Hull_ClipFace(float *norm,
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

int Hull_ClipFaceST(float *norm,
	float *ipts, float *ist, float *opts, float *ost, int num)
{
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
		l++;

		k=(j+1)%num;
		if(V3F_NDOT(ipts+(k*3), norm)>0)break;
		j=k;
	}

	//exit point
	if(V3F_NDOT(ipts+(j*3), norm)<0)
	{
		Hull_LinePlaneIntersectST(
			ipts+(j*3), ist+(j*2),
			ipts+(k*3), ist+(k*2),
			norm, opts+(l*3), ost+(l*2));
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
		Hull_LinePlaneIntersectST(
			ipts+(j*3), ist+(j*2),
			ipts+(k*3), ist+(k*2),
			norm, opts+(l*3), ost+(l*2));
		l++;
	}

	return(l);
}


int LBXGL_Brush_BuildFace(LBXGL_Brush *brush, int id,
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
		for(j=0; j<(l*3); j++)pts0[j]=pts1[j];
	}

	if(!l)return(0);

	sprintf(buf, "textures/%s", brush->tex[id]);
//	tn=Tex_LoadFile(buf, &xs, &ys);
	tn=LBXGL_Texture_LoadImageSize(buf, &xs, &ys);
	*rtn=tn;

	i=LBXGL_Texture_GetImageFlags(tn);
	if(i&LBXGL_TXFL_ALPHA)brush->flags|=LBXGL_BRFL_ALPHA;
	if(i&LBXGL_TXFL_VALPHA)brush->flags|=LBXGL_BRFL_ALPHA;
	if(i&LBXGL_TXFL_WINDOW)brush->flags|=LBXGL_BRFL_ALPHA;
	if(i&LBXGL_TXFL_NODRAW)brush->flags|=LBXGL_BRFL_ALPHA;

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

void LBXGL_Brush_BuildFaces(LBXGL_Brush *brush)
{
	static float xyz[256*3];
	static float st[256*3];
	static int nv[64];
	static int nt[64];
	static int np[64];
	float org[3], tv0[3];
	double f, g;
	int i, j, k;

	j=0; k=0;
	for(i=0; i<brush->n_face; i++)
	{
		np[k]=i;
		nv[k]=LBXGL_Brush_BuildFace(brush, i,
			xyz+j*3, st+j*2, &nt[k]);
		if(!nv[k])continue;
		j+=nv[k];
		k++;
	}
	brush->n_vec=j;
	brush->n_poly=k;

	brush->plnum=gcatomic(brush->n_poly*sizeof(int));
	brush->texnum=gcatomic(brush->n_poly*sizeof(int));
	brush->numvec=gcatomic(brush->n_poly*sizeof(int));
	brush->xyz=gcatomic(brush->n_vec*3*sizeof(float));
	brush->st=gcatomic(brush->n_vec*2*sizeof(float));

	for(i=0; i<brush->n_poly; i++)
	{
		brush->plnum[i]=np[i];
		brush->texnum[i]=nt[i];
		brush->numvec[i]=nv[i];
	}

	for(i=0; i<brush->n_vec; i++)
	{
		V3F_COPY(xyz+i*3, brush->xyz+i*3);
		V3F_COPY(st+i*2, brush->st+i*2);
	}


//	return;

	if(!brush->norg)brush->norg=gcatomic(brush->n_face*3*sizeof(float));

	for(i=0; i<brush->n_face; i++)
		{ V3F_ZERO(brush->norg+i*3); }

	k=0;
	for(i=0; i<brush->n_poly; i++)
	{
		V3F_ZERO(org);
		for(j=0; j<brush->numvec[i]; j++)
		{
			V3F_ADD(org, brush->xyz+k*3, org);
			k++;
		}
		V3F_SCALE(org, 1.0/brush->numvec[i], org);
		V3F_COPY(org, brush->norg+np[i]*3);
	}

	V3F_ZERO(org);
	for(i=0; i<brush->n_face; i++)
		{ V3F_ADD(org, brush->norg+i*3, org); }
	V3F_SCALE(org, 1.0/brush->n_face, brush->org);

	g=0;
	for(i=0; i<brush->n_vec; i++)
	{
		V3F_SUB(xyz+i*3, brush->org, tv0);
		f=V3F_LEN(tv0);
		if(f>g)g=f;
	}

	brush->rad=g;


	brush->mins[0]=999999; brush->mins[1]=999999; brush->mins[2]=999999;
	brush->maxs[0]=-999999; brush->maxs[1]=-999999; brush->maxs[2]=-999999;
	for(i=0; i<brush->n_vec; i++)
	{
		if(xyz[i*3+0]<brush->mins[0])brush->mins[0]=xyz[i*3+0];
		if(xyz[i*3+1]<brush->mins[1])brush->mins[1]=xyz[i*3+1];
		if(xyz[i*3+2]<brush->mins[2])brush->mins[2]=xyz[i*3+2];

		if(xyz[i*3+0]>brush->maxs[0])brush->maxs[0]=xyz[i*3+0];
		if(xyz[i*3+1]>brush->maxs[1])brush->maxs[1]=xyz[i*3+1];
		if(xyz[i*3+2]>brush->maxs[2])brush->maxs[2]=xyz[i*3+2];
	}
}


void LBXGL_Brush_DestroyFaces(LBXGL_Brush *brush)
{
	if(brush->plnum)gcfree(brush->plnum);
	if(brush->texnum)gcfree(brush->texnum);
	if(brush->numvec)gcfree(brush->numvec);
	if(brush->xyz)gcfree(brush->xyz);
	if(brush->st)gcfree(brush->st);

	brush->plnum=NULL;
	brush->texnum=NULL;
	brush->numvec=NULL;
	brush->xyz=NULL;
	brush->st=NULL;

	brush->n_vec=0;
	brush->n_poly=0;

	if(brush->dlrgb)gcfree(brush->dlrgb);
	if(brush->srgb)gcfree(brush->srgb);
	if(brush->rgb)gcfree(brush->rgb);
	brush->dlrgb=NULL;
	brush->srgb=NULL;
	brush->rgb=NULL;
}

int LBXGL_Brush_CheckLineBrush(LBXGL_Brush *brush, float *sv, float *ev)
{
	float tv0[3], tv1[3], tv2[3];
	float f, g;
	int i;

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

	return(1);
}

int LBXGL_Brush_GetExtents(LBXGL_Brush *brush, float *vec,
	float *rm, float *rn)
{
	float tv0[3], tv1[3], tv2[3];
	float f, g, m, n;
	int i;

	m=999999999; n=-999999999;
	for(i=0; i<brush->n_vec; i++)
	{
		f=V3F_DOT(brush->xyz+i*3, vec);
		if(f<m)m=f; if(f>n)n=f;
	}

	if(rm)*rm=m; if(rn)*rn=n;
	return(0);
}

int LBXGL_Brush_GetPlaneExtents(LBXGL_Brush *brush, float *norm,
	float *rm, float *rn)
{
	float tv0[3], tv1[3], tv2[3];
	float f, g, m, n;
	int i;

	m=999999999; n=-999999999;
	for(i=0; i<brush->n_vec; i++)
	{
		f=V3F_NDOT(brush->xyz+i*3, norm);
		if(f<m)m=f; if(f>n)n=f;
	}
	if(rm)*rm=m; if(rn)*rn=n;
	return(0);
}


void LBXGL_Brush_GetBBoxExtents(LBXGL_Brush *brush, float *vec,
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

void LBXGL_Brush_GetBBoxPlaneExtents(LBXGL_Brush *brush, float *norm,
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

LBXGL_Brush *LBXGL_Brush_New(int num)
{
	LBXGL_Brush *tmp;

	LBXGL_Brush_Init();

	tmp=gctalloc("lbxgl_brush_t", sizeof(LBXGL_Brush));
	tmp->n_face=num;

	tmp->norm=gcatomic(num*4*sizeof(float));
	tmp->sdir=gcatomic(num*4*sizeof(float));
	tmp->tdir=gcatomic(num*4*sizeof(float));
	tmp->norg=gcatomic(num*3*sizeof(float));
	tmp->tex=gcalloc(num*sizeof(char *));

	return(tmp);
}

LBXGL_Brush *LBXGL_Brush_MakeCubeExtents(float mx, float my, float mz,
	float nx, float ny, float nz, char *tex)
{
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

	for(i=0; i<6; i++)
		Hull_AdjacentNormals2(tmp->norm+i*4,
			tmp->sdir+i*4, tmp->tdir+i*4);

	return(tmp);
}

LBXGL_Brush *LBXGL_Brush_MakeCube(float x, float y, float z,
	float xs, float ys, float zs, char *tex)
{
	LBXGL_Brush *tmp;

	tmp=LBXGL_Brush_MakeCubeExtents(
		x-xs, y-ys, z-zs, x+xs, y+ys, z+zs, tex);
	return(tmp);
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
	LBXGL_Brush_DrawShadowVolume((LBXGL_Brush *)obj, org);
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

void LBXGL_Brush_Init()
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
