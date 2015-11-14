#include <lbxgl2.h>

static char *lbxgl_dtoa2p(double v)
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

static char *lbxgl_dtoa4p(double v)
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

LBXGL_API void LBXGL_BrushMap_SaveBrushPatch(
	LBXGL_BrushWorld *world, VFILE *fd, LBXGL_Brush *brush)
{
	float org[3];
	int i, j;

	V3F_ZERO(org);
	if(brush->se_owner)
		LBXGL_SEnt_GetVec3(brush->se_owner, "origin", org);

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
				lbxgl_dtoa2p(brush->patch->pts[i][j]->xyz[0]+org[0]),
				lbxgl_dtoa2p(brush->patch->pts[i][j]->xyz[1]+org[1]),
				lbxgl_dtoa2p(brush->patch->pts[i][j]->xyz[2]+org[2]),
				lbxgl_dtoa4p(brush->patch->pts[i][j]->uv[0]),
				lbxgl_dtoa4p(brush->patch->pts[i][j]->uv[1]));
		}
		vfprint(fd, " )\n");
	}
	vfprint(fd, "   )\n");

	vfprint(fd, "  }\n");
	vfprint(fd, " }\n");
}

int LBXGL_BrushMap_SaveBrushTryCube_CheckTProj(
	LBXGL_BrushWorld *world, VFILE *fd, LBXGL_Brush *brush,
	float *tvo, int tty)
{
	float tvs[4], tvt[4];
	int i;

	for(i=0; i<6; i++)
	{
		if(tty==2)
			{ Hull_AdjacentNormals22(brush->norm+i*4, tvs, tvt); }
		else
			{ Hull_AdjacentNormals2(brush->norm+i*4, tvs, tvt); }

		tvs[3]=V3F_DOT(tvo, tvs);
		tvt[3]=V3F_DOT(tvo, tvt);

		if(V4F_DIST(brush->sdir+i*4, tvs)>0.001)
			break;
		if(V4F_DIST(brush->tdir+i*4, tvt)>0.001)
			break;
	}
	if(i<6)return(0);
	return(1);
}

LBXGL_API int LBXGL_BrushMap_SaveBrushTryCube(
	LBXGL_BrushWorld *world, VFILE *fd, LBXGL_Brush *brush)
{
	float tvs[4], tvt[4], tvo[4];
	float tvm[4], tvn[4];
	float tv0[4], tv1[4];
	char *tex;
	float f, g;
	int i, j, k, tty;

	printf("LBXGL_BrushMap_SaveBrushTryCube: Begin\n");

	//skip named brushes
	if(brush->name)
	{
		printf("LBXGL_BrushMap_SaveBrushTryCube: Has Name\n");
		return(0);
	}

	//check wrong number of faces
	if(brush->n_face!=6)
	{
		printf("LBXGL_BrushMap_SaveBrushTryCube: Face Count Fail\n");
		return(0);
	}
	
	//check different textures
	tex=brush->tex[0];
	for(i=1; i<6; i++)
		if(strcmp(brush->tex[i], tex))
	{
		printf("LBXGL_BrushMap_SaveBrushTryCube: Texture Check Fail\n");
		return(0);
	}

	//check planes and determine bbox
	k=0;
	for(i=0; i<6; i++)
	{
		for(j=0; j<3; j++)
		{
			V3F_ZERO(tv0); tv0[j]=1;
			V3F_ZERO(tv1); tv1[j]=-1;
			f=V3F_DOT(brush->norm+i*4, tv0)-1.0;
			g=V3F_DOT(brush->norm+i*4, tv1)-1.0;
			if(fabs(f)<0.001)
				{ tvn[j]= brush->norm[i*4+3]; k|=(1<<j); break; }
			else if(fabs(g)<0.001)
				{ tvm[j]=-brush->norm[i*4+3]; k|=(8<<j); break; }
		}

		if(j>=3)
		{
			printf("LBXGL_BrushMap_SaveBrushTryCube: "
				"Bad Normal ( %.2f %.2f %.2f %.2f ) %.4f %.4f\n",
				brush->norm[i*4+0], brush->norm[i*4+1],
				brush->norm[i*4+2], brush->norm[i*4+3], f, g);
			return(0);
		}
	}

	if(k!=63)
	{
		printf("LBXGL_BrushMap_SaveBrushTryCube: Face Mask Fail %d\n", k);
		return(0);
	}

	//calculate texture origin
	V3F_ZERO(tvo); V3F_ZERO(tv1);
	for(i=0; i<6; i++)
	{
		V3F_ADDSCALE(tvo, brush->sdir+i*4, brush->sdir[i*4+3], tvo);
		V3F_ADDSCALE(tvo, brush->tdir+i*4, brush->tdir[i*4+3], tvo);
		tv1[0]+=fabs(brush->sdir[i*4+0]);
		tv1[1]+=fabs(brush->sdir[i*4+1]);
		tv1[2]+=fabs(brush->sdir[i*4+2]);
		tv1[0]+=fabs(brush->tdir[i*4+0]);
		tv1[1]+=fabs(brush->tdir[i*4+1]);
		tv1[2]+=fabs(brush->tdir[i*4+2]);
	}
	
	//scale for combined weights
	for(i=0; i<3; i++)
	{
		f=tv1[i];
		if(f<=0)f=1;
		tvo[i]=tvo[i]*(1.0/f);
	}
//	V3F_SCALE(tvo, 0.25, tvo);

	tty=0;
	printf("cubeDef1 ( %.2f %.2f %.2f ) ( %.2f %.2f %.2f ) "
			"%s ( %.2f %.2f %.2f ) %d\n",
		tvm[0], tvm[1], tvm[2], tvn[0], tvn[1], tvn[2], 
		tex, tvo[0], tvo[1], tvo[2], tty);

	tty=0;
	for(i=1; i<3; i++)
	{
		j=LBXGL_BrushMap_SaveBrushTryCube_CheckTProj(
			world, fd, brush, tvo, i);
		if(j)tty=i;
	}

	if(tty<=0)
	{
		printf("LBXGL_BrushMap_SaveBrushTryCube: TexProj Unknown\n");
		return(0);
	}

#if 0
	//check for special texture positioning
	tty=0;
	for(i=0; i<6; i++)
	{
		j=-1;
		Hull_AdjacentNormals2(brush->norm+i*4, tvs, tvt);
		tvs[3]=V3F_DOT(tvo, tvs);
		tvt[3]=V3F_DOT(tvo, tvt);

		if((V4F_DIST(brush->sdir+i*4, tvs)<=0.001) &&
			(V4F_DIST(brush->tdir+i*4, tvt)<=0.001))
				j=1;

		Hull_AdjacentNormals22(brush->norm+i*4, tvs, tvt);
		tvs[3]=V3F_DOT(tvo, tvs);
		tvt[3]=V3F_DOT(tvo, tvt);

		if((V4F_DIST(brush->sdir+i*4, tvs)<=0.001) &&
			(V4F_DIST(brush->tdir+i*4, tvt)<=0.001))
				j=2;

		if(j<0)
		{
			printf("LBXGL_BrushMap_SaveBrushTryCube: TexProj Unknown\n");
			return(0);
		}
		if(tty&&(tty!=j))
		{
			printf("LBXGL_BrushMap_SaveBrushTryCube: "
				"TexProj Mismatch %d,%d\n", tty, j);
			return(0);
		}
		tty=j;
	}
#endif

	printf("LBXGL_BrushMap_SaveBrushTryCube: OK\n");

	vfprint(fd, " cubeDef1 ( %s %s %s ) ( %s %s %s ) "
			"%s ( %s %s %s ) %d\n",
		lbxgl_dtoa2p(tvm[0]), lbxgl_dtoa2p(tvm[1]), lbxgl_dtoa2p(tvm[2]),
		lbxgl_dtoa2p(tvn[0]), lbxgl_dtoa2p(tvn[1]), lbxgl_dtoa2p(tvn[2]), 
		tex,
		lbxgl_dtoa2p(tvo[0]), lbxgl_dtoa2p(tvo[1]), lbxgl_dtoa2p(tvo[2]),
		tty);

//	vfprint(fd, " cubeDef1 ( %.2f %.2f %.2f ) ( %.2f %.2f %.2f ) "
//			"%s ( %.2f %.2f %.2f ) %d\n",
//		tvm[0], tvm[1], tvm[2], tvn[0], tvn[1], tvn[2], 
//		tex, tvo[0], tvo[1], tvo[2], tty);
	return(1);

//	return(0);
}

LBXGL_API void LBXGL_BrushMap_SaveBrushEntity(
	LBXGL_BrushWorld *world, VFILE *fd, LBXGL_Brush *brush)
{
	LBXGL_SEntity *ent;
	LBXGL_SEntPair *epcur;
	LBXGL_Brush *bcur;
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
		LBXGL_BrushMap_SaveBrush(world, fd, bcur);
		bcur=bcur->enext;
	}

	vfprint(fd, "}\n");
}

LBXGL_API void LBXGL_BrushMap_SaveBrush(
	LBXGL_BrushWorld *world, VFILE *fd, LBXGL_Brush *brush)
{
	float tv0[3], tv1[3], tv2[3];
	float *sd, *td;
	int i;

	if(brush->flags&(LBXGL_BRFL_DEL|LBXGL_BRFL_TRANSIENT))
		return;

	if(brush->se_child)
	{
		LBXGL_BrushMap_SaveBrushEntity(world, fd, brush);
		return;
	}

	if(brush->patch)
	{
		LBXGL_BrushMap_SaveBrushPatch(world, fd, brush);
		return;
	}

	if(brush->mesh)
	{
		LBXGL_BrushMap_SaveBrushMesh(world, fd, brush);
		return;
	}

	if(LBXGL_BrushMap_SaveBrushTryCube(world, fd, brush))
		return;

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
			lbxgl_dtoa4p(brush->norm[i*4+0]),
			lbxgl_dtoa4p(brush->norm[i*4+1]),
			lbxgl_dtoa4p(brush->norm[i*4+2]),
			lbxgl_dtoa4p(brush->norm[i*4+3]),
			
			brush->tex[i],
			lbxgl_dtoa4p(sd[0]), lbxgl_dtoa4p(sd[1]),
				lbxgl_dtoa4p(sd[2]), lbxgl_dtoa4p(sd[3]),
			lbxgl_dtoa4p(td[0]), lbxgl_dtoa4p(td[1]),
				lbxgl_dtoa4p(td[2]), lbxgl_dtoa4p(td[3]));
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

#if 0
		vfprint(fd, "  ( %s %s %s ) ( %s %s %s ) "
			"( %s %s %s ) %s "
			"[ %s %s %s %s ] [ %s %s %s %s ] "
			"0  1.0 1.0 0 0 0\n",
			lbxgl_dtoa2p(tv0[0]), lbxgl_dtoa2p(tv0[1]),
				lbxgl_dtoa2p(tv0[2]),
			lbxgl_dtoa2p(tv1[0]), lbxgl_dtoa2p(tv1[1]),
				lbxgl_dtoa2p(tv1[2]),
			lbxgl_dtoa2p(tv2[0]), lbxgl_dtoa2p(tv2[1]),
				lbxgl_dtoa2p(tv2[2]),
			brush->tex[i],
			lbxgl_dtoa4p(sd[0]), lbxgl_dtoa4p(sd[1]),
				lbxgl_dtoa4p(sd[2]), lbxgl_dtoa4p(sd[3]),
			lbxgl_dtoa4p(td[0]), lbxgl_dtoa4p(td[1]),
				lbxgl_dtoa4p(td[2]), lbxgl_dtoa4p(td[3]));
#endif

	}

	vfprint(fd, " }\n");
}

LBXGL_API void LBXGL_BrushMap_SaveEntity(
	LBXGL_BrushWorld *world, VFILE *fd, LBXGL_SEntity *ent)
{
	LBXGL_SEntPair *epcur;
	LBXGL_Brush *bcur;
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
		LBXGL_BrushMap_SaveBrush(world, fd, bcur);
		bcur=bcur->enext;
	}

	//any brushes w/o owners go to worldspawn
	s=LBXGL_SEnt_GetStr(ent, "classname");
	if(!strcmp(s, "worldspawn"))
	{
		bcur=world->brush;
		while(bcur)
		{
			if(!bcur->se_owner)
			{
				LBXGL_BrushMap_SaveBrush(world, fd, bcur);
			}
			bcur=bcur->next;
		}
	}

	vfprint(fd, "}\n");
}

LBXGL_API void LBXGL_BrushMap_SaveFile(LBXGL_BrushWorld *world, char *name)
{
	VFILE *fd;
	LBXGL_SEntity *ecur;

	printf("Saving Map %s\n", name);

	world->map_name=dystrdup(name);

	fd=vffopen(name, "wt");

	ecur=world->entity;
	while(ecur)
	{
		LBXGL_BrushMap_SaveEntity(world, fd, ecur);
		ecur=ecur->next;
	}

	vfclose(fd);
}

LBXGL_API void LBXGL_BrushMap_LoadPatchPoints(
	LBXGL_BrushWorld *world, LBXGL_Brush *brush, VFILE *fd)
{
	char buf[4096];
	LBXGL_BrushPatchPoint *pta[256];
	float org[3];
	float f, g, h;

	LBXGL_BrushPatchPoint **ptb;
	LBXGL_BrushPatchPoint *pt;
	char **a, *tex, *s;
	int i, j, ny, nx;

	V3F_ZERO(org);
	if(world->entity_psel)
		LBXGL_SEnt_GetVec3(world->entity_psel, "origin", org);

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
				sizeof(LBXGL_BrushPatchPoint));
			pt->xyz[0]=atof(a[i*7+2])-org[0];
			pt->xyz[1]=atof(a[i*7+3])-org[1];
			pt->xyz[2]=atof(a[i*7+4])-org[2];
			pt->uv[0]=atof(a[i*7+5]);
			pt->uv[1]=-atof(a[i*7+6]);
			
			pta[i]=pt;
		}
		
		if(i>nx)nx=i;
		
		ptb=gcalloc((nx+1)*sizeof(LBXGL_BrushPatchPoint **));		
		for(i=0; i<nx; i++)
			ptb[i]=pta[i];
		
		if((ny+1)>=brush->patch->m_pts)
		{
			i=brush->patch->m_pts;
			if((ny+1)>=i)i=i+(i>>1);
			brush->patch->pts=gcrealloc(brush->patch->pts,
				i*sizeof(LBXGL_BrushPatchPoint **));
			brush->patch->m_pts=i;
		}
		
		i=ny++;
		brush->patch->pts[i]=ptb;
	}

	brush->patch->nx_pts=nx;
	brush->patch->ny_pts=ny;
	
//	printf("LBXGL_BrushMap_LoadPatchPoints: %d %d\n", nx, ny);
}

LBXGL_API void LBXGL_BrushMap_LoadPatch(
	LBXGL_BrushWorld *world, LBXGL_Brush *brush, VFILE *fd)
{
	char buf[256];
	float v0[3], v1[3], v2[3];
	float tv0[4], tv1[4], tv2[4];
	float vs[4], vt[4], tn[4];
	float f, g, h;

	LBXGL_BrushPatch *ptmp;
	char **a, *tex, *s;
	int i, n;

	ptmp=gctalloc("lbxgl_brushpatch_t", sizeof(LBXGL_BrushPatch));
	brush->patch=ptmp;
	
	i=16;
	ptmp->pts=gcalloc(i*sizeof(LBXGL_BrushPatchPoint **));
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
			LBXGL_BrushMap_LoadPatchPoints(world, brush, fd);
			continue;
		}
	}
}

LBXGL_API void LBXGL_BrushMap_LoadBrush3(
	LBXGL_BrushWorld *world, LBXGL_Brush *brush, VFILE *fd)
{
	char buf[256];
	float v0[3], v1[3], v2[3];
	float tv0[4], tv1[4], tv2[4], tv3[4];
	float vs[4], vt[4], tn[4];
	float f, g, h;

	char **a, *tex, *s;
	int i, n, txn, xs, ys;

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

//		Hull_AdjacentNormals22(tn, tv0, tv1);
		Hull_AdjacentNormals23(tn, tv0, tv1);

//		if(v0[0]!=0)v0[0]=1.0/v0[0];
//		if(v0[1]!=0)v0[1]=1.0/v0[1];

//		if(v1[0]!=0)v1[0]=1.0/v1[0];
//		if(v1[1]!=0)v1[1]=1.0/v1[1];

		sprintf(buf, "textures/%s", tex);
		txn=LBXGL_Texture_LoadImageSize(buf, &xs, &ys);
		if(txn<0) { xs=256; ys=256; }

//		v0[0]*=256; v0[1]*=256;
//		v1[0]*=256; v1[1]*=256;

//		v0[0]*=xs; v0[1]*=ys;
//		v1[0]*=xs; v1[1]*=ys;

		v0[0]*=xs; v0[1]*=xs;
		v1[0]*=ys; v1[1]*=ys;

//		v0[1]=1-v0[1];
//		v1[0]=1-v1[0];
//		v1[1]=1-v1[1];

//		v0[2]*=xs; v1[2]*=ys;

#if 1
		vs[0]=v0[0]*tv0[0]+v0[1]*tv1[0];
		vs[1]=v0[0]*tv0[1]+v0[1]*tv1[1];
		vs[2]=v0[0]*tv0[2]+v0[1]*tv1[2];

		vt[0]=v1[0]*tv0[0]+v1[1]*tv1[0];
		vt[1]=v1[0]*tv0[1]+v1[1]*tv1[1];
		vt[2]=v1[0]*tv0[2]+v1[1]*tv1[2];
#endif

#if 0
		vs[0]=v0[0]*tv0[0]-v0[1]*tv1[0];
		vs[1]=v0[0]*tv0[1]-v0[1]*tv1[1];
		vs[2]=v0[0]*tv0[2]-v0[1]*tv1[2];

		vt[0]=-v1[0]*tv0[0]+v1[1]*tv1[0];
		vt[1]=-v1[0]*tv0[1]+v1[1]*tv1[1];
		vt[2]=-v1[0]*tv0[2]+v1[1]*tv1[2];
#endif

//		vs[0]=v0[0]*tv0[0]+v1[0]*tv1[0];
//		vs[1]=v0[0]*tv0[1]+v1[0]*tv1[1];
//		vs[2]=v0[0]*tv0[2]+v1[0]*tv1[2];

//		vt[0]=v0[1]*tv0[0]+v1[1]*tv1[0];
//		vt[1]=v0[1]*tv0[1]+v1[1]*tv1[1];
//		vt[2]=v0[1]*tv0[2]+v1[1]*tv1[2];


//		vs[0]=v0[0]*tv0[0]+v1[0]*tv0[0];
//		vs[1]=v0[0]*tv0[1]+v1[0]*tv0[1];
//		vs[2]=v0[0]*tv0[2]+v1[0]*tv0[2];

//		vt[0]=v0[1]*tv1[0]+v1[1]*tv1[0];
//		vt[1]=v0[1]*tv1[1]+v1[1]*tv1[1];
//		vt[2]=v0[1]*tv1[2]+v1[1]*tv1[2];

#if 0
//		v0[2]*=xs; v1[2]*=ys;

		vs[3]=(v0[2]*v0[0]+v1[2]*v1[0])*xs;
		vt[3]=(v0[2]*v0[1]+v1[2]*v1[1])*ys;
#endif

#if 1
		v0[2]*=xs; v1[2]*=ys;
		vs[3]=v0[2];
		vt[3]=v1[2];
#endif

//		V3F_SCALE(vt, -1, vt);

		LBXGL_Brush_CheckExpandFaces(brush, n+1);
		V4F_COPY(tn, brush->norm+n*4);
		V4F_COPY(vs, brush->sdir+n*4);
		V4F_COPY(vt, brush->tdir+n*4);
		brush->tex[n]=tex;
		n++;
	}

	brush->n_face=n;
}

LBXGL_API void LBXGL_BrushMap_LoadBrush1(
	LBXGL_BrushWorld *world, LBXGL_Brush *brush, VFILE *fd)
{
	char buf[256];
	float v0[3], v1[3], v2[3];
	float tv0[4], tv1[4], tv2[4];
	float vs[4], vt[4], tn[4];
	float f, g, h;

	char **a, *tex, *s;
	int i, n, txn, xs, ys;

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

		sprintf(buf, "textures/%s", tex);
		txn=LBXGL_Texture_LoadImageSize(buf, &xs, &ys);
		if(txn<0) { xs=256; ys=256; }

//		v0[0]*=256; v0[1]*=256;
//		v1[0]*=256; v1[1]*=256;

		v0[0]*=xs; v0[1]*=ys;
		v1[0]*=xs; v1[1]*=ys;

		vs[0]=v0[0]*tv0[0]+v0[1]*tv1[0];
		vs[1]=v0[0]*tv0[1]+v0[1]*tv1[1];
		vs[2]=v0[0]*tv0[2]+v0[1]*tv1[2];
		vs[3]=v0[2];

		vt[0]=v1[0]*tv0[0]+v1[1]*tv1[0];
		vt[1]=v1[0]*tv0[1]+v1[1]*tv1[1];
		vt[2]=v1[0]*tv0[2]+v1[1]*tv1[2];
		vt[3]=v1[2];

		LBXGL_Brush_CheckExpandFaces(brush, n+1);
		V4F_COPY(tn, brush->norm+n*4);
		V4F_COPY(vs, brush->sdir+n*4);
		V4F_COPY(vt, brush->tdir+n*4);
		brush->tex[n]=tex;
		n++;
	}

	brush->n_face=n;
}

LBXGL_API void LBXGL_BrushMap_LoadBrushEntity(
	LBXGL_BrushWorld *world, LBXGL_Brush *brush, VFILE *fd)
{
	char buf[256];
	LBXGL_SEntity *etmp;

	vfgets(buf, 255, fd);	//'{'
	etmp=LBXGL_BrushMap_LoadEntity(world, fd);
	brush->se_child=etmp;
}

LBXGL_API LBXGL_Brush *LBXGL_BrushMap_LoadBrush(
	LBXGL_BrushWorld *world, VFILE *fd)
{
	char buf[256];
	float v0[3], v1[3], v2[3];
	float tv0[4], tv1[4], tv2[4];
	float vs[4], vt[4], tn[4];
	float f, g, h;

	LBXGL_Brush *tmp;
	char **a, *tex, *s, *bname, *gname;
	int i, n, id;

	world->map_stat_brushes++;
	id=world->map_stat_brushes;

//	tmp=malloc(sizeof(LBXGL_Brush));
//	memset(tmp, 0, sizeof(LBXGL_Brush));
//	tmp=LBXGL_Brush_New(16);
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
			if(!tmp) { tmp=LBXGL_Brush_New(8); tmp->n_face=0; }
			LBXGL_BrushMap_LoadBrush1(world, tmp, fd);
			n=tmp->n_face;
			continue;
		}

		if(!strcmp(a[0], "brushDef2") ||
			!strcmp(a[0], "brushDef3"))
		{
			if(!tmp) { tmp=LBXGL_Brush_New(8); tmp->n_face=0; }
			LBXGL_BrushMap_LoadBrush3(world, tmp, fd);
			n=tmp->n_face;
			continue;
		}

		if(!strcmp(a[0], "patchDef2"))
		{
			if(!tmp)tmp=LBXGL_Brush_New(0);
			LBXGL_BrushMap_LoadPatch(world, tmp, fd);
			n=tmp->n_face;
			continue;
		}

		if(!strcmp(a[0], "patchDef3"))
		{
			if(!tmp)tmp=LBXGL_Brush_New(0);
			LBXGL_BrushMap_LoadPatch(world, tmp, fd);
			n=tmp->n_face;
			continue;
		}

		if(!strcmp(a[0], "meshDef1"))
		{
			if(!tmp)tmp=LBXGL_Brush_New(0);
//			LBXGL_BrushMap_LoadBrushMesh(world, tmp, fd);
			LBXGL_BrushMap_LoadBrushMeshList(world, tmp, fd);
			n=tmp->n_face;
//			continue;
			break;
		}

		if(!strcmp(a[0], "entityDef1"))
		{
			if(!tmp)tmp=LBXGL_Brush_New(0);
			LBXGL_BrushMap_LoadBrushEntity(world, tmp, fd);
			n=tmp->n_face;
			continue;
		}

		if(!tmp) { tmp=LBXGL_Brush_New(8); tmp->n_face=0; }

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
			if(strcmp(a[0], "(") || strcmp(a[4], ")") ||
				strcmp(a[5], "(") || strcmp(a[9], ")") ||
				strcmp(a[10], "(") || strcmp(a[14], ")"))
			{
				printf("LBXGL_BrushMap_LoadBrush: "
					"Bad Face Token: %s\n", buf);
				continue;
			}

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

			tn[0]=((int)(tn[0]*256))/256.0;
			tn[1]=((int)(tn[1]*256))/256.0;
			tn[2]=((int)(tn[2]*256))/256.0;
			V3F_NORMALIZE(tn, tn);
			
			f=V3F_LEN(tn);
			if((f<0.999) || (f>1.001))
			{
				printf("LBXGL_BrushMap_LoadBrush: "
					"Bad Face Normal: %s\n", buf);
				continue;
			}

			tn[3]=V3F_DOT(v0, tn);
			tn[3]=((int)(tn[3]*256))/256.0;

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

		LBXGL_Brush_CheckExpandFaces(tmp, n+1);
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

LBXGL_API LBXGL_Brush *LBXGL_BrushMap_LoadEntityDef(
	LBXGL_BrushWorld *world, VFILE *fd)
{
	char buf[256];
	LBXGL_Brush *tmp;
	LBXGL_SEntity *etmp;

//	vfgets(buf, 255, fd);	//'{'
	tmp=LBXGL_Brush_New(0);
	etmp=LBXGL_BrushMap_LoadEntity(world, fd);
	tmp->se_child=etmp;
	return(tmp);
}

LBXGL_API char *LBXGL_BrushMap_ReadBlockString(
	LBXGL_BrushWorld *world, VFILE *fd)
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

LBXGL_API LBXGL_SEntity *LBXGL_BrushMap_LoadEntity(LBXGL_BrushWorld *world, VFILE *fd)
{
	char buf[256];
	LBXGL_SEntity *tmp;
	LBXGL_Brush *btmp;
	char **a;
	char *s;
	int id, eid;

	world->map_stat_ents++;
	eid=world->map_stat_ents;

	tmp=gctalloc("lbxgl_sentity_s", sizeof(LBXGL_SEntity));
	LBXGL_SEnt_AddEntity(&world->entity, tmp);
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
			btmp=LBXGL_BrushMap_LoadBrush(world, fd);

			LBXGL_BrushWorld_AddBrush(world, btmp);
			
//			if(btmp->n_face || btmp->patch || btmp->mesh)
//				LBXGL_BrushWorld_AddBrush(world, btmp);
			continue;
		}

		if(!strcmp(a[0], "cubeDef1"))
		{
			world->entity_psel=tmp;

			world->map_stat_brushes++;
			id=world->map_stat_brushes;

			//cubeDef1 ( mx my mz ) ( nx ny nz ) texture ( tx ty tz ) tp
			btmp=LBXGL_Brush_MakeCubeExtentsTexPos(
				atof(a[2]), atof(a[3]), atof(a[4]),
				atof(a[7]), atof(a[8]), atof(a[9]),
				dystrdup(a[11]),
				atof(a[13]), atof(a[14]), atof(a[15]),
				atoi(a[17]));
			if(btmp->n_face || btmp->patch || btmp->mesh)
			{
				btmp->idnum=id;
				LBXGL_BrushWorld_AddBrush(world, btmp);
			}
			continue;
		}

		if(!strcmp(a[0], "entityDef") && !strcmp(a[1], "{"))
		{
			world->entity_psel=tmp;
			btmp=LBXGL_BrushMap_LoadEntityDef(world, fd);
			world->entity_psel=tmp;
			LBXGL_BrushWorld_AddBrush(world, btmp);
			continue;
		}

		if(a[0] && a[1] && !strcmp(a[1], "[["))
		{
			s=LBXGL_BrushMap_ReadBlockString(world, fd);
			LBXGL_SEnt_SetStr(tmp, a[0], s);
			continue;
		}

		if(!strcmp(a[0], "classname") && !strncmp(a[1], "light", 5))
			world->map_stat_lights++;

		LBXGL_SEnt_SetStr(tmp, a[0], a[1]);
	}

	return(tmp);
}

LBXGL_API void LBXGL_BrushMap_ClearMap(LBXGL_BrushWorld *world, char *name)
{
	LBXGL_BrushWorld_DrawFlush(world);
	LBXGL_BrushWorld_ClearSun(world);

	world->entity=NULL;
	world->brush=NULL;
	world->light=NULL;
	world->light_sun=NULL;

	world->bsp=NULL;
	world->portal=NULL;
	world->mesh=NULL;
	world->bmdl=NULL;

	world->entity_sel=NULL;
	world->entity_psel=NULL;
	world->brush_sel=NULL;

	world->mesh=NULL;
	world->object_list=NULL;

	world->map_name=dystrdup(name);
	world->map_stat_brushes=0;
	world->map_stat_ents=0;
	world->map_stat_lights=0;
}

LBXGL_API void LBXGL_BrushMap_LoadFile(LBXGL_BrushWorld *world, char *name)
{
	char buf[256];
	VFILE *fd;
	LBXGL_SEntity *ecur;
	char **a;

	LBXGL_BrushWorld_DrawFlush(world);
	LBXGL_BrushWorld_ClearSun(world);

	fd=vffopen(name, "rt");
	if(!fd)return;

	world->entity=NULL;
	world->brush=NULL;
	world->light=NULL;
	world->light_sun=NULL;

	world->bsp=NULL;
	world->portal=NULL;
	world->mesh=NULL;
	world->bmdl=NULL;

	world->entity_sel=NULL;
	world->entity_psel=NULL;
	world->brush_sel=NULL;

	world->mesh=NULL;
	world->object_list=NULL;


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
			ecur=LBXGL_BrushMap_LoadEntity(world, fd);
			continue;
		}
	}

	vfclose(fd);

	world->entity_sel=NULL;
	world->entity_psel=NULL;
	world->brush_sel=NULL;

	printf("Map Loaded: %d brushes, %d ents, %d lights\n",
		world->map_stat_brushes,
		world->map_stat_ents,
		world->map_stat_lights);

//	world->object_list=spawn_ents(world->entity);
}

