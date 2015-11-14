struct bsp_lm_s *main_lmroot=NULL;

void pdgl_main_bestuv(float *norm, float *udir, float *vdir);

void pdgl_main_readlights(char *name)
{
	FILE *fd;
	char buf[256];
	char **a;
	int i, j;

	main_lights=0;

	fd=fopen(name, "rt");
	if(!fd)return;

	i=0;
	while(!feof(fd))
	{
		memset(buf, 0, 256);
		fgets(buf, 255, fd);

		a=ksplit(buf);
		if(!a[0])continue;

		main_light_org[i*3+0]=atof(a[1]);
		main_light_org[i*3+1]=atof(a[2]);
		main_light_org[i*3+2]=atof(a[3]);

		main_light_clr[i*3+0]=atof(a[6]);
		main_light_clr[i*3+1]=atof(a[7]);
		main_light_clr[i*3+2]=atof(a[8]);

		main_light_val[i*2+0]=atof(a[10]);
		main_light_val[i*2+1]=atof(a[11]);

		i++;
	}

	main_lights=i;
}

struct bsp_lm_s *pdgl_main_lookuplm(float *norm)
{
	struct bsp_lm_s *cur;

	cur=main_lmroot;
	while(cur)
	{
		if(V4F_DIST2(cur->norm, norm)<0.01)
			return(cur);
		cur=cur->next;
	}
	return(NULL);
}

struct bsp_lm_s *pdgl_main_getlm(float *norm)
{
	struct bsp_lm_s *tmp;

	tmp=pdgl_main_lookuplm(norm);
	if(tmp)return(tmp);

	tmp=malloc(sizeof(struct bsp_lm_s));
	memset(tmp, 0, sizeof(struct bsp_lm_s));

	V4F_COPY(norm, tmp->norm);
	pdgl_main_bestuv(tmp->norm, tmp->udir, tmp->vdir);
	tmp->texnum=-1;

	tmp->next=main_lmroot;
	main_lmroot=tmp;

	return(tmp);
}

void pdgl_main_makelmface(struct bsp_face_s *face)
{
	struct bsp_lm_s *lm;

	lm=pdgl_main_getlm(face->norm);
	face->lm=lm;
	face->lmnext=lm->face;
	lm->face=face;
}

void pdgl_main_makelmfaces(struct bsp_face_s *face)
{
	struct bsp_face_s *cur;
	cur=face;
	while(cur)
	{
		pdgl_main_makelmface(cur);
		cur=cur->next;
	}
}

void pdgl_main_makelmnode(struct bsp_node_s *node)
{
	if(!node->lnode)return;
	pdgl_main_makelmfaces(node->face);
	pdgl_main_makelmnode(node->lnode);
	pdgl_main_makelmnode(node->rnode);
}


void pdgl_main_bestuv(float *norm, float *udir, float *vdir)
{
	float f;

	V3F_ZERO(udir);
	V3F_ZERO(vdir);

	if(fabs(norm[0])>fabs(norm[1]))
	{
		if(fabs(norm[0])>fabs(norm[2]))
			{ udir[1]=1; vdir[2]=1; }
		else { udir[0]=1; vdir[1]=1; }
	}else
	{
		if(fabs(norm[1])>fabs(norm[2]))
			{ udir[0]=1; vdir[2]=1; }
		else { udir[0]=1; vdir[1]=1; }
	}

	f=V3F_DOT(udir, norm);
	V3F_ADDSCALE(udir, norm, -f, udir);
	f=V3F_DOT(vdir, norm);
	V3F_ADDSCALE(vdir, norm, -f, vdir);

	V3F_NORMALIZE(udir, udir);
	V3F_NORMALIZE(vdir, vdir);
}

void pdgl_main_lmfacebox(struct bsp_face_s *face,
	float *udir, float *vdir, float *min, float *max)
{
	float f;
	int i;

	for(i=0; i<face->nvecs; i++)
	{
		f=V3F_DOT(face->vecs+i*3, udir);
		if(f<min[0])min[0]=f;
		if(f>max[0])max[0]=f;

		f=V3F_DOT(face->vecs+i*3, vdir);
		if(f<min[1])min[1]=f;
		if(f>max[1])max[1]=f;
	}
}

void pdgl_main_lmfacepointcolor(struct bsp_face_s *face, float *pt, byte *clr)
{
	byte *buf;
	float s, t;
	int x, y, xs, ys;

	xs=pdgl_main_tex_xs[face->num];
	ys=pdgl_main_tex_ys[face->num];
	buf=pdgl_main_tex_buf[face->num];
	if(!buf)
	{
		//opaque grey
		clr[0]=128; clr[1]=128; clr[2]=128; clr[3]=255;
		return;
	}

//	printf("buffer\n");

	s=V3F_NDOT(pt, face->sdir);
	t=V3F_NDOT(pt, face->tdir);
	x=((int)(s*xs))%xs;
	y=((int)(t*ys))%ys;

	clr[0]=buf[(y*xs+x)*4+0];
	clr[1]=buf[(y*xs+x)*4+1];
	clr[2]=buf[(y*xs+x)*4+2];
	clr[3]=buf[(y*xs+x)*4+3];
}


void pdgl_main_lmface(struct bsp_face_s *face, struct bsp_node_s *root)
{
	float min[2], max[2];
	float u, v, d, pt[3], lo[3];
	int i, j, k, x0, y0, x1, y1;

	for(i=0; i<face->nvecs; i++)
	{
		u=V3F_DOT(face->vecs+i*3, face->lm->udir);
		v=V3F_DOT(face->vecs+i*3, face->lm->vdir);

		u=(u-face->lm->min[0])/face->lm->usz;
		v=(v-face->lm->min[1])/face->lm->vsz;

		face->uv[i*2+0]=u;
		face->uv[i*2+1]=v;
	}
}

void pdgl_main_lmfaces(struct bsp_face_s *face, struct bsp_node_s *root)
{
	struct bsp_face_s *cur;
	cur=face;
	while(cur)
	{
		pdgl_main_lmface(cur, root);
		cur=cur->lmnext;
	}
}

void pdgl_main_drawlmlight(struct bsp_lm_s *lm, struct bsp_node_s *root,
	float *lo, float lv)
{
	struct bsp_face_s *cf;
	float pt[3];
	float f, g, u, v, d;
	int i, j, k, l;
	byte clr[4];

	if(V3F_NDOT(lo, lm->norm)<=0)
		return;

	g=6;
	for(i=0; i<lm->ys; i++)
		for(j=0; j<lm->xs; j++)
	{
		u=(j+0.5)*(lm->usz/lm->xs)+lm->min[0];
		v=(i+0.5)*(lm->vsz/lm->ys)+lm->min[1];

		pt[0]=lm->udir[0]*u + lm->vdir[0]*v + lm->norm[0]*lm->norm[3];
		pt[1]=lm->udir[1]*u + lm->vdir[1]*v + lm->norm[1]*lm->norm[3];
		pt[2]=lm->udir[2]*u + lm->vdir[2]*v + lm->norm[2]*lm->norm[3];

		d=V3F_DIST(lo, pt);
		f=(lv*g)/(d*d+1);
		if(f<4)continue;

		cf=pdgl_main_tracenode2(root, pt, lo, pt);
		l=0;
		while(cf)
		{
			pdgl_main_lmfacepointcolor(cf, pt, clr);
			l+=clr[3];
			if(l>=224)break;
			cf=pdgl_main_tracenode2(root, pt, lo, pt);
		}

//		k=pdgl_main_tracenode(root, pt, lo);

//		if(!k)
		if(!cf)
		{
			if(l>255)l=255;

			k=lm->lmbuf[i*lm->xs+j]+f*(1.0-(l/255.0));
			k=(k<0)?0:(k>255)?255:k;
			lm->lmbuf[i*lm->xs+j]=k;
		}
	}
}

void pdgl_main_makelm(struct bsp_lm_s *lm, struct bsp_node_s *root)
{
	struct bsp_face_s *cur;
	byte *buf;
	float pt[3], lo[3];
	float f, u, v, d;
	int i, j, k, mx;

	lm->min[0]=999999;  lm->min[1]=999999;
	lm->max[0]=-999999; lm->max[1]=-999999;
	cur=lm->face;
	while(cur)
	{
		pdgl_main_lmfacebox(cur, lm->udir, lm->vdir,
			lm->min, lm->max);
		cur=cur->lmnext;
	}

	d=8.0;
	mx=1024;
//	mx=256;

	lm->min[0]-=1.0/d;
	lm->min[1]-=1.0/d;
	lm->max[0]+=1.0/d;
	lm->max[1]+=1.0/d;

	lm->usz=lm->max[0]-lm->min[0];
	lm->vsz=lm->max[1]-lm->min[1];

	i=lm->usz*d;
	if(i<1)i=1;
	if(i>mx)i=mx;
	lm->xs=i;

	i=lm->vsz*d;
	if(i<1)i=1;
	if(i>mx)i=mx;
	lm->ys=i;

	//create lightmap and fill with grey
	lm->lmbuf=malloc(lm->xs*lm->ys);
	for(i=0; i<(lm->xs*lm->ys); i++)lm->lmbuf[i]=0;

//	V3F_ZERO(lo);
//	pdgl_main_drawlmlight(lm, root, lo, 1600);

//	lo[0]=0.5; lo[1]=0.5; lo[2]=0.5;
//	pdgl_main_drawlmlight(lm, root, lo, 800);
//	lo[0]=-0.5; lo[1]=-0.5; lo[2]=-0.5;
//	pdgl_main_drawlmlight(lm, root, lo, 800);

	for(i=0; i<main_lights; i++)
	{
		pdgl_main_drawlmlight(lm, root,
			main_light_org+i*3, main_light_val[i*2]);
	}

#if 1
	//generate a slightly blurred version of the image
	buf=malloc(lm->xs*lm->ys);
	memcpy(buf, lm->lmbuf, lm->xs*lm->ys);

	for(i=1; i<(lm->ys-1); i++)
		for(j=1; j<(lm->xs-1); j++)
	{
#if 1
		f=lm->lmbuf[i*lm->xs+j]*1.0 +
			lm->lmbuf[(i-1)*lm->xs+j]*0.5 +
			lm->lmbuf[(i+1)*lm->xs+j]*0.5 +
			lm->lmbuf[i*lm->xs+(j-1)]*0.5 +
			lm->lmbuf[i*lm->xs+(j+1)]*0.5 +
			lm->lmbuf[(i-1)*lm->xs+(j-1)]*0.414 +
			lm->lmbuf[(i-1)*lm->xs+(j+1)]*0.414 +
			lm->lmbuf[(i+1)*lm->xs+(j-1)]*0.414 +
			lm->lmbuf[(i+1)*lm->xs+(j+1)]*0.414;
		k=f/4.656;
#endif

#if 0
		k=lm->lmbuf[i*lm->xs+j] +
			lm->lmbuf[(i-1)*lm->xs+j] +
			lm->lmbuf[(i+1)*lm->xs+j] +
			lm->lmbuf[i*lm->xs+(j-1)] +
			lm->lmbuf[i*lm->xs+(j+1)] +
			lm->lmbuf[(i-1)*lm->xs+(j-1)] +
			lm->lmbuf[(i-1)*lm->xs+(j+1)] +
			lm->lmbuf[(i+1)*lm->xs+(j-1)] +
			lm->lmbuf[(i+1)*lm->xs+(j+1)];
		k=k/9;
#endif

		if(k<0)k=0;
		if(k>255)k=255;

		buf[i*lm->xs+j]=k;
	}

	free(lm->lmbuf);
	lm->lmbuf=buf;
#endif

	pdgl_main_lmfaces(lm->face, root);
//	lm->texnum=Tex_LoadTextureMono(lm->xs, lm->ys, lm->lmbuf);
}


void pdgl_main_buildlms(struct bsp_node_s *root)
{
	struct bsp_lm_s *cur;
	int i, j, k;

	pdgl_main_makelmnode(root);

	i=0; j=0;
	cur=main_lmroot;
	while(cur)
	{
		i++;
		cur=cur->next;
	}
//	printf("%d lms\n", i);

	j=0; k=0;
	cur=main_lmroot;
	while(cur)
	{
		pdgl_main_makelm(cur, root);
		k+=cur->xs*cur->ys;

		j++;
		printf("lm %d/%d (%dx%d)    \r", j, i, cur->xs, cur->ys);
		fflush(stdout);

		cur=cur->next;
	}
	printf("\n");
	printf("%d lms %dkB\n", i, k/1024);
}


void pdgl_main_writefixed(FILE *fd, float f, int s)
{
	int i;

	i=f*s;
	fputc(i&0xFF, fd); fputc((i>>8)&0xFF, fd);
	fputc((i>>16)&0xFF, fd); fputc((i>>24)&0xFF, fd);
}

void pdgl_main_writevector(FILE *fd, float *v)
{
	pdgl_main_writefixed(fd, v[0], 65536);
	pdgl_main_writefixed(fd, v[1], 65536);
	pdgl_main_writefixed(fd, v[2], 65536);
}

void pdgl_main_writenormal(FILE *fd, float *v)
{
	pdgl_main_writefixed(fd, v[0], 16777216);
	pdgl_main_writefixed(fd, v[1], 16777216);
	pdgl_main_writefixed(fd, v[2], 16777216);
	pdgl_main_writefixed(fd, v[3], 65536);
}

float pdgl_main_readfixed(FILE *fd, int s)
{
	int i;

	i=fgetc(fd);
	i|=fgetc(fd)<<8;
	i|=fgetc(fd)<<16;
	i|=fgetc(fd)<<24;

	return(((float)i)/s);
}

void pdgl_main_readvector(FILE *fd, float *v)
{
	v[0]=pdgl_main_readfixed(fd, 65536);
	v[1]=pdgl_main_readfixed(fd, 65536);
	v[2]=pdgl_main_readfixed(fd, 65536);
}

void pdgl_main_readnormal(FILE *fd, float *v)
{
	v[0]=pdgl_main_readfixed(fd, 16777216);
	v[1]=pdgl_main_readfixed(fd, 16777216);
	v[2]=pdgl_main_readfixed(fd, 16777216);
	v[3]=pdgl_main_readfixed(fd, 65536);
}

void pdgl_main_writeimg(FILE *fd, byte *buf, int xs, int ys)
{
	int i, j, k;
	int li, lj;

//	fwrite(buf, 1, xs*ys, fd);
//	return;

	li=0; lj=0;
	for(i=0; i<ys; i++)
		for(j=0; j<xs; j++)
	{
		if(i && j)
		{
			k=(buf[i*xs+j-1]+buf[(i-1)*xs+j]-
				buf[(i-1)*xs+j-1])&0xFF;
			k=(buf[i*xs+j]-k)&0xFF;
		}else k=buf[i*xs+j];

		if((k!=li) || (lj>62))
		{
			if(lj && ((lj>1) || ((li>=0x60) && (li<0xA0))))
				fputc(0x60+lj, fd);
			if(lj)fputc(li, fd);
			li=k; lj=1;
		}else lj++;
	}

	if(lj && ((lj>1) || ((li>=0x60) && (li<0xA0))))
		fputc(0x60+lj, fd);
	if(lj)fputc(li, fd);
}

void pdgl_main_readimg(FILE *fd, byte *buf, int xs, int ys)
{
	int i, j, k;
	int li, lj;

//	fread(buf, 1, xs*ys, fd);
//	return;

	li=0; lj=0;
	for(i=0; i<ys; i++)
		for(j=0; j<xs; j++)
	{
		if(!lj)
		{
			k=fgetc(fd);
			if((k>0x60) && (k<0xA0))
			{
				li=fgetc(fd);
				lj=k-0x61;
			}else li=k;
		}else lj--;

		if(i && j)
		{
			k=(buf[i*xs+j-1]+buf[(i-1)*xs+j]-
				buf[(i-1)*xs+j-1])&0xFF;
			buf[i*xs+j]=(li+k)&0xFF;
		}else buf[i*xs+j]=li;
	}
}


void pdgl_main_savelms(struct bsp_node_s *root, char *lmi)
{
	FILE *fd;
	struct bsp_lm_s *cur;
	int i, j, k;

	printf("LM Save '%s'\n", lmi);

	fd=fopen(lmi, "wb");

	i=pdgl_main_bsphash;
	fputc(i&0xFF, fd);
	fputc((i>>8)&0xFF, fd);
	fputc((i>>16)&0xFF, fd);
	fputc((i>>24)&0xFF, fd);

	cur=main_lmroot;
	while(cur)
	{
//		printf("LM offs %d\n", ftell(fd));

		fputc(cur->xs&0xFF, fd); fputc((cur->xs>>8)&0xFF, fd);
		fputc(cur->ys&0xFF, fd); fputc((cur->ys>>8)&0xFF, fd);

		pdgl_main_writenormal(fd, cur->norm);
		pdgl_main_writevector(fd, cur->udir);
		pdgl_main_writevector(fd, cur->vdir);

		pdgl_main_writefixed(fd, cur->min[0], 65536);
		pdgl_main_writefixed(fd, cur->min[1], 65536);
		pdgl_main_writefixed(fd, cur->max[0], 65536);
		pdgl_main_writefixed(fd, cur->max[1], 65536);

//		printf("LM offs2 %d\n", ftell(fd));

		pdgl_main_writeimg(fd, cur->lmbuf, cur->xs, cur->ys);

		cur=cur->next;
	}

	fputc(0, fd); fputc(0, fd);
	fputc(0, fd); fputc(0, fd);

	fclose(fd);
	printf("LM Save OK\n");
}

int pdgl_main_loadlms(struct bsp_node_s *root, char *lmi)
{
	FILE *fd;
	struct bsp_lm_s *cur, *tmp;
	int i, j, k;

	printf("LM Load '%s'\n", lmi);

	fd=fopen(lmi, "rb");
	if(!fd)return(-1);

	i=fgetc(fd);
	i|=fgetc(fd)<<8;
	i|=fgetc(fd)<<16;
	i|=fgetc(fd)<<24;

	if(i!=pdgl_main_bsphash)
	{
		fclose(fd);
		return(-1);
	}

	while(!feof(fd))
	{
//		printf("LM offs %d\n", ftell(fd));

		i=fgetc(fd); i|=fgetc(fd)<<8;
		j=fgetc(fd); j|=fgetc(fd)<<8;

		if(!i && !j)break;
		if(feof(fd))break;

		if((i>1024) || (j>1024))
		{
			printf("LM offs %d\n", ftell(fd)-4);
			printf("bad lm %dx%d\n", i, j);
			break;
		}

		tmp=malloc(sizeof(struct bsp_lm_s));
		memset(tmp, 0, sizeof(struct bsp_lm_s));

		cur=main_lmroot;
		while(cur && cur->next)cur=cur->next;
		if(cur)cur->next=tmp; else main_lmroot=tmp;

		tmp->xs=i;
		tmp->ys=j;

//		printf("lm %dx%d\n", tmp->xs, tmp->ys);

		pdgl_main_readnormal(fd, tmp->norm);
		pdgl_main_readvector(fd, tmp->udir);
		pdgl_main_readvector(fd, tmp->vdir);

		tmp->min[0]=pdgl_main_readfixed(fd, 65536);
		tmp->min[1]=pdgl_main_readfixed(fd, 65536);
		tmp->max[0]=pdgl_main_readfixed(fd, 65536);
		tmp->max[1]=pdgl_main_readfixed(fd, 65536);

		tmp->usz=tmp->max[0]-tmp->min[0];
		tmp->vsz=tmp->max[1]-tmp->min[1];

//		printf("LM offs2 %d\n", ftell(fd));

		tmp->lmbuf=malloc(tmp->xs*tmp->ys);
		pdgl_main_readimg(fd, tmp->lmbuf, tmp->xs, tmp->ys);
//		tmp->texnum=Tex_LoadTextureMono(tmp->xs, tmp->ys, tmp->lmbuf);
	}

	pdgl_main_makelmnode(root);
	cur=main_lmroot;
	while(cur)
	{
		pdgl_main_lmfaces(cur->face, root);
		cur=cur->next;
	}

	printf("LM Load OK\n");

	return(0);
}
