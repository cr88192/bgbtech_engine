#include <bgbasm.h>
#include <bgbgc.h>
#include <bgbdy.h>

BGBDYC_MetaNode *dyll_metapath_root=NULL;

char *dyll_metapath_hash_name[4096];
BGBDYC_MetaNode *dyll_metapath_hash_node[4096];

char *dyll_metapath_hash2_name[4096];
BGBDYC_MetaNode *dyll_metapath_hash2_snode[4096];
BGBDYC_MetaNode *dyll_metapath_hash2_dnode[4096];

char *dyll_metapath_searchpath[1024];
int dyll_metapath_nsearchpath=0;

extern char *dyll_lmods[4096];		//loaded modules (trimmed name)
extern int dyll_nlmods;

#if 1
BGBDYC_MetaNode *DYLL_MetaPath_LookupNodeR2(
	BGBDYC_MetaBiNode *node, char *name)
{
	BGBDYC_MetaNode *tmp, *tmp1, *mid;
	BGBDYC_MetaBiNode *btmp, *btmp1;
	int i, j;

	mid=node->mid;
//	if(mid->name==name)
//		return(mid);

	i=strcmp(name, mid->name);
	if(i<0)
	{
		if(!node->lnode)
			return(NULL);

		tmp=DYLL_MetaPath_LookupNodeR2(node->lnode, name);
//		DYLL_MetaPath_BalanceBiNode(node);

		return(tmp);
	}else if(i>0)
	{
		if(!node->rnode)
			return(NULL);

		tmp=DYLL_MetaPath_LookupNodeR2(node->rnode, name);
//		DYLL_MetaPath_BalanceBiNode(node);

		return(tmp);
	}else
	{
		return(mid);
	}

	return(NULL);
}

BGBDYC_MetaNode *DYLL_MetaPath_LookupNodeR(
	BGBDYC_MetaNode *node, char *path)
{
	char tb[256];
	BGBDYC_MetaNode *cur, *prv, *tmp;
	BGBDYC_MetaBiNode *btmp;
	char *s, *t, *s1;
	int i;

	s=path; t=tb;
	while(*s=='/')s++;
	if(!*s)return(node);

	while(*s && (*s!='/'))*t++=*s++;
	*t++=0;

	s1=tb; i=0;
	while(*s1)i=(i*251)+(*s1++);
	i=((i*251)>>8)&4095;
//	i&=4095;

	if((dyll_metapath_hash2_snode[i]==node) &&
		dyll_metapath_hash2_name[i] &&
		!strcmp(dyll_metapath_hash2_name[i], tb))
	{
		cur=dyll_metapath_hash2_dnode[i];
		if(!cur)return(NULL);
		if(cur)return(DYLL_MetaPath_LookupNodeR(cur, s));
	}

	t=basm_strdup(tb);
	if(node->biroot)
		{ cur=DYLL_MetaPath_LookupNodeR2(node->biroot, t); }
	else
		{ cur=NULL; }

	dyll_metapath_hash2_name[i]=t;
	dyll_metapath_hash2_snode[i]=node;
	dyll_metapath_hash2_dnode[i]=cur;

	if(!cur)return(NULL);
	return(DYLL_MetaPath_LookupNodeR(cur, s));
}
#endif

#if 0
BGBDYC_MetaNode *DYLL_MetaPath_LookupNodeR(BGBDYC_MetaNode *node, char *path)
{
	char tb[256];
	BGBDYC_MetaNode *cur;
	char *s, *t, *s1;
	int i;

	s=path; t=tb;
	while(*s=='/')s++;
	if(!*s)return(node);

	while(*s && (*s!='/'))*t++=*s++;
	*t++=0;

	s1=tb; i=0;
	while(*s1)i=(i*251)+(*s1++);
	i=((i*251)>>8)&4095;
//	i&=4095;

	if((dyll_metapath_hash2_snode[i]==node) &&
		dyll_metapath_hash2_name[i] &&
		!strcmp(dyll_metapath_hash2_name[i], tb))
	{
		cur=dyll_metapath_hash2_dnode[i];
		if(!cur)return(NULL);
		return(DYLL_MetaPath_LookupNodeR(cur, s));
	}

	t=basm_strdup(tb);
	cur=node->first;
	while(cur)
	{
		if(cur->name==t)
			break;
		cur=cur->next;
	}

	dyll_metapath_hash2_name[i]=t;
	dyll_metapath_hash2_snode[i]=node;
	dyll_metapath_hash2_dnode[i]=cur;

	if(!cur)return(NULL);
	return(DYLL_MetaPath_LookupNodeR(cur, s));
}
#endif

#if 0
BGBDYC_MetaNode *DYLL_MetaPath_GetNodeR(BGBDYC_MetaNode *node, char *path)
{
	char tb[256];
	BGBDYC_MetaNode *cur, *prv, *tmp;
	char *s, *t, *s1;
	int i;

	s=path; t=tb;
	while(*s=='/')s++;
	if(!*s)return(node);

	while(*s && (*s!='/'))*t++=*s++;
	*t++=0;

	s1=tb; i=0;
	while(*s1)i=(i*251)+(*s1++);
	i=((i*251)>>8)&4095;
//	i&=4095;

	if((dyll_metapath_hash2_snode[i]==node) &&
		dyll_metapath_hash2_name[i] &&
		!strcmp(dyll_metapath_hash2_name[i], tb))
	{
		cur=dyll_metapath_hash2_dnode[i];
		if(cur)return(DYLL_MetaPath_GetNodeR(cur, s));
	}

	t=basm_strdup(tb);
	cur=node->first;
	while(cur)
	{
		if(cur->name==t)
			break;
		cur=cur->next;
	}

	if(!cur)
	{
		tmp=gctalloc("_dyc_metanode_t", sizeof(BGBDYC_MetaNode));
		tmp->name=t;

		if(node->first)
		{
			cur=node->first; prv=NULL;
			while(cur)
			{
				if(strcmp(cur->name, t)>=0)
					break;
				prv=cur; cur=cur->next;
			}
			if(prv)
			{
				tmp->next=cur;
				prv->next=tmp;
			}else
			{
				tmp->next=cur;
				node->first=tmp;
			}
		}else
		{
			tmp->next=node->first;
			node->first=tmp;
		}

		cur=tmp;
	}

	dyll_metapath_hash2_name[i]=t;
	dyll_metapath_hash2_snode[i]=node;
	dyll_metapath_hash2_dnode[i]=cur;

	return(DYLL_MetaPath_GetNodeR(cur, s));
}
#endif

#if 0
// BGBDYC_MetaNode *DYLL_MetaPath_GetNodeR2(BGBDYC_MetaNode *node, char *name)
{
	BGBDYC_MetaNode *tmp, *tmp1;
	int i, j;

	if(node->name==name)
		return(node);

	i=strcmp(name, node->name);
	if(i==0)
	{
		return(node);
	}else if(i<0)
	{
		if(!node->bilnode)
		{
			tmp=gctalloc("_dyc_metanode_t",
				sizeof(BGBDYC_MetaNode));
			tmp->name=name;
			tmp->owner=node->owner;

			if(node->prev)
			{
				node->prev->next=tmp;
				tmp->next=node;
				tmp->prev=node->prev;
				node->prev=tmp;
			}else
			{
				tmp->next=node;
				node->prev=tmp;
				node->owner->first=tmp;
			}

			node->bilnode=tmp;
			return(tmp);
		}

		tmp=DYLL_MetaPath_GetNodeR2(node->bilnode, name);

		i=node->bilnode->depth;
		j=node->bilnode?node->bilnode->depth:0;
		if(i>(j+1))
		{
		}

		return(tmp);
	}else
	{
		if(!node->birnode)
		{
			tmp=gctalloc("_dyc_metanode_t",
				sizeof(BGBDYC_MetaNode));
			tmp->name=name;
			tmp->owner=node->owner;

			if(node->next)
			{
				node->next->prev=tmp;
				tmp->prev=node;
				tmp->next=node->next;
				node->next=tmp;
			}else
			{
				tmp->prev=node;
				node->next=tmp;
			}

			node->birnode=tmp;
			return(tmp);
		}

		tmp=DYLL_MetaPath_GetNodeR2(node->birnode, name);
		return(tmp);
	}

	return(NULL);
}

BGBDYC_MetaNode *DYLL_MetaPath_GetNodeR(BGBDYC_MetaNode *node, char *path)
{
	char tb[256];
	BGBDYC_MetaNode *cur, *prv, *tmp;
	char *s, *t, *s1;
	int i;

	s=path; t=tb;
	while(*s=='/')s++;
	if(!*s)return(node);

	while(*s && (*s!='/'))*t++=*s++;
	*t++=0;

	s1=tb; i=0;
	while(*s1)i=(i*251)+(*s1++);
	i=((i*251)>>8)&4095;
//	i&=4095;

	if((dyll_metapath_hash2_snode[i]==node) &&
		dyll_metapath_hash2_name[i] &&
		!strcmp(dyll_metapath_hash2_name[i], tb))
	{
		cur=dyll_metapath_hash2_dnode[i];
		if(cur)return(DYLL_MetaPath_GetNodeR(cur, s));
	}

	t=basm_strdup(tb);
	if(node->biroot)
	{
		cur=DYLL_MetaPath_GetNodeR2(node->biroot, t);
	}else
	{
		cur=gctalloc("_dyc_metanode_t", sizeof(BGBDYC_MetaNode));
		cur->name=t;
		cur->owner=node;

		node->biroot=cur;
		node->first=cur;
	}

	dyll_metapath_hash2_name[i]=t;
	dyll_metapath_hash2_snode[i]=node;
	dyll_metapath_hash2_dnode[i]=cur;

	return(DYLL_MetaPath_GetNodeR(cur, s));
}
#endif


#if 1
int DYLL_MetaPath_AdjustBiDepth(BGBDYC_MetaBiNode *node)
{
	int i, j;

	i=node->lnode?node->lnode->depth:-1;
	j=node->rnode?node->rnode->depth:-1;
	node->depth=((i>j)?i:j)+1;
	return(node->depth);
}

int DYLL_MetaPath_BalanceBiNode(BGBDYC_MetaBiNode *node)
{
	BGBDYC_MetaNode *tmp;
	BGBDYC_MetaBiNode *btmp;
	int i, j;

	i=node->lnode?node->lnode->depth:-1;
	j=node->rnode?node->rnode->depth:-1;
	node->depth=((i>j)?i:j)+1;

	if(i>(j+1))
	{
		btmp=node->lnode;
		tmp=node->mid;
		node->lnode=btmp->lnode;
		node->mid=btmp->mid;
		btmp->lnode=btmp->rnode;
		btmp->rnode=node->rnode;
		btmp->mid=tmp;
		node->rnode=btmp;

		DYLL_MetaPath_AdjustBiDepth(btmp);
		DYLL_MetaPath_AdjustBiDepth(node);
	}

	if(j>(i+1))
	{
		btmp=node->rnode;
		tmp=node->mid;
		node->rnode=btmp->rnode;
		node->mid=btmp->mid;
		btmp->rnode=btmp->lnode;
		btmp->lnode=node->lnode;
		btmp->mid=tmp;
		node->lnode=btmp;

		DYLL_MetaPath_AdjustBiDepth(btmp);
		DYLL_MetaPath_AdjustBiDepth(node);
	}
	return(0);
}

BGBDYC_MetaNode *DYLL_MetaPath_GetNodeR2(BGBDYC_MetaBiNode *node, char *name)
{
	BGBDYC_MetaNode *tmp, *tmp1, *mid;
	BGBDYC_MetaBiNode *btmp, *btmp1;
	int i, j;

	mid=node->mid;
//	if(mid->name==name)
//		return(mid);

	i=strcmp(name, mid->name);
	if(i<0)
	{
		if(!node->lnode)
		{
			tmp=gctalloc("_dyc_metanode_t",
				sizeof(BGBDYC_MetaNode));
			tmp->name=name;
			tmp->owner=mid->owner;

			if(mid->prev)
			{
				mid->prev->next=tmp;
				tmp->next=mid;
				tmp->prev=mid->prev;
				mid->prev=tmp;
			}else
			{
				tmp->next=mid;
				mid->prev=tmp;
				mid->owner->first=tmp;
			}

			btmp=gctalloc("_dyc_metabinode_t",
				sizeof(BGBDYC_MetaBiNode));
			btmp->mid=tmp;
			node->lnode=btmp;

			return(tmp);
		}

		tmp=DYLL_MetaPath_GetNodeR2(node->lnode, name);
		DYLL_MetaPath_BalanceBiNode(node);

		return(tmp);
	}else if(i>0)
	{
		if(!node->rnode)
		{
			tmp=gctalloc("_dyc_metanode_t",
				sizeof(BGBDYC_MetaNode));
			tmp->name=name;
			tmp->owner=mid->owner;

			if(mid->next)
			{
				mid->next->prev=tmp;
				tmp->prev=mid;
				tmp->next=mid->next;
				mid->next=tmp;
			}else
			{
				tmp->prev=mid;
				mid->next=tmp;
			}

			btmp=gctalloc("_dyc_metabinode_t",
				sizeof(BGBDYC_MetaBiNode));
			btmp->mid=tmp;
			node->rnode=btmp;

			return(tmp);
		}

		tmp=DYLL_MetaPath_GetNodeR2(node->rnode, name);
		DYLL_MetaPath_BalanceBiNode(node);

		return(tmp);
	}else
	{
		return(mid);
	}

	return(NULL);
}

BGBDYC_MetaNode *DYLL_MetaPath_GetNodeR(BGBDYC_MetaNode *node, char *path)
{
	char tb[256];
	BGBDYC_MetaNode *cur, *prv, *tmp;
	BGBDYC_MetaBiNode *btmp;
	char *s, *t, *s1;
	int i;

	s=path; t=tb;
	while(*s=='/')s++;
	if(!*s)return(node);

	while(*s && (*s!='/'))*t++=*s++;
	*t++=0;

	s1=tb; i=0;
	while(*s1)i=(i*251)+(*s1++);
	i=((i*251)>>8)&4095;
//	i&=4095;

	if((dyll_metapath_hash2_snode[i]==node) &&
		dyll_metapath_hash2_name[i] &&
		!strcmp(dyll_metapath_hash2_name[i], tb))
	{
		cur=dyll_metapath_hash2_dnode[i];
		if(cur)return(DYLL_MetaPath_GetNodeR(cur, s));
	}

	t=basm_strdup(tb);
	if(node->biroot)
	{
		cur=DYLL_MetaPath_GetNodeR2(node->biroot, t);
	}else
	{
		cur=gctalloc("_dyc_metanode_t", sizeof(BGBDYC_MetaNode));
		cur->name=t;
		cur->owner=node;
		node->first=cur;

		btmp=gctalloc("_dyc_metabinode_t",
			sizeof(BGBDYC_MetaBiNode));
		btmp->mid=cur;

		node->biroot=btmp;
	}

	dyll_metapath_hash2_name[i]=t;
	dyll_metapath_hash2_snode[i]=node;
	dyll_metapath_hash2_dnode[i]=cur;

	return(DYLL_MetaPath_GetNodeR(cur, s));
}
#endif



BGBDYC_MetaNode *DYLL_MetaPath_LookupNode(char *path)
{
	BGBDYC_MetaNode *tmp;
	char *s;
	int i;

	s=path; i=0;
	while(*s)i=(i*251)+(*s++);
	i=((i*251)>>8)&4095;
//	i&=4095;

	if(dyll_metapath_hash_name[i] &&
		!strcmp(dyll_metapath_hash_name[i], path))
			return(dyll_metapath_hash_node[i]);

	if(!dyll_metapath_root)return(NULL);
	tmp=DYLL_MetaPath_LookupNodeR(dyll_metapath_root, path);

//	if(dyll_metapath_hash_name[i])
//		free(dyll_metapath_hash_name[i]);
//	dyll_metapath_hash_name[i]=strdup(path);
	dyll_metapath_hash_name[i]=dyllStrdup(path);
	dyll_metapath_hash_node[i]=tmp;

	return(tmp);
}

BGBDYC_MetaNode *DYLL_MetaPath_GetNode(char *path)
{
	BGBDYC_MetaNode *tmp;
	char *s;
	int i;

	if(!dyll_metapath_root)
	{
		tmp=gctalloc("_dyc_metanode_t", sizeof(BGBDYC_MetaNode));
		dyll_metapath_root=tmp;
	}

	s=path; i=0;
	while(*s)i=(i*251)+(*s++);
	i=((i*251)>>8)&4095;
//	i&=4095;

	if(dyll_metapath_hash_name[i] &&
		!strcmp(dyll_metapath_hash_name[i], path))
	{
		tmp=dyll_metapath_hash_node[i];
		if(tmp)return(tmp);
	}

	tmp=DYLL_MetaPath_GetNodeR(dyll_metapath_root, path);

//	if(dyll_metapath_hash_name[i])
//		free(dyll_metapath_hash_name[i]);
//	dyll_metapath_hash_name[i]=strdup(path);
	dyll_metapath_hash_name[i]=dyllStrdup(path);
	dyll_metapath_hash_node[i]=tmp;

	return(tmp);
}

BGBDYC_MetaLeaf *DYLL_MetaPath_LookupNodeLeaf(BGBDYC_MetaNode *node, char *key)
{
	BGBDYC_MetaLeaf *cur;
	char *s, *t;

	if(!key || !(*key))key="_";

	t=basm_strdup(key);
	cur=node->leaf;
	while(cur)
	{
		if(cur->key==t)
			break;
		cur=cur->next;
	}

	if(!cur)return(NULL);
	return(cur);
}

BGBDYC_MetaLeaf *DYLL_MetaPath_GetNodeLeaf(BGBDYC_MetaNode *node, char *key)
{
	BGBDYC_MetaLeaf *cur, *prv, *tmp;
	char *s, *t;

	if(!key || !(*key))key="_";

	t=basm_strdup(key);
	cur=node->leaf;
	while(cur)
	{
		if(cur->key==t)
			break;
		cur=cur->next;
	}

	if(!cur)
	{
		tmp=gctalloc("_dyc_metaleaf_t", sizeof(BGBDYC_MetaLeaf));
		tmp->key=t;

		if(node->leaf)
		{
			cur=node->leaf; prv=NULL;
			while(cur)
			{
				if(strcmp(cur->key, t)>=0)
					break;
				prv=cur; cur=cur->next;
			}
			if(prv)
			{
				tmp->next=cur;
				prv->next=tmp;
			}else
			{
				tmp->next=cur;
				node->leaf=tmp;
			}
		}else
		{
			tmp->next=node->leaf;
			node->leaf=tmp;
		}

		cur=tmp;
	}

	return(cur);
}

char *DYLL_MetaPath_GetNodePathR(BGBDYC_MetaNode *node, char *t)
{
	if(node->owner && node->owner->name)
	{
		t=DYLL_MetaPath_GetNodePathR(node->owner, t);
		*t++='/';
	}
	
	strcpy(t, node->name);
	t+=strlen(t);
	return(t);
}

char *DYLL_MetaPath_GetNodePath(BGBDYC_MetaNode *node)
{
	char tb[1024];
	DYLL_MetaPath_GetNodePathR(node, tb);
	return(basm_strdup(tb));
}

void DYLL_MetaPath_SplitKey(char *path, char *nb, char *kb)
{
	char *s, *t;
	int i;

	s=path;

	t=nb;
	while(*s && (*s!=':'))
		*t++=*s++;
	*t++=0;
	if(*s==':')s++;

	t=kb;
	while(*s)
		*t++=*s++;
	*t++=0;
}

char *DYLL_MetaPath_LookupKey(char *path)
{
	char nb[1024], kb[256], kb2[1024];
	BGBDYC_MetaNode *node, *ncur;
	BGBDYC_MetaLeaf *leaf;
	char **a;
	char *s;
	int i, j;

	if(*path=='$')
	{
		a=gcrsplit(path);

		if(!strcmp(a[0], "$next"))
		{
			DYLL_MetaPath_SplitKey(a[1], nb, kb);

			node=DYLL_MetaPath_LookupNode(nb);
			if(!node)return(UNDEFINED);
			
			if(!kb[0])
			{
				if(!node->next)return(NULL);
				s=DYLL_MetaPath_GetNodePath(node->next);
				return(s);
			}
			
			leaf=DYLL_MetaPath_LookupNodeLeaf(node, kb);
			if(!leaf)return(UNDEFINED);
			if(!leaf->next)return(NULL);
			return(leaf->next->key);
		}

		if(!strcmp(a[0], "$prev"))
		{
			DYLL_MetaPath_SplitKey(a[1], nb, kb);
			node=DYLL_MetaPath_LookupNode(nb);
			if(!node)return(UNDEFINED);
			if(kb[0])return(UNDEFINED);
			if(!node->prev)return(NULL);
			return(DYLL_MetaPath_GetNodePath(node->prev));
		}

		if(!strcmp(a[0], "$child"))
		{
			DYLL_MetaPath_SplitKey(a[1], nb, kb);
			node=DYLL_MetaPath_LookupNode(nb);
			if(!node)return(UNDEFINED);
			if(kb[0])return(UNDEFINED);
			if(!node->first)return(NULL);
			return(DYLL_MetaPath_GetNodePath(node->first));
		}

		if(!strcmp(a[0], "$parent"))
		{
			DYLL_MetaPath_SplitKey(a[1], nb, kb);
			node=DYLL_MetaPath_LookupNode(nb);
			if(!node)return(UNDEFINED);
			if(kb[0])return(UNDEFINED);
			if(!node->owner)return(NULL);
			return(DYLL_MetaPath_GetNodePath(node->owner));
		}

		if(!strcmp(a[0], "$leaf"))
		{
			DYLL_MetaPath_SplitKey(a[1], nb, kb);
			node=DYLL_MetaPath_LookupNode(nb);
			if(!node)return(UNDEFINED);
			if(kb[0])return(UNDEFINED);
			if(!node->leaf)return(NULL);
			s=DYLL_MetaPath_GetNodePath(node->next);
			return(node->leaf->key);
		}

		if(!strcmp(a[0], "$find_first"))
		{
			DYLL_MetaPath_SplitKey(a[1], nb, kb);

			node=DYLL_MetaPath_LookupNode(nb);
			if(!node)return(UNDEFINED);
			ncur=node->first;
			while(ncur)
			{
				leaf=DYLL_MetaPath_LookupNodeLeaf(ncur, kb);
				if(!leaf || !leaf->val)
					{ ncur=ncur->next; continue; }
				if(!strcmp(leaf->val, a[2]))
					return(DYLL_MetaPath_GetNodePath(ncur));
				ncur=ncur->next;
			}
			return(NULL);
		}

		if(!strcmp(a[0], "$find_next"))
		{
			DYLL_MetaPath_SplitKey(a[1], nb, kb);

			node=DYLL_MetaPath_LookupNode(nb);
			if(!node)return(UNDEFINED);
			ncur=node->next;
			while(ncur)
			{
				leaf=DYLL_MetaPath_LookupNodeLeaf(ncur, kb);
				if(!leaf || !leaf->val)
					{ ncur=ncur->next; continue; }
				if(!strcmp(leaf->val, a[2]))
					return(DYLL_MetaPath_GetNodePath(ncur));
				ncur=ncur->next;
			}
			return(NULL);
		}

		return(UNDEFINED);
	}

	DYLL_MetaPath_SplitKey(path, nb, kb);

	node=DYLL_MetaPath_LookupNode(nb);
	if(!node)return(UNDEFINED);
	leaf=DYLL_MetaPath_LookupNodeLeaf(node, kb);
	if(!leaf)return(UNDEFINED);
	return(leaf->val);
}

char *DYLL_MetaPath_LookupKey2(char *path)
{
	char *s;
	s=DYLL_MetaPath_LookupKey(path);
	if(s==UNDEFINED)s=NULL;
	return(s);
}

void DYLL_MetaPath_BindKey(char *path, char *val)
{
	char nb[1024], kb[256];
	BGBDYC_MetaNode *node;
	BGBDYC_MetaLeaf *leaf;
	int i, j;

	DYLL_MetaPath_SplitKey(path, nb, kb);

	node=DYLL_MetaPath_GetNode(nb);
	if(!node)return;
	leaf=DYLL_MetaPath_GetNodeLeaf(node, kb);
	if(!leaf)return;
	leaf->val=basm_strdup(val);
}

void DYLL_MetaPath_BindKeyAlt(char *path, char *key, char *val)
{
//	char nb[1024], kb[256];
	BGBDYC_MetaNode *node;
	BGBDYC_MetaLeaf *leaf;
	int i, j;

//	DYLL_MetaPath_SplitKey(path, nb, kb);

	node=DYLL_MetaPath_GetNode(path);
	if(!node)return;
	leaf=DYLL_MetaPath_GetNodeLeaf(node, key);
	if(!leaf)return;
	leaf->val=basm_strdup(val);
}

int DYLL_MetaPath_LoadDB(char *name)
{
	return(DYLL_MetaPath_LoadDB2(name, NULL));
}

int DYLL_MetaPath_LoadDB2(char *name, char *base)
{
	char tb[1024], nb[1024], kb[256];
	VFILE *fd;
	char **a;
	char *s, *t;
	int i, nk, nl;

	printf("DYLL_MetaPath_LoadDB2: %s %s\n", name, base);

	sprintf(tb, ".db/loaded/%s", name);
	s=DYLL_MetaPath_LookupKey2(tb);
	if(s && !strcmp(s, "true"))
		return(1);

	fd=vffopen(name, "rb");
//	if(!fd)return(-1);

	if(!fd)
	{
		for(i=0; i<dyll_metapath_nsearchpath; i++)
		{
			sprintf(tb, "%s/%s", dyll_metapath_searchpath[i], name);
			fd=vffopen(tb, "rb");
			if(fd)break;
		}
	}

	if(!fd)return(-1);

	nb[0]=0; nk=0; nl=0;

	while(!vfeof(fd))
	{
		vfgets(tb, 1023, fd);
		nl++;

		s=tb;
		while(*s && (*s<=' '))s++;
		if(!(*s))continue;
		if((*s)==';')continue;
		if((*s)=='#')continue;
		if((*s)=='/')continue;

		if((*s)=='[')
		{
			s++;
			t=nb;
			if(base)
				{ strcpy(t, base); t+=strlen(t); *t++='/'; }
			while(*s && (*s!=']'))*t++=*s++;
			*t++=0;
			nk++;
			continue;
		}

		t=kb;
		while(*s && (*s!='='))*t++=*s++;
		*t++=0;
		if(*s=='=')s++;

		s=basm_strdup(s);
//		sprintf(tb, "%s:%s", nb, kb);

//		printf("DYLL_MetaPath_LoadAll: %s=%s\n", tb, s);

//		DYLL_MetaPath_BindKey(tb, s);
		DYLL_MetaPath_BindKeyAlt(nb, kb, s);
	}
	vfclose(fd);

	sprintf(tb, ".db/loaded/%s", name);
	DYLL_MetaPath_BindKey(tb, "true");

	printf("DYLL_MetaPath_LoadDB2: %s %s (%d keys, %d lines)\n",
		name, base, nk, nl);

	return(0);
}

int DYLL_MetaPath_LoadBufferDB(char *name, char *base,
	char *buf, int szbuf)
{
	char tb[1024], nb[1024], kb[256];
	VFILE *fd;
	char **a;
	char *s, *t, *te, *cs, *cse;
	int i;

	printf("DYLL_MetaPath_LoadBufferDB: %s %s\n", name, base);

	if(name)
	{
		sprintf(tb, ".db/loaded/%s", name);
		s=DYLL_MetaPath_LookupKey2(tb);
		if(s && !strcmp(s, "true"))
			return(1);
	}

//	fd=vffopen(name, "rb");
//	if(!fd)return(-1);

	if(!buf)return(-1);
	cs=buf; cse=buf+szbuf; nb[0]=0;
	while(*cs && (cs<cse))
	{
//		vfgets(tb, 1023, fd);

		t=tb; te=tb+1023;
		while(*cs && (t<te))
		{
			if((*cs=='\r') || (*cs=='\n'))
			{
				if(*cs=='\r')cs++;
				if(*cs=='\n')cs++;
				break;
			}
			*t++=*cs++;
		}
		*t++=0;

		s=tb;
		while(*s && (*s<=' '))s++;
		if(!(*s))continue;
		if((*s)==';')continue;
		if((*s)=='#')continue;
		if((*s)=='/')continue;

		if((*s)=='[')
		{
			s++;
			t=nb;
			if(base)
				{ strcpy(t, base); t+=strlen(t); *t++='/'; }
			while(*s && (*s!=']'))*t++=*s++;
			*t++=0;
			continue;
		}

		t=kb;
		while(*s && (*s!='='))*t++=*s++;
		*t++=0;
		if(*s=='=')s++;

		s=basm_strdup(s);
//		sprintf(tb, "%s:%s", nb, kb);

//		printf("DYLL_MetaPath_LoadAll: %s=%s\n", tb, s);

//		DYLL_MetaPath_BindKey(tb, s);
		DYLL_MetaPath_BindKeyAlt(nb, kb, s);
	}
//	vfclose(fd);

	if(name)
	{
		sprintf(tb, ".db/loaded/%s", name);
		DYLL_MetaPath_BindKey(tb, "true");
	}

	return(0);
}

void DYLL_MetaPath_LoadAll()
{
	DYLL_MetaPath_LoadDB("cc_usr/metapath.txt");
}

void DYLL_MetaPath_SaveNode(VFILE *fd,
	BGBDYC_MetaNode *node, char *path, int ind)
{
	char tb[256];
	BGBDYC_MetaLeaf *lcur;
	BGBDYC_MetaNode *ncur;
	char *s0;
	int i;

	if(node->leaf)
	{
		for(i=0; i<ind; i++)
			vfprint(fd, "  ");

		vfprint(fd, "[%s]\n", path);
		lcur=node->leaf;

		while(lcur)
		{
			if(!lcur->val || (lcur->val==UNDEFINED))
			{
				lcur=lcur->next;
				continue;
			}

			for(i=0; i<ind; i++)
				vfprint(fd, "  ");
			vfprint(fd, "  %s=%s\n", lcur->key, lcur->val);
			lcur=lcur->next;
		}
	}

	ncur=node->first;
	while(ncur)
	{
		if(!ncur->name || (ncur->name[0]=='.'))
			{ ncur=ncur->next; continue; }
		if(*path)sprintf(tb, "%s/%s", path, ncur->name);
			else strcpy(tb, ncur->name);
		DYLL_MetaPath_SaveNode(fd, ncur, tb, ind+1);
		ncur=ncur->next;
	}
}

void DYLL_MetaPath_SaveAll()
{
	VFILE *fd;
	int i;

	if(!dyll_metapath_root)return;

	fd=vffopen("cc_usr/metapath.txt", "wt");
	if(!fd)return;
	DYLL_MetaPath_SaveNode(fd, dyll_metapath_root, "", -1);
	vfclose(fd);
}

int DYLL_MetaPath_SaveDB(char *name)
{
	VFILE *fd;
	int i;

	if(!dyll_metapath_root)return(-1);

	fd=vffopen(name, "wt");
	if(!fd)return(-1);
	DYLL_MetaPath_SaveNode(fd, dyll_metapath_root, "", -1);
	vfclose(fd);

	return(0);
}

int DYLL_MetaPath_AddSearchPath(char *name)
{
	int i;

	if(!name)return(-1);

	for(i=0; i<dyll_metapath_nsearchpath; i++)
		if(!strcmp(dyll_metapath_searchpath[i], name))
			return(i);
	
	i=dyll_metapath_nsearchpath++;
	dyll_metapath_searchpath[i]=basm_strdup(name);
	return(i);
}

int DYLL_MetaPath_ParseSearchPath(char *path)
{
	char tb[1024];
	char *s, *t;
	
	s=path;
	while(*s)
	{
		while(*s && (*s<' '))s++;
		t=tb;
		while(*s)
		{
#ifdef WIN32
			if(*s==';')break;
#else
			if(*s==':')break;
#endif
			*t++=*s++;
		}
		*t++=0;

#ifdef WIN32
		if(*s==';')s++;
#else
		if(*s==':')s++;
#endif
		while(*s && (*s<' '))s++;
		
		if(tb[0])DYLL_MetaPath_AddSearchPath(tb);
	}
	return(0);
}

int DYLL_MetaPath_ParseEnvString(char *str)
{
	char nb[4096], vb[16384], tb[4096];
	char *s, *t;
	
	s=str; t=nb;
	while(*s && (*s!='='))*t++=*s++;
	*t++=0;
	if(*s!='=')return(-1);
	s++;
	
	t=vb;
	while(*s)*t++=*s++;
	*t++=0;

	sprintf(tb, ".env/%s", nb);
	DYLL_MetaPath_BindKey(tb, vb);
	return(0);
}


BGBDY_API void dyllMetaInit()
{
	static int init=0;
	char tb[256];
//	VFILE *fd;
	char **a;
	char *s;
	int i;

	if(init)return;
	init=1;

#ifdef WIN32
	a=BLNK_GetEnvironOS();
	for(i=0; a[i]; i++)
		DYLL_MetaPath_ParseEnvString(a[i]);
#endif

	sprintf(tb, ".env/PATH");
	s=DYLL_MetaPath_LookupKey2(tb);
	if(s)DYLL_MetaPath_ParseSearchPath(s);

	DYLL_MetaPath_LoadAll();
}

BGBDY_API void dyllMetaCommit()
{
	DYLL_MetaPath_SaveAll();
}

BGBDY_API int dyllMetaLoadDB(char *name)
{
	return(DYLL_MetaPath_LoadDB(name));
}

BGBDY_API int dyllMetaSaveDB(char *name)
{
	return(DYLL_MetaPath_SaveDB(name));
}

BGBDY_API int dyllMetaLoadDB2(char *name, char *base)
{
	return(DYLL_MetaPath_LoadDB2(name, base));
}

BGBDY_API int dyllMetaLoadBufferDB(char *name, char *base,
	char *buf, int szbuf)
{
	return(DYLL_MetaPath_LoadBufferDB(name, base, buf, szbuf));
}


BGBDY_API void dyllMetaBindKey(char *key, char *val)
{
	dyllMetaInit();
	DYLL_MetaPath_BindKey(key, val);
}

BGBDY_API char *dyllMetaLookupKey(char *key)
{
	char tb[1024];
	char *s;
	int i;

	dyllMetaInit();
	
	if(*key=='/')
	{
		s=DYLL_MetaPath_LookupKey2(key+1);
		return(s);
	}
	
	s=DYLL_MetaPath_LookupKey2(key);
	if(s)return(s);

	for(i=0; i<dyll_nlmods; i++)
	{
		sprintf(tb, "%s/%s", dyll_lmods[i], key);
		s=DYLL_MetaPath_LookupKey2(tb);
		if(s)return(s);
	}

//	if(!s)s=DYLL_MetaRDB_LookupCacheKey(key);
	if(s)return(s);

	return(NULL);
}
