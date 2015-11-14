#ifndef BSDE_SOLID_BSP_H
#define BSDE_SOLID_BSP_H

typedef struct BSDE_BSPFace_s BSDE_BSPFace;
typedef struct BSDE_BSPNode_s BSDE_BSPNode;
typedef struct BSDE_BSPTree_s BSDE_BSPTree;

struct BSDE_BSPFace_s {
BSDE_BSPFace *next;
bsde_real norm[4];
int nvecs;
bsde_real *vecs;	//vertices
bsde_real *evecs;	//edge planes
};

struct BSDE_BSPNode_s {
BSDE_BSPNode *lnode, *rnode;	//left/right, back/front
bsde_real norm[4];
bsde_real mins[3], maxs[3];

//leaf crap
BSDE_BSPFace *face;
};

struct BSDE_BSPTree_s {
BSDE_BSPNode *root;
int nvecs, nfacepts;
bsde_real *vecs;		//vertex array
short *facepts;		//face info/points array
};

#endif
