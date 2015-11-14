#include <bgbgc.h>
#include <bgbdy.h>

BGBDYC_RciTable *BGBDYC_RciAllocTable(char **cols)
{
	BGBDYC_RciTable *tmp;
	int i;

	tmp=gctalloc("_bgbdyc_rcit_t", sizeof(BGBDYC_RciTable));

	for(i=0; cols[i]; i++);
	tmp->ncol=i;

	tmp->col=gcalloc((tmp->ncol+1)*sizeof(char *));
	for(i=0; i<tmp->ncol; i++)
		tmp->col[i]=dysymbol(cols[i]);
	tmp->col[tmp->ncol]=NULL;

	return(tmp);
}

BGBDYC_RciDB *BGBDYC_RciAllocDB()
{
	BGBDYC_RciDB *tmp;
	int i;

	tmp=gctalloc("_bgbdyc_rcidb_t", sizeof(BGBDYC_RciDB));
	return(tmp);
}

BGBDYC_RciTable *BGBDYC_RciLookupTable(BGBDYC_RciDB *db, char *name)
{
	BGBDYC_RciTable *cur;

	cur=db->table;
	while(cur)
	{
		if(!strcmp(cur->name, name))
			return(cur);
		cur=cur->next;
	}

	return(NULL);
}

BGBDYC_RciTable *BGBDYC_RciCreateTable(BGBDYC_RciDB *db,
	char *name, char **cols)
{
	BGBDYC_RciTable *tab;
	int i;

	tab=BGBDYC_RciLookupTable(db, name);
	if(tab)
	{
		for(i=0; i<tab->ncol; i++)
		{
			if(!cols[i])return(NULL);
			if(strcmp(tab->col[i], cols[i]))
				return(NULL);
		}
		if(cols[i])return(NULL);

		return(tab);
	}

	tab=BGBDYC_RciAllocTable(cols);
	tab->name=dysymbol(name);
	tab->next=db->table;
	db->table=tab;

	return(tab);
}

BGBDYC_RciTable *BGBDYC_RciCreateTable1(BGBDYC_RciDB *db,
	char *name, char *c0)
{
	char *a[4];
	a[0]=c0; a[1]=NULL;
	return(BGBDYC_RciCreateTable(db, name, a));
}

BGBDYC_RciTable *BGBDYC_RciCreateTable2(BGBDYC_RciDB *db,
	char *name, char *c0, char *c1)
{
	char *a[4];
	a[0]=c0; a[1]=c1; a[2]=NULL;
	return(BGBDYC_RciCreateTable(db, name, a));
}

BGBDYC_RciTable *BGBDYC_RciCreateTable3(BGBDYC_RciDB *db,
	char *name, char *c0, char *c1, char *c2)
{
	char *a[4];
	a[0]=c0; a[1]=c1; a[2]=c2; a[3]=NULL;
	return(BGBDYC_RciCreateTable(db, name, a));
}

BGBDYC_RciTable *BGBDYC_RciCreateTable4(BGBDYC_RciDB *db,
	char *name, char *c0, char *c1, char *c2, char *c3)
{
	char *a[4];
	a[0]=c0; a[1]=c1; a[2]=c2; a[3]=c3; a[4]=NULL;
	return(BGBDYC_RciCreateTable(db, name, a));
}

dyt *BGBDYC_RciGetRow(BGBDYC_RciTable *tab, int idx)
{
	int i;

	if(idx<0)return(NULL);

	if(!tab->row)
	{
		i=16;
		while(i<=idx)i=i+(i>>1);

		tab->row=gcalloc(i*tab->ncol*sizeof(dyt));
		tab->map=gcalloc(i*tab->ncol);
		tab->mrow=i;	tab->nrow=idx;

		tab->map[idx]|=1;
		return(tab->row+(i*tab->ncol));
	}

	if(idx>=tab->mrow)
	{
		i=tab->mrow;
		while(i<=idx)i=i+(i>>1);
		tab->row=gcrealloc(tab->row, i*tab->ncol*sizeof(dyt));
		tab->map=gcrealloc(tab->map, i*tab->ncol);
		tab->mrow=i;	tab->nrow=idx;

		tab->map[idx]|=1;
		return(tab->row+(i*tab->ncol));
	}

	tab->map[idx]|=1;
	return(tab->row+(i*tab->ncol));
}

int BGBDYC_RciReadRow(BGBDYC_RciTable *tab, int idx, dyt *val)
{
	dyt *rp;
	s64 li, lj;
	int i, j;

	if((idx<0) || (idx>=tab->nrow))
	{
		val[0]=NULL;
		return(-1);
	}

	if(!tab->row || !tab->map[idx])
	{
		val[0]=NULL;
		return(-1);
	}

	rp=BGBDYC_RciGetRow(tab, idx);
	for(i=0; i<tab->ncol; i++)
		val[i]=rp[i];
	return(tab->ncol);
}

int BGBDYC_RciWriteRow(BGBDYC_RciTable *tab, int idx, dyt *val)
{
	dyt *rp;
	int i;

	if(idx<0)
	{
		for(i=0; i<tab->nrow; i++)
			if(!tab->map[i])break;
		idx=i;
	}

	rp=BGBDYC_RciGetRow(tab, idx);
	for(i=0; i<tab->ncol; i++)
		rp[i]=val[i];
	return(idx);
}

int BGBDYC_RciDecodeRow(BGBDYC_RciTable *tab, char *str)
{
	char **a;
	dyt *rp;
	char *s, *s1;
	dyt p;
	double d;
	s64 li;
	int i, j;
	int idx;

	s=str;
	while(*s && (*s<=' '))s++;
	if(!*s)return(-1);
	if(*s==';')return(-1);

	a=gcrsplit(str);
	if(!a[0])return(-1);

	idx=atoi(a[0]);

	rp=BGBDYC_RciGetRow(tab, idx);

	for(i=0; i<tab->ncol; i++)
	{
		s=a[i+1];
		if(!s)break;

		if(s[strlen(s)+1]=='\"')
		{
			p=dystring(s);
		}else if(!strcmp(s, "-"))
			{ p=NULL; }
		else if(!strcmp(s, "-T"))
			{ p=BGBDY_TRUE; }
		else if(!strcmp(s, "-F"))
			{ p=BGBDY_FALSE; }
		else if((*s>='0') && (*s<='9'))
		{
			s1=s;
			while(*s1 && (*s1!='.') && (*s1!='e'))s1++;

			if(*s1)
			{
				d=atof(s);
				p=dydouble(d);
			}else
			{
				li=atoll(s);
				p=dyint(li);
			}
		}else
		{
			p=dysymbol(s);
		}

		rp[i]=p;
	}

	return(i);
}

int BGBDYC_RciEncodeRow(BGBDYC_RciTable *tab, int idx, char *buf)
{
	char **a;
	char *t;
	dyt *rp;
	dyt p;
	int i, j;

	if((idx<0) || (idx>=tab->nrow))return(-1);
	if(!tab->row || !tab->map[idx])return(-1);

	t=buf;
	sprintf(t, "%d", idx);
	t+=strlen(t);

	rp=tab->row+(idx*tab->ncol); i=0;
	for(i=0; i<tab->ncol; i++)
	{
		p=rp[i];

		if(!p)
		{
			sprintf(t, " -");
			t+=strlen(t);
			continue;
		}

		if(p==BGBDY_TRUE)
		{
			sprintf(t, " -T");
			t+=strlen(t);
			continue;
		}

		if(p==BGBDY_FALSE)
		{
			sprintf(t, " -F");
			t+=strlen(t);
			continue;
		}

		if(dyintp(p))
		{
			j=dyintv(p);
			sprintf(t, " %d", j);
			t+=strlen(t);
			continue;
		}

		if(dysymbolp(p))
		{
			j=dyintv(p);
			sprintf(t, " %s", dysymbolv(p));
			t+=strlen(t);
			continue;
		}
		if(dystringp(p))
		{
			j=dyintv(p);
			sprintf(t, " \"%s\"", dystringv(p));
			t+=strlen(t);
			continue;
		}
	}

	*t++='\n';
	*t++=0;

	return(0);
}

BGBDYC_RciDB *BGBDYC_RciLoadDB(char *name)
{
	char tb[512];
	char **a;
	char *s;
	BGBDYC_RciDB *tmp;
	BGBDYC_RciTable *tab;
	VFILE *fd;

	tmp=BGBDYC_RciAllocDB();
	tab=NULL;

	fd=vffopen(name, "rt");
	while(!vfeof(fd))
	{
		vfgets(tb, 511, fd);

		s=tb;
		while(*s && (*s<=' '))s++;
		if(!*s)continue;
		if(*s==';')continue;

		a=gcrsplit(tb);
		if(!a[0])continue;

		if(!strcmp(a[0], "TABLE"))
			tab=BGBDYC_RciCreateTable(tmp, a[1], a+2);
		if(!tab)continue;

		BGBDYC_RciDecodeRow(tab, tb);
	}
	vfclose(fd);

	return(tmp);
}

int BGBDYC_RciSaveDB(char *name, BGBDYC_RciDB *db)
{
	char tb[512];
	BGBDYC_RciTable *cur;
	VFILE *fd;
	int i;

	fd=vffopen(name, "wt");

	cur=db->table;

	while(cur)
	{
		vfprint(fd, "TABLE %s", cur->name);
		for(i=0; i<cur->ncol; i++)
			vfprint(fd, " %s", cur->col[i]);
		vfprint(fd, "\n");

		for(i=1; i<cur->nrow; i++)
			if(cur->map[i])
		{
			BGBDYC_RciEncodeRow(cur, i, tb);
			vfputs(tb, fd);
		}

		cur=cur->next;
	}
	vfclose(fd);
}

int BGBDYC_RciIndex_NameType(BGBDYC_RciDB *db, char *name, char *sig)
{
	dyt ap[8];
	BGBDYC_RciTable *tab;
	dyt p0, p1;
	int i, j;

	tab=BGBDYC_RciLookupTable(db, "NameType");
	if(!tab)tab=BGBDYC_RciCreateTable2(db, "NameType", "Name", "Type");

	p0=dysymbol(name);
	p1=dysymbol(sig);

	for(i=0; i<tab->nrow; i++)
	{
		j=BGBDYC_RciReadRow(tab, i, ap);
		if(j<0)continue;
		if((ap[0]==p0) && (ap[1]==p1))
			return(i);
	}

	ap[0]=p0; ap[1]=p1; ap[2]=NULL; ap[3]=NULL;
	i=BGBDYC_RciWriteRow(tab, -1, ap);
	return(i);
}

int BGBDYC_RciIndex_FieldRef(BGBDYC_RciDB *db,
	char *cname, char *name, char *sig)
{
	dyt ap[8];
	BGBDYC_RciTable *tab;
	dyt p0, p1;
	int i, j, nt_i;

	nt_i=BGBDYC_RciIndex_NameType(db, name, sig);

	tab=BGBDYC_RciLookupTable(db, "FieldRef");
	if(!tab)tab=BGBDYC_RciCreateTable2(db, "FieldRef",
		"ClassName", "NameTypeIdx");

	p0=dysymbol(cname);
	p1=dyint(nt_i);

	for(i=0; i<tab->nrow; i++)
	{
		j=BGBDYC_RciReadRow(tab, i, ap);
		if(j<0)continue;
		if((ap[0]==p0) && (ap[1]==p1))
			return(i);
	}

	ap[0]=p0; ap[1]=p1; ap[2]=NULL; ap[3]=NULL;
	i=BGBDYC_RciWriteRow(tab, -1, ap);
	return(i);
}

int BGBDYC_RciIndex_MethodRef(BGBDYC_RciDB *db,
	char *cname, char *name, char *sig)
{
	dyt ap[8];
	BGBDYC_RciTable *tab;
	dyt p0, p1;
	int i, j, nt_i;

	nt_i=BGBDYC_RciIndex_NameType(db, name, sig);

	tab=BGBDYC_RciLookupTable(db, "MethodRef");
	if(!tab)tab=BGBDYC_RciCreateTable2(db, "MethodRef",
		"ClassName", "NameTypeIdx");

	p0=dysymbol(cname);
	p1=dyint(nt_i);

	for(i=0; i<tab->nrow; i++)
	{
		j=BGBDYC_RciReadRow(tab, i, ap);
		if(j<0)continue;
		if((ap[0]==p0) && (ap[1]==p1))
			return(i);
	}

	ap[0]=p0; ap[1]=p1; ap[2]=NULL; ap[3]=NULL;
	i=BGBDYC_RciWriteRow(tab, -1, ap);
	return(i);
}

int BGBDYC_RciIndex_Class(BGBDYC_RciDB *db, char *cname)
{
	dyt ap[8];
	BGBDYC_RciTable *tab;
	dyt p0, p1;
	int i, j;

	tab=BGBDYC_RciLookupTable(db, "Class");
	if(!tab)tab=BGBDYC_RciCreateTable3(db, "Class",
		"Name", "Super", "Flags");

	p0=dysymbol(cname);
//	p1=dysymbol(sig);

	for(i=0; i<tab->nrow; i++)
	{
		j=BGBDYC_RciReadRow(tab, i, ap);
		if(j<0)continue;
//		if((ap[0]==p0) && (ap[1]==p1))
//			return(i);

		if(ap[0]==p0)
			return(i);
	}

	ap[0]=p0; ap[1]=NULL; ap[2]=NULL; ap[3]=NULL;
	i=BGBDYC_RciWriteRow(tab, -1, ap);
	return(i);
}

int BGBDYC_RciIndex_Literal(BGBDYC_RciDB *db, dyt val)
{
	dyt ap[8];
	BGBDYC_RciTable *tab;
	int i, j;

	tab=BGBDYC_RciLookupTable(db, "Literal");
	if(!tab)tab=BGBDYC_RciCreateTable1(db, "Literal", "Value");

	for(i=0; i<tab->nrow; i++)
	{
		j=BGBDYC_RciReadRow(tab, i, ap);
		if(j<0)continue;
		if(ap[0]==val)
			return(i);
	}

	ap[0]=val; ap[1]=NULL; ap[2]=NULL; ap[3]=NULL;
	i=BGBDYC_RciWriteRow(tab, -1, ap);
	return(i);
}

int BGBDYC_RciIndex_ClassField(BGBDYC_RciDB *db,
	char *cname, char *name, char *sig)
{
	dyt ap[8];
	BGBDYC_RciTable *tab;
	dyt p0, p1;
	int i, j, c_i, nt_i;

	tab=BGBDYC_RciLookupTable(db, "ClassField");
	if(!tab)tab=BGBDYC_RciCreateTable4(db, "ClassField",
		"ClassIdx", "NameTypeIdx", "Flags", "LitIdx");

	c_i=BGBDYC_RciIndex_Class(db, cname);
	nt_i=BGBDYC_RciIndex_NameType(db, name, sig);

	p0=dyint(c_i);
	p1=dyint(nt_i);

	for(i=0; i<tab->nrow; i++)
	{
		j=BGBDYC_RciReadRow(tab, i, ap);
		if(j<0)continue;
		if((ap[0]==p0) && (ap[1]==p1))
			return(i);
	}

	ap[0]=p0; ap[1]=p1; ap[2]=NULL; ap[3]=NULL;
	i=BGBDYC_RciWriteRow(tab, -1, ap);
	return(i);
}

int BGBDYC_RciIndex_ClassMethod(BGBDYC_RciDB *db,
	char *cname, char *name, char *sig)
{
	dyt ap[8];
	BGBDYC_RciTable *tab;
	dyt p0, p1;
	int i, j, c_i, nt_i;

	tab=BGBDYC_RciLookupTable(db, "ClassMethod");
	if(!tab)tab=BGBDYC_RciCreateTable3(db, "ClassMethod",
		"ClassIdx", "NameTypeIdx", "Flags");

	c_i=BGBDYC_RciIndex_Class(db, cname);
	nt_i=BGBDYC_RciIndex_NameType(db, name, sig);

	p0=dyint(c_i);
	p1=dyint(nt_i);

	for(i=0; i<tab->nrow; i++)
	{
		j=BGBDYC_RciReadRow(tab, i, ap);
		if(j<0)continue;
		if((ap[0]==p0) && (ap[1]==p1))
			return(i);
	}

	ap[0]=p0; ap[1]=p1; ap[2]=NULL; ap[3]=NULL;
	i=BGBDYC_RciWriteRow(tab, -1, ap);
	return(i);
}
