#include <lbxgl2.h>
#include <math.h>

LBXGL_API void Skel_ClampV3F(float *iv, float *ov)
{
	V3F_COPY(iv, ov);
	if(fabs(ov[0])<0.0001)ov[0]=0;
	if(fabs(ov[1])<0.0001)ov[1]=0;
	if(fabs(ov[2])<0.0001)ov[2]=0;
}

LBXGL_API char *Skel_Num2Str(float f)
{
	char *s, *b;
	int i;

	b=gcralloc(16);
//	i=(int)round(f);
	i=floor(f+0.5);
	if(fabs(f-i)<0.001)
	{
		sprintf(b, "%d", i);
		return(b);
	}
	sprintf(b, "%.3f", f);
	s=b+strlen(b);

	s--;
	while((s>b) && (*s=='0'))s--;
	s++; *s++=0;
	return(b);
}

LBXGL_API int Skel_SaveBones(LBXGL_Skel2Model *mdl, char *name)
{
	static float bone_rorg[256*3];
	float tv0[3], tv1[3];
	char buf[256];
	char **a;
	VFILE *fd;
	int i, j;

//	fd=vffopen(name, "wt");
	fd=btcl_vffopen(name, "wt");
	if(!fd)
	{
		printf("can't write bones %s\n", name);
		return(-1);
	}

	printf("saving bones %s\n", name);

	for(i=0; i<mdl->num_bones; i++)
	{
		if(!mdl->bone_name[i])continue;

		bone_rorg[i*3+0]=mdl->bone_org[i*3+0];
		bone_rorg[i*3+1]=mdl->bone_org[i*3+1];
		bone_rorg[i*3+2]=mdl->bone_org[i*3+2];
		Skel_ClampV3F(bone_rorg+i*3, bone_rorg+i*3);

		if(mdl->bone_up[i]>=0)
		{
			j=mdl->bone_up[i];
			bone_rorg[i*3+0]+=bone_rorg[j*3+0];
			bone_rorg[i*3+1]+=bone_rorg[j*3+1];
			bone_rorg[i*3+2]+=bone_rorg[j*3+2];
		}

		vfprint(fd, "%d %d \"%s\" [ %.4g %.4g %.4g ]\n", i,
			mdl->bone_up[i], mdl->bone_name[i],
			bone_rorg[i*3+0], bone_rorg[i*3+1],
			bone_rorg[i*3+2]);
	}

	vfprint(fd, "\n\n");

	for(i=0; i<mdl->num_solids; i++)
	{
		j=mdl->solid[i]->bone;
		V3F_ADD(mdl->solid[i]->m, bone_rorg+j*3, tv0);
		V3F_ADD(mdl->solid[i]->n, bone_rorg+j*3, tv1);
		Skel_ClampV3F(tv0, tv0);
		Skel_ClampV3F(tv1, tv1);

		switch(mdl->solid[i]->type)
		{
		case BONE_SOLID_BBOX:
			vfprint(fd, "bbox %d [ %.4g %.4g %.4g ] "
				"[ %.4g %.4g %.4g ] %.4g %.4g\n",
				mdl->solid[i]->bone,
				tv0[0], tv0[1], tv0[2],
				tv1[0], tv1[1], tv1[2],
				mdl->solid[i]->r0, mdl->solid[i]->w);
			break;
		case BONE_SOLID_SPHERE:
			vfprint(fd, "sphere %d [ %.4g %.4g %.4g ] "
					"%.4g %.4g %.4g\n",
				mdl->solid[i]->bone,
				tv0[0], tv0[1], tv0[2],
				mdl->solid[i]->r0, mdl->solid[i]->r1,
				mdl->solid[i]->w);
			break;
		case BONE_SOLID_CAPSULE:
			vfprint(fd, "capsule %d [ %.4g %.4g %.4g ] "
				"[ %.4g %.4g %.4g ] %.4g %.4g %.4g\n",
				mdl->solid[i]->bone,
				tv0[0], tv0[1], tv0[2],
				tv1[0], tv1[1], tv1[2],
				mdl->solid[i]->r0, mdl->solid[i]->r1,
				mdl->solid[i]->w);
			break;
		default:
			break;
		}
	}

	vfclose(fd);
	return(0);
}

LBXGL_API int Skel_SaveStateBones(LBXGL_Skel2State *state, char *name)
{
	static float bone_rorg[256*3];
	float tv0[3], tv1[3];
	char buf[256];
	LBXGL_Skel2Model *mdl;
	char **a;
	VFILE *fd;
	int i, j;

//	fd=vffopen(name, "wt");
	fd=btcl_vffopen(name, "wt");
	if(!fd)
	{
		printf("can't write bones %s\n", name);
		return(-1);
	}

	printf("saving bones %s\n", name);

	mdl=state->mdl;

	for(i=0; i<mdl->num_bones; i++)
	{
		if(!mdl->bone_name[i])continue;

		bone_rorg[i*3+0]=state->bone_rorg[i*3+0];
		bone_rorg[i*3+1]=state->bone_rorg[i*3+1];
		bone_rorg[i*3+2]=state->bone_rorg[i*3+2];
		Skel_ClampV3F(bone_rorg+i*3, bone_rorg+i*3);

		if(mdl->bone_up[i]>=0)
		{
			j=mdl->bone_up[i];
//			bone_rorg[i*3+0]+=bone_rorg[j*3+0];
//			bone_rorg[i*3+1]+=bone_rorg[j*3+1];
//			bone_rorg[i*3+2]+=bone_rorg[j*3+2];
		}

		vfprint(fd, "%d %d \"%s\" [ %.4g %.4g %.4g ]\n", i,
			mdl->bone_up[i], mdl->bone_name[i],
			bone_rorg[i*3+0], bone_rorg[i*3+1],
			bone_rorg[i*3+2]);
	}

	vfprint(fd, "\n\n");

	for(i=0; i<mdl->num_solids; i++)
	{
		j=mdl->solid[i]->bone;
		V3F_ADD(mdl->solid[i]->m, bone_rorg+j*3, tv0);
		V3F_ADD(mdl->solid[i]->n, bone_rorg+j*3, tv1);
		Skel_ClampV3F(tv0, tv0);
		Skel_ClampV3F(tv1, tv1);

		switch(mdl->solid[i]->type)
		{
		case BONE_SOLID_BBOX:
			vfprint(fd, "bbox %d [ %.4g %.4g %.4g ] "
				"[ %.4g %.4g %.4g ] %.4g %.4g\n",
				mdl->solid[i]->bone,
				tv0[0], tv0[1], tv0[2],
				tv1[0], tv1[1], tv1[2],
				mdl->solid[i]->r0, mdl->solid[i]->w);
			break;
		case BONE_SOLID_SPHERE:
			vfprint(fd, "sphere %d [ %.4g %.4g %.4g ] "
					"%.4g %.4g %.4g\n",
				mdl->solid[i]->bone,
				tv0[0], tv0[1], tv0[2],
				mdl->solid[i]->r0, mdl->solid[i]->r1,
				mdl->solid[i]->w);
			break;
		case BONE_SOLID_CAPSULE:
			vfprint(fd, "capsule %d [ %.4g %.4g %.4g ] "
				"[ %.4g %.4g %.4g ] %.4g %.4g %.4g\n",
				mdl->solid[i]->bone,
				tv0[0], tv0[1], tv0[2],
				tv1[0], tv1[1], tv1[2],
				mdl->solid[i]->r0, mdl->solid[i]->r1,
				mdl->solid[i]->w);
			break;
		default:
			break;
		}
	}

	vfclose(fd);
	return(0);
}


LBXGL_API int Skel_SaveAnim(LBXGL_Skel2Model *mdl, LBXGL_Skel2Anim *anim, char *name)
{
	char buf[256];
	float pos[16];
	char **a;
	VFILE *fd;
	float f;
	int i, j, k, l;

//	fd=vffopen(name, "wt");
	fd=btcl_vffopen(name, "wt");
	if(!fd)
	{
		printf("can't write anim %s\n", name);
		return(-1);
	}

	printf("saving anim %s\n", name);

	for(i=0; i<anim->num_frames; i++)
	{
		l=-1;
		vfprint(fd, "frame %d\n", i);
		for(j=0; j<mdl->num_bones; j++)
		{
			if(!mdl->bone_name[j])continue;

			k=i*mdl->max_bones+j;
			if(anim->frame_mask[k>>3]&(1<<(k&7)))
			{
				vfprint(fd, "%s [ ", mdl->bone_name[j]);
				for(l=0; l<3; l++)
				{
					f=anim->frame_org[k*3+l];
//					if(fabs(f)<0.0001)f=0;
//					vfprint(fd, "%.4g ", f);

					vfprint(fd, "%s ", Skel_Num2Str(f));
				}
				vfprint(fd, "] [ ");
				for(l=0; l<9; l++)
				{
					f=anim->frame_rot[k*9+l];
//					if(fabs(f)<0.0001)f=0;
//					vfprint(fd, "%.4g ", f);
					vfprint(fd, "%s ", Skel_Num2Str(f));
				}
				vfprint(fd, "]\n");
			}
		}
		if(l>=0)vfprint(fd, "\n");
	}
	vfclose(fd);
	return(0);
}

