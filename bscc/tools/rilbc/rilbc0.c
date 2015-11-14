#include "rilbc0.h"

void *rilbc_malloc(int sz)
{
	void *p;
	p=malloc(sz);
	memset(p, 0, sz);
	return(p);
}

void *rilbc_realloc(void *p, int sz)
{
	void *q;
	q=realloc(p, sz);
}

RILBC_Context *RILBC_NewContext()
{
	RILBC_Context *tmp;

	tmp=rilbc_malloc(sizeof(RILBC_Context));

	tmp->strings=ril_malloc(256);
	tmp->n_strings=2; tmp->m_strings=256;

	tmp->stridx=ril_malloc(256*sizeof(int));
	tmp->n_stridx=2; tmp->m_stridx=256;

	tmp->lists=ril_malloc(256*sizeof(int));
	tmp->n_lists=2; tmp->m_lists=256;

	tmp->tables=ril_malloc(256*sizeof(RILBC_Table *));
	tmp->n_tables=1; tmp->m_tables=256;

	tmp->opcodes=ril_malloc(256*sizeof(RILBC_Opcode *));
	tmp->n_opcodes=1; tmp->m_opcodes=256;

	RILBC_DeclareTable(tmp, "Strings", NULL);
	RILBC_DeclareTable(tmp, "StringIndex", NULL);
	RILBC_DeclareTable(tmp, "Lists", NULL);
	RILBC_DeclareTable(tmp, "Tables", NULL);
	RILBC_DeclareTable(tmp, "Opcodes", NULL);

	return(tmp);
}

int RILBC_IndexString(RILBC_Context *ctx, char *str)
{
	char *s;
	int i, j;

	if(!str)return(0);
	if(!(*str))return(1);

	s=ctx->strings+2;
	while(*s)
	{
		if(!strcmp(s, str))
			return(s-ctx->strings);
		s+=strlen(s)+1;
	}
	
	i=ctx->n_strings+(strlen(str)+3);
	if(i>=ctx->m_strings)
	{
		j=ctx->m_strings;
		while(i>=j) j=j+(j>>1);
		ctx->strings=rilbc_realloc(ctx->strings, j);
		ctx->m_strings=j;
	}

	i=ctx->n_strings;
	s=ctx->strings+i;
	strcpy(s, str);
	s+=strlen(s)+1;
	*s=0;
	ctx->n_string=s-ctx->strings;
	
	return(i);
}

int rilbc_lstcmp(int *lsta, int *lstb)
{
	while(*lsta && *lstb && (*lsta==*lstb))
		{ lsta++; lstb++; }
	if(*lsta)return(1);
	if(*lstb)return(-1);
	return(0);
}

int RILBC_IndexStrIdx(RILBC_Context *ctx, char **str)
{
	int ta[64];
	int *si;
	int i, n;

	if(!str)return(0);
	if(!(str[0]))return(1);
	
	for(i=0; str[i]; i++)
		ta[i]=RILBC_IndexString(ctx, str[i]);
	ta[i]=0; n=i;

	si=ctx->stridx+2;
	while(*s)
	{
		if(!rilbc_lstcmp(si, ta))
			return(si-ctx->stridx);
		si+=n+1;
	}

	i=ctx->n_stridx+(n+2);
	if(i>=ctx->m_stridx)
	{
		j=ctx->m_stridx;
		while(i>=j) j=j+(j>>1);
		ctx->stridx=rilbc_realloc(ctx->stridx, j*sizeof(int));
		ctx->m_stridx=j;
	}

	i=ctx->n_stridx;
	si=ctx->stridx+i;
	strcpy(s, str);
	si+=n+1;
	*si=0;
	ctx->n_stridx=si-ctx->stridx;
	
	return(i);
}

int RILBC_IndexList(RILBC_Context *ctx, int **lst)
{
	int *si;
	int i, n;

	if(!lst)return(0);
	if(!(lst[0]))return(1);

	for(i=0; lst[i]; i++);
	n=i;

	si=ctx->lists+2;
	while(*s)
	{
		if(!rilbc_lstcmp(si, ta))
			return(si-ctx->lists);
		si+=n+1;
	}

	i=ctx->n_lists+(n+2);
	if(i>=ctx->m_lists)
	{
		j=ctx->m_stridx;
		while(i>=j) j=j+(j>>1);
		ctx->stridx=rilbc_realloc(ctx->stridx, j*sizeof(int));
		ctx->m_stridx=j;
	}

	i=ctx->n_stridx;
	si=ctx->stridx+i;
	strcpy(s, str);
	si+=n+1;
	*si=0;
	ctx->n_stridx=si-ctx->stridx;
	
	return(i);
}

int RILBC_IndexTable(RILBC_Context *ctx, char *name)
{
	int i, j, ni;
	
	ni=RILBC_IndexString(ctx, name);
	
	for(i=1; i<ctx->n_tables; i++)
	{
		if(ctx->tables[i]->name==ni)
			return(i);
	}
	
	if((ctx->n_tables+1)>=ctx->m_tables)
	{
		i=ctx->m_tables+(ctx->m_tables>>1);
		ctx->tables=rilbc_realloc(ctx->tables, i*sizeof(RILBC_Table *));
		ctx->m_tables=i;
	}

	i=ctx->n_tables++;
	ctx->tables[i]=rilbc_malloc(sizeof(RILBC_Table));
	ctx->tables[i]->name=ni;
	return(i);
}

int RILBC_DeclareTable(RILBC_Context *ctx, char *name, char **fields)
{
	int i, j;
	
	i=RILBC_IndexTable(ctx, name);
	j=RILBC_IndexStrIdx(ctx, fields);
	ctx->tables[i]->fields=j;
	return(i);
}

int RILBC_IndexOpcode(RILBC_Context *ctx, char *name)
{
	int i, j, ni;
	
	ni=RILBC_IndexString(ctx, name);
	
	for(i=1; i<ctx->n_opcodes; i++)
	{
		if(ctx->opcodes[i]->name==ni)
			return(i);
	}
	
	if((ctx->n_opcodes+1)>=ctx->m_opcodes)
	{
		i=ctx->m_opcodes+(ctx->m_opcodes>>1);
		ctx->opcodes=rilbc_realloc(ctx->opcodes, i*sizeof(RILBC_Opcode *));
		ctx->m_opcodes=i;
	}

	i=ctx->n_opcodes++;
	ctx->opcodes[i]=rilbc_malloc(sizeof(RILBC_Opcode));
	ctx->opcodes[i]->name=ni;
	return(i);
}
