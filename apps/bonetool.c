#include <windows.h>

#include <GL/gl.h>

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
// #include <unistd.h>

#ifndef M_PI
#define M_PI		3.1415926535897932384626433832795
#endif

#include <pdlib.h>

#include <pdgl.h>
#include <pdglui.h>
#include <pdgl_stub.h>

#include <lbxgl2.h>
#include <libvecmath.h>


float main_cam_org[3];
float main_cam_ang[3];
float main_cam_vel[3];
int main_cam_id;
int main_cam_raw=1;

float main_time;
float main_speed=12;

float main_cam_fw[3];
float main_cam_rt[3];
float main_cam_up[3];

float main_morg[3];
float main_mdir[3];


int main_bones;
int main_bone_up[64];
char *main_bone_name[64];
float main_bone_org[64*3];	//org in base frame
float main_bone_rot[64*9];	//rot in base frame

float main_bone_rorg[64*3];	//temp frame org
float main_bone_rmat[64*9];	//temp frame rot
int main_bone_sel;

float frame_org[256*64*3];	//org in each frame
float frame_rot[256*64*9];	//rot in each frame
byte frame_mask[256*64/8];	//which keyframes are set
int frame_cur, n_frames;
float frame_time;

float *mesh_vecs[64];
int *mesh_tris[64];
int mesh_nvecs[64];
int mesh_ntris[64];

float main_mesh_org[64*3];	//mesh translation
float main_mesh_rot[64*9];	//mesh rotation
LBXGL_Skel2MeshGroup *mesh_grp[64];
LBXGL_Skel2MeshGroup *mesh_root;		//vertex-skinned meshes

struct {
int type;
int bone;
float m[3];	//point 0 for capsules, min for bbox
float n[3];	//point 1 for capsules, max for bbox
float r0, r1;	//radius inner and outer
float w;
}bone_solid[256];

int bone_solids;

#define BONE_SOLID_BBOX		1
#define BONE_SOLID_CAPSULE	2
#define BONE_SOLID_SPHERE	3


char *bones_file;
char *model_file;
char *frame_file;

int main_loadsave;
int main_mode;

#define MODE_BONES	1
#define MODE_ANIMATE	2
#define MODE_MESH	3

#define MODE_FIRST	1
#define MODE_LAST	3


void main_BindMesh(LBXGL_Skel2Mesh *mesh);
void main_BindMeshGroup(LBXGL_Skel2MeshGroup *grp);

int main_load_rootmesh(char *name)
{
	mesh_root=AC3D_LoadModel(name);
	main_BindMeshGroup(mesh_root);
}

void main_DrawMesh(LBXGL_Skel2Mesh *mesh)
{
	static float txyz[65536*3];
	static float tnorm[16384*3];
	LBXGL_Skel2MeshFace *cur, *cur2;
	float im[16];
	float tv[3], tv0[3], tv1[3], tv2[3];
	float *v0, *v1, *imv;
	int i, j, k, l, l1;
	float f, g;

	//mesh is unbound
	if(!mesh->vidx)
	{
		v0=main_bone_rorg;
		imv=main_bone_rmat;

		Mat4F_Identity(im);
//		im[0]=imv[0];  im[1]=imv[1];  im[2]=imv[2];
//		im[4]=imv[3];  im[5]=imv[4];  im[6]=imv[5];
//		im[8]=imv[6];  im[9]=imv[7];  im[10]=imv[8];
//		im[12]=v0[0]; im[13]=v0[1]; im[14]=v0[2];

		AC3D_DrawMesh(mesh, im);
		return;
	}


	//calculate vertex poses
	for(i=0; i<mesh->n_vecs; i++)
	{
		V3F_ZERO(txyz+i*3);
		g=0;

		j=mesh->vidx[i];
		k=mesh->vnum[i];
		while(k--)
		{
			v0=mesh->wxyz+j*3;
			v1=main_bone_rorg+mesh->widx[j]*3;
			imv=main_bone_rmat+mesh->widx[j]*9;

			tv[0]=v0[0]*imv[0]+v0[1]*imv[3]+v0[2]*imv[6]+v1[0];
			tv[1]=v0[0]*imv[1]+v0[1]*imv[4]+v0[2]*imv[7]+v1[1];
			tv[2]=v0[0]*imv[2]+v0[1]*imv[5]+v0[2]*imv[8]+v1[2];

			f=mesh->wval[j];
			txyz[i*3+0]+=tv[0]*f;
			txyz[i*3+1]+=tv[1]*f;
			txyz[i*3+2]+=tv[2]*f;
			g+=f;

			j++;
		}

		if(g<0.01)g=1;
		txyz[i*3+0]/=g;
		txyz[i*3+1]/=g;
		txyz[i*3+2]/=g;
	}

	//calc face normals
	for(i=0; i<mesh->n_faces; i++)
	{
		cur=mesh->face[i];
		V3F_SUB(txyz+cur->vecs[1]*3, txyz+cur->vecs[0]*3, tv0);
		V3F_SUB(txyz+cur->vecs[2]*3, txyz+cur->vecs[1]*3, tv1);
		V3F_CROSS(tv0, tv1, tv2);
		V3F_NORMALIZE(tv2, tnorm+i*3);
	}

	glDisable(GL_TEXTURE_2D);

	if(mesh->texnum>0)
	{
		glEnable(GL_TEXTURE_2D);
		glBindTexture(GL_TEXTURE_2D, mesh->texnum);
	}

	for(i=0; i<mesh->n_faces; i++)
	{
		cur=mesh->face[i];

		glNormal3fv(tnorm+i*3);

		glBegin(GL_POLYGON);
		for(j=0; j<cur->n_vecs; j++)
		{
			V3F_ZERO(tv);
			k=cur->vecs[j];
			for(l=0; l<mesh->vfnum[k]; l++)
			{
				l1=mesh->vfids[mesh->vfidx[k]+l];
				f=V3F_DOT(tnorm+i*3, tnorm+l1*3);
//				f=1;

				if(f>1)f=1;
				if(f>0)
				{
					V3F_ADDSCALE(tv, tnorm+l1*3, f, tv);
				}
			}
			glNormal3fv(tv);


			glTexCoord2fv(mesh->stvecs+(2*cur->stvecs[j]));
			V3F_COPY(txyz+(3*cur->vecs[j]), tv);

//			f=V3F_DIST(tv, main_cam_org)/12.0;
//			glColor4f(1.0/f, 1.0/f, 1.0/f, 1);

			glVertex3fv(tv);
		}
		glEnd();

		cur=cur->next;
	}
}

void main_DrawMeshGroup(LBXGL_Skel2MeshGroup *grp)
{
	LBXGL_Skel2MeshGroup *gcur;
	LBXGL_Skel2Mesh *mcur;

	gcur=grp->first;
	while(gcur)
	{
		main_DrawMeshGroup(gcur);
		gcur=gcur->next;
	}

	mcur=grp->prim;
	while(mcur)
	{
		main_DrawMesh(mcur);
		mcur=mcur->next;
	}
}

float main_WeightVertexSolid(float *pt, int num)
{
	float tv[3], tv1[3];
	float *v0, *v1;
	float f, g, d;
	int i, j, k;

	i=bone_solid[num].bone;
	V3F_SUB(pt, main_bone_rorg+3*i, tv1);
	tv[0]=V3F_DOT(tv1, main_bone_rmat+9*i+0);
	tv[1]=V3F_DOT(tv1, main_bone_rmat+9*i+3);
	tv[2]=V3F_DOT(tv1, main_bone_rmat+9*i+6);	

	v0=bone_solid[num].m;
	v1=bone_solid[num].n;
	switch(bone_solid[num].type)
	{
	case BONE_SOLID_BBOX:
		for(i=0; i<3; i++)
		{
			if(tv[i]<v0[i])
			{
				d=v0[i]-tv[i];
				if(d>=bone_solid[num].r0)
					return(0);
				f=1.0-(d/bone_solid[num].r0);
				return(f*bone_solid[num].w);
			}
			if(tv[i]>v1[i])
			{
				d=tv[i]-v1[i];
				if(d>=bone_solid[num].r0)
					return(0);
				f=1.0-(d/bone_solid[num].r0);
				return(f*bone_solid[num].w);
			}
		}
		return(bone_solid[num].w);
		break;

	case BONE_SOLID_SPHERE:
		d=V3F_DIST(tv, v0);
		if(d<=bone_solid[num].r0)
			return(bone_solid[num].w);
		if(d>=bone_solid[num].r1)
			return(0);
		d=d-bone_solid[num].r0;
		f=bone_solid[num].r1-bone_solid[num].r0;
		g=1.0-(d/f);
		return(f*bone_solid[num].w);
		break;

	case BONE_SOLID_CAPSULE:
		d=Vec3F_DistLineSeg(tv, v0, v1);
		if(d<=bone_solid[num].r0)
			return(bone_solid[num].w);
		if(d>=bone_solid[num].r1)
			return(0);
		d=d-bone_solid[num].r0;
		f=bone_solid[num].r1-bone_solid[num].r0;
		g=1.0-(d/f);
		return(f*bone_solid[num].w);
		break;

	default:
		break;
	}

	return(0);
}

int main_WeightVertex(float *pt, float *txyz, int *tidx, float *tval, int n)
{
	float tv[3], tv1[3], bv[3];
	int i, j, k, bi, n0;
	float d, bd;

	n0=n;

	for(i=0; i<bone_solids; i++)
	{
		d=main_WeightVertexSolid(pt, i);

		if(d>0)
		{
			j=bone_solid[i].bone;

			//add to any existing weights
			for(k=n0; k<n; k++)
				if(tidx[k]==j)break;
			if(k<n)
			{
				tval[k]+=d;
				continue;
			}

			//add new weight
			V3F_SUB(pt, main_bone_rorg+3*j, tv1);
			tv[0]=V3F_DOT(tv1, main_bone_rmat+9*j+0);
			tv[1]=V3F_DOT(tv1, main_bone_rmat+9*j+3);
			tv[2]=V3F_DOT(tv1, main_bone_rmat+9*j+6);

			V3F_COPY(tv, txyz+n*3);
			tidx[n]=j;
			tval[n]=d;
			n++;
		}
	}

//	if(!bone_solids)
//	if(1)
	if(n==n0)
	{
		V3F_ZERO(bv);
		bi=-1;
		bd=-999999;

		//bones to parent bones
		for(j=0; j<main_bones; j++)
		{
			k=main_bone_up[j];
			if(k<0)continue;

			d=Vec3F_DistLineSeg(pt,
				main_bone_rorg+3*j,
				main_bone_rorg+3*k);
			if(d>1)d=1.0/d; else d=1;
			if(d<0.01)continue;

			V3F_SUB(pt, main_bone_rorg+3*k, tv);
			tv1[0]=V3F_DOT(tv, main_bone_rmat+9*k+0);
			tv1[1]=V3F_DOT(tv, main_bone_rmat+9*k+3);
			tv1[2]=V3F_DOT(tv, main_bone_rmat+9*k+6);

			if(d>bd)
			{
				V3F_COPY(tv1, bv);
				bi=k; bd=d;
			}
		}

		V3F_COPY(bv, txyz+n*3);
		tidx[n]=bi;
		tval[n]=bd;
		n++;
	}

	return(n);
}

void main_BindMesh(LBXGL_Skel2Mesh *mesh)
{
	static float txyz[65536*3];
	static int tidx[65536];
	static float tval[65536];

	LBXGL_Skel2MeshFace *cur;
	float tv[3], tv1[3], bv[3];
	int i, j, k, n, bi;
	float d, bd;

	if(mesh->vidx) { free(mesh->vidx); mesh->vidx=NULL; }
	if(mesh->vnum) { free(mesh->vnum); mesh->vnum=NULL; }
	if(mesh->wxyz) { free(mesh->wxyz); mesh->wxyz=NULL; }
	if(mesh->widx) { free(mesh->widx); mesh->widx=NULL; }
	if(mesh->wval) { free(mesh->wval); mesh->wval=NULL; }

	mesh->vidx=malloc(mesh->n_vecs*sizeof(int));
	mesh->vnum=malloc(mesh->n_vecs*sizeof(int));

	n=0;
	for(i=0; i<mesh->n_vecs; i++)
	{
		mesh->vidx[i]=n;

		n=main_WeightVertex(mesh->vecs+i*3, txyz, tidx, tval, n);

		mesh->vnum[i]=n-mesh->vidx[i];
	}

	mesh->wxyz=malloc(3*n*sizeof(float));
	mesh->widx=malloc(n*sizeof(int));
	mesh->wval=malloc(n*sizeof(float));

	for(i=0; i<n; i++)
	{
		V3F_COPY(txyz+i*3, mesh->wxyz+i*3);
		mesh->widx[i]=tidx[i];
		mesh->wval[i]=tval[i];
	}

	mesh->n_weights=n;


	cur=mesh->faces;
	while(cur)
	{
		for(i=0; i<cur->n_vecs; i++)
		{
			V3F_COPY(mesh->vecs+(3*cur->vecs[i]), tv);
			glVertex3fv(tv);
		}

		cur=cur->next;
	}
}

void main_BindMeshGroup(LBXGL_Skel2MeshGroup *grp)
{
	LBXGL_Skel2MeshGroup *gcur;
	LBXGL_Skel2Mesh *mcur;

	gcur=grp->first;
	while(gcur)
	{
		main_BindMeshGroup(gcur);
		gcur=gcur->next;
	}

	mcur=grp->prim;
	while(mcur)
	{
		main_BindMesh(mcur);
		mcur=mcur->next;
	}
}

void main_UnbindMesh(LBXGL_Skel2Mesh *mesh)
{
	static float txyz[65536*3];
	static int tidx[65536];
	static float tval[65536];

	if(mesh->vidx) { free(mesh->vidx); mesh->vidx=NULL; }
	if(mesh->vnum) { free(mesh->vnum); mesh->vnum=NULL; }
	if(mesh->wxyz) { free(mesh->wxyz); mesh->wxyz=NULL; }
	if(mesh->widx) { free(mesh->widx); mesh->widx=NULL; }
	if(mesh->wval) { free(mesh->wval); mesh->wval=NULL; }
}

void main_UnbindMeshGroup(LBXGL_Skel2MeshGroup *grp)
{
	LBXGL_Skel2MeshGroup *gcur;
	LBXGL_Skel2Mesh *mcur;

	gcur=grp->first;
	while(gcur)
	{
		main_UnbindMeshGroup(gcur);
		gcur=gcur->next;
	}

	mcur=grp->prim;
	while(mcur)
	{
		main_UnbindMesh(mcur);
		mcur=mcur->next;
	}
}

int main_draw_rootmesh()
{
	if(!mesh_root)return(0);

	main_DrawMeshGroup(mesh_root);
}


int main_load_mesh(int num, char *name, float *pos)
{
	static float vecs[4096*3];
	static int tris[4096*3];
	char buf[256];
	float tv[16];
	char **a;
	FILE *fd;
	int i, j, nv, nt;

	if(!stricmp(vfgetext(name), "ac"))
	{
		Mat4F_Copy(pos, tv);
		V4F_COPY(pos+0*4, tv+1*4);
		V4F_COPY(pos+1*4, tv+0*4);

		mesh_grp[num]=AC3D_LoadModelPos(name, tv);
		return(0);
	}

//	sprintf(buf, "resource/%s", name);
	fd=fopen(name, "rt");
	if(!fd)
	{
		printf("can't open mesh %s\n", name);
		return(-1);
	}

	printf("loading mesh %s\n", name);

	nv=0; nt=0;
	while(!feof(fd))
	{
		memset(buf, 0, 256);
		fgets(buf, 255, fd);
		a=ksplit(buf);

		if(!a[0])continue;
		if(!strncmp(a[0], "//", 2))continue;

		for(i=0; i<3; i++)
		{
			tv[i*3+0]=atof(a[i*3+2]);
			tv[i*3+1]=atof(a[i*3+0]);
			tv[i*3+2]=atof(a[i*3+1]);
		}

		for(i=0; i<3; i++)
		{
			for(j=0; j<nv; j++)
				if(V3F_DIST(tv+i*3, vecs+j*3)<0.001)
					break;
			if(j==nv)
			{
				V3F_COPY(tv+i*3, vecs+j*3);
				nv++;
			}

			tris[nt*3+i]=j;
		}
		nt++;
	}

	printf("mesh %d vecs, %d tris\n", nv, nt);

	mesh_vecs[num]=malloc(nv*3*sizeof(float));
	mesh_tris[num]=malloc(nt*3*sizeof(int));
	mesh_nvecs[num]=nv;
	mesh_ntris[num]=nt;

	for(i=0; i<nv; i++)
	{
		mesh_vecs[num][i*3+0]=vecs[i*3+0]*pos[0]+vecs[i*3+1]*pos[4]+
			vecs[i*3+2]*pos[8]+pos[12];
		mesh_vecs[num][i*3+1]=vecs[i*3+0]*pos[1]+vecs[i*3+1]*pos[5]+
			vecs[i*3+2]*pos[9]+pos[13];
		mesh_vecs[num][i*3+2]=vecs[i*3+0]*pos[2]+vecs[i*3+1]*pos[6]+
			vecs[i*3+2]*pos[10]+pos[14];
	}

	for(i=0; i<nt; i++)
	{
		mesh_tris[num][i*3+0]=tris[i*3+0];
		mesh_tris[num][i*3+1]=tris[i*3+1];
		mesh_tris[num][i*3+2]=tris[i*3+2];
	}

	fclose(fd);
}

int main_load_bones(char *name)
{
	char buf[256];
	char **a;
	FILE *fd;
	int i, j;
	float *v;

//	sprintf(buf, "resource/%s", name);
	fd=fopen(name, "rt");
	if(!fd)
	{
		printf("can't open bones %s\n", name);
		return(-1);
	}

	printf("loading bones %s\n", name);

	for(i=0; i<64; i++)
		main_bone_name[i]=NULL;

	main_bones=0;
	while(!feof(fd))
	{
		memset(buf, 0, 256);
		fgets(buf, 255, fd);
		a=ksplit(buf);

		if(!a[0])continue;
		if(!strncmp(a[0], "//", 2))continue;

		if(!strcmp(a[0], "bbox"))
		{
			bone_solid[bone_solids].type=BONE_SOLID_BBOX;
			bone_solid[bone_solids].bone=atoi(a[1]);

			v=main_bone_rorg+bone_solid[bone_solids].bone*3;
			bone_solid[bone_solids].m[0]=atof(a[3])-v[0];
			bone_solid[bone_solids].m[1]=atof(a[4])-v[1];
			bone_solid[bone_solids].m[2]=atof(a[5])-v[2];
			bone_solid[bone_solids].n[0]=atof(a[8])-v[0];
			bone_solid[bone_solids].n[1]=atof(a[9])-v[1];
			bone_solid[bone_solids].n[2]=atof(a[10])-v[2];

			bone_solid[bone_solids].r0=atof(a[12]);
			bone_solid[bone_solids].w=atof(a[13]);

			bone_solids++;
		}

		if(!strcmp(a[0], "capsule"))
		{
			bone_solid[bone_solids].type=BONE_SOLID_CAPSULE;
			bone_solid[bone_solids].bone=atoi(a[1]);

			v=main_bone_rorg+bone_solid[bone_solids].bone*3;
			bone_solid[bone_solids].m[0]=atof(a[3])-v[0];
			bone_solid[bone_solids].m[1]=atof(a[4])-v[1];
			bone_solid[bone_solids].m[2]=atof(a[5])-v[2];
			bone_solid[bone_solids].n[0]=atof(a[8])-v[0];
			bone_solid[bone_solids].n[1]=atof(a[9])-v[1];
			bone_solid[bone_solids].n[2]=atof(a[10])-v[2];

			bone_solid[bone_solids].r0=atof(a[12]);
			bone_solid[bone_solids].r1=atof(a[13]);
			bone_solid[bone_solids].w=atof(a[14]);

			bone_solids++;
		}

		if(!strcmp(a[0], "sphere"))
		{
			bone_solid[bone_solids].type=BONE_SOLID_SPHERE;
			bone_solid[bone_solids].bone=atoi(a[1]);

			v=main_bone_rorg+bone_solid[bone_solids].bone*3;
			bone_solid[bone_solids].m[0]=atof(a[3])-v[0];
			bone_solid[bone_solids].m[1]=atof(a[4])-v[1];
			bone_solid[bone_solids].m[2]=atof(a[5])-v[2];

			bone_solid[bone_solids].r0=atof(a[7]);
			bone_solid[bone_solids].r1=atof(a[8]);
			bone_solid[bone_solids].w=atof(a[9]);

			bone_solids++;
		}

		if((*a[0]<'0') || (*a[0]>'9'))
			continue;

		i=atoi(a[0]);
		if(i>=main_bones)main_bones=i+1;

		j=atoi(a[1]);
		main_bone_up[i]=j;
		main_bone_name[i]=strdup(a[2]);

		main_bone_rorg[i*3+0]=atof(a[4]);
		main_bone_rorg[i*3+1]=atof(a[5]);
		main_bone_rorg[i*3+2]=atof(a[6]);

		if(j>=0)
		{
			main_bone_org[i*3+0]=main_bone_rorg[i*3+0]-
				main_bone_rorg[j*3+0];
			main_bone_org[i*3+1]=main_bone_rorg[i*3+1]-
				main_bone_rorg[j*3+1];
			main_bone_org[i*3+2]=main_bone_rorg[i*3+2]-
				main_bone_rorg[j*3+2];
		}else
		{
			main_bone_org[i*3+0]=main_bone_rorg[i*3+0];
			main_bone_org[i*3+1]=main_bone_rorg[i*3+1];
			main_bone_org[i*3+2]=main_bone_rorg[i*3+2];
		}

		for(j=0; j<9; j++)main_bone_rot[i*9+j]=0;
		for(j=0; j<3; j++)main_bone_rot[i*9+j*3+j]=1;

		for(j=0; j<9; j++)main_bone_rmat[i*9+j]=0;
		for(j=0; j<3; j++)main_bone_rmat[i*9+j*3+j]=1;
	}
	fclose(fd);

	printf("skel loaded, %d bones, %d solids\n", main_bones, bone_solids);
}

int main_save_bones(char *name)
{
	char buf[256];
	char **a;
	FILE *fd;
	int i, j;

	fd=fopen(name, "wt");
	if(!fd)
	{
		printf("can't write bones %s\n", name);
		return(-1);
	}

	printf("saving bones %s\n", name);

	for(i=0; i<main_bones; i++)
	{
		if(!main_bone_name[i])continue;

		main_bone_rorg[i*3+0]=main_bone_org[i*3+0];
		main_bone_rorg[i*3+1]=main_bone_org[i*3+1];
		main_bone_rorg[i*3+2]=main_bone_org[i*3+2];

		if(main_bone_up[i]>=0)
		{
			j=main_bone_up[i];
			main_bone_rorg[i*3+0]+=main_bone_rorg[j*3+0];
			main_bone_rorg[i*3+1]+=main_bone_rorg[j*3+1];
			main_bone_rorg[i*3+2]+=main_bone_rorg[j*3+2];
		}

		fprintf(fd, "%d %d \"%s\" [ %g %g %g ]\n", i,
			main_bone_up[i], main_bone_name[i],
			main_bone_rorg[i*3+0], main_bone_rorg[i*3+1],
			main_bone_rorg[i*3+2]);
	}
	fclose(fd);
}

int main_load_model(char *name)
{
	char buf[256];
	float pos[16];
	char **a;
	char *base, *s;
	FILE *fd;
	int i, j;

	strcpy(buf, name);
	s=buf+strlen(buf);
	while((s>buf) && (*s!='/') && (*s!='\\'))s--;
	*s=0;
	base=strdup(buf);

//	sprintf(buf, "resource/%s", name);
	fd=fopen(name, "rt");
	if(!fd)
	{
		printf("can't open models %s\n", name);
		return(-1);
	}

	printf("loading models %s\n", name);

	while(!feof(fd))
	{
		memset(buf, 0, 256);
		fgets(buf, 255, fd);
		a=ksplit(buf);

		if(!a[0])continue;
		if(!strncmp(a[0], "//", 2))continue;

		if(!strcmp(a[0], "bones"))
		{
			sprintf(buf, "%s/%s", base, a[1]);
			main_load_bones(buf);
//			main_load_bones(a[1]);
		}

		if(!strcmp(a[0], "mesh"))
		{
			for(i=0; i<main_bones; i++)
				if(!strcmp(main_bone_name[i], a[1]))
					break;
			if(i>=main_bones)continue;

			for(j=0; j<16; j++)pos[j]=0;
			for(j=0; j<4; j++)pos[j*4+j]=1;

			for(j=0; j<12; j++)pos[((j/3)*4)+(j%3)]=atof(a[4+j]);

			sprintf(buf, "%s/%s", base, a[2]);
			main_load_mesh(i, buf, pos);
//			main_load_mesh(i, a[2], pos);
		}
	}
	fclose(fd);
}

int main_load_anim(char *name)
{
	char buf[256];
	float pos[16];
	char **a;
	FILE *fd;
	int i, j, k;

//	sprintf(buf, "resource/%s", name);
	fd=fopen(name, "rt");
	if(!fd)
	{
		printf("can't open anim %s\n", name);
		return(-1);
	}

	printf("loading anim %s\n", name);

	memset(frame_mask, 0, 256*64/8);
	n_frames=0;

	while(!feof(fd))
	{
		memset(buf, 0, 256);
		fgets(buf, 255, fd);
		a=ksplit(buf);

		if(!a[0])continue;
		if(!strncmp(a[0], "//", 2))continue;

		if(!strcmp(a[0], "frame"))
		{
			frame_cur=atoi(a[1]);
			if((frame_cur+1)>n_frames)n_frames=frame_cur+1;
		}

		for(i=0; i<main_bones; i++)
			if(main_bone_name[i] &&
				!strcmp(main_bone_name[i], a[0]))
					break;
		if(i>=main_bones)continue;

		j=frame_cur*64+i;
		for(k=0; k<3; k++)frame_org[j*3+k]=atof(a[2+k]);
		for(k=0; k<9; k++)frame_rot[j*9+k]=atof(a[7+k]);
		frame_mask[j>>3]|=1<<(j&7);
	}
	fclose(fd);

//	sprintf(buf, "%s.bak", name);
//	main_save_anim(buf);
}

int main_save_anim(char *name)
{
	char buf[256];
	float pos[16];
	char **a;
	FILE *fd;
	float f;
	int i, j, k, l;

//	sprintf(buf, "resource/%s", name);
	fd=fopen(name, "wt");
	if(!fd)
	{
		printf("can't write anim %s\n", name);
		return(-1);
	}

	printf("saving anim %s\n", name);

	for(i=0; i<n_frames; i++)
	{
		l=-1;
		fprintf(fd, "frame %d\n", i);
		for(j=0; j<main_bones; j++)
		{
			if(!main_bone_name[j])continue;

			k=i*64+j;
			if(frame_mask[k>>3]&(1<<(k&7)))
			{
				fprintf(fd, "%s [ ", main_bone_name[j]);
				for(l=0; l<3; l++)
				{
					f=frame_org[k*3+l];
					if(fabs(f)<0.0001)f=0;
					fprintf(fd, "%g ", f);
				}
				fprintf(fd, "] [ ");
				for(l=0; l<9; l++)
				{
					f=frame_rot[k*9+l];
					if(fabs(f)<0.0001)f=0;
					fprintf(fd, "%g ", f);
				}
				fprintf(fd, "]\n");
			}
		}
		if(l>=0)fprintf(fd, "\n");
	}
	fclose(fd);
}

void main_lerp_rot(int num, float *rot)
{
	float f;
	int i, j, k, t0, t1;

	//no frames, use base rot
	if(!n_frames)
	{
		for(i=0; i<9; i++)rot[i]=main_bone_rot[num*9+i];
		return;
	}

	//keyframe
	i=frame_cur*64+num;
	if(frame_mask[i>>3]&(1<<(i&7)))
	{
		for(j=0; j<9; j++)rot[j]=frame_rot[i*9+j];
		return;
	}

	i=frame_cur;
	while(1)
	{
		j=i*64+num;
		if(frame_mask[j>>3]&(1<<(j&7)))
			break;
		i--;
		if(i<0)i+=n_frames;
		if(i==frame_cur)break;
	}
	t0=i;

	i=frame_cur;
	while(1)
	{
		j=i*64+num;
		if(frame_mask[j>>3]&(1<<(j&7)))
			break;
		i++;
		if(i>=n_frames)i-=n_frames;
		if(i==frame_cur)break;
	}
	t1=i;

	//no keyframes, use base rot
	if((t0==frame_cur) || (t1==frame_cur))
	{
		for(i=0; i<9; i++)rot[i]=main_bone_rot[num*9+i];
		return;
	}

	//same keyframe, no need to lerp
	if(t0==t1)
	{
		i=t0*64+num;
		for(j=0; j<9; j++)rot[j]=frame_rot[i*9+j];
		return;
	}


	i=frame_cur; j=t1;
	if(i<t0)i+=n_frames;
	if(j<t0)j+=n_frames;
	f=((float)(i-t0))/(j-t0);

	i=t0*64+num;
	j=t1*64+num;
//	for(k=0; k<9; k++)rot[k]=
//		frame_rot[i*9+k]*(1-f) + frame_rot[j*9+k]*f;
//	Mat3F_Filter3Matrix(rot, rot);

	Mat3F_Interpolate(frame_rot+i*9, frame_rot+j*9, rot, f);
	Mat3F_Filter3Matrix(rot, rot);
}

void main_lerp_org(int num, float *org)
{
	float f;
	int i, j, k, t0, t1;

	//no frames, use base rot
	if(!n_frames)
	{
		org[0]=main_bone_org[num*3+0];
		org[1]=main_bone_org[num*3+1];
		org[2]=main_bone_org[num*3+2];
		return;
	}

	//keyframe
	i=frame_cur*64+num;
	if(frame_mask[i>>3]&(1<<(i&7)))
	{
		org[0]=frame_org[i*3+0];
		org[1]=frame_org[i*3+1];
		org[2]=frame_org[i*3+2];
		return;
	}

	i=frame_cur;
	while(1)
	{
		j=i*64+num;
		if(frame_mask[j>>3]&(1<<(j&7)))
			break;
		i--;
		if(i<0)i+=n_frames;
		if(i==frame_cur)break;
	}
	t0=i;

	i=frame_cur;
	while(1)
	{
		j=i*64+num;
		if(frame_mask[j>>3]&(1<<(j&7)))
			break;
		i++;
		if(i>=n_frames)i-=n_frames;
		if(i==frame_cur)break;
	}
	t1=i;

	//no keyframes, use base rot
	if((t0==frame_cur) || (t1==frame_cur))
	{
		org[0]=main_bone_org[num*3+0];
		org[1]=main_bone_org[num*3+1];
		org[2]=main_bone_org[num*3+2];
		return;
	}

	//same keyframe, no need to lerp
	if(t0==t1)
	{
		i=t0*64+num;
		org[0]=frame_org[i*3+0];
		org[1]=frame_org[i*3+1];
		org[2]=frame_org[i*3+2];
		return;
	}


	i=frame_cur; j=t1;
	if(i<t0)i+=n_frames;
	if(j<t0)j+=n_frames;
	f=((float)(i-t0))/(j-t0);

	i=t0*64+num;
	j=t1*64+num;
	org[0]=frame_org[i*3+0]*(1-f) + frame_org[j*3+0]*f;
	org[1]=frame_org[i*3+1]*(1-f) + frame_org[j*3+1]*f;
	org[2]=frame_org[i*3+2]*(1-f) + frame_org[j*3+2]*f;
}

void main_draw_star(float *org)
{
	float v[3];
	int i;

	for(i=1; i<27; i++)
	{
		v[0]=((i/1)%3)?((((i/1)%3)==1)?1:-1):0;
		v[1]=((i/3)%3)?((((i/3)%3)==1)?1:-1):0;
		v[2]=((i/9)%3)?((((i/9)%3)==1)?1:-1):0;
		V3F_NORMALIZE(v, v);

		glVertex3f(org[0]-v[0]*0.5, org[1]-v[1]*0.5, org[2]-v[2]*0.5);
		glVertex3f(org[0]+v[0]*0.5, org[1]+v[1]*0.5, org[2]+v[2]*0.5);
	}
}


void main_draw_mesh(int num, float *org, float *rot)
{
	float im[16];
	float tv[3], tv1[3], f;
	int i, j, k;

	if(mesh_grp[num])
	{
		Mat4F_Identity(im);
		im[0]=rot[0];  im[1]=rot[1];  im[2]=rot[2];
		im[4]=rot[3];  im[5]=rot[4];  im[6]=rot[5];
		im[8]=rot[6];  im[9]=rot[7];  im[10]=rot[8];
		im[12]=org[0]; im[13]=org[1]; im[14]=org[2];

		f=V3F_DIST(org, main_cam_org)/12.0;
		if(f<1)f=1;

		glColor4f(1.0/f, 1.0/f, 1.0/f, 1);

		if((main_mode==MODE_MESH) && (num==main_bone_sel))
			glColor4f(0.5/f, 1.0/f, 0.5/f, 1);

		AC3D_DrawMeshGroup(mesh_grp[num], im);

		return;
	}

	if(!mesh_vecs[num])return;
//	return;

	glBegin(GL_TRIANGLES);
	for(i=0; i<mesh_ntris[num]; i++)
		for(j=0; j<3; j++)
	{
		k=mesh_tris[num][i*3+j];
		V3F_COPY(mesh_vecs[num]+k*3, tv);
//		V3F_ADD(tv, org, tv);

		tv1[0]=tv[0]*rot[0]+tv[1]*rot[3]+tv[2]*rot[6]+org[0];
		tv1[1]=tv[0]*rot[1]+tv[1]*rot[4]+tv[2]*rot[7]+org[1];
		tv1[2]=tv[0]*rot[2]+tv[1]*rot[5]+tv[2]*rot[8]+org[2];

		f=V3F_DIST(tv1, main_cam_org)/12.0;
		glColor4f(1.0/f, 1.0/f, 1.0/f, 1);

		glVertex3fv(tv1);
	}
	glEnd();
}

char *Path_GenAbs(char *name)
{
	char buf[256], buf1[256];
	char *s;

	s=name;
	if(*s=='/')return(name);
	if(*s=='\\')return(name);

	while(*s && (*s!='/') && (*s!='\\'))s++;
	if(*s==':')return(name);

	getcwd(buf, 255);
	sprintf(buf1, "%s/%s", buf, name);
	name=strdup(buf1);

	return(name);
}

int ConCmds_LoadBones(char **a)
{
	bones_file=Path_GenAbs(a[1]);
	main_load_bones(bones_file);
	return(0);
}

int ConCmds_SaveBones(char **a)
{
	bones_file=Path_GenAbs(a[1]);
	main_save_bones(bones_file);
	return(0);
}

int ConCmds_LoadModel(char **a)
{
	model_file=Path_GenAbs(a[1]);
	main_load_model(model_file);
	return(0);
}

int ConCmds_LoadAnim(char **a)
{
	frame_file=Path_GenAbs(a[1]);
	main_load_anim(frame_file);
	return(0);
}

int ConCmds_SaveAnim(char **a)
{
	frame_file=Path_GenAbs(a[1]);
	main_save_anim(frame_file);
	return(0);
}

int ConCmds_RenameBone(char **a)
{
	int i;

	for(i=0; i<main_bones; i++)
		if(main_bone_name[i] && !strcmp(main_bone_name[i], a[1]))
			break;
	if(i>=main_bones)
	{
		Con_Printf("Unknown Bone '%s'\n", a[1]);
		return(0);
	}

	main_bone_name[i]=strdup(a[2]);

	return(0);
}

int pdgl_main_startup(int argc, char *argv[])
{
	float v[3];
	char *opts[4];
	int i, j, k, l;
	char *s;

	GfxFont_Init();
	Con_Init();
	ConCmds_Init();

	PDGLUI_Init();

	ConCmds_Register("load-bones", "Load bones", ConCmds_LoadBones);
	ConCmds_Register("save-bones", "Save bones", ConCmds_SaveBones);
	ConCmds_Register("load-model", "Load a model", ConCmds_LoadModel);
	ConCmds_Register("load-anim", "Load an animation", ConCmds_LoadAnim);
	ConCmds_Register("save-anim", "Save an animation", ConCmds_SaveAnim);

	ConCmds_Register("rename-bone", "Rename a Bone", ConCmds_RenameBone);

	UI_LoadSave_CheckDrives();
	UI_LoadSave_SetTitle("Test", NULL);

	main_cam_org[0]=0;
	main_cam_org[1]=-10;
	main_cam_org[2]=5;

	main_cam_ang[0]=90;
	main_cam_ang[1]=0;
	main_cam_ang[2]=0;


	bones_file=NULL;
	model_file=NULL;
	frame_file=NULL;

//	if(argc>1)model_file=Path_GenAbs(argv[1]);
//	if(argc>2)frame_file=Path_GenAbs(argv[2]);

//	model_file=argv[1];
//	frame_file=argv[2];

//	main_load_bones(argv[1]);
//	if(model_file)main_load_model(model_file);
//	if(frame_file)main_load_anim(frame_file);

	for(i=1; i<argc; i++)
	{
		if(argv[i][0]=='-')continue;

		s=Path_GenAbs(argv[i]);
		if(!stricmp(vfgetext(s), "bone"))
		{
			bones_file=s;
			main_load_bones(s);
		}
		if(!stricmp(vfgetext(s), "anim"))
		{
			frame_file=s;
			main_load_anim(s);
		}
		if(!stricmp(vfgetext(s), "model"))
		{
			model_file=s;
			main_load_model(s);
		}

		if(!stricmp(vfgetext(s), "ac"))
		{
			main_load_rootmesh(s);
		}
	}


	PDGLUI_BeginMenu("menu");

	PDGLUI_BeginSubMenu("Clear");
	PDGLUI_MenuItem("clear_all", "Clear All");
	PDGLUI_MenuItem("clear_bones", "Clear Bones");
	PDGLUI_MenuItem("clear_anim", "Clear Anim");
	PDGLUI_MenuItem("clear_mesh", "Clear Meshes");
	PDGLUI_EndSubMenu();

	PDGLUI_BeginSubMenu("Load");
	PDGLUI_MenuItem("load_bones", "Load &Bones");
	PDGLUI_MenuItem("load_anim", "Load &Anim");
	PDGLUI_MenuItem("load_model", "Load &Model");
	PDGLUI_MenuItem("load_ac3d", "Load &AC3D");
	PDGLUI_EndSubMenu();

	PDGLUI_BeginSubMenu("Save");
	PDGLUI_MenuItem("save_bones", "Save &Bones");
	PDGLUI_MenuItem("save_anim", "Save &Anim");
//	PDGLUI_MenuItem("save_model", "Save &Model");
//	PDGLUI_MenuItem("save_ac3d", "Save &AC3D");
	PDGLUI_EndSubMenu();

	PDGLUI_BeginSubMenu("Command");
	PDGLUI_MenuItem("cmd_rebind", "Rebind Meshes");
	PDGLUI_MenuItem("cmd_unbind", "Unbind Meshes");
	PDGLUI_EndSubMenu();

	PDGLUI_BeginSubMenu("Help");
	PDGLUI_MenuItem("help_generic", "General");
	PDGLUI_MenuItem("help_about", "About");
	PDGLUI_EndSubMenu();

#if 0
	PDGLUI_MenuItem("foo", "&Foo");
	PDGLUI_MenuItem("bar", "&Bar");
	PDGLUI_MenuItem("baz", "&Baz");

	PDGLUI_MenuItem(NULL, NULL);

	PDGLUI_BeginSubMenu("&FooBar");
	PDGLUI_MenuItem("foo", "&FooFoo");
	PDGLUI_MenuItem("bar", "&FooBar");
	PDGLUI_MenuItem("baz", "&FooBaz");
	PDGLUI_EndSubMenu();

	PDGLUI_BeginSubMenu("&FooBaz");
	PDGLUI_MenuItem("foo", "&BarFoo");
	PDGLUI_MenuItem("bar", "&BarBar");
	PDGLUI_MenuItem("baz", "&BarBaz");
	PDGLUI_EndSubMenu();
#endif

	PDGLUI_EndMenu();


	main_bone_sel=-1;
	main_mode=MODE_BONES;

	return(1);
}

void main_set_keyframe()
{
	float pt[3];
	int i, j, k, l;

	if(main_mode!=MODE_ANIMATE)return;

	i=main_bone_sel;
	j=main_bone_up[i];
	k=frame_cur*64+i;
	if(!(frame_mask[k>>3]&(1<<(k&7))))
	{
		frame_mask[k>>3]|=(1<<(k&7));
		if(j<0)
		{
			V3F_COPY(main_bone_rorg+i*3, frame_org+k*3);
			for(l=0; l<9; l++)frame_rot[k*9+l]=
				main_bone_rmat[i*9+l];
		}else
		{
			Mat3F_Mat3MultT(main_bone_rmat+i*9,
				main_bone_rmat+j*9, frame_rot+k*9);
			V3F_COPY(main_bone_rorg+i*3, pt);
			V3F_SUB(pt, main_bone_rorg+j*3, pt);
			frame_org[k*3+0]=V3F_DOT(pt, main_bone_rmat+j*9+0);
			frame_org[k*3+1]=V3F_DOT(pt, main_bone_rmat+j*9+3);
			frame_org[k*3+2]=V3F_DOT(pt, main_bone_rmat+j*9+6);
		}
	}

}

void main_delete_bone(int num)
{
	int i, j;

	main_bone_name[num]=NULL;
	for(i=0; i<n_frames; i++)
	{
		j=i*64+num;
		frame_mask[j>>3]&=~(1<<(j&7));
	}

	for(i=0; i<main_bones; i++)
	{
		if(!main_bone_name[i])continue;
		if(main_bone_up[i]==num)
			main_delete_bone(i);
	}
}


void main_show_help(char *mode)
{
	static char *helpstr_generic=
"*Controls*\n"
"\n"
"General:\n"
"	Left//Right Arrows, Move left//right\n"
"	Up//Down Arrows, Move forwards//backwards\n"
"	Shift+Up//Down Arrows, Move up//down\n"
"	Shift+Home, Rotate to nearest 45 degrees\n"
"	CMB+Drag, Rotate Camera\n"
"	Tab, Change mode\n"
"Bones:\n"
"	LMB+Drag, Rotate object\n"
"	Shift+LMB+Drag, Translate object\n"
"	Delete(Anim Mode), Delete keyframe\n"
"	Shift+Delete(Anim Mode), Delete all bone keyframes\n"
"	Delete(Bone Mode), Delete bone and sub bones\n"
"	Insert(Bone Mode), Create a new bone attached to the current bone\n"
"Anim:\n"
"	+, Next Frame\n"
"	--, Prev Frame\n"
"	**, New Frame\n"
"	//, Delete Frame\n"
"\n\n";

	static char *helpstr_about=
"BGB Skeletal Animating Tool\n"
"2005--2006 By Brendan G Bohannon\n"
"";

	char *helpstr;
	char *s;

	helpstr=helpstr_generic;
	if(mode && !strcmp(mode, "about"))helpstr=helpstr_about;

	s=PDGLUI_BeginForm("window", "help");
	if(!s)return;

	PDGLUI_FormPropertyS("title", "Help");

	PDGLUI_BeginWidget("vbox", NULL);

	PDGLUI_BeginWidget("text", NULL);
	PDGLUI_WidgetPropertyS("value", helpstr);
	PDGLUI_EndWidget();

	PDGLUI_BeginWidget("button", "btn_ok");
	PDGLUI_WidgetPropertyS("label", "   &OK   ");
	PDGLUI_EndWidget();

	PDGLUI_EndWidget();

	PDGLUI_EndForm();
}

void main_handle_form(char *frm)
{
	char *s0, *s1, *s2;
	int i;

	while(PDGLUI_HasEvent(frm))
	{
		s0=PDGLUI_GetEventWidget(frm);
		s1=PDGLUI_GetEventName(frm);
		i=PDGLUI_GetEventIVal(frm);
		s2=PDGLUI_GetEventSVal(frm);
		PDGLUI_NextEvent(frm);

		if(!strcmp(frm, "help") && s0 && !strcmp(s0, "btn_ok"))
		{
			PDGLUI_Forms_Unlink(frm);
		}
	}
}

void main_handle_events()
{
	main_handle_form("help");
}


void main_handle_input()
{
	char buf[64];
	float io[3], iv[4], fw[2], pt[4], tv[3], f, g, v;
	float im[16], im1[16], *imv;
	char *s;
	int i, j, k, l;
	unsigned short *kcur;

	if(Con_IsDown() || main_loadsave)
		return;

	if(pdgl_mb&4)
		PDGLUI_ShowMenu("menu", pdgl_mx, -pdgl_my);

	s=PDGLUI_GetMenuSelection("menu");
	if(s)
	{
		if(!strcmp(s, "clear_all"))
		{
			main_bones=0;
			bone_solids=0;
			n_frames=0;
			mesh_root=NULL;
		}
		if(!strcmp(s, "clear_bones"))
		{
			main_bones=0;
			bone_solids=0;
		}
		if(!strcmp(s, "clear_anim"))
			n_frames=0;
		if(!strcmp(s, "clear_mesh"))
			mesh_root=NULL;


		if(!strcmp(s, "load_bones"))
		{
			UI_LoadSave_SetTitle("Load Bones", "bone");
			main_loadsave=1;
		}
		if(!strcmp(s, "save_bones"))
		{
			UI_LoadSave_SetTitle("Save Bones", "bone");
			main_loadsave=2;
		}
		if(!strcmp(s, "load_anim"))
		{
			UI_LoadSave_SetTitle("Load Anim", "anim");
			main_loadsave=3;
		}
		if(!strcmp(s, "save_anim"))
		{
			UI_LoadSave_SetTitle("Save Anim", "anim");
			main_loadsave=4;
		}
		if(!strcmp(s, "load_model"))
		{
			UI_LoadSave_SetTitle("Load Model", "model");
			main_loadsave=5;
		}
		if(!strcmp(s, "save_model"))
		{
			UI_LoadSave_SetTitle("Save Model", "model");
			main_loadsave=6;
		}

		if(!strcmp(s, "load_mesh"))
		{
			UI_LoadSave_SetTitle("Load Mesh", "mesh");
			main_loadsave=7;
		}
		if(!strcmp(s, "save_mesh"))
		{
			UI_LoadSave_SetTitle("Save Mesh", "mesh");
			main_loadsave=8;
		}

		if(!strcmp(s, "load_ac3d"))
		{
			UI_LoadSave_SetTitle("Import AC3D", "ac");
			main_loadsave=9;
		}

		if(!strcmp(s, "cmd_rebind"))
			main_BindMeshGroup(mesh_root);
		if(!strcmp(s, "cmd_unbind"))
			main_UnbindMeshGroup(mesh_root);

		if(!strcmp(s, "help_generic"))
			main_show_help("general");
		if(!strcmp(s, "help_about"))
			main_show_help("about");
	}


	kcur=pdgl_keybuf;
	while(*kcur)
	{
		if(*kcur==K_ESCAPE)
		{
			main_kill=1;
			return;
		}

		if(*kcur==K_MWHEELUP)
		{
			main_speed*=1.5;
			Con_Printf("Move Speed %g ft/s\n", main_speed/12);
		}
		if(*kcur==K_MWHEELDOWN)
		{
			main_speed*=1.0/1.5;
			Con_Printf("Move Speed %g ft/s\n", main_speed/12);
		}

		if(*kcur==K_F1)
		{
			main_show_help("general");
		}

		if(*kcur==K_TAB)
		{
			main_mode++;
			if(main_mode>MODE_LAST)main_mode=MODE_FIRST;
		}

		if(*kcur=='S')
		{
			main_save_anim(frame_file);
		}


		if((*kcur==K_DEL) && (main_bone_sel>=0) &&
			(main_mode==MODE_ANIMATE))
		{
			if(PDGL_KeyDown(K_SHIFT))
			{
				for(i=0; i<n_frames; i++)
				{
					j=i*64+main_bone_sel;
					frame_mask[j>>3]&=~(1<<(j&7));
				}
			}else
			{
				i=frame_cur*64+main_bone_sel;
				frame_mask[i>>3]&=~(1<<(i&7));
			}
		}

		if((*kcur==K_DEL) && (main_bone_sel>=0) &&
			(main_mode==MODE_BONES))
		{
			main_delete_bone(main_bone_sel);
		}

		if((*kcur==K_DEL) && (main_bone_sel>=0) &&
			(main_mode==MODE_MESH))
		{
			mesh_grp[main_bone_sel]=NULL;
		}

		if((*kcur==K_INS) && (main_mode==MODE_BONES))
		{
			for(i=0; i<main_bones; i++)
				if(!main_bone_name[i])
					break;
			if(i>=main_bones)main_bones=i+1;

			sprintf(buf, "auto-%d", i);
			main_bone_name[i]=strdup(buf);
			main_bone_up[i]=main_bone_sel;
			Mat3F_Identity(main_bone_rot+i*9);
			Mat3F_Identity(main_bone_rmat+i*9);

			j=main_bone_sel;
			V3F_ZERO(tv);
			if(j>=0) { V3F_COPY(main_bone_rorg+j*3, tv); }

			pt[0]=V3F_DOT(tv, main_cam_rt)-
				V3F_DOT(main_cam_org, main_cam_rt);
			pt[1]=V3F_DOT(tv, main_cam_up)-
				V3F_DOT(main_cam_org, main_cam_up);
			pt[2]=V3F_DOT(tv, main_cam_fw)-
				V3F_DOT(main_cam_org, main_cam_fw);

			V3F_ZERO(io);
			V3F_ADDSCALE(io, main_cam_rt,
				(pdgl_mx/400.0)*pt[2]-pt[0], io);
			V3F_ADDSCALE(io, main_cam_up,
				(-pdgl_my/400.0)*pt[2]-pt[1], io);

			main_bone_org[i*3+0]=io[0];
			main_bone_org[i*3+1]=io[1];
			main_bone_org[i*3+2]=io[2];

			main_bone_rorg[i*3+0]=io[0]+tv[0];
			main_bone_rorg[i*3+1]=io[1]+tv[1];
			main_bone_rorg[i*3+2]=io[2]+tv[2];

			return;
		}
/*
		if(*kcur=='L')
		{
			UI_LoadSave_SetTitle("Load", "txt");
			main_loadsave=1;
		}
*/


		if(((*kcur=='+') || (*kcur=='=')) && (frame_time>=(1.0/15.0)))
		{
			main_mode=MODE_ANIMATE;

			frame_time-=1.0/15.0;
			frame_cur++;
			if(n_frames)frame_cur%=n_frames;
				else frame_cur=0;
		}
		if(((*kcur=='-') || (*kcur=='_')) && (frame_time>=(1.0/15.0)))
		{
			main_mode=MODE_ANIMATE;

			frame_time-=1.0/15.0;
			frame_cur--;
			if(frame_cur<0)frame_cur+=n_frames;
			if(n_frames)frame_cur%=n_frames;
				else frame_cur=0;
		}
		if(*kcur=='*')
		{
			main_mode=MODE_ANIMATE;

			for(i=n_frames; i>frame_cur; i--)
			{
				for(j=0; j<64*3; j++)
					frame_org[i*64*3+j]=
						frame_org[(i-1)*64*3+j];
				for(j=0; j<64*9; j++)
					frame_rot[i*64*9+j]=
						frame_rot[(i-1)*64*9+j];
				for(j=0; j<64/8; j++)
					frame_mask[i*64/8+j]=
						frame_mask[(i-1)*64/8+j];
			}
			if(n_frames)frame_cur++;
			for(j=0; j<64/8; j++)frame_mask[i*64/8+j]=0;
			n_frames++;
		}

		if((n_frames>0) && (*kcur=='/'))
		{
			main_mode=MODE_ANIMATE;

			for(i=frame_cur; i<n_frames; i++)
			{
				for(j=0; j<64*3; j++)
					frame_org[i*64*3+j]=
						frame_org[(i+1)*64*3+j];
				for(j=0; j<64*9; j++)
					frame_rot[i*64*9+j]=
						frame_rot[(i+1)*64*9+j];
				for(j=0; j<64/8; j++)
					frame_mask[i*64/8+j]=
						frame_mask[(i+1)*64/8+j];
			}
			if(frame_cur>0)frame_cur--;
			n_frames--;
		}

		kcur++;
	}

#if 0
	V3F_ZERO(iv);
	if(PDGL_KeyDown(K_SHIFT))
	{
		f=main_cam_ang[2]*(M_PI/180);
		if(PDGL_KeyDown(K_LEFTARROW))
		{
//			iv[0]-=pdgl_dt_f*15*cos(f);
//			iv[1]-=pdgl_dt_f*15*sin(f);

			V3F_ADDSCALE(iv, main_cam_rt, -pdgl_dt_f*15, iv);
		}
		if(PDGL_KeyDown(K_RIGHTARROW))
		{
//			iv[0]+=pdgl_dt_f*15*cos(f);
//			iv[1]+=pdgl_dt_f*15*sin(f);

			V3F_ADDSCALE(iv, main_cam_rt, pdgl_dt_f*15, iv);
		}

		if(PDGL_KeyDown(K_UPARROW))
			{ V3F_ADDSCALE(iv, main_cam_up, pdgl_dt_f*15, iv); }
		if(PDGL_KeyDown(K_DOWNARROW))
			{ V3F_ADDSCALE(iv, main_cam_up, -pdgl_dt_f*15, iv); }

		if(PDGL_KeyDown(K_PGUP))
			{ V3F_ADDSCALE(iv, main_cam_fw, pdgl_dt_f*15, iv); }
		if(PDGL_KeyDown(K_PGDN))
			{ V3F_ADDSCALE(iv, main_cam_fw, -pdgl_dt_f*15, iv); }

//		if(PDGL_KeyDown(K_UPARROW))iv[2]+=pdgl_dt_f*15;
//		if(PDGL_KeyDown(K_DOWNARROW))iv[2]-=pdgl_dt_f*15;

		if(PDGL_KeyDown(K_HOME))
		{
			i=(int)((main_cam_ang[0]+22.5)/45);
			main_cam_ang[0]=(i%8)*45;

			i=(int)((main_cam_ang[1]+22.5)/45);
			main_cam_ang[1]=(i%8)*45;

			i=(int)((main_cam_ang[2]+22.5)/45);
			main_cam_ang[2]=(i%8)*45;
		}
	}else
	{
		if(PDGL_KeyDown(K_LEFTARROW))main_cam_ang[2]+=90*pdgl_dt_f;
		if(PDGL_KeyDown(K_RIGHTARROW))main_cam_ang[2]-=90*pdgl_dt_f;
		if(PDGL_KeyDown(K_UPARROW))main_cam_ang[0]+=90*pdgl_dt_f;
		if(PDGL_KeyDown(K_DOWNARROW))main_cam_ang[0]-=90*pdgl_dt_f;

		if(main_cam_ang[0]<0)main_cam_ang[0]+=360;
		if(main_cam_ang[1]<0)main_cam_ang[1]+=360;
		if(main_cam_ang[2]<0)main_cam_ang[2]+=360;
		if(main_cam_ang[0]>=360)main_cam_ang[0]-=360;
		if(main_cam_ang[1]>=360)main_cam_ang[1]-=360;
		if(main_cam_ang[2]>=360)main_cam_ang[2]-=360;

		f=main_cam_ang[2]*(M_PI/180);
		if(PDGL_KeyDown(K_HOME))
		{
			iv[0]-=pdgl_dt_f*15*sin(f);
			iv[1]+=pdgl_dt_f*15*cos(f);
		}
		if(PDGL_KeyDown(K_END))
		{
			iv[0]+=pdgl_dt_f*15*sin(f);
			iv[1]-=pdgl_dt_f*15*cos(f);
		}

		if(PDGL_KeyDown(K_PGUP))iv[2]+=pdgl_dt_f*15;
		if(PDGL_KeyDown(K_PGDN))iv[2]-=pdgl_dt_f*15;
	}
#endif

#if 1
	V3F_ZERO(iv);
	if(PDGL_KeyDown(K_SHIFT) || PDGL_KeyDown(K_CTRL))
	{
		if(PDGL_KeyDown(K_LEFTARROW))
		{
			V3F_ADDSCALE(iv, main_cam_rt,
				-pdgl_dt_f*main_speed, iv);
		}
		if(PDGL_KeyDown(K_RIGHTARROW))
		{
			V3F_ADDSCALE(iv, main_cam_rt,
				pdgl_dt_f*main_speed, iv);
		}

		if(PDGL_KeyDown(K_UPARROW))
		{
			V3F_ADDSCALE(iv, main_cam_up,
				pdgl_dt_f*main_speed, iv);
		}
		if(PDGL_KeyDown(K_DOWNARROW))
		{
			V3F_ADDSCALE(iv, main_cam_up,
				-pdgl_dt_f*main_speed, iv);
		}

		if(PDGL_KeyDown(K_HOME))
		{
			i=(int)((main_cam_ang[0]+22.5)/45);
			main_cam_ang[0]=(i%8)*45;

			i=(int)((main_cam_ang[1]+22.5)/45);
			main_cam_ang[1]=(i%8)*45;

			i=(int)((main_cam_ang[2]+22.5)/45);
			main_cam_ang[2]=(i%8)*45;
		}
	}else
	{
		if(PDGL_KeyDown(K_LEFTARROW))
		{
			V3F_ADDSCALE(iv, main_cam_rt,
				-pdgl_dt_f*main_speed, iv);
		}
		if(PDGL_KeyDown(K_RIGHTARROW))
		{
			V3F_ADDSCALE(iv, main_cam_rt,
				pdgl_dt_f*main_speed, iv);
		}

		if(PDGL_KeyDown(K_UPARROW))
		{
			V3F_ADDSCALE(iv, main_cam_fw,
				pdgl_dt_f*main_speed, iv);
		}
		if(PDGL_KeyDown(K_DOWNARROW))
		{
			V3F_ADDSCALE(iv, main_cam_fw,
				-pdgl_dt_f*main_speed, iv);
		}
	}

	if(pdgl_mb&2)
	{
		if(pdgl_lmb&2)
		{
			main_cam_ang[0]-=pdgl_my;
			main_cam_ang[2]-=pdgl_mx;

			if(main_cam_ang[0]<0)main_cam_ang[0]=0;
			if(main_cam_ang[0]>180)main_cam_ang[0]=180;
		}

		Mouse_SetPos(0, 0);
	}
#endif

	V3F_ADD(main_cam_org, iv, main_cam_org);
	main_cam_ang[0]=floor(main_cam_ang[0]+0.5);
	main_cam_ang[1]=floor(main_cam_ang[1]+0.5);
	main_cam_ang[2]=floor(main_cam_ang[2]+0.5);

	V3F_ZERO(iv);

	f=main_cam_ang[2]*(M_PI/180);
	if(PDGL_KeyDown('e'))
	{
		iv[0]-=pdgl_dt_f*1*sin(f);
		iv[1]+=pdgl_dt_f*1*cos(f);
	}
	if(PDGL_KeyDown('q'))
	{
		iv[0]+=pdgl_dt_f*1*sin(f);
		iv[1]-=pdgl_dt_f*1*cos(f);
	}

	if(PDGL_KeyDown('a'))
	{
		iv[0]-=pdgl_dt_f*1*cos(f);
		iv[1]-=pdgl_dt_f*1*sin(f);
	}
	if(PDGL_KeyDown('d'))
	{
		iv[0]+=pdgl_dt_f*1*cos(f);
		iv[1]+=pdgl_dt_f*1*sin(f);
	}

	if(PDGL_KeyDown('w'))iv[2]+=pdgl_dt_f*1;
	if(PDGL_KeyDown('s'))iv[2]-=pdgl_dt_f*1;


	if(PDGL_KeyDown(K_ENTER))
		main_bone_sel=-1;

	for(i=0; i<main_bones; i++)
	{
		V3F_COPY(main_bone_rorg+i*3, io);

		f=V3F_DOT(io, main_mdir)-V3F_DOT(main_morg, main_mdir);
		V3F_ADDSCALE(main_morg, main_mdir, f, pt);
		f=V3F_DIST(pt, io);
		if(f<0.5)
		{
			if(PDGL_KeyDown(K_ENTER))
				main_bone_sel=i;
		}
	}

	if((pdgl_mb&1) && (pdgl_mx>(400-16*8)))
	{
		i=(pdgl_my+(300-24))/8;
		main_bone_sel=i;

		if(main_bone_sel<0)main_bone_sel=0;
		if(main_bone_sel>=main_bones)main_bone_sel=main_bones-1;
	}

	if((pdgl_mb&1) && (pdgl_mx<(-400+16*8)))
	{
		i=(pdgl_my+(300-24))/8;

		if(i==0)
		{
			UI_LoadSave_SetTitle("Load Bones", "bone");
			main_loadsave=1;
		}
		if(i==1)
		{
			UI_LoadSave_SetTitle("Save Bones", "bone");
			main_loadsave=2;
		}
		if(i==2)
		{
			UI_LoadSave_SetTitle("Load Anim", "anim");
			main_loadsave=3;
		}
		if(i==3)
		{
			UI_LoadSave_SetTitle("Save Anim", "anim");
			main_loadsave=4;
		}
		if(i==4)
		{
			UI_LoadSave_SetTitle("Load Model", "model");
			main_loadsave=5;
		}
		if(i==5)
		{
			UI_LoadSave_SetTitle("Save Model", "model");
			main_loadsave=6;
		}

		if(i==6)
		{
			UI_LoadSave_SetTitle("Load Mesh", "mesh");
			main_loadsave=7;
		}
		if(i==7)
		{
			UI_LoadSave_SetTitle("Save Mesh", "mesh");
			main_loadsave=8;
		}

		if(i==8)
		{
			UI_LoadSave_SetTitle("Import AC3D", "ac");
			main_loadsave=9;
		}
	}

	if((pdgl_mb&1) && (main_bone_sel>=0) && (pdgl_mx<(400-16*8)) &&
		(pdgl_mx>(-400+16*8)) && PDGL_KeyDown(K_SHIFT) &&
		((main_mode==MODE_BONES) || (main_mode==MODE_ANIMATE)))
	{
		main_set_keyframe();

		i=main_bone_sel;
		V3F_COPY(main_bone_rorg+i*3, io);
		v=V3F_DOT(io, main_cam_fw)-V3F_DOT(main_cam_org, main_cam_fw);

		V3F_SCALEADDSCALE(main_cam_up, -pdgl_dmy*v/300,
			main_cam_rt, pdgl_dmx*v/300, iv);

		j=main_bone_up[i];
		if(j<0)
		{
			V3F_ADD(main_bone_rorg+i*3, iv, pt);

			if(main_mode==MODE_ANIMATE)
			{
				k=frame_cur*64+i;
				frame_org[k*3+0]=pt[0];
				frame_org[k*3+1]=pt[1];
				frame_org[k*3+2]=pt[2];
			}else
			{
				V3F_COPY(pt, main_bone_org+i*3);
			}
		}else
		{
			V3F_ADD(main_bone_rorg+i*3, iv, pt);
			V3F_SUB(pt, main_bone_rorg+j*3, pt);

			imv=main_bone_rmat+j*9;
			if(main_mode==MODE_ANIMATE)
			{
				k=frame_cur*64+i;
				frame_org[k*3+0]=V3F_DOT(pt, imv+0);
				frame_org[k*3+1]=V3F_DOT(pt, imv+3);
				frame_org[k*3+2]=V3F_DOT(pt, imv+6);
			}else
			{
				main_bone_org[i*3+0]=V3F_DOT(pt, imv+0);
				main_bone_org[i*3+1]=V3F_DOT(pt, imv+3);
				main_bone_org[i*3+2]=V3F_DOT(pt, imv+6);
			}
		}
	}

	if((pdgl_mb&1) && (main_bone_sel>=0) && (pdgl_mx<(400-16*8)) &&
		(pdgl_mx>(-400+16*8)) && !PDGL_KeyDown(K_SHIFT) &&
		((main_mode==MODE_BONES) || (main_mode==MODE_ANIMATE)))
	{
		main_set_keyframe();

		i=main_bone_sel;
		V3F_COPY(main_bone_rorg+i*3, io);

		pt[0]=V3F_DOT(io, main_cam_rt)-
			V3F_DOT(main_cam_org, main_cam_rt);
		pt[1]=V3F_DOT(io, main_cam_up)-
			V3F_DOT(main_cam_org, main_cam_up);
		pt[2]=V3F_DOT(io, main_cam_fw)-
			V3F_DOT(main_cam_org, main_cam_fw);

		f=atan2((-pdgl_my/400.0)-(pt[1]/pt[2]),
			(pdgl_mx/400.0)-(pt[0]/pt[2]));
		g=atan2((-pdgl_lmy/400.0)-(pt[1]/pt[2]),
			(pdgl_lmx/400.0)-(pt[0]/pt[2]));

		Mat3F_Rotate3Matrix(main_bone_rmat+i*9,
			main_cam_fw, -(f-g), main_bone_rmat+i*9);

//		printf("rot %f\n", f-g);

		j=main_bone_up[i];
		if(j<0)
		{
			if(main_mode==MODE_ANIMATE)
			{
				k=frame_cur*64+i;
				for(j=0; j<9; j++)frame_rot[k*9+j]=
					main_bone_rmat[i*9+j];
			}else
			{
				for(j=0; j<9; j++)main_bone_rot[i*9+j]=
					main_bone_rmat[i*9+j];
			}
		}else
		{
			imv=main_bone_rmat+j*9;
			if(main_mode==MODE_ANIMATE)
			{
				k=frame_cur*64+i;
				Mat3F_Mat3MultT(main_bone_rmat+i*9, imv,
					frame_rot+k*9);
			}else
			{
				Mat3F_Mat3MultT(main_bone_rmat+i*9, imv,
					main_bone_rot+i*9);
			}

		}
	}
}

int pdgl_main_body()
{
	static int t;
	char buf[64];
	float mins[3], maxs[3];
	float io[3], iv[4], fw[2], pt[4], f, g, v;
	float im[16], im1[16], *imv, *v0, *v1, *v2;
	char *s;
	int i, j, k;
	unsigned short *kcur;
	LBXGL_Skel2MeshGroup *gtmp, *gcur;
	LBXGL_Skel2Mesh *mcur;


	f=main_cam_ang[2]*(M_PI/180);
	g=(main_cam_ang[0]-90)*(M_PI/180);
	main_cam_fw[0]=-sin(f)*cos(g);
	main_cam_fw[1]=cos(f)*cos(g);
	main_cam_fw[2]=sin(g);

	main_cam_rt[0]=cos(f);
	main_cam_rt[1]=sin(f);
	main_cam_rt[2]=0;

	main_cam_up[0]=sin(f)*sin(g);
	main_cam_up[1]=-cos(f)*sin(g);
	main_cam_up[2]=cos(g);

	iv[0]=(pdgl_mx/400.0)*main_cam_rt[0]+(-pdgl_my/400.0)*main_cam_up[0];
	iv[1]=(pdgl_mx/400.0)*main_cam_rt[1]+(-pdgl_my/400.0)*main_cam_up[1];
	iv[2]=(pdgl_mx/400.0)*main_cam_rt[2]+(-pdgl_my/400.0)*main_cam_up[2];
	main_morg[0]=main_cam_org[0]+iv[0]+main_cam_fw[0];
	main_morg[1]=main_cam_org[1]+iv[1]+main_cam_fw[1];
	main_morg[2]=main_cam_org[2]+iv[2]+main_cam_fw[2];

	main_mdir[0]=main_cam_fw[0]+iv[0];
	main_mdir[1]=main_cam_fw[1]+iv[1];
	main_mdir[2]=main_cam_fw[2]+iv[2];
	V3F_NORMALIZE(main_mdir, main_mdir);


	f=main_cam_ang[2]*(M_PI/180);
	fw[0]=-sin(f);
	fw[1]=cos(f);

	io[0]=main_cam_org[0]-10*sin(f);
	io[1]=main_cam_org[1]+10*cos(f);
	io[2]=main_cam_org[2];

	f=(((rand()&0xFFF)/2048.0)-1)*0.1;
	g=(((rand()&0xFFF)/2048.0)-1)*0.1;
	io[0]+=f;
	io[1]+=g;

	v=1000;

	main_time+=pdgl_dt_f;

	frame_time+=pdgl_dt_f;
	if(frame_time>=(2.0/15.0))frame_time=2.0/15.0;

	main_handle_input();


	Draw_SetPerspective_3D(4.0/3.0, 90, 0,
		main_cam_org, main_cam_ang, 0, 0, pdgl_xs, pdgl_ys);

	glDisable(GL_CULL_FACE);
	glDisable(GL_TEXTURE_2D);
	glEnable(GL_DEPTH_TEST);


//	PD3D_DrawGrid(0, 0, 16, 1);

//	glDisable(GL_DEPTH_TEST);
	glDisable (GL_TEXTURE_2D);

	glColor4f(0.5, 0.5, 0.5, 1);
	PD3D_DrawGrid2(48, 1);

	glColor4f(0.75, 0.75, 0.75, 1);
	PD3D_DrawGrid2(256, 12);

//	glDisable(GL_DEPTH_TEST);

	glColor4f(1, 0, 0, 1);

	glBegin(GL_LINES);

	V3F_ADD(main_cam_org, main_cam_fw, iv);
	glVertex3fv(main_cam_org);
	glVertex3fv(iv);

	V3F_ADD(main_cam_org, main_cam_rt, iv);
	glVertex3fv(main_cam_org);
	glVertex3fv(iv);

	V3F_ADD(main_cam_org, main_cam_up, iv);
	glVertex3fv(main_cam_org);
	glVertex3fv(iv);

	glEnd();

	glColor4f(0, 1, 1, 1);

	glDisable(GL_DEPTH_TEST);

	for(i=0; i<main_bones; i++)
	{
		j=main_bone_up[i];
		if(j<0)
		{
			V3F_COPY(main_bone_org+i*3, main_bone_rorg+i*3);
			Mat3F_Copy(main_bone_rot+i*9, main_bone_rmat+i*9);

			if(main_mode==MODE_ANIMATE)
			{
				main_lerp_org(i, main_bone_rorg+i*3);
				main_lerp_rot(i, main_bone_rmat+i*9);
			}
		}else
		{
			if(main_mode==MODE_ANIMATE)
			{
				main_lerp_org(i, pt);
				main_lerp_rot(i, im);
			}else
			{
				V3F_COPY(main_bone_org+i*3, pt);
				Mat3F_Copy(main_bone_rot+i*9, im);
			}

			imv=main_bone_rmat+j*9;
			Mat3F_Mat3Mult(im, imv, main_bone_rmat+i*9);
			io[0]=pt[0]*imv[0]+pt[1]*imv[3]+pt[2]*imv[6];
			io[1]=pt[0]*imv[1]+pt[1]*imv[4]+pt[2]*imv[7];
			io[2]=pt[0]*imv[2]+pt[1]*imv[5]+pt[2]*imv[8];
			V3F_ADD(io, main_bone_rorg+j*3, io);
			V3F_COPY(io, main_bone_rorg+i*3);
		}
	}

	glDisable(GL_CULL_FACE);
	glEnable(GL_DEPTH_TEST);
	glColor4f(1, 1, 1, 1);


	glEnable(GL_LIGHTING);
	glEnable(GL_NORMALIZE);

	glLightModeli(GL_LIGHT_MODEL_TWO_SIDE, GL_TRUE);
	pt[0]=1; pt[1]=1; pt[2]=1; pt[3]=1;
	glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, pt);
	pt[0]=main_cam_org[0];
	pt[1]=main_cam_org[1];
	pt[2]=main_cam_org[2];
	pt[3]=1;
	glLightfv(GL_LIGHT0, GL_POSITION, pt);
	glLightf(GL_LIGHT0, GL_QUADRATIC_ATTENUATION, 1.0/1000.0);
	glEnable(GL_LIGHT0);

	main_draw_rootmesh();

	for(i=0; i<main_bones; i++)
	{
		if(!main_bone_name[i])continue;
		main_draw_mesh(i, main_bone_rorg+i*3, main_bone_rmat+i*9);
	}


	glDisable(GL_LIGHTING);


	glDisable(GL_DEPTH_TEST);
	glBegin(GL_LINES);
	for(i=0; i<main_bones; i++)
	{
		if(!main_bone_name[i])continue;

		V3F_COPY(main_bone_rorg+i*3, io);

		glColor4f(1, 1, 0, 1);

		j=frame_cur*64+i;
		if(frame_mask[j>>3]&(1<<(j&7)))
			glColor4f(0, 1, 1, 1);

		f=V3F_DOT(io, main_mdir)-V3F_DOT(main_morg, main_mdir);
		V3F_ADDSCALE(main_morg, main_mdir, f, pt);
		f=V3F_DIST(pt, io);
		if(f<0.5)
		{
			glColor4f(1, 0, 1, 1);
//			if(PDGL_KeyDown(K_ENTER))
//				main_bone_sel=i;
		}
		if(main_bone_sel==i)
			glColor4f(0, 0.75, 0, 1);

		main_draw_star(io);

		j=main_bone_up[i];
		if(j<0)continue;

		glColor4f(0, 1, 1, 1);
		glVertex3fv(io);
		glVertex3fv(main_bone_rorg+j*3);

	}
	glEnd();

	glEnable(GL_DEPTH_TEST);

	if(main_mode==MODE_BONES)
		for(i=0; i<bone_solids; i++)
	{
		glMatrixMode(GL_MODELVIEW);
		glPushMatrix();

		j=bone_solid[i].bone;
		v0=main_bone_rorg+3*j;
		imv=main_bone_rmat+9*j;

		glTranslatef(v0[0], v0[1], v0[2]);

		for(j=0; j<3; j++)for(k=0; k<3; k++)
			im[j*4+k]=imv[j*3+k];
		im[0*4+3]=0; im[1*4+3]=0; im[2*4+3]=0;
		im[3*4+0]=0; im[3*4+1]=0; im[3*4+2]=0;
		im[3*4+3]=1;
		glMultMatrixf(im);

		v1=bone_solid[i].m;
		v2=bone_solid[i].n;

		switch(bone_solid[i].type)
		{
		case BONE_SOLID_BBOX:
			PD3D_DrawFlatCube(v1[0], v1[1], v1[2],
				v2[0], v2[1], v2[2],
				1, 0, 0, 0.25);
			break;
		case BONE_SOLID_SPHERE:
			PD3D_DrawFlatSphere(v1[0], v1[1], v1[2],
				bone_solid[i].r0,
				1, 0, 0, 0.25);
			if(bone_solid[i].r1>bone_solid[i].r0)
				PD3D_DrawFlatSphere(v1[0], v1[1], v1[2],
					bone_solid[i].r1,
					1, 0, 1, 0.25);
			break;
		case BONE_SOLID_CAPSULE:
			PD3D_DrawFlatCapsule(v1, v2, bone_solid[i].r0,
				1, 0, 0, 0.25);
			if(bone_solid[i].r1>bone_solid[i].r0)
				PD3D_DrawFlatCapsule(v1, v2, bone_solid[i].r1,
					1, 0, 1, 0.25);
			break;
		default:
			break;
		}

		glPopMatrix();
	}


	Draw_SetSolid2_2D(4.0/3.0, 400,
		VEC3(0, 0, 0), VEC3(0, 0, 0), 0, 0, pdgl_xs, pdgl_ys);

	glDisable(GL_TEXTURE_2D);
	glColor4f(0, 1, 0, 1);
	glBegin(GL_LINES);
	glVertex2f(-10, -10);
	glVertex2f(10, 10);
	glVertex2f(-10, 10);
	glVertex2f(10, -10);
	glEnd();

	Con_Render();

	GfxFont_SetFont("fixed", 0);

	sprintf(buf, "(%g %g %g) (%g %g %g)",
		main_cam_org[0], main_cam_org[1], main_cam_org[2],
		main_cam_ang[0], main_cam_ang[1], main_cam_ang[2]);
	GfxFont_DrawString(buf, -400, 300-12,
		8, 8,  0, 255, 0, 255);

	sprintf(buf, "%d/%d", (frame_cur+1), n_frames);
	GfxFont_DrawString(buf, -400, 300-12-8,
		8, 8,  0, 255, 0, 255);

	strcpy(buf, "");
	if(main_mode==MODE_BONES)strcpy(buf, "Bones");
	if(main_mode==MODE_ANIMATE)strcpy(buf, "Animate");
	if(main_mode==MODE_MESH)strcpy(buf, "Mesh");
	GfxFont_DrawString(buf, 400-(16*8), 300-12,
		8, 8,  0, 255, 0, 255);

	for(i=0; i<main_bones; i++)
	{
		j=(i==main_bone_sel);
		if(!main_bone_name[i])sprintf(buf, "    <none>");
			else sprintf(buf, "%s", main_bone_name[i]);
		GfxFont_DrawString(buf, 400-16*8, 300-24-8-i*8,
			8, 8,  j?0:255, 255, j?0:255, 255);
	}

	GfxFont_DrawString("Load-Bones", -400, 300-32,
		8, 8,  255, 255, 255, 255);
	GfxFont_DrawString("Save-Bones", -400, 300-40,
		8, 8,  255, 255, 255, 255);
	GfxFont_DrawString("Load-Anim", -400, 300-48,
		8, 8,  255, 255, 255, 255);
	GfxFont_DrawString("Save-Anim", -400, 300-56,
		8, 8,  255, 255, 255, 255);
	GfxFont_DrawString("Load-Model", -400, 300-64,
		8, 8,  255, 255, 255, 255);
	GfxFont_DrawString("Save-Model", -400, 300-72,
		8, 8,  255, 255, 255, 255);
	GfxFont_DrawString("Load-Mesh", -400, 300-80,
		8, 8,  255, 255, 255, 255);
	GfxFont_DrawString("Save-Mesh", -400, 300-88,
		8, 8,  255, 255, 255, 255);
	GfxFont_DrawString("Import AC3D", -400, 300-96,
		8, 8,  255, 255, 255, 255);
//	GfxFont_DrawString("Save-Model", -400, 300-72,
//		8, 8,  255, 255, 255, 255);

	if(main_loadsave)
	{
		UI_LoadSave_Draw();
		s=UI_LoadSave_GetName();

		if(s)
		{
			if(*s && (main_loadsave==1))
			{
				bones_file=s;
				main_load_bones(s);
			}
			if(*s && (main_loadsave==2))
			{
				bones_file=s;
				main_save_bones(s);
			}
			if(*s && (main_loadsave==3))
			{
				frame_file=s;
				main_load_anim(s);
			}
			if(*s && (main_loadsave==4))
			{
				frame_file=s;
				main_save_anim(s);
			}
			if(*s && (main_loadsave==5))
			{
				model_file=s;
				main_load_model(s);
			}
			if(*s && (main_loadsave==6))
			{
				model_file=s;
//				main_save_model(s);
			}

			if(*s && (main_loadsave==7))
			{
//				main_load_model(s);
			}
			if(*s && (main_loadsave==8))
			{
//				main_save_model(s);
			}

			if(*s && (main_loadsave==9))
			{
//				main_load_model(s);

				gtmp=AC3D_LoadModel(s);

				if(main_bone_sel>=0)
				{
					mesh_grp[main_bone_sel]=gtmp;
				}else
				{
					main_load_rootmesh(s);
				}
			}

			main_loadsave=0;
		}
	}

	PDGLUI_Frame();

	main_handle_events();


	glDisable(GL_TEXTURE_2D);
	glColor4f(1, 0, 0, 1);
	glBegin(GL_LINES);
	glVertex2f(pdgl_mx-10, -pdgl_my);
	glVertex2f(pdgl_mx+10, -pdgl_my);
	glVertex2f(pdgl_mx, -pdgl_my-10);
	glVertex2f(pdgl_mx, -pdgl_my+10);
	glEnd();

	return(0);
}

int pdgl_main_shutdown()
{
	printf("app1 shutdown\n");
	return(0);
}
