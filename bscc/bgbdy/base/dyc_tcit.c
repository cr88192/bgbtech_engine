#include <bgbgc.h>
#include <bgbdy.h>

byte *BGBDY_EncVLI(byte *ip, long long i)
{
	if(i>=0x100000000000000LL)
	{
		*ip++=0xFF;
		*ip++=(i>>56)&0xFF;
		*ip++=(i>>48)&0xFF;
		*ip++=(i>>40)&0xFF;
		*ip++=(i>>32)&0xFF;
		*ip++=(i>>24)&0xFF;
		*ip++=(i>>16)&0xFF;
		*ip++=(i>>8)&0xFF;
		*ip++=i&255;
		return(ip);
	}

	if(i>=0x2000000000000LL)
	{
		*ip++=(i>>56)|0xFE;
		*ip++=(i>>48)&0xFF;
		*ip++=(i>>40)&0xFF;
		*ip++=(i>>32)&0xFF;
		*ip++=(i>>24)&0xFF;
		*ip++=(i>>16)&0xFF;
		*ip++=(i>>8)&0xFF;
		*ip++=i&255;
		return(ip);
	}

	if(i>=0x40000000000LL)
	{
		*ip++=(i>>48)|0xFC;
		*ip++=(i>>40)&0xFF;
		*ip++=(i>>32)&0xFF;
		*ip++=(i>>24)&0xFF;
		*ip++=(i>>16)&0xFF;
		*ip++=(i>>8)&0xFF;
		*ip++=i&255;
		return(ip);
	}

	if(i>=0x800000000LL)
	{
		*ip++=(i>>40)|0xF8;
		*ip++=(i>>32)&0xFF;
		*ip++=(i>>24)&0xFF;
		*ip++=(i>>16)&0xFF;
		*ip++=(i>>8)&0xFF;
		*ip++=i&255;
		return(ip);
	}

	if(i>=0x10000000)
	{
		*ip++=(i>>32)|0xF0;
		*ip++=(i>>24)&0xFF;
		*ip++=(i>>16)&0xFF;
		*ip++=(i>>8)&0xFF;
		*ip++=i&255;
		return(ip);
	}

	if(i>=0x200000)
	{
		*ip++=(i>>24)|0xE0;
		*ip++=(i>>16)&0xFF;
		*ip++=(i>>8)&0xFF;
		*ip++=i&255;
		return(ip);
	}

	if(i>=0x4000)
	{
		*ip++=(i>>16)|0xC0;
		*ip++=(i>>8)&0xFF;
		*ip++=i&255;
		return(ip);
	}

	if(i>=0x80)
	{
		*ip++=(i>>8)|0x80;
		*ip++=i&255;
		return(ip);
	}

	*ip++=i;
	return(ip);
}

byte *BGBDY_EncSVLI(byte *ip, long long i)
{
	i=(i>=0)?(i*2):(((-i)*2)-1);
	ip=BGBDY_EncVLI(ip, i);
	return(ip);
}

s64 BGBDY_DecVLI(byte **rip)
{
	byte *ip;
	s64 i;

	ip=*rip;
	if(*ip<0x80)
	{
		i=*ip++;
		*rip=ip; return(i);
	}

	if(*ip<0xC0)
	{
		i=(*ip++)&0x3F;
		i=(i<<8)|(*ip++);
		*rip=ip; return(i);
	}

	if(*ip<0xE0)
	{
		i=(*ip++)&0x1F;
		i=(i<<8)|(*ip++);
		i=(i<<8)|(*ip++);
		*rip=ip; return(i);
	}

	if(*ip<0xF0)
	{
		i=(*ip++)&0x0F;
		i=(i<<8)|(*ip++);
		i=(i<<8)|(*ip++);
		i=(i<<8)|(*ip++);
		*rip=ip; return(i);
	}

	if(*ip<0xF8)
	{
		i=(*ip++)&0x07;
		i=(i<<8)|(*ip++);
		i=(i<<8)|(*ip++);
		i=(i<<8)|(*ip++);
		i=(i<<8)|(*ip++);
		*rip=ip; return(i);
	}

	if(*ip<0xFC)
	{
		i=(*ip++)&0x03;
		i=(i<<8)|(*ip++);
		i=(i<<8)|(*ip++);
		i=(i<<8)|(*ip++);
		i=(i<<8)|(*ip++);
		i=(i<<8)|(*ip++);
		*rip=ip; return(i);
	}

	if(*ip<0xFE)
	{
		i=(*ip++)&0x01;
		i=(i<<8)|(*ip++);
		i=(i<<8)|(*ip++);
		i=(i<<8)|(*ip++);
		i=(i<<8)|(*ip++);
		i=(i<<8)|(*ip++);
		i=(i<<8)|(*ip++);
		*rip=ip; return(i);
	}

	if(*ip<0xFF)
	{
		i=0; ip++;
		i=(i<<8)|(*ip++);
		i=(i<<8)|(*ip++);
		i=(i<<8)|(*ip++);
		i=(i<<8)|(*ip++);
		i=(i<<8)|(*ip++);
		i=(i<<8)|(*ip++);
		i=(i<<8)|(*ip++);
		*rip=ip; return(i);
	}

	i=0; ip++;
	i=(i<<8)|(*ip++);
	i=(i<<8)|(*ip++);
	i=(i<<8)|(*ip++);
	i=(i<<8)|(*ip++);
	i=(i<<8)|(*ip++);
	i=(i<<8)|(*ip++);
	i=(i<<8)|(*ip++);
	i=(i<<8)|(*ip++);
	*rip=ip;
	return(i);
}

s64 BGBDY_DecSVLI(byte **rip)
{
	s64 i;

	i=BGBDY_DecVLI(rip);
	i=(i&1)?(-((i>>1)+1)):(i>>1);
	return(i);
}

BGBDYC_TciTable *BGBDYC_AllocTCIT()
{
	BGBDYC_TciTable *tmp;

	tmp=gctalloc("_bgbdyc_tcit_t", sizeof(BGBDYC_TciTable));
	return(tmp);
}

int BGBDYC_TciIndexLit(BGBDYC_TciTable *tab, dyt val)
{
	int i;

	if(!tab->lit)
	{
		tab->lit=gcalloc(16*sizeof(dyt));
		tab->mlit=16;
	}

	for(i=0; i<tab->nlit; i++)
		if(dyeqp(tab->lit[i], val))
			return(i);

	if((tab->nlit+1)>=tab->mlit)
	{
		i=tab->mlit+(tab->mlit>>1);
		tab->lit=gcrealloc(tab->lit, i*sizeof(dyt));
		tab->mlit=i;
	}

	i=tab->nlit++;
	tab->lit[i]=val;
	return(i);
}

int BGBDYC_TciIndexFloat(BGBDYC_TciTable *tab, double val)
{
	int i;

	if(!tab->lit)
	{
		tab->lit=gcalloc(16*sizeof(dyt));
		tab->mlit=16;
	}

	for(i=0; i<tab->nlit; i++)
	{
		if(dyrealp(tab->lit[i]) && (dyrealv(tab->lit[i])==val))
			return(i);
	}

	if((tab->nlit+1)>=tab->mlit)
	{
		i=tab->mlit+(tab->mlit>>1);
		tab->lit=gcrealloc(tab->lit, i*sizeof(dyt));
		tab->mlit=i;
	}

	i=tab->nlit++;
	tab->lit[i]=dydouble(val);
	return(i);
}

int BGBDYC_TciReadTuple(BGBDYC_TciTable *tab, int idx, dyt *val)
{
	byte *s;
	dyt p;
	s64 li, lj;
	int i, j;

	if((idx<0) || (idx>=tab->ntab))
	{
		val[0]=NULL;
		return(-1);
	}

	if(!tab->tab || !tab->tab[idx])
	{
		val[0]=NULL;
		return(-1);
	}

	s=tab->tab[idx]; i=0;
	while(*s)
	{
		li=BGBDY_DecVLI(&s); p=NULL;
		switch(li&7)
		{
		case 0:
			switch(li>>3)
			{
			case 1: p=NULL; break;
			case 2: p=BGBDY_TRUE; break;
			case 3: p=BGBDY_FALSE; break;
			default: break;
			}
			break;
		case 1:
			lj=li>>3;
			lj=(lj&1)?(-((lj>>1)+1)):(lj>>1);
			p=dyint(lj);
			break;
		case 2:
			j=li>>3;
			p=tab->lit[j];
			break;
		}

		val[i++]=p;
	}

	val[i]=NULL;
	return(i);
}

int BGBDYC_TciWriteTuple(BGBDYC_TciTable *tab, int idx, dyt *val, int sz)
{
	byte tb[256];
	byte *s, *t;
	dyt p;
	s64 li;
	int i, j;

	if(!tab->tab)
	{
		i=16;
		while(idx>=i)i=i+(i>>1);
		tab->tab=gcalloc(i*sizeof(dyt));
		tab->mtab=i;
	}

	if(idx>=tab->mtab)
	{
		i=tab->mtab;
		while(idx>=i)i=i+(i>>1);
		tab->tab=gcrealloc(tab->tab, i*sizeof(dyt));
		tab->mtab=i;
	}

	if(idx>=tab->ntab)
		tab->ntab=idx+1;

	t=tb;
	for(i=0; i<sz; i++)
	{
		p=val[i];
		if(!p)	{ t=BGBDY_EncVLI(t, (1<<3)); continue; }
		if(p==BGBDY_TRUE)
			{ t=BGBDY_EncVLI(t, (2<<3)); continue; }
		if(p==BGBDY_FALSE)
			{ t=BGBDY_EncVLI(t, (3<<3)); continue; }

		if(dyintp(p))
		{
			li=dyintv(p);
			if((li>(-(1<<23))) && (li<(1<<23)))
			{
				li=(li>=0)?(li*2):(((-li)*2)-1);
				t=BGBDY_EncVLI(t, (li<<3)|1);
				continue;
			}
		}

		j=BGBDYC_TciIndexLit(tab, p);
		t=BGBDY_EncVLI(t, (j<<3)|2);
	}

	*t++=0;

	if(tab->tab[idx])
	{
		gcfree(tab->tab[idx]);
		tab->tab[idx]=NULL;
	}

	s=tb;
	i=t-s;
	tab->tab[idx]=gcalloc(i);
	memcpy(tab->tab[idx], tb, i);
}

int BGBDYC_TciIndexTuple(BGBDYC_TciTable *tab, dyt *val, int sz)
{
	dyt tv[16];
	int i, j;

	for(i=1; i<tab->ntab; i++)
		if(tab->tab[i])
	{
		j=BGBDYC_TciReadTuple(tab, i, tv);
		if(j!=sz)continue;

		for(j=0; j<sz; j++)
			if(!dyeqp(tv[j], val[j]))
				break;
		if(j>=sz)return(i);
	}

	for(i=1; i<tab->ntab; i++)
		if(!tab->tab[i])
	{
		BGBDYC_TciWriteTuple(tab, i, val, sz);
		return(i);
	}

	i=tab->ntab;
	BGBDYC_TciWriteTuple(tab, i, val, sz);
	return(i);
}

int BGBDYC_TciDecodeTuple(BGBDYC_TciTable *tab, char *str)
{
	byte tb[256];
	char **a;
	byte *s, *t, *s1;
	double d;
	dyt p;
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

	if(!tab->tab)
	{
		i=16;
		while(idx>=i)i=i+(i>>1);
		tab->tab=gcalloc(i*sizeof(dyt));
		tab->mtab=i;
	}

	if(idx>=tab->mtab)
	{
		i=tab->mtab;
		while(idx>=i)i=i+(i>>1);
		tab->tab=gcrealloc(tab->tab, i*sizeof(dyt));
		tab->mtab=i;
	}

	if(idx>=tab->ntab)
		tab->ntab=idx+1;

	t=tb;
	for(i=0; a[i+1]; i++)
	{
		s=a[i+1];

		if(s[strlen(s)+1]=='\"')
		{
			p=dystring(s);
			j=BGBDYC_TciIndexLit(tab, p);
			t=BGBDY_EncVLI(t, (j<<3)|2);
			continue;
		}

		if(!strcmp(s, "-"))
			{ t=BGBDY_EncVLI(t, (1<<3)); continue; }
		if(!strcmp(s, "-T"))
			{ t=BGBDY_EncVLI(t, (2<<3)); continue; }
		if(!strcmp(s, "-F"))
			{ t=BGBDY_EncVLI(t, (3<<3)); continue; }

		if((*s>='0') && (*s<='9'))
		{
			s1=s;
			while(*s1 && (*s1!='.') && (*s1!='e'))s1++;

			if(*s1)
			{
				d=atof(s);
				j=BGBDYC_TciIndexFloat(tab, d);
				t=BGBDY_EncVLI(t, (j<<3)|2);
				continue;
			}

			li=atoll(s);
			if((li>(-(1<<23))) && (li<(1<<23)))
			{
				li=(li>=0)?(li*2):(((-li)*2)-1);
				t=BGBDY_EncVLI(t, (li<<3)|1);
				continue;
			}

			p=dyint(li);
		}else
		{
			p=dysymbol(s);
		}

		j=BGBDYC_TciIndexLit(tab, p);
		t=BGBDY_EncVLI(t, (j<<3)|2);
	}

	*t++=0;

	if(tab->tab[idx])
	{
		gcfree(tab->tab[idx]);
		tab->tab[idx]=NULL;
	}

	s=tb;
	i=t-s;
	tab->tab[idx]=gcalloc(i);
	memcpy(tab->tab[idx], tb, i);

	return(i);
}

int BGBDYC_TciEncodeTuple(BGBDYC_TciTable *tab, int idx, char *buf)
{
	byte *s;
	char *t;
	dyt p;
	s64 li, lj;
	int i, j;

	if((idx<0) || (idx>=tab->ntab))
		return(-1);
	if(!tab->tab || !tab->tab[idx])
		return(-1);

	t=buf;
	sprintf(t, "%d", idx);
	t+=strlen(t);

	s=tab->tab[idx]; i=0;
	while(*s)
	{
		li=BGBDY_DecVLI(&s); p=NULL;
		switch(li&7)
		{
		case 0:
			switch(li>>3)
			{
			case 1: p=NULL; break;
			case 2: p=BGBDY_TRUE; break;
			case 3: p=BGBDY_FALSE; break;
			default: break;
			}
			break;
		case 1:
			lj=li>>3;
			lj=(lj&1)?(-((lj>>1)+1)):(lj>>1);
			p=dyint(lj);
			break;
		case 2:
			j=li>>3;
			p=tab->lit[j];
			break;
		}

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

BGBDYC_TciTable *BGBDYC_TciLoadTable(char *name)
{
	char tb[512];
	BGBDYC_TciTable *tmp;
	VFILE *fd;

	tmp=BGBDYC_AllocTCIT();
	fd=vffopen(name, "rt");
	while(!vfeof(fd))
	{
		vfgets(tb, 511, fd);
		BGBDYC_TciDecodeTuple(tmp, tb);
	}
	vfclose(fd);

	return(tmp);
}

int BGBDYC_TciSaveTable(char *name, BGBDYC_TciTable *tab)
{
	char tb[512];
	VFILE *fd;
	int i;

	fd=vffopen(name, "wt");
	for(i=1; i<tab->ntab; i++)
		if(tab->tab[i])
	{
		BGBDYC_TciEncodeTuple(tab, i, tb);
		vfputs(tb, fd);
	}
	vfclose(fd);
}

