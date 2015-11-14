#include <lbxgl2.h>

LBXGL_API void LBXGL_BrushMap_SaveBrushMesh(
	LBXGL_BrushWorld *world, VFILE *fd, LBXGL_Brush *brush)
{
	float org[3];
	LBXGL_MeshPrim *pcur;
	float *xyz, *st;
	int n, p;
	int i, j, k;

	V3F_ZERO(org);
	if(brush->se_owner)
		LBXGL_SEnt_GetVec3(brush->se_owner, "origin", org);

	vfprint(fd, " {\n");
	vfprint(fd, "  meshDef1\n");
	
	pcur=brush->mesh;
	while(pcur)
	{
		vfprint(fd, "  {\n");

		vfprint(fd, "   %s ( %.3f %.3f %.3f ) "
			"[ %.3f %.3f %.3f ] [ %.3f %.3f %.3f ] %d\n",
			brush->mesh->tex,

			brush->mesh->tex_org[0],
			brush->mesh->tex_org[1],
			brush->mesh->tex_org[2],

			brush->mesh->tex_sdir[0],
			brush->mesh->tex_sdir[1],
			brush->mesh->tex_sdir[2],

			brush->mesh->tex_tdir[0],
			brush->mesh->tex_tdir[1],
			brush->mesh->tex_tdir[2],
		
			brush->mesh->tex_mode
			);

		vfprint(fd, "   (\n");
		for(i=0; i<brush->mesh->n_face; i++)
		{
			p=brush->mesh->face[i*2+0];
			n=brush->mesh->face[i*2+1];

			vfprint(fd, "    (");
			for(j=0; j<n; j++)
			{
				st=brush->mesh->st+brush->mesh->pts[(p+j)*2+1]*2;
				xyz=brush->mesh->xyz+brush->mesh->pts[(p+j)*2+0]*3;

				vfprint(fd, " ( %.2f %.2f %.2f  %.4f %.4f )",
					xyz[0]+org[0],
					xyz[1]+org[1],
					xyz[2]+org[2],
					st[0], st[1]);
			}
			vfprint(fd, " )\n");
		}
		vfprint(fd, "   )\n");
		vfprint(fd, "  }\n");
		pcur=pcur->bnext;
	}
	vfprint(fd, " }\n");
}

LBXGL_API void LBXGL_BrushMap_LoadMeshFaces(
	LBXGL_BrushWorld *world, LBXGL_Brush *brush, VFILE *fd)
{
	char buf[4096];
	int vidx[256], stidx[256];
	float org[3], tv0[3], tv1[3];
	float f, g, h;

	LBXGL_BrushPatchPoint **ptb;
	LBXGL_BrushPatchPoint *pt;
	char **a, *tex, *s;
	int i, j, k, n;

	V3F_ZERO(org);
	if(world->entity_psel)
		LBXGL_SEnt_GetVec3(world->entity_psel, "origin", org);

	while(!vfeof(fd))
	{
		memset(buf, 0, 4096);
		vfgets(buf, 4095, fd);
		a=gcrsplit(buf);

		if(!a[0])continue;

		if(*(a[0])=='/')continue;	//comment
		if(*(a[0])==';')continue;	//comment
		if(!strcmp(a[0], "}"))break;	//end of brush

		if(!strcmp(a[0], ")"))break;	//end of mesh

		if(strcmp(a[0], "("))
			continue;

		// ( ( x1 y1 z1 u1 v1 ) ( x2 y2 z2 u2 v2 ) ( x3 y3 z3 u3 v3 ) )

		for(i=0; i<256; i++)
		{
			if(!strcmp(a[i*7+1], ")"))
				break;

			tv0[0]=atof(a[i*7+2])-org[0];
			tv0[1]=atof(a[i*7+3])-org[1];
			tv0[2]=atof(a[i*7+4])-org[2];
			tv1[0]=atof(a[i*7+5]);
			tv1[1]=-atof(a[i*7+6]);
			
			vidx[i]=LBXGL_Mesh_IndexXYZ(brush->mesh, tv0);
			stidx[i]=LBXGL_Mesh_IndexST(brush->mesh, tv1);
		}
		LBXGL_Mesh_AddPolygon(brush->mesh, vidx, stidx, i);
	}
}

LBXGL_API void LBXGL_BrushMap_LoadBrushMesh(LBXGL_BrushWorld *world,
	LBXGL_Brush *brush, VFILE *fd)
{
	char buf[256];
	float v0[3], v1[3], v2[3];
	float tv0[4], tv1[4], tv2[4];
	float vs[4], vt[4], tn[4];
	float f, g, h;

	LBXGL_MeshPrim *ptmp, *pcur;
	char **a, *tex, *s;
	int i, n;

	ptmp=LBXGL_Mesh_CreatePrim(NULL, NULL);
//	brush->mesh=ptmp;

	if(brush->mesh)
	{
		pcur=brush->mesh;
		while(pcur->bnext)
			pcur=pcur->bnext;
		pcur->bnext=ptmp;
	}else
	{
		brush->mesh=ptmp;
	}

	
//	vfgets(buf, 255, fd);	//'{'
	vfgets(buf, 255, fd);	//texture ( xo yo zo ) [ sx sy sz ] [ tx ty tz ]
	a=gcrsplit(buf);

	tex=dystrdup(a[0]);

	if(!strncmp(tex, "textures/", strlen("textures/")))
		tex+=strlen("textures/");

	ptmp->tex=tex;

	ptmp->tex_org[0]=atof(a[2]);
	ptmp->tex_org[1]=atof(a[3]);
	ptmp->tex_org[2]=atof(a[4]);

	ptmp->tex_sdir[0]=atof(a[7]);
	ptmp->tex_sdir[1]=atof(a[8]);
	ptmp->tex_sdir[2]=atof(a[9]);

	ptmp->tex_tdir[0]=atof(a[12]);
	ptmp->tex_tdir[1]=atof(a[13]);
	ptmp->tex_tdir[2]=atof(a[14]);

	ptmp->tex_mode=atoi(a[16]);

	while(!vfeof(fd))
	{
		memset(buf, 0, 256);
		vfgets(buf, 255, fd);
		a=gcrsplit(buf);

		if(!a[0])continue;

		if(*(a[0])=='/')continue;	//comment
		if(*(a[0])==';')continue;	//comment
		if(!strcmp(a[0], "}"))break;	//end of brush

		if(!strcmp(a[0], "("))
		{
			LBXGL_BrushMap_LoadMeshFaces(world, brush, fd);
			continue;
		}
	}
}

LBXGL_API void LBXGL_BrushMap_LoadBrushMeshList(
	LBXGL_BrushWorld *world, LBXGL_Brush *brush, VFILE *fd)
{
	char buf[256];
	char **a;

	while(!vfeof(fd))
	{
		memset(buf, 0, 256);
		vfgets(buf, 255, fd);
		a=gcrsplit(buf);

		if(!a[0])continue;

		if(*(a[0])=='/')continue;		//comment
		if(*(a[0])==';')continue;		//comment
		if(!strcmp(a[0], "}"))break;	//end of brush

		if(!strcmp(a[0], "{"))
		{
			LBXGL_BrushMap_LoadBrushMesh(world, brush, fd);
			continue;
		}
	}
}
