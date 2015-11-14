#include <bgbnet.h>

long Verify_FetchStructSlot(NET_Struct *ns, char *name, char **ty)
{
	int i;

	for(i=0; ns->types[i]; i++)
		if(!strcmp(ns->names[i], name))
	{
		*ty=ns->types[i];
		return(ns->values[i]);
	}
}

NET_Struct *Verify_NewNetStruct(int max)
{
	NET_Struct *tmp;

	tmp=kalloc(sizeof(NET_Struct));
	tmp->types=kalloc(max*sizeof(char *));
	tmp->names=kalloc(max*sizeof(char *));
	tmp->values=kalloc(max*sizeof(long));

	return(tmp);
}

int Verify_SetStructSlot(NET_Struct *ns, char *name, char *ty, long val)
{
	int i;

	for(i=0; ns->types[i]; i++)
		if(!strcmp(ns->names[i], name))
	{
		ns->types[i]=kstrdup(ty);
		ns->values[i]=val;
		return(0);
	}
	ns->types[i]=kstrdup(ty);
	ns->names[i]=kstrdup(name);
	ns->values[i]=val;

	i++;
	ns->types[i]=NULL;

	return(0);
}

long Verify_ArrayOfStructsTest(long arr)
{
	NET_Array *na;
	NET_Struct *ns;

	char *ty;
	int i, ax;
	long l;

	na=(NET_Array *)arr;

	ax=0;
	for(i=0; na->types[i]; i++)
	{
		ns=(NET_Struct *)na->values[i];
		l=Verify_FetchStructSlot(ns, "curly", &ty);
		ax+=l;
	}

	return(ax);
}

long Verify_CountTheEntities(char *str)
{
	char *s;
	int ltc, gtc, ampc, qc, dqc;
	NET_Struct *ns;

	ltc=0;
	gtc=0;
	ampc=0;
	qc=0;
	dqc=0;

	s=str;

	while(*s)
	{
		switch(*s++)
		{
		case '<':
			ltc++;
			break;
		case '>':
			gtc++;
			break;
		case '&':
			ampc++;
			break;
		case '\'':
			qc++;
			break;
		case '"':
			dqc++;
			break;
		default:
			break;
		}
	}

	ns=Verify_NewNetStruct(6);
	Verify_SetStructSlot(ns, "ctLeftAngleBrackets", "int", ltc);
	Verify_SetStructSlot(ns, "ctRightAngleBrackets", "int", gtc);
	Verify_SetStructSlot(ns, "ctAmpersands", "int", ampc);
	Verify_SetStructSlot(ns, "ctApostrophes", "int", qc);
	Verify_SetStructSlot(ns, "ctQuotes", "int", dqc);

	return((long)ns);
}

long Verify_EasyStructTest(long val)
{
	long l;
	int i;
	char *ty;

	i=0;

//	t=TyObj_GetSlot(obj, SYM("moe"));
//	i+=TOINT(t);

//	t=TyObj_GetSlot(obj, SYM("larry"));
//	i+=TOINT(t);

//	t=TyObj_GetSlot(obj, SYM("curly"));
//	i+=TOINT(t);

	l=Verify_FetchStructSlot(val, "moe", &ty);
	i+=l;

	l=Verify_FetchStructSlot(val, "larry", &ty);
	i+=l;

	l=Verify_FetchStructSlot(val, "curly", &ty);
	i+=l;

	return(i);
}

long Verify_ModerateSizeArrayCheck(long val)
{
//	elem cur, t;
	int i;
	char buf[256], *s;
	NET_Array *na;

	na=(NET_Array *)val;

	for(i=0; na->types[i]; i++);

	s=na->values[0];
	strcpy(buf, s);
	s=na->values[i-1];
	strcat(buf, s);

	return(kstrdup(buf));
}

long Verify_NestedStructTest(long st)
{
	long t, t2;
	int i;
	char *ty;

	t=st;
	t=Verify_FetchStructSlot(t, "2000", &ty);
	t=Verify_FetchStructSlot(t, "04", &ty);
	t=Verify_FetchStructSlot(t, "01", &ty);

//	i=0;
//	i+=TOINT(TyObj_GetSlot(t, SYM("curly")));
//	i+=TOINT(TyObj_GetSlot(t, SYM("larry")));
//	i+=TOINT(TyObj_GetSlot(t, SYM("moe")));

	i=0;
	i+=Verify_FetchStructSlot(t, "curly", &ty);
	i+=Verify_FetchStructSlot(t, "larry", &ty);
	i+=Verify_FetchStructSlot(t, "moe", &ty);

	return(i);
}

long Verify_SimpleStructReturnTest(long num)
{
//	elem t;
	NET_Struct *ns;
	long l;
	float f, f2;

	f=*(float *)(&num);

	ns=Verify_NewNetStruct(4);

	f2=f*10;
	l=*(long *)(&f2);
	Verify_SetStructSlot(ns, "times10", "float", l);

	f2=f*100;
	l=*(long *)(&f2);
	Verify_SetStructSlot(ns, "times100", "float", l);

	f2=f*1000;
	l=*(long *)(&f2);
	Verify_SetStructSlot(ns, "times1000", "float", l);

//	t=TyObj_CloneNull();
//	TyObj_SetSlot(t, SYM("times10"), FLONUM(TOFLOAT(num)*10));
//	TyObj_SetSlot(t, SYM("times100"), FLONUM(TOFLOAT(num)*100));
//	TyObj_SetSlot(t, SYM("times1000"), FLONUM(TOFLOAT(num)*1000));

	return(ns);
}

#if 0

elem Verify_NestedStructTest(elem str)
{
	elem t, t2;
	int i;

	t=TyObj_GetSlot(str, SYM("2000"));
	t=TyObj_GetSlot(t, SYM("04"));
	t=TyObj_GetSlot(t, SYM("01"));

	i=0;
	i+=TOINT(TyObj_GetSlot(t, SYM("curly")));
	i+=TOINT(TyObj_GetSlot(t, SYM("larry")));
	i+=TOINT(TyObj_GetSlot(t, SYM("moe")));

	return(FIXNUM(i));
}

elem Verify_SimpleStructReturnTest(elem num)
{
	elem t;
	int i;

	t=TyObj_CloneNull();
	TyObj_SetSlot(t, SYM("times10"), FLONUM(TOFLOAT(num)*10));
	TyObj_SetSlot(t, SYM("times100"), FLONUM(TOFLOAT(num)*100));
	TyObj_SetSlot(t, SYM("times1000"), FLONUM(TOFLOAT(num)*1000));

	return(t);
}

elem Verify_Func(elem func, elem args)
{
	elem t;

	if(func==SYM("validator1.arrayOfStructsTest"))
	{
		t=Verify_ArrayOfStructsTest(CAR(args));
		return(t);
	}
	if(func==SYM("validator1.countTheEntities"))
	{
		t=Verify_CountTheEntities(CAR(args));
		return(t);
	}
	if(func==SYM("validator1.easyStructTest"))
	{
		t=Verify_EasyStructTest(CAR(args));
		return(t);
	}
	if(func==SYM("validator1.echoStructTest"))
	{
		t=CAR(args);
		return(t);
	}
	if(func==SYM("validator1.manyTypesTest"))
	{
		t=args;
		return(t);
	}
	if(func==SYM("validator1.moderateSizeArrayCheck"))
	{
		t=Verify_ModerateSizeArrayCheck(CAR(args));
		return(t);
	}
	if(func==SYM("validator1.nestedStructTest"))
	{
		t=Verify_NestedStructTest(CAR(args));
		return(t);
	}
	if(func==SYM("validator1.simpleStructReturnTest"))
	{
		t=Verify_SimpleStructReturnTest(CAR(args));
		return(t);
	}
	return(MISC_FALSE);
}
#endif

int Verify_Init()
{
	NET_ExportFunc("validator1.arrayOfStructsTest",
		&Verify_ArrayOfStructsTest, "int", "array");
	NET_ExportFunc("validator1.countTheEntities",
		&Verify_CountTheEntities, "struct", "string");
	NET_ExportFunc("validator1.easyStructTest",
		&Verify_EasyStructTest, "int", "struct");
	NET_ExportFunc("validator1.moderateSizeArrayCheck",
		&Verify_ModerateSizeArrayCheck, "string", "array");
	NET_ExportFunc("validator1.nestedStructTest",
		&Verify_NestedStructTest, "int", "struct");

	NET_ExportFunc("validator1.simpleStructReturnTest",
		&Verify_SimpleStructReturnTest, "struct", "float");

	return(0);
}
