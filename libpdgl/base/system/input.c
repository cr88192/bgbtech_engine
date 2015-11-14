#include <pdgl.h>
#include <bgbgc.h>
#include <pdgl/keys.h>

#include <time.h>
// #include <sys/time.h>

typedef struct keyhandler_s keyhandler;
struct keyhandler_s {
keyhandler *next;
int (*func)(int key, int down);
};

static byte pdgl_keymap[32];
static unsigned short pdgl_keybuf2[64];
static int pdgl_keybuf2_pos;

keyhandler *pdgl_keyhandlers=NULL;

static int keyb_num_shift[10]={
')', '!', '@', '#', '$', '%', '^', '&', '*', '('
};

/*--
Cat lbxgl;C Interface
Form
	int PDGL_AddKeyHandler(int (*fcn)(int key, int down));
Description
	Adds a keystroke handler.
--*/
PDGL_API int PDGL_AddKeyHandler(int (*fcn)(int key, int down))
{
	keyhandler *tmp;

	tmp=gcalloc(sizeof(keyhandler));
	tmp->func=fcn;

	tmp->next=pdgl_keyhandlers;
	pdgl_keyhandlers=tmp;

	return(0);
}

/*--
Cat lbxgl;C Interface
Form
	int PDGL_KeyDown(int key);
Description
	Returns non-zero if a given key is pressed.
--*/
PDGL_API int PDGL_KeyDown(int key)
{
	if(pdgl_keymap[key>>3]&(1<<(key&7)))return(1);
	return(0);
}

PDGL_API int PDGL_GetKeyMap(byte *map)
{
	memset(map, pdgl_keymap, 32);
	return(0);
}

/*--
Cat lbxgl;C Interface
Form
	int GfxDev_Key_Event(int key, int down);
Description
	Called by key handler to indicate a status change for a given key.
Status Internal
--*/
PDGL_API int GfxDev_Key_Event(int key, int down)
{
	static int skm=0, skmd=0;
	int akey, akey2, nskm;
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

	nskm=0;
	if(PDGL_KeyDown(K_CTRL))
	{
		switch(akey)
		{
		case K_F1: nskm=K_SKM_F1; break;
		case K_F2: nskm=K_SKM_F2; break;
		case K_F3: nskm=K_SKM_F3; break;
		case K_F4: nskm=K_SKM_F4; break;
		case K_F5: nskm=K_SKM_F5; break;
		case K_F6: nskm=K_SKM_F6; break;
		case K_F7: nskm=K_SKM_F7; break;
		case K_F8: nskm=K_SKM_F8; break;
		case K_F9: nskm=K_SKM_F9; break;
		case K_F10: nskm=K_SKM_F10; break;
		case K_F11: nskm=K_SKM_F11; break;
		case K_F12: nskm=K_SKM_F12; break;
		default: break;
		}
	}else if(!skm)
	{
		if(!PDGL_KeyDown(K_CTRL) && !PDGL_KeyDown(K_ALT) &&
			!PDGL_KeyDown(K_SHIFT))
		{
			if(akey==K_ESC)
				nskm=K_SKM_ESC;
		}
	}

	if(down)
	{
		akey2=akey;
		if(skm && (akey>=' ') && (akey<='~'))
		{
			akey2=akey|skm;
			skmd=skm;
//			skm=0;
		}
	
		pdgl_keymap[akey>>3]|=1<<(akey&7);
		if(pdgl_keybuf2_pos<63)
		{
			pdgl_keybuf2[pdgl_keybuf2_pos++]=akey2;
			pdgl_keybuf2[pdgl_keybuf2_pos]=0;
		}

		cur=pdgl_keyhandlers;
		while(cur)
		{
			cur->func(akey2, 1);
			cur=cur->next;
		}
	}else
	{
		akey2=akey;
		if(skm && skmd && (akey>=' ') && (akey<='~'))
		{
			akey2=akey|skm;
			skm=0; skmd=0;
		}
	
		pdgl_keymap[akey>>3]&=~(1<<(akey&7));
		if(pdgl_keybuf2_pos<63)
		{
			pdgl_keybuf2[pdgl_keybuf2_pos++]=akey2|0x8000;
			pdgl_keybuf2[pdgl_keybuf2_pos]=0;
		}

		cur=pdgl_keyhandlers;
		while(cur)
		{
			cur->func(akey2, 0);
			cur=cur->next;
		}
	}
	
	if(nskm)
		{ skm=nskm; }
	return(0);
}

/*--
Cat lbxgl;C Interface
Form
	ushort *PDGL_GetKeybuf();
Description
	Returns the keybuf.
	The lower 8 bits hold the key code, bit 15 is used to flag release.
	The value 0 is used as a list terminator.
--*/
PDGL_API short *PDGL_GetKeybuf()
{
	unsigned short *buf;

	buf=gcralloc(128);
	memcpy(buf, pdgl_keybuf2, 128);

	pdgl_keybuf2_pos=0;
	pdgl_keybuf2[0]=0;

//	printf("buf %p\n", buf);

	return(buf);
}


PDGL_API int PDGL_TimeMS()
{
#ifdef _WIN32
	static unsigned int init;
	unsigned int t;

	t=timeGetTime();
	if(!init)init=t;

	return((unsigned int)(t-init));
#else
#ifndef linux
	static int init;
	int t;

	t=clock();
	t*=CLOCKS_PER_SEC/1000.0;

	if(!init)init=t;

	return((unsigned int)(t-init));
#endif
#ifdef linux
	struct timeval	tp;
	static int      secbase; 

	gettimeofday(&tp, NULL);  
	if(!secbase)secbase=tp.tv_sec;
	return(((tp.tv_sec-secbase)*1000)+tp.tv_usec/1000);
#endif
#endif
}

/*--
Cat lbxgl;C Interface
Form
	int PDGL_DeltaMS(int *l);
Description
	Return the current time time in 1ms intervals.
	l is used to hold the last time.
	l is updated to the current time after each call.
	l should initially hold 0.
--*/
PDGL_API int PDGL_DeltaMS(int *l)
{
	int t, dt;

	t=PDGL_TimeMS();
//	if(!*l)*l=t;
	dt=t-(*l);
	*l=t;
	return(dt);
}

void (*pdgl_shutdown[256])();
int pdgl_n_shutdown=0;

PDGL_API void PDGL_RegisterShutdown(void (*fcn)())
{
	int i;
	
	i=pdgl_n_shutdown++;
	pdgl_shutdown[i]=fcn;
}

PDGL_API void PDGL_Shutdown()
{
	int i;

	for(i=0; i<pdgl_n_shutdown; i++)
		pdgl_shutdown[i]();
}
