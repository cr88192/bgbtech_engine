extern char **pdgl_argv;
extern int pdgl_argc;
extern int pdgl_xs, pdgl_ys;
extern int pdgl_time, pdgl_dt;			//total and delta time (ms)
extern unsigned short *pdgl_keybuf;		//keys for a given frame
extern int pdgl_mx, pdgl_my, pdgl_mb;		//mouse pos
extern int pdgl_lmx, pdgl_lmy, pdgl_lmb;	//last most pos
extern int pdgl_dmx, pdgl_dmy;			//mouse pos delta
extern int pdgl_omx, pdgl_omy;			//last non-clicking mouse pos
extern float pdgl_dt_f;				//delta-time (float seconds)
extern float pdgl_adt_f;			//average delta-time
extern float pdgl_adt2_f;			//average delta-time 2
extern float pdgl_time_f;			//average delta-time 2
extern int pdgl_maxhz;				//max framerate


extern int main_doshot;
extern int main_kill;

extern float cam_org[3];
extern float cam_ang[3];
extern float cam_rot[9];

extern float cam_speed;
extern float cam_width;
extern int cam_constrain;	//constrain orientation
extern int cam_ortho;

extern float cam_lorg[3];
extern float cam_lang[3];
extern float cam_lrot[9];

extern float cam_rt[3];
extern float cam_fw[3];
extern float cam_up[3];

extern float cam_clip_lf[4];
extern float cam_clip_rt[4];
extern float cam_clip_up[4];
extern float cam_clip_dn[4];
extern float cam_clip_bk[4];

extern float cam_morg[3];
extern float cam_mdir[3];
extern float cam_mend[3];

extern float cam_lmorg[3];
extern float cam_lmdir[3];
extern float cam_lmend[3];
