#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <math.h>

#ifndef M_PI
#define M_PI 3.1415926535897932384626433832795
#endif

#define HASH_SZ (1<<16)


typedef unsigned char byte;
typedef unsigned short u16;

char *tst_strings[1<<20];
int tst_n_strings;

int tst_tab_first[1<<16];
int tst_tab_next[1<<16];

int load_strings(char *name)
{
	char tb[1024];
	FILE *fd;
	char *s, *t;
	int i, j, k, n;

	fd=fopen(name, "rt");
	if(!fd)return(-1);

	n=0;
	while(!feof(fd))
	{
		fgets(tb, 255, fd);
		tst_strings[n++]=strdup(tb);
	}
	fclose(fd);
	tst_n_strings=n;

	while(n<262144)
	{
		i=rand()%n; j=rand()%n;
		s=tst_strings[i];
		t=tst_strings[j];

		memset(tb, 0, 1024);
		i=rand()%strlen(s);
		strncpy(tb, s, i);
		i=rand()%strlen(t);
		strcat(tb, t+i);

		tst_strings[n++]=strdup(tb);
	}
	tst_n_strings=n;
}

int load_strings2(char *name)
{
	char tb[4096];
	FILE *fd;
	char *s, *t;
	int i, j, k, n;

	fd=fopen(name, "rb");
	if(!fd)return(-1);

	n=0;
	while(!feof(fd))
	{
		k=rand()%512+16;

		fread(tb, 1, k, fd);
		for(i=0; i<k; i++)
			if(!tb[i])tb[i]=0x80;
		tb[k]=0;

		tst_strings[n++]=strdup(tb);
	}
	fclose(fd);
	tst_n_strings=n;

//	while(n<262144)
	while(n<(1<<20))
	{
		i=rand()%n; j=rand()%n;
		s=tst_strings[i];
		t=tst_strings[j];

		memset(tb, 0, 4096);
		i=rand()%strlen(s);
		strncpy(tb, s, i);
		i=rand()%strlen(t);
		strcat(tb, t+i);

		tb[1024]=0;

		tst_strings[n++]=strdup(tb);
	}
	tst_n_strings=n;
}

int test_hash(
	int (*hash_str_f)(char *str),
	int (*hash_next_f)(int hash),
	int (*hash_idx_f)(int hash),
	int *cnt_first, int *cnt_next)
{
	int i, j, k, hi;

	for(i=0; i<HASH_SZ; i++)
	{
		cnt_first[i]=0;
		cnt_next[i]=0;
	}

	for(i=0; i<tst_n_strings; i++)
	{
		hi=hash_str_f(tst_strings[i]);

		j=hash_idx_f(hi)&(HASH_SZ-1);
		cnt_first[j]++;

		for(j=0; j<16; j++)
		{
			k=hash_idx_f(hi)&(HASH_SZ-1);
			cnt_next[k]++;
			hi=hash_next_f(hi);
		}
	}
}

int test_stat(int *tab)
{
	int hist[8];
	int min, max, tot;
	float avg, lavg, havg;
	float l2avg, hlavg, lhavg, h2avg;
	float f;
	int i, j, k;

	min=tab[0]; max=min; tot=0;
	for(i=0; i<HASH_SZ; i++)
	{
		if(tab[i]<min)min=tab[i];
		if(tab[i]>max)max=tab[i];
		tot+=tab[i];
	}

	avg=tot/((float)HASH_SZ);
	lavg=(min+avg)/2.0;
	havg=(max+avg)/2.0;

	l2avg=(min+lavg)/2.0;
	hlavg=(avg+lavg)/2.0;
	lhavg=(avg+havg)/2.0;
	h2avg=(max+havg)/2.0;

	printf("Min=%d Max=%d Avg=%f LAvg=%f HAvg=%f\n",
		min, max, avg, lavg, havg);

	for(i=0; i<8; i++)hist[i]=0;
	for(i=0; i<HASH_SZ; i++)
	{
		if(tab[i]<l2avg)hist[0]++;
		if((tab[i]>=l2avg) && (tab[i]<lavg))hist[1]++;
		if((tab[i]>=lavg) && (tab[i]<hlavg))hist[2]++;
		if((tab[i]>=hlavg) && (tab[i]<avg))hist[3]++;
		if((tab[i]>=avg) && (tab[i]<lhavg))hist[4]++;
		if((tab[i]>=lhavg) && (tab[i]<havg))hist[5]++;
		if((tab[i]>=havg) && (tab[i]<h2avg))hist[6]++;
		if(tab[i]>=h2avg)hist[7]++;
	}

	printf("%6d %6d %6d %6d  %6d %6d %6d %6d\n",
		hist[0], hist[1], hist[2], hist[3],
		hist[4], hist[5], hist[6], hist[7]);

	f=HASH_SZ;
	printf("%.4f %.4f %.4f %.4f  %.4f %.4f %.4f %.4f\n",
		hist[0]/f, hist[1]/f, hist[2]/f, hist[3]/f,
		hist[4]/f, hist[5]/f, hist[6]/f, hist[7]/f);
	printf("\n");
}

#if 0
int hash_string_0(char *str)
	{ char *s; int i; i=0; s=str; while(*s)i=(i*4093)+(*s++); return(i); }
int hash_next_0(int hi)		{ return(hi*4093); }
int hash_idx_0(int hi)		{ return(hi>>12); }

int hash_string_1(char *str)
	{ char *s; int i; i=0; s=str; while(*s)i=(i*4095)+(*s++); return(i); }
int hash_next_1(int hi) { return(hi*4095); }
int hash_idx_1(int hi)	{ return(hi>>12); }

int hash_string_2(char *str)
	{ char *s; int i; i=0; s=str; while(*s)i=(i*127)+(*s++); return(i); }
int hash_next_2(int hi) { return(hi*127); }
int hash_idx_2(int hi)	{ return(hi); }

int hash_string_3(char *str)
	{ char *s; int i; i=0; s=str; while(*s)i=(i*4093)+(*s++); return(i>>12); }
int hash_next_3(int hi) { return(hi*127); }
int hash_idx_3(int hi)	{ return(hi); }
#endif


u16 CRC16_CCIT(byte *data, int sz)
{
	byte *cs, *ce;
	u16 crc;

	cs=data; ce=cs+sz; crc=0;
	while(cs<ce)
	{
		crc  = (byte)(crc >> 8) | (crc << 8);
		crc ^= *cs++;
		crc ^= (byte)(crc & 0xff) >> 4;
		crc ^= crc << 12;
		crc ^= ((crc & 0xff) << 4) << 1;
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

u16 Hash16_BGB2(byte *data, int sz)
{
	byte *cs, *ce;
	u16 hash;

	cs=data; ce=cs+sz; hash=0;
	while(cs<ce)
		hash=(hash*127)+(*cs++);
	return(hash);
}

int hash_string_0(char *str)
	{ return(CRC16_CCIT(str, strlen(str))); }
int hash_next_0(int hi) { return(hi*127); }
int hash_idx_0(int hi)	{ return(hi); }

int hash_string_1(char *str)
	{ return(Hash16_BGB(str, strlen(str))); }
int hash_next_1(int hi) { return(hi*127); }
int hash_idx_1(int hi)	{ return(hi); }

int hash_string_2(char *str)
	{ return(Hash16_BGB2(str, strlen(str))); }
int hash_next_2(int hi) { return(hi*127); }
int hash_idx_2(int hi)	{ return(hi); }

int main()
{
	float tab[8];
	float f;
	int i;

//	load_strings("filelst1.txt");
	load_strings2("zpack.dll");

	for(i=0; i<16; i++)
		printf("%s", tst_strings[tst_n_strings-1-i]);

	for(i=0; i<HASH_SZ; i++)
		tst_tab_first[i]=rand()&(HASH_SZ-1);
	test_stat(tst_tab_first);

#if 1
	for(i=0; i<8; i++)tab[i]=sin((i+0.5)*M_PI/8);
	f=0; for(i=0; i<8; i++)f+=tab[i];

	for(i=0; i<8; i++)printf("%6d ", (int)(4096*tab[i]/f));
	printf("\n");

	for(i=0; i<8; i++)printf("%.4f ", tab[i]/f);
	printf("\n");
#endif
	printf("\n");
	printf("\n");


	printf("Hash Func 0\n");

	test_hash(hash_string_0, hash_next_0, hash_idx_0,
		tst_tab_first, tst_tab_next);
	test_stat(tst_tab_first);
//	test_stat(tst_tab_next);
	printf("\n");

	printf("Hash Func 1\n");
	test_hash(hash_string_1, hash_next_1, hash_idx_1,
		tst_tab_first, tst_tab_next);
	test_stat(tst_tab_first);
//	test_stat(tst_tab_next);
	printf("\n");

#if 1
	printf("Hash Func 2\n");
	test_hash(hash_string_2, hash_next_2, hash_idx_2,
		tst_tab_first, tst_tab_next);
	test_stat(tst_tab_first);
//	test_stat(tst_tab_next);
	printf("\n");
#endif

#if 0
	test_hash(hash_string_3, hash_next_3, hash_idx_3,
		tst_tab_first, tst_tab_next);
	test_stat(tst_tab_first);
	test_stat(tst_tab_next);
	printf("\n");
#endif
}

