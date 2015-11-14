/*
Sequences will be defined as nested arrays of the form:
{{<sequence-name>, <frames>}, ...}

Frames are arrays of the form:
{{<name>, <next>, <function>}, ...}

Sequences will be stuct in the "sequences" slot of entities.
The current sequence will be placed in the "sequence" slot of entities.
The sequence frame will be in "sequence_frame".
The sequence time (used to time "time until next frame") will be in
"sequence_time".
*/

#include <lbxgl2.h>

/*--
Cat lbxgl;Entity;Sequence
Form
	void **LBXGL_Sequence_GetFrame(void *seqs, char *seq, char *seqfr);
Description
	Looks up a particular sequence/frame in a sequences array.
--*/
void **LBXGL_Sequence_GetFrame(void *seqs, char *seq, char *seqfr)
{
	void **sa, **sna, **fa, **fna;
	int i, j;

	sa=NetVal_UnwrapDArray(seqs);
	for(i=0; sa[i]; i++)
	{
		sna=NetVal_UnwrapDArray(sa[i]);
		if(!strcmp(sna[0], seq))
		{
			fa=NetVal_UnwrapDArray(sna[1]);
			for(j=0; fa[j]; j++)
			{
				fna=NetVal_UnwrapDArray(fa[j]);
				if(!strcmp(fna[0], seqfr))
					return(fna);
			}
		}
	}
	return(NULL);
}

/*--
Cat lbxgl;Entity;Sequence
Form
	int LBXGL_Sequence_ScanFrame(char **seq, char **seqfr, char *frame);
Description
	Parses the sequence and frame out of a frame name.
	seq may not be modified, eg, if the name does not contain a sequence.
--*/
int LBXGL_Sequence_ScanFrame(char **seq, char **seqfr, char *frame)
{
	void **sa, **sna, *fa, *fna;
	int i, j;
	char *pseq, *pfr, *s, *t;

	s=frame;

	while(*s && (*s!='/'))s++;
	if(!*s)
	{
		*seqfr=NetVal_WrapString(frame);
		return(0);
	}

	pseq=kralloc(64);
	pfr=kralloc(64);
	s=frame;

	t=pseq;
	while(*s && (*s!='/'))*t++=*s++;
	*t++=0;

	s++;

	t=pfr;
	while(*s && (*s!='/'))*t++=*s++;
	*t++=0;

	*seq=NetVal_WrapString(pseq);
	*seqfr=NetVal_WrapString(pfr);
	return(0);
}

/*--
Cat lbxgl;Entity;Sequence
Form
	int LBXGL_Sequence_SetEntitySequence(LBXGL_Entity *ent, char *seq, \
	char *seqfr);
Description
	Changes the entity to having a specific sequence/frame \
	(eg: animation jump).
	This also invokes any function associated with that frame.
--*/
int LBXGL_Sequence_SetEntitySequence(LBXGL_Entity *ent,
	char *seq, char *seqfr)
{
	double *seqt;
	void *seqs, *fcn, *p;
	void **fa;
	void *args[2];
	int i;

	p=LBXGL_Entity_GetProperty(ent, "mdl");
	if(ObjType_TypeP(p, "lbxgl_skelmesh_t"))
	{
		i=LBXGL_PhysSkel_SetEntitySequence(ent, seq);
		return(i);
	}

	seqt=LBXGL_Entity_GetProperty(ent, "sequence_time");
	if(!seqt)
	{
		seqt=NetVal_WrapFloat(0.1);
		LBXGL_Entity_SetProperty(ent, "sequence_time", seqt);
	}

	seqs=LBXGL_Entity_GetProperty(ent, "sequences");

	LBXGL_Entity_SetProperty(ent, "sequence", seq);
	LBXGL_Entity_SetProperty(ent, "sequence_frame", seqfr);
	*seqt=0.1;

	LBXGL_Entity_SetProperty(ent, "frame", seqfr);

	fa=LBXGL_Sequence_GetFrame(seqs, seq, seqfr);
	if(!fa)
	{
		kprint("LBXGL_Sequence_SequenceEntity: "
			"Undefined Frame %s/%s\n",
			seq, seqfr);
		return(-1);
	}
	fcn=fa[2];
	if(ObjType_TypeP(fcn, "bool_t"))fcn=NULL;
	if(fcn)
	{
		args[0]=NULL;
		PDSCR_Interp_CallFunction(ent, fcn, args);
	}
	return(0);
}

/*--
Cat lbxgl;Entity;Sequence
Form
	int LBXGL_Sequence_SequenceEntity(LBXGL_Entity *ent, float dt);
Description
	Steps the entity along for the number of frames for the time given \
	in dt. This does nothing if "sequence" is NULL.
	All functions for the associated frames are invoked.
--*/
int LBXGL_Sequence_SequenceEntity(LBXGL_Entity *ent, float dt)
{
	char *seq, *seqfr, *seqn;
	double *seqt;
	void *seqs, *fcn, *p;
	void **fa, **fra;
	void *args[2];
	int i;

	p=LBXGL_Entity_GetProperty(ent, "mdl");
	if(ObjType_TypeP(p, "lbxgl_skelmesh_t"))
	{
		i=LBXGL_PhysSkel_SequenceEntity(ent, dt);
		return(i);
	}

	seq=LBXGL_Entity_GetProperty(ent, "sequence");
	if(!seq)return(0);

	seqt=LBXGL_Entity_GetProperty(ent, "sequence_time");
	if(!seqt)
	{
		seqt=NetVal_WrapFloat(0.1);
		LBXGL_Entity_SetProperty(ent, "sequence_time", seqt);
	}

//	kprint("seq %s %f\n", seq, *seqt);

	if(*seqt>0)*seqt-=dt;
	while((*seqt<=0) && (*seqt!=-1))
	{
		fra=NULL;
		p=LBXGL_Entity_GetProperty(ent, "frames");
		if(p)fra=NetVal_UnwrapDArray(p);

		seqs=LBXGL_Entity_GetProperty(ent, "sequences");
		seqfr=LBXGL_Entity_GetProperty(ent, "sequence_frame");

//		kprint("seq %s/%s\n", seq, seqfr);

		fa=LBXGL_Sequence_GetFrame(seqs, seq, seqfr);
		if(!fa)
		{
			*seqt=-1;
			break;
		}

		seqn=fa[1];
		if(!ObjType_TypeP(seqn, "string_t"))seqn=NULL;

		if(seqn)
		{
			LBXGL_Sequence_ScanFrame(&seq, &seqfr, seqn);
			LBXGL_Entity_SetProperty(ent, "sequence", seq);
			LBXGL_Entity_SetProperty(ent, "sequence_frame", seqfr);
			*seqt+=0.1;

			if(fra)
			{
				for(i=0; fra[i]; i++)
					if(!strcmp(seqfr, fra[i]))break;
				if(!fra[i])i=0;
				LBXGL_Entity_SetProperty(ent, "frame",
					NetVal_WrapFloat(i));
			}else LBXGL_Entity_SetProperty(ent, "frame", seqfr);

			fa=LBXGL_Sequence_GetFrame(seqs, seq, seqfr);
			if(!fa)
			{
				kprint("LBXGL_Sequence_SequenceEntity: "
					"Undefined Frame %s/%s\n",
					seq, seqfr);
				return(-1);
			}
			fcn=fa[2];
			if(ObjType_TypeP(fcn, "bool_t"))fcn=NULL;
			if(fcn)
			{
//				kprint("func %s\n", ObjType_GetTypeName(fcn));
				LBXGL_Entity_SetProperty(ent,
					"sequence_func", fcn);
			
				args[0]=NULL;
				LBXGL_Entity_SendMessage(ent, "sequence_func",
					args);
//				PDSCR_Interp_CallFunction(ent, fcn, args);
			}
		}else
		{
			LBXGL_Entity_SetProperty(ent, "sequence", NULL);
			LBXGL_Entity_SetProperty(ent, "sequence_frame", NULL);
			*seqt=-1;
		}
	}

	return(0);
}

/*--
Cat lbxgl;Entity;Sequence
Form
	int LBXGL_Sequence_SequenceEntities(LBXGL_Entity *ents, float dt);
Description
	Step along a list of entities, handling the sequencing for all \
	entities in the list.
--*/
int LBXGL_Sequence_SequenceEntities(LBXGL_Entity *ents, float dt)
{
	LBXGL_Entity *cur;

	cur=ents;
	while(cur)
	{
		LBXGL_Sequence_SequenceEntity(cur, dt);
		cur=cur->next;
	}
	return(0);
}

void *LBXGL_Sequence_LoadFramesFile(char *name)
{
	VFILE *fd;
	char buf[256], buf2[64];
	char *s, *s2, *t;
	int i, j, k;

	char frames[256][64];
	int num_frames;
	char anim_name[64];
	int anim_first, anim_last;
	void *arr[256], *p;

	num_frames=0;

	fd=vffopen(name, "rt");
	if(!fd)return(NULL);

	while(!vfeof(fd))
	{
		memset(buf, 0, 256);
		vfgets(buf, 255, fd);
		s=buf;
		while(*s && (*s<=' '))s++;
		if(!*s)continue;
		if(*s=='#')continue;

		if(*s=='$')
		{
			s++;

			t=anim_name;
			while(*s && (*s>' '))*t++=*s++;
			*t++=0;

			t=buf2;
			while(*s && (*s<=' '))s++;
			while(*s && (*s>' '))*t++=*s++;
			*t++=0;
			anim_first=atoi(buf2);

			t=buf2;
			while(*s && (*s<=' '))s++;
			while(*s && (*s>' '))*t++=*s++;
			*t++=0;
			anim_last=atoi(buf2);

			for(i=anim_first; i<=anim_last; i++)
			{
				t=anim_name;
				if(!anim_last)
				{
					kprints(buf2, "%s\n", t);
				}else if((anim_last>=10) ||
					isdigit(t[strlen(t)-1]))
				{
					kprints(buf2, "%s%02d\n", t, i);
				}else
				{
					kprints(buf2, "%s%d\n", t, i);
				}
				strcpy(frames[num_frames++], buf2);

			}

			continue;
		}

		while(*s)
		{
			t=frames[num_frames++];
//			t=buf2;
			while(*s && (*s>' '))*t++=*s++;
			*t++=0;

			while(*s && (*s<=' '))s++;
		}
	}

	vfclose(fd);

	kprint("loaded frames file %s, %d frames\n", name, num_frames);

	for(i=0; i<num_frames; i++)
		arr[i]=NetVal_WrapString(frames[i]);
	arr[i]=NULL;

	p=NetVal_WrapDArray(arr);
	return(p);
}

void *LBXGL_Sequence_LoadSequencesFile(char *name)
{
	VFILE *fd;
	char buf[256], buf2[64];
	char *s, *s2, *t;
	int i, j, k;

	static char anim_names[256][64];
	static int anim_first[256], anim_last[256];
	static void *arr[256], *p;

	static void *anim_arr[256], *frames_arr[256];
	int num_anims, num_anim_frames;

	static char frame_name[64], frame_next[64];

	fd=vffopen(name, "rt");
	if(!fd)return(NULL);

	num_anims=0;
	num_anim_frames=0;
	anim_names[0][0]=0;

	while(!vfeof(fd))
	{
		memset(buf, 0, 256);
		vfgets(buf, 255, fd);
		s=buf;
		while(*s && (*s<=' '))s++;
		if(!*s)continue;
		if(*s=='#')continue;

		if(*s=='$')
		{
			s++;

			if(num_anims || anim_names[0][0])
			{
				if(num_anim_frames)
				{
					arr[0]=NetVal_WrapString(
						anim_names[num_anims]);
					arr[1]=NetVal_WrapDArray(frames_arr);
					arr[2]=NULL;
					anim_arr[num_anims]=
						NetVal_WrapDArray(arr);
					num_anim_frames=0;
				}else
				{
					arr[0]=NetVal_WrapString(anim_names[num_anims]);
					arr[1]=NetVal_WrapBool(0);
					arr[2]=NULL;
					anim_arr[num_anims]=NetVal_WrapDArray(arr);
				}
				num_anims++;
			}

			t=anim_names[num_anims];
			while(*s && (*s>' '))*t++=*s++;
			*t++=0;

			t=buf2;
			while(*s && (*s<=' '))s++;
			while(*s && (*s>' '))*t++=*s++;
			*t++=0;
			anim_first[num_anims]=atoi(buf2);

			t=buf2;
			while(*s && (*s<=' '))s++;
			while(*s && (*s>' '))*t++=*s++;
			*t++=0;
			anim_last[num_anims]=atoi(buf2);

			anim_arr[num_anims]=NULL;
			continue;
		}

		t=frame_name;
		while(*s && (*s>' '))*t++=*s++;
		*t++=0;

		while(*s && (*s<=' '))s++;

		t=frame_next;
		while(*s && (*s>' '))*t++=*s++;
		*t++=0;

		arr[0]=NetVal_WrapString(frame_name);
		arr[1]=NetVal_WrapString(frame_next);
		arr[2]=NetVal_WrapBool(0);
		arr[3]=NULL;

		frames_arr[num_anim_frames++]=NetVal_WrapDArray(arr);
		frames_arr[num_anim_frames]=NULL;
	}
	if(num_anims || anim_names[0][0])
	{
		if(num_anim_frames)
		{
			arr[0]=NetVal_WrapString(anim_names[num_anims]);
			arr[1]=NetVal_WrapDArray(frames_arr);
			arr[2]=NULL;
			anim_arr[num_anims]=NetVal_WrapDArray(arr);
			num_anim_frames=0;
		}else
		{
			arr[0]=NetVal_WrapString(anim_names[num_anims]);
			arr[1]=NetVal_WrapBool(0);
			arr[2]=NULL;
			anim_arr[num_anims]=NetVal_WrapDArray(arr);
		}
		num_anims++;
	}
	anim_arr[num_anims]=NULL;

	p=NetVal_WrapDArray(anim_arr);

	vfclose(fd);

	return(p);
}
