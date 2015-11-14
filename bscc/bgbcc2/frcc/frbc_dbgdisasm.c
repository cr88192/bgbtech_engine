#include <bgbccc.h>

struct dbgdisop_s {
char *name;
char *pat;
};

/* 
 * 0-9, A-F, literal hex values.
 * a: Address
 * b: Binary Operator
 * c: Const
 * d: Dest Reg
 
 * g: Global Index
 
 * i: Index (Array/Generic)
 * j: Compare Operator
 * k: Const/Literal (Depends On Operator)
 * l: Literal Index
 
 * s: Source Reg A
 * t: Source Reg B
 * u: Unary Operator
 
 * W: W-Factor
 * X: X (Placeholder)
 * Y: Y-Factor
 * Z: Z-Factor (Type)
 */

struct dbgdisop_s bgbcc_frbc_ops[]=
{
{"BINOP.I",			"00bdst"},
{"BINOP.L",			"01bdst"},
{"BINOP.F",			"02bdst"},
{"BINOP.D",			"03bdst"},
{"BINOP.IC",		"04bdsccc"},
{"BINOP.LC",		"05bdsccc"},
{"BINOP.FC",		"06bdslll"},
{"BINOP.DC",		"07bdslll"},
{"UNOP.I",			"08uYds"},
{"UNOP.L",			"09uYds"},
{"UNOP.F",			"0AuYds"},
{"UNOP.D",			"0BuYds"},
{"UNOP.IC",			"0CuYdkkk"},
{"UNOP.LC",			"0DuYdkkk"},
{"UNOP.FC",			"0EuYdkkk"},
{"UNOP.DC",			"0FuYdkkk"},
{"CMP.I",			"10jdst"},
{"CMP.L",			"11jdst"},
{"CMP.F",			"12jdst"},
{"CMP.D",			"13jdst"},
{"CMP.IC",			"14jdskkk"},
{"CMP.LC",			"15jdskkk"},
{"CMP.FC",			"16jdskkk"},
{"CMP.DC",			"17jdskkk"},
{"JCMP.I",			"18jYstaaaa"},
{"JCMP.L",			"19jYstaaaa"},
{"JCMP.F",			"1AjYstaaaa"},
{"JCMP.D",			"1BjYstaaaa"},
{"JCMP.IC",			"1CjYskkkaaaa"},
{"JCMP.LC",			"1DjYskkkaaaa"},
{"JCMP.FC",			"1EjYskkkaaaa"},
{"JCMP.DC",			"1FjYskkkaaaa"},
{"JMP",				"20aaaa"},
{"JMPW",			"21aaaaaaaa"},
{"LDVAR",			"22Wdiiii"},
{"STVAR",			"23Wsiiii"},
{"LDGVAR",			"24Wdgggg"},
{"STGVAR",			"25Wsgggg"},
{"LDCONST",			"26Wdllll"},
{"LDCONSTW",		"27ZYddllllllll"},
{"CONV",			"28zYds"},
{"LDAGVAR",			"29Wdgggg"},
{"CONV.I",			"2AzYds"},
{"CONV.L",			"2BzYds"},
{"SHRMASK.I",		"2CYdsccc"},
{"SHRMASK.L",		"2DYdsccc"},
{"SHLMASK.I",		"2EYdsccc"},
{"SHLMASK.L",		"2FYdsccc"},
{"JCMP.RZI",		"30jsaaaa"},
{"JCMP.RZL",		"31jsaaaa"},
{"JCMP.RZF",		"32jsaaaa"},
{"JCMP.RZD",		"33jsaaaa"},
{"JCMP.AZI",		"34jsaaaa"},
{"JCMP.AZL",		"35jsaaaa"},
{"JCMP.AZF",		"36jsaaaa"},
{"JCMP.AZD",		"37jsaaaa"},
{"JCMP.LZI",		"38jsaaaa"},
{"JCMP.LZL",		"39jsaaaa"},
{"JCMP.LZF",		"3Ajsaaaa"},
{"JCMP.LZD",		"3Bjsaaaa"},
{"JCMP.RZP",		"3Cjsaaaa"},
{"JCMP.AZP",		"3Djsaaaa"},
{"JCMP.LZP",		"3Ejsaaaa"},
{"JCMP.P",			"3FjYstaaaa"},
{"LOADINDEX.I",		"40Ydst"},
{"LOADINDEX.L",		"41Ydst"},
{"LOADINDEX.F",		"42Ydst"},
{"LOADINDEX.D",		"43Ydst"},
{"LOADINDEX.SB",	"44Ydst"},
{"LOADINDEX.UB",	"45Ydst"},
{"LOADINDEX.SS",	"46Ydst"},
{"LOADINDEX.US",	"47Ydst"},
{"LOADINDEX.P",		"48Ydst"},
{"LOADINDEX.IC",	"49Ydsccc"},
{"LOADINDEX.LC",	"4AYdsccc"},
{"LOADINDEX.FC",	"4BYdsccc"},
{"LOADINDEX.DC",	"4CYdsccc"},
{"LOADINDEX.SBC",	"4DYdsccc"},
{"LOADINDEX.SSC",	"4EYdsccc"},
{"LOADINDEX.PC",	"4FYdsccc"},
{"STOREINDEX.I",	"50Ydst"},
{"STOREINDEX.L",	"51Ydst"},
{"STOREINDEX.F",	"52Ydst"},
{"STOREINDEX.D",	"53Ydst"},
{"STOREINDEX.B",	"54Ydst"},
{"STOREINDEX.S",	"55Ydst"},
{"STOREINDEX.P",	"56Ydst"},
{"LOADINDEX.SBC",	"57Ydsccc"},
{"LOADINDEX.SSC",	"58Ydsccc"},
{"STOREINDEX.IC",	"59Ydsccc"},
{"STOREINDEX.LC",	"5AYdsccc"},
{"STOREINDEX.FC",	"5BYdsccc"},
{"STOREINDEX.DC",	"5CYdsccc"},
{"STOREINDEX.BC",	"5DYdsccc"},
{"STOREINDEX.SC",	"5EYdsccc"},
{"STOREINDEX.PC",	"5FYdsccc"},
{"LEA.I",			"60Ydst"},
{"LEA.L",			"61Ydst"},
{"LEA.F",			"62Ydst"},
{"LEA.D",			"63Ydst"},
{"LEA.B",			"64Ydst"},
{"LEA.S",			"65Ydst"},
{"LEA.P",			"66Ydst"},
{"LEALVAR.V",		"67Ydst"},
{"LEA.IC",			"68Ydsccc"},
{"LEA.LC",			"69Ydsccc"},
{"LEA.FC",			"6AYdsccc"},
{"LEA.DC",			"6BYdsccc"},
{"LEA.BC",			"6CYdsccc"},
{"LEA.SC",			"6DYdsccc"},
{"LEA.PC",			"6EYdsccc"},
{"LEALVAR.VC",		"6FYdsccc"},
{"CALL",			"70ggggggNN"},
{"CALLP",			"71YsNN"},
{"RET.V",			"72"},
{"RET",				"73Ws"},
{"RETW",			"74ZYss"},
{"LOADSLOT",		"75ZYdsllllii"},
{"STORESLOT",		"76ZYstllllii"},
{"INITOBJ",			"77Wdllll"},
{"CSRV.V",			"78"},
{"CSRV",			"79Ws"},
{"CSRVW",			"7AZYss"},
{"INITARR",			"7BZdcccc"},
{"LABEL",			"7C"},
{"LABEL2",			"7Dllllll"},
{"NOP2",			"7EPP"},
{"NOP",				"7F"},
{"BINOP2",			"80ZYbdst"},
{"BINOP2.C",		"81ZYbdslll"},
{"UNOP2",			"82ZYuds0"},
{"UNOP2.C",			"83ZYudllll"},
{"CMP2",			"84ZYjdst"},
{"CMP2.C",			"85ZYjdslll"},
{"LDAVAR",			"86Ydss"},
{"COPYOBJ",			"87Ydslll"},
{"DROPOBJ",			"88slll"},
{"SIZEOF.I",		"89Ydllll"},
{"OFFSETOF.I",		"8AYdllllii"},
{"RET.C",			"8BZkkk"},
{"INITOBJARR",		"8CWdllllcccc"},
{"CALL2",			"8DNggg"},
{"MOV",				"8EZYds"},

{"BINOPW.I",		"E100bYddsstt"},
{"BINOPW.L",		"E101bYddsstt"},
{"BINOPW.F",		"E102bYddsstt"},
{"BINOPW.D",		"E103bYddsstt"},
{"BINOPW.IC",		"E104bYddssllllll"},
{"BINOPW.LC",		"E105bYddssllllll"},
{"BINOPW.FC",		"E106bYddssllllll"},
{"BINOPW.DC",		"E107bYddssllllll"},
{"UNOPW.I",			"E108bYddss"},
{"UNOPW.L",			"E109bYddss"},
{"UNOPW.F",			"E10AbYddss"},
{"UNOPW.D",			"E10BbYddss"},
{"UNOPW.IC",		"E10CbYddllllll"},
{"UNOPW.LC",		"E10DbYddllllll"},
{"UNOPW.FC",		"E10EbYddllllll"},
{"UNOPW.DC",		"E10FbYddllllll"},
{"CMPW.I",			"E110jYddsstt"},
{"CMPW.L",			"E111jYddsstt"},
{"CMPW.F",			"E112jYddsstt"},
{"CMPW.D",			"E113jYddsstt"},
{"CMPW.IC",			"E114jYddssllllll"},
{"CMPW.LC",			"E115jYddssllllll"},
{"CMPW.FC",			"E116jYddssllllll"},
{"CMPW.DC",			"E117jYddssllllll"},
{"JCMPW.I",			"E118jYddssttaaaaaaaa"},
{"JCMPW.L",			"E119jYddssttaaaaaaaa"},
{"JCMPW.F",			"E11AjYddssttaaaaaaaa"},
{"JCMPW.D",			"E11BjYddssttaaaaaaaa"},
{"JCMPW.IC",		"E11CjYddssllllllaaaaaaaa"},
{"JCMPW.LC",		"E11DjYddssllllllaaaaaaaa"},
{"JCMPW.FC",		"E11EjYddssllllllaaaaaaaa"},
{"JCMPW.DC",		"E11FjYddssllllllaaaaaaaa"},
{"SIZEOFW.I",		"E120ddllllll"},
{"OFFSETOFW.I",		"E121ddlllllliiii"},
{"LOADSLOTW.I",		"E122ZYddsslllllliiii"},
{"STORESLOTW.I",	"E123ZYssttlllllliiii"},
{"LOADSLOTAW.I",	"E124ZYddsslllllliiii"},
{"INITOBJW",		"E125ZYddllllll"},
{"COPYOBJW",		"E126Y0ddssllllll"},
{"DROPOBJW",		"E127Y0ssllllll"},
{"LOADINDEXW",		"E128ZYddsstt"},
{"STOREINDEXW",		"E129ZYddsstt"},
{"LOADINDEXAW",		"E12AZYddsstt"},
{"LEAW",			"E12BZYddsstt"},
{"LOADINDEXW.C",	"E12CZYddssiiiiiiii"},
{"STOREINDEXW.C",	"E12DZYddssiiiiiiii"},
{"LOADINDEXAW.C",	"E12EZYddssiiiiiiii"},
{"LEAW.C",			"E12FZYddssiiiiiiii"},

{NULL,				NULL}
};

int BGBCC_FrBC_DbgDisAsm_CheckMatch(
	BGBCC_FrCC_State *ctx, byte *css, char *pat)
{
	byte *cs;
	char *s;
	
	cs=css; s=pat;
	while(*s)
	{
		if((s[0]>='0') && (s[0]<='9'))
		{
			if((((*cs)>>4)&15)!=(s[0]-'0'))
				break;
		}
		if((s[0]>='A') && (s[0]<='F'))
		{
			if((((*cs)>>4)&15)!=(s[0]-'A'+10))
				break;
		}

		if(!s[1])
			break;

		if((s[1]>='0') && (s[1]<='9'))
		{
			if(((*cs)&15)!=(s[1]-'0'))
				break;
		}
		if((s[1]>='A') && (s[1]<='F'))
		{
			if(((*cs)&15)!=(s[1]-'A'+10))
				break;
		}
		
		cs++; s+=2;
	}
	
	if(!(*s))
		return(1);

	return(0);
}

int BGBCC_FrBC_DbgDisAsm_LookupMatch(
	BGBCC_FrCC_State *ctx, byte *cs)
{
	int i;
	
	for(i=0; bgbcc_frbc_ops[i].name; i++)
	{
		if(BGBCC_FrBC_DbgDisAsm_CheckMatch(
				ctx, cs, bgbcc_frbc_ops[i].pat))
			{ return(i); }
	}
	return(-1);
}

int BGBCC_FrBC_DbgDisAsm_GetArgsOpLen(
	BGBCC_FrCC_State *ctx, byte *css, int na)
{
	byte *cs;
	int i;

	cs=css;
	for(i=0; i<na; i++)
	{
		if((*cs>=0x00) && (*cs<0xA0))
			{ cs++; continue; }
		if((*cs>=0xA0) && (*cs<0xB0))
			{ cs+=2; continue; }
		if((*cs>=0xD0) && (*cs<=0xFF))
			{ cs++; continue; }

		switch(*cs)
		{
		case 0xB0: case 0xB1:
			cs+=2; break;
		case 0xB2: case 0xB3:
			cs+=3; break;
		case 0xB4: case 0xB5:
		case 0xB6: case 0xB7:
		case 0xB8: case 0xB9:
		case 0xBA: case 0xBB:
		case 0xBC: case 0xBD:
		case 0xBE:
			cs+=4; break;
		
		case 0xBF:
			cs+=3; break;

		case 0xCF:
			cs+=2; break;
		default:
			break;
		}
	}
	return(cs-css);
}

int BGBCC_FrBC_DbgDisAsm_GetPatOpLen(
	BGBCC_FrCC_State *ctx, byte *css, char *pat)
{
	byte *cs;
	char *s;
	int i, j, na;
	
	cs=css; s=pat; na=-1;
	while(*s)
	{
		if((s[0]=='P') && (s[1]=='P'))
		{
			cs=css+(*cs);
			s+=2;
			continue;
		}

		if((s[0]=='N') && (s[1]=='N'))
		{
			i=*cs++; s+=2;
			na=i;
//			j=BGBCC_FrBC_DbgDisAsm_GetArgsOpLen(ctx, cs, i);
//			cs+=j;
			continue;
		}

		if(s[0]=='N')
			{ na=(cs[0]>>4)&15; }

		cs++; s+=2;
	}
	
	if(na>=0)
	{
		j=BGBCC_FrBC_DbgDisAsm_GetArgsOpLen(ctx, cs, na);
		cs+=j;
	}
	
	if(*s)
		return(-1);
	
	return(cs-css);
}

int BGBCC_FrBC_DbgDisAsm_DecodeOpArgsList(
	BGBCC_FrCC_State *ctx, byte *css, byte **rcs, int na)
{
	static const char *ilpfd="ILPFDA";
	byte *cs;
	int i, j, k;

	if(na<=0)
		return(0);

	printf("\n\t[ ");

	cs=*rcs;
	for(i=0; i<na; i++)
	{
		if((*cs>=0x00) && (*cs<0x10))
			{ printf("R%dI ", (*cs++)&15); continue; }
		if((*cs>=0x10) && (*cs<0x20))
			{ printf("R%dL ", (*cs++)&15); continue; }
		if((*cs>=0x20) && (*cs<0x30))
			{ printf("R%dF ", (*cs++)&15); continue; }
		if((*cs>=0x30) && (*cs<0x40))
			{ printf("R%dD ", (*cs++)&15); continue; }
		if((*cs>=0x40) && (*cs<0x50))
			{ printf("R%dP ", (*cs++)&15); continue; }

		if((*cs>=0x50) && (*cs<0x60))
			{ printf("L%dI ", (*cs++)&15); continue; }
		if((*cs>=0x60) && (*cs<0x70))
			{ printf("L%dL ", (*cs++)&15); continue; }
		if((*cs>=0x70) && (*cs<0x80))
			{ printf("L%dF ", (*cs++)&15); continue; }
		if((*cs>=0x80) && (*cs<0x90))
			{ printf("L%dD ", (*cs++)&15); continue; }
		if((*cs>=0x90) && (*cs<0xA0))
			{ printf("L%dP ", (*cs++)&15); continue; }

		if((*cs>=0xD0) && (*cs<0xE0))
			{ printf("A%dI ", (*cs++)&15); continue; }
		if((*cs>=0xE0) && (*cs<0xF0))
			{ printf("A%dD ", (*cs++)&15); continue; }
		if((*cs>=0xF0) && (*cs<=0xFF))
			{ printf("A%dP ", (*cs++)&15); continue; }
			
		switch(*cs)
		{
		case 0xA0: printf("R%dI ", cs[1]); cs+=2; break;
		case 0xA1: printf("R%dL ", cs[1]); cs+=2; break;
		case 0xA2: printf("R%dF ", cs[1]); cs+=2; break;
		case 0xA3: printf("R%dD ", cs[1]); cs+=2; break;
		case 0xA4: printf("R%dP ", cs[1]); cs+=2; break;

		case 0xA5: printf("A%dI ", cs[1]); cs+=2; break;
		case 0xA6: printf("A%dL ", cs[1]); cs+=2; break;
		case 0xA7: printf("A%dF ", cs[1]); cs+=2; break;
		case 0xA8: printf("A%dD ", cs[1]); cs+=2; break;
		case 0xA9: printf("A%dP ", cs[1]); cs+=2; break;

		case 0xAA: printf("L%dI ", cs[1]); cs+=2; break;
		case 0xAB: printf("L%dL ", cs[1]); cs+=2; break;
		case 0xAC: printf("L%dF ", cs[1]); cs+=2; break;
		case 0xAD: printf("L%dD ", cs[1]); cs+=2; break;
		case 0xAE: printf("L%dP ", cs[1]); cs+=2; break;

		case 0xAF: printf("&L%d ", cs[1]); cs+=2; break;
		case 0xCF: printf("&A%d ", cs[1]); cs+=2; break;
		
		case 0xB0: case 0xB1:
			printf("C:%d ", (signed char)(cs[1]));
			cs+=2; break;
		case 0xB2: case 0xB3:
			printf("C:%d ", (signed short)((cs[1]<<8)|(cs[2])));
			cs+=3; break;

		case 0xB4: case 0xB5:
		case 0xB6: case 0xB7:
		case 0xB8:
			printf("L:%d%c ",
				(cs[1]<<16)|(cs[2]<<8)|(cs[3]),
				ilpfd[(*cs)-0xB4]);
			cs+=4; break;
		
		case 0xB9: case 0xBA: case 0xBB:
		case 0xBC: case 0xBD: case 0xBE:
			printf("G:%d%c ",
				(cs[1]<<16)|(cs[2]<<8)|(cs[3]),
				ilpfd[(*cs)-0xB9]);
			cs+=4; break;

		case 0xBF:
			j=(cs[1]<<8)|(cs[2]); cs+=3;
			k=(j>>12)&15; j=j&4095;
			switch(k)
			{
			case 4: case 5: case 6: case 7: case 8:
				printf("L:%d%c ", j, ilpfd[k-4]); break;
			case 9: case 10: case 11:
			case 12: case 13: case 14:
				printf("G:%d%c ", j, ilpfd[k-9]); break;
			default:
				printf("T%d:%d ", k, j);
				break;
			}
			break;
 
		default:
			printf("? ");
			break;
		}
	}

	printf("] ");
	
	*rcs=cs;
	return(0);
}

int BGBCC_FrBC_DbgDisAsm_DecodeOp(
	BGBCC_FrCC_State *ctx, byte *css, byte **rcs)
{
	static char *binop[16]={
		  "ADD",  "SUB",  "MUL",  "DIV",
		  "MOD",  "AND",   "OR",  "XOR",
		  "SHL",  "SHR", "SHRR", "MULH",
		"UMULH", "UDIV",    "-",    "-"};
	static char *unop[16]={
		"MOV1", "NEG1", "NOT1", "LNOT1",
		"INC1", "DEC1",   "-1",    "-1",
		"MOV2", "NEG2", "NOT2", "LNOT2",
		"INC2", "DEC2",   "-2",    "-2"};
	static char *jmpop[16]={
		"EQ1", "NE1", "LT1", "GT1",
		"LE1", "GE1", "AL1", "NV1",
		"EQ2", "NE2", "LT2", "GT2",
		"LE2", "GE2", "AL2", "NV2"};
	static char *yop[16]={
		"rRR", "rRA", "rRL", "rAR",
		"rAA", "rAL", "rLR", "rLA",
		"rLL", "ARR", "LRR", "LRA",
		"LAR", "LRL", "LLR", "LLL"};

	byte *cs;
	char *opn, *pat, *s;
	int idx, len, z, y, w, nr, na;
	int i, j, k;
	
	cs=*rcs;
	idx=BGBCC_FrBC_DbgDisAsm_LookupMatch(ctx, cs);
	if(idx<0)return(-1);
	
	opn=bgbcc_frbc_ops[idx].name;
	pat=bgbcc_frbc_ops[idx].pat;
	len=BGBCC_FrBC_DbgDisAsm_GetPatOpLen(ctx, cs, pat);
	if(len<=0)return(-2);
		
	printf("%04X: ", cs-css);
	for(i=0; i<len; i++)
		{ printf("%02X", cs[i]); }
	for(; i<12; i++)
		{ printf("  "); }
	printf(" %s ", opn);

	s=pat; na=-1;
	while(*s)
	{
		if(((s[0]>='0') && (s[0]<='9')) ||
			((s[0]>='A') && (s[0]<='F')))
		{
			if(((s[1]>='0') && (s[1]<='9')) ||
				((s[1]>='A') && (s[1]<='F')))
			{
				cs++; s+=2;
				continue;
			}
		}

		if((s[0]=='N') && (s[1]=='N'))
		{
			i=*cs++; s+=2;
			na=i;
//			BGBCC_FrBC_DbgDisAsm_DecodeOpArgsList(ctx, css, &cs, i);
			continue;
		}
		
		if((s[0]=='d') && (s[1]=='d'))
			{ printf("RdW:%d ", *cs++); s+=2; continue; }
		if((s[0]=='s') && (s[1]=='s'))
			{ printf("RsW:%d ", *cs++); s+=2; continue; }
		if((s[0]=='t') && (s[1]=='t'))
			{ printf("RtW:%d ", *cs++); s+=2; continue; }

		if(s[0]=='Y') { y=((*cs)>>4)&15; printf("Y:%s ", yop[y]); }
		if(s[0]=='Z') { z=((*cs)>>4)&15; printf("Z:%d ", z); }
		if(s[0]=='W') { w=((*cs)>>4)&15; printf("W:%d ", w); }

		if(s[0]=='b')
			{ printf("Ob:%s ", binop[((*cs)>>4)&15]); }
		if(s[0]=='u')
			{ printf("Ou:%s ", unop[((*cs)>>4)&15]); }
		if(s[0]=='j')
			{ printf("Oj:%s ", jmpop[((*cs)>>4)&15]); }

//		if((s[0]=='d') || (s[0]=='s') || (s[0]=='t'))
//			{ printf("R:%d ", ((*cs)>>4)&15); }
		if(s[0]=='d')
			{ printf("Rd:%d ", ((*cs)>>4)&15); }
		if(s[0]=='s')
			{ printf("Rs:%d ", ((*cs)>>4)&15); }
		if(s[0]=='t')
			{ printf("Rt:%d ", ((*cs)>>4)&15); }

		if(s[0]=='N')
			{ na=((*cs)>>4)&15; }

		if(s[0]=='a')
		{
			i=((*cs)&128)?-1:0;
			while(*s && (*s=='a'))
				{ i=(i<<8)|(*cs++); s+=2; }
			printf("A:%04X ", (cs-css)+i);
			continue;
		}

		if(s[0]=='c')
		{
			i=((*cs)&128)?-1:0;
			while(*s && (*s=='c'))
				{ i=(i<<8)|(*cs++); s+=2; }
			printf("C:%d", i);
			continue;
		}

		if(s[0]=='l')
		{
			i=0;
			while(*s && (*s=='l'))
				{ i=(i<<8)|(*cs++); s+=2; }
			printf("L:%d ", i);
			continue;
		}

		if(s[0]=='i')
		{
			i=((*cs)&128)?-1:0;
			while(*s && (*s=='i'))
				{ i=(i<<8)|(*cs++); s+=2; }
			printf("I:%d ", i);
			continue;
		}

		if(s[0]=='k')
		{
			i=((*cs)&128)?-1:0;
			while(*s && (*s=='k'))
				{ i=(i<<8)|(*cs++); s+=2; }
			printf("K:%d ", i);
			continue;
		}

		if(s[0]=='g')
		{
			i=((*cs)&128)?-1:0;
			while(*s && (*s=='g'))
				{ i=(i<<8)|(*cs++); s+=2; }
			printf("G:%d ", i);
			continue;
		}

		if(s[1]=='Y') { y=(*cs)&15; printf("Y:%s ", yop[y]); }
		if(s[1]=='Z') { z=(*cs)&15; printf("Z:%d ", z); }
		if(s[1]=='W') { w=(*cs)&15; printf("W:%d ", w); }

		if(s[1]=='b')
			{ printf("Ob:%s ", binop[(*cs)&15]); }
		if(s[1]=='u')
			{ printf("Ou:%s ", unop[(*cs)&15]); }
		if(s[1]=='j')
			{ printf("Oj:%s ", jmpop[(*cs)&15]); }

//		if((s[1]=='d') || (s[1]=='s') || (s[1]=='t'))
//			{ printf("R:%d ", (*cs)&15); }

		if(s[1]=='d')
			{ printf("Rd:%d ", (*cs)&15); }
		if(s[1]=='s')
			{ printf("Rs:%d ", (*cs)&15); }
		if(s[1]=='t')
			{ printf("Rt:%d ", (*cs)&15); }

		if(s[1]=='c')
		{
			i=(*cs++)&15; s+=2;
			i=(i<<28)>>28;
			while(*s && (*s=='c'))
				{ i=(i<<8)|(*cs++); s+=2; }
			printf("C:%d ", i);
			continue;
		}

		if(s[1]=='l')
		{
			i=(*cs++)&15; s+=2;
//			i=(i<<28)>>28;
			while(*s && (*s=='l'))
				{ i=(i<<8)|(*cs++); s+=2; }
			printf("L:%d ", i);
			continue;
		}

		if(s[1]=='i')
		{
			i=(*cs++)&15; s+=2;
			i=(i<<28)>>28;
			while(*s && (*s=='i'))
				{ i=(i<<8)|(*cs++); s+=2; }
			printf("I:%d ", i);
			continue;
		}

		if(s[1]=='k')
		{
			i=(*cs++)&15; s+=2;
			i=(i<<28)>>28;
			while(*s && (*s=='k'))
				{ i=(i<<8)|(*cs++); s+=2; }
			printf("K:%d ", i);
			continue;
		}

		if(s[1]=='g')
		{
			i=(*cs++)&15; s+=2;
			i=(i<<28)>>28;
			while(*s && (*s=='g'))
				{ i=(i<<8)|(*cs++); s+=2; }
			printf("G:%d ", i);
			continue;
		}

		cs++; s+=2;
	}

	if(na>=0)
	{
		BGBCC_FrBC_DbgDisAsm_DecodeOpArgsList(ctx, css, &cs, na);
	}
	
	printf("\n");
	
//	*rcs=cs;
	*rcs=(*rcs)+len;
	return(0);
}

void BGBCC_FrBC_DbgDisAsmFunction(BGBCC_FrCC_State *ctx,
	BGBCC_FrCC_LiteralInfo *obj)
{
	byte *cs, *css, *cse;
	int i;

	css=obj->decl->text;
	cse=css+obj->decl->sz_text;
	cs=css;
	
	if(cs>=cse)
		return;

	printf("DbgDisAsm %s%s\n", obj->decl->name, obj->decl->sig);

	while(cs<cse)
	{
		i=BGBCC_FrBC_DbgDisAsm_DecodeOp(ctx, css, &cs);
		if(i<0)
		{
			printf("Decode Break/Error %d\n", i);
			break;
		}
	}
	
	fgetc(stdin);
//	*(int *)-1=-1;
}
