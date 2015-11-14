#include <pdgl.h>

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
    8 bit left-center min (ssqrt)
    8 bit left-center max (ssqrt)
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
	8 bit left-center min (ssqrt)
	8 bit left-center max (ssqrt)
	32x 2-bit sample (64 bits)
	32x 4-bit sample (128 bits)

This directly codes all samples, with the 4-bit values encoding even samples, and the 2-bit values encoding odd samples.

The 4-bit samples are encoded between the block min/max values, and the 2-bit samples between the prior/next sample.

Sample interpolation, 2 bit samples:
0=prior sample, 1=next sample, 2=average, 3=quadratic interpolated value.

Basic Format 2:
	4 bit: Block-Mode (2)
	currently unused (12 bits, zeroed)
	16 bit min sample (center)
	16 bit max sample (center)
	8 bit left-center min (ssqrt)
	8 bit left-center max (ssqrt)
	32x 6-bit samples (192 bits)

This directly codes samples, with the 6-bit values encoding samples.
The 6-bit samples are encoded between the block min/max values.

This mode encodes even samples, with odd-samples being interpolated (quadratic).
The last sample is extrapolated.

Stereo Format 3:
	4 bit: Block-Mode (3)
	currently unused (12 bits, zeroed)
	16 bit min sample (center)
	16 bit max sample (center)
	8 bit left-center min (ssqrt)
	8 bit left-center max (ssqrt)
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

This mode encodes even samples, with odd-samples being interpolated (quadratic).

Basic Format 4:
	4 bit: Block-Mode (4)
	currently unused (12 bits, zeroed)
	16 bit min sample (center)
	16 bit max sample (center)
	8 bit left-center min (ssqrt)
	8 bit left-center max (ssqrt)
	8x 4-bit min (32 bits)
	8x 4-bit max (32 bits)
	64x 2-bit sample (128 bits)

The 4-bit values interpolate between the full min/max for the block.
The 2-bit samples interpolate between the min and max values for each sub-block (0=min, 1=1/3, 2=2/3, 3=max).

 */

int pdgl_btac_ssqrti(int v)
{
	int i;
	double f;
	f=(v>=0)?sqrt(v):-sqrt(-v);
	i=(int)(f+0.5);
	return(i);
}

int pdgl_btac_ssqri(int v)
{
	return((v>=0)?(v*v):(-v*v));
}

int pdgl_btac_spowi(int v, float e)
{
	int i;
	double f;
	f=(v>=0)?pow(v, e):-pow(-v, e);
	i=(int)(f+0.5);
	return(i);
}

double pdgl_btac_ssqrt(double v)
{
	double f;
	f=(v>=0)?sqrt(v):-sqrt(-v);
	return(f);
}

double pdgl_btac_ssqr(double v)
{
	return((v>=0)?(v*v):(-v*v));
}

void PDGL_BTAC_EncodeBlockHeaderS16(
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
		if(flags&PDGL_BTAC_EFL_HALFMONO)
			{ tl=iblk[i*2+0]; tr=tl; }
		else if(flags&PDGL_BTAC_EFL_STEREO)
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
	
	if(flags&PDGL_BTAC_EFL_MIDSTREAM)
	{
		ba=0;
		for(i=-64; i<128; i++)
		{
			if(flags&PDGL_BTAC_EFL_HALFMONO)
				{ tl=iblk[i*2+0]; tr=tl; }
			else if(flags&PDGL_BTAC_EFL_STEREO)
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

	if(flags&(PDGL_BTAC_EFL_CENTERMONO|PDGL_BTAC_EFL_HALFMONO))
		{ p0=0; p1=0; }

//	p0/=2;	p1/=2;
	p0=pdgl_btac_spowi(p0, 0.73);
	p1=pdgl_btac_spowi(p1, 0.73);

//	p0=pdgl_btac_spowi(p0, 0.90);
//	p1=pdgl_btac_spowi(p1, 0.90);

//	p0=pdgl_btac_spowi(p0, 0.80);
//	p1=pdgl_btac_spowi(p1, 0.80);

	p0=pdgl_btac_ssqrti(p0);
	p1=pdgl_btac_ssqrti(p1);

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

void PDGL_BTAC_EncodeBlockHeaderPanS16(
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
		if(flags&PDGL_BTAC_EFL_HALFMONO)
		{
			tl=iblk[i*2+0];
			tr=tl;
		}else if(flags&PDGL_BTAC_EFL_STEREO)
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

	if((flags&PDGL_BTAC_EFL_CENTERMONO) || (flags&PDGL_BTAC_EFL_HALFMONO))
		{ p0=0; p1=0; }

//	p0/=2;	p1/=2;
	p0=pdgl_btac_spowi(p0, 0.73);
	p1=pdgl_btac_spowi(p1, 0.73);

//	p0=pdgl_btac_spowi(p0, 0.90);
//	p1=pdgl_btac_spowi(p1, 0.90);

//	p0=pdgl_btac_spowi(p0, 0.80);
//	p1=pdgl_btac_spowi(p1, 0.80);

	p0=pdgl_btac_ssqrti(p0);
	p1=pdgl_btac_ssqrti(p1);

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
void PDGL_BTAC_EncodeBlock8S16(s16 *iblk, byte *oblk, int flags)
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

	PDGL_BTAC_EncodeBlockHeaderS16(iblk, oblk, tcblk, &cm, &cn, flags);

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
			{ s0=268435456/(cn-cm); }
		else { s0=0; }

		p0=(s0*(cm2-cm)+8388608)>>24;
		p1=(s0*(cn2-cm)+8388608)>>24;

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

void PDGL_BTAC_DecodeBlockHeader(byte *iblk, int flags,
	int *rcm, int *rcn, int *rca,
	int *rlm, int *rln, int *rla,
	int *rrm, int *rrn, int *rra)
{
	int cm, cn, ca;
	int lm, ln, la;
	int rm, rn, ra;
	int i, j;

	cm=(s16)(iblk[2]+(iblk[3]<<8));
	cn=(s16)(iblk[4]+(iblk[5]<<8));
	ca=(cm+cn)>>1;

	i=pdgl_btac_ssqri((s8)iblk[6]);
	j=pdgl_btac_ssqri((s8)iblk[7]);

	lm=cm+i; rm=cm-i;
	ln=cn+j; rn=cn-j;
	la=(lm+ln)>>1;
	ra=(rm+rn)>>1;

	lm=(lm<(-32768))?(-32768):((lm<32768)?lm:32767);
	ln=(ln<(-32768))?(-32768):((ln<32768)?ln:32767);
	rm=(rm<(-32768))?(-32768):((rm<32768)?rm:32767);
	rn=(rn<(-32768))?(-32768):((rn<32768)?rn:32767);
	
	*rcm=cm; *rcn=cn; *rca=ca;
	*rlm=lm; *rln=ln; *rla=la;
	*rrm=rm; *rrn=rn; *rra=ra;
}

void PDGL_BTAC_DecodeBlock8S16(byte *iblk, s16 *oblk, int flags)
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

	PDGL_BTAC_DecodeBlockHeader(iblk, flags,
		&cm, &cn, &ca, &lm, &ln, &la, &rm, &rn, &ra);

	for(i=0; i<16; i++)
	{
		p0=(iblk[16+(i>>1)]>>((i&1)*4))&15;
		p1=(iblk[24+(i>>1)]>>((i&1)*4))&15;

		if(flags&(PDGL_BTAC_EFL_STEREO|PDGL_BTAC_EFL_HALFMONO))
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
		}else
		{
			cm2=((s1*p0+8)>>4)+cm;
			cn2=((s1*p1+8)>>4)+cm;
			ca2=(cm2+cn2)>>1;

			cm2=(13*cm2+3*ca2+8)>>4;
			cn2=(13*cn2+3*ca2+8)>>4;

			cm2=(cm2<cm)?cm:((cm2<cn)?cm2:cn);
			cn2=(cn2<cm)?cm:((cn2<cn)?cn2:cn);
		}

		for(j=0; j<4; j++)
		{
			k=i*4+j;
		
			p0=(iblk[8+(k>>3)]&(1<<(k&7)));
			if(flags&PDGL_BTAC_EFL_HALFMONO)
				{ oblk[k*2+0]=p0?ln2:lm2; }
			else if(flags&PDGL_BTAC_EFL_STEREO)
			{
				oblk[k*2+0]=p0?ln2:lm2;
				oblk[k*2+1]=p0?rn2:rm2;
			}else
			{
				oblk[k]=p0?cn2:cm2;
			}
		}
	}
}
#endif

void PDGL_BTAC_EncodeBlock9S16(s16 *iblk, byte *oblk, int flags)
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

	PDGL_BTAC_EncodeBlockHeaderS16(iblk, oblk, tcblk, &cm, &cn, flags);

	oblk[0]=0x01;
	oblk[1]=0x00;
	
	for(i=0; i<32; i++)
	{
		cm2= 99999; cn2=-99999;
		tc=tcblk[i*2+0];

		if(cn!=cm)
			{ s0=268435456/(cn-cm); }
		else { s0=0; }
		p0=(s0*(tc-cm)+8388608)>>24;
		p0=(p0<0)?0:((p0<16)?p0:15);
		tcma[i]=p0;

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

		oblk[8+(i>>2)]|=k<<((i&3)*2);
	}
#endif
}

void PDGL_BTAC_DecodeBlock9S16(byte *iblk, s16 *oblk, int flags)
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

	PDGL_BTAC_DecodeBlockHeader(iblk, flags,
		&cm, &cn, &ca, &lm, &ln, &la, &rm, &rn, &ra);

	for(i=0; i<32; i++)
	{
		p0=(iblk[16+(i>>1)]>>((i&1)*4))&15;
	
		if(flags&PDGL_BTAC_EFL_HALFMONO)
		{
			s0=(cn-cm);
			cm2=((s0*p0+8)>>4)+cm;
			cm2=(cm2<cm)?cm:((cm2<cn)?cm2:cn);
			oblk[i*4+0]=cm2;
		}
		else if(flags&PDGL_BTAC_EFL_STEREO)
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

#if 1
	if(flags&PDGL_BTAC_EFL_HALFMONO)
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
	}else if(flags&PDGL_BTAC_EFL_STEREO)
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

void PDGL_BTAC_EncodeBlock10S16(s16 *iblk, byte *oblk, int flags)
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

	PDGL_BTAC_EncodeBlockHeaderS16(iblk, oblk, tcblk, &cm, &cn, flags);

	oblk[0]=0x02;
	oblk[1]=0x00;

	for(i=0; i<32; i++)
	{
		cm2= 99999; cn2=-99999;
		tc=tcblk[i*2+0];

		if(cn!=cm)
			{ s0=268435456/(cn-cm); }
		else { s0=0; }
		p0=(s0*(tc-cm)+2097152)>>22;
		p0=(p0<0)?0:((p0<64)?p0:63);
		tcma[i]=p0;
	}

	for(i=0; i<32; i++)
	{
		j=i*6;
		k=tcma[i]<<(j&7);

		oblk[8+(j>>3)+0]|=k;
		oblk[8+(j>>3)+1]|=k>>8;
	}
}

void PDGL_BTAC_DecodeBlock10S16(byte *iblk, s16 *oblk, int flags)
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

	PDGL_BTAC_DecodeBlockHeader(iblk, flags,
		&cm, &cn, &ca, &lm, &ln, &la, &rm, &rn, &ra);

	for(i=0; i<32; i++)
	{
		j=i*6;
		
		k=iblk[8+(j>>3)+0] | (iblk[8+(j>>3)+1]<<8);
		p0=(k>>(j&7))&63;
		
		if(flags&PDGL_BTAC_EFL_HALFMONO)
		{
			s0=(ln-lm);
			lm2=((s0*p0+32)>>6)+lm;

			lm2=(lm2<lm)?lm:((lm2<ln)?lm2:ln);

			oblk[i*4+0]=lm2;
			oblk[i*4+2]=lm2;
		}else if(flags&PDGL_BTAC_EFL_STEREO)
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

#if 1
	if(flags&PDGL_BTAC_EFL_HALFMONO)
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
	if(flags&PDGL_BTAC_EFL_STEREO)
	{
		for(i=0; i<32; i++)
			for(j=0; j<2; j++)
		{
			p1=oblk[i*4+0+j];
			p0=(i>0)?oblk[i*4-4+j]:p1;
			p2=(i<31)?oblk[i*4+4+j]:p1;
			p3=(i<30)?oblk[i*4+8+j]:p2;

			p3=(3*p1+3*p2-p0-p3+2)>>2;
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

void PDGL_BTAC_EncodeBlock12S16(s16 *iblk, byte *oblk, int flags)
{
	s16 tcblk[64], tcpan[64];
	int tcma[32];
	int tcmb[32];
	int lm, ln, cm, rm, rn, cn, tl, tr, tc;
	int cm2, cn2, ca2, ba;
	int p0, p1, p2, p3;
	int s0, s1;
	int i, j, k;

	PDGL_BTAC_EncodeBlockHeaderPanS16(
		iblk, oblk, tcblk, tcpan,
		&cm, &cn, &lm, &ln, &rm, &rn,
		flags);

	oblk[0]=0x03;
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

void PDGL_BTAC_DecodeBlock12S16(byte *iblk, s16 *oblk, int flags)
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

	PDGL_BTAC_DecodeBlockHeader(iblk, flags,
		&cm, &cn, &ca, &lm, &ln, &la, &rm, &rn, &ra);

	for(i=0; i<32; i++)
	{
		p0=(iblk[16+(i>>1)]>>((i&1)*4))&15;
		p1=(iblk[8+(i>>2)]>>((i&3)*2))&3;
	
		if(flags&PDGL_BTAC_EFL_HALFMONO)
		{
			s0=(cn-cm);
			cm2=((s0*p0+8)>>4)+cm;

			cm2=(cm2<cm)?cm:((cm2<cn)?cm2:cn);

			oblk[i*4+0]=cm2;
			oblk[i*4+2]=cm2;
		}else if(flags&PDGL_BTAC_EFL_STEREO)
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
				oblk[i*4+0]=lm3;
				oblk[i*4+1]=rm3;
				break;
			case 3:
				lm3=min(lm2, cm2);
				rm3=max(rm2, cm2);
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
	if(flags&PDGL_BTAC_EFL_HALFMONO)
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
	}else if(flags&PDGL_BTAC_EFL_STEREO)
	{
		for(i=0; i<32; i++)
			for(j=0; j<2; j++)
		{
			p1=oblk[i*4+0+j];
			p0=(i>0)?oblk[i*4-4+j]:p1;
			p2=(i<31)?oblk[i*4+4+j]:p1;
			p3=(i<30)?oblk[i*4+8+j]:p2;

			p3=(3*p1+3*p2-p0-p3+2)>>2;
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

#if 1
void PDGL_BTAC_EncodeBlock13S16(s16 *iblk, byte *oblk, int flags)
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

	PDGL_BTAC_EncodeBlockHeaderS16(iblk, oblk, tcblk, &cm, &cn, flags);

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

//		cm2a=(11*cm2+5*cn2+8)>>4;
//		cn2a=(11*cn2+5*cm2+8)>>4;

		s0=cn2-cm2;
		cm2a=((22*s0*1+32)>>6)+cm2;
		cn2a=((22*s0*2+32)>>6)+cm2;

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

void PDGL_BTAC_DecodeBlock13S16(byte *iblk, s16 *oblk, int flags)
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

	PDGL_BTAC_DecodeBlockHeader(iblk, flags,
		&cm, &cn, &ca, &lm, &ln, &la, &rm, &rn, &ra);

	for(i=0; i<8; i++)
	{
		p0=(iblk[ 8+(i>>1)]>>((i&1)*4))&15;
		p1=(iblk[12+(i>>1)]>>((i&1)*4))&15;
	
		if(flags&(PDGL_BTAC_EFL_STEREO|PDGL_BTAC_EFL_HALFMONO))
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
		}else
		{
			cm2=((s1*p0+8)>>4)+cm;
			cn2=((s1*p1+8)>>4)+cm;
			ca2=(cm2+cn2)>>1;

			cm2=(13*cm2+3*ca2+8)>>4;
			cn2=(13*cn2+3*ca2+8)>>4;

			cm2=(cm2<cm)?cm:((cm2<cn)?cm2:cn);
			cn2=(cn2<cm)?cm:((cn2<cn)?cn2:cn);
		}

		if(flags&PDGL_BTAC_EFL_HALFMONO)
		{
			for(j=0; j<8; j++)
			{
				k=i*8+j;
		
				p0=(iblk[16+(k>>2)]>>((k&3)*2))&3;

				s0=ln2-lm2;
				p1=((22*s0*p0+32)>>6)+lm2;
				oblk[k*2+0]=p1;
			}
		}else if(flags&PDGL_BTAC_EFL_STEREO)
		{
			for(j=0; j<8; j++)
			{
				k=i*8+j;
		
				p0=(iblk[16+(k>>2)]>>((k&3)*2))&3;

				s0=ln2-lm2;
				s1=rn2-rm2;
				p1=((22*s0*p0+32)>>6)+lm2;
				p2=((22*s1*p0+32)>>6)+rm2;
				oblk[k*2+0]=p1;
				oblk[k*2+1]=p2;
			}
		}else
		{
			for(j=0; j<8; j++)
			{
				k=i*8+j;
		
				p0=(iblk[16+(k>>2)]>>((k&3)*2))&3;

				s0=cn2-cm2;
				p1=((22*s0*p0+32)>>6)+cm2;
				oblk[k]=p1;
			}
		}
	}
}
#endif


PDGL_API void PDGL_BTAC_FilterStreamMonoS16(s16 *buf, int len)
{
	int ta[2];
	int a, b, c, n;
	int i, j, k;
	
	a=0;
	
//	for(i=8; i<(len-8); i++)
	for(i=0; i<len; i++)
	{
		b=buf[i];
//		c=(a*5+b*11+8)>>4;
//		c=(a*3+b*13+8)>>4;

//		c=(a*80+b*176+128)>>8;
		c=(a*48+b*208+128)>>8;
		buf[i]=c;
		
		a=(b*128+a*128+128)>>8;
//		a=(b*96+a*160+128)>>8;
//		a=(b*80+a*176+128)>>8;
//		a=(b*48+a*208+128)>>8;
	}
}

PDGL_API void PDGL_BTAC_DeblockStreamMonoS16(s16 *oblk, int len)
{
	int p0, p1, p2, p3;
	int q0, q1, q2, q3;
	float f, g;
	int i, j, k, n;

	n=(len+63)/64;

#if 1
	//deblocking...
	for(i=1; i<(n-1); i++)
	{
#if 1
		p0=oblk[i*64-2];
		p1=oblk[i*64-1];
		p2=oblk[i*64+0];
		p3=oblk[i*64+1];

		q0=(128*p0+ 64*p1+ 48*p2+ 16*p3)>>8;
		q1=( 48*p0+128*p1+ 48*p2+ 32*p3)>>8;
		q2=( 32*p0+ 48*p1+128*p2+ 48*p3)>>8;
		q3=( 16*p0+ 48*p1+ 64*p2+128*p3)>>8;
		
		oblk[i*64-2]=q0;
		oblk[i*64-1]=q1;
		oblk[i*64+0]=q2;
		oblk[i*64+1]=q3;
#endif
	}
#endif
}

PDGL_API void PDGL_BTAC_FilterStreamS16(s16 *buf, int len)
{
	int ta[2];
	int a, b, c, n;
	int i, j, k;
	
	ta[0]=0; ta[1]=0;
	
//	for(i=8; i<(len-8); i++)
	for(i=0; i<len; i++)
		for(j=0; j<2; j++)
	{
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

PDGL_API void PDGL_BTAC_DeblockStreamS16(
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

void PDGL_BTAC_EncodeBlockTagS16(s16 *iblk, byte *oblk,
	int tag, int flags)
{
	switch(tag)
	{
	case 0:
		PDGL_BTAC_EncodeBlock8S16(iblk, oblk, flags);
		break;
	case 1:
		PDGL_BTAC_EncodeBlock9S16(iblk, oblk, flags);
		break;
	case 2:
		PDGL_BTAC_EncodeBlock10S16(iblk, oblk, flags);
		break;
	case 3:
		PDGL_BTAC_EncodeBlock12S16(iblk, oblk, flags);
		break;
	case 4:
		PDGL_BTAC_EncodeBlock13S16(iblk, oblk, flags);
		break;
	}
}

void PDGL_BTAC_DecodeBlockTagS16(byte *iblk, s16 *oblk, int flags)
{
	switch(iblk[0]&15)
	{
	case 0:
		PDGL_BTAC_DecodeBlock8S16(iblk, oblk, flags);
		break;
	case 1:
		PDGL_BTAC_DecodeBlock9S16(iblk, oblk, flags);
		break;
	case 2:
		PDGL_BTAC_DecodeBlock10S16(iblk, oblk, flags);
		break;
	case 3:
		PDGL_BTAC_DecodeBlock12S16(iblk, oblk, flags);
		break;
	case 4:
		PDGL_BTAC_DecodeBlock13S16(iblk, oblk, flags);
		break;
	}
}

void PDGL_BTAC_CompandBlockS16(s16 *iblk, s16 *oblk, int flags)
{
	int i, j;

	for(i=0; i<64; i++)
		for(j=0; j<2; j++)
	{
		oblk[i*2+j]=(int)(181.02*pdgl_btac_ssqrt(iblk[i*2+j]));
//		oblk[i*2+j]=(int)(128.0*pdgl_btac_ssqrt(iblk[i*2+j]));
//		oblk[i*2+j]=(int)(pdgl_btac_ssqrt(iblk[i*2+j]));
	}
}

void PDGL_BTAC_UnCompandBlockS16(s16 *iblk, s16 *oblk, int flags)
{
	int i, j;

	for(i=0; i<64; i++)
		for(j=0; j<2; j++)
	{
		oblk[i*2+j]=(int)(pdgl_btac_ssqr(iblk[i*2+j]/181.02));
//		oblk[i*2+j]=(int)(pdgl_btac_ssqr(iblk[i*2+j]/128.0));
//		oblk[i*2+j]=(int)(pdgl_btac_ssqr(iblk[i*2+j]));
	}
}

PDGL_API int PDGL_BTAC_CalcBlockErrorS16(
	s16 *iblk, s16 *tblk, int flags)
{
	double e;
	int ld, rd, cd, c0, c1;
	int i;

	if(flags&PDGL_BTAC_EFL_HALFMONO)
	{
		e=0;
		for(i=0; i<64; i++)
		{
			ld=iblk[i*2+0]-tblk[i*2+0];
			e+=ld*ld;
		}
	
		i=sqrt(e/64);
		return(i);
	}else if(flags&PDGL_BTAC_EFL_STEREO)
	{
		e=0;
		for(i=0; i<64; i++)
		{
			ld=iblk[i*2+0]-tblk[i*2+0];
			rd=iblk[i*2+1]-tblk[i*2+1];
			c0=(iblk[i*2+0]+iblk[i*2+1])>>1;
			c1=(tblk[i*2+0]+tblk[i*2+1])>>1;
			cd=c1-c0;
			e+=ld*ld;	e+=rd*rd;
			e+=cd*cd;	e+=cd*cd;
		}
	
		i=sqrt(e/64);
		return(i);
	}else 
	{
		e=0;
		for(i=0; i<64; i++)
		{
			ld=iblk[i]-tblk[i];
			e+=ld*ld;
		}
	
		i=sqrt(e/64);
		return(i);
	}
}

PDGL_API int PDGL_BTAC_CheckBlockBestS16(s16 *iblk, int flags)
{
	s16 eblk[128];
	byte tblk[32];
	int bn, be, te;
	int i;
	
	bn=0; be=999999999;
	for(i=0; i<5; i++)
	{
		PDGL_BTAC_EncodeBlockTagS16(iblk, tblk, i, flags);
		PDGL_BTAC_DecodeBlockTagS16(tblk, eblk, flags);
		te=PDGL_BTAC_CalcBlockErrorS16(iblk, eblk, flags);
		if(te<be) { bn=i; be=te; }
	}
	
	return(bn);
}


PDGL_API void PDGL_BTAC_EncodeStreamS16(
	s16 *iblk, byte *oblk, int len, int flags)
{
	s16 tblk[64*2];
	int tcnt[16];
	int i, j, n, tag, ltag, rtag, fl2;
	
	for(i=0; i<16; i++)
		{ tcnt[i]=0; }
	
	n=(len+63)/64;
	for(i=0; i<n; i++)
	{
		fl2=flags;
		if((i>0) && i<(n-1))
			fl2|=PDGL_BTAC_EFL_MIDSTREAM;

		if((flags&PDGL_BTAC_EFL_STEREO) && (flags&PDGL_BTAC_EFL_HALFMONO))
		{
//			tag=0;
//			tag=2;	//2, 3
//			tag=PDGL_BTAC_CheckBlockBestS16(
//				iblk+i*2*64+0, flags&(~PDGL_BTAC_EFL_HALFMONO));

			ltag=PDGL_BTAC_CheckBlockBestS16(iblk+i*2*64+0, flags);
			rtag=PDGL_BTAC_CheckBlockBestS16(iblk+i*2*64+1, flags);

			PDGL_BTAC_EncodeBlockTagS16(
				iblk+i*2*64+0, oblk+i*64+ 0, ltag, fl2);
			PDGL_BTAC_EncodeBlockTagS16(
				iblk+i*2*64+1, oblk+i*64+32, rtag, fl2);
//			tcnt[tag]++;

			tcnt[ltag]++;
			tcnt[rtag]++;
		}else if(flags&PDGL_BTAC_EFL_STEREO)
		{
//			tag=0;
//			tag=1;
//			tag=4;
			tag=PDGL_BTAC_CheckBlockBestS16(iblk+i*2*64, flags);
			PDGL_BTAC_EncodeBlockTagS16(iblk+i*2*64, oblk+i*32, tag, fl2);

//			PDGL_BTAC_CompandBlockS16(iblk+i*2*64, tblk, flags);
//			tag=PDGL_BTAC_CheckBlockBestS16(tblk, flags);
//			PDGL_BTAC_EncodeBlockTagS16(tblk, oblk+i*32, tag, fl2);
		
//			printf("%d", tag);
			tcnt[tag]++;
		}else
		{
			tag=PDGL_BTAC_CheckBlockBestS16(iblk+i*64, flags);
			PDGL_BTAC_EncodeBlockTagS16(iblk+i*64, oblk+i*32, tag, fl2);
			tcnt[tag]++;
		}
	}

//	printf("\n");
	
	printf("Encode %d: ", n);
	for(i=0; i<16; i++)
		printf("%d ", tcnt[i]);
	printf("\n");
}

PDGL_API void PDGL_BTAC_DecodeStreamS16(
	byte *iblk, s16 *oblk, int len, int flags)
{
	s16 tblk[64*2];
	int p0, p1, p2, p3;
	int q0, q1, q2, q3;
	float f, g;
	int i, j, k, n;
	
	n=(len+63)/64;
	for(i=0; i<n; i++)
	{
		if((flags&PDGL_BTAC_EFL_STEREO) && (flags&PDGL_BTAC_EFL_HALFMONO))
		{
			PDGL_BTAC_DecodeBlockTagS16(
				iblk+i*64+ 0, oblk+i*2*64+0, flags);
			PDGL_BTAC_DecodeBlockTagS16(
				iblk+i*64+32, oblk+i*2*64+1, flags);
		}else if(flags&PDGL_BTAC_EFL_STEREO)
		{
//			PDGL_BTAC_DecodeBlockTagS16(iblk+i*32, tblk, flags);
//			PDGL_BTAC_UnCompandBlockS16(tblk, oblk+i*2*64, flags);

			PDGL_BTAC_DecodeBlockTagS16(iblk+i*32, oblk+i*2*64, flags);
		}else
		{
			PDGL_BTAC_DecodeBlockTagS16(iblk+i*32, oblk+i*64, flags);
		}
	}

	if((flags&PDGL_BTAC_EFL_STEREO) && (flags&PDGL_BTAC_EFL_HALFMONO))
	{
		PDGL_BTAC_DeblockStreamS16(oblk, len);
		PDGL_BTAC_FilterStreamS16(oblk, len);
	}else if(flags&PDGL_BTAC_EFL_STEREO)
	{
		PDGL_BTAC_DeblockStreamS16(oblk, len);
		PDGL_BTAC_FilterStreamS16(oblk, len);
	}else
	{
		PDGL_BTAC_DeblockStreamMonoS16(oblk, len);
		PDGL_BTAC_FilterStreamMonoS16(oblk, len);
	}
}

