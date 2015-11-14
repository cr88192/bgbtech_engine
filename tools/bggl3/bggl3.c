#include "bggl3.h"

#include <readline/readline.h>
#include <readline/history.h>

static int quit=0;

elem bggl3_quit(BGGL3_State *ctx, elem args)
{
	quit=1;
	return(NULL);
}

int main(int argc, char *argv[])
{
	char buf[256], *s;
	BGGL3_State *ctx;
	elem a;
	int i, mi, ni;

	BGGL3_InitHeap();
	BGGL3_InitBuiltin();

//	BGGL3_BindValue("quit", "help",
//		BGGL3_HashString("Type {quit} to quit"));
	BGGL3_AddBuiltin("quit", "quit", &bggl3_quit, 0);


	ctx=BGGL3_NewContext();

	mi=0; ni=0;
	for(i=1; i<argc; i++)
	{
		if(argv[i][0]=='-')
		{
			if(!strcmp(argv[i], "-i"))mi=1;
		}else
		{
			ni=1;
			BGGL3_LoadFile(ctx, argv[1]);
		}
	}

	if(ni && !mi)quit=1;

	while(!quit)
	{
//		memset(buf, 0, 256);
//		fgets(buf, 255, stdin);

		s=readline("> ");
		if(!s || !*s)continue;
		add_history(s);

		a=BGGL3_EvalString(ctx, "repl", s);
		BGGL3_Print(a);
		printf("\n");
	}
}
