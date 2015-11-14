#include <bgbgal.h>

bgalNeuronType *bgal_nn_type=NULL;

BGBDY_API bgalNeuronType *bgalLookupNeuronType(char *name)
{
	bgalNeuronType *cur;

	BGAL_InitNeuron();

	cur=bgal_nn_type;
	while(cur)
	{
		if(!strcmp(cur->name, name))
			return(cur);
		cur=cur->next;
	}
	return(NULL);
}

BGBDY_API bgalNeuronType *bgalGetNeuronType(char *name)
{
	bgalNeuronType *tmp;

	tmp=bgalLookupNeuronType(name);
	if(tmp)return(tmp);

	tmp=gctalloc("_bgal_neuron_type_t", sizeof(bgalNeuronType));
	tmp->name=dystrsym(name);
	tmp->next=bgal_nn_type;
	bgal_nn_type=tmp;

	return(tmp);
}

BGBDY_API bgalNeuron *bgalAllocNeuron(char *type)
{
	bgalNeuronType *ty;
	bgalNeuron *tmp;

	BGAL_InitNeuron();

	ty=bgalGetNeuronType(type);
	tmp=gctalloc("_bgal_neuron_t", sizeof(bgalNeuron));
	tmp->type=ty;

	if(ty->spawn)ty->spawn(tmp);

	return(tmp);
}

BGBDY_API bgalNeuronLink *bgalLookupNeuronLink(bgalNeuron *from, bgalNeuron *to)
{
	bgalNeuronLink *cur;

	cur=from->alist;
	while(cur)
	{
		if(cur->dnode==to)
			return(cur);
		cur=cur->anext;
	}
	return(NULL);
}

BGBDY_API bgalNeuronLink *bgalLinkNeurons(bgalNeuron *src, bgalNeuron *dst)
{
	bgalNeuronLink *tmp;

	tmp=bgalLookupNeuronLink(src, dst);
	if(tmp)return(tmp);

	tmp=gctalloc("_bgal_neuron_link_t", sizeof(bgalNeuronLink));
	tmp->anode=src; tmp->dnode=dst;
	tmp->anext=src->alist; tmp->dnext=dst->dlist;
	src->alist=tmp; src->dlist=tmp;

	if(src->type->notify_alist)
		src->type->notify_alist(src, tmp);
	if(dst->type->notify_dlist)
		dst->type->notify_dlist(dst, tmp);

	return(tmp);
}


BGBDY_API void bgalNeuronFire(bgalNeuron *node)
{
	static bgalNeuron *stack[256];
	static int stackpos=0;
	int i;

	if(!(node->type->fire))
		return;

	for(i=0; i<stackpos; i++)
		if(stack[i]==node)
			return;

	stack[stackpos++]=node;
	node->type->fire(node);
	stackpos--;
}

void bgal_nn_spawn_generic(bgalNeuron *self)
{
	self->limit=1;
}

void bgal_nn_fire_generic(bgalNeuron *self)
{
	bgalNeuronLink *cur;
	float f, g, tw;

	cur=self->alist; f=0;
	while(cur)
	{
		if(cur->dnode->type->pulse)
		{
			cur->dnode->type->pulse(cur->dnode,
				self, cur, self->energy);
			f+=cur->bpval;
			cur->bpval=0;
		}
		cur=cur->anext;
	}

	self->energy=0;

	cur=self->dlist; tw=0;
	while(cur) { tw+=fabs(cur->weight); cur=cur->dnext; }
	if(tw==0)tw=1;

	cur=self->dlist;
	while(cur)
	{
		g=f*(cur->weight/tw);
		g*=1.0/(1.0+log(cur->energy+1.0));

		cur->weight+=g;
		if(cur->weight<(-1))cur->weight=-1;
		if(cur->weight>1)cur->weight=1;

		cur->bpval+=g;
		cur=cur->dnext;
	}
}

void bgal_nn_pulse_generic(bgalNeuron *self, bgalNeuron *other,
	bgalNeuronLink *link, float value)
{
	float f;

	f=value*link->weight;
	link->energy+=fabs(f);
	self->energy+=f;

//	if(link->energy<0)link->energy=0;
//	if(self->energy<0)self->energy=0;

	if(self->energy>self->limit)
		bgalNeuronFire(self);
}

void BGAL_InitNeuron()
{
	static int init=0;
	bgalNeuronType *ty;

	if(init)return;
	init=1;

	ty=bgalGetNeuronType("generic");

	ty->spawn=bgal_nn_spawn_generic;
	ty->fire=bgal_nn_fire_generic;
	ty->pulse=bgal_nn_pulse_generic;
}

