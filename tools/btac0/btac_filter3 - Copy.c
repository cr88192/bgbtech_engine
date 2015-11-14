#include <bgbtac.h>

/*
Experimental block-based audio codec.
Encodes blocks of 64 samples into 256 bits (32 bytes).
At 44.1kHz this is 176kbps.
It can encode stereo using a "naive joint stereo" encoding.

Most block formats will encode a single center channel and will offset it for the left/right channel.

Basic Format 0:
	4 bit: Block-Mode (0)
    currently unused (12 bits, zeroed)
    16 bit min sample (center)
    16 bit max sample (center)
    8 bit left-center min (truncated)
    8 bit left-center max
    64 Samples, 1 bits/sample (64 bits)
    16x 4-bit min (64 bits)
    16x 4-bit max (64 bits)
 
The 4-bit values interpolate between the full min/max for the block.
The 1-bit samples select between the min and max value for each sample.
 
Note: Interpolated values are linear, thus 0=0/15, 1=1/15, 2=2/15, ..., 14=14/15, 15=15/15
 
Bit packing is in low-high order, and multibyte values are little-endian.

Basic Format 1:
	4 bit: Block-Mode (1)
	currently unused (12 bits, zeroed)
	16 bit min sample (center)
	16 bit max sample (center)
	8 bit left-center min (truncated)
	8 bit left-center max
	32x 2-bit sample (64 bits)
	32x 4-bit sample (128 bits)

This directly codes all samples, with the 4-bit values encoding even samples, and the 2-bit values encoding odd samples.

The 4-bit samples are encoded between the block min/max values, and the 2-bit samples between the prior/next sample.

Sample interpolation (2 bit samples):
0=prior sample, 1=next sample, 2=average, 3=quadratic interpolated value.

Basic Format 2:
	4 bit: Block-Mode (2)
	currently unused (12 bits, zeroed)
	16 bit min sample (center)
	16 bit max sample (center)
	8 bit left-center min (truncated)
	8 bit left-center max
	32x 6-bit samples (192 bits)

This directly codes samples, with the 6-bit values encoding samples.
The 6-bit samples are encoded between the block min/max values.

This mode encodes even samples, with odd-samples being interpolated.
The last sample is extrapolated.

Stereo Format 3:
	4 bit: Block-Mode (3)
	currently unused (12 bits, zeroed)
	16 bit min sample (center)
	16 bit max sample (center)
	8 bit left-center min (truncated)
	8 bit left-center max
	32x 2-bit pan (64 bits)
	32x 4-bit sample (128 bits)

This directly codes samples, with the 4-bit values encoding even samples.
The 2-bit pan value encodes the relative pan of the sample.

The 4-bit samples are encoded between the block min/max values.

The 2-bit samples represent values as:
0=center pan (offset):
	The sample will be offset for left/right channels.
1=center-pan (duplicate):
	The sample will be the same (center) value for both channels.
2=left-pan:
	The sample will be panned towards the left.
3=right pan:
	The sample will be panned towards the right.

This mode encodes even samples, with odd-samples being interpolated.

Basic Format 4:
	4 bit: Block-Mode (4)
	currently unused (12 bits, zeroed)
	16 bit min sample (center)
	16 bit max sample (center)
	8 bit left-center min (truncated)
	8 bit left-center max
	8x 4-bit min (32 bits)
	8x 4-bit max (32 bits)
	64x 2-bit sample (128 bits)

The 4-bit values interpolate between the full min/max for the block.
The 2-bit samples interpolate between the min and max values for each sub-block.

 */
 

void BGBTAC_EncodeBlock8MonoS16(s16 *iblk, byte *oblk)
{
	int tcma[16], tcna[16];
	int tcmb[16], tcnb[16];
	int lm, ln, cm, rm, rn, cn, tl, tr, tc;
	int cm2, cn2, ca2;
	int p0, p1, p2, p3;
	int s0, s1;
	int i, j, k;

	lm= 99999; rm= 99999; cm= 99999;
	ln=-99999; rn=-99999; cn=-99999;
	
	for(i=0; i<64; i++)
	{
		tl=iblk[i]; tr=tl;
		k=(tl+tr)>>1;
		if(k<cm) { lm=tl; rm=tr; cm=k; }
		if(k>cn) { ln=tl; rn=tr; cn=k; }
	}
	
	for(i=0; i<32; i++)
		{ oblk[i]=0; }
	
//	oblk[0]=cm&0xFF;
//	oblk[1]=(cm>>8)&0xFF;
//	oblk[2]=cn&0xFF;
//	oblk[3]=(cn>>8)&0xFF;
//	oblk[4]=((lm-rm+128)>>8)&0xFF;
//	oblk[5]=((ln-rn+128)>>8)&0xFF;

	oblk[2]=cm&0xFF;
	oblk[3]=(cm>>8)&0xFF;
	oblk[4]=cn&0xFF;
	oblk[5]=(cn>>8)&0xFF;
	oblk[6]=0;
	oblk[7]=0;
	
	for(i=0; i<16; i++)
	{
		cm2= 99999; cn2=-99999;
		for(j=0; j<4; j++)
		{
			tl=iblk[i*4+j]; tr=tl;
			tc=(tl+tr)>>1;
			if(tc<cm2) { cm2=tc; }
			if(tc>cn2) { cn2=tc; }
		}
		
		if(cn!=cm)
			{ s0=16777216/(cn-cm); }
		else { s0=0; }

		p0=(s0*(cm2-cm)+524288)>>20;
		p1=(s0*(cn2-cm)+524288)>>20;
		
		p0=(p0<0)?0:((p0<16)?p0:15);
		p1=(p1<0)?0:((p1<16)?p1:15);
		
		tcma[i]=p0;
		tcna[i]=p1;

		s1=(cn-cm);
		cm2=((s1*p0+8)>>4)+cm;
		cn2=((s1*p1+8)>>4)+cm;

		tcmb[i]=cm2;
		tcnb[i]=cn2;
	}

	for(i=0; i<16; i++)
	{
		oblk[16+(i>>1)]|=tcma[i]<<((i&1)*4);
		oblk[24+(i>>1)]|=tcna[i]<<((i&1)*4);
	}
	
	for(i=0; i<16; i++)
	{
		ca2=(tcmb[i]+tcnb[i])>>1;
	
		for(j=0; j<4; j++)
		{
			k=i*4+j;
		
			tl=iblk[k]; tr=tl;
//			tl=(iblk[k]+7)>>4; tr=tl;
			tc=(tl+tr)>>1;

			if(tc>=ca2)
				{ oblk[8+(k>>3)]|=1<<(k&7); }
			else
				{ oblk[8+(k>>3)]&=~(1<<(k&7)); }
		}
	}	
}

void BGBTAC_DecodeBlock8MonoS16(byte *iblk, s16 *oblk)
{
	int tcma[16], tcna[16];
	int tcmb[16], tcnb[16];
	int p0, p1, p2, p3;
	int cm, cn, ca;
	int cm2, cn2, ca2;
	int i, j, k;
	
//	cm=(s16)(iblk[0]+(iblk[1]<<8));
//	cn=(s16)(iblk[2]+(iblk[3]<<8));
	cm=(s16)(iblk[2]+(iblk[3]<<8));
	cn=(s16)(iblk[4]+(iblk[5]<<8));
	ca=(cm+cn)>>1;

	for(i=0; i<128; i++)
		{ oblk[i]=ca; }
		
	for(i=0; i<16; i++)
	{
		cm2=(iblk[16+(i>>1)]>>((i&1)*4))&15;
		cn2=(iblk[24+(i>>1)]>>((i&1)*4))&15;
//		cm2=tcmb[i];
//		cn2=tcnb[i];
	
		j=(cn-cm);
		p0=((j*cm2+8)>>4)+cm;
		p1=((j*cn2+8)>>4)+cm;

		p0=(p0<cm)?cm:((p0<cn)?p0:cn);
		p1=(p1<cm)?cm:((p1<cn)?p1:cn);
	
		tcma[i]=p0;
		tcna[i]=p1;
		
		p2=(p0+p1)>>1;
//		if(p2<cm)p2=cm;
//		if(p2>cn)p2=cn;
		
		for(j=0; j<4; j++)
		{
			k=i*4+j;
		
//			oblk[i*16+j]=p2;
			oblk[i*4+j]=(iblk[8+(k>>3)]&(1<<(k&7)))?p1:p0;
		}
	}
}

BTAC_API void BGBTAC_EncodeStream8MonoS16(s16 *iblk, byte *oblk, int len)
{
	int i, j, n;
	
	n=(len+63)/64;
	for(i=0; i<n; i++)
	{
		BGBTAC_EncodeBlock8MonoS16(iblk+i*64, oblk+i*32);
	}
}

BTAC_API void BGBTAC_DecodeStream8MonoS16(byte *iblk, s16 *oblk, int len)
{
	int i, j, n;
	
	n=(len+63)/64;
	for(i=0; i<n; i++)
	{
		BGBTAC_DecodeBlock8MonoS16(iblk+i*32, oblk+i*64);
	}
}

int bgbtac_ssqrti(int v)
{
	int i;
	double f;
	f=(v>=0)?sqrt(v):-sqrt(-v);
	i=(int)(f+0.5);
	return(i);
}

int bgbtac_ssqri(int v)
{
	return((v>=0)?(v*v):(-v*v));
}

int bgbtac_spowi(int v, float e)
{
	int i;
	double f;
	f=(v>=0)?pow(v, e):-pow(-v, e);
	i=(int)(f+0.5);
	return(i);
}

void BGBTAC_EncodeBlockHeaderStereoS16(
	s16 *iblk, byte *oblk, s16 *tcblk,
	int *rcm, int *rcn, int flags)
{
	int lm, ln, cm, rm, rn, cn, tl, tr, tc;
	int cm2, cn2, ca2, ba;
	int p0, p1, p2, p3;
	int s0, s1;
	int i, j, k;

#if 1
	lm= 99999; rm= 99999; cm= 99999;
	ln=-99999; rn=-99999; cn=-99999;
	
	ba=0;
	for(i=0; i<64; i++)
	{
		if(flags&BGBTAC_EFL_HALFMONO)
			{ tl=iblk[i*2+0]; tr=tl; }
		else if(flags&BGBTAC_EFL_STEREO)
			{ tl=iblk[i*2+0]; tr=iblk[i*2+1]; }
		else
			{ tl=iblk[i]; tr=tl; }
		k=(tl+tr)>>1;
		tcblk[i]=k;
		ba+=tl-k;
		if(k<cm) { lm=tl; rm=tr; cm=k; }
		if(k>cn) { ln=tl; rn=tr; cn=k; }
	}

	ba/=64;
	
	if(flags&BGBTAC_EFL_MIDSTREAM)
	{
		ba=0;
		for(i=-64; i<128; i++)
		{
			if(flags&BGBTAC_EFL_HALFMONO)
				{ tl=iblk[i*2+0]; tr=tl; }
			else if(flags&BGBTAC_EFL_STEREO)
				{ tl=iblk[i*2+0]; tr=iblk[i*2+1]; }
			else
				{ tl=iblk[i]; tr=tl; }
			k=(tl+tr)>>1;
			ba+=tl-k;
		}
		ba/=192;
	}
	
	for(i=0; i<32; i++)
		{ oblk[i]=0; }

	p0=lm-cm;
	p1=ln-cn;
	p2=ba;

	p0=(5*p0+11*p2)>>4;
	p1=(5*p1+11*p2)>>4;
//	p0=p2; p1=p2;

	if(flags&(BGBTAC_EFL_CENTERMONO|BGBTAC_EFL_HALFMONO))
		{ p0=0; p1=0; }

//	p0/=2;	p1/=2;
	p0=bgbtac_spowi(p0, 0.73);
	p1=bgbtac_spowi(p1, 0.73);

//	p0=bgbtac_spowi(p0, 0.90);
//	p1=bgbtac_spowi(p1, 0.90);

//	p0=bgbtac_spowi(p0, 0.80);
//	p1=bgbtac_spowi(p1, 0.80);

	p0=bgbtac_ssqrti(p0);
	p1=bgbtac_ssqrti(p1);

	p0=(p0<(-128))?(-128):((p0<128)?p0:127);
	p1=(p1<(-128))?(-128):((p1<128)?p1:127);
	
	oblk[0]=0x00;
	oblk[1]=0x00;

	oblk[2]=cm&0xFF;
	oblk[3]=(cm>>8)&0xFF;
	oblk[4]=cn&0xFF;
	oblk[5]=(cn>>8)&0xFF;
	oblk[6]=p0&0xFF;
	oblk[7]=p1&0xFF;
#endif

	*rcm=cm;
	*rcn=cn;
}

void BGBTAC_EncodeBlockHeaderPanStereoS16(
	s16 *iblk, byte *oblk,
	s16 *tcblk, s16 *tcpan,
	int *rcm, int *rcn,
	int *rlm, int *rln,
	int *rrm, int *rrn,
	int flags)
{
	int lm, ln, cm, rm, rn, cn, tl, tr, tc;
	int cm2, cn2, ca2, ba;
	int p0, p1, p2, p3;
	int s0, s1;
	int i, j, k;

#if 1
	lm= 99999; rm= 99999; cm= 99999;
	ln=-99999; rn=-99999; cn=-99999;
	
	ba=0;
	for(i=0; i<64; i++)
	{
		if(flags&BGBTAC_EFL_HALFMONO)
		{
			tl=iblk[i*2+0];
			tr=tl;
		}else if(flags&BGBTAC_EFL_STEREO)
		{
			tl=iblk[i*2+0];
			tr=iblk[i*2+1];
		}else
		{
			tl=iblk[i];
			tr=tl;
		}
		k=(tl+tr)>>1;
		tcblk[i]=k;
		tcpan[i]=tl-k;
		ba+=tl-k;
		if(k<cm) { lm=tl; rm=tr; cm=k; }
		if(k>cn) { ln=tl; rn=tr; cn=k; }
	}

	ba/=64;
	
	for(i=0; i<32; i++)
		{ oblk[i]=0; }

	p0=lm-cm;
	p1=ln-cn;
	p2=ba;

	p0=(5*p0+11*p2)>>4;
	p1=(5*p1+11*p2)>>4;
//	p0=p2; p1=p2;

	if((flags&BGBTAC_EFL_CENTERMONO) || (flags&BGBTAC_EFL_HALFMONO))
		{ p0=0; p1=0; }

//	p0/=2;	p1/=2;
	p0=bgbtac_spowi(p0, 0.73);
	p1=bgbtac_spowi(p1, 0.73);

//	p0=bgbtac_spowi(p0, 0.90);
//	p1=bgbtac_spowi(p1, 0.90);

//	p0=bgbtac_spowi(p0, 0.80);
//	p1=bgbtac_spowi(p1, 0.80);

	p0=bgbtac_ssqrti(p0);
	p1=bgbtac_ssqrti(p1);

	p0=(p0<(-128))?(-128):((p0<128)?p0:127);
	p1=(p1<(-128))?(-128):((p1<128)?p1:127);
	
	oblk[0]=0x00;
	oblk[1]=0x00;

	oblk[2]=cm&0xFF;
	oblk[3]=(cm>>8)&0xFF;
	oblk[4]=cn&0xFF;
	oblk[5]=(cn>>8)&0xFF;
	oblk[6]=p0&0xFF;
	oblk[7]=p1&0xFF;
#endif

	*rcm=cm;
	*rcn=cn;
	if(rlm && rln) { *rlm=lm; *rln=ln; }
	if(rrm && rrn) { *rrm=rm; *rrn=rn; }
}

#if 1
void BGBTAC_EncodeBlock8StereoS16(s16 *iblk, byte *oblk, int flags)
{
	s16 tcblk[64];
	int tcma[16], tcna[16];
	int tcmb[16], tcnb[16];
//	int lm, ln, cm, rm, rn, cn, tl, tr, tc;
	int cm, cn, tc;
	int cm2, cn2, ca2, ba;
	int p0, p1, p2, p3;
	int s0, s1;
	int i, j, k;

	BGBTAC_EncodeBlockHeaderStereoS16(iblk, oblk, tcblk, &cm, &cn, flags);

	for(i=0; i<16; i++)
	{
		cm2= 99999; cn2=-99999;
		for(j=0; j<4; j++)
		{
			tc=tcblk[i*4+j];
			if(tc<cm2) { cm2=tc; }
			if(tc>cn2) { cn2=tc; }
		}

#if 0
		ca2=(cm2+cn2)>>1; cm2=0; cn2=0;
		for(j=0; j<4; j++)
		{
			tc=tcblk[i*4+j];
			if(tc<ca2)	{ cm2+=tc; }
			else		{ cn2+=tc; }
		}
		cm2=(cm2+1)>>2;
		cn2=(cn2+1)>>2;
#endif

#if 0
		if(cn!=cm)
			{ s0=16777216/(cn-cm); }
		else { s0=0; }

		p0=(s0*(cm2-cm)+524288)>>20;
		p1=(s0*(cn2-cm)+524288)>>20;

//		p0=(s0*(cm2-cm)+262144)>>20;
//		p1=(s0*(cn2-cm)+786432)>>20;
#endif

#if 1
		if(cn!=cm)
			{ s0=268435456/(cn-cm); }
		else { s0=0; }

		p0=(s0*(cm2-cm)+8388608)>>24;
		p1=(s0*(cn2-cm)+8388608)>>24;
#endif

		p0=(p0<0)?0:((p0<16)?p0:15);
		p1=(p1<0)?0:((p1<16)?p1:15);
		
		tcma[i]=p0;
		tcna[i]=p1;

		s1=(cn-cm);
		cm2=((s1*p0+8)>>4)+cm;
		cn2=((s1*p1+8)>>4)+cm;

		tcmb[i]=cm2;
		tcnb[i]=cn2;
	}

	for(i=0; i<16; i++)
	{
		oblk[16+(i>>1)]|=tcma[i]<<((i&1)*4);
		oblk[24+(i>>1)]|=tcna[i]<<((i&1)*4);
	}
	
	for(i=0; i<16; i++)
	{
		ca2=(tcmb[i]+tcnb[i])>>1;
	
		for(j=0; j<4; j++)
		{
			k=i*4+j;
			tc=tcblk[k];
			if(tc>=ca2)
				{ oblk[8+(k>>3)]|=1<<(k&7); }
			else
				{ oblk[8+(k>>3)]&=~(1<<(k&7)); }
		}
	}	
}

void BGBTAC_DecodeBlock8StereoS16(byte *iblk, s16 *oblk, int flags)
{
//	int tcma[16], tcna[16];
//	int tcmb[16], tcnb[16];
	int p0, p1, p2, p3;
	int s0, s1;
	int cm, cn, ca;
	int lm, ln, la;
	int rm, rn, ra;
	int cm2, cn2, ca2;
	int lm2, ln2, la2;
	int rm2, rn2, ra2;
	int i, j, k;
	
//	cm=(s16)(iblk[0]+(iblk[1]<<8));
//	cn=(s16)(iblk[2]+(iblk[3]<<8));
	cm=(s16)(iblk[2]+(iblk[3]<<8));
	cn=(s16)(iblk[4]+(iblk[5]<<8));
	ca=(cm+cn)>>1;

//	i=((s16)(iblk[4]<<8));
//	j=((s16)(iblk[5]<<8));
//	i=((s16)(iblk[6]<<8));
//	j=((s16)(iblk[7]<<8));

	i=bgbtac_ssqri((s8)iblk[6]);
	j=bgbtac_ssqri((s8)iblk[7]);

	lm=cm+i; rm=cm-i;
	ln=cn+j; rn=cn-j;
	la=(lm+ln)>>1;
	ra=(rm+rn)>>1;

	lm=(lm<(-32768))?(-32768):((lm<32768)?lm:32767);
	ln=(ln<(-32768))?(-32768):((ln<32768)?ln:32767);
	rm=(rm<(-32768))?(-32768):((rm<32768)?rm:32767);
	rn=(rn<(-32768))?(-32768):((rn<32768)?rn:32767);

//	for(i=0; i<64; i++)
//	{
//		oblk[i*2+0]=la;
//		oblk[i*2+1]=ra;
//	}
		
	for(i=0; i<16; i++)
	{
		p0=(iblk[16+(i>>1)]>>((i&1)*4))&15;
		p1=(iblk[24+(i>>1)]>>((i&1)*4))&15;
//		cm2=tcmb[i];
//		cn2=tcnb[i];

//		s0=(cn-cm);
//		s1=(cn-cm);
	
		if(flags&(BGBTAC_EFL_STEREO|BGBTAC_EFL_HALFMONO))
		{
			s0=(ln-lm);
			s1=(rn-rm);
			lm2=((s0*p0+8)>>4)+lm;
			ln2=((s0*p1+8)>>4)+lm;
			rm2=((s1*p0+8)>>4)+rm;
			rn2=((s1*p1+8)>>4)+rm;
			la2=(lm2+ln2)>>1;
			ra2=(rm2+rn2)>>1;

//			lm2=(11*lm2+5*la2+8)>>4;
//			ln2=(11*ln2+5*la2+8)>>4;
//			rm2=(11*rm2+5*ra2+8)>>4;
//			rn2=(11*rn2+5*ra2+8)>>4;

			lm2=(13*lm2+3*la2+8)>>4;
			ln2=(13*ln2+3*la2+8)>>4;
			rm2=(13*rm2+3*ra2+8)>>4;
			rn2=(13*rn2+3*ra2+8)>>4;

//			lm2=(14*lm2+2*la2+8)>>4;
//			ln2=(14*ln2+2*la2+8)>>4;
//			rm2=(14*rm2+2*ra2+8)>>4;
//			rn2=(14*rn2+2*ra2+8)>>4;

			lm2=(lm2<lm)?lm:((lm2<ln)?lm2:ln);
			ln2=(ln2<lm)?lm:((ln2<ln)?ln2:ln);
			rm2=(rm2<rm)?rm:((rm2<rn)?rm2:rn);
			rn2=(rn2<rm)?rm:((rn2<rn)?rn2:rn);
		}else
		{
			cm2=((s1*p0+8)>>4)+cm;
			cn2=((s1*p1+8)>>4)+cm;
			ca2=(cm2+cn2)>>1;

//			cm2=(11*cm2+5*ca2+8)>>4;
//			cn2=(11*cn2+5*ca2+8)>>4;
			cm2=(13*cm2+3*ca2+8)>>4;
			cn2=(13*cn2+3*ca2+8)>>4;

			cm2=(cm2<cm)?cm:((cm2<cn)?cm2:cn);
			cn2=(cn2<cm)?cm:((cn2<cn)?cn2:cn);
		}

	
//		p2=(p0+p1)>>1;
//		if(p2<cm)p2=cm;
//		if(p2>cn)p2=cn;
		
		for(j=0; j<4; j++)
		{
			k=i*4+j;
		
//			oblk[i*16+j]=p2;
			p0=(iblk[8+(k>>3)]&(1<<(k&7)));
			if(flags&BGBTAC_EFL_HALFMONO)
				{ oblk[k*2+0]=p0?ln2:lm2; }
			else if(flags&BGBTAC_EFL_STEREO)
			{
				oblk[k*2+0]=p0?ln2:lm2;
				oblk[k*2+1]=p0?rn2:rm2;
			}else
			{
				oblk[k]=p0?cn2:cm2;
			}

//			oblk[k*2+0]=p0?cn2:cm2;
//			oblk[k*2+1]=p0?cn2:cm2;
		}
	}
}
#endif

#if 0
void BGBTAC_EncodeBlock8StereoS16(s16 *iblk, byte *oblk, int flags)
{
	static int qfac[4][4]={	//0.25, 0.416_, 0.583_, 0.75
//		{144,  96,  16, 0},
//		{ 87, 125,  44, 0},
//		{ 44, 125,  87, 0},
//		{ 16,  96, 144, 0}

//		{112, 128,  16, 0},
//		{ 64, 128,  32, 0},
//		{ 32, 128,  64, 0},
//		{ 16, 128, 112, 0}

//		{ 65, 181,  10, 0},
//		{ 13, 236,   7, 0},
//		{  7, 236,  13, 0},
//		{ 10, 181,  65, 0}

		{ 67, 181,   8, 0},
		{ 13, 236,   7, 0},
		{  7, 236,  13, 0},
		{  8, 181,  67, 0}

//		{0, 256, 0, 0},
//		{0, 256, 0, 0},
//		{0, 256, 0, 0},
//		{0, 256, 0, 0}
	};

	s16 tcblk[64];
	int tcma[16], tcna[16];
	int tcmb[16], tcnb[16];
//	int lm, ln, cm, rm, rn, cn, tl, tr, tc;
	int cm, cn, tc;
	int cm2, cn2, ca2, ba;
	int cm2a, cn2a, cm2b, cn2b;
	int cm3, cn3;
	int p0, p1, p2, p3;
	int s0, s1;
	int i, j, k;

	BGBTAC_EncodeBlockHeaderStereoS16(iblk, oblk, tcblk, &cm, &cn, flags);

	for(i=0; i<16; i++)
	{
		cm2= 99999; cn2=-99999;
		for(j=0; j<4; j++)
		{
			tc=tcblk[i*4+j];
			if(tc<cm2) { cm2=tc; }
			if(tc>cn2) { cn2=tc; }
		}
		
		if(cn!=cm)
			{ s0=16777216/(cn-cm); }
		else { s0=0; }

		p0=(s0*(cm2-cm)+524288)>>20;
		p1=(s0*(cn2-cm)+524288)>>20;

//		p0=(s0*(cm2-cm)+262144)>>20;
//		p1=(s0*(cn2-cm)+786432)>>20;
		
		p0=(p0<0)?0:((p0<16)?p0:15);
		p1=(p1<0)?0:((p1<16)?p1:15);
		
		tcma[i]=p0;
		tcna[i]=p1;

		s1=(cn-cm);
		cm2=((s1*p0+8)>>4)+cm;
		cn2=((s1*p1+8)>>4)+cm;

//		ca2=(cm2+cn2)>>1;
//		cm2=(11*cm2+5*ca2+8)>>4;
//		cn2=(11*cn2+5*ca2+8)>>4;

		tcmb[i]=cm2;
		tcnb[i]=cn2;
	}

	for(i=0; i<16; i++)
	{
		oblk[16+(i>>1)]|=tcma[i]<<((i&1)*4);
		oblk[24+(i>>1)]|=tcna[i]<<((i&1)*4);
	}
	
	for(i=0; i<16; i++)
	{
		cm2=tcmb[i]; cn2=tcnb[i];
		ca2=(tcmb[i]+tcnb[i])>>1;

		if(i>0)		{ cm2a=tcmb[i-1]; cn2a=tcnb[i-1]; }
		else		{ cm2a=cm2; cn2a=cn2; }
		if(i<15)	{ cm2b=tcmb[i+1]; cn2b=tcnb[i+1]; }
		else		{ cm2b=cm2; cn2b=cn2; }

		for(j=0; j<4; j++)
		{
			cm3=(qfac[j][0]*cm2a+qfac[j][1]*cm2+qfac[j][2]*cm2b+128)>>8;
			cn3=(qfac[j][0]*cn2a+qfac[j][1]*cn2+qfac[j][2]*cn2b+128)>>8;
			ca2=(cm3+cn3)>>1;

			k=i*4+j;
			tc=tcblk[k];
			if(tc>=ca2)		{ oblk[8+(k>>3)]|=  1<<(k&7); }
			else			{ oblk[8+(k>>3)]&=~(1<<(k&7)); }
		}
	}	
}

void BGBTAC_DecodeBlock8StereoS16(byte *iblk, s16 *oblk, int flags)
{
//	int tcma[16], tcna[16];
//	int tcmb[16], tcnb[16];

	int ltcmb[16], ltcnb[16];
	int rtcmb[16], rtcnb[16];
	static int qfac[4][4]={	//0.25, 0.416_, 0.583_, 0.75
//		{256+ 16-128, 128- 32,  16},
//		{256+ 44-213, 213- 89,  44},
//		{256+ 87-299, 299-174,  87},
//		{256+144-384, 384-288, 144}

//		{144,  96,  16, 0},
//		{ 87, 125,  44, 0},
//		{ 44, 125,  87, 0},
//		{ 16,  96, 144, 0}

//		{112, 128,  16, 0},
//		{ 64, 128,  32, 0},
//		{ 32, 128,  64, 0},
//		{ 16, 128, 112, 0}

//		{ 65, 181,  10, 0},
//		{ 13, 236,   7, 0},
//		{  7, 236,  13, 0},
//		{ 10, 181,  65, 0}

		{ 67, 181,   8, 0},
		{ 13, 236,   7, 0},
		{  7, 236,  13, 0},
		{  8, 181,  67, 0}

//		{0, 256, 0, 0},
//		{0, 256, 0, 0},
//		{0, 256, 0, 0},
//		{0, 256, 0, 0}
	};


	int p0, p1, p2, p3;
	int s0, s1;
	int cm, cn, ca;
	int lm, ln, la;
	int rm, rn, ra;
	int cm2, cn2, ca2;
	int lm2, ln2, la2;
	int rm2, rn2, ra2;
	int lm2a, ln2a, lm2b, ln2b;
	int rm2a, rn2a, rm2b, rn2b;

	int lm3, ln3;
	int rm3, rn3;
	int i, j, k;
	
	cm=(s16)(iblk[2]+(iblk[3]<<8));
	cn=(s16)(iblk[4]+(iblk[5]<<8));
	ca=(cm+cn)>>1;

	i=bgbtac_ssqri((s8)iblk[6]);
	j=bgbtac_ssqri((s8)iblk[7]);

	lm=cm+i; rm=cm-i;
	ln=cn+j; rn=cn-j;
	la=(lm+ln)>>1;
	ra=(rm+rn)>>1;

	lm=(lm<(-32768))?(-32768):((lm<32768)?lm:32767);
	ln=(ln<(-32768))?(-32768):((ln<32768)?ln:32767);
	rm=(rm<(-32768))?(-32768):((rm<32768)?rm:32767);
	rn=(rn<(-32768))?(-32768):((rn<32768)?rn:32767);

//	for(i=0; i<64; i++)
//	{
//		oblk[i*2+0]=la;
//		oblk[i*2+1]=ra;
//	}
		
	for(i=0; i<16; i++)
	{
		p0=(iblk[16+(i>>1)]>>((i&1)*4))&15;
		p1=(iblk[24+(i>>1)]>>((i&1)*4))&15;
//		cm2=tcmb[i];
//		cn2=tcnb[i];

//		s0=(cn-cm);
//		s1=(cn-cm);
	
		if(flags&BGBTAC_EFL_STEREO)
		{
			s0=(ln-lm);
			s1=(rn-rm);
			lm2=((s0*p0+8)>>4)+lm;
			ln2=((s0*p1+8)>>4)+lm;
			rm2=((s1*p0+8)>>4)+rm;
			rn2=((s1*p1+8)>>4)+rm;
			la2=(lm2+ln2)>>1;
			ra2=(rm2+rn2)>>1;

//			lm2=(11*lm2+5*la2+8)>>4;
//			ln2=(11*ln2+5*la2+8)>>4;
//			rm2=(11*rm2+5*ra2+8)>>4;
//			rn2=(11*rn2+5*ra2+8)>>4;

			lm2=(lm2<lm)?lm:((lm2<ln)?lm2:ln);
			ln2=(ln2<lm)?lm:((ln2<ln)?ln2:ln);
			rm2=(rm2<rm)?rm:((rm2<rn)?rm2:rn);
			rn2=(rn2<rm)?rm:((rn2<rn)?rn2:rn);
			
			ltcmb[i]=lm2;
			ltcnb[i]=ln2;
			rtcmb[i]=rm2;
			rtcnb[i]=rn2;
		}else
		{
			cm2=((s1*p0+8)>>4)+cm;
			cn2=((s1*p1+8)>>4)+cm;
			ca2=(cm2+cn2)>>1;

			cm2=(11*cm2+5*ca2+8)>>4;
			cn2=(11*cn2+5*ca2+8)>>4;

			cm2=(cm2<cm)?cm:((cm2<cn)?cm2:cn);
			cn2=(cn2<cm)?cm:((cn2<cn)?cn2:cn);

			ltcmb[i]=cm2;
			ltcnb[i]=cn2;
			rtcmb[i]=cm2;
			rtcnb[i]=cn2;
		}
	}
	
	if(flags&BGBTAC_EFL_STEREO)
	{
		for(i=0; i<16; i++)
		{
			lm2=ltcmb[i]; ln2=ltcnb[i];
			rm2=rtcmb[i]; rn2=rtcnb[i];

			if(i>0)
			{
				lm2a=ltcmb[i-1]; ln2a=ltcnb[i-1];
				rm2a=rtcmb[i-1]; rn2a=rtcnb[i-1];
			}else
			{
				lm2a=lm2; ln2a=ln2;
				rm2a=rm2; rn2a=rn2;
			}

			if(i<15)
			{
				lm2b=ltcmb[i+1]; ln2b=ltcnb[i+1];
				rm2b=rtcmb[i+1]; rn2b=rtcnb[i+1];
			}else
			{
				lm2b=lm2; ln2b=ln2;
				rm2b=rm2; rn2b=rn2;
			}

			for(j=0; j<4; j++)
			{
				lm3=(qfac[j][0]*lm2a+qfac[j][1]*lm2+qfac[j][2]*lm2b+128)>>8;
				ln3=(qfac[j][0]*ln2a+qfac[j][1]*ln2+qfac[j][2]*ln2b+128)>>8;
				rm3=(qfac[j][0]*rm2a+qfac[j][1]*rm2+qfac[j][2]*rm2b+128)>>8;
				rn3=(qfac[j][0]*rn2a+qfac[j][1]*rn2+qfac[j][2]*rn2b+128)>>8;

//				la2=(lm3+ln3)>>1;
//				ra2=(rm3+rn3)>>1;
//				lm3=(11*lm3+5*la2+8)>>4;
//				ln3=(11*ln3+5*la2+8)>>4;
//				rm3=(11*rm3+5*ra2+8)>>4;
//				rn3=(11*rn3+5*ra2+8)>>4;

				k=i*4+j;
		
//				oblk[i*16+j]=p2;
				p0=(iblk[8+(k>>3)]&(1<<(k&7)));
				oblk[k*2+0]=p0?ln3:lm3;
				oblk[k*2+1]=p0?rn3:rm3;

//				oblk[k*2+0]=p0?cn2:cm2;
//				oblk[k*2+1]=p0?cn2:cm2;
			}
		}
	}else
	{
		for(i=0; i<16; i++)
		{
			lm2=ltcmb[i]; ln2=ltcnb[i];

			if(i>0)
				{ lm2a=ltcmb[i-1]; ln2a=ltcnb[i-1]; }
			else
				{ lm2a=lm2; ln2a=ln2; }

			if(i<15)
				{ lm2b=ltcmb[i-1]; ln2b=ltcnb[i-1]; }
			else
				{ lm2b=lm2; ln2b=ln2; }

			for(j=0; j<4; j++)
			{
				lm3=(qfac[j][0]*lm2a+qfac[j][1]*lm2+qfac[j][2]*lm2b+128)>>8;
				ln3=(qfac[j][0]*ln2a+qfac[j][1]*ln2+qfac[j][2]*ln2b+128)>>8;
		
				k=i*4+j;
		
//				oblk[i*16+j]=p2;
				p0=(iblk[8+(k>>3)]&(1<<(k&7)));
				oblk[k]=p0?ln3:lm3;
			}
		}
	}
}
#endif


BTAC_API void BGBTAC_EncodeStream8StereoS16(
	s16 *iblk, byte *oblk, int len, int flags)
{
	int i, j, n;
	
	n=(len+63)/64;
	for(i=0; i<n; i++)
	{
		BGBTAC_EncodeBlock8StereoS16(iblk+i*2*64, oblk+i*32, flags);
	}
}

BTAC_API void BGBTAC_FilterStream8StereoS16(s16 *buf, int len)
{
	int ta[2];
	int a, b, c, n;
	int i, j, k;
	
	ta[0]=0; ta[1]=0;
	
//	for(i=8; i<(len-8); i++)
	for(i=0; i<len; i++)
		for(j=0; j<2; j++)
	{
//		a=0;
//		for(k=0; k<8; k++)
//		{
//			a+=buf[(i+k-4)*2+j];
//		}
//		a/=8;
		
		a=ta[j];
		
		b=buf[i*2+j];
//		c=(a*5+b*11+8)>>4;
//		c=(a*3+b*13+8)>>4;

//		c=(a*80+b*176+128)>>8;
		c=(a*48+b*208+128)>>8;
		buf[i*2+j]=c;
		
		a=(b*128+a*128+128)>>8;
//		a=(b*96+a*160+128)>>8;
//		a=(b*80+a*176+128)>>8;
//		a=(b*48+a*208+128)>>8;
		ta[j]=a;
	}
}

BTAC_API void BGBTAC_DeblockStream8StereoS16(
	s16 *oblk, int len)
{
	int p0, p1, p2, p3;
	int q0, q1, q2, q3;
	float f, g;
	int i, j, k, n;

	n=(len+63)/64;

#if 1
	//deblocking...
	for(i=1; i<(n-1); i++)
		for(j=0; j<2; j++)
	{
#if 0
		p0=oblk[i*64*2-2+j];
		p1=oblk[i*64*2+0+j];
		p0=(p0+p1)>>1;
		for(k=-16; k<16; k++)
		{
			f=0.5*M_PI*(k/16.0);
			g=cos(f);
			if(g<0)g=0;
			g=g*g;

			p1=oblk[(i*64+k)*2+j];
			p1=((1.0-g)*p1)+(g*p0);
			oblk[(i*64+k)*2+j]=p1;
		}
#endif
	
#if 0
		p0=oblk[i*64*2-2];
		p1=oblk[i*64*2-1];
		p2=oblk[i*64*2+0];
		p3=oblk[i*64*2+1];

		q0=(11*p0+5*p2)>>4;
		q1=(11*p1+5*p3)>>4;
		q2=(11*p2+5*p0)>>4;
		q3=(11*p3+5*p1)>>4;
		
		oblk[i*64*2-2]=q0;
		oblk[i*64*2-1]=q1;
		oblk[i*64*2+0]=q2;
		oblk[i*64*2+1]=q3;
#endif

#if 1
		p0=oblk[i*64*2-4+j];
		p1=oblk[i*64*2-2+j];
		p2=oblk[i*64*2+0+j];
		p3=oblk[i*64*2+2+j];

		q0=(128*p0+ 64*p1+ 48*p2+ 16*p3)>>8;
		q1=( 48*p0+128*p1+ 48*p2+ 32*p3)>>8;
		q2=( 32*p0+ 48*p1+128*p2+ 48*p3)>>8;
		q3=( 16*p0+ 48*p1+ 64*p2+128*p3)>>8;
		
		oblk[i*64*2-4+j]=q0;
		oblk[i*64*2-2+j]=q1;
		oblk[i*64*2+0+j]=q2;
		oblk[i*64*2+2+j]=q3;
#endif
	}
#endif
}

BTAC_API void BGBTAC_DecodeStream8StereoS16(
	byte *iblk, s16 *oblk, int len, int flags)
{
	int p0, p1, p2, p3;
	int q0, q1, q2, q3;
	float f, g;
	int i, j, k, n;
	
	n=(len+63)/64;
	for(i=0; i<n; i++)
	{
		BGBTAC_DecodeBlock8StereoS16(iblk+i*32, oblk+i*2*64, flags);
	}

	BGBTAC_FilterStream8StereoS16(oblk, len);
	BGBTAC_DeblockStream8StereoS16(oblk, len);
}


void BGBTAC_EncodeBlock9StereoS16(s16 *iblk, byte *oblk, int flags)
{
	s16 tcblk[64];
	int tcma[32];
	int tcmb[32];
//	int lm, ln, cm, rm, rn, cn, tl, tr, tc;
	int cm, cn, tc;
	int cm2, cn2, ca2, ba;
	int p0, p1, p2, p3;
	int s0, s1, s2, s3;
	int i, j, k;

	BGBTAC_EncodeBlockHeaderStereoS16(iblk, oblk, tcblk, &cm, &cn, flags);

	oblk[0]=0x01;
	oblk[1]=0x00;
	
	for(i=0; i<32; i++)
	{
		cm2= 99999; cn2=-99999;
		tc=tcblk[i*2+0];

#if 0
		if(cn!=cm)
			{ s0=16777216/(cn-cm); }
		else { s0=0; }

		p0=(s0*(tc-cm)+524288)>>20;
		p0=(p0<0)?0:((p0<16)?p0:15);
		tcma[i]=p0;
#endif

#if 1
		if(cn!=cm)
			{ s0=268435456/(cn-cm); }
		else { s0=0; }
		p0=(s0*(tc-cm)+8388608)>>24;
		p0=(p0<0)?0:((p0<16)?p0:15);
		tcma[i]=p0;
#endif


		s1=(cn-cm);
		p1=((s1*p0+8)>>4)+cm;

		tcmb[i]=p1;
	}

	for(i=0; i<32; i++)
		{ oblk[16+(i>>1)]|=tcma[i]<<((i&1)*4); }

#if 1
	for(i=0; i<32; i++)
	{
		p0=tcmb[i];
//		p1=tcmb[(i+1)&31];
//		p2=tcmb[(i-1)&31];
//		p3=tcmb[(i+2)&31];
		p1=(i<31)?tcmb[(i+1)&31]:p0;
		p2=(i>0)?tcmb[(i-1)&31]:p0;
		p3=(i<30)?tcmb[(i+2)&31]:p1;
		ca2=(p0+p1)>>1;
	
		tc=tcblk[i*2+1];

		k=0; s0=abs(tc-p0);
		s1=abs(tc-p1);
		if(s1<s0) { k=1; s0=s1; }
		s2=(p0+p1)>>1;
		s1=abs(tc-s2);
		if(s1<s0) { k=2; s0=s1; }

		s2=(3*p0+3*p1-p2-p3+2)>>2;
		s1=abs(tc-s2);
		if(s1<s0) { k=3; s0=s1; }

//		if(p0>p1)
//		{
//			k=(tc>ca2)?0:3;
//		}else
//		{
//			k=(tc>ca2)?3:0;
//		}
		oblk[8+(i>>2)]|=k<<((i&3)*2);
	}
#endif
}

void BGBTAC_DecodeBlock9StereoS16(byte *iblk, s16 *oblk, int flags)
{
	int p0, p1, p2, p3;
	int s0, s1;
	int cm, cn, ca;
	int lm, ln, la;
	int rm, rn, ra;
	int cm2, cn2, ca2;
	int lm2, ln2, la2;
	int rm2, rn2, ra2;
	int i, j, k;
	
	cm=(s16)(iblk[2]+(iblk[3]<<8));
	cn=(s16)(iblk[4]+(iblk[5]<<8));
	ca=(cm+cn)>>1;

	i=bgbtac_ssqri((s8)iblk[6]);
	j=bgbtac_ssqri((s8)iblk[7]);

	lm=cm+i; rm=cm-i;
	ln=cn+j; rn=cn-j;
	la=(lm+ln)>>1;
	ra=(rm+rn)>>1;

	lm=(lm<(-32768))?(-32768):((lm<32768)?lm:32767);
	ln=(ln<(-32768))?(-32768):((ln<32768)?ln:32767);
	rm=(rm<(-32768))?(-32768):((rm<32768)?rm:32767);
	rn=(rn<(-32768))?(-32768):((rn<32768)?rn:32767);

//	for(i=0; i<64; i++)
//	{
//		oblk[i*2+0]=la;
//		oblk[i*2+1]=ra;
//	}
		
	for(i=0; i<32; i++)
	{
		p0=(iblk[16+(i>>1)]>>((i&1)*4))&15;
	
		if(flags&BGBTAC_EFL_HALFMONO)
		{
			s0=(cn-cm);
			cm2=((s0*p0+8)>>4)+cm;
			cm2=(cm2<cm)?cm:((cm2<cn)?cm2:cn);
			oblk[i*4+0]=cm2;
		}
		else if(flags&BGBTAC_EFL_STEREO)
		{
			s0=(ln-lm);
			s1=(rn-rm);
			lm2=((s0*p0+8)>>4)+lm;
			rm2=((s1*p0+8)>>4)+rm;

			lm2=(lm2<lm)?lm:((lm2<ln)?lm2:ln);
			rm2=(rm2<rm)?rm:((rm2<rn)?rm2:rn);

			oblk[i*4+0]=lm2;
			oblk[i*4+1]=rm2;
		}else
		{
			s0=(cn-cm);
			cm2=((s0*p0+8)>>4)+cm;

			cm2=(cm2<cm)?cm:((cm2<cn)?cm2:cn);

			oblk[i*2+0]=cm2;
		}
	}

#if 0
	if(flags&BGBTAC_EFL_STEREO)
	{
		for(i=0; i<32; i++)
		{
			lm2=oblk[i*4+0];
			rm2=oblk[i*4+1];
			ln2=oblk[((i+1)&31)*4+0];
			rn2=oblk[((i+1)&31)*4+1];

			p0=(iblk[8+(i>>2)]>>((i&3)*2))&3;

			s0=(ln2-lm2);
			s1=(rn2-rm2);
			p2=((s0*p0+1)>>2)+lm2;
			p3=((s1*p0+1)>>2)+rm2;

//			p2=lm2;
//			p3=rm2;

			oblk[i*4+2]=p2;
			oblk[i*4+3]=p3;
		}
	}else
	{
		for(i=0; i<32; i++)
		{
			cm2=oblk[i*2+0];
			cn2=oblk[((i+1)&31)*2+0];

			p0=(iblk[8+(i>>2)]>>((i&3)*2))&3;

			s0=(cn2-cm2);
			p2=((s0*p0+1)>>2)+cm2;

			oblk[i*2+1]=p2;
		}
	}
#endif

#if 1
	if(flags&BGBTAC_EFL_HALFMONO)
	{
		for(i=0; i<32; i++)
		{
			p1=oblk[i*4+0];
			p0=(i>0)?oblk[i*4-4]:p1;
			p2=(i<31)?oblk[i*4+4]:p1;
			p3=(i<30)?oblk[i*4+8]:p2;

			p3=(3*p1+3*p2-p0-p3+2)>>2;
			p3=(p3<(-32768))?(-32768):((p3<32768)?p3:32767);

			oblk[i*4+2]=p3;
		}
	}else if(flags&BGBTAC_EFL_STEREO)
	{
		for(i=0; i<32; i++)
			for(j=0; j<2; j++)
		{
			p1=oblk[i*4+0+j];
			p0=(i>0)?oblk[i*4-4+j]:p1;
			p2=(i<31)?oblk[i*4+4+j]:p1;
			p3=(i<30)?oblk[i*4+8+j]:p2;

			s0=(iblk[8+(i>>2)]>>((i&3)*2))&3;
			
			switch(s0)
			{
			case 0:
				p3=p1;
				break;
			case 1:
				p3=p2;
				break;
			case 2:
				p3=(p1+p2)>>1;
				break;
			case 3:
				p3=(3*p1+3*p2-p0-p3+2)>>2;
				p3=(p3<(-32768))?(-32768):((p3<32768)?p3:32767);
				break;
			default:
				p3=(p1+p2)>>1;
				break;
			}

			oblk[i*4+2+j]=p3;
		}
	}else
	{
		for(i=0; i<32; i++)
		{
			p1=oblk[i*2+0];
			p0=(i>0)?oblk[i*2-2]:p1;
			p2=(i<31)?oblk[i*2+2]:p1;
			p3=(i<30)?oblk[i*2+4]:p2;

			p3=(3*p1+3*p2-p0-p3+2)>>2;
			p3=(p3<(-32768))?(-32768):((p3<32768)?p3:32767);

			oblk[i*2+1]=p3;
		}
	}
#endif
}

void BGBTAC_EncodeBlock10StereoS16(s16 *iblk, byte *oblk, int flags)
{
	s16 tcblk[64];
	int tcma[32];
	int tcmb[32];
//	int lm, ln, cm, rm, rn, cn, tl, tr, tc;
	int cm, cn, tc;
	int cm2, cn2, ca2, ba;
	int p0, p1, p2, p3;
	int s0, s1;
	int i, j, k;

	BGBTAC_EncodeBlockHeaderStereoS16(iblk, oblk, tcblk, &cm, &cn, flags);

	oblk[0]=0x02;
	oblk[1]=0x00;

	for(i=0; i<32; i++)
	{
		cm2= 99999; cn2=-99999;
		tc=tcblk[i*2+0];
//		tc=(tcblk[i*2+0]+tcblk[i*2+1])>>1;
//		tc=(2*tcblk[i*2+0]+tcblk[i*2+1]+tcblk[((i-1)&31)*2+1]+2)>>2;
		
		if(cn!=cm)
			{ s0=16777216/(cn-cm); }
		else { s0=0; }

		p0=(s0*(tc-cm)+131072)>>18;
		p0=(p0<0)?0:((p0<64)?p0:63);
		tcma[i]=p0;

//		s1=(cn-cm);
//		p1=((s1*p0+8)>>4)+cm;
//		tcmb[i]=p1;
	}

	for(i=0; i<32; i++)
	{
		j=i*6;
		k=tcma[i]<<(j&7);

		oblk[8+(j>>3)+0]|=k;
		oblk[8+(j>>3)+1]|=k>>8;
	
//		oblk[16+(i>>1)]|=tcma[i]<<((i&1)*4);
	}
}

void BGBTAC_DecodeBlock10StereoS16(byte *iblk, s16 *oblk, int flags)
{
	int p0, p1, p2, p3;
	int s0, s1;
	int cm, cn, ca;
	int lm, ln, la;
	int rm, rn, ra;
	int cm2, cn2, ca2;
	int lm2, ln2, la2;
	int rm2, rn2, ra2;
	int i, j, k;
	
	cm=(s16)(iblk[2]+(iblk[3]<<8));
	cn=(s16)(iblk[4]+(iblk[5]<<8));
	ca=(cm+cn)>>1;

	i=bgbtac_ssqri((s8)iblk[6]);
	j=bgbtac_ssqri((s8)iblk[7]);

	lm=cm+i; rm=cm-i;
	ln=cn+j; rn=cn-j;
	la=(lm+ln)>>1;
	ra=(rm+rn)>>1;

	lm=(lm<(-32768))?(-32768):((lm<32768)?lm:32767);
	ln=(ln<(-32768))?(-32768):((ln<32768)?ln:32767);
	rm=(rm<(-32768))?(-32768):((rm<32768)?rm:32767);
	rn=(rn<(-32768))?(-32768):((rn<32768)?rn:32767);

//	for(i=0; i<64; i++)
//	{
//		oblk[i*2+0]=la;
//		oblk[i*2+1]=ra;
//	}
		
	for(i=0; i<32; i++)
	{
		j=i*6;
		
		k=iblk[8+(j>>3)+0] | (iblk[8+(j>>3)+1]<<8);
		p0=(k>>(j&7))&63;
		
//		p0=(iblk[16+(i>>1)]>>((i&1)*4))&15;

		if(flags&BGBTAC_EFL_HALFMONO)
		{
			s0=(ln-lm);
			lm2=((s0*p0+32)>>6)+lm;

			lm2=(lm2<lm)?lm:((lm2<ln)?lm2:ln);

			oblk[i*4+0]=lm2;
			oblk[i*4+2]=lm2;
		}else if(flags&BGBTAC_EFL_STEREO)
		{
			s0=(ln-lm);
			s1=(rn-rm);
			lm2=((s0*p0+32)>>6)+lm;
			rm2=((s1*p0+32)>>6)+rm;

			lm2=(lm2<lm)?lm:((lm2<ln)?lm2:ln);
			rm2=(rm2<rm)?rm:((rm2<rn)?rm2:rn);

			oblk[i*4+0]=lm2;
			oblk[i*4+1]=rm2;
			oblk[i*4+2]=lm2;
			oblk[i*4+3]=rm2;
		}else
		{
			s0=(cn-cm);
			cm2=((s0*p0+32)>>6)+cm;
			cm2=(cm2<cm)?cm:((cm2<cn)?cm2:cn);

			oblk[i*2+0]=cm2;
			oblk[i*2+1]=cm2;
		}
	}

#if 0
	for(i=0; i<31; i++)
	{
		p0=oblk[i*4+2];		p1=oblk[i*4+3];
		p2=oblk[i*4+4];		p3=oblk[i*4+5];

		p0=(p0+p2)>>1;
		p1=(p1+p3)>>1;

		oblk[i*4+2]=p0;
		oblk[i*4+3]=p1;
	}
#endif

#if 0
	if(flags&BGBTAC_EFL_STEREO)
	{
		for(i=0; i<31; i++)
		{
			p0=oblk[i*4+2];		p1=oblk[i*4+3];
			p2=oblk[i*4+4];		p3=oblk[i*4+5];

			p0=(p0+p2)>>1;
			p1=(p1+p3)>>1;

			oblk[i*4+2]=p0;
			oblk[i*4+3]=p1;
		}
	}else
	{
		for(i=0; i<31; i++)
		{
			p0=oblk[i*2+1];		p1=oblk[i*2+2];
			p0=(p0+p1)>>1;
			oblk[i*2+1]=p0;
		}
	}
#endif

#if 1
	if(flags&BGBTAC_EFL_HALFMONO)
	{
		for(i=0; i<32; i++)
		{
			p1=oblk[i*4+0];
			p0=(i>0)?oblk[i*4-4]:p1;
			p2=(i<31)?oblk[i*4+4]:p1;
			p3=(i<30)?oblk[i*4+8]:p2;

			p3=(3*p1+3*p2-p0-p3+2)>>2;
			p3=(p3<(-32768))?(-32768):((p3<32768)?p3:32767);

			oblk[i*4+2]=p3;
		}
	}else
	if(flags&BGBTAC_EFL_STEREO)
	{
		for(i=0; i<32; i++)
			for(j=0; j<2; j++)
		{
			p1=oblk[i*4+0+j];
			p0=(i>0)?oblk[i*4-4+j]:p1;
			p2=(i<31)?oblk[i*4+4+j]:p1;
			p3=(i<30)?oblk[i*4+8+j]:p2;

//			s0=2*p1-p0;
//			s1=2*p2-p3;
//			p3=(s0+s1)>>1;
//			p3=(s0+s1+p1+p2)>>2;

			p3=(3*p1+3*p2-p0-p3+2)>>2;

//			p3=(p1+p2)-p0;
			p3=(p3<(-32768))?(-32768):((p3<32768)?p3:32767);

			oblk[i*4+2+j]=p3;
		}
	}else
	{
		for(i=0; i<32; i++)
		{
			p1=oblk[i*2+0];
			p0=(i>0)?oblk[i*2-2]:p1;
			p2=(i<31)?oblk[i*2+2]:p1;
			p3=(i<30)?oblk[i*2+4]:p2;

			p3=(3*p1+3*p2-p0-p3+2)>>2;
			p3=(p3<(-32768))?(-32768):((p3<32768)?p3:32767);

			oblk[i*2+1]=p3;
		}
	}
#endif
}

void BGBTAC_EncodeBlock12StereoS16(s16 *iblk, byte *oblk, int flags)
{
	s16 tcblk[64], tcpan[64];
	int tcma[32];
	int tcmb[32];
	int lm, ln, cm, rm, rn, cn, tl, tr, tc;
//	int cm, cn, tc;
	int cm2, cn2, ca2, ba;
	int p0, p1, p2, p3;
	int s0, s1;
	int i, j, k;

	BGBTAC_EncodeBlockHeaderPanStereoS16(
		iblk, oblk, tcblk, tcpan,
		&cm, &cn, &lm, &ln, &rm, &rn,
		flags);

	oblk[0]=0x03;
	oblk[1]=0x00;
	
	for(i=0; i<32; i++)
	{
		cm2= 99999; cn2=-99999;
		tc=tcblk[i*2+0];
		
		if(cn!=cm)
			{ s0=16777216/(cn-cm); }
		else { s0=0; }

		p0=(s0*(tc-cm)+524288)>>20;
		p0=(p0<0)?0:((p0<16)?p0:15);
		tcma[i]=p0;

		s1=(cn-cm);
		p1=((s1*p0+8)>>4)+cm;

		tcmb[i]=p1;
	}

	for(i=0; i<32; i++)
		{ oblk[16+(i>>1)]|=tcma[i]<<((i&1)*4); }

#if 1
	tl=abs(ln-cn);
	tr=-abs(rn-cn);

	for(i=0; i<32; i++)
	{
		tc=tcpan[i*2+0];

		p2=0;

#if 1
		if(tc>tl)
			{ p2=2; }
		else if(tc<tr)
			{ p2=3; }
		else if(abs(tc)<(tl>>2))
			{ p2=1; }
		else
			{ p2=0; }
#endif

		oblk[8+(i>>2)]|=p2<<((i&3)*2);
	}
#endif
}

void BGBTAC_DecodeBlock12StereoS16(byte *iblk, s16 *oblk, int flags)
{
	int p0, p1, p2, p3;
	int s0, s1, s2;
	int cm, cn, ca;
	int lm, ln, la;
	int rm, rn, ra;
	int cm2, cn2, ca2;
	int lm2, ln2, la2;
	int rm2, rn2, ra2;
	int lm3, rm3;
	int i, j, k;
	
	cm=(s16)(iblk[2]+(iblk[3]<<8));
	cn=(s16)(iblk[4]+(iblk[5]<<8));
	ca=(cm+cn)>>1;

	i=bgbtac_ssqri((s8)iblk[6]);
	j=bgbtac_ssqri((s8)iblk[7]);

	lm=cm+i; rm=cm-i;
	ln=cn+j; rn=cn-j;
	la=(lm+ln)>>1;
	ra=(rm+rn)>>1;

	lm=(lm<(-32768))?(-32768):((lm<32768)?lm:32767);
	ln=(ln<(-32768))?(-32768):((ln<32768)?ln:32767);
	rm=(rm<(-32768))?(-32768):((rm<32768)?rm:32767);
	rn=(rn<(-32768))?(-32768):((rn<32768)?rn:32767);

//	for(i=0; i<64; i++)
//	{
//		oblk[i*2+0]=la;
//		oblk[i*2+1]=ra;
//	}
		
	for(i=0; i<32; i++)
	{
		p0=(iblk[16+(i>>1)]>>((i&1)*4))&15;
		p1=(iblk[8+(i>>2)]>>((i&3)*2))&3;
	
		if(flags&BGBTAC_EFL_HALFMONO)
		{
			s0=(cn-cm);
			cm2=((s0*p0+8)>>4)+cm;

			cm2=(cm2<cm)?cm:((cm2<cn)?cm2:cn);

			oblk[i*4+0]=cm2;
			oblk[i*4+2]=cm2;
		}else if(flags&BGBTAC_EFL_STEREO)
		{
			s0=(ln-lm);
			s1=(rn-rm);
			s2=(cn-cm);
			lm2=((s0*p0+8)>>4)+lm;
			rm2=((s1*p0+8)>>4)+rm;
			cm2=((s2*p0+8)>>4)+cm;

			lm2=(lm2<lm)?lm:((lm2<ln)?lm2:ln);
			rm2=(rm2<rm)?rm:((rm2<rn)?rm2:rn);
			cm2=(cm2<cm)?cm:((cm2<cn)?cm2:cn);

			switch(p1)
			{
			case 0:
				oblk[i*4+0]=lm2;
				oblk[i*4+1]=rm2;
				break;
			case 1:
				oblk[i*4+0]=cm2;
				oblk[i*4+1]=cm2;
				break;
			case 2:
				lm3=max(lm2, cm2);
				rm3=min(rm2, cm2);
//				lm3=2*lm2-la;
//				rm3=((rm2-ra)>>1)+ra;
//				lm3=(lm3<(-32768))?(-32768):((lm3<32768)?lm3:32767);
//				rm3=(rm3<(-32768))?(-32768):((rm3<32768)?rm3:32767);
				oblk[i*4+0]=lm3;
				oblk[i*4+1]=rm3;
				break;
			case 3:
				lm3=min(lm2, cm2);
				rm3=max(rm2, cm2);
//				lm3=((lm2-la)>>1)+la;
//				rm3=2*rm2-ra;
//				lm3=(lm3<(-32768))?(-32768):((lm3<32768)?lm3:32767);
//				rm3=(rm3<(-32768))?(-32768):((rm3<32768)?rm3:32767);
				oblk[i*4+0]=lm3;
				oblk[i*4+1]=rm3;
				break;
			}
		}else
		{
			s0=(cn-cm);
			cm2=((s0*p0+8)>>4)+cm;

			cm2=(cm2<cm)?cm:((cm2<cn)?cm2:cn);

			oblk[i*2+0]=cm2;
		}
	}

#if 1
	if(flags&BGBTAC_EFL_HALFMONO)
	{
		for(i=0; i<32; i++)
		{
			p1=oblk[i*4+0];
			p0=(i>0)?oblk[i*4-4]:p1;
			p2=(i<31)?oblk[i*4+4]:p1;
			p3=(i<30)?oblk[i*4+8]:p2;

			p3=(3*p1+3*p2-p0-p3+2)>>2;
			p3=(p3<(-32768))?(-32768):((p3<32768)?p3:32767);

			oblk[i*4+2]=p3;
		}
	}else if(flags&BGBTAC_EFL_STEREO)
	{
		for(i=0; i<32; i++)
			for(j=0; j<2; j++)
		{
			p1=oblk[i*4+0+j];
			p0=(i>0)?oblk[i*4-4+j]:p1;
			p2=(i<31)?oblk[i*4+4+j]:p1;
			p3=(i<30)?oblk[i*4+8+j]:p2;

//			s0=2*p1-p0;
//			s1=2*p2-p3;
//			p3=(s0+s1)>>1;
//			p3=(s0+s1+p1+p2)>>2;

			p3=(3*p1+3*p2-p0-p3+2)>>2;

//			p3=(p1+p2)-p0;
			p3=(p3<(-32768))?(-32768):((p3<32768)?p3:32767);

			oblk[i*4+2+j]=p3;
		}
	}else
	{
		for(i=0; i<32; i++)
		{
			p1=oblk[i*2+0];
			p0=(i>0)?oblk[i*2-2]:p1;
			p2=(i<31)?oblk[i*2+2]:p1;
			p3=(i<30)?oblk[i*2+4]:p2;

			p3=(3*p1+3*p2-p0-p3+2)>>2;
			p3=(p3<(-32768))?(-32768):((p3<32768)?p3:32767);

			oblk[i*2+1]=p3;
		}
	}
#endif

#if 0
	if(flags&BGBTAC_EFL_STEREO)
	{
		for(i=0; i<32; i++)
		{
			lm2=oblk[i*4+0];
			rm2=oblk[i*4+1];
			ln2=oblk[((i+1)&31)*4+0];
			rn2=oblk[((i+1)&31)*4+1];

			p0=(iblk[8+(i>>2)]>>((i&3)*2))&3;

			s0=(ln2-lm2);
			s1=(rn2-rm2);
			p2=((s0*p0+1)>>2)+lm2;
			p3=((s1*p0+1)>>2)+rm2;

//			p2=lm2;
//			p3=rm2;

			oblk[i*4+2]=p2;
			oblk[i*4+3]=p3;
		}
	}else
	{
		for(i=0; i<32; i++)
		{
			cm2=oblk[i*2+0];
			cn2=oblk[((i+1)&31)*2+0];

			p0=(iblk[8+(i>>2)]>>((i&3)*2))&3;

			s0=(cn2-cm2);
			p2=((s0*p0+1)>>2)+cm2;

			oblk[i*2+1]=p2;
		}
	}
#endif
}

#if 0
void BGBTAC_EncodeBlock11StereoS16(s16 *iblk, byte *oblk, int flags)
{
	s16 tcblk[64];
	int tcma[32];
	int tcmb[32];
	int lm, ln, cm, rm, rn, cn, tl, tr, tc;
	int cm2, cn2, ca2, ba;
	int p0, p1, p2, p3;
	int s0, s1;
	int i, j, k;

	lm= 99999; rm= 99999; cm= 99999;
	ln=-99999; rn=-99999; cn=-99999;
	
	ba=0;
	for(i=0; i<64; i++)
	{
		tl=iblk[i*2+0];
		tr=iblk[i*2+1];
		k=(tl+tr)>>1;
		tcblk[i]=k;
		ba+=tl-k;
		if(k<cm) { lm=tl; rm=tr; cm=k; }
		if(k>cn) { ln=tl; rn=tr; cn=k; }
	}

	ba/=64;
	
	for(i=0; i<32; i++)
		{ oblk[i]=0; }

	p0=lm-cm;
	p1=ln-cn;
	p2=ba;

	p0=(5*p0+11*p2)>>4;
	p1=(5*p1+11*p2)>>4;

	p0=bgbtac_spowi(p0, 0.73);
	p1=bgbtac_spowi(p1, 0.73);

	p0=bgbtac_ssqrti(p0);
	p1=bgbtac_ssqrti(p1);

	p0=(p0<(-128))?(-128):((p0<128)?p0:127);
	p1=(p1<(-128))?(-128):((p1<128)?p1:127);
	
	oblk[0]=0x02;
	oblk[1]=0x00;
	oblk[2]=cm&0xFF;
	oblk[3]=(cm>>8)&0xFF;
	oblk[4]=cn&0xFF;
	oblk[5]=(cn>>8)&0xFF;
	oblk[6]=p0&0xFF;
	oblk[7]=p1&0xFF;
	
	for(i=0; i<16; i++)
	{
		cm2= 99999; cn2=-99999;
//		tc=tcblk[i*2+0];
//		tc=(tcblk[i*2+0]+tcblk[i*2+1])>>1;
		tc=(tcblk[i*4+0]+tcblk[i*4+1]+tcblk[i*4+2]+tcblk[i*4+3])>>2;
		
		if(cn!=cm)
			{ s0=16777216/(cn-cm); }
		else { s0=0; }

		p0=(s0*(tc-cm)+8192)>>12;
		p0=(p0<0)?0:((p0<4096)?p0:4095);
		tcma[i]=p0;
	}

	for(i=0; i<16; i++)
	{
		j=i*12;
		k=tcma[i]<<(j&7);

		oblk[8+(j>>3)+0]|=k;
		oblk[8+(j>>3)+1]|=k>>8;
		oblk[8+(j>>3)+2]|=k>>16;
	}
}

void BGBTAC_DecodeBlock11StereoS16(byte *iblk, s16 *oblk, int flags)
{
	int p0, p1, p2, p3;
	int s0, s1, s2, s3;
	int cm, cn, ca;
	int lm, ln, la;
	int rm, rn, ra;
	int cm2, cn2, ca2;
	int lm2, ln2, la2;
	int rm2, rn2, ra2;
	int i, j, k;
	
	cm=(s16)(iblk[2]+(iblk[3]<<8));
	cn=(s16)(iblk[4]+(iblk[5]<<8));
	ca=(cm+cn)>>1;

	i=bgbtac_ssqri((s8)iblk[6]);
	j=bgbtac_ssqri((s8)iblk[7]);

	lm=cm+i; rm=cm-i;
	ln=cn+j; rn=cn-j;
	la=(lm+ln)>>1;
	ra=(rm+rn)>>1;

	lm=(lm<(-32768))?(-32768):((lm<32768)?lm:32767);
	ln=(ln<(-32768))?(-32768):((ln<32768)?ln:32767);
	rm=(rm<(-32768))?(-32768):((rm<32768)?rm:32767);
	rn=(rn<(-32768))?(-32768):((rn<32768)?rn:32767);

//	for(i=0; i<64; i++)
//	{
//		oblk[i*2+0]=la;
//		oblk[i*2+1]=ra;
//	}
		
	for(i=0; i<16; i++)
	{
		j=i*12;
		
		k=iblk[8+(j>>3)+0] | (iblk[8+(j>>3)+1]<<8) |
			(iblk[8+(j>>3)+2]<<16);
		p0=(k>>(j&7))&4095;
		
//		p0=(iblk[16+(i>>1)]>>((i&1)*4))&15;
	
		s0=(ln-lm);
		s1=(rn-rm);
		lm2=((s0*p0+2048)>>12)+lm;
		rm2=((s1*p0+2048)>>12)+rm;

		lm2=(lm2<lm)?lm:((lm2<ln)?lm2:ln);
		rm2=(rm2<rm)?rm:((rm2<rn)?rm2:rn);

		oblk[i*8+0]=lm2;
		oblk[i*8+1]=rm2;
		oblk[i*8+2]=lm2;
		oblk[i*8+3]=rm2;
		oblk[i*8+4]=lm2;
		oblk[i*8+5]=rm2;
		oblk[i*8+6]=lm2;
		oblk[i*8+7]=rm2;
	}

#if 0
	for(i=0; i<31; i++)
	{
		p0=oblk[i*4+2];		p1=oblk[i*4+3];
		p2=oblk[i*4+4];		p3=oblk[i*4+5];

		p0=(p0+p2)>>1;
		p1=(p1+p3)>>1;

		oblk[i*4+2]=p0;
		oblk[i*4+3]=p1;
	}
#endif

#if 0
	for(i=0; i<32; i++)
		for(j=0; j<2; j++)
	{
		p0=(i>0)?oblk[i*4-4+j]:0;
		p1=oblk[i*4+0+j];
		p2=(i<31)?oblk[i*4+4+j]:p1;
		p3=(i<30)?oblk[i*4+8+j]:p2;

//		s0=2*p1-p0;
//		s1=2*p2-p3;
//		p3=(s0+s1)>>1;
//		p3=(s0+s1+p1+p2)>>2;

		p3=(3*p1+3*p2-p0-p3+2)>>2;

//		p3=(p1+p2)-p0;
		p3=(p3<(-32768))?(-32768):((p3<32768)?p3:32767);

		oblk[i*4+2+j]=p3;
	}
#endif

#if 1
	for(i=0; i<16; i++)
		for(j=0; j<2; j++)
	{
		p0=(i>0)?oblk[i*8-8+j]:0;
		p1=oblk[i*8+0+j];
		p2=(i<15)?oblk[i*8+8+j]:p1;
		p3=(i<14)?oblk[i*8+16+j]:p2;

		s0=2*p1-p0;
		s1=2*p2-p3;
		s2=(p1+s0)>>1;
		s3=(p2+s1)>>1;

		s2=(s2<(-32768))?(-32768):((s2<32768)?s2:32767);
		s3=(s3<(-32768))?(-32768):((s3<32768)?s3:32767);

		oblk[i*8+2+j]=s2;
		oblk[i*8+4+j]=(s2+s3)>>1;
		oblk[i*8+6+j]=s3;
	}
#endif

}
#endif

#if 1
void BGBTAC_EncodeBlock13StereoS16(s16 *iblk, byte *oblk, int flags)
{
	s16 tcblk[64];
	int tcma[16], tcna[16];
	int tcmb[16], tcnb[16];
//	int lm, ln, cm, rm, rn, cn, tl, tr, tc;
	int cm, cn, tc;
	int cm2, cn2, ca2, ba, cm2a, cn2a;
	int p0, p1, p2, p3;
	int s0, s1;
	int i, j, k;

	BGBTAC_EncodeBlockHeaderStereoS16(iblk, oblk, tcblk, &cm, &cn, flags);

	oblk[0]=0x04;
	oblk[1]=0x00;

	for(i=0; i<8; i++)
	{
		cm2= 99999; cn2=-99999;
		for(j=0; j<8; j++)
		{
			tc=tcblk[i*8+j];
			if(tc<cm2) { cm2=tc; }
			if(tc>cn2) { cn2=tc; }
		}

#if 1
		if(cn!=cm)
			{ s0=268435456/(cn-cm); }
		else { s0=0; }

		p0=(s0*(cm2-cm)+8388608)>>24;
		p1=(s0*(cn2-cm)+8388608)>>24;
#endif

		p0=(p0<0)?0:((p0<16)?p0:15);
		p1=(p1<0)?0:((p1<16)?p1:15);
		
		tcma[i]=p0;
		tcna[i]=p1;

		s1=(cn-cm);
		cm2=((s1*p0+8)>>4)+cm;
		cn2=((s1*p1+8)>>4)+cm;
		ca2=(cm2+cn2)>>1;

		cm2=(13*cm2+3*ca2+8)>>4;
		cn2=(13*cn2+3*ca2+8)>>4;

		tcmb[i]=cm2;
		tcnb[i]=cn2;
	}

	for(i=0; i<8; i++)
	{
		oblk[ 8+(i>>1)]|=tcma[i]<<((i&1)*4);
		oblk[12+(i>>1)]|=tcna[i]<<((i&1)*4);
	}
	
	for(i=0; i<8; i++)
	{
		cm2=tcmb[i];
		cn2=tcnb[i];
		ca2=(cm2+cn2)>>1;

		cm2a=(11*cm2+5*cn2+8)>>4;
		cn2a=(11*cn2+5*cm2+8)>>4;

		for(j=0; j<8; j++)
		{
			k=i*8+j;
			tc=tcblk[k];
			
//			p0=0; s0=abs(tc-cm2);
//			s1=abs(tc-cn2);
//			if(s1<s0) { p0=1; s0=s1; }
//			s1=abs(tc-ca2);
//			if(s1<s0) { p0=2; s0=s1; }

			p0=0; s0=abs(tc-cm2);
			s1=abs(tc-cn2);
			if(s1<s0) { p0=3; s0=s1; }
			s1=abs(tc-cm2a);
			if(s1<s0) { p0=1; s0=s1; }
			s1=abs(tc-cn2a);
			if(s1<s0) { p0=2; s0=s1; }
			
			oblk[16+(k>>2)]|=p0<<((k&3)*2);
		}
	}	
}

void BGBTAC_DecodeBlock13StereoS16(byte *iblk, s16 *oblk, int flags)
{
//	int tcma[16], tcna[16];
//	int tcmb[16], tcnb[16];
	int p0, p1, p2, p3;
	int s0, s1;
	int cm, cn, ca;
	int lm, ln, la;
	int rm, rn, ra;
	int cm2, cn2, ca2;
	int lm2, ln2, la2, lm2a, ln2a;
	int rm2, rn2, ra2, rm2a, rn2a;
	int i, j, k;
	
	cm=(s16)(iblk[2]+(iblk[3]<<8));
	cn=(s16)(iblk[4]+(iblk[5]<<8));
	ca=(cm+cn)>>1;

	i=bgbtac_ssqri((s8)iblk[6]);
	j=bgbtac_ssqri((s8)iblk[7]);

	lm=cm+i; rm=cm-i;
	ln=cn+j; rn=cn-j;
	la=(lm+ln)>>1;
	ra=(rm+rn)>>1;

	lm=(lm<(-32768))?(-32768):((lm<32768)?lm:32767);
	ln=(ln<(-32768))?(-32768):((ln<32768)?ln:32767);
	rm=(rm<(-32768))?(-32768):((rm<32768)?rm:32767);
	rn=(rn<(-32768))?(-32768):((rn<32768)?rn:32767);

//	for(i=0; i<64; i++)
//	{
//		oblk[i*2+0]=la;
//		oblk[i*2+1]=ra;
//	}
		
	for(i=0; i<8; i++)
	{
		p0=(iblk[ 8+(i>>1)]>>((i&1)*4))&15;
		p1=(iblk[12+(i>>1)]>>((i&1)*4))&15;
	
		if(flags&(BGBTAC_EFL_STEREO|BGBTAC_EFL_HALFMONO))
		{
			s0=(ln-lm);
			s1=(rn-rm);
			lm2=((s0*p0+8)>>4)+lm;
			ln2=((s0*p1+8)>>4)+lm;
			rm2=((s1*p0+8)>>4)+rm;
			rn2=((s1*p1+8)>>4)+rm;
			la2=(lm2+ln2)>>1;
			ra2=(rm2+rn2)>>1;

			lm2=(13*lm2+3*la2+8)>>4;
			ln2=(13*ln2+3*la2+8)>>4;
			rm2=(13*rm2+3*ra2+8)>>4;
			rn2=(13*rn2+3*ra2+8)>>4;

			lm2=(lm2<lm)?lm:((lm2<ln)?lm2:ln);
			ln2=(ln2<lm)?lm:((ln2<ln)?ln2:ln);
			rm2=(rm2<rm)?rm:((rm2<rn)?rm2:rn);
			rn2=(rn2<rm)?rm:((rn2<rn)?rn2:rn);
			
			lm2a=(11*lm2+5*ln2+8)>>4;
			ln2a=(11*ln2+5*lm2+8)>>4;
			rm2a=(11*rm2+5*rn2+8)>>4;
			rn2a=(11*rn2+5*rm2+8)>>4;
		}else
		{
			cm2=((s1*p0+8)>>4)+cm;
			cn2=((s1*p1+8)>>4)+cm;
			ca2=(cm2+cn2)>>1;

//			cm2=(11*cm2+5*ca2+8)>>4;
//			cn2=(11*cn2+5*ca2+8)>>4;
			cm2=(13*cm2+3*ca2+8)>>4;
			cn2=(13*cn2+3*ca2+8)>>4;

			cm2=(cm2<cm)?cm:((cm2<cn)?cm2:cn);
			cn2=(cn2<cm)?cm:((cn2<cn)?cn2:cn);
		}

		if(flags&BGBTAC_EFL_HALFMONO)
		{
			for(j=0; j<8; j++)
			{
				k=i*8+j;
		
				p0=(iblk[16+(k>>2)]>>((k&3)*2))&3;
				
				switch(p0)
				{
				case 0: oblk[k*2+0]=lm2; break;
				case 1: oblk[k*2+0]=lm2a; break;
				case 2: oblk[k*2+0]=ln2a; break;
				case 3: oblk[k*2+0]=ln2; break;
				}
			}
		}else if(flags&BGBTAC_EFL_STEREO)
		{
			for(j=0; j<8; j++)
			{
				k=i*8+j;
		
				p0=(iblk[16+(k>>2)]>>((k&3)*2))&3;
				
				switch(p0)
				{
				case 0:
					oblk[k*2+0]=lm2;
					oblk[k*2+1]=rm2;
					break;
//				case 1:
//					oblk[k*2+0]=ln2;
//					oblk[k*2+1]=rn2;
//					break;
//				case 2:
//					oblk[k*2+0]=la2;
//					oblk[k*2+1]=ra2;
//					break;
				case 1:
					oblk[k*2+0]=lm2a;
					oblk[k*2+1]=rm2a;
					break;
				case 2:
					oblk[k*2+0]=ln2a;
					oblk[k*2+1]=rn2a;
					break;
				case 3:
					oblk[k*2+0]=ln2;
					oblk[k*2+1]=rn2;
					break;
				}
			}
		}else
		{
			for(j=0; j<8; j++)
			{
				k=i*8+j;
		
				p0=(iblk[16+(k>>2)]>>((k&3)*2))&3;
				
				switch(p0)
				{
				case 0: oblk[k]=lm2; break;
				case 1: oblk[k]=lm2a; break;
				case 2: oblk[k]=ln2a; break;
				case 3: oblk[k]=ln2; break;
				}
			}
		}
	}
}
#endif


void BGBTAC_EncodeBlockTagStereoS16(s16 *iblk, byte *oblk,
	int tag, int flags)
{
	switch(tag)
	{
	case 0:
		BGBTAC_EncodeBlock8StereoS16(iblk, oblk, flags);
		break;
	case 1:
		BGBTAC_EncodeBlock9StereoS16(iblk, oblk, flags);
		break;
	case 2:
		BGBTAC_EncodeBlock10StereoS16(iblk, oblk, flags);
		break;
	case 3:
		BGBTAC_EncodeBlock12StereoS16(iblk, oblk, flags);
		break;
	case 4:
		BGBTAC_EncodeBlock13StereoS16(iblk, oblk, flags);
		break;
	}
}

void BGBTAC_DecodeBlockTagStereoS16(byte *iblk, s16 *oblk, int flags)
{
	switch(iblk[0]&15)
	{
	case 0:
		BGBTAC_DecodeBlock8StereoS16(iblk, oblk, flags);
		break;
	case 1:
		BGBTAC_DecodeBlock9StereoS16(iblk, oblk, flags);
		break;
	case 2:
		BGBTAC_DecodeBlock10StereoS16(iblk, oblk, flags);
		break;
	case 3:
		BGBTAC_DecodeBlock12StereoS16(iblk, oblk, flags);
		break;
	case 4:
		BGBTAC_DecodeBlock13StereoS16(iblk, oblk, flags);
		break;
	}
}

BTAC_API int BGBTAC_CalcBlockErrorStereoS16(
	s16 *iblk, s16 *tblk, int flags)
{
	double e;
	int ld, rd, cd, c0, c1;
	int i;
	
	e=0;
	for(i=0; i<64; i++)
	{
		ld=iblk[i*2+0]-tblk[i*2+0];
		rd=iblk[i*2+1]-tblk[i*2+1];
		c0=(iblk[i*2+0]+iblk[i*2+1])>>1;
		c1=(tblk[i*2+0]+tblk[i*2+1])>>1;
		cd=c1-c0;
		e+=ld*ld;	e+=rd*rd;
		e+=cd*cd;
		e+=cd*cd;
	}
	
	i=sqrt(e/64);
	return(i);
}

BTAC_API int BGBTAC_CheckBlockBestStereoS16(s16 *iblk, int flags)
{
	s16 eblk[128];
	byte tblk[32];
	int bn, be, te;
	int i;
	
	bn=0; be=999999999;
	for(i=0; i<5; i++)
	{
		BGBTAC_EncodeBlockTagStereoS16(iblk, tblk, i, flags);
		BGBTAC_DecodeBlockTagStereoS16(tblk, eblk, flags);
		te=BGBTAC_CalcBlockErrorStereoS16(iblk, eblk, flags);
		if(te<be) { bn=i; be=te; }
	}
	
	return(bn);
}


BTAC_API void BGBTAC_EncodeStream9StereoS16(
	s16 *iblk, byte *oblk, int len, int flags)
{
	int tcnt[16];
	int i, j, n, tag, ltag, rtag, fl2;
	
	for(i=0; i<16; i++)
		{ tcnt[i]=0; }
	
	n=(len+63)/64;
	for(i=0; i<n; i++)
	{
//		BGBTAC_EncodeBlock9StereoS16(iblk+i*2*64, oblk+i*32);
//		BGBTAC_EncodeBlock10StereoS16(iblk+i*2*64, oblk+i*32);

		fl2=flags;
		if((i>0) && i<(n-1))
			fl2|=BGBTAC_EFL_MIDSTREAM;

		if((flags&BGBTAC_EFL_STEREO) && (flags&BGBTAC_EFL_HALFMONO))
		{
//			tag=0;
//			tag=2;	//2, 3
//			tag=BGBTAC_CheckBlockBestStereoS16(
//				iblk+i*2*64+0, flags&(~BGBTAC_EFL_HALFMONO));

			ltag=BGBTAC_CheckBlockBestStereoS16(iblk+i*2*64+0, flags);
			rtag=BGBTAC_CheckBlockBestStereoS16(iblk+i*2*64+1, flags);

			BGBTAC_EncodeBlockTagStereoS16(
				iblk+i*2*64+0, oblk+i*64+ 0, ltag, fl2);
			BGBTAC_EncodeBlockTagStereoS16(
				iblk+i*2*64+1, oblk+i*64+32, rtag, fl2);
//			tcnt[tag]++;

			tcnt[ltag]++;
			tcnt[rtag]++;
		}else
		{
//			tag=0;
//			tag=1;
//			tag=4;
			tag=BGBTAC_CheckBlockBestStereoS16(iblk+i*2*64, flags);
//			BGBTAC_EncodeBlockTagStereoS16(iblk+i*2*64,
//				oblk+i*32, tag, flags);
			BGBTAC_EncodeBlockTagStereoS16(iblk+i*2*64, oblk+i*32, tag, fl2);
		
//			printf("%d", tag);
			tcnt[tag]++;
		}
	}

//	printf("\n");
	
	printf("Encode %d: ", n);
	for(i=0; i<16; i++)
		printf("%d ", tcnt[i]);
	printf("\n");
}

BTAC_API void BGBTAC_DecodeStream9StereoS16(
	byte *iblk, s16 *oblk, int len, int flags)
{
	int p0, p1, p2, p3;
	int q0, q1, q2, q3;
	float f, g;
	int i, j, k, n;
	
	n=(len+63)/64;
	for(i=0; i<n; i++)
	{
		if((flags&BGBTAC_EFL_STEREO) && (flags&BGBTAC_EFL_HALFMONO))
		{
			BGBTAC_DecodeBlockTagStereoS16(
				iblk+i*64+ 0, oblk+i*2*64+0, flags);
			BGBTAC_DecodeBlockTagStereoS16(
				iblk+i*64+32, oblk+i*2*64+1, flags);
		}else
		{
//			BGBTAC_DecodeBlock9StereoS16(iblk+i*32, oblk+i*2*64, flags);
//			BGBTAC_DecodeBlock10StereoS16(iblk+i*32, oblk+i*2*64, flags);
			BGBTAC_DecodeBlockTagStereoS16(iblk+i*32, oblk+i*2*64, flags);
		}
	}

	if((flags&BGBTAC_EFL_STEREO) && (flags&BGBTAC_EFL_HALFMONO))
	{
		BGBTAC_DeblockStream8StereoS16(oblk, len);
		BGBTAC_FilterStream8StereoS16(oblk, len);
	}else
	{
		BGBTAC_DeblockStream8StereoS16(oblk, len);
		BGBTAC_FilterStream8StereoS16(oblk, len);
	}
}

