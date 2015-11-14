#include <general.h>
#include <net/xml_rpc.h>

elem XmlRpc_DecodeArraySlots(elem param)
{
	elem t, x;
	elem cur;

	x=MISC_EOL;
	cur=CDDR(param);
	while(ELEM_CONSP(cur))
	{
		if(CAAR(cur)==SYM("value"))
		{
			t=XmlRpc_DecodeValue(CADDR(CAR(cur)));
			x=CONS(t, x);
		}
		cur=CDR(cur);
	}
	x=TyFcn_NReverse(x);
	return(x);
}

elem XmlRpc_DecodeArray(elem str)
{
	elem t, x;
	elem cur;

	t=MISC_EOL;
	cur=CDDR(str);
	while(ELEM_CONSP(cur))
	{
		if(CAAR(cur)==SYM("data"))
		{
			t=XmlRpc_DecodeArraySlots(CAR(cur));
		}
		cur=CDR(cur);
	}
	return(t);
}

elem XmlRpc_DecodeMember(elem obj, elem mem)
{
	elem t, x;
	elem cur;

	elem var, val;

	t=MISC_NULL;
	cur=CDDR(mem);
	while(ELEM_CONSP(cur))
	{
		if(CAAR(cur)==SYM("name"))
		{
			var=SYM(ELEM_TOSTRING(CADDR(CAR(cur))));
		}
		if(CAAR(cur)==SYM("value"))
		{
			val=XmlRpc_DecodeValue(CADDR(CAR(cur)));
		}
		cur=CDR(cur);
	}
	TyObj_SetSlot(obj, var, val);

	return(t);
}

elem XmlRpc_DecodeStruct(elem str)
{
	elem t, x;
	elem cur;

	t=TyObj_CloneNull();

	cur=CDDR(str);
	while(ELEM_CONSP(cur))
	{
		XmlRpc_DecodeMember(t, CAR(cur));
		cur=CDR(cur);
	}
	return(t);
}

elem XmlRpc_DecodeValue(elem val)
{
	elem t, x;
	char *s, *s2;
	int i, j;
	char buf[5];

	if(ELEM_STRINGP(val))return(val);

	if(CAR(val)==SYM("i4"))
	{
		s=ELEM_TOSTRING(CADDR(val));
		t=FIXNUM(atoi(s));
		return(t);
	}
	if(CAR(val)==SYM("int"))
	{
		s=ELEM_TOSTRING(CADDR(val));
		t=FIXNUM(atoi(s));
		return(t);
	}
	if(CAR(val)==SYM("boolean"))
	{
		s=ELEM_TOSTRING(CADDR(val));
		i=atoi(s);
		t=MISC_TRUE;
		if(!i)t=MISC_FALSE;
		return(t);
	}
	if(CAR(val)==SYM("string"))
	{
		t=CADDR(val);
		return(t);
	}
	if(CAR(val)==SYM("double"))
	{
		s=ELEM_TOSTRING(CADDR(val));
		t=FLONUM(atof(s));
		return(t);
	}
	if(CAR(val)==SYM("dateTime.iso8601"))
	{
		s=ELEM_TOSTRING(CADDR(val));
		x=MISC_EOL;

		memset(buf, 0, 5);
		strncpy(buf, s, 4);
		t=FIXNUM(atoi(buf));
		x=CONS(t, x);

		memset(buf, 0, 5);

		strncpy(buf, s+4, 2);
		t=FIXNUM(atoi(buf));
		x=CONS(t, x);

		strncpy(buf, s+6, 2);
		t=FIXNUM(atoi(buf));
		x=CONS(t, x);

		strncpy(buf, s+9, 2);
		t=FIXNUM(atoi(buf));
		x=CONS(t, x);

		strncpy(buf, s+12, 2);
		t=FIXNUM(atoi(buf));
		x=CONS(t, x);

		strncpy(buf, s+15, 2);
		t=FIXNUM(atoi(buf));
		x=CONS(t, x);

		x=TyFcn_NReverse(x);
		x=CONS(SYM("date-time:"), x);

		return(x);
	}
	if(CAR(val)==SYM("base64"))
	{
		s=ELEM_TOSTRING(CADDR(val));
		i=strlen(s);
		j=(i*3)/4;
		t=VECTOR_NEWT(j, VECTOR_U8);
		s2=TyFcn_ByteVectorBody(t);

		kprint("recv mime %d->%d\n", i, j);

		HttpNode_DecodeMime(s2, s, i);
		return(t);
	}

	if(CAR(val)==SYM("struct"))
	{
		t=XmlRpc_DecodeStruct(val);
		return(t);
	}
	if(CAR(val)==SYM("array"))
	{
		t=XmlRpc_DecodeArray(val);
		return(t);
	}
}

elem XmlRpc_DecodeParam(elem param)
{
	elem t, x;
	elem cur;

	t=MISC_NULL;
	cur=CDDR(param);
	while(ELEM_CONSP(cur))
	{
		if(CAAR(cur)==SYM("value"))
		{
			t=XmlRpc_DecodeValue(CADDR(CAR(cur)));
		}
		cur=CDR(cur);
	}
	return(t);
}

elem XmlRpc_DecodeParams(elem lst)
{
	elem t, x;
	elem cur;

	x=MISC_EOL;
	cur=lst;
	while(ELEM_CONSP(cur))
	{
		if(CAAR(cur)==SYM("param"))
		{
			t=XmlRpc_DecodeParam(CAR(cur));
			x=CONS(t, x);
		}
		cur=CDR(cur);
	}
	x=TyFcn_NReverse(x);

	return(x);
}


elem XmlRpc_EncodeArray(elem lst)
{
	elem cur;
	elem t, x;

	x=MISC_EOL;
	cur=lst;

	while(ELEM_CONSP(cur))
	{
		t=XmlRpc_EncodeValue(CAR(cur));

		t=CONS(t, MISC_EOL);
		t=CONS(MISC_EOL, t);
		t=CONS(SYM("value"), t);

		x=CONS(t, x);

		cur=CDR(cur);
	}

	x=TyFcn_NReverse(x);

	x=CONS(MISC_EOL, x);
	x=CONS(SYM("data"), x);

	x=CONS(x, MISC_EOL);
	x=CONS(MISC_EOL, x);
	x=CONS(SYM("array"), x);

	return(x);
}

elem XmlRpc_EncodeStruct(elem obj)
{
	elem lst, cur;
	elem t, t2, x;

	x=MISC_EOL;
	lst=TyObj_SlotNames(obj);
	cur=lst;
	while(ELEM_CONSP(cur))
	{
		t2=MISC_EOL;

		t=TyObj_GetSlot(obj, CAR(cur));
		t=XmlRpc_EncodeValue(t);
		t=CONS(t, MISC_EOL);
		t=CONS(MISC_EOL, t);
		t=CONS(SYM("value"), t);

		t2=CONS(t, t2);

		t=STRING(ELEM_TOSYMBOL(CAR(cur)));
		t=CONS(t, MISC_EOL);
		t=CONS(MISC_EOL, t);
		t=CONS(SYM("name"), t);

		t2=CONS(t, t2);

		t2=CONS(MISC_EOL, t2);
		t2=CONS(SYM("member"), t2);

		x=CONS(t2, x);

		cur=CDR(cur);
	}

	x=CONS(MISC_EOL, x);
	x=CONS(SYM("struct"), x);

	return(x);
}

elem XmlRpc_EncodeDate(elem obj)
{
	char buf[18];
	int i, a[6];
	elem t;

	for(i=0; i<6; i++)a[i]=TOINT(LIST_REF(obj, i+1));
	sprintf(buf, "%04d%02d%02dT%02d:%02d:%02d",
		a[0], a[1], a[2], a[3], a[4], a[5]);

	t=STRING(buf);
	t=CONS(t, MISC_EOL);
	t=CONS(MISC_EOL, t);
	t=CONS(SYM("dateTime.iso8601"), t);
	return(t);
}

elem XmlRpc_EncodeValue(elem val)
{
	char buf[256];
	int i;
	double x;
	elem t;

	char *s, *s2;

	if(ELEM_STRINGP(val))
	{
		t=val;
		t=CONS(t, MISC_EOL);
		t=CONS(MISC_EOL, t);
		t=CONS(SYM("string"), t);
		return(t);
	}
	if(ELEM_FIXNUMP(val))
	{
		i=TOINT(val);
		sprintf(buf, "%d", i);
		t=STRING(buf);
		t=CONS(t, MISC_EOL);
		t=CONS(MISC_EOL, t);
		t=CONS(SYM("i4"), t);
		return(t);
	}
	if(ELEM_FLONUMP(val))
	{
		x=TOFLOAT(val);
		sprintf(buf, "%g", x);
		t=STRING(buf);
		t=CONS(t, MISC_EOL);
		t=CONS(MISC_EOL, t);
		t=CONS(SYM("double"), t);
		return(t);
	}

	if(ELEM_CONSP(val))
	{
		if(CAR(val)==SYM("date-time:"))
		{
			t=XmlRpc_EncodeDate(val);
			return(t);
		}

		t=XmlRpc_EncodeArray(val);
		return(t);
	}

	if(ELEM_ENVOBJP(val))
	{
		t=XmlRpc_EncodeStruct(val);
		return(t);
	}

	if(ELEM_BYTEVECTORP(val))
	{
		s=TyFcn_ByteVectorBody(val);
		i=VECTOR_LEN(val);
		s2=kalloc(((i*4)/3)+5);
		HttpNode_EncodeMime(s2, s, i);

		kprint("send mime %d->%d\n", i, (i*4)/3);
		t=STRING(s2);
		kfree(s2);
		t=CONS(t, MISC_EOL);
		t=CONS(MISC_EOL, t);
		t=CONS(SYM("base64"), t);
		return(t);
	}

	if(val==MISC_TRUE)
	{
		t=STRING("1");
		t=CONS(t, MISC_EOL);
		t=CONS(MISC_EOL, t);
		t=CONS(SYM("boolean"), t);
		return(t);
	}
	if(val==MISC_FALSE)
	{
		t=STRING("0");
		t=CONS(t, MISC_EOL);
		t=CONS(MISC_EOL, t);
		t=CONS(SYM("boolean"), t);
		return(t);
	}

	if(val==MISC_NULL)
	{
		t=STRING("$null");
		t=CONS(t, MISC_EOL);
		t=CONS(MISC_EOL, t);
		t=CONS(SYM("boolean"), t);
		return(t);
	}

	t=STRING("$undefined");
	t=CONS(t, MISC_EOL);
	t=CONS(MISC_EOL, t);
	t=CONS(SYM("string"), t);
	return(t);
}

elem XmlRpc_EncodeResponse(elem val)
{
	elem t, x;

	t=XmlRpc_EncodeValue(val);

	x=CONS(t, MISC_EOL);
	x=CONS(MISC_EOL, x);
	x=CONS(SYM("value"), x);

	x=CONS(x, MISC_EOL);
	x=CONS(MISC_EOL, x);
	x=CONS(SYM("param"), x);

	x=CONS(x, MISC_EOL);
	x=CONS(MISC_EOL, x);
	x=CONS(SYM("params"), x);

	x=CONS(x, MISC_EOL);
	x=CONS(MISC_EOL, x);
	x=CONS(SYM("methodResponse"), x);

	return(x);
}

elem XmlRpc_HandleCall(elem req)
{
	elem cur, t;
	elem method, params;

	method=MISC_NULL;
	params=MISC_EOL;

	if(CAR(req)==SYM("methodCall"))
	{
		cur=CDDR(req);
		while(ELEM_CONSP(cur))
		{
			if(CAAR(cur)==SYM("methodName"))
			{
				method=CADDR(CAR(cur));
			}
			if(CAAR(cur)==SYM("params"))
			{
				t=CDDR(CAR(cur));
				params=XmlRpc_DecodeParams(t);
			}
			cur=CDR(cur);
		}
	}

	kprint("method call: ");
	TyFcn_DumpElem(method);
	kprint(" with: ");
	TyFcn_DumpElemBR(params);

	method=SYM(ELEM_TOSTRING(method));
	t=Verify_Func(method, params);
//	t=MISC_NULL;

	return(t);
}

int XmlRpc_HandleMessage(HTTP_Con *con, char *msg)
{
	elem t;
	char *s, *buf;

	kprint("handle message\n");

	s=msg;
	t=ParseXML_ParseExpr(&s);
	while(1)
	{
		if(!ELEM_CONSP(t))break;
		if(CAR(t)!=SYM("?"))break;
		t=ParseXML_ParseExpr(&s);

//		TyFcn_DumpElem(t);
	}

	kprint("handle call\n");

	t=XmlRpc_HandleCall(t);

	kprint("encode response\n");
	t=XmlRpc_EncodeResponse(t);

	kprint("send response\n");

	buf=kalloc(16384);
	s=buf;

	s=kprints(s, "<?xml version=\"1.0\"?>\n");
	s=ParseXML_PrintExpr(s, t);

	HttpNode_SendResponse(con, buf, s-buf);

	kfree(buf);

	return(0);
}
