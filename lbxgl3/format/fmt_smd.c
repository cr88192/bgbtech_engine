/*
Issue of transposed space.
I can't figure this one out entirely, so I can't go over to using a more
normal representation for the transforms.

Thus, at the time of this writing, the matrices are layed out as:
[Ix Jx Kx Tx]
[Iy Jy Ky Ty]
[Iz Jz Kz Tz]
[ 0  0  0  1]

Other oddities exist, oh well...
*/

#include <lbxgl2.h>


//shared temporary buffers for loading smd models
static int fmtsmd_num_xyz;
static int fmtsmd_num_norm;
static int fmtsmd_num_st;

static short		*fmtsmd_xyzbone=NULL;
static float		*fmtsmd_xyz=NULL;
static short		*fmtsmd_normbone=NULL;
static float		*fmtsmd_norm=NULL;
static float		*fmtsmd_st=NULL;

static int fmtsmd_num_face;
static int fmtsmd_num_skin;
static LBXGL_SkelFace	**fmtsmd_face;
static char		**fmtsmd_skinname;


static char **split_str(char *s)
{
	char **a, *t;
	int i;

	a=gcralloc(64*sizeof(char *));
	i=0;
	while(*s)
	{
		while(*s && (*s<=' '))s++;
		if(!*s)break;
		t=gcralloc(256);
		a[i++]=t;

		if(*s=='"')
		{
			s++;
			while(*s && (*s!='"'))*t++=*s++;
			if(*s=='"')s++;

			*t++=0;
			continue;
		}

		while(*s && (*s>' '))*t++=*s++;
		*t++=0;
	}
	a[i]=NULL;

	return(a);
}

void LBXGL_FmtSmd_FromAngles(float *a, float *b)
{
	float sx, cx, sy, cy, sz, cz;

	sx=sin(a[0]*0.5);
	cx=cos(a[0]*0.5);
	sy=sin(a[1]*0.5);
	cy=cos(a[1]*0.5);
	sz=sin(a[2]*0.5);
	cz=cos(a[2]*0.5);

	b[0]=sx*cy*cz-cx*sy*sz; // X
	b[1]=cx*sy*cz+sx*cy*sz; // Y
	b[2]=cx*cy*sz-sx*sy*cz; // Z
	b[3]=cx*cy*cz+sx*sy*sz; // W
}

void LBXGL_FmtSmd_ToMatrix(float *a, float *b)
{
	float xx, xy, xz, xw, yy, yz, yw, zz, zw;
	int i;

	for(i=0; i<16; i++)b[i]=0;
	for(i=0; i<4; i++)b[(i*4)+i]=1;

	xx=a[0]*a[0];
	xy=a[0]*a[1];
	xz=a[0]*a[2];
	xw=a[0]*a[3];
	yy=a[1]*a[1];
	yz=a[1]*a[2];
	yw=a[1]*a[3];
	zz=a[2]*a[2];
	zw=a[2]*a[3];

	b[0]= 1-(2*yy)-(2*zz);
	b[1]=   (2*xy)-(2*zw);
	b[2]=   (2*xz)+(2*yw);

	b[4]=   (2*xy)+(2*zw);
	b[5]= 1-(2*xx)-(2*zz);
	b[6]=   (2*yz)-(2*xw);

	b[8]=   (2*xz)-(2*yw);
	b[9]=   (2*yz)+(2*xw);
	b[10]=1-(2*xx)-(2*yy);
}


//Load SMD Reference

int LBXGL_FmtSmd_LoadBaseBones(LBXGL_SkelMesh *mdl, VFILE *fd)
{
	LBXGL_SkelBone **bone;
	char *buf, **a;
	int i, j, n;

	bone=gcalloc(LBXGL_SKELMESH_DEFMAXBONES*sizeof(LBXGL_SkelBone *));

	n=0;
	while(!vfeof(fd))
	{
		buf=gcralloc(256);
		memset(buf, 0, 256);
		vfgets(buf, 255, fd);

		a=split_str(buf);
		if(!a[0])continue;

		if(!strcmp(a[0], "end"))
			break;

		i=atoi(a[0]);

		bone[i]=gcalloc(sizeof(LBXGL_SkelBone));
		bone[i]->name=dystrdup(a[1]);
		bone[i]->parent=atoi(a[2]);

		for(j=0; j<3; j++)
		{
			bone[i]->min[j]=999999;
			bone[i]->max[j]=-999999;
		}

		if((i+1)>n)n=i+1;
	}

	mdl->num_bones=n;
	mdl->bone=gcalloc(n*sizeof(LBXGL_SkelBone *));
	for(i=0; i<n; i++)mdl->bone[i]=bone[i];
	gcfree(bone);

	return(0);
}

int LBXGL_FmtSmd_LoadBaseFrame(LBXGL_SkelMesh *mdl, VFILE *fd)
{
	char *buf, **a;
	int i, j, n;

	while(!vfeof(fd))
	{
		buf=gcralloc(256);
		memset(buf, 0, 256);
		vfgets(buf, 255, fd);

		a=split_str(buf);
		if(!a[0])continue;

		if(!strcmp(a[0], "end"))
			break;

		if(!strcmp(a[0], "time"))
			continue;

		i=atoi(a[0]);

//		bone[i]->name=dystrdup(a[1]);
//		bone[i]->parent=atoi(a[2]);
		for(j=0; j<3; j++)
		{
			mdl->bone[i]->pos[j]=atof(a[1+j]);
			mdl->bone[i]->rot[j]=atof(a[4+j]);
		}
	}

	return(0);
}

int LBXGL_FmtSmd_IndexSkin(char *name)
{
	int i;

	for(i=0; i<fmtsmd_num_skin; i++)
		if(!strcmp(fmtsmd_skinname[i], name))
			return(i);
	fmtsmd_skinname[i]=dystrdup(name);
	fmtsmd_num_skin++;

	return(i);
}

int LBXGL_FmtSmd_IndexXYZ(int b, float *v)
{
	int i;
	for(i=0; i<fmtsmd_num_xyz; i++)
	{
		if((fmtsmd_xyzbone[i]==b) &&
			(fmtsmd_xyz[i*3+0]==v[0]) &&
			(fmtsmd_xyz[i*3+1]==v[1]) &&
			(fmtsmd_xyz[i*3+2]==v[2]))
				return(i);
	}
	fmtsmd_num_xyz++;

	fmtsmd_xyzbone[i]=b;
	fmtsmd_xyz[i*3+0]=v[0];
	fmtsmd_xyz[i*3+1]=v[1];
	fmtsmd_xyz[i*3+2]=v[2];
	return(i);
}

int LBXGL_FmtSmd_IndexNorm(int b, float *v)
{
	int i;
	for(i=0; i<fmtsmd_num_norm; i++)
	{
		if((fmtsmd_normbone[i]==b) &&
			(fmtsmd_norm[i*3+0]==v[0]) &&
			(fmtsmd_norm[i*3+1]==v[1]) &&
			(fmtsmd_norm[i*3+2]==v[2]))
				return(i);
	}
	fmtsmd_num_norm++;

	fmtsmd_normbone[i]=b;
	fmtsmd_norm[i*3+0]=v[0];
	fmtsmd_norm[i*3+1]=v[1];
	fmtsmd_norm[i*3+2]=v[2];
	return(i);
}

int LBXGL_FmtSmd_IndexST(float *v)
{
	int i;
	for(i=0; i<fmtsmd_num_st; i++)
	{
		if((fmtsmd_st[i*2+0]==v[0]) &&
			(fmtsmd_st[i*2+1]==v[1]))
				return(i);
	}
	fmtsmd_num_st++;

	fmtsmd_st[i*2+0]=v[0];
	fmtsmd_st[i*2+1]=v[1];
	return(i);
}

int LBXGL_FmtSmd_LoadTriangles(LBXGL_SkelMesh *mdl, VFILE *fd)
{
	float tv0[3], tv1[3], tv2[3];
	float rv0[3], rv1[3], rv2[3];
	float cm[16];
	float *xforms;

	char *buf, **a, *s;
	int i, j, k, l, n;

	int hl2;
	float cw, bw;

	printf("LBXGL_FmtSmd_LoadTriangles\n");

	xforms=malloc(mdl->num_bones*16*sizeof(float));
	LBXGL_FmtSmd_CalcTransforms(mdl, -1, -1, xforms);

	if(!fmtsmd_xyz)
	{
		fmtsmd_face=malloc(65536*sizeof(LBXGL_SkelFace *));
		fmtsmd_skinname=gcalloc(256*sizeof(char *));

		fmtsmd_xyzbone=malloc(65536*sizeof(short));
		fmtsmd_xyz=malloc(3*65536*sizeof(float));
		fmtsmd_normbone=malloc(65536*sizeof(short));
		fmtsmd_norm=malloc(3*65536*sizeof(float));

		fmtsmd_st=malloc(2*65536*sizeof(float));
	}

	fmtsmd_num_xyz=0;
	fmtsmd_num_norm=0;
	fmtsmd_num_st=0;

	fmtsmd_num_face=0;
	fmtsmd_num_skin=0;

	hl2=0;

	n=0;
	while(!vfeof(fd))
	{
		buf=gcralloc(256);
		memset(buf, 0, 256);
		vfgets(buf, 255, fd);

		a=split_str(buf);
		if(!a[0])continue;

		if(!strcmp(a[0], "end"))
			break;

		i=LBXGL_FmtSmd_IndexSkin(a[0]);

		fmtsmd_face[n]=gcalloc(sizeof(LBXGL_SkelFace));
		fmtsmd_face[n]->skinnum=i;

		for(i=0; i<3; i++)
		{
			memset(buf, 0, 256);
			vfgets(buf, 255, fd);
			a=split_str(buf);

			for(j=0; j<3; j++)
			{
				tv0[j]=atof(a[1+j]);
				tv1[j]=atof(a[4+j]);
			}
			for(j=0; j<2; j++)
				tv2[j]=atof(a[7+j]);

			j=atoi(a[0]);
			if(!j && a[9])
			{
				if(!hl2)
				{
					printf("Detect HL2 SMD");
					hl2=1;
				}

				l=atoi(a[9]);
				bw=-1;
				for(k=0; k<l; k++)
				{
					cw=atof(a[10+k*2+1]);
					if(cw>bw)
					{
						j=atoi(a[10+k*2]);
						bw=cw;
					}
				}
			}

			MatNF_Copy(xforms+(j*16), cm, 4, 4);
			Vec3F_PointFromParentSpace(tv0, cm, rv0);
			Vec3F_NormalFromParentSpace(tv1, cm, rv1);

			for(k=0; k<3; k++)
			{
				if(rv0[k]<mdl->bone[j]->min[k])
					mdl->bone[j]->min[k]=rv0[k];
				if(rv0[k]>mdl->bone[j]->max[k])
					mdl->bone[j]->max[k]=rv0[k];
			}

			fmtsmd_face[n]->xyz[i]=LBXGL_FmtSmd_IndexXYZ(j, rv0);
			fmtsmd_face[n]->norm[i]=LBXGL_FmtSmd_IndexNorm(j, rv1);
			fmtsmd_face[n]->st[i]=LBXGL_FmtSmd_IndexST(tv2);
		}

		n++;
	}
	fmtsmd_num_face=n;

	mdl->num_xyz=fmtsmd_num_xyz;
	mdl->num_st=fmtsmd_num_st;
	mdl->num_norm=fmtsmd_num_norm;

	mdl->v_xyzbone=gcalloc(mdl->num_xyz*sizeof(short));
	mdl->v_xyz=gcalloc(3*mdl->num_xyz*sizeof(float));
	mdl->v_normbone=gcalloc(mdl->num_norm*sizeof(short));
	mdl->v_norm=gcalloc(3*mdl->num_norm*sizeof(float));
	mdl->v_st=gcalloc(2*mdl->num_st*sizeof(float));

	for(i=0; i<mdl->num_xyz; i++)
	{
		mdl->v_xyzbone[i]=fmtsmd_xyzbone[i];
		mdl->v_xyz[i*3+0]=fmtsmd_xyz[i*3+0];
		mdl->v_xyz[i*3+1]=fmtsmd_xyz[i*3+1];
		mdl->v_xyz[i*3+2]=fmtsmd_xyz[i*3+2];
	}
	for(i=0; i<mdl->num_norm; i++)
	{
		mdl->v_normbone[i]=fmtsmd_normbone[i];
		mdl->v_norm[i*3+0]=fmtsmd_norm[i*3+0];
		mdl->v_norm[i*3+1]=fmtsmd_norm[i*3+1];
		mdl->v_norm[i*3+2]=fmtsmd_norm[i*3+2];
	}
	for(i=0; i<mdl->num_st; i++)
	{
		mdl->v_st[i*2+0]=fmtsmd_st[i*2+0];
		mdl->v_st[i*2+1]=fmtsmd_st[i*2+1];
	}

	mdl->num_tris=fmtsmd_num_face;
	mdl->num_skin=fmtsmd_num_skin;

	mdl->tris=gcalloc(mdl->num_tris*sizeof(LBXGL_SkelFace *));
	mdl->skin_name=gcalloc(mdl->num_skin*sizeof(char *));
	mdl->skin_texnum=gcalloc(mdl->num_skin*sizeof(int));

	for(i=0; i<mdl->num_tris; i++)
		mdl->tris[i]=fmtsmd_face[i];
	for(i=0; i<mdl->num_skin; i++)
	{
		mdl->skin_name[i]=fmtsmd_skinname[i];
		s=gcrsprint("%s/%s", mdl->base, fmtsmd_skinname[i]);
		mdl->skin_texnum[i]=LBXGL_Texture_LoadImage(s);
	}

	printf("LBXGL_FmtSmd_LoadTriangles: Done\n");

	return(0);
}

LBXGL_SkelMesh *LBXGL_FmtSmd_LoadReference(char *name, char *base)
{
	LBXGL_SkelMesh *tmp;
	VFILE *fd;
	char *buf, **a;
	int i;

	printf("LBXGL_FmtSmd_LoadReference\n");

	fd=vffopen(name, "rt");
	if(!fd)
	{
		printf("LBXGL_FmtSmd_LoadReference: can't open '%s'\n", name);
		return(NULL);
	}
	tmp=gctalloc("lbxgl_skelmesh_t", sizeof(LBXGL_SkelMesh));
	tmp->anim=gcalloc(256*sizeof(LBXGL_SkelAnim *));
	tmp->base=dystrdup(base);

	buf=gcralloc(256);
	vfgets(buf, 255, fd);
	a=split_str(buf);
	if(!a[0] || strcmp(a[0], "version") || !a[1] || (atoi(a[1])!=1))
	{
		printf("LBXGL_FmtSmd_LoadReference: invalid header\n", name);
		return(NULL);
	}

	while(!vfeof(fd))
	{
		buf=gcralloc(256);
		memset(buf, 0, 256);
		vfgets(buf, 255, fd);

		a=split_str(buf);
		if(!a[0])continue;

		if(!strcmp(a[0], "nodes"))
			LBXGL_FmtSmd_LoadBaseBones(tmp, fd);
		if(!strcmp(a[0], "skeleton"))
			LBXGL_FmtSmd_LoadBaseFrame(tmp, fd);
		if(!strcmp(a[0], "triangles"))
			LBXGL_FmtSmd_LoadTriangles(tmp, fd);
	}

	printf("LBXGL_FmtSmd_LoadReference: %d skin %d tris, %d xyz, "
		"%d st, %d normals\n",
		tmp->num_skin, tmp->num_tris, tmp->num_xyz,
		tmp->num_st, tmp->num_norm);

	for(i=0; i<tmp->num_bones; i++)
	{
		if(tmp->bone[i]->min[0]>tmp->bone[i]->max[0])
		{
			//empty bone
			tmp->bone[i]->min[0]=0;
			tmp->bone[i]->max[0]=0;
			tmp->bone[i]->min[1]=0;
			tmp->bone[i]->max[1]=0;
			tmp->bone[i]->min[2]=0;
			tmp->bone[i]->max[2]=0;
		}

		tmp->bone[i]->com[0]=(tmp->bone[i]->min[0]+tmp->bone[i]->max[0])/2;
		tmp->bone[i]->com[1]=(tmp->bone[i]->min[1]+tmp->bone[i]->max[1])/2;
		tmp->bone[i]->com[2]=(tmp->bone[i]->min[2]+tmp->bone[i]->max[2])/2;

		tmp->bone[i]->vol[0]=(tmp->bone[i]->max[0]-tmp->bone[i]->min[0]);
		tmp->bone[i]->vol[1]=(tmp->bone[i]->max[1]-tmp->bone[i]->min[1]);
		tmp->bone[i]->vol[2]=(tmp->bone[i]->max[2]-tmp->bone[i]->min[2]);

		tmp->bone[i]->weight=tmp->bone[i]->vol[0]*
			tmp->bone[i]->vol[1]*tmp->bone[i]->vol[2];

/*
		printf("bone %d:\n"
				"\tmin (%.3f %.3f %.3f) max (%.3f %.3f %.3f)\n"
				"\tvol (%.3f %.3f %.3f) mass %.3f\n",
			i,
			tmp->bone[i]->min[0], tmp->bone[i]->min[1],
				tmp->bone[i]->min[2],
			tmp->bone[i]->max[0], tmp->bone[i]->max[1],
				tmp->bone[i]->max[2],
			tmp->bone[i]->vol[0], tmp->bone[i]->vol[1],
				tmp->bone[i]->vol[2],
			tmp->bone[i]->weight);
*/
	}

	printf("LBXGL_FmtSmd_LoadReference: Done\n");

	return(tmp);
}



//Load SMD Animation

int LBXGL_FmtSmd_LoadAnimBones(LBXGL_SkelAnim *anim, VFILE *fd)
{
	char **bone;
	char *buf, **a;
	int i, j, n;

	bone=gcalloc(256*sizeof(char *));

	n=0;
	while(!vfeof(fd))
	{
		buf=gcralloc(256);
		memset(buf, 0, 256);
		vfgets(buf, 255, fd);

		a=split_str(buf);
		if(!a[0])continue;

		if(!strcmp(a[0], "end"))
			break;

		i=atoi(a[0]);
		bone[i]=dystrdup(a[1]);
		if((i+1)>n)n=i+1;
	}

	anim->num_bones=n;
	anim->boneref=gcalloc(n*sizeof(char *));
	for(i=0; i<n; i++)anim->boneref[i]=bone[i];
	gcfree(bone);

	return(0);
}

int LBXGL_FmtSmd_LoadAnimFrames(LBXGL_SkelAnim *anim, VFILE *fd)
{
	float *pos;
	char *buf, **a;
	int i, j, k, n;

//	if(!pos)pos=malloc(65536*sizeof(float));
//	memset(pos, 0, 65536*sizeof(float));

	anim->bonepos=gcalloc(1000*sizeof(float *));

	i=0;
	n=0;
	while(!vfeof(fd))
	{
		buf=gcralloc(256);
		memset(buf, 0, 256);
		vfgets(buf, 255, fd);

		a=split_str(buf);
		if(!a[0])continue;

		if(!strcmp(a[0], "end"))
			break;

		if(!strcmp(a[0], "time"))
		{
			i=atoi(a[1]);
			pos=gcalloc(anim->num_bones*6*sizeof(float));
			anim->bonepos[i]=pos;

			if((i+1)>n)n=i+1;
			continue;
		}

		j=atoi(a[0]);
		for(k=0; k<6; k++)
			pos[j*6+k]=atof(a[1+k]);
	}

	anim->num_frames=n;

/*
	anim->bonepos=gcalloc(anim->num_frames*anim->num_bones*6*sizeof(float));
	for(i=0; i<anim->num_frames*anim->num_bones; i++)
		for(j=0; j<6; j++)
	{
		anim->bonepos[i*6+j]=pos[i*6+j];
	}
*/

	return(0);
}

LBXGL_SkelAnim *LBXGL_FmtSmd_LoadAnimation(LBXGL_SkelMesh *mdl, char *sname, char *fname)
{
	LBXGL_SkelAnim *tmp;
	VFILE *fd;
	char *buf, **a;

	fd=vffopen(fname, "rt");
	if(!fd)
	{
		printf("LBXGL_FmtSmd_LoadAnimation: can't open '%s'\n", fname);
		return(NULL);
	}
//	tmp=ObjType_New("lbxgl_skelanim_t", sizeof(LBXGL_SkelMesh));

	buf=gcralloc(256);
	vfgets(buf, 255, fd);
	a=split_str(buf);
	if(!a[0] || strcmp(a[0], "version") || !a[1] || (atoi(a[1])!=1))
	{
		printf("LBXGL_FmtSmd_LoadAnimation: invalid header\n", fname);
		return(NULL);
	}

	tmp=gcalloc(sizeof(LBXGL_SkelAnim));
	mdl->anim[mdl->num_anims++]=tmp;

	tmp->name=dystrdup(sname);
	tmp->next=NULL;
	tmp->fps=24;

	while(!vfeof(fd))
	{
		buf=gcralloc(256);
		memset(buf, 0, 256);
		vfgets(buf, 255, fd);

		a=split_str(buf);
		if(!a[0])continue;

		if(!strcmp(a[0], "nodes"))
			LBXGL_FmtSmd_LoadAnimBones(tmp, fd);
		if(!strcmp(a[0], "skeleton"))
			LBXGL_FmtSmd_LoadAnimFrames(tmp, fd);
	}
	return(tmp);
}



//Load SMD Model Info

void *LBXGL_FmtSmd_String2Object(char *str)
{
	double *fa;
	char *s;
	char **as2;
	void *p;

	int i, n;

	s=str;
	while(*s && (*s<=' '))s++;
	if(((*s>='0') && (*s<='9')) || (*s=='-'))
	{
		as2=split_str(str);
		for(n=0; as2[n]; n++);

		if(n>1)
		{
			fa=gctalloc("bggl2_numarray_t", n*sizeof(double));
			for(i=0; i<n; i++)fa[i]=atof(as2[i]);
			p=fa;
		}else p=dyfloat(atof(str));
	}else p=dystrdup(str);

	return(p);
}

LBXGL_SkelMesh *LBXGL_FmtSmd_LoadModelInfo(char *name)
{
	LBXGL_SkelMesh *mdl;
	LBXGL_SkelAnim *anim;
	LBXGL_SkelEvent *lev, *ev;
	VFILE *fd;
	char *buf, **a;
	char *base, *s;
	float unit;
	int i, j;

	printf("LBXGL_FmtSmd_LoadModelInfo\n");

	fd=vffopen(name, "rt");
	if(!fd)
	{
		printf("LBXGL_FmtSmd_LoadModelInfo: can't open '%s'\n", name);
		return(NULL);
	}

	buf=gcalloc(256);

	//by default, the base is relative to the location of the info file
	base=NULL;
	strcpy(buf, name);
	s=buf+strlen(buf);
	while((s>buf) && (*s!='/') && (*s!='\\'))s--;
	*s=0;
	if(*buf)base=dystrdup(buf);


	unit=1.0;
	mdl=NULL;
	anim=NULL;
	lev=NULL;
	while(!vfeof(fd))
	{
		memset(buf, 0, 256);
		vfgets(buf, 255, fd);

		a=split_str(buf);
		if(!a[0])continue;

		if(!strncmp(a[0], "//", 2))
			continue;

		if(!strcmp(a[0], "base"))base=dystrdup(a[1]);
		if(!strcmp(a[0], "unit"))unit=atof(a[1]);

		if(!strcmp(a[0], "body") || !strcmp(a[0], "$model"))
		{
			s=gcrsprint("%s/%s.smd", base, a[1]);
			mdl=LBXGL_FmtSmd_LoadReference(s, base);
			mdl->unit=unit;

			continue;
		}
		if(!strcmp(a[0], "sequence"))
		{
			s=gcrsprint("%s/%s.smd", base, a[2]);
			anim=LBXGL_FmtSmd_LoadAnimation(mdl, a[1], s);

			a=split_str(buf);	//resplit
			for(j=3; a[j]; j++)
			{
				if(!strcmp(a[j], "fps"))
					anim->fps=atof(a[j+1]);
				if(!strcmp(a[j], "next"))
					anim->next=dystrdup(a[j+1]);
				if(!strcmp(a[j], "rnext"))
				{
					anim->rnext[anim->num_rnext]=
						dystrdup(a[j+1]);
					anim->rnext_weight[anim->num_rnext]=
						atoi(a[j+2]);
					anim->num_rnext++;
				}
			}

			lev=NULL;

			continue;
		}

		if(!strcmp(a[0], "event"))
		{
			ev=gcalloc(sizeof(LBXGL_SkelEvent));
			ev->frame=atoi(a[1]);
			ev->target=dystrdup(a[2]);
			ev->event=dystrdup(a[3]);

			for(i=0; a[i+4]; i++)
			{
				ev->args[i]=LBXGL_FmtSmd_String2Object(a[i+4]);
			}

			if(lev)lev->next=ev;
				else anim->events=ev;
			lev=ev;

			continue;
		}

		if(!strcmp(a[0], "message"))
		{
			ev=gcalloc(sizeof(LBXGL_SkelEvent));
			ev->frame=atoi(a[1]);
			ev->target=dystrdup("SELF");
			ev->event=dystrdup(a[2]);

			for(i=0; a[i+3]; i++)
			{
				ev->args[i]=LBXGL_FmtSmd_String2Object(a[i+3]);
			}

			if(lev)lev->next=ev;
				else anim->events=ev;
			lev=ev;

			continue;
		}
	}
	return(mdl);
}



//Utility Stuff
int LBXGL_FmtSmd_IndexAnim(LBXGL_SkelMesh *mdl, char *name)
{
	int i;

	for(i=0; i<mdl->num_anims; i++)
		if(!strcmp(mdl->anim[i]->name, name))
			return(i);
	return(-1);
}



//Render SMD Mesh

int LBXGL_FmtSmd_CalcTransforms(LBXGL_SkelMesh *mdl, int seq, int frame,
	float *xforms)
{
	float tmat[16], tmat2[16], pmat[16], tmat3[16];
	float pos[3], rot[3], q[4];
	int i, j, k;

	for(i=0; i<mdl->num_bones; i++)
	{
		if((seq>=0) && mdl->anim[seq])
			frame%=mdl->anim[seq]->num_frames;

		pos[0]=mdl->bone[i]->pos[0];
		pos[1]=mdl->bone[i]->pos[1];
		pos[2]=mdl->bone[i]->pos[2];
		rot[0]=mdl->bone[i]->rot[0];
		rot[1]=mdl->bone[i]->rot[1];
		rot[2]=mdl->bone[i]->rot[2];

		if(seq>=0)for(j=0; j<mdl->anim[seq]->num_bones; j++)
			if(!strcmp(mdl->anim[seq]->boneref[j], mdl->bone[i]->name))
		{
			pos[0]=mdl->anim[seq]->bonepos[frame][j*6+0];
			pos[1]=mdl->anim[seq]->bonepos[frame][j*6+1];
			pos[2]=mdl->anim[seq]->bonepos[frame][j*6+2];
			rot[0]=mdl->anim[seq]->bonepos[frame][j*6+3];
			rot[1]=mdl->anim[seq]->bonepos[frame][j*6+4];
			rot[2]=mdl->anim[seq]->bonepos[frame][j*6+5];

			break;
		}

/*
		pos[0]-=mdl->bone[i]->pos[0];
		pos[1]-=mdl->bone[i]->pos[1];
		pos[2]-=mdl->bone[i]->pos[2];
		rot[0]-=mdl->bone[i]->rot[0];
		rot[1]-=mdl->bone[i]->rot[1];
		rot[2]-=mdl->bone[i]->rot[2];
*/

		LBXGL_FmtSmd_FromAngles(rot, q);
		LBXGL_FmtSmd_ToMatrix(q, tmat);
		tmat[3]=pos[0];
		tmat[7]=pos[1];
		tmat[11]=pos[2];

		if(mdl->bone[i]->parent>=0)
		{
//			MatNF_MatMult(&xforms[mdl->bone[i]->parent*16], 4, 4,
//				tmat, 4, 4, tmat2);
			Mat4F_MatMult(&xforms[mdl->bone[i]->parent*16],
				tmat, tmat2);
			MatNF_Copy(tmat2, &xforms[i*16], 4, 4);
		}else MatNF_Copy(tmat, &xforms[i*16], 4, 4);
	}

	return(0);
}

#if 0
int LBXGL_FmtSmd_CalcTransformsQuat(float *orgs, float *quats, int num,
	float *xforms)
{
	float tmat[16], tmat2[16], pmat[16], tmat3[16];
	float pos[3], rot[3], q[4];
	int i, j, k;

	for(i=0; i<num; i++)
	{

		pos[0]=orgs[i*3+0];
		pos[1]=orgs[i*3+1];
		pos[2]=orgs[i*3+2];
		rot[0]=mdl->bone[i]->rot[0];
		rot[1]=mdl->bone[i]->rot[1];
		rot[2]=mdl->bone[i]->rot[2];

		if(seq>=0)for(j=0; j<mdl->anim[seq]->num_bones; j++)
			if(!strcmp(mdl->anim[seq]->boneref[j], mdl->bone[i]->name))
		{
			pos[0]=mdl->anim[seq]->bonepos[frame][j*6+0];
			pos[1]=mdl->anim[seq]->bonepos[frame][j*6+1];
			pos[2]=mdl->anim[seq]->bonepos[frame][j*6+2];
			rot[0]=mdl->anim[seq]->bonepos[frame][j*6+3];
			rot[1]=mdl->anim[seq]->bonepos[frame][j*6+4];
			rot[2]=mdl->anim[seq]->bonepos[frame][j*6+5];

			break;
		}

/*
		pos[0]-=mdl->bone[i]->pos[0];
		pos[1]-=mdl->bone[i]->pos[1];
		pos[2]-=mdl->bone[i]->pos[2];
		rot[0]-=mdl->bone[i]->rot[0];
		rot[1]-=mdl->bone[i]->rot[1];
		rot[2]-=mdl->bone[i]->rot[2];
*/

		LBXGL_FmtSmd_FromAngles(rot, q);
		LBXGL_FmtSmd_ToMatrix(q, tmat);
		tmat[3]=pos[0];
		tmat[7]=pos[1];
		tmat[11]=pos[2];

		if(mdl->bone[i]->parent>=0)
		{
			MatNF_MatMult(&xforms[mdl->bone[i]->parent*16], 4, 4,
				tmat, 4, 4, tmat2);
			MatNF_Copy(tmat2, &xforms[i*16], 4, 4);
		}else MatNF_Copy(tmat, &xforms[i*16], 4, 4);
	}

	return(0);
}
#endif

int LBXGL_FmtSmd_RenderModel(LBXGL_SkelMesh *mdl, float *xforms)
{
	int i, j, k, vt;
	float cm[16], c[4], ss, ts;
	short *cmds;
	float *vec, *transvecs;

	vt=0;

	transvecs=gcalloc(mdl->num_xyz*3*sizeof(float));
	for(i=0; i<mdl->num_xyz; i++)
	{
		j=mdl->v_xyzbone[i];

		MatNF_Copy(xforms+(j*16), cm, 4, 4);
		Vec3F_PointToParentSpace(mdl->v_xyz+(i*3), cm,
			transvecs+(i*3));
//		VecNF_Copy(&mdl->v_xyz[i*3], &transvecs[i*3], 3);
	}

	if(vt)glDisable(GL_TEXTURE_2D);
		else pdglEnable(GL_TEXTURE_2D);

//	pdglColor4f(1, 1, 1, 1);

	for(i=0; i<mdl->num_tris; i++)
	{
		Draw_Bind(mdl->skin_texnum[mdl->tris[i]->skinnum]);
		pdglBegin(PDGL_POLYGON);
		for(j=0; j<3; j++)
		{
			k=mdl->tris[i]->st[j];
			pdglTexCoord2fv(mdl->v_st+(k*2));

			k=mdl->tris[i]->xyz[j];
			pdglVertex3fv(transvecs+(k*3));
		}
		pdglEnd();
	}

	gcfree(transvecs);

	pdglEnableTexture2D();

	return(0);
}

int LBXGL_FmtSmd_RenderBox(
	float *xform, float *min, float *max, float *c)
{
	int i, j;

	float vtxa[8*3], vt[3];
	static int vtxb[6*4]=
	{
		1, 3, 2, 0,
		4, 6, 7, 5,
		5, 7, 3, 1,
		0, 2, 6, 4,
		5, 1, 0, 4,
		3, 7, 6, 2
	};

	float *pt1, *pt2, *pt3, *pt4;

	pdglDisableTexture2D();
	pdglEnable(GL_BLEND);

	for(i=0; i<8; i++)
	{
		vt[0]=(i&1)?max[0]:min[0];
		vt[1]=(i&2)?max[1]:min[1];
		vt[2]=(i&4)?max[2]:min[2];
//		VecNF_Copy(vt, &vtxa[i*3], 3);
		Vec3F_PointToParentSpace(vt, xform, &vtxa[i*3]);
	}

	pdglColor4fv(c);

	pdglBegin(PDGL_LINES);
	for(i=0; i<6; i++)
	{
		pt1=vtxa+(vtxb[(i*4)+0]*3);
		pt2=vtxa+(vtxb[(i*4)+1]*3);
		pt3=vtxa+(vtxb[(i*4)+2]*3);
		pt4=vtxa+(vtxb[(i*4)+3]*3);

		pdglVertex3fv(pt1);
		pdglVertex3fv(pt2);

		pdglVertex3fv(pt2);
		pdglVertex3fv(pt3);

		pdglVertex3fv(pt3);
		pdglVertex3fv(pt4);

		pdglVertex3fv(pt4);
		pdglVertex3fv(pt1);
	}
	pdglEnd();

	return(0);
}

int LBXGL_FmtSmd_RenderHitboxes(LBXGL_SkelMesh *mdl, float *xforms)
{
	float c[4];
	int i;

	pdglDisableTexture2D();

	c[0]=1;
	c[1]=0;
	c[2]=0;
	c[3]=1;

	for(i=0; i<mdl->num_bones; i++)
	{
		LBXGL_FmtSmd_RenderBox(xforms+(i*16),
			mdl->bone[i]->min, mdl->bone[i]->max, c);
	}

	return(0);
}

static float norm_i[3]={1, 0, 0};
static float norm_j[3]={0, 1, 0};
static float norm_k[3]={0, 0, 1};

int LBXGL_FmtSmd_RenderBones(LBXGL_SkelMesh *mdl, float *xforms)
{
	int i, j;
	float o[3], o2[3], pv[3], cv[3], ci[3], cj[3], ck[3];
	float mo[3];
	float pm[16], cm[16];

	for(i=0; i<3; i++)o[i]=0;

	pdglDisableTexture2D();
	pdglDisable(GL_DEPTH_TEST);

	pdglBegin(PDGL_LINES);
	for(i=0; i<mdl->num_bones; i++)
	{
		j=mdl->bone[i]->parent;
		if(j!=-1)
		{
			MatNF_Copy(&xforms[j*16], pm, 4, 4);
			MatNF_Copy(&xforms[i*16], cm, 4, 4);
			Vec3F_PointToParentSpace(o, pm, pv);
			Vec3F_PointToParentSpace(o, cm, cv);
			Vec3F_PointToParentSpace(norm_i, cm, ci);
			Vec3F_PointToParentSpace(norm_j, cm, cj);
			Vec3F_PointToParentSpace(norm_k, cm, ck);

			Vec3F_PointToParentSpace(mdl->bone[i]->com, cm, mo);

			pdglColor4f(1, 1, 0, 1);
			pdglVertex3f(pv[0], pv[1], pv[2]);
			pdglVertex3f(cv[0], cv[1], cv[2]);

//			pdglColor4f(0, 0, 1, 1);
//			pdglVertex3f(cv[0], cv[1], cv[2]);
//			pdglVertex3f(cv2[0], cv2[1], cv2[2]);

			pdglColor4f(1, 0, 0, 1);
			pdglVertex3f(cv[0], cv[1], cv[2]);
			pdglVertex3f(ci[0], ci[1], ci[2]);
			pdglColor4f(0, 1, 0, 1);
			pdglVertex3f(cv[0], cv[1], cv[2]);
			pdglVertex3f(cj[0], cj[1], cj[2]);
			pdglColor4f(0, 0, 1, 1);
			pdglVertex3f(cv[0], cv[1], cv[2]);
			pdglVertex3f(ck[0], ck[1], ck[2]);


			pdglColor4f(0, 0.5, 0.5, 1);
			pdglVertex3f(cv[0], cv[1], cv[2]);
			pdglVertex3f(mo[0], mo[1], mo[2]);

//			pdglColor4f(0, 1, 0, 1);
//			pdglVertex3f(pv[0]-1, pv[1]-1, pv[2]-1);
//			pdglVertex3f(pv[0]+1, pv[1]+1, pv[2]+1);

//			pdglColor4f(0, 1, 0, 1);
//			pdglVertex3f(cv[0]-1, cv[1]-1, cv[2]-1);
//			pdglVertex3f(cv[0]+1, cv[1]+1, cv[2]+1);
		}
	}
	pdglEnd();

	pdglEnable(GL_DEPTH_TEST);
	pdglEnableTexture2D();

	return(0);
}


int LBXGL_FmtSmd_Render(LBXGL_SkelMesh *mdl, int seq, int frame)
{
	float *xforms;

	xforms=gcalloc(mdl->num_bones*16*sizeof(float));
	LBXGL_FmtSmd_CalcTransforms(mdl, seq, frame, xforms);

//	pdglMatrixMode(GL_MODELVIEW);
	pdglModelviewMatrix();
	pdglPushMatrix();

//	pdglScalef(1.0/36.0, 1.0/36.0, 1.0/36.0);
	pdglScalef(1.0/mdl->unit, 1.0/mdl->unit, 1.0/mdl->unit);
	pdglColor4f(1, 1, 1, 1);

	LBXGL_FmtSmd_RenderModel(mdl, xforms);
	LBXGL_FmtSmd_RenderBones(mdl, xforms);
	LBXGL_FmtSmd_RenderHitboxes(mdl, xforms);

//	pdglMatrixMode(GL_MODELVIEW);
	pdglPopMatrix();

	gcfree(xforms);
	return(0);
}


int LBXGL_FmtSmd_SetStateFrame(LBXGL_SkelState *state, int seq, int frame)
{
	float tmat[16], tmat2[16], pmat[16], tmat3[16];
	float pos[3], rot[3], q[4];
	int i, j, k;

	for(i=0; i<state->mesh->num_bones; i++)
	{
		if((seq>=0) && state->mesh->anim[seq])
			frame%=state->mesh->anim[seq]->num_frames;

		pos[0]=state->mesh->bone[i]->pos[0];
		pos[1]=state->mesh->bone[i]->pos[1];
		pos[2]=state->mesh->bone[i]->pos[2];
		rot[0]=state->mesh->bone[i]->rot[0];
		rot[1]=state->mesh->bone[i]->rot[1];
		rot[2]=state->mesh->bone[i]->rot[2];

		if(seq>=0)for(j=0; j<state->mesh->anim[seq]->num_bones; j++)
			if(!strcmp(state->mesh->anim[seq]->boneref[j],
				state->mesh->bone[i]->name))
		{
			pos[0]=state->mesh->anim[seq]->bonepos[frame][j*6+0];
			pos[1]=state->mesh->anim[seq]->bonepos[frame][j*6+1];
			pos[2]=state->mesh->anim[seq]->bonepos[frame][j*6+2];
			rot[0]=state->mesh->anim[seq]->bonepos[frame][j*6+3];
			rot[1]=state->mesh->anim[seq]->bonepos[frame][j*6+4];
			rot[2]=state->mesh->anim[seq]->bonepos[frame][j*6+5];

			break;
		}

		for(j=0; j<3; j++)
		{
			state->basepos[i*6+j]=pos[j];
			state->basepos[i*6+3+j]=rot[j];

			state->relpos[i*6+j]=0;
			state->relpos[i*6+3+j]=0;
		}
	}

	return(0);
}
