#include <bttile2d.h>

byte *tile2d_map;
byte *tile2d_map_ents;
int tile2d_map_xs;
int tile2d_map_ys;

char *Tile2D_TexNameForColor(byte *clr)
{
	char *s;
	
	s=NULL;
	if((clr[0]==128) && (clr[1]==0) && (clr[2]==0))
		{ s="textures/voxel/dirt"; }
	if((clr[0]==0) && (clr[1]==128) && (clr[2]==0))
		{ s="textures/voxel/grass"; }
		
	return(s);
}

char *Tile2D_TexNameForPoint(float x, float y)
{
	char *s;
	int i, j;
	
	i=((int)floor(x))+(tile2d_map_xs/2);
	j=((int)floor(y))+(tile2d_map_ys/2);
	if(i<0)i=0;
	if(j<0)j=0;
	if(i>=tile2d_map_xs)i=tile2d_map_xs-1;
	if(j>=tile2d_map_ys)j=tile2d_map_ys-1;

	s=Tile2D_TexNameForColor(tile2d_map+(j*tile2d_map_xs+i)*4);
	return(s);
}

char *Tile2D_TexNameForBox(float x, float y, float xs, float ys)
{
	char *s;

	s=Tile2D_TexNameForPoint(x, y);
	if(s)return(s);
	s=Tile2D_TexNameForPoint(x+xs, y);
	if(s)return(s);

	s=Tile2D_TexNameForPoint(x, y+ys);
	if(s)return(s);
	s=Tile2D_TexNameForPoint(x+xs, y+ys);
	return(s);
}

char *Tile2D_EntityNameForColor(byte *clr)
{
	char *s;
	
	s=NULL;

	if((clr[0]==0) && (clr[1]==0) && (clr[2]==0))
		return(NULL);
	if(clr[3]<=64)
		return(NULL);

	if((clr[0]==128) && (clr[1]==0) && (clr[2]==0))
		{ s="monster_pedobear"; }
	if((clr[0]==255) && (clr[1]==0) && (clr[2]==0))
		{ s="monster_giantbear"; }

//	if((clr[0]==0) && (clr[1]==128) && (clr[2]==0))
//		{ s="textures/voxel/grass"; }
		
	return(s);
}
