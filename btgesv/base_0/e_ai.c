#include <btgesv.h>

btEntity btge_sightEntity;
float btge_sightEntityTime;

BTSV_API void BT_SightEntity(btEntity self, float time)
{
	btge_sightEntity=self;
	btge_sightEntityTime=btDelayTime(time);
}

BTSV_API int BT_ChangeYaw(btEntity self)
{
	float ny, dy, dy1, sg;

#if 1
	if(self->goalEntity != self->enemy)
	{
		btSetYaw(self, self->idealYaw);
		return(0);
	}
#endif

//	btSetYaw(self, self->idealYaw);
//	return(0);

//	dy=btAngleMod(self->idealYaw-btCurrentYaw(self));
	dy=self->idealYaw-btCurrentYaw(self); sg=1;
	if(dy<0) { dy=-dy; sg=-sg; }
	if(dy>180) { dy=360-dy; sg=-sg; }

	dy1=self->yawSpeed*btStepTime();
	if(dy1<dy)dy=dy1;
	ny=btCurrentYaw(self)+dy*sg;
	ny=btAngleMod(ny);
	btSetYaw(self, ny);
	return(0);
}

BTSV_API int BT_CloseEnough(btEntity self, btEntity targ, float dist)
{
	if(v3x(targ->absmin)>(v3x(self->absmax)+dist))return(false);
	if(v3y(targ->absmin)>(v3y(self->absmax)+dist))return(false);
	if(v3z(targ->absmin)>(v3z(self->absmax)+dist))return(false);
	if(v3x(targ->absmax)<(v3x(self->absmin)-dist))return(false);
	if(v3y(targ->absmax)<(v3y(self->absmin)-dist))return(false);
	if(v3z(targ->absmax)<(v3z(self->absmin)-dist))return(false);
	return(true);
}

BTSV_API void BT_NewGoalMove(btEntity self, btEntity targ, float dist)
{
	if(btRandom()<0.33)
	{
		self->idealYaw=BT_TargetYaw(self, self->goalEntity);
	}
	self->idealYaw=btRandom()*360;
}

BTSV_API void BT_MoveToGoal(btEntity self, float dist)
{
	float f, g;
	char *s;
	int i;
	
	if(self->flags&BT_FL_SCRIPTED)
	{
		if(!self->goalEntity)
		{
//			*(int *)-1=-1;
			printf("BT_MoveToGoal: Scripted but no goalEntity\n");
//			self->flags&=~BT_FL_SCRIPTED;
			return;
		}

		self->idealYaw=BT_TargetYaw(self, self->goalEntity);
		if(!BT_FacingIdealP(self))
			BT_ChangeYaw(self);
		btWalkmove(self, btCurrentYaw(self), dist);
		return;
	}
	
	if(!(self->flags&BT_FL_ONGROUND) &&
		!(self->flags&(BT_FL_SWIM|BT_FL_FLY)))
	{
		self->idealYaw=BT_TargetYaw(self, self->goalEntity);
		if(!BT_FacingIdealP(self))
			BT_ChangeYaw(self);

//		printf("BT_MoveToGoal: %d Moving To Goal (B)\n", self->entnum);

//		btWalkmove(self, btCurrentYaw(self), 0);
		btWalkmove(self, btCurrentYaw(self), dist);
		return;
	}

	if(!self->goalEntity)
	{
		printf("BT_MoveToGoal: No GoalEntity\n");
	}

	if((self->goalEntity==self->enemy) && 
		BT_CloseEnough(self, self->goalEntity, dist))
	{
//		printf("BT_MoveToGoal: Enemy Close Enough\n");
		return;
	}

#if 0
	if(self->goalEntity != self->enemy)
	{
		self->idealYaw=BT_TargetYaw(self, self->goalEntity);
	}
#endif

	BT_ChangeYaw(self);

//	f=BT_TargetDistance(self, self->goalEntity);
//	printf("BT_MoveToGoal: %d Moving To Goal %s, dist=%f\n",
//		self->entnum, self->goalEntity->targetname, f);

	i=btWalkmove(self, btCurrentYaw(self), dist);
//	if(!i || (btRandom()<0.25*btRelStepTime()))
	if(!i)
	{
//		if((self->flags&BT_FL_ONGROUND) && (self->flags&BT_FL_JUMPMOVE))
		if(self->flags&BT_FL_JUMPMOVE)
		{
			printf("BT_MoveToGoal: Jump\n");

			s="sound/player/jump1";
			btSound(self, BT_CHAN_BODY, s, 1.0, BT_ATTN_NORM);

			self->flags&=~BT_FL_ONGROUND;
			self->velocity=v3add(self->velocity, vec3(0,0,200));
			return;
		}
	
		printf("BT_MoveToGoal: %d Blocked\n", self->entnum);
		self->origin=v3add(self->origin, vec3(0,0,1));

		BT_NewGoalMove(self, self->goalEntity, dist);
		self->wanderTime=btDelayTime(1.0);
	}else if(btTime()>self->wanderTime)
	{
//		printf("BT_MoveToGoal: Enemy Close Enough\n");

		//try going directly towards goal
		self->idealYaw=BT_TargetYaw(self, self->goalEntity);
	}
}

BTSV_API int BT_Range(btEntity self, btEntity targ)
{
	float d;
	
	d=v3len(v3sub(targ->origin, self->origin));
	if(d<120)return(BT_RANGE_MELEE);
	if(d<500)return(BT_RANGE_NEAR);
	if(d<1000)return(BT_RANGE_MID);
	return(BT_RANGE_FAR);
}

BTSV_API int BT_VisibleP(btEntity self, btEntity targ)
{
	BTGE_Trace tr;
	vec3 pt0, pt1;
	int i;
	
//	pt0=v3add(self->origin, self->vieworg);
//	pt1=v3add(targ->origin, targ->vieworg);
	pt0=btCalcCenter(self);
	pt1=btCalcCenter(targ);
	i=btTraceline(&tr, pt0, pt1,
		BT_TRACE_NOMONSTERS|BT_TRACE_BSPSOLID,
		self);
	
	if(i)return(false);
	if(tr.brush)return(false);
	
	if(tr.fraction<1.0)
		return(false);
	return(true);
}

BTSV_API vec3 BT_Forward(btEntity self)
{
	float f;
	
	f=btCurrentYaw(self)*(M_PI/180);
	return(vec3(cos(f), sin(f), 0));
}

BTSV_API int BT_InfrontP(btEntity self, btEntity targ)
{
	vec3 fw;
	float d;
	
	fw=BT_Forward(self);
	d=v3dot(v3norm(v3sub(btCalcCenter(targ), btCalcCenter(self))), fw);
	if(d>0.33)return(true);
	return(false);
}


BTSV_API float BT_EnemyYaw(btEntity self)
{
	float f;
	f=btVecToYaw(v3sub(btCalcCenter(self->enemy), btCalcCenter(self)));
	return(f);
}

BTSV_API float BT_TargetYaw(btEntity self, btEntity targ)
{
	float f;
	f=btVecToYaw(v3sub(btCalcCenter(targ), btCalcCenter(self)));
	return(f);
}

BTSV_API vec3 BT_TargetDirection(btEntity self, btEntity targ)
{
	vec3 v;
//	v=v3norm(v3sub(targ->origin, self->origin));
	v=v3norm(v3sub(btCalcCenter(targ), btCalcCenter(self)));
	return(v);
}

BTSV_API float BT_TargetDistance(btEntity self, btEntity targ)
{
	float f;
//	v=v3norm(v3sub(targ->origin, self->origin));
	f=v3dist(btCalcCenter(targ), btCalcCenter(self));
	return(f);
}

BTSV_API void BT_HuntTarget(btEntity self)
{
	if(!self->enemy)return;
	self->goalEntity=self->enemy;
	self->idealYaw=BT_EnemyYaw(self);
//	btSetThink(self, self->avt->run, 0.1);
	btEntRun(self);
}

BTSV_API void BT_SightSound(btEntity self)
{
	btSound(self, BT_CHAN_VOICE, self->snd_sight, 1.0, BT_ATTN_NORM);
}

BTSV_API void BT_FoundTarget(btEntity self)
{
//	btge_sightEntity=self;
//	btge_sightEntityTime=btDelayTime(0);

	BT_SightEntity(self, 0.1);

	self->showHostile=btDelayTime(1);
	BT_SightSound(self);
	BT_HuntTarget(self);
}

/** Checks if other is someone who we will not attack (passive towards) */
BTSV_API bool BT_CheckNoAttack(btEntity self, btEntity other)
{
	//can't attack nothing
	if(!other)return(true);

	//ignore non-active entities
	if(!(other->flags&(BT_FL_CLIENT|BT_FL_MONSTER)))
		return(true);

	//ignore dead stuff
	if(other->health<=0)
		return(true);

	//passive entities never attack
	if(self->flags&BT_FL_AI_PASSIVE)
		return(true);

	//ignore enemies stuck in dialogue boxes
	if(other->dlg)
		return(true);

	//ignore others on same team
	if(self->team && (self->team==other->team))
		return(true);

	//ignore others with the same team leader (or the leader themselves)
	if(self->teamLeader &&
			((self->teamLeader==other) ||
			 (self->teamLeader==other->teamLeader)))
		return(true);

	//ignore followers
	if(other->teamLeader && (other->teamLeader==self))
		return(true);

	//ignore monsters of same type (unless on different teams)
	if(!self->team && !other->team &&
		self->classname && other->classname &&
		!strcmp(self->classname, other->classname))
			return(true);

	//if we are a good-guy...
	if(self->flags&BT_FL_AI_GOODGUY)
	{
		//ignore players and other good-guys
		if(other->flags&(BT_FL_AI_GOODGUY|BT_FL_CLIENT))
			return(true);
	}

	return(false);
}

/** Checks if other is someone who we would attack (hostile towards) */
BTSV_API bool BT_CheckWouldAttack(btEntity self, btEntity other)
{
	//can't attack nothing
	if(!other)return(false);

	//ignore anyone we can't attack
	if(BT_CheckNoAttack(self, other))
		return(false);

	//passive types never attack
	if(self->flags&BT_FL_AI_PASSIVE)
		return(false);

	//and, neutral don't attack (unless provoked)
	if(self->flags&BT_FL_AI_NEUTRAL)
		return(false);

	//if we are a bad guy...
	if(!(self->flags&BT_FL_AI_GOODGUY))
	{
		//get pissed off at players and good-guys
		if(other->flags&(BT_FL_AI_GOODGUY|BT_FL_CLIENT))
			return(true);
	}

	return(false);
}

/** Checks if other is someone we would target */
BTSV_API bool BT_CheckPossibleTarget(btEntity self, btEntity other)
{
	int r;

	//ignore anyone we wouldn't attack on sight
	if(!BT_CheckWouldAttack(self, other))
		return(false);

	r=BT_Range(self, other);
	
	//too far away?
	if(r==BT_RANGE_FAR)
		return(false);

	//invisible?
	if(!BT_VisibleP(self, other))
		return(false);
	
//	printf("BT_FindTarget: Sight %p %p\n", self, other);

	if(r==BT_RANGE_NEAR)
	{
		//is sneaking up on us?
		if((other->showHostile<btTime()) &&
			!BT_InfrontP(self, other))
				return(false);
	}else if(r==BT_RANGE_MID)
	{
		//is behind us?
		if(!BT_InfrontP(self, other))
			return(false);
	}

	return(true);
}

//determine if a target is visible to self
BTSV_API btEntity BT_CheckTarget(btEntity self)
{
	btEntity cl;
	btEntity cur, nxt;
	float d, bd;

	//what about client?
	cl=btGetCurrentClient();
	
	//not hostile towards client? ignore them.
	if(!BT_CheckWouldAttack(self, cl))
		cl=NULL;

	//calc distance to target
	bd=999999;
	if(cl)bd=BT_TargetDistance(self, cl);

	if(btge_sightEntity &&
		(btTime()<=btge_sightEntityTime) &&
		BT_VisibleP(self, btge_sightEntity))
	{
		cur=btge_sightEntity;
		if(BT_CheckPossibleTarget(self, cur))
		{
			d=BT_TargetDistance(self, cur);
			if(d<bd) { cl=cur; bd=d; }
		}
		
		cur=cur->enemy;
		if(cur && BT_CheckPossibleTarget(self, cur))
		{
			d=BT_TargetDistance(self, cur);
			if(d<bd) { cl=cur; bd=d; }
		}
	}

	//see if anyone else is in range
	cur=btFindRadius(btCalcCenter(self), 512);
	while(cur)
	{
		d=BT_TargetDistance(self, cur);
		
		//if further than nearest match, ignore
		if(d>=bd)
			{ cur=cur->chain; continue; }

		if(!BT_CheckPossibleTarget(self, cur))
		{
			cur=cur->chain;
			continue;
		}

		//choose if closer
		if(d<bd)
			{ cl=cur; bd=d; }

		cur=cur->chain;
	}

	return(cl);
}

BTSV_API int BT_InvisibleP(btEntity self, btEntity other)
{
	if(other->flags&BT_FL_NOTARGET)
		return(true);
	return(false);
}

BTSV_API int BT_FindTarget(btEntity self)
{
	btEntity other;
	int r;

	//scripted NPC's are blind
	if(self->flags&BT_FL_SCRIPTED)
		return(false);

	other=BT_CheckTarget(self);
	if(!other)return(false);
	
	if(other==self->enemy)
		return(false);
	if(BT_InvisibleP(self, other))
		return(false);
		
	r=BT_Range(self, other);
	if(r==BT_RANGE_FAR)
		return(false);

	if(!BT_VisibleP(self, other))
		return(false);
	
//	printf("BT_FindTarget: Sight %p %p\n", self, other);

	if(r==BT_RANGE_NEAR)
	{
		if((other->showHostile<btTime()) &&
			!BT_InfrontP(self, other))
				return(false);
	}else if(r==BT_RANGE_MID)
	{
		if(!BT_InfrontP(self, other))
			return(false);
	}
	
	self->enemy=other;

#if 0
	while(self->enemy && strcmp(self->enemy->classname, "player"))
	{
		self->enemy=self->enemy->enemy;
	}
	if(!self->enemy)return(false);
#endif
	
	BT_FoundTarget(self);
	return(true);
}


BTSV_API bool BT_CheckPossibleRoam(btEntity self, btEntity other)
{
	if(!strcmp(other->classname, "path_corner_roam"))
	{
		if(other->team && (other->team!=self->team))
			return(false);
		return(true);
	}
	return(false);
}

//determine if a place to roam to is nearby
BTSV_API btEntity BT_CheckRoamTarget(btEntity self)
{
	btEntity cl;
	btEntity cur, nxt, lst;
	float f, d, r, bd;

	cl=NULL; bd=999999;

	//see if anyone else is in range
	cur=btFindRadius(btCalcCenter(self), 512);
	while(cur)
	{
		d=BT_TargetDistance(self, cur);
		r=d/bd;
		
		//ignore if >2x distance to current target
		if(r>2.0)
			{ cur=cur->chain; continue; }

		if(!BT_CheckPossibleRoam(self, cur))
		{
			cur=cur->chain;
			continue;
		}

		f=btRandom()*2;
		if(f>=r) { cl=cur; bd=d; }

		cur=cur->chain;
	}

	return(cl);
}

BTSV_API bool BT_FindRoamTarget(btEntity self)
{
	btEntity other;
	int r;

	//scripted NPC's are blind
	if(self->flags&BT_FL_SCRIPTED)
		return(false);

	//non-roaming NPCs don't roam...
	if(!(self->flags&BT_FL_AI_ROAMING))
		return(false);

	other=BT_CheckRoamTarget(self);
	if(!other)return(false);
	
	self->moveTarget=other;
	self->goalEntity=other;
	return(true);
}

BTSV_API void BTAI_Forward(btEntity self, float dist)
{
	btWalkmove(self, btCurrentYaw(self), dist);
}

BTSV_API void BTAI_Back(btEntity self, float dist)
{
	btWalkmove(self, btCurrentYaw(self)+180, dist);
}

BTSV_API void BTAI_Pain(btEntity self, float dist)
{
	BTAI_Back(self, dist);
}

BTSV_API void BTAI_PainForward(btEntity self, float dist)
{
	btWalkmove(self, self->idealYaw, dist);
}

BTSV_API void BTAI_Walk(btEntity self, float dist)
{
	if(self->flags&BT_FL_SCRIPTED)
	{
		//we are scripted, base action only
		BT_MoveToGoal(self, dist);
		return;
	}


	if(BT_FindTarget(self))
	{
		printf("BTAI_Walk: Found Target\n");
		return;
	}

	if(self->enemy)
	{
		if(btEntRun(self))
		{
			printf("BTAI_Walk: Have Enemy\n");
			return;
		}
	}

#if 1
	if(!self->goalEntity)
	{
		printf("BTAI_Walk: No Goal, Standing\n");
		btEntStand(self);
		return;
	}
#endif

//	printf("BTAI_Walk: %d Moving To Goal\n", self->entnum);

	BT_MoveToGoal(self, dist);
}

BTSV_API void BTAI_Stand(btEntity self, float dist)
{
	if(self->flags&BT_FL_SCRIPTED)
	{
		//we are scripted, base action only
		BTAI_Forward(self, dist);
		return;
	}

	if(BT_FindTarget(self))
		return;

	if(self->enemy)
		if(btEntRun(self))
			return;

	if(btTime()>self->pauseTime)
	{
		btEntWalk(self);
		return;
	}

	//check if there is anywhere to go
	if(BT_FindRoamTarget(self))
	{
		btEntWalk(self);
		return;
	}

	BTAI_Forward(self, dist);
}

BTSV_API void BTAI_Turn(btEntity self)
{
	if(BT_FindTarget(self))
		return;
		
	BT_ChangeYaw(self);
}

BTSV_API void BTAI_ChooseTurn(btEntity self, vec3 dest)
{
	vec3 dir;
	
	dir=v3sub(dest, self->origin);
	self->idealYaw=btVecToYaw(dir);
}

BTSV_API int BT_FacingIdealP(btEntity self)
{
	float d;
	
	d=btAngleMod(btCurrentYaw(self)-self->idealYaw);
	if((d<45) || (d>315))
		return(true);
	return(false);
}

BTSV_API void BTAI_RunMelee(btEntity self)
{
	self->idealYaw=BT_EnemyYaw(self);
	BT_ChangeYaw(self);
	
	if(BT_FacingIdealP(self))
	{
//		if(self->avt->melee)
//			self->avt->melee(self);
		btEntMelee(self);
		self->attackState=BT_AS_STRAIGHT;
	}
}

BTSV_API void BTAI_RunMissile(btEntity self)
{
	self->idealYaw=BT_EnemyYaw(self);
	BT_ChangeYaw(self);
	
	if(BT_FacingIdealP(self))
	{
//		if(self->avt->missile)
//			self->avt->missile(self);
		btEntMissile(self);
		self->attackState=BT_AS_STRAIGHT;
	}
}

BTSV_API void BTAI_StopRun(btEntity self)
{
	if(self->moveTarget)
	{
		btEntWalk(self);
//		if(self->avt->walk)
//			self->avt->walk(self);
	}else
	{
		btEntStand(self);
//		if(self->avt->stand)
//			self->avt->stand(self);
	}
}

BTSV_API void BTAI_Run(btEntity self, float dist)
{
	if(self->flags&BT_FL_SCRIPTED)
	{
		//we are scripted, base action only
//		BTAI_Forward(self, dist);
		BT_MoveToGoal(self, dist);
		return;
	}

	if(!self->enemy)
	{
		//no enemy, disengage running
		BTAI_StopRun(self);
		return;
	}
	
	//is enemy dead?
//	if(self->enemy->health<=0)

	//check if enemy is no longer attackable
	if(BT_CheckNoAttack(self, self->enemy))
	{
		//is an old enemy still alive?
		if(self->oldEnemy && (self->oldEnemy->health>0))
		{
			//yep, go after them
			self->enemy=self->oldEnemy;
			BT_HuntTarget(self);
		}

		//clear and try to find someone else
		self->enemy=NULL;
		if(!BT_FindTarget(self))
			BTAI_StopRun(self);
		//until next time
		return;
	}
	
	self->showHostile=btDelayTime(1);
	
	//if enemy visible, delay boredom
	if(BT_VisibleP(self, self->enemy))
	{
		if(self->flags&BT_FL_AI_NOCHASE)
				self->searchTime=btDelayTime(0.5);
		else	self->searchTime=btDelayTime(5);
	}

	//bored...
	if(self->searchTime<btTime())
	{
		//anyone else to shoot?...
		if(BT_FindTarget(self))
			return;

		if(self->flags&BT_FL_AI_NOCHASE)
		{
			//nochase and lost sight, so give up
			self->enemy=NULL;
			BTAI_StopRun(self);
		}
	}
	
	if(self->attackState==BT_AS_MISSILE)
	{
		BTAI_RunMissile(self);
		return;
	}

	if(self->attackState==BT_AS_MELEE)
	{
		BTAI_RunMelee(self);
		return;
	}

	if(BT_CheckAttack(self))
		return;

	//head towards goal
	BT_MoveToGoal(self, dist);
}

BTSV_API void BTAI_Face(btEntity self)
{
	vec3 dir;
	
	dir=v3sub(self->enemy->origin, self->origin);
	self->idealYaw=btVecToYaw(dir);
	BT_ChangeYaw(self);
}

BTSV_API void BTAI_Charge(btEntity self, float dist)
{
	BTAI_Face(self);
	BT_MoveToGoal(self, dist);
}

BTSV_API void BTAI_Melee(btEntity self)
{
	vec3 dir;
	float dmg;

	if(!self->enemy)
		return;

	dir=v3sub(self->enemy->origin, self->origin);
	if(v3len(dir)>60)
		return;
	dmg=9*btRandom();
	BT_Damage(self->enemy, self, dmg, BT_MOD_MELEE);
}

BTSV_API void BTAI_ScriptedSetAnim(btEntity self, char *anim)
{
	if(!anim)return;
	
	if(!strcmp(anim, "stand"))
		{ btEntStand(self); return; }
	if(!strcmp(anim, "walk"))
		{ btEntWalk(self); return; }
	if(!strcmp(anim, "run"))
		{ btEntRun(self); return; }
	if(!strcmp(anim, "missile"))
		{ btEntMissile(self); return; }
	if(!strcmp(anim, "melee"))
		{ btEntMelee(self); return; }

	if(!strcmp(anim, "teleport"))
	{
		BT_TeleportToTarget(self, self->goalEntity, 0);
		return;
	}

	if(!strcmp(anim, "teleport_silent"))
	{
		BT_TeleportToTarget(self, self->goalEntity,
			BT_SPAWNFLAG_TELEPORT_SILENT);
		return;
	}
}

/** Set out for script node */
BTSV_API void BTAI_ScriptedSetNode(btEntity self, btEntity other)
{
	if(!other || !other->classname)
	{
		if(self->flags&BT_FL_SCRIPTED)
		{
			self->flags&=~BT_FL_SCRIPTED;
			self->goalEntity=NULL;
		}
		return;
	}

	if(!strcmp(other->classname, "path_corner_event"))
	{
		//being sent to a script node
		self->flags|=BT_FL_SCRIPTED;
		self->target=other->target;
		self->killtarget=other->killtarget;
		self->goalEntity=other;
	
		BTAI_ScriptedSetAnim(self, other->oldanim);
		return;
	}

	//disengage from scripting
	self->flags&=~BT_FL_SCRIPTED;

	if(other->flags&(BT_FL_CLIENT|BT_FL_MONSTER))
	{
		//ok, other is a client or monster
		//act as if they shot us
		BT_RespondToDamage(self, other, 0);
		return;
	}
	
	self->goalEntity=other;
	self->moveTarget=self->goalEntity;
	btEntWalk(self);
}

/** Respond to script node */
BTSV_API void BTAI_ScriptedTouchNode(btEntity self, btEntity other)
{
	self->flags|=BT_FL_SCRIPTED;
	self->target=other->target;
	self->killtarget=other->killtarget;

	BTAI_ScriptedSetAnim(self, other->anim);
	self->enemy=btFindTargets(other->killtarget);

	self->goalEntity=NULL;
	self->pauseTime=btDelayTime(other->wait);

	if(other->sound)
	{
		//if there is a sound for the node, play it
		btSound(self, BT_CHAN_VOICE,
			other->sound, 1.0, BT_ATTN_NORM);
	}

	if(other->use_eval)
		btScriptEval(other->use_eval);

	if(other->message)
		btCenterprint(NULL, "%s", other->message);

	BTAI_ScriptedThink(other);
}

/** Called once per frame for scripted NPCs */
BTSV_API void BTAI_ScriptedThink(btEntity self)
{
	btEntity ent;

	if((self->pauseTime>0) && (self->pauseTime<=btTime()))
	{
		self->pauseTime=-1;
		ent=btFindTargets(self->target);
		BTAI_ScriptedSetNode(self, ent);
	}
}

BTSV_API bool BT_CheckAttack(btEntity self)
{
	BTGE_Trace tr;
	vec3 org, torg;
	float chance;
	
	org=btCalcCenter(self);
	torg=btCalcCenter(self->enemy);

	btTraceline(&tr, org, torg, BT_TRACE_MONSTERS, self);
	if(tr.ent!=self->enemy)
		return(false);
	if(BT_Range(self, self->enemy)==BT_RANGE_MELEE)
	{
//		if(self->avt->melee)
//			self->avt->melee(self);

		if(btTime()<self->attackFinished)
			return(false);

//		if(self->avt->melee)
		if(btEntHasMeleeP(self))
			self->attackState=BT_AS_MELEE;
		else self->attackState=BT_AS_MISSILE;

		return(true);
	}

//	if(!self->avt->missile)
	if(!btEntHasMissileP(self))
		return(false);

	if(btTime()<self->attackFinished)
		return(false);

	if(BT_Range(self, self->enemy)==BT_RANGE_FAR)
		return(false);

	if(BT_Range(self, self->enemy)==BT_RANGE_MELEE)
	{
		chance=0.9;
		self->attackFinished=0;
	}else if(BT_Range(self, self->enemy)==BT_RANGE_NEAR)
	{
//		chance=(self->avt->melee)?0.2:0.4;
		chance=btEntHasMeleeP(self)?0.2:0.4;
	}else if(BT_Range(self, self->enemy)==BT_RANGE_MID)
	{
//		chance=(self->avt->melee)?0.05:0.1;
		chance=btEntHasMeleeP(self)?0.05:0.1;
	}else chance=0;
	
	if(btRandom()<chance)
	{
//		self->avt->missile(self);
		self->attackState=BT_AS_MISSILE;
		self->attackFinished=btDelayTime(2*btRandom());
		return(true);
	}
	
	return(false);
}

BTSV_API void btaiStand(btEntity self, float dist)
{
	BTAI_Stand(self, dist);
}

BTSV_API void btaiWalk(btEntity self, float dist)
{
	BTAI_Walk(self, dist);
}

BTSV_API void btaiRun(btEntity self, float dist)
{
	BTAI_Run(self, dist);
}
