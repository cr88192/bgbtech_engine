#include <bgbdy.h>

typedef struct {
// VFILE *fd;
byte *cs;

int c;					//next char
char *ns_mru[32];		//namespaces MRU
char *tag_mru[64];		//tags MRU
char *attr_mru[64];		//attribute name MRU
char *str_mru[128];		//strings MRU
double strd_mru[128];	//strings(double) MRU

byte *lzm_win;			//lz+markov window
u16 *lzm_chain;			//chains
u16 *lzm_hash;			//hash table
int lzm_pos;			//window pos
int lzm_ctx;			//context

char *typens;			//namespace prefix for types
}DYX_SBXE_Context;


void *sbxe_malloc(int sz)
{
	void *p;
	p=malloc(sz);
	memset(p, 0, sz);
	return(p);
}

void *sbxe_tmalloc(char *ty, int sz)
{
	return(sbxe_malloc(sz));
}

void sbxe_free(void *ptr)
{
	free(ptr);
}

char *sbxe_strdup(char *str)
{
	return(dystrdup(str));
}

//LZ+Markov

void DYX_SBXE_LZMUpdateByte(DYX_SBXE_Context *ctx, int v) //AH:ignore
{
	ctx->lzm_chain[ctx->lzm_pos]=ctx->lzm_hash[ctx->lzm_ctx];
	ctx->lzm_hash[ctx->lzm_ctx]=ctx->lzm_pos;

	ctx->lzm_win[ctx->lzm_pos]=v;
	ctx->lzm_pos=(ctx->lzm_pos+1)&16383;
	ctx->lzm_ctx=((ctx->lzm_ctx<<4)^v)&4095;
//	ctx->lzm_ctx=v;
}

void DYX_SBXE_LZMUpdateString(DYX_SBXE_Context *ctx,
	byte *s, int l) //AH:ignore
{
	while(l--)DYX_SBXE_LZMUpdateByte(ctx, *s++);
}

int DYX_SBXE_LZMFindMatch(DYX_SBXE_Context *ctx, byte *str,
	int *ri, int *rl) //AH:ignore
{
	int i, j, k, bi, bl;
	byte *s, *t;

	bi=0; bl=0;
	i=0; j=ctx->lzm_hash[ctx->lzm_ctx];
	while(i<16)
	{
		k=j;
		s=str;
		t=ctx->lzm_win+k;
		while(*s && (*s==*t)){s++; k=(k+1)&16383; t=ctx->lzm_win+k;}

		k=s-str;
		if(k>bl) { bi=i; bl=k; }

		i++;
		j=ctx->lzm_chain[j];
	}
	if(bl<3)return(0);
	*ri=bi; *rl=bl;
	return(1);
}

void DYX_SBXE_LZMEncodeString(DYX_SBXE_Context *ctx, char *str) //AH:ignore
{
	int i, j, k;
	byte *s;

	if(!ctx->lzm_win)
	{
		ctx->lzm_win=sbxe_malloc(16384);
		ctx->lzm_chain=sbxe_malloc(16384*sizeof(u16));
		ctx->lzm_hash=sbxe_malloc(4096*sizeof(u16));
		ctx->lzm_pos=0;
	}
	ctx->lzm_ctx=0x0000;	//initial context

	*ctx->cs++=0x11;

	s=str;
	while(*s)
	{
		i=DYX_SBXE_LZMFindMatch(ctx, s, &j, &k);
		if(i)
		{
			*ctx->cs++=0xF0+j;
			*ctx->cs++=k;
			DYX_SBXE_LZMUpdateString(ctx, s, k);
			s+=k;
		}else
		{
			*ctx->cs++=*s;
			if(*s>=0xF0)*ctx->cs++=0x00;
			DYX_SBXE_LZMUpdateByte(ctx, *s);
			s++;
		}
	}
	*ctx->cs++=0;
}

char *DYX_SBXE_LZMDecodeString(DYX_SBXE_Context *ctx) //AH:ignore
{
	byte buf[4096], *s, *t;
	int i, j, k, c;

	if(!ctx->lzm_win)
	{
		ctx->lzm_win=sbxe_malloc(16384);
		ctx->lzm_chain=sbxe_malloc(16384*sizeof(u16));
		ctx->lzm_hash=sbxe_malloc(4096*sizeof(u16));
		ctx->lzm_pos=0;
	}
	ctx->lzm_ctx=0x0000;	//initial context

	t=buf;
	c=(*ctx->cs++);
	while(c)
	{
		if(c>=0xF0)
		{
			i=(*ctx->cs++);
			if(!i)
			{
				*t++=c;
				DYX_SBXE_LZMUpdateByte(ctx, c);
				c=(*ctx->cs++);
				continue;
			}

			j=ctx->lzm_hash[ctx->lzm_ctx];
			k=c-0xF0;
			while(k--)j=ctx->lzm_chain[j];

			if((t+k)>=(buf+4096))
				break;

			for(k=0; k<i; k++)t[k]=ctx->lzm_win[(j+k)&16383];
			DYX_SBXE_LZMUpdateString(ctx, t, i);
			t+=i;

			c=(*ctx->cs++);
			continue;
		}

		if((t+1)>=(buf+4096))
			break;

		*t++=c;
		DYX_SBXE_LZMUpdateByte(ctx, c);
		c=(*ctx->cs++);
	}
	*t++=0;

	return(sbxe_strdup(buf));
}

//Misc Stuff

void DYX_SBXE_EncodeUVLI(DYX_SBXE_Context *ctx, s64 v) //AH:ignore
{
	int i;
	for(i=1; v>>(i*7); i++);
	while((i--)>1)*ctx->cs++=((v>>(i*7))&0x7F)|0x80;
	*ctx->cs++=v&0x7F;
}

s64 DYX_SBXE_DecodeUVLI(DYX_SBXE_Context *ctx) //AH:ignore
{
	s64 i;
	int c;

	i=0;
	c=(*ctx->cs++);
	while(c&0x80)
	{
		i=(i<<7)|(c&0x7F);
		c=(*ctx->cs++);
	}
	i=(i<<7)|(c&0x7F);
	return(i);
}

void DYX_SBXE_EncodeVLI(DYX_SBXE_Context *ctx, s64 v) //AH:ignore
{
	DYX_SBXE_EncodeUVLI(ctx, (v<0)?(((-v)<<1)|1):(v<<1));
}

s64 DYX_SBXE_DecodeVLI(DYX_SBXE_Context *ctx) //AH:ignore
{
	s64 i;
	i=DYX_SBXE_DecodeUVLI(ctx);
	return((i&1)?(-(i>>1)):(i>>1));
}

void DYX_SBXE_IndexMRU(DYX_SBXE_Context *ctx, int idx) //AH:ignore
{
	char *s;
	double v;
	int i, j;

	s=ctx->str_mru[idx];
	v=ctx->strd_mru[idx];
	for(i=idx; i<127; i++)
	{
		ctx->str_mru[i]=ctx->str_mru[i+1];
		ctx->strd_mru[i]=ctx->strd_mru[i+1];
	}
	ctx->str_mru[126]=s;
	ctx->strd_mru[126]=v;
}

void DYX_SBXE_ShiftMRU(DYX_SBXE_Context *ctx) //AH:ignore
{
	int i;
	for(i=0; i<127; i++)
	{
		ctx->str_mru[i]=ctx->str_mru[i+1];
		ctx->strd_mru[i]=ctx->strd_mru[i+1];
	}
}

void DYX_SBXE_TextMRU(DYX_SBXE_Context *ctx, char *str) //AH:ignore
{
	char *s;
	double v;
	int i, j;

	for(i=0; i<127; i++)
		if(ctx->str_mru[i] &&
			(ctx->str_mru[i]!=UNDEFINED) &&
			!strcmp(ctx->str_mru[i], str))
	{
		DYX_SBXE_IndexMRU(ctx, i);
		return;
	}
	DYX_SBXE_ShiftMRU(ctx);
	ctx->str_mru[126]=sbxe_strdup(str);
}

void DYX_SBXE_NumberMRU(DYX_SBXE_Context *ctx, double val) //AH:ignore
{
	char *s;
	double v;
	int i, j;

	for(i=0; i<127; i++)
		if((ctx->str_mru[i]==UNDEFINED) && (ctx->strd_mru[i]==val))
	{
		DYX_SBXE_IndexMRU(ctx, i);
		return;
	}
	DYX_SBXE_ShiftMRU(ctx);
	ctx->str_mru[126]=UNDEFINED;
	ctx->strd_mru[126]=val;
}


//Encoder

void DYX_SBXE_EncodeNumber(DYX_SBXE_Context *ctx, double val) //AH:ignore
{
	char *s;
	s64 li, lj, lk;
	int i, j;

	for(i=0; i<127; i++)
		if((ctx->str_mru[i]==UNDEFINED) && (ctx->strd_mru[i]==val))
	{
		DYX_SBXE_IndexMRU(ctx, i);
		*ctx->cs++=0x80+i;
		return;
	}
	
	i=val;
	if(i==val)
	{
		*ctx->cs++=0x10;
		DYX_SBXE_EncodeVLI(ctx, i);
		DYX_SBXE_NumberMRU(ctx, i);
		return;
	}
	
#if 1
	li=(s64)val;
	lj=(val-li)*1000000000;
	lk=0;
	for(i=0; i<9; i++)
	{
		if(lj%10)break;
		lj/=10;
	}
	for(; i<9; i++)
	{
		j=lj%10;
		lk=(lk*11)+(j+1);
	}

	*ctx->cs++=0x16;
	DYX_SBXE_EncodeVLI(ctx, li);
	DYX_SBXE_EncodeUVLI(ctx, lk);
	DYX_SBXE_NumberMRU(ctx, val);
	return;
#endif

//	DYX_SBXE_EncodeText(ctx, gcrdtoa(val));
}

void DYX_SBXE_EncodeText(DYX_SBXE_Context *ctx, char *str) //AH:ignore
{
	char *s;
	int i, j;

	for(i=0; i<127; i++)
		if(ctx->str_mru[i] &&
			(ctx->str_mru[i]!=UNDEFINED) &&
			!strcmp(ctx->str_mru[i], str))
	{
		DYX_SBXE_IndexMRU(ctx, i);
		*ctx->cs++=0x80+i;
		return;
	}

#if 1
	//special case: integer string in probably sane range
	s=str;
	if(*s=='-')s++;
	while(*s && (*s>='0') && (*s<='9'))s++;

	if(!(*s) && (strlen(str)<=8))
	{
		i=atoi(str);
		*ctx->cs++=0x10;
		DYX_SBXE_EncodeVLI(ctx, i);
//		DYX_SBXE_TextMRU(ctx, str);
		DYX_SBXE_NumberMRU(ctx, i);
		return;
	}
#endif

#if 1
	DYX_SBXE_LZMEncodeString(ctx, str);
	DYX_SBXE_TextMRU(ctx, str);
	return;
#endif

#if 0
	for(i=0; i<127; i++)
	{
		ctx->str_mru[i]=ctx->str_mru[i+1];
		ctx->strd_mru[i]=ctx->strd_mru[i+1];
	}
	ctx->str_mru[126]=sbxe_strdup(str);
	*ctx->cs++=0xFF;
	memcpy(ctx->cs, str, strlen(str)+1);
	ctx->cs+=strlen(str)+1;
#endif
}

void DYX_SBXE_EncodeTag(DYX_SBXE_Context *ctx, char *str) //AH:ignore
{
	char *s;
	int i, j, k, l;

	for(i=0; i<63; i++)
		if(ctx->tag_mru[i] && !strcmp(ctx->tag_mru[i], str))
	{
		s=ctx->tag_mru[i];
		for(j=i; j<63; j++)ctx->tag_mru[j]=ctx->tag_mru[j+1];
		ctx->tag_mru[62]=s;
		*ctx->cs++=0x40+i;
		return;
	}
	for(i=0; i<63; i++)ctx->tag_mru[i]=ctx->tag_mru[i+1];
	ctx->tag_mru[62]=sbxe_strdup(str);
	*ctx->cs++=0x7F;
	memcpy(ctx->cs, str, strlen(str)+1);
	ctx->cs+=strlen(str)+1;
}

void DYX_SBXE_EncodeAttr(DYX_SBXE_Context *ctx, char *str) //AH:ignore
{
	char *s;
	int i, j;

	for(i=0; i<63; i++)
		if(ctx->attr_mru[i] && !strcmp(ctx->attr_mru[i], str))
	{
		s=ctx->attr_mru[i];
		for(j=i; j<63; j++)ctx->attr_mru[j]=ctx->attr_mru[j+1];
		ctx->attr_mru[62]=s;
		*ctx->cs++=0x40+i;
		return;
	}
	for(i=0; i<63; i++)ctx->attr_mru[i]=ctx->attr_mru[i+1];
	ctx->attr_mru[62]=sbxe_strdup(str);
	*ctx->cs++=0x7F;
	memcpy(ctx->cs, str, strlen(str)+1);
	ctx->cs+=strlen(str)+1;
}

void DYX_SBXE_EncodeNS(DYX_SBXE_Context *ctx, char *str) //AH:ignore
{
	char *s;
	int i, j;

	for(i=0; i<31; i++)
		if(ctx->ns_mru[i] && !strcmp(ctx->ns_mru[i], str))
	{
		s=ctx->ns_mru[i];
		for(j=i; j<31; j++)ctx->ns_mru[j]=ctx->ns_mru[j+1];
		ctx->ns_mru[30]=s;
		*ctx->cs++=0x20+i;
		return;
	}
	for(i=0; i<31; i++)ctx->ns_mru[i]=ctx->ns_mru[i+1];
	ctx->ns_mru[30]=sbxe_strdup(str);
	*ctx->cs++=0x3F;
	memcpy(ctx->cs, str, strlen(str)+1);
	ctx->cs+=strlen(str)+1;
}

void DYX_SBXE_EncodeAttrList(DYX_SBXE_Context *ctx,
	dyxAttr *first) //AH:ignore
{
	dyxAttr *acur;

	acur=first;
	while(acur)
	{
		if(acur->ns)DYX_SBXE_EncodeNS(ctx, acur->ns);
		DYX_SBXE_EncodeAttr(ctx, acur->var);
		if(acur->val==UNDEFINED)
				DYX_SBXE_EncodeNumber(ctx, acur->dval);
		else	DYX_SBXE_EncodeText(ctx, acur->val);
		acur=acur->next;
	}
	*ctx->cs++=0;
}

void DYX_SBXE_EncodeNode(DYX_SBXE_Context *ctx, dyxNode *expr) //AH:ignore
{
	dyxNode *ncur;
	dyxAttr *acur;

	char buf[256], *s, *otns;
	int i;

#if 0
	if(expr->type==NETPARSE_NODE_BINARY)
	{
		*ctx->cs++=0x13;
		if(expr->ns)DYX_SBXE_EncodeNS(ctx, expr->ns);
		DYX_SBXE_EncodeTag(ctx, expr->key);
//		DYX_SBXE_EncodeAttrList(ctx, expr->attr);

		s=NetParse_GetNodeNamespacePrefix(expr, NETPARSE_XMLNS_TYPES);

#if 1	//skip binary size attribute (remade on load)
		acur=expr->attr;
		while(acur)
		{
			if(s && acur->ns && !strcmp(acur->ns, s))
			{
				if(!strcmp(acur->key, "size"))
				{
					acur=acur->next;
					continue;
				}
				if(!strcmp(acur->key, "type") &&
					!strcmp(acur->value, "binary.base64"))
				{
					acur=acur->next;
					continue;
				}
			}

			if(acur->ns)DYX_SBXE_EncodeNS(ctx, acur->ns);
			DYX_SBXE_EncodeAttr(ctx, acur->key);
			DYX_SBXE_EncodeText(ctx, acur->value);
			acur=acur->next;
		}
		*ctx->cs++=0;
#endif

//		i=NetParse_GetNodeIntAttrNS(expr, "dt", "size");
		i=NetParse_GetNodeIntAttrNS(expr, s, "size");

		DYX_SBXE_EncodeUVLI(ctx, i);
		memcpy(ctx->cs, expr->text, i);
		ctx->cs+=i;

		return;
	}
#endif

#if 0
	if(expr->type==NETPARSE_NODE_CDATA_SECTION)
	{
		*ctx->cs++=0x12;
		s=expr->text;
		while(strlen(s)>255)
		{
			strncpy(s, buf, 256);
			buf[255]=0;
			DYX_SBXE_EncodeText(ctx, buf);
			s+=255;
		}
		DYX_SBXE_EncodeText(ctx, s);
		*ctx->cs++=0;
		return;
	}
#endif

	if(!expr->tag)
	{
		if(!expr->text)
		{
//			kprint("DYX_SBXE_EncodeNode: Bad Node\n");
			return;
		}

		s=expr->text;
		while(strlen(s)>255)
		{
			strncpy(s, buf, 256);
			buf[255]=0;
			DYX_SBXE_EncodeText(ctx, buf);
			s+=255;
		}
		DYX_SBXE_EncodeText(ctx, s);

		return;
	}

	if(expr->ns)DYX_SBXE_EncodeNS(ctx, expr->ns);
//	DYX_SBXE_EncodeTag(ctx, expr->key);
//	DYX_SBXE_EncodeAttrList(ctx, expr->attr);

	s=buf;
	if(!expr->attr && !expr->down){*s++='=';}
		else if(!expr->attr){*s++=':';}
		else if(!expr->down){*s++='/';}
	strcpy(s, expr->tag);
	DYX_SBXE_EncodeTag(ctx, buf);

	if(expr->attr)DYX_SBXE_EncodeAttrList(ctx, expr->attr);
	if(!expr->down)
		return;

	ncur=expr->down;
	while(ncur)
	{
		DYX_SBXE_EncodeNode(ctx, ncur);
		ncur=ncur->next;
	}
	*ctx->cs++=0;
}

void DYX_SBXE_EncodeNodeList(DYX_SBXE_Context *ctx, dyxNode *expr) //AH:ignore
{
	dyxNode *ncur;

	ncur=expr;
	while(ncur)
	{
		DYX_SBXE_EncodeNode(ctx, ncur);
		ncur=ncur->next;
	}
	*ctx->cs++=0;
}


//Decoder

int DYX_SBXE_DecodeCheckNumber(DYX_SBXE_Context *ctx) //AH:ignore
{
	byte *op;
	int n;

	op=ctx->cs-1;
	n=0;

	if(ctx->c==0x10)
	{
		DYX_SBXE_DecodeVLI(ctx);
		if(!(*ctx->cs))n=1;
	}

	if(ctx->c==0x16)
	{
		DYX_SBXE_DecodeVLI(ctx);
		DYX_SBXE_DecodeUVLI(ctx);
		if(!(*ctx->cs))n=1;
	}

	ctx->cs=op;
	ctx->c=(*ctx->cs++);
	return(n);
}

double DYX_SBXE_DecodeNumber(DYX_SBXE_Context *ctx) //AH:ignore
{
	double v, f;
	s64 li;
	int i;

	if(ctx->c==0x10)
	{
		v=DYX_SBXE_DecodeVLI(ctx);
		DYX_SBXE_ShiftMRU(ctx);
		ctx->str_mru[126]=UNDEFINED;
		ctx->strd_mru[126]=v;
		return(v);
	}

	if(ctx->c==0x16)
	{
		v=DYX_SBXE_DecodeVLI(ctx);
		li=DYX_SBXE_DecodeUVLI(ctx);
		
		f=0.1;
		while(li)
		{
			i=(li%11)-1; li/=11;
			v=v+(i*f); f*=0.1;
		}
		
		DYX_SBXE_ShiftMRU(ctx);
		ctx->str_mru[126]=UNDEFINED;
		ctx->strd_mru[126]=v;
		return(v);
	}
	
	return(0);
}

char *DYX_SBXE_DecodeText(DYX_SBXE_Context *ctx) //AH:ignore
{
	char buf[256], *s;
	double v;
	int i, j;

	if(ctx->c==0x10)
	{
		i=DYX_SBXE_DecodeVLI(ctx);
		sprintf(buf, "%d", i);
		s=sbxe_strdup(buf);
//		for(j=0; j<127; j++)ctx->str_mru[j]=ctx->str_mru[j+1];
		DYX_SBXE_ShiftMRU(ctx);
		ctx->str_mru[126]=s;
		return(s);
	}

	if(ctx->c==0x11)
	{
		s=DYX_SBXE_LZMDecodeString(ctx);
//		for(j=0; j<127; j++)ctx->str_mru[j]=ctx->str_mru[j+1];
		DYX_SBXE_ShiftMRU(ctx);
		ctx->str_mru[126]=s;
		return(s);
	}

	if(ctx->c==0x16)
	{
		v=DYX_SBXE_DecodeNumber(ctx);
		s=sbxe_strdup(gcrdtoa(v));
		DYX_SBXE_ShiftMRU(ctx);
		ctx->str_mru[126]=s;
		return(s);
	}

	if(ctx->c==0xFF)
	{
		s=buf; j=-1;
		while(j){ j=(*ctx->cs++); *s++=j; }

		s=sbxe_strdup(buf);
//		for(j=0; j<127; j++)ctx->str_mru[j]=ctx->str_mru[j+1];
		DYX_SBXE_ShiftMRU(ctx);
		ctx->str_mru[126]=s;
		return(s);
	}
	if(ctx->c>=0x80)
	{
		i=ctx->c-0x80;
		s=ctx->str_mru[i];
		v=ctx->strd_mru[i];
//		for(j=i; j<127; j++)ctx->str_mru[j]=ctx->str_mru[j+1];
		DYX_SBXE_ShiftMRU(ctx);
		ctx->str_mru[126]=s;
		ctx->strd_mru[126]=v;
		
		if(s==UNDEFINED)
			s=sbxe_strdup(gcrdtoa(v));
		return(s);
	}

	return(NULL);
}

char *DYX_SBXE_DecodeTag(DYX_SBXE_Context *ctx) //AH:ignore
{
	char buf[256], *s;
	int i, j;

	if(ctx->c==0x7F)
	{
		s=buf; j=-1;
		while(j){ j=(*ctx->cs++); *s++=j; }
		s=sbxe_strdup(buf);
		for(j=0; j<63; j++)ctx->tag_mru[j]=ctx->tag_mru[j+1];
		ctx->tag_mru[62]=s;
		return(s);
	}
	if(ctx->c>=0x40)
	{
		i=ctx->c-0x40; s=ctx->tag_mru[i];
		for(j=i; j<63; j++)ctx->tag_mru[j]=ctx->tag_mru[j+1];
		ctx->tag_mru[62]=s;
		return(s);
	}
	return(NULL);
}

char *DYX_SBXE_DecodeAttr(DYX_SBXE_Context *ctx) //AH:ignore
{
	char buf[256], *s;
	int i, j;

	if(ctx->c==0x7F)
	{
		s=buf; j=-1;
		while(j){ j=(*ctx->cs++); *s++=j; }
		s=sbxe_strdup(buf);
		for(j=0; j<63; j++)ctx->attr_mru[j]=ctx->attr_mru[j+1];
		ctx->attr_mru[62]=s;
		return(s);
	}
	if(ctx->c>=0x40)
	{
		i=ctx->c-0x40; s=ctx->attr_mru[i];
		for(j=i; j<63; j++)ctx->attr_mru[j]=ctx->attr_mru[j+1];
		ctx->attr_mru[62]=s;
		return(s);
	}
	return(NULL);
}

char *DYX_SBXE_DecodeNS(DYX_SBXE_Context *ctx) //AH:ignore
{
	char buf[256], *s;
	int i, j;

	if(ctx->c==0x3F)
	{
		s=buf; j=-1;
		while(j){ j=(*ctx->cs++); *s++=j; }
		s=sbxe_strdup(buf);
		for(j=0; j<31; j++)ctx->ns_mru[j]=ctx->ns_mru[j+1];
		ctx->ns_mru[30]=s;
		return(s);
	}
	if((ctx->c>=0x20) && (ctx->c<0x3F))
	{
		i=ctx->c-0x20; s=ctx->ns_mru[i];
		for(j=i; j<31; j++)ctx->ns_mru[j]=ctx->ns_mru[j+1];
		ctx->ns_mru[30]=s;
		return(s);
	}
	return(NULL);
}


dyxNode *DYX_SBXE_DecodeTextNode(DYX_SBXE_Context *ctx) //AH:ignore
{
	static char buf[16384];
	dyxNode *n;
	char *s, *t;
	int i, cd;

	if(ctx->c==0x12)
	{
		ctx->c=(*ctx->cs++);
		cd=1;
	}else cd=0;

	i=0;
	t=buf;
	while(1)
	{
		if((ctx->c>=0x80) || (ctx->c==0x10) || (ctx->c==0x11))
		{
			s=DYX_SBXE_DecodeText(ctx);
			strcpy(t, s);
			t+=strlen(s);

			i++;
			ctx->c=(*ctx->cs++);
			continue;
		}

		*t=0;
		break;
	}

	n=dyxNewText(buf);
//	if(cd)n->type=NETPARSE_NODE_CDATA_SECTION;
//		else n->type=NETPARSE_NODE_TEXT;
	return(n);
}

dyxAttr *DYX_SBXE_DecodeAttribute(DYX_SBXE_Context *ctx) //AH:ignore
{
	static char buf[16384];
	dyxAttr *tmp;
	double v;
	char *s, *t;
	int i, j;

	tmp=dyxNewAttr();

	tmp->ns=DYX_SBXE_DecodeNS(ctx);
	if(tmp->ns)ctx->c=(*ctx->cs++);

	tmp->var=DYX_SBXE_DecodeAttr(ctx);
	ctx->c=(*ctx->cs++);

	if(DYX_SBXE_DecodeCheckNumber(ctx))
	{
		v=DYX_SBXE_DecodeNumber(ctx);
		ctx->c=(*ctx->cs++);
		tmp->val=UNDEFINED;
		tmp->dval=v;
		return(tmp);
	}

	i=0;
	t=buf;
	while(1)
	{
		if((ctx->c>=0x80) || (ctx->c==0x10) || (ctx->c==0x11))
		{
			s=DYX_SBXE_DecodeText(ctx);
			strcpy(t, s);
			t+=strlen(s);

			i++;
			ctx->c=(*ctx->cs++);
			continue;
		}

		*t=0;
		break;
	}

	if(i==1)tmp->val=s;
		else tmp->val=sbxe_strdup(buf);
	return(tmp);
}

dyxAttr *DYX_SBXE_DecodeAttributeList(DYX_SBXE_Context *ctx) //AH:ignore
{
	dyxAttr *atmp, *alst, *afst;

	afst=NULL; alst=NULL;
	while(ctx->c > 0)
	{
		atmp=DYX_SBXE_DecodeAttribute(ctx);
		if(afst)
		{
			alst->next=atmp;
			alst=atmp;
		}else
		{
			afst=atmp;
			alst=atmp;
		}
	}

	return(afst);
}

#if 0
dyxNode *DYX_SBXE_DecodeDataNode(DYX_SBXE_Context *ctx,
	dyxNode *up) //AH:ignore
{
	dyxNode *tmp;
	byte *buf;
	char *s, *t;
	int i;

	tmp=dyxNew(NULL);
	tmp->up=up;
	ctx->c=(*ctx->cs++);

	tmp->ns=DYX_SBXE_DecodeNS(ctx);
	if(tmp->ns)ctx->c=(*ctx->cs++);
	tmp->key=DYX_SBXE_DecodeTag(ctx);

	ctx->c=(*ctx->cs++);
	tmp->attr=DYX_SBXE_DecodeAttributeList(ctx);

	i=DYX_SBXE_DecodeUVLI(ctx);
	buf=kalloc(i);
	memcpy(buf, i, ctx->cs);
	ctx->cs+=i;

	tmp->text=buf;
//	NetParse_SetNodeIntAttrNS(tmp, "dt", "size", i);
	s=NetParse_GetNodeNamespacePrefix(tmp, NETPARSE_XMLNS_TYPES);

	//hack: binary node but no namespace, come up with one
	if(!s)
	{
		NetParse_SetNodeAttrNS(tmp, "xmlns", "dtx",
			NETPARSE_XMLNS_TYPES);
		s="dtx";
	}

	if(s)
	{
		NetParse_SetNodeIntAttrNS(tmp, s, "size", i);
		t=NetParse_GetNodeAttrNS(tmp, s, "type");
		if(!t)NetParse_SetNodeAttrNS(tmp, s, "type", "binary.base64");
	}

	ctx->c=(*ctx->cs++);
	return(tmp);
}
#endif

dyxNode *DYX_SBXE_DecodeNodeList(DYX_SBXE_Context *ctx,
	dyxNode *up); //AH:ignore

dyxNode *DYX_SBXE_DecodeNode(DYX_SBXE_Context *ctx,
	dyxNode *up) //AH:ignore
{
	char *s, *t;
	dyxNode *tmp, *n, *lst;
	int i;

#if 0
	if(ctx->c==0x13)
	{
		tmp=DYX_SBXE_DecodeDataNode(ctx, up);
		return(tmp);
	}
#endif

	if((ctx->c>=0x80) || (ctx->c==0x10) ||
		(ctx->c==0x11) || (ctx->c==0x12))
	{
		tmp=DYX_SBXE_DecodeTextNode(ctx);
		return(tmp);
	}

	tmp=dyxNew(NULL);
	tmp->up=up;
	tmp->ns=DYX_SBXE_DecodeNS(ctx);
	if(tmp->ns)ctx->c=(*ctx->cs++);

	s=DYX_SBXE_DecodeTag(ctx);
	t=s;
	if((*t=='/') || (*t==':') || (*t=='='))
		t=sbxe_strdup(s+1);	//keep interned
		//t++;
	tmp->tag=t;

	if((*s!=':') && (*s!='='))
	{
		ctx->c=(*ctx->cs++);
		tmp->attr=DYX_SBXE_DecodeAttributeList(ctx);
	}

	if((*s!='/') && (*s!='='))
	{
		lst=DYX_SBXE_DecodeNodeList(ctx, tmp);
		tmp->down=lst;
	}

	ctx->c=(*ctx->cs++);
	return(tmp);
}

dyxNode *DYX_SBXE_DecodeNodeList(DYX_SBXE_Context *ctx, dyxNode *up) //AH:ignore
{
	dyxNode *tmp, *lst;

	lst=NULL;
	ctx->c=(*ctx->cs++);
	while(ctx->c > 0)
	{
		tmp=DYX_SBXE_DecodeNode(ctx, up);
		lst=dyxAddEnd(lst, tmp);
	}
	return(lst);
}


//general

DYX_SBXE_Context *DYX_SBXE_NewContext(byte *buf) //AH:ignore
{
	DYX_SBXE_Context *tmp;

	tmp=sbxe_tmalloc("_sbxe_context_t", sizeof(DYX_SBXE_Context));
	tmp->cs=buf;
	return(tmp);
}

int DYX_SBXE_DestroyContext(DYX_SBXE_Context *ctx) //AH:ignore
{
	if(ctx->lzm_win)sbxe_free(ctx->lzm_win);
	if(ctx->lzm_chain)sbxe_free(ctx->lzm_chain);
	if(ctx->lzm_hash)sbxe_free(ctx->lzm_hash);
	sbxe_free(ctx);
	return(0);
}


BGBDY_API int DYX_SBXE_WriteNodeList(byte *buf, dyxNode *expr)
{
	DYX_SBXE_Context *ctx;
	int sz;

	ctx=DYX_SBXE_NewContext(buf);
	DYX_SBXE_EncodeNodeList(ctx, expr);
	sz=ctx->cs-buf;
	DYX_SBXE_DestroyContext(ctx);
	return(sz);
}

BGBDY_API dyxNode *DYX_SBXE_ReadNodeList(byte *buf, int sz)
{
	DYX_SBXE_Context *ctx;
	dyxNode *tmp;

	if(!strncmp(buf, "SBXE", 4))
		buf+=strlen(buf)+1;

	ctx=DYX_SBXE_NewContext(buf);
	tmp=DYX_SBXE_DecodeNodeList(ctx, NULL);
	DYX_SBXE_DestroyContext(ctx);
	return(tmp);
}
