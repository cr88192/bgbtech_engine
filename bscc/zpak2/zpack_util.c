#include <zpack.h>

u16 ZPACK_GetU16(byte *s)
{
	return(s[0]|(s[1]<<8));
}

u32 ZPACK_GetU32(byte *s)
{
	return(s[0]|(s[1]<<8)|(s[2]<<16)|(s[3]<<24));
}

u64 ZPACK_GetU64(byte *s)
{
	u64 i;
	i=s[0];		i|=s[1]<<8;	i|=s[2]<<16;	i|=s[3]<<24;
	i|=(u64)s[4]<<32;		i|=(u64)s[5]<<40;
	i|=(u64)s[6]<<48;		i|=(u64)s[7]<<56;
	return(i);
}

void ZPACK_SetU16(byte *s, u16 i)
{
	s[0]=i&0xFF;	s[1]=(i>>8)&0xFF;
}

void ZPACK_SetU32(byte *s, u32 i)
{
	s[0]=i&0xFF;		s[1]=(i>>8)&0xFF;
	s[2]=(i>>16)&0xFF;	s[3]=(i>>24)&0xFF;
}

void ZPACK_SetU64(byte *s, u64 i)
{
	s[0]=i&0xFF;		s[1]=(i>>8)&0xFF;
	s[2]=(i>>16)&0xFF;	s[3]=(i>>24)&0xFF;
	s[4]=(i>>32)&0xFF;	s[5]=(i>>40)&0xFF;
	s[6]=(i>>48)&0xFF;	s[7]=(i>>56)&0xFF;
}

u32 ZPACK_CRC32(void *buf, int sz, u32 lcrc)
{
	static u32 crctab[256];
	static int init=0;

	byte *s;
	u32 c;
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


int ZPACK_ReadU16(FILE *fd)
{
	int i;
	i=fgetc(fd);
	i|=fgetc(fd)<<8;
	return(i);
}

u32 ZPACK_ReadU32(FILE *fd)
{
	u32 i;
	i=fgetc(fd);
	i|=fgetc(fd)<<8;
	i|=fgetc(fd)<<16;
	i|=fgetc(fd)<<24;
	return(i);
}

void ZPACK_WriteU16(FILE *fd, int v)
{
	fputc(v&0xFF, fd);
	fputc((v>>8)&0xFF, fd);
}

void ZPACK_WriteU32(FILE *fd, int v)
{
	fputc(v&0xFF, fd);
	fputc((v>>8)&0xFF, fd);
	fputc((v>>16)&0xFF, fd);
	fputc((v>>24)&0xFF, fd);
}

void ZPACK_WritePad(FILE *fd, int sz)
{
	static byte buf[1024];
	static int init=0;
	int i;

	if(!init) { memset(buf, 0, 1024); init=1; }
	i=sz;
	while(i>=1024) { fwrite(buf, 1, 1024, fd); i-=1024; }
	fwrite(buf, 1, i, fd);
}

void ZPACK_WriteZeroSpan(FILE *fd, int offs, int sz)
{
	fseek(fd, offs, 0);
	ZPACK_WritePad(fd, sz);
}

void ZPACK_WritePadFile(FILE *fd, int offs)
{
	int i;
	fseek(fd, 0, 2);
	i=ftell(fd);

	if(i<offs)ZPACK_WritePad(fd, offs-i);
}

int ZPACK_CharEq(char i, char j)
{
	if((i>='A') && (i<='Z'))i=i-'A'+'a';
	if((j>='A') && (j<='Z'))j=j-'A'+'a';
	return(i==j);
}

ZPACK_API int ZPACK_Match(char *s0, char *s1)
{
	int i;

	while(*s0 && *s1)
	{
		if(*s0=='*')
		{
			if(!*s1)return(1);
			s0++;
			while(*s1)
			{
				i=ZPACK_Match(s0, s1);
				if(i)return(i);
				s1++;
			}
			return(0);
		}
		if(*s0=='?') { s0++; s1++; continue; }
//		if(*s0!=*s1)break;
		if(!ZPACK_CharEq(*s0, *s1))break;
		s0++; s1++;
	}
	if(*s0 || *s1)return(0);
	return(1);
}

ZPACK_API int ZPACK_Match2(char *s0, char *s1)
{
	int i;

	while(*s0 && *s1)
	{
		if(*s0=='\\')
		{
			s0++;
//			if(*s0!=*s1)break;
			if(!ZPACK_CharEq(*s0, *s1))break;
			s0++; s1++;
			continue;
		}

		if((s0[0]=='*') && (s0[1]=='*'))
		{
			if(!*s1)return(1);
			s0+=2;
			while(*s1)
			{
				i=ZPACK_Match(s0, s1);
				if(i)return(i);
				s1++;
			}
			return((*s0)?0:1);
		}
		if(*s0=='*')
		{
			if(!*s1)return(1);
			s0++;
			while(*s1)
			{
				if(*s1=='/')
				{
					if(*s0=='/')
					{
						while(*s0=='/')s0++;
						while(*s1=='/')s1++;
						i=ZPACK_Match(s0, s1);
						return(i);
					}
					return(0);
				}

				i=ZPACK_Match(s0, s1);
				if(i)return(i);
				s1++;
			}
			return((*s0)?0:1);
		}

		if(*s0=='/' && *s1=='/')
		{
			while(*s0=='/')s0++;
			while(*s1=='/')s1++;
			continue;
		}

		if(*s0=='?') { s0++; s1++; continue; }
//		if(*s0!=*s1)break;
		if(!ZPACK_CharEq(*s0, *s1))break;
		s0++; s1++;
	}
	if(*s0 || *s1)return(0);
	return(1);
}

ZPACK_API int ZPACK_MultiMatch(char **ms, char *s)
{
	int i, j;

	if(!ms)return(1);
	if(!ms[0])return(1);

	for(i=0; ms[i]; i++)
	{
		j=ZPACK_Match(ms[i], s);
		if(j)return(j);
	}
	return(0);
}

int ZPACK_HashName(char *name)
{
	char *s;
	int i, hi;

	hi=0; s=name;
	while(*s)
	{
		i=*s++;
		if((i>='A') && (i<='Z'))i=i-'A'+'a';
		hi=(hi>>12)^(hi<<3)^i;
	}
	hi&=0xFFF;
	return(hi);
}

int zpack_stricmp(char *s0, char *s1)
{
	int i, j;

	while(*s0 && *s1)
	{
		i=*s0++; j=*s1++;
		if((i>='A') && (i<='Z'))i=i-'A'+'a';
		if((j>='A') && (j<='Z'))j=j-'A'+'a';
		if(i!=j)return((i>j)?1:-1);
	}
	if(*s0==*s1)return(0);
	return(*s0?1:-1);
}

/*
	The following functions have been adapted from Zlib
	(C) 1995-2006 by Mark Adler and Jean-loup Gailly
 */

#define GF2_DIM 32

static u32 gf2_matrix_times(u32 *mat, u32 vec)
{
	u32 sum;

	sum=0;
	while(vec)
	{
		if (vec&1)sum^=*mat;
		vec>>=1;
		mat++;
	}
	return(sum);
}

static void gf2_matrix_square(u32 *square, u32 *mat)
{
	int n;
	for (n=0; n<GF2_DIM; n++)
		square[n]=gf2_matrix_times(mat, mat[n]);
}

u32 ZPACK_CombineCRC32(u32 crc1, u32 crc2, u32 len2)
{
	u32 even[GF2_DIM];    /* even-power-of-two zeros operator */
	u32 odd[GF2_DIM];     /* odd-power-of-two zeros operator */
	u32 row;
	int n;

	/* degenerate case */
	if (len2==0)return(crc1);

	/* put operator for one zero bit in odd */
	odd[0] = 0xedb88320L;           /* CRC-32 polynomial */
	row = 1;
	for (n = 1; n < GF2_DIM; n++) {
		odd[n] = row;
		row <<= 1;
	}

	/* put operator for two zero bits in even */
	gf2_matrix_square(even, odd);

	/* put operator for four zero bits in odd */
	gf2_matrix_square(odd, even);

	/* apply len2 zeros to crc1 (first square will put the operator for
	one zero byte, eight zero bits, in even) */
	do {
		/* apply zeros operator for this bit of len2 */
		gf2_matrix_square(even, odd);
		if(len2&1)crc1=gf2_matrix_times(even, crc1);
		len2>>=1;

		/* if no more bits set, then done */
		if(len2==0)break;

		/* another iteration of the loop with odd and even swapped */
		gf2_matrix_square(odd, even);
		if(len2&1)crc1=gf2_matrix_times(odd, crc1);
		len2>>=1;

		/* if no more bits set, then done */
	} while(len2!=0);

	/* return combined crc */
	crc1^=crc2;
	return(crc1);
}

#if 0
//lame fallback in-case for whatever reason I can't use the above
u32 ZPACK_CombineCRC32(u32 crc1, u32 crc2, u32 len2)
{
	return(crc1^crc2);
}
#endif
