//source/api_bsde.cpp
BSDE_API char *bsdeGetString(int str);
BSDE_API void bsdeSRand(bsdeUInt seed);
BSDE_API void bsdeSRand2(bsdeUInt seed1, bsdeUInt seed2);
BSDE_API int bsdeNewWorld();
BSDE_API void bsdeBindWorld(int i);
BSDE_API void bsdeStepWorld(double dt);
BSDE_API void bsdeEmptyWorld();
BSDE_API double bsdeGetWorldAttrF(int var);
BSDE_API void bsdeSetWorldAttrF(int var, double val);
BSDE_API int bsdeGetWorldAttrI(int var);
BSDE_API void bsdeSetWorldAttrI(int var, int val);
BSDE_API void bsdeGetWorldAttrFV(int var, float *val, int n);
BSDE_API void bsdeSetWorldAttrFV(int var, float *val, int n);
BSDE_API void bsdeGetWorldAttrDV(int var, double *val, int n);
BSDE_API void bsdeSetWorldAttrDV(int var, double *val, int n);
BSDE_API double bsdeGetWorldf(int var);
BSDE_API void bsdeSetWorldf(int var, double val);
BSDE_API int bsdeGetWorldi(int var);
BSDE_API void bsdeSetWorldi(int var, int val);
BSDE_API void bsdeGetWorldfv(int var, float *val, int n);
BSDE_API void bsdeSetWorldfv(int var, float *val, int n);
BSDE_API void bsdeGetWorlddv(int var, double *val, int n);
BSDE_API void bsdeSetWorlddv(int var, double *val, int n);
BSDE_API void bsdeGetWorld2fv(int var, float *val);
BSDE_API void bsdeSetWorld2fv(int var, float *val);
BSDE_API void bsdeGetWorld3fv(int var, float *val);
BSDE_API void bsdeSetWorld3fv(int var, float *val);
BSDE_API void bsdeGetWorld4fv(int var, float *val);
BSDE_API void bsdeSetWorld4fv(int var, float *val);
BSDE_API void bsdeGetWorld2dv(int var, double *val);
BSDE_API void bsdeSetWorld2dv(int var, double *val);
BSDE_API void bsdeGetWorld3dv(int var, double *val);
BSDE_API void bsdeSetWorld3dv(int var, double *val);
BSDE_API void bsdeGetWorld4dv(int var, double *val);
BSDE_API void bsdeSetWorld4dv(int var, double *val);
BSDE_API void bsdeSetWorld2f(int var, double x, double y);
BSDE_API void bsdeSetWorld3f(int var, double x, double y, double z);
BSDE_API void bsdeSetWorld4f(int var, double x, double y, double z, double w);
BSDE_API int *bsdeGetTriggerArray();
BSDE_API int bsdeGetTriggerArrayPos();
BSDE_API void bsdeResetTriggerArray();
BSDE_API void bsdeSetTriggerArray(int *arr, int num);
BSDE_API int bsdeNewAABB(double mx, double my, double mz,double nx, double ny, double nz);
BSDE_API int bsdeNewOBB(double mx, double my, double mz,double nx, double ny, double nz);
BSDE_API int bsdeNewSphere(double radius);
BSDE_API int bsdeNewCylinder(double height, double radius);
BSDE_API int bsdeNewCappedCylinder(double height, double radius);
BSDE_API int bsdeNewPlane(double x, double y, double z, double d);
BSDE_API int bsdeNewHullF(float *norms, int n);
BSDE_API int bsdeNewHullD(double *norms, int n);
BSDE_API int bsdeNewTriMeshF(float *vecs, int *tris, int nv, int nt);
BSDE_API int bsdeNewTriMeshD(double *vecs, int *tris, int nv, int nt);
BSDE_API int bsdeNewHullf(float *norms, int n);
BSDE_API int bsdeNewHulld(double *norms, int n);
BSDE_API int bsdeNewTriMeshf(float *vecs, int *tris, int nv, int nt);
BSDE_API int bsdeNewTriMeshd(double *vecs, int *tris, int nv, int nt);
BSDE_API void bsdeDestroy(int obj);
BSDE_API void bsdeHide(int obj);
BSDE_API void bsdeUnhide(int obj);
BSDE_API double bsdeGetAttrF(int obj, int var);
BSDE_API void bsdeSetAttrF(int obj, int var, double val);
BSDE_API int bsdeGetAttrI(int obj, int var);
BSDE_API void bsdeSetAttrI(int obj, int var, int val);
BSDE_API void bsdeGetAttrFV(int obj, int var, float *val, int n);
BSDE_API void bsdeSetAttrFV(int obj, int var, float *val, int n);
BSDE_API void bsdeGetAttrDV(int obj, int var, double *val, int n);
BSDE_API void bsdeSetAttrDV(int obj, int var, double *val, int n);
BSDE_API void bsdeGetAttrIV(int obj, int var, int *val, int n);
BSDE_API void bsdeSetAttrIV(int obj, int var, int *val, int n);
BSDE_API void bsdeGetAttr2FV(int obj, int var, float *val);
BSDE_API void bsdeSetAttr2FV(int obj, int var, float *val);
BSDE_API void bsdeGetAttr2DV(int obj, int var, double *val);
BSDE_API void bsdeSetAttr2DV(int obj, int var, double *val);
BSDE_API void bsdeGetAttr2IV(int obj, int var, int *val);
BSDE_API void bsdeSetAttr2IV(int obj, int var, int *val);
BSDE_API void bsdeGetAttr3FV(int obj, int var, float *val);
BSDE_API void bsdeSetAttr3FV(int obj, int var, float *val);
BSDE_API void bsdeGetAttr3DV(int obj, int var, double *val);
BSDE_API void bsdeSetAttr3DV(int obj, int var, double *val);
BSDE_API void bsdeGetAttr3IV(int obj, int var, int *val);
BSDE_API void bsdeSetAttr3IV(int obj, int var, int *val);
BSDE_API void bsdeGetAttr4FV(int obj, int var, float *val);
BSDE_API void bsdeSetAttr4FV(int obj, int var, float *val);
BSDE_API void bsdeGetAttr4DV(int obj, int var, double *val);
BSDE_API void bsdeSetAttr4DV(int obj, int var, double *val);
BSDE_API void bsdeGetAttr4IV(int obj, int var, int *val);
BSDE_API void bsdeSetAttr4IV(int obj, int var, int *val);
BSDE_API void bsdeSetAttr2D(int obj, int var, double x, double y);
BSDE_API void bsdeSetAttr2I(int obj, int var, int x, int y);
BSDE_API void bsdeSetAttr2F(int obj, int var, float x, float y);
BSDE_API void bsdeSetAttr3D(int obj, int var, double x, double y, double z);
BSDE_API void bsdeSetAttr3I(int obj, int var, int x, int y, int z);
BSDE_API void bsdeSetAttr3F(int obj, int var, float x, float y, float z);
BSDE_API void bsdeSetAttr4D(int obj, int var, double x, double y, double z, double w);
BSDE_API void bsdeSetAttr4I(int obj, int var, int x, int y, int z, int w);
BSDE_API void bsdeSetAttr4F(int obj, int var, float x, float y, float z, float w);
BSDE_API double bsdeGetAttrf(int obj, int var);
BSDE_API void bsdeSetAttrf(int obj, int var, double val);
BSDE_API int bsdeGetAttri(int obj, int var);
BSDE_API void bsdeSetAttri(int obj, int var, int val);
BSDE_API void bsdeGetAttrfv(int obj, int var, float *val, int n);
BSDE_API void bsdeSetAttrfv(int obj, int var, float *val, int n);
BSDE_API void bsdeGetAttrdv(int obj, int var, double *val, int n);
BSDE_API void bsdeSetAttrdv(int obj, int var, double *val, int n);
BSDE_API void bsdeGetAttriv(int obj, int var, int *val, int n);
BSDE_API void bsdeSetAttriv(int obj, int var, int *val, int n);
BSDE_API void bsdeGetAttr2fv(int obj, int var, float *val);
BSDE_API void bsdeSetAttr2fv(int obj, int var, float *val);
BSDE_API void bsdeGetAttr2dv(int obj, int var, double *val);
BSDE_API void bsdeSetAttr2dv(int obj, int var, double *val);
BSDE_API void bsdeGetAttr2iv(int obj, int var, int *val);
BSDE_API void bsdeSetAttr2iv(int obj, int var, int *val);
BSDE_API void bsdeGetAttr3fv(int obj, int var, float *val);
BSDE_API void bsdeSetAttr3fv(int obj, int var, float *val);
BSDE_API void bsdeGetAttr3dv(int obj, int var, double *val);
BSDE_API void bsdeSetAttr3dv(int obj, int var, double *val);
BSDE_API void bsdeGetAttr3iv(int obj, int var, int *val);
BSDE_API void bsdeSetAttr3iv(int obj, int var, int *val);
BSDE_API void bsdeGetAttr4fv(int obj, int var, float *val);
BSDE_API void bsdeSetAttr4fv(int obj, int var, float *val);
BSDE_API void bsdeGetAttr4dv(int obj, int var, double *val);
BSDE_API void bsdeSetAttr4dv(int obj, int var, double *val);
BSDE_API void bsdeGetAttr4iv(int obj, int var, int *val);
BSDE_API void bsdeSetAttr4iv(int obj, int var, int *val);
BSDE_API void bsdeSetAttr2f(int obj, int var, double x, double y);
BSDE_API void bsdeSetAttr2i(int obj, int var, int x, int y);
BSDE_API void bsdeSetAttr3f(int obj, int var, double x, double y, double z);
BSDE_API void bsdeSetAttr3i(int obj, int var, int x, int y, int z);
BSDE_API void bsdeSetAttr4f(int obj, int var, double x, double y, double z, double w);
BSDE_API void bsdeSetAttr4i(int obj, int var, int x, int y, int z, int w);
BSDE_API int bsdeNewJoint(int type);
BSDE_API void bsdeBindJointSolids(int obj, int obja, int objb);
BSDE_API double bsdeGetJointAttrF(int obj, int var);
BSDE_API void bsdeSetJointAttrF(int obj, int var, double val);
BSDE_API int bsdeGetJointAttrI(int obj, int var);
BSDE_API void bsdeSetJointAttrI(int obj, int var, int val);
BSDE_API void bsdeGetJointAttrFV(int obj, int var, float *val, int n);
BSDE_API void bsdeSetJointAttrFV(int obj, int var, float *val, int n);
BSDE_API void bsdeGetJointAttrDV(int obj, int var, double *val, int n);
BSDE_API void bsdeSetJointAttrDV(int obj, int var, double *val, int n);
BSDE_API double bsdeGetJointf(int obj, int var);
BSDE_API void bsdeSetJointf(int obj, int var, double val);
BSDE_API int bsdeGetJointi(int obj, int var);
BSDE_API void bsdeSetJointi(int obj, int var, int val);
BSDE_API void bsdeGetJointfv(int obj, int var, float *val, int n);
BSDE_API void bsdeSetJointfv(int obj, int var, float *val, int n);
BSDE_API void bsdeGetJointdv(int obj, int var, double *val, int n);
BSDE_API void bsdeSetJointdv(int obj, int var, double *val, int n);
BSDE_API void bsdeSetJoint2f(int obj, int var, double x, double y);
BSDE_API void bsdeSetJoint3f(int obj, int var, double x, double y, double z);
BSDE_API void bsdeSetJoint4f(int obj, int var, double x, double y, double z, double w);
BSDE_API void bsdeSetJointCubic(int obj,double vx, double vy, double sx, double sy, double fa, double fb, double fc, double fd);
BSDE_API void bsdeSetJointQuadratic(int obj,double vx, double vy, double sx, double sy, double fa, double fb, double fc);
BSDE_API void bsdeBegin(int type);
BSDE_API int bsdeEnd();
BSDE_API void bsdeVertex3f(double x, double y, double z);
BSDE_API void bsdeVertex4f(double x, double y, double z, double w);
BSDE_API void bsdeVertex3d(double x, double y, double z);
BSDE_API void bsdeVertex4d(double x, double y, double z, double w);
BSDE_API void bsdeVertex3fv(float *v);
BSDE_API void bsdeVertex4fv(float *v);
BSDE_API void bsdeVertex3dv(double *v);
BSDE_API void bsdeVertex4dv(double *v);
BSDE_API void bsdeNormal3f(double x, double y, double z);
BSDE_API void bsdeNormal4f(double x, double y, double z, double w);
BSDE_API void bsdeNormal3d(double x, double y, double z);
BSDE_API void bsdeNormal4d(double x, double y, double z, double w);
BSDE_API void bsdeNormal3fv(float *v);
BSDE_API void bsdeNormal4fv(float *v);
BSDE_API void bsdeNormal3dv(double *v);
BSDE_API void bsdeNormal4dv(double *v);
BSDE_API void bsdeAttrI(int var, int val);
BSDE_API void bsdeAttrF(int var, double val);
BSDE_API void bsdeAttrIV(int var, int *val, int num);
BSDE_API void bsdeAttrFV(int var, float *val, int num);
BSDE_API void bsdeAttrDV(int var, double *val, int num);
BSDE_API void bsdeAttri(int var, int val);
BSDE_API void bsdeAttrf(int var, double val);
BSDE_API void bsdeAttriv(int var, int *val, int num);
BSDE_API void bsdeAttrfv(int var, float *val, int num);
BSDE_API void bsdeAttrdv(int var, double *val, int num);
BSDE_API void bsdeAttr2i(int var, int x, int y);
BSDE_API void bsdeAttr3i(int var, int x, int y, int z);
BSDE_API void bsdeAttr4i(int var, int x, int y, int z, int w);
BSDE_API void bsdeAttr2f(int var, double x, double y);
BSDE_API void bsdeAttr3f(int var, double x, double y, double z);
BSDE_API void bsdeAttr4f(int var, double x, double y, double z, double w);
BSDE_API int bsdePeekMessage();
BSDE_API int bsdeNextMessage();
BSDE_API int bsdeGetMessagei(int var);
BSDE_API double bsdeGetMessagef(int var);
BSDE_API void bsdeGetMessageiv(int var, int *val, int n);
BSDE_API void bsdeGetMessagefv(int var, float *val, int n);
BSDE_API void bsdeGetMessagedv(int var, double *val, int n);
BSDE_API void bsdeGetMessage2iv(int var, int *val);
BSDE_API void bsdeGetMessage2fv(int var, float *val);
BSDE_API void bsdeGetMessage2dv(int var, double *val);
BSDE_API void bsdeGetMessage3iv(int var, int *val);
BSDE_API void bsdeGetMessage3fv(int var, float *val);
BSDE_API void bsdeGetMessage3dv(int var, double *val);
BSDE_API void bsdeGetMessage4iv(int var, int *val);
BSDE_API void bsdeGetMessage4fv(int var, float *val);
BSDE_API void bsdeGetMessage4dv(int var, double *val);
BSDE_API int bsdeQueryLineDV(int *hdl, int mhdl, double *start, double *end);
BSDE_API int bsdeQueryBoxDV(int *hdl, int mhdl, double *mins, double *maxs);
BSDE_API int bsdeQueryHullDV(int *hdl, int mhdl, double *nv, int nnv);
BSDE_API int bsdeQueryLineFV(int *hdl, int mhdl, float *start, float *end);
BSDE_API int bsdeQueryBoxFV(int *hdl, int mhdl, float *mins, float *maxs);
BSDE_API int bsdeQueryHullFV(int *hdl, int mhdl, float *nv, int nnv);
