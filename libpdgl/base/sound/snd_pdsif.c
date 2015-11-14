#include <pdgl.h>

void *pdgl_sound_playsound(PDSCR0_Context *ctx, void **args, int n)
{
	char *samp;
	float vol;

	samp=args[0];
	vol=NetVal_UnwrapFloat(args[1]);
	PDGL_Sound_PlaySample(samp, 0, vol);

	return(NULL);
}

void *pdgl_sound_playloop(PDSCR0_Context *ctx, void **args, int n)
{
	char *samp;
	float vol;

	samp=args[0];
	vol=NetVal_UnwrapFloat(args[1]);
	PDGL_Sound_PlaySample(samp, 1, vol);

	return(NULL);
}

void *pdgl_sound_playpointsound(PDSCR0_Context *ctx, void **args, int n)
{
	char *samp;
	float vol;
	float *org, *vel, scale;

	samp=args[0];
	vol=NetVal_UnwrapFloat(args[1]);
	org=args[2];
	vel=args[3];
	scale=NetVal_UnwrapFloat(args[4]);

	PDGL_Sound_PlayPointSample(samp, 0, vol, scale, org, vel);
	return(NULL);
}

void *pdgl_sound_playpointloop(PDSCR0_Context *ctx, void **args, int n)
{
	char *samp;
	float vol;
	float *org, *vel, scale;

	samp=args[0];
	vol=NetVal_UnwrapFloat(args[1]);
	org=args[2];
	vel=args[3];
	scale=NetVal_UnwrapFloat(args[4]);

	PDGL_Sound_PlayPointSample(samp, 1, vol, scale, org, vel);
	return(NULL);
}

int PDGL_Sound_InitScript()
{
	short *s;
	char *t;
	int rc;
	int i, j;

	PDSCR_Builtin_AddBuiltin("PlaySound",
		"(sample, volume), plays a sample",
		&pdgl_sound_playsound, 2);
	PDSCR_Builtin_AddBuiltin("PlayLoop",
		"(sample, volume), plays a sample in a loop",
		&pdgl_sound_playloop, 2);
	PDSCR_Builtin_AddBuiltin("PlayPointSound",
		"(sample, volume, origin, velocity, scale), plays a sample from a "
		"given point, with the distance scaled by scale",
		&pdgl_sound_playpointsound, 5);
	PDSCR_Builtin_AddBuiltin("PlayPointLoop",
		"(sample, volume, origin, velocity, scale), plays a looped sample "
		"from a given point, with the distance scaled by scale",
		&pdgl_sound_playpointloop, 5);

	return(0);
}
