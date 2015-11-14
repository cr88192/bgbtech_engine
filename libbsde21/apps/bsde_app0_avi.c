#include <windows.h>

#include <GL/gl.h>

#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include "lbxgl_vid_avi.h"

FILE *main_avi_fd=NULL;

int write_int32(FILE *fd, int v)
{
	fputc(v&0xFF, fd);
	fputc((v>>8)&0xFF, fd);
	fputc((v>>16)&0xFF, fd);
	fputc((v>>24)&0xFF, fd);
}

int main_avi_opennew(char *name, int xs, int ys, int fps)
{
	FILE *fd;

	fd=fopen(name, "wb");
	if(!fd)return(-1);

	main_avi_fd=fd;

	write_int32(fd, RIFF_TAG_RIFF);
	write_int32(fd, 0);
	write_int32(fd, RIFF_TAG_AVI);

	write_int32(fd, RIFF_TAG_LIST);
	write_int32(fd, 0);
	write_int32(fd, RIFF_TAG_hdrl);


	write_int32(fd, RIFF_TAG_avih);
	write_int32(fd, 56);

	write_int32(fd, 1000000/fps);
	write_int32(fd, xs*ys*3*fps);
	write_int32(fd, 0);	//padding
	write_int32(fd, 0);	//flags

	write_int32(fd, 0);	//total frames
	write_int32(fd, 0);	//initial frames
	write_int32(fd, 1);	//streams

	write_int32(fd, xy*ys*4);	//suggest buffer
	write_int32(fd, xs);	//width
	write_int32(fd, ys);	//height

	write_int32(fd, 0);	//reserved
	write_int32(fd, 0);	//reserved
	write_int32(fd, 0);	//reserved
	write_int32(fd, 0);	//reserved


	write_int32(fd, RIFF_TAG_LIST);
	write_int32(fd, 0);
	write_int32(fd, RIFF_TAG_strl);


	write_int32(fd, RIFF_TAG_strh);
	write_int32(fd, 56);
	write_int32(fd, RIFF_TAG_vids);
	write_int32(fd, 0);	//handler

	write_int32(fd, 0);	//flags
	write_int32(fd, 0);	//priority
	write_int32(fd, 0);	//initial frames

	write_int32(fd, fps);	//time scale
	write_int32(fd, xs*ys*3*fps);	//data rate
	write_int32(fd, 0);	//start time

	write_int32(fd, 0);	//data length
	write_int32(fd, xy*ys*4);	//suggest buffer
	write_int32(fd, 10000);	//data length
}