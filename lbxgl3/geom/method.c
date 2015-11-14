#include <lbxgl2.h>

LBXGL_RenderMethods *lbxgl_method_root=NULL;
LBXGL_RenderMethods *lbxgl_method_hash[4096];

LBXGL_API LBXGL_RenderMethods *LBXGL_Method_LookupRenderMethods(char *type)
{
	LBXGL_RenderMethods *cur;

	cur=lbxgl_method_root;
	while(cur)
	{
		if(!strcmp(cur->type, type))
			return(cur);
		cur=cur->next;
	}
	return(NULL);
}

LBXGL_API LBXGL_RenderMethods *LBXGL_Method_GetRenderMethods(char *type)
{
	LBXGL_RenderMethods *tmp;

	tmp=LBXGL_Method_LookupRenderMethods(type);
	if(tmp)return(tmp);

	tmp=gctalloc("lbxgl_rendermethods_t", sizeof(LBXGL_RenderMethods));
	tmp->type=dystrdup(type);
	tmp->next=lbxgl_method_root;
	lbxgl_method_root=tmp;

	return(tmp);
}

LBXGL_API LBXGL_RenderMethods *LBXGL_Method_GetRenderMethodsObj(dyt obj)
{
	LBXGL_RenderMethods *tmp;
	char *ty, *s;
	int i;

	ty=dygettype(obj);
//	if(!ty)return(NULL);
	if(!ty)ty="";

	s=ty; i=0;
	while(*s)i=i*251+(*s++);
	i=(i>>8)&0xFFF;

	tmp=lbxgl_method_hash[i];
	if(tmp && !strcmp(tmp->type, ty))return(tmp);

	tmp=LBXGL_Method_GetRenderMethods(ty);
	lbxgl_method_hash[i]=tmp;
	return(tmp);
}


LBXGL_API int LBXGL_GenericRenderDark(dyt obj)
{
	LBXGL_RenderMethods *ctx;
	ctx=LBXGL_Method_GetRenderMethodsObj(obj);
	if(ctx->render_dark)
		return(ctx->render_dark(obj));
	return(-1);
}

LBXGL_API int LBXGL_GenericRenderLight(dyt obj)
{
	LBXGL_RenderMethods *ctx;
	ctx=LBXGL_Method_GetRenderMethodsObj(obj);
	if(ctx->render_light)
		return(ctx->render_light(obj));
	return(-1);
}

LBXGL_API int LBXGL_GenericRenderShadow(dyt obj, float *org)
{
	LBXGL_RenderMethods *ctx;
	ctx=LBXGL_Method_GetRenderMethodsObj(obj);
	if(ctx->render_shadow)
		return(ctx->render_shadow(obj, org));
	return(-1);
}

LBXGL_API int LBXGL_GenericRenderFinal(dyt obj)
{
	LBXGL_RenderMethods *ctx;
	ctx=LBXGL_Method_GetRenderMethodsObj(obj);
	if(ctx->render_final)
		return(ctx->render_final(obj));
	return(-1);
}

LBXGL_API int LBXGL_GenericRenderDraw(dyt obj)
{
	LBXGL_RenderMethods *ctx;
	ctx=LBXGL_Method_GetRenderMethodsObj(obj);
	if(ctx->render_draw)
		return(ctx->render_draw(obj));
	return(-1);
}

LBXGL_API int LBXGL_GenericGetSphere(dyt obj, float *org, float *rad)
{
	LBXGL_RenderMethods *ctx;
	ctx=LBXGL_Method_GetRenderMethodsObj(obj);
	if(ctx->get_sphere)
		return(ctx->get_sphere(obj, org, rad));
	return(-1);
}
