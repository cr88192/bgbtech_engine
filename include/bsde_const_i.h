/*
internal constants
*/

#ifndef BSDE_CONSTI_H
#define BSDE_CONSTI_H

//#define BSDE_M_BIGVAL		999999999
//#define BSDE_M_SMALLVAL		0.000000001

//#define BSDE_M_DEF_ELASTIC	0.25
#define BSDE_M_DEF_FRICTION	0.50

#define BSDE_M_DEF_ELASTIC	0.1

#define BSDE_M_DEF_GRAM		0.000001
#define BSDE_M_DEF_KILOGRAM	0.001
#define BSDE_M_DEF_MEGAGRAM	1.0
#define BSDE_M_DEF_MILLIGRAM	0.000000001


#define BSDE_MOVEFL_NOSPIN	0x0001	//solid can't rotate
#define BSDE_MOVEFL_NOMOVE	0x0002	//solid can't be moved
#define BSDE_MOVEFL_NOGRAVITY	0x0004	//solid is not effected by gravity
#define BSDE_MOVEFL_STATIC	0x0008	//solid is static
#define BSDE_MOVEFL_SEMISTATIC	0x0010	//solid is semi-static
#define BSDE_MOVEFL_FLUID	0x0020	//solid is a fluid
#define BSDE_MOVEFL_NONSOLID	0x0040	//solid is not solid
#define BSDE_MOVEFL_TRIGGER	0x0080	//solid is a trigger
#define BSDE_MOVEFL_HIDDEN	0x0100	//solid is a hidden/ignored
#define BSDE_MOVEFL_BLOCKED	0x0200	//solid is a blocked
#define BSDE_MOVEFL_PREDICT	0x0400	//cleared when predicted
#define BSDE_MOVEFL_IDLE	0x0800	//object is idle (not recently moving)
#define BSDE_MOVEFL_ACTOR	0x1000	//object is an actor

#define BSDE_MOVEFL_WANTORG	0x2000	//object wants a specific origin
#define BSDE_MOVEFL_WANTVEL	0x4000	//object wants a specific velocity
#define BSDE_MOVEFL_WANTROT	0x8000	//object wants a specific rotation

#define BSDE_WORLDFL_STRICT	0x0001	//strict no-penetration

#define BSDE_STATEFL_BBOX	0x0001	//represent solid as a bbox
#define BSDE_STATEFL_MOVECACHE	0x0002	//movement is cached

//0x0800-0x0FFF, internal tags

#define BSDE_TAG_BEGIN		0x0800
#define BSDE_TAG_END		0x0801
#define BSDE_TAG_NORMAL		0x0802
#define BSDE_TAG_VERTEX		0x0803
#define BSDE_TAG_ATTR_I		0x0804
#define BSDE_TAG_ATTR_F		0x0805
#define BSDE_TAG_ATTR_IV	0x0806
#define BSDE_TAG_ATTR_FV	0x0807

#define BSDE_TAG_ATTR_2I	0x0808
#define BSDE_TAG_ATTR_3I	0x0809
#define BSDE_TAG_ATTR_4I	0x080A
#define BSDE_TAG_ATTR_2F	0x080B
#define BSDE_TAG_ATTR_3F	0x080C
#define BSDE_TAG_ATTR_4F	0x080D
#define BSDE_TAG_MSG		0x080E
#define BSDE_TAG_ENDMSG		0x080F

#endif
