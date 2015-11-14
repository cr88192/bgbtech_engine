#include <bgbgc.h>
#include <bgbdy.h>

byte *dyll_stuffstr_buf;
int dyll_stuffstr_sz;
int dyll_stuffstr_pos;

int *dyll_stuffstr_hash;

int *dyll_stuffstr_varr;	//value array
int *dyll_stuffstr_carr;	//chain array
int dyll_stuffstr_nstr;
int dyll_stuffstr_mstr;
int *dyll_stuffstr_strhash;

int DYLL_StuffStr_Init()
{
	static int init=0;

	if(init)return(0);
	init=1;

	dyll_stuffstr_buf=malloc(65536);
	dyll_stuffstr_sz=65536;
	dyll_stuffstr_pos=0;

	dyll_stuffstr_hash=malloc(4096*sizeof(short));
	memset(dyll_stuffstr_hash, 0, 4096*sizeof(short));

	dyll_stuffstr_strhash=malloc(4096*sizeof(short));
	memset(dyll_stuffstr_strhash, 0, 4096*sizeof(short));


	dyll_stuffstr_varr=malloc(65536*sizeof(int));
	dyll_stuffstr_carr=malloc(65536*sizeof(int));
	dyll_stuffstr_nstr=0;
	dyll_stuffstr_mstr=65536;

	return(0);
}

void DYLL_StuffStr_OutByte(byte c)
{
	int i;

	if((dyll_stuffstr_pos+1)>=dyll_stuffstr_sz)
	{
		i=dyll_stuffstr_sz+(dyll_stuffstr_sz>>1);
		dyll_stuffstr_buf=realloc(dyll_stuffstr_buf, i);
		dyll_stuffstr_sz=i;
	}

	dyll_stuffstr_buf[dyll_stuffstr_pos++]=c;

	if(dyll_stuffstr_pos>=3)
	{
		i=dyll_stuffstr_buf[dyll_stuffstr_pos-3];
		i=(i*127)+dyll_stuffstr_buf[dyll_stuffstr_pos-2];
		i=(i*127)+dyll_stuffstr_buf[dyll_stuffstr_pos-1];
		i&=0x3FF;

		dyll_stuffstr_hash[i]=dyll_stuffstr_pos-3;
	}
}

void DYLL_StuffStr_OutVLI(int i)
{
	if(i>2097151)
	{
		DYLL_StuffStr_OutByte(0xE0|((i>>24)&0x0F));
		DYLL_StuffStr_OutByte((i>>16)&0xFF);
		DYLL_StuffStr_OutByte((i>>8)&0xFF);
		DYLL_StuffStr_OutByte(i&0xFF);
	}
	if(i>16383)
	{
		DYLL_StuffStr_OutByte(0xC0|((i>>16)&0x1F));
		DYLL_StuffStr_OutByte((i>>8)&0xFF);
		DYLL_StuffStr_OutByte(i&0xFF);
	}
	if(i>127)
	{
		DYLL_StuffStr_OutByte(0x80|((i>>8)&0x3F));
		DYLL_StuffStr_OutByte(i&0xFF);
	}
	DYLL_StuffStr_OutByte(i);
}

int DYLL_StuffStr_Lookup(byte *str, int *roffs, int *rsz)
{
	byte *s, *t;
	int i, j, k, hi;

	if(!str[0])return(0);
	if(!str[1])return(0);
	if(!str[2])return(0);

	hi=(((str[0]*127)+str[1]*127)+str[2])&0x3FF;

	i=dyll_stuffstr_hash[hi];

	s=str;
	t=dyll_stuffstr_buf+i;
	for(j=0; s[j] && (s[j]==t[j]); j++);


	if(j>=3)
	{
		k=dyll_stuffstr_pos-i;
		if(k>=65536)return(0);

		if(j>63)j=63;

		*roffs=k;
		*rsz=j;
		return(1);
	}

	return(0);
}

int DYLL_StuffStr_StuffString(byte *str)
{
	byte *s;
	int i, idx, offs, sz;

	idx=dyll_stuffstr_pos;

	s=str;
	while(*s)
	{
		i=DYLL_StuffStr_Lookup(s, &offs, &sz);

		if(i)
		{
			DYLL_StuffStr_OutByte(0x80+sz);
//			DYLL_StuffStr_OutVLI(offs);

			DYLL_StuffStr_OutByte((offs>>8)&0xFF);
			DYLL_StuffStr_OutByte(offs&0xFF);

			s+=sz;
			continue;
		}

		if((*s>=0x80) && (*s<0xC0))
		{
			DYLL_StuffStr_OutByte(0x80);
			DYLL_StuffStr_OutByte(*s++);
			continue;
		}
		DYLL_StuffStr_OutByte(*s++);
	}
	DYLL_StuffStr_OutByte(0);

	return(idx);
}

int DYLL_StuffStr_UnstuffString(byte *buf, int idx)
{
	byte *s, *t, *s1;
	int sz, offs;

	s=dyll_stuffstr_buf+idx; t=buf;

	while(*s)
	{
		if(*s==0x80)
		{
			s++;
			*t++=*s++;
			continue;
		}

		if((*s>=0x83) && (*s<0xC0))
		{
			sz=*s-0x80;
			offs=(s[1]<<8)+s[2];
			memcpy(t, s-offs, sz);
			t+=sz; s+=3;
			continue;
		}

		*t++=*s++;
	}

	*t++=0;
}

int DYLL_StuffStr_StrDup_i(char *str)
{
	char tb[4096];
	char *s, *t;
	int i, j, hi;

	if(!str)return(0);

	s=str; hi=0;
	while(*s)hi=(hi*251)+(*s++);
	hi&=4095;

	i=dyll_stuffstr_strhash[hi];
	while(i)
	{
		DYLL_StuffStr_UnstuffString(tb, dyll_stuffstr_varr[i]);
		if(!strcmp(tb, str))return(i);
		i=dyll_stuffstr_carr[i];
	}

	if((dyll_stuffstr_nstr+1)>=dyll_stuffstr_mstr)
	{
		i=dyll_stuffstr_mstr+(dyll_stuffstr_mstr>>1);
		dyll_stuffstr_varr=realloc(dyll_stuffstr_varr, i*sizeof(int));
		dyll_stuffstr_carr=realloc(dyll_stuffstr_carr, i*sizeof(int));
		dyll_stuffstr_mstr=i;
	}

	j=DYLL_StuffStr_StuffString((byte *)str);

	i=dyll_stuffstr_nstr++;
	dyll_stuffstr_varr[i]=j;
	dyll_stuffstr_carr[i]=dyll_stuffstr_strhash[hi];
	dyll_stuffstr_strhash[hi]=i;

	return(i);
}

char *DYLL_StuffStr_StrTab_i(int i)
{
	char tb[4096];

	DYLL_StuffStr_UnstuffString(tb, dyll_stuffstr_varr[i]);
	return(gcrstrdup(tb));
}

