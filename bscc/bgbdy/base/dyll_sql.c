#include <bgbdy.h>
#include <sqlite3.h>

int dyll_sql_errno=0;
char *dyll_sql_errstr=NULL;

BGBDYC_RDB *dyll_sql_dbroot=NULL;


BGBDY_API int dyllSqlErrno()
	{ return(dyll_sql_errno); }
BGBDY_API char *dyllSqlErrStr()
	{ return(dyll_sql_errstr); }

BGBDYC_RDB *DYLL_SQL_LookupDB(char *name)
{
	BGBDYC_RDB *cur;

	cur=dyll_sql_dbroot;
	while(cur)
	{
		if(!strcmp(cur->name, name))
			return(cur);
		cur=cur->next;
	}

	return(NULL);
}

void DYLL_SQL_UnlinkDB(BGBDYC_RDB *db)
{
	BGBDYC_RDB *cur, *prv;

	cur=dyll_sql_dbroot; prv=NULL;
	while(cur && (cur!=db))
		{ prv=cur; cur=cur->next; }

	if(cur)
	{
		if(prv) { prv->next=cur->next; }
		else { dyll_sql_dbroot=cur->next; }
	}
}

BGBDY_API BGBDYC_RDB *dyllSqlOpenDB(char *name)
{
	BGBDYC_RDB *tmp;
	sqlite3 *db;
	int rc;

	if(name)
	{
		tmp=DYLL_SQL_LookupDB(name);
		if(tmp)
		{
			tmp->cnt++;
			return(tmp);
		}
	}

	if(!name)name=":memory:";

	rc=sqlite3_open(name, &db);
	if(rc)
	{
		sqlite3_close(db);
		return(NULL);
	}

	tmp=gctalloc("_dyll_sqldb_t", sizeof(BGBDYC_RDB));
	tmp->name=dystrdup(name);
	tmp->db=db;
	tmp->cnt=1;

	tmp->next=dyll_sql_dbroot;
	dyll_sql_dbroot=tmp;

	return(tmp);
}

BGBDY_API BGBDYC_RDB *dyllSqlOpenTempDB(char *name)
{
	BGBDYC_RDB *tmp;

	if(!name)name="";
	tmp=DYLL_SQL_LookupDB(name);
	if(tmp)
	{
		tmp->cnt++;
		return(tmp);
	}

	tmp=dyllSqlOpenDB(NULL);
	tmp->name=dystrdup(name);
	return(tmp);
}

BGBDY_API void dyllSqlCloseDB(BGBDYC_RDB *db)
{
	if(!db)return;

	db->cnt--;
	if(db->cnt>0)return;

	DYLL_SQL_UnlinkDB(db);
	sqlite3_close((sqlite3 *)(db->db));
	gcfree(db);
}

BGBDY_API int dyllSqlExec(BGBDYC_RDB *db, char *cmd)
{
	char *str;
	int rc;

	dyll_sql_errno=0;
	dyll_sql_errstr=NULL;
	rc=sqlite3_exec((sqlite3 *)(db->db),
		cmd, NULL, NULL, &str);
	if(rc!=SQLITE_OK)
	{
		dyll_sql_errno=rc;
		dyll_sql_errstr=dystrdup(str);
		sqlite3_free(str);
		return(-1);
	}
	return(0);
}

BGBDY_API int dyllSqlExecQuery(BGBDYC_RDB *db, char *cmd,
	int (*fcn)(void *data, int argc, char **argv, char **colName),
	void *data)
{
	char *str;
	int rc;

	dyll_sql_errno=0;
	dyll_sql_errstr=NULL;
	rc=sqlite3_exec((sqlite3 *)(db->db),
		cmd, fcn, data, &str);
	if(rc!=SQLITE_OK)
	{
		dyll_sql_errno=rc;
		dyll_sql_errstr=dystrdup(str);
		sqlite3_free(str);
		return(-1);
	}
	return(0);
}

BGBDY_API int dyllSqlExecQueryTab(BGBDYC_RDB *db, char *cmd,
	char ***rtab, int *rncol, int *rnrow,
	void *data)
{
	char *str;
	int rc;

	dyll_sql_errno=0;
	dyll_sql_errstr=NULL;
	rc=sqlite3_get_table((sqlite3 *)(db->db),
		cmd, rtab, rnrow, rncol, &str);
	if(rc!=SQLITE_OK)
	{
		dyll_sql_errno=rc;
		dyll_sql_errstr=dystrdup(str);
		sqlite3_free(str);
		return(-1);
	}
	return(0);
}

BGBDY_API int dyllSqlExecFreeTab(BGBDYC_RDB *db, char **tab)
{
	sqlite3_free_table(tab);
	return(0);
}

BGBDY_API BGBDYC_RDBTable *dyllSqlExecQueryTable(BGBDYC_RDB *db, char *cmd)
{
	BGBDYC_RDBTable *tmp;
	char **tab;
	char *str;
	int rc, nrow, ncol;

	dyll_sql_errno=0;
	dyll_sql_errstr=NULL;
	rc=sqlite3_get_table((sqlite3 *)(db->db),
		cmd, &tab, &nrow, &ncol, &str);

	if(rc!=SQLITE_OK)
	{
		dyll_sql_errno=rc;
		dyll_sql_errstr=dystrdup(str);
		sqlite3_free(str);
		return(NULL);
	}

	
	tmp=gctalloc("_dyll_sqltable_t", sizeof(BGBDYC_RDBTable));
	tmp->db=db;
	tmp->cols=tab;
	tmp->rows=tab+ncol;
	tmp->ncol=ncol;
	tmp->nrow=nrow;

	return(tmp);
}

BGBDY_API int dyllSqlExecFreeTable(BGBDYC_RDBTable *tab)
{
	if(!tab)return(0);
	sqlite3_free_table(tab->cols);
	gcfree(tab);
	return(0);
}


BGBDY_API int dyllSqlExecPrint(BGBDYC_RDB *db, char *cmd)
{
	char *str;
	int rc;

	rc=sqlite3_exec((sqlite3 *)(db->db),
		cmd, NULL, NULL, &str);
	if(rc!=SQLITE_OK)
	{
		printf("SQL Error(%d): %s\n", rc, str);
		sqlite3_free(str);
		return(-1);
	}
	return(0);
}

BGBDY_API int dyllSqlPrintTab(char **cols, char **rows, int ncol, int nrow)
{
	int szCol[64];
	char *str;
	int i, j, k, l;

	for(i=0; i<ncol; i++)
		szCol[i]=strlen(cols[i])+1;

	for(i=0; i<nrow; i++)
	{
		for(j=0; j<ncol; j++)
		{
			k=strlen(rows[i*ncol+j])+1;
			if(k>szCol[j])szCol[j]=k;
		}
	}

	for(i=0; i<ncol; i++)
	{
		printf("%s", cols[i]);
		k=szCol[i]-strlen(cols[i]);
		for(j=0; j<k; j++)
			printf(" ");
		printf("| ");
	}
	printf("\n");

	for(i=0; i<ncol; i++)
	{
		for(j=0; j<szCol[i]; j++)
			printf("-");
		printf("+-");
	}
	printf("\n");

	for(i=0; i<nrow; i++)
	{
		for(j=0; j<ncol; j++)
		{
			str=rows[i*ncol+j];
			printf("%s", str);
			l=szCol[j]-strlen(str);
			for(k=0; k<l; k++)
				printf(" ");
			printf("| ");
		}
		printf("\n");
	}
	printf("\n");

	return(0);
}

BGBDY_API int dyllSqlPrintTable(BGBDYC_RDBTable *tab)
{
	dyllSqlPrintTab(tab->cols, tab->rows, tab->ncol, tab->nrow);
	return(0);
}

BGBDY_API int dyllSqlExecPrintTable(BGBDYC_RDB *db, char *cmd)
{
	char **tab;
	char *str;
	int rc, nrow, ncol;

	dyll_sql_errno=0;
	dyll_sql_errstr=NULL;
	rc=sqlite3_get_table((sqlite3 *)(db->db),
		cmd, &tab, &nrow, &ncol, &str);

	if(rc!=SQLITE_OK)
	{
		printf("SQL Error(%d): %s\n", rc, str);
		sqlite3_free(str);
		return(-1);
	}

	dyllSqlPrintTab(tab, tab+ncol, ncol, nrow);

	sqlite3_free_table(tab);

	return(0);
}


void DYLL_MetaRDB_TryCacheKey(char *key)
{
}

char *DYLL_MetaRDB_LookupCacheKey(char *key)
{
	char tb[256];
	char *s;

	sprintf(tb, ".metardb/cache/%s", key);
	s=DYLL_MetaPath_LookupKey2(tb);
	if(s)return(s);

	DYLL_MetaRDB_TryCacheKey(key);

	s=DYLL_MetaPath_LookupKey2(tb);
	if(s)return(s);

	return(NULL);
}
