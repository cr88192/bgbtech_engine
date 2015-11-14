#include <lbxgl2.h>

extern LBXGL_BrushWorld *lbxgl_brush2d_world;


LBXGL_API BTGE_DrawSequence2D_FramePos *
	LBXGL_Draw2D_AllocSequenceFramePos(void)
{
	BTGE_DrawSequence2D_FramePos *tmp;
	tmp=gctalloc("btge_sequence2d_framepos_t",
		sizeof(BTGE_DrawSequence2D_FramePos));
	return(tmp);
}

LBXGL_API BTGE_DrawSequence2D_Frame *
	LBXGL_Draw2D_AllocSequenceFrame(void)
{
	BTGE_DrawSequence2D_Frame *tmp;
	tmp=gctalloc("btge_sequence2d_frame_t",
		sizeof(BTGE_DrawSequence2D_Frame));
	return(tmp);
}

LBXGL_API BTGE_DrawSequence2D *LBXGL_Draw2D_AllocSequence(void)
{
	BTGE_DrawSequence2D *tmp;
	tmp=gctalloc("btge_sequence2d_t", sizeof(BTGE_DrawSequence2D));
	tmp->n_entity=1;
	return(tmp);
}

LBXGL_API void LBXGL_Draw2D_WriteSequence_FramePos(VFILE *fd,
	BTGE_DrawSequence2D_FramePos *frpos, int entid)
{
	vfprint(fd, "entpos %d %f %f %f %f\n", entid,
		v2x(frpos->org), v2y(frpos->org),
		frpos->angle, frpos->scale);
}

LBXGL_API void LBXGL_Draw2D_WriteSequence_Frame(VFILE *fd,
	BTGE_DrawSequence2D_Frame *frame, int frnum)
{
	int i;

	vfprint(fd, "frame %d {\n", frnum);
	
	for(i=0; i<frame->n_pos; i++)
	{
		if(frame->entpos[i])
		{
			LBXGL_Draw2D_WriteSequence_FramePos(fd,
				frame->entpos[i], i);
		}
	}

	vfprint(fd, "}\n");
}

LBXGL_API void LBXGL_Draw2D_WriteSequence_Entity(VFILE *fd,
	BTGE_DrawSequence2D *seq,
	BTGE_DrawableObject2D *ent)
{
	vfprint(fd, "entity %d {\n", ent->entid);

	if(ent->imgname)
		vfprint(fd, "  texture \"%s\"\n", ent->imgname);

	vfprint(fd, "  origin %f %f\n", v2x(ent->org), v2y(ent->org));
	vfprint(fd, "  size %f %f\n", v2x(ent->size), v2y(ent->size));
	vfprint(fd, "  angle %f\n", ent->angle);

	vfprint(fd, "}\n");
}

LBXGL_API void LBXGL_Draw2D_WriteSequence(VFILE *fd,
	BTGE_DrawSequence2D *seq)
{
	BTGE_DrawableObject2D *ecur;
	int i, j, k;
	
	ecur=seq->entity;
	while(ecur)
	{
		LBXGL_Draw2D_WriteSequence_Entity(fd, seq, ecur);
		ecur=ecur->seqnext;
	}
	
	for(i=0; i<seq->n_frame; i++)
	{
		if(seq->frame[i])
		{
			LBXGL_Draw2D_WriteSequence_Frame(fd,
				seq->frame[i], i);
		}
	}
}

LBXGL_API void LBXGL_Draw2D_CheckExpandFrame(
	BTGE_DrawSequence2D *seq, int frnum)
{
	int i;

	if(!seq->frame)
	{
		i=64;
		while(i<=frnum)i=i+(i>>1);
		seq->frame=gcalloc(i*sizeof(BTGE_DrawSequence2D_Frame *));
		seq->m_frame=i;
		seq->n_frame=0;
	}
	
	if(frnum>=seq->m_frame)
	{
		i=seq->m_frame;
		while(i<=frnum)i=i+(i>>1);
		seq->frame=gcrealloc(seq->frame,
			i*sizeof(BTGE_DrawSequence2D_Frame *));
		seq->m_frame=i;
	}
}

LBXGL_API void LBXGL_Draw2D_CheckFrameEntID(
	BTGE_DrawSequence2D_Frame *frame, int entid)
{
	int i;
	
	if(!frame->entpos)
	{
		i=16;
		while(i<=entid)i=i+(i>>1);
		frame->entpos=gcalloc(i*sizeof(BTGE_DrawSequence2D_FramePos *));
		frame->m_pos=i;
	}
	
	if(entid>=frame->m_pos)
	{
		i=frame->m_pos;
		while(i<=entid)i=i+(i>>1);
		frame->entpos=gcrealloc(frame->entpos,
			i*sizeof(BTGE_DrawSequence2D_FramePos *));
		frame->m_pos=i;
	}
}

LBXGL_API void LBXGL_Draw2D_ReadSequence_Frame(VFILE *fd,
	BTGE_DrawSequence2D *seq, int frnum)
{
	char buf[256];
	BTGE_DrawSequence2D_Frame *frame;
	BTGE_DrawSequence2D_FramePos *frpos;
	char **a, *s;
	int i;

	frame=LBXGL_Draw2D_AllocSequenceFrame();

	LBXGL_Draw2D_CheckExpandFrame(seq, frnum);

	seq->frame[frnum]=frame;
	if(frnum>=seq->n_frame)
		seq->n_frame=frnum+1;

	while(!vfeof(fd))
	{
		vfgets(buf, 255, fd);
		a=gcrsplit(buf);
		if(!a[0])continue;

		if(!strcmp(a[0], "}"))
			break;

		if(!strcmp(a[0], "flags"))
		{
		}

		if(!strcmp(a[0], "entpos"))
		{
			frpos=LBXGL_Draw2D_AllocSequenceFramePos();
		
			i=atoi(a[1]);
			LBXGL_Draw2D_CheckFrameEntID(frame, i);
			frame->n_pos=i;
			frame->entpos[i]=frpos;
			
			frpos->org=vec2(atof(a[2]), atof(a[3]));
			frpos->angle=atof(a[4]);
			frpos->scale=atof(a[5]);
			continue;
		}

//		if(!strcmp(a[0], "entity"))
//		{
//			LBXGL_Draw2D_ReadSequence_Entity(fd, seq);
//			continue;
//		}
	}
}

LBXGL_API void LBXGL_Draw2D_ReadSequence_Entity(VFILE *fd,
	BTGE_DrawSequence2D *seq, int entid)
{
	char buf[256];
	BTGE_DrawableObject2D *ent;
	char **a, *s;
	int i;

	ent=LBXGL_BrushDraw2D_CreateObject(lbxgl_brush2d_world);
	ent->entid=entid;
	
	ent->seqnext=seq->entity;
	seq->entity=ent;
	
	if(entid>=seq->n_entity)
		seq->n_entity=entid+1;

	while(!vfeof(fd))
	{
		vfgets(buf, 255, fd);
		a=gcrsplit(buf);
		if(!a[0])continue;

		if(!strcmp(a[0], "}"))
			break;

		if(!strcmp(a[0], "flags"))
		{
		}

//		if(!strcmp(a[0], "entity"))
//		{
//			LBXGL_Draw2D_ReadSequence_Entity(fd, seq);
//			continue;
//		}

		if(!strcmp(a[0], "texture"))
		{
			ent->imgname=dystrdup(a[1]);
			ent->tex=LBXGL_Texture_LoadImage(ent->imgname);
			continue;
		}

		if(!strcmp(a[0], "origin"))
		{
			ent->org=vec2(atof(a[1]), atof(a[2]));
			continue;
		}

		if(!strcmp(a[0], "size"))
		{
			ent->size=vec2(atof(a[1]), atof(a[2]));
			continue;
		}

		if(!strcmp(a[0], "angle"))
		{
			ent->angle=atof(a[1]);
			continue;
		}
	}
}

LBXGL_API void LBXGL_Draw2D_ReadSequence(VFILE *fd,
	BTGE_DrawSequence2D *seq)
{
	char buf[256];
	char **a, *s;
	int i;

	while(!vfeof(fd))
	{
		vfgets(buf, 255, fd);
		a=gcrsplit(buf);
		if(!a[0])continue;

		if(!strcmp(a[0], "flags"))
		{
		}

		if(!strcmp(a[0], "entity"))
		{
			LBXGL_Draw2D_ReadSequence_Entity(fd, seq, atoi(a[1]));
			continue;
		}
	}
}

LBXGL_API BTGE_DrawSequence2D *LBXGL_Draw2D_LoadSequence(char *name)
{
	VFILE *fd;
	BTGE_DrawSequence2D *seq;
	
	fd=vffopen(name, "rt");
	if(!fd)return(NULL);
	
	seq=LBXGL_Draw2D_AllocSequence();
	LBXGL_Draw2D_ReadSequence(fd, seq);

	if(seq->fps<=0)
		seq->fps=10.0;
	seq->fdt=1.0/seq->fps;

	vfclose(fd);
	return(seq);
}

LBXGL_API int LBXGL_Draw2D_SaveSequence(char *name,
	BTGE_DrawSequence2D *seq)
{
	VFILE *fd;
//	BTGE_DrawSequence2D *seq;
	
	fd=vffopen(name, "wt");
	if(!fd)return(-1);

	LBXGL_Draw2D_WriteSequence(fd, seq);

	vfclose(fd);
	return(0);
}


LBXGL_API int LBXGL_Draw2D_SequenceInterpolateEntityPosition(
	BTGE_DrawSequence2D *seq, int entid, float frame,
	BTGE_DrawSequence2D_FramePos *pos)
{
	BTGE_DrawSequence2D_FramePos *posa, *posb, *tpos;
	int frbase, frnuma, frnumb;
	float f, g, t, aa, ab, a, sc;
	int i, j, k;
	
	if(!seq->n_frame)
		return(0);
	
	frbase=(int)frame;
	posa=NULL; posb=NULL;

	while(frbase>=seq->n_frame)
		{ frbase-=seq->n_frame; }

	frnuma=frbase;
	while(1)
	{
		if(seq->frame[frnuma] &&
			(entid<seq->frame[frnuma]->n_pos))
		{
			tpos=seq->frame[frnuma]->entpos[entid];
			if(tpos)
				{ posa=tpos; break; }
		}
		frnuma--;
		if(frnuma<0)
			frnuma+=seq->n_frame;
		if(frnuma==frbase)
			break;
	}

	frnumb=frbase;
	while(1)
	{
		if(seq->frame[frnumb] &&
			(entid<seq->frame[frnumb]->n_pos))
		{
			tpos=seq->frame[frnumb]->entpos[entid];
			if(tpos)
				{ posb=tpos; break; }
		}
		frnumb++;
		if(frnumb>=seq->n_frame)
			frnumb-=seq->n_frame;
		if(frnumb==frbase)
			break;
	}
	
	if(!posa || !posb)
		return(0);
		
	if(frnuma==frnumb)
	{
		pos->org=posa->org;
		pos->angle=posa->angle;
		pos->scale=posa->scale;
		pos->statefl=posa->statefl;
		return(1);
	}
	
	i=frnuma; j=frnumb;
	if(j<i)j+=seq->n_frame;
	k=j-i;
	t=(frame-i)/k;
	if(t<0.0)t=0.0;
	if(t>1.0)t=1.0;

	aa=posa->angle;
	ab=posb->angle;
	f=ab-aa;
	if(fabs(f>180))
		{ ab+=360; }
	a=(1.0-t)*aa+t*ab;
	while(a<0)a+=360;
	while(a>=360)a-=360;

	f=posa->scale;
	g=posb->scale;
	sc=(1.0-t)*f+t*g;

	pos->org=v2lerp(posa->org, posb->org, t);
	pos->angle=a;
	pos->scale=sc;
	pos->statefl=posa->statefl;
	
	return(2);
}

LBXGL_API BTGE_DrawableObject2D *LBXGL_Draw2D_SequenceLookupEntity(
	BTGE_DrawSequence2D *seq, int entid)
{
	BTGE_DrawableObject2D *ecur;

	ecur=seq->entity;
	while(ecur)
	{
		if(ecur->entid==entid)
			return(ecur);
		ecur=ecur->seqnext;
	}
	return(NULL);
}

LBXGL_API int LBXGL_Draw2D_SequenceUpdateEntity(
	BTGE_DrawSequence2D *seq, int entid, float frame)
{
	BTGE_DrawableObject2D *ent;
	BTGE_DrawSequence2D_FramePos tpos;
	BTGE_DrawSequence2D_FramePos *pos;
	int i;
	
	ent=LBXGL_Draw2D_SequenceLookupEntity(seq, entid);
	if(!ent)return(-1);
	
	pos=&tpos;
	memset(pos, 0, sizeof(BTGE_DrawSequence2D_FramePos));

	i=LBXGL_Draw2D_SequenceInterpolateEntityPosition(seq, entid, frame, pos);
	
	if(i)
	{
		ent->org=pos->org;
		ent->angle=pos->angle;
		ent->scale=pos->scale;
		ent->stateflags=pos->statefl;
	}
	
	return(0);
}

LBXGL_API int LBXGL_Draw2D_SequenceUpdateEntities(
	BTGE_DrawSequence2D *seq, float frame)
{
	BTGE_DrawableObject2D *ecur;

	ecur=seq->entity;
	while(ecur)
	{
		LBXGL_Draw2D_SequenceUpdateEntity(seq, ecur->entid, frame);
		ecur=ecur->seqnext;
	}
	return(0);
}

LBXGL_API int LBXGL_Draw2D_SequenceUpdate(
	BTGE_DrawSequence2D *seq, float dt)
{
	seq->adt+=dt;
	while(seq->adt>=seq->fdt)
	{
		seq->adt-=seq->fdt;
		seq->cur_frame++;
	}
	
	LBXGL_Draw2D_SequenceUpdateEntities(seq, seq->cur_frame+seq->adt);
	return(0);
}

LBXGL_API int LBXGL_Draw2D_SequenceInsertFrame(
	BTGE_DrawSequence2D *seq, int frame)
{
	int i, fr2;

	fr2=frame+1;
	LBXGL_Draw2D_CheckExpandFrame(seq, seq->n_frame+1);
//	for(i=seq->n_frame; i>frame; i--)
//		{ seq->frame[i]=seq->frame[i-1]; }
//	seq->frame[frame]=NULL;
	for(i=seq->n_frame; i>fr2; i--)
		{ seq->frame[i]=seq->frame[i-1]; }
	seq->frame[fr2]=NULL;
	seq->n_frame++;
	return(0);
}

LBXGL_API int LBXGL_Draw2D_SequenceDeleteFrame(
	BTGE_DrawSequence2D *seq, int frame)
{
	int i;
	
	for(i=frame; i<seq->n_frame; i++)
		{ seq->frame[i]=seq->frame[i+1]; }
	seq->n_frame--;
	return(0);
}

LBXGL_API BTGE_DrawSequence2D_FramePos *
	LBXGL_Draw2D_SequenceInsertFramePos(
		BTGE_DrawSequence2D *seq, int frnum, int entid)
{
	BTGE_DrawSequence2D_Frame *frame;
	BTGE_DrawSequence2D_FramePos *frpos;
	char **a, *s;
	int i;

	LBXGL_Draw2D_CheckExpandFrame(seq, frnum);

	frame=seq->frame[frnum];

	if(!frame)
	{
		frame=LBXGL_Draw2D_AllocSequenceFrame();
		seq->frame[frnum]=frame;
		if(frnum>=seq->n_frame)
			seq->n_frame=frnum+1;
	}

	LBXGL_Draw2D_CheckFrameEntID(frame, entid);
	
	frpos=frame->entpos[entid];
	
	if(!frpos)
	{
		frpos=LBXGL_Draw2D_AllocSequenceFramePos();
		LBXGL_Draw2D_SequenceInterpolateEntityPosition(
			seq, entid, frnum, frpos);
		frame->entpos[entid]=frpos;
	}

	return(frpos);
}

LBXGL_API int LBXGL_Draw2D_SequenceDeleteFramePos(
		BTGE_DrawSequence2D *seq, int frnum, int entid)
{
	BTGE_DrawSequence2D_Frame *frame;
	BTGE_DrawSequence2D_FramePos *frpos;
	char **a, *s;
	int i;

	LBXGL_Draw2D_CheckExpandFrame(seq, frnum);

	frame=seq->frame[frnum];

	if(!frame)
	{
		frame=LBXGL_Draw2D_AllocSequenceFrame();
		seq->frame[frnum]=frame;
		if(frnum>=seq->n_frame)
			seq->n_frame=frnum+1;
	}

	LBXGL_Draw2D_CheckFrameEntID(frame, entid);
	
	frpos=frame->entpos[entid];
	
	if(frpos)
	{
		gcfree(frpos);
		frame->entpos[entid]=NULL;
	}
	
	return(0);
}

LBXGL_API BTGE_DrawableObject2D *LBXGL_Draw2D_SequenceCreateEntity(
	BTGE_DrawSequence2D *seq)
{
	BTGE_DrawableObject2D *ent;

	ent=LBXGL_BrushDraw2D_CreateObject(lbxgl_brush2d_world);
	ent->entid=seq->n_entity++;

	ent->seqnext=seq->entity;
	seq->entity=ent;
	ent->seq=seq;
	
	return(ent);

//	ecur=seq->entity;
//	while(ecur)
//	{
//		if(ecur->entid==entid)
//			return(ecur);
//		ecur=ecur->seqnext;
//	}
	return(NULL);
}
