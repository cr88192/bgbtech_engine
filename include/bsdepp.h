#ifndef BSDEPP_H
#define BSDEPP_H

#ifndef __cplusplus
#error BSDE++ can't be used with C
#endif

class bsdeSolid {
	protected:
	int handle;

	public:
	int GetAttrI(int var);
	bsde_real GetAttrF(int var);
	void GetAttrIV(int var, int *val, int n);
	void GetAttrFV(int var, bsde_real *val, int n);
	void GetAttrDV(int var, bsde_real *val, int n);

	void SetAttrI(int var, int val);
	void SetAttrF(int var, bsde_real val);
	void SetAttrIV(int var, int *val, int n);
	void SetAttrFV(int var, bsde_real *val, int n);
	void SetAttrDV(int var, bsde_real *val, int n);

	void Hide();
	void Unhide();
	void Destroy();
};

class bsdeSolidAABB : bsdeSolid {
	public:
	bsdeSolidAABB(
		bsde_real mx, bsde_real my, bsde_real mz,
		bsde_real nx, bsde_real ny, bsde_real nz);
};

class bsdeSolidOBB : bsdeSolid {
	public:
	bsdeSolidOBB(
		bsde_real mx, bsde_real my, bsde_real mz,
		bsde_real nx, bsde_real ny, bsde_real nz);
	bsdeSolidOBB(bsde_real xs, bsde_real ys, bsde_real zs);
};

class bsdeSolidSphere : bsdeSolid {
	public:
	bsdeSolidSphere(bsde_real r);
};

class bsdeSolidCylinder : bsdeSolid {
	public:
	bsdeSolidCylinder(bsde_real h, bsde_real r);
};

class bsdeSolidCapsule : bsdeSolid {
	public:
	bsdeSolidCapsule(bsde_real h, bsde_real r);
};

class bsdeSolidPlane : bsdeSolid {
	public:
	bsdeSolidPlane(bsde_real x, bsde_real y, bsde_real z, bsde_real d);
};

class bsdeSolidHull : bsdeSolid {
	public:
	bsdeSolidHull(bsde_real *norms, int n);
	bsdeSolidHull(bsde_real *norms, int n);
};

class bsdeSolidTriMesh : bsdeSolid {
	public:
	bsdeSolidTriMesh(bsde_real *vecs, int *tris, int nv, int nt);
	bsdeSolidTriMesh(bsde_real *vecs, int *tris, int nv, int nt);
};

#endif
