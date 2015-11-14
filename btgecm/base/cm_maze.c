
#include <btgecm.h>

/**
 * Generate a maze as a byte-array.
 * 
 * Output Byte Values:
 * 0=unvisited;
 * 1=empty;
 * 2=wall (generic);
 * 3=start;
 * 4=end.
 * 5=wall-A (X-axis);
 * 6=wall-B (Y-axis);
 * 7=wall-C (cross);
 * 
*/

/** Random number, updating passed seed (32-bits) */
BTGE_API int BTGE_QuickRand(int *rseed)
{
	int v;
	
	v=((*rseed)>>16)&0xFFFF;
	*rseed=(*rseed*65521)+1;
	return(v);
}

BTGE_API int BTGE_QuickRangeRand(int *rseed, int min, int max)
{
	int i;
	
	if(max<min)return(min);
	i=BTGE_QuickRand(rseed);
	return(i%(max-min+1));
}

BTGE_API bool BTGE_CheckPointMaze(
	int xs, int ys, int px, int py)
{
	if((px>1) && (py>1) && (px<(xs-1)) && (py<(ys-1)))
		return(true);
	return(false);
}

BTGE_API bool BTGE_CheckNewPointMaze(
	byte *buf, int xs, int ys, int px, int py)
{
	if((px>=0) && (py>=0) && (px<xs) && (py<ys))
	{
		if(!buf[py*xs+px])
			return(true);
		return(false);
	}
	return(false);
}

BTGE_API int BTGE_GetPointMaze(
	byte *buf, int xs, int ys, int px, int py)
{
	if((px>=0) && (py>=0) && (px<xs) && (py<ys))
		return(buf[py*xs+px]);
	return(-1);
}

BTGE_API bool BTGE_CheckWallPointMaze(
	byte *buf, int xs, int ys, int px, int py)
{
	int i;
	
	i=BTGE_GetPointMaze(buf, xs, ys, px, py);
	if(i==2)return(true);
	if(i==5)return(true);
	if(i==6)return(true);
	if(i==7)return(true);
	return(false);
}

BTGE_API void BTGE_GenerateMaze(byte *buf,
	int xs, int ys, int seed, int flags)
{
	static short stack[128*128];
	static int dirs[16]=
//		{1,0, 0, 1, -1,0,  0,-1,
		{-1,0, 1, 0, 0,-1, 0, 1,
		  1,1, 1,-1,-1, 1,-1,-1};
	int stackpos, cseed;
	int px, py, spx, spy, epx, epy, tpx, tpy;
	int i, j, k;
	
	//setup
	memset(buf, 0, xs*ys);
	cseed=seed; stackpos=0;
	for(i=0; i<256; i++)
		BTGE_QuickRand(&cseed);

	//border walls
	for(i=0; i<xs; i++)
		{ buf[i]=2; buf[(ys-1)*xs+i]=2; }
	for(i=0; i<ys; i++)
		{ buf[i*xs+0]=2; buf[i*xs+(xs-1)]=2; }

	//pick start point
	while(1)
	{
		spx=BTGE_QuickRand(&cseed)%xs;
		spy=BTGE_QuickRand(&cseed)%ys;
		if(BTGE_CheckNewPointMaze(buf, xs, ys, spx, spy))
			break;
	}
	buf[spy*xs+spx]=3;

	//pick end point
	while(1)
	{
		epx=BTGE_QuickRand(&cseed)%xs;
		epy=BTGE_QuickRand(&cseed)%ys;
		if(BTGE_CheckNewPointMaze(buf, xs, ys, epx, epy))
			break;
	}	
	buf[epy*xs+epx]=4;
	
	//generate maze
	px=epx;
	py=epy;
	stack[stackpos++]=py*xs+px;
//	while(stackpos)
	while(1)
	{
#if 1
		if(!stackpos)
		{
			for(i=0; i<xs*ys; i++)
				if(!buf[i])
			{
				stack[stackpos++]=i;
				break;
			}
		}
#endif

		if(!stackpos)
			break;
		i=stack[stackpos-1];
		px=i%xs; py=i/xs;
		
//		if(buf[py*xs+px])
//			continue;

#if 1
		if(!buf[py*xs+px])
		{
			j=0;
			for(i=0; i<4; i++)
			{
				tpx=px+dirs[i*2+0];
				tpy=py+dirs[i*2+1];
				k=buf[tpy*xs+tpx];
//				if((k==0) || (k==1))
				if(k==1)
					j|=1<<i;
			}
			switch(j)
			{
				case 0: k=0; break;
				case 1: case 2:
				case 4: case 8:
					k=0; break;
				
				case 3: k=6; break;
				case 12: k=5; break;
				default: k=7; break;
			}
			if(k)
			{
				buf[py*xs+px]=k;
				stackpos--;
				continue;
			}
		}
#endif

#if 0
		if(!buf[py*xs+px])
		{
			j=0;
			for(i=0; i<4; i++)
			{
				tpx=px+dirs[i*2+0];
				tpy=py+dirs[i*2+1];
				if(buf[tpy*xs+tpx]==1)
					j++;
			}
			if(j>=2)
			{
				buf[py*xs+px]=2;
				stackpos--;
				continue;
			}
		}
#endif

		if(!buf[py*xs+px])
			buf[py*xs+px]=1;

		for(j=0; j<8; j++)
		{
			i=BTGE_QuickRand(&cseed)&3;
			tpx=px+dirs[i*2+0];
			tpy=py+dirs[i*2+1];
			if(BTGE_CheckNewPointMaze(buf, xs, ys, tpx, tpy))
			{
				stack[stackpos++]=tpy*xs+tpx;
//				buf[tpy*xs+tpx]=1;
				break;
			}
		}

		if(j>=8)stackpos--;
	}
}

BTGE_API void BTGE_GenerateMaze3D(byte *buf,
	int xs, int ys, int zs, int seed, int flags)
{
	memset(buf, 0, xs*ys*zs);
	BTGE_GenerateMaze(buf, xs, ys, seed, flags);
}
