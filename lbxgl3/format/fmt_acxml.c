#include <lbxgl2.h>

int LBXGL_ACXML_IndexXYZ(LBXGL_MeshPrim *prim, float x, float y, float z)
{
	float *xyz;
	int i;

	if(!prim->xyz)
	{
		prim->n_xyz=0;
		prim->m_xyz=16;
		prim->xyz=kalloc(prim->m_xyz*3*sizeof(float));
	}

	for(i=0; i<prim->n_xyz; i++)
		if((fabs(prim->xyz[i*3+0]-x)<0.0001) &&
			(fabs(prim->xyz[i*3+1]-y)<0.0001) &&
			(fabs(prim->xyz[i*3+2]-z)<0.0001))
				return(i);

	if(prim->n_xyz>=prim->m_xyz)
	{
		i=prim->m_xyz+(prim->m_xyz>>1);
		xyz=kalloc(i*3*sizeof(float));
		memcpy(xyz, prim->xyz, prim->m_xyz*3*sizeof(float));
		kfree(prim->xyz);
		prim->xyz=xyz;
		prim->m_xyz=i;
	}

	prim->xyz[i*3+0]=x;
	prim->xyz[i*3+1]=y;
	prim->xyz[i*3+2]=z;
	prim->n_xyz++;

	return(i);
}

int LBXGL_ACXML_IndexST(LBXGL_MeshPrim *prim, float s, float t)
{
	float *st;
	int i;

	if(!prim->st)
	{
		prim->n_st=0;
		prim->m_st=16;
		prim->st=kalloc(prim->m_st*2*sizeof(float));
	}

	for(i=0; i<prim->n_st; i++)
		if((fabs(prim->st[i*2+0]-s)<0.0001) &&
			(fabs(prim->st[i*2+1]-t)<0.0001))
				return(i);

	if(prim->n_st>=prim->m_st)
	{
		i=prim->m_st+(prim->m_st>>1);
		st=kalloc(i*2*sizeof(float));
		memcpy(st, prim->st, prim->m_st*2*sizeof(float));
		kfree(prim->st);
		prim->st=st;
		prim->m_st=i;
	}

	prim->st[i*2+0]=s;
	prim->st[i*2+1]=t;
	prim->n_st++;

	return(i);
}

void LBXGL_ACXML_LoadSurface(LBXGL_MeshPrim *prim, NetParse_XmlStream *strm)
{
	char *str;
	int *pts;
	float s, t;
	int i, j, n, v, nv, pi;

	if(!prim->pts)
	{
		prim->n_pts=0;
		prim->m_pts=16;
		prim->pts=kalloc(prim->m_pts*2*sizeof(int));
	}

	if(!prim->face)
	{
		prim->n_face=0;
		prim->m_face=8;
		prim->face=kalloc(prim->m_face*2*sizeof(int));
	}

	if(prim->n_face>=prim->m_face)
	{
		i=prim->m_face+(prim->m_face>>1);
		pts=kalloc(i*2*sizeof(int));
		memcpy(pts, prim->face, prim->m_face*2*sizeof(int));
		kfree(prim->face);
		prim->face=pts;
		prim->m_face=i;
	}

	while(1)
	{
		n=NetParse_XMLStream_ParseTag(strm);
		if(n==3)break;

		if(!strcmp(strm->key, "refs") && (n==2))
		{
			str=NetParse_XMLStream_GetAttr(strm, "num");
			if(str)n=atoi(str);
				else n=16;

			while((prim->n_pts+n)>=prim->m_pts)
			{
				i=prim->m_pts+(prim->m_pts>>1);
				pts=kalloc(i*2*sizeof(int));
				memcpy(pts, prim->pts,
					prim->m_pts*2*sizeof(int));
				kfree(prim->pts);
				prim->pts=pts;
				prim->m_pts=i;
			}

			j=prim->n_pts; i=0;
			while(1)
			{
				n=NetParse_XMLStream_ParseTag(strm);
				if(n==3)break;

				if(strcmp(strm->key, "ref"))
				{
					NetParse_XMLStream_EatTag(strm);
					continue;
				}

				v=0; s=0; t=0;
				str=NetParse_XMLStream_GetAttr(strm, "v");
				if(str)v=atoi(str);
				str=NetParse_XMLStream_GetAttr(strm, "st");
				if(str)sscanf(str, "%f %f", &s, &t);

				prim->pts[(j+i)*2+0]=v;
				prim->pts[(j+i)*2+1]=
					LBXGL_ACXML_IndexST(prim, s, t);
				i++;

				NetParse_XMLStream_EatTag(strm);
			}

			continue;
		}

		NetParse_XMLStream_EatTag(strm);
	}

	i=prim->n_face++;
	prim->face[i*2+0]=prim->n_pts;
	prim->face[i*2+1]=n;
	prim->n_pts+=n;
}

int LBXGL_ACXML_LoadSurfaces(LBXGL_MeshPrim *prim, NetParse_XmlStream *strm)
{
	char *str;
	int i, j, k, l, n, n0;
	float f;

	while(1)
	{
		n=NetParse_XMLStream_ParseTag(strm);
		if(n==3)break;

		if(!strcmp(strm->key, "surface"))
		{
			LBXGL_ACXML_LoadSurface(prim, strm);
			continue;
		}
		NetParse_XMLStream_EatTag(strm);
	}

	return(0);
}


LBXGL_MeshPrim *LBXGL_ACXML_LoadPolyObject(NetParse_XmlStream *strm, float *offs, char *base)
{
	LBXGL_MeshPrim *tmp;
	float *vecs;
	char *str, **a;
	int i, n, nk;
	float vec[3], roffs[3];

	tmp=ObjType_New("lbxgl_meshprim_t", sizeof(LBXGL_MeshPrim));

	roffs[0]=offs[0];
	roffs[1]=offs[1];
	roffs[2]=offs[2];

	str=NetParse_XMLStream_GetAttr(strm, "trans");
	if(str)
	{
		a=ksplit(str);
		roffs[0]+=atof(a[1]);
		roffs[1]+=atof(a[2]);
		roffs[2]+=atof(a[3]);
	}

	str=NetParse_XMLStream_GetAttr(strm, "texture");
	if(str)
	{
		tmp->tex=kstrdup(str);
		tmp->texnum=LBXGL_Texture_LoadImage(tmp->tex);
	}

	str=NetParse_XMLStream_GetAttr(strm, "name");
	if(str)tmp->name=kstrdup(str);

	while(1)
	{
		n=NetParse_XMLStream_ParseTag(strm);
		if(n==3)break;

		if(!strcmp(strm->key, "vertices"))
		{
			str=NetParse_XMLStream_GetAttr(strm, "num");
			if(str)n=atoi(a[1]);

			i=16;
			while(i<=n)i=i+(i>>1);
			vecs=kalloc(i*3*sizeof(float));

			tmp->n_xyz=n;
			tmp->m_xyz=i;
			tmp->xyz=vecs;

			i=0;
			while(1)
			{
				n=NetParse_XMLStream_ParseTag(strm);
				if(n==3)break;

				if(strcmp(strm->key, "vertex"))
				{
					NetParse_XMLStream_EatTag(strm);
					continue;
				}

				str=NetParse_XMLStream_GetAttr(strm, "pt");
				sscanf(str, "%f %f %f",
					&vecs[(i*3)+0],
					&vecs[(i*3)+1],
					&vecs[(i*3)+2]);

				vecs[(i*3)+0]+=roffs[0];
				vecs[(i*3)+1]+=roffs[1];
				vecs[(i*3)+2]+=roffs[2];
				i++;

				NetParse_XMLStream_EatTag(strm);
			}
			continue;
		}

		if(!strcmp(strm->key, "surfaces"))
		{
			LBXGL_ACXML_LoadSurfaces(tmp, strm);
			continue;
		}

		NetParse_XMLStream_EatTag(strm);
	}

	return(tmp);
}

LBXGL_MeshGroup *LBXGL_ACXML_LoadGroupObject(NetParse_XmlStream *strm, float *offs, char *base)
{
	LBXGL_MeshGroup *tmp, *grp;
	LBXGL_MeshPrim *prim;
	char **a, *str;
	int i, n, nk;
	char *name, *data;
	float roffs[3];

	tmp=ObjType_New("lbxgl_meshgroup_t", sizeof(LBXGL_MeshGroup));

	roffs[0]=offs[0];
	roffs[1]=offs[1];
	roffs[2]=offs[2];

	str=NetParse_XMLStream_GetAttr(strm, "trans");
	if(str)
	{
		a=ksplit(str);
		roffs[0]+=atof(a[1]);
		roffs[1]+=atof(a[2]);
		roffs[2]+=atof(a[3]);
	}

	str=NetParse_XMLStream_GetAttr(strm, "name");
	if(str)tmp->name=kstrdup(str);

	while(1)
	{
		n=NetParse_XMLStream_ParseTag(strm);
		if(n==3)break;

		if(!strcmp(strm->key, "group"))
		{
			grp=LBXGL_ACXML_LoadGroupObject(strm, roffs, base);
			grp->next=tmp->first;
			tmp->first=grp;
			continue;
		}
		if(!strcmp(strm->key, "object"))
		{
			prim=LBXGL_ACXML_LoadPolyObject(strm, roffs, base);
			prim->next=tmp->prim;
			tmp->prim=prim;
			continue;
		}

		NetParse_XMLStream_EatTag(strm);
	}

	return(tmp);
}

LBXGL_MeshGroup *LBXGL_ACXML_LoadModel(char *name)
{
	NetParse_XmlStream *strm;
	VFILE *fd;
	char *buf, *base, *t;
	LBXGL_MeshGroup *tmp, *grp;
	char **a;
	float org[3];
	int sz, n;

	buf=kralloc(256);
	strcpy(buf, name);
	t=buf+strlen(buf);
	while((t>buf) && (*t!='/'))t--;
	*t=0;
	base=kstrdup(buf);

	printf("LBXGL_ACXML_LoadModel: loading '%s'\n", name);

	fd=vffopen(name, "rt");
	if(!fd)
	{
		printf("LBXGL_ACXML_LoadModel: can't open '%s'\n", name);
		return(NULL);
	}

	vfseek(fd, 0, 2);
	sz=vftell(fd);
	buf=malloc(sz+1);
	vfseek(fd, 0, 0);
	vfread(buf, 1, sz, fd);
	buf[sz]=0;
	vfclose(fd);

	strm=NetParse_XMLStream_CreateStream(buf, sz);

	tmp=NULL;
	V3F_ZERO(org);

	while(1)
	{
		n=NetParse_XMLStream_ParseTag(strm);
		if(n<=0)break;

		if(!strcmp(strm->key, "acxml"))break;
		NetParse_XMLStream_EatTag(strm);
	}

	if((n==2) && !strcmp(strm->key, "acxml"))
		while(1)
	{
		n=NetParse_XMLStream_ParseTag(strm);
		if(n==0)break;

		if(!strcmp(strm->key, "group"))
		{
			V3F_ZERO(org);
			tmp=LBXGL_ACXML_LoadGroupObject(strm, org, base);
			continue;
		}

		NetParse_XMLStream_EatTag(strm);
	}

	if(!tmp)
	{
		printf("LBXGL_ACXML_LoadModel: no root object for '%s'\n", name);
		return(tmp);
	}

	return(tmp);
}



void LBXGL_ACXML_SaveSurface(LBXGL_MeshPrim *prim, VFILE *fd)
{
}

int LBXGL_ACXML_SavePrim(LBXGL_MeshPrim *prim, VFILE *fd)
{
	int i, j, k, p, n;

	prim=LBXGL_Mesh_ClonePrim(prim);
	LBXGL_Mesh_OptimizePrim(prim);

	kprintvf(fd, "<object");
	if(prim->name)kprintvf(fd, " name=\"%s\"", prim->name);
	if(prim->tex)kprintvf(fd, " texture=\"%s\"", prim->tex);
	kprintvf(fd, ">\n");

	kprintvf(fd, "  <vertices num=\"%d\">\n", prim->n_xyz);
	for(i=0; i<prim->n_xyz; i++)
	{
		kprintvf(fd, "    <vertex pt=\"%f %f %f\"/>\n",
			prim->xyz[i*3+0], prim->xyz[i*3+1], prim->xyz[i*3+2]);
	}
	kprintvf(fd, "  </vertices>\n");

	kprintvf(fd, "  <surfaces num=\"%d\">\n", prim->n_face);
	for(i=0; i<prim->n_face; i++)
	{
		p=prim->face[i*2+0];
		n=prim->face[i*2+1];

//		kprintvf(fd, "    <surface num=\"%d\">\n", n);

		kprintvf(fd, "    <surface>\n");
		kprintvf(fd, "     <refs num=\"%d\">\n", n);
		for(j=0; j<n; j++)
		{
			k=prim->pts[(p+j)*2+1];
			kprintvf(fd, "      <ref v=\"%d\" st=\"%f %f\"/>\n",
				prim->pts[(p+j)*2+0],
				prim->st[k*2+0], prim->st[k*2+1]);
		}

		kprintvf(fd, "     </refs>\n");
		kprintvf(fd, "    </surface>\n");
	}
	kprintvf(fd, "  </surfaces>\n");

	kprintvf(fd, "</object>\n");
	LBXGL_Mesh_DestroyPrim(prim);
}

int LBXGL_ACXML_SaveGroup(LBXGL_MeshGroup *grp, VFILE *fd)
{
	LBXGL_MeshGroup *gcur;
	LBXGL_MeshPrim *mcur;
	int n;

	kprintvf(fd, "<group");
	if(grp->name)kprintvf(fd, " name=\"%s\"\n", grp->name);
	kprintvf(fd, ">\n");

	gcur=grp->first;
	while(gcur)
	{
		if(gcur->fl&128) { gcur=gcur->next; continue; }
		LBXGL_ACXML_SaveGroup(gcur, fd);
		gcur=gcur->next;
	}

	mcur=grp->prim;
	while(mcur)
	{
		if(mcur->fl&128) { mcur=mcur->next; continue; }
		LBXGL_ACXML_SavePrim(mcur, fd);
		mcur=mcur->next;
	}

	kprintvf(fd, "</group>\n");
}

int LBXGL_ACXML_SaveModel(LBXGL_MeshGroup *grp, char *name)
{
	LBXGL_MeshGroup *gcur;
	LBXGL_MeshPrim *mcur;
	VFILE *fd;
	int n;

	printf("LBXGL_ACXML_SaveModel: %s\n", name);

	fd=vffopen(name, "wt");
	if(!fd)
	{
		printf("fail open %s\n", name);
		return(-1);
	}

	kprintvf(fd, "<acxml xmlns="
		"\"uuid:a7a10504-5e07-4886-879a-dbd061164ceb\">\n");

#if 0
	gcur=grp->first;
	while(gcur)
	{
		if(gcur->fl&128) { gcur=gcur->next; continue; }
		LBXGL_ACXML_SaveGroup(gcur, fd);
		gcur=gcur->next;
	}

	mcur=grp->prim;
	while(mcur)
	{
		if(mcur->fl&128) { mcur=mcur->next; continue; }
		LBXGL_ACXML_SavePrim(mcur, fd);
		mcur=mcur->next;
	}
#endif

	LBXGL_ACXML_SaveGroup(grp, fd);

	kprintvf(fd, "</acxml>\n");

	vfclose(fd);
	return(0);
}
