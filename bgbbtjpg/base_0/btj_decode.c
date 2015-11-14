#include <bgbbtj.h>

extern const int pdjpg_zigzag[64];
extern const int pdjpg_zigzag2[64];

extern float pdjpg_exptab[256];
extern float pdjpg_scltab[256];

//Decoder

int BGBBTJ_JPG_MarkerSOS(BGBBTJ_JPG_Context *ctx, byte *buf)
{
	short dbuf[4*64];
	byte ch[4], cv[4], qid[4];
	byte step[4];
	int xi[4], yi[4], wi[4], hi[4];
	byte cdt[4], cat[4];
	int ccnt[4];

	byte *s;
	int i, j, k, l, i1, j1, k1, l1;
	int w, h, n, ns, rt;

	l=(buf[0]<<8)|buf[1];
//	printf("ns %d\n", buf[2]);

#if 1
	if((ctx->xs!=ctx->lxs) || (ctx->ys!=ctx->lys))
	{
		for(i=0; i<4; i++)
		{
			if(ctx->jpg_scbuf[i])free(ctx->jpg_scbuf[i]);
			if(ctx->jpg_sibuf[i])free(ctx->jpg_sibuf[i]);
			ctx->jpg_scbuf[i]=NULL;
			ctx->jpg_sibuf[i]=NULL;
		}
	}
#endif

	ns=buf[2]; rt=0;
	s=buf+3;
	for(i=0; i<buf[2]; i++)
	{
//		printf("%d %d %d\n", s[0], s[1]>>4, s[1]&15);

		cdt[i]=s[1]>>4;
		cat[i]=s[1]&15;

		for(j=0; j<ctx->jpg_nc; j++)
			if(ctx->jpg_cid[j]==s[0])
		{
			ch[i]=ctx->jpg_ch[j];
			cv[i]=ctx->jpg_cv[j];
			qid[i]=ctx->jpg_qid[j];
			xi[i]=ctx->jpg_cxi[j];
			yi[i]=ctx->jpg_cyi[j];

			step[i]=ch[i]*cv[i];
			break;
		}

		w=(xi[i]+7)/8;
		h=(yi[i]+7)/8;
//		w=(xi[i]+15)/8;
//		h=(yi[i]+15)/8;
		wi[i]=w;
		hi[i]=h;

		n=(w+1)*(h+2);
		ctx->jpg_scid[i]=s[0];
		ctx->jpg_scn[i]=j;
		if(!ctx->jpg_scbuf[i])
			ctx->jpg_scbuf[i]=malloc(n*64*sizeof(short));
		if(!ctx->jpg_sibuf[i])
			ctx->jpg_sibuf[i]=malloc(n*64*sizeof(short));

//		memset(ctx->jpg_sibuf[i], 0, n*64);

		ccnt[i]=0;

		s+=2;
	}
	s+=3;

	i=(ctx->xs+ctx->jpg_chm*8-1)/(ctx->jpg_chm*8);
	j=(ctx->ys+ctx->jpg_chn*8-1)/(ctx->jpg_chn*8);
	n=i*j;

#if 1
	for(i=0; i<ctx->jpg_nc; i++)
	{
		j=ctx->jpg_scn[i];
//		ctx->jpg_schsc[i]=ctx->jpg_ch[j]/(ctx->jpg_chm*1.0);
//		ctx->jpg_scvsc[i]=ctx->jpg_cv[j]/(ctx->jpg_chn*1.0);

		ctx->jpg_schsci[i]=(65536*ctx->jpg_ch[j])/ctx->jpg_chm;
		ctx->jpg_scvsci[i]=(65536*ctx->jpg_cv[j])/ctx->jpg_chn;

		ctx->jpg_schs[i]=(ctx->jpg_cxi[j]+7)/8;
		ctx->jpg_scvs[i]=(ctx->jpg_cyi[j]+7)/8;
		
	}

	if(ctx->jpg_nc==3)
	{
		i=1;
		if(ctx->jpg_schsci[0]!=65536)i=0;
		if(ctx->jpg_scvsci[0]!=65536)i=0;
		if(ctx->jpg_schsci[1]!=32768)i=0;
		if(ctx->jpg_scvsci[1]!=32768)i=0;
		if(ctx->jpg_schsci[2]!=32768)i=0;
		if(ctx->jpg_scvsci[2]!=32768)i=0;
		ctx->jpg_is420=i;

		i=1;
		if(ctx->jpg_schsci[0]!=65536)i=0;
		if(ctx->jpg_scvsci[0]!=65536)i=0;
		if(ctx->jpg_schsci[1]!=65536)i=0;
		if(ctx->jpg_scvsci[1]!=65536)i=0;
		if(ctx->jpg_schsci[2]!=65536)i=0;
		if(ctx->jpg_scvsci[2]!=65536)i=0;
		ctx->jpg_is444=i;
	}else
	{
		ctx->jpg_is420=0;
		ctx->jpg_is444=0;
	}
#endif

	BGBBTJ_Huff_InitStream(ctx, s);

	for(i=0; i<(4*64); i++)dbuf[i]=0;

	if(ctx->jpg_nc==1)
	{
		rt=0;
		for(i=0; i<n; i++)
		{
			rt=BGBBTJ_Huff_DecodeBlock(ctx, dbuf,
				cdt[0]*2+0, cat[0]*2+1, i, n);
			if(rt<0)break;
			BGBBTJ_Huff_DequantBlock(ctx, dbuf,
				ctx->jpg_scbuf[0]+i*64, qid[0]);
			BGBBTJ_JPG_TransIDCT(
				ctx->jpg_scbuf[0]+i*64,
				ctx->jpg_sibuf[0]+i*64, 128);
		}
	}else
	if(ctx->jpg_is420 && !ctx->jpg_rdct)
//	if(0)
	{
		rt=0;
		for(i=0; i<n; i++)
		{
#if 1
			j=wi[0]>>1;
			i1=(i/j)<<1;
			j1=(i%j)<<1;

			k=((i1+0)*wi[0])+(j1+0);
			rt=BGBBTJ_Huff_DecodeBlock(ctx, dbuf,
				cdt[0]*2+0, cat[0]*2+1, i, n);
			if(rt<0)break;
			BGBBTJ_Huff_DequantBlock(ctx, dbuf,
				ctx->jpg_scbuf[0]+k*64, qid[0]);
			BGBBTJ_JPG_TransIDCT(
				ctx->jpg_scbuf[0]+k*64,
				ctx->jpg_sibuf[0]+k*64, 128);

			k=((i1+0)*wi[0])+(j1+1);
			rt=BGBBTJ_Huff_DecodeBlock(ctx, dbuf,
				cdt[0]*2+0, cat[0]*2+1, i, n);
			if(rt<0)break;
			BGBBTJ_Huff_DequantBlock(ctx, dbuf,
				ctx->jpg_scbuf[0]+k*64, qid[0]);
			BGBBTJ_JPG_TransIDCT(
				ctx->jpg_scbuf[0]+k*64,
				ctx->jpg_sibuf[0]+k*64, 128);

			k=((i1+1)*wi[0])+(j1+0);
			rt=BGBBTJ_Huff_DecodeBlock(ctx, dbuf,
				cdt[0]*2+0, cat[0]*2+1, i, n);
			if(rt<0)break;
			BGBBTJ_Huff_DequantBlock(ctx, dbuf,
				ctx->jpg_scbuf[0]+k*64, qid[0]);
			BGBBTJ_JPG_TransIDCT(
				ctx->jpg_scbuf[0]+k*64,
				ctx->jpg_sibuf[0]+k*64, 128);

			k=((i1+1)*wi[0])+(j1+1);
			rt=BGBBTJ_Huff_DecodeBlock(ctx, dbuf,
				cdt[0]*2+0, cat[0]*2+1, i, n);
			if(rt<0)break;
			BGBBTJ_Huff_DequantBlock(ctx, dbuf,
				ctx->jpg_scbuf[0]+k*64, qid[0]);
			BGBBTJ_JPG_TransIDCT(
				ctx->jpg_scbuf[0]+k*64,
				ctx->jpg_sibuf[0]+k*64, 128);
#endif

#if 1
			rt=BGBBTJ_Huff_DecodeBlock(ctx, dbuf+1*64,
				cdt[1]*2+0, cat[1]*2+1, i, n);
			if(rt<0)break;
			BGBBTJ_Huff_DequantBlock(ctx, dbuf+1*64,
				ctx->jpg_scbuf[1]+i*64, qid[1]);
			BGBBTJ_JPG_TransIDCT(
				ctx->jpg_scbuf[1]+i*64,
				ctx->jpg_sibuf[1]+i*64, 128);

			rt=BGBBTJ_Huff_DecodeBlock(ctx, dbuf+2*64,
				cdt[2]*2+0, cat[2]*2+1, i, n);
			if(rt<0)break;
			BGBBTJ_Huff_DequantBlock(ctx, dbuf+2*64,
				ctx->jpg_scbuf[2]+i*64, qid[2]);
			BGBBTJ_JPG_TransIDCT(
				ctx->jpg_scbuf[2]+i*64,
				ctx->jpg_sibuf[2]+i*64, 128);
#endif
		}
	}else
	{
		rt=0;
		for(i=0; i<n; i++)
		{
			for(j=0; j<ns; j++)
			{
				for(k=0; k<cv[j]; k++)
				{
					for(l=0; l<ch[j]; l++)
					{
						if(rt<0)break;

						i1=(i/(wi[j]/ch[j]))*ch[j];
						j1=(i%(wi[j]/ch[j]))*ch[j];
						k1=((i1+k)*wi[j])+(j1+l);

						rt=BGBBTJ_Huff_DecodeBlock(ctx, dbuf+j*64,
							cdt[j]*2+0, cat[j]*2+1, i, n);
						if(rt<0)break;
						BGBBTJ_Huff_DequantBlock(ctx, dbuf+j*64,
							ctx->jpg_scbuf[j]+k1*64, qid[j]);
						
						if(ctx->jpg_rdct)
						{
							BGBBTJ_JPG_TransIRDCT(ctx->jpg_scbuf[j]+k1*64,
								ctx->jpg_sibuf[j]+k1*64, 128);
						}else
						{
							BGBBTJ_JPG_TransIDCT(ctx->jpg_scbuf[j]+k1*64,
								ctx->jpg_sibuf[j]+k1*64, 128);

//							BGBBTJ_JPG_TransIQDCT(ctx->jpg_scbuf[j]+k1*64,
//								ctx->jpg_sibuf[j]+k1*64, 128);
						}
					}
					if(rt<0)break;
				}
				if(rt<0)break;
			}
			if(rt<0)break;
		}
	}

	if(rt<0)
	{
		printf("BGBBTJ_JPG_MarkerSOS: Bad Image\n");
		return(-1);
	}

	i=ctx->huff_cs-buf;
	return(i);

//	return(0);
}

int BGBBTJ_JPG_MarkerSOF0(BGBBTJ_JPG_Context *ctx, byte *buf)
{
	byte *cnt, *s;
	int i, j, k, l, m, n;

	l=(buf[0]<<8)|buf[1];
//	printf("p %d\n", buf[2]);

	i=(buf[3]<<8)|buf[4];
	j=(buf[5]<<8)|buf[6];

	ctx->xs=j;
	ctx->ys=i;
//	printf("size %dx%d\n", j, i);

	if((ctx->xs!=ctx->lxs) || (ctx->ys!=ctx->lys))
	{
		if(ctx->badb)
		{
			free(ctx->badb);
			ctx->badb=NULL;
		}
	}

	m=0; n=0;

	k=buf[7];
	s=buf+8;
	for(i=0; i<k; i++)
	{
		ctx->jpg_cid[i]=s[0];
		ctx->jpg_ch[i]=s[1]>>4;
		ctx->jpg_cv[i]=s[1]&15;
		ctx->jpg_qid[i]=s[2];

		if(ctx->jpg_ch[i]>m)m=ctx->jpg_ch[i];
		if(ctx->jpg_cv[i]>n)n=ctx->jpg_cv[i];

//		printf("CID %d: %d %d %d\n", ctx->jpg_cid[i],
//			ctx->jpg_ch[i], ctx->jpg_cv[i], ctx->jpg_qid[i]);
		s+=3;
	}

	ctx->jpg_chm=m;
	ctx->jpg_chn=n;

	for(i=0; i<k; i++)
	{
		ctx->jpg_cxi[i]=(ctx->jpg_ch[i]*ctx->xs)/m;
		ctx->jpg_cyi[i]=(ctx->jpg_cv[i]*ctx->ys)/n;
	}

	ctx->jpg_nc=k;

	return(0);
}

int BGBBTJ_JPG_MarkerDQT(BGBBTJ_JPG_Context *ctx, byte *buf)
{
	byte *s, *se;
	int i, j, l;

	l=(buf[0]<<8)|buf[1];

	s=buf+2;
	se=buf+l;

	while(s<se)
	{
		i=(s[0]>>4)&15;
		if(i)
		{
//			printf("bad bits %d\n", i);
			return(-1);
		}
		i=s[0]&15;
//		printf("dest %d\n", i);

		for(j=0; j<64; j++)
			ctx->jpg_qt[i][pdjpg_zigzag2[j]]=s[j+1];
		s+=65;
	}
	
	return(s-buf);
//	return(0);
}

int BGBBTJ_JPG_MarkerDHT(BGBBTJ_JPG_Context *ctx, byte *buf)
{
	byte *cnt, *s;
	int i, j, k, l, tn;

	l=(buf[0]<<8)|buf[1];

	j=(buf[2]>>4)&15;
	i=buf[2]&15;
//	printf("dest %d, type %d\n", i, j);

	tn=i*2+j;

	for(i=0; i<256; i++)
	{
		ctx->huff_len[tn*256+i]=0;
		ctx->huff_next[tn*256+i]=-1;
		ctx->huff_idx[tn*256+i]=-1;
	}

	cnt=buf+3;
	s=cnt+16;
	k=0; l=0;
	for(i=0; i<16; i++)
	{
		k<<=1;
		for(j=0; j<cnt[i]; j++)
		{
			ctx->huff_code[tn*256+(*s)]=k++;
			ctx->huff_len[tn*256+(*s)]=i+1;
			ctx->huff_mask[tn*256+(*s)]=(1<<(i+1))-1;
			l=i+1;
			s++;
		}
	}
//	printf("%04X %d\n", k, l);

	//build lookup indices / chains

	for(i=0; i<256; i++)
	{
//		ctx->huff_next[tn*256+i]=0;
//		ctx->huff_idx[tn*256+i]=0;
		ctx->huff_next[tn*256+i]=-1;
		ctx->huff_idx[tn*256+i]=-1;
	}

	for(i=0; i<256; i++)
	{
		l=ctx->huff_len[tn*256+i];
		if(!l)continue;
		j=ctx->huff_code[tn*256+i];
		
		if(l<=8)
		{
			j=j<<(8-l);
			k=1<<(8-l);
			while((k--) && (j<256))
			{
				ctx->huff_idx[tn*256+j]=i;
				j++;
			}
		}else
		{
			j=j>>(l-8);
			ctx->huff_next[tn*256+i]=
				ctx->huff_idx[tn*256+j];
			ctx->huff_idx[tn*256+j]=i;
		}
	}

	return(s-buf);
//	return(0);
}

int BGBBTJ_JPG_MarkerAPP0(BGBBTJ_JPG_Context *ctx, byte *buf)
{
	return(0);
}

int BGBBTJ_JPG_MarkerAPP9(BGBBTJ_JPG_Context *ctx, byte *buf)
{
	int i;
	buf+=2;

	if(!memcmp(buf, "JPSERM", 6))
	{
		ctx->jpg_rdct=1;
		return(1);
	}
	
	return(0);
}

int BGBBTJ_JPG_MarkerAPP11(BGBBTJ_JPG_Context *ctx, byte *buf)
{
	int cr, cg, cb, ca;

	buf+=2;

	if(!strcmp((char *)buf, "AlphaColor"))
	{
		buf+=strlen((char *)buf)+1;
		sscanf(buf, "%d %d %d %d", &cr, &cg, &cb, &ca);
		BGBBTJ_JPG_SetContextAlphaColor(ctx, cr, cg, cb, ca);
		return(1);
	}
	
	return(0);
}

int BGBBTJ_JPG_MarkerAPP12(BGBBTJ_JPG_Context *ctx, byte *buf)
{
	byte *tbuf;
	int i, sz, sz1;
	
	sz=(buf[0]<<8)|buf[1];
	buf+=2;

	i=(buf[0]<<24)|(buf[1]<<16)|(buf[2]<<8)|buf[3];
//	printf("BGBBTJ_JPG_MarkerAPP12: %08X\n", i);

	if(!memcmp(buf, "BCST", 4))
	{
		ctx->bcst_version=(buf[4]<<8)|buf[5];
		ctx->bcst_trans=buf[6];
		ctx->bcst_bpp=buf[7];
		ctx->bcst_flags=
			(buf[8]<<24)|(buf[9]<<16)|
			(buf[10]<<8)|buf[11];

		if(sz>14)
		{
			ctx->bcst_alpha=buf[12];
			ctx->bcst_blend=buf[13];
			ctx->bcst_orgx=((buf[14]<<8)|buf[15]);
			ctx->bcst_orgy=((buf[16]<<8)|buf[17]);

			ctx->bcst_minx = ((buf[18] << 8) | buf[19]);
			ctx->bcst_miny = ((buf[20] << 8) | buf[21]);
			ctx->bcst_maxx = ((buf[22] << 8) | buf[23]);
			ctx->bcst_maxy = ((buf[24] << 8) | buf[25]);
		}
		else
		{
			ctx->bcst_alpha=255;
			ctx->bcst_blend=0;
			ctx->bcst_orgx=0;
			ctx->bcst_orgy=0;

			ctx->bcst_minx = 0;
			ctx->bcst_miny = 0;
			ctx->bcst_maxx = 0;
			ctx->bcst_maxy = 0;
		}

		ctx->jpg_clrtrans=ctx->bcst_trans;
		
		if(ctx->bcst_flags&BGBBTJ_JPG_BCSFL_RDCT)
			ctx->jpg_rdct=1;
			
//		printf("BGBBTJ_JPG_MarkerAPP12: BCST %04X %02X %02X %08X\n",
//			ctx->bcst_version, ctx->bcst_trans,
//			ctx->bcst_bpp, ctx->bcst_flags);
		return(1);
	}

	if(!memcmp(buf, "BA1Z", 4))
	{
		if(ctx->badb)
		{
			PDUNZ_DecodeStreamSz(buf+6, ctx->badb, sz-8,
				ctx->sz_badb, &sz1, BGBBTJ_ZFL_ESCAPE_FF);
		}else
		{
			tbuf=malloc(1<<20);
			PDUNZ_DecodeStreamSz(buf+6, tbuf, sz-8, 1<<20, &sz1,
				BGBBTJ_ZFL_ESCAPE_FF);
			ctx->badb=malloc(sz1);
			ctx->sz_badb=sz1;
			memcpy(ctx->badb, tbuf, sz1);
			free(tbuf);
		}
	}

	return(0);
}

int BGBBTJ_JPG_MarkerAPP14(BGBBTJ_JPG_Context *ctx, byte *buf)
{
	buf+=2;

#if 0
	if(!memcmp((char *)buf, "Adobe", 5))
	{
		ctx->jpg_app14_dctversion=(buf[5]<<8)|buf[6];
		ctx->jpg_app14_flags0=(buf[7]<<8)|buf[8];
		ctx->jpg_app14_flags1=(buf[9]<<8)|buf[10];
		ctx->jpg_app14_trans=buf[11];
		
		switch(ctx->jpg_app14_trans)
		{
		case 0: ctx->jpg_clrtrans=BGBBTJ_JPG_CLRS_RGB; break;
		case 1: ctx->jpg_clrtrans=BGBBTJ_JPG_CLRS_YCBCR; break;
		case 2: ctx->jpg_clrtrans=BGBBTJ_JPG_CLRS_YCCK; break;
		}
		
		return(1);
	}
#endif

	return(0);
}

int BGBBTJ_JPG_MarkerAPPN(BGBBTJ_JPG_Context *ctx, byte *buf)
{
	return(0);
}

#if 0
int BGBBTJ_JPG_GetComponentPixel(BGBBTJ_JPG_Context *ctx, int cn, int x, int y)
{
	int i, j, k, l;
	int id, w, h, n, v;

//	id=ctx->jpg_scn[cn];

	i=x*ctx->jpg_schsc[cn];
	j=y*ctx->jpg_scvsc[cn];
//	i=(ctx->jpg_ch[id]*x)/ctx->jpg_chm;
//	j=(ctx->jpg_cv[id]*y)/ctx->jpg_chn;
	k=i>>3;
	l=j>>3;

	w=ctx->jpg_schs[cn];
	h=ctx->jpg_scvs[cn];
//	w=(ctx->jpg_cxi[id]+7)/8;
//	h=(ctx->jpg_cyi[id]+7)/8;
//	w=(ctx->jpg_cxi[id]+15)/8;
//	h=(ctx->jpg_cyi[id]+15)/8;
	n=(l*w)+k;

	k=i&7;
	l=j&7;

	v=ctx->jpg_sibuf[cn][n*64+(l*8+k)];
	return(v);
}
#endif

#if 1
int BGBBTJ_JPG_GetComponentPixel(BGBBTJ_JPG_Context *ctx, int cn, int x, int y)
{
	int i, j, k, l;
	int id, w, h, n, v;

//	i=x*ctx->jpg_schsc[cn];
//	j=y*ctx->jpg_scvsc[cn];

	i=(x*ctx->jpg_schsci[cn])>>16;
	j=(y*ctx->jpg_scvsci[cn])>>16;

	k=i>>3;
	l=j>>3;

	w=ctx->jpg_schs[cn];
//	h=ctx->jpg_scvs[cn];
	n=(l*w)+k;

	k=i&7;
	l=j&7;

	v=ctx->jpg_sibuf[cn][n*64+(l*8+k)];
	return(v);
}
#endif

#if 1
int BGBBTJ_JPG_GetComponentPixelS1(BGBBTJ_JPG_Context *ctx, int cn, int x, int y)
{
	int i, j, k, l;
	int id, w, h, n, v;

	i=x>>3;
	j=y>>3;
	w=ctx->jpg_schs[cn];
//	h=ctx->jpg_scvs[cn];
	n=(j*w)+i;

	k=x&7;
	l=y&7;

	v=ctx->jpg_sibuf[cn][n*64+(l*8+k)];
	return(v);
}

int BGBBTJ_JPG_GetComponentPixelS2(BGBBTJ_JPG_Context *ctx, int cn, int x, int y)
{
	int i, j, k, l;
	int id, w, h, n, v;

	i=x>>1;
	j=y>>1;

	k=i>>3;
	l=j>>3;
	w=ctx->jpg_schs[cn];
//	h=ctx->jpg_scvs[cn];
	n=(l*w)+k;

	k=i&7;
	l=j&7;

	v=ctx->jpg_sibuf[cn][n*64+(l*8+k)];
	return(v);
}

int BGBBTJ_JPG_GetComponentPixelS122(BGBBTJ_JPG_Context *ctx, int x, int y,
	int *ry, int *ru, int *rv)
{
	int i, j, k, l;
	int t0, t1, w0, w1, n0, n1, x1, y1;

	x1=x>>1; y1=y>>1;

	w0=ctx->jpg_schs[0];
	w1=ctx->jpg_schs[1];

	i=x>>3; j=y>>3;
	k=x1>>3; l=y1>>3;
	n0=(j*w0)+i;
	n1=(l*w1)+k;

	i=x&7; j=y&7;
	k=x1&7; l=y1&7;
	t0=(n0<<6)+((j<<3)+i);
	t1=(n1<<6)+((l<<3)+k);

	*ry=ctx->jpg_sibuf[0][t0];
	*ru=ctx->jpg_sibuf[1][t1];
	*rv=ctx->jpg_sibuf[2][t1];

	return(0);
}

int BGBBTJ_JPG_GetComponentPixelS111(BGBBTJ_JPG_Context *ctx, int x, int y,
	int *ry, int *ru, int *rv)
{
	int i, j, k, l;
	int t0, t1, w0, w1, n0, n1, x1, y1;

	w0=ctx->jpg_schs[0];
	i=x>>3; j=y>>3;
	n0=(j*w0)+i;

	i=x&7; j=y&7;
	t0=(n0<<6)+((j<<3)+i);

	*ry=ctx->jpg_sibuf[0][t0];
	*ru=ctx->jpg_sibuf[1][t0];
	*rv=ctx->jpg_sibuf[2][t0];

	return(0);
}

int BGBBTJ_JPG_GetComponentPixelS420(BGBBTJ_JPG_Context *ctx, int x, int y,
	int *ry0, int *ry1, int *ry2, int *ry3,
	int *ru, int *rv)
{
	int i, j, k, l;
	int t0, t1, w0, w1, n0, n1, x1, y1;

	x1=x>>1; y1=y>>1;

	w0=ctx->jpg_schs[0];
	w1=ctx->jpg_schs[1];

	i=x>>3; j=y>>3;
	k=x1>>3; l=y1>>3;
	n0=(j*w0)+i;
	n1=(l*w1)+k;

	i=x&7; j=y&7;
	k=x1&7; l=y1&7;
	t0=(n0<<6)+((j<<3)+i);
	t1=(n1<<6)+((l<<3)+k);

	*ry0=ctx->jpg_sibuf[0][t0];
	*ry1=ctx->jpg_sibuf[0][t0+1];
	*ry2=ctx->jpg_sibuf[0][t0+8];
	*ry3=ctx->jpg_sibuf[0][t0+9];

	*ru=ctx->jpg_sibuf[1][t1];
	*rv=ctx->jpg_sibuf[2][t1];

	return(0);
}

#endif

#if 1
int BGBBTJ_JPG_GetComponentPixelAlpha(
	BGBBTJ_JPG_Context *ctx, int cn, int x, int y)
{
	int i, j, k, l;
	int id, w, h, n, v;

	if(!ctx->badb)
		return(255);

	i=(x*ctx->jpg_schsci[cn])>>16;
	j=(y*ctx->jpg_scvsci[cn])>>16;

	k=i>>3;
	l=j>>3;

	w=ctx->jpg_schs[cn];
//	h=ctx->jpg_scvs[cn];
	n=(l*w)+k;

	k=i&7;
	l=j&7;

//	v=ctx->jpg_sibuf[cn][n*64+(l*8+k)];
	v=((ctx->badb[n*8+l]>>(7-k))&1)?255:0;
	return(v);
}
#endif

int BGBBTJ_JPG_DecodeColorTransformYCbCr420(BGBBTJ_JPG_Context *ctx, byte *obuf)
{
	int y, u, v, r, g, b, a, u1, v1;
	int y0, y1, y2, y3;
	int r0, g0, b0, a0;
	int r1, g1, b1, a1;
	int r2, g2, b2, a2;
	int r3, g3, b3, a3;
	int l0, l1, l2, l3;
	int i, j, k, l;

#if 1
		for(i=0; i<ctx->ys; i+=2)
			for(j=0; j<ctx->xs; j+=2)
		{
//			BGBBTJ_JPG_GetComponentPixelS122(ctx, j, i, &y0, &u, &v);
//			y1=BGBBTJ_JPG_GetComponentPixelS1(ctx, 0, j+1, i+0);
//			y2=BGBBTJ_JPG_GetComponentPixelS1(ctx, 0, j+0, i+1);
//			y3=BGBBTJ_JPG_GetComponentPixelS1(ctx, 0, j+1, i+1);

			BGBBTJ_JPG_GetComponentPixelS420(ctx, j, i,
				&y0, &y1, &y2, &y3, &u, &v);

			if(ctx->alphaClr)
			{
				a=255;

				k=y0-ctx->alphaClrY; l=k*k;
				k=y1-ctx->alphaClrY; k=k*k; if(k<l)l=k;
				k=y2-ctx->alphaClrY; k=k*k; if(k<l)l=k;
				k=y3-ctx->alphaClrY; k=k*k; if(k<l)l=k;
				
//				k=y0-ctx->alphaClrY; l=k*k;
				k=u-ctx->alphaClrU; l+=k*k;
				k=v-ctx->alphaClrV; l+=k*k;
				
				a=ctx->alphaClrA;
				a=(l<=(a*a))?0:255;
			}else { a=255; }

			u1=u-128; v1=v-128;
			l0=91881*v1;
			l1=-22554*u1-46802*v1;
			l2=116130*u1;

			y0<<=16; y1<<=16; y2<<=16; y3<<=16;
			r0=y0+l0; g0=y0+l1; b0=y0+l2;
			r1=y1+l0; g1=y1+l1; b1=y1+l2;
			r2=y2+l0; g2=y2+l1; b2=y2+l2;
			r3=y3+l0; g3=y3+l1; b3=y3+l2;

//			r0=65536*y0+l0; g0=65536*y0+l1; b0=65536*y0+l2;
//			r1=65536*y1+l0; g1=65536*y1+l1; b1=65536*y1+l2;
//			r2=65536*y2+l0; g2=65536*y2+l1; b2=65536*y2+l2;
//			r3=65536*y3+l0; g3=65536*y3+l1; b3=65536*y3+l2;

			r0>>=16; g0>>=16; b0>>=16;
			r1>>=16; g1>>=16; b1>>=16;
			r2>>=16; g2>>=16; b2>>=16;
			r3>>=16; g3>>=16; b3>>=16;

#if 0
			if((r0|r1|r2|r3)&(~255))
			{
				r0=(r0<0)?0:((r0>255)?255:r0);
				r1=(r1<0)?0:((r1>255)?255:r1);
				r2=(r2<0)?0:((r2>255)?255:r2);
				r3=(r3<0)?0:((r3>255)?255:r3);
			}

			if((g0|g1|g2|g3)&(~255))
			{
				g0=(g0<0)?0:((g0>255)?255:g0);
				g1=(g1<0)?0:((g1>255)?255:g1);
				g2=(g2<0)?0:((g2>255)?255:g2);
				g3=(g3<0)?0:((g3>255)?255:g3);
			}

			if((b0|b1|b2|b3)&(~255))
			{
				b0=(b0<0)?0:((b0>255)?255:b0);
				b1=(b1<0)?0:((b1>255)?255:b1);
				b2=(b2<0)?0:((b2>255)?255:b2);
				b3=(b3<0)?0:((b3>255)?255:b3);
			}
#endif

#if 1
			if((r0|r1|r2|r3)&(~255))
			{
				if(r0&(~255))r0=(r0<0)?0:((r0>255)?255:r0);
				if(r1&(~255))r1=(r1<0)?0:((r1>255)?255:r1);
				if(r2&(~255))r2=(r2<0)?0:((r2>255)?255:r2);
				if(r3&(~255))r3=(r3<0)?0:((r3>255)?255:r3);
			}

			if((g0|g1|g2|g3)&(~255))
			{
				if(g0&(~255))g0=(g0<0)?0:((g0>255)?255:g0);
				if(g1&(~255))g1=(g1<0)?0:((g1>255)?255:g1);
				if(g2&(~255))g2=(g2<0)?0:((g2>255)?255:g2);
				if(g3&(~255))g3=(g3<0)?0:((g3>255)?255:g3);
			}

			if((b0|b1|b2|b3)&(~255))
			{
				if(b0&(~255))b0=(b0<0)?0:((b0>255)?255:b0);
				if(b1&(~255))b1=(b1<0)?0:((b1>255)?255:b1);
				if(b2&(~255))b2=(b2<0)?0:((b2>255)?255:b2);
				if(b3&(~255))b3=(b3<0)?0:((b3>255)?255:b3);
			}
#endif

#if 0
			k=r0|r1|r2|r3 | g0|g1|g2|g3 | b0|b1|b2|b3;
			if(k&(~255))
			{
				if(r0&(~255))r0=(r0<0)?0:((r0>255)?255:r0);
				if(g0&(~255))g0=(g0<0)?0:((g0>255)?255:g0);
				if(b0&(~255))b0=(b0<0)?0:((b0>255)?255:b0);
				if(r1&(~255))r1=(r1<0)?0:((r1>255)?255:r1);
				if(g1&(~255))g1=(g1<0)?0:((g1>255)?255:g1);
				if(b1&(~255))b1=(b1<0)?0:((b1>255)?255:b1);
				if(r2&(~255))r2=(r2<0)?0:((r2>255)?255:r2);
				if(g2&(~255))g2=(g2<0)?0:((g2>255)?255:g2);
				if(b2&(~255))b2=(b2<0)?0:((b2>255)?255:b2);
				if(r3&(~255))r3=(r3<0)?0:((r3>255)?255:r3);
				if(g3&(~255))g3=(g3<0)?0:((g3>255)?255:g3);
				if(b3&(~255))b3=(b3<0)?0:((b3>255)?255:b3);
			}
#endif

#if 0
			r0=(!(r0&(~255)))?r0:((r0<0)?0:((r0>255)?255:r0));
			g0=(!(g0&(~255)))?g0:((g0<0)?0:((g0>255)?255:g0));
			b0=(!(b0&(~255)))?b0:((b0<0)?0:((b0>255)?255:b0));
			r1=(!(r1&(~255)))?r1:((r1<0)?0:((r1>255)?255:r1));
			g1=(!(g1&(~255)))?g1:((g1<0)?0:((g1>255)?255:g1));
			b1=(!(b1&(~255)))?b1:((b1<0)?0:((b1>255)?255:b1));
			r2=(!(r2&(~255)))?r2:((r2<0)?0:((r2>255)?255:r2));
			g2=(!(g2&(~255)))?g2:((g2<0)?0:((g2>255)?255:g2));
			b2=(!(b2&(~255)))?b2:((b2<0)?0:((b2>255)?255:b2));
			r3=(!(r3&(~255)))?r3:((r3<0)?0:((r3>255)?255:r3));
			g3=(!(g3&(~255)))?g3:((g3<0)?0:((g3>255)?255:g3));
			b3=(!(b3&(~255)))?b3:((b3<0)?0:((b3>255)?255:b3));
#endif

#if 0
			r0=(r0<0)?0:((r0>255)?255:r0);
			g0=(g0<0)?0:((g0>255)?255:g0);
			b0=(b0<0)?0:((b0>255)?255:b0);
			r1=(r1<0)?0:((r1>255)?255:r1);
			g1=(g1<0)?0:((g1>255)?255:g1);
			b1=(b1<0)?0:((b1>255)?255:b1);
			r2=(r2<0)?0:((r2>255)?255:r2);
			g2=(g2<0)?0:((g2>255)?255:g2);
			b2=(b2<0)?0:((b2>255)?255:b2);
			r3=(r3<0)?0:((r3>255)?255:r3);
			g3=(g3<0)?0:((g3>255)?255:g3);
			b3=(b3<0)?0:((b3>255)?255:b3);
#endif

			k=ctx->ys-1-i;
			l0=((k*ctx->xs)+j+0)*4;
//			l1=((k*ctx->xs)+j+1)*4;
			k=ctx->ys-2-i;
			l2=((k*ctx->xs)+j+0)*4;
//			l3=((k*ctx->xs)+j+1)*4;

//			ct=obuf+l0; *ct++=r0; *ct++=g0; *ct++=b0; *ct++=a;
//			ct=obuf+l1; *ct++=r1; *ct++=g1; *ct++=b1; *ct++=a;
//			ct=obuf+l2; *ct++=r2; *ct++=g2; *ct++=b2; *ct++=a;
//			ct=obuf+l3; *ct++=r3; *ct++=g3; *ct++=b3; *ct++=a;

			obuf[l0+0]=r0; obuf[l0+1]=g0; obuf[l0+2]=b0; obuf[l0+3]=a;
			obuf[l0+4]=r1; obuf[l0+5]=g1; obuf[l0+6]=b1; obuf[l0+7]=a;
			obuf[l2+0]=r2; obuf[l2+1]=g2; obuf[l2+2]=b2; obuf[l2+3]=a;
			obuf[l2+4]=r3; obuf[l2+5]=g3; obuf[l2+6]=b3; obuf[l2+7]=a;

//			obuf[l0+0]=r0; obuf[l0+1]=g0; obuf[l0+2]=b0; obuf[l0+3]=a;
//			obuf[l1+0]=r1; obuf[l1+1]=g1; obuf[l1+2]=b1; obuf[l1+3]=a;
//			obuf[l2+0]=r2; obuf[l2+1]=g2; obuf[l2+2]=b2; obuf[l2+3]=a;
//			obuf[l3+0]=r3; obuf[l3+1]=g3; obuf[l3+2]=b3; obuf[l3+3]=a;
		}
#endif

#if 0
		for(i=0; i<ctx->ys; i++)
			for(j=0; j<ctx->xs; j++)
		{
			BGBBTJ_JPG_GetComponentPixelS122(ctx, j, i, &y, &u, &v);

			if(ctx->alphaClr)
			{
				k=y-ctx->alphaClrY; l=k*k;
				k=u-ctx->alphaClrU; l+=k*k;
				k=v-ctx->alphaClrV; l+=k*k;
				
				a=ctx->alphaClrA;
				a=(l<=(a*a))?0:255;
			}else { a=255; }

			u1=u-128; v1=v-128;
			r=65536*y          +91881*v1;
			g=65536*y- 22554*u1-46802*v1;
			b=65536*y+116130*u1;
			r>>=16; g>>=16; b>>=16;

			r=(r<0)?0:((r>255)?255:r);
			g=(g<0)?0:((g>255)?255:g);
			b=(b<0)?0:((b>255)?255:b);

			k=ctx->ys-1-i;
			l=((k*ctx->xs)+j)*4;

			obuf[l+0]=r;
			obuf[l+1]=g;
			obuf[l+2]=b;
			obuf[l+3]=a;
		}
#endif
	return(0);
}

/** Simulate basic decoding, but doesn't actually decode image.
  * Can be used to retrieve resolution or initialize Huffman tables
  * at a lower cost than a full decode. */
BGBBTJ_API int BGBBTJ_JPG_DummyDecodeCtx(BGBBTJ_JPG_Context *ctx,
	byte *buf, int sz, int *xs, int *ys)
{
	byte *obuf, *ct;
	int y, u, v, r, g, b, a, u1, v1;
	int y0, y1, y2, y3;
	int r0, g0, b0, a0;
	int r1, g1, b1, a1;
	int r2, g2, b2, a2;
	int r3, g3, b3, a3;
	int l0, l1, l2, l3;
	int i, j, k, l;

//	if((buf[0]!=0xFF) || (buf[1]!=JPG_SOI))
//		return(-1);

	ctx->jpg_is420=0;
	ctx->jpg_rdct=0;
	ctx->jpg_clrtrans=0;

	ctx->bcst_version=0;
	ctx->bcst_flags=0;
	ctx->bcst_trans=0;
	ctx->bcst_bpp=0;

	for(i=0; i<4; i++)
	{
		ctx->jpg_scbuf[i]=NULL;
		ctx->jpg_sibuf[i]=NULL;
	}

	for(i=0; i<sz; i++)
	{
//		if((buf[i]==0xFF) && (buf[i+1]!=0x00) && (buf[i+1]!=0xFF))
		if(buf[i]==0xFF)
		{
			if(buf[i+1]==0x00)
				{ i++; continue; }
			if(buf[i+1]==0xFF)
				{ i++; continue; }

			j=buf[i+1]-0xC0;
			if(j<0)j=63;

//			printf("@ %d %s(%02X)\n", i,
//				pdjpg_marker[j], buf[i+1]);

			if(buf[i+1]==JPG_DQT)BGBBTJ_JPG_MarkerDQT(ctx, buf+i+2);
			if(buf[i+1]==JPG_DHT)BGBBTJ_JPG_MarkerDHT(ctx, buf+i+2);
			if(buf[i+1]==JPG_SOF0)BGBBTJ_JPG_MarkerSOF0(ctx, buf+i+2);
			if(buf[i+1]==JPG_SOS)
			{
//				j=BGBBTJ_JPG_MarkerSOS(ctx, buf+i+2);
//				if(j>0) { i+=j; }
				continue;
			}

			if((buf[i+1]>=JPG_APP0) && (buf[i+1]<=JPG_APP15))
			{
				if(buf[i+1]==JPG_APP0)BGBBTJ_JPG_MarkerAPP0(ctx, buf+i+2);
				if(buf[i+1]==JPG_APP9)BGBBTJ_JPG_MarkerAPP9(ctx, buf+i+2);
				
				if(buf[i+1]==JPG_APP11)BGBBTJ_JPG_MarkerAPP11(ctx, buf+i+2);
				if(buf[i+1]==JPG_APP12)BGBBTJ_JPG_MarkerAPP12(ctx, buf+i+2);
				if(buf[i+1]==JPG_APP14)BGBBTJ_JPG_MarkerAPP14(ctx, buf+i+2);

				//skip over marker
				j=(buf[i+2]<<8)|buf[i+3];
				i+=j;
				continue;
			}

			if(buf[i+1]==JPG_EOI)break;
		}
	}

	*xs=ctx->xs;
	*ys=ctx->ys;

	return(0);
}

int BGBBTJ_JPG_DecodeColorTransform_YCbCr420_DXT1(
	BGBBTJ_JPG_Context *ctx, byte *obuf, int xs, int ys, int stride)
{
	short *yblk, *ublk, *vblk;
	byte *oblk;
	int xs1, ys1, xs2, ys2, xs3, ys3, xs4, ys4;
	int i, j, k, i1;

	xs1=xs>>1; ys1=ys>>1;
	xs2=xs>>2; ys2=ys>>2;
	xs3=xs>>3; ys3=ys>>3;
	xs4=xs>>4; ys4=ys>>4;

	for(i=0; i<ys4; i++)
		for(j=0; j<xs4; j++)
	{
		k=i*xs4+j;
		ublk=ctx->jpg_sibuf[1]+k*64;
		vblk=ctx->jpg_sibuf[2]+k*64;

#if 1
		i1=ys4-i-1;

		k=((i*2+0)*xs3)+(j*2+0);
		yblk=ctx->jpg_sibuf[0]+k*64;

		k=((i1*4+3)*xs2)+(j*4+0); oblk=obuf+k*stride;
		BGBBTJ_BCn_EncodeBlockDXT1F_YUV420(oblk, yblk+0, ublk+0, vblk+0);
		k=((i1*4+3)*xs2)+(j*4+1); oblk=obuf+k*stride;
		BGBBTJ_BCn_EncodeBlockDXT1F_YUV420(oblk, yblk+4, ublk+2, vblk+2);
		k=((i1*4+2)*xs2)+(j*4+0); oblk=obuf+k*stride;
		BGBBTJ_BCn_EncodeBlockDXT1F_YUV420(oblk, yblk+32, ublk+16, vblk+16);
		k=((i1*4+2)*xs2)+(j*4+1); oblk=obuf+k*stride;
		BGBBTJ_BCn_EncodeBlockDXT1F_YUV420(oblk, yblk+36, ublk+18, vblk+18);

		k=((i*2+0)*xs3)+(j*2+1);
		yblk=ctx->jpg_sibuf[0]+k*64;
	
		k=((i1*4+3)*xs2)+(j*4+2); oblk=obuf+k*stride;
		BGBBTJ_BCn_EncodeBlockDXT1F_YUV420(oblk, yblk+0, ublk+4, vblk+4);
		k=((i1*4+3)*xs2)+(j*4+3); oblk=obuf+k*stride;
		BGBBTJ_BCn_EncodeBlockDXT1F_YUV420(oblk, yblk+4, ublk+6, vblk+6);
		k=((i1*4+2)*xs2)+(j*4+2); oblk=obuf+k*stride;
		BGBBTJ_BCn_EncodeBlockDXT1F_YUV420(oblk, yblk+32, ublk+20, vblk+20);
		k=((i1*4+2)*xs2)+(j*4+3); oblk=obuf+k*stride;
		BGBBTJ_BCn_EncodeBlockDXT1F_YUV420(oblk, yblk+36, ublk+22, vblk+22);

		k=((i*2+1)*xs3)+(j*2+0);
		yblk=ctx->jpg_sibuf[0]+k*64;
	
		k=((i1*4+1)*xs2)+(j*4+0); oblk=obuf+k*stride;
		BGBBTJ_BCn_EncodeBlockDXT1F_YUV420(oblk, yblk+0, ublk+32, vblk+32);
		k=((i1*4+1)*xs2)+(j*4+1); oblk=obuf+k*stride;
		BGBBTJ_BCn_EncodeBlockDXT1F_YUV420(oblk, yblk+4, ublk+34, vblk+34);
		k=((i1*4+0)*xs2)+(j*4+0); oblk=obuf+k*stride;
		BGBBTJ_BCn_EncodeBlockDXT1F_YUV420(oblk, yblk+32, ublk+48, vblk+48);
		k=((i1*4+0)*xs2)+(j*4+1); oblk=obuf+k*stride;
		BGBBTJ_BCn_EncodeBlockDXT1F_YUV420(oblk, yblk+36, ublk+50, vblk+50);

		k=((i*2+1)*xs3)+(j*2+1);
		yblk=ctx->jpg_sibuf[0]+k*64;
	
		k=((i1*4+1)*xs2)+(j*4+2); oblk=obuf+k*stride;
		BGBBTJ_BCn_EncodeBlockDXT1F_YUV420(oblk, yblk+0, ublk+36, vblk+36);
		k=((i1*4+1)*xs2)+(j*4+3); oblk=obuf+k*stride;
		BGBBTJ_BCn_EncodeBlockDXT1F_YUV420(oblk, yblk+4, ublk+38, vblk+38);
		k=((i1*4+0)*xs2)+(j*4+2); oblk=obuf+k*stride;
		BGBBTJ_BCn_EncodeBlockDXT1F_YUV420(oblk, yblk+32, ublk+52, vblk+52);
		k=((i1*4+0)*xs2)+(j*4+3); oblk=obuf+k*stride;
		BGBBTJ_BCn_EncodeBlockDXT1F_YUV420(oblk, yblk+36, ublk+54, vblk+54);
#endif

#if 0
		k=((i*2+0)*xs3)+(j*2+0);
		yblk=ctx->jpg_sibuf[0]+k*64;

		k=((i*4+0)*xs2)+(j*4+0); oblk=obuf+k*stride;
		BGBBTJ_BCn_EncodeBlockDXT1F_YUV420(oblk, yblk+0, ublk+0, vblk+0);
		k=((i*4+0)*xs2)+(j*4+1); oblk=obuf+k*stride;
		BGBBTJ_BCn_EncodeBlockDXT1F_YUV420(oblk, yblk+4, ublk+2, vblk+2);
		k=((i*4+1)*xs2)+(j*4+0); oblk=obuf+k*stride;
		BGBBTJ_BCn_EncodeBlockDXT1F_YUV420(oblk, yblk+32, ublk+16, vblk+16);
		k=((i*4+1)*xs2)+(j*4+1); oblk=obuf+k*stride;
		BGBBTJ_BCn_EncodeBlockDXT1F_YUV420(oblk, yblk+36, ublk+18, vblk+18);

		k=((i*2+0)*xs3)+(j*2+1);
		yblk=ctx->jpg_sibuf[0]+k*64;
	
		k=((i*4+0)*xs2)+(j*4+2); oblk=obuf+k*stride;
		BGBBTJ_BCn_EncodeBlockDXT1F_YUV420(oblk, yblk+0, ublk+4, vblk+4);
		k=((i*4+0)*xs2)+(j*4+3); oblk=obuf+k*stride;
		BGBBTJ_BCn_EncodeBlockDXT1F_YUV420(oblk, yblk+4, ublk+6, vblk+6);
		k=((i*4+1)*xs2)+(j*4+2); oblk=obuf+k*stride;
		BGBBTJ_BCn_EncodeBlockDXT1F_YUV420(oblk, yblk+32, ublk+20, vblk+20);
		k=((i*4+1)*xs2)+(j*4+3); oblk=obuf+k*stride;
		BGBBTJ_BCn_EncodeBlockDXT1F_YUV420(oblk, yblk+36, ublk+22, vblk+22);

		k=((i*2+1)*xs3)+(j*2+0);
		yblk=ctx->jpg_sibuf[0]+k*64;
	
		k=((i*4+2)*xs2)+(j*4+0); oblk=obuf+k*stride;
		BGBBTJ_BCn_EncodeBlockDXT1F_YUV420(oblk, yblk+0, ublk+32, vblk+32);
		k=((i*4+2)*xs2)+(j*4+1); oblk=obuf+k*stride;
		BGBBTJ_BCn_EncodeBlockDXT1F_YUV420(oblk, yblk+4, ublk+34, vblk+34);
		k=((i*4+3)*xs2)+(j*4+0); oblk=obuf+k*stride;
		BGBBTJ_BCn_EncodeBlockDXT1F_YUV420(oblk, yblk+32, ublk+48, vblk+48);
		k=((i*4+3)*xs2)+(j*4+1); oblk=obuf+k*stride;
		BGBBTJ_BCn_EncodeBlockDXT1F_YUV420(oblk, yblk+36, ublk+50, vblk+50);

		k=((i*2+1)*xs3)+(j*2+1);
		yblk=ctx->jpg_sibuf[0]+k*64;
	
		k=((i*4+2)*xs2)+(j*4+2); oblk=obuf+k*stride;
		BGBBTJ_BCn_EncodeBlockDXT1F_YUV420(oblk, yblk+0, ublk+36, vblk+36);
		k=((i*4+2)*xs2)+(j*4+3); oblk=obuf+k*stride;
		BGBBTJ_BCn_EncodeBlockDXT1F_YUV420(oblk, yblk+4, ublk+38, vblk+38);
		k=((i*4+3)*xs2)+(j*4+2); oblk=obuf+k*stride;
		BGBBTJ_BCn_EncodeBlockDXT1F_YUV420(oblk, yblk+32, ublk+52, vblk+52);
		k=((i*4+3)*xs2)+(j*4+3); oblk=obuf+k*stride;
		BGBBTJ_BCn_EncodeBlockDXT1F_YUV420(oblk, yblk+36, ublk+54, vblk+54);
#endif

	}
	return(0);
}

int BGBBTJ_JPG_DecodeColorTransform_YCbCr420_DXT1A(
	BGBBTJ_JPG_Context *ctx, byte *obuf, int xs, int ys, int stride)
{
	short *yblk, *ublk, *vblk;
	byte *oblk, *ablk;
	int xs1, ys1, xs2, ys2, xs3, ys3, xs4, ys4;
	int i, j, k, i1, a;

	xs1=xs>>1; ys1=ys>>1;
	xs2=xs>>2; ys2=ys>>2;
	xs3=xs>>3; ys3=ys>>3;
	xs4=xs>>4; ys4=ys>>4;

	for(i=0; i<ys4; i++)
		for(j=0; j<xs4; j++)
	{
		k=i*xs4+j;
		ublk=ctx->jpg_sibuf[1]+k*64;
		vblk=ctx->jpg_sibuf[2]+k*64;

		i1=ys4-i-1;

		k=((i*2+0)*xs3)+(j*2+0);
		yblk=ctx->jpg_sibuf[0]+k*64;
		ablk=ctx->badb+k*8;

		a=(	(ablk[0]&0xF0)<<8)|((ablk[1]&0xF0)<<4)|
			(ablk[2]&0xF0)|((ablk[3]&0xF0)>>8);
		k=((i1*4+3)*xs2)+(j*4+0); oblk=obuf+k*stride;
		BGBBTJ_BCn_EncodeBlockDXT1A_YUV420(
			oblk, yblk+0, ublk+0, vblk+0, a);

		a=(	(ablk[0]&0x0F)<<12)|((ablk[1]&0x0F)<<8)|
			((ablk[2]&0x0F)<<4)|(ablk[3]&0x0F);
		k=((i1*4+3)*xs2)+(j*4+1); oblk=obuf+k*stride;
		BGBBTJ_BCn_EncodeBlockDXT1A_YUV420(
			oblk, yblk+4, ublk+2, vblk+2, a);

		a=(	(ablk[4]&0xF0)<<8)|((ablk[5]&0xF0)<<4)|
			(ablk[6]&0xF0)|((ablk[7]&0xF0)>>8);
		k=((i1*4+2)*xs2)+(j*4+0); oblk=obuf+k*stride;
		BGBBTJ_BCn_EncodeBlockDXT1A_YUV420(
			oblk, yblk+32, ublk+16, vblk+16, a);

		a=(	(ablk[4]&0x0F)<<12)|((ablk[5]&0x0F)<<8)|
			((ablk[6]&0x0F)<<4)|(ablk[7]&0x0F);
		k=((i1*4+2)*xs2)+(j*4+1); oblk=obuf+k*stride;
		BGBBTJ_BCn_EncodeBlockDXT1A_YUV420(
			oblk, yblk+36, ublk+18, vblk+18, a);


		k=((i*2+0)*xs3)+(j*2+1);
		yblk=ctx->jpg_sibuf[0]+k*64;
		ablk=ctx->badb+k*8;
	
		a=(	(ablk[0]&0xF0)<<8)|((ablk[1]&0xF0)<<4)|
			(ablk[2]&0xF0)|((ablk[3]&0xF0)>>8);
		k=((i1*4+3)*xs2)+(j*4+2); oblk=obuf+k*stride;
		BGBBTJ_BCn_EncodeBlockDXT1A_YUV420(
			oblk, yblk+0, ublk+4, vblk+4, a);

		a=(	(ablk[0]&0x0F)<<12)|((ablk[1]&0x0F)<<8)|
			((ablk[2]&0x0F)<<4)|(ablk[3]&0x0F);
		k=((i1*4+3)*xs2)+(j*4+3); oblk=obuf+k*stride;
		BGBBTJ_BCn_EncodeBlockDXT1A_YUV420(
			oblk, yblk+4, ublk+6, vblk+6, a);

		a=(	(ablk[4]&0xF0)<<8)|((ablk[5]&0xF0)<<4)|
			(ablk[6]&0xF0)|((ablk[7]&0xF0)>>8);
		k=((i1*4+2)*xs2)+(j*4+2); oblk=obuf+k*stride;
		BGBBTJ_BCn_EncodeBlockDXT1A_YUV420(
			oblk, yblk+32, ublk+20, vblk+20, a);

		a=(	(ablk[4]&0x0F)<<12)|((ablk[5]&0x0F)<<8)|
			((ablk[6]&0x0F)<<4)|(ablk[7]&0x0F);
		k=((i1*4+2)*xs2)+(j*4+3); oblk=obuf+k*stride;
		BGBBTJ_BCn_EncodeBlockDXT1A_YUV420(
			oblk, yblk+36, ublk+22, vblk+22, a);


		k=((i*2+1)*xs3)+(j*2+0);
		yblk=ctx->jpg_sibuf[0]+k*64;
		ablk=ctx->badb+k*8;
	
		a=(	(ablk[0]&0xF0)<<8)|((ablk[1]&0xF0)<<4)|
			(ablk[2]&0xF0)|((ablk[3]&0xF0)>>8);
		k=((i1*4+1)*xs2)+(j*4+0); oblk=obuf+k*stride;
		BGBBTJ_BCn_EncodeBlockDXT1A_YUV420(
			oblk, yblk+0, ublk+32, vblk+32, a);

		a=(	(ablk[0]&0x0F)<<12)|((ablk[1]&0x0F)<<8)|
			((ablk[2]&0x0F)<<4)|(ablk[3]&0x0F);
		k=((i1*4+1)*xs2)+(j*4+1); oblk=obuf+k*stride;
		BGBBTJ_BCn_EncodeBlockDXT1A_YUV420(
			oblk, yblk+4, ublk+34, vblk+34, a);

		a=(	(ablk[4]&0xF0)<<8)|((ablk[5]&0xF0)<<4)|
			(ablk[6]&0xF0)|((ablk[7]&0xF0)>>8);
		k=((i1*4+0)*xs2)+(j*4+0); oblk=obuf+k*stride;
		BGBBTJ_BCn_EncodeBlockDXT1A_YUV420(
			oblk, yblk+32, ublk+48, vblk+48, a);

		a=(	(ablk[4]&0x0F)<<12)|((ablk[5]&0x0F)<<8)|
			((ablk[6]&0x0F)<<4)|(ablk[7]&0x0F);
		k=((i1*4+0)*xs2)+(j*4+1); oblk=obuf+k*stride;
		BGBBTJ_BCn_EncodeBlockDXT1A_YUV420(
			oblk, yblk+36, ublk+50, vblk+50, a);


		k=((i*2+1)*xs3)+(j*2+1);
		yblk=ctx->jpg_sibuf[0]+k*64;
		ablk=ctx->badb+k*8;
	
		a=(	(ablk[0]&0xF0)<<8)|((ablk[1]&0xF0)<<4)|
			(ablk[2]&0xF0)|((ablk[3]&0xF0)>>8);
		k=((i1*4+1)*xs2)+(j*4+2); oblk=obuf+k*stride;
		BGBBTJ_BCn_EncodeBlockDXT1A_YUV420(
			oblk, yblk+0, ublk+36, vblk+36, a);

		a=(	(ablk[0]&0x0F)<<12)|((ablk[1]&0x0F)<<8)|
			((ablk[2]&0x0F)<<4)|(ablk[3]&0x0F);
		k=((i1*4+1)*xs2)+(j*4+3); oblk=obuf+k*stride;
		BGBBTJ_BCn_EncodeBlockDXT1A_YUV420(
			oblk, yblk+4, ublk+38, vblk+38, a);

		a=(	(ablk[4]&0xF0)<<8)|((ablk[5]&0xF0)<<4)|
			(ablk[6]&0xF0)|((ablk[7]&0xF0)>>8);
		k=((i1*4+0)*xs2)+(j*4+2); oblk=obuf+k*stride;
		BGBBTJ_BCn_EncodeBlockDXT1A_YUV420(
			oblk, yblk+32, ublk+52, vblk+52, a);

		a=(	(ablk[4]&0x0F)<<12)|((ablk[5]&0x0F)<<8)|
			((ablk[6]&0x0F)<<4)|(ablk[7]&0x0F);
		k=((i1*4+0)*xs2)+(j*4+3); oblk=obuf+k*stride;
		BGBBTJ_BCn_EncodeBlockDXT1A_YUV420(
			oblk, yblk+36, ublk+54, vblk+54, a);
	}
	return(0);
}

#if 1
int BGBBTJ_JPG_DecodeColorTransform_YBC4(
	BGBBTJ_JPG_Context *ctx, byte *obuf, int xs, int ys, int stride)
{
	short *yblk;
	byte *oblk, *ablk;
	int xs1, ys1, xs2, ys2, xs3, ys3, xs4, ys4;
	int i, j, k, i1, a;

	xs1=xs>>1; ys1=ys>>1;
	xs2=xs>>2; ys2=ys>>2;
	xs3=xs>>3; ys3=ys>>3;
	xs4=xs>>4; ys4=ys>>4;

	for(i=0; i<ys3; i++)
		for(j=0; j<xs3; j++)
	{
		k=i*xs3+j;
		i1=ys3-i-1;

		yblk=ctx->jpg_sibuf[0]+k*64;
		k=((i1*2+1)*xs2)+(j*2+0); oblk=obuf+k*stride;
		BGBBTJ_BCn_EncodeBlockBC4Y8x8(oblk, yblk+0);
		k=((i1*2+1)*xs2)+(j*2+1); oblk=obuf+k*stride;
		BGBBTJ_BCn_EncodeBlockBC4Y8x8(oblk, yblk+4);
		k=((i1*2+0)*xs2)+(j*2+0); oblk=obuf+k*stride;
		BGBBTJ_BCn_EncodeBlockBC4Y8x8(oblk, yblk+32);
		k=((i1*2+0)*xs2)+(j*2+1); oblk=obuf+k*stride;
		BGBBTJ_BCn_EncodeBlockBC4Y8x8(oblk, yblk+36);
	}
	return(0);
}
#endif

#if 1
int BGBBTJ_JPG_DecodeColorTransform_YCbCr420_BC7(
	BGBBTJ_JPG_Context *ctx, byte *obuf, int xs, int ys,
	int stride, int vfl)
{
	short *yblk, *ublk, *vblk;
	byte *oblk;
	int xs1, ys1, xs2, ys2, xs3, ys3, xs4, ys4;
	int i, j, k, i1;

	xs1=xs>>1; ys1=ys>>1;
	xs2=xs>>2; ys2=ys>>2;
	xs3=xs>>3; ys3=ys>>3;
	xs4=xs>>4; ys4=ys>>4;

	for(i=0; i<ys4; i++)
		for(j=0; j<xs4; j++)
	{
		k=i*xs4+j;
		ublk=ctx->jpg_sibuf[1]+k*64;
		vblk=ctx->jpg_sibuf[2]+k*64;

		i1=ys4-i-1;

		k=((i*2+0)*xs3)+(j*2+0);
		yblk=ctx->jpg_sibuf[0]+k*64;

		k=((i1*4+3)*xs2)+(j*4+0); oblk=obuf+k*stride;
		BGBBTJ_BC7_EncodeBlock_YUV420(oblk, yblk+0, ublk+0, vblk+0, vfl);
		k=((i1*4+3)*xs2)+(j*4+1); oblk=obuf+k*stride;
		BGBBTJ_BC7_EncodeBlock_YUV420(oblk, yblk+4, ublk+2, vblk+2, vfl);
		k=((i1*4+2)*xs2)+(j*4+0); oblk=obuf+k*stride;
		BGBBTJ_BC7_EncodeBlock_YUV420(oblk, yblk+32, ublk+16, vblk+16, vfl);
		k=((i1*4+2)*xs2)+(j*4+1); oblk=obuf+k*stride;
		BGBBTJ_BC7_EncodeBlock_YUV420(oblk, yblk+36, ublk+18, vblk+18, vfl);

		k=((i*2+0)*xs3)+(j*2+1);
		yblk=ctx->jpg_sibuf[0]+k*64;
	
		k=((i1*4+3)*xs2)+(j*4+2); oblk=obuf+k*stride;
		BGBBTJ_BC7_EncodeBlock_YUV420(oblk, yblk+0, ublk+4, vblk+4, vfl);
		k=((i1*4+3)*xs2)+(j*4+3); oblk=obuf+k*stride;
		BGBBTJ_BC7_EncodeBlock_YUV420(oblk, yblk+4, ublk+6, vblk+6, vfl);
		k=((i1*4+2)*xs2)+(j*4+2); oblk=obuf+k*stride;
		BGBBTJ_BC7_EncodeBlock_YUV420(oblk, yblk+32, ublk+20, vblk+20, vfl);
		k=((i1*4+2)*xs2)+(j*4+3); oblk=obuf+k*stride;
		BGBBTJ_BC7_EncodeBlock_YUV420(oblk, yblk+36, ublk+22, vblk+22, vfl);

		k=((i*2+1)*xs3)+(j*2+0);
		yblk=ctx->jpg_sibuf[0]+k*64;
	
		k=((i1*4+1)*xs2)+(j*4+0); oblk=obuf+k*stride;
		BGBBTJ_BC7_EncodeBlock_YUV420(oblk, yblk+0, ublk+32, vblk+32, vfl);
		k=((i1*4+1)*xs2)+(j*4+1); oblk=obuf+k*stride;
		BGBBTJ_BC7_EncodeBlock_YUV420(oblk, yblk+4, ublk+34, vblk+34, vfl);
		k=((i1*4+0)*xs2)+(j*4+0); oblk=obuf+k*stride;
		BGBBTJ_BC7_EncodeBlock_YUV420(oblk, yblk+32, ublk+48, vblk+48, vfl);
		k=((i1*4+0)*xs2)+(j*4+1); oblk=obuf+k*stride;
		BGBBTJ_BC7_EncodeBlock_YUV420(oblk, yblk+36, ublk+50, vblk+50, vfl);

		k=((i*2+1)*xs3)+(j*2+1);
		yblk=ctx->jpg_sibuf[0]+k*64;
	
		k=((i1*4+1)*xs2)+(j*4+2); oblk=obuf+k*stride;
		BGBBTJ_BC7_EncodeBlock_YUV420(oblk, yblk+0, ublk+36, vblk+36, vfl);
		k=((i1*4+1)*xs2)+(j*4+3); oblk=obuf+k*stride;
		BGBBTJ_BC7_EncodeBlock_YUV420(oblk, yblk+4, ublk+38, vblk+38, vfl);
		k=((i1*4+0)*xs2)+(j*4+2); oblk=obuf+k*stride;
		BGBBTJ_BC7_EncodeBlock_YUV420(oblk, yblk+32, ublk+52, vblk+52, vfl);
		k=((i1*4+0)*xs2)+(j*4+3); oblk=obuf+k*stride;
		BGBBTJ_BC7_EncodeBlock_YUV420(oblk, yblk+36, ublk+54, vblk+54, vfl);
	}
	return(0);
}
#endif

void BGBBTJ_JPG_DecodeColorTransform_YFast(
	BGBBTJ_JPG_Context *ctx, byte *obuf, int xs, int ys)
{
	short *yblk;
	byte *oblk;
	int xs1, ys1, xs2, ys2, xs3, ys3, xs4, ys4;
	int y, l0, l1;
	int i, j, k;

	xs3=xs>>3; ys3=ys>>3;
	for(i=0; i<ys3; i++)
		for(j=0; j<xs3; j++)
	{
		k=(i*xs3)+j;
		yblk=ctx->jpg_sibuf[0]+k*64;
		
		oblk=obuf+((((ys3-i-1)*8)*xs)+(j*8))*4;
		for(k=0; k<8; k++)
		{
			l0=(7-k)*8; l1=k*xs*4;
			y=yblk[l0+0]; y=(y<0)?0:((y>255)?255:y);
			oblk[l1+ 0]=y; oblk[l1+ 1]=y; oblk[l1+ 2]=y;
			y=yblk[l0+1]; y=(y<0)?0:((y>255)?255:y);
			oblk[l1+ 4]=y; oblk[l1+ 5]=y; oblk[l1+ 6]=y;
			y=yblk[l0+2]; y=(y<0)?0:((y>255)?255:y);
			oblk[l1+ 8]=y; oblk[l1+ 9]=y; oblk[l1+10]=y;
			y=yblk[l0+3]; y=(y<0)?0:((y>255)?255:y);
			oblk[l1+12]=y; oblk[l1+13]=y; oblk[l1+14]=y;
			y=yblk[l0+4]; y=(y<0)?0:((y>255)?255:y);
			oblk[l1+16]=y; oblk[l1+17]=y; oblk[l1+18]=y;
			y=yblk[l0+5]; y=(y<0)?0:((y>255)?255:y);
			oblk[l1+20]=y; oblk[l1+21]=y; oblk[l1+22]=y;
			y=yblk[l0+6]; y=(y<0)?0:((y>255)?255:y);
			oblk[l1+24]=y; oblk[l1+25]=y; oblk[l1+26]=y;
			y=yblk[l0+7]; y=(y<0)?0:((y>255)?255:y);
			oblk[l1+28]=y; oblk[l1+29]=y; oblk[l1+30]=y;
		}
	}
}

BGBBTJ_API void BGBBTJ_JPG_DecodeCtxI_ColorTransform(
	BGBBTJ_JPG_Context *ctx, byte *obuf, int xs, int ys, int clrs)
{
	int y, u, v, r, g, b, a, u1, v1;
//	int y0, y1, y2, y3;
//	int r0, g0, b0, a0;
//	int r1, g1, b1, a1;
//	int r2, g2, b2, a2;
//	int r3, g3, b3, a3;
//	int l0, l1, l2, l3;
	int i, j, k, l;

	if((clrs==BGBBTJ_JPG_BC1) || (clrs==BGBBTJ_JPG_BC1F))
	{
		if(ctx->jpg_is420 && !((ctx->xs|ctx->ys)&15) &&
			(ctx->jpg_clrtrans==BGBBTJ_JPG_CLRS_YCBCR))
		{
			BGBBTJ_JPG_DecodeColorTransform_YCbCr420_DXT1(
				ctx, obuf, xs, ys, 8);
			return;
		}
		return;
	}

	if(clrs==BGBBTJ_JPG_BC1A)
	{
		if(ctx->jpg_is420 && !((ctx->xs|ctx->ys)&15) &&
			(ctx->jpg_clrtrans==BGBBTJ_JPG_CLRS_YCBCR))
		{
			if(ctx->badb)
			{
				BGBBTJ_JPG_DecodeColorTransform_YCbCr420_DXT1A(
					ctx, obuf, xs, ys, 8);
			}else
			{
				BGBBTJ_JPG_DecodeColorTransform_YCbCr420_DXT1(
					ctx, obuf, xs, ys, 8);
			}
			return;
		}
		return;
	}

	if((clrs==BGBBTJ_JPG_BC3) || (clrs==BGBBTJ_JPG_BC3F) ||
		(clrs==BGBBTJ_JPG_BC3_VF))
	{
		if(ctx->jpg_is420 && !((ctx->xs|ctx->ys)&15) &&
			(ctx->jpg_clrtrans==BGBBTJ_JPG_CLRS_YCBCR))
		{
			BGBBTJ_JPG_DecodeColorTransform_YCbCr420_DXT1(
				ctx, obuf+8, xs, ys, 16);
			return;
		}
		return;
	}

	if((clrs==BGBBTJ_JPG_MB5C) ||
		(clrs==BGBBTJ_JPG_MB5C_VF))
	{
		if(ctx->jpg_is420 && !((ctx->xs|ctx->ys)&15) &&
			(ctx->jpg_clrtrans==BGBBTJ_JPG_CLRS_YCBCR))
		{
			BGBBTJ_JPG_DecodeColorTransform_YCbCr420_DXT1(
				ctx, obuf+8, xs, ys, 32);
			return;
		}
		return;
	}

	if((clrs==BGBBTJ_JPG_BC7) ||
		(clrs==BGBBTJ_JPG_BC7_VF2))
	{
		if(ctx->jpg_is420 && !((ctx->xs|ctx->ys)&15) &&
			(ctx->jpg_clrtrans==BGBBTJ_JPG_CLRS_YCBCR))
		{
			BGBBTJ_JPG_DecodeColorTransform_YCbCr420_BC7(
				ctx, obuf, xs, ys, 16, 1);
			return;
		}
		return;
	}

	if(clrs==BGBBTJ_JPG_BC7_VF)
	{
		if(ctx->jpg_is420 && !((ctx->xs|ctx->ys)&15) &&
			(ctx->jpg_clrtrans==BGBBTJ_JPG_CLRS_YCBCR))
		{
			BGBBTJ_JPG_DecodeColorTransform_YCbCr420_BC7(
				ctx, obuf, xs, ys, 16, 0);
			return;
		}
		return;
	}

	if(clrs==BGBBTJ_JPG_BC4)
	{
		if(ctx->jpg_nc==1)
		{
			BGBBTJ_JPG_DecodeColorTransform_YBC4(ctx, obuf, xs, ys, 8);
			return;
		}
		return;
	}

	if(clrs==BGBBTJ_JPG_BC4A)
	{
		if(ctx->jpg_nc==1)
		{
			BGBBTJ_JPG_DecodeColorTransform_YBC4(ctx, obuf, xs, ys, 16);
			return;
		}
		return;
	}

	if(clrs==BGBBTJ_JPG_YYYA)
	{
		for(i=0; i<ctx->ys; i++)
			for(j=0; j<ctx->xs; j++)
		{
			y=BGBBTJ_JPG_GetComponentPixel(ctx, 0, j, i);
//			if(y<0)y=0;
//			if(y>255)y=255;
			y=(y<0)?0:((y>255)?255:y);
			r=y; g=y; b=y; a=255;

			k=ctx->ys-1-i;
			l=((k*ctx->xs)+j)*4;

			obuf[l+0]=r;
			obuf[l+1]=g;
			obuf[l+2]=b;
			obuf[l+3]=a;
		}
		return;
	}

	if(ctx->jpg_is420 && !((ctx->xs|ctx->ys)&1) &&
		(ctx->jpg_clrtrans==BGBBTJ_JPG_CLRS_YCBCR))
	{
		BGBBTJ_JPG_DecodeColorTransformYCbCr420(ctx, obuf);
		return;
	}
	
	if(ctx->jpg_nc==1)
	{
#if 0
		if(!((ctx->xs|ctx->ys)&7))
		{
			BGBBTJ_JPG_DecodeColorTransform_YFast(
				ctx, obuf, ctx->xs, ctx->ys);
			return;
		}
#endif

		for(i=0; i<ctx->ys; i++)
			for(j=0; j<ctx->xs; j++)
		{
			y=BGBBTJ_JPG_GetComponentPixel(ctx, 0, j, i);
			y=(y<0)?0:((y>255)?255:y);
			r=y; g=y; b=y; a=255;

			k=ctx->ys-1-i;
			l=((k*ctx->xs)+j)*4;

			obuf[l+0]=r;
			obuf[l+1]=g;
			obuf[l+2]=b;
			obuf[l+3]=a;
		}
		return;
	}
	
	if(1)
	{
		for(i=0; i<ctx->ys; i++)
			for(j=0; j<ctx->xs; j++)
		{
			if(ctx->jpg_nc==3)
			{
				if(ctx->jpg_is420)
				{
//					y=BGBBTJ_JPG_GetComponentPixelS1(ctx, 0, j, i);
//					u=BGBBTJ_JPG_GetComponentPixelS2(ctx, 1, j, i);
//					v=BGBBTJ_JPG_GetComponentPixelS2(ctx, 2, j, i);
					BGBBTJ_JPG_GetComponentPixelS122(ctx, j, i, &y, &u, &v);
//					a=255;
					a=BGBBTJ_JPG_GetComponentPixelAlpha(ctx, 0, j, i);
				}else if(ctx->jpg_is444)
				{
					BGBBTJ_JPG_GetComponentPixelS111(ctx, j, i, &y, &u, &v);
//					a=255;
					a=BGBBTJ_JPG_GetComponentPixelAlpha(ctx, 0, j, i);
				}else
				{
					y=BGBBTJ_JPG_GetComponentPixel(ctx, 0, j, i);
					u=BGBBTJ_JPG_GetComponentPixel(ctx, 1, j, i);
					v=BGBBTJ_JPG_GetComponentPixel(ctx, 2, j, i);
//					a=255;
					a=BGBBTJ_JPG_GetComponentPixelAlpha(ctx, 0, j, i);
				}

				if(ctx->alphaClr)
				{
					k=y-ctx->alphaClrY; l=k*k;
					k=u-ctx->alphaClrU; l+=k*k;
					k=v-ctx->alphaClrV; l+=k*k;
				
					a=ctx->alphaClrA;
					a=(l<=(a*a))?0:255;
				
//					if(l<(16*16))
//					{
//						a=255*((16*sqrt(l))/ctx->alphaClrA);
//						a=(a<0)?0:(a>255)?255:a;
//					}
				}

//				r=y+1.40200*(v-128);
//				g=y-0.34414*(u-128)-0.71414*(v-128);
//				b=y+1.77200*(u-128);

				if(ctx->jpg_clrtrans==BGBBTJ_JPG_CLRS_YCBCR)
				{
					u1=u-128; v1=v-128;
					r=65536*y          +91881*v1;
					g=65536*y- 22554*u1-46802*v1;
					b=65536*y+116130*u1;
					r>>=16; g>>=16; b>>=16;
				}else if(ctx->jpg_clrtrans==BGBBTJ_JPG_CLRS_ORCT)
				{
					u1=u-128; v1=v-128;
					g=y-(u1+v1)/4;
					r=v1+g;
					b=u1+g;
				}else if(ctx->jpg_clrtrans==BGBBTJ_JPG_CLRS_RGB)
				{
					r=y;
					g=u;
					b=v;
				}

				r=(r<0)?0:((r>255)?255:r);
				g=(g<0)?0:((g>255)?255:g);
				b=(b<0)?0:((b>255)?255:b);

//				r=(r<0)?0:((r>16777215)?255:(r>>16));
//				g=(g<0)?0:((g>16777215)?255:(g>>16));
//				b=(b<0)?0:((b>16777215)?255:(b>>16));
			}else if(ctx->jpg_nc==1)
			{
				y=BGBBTJ_JPG_GetComponentPixel(ctx, 0, j, i);
				y=(y<0)?0:((y>255)?255:y);
				r=y; g=y; b=y; a=255;
			}

			k=ctx->ys-1-i;
			l=((k*ctx->xs)+j)*4;

			obuf[l+0]=r;
			obuf[l+1]=g;
			obuf[l+2]=b;
			obuf[l+3]=a;
		}
	}
}

BGBBTJ_API byte *BGBBTJ_JPG_DecodeCtx(BGBBTJ_JPG_Context *ctx,
	byte *buf, int sz, int *xs, int *ys)
{
	return(BGBBTJ_JPG_DecodeCtxI(ctx, buf, sz, BGBBTJ_JPG_RGBA, xs, ys));
}

BGBBTJ_API byte *BGBBTJ_JPG_DecodeCtxClrs(BGBBTJ_JPG_Context *ctx,
	byte *buf, int sz, int clrs, int *xs, int *ys)
{
	return(BGBBTJ_JPG_DecodeCtxI(ctx, buf, sz, clrs, xs, ys));
}

byte *BGBBTJ_JPG_DecodeCtxI(BGBBTJ_JPG_Context *ctx,
	byte *buf, int sz, int clrs, int *xs, int *ys)
{
	byte *obuf, *ct;
	int y, u, v, r, g, b, a, u1, v1;
	int y0, y1, y2, y3;
	int r0, g0, b0, a0;
	int r1, g1, b1, a1;
	int r2, g2, b2, a2;
	int r3, g3, b3, a3;
	int l0, l1, l2, l3;
	int i, j, k, l, lvm, lvo;

//	if((buf[0]!=0xFF) || (buf[1]!=JPG_SOI))
//		return(NULL);

	ctx->jpg_is420=0;
	ctx->jpg_rdct=0;
	ctx->jpg_clrtrans=0;

	ctx->bcst_version=0;
	ctx->bcst_flags=0;
	ctx->bcst_trans=0;
	ctx->bcst_bpp=0;

#if 0
	for(i=0; i<4; i++)
	{
		ctx->jpg_scbuf[i]=NULL;
		ctx->jpg_sibuf[i]=NULL;
	}
#endif

	lvm=0;
	for(i=0; i<sz; i++)
	{
//		if((buf[i]==0xFF) && (buf[i+1]!=0x00) && (buf[i+1]!=0xFF))
		if(buf[i]==0xFF)
		{
			if(buf[i+1]==0x00)
				{ i++; continue; }
			if(buf[i+1]==0xFF)
				{ i++; continue; }

			j=buf[i+1]-0xC0;
			if(j<0)j=63;

			lvm=buf[i+1]; lvo=i;

//			printf("@ %d %s(%02X)\n", i,
//				pdjpg_marker[j], buf[i+1]);

			if(buf[i+1]==JPG_DQT)
			{
				BGBBTJ_JPG_MarkerDQT(ctx, buf+i+2);
				//skip over marker
				j=(buf[i+2]<<8)|buf[i+3];
				i+=j+1;
				continue;
			}

			if(buf[i+1]==JPG_DHT)
			{
				BGBBTJ_JPG_MarkerDHT(ctx, buf+i+2);
				//skip over marker
				j=(buf[i+2]<<8)|buf[i+3];
				i+=j+1;
				continue;
			}

			if(buf[i+1]==JPG_SOF0)
			{
				BGBBTJ_JPG_MarkerSOF0(ctx, buf+i+2);
				//skip over marker
				j=(buf[i+2]<<8)|buf[i+3];
				i+=j+1;
				continue;
			}
			if(buf[i+1]==JPG_SOS)
			{
				j=BGBBTJ_JPG_MarkerSOS(ctx, buf+i+2);
				if(j>0) { i+=j; }
				continue;

				//skip over marker
//				j=(buf[i+2]<<8)|buf[i+3];
//				i+=j+1;
//				continue;
			}

			if((buf[i+1]>=JPG_APP0) && (buf[i+1]<=JPG_APP15))
			{
				if(buf[i+1]==JPG_APP0)BGBBTJ_JPG_MarkerAPP0(ctx, buf+i+2);
				if(buf[i+1]==JPG_APP11)BGBBTJ_JPG_MarkerAPP11(ctx, buf+i+2);
				if(buf[i+1]==JPG_APP12)BGBBTJ_JPG_MarkerAPP12(ctx, buf+i+2);
				if(buf[i+1]==JPG_APP14)BGBBTJ_JPG_MarkerAPP14(ctx, buf+i+2);

				//skip over marker
				j=(buf[i+2]<<8)|buf[i+3];
				i+=j+1;
				continue;
			}

			if(buf[i+1]==JPG_SOI)
			{
				i++;
				continue;
			}

			if(buf[i+1]==JPG_EOI)
			{
				i++;
				break;
			}
		}
		
//		printf("BGBBTJ_JPG_DecodeCtxI: Misalign After %02X @%06X\n",
//			lvm, lvo);
	}

	obuf=ctx->jpg_imgbuf;
	if(!obuf || (ctx->xs!=ctx->lxs) || (ctx->ys!=ctx->lys))
	{
		if(ctx->jpg_imgbuf)
			free(ctx->jpg_imgbuf);
		
		obuf=malloc((ctx->xs+1)*(ctx->ys+1)*4);
		memset(obuf, 0xFF, ctx->xs*ctx->ys*4);
		
		ctx->jpg_imgbuf=obuf;
		ctx->lxs=ctx->xs;
		ctx->lys=ctx->ys;
	}

#if 0
	for(i=0; i<ctx->jpg_nc; i++)
	{
		j=ctx->jpg_scn[i];
//		ctx->jpg_schsc[i]=ctx->jpg_ch[j]/(ctx->jpg_chm*1.0);
//		ctx->jpg_scvsc[i]=ctx->jpg_cv[j]/(ctx->jpg_chn*1.0);

		ctx->jpg_schsci[i]=(65536*ctx->jpg_ch[j])/ctx->jpg_chm;
		ctx->jpg_scvsci[i]=(65536*ctx->jpg_cv[j])/ctx->jpg_chn;

		ctx->jpg_schs[i]=(ctx->jpg_cxi[j]+7)/8;
		ctx->jpg_scvs[i]=(ctx->jpg_cyi[j]+7)/8;
		
	}

	if(ctx->jpg_nc==3)
	{
		i=1;
		if(ctx->jpg_schsci[0]!=65536)i=0;
		if(ctx->jpg_scvsci[0]!=65536)i=0;
		if(ctx->jpg_schsci[1]!=32768)i=0;
		if(ctx->jpg_scvsci[1]!=32768)i=0;
		if(ctx->jpg_schsci[2]!=32768)i=0;
		if(ctx->jpg_scvsci[2]!=32768)i=0;
		ctx->jpg_is420=i;

		i=1;
		if(ctx->jpg_schsci[0]!=65536)i=0;
		if(ctx->jpg_scvsci[0]!=65536)i=0;
		if(ctx->jpg_schsci[1]!=65536)i=0;
		if(ctx->jpg_scvsci[1]!=65536)i=0;
		if(ctx->jpg_schsci[2]!=65536)i=0;
		if(ctx->jpg_scvsci[2]!=65536)i=0;
		ctx->jpg_is444=i;
	}else
	{
		ctx->jpg_is420=0;
		ctx->jpg_is444=0;
	}
#endif

	BGBBTJ_JPG_DecodeCtxI_ColorTransform(ctx, obuf, ctx->xs, ctx->ys, clrs);

#if 0
	for(i=0; i<4; i++)
	{
		if(ctx->jpg_scbuf[i])free(ctx->jpg_scbuf[i]);
		if(ctx->jpg_sibuf[i])free(ctx->jpg_sibuf[i]);
		ctx->jpg_scbuf[i]=NULL;
		ctx->jpg_sibuf[i]=NULL;
	}
#endif

	*xs=ctx->xs;
	*ys=ctx->ys;
	return(obuf);
}

BGBBTJ_API void BGBBTJ_JPG_Free(byte *buf)
{
	if(!buf)return;
	free(buf);
}


