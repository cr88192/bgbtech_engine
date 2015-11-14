/*

Soft Bodies / Finite Element Method


*/

#include "bsde_internal.h"

BSDE_Solid *BSDE_SoftBody_NewSolid(int type)
{
	BSDE_Solid *tmp;

	tmp=(BSDE_Solid *)malloc(sizeof(BSDE_Solid));
	memset(tmp, 0, sizeof(BSDE_Solid));
	tmp->solidtype=type;
	BSDE_Quat_Identity(tmp->rot);

	V3_CONST(tmp->mins, -1);
	V3_CONST(tmp->maxs, 1);
	tmp->radius=1;
	tmp->height=1;
	tmp->volume=8;
	tmp->mass=8;

	tmp->elastic=BSDE_M_DEF_ELASTIC;
	tmp->friction=BSDE_M_DEF_FRICTION;

	tmp->fem_vtx=(BSDE_SoftBody_Vertex **)
		malloc(1024*sizeof(BSDE_SoftBody_Vertex *));
	tmp->fem_elem=(BSDE_SoftBody_Element **)
		malloc(1024*sizeof(BSDE_SoftBody_Element *));
	tmp->fem_face=(BSDE_SoftBody_Face **)
		malloc(1024*sizeof(BSDE_SoftBody_Face *));

	memset(tmp->fem_vtx, 0, 1024*sizeof(BSDE_SoftBody_Vertex *));
	memset(tmp->fem_elem, 0, 1024*sizeof(BSDE_SoftBody_Element *));
	memset(tmp->fem_face, 0, 1024*sizeof(BSDE_SoftBody_Face *));

	tmp->fem_n_vtx=1024;
	tmp->fem_n_elem=1024;
	tmp->fem_n_face=1024;

	return(tmp);
}

BSDE_SoftBody_Vertex *BSDE_SoftBody_AllocVertex(int type,
	bsde_real x, bsde_real y, bsde_real z)
{
	BSDE_SoftBody_Vertex *tmp;

	tmp=(BSDE_SoftBody_Vertex *)malloc(sizeof(BSDE_SoftBody_Vertex));
	memset(tmp, 0, sizeof(BSDE_SoftBody_Vertex));

	tmp->type=type; tmp->org[0]=x; tmp->org[1]=y; tmp->org[2]=z;
	tmp->mass=BSDE_M_DEF_GRAM;
	return(tmp);
}

BSDE_SoftBody_Element *BSDE_SoftBody_AllocElem(int type)
{
	BSDE_SoftBody_Element *tmp;

	tmp=(BSDE_SoftBody_Element *)malloc(sizeof(BSDE_SoftBody_Element));
	memset(tmp, 0, sizeof(BSDE_SoftBody_Element));

	tmp->type=type; tmp->vol=0.001;

	return(tmp);
}

BSDE_SoftBody_Face *BSDE_SoftBody_AllocFace(int type)
{
	BSDE_SoftBody_Face *tmp;

	tmp=(BSDE_SoftBody_Face *)malloc(sizeof(BSDE_SoftBody_Face));
	memset(tmp, 0, sizeof(BSDE_SoftBody_Face));
	tmp->type=type;

	return(tmp);
}
