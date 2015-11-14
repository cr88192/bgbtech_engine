/*
DYSHell:
basically, this will be a command shell interface currently located within
BGBDY. This will manage the console buffer, which will hold previously printed
messages, and a history of prior commands.

The console will be bottom to top, so line 0 will be the most recent.
Console strings may contain UTF8.

Console Color Strings:
will be mapped 1-to-1 with string characters.
a UTF-8 codepoint may take multiple color bytes.
NULL=no formatting applied (console default).
low 4 bits: foreground color (usual 16-colors).
high 4 bits: background color (8 colors).

Console Default Color:
0x80, default background, black letters.
this value will be special and mean that the character uses whatever is the
default color scheme.

Console Effects:
Will be mapped 1:1 with console color.
NULL=no effects.
low 4 bits: effect mode;
high 4 bits: flags.
 */

#include <bgbdy.h>

DYSH_Console *DYSH_NewConsole(int width, int szcon, int szhist)
{
	DYSH_Console *tmp;
	
	tmp=gctalloc("_dysh_console_t", sizeof(DYSH_Console));
	tmp->cd=gctalloc("_dysh_consoledata_t", sizeof(DYSH_ConsoleData));
	tmp->cd->szconbuf=szcon;
	tmp->cd->width=width;
//	tmp->szhistory=szhis;

	tmp->cd->conbuf=gcalloc(szcon*sizeof(char *));
	tmp->cd->conclr=gcalloc(szcon*sizeof(byte *));
	tmp->cd->coneff=gcalloc(szcon*sizeof(byte *));
//	tmp->history=gcalloc(szhist*sizeof(char *));

	return(tmp);
}

void DYSH_FreeBuf(void *buf)
{
	if(buf)gcfree(buf);
}

char *DYSH_StrCopy(char *str)
{
	char *s;
	
	s=gcalloc(strlen(str)+1);
	strcpy(s, str);
	return(s);
}

void DYSH_PrintAddLineClr(DYSH_Console *tmp,
	char *str, byte *clr, byte *eff)
{
	int i;
	
	DYSH_FreeBuf(tmp->cd->conbuf[tmp->cd->szconbuf-1]);
	DYSH_FreeBuf(tmp->cd->conclr[tmp->cd->szconbuf-1]);
	DYSH_FreeBuf(tmp->cd->coneff[tmp->cd->szconbuf-1]);

	for(i=tmp->cd->szconbuf-1; i>0; i--)
	{
		tmp->cd->conbuf[i]=tmp->cd->conbuf[i-1];
		tmp->cd->conclr[i]=tmp->cd->conclr[i-1];
		tmp->cd->coneff[i]=tmp->cd->coneff[i-1];
	}
	
	i=strlen(str);
	tmp->cd->conbuf[i]=gcalloc(i+1);
	tmp->cd->conclr[i]=gcalloc(i+1);
	tmp->cd->coneff[i]=gcalloc(i+1);

	memcpy(tmp->cd->conbuf[i], str, i+1);
	memcpy(tmp->cd->conclr[i], clr, i+1);
	memcpy(tmp->cd->coneff[i], eff, i+1);
}

void DYSH_PrintAddLine(DYSH_Console *tmp, char *str)
{
	char strbuf[256];
	byte clrbuf[256];
	byte effbuf[256];
}

//void DYSH_ConsolePuts(DYSH_Console *con, char *str)
//{
//}