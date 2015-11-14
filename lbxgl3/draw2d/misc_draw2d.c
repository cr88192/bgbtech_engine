#include <lbxgl2.h>

LBXGL_BrushWorld *lbxgl_brush2d_world;
BTGE_DrawSequence2D *lbxgl_draw2d_editseq;

LBXGL_API void LBXGL_BrushDraw2D_BindWorld(
	LBXGL_BrushWorld *world)
{
	lbxgl_brush2d_world=world;
	
	LBXGL_Draw2DSeq_Init();
}

LBXGL_API LBXGL_BrushWorld *LBXGL_BrushDraw2D_GetBoundWorld(void)
	{ return(lbxgl_brush2d_world); }

LBXGL_API int LBXGL_BrushDraw2D_HandleInput(
	LBXGL_BrushWorld *world)
{
	BTGE_DrawableObject2D *cur, *lst, *cur1, *prv1;
	dyt t;
	int i;

	if(lbxgl_draw2d_editseq)
	{
		i=LBXGL_Draw2DSeq_HandleInput(lbxgl_draw2d_editseq);
		if(i)return(1);
	}

	cur=world->drawobj2d; lst=NULL;
	while(cur)
	{
		if(cur->flags&BTGE_DO2D_SKY)
			{ cur=cur->next; continue; }

		if(cur->ord<0)
			{ cur=cur->next; continue; }
	
		cur1=lst; prv1=NULL;
		while(cur1 && (cur1->ord<cur->ord))
			{ prv1=cur1; cur1=cur1->chain; }
		if(prv1)
			{ cur->chain=cur1; prv1->chain=cur; }
		else
			{ cur->chain=cur1; lst=cur; }
		cur=cur->next;
	}

	cur=lst;
	while(cur)
	{
		if(cur->dy_this)
		{
			t=dyCall1(cur->dy_this, "handleInput", (dyt)cur);
			if(t==BGBDY_TRUE)
				return(1);
			if(dyintp(t) && (dyintv(t)>0))
				return(1);
		}
		if(cur->HandleInput)
		{
			i=cur->HandleInput(cur);
			if(i>0)
				return(1);
		}
		cur=cur->chain;
	}
	return(0);
}

LBXGL_API void LBXGL_BrushDraw2D_DrawWorld2D(
	LBXGL_BrushWorld *world)
{
	BTGE_DrawableObject2D *cur, *lst, *cur1, *prv1;

	if(lbxgl_draw2d_editseq)
	{
		LBXGL_Draw2DSeq_DrawUI(lbxgl_draw2d_editseq);
	}

#if 0
	cur=world->drawobj2d;
	while(cur)
	{
		if(cur->Draw)
			{ cur->Draw(cur); }
		cur=cur->next;
	}
#endif

#if 1
	cur=world->drawobj2d; lst=NULL;
	while(cur)
	{
		if(cur->flags&BTGE_DO2D_SKY)
			{ cur=cur->next; continue; }

		if(cur->ord<0)
			{ cur=cur->next; continue; }
	
		cur1=lst; prv1=NULL;
		while(cur1 && (cur1->ord<cur->ord))
			{ prv1=cur1; cur1=cur1->chain; }
		if(prv1)
			{ cur->chain=cur1; prv1->chain=cur; }
		else
			{ cur->chain=cur1; lst=cur; }
		cur=cur->next;
	}

	cur=lst;
	while(cur)
	{
		if(cur->dy_this)
			dyCall1(cur->dy_this, "draw", (dyt)cur);
		if(cur->Draw)
			{ cur->Draw(cur); }
		cur=cur->chain;
	}
#endif
}

LBXGL_API void LBXGL_BrushDraw2D_DrawWorldSky2D(
	LBXGL_BrushWorld *world)
{
	BTGE_DrawableObject2D *cur, *lst, *cur1, *prv1;

	cur=world->drawobj2d; lst=NULL;
	while(cur)
	{
		if(!(cur->flags&BTGE_DO2D_SKY))
			{ cur=cur->next; continue; }

		if(cur->ord<0)
			{ cur=cur->next; continue; }
	
		cur1=lst; prv1=NULL;
		while(cur1 && (cur1->ord<cur->ord))
			{ prv1=cur1; cur1=cur1->chain; }
		if(prv1)
			{ cur->chain=cur1; prv1->chain=cur; }
		else
			{ cur->chain=cur1; lst=cur; }
		cur=cur->next;
	}

	cur=lst;
	while(cur)
	{
		if(cur->dy_this)
			dyCall1(cur->dy_this, "draw", (dyt)cur);
		if(cur->Draw)
			{ cur->Draw(cur); }
		cur=cur->chain;
	}
}

LBXGL_API void LBXGL_BrushDraw2D_UpdateWorld2D(
	LBXGL_BrushWorld *world, float dt)
{
	BTGE_DrawableObject2D *cur, *nxt;

	if(lbxgl_draw2d_editseq)
	{
		LBXGL_Draw2DSeq_UpdateUI(lbxgl_draw2d_editseq, dt);
	}

#if 1
	cur=world->drawobj2d;
	while(cur)
	{
		nxt=cur->next;
		if(cur->dy_this)
			dyCall2(cur->dy_this, "update", (dyt)cur, dyflonum(dt));
		if(cur->Update)
			{ cur->Update(cur, dt); }
		cur=nxt;
	}
#endif
}

LBXGL_API BTGE_DrawableObject2D *LBXGL_BrushDraw2D_CreateObject(
	LBXGL_BrushWorld *world)
{
	BTGE_DrawableObject2D *tmp;
	
	tmp=gctalloc("btge_drawableobject2d_t",
		sizeof(BTGE_DrawableObject2D));
	tmp->world=world;
	tmp->next=world->drawobj2d;
	world->drawobj2d=tmp;
	
	return(tmp);
}

LBXGL_API void LBXGL_BrushDraw2D_DestroyObj(
	BTGE_DrawableObject2D *obj)
{
	BTGE_DrawableObject2D *cur, *prv;
	
	cur=obj->world->drawobj2d; prv=NULL;
	while(cur)
	{
		if(cur==obj)break;
		prv=cur; cur=cur->next;
	}
	
	if(cur)
	{
		if(prv)
		{
			prv->next=cur->next;
			gcfree(cur);
		}else
		{
			obj->world->drawobj2d=cur->next;
			if(cur->dy_this)
				dyCall1(cur->dy_this, "destroy", (dyt)cur);
			if(cur->Destroy)
				{ cur->Destroy(cur); }
			gcfree(cur);
		}
	}
}

LBXGL_API void LBXGL_BrushDraw2D_DrawImage(
	BTGE_DrawableObject2D *obj)
{
	float ox, oy, xs, ys;
	
	ox=v2x(obj->org);
	oy=v2y(obj->org);
	xs=v2x(obj->size);
	ys=v2y(obj->size);

	if(obj->sound_chan>0)
		LBXGL_Shader_SetCurrentMixerChannel(obj->sound_chan);

	LBXGL_Shader_Color4f(1, 1, 1, 1);
//	glEnable(GL_TEXTURE_2D);
//	glBindTexture(GL_TEXTURE_2D, obj->tex);
	LBXGL_Shader_BindTexture(obj->tex);

	LBXGL_Shader_Begin(PDGL_POLYGON);

	LBXGL_Shader_TexCoord2f(0, 0);
	LBXGL_Shader_Vertex2f(ox, oy);

	LBXGL_Shader_TexCoord2f(1, 0);
	LBXGL_Shader_Vertex2f(ox+xs, oy);

	LBXGL_Shader_TexCoord2f(1, 1);
	LBXGL_Shader_Vertex2f(ox+xs, oy+ys);

	LBXGL_Shader_TexCoord2f(0, 1);
	LBXGL_Shader_Vertex2f(ox, oy+ys);

	LBXGL_Shader_End();
}

LBXGL_API void LBXGL_BrushDraw2D_DrawImage2(
	BTGE_DrawableObject2D *obj)
{
	vec2 morg;
	float xx, xy, yx, yy, ang;
	float ox, oy, xs, ys, cx, cy;
	
	ox=v2x(obj->org);
	oy=v2y(obj->org);
	xs=v2x(obj->size);
	ys=v2y(obj->size);
	cx=v2x(obj->coffs);
	cy=v2y(obj->coffs);
	ang=obj->angle*(M_PI/180.0);
//	xs2=xs/2;
//	ys2=ys/2;

	xx= cos(ang);	xy=sin(ang);
	yx=-sin(ang);	yy=cos(ang);

	if(obj->sound_chan>0)
		LBXGL_Shader_SetCurrentMixerChannel(obj->sound_chan);

	LBXGL_Shader_Color4f(1, 1, 1, 1);
	
	if(obj->seq)
	{
		if(obj->entid && (obj->entid==obj->seq->sel_entid))
		{
			LBXGL_Shader_Color4f(0.75, 1, 0.75, 1);
		}else
		{
			morg=LBXGL_Draw2DSeq_HandleInput_GetMouseRelPos(obj->seq);
			if(LBXGL_BrushDraw2D_CheckPointObject(obj, morg))
			{
				LBXGL_Shader_Color4f(1.0, 0.75, 0.75, 1);
			}
		}
	}
	
//	glEnable(GL_TEXTURE_2D);
//	glBindTexture(GL_TEXTURE_2D, obj->tex);
	LBXGL_Shader_BindTexture(obj->tex);

	LBXGL_Shader_Begin(PDGL_POLYGON);
	LBXGL_Shader_TexCoord2f(0, 0);
	LBXGL_Shader_Vertex2f(ox+( 0-cx)*xx+( 0-cy)*yx, oy+( 0-cx)*xy+( 0-cy)*yy);
	LBXGL_Shader_TexCoord2f(1, 0);
	LBXGL_Shader_Vertex2f(ox+(xs-cx)*xx+( 0-cy)*yx, oy+(xs-cx)*xy+( 0-cy)*yy);
	LBXGL_Shader_TexCoord2f(1, 1);
	LBXGL_Shader_Vertex2f(ox+(xs-cx)*xx+(ys-cy)*yx, oy+(xs-cx)*xy+(ys-cy)*yy);
	LBXGL_Shader_TexCoord2f(0, 1);
	LBXGL_Shader_Vertex2f(ox+( 0-cx)*xx+(ys-cy)*yx, oy+( 0-cx)*xy+(ys-cy)*yy);
	LBXGL_Shader_End();
}

LBXGL_API void LBXGL_BrushDraw2D_UpdateDefault(
	BTGE_DrawableObject2D *obj, float dt)
{
	obj->org=v2addscale(obj->org, obj->vel, dt);
	obj->angle=obj->angle+(obj->angvel*dt);
}

LBXGL_API void LBXGL_BrushDraw2D_UpdateDefaultFall(
	BTGE_DrawableObject2D *obj, float dt)
{
	obj->org=v2addscale(obj->org, obj->vel, dt);
	obj->vel=v2addscale(obj->vel, vec2(0, -100), dt);
	obj->angle=obj->angle+(obj->angvel*dt);
}

LBXGL_API void LBXGL_BrushDraw2D_UpdateDefaultFallTemp(
	BTGE_DrawableObject2D *obj, float dt)
{
	obj->org=v2addscale(obj->org, obj->vel, dt);
	obj->vel=v2addscale(obj->vel, vec2(0, -100), dt);
	obj->angle=obj->angle+(obj->angvel*dt);
	
	obj->accDt+=dt;
	if(obj->accDt>=10)
		{ LBXGL_BrushDraw2D_DestroyObj(obj); }
}

LBXGL_API vec2 LBXGL_BrushDraw2D_UpdateJitterVec2(vec2 v, float r)
{
	vec2 w;
	
	w=vec2(v2y(v), -v2x(v));
	v=v2addscale(v, w, r*bgbrng_sfrand());
	return(v);
}

LBXGL_API void LBXGL_BrushDraw2D_UpdateEmitter(
	BTGE_DrawableObject2D *obj, float dt)
{
	BTGE_DrawableObject2D *tmp;
	int i, n;

	obj->org=v2addscale(obj->org, obj->vel, dt);
	obj->angle=obj->angle+(obj->angvel*dt);
	
	obj->accDt+=dt;
	n=obj->rate*obj->accDt*(1.0+(0.33*bgbrng_sfrand()))+0.5;
	if(n>0) { obj->accDt=0; }
	
	for(i=0; i<n; i++)
	{
//		LBXGL_BrushDraw2D_CreateDisplayImage(obj->world, v2x(obj->org))
		tmp=LBXGL_BrushDraw2D_CreateObject(obj->world);
		tmp->org=obj->org;
		tmp->size=obj->size;
		tmp->vel=LBXGL_BrushDraw2D_UpdateJitterVec2(obj->dir, 0.25);
		tmp->imgname=obj->imgname;
		tmp->tex=obj->tex;
		tmp->ord=obj->ord;

		tmp->Draw=LBXGL_BrushDraw2D_DrawImage;
		tmp->Update=LBXGL_BrushDraw2D_UpdateDefaultFallTemp;
	}
}

LBXGL_API BTGE_DrawableObject2D *LBXGL_BrushDraw2D_CreateObjectSize(
	LBXGL_BrushWorld *world, float ox, float oy, float xs, float ys)
{
	BTGE_DrawableObject2D *tmp;
	
	tmp=LBXGL_BrushDraw2D_CreateObject(world);
//	tmp->ox=ox;	tmp->oy=oy;
//	tmp->xs=xs;	tmp->ys=ys;
	tmp->org=vec2(ox, oy);
	tmp->size=vec2(xs, ys);
	tmp->coffs=vec2(0, 0);
	tmp->angle=0;
	tmp->angvel=0;

	return(tmp);
}

LBXGL_API BTGE_DrawableObject2D *LBXGL_BrushDraw2D_CreateObjectSizeObject(
	LBXGL_BrushWorld *world, float ox, float oy, float xs, float ys,
	dyt obj)
{
	BTGE_DrawableObject2D *tmp;
	
	tmp=LBXGL_BrushDraw2D_CreateObject(world);
//	tmp->ox=ox;	tmp->oy=oy;
//	tmp->xs=xs;	tmp->ys=ys;
	tmp->org=vec2(ox, oy);
	tmp->size=vec2(xs, ys);
	tmp->coffs=vec2(0, 0);
	tmp->angle=0;
	tmp->angvel=0;
	tmp->dy_this=obj;

	return(tmp);
}

LBXGL_API BTGE_DrawableObject2D *LBXGL_BrushDraw2D_CreateDisplayImage(
	LBXGL_BrushWorld *world, char *name,
	float ox, float oy, float xs, float ys)
{
	BTGE_DrawableObject2D *tmp;
	
	tmp=LBXGL_BrushDraw2D_CreateObject(world);
//	tmp->ox=ox;	tmp->oy=oy;
//	tmp->xs=xs;	tmp->ys=ys;
	tmp->org=vec2(ox, oy);
	tmp->size=vec2(xs, ys);
//	tmp->ord=obj->ord;
	tmp->coffs=vec2(0, 0);
	tmp->angle=0;
	tmp->angvel=0;
	tmp->dy_this=NULL;

	tmp->imgname=dystrdup(name);
	tmp->tex=LBXGL_Texture_LoadImage(name);
	
	tmp->Draw=LBXGL_BrushDraw2D_DrawImage2;
	tmp->Update=LBXGL_BrushDraw2D_UpdateDefault;
	
	return(tmp);
}

LBXGL_API BTGE_DrawableObject2D *LBXGL_BrushDraw2D_CreateDisplayImage2(
	char *name, float ox, float oy, float xs, float ys)
{
	return(LBXGL_BrushDraw2D_CreateDisplayImage(
		LBXGL_BrushDraw2D_GetBoundWorld(),
		name, ox, oy, xs, ys));
}

LBXGL_API BTGE_DrawableObject2D *LBXGL_BrushDraw2D_CreateSpriteEmitter(
	LBXGL_BrushWorld *world, char *name,
	float ox, float oy, float xs, float ys,
	float vx, float vy, float rate)
{
	BTGE_DrawableObject2D *tmp;
	
	tmp=LBXGL_BrushDraw2D_CreateObject(world);
//	tmp->ox=ox;	tmp->oy=oy;
//	tmp->xs=xs;	tmp->ys=ys;
	tmp->org=vec2(ox, oy);
	tmp->size=vec2(xs, ys);
	tmp->dir=vec2(vx, vy);
	tmp->rate=rate;
	tmp->coffs=vec2(0, 0);
	tmp->angle=0;
	tmp->angvel=0;
	tmp->dy_this=NULL;

	tmp->imgname=dystrdup(name);
	tmp->tex=LBXGL_Texture_LoadImage(name);
	
//	tmp->Draw=LBXGL_BrushDraw2D_DrawImage;
	tmp->Update=LBXGL_BrushDraw2D_UpdateEmitter;
	
	return(tmp);
}

LBXGL_API BTGE_DrawableObject2D *LBXGL_BrushDraw2D_CreateSpriteEmitter2(
	char *name, float ox, float oy, float xs, float ys,
	float vx, float vy, float rate)
{
	return(LBXGL_BrushDraw2D_CreateSpriteEmitter(
		LBXGL_BrushDraw2D_GetBoundWorld(),
		name, ox, oy, xs, ys, vx, vy, rate));
}

LBXGL_API void LBXGL_BrushDraw2D_SetObjectImage(
	BTGE_DrawableObject2D *obj, char *name)
		{ obj->tex=LBXGL_Texture_LoadImage(name); }
LBXGL_API void LBXGL_BrushDraw2D_SetObjectOrdinal(
	BTGE_DrawableObject2D *obj, float ord)
		{ obj->ord=ord; }

LBXGL_API void LBXGL_BrushDraw2D_SetObjectAngle(
	BTGE_DrawableObject2D *obj, float ang)
		{ obj->angle=ang; }
LBXGL_API void LBXGL_BrushDraw2D_SetObjectAngularVelocity(
	BTGE_DrawableObject2D *obj, float avel)
		{ obj->angvel=avel; }

LBXGL_API void LBXGL_BrushDraw2D_SetObjectOrigin(
	BTGE_DrawableObject2D *obj, vec2 org)
		{ obj->org=org; }

LBXGL_API void LBXGL_BrushDraw2D_SetObjectVelocity(
	BTGE_DrawableObject2D *obj, vec2 vel)
		{ obj->vel=vel; }

LBXGL_API void LBXGL_BrushDraw2D_SetObjectCenter(
	BTGE_DrawableObject2D *obj, vec2 offs)
		{ obj->coffs=offs; }


LBXGL_API int LBXGL_BrushDraw2D_CheckPointObject(
	BTGE_DrawableObject2D *obj, vec2 pt)
{
	float xx, xy, yx, yy, ang;
	float ox, oy, xs, ys, cx, cy, sc;
	float mx, my, px, py;
	
	ox=v2x(obj->org);
	oy=v2y(obj->org);
	xs=v2x(obj->size);
	ys=v2y(obj->size);
	cx=v2x(obj->coffs);
	cy=v2y(obj->coffs);
	ang=obj->angle*(M_PI/180.0);
	sc=obj->scale;
	if(fabs(sc)<0.0001)sc=1.0;
//	xs2=xs/2;
//	ys2=ys/2;

	xx= cos(ang);	xy=sin(ang);
	yx=-sin(ang);	yy=cos(ang);

	px=v2x(pt);
	py=v2y(pt);
	mx=(px*xx+py*yx)/sc;
	my=(px*xy+py*yy)/sc;
	
	if(mx<(ox-cx))return(0);
	if(my<(oy-cy))return(0);
	if(mx>(ox+xs-cx))return(0);
	if(my>(oy+ys-cy))return(0);
	return(1);

#if 0
	LBXGL_Shader_Color4f(1, 1, 1, 1);
//	glEnable(GL_TEXTURE_2D);
//	glBindTexture(GL_TEXTURE_2D, obj->tex);
	LBXGL_Shader_BindTexture(obj->tex);

	LBXGL_Shader_Begin(PDGL_POLYGON);
	LBXGL_Shader_TexCoord2f(0, 0);
	LBXGL_Shader_Vertex2f(ox+( 0-cx)*xx+( 0-cy)*yx, oy+( 0-cx)*xy+( 0-cy)*yy);
	LBXGL_Shader_TexCoord2f(1, 0);
	LBXGL_Shader_Vertex2f(ox+(xs-cx)*xx+( 0-cy)*yx, oy+(xs-cx)*xy+( 0-cy)*yy);
	LBXGL_Shader_TexCoord2f(1, 1);
	LBXGL_Shader_Vertex2f(ox+(xs-cx)*xx+(ys-cy)*yx, oy+(xs-cx)*xy+(ys-cy)*yy);
	LBXGL_Shader_TexCoord2f(0, 1);
	LBXGL_Shader_Vertex2f(ox+( 0-cx)*xx+(ys-cy)*yx, oy+( 0-cx)*xy+(ys-cy)*yy);
	LBXGL_Shader_End();
#endif
}

LBXGL_API vec2 LBXGL_BrushDraw2D_CalcObjectCenter(
	BTGE_DrawableObject2D *obj)
{
	vec2 mid, corg;
	mid=v2addscale(obj->org, obj->size, 0.5);
	corg=v2sub(mid, obj->coffs);
	return(corg);
}
