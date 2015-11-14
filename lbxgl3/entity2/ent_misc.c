#include <lbxgl2.h>

void *trigger_relay_use(LBXGL_Entity *self)
{
	float f;

	f=EntGetFloat(self, "thinktime");
	if(f>=0)return(NULL);

	f=EntGetFloat(self, "delay");
	if(f<0)f=5.0;
	EntSetFloat(self, "thinktime", f);

	EntUseTarget(self);
	return(NULL);
}

int trigger_relay(Entity *self)
{
	EntSetCallback0(self, "use", trigger_relay_use);
	return(0);
}


void *trigger_once_use(LBXGL_Entity *self)
{
	EntUseTarget(self);
	EntRemove(self);
	return(NULL);
}

void *trigger_once_touch(Entity *self, Entity *other)
{
	EntUseTarget(self);
	EntRemove(self);
	return(NULL);
}

int trigger_once(Entity *self)
{
	EntSetCallback0(self, "use", trigger_once_use);
	EntSetCallback0(self, "touch", trigger_once_touch);
	EntSetSolid(self, "solid_trigger");
	EntSetMove(self, "move_none");
	return(0);
}


void *trigger_multiple_use(LBXGL_Entity *self)
{
	float f;

	f=EntGetFloat(self, "thinktime");
	if(f>=0)return(NULL);

	f=EntGetFloat(self, "delay");
	if(f<0)f=5.0;
	EntSetFloat(self, "thinktime", f);

	EntSetSolid(self, "solid_not");
	EntUseTarget(self);

	return(NULL);
}

void *trigger_multiple_touch(Entity *self, Entity *other)
{
	trigger_multiple_use(self);
	return(NULL);
}

void *trigger_multiple_think(LBXGL_Entity *self)
{
	EntSetSolid(self, "solid_trigger");
	return(NULL);
}

int trigger_multiple(Entity *self)
{
	EntSetCallback0(self, "use", trigger_multiple_use);
	EntSetCallback0(self, "touch", trigger_multiple_touch);
	EntSetCallback0(self, "think", trigger_multiple_think);

	EntSetFloat(self, "thinktime", -1);
	EntSetSolid(self, "solid_trigger");
	EntSetMove(self, "move_none");
	return(0);
}

