#include <bgbasm.h>

BASM_PE_Info *basm_pe_image=NULL;


BASM_PE_Info *BASM_PE_LookupImage(char *name)
{
	BASM_PE_Info *cur;

	cur=basm_pe_image;
	while(cur)
	{
		if(!strcmp(cur->name, name))
			return(cur);
		cur=cur->next;
	}

	return(NULL);
}

char *BASM_PE_NormalizeName(char *name)
{
	char tb[256];
	char *s, *t;
	int i;

	s=name;
	while((s>name) && ((*s!='/') && (*s!='\\'))) s--;

	t=tb;
	while(*s)
	{
		if(*s=='.')break;
		i=*s++;
		if((i>='A') && (i<='Z'))
			i=i-'A'+'a';
		*t++=i;
	}
	*t++=0;

	return(basm_strdup(tb));
}


byte *BASM_PE_AllocImageSpace(s64 base, int sz)
{
	byte *p;
	int i, j, k;

#ifdef linux
	p=(byte *)mmap((void *)base, sz,
		PROT_READ|PROT_WRITE|PROT_EXEC,
		MAP_PRIVATE|MAP_ANONYMOUS, -1, 0);

	if(!p)
	{
		p=(byte *)mmap(NULL, sz,
			PROT_READ|PROT_WRITE|PROT_EXEC,
			MAP_PRIVATE|MAP_ANONYMOUS, -1, 0);
	}
#else
#ifdef WIN32
	p=(byte *)VirtualAlloc((void *)base, sz,
		MEM_COMMIT|MEM_RESERVE, PAGE_EXECUTE_READWRITE);
	if(!p)
	{
		p=(byte *)VirtualAlloc(NULL, sz,
			MEM_COMMIT|MEM_RESERVE, PAGE_EXECUTE_READWRITE);
	}
#else
	p=(byte *)malloc(sz);
#endif
#endif

	return(p);
}


byte *BASM_PE_LookupImageSectionBase(BASM_PE_Info *ctx, char *name)
{
	int i;

	for(i=0; i<ctx->nsecs; i++)
	{
		if(!strcmp(ctx->sec_name[i], name))
			return(ctx->img_addr+ctx->sec_rva_off[i]);
	}
	return(NULL);
}

byte *BASM_PE_LookupImageSectionEnd(BASM_PE_Info *ctx, char *name)
{
	byte *p;
	int i;

	for(i=0; i<ctx->nsecs; i++)
	{
		if(!strcmp(ctx->sec_name[i], name))
		{
			p=ctx->img_addr+ctx->sec_rva_off[i]+
				ctx->sec_rva_sz[i];
			return(p);
		}
	}
	return(NULL);
}

byte *BASM_PE_LookupImageExport(BASM_PE_Info *ctx, char *name)
{
	byte *p;
	int i;

	i=BASM_PE_LookupImageExportNameOrdinal(ctx, name);
	if(i<=0)return(NULL);

	p=BASM_PE_LookupImageExportOrdinal(ctx, i);
	return(p);
}

int BASM_PE_LookupImageExportNameOrdinal(BASM_PE_Info *ctx, char *name)
{
	byte *a, *b, *c, *d;
	char *s;
	int i, j, k, n;

	a=ctx->img_addr+ctx->ddir_rva[0];
	n=a[24]|(a[25]<<8)|(a[26]<<16)|(a[27]<<24);

	i=a[32]|(a[33]<<8)|(a[34]<<16)|(a[35]<<24);
	j=a[36]|(a[37]<<8)|(a[38]<<16)|(a[39]<<24);
	b=ctx->img_addr+i;
	c=ctx->img_addr+j;

	for(i=0; i<n; i++)
	{
		j=b[i*4+0]|(b[i*4+1]<<8)|(b[i*4+2]<<16)|(b[i*4+3]<<24);
		s=ctx->img_addr+j;

		if(!strcmp(s, name))
		{
			k=c[i*2+0]|(c[i*2+1]<<8);
			return(k);
		}
	}
	return(0);
}

byte *BASM_PE_LookupImageExportOrdinal(BASM_PE_Info *ctx, int ord)
{
	byte *a, *b, *c, *d;
	int i, j, k;

	a=ctx->img_addr+ctx->ddir_rva[0];
	i=a[16]|(a[17]<<8)|(a[18]<<16)|(a[19]<<24);

	j=a[28]|(a[29]<<8)|(a[30]<<16)|(a[31]<<24);
	b=ctx->img_addr+j;	//Export Address Table

	c=b+(ord-i)*8;

	k=c[4]|(c[5]<<8)|(c[6]<<16)|(c[7]<<24);
	if(k)
	{
		//forwarded symbol
		//yeah...
		return(NULL);
	}

	k=c[0]|(c[1]<<8)|(c[2]<<16)|(c[3]<<24);

	d=ctx->img_addr+k;
	return(k);
}


BASM_PE_Info *BASM_PE_LoadImageBuf(char *name, byte *buf, int sz)
{
	char nbuf[256];

	BASM_PE_Info *ctx, *ctx2;

	void *p, *q;
	byte *a, *b, *cs, *ct, *ce, *ce2;
	char *s;
	int optoffs;
	s64 disp, addr;
//	int syo;
	int i, j, k, l;

	if((buf[0]!='M') || (buf[1]!='Z'))
	{
		printf("BASM_PE_LoadImageBuf: MZ Sig Fail\n");
		return(NULL);
	}

	ctx=malloc(sizeof(BASM_PE_Info));
	memset(ctx, 0, sizeof(BASM_PE_Info));

	ctx->name=basm_strdup(name);
	ctx->buf=buf;
	ctx->bsz=sz;

	ctx->next=basm_pe_image;
	basm_pe_image=ctx;


	ctx->peoffs=buf[0x3C]|(buf[0x3D]<<8)|(buf[0x3E]<<16)|(buf[0x3F]<<24);

	if((buf[ctx->peoffs+0]!='P') || (buf[ctx->peoffs+1]!='E'))
	{
		printf("BASM_PE_LoadImageBuf: PE Sig Fail\n");
		return(NULL);
	}

	printf("PE Sig At 0x%X\n", ctx->peoffs);
	ctx->coff_hdr=ctx->peoffs+4;

	i=buf[ctx->coff_hdr+0]|(buf[ctx->coff_hdr+1]<<8);
	ctx->mach=i;
	printf("\tMachine 0x%04X\n", i);

	ctx->symoffs=buf[ctx->coff_hdr+8]|(buf[ctx->coff_hdr+9]<<8)|
		(buf[ctx->coff_hdr+10]<<16)|(buf[ctx->coff_hdr+11]<<24);
	ctx->nsyms=buf[ctx->coff_hdr+12]|(buf[ctx->coff_hdr+13]<<8)|
		(buf[ctx->coff_hdr+14]<<16)|(buf[ctx->coff_hdr+15]<<24);

	printf("\tSyms 0x%X %d\n", ctx->symoffs, ctx->nsyms);

	ctx->stroffs=ctx->symoffs+(ctx->nsyms*18);

	i=buf[ctx->coff_hdr+16]|(buf[ctx->coff_hdr+17]<<8);
	ctx->optoffs=ctx->coff_hdr+20;
	ctx->secoffs=ctx->coff_hdr+20+i;

	ctx->nsecs=buf[ctx->coff_hdr+2]|(buf[ctx->coff_hdr+3]<<8);

	if(ctx->mach==0x8664)
	{
		ctx->img_base=	( (s64)buf[ctx->coff_hdr+20+24])|
				(((s64)buf[ctx->coff_hdr+20+25])<<8)|
				(((s64)buf[ctx->coff_hdr+20+26])<<16)|
				(((s64)buf[ctx->coff_hdr+20+27])<<24)|
				(((s64)buf[ctx->coff_hdr+20+28])<<32)|
				(((s64)buf[ctx->coff_hdr+20+29])<<40)|
				(((s64)buf[ctx->coff_hdr+20+30])<<48)|
				(((s64)buf[ctx->coff_hdr+20+31])<<56);

		ctx->img_size=	 buf[ctx->coff_hdr+20+56]|
				(buf[ctx->coff_hdr+20+57]<<8)|
				(buf[ctx->coff_hdr+20+58]<<16)|
				(buf[ctx->coff_hdr+20+59]<<24);

		ctx->nddir=	 buf[ctx->coff_hdr+20+108]|
				(buf[ctx->coff_hdr+20+109]<<8)|
				(buf[ctx->coff_hdr+20+110]<<16)|
				(buf[ctx->coff_hdr+20+111]<<24);
		ctx->ddiroffs=ctx->coff_hdr+20+112;
	}else
	{
		ctx->img_base=	 buf[ctx->coff_hdr+20+28]|
				(buf[ctx->coff_hdr+20+29]<<8)|
				(buf[ctx->coff_hdr+20+30]<<16)|
				(buf[ctx->coff_hdr+20+31]<<24);

		ctx->img_size=	 buf[ctx->coff_hdr+20+56]|
				(buf[ctx->coff_hdr+20+57]<<8)|
				(buf[ctx->coff_hdr+20+58]<<16)|
				(buf[ctx->coff_hdr+20+59]<<24);

		ctx->nddir=	 buf[ctx->coff_hdr+20+92]|
				(buf[ctx->coff_hdr+20+93]<<8)|
				(buf[ctx->coff_hdr+20+94]<<16)|
				(buf[ctx->coff_hdr+20+95]<<24);
		ctx->ddiroffs=ctx->coff_hdr+20+96;
	}

	for(i=0; i<ctx->nddir; i++)
	{
		j=ctx->ddiroffs+(i*8);

		k=buf[j+0]|(buf[j+1]<<8)|(buf[j+2]<<16)|(buf[j+3]<<24);
		l=buf[j+4]|(buf[j+5]<<8)|(buf[j+6]<<16)|(buf[j+7]<<24);

		ctx->ddir_rva[i]=k;
		ctx->ddir_sz[i]=l;
	}

	printf("\tDisplacement 0x%08X%08X\n",
		(u32)(ctx->img_base>>32), (u32)ctx->img_base);

//	printf("\tSecs 0x%X %d\n", secoffs, nsecs);

//	printf("\nSections:\n");
	for(i=0; i<ctx->nsecs; i++)
	{
		j=ctx->secoffs+(i*40);

		s=(char *)(buf+j);
		l=buf[j+8]|(buf[j+9]<<8)|(buf[j+10]<<16)|(buf[j+11]<<24);
		k=buf[j+12]|(buf[j+13]<<8)|(buf[j+14]<<16)|(buf[j+15]<<24);

		ctx->sec_name[i]=basm_strdup(s);

		ctx->sec_rva_off[i]=k;
		ctx->sec_rva_size[i]=l;

		l=buf[j+16]|(buf[j+17]<<8)|(buf[j+18]<<16)|(buf[j+19]<<24);
		k=buf[j+20]|(buf[j+21]<<8)|(buf[j+22]<<16)|(buf[j+23]<<24);

		ctx->sec_buf_off[i]=k;
		ctx->sec_buf_size[i]=l;

		k=buf[j+36]|(buf[j+37]<<8)|(buf[j+38]<<16)|(buf[j+39]<<24);
		ctx->sec_flag[i]=k;

		printf("\t%d %s 0x%08X %d\n", i+1, s, k, l);

//		BLNK_AddExeSection(s, secva[i], secvs[i], 0);
	}

#if 0
	i=16;
	while(i<ctx->nsyms)i=i+(i>>1);
	ctx->sym_name=malloc(i*sizeof(char *));
	ctx->sym_addr=malloc(i*sizeof(void *));
	ctx->sym_rva=malloc(i*sizeof(int));

//	ctx->sym_nsym=0;
//	ctx->sym_msym=i;

//	syo=blnk_sym_nsym;

//	printf("\nSymbols:\n");
	for(i=0; i<nsyms; i++)
	{
		j=symoffs+(i*18);

		if(buf[j])
		{
			memset(nbuf, 0, 16);
			memcpy(nbuf, buf+j, 8);
			s=nbuf;
		}else
		{
			k=buf[j+4]|(buf[j+5]<<8)|(buf[j+6]<<16)|(buf[j+7]<<24);
			s=(char *)(buf+stroffs+k);
		}

		k=buf[j+8]|(buf[j+9]<<8)|(buf[j+10]<<16)|(buf[j+11]<<24);
		l=buf[j+12]|(buf[j+13]<<8);

		if((l>0) && (l<=nsecs))addr=ctx->sec_rva_off[l-1]+k;

//		if((*s=='.') || ((s[0]=='_') && (s[1]=='_')))
//		if((*s=='.') || ((s[0]=='_') && (s[1]=='_')) || (*s!='_'))
		if(*s!='_')
		{
			i+=buf[j+17];
			continue;
		}

//		printf("\t%s 0x%08X %d\n", s, k, l);

		ctx->sym_name=basm_strdup(s);
		ctx->sym_rva=addr;

//		BLNK_AddSym(s+1, (void *)addr);

		i+=buf[j+17];
	}
#endif

	ctx->img_addr=BASM_PE_AllocImageSpace(
		ctx->img_base, ctx->img_size);

	for(i=0; i<ctx->nsecs; i++)
	{
//		ctx->sec_name[i]=basm_strdup(s);
//		ctx->sec_rva_off[i]=k;
//		ctx->sec_rva_size[i]=l;
//		ctx->sec_buf_off[i]=k;
//		ctx->sec_buf_size[i]=l;
//		ctx->sec_flag[i]=k;

		memset(ctx->img_addr+ctx->sec_rva_off[i], 
			0, ctx->sec_rva_size[i]);

		memcpy(ctx->img_addr+ctx->sec_rva_off[i], 
			ctx->buf+ctx->sec_buf_off[i],
			ctx->sec_buf_size[i]);
	}

	if(ctx->img_addr!=((byte *)ctx->img_base))
	{
		disp=ctx->img_addr-((byte *)ctx->img_base);

		printf("Image Rebase %p -> %p, %ld bytes\n",
			((byte *)ctx->img_base), ctx->img_addr, disp);

		a=BASM_PE_LookupImageSectionBase(ctx, ".reloc");
		b=BASM_PE_LookupImageSectionEnd(ctx, ".reloc");

		cs=a; ce=b;
		while(cs<ce)
		{
			i=cs[0]+(cs[1]<<8)+(cs[2]<<16)+(cs[3]<<24);
			j=cs[4]+(cs[5]<<8)+(cs[6]<<16)+(cs[7]<<24);

			ct=ctx->img_addr+i;
			ce2=cs+j;

			cs+=8;
			while(cs<ce2)
			{
				i=cs[0]+(cs[1]<<8);
				j=(i>>12)&15; i&=4095;
				k=cs[2]+(cs[3]<<8);
				k&=4095;

				switch(j)
				{
				case 0: break;
				case 1:
					k=ct[i+0]+(ct[i+1]<<8);
					k+=disp>>16;
					ct[i+0]=k&255;
					ct[i+1]=(k>>8)&255;
					break;
				case 2:
					k=ct[i+0]+(ct[i+1]<<8);
					k+=disp;
					ct[i+0]=k&255;
					ct[i+1]=(k>>8)&255;
					break;
				case 3:
					k=ct[i+0]+(ct[i+1]<<8)+
						(ct[i+2]<<16)+(ct[i+3]<<24);
					k+=disp;
					ct[i+0]=k&255;
					ct[i+1]=(k>>8)&255;
					ct[i+2]=(k>>16)&255;
					ct[i+3]=(k>>24)&255;
					break;

				case 4:
					l=ct[k+0]+(ct[k+1]<<8)+
						(ct[i+0]<<16)+(ct[i+1]<<24);
					l+=disp;
					ct[k+0]=l&255;
					ct[k+1]=(l>>8)&255;
					ct[i+0]=(l>>16)&255;
					ct[i+1]=(l>>24)&255;
					break;

				case 5: case 6: case 7: break;
				case 8: break;
				case 9: break;

				case 10:
					addr=	( (s64)ct[i+0])|
						(((s64)ct[i+1])<<8)|
						(((s64)ct[i+2])<<16)|
						(((s64)ct[i+3])<<24)|
						(((s64)ct[i+4])<<32)|
						(((s64)ct[i+5])<<40)|
						(((s64)ct[i+6])<<48)|
						(((s64)ct[i+7])<<56);
					addr+=disp;

					ct[i+0]=addr&255;
					ct[i+1]=(addr>>8)&255;
					ct[i+2]=(addr>>16)&255;
					ct[i+3]=(addr>>24)&255;
					ct[i+4]=(addr>>32)&255;
					ct[i+5]=(addr>>40)&255;
					ct[i+6]=(addr>>48)&255;
					ct[i+7]=(addr>>56)&255;
					break;

				default:
					break;
				}

				cs+=2;
			}
		}
	}

#if 1
	//Import Directory Table
	cs=ctx->img_addr+ctx->ddir_rva[1];
	while(1)
	{
		i=cs[12]+(cs[13]<<8)+(cs[14]<<16)+(cs[15]<<24);
		if(!i)break;

		s=ctx->img_addr+i;	//DLL Name
		ctx2=BASM_PE_LoadImage(s);
		if(!ctx2)
		{
			printf("BASM_PE_LoadImageBuf: "
				"Import Failed for: '%s'\n", s);
			cs+=20;
			continue;
		}

		i=cs[16]+(cs[17]<<8)+(cs[18]<<16)+(cs[19]<<24);
		if(!i)break;

		cs2=ctx->img_addr+i;	//Import Address Table

		while(cs2)
		{
			if(ctx->mach==0x8664)
			{
				addr=	 ((s64)cs2[0])     +
					(((s64)cs2[1])<< 8)+
					(((s64)cs2[2])<<16)+
					(((s64)cs2[3])<<24)+
					(((s64)cs2[4])<<32)+
					(((s64)cs2[5])<<40)+
					(((s64)cs2[6])<<48)+
					(((s64)cs2[7])<<56);
				if(!addr)break;

				if((addr>>63)&1)
				{
					p=BASM_PE_LookupImageExportOrdinal(
						ctx2, (int)(addr&65535));
					addr=(s64)p;
				}else
				{
					s=ctx->img_addr+(addr&0x7FFFFFFF);
					s+=2;

					p=BASM_PE_LookupImageExport(
						ctx2, s);
					addr=(s64)p;
				}

				cs2[0]=addr&255;
				cs2[1]=(addr>>8)&255;
				cs2[2]=(addr>>16)&255;
				cs2[3]=(addr>>24)&255;
				cs2[4]=(addr>>32)&255;
				cs2[5]=(addr>>40)&255;
				cs2[6]=(addr>>48)&255;
				cs2[7]=(addr>>56)&255;
				cs2+=8;
			}else
			{
				addr=cs2[0]+(cs2[1]<<8)+
					(cs2[2]<<16)+(cs2[3]<<24);
				if(!addr)break;

				if((addr>>31)&1)
				{
					p=BASM_PE_LookupImageExportOrdinal(
						ctx2, (int)(addr&65535));
					addr=(u32)p;
				}else
				{
					s=ctx->img_addr+(addr&0x7FFFFFFF);
					s+=2;

					p=BASM_PE_LookupImageExport(
						ctx2, s);
					addr=(u32)p;
				}

				cs2[0]=addr&255;
				cs2[1]=(addr>>8)&255;
				cs2[2]=(addr>>16)&255;
				cs2[3]=(addr>>24)&255;

				cs2+=4;
			}
		}

		cs+=20;
	}
#endif

	return(ctx);
}

BASM_PE_Info *BASM_PE_LoadImage(char *name)
{
	BASM_PE_Info *ctx;
	char *nname;
	byte *buf;
	FILE *fd;
	int sz;

	BLNK_InitLink();

	nname=BASM_PE_NormalizeName(name);

	ctx=BASM_PE_LookupImage(nname);
	if(ctx)return(ctx);

	printf("BASM_PE_LoadImage '%s'\n", name);

//	printf("open %s\n", name);
	fd=fopen(name, "rb");

	if(!fd)
	{
		sprintf(nbuf, "%s.dll", name);
		fd=fopen(nbuf, "rb");
	}

	if(!fd)
	{
		sprintf(nbuf, "%s.exe", name);
		fd=fopen(nbuf, "rb");
	}

	if(!fd)
	{
		printf("BASM_PE_LoadImage: fail open\n");
		return(NULL);
	}

	fseek(fd, 0, 2);
	sz=ftell(fd);
	fseek(fd, 0, 0);

	buf=(byte *)malloc(sz);
	fread(buf, 1, sz, fd);
	fclose(fd);

	printf("PE Image %d (0x%X) bytes\n", sz, sz);

	ctx=BASM_PE_LoadImageBuf(nname, buf, sz);

	return(ctx);
}

