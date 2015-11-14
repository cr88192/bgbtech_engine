#include <lbxgl2.h>

void *trigger_relay_use(lbxEntity *self)
{
	float f;

	f=lbxEntGetFloat(self, "thinktime");
	if(f>=0)return(NULL);

	f=lbxEntGetFloat(self, "delay");
	if(f<0)f=5.0;
	lbxEntSetFloat(self, "thinktime", f);

	lbxEntUseTarget(self);
	return(NULL);
}

LBXGL_API int trigger_relay(lbxEntity *self)
{
//	lbxEntSetCallback0(self, "use", trigger_relay_use);
	return(0);
}


void *trigger_once_use(lbxEntity *self)
{
	lbxEntUseTarget(self);
	lbxEntRemove(self);
	return(NULL);
}

void *trigger_once_touch(lbxEntity *self, lbxEntity *other)
{
	lbxEntUseTarget(self);
	lbxEntRemove(self);
	return(NULL);
}

LBXGL_API int trigger_once(lbxEntity *self)
{
//	lbxEntSetCallback0(self, "use", trigger_once_use);
//	lbxEntSetCallback0(self, "touch", trigger_once_touch);
	lbxEntSetSolid(self, "solid_trigger");
	lbxEntSetMove(self, "move_none");
	return(0);
}


void *trigger_multiple_use(lbxEntity *self)
{
	float f;

	f=lbxEntGetFloat(self, "thinktime");
	if(f>=0)return(NULL);

	f=lbxEntGetFloat(self, "delay");
	if(f<0)f=5.0;
	lbxEntSetFloat(self, "thinktime", f);

	lbxEntSetSolid(self, "solid_not");
	lbxEntUseTarget(self);

	return(NULL);
}

void *trigger_multiple_touch(lbxEntity *self, lbxEntity *other)
{
	trigger_multiple_use(self);
	return(NULL);
}

void *trigger_multiple_think(lbxEntity *self)
{
	lbxEntSetSolid(self, "solid_trigger");
	return(NULL);
}

LBXGL_API int trigger_multiple(lbxEntity *self)
{
//	lbxEntSetCallback0(self, "use", trigger_multiple_use);
//	lbxEntSetCallback0(self, "touch", trigger_multiple_touch);
//	lbxEntSetCallback0(self, "think", trigger_multiple_think);

	lbxEntSetFloat(self, "thinktime", -1);
	lbxEntSetSolid(self, "solid_trigger");
	lbxEntSetMove(self, "move_none");
	return(0);
}

LBXGL_API int light(lbxEntity *self)
{
	return(0);
}

LBXGL_API int path_corner(lbxEntity *self)
{
	return(0);
}
