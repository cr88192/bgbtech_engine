#include <bgbgc.h>
#include <bgbdy.h>

extern int netval_float28;
extern size_t netval_irange;
extern size_t netval_frange;

extern s64 netval_imin;
extern s64 netval_imax;
extern s64 netval_iofs;

extern byte *bgbdy_vintrange;
extern byte *bgbdy_vffrange;

static char *dyll_regs32[16]=
{
 "eax",  "ecx",  "edx",  "ebx",  "ebp",  "esp",  "esi",  "edi",
 "r8d",  "r9d", "r10d", "r11d", "r12d", "r13d", "r14d", "r15d"
};
static char *dyll_regs64[16]=
{
"rax", "rcx", "rdx", "rbx", "rbp", "rsp", "rsi", "rdi",
 "r8",  "r9", "r10", "r11", "r12", "r13", "r14", "r15"
};

static char *dyll_regs_xmm[16]=
{
 "xmm0",  "xmm1",  "xmm2",  "xmm3",
 "xmm4",  "xmm5",  "xmm6",  "xmm7",
 "xmm8",  "xmm9", "xmm10", "xmm11",
"xmm12", "xmm13", "xmm14", "xmm15"
};

/** Get the 'regs=' option from the args list */
s64 DYLL_ThunkGen_GetRegsMask(char *nosv, char **args)
{
	char *s;
	int i;
	
	for(i=0; args[i]; i++)
	{
		s=args[i];
		if(!strncmp(s, "regs=", 5))
			return(atoi(s+5));
	}
	return(0);
}

/** save the register-state, but skip nosv if given */
void DYLL_ThunkGen_SaveRegs(char *nosv, char **args)
{
	int i, j, sp, spg, spx;
	s64 rm;

	rm=DYLL_ThunkGen_GetRegsMask(nosv, args);

#ifdef __i386__
	if(nosv)
	{
		for(i=0; i<8; i++)
			if(!strcmp(dyll_regs32[i], nosv))
				rm|=1<<i;
		for(i=8; i<16; i++)
			if(!strcmp(dyll_regs_xmm[i-8], nosv))
				rm|=1<<i;
	}

	spx=0;
	for(i=8; i<16; i++)
		if(!(rm&(1<<i)))
			spx+=16;
	spg=0;
	for(i=0; i<8; i++)
		if(!(rm&(1<<i)))
			spg+=4;
	sp=spg+spx;

	dyllAsmPrint("sub esp, %d\n", sp);

	j=0;
	for(i=8; i<16; i++)
		if(!(rm&(1<<i)))
	{
		dyllAsmPrint("movups [esp+%d], %s\n",
			spg+j*16, dyll_regs_xmm[i-8]);
		j++;
	}

	j=0;
	for(i=0; i<8; i++)
		if(!(rm&(1<<i)))
	{
		dyllAsmPrint("mov [esp+%d], %s\n",
			j*4, dyll_regs32[i-8]);
		j++;
	}
#endif
}

/** restore the register-state, but skip nosv if given */
void DYLL_ThunkGen_RestoreRegs(char *nosv, char **args)
{
	int i, j, sp, spg, spx;
	s64 rm;

	rm=DYLL_ThunkGen_GetRegsMask(nosv, args);

#ifdef __i386__
	if(nosv)
	{
		for(i=0; i<8; i++)
			if(!strcmp(dyll_regs32[i], nosv))
				rm|=1<<i;
		for(i=8; i<16; i++)
			if(!strcmp(dyll_regs_xmm[i-8], nosv))
				rm|=1<<i;
	}

	spx=0;
	for(i=8; i<16; i++)
		if(!(rm&(1<<i)))
			spx+=16;
	spg=0;
	for(i=0; i<8; i++)
		if(!(rm&(1<<i)))
			spg+=4;
	sp=spg+spx;

	j=0;
	for(i=8; i<16; i++)
		if(!(rm&(1<<i)))
	{
		dyllAsmPrint("movups %s, [esp+%d]\n",
			dyll_regs_xmm[i-8], spg+j*16);
		j++;
	}

	j=0;
	for(i=0; i<8; i++)
		if(!(rm&(1<<i)))
	{
		dyllAsmPrint("mov %s, [esp+%d]\n",
			dyll_regs32[i-8], j*4);
		j++;
	}

	dyllAsmPrint("add esp, %d\n", sp);
#endif
}

void *dyll_meta_classslot(char *sym, char *name, char **args)
{
	dycClass cls;
	dycSlot slot;
	void *p;

	cls=dycGetClass(args[0]);
	slot=dycIdxSlot(cls, args[1]);
	return((void *)slot);
}

void *dyll_meta_classmethod(char *sym, char *name, char **args)
{
	char tb[256];
	dycClass cls;
	dycMethod minf;
	void *p;
	char *s, *t;

	s=args[1]; t=tb;
	while(*s && (*s!='('))
		*t++=*s++;
	*t++=0;

	cls=dycGetClass(args[0]);
	minf=dycIdxMethod(cls, tb, s);
	return((void *)minf);
}

void *dyll_meta_fieldref(char *sym, char *name, char **args)
{
	dycClass cls;
	dycSlot slot;
	char *str;
	void *p, *q;

	cls=dycGetClass(args[0]);
	slot=dycIdxSlot(cls, args[1]);
	q=dycGetStaticPtr(cls, slot);

#ifdef __i386__
	if(q)
	{
		dyllAsmBegin();
		dyllAsmPrint("%s:\n", sym);
		dyllAsmPrint("mov %s, 0x%08X\n", args[2], (u32)q);
		dyllAsmPrint("ret\n");
		p=dyllAsmEnd();
		return(p);
	}
#endif

#ifdef __x86_64__
	if(q)
	{
		dyllAsmBegin();
		dyllAsmPrint("%s:\n", sym);
		dyllAsmPrint("mov %s, %ld\n", args[2], (s64)q);
		dyllAsmPrint("ret\n");
		p=dyllAsmEnd();
		return(p);
	}
#endif

	str=dyllComposeLinkMetaName2("ClassSlot", args[0], args[1]);

#ifdef __i386__
	dyllAsmBegin();
	dyllAsmPrint("%s:\n", sym);
	dyllAsmPrint("push ebp; mov ebp, esp\n");
	dyllAsmPrint("push %s; pushad\n", args[2]);

	dyllAsmPrint("push dword %s\n", str);
	dyllAsmPrint("push dword %s\n", args[3]);
	dyllAsmPrint("call dword BGBDYC_GetObjectSlotIdxPtrTH\n");
	dyllAsmPrint("mov [ebp-4], eax\n");
	dyllAsmPrint("add esp, 8\n");

	dyllAsmPrint("popad; pop %s\n", args[2]);
	dyllAsmPrint("pop ebp; ret\n");
	p=dyllAsmEnd();
	return(p);
#endif

	return(NULL);
}

void *dyll_meta_staticfieldref(char *sym, char *name, char **args)
{
	dycClass cls;
	dycSlot slot;
	void *p, *q;

	cls=dycGetClass(args[0]);
	slot=dycIdxSlot(cls, args[1]);
	q=dycGetStaticPtr(cls, slot);

#ifdef __i386__
	dyllAsmBegin();
	dyllAsmPrint("%s:\n", sym);
	dyllAsmPrint("mov %s, 0x%08X\n", args[2], (u32)q);
	dyllAsmPrint("ret\n");
	p=dyllAsmEnd();
	return(p);
#endif

	return(NULL);
}

void *dyll_meta_methodref(char *sym, char *name, char **args)
{
	char *str, *sig, *esig, *s1, *s2;
	void *p;
	int i, j;

	str=dyllComposeLinkMetaName2("ClassMethod", args[0], args[1]);

	s1=dyllGenSym();

	s2=args[1];
	while(*s2 && (*s2!='('))s2++;
	sig=s2;

	while(*s2 && (*s2!=')'))s2++;
	if(*s2==')')s2++;
	esig=s2;

#ifdef __i386__
	dyllAsmBegin();
	dyllAsmPrint("%s:\n", sym);
	dyllAsmPrint("mov %s, %s\n", args[2], s1);
	dyllAsmPrint("ret\n");
	p=dyllAsmEnd();

	if((*esig=='X') || (*esig=='U') || (*esig=='W') ||
		(*esig=='n') || (*esig=='o') || (*esig=='g'))
	{
		dyllAsmBegin();
		dyllAsmPrint("%s:\n", s1);
		dyllAsmPrint("push ebp; mov ebp, esp\n");

		j=dyllGetSigUArgsSize(sig);
		dyllAsmPrint("sub esp, %d\n", j*4);
		for(i=0; i<j; i++)
		{
			dyllAsmPrint("mov ecx, [ebp+%d]\n", 12+i*4);
			dyllAsmPrint("mov [esp+%d], ecx\n", i*4);
		}

		dyllAsmPrint("pop ecx\n");	//this
		dyllAsmPrint("push dword %s\n", str);
		dyllAsmPrint("push ecx\n");

		dyllAsmPrint("call dword dycCall\n");

		j=dyllSigQuickSize(esig);

		dyllAsmPrint("mov edx, [ebp+8]\n");

		if(j==8)
		{
			dyllAsmPrint("movq xmm0, [eax]\n");
			dyllAsmPrint("movq [edx], xmm0\n");
		}else if(j==16)
		{
			dyllAsmPrint("movdqu xmm0, [eax]\n");
			dyllAsmPrint("movdqu [edx], xmm0\n");
		}else
		{
			j=(j+3)/4;
			for(i=0; i<j; i++)
			{
				dyllAsmPrint("mov ecx, [eax+%d]\n", i*4);
				dyllAsmPrint("mov [edx+%d], ecx\n", i*4);
			}
		}

		dyllAsmPrint("mov esp, ebp\n");
		dyllAsmPrint("pop ebp; ret\n");
		dyllAsmEnd();

		return(p);
	}

	dyllAsmBegin();
	dyllAsmPrint("%s:\n", s1);
	dyllAsmPrint("pop ecx\n");	//ret addr
	dyllAsmPrint("pop eax\n");	//this
	dyllAsmPrint("push dword %s\n", str);
	dyllAsmPrint("push eax\n");
	dyllAsmPrint("push ecx\n");

	switch(*esig)
	{
	case 'a': case 'h': case 'b': case 'c':
		dyllAsmPrint("jmp dword dycCalli\n"); break;
	case 's': case 't': case 'w':
		dyllAsmPrint("jmp dword dycCalli\n"); break;

	case 'i': case 'j': 
		dyllAsmPrint("jmp dword dycCalli\n"); break;
	case 'l': case 'm':
		dyllAsmPrint("jmp dword dycCalli\n"); break;

	case 'x': dyllAsmPrint("jmp dword dycCalll\n"); break;
	case 'y': dyllAsmPrint("jmp dword dycCalll\n"); break;
	case 'f': dyllAsmPrint("jmp dword dycCallf\n"); break;
	case 'd': dyllAsmPrint("jmp dword dycCalld\n"); break;

	case 'r': dyllAsmPrint("jmp dword dycCallr\n"); break;
	case 'L': dyllAsmPrint("jmp dword dycCallr\n"); break;
	case 'Q': dyllAsmPrint("jmp dword dycCallr\n"); break;

	case 'C':
		switch(esig[1])
		{
		case '2': case '3': case '4': case '5':
		case '6': case '7': case '8': case '9':
			dyllAsmPrint("jmp dword dycCallr\n");
			break;
		default: dyllAsmPrint("jmp dword dycCallv\n"); break;
		}
		break;

	case 'P': dyllAsmPrint("jmp dword dycCallp\n"); break;
	case 'v': dyllAsmPrint("jmp dword dycCallv\n"); break;

	default: dyllAsmPrint("jmp dword dycCallv\n"); break;

	}

	dyllAsmEnd();
	return(p);
#endif

	return(NULL);
}

void *dyll_meta_staticmethodref(char *sym, char *name, char **args)
{
	char tb[256];
	dycClass cls;
	dycMethod minf;
	char *sig, *esig, *s1, *s2;
	char *s, *t;
	void *p;
	int i, j;

	s=args[1]; t=tb;
	while(*s && (*s!='('))*t++=*s++;
	*t++=0; sig=s;
	while(*s && (*s!=')'))s++;
	if(*s==')')s++;
	esig=s;

	cls=dycGetClass(args[0]);
	minf=dycIdxMethod(cls, tb, sig);

	s1=dyllGenSym();

#ifdef __i386__
	dyllAsmBegin();
	dyllAsmPrint("%s:\n", sym);
	dyllAsmPrint("mov %s, %s\n", args[2], s1);
	dyllAsmPrint("ret\n");
	p=dyllAsmEnd();

	if((*esig=='X') || (*esig=='U') || (*esig=='W') ||
		(*esig=='n') || (*esig=='o') || (*esig=='g'))
	{
		dyllAsmBegin();
		dyllAsmPrint("%s:\n", s1);
		dyllAsmPrint("push ebp; mov ebp, esp\n");

		j=dyllGetSigUArgsSize(sig);
		dyllAsmPrint("sub esp, %d\n", j*4);
		for(i=0; i<j; i++)
		{
			dyllAsmPrint("mov ecx, [ebp+%d]\n", 12+i*4);
			dyllAsmPrint("mov [esp+%d], ecx\n", i*4);
		}

		dyllAsmPrint("push dword %p\n", minf);
		dyllAsmPrint("push dword %p\n", cls);

		dyllAsmPrint("call dword dycCallStatic\n");

		j=dyllSigQuickSize(esig);

		dyllAsmPrint("mov edx, [ebp+8]\n");

		if(j==8)
		{
			dyllAsmPrint("movq xmm0, [eax]\n");
			dyllAsmPrint("movq [edx], xmm0\n");
		}else if(j==16)
		{
			dyllAsmPrint("movdqu xmm0, [eax]\n");
			dyllAsmPrint("movdqu [edx], xmm0\n");
		}else
		{
			j=(j+3)/4;
			for(i=0; i<j; i++)
			{
				dyllAsmPrint("mov ecx, [eax+%d]\n", i*4);
				dyllAsmPrint("mov [edx+%d], ecx\n", i*4);
			}
		}

		dyllAsmPrint("mov esp, ebp\n");
		dyllAsmPrint("pop ebp; ret\n");
		dyllAsmEnd();

		return(p);
	}

	dyllAsmBegin();
	dyllAsmPrint("%s:\n", s1);
	dyllAsmPrint("pop ecx\n");	//ret addr
	dyllAsmPrint("push dword %p\n", minf);
	dyllAsmPrint("push dword %p\n", cls);
	dyllAsmPrint("push ecx\n");

	switch(*esig)
	{
	case 'a': case 'h': case 'b': case 'c':
		dyllAsmPrint("jmp dword dycCallStatici\n"); break;
	case 's': case 't': case 'w':
		dyllAsmPrint("jmp dword dycCallStatici\n"); break;

	case 'i': case 'j': 
		dyllAsmPrint("jmp dword dycCallStatici\n"); break;
	case 'l': case 'm':
		dyllAsmPrint("jmp dword dycCallStatici\n"); break;

	case 'x': dyllAsmPrint("jmp dword dycCallStaticl\n"); break;
	case 'y': dyllAsmPrint("jmp dword dycCallStaticl\n"); break;
	case 'f': dyllAsmPrint("jmp dword dycCallStaticf\n"); break;
	case 'd': dyllAsmPrint("jmp dword dycCallStaticd\n"); break;

	case 'r': dyllAsmPrint("jmp dword dycCallStaticr\n"); break;
	case 'L': dyllAsmPrint("jmp dword dycCallStaticr\n"); break;
	case 'Q': dyllAsmPrint("jmp dword dycCallStaticr\n"); break;

	case 'C':
		switch(esig[1])
		{
		case '2': case '3': case '4': case '5':
		case '6': case '7': case '8': case '9':
			dyllAsmPrint("jmp dword dycCallStaticr\n");
			break;
		default: dyllAsmPrint("jmp dword dycCallStaticv\n"); break;
		}
		break;

	case 'P': dyllAsmPrint("jmp dword dycCallStaticp\n"); break;
	case 'v': dyllAsmPrint("jmp dword dycCallStaticv\n"); break;

	default: dyllAsmPrint("jmp dword dycCallStaticv\n"); break;

	}

	dyllAsmEnd();
	return(p);
#endif

	return(NULL);
}

void *dyll_meta_loadint(char *sym, char *name, char **args)
{
	char *reg;
	s64 val;
	dyt v;
	void *p;
	
	reg=args[0];
	val=atoi(args[1]);
	v=dyint(val);
	
#if 0
	if((val>=netval_imin) && (val<=netval_imax) && (((int)val)==val))
	{
		dyllAsmBegin();
		dyllAsmPrint("%s:\n", sym);
		dyllAsmPrint("lea %s, [bgbdy_fixnum_base%+d]\n",
			reg, (int)(i+netval_iofs));
		dyllAsmPrint("ret\n");
		p=dyllAsmEnd();
		return(p);
	}
#endif

#ifdef __i386__
	dyllAsmBegin();
	dyllAsmPrint("%s:\n", sym);
	dyllAsmPrint("mov %s, 0x%08X\n", reg, (u32)v);
	dyllAsmPrint("ret\n");
	
	if(!dyfixintp(v))
	{
		dyllAsmPrint("[section .data]\n");
		dyllAsmPrint("dd 0x%08X\n", reg, (u32)v);
	}
	
	p=dyllAsmEnd();
	return(p);
#endif

	return(NULL);
}

void *dyll_meta_loadflonum(char *sym, char *name, char **args)
{
	char *reg;
	float val;
	void *p;
	
	reg=args[0];
	val=atof(args[1]);

	dyllAsmBegin();
	dyllAsmPrint("%s:\n", sym);
	dyllAsmPrint("mov %s, %p\n",
		reg, dyflonum(val));
	dyllAsmPrint("ret\n");
	p=dyllAsmEnd();
	return(p);
}

void *dyll_meta_loadsymbol(char *sym, char *name, char **args)
{
	char *reg;
	dyt v;
	void *p;
	
	reg=args[0]; v=NULL;
	
	if(!strcmp(name, "DySymbol"))
		v=dysymbol(args[1]);
	if(!strcmp(name, "DyKeyword"))
		v=dykeyword(args[1]);
	if(!strcmp(name, "DyString"))
		v=dykeyword(args[1]);

	if(!v)
		return(NULL);

#ifdef __i386__
	dyllAsmBegin();
	dyllAsmPrint("%s:\n", sym);
	dyllAsmPrint("mov %s, 0x%08X\n", reg, (u32)v);
	dyllAsmPrint("ret\n");
	p=dyllAsmEnd();
	return(p);
#endif
}

void *dyll_meta_unaryop(char *sym, char *name, char **args)
{
	char *dreg, *sreg;
	char *fcn;
	void *p;
	int i;
	
	dreg=args[0];
	sreg=args[1];

	fcn=NULL;
	if(!strcmp(name, "DyNeg"))fcn="dyneg";
	if(!strcmp(name, "DyNot"))fcn="dynot";
	if(!strcmp(name, "DyLNot"))fcn="dylnot";
	if(!strcmp(name, "DyDefer"))fcn="dyumul";

	if(!strcmp(name, "DyCar"))fcn="dycar";
	if(!strcmp(name, "DyCdr"))fcn="dycdr";

	if(!strcmp(name, "DyCopyValue"))fcn="dyCopyValue";
	if(!strcmp(name, "DyDropValue"))fcn="dyDropValue";

	if(!strcmp(name, "DyDelete"))fcn="dyDelete";
	if(!strcmp(name, "DyClone"))fcn="dyClone";

	if(!fcn)
		return(NULL);

#ifdef __i386__
	dyllAsmBegin();
	dyllAsmPrint("%s:\n", sym);

	DYLL_ThunkGen_SaveRegs(dreg, args);

	dyllAsmPrint("push dword %s\n", sreg);
	dyllAsmPrint("call %s\n", fcn);
	dyllAsmPrint("add esp, 4\n", fcn);
	dyllAsmPrint("mov %s, eax\n", dreg);

	DYLL_ThunkGen_RestoreRegs(dreg, args);

	dyllAsmPrint("ret\n");

	p=dyllAsmEnd();
	return(p);
#endif

	return(NULL);
}

void *dyll_meta_binaryop(char *sym, char *name, char **args)
{
	char *dreg, *lreg, *rreg;
	char *fcn;
	void *p;
	int i;
	
	dreg=args[0];
	lreg=args[1];
	rreg=args[2];
//	val=atof(args[1]);

	fcn=NULL;
	if(!strcmp(name, "DyAdd"))fcn="dyadd";
	if(!strcmp(name, "DySub"))fcn="dysub";
	if(!strcmp(name, "DyMul"))fcn="dymul";
	if(!strcmp(name, "DyDiv"))fcn="dydiv";
	if(!strcmp(name, "DyMod"))fcn="dymod";
	if(!strcmp(name, "DyAnd"))fcn="dyand";
	if(!strcmp(name, "DyOr"))fcn="dyor";
	if(!strcmp(name, "DyXor"))fcn="dyxor";

	if(!strcmp(name, "DyShl"))fcn="dyshl";
	if(!strcmp(name, "DyShr"))fcn="dyshr";
	if(!strcmp(name, "DyExp"))fcn="dyexp";

	if(!strcmp(name, "DyEqv"))fcn="dyEqv";
	if(!strcmp(name, "DyNeqv"))fcn="dyNeqv";
	if(!strcmp(name, "DyEq"))fcn="dyEq";
	if(!strcmp(name, "DyNeq"))fcn="dyNeq";
	if(!strcmp(name, "DyLt"))fcn="dyLt";
	if(!strcmp(name, "DyGt"))fcn="dyGt";
	if(!strcmp(name, "DyLe"))fcn="dyLe";
	if(!strcmp(name, "DyGe"))fcn="dyGe";

	if(!strcmp(name, "DyGetKey"))fcn="dyGetKey";

	if(!strcmp(name, "DyCons"))fcn="dycons";

	if(!fcn)
		return(NULL);

#ifdef __i386__
	dyllAsmBegin();
	dyllAsmPrint("%s:\n", sym);

	DYLL_ThunkGen_SaveRegs(dreg, args);

	dyllAsmPrint("push dword %s\n", rreg);
	dyllAsmPrint("push dword %s\n", lreg);
	dyllAsmPrint("call %s\n", fcn);
	dyllAsmPrint("add esp, 8\n", fcn);
	dyllAsmPrint("mov %s, eax\n", dreg);

	DYLL_ThunkGen_RestoreRegs(dreg, args);

	dyllAsmPrint("ret\n");

	p=dyllAsmEnd();
	return(p);
#endif

	return(NULL);
}

void *dyll_meta_trinaryop(char *sym, char *name, char **args)
{
	char *dreg, *lreg, *mreg, *rreg;
	char *fcn;
	void *p;
	int i;
	
	dreg=args[0];
	lreg=args[1];
	mreg=args[2];
	rreg=args[3];

	fcn=NULL;

	if(!strcmp(name, "DySetKey"))fcn="dySetKey";
	if(!strcmp(name, "DyApplyMethodN"))fcn="dyApplyMethodLst";

	if(!fcn)
		return(NULL);

#ifdef __i386__
	dyllAsmBegin();
	dyllAsmPrint("%s:\n", sym);

	DYLL_ThunkGen_SaveRegs(dreg, args);

	dyllAsmPrint("push dword %s\n", rreg);
	dyllAsmPrint("push dword %s\n", mreg);
	dyllAsmPrint("push dword %s\n", lreg);
	dyllAsmPrint("call %s\n", fcn);
	dyllAsmPrint("add esp, 12\n", fcn);
	dyllAsmPrint("mov %s, eax\n", dreg);

	DYLL_ThunkGen_RestoreRegs(dreg, args);

	dyllAsmPrint("ret\n");

	p=dyllAsmEnd();
	return(p);
#endif

	return(NULL);
}

void *dyll_meta_dygetslot(char *sym, char *name, char **args)
{
	char *dreg, *lreg, *rreg;
	char *fcn;
	void *p;
	int i;
	
	dreg=args[0];
	lreg=args[1];
	rreg=args[2];

	fcn=NULL;
	if(!strcmp(name, "DyGetSlot"))fcn="dyGet";
	if(!strcmp(name, "DyGetDelegate"))fcn="dyGetDelegate";

	if(!fcn)
		return(NULL);

#ifdef __i386__
	dyllAsmBegin();
	dyllAsmPrint("%s:\n", sym);

	DYLL_ThunkGen_SaveRegs(dreg, args);

//	dyllAsmPrint("push dword %s\n", rreg);
	dyllAsmPrint("push dword .str\n");
	dyllAsmPrint("push dword %s\n", lreg);
	dyllAsmPrint("call %s\n", fcn);
	dyllAsmPrint("add esp, 8\n", fcn);
	dyllAsmPrint("mov %s, eax\n", dreg);

	DYLL_ThunkGen_RestoreRegs(dreg, args);

	dyllAsmPrint("ret\n");

	dyllAsmPrint(".str: db \"%s\"\n", rreg);

	p=dyllAsmEnd();
	return(p);
#endif

	return(NULL);
}

void *dyll_meta_dysetslot(char *sym, char *name, char **args)
{
	char *dreg, *lreg, *rreg, *str;
	char *fcn;
	void *p;
	int i;
	
	dreg=args[0];
	lreg=args[1];
	str=args[2];
	rreg=args[3];

	fcn=NULL;
	if(!strcmp(name, "DySetSlot"))fcn="dySet";
	if(!strcmp(name, "DyBindSlot"))fcn="dyBind";
	if(!strcmp(name, "DyTrySetSlot"))fcn="dyTrySet";
	if(!strcmp(name, "DySetDelegate"))fcn="dySetDelegate";
	if(!strcmp(name, "DyMethodCallN"))fcn="dyCallN";

	if(!fcn)
		return(NULL);

#ifdef __i386__
	dyllAsmBegin();
	dyllAsmPrint("%s:\n", sym);

	DYLL_ThunkGen_SaveRegs(dreg, args);

	dyllAsmPrint("push dword %s\n", rreg);
	dyllAsmPrint("push dword .str\n");
	dyllAsmPrint("push dword %s\n", lreg);
	dyllAsmPrint("call %s\n", fcn);
	dyllAsmPrint("add esp, 12\n", fcn);
	dyllAsmPrint("mov %s, eax\n", dreg);

	DYLL_ThunkGen_RestoreRegs(dreg, args);

	dyllAsmPrint("ret\n");

	dyllAsmPrint(".str: db \"%s\"\n", str);

	p=dyllAsmEnd();
	return(p);
#endif

	return(NULL);
}

void *dyll_meta_dymethodcall(char *sym, char *name, char **args)
{
	char *dreg, *oreg, *areg, *str, *acnt;
	char *fcn;
	void *p;
	int i;
	
	dreg=args[0];
	oreg=args[1];
	str=args[2];
	areg=args[3];
	acnt=args[4];

	fcn=NULL;
	if(!strcmp(name, "DyMethodCall"))fcn="dyCall";

	if(!fcn)
		return(NULL);

#ifdef __i386__
	dyllAsmBegin();
	dyllAsmPrint("%s:\n", sym);

	DYLL_ThunkGen_SaveRegs(dreg, args);

	dyllAsmPrint("push dword %s\n", acnt);
	dyllAsmPrint("push dword %s\n", areg);
	dyllAsmPrint("push dword .str\n");
	dyllAsmPrint("push dword %s\n", oreg);
	dyllAsmPrint("call %s\n", fcn);
	dyllAsmPrint("add esp, 16\n", fcn);
	dyllAsmPrint("mov %s, eax\n", dreg);

	DYLL_ThunkGen_RestoreRegs(dreg, args);

	dyllAsmPrint("ret\n");

	dyllAsmPrint(".str: db \"%s\"\n", str);

	p=dyllAsmEnd();
	return(p);
#endif

	return(NULL);
}

void *dyll_meta_dyapplymethod(char *sym, char *name, char **args)
{
	char *dreg, *oreg, *areg, *afcn, *acnt;
	char *fcn;
	void *p;
	int i;
	
	dreg=args[0];
	oreg=args[1];
	afcn=args[2];
	areg=args[3];
	acnt=args[4];

	fcn=NULL;
	if(!strcmp(name, "DyApplyMethodCall"))fcn="dyApplyMethod";

	if(!fcn)
		return(NULL);

#ifdef __i386__
	dyllAsmBegin();
	dyllAsmPrint("%s:\n", sym);

	DYLL_ThunkGen_SaveRegs(dreg, args);

	dyllAsmPrint("push dword %s\n", acnt);
	dyllAsmPrint("push dword %s\n", areg);
	dyllAsmPrint("push dword %s\n", afcn);
	dyllAsmPrint("push dword %s\n", oreg);
	dyllAsmPrint("call %s\n", fcn);
	dyllAsmPrint("add esp, 16\n", fcn);
	dyllAsmPrint("mov %s, eax\n", dreg);

	DYLL_ThunkGen_RestoreRegs(dreg, args);

	dyllAsmPrint("ret\n");

//	dyllAsmPrint(".str: db \"%s\"\n", str);

	p=dyllAsmEnd();
	return(p);
#endif

	return(NULL);
}

void *dyll_meta_dyapply(char *sym, char *name, char **args)
{
	char *dreg, *oreg, *areg, *afcn, *acnt;
	char *fcn;
	void *p;
	int i;
	
	dreg=args[0];
	afcn=args[1];
	areg=args[2];
	acnt=args[3];

	oreg="0";

	fcn=NULL;
	if(!strcmp(name, "DyApply"))fcn="dyApplyMethod";

	if(!fcn)
		return(NULL);

#ifdef __i386__
	dyllAsmBegin();
	dyllAsmPrint("%s:\n", sym);

	DYLL_ThunkGen_SaveRegs(dreg, args);

	dyllAsmPrint("push dword %s\n", acnt);
	dyllAsmPrint("push dword %s\n", areg);
	dyllAsmPrint("push dword %s\n", afcn);
	dyllAsmPrint("push dword %s\n", oreg);
	dyllAsmPrint("call %s\n", fcn);
	dyllAsmPrint("add esp, 16\n", fcn);
	dyllAsmPrint("mov %s, eax\n", dreg);

	DYLL_ThunkGen_RestoreRegs(dreg, args);

	dyllAsmPrint("ret\n");

//	dyllAsmPrint(".str: db \"%s\"\n", str);

	p=dyllAsmEnd();
	return(p);
#endif

	return(NULL);
}

void *dyll_meta_dycall(char *sym, char *name, char **args)
{
	char *dreg, *areg, *str, *acnt;
	char *fcn;
	void *p;
	int i;
	
	dreg=args[0];
	str=args[1];
	areg=args[2];
	acnt=args[3];

	fcn=NULL;
	if(!strcmp(name, "DyCall"))fcn="dyllCallTopA";
	if(!strcmp(name, "DyNew"))fcn="dyNewA";

	if(!fcn)
		return(NULL);

#ifdef __i386__
	dyllAsmBegin();
	dyllAsmPrint("%s:\n", sym);

	DYLL_ThunkGen_SaveRegs(dreg, args);

	dyllAsmPrint("push dword %s\n", acnt);
	dyllAsmPrint("push dword %s\n", areg);
	dyllAsmPrint("push dword .str\n");
	dyllAsmPrint("call %s\n", fcn);
	dyllAsmPrint("add esp, 12\n", fcn);
	dyllAsmPrint("mov %s, eax\n", dreg);

	DYLL_ThunkGen_RestoreRegs(dreg, args);

	dyllAsmPrint("ret\n");

	dyllAsmPrint(".str: db \"%s\"\n", str);

	p=dyllAsmEnd();
	return(p);
#endif

	return(NULL);
}

void BGBGC_InitDyLL_Thunk()
{
	dyllSetAddr("bgbdy_fixnum_base", bgbdy_vintrange);
	dyllSetAddr("bgbdy_flonum_base", bgbdy_vffrange);

	dyllAsmRegisterLinkMeta("ClassSlot", dyll_meta_classslot);
	dyllAsmRegisterLinkMeta("ClassMethod", dyll_meta_classmethod);

	dyllAsmRegisterLinkMeta("FieldRef", dyll_meta_fieldref);
	dyllAsmRegisterLinkMeta("MethodRef", dyll_meta_methodref);
	dyllAsmRegisterLinkMeta("StaticFieldRef", dyll_meta_staticfieldref);
	dyllAsmRegisterLinkMeta("StaticMethodRef", dyll_meta_staticmethodref);



	dyllAsmRegisterLinkMeta("DyLoadInt", dyll_meta_loadint);
	dyllAsmRegisterLinkMeta("DyLoadLong", dyll_meta_loadint);
	dyllAsmRegisterLinkMeta("DyLoadFlonum", dyll_meta_loadflonum);

	dyllAsmRegisterLinkMeta("DyLoadSymbol", dyll_meta_loadsymbol);
	dyllAsmRegisterLinkMeta("DyLoadKeyword", dyll_meta_loadsymbol);
	dyllAsmRegisterLinkMeta("DyLoadString", dyll_meta_loadsymbol);


	dyllAsmRegisterLinkMeta("DyNeg", dyll_meta_unaryop);
	dyllAsmRegisterLinkMeta("DyNot", dyll_meta_unaryop);
	dyllAsmRegisterLinkMeta("DyLNot", dyll_meta_unaryop);
	dyllAsmRegisterLinkMeta("DyDefer", dyll_meta_unaryop);

	dyllAsmRegisterLinkMeta("DyCar", dyll_meta_unaryop);
	dyllAsmRegisterLinkMeta("DyCdr", dyll_meta_unaryop);

	dyllAsmRegisterLinkMeta("DyCopyValue", dyll_meta_unaryop);
	dyllAsmRegisterLinkMeta("DyDropValue", dyll_meta_unaryop);
	dyllAsmRegisterLinkMeta("DyDelete", dyll_meta_unaryop);
	dyllAsmRegisterLinkMeta("DyClone", dyll_meta_unaryop);


	dyllAsmRegisterLinkMeta("DyAdd", dyll_meta_binaryop);
	dyllAsmRegisterLinkMeta("DySub", dyll_meta_binaryop);
	dyllAsmRegisterLinkMeta("DyMul", dyll_meta_binaryop);
	dyllAsmRegisterLinkMeta("DyDiv", dyll_meta_binaryop);
	dyllAsmRegisterLinkMeta("DyMod", dyll_meta_binaryop);
	dyllAsmRegisterLinkMeta("DyAnd", dyll_meta_binaryop);
	dyllAsmRegisterLinkMeta("DyOr", dyll_meta_binaryop);
	dyllAsmRegisterLinkMeta("DyXor", dyll_meta_binaryop);
	dyllAsmRegisterLinkMeta("DyShl", dyll_meta_binaryop);
	dyllAsmRegisterLinkMeta("DyShr", dyll_meta_binaryop);

	dyllAsmRegisterLinkMeta("DyExp", dyll_meta_binaryop);
	dyllAsmRegisterLinkMeta("DyCons", dyll_meta_binaryop);

	dyllAsmRegisterLinkMeta("DyGetKey", dyll_meta_binaryop);
	dyllAsmRegisterLinkMeta("DySetKey", dyll_meta_trinaryop);

	dyllAsmRegisterLinkMeta("DyGetSlot", dyll_meta_dygetslot);
	dyllAsmRegisterLinkMeta("DyGetDelegate", dyll_meta_dygetslot);

	dyllAsmRegisterLinkMeta("DySetSlot", dyll_meta_dysetslot);
	dyllAsmRegisterLinkMeta("DyBindSlot", dyll_meta_dysetslot);
	dyllAsmRegisterLinkMeta("DyTrySetSlot", dyll_meta_dysetslot);
	dyllAsmRegisterLinkMeta("DySetDelegate", dyll_meta_dysetslot);
	dyllAsmRegisterLinkMeta("DyMethodCallN", dyll_meta_dysetslot);

	dyllAsmRegisterLinkMeta("DyMethodCall", dyll_meta_dymethodcall);

	dyllAsmRegisterLinkMeta("DyCall", dyll_meta_dycall);
	dyllAsmRegisterLinkMeta("DyNew", dyll_meta_dycall);

	dyllAsmRegisterLinkMeta("DyApply", dyll_meta_dyapply);

	dyllAsmRegisterLinkMeta("DyApplyMethod", dyll_meta_dyapplymethod);
	dyllAsmRegisterLinkMeta("DyApplyMethodN", dyll_meta_trinaryop);
}
