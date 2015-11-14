#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define WIDTH	24
#define PREC	16

int DYS_DumpBuf(char *tb, int sz)
{
	int i;

	for(i=0; i<sz; i++)fputc(tb[i]+'0', stdout);
//	for(i=0; i<sz; i++)printf("%d ", tb[i]);
	fputc('\n', stdout);
}

int DYS_InvertBuf(char *tb, int sz)
{
	char *t;

	t=tb+(sz-1);
	while(t>=tb)*t--=9-(*t);
	return(0);
}

int DYS_NegBuf(char *tb, int sz)
{
	char *t;
	int i;

	t=tb+(sz-1); i=1;
	while(t>=tb)
	{
		i+=9-(*t);
		*t--=(i%10); i/=10;
	}

	return(i);
}

int DYS_Shl10Buf(char *tb, int sz, int sh)
{
	char *s, *t, *se;
	s=tb+sh; t=tb; se=tb+sz;
	while(s<se)*t++=*s++;
	while(t<se)*t++=0;
	return(0);
}

int DYS_Shr10Buf(char *tb, int sz, int sh)
{
	char *s, *t;
	s=tb+(sz-1)-sh; t=tb+(sz-1);
	while(s>=tb)*t--=*s--;
	while(t>=tb)*t--=(s[1]>=5)?9:0;
	return(0);
}

int DYS_Shl2Buf1(char *tb, int sz)
{
	char *t;
	int i;

	t=tb+sz-1; i=0;
	while(t>=tb)
	{
		i+=(*t)*2;
		*t--=(i%10); i/=10;
	}
	return(0);
}

int DYS_Shr2Buf1(char *tb, int sz)
{
	char *t, *te;
	int i, j;

	t=tb; te=tb+sz; i=(tb[0]>=5)?9:0;
	while(t<te)
	{
		j=i&1;
		i=(*t);
		j=(i/2)+(j*5);
		*t--=j;
	}
	return(0);
}

void DYS_Shl2Buf(char *tb, int sz, int sh)
{
	int i;
	for(i=0; i<sh; i++)DYS_Shl2Buf1(tb, sz);
}

void DYS_Shr2Buf(char *tb, int sz, int sh)
{
	int i;
	for(i=0; i<sh; i++)DYS_Shr2Buf1(tb, sz);
}

int DYS_AddBufInt(char *tb, int sz, int v)
{
	char *t;
	int i;

	t=tb+(sz-1);

	i=v;
	while(i && (t>=tb))
	{
		i+=*t;
		*t--=(i%10); i/=10;
	}

	return(i);
}

int DYS_AddBuf(char *sa, int sza, char *sb, int szb)
{
	char *s, *t;
	int i, j;

	s=sb+(szb-1);
	t=sa+(sza-1);

	i=0;
	while((s>=sb) && (t>=sa))
	{
		i+=(*t)+(*s);
		*t=(i%10); i/=10;
		s--; t--;
	}

	if(t<sa)return(-1);

	j=(sb[0]>=5)?9:0;
	while(i && (t>=sa))
	{
		i+=(*t)+j;
		*t=(i%10); i/=10;
		t--;
	}

	return(i);
}

int DYS_SubBuf(char *sa, int sza, char *sb, int szb)
{
	char *s, *t;
	int i, j;

	s=sb+(szb-1);
	t=sa+(sza-1);

	i=1;
	while((s>=sb) && (t>=sa))
	{
		i+=(*t)+(9-(*s));
		*t=(i%10); i/=10;
		s--; t--;
	}

	if(t<sa)return(-1);

	j=(sb[0]>=5)?9:0;
	while(i && (t>=sa))
	{
		i+=(*t)+(9-j);
		*t=(i%10); i/=10;
		t--;
	}

	return(i);
}

int DYS_AddBufScaleInt(char *sa, int sza, char *sb, int szb, int sc)
{
	char *s, *t;
	int i;

	s=sb+(szb-1);
	t=sa+(sza-1);

	i=0;
	while((s>=sb) && (t>=sa))
	{
		i+=(*t)+((*s)*sc);
		*t=(i%10); i/=10;
		s--; t--;
	}

	if(t<sa)return(-1);

	while(i && (t>=sa))
	{
		i+=(*t);
		*t=(i%10); i/=10;
		t--;
	}

	return(i);
}

int DYS_CmpBuf(char *sa, int sza, char *sb, int szb)
{
	int pa, pb;
	int i, j, k;

	if((sa[0]>=5) && (sb[0]<5))return(-1);
	if((sa[0]<5) && (sb[0]>=5))return(1);

	i=(sza>szb)?sza:szb;
	pa=sza-i;
	pb=szb-i;

	while(pa<sza)
	{
		i=(pa>=0)?sa[pa]:((sa[0]>=5)?9:0);
		j=(pb>=0)?sb[pb]:((sb[0]>=5)?9:0);
		if(i!=j)break;
		pa++; pb++;
	}

	if(pa>=sza)return(0);

	if(i<j)return(-1);
	if(i>j)return(1);
	return(0);
}

int DYS_CopyStrBuf(char *tb, int sz, char *sa)
{
	char *s, *t;
	int i, j, pt;

	if((sa[0]=='0') && (sa[1]=='x'))
	{
		for(i=0; i<sz; i++)tb[i]=0;

		s=sa+2;
		while(*s)
		{
			i=0;
			if((*s>='0') && (*s<='9'))i=*s-'0';
			if((*s>='A') && (*s<='F'))i=*s-'A'+10;
			if((*s>='a') && (*s<='f'))i=*s-'a'+10;
			s++;

			DYS_Shl2Buf(tb, sz, 4);
			DYS_AddBufInt(tb, sz, i);
		}

		return(0);
	}

	s=sa+strlen(sa)-1;
	t=tb+(sz-1);

	i=0; j=0; pt=0;
	while((s>=sa) && (t>=tb))
	{
		if(*s=='-') break;
		if(*s=='.') { s--; pt=j; continue; }

		*t--=(*s--)-'0'; j++;
	}

	while(t>=tb)*t--=0;

	if(*s=='-')DYS_NegBuf(tb, sz);
	return(pt);
}

int DYS_CopyBufStr(char *tb, char *sa, int sz, int pt)
{
	char *s, *t, *s1, *se, *sp;
	int i;

//	printf("TS0 %d %d\n", sz, p);

	t=tb;
	if(sa[0]>=5)
	{
		DYS_NegBuf(sa, sz);
		*t++='-';
	}

	if(pt>PREC)
	{
		i=pt-PREC;
		sz-=i;
		pt-=i;
	}

	s=sa; se=sa+sz; sp=sa+sz-pt;

	while((!(*s)) && (s<sp))s++;

//	printf("TS1 %d\n", p);

	if(s>=sp)*t++='0';
	while(s<sp)*t++=(*s++)+'0';

	if(s<se)
	{
		*t++='.';
		while(s<se)
		{
			*t++=(*s++)+'0';
			s1=s; while(!(*s1) && (s1<se))s1++;
			if(s1>=se)break;
		}
	}

	*t++=0;

//	DYS_DumpBuf(sa, sz);
//	printf("TS %s %d\n", tb, p);

	return(0);
}

char *dysShl(char *s, int v)
{
	char ta[WIDTH], tb[WIDTH];
	int i, pa, pb;

	pa=DYS_CopyStrBuf(ta, WIDTH, s);
	DYS_Shl2Buf(ta, WIDTH, v);
	DYS_CopyBufStr(tb, ta, WIDTH, pa);
	return(strdup(tb));
}

char *dysShr(char *s, int v)
{
	char ta[WIDTH], tb[WIDTH];
	int i, pa, pb;

	pa=DYS_CopyStrBuf(ta, WIDTH, s);
	DYS_Shr2Buf(ta, WIDTH, v);
	DYS_CopyBufStr(tb, ta, WIDTH, pa);
	return(strdup(tb));
}

char *dysShl10(char *s, int v)
{
	char ta[WIDTH], tb[WIDTH];
	int i, pa, pb;

	pa=DYS_CopyStrBuf(ta, WIDTH, s);
	if(v>pa) { DYS_Shl10Buf(ta, WIDTH, v-pa); pa=0; }
		else pa-=v;
	DYS_CopyBufStr(tb, ta, WIDTH, pa);
	return(strdup(tb));
}

char *dysShr10(char *s, int v)
{
	char ta[WIDTH], tb[WIDTH];
	int i, pa, pb;

	pa=DYS_CopyStrBuf(ta, WIDTH, s);
	DYS_Shr10Buf(ta, WIDTH, v);
	DYS_CopyBufStr(tb, ta, WIDTH, pa);
	return(strdup(tb));
}

char *dysShr10F(char *s, int v)
{
	char ta[WIDTH], tb[WIDTH];
	int i, pa, pb;

	pa=DYS_CopyStrBuf(ta, WIDTH, s);
	pa+=v;
	DYS_CopyBufStr(tb, ta, WIDTH, pa);
	return(strdup(tb));
}

int dysCmp(char *s0, char *s1)
{
	char ta[WIDTH], tb[WIDTH];
	int i, pa, pb;

	pa=DYS_CopyStrBuf(ta, WIDTH, s0);
	pb=DYS_CopyStrBuf(tb, WIDTH, s1);

	if(pa || pb)
	{
		i=pa>pb?pa:pb;
		DYS_Shl10Buf(ta, WIDTH, i-pa);
		DYS_Shl10Buf(tb, WIDTH, i-pb);
		pa=i;
	}

	i=DYS_CmpBuf(ta, WIDTH, tb, WIDTH);
	return(i);
}

char *dysAdd(char *s0, char *s1)
{
	char ta[WIDTH], tb[WIDTH];
	int i, pa, pb;

	pa=DYS_CopyStrBuf(ta, WIDTH, s0);
	pb=DYS_CopyStrBuf(tb, WIDTH, s1);

//	DYS_DumpBuf(ta, WIDTH);
//	DYS_DumpBuf(tb, WIDTH);

	if(pa || pb)
	{
		i=pa>pb?pa:pb;
		DYS_Shl10Buf(ta, WIDTH, i-pa);
		DYS_Shl10Buf(tb, WIDTH, i-pb);
		pa=i;
	}

//	DYS_DumpBuf(ta, WIDTH);
//	DYS_DumpBuf(tb, WIDTH);

	DYS_AddBuf(ta, WIDTH, tb, WIDTH);

//	DYS_DumpBuf(ta, WIDTH);

	DYS_CopyBufStr(tb, ta, WIDTH, pa);
	return(strdup(tb));
}

char *dysSub(char *s0, char *s1)
{
	char ta[WIDTH], tb[WIDTH];
	int i, pa, pb;

	pa=DYS_CopyStrBuf(ta, WIDTH, s0);
	pb=DYS_CopyStrBuf(tb, WIDTH, s1);

	DYS_NegBuf(tb, WIDTH);

	if(pa || pb)
	{
		i=pa>pb?pa:pb;
		DYS_Shl10Buf(ta, WIDTH, i-pa);
		DYS_Shl10Buf(tb, WIDTH, i-pb);
		pa=i;
	}

//	DYS_DumpBuf(ta, WIDTH);
//	DYS_DumpBuf(tb, WIDTH);

	DYS_AddBuf(ta, WIDTH, tb, WIDTH);

//	DYS_DumpBuf(ta, WIDTH);

	DYS_CopyBufStr(tb, ta, WIDTH, pa);
	return(strdup(tb));
}

char *dysMul(char *s0, char *s1)
{
	char ta[WIDTH], tb[WIDTH], tc[WIDTH];
	int i, j, pa, pb, pc;

	pa=DYS_CopyStrBuf(ta, WIDTH, s0);
	pb=DYS_CopyStrBuf(tb, WIDTH, s1);
	pc=pa+pb;

	for(i=0; i<WIDTH; i++)tc[i]=0;

	for(i=0; i<WIDTH; i++)
	{
		DYS_AddBufScaleInt(tc, WIDTH-i, ta, WIDTH, tb[WIDTH-1-i]);
	}

	DYS_CopyBufStr(ta, tc, WIDTH, pc);
	return(strdup(ta));
}

char *dysDiv(char *s0, char *s1)
{
	char ta[WIDTH*3], tb[WIDTH*3], tc[WIDTH*2];
	int pa, pb, pc;
	int i, j, k, sg;

	for(i=0; i<(3*WIDTH); i++)ta[i]=0;
	for(i=0; i<(3*WIDTH); i++)tb[i]=0;
	for(i=0; i<(2*WIDTH); i++)tc[i]=0;

	pa=DYS_CopyStrBuf(ta+WIDTH, WIDTH, s0);
	pb=DYS_CopyStrBuf(tb, WIDTH, s1);
	pc=pa+pb;

	sg=0;
	if(ta[WIDTH]>=5) { DYS_NegBuf(ta+WIDTH, WIDTH); sg=!sg; }
	if(tb[0]>=5) { DYS_NegBuf(tb, WIDTH); sg=!sg; }

	DYS_Shr10Buf(ta, WIDTH*3, pa);
	DYS_Shr10Buf(tb, WIDTH*3, pb);

//	DYS_DumpBuf(ta, WIDTH*3);
//	DYS_DumpBuf(tb, WIDTH*3);

	for(i=0; i<=WIDTH*2; i++)
	{
		j=10;

		while(DYS_CmpBuf(ta, WIDTH*3, tb, WIDTH*3-i)>=0)
		{
//			printf("A ");
//			DYS_DumpBuf(ta, WIDTH*3);
//			printf("B "); k=i; while(k--)fputc(' ', stdout);
//			DYS_DumpBuf(tb, WIDTH*3-i);

			DYS_SubBuf(ta, WIDTH*3, tb, WIDTH*3-i);
			DYS_AddBufInt(tc, i, 1);

			if(!(j--))break;
		}
	}

//	DYS_DumpBuf(ta, WIDTH*3);
//	DYS_DumpBuf(tb, WIDTH*3);
//	DYS_DumpBuf(tc, WIDTH*2);

	if(sg)DYS_NegBuf(tc, WIDTH*2);

	DYS_CopyBufStr(tb, tc, WIDTH*2, WIDTH);
	return(strdup(tb));
}


int main()
{
	char *s0, *s1, *s2;
	int i, j, k;

	s0="1234.5";
	s1="-4321.0";

	s2=dysAdd(s0, s1);
	printf("%s %s %s\n", s0, s1, s2);

//	s0="1234.5";
//	s1="-4321.0";

	s2=dysSub(s0, s1);
	printf("%s %s %s\n", s0, s1, s2);

	s0="12345678";
	s1="-87654321";

	s2=dysMul(s0, s1);
	printf("%s %s %s\n", s0, s1, s2);

	i=dysCmp(s0, s1);
	j=dysCmp(s1, s0);
	k=0;
	printf("%s %s %d %d %d\n", s0, s1, i, j, k);

	s2=dysDiv(s0, s1);
	printf("%s %s %s\n", s0, s1, s2);

	s2=dysDiv(s0, "0x100");
	printf("%s %s %s\n", s0, s1, s2);
}
