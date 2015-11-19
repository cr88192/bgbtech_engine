/*
Draws a load/save dialog.
*/

#include <pdgl.h>
#include <pdglui.h>

#ifdef WIN32
#include <windows.h>
#endif

#include <sys/types.h>
#include <sys/stat.h>

#ifndef MSVC
#include <dirent.h>
#include <unistd.h>
#endif

#include <time.h>

//extern int pdglui_state->mx, pdglui_state->my, pdglui_state->mb;
//extern int pdglui_lmx, pdglui_lmy, pdglui_lmb;
//extern unsigned short *pdglui_state->keys;

extern PDGLUI_InputState *pdglui_state;

char *loadsave_title, *loadsave_ext;
char *loadsave_path;
char *loadsave_fnames[4096];
int loadsave_n_fnames;
int loadsave_index;
int loadsave_selindex;
char *loadsave_name;
int loadsave_drives;

char loadsave_namebuf[256];
int loadsave_namepos;

PDGLUI_API void UI_LoadSave_CheckDrives()
{
#ifdef WIN32
	struct stat st;
	char buf[256];
	int i, j;

	for(i=0; i<26; i++)
	{
//		sprintf(buf, "%c:\\", 'a'+i);
//		j=stat(buf, &st);
//		if(j>=0)loadsave_drives|=1<<i;

		sprintf(buf, "%c:\\", 'a'+i);
		j=GetDriveType(buf);
		if(j>=2)loadsave_drives|=1<<i;
	}
	printf("drives %d\n", loadsave_drives);
#endif
}

PDGLUI_API void UI_LoadSave_ReloadPath()
{
	char buf[256];
	VFDIR *dir;
	char *s, *t, *name;
	int i, j, k, l;

//	strcpy(buf, loadsave_path);
//	strcat(buf, "/");
//	dir=opendir(buf);

	dir=vfopendir(loadsave_path);

	if(!dir)
	{
		Con_Printf("No opendir '%s'\n", loadsave_path);

//		for(i=0; i<loadsave_n_fnames; i++)
//			free(loadsave_fnames[i]);
		loadsave_n_fnames=0;
		loadsave_index=0;
		return;
	}

	loadsave_n_fnames=0;
	loadsave_index=0;

	while(1)
	{
		name=vfreaddir(dir);
		if(!name)break;

		sprintf(buf, "%s/%s", loadsave_path, name);
		i=vfstatflags(buf);
//		if(i<0)continue;
		if(i<0)i=0;

		if((i&VF_IFREG) && loadsave_ext)
		{
			s=vfgetext(name);
			t=loadsave_ext;

			if(s && !vfcmpext(s, loadsave_ext))
				continue;
		}

		for(i=0; i<loadsave_n_fnames; i++)
			if(strcmp(loadsave_fnames[i], name)>0)
				break;
		for(j=loadsave_n_fnames; j>i; j--)
			loadsave_fnames[j]=loadsave_fnames[j-1];
//		loadsave_fnames[i]=strdup(name);
		loadsave_fnames[i]=dystrdup(name);
		loadsave_n_fnames++;
	}

	if(!loadsave_n_fnames)
		Con_Printf("No files in '%s'\n", loadsave_path);

	vfclosedir(dir);
}

#if 0
PDGLUI_API void UI_LoadSave_ReloadPath()
{
	struct stat st;
	char buf[256];
	DIR *dir;
	struct dirent *de;
	char *s, *t;
	int i, j, k, l;

//	strcpy(buf, loadsave_path);
//	strcat(buf, "/");
//	dir=opendir(buf);

	dir=opendir(loadsave_path);

	if(!dir)
	{
//		for(i=0; i<loadsave_n_fnames; i++)
//			free(loadsave_fnames[i]);
		loadsave_n_fnames=0;
		loadsave_index=0;
		return;
	}

	loadsave_n_fnames=0;
	loadsave_index=0;

	while(1)
	{
		de=readdir(dir);
		if(!de)break;

		sprintf(buf, "%s/%s", loadsave_path, de->d_name);
		i=stat(buf, &st);
		if(i<0)continue;

		if(S_ISREG(st.st_mode) && loadsave_ext)
		{
			s=vfgetext(de->d_name);
			t=loadsave_ext;
//			if(s && stricmp(s, loadsave_ext))
//				continue;

			if(s && !vfcmpext(s, loadsave_ext))
				continue;
		}

		for(i=0; i<loadsave_n_fnames; i++)
//			if(stricmp(loadsave_fnames[i], de->d_name)>0)
			if(strcmp(loadsave_fnames[i], de->d_name)>0)
				break;
		for(j=loadsave_n_fnames; j>i; j--)
			loadsave_fnames[j]=loadsave_fnames[j-1];
		loadsave_fnames[i]=strdup(de->d_name);
		loadsave_n_fnames++;
	}

	closedir(dir);
}
#endif

PDGLUI_API void UI_LoadSave_SetTitle(char *name, char *ext)
{
	char buf[256];

	loadsave_title=dystrdup(name);
	loadsave_name=NULL;
	loadsave_ext=NULL;

	if(ext)loadsave_ext=dystrdup(ext);

	if(!loadsave_path)
	{
		getcwd(buf, 255);
		loadsave_path=dystrdup(buf);
	}

	UI_LoadSave_ReloadPath();
}

PDGLUI_API char *UI_LoadSave_GetName()
{
	return(loadsave_name);
}

void UI_LoadSave_HandleName(char *name)
{
//	struct stat st;
	char buf[256];
	char *s;
	int i;

	if(!strcmp(name, "."))return;
	if(!strcmp(name, ".."))
	{
		s=loadsave_path+strlen(loadsave_path);
		while((s>loadsave_path) && (*s!='/') && (*s!='\\'))
			s--;
		if(s>loadsave_path)
		{
			if(*(s-1)==':')*s++='\\';
			*s=0;
		}

//		chdir(loadsave_path);
//		getcwd(buf, 255);
//		loadsave_path=dystrdup(buf);

		UI_LoadSave_ReloadPath();
		return;
	}

	sprintf(buf, "%s/%s", loadsave_path, name);
//	i=stat(buf, &st);
	i=vfstatflags(buf);
	if(i<0)
	{
		loadsave_name=dystrdup(buf);
		return;
	}

//	if(S_ISDIR(st.st_mode))
	if(i&VF_IFDIR)
	{
		chdir(buf);
		getcwd(buf, 255);
		loadsave_path=dystrdup(buf);
		UI_LoadSave_ReloadPath();
		return;
	}

	loadsave_name=dystrdup(buf);
	return;
}

PDGLUI_API void UI_LoadSave_Draw()
{
	char buf[256];
	unsigned short *kcur;
	char *s;
	int i, j;

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

	GfxFont_DrawString(loadsave_title, -400, 300-24,
		8, 8,  255, 255, 255, 255);
	GfxFont_DrawString(loadsave_path, -400, 300-32,
		8, 8,  0, 0, 0, 255);

	GfxFont_DrawString(loadsave_namebuf, -400, 300-40,
		8, 8,  0, 0, 0, 255);

	i=(clock()*1000)/CLOCKS_PER_SEC;
	i=(i/250)&1;
	j=loadsave_namepos;
	if(i)GfxFont_DrawChar('|',
		-400+(j*8), 300-40, 8, 8,
		127, 127, 127, 255);


	for(i=0; i<26; i++)
	{
		if(!(loadsave_drives&(1<<i)))
			continue;

		s=loadsave_path;
		j=0;
		if(((*s)>='A') && ((*s)<='Z'))j=*s-'A';
		if(((*s)>='a') && ((*s)<='z'))j=*s-'a';

		sprintf(buf, "%c", 'A'+i);
		GfxFont_DrawString(buf, -400+i*16, -200,
			8, 8,  0, (i==j)?255:0, 0, 255);
	}

	for(i=0; i<56; i++)
	{
		j=loadsave_index+i;
		if(j>=loadsave_n_fnames)
			break;

		s=loadsave_fnames[loadsave_index+i];
		GfxFont_DrawString(s, -400+32, 300-48-i*8,
			8, 8,  0, (j==loadsave_selindex)?255:0, 0, 255);
	}

	if(pdglui_state->mb&1)
	{
		j=pdglui_state->my+300-40;
		if(j>0)i=j/8; else i=-1;
		if((i>=0) && (i<56))
		{
			j=loadsave_index+i;
			if((j>=0) && (j<loadsave_n_fnames))
			{
				loadsave_selindex=j;
				strcpy(loadsave_namebuf,
					loadsave_fnames[loadsave_selindex]);
				loadsave_namepos=strlen(loadsave_namebuf);
			}
		}

		if((pdglui_state->my>=(200-8)) && (pdglui_state->my<=200))
		{
			i=(pdglui_state->mx+400)/16;
			
			if(loadsave_drives&(1<<i))
			{
				sprintf(buf, "%c:\\", 'a'+i);
				loadsave_path=dystrdup(buf);
				UI_LoadSave_ReloadPath();
			}
		}
	}

	kcur=pdglui_state->keys;
	while(*kcur)
	{
		if(*kcur==K_ESCAPE)
		{
			loadsave_name="";
		}

		if(*kcur==K_ENTER)
		{
//			UI_LoadSave_HandleName(
//				loadsave_fnames[loadsave_selindex]);
			UI_LoadSave_HandleName(loadsave_namebuf);
		}

		if(*kcur==K_LEFTARROW)
		{
			loadsave_namepos--;
			if(loadsave_namepos<0)loadsave_namepos=0;
		}
		if(*kcur==K_RIGHTARROW)
		{
			loadsave_namepos++;
			i=strlen(loadsave_namebuf);
			if(loadsave_namepos>i)loadsave_namepos=i;
		}
		if((*kcur>=' ') && (*kcur<='~'))
		{
			for(i=63; i>loadsave_namepos; i--)
				loadsave_namebuf[i]=loadsave_namebuf[i-1];
			loadsave_namebuf[i]=*kcur;
			loadsave_namepos++;
		}
		if(*kcur==K_BACKSPACE)
		{
			if(loadsave_namepos>0)
			{
				for(i=loadsave_namepos-1; i<63; i++)
					loadsave_namebuf[i]=loadsave_namebuf[i+1];
				loadsave_namepos--;
			}
		}

		if(*kcur==K_PGDN)
		{
			loadsave_index+=25;
			if(loadsave_index<0)loadsave_index=0;
			if(loadsave_index>loadsave_n_fnames)
				loadsave_index=loadsave_n_fnames;
		}
		if(*kcur==K_PGUP)
		{
			loadsave_index-=25;
			if(loadsave_index<0)loadsave_index=0;
			if(loadsave_index>loadsave_n_fnames)
				loadsave_index=loadsave_n_fnames;
		}

		kcur++;
	}
}
