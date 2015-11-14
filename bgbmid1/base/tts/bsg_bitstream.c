#include <bgbmid.h>

byte *sgwz_cs;		//current pos in bitstream (input)
byte *sgwz_ct;		//current pos in bitstream (output)
byte *sgwz_cse;		//current end of bitstream (input)
u32 sgwz_win;	//bitstream window
int sgwz_pos;		//bitstream offset
int sgwz_isend;		//bitstream has broken (decoder)


int SGWZ_NextByte()
{
	int i;

	sgwz_isend=0;
	i=*sgwz_cs++;
	return(i);
}

int SGWZ_SetupReadStream()
{
	sgwz_win=0;
	sgwz_pos=0;
	sgwz_isend=0;

	sgwz_win=SGWZ_NextByte();
	sgwz_win|=SGWZ_NextByte()<<8;
	sgwz_win|=SGWZ_NextByte()<<16;
	sgwz_win|=SGWZ_NextByte()<<24;
	return(0);
}

int SGWZ_InitStream(byte *buf)
{
	sgwz_win=0;
	sgwz_pos=0;
	sgwz_isend=0;

	sgwz_cs=buf;
	sgwz_win=SGWZ_NextByte();
	sgwz_win|=SGWZ_NextByte()<<8;
	sgwz_win|=SGWZ_NextByte()<<16;
	sgwz_win|=SGWZ_NextByte()<<24;
	return(0);
}

int SGWZ_InitWriteStream(byte *buf)
{
	sgwz_win=0;
	sgwz_pos=0;
	sgwz_isend=0;
	sgwz_ct=buf;
	return(0);
}

void SGWZ_AdjustRead()
{
	int i;

	while(sgwz_pos>=8)
	{
		i=SGWZ_NextByte();

		sgwz_win>>=8;
		sgwz_win|=i<<24;
		sgwz_pos-=8;
	}
}

void SGWZ_AdjustWrite()
{
	int i;
	while(sgwz_pos>=8)
	{
		if(!sgwz_ct)
		{
			sgwz_win>>=8;
			sgwz_pos-=8;
			continue;
		}

		i=sgwz_win&0xFF;
		*sgwz_ct++=i;
		sgwz_win>>=8;
		sgwz_pos-=8;
	}
}

int SGWZ_ReadBit()
{
	int i;

	i=(sgwz_win>>sgwz_pos)&1;
	sgwz_pos++;
	SGWZ_AdjustRead();
	return(i);
}

int SGWZ_ReadNBits(int n)
{
	int i, j;

	i=(sgwz_win>>sgwz_pos)&((1<<n)-1);
	sgwz_pos+=n;
	SGWZ_AdjustRead();
	return(i);
}

void SGWZ_SkipNBits(int n)
{
	int j;

	sgwz_pos+=n;
	SGWZ_AdjustRead();
}

int SGWZ_PeekWord()
{
	int i;
	i=(sgwz_win>>sgwz_pos)&65535;
	return(i);
}

void SGWZ_WriteBit(int i)
{
	sgwz_win|=i<<sgwz_pos;
	sgwz_pos++;
	SGWZ_AdjustWrite();
}

void SGWZ_WriteNBits(int i, int n)
{
	i&=(1<<n)-1;

	sgwz_win|=i<<sgwz_pos;
	sgwz_pos+=n;
	SGWZ_AdjustWrite();
}

void SGWZ_FlushBits()
{
	int i;

	if(!sgwz_ct)
	{
		sgwz_win=0;
		sgwz_pos=0;
		return;
	}

	while(sgwz_pos>0)
	{
		i=sgwz_win&0xFF;
		*sgwz_ct++=i;
		sgwz_win>>=8;
		sgwz_pos-=8;
	}

	sgwz_win=0;
	sgwz_pos=0;
}

int SGWZ_ReadByte()
{
	sgwz_win=0;
	sgwz_pos=0;

	return(SGWZ_NextByte());
}

int SGWZ_ReadShort()
{
	int i;
	i=SGWZ_ReadByte();
	i+=SGWZ_ReadByte()<<8;
	i=(short)i;
	return(i);
}

int SGWZ_ReadInt()
{
	u32 i;
	int j;
	i=SGWZ_ReadByte();
	i+=SGWZ_ReadByte()<<8;
	i+=SGWZ_ReadByte()<<16;
	i+=SGWZ_ReadByte()<<24;

	j=(int)i;
	return(j);
}

void SGWZ_WriteByte(int i)
{
	SGWZ_FlushBits();
	if(!sgwz_ct)return;
	i&=0xFF;
	*sgwz_ct++=i;
}

void SGWZ_WriteShort(int i)
{
	SGWZ_FlushBits();
	if(!sgwz_ct)return;
	*sgwz_ct++=i&0xFF;
	*sgwz_ct++=(i>>8)&0xFF;
}

void SGWZ_WriteInt(int i)
{
	SGWZ_FlushBits();
	if(!sgwz_ct)return;
	*sgwz_ct++=i&0xFF;
	*sgwz_ct++=(i>>8)&0xFF;
	*sgwz_ct++=(i>>16)&0xFF;
	*sgwz_ct++=(i>>24)&0xFF;
}

int SGWZ_ReadUnary()
{
	int i;

	i=0;
	while(1)
	{
		if(!SGWZ_ReadBit())
			break;
		i++;
	}

	return(i);
}

int SGWZ_ReadRice(int n)
{
	int i, j;

	i=SGWZ_ReadUnary();
	j=SGWZ_ReadNBits(n);
	return((i<<n)|j);
}

void SGWZ_WriteUnary(int i)
{
	while(i--)SGWZ_WriteBit(1);
	SGWZ_WriteBit(0);
}

void SGWZ_WriteRice(int v, int n)
{
	int i, j;

	i=v>>n;
	while(i--)SGWZ_WriteBit(1);
	SGWZ_WriteBit(0);
	SGWZ_WriteNBits(v, n);
}

int SGWZ_ReadSRice(int n)
{
	int i;
	i=SGWZ_ReadRice(n);
	i=(i&1)?(-((i>>1)+1)):(i>>1);
	return(i);
}

void SGWZ_WriteSRice(int v, int n)
{
	int i, j;

	if(v<0)SGWZ_WriteRice(((-v)<<1)-1, n);
		else SGWZ_WriteRice(v<<1, n);
}
