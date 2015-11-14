#include <general.h>

elem Verify_ArrayOfStructsTest(elem lst)
{
	elem cur, t;
	int i;

	i=0;
	cur=lst;
	while(ELEM_CONSP(cur))
	{
		t=TyObj_GetSlot(CAR(cur), SYM("curly"));
		i+=TOINT(t);
		cur=CDR(cur);
	}
	return(FIXNUM(i));
}

elem Verify_CountTheEntities(elem str)
{
	elem t;
	char *s;
	int ltc, gtc, ampc, qc, dqc;

	ltc=0;
	gtc=0;
	ampc=0;
	qc=0;
	dqc=0;

	s=ELEM_TOSTRING(str);

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

	t=TyObj_CloneNull();
	TyObj_SetSlot(t, SYM("ctLeftAngleBrackets"), FIXNUM(ltc));
	TyObj_SetSlot(t, SYM("ctRightAngleBrackets"), FIXNUM(gtc));
	TyObj_SetSlot(t, SYM("ctAmpersands"), FIXNUM(ampc));
	TyObj_SetSlot(t, SYM("ctApostrophes"), FIXNUM(qc));
	TyObj_SetSlot(t, SYM("ctQuotes"), FIXNUM(dqc));

	return(t);
}

elem Verify_EasyStructTest(elem obj)
{
	elem t;
	int i;

	i=0;

	t=TyObj_GetSlot(obj, SYM("moe"));
	i+=TOINT(t);

	t=TyObj_GetSlot(obj, SYM("larry"));
	i+=TOINT(t);

	t=TyObj_GetSlot(obj, SYM("curly"));
	i+=TOINT(t);

	return(FIXNUM(i));
}

elem Verify_ModerateSizeArrayCheck(elem lst)
{
	elem cur, t;
	int i;
	char buf[256], *s;

	i=0;
	cur=lst;
	while(ELEM_CONSP(CDR(cur)))
	{
		cur=CDR(cur);
	}

	s=ELEM_TOSTRING(CAR(lst));
	strcpy(buf, s);
	s=ELEM_TOSTRING(CAR(cur));
	strcat(buf, s);

	return(STRING(buf));
}

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
