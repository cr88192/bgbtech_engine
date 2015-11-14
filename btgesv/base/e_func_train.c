#include <btgesv.h>

void func_train_init(btEntity self)
{
	float ang[4], tv[4];
	float f, g, h;

	if(self->damage==0)
		self->damage=10;
	if(self->speed==0)
		self->speed=100;

	printf("func_train_init: spawnflags=%d\n", self->spawnflags);

	self->solidtype=BT_SOLID_BSP;
	self->movetype=BT_MOVE_PUSH;
	self->angles=vec3(0,0,0);

	//allow everything to spawn
	btSetThink(self, func_train_goStart, 0.1);
}

void func_train_goStart(btEntity self)
{
	btEntity tgt;

	tgt=btFindTargets(self->target);
	if(!tgt)
	{
		printf("func_train_init: no target %s\n",
			self->target);
		return;
	}

	self->enemy=tgt;
	self->origin=v3dsub(tgt->origin, v3f2d(self->mins));
	
	if(!self->targetname ||
			(self->spawnflags&BT_SPAWNFLAG_TRAIN_START_ON))
		func_train_goNext(self);
}

void func_train_goNext(btEntity self)
{
	btEntity tgt;

//	printf("func_train_goNext\n");

	if(!self->enemy)
	{
		printf("func_train_goNext: no enemy\n");
		return;
	}
	tgt=btFindTargets(self->enemy->target);
	if(!tgt)
	{
		printf("func_train_goNext: no next enemy %s\n",
			self->enemy->target);
		return;
	}
	BT_CalcMove(self, v3dsub(tgt->origin, v3f2d(self->mins)), self->speed,
		func_train_movethink);
	self->enemy=tgt;
}

void func_train_goCurrent(btEntity self)
{
//	printf("func_train_goCurrent\n");

	if(!self->enemy)
	{
		printf("func_train_goCurrent: no enemy\n");
		return;
	}
	BT_CalcMove(self,
		v3dsub(self->enemy->origin, v3f2d(self->mins)),
		self->speed,
		func_train_movethink);
}

void func_train_movethink(btEntity self)
{
//	printf("func_train_movethink\n");
//	if(self->wait>0)
	if(self->enemy->wait>0)
	{
//		btSetThink(self, func_train_waitthink, self->wait);
		btSetThink(self, func_train_waitthink, self->enemy->wait);
		return;
	}

	if(self->wait>0)
	{
//		btSetThink(self, func_train_waitthink, self->wait);
		btSetThink(self, func_train_waitthink, self->wait);
		return;
	}

	if(self->enemy->wait<0)
		return;

	func_train_goNext(self);
}

void func_train_waitthink(btEntity self)
{
//	printf("func_train_waitthink\n");
	func_train_goNext(self);
}

void func_train_use(btEntity self, btEntity other)
{
	vec3 t;

//	printf("func_train_use\n");

	if(self->spawnflags&BT_SPAWNFLAG_TRAIN_START_ON)
	{
		if(!(self->spawnflags&BT_SPAWNFLAG_TRAIN_TOGGLE))
			return;
		//turn train off/on...
		return;
	}
//	func_train_goCurrent(self);
	func_train_goNext(self);
}

void func_train_blocked(btEntity self, btEntity other)
{
	BT_Damage(other, self, self->damage, BT_MOD_CRUSH);
//	func_door_use(self, other);
}


BTGE_Entity_Iface func_train_vt =
{
func_train_init,		func_deinit,
func_null,				func_null,
func_train_blocked,		func_touch,
func_train_use,			func_pain,
func_die,				func_cmdmsg,
};

BTSV_API btEntity BtSp_func_train(btSEntity args)
{
	btEntity self;
	
	self=btNewEntityTriggerInit(args, &func_train_vt);
	return(self);
}
