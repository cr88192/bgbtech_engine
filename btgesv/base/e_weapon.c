#include <btgesv.h>

BTGE_MoveFrame weapon_active_mf[]=
{
{NULL, 0, NULL},
{NULL, 0, NULL},
{NULL, 0, NULL},
{NULL, 0, NULL},
{NULL, 0, NULL},
{NULL, 0, NULL},
};

BTGE_MoveFrame weapon_power_mf[]=
{
{NULL, 0, NULL},
{NULL, 0, NULL},
{NULL, 0, NULL},
{NULL, 0, NULL},
{NULL, 0, NULL},
{NULL, 0, weapon_power_think},
{NULL, 0, NULL},
{NULL, 0, NULL},
{NULL, 0, NULL},
{NULL, 0, NULL},
{NULL, 0, weapon_power_think},
{NULL, 0, NULL},
{NULL, 0, NULL},
{NULL, 0, NULL},
{NULL, 0, NULL},
{NULL, 0, weapon_power_think},
{NULL, 0, NULL},
{NULL, 0, NULL},
{NULL, 0, NULL},
{NULL, 0, NULL},
{NULL, 0, weapon_power_think},
{NULL, 0, NULL},
{NULL, 0, NULL},
{NULL, 0, NULL},
{NULL, 0, NULL},
{NULL, 0, weapon_power_think},
{NULL, 0, NULL},
{NULL, 0, NULL},
{NULL, 0, NULL},
{NULL, 0, NULL},
};

BTGE_MoveFrame weapon_idle_mf[]=
{
{NULL, 0, NULL},
{NULL, 0, NULL},
{NULL, 0, NULL},
{NULL, 0, NULL},
{NULL, 0, NULL},
{NULL, 0, NULL},
{NULL, 0, NULL},
{NULL, 0, NULL},
{NULL, 0, NULL},
{NULL, 0, NULL},
{NULL, 0, NULL},
{NULL, 0, NULL},
{NULL, 0, NULL},
{NULL, 0, NULL},
{NULL, 0, NULL},
{NULL, 0, NULL},
{NULL, 0, NULL},
{NULL, 0, NULL},
{NULL, 0, NULL},
{NULL, 0, NULL},
{NULL, 0, NULL},
{NULL, 0, NULL},
{NULL, 0, NULL},
{NULL, 0, NULL},
{NULL, 0, NULL},
{NULL, 0, NULL},
{NULL, 0, NULL},
{NULL, 0, NULL},
{NULL, 0, NULL},
{NULL, 0, NULL},
};

BTGE_MoveInfo weapon_active_mfi =
	{"active", 0, 6, weapon_active_mf, weapon_idle};

BTGE_MoveInfo weapon_putway_mfi =
	{"putway", 0, 6, weapon_active_mf, weapon_idle};

BTGE_MoveInfo weapon_power_mfi =
	{"pow", 0, 27, weapon_power_mf, weapon_power};

BTGE_MoveInfo weapon_idle_mfi =
	{"idle", 0, 30, weapon_idle_mf, weapon_idle};

void weapon_idle(btEntity self)
{
//	if(self->weaponmove==(&weapon_idle_mfi))
//		return;
	btSetWeaponMove(self, &weapon_idle_mfi);
}

void weapon_power(btEntity self)
{
	if(self->weaponmove==(&weapon_power_mfi))
		return;
	btSetWeaponMove(self, &weapon_power_mfi);
}

void weapon_power_think(btEntity self)
{
	if(BT_AttackFinishedP(self))
	{
		weapon_idle(self);
		return;
	}
}

int BT_PlayerImpulseUse(btEntity self)
{
	BTGE_Trace tr;
	btEntity cur;

	printf("BT_PlayerImpulseUse\n");
	
	btTraceline(&tr, self->morg, self->mend,
		BT_TRACE_MONSTERS|BT_TRACE_TRIGGER|BT_TRACE_BSPSOLID, self);
	cur=tr.ent;
	while(cur)
	{
		btEntUse(cur, self);
		cur=cur->chain;
	}

	btSound(self, BT_CHAN_WEAPON, "sound/misc/keyuse",
		1.0, BT_ATTN_NORM);
		
	return(0);
}

int BT_PlayerImpulseDamage(btEntity self)
{
	BTGE_Trace tr;
	vec3d worg;
	float sc_mp, sc_dmg, sc_ft;
	btEntity cur;
	int i;

//	printf("BT_PlayerImpulseDamage\n");

#if 0
	btTraceline(&tr, self->morg, self->mend,
		BT_TRACE_MONSTERS|BT_TRACE_TRIGGER|BT_TRACE_BSPSOLID, self);
	cur=tr.ent;
	while(cur)
	{
//		btEntUse(cur, self);
		BT_Damage(cur, self, 9999, BT_MOD_DAMAGEGUN);
		cur=cur->chain;
	}
#endif

//	btSound(self, BT_CHAN_VOICE, "sound/misc/keyuse",
//		1.0, BT_ATTN_NORM);

//	btSound(self, BT_CHAN_VOICE, "sound/weapons/sshotf1b",
//		1.0, BT_ATTN_NORM);

//	btSound(self, BT_CHAN_VOICE, "sound/weapons/blastf1a",
//		1.0, BT_ATTN_NORM);
		
//	BT_FireRocket(self, self->morg, self->mdir, 120, 600, 64);
//	BT_FireBlaster(self, self->morg, self->mdir, 120, 800, 64);

	if(!BT_AttackFinishedP(self))
		return(0);

	if(self->item)
	{
		//holding an item, so use it...
		btEntityUseItem(self, self->item);
		
		if(self->item->flags&BT_ITFL_CONSUMABLE)
		{
		}
		
		return(0);
	}

//	btSetWeaponMove(self, &weapon_power_mfi);
	weapon_power(self);

	sc_dmg=1.0;
//	sc_mp=0.5;
	sc_mp=1.0;
	sc_ft=1.0;
	
	if(!BT_QuadFinishedP(self))
	{
//		sc_dmg*=4;
		btSound(self, BT_CHAN_ITEM, "sound/items/damage3",
			1.0, BT_ATTN_NORM);
	}
	
	switch(self->weaponnum)
	{
	case 1:
//		BT_RadiusDamage(self, self->owner, self->damage, nodmg,
//			self->height, BT_MOD_ROCKET);

		if(self->curMP<(1*sc_mp))break;
		self->curMP-=1*sc_mp;

//		btSound(self, BT_CHAN_WEAPON, "sound/weapons/blastf1a",
//			1.0, BT_ATTN_NORM);
//		BT_FireBlasterColor(self, self->morg, self->mdir, 45, 800, 24, 1);
		BT_FireBlockPlacerColor(self, self->morg, self->mdir,
			45, 400, 24, 1, -1);
		BT_AttackFinished(self, 0.3*sc_ft);

		break;

	case 2:
		if(self->curMP<(1*sc_mp))break;
		self->curMP-=1*sc_mp;

		btSound(self, BT_CHAN_WEAPON, "sound/weapons/blastf1a",
			1.0, BT_ATTN_NORM);
		BT_FireBlaster(self, self->morg, self->mdir, 25*sc_dmg, 800, 32);
		BT_AttackFinished(self, 0.5*sc_ft);
		break;

	case 3:
		if(self->curMP<(4*sc_mp))break;
		self->curMP-=4*sc_mp;

		btSound(self, BT_CHAN_WEAPON, "sound/weapons/shotgf1b",
			1.0, BT_ATTN_NORM);
		BT_FireBullet(self, self->morg, self->mdir, 35*sc_dmg);
		BT_AttackFinished(self, 0.5*sc_ft);
		break;

	case 4:
		if(self->curMP<(1*sc_mp))break;
		self->curMP-=1*sc_mp;

		btSound(self, BT_CHAN_WEAPON, "sound/weapons/machgf1b",
			1.0, BT_ATTN_NORM);
		BT_FireBullet(self, self->morg, self->mdir, 10*sc_dmg);
		BT_AttackFinishedRapid(self, 0.09*sc_ft);
		break;

	case 5:
		if(self->curMP<(20*sc_mp))break;
		self->curMP-=(20*sc_mp);

		btSound(self, BT_CHAN_WEAPON, "sound/weapons/rocklf1a",
			1.0, BT_ATTN_NORM);
		BT_FireRocket(self, self->morg, self->mdir, 120*sc_dmg, 600, 160);
		BT_AttackFinished(self, 1.0*sc_ft);
		break;

	case 6:
		if(self->curMP<(2*sc_mp))break;
//		self->curMP-=2*sc_mp;
		self->curMP-=1*sc_mp;

		i=btRandom()*3+1;

//		btSound(self, BT_CHAN_WEAPON, "sound/weapons/laser3_2",
//			1.0, BT_ATTN_NORM);
		btSound(self, BT_CHAN_WEAPON, "sound/weapons/laser2_6",
			1.0, BT_ATTN_NORM);
//		BT_FireBullet(self, self->morg, self->mdir, 10*sc_dmg);
		BT_FirePlasma(self, self->morg, self->mdir, 15*sc_dmg, 800, 32, 0);
//		BT_FirePlasma(self, self->morg, self->mdir, 35*sc_dmg, 800, 32, 0);
//		BT_FirePlasma(self, self->morg, self->mdir, 35*sc_dmg, 800, 32, i);
		BT_AttackFinishedRapid(self, 0.09*sc_ft);
		break;

	case 7:
		if(self->curMP<(40*sc_mp))break;
		self->curMP-=40*sc_mp;

//		btSound(self, BT_CHAN_WEAPON, "sound/weapons/laser3_2",
//			1.0, BT_ATTN_NORM);
//		btSound(self, BT_CHAN_WEAPON, "sound/weapons/laser2_6",
//			1.0, BT_ATTN_NORM);
		btSound(self, BT_CHAN_WEAPON, "sound/weapons/bfg__f1y",
			1.0, BT_ATTN_NORM);
		BT_FireBFG(self, self->morg, self->mdir, 240*sc_dmg, 400, 280);
		BT_AttackFinished(self, 2.0*sc_ft);
		break;

	case 8:
		if(self->curMP<(1*sc_mp))break;
		self->curMP-=1*sc_mp;

//		btSound(self, BT_CHAN_WEAPON, "sound/weapons/laser3_2",
//			1.0, BT_ATTN_NORM);
//		btSound(self, BT_CHAN_WEAPON, "sound/weapons/laser2_6",
//			1.0, BT_ATTN_NORM);
//		btSound(self, BT_CHAN_WEAPON, "sound/weapons/bfg__f1y",
//			1.0, BT_ATTN_NORM);
//		BT_FireBFG(self, self->morg, self->mdir, 240, 600, 180);

//		btSound(self, BT_CHAN_WEAPON, "sound/weapons/hyprbl1a",
//			1.0, BT_ATTN_NORM);

//		btSound(self, BT_CHAN_WEAPON, "sounds/tonal/hz60_e0",
//			1.0, BT_ATTN_NORM);

//		btSound(self, BT_CHAN_WEAPON, "sounds/tonal/hz1000t",
//			1.0, BT_ATTN_NORM);

//		btSound(self, BT_CHAN_WEAPON, "sounds/tonal/laser0",
//			1.0, BT_ATTN_NORM);

		btSound(self, BT_CHAN_WEAPON, "sound/weapons/laser4_1",
			1.0, BT_ATTN_NORM);

		worg=v3dsub(self->morg, vec3d(0, 0, 6));

		BT_FireLaser(self, worg, self->mdir, 30*sc_dmg*(1.0/sc_ft));
		BT_AttackFinishedRapid(self, 0.09);
		break;

#if 1
	case 9:
		if(self->curMP<(7*sc_mp))break;
		self->curMP-=(7*sc_mp);

		btSound(self, BT_CHAN_WEAPON, "sound/weapons/rocklf1a",
			1.0, BT_ATTN_NORM);
		BT_FireRocketShovel(self, self->morg, self->mdir,
			120*sc_dmg, 600, 160);
		BT_AttackFinished(self, 1.0*sc_ft);
		break;
#endif

#if 0
	case 9:

		if(self->curMP<(15*sc_mp))break;
		self->curMP-=(15*sc_mp);

		btSound(self, BT_CHAN_WEAPON, "sound/weapons/rocklf1a",
			1.0, BT_ATTN_NORM);
		BT_FireGrenade(self, self->morg, self->mdir, 120*sc_dmg, 600, 160);
		BT_AttackFinished(self, 1.0*sc_ft);
		break;

//		btSpeak2(self, BT_CHAN_VOICE,
//			"female0", NULL, "This is a test",
//			1.0, BT_ATTN_NORM);
		break;
#endif

	default:
		break;
	}

	return(0);
}

int BT_PlayerImpulseAltDamage(btEntity self)
{
	BTGE_Trace tr;
	vec3 worg;
	float sc_mp, sc_dmg, sc_ft;
	btEntity cur;
	int i;

//	printf("BT_PlayerImpulseAltDamage\n");

	if(!BT_AttackFinishedP(self))
		return(0);

	if(self->item)
	{
		//holding an item, so use it...
		btEntityUseItem(self, self->item);
		
		if(self->item->flags&BT_ITFL_CONSUMABLE)
		{
		}
		
		return(0);
	}

//	btSetWeaponMove(self, &weapon_power_mfi);
	weapon_power(self);

	sc_dmg=1.0;
//	sc_mp=0.5;
	sc_mp=1.0;
	sc_ft=1.0;
	switch(self->weaponnum)
	{
	case 1:
//		BT_RadiusDamage(self, self->owner, self->damage, nodmg,
//			self->height, BT_MOD_ROCKET);

		if(self->curMP<(1*sc_mp))break;
		self->curMP-=1*sc_mp;

//		btSound(self, BT_CHAN_WEAPON, "sound/weapons/blastf1a",
//			1.0, BT_ATTN_NORM);
		i=self->placer_vox;
		if(i<=0)i=VOX_TY_COBBLESTONE;
		BT_FireBlockPlacerColor(self, self->morg, self->mdir,
			45, 400, 24, 2, i);
//		BT_FireBlockPlacerColor(self, self->morg, self->mdir,
//			45, 400, 24, 2, VOX_TY_COBBLESTONE);
		BT_AttackFinished(self, 0.3*sc_ft);

		break;

#if 0
	case 2:
		if(self->curMP<(1*sc_mp))break;
		self->curMP-=1*sc_mp;

		btSound(self, BT_CHAN_WEAPON, "sound/weapons/blastf1a",
			1.0, BT_ATTN_NORM);
		BT_FireBlaster(self, self->morg, self->mdir, 25*sc_dmg, 800, 32);
		BT_AttackFinished(self, 0.5*sc_ft);
		break;

	case 3:
		if(self->curMP<(4*sc_mp))break;
		self->curMP-=4*sc_mp;

		btSound(self, BT_CHAN_WEAPON, "sound/weapons/shotgf1b",
			1.0, BT_ATTN_NORM);
		BT_FireBullet(self, self->morg, self->mdir, 35*sc_dmg);
		BT_AttackFinished(self, 0.5*sc_ft);
		break;

	case 4:
		if(self->curMP<(1*sc_mp))break;
		self->curMP-=1*sc_mp;

		btSound(self, BT_CHAN_WEAPON, "sound/weapons/machgf1b",
			1.0, BT_ATTN_NORM);
		BT_FireBullet(self, self->morg, self->mdir, 10*sc_dmg);
		BT_AttackFinishedRapid(self, 0.09*sc_ft);
		break;

	case 5:
		if(self->curMP<(20*sc_mp))break;
		self->curMP-=(20*sc_mp);

		btSound(self, BT_CHAN_WEAPON, "sound/weapons/rocklf1a",
			1.0, BT_ATTN_NORM);
		BT_FireRocket(self, self->morg, self->mdir, 120*sc_dmg, 600, 160);
		BT_AttackFinished(self, 1.0*sc_ft);
		break;

	case 6:
		if(self->curMP<(2*sc_mp))break;
		self->curMP-=2*sc_mp;

		i=btRandom()*3+1;

//		btSound(self, BT_CHAN_WEAPON, "sound/weapons/laser3_2",
//			1.0, BT_ATTN_NORM);
		btSound(self, BT_CHAN_WEAPON, "sound/weapons/laser2_6",
			1.0, BT_ATTN_NORM);
//		BT_FireBullet(self, self->morg, self->mdir, 10*sc_dmg);
		BT_FirePlasma(self, self->morg, self->mdir, 35*sc_dmg, 800, 32, 0);
//		BT_FirePlasma(self, self->morg, self->mdir, 35*sc_dmg, 800, 32, i);
		BT_AttackFinishedRapid(self, 0.09*sc_ft);
		break;

	case 7:
		if(self->curMP<(40*sc_mp))break;
		self->curMP-=40*sc_mp;

//		btSound(self, BT_CHAN_WEAPON, "sound/weapons/laser3_2",
//			1.0, BT_ATTN_NORM);
//		btSound(self, BT_CHAN_WEAPON, "sound/weapons/laser2_6",
//			1.0, BT_ATTN_NORM);
		btSound(self, BT_CHAN_WEAPON, "sound/weapons/bfg__f1y",
			1.0, BT_ATTN_NORM);
		BT_FireBFG(self, self->morg, self->mdir, 240*sc_dmg, 400, 280);
		BT_AttackFinished(self, 2.0*sc_ft);
		break;

	case 8:
		if(self->curMP<(1*sc_mp))break;
		self->curMP-=1*sc_mp;

//		btSound(self, BT_CHAN_WEAPON, "sound/weapons/laser3_2",
//			1.0, BT_ATTN_NORM);
//		btSound(self, BT_CHAN_WEAPON, "sound/weapons/laser2_6",
//			1.0, BT_ATTN_NORM);
//		btSound(self, BT_CHAN_WEAPON, "sound/weapons/bfg__f1y",
//			1.0, BT_ATTN_NORM);
//		BT_FireBFG(self, self->morg, self->mdir, 240, 600, 180);

//		btSound(self, BT_CHAN_WEAPON, "sound/weapons/hyprbl1a",
//			1.0, BT_ATTN_NORM);

//		btSound(self, BT_CHAN_WEAPON, "sounds/tonal/hz60_e0",
//			1.0, BT_ATTN_NORM);

//		btSound(self, BT_CHAN_WEAPON, "sounds/tonal/hz1000t",
//			1.0, BT_ATTN_NORM);

//		btSound(self, BT_CHAN_WEAPON, "sounds/tonal/laser0",
//			1.0, BT_ATTN_NORM);

		btSound(self, BT_CHAN_WEAPON, "sound/weapons/laser4_1",
			1.0, BT_ATTN_NORM);

		worg=v3sub(self->morg, vec3(0, 0, 6));

		BT_FireLaser(self, worg, self->mdir, 30*sc_dmg*(1.0/sc_ft));
		BT_AttackFinishedRapid(self, 0.09);
		break;

	case 9:

		if(self->curMP<(15*sc_mp))break;
		self->curMP-=(15*sc_mp);

		btSound(self, BT_CHAN_WEAPON, "sound/weapons/rocklf1a",
			1.0, BT_ATTN_NORM);
		BT_FireGrenade(self, self->morg, self->mdir, 120*sc_dmg, 600, 160);
		BT_AttackFinished(self, 1.0*sc_ft);
		break;

//		btSpeak2(self, BT_CHAN_VOICE,
//			"female0", NULL, "This is a test",
//			1.0, BT_ATTN_NORM);
		break;
#endif

	default:
		break;
	}

	return(0);
}

int BT_PlayerImpulseAlt2Damage(btEntity self)
{
	BTGE_Trace tr;
	vec3 worg;
	float sc_mp, sc_dmg, sc_ft;
	btEntity cur;
	int i;

//	printf("BT_PlayerImpulseAltDamage\n");

	if(!BT_AttackFinishedP(self))
		return(0);

	if(self->item)
	{
		//holding an item, so use it...
//		btEntityUseItem(self, self->item);
		
		if(self->item->flags&BT_ITFL_CONSUMABLE)
		{
		}
		
		return(0);
	}

	if(self->flags2&BT_FL2_RAILMOVE)
	{
		btSpeak(self, BT_CHAN_VOICE, "rail move disabled",
			1.0, BT_ATTN_NORM);

		self->flags2&=~BT_FL2_RAILMOVE;
		BT_AttackFinished(self, 0.25);
		return(0);
	}

//	btSetWeaponMove(self, &weapon_power_mfi);
	weapon_power(self);

	sc_dmg=1.0;
//	sc_mp=0.5;
	sc_mp=1.0;
	sc_ft=1.0;
	switch(self->weaponnum)
	{
	case 1:
//		BT_RadiusDamage(self, self->owner, self->damage, nodmg,
//			self->height, BT_MOD_ROCKET);

		if(self->curMP<(1*sc_mp))break;
		self->curMP-=1*sc_mp;

//		btSound(self, BT_CHAN_WEAPON, "sound/weapons/blastf1a",
//			1.0, BT_ATTN_NORM);
//		i=self->placer_vox;
//		if(i<=0)i=VOX_TY_COBBLESTONE;
		i=VOX_TY_BLOCK_TRANSIT;
		BT_FireBlockPlacerColor(self, self->morg, self->mdir,
			45, 400, 24, 6, i);
//		BT_FireBlockPlacerColor(self, self->morg, self->mdir,
//			45, 400, 24, 2, VOX_TY_COBBLESTONE);
		BT_AttackFinished(self, 0.3*sc_ft);

		break;

	default:
		i=btPointVoxelType(self->origin, self);
		if(i==VOX_TY_RAIL)
		{
			btSpeak(self, BT_CHAN_VOICE, "rail move enabled",
				1.0, BT_ATTN_NORM);

			self->flags2|=BT_FL2_RAILMOVE;
			BT_AttackFinished(self, 0.25);
			break;
		}
		break;
	}

	return(0);
}

int BT_PlayerHoldItem(btEntity self, btItem item)
{
	self->weaponnum=0;
	self->item=item;

	self->weaponanim="idle";
	self->weaponframe=0;
//	btSetViewModel(self, item->viewModel);
	weapon_idle(self);
	return(0);
}

int BT_PlayerWeaponChange(btEntity self, int num)
{
	char tb[256];
	char *str;

	if(!(self->itemflags&(BT_ITFL_WEAPON1<<(num-1))))
	{
		sprintf(tb, "No weapon %d\n", num);
		btSpeak(self, BT_CHAN_VOICE, tb,
			1.0, BT_ATTN_NORM);

		btPrint(self, "Don't have weapon %d\n", num);
		return(-1);
	}

	self->weaponnum=num;
	self->item=NULL;

	str=NULL;
	switch(num)
	{
		case 1: str="models/weapons/v_handgr/v_handgr.model"; break;
		case 2: str="models/weapons/v_blast/v_blast.model"; break;
		case 3: str="models/weapons/v_shotg2/v_shotg2.model"; break;
		case 4: str="models/weapons/v_chain/v_chain.model"; break;
		case 5: str="models/weapons/v_rocket/v_rocket.model"; break;
		case 6: str="models/weapons/v_hyperb/v_hyperb.model"; break;
//		case 7: str="models/weapons/v_launch/v_launch.model"; break;
		case 7: str="models/weapons/v_bfg/v_bfg.model"; break;
		case 8: str="models/weapons/v_rail/v_rail.model"; break;
		case 9: str="models/weapons/v_rockets/v_rockets.model"; break;
//		case 9: str="models/weapons/v_handgr/v_handgr.model"; break;
		default: break;
	}
	
	self->weaponanim="idle";
	self->weaponframe=0;

	btSetViewModel(self, str);
//	btSetWeaponMove(self, &weapon_idle_mfi);
	weapon_idle(self);

	return(0);
}

BTSV_API int BT_PlayerImpulse(btEntity self, int flags)
{
//	if(flags)
//		printf("BT_PlayerImpulse: %08X\n", flags);
	
	//dead, so ignore impulse commands
	if(self->health<=0)
	{
		//enforce death delay
		if(!BT_AttackFinishedP(self))
			return(0);

		if((flags&BT_IMPULSE_USE) || (flags&BT_IMPULSE_FIRE))
			BT_ClientRespawn(self);

		return(0);
	}

	//in a dialog box
	if(self->dlg)
	{
		return(BT_PlayerDialogImpulse(self, flags));
	}
	
	if(flags&BT_IMPULSE_0)
		return(BT_PlayerWeaponChange(self, 10));
	if(flags&BT_IMPULSE_1)
		return(BT_PlayerWeaponChange(self, 1));
	if(flags&BT_IMPULSE_2)
		return(BT_PlayerWeaponChange(self, 2));
	if(flags&BT_IMPULSE_3)
		return(BT_PlayerWeaponChange(self, 3));
	if(flags&BT_IMPULSE_4)
		return(BT_PlayerWeaponChange(self, 4));
	if(flags&BT_IMPULSE_5)
		return(BT_PlayerWeaponChange(self, 5));
	if(flags&BT_IMPULSE_6)
		return(BT_PlayerWeaponChange(self, 6));
	if(flags&BT_IMPULSE_7)
		return(BT_PlayerWeaponChange(self, 7));
	if(flags&BT_IMPULSE_8)
		return(BT_PlayerWeaponChange(self, 8));
	if(flags&BT_IMPULSE_9)
		return(BT_PlayerWeaponChange(self, 9));

	if(!(self->flags&BT_FL_CREATIVE))
	{
		if(flags&BT_IMPULSE_ALTFIRE2)
			BT_PlayerImpulseAlt2Damage(self);

//		if(flags&(BT_IMPULSE_FIRE|BT_IMPULSE_USE))
		if(flags&BT_IMPULSE_USE)
			BT_PlayerImpulseUse(self);
		if(flags&BT_IMPULSE_FIRE)
			BT_PlayerImpulseDamage(self);
		if(flags&BT_IMPULSE_ALTFIRE)
			BT_PlayerImpulseAltDamage(self);
	}

	return(0);
}

BTSV_API btEntity BT_BulletMuzzleFlash(btEntity self,
	vec3d org, vec3 dir, int mod)
{
	btEntity tmp;
	
	tmp=btNewEntityTriggerInit(NULL, NULL);
//	tmp->solidtype=BT_SOLID_BBOX;
//	tmp->solidtype=BT_SOLID_TRIGGER;
//	tmp->movetype=BT_MOVE_FLYMISSILE;
	tmp->owner=self;

	tmp->origin=org;
	tmp->angles=v3sub(btVecToAngles(dir), vec3(0,0,180));
	tmp->velocity=v3scale(dir, 1000);
	
	tmp->effects=BT_EFF_NOSHADOW|
		BT_EFF_GLOW_RED|BT_EFF_GLOW_GREEN|BT_EFF_GLOW_HIGH|
		BT_EFF_GLOW_BRIGHT1|BT_EFF_GLOW_BRIGHT0;
	btSetThink(tmp, btDeleteEntity, 0.1);
	
	return(tmp);
}

BTSV_API btEntity BT_FireBullet(btEntity self,
	vec3d org, vec3 dir, float dmg)
{
	BTGE_Trace tr;
	btEntity cur;

	BT_BulletMuzzleFlash(self, org, dir, BT_MOD_BULLET);

	btTraceline(&tr, self->morg, self->mend,
		BT_TRACE_MONSTERS|BT_TRACE_BSPSOLID, self);

	cur=tr.ent;
	while(cur)
	{
		BT_Damage(cur, self, dmg, BT_MOD_BULLET);
		cur=cur->chain;
	}
	
	return(tr.ent);
}

BTSV_API void BT_RocketTouch(btEntity self, btEntity other)
{
	btEntity nodmg;
	int i;

	if(other)
	{
		if(other==self->owner)return;
		if(other->solidtype==BT_SOLID_NOT)return;
		if(other->solidtype==BT_SOLID_TRIGGER)return;

		i=btEntityBoxContents(other, self->absmin, self->absmax);
		if(!(i&BT_CONTENTS_CHECKSOLID))return;
		if(i&BT_CONTENTS_SKY)return;
	}
	
	nodmg=NULL;
	if(self->owner && (self->owner->flags&BT_FL_MONSTER))
	{
		//monsters shouldn't blow themselves up with their own missiles
		nodmg=self->owner;
	}

	self->vt->touch=NULL;

	if(!nodmg)nodmg=other;
	BT_Damage(other, self->owner, self->damage, BT_MOD_ROCKET);
	BT_RadiusDamage(self, self->owner, self->damage, nodmg,
		self->height, BT_MOD_ROCKET);
	BT_EntityExplode(self);
}

BTSV_API void BT_RocketShovelTouch(btEntity self, btEntity other)
{
	btEntity nodmg;
	int i;

	if(other)
	{
		if(other==self->owner)return;
		if(other->solidtype==BT_SOLID_NOT)return;
		if(other->solidtype==BT_SOLID_TRIGGER)return;

		i=btEntityBoxContents(other, self->absmin, self->absmax);
		if(!(i&BT_CONTENTS_CHECKSOLID))return;
		if(i&BT_CONTENTS_SKY)return;
	}
	
	nodmg=NULL;
	if(self->owner && (self->owner->flags&BT_FL_MONSTER))
	{
		//monsters shouldn't blow themselves up with their own missiles
		nodmg=self->owner;
	}

	self->vt->touch=NULL;

	if(!nodmg)nodmg=other;
//	BT_Damage(other, self->owner, self->damage, BT_MOD_ROCKETSHOVEL);
	BT_RadiusDamage(self, self->owner, self->damage, nodmg,
		self->height, BT_MOD_ROCKETSHOVEL);
	BT_EntityExplode(self);
}

BTSV_API void BT_GrenadeTouch(btEntity self, btEntity other)
{
	btSound(self, BT_CHAN_VOICE, "sound/weapons/grenlb1b",
		1.0, BT_ATTN_NORM);
}

BTSV_API void BT_BlasterTouch(btEntity self, btEntity other)
{
	btEntity nodmg;
	int i;

	if(other)
	{
		if(other==self->owner)return;
		if(other->solidtype==BT_SOLID_NOT)return;
		if(other->solidtype==BT_SOLID_TRIGGER)return;

		i=btEntityBoxContents(other, self->absmin, self->absmax);
		if(!(i&BT_CONTENTS_CHECKSOLID))return;
		if(i&BT_CONTENTS_SKY)return;
	}
	
	nodmg=NULL;
	if(self->owner && (self->owner->flags&BT_FL_MONSTER))
	{
		//monsters shouldn't blow themselves up with their own missiles
		nodmg=self->owner;
	}

	self->vt->touch=NULL;

	if(!nodmg)nodmg=other;
	BT_Damage(other, self->owner, self->damage, BT_MOD_BLASTER);
	BT_RadiusDamage(self, self->owner, self->damage, nodmg,
		self->height, BT_MOD_BLASTER);
//	BT_EntitySparksExplode(self);
	BT_EntitySmallSparksExplode(self);
}

BTSV_API void BT_BlockPlacerTouch(btEntity self, btEntity other)
{
	float tv[3];
	BTGE_VoxelData *vox, *vox1;
	btEntity nodmg;
	vec3d org;
	int x, y, z, di;
	int i;

	if(other)
	{
		if(other==self->owner)return;
		if(other->solidtype==BT_SOLID_NOT)return;
		if(other->solidtype==BT_SOLID_TRIGGER)return;

		i=btEntityBoxContents(other, self->absmin, self->absmax);
		if(!(i&BT_CONTENTS_CHECKSOLID))return;
		if(i&BT_CONTENTS_SKY)return;
		
		btEntUse(other, self->owner);
		BT_EntitySmallSparksExplode(self);
		return;
	}

	if(self->solidtype==BT_SOLID_NOT)
		return;

	nodmg=NULL;
	if(self->owner && (self->owner->flags&BT_FL_MONSTER))
	{
		//monsters shouldn't blow themselves up with their own missiles
		nodmg=self->owner;
	}

	self->vt->touch=NULL;

	if(!nodmg)nodmg=other;

	if((self->placer_vox>=0) && (self->placer_vox!=VOX_TY_BLOCK_TRANSIT))
	{
//		org=v3sub(self->origin, v3scale(self->velocity, 0.01));
		org=self->origin;
	}else
	{
		x=floor(v3dx(self->origin)/32);
		y=floor(v3dy(self->origin)/32);
		z=floor(v3dz(self->origin)/32);
		vox=BTGE_Voxel_GetWorldVoxel(btCurrentWorld(), x, y, z);

//		if(!BTGE_Voxel_VoxelFluidP(vox))
			org=v3dadd(self->origin, v3f2d(v3scale(self->velocity, 0.025)));
	}
//	x=v3x(org)/32;
//	y=v3y(org)/32;
//	z=v3z(org)/32;

	x=floor(v3dx(org)/32);
	y=floor(v3dy(org)/32);
	z=floor(v3dz(org)/32);

	vox=BTGE_Voxel_GetWorldVoxel(btCurrentWorld(), x, y, z);

	if(self->placer_vox==VOX_TY_BLOCK_TRANSIT)
	{
		if(self->owner)
		{
			self->owner->placer_vox=vox->type;
//			BT_EntityGiveVoxel(self->owner, vox, 1);
		}

		btSound(self, BT_CHAN_VOICE, "sound/weapons/blkbrk0",
			1.0, BT_ATTN_NORM);
		BT_EntitySmallSparksExplode(self);
		return;
	}

	if((self->placer_vox>0) &&
		(!vox || !(vox->type&VOX_TY_MASK) ||
		BTGE_Voxel_VoxelFluidP(vox)))
	{
		if(!BT_EntityTryInventoryVoxTyTakeCount(self->owner,
			self->placer_vox&VOX_TY_MASK, 1))
		{
		}

		BTGE_Voxel_GetWorldVoxelDirty(btCurrentWorld(), x+1, y, z);
		BTGE_Voxel_GetWorldVoxelDirty(btCurrentWorld(), x-1, y, z);
		BTGE_Voxel_GetWorldVoxelDirty(btCurrentWorld(), x, y+1, z);
		BTGE_Voxel_GetWorldVoxelDirty(btCurrentWorld(), x, y-1, z);
		BTGE_Voxel_GetWorldVoxelDirty(btCurrentWorld(), x, y, z+1);
		BTGE_Voxel_GetWorldVoxelDirty(btCurrentWorld(), x, y, z-1);

		//place block
		vox=BTGE_Voxel_GetWorldVoxelDirty(btCurrentWorld(), x, y, z);
		vox->type=self->placer_vox;
		vox->aux=self->placer_aux&0xFF;
		vox->aux2=(self->placer_aux>>8)&0xFFFF;

		if(self->placer_value)
		{
			i=BTGE_Voxel_WorldIndexValue(
				btCurrentWorld(), x, y, z,
				self->placer_value);
			vox->aux2=i;
		}

		switch(vox->type&VOX_TY_MASK)
		{
		case VOX_TY_TORCH:
		case VOX_TY_REDSTONE_TORCH0:
		case VOX_TY_REDSTONE_TORCH1:
		case VOX_TY_GREENSTONE_TORCH0:
		case VOX_TY_GREENSTONE_TORCH1:
		case VOX_TY_BLUESTONE_TORCH0:
		case VOX_TY_BLUESTONE_TORCH1:
			vfvec3(tv, self->velocity);
			if(fabs(tv[0])>=fabs(tv[1]))
			{
				if(fabs(tv[0])>=fabs(tv[2]))
					{ di=(tv[0]>=0)?2:1; }
				else { di=5; }
			}else
			{
				if(fabs(tv[1])>=fabs(tv[2]))
					{ di=(tv[1]>=0)?3:4; }
				else { di=5; }
			}
			vox->aux=(vox->aux&0xF0)|di;
			break;
		case VOX_TY_REDSTONE_RELAY0:
		case VOX_TY_REDSTONE_RELAY1:
		case VOX_TY_REDSTONE_GATE_AND:
		case VOX_TY_REDSTONE_GATE_OR:
		case VOX_TY_REDSTONE_GATE_XOR:
		case VOX_TY_REDSTONE_GATE_NAND:
		case VOX_TY_REDSTONE_GATE_NOR:
		case VOX_TY_REDSTONE_GATE_XNOR:
			vfvec3(tv, self->velocity);
			if(fabs(tv[0])>=fabs(tv[1]))
			{
				di=(tv[0]>=0)?VOX_AXFL_RELAY_EAST:VOX_AXFL_RELAY_WEST;

//				if(fabs(tv[0])>=fabs(tv[2]))
//					{ di=(tv[0]>=0)?VOX_AXFL_RELAY_EAST:
//						VOX_AXFL_RELAY_WEST; }
//				else { di=VOX_AXFL_RELAY_UP; }
			}else
			{
				di=(tv[1]>=0)?VOX_AXFL_RELAY_NORTH:VOX_AXFL_RELAY_SOUTH;

//				if(fabs(tv[1])>=fabs(tv[2]))
//					{ di=(tv[1]>=0)?VOX_AXFL_RELAY_NORTH:
//						VOX_AXFL_RELAY_SOUTH; }
//				else { di=VOX_AXFL_RELAY_UP; }
			}
//			vox->aux=(vox->aux&0xF0)|di;
			vox->aux=(vox->aux&0xFC)|di;
			break;

		case VOX_TY_RAIL:
			vfvec3(tv, self->velocity);
			if(fabs(tv[0])>=fabs(tv[1]))
			{
				di=1;
			}else
			{
				di=0;
			}
			vox->aux=(vox->aux&0xFC)|di;
			break;
		case VOX_TY_WOOD_STAIRS:
		case VOX_TY_COBBLE_STAIRS:
		case VOX_TY_BRICK_STAIRS:
		case VOX_TY_STONEBRICK_STAIRS:
		case VOX_TY_NETHERBRICK_STAIRS:
		case VOX_TY_SANDSTONE_STAIRS:
		case VOX_TY_SPRUCE_STAIRS:
		case VOX_TY_BIRCH_STAIRS:
		case VOX_TY_JUNGLE_STAIRS:
			vfvec3(tv, self->velocity);
			if(fabs(tv[0])>=fabs(tv[1]))
			{
				di=(tv[0]>=0)?VOX_AXFL_STAIRS_EAST:VOX_AXFL_STAIRS_WEST;
			}else
			{
				di=(tv[1]>=0)?VOX_AXFL_STAIRS_NORTH:VOX_AXFL_STAIRS_SOUTH;
			}
			vox->aux=(vox->aux&0xFC)|di;
			break;
		case VOX_TY_PISTON:
		case VOX_TY_STICKY_PISTON:
		case VOX_TY_PISTON_HEAD:
			vfvec3(tv, self->velocity);
			if(fabs(tv[0])>=fabs(tv[1]))
			{
				if(fabs(tv[0])>=fabs(tv[2]))
				{
					di=(tv[0]>=0)?
						VOX_AXFL_PISTON_WEST:
						VOX_AXFL_PISTON_EAST;
				}
				else
				{
					di=(tv[2]>=0)?
						VOX_AXFL_PISTON_DOWN:
						VOX_AXFL_PISTON_UP;
				}
			}else
			{
				if(fabs(tv[1])>=fabs(tv[2]))
				{
					di=(tv[1]>=0)?
						VOX_AXFL_PISTON_SOUTH:
						VOX_AXFL_PISTON_NORTH;
				}
				else
				{
					di=(tv[2]>=0)?
						VOX_AXFL_PISTON_DOWN:
						VOX_AXFL_PISTON_UP;
				}
			}
			vox->aux=(vox->aux&0xF0)|di;
			break;

		case VOX_TY_LADDER:
			vfvec3(tv, self->velocity);
			if(fabs(tv[0])>=fabs(tv[1]))
			{
				di=(tv[0]>=0)?
					VOX_AXFL_LADDER_WEST:
					VOX_AXFL_LADDER_EAST;
			}else
			{
				di=(tv[1]>=0)?
					VOX_AXFL_LADDER_SOUTH:
					VOX_AXFL_LADDER_NORTH;
			}
			vox->aux=(vox->aux&0xF0)|di;
			break;
		case VOX_TY_WOOD_DOOR:
		case VOX_TY_IRON_DOOR:
			vfvec3(tv, self->velocity);
			if(fabs(tv[0])>=fabs(tv[1]))
			{
				di=(tv[0]>=0)?
					VOX_AXFL_DOOR_WEST:
					VOX_AXFL_DOOR_EAST;
			}else
			{
				di=(tv[1]>=0)?
					VOX_AXFL_DOOR_SOUTH:
					VOX_AXFL_DOOR_NORTH;
			}
			vox->aux=(vox->aux&0xF0)|di;

			vox1=BTGE_Voxel_GetWorldVoxelDirty(btCurrentWorld(), x, y, z+1);
			if(!vox1->type)
			{
				vox1->type=self->placer_vox;
				vox1->aux=vox->aux|0x08;
				vox1->aux2=(self->placer_aux>>8)&0xFFFF;
			}else
			{
				vox1=BTGE_Voxel_GetWorldVoxelDirty(
					btCurrentWorld(), x, y, z-1);
				if(!vox1->type)
				{
					vox1->type=self->placer_vox;
					vox1->aux=vox->aux;
					vox1->aux2=(self->placer_aux>>8)&0xFFFF;
					vox->aux|=0x08;
				}else
				{
					vox->type=0;
				}
			}

			break;

		default: break;
		}

		btSound(self, BT_CHAN_VOICE, "sound/weapons/blkset0",
			1.0, BT_ATTN_NORM);

		BT_EntitySmallSparksExplode(self);
		return;
	}

	if((self->placer_vox<0) && vox && ((vox->type&VOX_TY_MASK)>0))
	{
		if(self->owner)
		{
			BT_EntityGiveVoxel(self->owner, vox, 1);
		}

		BTGE_Voxel_GetWorldVoxelDirty(btCurrentWorld(), x+1, y, z);
		BTGE_Voxel_GetWorldVoxelDirty(btCurrentWorld(), x-1, y, z);
		BTGE_Voxel_GetWorldVoxelDirty(btCurrentWorld(), x, y+1, z);
		BTGE_Voxel_GetWorldVoxelDirty(btCurrentWorld(), x, y-1, z);
		BTGE_Voxel_GetWorldVoxelDirty(btCurrentWorld(), x, y, z+1);
		BTGE_Voxel_GetWorldVoxelDirty(btCurrentWorld(), x, y, z-1);

		//destroy block
		vox=BTGE_Voxel_GetWorldVoxelDirty(btCurrentWorld(), x, y, z);
		
		switch(vox->type)
		{
		case VOX_TY_WOOD_DOOR:
		case VOX_TY_IRON_DOOR:
			vox1=BTGE_Voxel_GetWorldVoxelDirty(btCurrentWorld(), x, y, z+1);
			if(vox1->type==vox->type)
				{ vox1->type=0; vox1->aux=0; vox1->aux2=0; }
			vox1=BTGE_Voxel_GetWorldVoxelDirty(btCurrentWorld(), x, y, z-1);
			if(vox1->type==vox->type)
				{ vox1->type=0; vox1->aux=0; vox1->aux2=0; }
			vox->type=0;
			vox->aux=0;
			vox->aux2=0;
			break;

		default:
			vox->type=0;
			vox->aux=0;
			vox->aux2=0;
			break;
		}

		btSound(self, BT_CHAN_VOICE, "sound/weapons/blkbrk0",
			1.0, BT_ATTN_NORM);
		BT_EntitySmallSparksExplode(self);
		return;
	}

	if(self->placer_vox<0)
	{
		dyPrintf("BT_BlockPlacerTouch: Explosive Break\n");
		BT_RadiusDamage(self, self->owner, self->damage, nodmg,
			self->height, BT_MOD_BLASTER);

		btSound(self, BT_CHAN_VOICE, "sound/weapons/blkbrk0",
			1.0, BT_ATTN_NORM);
		BT_EntitySmallSparksExplode(self);
		return;
	}

	dyPrintf("BT_BlockPlacerTouch: No Action\n");
	BT_EntitySparksExplode(self);

//	BT_Damage(other, self->owner, self->damage, BT_MOD_BLASTER);
//	BT_RadiusDamage(self, self->owner, self->damage, nodmg,
//		self->height, BT_MOD_BLASTER);
//	BT_EntitySparksExplode(self);
}

BTSV_API void BT_AnimDieThink(btEntity self)
{
	self->frame++;
	if(self->frame>=self->count)
	{
		btSetModel(self, NULL);
		btSetThink(self, btDeleteEntity, 0.1);
		return;
	}
	btSetThink(self, BT_AnimDieThink, 0.1);
}

BTSV_API void BT_PlasmaTouch(btEntity self, btEntity other)
{
	btEntity nodmg;
	int i;

	if(other)
	{
		if(other==self->owner)return;
		if(other->solidtype==BT_SOLID_NOT)return;
		if(other->solidtype==BT_SOLID_TRIGGER)return;

		i=btEntityBoxContents(other, self->absmin, self->absmax);
		if(!(i&BT_CONTENTS_CHECKSOLID))return;
		if(i&BT_CONTENTS_SKY)return;
	}
	
//	*(int *)-1=-1;
	
	nodmg=NULL;
	if(self->owner && (self->owner->flags&BT_FL_MONSTER))
	{
		//monsters shouldn't blow themselves up with their own missiles
		nodmg=self->owner;
	}

	self->vt->touch=NULL;

	btSound(self, BT_CHAN_VOICE, "sound/weapons/lashit",
		1.0, BT_ATTN_NORM);

	if(!nodmg)nodmg=other;
	BT_Damage(other, self->owner, self->damage, BT_MOD_PLASMA);
	BT_RadiusDamage(self, self->owner, self->damage, nodmg,
		self->height, BT_MOD_PLASMA);
//	BT_EntitySparksExplode(self);

	self->count=5;
	BT_AnimDieThink(self);
}

BTSV_API void BT_BFGTouch(btEntity self, btEntity other)
{
	btEntity nodmg;
	int i;

	if(other)
	{
		if(other==self->owner)return;
		if(other->solidtype==BT_SOLID_NOT)return;
		if(other->solidtype==BT_SOLID_TRIGGER)return;

		i=btEntityBoxContents(other, self->absmin, self->absmax);
		if(!(i&BT_CONTENTS_CHECKSOLID))return;
		if(i&BT_CONTENTS_SKY)return;
	}
	
	nodmg=NULL;
	if(self->owner && (self->owner->flags&BT_FL_MONSTER))
	{
		//monsters shouldn't blow themselves up with their own missiles
		nodmg=self->owner;
	}

	self->vt->touch=NULL;

	btSound(self, BT_CHAN_VOICE, "sound/weapons/bfg__x1b",
		1.0, BT_ATTN_LOW);

	if(!nodmg)nodmg=other;
	BT_Damage(other, self, self->damage, BT_MOD_BIGPLASMA);
	BT_RadiusDamage(self, self->owner, self->damage, nodmg,
		self->height, BT_MOD_BIGPLASMA);
//	BT_EntitySparksExplode(self);

	btSetModel(self, "s_bfg3");
	self->frame=-1;
	self->count=5;
	BT_AnimDieThink(self);
}

BTSV_API void BT_RocketThink(btEntity self)
	{ btEntTouch(self, NULL); }
BTSV_API void BT_PlacerThink(btEntity self)
{
//	btEntTouch(self, NULL);
	BT_EntitySparksExplode(self);
}

BTSV_API void BT_GrenadeThink(btEntity self)
{
	btEntity nodmg;
	int i;

	nodmg=NULL;
	if(self->owner && (self->owner->flags&BT_FL_MONSTER))
	{
		//monsters shouldn't blow themselves up with their own missiles
		nodmg=self->owner;
	}

	if(self->vt)
		{ self->vt->touch=NULL; }

//	if(!nodmg)nodmg=other;
//	BT_Damage(other, self->owner, self->damage, BT_MOD_ROCKET);
	BT_RadiusDamage(self, self->owner, self->damage, nodmg,
		self->height, BT_MOD_ROCKET);
	BT_EntityExplode(self);
}

BTSV_API btEntity BT_FireRocket(btEntity self,
	vec3d org, vec3 dir, float dmg, float speed, float radius)
{
	btEntity tmp;
	
	if((self->flags&BT_FL_MONSTER) && !(self->flags&BT_FL_AI_PASSIVE))
	{
		if(btGetSkill()==BT_SKILL_EASY)
			{ dmg*=0.5; }
		if((btGetSkill()==BT_SKILL_HARD) ||
			(btGetSkill()==BT_SKILL_NIGHTMARE))
			{ dmg*=4; }
	}
	
	tmp=btNewEntityTriggerInit(NULL, NULL);
//	tmp->solidtype=BT_SOLID_BBOX;
	tmp->solidtype=BT_SOLID_TRIGGER;
	tmp->movetype=BT_MOVE_FLYMISSILE;
	tmp->owner=self;
	
	tmp->mins=vec3(-8,-8,-8);
	tmp->maxs=vec3( 8, 8, 8);
	
	btSetModel(tmp, "models/objects/rocket/rocket0.ac");
	
	tmp->origin=org;
	tmp->angles=v3sub(btVecToAngles(dir), vec3(0,0,180));
	tmp->velocity=v3scale(dir, speed);
	tmp->damage=dmg;
	tmp->height=radius;
	
	tmp->effects=BT_EFF_SMOKETRAIL|BT_EFF_NOSHADOW|
		BT_EFF_GLOW_RED|BT_EFF_GLOW_HIGH|
		BT_EFF_GLOW_BRIGHT1|BT_EFF_GLOW_BRIGHT0;
	
	tmp->vt->touch=BT_RocketTouch;
	btSetThink(tmp, BT_RocketThink, 25);
	
	return(tmp);
}

BTSV_API btEntity BT_FireRocketShovel(btEntity self,
	vec3d org, vec3 dir, float dmg, float speed, float radius)
{
	btEntity tmp;
	tmp=BT_FireRocket(self, org, dir, dmg, speed, radius);
	tmp->vt->touch=BT_RocketShovelTouch;
	return(tmp);
}

BTSV_API btEntity BT_FireGrenade(btEntity self,
	vec3d org, vec3 dir, float dmg, float speed, float radius)
{
	btEntity tmp;
	
	tmp=btNewEntityTriggerInit(NULL, NULL);
//	tmp->solidtype=BT_SOLID_BBOX;
	tmp->solidtype=BT_SOLID_TRIGGER;
	tmp->movetype=BT_MOVE_BOUNCE;
	tmp->owner=self;
	
	tmp->mins=vec3(-8,-8,-8);
	tmp->maxs=vec3( 8, 8, 8);
	
	btSetModel(tmp, "models/objects/rocket/rocket0.ac");
	
	tmp->origin=org;
	tmp->angles=v3sub(btVecToAngles(dir), vec3(0,0,180));
	tmp->velocity=v3scale(dir, speed);
	tmp->damage=dmg;
	tmp->height=radius;
	
	tmp->effects=BT_EFF_SMOKETRAIL|BT_EFF_NOSHADOW|
		BT_EFF_GLOW_RED|BT_EFF_GLOW_HIGH|
		BT_EFF_GLOW_BRIGHT1|BT_EFF_GLOW_BRIGHT0;
	
	tmp->vt->touch=BT_GrenadeTouch;
	btSetThink(tmp, BT_GrenadeThink, 10);
	
	return(tmp);
}

BTSV_API btEntity BT_FireBlaster(btEntity self,
	vec3d org, vec3 dir, float dmg, float speed, float radius)
{
	return(BT_FireBlasterColor(self, org, dir, dmg, speed, radius, 0));
}

BTSV_API btEntity BT_FireBlasterColor(btEntity self,
	vec3d org, vec3 dir, float dmg, float speed, float radius, int clr)
{
	btEntity tmp;

	if((self->flags&BT_FL_MONSTER) && !(self->flags&BT_FL_AI_PASSIVE))
	{
		if(btGetSkill()==BT_SKILL_EASY)
			{ dmg*=0.5; }
		if((btGetSkill()==BT_SKILL_HARD) ||
			(btGetSkill()==BT_SKILL_NIGHTMARE))
			{ dmg*=4; }
	}

	tmp=btNewEntityTriggerInit(NULL, NULL);
//	tmp->solidtype=BT_SOLID_BBOX;
	tmp->solidtype=BT_SOLID_TRIGGER;
	tmp->movetype=BT_MOVE_FLYMISSILE;
	tmp->owner=self;
	
	tmp->mins=vec3(-2,-2,-2);
	tmp->maxs=vec3( 2, 2, 2);
	
	btSetModel(tmp, "models/objects/laser/laser0.ac");
	
	tmp->origin=org;
	tmp->angles=v3sub(btVecToAngles(dir), vec3(0,0,180));
	tmp->velocity=v3scale(dir, speed);
	tmp->damage=dmg;
	tmp->height=radius;
	
	tmp->effects=BT_EFF_NOSHADOW|
		BT_EFF_SPARKSTRAIL|
//		BT_EFF_GLOW_RED|BT_EFF_GLOW_GREEN|
		// BT_EFF_GLOW_HIGH|
		BT_EFF_GLOW_BRIGHT1|BT_EFF_GLOW_BRIGHT0;
	
	switch(clr)
	{
	case 0: tmp->effects|=BT_EFF_GLOW_RED|BT_EFF_GLOW_GREEN; break;
	case 1: tmp->effects|=BT_EFF_GLOW_RED; break;
	case 2: tmp->effects|=BT_EFF_GLOW_GREEN; break;
	case 3: tmp->effects|=BT_EFF_GLOW_RED|BT_EFF_GLOW_GREEN; break;
	case 4: tmp->effects|=BT_EFF_GLOW_BLUE; break;
	case 5: tmp->effects|=BT_EFF_GLOW_RED|BT_EFF_GLOW_BLUE; break;
	case 6: tmp->effects|=BT_EFF_GLOW_GREEN|BT_EFF_GLOW_BLUE; break;
	case 7: tmp->effects|=BT_EFF_GLOW_WHITE; break;
	}
	
	tmp->vt->touch=BT_BlasterTouch;
	btSetThink(tmp, BT_RocketThink, 25);
	
	return(tmp);
}

BTSV_API btEntity BT_FireBlockPlacerColor(btEntity self,
	vec3d org, vec3 dir, float dmg, float speed, float radius,
	int clr, int voxty)
{
	btEntity tmp;
	
	tmp=btNewEntityTriggerInit(NULL, NULL);
//	tmp->solidtype=BT_SOLID_BBOX;
	tmp->solidtype=BT_SOLID_TRIGGER;
	tmp->movetype=BT_MOVE_FLYMISSILE;
	tmp->owner=self;
	
	tmp->mins=vec3(-2,-2,-2);
	tmp->maxs=vec3( 2, 2, 2);
	
	tmp->flags|=BT_FL_FLY;
	
	btSetModel(tmp, "models/objects/laser/laser0.ac");
	
	tmp->origin=org;
	tmp->angles=v3sub(btVecToAngles(dir), vec3(0,0,180));
	tmp->velocity=v3scale(dir, speed);
	tmp->damage=dmg;
	tmp->height=radius;
//	tmp->state=voxty;
	tmp->placer_vox=voxty;
	tmp->placer_aux=self->placer_aux;
	tmp->placer_value=self->placer_value;
	
	tmp->effects=BT_EFF_NOSHADOW|
		BT_EFF_SPARKSTRAIL|
//		BT_EFF_GLOW_RED|BT_EFF_GLOW_GREEN|
		// BT_EFF_GLOW_HIGH|
		BT_EFF_GLOW_BRIGHT1|BT_EFF_GLOW_BRIGHT0;
	
	switch(clr)
	{
	case 0: tmp->effects|=BT_EFF_GLOW_RED|BT_EFF_GLOW_GREEN; break;
	case 1: tmp->effects|=BT_EFF_GLOW_RED; break;
	case 2: tmp->effects|=BT_EFF_GLOW_GREEN; break;
	case 3: tmp->effects|=BT_EFF_GLOW_RED|BT_EFF_GLOW_GREEN; break;
	case 4: tmp->effects|=BT_EFF_GLOW_BLUE; break;
	case 5: tmp->effects|=BT_EFF_GLOW_RED|BT_EFF_GLOW_BLUE; break;
	case 6: tmp->effects|=BT_EFF_GLOW_GREEN|BT_EFF_GLOW_BLUE; break;
	case 7: tmp->effects|=BT_EFF_GLOW_WHITE; break;
	}
	
	tmp->vt->touch=BT_BlockPlacerTouch;
//	btSetThink(tmp, BT_RocketThink, 25);
	btSetThink(tmp, BT_PlacerThink, 5);
	
	return(tmp);
}

BTSV_API btEntity BT_FirePlasma(btEntity self,
	vec3d org, vec3 dir, float dmg, float speed, float radius, int clr)
{
	btEntity tmp;

	if((self->flags&BT_FL_MONSTER) && !(self->flags&BT_FL_AI_PASSIVE))
	{
		if(btGetSkill()==BT_SKILL_EASY)
			{ dmg*=0.5; }
		if((btGetSkill()==BT_SKILL_HARD) ||
			(btGetSkill()==BT_SKILL_NIGHTMARE))
			{ dmg*=4; }
	}

	tmp=btNewEntityTriggerInit(NULL, NULL);
//	tmp->solidtype=BT_SOLID_BBOX;
	tmp->solidtype=BT_SOLID_TRIGGER;
	tmp->movetype=BT_MOVE_FLYMISSILE;
	tmp->owner=self;
	
	tmp->mins=vec3(-4,-4,-4);
	tmp->maxs=vec3( 4, 4, 4);

	tmp->origin=org;
	tmp->angles=v3sub(btVecToAngles(dir), vec3(0,0,180));
	tmp->velocity=v3scale(dir, speed);
	tmp->damage=dmg;
	tmp->height=radius;

	tmp->effects=BT_EFF_NOSHADOW|
		BT_EFF_GLOW_BRIGHT1|BT_EFF_GLOW_BRIGHT0;
	
	switch(clr)
	{
	case 0:
	case 1:
		btSetModel(tmp, "s_plasma");	
		tmp->effects|=BT_EFF_GLOW_GREEN|BT_EFF_GLOW_BLUE;
		break;
	case 2:
		btSetModel(tmp, "s_plasma2");	
		tmp->effects|=BT_EFF_GLOW_RED|BT_EFF_GLOW_BLUE;
		break;
	case 3:
		btSetModel(tmp, "s_plasma3");	
		tmp->effects|=BT_EFF_GLOW_GREEN|BT_EFF_GLOW_RED;
		break;
	case 4:
		btSetModel(tmp, "s_plasma4");	
		tmp->effects|=BT_EFF_GLOW_GREEN|
			BT_EFF_GLOW_RED|BT_EFF_GLOW_BLUE;
		break;
	default:
		btSetModel(tmp, "s_plasma");	
		tmp->effects|=BT_EFF_GLOW_GREEN|BT_EFF_GLOW_BLUE;
		break;
	}
	
	tmp->vt->touch=BT_PlasmaTouch;
	btSetThink(tmp, BT_RocketThink, 25);
	
	return(tmp);
}

BTSV_API btEntity BT_FireBFG(btEntity self,
	vec3d org, vec3 dir, float dmg, float speed, float radius)
{
	btEntity tmp;

	if((self->flags&BT_FL_MONSTER) && !(self->flags&BT_FL_AI_PASSIVE))
	{
		if(btGetSkill()==BT_SKILL_EASY)
			{ dmg*=0.5; }
		if((btGetSkill()==BT_SKILL_HARD) ||
			(btGetSkill()==BT_SKILL_NIGHTMARE))
			{ dmg*=4; }
	}

	tmp=btNewEntityTriggerInit(NULL, NULL);
//	tmp->solidtype=BT_SOLID_BBOX;
	tmp->solidtype=BT_SOLID_TRIGGER;
	tmp->movetype=BT_MOVE_FLYMISSILE;
	tmp->owner=self;
	
	tmp->mins=vec3(-8,-8,-8);
	tmp->maxs=vec3( 8, 8, 8);
	
//	btSetModel(tmp, "models/objects/rocket/rocket0.ac");
//	btSetModel(tmp, "sprites/s_bfg1");
	btSetModel(tmp, "s_bfg1");
	
	tmp->origin=org;
	tmp->angles=v3sub(btVecToAngles(dir), vec3(0,0,180));
	tmp->velocity=v3scale(dir, speed);
	tmp->damage=dmg;
	tmp->height=radius;
	
	tmp->effects=BT_EFF_NOSHADOW|
		BT_EFF_GLOW_GREEN|
//		BT_EFF_GLOW_HIGH|
		BT_EFF_GLOW_BRIGHT1|BT_EFF_GLOW_BRIGHT0;
	
	tmp->vt->touch=BT_BFGTouch;
	btSetThink(tmp, BT_RocketThink, 25);
	
	return(tmp);
}


BTSV_API btEntity BT_FireLaser(btEntity self,
	vec3d org, vec3 dir, float dmg)
{
	BTGE_Trace tr;
	btEntity cur;
	btEntity tmp;

	if((self->flags&BT_FL_MONSTER) && !(self->flags&BT_FL_AI_PASSIVE))
	{
		if(btGetSkill()==BT_SKILL_EASY)
			{ dmg*=0.5; }
		if((btGetSkill()==BT_SKILL_HARD) ||
			(btGetSkill()==BT_SKILL_NIGHTMARE))
			{ dmg*=4; }
	}

	tmp=btNewEntityTriggerInit(NULL, NULL);
//	tmp->solidtype=BT_SOLID_BBOX;
//	tmp->solidtype=BT_SOLID_TRIGGER;
//	tmp->movetype=BT_MOVE_FLYMISSILE;
	tmp->owner=self;
	
	tmp->effects=BT_EFF_BEAM_LASER|BT_EFF_GLOW_RED|BT_EFF_ALPHA_50;

	tmp->origin=org;
	tmp->velocity=v3scale(dir, 1000);

	btSetThink(tmp, btDeleteEntity, 0.25);

//	BT_BulletMuzzleFlash(self, org, dir, BT_MOD_BULLET);

	btTraceline(&tr, self->morg, self->mend,
		BT_TRACE_MONSTERS|BT_TRACE_BSPSOLID, self);

	cur=tr.ent;
	while(cur)
	{
		BT_Damage(cur, self, dmg, BT_MOD_BULLET);
		cur=cur->chain;
	}

	return(tmp);
}
