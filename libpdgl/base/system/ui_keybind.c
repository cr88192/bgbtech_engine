#include <pdgl.h>
#include <pdglui.h>

char *pdglui_keynames[]=
{
"NUL",			//0
"SOH",			//1
"STX",			//2
"ETX",			//3
"EOT",			//4
"ENQ",			//5
"ACK",			//6
"BEL",			//7
"BACKSPACE",	//8
"TAB",			//9
"LF",			//10
"VT",			//11
"FF",			//12
"ENTER",		//13
"SO",			//14
"SI",			//15
"DLE",			//16
"DC1",			//17
"DC2",			//18
"DC3",			//19
"DC4",			//20
"NAK",			//21
"SYN",			//22
"ETB",			//23
"CAN",			//24
"EM",			//25
"SUB",			//26
"ESCAPE",		//27
"FS",			//28
"GS",			//29
"RS",			//30
"US",			//31
"SPACE",		//32
"EXCLAMATION",	//33
"DOUBLEQUOTE",	//34
"POUND",		//35
"DOLLAR",		//36
"PERCENT",		//37
"AMPERSAND",	//38
"APOSTROPHE",	//39
"LEFTPAREN",	//40
"RIGHTPAREN",	//41
"ASTERISK",		//42
"PLUS",			//43
"COMMA",		//44
"MINUS",		//45
"PERIOD",		//46
"SLASH",		//47
"0",			//48
"1",			//49
"2",			//50
"3",			//51
"4",			//52
"5",			//53
"6",			//54
"7",			//55
"8",			//56
"9",			//57
"COLON",		//58
"SEMICOLON",	//59
"LESSTHAN",		//60
"EQUAL",		//61
"GREATERTHAN",	//62
"QUESTION",		//63
"AT",			//64
"A",			//65
"B",			//66
"C",			//67
"D",			//68
"E",			//69
"F",			//70
"G",			//71
"H",			//72
"I",			//73
"J",			//74
"K",			//75
"L",			//76
"M",			//77
"N",			//78
"O",			//79
"P",			//80
"Q",			//81
"R",			//82
"S",			//83
"T",			//84
"U",			//85
"V",			//86
"W",			//87
"X",			//88
"Y",			//89
"Z",			//90
"LEFTBRACKET",	//91
"BACKSLASH",	//92
"RIGHTBRACKET",	//93
"CARROT",		//94
"UNDERSCORE",	//95
"BACKQUOTE",	//96
"SMALL_A",		//97
"SMALL_B",		//98
"SMALL_C",		//99
"SMALL_D",		//100
"SMALL_E",		//101
"SMALL_F",		//102
"SMALL_G",		//103
"SMALL_H",		//104
"SMALL_I",		//105
"SMALL_J",		//106
"SMALL_K",		//107
"SMALL_L",		//108
"SMALL_M",		//109
"SMALL_N",		//110
"SMALL_O",		//111
"SMALL_P",		//112
"SMALL_Q",		//113
"SMALL_R",		//114
"SMALL_S",		//115
"SMALL_T",		//116
"SMALL_U",		//117
"SMALL_V",		//118
"SMALL_W",		//119
"SMALL_X",		//120
"SMALL_Y",		//121
"SMALL_Z",		//122
"LEFTBRACE",	//123
"PIPE",			//124
"RIGHTBRACE",	//125
"TILDE",		//126
"DELETE",		//127
"UPARROW",		//128
"DOWNARROW",	//129
"LEFTARROW",	//130
"RIGHTARROW",	//131
"ALT",			//132
"CTRL",			//133
"SHIFT",		//134
"F1",			//135
"F2",			//136
"F3",			//137
"F4",			//138
"F5",			//139
"F6",			//140
"F7",			//141
"F8",			//142
"F9",			//143
"F10",			//144
"F11",			//145
"F12",			//146
"INSERT",		//147
"",				//148
"PGDN",			//149
"PGUP",			//150
"HOME",			//151
"END",			//152
"PAUSE",		//153
"MWHEELUP",		//154
"MWHEELDOWN",	//155
"AUX",			//156
"MOUSE1",		//157
"MOUSE2",		//158
"MOUSE3",		//159
"",		//160
"",		//161
"",		//162
"",		//163
"",		//164
"",		//165
"",		//166
"",		//167
"",		//168
"",		//169
"",		//170
"",		//171
"",		//172
"",		//173
"",		//174
"",		//175
"",		//176
"",		//177
"",		//178
"",		//179
"",		//180
"",		//181
"",		//182
"",		//183
"",		//184
"",		//185
"",		//186
"",		//187
"",		//188
"",		//189
"",		//190
"",		//191
"FORWARD",		//192
"BACKWARD",		//193
"MOVELEFT",		//194
"MOVERIGHT",	//195
"MOVEUP",		//196
"MOVEDOWN",		//197
"USE",			//198
"ATTACK",		//199
"ATTACK2",		//200
"NEXTWEAPON",	//201
"PREVWEAPON",	//202
NULL
};

char *pdglui_key_binds[256];

PDGL_API int UI_LookupKey(char *name)
{
	int i;
	
	if(!name || !(*name))
		return(-1);
	
	for(i=0; pdglui_keynames[i]; i++)
		if(!stricmp(pdglui_keynames[i], name))
			return(i);
	return(-1);
}

PDGL_API char *UI_NameForKey(int key)
{
	if((key<0) || (key>=256))
		return(NULL);
	return(pdglui_keynames[key]);
}

PDGL_API char *UI_NameForKey2(int key)
{
	if((key>='a') && (key<='z'))
		return(UI_NameForKey(key-'a'+'A'));
	return(UI_NameForKey(key));
}

PDGL_API int UI_BindKey(char *name, char *cmd)
{
	int i;

	i=UI_LookupKey(name);
	if(name<0)return(-1);
	pdglui_key_binds[i]=dyllStrdup(cmd);
	return(i);
}

PDGL_API char *UI_BindingForKey(int key)
{
	if((key<0) || (key>=256))
		return(NULL);
	return(pdglui_key_binds[key]);
}

PDGL_API char *UI_BindingForKey2(int key)
{
	if((key>='a') && (key<='z'))
		return(UI_BindingForKey(key-'a'+'A'));
	return(UI_BindingForKey(key));
}
