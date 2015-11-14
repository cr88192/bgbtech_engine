#include <lbxgl2.h>
#include <btgecl.h>

BTCL_World *btcl_world;
PDGLUI_InputState *btcl_state;
PDGLUI_Camera *btcl_cam;
LBXGL_BrushWorld *btcl_brushWorld;
BTCL_MessageHandler *btcl_handler;
float btcl_dt;

BTCL_API dyt BTCL_SendMessage(BTCL_World *wrl, dyt msg)
{
	wrl->msg_queue=dynappend(wrl->msg_queue, dylist1(msg));
}

BTCL_API dyt BTCL_ComposeBrushDelta(BTCL_World *wrl, LBXGL_Brush *brush)
{
	dyt t, l, l1;
	int i;
	
	if(!brush->idnum)
	{
		btcl_brushWorld->map_stat_brushes++;
		i=btcl_brushWorld->map_stat_brushes;
		brush->idnum=i;
		brush->flags|=BTGE_BRFL_NEW;
	}
	
	if(brush->flags&BTGE_BRFL_DEL)
	{
		t=dylist2s("delbrush", dyint(brush->idnum));
		return(t);
	}

	l=NULL;
	for(i=0; i<brush->n_face; i++)
	{
		l1=NULL;
		t=dylist5s("norm",
			dyflonum(brush->norm[i*4+0]),
			dyflonum(brush->norm[i*4+1]),
			dyflonum(brush->norm[i*4+2]),
			dyflonum(brush->norm[i*4+3]));
		l1=dycons(t, l1);

		t=dylist5s("sdir",
			dyflonum(brush->sdir[i*4+0]),
			dyflonum(brush->sdir[i*4+1]),
			dyflonum(brush->sdir[i*4+2]),
			dyflonum(brush->sdir[i*4+3]));
		l1=dycons(t, l1);

		t=dylist5s("tdir",
			dyflonum(brush->tdir[i*4+0]),
			dyflonum(brush->tdir[i*4+1]),
			dyflonum(brush->tdir[i*4+2]),
			dyflonum(brush->tdir[i*4+3]));
		l1=dycons(t, l1);

		t=dylist2s("texname", dystring(brush->tex[i]));
		l1=dycons(t, l1);

		t=dycons2s("face", dyint(i), l1);
		l=dycons(t, l);
	}

	if(brush->flags&BTGE_BRFL_NEW)
	{
		t=dycons3s("newbrush",
			dyint(brush->se_owner->idnum),
			dyint(brush->idnum), l);
		brush->flags&=~BTGE_BRFL_NEW;
		return(t);
	}

	if(!l)return(NULL);
	t=dycons2s("bdelta", dyint(brush->idnum), l);
	return(t);
}

BTCL_API dyt BTCL_ComposeWorldDelta(BTCL_World *wrl)
{
	LBXGL_Brush *cur;
	LBXGL_SEntity *scur;
	BTGE_SEntPair *kcur;
	dyt t, l, l1;
	int i;

	l=NULL;
 	
	cur=btcl_brushWorld->brush;
	while(cur)
	{
		if(cur->flags&BTGE_BRFL_TRANSIENT)
		{
			cur=cur->next;
			continue;
		}
	
		if(!cur->idnum)
		{
			btcl_brushWorld->map_stat_brushes++;
			i=btcl_brushWorld->map_stat_brushes;
			cur->idnum=i;
			cur->flags|=BTGE_BRFL_NEW|BTGE_BRFL_DIRTY;
		}

		if(cur->flags&BTGE_BRFL_DIRTY)
		{
			t=BTCL_ComposeBrushDelta(wrl, cur);
			if(t)l=dycons(t, l);
//			t=dylist2s("bdelta", dyint(cur->idnum));

			cur->flags&=~BTGE_BRFL_DIRTY;
		}

		cur=cur->next;
	}

	scur=btcl_brushWorld->entity;
	while(scur)
	{
		if(!scur->idnum)
		{
			btcl_brushWorld->map_stat_ents++;
			i=btcl_brushWorld->map_stat_ents;
			scur->idnum=i;

			kcur=scur->keys; l1=NULL;
			while(kcur)
			{
				t=dylist3s("key",
					dystring(kcur->name),
					dystring(kcur->value));
				l1=dycons(t, l1);
				
				kcur->flags&=~BTGE_SEFL_DIRTY;
				kcur=kcur->next;
			}

			scur->flags&=~BTGE_SEFL_DIRTY;

			t=dycons2s("newsent", dyint(i), l1);
			l=dycons(t, l);

			scur=scur->next;
			continue;
		}

		if(scur->flags&BTGE_SEFL_DIRTY)
		{
//			btcl_brushWorld->map_stat_ents++;
//			i=btcl_brushWorld->map_stat_ents;
//			scur->idnum=i;

			kcur=scur->keys; l1=NULL;
			while(kcur)
			{
				t=dylist3s("key",
					dystring(kcur->name),
					dystring(kcur->value));
				l1=dycons(t, l1);
				
				kcur->flags&=~BTGE_SEFL_DIRTY;
				kcur=kcur->next;
			}

			scur->flags&=~BTGE_SEFL_DIRTY;

			i=scur->idnum;
			t=dycons2s("sedelta", dyint(i), l1);
			l=dycons(t, l);

			scur=scur->next;
			continue;
		}

		scur=scur->next;
	}

	if(!l)return(NULL);
	
	t=dyconss("wdelta", l);
	printf("BTCL_ComposeWorldDelta\n");
	dysPrintln(t);

	return(t);
}

BTCL_API dyt BTCL_ComposeUpdate(BTCL_World *wrl)
{
	char tb[256];
	float tv[3];
	PDGLUI_InputState *state;
	PDGLUI_Camera *cam;
	dyt t, lst;
	char *ct;
	int i, j;
	
	state=PDGLUI_GetState();
	cam=PDGLUI_GetCamera();
	
//	lst=NULL;
	lst=wrl->msg_queue;
	wrl->msg_queue=NULL;

	//frame delta time
	t=dylist2s("framedt", dyflonum(state->dt_f));
	lst=dycons(t, lst);

	//time client thinks it is on server
	t=dylist2s("svtime", dyflonum(btclTime(wrl)));
	lst=dycons(t, lst);

	//current time on client
	t=dylist2s("cltime", dyflonum(btclClientTime()));
	lst=dycons(t, lst);

	//mouse state
	t=dylist4s("mpos",
		dyint(state->mx),
		dyint(state->my),
		dyint(state->mb));
	lst=dycons(t, lst);

	//last mouse state
	t=dylist4s("lmpos",
		dyint(state->lmx),
		dyint(state->lmy),
		dyint(state->lmb));
	lst=dycons(t, lst);

	//impulse angle (mouse movement)
	t=dylist4s("iang",
		dyflonum(cam->iang[0]),
		dyflonum(cam->iang[1]),
		dyflonum(cam->iang[2]));
	lst=dycons(t, lst);

	//current view origin
	t=dylist4s("org",
		dyflonum(cam->org[0]),
		dyflonum(cam->org[1]),
		dyflonum(cam->org[2]));
	lst=dycons(t, lst);

	//current view angle
	t=dylist4s("ang",
		dyflonum(cam->ang[0]),
		dyflonum(cam->ang[1]),
		dyflonum(cam->ang[2]+90));
	lst=dycons(t, lst);

	//impulse velocity
//	V3F_COPY(cam->ivel, tv);
	V3F_COPY(cam->ipvel, tv);
	
	tv[2]=0;
	
	V3F_NORMALIZE(tv, tv);
	V3F_SCALE(tv, state->dt_f*cam->speed, tv);
	
	tv[2]=cam->izvel;	//require explicit up/down movement

	t=dylist4s("ivel",
		dyflonum(tv[0]/state->dt_f),
		dyflonum(tv[1]/state->dt_f),
		dyflonum(tv[2]/state->dt_f));
	lst=dycons(t, lst);

	t=dylist4s("ivel2",
		dyflonum(cam->ivel[0]/state->dt_f),
		dyflonum(cam->ivel[1]/state->dt_f),
		dyflonum(cam->ivel[2]/state->dt_f));
	lst=dycons(t, lst);


	t=dylist4s("morg",
		dyflonum(cam->morg[0]),
		dyflonum(cam->morg[1]),
		dyflonum(cam->morg[2]));
	lst=dycons(t, lst);

	t=dylist4s("mend",
		dyflonum(cam->mend[0]),
		dyflonum(cam->mend[1]),
		dyflonum(cam->mend[2]));
	lst=dycons(t, lst);

	t=dylist4s("mdir",
		dyflonum(cam->mdir[0]),
		dyflonum(cam->mdir[1]),
		dyflonum(cam->mdir[2]));
	lst=dycons(t, lst);

//	if(wrl->impfl)
//		printf("BTCL_ComposeUpdate: impfl=%04X\n", wrl->impfl);

	t=dylist2s("impfl", dyint(wrl->impfl));
	lst=dycons(t, lst);

	if(wrl->clientflags!=wrl->oldclientflags)
	{
		t=dylist2s("clfl", dyint(wrl->clientflags));
		lst=dycons(t, lst);
		wrl->oldclientflags=wrl->clientflags;
	}

	if(wrl->viewflags&BT_EF2_CAM_GUIMODE)
	{
		ct=tb;
		for(i=0; state->keys[i]; i++)
			{ BGBDY_EmitChar(&ct, state->keys[i]); }
		*ct++=0;

		if(tb[0])
		{
			t=dylist2s("keys", dystring(tb));
			lst=dycons(t, lst);
		}

//		if(*state->keys)
//		{
//			t=dylist2s("keys", dywstring(state->keys));
//			lst=dycons(t, lst);
//		}
		
		ct=tb;
		for(i=1; i<256; i++)
			if(state->keymap[i>>3]&(1<<(i&7)))
				{ BGBDY_EmitChar(&ct, i); }
		*ct++=0;
		
		if(tb[0])
		{
			t=dylist2s("keybuf", dystring(tb));
			lst=dycons(t, lst);
		}
	}

	t=BTCL_ComposeWorldDelta(wrl);
	if(t)lst=dycons(t, lst);

	lst=BTCLUI_ComposeUpdate(wrl, lst);

	t=dycons2s("impulse", dyint(wrl->camnum), lst);
	return(t);
}
