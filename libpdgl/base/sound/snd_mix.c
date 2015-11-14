#include <pdgl.h>

extern PDGL_Sample *pdgl_sample_root;

extern short *pdgl_sound_mixbuf;
extern PDGL_MixChannel *pdgl_sound_channels;
extern float pdgl_sound_vol;
extern float pdgl_sound_camorigin[4];
extern float pdgl_sound_camorigin2[4];
extern float pdgl_sound_camvelocity[4];
extern int pdgl_sound_lmixpos;
extern int pdgl_sound_mixpos;

extern short *pdgl_sound_samplebuf;	//temporary buffer for holding samples

float pdgl_sound_echodelay[16];
float pdgl_sound_echovol[16];
int pdgl_sound_echos=0;
float pdgl_sound_dampen=0;


int PDGL_Sound_SetDampen(float v)
{
	pdgl_sound_dampen=v;
}

int PDGL_Sound_ClearEcho()
{
	pdgl_sound_echos=0;
}

int PDGL_Sound_SetEcho(float delay, float vol)
{
	pdgl_sound_echodelay[0]=delay;
	pdgl_sound_echovol[0]=vol;
	pdgl_sound_echos=1;
	return(0);
}

int PDGL_Sound_SetEchos(float *delay, float *vol, int num)
{
	int i;

	if(num>16)num=16;
	for(i=0; i<num; i++)
	{
		pdgl_sound_echodelay[i]=delay[i];
		pdgl_sound_echovol[i]=vol[i];
	}
	pdgl_sound_echos=num;

	return(0);
}

int PDGL_Sound_CalcDoppler(
	float *lorg, float *lvel, float *sorg, float *svel, int cnt)
{
	float vl, vs, r, c;
	float nv[3];
	int i;

//	vl=Vl*N(Os-Ol)
//	vs=Vs*N(Ol-Os)

	V3F_SUB(sorg, lorg, nv);
	V3F_NORMALIZE(nv, nv);
	vl=V3F_DOT(lvel, nv);
	vs=V3F_DOT(svel, nv);

//	r=((c+vl)/(c-vs))
//	Fl=Fs*r
//	Fs=Fl/r

	c=PDGL_SOUND_C;
	r=((c+vl)/(c-vs));

//	Ll=Ls/r
//	Ls=Ll*r

	i=cnt*r;

	return(i);
}

int PDGL_Sound_CalcMoveOrg(
	float *iorg, float *ivel, float *forg, float dt)
{
	forg[0]=iorg[0]+ivel[0]*dt;
	forg[1]=iorg[1]+ivel[1]*dt;
	forg[2]=iorg[2]+ivel[2]*dt;

	return(0);
}

int PDGL_Sound_MixSamples(int cnt, float dt)
{
	int i, j, k, l, rcnt, rpos;
	PDGL_MixChannel *cur, *lst, *nxt;
	float vs, vsf, vt, vtf;
	float pf[3];

//	kprint("Mixing %d samples\n", cnt);

	if(cnt<0)cnt=0;
	if(cnt>(1<<19))cnt=(1<<19);

	for(i=0; i<cnt; i++)
		pdgl_sound_mixbuf[pdgl_sound_mixpos+i]=0;

	if(!pdgl_sound_samplebuf)
		pdgl_sound_samplebuf=malloc(1<<20);

	cur=pdgl_sound_channels;
	lst=NULL;
	while(cur)
	{
		rcnt=cnt;

		vs=1;	//sample volume at start
		vsf=1;	//sample volume at end

		if(cur->flags&PDGL_SOUND_POINT)
		{
			vs=0; vsf=0;
			for(i=0; i<cur->num_points; i++)
				if(cur->points[(i*4)+3]>0)
			{
				vt=VecNF_Distance(cur->points+(i*4),
					pdgl_sound_camorigin, 3);
				vt*=cur->points[(i*4)+3];
				if(vt<1)vt=1;
				vt=1/vt;

				PDGL_Sound_CalcMoveOrg(
					cur->points+(i*4), cur->vel+(i*4), pf, dt);
				vtf=VecNF_Distance(pf, pdgl_sound_camorigin2, 3);
				vtf*=cur->points[(i*4)+3];
				if(vtf<1)vtf=1;
				vtf=1/vtf;

				if(cur->flags&PDGL_SOUND_HIGHATTN)
					{ vt=vt*vt; vtf=vtf*vtf; }
				if(cur->flags&PDGL_SOUND_LOWATTN)
					{ vt=sqrt(vt); vtf=sqrt(vtf); }

				if(vt>vs)
				{
					rcnt=PDGL_Sound_CalcDoppler(
						pdgl_sound_camorigin,
						pdgl_sound_camvelocity,
						cur->points+(i*4),
						cur->vel+(i*4),
						cnt);

					vs=vt;
					vsf=vtf;
				}
			}
		}

		//if sound is being halted, make it fade out
		if(cur->flags&PDGL_SOUND_UNLINK)vsf=0;

//		if((rcnt<=0) || (rcnt>(1<<18)) || (vs<0.001))
		if((rcnt<=0) || (rcnt>(1<<18)))
		{
			lst=cur;
			cur=cur->next;
			continue;
		}

		vs*=cur->vol*pdgl_sound_vol;
		vsf*=cur->vol*pdgl_sound_vol;

		//ignore sufficiently quiet samples
		if((vs<0.001) && (vsf<0.001))
		{
			lst=cur;
			cur=cur->next;
			continue;
		}

		for(i=0; i<rcnt; i++)pdgl_sound_samplebuf[i]=0;

		if(cur->getsamples)
		{
			i=cur->getsamples(pdgl_sound_samplebuf,
				rcnt, cur->data);
			if(i<0)cur->flags|=PDGL_SOUND_UNLINK;

			if(i<cnt)printf("Sound_MixSamples: Sample drain on "
				"stream '%s'\n", cur->name);
		}

		if(cur->samp)
		{
			if(cur->flags&PDGL_SOUND_LOOP)
			{
				j=0;
				if(cur->delay>0)
				{
					j+=cur->delay;
					cur->delay-=rcnt;
				}

				while(j<rcnt)
				{
					l=cur->len-cur->offs;
					if((rcnt-j)<l)l=(rcnt-j);
					if(l<=0)break;
					for(i=0; i<l; i++)
						pdgl_sound_samplebuf[j+i]=
						cur->samp->buf[cur->offs+i];
					cur->offs+=l;
					if(cur->offs>=cur->len)cur->offs=0;
					j+=l;
				}
			}else
			{
				j=0;
				if(cur->delay>0)
				{
					j+=cur->delay;
					cur->delay-=rcnt;
				}

				l=cur->len-cur->offs;
				if((rcnt-j)<l)l=rcnt-j;
				if(l<0)l=0;

				for(i=0; i<l; i++)
					pdgl_sound_samplebuf[j+i]=
						cur->samp->buf[cur->offs+i];
				cur->offs+=l;

				if(cur->offs>=cur->len)
					cur->flags|=PDGL_SOUND_UNLINK;
			}
		}

		if(rcnt!=cnt)
		{
			PDGL_Sound_ScaleSample(
				pdgl_sound_samplebuf+rcnt, cnt,
				pdgl_sound_samplebuf, rcnt);
			for(i=0; i<cnt; i++)
				pdgl_sound_samplebuf[i]=
					pdgl_sound_samplebuf[i+rcnt];
		}

		//calculate the current volume of a sound effect
		if(cur->flags&PDGL_SOUND_CALCCURVOL)
		{
			vtf=0;
			for(i=0; i<cnt; i++)
			{
				j=pdgl_sound_samplebuf[i];
				if(j<0)j=-j;
				vtf+=j;
			}

			vt=(vs>vsf)?vs:vsf;
			cur->curvol=(vtf/cnt)*cur->vol;
			cur->currvol=(vtf/cnt)*vt;
		}

//		vs*=cur->vol*pdgl_sound_vol;
//		vsf*=cur->vol*pdgl_sound_vol;
		vt=(vsf-vs)/cnt;
		for(i=0; i<cnt; i++)
		{
//			j=pdgl_sound_mixbuf[i]+
//				(pdgl_sound_samplebuf[i]*cur->vol*
//				pdgl_sound_vol*vs);

			k=pdgl_sound_mixpos+i;
//			j=pdgl_sound_mixbuf[k]+
//				(pdgl_sound_samplebuf[i]*cur->vol*
//				pdgl_sound_vol*(vs+vt*i));

			j=pdgl_sound_samplebuf[i]*(vs+vt*i);
			j=pdgl_sound_mixbuf[k]+j;

			if(j<-32767)j=-32767;
			if(j>32767)j=32767;
			pdgl_sound_mixbuf[k]=j;
		}

		if(cur->flags&PDGL_SOUND_UNLINK)
		{
//			printf("Sound_MixSamples: Unlink %p %s\n",
//				cur, cur->name);
		
			if(lst)lst->next=cur->next;
				else pdgl_sound_channels=cur->next;
			nxt=cur->next;
			if(cur->points)gcfree(cur->points);
			if(cur->vel)gcfree(cur->vel);
			gcfree(cur);
			cur=nxt;
		}else
		{
			lst=cur;
			cur=cur->next;
		}
	}

	for(i=0; i<pdgl_sound_echos; i++)
	{
		rcnt=pdgl_sound_echodelay[i]*44100;
		for(j=0; j<cnt; j++)
		{
			rpos=j-rcnt;
			if(rpos<0)rpos+=pdgl_sound_lmixpos;
				else rpos+=pdgl_sound_mixpos;
			if(rpos<0)rpos=0;

			k=pdgl_sound_mixpos+j;
			l=pdgl_sound_mixbuf[k]+
				(pdgl_sound_mixbuf[rpos]*
					pdgl_sound_echovol[i]);
			if(l<-32767)l=-32767;
			if(l>32767)l=32767;
			pdgl_sound_mixbuf[k]=l;
		}
	}

	if(pdgl_sound_dampen>0)
	{
		k=pdgl_sound_mixpos+i;
		vs=pdgl_sound_mixbuf[k];

		for(i=1; i<cnt; i++)
		{
			k=pdgl_sound_mixpos+i;
			vs=(vs*pdgl_sound_dampen)+
				(pdgl_sound_mixbuf[k]*(1.0-pdgl_sound_dampen));

			j=vs;
			if(j<-32767)j=-32767;
			if(j>32767)j=32767;
			pdgl_sound_mixbuf[k]=j;
		}
	}

	return(0);
}
