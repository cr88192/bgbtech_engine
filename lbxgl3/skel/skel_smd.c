#include <lbxgl2.h>


static int skelsmd_num_skin;
static char	**skelsmd_skinname;

static int skelsmd_num_face;
static float	*skelsmd_face_xyz;
static float	*skelsmd_face_st;
static short	*skelsmd_face_skin;
static short	*skelsmd_face_bone;

static int skelsmd_num_xyz;
static int skelsmd_num_st;
static float skelsmd_xyz[3*16384];
static float skelsmd_st[3*16384];
static int skelsmd_bone[16384];

static float skelsmd_xform[64*16];


LBXGL_API void LBXGL_SkelSmd_FromAngles(float *a, float *b)
{
	float sx, cx, sy, cy, sz, cz;

	sx=sin(a[0]*0.5);
	cx=cos(a[0]*0.5);
	sy=sin(a[1]*0.5);
	cy=cos(a[1]*0.5);
	sz=sin(a[2]*0.5);
	cz=cos(a[2]*0.5);

	b[0]=sx*cy*cz-cx*sy*sz; // X
	b[1]=cx*sy*cz+sx*cy*sz; // Y
	b[2]=cx*cy*sz-sx*sy*cz; // Z
	b[3]=cx*cy*cz+sx*sy*sz; // W
}

LBXGL_API void LBXGL_SkelSmd_ToMatrix(float *a, float *b)
{
	float xx, xy, xz, xw, yy, yz, yw, zz, zw;
	int i;

	for(i=0; i<16; i++)b[i]=0;
	for(i=0; i<4; i++)b[(i*4)+i]=1;

	xx=a[0]*a[0];
	xy=a[0]*a[1];
	xz=a[0]*a[2];
	xw=a[0]*a[3];
	yy=a[1]*a[1];
	yz=a[1]*a[2];
	yw=a[1]*a[3];
	zz=a[2]*a[2];
	zw=a[2]*a[3];

	b[0]= 1-(2*yy)-(2*zz);
	b[1]=   (2*xy)-(2*zw);
	b[2]=   (2*xz)+(2*yw);

	b[4]=   (2*xy)+(2*zw);
	b[5]= 1-(2*xx)-(2*zz);
	b[6]=   (2*yz)-(2*xw);

	b[8]=   (2*xz)-(2*yw);
	b[9]=   (2*yz)+(2*xw);
	b[10]=1-(2*xx)-(2*yy);
}


//Load SMD Reference

LBXGL_API int LBXGL_SkelSmd_LoadBaseBones(LBXGL_Skel2Model *mdl, VFILE *fd)
{
	static char *bone_name[256];
	static float bone_org[256*3];
	static float bone_rot[256*9];
	static float bone_rorg[256*3];
	static int bone_up[256];
	char *buf, **a;
	int i, j, n;

	n=0;
	while(!vfeof(fd))
	{
		buf=gcralloc(256);
		memset(buf, 0, 256);
		vfgets(buf, 255, fd);

		a=gcrsplit(buf);
		if(!a[0])continue;

		if(!strcmp(a[0], "end"))
			break;

		i=atoi(a[0]);

		bone_name[i]=dystrdup(a[1]);
		bone_up[i]=atoi(a[2]);

		if((i+1)>n)n=i+1;
	}

	mdl->num_bones=n;
	if(!mdl->max_bones)
	{
		//model lacks space for skeleton, so set it up
		//this should be the case for render-only models
		//or other models that may not add any new bones

		mdl->max_bones=mdl->num_bones;
		mdl->bone_name=gctalloc("lbxgl_skel_smd", mdl->max_bones*sizeof(char *));
		mdl->bone_up=gctalloc("lbxgl_skel_smd", mdl->max_bones*sizeof(int));
		mdl->bone_org=gctalloc("lbxgl_skel_smd", mdl->max_bones*3*sizeof(float));
		mdl->bone_rot=gctalloc("lbxgl_skel_smd", mdl->max_bones*9*sizeof(float));
		mdl->bone_ang=gctalloc("lbxgl_skel_smd", mdl->max_bones*3*sizeof(float));
	}

	for(i=0; i<n; i++)
	{
		mdl->bone_name[i]=bone_name[i];
		mdl->bone_up[i]=bone_up[i];
	}

	return(0);
}

LBXGL_API int LBXGL_SkelSmd_LoadBaseFrame(LBXGL_Skel2Model *mdl, VFILE *fd)
{
	float org[3], ang[3], q[4], rot[9];
	char *buf, **a;
	int i, j, n;

	while(!vfeof(fd))
	{
		buf=gcralloc(256);
		memset(buf, 0, 256);
		vfgets(buf, 255, fd);

		a=gcrsplit(buf);
		if(!a[0])continue;

		if(!strcmp(a[0], "end"))
			break;

		if(!strcmp(a[0], "time"))
			continue;

		i=atoi(a[0]);
		for(j=0; j<3; j++)
		{
			org[j]=atof(a[1+j]);
			ang[j]=atof(a[4+j]);
		}

		LBXGL_SkelSmd_FromAngles(ang, q);
		QuatF_To3Matrix(q, rot);

		V3F_COPY(org, mdl->bone_org+i*3);
		V3F_COPY(ang, mdl->bone_ang+i*3);
		Mat3F_Copy(rot, mdl->bone_rot+i*9);
	}

	return(0);
}


#if 1

LBXGL_API int LBXGL_SkelSmd_IndexSkin(char *name)
{
	int i;

	for(i=0; i<skelsmd_num_skin; i++)
		if(!strcmp(skelsmd_skinname[i], name))
			return(i);
	skelsmd_skinname[i]=dystrdup(name);
	skelsmd_num_skin++;

	return(i);
}

LBXGL_API LBXGL_Skel2Mesh *LBXGL_SkelSmd_BuildMesh()
{
	static int tidx[65536];
	float tv0[3], tv1[3], tv2[3];
	int tlst[16];

	LBXGL_Skel2Mesh *tmp;
	LBXGL_Skel2MeshFace *face, *face2;
	int i, j, k, l, n, n0;
	float f;

	for(i=0; i<16; i++)tlst[i]=-1;

	tmp=gctalloc("lbxgl_skel_smd", sizeof(LBXGL_Skel2Mesh));
	tmp->face=gctalloc("lbxgl_skel_smd", skelsmd_num_face*sizeof(LBXGL_Skel2MeshFace *));
	tmp->n_faces=skelsmd_num_face;

	for(i=0; i<skelsmd_num_skin; i++)
	{
		tmp->texname[i]=skelsmd_skinname[i];
		tmp->texnum[i]=-1;
	}
	tmp->n_tex=skelsmd_num_skin;

	skelsmd_num_xyz=0;
	skelsmd_num_st=0;

	for(i=0; i<skelsmd_num_face; i++)
	{
		face=gctalloc("lbxgl_skel_smd", sizeof(LBXGL_Skel2MeshFace));
		tmp->face[i]=face;

		if(i)tmp->face[i-1]->next=face;
			else tmp->faces=face;

		j=skelsmd_face_skin[i];
		if(tlst[j]>=0)
		{
			tmp->face[tlst[j]]->tnext=i;
			tlst[j]=i;
		}else
		{
			tmp->texface[j]=i;
			tlst[j]=i;
		}

		for(j=0; j<3; j++)
		{
			l=skelsmd_face_bone[i*3+j];
			for(k=0; k<skelsmd_num_xyz; k++)
				if((skelsmd_bone[k]==l) && 
					V3F_DIST(skelsmd_xyz+k*3,
						skelsmd_face_xyz+i*9+j*3)<0.01)
							break;
			if(k>=skelsmd_num_xyz)
			{
				skelsmd_bone[k]=l;
				V3F_COPY(skelsmd_face_xyz+i*9+j*3,
					skelsmd_xyz+k*3);
				skelsmd_num_xyz++;
			}

			face->vecs[j]=k;
		}

		for(j=0; j<3; j++)
		{
			for(k=0; k<skelsmd_num_st; k++)
				if(V2F_DIST(skelsmd_st+k*2,
					skelsmd_face_st+i*6+j*2)<0.01)
						break;
			if(k>=skelsmd_num_st)
			{
				V2F_COPY(skelsmd_face_xyz+i*6+j*2, skelsmd_st+k*2);
				skelsmd_num_st++;
			}

			face->stvecs[j]=k;
		}

		V3F_SUB(skelsmd_xyz+face->vecs[1]*3,
			skelsmd_xyz+face->vecs[0]*3, tv0);
		V3F_SUB(skelsmd_xyz+face->vecs[2]*3,
			skelsmd_xyz+face->vecs[1]*3, tv1);
		V3F_CROSS(tv0, tv1, tv2);
		V3F_NORMALIZE(tv2, face->norm);
	}

	tmp->vecs=gctalloc("lbxgl_skel_smd", skelsmd_num_xyz*3*sizeof(float));
	tmp->stvecs=gctalloc("lbxgl_skel_smd", skelsmd_num_st*2*sizeof(float));
	tmp->n_vecs=skelsmd_num_xyz;
	tmp->n_stvecs=skelsmd_num_st;

	for(i=0; i<skelsmd_num_xyz; i++)
		{ V3F_COPY(skelsmd_xyz+i*3, tmp->vecs+i*3); }
	for(i=0; i<skelsmd_num_st; i++)
		{ V2F_COPY(skelsmd_st+i*2, tmp->stvecs+i*2); }


	//generate vertex -> faces mappings

	tmp->vfidx=gctalloc("lbxgl_skel_smd", tmp->n_vecs*sizeof(int));
	tmp->vfnum=gctalloc("lbxgl_skel_smd", tmp->n_vecs*sizeof(int));

	n=0;
	for(i=0; i<tmp->n_vecs; i++)
	{
		n0=n;

		for(j=0; j<tmp->n_faces; j++)
		{
			face=tmp->face[j];

			for(k=0; k<3; k++)
				if(face->vecs[k]==i)
					break;
			if(k<3)tidx[n++]=j;

			if(n>=65536)
				*(int *)-1=-1;
		}

		tmp->vfidx[i]=n0;
		tmp->vfnum[i]=n-n0;
	}

	tmp->vfids=gctalloc("lbxgl_skel_smd", n*sizeof(int));
	for(i=0; i<n; i++)tmp->vfids[i]=tidx[i];


	//calculate vertex normals
	face=tmp->faces;
	while(face)
	{
		for(i=0; i<3; i++)
		{
			V3F_ZERO(face->nvecs+i*3);

			j=face->vecs[i];
			for(k=0; k<tmp->vfnum[j]; k++)
			{
				l=tmp->vfids[tmp->vfidx[j]+k];
				face2=tmp->face[l];

				f=V3F_DOT(face->norm, face2->norm);
				if(f>=0.75)
				{
					V3F_ADDSCALE(face->nvecs+i*3,
						face2->norm, f,
						face->nvecs+i*3);
				}

				face2=face2->next;
			}

			V3F_NORMALIZE(face->nvecs+i*3, face->nvecs+i*3);
		}

		face=face->next;
	}

	return(tmp);
}

LBXGL_API void LBXGL_SkelSmd_BindMesh(LBXGL_Skel2Mesh *mesh)
{
	float tv[3], tv1[3], bv[3];
	int i, j, k, n, bi;
	float d, bd;

	mesh->vidx=gctalloc("lbxgl_skel_smd", mesh->n_vecs*sizeof(int));
	mesh->vnum=gctalloc("lbxgl_skel_smd", mesh->n_vecs*sizeof(int));

	n=skelsmd_num_xyz;
	mesh->wxyz=gctalloc("lbxgl_skel_smd", 3*n*sizeof(float));
	mesh->widx=gctalloc("lbxgl_skel_smd", n*sizeof(int));
	mesh->wval=gctalloc("lbxgl_skel_smd", n*sizeof(float));

	for(i=0; i<n; i++)
	{
		mesh->vidx[i]=i;
		mesh->vnum[i]=1;

		j=skelsmd_bone[i];
		Vec3F_PointFromChildSpace(skelsmd_xyz+i*3,
			skelsmd_xform+j*16, mesh->wxyz+i*3);
		mesh->widx[i]=j;
		mesh->wval[i]=1;
	}

	mesh->n_weights=n;
}

LBXGL_API int LBXGL_SkelSmd_LoadTriangles(LBXGL_Skel2Model *mdl, VFILE *fd)
{
	float tv0[3], tv1[3], tv2[3];
	float rv0[3], rv1[3], rv2[3];
	float pt[3], io[3];
	float cm[16], im[9];
	LBXGL_Skel2MeshGroup *tmp;

	char *buf, **a, *s;
	int i, j, k, l, n;

	int hl2;
	float cw, bw;

	printf("LBXGL_SkelSmd_LoadTriangles\n");

	for(i=0; i<mdl->num_bones; i++)
	{
		j=mdl->bone_up[i];
		if(j<0)
		{
			V3F_COPY(mdl->bone_org+i*3, pt);
			Mat3F_Copy(mdl->bone_rot+i*9, im);

			Mat3to4F_Copy(im, cm);
			cm[12]=pt[0]; cm[13]=pt[1]; cm[14]=pt[2];
			Mat4F_Copy(cm, skelsmd_xform+i*16);
		}else
		{
			V3F_COPY(mdl->bone_org+i*3, pt);
			Mat3F_Copy(mdl->bone_rot+i*9, im);

			Mat3to4F_Copy(im, cm);
			cm[12]=pt[0]; cm[13]=pt[1]; cm[14]=pt[2];
			Mat4F_MatMult(cm, skelsmd_xform+j*16,
				skelsmd_xform+i*16);
		}
	}


	if(!skelsmd_face_xyz)
	{
		skelsmd_face_xyz=malloc(65536*9*sizeof(short));
		skelsmd_face_st=malloc(65536*6*sizeof(short));
		skelsmd_face_skin=malloc(65536*sizeof(short));
		skelsmd_face_bone=malloc(65536*3*sizeof(short));

		skelsmd_skinname=gctalloc("lbxgl_skel_smd", 256*sizeof(char *));
	}

	skelsmd_num_face=0;
	skelsmd_num_skin=0;

	hl2=0;

	n=0;
	while(!vfeof(fd))
	{
		buf=gcralloc(256);
		memset(buf, 0, 256);
		vfgets(buf, 255, fd);

		a=gcrsplit(buf);
		if(!a[0])continue;

		if(!strcmp(a[0], "end"))
			break;

		i=LBXGL_SkelSmd_IndexSkin(a[0]);
		skelsmd_face_skin[n]=i;

		for(i=0; i<3; i++)
		{
			memset(buf, 0, 256);
			vfgets(buf, 255, fd);
			a=gcrsplit(buf);

			for(j=0; j<3; j++)
			{
				tv0[j]=atof(a[1+j]);
				tv1[j]=atof(a[4+j]);
			}
			for(j=0; j<2; j++)
				tv2[j]=atof(a[7+j]);

			j=atoi(a[0]);
			if(!j && a[9])
			{
				if(!hl2)
				{
					printf("Detect HL2 SMD");
					hl2=1;
				}

				l=atoi(a[9]);
				bw=-1;
				for(k=0; k<l; k++)
				{
					cw=atof(a[10+k*2+1]);
					if(cw>bw)
					{
						j=atoi(a[10+k*2]);
						bw=cw;
					}
				}
			}

			skelsmd_face_bone[n*3+i]=j;
			V3F_COPY(tv0, skelsmd_face_xyz+n*9+i*3);
			V2F_COPY(tv2, skelsmd_face_st+n*6+i*2);
		}

		n++;
	}
	skelsmd_num_face=n;

	tmp=AC3D_CreateMeshGroup();
	tmp->prim=LBXGL_SkelSmd_BuildMesh();
	LBXGL_SkelSmd_BindMesh(tmp->prim);

	mdl->mesh=tmp;

	printf("LBXGL_SkelSmd_LoadTriangles: Done\n");

	return(0);
}

#endif

LBXGL_API int LBXGL_SkelSmd_LoadReference(LBXGL_Skel2Model *mdl, char *name)
{
	VFILE *fd;
	char *buf, **a;
	int i;

	fd=vffopen(name, "rt");
	if(!fd)
	{
		printf("LBXGL_SkelSmd_LoadReference: can't open '%s'\n", name);
		return(-1);
	}

	buf=gcralloc(256);
	vfgets(buf, 255, fd);
	a=gcrsplit(buf);
	if(!a[0] || strcmp(a[0], "version") || !a[1] || (atoi(a[1])!=1))
	{
		printf("LBXGL_SkelSmd_LoadReference: invalid header\n", name);
		return(-1);
	}

	while(!vfeof(fd))
	{
		buf=gcralloc(256);
		memset(buf, 0, 256);
		vfgets(buf, 255, fd);

		a=gcrsplit(buf);
		if(!a[0])continue;

		if(!strcmp(a[0], "nodes"))
			LBXGL_SkelSmd_LoadBaseBones(mdl, fd);
		if(!strcmp(a[0], "skeleton"))
			LBXGL_SkelSmd_LoadBaseFrame(mdl, fd);
		if(!strcmp(a[0], "triangles"))
			LBXGL_SkelSmd_LoadTriangles(mdl, fd);
	}

	vfclose(fd);

	printf("LBXGL_SkelSmd_LoadReference: Done\n");

	return(0);
}

LBXGL_API LBXGL_Skel2Anim *LBXGL_SkelSmd_LoadAnim(LBXGL_Skel2Model *mdl, char *name)
{
	static float frame_org[256*64*3];
	static float frame_rot[256*64*9];
	static byte frame_mask[256*64/8];

	float org[3], ang[3], q[4], rot[9];
	int n_frames, frame_cur;

	LBXGL_Skel2Anim *tmp;

	char buf[256];
	float pos[16];
	char **a;
	VFILE *fd;
	int i, j, k, l;

	fd=vffopen(name, "rt");
	if(!fd)
	{
		printf("can't open anim %s\n", name);
		return(NULL);
	}

	vfgets(buf, 255, fd);
	a=gcrsplit(buf);
	if(!a[0] || strcmp(a[0], "version") || !a[1] || (atoi(a[1])!=1))
	{
		printf("LBXGL_SkelSmd_LoadAnim: invalid header\n", name);
		return(NULL);
	}

	memset(frame_mask, 0, 256*64/8);
	n_frames=0;
	frame_cur=0;

	frame_cur=-1;

	while(!vfeof(fd))
	{
		memset(buf, 0, 256);
		vfgets(buf, 255, fd);
		a=gcrsplit(buf);

		if(!a[0])continue;
		if(!strncmp(a[0], "//", 2))continue;

		if(!strcmp(a[0], "skeleton"))
		{
			frame_cur=0;
			continue;
		}

		if(frame_cur<0)
			continue;

		if(!strcmp(a[0], "end"))
		{
			frame_cur=-1;
			continue;
		}

		if(!strcmp(a[0], "time"))
		{
			frame_cur=atoi(a[1]);
			if((frame_cur+1)>n_frames)n_frames=frame_cur+1;
		}


		if(!((a[0][0]>='0')&&(a[0][0]<='9')))
			continue;

		i=atoi(a[0]);
		for(j=0; j<3; j++)
		{
			org[j]=atof(a[1+j]);
			ang[j]=atof(a[4+j]);
		}

		LBXGL_SkelSmd_FromAngles(ang, q);
		QuatF_To3Matrix(q, rot);

		j=frame_cur*64+i;

		V3F_COPY(org, frame_org+j*3);
//		V3F_COPY(ang, mdl->bone_ang+i*3);
		Mat3F_Copy(rot, frame_rot+j*9);

		frame_mask[j>>3]|=1<<(j&7);
	}
	vfclose(fd);

	tmp=gctalloc("lbxgl_skel_smd", sizeof(LBXGL_Skel2Anim));

	tmp->num_frames=n_frames;
	if(n_frames<64)n_frames=64;

	tmp->frame_org=gctalloc("lbxgl_skel_smd", n_frames*mdl->max_bones*3*sizeof(float));
	tmp->frame_rot=gctalloc("lbxgl_skel_smd", n_frames*mdl->max_bones*9*sizeof(float));
	tmp->frame_mask=gctalloc("lbxgl_skel_smd", (n_frames*mdl->max_bones+7)/8);

	for(i=0; i<tmp->num_frames; i++)
		for(j=0; j<mdl->num_bones; j++)
	{
		k=i*64+j;
		l=i*mdl->max_bones+j;

		V3F_COPY(frame_org+k*3, tmp->frame_org+l*3);

		V3F_COPY(frame_rot+k*9+0, tmp->frame_rot+l*9+0);
		V3F_COPY(frame_rot+k*9+3, tmp->frame_rot+l*9+3);
		V3F_COPY(frame_rot+k*9+6, tmp->frame_rot+l*9+6);

		if(frame_mask[k>>3]&(1<<(k&7)))
			tmp->frame_mask[l>>3]|=1<<(l&7);
	}

	tmp->frame_time=1.0/15.0;

	return(tmp);
}
