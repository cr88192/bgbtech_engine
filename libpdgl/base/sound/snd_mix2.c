#include <pdgl.h>

extern PDGL_Sample *pdgl_sample_root;

extern short *pdgl_sound_mixbuf;
extern PDGL_MixChannel *pdgl_sound_channels;
extern float pdgl_sound_vol;
extern float pdgl_sound_camorigin[4];
extern float pdgl_sound_camorigin2[4];
extern float pdgl_sound_camvelocity[4];
extern float pdgl_sound_camrotation[9];
extern int pdgl_sound_lmixpos;
extern int pdgl_sound_mixpos;

extern short *pdgl_sound_samplebuf;	//temporary buffer for holding samples

float pdgl_sound_echodelay[16];
float pdgl_sound_echovol[16];
int pdgl_sound_echos=0;
float pdgl_sound_dampen=0;


float pdgl_fmin(float f, float g)
	{ return((f<=g)?f:g); }
float pdgl_fmax(float f, float g)
	{ return((f>=g)?f:g); }
float pdgl_fclamp(float x, float min, float max)
	{ return(pdgl_fmin(pdgl_fmax(x, min), max)); }

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


PDGL_API void PDGL_Sound_MixPopReduceStereo(short *buffer, int cnt)
{
	float f;
	int i, j, k, l;
	
	k=((int)buffer[-2]+(int)buffer[0])/2;
	l=((int)buffer[-1]+(int)buffer[1])/2;

	for(i=0; i<cnt; i++)
	{
		f=i/(float)cnt;
		buffer[i*2+0]=(buffer[i*2+0]*f)+(k*(1.0-f));
		buffer[i*2+1]=(buffer[i*2+1]*f)+(l*(1.0-f));
		buffer[-i*2+0]=(buffer[-i*2+0]*f)+(k*(1.0-f));
		buffer[-i*2+1]=(buffer[-i*2+1]*f)+(l*(1.0-f));
	}
}

PDGL_API void PDGL_Sound_MixPopReduceStereo2(
	short *buffer, int size, int offs, int cnt)
{
	if(offs<(2*cnt))return;
	if((offs+(2*cnt))>=size)return;
	PDGL_Sound_MixPopReduceStereo(buffer+offs, cnt);
}

int PDGL_Sound_MixSamples(int cnt, float dt)
{
	int i, j, k, l, l0, rcnt, rpos;
	PDGL_MixChannel *cur, *lst, *nxt;
	float vs, vsf, vt, vtf;			//volume, start/final
	float lvs, lvsf, lvt, lvtf;		//left volume
	float rvs, rvsf, rvt, rvtf;		//right volume
	float vsp, vtp, pan;			//left/right pan
	float vspfw, vtpfw;				//forward/back pan
	float vspup, vtpup;				//up/down pan
	float f, g;
	float pf[3], io[3];

//	kprint("Mixing %d samples\n", cnt);

	if(cnt<0)cnt=0;
	if(cnt>(1<<18))cnt=(1<<18);

	for(i=0; i<(cnt*2); i++)
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
		pan=0.5;

		if(cur->flags&PDGL_SOUND_POINT)
		{
			vs=0; vsf=0; vsp=0.5;
			vspfw=0.5; vspup=0.5;
			for(i=0; i<cur->num_points; i++)
				if(cur->points[(i*4)+3]>0)
			{
				V3F_SUB(cur->points+(i*4),
					pdgl_sound_camorigin, io);
				vt=V3F_NORMALIZE(io, io);
				vtp=(1.0+V3F_DOT(io, pdgl_sound_camrotation+0))*0.5;
				vtpfw=(1.0+V3F_DOT(io, pdgl_sound_camrotation+3))*0.5;
				vtpup=(1.0+V3F_DOT(io, pdgl_sound_camrotation+6))*0.5;
			
//				vt=VecNF_Distance(cur->points+(i*4),
//					pdgl_sound_camorigin, 3);
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
					vsp=vtp;
					vspfw=vtpfw;
					vspup=vtpup;
				}
			}
		}

//		rcnt=cnt;

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


		//calculate relative left/right volumes from pan
		pan=vsp;
//		lvt=0.0+(1.0-pan);
//		rvt=0.0+(0.0+pan);
		lvt=1.0+(1.0-pan);
		rvt=1.0+(0.0+pan);
//		lvt=3.0+(1.0-pan);
//		rvt=3.0+(0.0+pan);

		//and normalize
		f=sqrt(lvt*lvt+rvt*rvt);
		lvt/=f; rvt/=f;

		//apply front/back volume adjustment
		f=(2+vspfw)/3;
		lvt*=f; rvt*=f;

		//calculate left/right mix volumes
		lvs=vs*lvt; lvsf=vsf*lvt;
		rvs=vs*rvt; rvsf=vsf*rvt;


		for(i=0; i<rcnt; i++)pdgl_sound_samplebuf[i]=0;

		if(cur->getsamples)
		{
			i=cur->getsamples(pdgl_sound_samplebuf,
				rcnt, cur->data);
			if(i<0)cur->flags|=PDGL_SOUND_UNLINK;

			if(i<cnt)printf("Sound_MixSamples: Sample drain on "
				"stream '%s'\n", cur->name);
		}

		if(cur->samp && cur->samp->buf)
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
					{
//						f=pdgl_fmin(i/16.0, (l-i)/16.0);
//						f=pdgl_fmax(f, 1.0);
						pdgl_sound_samplebuf[j+i]=
							cur->samp->buf[cur->offs+i];
					}
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
				l0=l;
				if((rcnt-j)<l)l=rcnt-j;
				if(l<0)l=0;

				for(i=0; i<l; i++)
				{
//					f=pdgl_fmin(i/4.0, (l-i)/4.0);
					f=pdgl_fmin((cur->offs+i)/16.0, (l0-i)/16.0);
//					f=(l0-i)/16.0;
//					f=pdgl_fmax(f, 1.0);
					f=pdgl_fmin(f, 1.0);
//					f=pdgl_fclamp(f, 0.0, 1.0);
					pdgl_sound_samplebuf[j+i]=
						cur->samp->buf[cur->offs+i]*f;
				}
				cur->offs+=l;

				if(cur->offs>=cur->len)
					cur->flags|=PDGL_SOUND_UNLINK;
			}
		}else if(cur->samp)
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
					{
//						f=pdgl_fmin(i/16.0, (l-i)/16.0);
//						f=pdgl_fmax(f, 1.0);
						pdgl_sound_samplebuf[j+i]=
							PDGL_BTAC_GetSamplePoint(cur->samp, cur->offs+i);
					}
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
				l0=l;
				if((rcnt-j)<l)l=rcnt-j;
				if(l<0)l=0;

				for(i=0; i<l; i++)
				{
//					f=pdgl_fmin(i/4.0, (l-i)/4.0);
					f=pdgl_fmin((cur->offs+i)/16.0, (l0-i)/16.0);
//					f=(l0-i)/16.0;
//					f=pdgl_fmax(f, 1.0);
					f=pdgl_fmin(f, 1.0);
//					f=pdgl_fclamp(f, 0.0, 1.0);
					pdgl_sound_samplebuf[j+i]=
						PDGL_BTAC_GetSamplePoint(cur->samp, cur->offs+i)*f;
				}
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

		if(cur->flags&PDGL_SOUND_CALCCURDCT)
		{
			if(!cur->curdct)
			{
				cur->curdct=gcatomic(256*sizeof(int));
			}
			
			PDGL_Sound_WaveCalcDCT(cur->curdct, 64,
				pdgl_sound_samplebuf, cnt/4, 4);
		}

		lvt=(lvsf-lvs)/cnt;
		rvt=(rvsf-rvs)/cnt;
		for(i=0; i<cnt; i++)
		{
			k=pdgl_sound_mixpos+i*2+0;
			j=pdgl_sound_samplebuf[i]*(lvs+lvt*i);
			j=pdgl_sound_mixbuf[k]+j;
			if(j<-32767)j=-32767;
			if(j>32767)j=32767;
			pdgl_sound_mixbuf[k]=j;

			k=pdgl_sound_mixpos+i*2+1;
			j=pdgl_sound_samplebuf[i]*(rvs+rvt*i);
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

#if 1
	if(pdgl_sound_mixpos>4)
	{
		j=pdgl_sound_mixpos;
		k=(pdgl_sound_mixbuf[j-2]+
			pdgl_sound_mixbuf[j+0])/2;
		l=(pdgl_sound_mixbuf[j-1]+
			pdgl_sound_mixbuf[j+1])/2;

//		k=pdgl_sound_mixbuf[j-2];
//		l=pdgl_sound_mixbuf[j-1];

		l0=16;
		for(i=(-l0+1); i<l0; i++)
		{
			j=pdgl_sound_mixpos+i*2;
			f=fabs(i/(float)l0);
			pdgl_sound_mixbuf[j+0]=(pdgl_sound_mixbuf[j+0]*f)+(k*(1.0-f));
			pdgl_sound_mixbuf[j+1]=(pdgl_sound_mixbuf[j+1]*f)+(l*(1.0-f));
		}
	}
#endif

	return(0);
}
