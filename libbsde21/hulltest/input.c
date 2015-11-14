#include <windows.h>

#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include <pdgl_keys.h>

typedef struct keyhandler_s keyhandler;
struct keyhandler_s {
keyhandler *next;
int (*func)(int key, int down);
};

static byte pdgl_keymap[32];
static unsigned short pdgl_keybuf[64];
static int pdgl_keybuf_pos;

keyhandler *pdgl_keyhandlers=NULL;

static int keyb_num_shift[10]={
')', '!', '@', '#', '$', '%', '^', '&', '*', '('
};

int PDGL_AddKeyHandler(int (*fcn)(int key, int down))
{
	keyhandler *tmp;

	tmp=malloc(sizeof(keyhandler));
	tmp->func=fcn;

	tmp->next=pdgl_keyhandlers;
	pdgl_keyhandlers=tmp;

	return(0);
}

int PDGL_KeyDown(int key)
{
	if(pdgl_keymap[key>>3]&(1<<(key&7)))return(1);
	return(0);
}

int GfxDev_Key_Event(int key, int down)
{
	int akey;
	keyhandler *cur;

	akey=key;
	if(PDGL_KeyDown(K_SHIFT))
	{
		if(key>='a' && key<='z')
			akey=key-'a'+'A';
		if(key>='0' && key<='9')
			akey=keyb_num_shift[key-'0'];

		if(key=='\'')akey='"';
		if(key=='[')akey='{';
		if(key==']')akey='}';
		if(key=='-')akey='_';
		if(key=='=')akey='+';
		if(key==';')akey=':';
		if(key==',')akey='<';
		if(key=='.')akey='>';
		if(key=='/')akey='?';
		if(key=='`')akey='~';
		if(key=='\\')akey='|';
	}

	if(down)
	{
		pdgl_keymap[akey>>3]|=1<<(akey&7);
		if(pdgl_keybuf_pos<63)
		{
			pdgl_keybuf[pdgl_keybuf_pos++]=akey;
			pdgl_keybuf[pdgl_keybuf_pos]=0;
		}

		cur=pdgl_keyhandlers;
		while(cur)
		{
			cur->func(akey, 1);
			cur=cur->next;
		}
	}else
	{
		pdgl_keymap[akey>>3]&=~(1<<(akey&7));
		if(pdgl_keybuf_pos<63)
		{
			pdgl_keybuf[pdgl_keybuf_pos++]=akey|0x8000;
			pdgl_keybuf[pdgl_keybuf_pos]=0;
		}

		cur=pdgl_keyhandlers;
		while(cur)
		{
			cur->func(akey, 0);
			cur=cur->next;
		}
	}
	return(0);
}

unsigned short *PDGL_GetKeybuf()
{
	static unsigned short buf[128];

	memcpy(buf, pdgl_keybuf, 128);

	pdgl_keybuf_pos=0;
	pdgl_keybuf[0]=0;

	return(buf);
}


int PDGL_TimeMS()
{
#if 1
	int t;

	t=clock();
	t*=CLOCKS_PER_SEC/1000.0;

	return(t);
#endif
#if 0
	struct timeval tp;
	static int secbase; 

	gettimeofday(&tp, NULL);  
	if(!secbase)secbase=tp.tv_sec;
	return(((tp.tv_sec-secbase)*1000)+tp.tv_usec/1000);
#endif
}

int PDGL_DeltaMS(int *l)
{
	int t, dt;

	t=PDGL_TimeMS();
	if(!*l)*l=t;
	dt=t-(*l);
	*l=t;
	return(dt);
}
