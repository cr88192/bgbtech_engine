#include <lbxgl2.h>

//extern float pdgl_dt_f;

//extern float cam_rt[3];
//extern float cam_fw[3];
//extern float cam_up[3];

extern PDGLUI_InputState *lbxgl_state;
extern PDGLUI_Camera *lbxgl_cam;


LBXGL_ParticleInfo *lbxgl_part_type[256];
char lbxgl_part_ntypes=0;

LBXGL_Particle *lbxgl_part_free=NULL;
LBXGL_Particle *lbxgl_part_root=NULL;
int lbxgl_part_cnt, lbxgl_part_max;

LBXGL_ParticleNode *lbxgl_part_noderoot=NULL;
LBXGL_ParticleNode *lbxgl_part_nodefree=NULL;

int lbxgl_part_flame;
int lbxgl_part_flame_green;
int lbxgl_part_flame_blue;
int lbxgl_part_turb;

int lbxgl_part_fpart[4];
int lbxgl_part_fbpart[4];

int lbxgl_part_gibpart[4];
int lbxgl_part_mtgpart[8];

int lbxgl_part_smoke;
int lbxgl_part_generic;
int lbxgl_part_generic2;

int lbxgl_part_square;

float lbxgl_part_expose;

LBXGL_API void LBXGL_Particle_Init()
{
	static int init=0;
	LBXGL_ParticleInfo *inf;

	if(init)return;
	init=1;

	printf("LBXGL_Particle_Init\n");

	LBXGL_InitCam();

	lbxgl_part_flame=LBXGL_Texture_LoadImage("textures/effects/flame0");
	lbxgl_part_flame_green=LBXGL_Texture_LoadImage("textures/effects/flame_green0");
	lbxgl_part_flame_blue=LBXGL_Texture_LoadImage("textures/effects/flame_blue0");

//	lbxgl_part_flame=LBXGL_Texture_LoadImage("textures/effects/fire/bgb_fire2");
	lbxgl_part_turb=LBXGL_Texture_LoadImage("textures/effects/fire/turb0");

//	lbxgl_part_fpart[0]=LBXGL_Texture_LoadImage(
//		"textures/effects/fire/fpart0");
//	lbxgl_part_fpart[1]=LBXGL_Texture_LoadImage(
//		"textures/effects/fire/fpart1");
//	lbxgl_part_fpart[2]=LBXGL_Texture_LoadImage(
//		"textures/effects/fire/fpart2");
//	lbxgl_part_fpart[3]=LBXGL_Texture_LoadImage(
//		"textures/effects/fire/fpart3");

	lbxgl_part_fpart[0]=LBXGL_Texture_LoadImage(
		"textures/effects/fire/fy_part0");
	lbxgl_part_fpart[1]=LBXGL_Texture_LoadImage(
		"textures/effects/fire/fy_part1");
	lbxgl_part_fpart[2]=LBXGL_Texture_LoadImage(
		"textures/effects/fire/fy_part2");
	lbxgl_part_fpart[3]=LBXGL_Texture_LoadImage(
		"textures/effects/fire/fy_part3");

	lbxgl_part_fbpart[0]=LBXGL_Texture_LoadImage("textures/effects/fire/fb_part0");
	lbxgl_part_fbpart[1]=LBXGL_Texture_LoadImage("textures/effects/fire/fb_part1");
	lbxgl_part_fbpart[2]=LBXGL_Texture_LoadImage("textures/effects/fire/fb_part2");
	lbxgl_part_fbpart[3]=LBXGL_Texture_LoadImage("textures/effects/fire/fb_part3");

	lbxgl_part_gibpart[0]=LBXGL_Texture_LoadImage("textures/effects/gib/gib_part0");
	lbxgl_part_gibpart[1]=LBXGL_Texture_LoadImage("textures/effects/gib/gib_part1");
	lbxgl_part_gibpart[2]=LBXGL_Texture_LoadImage("textures/effects/gib/gib_part2");
	lbxgl_part_gibpart[3]=LBXGL_Texture_LoadImage("textures/effects/gib/gib_part3");

	lbxgl_part_mtgpart[0]=LBXGL_Texture_LoadImage("textures/effects/metgib/met_part0");
	lbxgl_part_mtgpart[1]=LBXGL_Texture_LoadImage("textures/effects/metgib/met_part1");
	lbxgl_part_mtgpart[2]=LBXGL_Texture_LoadImage("textures/effects/metgib/met_part2");
	lbxgl_part_mtgpart[3]=LBXGL_Texture_LoadImage("textures/effects/metgib/met_part3");
	lbxgl_part_mtgpart[4]=LBXGL_Texture_LoadImage("textures/effects/metgib/met_part4");
	lbxgl_part_mtgpart[5]=LBXGL_Texture_LoadImage("textures/effects/metgib/met_part5");
	lbxgl_part_mtgpart[6]=LBXGL_Texture_LoadImage("textures/effects/metgib/met_part6");
	lbxgl_part_mtgpart[7]=LBXGL_Texture_LoadImage("textures/effects/metgib/met_part7");

	lbxgl_part_smoke=LBXGL_Texture_LoadImage("textures/effects/smoke1");
	lbxgl_part_generic=LBXGL_Texture_LoadImage("textures/effects/fpart");
	lbxgl_part_generic2=LBXGL_Texture_LoadImage("textures/effects/fp2");

	lbxgl_part_square=LBXGL_Texture_LoadImage("textures/effects/square0");

	inf=LBXGL_Particle_GetTypeInfo("flame");
	inf->step=lbxgl_part_flame_step;
//	inf->draw=lbxgl_part_sprite_draw;
//	inf->draw=lbxgl_part_sprite_draw_light;
	inf->draw=lbxgl_part_sprite_draw_lightexpose;
	inf->spawn=lbxgl_part_flame_spawn;
	inf->drawMulti=lbxgl_part_sprite_drawMulti_lightexpose;

	inf=LBXGL_Particle_GetTypeInfo("flame_blue");
	inf->step=lbxgl_part_flame_blue_step;
//	inf->draw=lbxgl_part_sprite_draw;
//	inf->draw=lbxgl_part_sprite_draw_light;
	inf->draw=lbxgl_part_sprite_draw_lightexpose;
	inf->spawn=lbxgl_part_flame_blue_spawn;
	inf->drawMulti=lbxgl_part_sprite_drawMulti_lightexpose;

//	inf=LBXGL_Particle_GetTypeInfo("flame");
//	inf->step=lbxgl_part_flame_step;
//	inf->draw=lbxgl_part_sprite_draw;
//	inf->spawn=lbxgl_part_flame2_spawn;

	inf=LBXGL_Particle_GetTypeInfo("smoke");
	inf->step=lbxgl_part_smoke_step;
	inf->draw=lbxgl_part_sprite_draw;
	inf->spawn=lbxgl_part_smoke_spawn;
	inf->drawMulti=lbxgl_part_sprite_drawMulti;

	inf=LBXGL_Particle_GetTypeInfo("smoke_light");
	inf->step=lbxgl_part_smoke_step;
	inf->draw=lbxgl_part_sprite_draw_light;
	inf->spawn=lbxgl_part_smoke_light_spawn;
	inf->drawMulti=lbxgl_part_sprite_drawMulti_light;


	inf=LBXGL_Particle_GetTypeInfo("gib_fall");
	inf->step=lbxgl_part_fall_step;
	inf->draw=lbxgl_part_sprite_draw;
	inf->spawn=lbxgl_part_gib_spawn;
	inf->drawMulti=lbxgl_part_sprite_drawMulti;

	inf=LBXGL_Particle_GetTypeInfo("metgib_fall");
	inf->step=lbxgl_part_fall_step;
	inf->draw=lbxgl_part_sprite_draw;
	inf->spawn=lbxgl_part_metgib_spawn;
	inf->drawMulti=lbxgl_part_sprite_drawMulti;

	inf=LBXGL_Particle_GetTypeInfo("generic_rise");
	inf->step=lbxgl_part_flame_step;
	inf->draw=lbxgl_part_sprite_draw;
	inf->spawn=lbxgl_part_generic_spawn;
	inf->drawMulti=lbxgl_part_sprite_drawMulti;

	inf=LBXGL_Particle_GetTypeInfo("generic_fall");
	inf->step=lbxgl_part_fall_step;
	inf->draw=lbxgl_part_sprite_draw;
	inf->spawn=lbxgl_part_generic_spawn;
	inf->drawMulti=lbxgl_part_sprite_drawMulti;

	inf=LBXGL_Particle_GetTypeInfo("generic_light_rise");
	inf->step=lbxgl_part_flame_step;
	inf->draw=lbxgl_part_sprite_draw_light;
	inf->spawn=lbxgl_part_generic_spawn;
	inf->drawMulti=lbxgl_part_sprite_drawMulti_light;

	inf=LBXGL_Particle_GetTypeInfo("generic_light_fall");
	inf->step=lbxgl_part_fall_step;
	inf->draw=lbxgl_part_sprite_draw_light;
	inf->spawn=lbxgl_part_generic_spawn;
	inf->drawMulti=lbxgl_part_sprite_drawMulti_light;



	inf=LBXGL_Particle_GetTypeInfo("square_rise");
	inf->step=lbxgl_part_flame_step;
	inf->draw=lbxgl_part_sprite_draw;
	inf->spawn=lbxgl_part_square_spawn;
	inf->drawMulti=lbxgl_part_sprite_drawMulti;

	inf=LBXGL_Particle_GetTypeInfo("square_light_rise");
	inf->step=lbxgl_part_flame_step;
	inf->draw=lbxgl_part_sprite_draw_light;
	inf->spawn=lbxgl_part_square_spawn;
	inf->drawMulti=lbxgl_part_sprite_drawMulti_light;

	inf=LBXGL_Particle_GetTypeInfo("square_fall");
	inf->step=lbxgl_part_fall_step;
	inf->draw=lbxgl_part_sprite_draw;
	inf->spawn=lbxgl_part_square_spawn;
	inf->drawMulti=lbxgl_part_sprite_drawMulti;

	inf=LBXGL_Particle_GetTypeInfo("square_light_fall");
	inf->step=lbxgl_part_fall_step;
	inf->draw=lbxgl_part_sprite_draw_light;
	inf->spawn=lbxgl_part_square_spawn;
	inf->drawMulti=lbxgl_part_sprite_drawMulti_light;

	LBXGL_Particle_Init2();

	lbxgl_part_cnt=0;
	lbxgl_part_max=10000;

//	kseed_string("foo particle seed string");
}

LBXGL_API int LBXGL_Particle_LookupType(char *name)
{
	int i;
	for(i=0; i<lbxgl_part_ntypes; i++)
		if(!strcmp(lbxgl_part_type[i]->name, name))
			return(i);
	return(-1);
}

LBXGL_API int LBXGL_Particle_GetType(char *name)
{
	int i;

	i=LBXGL_Particle_LookupType(name);
	if(i>=0)return(i);

	i=lbxgl_part_ntypes++;
	lbxgl_part_type[i]=malloc(sizeof(LBXGL_ParticleInfo));
	memset(lbxgl_part_type[i], 0, sizeof(LBXGL_ParticleInfo));

	lbxgl_part_type[i]->name=strdup(name);
	return(i);
}

LBXGL_API LBXGL_ParticleInfo *LBXGL_Particle_GetTypeInfo(char *name)
{
	LBXGL_ParticleInfo *inf;
	int i;

	i=LBXGL_Particle_GetType(name);
	inf=lbxgl_part_type[i];
	return(inf);
}

LBXGL_API LBXGL_Particle *LBXGL_Particle_Alloc(char *name, int cnt)
{
	LBXGL_Particle *cur, *lst;
	int i;

	if(lbxgl_part_cnt>=lbxgl_part_max)
	{
		return(NULL);
	}

	i=LBXGL_Particle_GetType(name);

	lst=NULL;
	while(cnt && lbxgl_part_free)
	{
		cur=lbxgl_part_free;
		lbxgl_part_free=cur->next;

		cur->prev=NULL;
		cur->next=lbxgl_part_root;
		if(lbxgl_part_root)lbxgl_part_root->prev=cur;
		lbxgl_part_root=cur;

		lbxgl_part_cnt++;

		cur->type=i;
		cur->chain=lst;
		lst=cur;
		cnt--;
	}

	while(cnt)
	{
		cur=gctalloc("lbxgl_particle_t", sizeof(LBXGL_Particle));

		cur->prev=NULL;
		cur->next=lbxgl_part_root;
		if(lbxgl_part_root)lbxgl_part_root->prev=cur;
		lbxgl_part_root=cur;

		lbxgl_part_cnt++;

		cur->type=i;
		cur->chain=lst;
		lst=cur;
		cnt--;
	}

	return(lst);
}

LBXGL_API void LBXGL_Particle_Free(LBXGL_Particle *cur)
{
	if(cur->type<0)
	{
		*(int *)-1=-1;
	}

	lbxgl_part_cnt--;

	if(cur->prev)
	{
		cur->prev->next=cur->next;
		if(cur->next)cur->next->prev=cur->prev;

		cur->type=-1;
		cur->prev=NULL;
		cur->next=lbxgl_part_free;
		lbxgl_part_free=cur;
	}else
	{
		lbxgl_part_root=cur->next;
		if(cur->next)cur->next->prev=NULL;

		cur->prev=NULL;
		cur->next=lbxgl_part_free;
		lbxgl_part_free=cur;
	}
}

LBXGL_API void LBXGL_Particle_Step(float dt)
{
	LBXGL_Particle *cur, *nxt;
	LBXGL_ParticleInfo *inf;
	void *p;

	LBXGL_Particle_Init();

//	LBXGL_Particle_RebuildTree();

	if(dt>1)
	{
//		*(int *)-1=-1;
		return;
	}

	cur=lbxgl_part_root;
	while(cur)
	{
		if(cur->type<0)
			{ *(int *)-1=-1; }
	
		inf=lbxgl_part_type[cur->type];
		nxt=cur->next;
		if(inf->step)inf->step(cur, dt);
		cur=nxt;
	}
}

LBXGL_Particle *lbxgl_particle_drawhash[256];

LBXGL_API void LBXGL_Particle_DrawR(LBXGL_ParticleNode *node)
{
//	LBXGL_Particle *hash[64];
	LBXGL_ParticleInfo *inf;
	LBXGL_Particle *cur, *lst;
	float f, g;
	int i, j, k;

	if(!node)return;

#if 1
	cur=node->mid;
	while(cur)
	{
		i=cur->type*251+cur->tex;
		i=((i*251)>>8)&255;
		cur->chain=lbxgl_particle_drawhash[i];
		lbxgl_particle_drawhash[i]=cur;
		cur=cur->nnext;
	}
#endif

#if 0
	memset(hash, 0, 64*sizeof(LBXGL_Particle *));
	cur=node->mid;
	while(cur)
	{
		i=cur->type*251+cur->tex;
		i=((i*251)>>8)&63;
		cur->chain=hash[i];
		hash[i]=cur;
		cur=cur->nnext;
	}

	for(i=0; i<64; i++)
	{
		cur=hash[i];
		while(cur)
		{
			inf=lbxgl_part_type[cur->type];
			if(inf->drawMulti)
			{
				cur=inf->drawMulti(cur);
				continue;
			}else if(inf->draw)
			{
				inf->draw(cur);
			}
			cur=cur->chain;
		}
	}
#endif

#if 0
	cur=node->mid;
	while(cur)
	{
		inf=lbxgl_part_type[cur->type];
		if(inf->drawMulti)
		{
			cur=inf->drawMulti(cur);
			continue;
		}else if(inf->draw)
		{
			inf->draw(cur);
		}

		cur=cur->nnext;
	}
#endif

	f=V3F_NDOT(lbxgl_cam->org, node->norm);

	if(f<0)
	{
		LBXGL_Particle_DrawR(node->right);
		LBXGL_Particle_DrawR(node->left);
	}else
	{
		LBXGL_Particle_DrawR(node->left);
		LBXGL_Particle_DrawR(node->right);
	}
}

LBXGL_API void LBXGL_Particle_Draw()
{
	LBXGL_Particle *cur;
	LBXGL_ParticleInfo *inf;
	void *p;
	int i, j, k;

	LBXGL_Particle_Init();
	LBXGL_Particle_RebuildTree();

	memset(lbxgl_particle_drawhash, 0, 256*sizeof(LBXGL_Particle *));

	LBXGL_Particle_DrawR(lbxgl_part_noderoot);

#if 1
//	memset(lbxgl_particle_drawhash, 0, 256*sizeof(LBXGL_Particle *));
	for(i=0; i<256; i++)
	{
		cur=lbxgl_particle_drawhash[i];
		while(cur)
		{
			inf=lbxgl_part_type[cur->type];
			if(inf->drawMulti)
			{
				cur=inf->drawMulti(cur);
				continue;
			}else if(inf->draw)
			{
				inf->draw(cur);
			}
			cur=cur->chain;
		}
	}
#endif


#if 0
	cur=lbxgl_part_root;
	while(cur)
	{
		inf=lbxgl_part_type[cur->type];
		if(inf->draw)inf->draw(cur);
		cur=cur->next;
	}
#endif
}

LBXGL_API void LBXGL_Particle_DrawWorld(LBXGL_BrushWorld *world)
{
	lbxgl_part_expose=world->cam_expose;
	LBXGL_Particle_Draw();
}

LBXGL_API void LBXGL_Particle_Spawn(char *ty, int cnt,
	float *org, float *vel)
{
	LBXGL_Particle *cur, *lst;
	LBXGL_ParticleInfo *inf;
	int i;

	LBXGL_Particle_Init();

	if(!cnt)return;

//	printf("LBXGL_Particle_Spawn: %d (%g %g %g) (%g %g %g)\n", cnt,
//		org[0], org[1], org[2], vel[0], vel[1], vel[2]);

	i=LBXGL_Particle_GetType(ty);
	inf=lbxgl_part_type[i];

	lst=LBXGL_Particle_Alloc(ty, cnt);

	if(!inf->spawn)return;

	cur=lst;
	while(cur)
	{
		cur->clr=0xFFFFFF;
		inf->spawn(cur, org, vel);
		cur=cur->chain;
	}
}

LBXGL_API void LBXGL_Particle_SpawnSpread(
	char *ty, int cnt, int clr,
	float *org, float *vel, float *svel)
{
	float tv[3];
	LBXGL_Particle *cur, *lst;
	LBXGL_ParticleInfo *inf;
	int i;

	LBXGL_Particle_Init();

	if(!cnt)return;

//	printf("LBXGL_Particle_Spawn: %d (%g %g %g) (%g %g %g)\n", cnt,
//		org[0], org[1], org[2], vel[0], vel[1], vel[2]);

	i=LBXGL_Particle_GetType(ty);
	inf=lbxgl_part_type[i];

	lst=LBXGL_Particle_Alloc(ty, cnt);

	if(!inf->spawn)return;

	cur=lst;
	while(cur)
	{
		cur->clr=clr;
		tv[0]=vel[0]+svel[0]*bgbrng_randg();
		tv[1]=vel[1]+svel[1]*bgbrng_randg();
		tv[2]=vel[2]+svel[2]*bgbrng_randg();
		inf->spawn(cur, org, tv);
		cur=cur->chain;
	}
}

LBXGL_API LBXGL_ParticleNode *LBXGL_Particle_AllocNode()
{
	LBXGL_ParticleNode *tmp;
	if(lbxgl_part_nodefree)
	{
		tmp=lbxgl_part_nodefree;
		lbxgl_part_nodefree=tmp->left;

		tmp->left=NULL;
//		tmp->right=NULL;
//		tmp->mid=NULL;
		return(tmp);
	}

	tmp=gcalloc(sizeof(LBXGL_ParticleNode));
	return(tmp);
}

LBXGL_API void LBXGL_Particle_FreeNode(LBXGL_ParticleNode *tmp)
{
	tmp->left=NULL;
	tmp->right=NULL;
	tmp->mid=NULL;
	tmp->left=lbxgl_part_nodefree;
	lbxgl_part_nodefree=tmp;
}

LBXGL_API void LBXGL_Particle_FreeTreeNode(LBXGL_ParticleNode *tmp)
{
	if(!tmp)return;
	LBXGL_Particle_FreeTreeNode(tmp->left);
	LBXGL_Particle_FreeTreeNode(tmp->right);
	LBXGL_Particle_FreeNode(tmp);
}

LBXGL_API LBXGL_ParticleNode *LBXGL_Particle_BuildNodeList(LBXGL_Particle *lst)
{
	float org[3], dir[4];
	LBXGL_ParticleNode *tmp;
	LBXGL_Particle *cur, *nxt, *llst, *rlst, *mlst;
	float f, g, m, n;
	int i, j, k;

	tmp=LBXGL_Particle_AllocNode();
	if(!lst || !lst->next)
	{
		cur=lst;
		while(cur)
		{
			cur->nnext=tmp->mid;
			tmp->mid=cur;
			cur=cur->chain;
		}
		return(tmp);
	}

	cur=lst; V3F_ZERO(org); i=0;
	while(cur)
	{
		org[0]+=cur->org[0];
		org[1]+=cur->org[1];
		org[2]+=cur->org[2];
		i++; cur=cur->chain;
	}
	org[0]/=i; org[1]/=i; org[2]/=i;

	cur=lst; V3F_ZERO(dir);
	while(cur)
	{
		dir[0]+=fabs(cur->org[0]-org[0]);
		dir[1]+=fabs(cur->org[1]-org[1]);
		dir[2]+=fabs(cur->org[2]-org[2]);
		cur=cur->chain;
	}
	V3F_NORMALIZE(dir, dir); dir[3]=V3F_DOT(org, dir);

	cur=lst; i=0; j=0; m=0; n=0;
	while(cur)
	{
		f=V3F_NDOT(cur->org, dir);
		if(f==0) { k++; cur=cur->chain; continue; }

		g=(cur->psi_lin/fabs(f)) + 
			(cur->psi_quad/(f*f)) +
			(cur->psi_cube/fabs(f*f*f));

//		if(g>0.1) { k++; cur=cur->chain; continue; }
		if(g>1) { k++; cur=cur->chain; continue; }

		if(f<0)i++; if(f>0)j++;
		if(f<m)m=f;
		if(f>n)n=f;
		if(i && j) break;
		cur=cur->chain;
	}

//	if(!i || !j)
	if((i<16) || (j<16) || ((n-m)<64))
	{
		cur=lst;
		while(cur)
		{
			cur->nnext=tmp->mid;
			tmp->mid=cur;
			cur=cur->chain;
		}
		return(tmp);
	}

	cur=lst; llst=NULL; rlst=NULL; mlst=NULL;
	while(cur)
	{
		nxt=cur->chain;
		f=V3F_NDOT(cur->org, dir);
		if(f==0) { cur->chain=mlst; mlst=cur; cur=nxt; continue; }
		g=(cur->psi_lin/fabs(f)) + 
			(cur->psi_quad/(f*f)) +
			(cur->psi_cube/fabs(f*f*f));
//		if(g>0.1) { cur->chain=mlst; mlst=cur; cur=nxt; continue; }
		if(g>1) { cur->chain=mlst; mlst=cur; cur=nxt; continue; }
		if(f<0) { cur->chain=llst; llst=cur; cur=nxt; continue; }
		if(f>0) { cur->chain=rlst; rlst=cur; cur=nxt; continue; }
		cur->chain=mlst; mlst=cur; cur=nxt; continue;
	}

	tmp->left=LBXGL_Particle_BuildNodeList(llst);
	tmp->right=LBXGL_Particle_BuildNodeList(rlst);

	cur=mlst;
	while(cur)
	{
		cur->nnext=tmp->mid;
		tmp->mid=cur;
		cur=cur->chain;
	}
	return(tmp);
}

LBXGL_API void LBXGL_Particle_RebuildTree()
{
	LBXGL_Particle *cur, *lst;
	float f;

	LBXGL_Particle_FreeTreeNode(lbxgl_part_noderoot);
	lbxgl_part_noderoot=NULL;

	cur=lbxgl_part_root; lst=NULL;
	while(cur)
	{
//		f=fabs(cur->psi_lin)+
//			fabs(cur->psi_quad)+
//			fabs(cur->psi_cube);
//		if(f<0.1) { cur=cur->next; continue; }

		cur->chain=lst; lst=cur;
		cur=cur->next;
	}

	if(!lst)return;

	lbxgl_part_noderoot=LBXGL_Particle_BuildNodeList(lst);
}

void LBXGL_Particle_CalcPsiPartR(LBXGL_ParticleNode *node,
	LBXGL_Particle *self, float *vec)
{
	float dv[3];
	LBXGL_Particle *cur, *lst;
	float f, g;

	if(!node)return;

	cur=node->mid;
	while(cur)
	{
		if(cur==self) { cur=cur->nnext; continue; }
		V3F_SUB(self->org, cur->org, dv);
		f=V3F_NORMALIZE(dv, dv);
		g=(cur->psi_lin/fabs(f)) + 
			(cur->psi_quad/(f*f)) +
			(cur->psi_cube/fabs(f*f*f));
		V3F_ADDSCALE(vec, dv, g, vec);
		cur=cur->nnext;
	}

	f=V3F_NDOT(self->org, node->norm);

	if(fabs(f)>0.1)
	{
		g=(self->psi_lin/fabs(f)) + 
			(self->psi_quad/(f*f)) +
			(self->psi_cube/fabs(f*f*f));
	}else
	{
		g=0.1;
		g=(self->psi_lin/g) + 
			(self->psi_quad/(g*g)) +
			(self->psi_cube/(g*g*g));
	}

	if(g>0.1)
	{
		LBXGL_Particle_CalcPsiPartR(node->left, self, vec);
		LBXGL_Particle_CalcPsiPartR(node->right, self, vec);
		return;
	}

	if(f<0)
	{
		LBXGL_Particle_CalcPsiPartR(node->left, self, vec);
		return;
	}

	if(f>0)
	{
		LBXGL_Particle_CalcPsiPartR(node->right, self, vec);
		return;
	}

	LBXGL_Particle_CalcPsiPartR(node->left, self, vec);
	LBXGL_Particle_CalcPsiPartR(node->right, self, vec);
}

LBXGL_API void LBXGL_Particle_CalcPsiPart(LBXGL_Particle *self, float *vec)
{
	float f;

	f=fabs(self->psi_lin)+fabs(self->psi_quad)+fabs(self->psi_cube);
	if(f<0.1)
	{
		V3F_ZERO(vec);
		return;
	}

	V3F_ZERO(vec);
	LBXGL_Particle_CalcPsiPartR(lbxgl_part_noderoot, self, vec);

	vec[0]+=10*pow(bgbrng_randg(), 2);
	vec[1]+=10*pow(bgbrng_randg(), 2);
	vec[2]+=10*pow(bgbrng_randg(), 2);
}


void lbxgl_part_sprite_draw_deform(LBXGL_Particle *self)
{
	float v[3];
	float f;
	int def;
	int i;

//	printf("sprite draw (%g %g %g) %g %d\n",
//		self->org[0], self->org[1], self->org[2],
//		self->rad, self->tex);

	glDisable(GL_DEPTH_TEST);
//	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
//	glDisable(GL_TEXTURE_2D);

	glBlendFunc(GL_SRC_COLOR, GL_ONE_MINUS_SRC_COLOR);

	i=PDGL_LoadShader("bgb_deform2");
	PDGL_BindShader(i);

	PDGL_Uniform1f("time", self->time);
	PDGL_Uniform1f("scale", 0.25);
	PDGL_Uniform1f("rate", 0.25);

	PDGL_Uniform1i("texBase", 0);
	PDGL_Uniform1i("texDeform", 1);

	pdglActiveTexture(0);
	glBindTexture(GL_TEXTURE_2D, self->tex);
	glEnable(GL_TEXTURE_2D);

	pdglActiveTexture(1);
	glBindTexture(GL_TEXTURE_2D, lbxgl_part_turb);
	glEnable(GL_TEXTURE_2D);

	f=1.0-(self->time/self->lim);

//	pdglColor4f(1, 1, 1, 1.0-(self->time/self->lim));
	pdglColor4f(f, f, f, f);

	pdglBegin(PDGL_QUADS);

	V3F_ADDSCALE(self->org, lbxgl_cam->rt, -self->rad, v);
	V3F_ADDSCALE(v, lbxgl_cam->up, self->rad, v);
	pdglTexCoord2f(0, 1);
	pdglVertex3fv(v);

	V3F_ADDSCALE(self->org, lbxgl_cam->rt, -self->rad, v);
	V3F_ADDSCALE(v, lbxgl_cam->up, -self->rad, v);
	pdglTexCoord2f(0, 0);
	pdglVertex3fv(v);

	V3F_ADDSCALE(self->org, lbxgl_cam->rt, self->rad, v);
	V3F_ADDSCALE(v, lbxgl_cam->up, -self->rad, v);
	pdglTexCoord2f(1, 0);
	pdglVertex3fv(v);

	V3F_ADDSCALE(self->org, lbxgl_cam->rt, self->rad, v);
	V3F_ADDSCALE(v, lbxgl_cam->up, self->rad, v);
	pdglTexCoord2f(1, 1);
	pdglVertex3fv(v);

	pdglEnd();

	pdglActiveTexture(0);
	glBindTexture(GL_TEXTURE_2D, 0);
	glDisable(GL_TEXTURE_2D);

	pdglActiveTexture(1);
	glBindTexture(GL_TEXTURE_2D, 0);
	glDisable(GL_TEXTURE_2D);

	pdglActiveTexture(0);
	PDGL_UnbindShader();

	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glEnable(GL_DEPTH_TEST);
}

LBXGL_Particle *lbxgl_part_sprite_drawMulti(LBXGL_Particle *self)
{
	LBXGL_Particle *cur;
	float v[3];

//	printf("sprite draw (%g %g %g) %g %d\n",
//		self->org[0], self->org[1], self->org[2],
//		self->rad, self->tex);

	glDepthMask(0);
//	glDisable(GL_DEPTH_TEST);
	LBXGL_Shader_BlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
//	glDisable(GL_TEXTURE_2D);

//	glEnable(GL_TEXTURE_2D);
//	glBindTexture(GL_TEXTURE_2D, self->tex);

	LBXGL_Shader_BindTexture(self->tex);

//	pdglColor4f(1, 1, 1, 1.0-(self->time/self->lim));

	LBXGL_Shader_Begin(PDGL_QUADS);

	cur=self;
	while(cur)
	{
		if(cur->type!=self->type)
			break;
		if(cur->tex!=self->tex)
			break;
	
		LBXGL_Shader_Color4f(
			(cur->clr&0xFF)*(1.0/255.0),
			(cur->clr&0xFF00)*(1.0/65280.0),
			(cur->clr&0xFF0000)*(1.0/16711680.0),
			1.0-(cur->time/cur->lim));

		V3F_ADDSCALE(cur->org, lbxgl_cam->rt, -self->rad, v);
		V3F_ADDSCALE(v, lbxgl_cam->up, cur->rad, v);
		LBXGL_Shader_TexCoord2f(0, 1);
		LBXGL_Shader_Vertex3fv(v);

		V3F_ADDSCALE(cur->org, lbxgl_cam->rt, -cur->rad, v);
		V3F_ADDSCALE(v, lbxgl_cam->up, -cur->rad, v);
		LBXGL_Shader_TexCoord2f(0, 0);
		LBXGL_Shader_Vertex3fv(v);

		V3F_ADDSCALE(cur->org, lbxgl_cam->rt, cur->rad, v);
		V3F_ADDSCALE(v, lbxgl_cam->up, -cur->rad, v);
		LBXGL_Shader_TexCoord2f(1, 0);
		LBXGL_Shader_Vertex3fv(v);

		V3F_ADDSCALE(cur->org, lbxgl_cam->rt, cur->rad, v);
		V3F_ADDSCALE(v, lbxgl_cam->up, cur->rad, v);
		LBXGL_Shader_TexCoord2f(1, 1);
		LBXGL_Shader_Vertex3fv(v);
		
//		cur=cur->nnext;
		cur=cur->chain;
	}

	LBXGL_Shader_End();

	glDepthMask(1);
//	glEnable(GL_DEPTH_TEST);

	return(cur);
}

LBXGL_Particle *lbxgl_part_sprite_drawMulti_light(LBXGL_Particle *self)
{
	LBXGL_Particle *cur;
	float v[3];

//	printf("sprite draw (%g %g %g) %g %d\n",
//		self->org[0], self->org[1], self->org[2],
//		self->rad, self->tex);

	glDepthMask(0);
//	glDisable(GL_DEPTH_TEST);
	LBXGL_Shader_BlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
//	glDisable(GL_TEXTURE_2D);

//	glEnable(GL_TEXTURE_2D);
//	glBindTexture(GL_TEXTURE_2D, self->tex);

	LBXGL_Shader_BindTexture(self->tex);

//	pdglColor4f(1, 1, 1, 1.0-(self->time/self->lim));

	LBXGL_Shader_Begin(PDGL_QUADS);

	cur=self;
	while(cur)
	{
		if(cur->type!=self->type)
			break;
		if(cur->tex!=self->tex)
			break;
	
		LBXGL_Shader_Color4f(
			(cur->clr&0xFF)*(1.0/255.0),
			(cur->clr&0xFF00)*(1.0/65280.0),
			(cur->clr&0xFF0000)*(1.0/16711680.0),
			(1.0-(cur->time/cur->lim))*0.25);

		V3F_ADDSCALE(cur->org, lbxgl_cam->rt, -self->rad, v);
		V3F_ADDSCALE(v, lbxgl_cam->up, cur->rad, v);
		LBXGL_Shader_TexCoord2f(0, 1);
		LBXGL_Shader_Vertex3fv(v);

		V3F_ADDSCALE(cur->org, lbxgl_cam->rt, -cur->rad, v);
		V3F_ADDSCALE(v, lbxgl_cam->up, -cur->rad, v);
		LBXGL_Shader_TexCoord2f(0, 0);
		LBXGL_Shader_Vertex3fv(v);

		V3F_ADDSCALE(cur->org, lbxgl_cam->rt, cur->rad, v);
		V3F_ADDSCALE(v, lbxgl_cam->up, -cur->rad, v);
		LBXGL_Shader_TexCoord2f(1, 0);
		LBXGL_Shader_Vertex3fv(v);

		V3F_ADDSCALE(cur->org, lbxgl_cam->rt, cur->rad, v);
		V3F_ADDSCALE(v, lbxgl_cam->up, cur->rad, v);
		LBXGL_Shader_TexCoord2f(1, 1);
		LBXGL_Shader_Vertex3fv(v);
		
//		cur=cur->nnext;
		cur=cur->chain;
	}

	LBXGL_Shader_End();

	glDepthMask(1);
//	glEnable(GL_DEPTH_TEST);

	return(cur);
}

LBXGL_Particle *lbxgl_part_sprite_drawMulti_lightexpose(LBXGL_Particle *self)
{
	float v[3], pt0[3], pt1[3], pt2[3], pt3[3];
	LBXGL_Particle *cur;
	float g, h;

	glDepthMask(0);
//	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	LBXGL_Shader_BlendFunc(GL_SRC_COLOR, GL_ONE);

//	glEnable(GL_TEXTURE_2D);
//	glBindTexture(GL_TEXTURE_2D, self->tex);

	LBXGL_Shader_BindTexture(self->tex);

//	V3F_ADDSCALE(self->org, lbxgl_cam->rt, -self->rad, v);
//	V3F_ADDSCALE(v, lbxgl_cam->up, self->rad, pt0);

//	V3F_ADDSCALE(self->org, lbxgl_cam->rt, -self->rad, v);
//	V3F_ADDSCALE(v, lbxgl_cam->up, -self->rad, pt1);

//	V3F_ADDSCALE(self->org, lbxgl_cam->rt, self->rad, v);
//	V3F_ADDSCALE(v, lbxgl_cam->up, -self->rad, pt2);

//	V3F_ADDSCALE(self->org, lbxgl_cam->rt, self->rad, pt3);
//	V3F_ADDSCALE(v, lbxgl_cam->up, self->rad, pt3);

//	g=(1.0-(self->time/self->lim))*0.25;
	g=1.0-(self->time/self->lim);
	h=1.0/lbxgl_part_expose;
	if(h<1.0)h=1.0;
	
	h*=g;

//	pdglBegin(PDGL_QUADS);

//	LBXGL_Shader_Color4f(
//		h*(self->clr&0xFF)*(1.0/255.0),
//		h*(self->clr&0xFF00)*(1.0/65280.0),
//		h*(self->clr&0xFF0000)*(1.0/16711680.0),
//		1);

	LBXGL_Shader_Begin(PDGL_QUADS);

	cur=self;
	while(cur)
	{
		if(cur->type!=self->type)
			break;
		if(cur->tex!=self->tex)
			break;
	
		LBXGL_Shader_Color4f(
			h*(cur->clr&0xFF)*(1.0/255.0),
			h*(cur->clr&0xFF00)*(1.0/65280.0),
			h*(cur->clr&0xFF0000)*(1.0/16711680.0),
			1);

		V3F_ADDSCALE(cur->org, lbxgl_cam->rt, -self->rad, v);
		V3F_ADDSCALE(v, lbxgl_cam->up, cur->rad, v);
		LBXGL_Shader_TexCoord2f(0, 1);
		LBXGL_Shader_Vertex3fv(v);

		V3F_ADDSCALE(cur->org, lbxgl_cam->rt, -cur->rad, v);
		V3F_ADDSCALE(v, lbxgl_cam->up, -cur->rad, v);
		LBXGL_Shader_TexCoord2f(0, 0);
		LBXGL_Shader_Vertex3fv(v);

		V3F_ADDSCALE(cur->org, lbxgl_cam->rt, cur->rad, v);
		V3F_ADDSCALE(v, lbxgl_cam->up, -cur->rad, v);
		LBXGL_Shader_TexCoord2f(1, 0);
		LBXGL_Shader_Vertex3fv(v);

		V3F_ADDSCALE(cur->org, lbxgl_cam->rt, cur->rad, v);
		V3F_ADDSCALE(v, lbxgl_cam->up, cur->rad, v);
		LBXGL_Shader_TexCoord2f(1, 1);
		LBXGL_Shader_Vertex3fv(v);
		
//		cur=cur->nnext;
		cur=cur->chain;
	}

	LBXGL_Shader_End();
	LBXGL_Shader_BlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	glDepthMask(1);

	return(cur);
}

#if 1
void lbxgl_part_sprite_draw(LBXGL_Particle *self)
{
	float v[3];

//	printf("sprite draw (%g %g %g) %g %d\n",
//		self->org[0], self->org[1], self->org[2],
//		self->rad, self->tex);

	glDepthMask(0);
//	glDisable(GL_DEPTH_TEST);
	LBXGL_Shader_BlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
//	glDisable(GL_TEXTURE_2D);

//	glEnable(GL_TEXTURE_2D);
//	glBindTexture(GL_TEXTURE_2D, self->tex);

	LBXGL_Shader_BindTexture(self->tex);

//	pdglColor4f(1, 1, 1, 1.0-(self->time/self->lim));
	LBXGL_Shader_Color4f(
		(self->clr&0xFF)*(1.0/255.0),
		(self->clr&0xFF00)*(1.0/65280.0),
		(self->clr&0xFF0000)*(1.0/16711680.0),
		1.0-(self->time/self->lim));

	LBXGL_Shader_Begin(PDGL_QUADS);

	V3F_ADDSCALE(self->org, lbxgl_cam->rt, -self->rad, v);
	V3F_ADDSCALE(v, lbxgl_cam->up, self->rad, v);
	LBXGL_Shader_TexCoord2f(0, 1);
	LBXGL_Shader_Vertex3fv(v);

	V3F_ADDSCALE(self->org, lbxgl_cam->rt, -self->rad, v);
	V3F_ADDSCALE(v, lbxgl_cam->up, -self->rad, v);
	LBXGL_Shader_TexCoord2f(0, 0);
	LBXGL_Shader_Vertex3fv(v);

	V3F_ADDSCALE(self->org, lbxgl_cam->rt, self->rad, v);
	V3F_ADDSCALE(v, lbxgl_cam->up, -self->rad, v);
	LBXGL_Shader_TexCoord2f(1, 0);
	LBXGL_Shader_Vertex3fv(v);

	V3F_ADDSCALE(self->org, lbxgl_cam->rt, self->rad, v);
	V3F_ADDSCALE(v, lbxgl_cam->up, self->rad, v);
	LBXGL_Shader_TexCoord2f(1, 1);
	LBXGL_Shader_Vertex3fv(v);

	LBXGL_Shader_End();

	glDepthMask(1);
//	glEnable(GL_DEPTH_TEST);
}

void lbxgl_part_sprite_draw_light(LBXGL_Particle *self)
{
	float v[3];

	glDepthMask(0);

	LBXGL_Shader_BlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

//	glEnable(GL_TEXTURE_2D);
//	glBindTexture(GL_TEXTURE_2D, self->tex);

	LBXGL_Shader_BindTexture(self->tex);


//	pdglColor4f(1, 1, 1, (1.0-(self->time/self->lim))*0.25);
	LBXGL_Shader_Color4f(
		(self->clr&0xFF)*(1.0/255.0),
		(self->clr&0xFF00)*(1.0/65280.0),
		(self->clr&0xFF0000)*(1.0/16711680.0),
		(1.0-(self->time/self->lim))*0.25);

	LBXGL_Shader_Begin(PDGL_QUADS);

	V3F_ADDSCALE(self->org, lbxgl_cam->rt, -self->rad, v);
	V3F_ADDSCALE(v, lbxgl_cam->up, self->rad, v);
	LBXGL_Shader_TexCoord2f(0, 1);
	LBXGL_Shader_Vertex3fv(v);

	V3F_ADDSCALE(self->org, lbxgl_cam->rt, -self->rad, v);
	V3F_ADDSCALE(v, lbxgl_cam->up, -self->rad, v);
	LBXGL_Shader_TexCoord2f(0, 0);
	LBXGL_Shader_Vertex3fv(v);

	V3F_ADDSCALE(self->org, lbxgl_cam->rt, self->rad, v);
	V3F_ADDSCALE(v, lbxgl_cam->up, -self->rad, v);
	LBXGL_Shader_TexCoord2f(1, 0);
	LBXGL_Shader_Vertex3fv(v);

	V3F_ADDSCALE(self->org, lbxgl_cam->rt, self->rad, v);
	V3F_ADDSCALE(v, lbxgl_cam->up, self->rad, v);
	LBXGL_Shader_TexCoord2f(1, 1);
	LBXGL_Shader_Vertex3fv(v);

	LBXGL_Shader_End();

	glDepthMask(1);
}

void lbxgl_part_sprite_draw_lightexpose(LBXGL_Particle *self)
{
	float v[3], pt0[3], pt1[3], pt2[3], pt3[3];
	float g, h;

	glDepthMask(0);
//	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	LBXGL_Shader_BlendFunc(GL_SRC_COLOR, GL_ONE);

//	glEnable(GL_TEXTURE_2D);
//	glBindTexture(GL_TEXTURE_2D, self->tex);

	LBXGL_Shader_BindTexture(self->tex);

//	pdglColor4f(1, 1, 1, (1.0-(self->time/self->lim))*0.25);
//	pdglColor4f(
//		(self->clr&0xFF)*(1.0/255.0),
//		(self->clr&0xFF00)*(1.0/65280.0),
//		(self->clr&0xFF0000)*(1.0/16711680.0),
//		(1.0-(self->time/self->lim))*0.25);

	V3F_ADDSCALE(self->org, lbxgl_cam->rt, -self->rad, v);
	V3F_ADDSCALE(v, lbxgl_cam->up, self->rad, pt0);

	V3F_ADDSCALE(self->org, lbxgl_cam->rt, -self->rad, v);
	V3F_ADDSCALE(v, lbxgl_cam->up, -self->rad, pt1);

	V3F_ADDSCALE(self->org, lbxgl_cam->rt, self->rad, v);
	V3F_ADDSCALE(v, lbxgl_cam->up, -self->rad, pt2);

	V3F_ADDSCALE(self->org, lbxgl_cam->rt, self->rad, pt3);
	V3F_ADDSCALE(v, lbxgl_cam->up, self->rad, pt3);

//	g=(1.0-(self->time/self->lim))*0.25;
	g=1.0-(self->time/self->lim);
	h=1.0/lbxgl_part_expose;
	if(h<1.0)h=1.0;
	
	h*=g;

//	pdglBegin(PDGL_QUADS);

#if 0
	pdglColor4f(
		(self->clr&0xFF)*(1.0/255.0),
		(self->clr&0xFF00)*(1.0/65280.0),
		(self->clr&0xFF0000)*(1.0/16711680.0),
		1);

	while(h>1.0)
	{
//		pdglBegin(PDGL_QUADS);
		LBXGL_Shader_TexCoord2f(0, 1);
		LBXGL_Shader_Vertex3fv(pt0);
		LBXGL_Shader_TexCoord2f(0, 0);
		LBXGL_Shader_Vertex3fv(pt1);
		LBXGL_Shader_TexCoord2f(1, 0);
		LBXGL_Shader_Vertex3fv(pt2);
		LBXGL_Shader_TexCoord2f(1, 1);
		LBXGL_Shader_Vertex3fv(pt3);
//		pdglEnd();
		
		h-=1.0;
	}
#endif

	LBXGL_Shader_Color4f(
		h*(self->clr&0xFF)*(1.0/255.0),
		h*(self->clr&0xFF00)*(1.0/65280.0),
		h*(self->clr&0xFF0000)*(1.0/16711680.0),
		1);

	LBXGL_Shader_Begin(PDGL_QUADS);

	LBXGL_Shader_TexCoord2f(0, 1);
	LBXGL_Shader_Vertex3fv(pt0);

	LBXGL_Shader_TexCoord2f(0, 0);
	LBXGL_Shader_Vertex3fv(pt1);

	LBXGL_Shader_TexCoord2f(1, 0);
	LBXGL_Shader_Vertex3fv(pt2);

	LBXGL_Shader_TexCoord2f(1, 1);
	LBXGL_Shader_Vertex3fv(pt3);

	LBXGL_Shader_End();

	glDepthMask(1);
}
#endif

#if 0
void lbxgl_part_sprite_draw(LBXGL_Particle *self)
{
	float v[3];

//	printf("sprite draw (%g %g %g) %g %d\n",
//		self->org[0], self->org[1], self->org[2],
//		self->rad, self->tex);

	glDepthMask(0);
//	glDisable(GL_DEPTH_TEST);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
//	glDisable(GL_TEXTURE_2D);

	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, self->tex);


//	pdglColor4f(1, 1, 1, 1.0-(self->time/self->lim));
	pdglColor4f(
		(self->clr&0xFF)*(1.0/255.0),
		(self->clr&0xFF00)*(1.0/65280.0),
		(self->clr&0xFF0000)*(1.0/16711680.0),
		1.0-(self->time/self->lim));

	pdglBegin(PDGL_QUADS);

	V3F_ADDSCALE(self->org, lbxgl_cam->rt, -self->rad, v);
	V3F_ADDSCALE(v, lbxgl_cam->up, self->rad, v);
	pdglTexCoord2f(0, 1);
	pdglVertex3fv(v);

	V3F_ADDSCALE(self->org, lbxgl_cam->rt, -self->rad, v);
	V3F_ADDSCALE(v, lbxgl_cam->up, -self->rad, v);
	pdglTexCoord2f(0, 0);
	pdglVertex3fv(v);

	V3F_ADDSCALE(self->org, lbxgl_cam->rt, self->rad, v);
	V3F_ADDSCALE(v, lbxgl_cam->up, -self->rad, v);
	pdglTexCoord2f(1, 0);
	pdglVertex3fv(v);

	V3F_ADDSCALE(self->org, lbxgl_cam->rt, self->rad, v);
	V3F_ADDSCALE(v, lbxgl_cam->up, self->rad, v);
	pdglTexCoord2f(1, 1);
	pdglVertex3fv(v);

	pdglEnd();

	glDepthMask(1);
//	glEnable(GL_DEPTH_TEST);
}
#endif

#if 0
void lbxgl_part_sprite_draw_light(LBXGL_Particle *self)
{
	float v[3];

	glDepthMask(0);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, self->tex);


//	pdglColor4f(1, 1, 1, (1.0-(self->time/self->lim))*0.25);
	pdglColor4f(
		(self->clr&0xFF)*(1.0/255.0),
		(self->clr&0xFF00)*(1.0/65280.0),
		(self->clr&0xFF0000)*(1.0/16711680.0),
		(1.0-(self->time/self->lim))*0.25);

	pdglBegin(PDGL_QUADS);

	V3F_ADDSCALE(self->org, lbxgl_cam->rt, -self->rad, v);
	V3F_ADDSCALE(v, lbxgl_cam->up, self->rad, v);
	pdglTexCoord2f(0, 1);
	pdglVertex3fv(v);

	V3F_ADDSCALE(self->org, lbxgl_cam->rt, -self->rad, v);
	V3F_ADDSCALE(v, lbxgl_cam->up, -self->rad, v);
	pdglTexCoord2f(0, 0);
	pdglVertex3fv(v);

	V3F_ADDSCALE(self->org, lbxgl_cam->rt, self->rad, v);
	V3F_ADDSCALE(v, lbxgl_cam->up, -self->rad, v);
	pdglTexCoord2f(1, 0);
	pdglVertex3fv(v);

	V3F_ADDSCALE(self->org, lbxgl_cam->rt, self->rad, v);
	V3F_ADDSCALE(v, lbxgl_cam->up, self->rad, v);
	pdglTexCoord2f(1, 1);
	pdglVertex3fv(v);

	pdglEnd();

	glDepthMask(1);
}
#endif

#if 0
void lbxgl_part_sprite_draw_lightexpose(LBXGL_Particle *self)
{
	float v[3], pt0[3], pt1[3], pt2[3], pt3[3];
	float g, h;

	glDepthMask(0);
//	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	glBlendFunc(GL_SRC_COLOR, GL_ONE);

	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, self->tex);


//	pdglColor4f(1, 1, 1, (1.0-(self->time/self->lim))*0.25);
//	pdglColor4f(
//		(self->clr&0xFF)*(1.0/255.0),
//		(self->clr&0xFF00)*(1.0/65280.0),
//		(self->clr&0xFF0000)*(1.0/16711680.0),
//		(1.0-(self->time/self->lim))*0.25);

	V3F_ADDSCALE(self->org, lbxgl_cam->rt, -self->rad, v);
	V3F_ADDSCALE(v, lbxgl_cam->up, self->rad, pt0);

	V3F_ADDSCALE(self->org, lbxgl_cam->rt, -self->rad, v);
	V3F_ADDSCALE(v, lbxgl_cam->up, -self->rad, pt1);

	V3F_ADDSCALE(self->org, lbxgl_cam->rt, self->rad, v);
	V3F_ADDSCALE(v, lbxgl_cam->up, -self->rad, pt2);

	V3F_ADDSCALE(self->org, lbxgl_cam->rt, self->rad, pt3);
	V3F_ADDSCALE(v, lbxgl_cam->up, self->rad, pt3);

//	g=(1.0-(self->time/self->lim))*0.25;
	g=1.0-(self->time/self->lim);
	h=1.0/lbxgl_part_expose;
	if(h<1.0)h=1.0;
	
	h*=g;

//	pdglBegin(PDGL_QUADS);

#if 0
	pdglColor4f(
		(self->clr&0xFF)*(1.0/255.0),
		(self->clr&0xFF00)*(1.0/65280.0),
		(self->clr&0xFF0000)*(1.0/16711680.0),
		1);

	while(h>1.0)
	{
//		pdglBegin(PDGL_QUADS);
		pdglTexCoord2f(0, 1);
		pdglVertex3fv(pt0);
		pdglTexCoord2f(0, 0);
		pdglVertex3fv(pt1);
		pdglTexCoord2f(1, 0);
		pdglVertex3fv(pt2);
		pdglTexCoord2f(1, 1);
		pdglVertex3fv(pt3);
//		pdglEnd();
		
		h-=1.0;
	}
#endif

	pdglColor4f(
		h*(self->clr&0xFF)*(1.0/255.0),
		h*(self->clr&0xFF00)*(1.0/65280.0),
		h*(self->clr&0xFF0000)*(1.0/16711680.0),
		1);

	pdglBegin(PDGL_QUADS);

	pdglTexCoord2f(0, 1);
	pdglVertex3fv(pt0);

	pdglTexCoord2f(0, 0);
	pdglVertex3fv(pt1);

	pdglTexCoord2f(1, 0);
	pdglVertex3fv(pt2);

	pdglTexCoord2f(1, 1);
	pdglVertex3fv(pt3);

	pdglEnd();

	glDepthMask(1);
}
#endif

void lbxgl_part_flame_step(LBXGL_Particle *self, float dt)
{
	float dv[3];

	self->time+=dt;
	if(self->time>self->lim)
	{
		LBXGL_Particle_Free(self);
		return;
	}

	V3F_ADDSCALE(self->org, self->vel, dt, self->org);
	self->vel[2]+=98*dt;	//g = approx 392in/s2, rise at 0.25g

	LBXGL_Particle_CalcPsiPart(self, dv);
	V3F_ADDSCALE(self->vel, dv, dt, self->vel);
}

void lbxgl_part_fall_step(LBXGL_Particle *self, float dt)
{
	float dv[3];

	self->time+=dt;
	if(self->time>self->lim)
	{
		LBXGL_Particle_Free(self);
		return;
	}

	V3F_ADDSCALE(self->org, self->vel, dt, self->org);
	self->vel[2]-=98*dt;	//g = approx 392in/s2, rise at 0.25g

	LBXGL_Particle_CalcPsiPart(self, dv);
	V3F_ADDSCALE(self->vel, dv, dt, self->vel);
}

void lbxgl_part_colorjitter(LBXGL_Particle *self,
	float scr, float scg, float scb)
{
	float f, g, h;
	int i;

	f=(self->clr&0xFF)*(1.0/255.0);
	g=(self->clr&0xFF00)*(1.0/65280.0);
	h=(self->clr&0xFF0000)*(1.0/16711680.0);
	f=f*((1.0-scr)+bgbrng_randg()*scr);
	g=g*((1.0-scg)+bgbrng_randg()*scg);
	h=h*((1.0-scb)+bgbrng_randg()*scb);
	f=(f<0)?0:((f>1)?1:f);
	g=(g<0)?0:((g>1)?1:g);
	h=(h<0)?0:((h>1)?1:h);
	i=h*255;
	i+=(i<<8)+(g*255);
	i+=(i<<8)+(f*255);
	self->clr=i;
}

void lbxgl_part_flame_spawn(LBXGL_Particle *self, float *org, float *vel)
{
	float f, g;
	V3F_COPY(org, self->org);
	V3F_COPY(vel, self->vel);

	self->org[0]+=1*bgbrng_randg();
	self->org[1]+=1*bgbrng_randg();
	self->org[2]+=1*bgbrng_randg();

	f=V3F_LEN(vel);
	if(f<1)f=1;
	g=f*0.33+12;

	self->vel[0]+=g*bgbrng_randg();
	self->vel[1]+=g*bgbrng_randg();
	self->vel[2]+=g*bgbrng_randg();

	if(!self->clr)self->clr=0xFFFFFF;
	lbxgl_part_colorjitter(self, 0.1, 0.1, 0.1);

//	self->psi_lin=64;
//	self->psi_quad=16;

	self->time=0;
	self->lim=1.0;
//	self->tex=lbxgl_part_flame;
	self->tex=lbxgl_part_fpart[rand()&3];
	self->rad=5.0+(3*bgbrng_randg());
}

void lbxgl_part_flame2_spawn(LBXGL_Particle *self, float *org, float *vel)
{
	float f, g;
	V3F_COPY(org, self->org);
	V3F_COPY(vel, self->vel);

	self->org[0]+=1*bgbrng_randg();
	self->org[1]+=1*bgbrng_randg();
	self->org[2]+=1*bgbrng_randg();

	f=V3F_LEN(vel);
	if(f<1)f=1;
	g=f*0.33;

	self->vel[0]+=g*bgbrng_randg();
	self->vel[1]+=g*bgbrng_randg();
	self->vel[2]+=g*bgbrng_randg();

	if(!self->clr)self->clr=0xFFFFFF;
	lbxgl_part_colorjitter(self, 0.1, 0.1, 0.1);

	self->time=0;
	self->lim=1.0+(0.25*bgbrng_randg());
	self->tex=lbxgl_part_fpart[rand()&3];
	self->rad=5.0+(3*bgbrng_randg());
}


void lbxgl_part_flame_blue_step(LBXGL_Particle *self, float dt)
{
	self->time+=dt;
	if(self->time>self->lim)
	{
		LBXGL_Particle_Free(self);
		return;
	}

	V3F_ADDSCALE(self->org, self->vel, dt, self->org);
	self->vel[2]+=25*dt;	//g = approx 392in/s2, rise at 0.25g
}

void lbxgl_part_flame_blue_spawn(LBXGL_Particle *self, float *org, float *vel)
{
	float f, g;

	lbxgl_part_flame_spawn(self, org, vel);

	V3F_COPY(vel, self->vel);
	V3F_COPY(org, self->org);
	self->org[0]+=0.25*bgbrng_randg();
	self->org[1]+=0.25*bgbrng_randg();
	self->org[2]+=0.25*bgbrng_randg();


	self->time=0;
	self->lim=1.0+(0.25*bgbrng_randg());
//	self->tex=lbxgl_part_flame_blue;
	self->tex=lbxgl_part_fbpart[rand()&3];
	self->rad=5.0+(3*bgbrng_randg());
}

void lbxgl_part_smoke_step(LBXGL_Particle *self, float dt)
{
	self->time+=dt;
	if(self->time>self->lim)
	{
		LBXGL_Particle_Free(self);
		return;
	}

	V3F_ADDSCALE(self->org, self->vel, dt, self->org);
	self->vel[2]+=24*dt;	//g = approx 392in/s2, rise at 0.0625g
}

void lbxgl_part_smoke_spawn(LBXGL_Particle *self, float *org, float *vel)
{
	float f, g;

	V3F_COPY(org, self->org);
	V3F_COPY(vel, self->vel);

	self->org[0]+=1*bgbrng_randg();
	self->org[1]+=1*bgbrng_randg();
	self->org[2]+=1*bgbrng_randg();

	f=V3F_LEN(vel);
	if(f<1)f=1;
	g=f*0.33;

	self->vel[0]+=g*bgbrng_randg();
	self->vel[1]+=g*bgbrng_randg();
	self->vel[2]+=g*bgbrng_randg();

	self->time=0;
	self->lim=5.0+(3*bgbrng_randg());
	self->tex=lbxgl_part_smoke;
	self->rad=12.0+(6*bgbrng_randg());
}

void lbxgl_part_smoke_light_spawn(LBXGL_Particle *self, float *org, float *vel)
{
	float f, g;

	V3F_COPY(org, self->org);
	V3F_COPY(vel, self->vel);

	self->org[0]+=1*bgbrng_randg();
	self->org[1]+=1*bgbrng_randg();
	self->org[2]+=1*bgbrng_randg();

	f=V3F_LEN(vel);
	if(f<1)f=1;
	g=f*0.33;

	self->vel[0]+=g*bgbrng_randg();
	self->vel[1]+=g*bgbrng_randg();
	self->vel[2]+=g*bgbrng_randg();

	self->time=0;
	self->lim=2.5+(1.5*bgbrng_randg());
	self->tex=lbxgl_part_smoke;
	self->rad=4.0+(2*bgbrng_randg());
}

void lbxgl_part_generic_spawn(LBXGL_Particle *self, float *org, float *vel)
{
	float f, g;

	V3F_COPY(org, self->org);
	V3F_COPY(vel, self->vel);

	self->org[0]+=1*bgbrng_randg();
	self->org[1]+=1*bgbrng_randg();
	self->org[2]+=1*bgbrng_randg();

	f=V3F_LEN(vel);
	if(f<1)f=1;
	g=f*0.33;

	self->vel[0]+=g*bgbrng_randg();
	self->vel[1]+=g*bgbrng_randg();
	self->vel[2]+=g*bgbrng_randg();

	self->time=0;
	self->lim=2.5+(1.5*bgbrng_randg());
	self->tex=lbxgl_part_generic;
	self->rad=4.0+(2*bgbrng_randg());
}

void lbxgl_part_square_spawn(LBXGL_Particle *self, float *org, float *vel)
{
	float f, g;

	V3F_COPY(org, self->org);
	V3F_COPY(vel, self->vel);

	self->org[0]+=1*bgbrng_randg();
	self->org[1]+=1*bgbrng_randg();
	self->org[2]+=1*bgbrng_randg();

	f=V3F_LEN(vel);
	if(f<1)f=1;
	g=f*0.33;

	self->vel[0]+=g*bgbrng_randg();
	self->vel[1]+=g*bgbrng_randg();
	self->vel[2]+=g*bgbrng_randg();

	self->time=0;
	self->lim=2.5+(1.5*bgbrng_randg());
	self->tex=lbxgl_part_square;
	self->rad=4.0+(2*bgbrng_randg());
}

void lbxgl_part_gib_spawn(LBXGL_Particle *self, float *org, float *vel)
{
	float f, g;

	lbxgl_part_flame_spawn(self, org, vel);

	V3F_COPY(vel, self->vel);
	V3F_COPY(org, self->org);
	self->org[0]+=0.25*bgbrng_randg();
	self->org[1]+=0.25*bgbrng_randg();
	self->org[2]+=0.25*bgbrng_randg();

	self->time=0;
	self->lim=10.0+(0.25*bgbrng_randg());
//	self->tex=lbxgl_part_flame_blue;
	self->tex=lbxgl_part_gibpart[rand()&3];
	self->rad=5.0+(3*bgbrng_randg());
}

void lbxgl_part_metgib_spawn(LBXGL_Particle *self, float *org, float *vel)
{
	float f, g;

	lbxgl_part_flame_spawn(self, org, vel);

	V3F_COPY(vel, self->vel);
	V3F_COPY(org, self->org);
	self->org[0]+=0.25*bgbrng_randg();
	self->org[1]+=0.25*bgbrng_randg();
	self->org[2]+=0.25*bgbrng_randg();

	self->time=0;
	self->lim=10.0+(0.25*bgbrng_randg());
//	self->tex=lbxgl_part_flame_blue;
	self->tex=lbxgl_part_mtgpart[rand()&7];
	self->rad=5.0+(3*bgbrng_randg());
}


int shader_flame_orgvel(int nvec, float *xyz, float *vn, float pwr,
	float *org, float *vel)
{
	float tmat[16], tv[3];

	float *v;
	float f;
	int i;

	V3F_ZERO(org);
	for(i=0; i<nvec; i++)	{ v=xyz+i*4; V3F_ADD(org, v, org); }
	V3F_SCALE(org, 1.0/nvec, org);

	V3F_ZERO(vel);
	for(i=0; i<nvec; i++)	{ v=vn+i*4; V3F_ADD(vel, v, vel); }
	V3F_NORMALIZE(vel, vel);

	f=pwr*(bgbrng_randf()+0.5);
	V3F_SCALE(vel, f, vel);

#if 1
//	glGetFloatv(GL_MODELVIEW_MATRIX, tmat);
	pdglGetModelviewMatrix(tmat);
	tv[0]=V3F_DOT(org, tmat+0)+tmat[12];
	tv[1]=V3F_DOT(org, tmat+4)+tmat[13];
	tv[2]=V3F_DOT(org, tmat+8)+tmat[14];
	V3F_COPY(tv, org);

	tv[0]=V3F_DOT(vel, tmat+0);
	tv[1]=V3F_DOT(vel, tmat+4);
	tv[2]=V3F_DOT(vel, tmat+8);
	V3F_COPY(tv, vel);
#endif
	return(0);
}

LBXGL_API int shader_flame(LBXGL_TexImage *sdr, int lnum, int nvec,
	float *xyz, float *st, float *vn, float *clr)
{
	float org[3], vel[3];
	float f;

	shader_flame_orgvel(nvec, xyz, vn, 2.0, org, vel);

//	f=100*bgbrng_randf()*pdgl_dt_f;
//	f=10*krandf()+1;

	f=10*bgbrng_randf()*lbxgl_state->dt_f;

	if(f<1.0)f=1.0;

	LBXGL_Particle_Spawn("flame", (int)f, org, vel);
	return(0);
}


LBXGL_API int shader_flame_blue(LBXGL_TexImage *sdr, int lnum, int nvec,
	float *xyz, float *st, float *vn, float *clr)
{
	float org[3], vel[3], *v;
	float f;
	int i;

	shader_flame_orgvel(nvec, xyz, vn, 98.0, org, vel);

//	f=250*bgbrng_randf()*pdgl_dt_f;
//	f=10*krandf()+1;
	f=50*bgbrng_randf()*lbxgl_state->dt_f;
	if(f<1.0)f=1.0;

	LBXGL_Particle_Spawn("flame_blue", (int)f, org, vel);
	return(0);
}

LBXGL_API int shader_smoke(LBXGL_TexImage *sdr, int lnum, int nvec,
	float *xyz, float *st, float *vn, float *clr)
{
	float org[3], vel[3], *v;
	float f;
	int i;

	shader_flame_orgvel(nvec, xyz, vn, 2.0, org, vel);

	f=50*bgbrng_randf()*lbxgl_state->dt_f;
//	f=10*krandf()+1;
	if(f<1.0)f=1.0;

	LBXGL_Particle_Spawn("smoke", (int)f, org, vel);
	return(0);
}

LBXGL_API int shader_smoke_light(LBXGL_TexImage *sdr, int lnum, int nvec,
	float *xyz, float *st, float *vn, float *clr)
{
	float org[3], vel[3], *v;
	float f;
	int i;

	shader_flame_orgvel(nvec, xyz, vn, 2.0, org, vel);

	f=10*bgbrng_randf()*lbxgl_state->dt_f;
//	f=10*krandf()+1;
	if(f<1.0)f=1.0;

	LBXGL_Particle_Spawn("smoke_light", (int)f, org, vel);
	return(0);
}

LBXGL_API int texgen_maze(LBXGL_TexImage *sdr)
{
	static byte buf1[64*64*4];
	static byte pal[768]=
	{
	  0,  0,  0,	//0
	255,255,255,	//1
	 64, 64, 64,	//2
	255,  0,  0,	//3
	  0,255,  0,	//4
	  0, 64, 64,	//5
	 64,  0, 64,	//6
	 64, 64,  0,	//7
	};
	byte buf[64*64];
	int i, j, sz;
	
	sz=16;
	BTGE_GenerateMaze(buf, sz, sz, rand(), 0);
	
	for(i=0; i<sz*sz; i++)
	{
		j=buf[i];
		buf1[i*4+0]=pal[j*3+0];
		buf1[i*4+1]=pal[j*3+1];
		buf1[i*4+2]=pal[j*3+2];
		buf1[i*4+3]=255;
	}
	
	j=Tex_LoadTexture(sz, sz, buf1, 2|16);
	return(j);
}
