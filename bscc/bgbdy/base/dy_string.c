#include <bgbdy.h>

dyt bgbdy_string_length(dyt obj)
{
	char *s;
	int i;

	s=dystringv(obj);
	i=0;
	while(*s)
		{ i++; BGBDY_ParseChar(&s); }
	return(dyint(i));
}

dyt bgbdy_string_split(dyt obj)
{
	dyt da[256];
	char **a;
	dyt t;
	int i;
	
	a=gcrsplit(dystringv(obj));
	for(i=0; a[i]; i++)
		da[i]=dystring(a[i]);
	t=dyWrapArray(da, i);
	return(t);
}

dyt bgbdy_string_toLower(dyt obj)
{
	char tb[4096];
	char *s, *t;
	int i, hi;

	s=dystringv(obj); t=tb;
	while(*s)
	{
		i=BGBDY_ParseChar(&s);
		if((i>='A') && (i<='Z'))
			i=(i-'A')+'a';
		BGBDY_EmitChar(&t, i);
	}
	*t++=0;
	
	return(dystring(tb));
}

dyt bgbdy_string_toUpper(dyt obj)
{
	char tb[4096];
	char *s, *t;
	int i, hi;

	s=dystringv(obj); t=tb;
	while(*s)
	{
		i=BGBDY_ParseChar(&s);
		if((i>='a') && (i<='z'))
			i=(i-'a')+'A';
		BGBDY_EmitChar(&t, i);
	}
	*t++=0;
	
	return(dystring(tb));
}

dyt bgbdy_string_hash(dyt obj)
{
	char *s;
	int i, hi;

	s=dystringv(obj);
	hi=0;
	while(*s)
		hi=(hi*251)+(*s++);
	return(dyint(hi));
}

int BGBDY_InitStringTypes()
{
	dyTypeMethod0("_string_t", "hash", bgbdy_string_hash);
	dyTypeMethod0("_string_t", "length", bgbdy_string_length);
	dyTypeMethod0("_string_t", "split", bgbdy_string_split);
	dyTypeMethod0("_string_t", "toLower", bgbdy_string_toLower);
	dyTypeMethod0("_string_t", "toUpper", bgbdy_string_toUpper);

	return(0);
}

BGBDY_API char **dyStableSplit(char *str)
{
	char **a, **b;
	int i;

	a=gcrsplit(str);
	for(i=0; a[i]; i++);
	b=gcalloc((i+1)*sizeof(char *));
	
	for(i=0; a[i]; i++)
		b[i]=dystrdup(a[i]);
	b[i]=NULL;
	return(b);
}
