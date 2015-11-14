#ifndef BTGE_SHARED_H
#define BTGE_SHARED_H


#ifndef TRUE
#define TRUE 1
#endif

#ifndef FALSE
#define FALSE 0
#endif

#define BT_IMPULSE_0			0x00000001
#define BT_IMPULSE_1			0x00000002
#define BT_IMPULSE_2			0x00000004
#define BT_IMPULSE_3			0x00000008
#define BT_IMPULSE_4			0x00000010
#define BT_IMPULSE_5			0x00000020
#define BT_IMPULSE_6			0x00000040
#define BT_IMPULSE_7			0x00000080
#define BT_IMPULSE_8			0x00000100
#define BT_IMPULSE_9			0x00000200

#define BT_IMPULSE_FIRE			0x00001000
#define BT_IMPULSE_ALTFIRE		0x00002000
#define BT_IMPULSE_USE			0x00004000
#define BT_IMPULSE_ALTFIRE2		0x00008000
#define BT_IMPULSE_NEXTWEAP		0x00010000
#define BT_IMPULSE_PREVWEAP		0x00020000

#define BT_CLFL_INVEN_OPEN		0x00000001		//inventory is open


//RGB colors, these are 24-bits and packed into an int
#define BT_RGB_BLACK			0x000000	//pure black
#define BT_RGB_RED_DARK			0x00003F	//
#define BT_RGB_GREEN_DARK		0x003F00	//
#define BT_RGB_BLUE_DARK		0x3F0000	//
#define BT_RGB_RED_MID			0x00007F	//
#define BT_RGB_GREEN_MID		0x007F00	//
#define BT_RGB_BLUE_MID			0x7F0000	//
#define BT_RGB_RED				0x0000FF	//
#define BT_RGB_GREEN			0x00FF00	//
#define BT_RGB_BLUE				0xFF0000	//
#define BT_RGB_WHITE			0xFFFFFF	//

#define BT_RGB_YELLOW			0x00FFFF	//
#define BT_RGB_VIOLET			0xFF00FF	//
#define BT_RGB_CYAN				0xFFFF00	//


#define BT_CHAN_AUTO				0	//plays a sound, no override
#define BT_CHAN_WEAPON				1	//
#define BT_CHAN_VOICE				2	//chan 1-7 will override a
#define BT_CHAN_ITEM				3	//previously playing sound
#define BT_CHAN_BODY				4	//
//channel flags
#define BT_CHAN_EVERYONE			8	//sent to everyone
#define BT_CHAN_RELIABLE			16	//do not drop
#define BT_CHAN_AMBIENT				32	//ambient sound, will play in a loop

#define BT_CHAN_AMB_VOICE			34	//ambient sound, ambient voice sound

#define BT_ATTN_NONE				0	//full volume everywhere
#define BT_ATTN_NORM				1	//default distance-based drop off
#define BT_ATTN_IDLE				2	//idle sound
#define BT_ATTN_STATIC				3	//rapid drop off with distance
#define BT_ATTN_LOW					4	//gradual drop off

//generic rotating effects
#define BT_EFF_ROTATE				0x00000001	//rotating
#define BT_EFF_NOSHADOW				0x00000002	//no shadows for entity

#define BT_EFF_BEAM					0x00000004	//beam effects

#define BT_EFF_BEAM_LASER			BT_EFF_BEAM		//laser beam

#define BT_EFF_BLOODTRAIL			0x00000010	//blood trail (gibs)
#define BT_EFF_SMOKETRAIL			0x00000020	//smoke trail (rockets/grenades)
#define BT_EFF_FIRETRAIL			0x00000030	//fire trail (fireballs)
#define BT_EFF_SPARKSTRAIL			0x00000040	//sparks trail (blaster)
#define BT_EFF_TRAIL_MASK			0x000000F0	//trail-type mask

//these bits control dynamic-light properties of the entity
#define BT_EFF_GLOW_RED				0x00000100	//glows red
#define BT_EFF_GLOW_GREEN			0x00000200	//glows green
#define BT_EFF_GLOW_BLUE			0x00000400	//glows blue
#define BT_EFF_GLOW_HIGH			0x00000800	//glows high
#define BT_EFF_GLOW_BRIGHT0			0x00001000	//glow brightness 0
#define BT_EFF_GLOW_BRIGHT1			0x00002000	//glow brightness 1
#define BT_EFF_GLOW_BRIGHT2			0x00004000	//glow brightness 2
#define BT_EFF_GLOW_BRIGHT3			0x00008000	//glow brightness 3
#define BT_EFF_GLOW_PULSE0			0x00010000	//glow pulse 0
#define BT_EFF_GLOW_PULSE1			0x00020000	//glow pulse 1
#define BT_EFF_GLOW_PULSE2			0x00040000	//glow pulse 2
#define BT_EFF_GLOW_PULSE3			0x00080000	//glow pulse 3
#define BT_EFF_GLOW_MASK			0x000FFF00	//glow effect mask

#define BT_EFF_GLOW_YELLOW			0x00000300	//glows yellow
#define BT_EFF_GLOW_VIOLET			0x00000500	//glows violet
#define BT_EFF_GLOW_CYAN			0x00000600	//glows cyan
#define BT_EFF_GLOW_WHITE			0x00000700	//glows white


#define BT_EFF_ALPHA_BIT0			0x00100000	//25% alpha
#define BT_EFF_ALPHA_BIT1			0x00200000	//50% alpha
#define BT_EFF_ALPHA_BIT2			0x00400000	//50% alpha
#define BT_EFF_ALPHA_BIT3			0x00800000	//50% alpha

#define BT_EFF_ALPHA_25				0x00400000	//25% alpha
#define BT_EFF_ALPHA_50				0x00800000	//50% alpha
#define BT_EFF_ALPHA_75				0x00C00000	//75% alpha
#define BT_EFF_ALPHA_MASK			0x00F00000	//alpha effect mask
#define BT_EFF_ALPHA_SHIFT			20			//alpha effect shift

#define BT_EF2_VOLSTAT				0x00000001	//track volume status
#define BT_EF2_ISDEAD				0x00000002	//entity is dead
#define BT_EF2_CAM_2D				0x00000004	//use a 2D camera view
#define BT_EF2_CAM_FIXED_ORG		0x00000008	//camera view is fixed
#define BT_EF2_CAM_FIXED_ROT		0x00000010	//camera view is fixed
#define BT_EF2_CAM_FORCEANGLE		0x00000020	//camera uses update angle
#define BT_EF2_NOLERP				0x00000040	//do not LERP position
#define BT_EF2_VIEWMODEL			0x00000080	//model is a viewmodel

//scale (x/8)^2+1
// #define BT_EF2_SCALE_SIGN		0x00000040	//scale sign (scale smaller)
#define BT_EF2_SCALE_BIT0			0x00000100	//scale bit 0
#define BT_EF2_SCALE_BIT1			0x00000200	//scale bit 1
#define BT_EF2_SCALE_BIT2			0x00000400	//scale bit 2
#define BT_EF2_SCALE_BIT3			0x00000800	//scale bit 3
#define BT_EF2_SCALE_BIT4			0x00001000	//scale bit 4
#define BT_EF2_SCALE_BIT5			0x00002000	//scale bit 5
#define BT_EF2_SCALE_BIT6			0x00004000	//scale bit 6
#define BT_EF2_SCALE_BIT7			0x00008000	//scale bit 7
#define BT_EF2_SCALE_MASK			0x0000FF00	//scale mask
#define BT_EF2_SCALE_LSHIFT			8			//fluid-type bit 0

#define BT_EF2_SCENERY				0x00010000	//object is scenery
#define BT_EF2_CAM_ALTORG			0x00020000	//cam alternate origin
#define BT_EF2_CAM_ALTROT			0x00040000	//cam alternate rotation
#define BT_EF2_CAM_CREATIVE			0x00080000	//we are in editing mode

#define BT_EF2_CAM_GUIMODE			0x00100000	//client is in GUI mode
#define BT_EF2_CAM_CLIENTMOVE		0x00200000	//client-side movement
#define BT_EF2_CAM_HFLIP			0x00400000	//view flip horizontal
#define BT_EF2_CAM_VFLIP			0x00800000	//view flip vertical
#define BT_EF2_CAM_WEAPONFLIP		0x01000000	//flip weapon


#endif
