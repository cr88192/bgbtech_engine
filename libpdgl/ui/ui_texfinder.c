/*
Texture Finding Dialog.
*/

#include <pdgl.h>
#include <pdglui.h>

#ifdef WIN32
#include <windows.h>
#endif

#ifndef MSVC
#include <sys/types.h>
#include <sys/stat.h>
#include <dirent.h>
#include <unistd.h>
#endif

#include <time.h>

// #define stricmp strcasecmp

// extern int pdglui_state->mx, pdglui_state->my, pdglui_state->mb;
// extern int pdgl_lmx, pdgl_lmy, pdgl_lmb;
// extern unsigned short *pdglui_state->keys;

extern PDGLUI_InputState *pdglui_state;

char *texfinder_title;
char *texfinder_path=NULL;
char *texfinder_fnames[4096];
int texfinder_n_fnames;
int texfinder_index;
int texfinder_selindex;
char *texfinder_name;

char *texfinder_basepath=NULL;

char texfinder_namebuf[64];
int texfinder_namepos;

PDGLUI_API char *UI_TexFinder_GenAbs(char *tn)
{
	char buf[256];

	if(texfinder_path && (*texfinder_path))
	{
		if(tn)
		{
			sprintf(buf, "%s/textures/%s/%s",
				texfinder_basepath, texfinder_path, tn);
		}else
		{
			sprintf(buf, "%s/textures/%s",
				texfinder_basepath, texfinder_path);
		}

		return(gcrstrdup(buf));
	}else
	{
		if(tn)
		{
			sprintf(buf, "%s/textures/%s",
				texfinder_basepath, tn);
		}else
		{
			sprintf(buf, "%s/textures",
				texfinder_basepath);
		}

		return(gcrstrdup(buf));
	}
}

void UI_TexFinder_ReloadPath()
{
	char buf[256];
	VFDIR *dir;
	char *s, *t, *name;
	int i, j, k, l;

//	strcpy(buf, texfinder_path);
//	strcat(buf, "/");
//	dir=opendir(buf);

//	dir=vfopendir(texfinder_path);

	s=UI_TexFinder_GenAbs(NULL);
	dir=vfopendir(s);

	if(!dir)
	{
		printf("UI_TexFinder_ReloadPath: Fail %s\n", s);
	
		for(i=0; i<texfinder_n_fnames; i++)
			free(texfinder_fnames[i]);
		texfinder_n_fnames=0;
		texfinder_index=0;
		return;
	}

	texfinder_n_fnames=0;
	texfinder_index=0;

	while(1)
	{
		name=vfreaddir(dir);
		if(!name)break;

//		sprintf(buf, "%s/%s", texfinder_path, name);
		strcpy(buf, UI_TexFinder_GenAbs(name));

		i=vfstatflags(buf);
//		if(i<0)continue;
		if(i<0)i=0;

		if(i&VF_IFREG)
		{
			s=vfgetext(name);
			t="bmp;tga;png;pcx;jpg";

			if(s && !vfcmpext(s, t))
				continue;

//			if(stricmp(s, "bmp") && stricmp(s, "tga") &&
//				stricmp(s, "png") && stricmp(s, "pcx") &&
//				stricmp(s, "jpg"))
//					continue;
		}

		s=name+strlen(name);
		while((s>name) && (*s!='.'))s--;
		if(*s=='.')
		{
			if(!strcmp(s, ".txt"))continue;
			if(!strcmp(s, ".pdn"))continue;
		
			while((s>name) && (*s!='_'))s--;
			t=buf;
			while(*s && (*s!='.'))*t++=*s++;
			*t++=0;
			
			if(!strcmp(buf, "_add"))continue;
			if(!strcmp(buf, "_luma"))continue;
			if(!strcmp(buf, "_spec"))continue;
			if(!strcmp(buf, "_gloss"))continue;
			if(!strcmp(buf, "_bump"))continue;
			if(!strcmp(buf, "_norm"))continue;

			if(!strcmp(buf, "_link"))continue;
		}

		for(i=0; i<texfinder_n_fnames; i++)
			if(strcmp(texfinder_fnames[i], name)>0)
				break;
		for(j=texfinder_n_fnames; j>i; j--)
			texfinder_fnames[j]=texfinder_fnames[j-1];
//		texfinder_fnames[i]=strdup(name);
		texfinder_fnames[i]=dystrdup(name);
		texfinder_n_fnames++;
	}

	vfclosedir(dir);
}

//#ifndef MSVC
#if 0
void UI_TexFinder_ReloadPath()
{
	struct stat st;
	char buf[256];
	DIR *dir;
	struct dirent *de;
	char *s;
	int i, j, k, l;

//	strcpy(buf, texfinder_path);
//	strcat(buf, "/");
//	dir=opendir(buf);

	s=UI_TexFinder_GenAbs(NULL);
	dir=opendir(s);

	printf("Load path %s\n", s);

//	dir=opendir(texfinder_path);

	if(!dir)
	{
		printf("Fail Load path %s\n", s);

		for(i=0; i<texfinder_n_fnames; i++)
			free(texfinder_fnames[i]);
		texfinder_n_fnames=0;
		texfinder_index=0;
		return;
	}

	texfinder_n_fnames=0;
	texfinder_index=0;

	while(1)
	{
		de=readdir(dir);
		if(!de)break;

//		sprintf(buf, "resource/textures/%s/%s",
//			texfinder_path, de->d_name);

		strcpy(buf, UI_TexFinder_GenAbs(de->d_name));
		i=stat(buf, &st);
		if(i<0)continue;

		if(S_ISREG(st.st_mode))
		{
			s=vfgetext(de->d_name);

			if(stricmp(s, "bmp") && stricmp(s, "tga") &&
				stricmp(s, "png") && stricmp(s, "pcx") &&
				stricmp(s, "jpg"))
					continue;
		}

		for(i=0; i<texfinder_n_fnames; i++)
			if(stricmp(texfinder_fnames[i], de->d_name)>0)
				break;
		for(j=texfinder_n_fnames; j>i; j--)
			texfinder_fnames[j]=texfinder_fnames[j-1];
		texfinder_fnames[i]=strdup(de->d_name);
		texfinder_n_fnames++;
	}

	closedir(dir);
}
#endif

PDGLUI_API void UI_TexFinder_SetTitle(char *name)
{
	char buf[256];

	texfinder_title=dystrdup(name);
	texfinder_name=NULL;

	if(!texfinder_path)
	{
		texfinder_path=dystrdup("");

//		getcwd(buf, 255);
//		texfinder_basepath=dystrdup(buf);

		texfinder_basepath="";
	}

	UI_TexFinder_ReloadPath();
}

PDGLUI_API char *UI_TexFinder_GetName()
{
	return(texfinder_name);
}

void UI_TexFinder_HandleName(char *name)
{
	char buf[256];
	char *s;
	int i;

	if(!strcmp(name, "."))return;
	if(!strcmp(name, ".."))
	{
		s=texfinder_path+strlen(texfinder_path);
		while((s>texfinder_path) && (*s!='/') && (*s!='\\'))
			s--;
		if(s>texfinder_path)
		{
			if(*(s-1)==':')*s++='\\';
			*s=0;
		}else *s=0;

		UI_TexFinder_ReloadPath();
		return;
	}

	strcpy(buf, UI_TexFinder_GenAbs(name));
	i=vfstatflags(buf);
	if(i<0)
	{
		printf("Fail State '%s'\n", buf);

		if(*texfinder_path)
			sprintf(buf, "%s/%s", texfinder_path, name);
			else strcpy(buf, name);

		texfinder_name=strdup(buf);
		return;
	}

	if(i&VF_IFDIR)
	{
		sprintf(buf, "%s/%s", texfinder_path, name);
		texfinder_path=dystrdup(buf);

		UI_TexFinder_ReloadPath();
		return;
	}

	if(*texfinder_path)
		sprintf(buf, "%s/%s", texfinder_path, name);
		else strcpy(buf, name);
	s=buf+strlen(buf);
	while((s>buf) && (*s!='.') && (*s!='/'))s--;
	if(*s=='.')*s=0;

	texfinder_name=strdup(buf);
	while(*texfinder_name=='/')texfinder_name++;
	return;
}

//#ifndef MSVC
#if 0
void UI_TexFinder_HandleName(char *name)
{
	struct stat st;
	char buf[256];
	char *s;
	int i;

	if(!strcmp(name, "."))return;
	if(!strcmp(name, ".."))
	{
		s=texfinder_path+strlen(texfinder_path);
		while((s>texfinder_path) && (*s!='/') && (*s!='\\'))
			s--;
		if(s>texfinder_path)
		{
			if(*(s-1)==':')*s++='\\';
			*s=0;
		}else *s=0;

//		chdir(texfinder_path);
//		getcwd(buf, 255);
//		texfinder_path=dystrdup(buf);

		UI_TexFinder_ReloadPath();
		return;
	}

//	sprintf(buf, "resource/textures/%s/%s", texfinder_path, name);
	strcpy(buf, UI_TexFinder_GenAbs(name));
	i=stat(buf, &st);
	if(i<0)
	{
		texfinder_name=strdup(buf);
		return;
	}

	if(S_ISDIR(st.st_mode))
	{
//		chdir(buf);
//		getcwd(buf, 255);
//		texfinder_path=dystrdup(buf);

		sprintf(buf, "%s/%s", texfinder_path, name);
		texfinder_path=dystrdup(buf);

		UI_TexFinder_ReloadPath();
		return;
	}


	if(*texfinder_path)
		sprintf(buf, "%s/%s", texfinder_path, name);
		else strcpy(buf, name);
	s=buf+strlen(buf);
	while((s>buf) && (*s!='.') && (*s!='/'))s--;
	if(*s=='.')*s=0;

	texfinder_name=strdup(buf);
	while(*texfinder_name=='/')texfinder_name++;
	return;
}
#endif

PDGLUI_API void UI_TexFinder_Draw()
{
	char buf[256];
	unsigned short *kcur;
	char *s, *t;
	int i, j, w, h;

//	pdglDisable(GL_TEXTURE_2D);
	pdglDisableTexture2D();

	pdglBegin(PDGL_QUADS);

	pdglColor4f(0, 0, 1, 1);
	pdglVertex2f(-400, 300-16);
	pdglVertex2f(-400, 300-24);
	pdglVertex2f(200, 300-24);
	pdglVertex2f(200, 300-16);

	pdglColor4f(0.5, 0.5, 0.5, 1);
	pdglVertex2f(-400, 300-24);
	pdglVertex2f(-400, -200);
	pdglVertex2f(200, -200);
	pdglVertex2f(200, 300-24);

	pdglColor4f(1, 1, 1, 1);
	pdglVertex2f(-400, 300-32);
	pdglVertex2f(-400, 300-40);
	pdglVertex2f(200, 300-40);
	pdglVertex2f(200, 300-32);

	pdglEnd();

	GfxFont_DrawString(texfinder_title, -400, 300-24,
		8, 8,  255, 255, 255, 255);
	GfxFont_DrawString(texfinder_path, -400, 300-32,
		8, 8,  0, 0, 0, 255);

	GfxFont_DrawString(texfinder_namebuf, -400, 300-40,
		8, 8,  0, 0, 0, 255);

	i=(clock()*1000)/CLOCKS_PER_SEC;
	i=(i/250)&1;
	j=texfinder_namepos;
	if(i)GfxFont_DrawChar('|',
		-400+(j*8), 300-40, 8, 8,
		127, 127, 127, 255);


	for(i=0; i<56; i++)
	{
		j=texfinder_index+i;
		if(j>=texfinder_n_fnames)
			break;

		s=texfinder_fnames[texfinder_index+i];
		GfxFont_DrawString(s, -400+32, 300-48-i*8,
			8, 8,  0, (j==texfinder_selindex)?255:0, 0, 255);
	}

	s=vfgetext(texfinder_namebuf);
	t="bmp;tga;png;pcx;jpg";

//	if(s && (!stricmp(s, "bmp") ||
//		!stricmp(s, "tga") ||
//		!stricmp(s, "png") ||
//		!stricmp(s, "pcx") ||
//		!stricmp(s, "jpg")
//		))
	if(s && vfcmpext(s, t))
	{
//		sprintf(buf, "resource/textures/%s", brush_sel->tex[i]);

		strcpy(buf, UI_TexFinder_GenAbs(texfinder_namebuf));
		j=Tex_LoadFile(buf, &w, &h);

		pdglColor4f(1, 1, 1, 1);
		pdglEnableTexture2D();
		pdglBindTexture(GL_TEXTURE_2D, j);

		if(w==h)
		{
			pdglBegin(PDGL_POLYGON);
			pdglTexCoord2f(0, 0);
			pdglVertex2f(200-24*8, -200);
			pdglTexCoord2f(1, 0);
			pdglVertex2f(200, -200);
			pdglTexCoord2f(1, 1);
			pdglVertex2f(200, -200+24*8);
			pdglTexCoord2f(0, 1);
			pdglVertex2f(200-24*8, -200+24*8);
			pdglEnd();
		}else if(w>h)
		{
			j=(24*8*h)/w;
			pdglBegin(PDGL_POLYGON);
			pdglTexCoord2f(0, 0);
			pdglVertex2f(200-24*8, -200);
			pdglTexCoord2f(1, 0);
			pdglVertex2f(200, -200);
			pdglTexCoord2f(1, 1);
			pdglVertex2f(200, -200+j);
			pdglTexCoord2f(0, 1);
			pdglVertex2f(200-24*8, -200+j);
			pdglEnd();
		}else
		{
			j=(24*8*w)/h;
			pdglBegin(PDGL_POLYGON);
			pdglTexCoord2f(0, 0);
			pdglVertex2f(200-j, -200);
			pdglTexCoord2f(1, 0);
			pdglVertex2f(200, -200);
			pdglTexCoord2f(1, 1);
			pdglVertex2f(200, -200+24*8);
			pdglTexCoord2f(0, 1);
			pdglVertex2f(200-j, -200+24*8);
			pdglEnd();
		}
	}

	if(pdglui_state->mb&1)
	{
		j=pdglui_state->my+300-40;
		if(j>0)i=j/8; else i=-1;
		if((i>=0) && (i<56))
		{
			j=texfinder_index+i;
			if((j>=0) && (j<texfinder_n_fnames))
				texfinder_selindex=j;
			strcpy(texfinder_namebuf,
				texfinder_fnames[texfinder_selindex]);
			texfinder_namepos=strlen(texfinder_namebuf);
		}
	}

	kcur=pdglui_state->keys;
	while(*kcur)
	{
		if(*kcur==K_ESCAPE)
		{
			texfinder_name="";
		}

		if(*kcur==K_ENTER)
		{
//			UI_TexFinder_HandleName(
//				texfinder_fnames[texfinder_selindex]);
			UI_TexFinder_HandleName(texfinder_namebuf);
		}

		if(*kcur==K_UPARROW)
		{
			j=texfinder_selindex-1;
			if((j>=0) && (j<texfinder_n_fnames))
				texfinder_selindex=j;
			strcpy(texfinder_namebuf,
				texfinder_fnames[texfinder_selindex]);
			texfinder_namepos=strlen(texfinder_namebuf);
		}
		if(*kcur==K_DOWNARROW)
		{
			j=texfinder_selindex+1;
			if((j>=0) && (j<texfinder_n_fnames))
				texfinder_selindex=j;
			strcpy(texfinder_namebuf,
				texfinder_fnames[texfinder_selindex]);
			texfinder_namepos=strlen(texfinder_namebuf);
		}

		if(*kcur==K_LEFTARROW)
		{
			texfinder_namepos--;
			if(texfinder_namepos<0)texfinder_namepos=0;
		}
		if(*kcur==K_RIGHTARROW)
		{
			texfinder_namepos++;
			i=strlen(texfinder_namebuf);
			if(texfinder_namepos>i)texfinder_namepos=i;
		}
		if((*kcur>=' ') && (*kcur<='~'))
		{
			for(i=63; i>texfinder_namepos; i--)
				texfinder_namebuf[i]=texfinder_namebuf[i-1];
			texfinder_namebuf[i]=*kcur;
			texfinder_namepos++;
		}
		if(*kcur==K_BACKSPACE)
		{
			if(texfinder_namepos>0)
			{
				for(i=texfinder_namepos-1; i<63; i++)
					texfinder_namebuf[i]=texfinder_namebuf[i+1];
				texfinder_namepos--;
			}
		}

		if(*kcur==K_PGDN)
		{
			texfinder_index+=25;
			if(texfinder_index<0)texfinder_index=0;
			if(texfinder_index>texfinder_n_fnames)
				texfinder_index=texfinder_n_fnames;
		}
		if(*kcur==K_PGUP)
		{
			texfinder_index-=25;
			if(texfinder_index<0)texfinder_index=0;
			if(texfinder_index>texfinder_n_fnames)
				texfinder_index=texfinder_n_fnames;
		}

		kcur++;
	}
}
