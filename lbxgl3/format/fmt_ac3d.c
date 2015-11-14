#include <lbxgl2.h>

int LBXGL_AC3D_IndexXYZ(LBXGL_MeshPrim *prim, float x, float y, float z)
{
	float *xyz;
	int i;

	if(!prim->xyz)
	{
		prim->n_xyz=0;
		prim->m_xyz=16;
		prim->xyz=gctatomic("lbxgl_ac3d_xyz", prim->m_xyz*3*sizeof(float));
	}

	for(i=0; i<prim->n_xyz; i++)
		if((fabs(prim->xyz[i*3+0]-x)<0.0001) &&
			(fabs(prim->xyz[i*3+1]-y)<0.0001) &&
			(fabs(prim->xyz[i*3+2]-z)<0.0001))
				return(i);

	if(prim->n_xyz>=prim->m_xyz)
	{
		i=prim->m_xyz+(prim->m_xyz>>1);
//		xyz=gcatomic(i*3*sizeof(float));
//		memcpy(xyz, prim->xyz, prim->m_xyz*3*sizeof(float));
//		gcfree(prim->xyz);
//		prim->xyz=xyz;
		prim->xyz=gcrealloc(prim->xyz, i*3*sizeof(float));
		prim->m_xyz=i;
	}

	i=prim->n_xyz++;
	prim->xyz[i*3+0]=x;
	prim->xyz[i*3+1]=y;
	prim->xyz[i*3+2]=z;

	return(i);
}

int LBXGL_AC3D_IndexST(LBXGL_MeshPrim *prim, float s, float t)
{
	float *st;
	int i;

	if(!prim->st)
	{
		prim->n_st=0;
		prim->m_st=16;
		prim->st=gctatomic("lbxgl_ac3d_st", prim->m_st*2*sizeof(float));
	}

	for(i=0; i<prim->n_st; i++)
		if((fabs(prim->st[i*2+0]-s)<0.0001) &&
			(fabs(prim->st[i*2+1]-t)<0.0001))
				return(i);

	if(prim->n_st>=prim->m_st)
	{
		i=prim->m_st+(prim->m_st>>1);
//		st=gcatomic(i*2*sizeof(float));
//		memcpy(st, prim->st, prim->m_st*2*sizeof(float));
//		gcfree(prim->st);
//		prim->st=st;
		prim->st=gcrealloc(prim->st, i*2*sizeof(float));
		prim->m_st=i;
	}

	i=prim->n_st++;
	prim->st[i*2+0]=s;
	prim->st[i*2+1]=t;

	return(i);
}

void LBXGL_AC3D_LoadSurface(LBXGL_MeshPrim *prim, VFILE *fd)
{
	char **a, *buf;
	int *pts;
	float s, t;
	int i, j, n, v, nv, pi;

	if(!prim->pts)
	{
		prim->n_pts=0;
		prim->m_pts=16;
		prim->pts=gctatomic("lbxgl_ac3d_pts", prim->m_pts*2*sizeof(int));
	}

	if(!prim->face)
	{
		prim->n_face=0;
		prim->m_face=8;
		prim->face=gctatomic("lbxgl_ac3d_face", prim->m_face*2*sizeof(int));
	}

	if(prim->n_face>=prim->m_face)
	{
		i=prim->m_face+(prim->m_face>>1);
//		pts=gcatomic(i*2*sizeof(int));
//		memcpy(pts, prim->face, prim->m_face*2*sizeof(int));
//		gcfree(prim->face);
//		prim->face=pts;
		prim->face=gcrealloc(prim->face, i*2*sizeof(int));
		prim->m_face=i;
	}

	n=0;

	while(!vfeof(fd))
	{
		buf=gcralloc(256);
		vfgets(buf, 255, fd);

		a=gcrsplit(buf);

		if(!strcmp(a[0], "refs"))
		{
			n=atoi(a[1]);

			while((prim->n_pts+n)>=prim->m_pts)
			{
//				i=prim->m_pts+(prim->m_pts>>1);
//				pts=gcatomic(i*2*sizeof(int));
//				memcpy(pts, prim->pts,
//					prim->m_pts*2*sizeof(int));
//				gcfree(prim->pts);
//				prim->pts=pts;
				i=prim->m_pts;
				while((prim->n_pts+n)>=i)
					i=i+(i>>1);
				prim->pts=gcrealloc(prim->pts, i*2*sizeof(int));
				prim->m_pts=i;
			}

			j=prim->n_pts;
			for(i=0; i<n; i++)
			{
				vfgets(buf, 255, fd);
				sscanf(buf, "%d %f %f", &v, &s, &t);
				prim->pts[(j+i)*2+0]=v;
				prim->pts[(j+i)*2+1]=
					LBXGL_AC3D_IndexST(prim, s, t);
			}

			break;
		}
	}

	if(!n)return;

	i=prim->n_face++;
	prim->face[i*2+0]=prim->n_pts;
	prim->face[i*2+1]=n;
	prim->n_pts+=n;
}

int LBXGL_AC3D_LoadSurfaces(LBXGL_MeshPrim *prim, int num, VFILE *fd)
{
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
			LBXGL_AC3D_LoadSurface(prim, fd);
			n--;
		}
	}

	return(0);
}



char *LBXGL_AC3D_GenAbs(char *base, char *name)
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

LBXGL_MeshPrim *LBXGL_AC3D_LoadPolyObject(VFILE *fd, float *offs, char *base)
{
	LBXGL_MeshPrim *tmp;
	float *vecs;
	char **a, *buf;
	int i, n, nk;

	float vec[3], roffs[3];

	tmp=gctalloc("lbxgl_meshprim_t", sizeof(LBXGL_MeshPrim));

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
			continue;
		}

		if(!strcmp(a[0], "texture"))
		{
			tmp->tex=dystrdup(a[1]);
			tmp->texnum=LBXGL_Texture_LoadImage(tmp->tex);
			if(tmp->texnum<0)
			{
				buf=AC3D_GenAbs(base, a[1]);
//				tmp->texnum=Tex_LoadFile(buf, NULL, NULL);
				tmp->texnum=LBXGL_Texture_LoadImage(buf);
			}
			continue;
		}

		if(!strcmp(a[0], "name"))
		{
			tmp->name=dystrdup(a[1]);
			continue;
		}

		if(!strcmp(a[0], "data"))
		{
			i=atoi(a[1]);
			buf=gcralloc(i+1);
			vfread(buf, 1, i, fd);
			continue;
		}

		if(!strcmp(a[0], "numvert"))
		{
			n=atoi(a[1]);

			i=16;
			while(i<=n)i=i+(i>>1);
			vecs=gctatomic("lbxgl_ac3d_xyz", i*3*sizeof(float));

			tmp->n_xyz=n;
			tmp->m_xyz=i;
			tmp->xyz=vecs;

			for(i=0; i<n; i++)
			{
				vfgets(buf, 255, fd);
				sscanf(buf, "%f %f %f",
					&vecs[(i*3)+0],
					&vecs[(i*3)+2],
					&vecs[(i*3)+1]);

				vecs[(i*3)+1]=-vecs[(i*3)+1];

				vecs[(i*3)+0]+=roffs[0];
				vecs[(i*3)+2]+=roffs[1];
				vecs[(i*3)+1]+=roffs[2];
			}
			continue;
		}
		if(!strcmp(a[0], "numsurf"))
		{
			n=atoi(a[1]);
			LBXGL_AC3D_LoadSurfaces(tmp, n, fd);
			continue;
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
				LBXGL_AC3D_LoadGroupObject(fd, roffs, base);
			}else if(!strcmp(a[1], "poly"))
			{
				LBXGL_AC3D_LoadPolyObject(fd, roffs, base);
			}

			nk--;
		}
	}

	return(tmp);
}

LBXGL_MeshGroup *LBXGL_AC3D_LoadGroupObject(VFILE *fd, float *offs, char *base)
{
	LBXGL_MeshGroup *tmp, *grp;
	LBXGL_MeshPrim *prim;
	char **a, *buf;
	int i, n, nk;
	char *name, *data;
	float roffs[3];

	tmp=gctalloc("lbxgl_meshgroup_t", sizeof(LBXGL_MeshGroup));

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
			continue;
		}

		if(!strcmp(a[0], "name"))
		{
			tmp->name=dystrdup(a[1]);
			continue;
		}

		if(!strcmp(a[0], "data"))
		{
			i=atoi(a[1]);
			data=gcralloc(i+1);
			vfread(data, 1, i, fd);
			continue;
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
				grp=LBXGL_AC3D_LoadGroupObject(fd, roffs, base);
				grp->next=tmp->first;
				tmp->first=grp;
			}else if(!strcmp(a[1], "poly"))
			{
				prim=LBXGL_AC3D_LoadPolyObject(fd, roffs, base);
				prim->next=tmp->prim;
				tmp->prim=prim;
			}

			nk--;
		}
	}

	return(tmp);
}

LBXGL_MeshGroup *LBXGL_AC3D_LoadModel(char *name)
{
	VFILE *fd;
	char *buf, *base, *t;
	LBXGL_MeshGroup *tmp, *grp;
	char **a;
	float org[3];

	buf=gcralloc(256);
	strcpy(buf, name);
	t=buf+strlen(buf);
	while((t>buf) && (*t!='/'))t--;
	*t=0;
	base=dystrdup(buf);

	printf("LBXGL_AC3D_LoadModel: loading '%s'\n", name);

//	fd=vffopen(name, "rt");
	fd=btcl_vffopen(name, "rt");
	if(!fd)
	{
		printf("LBXGL_AC3D_LoadModel: can't open '%s'\n", name);
		return(NULL);
	}

	tmp=NULL;

	vfgets(buf, 255, fd);
	if(strncmp(buf, "AC3Db", 5))
	{
		printf("LBXGL_AC3D_LoadModel: bad model '%s'\n", name);
		return(tmp);
	}

	V3F_ZERO(org);
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
			tmp=LBXGL_AC3D_LoadGroupObject(fd, org, base);
			break;
		}
	}

	if(!tmp)
	{
		printf("LBXGL_AC3D_LoadModel: no root object for '%s'\n", name);
		return(tmp);
	}

	return(tmp);
}



void LBXGL_AC3D_SaveSurface(LBXGL_MeshPrim *prim, VFILE *fd)
{
}

int LBXGL_AC3D_SavePrim(LBXGL_MeshPrim *prim, VFILE *fd)
{
	int i, j, k, p, n;

	prim=LBXGL_Mesh_ClonePrim(prim);
	LBXGL_Mesh_OptimizePrim(prim);

	vfprint(fd, "OBJECT poly\n");

	if(prim->name)vfprint(fd, "name \"%s\"\n", prim->name);
	if(prim->tex)vfprint(fd, "texture \"%s\"\n", prim->tex);

	vfprint(fd, "numvert %d\n", prim->n_xyz);
	for(i=0; i<prim->n_xyz; i++)
	{
		vfprint(fd, "%f %f %f\n",
			prim->xyz[i*3+0], prim->xyz[i*3+2], -prim->xyz[i*3+1]);
	}

	vfprint(fd, "numsurf %d\n", prim->n_face);
	for(i=0; i<prim->n_face; i++)
	{
		vfprint(fd, "SURF %d\n", 0);
		vfprint(fd, "mat %d\n", 0);

		p=prim->face[i*2+0];
		n=prim->face[i*2+1];

		vfprint(fd, "refs %d\n", n);
		for(j=0; j<n; j++)
		{
			k=prim->pts[(p+j)*2+1];
			vfprint(fd, "%d %f %f\n", prim->pts[(p+j)*2+0],
				prim->st[k*2+0], prim->st[k*2+1]);
		}
	}

	vfprint(fd, "kids 0\n");
	LBXGL_Mesh_DestroyPrim(prim);
	return(0);
}

int LBXGL_AC3D_SaveGroup(LBXGL_MeshGroup *grp, VFILE *fd)
{
	LBXGL_MeshGroup *gcur;
	LBXGL_MeshPrim *mcur;
	int n;

	vfprint(fd, "OBJECT group\n");

	n=0;
	gcur=grp->first;
	while(gcur)
	{
		if(gcur->fl&128) { gcur=gcur->next; continue; }
		gcur=gcur->next; n++;
	}
	mcur=grp->prim;
	while(mcur)
	{
		if(mcur->fl&128) { mcur=mcur->next; continue; }
		mcur=mcur->next; n++;
	}

	if(grp->name)vfprint(fd, "name \"%s\"\n", grp->name);

	vfprint(fd, "kids %d\n", n);

	gcur=grp->first;
	while(gcur)
	{
		if(gcur->fl&128) { gcur=gcur->next; continue; }
		LBXGL_AC3D_SaveGroup(gcur, fd);
		gcur=gcur->next;
	}

	mcur=grp->prim;
	while(mcur)
	{
		if(mcur->fl&128) { mcur=mcur->next; continue; }
		LBXGL_AC3D_SavePrim(mcur, fd);
		mcur=mcur->next;
	}
	return(0);
}

int LBXGL_AC3D_SaveModel(LBXGL_MeshGroup *grp, char *name)
{
	LBXGL_MeshGroup *gcur;
	LBXGL_MeshPrim *mcur;
	VFILE *fd;
	int n;

	printf("LBXGL_AC3D_SaveModel: %s\n", name);

//	fd=vffopen(name, "wt");
	fd=btcl_vffopen(name, "wt");
	if(!fd)
	{
		printf("fail open %s\n", name);
		return(-1);
	}

	vfprint(fd, "AC3Db\n");
	vfprint(fd, "MATERIAL \"\" rgb 1 1 1  amb 0.2 0.2 0.2  "
		"emis 0 0 0  spec 0.5 0.5 0.5  shi 10  trans 0\n");

	vfprint(fd, "OBJECT world\n");


	n=0;
	gcur=grp->first;
	while(gcur)
	{
		if(gcur->fl&128) { gcur=gcur->next; continue; }
		gcur=gcur->next; n++;
	}
	mcur=grp->prim;
	while(mcur)
	{
		if(mcur->fl&128) { mcur=mcur->next; continue; }
		mcur=mcur->next; n++;
	}

	vfprint(fd, "kids %d\n", n);

	gcur=grp->first;
	while(gcur)
	{
		if(gcur->fl&128) { gcur=gcur->next; continue; }
		LBXGL_AC3D_SaveGroup(gcur, fd);
		gcur=gcur->next;
	}

	mcur=grp->prim;
	while(mcur)
	{
		if(mcur->fl&128) { mcur=mcur->next; continue; }
		LBXGL_AC3D_SavePrim(mcur, fd);
		mcur=mcur->next;
	}

	vfclose(fd);
	return(0);
}
