#include "bggl3.h"

int bggl3_id_int, bggl3_id_float, bggl3_id_cons, bggl3_id_null;
int bggl3_id_symbol, bggl3_id_string;

BGGL3_Binding *bggl3_inittop=NULL;
BGGL3_Binding *bggl3_bindpool=NULL;

BGGL3_State *BGGL3_NewContext()
{
	BGGL3_State *tmp;

	tmp=malloc(sizeof(BGGL3_State));
	memset(tmp, 0, sizeof(BGGL3_State));

	tmp->env=bggl3_inittop;

	return(tmp);
}

BGGL3_Binding *BGGL3_AllocBinding()
{
	BGGL3_Binding *tmp;

	if(bggl3_bindpool)
	{
		tmp=bggl3_bindpool;
		bggl3_bindpool=tmp->next;
		return(tmp);
	}

	tmp=malloc(sizeof(BGGL3_Binding));
	return(tmp);
}

void BGGL3_BindValue(char *name, char *desc, elem val)
{
	BGGL3_Binding *tmp;

	tmp=BGGL3_AllocBinding();
	tmp->var=BGGL3_HashSymbol(name);
	tmp->val=val;

	tmp->next=bggl3_inittop;
	bggl3_inittop=tmp;
}

void BGGL3_AddBuiltin(char *name, char *desc,
	elem (*func)(BGGL3_State *ctx, elem args), int n)
{
	BGGL3_Binding *tmp;
	BGGL3_Builtin *btmp;

	btmp=BGGL3_Alloc("builtin", sizeof(BGGL3_Builtin));
	btmp->func=func;
	btmp->n_args=n;
	btmp->name=strdup(name);
	btmp->desc=strdup(desc);

	tmp=BGGL3_AllocBinding();
	tmp->var=BGGL3_HashSymbol(name);
	tmp->val=(elem)btmp;

	tmp->next=bggl3_inittop;
	bggl3_inittop=tmp;
}

elem BGGL3_LookupList(BGGL3_Binding *lst, elem expr)
{
	while(lst)
	{
		if(lst->var==expr)
			return(lst->val);
		lst=lst->next;
	}
	return(NULL);
}

elem BGGL3_AssignList(BGGL3_Binding *lst, elem var, elem val)
{
	while(lst)
	{
		if(lst->var==var)
		{
			lst->val=val;
			return(lst->val);
		}
		lst=lst->next;
	}
	return(NULL);
}

elem BGGL3_EvalList(BGGL3_State *ctx, elem cur)
{
	elem a, b, c;

	if(!cur)return(cur);

	if(BGGL3_CONSP(BGGL3_CAR(cur)) &&
		(BGGL3_CAR(BGGL3_CAR(cur))==BGGL3_HashSymbol("comma")))
	{
		a=BGGL3_EvalExpr(ctx, BGGL3_CADR(BGGL3_CAR(cur)));
		b=BGGL3_EvalList(ctx, BGGL3_CDR(cur));

		if(!a)return(b);
		c=a;
		while(BGGL3_CDR(c))c=BGGL3_CDR(c);
		BGGL3_CDRN(c, b);
		return(a);
	}

	a=BGGL3_EvalExpr(ctx, BGGL3_CAR(cur));
	b=BGGL3_EvalList(ctx, BGGL3_CDR(cur));
	c=BGGL3_CONS(a, b);
	return(c);
}

BGGL3_Binding *BGGL3_FillArgs(BGGL3_Binding *lst, elem vars)
{
	BGGL3_Binding *tmp;
	elem a, b, ca, cb;
	int i, j;

	a=vars;
	while(BGGL3_CONSP(a))
	{
		ca=BGGL3_CAR(a);
		if(BGGL3_CONSP(ca))
		{
			lst=BGGL3_FillArgs(lst, ca);
		}else if(BGGL3_SYMBOLP(ca))
		{
			tmp=BGGL3_AllocBinding();
			tmp->var=ca;
			tmp->val=NULL;

			tmp->next=lst;
			lst=tmp;
		}

		a=BGGL3_CDR(a);
	}

	if(a && !BGGL3_CONSP(a))
	{
		tmp=BGGL3_AllocBinding();
		tmp->var=a;
		tmp->val=NULL;
		tmp->next=lst;
		lst=tmp;
	}

	return(lst);
}

BGGL3_Binding *BGGL3_BindArgs(BGGL3_Binding *lst, elem vars, elem vals)
{
	BGGL3_Binding *tmp;
	elem a, b, ca, cb;
	int i, j;

	if(BGGL3_CONSP(vars) && BGGL3_FVECTORP(vals))
	{
		a=vars; i=0;
		while(BGGL3_CONSP(a))
		{
			ca=BGGL3_CAR(a);
			if(BGGL3_SYMBOLP(ca))
			{
				tmp=BGGL3_AllocBinding();
				tmp->var=ca;
				tmp->val=BGGL3_FLOAT(((float *)vals)[i]);

				tmp->next=lst;
				lst=tmp;
			}

			i++;
			a=BGGL3_CDR(a);
		}
		return(lst);
	}

	a=vars;
	b=vals;
	while(BGGL3_CONSP(a) && BGGL3_CONSP(b))
	{
		ca=BGGL3_CAR(a);
		cb=BGGL3_CAR(b);
		i=BGGL3_GetTypeID(ca);
		if(i==bggl3_id_cons)
		{
			lst=BGGL3_BindArgs(lst, ca, cb);
		}else if(BGGL3_SYMBOLP(ca))
		{
			tmp=BGGL3_AllocBinding();
			tmp->var=ca;
			tmp->val=cb;

			tmp->next=lst;
			lst=tmp;
		}

		a=BGGL3_CDR(a);
		b=BGGL3_CDR(b);
	}

	if(a && !BGGL3_CONSP(a))
	{
		tmp=BGGL3_AllocBinding();
		tmp->var=a;
		tmp->val=b;
		tmp->next=lst;
		lst=tmp;

		return(lst);
	}

	if(a || b)
	{
		printf("BGGL3_BindArgs: Args mismatch\n");
		exit(-1);
	}

	return(lst);
}

BGGL3_Binding *BGGL3_SetArgs(BGGL3_Binding *lst, elem vars, elem vals)
{
	elem a, b, ca, cb;
	int i, j;

	if(BGGL3_CONSP(vars) && BGGL3_FVECTORP(vals))
	{
		a=vars; i=0;
		while(BGGL3_CONSP(a))
		{
			ca=BGGL3_CAR(a);
			cb=BGGL3_FLOAT(((float *)vals)[i]);
			if(BGGL3_SYMBOLP(ca))
				BGGL3_AssignList(lst, ca, cb);

			i++;
			a=BGGL3_CDR(a);
		}
		return(lst);
	}

	a=vars;
	b=vals;
	while(BGGL3_CONSP(a) && BGGL3_CONSP(b))
	{
		ca=BGGL3_CAR(a);
		cb=BGGL3_CAR(b);
		i=BGGL3_GetTypeID(ca);
		if(BGGL3_CONSP(ca))
			BGGL3_SetArgs(lst, ca, cb);
		else if(BGGL3_SYMBOLP(ca))
			BGGL3_AssignList(lst, ca, cb);

		a=BGGL3_CDR(a);
		b=BGGL3_CDR(b);
	}

	if(a && !BGGL3_CONSP(a))
	{
		BGGL3_AssignList(lst, a, b);
		return(lst);
	}

	if(a || b)
	{
		printf("BGGL3_SetArgs: Args mismatch\n");
		exit(-1);
	}

	return(lst);
}

elem BGGL3_EvalExprApply(BGGL3_State *ctx, elem expr)
{
	BGGL3_Binding *tenv;
	BGGL3_Func *tf;
	BGGL3_Builtin *bf;

	elem a, b, c, d;
	int i;


	a=BGGL3_CAR(expr);
	if(BGGL3_SYMBOLP(a))
	{
		if(!strcmp(a, "quote"))
		{
			c=BGGL3_CADR(expr);
			return(c);
		}

		if(!strcmp(a, "if"))
		{
			tenv=ctx->env;
			a=BGGL3_EvalExpr(ctx, BGGL3_CADR(expr));
			ctx->env=tenv;

			if(BGGL3_TOINT(a))
				b=BGGL3_EvalExpr(ctx, BGGL3_CADDR(expr));
			else b=BGGL3_EvalExpr(ctx, BGGL3_CADDDR(expr));
			ctx->env=tenv;

			return(b);
		}

		if(!strcmp(a, "list"))
		{
			c=BGGL3_CDR(expr);
			c=BGGL3_EvalList(ctx, c);
			return(c);
		}

		if(!strcmp(a, "begin"))
		{
			tenv=ctx->env;

			a=NULL;
			c=BGGL3_CDR(expr);
			while(c)
			{
				a=BGGL3_EvalExpr(ctx, BGGL3_CAR(c));
				c=BGGL3_CDR(c);
			}

			ctx->env=tenv;
			return(a);
		}

		if(!strcmp(a, "fun"))
		{
			tf=BGGL3_Alloc("func", sizeof(BGGL3_Func));
			tf->env=ctx->env;
			tf->args=BGGL3_CADR(expr);
			tf->body=BGGL3_CADDR(expr);

			return((elem)tf);
		}

		if(!strcmp(a, "="))
		{
			c=BGGL3_CADR(expr);
			if(BGGL3_CONSP(c) && BGGL3_KEYWORDP(BGGL3_CAR(c)))
			{
				b=BGGL3_HashSymbol((char *)BGGL3_CAR(c));
				c=BGGL3_CDR(c);

				ctx->env=BGGL3_FillArgs(ctx->env, b);

				tf=BGGL3_Alloc("func", sizeof(BGGL3_Func));
				tf->env=ctx->env;
				tf->args=c;
				tf->body=BGGL3_CADDR(expr);

				BGGL3_SetArgs(ctx->env, b, (elem)tf);

				return((elem)tf);
			}

			ctx->env=BGGL3_FillArgs(ctx->env, BGGL3_CADR(expr));

			c=BGGL3_EvalExpr(ctx, BGGL3_CADDR(expr));
			BGGL3_SetArgs(ctx->env, BGGL3_CADR(expr), c);

//			ctx->env=BGGL3_BindArgs(ctx->env, BGGL3_CADR(expr), c);
			return(c);
		}


		b=BGGL3_LookupList(ctx->env, a);
		if(!b)b=BGGL3_LookupList(ctx->denv, a);

		if(!b)
		{
			printf("Unbound function '%s'\n", a);
			exit(-1);
		}
	}else
	{
		b=BGGL3_EvalExpr(ctx, a);
	}

	if(BGGL3_FVECTORP(b))
	{
		c=BGGL3_CDR(expr);
		c=BGGL3_EvalList(ctx, c);

		if(BGGL3_NUMBERP(BGGL3_CAR(c)))
		{
			i=BGGL3_TOINT(BGGL3_CAR(c));
			if(!BGGL3_CDR(c))
				return(BGGL3_FLOAT(((float *)b)[i]));
			((float *)b)[i]=BGGL3_TOFLOAT(BGGL3_CADR(c));
			return(NULL);
		}
		return(NULL);
	}

	if(BGGL3_CONSP(b))
	{
		c=BGGL3_CDR(expr);
		c=BGGL3_EvalList(ctx, c);

		if(BGGL3_NUMBERP(BGGL3_CAR(c)))
		{
			i=BGGL3_TOINT(BGGL3_CAR(c));

			d=b;
			while(d && i--)d=BGGL3_CDR(d);
			if(!d)return(NULL);

			if(!BGGL3_CDR(c))return(BGGL3_CAR(d));
			BGGL3_CARN(d, BGGL3_CADR(c));
			return(NULL);
		}
		return(NULL);
	}

	if(BGGL3_NUMBERP(b))
	{
		c=BGGL3_CDR(expr);
		c=BGGL3_EvalList(ctx, c);
		c=BGGL3_CONS(b, c);

		c=BGGL3_List2FVector(c);
		return(c);
	}

	if(!strcmp(BGGL3_GetType(b), "func"))
	{
		c=BGGL3_CDR(expr);
		c=BGGL3_EvalList(ctx, c);

		tf=b;
		tenv=ctx->env;
//		ctx->env=BGGL3_BindArgs(ctx->env, tf->args, c);
		ctx->env=BGGL3_BindArgs(tf->env, tf->args, c);
		a=BGGL3_EvalExpr(ctx, tf->body);

		ctx->env=tenv;
		return(a);
	}

	if(!strcmp(BGGL3_GetType(b), "builtin"))
	{
		c=BGGL3_CDR(expr);
		c=BGGL3_EvalList(ctx, c);

		bf=b;

		i=BGGL3_ListLength(c);
		if((bf->n_args>=0) && (i!=bf->n_args))
		{
			printf("Args count mismatch %s/%d <- %d\n",
				bf->name, bf->n_args, i);
			exit(-1);
		}
		if((bf->n_args<0) && (i<((-bf->n_args)-1)))
		{
			printf("Too few args %s/%d <- %d\n",
				bf->name, (-bf->n_args)-1, i);
			exit(-1);
		}

		a=bf->func(ctx, c);
		return(a);
	}

	return(NULL);
}

elem BGGL3_EvalExpr(BGGL3_State *ctx, elem expr)
{
	elem a, b, c;
	int i;

	if(!expr)return(expr);

	i=BGGL3_GetTypeID(expr);
	if(i==bggl3_id_int)
		return(expr);
	if(i==bggl3_id_float)
		return(expr);
	if(i==bggl3_id_string)
		return(expr);

	if(i==bggl3_id_symbol)
	{
		a=BGGL3_LookupList(ctx->env, expr);
		if(a)return(a);
		a=BGGL3_LookupList(ctx->denv, expr);
		return(a);
	}

	if(i==bggl3_id_cons)
	{
		a=BGGL3_EvalExprApply(ctx, expr);
		return(a);
	}

	return(expr);
}

elem BGGL3_ApplyArgs(BGGL3_State *ctx, elem b, elem c)
{
	BGGL3_Binding *tenv;
	BGGL3_Func *tf;
	BGGL3_Builtin *bf;
	elem a;
	int i;

	if(!strcmp(BGGL3_GetType(b), "func"))
	{
		tf=b;
		tenv=ctx->env;
		ctx->env=BGGL3_BindArgs(tf->env, tf->args, c);
		a=BGGL3_EvalExpr(ctx, tf->body);

		ctx->env=tenv;
		return(a);
	}

	if(!strcmp(BGGL3_GetType(b), "builtin"))
	{
		bf=b;

		i=BGGL3_ListLength(c);
		if((bf->n_args>=0) && (i!=bf->n_args))
		{
			printf("Args count mismatch %s/%d <- %d\n",
				bf->name, bf->n_args, i);
			exit(-1);
		}
		if((bf->n_args<0) && (i<((-bf->n_args)-1)))
		{
			printf("Too few args %s/%d <- %d\n",
				bf->name, (-bf->n_args)-1, i);
			exit(-1);
		}

		a=bf->func(ctx, c);
		return(a);
	}

	return(NULL);
}

elem BGGL3_ApplyArg(BGGL3_State *ctx, elem fcn, elem arg)
{
	return(BGGL3_ApplyArgs(ctx, fcn, BGGL3_CONS(arg, NULL)));
}
