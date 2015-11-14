/*
DXTn packed images.

Each block tag will be encoded in the form (byte):
0 <block:QWORD>		Literal Block.
1-127				Single byte block index.
128-191	X			Two byte block index (16384 blocks).
192-223	XX			Three byte block index (2097152 blocks).
224-238 I			LZ/RLE Run (2-16 blocks, Index)
239		LI			LZ/RLE Run (Length, Index)
240		XXX			24-Bit Index
241		XXXX		32-Bit Index
242-246				Literal Blocks (2-6 Blocks)
247		L			Literal Blocks (L Blocks)
248-250	J			Raster Relative 1-3, X (Sign Folded X)
251		IJ			Raster Relative 4+, YX (Sign Folded X)
252-255				Escape (2-byte commands)

The block index will indicate how many blocks backwards to look for a matching block (1 will repeat the prior block).

Length/Index values will use the same organization as above, only limited to encoding numeric values.

0-127				0-127.
128-191	X			128-16383.
192-223	XX			16384-2097151.
240		XXX			24-Bit Index (0-16777215)
241		XXXX		32-Bit Index (0-4294967295)
248-250	J			Raster Relative 1-3, X (Sign Folded X)
251		IJ			Raster Relative 4+, YX (Sign Folded X)

Sign Folding will fold the sign according to the pattern:
0, -1, 1, -2, 2, ...

Note that DXT5 images will be split into 2 block-planes, with the first encoding the alpha component, followed by the plane encoding the RGB components.

If used with BTAC, this will be split into 4 or 16 planes.

Raster Relative References will encode an offset relative to a scanline (as opposed to a linear distance).
*/

#include <bgbbtj.h>

byte *BGBBTJ_PackBCn_EmitValue(byte *ct, int val)
{
	if(val<128)
	{
		*ct++=val;
		return(ct);
	}
	if(val<16384)
	{
		*ct++=128+(val>>8);
		*ct++=val;
		return(ct);
	}
	if(val<2097152)
	{
		*ct++=192+(val>>16);
		*ct++=val>>8;
		*ct++=val;
		return(ct);
	}
	if(val<1677216)
	{
		*ct++=240;
		*ct++=val>>16;
		*ct++=val>>8;
		*ct++=val;
		return(ct);
	}
	*ct++=241;
	*ct++=val>>24;	*ct++=val>>16;
	*ct++=val>>8;	*ct++=val;
	return(ct);
}

byte *BGBBTJ_PackBCn_EmitRun(byte *ct, int idx, int len)
{
	if(idx==0)
	{
		if(len<2)
		{
			*ct++=0;
			return(ct);
		}
		if(len<7)
		{
			*ct++=242+(len-2);
			return(ct);
		}

		*ct++=247;
		ct=BGBBTJ_PackBCn_EmitValue(ct, len);
		return(ct);
	}

	if(len<2)
	{
		ct=BGBBTJ_PackBCn_EmitValue(ct, idx);
		return(ct);
	}

	if(len<=16)
	{
		*ct++=224+(len-2);
		ct=BGBBTJ_PackBCn_EmitValue(ct, idx);
		return(ct);
	}

	*ct++=239;
	ct=BGBBTJ_PackBCn_EmitValue(ct, len);
	ct=BGBBTJ_PackBCn_EmitValue(ct, idx);
	return(ct);
}

byte *BGBBTJ_PackBCn_EmitValueSign(
	byte *ct, int val)
{
	return(BGBBTJ_PackBCn_EmitValue(ct, (val>=0)?(val<<1):(((-val)<<1)-1)));
}

byte *BGBBTJ_PackBCn_EmitValueScan(
	byte *ct, int val, int scan)
{
	int by, bx;

	if((val>=128) && (scan>1))
	{
		by=(val+(scan>>1))/scan;
		bx=val-by*scan;
		
		if((by>=1) && (by<=3) && (bx>=-64) && (bx<64))
		{
			*ct++=248+(by-1);
			ct=BGBBTJ_PackBCn_EmitValueSign(ct, bx);
			return(ct);
		}

#if 1
		if((val>=16384) && (by>=4) && (by<128) && (bx>=-64) && (bx<64))
		{
			*ct++=251;
			ct=BGBBTJ_PackBCn_EmitValue(ct, by);
			ct=BGBBTJ_PackBCn_EmitValueSign(ct, bx);
			return(ct);
		}
#endif
	}

	return(BGBBTJ_PackBCn_EmitValue(ct, val));
}

byte *BGBBTJ_PackBCn_EmitRunScan(
	byte *ct, int idx, int len, int scan)
{
	if(idx==0)
	{
		if(len<2)
		{
			*ct++=0;
			return(ct);
		}
		if(len<7)
		{
			*ct++=242+(len-2);
			return(ct);
		}

		*ct++=247;
		ct=BGBBTJ_PackBCn_EmitValue(ct, len);
		return(ct);
	}

	if(len<2)
	{
		ct=BGBBTJ_PackBCn_EmitValueScan(ct, idx, scan);
		return(ct);
	}

	if(len<=16)
	{
		*ct++=224+(len-2);
		ct=BGBBTJ_PackBCn_EmitValueScan(ct, idx, scan);
		return(ct);
	}

	*ct++=239;
	ct=BGBBTJ_PackBCn_EmitValue(ct, len);
	ct=BGBBTJ_PackBCn_EmitValueScan(ct, idx, scan);
	return(ct);
}

byte *BGBBTJ_PackBCn_EmitBlockValue(byte *ct, u64 block)
{
	*ct++=block;		*ct++=block>>8;
	*ct++=block>>16;	*ct++=block>>24;
	*ct++=block>>32;	*ct++=block>>40;
	*ct++=block>>48;	*ct++=block>>56;
	return(ct);
}

u64 BGBBTJ_PackBCn_GetBlockValue(byte *cs)
{
	u32 t1, t2;
	u64 tmp;
#if defined(X86) || defined(X86_64)
	return(*(u64 *)cs);
#else
	t1=cs[0] | (cs[1]<<8) | (cs[2]<<16) | (cs[3]<<24);
	t2=cs[4] | (cs[5]<<8) | (cs[6]<<16) | (cs[7]<<24);
	tmp=t1+(((u64)t2)<<32);
#endif
	return(tmp);
}

int BGBBTJ_PackBCn_LookupBlockSpan(
	byte *css, byte *cs, byte *cse, int stride,
	int *blkchn, int *blkhash,
	int *ridx, int *rlen, int max)
{
	u64 li, lj, lk;
	int bi, bl, ci, cl, shr;
	byte *csi, *csj, *csk;
	int i;

	switch(stride)
	{
	case 4: shr=2; break;
	case 8: shr=3; break;
	case 16: shr=4; break;
	case 32: shr=5; break;
	case 64: shr=6; break;
	}

	li=BGBBTJ_PackBCn_GetBlockValue(cs);
	
//	lj=li*2147483647*127*127;
	lj=li*187284906682965LL;
//	ci=(lj>>48)&4095;
	ci=(lj>>48)&65535;
	i=blkhash[ci];

	if(i<=0) { 	*ridx=0; *rlen=0; return(0); }

	csi=css+i*stride;
	
//	csi=cs-stride;
	bi=0; bl=0;

//	while((csi>=css) && (csi<cs))
	while(csi>=css)
	{
		lj=BGBBTJ_PackBCn_GetBlockValue(csi);
		if(li!=lj)
			{ csi-=stride; continue; }
		csj=csi+stride;
		csk=cs+stride;
		while(csk<cse)
		{
			lj=BGBBTJ_PackBCn_GetBlockValue(csj);
			lk=BGBBTJ_PackBCn_GetBlockValue(csk);
			if(lj!=lk)break;
			csj+=stride; csk+=stride;
		}
		
//		ci=(cs-csi)/stride;
//		cl=(csk-cs)/stride;
		ci=(cs-csi)>>shr;
		cl=(csk-cs)>>shr;
		if(cl>bl) { bi=ci; bl=cl; }
		if(bl>=max)break;
//		csi-=stride;

//		if((ci>1024) && (bl>=256))
//			break;
//		if((ci>4096) && (bl>=64))
//			break;

//		if((ci*bl)>65536)
//			break;

//		i=blkchn[(cs-css)/stride];
		i=blkchn[(cs-css)>>shr];
		if(i<=0)break;
		csi-=i*stride;
	}
	
	*ridx=bi;
	*rlen=bl;
	return(0);
}

int BGBBTJ_PackBCn_UpdateBlockSpan(
	byte *css, byte *cs, byte *cse, int stride,
	int *blkchn, int *blkhash, int len)
{
	u64 li, lj;
	byte *cs1;
	int ci, cj;
	int i, j, k;

	for(i=0; i<len; i++)
	{
		cs1=cs+i*stride;
		li=BGBBTJ_PackBCn_GetBlockValue(cs1);
		lj=li*187284906682965LL;
//		ci=(lj>>48)&4095;
		ci=(lj>>48)&65535;
		
		cj=(cs1-css)/stride;
		k=cj-blkhash[ci];
		blkchn[cj]=k;
		blkhash[ci]=cj;
	}
	return(0);
}

byte *BGBBTJ_PackBCn_EncodeBlockArray(byte *obuf, byte *blks,
	int count, int stride)
{
	byte *cs, *cse, *ct, *cs1;
	int *blkchn, *blkhash;
	u64 li;
	int i, bi, bl, bi1, bl1;
	
	blkchn=malloc(count*sizeof(int));
	blkhash=malloc(65536*sizeof(int));
	memset(blkchn, 0, count*sizeof(int));
	memset(blkhash, 0, 65536*sizeof(int));
	
	ct=obuf;
	cs=blks; cse=blks+count*stride;
	while(cs<cse)
	{
		BGBBTJ_PackBCn_LookupBlockSpan(blks, cs, cse, stride,
			blkchn, blkhash,
			&bi, &bl, 65536);

#if 1
		if(bi==0)
		{
			bl=1; cs1=cs+stride;
//			while((cs1<cse) && (bl<6))
//			while(cs1<cse)
			while((cs1<cse) && (bl<256))
			{
				BGBBTJ_PackBCn_LookupBlockSpan(
					blks, cs1, cse, stride,
					blkchn, blkhash,
					&bi1, &bl1, 1);
				if(bi1>0)break;
				bl++;
				cs1=cs+bl*stride;
			}
		}
#endif

		BGBBTJ_PackBCn_UpdateBlockSpan(blks, cs, cse, stride,
			blkchn, blkhash, bl);

		if(bi<1)
		{
			if(bl<2)
			{
				*ct++=0;
				li=BGBBTJ_PackBCn_GetBlockValue(cs);
				ct=BGBBTJ_PackBCn_EmitBlockValue(ct, li);
				cs+=stride;
				continue;
			}
			ct=BGBBTJ_PackBCn_EmitRun(ct, 0, bl);
			for(i=0; i<bl; i++)
			{
				li=BGBBTJ_PackBCn_GetBlockValue(cs);
				ct=BGBBTJ_PackBCn_EmitBlockValue(ct, li);
				cs+=stride;
			}
			continue;
		}
		ct=BGBBTJ_PackBCn_EmitRun(ct, bi, bl);
		cs+=bl*stride;
	}
	
	free(blkchn);
	free(blkhash);
	
	return(ct);
}

BGBBTJ_API int BGBBTJ_PackBCn_EncodeBlocksDXT1(
	byte *obuf, byte *blks, int count)
{
	byte *ct;
	ct=BGBBTJ_PackBCn_EncodeBlockArray(obuf, blks, count, 8);
	return(ct-obuf);
}

BGBBTJ_API int BGBBTJ_PackBCn_EncodeBlocksDXT5(
	byte *obuf, byte *blks, int count)
{
	byte *ct;
	ct=BGBBTJ_PackBCn_EncodeBlockArray(obuf, blks, count, 16);
	ct=BGBBTJ_PackBCn_EncodeBlockArray(ct, blks+8, count, 16);
	return(ct-obuf);
}

byte *BGBBTJ_PackBCn_EncodePatchBlockArray(byte *obuf, byte *blks,
	byte *iblks, int count, int stride)
{
	byte *cs, *cse, *ct, *cs1;
	int *blkchn, *blkhash;
	u64 li;
	int i, bi, bl, bi1, bl1;
	
	blkchn=malloc(2*count*sizeof(int));
	blkhash=malloc(65536*sizeof(int));
	memset(blkchn, 0, 2*count*sizeof(int));
	memset(blkhash, 0, 65536*sizeof(int));

	cs=iblks; cse=iblks+count*stride;
	BGBBTJ_PackBCn_UpdateBlockSpan(iblks, cs, cse, stride,
		blkchn, blkhash, count);

	ct=obuf;
	cs=blks; cse=blks+count*stride;
	while(cs<cse)
	{
		BGBBTJ_PackBCn_LookupBlockSpan(blks, cs, cse, stride,
			blkchn, blkhash,
			&bi, &bl, 65536);

#if 1
		if(bi==0)
		{
			bl=1; cs1=cs+stride;
//			while((cs1<cse) && (bl<6))
//			while(cs1<cse)
			while((cs1<cse) && (bl<256))
			{
				BGBBTJ_PackBCn_LookupBlockSpan(
					blks, cs1, cse, stride,
					blkchn, blkhash,
					&bi1, &bl1, 1);
				if(bi1>0)break;
				bl++;
				cs1=cs+bl*stride;
			}
		}
#endif

		BGBBTJ_PackBCn_UpdateBlockSpan(blks, cs, cse, stride,
			blkchn, blkhash, bl);

		if(bi<1)
		{
			if(bl<2)
			{
				*ct++=0;
				li=BGBBTJ_PackBCn_GetBlockValue(cs);
				ct=BGBBTJ_PackBCn_EmitBlockValue(ct, li);
				cs+=stride;
				continue;
			}
			ct=BGBBTJ_PackBCn_EmitRun(ct, 0, bl);
			for(i=0; i<bl; i++)
			{
				li=BGBBTJ_PackBCn_GetBlockValue(cs);
				ct=BGBBTJ_PackBCn_EmitBlockValue(ct, li);
				cs+=stride;
			}
			continue;
		}
		ct=BGBBTJ_PackBCn_EmitRun(ct, bi, bl);
		cs+=bl*stride;
	}
	
	free(blkchn);
	free(blkhash);
	
	return(ct);
}

BGBBTJ_API int BGBBTJ_PackBCn_EncodePatchBlocksDXT1(
	byte *obuf, byte *blks, int count)
{
	byte *ct;
	ct=BGBBTJ_PackBCn_EncodePatchBlockArray(
		obuf, blks+count*8, blks, count, 8);
	return(ct-obuf);
}

BGBBTJ_API int BGBBTJ_PackBCn_EncodePatchBlocksDXT5(
	byte *obuf, byte *blks, int count)
{
	byte *ct;
	ct=BGBBTJ_PackBCn_EncodePatchBlockArray(
		obuf, blks+count*16, blks, count, 16);
	ct=BGBBTJ_PackBCn_EncodePatchBlockArray(
		ct, blks+count*16+8, blks+8, count, 16);
	return(ct-obuf);
}

byte *BGBBTJ_PackBCn_DecodeValue(byte *cs, int *rval)
{
	int op, i;
	
	op=*cs++;
	if(op<128)
		{ *rval=op; return(cs); }
	if(op<192)
	{
		*rval=((op-128)<<8)|(*cs++);
		return(cs);
	}
	if(op<224)
	{
		i=((op-192)<<8)|(*cs++);
		i=(i<<8)|(*cs++);
		*rval=i;
		return(cs);
	}

	if(op==240)
	{
		i=*cs++;
		i=(i<<8)|(*cs++);
		i=(i<<8)|(*cs++);
		*rval=i;
		return(cs);
	}
	if(op==241)
	{
		i=*cs++;			i=(i<<8)|(*cs++);
		i=(i<<8)|(*cs++);	i=(i<<8)|(*cs++);
		*rval=i;
		return(cs);
	}

	return(cs);
}

byte *BGBBTJ_PackBCn_DecodeRun(byte *cs, int *ridx, int *rlen)
{
	int op, i;
	
	op=*cs++;
	if(!op)
		{ *ridx=0; *rlen=0; return(cs); }
	if(op<128)
		{ *ridx=op; *rlen=1; return(cs); }
	if(op<192)
	{
		*ridx=((op-128)<<8)|(*cs++);
		*rlen=1;
		return(cs);
	}
	if(op<224)
	{
		i=((op-192)<<8)|(*cs++);
		i=(i<<8)|(*cs++);
		*ridx=i;
		*rlen=1;
		return(cs);
	}
	if(op<239)
	{
		cs=BGBBTJ_PackBCn_DecodeValue(cs, ridx);
		*rlen=(op-224)+2;
		return(cs);
	}
	if(op==239)
	{
		cs=BGBBTJ_PackBCn_DecodeValue(cs, rlen);
		cs=BGBBTJ_PackBCn_DecodeValue(cs, ridx);
		return(cs);
	}
	if(op==240)
	{
		i=*cs++;
		i=(i<<8)|(*cs++);
		i=(i<<8)|(*cs++);
		*ridx=i;
		*rlen=1;
		return(cs);
	}
	if(op==241)
	{
		i=*cs++;			i=(i<<8)|(*cs++);
		i=(i<<8)|(*cs++);	i=(i<<8)|(*cs++);
		*ridx=i;
		*rlen=1;
		return(cs);
	}

	if(op<247)
	{
		*ridx=0;
		*rlen=(op-242)+2;
		return(cs);
	}
	if(op==247)
	{
		*ridx=0;
		cs=BGBBTJ_PackBCn_DecodeValue(cs, rlen);
		return(cs);
	}

	*ridx=0; *rlen=0;
	return(cs);
}

byte *BGBBTJ_PackBCn_DecodeValueSign(byte *cs, int *rval)
{
	int i;
	cs=BGBBTJ_PackBCn_DecodeValue(cs, &i);
	*rval=(i&1)?(-((i+1)>>1)):(i>>1);
	return(cs);
}

byte *BGBBTJ_PackBCn_DecodeValueScan(byte *cs, int *rval, int scan)
{
	int op, i, j;
	
	op=*cs;

	if((op>=248) && (op<=250))
	{
		cs++;
		cs=BGBBTJ_PackBCn_DecodeValueSign(cs, &i);
		*rval=((op-248)+1)*scan+i;
		return(cs);
	}

	if(op==251)
	{
		cs++;
		cs=BGBBTJ_PackBCn_DecodeValue(cs, &i);
		cs=BGBBTJ_PackBCn_DecodeValueSign(cs, &j);
		*rval=i*scan+j;
		return(cs);
	}
	
	return(BGBBTJ_PackBCn_DecodeValue(cs, rval));
}

byte *BGBBTJ_PackBCn_DecodeRunScan(
	byte *cs, int *ridx, int *rlen, int scan)
{
	int op, i, j;
	
	op=*cs++;
	if(!op)
		{ *ridx=0; *rlen=0; return(cs); }
	if(op<128)
		{ *ridx=op; *rlen=1; return(cs); }
	if(op<192)
	{
		*ridx=((op-128)<<8)|(*cs++);
		*rlen=1;
		return(cs);
	}
	if(op<224)
	{
		i=((op-192)<<8)|(*cs++);
		i=(i<<8)|(*cs++);
		*ridx=i;
		*rlen=1;
		return(cs);
	}
	if(op<239)
	{
		cs=BGBBTJ_PackBCn_DecodeValueScan(cs, ridx, scan);
		*rlen=(op-224)+2;
		return(cs);
	}
	if(op==239)
	{
		cs=BGBBTJ_PackBCn_DecodeValue(cs, rlen);
		cs=BGBBTJ_PackBCn_DecodeValueScan(cs, ridx, scan);
		return(cs);
	}
	if(op==240)
	{
		i=*cs++;
		i=(i<<8)|(*cs++);
		i=(i<<8)|(*cs++);
		*ridx=i;
		*rlen=1;
		return(cs);
	}
	if(op==241)
	{
		i=*cs++;			i=(i<<8)|(*cs++);
		i=(i<<8)|(*cs++);	i=(i<<8)|(*cs++);
		*ridx=i;
		*rlen=1;
		return(cs);
	}

	if(op<247)
	{
		*ridx=0;
		*rlen=(op-242)+2;
		return(cs);
	}
	if(op==247)
	{
		*ridx=0;
		cs=BGBBTJ_PackBCn_DecodeValue(cs, rlen);
		return(cs);
	}

	if((op>=248) && (op<=250))
	{
		cs=BGBBTJ_PackBCn_DecodeValueSign(cs, &i);
		*ridx=((op-248)+1)*scan+i;
		*rlen=1;
		return(cs);
	}

	if(op==251)
	{
		cs=BGBBTJ_PackBCn_DecodeValue(cs, &i);
		cs=BGBBTJ_PackBCn_DecodeValueSign(cs, &j);
		*ridx=i*scan+j;
		*rlen=1;
		return(cs);
	}

	*ridx=0; *rlen=0;
	return(cs);
}

byte *BGBBTJ_PackBCn_DecodeBlockArray(byte *ibuf, byte *blks,
	int count, int stride)
{
	byte *cs, *ct, *cte, *cs1, *ct1, *cs1e;
	byte *cs0, *cs0a, *cs0b, *cs0c, *cs0d;
	int bi, bl;
	int i, j;

	cs0d=NULL; cs0c=NULL; cs0b=NULL; cs0a=NULL; cs0=NULL;
	cs=ibuf; ct=blks; cte=ct+count*stride;
	while(ct<cte)
	{
		cs0d=cs0c; cs0c=cs0b; cs0b=cs0a; cs0a=cs0; cs0=cs;
		cs=BGBBTJ_PackBCn_DecodeRun(cs, &bi, &bl);
		if(bi==0)
		{
			if(bl>1)
			{
#if defined(X86) || defined(X86_64)
				cs1e=cs+bl*8;
				while(cs<cs1e)
				{
					*(u64 *)ct=*(u64 *)(cs);
					cs+=8; ct+=stride;
				}
#else
				for(i=0; i<bl; i++)
					{ memcpy(ct+i*stride, cs+i*8, 8); }
				cs+=bl*8;
				ct+=bl*stride;
#endif
				continue;
			}
#if defined(X86) || defined(X86_64)
			*(u64 *)ct=*(u64 *)cs;
#else
			memcpy(ct, cs, 8);
#endif
			ct+=stride; cs+=8;
			continue;
		}

#if defined(X86) || defined(X86_64)
		cs1=ct-bi*stride;
		cs1e=cs1+bl*stride;
		while(cs1<cs1e)
		{
			*(u64 *)ct=*(u64 *)(cs1);
			cs1+=stride; ct+=stride;
		}
#else
		for(i=0; i<bl; i++)
			{ memcpy(ct+i*stride, ct+(i-bi)*stride, 8); }
		ct+=bl*stride;
#endif
	}
	return(cs);
}

BGBBTJ_API int BGBBTJ_PackBCn_DecodeBlocksDXT1(
	byte *ibuf, byte *blks, int count)
{
	byte *cs;
	cs=BGBBTJ_PackBCn_DecodeBlockArray(ibuf, blks, count, 8);
	return(cs-ibuf);
}

BGBBTJ_API int BGBBTJ_PackBCn_DecodeBlocksDXT5(
	byte *ibuf, byte *blks, int count)
{
	byte *cs;
	cs=BGBBTJ_PackBCn_DecodeBlockArray(ibuf, blks, count, 16);
	cs=BGBBTJ_PackBCn_DecodeBlockArray(cs, blks+8, count, 16);
	return(cs-ibuf);
}

/*
BlockPack with literal DXTn blocks stored out-of-band.

The main buffer will then contain commands, with the 'lbuf' holding any literal blocks.
*/

byte *BGBBTJ_PackBCn_EncodeBlockArray2(
	byte *obuf, byte **rlbuf, byte *blks, byte *iblks,
	int count, int stride, int scan)
{
	byte *cs, *css, *cse, *ct, *ctl, *cs1;
	int *blkchn, *blkhash;
	u64 li;
	int i, bi, bl, bi1, bl1;
	
	blkchn=malloc(2*count*sizeof(int));
	blkhash=malloc(65536*sizeof(int));
	memset(blkchn, 0, 2*count*sizeof(int));
	memset(blkhash, 0, 65536*sizeof(int));

	if(iblks)
	{
		css=iblks; cs=iblks; cse=iblks+count*stride;
		BGBBTJ_PackBCn_UpdateBlockSpan(css, cs, cse, stride,
			blkchn, blkhash, count);

		ct=obuf; ctl=*rlbuf;
		cs=blks; cse=blks+count*stride;
	}else
	{
		ct=obuf; ctl=*rlbuf;
		cs=blks; css=blks; cse=blks+count*stride;
	}

//	ct=obuf; ctl=*rlbuf;
//	cs=blks; cse=blks+count*stride;
	while(cs<cse)
	{
		BGBBTJ_PackBCn_LookupBlockSpan(css, cs, cse, stride,
			blkchn, blkhash,
			&bi, &bl, 65536);

#if 1
		if(bi==0)
		{
			bl=1; cs1=cs+stride;
//			while((cs1<cse) && (bl<6))
//			while(cs1<cse)
			while((cs1<cse) && (bl<256))
			{
				BGBBTJ_PackBCn_LookupBlockSpan(
					css, cs1, cse, stride,
					blkchn, blkhash,
					&bi1, &bl1, 1);
				if(bi1>0)break;
				bl++;
				cs1=cs+bl*stride;
			}
		}
#endif

		BGBBTJ_PackBCn_UpdateBlockSpan(css, cs, cse, stride,
			blkchn, blkhash, bl);

		if(bi<1)
		{
			if(bl<2)
			{
				*ct++=0;
				li=BGBBTJ_PackBCn_GetBlockValue(cs);
				ctl=BGBBTJ_PackBCn_EmitBlockValue(ctl, li);
				cs+=stride;
				continue;
			}
			ct=BGBBTJ_PackBCn_EmitRunScan(ct, 0, bl, scan);
			for(i=0; i<bl; i++)
			{
				li=BGBBTJ_PackBCn_GetBlockValue(cs);
				ctl=BGBBTJ_PackBCn_EmitBlockValue(ctl, li);
				cs+=stride;
			}
			continue;
		}
		ct=BGBBTJ_PackBCn_EmitRunScan(ct, bi, bl, scan);
		cs+=bl*stride;
	}
	
	free(blkchn);
	free(blkhash);
	
	*rlbuf=ctl;
	return(ct);
}

BGBBTJ_API int BGBBTJ_PackBCn_EncodeBlocks2DXT1(
	byte *obuf, byte *lbuf, byte *blks, int count, int scan, int *rlsz)
{
	byte *ct, *ctl;
	
	ct=obuf; ctl=lbuf;
	ct=BGBBTJ_PackBCn_EncodeBlockArray2(
		ct, &ctl, blks, NULL, count, 8, scan);
	*rlsz=ctl-lbuf;
	return(ct-obuf);
}

BGBBTJ_API int BGBBTJ_PackBCn_EncodeBlocks2DXT5(
	byte *obuf, byte *lbuf, byte *blks, int count, int scan, int *rlsz)
{
	byte *ct, *ctl;
	ct=obuf; ctl=lbuf;
	ct=BGBBTJ_PackBCn_EncodeBlockArray2(
		ct, &ctl, blks, NULL, count, 16, scan);
	ct=BGBBTJ_PackBCn_EncodeBlockArray2(
		ct, &ctl, blks+8, NULL, count, 16, scan);
	*rlsz=ctl-lbuf;
	return(ct-obuf);
}

BGBBTJ_API int BGBBTJ_PackBCn_EncodePatchBlocks2DXT1(
	byte *obuf, byte *lbuf, byte *blks, int count, int scan, int *rlsz)
{
	byte *ct, *ctl;
	
	ct=obuf; ctl=lbuf;
	ct=BGBBTJ_PackBCn_EncodeBlockArray2(
		ct, &ctl, blks+count*8, blks, count, 8, scan);
	*rlsz=ctl-lbuf;
	return(ct-obuf);
}

BGBBTJ_API int BGBBTJ_PackBCn_EncodePatchBlocks2DXT5(
	byte *obuf, byte *lbuf, byte *blks, int count, int scan, int *rlsz)
{
	byte *ct, *ctl;
	ct=obuf; ctl=lbuf;
	ct=BGBBTJ_PackBCn_EncodeBlockArray2(
		ct, &ctl, blks+count*16, blks, count, 16, scan);
	ct=BGBBTJ_PackBCn_EncodeBlockArray2(
		ct, &ctl, blks+count*16+8, blks+8, count, 16, scan);
	*rlsz=ctl-lbuf;
	return(ct-obuf);
}


byte *BGBBTJ_PackBCn_DecodeBlockArray2(byte *ibuf,
	byte **rlbuf, byte *blks,
	int count, int stride, int scan)
{
	byte *cs, *csl, *ct, *cte, *cs1, *ct1, *cs1e;
	byte *cs0, *cs0a, *cs0b, *cs0c, *cs0d;
	int bi, bl;
	int i, j;

	cs0d=NULL; cs0c=NULL; cs0b=NULL; cs0a=NULL; cs0=NULL;
	cs=ibuf; csl=*rlbuf; ct=blks; cte=ct+count*stride;
	while(ct<cte)
	{
		cs0d=cs0c; cs0c=cs0b; cs0b=cs0a; cs0a=cs0; cs0=cs;
		cs=BGBBTJ_PackBCn_DecodeRunScan(cs, &bi, &bl, scan);
		
		if((ct+(bl*stride))>cte)
			break;
		
		if(bi==0)
		{
			if(bl>1)
			{
#if defined(X86) || defined(X86_64)
				cs1e=csl+bl*8;
				while(csl<cs1e)
				{
					*(u64 *)ct=*(u64 *)(csl);
					csl+=8; ct+=stride;
				}
#else
				for(i=0; i<bl; i++)
					{ memcpy(ct+i*stride, csl+i*8, 8); }
				csl+=bl*8;
				ct+=bl*stride;
#endif
				continue;
			}
#if defined(X86) || defined(X86_64)
			*(u64 *)ct=*(u64 *)csl;
#else
			memcpy(ct, csl, 8);
#endif
			ct+=stride; csl+=8;
			continue;
		}

#if defined(X86) || defined(X86_64)
		cs1=ct-bi*stride;
		cs1e=cs1+bl*stride;
		while(cs1<cs1e)
		{
			*(u64 *)ct=*(u64 *)(cs1);
			cs1+=stride; ct+=stride;
		}
#else
		for(i=0; i<bl; i++)
			{ memcpy(ct+i*stride, ct+(i-bi)*stride, 8); }
		ct+=bl*stride;
#endif
	}
	
	*rlbuf=csl;
	return(cs);
}

BGBBTJ_API int BGBBTJ_PackBCn_DecodeBlocks2DXT1(
	byte *ibuf, byte *lbuf, byte *blks, int count, int scan)
{
	byte *cs, *csl;
	
	csl=lbuf;
	cs=BGBBTJ_PackBCn_DecodeBlockArray2(ibuf, &csl, blks, count, 8, scan);
	return(cs-ibuf);
}

BGBBTJ_API int BGBBTJ_PackBCn_DecodeBlocks2DXT5(
	byte *ibuf, byte *lbuf, byte *blks, int count, int scan)
{
	byte *cs, *csl;

	cs=ibuf; csl=lbuf;
	cs=BGBBTJ_PackBCn_DecodeBlockArray2(cs, &csl, blks, count, 16, scan);
	cs=BGBBTJ_PackBCn_DecodeBlockArray2(cs, &csl, blks+8, count, 16, scan);
	return(cs-ibuf);
}


byte *BGBBTJ_PackBCn_StatDecodeRun(byte *cs,
	int *ridx, int *rlen, int *stats, int scan)
{
	int op, i, j;
	
	op=*cs++;
	stats[op]++;
	
	if(!op)
		{ *ridx=0; *rlen=0; return(cs); }
	if(op<128)
		{ *ridx=op; *rlen=1; return(cs); }
	if(op<192)
	{
		stats[512+(*cs)]++;
		*ridx=((op-128)<<8)|(*cs++);
		*rlen=1;
		return(cs);
	}
	if(op<224)
	{
		stats[512+cs[0]]++;
		stats[512+cs[1]]++;

		i=((op-192)<<8)|(*cs++);
		i=(i<<8)|(*cs++);
		*ridx=i;
		*rlen=1;
		return(cs);
	}
	if(op<239)
	{
		stats[256+(*cs)]++;
		cs=BGBBTJ_PackBCn_DecodeValueScan(cs, ridx, scan);
		*rlen=(op-224)+2;
		return(cs);
	}
	if(op==239)
	{
		stats[256+(*cs)]++;
		cs=BGBBTJ_PackBCn_DecodeValue(cs, rlen);
		stats[256+(*cs)]++;
		cs=BGBBTJ_PackBCn_DecodeValueScan(cs, ridx, scan);
		return(cs);
	}
	if(op==240)
	{
		i=*cs++;
		i=(i<<8)|(*cs++);
		i=(i<<8)|(*cs++);
		*ridx=i;
		*rlen=1;
		return(cs);
	}
	if(op==241)
	{
		i=*cs++;			i=(i<<8)|(*cs++);
		i=(i<<8)|(*cs++);	i=(i<<8)|(*cs++);
		*ridx=i;
		*rlen=1;
		return(cs);
	}

	if(op<247)
	{
		*ridx=0;
		*rlen=(op-242)+2;
		return(cs);
	}
	if(op==247)
	{
		stats[256+(*cs)]++;
		*ridx=0;
		cs=BGBBTJ_PackBCn_DecodeValue(cs, rlen);
		return(cs);
	}

	if((op>=248) && (op<=250))
	{
		stats[256+(*cs)]++;
		cs=BGBBTJ_PackBCn_DecodeValueSign(cs, &i);
		*ridx=((op-248)+1)*scan+i;
		*rlen=1;
		return(cs);
	}

	if(op==251)
	{
		stats[256+(*cs)]++;
		cs=BGBBTJ_PackBCn_DecodeValue(cs, &i);
		cs=BGBBTJ_PackBCn_DecodeValueSign(cs, &j);
		*ridx=i*scan+j;
		*rlen=1;
		return(cs);
	}

	*ridx=0; *rlen=0;
	return(cs);
}

byte *BGBBTJ_PackBCn_StatBlockArray2(
	byte *ibuf, int *stats, int count, int scan)
{
	byte *cs, *cs1, *ct1, *cs1e;
	byte *cs0, *cs0a, *cs0b, *cs0c, *cs0d;
	int bi, bl;
	int i, j, n;

	cs0d=NULL; cs0c=NULL; cs0b=NULL; cs0a=NULL; cs0=NULL;
	cs=ibuf; n=0;
	while(n<count)
	{
		cs0d=cs0c; cs0c=cs0b; cs0b=cs0a; cs0a=cs0; cs0=cs;
//		stats[*cs]++;
		cs=BGBBTJ_PackBCn_StatDecodeRun(cs, &bi, &bl, stats, scan);
		if(bi==0)
		{
			if(bl>1)
			{
				n+=bl;
				continue;
			}
			n++;
			continue;
		}

		n+=bl;
	}
	return(cs);
}
