#include <stdio.h>
#include <stdlib.h>
#include <string.h>

//#ifdef linux
#if 0
#include <readline/readline.h>
#include <readline/history.h>
#endif

#include <time.h>

#include <bgbsvm.h>

// extern int bsvm_opcnt;

#ifdef MSVC
__declspec(dllexport) void bsvm_foo()
{
}
#endif

dyt BSVM_LoadScriptObj(char *name, dyt obj)
{
	char tb[256];
	FILE *fd;
	dyt t;
	char *s, *buf;
	byte buf1[4096], *ip;
	int sz;

	BSVM_Init();

	fd=fopen(name, "rb");
	if(!fd)return(NULL);
	fseek(fd, 0, 2);
	sz=ftell(fd);

	buf=malloc(sz);
	fseek(fd, 0, 0);
	fread(buf, 1, sz, fd);
	fclose(fd);

#if 0
	BSVM_Parse_SetLinenum("<eval-str>", buf, 1);
	s=buf;
	t=BSVM_Parse_Block(&s);
	ip=BSVM_DSOP_FlattenBuffer(buf1, t);

	sprintf(tb, "%s.dso", name);
	fd=fopen(tb, "wb");
	fwrite(buf1, 1, ip-buf1, fd);
	fclose(fd);
#endif

	t=BSVM_EvalStringObj(buf, obj);
	free(buf);

	return(t);
}

int main(int argc, char *argv[])
{
	char buf[256];
	char *s;
	dyt a, t;
	int i, it, dt;
	float dtf;

	dy_init(&argc);

//	BSVM_TY_Init();
	BSVM_Init();

	for(i=1; i<argc; i++)
		BSVM_LoadScriptObj(argv[i], BGBDY_NULL);

//	a=BSVM_TY_Cons(BSVM_TY_HashSymbol("a"),
//		BSVM_TY_Cons(BSVM_TY_Float(-3.225), BSVM_TY_HashSymbol("c")));
//	a=BSVM_ParseS_FromString("(foo (bar baz:) (alpha beta gamma delta) 3 4.0 \"yo\" : dude)");

//	BSVM_TY_PrintTC(a);
//	printf("\n");

//	BSVM_SVM_EvalString("(print \"tst \" (* (+ 2 3) 5) 'foo 'bar \" baz\n\")");

//	BSVM_GC_PrintStats();

	while(1)
	{
//#ifndef linux
#if 1
		printf("> ");
		memset(buf, 0, 256);
		fgets(buf, 255, stdin);
		s=buf;
#endif

//#ifdef linux
#if 0
		s=readline("> ");
#endif

		while(*s && (*s<=' '))s++;
		if(!*s)continue;

//#ifdef linux
#if 0
		add_history(s);
		strcpy(buf, s);
#endif

		strcat(buf, ";;");
//		free(s);

		BSVMP_SetLinenum("<repl>", buf, 1);

		it=clock();
		BSVM_SetOpCnt(0);

		t=BSVM_EvalString(buf);

//		s=buf;
//		t=BSVM_Parse_BlockStatement2(&s);

//		BSVM_TY_PrintLN(t);
		dyPrint(t);
		printf("\n");

		dt=clock()-it;
		if(dt)
		{
			dtf=(dt*1000.0)/CLOCKS_PER_SEC;
			printf("eval took %gms, %g MIPS\n",
				dtf, BSVM_GetOpCnt()/(dtf*1000.0));
		}

		if(t==dykeyword("quit"))
			break;

//		BSVM_GC_CheckRef(t);
//		BSVM_GC_PrintStats();
	}

//	printf("VM End\n");

	return(0);
}
