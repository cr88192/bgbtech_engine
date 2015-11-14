#include <bgbccc.h>

int BGBJP_GetLinenum()
	{ return(BGBCP_GetLinenum()); }

char *BGBJP_GetFilename()
	{ return(BGBCP_GetFilename()); }

int BGBJP_CalcLinenum(char *se)
	{ return(BGBCP_CalcLinenum(se)); }

int BGBJP_SetLinenum(char *fname, char *base, int num)
	{ return(BGBCP_SetLinenum(fname, base, num)); }

int BGBJP_PushLinenum()
	{ return(BGBCP_PushLinenum()); }

int BGBJP_PopLinenum()
	{ return(BGBCP_PopLinenum()); }

int BGBJP_SkimLinenum(char *s, char **rfn, int *rln)
	{ return(BGBCP_SkimLinenum(s, rfn, rln)); }

char *BGBJP_GetLastFileName()
	{ return(BGBCP_GetLastFileName()); }

int BGBJP_GetLastLineNumber()
	{ return(BGBCP_GetLastLineNumber()); }

char *BGBJP_EatWhiteOnly(char *s)
	{ return(BGBCP_EatWhiteOnly(s)); }

char *BGBJP_EatWhiteOnly2(char *s)
	{ return(BGBCP_EatWhiteOnly2(s)); }

char *BGBJP_EatWhite(char *s)
	{ return(BGBCP_EatWhite(s)); }

int BGBJP_IsWhiteOnly(char *s)
	{ return(BGBCP_IsWhiteOnly(s)); }

int BGBJP_IsLineBreak(char *se)
	{ return(BGBCP_IsLineBreak(se)); }

int BGBJP_OpChar(int c)
	{ return(BGBCP_OpChar(c)); }

int BGBJP_PeekChar(char **str)
	{ return(BGBCP_PeekChar(str)); }

int BGBJP_ParseChar(char **str)
	{ return(BGBCP_ParseChar(str)); }

int BGBJP_EmitChar(char **str, int j)
	{ return(BGBCP_EmitChar(str, j)); }

int BGBJP_NameChar(int c)
{
	if(BGBCP_NameChar(c))return(1);
	return(0);
}


char *BGBJP_Token(char *s, char *b, int *ty)
	{ return(BGBCP_TokenI(s, b, ty, BGBCC_LANG_JAVA, 256)); }
void BGBJP_FlushToken(char *s)
	{ }

char *BGBJP_TokenGen(char *s, char *b, int *ty)
{
	return(BGBCP_TokenGen(s, b, ty));
}

