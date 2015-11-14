#include <btgesv.h>

void BT_MonsterUseThink(btEntity self)
{
	BT_RespondToDamage(self, self->oldEnemy, 0);
}

/** monster was used by someone, decide how to respond */
void BT_MonsterUse(btEntity self, btEntity other)
{
	if(self->enemy)return;
	if(self->health<=0)return;
	
	if(self->dlg_info)
		BT_EntityDialogUse(self, other);

	if(other->flags&BT_FL_NOTARGET)return;
	
	self->oldEnemy=other;
	self->f_think=BT_MonsterUseThink;
	self->nextthink=btDelayTime(0.1);
}

void BT_MonsterDeathUse(btEntity self, btEntity other)
{
	self->flags&=~(BT_FL_FLY|BT_FL_SWIM);
	if(!self->target)return;
	BT_UseTargets(self, other);
}

void BT_MonsterStartGo(btEntity self)
{
	btEntity cur, lst;
//	if(!self->eAct)self->eAct=gcalloc(sizeof(BTGE_EntityActor));

	if(!btWalkmove(self, 0, 0))
		self->origin=v3add(self->origin, vec3(0, 0, 8));
	if(!btWalkmove(self, 0, 0))
	{
		//started in solid
		btDPrint("monster %s start solid at (%s)\n",
			self->classname,
			btVec3ToStr(self->origin));
	}

	if(self->health==0)
		self->health=25;
	if(self->maxHealth==0)
		self->maxHealth=self->health;
		
	if(!self->ttsVoice)
		self->ttsVoice="female0";
	
	self->takedamage=BT_DAMAGE_AIM;
	self->idealYaw=btCurrentYaw(self);
	if(self->yawSpeed==0)
		self->yawSpeed=360;
	self->vieworg=vec3(0,0,25);
	self->f_use=BT_MonsterUse;
	self->flags|=BT_FL_MONSTER;
	
	if(self->target)
	{
		lst=btFindTargets(self->target);

		if(!lst)
		{
			btDPrint("monster %s missing target at (%s)\n",
				self->classname,
				btVec3ToStr(self->origin));
		}

		cur=lst;
		while(cur)
		{
			if(!strcmp(cur->classname, "path_corner"))
				self->goalEntity=cur;
			if(!strcmp(cur->classname, "path_corner_event"))
				self->goalEntity=cur;
			cur=cur->chain;
		}
	}
	
	if(self->goalEntity)
	{
		self->moveTarget=self->goalEntity;
		BTAI_ScriptedSetNode(self, self->goalEntity);

		if(self->anim)
			return;

#if 0
		self->moveTarget=self->goalEntity;
		
		if(self->goalEntity==self)
		{
			self->goalEntity=NULL;
			self->moveTarget=self->goalEntity;
		}
#endif
	}
	
	if(self->moveTarget)
	{	
		btEntWalk(self);
	}else
	{
		self->pauseTime=btDelayTime(999999);
		btEntStand(self);
	}
	
	BT_EntityDialogSetupTest(self);
}

void BT_WalkmonsterStartGo(btEntity self)
{
	self->origin=v3add(self->origin, vec3(0, 0, 1));
	btDropToFloor(self);
	BT_MonsterStartGo(self);
}

void BT_FlymonsterStartGo(btEntity self)
{
	self->flags|=BT_FL_FLY;
	BT_MonsterStartGo(self);
}

void BT_SwimmonsterStartGo(btEntity self)
{
	self->flags|=BT_FL_SWIM;
	BT_MonsterStartGo(self);
}

void BT_MonsterNoSpawn(btEntity self)
	{ btDeleteEntity(self); }

/**
 * Used by spawn functions to indicate that this is a walking monster
 * The monster will init after a short delay.
 */
BTSV_API void btWalkmonsterStart(btEntity self)
{
	if(btGameIsDeathmatch() || btGameIsNoMonsters())
	{
		btSetThink(self, BT_MonsterNoSpawn, btRandom()*0.5);
		return;
	}

	btSetThink(self, BT_WalkmonsterStartGo, btRandom()*0.5);
}

/**
 * Used by spawn functions to indicate that this is a flying monster
 * The monster will init after a short delay.
 */
BTSV_API void btFlymonsterStart(btEntity self)
{
	if(btGameIsDeathmatch() || btGameIsNoMonsters())
	{
		btSetThink(self, BT_MonsterNoSpawn, btRandom()*0.5);
		return;
	}

	btSetThink(self, BT_FlymonsterStartGo, btRandom()*0.5);
}

/**
 * Used by spawn functions to indicate that this is a swimming monster
 * The monster will init after a short delay.
 */
BTSV_API void btSwimmonsterStart(btEntity self)
{
	if(btGameIsDeathmatch() || btGameIsNoMonsters())
	{
		btSetThink(self, BT_MonsterNoSpawn, btRandom()*0.5);
		return;
	}

	btSetThink(self, BT_SwimmonsterStartGo, btRandom()*0.5);
}

/**
  * Determine if a client is within a certain distance.
  */
bool BT_CheckClientNearby(btEntity self, float dist)
{
	btEntity cl;
	float d;

//	cl=btGetCurrentClient();
//	if(!cl)return(false);
	
//	d=BT_TargetDistance(self, cl);
//	if(d<dist)return(true);

	cl=btFindClients();
	while(cl)
	{
		d=BT_TargetDistance(self, cl);
		if(d<dist)return(true);
		cl=cl->chain;
	}

	return(false);

//	if(d>dist)return(false);
//	return(true);
}

void BT_MonsterMoveThink(btEntity self)
{
	BTGE_MoveInfo *move;
	BTGE_MoveFrame *frm;
	int num;
	
	move=self->move;
	if(!move)return;
	
	if(!BT_CheckClientNearby(self, 1024))
	{
		//no client is nearby, so suspend...
		btSetThink(self, BT_MonsterMoveThink, 0.5);
		return;
	}
	
	//check if we have reached the end
	if(self->frame>=move->endFrame)
	{
		//and call final function if so
		if(move->f_end)
			move->f_end(self);
		return;
	}

	//set up think to call back here by default
	btSetThink(self, BT_MonsterMoveThink2, 0.1);
	
	//do stuff for frame
	num=self->frame-move->startFrame;
	frm=&(move->frame[num]);
	if(frm->f_ai)
		frm->f_ai(self, frm->dist);
	if(frm->f_think)
		frm->f_think(self);
}

void BT_MonsterMoveThink2(btEntity self)
{
	//advance to next frame
	self->frame++;
	BT_MonsterMoveThink(self);
}

BTSV_API void btSetMove(btEntity self, BTGE_MoveInfo *move)
{
	if(!move)
	{
		//no move means kill animation
		self->move=NULL;
		self->nextthink=-1;
		return;
	}

	//ok, set current animation in motion
	self->move=move;
	self->anim=move->anim;
	self->frame=move->startFrame;
	btSetThink(self, BT_MonsterMoveThink, 0.1);
}

void BT_MonsterMove_GetMoveFrame(btEntity self,
	BTGE_MoveInfo **rmove, BTGE_MoveFrame **rfrm)
{
	BTGE_MoveInfo *move;
	BTGE_MoveFrame *frm;
	int num;

	if(rmove)*rmove=NULL;
	if(rfrm)*rfrm=NULL;

	move=self->move;
	if(!move)return;

	num=self->frame-move->startFrame;
	frm=&(move->frame[num]);

	if(rmove)*rmove=move;
	if(rfrm)*rfrm=frm;
}

BTSV_API void BT_MonsterMove_DyAI(btEntity self, float dist)
{
	BTGE_MoveFrame_DyInfo *dyi;
	BTGE_MoveInfo *move;
	BTGE_MoveFrame *frm;
	int num;
	
	BT_MonsterMove_GetMoveFrame(self, &move, &frm);
	if(!move || !frm)return;
	dyi=frm->data;

	if(!dyi)return;

	dyApplyMethod2(self->dy_this, dyi->dyf_ai,
		(dyt)self, dyfloat(dist));
}

BTSV_API void BT_MonsterMove_DyThink(btEntity self)
{
	BTGE_MoveFrame_DyInfo *dyi;
	BTGE_MoveInfo *move;
	BTGE_MoveFrame *frm;
	int num;
	
	BT_MonsterMove_GetMoveFrame(self, &move, &frm);
	if(!move || !frm)return;
	dyi=frm->data;

	if(!dyi)return;

	dyApplyMethod1(self->dy_this, dyi->dyf_think, (dyt)self);
}

BTSV_API void BT_MonsterMove_DyEnd(btEntity self)
{
	BTGE_MoveFrame_DyInfo *dyi;
	BTGE_MoveInfo *move;

	move=self->move;
	if(!move)return;
	dyi=move->data;

	if(!dyi)return;

	dyApplyMethod1(self->dy_this, dyi->dyf_end, (dyt)self);
}

void BT_MonsterMove_SetupFrameDyAI(BTGE_MoveFrame *frm, dyt v)
{
	BTGE_MoveFrame_DyInfo *dyi;
	void *p;
	char *s;

	if(!v || (v==UNDEFINED))
		return;

	s=NULL;
	if(dysymbolp(v) || dykeywordp(v))
		s=dysymbolv(v);
	if(dystringp(v))
		s=dystringv(v);

	if(s)
	{
		p=dyllGetAddr(s);
		if(p)
		{
			frm->f_ai=p;
			return;
		}
	}

	if(dyRawFuncBasicP(v))
	{
		p=dyRawFuncGetFuncPtr(v);
		if(p) { frm->f_ai=p; return; }
	}
	
	dyi=(BTGE_MoveFrame_DyInfo *)(frm->data);
	if(!dyi)
	{
		dyi=gctalloc("btge_moveframe_dyinfo_t",
			sizeof(BTGE_MoveFrame_DyInfo));
		frm->data=dyi;
	}
	
	frm->f_ai=BT_MonsterMove_DyAI;
	dyi->dyf_ai=v;
}

void BT_MonsterMove_SetupFrameDyThink(BTGE_MoveFrame *frm, dyt v)
{
	BTGE_MoveFrame_DyInfo *dyi;
	void *p;
	char *s;

	if(!v || (v==UNDEFINED))
		return;

	s=NULL;
	if(dysymbolp(v) || dykeywordp(v))
		s=dysymbolv(v);
	if(dystringp(v))
		s=dystringv(v);

	if(s)
	{
		p=dyllGetAddr(s);
		if(p) { frm->f_think=p; return; }
	}

	if(dyRawFuncBasicP(v))
	{
		p=dyRawFuncGetFuncPtr(v);
		if(p) { frm->f_think=p; return; }
	}
		
	dyi=(BTGE_MoveFrame_DyInfo *)(frm->data);
	if(!dyi)
	{
		dyi=gctalloc("btge_moveframe_dyinfo_t",
			sizeof(BTGE_MoveFrame_DyInfo));
		frm->data=dyi;
	}
	
	frm->f_think=BT_MonsterMove_DyThink;
	dyi->dyf_think=v;
}

void BT_MonsterMove_SetupMoveInfoDyEnd(BTGE_MoveInfo *move, dyt v)
{
	BTGE_MoveFrame_DyInfo *dyi;
	void *p;
	char *s;

	if(!v || (v==UNDEFINED))
		return;

	s=NULL;
	if(dysymbolp(v) || dykeywordp(v))
		s=dysymbolv(v);
	if(dystringp(v))
		s=dystringv(v);

	if(s)
	{
		p=dyllGetAddr(s);
		if(p) { move->f_end=p; return; }
	}
	
	if(dyRawFuncBasicP(v))
	{
		p=dyRawFuncGetFuncPtr(v);
		if(p) { move->f_end=p; return; }
	}
	
	dyi=(BTGE_MoveFrame_DyInfo *)(move->data);
	if(!dyi)
	{
		dyi=gctalloc("btge_moveframe_dyinfo_t",
			sizeof(BTGE_MoveFrame_DyInfo));
		move->data=dyi;
	}
	
	move->f_end=BT_MonsterMove_DyEnd;
	dyi->dyf_end=v;
}

BTSV_API BTGE_MoveInfo *btDyBuildMove(char *anim,
	int startFrame, int endFrame, dyt frames, dyt f_end)
{
	BTGE_MoveInfo *tmp;
	BTGE_MoveFrame *frm;
	dyt t, v;
	int i, n;
	
	tmp=gctalloc("btge_moveinfo_t", sizeof(BTGE_MoveInfo));
	tmp->anim=dystrdup(anim);
	tmp->startFrame=startFrame;
	tmp->endFrame=endFrame;
	
	n=(endFrame-startFrame)+1;
	tmp->frame=gcalloc(n*sizeof(BTGE_MoveFrame));
	
	BT_MonsterMove_SetupMoveInfoDyEnd(tmp, f_end);

	if(dyarrayp(frames))
	{
		for(i=0; i<n; i++)
		{
			frm=&(tmp->frame[i]);
			t=dyarrayidx(frames, i);
			if(!t || (t==UNDEFINED))
				continue;
				
			if(dyarrayp(t))
			{
				v=dyarrayidx(t, 0);
				BT_MonsterMove_SetupFrameDyAI(frm, v);
				v=dyarrayidx(t, 1);
				frm->dist=dyfloatv(v);
				v=dyarrayidx(t, 2);
				BT_MonsterMove_SetupFrameDyThink(frm, v);
			}
		}
	}
	
	return(tmp);
}
