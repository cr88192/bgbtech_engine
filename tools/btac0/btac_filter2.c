#include <bgbtac.h>

void BGBTAC_EncodeBlock3MonoS16(s16 *iblk, byte *oblk)
{
	int tcma[8], tcna[8];
	int tcmb[8], tcnb[8];
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
//		tl=(iblk[i]+7)>>4; tr=tl;
		k=(tl+tr)>>1;
		if(k<cm) { lm=tl; rm=tr; cm=k; }
		if(k>cn) { ln=tl; rn=tr; cn=k; }
	}
	
	oblk[0]=cm&0xFF;
	oblk[1]=(cm>>8)&0xFF;
	oblk[2]=cn&0xFF;
	oblk[3]=(cn>>8)&0xFF;
	oblk[4]=((lm-rm+128)>>8)&0xFF;
	oblk[5]=((ln-rn+128)>>8)&0xFF;
	
	for(i=0; i<8; i++)
	{
		cm2= 99999; cn2=-99999;
		for(j=0; j<8; j++)
		{
			tl=iblk[i*8+j]; tr=tl;
//			tl=(iblk[i*8+j]+7)>>4; tr=tl;
			tc=(tl+tr)>>1;
			if(tc<cm2) { cm2=tc; }
			if(tc>cn2) { cn2=tc; }
		}
		
		if(cn!=cm)
//			{ s0=16384/(cn-cm); }
			{ s0=16777216/(cn-cm); }
		else { s0=0; }

//		p0=(s0*(cm2-cm)+255)>>9;
//		p1=(s0*(cn2-cm)+255)>>9;

		p0=(s0*(cm2-cm)+262144)>>19;
		p1=(s0*(cn2-cm)+262144)>>19;
		
		p0=(p0<0)?0:((p0<31)?p0:31);
		p1=(p1<0)?0:((p1<31)?p1:31);
		
		tcma[i]=p0;
		tcna[i]=p1;

		tcmb[i]=cm2;
		tcnb[i]=cn2;
	}
	
	// 6: aaaa abbb
	// 7: bbcc cccd
	// 8: dddd eeee
	// 9: efff ffgg
	//10: gggh hhhh
	
	oblk[ 6]=(tcma[0]>>0) | (tcma[1]<<5);
	oblk[ 7]=(tcma[1]>>3) | (tcma[2]<<2) | (tcma[3]<<7);
	oblk[ 8]=(tcma[3]>>1) | (tcma[4]<<4);
	oblk[ 9]=(tcma[4]>>4) | (tcma[5]<<1) | (tcma[6]<<6);
	oblk[10]=(tcma[6]>>2) | (tcma[7]<<3);

	oblk[11]=(tcna[0]>>0) | (tcna[1]<<5);
	oblk[12]=(tcna[1]>>3) | (tcna[2]<<2) | (tcma[3]<<7);
	oblk[13]=(tcna[3]>>1) | (tcna[4]<<4);
	oblk[14]=(tcna[4]>>4) | (tcna[5]<<1) | (tcma[6]<<6);
	oblk[15]=(tcna[6]>>2) | (tcna[7]<<3);
	
	for(i=0; i<8; i++)
	{
		cm2=tcmb[i]; cn2=tcnb[i];
		ca2=(tcmb[i]+tcnb[i])>>1;

		if(cn2!=cm2)
			{ s1=16777216/(cn2-cm2); }
		else { s1=0; }

		for(j=0; j<8; j++)
		{
			k=i*8+j;
		
			tl=iblk[k]; tr=tl;
//			tl=(iblk[k]+7)>>4; tr=tl;
			tc=(tl+tr)>>1;

			p0=(s1*(tc-cm2)+2097152)>>22;

			if((p0<0) || (p0>4))
				printf("%d\n", p0);

			p0=(p0<0)?0:((p0<4)?p0:3);

			oblk[16+(k>>2)]&=~(3<<((k&3)*2));
			oblk[16+(k>>2)]|=p0<<((k&3)*2);

//			if(tc>=ca2)
//				{ oblk[16+(k>>3)]|=1<<(k&7); }
//			else
//				{ oblk[16+(k>>3)]&=~(1<<(k&7)); }
		}
	}	
}

BTAC_API void BGBTAC_EncodeStream3MonoS16(s16 *iblk, byte *oblk, int len)
{
	int i, j, n;
	
	n=(len+63)/64;
	for(i=0; i<n; i++)
	{
		BGBTAC_EncodeBlock3MonoS16(iblk+i*64, oblk+i*32);
	}
}

void BGBTAC_DecodeBlock3MonoS16(byte *iblk, s16 *oblk)
{
	int tcma[8], tcna[8];
	int tcmb[8], tcnb[8];
	int p0, p1, p2, p3;
	int s0, s1;
	int cm, cn, ca;
	int cm2, cn2, ca2;
	int i, j, k;
	
	cm=(s16)(iblk[0]+(iblk[1]<<8));
	cn=(s16)(iblk[2]+(iblk[3]<<8));
	ca=(cm+cn)>>1;

//	for(i=0; i<64; i++)
//		{ oblk[i]=ca; }

	// 6: aaaa abbb
	// 7: bbcc cccd
	// 8: dddd eeee
	// 9: efff ffgg
	//10: gggh hhhh
	
	tcmb[0]=  iblk[ 6]&0x1F;
	tcmb[1]=((iblk[ 6]>>5)|(iblk[ 7]<<3))&0x1F;
	tcmb[2]= (iblk[ 7]>>2)&0x1F;
	tcmb[3]=((iblk[ 7]>>7)|(iblk[ 8]<<1))&0x1F;
	tcmb[4]=((iblk[ 8]>>4)|(iblk[ 9]<<4))&0x1F;
	tcmb[5]= (iblk[ 9]>>1)&0x1F;
	tcmb[6]=((iblk[ 9]>>6)|(iblk[10]<<2))&0x1F;
	tcmb[7]= (iblk[10]>>3)&0x1F;

	tcnb[0]=  iblk[11]&0x1F;
	tcnb[1]=((iblk[11]>>5)|(iblk[12]<<3))&0x1F;
	tcnb[2]= (iblk[12]>>2)&0x1F;
	tcnb[3]=((iblk[12]>>7)|(iblk[13]<<1))&0x1F;
	tcnb[4]=((iblk[13]>>4)|(iblk[14]<<4))&0x1F;
	tcnb[5]= (iblk[14]>>1)&0x1F;
	tcnb[6]=((iblk[14]>>6)|(iblk[15]<<2))&0x1F;
	tcnb[7]= (iblk[15]>>3)&0x1F;
	
	for(i=0; i<8; i++)
	{
		cm2=tcmb[i];
		cn2=tcnb[i];
	
		j=(cn-cm);
		p0=((j*cm2+16)>>5)+cm;
		p1=((j*cn2+16)>>5)+cm;

		p0=(p0<cm)?cm:((p0<cn)?p0:cn);
		p1=(p1<cm)?cm:((p1<cn)?p1:cn);
	
		tcma[i]=p0;
		tcna[i]=p1;
		
		p2=(p0+p1)>>1;
//		if(p2<cm)p2=cm;
//		if(p2>cn)p2=cn;

		cm2=tcma[i]; cn2=tcna[i];
		s1=(cn2-cm2);

		for(j=0; j<8; j++)
		{
			k=i*8+j;

			p3=(iblk[16+(k>>2)]>>((k&3)*2))&3;

			p3=((s1*p3+1)>>2)+cm2;
			p3=(p3<cm2)?cm2:((p3<cn2)?p3:cn2);
			oblk[i*8+j]=p3;

//			oblk[i*8+j]=(iblk[16+(k>>3)]&(1<<(k&7)))?p1:p0;
		
//			oblk[i*16+j]=p2;
//			oblk[i*8+j]=(iblk[16+(k>>3)]&(1<<(k&7)))?p1:p0;
		}
	}
}

BTAC_API void BGBTAC_DecodeStream3MonoS16(byte *iblk, s16 *oblk, int len)
{
	int i, j, n;
	
	n=(len+63)/64;
	for(i=0; i<n; i++)
	{
		BGBTAC_DecodeBlock3MonoS16(iblk+i*32, oblk+i*64);
	}
}


void BGBTAC_EncodeBlock4MonoS16(s16 *iblk, byte *oblk)
{
	int tcma[8], tcna[8];
	int tcmb[8], tcnb[8];
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
//		tl=(iblk[i]+7)>>4; tr=tl;
		k=(tl+tr)>>1;
		if(k<cm) { lm=tl; rm=tr; cm=k; }
		if(k>cn) { ln=tl; rn=tr; cn=k; }
	}
	
	oblk[0]=cm&0xFF;
	oblk[1]=(cm>>8)&0xFF;
	oblk[2]=cn&0xFF;
	oblk[3]=(cn>>8)&0xFF;
	oblk[4]=((lm-rm+128)>>8)&0xFF;
	oblk[5]=((ln-rn+128)>>8)&0xFF;
	
	for(i=0; i<4; i++)
	{
		cm2= 99999; cn2=-99999;
		for(j=0; j<16; j++)
		{
			tl=iblk[i*8+j]; tr=tl;
			tc=(tl+tr)>>1;
			if(tc<cm2) { cm2=tc; }
			if(tc>cn2) { cn2=tc; }
		}
		
		if(cn!=cm)
//			{ s0=16384/(cn-cm); }
			{ s0=16777216/(cn-cm); }
		else { s0=0; }

//		p0=(s0*(cm2-cm)+255)>>9;
//		p1=(s0*(cn2-cm)+255)>>9;

//		p0=(s0*(cm2-cm)+262144)>>19;
//		p1=(s0*(cn2-cm)+262144)>>19;
//		p0=(p0<0)?0:((p0<31)?p0:31);
//		p1=(p1<0)?0:((p1<31)?p1:31);

		p0=(s0*(cm2-cm)+8192)>>14;
		p1=(s0*(cn2-cm)+8192)>>14;
		p0=(p0<0)?0:((p0<1024)?p0:1023);
		p1=(p1<0)?0:((p1<1024)?p1:1023);
		
		tcma[i]=p0;
		tcna[i]=p1;

		tcmb[i]=cm2;
		tcnb[i]=cn2;
	}

#if 0
	// 6: aaaa abbb
	// 7: bbcc cccd
	// 8: dddd eeee
	// 9: efff ffgg
	//10: gggh hhhh
	
//	oblk[ 6]=(tcma[0]>>0) | (tcma[1]<<5);
//	oblk[ 7]=(tcma[1]>>3) | (tcma[2]<<2) | (tcma[3]<<7);
//	oblk[ 8]=(tcma[3]>>1) | (tcma[4]<<4);
//	oblk[ 9]=(tcma[4]>>4) | (tcma[5]<<1) | (tcma[6]<<6);
//	oblk[10]=(tcma[6]>>2) | (tcma[7]<<3);

//	oblk[11]=(tcna[0]>>0) | (tcna[1]<<5);
//	oblk[12]=(tcna[1]>>3) | (tcna[2]<<2) | (tcma[3]<<7);
//	oblk[13]=(tcna[3]>>1) | (tcna[4]<<4);
//	oblk[14]=(tcna[4]>>4) | (tcna[5]<<1) | (tcma[6]<<6);
//	oblk[15]=(tcna[6]>>2) | (tcna[7]<<3);
#endif

	// 6: aaaa aaaa
	// 7: aabb bbbb
	// 8: bbbb cccc
	// 9: cccc ccdd
	//10: dddd dddd

	oblk[ 6]=(tcma[0]>>0);
	oblk[ 7]=(tcma[0]>>8) | (tcma[1]<<2);
	oblk[ 8]=(tcma[1]>>6) | (tcma[2]<<4);
	oblk[ 9]=(tcma[2]>>4) | (tcma[3]<<6);
	oblk[10]=(tcma[3]>>2);

	oblk[11]=(tcna[0]>>0);
	oblk[12]=(tcna[0]>>8) | (tcna[1]<<2);
	oblk[13]=(tcna[1]>>6) | (tcna[2]<<4);
	oblk[14]=(tcna[2]>>4) | (tcna[3]<<6);
	oblk[15]=(tcna[3]>>2);

	for(i=0; i<4; i++)
	{
		cm2=tcmb[i]; cn2=tcnb[i];
		ca2=(tcmb[i]+tcnb[i])>>1;

		if(cn2!=cm2)
			{ s1=16777216/(cn2-cm2); }
		else { s1=0; }

		for(j=0; j<16; j++)
		{
			k=i*16+j;
		
			tl=iblk[k]; tr=tl;
			tc=(tl+tr)>>1;

			p0=(s1*(tc-cm2)+2097152)>>22;

			if((p0<0) || (p0>4))
				printf("%d\n", p0);

			p0=(p0<0)?0:((p0<4)?p0:3);

			oblk[16+(k>>2)]&=~(3<<((k&3)*2));
			oblk[16+(k>>2)]|=p0<<((k&3)*2);
		}
	}	
}

void BGBTAC_DecodeBlock4MonoS16(byte *iblk, s16 *oblk)
{
	int tcma[8], tcna[8];
	int tcmb[8], tcnb[8];
	int p0, p1, p2, p3;
	int s0, s1;
	int cm, cn, ca;
	int cm2, cn2, ca2;
	int i, j, k;
	
	cm=(s16)(iblk[0]+(iblk[1]<<8));
	cn=(s16)(iblk[2]+(iblk[3]<<8));
	ca=(cm+cn)>>1;

//	for(i=0; i<64; i++)
//		{ oblk[i]=ca; }

#if 0
	// 6: aaaa abbb
	// 7: bbcc cccd
	// 8: dddd eeee
	// 9: efff ffgg
	//10: gggh hhhh
	
	tcmb[0]=  iblk[ 6]&0x1F;
	tcmb[1]=((iblk[ 6]>>5)|(iblk[ 7]<<3))&0x1F;
	tcmb[2]= (iblk[ 7]>>2)&0x1F;
	tcmb[3]=((iblk[ 7]>>7)|(iblk[ 8]<<1))&0x1F;
	tcmb[4]=((iblk[ 8]>>4)|(iblk[ 9]<<4))&0x1F;
	tcmb[5]= (iblk[ 9]>>1)&0x1F;
	tcmb[6]=((iblk[ 9]>>6)|(iblk[10]<<2))&0x1F;
	tcmb[7]= (iblk[10]>>3)&0x1F;

	tcnb[0]=  iblk[11]&0x1F;
	tcnb[1]=((iblk[11]>>5)|(iblk[12]<<3))&0x1F;
	tcnb[2]= (iblk[12]>>2)&0x1F;
	tcnb[3]=((iblk[12]>>7)|(iblk[13]<<1))&0x1F;
	tcnb[4]=((iblk[13]>>4)|(iblk[14]<<4))&0x1F;
	tcnb[5]= (iblk[14]>>1)&0x1F;
	tcnb[6]=((iblk[14]>>6)|(iblk[15]<<2))&0x1F;
	tcnb[7]= (iblk[15]>>3)&0x1F;
#endif

	// 6: aaaa aaaa
	// 7: aabb bbbb
	// 8: bbbb cccc
	// 9: cccc ccdd
	//10: dddd dddd

	tcmb[0]=((iblk[ 6]>>0)|(iblk[ 7]<<8))&0x3FF;
	tcmb[1]=((iblk[ 7]>>2)|(iblk[ 8]<<6))&0x3FF;
	tcmb[2]=((iblk[ 8]>>4)|(iblk[ 9]<<4))&0x3FF;
	tcmb[3]=((iblk[ 9]>>6)|(iblk[10]<<2))&0x3FF;

	tcnb[0]=((iblk[11]>>0)|(iblk[12]<<8))&0x3FF;
	tcnb[1]=((iblk[12]>>2)|(iblk[13]<<6))&0x3FF;
	tcnb[2]=((iblk[13]>>4)|(iblk[14]<<4))&0x3FF;
	tcnb[3]=((iblk[14]>>6)|(iblk[15]<<2))&0x3FF;

	for(i=0; i<4; i++)
	{
		cm2=tcmb[i];
		cn2=tcnb[i];
	
		j=(cn-cm);
		p0=((j*cm2+512)>>10)+cm;
		p1=((j*cn2+512)>>10)+cm;

		p0=(p0<cm)?cm:((p0<cn)?p0:cn);
		p1=(p1<cm)?cm:((p1<cn)?p1:cn);
	
		tcma[i]=p0;
		tcna[i]=p1;
		
		p2=(p0+p1)>>1;
//		if(p2<cm)p2=cm;
//		if(p2>cn)p2=cn;

		cm2=tcma[i]; cn2=tcna[i];
		s1=(cn2-cm2);

		for(j=0; j<16; j++)
		{
			k=i*16+j;

			p3=(iblk[16+(k>>2)]>>((k&3)*2))&3;

			p3=((s1*p3+1)>>2)+cm2;
			p3=(p3<cm2)?cm2:((p3<cn2)?p3:cn2);
			oblk[i*16+j]=p3;
		}
	}
}

BTAC_API void BGBTAC_EncodeStream4MonoS16(s16 *iblk, byte *oblk, int len)
{
	int i, j, n;
	
	n=(len+63)/64;
	for(i=0; i<n; i++)
	{
		BGBTAC_EncodeBlock4MonoS16(iblk+i*64, oblk+i*32);
	}
}

BTAC_API void BGBTAC_DecodeStream4MonoS16(byte *iblk, s16 *oblk, int len)
{
	int i, j, n;
	
	n=(len+63)/64;
	for(i=0; i<n; i++)
	{
		BGBTAC_DecodeBlock4MonoS16(iblk+i*32, oblk+i*64);
	}
}
