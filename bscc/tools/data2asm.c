#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef unsigned char byte;
typedef unsigned short u16;

u16 CRC16_CCIT(byte *data, int sz)
{
	byte *cs, *ce;
	u16 crc;

	cs=data; ce=cs+sz; crc=0;
	while(cs<ce)
	{
		crc  = ((crc>>8)&0xFF) | (crc<<8);
		crc ^= (*cs++);
		crc ^= (crc&0xFF)>>4;
		crc ^= crc<<12;
		crc ^= (crc&0xFF)<<5;
	}

	return(crc);
}


u16 Hash16_BGB(byte *data, int sz)
{
	byte *cs, *ce;
	u16 hash;

	cs=data; ce=cs+sz; hash=0;
	while(cs<ce)
		hash=(hash*31)+(*cs++);
	return(hash);
}

int usage(int argc, char **argv)
{
}

int main(int argc, char *argv[])
{
	char fccb[8];
	char *infn, *outfn, *fcc;
	FILE *infd, *outfd;
	byte *buf, *buf1;
	int i, j, sz;

	infn=NULL; outfn=NULL; fcc="RAW ";
	for(i=1; i<argc; i++)
	{
		if(argv[i][0]=='-')
		{
			if(argv[i][1]=='T')
			{
				strcpy(fccb, "    ");
				strcpy(fccb, argv[i]+2);
				fcc=fccb;
			}

			continue;
		}

		if(!outfn) { outfn=argv[i]; continue; }
		if(!infn) { infn=argv[i]; continue; }

		usage(argc, argv);
		return(EXIT_FAILURE);
	}

	if(!infn || !outfn)
	{
		usage(argc, argv);
		return(EXIT_FAILURE);
	}

	infd=fopen(infn, "rb");
	fseek(infd, 0, 2);
	sz=ftell(infd);
	fseek(infd, 0, 0);

	buf=malloc(sz+256);
	memset(buf, 0, sz+256);

	buf1=buf+60+32;
	fread(buf1, 1, sz, infd);
	fclose(infd);

	//chunk header
	buf1=buf+60;
	buf1[ 0]='\x89';
	buf1[ 1]='\x0A';
	buf1[ 2]='\x0D';
	buf1[ 3]='\x1A';
	buf1[ 4]='C';
	buf1[ 5]='H';
	buf1[ 6]='N';
	buf1[ 7]='K';
	buf1[ 8]=fcc[0];
	buf1[ 9]=fcc[1];
	buf1[10]=fcc[2];
	buf1[11]=fcc[3];
	buf1[12]=32;	//head size
	buf1[13]=0;	//head size
	buf1[14]=0;	//head checksum
	buf1[15]=0;	//head chechsum

	i=sz+32; j=0;
	buf1[16]=(i    )&0xFF;
	buf1[17]=(i>> 8)&0xFF;
	buf1[18]=(i>>16)&0xFF;
	buf1[19]=(i>>24)&0xFF;

	buf1[20]=(j    )&0xFF;
	buf1[21]=(j>> 8)&0xFF;
	buf1[22]=(j>>16)&0xFF;
	buf1[23]=(j>>24)&0xFF;

	i=CRC16_CCIT(buf1, 32);
	buf1[14]=(i   )&0xFF;
	buf1[15]=(i>>8)&0xFF;

	//COFF header

	buf1=buf;
	buf1[ 0]='\x4C';	//i386 magic
	buf1[ 1]='\x01';	//i386 magic
	buf1[ 2]='\x01';	//sections
	buf1[ 3]='\x00';	//sections
	buf1[ 4]=0;		//timestamp
	buf1[ 5]=0;		//timestamp
	buf1[ 6]=0;		//timestamp
	buf1[ 7]=0;		//timestamp
	buf1[ 8]=0;		//symbol table
	buf1[ 9]=0;		//symbol table
	buf1[10]=0;		//symbol table
	buf1[11]=0;		//symbol table
	buf1[12]=0;		//symbol table size
	buf1[13]=0;		//symbol table size
	buf1[14]=0;		//symbol table size
	buf1[15]=0;		//symbol table size	
	buf1[16]=0;		//opt header size
	buf1[17]=0;		//opt header size
	buf1[18]=0;		//characteristics
	buf1[19]=0;		//characteristics

	//coff sections
	buf1[20]='.';		//section name
	buf1[21]='c';
	buf1[22]='h';
	buf1[23]='u';
	buf1[24]='n';
	buf1[25]='k';
	buf1[26]='s';
	buf1[27]='\x00';

	buf1[28]=0;
	buf1[29]=0;
	buf1[30]=0;
	buf1[31]=0;

	buf1[32]=0;
	buf1[33]=0;
	buf1[34]=0;
	buf1[35]=0;

	i=sz+32; j=60;
	buf1[36]=(i    )&0xFF;
	buf1[37]=(i>> 8)&0xFF;
	buf1[38]=(i>>16)&0xFF;
	buf1[39]=(i>>24)&0xFF;

	buf1[40]=(j    )&0xFF;
	buf1[41]=(j>> 8)&0xFF;
	buf1[42]=(j>>16)&0xFF;
	buf1[43]=(j>>24)&0xFF;

	buf1[44]=0;
	buf1[45]=0;
	buf1[46]=0;
	buf1[47]=0;

	buf1[48]=0;
	buf1[49]=0;
	buf1[50]=0;
	buf1[51]=0;

	buf1[52]=0;
	buf1[53]=0;

	buf1[54]=0;
	buf1[55]=0;

	buf1[56]=0x40;
	buf1[57]=0x00;
	buf1[58]=0x50;
	buf1[59]=0xE0;

	i=60+32+sz;
	outfd=fopen(outfn, "wb");
	fwrite(buf, 1, i, outfd);
	fclose(outfd);

//	return(EXIT_FAILURE);
	return(EXIT_SUCCESS);
}
