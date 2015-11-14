#include <bgbmid.h>

extern byte *sgwz_cs;	//current pos in bitstream (input)
extern byte *sgwz_ct;	//current pos in bitstream (output)
extern byte *sgwz_cse;	//current end of bitstream (input)
extern u32 sgwz_win;	//bitstream window
extern int sgwz_pos;		//bitstream offset
extern int sgwz_isend;		//bitstream has broken (decoder)

int SGWZ_PredSample(int s0, int s1, int s2, int s3, int fn)
{
	int i, j, k;
	switch(fn)
	{
	case 0: i=0; break;
	case 1: i=s0; break;
	case 2: i=s0+((s0-s1)/2); break;
	case 3: i=s0+(s0-s1); break;
	case 4: j=s1+(s1-s2); i=s0+(s0-j); break;
	case 5: k=s2-s1; j=s0-s1; i=j+(j-k); i=s0+i; break;
//	case 6: i=s0-(s0-s1); break;
	default: i=-1; break;
	}
	return(i);
}

int SGWZ_FiltEntropy(short *bp, int len, int fn)
{
	int s0, s1, s2, s3, e;
	int i, j, k, l;

	s3=0; s2=0; s1=0; s0=0; e=0;
	for(i=0; i<len; i++)
	{
		j=SGWZ_PredSample(s0, s1, s2, s3, fn);
		k=bp[i]-j; e+=(k<0)?(-k):k;
		s3=s2; s2=s1; s1=s0; s0=bp[i];
	}
	return(e);
}

int SGWZ_BestFilt(short *bp, int len, int *ev)
{
	int bf, be, e;
	int i, j;

	bf=0; be=1<<30;
	for(i=0; i<64; i++)
	{
		j=SGWZ_PredSample(0, 0, 0, 0, i);
		if(j==-1)continue;

		e=SGWZ_FiltEntropy(bp, len, i);
		if(e<be) { bf=i; be=e; }
	}

	if(ev)*ev=be;
	return(bf);
}

int SGWZ_CalcEncValBits(int v, int st)
{
	int i, j;

	v=(v<0)?(((-v)<<1)-1):(v<<1);
	return((v>>st)+st+1);

	if(!st)return(1024);

	i=1; j=v;
	while(j>=(1<<(i*st)))
	{
		i++;
		j-=1<<(i*st);
	}

	return(i+i*st);

//	return((v>>st)+st+1);
}

int SGWZ_CalcEncBits(short *bp, int len, int fn, int st)
{
	int s0, s1, s2, s3, e;
	int i, j, k, l;

	s3=0; s2=0; s1=0; s0=0; e=0;
	for(i=0; i<len; i++)
	{
		j=SGWZ_PredSample(s0, s1, s2, s3, fn);
		k=bp[i]-j; e+=SGWZ_CalcEncValBits(k, st);
		s3=s2; s2=s1; s1=s0; s0=bp[i];
	}
	return(e);
}

int SGWZ_BestEncBits(short *bp, int len, int fn, int *ev)
{
	int bf, be, e;
	int i;

	bf=0; be=1<<30;
	for(i=0; i<16; i++)
	{
		e=SGWZ_CalcEncBits(bp, len, fn, i);
		if(e<be) { bf=i; be=e; }
	}

	if(ev)*ev=be;
	return(bf);
}

int SGWZ_EncodePayload(short *bp, int len, int fn, int st)
{
	int s0, s1, s2, s3;
	int i, j, k, l;

	s3=0; s2=0; s1=0; s0=0;
	for(i=0; i<len; i++)
	{
		j=SGWZ_PredSample(s0, s1, s2, s3, fn);
		k=bp[i]-j; SGWZ_WriteSRice(k, st);
		s3=s2; s2=s1; s1=s0; s0=bp[i];
	}
	return(0);
}

int SGWZ_DecodePayload(short *bp, int len, int fn, int st)
{
	int s0, s1, s2, s3;
	int i, j, k, l;

	s3=0; s2=0; s1=0; s0=0;
	for(i=0; i<len; i++)
	{
		j=SGWZ_PredSample(s0, s1, s2, s3, fn);
		k=j+SGWZ_ReadSRice(st); bp[i]=k;
		s3=s2; s2=s1; s1=s0; s0=bp[i];
	}
	return(0);
}

BMID_API int SGWZ_Encode(short *bp, int rt, int len, byte *obuf)
{
	int fn, st;
	int i, j, k, l;

	printf("SGWZ_Encode: len=%d\n", len);

	fn=SGWZ_BestFilt(bp, len, &k);
	st=SGWZ_BestEncBits(bp, len, fn, &l);

	printf("SGWZ_Encode: FN=%d ST=%d E=%d L=%d\n", fn, st, k, l);

	SGWZ_InitWriteStream(obuf);

	SGWZ_WriteByte('W');
	SGWZ_WriteByte('A');
	SGWZ_WriteByte('Z');
	SGWZ_WriteByte('0');

	SGWZ_WriteInt(len);
	SGWZ_WriteInt(rt);

	SGWZ_WriteByte(16);
	SGWZ_WriteByte(1);
	SGWZ_WriteByte(fn);
	SGWZ_WriteByte(st);

	SGWZ_EncodePayload(bp, len, fn, st);

	SGWZ_FlushBits();

	printf("SGWZ_Encode: OK\n");

	i=sgwz_ct-obuf;
	return(i);
}

BMID_API short *SGWZ_DecodeMono16(byte *buf, int *rrt, int *rlen)
{
	short *bp;
	int fn, st, rt, len;
	int i, j, k, l;

	sgwz_cs=buf;

	i=SGWZ_ReadByte();
	j=SGWZ_ReadByte();
	k=SGWZ_ReadByte();
	l=SGWZ_ReadByte();

	if((i!='W') || (j!='A') || (k!='Z') || (l!='0'))
	{
		printf("Bad WaveZ Magic %c%c%c%c\n", i, j, k, l);
		return(NULL);
	}

	len=SGWZ_ReadInt();
	rt=SGWZ_ReadInt();

	i=SGWZ_ReadByte();
	j=SGWZ_ReadByte();
	fn=SGWZ_ReadByte();
	st=SGWZ_ReadByte();

	if(i!=16)return(NULL);
	if(j!=1)return(NULL);

	SGWZ_SetupReadStream();

	bp=malloc(len*sizeof(short));
	SGWZ_DecodePayload(bp, len, fn, st);

//	printf("Dec %d %d\n", rt, len);

	*rrt=rt;
	*rlen=len;
	return(bp);
}

BMID_API short *SGWZ_Decode_44Mono16(byte *buf, int *rlen)
{
	short *bp, *bp1;
	int rt, len;
	int i, j, k;

	bp=SGWZ_DecodeMono16(buf, &rt, &len);
	if(!bp)return(NULL);

	if(rt!=44100)
	{
		i=len*(44100.0/rt);
		bp1=malloc(i*sizeof(short));

		BGBMID_ScaleSample(bp1, i, bp, len);

		free(bp);
		bp=bp1;
		len=i;
	}

	*rlen=len;
	return(bp);
}

BMID_API short *SGWZ_Decode_16Mono16(byte *buf, int *rlen)
{
	short *bp, *bp1;
	int rt, len;
	int i, j, k;

	bp=SGWZ_DecodeMono16(buf, &rt, &len);
	if(!bp)return(NULL);

	if(rt!=16000)
	{
		i=len*(16000.0/rt);
		bp1=malloc(i*sizeof(short));

		BGBMID_ScaleSample(bp1, i, bp, len);

		free(bp);
		bp=bp1;
		len=i;
	}

	*rlen=len;
	return(bp);
}
