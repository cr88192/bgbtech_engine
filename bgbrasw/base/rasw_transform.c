#include <bgbrasw.h>

void RASW_DrawArrays(
	int prim, int base, int nxyz,
	int xyzsize, float *xyz, 
	int stsize, float *st, 
	int normsize, float *norm,
	int rgbsize, float *rgb)
{
	float tvmv[4], tvp[4];
	int i, j, k;
	
	for(i=0; i<nxyz; i++)
	{
	}
}

void RASWGL_DrawElements(
	int prim, int nelem, int elemtype, void *elems,
	int xyzsize, int xyztype, int xyzstep, void *xyz, 
	int stsize, int sttype, int ststep, void *st, 
	int normsize, int normtype, int normstep, void *norm,
	int rgbsize, int rgbtype, int rgbstep, void *rgb)
{
	PDGL_DrawPrim_DrawElementsNormalTexRGB(
		prim, nelem, elemtype, elems,
		xyzsize, xyztype, xyzstep, xyz,
		stsize, sttype, ststep, st,
		normsize, normtype, normstep, norm,
		rgbsize, rgbtype, rgbstep, rgb);
}
