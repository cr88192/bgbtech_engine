#include <bgbnet.h>

char **httpnode_mime_exts;
char **httpnode_mime_types;

char *httpnode_mime_inittypes[]=
{
	"htm",	"text/html",
	"html",	"text/html",
	"txt",	"text/plain",
	"gif",	"image/gif",
	"png",	"image/png",
	"jpg",	"image/jpeg",
	"jpeg",	"image/jpeg",
	NULL,	NULL
};

char httpnode_mimetable[64]=
	"ABCDEFGHIJKLMNOP"
	"QRSTUVWXYZabcdef"
	"ghijklmnopqrstuv"
	"wxyz0123456789+/";

/*--
Cat pdnet;Protocols;HTTP
Form
	int HttpNode_EncodeMime(byte *out, byte *iin, int sz);
Description
	Encodes some data as mime.
--*/
int HttpNode_EncodeMime(byte *o, byte *i, int sz)
{
	unsigned long v;
	int osz, j;

	osz=0;
	while(sz)
	{
		v=(i[0]<<16)+(i[1]<<8)+i[2];

		j=(sz*4)/3;
		if(j>0)o[0]=httpnode_mimetable[(v>>18)&0x3f];
		if(j>1)o[1]=httpnode_mimetable[(v>>12)&0x3f];
		if(j>2)o[2]=httpnode_mimetable[(v>>6)&0x3f];
		if(j>3)o[3]=httpnode_mimetable[v&0x3f];

		i+=3;
		o+=4;

		j=(sz<3)?sz:3;
		sz-=j;
		osz+=(j*4)/3;
	}

	return(osz);
}

static int mime_conv(int v)
{
	int i;
	for(i=0; i<64; i++)
		if(httpnode_mimetable[i]==v)
			return(i);
	return(0);
}

/*--
Cat pdnet;Protocols;HTTP
Form
	int HttpNode_DecodeMime(byte *out, byte *in, int sz);
Description
	Decodes some mime data.
--*/
int HttpNode_DecodeMime(byte *o, byte *i, int sz)
{
	unsigned long v;
	int osz, j;

	osz=0;
	while(sz)
	{
		v=mime_conv(i[0])<<18;
		if(sz>1)v+=mime_conv(i[1])<<12;
		if(sz>2)v+=mime_conv(i[2])<<6;
		if(sz>3)v+=mime_conv(i[3]);

		o[0]=(v>>16)&0xff;
		o[1]=(v>>8)&0xff;
		o[2]=v&0xff;

		i+=4;
		o+=3;

		j=(sz<4)?sz:4;
		sz-=j;
		osz+=(j*3)/4;
	}

	return(osz);
}

/*--
Cat pdnet;Protocols;HTTP
Form
	char *HttpNode_MimeFromExt(char *ext);
Description
	Fecthes the mime type associated with a given file extension.
--*/
char *HttpNode_MimeFromExt(char *ext)
{
	int i;

	gc_printf("mime from ext %s\n", ext);

	for(i=0; httpnode_mime_exts[i]; i++)
	{
		if(!strcmp(httpnode_mime_exts[i], ext))
			return(httpnode_mime_types[i]);
	}
	return("application/unknown");
}

/*--
Cat pdnet;Protocols;HTTP
Form
	char *HttpNode_MimeFromName(char *name);
Description
	Fetches the mime time for a given filename.
--*/
char *HttpNode_MimeFromName(char *name)
{
	char *s, *t;
	char buf[10];

	s=name+strlen(name);
	while((s>name) && (*s!='.'))s--;
	s++;

	t=buf;
	while(*s)*t++=*s++;
	*t++=0;

	s=HttpNode_MimeFromExt(buf);
	return(s);
}

/*--
Cat pdnet;Protocols;HTTP
Form
	int HttpNode_MimeInit();
Description
	Initialize the mime tables.
Status Internal
--*/
int HttpNode_MimeInit()
{
	FILE *fd;
	int i;
	char *s1, *s2;
	char buf1[256], buf2[64], buf3[64];

	httpnode_mime_exts=malloc(256*sizeof(char *));
	httpnode_mime_types=malloc(256*sizeof(char *));

	fd=fopen("mime.txt", "rt");
	if(!fd)
	{
		for(i=0; httpnode_mime_inittypes[i*2]; i++)
		{
			s1=httpnode_mime_inittypes[i*2];
			s2=httpnode_mime_inittypes[i*2+1];
			httpnode_mime_exts[i]=strdup(s1);
			httpnode_mime_types[i]=strdup(s2);
		}

		fd=fopen("mime.txt", "wt");

		for(i=0; httpnode_mime_inittypes[i*2]; i++)
		{
			s1=httpnode_mime_inittypes[i*2];
			s2=httpnode_mime_inittypes[i*2+1];
			fprintf(fd, "%s\t%s\n", s1, s2);
		}

		fclose(fd);
	}else
	{
		i=0;
		while(!feof(fd))
		{
			memset(buf1, 0, 256);
			fgets(buf1, 255, fd);
			if(!buf1[0] || (buf1[0]=='\n'))continue;

			sscanf(buf1, "%s %s", buf2, buf3);

			httpnode_mime_exts[i]=strdup(buf2);
			httpnode_mime_types[i]=strdup(buf3);
			i++;
		}
		httpnode_mime_exts[i]=NULL;
		httpnode_mime_types[i]=NULL;

		fclose(fd);
	}

	return(0);
}
