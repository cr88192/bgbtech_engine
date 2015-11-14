#include <btgecm.h>

static char *btge_dtoa2p(double v)
{
	char tb[256];
	int i;
	
	i=(int)((v>=0)?(v+0.5):(v-0.5));
	if(fabs(v-i)<=0.01)
	{
		sprintf(tb, "%d", i);
		return(gcrstrdup(tb));
	}
	sprintf(tb, "%.2f", v);
	return(gcrstrdup(tb));
}

static char *btge_dtoa4p(double v)
{
	char tb[256];
	int i;

	i=(int)((v>=0)?(v+0.5):(v-0.5));
	if(fabs(v-i)<=0.0001)
	{
		sprintf(tb, "%d", i);
		return(gcrstrdup(tb));
	}
	sprintf(tb, "%.4f", v);
	return(gcrstrdup(tb));
}

BTGE_API void BTGE_BrushMap_SaveBrushPatch(
	BTGE_BrushWorld *world, VFILE *fd, BTGE_Brush *brush)
{
	float org[3];
	int i, j;

	V3F_ZERO(org);
	if(brush->se_owner)
	{
		BTGE_SEnt_GetVec3(brush->se_owner, "origin", org);
	}

	vfprint(fd, " {\n");
	vfprint(fd, "  patchDef2\n");
	vfprint(fd, "  {\n");
	vfprint(fd, "   %s\n", brush->patch->tex);

	vfprint(fd, "   ( %.2f %.2f %.2f %.2f %.2f )\n",
		brush->patch->xoffs, brush->patch->yoffs,
		brush->patch->rot,
		brush->patch->xscale, brush->patch->yscale);
	vfprint(fd, "   (\n");
	for(i=0; i<brush->patch->ny_pts; i++)
	{
		vfprint(fd, "    (");
		for(j=0; j<brush->patch->nx_pts; j++)
		{
//			vfprint(fd, " ( %.2f %.2f %.2f  %.2f %.2f )",
//				brush->patch->pts[i][j]->xyz[0]+org[0],
//				brush->patch->pts[i][j]->xyz[1]+org[1],
//				brush->patch->pts[i][j]->xyz[2]+org[2],
//				brush->patch->pts[i][j]->uv[0],
//				brush->patch->pts[i][j]->uv[1]);

			vfprint(fd, " ( %s %s %s  %s %s )",
				btge_dtoa2p(brush->patch->pts[i][j]->xyz[0]+org[0]),
				btge_dtoa2p(brush->patch->pts[i][j]->xyz[1]+org[1]),
				btge_dtoa2p(brush->patch->pts[i][j]->xyz[2]+org[2]),
				btge_dtoa4p(brush->patch->pts[i][j]->uv[0]),
				btge_dtoa4p(brush->patch->pts[i][j]->uv[1]));
		}
		vfprint(fd, " )\n");
	}
	vfprint(fd, "   )\n");

	vfprint(fd, "  }\n");
	vfprint(fd, " }\n");
}

BTGE_API void BTGE_BrushMap_SaveBrushEntity(
	BTGE_BrushWorld *world, VFILE *fd, BTGE_Brush *brush)
{
	BTGE_SEntity *ent;
	BTGE_SEntPair *epcur;
	BTGE_Brush *bcur;
	char *s;

	ent=brush->se_child;

	vfprint(fd, "entityDef {\n");

	epcur=ent->keys;
	while(epcur)
	{
		vfprint(fd, "\"%s\" \"%s\"\n", epcur->name, epcur->value);
		epcur=epcur->next;
	}

	bcur=ent->brush;
	while(bcur)
	{
		BTGE_BrushMap_SaveBrush(world, fd, bcur);
		bcur=bcur->enext;
	}

	vfprint(fd, "}\n");
}

BTGE_API void BTGE_BrushMap_SaveBrush(
	BTGE_BrushWorld *world, VFILE *fd, BTGE_Brush *brush)
{
	float tv0[3], tv1[3], tv2[3];
	float *sd, *td;
	int i;

	if(brush->flags&(BTGE_BRFL_DEL|BTGE_BRFL_TRANSIENT))
		return;

	if(brush->se_child)
	{
		BTGE_BrushMap_SaveBrushEntity(world, fd, brush);
		return;
	}

	if(brush->patch)
	{
		BTGE_BrushMap_SaveBrushPatch(world, fd, brush);
		return;
	}

	if(brush->mesh)
	{
		BTGE_BrushMap_SaveBrushMesh(world, fd, brush);
		return;
	}

	vfprint(fd, " {\n");

//	if(brush->idnum)
//		{ vfprint(fd, "  \"idnum\" \"%d\"\n", brush->idnum); }
	if(brush->name)
		{ vfprint(fd, "  \"name\" \"%s\"\n", brush->name); }
	if(brush->gname)
		{ vfprint(fd, "  \"groupname\" \"%s\"\n", brush->gname); }

	for(i=0; i<brush->n_face; i++)
	{
		V3F_SCALE(brush->norm+i*4, brush->norm[i*4+3], tv0);
		Hull_AdjacentNormals(brush->norm+i*4, tv2, tv1);
		V3F_ADDSCALE(tv0, tv1, 128, tv1);
		V3F_ADDSCALE(tv0, tv2, 128, tv2);
		sd=brush->sdir+i*4;
		td=brush->tdir+i*4;

#if 1
		vfprint(fd, "  [ %s %s %s %s ] %s "
			"[ %s %s %s %s ] [ %s %s %s %s ]\n",
			btge_dtoa4p(brush->norm[i*4+0]),
			btge_dtoa4p(brush->norm[i*4+1]),
			btge_dtoa4p(brush->norm[i*4+2]),
			btge_dtoa4p(brush->norm[i*4+3]),
			
			brush->tex[i],
			btge_dtoa4p(sd[0]), btge_dtoa4p(sd[1]),
				btge_dtoa4p(sd[2]), btge_dtoa4p(sd[3]),
			btge_dtoa4p(td[0]), btge_dtoa4p(td[1]),
				btge_dtoa4p(td[2]), btge_dtoa4p(td[3]));
#endif

#if 0
		vfprint(fd, "  ( %.2f %.2f %.2f ) ( %.2f %.2f %.2f ) "
			"( %.2f %.2f %.2f ) %s "
			"[ %.4f %.4f %.4f %.4f ] [ %.4f %.4f %.4f %.4f ] "
			"0  1.00000 1.00000 0 0 0\n",
			tv0[0], tv0[1], tv0[2],
			tv1[0], tv1[1], tv1[2],
			tv2[0], tv2[1], tv2[2], brush->tex[i],
			sd[0], sd[1], sd[2], sd[3],
			td[0], td[1], td[2], td[3]);
#endif
	}

	vfprint(fd, " }\n");
}

BTGE_API void BTGE_BrushMap_SaveEntity(
	BTGE_BrushWorld *world, VFILE *fd, BTGE_SEntity *ent)
{
	BTGE_SEntPair *epcur;
	BTGE_Brush *bcur;
	char *s;

	vfprint(fd, "{\n");

	epcur=ent->keys;
	while(epcur)
	{
		vfprint(fd, "\"%s\" \"%s\"\n", epcur->name, epcur->value);
		epcur=epcur->next;
	}

	bcur=ent->brush;
	while(bcur)
	{
		BTGE_BrushMap_SaveBrush(world, fd, bcur);
		bcur=bcur->enext;
	}

	//any brushes w/o owners go to worldspawn
	s=BTGE_SEnt_GetStr(ent, "classname");
	if(!strcmp(s, "worldspawn"))
	{
		bcur=world->brush;
		while(bcur)
		{
			if(!bcur->se_owner)
			{
				BTGE_BrushMap_SaveBrush(world, fd, bcur);
			}
			bcur=bcur->next;
		}
	}

	vfprint(fd, "}\n");
}

BTGE_API void BTGE_BrushMap_SaveFile(BTGE_BrushWorld *world, char *name)
{
	VFILE *fd;
	BTGE_SEntity *ecur;

	printf("Saving Map %s\n", name);

	world->map_name=dystrdup(name);

	fd=vffopen(name, "wt");

	ecur=world->entity;
	while(ecur)
	{
		BTGE_BrushMap_SaveEntity(world, fd, ecur);
		ecur=ecur->next;
	}

	vfclose(fd);
}

BTGE_API void BTGE_BrushMap_LoadPatchPoints(BTGE_BrushWorld *world, BTGE_Brush *brush, VFILE *fd)
{
	char buf[4096];
	BTGE_BrushPatchPoint *pta[256];
	float org[3];
	float f, g, h;

	BTGE_BrushPatchPoint **ptb;
	BTGE_BrushPatchPoint *pt;
	char **a, *tex, *s;
	int i, j, ny, nx;

	V3F_ZERO(org);
	if(world->entity_psel)
		BTGE_SEnt_GetVec3(world->entity_psel, "origin", org);

	ny=0; nx=0;
	while(!vfeof(fd))
	{
		memset(buf, 0, 4096);
		vfgets(buf, 4095, fd);
		a=gcrsplit(buf);

		if(!a[0])continue;

		if(*(a[0])=='/')continue;	//comment
		if(*(a[0])==';')continue;	//comment
		if(!strcmp(a[0], "}"))break;	//end of brush

		if(!strcmp(a[0], ")"))break;	//end of patch

		if(strcmp(a[0], "("))
			continue;

		// ( ( x1 y1 z1 u1 v1 ) ( x2 y2 z2 u2 v2 ) ( x3 y3 z3 u3 v3 ) )

		for(i=0; i<256; i++)
			pta[i]=NULL;

		for(i=0; i<256; i++)
		{
			if(!strcmp(a[i*7+1], ")"))
				break;

			pt=gctalloc("lbxgl_brushpatchpoint_t",
				sizeof(BTGE_BrushPatchPoint));
			pt->xyz[0]=atof(a[i*7+2])-org[0];
			pt->xyz[1]=atof(a[i*7+3])-org[1];
			pt->xyz[2]=atof(a[i*7+4])-org[2];
			pt->uv[0]=atof(a[i*7+5]);
			pt->uv[1]=atof(a[i*7+6]);
			
			pta[i]=pt;
		}
		
		if(i>nx)nx=i;
		
		ptb=gcalloc((nx+1)*sizeof(BTGE_BrushPatchPoint **));		
		for(i=0; i<nx; i++)
			ptb[i]=pta[i];
		
		if((ny+1)>=brush->patch->m_pts)
		{
			i=brush->patch->m_pts;
			if((ny+1)>=i)i=i+(i>>1);
			brush->patch->pts=gcrealloc(brush->patch->pts,
				i*sizeof(BTGE_BrushPatchPoint **));
			brush->patch->m_pts=i;
		}
		
		i=ny++;
		brush->patch->pts[i]=ptb;
	}

	brush->patch->nx_pts=nx;
	brush->patch->ny_pts=ny;
	
//	printf("BTGE_BrushMap_LoadPatchPoints: %d %d\n", nx, ny);
}

BTGE_API void BTGE_BrushMap_LoadPatch(
	BTGE_BrushWorld *world, BTGE_Brush *brush, VFILE *fd)
{
	char buf[256];
	float v0[3], v1[3], v2[3];
	float tv0[4], tv1[4], tv2[4];
	float vs[4], vt[4], tn[4];
	float f, g, h;

	BTGE_BrushPatch *ptmp;
	char **a, *tex, *s;
	int i, n;

	ptmp=gctalloc("lbxgl_brushpatch_t", sizeof(BTGE_BrushPatch));
	brush->patch=ptmp;
	
	i=16;
	ptmp->pts=gcalloc(i*sizeof(BTGE_BrushPatchPoint **));
	ptmp->nx_pts=0;
	ptmp->ny_pts=0;
	ptmp->m_pts=i;
	

	vfgets(buf, 255, fd);	//'{'
	vfgets(buf, 255, fd);	//texture
	a=gcrsplit(buf);
	tex=dystrdup(a[0]);

	if(!strncmp(tex, "textures/", strlen("textures/")))
		tex+=strlen("textures/");

	ptmp->tex=tex;

	vfgets(buf, 255, fd);	// ( xoffs yoffs rot xscale yscale )
	a=gcrsplit(buf);
	ptmp->xoffs=atof(a[1]);
	ptmp->yoffs=atof(a[2]);
	ptmp->rot=atof(a[3]);
	ptmp->xscale=atof(a[4]);
	ptmp->yscale=atof(a[5]);

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
			BTGE_BrushMap_LoadPatchPoints(world, brush, fd);
			continue;
		}
	}
}

BTGE_API void BTGE_BrushMap_LoadBrush3(BTGE_BrushWorld *world, BTGE_Brush *brush, VFILE *fd)
{
	char buf[256];
	float v0[3], v1[3], v2[3];
	float tv0[4], tv1[4], tv2[4];
	float vs[4], vt[4], tn[4];
	float f, g, h;

	char **a, *tex, *s;
	int i, n;

	n=0;
	while(!vfeof(fd))
	{
		memset(buf, 0, 256);
		vfgets(buf, 255, fd);
		a=gcrsplit(buf);

		if(!a[0])continue;

		if(*(a[0])=='/')continue;	//comment
		if(*(a[0])==';')continue;	//comment
		if(!strcmp(a[0], "}"))break;	//end of brush

		if(strcmp(a[0], "("))continue;

		// ( nx ny nz nd ) ( ( xx xy xo ) ( yx yy yo ) ) tex
		tn[0]=atof(a[1]);
		tn[1]=atof(a[2]);
		tn[2]=atof(a[3]);
		tn[3]=-atof(a[4]);

		v0[0]=atof(a[8]);
		v0[1]=atof(a[9]);
		v0[2]=atof(a[10]);

		v1[0]=atof(a[13]);
		v1[1]=atof(a[14]);
		v1[2]=atof(a[15]);

		tex=dystrdup(a[18]);

		if(!strncmp(tex, "textures/", strlen("textures/")))
			tex+=strlen("textures/");

		Hull_AdjacentNormals22(tn, tv0, tv1);

		vs[0]=v0[0]*tv0[0]+v0[1]*tv1[0];
		vs[1]=v0[0]*tv0[1]+v0[1]*tv1[1];
		vs[2]=v0[0]*tv0[2]+v0[1]*tv1[2];
		vs[3]=v0[2];

		vt[0]=v1[0]*tv0[0]+v1[1]*tv1[0];
		vt[1]=v1[0]*tv0[1]+v1[1]*tv1[1];
		vt[2]=v1[0]*tv0[2]+v1[1]*tv1[2];
		vt[3]=v1[2];

		BTGE_Brush_CheckExpandFaces(brush, n+1);
		V4F_COPY(tn, brush->norm+n*4);
		V4F_COPY(vs, brush->sdir+n*4);
		V4F_COPY(vt, brush->tdir+n*4);
		brush->tex[n]=tex;
		n++;
	}

	brush->n_face=n;
}

BTGE_API void BTGE_BrushMap_LoadBrush1(BTGE_BrushWorld *world, BTGE_Brush *brush, VFILE *fd)
{
	char buf[256];
	float v0[3], v1[3], v2[3];
	float tv0[4], tv1[4], tv2[4];
	float vs[4], vt[4], tn[4];
	float f, g, h;

	char **a, *tex, *s;
	int i, n;

	n=0;
	while(!vfeof(fd))
	{
		memset(buf, 0, 256);
		vfgets(buf, 255, fd);
		a=gcrsplit(buf);

		if(!a[0])continue;

		if(*(a[0])=='/')continue;	//comment
		if(*(a[0])==';')continue;	//comment
		if(!strcmp(a[0], "}"))break;	//end of brush

		if(strcmp(a[0], "("))continue;

		v0[0]=atof(a[1]);
		v0[1]=atof(a[2]);
		v0[2]=atof(a[3]);
		v1[0]=atof(a[6]);
		v1[1]=atof(a[7]);
		v1[2]=atof(a[8]);
		v2[0]=atof(a[11]);
		v2[1]=atof(a[12]);
		v2[2]=atof(a[13]);

//		tex=dystrdup(a[15]);

		V3F_SUB(v1, v0, tv0);
		V3F_SUB(v2, v0, tv1);
		V3F_NORMALIZE(tv0, tv0);
		V3F_NORMALIZE(tv1, tv1);

		V3F_CROSS(tv1, tv0, tn);
		V3F_NORMALIZE(tn, tn);
		tn[3]=V3F_DOT(v0, tn);

		// ( x0 y0 z0 ) ( x1 y1 z1 ) ( x2 y2 z2 ) ( ( xx xy xo ) ( yx yy yo ) ) tex

		v0[0]=atof(a[17]);
		v0[1]=atof(a[18]);
		v0[2]=atof(a[19]);

		v1[0]=atof(a[22]);
		v1[1]=atof(a[23]);
		v1[2]=atof(a[24]);

		tex=dystrdup(a[27]);

		if(!strncmp(tex, "textures/", strlen("textures/")))
			tex+=strlen("textures/");

		Hull_AdjacentNormals22(tn, tv0, tv1);

//		if(v0[0]!=0)v0[0]=1.0/v0[0];
//		if(v0[1]!=0)v0[1]=1.0/v0[1];

//		if(v1[0]!=0)v1[0]=1.0/v1[0];
//		if(v1[1]!=0)v1[1]=1.0/v1[1];

		v0[0]*=256;
		v0[1]*=256;
		v1[0]*=256;
		v1[1]*=256;

		vs[0]=v0[0]*tv0[0]+v0[1]*tv1[0];
		vs[1]=v0[0]*tv0[1]+v0[1]*tv1[1];
		vs[2]=v0[0]*tv0[2]+v0[1]*tv1[2];
		vs[3]=v0[2];

		vt[0]=v1[0]*tv0[0]+v1[1]*tv1[0];
		vt[1]=v1[0]*tv0[1]+v1[1]*tv1[1];
		vt[2]=v1[0]*tv0[2]+v1[1]*tv1[2];
		vt[3]=v1[2];

		BTGE_Brush_CheckExpandFaces(brush, n+1);
		V4F_COPY(tn, brush->norm+n*4);
		V4F_COPY(vs, brush->sdir+n*4);
		V4F_COPY(vt, brush->tdir+n*4);
		brush->tex[n]=tex;
		n++;
	}

	brush->n_face=n;
}

BTGE_API void BTGE_BrushMap_LoadBrushEntity(
	BTGE_BrushWorld *world, BTGE_Brush *brush, VFILE *fd)
{
	char buf[256];
	BTGE_SEntity *etmp;

	vfgets(buf, 255, fd);	//'{'
	etmp=BTGE_BrushMap_LoadEntity(world, fd);
	brush->se_child=etmp;
}

BTGE_API BTGE_Brush *BTGE_BrushMap_LoadBrush(BTGE_BrushWorld *world, VFILE *fd)
{
	char buf[256];
	float v0[3], v1[3], v2[3];
	float tv0[4], tv1[4], tv2[4];
	float vs[4], vt[4], tn[4];
	float f, g, h;

	BTGE_Brush *tmp;
	char **a, *tex, *s, *bname, *gname;
	int i, n, id;

	world->map_stat_brushes++;
	id=world->map_stat_brushes;

//	tmp=BTGE_Brush_New(16);
//	tmp->n_face=0;
	tmp=NULL; n=0; bname=NULL; gname=NULL;

	while(!vfeof(fd))
	{
		memset(buf, 0, 256);
		vfgets(buf, 255, fd);
		a=gcrsplit(buf);

		if(!a[0])continue;

		if(!strcmp(a[0], "name"))
			{ bname=dystrdup(a[1]); continue; }
		if(!strcmp(a[0], "groupname"))
			{ gname=dystrdup(a[1]); continue; }

		if(*(a[0])=='/')continue;	//comment
		if(*(a[0])==';')continue;	//comment

		if(!strcmp(a[0], "}"))break;	//end of brush

		if(!strcmp(a[0], "brushDef"))
		{
			if(!tmp) { tmp=BTGE_Brush_New(8); tmp->n_face=0; }
			BTGE_BrushMap_LoadBrush1(world, tmp, fd);
			n=tmp->n_face;
			continue;
		}

		if(!strcmp(a[0], "brushDef2") ||
			!strcmp(a[0], "brushDef3"))
		{
			if(!tmp) { tmp=BTGE_Brush_New(8); tmp->n_face=0; }
			BTGE_BrushMap_LoadBrush3(world, tmp, fd);
			n=tmp->n_face;
			continue;
		}

		if(!strcmp(a[0], "patchDef2"))
		{
			if(!tmp)tmp=BTGE_Brush_New(0);
			BTGE_BrushMap_LoadPatch(world, tmp, fd);
			n=tmp->n_face;
			continue;
		}

		if(!strcmp(a[0], "patchDef3"))
		{
			if(!tmp)tmp=BTGE_Brush_New(0);
			BTGE_BrushMap_LoadPatch(world, tmp, fd);
			n=tmp->n_face;
			continue;
		}

		if(!strcmp(a[0], "meshDef1"))
		{
			if(!tmp)tmp=BTGE_Brush_New(0);
//			BTGE_BrushMap_LoadBrushMesh(world, tmp, fd);
			BTGE_BrushMap_LoadBrushMeshList(world, tmp, fd);
			n=tmp->n_face;
//			continue;
			break;
		}

		if(!strcmp(a[0], "entityDef") ||
			!strcmp(a[0], "entityDef1"))
		{
			if(!tmp)tmp=BTGE_Brush_New(0);
			BTGE_BrushMap_LoadBrushEntity(world, tmp, fd);
			n=tmp->n_face;
			continue;
		}

		if(!tmp) { tmp=BTGE_Brush_New(8); tmp->n_face=0; }

		if(!strcmp(a[0], "["))
		{
			// [ nx ny nz nd ] tex [ sx sy sz so ] [ tx ty tz to ]
			tn[0]=atof(a[1]);
			tn[1]=atof(a[2]);
			tn[2]=atof(a[3]);
			tn[3]=atof(a[4]);

			V3F_NORMALIZE(tn, tn);

			tex=dystrdup(a[6]);

			vs[0]=atof(a[8]);
			vs[1]=atof(a[9]);
			vs[2]=atof(a[10]);
			vs[3]=atof(a[11]);

			vt[0]=atof(a[14]);
			vt[1]=atof(a[15]);
			vt[2]=atof(a[16]);
			vt[3]=atof(a[17]);
		}
		else
		{
			if(strcmp(a[0], "("))continue;
			v0[0]=atof(a[1]);
			v0[1]=atof(a[2]);
			v0[2]=atof(a[3]);
			v1[0]=atof(a[6]);
			v1[1]=atof(a[7]);
			v1[2]=atof(a[8]);
			v2[0]=atof(a[11]);
			v2[1]=atof(a[12]);
			v2[2]=atof(a[13]);

			tex=dystrdup(a[15]);

			V3F_SUB(v1, v0, tv0);
			V3F_SUB(v2, v0, tv1);
			V3F_NORMALIZE(tv0, tv0);
			V3F_NORMALIZE(tv1, tv1);

			V3F_CROSS(tv1, tv0, tn);
			V3F_NORMALIZE(tn, tn);
			tn[3]=V3F_DOT(v0, tn);

			if(!strcmp(a[16], "["))
			{
				vs[0]=atof(a[17]);
				vs[1]=atof(a[18]);
				vs[2]=atof(a[19]);
				vs[3]=atof(a[20]);

				vt[0]=atof(a[23]);
				vt[1]=atof(a[24]);
				vt[2]=atof(a[25]);
				vt[3]=atof(a[26]);
			}else
			{
				Hull_AdjacentNormals22(tn, vs, vt);
				vs[3]=-atof(a[16]);
				vt[3]=atof(a[17]);

				f=-atof(a[18])*(M_PI/180);
				g=cos(f); h=sin(f);
//				V3F_SCALEADDSCALE(vs, g, vt, h, tv0);
//				V3F_SCALEADDSCALE(vt, g, vs, -h, tv1);

				V3F_SCALEADDSCALE(vs, g, vt, -h, tv0);
				V3F_SCALEADDSCALE(vt, g, vs, h, tv1);

//				V3F_COPY(tv0, vs);
//				V3F_COPY(tv1, vt);

				f=atof(a[19]); g=atof(a[20]);
				if(f==0)f=1; if(g==0)g=1;
				V3F_SCALE(tv0, 1.0/f, vs);
				V3F_SCALE(tv1, 1.0/g, vt);

//				vs[3]*=1.0/f; vt[3]*=1.0/g;
			}
		}
#if 1
		s=tex;
		while(*s)
		{
			if(*s=='/')break;
			if(*s=='\\')break;
			s++;
		}
		
		if(*s=='\\')
		{
			strcpy(buf, tex);
			for(i=0; buf[i]; i++)
			{
				if(buf[i]=='\\')buf[i]='/';
				if(buf[i]=='*')buf[i]='#';
			}
			tex=dystrdup(buf);
		}
		
		if(!(*s))
		{
//			sprintf(buf, "q1tex/%s", tex);
			sprintf(buf, "q1tex2/%s", tex);

			for(i=0; buf[i]; i++)
			{
				if(buf[i]=='*')buf[i]='#';
				if((buf[i]>='A') && (buf[i]<='Z'))
					buf[i]=buf[i]-'A'+'a';
			}

			tex=dystrdup(buf);
		}
#endif

		BTGE_Brush_CheckExpandFaces(tmp, n+1);
		V4F_COPY(tn, tmp->norm+n*4);
		V4F_COPY(vs, tmp->sdir+n*4);
		V4F_COPY(vt, tmp->tdir+n*4);
		tmp->tex[n]=tex;
		n++;
	}

	tmp->name=bname;
	tmp->gname=gname;
	tmp->idnum=id;
	tmp->n_face=n;
	return(tmp);
}

BTGE_API BTGE_Brush *BTGE_BrushMap_LoadEntityDef(
	BTGE_BrushWorld *world, VFILE *fd)
{
	char buf[256];
	BTGE_Brush *tmp;
	BTGE_SEntity *etmp;

//	vfgets(buf, 255, fd);	//'{'
	tmp=BTGE_Brush_New(0);
	etmp=BTGE_BrushMap_LoadEntity(world, fd);
	tmp->se_child=etmp;
	return(tmp);
}

BTGE_API char *BTGE_BrushMap_ReadBlockString(
	BTGE_BrushWorld *world, VFILE *fd)
{
	char buf[256];
	char buf2[16384];
	char **a;
	char *s, *t;

	t=buf2;
	while(!vfeof(fd))
	{
		memset(buf, 0, 256);
		vfgets(buf, 255, fd);
		a=gcrsplit(buf);

		if(!strcmp(a[0], "]]"))
			break;
		sprintf(t, "%s\n", buf);
		t+=strlen(t);
	}
	*t++=0;
	
	s=dystrdup(buf2);
	return(s);
}

BTGE_API BTGE_SEntity *BTGE_BrushMap_LoadEntity(BTGE_BrushWorld *world, VFILE *fd)
{
	char buf[256];
	BTGE_SEntity *tmp;
	BTGE_Brush *btmp;
	char **a;
	char *s;
	int id, eid;

	world->map_stat_ents++;
	eid=world->map_stat_ents;

	tmp=gctalloc("lbxgl_sentity_s", sizeof(BTGE_SEntity));
	BTGE_SEnt_AddEntity(&world->entity, tmp);
	tmp->wrl=world;
	tmp->idnum=eid;

	while(!vfeof(fd))
	{
		memset(buf, 0, 256);
		vfgets(buf, 255, fd);
		a=gcrsplit(buf);

		if(!a[0])continue;

		if(*(a[0])=='/')continue;	//comment
		if(*(a[0])==';')continue;	//comment

		if(!strcmp(a[0], "}"))break;

		if(!strcmp(a[0], "{"))
		{
			world->entity_psel=tmp;
			btmp=BTGE_BrushMap_LoadBrush(world, fd);

			BTGE_BrushWorld_AddBrush(world, btmp);

//			if(btmp->n_face || btmp->patch || btmp->mesh)
//				BTGE_BrushWorld_AddBrush(world, btmp);

			continue;
		}

		if(!strcmp(a[0], "cubeDef1"))
		{
			world->entity_psel=tmp;

			world->map_stat_brushes++;
			id=world->map_stat_brushes;

			//cubeDef1 ( mx my mz ) ( nx ny nz ) texture ( tx ty tz ) tp
			btmp=BTGE_Brush_MakeCubeExtentsTexPos(
				atof(a[2]), atof(a[3]), atof(a[4]),
				atof(a[7]), atof(a[8]), atof(a[9]),
				dystrdup(a[11]),
				atof(a[13]), atof(a[14]), atof(a[15]),
				atoi(a[17]));
//			btmp=BTGE_Brush_MakeCubeExtents(
//				atof(a[2]), atof(a[3]), atof(a[4]),
//				atof(a[7]), atof(a[8]), atof(a[9]),
//				dystrdup(a[11]));
			if(btmp->n_face || btmp->patch || btmp->mesh)
			{
				btmp->idnum=id;
				BTGE_BrushWorld_AddBrush(world, btmp);
			}
			continue;
		}

		if(!strcmp(a[0], "entityDef") && !strcmp(a[1], "{"))
		{
			world->entity_psel=tmp;
			btmp=BTGE_BrushMap_LoadEntityDef(world, fd);
			world->entity_psel=tmp;
			BTGE_BrushWorld_AddBrush(world, btmp);
			continue;
		}

		if(a[0] && a[1] && !strcmp(a[1], "[["))
		{
			s=BTGE_BrushMap_ReadBlockString(world, fd);
			BTGE_SEnt_SetStr(tmp, a[0], s);
			continue;
		}

		if(!strcmp(a[0], "classname") && !strncmp(a[1], "light", 5))
			world->map_stat_lights++;

		BTGE_SEnt_SetStr(tmp, a[0], a[1]);
	}

	return(tmp);
}

BTGE_API void BTGE_BrushMap_LoadFile(BTGE_BrushWorld *world, char *name)
{
	char buf[256];
	VFILE *fd;
	BTGE_SEntity *ecur;
	char **a;

	BTGE_BrushWorld_DrawFlush(world);
//	BTGE_WorldTeardown(world);
	BTGE_BrushWorld_Teardown(world);

	fd=vffopen(name, "rt");
	if(!fd)return;

	world->ents=NULL;
	world->n_ents=0;
	world->m_ents=0;

	world->portal=NULL;
	world->light=NULL;

	world->entity=NULL;
	world->brush=NULL;
//	world->entity_sel=NULL;
	world->entity_psel=NULL;
//	world->brush_sel=NULL;

//	world->mesh=NULL;
//	world->object_list=NULL;


	world->map_name=dystrdup(name);
	world->map_stat_brushes=0;
	world->map_stat_ents=0;
	world->map_stat_lights=0;

	while(!vfeof(fd))
	{
		memset(buf, 0, 256);
		vfgets(buf, 255, fd);
		a=gcrsplit(buf);

		if(!a[0])continue;

		if(!strcmp(a[0], "{"))
		{
			ecur=BTGE_BrushMap_LoadEntity(world, fd);
			continue;
		}
	}

	vfclose(fd);

//	world->entity_sel=NULL;
	world->entity_psel=NULL;
//	world->brush_sel=NULL;

	printf("Map Loaded: %d brushes, %d ents, %d lights\n",
		world->map_stat_brushes,
		world->map_stat_ents,
		world->map_stat_lights);

//	world->object_list=spawn_ents(world->entity);
}

