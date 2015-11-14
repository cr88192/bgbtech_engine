#include <bgbccc.h>

/** Stub Errors which indicate Error conditions.
  * These mean the output code will not work.
  * Or for fully unimplemented functionality.
  */
void BGBCC_FrBC_StubErrorLLn(BGBCC_FrCC_State *ctx,
	char *file, int line)
{
	BGBCC_FrCC_Error(ctx, "StubError %s:%d (CC=%s:%d)\n",
		file, line, ctx->lfn, ctx->lln);
	*(int *)-1=-1;
}

/** Stub Warnings which indicate Warning conditions.
  * These are for functionality which may produce unintended behavior.
  * This may also apply to hacked / incomplete functionality.
  */
void BGBCC_FrBC_StubWarnLLn(BGBCC_FrCC_State *ctx,
	char *file, int line)
{
	BGBCC_FrCC_Warn(ctx, "StubWarning %s:%d (CC=%s:%d)\n",
		file, line, ctx->lfn, ctx->lln);
}

/** Stub Warnings which indicate Note conditions.
  * These are for unimplemented / incomplete functionality, 
  * Which does not otherwise effect output code.
  * These are to be queitly logged.
  */
void BGBCC_FrBC_StubNoteLLn(BGBCC_FrCC_State *ctx,
	char *file, int line)
{
	BGBCC_FrCC_Note(ctx, "StubNote %s:%d (CC=%s:%d)\n",
		file, line, ctx->lfn, ctx->lln);
}


/** Stub Errors which indicate Error conditions.
  * These mean the output code will not work.
  * Or for fully unimplemented functionality.
  */
void BGBCC_FrBC_TagErrorLLn(BGBCC_FrCC_State *ctx, int tag,
	char *file, int line)
{
	BGBCC_FrCC_Error(ctx, "TagError %s:%d(%04X) (CC=%s:%d)\n",
		file, line, tag, ctx->lfn, ctx->lln);
	*(int *)-1=-1;
}

/** Stub Warnings which indicate Warning conditions.
  * These are for functionality which may produce unintended behavior.
  * This may also apply to hacked / incomplete functionality.
  */
void BGBCC_FrBC_TagWarnLLn(BGBCC_FrCC_State *ctx, int tag,
	char *file, int line)
{
	BGBCC_FrCC_Warn(ctx, "TagWarning %s:%d(%04X) (CC=%s:%d)\n",
		file, line, tag, ctx->lfn, ctx->lln);
}

/** Stub Warnings which indicate Note conditions.
  * These are for unimplemented / incomplete functionality, 
  * Which does not otherwise effect output code.
  * These are to be queitly logged.
  */
void BGBCC_FrBC_TagNoteLLn(BGBCC_FrCC_State *ctx, int tag,
	char *file, int line)
{
	BGBCC_FrCC_Note(ctx, "TagNote %s:%d(%04X) (CC=%s:%d)\n",
		file, line, tag, ctx->lfn, ctx->lln);
}


void BGBCC_FrBC_AddGlobalDecl(BGBCC_FrCC_State *ctx,
	BGBCC_FrCC_RegisterInfo *decl)
{
	if(!ctx->n_reg_globals)
		ctx->n_reg_globals++;
	decl->gblid=ctx->n_reg_globals;
	ctx->reg_globals[ctx->n_reg_globals++]=decl;
}

void BGBCC_FrBC_AddFrameArg(BGBCC_FrCC_State *ctx,
	BGBCC_FrCC_RegisterInfo *frame,
	BGBCC_FrCC_RegisterInfo *decl)
{
	if(!frame->args)
	{
		frame->args=bgbcc_malloc(64*sizeof(BGBCC_FrCC_RegisterInfo *));
		frame->n_args=0;
		frame->m_args=64;
	}

	decl->gblid=frame->n_args;
	frame->args[frame->n_args++]=decl;
}

void BGBCC_FrBC_AddFrameLocal(BGBCC_FrCC_State *ctx,
	BGBCC_FrCC_RegisterInfo *frame,
	BGBCC_FrCC_RegisterInfo *decl)
{
	if(!frame->locals)
	{
		frame->locals=bgbcc_malloc(256*sizeof(BGBCC_FrCC_RegisterInfo *));
		frame->n_locals=0;
		frame->m_locals=256;
	}

	decl->gblid=frame->n_locals;
	frame->locals[frame->n_locals++]=decl;
}

void BGBCC_FrBC_AddFrameField(BGBCC_FrCC_State *ctx,
	BGBCC_FrCC_RegisterInfo *frame,
	BGBCC_FrCC_RegisterInfo *decl)
{
	if(!frame->fields)
	{
		frame->fields=bgbcc_malloc(256*sizeof(BGBCC_FrCC_RegisterInfo *));
		frame->n_fields=0;
		frame->m_fields=256;
	}

	decl->gblid=frame->n_fields;
	frame->fields[frame->n_fields++]=decl;
}

void BGBCC_FrBC_AddLiteral(BGBCC_FrCC_State *ctx,
	BGBCC_FrCC_LiteralInfo *obj)
{
	if(!ctx->n_literals)
		ctx->n_literals++;
		
	obj->litid=ctx->n_literals;
	ctx->literals[ctx->n_literals++]=obj;
}

void BGBCC_FrBC_Begin(BGBCC_FrCC_State *ctx, int tag)
{
	BGBCC_FrBC_BeginName(ctx, tag, NULL);
}

void BGBCC_FrBC_BeginName(BGBCC_FrCC_State *ctx, int tag, char *name)
{
	BGBCC_FrCC_LiteralInfo *obj;
	BGBCC_FrCC_RegisterInfo *decl;

	if(name)
	{
		if(tag==FR2C_CMD_STRUCT)
		{
			obj=BGBCC_FrBC_LookupStructure(ctx, name);
			if(obj)
			{
				if(!obj->decl)
				{
					obj->littype=FR2C_LITID_STRUCT;
					obj->decl=bgbcc_malloc(sizeof(BGBCC_FrCC_RegisterInfo));
					obj->decl->regtype=FR2C_LITID_STRUCT;
				}
			
				obj->decl->n_fields=0;
				ctx->cur_objstack[ctx->cur_objstackpos++]=obj;
				ctx->cur_obj=obj;
				return;
			}
		}

		if(tag==FR2C_CMD_UNION)
		{
			obj=BGBCC_FrBC_LookupStructure(ctx, name);
			if(obj)
			{
				if(!obj->decl)
				{
					obj->littype=FR2C_LITID_UNION;
					obj->decl=bgbcc_malloc(sizeof(BGBCC_FrCC_RegisterInfo));
					obj->decl->regtype=FR2C_LITID_UNION;
				}
			
				obj->decl->n_fields=0;
				ctx->cur_objstack[ctx->cur_objstackpos++]=obj;
				ctx->cur_obj=obj;
				return;
			}
		}

		if(tag==FR2C_CMD_FUNCTION)
		{
			decl=BGBCC_FrBC_LookupGlobal(ctx, name);
			if(decl)
			{
				obj=bgbcc_malloc(sizeof(BGBCC_FrCC_LiteralInfo));
				obj->parent=ctx->cur_obj;
	
				ctx->cur_objstack[ctx->cur_objstackpos++]=obj;
				ctx->cur_obj=obj;
				
				obj->decl=decl;

				obj->littype=FR2C_LITID_FUNCTION;
				obj->decl->regtype=FR2C_LITID_FUNCTION;

				ctx->regstackpos=0;
				ctx->uregstackpos=0;
				ctx->markstackpos=0;
				ctx->ip=ctx->ips;
				ctx->n_goto=0;
				ctx->n_lbl=0;
				return;
			}
		}

		if((tag==FR2C_CMD_PROTOTYPE) ||
			(tag==FR2C_CMD_S_PROTOTYPE))
		{
			decl=BGBCC_FrBC_LookupGlobal(ctx, name);
			if(decl)
			{
				obj=bgbcc_malloc(sizeof(BGBCC_FrCC_LiteralInfo));
				obj->parent=ctx->cur_obj;
	
				ctx->cur_objstack[ctx->cur_objstackpos++]=obj;
				ctx->cur_obj=obj;

				obj->littype=FR2C_LITID_FUNCTION;
				obj->decl=bgbcc_malloc(sizeof(BGBCC_FrCC_RegisterInfo));
				obj->decl->regtype=FR2C_LITID_FUNCTION;
				return;
			}
		}
	}

	obj=bgbcc_malloc(sizeof(BGBCC_FrCC_LiteralInfo));
	obj->parent=ctx->cur_obj;
	
	ctx->cur_objstack[ctx->cur_objstackpos++]=obj;
	ctx->cur_obj=obj;
	
	switch(tag)
	{
	case FR2C_CMD_VARDECL:
		obj->decl=bgbcc_malloc(sizeof(BGBCC_FrCC_RegisterInfo));
		obj->decl->regtype=FR2C_LITID_VAR;
		
		if(obj->parent)
		{
			obj->littype=FR2C_LITID_VAR;
			switch(obj->parent->littype)
			{
			case FR2C_LITID_ARGS:
				BGBCC_FrBC_AddFrameArg(ctx,
					obj->parent->parent->decl,
					obj->decl);
				break;
			case FR2C_LITID_LOCALS:
				BGBCC_FrBC_AddFrameLocal(ctx,
					obj->parent->parent->decl,
					obj->decl);
				break;
			case FR2C_LITID_FUNCTION:
				obj->parent->decl->defv=obj->decl;
				break;
			case FR2C_LITID_STRUCT:
			case FR2C_LITID_UNION:
				BGBCC_FrBC_AddFrameField(ctx,
					obj->parent->decl,
					obj->decl);
				break;
			}
		}else
		{
			obj->littype=FR2C_LITID_GLOBALVAR;
			obj->decl->regtype=FR2C_LITID_GLOBALVAR;
			BGBCC_FrBC_AddGlobalDecl(ctx, obj->decl);
		}
		break;
	case FR2C_CMD_FUNCTION:
		obj->littype=FR2C_LITID_FUNCTION;
		obj->decl=bgbcc_malloc(sizeof(BGBCC_FrCC_RegisterInfo));
		obj->decl->regtype=FR2C_LITID_FUNCTION;
		BGBCC_FrBC_AddGlobalDecl(ctx, obj->decl);

		ctx->regstackpos=0;
		ctx->uregstackpos=0;
		ctx->markstackpos=0;
		ctx->ip=ctx->ips;
		ctx->n_goto=0;
		ctx->n_lbl=0;
		break;

	case FR2C_CMD_ARGS:
		obj->littype=FR2C_LITID_ARGS;
		break;
	case FR2C_CMD_LOCALS:
		obj->littype=FR2C_LITID_LOCALS;
		break;
	case FR2C_CMD_VARVALUE:
		obj->littype=FR2C_LITID_VALUE;
		break;

	case FR2C_CMD_PROTOTYPE:
	case FR2C_CMD_S_PROTOTYPE:
		obj->littype=FR2C_LITID_FUNCTION;
		obj->decl=bgbcc_malloc(sizeof(BGBCC_FrCC_RegisterInfo));
		obj->decl->regtype=FR2C_LITID_FUNCTION;
		BGBCC_FrBC_AddGlobalDecl(ctx, obj->decl);

		ctx->ip=ctx->ips;
		ctx->n_goto=0;
		ctx->n_lbl=0;
		break;

	case FR2C_CMD_STRUCT:
		obj->littype=FR2C_LITID_STRUCT;
		obj->decl=bgbcc_malloc(sizeof(BGBCC_FrCC_RegisterInfo));
		obj->decl->regtype=FR2C_LITID_STRUCT;
		BGBCC_FrBC_AddLiteral(ctx, obj);
		break;
	case FR2C_CMD_UNION:
		obj->littype=FR2C_LITID_UNION;
		obj->decl=bgbcc_malloc(sizeof(BGBCC_FrCC_RegisterInfo));
		obj->decl->regtype=FR2C_LITID_UNION;
		BGBCC_FrBC_AddLiteral(ctx, obj);
		break;

	case FR2C_CMD_LIST:
		obj->littype=FR2C_LITID_LIST;
		obj->decl=bgbcc_malloc(sizeof(BGBCC_FrCC_RegisterInfo));
		obj->decl->regtype=FR2C_LITID_LIST;
		BGBCC_FrBC_AddLiteral(ctx, obj);
		break;

	case FR2C_CMD_BODY:
		ctx->cur_func=obj->parent->decl;
		ctx->regstackpos=0;
		ctx->markstackpos=0;
		ctx->ip=ctx->ips;
		ctx->n_goto=0;
		ctx->n_lbl=0;
		break;

	default:
		break;
	}

//	BGBCC_FrBC_StubError(ctx);
}

int BGBCC_FrBC_LookupCurFunctionLabel(BGBCC_FrCC_State *ctx, char *name)
{
	int i, j, k;
	
	for(i=0; i<ctx->n_lbl; i++)
	{
		if(!strcmp(ctx->lbl_name[i], name))
			return(ctx->lbl_dest[i]);
	}
	return(-1);
}

void BGBCC_FrBC_EndFunction(BGBCC_FrCC_State *ctx,
	BGBCC_FrCC_LiteralInfo *obj)
{
	byte *rla, *rld;
	int i, j, k;

	for(i=0; i<ctx->n_goto; i++)
	{
		switch(ctx->goto_type[i])
		{
		case FR2C_JMPMODE_A16:
			j=BGBCC_FrBC_LookupCurFunctionLabel(ctx,
				ctx->goto_name[i]);
			rla=ctx->ips+ctx->goto_dest[i];
			rld=ctx->ips+j;
			k=(rla[-2]<<8)|(rla[-1]);
			k+=rld-rla;
			rla[-2]=(k>>8)&255;
			rla[-1]=(k   )&255;
			break;
		case FR2C_JMPMODE_A32:
			j=BGBCC_FrBC_LookupCurFunctionLabel(ctx,
				ctx->goto_name[i]);
			rla=ctx->ips+ctx->goto_dest[i];
			rld=ctx->ips+j;
			k=(rla[-4]<<24)|(rla[-3]<<16)|(rla[-2]<<8)|(rla[-1]);
			k+=rld-rla;
			rla[-4]=(k>>24)&255;
			rla[-3]=(k>>16)&255;
			rla[-2]=(k>> 8)&255;
			rla[-1]=(k    )&255;
			break;
		default:
			break;
		}
	}

	if(obj->decl->defv)
	{
		obj->decl->name=obj->decl->defv->name;
		obj->decl->sig=obj->decl->defv->sig;
		obj->decl->flagstr=obj->decl->defv->flagstr;
	}

	i=ctx->ip-ctx->ips;
	obj->decl->text=bgbcc_malloc(i);
	obj->decl->sz_text=i;
	memcpy(obj->decl->text, ctx->ips, i);
	
	BGBCC_FrBC_DbgDisAsmFunction(ctx, obj);
}

void BGBCC_FrBC_End(BGBCC_FrCC_State *ctx)
{
	BGBCC_FrCC_LiteralInfo *obj;
	frbc_register reg;
	int i, j, k;

	obj=ctx->cur_obj;
	switch(obj->littype)
	{
	case FR2C_LITID_FUNCTION:
		BGBCC_FrBC_EmitCallRetDefault(ctx);
		BGBCC_FrBC_EndFunction(ctx, obj);
		break;
	case FR2C_LITID_VALUE:
		break;
	case FR2C_LITID_GLOBALVAR:
		if(obj->decl && obj->decl->sig)
		{
			BGBCC_FrBC_TypeFromSig(ctx,
				&(obj->decl->type), obj->decl->sig);
		}
		break;
	case FR2C_LITID_VAR:
		if(obj->decl && obj->decl->sig)
		{
			BGBCC_FrBC_TypeFromSig(ctx,
				&(obj->decl->type), obj->decl->sig);
		}
		break;

	case FR2C_LITID_LIST:
		switch(obj->parent->littype)
		{
		case FR2C_LITID_LIST:
			BGBCC_FrBC_GetRegForLiteralValue(ctx, &reg, obj->litid);
			BGBCC_FrBC_ListAddLiteral(ctx, obj->parent, reg);
			break;
		case FR2C_LITID_VALUE:
			break;
		default:
			break;
		}
		break;
	}

	ctx->cur_objstackpos--;
	if(ctx->cur_objstackpos>0)
	{
		ctx->cur_obj=ctx->cur_objstack[ctx->cur_objstackpos-1];
	}else
	{
		ctx->cur_obj=NULL;
	}

//	BGBCC_FrBC_StubError(ctx);
}

void BGBCC_FrBC_AttribStr(BGBCC_FrCC_State *ctx, int attr, char *str)
{
	BGBCC_FrCC_LiteralInfo *obj;
	int i, j, k;

	obj=ctx->cur_obj;
	switch(obj->littype)
	{
	case FR2C_LITID_VAR:
	case FR2C_LITID_FUNCTION:
	case FR2C_LITID_GLOBALVAR:
	case FR2C_LITID_STRUCT:
	case FR2C_LITID_UNION:
		switch(attr)
		{
		case FR2C_ATTR_NAME:
			obj->decl->name=bgbcc_strdup(str);
			if(!obj->name)obj->name=obj->decl->name;
			break;
		case FR2C_ATTR_SIG:
			obj->decl->sig=bgbcc_strdup(str);
			if(!obj->sig)obj->sig=obj->decl->sig;
			break;
		case FR2C_ATTR_FLAGS:
			obj->decl->flagstr=bgbcc_strdup(str);
			break;
		default:
			break;
		}
		break;
	default:
		BGBCC_FrBC_StubError(ctx);
		break;
	}
}

void BGBCC_FrBC_AttribInt(BGBCC_FrCC_State *ctx, int attr, int val)
{
	BGBCC_FrCC_LiteralInfo *obj;
	int i, j, k;

	obj=ctx->cur_obj;
	switch(obj->littype)
	{
	case FR2C_LITID_VAR:
	case FR2C_LITID_FUNCTION:
	case FR2C_LITID_GLOBALVAR:
	case FR2C_LITID_STRUCT:
	case FR2C_LITID_UNION:
		switch(attr)
		{
		case FR2C_ATTR_FLAGS:
			obj->decl->flagsint=val;
			break;
		default:
			break;
		}
		break;
	default:
		BGBCC_FrBC_StubError(ctx);
		break;
	}
}

void BGBCC_FrBC_AttribLong(BGBCC_FrCC_State *ctx, int attr, s64 val)
{
	BGBCC_FrCC_LiteralInfo *obj;
	int i, j, k;

	obj=ctx->cur_obj;
	switch(obj->littype)
	{
	case FR2C_LITID_VAR:
	case FR2C_LITID_FUNCTION:
	case FR2C_LITID_GLOBALVAR:
	case FR2C_LITID_STRUCT:
	case FR2C_LITID_UNION:
		switch(attr)
		{
		case FR2C_ATTR_FLAGS:
			obj->decl->flagsint=val;
			break;
		default:
			break;
		}
		break;
	default:
		BGBCC_FrBC_StubError(ctx);
		break;
	}
}


void BGBCC_FrBC_Marker(BGBCC_FrCC_State *ctx, int tag)
{
	switch(tag)
	{
	case FR2C_CMD_VARARGS:
		break;
	default:
		BGBCC_FrBC_StubError(ctx);
	}
}

void BGBCC_FrBC_ListAddLiteral(BGBCC_FrCC_State *ctx, 
	BGBCC_FrCC_LiteralInfo *list, frbc_register val)
{
	BGBCC_FrCC_RegisterInfo *def;
	int i;

	def=list->decl;
	if(!def->listdata)
	{
		def->listdata=bgbcc_malloc(64*sizeof(frbc_register));
		def->n_listdata=0;
		def->m_listdata=64;
	}

	if((def->n_listdata+1)>=def->m_listdata)
	{
		i=def->m_listdata;
		i=i+(i>>1);
		def->listdata=bgbcc_realloc(def->listdata,
			i*sizeof(frbc_register));
		def->m_listdata=i;
	}

	i=def->n_listdata++;
	def->listdata[i]=val;

//	BGBCC_FrBC_StubError(ctx);
}

void BGBCC_FrBC_LiteralInt(BGBCC_FrCC_State *ctx, int attr, s32 val)
{
	BGBCC_FrCC_LiteralInfo *obj;
	frbc_register reg;
	int i, j, k;

	obj=ctx->cur_obj;
	switch(obj->littype)
	{
	case FR2C_LITID_LIST:
		BGBCC_FrBC_GetRegForIntValue(ctx, &reg, val);
		BGBCC_FrBC_ListAddLiteral(ctx, ctx->cur_obj, reg);
		break;
	case FR2C_LITID_VALUE:
		BGBCC_FrBC_GetRegForIntValue(ctx, &reg, val);
		obj->parent->decl->value=reg;
		break;
	default:
		BGBCC_FrBC_StubError(ctx);
		break;
	}
}

void BGBCC_FrBC_LiteralLong(BGBCC_FrCC_State *ctx, int attr, s64 val)
{
	BGBCC_FrCC_LiteralInfo *obj;
	frbc_register reg;
	int i, j, k;

	obj=ctx->cur_obj;
	switch(obj->littype)
	{
	case FR2C_LITID_LIST:
		BGBCC_FrBC_GetRegForLongValue(ctx, &reg, val);
		BGBCC_FrBC_ListAddLiteral(ctx, ctx->cur_obj, reg);
		break;
	case FR2C_LITID_VALUE:
		BGBCC_FrBC_GetRegForLongValue(ctx, &reg, val);
		obj->parent->decl->value=reg;
		break;
	default:
		BGBCC_FrBC_StubError(ctx);
		break;
	}
}

void BGBCC_FrBC_LiteralFloat(BGBCC_FrCC_State *ctx, int attr, double val)
{
	BGBCC_FrCC_LiteralInfo *obj;
	frbc_register reg;
	int i, j, k;

	obj=ctx->cur_obj;
	switch(obj->littype)
	{
	case FR2C_LITID_LIST:
		BGBCC_FrBC_GetRegForFloatValue(ctx, &reg, val);
		BGBCC_FrBC_ListAddLiteral(ctx, ctx->cur_obj, reg);
		break;
	case FR2C_LITID_VALUE:
		BGBCC_FrBC_GetRegForFloatValue(ctx, &reg, val);
		obj->parent->decl->value=reg;
		break;
	default:
		BGBCC_FrBC_StubError(ctx);
		break;
	}
}

void BGBCC_FrBC_LiteralDouble(BGBCC_FrCC_State *ctx, int attr, double val)
{
	BGBCC_FrCC_LiteralInfo *obj;
	frbc_register reg;
	int i, j, k;

	obj=ctx->cur_obj;
	switch(obj->littype)
	{
	case FR2C_LITID_LIST:
		BGBCC_FrBC_GetRegForDoubleValue(ctx, &reg, val);
		BGBCC_FrBC_ListAddLiteral(ctx, ctx->cur_obj, reg);
		break;
	case FR2C_LITID_VALUE:
		BGBCC_FrBC_GetRegForDoubleValue(ctx, &reg, val);
		obj->parent->decl->value=reg;
		break;
	default:
		BGBCC_FrBC_StubError(ctx);
		break;
	}
}

void BGBCC_FrBC_LiteralStr(BGBCC_FrCC_State *ctx, int attr, char *str)
{
	BGBCC_FrCC_LiteralInfo *obj;
	frbc_register reg;
	int i, j, k;

	obj=ctx->cur_obj;
	switch(obj->littype)
	{
	case FR2C_LITID_LIST:
		BGBCC_FrBC_GetRegForStringValue(ctx, &reg, str);
		BGBCC_FrBC_ListAddLiteral(ctx, ctx->cur_obj, reg);
		break;
	case FR2C_LITID_VALUE:
		BGBCC_FrBC_GetRegForStringValue(ctx, &reg, str);
		obj->parent->decl->value=reg;
		break;
	default:
		BGBCC_FrBC_StubError(ctx);
		break;
	}
}

void BGBCC_FrBC_LiteralWStr(BGBCC_FrCC_State *ctx, int attr, char *str)
{
	BGBCC_FrCC_LiteralInfo *obj;
	frbc_register reg;
	int i, j, k;

	obj=ctx->cur_obj;
	switch(obj->littype)
	{
	case FR2C_LITID_LIST:
		BGBCC_FrBC_GetRegForWStringValue(ctx, &reg, str);
		BGBCC_FrBC_ListAddLiteral(ctx, ctx->cur_obj, reg);
		break;
	case FR2C_LITID_VALUE:
		BGBCC_FrBC_GetRegForWStringValue(ctx, &reg, str);
		obj->parent->decl->value=reg;
		break;
	default:
		BGBCC_FrBC_StubError(ctx);
		break;
	}
}

byte *BGBCC_FrBC_EmitBufVLIE(byte *ct, s64 vli)
{
	if(vli<0)
	{
		*ct++=0x80|((-vli)&63);
		return(ct);
	}
	
	if(vli<0x80)
	{
		*ct++=vli;
	}else if(vli<0x2000)
	{
		*ct++=0xC0|((vli>>8)&31);
		*ct++=vli&255;
	}else if(vli<0x100000)
	{
		*ct++=0xE0|((vli>>16)&15);
		*ct++=(vli>> 8)&255;
		*ct++=(vli    )&255;
	}else if(vli<0x8000000)
	{
		*ct++=0xF0|((vli>>24)&7);
		*ct++=(vli>>16)&255;
		*ct++=(vli>> 8)&255;
		*ct++=(vli    )&255;
	}else if(vli<0x400000000)
	{
		*ct++=0xF8|((vli>>32)&3);
		*ct++=(vli>>24)&255;
		*ct++=(vli>>16)&255;
		*ct++=(vli>> 8)&255;
		*ct++=(vli    )&255;
	}else if(vli<0x20000000000)
	{
		*ct++=0xFC|((vli>>40)&1);
		*ct++=(vli>>32)&255;
		*ct++=(vli>>24)&255;
		*ct++=(vli>>16)&255;
		*ct++=(vli>> 8)&255;
		*ct++=(vli    )&255;
	}else if(vli<0x1000000000000)
	{
		*ct++=0xFE;
		*ct++=(vli>>40)&255;
		*ct++=(vli>>32)&255;
		*ct++=(vli>>24)&255;
		*ct++=(vli>>16)&255;
		*ct++=(vli>> 8)&255;
		*ct++=(vli    )&255;
	}else if(vli<0x80000000000000)
	{
		*ct++=0xFF;
		*ct++=0x80|((vli>>48)&63);
		*ct++=(vli>>40)&255;
		*ct++=(vli>>32)&255;
		*ct++=(vli>>24)&255;
		*ct++=(vli>>16)&255;
		*ct++=(vli>> 8)&255;
		*ct++=(vli    )&255;
	}else if(vli<0x4000000000000000)
	{
		*ct++=0xFF;
		*ct++=0x80|((vli>>56)&31);
		*ct++=(vli>>48)&255;
		*ct++=(vli>>40)&255;
		*ct++=(vli>>32)&255;
		*ct++=(vli>>24)&255;
		*ct++=(vli>>16)&255;
		*ct++=(vli>> 8)&255;
		*ct++=(vli    )&255;
	}else
	{
		*ct++=0xFF;
		*ct++=0xC0;
		*ct++=(vli>>56)&255;
		*ct++=(vli>>48)&255;
		*ct++=(vli>>40)&255;
		*ct++=(vli>>32)&255;
		*ct++=(vli>>24)&255;
		*ct++=(vli>>16)&255;
		*ct++=(vli>> 8)&255;
		*ct++=(vli    )&255;
	}

	return(ct);
}

void BGBCC_FrBC_FlattenImage_EmitGlobalVarDecl(
	BGBCC_FrCC_State *ctx, byte *obuf, int omsz,
	byte **rct, BGBCC_FrCC_RegisterInfo *gbl)
{
	byte *ct, *ct1, *ctm0;
	int sz;
	int i, j, k;
	
	if(!obuf)
	{
		if(ctx->ctxflags&BGBCC_CTXFL_SAVENAMES)
			{ gbl->flagsint|=BGBCC_TYFL_EXPNAME; }
	
		if(gbl->name && (gbl->flagsint&BGBCC_TYFL_EXPNAME))
			{ BGBCC_FrBC_IndexCountString(ctx, gbl->name); }
		if(gbl->sig)
			{ BGBCC_FrBC_IndexCountString(ctx, gbl->sig); }
		if(gbl->flagstr)
			{ BGBCC_FrBC_IndexCountString(ctx, gbl->flagstr); }

		return;
	}

	ct=*rct;

	ctm0=ct;
//	*ct++=0xE4;		*ct++=0;
//	*ct++=0;		*ct++=0;
//	*ct++='D';		*ct++='V';
//	*ct++='A';		*ct++='R';

	*ct++=0xE5;		*ct++=0;
	*ct++='D';		*ct++='V';
	
	if(gbl->name && (gbl->flagsint&BGBCC_TYFL_EXPNAME))
	{
		i=BGBCC_FrBC_LookupString(ctx, gbl->name);
		if(i>0)
		{
			*ct++=FR2C_TAG_NAME;
			ct=BGBCC_FrBC_EmitBufVLIE(ct, i);
		}
	}

	if(gbl->sig)
	{
		i=BGBCC_FrBC_LookupString(ctx, gbl->sig);
		if(i>0)
		{
			*ct++=FR2C_TAG_SIG;
			ct=BGBCC_FrBC_EmitBufVLIE(ct, i);
		}
	}

	if(gbl->flagstr)
	{
		i=BGBCC_FrBC_LookupString(ctx, gbl->flagstr);
		if(i>0)
		{
			*ct++=FR2C_TAG_FLAGS;
			ct=BGBCC_FrBC_EmitBufVLIE(ct, i);
		}
	}

	if(gbl->validx>0)
	{
		*ct++=FR2C_TAG_VALUE;
		ct=BGBCC_FrBC_EmitBufVLIE(ct, gbl->validx);
	}

	i=ct-ctm0;
//	ctm0[1]=(i>>16)&255;
//	ctm0[2]=(i>> 8)&255;
//	ctm0[3]=(i    )&255;
	ctm0[1]=i&255;

	*rct=ct;
}

void BGBCC_FrBC_FlattenImage_EmitGlobalFuncDecl(
	BGBCC_FrCC_State *ctx, byte *obuf, int omsz,
	byte **rct, BGBCC_FrCC_RegisterInfo *gbl)
{
	byte *ct, *ct1, *ctm0;
	int sz;
	int i, j, k;

	if(!obuf)
	{
		if(ctx->ctxflags&BGBCC_CTXFL_SAVENAMES)
			{ gbl->flagsint|=BGBCC_TYFL_EXPNAME; }

		if(!(gbl->flagsint&BGBCC_TYFL_DLLEXPORT) &&
			!(gbl->regflags&BGBCC_REGFL_ACCESSED))
		{
			gbl->regflags|=BGBCC_REGFL_CULL;
			return;
		}
		
		if(gbl->sz_text<=0)
		{
			gbl->flagsint|=BGBCC_TYFL_EXPNAME;
		}

		if(gbl->name && (gbl->flagsint&BGBCC_TYFL_EXPNAME))
			{ BGBCC_FrBC_IndexCountString(ctx, gbl->name); }
		if(gbl->sig)
			{ BGBCC_FrBC_IndexCountString(ctx, gbl->sig); }
		if(gbl->flagstr)
			{ BGBCC_FrBC_IndexCountString(ctx, gbl->flagstr); }

		return;
	}

	sz=64;
	if(gbl->text)
		{ sz+=gbl->sz_text; }

	ct=*rct;

	ctm0=ct;
	if(sz>=256)
	{
		*ct++=0xE4;		*ct++=0;
		*ct++=0;		*ct++=0;
		*ct++='F';		*ct++='U';
		*ct++='N';		*ct++='C';
	}else
	{
		*ct++=0xE5;		*ct++=0;
		*ct++='F';		*ct++='N';
	}
	
	if(gbl->name && (gbl->flagsint&BGBCC_TYFL_EXPNAME))
	{
		i=BGBCC_FrBC_LookupString(ctx, gbl->name);
		if(i>0)
		{
			*ct++=FR2C_TAG_NAME;
			ct=BGBCC_FrBC_EmitBufVLIE(ct, i);
		}
	}

	if(gbl->sig)
	{
		i=BGBCC_FrBC_LookupString(ctx, gbl->sig);
		if(i>0)
		{
			*ct++=FR2C_TAG_SIG;
			ct=BGBCC_FrBC_EmitBufVLIE(ct, i);
		}
	}

	if(gbl->flagstr)
	{
		i=BGBCC_FrBC_LookupString(ctx, gbl->flagstr);
		if(i>0)
		{
			*ct++=FR2C_TAG_FLAGS;
			ct=BGBCC_FrBC_EmitBufVLIE(ct, i);
		}
	}
	
	if(gbl->n_args>0)
	{
		i=gbl->n_args;
		*ct++=FR2C_TAG_NARGS;
		ct=BGBCC_FrBC_EmitBufVLIE(ct, i);
	}

	if(gbl->n_locals>0)
	{
		i=gbl->n_locals;
		*ct++=FR2C_TAG_NLOCALS;
		ct=BGBCC_FrBC_EmitBufVLIE(ct, i);
	}

	if(gbl->n_regs>0)
	{
		i=gbl->n_regs;
		*ct++=FR2C_TAG_NREGS;
		ct=BGBCC_FrBC_EmitBufVLIE(ct, i);
	}

	if(gbl->n_cargs>0)
	{
		i=gbl->n_cargs;
		*ct++=FR2C_TAG_NCARGS;
		ct=BGBCC_FrBC_EmitBufVLIE(ct, i);
	}
	
	if(gbl->text && (gbl->sz_text>0))
	{
		if(gbl->sz_text>=248)
		{
			i=gbl->sz_text+4; ct1=ct+i;
			*ct++=0xE1;			*ct++=(i>>16)&255;
			*ct++=(i>>8)&255;	*ct++=(i    )&255;
			memcpy(ct, gbl->text, gbl->sz_text);
			ct=ct1;
		}else
		{
			i=gbl->sz_text+2; ct1=ct+i;
			*ct++=0xE2;			*ct++=i&255;
			memcpy(ct, gbl->text, gbl->sz_text);
			ct=ct1;
		}
	}
	
//	if(gbl->validx>0)
//	{
//		i=gbl->validx;
//		*ct++=FR2C_TAG_VALUE;
//		ct=BGBCC_FrBC_EmitBufVLIE(ct, i);
//	}

	if(sz>=256)
	{
		i=ct-ctm0;
		if((i<0) || (i>=16777216))
			{ BGBCC_FrBC_TagError(ctx, FR2C_TERR_FORMAT_SANITY); }
		ctm0[1]=(i>>16)&255;
		ctm0[2]=(i>> 8)&255;
		ctm0[3]=(i    )&255;
	}else
	{
		i=ct-ctm0;
		if((i<0) || (i>=256))
			{ BGBCC_FrBC_TagError(ctx, FR2C_TERR_FORMAT_SANITY); }
		ctm0[1]=i&255;
	}

	*rct=ct;
}

void BGBCC_FrBC_FlattenImage_EmitGlobalNullDecl(
	BGBCC_FrCC_State *ctx, byte *obuf, int omsz,
	byte **rct, BGBCC_FrCC_RegisterInfo *gbl)
{
	byte *ct;
	
	if(!obuf)
		return;
	
	ct=*rct;
	*ct++=0xE4;		*ct++=0;
	*ct++=0;		*ct++=8;
	*ct++='N';		*ct++='U';
	*ct++='L';		*ct++='L';
	*rct=ct;
}

void BGBCC_FrBC_FlattenImage_EmitLiteralNullDecl(
	BGBCC_FrCC_State *ctx, byte *obuf, int omsz,
	byte **rct, BGBCC_FrCC_LiteralInfo *lit)
{
	byte *ct;	

	if(!obuf)
		return;

	ct=*rct;
	*ct++=0xE4;		*ct++=0;
	*ct++=0;		*ct++=8;
	*ct++='N';		*ct++='U';
	*ct++='L';		*ct++='L';
	*rct=ct;
}

void BGBCC_FrBC_FlattenImage_EmitGlobal(
	BGBCC_FrCC_State *ctx, byte *obuf, int omsz,
	byte **rct, int gidx)
{
	BGBCC_FrCC_RegisterInfo *gbl;
	
	gbl=ctx->reg_globals[gidx];
	if(!gbl)
	{
		BGBCC_FrBC_FlattenImage_EmitGlobalNullDecl(
			ctx, obuf, omsz, rct, gbl);
		return;
	}
	
	switch(gbl->regtype)
	{
	case FR2C_LITID_GLOBALVAR:
		BGBCC_FrBC_FlattenImage_EmitGlobalVarDecl(
			ctx, obuf, omsz, rct, gbl);
		break;
	case FR2C_LITID_FUNCTION:
		BGBCC_FrBC_FlattenImage_EmitGlobalFuncDecl(
			ctx, obuf, omsz, rct, gbl);
		break;
	default:
		BGBCC_FrBC_FlattenImage_EmitGlobalNullDecl(
			ctx, obuf, omsz, rct, gbl);
		break;
	}
}

void BGBCC_FrBC_FlattenImage_EmitStructDefField(
	BGBCC_FrCC_State *ctx, byte *obuf, int omsz,
	byte **rct, BGBCC_FrCC_RegisterInfo *gbl)
{
	byte *ct, *ct1, *ctm0;
	int sz, svn;
	int i, j, k;
	
	if(!obuf)
	{
		if(ctx->ctxflags&BGBCC_CTXFL_SAVENAMES)
			{ gbl->flagsint|=BGBCC_TYFL_EXPNAME; }

		if(gbl->name && (gbl->flagsint&BGBCC_TYFL_EXPNAME))
			{ BGBCC_FrBC_IndexCountString(ctx, gbl->name); }
		if(gbl->sig)
			{ BGBCC_FrBC_IndexCountString(ctx, gbl->sig); }
		if(gbl->flagstr)
			{ BGBCC_FrBC_IndexCountString(ctx, gbl->flagstr); }

		return;
	}

	ct=*rct;

	ctm0=ct;
	*ct++=0xE5;		*ct++=0;
	*ct++='D';		*ct++='F';
	
	if(gbl->name && (gbl->flagsint&BGBCC_TYFL_EXPNAME))
	{
		i=BGBCC_FrBC_LookupString(ctx, gbl->name);
		if(i>0)
		{
			*ct++=FR2C_TAG_NAME;
			ct=BGBCC_FrBC_EmitBufVLIE(ct, i);
		}
	}

	if(gbl->sig)
	{
		i=BGBCC_FrBC_LookupString(ctx, gbl->sig);
		if(i>0)
		{
			*ct++=FR2C_TAG_SIG;
			ct=BGBCC_FrBC_EmitBufVLIE(ct, i);
		}
	}

	if(gbl->flagstr)
	{
		i=BGBCC_FrBC_LookupString(ctx, gbl->flagstr);
		if(i>0)
		{
			*ct++=FR2C_TAG_FLAGS;
			ct=BGBCC_FrBC_EmitBufVLIE(ct, i);
		}
	}

	if(gbl->validx>0)
	{
		*ct++=FR2C_TAG_VALUE;
		ct=BGBCC_FrBC_EmitBufVLIE(ct, gbl->validx);
	}

	i=ct-ctm0;
	ctm0[1]=i&255;

	*rct=ct;
}

void BGBCC_FrBC_FlattenImage_EmitStructDef(
	BGBCC_FrCC_State *ctx, byte *obuf, int omsz,
	byte **rct, BGBCC_FrCC_LiteralInfo *obj)
{
	BGBCC_FrCC_RegisterInfo *gbl;
	byte *ct, *ct1, *ctm0;
	int sz;
	int i, j, k;

	gbl=obj->decl;

	if(!gbl)
	{
		if(!obuf)
			return;

		ct=*rct;
		*ct++=0xE5;		*ct++=0;
		*ct++='S';		*ct++='T';
		*rct=ct;
		return;

//		obj->decl=bgbcc_malloc(sizeof(BGBCC_FrCC_RegisterInfo));
//		gbl=obj->decl;
	}

	if(!obuf)
	{
		if(ctx->ctxflags&BGBCC_CTXFL_SAVENAMES)
			{ gbl->flagsint|=BGBCC_TYFL_EXPNAME; }

		if(gbl->name && (gbl->flagsint&BGBCC_TYFL_EXPNAME))
			{ BGBCC_FrBC_IndexCountString(ctx, gbl->name); }
		if(gbl->sig)
			{ BGBCC_FrBC_IndexCountString(ctx, gbl->sig); }
		if(gbl->flagstr)
			{ BGBCC_FrBC_IndexCountString(ctx, gbl->flagstr); }

		for(i=0; i<gbl->n_fields; i++)
		{
			BGBCC_FrBC_FlattenImage_EmitStructDefField(
				ctx, obuf, omsz, rct, gbl->fields[i]);
		}

		return;
	}

	ct=*rct;

	ctm0=ct;
	*ct++=0xE4;		*ct++=0;
	*ct++=0;		*ct++=0;
	*ct++='D';		*ct++='S';
	*ct++='T';		*ct++='R';
	
	if(obj->littype==FR2C_LITID_UNION)
	{
		ct[-1]='U';
	}

	if(gbl->name && (gbl->flagsint&BGBCC_TYFL_EXPNAME))
	{
		i=BGBCC_FrBC_LookupString(ctx, gbl->name);
		if(i>0)
		{
			*ct++=FR2C_TAG_NAME;
			ct=BGBCC_FrBC_EmitBufVLIE(ct, i);
		}
	}

	if(gbl->sig)
	{
		i=BGBCC_FrBC_LookupString(ctx, gbl->sig);
		if(i>0)
		{
			*ct++=FR2C_TAG_SIG;
			ct=BGBCC_FrBC_EmitBufVLIE(ct, i);
		}
	}

	if(gbl->flagstr)
	{
		i=BGBCC_FrBC_LookupString(ctx, gbl->flagstr);
		if(i>0)
		{
			*ct++=FR2C_TAG_FLAGS;
			ct=BGBCC_FrBC_EmitBufVLIE(ct, i);
		}
	}
	
	for(i=0; i<gbl->n_fields; i++)
	{
		BGBCC_FrBC_FlattenImage_EmitStructDefField(
			ctx, obuf, omsz, &ct, gbl->fields[i]);
	}
	
	i=ct-ctm0;
	ctm0[1]=(i>>16)&255;
	ctm0[2]=(i>> 8)&255;
	ctm0[3]=(i    )&255;

	*rct=ct;
}

void BGBCC_FrBC_FlattenImage_EmitTypeDef(
	BGBCC_FrCC_State *ctx, byte *obuf, int omsz,
	byte **rct, BGBCC_FrCC_LiteralInfo *obj)
{
	BGBCC_FrCC_RegisterInfo *gbl;
	byte *ct, *ct1, *ctm0;
	int sz;
	int i, j, k;

	gbl=obj->decl;

//	if(!gbl)
//	{
//		if(!obuf)
//			return;
//	}

	if(!obuf)
	{
//		if(ctx->ctxflags&BGBCC_CTXFL_SAVENAMES)
//			{ gbl->flagsint|=BGBCC_TYFL_EXPNAME; }

//		if(obj->name)
		if(obj->name && (ctx->ctxflags&BGBCC_CTXFL_SAVENAMES))
			{ BGBCC_FrBC_IndexCountString(ctx, obj->name); }
		if(obj->sig)
			{ BGBCC_FrBC_IndexCountString(ctx, obj->sig); }

		if(gbl && gbl->flagstr)
			{ BGBCC_FrBC_IndexCountString(ctx, gbl->flagstr); }

		if(gbl)
		{
			for(i=0; i<gbl->n_fields; i++)
			{
				BGBCC_FrBC_FlattenImage_EmitStructDefField(
					ctx, obuf, omsz, rct, gbl->fields[i]);
			}
		}

		return;
	}

	ct=*rct;

	ctm0=ct;
	*ct++=0xE4;		*ct++=0;
	*ct++=0;		*ct++=0;
	*ct++='D';		*ct++='T';
	*ct++='Y';		*ct++='D';
	
//	if(gbl->name && (gbl->flagsint&BGBCC_TYFL_EXPNAME))
//	if(cur->name)
	if(obj->name && (ctx->ctxflags&BGBCC_CTXFL_SAVENAMES))
	{
		i=BGBCC_FrBC_LookupString(ctx, obj->name);
		if(i>0)
		{
			*ct++=FR2C_TAG_NAME;
			ct=BGBCC_FrBC_EmitBufVLIE(ct, i);
		}
	}

	if(obj->sig)
	{
		i=BGBCC_FrBC_LookupString(ctx, obj->sig);
		if(i>0)
		{
			*ct++=FR2C_TAG_SIG;
			ct=BGBCC_FrBC_EmitBufVLIE(ct, i);
		}
	}

	if(gbl && gbl->flagstr)
	{
		i=BGBCC_FrBC_LookupString(ctx, gbl->flagstr);
		if(i>0)
		{
			*ct++=FR2C_TAG_FLAGS;
			ct=BGBCC_FrBC_EmitBufVLIE(ct, i);
		}
	}
	
	if(gbl)
	{
		for(i=0; i<gbl->n_fields; i++)
		{
			BGBCC_FrBC_FlattenImage_EmitStructDefField(
				ctx, obuf, omsz, &ct, gbl->fields[i]);
		}
	}
	
	i=ct-ctm0;
	ctm0[1]=(i>>16)&255;
	ctm0[2]=(i>> 8)&255;
	ctm0[3]=(i    )&255;

	*rct=ct;
}

void BGBCC_FrBC_FlattenImage_EmitLiteral(
	BGBCC_FrCC_State *ctx, byte *obuf, int omsz,
	byte **rct, int idx)
{
	BGBCC_FrCC_LiteralInfo *lit;
	
	lit=ctx->literals[idx];
	if(!lit)
	{
		BGBCC_FrBC_FlattenImage_EmitLiteralNullDecl(
			ctx, obuf, omsz, rct, lit);
		return;
	}
	
	switch(lit->littype)
	{
//	case FR2C_LITID_GLOBALVAR:
//		BGBCC_FrBC_FlattenImage_EmitGlobalVarDecl(
//			ctx, obuf, omsz, rct, gbl);
//		break;
//	case FR2C_LITID_FUNCTION:
//		BGBCC_FrBC_FlattenImage_EmitGlobalFuncDecl(
//			ctx, obuf, omsz, rct, gbl);
//		break;

	case FR2C_LITID_STRUCT:
	case FR2C_LITID_UNION:
		BGBCC_FrBC_FlattenImage_EmitStructDef(
			ctx, obuf, omsz, rct, lit);
		break;
	case FR2C_LITID_TYPEDEF:
		BGBCC_FrBC_FlattenImage_EmitTypeDef(
			ctx, obuf, omsz, rct, lit);
		break;
	default:
		BGBCC_FrBC_FlattenImage_EmitLiteralNullDecl(
			ctx, obuf, omsz, rct, lit);
		break;
	}
}

void BGBCC_FrBC_FlattenImage_Globals(
	BGBCC_FrCC_State *ctx, byte *obuf, int omsz, byte **rct)
{
	BGBCC_FrCC_RegisterInfo *gbl;
	byte *ct, *ct1, *ctm0;
	int sz;
	int i, j, k;
	
	if(ctx->n_reg_globals<=0)
		return;
	
	ct=*rct;

	ctm0=ct;
	*ct++=0xE4;		*ct++=0;
	*ct++=0;		*ct++=0;
	*ct++='G';		*ct++='D';
	*ct++='E';		*ct++='F';
	
	for(i=0; i<ctx->n_reg_globals; i++)
	{
		gbl=ctx->reg_globals[i];
		if(gbl && gbl->regflags&BGBCC_REGFL_CULL)
			{ ctx->idx_globals[i]=0; continue; }
	
		ctx->idx_globals[i]=ct-(ctm0+8);
		BGBCC_FrBC_FlattenImage_EmitGlobal(ctx, obuf, omsz, &ct, i);
	}

	i=ct-ctm0;
	ctm0[1]=(i>>16)&255;
	ctm0[2]=(i>> 8)&255;
	ctm0[3]=(i    )&255;
	sz=i;

	if(sz>=16777216)
	{
		i=ctx->n_reg_globals*4+8; ct1=ct+i;
		*ct++=0xE4;			*ct++=(i>>16)&255;
		*ct++=(i>>8)&255;	*ct++=(i    )&255;
		*ct++='G';	*ct++='I';	*ct++='D';	*ct++='X';
		
		for(i=0; i<ctx->n_reg_globals; i++)
		{
			ct[i*4+0]=(ctx->idx_globals[i]>>24)&255;
			ct[i*4+1]=(ctx->idx_globals[i]>>16)&255;
			ct[i*4+2]=(ctx->idx_globals[i]>> 8)&255;
			ct[i*4+3]=(ctx->idx_globals[i]    )&255;
		}
		ct=ct1;
	}else if(sz>=65536)
	{
		i=ctx->n_reg_globals*3+8; ct1=ct+i;
		*ct++=0xE4;			*ct++=(i>>16)&255;
		*ct++=(i>>8)&255;	*ct++=(i    )&255;
		*ct++='G';	*ct++='I';	*ct++='X';	*ct++='3';
		
		for(i=0; i<ctx->n_reg_globals; i++)
		{
			ct[i*3+0]=(ctx->idx_globals[i]>>16)&255;
			ct[i*3+1]=(ctx->idx_globals[i]>> 8)&255;
			ct[i*3+2]=(ctx->idx_globals[i]    )&255;
		}
		ct=ct1;
	}else
	{
		i=ctx->n_reg_globals*2+8; ct1=ct+i;
		*ct++=0xE4;			*ct++=(i>>16)&255;
		*ct++=(i>>8)&255;	*ct++=(i    )&255;
		*ct++='G';	*ct++='I';	*ct++='X';	*ct++='2';
		
		for(i=0; i<ctx->n_reg_globals; i++)
		{
			ct[i*2+0]=(ctx->idx_globals[i]>> 8)&255;
			ct[i*2+1]=(ctx->idx_globals[i]    )&255;
		}
		ct=ct1;
	}

	*rct=ct;
}

void BGBCC_FrBC_FlattenImage_Literals(
	BGBCC_FrCC_State *ctx, byte *obuf, int omsz, byte **rct)
{
	byte *ct, *ct1, *ctm0;
	int sz;
	int i, j, k;
	
	if(ctx->n_reg_globals<=0)
		return;
	
	ct=*rct;

	ctm0=ct;
	*ct++=0xE4;		*ct++=0;
	*ct++=0;		*ct++=0;
	*ct++='L';		*ct++='D';
	*ct++='E';		*ct++='F';
	
	for(i=0; i<ctx->n_literals; i++)
	{
		ctx->idx_literals[i]=ct-(ctm0+8);
		BGBCC_FrBC_FlattenImage_EmitLiteral(ctx, obuf, omsz, &ct, i);
	}

	i=ct-ctm0;
	ctm0[1]=(i>>16)&255;
	ctm0[2]=(i>> 8)&255;
	ctm0[3]=(i    )&255;
	sz=i;

	if(sz>=16777216)
	{
		i=ctx->n_literals*4+8; ct1=ct+i;
		*ct++=0xE4;			*ct++=(i>>16)&255;
		*ct++=(i>>8)&255;	*ct++=(i    )&255;
		*ct++='L';	*ct++='I';	*ct++='D';	*ct++='X';
		
		for(i=0; i<ctx->n_literals; i++)
		{
			ct[i*4+0]=(ctx->idx_literals[i]>>24)&255;
			ct[i*4+1]=(ctx->idx_literals[i]>>16)&255;
			ct[i*4+2]=(ctx->idx_literals[i]>> 8)&255;
			ct[i*4+3]=(ctx->idx_literals[i]    )&255;
		}
		ct=ct1;
	}else if(sz>=65536)
	{
		i=ctx->n_literals*3+8; ct1=ct+i;
		*ct++=0xE4;			*ct++=(i>>16)&255;
		*ct++=(i>>8)&255;	*ct++=(i    )&255;
		*ct++='L';	*ct++='I';	*ct++='X';	*ct++='3';
		
		for(i=0; i<ctx->n_literals; i++)
		{
			ct[i*3+0]=(ctx->idx_literals[i]>>16)&255;
			ct[i*3+1]=(ctx->idx_literals[i]>> 8)&255;
			ct[i*3+2]=(ctx->idx_literals[i]    )&255;
		}
		ct=ct1;
	}else
	{
		i=ctx->n_literals*2+8; ct1=ct+i;
		*ct++=0xE4;			*ct++=(i>>16)&255;
		*ct++=(i>>8)&255;	*ct++=(i    )&255;
		*ct++='L';	*ct++='I';	*ct++='X';	*ct++='2';
		
		for(i=0; i<ctx->n_literals; i++)
		{
			ct[i*2+0]=(ctx->idx_literals[i]>>8)&255;
			ct[i*2+1]=(ctx->idx_literals[i]   )&255;
		}
		ct=ct1;
	}

	*rct=ct;
}

void BGBCC_FrBC_PreFlattenImage_Globals(BGBCC_FrCC_State *ctx)
{
	int i;

	for(i=0; i<ctx->n_reg_globals; i++)
	{
		BGBCC_FrBC_FlattenImage_EmitGlobal(ctx, NULL, 0, NULL, i);
	}
}

void BGBCC_FrBC_PreFlattenImage_Literals(BGBCC_FrCC_State *ctx)
{
	int i;

	for(i=0; i<ctx->n_literals; i++)
	{
		BGBCC_FrBC_FlattenImage_EmitLiteral(ctx, NULL, 0, NULL, i);
	}
}

int BGBCC_FrBC_FlattenImage(BGBCC_FrCC_State *ctx, byte *obuf, int omsz)
{
	byte *ct, *ct1;
	int sz;
	int i, j, k;
	
	BGBCC_FrBC_PreFlattenImage_Globals(ctx);
	BGBCC_FrBC_PreFlattenImage_Literals(ctx);
	BGBCC_FrBC_FlattenCountStrings(ctx);

	ct=obuf;
	*ct++='F';	*ct++='R';	*ct++='B';	*ct++='C';
	*ct++='2';	*ct++='C';	*ct++='0';	*ct++='0';
	*ct++=0;	*ct++=0;	*ct++=0;	*ct++=0;

	if(ctx->strtab)
	{
		i=ctx->sz_strtab+8; ct1=ct+i;
		*ct++=0xE4;			*ct++=(i>>16)&255;
		*ct++=(i>>8)&255;	*ct++=(i    )&255;
		*ct++='S';	*ct++='T';	*ct++='R';	*ct++='S';
		memcpy(ct, ctx->strtab, ctx->sz_strtab);
		ct=ct1;
	}

	if(ctx->wstrtab)
	{
		i=ctx->sz_wstrtab+8; ct1=ct+i;
		*ct++=0xE4;			*ct++=(i>>16)&255;
		*ct++=(i>>8)&255;	*ct++=(i    )&255;
		*ct++='W';	*ct++='S';	*ct++='T';	*ct++='R';
		memcpy(ct, ctx->wstrtab, ctx->sz_wstrtab);
		ct=ct1;
	}

	if(ctx->ctab_lvt4)
	{
		i=ctx->n_ctab_lvt4*4+8; ct1=ct+i;
		*ct++=0xE4;			*ct++=(i>>16)&255;
		*ct++=(i>>8)&255;	*ct++=(i    )&255;
		*ct++='L';	*ct++='V';	*ct++='T';	*ct++='4';
		
		for(i=0; i<ctx->n_ctab_lvt4; i++)
		{
			ct[i*4+0]=(ctx->ctab_lvt4[i]>>24)&255;
			ct[i*4+1]=(ctx->ctab_lvt4[i]>>16)&255;
			ct[i*4+2]=(ctx->ctab_lvt4[i]>> 8)&255;
			ct[i*4+3]=(ctx->ctab_lvt4[i]    )&255;
		}

		ct=ct1;
	}

	if(ctx->ctab_lvt8)
	{
		i=ctx->n_ctab_lvt8*8+8; ct1=ct+i;
		*ct++=0xE4;			*ct++=(i>>16)&255;
		*ct++=(i>>8)&255;	*ct++=(i    )&255;
		*ct++='L';	*ct++='V';	*ct++='T';	*ct++='8';
		
		for(i=0; i<ctx->n_ctab_lvt8; i++)
		{
			ct[i*8+0]=(ctx->ctab_lvt8[i]>>56)&255;
			ct[i*8+1]=(ctx->ctab_lvt8[i]>>48)&255;
			ct[i*8+2]=(ctx->ctab_lvt8[i]>>40)&255;
			ct[i*8+3]=(ctx->ctab_lvt8[i]>>32)&255;
			ct[i*8+4]=(ctx->ctab_lvt8[i]>>24)&255;
			ct[i*8+5]=(ctx->ctab_lvt8[i]>>16)&255;
			ct[i*8+6]=(ctx->ctab_lvt8[i]>> 8)&255;
			ct[i*8+7]=(ctx->ctab_lvt8[i]    )&255;
		}

		ct=ct1;
	}

	BGBCC_FrBC_FlattenImage_Globals(ctx, obuf, omsz, &ct);
	BGBCC_FrBC_FlattenImage_Literals(ctx, obuf, omsz, &ct);

	sz=ct-obuf;
	obuf[ 8]=(sz>>24)&255;
	obuf[ 9]=(sz>>16)&255;
	obuf[10]=(sz>> 8)&255;
	obuf[11]=(sz    )&255;
	return(sz);
}
