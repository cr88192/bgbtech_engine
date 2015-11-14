#include <lbxgl2.h>

LBXGL_API LBXGL_Skel2Mesh *AC3D_CreateMesh()
{
	LBXGL_Skel2Mesh *tmp;

	tmp=gcalloc(sizeof(LBXGL_Skel2Mesh));
	memset(tmp, 0, sizeof(LBXGL_Skel2Mesh));
	tmp->texnum[0]=-1;

	return(tmp);
}

LBXGL_API LBXGL_Skel2MeshGroup *AC3D_CreateMeshGroup()
{
	LBXGL_Skel2MeshGroup *tmp;

	tmp=gcalloc(sizeof(LBXGL_Skel2MeshGroup));
	memset(tmp, 0, sizeof(LBXGL_Skel2MeshGroup));

	return(tmp);
}

LBXGL_API int AC3D_IndexST(LBXGL_Skel2Mesh *prim, float s, float t)
{
	int i;

	for(i=0; i<prim->n_stvecs; i++)
		if((prim->stvecs[i*2+0]==s) && (prim->stvecs[i*2+1]==t))
			return(i);
	prim->stvecs[i*2+0]=s;
	prim->stvecs[i*2+1]=t;
	prim->n_stvecs++;

	return(i);
}

#if 0
// void AC3D_LoadSurface(LBXGL_Skel2Mesh *prim, VFILE *fd)
{
	LBXGL_Skel2MeshFace *face;
	char **a, *buf;
	float tv0[3], tv1[3], tv2[3];
	float s, t;
	int i, n, v;

	face=gcalloc(sizeof(LBXGL_Skel2MeshFace));
	memset(face, 0, sizeof(LBXGL_Skel2MeshFace));

	while(!vfeof(fd))
	{
		buf=gcralloc(256);
		vfgets(buf, 255, fd);

		a=gcrsplit(buf);

		if(!strcmp(a[0], "refs"))
		{
			n=atoi(a[1]);
			face->n_vecs=n;

			for(i=0; i<n; i++)
			{
				vfgets(buf, 255, fd);
				sscanf(buf, "%d %f %f", &v, &s, &t);

				face->vecs[i]=v;
				face->stvecs[i]=AC3D_IndexST(prim, s, t);
			}

			break;
		}
	}

	V3F_SUB(prim->vecs+face->vecs[1]*3, prim->vecs+face->vecs[0]*3, tv0);
	V3F_SUB(prim->vecs+face->vecs[2]*3, prim->vecs+face->vecs[1]*3, tv1);
	V3F_CROSS(tv0, tv1, tv2);
	V3F_NORMALIZE(tv2, face->norm);

	face->next=prim->faces;
	prim->faces=face;
}

// int AC3D_LoadSurfaces(LBXGL_Skel2Mesh *prim, int num, VFILE *fd)
{
	static int tidx[65536];
	LBXGL_Skel2MeshFace *fcur, *fcur2;
	char **a, *buf;
	int i, j, k, l, n, n0;
	float f;

	prim->face=gcalloc(num*sizeof(LBXGL_Skel2MeshFace *));

	i=0; n=num;
	while(!vfeof(fd) && n)
	{
		buf=gcralloc(256);
		vfgets(buf, 255, fd);
		a=gcrsplit(buf);

		if(!strcmp(a[0], "SURF"))
		{
			AC3D_LoadSurface(prim, fd);
			prim->face[i++]=prim->faces;
			n--;
		}
	}
	prim->n_faces=num;


	//generate vertex -> faces mappings

	prim->vfidx=gcalloc(prim->n_vecs*sizeof(int));
	prim->vfnum=gcalloc(prim->n_vecs*sizeof(int));

	n=0;
	for(i=0; i<prim->n_vecs; i++)
	{
		n0=n;

		for(j=0; j<prim->n_faces; j++)
		{
			fcur=prim->face[j];

			for(k=0; k<fcur->n_vecs; k++)
				if(fcur->vecs[k]==i)
					break;
			if(k<fcur->n_vecs)tidx[n++]=j;

			if(n>=65536)
				*(int *)-1=-1;
		}

		prim->vfidx[i]=n0;
		prim->vfnum[i]=n-n0;
	}

	prim->vfids=gcalloc(n*sizeof(int));
	for(i=0; i<n; i++)prim->vfids[i]=tidx[i];


	//calculate vertex normals
	fcur=prim->faces;
	while(fcur)
	{
		for(i=0; i<fcur->n_vecs; i++)
		{
			V3F_ZERO(fcur->nvecs+i*3);

			j=fcur->vecs[i];
			for(k=0; k<prim->vfnum[j]; k++)
			{
				l=prim->vfids[prim->vfidx[j]+k];
				fcur2=prim->face[l];

				f=V3F_DOT(fcur->norm, fcur2->norm);
				if(f>=0.75)
				{
					V3F_ADDSCALE(fcur->nvecs+i*3,
						fcur2->norm, f,
						fcur->nvecs+i*3);
				}

				fcur2=fcur2->next;
			}

			V3F_NORMALIZE(fcur->nvecs+i*3, fcur->nvecs+i*3);
		}

		fcur=fcur->next;
	}

#if 0
	fcur=prim->faces;
	while(fcur)
	{
		for(i=0; i<fcur->n_vecs; i++)
		{
			V3F_ZERO(fcur->nvecs+i*3);

			fcur2=prim->faces;
			while(fcur2)
			{
#if 0
				if(fcur==fcur2)
				{
					fcur2=fcur2->next;
					continue;
				}
#endif

				for(j=0; j<fcur2->n_vecs; j++)
					if(fcur->vecs[i]==fcur2->vecs[j])
						break;

				f=V3F_DOT(fcur->norm, fcur2->norm);
				if((j<fcur2->n_vecs) && (f>=0.75))
				{
					V3F_ADDSCALE(fcur->nvecs+i*3,
						fcur2->norm, f,
						fcur->nvecs+i*3);
				}

				fcur2=fcur2->next;
			}

			V3F_NORMALIZE(fcur->nvecs+i*3, fcur->nvecs+i*3);
		}

		fcur=fcur->next;
	}
#endif

	return(0);
}

#endif

LBXGL_API void AC3D_LoadSurface(LBXGL_Skel2Mesh *prim, VFILE *fd)
{
	LBXGL_Skel2MeshFace *face;
	char **a, *buf;
	int vecs[16], stvecs[16];
	float tv0[3], tv1[3], tv2[3];
	float s, t;
	int i, n, v, nv;

	while(!vfeof(fd))
	{
		buf=gcralloc(256);
		vfgets(buf, 255, fd);

		a=gcrsplit(buf);

		if(!strcmp(a[0], "refs"))
		{
			n=atoi(a[1]);
			for(i=0; i<n; i++)
			{
				vfgets(buf, 255, fd);
				sscanf(buf, "%d %f %f", &v, &s, &t);
				vecs[i]=v;
				stvecs[i]=AC3D_IndexST(prim, s, t);
			}

			break;
		}
	}

	for(i=1; i<(n-1); i++)
	{
		face=gcalloc(sizeof(LBXGL_Skel2MeshFace));
		memset(face, 0, sizeof(LBXGL_Skel2MeshFace));

//		face->n_vecs=3;
		face->vecs[0]=vecs[0];
		face->vecs[2]=vecs[i];
		face->vecs[1]=vecs[i+1];

		face->stvecs[0]=stvecs[0];
		face->stvecs[2]=stvecs[i];
		face->stvecs[1]=stvecs[i+1];

		V3F_SUB(prim->vecs+face->vecs[1]*3,
			prim->vecs+face->vecs[0]*3, tv0);
		V3F_SUB(prim->vecs+face->vecs[2]*3,
			prim->vecs+face->vecs[1]*3, tv1);
		V3F_CROSS(tv0, tv1, tv2);
		V3F_NORMALIZE(tv2, face->norm);

		face->next=prim->faces;
		prim->faces=face;
	}
}

LBXGL_API int AC3D_LoadSurfaces(LBXGL_Skel2Mesh *prim, int num, VFILE *fd)
{
	static int tidx[65536];
	LBXGL_Skel2MeshFace *fcur, *fcur2;
	char **a, *buf;
	int i, j, k, l, n, n0;
	float f;


	i=0; n=num;
	while(!vfeof(fd) && n)
	{
		buf=gcralloc(256);
		vfgets(buf, 255, fd);
		a=gcrsplit(buf);

		if(!strcmp(a[0], "SURF"))
		{
			AC3D_LoadSurface(prim, fd);
//			prim->face[i++]=prim->faces;
			n--;
		}
	}


	//generate array of faces

	n=0; fcur=prim->faces;
	while(fcur) { n++; fcur=fcur->next; }

	prim->n_faces=n;
	prim->face=gcalloc(n*sizeof(LBXGL_Skel2MeshFace *));

	n=0;
	fcur=prim->faces;
	while(fcur)
	{
		if(fcur->next)fcur->tnext=n+1;
		prim->face[n++]=fcur;
		fcur=fcur->next;
	}
	prim->texface[0]=0;


	//generate vertex -> faces mappings

	prim->vfidx=gcalloc(prim->n_vecs*sizeof(int));
	prim->vfnum=gcalloc(prim->n_vecs*sizeof(int));

	n=0;
	for(i=0; i<prim->n_vecs; i++)
	{
		n0=n;

		for(j=0; j<prim->n_faces; j++)
		{
			fcur=prim->face[j];

//			for(k=0; k<fcur->n_vecs; k++)
			for(k=0; k<3; k++)
				if(fcur->vecs[k]==i)
					break;
//			if(k<fcur->n_vecs)tidx[n++]=j;
			if(k<3)tidx[n++]=j;

			if(n>=65536)
				*(int *)-1=-1;
		}

		prim->vfidx[i]=n0;
		prim->vfnum[i]=n-n0;
	}

	prim->vfids=gcalloc(n*sizeof(int));
	for(i=0; i<n; i++)prim->vfids[i]=tidx[i];


	//calculate vertex normals
	fcur=prim->faces;
	while(fcur)
	{
//		for(i=0; i<fcur->n_vecs; i++)
		for(i=0; i<3; i++)
		{
			V3F_ZERO(fcur->nvecs+i*3);

			j=fcur->vecs[i];
			for(k=0; k<prim->vfnum[j]; k++)
			{
				l=prim->vfids[prim->vfidx[j]+k];
				fcur2=prim->face[l];

				f=V3F_DOT(fcur->norm, fcur2->norm);
				if(f>=0.75)
				{
					V3F_ADDSCALE(fcur->nvecs+i*3,
						fcur2->norm, f,
						fcur->nvecs+i*3);
				}

				fcur2=fcur2->next;
			}

			V3F_NORMALIZE(fcur->nvecs+i*3, fcur->nvecs+i*3);
		}

		fcur=fcur->next;
	}

	return(0);
}

LBXGL_API char *AC3D_GenAbs(char *base, char *name)
{
	char buf[256];
	char *s;

	s=name;
	if(*s=='/')return(name);
	if(*s=='\\')return(name);

	while(*s && (*s!='/') && (*s!='\\'))s++;
	if(*s==':')return(name);

	sprintf(buf, "%s/%s", base, name);
	name=gcrstrdup(buf);

	return(name);
}

LBXGL_API LBXGL_Skel2Mesh *AC3D_LoadPolyObject(VFILE *fd, float *offs, char *base)
{
	static float stvecs[65536*2];
	LBXGL_Skel2Mesh *tmp;
	float *vecs;
	char **a, *buf;
	int i, n, nk;

	float vec[3], roffs[3];

	tmp=AC3D_CreateMesh();

	roffs[0]=offs[0];
	roffs[1]=offs[1];
	roffs[2]=offs[2];

	nk=0;
	while(!vfeof(fd))
	{
		buf=gcralloc(256);
		vfgets(buf, 255, fd);

		a=gcrsplit(buf);

		if(!strcmp(a[0], "kids"))
		{
			nk=atoi(a[1]);
			break;
		}

		if(!strcmp(a[0], "loc"))
		{
			roffs[0]+=atof(a[1]);
			roffs[1]+=atof(a[2]);
			roffs[2]+=atof(a[3]);
		}

		if(!strcmp(a[0], "texture"))
		{
			tmp->n_tex=1;
			tmp->texname[0]=dystrdup(a[1]);
			tmp->texnum[0]=LBXGL_Texture_LoadImage(tmp->texname[0]);
			if(tmp->texnum<0)
			{
				buf=AC3D_GenAbs(base, a[1]);
				tmp->texnum[0]=Tex_LoadFile(buf, NULL, NULL);
			}
		}

		if(!strcmp(a[0], "name"))
			tmp->name=dystrdup(a[1]);

		if(!strcmp(a[0], "data"))
		{
			i=atoi(a[1]);
			buf=gcralloc(i+1);
			vfread(buf, 1, i, fd);
		}

		if(!strcmp(a[0], "numvert"))
		{
			n=atoi(a[1]);
			vecs=gcalloc(n*3*sizeof(float));

			tmp->n_vecs=n;
			tmp->vecs=vecs;

			for(i=0; i<n; i++)
			{
				vfgets(buf, 255, fd);
				sscanf(buf, "%f %f %f",
					&vecs[(i*3)+0],
					&vecs[(i*3)+2],
					&vecs[(i*3)+1]);

//				vecs[(i*3)+1]=-vecs[(i*3)+1];

				vecs[(i*3)+0]+=roffs[0];
				vecs[(i*3)+2]+=roffs[1];
				vecs[(i*3)+1]+=roffs[2];
			}
		}
		if(!strcmp(a[0], "numsurf"))
		{
			tmp->stvecs=stvecs;
			tmp->n_stvecs=0;

			n=atoi(a[1]);
			AC3D_LoadSurfaces(tmp, n, fd);

			vecs=gcalloc(tmp->n_stvecs*2*sizeof(float));
			memcpy(vecs, stvecs, tmp->n_stvecs*2*sizeof(float));
			tmp->stvecs=vecs;
		}
	}

	if(!tmp->n_tex)
	{
		tmp->n_tex=1;
		tmp->texname[0]=dystrdup("");
		tmp->texnum[0]=-1;
	}

	while(!vfeof(fd) && nk)
	{
		buf=gcralloc(256);
		vfgets(buf, 255, fd);

		a=gcrsplit(buf);

		if(!strcmp(a[0], "OBJECT"))
		{
			if(!strcmp(a[1], "group"))
			{
				AC3D_LoadGroupObject(fd, roffs, base);
			}else if(!strcmp(a[1], "poly"))
			{
				AC3D_LoadPolyObject(fd, roffs, base);
//				MergeWithMesh(tmp, t2);
			}

			nk--;
		}
	}

	return(tmp);
}

LBXGL_API LBXGL_Skel2MeshGroup *AC3D_LoadGroupObject(VFILE *fd, float *offs, char *base)
{
	LBXGL_Skel2MeshGroup *tmp, *grp;
	LBXGL_Skel2Mesh *prim;
	char **a, *buf;
	int i, n, nk;
	char *name, *data;
	float roffs[3];

	tmp=AC3D_CreateMeshGroup();

	roffs[0]=offs[0];
	roffs[1]=offs[1];
	roffs[2]=offs[2];

	nk=0;
	while(!vfeof(fd))
	{
		buf=gcralloc(256);
		vfgets(buf, 255, fd);

		a=gcrsplit(buf);

		if(!strcmp(a[0], "kids"))
		{
			nk=atoi(a[1]);
			break;
		}

		if(!strcmp(a[0], "loc"))
		{
			roffs[0]+=atof(a[1]);
			roffs[1]+=atof(a[2]);
			roffs[2]+=atof(a[3]);
		}

		if(!strcmp(a[0], "name"))
			tmp->name=dystrdup(a[1]);

		if(!strcmp(a[0], "data"))
		{
			i=atoi(a[1]);
			data=gcralloc(i+1);
			vfread(data, 1, i, fd);
		}
	}

	while(!vfeof(fd) && nk)
	{
		buf=gcralloc(256);
		vfgets(buf, 255, fd);

		a=gcrsplit(buf);

		if(!strcmp(a[0], "OBJECT"))
		{
			if(!strcmp(a[1], "group"))
			{
				grp=AC3D_LoadGroupObject(fd, roffs, base);

				grp->next=tmp->first;
				tmp->first=grp;
			}else if(!strcmp(a[1], "poly"))
			{
				prim=AC3D_LoadPolyObject(fd, roffs, base);
				prim->next=tmp->prim;
				tmp->prim=prim;
			}

			nk--;
		}
	}

	return(tmp);
}

LBXGL_API LBXGL_Skel2MeshGroup *AC3D_LoadModel(char *name)
{
	VFILE *fd;
	char *buf, *base, *t;
	LBXGL_Skel2MeshGroup *tmp, *grp;
	char **a;
	float *matcol;
	int mats;
	float org[3];

	buf=gcralloc(256);
	strcpy(buf, name);
	t=buf+strlen(buf);
	while((t>buf) && (*t!='/'))t--;
	*t=0;
	base=dystrdup(buf);

	printf("AC3D_LoadModel: loading '%s'\n", name);

	fd=vffopen(name, "rt");

	if(!fd)
	{
		printf("AC3D_LoadModel: can't open '%s'\n", name);
		return(NULL);
	}

	tmp=NULL;

	vfgets(buf, 255, fd);
	if(strncmp(buf, "AC3Db", 5))
	{
		printf("AC3D_LoadModel: bad model '%s'\n", name);
		return(tmp);
	}

	while(!vfeof(fd))
	{
		buf=gcralloc(256);
		vfgets(buf, 255, fd);

		a=gcrsplit(buf);
		if(!a[0])continue;

		if(!strcmp(a[0], "MATERIAL"))
		{
		}

		if(!strcmp(a[0], "OBJECT"))
		{
			V3F_ZERO(org);
			tmp=AC3D_LoadGroupObject(fd, org, base);
			break;
		}
	}

	if(!tmp)
	{
		printf("AC3D_LoadModel: no root object for '%s'\n", name);
		return(tmp);
	}

	return(tmp);
}

LBXGL_API LBXGL_Skel2MeshGroup *AC3D_LoadModelPos(char *name, float *pos)
{
	LBXGL_Skel2MeshGroup *tmp;

	tmp=AC3D_LoadModel(name);
	AC3D_MoveMeshGroup(tmp, pos);
	return(tmp);
}

LBXGL_API void AC3D_MoveMesh(LBXGL_Skel2Mesh *mesh, float *pos)
{
	LBXGL_Skel2MeshFace *cur;
	float tv[3];
	int i;

	for(i=0; i<mesh->n_vecs; i++)
	{
		Mat4F_TransformPoint(mesh->vecs+(3*i), pos, tv);
		V3F_COPY(tv, mesh->vecs+(3*i));
	}
}

LBXGL_API void AC3D_MoveMeshGroup(LBXGL_Skel2MeshGroup *grp, float *pos)
{
	LBXGL_Skel2MeshGroup *gcur;
	LBXGL_Skel2Mesh *mcur;

	gcur=grp->first;
	while(gcur)
	{
		AC3D_MoveMeshGroup(gcur, pos);
		gcur=gcur->next;
	}

	mcur=grp->prim;
	while(mcur)
	{
		AC3D_MoveMesh(mcur, pos);
		mcur=mcur->next;
	}
}


#if 0
// LBXGL_API void AC3D_DrawMesh(LBXGL_Skel2Mesh *mesh, float *pos)
{
	LBXGL_Skel2MeshFace *cur;
	float tv[3];
	int i;

	glDisable(GL_TEXTURE_2D);

	if(mesh->texnum>0)
	{
		glEnable(GL_TEXTURE_2D);
		glBindTexture(GL_TEXTURE_2D, mesh->texnum);
	}

	cur=mesh->faces;
	while(cur)
	{
		glBegin(GL_POLYGON);
		for(i=0; i<cur->n_vecs; i++)
		{
			glTexCoord2fv(mesh->stvecs+(2*cur->stvecs[i]));
			Mat4F_TransformPoint(mesh->vecs+(3*cur->vecs[i]),
				pos, tv);
			glVertex3fv(tv);
		}
		glEnd();

		cur=cur->next;
	}
}
#endif

#if 1
LBXGL_API void AC3D_DrawMesh(LBXGL_Skel2Mesh *mesh, float *pos)
{
	static float txyz[65536*3];
	static float tnorm[16384*3];
	LBXGL_Skel2MeshFace *cur, *cur2;
	float tv[3], tv0[3], tv1[3], tv2[3];
	float *v0, *v1, *imv;
	int i, j, k, l, l1;
	float f, g;

	//calculate vertex poses
	for(i=0; i<mesh->n_vecs; i++)
		Mat4F_TransformPoint(mesh->vecs+(3*i), pos, txyz+i*3);

	//calc face normals
	for(i=0; i<mesh->n_faces; i++)
	{
		cur=mesh->face[i];
		V3F_SUB(txyz+cur->vecs[1]*3, txyz+cur->vecs[0]*3, tv0);
		V3F_SUB(txyz+cur->vecs[2]*3, txyz+cur->vecs[1]*3, tv1);
		V3F_CROSS(tv0, tv1, tv2);
		V3F_NORMALIZE(tv2, tnorm+i*3);
	}

	glDisable(GL_TEXTURE_2D);

	if(mesh->texnum[0]>0)
	{
		glEnable(GL_TEXTURE_2D);
		glBindTexture(GL_TEXTURE_2D, mesh->texnum[0]);
	}

	for(i=0; i<mesh->n_faces; i++)
	{
		cur=mesh->face[i];

		glNormal3fv(tnorm+i*3);

		glBegin(GL_POLYGON);
//		for(j=0; j<cur->n_vecs; j++)
		for(j=0; j<3; j++)
		{
			V3F_ZERO(tv);
			k=cur->vecs[j];
			for(l=0; l<mesh->vfnum[k]; l++)
			{
				l1=mesh->vfids[mesh->vfidx[k]+l];
				f=V3F_DOT(tnorm+i*3, tnorm+l1*3);
//				f=1;

				if(f>1)f=1;
				if(f>0)
				{
					V3F_ADDSCALE(tv, tnorm+l1*3, f, tv);
				}
			}
			glNormal3fv(tv);

			glTexCoord2fv(mesh->stvecs+(2*cur->stvecs[j]));
			V3F_COPY(txyz+(3*cur->vecs[j]), tv);

//			f=V3F_DIST(tv, main_cam_org)/12.0;
//			glColor4f(1.0/f, 1.0/f, 1.0/f, 1);

			glVertex3fv(tv);
		}
		glEnd();

		cur=cur->next;
	}
}
#endif

LBXGL_API void AC3D_DrawMeshGroup(LBXGL_Skel2MeshGroup *grp, float *pos)
{
	LBXGL_Skel2MeshGroup *gcur;
	LBXGL_Skel2Mesh *mcur;

	gcur=grp->first;
	while(gcur)
	{
		AC3D_DrawMeshGroup(gcur, pos);
		gcur=gcur->next;
	}

	mcur=grp->prim;
	while(mcur)
	{
		AC3D_DrawMesh(mcur, pos);
		mcur=mcur->next;
	}
}
