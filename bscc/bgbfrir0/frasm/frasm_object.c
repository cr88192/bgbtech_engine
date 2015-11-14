#include <bgbfrir.h>

FRIR_ObjectContext *FRASM_AllocObject()
{
	FRIR_ObjectContext *tmp;
	
	tmp=gcalloc("frasm_objectcontext_s", sizeof(FRIR_ObjectContext));
	return(tmp);
}

void FRASM_FreeObject(FRIR_ObjectContext *ctx)
{
	gcfree(ctx);
}

int FRASM_IndexString(FRIR_ObjectContext *ctx, char *str)
{
	char *cs, *cse;
	int i, j, k;

	if(!ctx->strtab)
	{
		ctx->strtab=gcatomic(1<<16);
		ctx->sz_strtab=1<<16;
		ctx->strtabe=2;
	}
	
	if(!str)return(0);		//NULL string
	if(!*str)return(1);	//empty string
	
	cs=ctx->strtab+2; cse=ctx->strtab+ctx->strtabe;
	while(cs<cse)
	{
		if(!strcmp(cs, str))
			return(cs-ctx->strtab);
		cs+=strlen(cs)+1;
	}
	
	cse=ctx->strtab+ctx->sz_strtab;
	if(cs+strlen(str)>=cse)
	{
		i=cs-ctx->strtab;
		j=ctx->sz_strtab;
		j=j+(j>>1);
		
		ctx->strtab=gcrealloc(ctx->strtab, j);
		ctx->sz_strtab=j;
		cs=ctx->strtab+i;
	}
	
	i=cs-ctx->strtab;
	strcpy(cs, str);
	cs+=strlen(str)+1;
	ctx->strtabe=cs-ctx->strtab;
	
	return(i);
}

int FRASM_SetMetaKeyStr(FRIR_ObjectContext *ctx,
	char *path, char *key, char *val)
{
}


byte *FRASM_EncodeIndexTable(byte *obuf, int obsz, int *idx, int nidx)
{
	int p0, p1, p2, pe, pe1, pv, pz;
	byte *ct;
	int i, j, k;

	ct=obuf;
	p0=0; p1=0; pz=0;
	for(i=0; i<nidx; i++)
	{
		pv=idx[i];
		p2=2*p1-p0;
		pe=pv-p2;
		pe1=pv-p1;
		if(!pe) { pz++; p0=p1; p1=pv; continue; }
		if(pz)
		{
			while(pz>=256)
			{
				*ct++=0xF0;
				*ct++=240;
				pz-=256;
			}

			if(pz<2)
				{ *ct++=0; }
			else if(pz<16)
				{ *ct++=0xE0+pz; }
			else if(pz<256)
				{ *ct++=0xF0; *ct++=pz-16; }
			else
				{ *(int *)-1=-1; }
				
			pz=0;
		}
		
		j=(pe>=0)?(pe<<1):(((-pe)<<1)-1);
		k=(pe1>=0)?(pe1<<1):(((-pe1)<<1)-1);
		
		if(j<128)
			{ *ct++=j; }
		else if(j<16384)
			{ *ct++=0x80|(j>>8); *ct++=j&255; }
		else if(k<256)
			{ *ct++=0xF1; *ct++=k; }
		else if(j<2097152)
			{ *ct++=0xC0|(j>>16); *ct++=(j>>8)&255; *ct++=j&255; }
		else if(k<65536)
			{ *ct++=0xF2; *ct++=(k>>8)&255; *ct++=k&255; }
		else
		{
			j=pv;
			if(j<16777216)
			{
				*ct++=0xE0;			*ct++=(j>>16)&255;
				*ct++=(j>>8)&255;	*ct++=j&255;
			}else
			{
				*ct++=0xE1;
				*ct++=(j>>24)&255;	*ct++=(j>>16)&255;
				*ct++=(j>>8)&255;	*ct++=j&255;
			}
		}
		
		p0=p1; p1=pv;
	}
	
	return(ct);
}

int FRASM_DecodeIndexTable(byte *ibuf, int ibsz, int *oidx, int midx)
{
	byte *cs, *cse;
	int *ct, *cte;
	int p0, p1, p2, pe, pe1, pv, pz;
	int i, j, k;

	p0=0; p1=0;
	cs=ibuf; cse=ibuf+ibsz;
	ct=oidx; cse=oidx+midx;
	while((cs<cse) && (ct<cte))
	{
		p2=2*p1-p0;
		i=*cs++;
		if(i<0x80)
		{
			j=(i&1)?(-((i+1)>>1)):(i>>1);
			pv=p2+j;
			*ct++=pv;
			p0=p1; p1=pv;
			continue;
		}else if(i<0xC0)
		{
			i=((i&63)<<8)|(*cs++);
			j=(i&1)?(-((i+1)>>1)):(i>>1);
			pv=p2+j;
			*ct++=pv;
			p0=p1; p1=pv;
			continue;
		}else if(i<0xE0)
		{
			i=((i&31)<<8)|(*cs++);
			i=(i<<8)|(*cs++);
			j=(i&1)?(-((i+1)>>1)):(i>>1);
			pv=p2+j;
			*ct++=pv;
			p0=p1; p1=pv;
			continue;
		}else if(i==0xE0)
		{
			pv=(cs[0]<<16)|(cs[1]<<8)|cs[2]; cs+=3;
			*ct++=pv;
			p0=p1; p1=pv;
			continue;
		}else if(i==0xE1)
		{
			pv=(cs[0]<<24)|(cs[1]<<16)|(cs[2]<<8)|cs[3]; cs+=4;
			*ct++=pv;
			p0=p1; p1=pv;
			continue;
		}else if(i<0xF0)
		{
			j=i-0xE0;
			while(j>0)
			{
				p2=2*p1-p0;
				*ct++=p2;
				p0=p1; p1=p2;
				j--;
			}
			continue;
		}else if(i==0xF0)
		{
			j=(*cs++)+16;
			while(j>0)
			{
				p2=2*p1-p0;
				*ct++=p2;
				p0=p1; p1=p2;
				j--;
			}
			continue;
		}else if(i==0xF1)
		{
			i=*cs++;
			j=(i&1)?(-((i+1)>>1)):(i>>1);
			pv=p2+j;
			*ct++=pv;
			p0=p1; p1=pv;
			continue;
		}else if(i==0xF2)
		{
			i=*cs++;
			i=(i<<8)|(*cs++);
			j=(i&1)?(-((i+1)>>1)):(i>>1);
			pv=p2+j;
			*ct++=pv;
			p0=p1; p1=pv;
			continue;
		}
	}
}
