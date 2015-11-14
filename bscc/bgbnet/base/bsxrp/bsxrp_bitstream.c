#include <bgbnet.h>

void BSXRP_SetupBitstreamRead(BSXRP_StreamContext *ctx,
	byte *css, byte *cse)
{
	int i;

	ctx->ibuf=css;
	ctx->cs=css;
	ctx->cse=cse;
	for(i=0; i<4; i++)
	{
		ctx->bit_win>>=8;
		ctx->bit_win|=BSXRP_ReadCodedByte(ctx)<<24;
	}
	ctx->bit_pos=0;
}

void BSXRP_SetupBitstreamWrite(BSXRP_StreamContext *ctx,
	byte *cts, byte *cte)
{
	ctx->obuf=cts;
	ctx->ct=cts;
	ctx->cte=cte;
	ctx->bit_win=0;
	ctx->bit_pos=0;
}

byte *BSXRP_FinishBitstreamRead(BSXRP_StreamContext *ctx)
{
	byte *cs;
	return(ctx->cs);
}

byte *BSXRP_FinishBitstreamWrite(BSXRP_StreamContext *ctx)
{
	while(ctx->bit_pos>0)
	{
		BSXRP_EmitCodedByte(ctx, ctx->bit_win&0xFF);
		ctx->bit_win>>=8;
		ctx->bit_pos-=8;
	}
	return(ctx->ct);
}

void BSXRP_EmitCodedByte(BSXRP_StreamContext *ctx, int i)
{
	*ctx->ct++=i;
	if((ctx->ct>=(ctx->obuf+3)) &&
		(ctx->ct[-3]==0x7F) &&
		(ctx->ct[-2]==0xFF) &&
		(ctx->ct[-1]==0xFE))
	{
		*ctx->ct++=0x00;
	}
}

int BSXRP_ReadCodedByte(BSXRP_StreamContext *ctx)
{
	if((ctx->cs>=(ctx->ibuf+3)) &&
		(ctx->cs[-3]==0x7F) &&
		(ctx->cs[-2]==0xFF) &&
		(ctx->cs[-1]==0xFE))
	{
		if(ctx->cs[0]==0x00)
		{
			ctx->cs++;	//escape byte
			return(*ctx->cs++);
		}
		
		//errm... we just read into an escape code
		return(0);
	}

	return(*ctx->cs++);
//	*ctx->ct++=i;
}

void BSXRP_WriteBit(BSXRP_StreamContext *ctx, int i)
{
	ctx->bit_win|=i<<ctx->bit_pos;
	ctx->bit_pos++;
	if(ctx->bit_pos>=8)
	{
		BSXRP_EmitCodedByte(ctx, ctx->bit_win&0xFF);
		ctx->bit_win>>=8;
		ctx->bit_pos-=8;
	}
}

void BSXRP_Write2Bits(BSXRP_StreamContext *ctx, int i)
{
	ctx->bit_win|=i<<ctx->bit_pos;
	ctx->bit_pos+=2;
	if(ctx->bit_pos>=8)
	{
		BSXRP_EmitCodedByte(ctx, ctx->bit_win&0xFF);
		ctx->bit_win>>=8;
		ctx->bit_pos-=8;
	}
}

void BSXRP_Write3Bits(BSXRP_StreamContext *ctx, int i)
{
	ctx->bit_win|=i<<ctx->bit_pos;
	ctx->bit_pos+=3;
	if(ctx->bit_pos>=8)
	{
		BSXRP_EmitCodedByte(ctx, ctx->bit_win&0xFF);
		ctx->bit_win>>=8;
		ctx->bit_pos-=8;
	}
}

void BSXRP_Write4Bits(BSXRP_StreamContext *ctx, int i)
{
	ctx->bit_win|=i<<ctx->bit_pos;
	ctx->bit_pos+=4;
	if(ctx->bit_pos>=8)
	{
		BSXRP_EmitCodedByte(ctx, ctx->bit_win&0xFF);
		ctx->bit_win>>=8;
		ctx->bit_pos-=8;
	}
}

void BSXRP_Write5Bits(BSXRP_StreamContext *ctx, int i)
{
	ctx->bit_win|=i<<ctx->bit_pos;
	ctx->bit_pos+=5;
	if(ctx->bit_pos>=8)
	{
		BSXRP_EmitCodedByte(ctx, ctx->bit_win&0xFF);
		ctx->bit_win>>=8;
		ctx->bit_pos-=8;
	}
}

void BSXRP_WriteNBits(BSXRP_StreamContext *ctx, int i, int n)
{
	ctx->bit_win|=i<<ctx->bit_pos;
	ctx->bit_pos+=n;
	while(ctx->bit_pos>=8)
	{
		BSXRP_EmitCodedByte(ctx, ctx->bit_win&0xFF);
		ctx->bit_win>>=8;
		ctx->bit_pos-=8;
	}
}

void BSXRP_WriteNBitsL(BSXRP_StreamContext *ctx, u64 val, int n)
{
	u64 l;
	int i;
	
	l=val; i=n;
	while(i>16)
	{
		BSXRP_WriteNBits(ctx, (int)(l&65535), 16);
		l>>=16; i-=16;
	}
	BSXRP_WriteNBits(ctx, (int)l, i);
}

void BSXRP_FlushBits(BSXRP_StreamContext *ctx)
{
	while(ctx->bit_pos>0)
	{
		BSXRP_EmitCodedByte(ctx, ctx->bit_win&0xFF);
		ctx->bit_win>>=8;
		ctx->bit_pos-=8;
	}
	ctx->bit_pos=0;
}

void BSXRP_WriteRice(BSXRP_StreamContext *ctx, int v, int n)
{
	int i, j;
	
	i=v>>n;
	while(i--)BSXRP_WriteBit(ctx, 1);
	BSXRP_WriteBit(ctx, 0);
	BSXRP_WriteNBits(ctx, v&((1<<n)-1), n);
}




int BSXRP_ReadBit(BSXRP_StreamContext *ctx)
{
	int i;

	i=(ctx->bit_win>>ctx->bit_pos)&1;
	ctx->bit_pos++;
	if(ctx->bit_pos>=8)
	{
		ctx->bit_win=(ctx->bit_win>>8)|(BSXRP_ReadCodedByte(ctx)<<24);
		ctx->bit_pos-=8;
	}
	return(i);
}

int BSXRP_Read2Bits(BSXRP_StreamContext *ctx)
{
	int i;
	i=(ctx->bit_win>>ctx->bit_pos)&3;
	ctx->bit_pos+=2;
	if(ctx->bit_pos>=8)
	{
		ctx->bit_win=(ctx->bit_win>>8)|(BSXRP_ReadCodedByte(ctx)<<24);
		ctx->bit_pos-=8;
	}
	return(i);
}

int BSXRP_Read3Bits(BSXRP_StreamContext *ctx)
{
	int i;
	i=(ctx->bit_win>>ctx->bit_pos)&7;
	ctx->bit_pos+=3;
	if(ctx->bit_pos>=8)
	{
		ctx->bit_win=(ctx->bit_win>>8)|(BSXRP_ReadCodedByte(ctx)<<24);
		ctx->bit_pos-=8;
	}
	return(i);
}

int BSXRP_Read4Bits(BSXRP_StreamContext *ctx)
{
	int i;
	i=(ctx->bit_win>>ctx->bit_pos)&15;
	ctx->bit_pos+=4;
	if(ctx->bit_pos>=8)
	{
		ctx->bit_win=(ctx->bit_win>>8)|(BSXRP_ReadCodedByte(ctx)<<24);
		ctx->bit_pos-=8;
	}
	return(i);
}

int BSXRP_Read5Bits(BSXRP_StreamContext *ctx)
{
	int i;
	i=(ctx->bit_win>>ctx->bit_pos)&31;
	ctx->bit_pos+=5;
	if(ctx->bit_pos>=8)
	{
		ctx->bit_win=(ctx->bit_win>>8)|(BSXRP_ReadCodedByte(ctx)<<24);
		ctx->bit_pos-=8;
	}
	return(i);
}

int BSXRP_ReadNBits(BSXRP_StreamContext *ctx, int n)
{
	int i;
	i=(ctx->bit_win>>ctx->bit_pos)&((1<<n)-1);
	ctx->bit_pos+=n;
	while(ctx->bit_pos>=8)
	{
		ctx->bit_win=(ctx->bit_win>>8)|(BSXRP_ReadCodedByte(ctx)<<24);
		ctx->bit_pos-=8;
	}
	return(i);
}

u64 BSXRP_ReadNBitsL(BSXRP_StreamContext *ctx, int n)
{
	u64 v;
	int i, j;
	
	v=0; i=n; j=0;
	while(i>16)
	{
		v|=((u64)BSXRP_ReadNBits(ctx, 16))<<j;
		i-=16; j+=16;
	}
	v|=((u64)BSXRP_ReadNBits(ctx, i))<<j;
	return(v);
}

void BSXRP_SkipNBits(BSXRP_StreamContext *ctx, int n)
{
	ctx->bit_pos+=n;
	while(ctx->bit_pos>=8)
	{
		ctx->bit_win=(ctx->bit_win>>8)|(BSXRP_ReadCodedByte(ctx)<<24);
		ctx->bit_pos-=8;
	}
}

int BSXRP_ReadRice(BSXRP_StreamContext *ctx, int n)
{
	int i, j;
	
	i=0;
	while(BSXRP_ReadBit(ctx))i++;
	j=BSXRP_ReadNBits(ctx, n);
	return((i<<n)|j);
}

int BSXRP_PeekWord(BSXRP_StreamContext *ctx)
{
	int i;
	i=(ctx->bit_win>>ctx->bit_pos)&65535;
	return(i);
}

void BSXRP_AlignByte(BSXRP_StreamContext *ctx)
{
	while(ctx->bit_pos>0)
	{
		ctx->bit_win=(ctx->bit_win>>8)|(BSXRP_ReadCodedByte(ctx)<<24);
		ctx->bit_pos-=8;
	}
	ctx->bit_pos=0;
}

int BSXRP_ReadAlignedByte(BSXRP_StreamContext *ctx)
{
	int i;
	i=ctx->bit_win&0xFF;
	ctx->bit_win=(ctx->bit_win>>8)|(BSXRP_ReadCodedByte(ctx)<<24);
	return(i);
}

int BSXRP_ReadAlignedWord(BSXRP_StreamContext *ctx)
{
	int i;
	i=ctx->bit_win&0xFFFF;
//	ctx->bit_win=(ctx->bit_win>>16)|(ctx->cs[0]<<16)|(ctx->cs[1]<<24);
//	ctx->cs+=2;
	ctx->bit_win=(ctx->bit_win>>8)|(BSXRP_ReadCodedByte(ctx)<<24);
	ctx->bit_win=(ctx->bit_win>>8)|(BSXRP_ReadCodedByte(ctx)<<24);
	return(i);
}
