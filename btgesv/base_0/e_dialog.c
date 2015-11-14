#include <btgesv.h>

BTGE_DialogNode bt_dlgtst_main=
{
"main",
"Main Menu Test",
"images/faces/face0_creepy_mad",
{"Menu A", "Menu B", "Menu C", NULL},
{"menu_a", "menu_b", "menu_c", NULL}
};

BTGE_DialogNode bt_dlgtst_menu_a=
{
"menu_a",
"Menu A Test",
"images/faces/face0_creepy_mad",
{"Menu A", "Menu B", "Menu C", "Main", NULL},
{"menu_a", "menu_b", "menu_c", "main", NULL}
};

BTGE_DialogNode bt_dlgtst_menu_b=
{
"menu_b",
"Menu B Test",
"images/faces/face0_creepy_mad",
{"Menu A", "Menu B", "Menu C", "Main", NULL},
{"menu_a", "menu_b", "menu_c", "main", NULL}
};

BTGE_DialogNode bt_dlgtst_menu_c=
{
"menu_c",
"Menu C Test",
"images/faces/face0_creepy_mad",
{"Menu A", "Menu B", "Menu C", "Main", NULL},
{"menu_a", "menu_b", "menu_c", "main", NULL}
};

BTGE_DialogNode *bt_dlgtst_arr[]=
{
	&bt_dlgtst_main,
	&bt_dlgtst_menu_a,
	&bt_dlgtst_menu_b,
	&bt_dlgtst_menu_c,
	NULL
};

BTGE_DialogInfo bt_dlgtst_info=
{
	bt_dlgtst_arr,
	4, 0
};


BTSV_API BTGE_DialogInfo *BT_AllocDialogInfo()
{
	BTGE_DialogInfo *tmp;
	tmp=gctalloc("btge_dialoginfo_t", sizeof(BTGE_DialogInfo));
	return(tmp);
}

BTSV_API BTGE_DialogNode *BT_AllocDialogNode()
{
	BTGE_DialogNode *tmp;
	tmp=gctalloc("btge_dialognode_t", sizeof(BTGE_DialogNode));
	return(tmp);
}

BTSV_API BTGE_DialogNode *btNewDialogNode(
	char *name, char *msg, char *img)
{
	BTGE_DialogNode *tmp;
	tmp=BT_AllocDialogNode();
	tmp->name=dystrdup(name);
	tmp->message=dystrdup(msg);
	tmp->image=dystrdup(img);
	return(tmp);
}

BTSV_API void btSetDialogNodeOption(
	BTGE_DialogNode *node, int i, char *msg, char *tgt)
{
	node->opt_msg[i-1]=dystrdup(msg);
	node->opt_tgt[i-1]=dystrdup(tgt);
}

BTSV_API void btAddDialogInfoNode(
	BTGE_DialogInfo *info, BTGE_DialogNode *node)
{
	BTGE_DialogNode **ona;
	int i, j;

	if(!info->node)
	{
		info->node=gcalloc(16*sizeof(BTGE_DialogNode *));
		info->n_node=0;
		info->m_node=16;
	}
	
	if(info->m_node<=0)
	{
		ona=info->node;
		for(j=0; ona[j]; j++);
		i=16;
		while(i<=j)i=i+(i>>1);
		info->node=gcalloc(i*sizeof(BTGE_DialogNode *));
		info->n_node=j; info->m_node=i;
		for(i=j; ona[j]; j++)
			info->node[j]=ona[j];
	}
	
	if((info->n_node+1)>=info->m_node)
	{
		i=info->m_node; i=i+(i>>1);
		info->node=gcrealloc(info->node, i*sizeof(BTGE_DialogNode *));
		info->m_node=i;
	}
	
	i=info->n_node++;
	info->node[i]=node;
}


BTSV_API void BT_EntityDialogSetupTest(btEntity self)
{
	self->dlg_info=&bt_dlgtst_info;
}

BTSV_API int BT_PlayerDialogImpulse(btEntity self, int flags)
{
	char tbuf[1024];
	char tb[256];
	char *s, *t;
	int i;

	//force leave box
	if(flags&BT_IMPULSE_USE)
	{
//		BT_PlayerImpulseUse(self);
		btCenterprintBox(self->dlg->client, "");
		gcfree(self->dlg);
		self->dlg=NULL;
		return(0);
	}

	if(flags)
	{
		self->dlg->impulse=flags;
		btEntUse(self->dlg->npc, self);
	}

	if(self->dlg->msg)
	{
		sprintf(tbuf, "%s\n\n", self->dlg->msg);
		t=tbuf+strlen(tbuf);
		for(i=0; i<10; i++)
		{
			if(!self->dlg->opt_msg[i])break;
			sprintf(t, "%d: %s\n", i+1, self->dlg->opt_msg[i]);
			t+=strlen(t);
		}

		btCenterprintBoxName(self->dlg->client,
			"%s", self->dlg->cur_name);
		btCenterprintBoxImage(self->dlg->client,
			"%s", self->dlg->cur_img);
	
		btCenterprintBox(self->dlg->client, "%s", tbuf);
//		btCenterprintBox(self->dlg->client, "%s", self->dlg->msg);
	}else
	{
		btCenterprintBox(self->dlg->client, "");
		gcfree(self->dlg);
		self->dlg=NULL;
	}
	return(0);
}


BTSV_API int BT_DialogLookupInfoNode(BTGE_DialogInfo *info, char *name)
{
	int i;
	
	for(i=0; info->node[i]; i++)
	{
		if(!strcmp(info->node[i]->name, name))
			return(i);
	}
	return(-1);
}

BTSV_API int BT_DialogSetupForNode(
	btEntity self, BTGE_DialogInfo *info, char *name)
{
	BTGE_DialogNode *node;
	int i, j, k;

	//lookup main node
	i=BT_DialogLookupInfoNode(info, name);
	if(i<0)
	{
		if(self->dlg)
			self->dlg->msg=NULL;
		return(0);
	}
	
	node=info->node[i];
	
	//create info if doesn't exist
	if(!self->dlg)
		self->dlg=gctalloc("btge_entitydialog_t", sizeof(BTGE_EntityDialog));
	
	self->dlg->info=info;
	self->dlg->state=i;
	self->dlg->msg=node->message;
	self->dlg->cur_img=node->image;

	if(self->dlg->npc)
		self->dlg->cur_name=self->dlg->npc->classname;
	
	for(j=0; j<10; j++)
	{
		self->dlg->opt_msg[j]=node->opt_msg[j];
		self->dlg->opt_tgt[j]=node->opt_tgt[j];
	}
	
	if(node->fcn_f)
		{ node->fcn_f(self, self->dlg); }
	if(node->fcn_dy)
	{
		dyApplyMethod2(self->dy_this, node->fcn_dy,
			(dyt)self, (dyt)self->dlg);
	}
	
	return(0);
}

BTSV_API void BT_EntityDialogUse(btEntity self, btEntity other)
{
	char *s;
	int i, j, k;

	//other is involved in dialogue
	if(other->dlg)
	{
		i=-1;
		if(other->dlg->impulse&BT_IMPULSE_1)i=0;
		if(other->dlg->impulse&BT_IMPULSE_2)i=1;
		if(other->dlg->impulse&BT_IMPULSE_3)i=2;
		if(other->dlg->impulse&BT_IMPULSE_4)i=3;
		if(other->dlg->impulse&BT_IMPULSE_5)i=4;
		if(other->dlg->impulse&BT_IMPULSE_6)i=5;
		if(other->dlg->impulse&BT_IMPULSE_7)i=6;
		if(other->dlg->impulse&BT_IMPULSE_8)i=7;
		if(other->dlg->impulse&BT_IMPULSE_9)i=8;
		if(other->dlg->impulse&BT_IMPULSE_0)i=9;

		if(i<0)return;

		s=other->dlg->opt_tgt[i];
		if(s)
		{
			BT_DialogSetupForNode(other, other->dlg->info, s);
			BT_PlayerDialogImpulse(other, 0);
		}

		return;
	}

	//see if we don't have any dialogue
	if(!self->dlg_info)
		return;
	
	if(!(other->impulseFlags&BT_IMPULSE_USE))
		return;
	
	//set up for main node
	BT_DialogSetupForNode(other, self->dlg_info, "main");
	other->dlg->client=other;
	other->dlg->npc=self;
	BT_PlayerDialogImpulse(other, 0);
}
