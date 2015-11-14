#include <bgbbtj.h>

byte *loadfile(char *name, int *rsz)
{
	byte *buf;
	VFILE *fd;
	int sz;
	
	fd=vffopen(name, "rb");
	if(!fd)return(NULL);
	
	vfseek(fd, 0, 2);
	sz=vftell(fd);

	buf=malloc(sz);
	vfseek(fd, 0, 0);
	vfread(buf, 1, sz, fd);
	
	vfclose(fd);
	if(rsz)*rsz=sz;

	return(buf);
}

int bc7_getmodeid(byte *cs)
{
	int i, j;
	
	j=*cs;
	if((j&1)==1)
		{ i=0; }
	else if((j&3)==2)
		{ i=1; }
	else if((j&7)==4)
		{ i=2; }
	else if((j&15)==8)
		{ i=3; }
	else if((j&31)==16)
		{ i=4; }
	else if((j&63)==32)
		{ i=5; }
	else if((j&127)==64)
		{ i=6; }
	else if((j&255)==128)
		{ i=7; }
	else { i=8; }

	return(i);
}

int main(int argc, char *argv)
{
	int mode_cnt[16];
	double mode_err[16];
	double mode_err_r[16];
	double mode_err_g[16];
	double mode_err_b[16];
	double mode_err_a[16];
	byte *buf_tga, *buf_bptc, *buf_dec;
	byte *cs;
	char *in_bptc;
	char *in_tga;
//	char *out_base;
	int xs, ys, xs1, ys1;
	int i1, j1, k1, l1;
	int i, j, k, l, n;
	
//	in_bptc="colors-128-128-RGB.avpcl";
//	in_tga="colors.tga";
//	in_tga="colors-avpcl.tga";

	in_bptc="BC7Ltest-152-152-RGBA.avpcl";
	in_tga="BC7Ltest.tga";
	
	buf_tga=BGBBTJ_TGA_LoadRaw(in_tga, &xs, &ys);
	buf_bptc=loadfile(in_bptc, NULL);
	
	bgbbtj_imgflip_single(buf_tga, xs, ys);
	buf_dec=malloc(xs*ys*4);
	
	BGBBTJ_BC7_DecodeImageBC7(buf_bptc, buf_dec, xs, ys, 4);
	
	imgstat(buf_tga, buf_dec, xs*ys);
	
	BGBBTJ_PNG_SaveRaw("colors_dump0.png", buf_dec, xs, ys);
	
	xs1=((xs+3)/4);
	ys1=((ys+3)/4);
	n=xs1*ys1;
		
	for(i=0; i<16; i++)
	{
		mode_cnt[i]=0;
		mode_err[i]=0;
		mode_err_r[i]=0;
		mode_err_g[i]=0;
		mode_err_b[i]=0;
		mode_err_a[i]=0;
	}

	for(i=0; i<ys1; i++)
		for(j=0; j<xs1; j++)
	{
		k=i*xs1+j;
		cs=buf_bptc+k*16;
		l=bc7_getmodeid(cs);
		mode_cnt[l]++;
		
		for(i1=0; i1<4; i1++)
			for(j1=0; j1<4; j1++)
		{
			k1=((i*4+i1)*xs+(j*4+j1))*4;
			l1=buf_tga[k1+0]-buf_dec[k1+0];
			mode_err[l]+=l1*l1;
			mode_err_r[l]+=l1*l1;
			l1=buf_tga[k1+1]-buf_dec[k1+1];
			mode_err[l]+=l1*l1;
			mode_err_g[l]+=l1*l1;
			l1=buf_tga[k1+2]-buf_dec[k1+2];
			mode_err[l]+=l1*l1;
			mode_err_b[l]+=l1*l1;
			l1=buf_tga[k1+3]-buf_dec[k1+3];
			mode_err[l]+=l1*l1;
			mode_err_a[l]+=l1*l1;
		}
	}

#if 0
	for(i=0; i<n; i++)
	{
		cs=buf_bptc+i*16;
		j=bc7_getmodeid(cs);
		mode_cnt[j]++;
	}
#endif

	for(i=0; i<16; i++)
	{
//		printf("%d ", mode_cnt[i]);
//		printf("%d:%d(%f) ", i, mode_cnt[i], sqrt(mode_err[i]/(4*n)));
		printf("%d: %d (%f)\n", i, mode_cnt[i], sqrt(mode_err[i]/(4*n)));
		if(mode_err[i]>0)
		{
			printf("\t%f\n", sqrt(mode_err_r[i]/n));
			printf("\t%f\n", sqrt(mode_err_g[i]/n));
			printf("\t%f\n", sqrt(mode_err_b[i]/n));
			printf("\t%f\n", sqrt(mode_err_a[i]/n));
		}
	}
	printf("\n");
	
	return(0);
}
