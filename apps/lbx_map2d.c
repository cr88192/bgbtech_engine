// #include <windows.h>
// #include <GL/gl.h>

#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#ifndef M_PI
#define M_PI		3.1415926535897932384626433832795
#endif

#include <pdgl.h>
#include <pdglui.h>
#include <pdgl_stub.h>

#include <lbxgl2.h>
#include <libvecmath.h>

#include <bgbasm.h>
#include <bgbgc.h>
#include <bgbdy.h>
#include <bgbccc.h>

typedef struct Map2D_Sector_s Map2D_Sector;
typedef struct Map2D_Wall_s Map2D_Wall;
typedef struct Map2D_Side_s Map2D_Side;
typedef struct Map2D_Vertex_s Map2D_Vertex;

struct Map2D_Sector_s
{
// Map2D_Sector *next;
int height_floor;
int height_ceil;
int tex_floor;
int tex_ceil;
};

struct Map2D_Wall_s
{
int vec0;	//first vertex
int vec1;	//second vertex
int side0;	//first side (wall a->b)
int side1;	//second side (wall b->a)
};

struct Map2D_Side_s
{
int sector;	//sector
int tex_low;
int tex_mid;
int tex_high;
};

struct Map2D_Vertex_s
{
int x, y;
};

Map2D_Sector **map2d_sector;
Map2D_Wall **map2d_wall;
Map2D_Side **map2d_side;
Map2D_Vertex **map2d_vertex;

int map2d_n_sector;
int map2d_n_wall;
int map2d_n_side;
int map2d_n_vertex;

int map2d_m_sector;
int map2d_m_wall;
int map2d_m_side;
int map2d_m_vertex;


int Map2D_IndexVertex(int x, int y)
{
	int i;

	for(i=0; i<map2d_n_vertex; i++)
	{
		if(map2d_vertex[i]->x != x)
			continue;
		if(map2d_vertex[i]->y != y)
			continue;
		return(i);
	}

	if(map2d_n_vertex>=map2d_m_vertex)
	{
		i=map2d_m_vertex+(map2d_m_vertex>>1);
		map2d_vertex=gcrealloc(map2d_vertex,
			i*sizeof(Map2D_Vertex *));
		map2d_m_vertex=i;
	}

	i=map2d_n_vertex++;
	map2d_vertex[i]=gcalloc(sizeof(Map2D_Vertex));
	map2d_vertex[i]->x=x;
	map2d_vertex[i]->y=y;

	return(i);
}

int Map2D_IndexWall(int v0, int v1)
{
	int i;

	for(i=0; i<map2d_n_wall; i++)
	{
		if((map2d_wall[i]->vec0==v0) && (map2d_wall[i]->vec1==v1))
			return(i);
		if((map2d_wall[i]->vec0==v1) && (map2d_wall[i]->vec1==v0))
			return(i);
	}

	if(map2d_n_wall>=map2d_m_wall)
	{
		i=map2d_m_wall+(map2d_m_wall>>1);
		map2d_wall=gcrealloc(map2d_wall,
			i*sizeof(Map2D_Wall *));
		map2d_m_wall=i;
	}

	i=map2d_n_wall++;
	map2d_wall[i]=gcalloc(sizeof(Map2D_Wall));
	map2d_wall[i]->vec0=v0;
	map2d_wall[i]->vec1=v1;
	return(i);
}

int Map2D_IndexWallSide(int v0, int v1, int side)
{
	int w0;

	w0=Map2D_IndexWall(v0, v1);
	if(map2d_wall[w0]->vec0==v0)
	{
		map2d_wall[w0]->side0=side;
	}else
	{
		map2d_wall[w0]->side1=side;
	}

	return(w0);
}

int Map2D_IndexSide(int sector, char *low, char *mid, char *high)
{
	int ltex, mtex, htex;
	int i;

	ltex=0; mtex=0; htex=0;

	if(low && strcmp(low, "-"))
		ltex=LBXGL_Texture_LoadImage(low);
	if(mid && strcmp(mid, "-"))
		mtex=LBXGL_Texture_LoadImage(mid);
	if(high && strcmp(high, "-"))
		htex=LBXGL_Texture_LoadImage(high);

	for(i=0; i<map2d_n_side; i++)
	{
		if(map2d_side[i]->sector != sector)
			continue;
		if(map2d_side[i]->tex_low != ltex)
			continue;
		if(map2d_side[i]->tex_mid != mtex)
			continue;
		if(map2d_side[i]->tex_high != htex)
			continue;
		return(i);
	}

	if(map2d_n_side>=map2d_m_side)
	{
		i=map2d_m_side+(map2d_m_side>>1);
		map2d_side=gcrealloc(map2d_side,
			i*sizeof(Map2D_Side *));
		map2d_m_side=i;
	}

	i=map2d_n_side++;
	map2d_side[i]=gcalloc(sizeof(Map2D_Side));
	map2d_side[i]->sector=sector;
	map2d_side[i]->tex_low=ltex;
	map2d_side[i]->tex_mid=mtex;
	map2d_side[i]->tex_high=htex;
	return(i);
}

int Map2D_CreateSector(int hfloor, int hceil, char *floortex, char *ceiltex)
{
	int ftex, ctex;
	int i;

	ftex=0; ctex=0;

	if(floortex && strcmp(floortex, "-"))
		ftex=LBXGL_Texture_LoadImage(floortex);
	if(ceiltex && strcmp(ceiltex, "-"))
		ctex=LBXGL_Texture_LoadImage(ceiltex);

	if(map2d_n_sector>=map2d_m_sector)
	{
		i=map2d_m_sector+(map2d_m_sector>>1);
		map2d_sector=gcrealloc(map2d_sector,
			i*sizeof(Map2D_Sector *));
		map2d_m_sector=i;
	}

	i=map2d_n_sector++;
	map2d_sector[i]=gcalloc(sizeof(Map2D_Sector));
	map2d_sector[i]->height_floor=hfloor;
	map2d_sector[i]->height_ceil=hceil;
	map2d_sector[i]->tex_floor=ftex;
	map2d_sector[i]->tex_ceil=ctex;
	return(i);
}

int Map2D_FindWallSector(int last, int sector)
{
	Map2D_Side *sd0, *sd1;
	int i;

	for(i=last+1; i<map2d_n_wall; i++)
	{
		sd0=map2d_side[map2d_wall[i]->side0];
		sd1=map2d_side[map2d_wall[i]->side1];
		if(sd0->sector==sector)return(i);
		if(sd1->sector==sector)return(i);
	}
	return(-1);
}

int Map2D_Init()
{
	map2d_sector=gcalloc(256*sizeof(Map2D_Sector *));
	map2d_wall=gcalloc(256*sizeof(Map2D_Wall *));
	map2d_side=gcalloc(256*sizeof(Map2D_Side *));
	map2d_vertex=gcalloc(256*sizeof(Map2D_Vertex *));

	map2d_n_sector=0;
	map2d_n_wall=0;
	map2d_n_side=0;
	map2d_n_vertex=0;

	map2d_m_sector=256;
	map2d_m_wall=256;
	map2d_m_side=256;
	map2d_m_vertex=256;
}

int Map2D_TstWorld()
{
	int v0, v1, v2, v3, v4, v5;
	int w0, w1, w2, w3;
	int s0, s1;
	int sd0, sd1, sd2, sd3;

	Map2D_CreateSector(-64, 64, NULL, NULL);
	Map2D_IndexSide(0, NULL, NULL, NULL);

	s0=Map2D_CreateSector(-64, 64,
		"textures/ground/gimp_ground1",
		"textures/misc/sky");

	s1=Map2D_CreateSector(-48, 48,
		"textures/ground/gimp_ground1",
		"textures/misc/sky");

	v0=Map2D_IndexVertex(-256, -256);
	v1=Map2D_IndexVertex(256, -256);
	v2=Map2D_IndexVertex(256, 256);
	v3=Map2D_IndexVertex(-256, 256);

	v4=Map2D_IndexVertex(384, -256);
	v5=Map2D_IndexVertex(384, 256);

	sd0=Map2D_IndexSide(s0, NULL, "textures/cs/oldbrick", NULL);
	sd1=Map2D_IndexSide(s1, NULL, "textures/cs/oldbrick", NULL);

	sd2=Map2D_IndexSide(s0,
		"textures/cs/marble_13", NULL, "textures/cs/marble_09");
	sd3=Map2D_IndexSide(s1, NULL, NULL, NULL);

	Map2D_IndexWallSide(v0, v1, sd0);
	Map2D_IndexWallSide(v1, v2, sd2);
	Map2D_IndexWallSide(v2, v3, sd0);
	Map2D_IndexWallSide(v3, v0, sd0);

	Map2D_IndexWallSide(v1, v4, sd1);
	Map2D_IndexWallSide(v4, v5, sd1);
	Map2D_IndexWallSide(v5, v2, sd1);
	Map2D_IndexWallSide(v2, v1, sd3);

	printf("%d sectors\n", map2d_n_sector);
	printf("%d vertices\n", map2d_n_vertex);
	printf("%d walls\n", map2d_n_wall);
	printf("%d sides\n", map2d_n_side);
}


int Map2D_DrawWallMid(
	Map2D_Vertex *vtx0, Map2D_Vertex *vtx1,
	Map2D_Side *side, Map2D_Sector *sec)
{
	int i, j, k, len, base, xs, ys;
	float s0, s1, t0, t1;

	i=vtx1->x-vtx0->x; j=vtx1->y-vtx0->y; k=i*i+j*j;
	len=sqrt(k);
	i=vtx0->x; j=vtx0->y; k=i*i+j*j;
	base=sqrt(k);

	xs=64; ys=64;
	LBXGL_Texture_GetImageSize(side->tex_mid, &xs, &ys);
	s0=base/((float)xs);
	s1=(base+len)/((float)xs);
	t0=sec->height_floor/((float)ys);
	t1=sec->height_ceil/((float)ys);

	pdglColor4f(1, 1, 1, 1);

	pdglBindTexture(GL_TEXTURE_2D, side->tex_mid);
	pdglEnable(GL_TEXTURE_2D);

	pdglBegin(GL_POLYGON);
	pdglTexCoord2f(s0, t1);
	pdglVertex3f(vtx0->x, vtx0->y, sec->height_ceil);
	pdglTexCoord2f(s0, t0);
	pdglVertex3f(vtx0->x, vtx0->y, sec->height_floor);
	pdglTexCoord2f(s1, t0);
	pdglVertex3f(vtx1->x, vtx1->y, sec->height_floor);
	pdglTexCoord2f(s1, t1);
	pdglVertex3f(vtx1->x, vtx1->y, sec->height_ceil);
	pdglEnd();
}

int Map2D_DrawWallLow(
	Map2D_Vertex *vtx0, Map2D_Vertex *vtx1,
	Map2D_Side *side, Map2D_Sector *sec, Map2D_Sector *sec1)
{
	int i, j, k, len, base, xs, ys;
	float s0, s1, t0, t1;

	i=vtx1->x-vtx0->x; j=vtx1->y-vtx0->y; k=i*i+j*j;
	len=sqrt(k);
	i=vtx0->x; j=vtx0->y; k=i*i+j*j;
	base=sqrt(k);

	xs=64; ys=64;
	LBXGL_Texture_GetImageSize(side->tex_low, &xs, &ys);
	s0=base/((float)xs);
	s1=(base+len)/((float)xs);
	t0=sec->height_floor/((float)ys);
	t1=sec1->height_floor/((float)ys);

	pdglColor4f(1, 1, 1, 1);

	pdglBindTexture(GL_TEXTURE_2D, side->tex_low);
	pdglEnable(GL_TEXTURE_2D);

	pdglBegin(GL_POLYGON);
	pdglTexCoord2f(s0, t1);
	pdglVertex3f(vtx0->x, vtx0->y, sec1->height_floor);
	pdglTexCoord2f(s0, t0);
	pdglVertex3f(vtx0->x, vtx0->y, sec->height_floor);
	pdglTexCoord2f(s1, t0);
	pdglVertex3f(vtx1->x, vtx1->y, sec->height_floor);
	pdglTexCoord2f(s1, t1);
	pdglVertex3f(vtx1->x, vtx1->y, sec1->height_floor);
	pdglEnd();
}

int Map2D_DrawWallHigh(
	Map2D_Vertex *vtx0, Map2D_Vertex *vtx1,
	Map2D_Side *side, Map2D_Sector *sec, Map2D_Sector *sec1)
{
	int i, j, k, len, base, xs, ys;
	float s0, s1, t0, t1;

	i=vtx1->x-vtx0->x; j=vtx1->y-vtx0->y; k=i*i+j*j;
	len=sqrt(k);
	i=vtx0->x; j=vtx0->y; k=i*i+j*j;
	base=sqrt(k);

	xs=64; ys=64;
	LBXGL_Texture_GetImageSize(side->tex_high, &xs, &ys);
	s0=base/((float)xs);
	s1=(base+len)/((float)xs);
	t0=sec1->height_ceil/((float)ys);
	t1=sec->height_ceil/((float)ys);

	pdglColor4f(1, 1, 1, 1);

	pdglBindTexture(GL_TEXTURE_2D, side->tex_high);
	pdglEnable(GL_TEXTURE_2D);

	pdglBegin(GL_POLYGON);
	pdglTexCoord2f(s0, t1);
	pdglVertex3f(vtx0->x, vtx0->y, sec->height_ceil);
	pdglTexCoord2f(s0, t0);
	pdglVertex3f(vtx0->x, vtx0->y, sec1->height_ceil);
	pdglTexCoord2f(s1, t0);
	pdglVertex3f(vtx1->x, vtx1->y, sec1->height_ceil);
	pdglTexCoord2f(s1, t1);
	pdglVertex3f(vtx1->x, vtx1->y, sec->height_ceil);
	pdglEnd();
}

int Map2D_DrawWall(int wall)
{
	Map2D_Vertex *vtx0, *vtx1;
	Map2D_Side *sd0, *sd1;
	Map2D_Sector *sec0, *sec1;
	int i, j, k, len, base, xs, ys;
	float s0, s1, t0, t1;

	sd0=NULL; sd1=NULL;
	sec0=NULL; sec1=NULL;

	vtx0=map2d_vertex[map2d_wall[wall]->vec0];
	vtx1=map2d_vertex[map2d_wall[wall]->vec1];
	if(map2d_wall[wall]->side0>0)
		sd0=map2d_side[map2d_wall[wall]->side0];
	if(map2d_wall[wall]->side1>0)
		sd1=map2d_side[map2d_wall[wall]->side1];
	if(sd0 && sd0->sector>0)
		sec0=map2d_sector[sd0->sector];
	if(sd1 && sd1->sector>0)
		sec1=map2d_sector[sd1->sector];

	if(sd0 && sec0 && (sd0->tex_mid>0))
		Map2D_DrawWallMid(vtx0, vtx1, sd0, sec0);

	if(sd1 && sec1 && (sd1->tex_mid>0))
		Map2D_DrawWallMid(vtx1, vtx0, sd1, sec1);

	if(sec0 && sec1 && sec0->height_floor<sec1->height_floor)
		Map2D_DrawWallLow(vtx0, vtx1, sd0, sec0, sec1);
	if(sec0 && sec1 && sec1->height_floor<sec0->height_floor)
		Map2D_DrawWallLow(vtx1, vtx0, sd1, sec1, sec0);

	if(sec0 && sec1 && sec0->height_ceil>sec1->height_ceil)
		Map2D_DrawWallHigh(vtx0, vtx1, sd0, sec0, sec1);
	if(sec0 && sec1 && sec1->height_ceil>sec0->height_ceil)
		Map2D_DrawWallHigh(vtx1, vtx0, sd1, sec1, sec0);
}

int Map2D_DrawSector(int sector)
{
	int wall[64], org[2];
	Map2D_Vertex *vtx0, *vtx1;
	Map2D_Sector *sec;
	int nwall;
	int i;

	nwall=0; i=-1;
	while(1)
	{
		i=Map2D_FindWallSector(i, sector);
		if(i<0)break;
		wall[nwall++]=i;
	}

	if(!nwall)return(-1);

	org[0]=0; org[1]=0;
	for(i=0; i<nwall; i++)
	{
		vtx0=map2d_vertex[map2d_wall[wall[i]]->vec0];
		vtx1=map2d_vertex[map2d_wall[wall[i]]->vec1];

		org[0]+=vtx0->x;
		org[0]+=vtx1->x;
		org[1]+=vtx0->y;
		org[1]+=vtx1->y;
	}

	org[0]/=nwall*2;
	org[1]/=nwall*2;

	sec=map2d_sector[sector];

	pdglColor4f(1, 1, 1, 1);

	pdglBindTexture(GL_TEXTURE_2D, sec->tex_floor);
	pdglEnable(GL_TEXTURE_2D);

	pdglBegin(GL_TRIANGLES);
	for(i=0; i<nwall; i++)
	{
		vtx0=map2d_vertex[map2d_wall[wall[i]]->vec0];
		vtx1=map2d_vertex[map2d_wall[wall[i]]->vec1];
		pdglTexCoord2f(vtx0->x/64.0, vtx0->y/64.0);
		pdglVertex3f(vtx0->x, vtx0->y, sec->height_floor);
		pdglTexCoord2f(vtx1->x/64.0, vtx1->y/64.0);
		pdglVertex3f(vtx1->x, vtx1->y, sec->height_floor);
		pdglTexCoord2f(org[0]/64.0, org[1]/64.0);
		pdglVertex3f(org[0], org[1], sec->height_floor);
	}
	pdglEnd();

	pdglBindTexture(GL_TEXTURE_2D, sec->tex_ceil);
	pdglEnable(GL_TEXTURE_2D);

	pdglBegin(GL_TRIANGLES);
	for(i=0; i<nwall; i++)
	{
		vtx0=map2d_vertex[map2d_wall[wall[i]]->vec0];
		vtx1=map2d_vertex[map2d_wall[wall[i]]->vec1];

		pdglTexCoord2f(vtx0->x/64.0, vtx0->y/64.0);
		pdglVertex3f(vtx0->x, vtx0->y, sec->height_ceil);
		pdglTexCoord2f(vtx1->x/64.0, vtx1->y/64.0);
		pdglVertex3f(vtx1->x, vtx1->y, sec->height_ceil);
		pdglTexCoord2f(org[0]/64.0, org[1]/64.0);
		pdglVertex3f(org[0], org[1], sec->height_ceil);
	}
	pdglEnd();

//	pdglTexCoord2f(0, 1);
//	pdglVertex3f(vtx0->x, vtx0->y, sec->height_floor);
//	pdglTexCoord2f(0, 0);
//	pdglVertex3f(vtx0->x, vtx0->y, sec->height_floor);
//	pdglTexCoord2f(1, 0);
//	pdglVertex3f(vtx1->x, vtx1->y, sec->height_floor);
//	pdglTexCoord2f(1, 1);
//	pdglVertex3f(vtx1->x, vtx1->y, sec->height_floor);
}

int Map2D_DrawWorld()
{
	int i;

	for(i=0; i<map2d_n_wall; i++)
		Map2D_DrawWall(i);

	for(i=1; i<map2d_n_sector; i++)
		Map2D_DrawSector(i);
}

int pdgl_main_startup(int argc, char *argv[])
{
	char *uds[64], *args[256];
	void *p;
	int n, m, nargs;
	int i;

	GfxFont_Init();
	Con_Init();
	ConCmds_Init();

	PDGLUI_Init();
	UI_Camera_Init();

	Map2D_Init();
	Map2D_TstWorld();

	nargs=0;
	args[nargs++]=argv[0];

	n=0; m=0;
	for(i=1; i<argc; i++)
	{
		if(argv[i][0]=='-')
		{
			if(!strcmp(argv[i], "--"))
				{ i++; break; }
			if(!strcmp(argv[i], "--pc"))
				m|=1;
			continue;
		}
		uds[n++]=argv[i];
	}

	nargs=0; args[nargs++]=argv[0];
	for(; i<argc; i++)args[nargs++]=argv[i];

#ifdef WIN32
	ccLoadLibrary("msvcrt");
#endif

	for(i=0; i<n; i++)
		ccLoadModule(uds[i]);

//	for(i=1; i<argc; i++)
//		ccLoadModule(argv[i]);

	p=ccGetAddr("runc_start");
	if(p)
	{
		printf("Got %p\n", p);
		i=((int(*)(int argc, char **argv))p)(nargs, args);
		return(i);
	}

	return(1);
}

int main_handle_input()
{
	char buf[64];
	void *p;
	int i, j, k;

	if(Con_IsDown())
		return(0);

	if(PDGLUI_HasFocus())
		return(0);

#if 0
	p=ccGetAddr("runc_handle_input");
	if(p)
	{
		printf("Got %p\n", p);
		i=((int(*)())p)();
		if(i)return(i);
	}
#endif

	UI_Camera_Update();

	return(0);
}

int pdgl_main_body()
{
	void *p;
	int i, j, k, ret;

	ret=0;

	i=main_handle_input();
	if(i)ret=i;

	UI_Camera_Setup3D();

	pdglDisable(GL_CULL_FACE);
	pdglDisable(GL_TEXTURE_2D);
	PD3D_DrawGrid(0, 0, 16, 1);

	Map2D_DrawWorld();

#if 0
	p=ccGetAddr("runc_draw3d");
	if(p)
	{
//		printf("Got %p\n", p);
		i=((int(*)())p)();
		if(i)ret=i;
	}
#endif

	UI_Camera_Setup2D();

#if 0
	p=ccGetAddr("runc_draw2d");
	if(p)
	{
//		printf("Got %p\n", p);
		i=((int(*)())p)();
		if(i)ret=i;
	}
#endif

	Con_Render();
	PDGLUI_Frame();

	pdglDisable(GL_TEXTURE_2D);
	pdglColor4f(1, 0, 0, 1);

	pdglBegin(GL_LINES);
	pdglVertex2f(state->mx-10, -state->my);
	pdglVertex2f(state->mx+10, -state->my);
	pdglVertex2f(state->mx, -state->my-10);
	pdglVertex2f(state->mx, -state->my+10);
	pdglEnd();

	return(0);
}

int pdgl_main_shutdown()
{
	void *p;
	int i;

	p=ccGetAddr("runc_shutdown");
	if(p)
	{
		i=((int(*)())p)();
		return(i);
	}

	printf("shutdown\n");
	return(0);
}
