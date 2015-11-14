#include <lbxgl2.h>

void LBXGL_ASE_LoadDummy(VFILE *fd)
{
	char *buf;
	char **a;

	while(!vfeof(fd))
	{
		buf=gcralloc(256);
		vfgets(buf, 255, fd);

		a=gcrsplit(buf);

		if(!strcmp(a[0], "}"))
			break;
	}
}

void LBXGL_ASE_LoadMeshVertices(VFILE *fd, LBXGL_MeshPrim *prim)
{
	char *buf;
	char **a;
	int i;

	while(!vfeof(fd))
	{
		buf=gcralloc(256);
		vfgets(buf, 255, fd);

		a=gcrsplit(buf);

		if(!strcmp(a[0], "}"))
			break;

		if(!strcmp(a[0], "*MESH_VERTEX"))
		{
			i=atoi(a[1]);
			if(i>=prim->n_xyz)
			{
				prim->n_xyz=i+1;
				LBXGL_Mesh_CheckExpandXYZ(prim);
			}
			
			prim->xyz[i*3+0]=atof(a[2]);
			prim->xyz[i*3+1]=atof(a[3]);
			prim->xyz[i*3+2]=atof(a[4]);
			continue;
		}
	}
}

void LBXGL_ASE_LoadMeshFaces(VFILE *fd, LBXGL_MeshPrim *prim)
{
	int vidx[64];
	char *buf;
	char **a;
	int i, j, k, l;

	while(!vfeof(fd))
	{
		buf=gcralloc(256);
		vfgets(buf, 255, fd);

		a=gcrsplit(buf);

		if(!strcmp(a[0], "}"))
			break;

		if(!strcmp(a[0], "*MESH_FACE"))
		{
			i=atoi(a[1]);
			if(i>=prim->n_face)
			{
				prim->n_face=i+1;
				LBXGL_Mesh_CheckExpandFace(prim);
			}
			
			for(j=0; j<26; j++)
			{
				if((a[2+j*2+0][0]!=('A'+j)) ||
					(a[2+j*2+0][1]!=':'))
						break;
				vidx[j]=atoi(a[2+j*2+1]);
			}
			
			l=j;
			k=LBXGL_Mesh_AllocPTS(prim, l);
			prim->face[i*2+0]=k;
			prim->face[i*2+1]=l;
			
			for(j=0; j<l; j++)
				{ prim->pts[(k+j)*2+0]=vidx[j]; }
				
			continue;
		}
	}
}

void LBXGL_ASE_LoadMeshVertST(VFILE *fd, LBXGL_MeshPrim *prim)
{
	char *buf;
	char **a;
	int i;

	while(!vfeof(fd))
	{
		buf=gcralloc(256);
		vfgets(buf, 255, fd);

		a=gcrsplit(buf);

		if(!strcmp(a[0], "}"))
			break;

		if(!strcmp(a[0], "*MESH_TVERT"))
		{
			i=atoi(a[1]);
			if(i>=prim->n_st)
			{
				prim->n_st=i+1;
				LBXGL_Mesh_CheckExpandST(prim);
			}
			
			prim->st[i*2+0]=atof(a[2]);
			prim->st[i*2+1]=atof(a[3]);
			continue;
		}
	}
}

void LBXGL_ASE_LoadMeshFaceST(VFILE *fd, LBXGL_MeshPrim *prim)
{
	char *buf;
	char **a;
	int i, j, k, l;

	while(!vfeof(fd))
	{
		buf=gcralloc(256);
		vfgets(buf, 255, fd);

		a=gcrsplit(buf);

		if(!strcmp(a[0], "}"))
			break;

		if(!strcmp(a[0], "*MESH_TFACE"))
		{
			i=atoi(a[1]);
			if(i>=prim->n_face)
			{
				prim->n_st=i+1;
				LBXGL_Mesh_CheckExpandFace(prim);
			}
			
			k=prim->face[i*2+0];
			l=prim->face[i*2+1];
			
			for(j=0; j<l; j++)
			{
				prim->pts[(k+j)*2+1]=atoi(a[2+j]);
			}
			continue;
		}
	}
}

LBXGL_MeshPrim *LBXGL_ASE_LoadMesh(VFILE *fd, LBXGL_MeshGroup *grp)
{
	LBXGL_MeshPrim *tmp;
	char *buf;
	char **a;

	tmp=LBXGL_Mesh_CreatePrim(grp, NULL);

	while(!vfeof(fd))
	{
		buf=gcralloc(256);
		vfgets(buf, 255, fd);

		a=gcrsplit(buf);

		if(!strcmp(a[0], "}"))
			break;

		if(!strcmp(a[0], "*MESH_VERTEX_LIST"))
		{
			LBXGL_ASE_LoadMeshVertices(fd, tmp);
			continue;
		}

		if(!strcmp(a[0], "*MESH_FACE_LIST"))
		{
			LBXGL_ASE_LoadMeshFaces(fd, tmp);
			continue;
		}

		if(!strcmp(a[0], "*MESH_TVERTLIST"))
		{
			LBXGL_ASE_LoadMeshVertST(fd, tmp);
			continue;
		}
		if(!strcmp(a[0], "*MESH_TFACELIST"))
		{
			LBXGL_ASE_LoadMeshFaceST(fd, tmp);
			continue;
		}
	}
	
	return(tmp);
}

LBXGL_MeshGroup *LBXGL_ASE_LoadGeomObject(VFILE *fd,
	LBXGL_MeshGroup *grp)
{
	char *buf;
	char **a;

	while(!vfeof(fd))
	{
		buf=gcralloc(256);
		vfgets(buf, 255, fd);

		a=gcrsplit(buf);

		if(!strcmp(a[0], "}"))
			break;

		if(!strcmp(a[0], "*NODE_TM"))
		{
			LBXGL_ASE_LoadDummy(fd);
			continue;
		}

		if(!strcmp(a[0], "*MESH"))
		{
			LBXGL_ASE_LoadMesh(fd, grp);
			continue;
		}
	}
	return(NULL);
}

LBXGL_MeshGroup *LBXGL_ASE_LoadModel(char *name)
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

	printf("LBXGL_ASE_LoadModel: loading '%s'\n", name);

	fd=vffopen(name, "rt");
	if(!fd)
	{
		printf("LBXGL_ASE_LoadModel: can't open '%s'\n", name);
		return(NULL);
	}

	tmp=LBXGL_Mesh_CreateGroup(NULL, NULL);

	V3F_ZERO(org);
	while(!vfeof(fd))
	{
		buf=gcralloc(256);
		vfgets(buf, 255, fd);
		a=gcrsplit(buf);
		if(!a[0])continue;

		if(!strcmp(a[0], "*SCENE"))
		{
			LBXGL_ASE_LoadDummy(fd);
			continue;
		}

		if(!strcmp(a[0], "*MATERIAL_LIST"))
		{
			LBXGL_ASE_LoadDummy(fd);
			continue;
		}

		if(!strcmp(a[0], "*GEOMOBJECT"))
		{
			V3F_ZERO(org);
			LBXGL_ASE_LoadGeomObject(fd, tmp);
			break;
		}
	}

	if(!tmp)
	{
		printf("LBXGL_ASE_LoadModel: no root object for '%s'\n", name);
		return(tmp);
	}

	return(tmp);
}
