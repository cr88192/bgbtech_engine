#include <btgecm.h>

/*ENTITY mesh_stairs ?
Mesh entity for generating stairs.
origin: Origin
direction: Direction stairs are facing.
width: Width of stairs.
height: Height of steps (rise for each step).
depth: Distance from front of step to back of step.
count: Step Count.
thickness: Step Thickness.
texture: Texture.
*/
BTGE_API void BtSb_mesh_stairs(BTGE_BrushWorld *world, BTGE_Brush *brush)
{
	float org[3], dir[3], pto[3];
	float lxd[3], lyd[3], lzd[3];
	float pt0[3], pt1[3], pt2[3], pt3[3];
	BTGE_MeshPrim *prim;
	char *tex;
	int w, h, d, c, th;
	int i0, i1, i2, i3;
	int j0, j1, j2, j3;
	int i;
	
	BTGE_SEnt_GetVec3(brush->se_child, "origin", org);
	BTGE_SEnt_GetVec3(brush->se_child, "direction", dir);
	
	w=BTGE_SEnt_GetNum(brush->se_child, "width");
	h=BTGE_SEnt_GetNum(brush->se_child, "height");
	d=BTGE_SEnt_GetNum(brush->se_child, "depth");
	c=BTGE_SEnt_GetNum(brush->se_child, "count");
	th=BTGE_SEnt_GetNum(brush->se_child, "thickness");

	tex=BTGE_SEnt_GetStr(brush->se_child, "texture");
	
	lzd[0]=0; lzd[1]=0; lzd[2]=1;
	V3F_NORMALIZE(dir, lyd);
	V3F_CROSS(lzd, lyd, lxd);
	
	prim=BTGE_Mesh_CreatePrim(NULL, NULL);
	prim->tex=dystrdup(tex);
	
	for(i=0; i<c; i++)
	{
		V3F_ADDSCALE(org, lyd, i*d, pto);
		V3F_ADDSCALE(pto, lzd, (i+1)*d, pto);
	
		V3F_ADDSCALE(pto, lxd, -w*0.5, pt0);
		V3F_ADDSCALE(pt0, lyd, -d*0.5, pt0);
		V3F_ADDSCALE(pt0, lyd, -th, pt0);

		V3F_ADDSCALE(pto, lxd, w*0.5, pt1);
		V3F_ADDSCALE(pt1, lyd, d*0.5, pt1);

		BTGE_Mesh_AddCube(prim, pt0, pt1);
	}
	
	brush->mesh=prim;
}


/*ENTITY mesh_stairs ?
Mesh entity for generating pipes.
origin: Origin
radius: Pipe Radius
texture: Texture
points: A list of control points ("( x y z )").
step: Subdivision factor for points.
*/
BTGE_API void BtSb_mesh_pipes(BTGE_BrushWorld *world, BTGE_Brush *brush)
{
	float pts0[16*3];
	float pts1[256*3];
	float org[3], tv0[3], tv1[3];
	BTGE_MeshPrim *prim;
	float rad, step;
	char *tex, *cpstr;
	float *xyz, *xyz0, *xyz1, *xyz2; 
	char **a;
	float f, g, h;
	int i0, i1, i2;
	int i, j, n, n1;
	
	BTGE_SEnt_GetVec3(brush->se_child, "origin", org);
//	BTGE_SEnt_GetVec3(brush->se_child, "direction", dir);
	
	rad=BTGE_SEnt_GetNum(brush->se_child, "width");
	step=BTGE_SEnt_GetNum(brush->se_child, "step");

	if(step<=0)step=1;

	tex=BTGE_SEnt_GetStr(brush->se_child, "texture");
	cpstr=BTGE_SEnt_GetStr(brush->se_child, "points");
	
	a=gcrsplit(cpstr); n=0;
	while(a[n*5] && !strcmp(a[n*5], "("))
	{
		pts0[0]=atof(a[n*5+1]);
		pts0[1]=atof(a[n*5+2]);
		pts0[2]=atof(a[n*5+3]);
		n++;
	}
	
	n1=n*step;
	for(i=0; i<n1; i++)
	{
		f=i/step;
		j=f; g=f-j;
		i0=j-1; i1=j; i2=j+1;
		if(i0<0)i0=0;
		if(i2>n)i2=n;

		xyz0=pts0+i0*3;
		xyz1=pts0+i1*3;
		xyz2=pts0+i2*3;
		xyz=pts1+i*3;

		f=1.0-g;
		V3F_SCALEADDSCALE(xyz0, f, xyz1, g, tv0);
		V3F_SCALEADDSCALE(xyz1, f, xyz2, g, tv1);
		V3F_SCALEADDSCALE(tv0, f, tv1, g, xyz);
	}

	prim=BTGE_Mesh_CreatePrim(NULL, NULL);
	prim->tex=dystrdup(tex);

	f=0;
	for(i=0; i<(n1-1); i++)
	{
		xyz0=pts1+(i+0)*3;
		xyz1=pts1+(i+1)*3;
		g=V3F_DIST(xyz1, xyz0)/64;
		BTGE_Mesh_AddCapsule(prim, xyz0, xyz1, rad, f, f+g);
		f+=g;
	}

	brush->mesh=prim;
}


/*ENTITY gen_tiles ?
Construction entity for generating tile-based worlds.
origin: Origin
image: Image name.
step: tile spep size.
*/
BTGE_API void BtSg_gen_tiles(BTGE_BrushWorld *world, BTGE_SEntity *ent)
{
	char tb[256];
	byte pal[768];
	char *tilenames[256];
	char *texnames[256];
	float org[3], tv[3], tvm[3], tvn[3];
	BTGE_Brush *btmp, *blst;
	byte *buf, *buftx;
	char *img, *tex, *str;
	float step;
	int xs, ys, xs1, ys1, pxs, pxox, pxoy, pxtox, pxtoy;
	int i, j, k, l, np, ntl, ntx;

//	*(int *)-1=-1;

	BTGE_SEnt_GetVec3(ent, "origin", org);

	np=BTGECM_ColorKey_LoadPaletteDefault(pal, 256);
	str=BTGE_SEnt_GetStr(ent, "palette");
	if(str)
	{
		i=BTGECM_ColorKey_LoadPalette(str, pal, 256);
		if(i>0)np=i;
	}

	ntl=0; ntx=0;
	for(i=0; i<256; i++)
	{
		tilenames[i]=NULL;
		texnames[i]=NULL;
	}

	str=BTGE_SEnt_GetStr(ent, "tiletab");
	if(str)
	{
		ntl=BTGECM_ColorKey_LoadNames(str, "tile", tilenames, 256);
		ntx=BTGECM_ColorKey_LoadNames(str, "tex", texnames, 256);
	}

	img=BTGE_SEnt_GetStr(ent, "image");
	if(!img)
	{
		printf("BtSg_gen_tiles: no image\n");
		return;
	}

	sprintf(tb, "%s.png", img);
	buf=BTGECM_PNG_LoadName(tb, &xs, &ys);
	if(!buf)
	{
		printf("BtSg_gen_tiles: failed load image %s\n", img);
		return;
	}

	sprintf(tb, "%s_tex.png", img);
	buftx=BTGECM_PNG_LoadName(tb, &xs1, &ys1);
	
	step=BTGE_SEnt_GetNum(ent, "step");
	if(step<=0)step=36;

	pxs=BTGE_SEnt_GetNum(ent, "pixelstep");
	if(pxs<=0)pxs=1;

	i=BTGE_SEnt_GetVec2(ent, "pixeloffs", tv);
	if(i>=0) { pxox=tv[0]; pxoy=tv[1]; }
	else { pxox=0; pxoy=0; }

	i=BTGE_SEnt_GetVec2(ent, "texpixeloffs", tv);
	if(i>=0) { pxtox=tv[0]; pxtoy=tv[1]; }
	else { pxtox=0; pxtoy=0; }

	if(!buftx && (pxs!=1))
		{ buftx=buf; }

	for(i=0; i<ys; i++)
		for(j=0; j<xs; j++)
	{
//		k=BTGECM_Misc_GetColorIndexPDN(buf+((i*xs)+j)*4);
		k=BTGECM_ColorKey_GetColorPaletteIndex(
			buf+(((i*pxs+pxoy)*xs)+(j*pxs+pxox))*4, pal, np);
		if(!k)continue;
		if(k==-1)k=7;

		tex="bgb_metal/bgb_metal_pan1";
		if(buftx)
		{
//			l=BTGECM_Misc_GetColorIndexPDN(buftx+((i*xs)+j)*4);
//			l=BTGECM_ColorKey_GetColorPaletteIndex(
//				buftx+((i*xs)+j)*4, pal, np);
			l=BTGECM_ColorKey_GetColorPaletteIndex(
				buftx+(((i*pxs+pxtoy)*xs)+(j*pxs+pxtox))*4, pal, np);
			
			if(texnames[l])
				tex=texnames[l];

#if 0
			switch(l)
			{
			case 7:
				break;
			case 15:
				tex="brick_stone/aardmbr32_1";
				break;
			case 9:
				tex="cs/andy_wood";
				break;
			case 37:
				tex="cs/terrain";
				break;
			default:
				break;
			}
#endif
		}
		
		tv[0]=org[0]+(j-(xs/2))*step;
		tv[1]=org[1]+(i-(ys/2))*step;
		tv[2]=org[2];
		
		tvm[0]=tv[0]-(step/2);
		tvm[1]=tv[1]-(step/2);
		tvm[2]=tv[2]-64;

		tvn[0]=tv[0]+(step/2);
		tvn[1]=tv[1]+(step/2);
		tvn[2]=tv[2]+0;
		
		switch(k)
		{
		case 7:
			break;
		case 15:
			tvn[2]=tv[2]+256;
			break;
		case 9:
			tvm[2]=tv[2]-8192;
			tvn[2]=tv[2]+1024;
			break;
		case 37:
			tvm[2]=tv[2]+256-32;
			tvn[2]=tv[2]+256;
			break;
		default:
			break;
		}

		btmp=BTGE_Brush_MakeCubeExtents(
			tvm[0], tvm[1], tvm[2],
			tvn[0], tvn[1], tvn[2],
			tex);
		blst=btmp;

		if(k==54)
		{
			btmp=BTGE_Brush_MakeCubeExtents(
				tvm[0], tvm[1], tvn[2]+1024-32,
				tvn[0], tvn[1], tvn[2]+1024,
				tex);
			btmp->chain=blst; blst=btmp;
		}

		if(k==55)
		{
			btmp=BTGE_Brush_MakeCubeExtents(
				tvm[0], tvm[1], tvn[2]+256-32,
				tvn[0], tvn[1], tvn[2]+256,
				tex);
			btmp->chain=blst; blst=btmp;
		}

		BTGE_BrushWorld_AddBrushChainListTransient(world, blst);
	}
	
}
