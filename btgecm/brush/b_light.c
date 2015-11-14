#include <btgecm.h>

BTGE_Brush *BTGE_BrushWorld_QueryBrushesLight(
	BTGE_BrushWorld *world, BTGE_Brush *fst, BTGE_Light *light)
{
	float vorg[3], sdir[3];
	float tv0[3];
	BTGE_Brush *cur, *nxt, *lst, *lstb;
	float f, g, h, d, lr, vr, sd;
	int i;

	world->shadowcam=0;

//	lr=BTGE_Shadow_LightGammaEffectRadius(light, 0.10);
	lr=light->val[0];

	lst=BTGE_BrushBSP_QueryTreeNode(world->bsp, light->org, lr);

//	V3F_SCALEADDSCALE(world->autovis_mins, 0.5,
//		world->autovis_maxs, 0.5, vorg);
//	vr=V3F_DIST(world->autovis_maxs, vorg);

#if 1
	cur=lst; lstb=NULL;
	while(cur)
	{
		nxt=cur->chain;

		if(cur->flags&(BTGE_BRFL_NOSHADOW|BTGE_BRFL_EFFECT))
			{ cur=nxt; continue; }

//		if(!BTGE_Brush_CheckBrushShadowFrustum(cur, light->org))
//			{ cur=nxt; continue; }

		cur->chain=lstb; lstb=cur;
		cur=nxt;
	}
	lst=lstb;
#endif

	return(lst);
}

BTGE_Brush *BTGE_BrushWorld_QueryBrushesLight2(
	BTGE_BrushWorld *world, BTGE_Brush *fst, BTGE_Light *light)
{
	BTGE_Brush *cur, *nxt, *lst, *lstb;
	float f, g, h, d;
	int i;

//	d=BTGE_Shadow_LightGammaEffectRadius(light, 0.10);
	d=light->val[0];
	lst=BTGE_BrushBSP_QueryTreeNode(world->bsp, light->org, d);

	cur=lst; lstb=NULL;
	while(cur)
	{
		nxt=cur->chain;

		if(cur->flags&BTGE_BRFL_EFFECT)
			{ cur=nxt; continue; }

		cur->chain=lstb; lstb=cur;
		cur=nxt;
	}

	return(lstb);
}

float BTGE_BrushWorld_SetupLightFrame(
	BTGE_BrushWorld *world, BTGE_Light *light)
{
	static char *lightstyle[]=
	{
	// 0, normal
		"m",	
	// 1, flicker
		"mmnmmommommnonmmonqnmmo", 
	// 2, slow strong pulse
		"abcdefghijklmnopqrstuvwxyzyxwvutsrqponmlkjihgfedcba",
	// 3, candle 1
		"mmmmmaaaaammmmmaaaaaabcdefgabcdefg",
	// 4, fast strobe
		"mamamamamama",
	// 5, gentle pulse
		"jklmnopqrstuvwxyzyxwvutsrqponmlkj",
	// 6, alternate flicker
		"nmonqnmomnmomomno",
	// 7, candle 2
		"mmmaaaabcdefgmmmmaaaammmaamm",
	// 8, candle 3
		"mmmaaammmaaammmabcdefaaaammmmabcdefmmmaaaa",
	// 9, slow strobe
		"aaaaaaaazzzzzzzz",
	// 10, flourescent flicker
		"mmamammmmammamamaaamammma",
	// 11, slow pulse absent black
		"abcdefghijklmnopqrrqponmlkjihgfedcba"
	};

	float f, g, h, d, gam;
	char *s;
	int i, j;

	gam=light->val[0];
	f=0;	//current time

	if((light->style>0) && (light->style<=11))
	{
		s=lightstyle[light->style];
		i=((int)(f*10))%strlen(s);
		gam*=(s[i]-'a')*(1.0/('m'-'a'));
	}

	light->curval=gam;
	return(gam);
}


BTGE_API BTGE_Light *BTGE_WorldNewLight(BTGE_BrushWorld *wrl)
{
	BTGE_Light *dl;

	dl=gctalloc("btge_light_t", sizeof(BTGE_Light));
	dl->next=wrl->light;
	wrl->light=dl;
	return(dl);
}

BTGE_API BTGE_Light *BTGE_WorldFetchLight(BTGE_BrushWorld *wrl, int id)
{
	BTGE_Light *cur;
	int i;

	cur=wrl->light;
	while(cur)
	{
		if(cur->num==id)
			return(cur);
		cur=cur->next;
	}
	return(NULL);
}


BTGE_API BTGE_Light *BTGE_WorldGetLight(BTGE_BrushWorld *wrl, int id)
{
	BTGE_Light *cur;
	int i;

	cur=BTGE_WorldFetchLight(wrl, id);
	if(cur)return(cur);

	cur=BTGE_WorldNewLight(wrl);
	cur->num=id;
	return(cur);
}

#if 0
// BTGE_API BTGE_Light *BTGE_AllocServerLight(BTGE_BrushWorld *wrl)
{
	BTGE_Light *tmp;
	int i, j;

	if(!wrl->light_mask)
		wrl->light_mask=gcatomic(65536/8);

//	j=wrl->light_rov;
//	for(i=j+1; i!=j; i=(i+1)&65535)
	for(i=0; i<65536; i++)
		if(!(wrl->light_mask[i>>3]&(1<<(i&7))))
			break;
	wrl->light_mask[i>>3]|=1<<(i&7);
	wrl->light_rov=i;
	
	tmp=BTGE_WorldGetLight(wrl, i|BT_LNUM_SERVER);
	return(tmp);
}
#endif

BTGE_API void BTGE_FreeServerLight(BTGE_BrushWorld *wrl,
	BTGE_Light *light)
{
	BTGE_Light *cur;

	if(light==wrl->light)
	{
		wrl->light=light->next;
	}else
	{
		cur=wrl->light;
		while(cur && (cur->next!=light))
			cur=cur->next;
		if(cur)
			cur->next=light->next;
	}
	
	gcfree(light);
}
