//source/solid_aabb.cpp
void BSDE_SolidAABB_BoxMove(double *bmins, double *bmaxs,double *start, double *end, double *mins, double *maxs);
int BSDE_SolidAABB_BoxCollideP(double *ma, double *na, double *mb, double *nb);
int BSDE_SolidAABB_BoxCollideAxis(double *ma, double *na, double *mb, double *nb);
int BSDE_SolidAABB_PointInBoxP(double *p, double *min, double *max);
void BSDE_SolidAABB_BoxIntersection(double *ma, double *na,double *mb, double *nb, double *mc, double *nc);
int BSDE_SolidAABB_BoxCollideContact(double *ma, double *na,double *mb, double *nb, double *org, double *norm, double *dist);
void BSDE_SolidAABB_BoxPlaneExtents(double *mins, double *maxs,double *norm, double *min, double *max);
int BSDE_SolidAABB_BoxLineCollideP(double *min, double *max,double *sp, double *ep);
//source/solid_obb.cpp
int BSDE_SolidOBB_CheckObbPlane(double *mins, double *maxs,double *xform, double *norm, double *io);
int BSDE_SolidOBB_ObbPlaneIntersect(double *mins, double *maxs,double *xform, double *norm, double *io);
int BSDE_SolidOBB_ObbPlaneExtents(double *mins, double *maxs,double *xform, double *norm, double *ext);
void BSDE_SolidOBB_PlaneExtents(double *mins, double *maxs,double *xform, double *norm, double *min, double *max);
int BSDE_SolidOBB_ObbPlaneMoveExtents(double *mins, double *maxs,double *sxform, double *exform, double *norm, double *ext);
int BSDE_SolidOBB_CheckObbMovePlane(double *mins, double *maxs,double *sxform, double *exform, double *norm, double *io);
int BSDE_SolidOBB_CheckCollide(double *amins, double *amaxs, double *apos, double *bmins, double *bmaxs, double *bpos);
int BSDE_SolidOBB_CheckObbMove(double *amins, double *amaxs, double *aspos, double *aepos, double *bmins, double *bmaxs, double *bspos, double *bepos, double *org);
int BSDE_SolidOBB_ObbImpactNormal(double *amins, double *amaxs, double *aspos, double *aepos, double *bmins, double *bmaxs, double *bspos, double *bepos, double *vel, double *norm);
void BSDE_SolidOBB_Box(double *mins, double *maxs, double *pos,double *bmins, double *bmaxs);
void BSDE_SolidOBB_BoxMove(double *mins, double *maxs, double *spos, double *epos, double *bmins, double *bmaxs);
int BSDE_SolidOBB_ObbPointCollide(double *mins, double *maxs, double *pos, double *org);
void BSDE_SolidOBB_GenPoints(double *mins, double *maxs, double *pos, double *pts);
void BSDE_SolidOBB_GenPoints4x(double *mins, double *maxs, double *pos, double *pts);
void BSDE_SolidOBB_GenPointsNx(double *mins, double *maxs, double *pos, double *pts, int n);
void BSDE_SolidOBB_GenPointsCubeNx(double *mins, double *maxs, double *pts, int n);
int BSDE_SolidOBB_ClipPoints(double *mins, double *maxs, double *pos, double *ipts, double *opts, int npts);
void BSDE_SolidOBB_ObbNearestPoint(double *mins, double *maxs, double *pos, double *org, double *pt);
//source/solid_obb3.cpp
int BSDE_SolidOBB3_CollideNormal(double *amins, double *amaxs, double *apos, double *bmins, double *bmaxs, double *bpos, double *norm, double *dist);
void BSDE_SolidOBB3_GenPoints(double *mins, double *maxs, double *pos, double *pts);
void BSDE_SolidOBB3_GenPointsLocal(double *mins, double *maxs, double *pts);
void BSDE_SolidOBB3_GenNormsLocal(double *mins, double *maxs, double *norms);
void BSDE_SolidOBB3_ObbBestNormalPoint(double *mins, double *maxs, double *pos, double *org, double *norm);
int BSDE_SolidOBB3_CheckPointCollide(double *mins, double *maxs, double *pos, double *pt);
int BSDE_SolidOBB3_CollideContact(double *amins, double *amaxs, double *apos, double *bmins, double *bmaxs, double *bpos, double *rorg, double *rnorm, double *rdist);
int BSDE_SolidOBB3_CollideAABBContact(double *amins, double *amaxs, double *apos, double *bmins, double *bmaxs, double *bpos, double *rorg, double *rnorm, double *rdist);
//source/solid_cylinder.cpp
void BSDE_SolidCylinder_MakeBox(double height, double radius, int t,double *pos, double *mins, double *maxs);
void BSDE_SolidCylinder_BoxMove(double height, double radius, int t, double *spos, double *epos, double *mins, double *maxs);
int BSDE_SolidCylinder_CollideObbContact(double alen, double arad, int t, double *apos, double *bmins, double *bmaxs, double *bpos, double *org, double *norm, double *dist);
double BSDE_SolidCylinder_AxisRadius(double len, double rad, int t, double *pos, double *norm);
void BSDE_SolidCylinder_PointLocal(double *pos, double *org, double *pt);
void BSDE_SolidCylinder_NearestPoint(double len, double rad, int t, double *pos, double *point, double *pt);
int BSDE_SolidCylinder_PointCollide(double len, double rad, int t, double *pos, double *point);
void BSDE_SolidCylinder_OffsetCPA(double *ua, double *va, double *ub, double *vb, double *da, double *db);
int BSDE_SolidCylinder_LineCollide(double len, double rad, int t, double *pos, double *sp, double *ep);
int BSDE_SolidCylinder_CollideContact(double alen, double arad, int at, double *apos, double blen, double brad, int bt, double *bpos, double *org, double *norm, double *dist);
int BSDE_SolidCylinder_CollideObbContact(double alen, double arad, int ty, double *apos, double *bmins, double *bmaxs, double *bpos, double *org, double *norm, double *dist);
//source/solid_brep.cpp
void BSDE_SolidBRep_ProjectTriangleLine(double *u0, double *u1, double *u2, double *iv, double *rm, double *rn);
int BSDE_SolidBRep_CheckLineTriangleTriangle(double *u0, double *u1, double *u2, double *v0, double *v1, double *v2, double *iv);
int BSDE_SolidBRep_CheckLineTriangleTriangle2(double *u0, double *u1, double *u2, double *v0, double *v1, double *v2, double *iv, double *pt);
int BSDE_SolidBRep_CheckTriangleTriangle(double *u0, double *u1, double *u2, double *v0, double *v1, double *v2);
int BSDE_SolidBRep_CheckTriangleTriangle2(double *u0, double *u1, double *u2, double *v0, double *v1, double *v2, double *iorg, double *idir);
int BSDE_SolidBRep_GetTriangleNormal(double *u0, double *u1, double *u2, double *io);
int BSDE_SolidBRep_GetTriangleIntersection(double *u0, double *u1, double *u2, double *v0, double *v1, double *v2, double *io);
int BSDE_SolidBRep_CheckBrep(double *av, int *at, int na, double *bv, int *bt, int nb, double *rio, double *riv);
int BSDE_SolidBRep_CheckBrep2(double *av, int *at, int na, double *bv, int *bt, int nb, double *rio, double *riv, int rn);
int BSDE_SolidBRep_TrimBrep(double *av, int *at, int na,int *at2, double *bmins, double *bmaxs);
void BSDE_SolidBRep_ProjectLine(double *vecs, int *tris, int nv, int nt, double *pos, double *norm, double *rmin, double *rmax);
void BSDE_SolidBRep_ProjectVecsLine(double *vecs, int nv, double *norm, double *rmin, double *rmax);
void BSDE_SolidBRep_ProjectLine2(BSDE_Solid *obj, double *pos,double *norm, double *rmin, double *rmax);
void BSDE_SolidBRep_ProjectLine3(BSDE_Solid *obj,double *norm, double *rmin, double *rmax);
double BSDE_SolidBRep_ApproxBrepVolume(double *vecs, int *tris, int nv, int nt);
void BSDE_SolidBRep_PlaneIntersect(BSDE_Solid *obj, double *pos,double *norm, double *io);
void BSDE_SolidBRep_Predict(BSDE_Solid *obj);
int BSDE_SolidBRep_CheckCollide(BSDE_Solid *aobj, BSDE_Solid *bobj,double *apos, double *bpos);
int BSDE_SolidBRep_MultiContact(BSDE_World *world,BSDE_Solid *aobj, BSDE_Solid *bobj, double *apos, double *bpos, BSDE_Contact **lst, int num);
//source/solid_sphere.cpp
int BSDE_SolidSphere_CollideContact(double arad, double *aorg, double brad, double *borg, double *org, double *norm, double *dist);
int BSDE_SolidSphere_CollideObbContact(double arad, double *aorg, double *bmins, double *bmaxs, double *pos, double *org, double *norm, double *dist);
int BSDE_SolidSphere_CollideObbContact(double arad, double *aorg, double *bmins, double *bmaxs, double *bpos, double *org, double *norm, double *dist);
//source/solid_plane.cpp
void BSDE_SolidPlane_TransformNormal(double *dir, double *xform, double *norm);
int BSDE_SolidPlane_PointCollide(double *dir, double *xform, double *point);
//source/solid_hull.cpp
void BSDE_SolidHull_ModifyHullFaces(BSDE_Solid *obj, double *norms, int n);
void BSDE_SolidHull_MakeHullFaces(BSDE_Solid *obj);
void BSDE_SolidHull_ComputeVolume(BSDE_Solid *obj);
void BSDE_SolidHull_ProjectLine(BSDE_Solid *obj,double *xform, double *norm, double *min, double *max);
void BSDE_SolidHull_ProjectLine(BSDE_Solid *obj,double *xform, double *norm, double *min, double *max);
void BSDE_SolidHull_Box(BSDE_Solid *obj,double *xform, double *mins, double *maxs);
void BSDE_SolidHull_BoxMove(BSDE_Solid *obj,double *s_xform, double *e_xform, double *mins, double *maxs);
int BSDE_SolidHull_PointCollide(BSDE_Solid *obj, double *xform,double *org);
int BSDE_SolidHull_LineCollide(BSDE_Solid *obj, double *xform,double *sp, double *ep);
void BSDE_SolidHull_NearestPoint(BSDE_Solid *obj, double *xform,double *org, double *pt);
int BSDE_SolidHull_CheckCollide(BSDE_Solid *aobj, BSDE_Solid *bobj,double *axform, double *bxform);
int BSDE_SolidHull_CheckObbCollide(BSDE_Solid *aobj, BSDE_Solid *bobj,double *axform, double *bxform);
int BSDE_SolidHull_CheckSphereCollide(BSDE_Solid *aobj, BSDE_Solid *bobj,double *axform, double *bxform, double *aorg, double *borg);
int BSDE_SolidHull_CheckCylinderCollide(BSDE_Solid *aobj, BSDE_Solid *bobj,double *axform, double *bxform, double *aorg, double *borg);
int BSDE_SolidHull_CheckTriMeshCollide(BSDE_Solid *aobj, BSDE_Solid *bobj,double *axform, double *bxform);
int BSDE_SolidHull_CollidePoints(BSDE_Solid *obj,double *ipts, double *opts, int npts);
void BSDE_SolidHull_CollideOrigin(BSDE_Solid *aobj, BSDE_Solid *bobj,double *org);
int BSDE_SolidHull_ContactHull(BSDE_Solid *aobj, BSDE_Solid *bobj,double *axform, double *bxform, double *aorg, double *borg, double *org, double *norm, double *dist);
int BSDE_SolidHull_ContactBox(BSDE_Solid *aobj, BSDE_Solid *bobj,double *axform, double *bxform, double *aorg, double *borg, double *org, double *norm, double *dist);
int BSDE_SolidHull_ContactSphere(BSDE_Solid *aobj, BSDE_Solid *bobj,double *axform, double *bxform, double *aorg, double *borg, double *org, double *norm, double *dist);
int BSDE_SolidHull_ContactGeneric(BSDE_Solid *aobj, BSDE_Solid *bobj,double *axform, double *bxform, double *aorg, double *borg, double *org, double *dir, double *dist);
void BSDE_SolidHull_HullPlaneIntersect(BSDE_Solid *obj, double *pos,double *norm, double *io);
//source/solid_hull2.cpp
void BSDE_SolidHull2_ProjectPointsLine(double *vecs, int nvecs,double *xform, double *norm, double *min, double *max);
int BSDE_SolidHull2_CheckCollide(double *anorm, double *bnorm, double *avecs, double *bvecs, int anf, int bnf, int anv, int bnv, double *axform, double *bxform);
int BSDE_SolidHull2_CalcCollide(BSDE_Solid *aobj, BSDE_Solid *bobj, double *anorm, double *bnorm, double *avecs, double *bvecs, int anf, int bnf, int anv, int bnv, double *axform, double *bxform, double *org, double *norm, double *dist);
int BSDE_SolidHull2_CollideOBB(double *amins, double *amaxs, double *bmins, double *bmaxs, double *axform, double *bxform, double *rorg, double *rnorm, double *rdist);
int BSDE_SolidHull2_CalcCollideStatic(BSDE_Solid *aobj, BSDE_Solid *bobj, double *anorm, double *bnorm, double *avecs, double *bvecs, int anf, int bnf, int anv, int bnv, double *axform, double *bxform, double *org, double *norm, double *dist);
int BSDE_SolidHull2_CalcCollideStaticSphere(BSDE_Solid *aobj, BSDE_Solid *bobj, double *anorm, double *avecs, int anf, int anv, double *axform, double *bxform, double *org, double *norm, double *dist);
//source/solid_bsp.cpp
void BSDE_SolidBSP_FaceExtents(BSDE_BSPFace *face, double *norm,double *min, double *max);
int BSDE_SolidBSP_CheckFastMoveFace(BSDE_BSPFace *face,BSDE_Solid *obj, double *pos, double *org, double *norm, int max);
int BSDE_SolidBSP_CheckGenericMoveFace(BSDE_BSPFace *face,BSDE_Solid *obj, double *pos, double *org, double *norm, int max);
void BSDE_SolidBSP_ObbFaceIntersect(double *mins, double *maxs,double *pos, BSDE_BSPFace *face, double *io);
int BSDE_SolidBSP_CheckObbMoveFace(BSDE_BSPFace *face,BSDE_Solid *obj, double *pos, double *org, double *norm, int max);
int BSDE_SolidBSP_CheckSphereFace(BSDE_BSPFace *face,BSDE_Solid *obj, double *pos, double *org, double *norm, int max);
int BSDE_SolidBSP_CheckCylinderFace(BSDE_BSPFace *face,BSDE_Solid *obj, double *pos, double *org, double *norm, int max);
int BSDE_SolidBSP_CheckHullFace(BSDE_BSPFace *face,BSDE_Solid *obj, double *pos, double *org, double *norm, int max);
int BSDE_SolidBSP_CheckTriMeshFace(BSDE_BSPFace *face,BSDE_Solid *obj, double *pos, double *org, double *norm, int max);
int BSDE_SolidBSP_CheckMoveFace(BSDE_BSPFace *face,BSDE_Solid *obj, double *pos, double *org, double *norm, int max);
int BSDE_SolidBSP_CheckMoveLeaf(BSDE_BSPNode *node,BSDE_Solid *obj, double *pos, double *org, double *norm, int max);
int BSDE_SolidBSP_CheckMoveNode(BSDE_BSPNode *node,BSDE_Solid *obj, double *pos, double *org, double *norm, int max);
int BSDE_SolidBSP_CheckCollide(BSDE_Solid *aobj, double *apos, BSDE_Solid *bobj, double *bpos);
int BSDE_SolidBSP_CollideMultiContact(BSDE_World *world, BSDE_Solid *aobj, double *apos, BSDE_Solid *bobj, double *bpos, BSDE_Contact **lst);
int BSDE_SolidBSP_TraceFace2(BSDE_BSPFace *face, double *sp, double *ep,double *io);
BSDE_BSPFace *BSDE_SolidBSP_TraceFaces2(BSDE_BSPFace *face,double *sp, double *ep, double *io);
BSDE_BSPFace *BSDE_SolidBSP_TraceNode2(BSDE_BSPNode *node,double *sp, double *ep, double *io);
int BSDE_SolidBSP_LineCollide(BSDE_Solid *obj, double *xform,double *isp, double *iep);
BSDE_BSPFace *BSDE_SolidBSP_BuildFace(BSDE_World *world,int **arrp, double **varrp, double *mins, double *maxs);
BSDE_BSPNode *BSDE_SolidBSP_BuildLeaf(BSDE_World *world,int **arrp, double **varrp, double *mins, double *maxs);
BSDE_BSPNode *BSDE_SolidBSP_BuildNode(BSDE_World *world,int **arrp, double **varrp, double *mins, double *maxs);
BSDE_Solid *BSDE_SolidBSP_BuildArray(BSDE_World *world,int **arrp, double **varrp);
//source/solid_bsp2.cpp
void BSDE_SolidBSP2_FaceExtents(BSDE_BSPFace *face, double *xform,double *norm, double *min, double *max);
int BSDE_SolidBSP2_CheckFastMoveFace(BSDE_BSPFace *face, double *xform,BSDE_Solid *obj, double *pos, double *org, double *norm, int max);
int BSDE_SolidBSP2_CheckGenericMoveFace(BSDE_BSPFace *face, double *xform,BSDE_Solid *obj, double *pos, double *org, double *norm, int max);
void BSDE_SolidBSP2_ObbFaceIntersect(double *mins, double *maxs,double *pos, BSDE_BSPFace *face, double *xform, double *io);
int BSDE_SolidBSP2_CheckObbMoveFace(BSDE_BSPFace *face, double *xform,BSDE_Solid *obj, double *pos, double *org, double *norm, int max);
int BSDE_SolidBSP2_CheckSphereFace(BSDE_BSPFace *face, double *xform,BSDE_Solid *obj, double *pos, double *org, double *norm, int max);
int BSDE_SolidBSP2_CheckHullFace(BSDE_BSPFace *face, double *xform,BSDE_Solid *obj, double *pos, double *org, double *norm, int max);
int BSDE_SolidBSP2_CheckMoveFace(BSDE_BSPFace *face, double *xform,BSDE_Solid *obj, double *pos, double *org, double *norm, int max);
int BSDE_SolidBSP2_CheckMoveLeaf(BSDE_BSPNode *node, double *xform,BSDE_Solid *obj, double *pos, double *org, double *norm, int max);
int BSDE_SolidBSP2_CheckMoveNode(BSDE_BSPNode *node, double *xform,BSDE_Solid *obj, double *pos, double *org, double *norm, int max);
int BSDE_SolidBSP2_CheckCollide(BSDE_Solid *aobj, double *apos, BSDE_Solid *bobj, double *bpos);
int BSDE_SolidBSP2_CollideMultiContact(BSDE_World *world, BSDE_Solid *aobj, double *apos, BSDE_Solid *bobj, double *bpos, BSDE_Contact **lst);
int BSDE_SolidBSP2_CheckIsecBoxFace(BSDE_BSPFace *face, double *xform,double *mins, double *maxs, double *pos);
int BSDE_SolidBSP2_CheckIsecBoxLeaf(BSDE_BSPNode *node, double *xform,double *mins, double *maxs, double *pos);
int BSDE_SolidBSP2_CheckIsecBoxNode(BSDE_BSPNode *node, double *xform,double *mins, double *maxs, double *pos);
int BSDE_SolidBSP2_BoxCollide(BSDE_Solid *obj, double *xform,double *mins, double *maxs, double *pos);
//source/bsde_collide.cpp
int bsde_checkcollide_obb(BSDE_Solid *aobj, BSDE_Solid *bobj,double *axform, double *bxform, double *aorg, double *borg);
int bsde_checkcollide_sphere(BSDE_Solid *aobj, BSDE_Solid *bobj,double *axform, double *bxform, double *aorg, double *borg);
int bsde_checkcollide_cylinder(BSDE_Solid *aobj, BSDE_Solid *bobj,double *axform, double *bxform, double *aorg, double *borg, int t);
int bsde_checkcollide_plane(BSDE_Solid *aobj, BSDE_Solid *bobj,double *axform, double *bxform, double *aorg, double *borg);
int bsde_checkcollide_hull(BSDE_Solid *aobj, BSDE_Solid *bobj,double *axform, double *bxform, double *aorg, double *borg);
int bsde_checkcollide(BSDE_Solid *aobj, BSDE_Solid *bobj,double *axform, double *bxform, double *aorg, double *borg);
int BSDE_CheckCollide(BSDE_Solid *aobj, BSDE_Solid *bobj);
int BSDE_CheckInitialCollide(BSDE_Solid *aobj, BSDE_Solid *bobj);
int BSDE_CheckCollide2(BSDE_Solid *aobj, BSDE_Solid *bobj, int m);
double BSDE_CollideTimeR(BSDE_Solid *aobj, BSDE_Solid *bobj,double bt, double dt, double mdt);
double BSDE_CollideTime(BSDE_Solid *aobj, BSDE_Solid *bobj,double dt, double mdt);
int bsde_collide_aabb(BSDE_World *world, BSDE_Solid *aobj, BSDE_Solid *bobj,BSDE_Contact **lst, double *axform, double *bxform, double *aorg, double *borg);
int bsde_collide_obb(BSDE_World *world, BSDE_Solid *aobj, BSDE_Solid *bobj,BSDE_Contact **lst, double *axform, double *bxform, double *aorg, double *borg);
int bsde_collide_sphere(BSDE_World *world,BSDE_Solid *aobj, BSDE_Solid *bobj, BSDE_Contact **lst, double *axform, double *bxform, double *aorg, double *borg);
int bsde_collide_cylinder(BSDE_World *world,BSDE_Solid *aobj, BSDE_Solid *bobj, BSDE_Contact **lst, double *axform, double *bxform, double *aorg, double *borg, int t);
int bsde_collide_plane(BSDE_World *world, BSDE_Solid *aobj, BSDE_Solid *bobj, BSDE_Contact **lst, double *axform, double *bxform, double *aorg, double *borg);
int bsde_collide_hull(BSDE_World *world,BSDE_Solid *aobj, BSDE_Solid *bobj, BSDE_Contact **lst,  double *axform, double *bxform, double *aorg, double *borg);
int bsde_collide(BSDE_World *world,BSDE_Solid *aobj, BSDE_Solid *bobj, BSDE_Contact **lst, double *axform, double *bxform, double *aorg, double *borg);
int BSDE_Collide(BSDE_World *world, BSDE_Solid *aobj, BSDE_Solid *bobj,BSDE_Contact **lst);
int BSDE_InitialCollide(BSDE_World *world,BSDE_Solid *aobj, BSDE_Solid *bobj, BSDE_Contact **lst);
//source/bsde_solid.cpp
BSDE_Contact *BSDE_MakeContact(BSDE_World *world, BSDE_Solid *obja, BSDE_Solid *objb, double *org, double *norm, double depth);
BSDE_Contact *BSDE_MakePointsContact(BSDE_World *world, BSDE_Solid *obja, BSDE_Solid *objb, double *pts, double *norm, double depth, int npts);
void BSDE_FreeContacts();
BSDE_Solid *BSDE_NewSolidArray(BSDE_World *world,int type, int **arr, double **varr, double **sarr);
void BSDE_ArraySkipUnknown(int **arr, double **varr);
BSDE_Solid *BSDE_NewSolid(int type);
BSDE_Solid *BSDE_NewAABB(double mx, double my, double mz, double nx, double ny, double nz);
BSDE_Solid *BSDE_NewOBB(double mx, double my, double mz, double nx, double ny, double nz);
BSDE_Solid *BSDE_NewSphere(double rad);
BSDE_Solid *BSDE_NewCylinder(double height, double radius);
BSDE_Solid *BSDE_NewCappedCylinder(double height, double radius);
BSDE_Solid *BSDE_NewPlane(double x, double y, double z, double d);
BSDE_Solid *BSDE_NewHull(double *norms, int n);
BSDE_Solid *BSDE_NewTriMesh(double *vecs, int *tris, int nv, int nt);
void BSDE_Destroy(BSDE_Solid *self);
void BSDE_Box(BSDE_Solid *self, double *mins, double *maxs);
void BSDE_BoxMove(BSDE_Solid *self, double *mins, double *maxs);
void BSDE_BoxMovePos(BSDE_Solid *self,double *sxform, double *exform, double *sorg, double *eorg, double *mins, double *maxs);
double BSDE_GetAttrF(BSDE_Solid *self, int var);
void BSDE_SetAttrF(BSDE_Solid *self, int var, double val);
int BSDE_GetAttrI(BSDE_Solid *self, int var);
void BSDE_SetAttrI(BSDE_Solid *self, int var, int val);
void BSDE_GetAttrFV(BSDE_Solid *self, int var, float *val, int n);
void BSDE_SetAttrFV(BSDE_Solid *self, int var, float *val, int n);
void BSDE_GetAttrDV(BSDE_Solid *self, int var, double *val, int n);
void BSDE_SetAttrDV(BSDE_Solid *self, int var, double *val, int n);
void BSDE_GetAttrIV(BSDE_Solid *self, int var, int *val, int n);
void BSDE_SetAttrIV(BSDE_Solid *self, int var, int *val, int n);
int BSDE_PointCollide(BSDE_Solid *self, double *point);
int BSDE_LineCollide(BSDE_Solid *self, double *isp, double *iep);
void BSDE_PlaneExtents(BSDE_Solid *self, double *normal,double *min, double *max);
void BSDE_PlaneExtentsPos(BSDE_Solid *self, double *pos, double *normal,double *min, double *max);
void BSDE_PlaneMoveExtents(BSDE_Solid *self, double *normal,double *min, double *max);
void BSDE_PlaneFastMoveExtents(BSDE_Solid *self, double *normal,double *min, double *max);
int BSDE_BoxCollide(BSDE_Solid *self, double *mins, double *maxs);
int BSDE_HullCollide(BSDE_Solid *self, double *nv, int nnv);
//source/bsde_joint.cpp
BSDE_Joint *BSDE_NewJoint(int type);
BSDE_Joint *BSDE_NextJointLink(BSDE_Joint *joint, BSDE_Solid *obj);
void BSDE_SetNextJointLink(BSDE_Joint *joint, BSDE_Solid *obj,BSDE_Joint *nxt);
void BSDE_UnlinkJointSolid(BSDE_Joint *joint, BSDE_Solid *obj);
void BSDE_BindJointSolids(BSDE_Joint *joint,BSDE_Solid *obja, BSDE_Solid *objb);
void BSDE_JointConstrainPoints(BSDE_World *world,BSDE_Joint *joint, double *orga, double *orgb, double dt);
void BSDE_JointConstrainPoints2(BSDE_World *world,BSDE_Joint *joint, double *orga, double *orgb, double dt);
void BSDE_JointConstrainPointsDirect(BSDE_World *world,BSDE_Joint *joint, double *orga, double *orgb, double dt);
void BSDE_HandleJointFixed(BSDE_World *world, BSDE_Joint *joint, double dt);
void BSDE_HandleJointBall(BSDE_World *world, BSDE_Joint *joint, double dt);
void BSDE_HandleJointHinge(BSDE_World *world, BSDE_Joint *joint, double dt);
void BSDE_HandleJointUniversal(BSDE_World *world,BSDE_Joint *joint, double dt);
void BSDE_HandleJointElastic(BSDE_World *world,BSDE_Joint *joint, double dt);
void BSDE_HandleJoint(BSDE_World *world, BSDE_Joint *joint, double dt);
double BSDE_GetJointAttrF(BSDE_Joint *self, int var);
void BSDE_SetJointAttrF(BSDE_Joint *self, int var, double val);
int BSDE_GetJointAttrI(BSDE_Joint *self, int var);
void BSDE_SetJointAttrI(BSDE_Joint *self, int var, int val);
void BSDE_GetJointAttrFV(BSDE_Joint *self, int var, float *val, int n);
void BSDE_SetJointAttrFV(BSDE_Joint *self, int var, float *val, int n);
void BSDE_GetJointAttrDV(BSDE_Joint *self, int var, double *val, int n);
void BSDE_SetJointAttrDV(BSDE_Joint *self, int var, double *val, int n);
void BSDE_SetJointCubic(BSDE_Joint *self,double vx, double vy, double sx, double sy, double fa, double fb, double fc, double fd);
//source/bsde_world.cpp
BSDE_World *BSDE_GetWorld();
void BSDE_SetWorld(int id);
int BSDE_CreateWorld();
int BSDE_EmptyWorld(BSDE_World *world);
void BSDE_StepWorld(BSDE_World *world, double dt);
void BSDE_WorldForces(BSDE_World *world, double dt);
int bsde_finite(double val);
void BSDE_UpdateVel(BSDE_World *world, BSDE_Solid *cur, double dt);
void BSDE_Update(BSDE_World *world, BSDE_Solid *cur, double dt);
void BSDE_Predict(BSDE_World *world, BSDE_Solid *cur, double dt);
void BSDE_PredictPosTime(BSDE_Solid *cur, double dt,double *org, double *rot, double *xform);
void BSDE_SetWorldGravity(BSDE_World *world, double x, double y, double z);
void BSDE_GetWorldGravity(BSDE_World *world, double *v);
double BSDE_GetWorldAttrF(BSDE_World *world, int var);
void BSDE_SetWorldAttrF(BSDE_World *world, int var, double val);
int BSDE_GetWorldAttrI(BSDE_World *world, int var);
void BSDE_SetWorldAttrI(BSDE_World *world, int var, int val);
void BSDE_GetWorldAttrFV(BSDE_World *world, int var, float *val, int n);
void BSDE_SetWorldAttrFV(BSDE_World *world, int var, float *val, int n);
void BSDE_GetWorldAttrDV(BSDE_World *world, int var, double *val, int n);
void BSDE_SetWorldAttrDV(BSDE_World *world, int var, double *val, int n);
void *BSDE_GetBuffer(BSDE_World *world, int var);
void BSDE_SetBuffer(BSDE_World *world,int var, int type, int num, int stride, void *val);
int BSDE_AddSolidWorld(BSDE_World *world, BSDE_Solid *solid);
int BSDE_AddJointWorld(BSDE_World *world, BSDE_Joint *joint);
BSDE_World *BSDE_NewWorld();
int *BSDE_GetTriggerArray(BSDE_World *world);
int BSDE_GetTriggerArrayPos(BSDE_World *world);
void BSDE_ResetTriggerArray(BSDE_World *world);
void BSDE_SetTriggerArray(BSDE_World *world, int *arr, int num);
void BSDE_WorldAddTriggerContact(BSDE_World *world,BSDE_Solid *obja, BSDE_Solid *objb);
//source/bsde_math.cpp
void BSDE_CrossXForm(double *r, double *s);
void BSDE_BuildInertiaTensor(BSDE_Solid *obj);
void BSDE_BuildInertiaTensor(BSDE_Solid *obj);
void BSDE_BuildBoxInertiaTensor(BSDE_Solid *obj);
void BSDE_WorldInertiaTensor(BSDE_Solid *obj);
void BSDE_TransformPointLocal(BSDE_Solid *obj, double *org, double *pt);
void BSDE_TransformPointGlobal(BSDE_Solid *obj, double *org, double *pt);
void BSDE_TransformVectorLocal(BSDE_Solid *obj, double *org, double *pt);
void BSDE_TransformVectorGlobal(BSDE_Solid *obj, double *org, double *pt);
//source/bsde_force.cpp
void BSDE_CalcPointVelocity(double *org, double *vel, double *avel,double *point, double *pvel);
void BSDE_ApplyForce(BSDE_Solid *self, double *dir);
void BSDE_ApplyAcceleration(BSDE_Solid *self, double *dir);
void BSDE_GetVelocity(BSDE_Solid *self, double *vel);
void BSDE_GetPointAngleVelocity(BSDE_Solid *self, double *org, double *vel);
void BSDE_GetPointVelocity(BSDE_Solid *self, double *org, double *vel);
void BSDE_GetPointForce(BSDE_Solid *self, double *org, double *vel);
void BSDE_ApplyPointAcceleration(BSDE_Solid *self,double *org, double *vel);
void BSDE_ApplyPointForce(BSDE_Solid *self,double *org, double *dir);
void BSDE_ApplyPointAngleForce(BSDE_Solid *self,double *org, double *dir);
void BSDE_ApplyTorque(BSDE_Solid *self, double *dir);
void BSDE_HandleFluidContact(BSDE_World *world,BSDE_Contact *self, double dt);
void BSDE_ComputeImpulse(BSDE_Solid *obja,double *org, double *normal, double *force);
void BSDE_ComputeImpulse2(BSDE_Solid *obja, BSDE_Solid *objb,double *org, double *normal, double depth, double dt, double *fa, double *fb);
void BSDE_ComputeImpulse3(BSDE_Solid *obja, BSDE_Solid *objb,double *org, double *normal, double depth, double dt, double *fa, double *fb);
void BSDE_HandleContact(BSDE_World *world, BSDE_Contact *self, double dt);
//source/bsde_iter.cpp
double bsde_log2(double f);
void BSDE_SetupStack(BSDE_World *world);
void BSDE_PushCollisions(BSDE_World *world, int b);
void BSDE_PushInitial(BSDE_World *world);
void BSDE_PredictWorld(BSDE_World *world, double dt, int b);
void BSDE_UpdateWorld(BSDE_World *world, double dt, int b);
void BSDE_ConstrainWorld(BSDE_World *world, double dt);
void BSDE_CollideWorld(BSDE_World *world, double dt, int b);
void BSDE_BlockCollisionsWorld(BSDE_World *world, double dt, int b);
void BSDE_RecurseWorld(BSDE_World *world, double dt, int lb);
int BSDE_WorldSingleStep(BSDE_World *world, double dt);
void BSDE_WorldMultiStep(BSDE_World *world, double dt);
//source/bsde_iter2.cpp
void BSDE_TimeIter_PushInitial(BSDE_World *world);
void BSDE_TimeIter_FirstCollision(BSDE_World *world, double dt,BSDE_Solid **aobj, BSDE_Solid **bobj, double *rdt);
void BSDE_TimeIter_StepWorld(BSDE_World *world, double dt);
//source/bsde_iter3.cpp
BSDE_QuadNode *BSDE_Iter3_AllocQuadNode();
BSDE_BinaryNode *BSDE_Iter3_AllocBinaryNode();
int BSDE_Iter3_CalcCentroid(BSDE_Solid *first, double *org);
int BSDE_Iter3_CalcNormal(BSDE_Solid *first, double *norm);
int BSDE_Iter3_CollideList(BSDE_World *world,BSDE_Solid *obj, BSDE_Solid *first, int n);
int BSDE_Iter3_CheckCollideList(BSDE_World *world,BSDE_Solid *obj, BSDE_Solid *first, int m);
int BSDE_Iter3_SolidQuad(BSDE_Solid *solid, double *org);
BSDE_QuadNode *BSDE_Iter3_BuildNode(BSDE_Solid *first);
int BSDE_Iter3_CollideTree(BSDE_World *world, BSDE_QuadNode *node,BSDE_Solid *obj, int n);
BSDE_BinaryNode *BSDE_Iter3_BuildBiNode(BSDE_Solid *first);
int BSDE_Iter3_CollideBiTree(BSDE_World *world, BSDE_BinaryNode *node,BSDE_Solid *obj, int n);
int BSDE_Iter3_CollideBiObj(BSDE_World *world, BSDE_Solid *obj, int n);
int BSDE_Iter3_CheckCollideBiTree(BSDE_World *world,BSDE_BinaryNode *node, BSDE_Solid *obj, int m);
int BSDE_Iter3_CheckCollideBiObj(BSDE_World *world, BSDE_Solid *obj, int m);
int BSDE_WorldSingleStep2(BSDE_World *world, double dt);
//source/bsde_message.cpp
void BSDE_MsgEmitInt(int val);
void BSDE_MsgEmitScalar(double val);
void BSDE_MsgEmitVector(double x, double y, double z, double w);
void BSDE_MsgBegin(int to, int msg);
void BSDE_MsgEnd();
void BSDE_MsgAttrI(int var, int val);
void BSDE_MsgAttrF(int var, double val);
void BSDE_MsgAttrIV(int var, int *val, int num);
void BSDE_MsgAttrFV(int var, float *val, int num);
void BSDE_MsgAttrDV(int var, double *val, int num);
int BSDE_MsgFlushQueue(int id);
int BSDE_MsgPeek();
int BSDE_MsgNext();
int BSDE_MsgGetAttrTag(int id, int *ii, int *iv, int *is);
int BSDE_MsgGetNextAttr(int id, int *ii, int *iv, int *is);
int BSDE_MsgLookup(int var, int *rid, int *rii, int *riv, int *ris);
int BSDE_MsgGetAttrI(int var);
double BSDE_MsgGetAttrF(int var);
int BSDE_MsgGetAttrIV(int var, int *val, int n);
int BSDE_MsgGetAttrFV(int var, float *val, int n);
int BSDE_MsgGetAttrDV(int var, double *val, int n);
//source/bsde_thread.cpp
BSDE_ThreadState *BSDE_GetThreadState();
//source/bsde_build.cpp
void BSDE_BuildEmitInt(int val);
void BSDE_BuildEmitScalar(double val);
void BSDE_BuildEmitVector(double x, double y, double z, double w);
void BSDE_Begin(BSDE_World *wrl, int type);
int BSDE_End(BSDE_World *wrl);
//source/bsde_random.cpp
void BSDE_SRand(BSDE_World *wrl, bsdeUInt seed);
void BSDE_SRand2(BSDE_World *wrl, bsdeUInt seed1, bsdeUInt seed2);
bsdeUInt BSDE_Rand(BSDE_World *wrl);
bsdeUInt BSDE_Rand2(BSDE_World *wrl);
void BSDE_Seed2(BSDE_World *wrl, bsdeUInt s0, bsdeUInt s1);
void BSDE_Seed21(BSDE_World *wrl);
bsdeUInt BSDE_RandJ(BSDE_World *wrl);
double BSDE_RandF(BSDE_World *wrl);
double BSDE_RandG(BSDE_World *wrl);
//source/misc_vecheap.cpp
void BSDE_VecHeap_Init();
void BSDE_VecHeap_NewIBlock();
void BSDE_VecHeap_NewFBlock();
int BSDE_VecHeap_AllocICellBlk(int blk);
int BSDE_VecHeap_AllocFCellBlk(int blk);
int BSDE_VecHeap_AllocICellsBlk(int blk, int sz);
int BSDE_VecHeap_AllocFCellsBlk(int blk, int sz);
int *BSDE_VecHeap_TryAllocInt(int cnt);
float *BSDE_VecHeap_TryAllocFloat(int cnt);
int *BSDE_VecHeap_AllocInt(int cnt);
float *BSDE_VecHeap_AllocFloat(int cnt);
void BSDE_VecHeap_FreeICellsBlk(int blk, int idx);
void BSDE_VecHeap_FreeFCellsBlk(int blk, int idx);
void BSDE_VecHeap_Free(void *v);
//source/math_vec3.cpp
void BSDE_Vec3F_ProjectPointOnPlane(float *d, float *v, float *z);
void BSDE_Vec3F_ProjectPointOnPlane2(float *d, float *v, float *z, float zn);
void BSDE_Vec3D_ProjectPointOnPlane(double *d, double *v, double *z);
void BSDE_Vec3D_ProjectPointOnPlane2(double *d, double *v, double *z, double zn);
float BSDE_Vec3F_DistLine(float *pt, float *v0, float *v1);
float BSDE_Vec3F_DistLineSeg(float *pt, float *v0, float *v1);
double BSDE_Vec3D_DistLine(double *pt, double *v0, double *v1);
double BSDE_Vec3D_DistLineSeg(double *pt, double *v0, double *v1);
//source/math_matd.cpp
void BSDE_Mat3D_MatMult(double *a, double *b, double *c);
void BSDE_Mat3D_Transpose(double *a, double *b);
void BSDE_Mat3D_Copy(double *a, double *b);
int BSDE_Mat3D_Equal(double *a, double *b);
void BSDE_Mat3D2F_Copy(double *a, float *b);
void BSDE_Mat3F2D_Copy(float *a, double *b);
void BSDE_Mat3D_Scale(double *a, double b, double *c);
void BSDE_Mat3D_TransformVector(double *dir, double *xform, double *v);
void BSDE_Mat3D_TransformVectorT(double *dir, double *xform, double *v);
void BSDE_Mat3D_Normalize(double *rot);
void BSDE_Mat4D_Identity(double *b);
void BSDE_Mat4D_Copy(double *a, double *b);
int BSDE_Mat4D_Equal(double *a, double *b);
void BSDE_Mat4D_MatMult(double *a, double *b, double *c);
void BSDE_Mat4D_Transpose(double *a, double *b);
void BSDE_Mat4D_InverseTransform(double *a, double *b);
void BSDE_Mat34D_MatMult(double *a, double *b, double *c);
void BSDE_Mat34D_Transpose(double *a, double *b);
void BSDE_MatND_MatMult(double *a, int ah, int aw, double *b, int bh, int bw, double *c);
void BSDE_MatND_Inverse(double *a, double *b, int n);
//source/math_quatd.cpp
void BSDE_QuatD_Identity(double *a);
void BSDE_QuatD_Conjugate(double *a, double *b);
double BSDE_QuatD_Magnitude(double *a);
double BSDE_QuatD_Normalize(double *a, double *b);
void BSDE_QuatD_Multiply(double *a, double *b, double *c);
void BSDE_QuatD_ToMatrix(double *a, double *b);
void BSDE_QuatD_To3Matrix(double *a, double *b);
void BSDE_QuatD_FromAxis(double *a, double b, double *c);
void BSDE_QuatD_ToAxis(double *a, double *b, double *c);
void BSDE_QuatD_FromAngles(double *a, double *b);
void BSDE_QuatD_ToAngles(double *a, double *b);
void BSDE_QuatD_Lerp(double *a, double *b, double t, double *c);
void BSDE_QuatD_SLerp(double *a, double *ib, double t, double *c);
//source/math_planed.cpp
void BSDE_PlaneD_LinePlaneIntersect(double *start, double *end,double *norm, double *point);
int BSDE_PlaneD_CheckLinePlane(double *start, double *end, double *norm);
int BSDE_PlaneD_ClipLinePlane(double *start, double *end,double *norm, double *end2);
void BSDE_PlaneD_NearestPointLinePlane(double *start, double *end,double *norm, double *pt);
int BSDE_PlaneD_CheckPlaneBox(double *norm, double *mins, double *maxs);
void BSDE_PlaneD_TransformVector(double *dir, double *xform, double *v);
void BSDE_PlaneD_TransformVectorT(double *dir, double *xform, double *v);
void BSDE_PlaneD_TransformNormal(double *dir, double *xform, double *norm);
void BSDE_PlaneD_TransformNormalT(double *dir, double *xform, double *norm);
void BSDE_PlaneD_TransformNormalN(double *dir, double *xform, double *norm);
void BSDE_PlaneD_TransformNormalNT(double *dir, double *xform, double *norm);
void BSDE_PlaneD_TransformPoint(double *point, double *xform, double *org);
void BSDE_PlaneD_TransformPointT(double *point, double *xform, double *org);
void BSDE_PlaneD_TransformPointN(double *point, double *xform, double *org);
void BSDE_PlaneD_TransformPointNT(double *point, double *xform, double *org);
void BSDE_PlaneD_TransformPointLocal(double *point, double *xform, double *org);
void BSDE_PlaneD_AdjacentNormals(double *n, double *x, double *y);
void BSDE_PlaneD_AdjacentNormals2(double *norm, double *udir, double *vdir);
//source/math_hull.cpp
void BSDE_SolidHull_MakePlaneFace(double *norm, double *pts);
int BSDE_SolidHull_ClipFace(double *norm,double *ipts, double *opts, int num);
void BSDE_SolidHull_MakeHullGeom(double *norm, int nnorm,double *rvec, int *rfv, int *rfc, int *rnvec, int *rnfv, int *rnfc);
void BSDE_SolidHull_AlignPoints(double *vec, int nvec,double *org, double *vx, double *vy, double *vz);
void BSDE_SolidHull_FitBoxPoints(double *vec, int nvec,double *mins, double *maxs, double *pos);
int BSDE_SolidHull_HullFromPoints(double *vec, int nvec, double *nv);
int BSDE_SolidHull_HullFromPointsMask(double *vec, int base, int lim,bsdeUInt *mask, double *nv);
int BSDE_SolidHull_HullFiltPoints(double *vec, int nvec, double *nv);
int BSDE_SolidHull_HullFiltPointsR(double *vec, int cnt, double *nv);
int BSDE_SolidHull_HullFromPoints2(double *vec, int nvec, double *nv);
