#include <bgbtac.h>

void *bgbtac_malloc(int sz)
{
	void *ptr;
	ptr=malloc(sz);
	memset(ptr, 0, sz);
	return(ptr);
}

void bgbtac_free(void *ptr)
{
	free(ptr);
}

void BGBTAC_EncodeBlockStereoS16(s16 *iblk, byte *oblk)
{
	int tcma[8], tcna[8];
	int tcmb[8], tcnb[8];
	int lm, ln, cm, rm, rn, cn, tl, tr, tc;
	int cm2, cn2, ca2;
	int p0, p1, p2, p3;
	int i, j, k;

	lm= 99999; rm= 99999; cm= 99999;
	ln=-99999; rn=-99999; cn=-99999;
	
	for(i=0; i<128; i++)
	{
		tl=iblk[i*2+0];
		tr=iblk[i*2+1];
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
		for(j=0; j<16; j++)
		{
			tl=iblk[(i*16+j)*2+0];
			tr=iblk[(i*16+j)*2+1];
			tc=(tl+tr)>>1;
			if(tc<cm2) { cm2=tc; }
			if(tc>cn2) { cn2=tc; }
		}
		
		j=16384/(cn-cm);
		p0=(j*(cm2-cm)+7)>>4;
		p1=(j*(cn2-cm)+7)>>4;
		
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
		ca2=(tcmb[i]+tcnb[i])>>2;
	
		for(j=0; j<16; j++)
		{
			k=i*16+j;
		
			tl=iblk[k*2+0];
			tr=iblk[k*2+1];
			tc=(tl+tr)>>1;

			if(tc>=ca2)
				{ oblk[16+(k>>3)]|=1<<(k&7); }
			else
				{ oblk[16+(k>>3)]&=~(1<<(k&7)); }
		}
	}	
}

BTAC_API void BGBTAC_EncodeStreamStereoS16(s16 *iblk, byte *oblk, int cnt)
{
	int i, j, n;
	
	n=(cnt+127)/128;
	for(i=0; i<n; i++)
	{
		BGBTAC_EncodeBlockStereoS16(iblk+i*2*128, oblk+i*32);
	}
}


void BGBTAC_EncodeBlockMonoS16(s16 *iblk, byte *oblk)
{
	int tcma[8], tcna[8];
	int tcmb[8], tcnb[8];
	int lm, ln, cm, rm, rn, cn, tl, tr, tc;
	int cm2, cn2, ca2;
	int p0, p1, p2, p3;
	int i, j, k;

	lm= 99999; rm= 99999; cm= 99999;
	ln=-99999; rn=-99999; cn=-99999;
	
	for(i=0; i<128; i++)
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
		for(j=0; j<16; j++)
		{
			tl=iblk[i*16+j]; tr=tl;
//			tl=(iblk[i*16+j]+7)>>4; tr=tl;
			tc=(tl+tr)>>1;
			if(tc<cm2) { cm2=tc; }
			if(tc>cn2) { cn2=tc; }
		}
		
		if(cn!=cm)
//			{ j=16384/(cn-cm); }
			{ j=16777216/(cn-cm); }
		else { j=0; }

//		p0=(j*(cm2-cm)+255)>>9;
//		p1=(j*(cn2-cm)+255)>>9;

		p0=(j*(cm2-cm)+262144)>>19;
		p1=(j*(cn2-cm)+262144)>>19;
		
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
		ca2=(tcmb[i]+tcnb[i])>>1;
	
		for(j=0; j<16; j++)
		{
			k=i*16+j;
		
			tl=iblk[k]; tr=tl;
//			tl=(iblk[k]+7)>>4; tr=tl;
			tc=(tl+tr)>>1;

			if(tc>=ca2)
				{ oblk[16+(k>>3)]|=1<<(k&7); }
			else
				{ oblk[16+(k>>3)]&=~(1<<(k&7)); }
		}
	}	
}

BTAC_API void BGBTAC_EncodeStreamMonoS16(s16 *iblk, byte *oblk, int len)
{
	int i, j, n;
	
	n=(len+127)/128;
	for(i=0; i<n; i++)
	{
		BGBTAC_EncodeBlockMonoS16(iblk+i*128, oblk+i*32);
	}
}

void BGBTAC_DecodeBlockMonoS16(byte *iblk, s16 *oblk)
{
	int tcma[8], tcna[8];
	int tcmb[8], tcnb[8];
	int p0, p1, p2, p3;
	int cm, cn, ca;
	int cm2, cn2, ca2;
	int i, j, k;
	
	cm=(s16)(iblk[0]+(iblk[1]<<8));
	cn=(s16)(iblk[2]+(iblk[3]<<8));
	ca=(cm+cn)>>1;

	for(i=0; i<128; i++)
		{ oblk[i]=ca; }

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
		
		for(j=0; j<16; j++)
		{
			k=i*16+j;
		
//			oblk[i*16+j]=p2;
			oblk[i*16+j]=(iblk[16+(k>>3)]&(1<<(k&7)))?p1:p0;
		}
	}
}

BTAC_API void BGBTAC_DecodeStreamMonoS16(byte *iblk, s16 *oblk, int len)
{
	int i, j, n;
	
	n=(len+127)/128;
	for(i=0; i<n; i++)
	{
		BGBTAC_DecodeBlockMonoS16(iblk+i*32, oblk+i*128);
	}
}






void BGBTAC_EncodeBlock2MonoS16(s16 *iblk, byte *oblk)
{
	int tcma[8], tcna[8];
	int tcmb[8], tcnb[8];
	int tsc[16];
	int lm, ln, cm, rm, rn, cn, tl, tr, tc;
	int cm2, cn2, ca2;
	int p0, p1, p2, p3;
	int s0, s1, s2, s3;
	int i, j, k;

	lm= 99999; rm= 99999; cm= 99999;
	ln=-99999; rn=-99999; cn=-99999;
	
	for(i=0; i<128; i++)
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
		for(j=0; j<16; j++)
		{
			tl=iblk[i*16+j]; tr=tl;
//			tl=(iblk[i*16+j]+7)>>4; tr=tl;
			tc=(tl+tr)>>1;
			if(tc<cm2) { cm2=tc; }
			if(tc>cn2) { cn2=tc; }
		}
		
		if(cn!=cm)
//			{ j=16384/(cn-cm); }
			{ j=16777216/(cn-cm); }
		else { j=0; }

//		p0=(j*(cm2-cm)+255)>>9;
//		p1=(j*(cn2-cm)+255)>>9;

//		p0=(j*(cm2-cm)+262144)>>19;
//		p1=(j*(cn2-cm)+262144)>>19;

//		p0=(p0<0)?0:((p0<31)?p0:31);
//		p1=(p1<0)?0:((p1<31)?p1:31);


//		p0=(j*(cm2-cm)+32768)>>16;
//		p1=(j*(cn2-cm)+32768)>>16;

		p0=(j*(cm2-cm)+16384)>>16;
		p1=(j*(cn2-cm)+49152)>>16;
		
		if((p0<0) || (p1<0) || (p0>256) || (p1>256))
			printf("%d %d\n", p0, p1);
		
		p0=(p0<0)?0:((p0<256)?p0:255);
		p1=(p1<0)?0:((p1<256)?p1:255);
		
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

	for(i=0; i<8; i++)
	{
		oblk[16+i*2+0]=tcma[i];
		oblk[16+i*2+1]=tcna[i];
	}
	
	for(i=0; i<8; i++)
	{
		cm2=tcmb[i]; cn2=tcnb[i];
		ca2=(tcmb[i]+tcnb[i])>>1;

		if(cn2!=cm2)
			{ s0=16777216/(cn2-cm2); }
		else { s0=0; }
	
		for(j=0; j<16; j++)
		{
			k=i*16+j;
		
			tl=iblk[k]; tr=tl;
//			tl=(iblk[k]+7)>>4; tr=tl;
			tc=(tl+tr)>>1;

//			p0=(s0*(tc-cm2)+255)>>9;
//			p0=(s0*(tc-cm2)+1048576)>>21;
			p0=(s0*(tc-cm2)+2097152)>>22;

			if((p0<0) || (p0>4))
				printf("%d\n", p0);

			p0=(p0<0)?0:((p0<4)?p0:3);

//			p0=(tc>=ca2)?3:0;
//			tsc[j]=p0;

			oblk[32+(k>>2)]&=~(3<<((k&3)*2));
			oblk[32+(k>>2)]|=p0<<((k&3)*2);

//			if(tc>=ca2)
//				{ oblk[16+(k>>3)]|=1<<(k&7); }
//			else
//				{ oblk[16+(k>>3)]&=~(1<<(k&7)); }
		}

//		oblk[16+(i*6)+0]= tsc[ 0]    |(tsc[ 1]<<3)|(tsc[ 2]<<6);
//		oblk[16+(i*6)+1]=(tsc[ 2]>>2)|(tsc[ 3]<<1)|(tsc[ 4]<<4)|(tsc[ 5]<<7);
//		oblk[16+(i*6)+2]=(tsc[ 5]>>1)|(tsc[ 6]<<2)|(tsc[ 7]<<5);
//		oblk[16+(i*6)+3]= tsc[ 8]    |(tsc[ 9]<<3)|(tsc[10]<<6);
//		oblk[16+(i*6)+4]=(tsc[10]>>2)|(tsc[11]<<1)|(tsc[12]<<4)|(tsc[13]<<7);
//		oblk[16+(i*6)+5]=(tsc[13]>>1)|(tsc[14]<<2)|(tsc[14]<<5);
	}	
}

void BGBTAC_DecodeBlock2MonoS16(byte *iblk, s16 *oblk)
{
	int tcma[8], tcna[8];
	int tcmb[8], tcnb[8];
	int tsc[16];
	int p0, p1, p2, p3;
	int s0, s1, s2, s3;
	int cm, cn, ca;
	int cm2, cn2, ca2;
	int i, j, k;
	
	cm=(s16)(iblk[0]+(iblk[1]<<8));
	cn=(s16)(iblk[2]+(iblk[3]<<8));
	ca=(cm+cn)>>1;

	for(i=0; i<128; i++)
		{ oblk[i]=ca; }

	// 6: aaaa abbb
	// 7: bbcc cccd
	// 8: dddd eeee
	// 9: efff ffgg
	//10: gggh hhhh

#if 0
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

	for(i=0; i<8; i++)
	{
		tcmb[i]=iblk[16+i*2+0];
		tcnb[i]=iblk[16+i*2+1];
	}

	for(i=0; i<8; i++)
	{
		cm2=tcmb[i];
		cn2=tcnb[i];
	
		j=(cn-cm);
//		p0=((j*cm2+16)>>5)+cm;
//		p1=((j*cn2+16)>>5)+cm;

		p0=((j*cm2+128)>>8)+cm;
		p1=((j*cn2+128)>>8)+cm;

		p0=(p0<cm)?cm:((p0<cn)?p0:cn);
		p1=(p1<cm)?cm:((p1<cn)?p1:cn);
	
		tcma[i]=p0;
		tcna[i]=p1;
		
		p2=(p0+p1)>>1;
//		if(p2<cm)p2=cm;
//		if(p2>cn)p2=cn;
		
		for(j=0; j<16; j++)
		{
//			k=j*3;
//			p3=((iblk[(16+i*6+0)+(k>>3)]|
//				(iblk[(16+i*6+1)+(k>>3)]<<8))>>(k&7))&7;

			k=j*2;
			p3=(iblk[(32+i*4+0)+(k>>3)]>>(k&7))&3;
			tsc[j]=p3;
		}

		cm2=tcma[i];
		cn2=tcna[i];
		s1=(cn2-cm2);
		
		for(j=0; j<16; j++)
		{
			p3=((s1*tsc[j]+1)>>2)+cm2;
			p3=(p3<cm2)?cm2:((p3<cn2)?p3:cn2);
			oblk[i*16+j]=p3;

//			k=i*16+j;
		
//			oblk[i*16+j]=p2;
//			oblk[i*16+j]=(iblk[16+(k>>3)]&(1<<(k&7)))?p1:p0;
		}
	}
}

BTAC_API void BGBTAC_EncodeStream2MonoS16(s16 *iblk, byte *oblk, int len)
{
	int i, j, n;
	
	n=(len+127)/128;
	for(i=0; i<n; i++)
	{
		BGBTAC_EncodeBlock2MonoS16(iblk+i*128, oblk+i*64);
	}
}

BTAC_API void BGBTAC_DecodeStream2MonoS16(byte *iblk, s16 *oblk, int len)
{
	int i, j, n;
	
	n=(len+127)/128;
	for(i=0; i<n; i++)
	{
		BGBTAC_DecodeBlock2MonoS16(iblk+i*64, oblk+i*128);
	}
}
