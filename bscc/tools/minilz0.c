#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

typedef unsigned char byte;
typedef unsigned short ushort;
typedef unsigned int uint;

int MiniLZ_Decode(byte *ibuf, byte *obuf)
{
	byte *cs, *ct, *cs1;
	int i, j, k, mrk;

	cs=ibuf; ct=obuf; mrk=0;
	while(1)
	{	if(!(mrk&256))mrk=(*cs++)|0xFF00;
		if(mrk&1)
		{	i=*cs++;
			if((i&0xC0)==0x00)
				{ j=(*cs++)|((i<<4)&0x300); i=(i&15)+3; }
			else if((i&0xC0)==0x40)
				{ j=(cs[0]<<8)+cs[1]; cs+=2;
				i=(((i<<2)|(j>>14))&255)+3; j&=16383; }
			else if((i&0xC0)==0x80)
				{ k=i; i=(*cs++)|((k<<4)&0x300); j=k&15; }
			else if((i&0xF0)==0xC0)
				{ k=(i<<24)|(cs[0]<<16)|(cs[1]<<8)|cs[2];
				cs+=3; i=(k>>18)&1023; j=k&262143; }
			else if((i&0xF0)==0xD0)
				{ i=((i&15)<<8)|(*cs++);
				j=(cs[0]<<16)+(cs[1]<<8)+cs[2]; cs+=3; }
			if(!i)break;
			cs1=ct-j; while(i--)*ct++=*cs1++;
			mrk>>=1; continue;
		}
		*ct++=*cs++; mrk>>=1;
	}
	return(ct-obuf);
}

void MiniLZ_Lookup(byte *cs, byte *cse, int chi,
	byte **hash, byte **chain,
	int *rbj, int *rbk)
{
	const int L1=18, D1=256, L2=64, D2=64, L3=258, D3=16;
	byte *s0, *s1, *s1e;
	int i, j, k, bj, bk, sd;
	i=((cs[0]*251+cs[1])*251+cs[2])&0xFFF;
	s0=hash[i]; k=cs-s0; bj=0; bk=0; sd=1024;
	while(s0 && (k<65536) && (sd--))
	{	s1=cs; s1e=cs+1023; if(cse<s1e)s1e=cse;
		while((s1<s1e) && (*s0==*s1)) { *s0++; *s1++; } j=s1-cs;
		if((j>bj) || (!bj)) { if((j>=L1) && (sd>D1))
			{ sd=D1; if((j>=L2) && (sd>D2))
				{ sd=D2; if((j>=L3) && (sd>D3))sd=D3; } }
			bj=j; bk=k; }
		s0=chain[(chi-k)&65535]; k=cs-s0;
	}
	*rbj=bj; *rbk=bk;
}

int MiniLZ_Encode(byte *ibuf, byte *obuf, int sz)
{
	static byte *chain[65536], *hash[4096];
	byte *cs, *ct, *cse, *mps;
	int i, j, k, bj, bk, bj2, bk2, chi, mp;

	for(i=0; i<65536; i++)chain[i]=NULL;
	for(i=0; i<4096; i++)hash[i]=NULL;

	cs=ibuf; cse=ibuf+sz; ct=obuf;
	mps=ct++; *mps=0; mp=0; bj=0; bk=0;
	while(cs<cse)
	{
		chi=cs-ibuf;
		MiniLZ_Lookup(cs+1, cse, chi, hash, chain, &bj2, &bk2);
		if(bj2>(bj+1))bj=0;
		if((bj>=3) && ((bk<16384) || (bj>=8)))
		{
			if((bj<=18) && (bk<1024))
				{ *ct++=(bj-3)|((bk>>4)&0x30); *ct++=bk&0xFF; }
			else if(bk<16)
				{ *ct++=0x80|bk|((bj>>4)&0x30); *ct++=bj&0xFF; }
			else if((bj<=258) && (bk<16384))
			{	j=bj-3;
				*ct++=0x40 | ((j>>2)&63);
				*ct++=((bk>>8)&0x3F)|(j<<6)&0xC0;
				*ct++=bk&0xFF;
			}else {	*ct++=0xC0 | ((bj>>6)&15);
				*ct++=((bk>>16)&0x3)|(bj<<2)&0xFC;
				*ct++=(bk>>8)&0xFF;
				*ct++=bk&0xFF; }
			j=bj; while(j--)
			{	i=((cs[0]*251+cs[1])*251+cs[2])&0xFFF;
				chain[(chi++)&65535]=hash[i];
				hash[i]=cs++; }
			MiniLZ_Lookup(cs, cse, cs-ibuf, hash, chain, &bj, &bk);
			*mps|=1<<(mp++); if(mp>=8) { mps=ct++; *mps=0; mp=0; }
			continue;
		}
		i=((cs[0]*251+cs[1])*251+cs[2])&0xFFF;
		chain[chi&65535]=hash[i]; hash[i]=cs;
		bj=bj2; bk=bk2;
		*ct++=*cs++; mp++; if(mp>=8) { mps=ct++; *mps=0; mp=0; }
	}
	*ct++=0x80; *ct++=0x00; *mps|=1<<(mp++);
	return(ct-obuf);
}


uint MiniLZ_DataAdler32(void *buf, int sz, uint lcrc)
{
	byte *s;
	int i, c, s1, s2;

	s=(byte *)buf;
	s1=lcrc&0xFFFF;
	s2=(lcrc>>16)&0xFFFF;
	for(i=0; i<sz; i++)
	{
		s1=(s1+(s[i]))%65521;
//		s1=(s1+(*s++))%65521;
		s2=(s1+s2)%65521;
	}
	return((s2<<16)+s1);
}

void *MiniLZ_LoadFile(char *name, int *rsz)
{
	void *buf;
	FILE *fd;
	int sz;

	fd=fopen(name, "rb");
	if(!fd)return(NULL);

	fseek(fd, 0, 2);
	sz=ftell(fd);
	fseek(fd, 0, 0);

	buf=malloc(sz+16);
	memset(buf, 0, sz+16);
	fread(buf, 1, sz, fd);

	fclose(fd);

	if(rsz)*rsz=sz;
	return(buf);
}

int main(int argc, char *argv[])
{
	FILE *ifd, *ofd;
	byte *ibuf, *obuf;
	int i, j, k, t, ct, tt, eof;
	unsigned int crc, sz, sz2;
	float ft, ft1;

	if(argc<3)
	{
		printf("usage: %s mode infile outfile\n", argv[0]);
		printf("	mode:\n");
		printf("	-c compress\n");
		printf("	-d decompress\n");
		exit(-1);
	}

	if(!strcmp(argv[1], "-c"))
	{
		ibuf=MiniLZ_LoadFile(argv[2], &sz);
		i=MiniLZ_DataAdler32(ibuf, sz, 0);
		printf("load %d %08X\n", sz, i);

		obuf=malloc(sz*2);
		sz2=MiniLZ_Encode(ibuf, obuf, sz);

		printf("%d -> %d\n", sz, sz2);

		sz=MiniLZ_Decode(obuf, ibuf);
		i=MiniLZ_DataAdler32(ibuf, sz, 0);
		printf("%d -> %d  %08X\n", sz2, sz, i);

		return(0);
	}

	if(!strcmp(argv[1], "-d"))
	{
	}
	return(0);
}
