
//lighting experiment

int LBXGL_ClipWinding(float *norm, float *ipts, float *opts, int num)
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
	if(V3D_NDOT(ipts+(j*3), norm)<0)
	{
		Vec3F_LinePlaneIntersect(
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
	if(V3F_NDOT(ipts+(k*3), norm)<0)
	{
		Vec3F_LinePlaneIntersect(
			ipts+(j*3), ipts+(k*3), norm, opts+(l*3));
		l++;
	}

	return(l);
}

//verify whether the volume of 2 windings intersects
int LBXGL_CheckWindingPointCollide(float *org,
	float *apts, int anum, float *bpts, int bnum)
{
}

int LBXGL_ClipWindingsPointFace(struct bsp_face_s *face, float *org,
	float *pts, int *cnts, int num)
{
	static float pts0[4096*3], pts1[4096*3];
	static int cnts0[64], flgs0[64];
	static int cnts1[64], flgs1[64];

	float dv0[3], dv1[3], dn[4], dn1[4];
	float f;
	int i, j, k, l, m, n, n1;

	if(V3F_NDOT(org, face->norm)<=0.05)
		return(num);

	//copy input to temp state
	n=num;
	k=0;
	for(i=0; i<num; i++)
	{
		cnts0[i]=cnts[i];
		flgs0[i]=0;

		for(j=0; j<cnts[i]; j++)
		{
			V3F_COPY(pts+k*3, pts0+k*3);
			k++;
		}
	}

#if 1
	//clip
	for(i=0; i<face->nvecs; i++)
	{
		//compute edge planes
		j=(i+1)%face->nvecs;
		V3F_SUB(face->vecs+j*3, face->vecs+i*3, dv0);
		V3F_SUB(org, face->vecs+i*3, dv1);
//		V3F_CROSS(dv0, dv1, dn);
		V3F_CROSS(dv1, dv0, dn);
		f=V3F_NORMALIZE(dn, dn);
		V3F_SCALE(dn, -1, dn1);
		dn[3]=V3F_DOT(org, dn);

		dn1[3]=V3F_DOT(org, dn1);


		V4F_COPY(face->norm, dn);
		V4F_SCALE(dn, -1, dn1);

		//clip
		k=0; l=0; n1=0;
		for(j=0; j<n; j++)
		{
#if 1
			//inside
			m=LBXGL_ClipWinding(dn, pts0+k*3, pts1+l*3, cnts0[j]);
			cnts1[n1]=m;
			flgs1[n1]=flgs0[j];
			l+=m;
			if(m)n1++;
#endif

#if 1
			//outside
			m=LBXGL_ClipWinding(dn1, pts0+k*3, pts1+l*3, cnts0[j]);
			cnts1[n1]=m;
			flgs1[n1]=flgs0[j]|1;
			l+=m;
			if(m)n1++;
#endif

			k+=cnts0[j];
		}

		//copy
		l=0; n=n1;
		for(j=0; j<n1; j++)
		{
			cnts0[j]=cnts1[j];
			flgs0[j]=flgs1[j];

			for(k=0; k<cnts0[j]; k++)
			{
				V3F_COPY(pts1+l*3, pts0+l*3);
				l++;
			}
		}
	}
#endif

	//copy temp state to output
	k=0; l=0; n1=0;
	for(i=0; i<n; i++)
	{
		//skip inside faces
//		if(!flgs0[i])
		if(0)
		{
			l+=cnts0[i];
			continue;
		}

		cnts[n1]=cnts0[i];
		for(j=0; j<cnts0[i]; j++)
		{
			V3F_COPY(pts0+l*3, pts+k*3);
			k++; l++;
		}
		n1++;
	}

	return(n1);
}

void LBXGL_FaceExtents(struct bsp_face_s *face, float *norm,
	float *rm, float *rn)
{
	float m, n, f;
	int i;

	m=999999; n=-999999;
	for(i=0; i<face->nvecs; i++)
	{
		f=V3F_DOT(face->vecs+i*3, norm);
		if(f<m)m=f;
		if(f>n)n=f;
	}
	*rm=m; *rn=n;
}

int LBXGL_CheckFacePointCollide(float *org,
	struct bsp_face_s *aface, struct bsp_face_s *bface)
{
	float dv0[3], dv1[3], dn[4];
	float m, n;
	int i, j;

	LBXGL_FaceExtents(bface, aface->norm, &m, &n);
	if(n<=aface->norm[3])return(0);

	for(i=0; i<aface->nvecs; i++)
	{
		j=(i+1)%aface->nvecs;
		V3F_SUB(aface->vecs+j*3, aface->vecs+i*3, dv0);
		V3F_SUB(org, aface->vecs+i*3, dv1);
		V3F_CROSS(dv0, dv1, dn);
//		V3F_CROSS(dv1, dv0, dn);
		V3F_NORMALIZE(dn, dn);
		dn[3]=V3F_DOT(org, dn);


		LBXGL_FaceExtents(bface, dn, &m, &n);
		if(m>=dn[3])return(0);
	}

	return(1);
}

int LBXGL_ClipWindingsPointNode(struct bsp_node_s *node, float *org,
	float *pts, int *cnts, int num, struct bsp_face_s *lface)
{
	struct bsp_face_s *cur;
	int i, j, k;
	float f, m, n;

	if(!node->lnode)
		return(num);
	if(!num)return(num);

	cur=node->face;
	while(cur)
	{
		i=LBXGL_CheckFacePointCollide(org, lface, cur);
		if(!i)
		{
			cur=cur->next;
			continue;
		}


//		num=LBXGL_ClipWindingsPointFace(cur, org, pts, cnts, num);
		num=0;
		break;

		cur=cur->next;
	}

	if(!num)return(num);


	f=V3F_NDOT(org, node->norm);
	m=f; n=f; k=0;
	for(i=0; i<num; i++)
	{
		for(j=0; j<cnts[i]; j++)
		{
			f=V3F_NDOT(pts+k*3, node->norm);
			k++;

			if(f<m)m=f;
			if(f>n)n=f;
		}
	}
	if(m>=n)return(0);

#if 0
	if(n<=0)
	{
		i=LBXGL_ClipWindingsPointNode(node->lnode, org,
			pts, cnts, num, lface);
		return(i);
	}

	if(m>=0)
	{
		i=LBXGL_ClipWindingsPointNode(node->rnode, org,
			pts, cnts, num, lface);
		return(i);
	}
#endif

	num=LBXGL_ClipWindingsPointNode(node->lnode, org,
		pts, cnts, num, lface);
	num=LBXGL_ClipWindingsPointNode(node->rnode, org,
		pts, cnts, num, lface);
	return(num);
}

void pdgl_main_drawlightface(struct bsp_face_s *face,
	struct bsp_node_s *root)
{
	static float pts[4096*3];
	static int cnts[256];

	float org[3];
	float f, g;
	int i, j, k, n;

	V3F_ZERO(org);
	if(V3F_NDOT(org, face->norm)<=0)
		return;

	for(i=0; i<face->nvecs; i++)
		{ V3F_COPY(face->vecs+i*3, pts+i*3); }
	cnts[0]=face->nvecs;
	n=1;

	n=LBXGL_ClipWindingsPointNode(root, org, pts, cnts, 1, face);
//	printf("n %d\n", n);


	glDisable(GL_TEXTURE_2D);

	glBegin(GL_POLYGON);

	g=20;
	k=0;
	for(i=0; i<n; i++)
		for(j=0; j<cnts[i]; j++)
	{
		f=Vec3F_Length(pts+k*3);
		if(f<1)f=1;
		glColor4f(g/f, g/f, g/f, 1);

		glVertex3fv(pts+k*3);
		k++;
	}
	glEnd();

#if 0
	glBegin(GL_POLYGON);
	for(i=0; i<face->nvecs; i++)
	{
		f=Vec3F_Length(face->vecs+i*3);
		if(f<1)f=1;
		glColor4f(10.0/f, 10.0/f, 10.0/f, 1);

		glVertex3fv(face->vecs+i*3);
	}
	glEnd();
#endif
}

void pdgl_main_drawlightfaces(struct bsp_face_s *face,
	struct bsp_node_s *root)
{
	struct bsp_face_s *cur;
	cur=face;
	while(cur)
	{
		pdgl_main_drawlightface(cur, root);
		cur=cur->next;
	}
}

void pdgl_main_drawlightnode(struct bsp_node_s *node,
	struct bsp_node_s *root)
{
	struct bsp_face_s *cur;
	float f;

	if(!node->lnode)
	{
		draw_ents(node->ents);
		return;
	}

	draw_ents(node->ents);

	f=Vec3F_DotProduct(main_cam_org, node->norm)-node->norm[3];
	if(f<0)
	{
		pdgl_main_drawlightnode(node->rnode, root);
		pdgl_main_drawlightfaces(node->face, root);
		pdgl_main_drawlightnode(node->lnode, root);
	}else
	{
		pdgl_main_drawlightnode(node->lnode, root);
		pdgl_main_drawlightfaces(node->face, root);
		pdgl_main_drawlightnode(node->rnode, root);
	}
}
