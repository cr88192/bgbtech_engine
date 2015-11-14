#include <walpng.h>

#ifndef FOURCC
#define FOURCC(a, b, c, d) ((a)|((b)<<8)|((c)<<16)|((d)<<24))
#endif


#define FCC_PNG_LOW	FOURCC(0x89, 'P', 'N', 'G')
#define FCC_PNG_HIGH	FOURCC(0x0D, 0x0A, 0x1A, 0x0A)

#define FCC_IHDR	FOURCC('I', 'H', 'D', 'R')
#define FCC_IEND	FOURCC('I', 'E', 'N', 'D')
#define FCC_IDAT	FOURCC('I', 'D', 'A', 'T')
#define FCC_PLTE	FOURCC('P', 'L', 'T', 'E')
#define FCC_tRNS	FOURCC('t', 'R', 'N', 'S')

uint BTV0_DataAdler32(void *buf, int sz, uint lcrc)
{
	byte *s;
	int i, c, s1, s2;

	s=buf;
	s1=lcrc&0xFFFF;
	s2=(lcrc>>16)&0xFFFF;
	for(i=0; i<sz; i++)
	{
		s1=(s1+(s[i]))%65521;
		s2=(s1+s2)%65521;
	}
	return((s2<<16)+s1);
}

uint BTV0_CRC32(void *buf, int sz, uint lcrc)
{
	static uint crctab[256];
	static int init=0;

	byte *s;
	uint c;
	int i, j;

	if(!init)
	{
		init=1;

		for(i=0; i<256; i++)
		{
			c=i;
			for(j=0; j<8; j++)
				if(c&1)c=0xedb88320^(c>>1);
					else c>>=1;
			crctab[i]=c;
		}
	}

	c=lcrc; s=buf; i=sz;
	while(i--)c=crctab[(c^(*s++))&0xFF]^(c>>8);
	return(c);
}

byte *BTV0_WriteInt32LE(byte *ct, uint v)
{
	*ct++=v&0xFF;
	*ct++=(v>>8)&0xFF;
	*ct++=(v>>16)&0xFF;
	*ct++=(v>>24)&0xFF;
	return(ct);
}

byte *BTV0_WriteInt32BE(byte *ct, uint v)
{
	*ct++=(v>>24)&0xFF;
	*ct++=(v>>16)&0xFF;
	*ct++=(v>>8)&0xFF;
	*ct++=v&0xFF;
	return(ct);
}

byte *BTV0_WriteFourcc(byte *ct, uint v)
{
	*ct++=v&0xFF;
	*ct++=(v>>8)&0xFF;
	*ct++=(v>>16)&0xFF;
	*ct++=(v>>24)&0xFF;
	return(ct);
}

byte *BTV0_WriteChunk(byte *ct, uint fcc, byte *buf, int len)
{
	byte tb[4];
	uint crc;

	*ct++=0x7F;
	*ct++=0xFF;
	*ct++=0xFE;
	*ct++=0x20;

//	ct=BTV0_WriteInt32BE(ct, len);
	ct=BTV0_WriteInt32LE(ct, len);
	ct=BTV0_WriteFourcc(ct, fcc);
//	fwrite(buf, 1, len, fd);
	memcpy(ct, buf, len);
	ct+=len;

#if 0
	tb[0]=fcc&0xFF; tb[1]=(fcc>>8)&0xFF;
	tb[2]=(fcc>>16)&0xFF; tb[3]=(fcc>>24)&0xFF;
	crc=BTV0_CRC32(tb, 4, (uint)-1);
	crc=BTV0_CRC32(buf, len, crc);

//	printf("%s %d %X\n", BTV0_Fourcc2String(fcc), len, ~crc);

	ct=BTV0_WriteInt32BE(ct, ~crc);
#endif
}

byte *BTV0_EmitString(byte *ct, char *str)
{
	strcpy(ct, str);
	ct+=strlen(str)+1;
	return(ct);
}

byte *BTV0_EmitMarker(byte *ct, int id)
{
	*ct++=0x7F;
	*ct++=0xFF;
	*ct++=0xFE;
	*ct++=id;
	return(ct);
}

byte *BTV0_EmitMarkerTag(byte *ct, char *name, char *value)
{
	byte *ct1;
	int i;

	printf("BTV0_EmitMarkerTag: %s %s\n", name, value);

	ct=BTV0_EmitMarker(ct, 0x21);

	ct1=ct;
	*ct++=0x00;
	*ct++=0x00;
	
	ct=BTV0_EmitString(ct, name);
	ct=BTV0_EmitString(ct, value);
	
	i=ct-ct1;
	ct1[0]=i&0xFF;
	ct1[0]=(i>>8)&0xFF;
	
	return(ct);
}

#ifndef ABS
#define ABS(v)	(((v)<0)?(-(v)):(v))
#endif

int BTV0_Paeth(int a, int b, int c)
{
	int p, pa, pb, pc;

	p=a+b-c;
	pa=(p>a)?(p-a):(a-p);
	pb=(p>b)?(p-b):(b-p);
	pc=(p>c)?(p-c):(c-p);

	p=(pa<=pb)?((pa<=pc)?a:c):((pb<=pc)?b:c);
	return(p);
}

int BTV0_Mod(int x, int xs)
{
	while(x<0)x+=xs;
	while(x>=xs)x-=xs;
	return(x);
}

int BTV0_FindImageBufferBestOffset(PDBTV0_Context *ctx,
	byte *olbuf, byte *buf, int xs, int ys,
	int x, int y, int *rxo, int *ryo)
{
	int xo, yo, x2, y2, x3, y3;
	int bx, by, be;
	int a, b;
	int i, j, k, l, e;

	*rxo=0;
	*ryo=0;

	bx=0; by=0; be=xs*255*4;

	for(i=0; i<32; i++)
		for(j=0; j<32; j++)
	{
		xo=(j&1)?(-(j>>1)):(j>>1);
		yo=(i&1)?(-(i>>1)):(i>>1);
		
		x2=x+xo;
		y2=y+yo;

		y3=BTV0_Mod(ys-y2-1, ys);
		
		e=0;
		for(k=0; k<xs; k++)
		{
			x3=BTV0_Mod(x2+k, xs);
		
			for(l=0; l<4; l++)
			{
				a=buf[(((ys-y-1)*xs)+k)*4+l];
				b=olbuf[((y3*xs)+x3)*4+l];
				e+=ABS(a-b);
			}
			
			if(e>=be)break;
		}
		
		if(e<be) { bx=xo; by=yo; be=e; }
	}
	
//	printf("%dx%d ", bx, by);

	*rxo=bx;
	*ryo=by;
	return(1);
}

byte *BTV0_EncodeImageBuffer(PDBTV0_Context *ctx,
	byte *obuf, byte *buf,
	int xs, int ys, int layer)
{
	byte hbuf[64];
	int tpx[4];
	int tfmc[64];
	byte *buf2, *olbuf, *s, *t, *ct;
	int fm, sz, pa, pb, pc, pd;
	int pe, pf, pg, ph, xo, yo;
	int toy0, toy1, tox0, tox1;
	int tpr, tpg, tpb, tpy, tpu, tpv;
	int e, be, bf, am, ssz, bcs;
	int i, j, k, l;

	am=0;
	for(i=0; i<(xs*ys); i++)
		if(buf[i*4+3]<255)am=1;
	ssz=am?4:3;

	buf2=malloc(xs*ys*ssz+16*ys);

	olbuf=ctx->oldlayer[layer];
	if(!olbuf)
	{
		olbuf=malloc(xs*ys*4);
		memset(olbuf, 0, xs*ys*4);
		ctx->oldlayer[layer]=olbuf;
	}

	for(i=0; i<64; i++)tfmc[i]=0;

	t=buf2;
	for(i=0; i<ys; i++)
	{
		BTV0_FindImageBufferBestOffset(ctx, olbuf, buf, xs, ys,
			0, i, &xo, &yo);

		toy0=BTV0_Mod(ys-(i+yo), ys);
		toy1=BTV0_Mod(ys-(i+yo)-1, ys);
	
		be=ssz*xs*255; bf=0;
		for(l=0; l<24; l++)
		{
			e=0;
			for(j=0; j<xs; j++)
			{
				tox0=BTV0_Mod(j+xo, xs);
				tox1=BTV0_Mod(j+xo-1, xs);

				for(k=0; k<ssz; k++)
				{
					pd=buf[(((ys-i-1)*xs)+j)*4+k];
//					ph=olbuf[(((ys-i-1)*xs)+j)*4+k];

					pe=olbuf[((toy0*xs)+tox1)*4+k];
					pf=olbuf[((toy0*xs)+tox0)*4+k];
					pg=olbuf[((toy1*xs)+tox1)*4+k];
					ph=olbuf[((toy1*xs)+tox0)*4+k];

					if(i && j)
					{
						pa=buf[(((ys-i)*xs)+j-1)*4+k];
						pb=buf[(((ys-i)*xs)+j)*4+k];
						pc=buf[(((ys-i-1)*xs)+j-1)*4+k];

//						pe=olbuf[(((ys-i)*xs)+j-1)*4+k];
//						pf=olbuf[(((ys-i)*xs)+j)*4+k];
//						pg=olbuf[(((ys-i-1)*xs)+j-1)*4+k];
					}else if(i)
					{
						pa=0; pb=buf[(((ys-i)*xs)+j)*4+k]; pc=0;
//						pe=0; pf=olbuf[(((ys-i)*xs)+j)*4+k]; pg=0;
					}else if(j)
					{
						pa=0; pb=0; pc=buf[(((ys-i-1)*xs)+j-1)*4+k];
//						pe=0; pf=0; pg=olbuf[(((ys-i-1)*xs)+j-1)*4+k];
					}else
					{
						pa=0; pb=0; pc=0;
//						pe=0; pf=0; pg=0;
					}

					switch(l)
					{
					case 0: e+=ABS(pd); break;
					case 1: e+=ABS(pd-pc); break;
					case 2: e+=ABS(pd-pb); break;
					case 3: e+=ABS(pd-((pc+pb)>>1)); break;
					case 4: e+=ABS(pd-BTV0_Paeth(pc, pb, pa)); break;
					case 5: e+=ABS(pd-((pb+pc)-pa)); break;
					case 8: e+=ABS(pd-ph); break;
					case 9: e+=ABS(pd-pg); break;
					case 10: e+=ABS(pd-pf); break;
					case 11: e+=ABS(pd-((pg+pf)>>1)); break;
					case 12: e+=ABS(pd-BTV0_Paeth(pg, pf, pe)); break;
					case 13: e+=ABS(pd-((pf+pg)-pe)); break;
					case 14: e+=ABS(pd-(2*((pb+pc)-pa)-ph)); break;
					case 15: e+=ABS(pd-(2*ph-((pb+pc)-pa))); break;

					case 16:
						e+=ABS(pd-((BTV0_Paeth(pc, pb, pa)+ph)>>1));
						break;
					case 17: e+=ABS(pd-(((ph+pb+pc)-pa)>>1)); break;
					
					default: e+=2*ABS(pd); break;
					}
				}
			}

			if(e<be)
			{
				be=e;
				bf=l;
			}
		}

//		printf("%d(%.2f) ", bf, ((float)be)/(ssz*xs));

		fm=bf;
		tfmc[fm]++;

		be=ssz*xs*255; bcs=0;
		for(l=0; l<16; l++)
		{
			e=0;
			for(j=0; j<xs; j++)
			{
				tox0=BTV0_Mod(j+xo, xs);
				tox1=BTV0_Mod(j+xo-1, xs);

				tpx[0]=0; tpx[1]=0;
				tpx[2]=0; tpx[3]=0;
			
				for(k=0; k<ssz; k++)
				{
					pd=buf[(((ys-i-1)*xs)+j)*4+k];
//					ph=olbuf[(((ys-i-1)*xs)+j)*4+k];

					pe=olbuf[((toy0*xs)+tox1)*4+k];
					pf=olbuf[((toy0*xs)+tox0)*4+k];
					pg=olbuf[((toy1*xs)+tox1)*4+k];
					ph=olbuf[((toy1*xs)+tox0)*4+k];

					if(i && j)
					{
						pa=buf[(((ys-i)*xs)+j-1)*4+k];
						pb=buf[(((ys-i)*xs)+j)*4+k];
						pc=buf[(((ys-i-1)*xs)+j-1)*4+k];

//						pe=olbuf[(((ys-i)*xs)+j-1)*4+k];
//						pf=olbuf[(((ys-i)*xs)+j)*4+k];
//						pg=olbuf[(((ys-i-1)*xs)+j-1)*4+k];
					}else if(i)
					{
						pa=0; pb=buf[(((ys-i)*xs)+j)*4+k]; pc=0;
//						pe=0; pf=olbuf[(((ys-i)*xs)+j)*4+k]; pg=0;
					}else if(j)
					{
						pa=0; pb=0; pc=buf[(((ys-i-1)*xs)+j-1)*4+k];
//						pe=0; pf=0; pg=olbuf[(((ys-i-1)*xs)+j-1)*4+k];
					}else
					{
						pa=0; pb=0; pc=0;
//						pe=0; pf=0; pg=0;
					}

					switch(fm)
					{
					case 0: tpx[k]=pd; break;
					case 1: tpx[k]=pd-pc; break;
					case 2: tpx[k]=pd-pb; break;
					case 3: tpx[k]=pd-((pc+pb)>>1); break;
					case 4: tpx[k]=pd-BTV0_Paeth(pc, pb, pa); break;
					case 5: tpx[k]=pd-((pb+pc)-pa); break;
					case 8: tpx[k]=pd-ph; break;
					case 9: tpx[k]=pd-pg; break;
					case 10: tpx[k]=pd-pf; break;
					case 11: tpx[k]=pd-((pg+pf)>>1); break;
					case 12: tpx[k]=pd-BTV0_Paeth(pg, pf, pe); break;
					case 13: tpx[k]=pd-((pf+pg)-pe); break;
					case 14: tpx[k]=pd-(2*((pb+pc)-pa)-ph); break;
					case 15: tpx[k]=pd-(2*ph-((pb+pc)-pa)); break;

					case 16:
						tpx[k]=pd-((BTV0_Paeth(pc, pb, pa)+ph)>>1);
						break;
					case 17: tpx[k]=pd-(((ph+pb+pc)-pa)>>1); break;
					}
				}
				
				switch(l)
				{
				case 0: case 1:
					e+=ABS(tpx[0])+ABS(tpx[1])+ABS(tpx[2])+ABS(tpx[3]);
					break;
				case 2: case 3:
					tpy=tpx[1]; tpu=tpx[2]-tpx[1]; tpv=tpx[0]-tpx[1];
					e+=ABS(tpy)+ABS(tpu)+ABS(tpv)+ABS(tpx[3]);
					break;

#if 0
				case 8: case 9:
					tpy=0+tpx[0]*0.2857+tpx[1]*0.5714+tpx[2]*0.1429;
					tpu=0-tpx[0]*0.1429-tpx[1]*0.2857+tpx[2]*0.4286;
					tpv=0+tpx[0]*0.3571-tpx[1]*0.2857+tpx[2]*0.0714;
					e+=ABS(tpy)+ABS(tpu)+ABS(tpv)+ABS(tpx[3]);
					break;
#endif

				default: e+=4*256; break;
				}
			}

			if(e<be)
			{
				be=e;
				bcs=l;
			}
		}

		if((bcs==0) && am)bcs=1;
		if((bcs==2) && am)bcs=3;
		if((bcs==4) && am)bcs=5;

//		if(bcs!=0)
//			printf("C%d(%.2f) ", bcs, ((float)be)/(ssz*xs));

		fm=bf;
		*t++=fm;
		*t++=bcs;

		for(j=0; j<xs; j++)
		{
			tox0=BTV0_Mod(j+xo, xs);
			tox1=BTV0_Mod(j+xo-1, xs);

			tpx[0]=0; tpx[1]=0;
			tpx[2]=0; tpx[3]=0;

			for(k=0; k<ssz; k++)
			{
				pd=buf[(((ys-i-1)*xs)+j)*4+k];
//				ph=olbuf[(((ys-i-1)*xs)+j)*4+k];

				pe=olbuf[((toy0*xs)+tox1)*4+k];
				pf=olbuf[((toy0*xs)+tox0)*4+k];
				pg=olbuf[((toy1*xs)+tox1)*4+k];
				ph=olbuf[((toy1*xs)+tox0)*4+k];

				if(i && j)
				{
					pa=buf[(((ys-i)*xs)+j-1)*4+k];
					pb=buf[(((ys-i)*xs)+j)*4+k];
					pc=buf[(((ys-i-1)*xs)+j-1)*4+k];

//					pe=olbuf[(((ys-i)*xs)+j-1)*4+k];
//					pf=olbuf[(((ys-i)*xs)+j)*4+k];
//					pg=olbuf[(((ys-i-1)*xs)+j-1)*4+k];
				}else if(i)
				{
					pa=0; pb=buf[(((ys-i)*xs)+j)*4+k]; pc=0;
//					pe=0; pf=olbuf[(((ys-i)*xs)+j)*4+k]; pg=0;
				}else if(j)
				{
					pa=0; pb=0; pc=buf[(((ys-i-1)*xs)+j-1)*4+k];
//					pe=0; pf=0; pg=olbuf[(((ys-i-1)*xs)+j-1)*4+k];
				}else
				{
					pa=0; pb=0; pc=0;
//					pe=0; pf=0; pg=0;
				}

#if 1
				switch(fm)
				{
				case 0: tpx[k]=pd; break;
				case 1: tpx[k]=pd-pc; break;
				case 2: tpx[k]=pd-pb; break;
				case 3: tpx[k]=pd-((pc+pb)>>1); break;
				case 4: tpx[k]=pd-BTV0_Paeth(pc, pb, pa); break;
				case 5: tpx[k]=pd-((pb+pc)-pa); break;
				case 8: tpx[k]=pd-ph; break;
				case 9: tpx[k]=pd-pg; break;
				case 10: tpx[k]=pd-pf; break;
				case 11: tpx[k]=pd-((pg+pf)>>1); break;
				case 12: tpx[k]=pd-BTV0_Paeth(pg, pf, pe); break;
				case 13: tpx[k]=pd-((pf+pg)-pe); break;
				case 14: tpx[k]=pd-(2*((pb+pc)-pa)-ph); break;
				case 15: tpx[k]=pd-(2*ph-((pb+pc)-pa)); break;

				case 16:
					tpx[k]=pd-((BTV0_Paeth(pc, pb, pa)+ph)>>1);
					break;
				case 17: tpx[k]=pd-(((ph+pb+pc)-pa)>>1); break;
				}
#endif

#if 0
				switch(fm)
				{
				case 0:
					*t++=pd;
					break;
				case 1:
					*t++=pd-pc;
					break;
				case 2:
					*t++=pd-pb;
					break;
				case 3:
					*t++=pd-((pc+pb)>>1);
					break;
				case 4:
					*t++=pd-BTV0_Paeth(pc, pb, pa);
					break;
				case 5:
					*t++=pd-((pb+pc)-pa);
					break;

				case 8:
					*t++=pd-ph;
					break;
				case 9:
					*t++=pd-pg;
					break;
				case 10:
					*t++=pd-pf;
					break;
				case 11:
					*t++=pd-((pg+pf)>>1);
					break;
				case 12:
					*t++=pd-BTV0_Paeth(pg, pf, pe);
					break;
				case 13:
					*t++=pd-((pf+pg)-pe);
					break;
				case 14:
					*t++=pd-(2*((pb+pc)-pa)-ph);
					break;
				case 15:
					*t++=pd-(2*ph-((pb+pc)-pa));
					break;

				default:
					break;
				}
#endif

			}

			switch(bcs)
			{
			case 0: *t++=tpx[0]; *t++=tpx[1]; *t++=tpx[2]; break;
			case 1:
				*t++=tpx[0]; *t++=tpx[1];
				*t++=tpx[2]; *t++=tpx[3]; break;
			case 2:
				tpy=tpx[1]; tpu=tpx[2]-tpx[1]; tpv=tpx[0]-tpx[1];
				*t++=tpy; *t++=tpu; *t++=tpv; break;
			case 3:
				tpy=tpx[1]; tpu=tpx[2]-tpx[1]; tpv=tpx[0]-tpx[1];
				*t++=tpy; *t++=tpu; *t++=tpv; *t++=tpx[3]; break;

#if 0
			case 8: case 9:
				tpy=0+tpx[0]*0.2857+tpx[1]*0.5714+tpx[2]*0.1429;
				tpu=0-tpx[0]*0.1429-tpx[1]*0.2857+tpx[2]*0.4286;
				tpv=0+tpx[0]*0.3571-tpx[1]*0.2857+tpx[2]*0.0714;
				e+=ABS(tpy)+ABS(tpu)+ABS(tpv)+ABS(tpx[3]);
				break;
#endif

			default: e+=4*256; break;
			}
		}
	}
//	printf("\n");

	for(i=0; i<64; i++)
	{
		if(tfmc[i])
		{
			j=tfmc[i];
			printf("Fm %d: %d/%d %.02f%%\n", i, j, ys, 100*((float)j)/ys);
		}
	}

	memcpy(olbuf, buf, xs*ys*4);

	buf=malloc(xs*ys*8);

	//zlib header
	i=(7<<12)|(8<<8)|(3<<6);
	j=i%31;
	if(j)i|=31-j;
	buf[0]=i>>8;
	buf[1]=i&0xFF;
//	printf("Zlib %04X\n", i);

	i=PDZ2_EncodeStreamLvl(buf2, buf+2, xs*ys*ssz+ys, xs*ys*8, 1);
//	i=PDZ2_EncodeStreamLvl(buf2, buf+2, xs*ys*ssz+ys, xs*ys*8, 9);
//	i=vfDeflateBufferLvl(buf+2, buf2, xs*ys*8, xs*ys*ssz+ys, 9);

	//checksum
	j=BTV0_DataAdler32(buf2, xs*ys*ssz+ys, 1);
	buf[2+i+0]=(j>>24)&0xFF;
	buf[2+i+1]=(j>>16)&0xFF;
	buf[2+i+2]=(j>>8)&0xFF;
	buf[2+i+3]=j&0xFF;

	printf("Adler32: Wrote %08X (Size=%d)\n", j, i);

	sz=i+6;
	free(buf2);

	ct=obuf;

//	BTV0_WriteFourcc(fd, FCC_PNG_LOW);
//	BTV0_WriteFourcc(fd, FCC_PNG_HIGH);

	hbuf[0]=xs>>24; hbuf[1]=xs>>16; hbuf[2]=xs>>8; hbuf[3]=xs;
	hbuf[4]=ys>>24; hbuf[5]=ys>>16; hbuf[6]=ys>>8; hbuf[7]=ys;
	hbuf[8]=8;	//8 bits/sample
	hbuf[9]=am?6:2;	//RGB(A)
	hbuf[10]=0;	//0=deflate
	hbuf[11]=0;	//no filter
	hbuf[12]=0;	//no interlace

	ct=BTV0_WriteChunk(ct, FCC_IHDR, hbuf, 13);

	s=buf; i=sz;
	while(i>65536)
	{
		ct=BTV0_WriteChunk(ct, FCC_IDAT, s, 65536);
		s+=65536;
		i-=65536;
	}
	ct=BTV0_WriteChunk(ct, FCC_IDAT, s, i);

//	ct=BTV0_WriteChunk(ct, FCC_IEND, NULL, 0);
	
	return(ct);
}

PDBTV0_Context *BTV0_AllocContext()
{
	PDBTV0_Context *ctx;
	
	ctx=malloc(sizeof(PDBTV0_Context));
	memset(ctx, 0, sizeof(PDBTV0_Context));
	return(ctx);
}

int BTV0_EncodeCtx(PDBTV0_Context *ctx,
	byte *ibuf, byte *obuf, int xs, int ys, int qf, int pf)
{
	byte *ct;
	
	ct=obuf;
	ct=BTV0_EmitMarkerTag(ct, "CompLayer", "RGBA");
	ct=BTV0_EncodeImageBuffer(ctx, ct, ibuf, xs, ys, 0);
	return(ct-obuf);
}

int BTV0_EncodeComponentCtx(
	PDBTV0_Context *ctx,
	byte *rgba, byte *norm, byte *spec, byte *luma,
	byte *obuf, int xs, int ys, int qf, int pf)
{
	byte *ct;
	
	ct=obuf;
	if(rgba)
	{
		ct=BTV0_EmitMarkerTag(ct, "CompLayer", "RGBA");
		ct=BTV0_EncodeImageBuffer(ctx, ct, rgba, xs, ys, 0);
	}
	if(norm)
	{
		ct=BTV0_EmitMarkerTag(ct, "CompLayer", "XYZD");
		ct=BTV0_EncodeImageBuffer(ctx, ct, norm, xs, ys, 1);
	}
	if(spec)
	{
		ct=BTV0_EmitMarkerTag(ct, "CompLayer", "SpRGBA");
		ct=BTV0_EncodeImageBuffer(ctx, ct, spec, xs, ys, 2);
	}
	if(luma)
	{
		ct=BTV0_EmitMarkerTag(ct, "CompLayer", "LuRGBA");
		ct=BTV0_EncodeImageBuffer(ctx, ct, luma, xs, ys, 3);
	}
	return(ct-obuf);
}
