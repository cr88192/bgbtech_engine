#include <btgesv.h>

BTGE_BrushWorld *btge_world;

int BT_CanDamage(btEntity self, btEntity targ)
{
	BTGE_Trace tr;
	vec3d mid, org;

	org=btCalcCenter(self);
	mid=btCalcCenter(targ);

	if(targ->movetype==BT_MOVE_PUSH)
	{
		btTraceline(&tr, org, mid,
			BT_TRACE_NOMONSTERS|BT_TRACE_BSPSOLID, self);
		
		if(tr.fraction==1.0)
			return(TRUE);
		if(tr.ent==targ)
			return(TRUE);
		return(FALSE);
	}

	btTraceline(&tr, org, mid,
		BT_TRACE_MONSTERS|BT_TRACE_BSPSOLID, self);
	if(tr.fraction==1.0)return(TRUE);

//	btTraceLine(&tr, self->origin, v3add(targ->origin, vec3(15,0,0)), BT_FL_MONSTER, self);
//	if(tr.fraction==1.0)return(TRUE);

	return(FALSE);
}

void BT_Killed(btEntity self, btEntity other, float dmg, int mod)
{
	if(self->flags&BT_FL_CLIENT)
		if(self->health<(-99))
			self->health=-99;		//clamp for players
	
	if(self->movetype==BT_MOVE_PUSH)
	{
		//shot a door or something
		btEntDie(self, other, dmg);
		return;
	}
	
	self->takedamage=BT_DAMAGE_NO;
//	self->touch=NULL;

	if(self->flags&BT_FL_CLIENT)
		BT_ClientKilled(self, dmg, mod);

	BT_MonsterDeathUse(self, other);
	BT_MonsterDropItems(self, other);
	btEntDie(self, other, dmg);
}

/**
 * self was just damaged, decide whether to respond to the attack
 * damage is the amount of damage, or 0 if called by a trigger
 */
void BT_RespondToDamage(btEntity self, btEntity other, float damage)
{
	//ignore environment
	if(!other)return;

	if(self->flags&BT_FL_SCRIPTED)
	{
		//for now, being scripted==blind
		return;
	}
	
	if(self->flags&BT_FL_MONSTER)
	{
		//ignore self-inflicted pain, or the current enemy shooting us
		if((self==other) || (other==self->enemy))
			return;
		
		//ignore non-active entities
		if(!(other->flags&(BT_FL_CLIENT|BT_FL_MONSTER)))
			return;

		//ignore others if they are not attackable
		if(BT_CheckNoAttack(self, other))
			return;

#if 0
		//ignore others on same team
		if(self->team && (self->team==other->team))
			return;
		
		//ignore monsters of same type (unless on different teams)
		if(!self->team && !other->team &&
			self->classname && other->classname &&
			!strcmp(self->classname, other->classname))
				return;
#endif

		self->oldEnemy=self->enemy;
		self->enemy=other;
		BT_FoundTarget(self);
		return;
	}
}

BTSV_API void BT_Damage(btEntity self, btEntity other,
	float damage, int mod)
{
	float save, take;

	if(!self)return;

	if(self->takedamage==BT_DAMAGE_NO)
		return;
	if(self->flags&BT_FL_INVULNERABLE)
		return;

	//ignore friendly fire
	if(other && (damage>0) && self->team && (self->team==other->team))
		return;

	if((damage>0) && (self->invincibleFinished>btTime()))
	{
		//protection effects
		if(self->flags&BT_FL_CLIENT)
		{
			BT_ClientPain(self, damage, mod);
		}
		return;
	}

//	if(other && (other->quadFinished>btTime()))
	if(other && (other->quadFinished>btTime()) && (self!=other))
		damage*=4;
		
//	save=targ
	//armor and knockback...

	take=damage;

	save=damage*0.5;
	if(save>self->curArmor)
		save=self->curArmor;
		
	self->curArmor-=save;
	take-=save;
		
	self->health-=take;
	if(self->health<=0)
	{
		BT_Killed(self, other, take, mod);
		return;
	}

	if(self->flags&BT_FL_CLIENT)
	{
		if(damage>0)
			BT_ClientPain(self, take, mod);
	}

	if(damage>=0)
	{
		BT_RespondToDamage(self, other, take);
		btEntPain(self, other, take);
	}
}

BTSV_API void BT_Heal(btEntity self, btEntity other, float damage, int mod)
{
	BT_Damage(self, other, -damage, mod);
}

BTSV_API void BT_RadiusDamage(btEntity inflictor, btEntity attacker,
	float damage, btEntity ignore, float radius, int mod)
{
	vec3d origin, org;
	vec3 dir, accel;
	btEntity cur, nxt;
	float dmg, dmg2, pts, scale, mass;
	float f, g;
	int vxfl;
	
	if(damage==0)return;
	if(radius<=0)return;

	origin=btCalcCenter(inflictor);

	vxfl=0;
	if(mod==BT_MOD_ROCKETSHOVEL)
		vxfl|=1;
	BTGE_Voxel_UpdateBlastDamage2(btge_world,
		origin, radius, damage, vxfl);

//	if(mod==BT_MOD_ROCKETSHOVEL)
//		damage*=0.25;

	scale=damage/radius;
//	origin=btCalcCenter(inflictor);
	cur=btFindRadius(origin, radius);
	while(cur)
	{
		if(cur==ignore)
		{
			cur=cur->chain;
			continue;
		}
	
		org=btCalcCenter(cur);
		dir=v3d2f(v3dsub(org, origin));
//		pts=v3len(dir)*0.5;
		pts=btSmallDistancePointBox(origin,
			cur->absmin, cur->absmax);

//		pts=v3len(dir)*scale;
		if(pts<0)pts=0;
//		if(cur==attacker)pts*=0.5;
		if(damage>0)
		{
			dmg=damage-pts;
			if(dmg<0)dmg=0;
			dmg2=(cur==attacker)?(dmg*0.25):dmg;
			if(mod==BT_MOD_ROCKETSHOVEL)
				dmg2*=0.25;
		}else
		{
			dmg=damage+pts;
			if(dmg>0)dmg=0;
			dmg2=dmg;
		}
		nxt=cur->chain;
		
//		if((dmg!=0) && BT_CanDamage(cur, inflictor))
		if(dmg!=0)
		{
			BT_Damage(cur, attacker, dmg2, mod);
		}
		
		//push entity by damage recieved
		if((dmg>0) && (cur->flags&(BT_FL_CLIENT|BT_FL_MONSTER)))
		{
			mass=cur->mass;
			if(mass<=0)
				mass=50;

			if((cur->flags&BT_FL_CLIENT) && (cur==attacker))
			{
				f=dmg/25;
//				g=1600.0/(mass/50);
//				g=2400.0/(mass/50);
//				g=(f*2400.0)/(mass/50);
				g=(f*600.0)/(mass/50);
			}else
			{
				f=dmg/25;
//				f=1;
//				f=CLAMP(f, 0.25, 16);
				f=CLAMP(f, 0.05, 16);
//				g=f*(500.0/(mass/50));
				g=f*(250.0/(mass/50));
			}

			dir=v3d2f(v3dnorm(v3dsub(org, origin)));
			accel=v3scale(dir, g);
			printf("BT_RadiusDamage: Accel (%g,%g,%g) "
				"g=%g cur=%p att=%p\n",
				v3x(accel), v3y(accel), v3z(accel), g, cur, attacker);

			cur->flags&=~BT_FL_ONGROUND;
			cur->velocity=v3add(cur->velocity,
				accel);
		}
		
		cur=nxt;
	}
}


BTSV_API void BT_BoxDamage(btEntity inflictor, btEntity attacker,
	float damage, btEntity ignore, vec3d mins, vec3d maxs, int mod)
{
	btEntity cur, nxt;
	float dmg;
	
	if(damage==0)return;
	
	cur=btFindBox(mins, maxs);
	while(cur)
	{
		if(cur==ignore)
		{
			cur=cur->chain;
			continue;
		}

		dmg=damage;
		if(cur==attacker)dmg*=0.5;
		nxt=cur->chain;
		BT_Damage(cur, attacker, dmg, mod);		
		cur=nxt;
	}
}
