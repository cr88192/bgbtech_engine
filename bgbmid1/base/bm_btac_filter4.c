#include <bgbmid.h>

/*

256 samples in 1024 bits.

Tags 0-7 will direct to groups of 4 64-sample blocks.

*/

void BGBMID_BTAC_Encode64BlockHeaderSide2S16(
	s16 *iblk, byte *oblk,
	s16 *tcblk, s16 *tcpan,
	int *rcm, int *rcn,
	int *rsm, int *rsn,
	int flags)
{
	int sm, sn, cm, cn, tl, tr, tc;
	int lm2, ln2, rm2, rn2;
	int cm2, cn2, ca2, ba;
	int p0, p1, p2, p3;
	int s0, s1;
	int i, j, k, l;

#if 1
	cm= 99999; cn=-99999;
	sm= 99999; sn=-99999;
	
	ba=0;
	for(i=0; i<64; i++)
	{
		if(flags&BGBMID_BTAC_EFL_HALFMONO)
		{
			tl=iblk[i*2+0];
			tr=tl;
		}else if(flags&BGBMID_BTAC_EFL_STEREO)
		{
			tl=iblk[i*2+0];
			tr=iblk[i*2+1];
		}else
		{
			tl=iblk[i];
			tr=tl;
		}
		k=(tl+tr)>>1;
//		l=tl-k;
		l=((tl-k)+(k-tr))>>1;
		l=(l<(-32768))?(-32768):((l<32768)?l:32767);
		tcblk[i]=k;
		if(tcpan)
			{ tcpan[i]=l; }
		ba+=l;
		if(k<cm) { cm=k; }
		if(k>cn) { cn=k; }
		if(l<sm) { sm=l; }
		if(l>sn) { sn=l; }
	}

	ba/=64;

	for(i=0; i<32; i++)
		{ oblk[i]=0; }
	
	oblk[0]=0x00;
	oblk[1]=0x00;

	oblk[2]=sm&0xFF;
	oblk[3]=(sm>>8)&0xFF;
	oblk[4]=sn&0xFF;
	oblk[5]=(sn>>8)&0xFF;

	p0=(cm+8)>>4;
	p1=(cn+8)>>4;

	p0=bgbmid_clampi(p0, -2048, 2047);
	p1=bgbmid_clampi(p1, -2048, 2047);

	oblk[1]=(p0&15) | ((p1&15)<<4);
	oblk[6]=(p0>>4)&0xFF;
	oblk[7]=(p1>>4)&0xFF;
#endif

	cm=p0<<4;
	cn=p1<<4;

	*rcm=cm; *rcn=cn;
	if(rsm && rsn) { *rsm=sm; *rsn=sn; }
}


#if 1
void BGBMID_BTAC_Encode64Block15S16(s16 *iblk, byte *oblk, int flags)
{
	s16 tcblk[64], tcpan[64];
	int tcma[32], tsma[32];
	int cm, cn, sm, sn, tl, tr, tc;
	int cm2, cn2, ca2, ba;
	int p0, p1, p2, p3;
	int s0, s1;
	int i, j, k;

	BGBMID_BTAC_Encode64BlockHeaderSide2S16(
		iblk, oblk, tcblk, tcpan,
		&cm, &cn, &sm, &sn, flags);

	oblk[0]=0x05;
//	oblk[1]=0x00;
	
	for(i=0; i<32; i++)
	{
		cm2= 99999; cn2=-99999;
//		tc=tcblk[i*2+0];
		tc=tcpan[i*2+0];

		if(sn!=sm)
			{ s0=268435456/(sn-sm); }
		else { s0=0; }
		p0=(s0*(tc-sm)+8388608)>>24;
		p0=(p0<0)?0:((p0<16)?p0:15);
		tsma[i]=p0;
	}

	for(i=0; i<16; i++)
	{
		cm2= 99999; cn2=-99999;
//		tc=tcpan[i*4+0];
//		tc=(tcpan[i*4+0]+tcpan[i*4+1]+tcpan[i*4+2]+tcpan[i*4+3])>>2;
		tc=(tcblk[i*4+0]+tcblk[i*4+1]+tcblk[i*4+2]+tcblk[i*4+3])>>2;

		if(cn!=cm)
			{ s0=268435456/(cn-cm); }
		else { s0=0; }
		p0=(s0*(tc-cm)+8388608)>>24;
		p0=(p0<0)?0:((p0<16)?p0:15);
		tcma[i]=p0;
	}

	for(i=0; i<32; i++)
		{ oblk[16+(i>>1)]|=tsma[i]<<((i&1)*4); }
	for(i=0; i<16; i++)
		{ oblk[8+(i>>1)]|=tcma[i]<<((i&1)*4); }
}

void BGBMID_BTAC_Decode64Block15S16(byte *iblk, s16 *oblk, int flags)
{
	int tcma[32], tsma[32];
	int p0, p1, p2, p3;
	int s0, s1, s2;
	int cm, cn, ca;
	int sm, sn, sa;
	int cm2, cn2, ca2;
	int sm2, sn2, sa2;
	int tl1, tr1, tl2, tr2;
	int i, j, k;

//	BGBMID_BTAC_Decode64BlockSideHeader(iblk, flags,
//		&cm, &cn, &ca, &sm, &sn, &sa);
	BGBMID_BTAC_Decode64BlockSideHeader(iblk, flags,
		&sm, &sn, &sa, &cm, &cn, &ca);

	for(i=0; i<32; i++)
	{
		p0=(iblk[16+(i>>1)]>>((i&1)*4))&15;
		s0=(sn-sm);
		sm2=((s0*p0+8)>>4)+sm;
		sm2=(sm2<sm)?sm:((sm2<sn)?sm2:sn);
		tsma[i]=sm2;
	}

	for(i=0; i<16; i++)
	{
		p0=(iblk[8+(i>>1)]>>((i&1)*4))&15;
		s0=(cn-cm);
		cm2=((s0*p0+8)>>4)+cm;
		cm2=(cm2<cm)?cm:((cm2<cn)?cm2:cn);
		tcma[i]=cm2;
	}

	if(flags&BGBMID_BTAC_EFL_HALFMONO)
	{
		for(i=0; i<32; i++)
		{
			p0=tcma[i>>1];
			oblk[i*4+0]=p0;
			oblk[i*4+2]=p0;
		}
	}else if(flags&BGBMID_BTAC_EFL_STEREO)
	{
		for(i=0; i<32; i++)
		{
			p1=tsma[i];
			p0=(i>0)?tsma[i-1]:p1;
			p2=(i<31)?tsma[i+1]:p1;
			p3=(i<30)?tsma[i+2]:p2;

			p3=(3*p1+3*p2-p0-p3+2)>>2;

			s0=tcma[i>>1];

			tl1=s0+p1;
			tr1=s0-p1;
			tl2=s0+p3;
			tr2=s0-p3;

			tl1=(tl1<(-32768))?(-32768):((tl1<32768)?tl1:32767);
			tr1=(tr1<(-32768))?(-32768):((tr1<32768)?tr1:32767);
			tl2=(tl2<(-32768))?(-32768):((tl2<32768)?tl2:32767);
			tr2=(tr2<(-32768))?(-32768):((tr2<32768)?tr2:32767);

			oblk[i*4+0]=tl1;
			oblk[i*4+1]=tr1;
			oblk[i*4+2]=tl2;
			oblk[i*4+3]=tr2;
		}
	}else
	{
		for(i=0; i<32; i++)
		{
			p0=tcma[i>>1];
			oblk[i*2+0]=p0;
			oblk[i*2+1]=p0;
		}
	}
}
#endif


void BGBMID_BTAC_Upsample256BlockS16(s16 *iblk, s16 *oblk, int flags)
{
	int p0, p1, p2, p3;
	int ch;
	int i, j, k;

	if(flags&BGBMID_BTAC_EFL_STEREO)
	{
#if 1
		ch=(flags&BGBMID_BTAC_EFL_HALFMONO)?1:2;
		for(i=0; i<128; i++)
			for(j=0; j<ch; j++)
		{
			p1=iblk[i*2+j];
			p0=(i>0)?iblk[(i-1)*2+j]:p1;
			p2=(i<127)?iblk[(i+1)*2+j]:p1;
			p3=(i<126)?iblk[(i+2)*2+j]:p2;

			p3=(3*p1+3*p2-p0-p3+2)>>2;
			p3=(p3<(-32768))?(-32768):((p3<32768)?p3:32767);

			oblk[i*4+0+j]=p1;
			oblk[i*4+2+j]=p3;
		}
#endif

#if 0
		for(i=0; i<128; i++)
		{
			oblk[i*4+0]=iblk[i*2+0];
			oblk[i*4+1]=iblk[i*2+1];
			oblk[i*4+2]=iblk[i*2+0];
			oblk[i*4+3]=iblk[i*2+1];
		}
#endif
	}else
	{
		for(i=0; i<128; i++)
		{
			oblk[i*2+0]=iblk[i];
			oblk[i*2+1]=iblk[i];
		}
	}
}

void BGBMID_BTAC_AddResiduals256BlockS16(
	s16 *iblk, s16 *trblk, s16 *oblk, int flags)
{
	int i, j, k;

	if(flags&BGBMID_BTAC_EFL_STEREO)
	{
		for(i=0; i<256; i++)
		{
			j=iblk[i*2+0]+trblk[(255-i)*2+0];
			k=iblk[i*2+1]+trblk[(255-i)*2+1];
			j=bgbmid_clampi(j, -32768, 32767);
			k=bgbmid_clampi(k, -32768, 32767);
			oblk[i*2+0]=j;
			oblk[i*2+1]=k;
		}
	}else
	{
		for(i=0; i<256; i++)
		{
			j=iblk[i]+trblk[255-i];
			j=bgbmid_clampi(j, -32768, 32767);
			oblk[i]=j;
		}
	}
}

void BGBMID_BTAC_Decode256Block0S16(byte *iblk, s16 *oblk, int flags)
{
	s16 trblk[2*256];
	int i, j, k;

	if((flags&BGBMID_BTAC_EBMODE_MASK)==BGBMID_BTAC_EBMODE_BLK256LQ)
	{
		if(flags&BGBMID_BTAC_EFL_STEREO)
		{
			BGBMID_BTAC_Decode64BlockTagS16(iblk+0*32, oblk+2*2*64, flags);
			BGBMID_BTAC_Decode64BlockTagS16(iblk+1*32, oblk+3*2*64, flags);
			BGBMID_BTAC_Upsample256BlockS16(oblk+2*2*64, oblk, flags);
		}else
		{
			BGBMID_BTAC_Decode64BlockTagS16(iblk+0*32, oblk+2*64, flags);
			BGBMID_BTAC_Decode64BlockTagS16(iblk+1*32, oblk+3*64, flags);
			BGBMID_BTAC_Upsample256BlockS16(oblk+2*64, oblk, flags);
		}
		return;
	}

	if(flags&BGBMID_BTAC_EFL_STEREO)
	{
		BGBMID_BTAC_Decode64BlockTagS16(iblk+0*32, oblk+0*2*64, flags);
		BGBMID_BTAC_Decode64BlockTagS16(iblk+1*32, oblk+1*2*64, flags);
		BGBMID_BTAC_Decode64BlockTagS16(iblk+2*32, oblk+2*2*64, flags);
		BGBMID_BTAC_Decode64BlockTagS16(iblk+3*32, oblk+3*2*64, flags);
	}else
	{
		BGBMID_BTAC_Decode64BlockTagS16(iblk+0*32, oblk+0*64, flags);
		BGBMID_BTAC_Decode64BlockTagS16(iblk+1*32, oblk+1*64, flags);
		BGBMID_BTAC_Decode64BlockTagS16(iblk+2*32, oblk+2*64, flags);
		BGBMID_BTAC_Decode64BlockTagS16(iblk+3*32, oblk+3*64, flags);
	}

	if((flags&BGBMID_BTAC_EBMODE_MASK)==BGBMID_BTAC_EBMODE_BLK256HQ)
	{
		if(flags&BGBMID_BTAC_EFL_STEREO)
		{
			BGBMID_BTAC_Decode64BlockTagS16(iblk+4*32, trblk+0*2*64, flags);
			BGBMID_BTAC_Decode64BlockTagS16(iblk+5*32, trblk+1*2*64, flags);
			BGBMID_BTAC_Decode64BlockTagS16(iblk+6*32, trblk+2*2*64, flags);
			BGBMID_BTAC_Decode64BlockTagS16(iblk+7*32, trblk+3*2*64, flags);
			BGBMID_BTAC_AddResiduals256BlockS16(oblk, trblk, oblk, flags);
		}else
		{
			BGBMID_BTAC_Decode64BlockTagS16(iblk+4*32, trblk+0*64, flags);
			BGBMID_BTAC_Decode64BlockTagS16(iblk+5*32, trblk+1*64, flags);
			BGBMID_BTAC_Decode64BlockTagS16(iblk+6*32, trblk+2*64, flags);
			BGBMID_BTAC_Decode64BlockTagS16(iblk+7*32, trblk+3*64, flags);
			BGBMID_BTAC_AddResiduals256BlockS16(oblk, trblk, oblk, flags);
		}
	}
}

void BGBMID_BTAC_Decode256BlockTagS16(byte *iblk, s16 *oblk, int flags)
{
	switch(iblk[0]&15)
	{
	case 0: case 1: case 2: case 3:
	case 4: case 5: case 6: case 7:
		BGBMID_BTAC_Decode256Block0S16(iblk, oblk, flags);
		break;
	}
}

BMID_API int BGBMID_BTAC_BlockSize256Flags(int flags)
{
	int obs;
	
	if((flags&BGBMID_BTAC_EFL_STEREO) && (flags&BGBMID_BTAC_EFL_HALFMONO))
	{
		switch(flags&BGBMID_BTAC_EBMODE_MASK)
		{
		case BGBMID_BTAC_EBMODE_BLK256: obs=256; break;
		case BGBMID_BTAC_EBMODE_BLK256LQ: obs=128; break;
		case BGBMID_BTAC_EBMODE_BLK256HQ: obs=512; break;
		default: obs=256; break;
		}
	}else
	{
		switch(flags&BGBMID_BTAC_EBMODE_MASK)
		{
		case BGBMID_BTAC_EBMODE_BLK256: obs=128; break;
		case BGBMID_BTAC_EBMODE_BLK256LQ: obs=64; break;
		case BGBMID_BTAC_EBMODE_BLK256HQ: obs=256; break;
		default: obs=128; break;
		}
	}
	return(obs);
}

BMID_API void BGBMID_BTAC_Decode256StreamS16(
	byte *iblk, s16 *oblk, int len, int flags)
{
	s16 tblk[256*2];
	int p0, p1, p2, p3;
	int q0, q1, q2, q3;
	float f, g;
	int i, j, k, n, ibs;
	
	ibs=BGBMID_BTAC_BlockSize256Flags(flags);
	
	n=(len+255)/256;
	for(i=0; i<n; i++)
	{
		if((flags&BGBMID_BTAC_EFL_STEREO) && (flags&BGBMID_BTAC_EFL_HALFMONO))
		{
			BGBMID_BTAC_Decode256BlockTagS16(
				iblk+i*ibs+       0, oblk+i*2*256+0, flags);
			BGBMID_BTAC_Decode256BlockTagS16(
				iblk+i*ibs+(ibs>>1), oblk+i*2*256+1, flags);
		}else if(flags&BGBMID_BTAC_EFL_STEREO)
		{
			BGBMID_BTAC_Decode256BlockTagS16(iblk+i*ibs, oblk+i*2*256, flags);
		}else
		{
			BGBMID_BTAC_Decode256BlockTagS16(iblk+i*ibs, oblk+i*256, flags);
		}
	}
}



void BGBMID_BTAC_Encode256LQBlock0S16(s16 *iblk, byte *oblk, int flags)
{
	s16 tblk[128*2];
	int i;

	if(flags&BGBMID_BTAC_EFL_STEREO)
	{
		for(i=0; i<128; i++)
		{
			tblk[i*2+0]=iblk[i*4+0];
			tblk[i*2+1]=iblk[i*4+1];
		}
	
		BGBMID_BTAC_Encode64BlockBestTagS16(tblk+0*2*64, oblk+0*32, flags);
		BGBMID_BTAC_Encode64BlockBestTagS16(tblk+1*2*64, oblk+1*32, flags);
	}else
	{
		for(i=0; i<128; i++)
			{ tblk[i]=iblk[i*2+0]; }
		BGBMID_BTAC_Encode64BlockBestTagS16(tblk+0*64, oblk+0*32, flags);
		BGBMID_BTAC_Encode64BlockBestTagS16(tblk+1*64, oblk+1*32, flags);
	}
}

void BGBMID_BTAC_Encode256HQBlock0S16(s16 *iblk, byte *oblk, int flags)
{
	s16 tblk[256*2];
	s16 trblk[256*2];
	int i, j, k;

	if(flags&BGBMID_BTAC_EFL_STEREO)
	{
		BGBMID_BTAC_Encode64BlockBestTagS16(iblk+0*2*64, oblk+0*32, flags);
		BGBMID_BTAC_Encode64BlockBestTagS16(iblk+1*2*64, oblk+1*32, flags);
		BGBMID_BTAC_Encode64BlockBestTagS16(iblk+2*2*64, oblk+2*32, flags);
		BGBMID_BTAC_Encode64BlockBestTagS16(iblk+3*2*64, oblk+3*32, flags);

		BGBMID_BTAC_Decode64BlockTagS16(oblk+0*32, tblk+0*2*64, flags);
		BGBMID_BTAC_Decode64BlockTagS16(oblk+1*32, tblk+1*2*64, flags);
		BGBMID_BTAC_Decode64BlockTagS16(oblk+2*32, tblk+2*2*64, flags);
		BGBMID_BTAC_Decode64BlockTagS16(oblk+3*32, tblk+3*2*64, flags);

		for(i=0; i<256; i++)
		{
			j=iblk[i*2+0]-tblk[i*2+0];
			k=iblk[i*2+1]-tblk[i*2+1];
			j=bgbmid_clampi(j, -32768, 32767);
			k=bgbmid_clampi(k, -32768, 32767);
			trblk[(255-i)*2+0]=j;
			trblk[(255-i)*2+1]=k;
		}

		BGBMID_BTAC_Encode64BlockBestTagS16(trblk+0*2*64, oblk+4*32, flags);
		BGBMID_BTAC_Encode64BlockBestTagS16(trblk+1*2*64, oblk+5*32, flags);
		BGBMID_BTAC_Encode64BlockBestTagS16(trblk+2*2*64, oblk+6*32, flags);
		BGBMID_BTAC_Encode64BlockBestTagS16(trblk+3*2*64, oblk+7*32, flags);
	}else
	{
		BGBMID_BTAC_Encode64BlockBestTagS16(iblk+0*64, oblk+0*32, flags);
		BGBMID_BTAC_Encode64BlockBestTagS16(iblk+1*64, oblk+1*32, flags);
		BGBMID_BTAC_Encode64BlockBestTagS16(iblk+2*64, oblk+2*32, flags);
		BGBMID_BTAC_Encode64BlockBestTagS16(iblk+3*64, oblk+3*32, flags);

		BGBMID_BTAC_Decode64BlockTagS16(oblk+0*32, tblk+0*64, flags);
		BGBMID_BTAC_Decode64BlockTagS16(oblk+1*32, tblk+1*64, flags);
		BGBMID_BTAC_Decode64BlockTagS16(oblk+2*32, tblk+2*64, flags);
		BGBMID_BTAC_Decode64BlockTagS16(oblk+3*32, tblk+3*64, flags);

		for(i=0; i<256; i++)
		{
			j=iblk[i]-tblk[i];
			j=bgbmid_clampi(j, -32768, 32767);
			trblk[255-i]=j;
		}

		BGBMID_BTAC_Encode64BlockBestTagS16(trblk+0*64, oblk+4*32, flags);
		BGBMID_BTAC_Encode64BlockBestTagS16(trblk+1*64, oblk+5*32, flags);
		BGBMID_BTAC_Encode64BlockBestTagS16(trblk+2*64, oblk+6*32, flags);
		BGBMID_BTAC_Encode64BlockBestTagS16(trblk+3*64, oblk+7*32, flags);
	}
}

void BGBMID_BTAC_Encode256Block0S16(s16 *iblk, byte *oblk, int flags)
{
	if((flags&BGBMID_BTAC_EBMODE_MASK)==BGBMID_BTAC_EBMODE_BLK256LQ)
	{
		BGBMID_BTAC_Encode256LQBlock0S16(iblk, oblk, flags);
		return;
	}

	if((flags&BGBMID_BTAC_EBMODE_MASK)==BGBMID_BTAC_EBMODE_BLK256HQ)
	{
		BGBMID_BTAC_Encode256HQBlock0S16(iblk, oblk, flags);
		return;
	}

	if((flags&BGBMID_BTAC_EFL_STEREO) && (flags&BGBMID_BTAC_EFL_HALFMONO))
	{
		BGBMID_BTAC_Encode64BlockBestTagS16(iblk+0*2*64, oblk+0*32, flags);
		BGBMID_BTAC_Encode64BlockBestTagS16(iblk+1*2*64, oblk+1*32, flags);
		BGBMID_BTAC_Encode64BlockBestTagS16(iblk+2*2*64, oblk+2*32, flags);
		BGBMID_BTAC_Encode64BlockBestTagS16(iblk+3*2*64, oblk+3*32, flags);
	}else if(flags&BGBMID_BTAC_EFL_STEREO)
	{
		BGBMID_BTAC_Encode64BlockBestTagS16(iblk+0*2*64, oblk+0*32, flags);
		BGBMID_BTAC_Encode64BlockBestTagS16(iblk+1*2*64, oblk+1*32, flags);
		BGBMID_BTAC_Encode64BlockBestTagS16(iblk+2*2*64, oblk+2*32, flags);
		BGBMID_BTAC_Encode64BlockBestTagS16(iblk+3*2*64, oblk+3*32, flags);
	}else
	{
		BGBMID_BTAC_Encode64BlockBestTagS16(iblk+0*64, oblk+0*32, flags);
		BGBMID_BTAC_Encode64BlockBestTagS16(iblk+1*64, oblk+1*32, flags);
		BGBMID_BTAC_Encode64BlockBestTagS16(iblk+2*64, oblk+2*32, flags);
		BGBMID_BTAC_Encode64BlockBestTagS16(iblk+3*64, oblk+3*32, flags);
	}
}

void BGBMID_BTAC_Encode256BlockTagS16(s16 *iblk, byte *oblk,
	int tag, int flags)
{
	switch(tag)
	{
	case 0: case 1: case 2: case 3:
	case 4: case 5: case 6: case 7:
		BGBMID_BTAC_Encode256Block0S16(iblk, oblk, flags);
		break;
	default:
		break;
	}
}

BMID_API int BGBMID_BTAC_Calc256BlockErrorS16(
	s16 *iblk, s16 *tblk, int flags)
{
	double e;
	int ld, rd, cd, c0, c1;
	int i, j, k;

	if(flags&BGBMID_BTAC_EFL_HALFMONO)
	{
		e=0;
		for(i=0; i<256; i++)
		{
			ld=iblk[i*2+0]-tblk[i*2+0];
			e+=ld*ld;
		}
	
		i=sqrt(e/256);
		return(i);
	}else if(flags&BGBMID_BTAC_EFL_STEREO)
	{
		e=0;
		for(i=0; i<256; i++)
		{
			ld=iblk[i*2+0]-tblk[i*2+0];
			rd=iblk[i*2+1]-tblk[i*2+1];
			c0=(iblk[i*2+0]+iblk[i*2+1])>>1;
			c1=(tblk[i*2+0]+tblk[i*2+1])>>1;
			cd=c1-c0;
			e+=ld*ld;	e+=rd*rd;
			e+=cd*cd;	e+=cd*cd;
		}

#if 1
//		e=0;
		for(i=0; i<255; i++)
		{
			j=iblk[(i+1)*2+0]-iblk[(i+0)*2+0];
			k=tblk[(i+1)*2+0]-tblk[(i+0)*2+0];
			ld=(k*k)/(j*j+1);

			j=iblk[(i+1)*2+1]-iblk[(i+0)*2+1];
			k=tblk[(i+1)*2+1]-tblk[(i+0)*2+1];
			rd=(k*k)/(j*j+1);
		
			e+=ld;	e+=rd;
//			e+=ld*ld;	e+=rd*rd;
		}
#endif

		i=sqrt(e/256);
		return(i);
	}else 
	{
		e=0;
		for(i=0; i<256; i++)
		{
			ld=iblk[i]-tblk[i];
			e+=ld*ld;
		}
	
		i=sqrt(e/256);
		return(i);
	}
}

BMID_API int BGBMID_BTAC_Check256BlockBestS16(s16 *iblk, int flags)
{
	s16 eblk[512];
	byte tblk[256];
	int bn, be, te;
	int i;
	
	bn=0; be=999999999;
	for(i=0; i<1; i++)
	{
		BGBMID_BTAC_Encode256BlockTagS16(iblk, tblk, i, flags);
		BGBMID_BTAC_Decode256BlockTagS16(tblk, eblk, flags);
		te=BGBMID_BTAC_Calc256BlockErrorS16(iblk, eblk, flags);
		if(te<be) { bn=i; be=te; }
	}

	return(bn);
}

void BGBMID_BTAC_Encode256BlockBestTagS16(s16 *iblk, byte *oblk, int flags)
{
	int tag, ltag, rtag;

	if((flags&BGBMID_BTAC_EFL_STEREO) && (flags&BGBMID_BTAC_EFL_HALFMONO))
	{
		ltag=BGBMID_BTAC_Check256BlockBestS16(iblk+0, flags);
		rtag=BGBMID_BTAC_Check256BlockBestS16(iblk+1, flags);

		BGBMID_BTAC_Encode256BlockTagS16(
			iblk+0, oblk+  0, ltag, flags);
		BGBMID_BTAC_Encode256BlockTagS16(
			iblk+1, oblk+128, rtag, flags);
	}else if(flags&BGBMID_BTAC_EFL_STEREO)
	{
		tag=BGBMID_BTAC_Check256BlockBestS16(iblk, flags);
		BGBMID_BTAC_Encode256BlockTagS16(iblk, oblk, tag, flags);
	}else
	{
		tag=BGBMID_BTAC_Check256BlockBestS16(iblk, flags);
		BGBMID_BTAC_Encode256BlockTagS16(iblk, oblk, tag, flags);
	}
}

BMID_API void BGBMID_BTAC_Encode256StreamS16(
	s16 *iblk, byte *oblk, int len, int flags)
{
	int i, j, n, tag, ltag, rtag, fl2;
	int ibs, obs;
	
//	for(i=0; i<16; i++)
//		{ tcnt[i]=0; }
	
	if((flags&BGBMID_BTAC_EFL_STEREO) && (flags&BGBMID_BTAC_EFL_HALFMONO))
	{
		ibs=2*256;
		obs=BGBMID_BTAC_BlockSize256Flags(flags);
	}else if(flags&BGBMID_BTAC_EFL_STEREO)
	{
		ibs=2*256;
		obs=BGBMID_BTAC_BlockSize256Flags(flags);
	}else
	{
		ibs=256;
		obs=BGBMID_BTAC_BlockSize256Flags(flags);
	}

	n=(len+255)/256;
	for(i=0; i<n; i++)
	{
		fl2=flags;
		if((i>0) && i<(n-1))
			fl2|=BGBMID_BTAC_EFL_MIDSTREAM;
		BGBMID_BTAC_Encode256BlockBestTagS16(iblk+i*ibs, oblk+i*obs, fl2);
	}

#if 0
//	printf("\n");
	
	printf("Encode %d: ", n);
	for(i=0; i<16; i++)
		printf("%d ", tcnt[i]);
	printf("\n");
#endif
}
