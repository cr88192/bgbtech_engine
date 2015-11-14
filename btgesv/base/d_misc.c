#include <btgesv.h>

BTSV_API double btSEntGetNum(btSEntity ent, char *name)
	{ return(BTGE_SEnt_GetNum(ent, name)); }
BTSV_API s64 btSEntGetInt(btSEntity ent, char *name)
	{ return(BTGE_SEnt_GetNum(ent, name)); }

BTSV_API vec3 btSEntGetVec3(btSEntity ent, char *name)
{
	float tv[4];
	int i;
	i=BTGE_SEnt_GetVec3(ent, name, tv);
	if(i<0)return(vec3(0,0,0));
	return(vec3vf(tv));
}

BTSV_API vec3 btSEntGetVec4(btSEntity ent, char *name)
{
	float tv[4];
	int i;
	i=BTGE_SEnt_GetVec4(ent, name, tv);
	if(i<0)return(vec4(0,0,0,0));
	return(vec4vf(tv));
}

BTSV_API void BtSg_func_duplicator(btWorld wrl, btSEntity args)
{
	float tv[4], tv1[4];
	BTGE_Brush *bcur, *blst;
	int i, j, n;

	BTGE_SEnt_GetVec3(args, "offset", tv);
	n=BTGE_SEnt_GetNum(args, "count");
	
	if(n<=0)return;

	for(i=0; i<n; i++)
	{
		V3F_SCALE(tv, i, tv1);
	
		bcur=args->brush; blst=NULL;
		while(bcur)
			{ bcur->chain=blst; blst=bcur; bcur=bcur->enext; }
		blst=BTGE_Brush_CloneBrushChainList(blst);
		BTGE_Brush_TranslateBrushChainList(blst, tv1);
		BTGE_BrushWorld_AddBrushChainListTransient(wrl, blst);
	}
}
