#include <bgbmid.h>

void BSG_GenNumber_r(BSG_State *ctx, int v)
{
	if(v<0)
	{
		BSG_GenNumber_r(ctx, -v);
		BSG_PushWord(ctx, "negative");
		return;
	}

	if(v>=1000000)
	{
		BSG_GenNumber_r(ctx, v%1000000);
		BSG_PushWord(ctx, "million");
		BSG_GenNumber_r(ctx, v/1000000);
		return;
	}

	if(v>=1000)
	{
		BSG_GenNumber_r(ctx, v%1000);
		BSG_PushWord(ctx, "thousand");
		BSG_GenNumber_r(ctx, v/1000);
		return;
	}

	if(v>=100)
	{
		BSG_GenNumber_r(ctx, v%100);
		BSG_PushWord(ctx, "hundred");
		BSG_GenNumber_r(ctx, v/100);
		return;
	}

	if(v>=20)
	{
		BSG_GenNumber_r(ctx, v%10);
//		BSG_PushWord(ctx, "hundred");
//		BSG_GenNumber_r(ctx, v/100);

		switch(v/10)
		{
		case 2: BSG_PushWord(ctx, "twenty"); break;
		case 3: BSG_PushWord(ctx, "thirty"); break;
		case 4: BSG_PushWord(ctx, "fourty"); break;
		case 5: BSG_PushWord(ctx, "fifty"); break;
		case 6: BSG_PushWord(ctx, "sixty"); break;
		case 7: BSG_PushWord(ctx, "seventy"); break;
		case 8: BSG_PushWord(ctx, "eighty"); break;
		case 9: BSG_PushWord(ctx, "ninety"); break;
		default: break;
		}
		return;
	}

	switch(v)
	{
	case 1: BSG_PushWord(ctx, "one"); break;
	case 2: BSG_PushWord(ctx, "two"); break;
	case 3: BSG_PushWord(ctx, "three"); break;
	case 4: BSG_PushWord(ctx, "four"); break;
	case 5: BSG_PushWord(ctx, "five"); break;
	case 6: BSG_PushWord(ctx, "six"); break;
	case 7: BSG_PushWord(ctx, "seven"); break;
	case 8: BSG_PushWord(ctx, "eight"); break;
	case 9: BSG_PushWord(ctx, "nine"); break;
	case 10: BSG_PushWord(ctx, "ten"); break;
	case 11: BSG_PushWord(ctx, "eleven"); break;
	case 12: BSG_PushWord(ctx, "twelve"); break;
	case 13: BSG_PushWord(ctx, "thirteen"); break;
	case 14: BSG_PushWord(ctx, "fourteen"); break;
	case 15: BSG_PushWord(ctx, "fifteen"); break;
	case 16: BSG_PushWord(ctx, "sixteen"); break;
	case 17: BSG_PushWord(ctx, "seventeen"); break;
	case 18: BSG_PushWord(ctx, "eighteen"); break;
	case 19: BSG_PushWord(ctx, "nineteen"); break;
	}
}

void BSG_GenNumber_th_r(BSG_State *ctx, int v)
{
	if(v>=1000000)
	{
		BSG_GenNumber_th_r(ctx, v%1000000);
		if(v%1000000)BSG_PushWord(ctx, "million");
			else BSG_PushWord(ctx, "millionth");
		BSG_GenNumber_r(ctx, v/1000000);
		return;
	}

	if(v>=1000)
	{
		BSG_GenNumber_th_r(ctx, v%1000);
		if(v%1000)BSG_PushWord(ctx, "thousand");
			else BSG_PushWord(ctx, "thousandth");
		BSG_GenNumber_r(ctx, v/1000);
		return;
	}

	if(v>=100)
	{
		BSG_GenNumber_th_r(ctx, v%100);
		if(v%100)BSG_PushWord(ctx, "hundred");
			else BSG_PushWord(ctx, "hundreth");
		BSG_GenNumber_r(ctx, v/100);
		return;
	}

	if(v>=20)
	{
		if(v%10)
		{
			BSG_GenNumber_th_r(ctx, v%10);
			switch(v/10)
			{
			case 2: BSG_PushWord(ctx, "twenty"); break;
			case 3: BSG_PushWord(ctx, "thirty"); break;
			case 4: BSG_PushWord(ctx, "fourty"); break;
			case 5: BSG_PushWord(ctx, "fifty"); break;
			case 6: BSG_PushWord(ctx, "sixty"); break;
			case 7: BSG_PushWord(ctx, "seventy"); break;
			case 8: BSG_PushWord(ctx, "eighty"); break;
			case 9: BSG_PushWord(ctx, "ninety"); break;
			default: break;
			}
		}else
		{
			switch(v/10)
			{
			case 2: BSG_PushWord(ctx, "twentyith"); break;
			case 3: BSG_PushWord(ctx, "thirtyith"); break;
			case 4: BSG_PushWord(ctx, "fourtyith"); break;
			case 5: BSG_PushWord(ctx, "fiftyith"); break;
			case 6: BSG_PushWord(ctx, "sixtyith"); break;
			case 7: BSG_PushWord(ctx, "seventyith"); break;
			case 8: BSG_PushWord(ctx, "eightyith"); break;
			case 9: BSG_PushWord(ctx, "ninetyith"); break;
			default: break;
			}
		}
		return;
	}

	switch(v)
	{
	case 1: BSG_PushWord(ctx, "first"); break;
	case 2: BSG_PushWord(ctx, "second"); break;
	case 3: BSG_PushWord(ctx, "third"); break;
	case 4: BSG_PushWord(ctx, "fourth"); break;
	case 5: BSG_PushWord(ctx, "fifth"); break;
	case 6: BSG_PushWord(ctx, "sixth"); break;
	case 7: BSG_PushWord(ctx, "seventh"); break;
	case 8: BSG_PushWord(ctx, "eighth"); break;
	case 9: BSG_PushWord(ctx, "nineth"); break;
	case 10: BSG_PushWord(ctx, "tenth"); break;
	case 11: BSG_PushWord(ctx, "eleventh"); break;
	case 12: BSG_PushWord(ctx, "twelth"); break;
	case 13: BSG_PushWord(ctx, "thirteenth"); break;
	case 14: BSG_PushWord(ctx, "fourteenth"); break;
	case 15: BSG_PushWord(ctx, "fifteenth"); break;
	case 16: BSG_PushWord(ctx, "sixteenth"); break;
	case 17: BSG_PushWord(ctx, "seventeenth"); break;
	case 18: BSG_PushWord(ctx, "eighteenth"); break;
	case 19: BSG_PushWord(ctx, "nineteenth"); break;
	}
}

void BSG_GenNumber_digit(BSG_State *ctx, int v)
{
	switch(v)
	{
//	case 0: BSG_PushWord(ctx, "zero"); break;
	case 0: BSG_PushWord(ctx, "O_ltr"); break;
	case 1: BSG_PushWord(ctx, "one"); break;
	case 2: BSG_PushWord(ctx, "two"); break;
	case 3: BSG_PushWord(ctx, "three"); break;
	case 4: BSG_PushWord(ctx, "four"); break;
	case 5: BSG_PushWord(ctx, "five"); break;
	case 6: BSG_PushWord(ctx, "six"); break;
	case 7: BSG_PushWord(ctx, "seven"); break;
	case 8: BSG_PushWord(ctx, "eight"); break;
	case 9: BSG_PushWord(ctx, "nine"); break;
	default: break;
	}
}

void BSG_GenNumber(BSG_State *ctx, char *s)
{
	double f, g;
	int i, j, k;

	f=atof(s);
	i=f;

	if(fabs(f-i)>0.001)
	{
		g=fabs(f-i); k=0;
		for(j=0; j<9; j++)
		{
			if(g<0.001)break;
			i=(g*10)+0.001; g=g*10-i;
			k=k*10+i;
		}
		if((g+0.5)>=1)k++;

		while(j--)
		{
			BSG_GenNumber_digit(ctx, k%10);
			k/=10;
		}

		BSG_PushWord(ctx, "point");

		i=f;
		BSG_GenNumber_r(ctx, i);
		return;
	}

	i=f;

	if(!strcmp(s+strlen(s)-2, "st") ||
		!strcmp(s+strlen(s)-2, "nd") ||
		!strcmp(s+strlen(s)-2, "rd") ||
		!strcmp(s+strlen(s)-2, "th"))
	{
//		BSG_GenNumber_th_r(ctx, i);

		if(!i) { BSG_PushWord(ctx, "zeroth"); return; }

		if(f<0)
		{
			BSG_GenNumber_th_r(ctx, -i);
			BSG_PushWord(ctx, "negative");
		}else BSG_GenNumber_th_r(ctx, i);
		return;
	}

	if(!i) { BSG_PushWord(ctx, "zero"); return; }

	if(f<0)
	{
		BSG_GenNumber_r(ctx, -i);
		BSG_PushWord(ctx, "negative");
	}else BSG_GenNumber_r(ctx, i);
}

void BSG_GenAcron(BSG_State *ctx, char *str)
{
	char buf[16];
	char *s;
	int i;

	s=str+strlen(str)-1;
	while(s>=str)
	{
		i=-1;
		if((*s>='A') && (*s<='Z'))i=*s;
		if((*s>='a') && (*s<='z'))i=*s-'a'+'A';
		if(i<0) {s--; continue; }

//		BSG_PushWord(ctx, "_");
		sprintf(buf, "%c_ltr", i);
		BSG_PushWord(ctx, buf);
		s--;
	}
}


int BSG_SpellVowel(char *str)
{
	int i;

	i=*str;
	if((*str>='A') && (*str<='Z'))i=*str-'A'+'a';
	if(i=='a')return(1);	if(i=='e')return(1);
	if(i=='i')return(1);	if(i=='o')return(1);
	if(i=='u')return(1);
	return(0);
}

int BSG_SpellConsonant(char *str)
{
	int i;

	i=*str;
	if((*str>='A') && (*str<='Z'))i=*str-'A'+'a';
	if(i<'a')return(0);	if(i>'z')return(0);
	if(i=='a')return(0);	if(i=='e')return(0);
	if(i=='i')return(0);	if(i=='o')return(0);
	if(i=='u')return(0);
	return(1);
}


char *BSG_SpellMonograph(char *str)
{
	if(*str=='c')return("k");
	return(NULL);
}

char *BSG_SpellDigraph(char *str)
{
	if((str[0]=='c') && (str[1]=='h'))return("C");
	if((str[0]=='s') && (str[1]=='h'))return("S");
	if((str[0]=='t') && (str[1]=='h'))return("T");
	if((str[0]=='n') && (str[1]=='g'))return("N");
	if((str[0]=='k') && (str[1]=='s'))return("X");
	if((str[0]=='s') && (str[1]=='k'))return("x");
	if((str[0]=='t') && (str[1]=='s'))return("c");
	if((str[0]=='s') && (str[1]=='t'))return("q");

	if((str[0]=='c') && (str[1]=='k'))return("k");
	if((str[0]=='p') && (str[1]=='h'))return("f");
	if((str[0]=='r') && (str[1]=='h'))return("r");
	if((str[0]=='s') && (str[1]=='c'))return("s");
	if((str[0]=='w') && (str[1]=='h'))return("w");
	if((str[0]=='w') && (str[1]=='r'))return("r");

	if((str[0]=='l') && (str[1]=='l'))return("l");

	if((str[0]=='g') && (str[1]=='h'))
	{
		if(!BSG_SpellVowel(str+2))
			return("f");
		return("g");
	}


	if((str[0]=='a') && (str[1]=='i'))return("A");
	if((str[0]=='o') && (str[1]=='o'))return("U");
	if((str[0]=='e') && (str[1]=='e'))return("E");

	if((str[0]=='a') && (str[1]=='u'))return("Q");
	if((str[0]=='a') && (str[1]=='o'))return("Q");

	if((str[0]=='o') && (str[1]=='u'))return("O");
	if((str[0]=='e') && (str[1]=='a'))return("e");

	if((str[0]=='l') && (str[1]=='y') && str[2])return("li");
	if((str[0]=='r') && (str[1]=='y') && str[2])return("ri");
	if((str[0]=='l') && (str[1]=='y') && !str[2])return("lE");
	if((str[0]=='r') && (str[1]=='y') && !str[2])return("rE");

	return(NULL);
}

char *BSG_SpellTrigraph(char *str)
{
	if((str[0]=='s') && (str[1]=='c') && (str[2]=='h'))return("x");
	if((str[0]=='t') && (str[1]=='c') && (str[2]=='h'))return("C");
	return(NULL);
}

int BSG_SpellCheckCE(char *str)
{
	if(!BSG_SpellConsonant(str))return(0);
	if(str[1]!='e')return(0);
	if(str[2])return(0);
	return(1);
}

int BSG_SpellCheckCV(char *str)
{
	if(!BSG_SpellConsonant(str))return(0);
	if(!BSG_SpellVowel(str+1))return(0);
	return(1);
}

void BSG_GenDecipher(BSG_State *ctx, char *str)
{
	char buf[64], tb[64];
	char *s, *t, *s1;
	int i, j;

//	printf("decipher %s\n", str);

	s=str;
	while(*s && (*s>='A') && (*s<='Z')) s++;

	if(!*s && (s>str))
	{
		BSG_GenAcron(ctx, str);
		return;
	}

	i=*str;
	if(((i<'a') || (i>'z')) && ((i<'A') || (i>'Z')))return;

	s=str; t=tb;
	while(*s)
	{
		i=-1;
		if((*s>='a') && (*s<='z'))i=*s;
		if((*s>='A') && (*s<='Z'))i=*s-'A'+'a';
		if(i<0) {s++; continue; }
		*t++=i; s++;
	}
	*t=0;

	s=tb; t=buf;
	while(*s)
	{
		if(!strncmp(s, "tion", 4))
			{ s+=4; strcpy(t, "SUn"); t+=3; }
		if(!strncmp(s, "sion", 4))
			{ s+=4; strcpy(t, "SUn"); t+=3; }

		if(!strcmp(s, "ge"))
			{ s+=2; strcpy(t, "j_"); t+=2; }

		s1=BSG_SpellTrigraph(s);
		if(s1) { strcpy(t, s1); t+=strlen(t); s+=3; continue; }

		s1=BSG_SpellDigraph(s);
		if(s1) { strcpy(t, s1); t+=strlen(t); s+=2; continue; }

		s1=BSG_SpellMonograph(s);
		if(s1) { strcpy(t, s1); t+=strlen(t); s++; continue; }

		if(BSG_SpellConsonant(s))
		{
			*t++=*s++;

			if(BSG_SpellConsonant(s) && !s[1])
			{
				*t++=*s++;
//				*t++='_';
				continue;

			}
//			if(*s=='y') { *t++=='E'; *s++; }

			continue;
		}

		if(BSG_SpellCheckCE(s+1))
		{
			i=*s++;
			if(i=='a')i='A';
			if(i=='e')i='E';
			if(i=='i')i='I';
			if(i=='o')i='O';
			if(i=='u')i='U';
			*t++=i;
			*t++=*s++;	//consonant
			s++;		//e
			continue;
		}

		if(BSG_SpellCheckCV(s+1))
		{
			i=*s++;
			if(i=='a')i='A';
			if(i=='e')i='E';
			if(i=='i')i='I';
			if(i=='o')i='O';
			if(i=='u')i='U';
			*t++=i;
			continue;
		}
		*t++=*s++;
	}
	*t=0;

	sprintf(tb, "*%s", buf);
	BSG_PushWord(ctx, tb);

//	printf("emit %s\n", tb);
}
