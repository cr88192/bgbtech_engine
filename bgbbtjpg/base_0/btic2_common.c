static int trans8[256]={
0x00, 0x80, 0x40, 0xC0, 0x20, 0xA0, 0x60, 0xE0,
0x10, 0x90, 0x50, 0xD0, 0x30, 0xB0, 0x70, 0xF0,
0x08, 0x88, 0x48, 0xC8, 0x28, 0xA8, 0x68, 0xE8,
0x18, 0x98, 0x58, 0xD8, 0x38, 0xB8, 0x78, 0xF8,
0x04, 0x84, 0x44, 0xC4, 0x24, 0xA4, 0x64, 0xE4,
0x14, 0x94, 0x54, 0xD4, 0x34, 0xB4, 0x74, 0xF4,
0x0C, 0x8C, 0x4C, 0xCC, 0x2C, 0xAC, 0x6C, 0xEC,
0x1C, 0x9C, 0x5C, 0xDC, 0x3C, 0xBC, 0x7C, 0xFC,
0x02, 0x82, 0x42, 0xC2, 0x22, 0xA2, 0x62, 0xE2,
0x12, 0x92, 0x52, 0xD2, 0x32, 0xB2, 0x72, 0xF2,
0x0A, 0x8A, 0x4A, 0xCA, 0x2A, 0xAA, 0x6A, 0xEA,
0x1A, 0x9A, 0x5A, 0xDA, 0x3A, 0xBA, 0x7A, 0xFA,
0x06, 0x86, 0x46, 0xC6, 0x26, 0xA6, 0x66, 0xE6,
0x16, 0x96, 0x56, 0xD6, 0x36, 0xB6, 0x76, 0xF6,
0x0E, 0x8E, 0x4E, 0xCE, 0x2E, 0xAE, 0x6E, 0xEE,
0x1E, 0x9E, 0x5E, 0xDE, 0x3E, 0xBE, 0x7E, 0xFE,
0x01, 0x81, 0x41, 0xC1, 0x21, 0xA1, 0x61, 0xE1,
0x11, 0x91, 0x51, 0xD1, 0x31, 0xB1, 0x71, 0xF1,
0x09, 0x89, 0x49, 0xC9, 0x29, 0xA9, 0x69, 0xE9,
0x19, 0x99, 0x59, 0xD9, 0x39, 0xB9, 0x79, 0xF9,
0x05, 0x85, 0x45, 0xC5, 0x25, 0xA5, 0x65, 0xE5,
0x15, 0x95, 0x55, 0xD5, 0x35, 0xB5, 0x75, 0xF5,
0x0D, 0x8D, 0x4D, 0xCD, 0x2D, 0xAD, 0x6D, 0xED,
0x1D, 0x9D, 0x5D, 0xDD, 0x3D, 0xBD, 0x7D, 0xFD,
0x03, 0x83, 0x43, 0xC3, 0x23, 0xA3, 0x63, 0xE3,
0x13, 0x93, 0x53, 0xD3, 0x33, 0xB3, 0x73, 0xF3,
0x0B, 0x8B, 0x4B, 0xCB, 0x2B, 0xAB, 0x6B, 0xEB,
0x1B, 0x9B, 0x5B, 0xDB, 0x3B, 0xBB, 0x7B, 0xFB,
0x07, 0x87, 0x47, 0xC7, 0x27, 0xA7, 0x67, 0xE7,
0x17, 0x97, 0x57, 0xD7, 0x37, 0xB7, 0x77, 0xF7,
0x0F, 0x8F, 0x4F, 0xCF, 0x2F, 0xAF, 0x6F, 0xEF,
0x1F, 0x9F, 0x5F, 0xDF, 0x3F, 0xBF, 0x7F, 0xFF
};

int BTIC2BH_TransposeByte(int v)
{
	return(trans8[v]);
}

int BTIC2BH_TransposeWord(int v)
{
	int i;
	i=trans8[(v>>8)&0xFF]|(trans8[v&0xFF]<<8);
	return(i);
}

int BTIC2BH_SetupTable(byte *cl, int ncl,
	u16 *tc, u16 *tm, byte *tl,
	u16 *ti, u16 *tn)
{
	int cnt[16], nc[16], nn[16], nf[16];
	int i, j, k, k2, l, ff;

	for(i=0; i<16; i++) { cnt[i]=0; nc[i]=0; nn[i]=-1; nf[i]=-1; }
	for(i=0; i<256; i++)ti[i]=0xFFFF;

	for(i=0; i<ncl; i++)cnt[cl[i]]++;
	cnt[0]=0;

#if 1
	j=cl[0];
	for(i=15; i>=1; i--)
		if(cnt[i])break;
	if(j>i)j=i;
	if(!i)return(-12);	//no codes

	j=1;
	for(i=1; i<16; i++)
	{
		j<<=1;
		j-=cnt[i];

//		dyPrintf("%d %d %d\n", i, j, cnt[i]);
		if(j<0)return(-10);	//over subscribed
	}
	if((j>0) && ((ncl-cnt[0])!=1))
		return(-11);	//incomplete set
#endif

	j=0;
	for(i=1; i<16; i++)
	{
		j=(j+cnt[i-1])<<1;
		nc[i]=j;
	}

	for(i=0; i<ncl; i++)
	{
		l=cl[i];
		if(!l)continue;

		tl[i]=cl[i];
		tm[i]=(1<<tl[i])-1;
		tc[i]=nc[l]++;

		if(nn[l]>=0)
		{
			tn[nn[l]]=i;
			nn[l]=i;
		}else
		{
			nf[l]=i;
			nn[l]=i;
		}
	}

	j=-1; ff=-1;
	for(i=1; i<16; i++)
	{
		if(nf[i]>=0)
		{
			if(ff<0)ff=nf[i];
			if(j>=0)tn[j]=nf[i];
			j=nn[i];
		}
	}
	if(j>=0)tn[j]=0xFFFF;

	for(i=ff; i!=0xFFFF; i=tn[i])
	{
		l=cl[i];
		if(l<=8)
		{
			k2=tc[i]<<(8-l);
			j=1<<(8-l);
			while(j--)
			{
				k=BTIC2BH_TransposeByte(k2++);
				if(ti[k]==0xFFFF)ti[k]=i;
			}
		}else
		{
			k2=tc[i]>>(l-8);
			k=BTIC2BH_TransposeByte(k2);
			if(ti[k]==0xFFFF)ti[k]=i;
		}

		k2=(tc[i])<<(16-l);
		k=BTIC2BH_TransposeWord(k2++);
		tc[i]=k;
	}

	j=0;
	for(i=0; i<256; i++)if(ti[i]==0xFFFF)
	{
		printf("table bad index %d\n", i);
		j=-9;
	}
	return(j);
}


int BTIC2B_Paeth(int a, int b, int c)
{
	int p, pa, pb, pc;

	p=a+b-c;
	pa=(p>a)?(p-a):(a-p);
	pb=(p>b)?(p-b):(b-p);
	pc=(p>c)?(p-c):(c-p);

	p=(pa<=pb)?((pa<=pc)?a:c):((pb<=pc)?b:c);
	return(p);
}