struct bsp_face_s {
struct bsp_face_s *next;
float norm[4], sdir[4], tdir[4];
int nvecs;
float *vecs;
float *evecs;
float *st;
float *uv;
int num;

struct bsp_lm_s *lm;
struct bsp_face_s *lmnext;
};

struct bsp_node_s {
struct bsp_node_s *lnode, *rnode;
struct bsp_face_s *face;
struct main_entity_s *ents;	//linked entities
struct bsp_lm_s *lm;
float norm[4];
int num;
};

struct bsp_lm_s {
struct bsp_lm_s *next;
struct bsp_face_s *face;

int xs, ys, texnum;
byte *lmbuf;	//lightmap data
byte *txbuf;	//crude texture

float norm[4];
float udir[3], vdir[3];		//uv vectors
float min[2], max[2];		//uv extents
float usz, vsz;
};


char **pdgl_main_texnames;
int *pdgl_main_tex_xs;
int *pdgl_main_tex_ys;
int *pdgl_main_tex_num;
int *pdgl_main_tex_num2;
int *pdgl_main_tex_flags;
byte **pdgl_main_tex_buf;

int pdgl_main_bsphash=0;
int pdgl_main_faceid=0;

int Vec3F_LinePlaneIntersect(float *start, float *end,
	float *norm, float *point)
{
	float dir[16], x;

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

void pdgl_main_hashline(char *s)
{
	int l, h;

	l=pdgl_main_bsphash&0xFFFF;
	h=(pdgl_main_bsphash>>16)&0xFFFF;

	while(*s)
	{
		h=(h+l)%65521;
		l=(l+*s++)%65521;
	}
	pdgl_main_bsphash=(h<<16)|l;
}

void pdgl_main_skipface(FILE *fd, char **ap)
{
	char buf[256];
	int n;

	n=atoi(ap[1]);
	while(n--)fgets(buf, 255, fd);
}

struct bsp_face_s *pdgl_main_readface(FILE *fd, char **ap)
{
	struct bsp_face_s *tmp;
	char buf[256], **a;
	int i, j, k, n;

	float pt[3], dv[3], dn[4];
	float f, g;

	tmp=malloc(sizeof(struct bsp_face_s));
	memset(tmp, 0, sizeof(struct bsp_face_s));
	tmp->norm[0]=atof(ap[3]);
	tmp->norm[1]=atof(ap[4]);
	tmp->norm[2]=atof(ap[5]);
	tmp->norm[3]=atof(ap[6])/36.0;

	tmp->sdir[0]=atof(ap[9]);
	tmp->sdir[1]=atof(ap[10]);
	tmp->sdir[2]=atof(ap[11]);
	tmp->sdir[3]=atof(ap[12])/36.0;

	tmp->tdir[0]=atof(ap[15]);
	tmp->tdir[1]=atof(ap[16]);
	tmp->tdir[2]=atof(ap[17]);
	tmp->tdir[3]=atof(ap[18])/36.0;

	i=atoi(ap[20]);
	tmp->num=i;

	n=atoi(ap[1]);
	tmp->nvecs=n;
	tmp->vecs=malloc(n*3*sizeof(float));
	tmp->evecs=malloc(n*4*sizeof(float));
	tmp->st=malloc(n*2*sizeof(float));
	tmp->uv=malloc(n*2*sizeof(float));

	j=0;
	for(i=0; i<n; i++)
	{
		fgets(buf, 255, fd);
		pdgl_main_hashline(buf);
		a=ksplit(buf);

		tmp->vecs[j*3+0]=atof(a[0])/36.0;
		tmp->vecs[j*3+1]=atof(a[1])/36.0;
		tmp->vecs[j*3+2]=atof(a[2])/36.0;
		tmp->st[j*2+0]=atof(a[3]);
		tmp->st[j*2+1]=atof(a[4]);

		//eliminate collinear points
		for(k=0; k<j; k++)
			if(V3F_DIST(tmp->vecs+k*3, tmp->vecs+j*3)<0.0001)
				break;
		if(k<j)continue;
		j++;
	}
	tmp->nvecs=j;

	for(i=0; i<tmp->nvecs; i++)
	{
		j=(i+1)%tmp->nvecs;
		V3F_SUB(tmp->vecs+j*3, tmp->vecs+i*3, dv);
//		V3F_CROSS(dv, tmp->norm, dn);
		V3F_CROSS(tmp->norm, dv, dn);
		V3F_NORMALIZE(dn, dn);
		dn[3]=V3F_DOT(tmp->vecs+i*3, dn);

		V4F_COPY(dn, tmp->evecs+i*4);
	}

	return(tmp);
}

struct bsp_face_s *pdgl_main_readleafface(FILE *fd, char **ap)
{
	struct bsp_face_s *tmp;
	char buf[256], **a;
	int i, n;

	tmp=malloc(sizeof(struct bsp_face_s));
	memset(tmp, 0, sizeof(struct bsp_face_s));
	tmp->norm[0]=atof(ap[3]);
	tmp->norm[1]=atof(ap[4]);
	tmp->norm[2]=atof(ap[5]);
	tmp->norm[3]=atof(ap[6])/36.0;

	n=atoi(ap[1]);
	tmp->nvecs=n;
	tmp->vecs=malloc(n*3*sizeof(float));

	for(i=0; i<n; i++)
	{
		fgets(buf, 255, fd);
		pdgl_main_hashline(buf);
		a=ksplit(buf);

		tmp->vecs[i*3+0]=atof(a[0])/36.0;
		tmp->vecs[i*3+1]=atof(a[1])/36.0;
		tmp->vecs[i*3+2]=atof(a[2])/36.0;
	}

	return(tmp);
}

struct bsp_node_s *pdgl_main_readleaf(FILE *fd, char **ap)
{
	struct bsp_node_s *tmp, *tn;
	struct bsp_face_s *tf;
	char buf[256];
	char **a;

	tmp=malloc(sizeof(struct bsp_node_s));
	memset(tmp, 0, sizeof(struct bsp_node_s));
	tmp->num=main_leafnum++;

	while(!feof(fd))
	{
		memset(buf, 0, 256);
		fgets(buf, 255, fd);
		pdgl_main_hashline(buf);

		a=ksplit(buf);
		if(!a[0])continue;

		if(!strcmp(a[0], "end"))break;
		if(!strcmp(a[0], "leaf-face"))
		{
			tf=pdgl_main_readleafface(fd, a);
			tf->next=tmp->face;
			tmp->face=tf;
			continue;
		}
	}

	return(tmp);
}

struct bsp_node_s *pdgl_main_readnode(FILE *fd, char **ap)
{
	struct bsp_node_s *tmp, *tn;
	struct bsp_face_s *tf;
	char buf[256];
	char **a;

	tmp=malloc(sizeof(struct bsp_node_s));
	memset(tmp, 0, sizeof(struct bsp_node_s));
	tmp->norm[0]=atof(ap[2]);
	tmp->norm[1]=atof(ap[3]);
	tmp->norm[2]=atof(ap[4]);
	tmp->norm[3]=atof(ap[5])/36.0;
	tmp->num=main_nodenum++;

	while(!feof(fd))
	{
		memset(buf, 0, 256);
		fgets(buf, 255, fd);
		pdgl_main_hashline(buf);

		a=ksplit(buf);
		if(!a[0])continue;

		if(!strcmp(a[0], "end"))break;
		if(!strcmp(a[0], "face"))
		{
			tf=pdgl_main_readface(fd, a);
			tf->next=tmp->face;
			tmp->face=tf;
			continue;
		}

		if(!strcmp(a[0], "node"))
		{
			tn=pdgl_main_readnode(fd, a);
			if(!tmp->lnode)tmp->lnode=tn;
			else if(!tmp->rnode)tmp->rnode=tn;
			else printf("node problem: too many children\n");
			continue;
		}
		if(!strcmp(a[0], "leaf"))
		{
			tn=pdgl_main_readleaf(fd, a);
			if(!tmp->lnode)tmp->lnode=tn;
			else if(!tmp->rnode)tmp->rnode=tn;
			else printf("node problem: too many children\n");
			continue;
		}
	}

	return(tmp);
}

struct bsp_node_s *pdgl_main_readbsp(char *name)
{
	struct bsp_node_s *tmp;
	FILE *fd;
	char buf[256];
	char **a;
	int i, j;

	fd=fopen(name, "rt");
	if(!fd)return(NULL);
	main_leafnum=0;
	main_nodenum=0;
	pdgl_main_bsphash=0;

	tmp=NULL;
	while(!feof(fd))
	{
		memset(buf, 0, 256);
		fgets(buf, 255, fd);
		pdgl_main_hashline(buf);

		a=ksplit(buf);
		if(!a[0])continue;

		if(!strcmp(a[0], "textures"))
		{
			printf("textures\n");

			i=atoi(a[1]);
			pdgl_main_texnames=malloc((i+1)*sizeof(char *));
			pdgl_main_tex_xs=malloc((i+1)*sizeof(int));
			pdgl_main_tex_ys=malloc((i+1)*sizeof(int));
			pdgl_main_tex_num=malloc((i+1)*sizeof(int));
			pdgl_main_tex_num2=malloc((i+1)*sizeof(int));
			pdgl_main_tex_flags=malloc((i+1)*sizeof(int));
			pdgl_main_tex_buf=malloc((i+1)*sizeof(byte *));

			for(j=0; j<i; j++)
			{
				memset(buf, 0, 256);
				fgets(buf, 255, fd);
				pdgl_main_hashline(buf);
				a=ksplit(buf);

				sprintf(buf, "textures/%s", a[1]);

				pdgl_main_texnames[j]=strdup(buf);
				pdgl_main_tex_xs[j]=atoi(a[2]);
				pdgl_main_tex_ys[j]=atoi(a[3]);

#if 0
				pdgl_main_tex_num[j]=
					LBXGL_Texture_LoadImage(buf);
				pdgl_main_tex_flags[j]=
					LBXGL_Texture_GetImageFlags(
						pdgl_main_tex_num[j]);
				pdgl_main_tex_buf[j]=
					LBXGL_Texture_GetImageBuffer(
						pdgl_main_tex_num[j],
						NULL, NULL);

				sprintf(buf, "textures/%s_s", a[1]);
				pdgl_main_tex_num2[j]=
					LBXGL_Texture_LoadImage(buf);

				printf("  %s %d %d %d %d\n",
					pdgl_main_texnames[j],
					pdgl_main_tex_xs[j],
					pdgl_main_tex_ys[j],
					pdgl_main_tex_num[j],
					pdgl_main_tex_num2[j]
					);
#endif
			}

			continue;
		}

		if(!strcmp(a[0], "bsp"))
		{
			fgets(buf, 255, fd);
			pdgl_main_hashline(buf);
			a=ksplit(buf);

			tmp=pdgl_main_readnode(fd, a);
			continue;
		}
	}
	return(tmp);
}


int pdgl_main_traceface(struct bsp_face_s *face, float *sp, float *ep)
{
	float pt[3], dv[3], dn[4];
	float f, g;
	int i, j;

	f=V3D_NDOT(sp, face->norm);
	g=V3D_NDOT(ep, face->norm);

	if((f<=0) && (g<=0))return(0);
	if((f>=0) && (g>=0))return(0);

	Vec3F_LinePlaneIntersect(sp, ep, face->norm, pt);
	for(i=0; i<face->nvecs; i++)
	{
		f=V3F_NDOT(pt, face->evecs+i*4);
		if(f>=0)return(0);
	}
	return(1);
}

int pdgl_main_tracefaces(struct bsp_face_s *face, float *sp, float *ep)
{
	struct bsp_face_s *cur;
	int i;

	cur=face;
	while(cur)
	{
		i=pdgl_main_traceface(cur, sp, ep);
		if(i)return(1);
		cur=cur->next;
	}
	return(0);
}

int pdgl_main_tracenode(struct bsp_node_s *node, float *sp, float *ep)
{
	float f, g;
	int i, j;

	if(!node->lnode)
		return(0);

	f=V3D_NDOT(sp, node->norm);
	g=V3D_NDOT(ep, node->norm);

	if((f<0) && (g<0))
	{
		i=pdgl_main_tracenode(node->lnode, sp, ep);
		return(i);
	}
	if((f>0) && (g>0))
	{
		i=pdgl_main_tracenode(node->rnode, sp, ep);
		return(i);
	}

	i=pdgl_main_tracefaces(node->face, sp, ep);
	if(i)return(i);

	i=pdgl_main_tracenode(node->lnode, sp, ep);
	if(i)return(i);
	i=pdgl_main_tracenode(node->rnode, sp, ep);
	return(i);
}


int pdgl_main_traceface2(struct bsp_face_s *face, float *sp, float *ep,
	float *io)
{
	float pt[3], dv[3], dn[4];
	float f, g;
	int i, j;

	f=V3D_NDOT(sp, face->norm);
	g=V3D_NDOT(ep, face->norm);

	if((f<=0) && (g<=0))return(0);
	if((f>=0) && (g>=0))return(0);

	Vec3F_LinePlaneIntersect(sp, ep, face->norm, pt);
	for(i=0; i<face->nvecs; i++)
	{
		f=V3F_NDOT(pt, face->evecs+i*4);
		if(f>=0)return(0);
	}

	V3F_COPY(pt, io);
	return(1);
}

struct bsp_face_s *pdgl_main_tracefaces2(struct bsp_face_s *face,
	float *sp, float *ep, float *io)
{
	struct bsp_face_s *cur;
	int i;

	cur=face;
	while(cur)
	{
		i=pdgl_main_traceface2(cur, sp, ep, io);
		if(i)return(cur);
		cur=cur->next;
	}
	return(NULL);
}

struct bsp_face_s *pdgl_main_tracenode2(struct bsp_node_s *node,
	float *sp, float *ep, float *io)
{
	struct bsp_face_s *tmp;
	float f, g;
	int i, j;

	if(!node->lnode)return(NULL);

	f=V3D_NDOT(sp, node->norm);
	g=V3D_NDOT(ep, node->norm);

	if((f<=0) && (g<=0))
	{
		tmp=pdgl_main_tracenode2(node->lnode, sp, ep, io);
		return(tmp);
	}
	if((f>=0) && (g>=0))
	{
		tmp=pdgl_main_tracenode2(node->rnode, sp, ep, io);
		return(tmp);
	}

	if(f<g)
	{
		tmp=pdgl_main_tracenode2(node->lnode, sp, ep, io);
		if(tmp)return(tmp);

		tmp=pdgl_main_tracefaces2(node->face, sp, ep, io);
		if(tmp)return(tmp);

		tmp=pdgl_main_tracenode2(node->rnode, sp, ep, io);
		return(tmp);
	}else
	{
		tmp=pdgl_main_tracenode2(node->rnode, sp, ep, io);
		if(tmp)return(tmp);

		tmp=pdgl_main_tracefaces2(node->face, sp, ep, io);
		if(tmp)return(tmp);

		tmp=pdgl_main_tracenode2(node->lnode, sp, ep, io);
		return(tmp);
	}
}

#if 0
void pdgl_main_drawface2(float *xyz, float *st, int num,
	int fl, int txs, int tys)
{
	float s, t, f;
	int i;

	glBegin(GL_POLYGON);
	for(i=0; i<num; i++)
	{
		if(fl&LBXGL_TXFL_WARPST)
		{
			f=V3F_LEN(xyz+i*3);

			s=st[i*2+0];
			t=st[i*2+1];
			s+=sin(main_time+f)/txs*4;
			t+=cos(main_time+f)/tys*4;
			glTexCoord2f(s, t);
		}else glTexCoord2fv(st+i*2);
		glVertex3fv(xyz+i*3);
	}
	glEnd();
}

void pdgl_main_drawface3(float *xyz, float *st, int num,
	int fl, int txs, int tys)
{
	float s, t, f;
	int i;

	glBegin(GL_POLYGON);
	for(i=0; i<num; i++)
	{
		glTexCoord2fv(st+i*2);
		glVertex3fv(xyz+i*3);
	}
	glEnd();
}

void pdgl_main_drawface4(float *xyz, float *st, int num)
{
	float f, d;
	int i, j;
	float r, g, b;

	if(!main_dlights)return;

	glDisable(GL_TEXTURE_2D);

	glBegin(GL_POLYGON);
	for(i=0; i<num; i++)
	{
		r=0; g=0; b=0;

		for(j=0; j<main_dlights; j++)
		{
			d=V3F_DIST(xyz+i*3, main_dlight_org+j*3);
			f=((1.0/255.0)*main_light_val[j*2+0])/(d+1);
			r+=main_light_clr[j*3+0]*f;
			g+=main_light_clr[j*3+1]*f;
			b+=main_light_clr[j*3+2]*f;
		}

		glColor4f(r, g, b, 1.0);
//		glTexCoord2fv(st+i*2);
		glVertex3fv(xyz+i*3);
	}
	glEnd();

	glEnable(GL_TEXTURE_2D);
	glColor4f(1, 1, 1, 1);
}

void pdgl_main_drawface41(struct bsp_face_s *face)
{
	float pt[3], pt1[3];
	float f, d, v;
	int i, j;
	float r, g, b;
	float a, x, y;

	if(!main_dlights)return;

	glDisable(GL_TEXTURE_2D);
	glDisable(GL_DEPTH_TEST);
//	glDepthFunc(GL_GREATER);
	glEnable(GL_STENCIL_TEST);

	r=0; g=0; b=0;
	for(i=0; i<main_dlights; i++)
	{
		V3F_COPY(main_dlight_org+i*3, pt);
		f=V3F_NDOT(pt, face->norm);
//		if(f<0)continue;

		V3F_ADDSCALE(pt, face->norm, -f-0.01, pt);

#if 0
		for(j=0; j<face->nvecs; j++)
		{
			f=V3F_NDOT(pt, face->evecs+j*4);
			if(f>0)
			{
				V3F_ADDSCALE(pt, face->evecs+j*4, -f, pt);
			}
		}
#endif

		v=main_light_val[i*2+0];
		d=V3F_DIST(pt, main_dlight_org+i*3);
//		f=(v/255.0)/(d+1);
		f=(6*v/255.0)/(d*d+1);
		if(f>1)f=1;

//		f=1;
//		if(f<0.1)continue;

		glBegin(GL_TRIANGLE_FAN);
		glColor4f(f, f, f, 1.0);
		glVertex3fv(pt);

#if 0
		for(j=0; j<face->nvecs; j++)
		{
			d=V3F_DIST(face->vecs+j*3, main_dlight_org+i*3);
//			f=(v/255.0)/(d+1);
			f=(6*v/255.0)/(d*d+1);
			glColor4f(f, f, f, 1.0);
			glVertex3fv(face->vecs+j*3);
		}

		d=V3F_DIST(face->vecs, main_dlight_org+i*3);
//		f=(v/255.0)/(d+1);
		f=(6*v/255.0)/(d*d+1);
		glColor4f(f, f, f, 1.0);
		glVertex3fv(face->vecs);
		glEnd();
#endif

#if 1
		glColor4f(0.1, 0.1, 0.1, 1.0);
		for(j=0; j<9; j++)
		{
			a=(1.0/4.0*M_PI)*j;
			x=sin(a)*(f/0.1); y=cos(a)*(f/0.1);

			pt1[0]=pt[0] + face->sdir[0]*x + face->tdir[0]*y;
			pt1[1]=pt[1] + face->sdir[1]*x + face->tdir[1]*y;
			pt1[2]=pt[2] + face->sdir[2]*x + face->tdir[2]*y;
			glVertex3fv(pt1);
		}
		glEnd();
#endif
	}

	glDisable(GL_STENCIL_TEST);
	glDepthFunc(GL_LEQUAL);
	glEnable(GL_TEXTURE_2D);
	glColor4f(1, 1, 1, 1);

#if 0

	glBegin(GL_POLYGON);
	for(i=0; i<face->nvecs; i++)
	{
#if 0
		r=0; g=0; b=0;
		for(j=0; j<main_dlights; j++)
		{
			d=V3F_DIST(xyz+i*3, main_dlight_org+j*3);
			f=((1.0/255.0)*main_light_val[j*2+0])/(d+1);
			r+=main_light_clr[j*3+0]*f;
			g+=main_light_clr[j*3+1]*f;
			b+=main_light_clr[j*3+2]*f;
		}
		glColor4f(r, g, b, 1.0);
#endif

		glVertex3fv(face->vecs+i*3);
	}
	glEnd();

	glEnable(GL_TEXTURE_2D);
	glColor4f(1, 1, 1, 1);
#endif
}

void pdgl_main_drawface42(struct bsp_face_s *face)
{
	float pt[3], pt1[3];
	float f, d, v;
	int i, j, vlt;
	float r, g, b;
	float a, x, y;

	if(!main_dlights)return;

	glDisable(GL_TEXTURE_2D);
	glDisable(GL_DEPTH_TEST);
//	glDepthFunc(GL_GEQUAL);

	r=0; g=0; b=0;
	for(i=0; i<main_dlights; i++)
	{
		V3F_COPY(main_dlight_org+i*3, pt);
		f=V3F_NDOT(pt, face->norm);
//		if(f<0)continue;

		V3F_ADDSCALE(pt, face->norm, -f-0.1, pt);

		vlt=0;
#if 1
		for(j=0; j<face->nvecs; j++)
		{
			f=V3F_NDOT(pt, face->evecs+j*4);
			if(f>0)
			{
				vlt=1;
				V3F_ADDSCALE(pt, face->evecs+j*4, -f, pt);
			}
		}
#endif

		v=main_dlight_val[i*2+0];
		d=V3F_DIST(pt, main_dlight_org+i*3);
//		f=(v/255.0)/(d+1);
		f=(6*v/255.0)/(d*d+1);

//		f=1;
//		if(f<0.1)continue;

		if(!vlt)
		{
			glBegin(GL_TRIANGLE_FAN);
			glColor4f(f, f, f, 1.0);
			glVertex3fv(pt);

			for(j=0; j<face->nvecs; j++)
			{
				d=V3F_DIST(face->vecs+j*3, main_dlight_org+i*3);
				f=(6*v/255.0)/(d*d+1);
				glColor4f(f, f, f, 1.0);
				glVertex3fv(face->vecs+j*3);
			}

			d=V3F_DIST(face->vecs, main_dlight_org+i*3);
			f=(6*v/255.0)/(d*d+1);
			glColor4f(f, f, f, 1.0);
			glVertex3fv(face->vecs);
			glEnd();
		}else
		{
			glBegin(GL_POLYGON);
			for(j=0; j<face->nvecs; j++)
			{
				d=V3F_DIST(face->vecs+j*3, main_dlight_org+i*3);
				f=(6*v/255.0)/(d*d+1);
				glColor4f(f, f, f, 1.0);
				glVertex3fv(face->vecs+j*3);
			}
			glEnd();
		}
	}

	glDepthFunc(GL_LEQUAL);
	glEnable(GL_TEXTURE_2D);
	glColor4f(1, 1, 1, 1);
}
#endif

void pdgl_main_drawface(struct bsp_face_s *face)
{
	float nv[3];
	float s, t, f, r, g, b;
	int i, fl, xs, ys;

#if 0
	fl=pdgl_main_tex_flags[face->num];
	xs=pdgl_main_tex_xs[face->num];
	ys=pdgl_main_tex_ys[face->num];

	//cull non-window backfaces
	if((V3F_NDOT(main_cam_org, face->norm)<=0) &&
		!(fl&LBXGL_TXFL_WINDOW))
			return;

	glDepthFunc(GL_LEQUAL);
	glEnable(GL_TEXTURE_2D);
	glDisable(GL_LIGHTING);
	glColor4f(1, 1, 1, 1);

	V3F_SCALE(face->norm, -1, nv);
	glNormal3fv(nv);

	if(face->lm && (face->lm->texnum>=0))
	{
		if(fl&LBXGL_TXFL_WINDOW)
		{
			glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_TRUE);

			glBlendFunc(GL_ONE, GL_ZERO);
			Draw_Bind(pdgl_main_tex_num[face->num]);
			pdgl_main_drawface2(face->vecs, face->st, face->nvecs,
				fl, xs, ys);

			glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);

			glBlendFunc(GL_DST_ALPHA, GL_ONE_MINUS_DST_ALPHA);
			Draw_Bind(face->lm->texnum);
			pdgl_main_drawface3(face->vecs, face->uv, face->nvecs,
				fl, xs, ys);
		}else
		{
			glGetIntegerv(GL_STENCIL_BITS, &i);
			if(i>16)i=16;
			i=(1<<i)-1;

//			printf("%d\n", i);

//			glStencilFunc(GL_EQUAL, pdgl_main_faceid&i, i);
//			glStencilFunc(GL_EQUAL, 0, 0xFF);
//			glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);
//			pdgl_main_faceid++;

			glBlendFunc(GL_ONE, GL_ZERO);
			Draw_Bind(face->lm->texnum);
			pdgl_main_drawface3(face->vecs, face->uv, face->nvecs,
				fl, xs, ys);

			glBlendFunc(GL_ONE, GL_ONE);
//			pdgl_main_drawface4(face->vecs, face->uv, face->nvecs);
//			pdgl_main_drawface41(face);
			pdgl_main_drawface42(face);
		}
	}

	glEnable(GL_DEPTH_TEST);

	glBlendFunc(GL_DST_COLOR, GL_ONE_MINUS_SRC_ALPHA);
	Draw_Bind(pdgl_main_tex_num[face->num]);
	pdgl_main_drawface2(face->vecs, face->st, face->nvecs, fl, xs, ys);

	if(pdgl_main_tex_num2[face->num]>=0)
	{
		glBlendFunc(GL_SRC_COLOR, GL_ONE);
		Draw_Bind(pdgl_main_tex_num2[face->num]);
		pdgl_main_drawface2(face->vecs, face->st, face->nvecs,
			fl, xs, ys);
	}

	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glEnable(GL_DEPTH_TEST);

#endif

	glDisable(GL_TEXTURE_2D);

	glBegin(GL_POLYGON);
	for(i=0; i<face->nvecs; i++)
	{
//		r=sin(face->vecs[i*3+0])*0.5+0.5;
//		g=sin(face->vecs[i*3+1])*0.5+0.5;
//		b=sin(face->vecs[i*3+2])*0.5+0.5;

//		glColor4f(r, g, b, 0.66);
		glVertex3fv(face->vecs+i*3);
	}
	glEnd();
}

void pdgl_main_drawfaces(struct bsp_face_s *face)
{
	struct bsp_face_s *cur;
	cur=face;
	while(cur)
	{
		pdgl_main_drawface(cur);
		cur=cur->next;
	}
}

void pdgl_main_drawnode(struct bsp_node_s *node)
{
	struct bsp_face_s *cur;
	float corg[3];
	float f;

	if(!node->lnode)
	{
		glDisable(GL_TEXTURE_2D);
		glColor4f((node->num&3)/3.0, (node->num&12)/12.0,
			(node->num&48)/48.0, 0.5);

		draw_ents(node->ents);
//		pdgl_main_drawfaces(node->face);
		return;
	}

	draw_ents(node->ents);

//	Vec3F_Scale(main_cam_org, 36, corg);
//	f=Vec3F_DotProduct(corg, node->norm)-node->norm[3];
	f=V3F_NDOT(main_cam_org, node->norm);
	if(f<0)
	{
		pdgl_main_drawnode(node->rnode);

//		draw_ents(node->ents);

		glDisable(GL_TEXTURE_2D);
		glColor4f((node->num&3)/3.0, (node->num&12)/12.0,
			(node->num&48)/48.0, 0.25);
		pdgl_main_drawfaces(node->face);

		pdgl_main_drawnode(node->lnode);
	}else
	{
		pdgl_main_drawnode(node->lnode);

//		draw_ents(node->ents);

		glDisable(GL_TEXTURE_2D);
		glColor4f((node->num&3)/3.0, (node->num&12)/12.0,
			(node->num&48)/48.0, 0.25);
		pdgl_main_drawfaces(node->face);

		pdgl_main_drawnode(node->rnode);
	}
}


void pdgl_main_sendface(struct bsp_face_s *face)
{
	int i;

	bsdeBegin(BSDE_BSP_FACE);
	bsdeNormal4fv(face->norm);
	for(i=0; i<face->nvecs; i++)
		bsdeVertex3fv(face->vecs+i*3);
	bsdeEnd();
}

void pdgl_main_sendfaces(struct bsp_face_s *face)
{
	struct bsp_face_s *cur;
	cur=face;
	while(cur)
	{
		pdgl_main_sendface(cur);
		cur=cur->next;
	}
}

void pdgl_main_sendnode(struct bsp_node_s *node)
{
	struct bsp_face_s *cur;
	float corg[3];
	float f;

	if(!node->lnode)
	{
		bsdeBegin(BSDE_BSP_LEAF);
		pdgl_main_sendfaces(node->face);
		bsdeEnd();
		return;
	}

	bsdeBegin(BSDE_BSP_NODE);
	bsdeNormal4fv(node->norm);
	pdgl_main_sendnode(node->lnode);
	pdgl_main_sendnode(node->rnode);
	bsdeEnd();
}

