/*
internal and external constants.

Tag Space:
0x00000000..0x0000FFFF: reserved for tags
	0x0000..0x00FF: context-specific
	0x0100..0x0FFF: general properties
	0x1000..0x3FFF: user properties (constant)
	0x4000..0x7FFF: dynamic properties (allocated)
	0x8000..0xFFFF: reserved

*/

#ifndef BSDE_CONST_H
#define BSDE_CONST_H

//0..255: reserved for small context-specific integers
#define BSDE_NULL			0x0000
#define BSDE_FALSE			0x0000
#define BSDE_TRUE			0x0001
#define BSDE_ZERO			0x0000
#define BSDE_ONE			0x0001
#define BSDE_TWO			0x0002
#define BSDE_THREE			0x0003
#define BSDE_FOUR			0x0004

#define BSDE_0				0x0000
#define BSDE_1				0x0001
#define BSDE_2				0x0002
#define BSDE_3				0x0003
#define BSDE_4				0x0004
#define BSDE_5				0x0005
#define BSDE_6				0x0006
#define BSDE_7				0x0007

#define BSDE_CREATOR		0x0001
#define BSDE_ENGINE			0x0002
#define BSDE_ENGINE_VER		0x0003
#define BSDE_VERSION		0x0004
#define BSDE_EXTENSIONS		0x0005

#define BSDE_PARAM_0		0x0000
#define BSDE_PARAM_1		0x0001
#define BSDE_PARAM_2		0x0002
#define BSDE_PARAM_3		0x0003
#define BSDE_PARAM_4		0x0004
#define BSDE_PARAM_5		0x0005
#define BSDE_PARAM_6		0x0006
#define BSDE_PARAM_7		0x0007


//vector properties
#define BSDE_ORIGIN				0x0100
#define BSDE_VELOCITY			0x0101
#define BSDE_TORQUE				0x0102
#define BSDE_ANGULAR_VELOCITY	0x0103
#define BSDE_ROTATION			0x0104	//rotation quaternion
#define BSDE_MINS				0x0105
#define BSDE_MAXS				0x0106
#define BSDE_MASS_OFFSET		0x0107
#define BSDE_INERTIA_TENSOR		0x0108
#define BSDE_DIRECTION			0x0109
#define BSDE_ORIGIN_A			0x010A	//origin as per A
#define BSDE_ORIGIN_B			0x010B	//origin as per B
#define BSDE_ORIGIN_RA			0x010C	//local origin as per A
#define BSDE_ORIGIN_RB			0x010D	//local origin as per B
#define BSDE_DIRECTION_A		0x010E	//direction on A
#define BSDE_DIRECTION_B		0x010F	//direction on B
#define BSDE_DIRECTION_RA		0x0110	//direction in A's local space
#define BSDE_DIRECTION_RB		0x0111	//direction in B's local space
#define BSDE_ROTATION_MATRIX	0x0112	//3x3 rotation matrix
#define BSDE_NORMAL				0x0113
#define BSDE_VERTICES			0x0114	//solid vertices
#define BSDE_CONTACT_VELOCITY	0x0115	//velocity at contacts
#define BSDE_ACCELERATION		0x0116	//linear acceleration
#define BSDE_LOCAL_THRUST		0x0117
#define BSDE_ORIGIN_0			0x0100	//base origin
#define BSDE_ORIGIN_1			0x0118	//2nd origin (joints)
#define BSDE_ORIGIN_2			0x0119	//3rd origin (joints)
#define BSDE_ORIGIN_3			0x011A	//4th origin (joints)
#define BSDE_DIRECTION_0		0x0109	//first direction
#define BSDE_DIRECTION_1		0x011B	//second direction
#define BSDE_ORIGIN_123			0x011C	//set base and generate others

#define BSDE_WANTED_ORIGIN		0x011D	//object "wants" this origin
#define BSDE_WANTED_VELOCITY	0x011E	//object "wants" this velocity
#define BSDE_WANTED_ROTATION	0x011F	//object "wants" this rotation

//bsde_real properties
#define BSDE_HEIGHT				0x0120
#define BSDE_RADIUS				0x0121
#define BSDE_MASS				0x0122
#define BSDE_VOLUME				0x0123
#define BSDE_DENSITY			0x0124
#define BSDE_ELASTIC			0x0125
#define BSDE_FRICTION			0x0126
#define BSDE_VISCOSITY			0x0127
#define BSDE_CONTACT_POWER		0x0128
#define BSDE_INERTIA_SCALAR		0x0129

#define BSDE_LENGTH				0x012A
#define BSDE_AREA				0x012B
#define BSDE_FORCE				0x012C
#define BSDE_POLARITY			0x012D
#define BSDE_PLASTIC			0x012E
#define BSDE_FRACTURE			0x012F

//integer/boolean properties
#define BSDE_SOLIDTYPE			0x0140
#define BSDE_MOVEFLAGS			0x0141
#define BSDE_NOSPIN				0x0142	//solid can't rotate
#define BSDE_NOMOVE				0x0143	//solid can't be moved
#define BSDE_NOGRAVITY			0x0144	//solid is not effected by gravity
#define BSDE_STATIC				0x0145	//solid isn't effected by physics
#define BSDE_SEMISTATIC			0x0146	//solid may move but is
					//otherwise uneffected by physics
#define BSDE_FLUID				0x0147	//solid is a fluid
#define BSDE_NONSOLID			0x0148	//solid is non-solid
#define BSDE_TRIGGER			0x0149	//solid is a trigger

#define BSDE_NUM_FACES			0x014A	//number of faces (hull/trimesh)
#define BSDE_NUM_VERTICES		0x014B	//number of vertices (hull/trimesh)

#define BSDE_ACTOR				0x014C	//solid is an actor
#define BSDE_MOVETYPE			0x014D	//move-type
					//use relevant physics tweaks

//integer vector properties
//0x0160..0x017F
#define BSDE_FACE_VERTICES		0x0160	//face vertex numbers
#define BSDE_FACE_COUNTS		0x0161	//vertex counts for each face

//world properties

#define BSDE_GRAVITY			0x0180	//world gravity
#define BSDE_MAXSTEP			0x0181
#define BSDE_MINSTEP			0x0182
#define BSDE_MAXFRAME			0x0183
#define BSDE_REALTIME			0x0184
#define BSDE_WORLDTIME			0x0185

#define BSDE_CONTACT_SELF		0x0186	//contact self array
#define BSDE_CONTACT_OTHER		0x0187	//contact other array
#define BSDE_CONTACT_ORIGIN		0x0188	//contact origin array
#define BSDE_CONTACT_NORMAL		0x0189	//contact normal array
#define BSDE_TRIGGER_ARRAY		0x018A	//trigger array

#define BSDE_COLLIDE_STRICT		0x018B	//do not allow interpenetration
#define BSDE_FRAME_PASSES		0x018C

#define BSDE_UNIT_LENGTH		0x018D
#define BSDE_UNIT_VOLUME		0x018E
#define BSDE_UNIT_MASS			0x018F
#define BSDE_UNIT_FORCE			0x0190


//solid types

#define BSDE_SOLID_OTHER		0x0210	//"other" solid type
#define BSDE_SOLID_AABB			0x0211
#define BSDE_SOLID_OBB			0x0212
#define BSDE_SOLID_SPHERE		0x0213
#define BSDE_SOLID_CYLINDER		0x0214	//flat topped cylinder
#define BSDE_SOLID_CCYLINDER	0x0215	//capped cylinder
#define BSDE_SOLID_CAPSULE		0x0215	//sphere capped cylinder
#define BSDE_SOLID_PLANE		0x0216	//normal and distance
#define BSDE_SOLID_TRIMESH		0x0217	//triangular mesh
#define BSDE_SOLID_HULL			0x0218	//convex hull
#define BSDE_SOLID_BSP			0x0219	//bsp tree
#define BSDE_SOLID_SOFTBODY		0x021A	//soft-body
#define BSDE_SOLID_VOXELS		0x021B	//voxel array


//joint types

#define BSDE_JOINT_OTHER		0x0240	//"other" joint type
#define BSDE_JOINT_BALL			0x0241	//keeps origins together
#define BSDE_JOINT_HINGE		0x0242	//works like a hinge
#define BSDE_JOINT_SLIDER		0x0243	//allows sliding along axis
#define BSDE_JOINT_UNIVERSAL	0x0244	//ball, keeps axis at 90 degrees
#define BSDE_JOINT_FIXED		0x0245	//fuses objects
#define BSDE_JOINT_ELASTIC		0x0246	//pulls objects


//aliases
#define BSDE_OTHER				0x0210	//"other" solid type
#define BSDE_AABB				0x0211
#define BSDE_OBB				0x0212
#define BSDE_SPHERE				0x0213
#define BSDE_CYLINDER			0x0214	//flat topped cylinder
#define BSDE_CCYLINDER			0x0215	//capped cylinder
#define BSDE_CAPSULE			0x0215	//sphere capped cylinder
#define BSDE_PLANE				0x0216	//normal and distance
#define BSDE_TRIMESH			0x0217	//triangular mesh
#define BSDE_HULL				0x0218	//convex hull
#define BSDE_BSP				0x0219	//bsp tree
#define BSDE_SOFTBODY			0x021A	//soft-body

#define BSDE_BALL				0x0241	//keeps origins together
#define BSDE_HINGE				0x0242	//works like a hinge
#define BSDE_SLIDER				0x0243	//allows sliding along axis
#define BSDE_UNIVERSAL			0x0244	//ball, keeps axis at 90 degrees
#define BSDE_FIXED				0x0245	//fuses objects
// #define BSDE_ELASTIC		0x0246	//pulls objects


#define BSDE_SPRING				0x0250	//
#define BSDE_TRIANGLE			0x0251	//
#define BSDE_TETRAHEDRON		0x0252	//
#define BSDE_HEXAHEDRON			0x0253	//


//joint properties
#define BSDE_STRENGTH			0x0260
#define BSDE_STRENGTH_0			0x0260
#define BSDE_STRENGTH_1			0x0261
#define BSDE_STRENGTH_2			0x0262
#define BSDE_STRENGTH_3			0x0263
#define BSDE_CONSTANT_ATTENUATION	0x0264
#define BSDE_LINEAR_ATTENUATION		0x0265
#define BSDE_QUADRATIC_ATTENUATION	0x0266
#define BSDE_CUBIC_ATTENUATION		0x0267



//data types

#define BSDE_BYTE				0x0280
#define BSDE_UNSIGNED_BYTE		0x0281
#define BSDE_SHORT				0x0282
#define BSDE_UNSIGNED_SHORT		0x0283
#define BSDE_INT				0x0284
#define BSDE_UNSIGNED_INT		0x0285
#define BSDE_FLOAT				0x0286
#define BSDE_DOUBLE				0x0287

#define BSDE_2_BYTES			0x0288
#define BSDE_3_BYTES			0x0289
#define BSDE_4_BYTES			0x028A
#define BSDE_2_SHORTS			0x028B
#define BSDE_3_SHORTS			0x028C
#define BSDE_4_SHORTS			0x028D
#define BSDE_2_INTS				0x028E
#define BSDE_3_INTS				0x028F
#define BSDE_4_INTS				0x0290

#define BSDE_2_FLOATS			0x0291
#define BSDE_V2F				0x0291
#define BSDE_3_FLOATS			0x0292
#define BSDE_V3F				0x0292
#define BSDE_4_FLOATS			0x0293
#define BSDE_V4F				0x0293
#define BSDE_2_DOUBLES			0x0294
#define BSDE_V2D				0x0294
#define BSDE_3_DOUBLES			0x0295
#define BSDE_V3D				0x0295
#define BSDE_4_DOUBLES			0x0296
#define BSDE_V4D				0x0296


//construction related
#define	BSDE_BSP_LEAF			0x02C0
#define	BSDE_BSP_NODE			0x02C1
#define	BSDE_BSP_FACE			0x02C2

#define	BSDE_LEAF				0x02C0
#define	BSDE_NODE				0x02C1
#define	BSDE_FACE				0x02C2


//basic units: 0x2E0..0x300

//length (l), area (l^2), volume (l^3)
#define BSDE_UNIT_METER			0x02E0
#define BSDE_UNIT_CENTIMETER	0x02E1
#define BSDE_UNIT_KILOMETER		0x02E2
#define BSDE_UNIT_FOOT			0x02E3
#define BSDE_UNIT_INCH			0x02E4	//standard inch
#define BSDE_UNIT_YARD			0x02E5
#define BSDE_UNIT_MILE			0x02E6

#define BSDE_UNIT_INCH_F8		0x02F0	//'inch' as 1/8 foot
#define BSDE_UNIT_INCH_F10		0x02F1	//'inch' as 1/10 foot
#define BSDE_UNIT_INCH_F16		0x02F2	//'inch' as 1/16 foot
#define BSDE_UNIT_INCH_M32		0x02F3	//'inch' as 1/32 meter

//mass
#define BSDE_UNIT_POUND_M		0x02E7	//metric lb, or 0.5kg
#define BSDE_UNIT_GRAM			0x02E8
#define BSDE_UNIT_KILOGRAM		0x02E9
#define BSDE_UNIT_MEGAGRAM		0x02EA
#define BSDE_UNIT_MILLIGRAM		0x02EB

//weight/force
#define BSDE_UNIT_POUND			0x02EC	//force, or 0.4525kg
#define BSDE_UNIT_NEWTON		0x02ED

//volume
#define BSDE_UNIT_GALLON		0x02EE
#define BSDE_UNIT_LITER			0x02EF

//aliases
#define BSDE_METER				0x02E0
#define BSDE_CENTIMETER			0x02E1
#define BSDE_KILOMETER			0x02E2
#define BSDE_FOOT				0x02E3
#define BSDE_INCH				0x02E4
#define BSDE_YARD				0x02E5
#define BSDE_MILE				0x02E6
#define BSDE_POUND_M			0x02E7	//metric lb, or 0.5kg
#define BSDE_GRAM				0x02E8
#define BSDE_KILOGRAM			0x02E9
#define BSDE_MEGAGRAM			0x02EA
#define BSDE_MILLIGRAM			0x02EB
#define BSDE_POUND				0x02EC	//force, or 0.4525kg
#define BSDE_NEWTON				0x02ED

#define BSDE_GALLON				0x02EE
#define BSDE_LITER				0x02EF


//attenuations

#define BSDE_LENGTH_CONSTANT_ATTN	0x0300
#define BSDE_LENGTH_LINEAR_ATTN		0x0301
#define BSDE_LENGTH_QUADRATIC_ATTN	0x0302
#define BSDE_LENGTH_CUBIC_ATTN		0x0303

#define BSDE_AREA_CONSTANT_ATTN		0x0304
#define BSDE_AREA_LINEAR_ATTN		0x0305
#define BSDE_AREA_QUADRATIC_ATTN	0x0306
#define BSDE_AREA_CUBIC_ATTN		0x0307

#define BSDE_VOLUME_CONSTANT_ATTN	0x0308
#define BSDE_VOLUME_LINEAR_ATTN		0x0309
#define BSDE_VOLUME_QUADRATIC_ATTN	0x030A
#define BSDE_VOLUME_CUBIC_ATTN		0x030B

#define BSDE_PLASTIC_CONSTANT_ATTN	0x030C
#define BSDE_PLASTIC_LINEAR_ATTN	0x030D
#define BSDE_PLASTIC_QUADRATIC_ATTN	0x030E
#define BSDE_PLASTIC_CUBIC_ATTN		0x030F


#define BSDE_LENGTH_A_CONSTANT_ATTN	0x0310
#define BSDE_LENGTH_A_LINEAR_ATTN	0x0311
#define BSDE_LENGTH_A_QUADRATIC_ATTN	0x0312
#define BSDE_LENGTH_A_CUBIC_ATTN	0x0313
#define BSDE_AREA_A_CONSTANT_ATTN	0x0314
#define BSDE_AREA_A_LINEAR_ATTN		0x0315
#define BSDE_AREA_A_QUADRATIC_ATTN	0x0316
#define BSDE_AREA_A_CUBIC_ATTN		0x0317
#define BSDE_VOLUME_A_CONSTANT_ATTN	0x0318
#define BSDE_VOLUME_A_LINEAR_ATTN	0x0319
#define BSDE_VOLUME_A_QUADRATIC_ATTN	0x031A
#define BSDE_VOLUME_A_CUBIC_ATTN	0x031B
#define BSDE_PLASTIC_A_CONSTANT_ATTN	0x031C
#define BSDE_PLASTIC_A_LINEAR_ATTN	0x031D
#define BSDE_PLASTIC_A_QUADRATIC_ATTN	0x031E
#define BSDE_PLASTIC_A_CUBIC_ATTN	0x031F

#define BSDE_LENGTH_B_CONSTANT_ATTN	0x0320
#define BSDE_LENGTH_B_LINEAR_ATTN	0x0321
#define BSDE_LENGTH_B_QUADRATIC_ATTN	0x0322
#define BSDE_LENGTH_B_CUBIC_ATTN	0x0323
#define BSDE_AREA_B_CONSTANT_ATTN	0x0324
#define BSDE_AREA_B_LINEAR_ATTN		0x0325
#define BSDE_AREA_B_QUADRATIC_ATTN	0x0326
#define BSDE_AREA_B_CUBIC_ATTN		0x0327
#define BSDE_VOLUME_B_CONSTANT_ATTN	0x0328
#define BSDE_VOLUME_B_LINEAR_ATTN	0x0329
#define BSDE_VOLUME_B_QUADRATIC_ATTN	0x032A
#define BSDE_VOLUME_B_CUBIC_ATTN	0x032B
#define BSDE_PLASTIC_B_CONSTANT_ATTN	0x032C
#define BSDE_PLASTIC_B_LINEAR_ATTN	0x032D
#define BSDE_PLASTIC_B_QUADRATIC_ATTN	0x032E
#define BSDE_PLASTIC_B_CUBIC_ATTN	0x032F

//Special Properties
#define BSDE_MMINS					0x0340
#define BSDE_MMAXS					0x0341
#define BSDE_RMINS					0x0342
#define BSDE_RMAXS					0x0343

#define BSDE_VOXEL_SIZE				0x0344	//voxel size (real)
#define BSDE_CHUNK_XS				0x0345	//chunk X size (int)
#define BSDE_CHUNK_YS				0x0346	//chunk Y size (int)
#define BSDE_CHUNK_ZS				0x0347	//chunk Z size (int)
#define BSDE_VOXEL_TYPE				0x0348	//voxel type (pointer)
#define BSDE_VOXEL_META				0x0349	//voxel meta (pointer)


//vertex slots 0x0C00..0x0CFF
#define BSDE_VERTEX0		0x0C00
#define BSDE_VERTEX1		0x0C01
#define BSDE_VERTEX2		0x0C02
#define BSDE_VERTEX3		0x0C03
#define BSDE_VERTEX4		0x0C04
#define BSDE_VERTEX5		0x0C05
#define BSDE_VERTEX6		0x0C06
#define BSDE_VERTEX7		0x0C07
#define BSDE_VERTEX8		0x0C08
#define BSDE_VERTEX9		0x0C09
#define BSDE_VERTEX10		0x0C0A
#define BSDE_VERTEX11		0x0C0B
#define BSDE_VERTEX12		0x0C0C
#define BSDE_VERTEX13		0x0C0D
#define BSDE_VERTEX14		0x0C0E
#define BSDE_VERTEX15		0x0C0F

//length slots 0x0D00..0x0DFF
#define BSDE_LENGTH0		0x0D00
#define BSDE_LENGTH1		0x0D01
#define BSDE_LENGTH2		0x0D02
#define BSDE_LENGTH3		0x0D03
#define BSDE_LENGTH4		0x0D04
#define BSDE_LENGTH5		0x0D05
#define BSDE_LENGTH6		0x0D06
#define BSDE_LENGTH7		0x0D07
#define BSDE_LENGTH8		0x0D08
#define BSDE_LENGTH9		0x0D09
#define BSDE_LENGTH10		0x0D0A
#define BSDE_LENGTH11		0x0D0B
#define BSDE_LENGTH12		0x0D0C
#define BSDE_LENGTH13		0x0D0D
#define BSDE_LENGTH14		0x0D0E
#define BSDE_LENGTH15		0x0D0F


//user-defined properties 0x1000-0x3FFF
#define BSDE_USER0		0x1000
#define BSDE_USER1		0x1001
#define BSDE_USER2		0x1002
#define BSDE_USER3		0x1003
#define BSDE_USER4		0x1004
#define BSDE_USER5		0x1005
#define BSDE_USER6		0x1006
#define BSDE_USER7		0x1007


#endif
