#include <btanim2d.h>

PDGLUI_InputState *bta2d_state;
PDGLUI_Camera *bta2d_cam;

float *bta2d_draw_xy;
float *bta2d_draw_st;
float *bta2d_draw_rgba;
int *bta2d_draw_tex;

int bta2d_draw_ntris;
int bta2d_draw_mtris;

byte *bta2d_canvas_buf;
int bta2d_canvas_xs;
int bta2d_canvas_ys;
int bta2d_canvas_tex;
byte bta2d_canvas_dirty;
int bta2d_canvas_zoom;

BTA2D_EditTool *bta2d_tools[64];
int bta2d_n_tools;

BTA2D_EditLayer *bta2d_layers[64];
int bta2d_n_layers;

int bta2d_tool_id;
byte bta2d_tool_color1[4];
byte bta2d_tool_color2[4];

byte bta2d_color_palette[64*4];


BTA2D_API void BTA2D_Init()
{
	static int init=0;
	BTA2D_EditLayer *ltmp;
	int i;

	if(init)return;
	init=1;

	bta2d_state=PDGLUI_GetState();
	bta2d_cam=PDGLUI_GetCamera();
	
	i=256;
	bta2d_draw_xy=malloc(i*3*2*sizeof(float));
	bta2d_draw_st=malloc(i*3*2*sizeof(float));
	bta2d_draw_rgba=malloc(i*3*4*sizeof(float));
	bta2d_draw_tex=malloc(i*sizeof(int));
	bta2d_draw_ntris=0;
	bta2d_draw_mtris=i*3;
	
	bta2d_canvas_xs=256;
	bta2d_canvas_ys=256;
	bta2d_canvas_buf=malloc(bta2d_canvas_xs*bta2d_canvas_ys*4);
	memset(bta2d_canvas_buf, 255, bta2d_canvas_xs*bta2d_canvas_ys*4);
	bta2d_canvas_tex=Tex_LoadTexture2(
		bta2d_canvas_xs, bta2d_canvas_ys,
		bta2d_canvas_buf, 2, -1, 0);
	bta2d_canvas_dirty=1;
	bta2d_canvas_zoom=2;
	
	bta2d_n_layers=0;
	ltmp=BTA2D_AllocEditLayer(256, 256);
	bta2d_layers[bta2d_n_layers++]=ltmp;
	
	BTA2D_DefineTool(BTA2D_TOOL_SELECT, "anim2d/images/select");
	BTA2D_DefineTool(BTA2D_TOOL_MOVEPIXELS, "anim2d/images/movepixels");
	BTA2D_DefineTool(BTA2D_TOOL_MOVESELECT, "anim2d/images/moveselect");
	BTA2D_DefineTool(BTA2D_TOOL_PENCIL, "anim2d/images/pencil");
	BTA2D_DefineTool(BTA2D_TOOL_PAINTBRUSH, "anim2d/images/paintbrush");
	BTA2D_DefineTool(BTA2D_TOOL_BUCKET, "anim2d/images/bucket");
	BTA2D_DefineTool(BTA2D_TOOL_ERASER, "anim2d/images/eraser");
	
	bta2d_tool_color1[0]=255;
	bta2d_tool_color1[1]=255;
	bta2d_tool_color1[2]=255;
	bta2d_tool_color1[3]=255;

	bta2d_tool_color2[0]=0;
	bta2d_tool_color2[1]=0;
	bta2d_tool_color2[2]=0;
	bta2d_tool_color2[3]=255;
	
	for(i=0; i<16; i++)
	{
		bta2d_color_palette[i*4+0]=(i&1)?255:0;
		bta2d_color_palette[i*4+1]=(i&2)?255:0;
		bta2d_color_palette[i*4+2]=(i&4)?255:0;
		bta2d_color_palette[i*4+3]=255;
	}

	for(i=16; i<32; i++)
	{
		bta2d_color_palette[i*4+0]=(i&1)?128:0;
		bta2d_color_palette[i*4+1]=(i&2)?128:0;
		bta2d_color_palette[i*4+2]=(i&4)?128:0;
		bta2d_color_palette[i*4+3]=255;
	}

	for(i=32; i<48; i++)
	{
		bta2d_color_palette[i*4+0]=(i&1)?255:128;
		bta2d_color_palette[i*4+1]=(i&2)?255:128;
		bta2d_color_palette[i*4+2]=(i&4)?255:128;
		bta2d_color_palette[i*4+3]=255;
	}

	for(i=48; i<64; i++)
	{
		bta2d_color_palette[i*4+0]=(i&1)?128:64;
		bta2d_color_palette[i*4+1]=(i&2)?128:64;
		bta2d_color_palette[i*4+2]=(i&4)?128:64;
		bta2d_color_palette[i*4+3]=255;
	}
}

BTA2D_API int BTA2D_DefineTool(int id, char *img)
{
	int n;
	
	n=bta2d_n_tools++;
	bta2d_tools[n]=gcalloc(sizeof(BTA2D_EditTool));
	bta2d_tools[n]->id=id;
	bta2d_tools[n]->img=dystrdup(img);
	return(n);
}

BTA2D_API void BTA2D_BeginDrawing()
{
	bta2d_draw_ntris=0;
}

BTA2D_API void BTA2D_EndDrawing()
{
	int i, j, k;

	for(i=0; i<bta2d_draw_ntris; i++)
	{
		for(j=i+1; j<bta2d_draw_ntris; j++)
		{
			if(bta2d_draw_tex[j]!=bta2d_draw_tex[i])
				break;
		}

		glBindTexture(GL_TEXTURE_2D, bta2d_draw_tex[i]);
//		PDGL_DrawPrim_DrawArraysTexRGB(
//			GL_TRIANGLES, i*3, (j-i)*3,
//			2, GL_FLOAT, 0, bta2d_draw_xy+i*3*2,
//			2, GL_FLOAT, 0, bta2d_draw_st+i*3*2,
//			4, GL_FLOAT, 0, bta2d_draw_rgba+i*3*4);

		PDGL_DrawPrim_DrawArraysTexRGB(
			GL_TRIANGLES, i*3, (j-i)*3,
			2, GL_FLOAT, 0, bta2d_draw_xy,
			2, GL_FLOAT, 0, bta2d_draw_st,
			4, GL_FLOAT, 0, bta2d_draw_rgba);
		i=j-1;
	}
}

BTA2D_API void BTA2D_EmitTrianglefv(
	float *xy0, float *xy1, float *xy2,
	float *st0, float *st1, float *st2,
	float *rgba0, float *rgba1, float *rgba2,
	int tex)
{
	int i;
	
	i=bta2d_draw_ntris++;
	
	bta2d_draw_xy[(i*3+0)*2+0]=xy0[0];
	bta2d_draw_xy[(i*3+0)*2+1]=xy0[1];
	bta2d_draw_xy[(i*3+1)*2+0]=xy1[0];
	bta2d_draw_xy[(i*3+1)*2+1]=xy1[1];
	bta2d_draw_xy[(i*3+2)*2+0]=xy2[0];
	bta2d_draw_xy[(i*3+2)*2+1]=xy2[1];

	bta2d_draw_st[(i*3+0)*2+0]=st0[0];
	bta2d_draw_st[(i*3+0)*2+1]=st0[1];
	bta2d_draw_st[(i*3+1)*2+0]=st1[0];
	bta2d_draw_st[(i*3+1)*2+1]=st1[1];
	bta2d_draw_st[(i*3+2)*2+0]=st2[0];
	bta2d_draw_st[(i*3+2)*2+1]=st2[1];

	bta2d_draw_rgba[(i*3+0)*4+0]=rgba0[0];
	bta2d_draw_rgba[(i*3+0)*4+1]=rgba0[1];
	bta2d_draw_rgba[(i*3+0)*4+2]=rgba0[2];
	bta2d_draw_rgba[(i*3+0)*4+3]=rgba0[3];
	bta2d_draw_rgba[(i*3+1)*4+0]=rgba1[0];
	bta2d_draw_rgba[(i*3+1)*4+1]=rgba1[1];
	bta2d_draw_rgba[(i*3+1)*4+2]=rgba1[2];
	bta2d_draw_rgba[(i*3+1)*4+3]=rgba1[3];
	bta2d_draw_rgba[(i*3+2)*4+0]=rgba2[0];
	bta2d_draw_rgba[(i*3+2)*4+1]=rgba2[1];
	bta2d_draw_rgba[(i*3+2)*4+2]=rgba2[2];
	bta2d_draw_rgba[(i*3+2)*4+3]=rgba2[3];

	bta2d_draw_tex[i]=tex;
}

BTA2D_API void BTA2D_EmitQuadfv(
	float *xy0, float *xy1, float *xy2, float *xy3,
	float *st0, float *st1, float *st2, float *st3,
	float *rgba0, float *rgba1, float *rgba2, float *rgba3,
	int tex)
{
	BTA2D_EmitTrianglefv(xy0, xy1, xy2, st0, st1, st2,
		rgba0, rgba1, rgba2, tex);
	BTA2D_EmitTrianglefv(xy0, xy2, xy3, st0, st2, st3,
		rgba0, rgba2, rgba3, tex);
}

BTA2D_API void BTA2D_EmitBasicTexQuadf(
	float mx, float my, float nx, float ny, int tex)
{
	float pt[8];
	float st[8];
	float clr[4];
	
	pt[0]=mx; pt[1]=my; st[0]=0; st[1]=0;
	pt[2]=nx; pt[3]=my; st[2]=1; st[3]=0;
	pt[4]=nx; pt[5]=ny; st[4]=1; st[5]=1;
	pt[6]=mx; pt[7]=ny; st[6]=0; st[7]=1;
	clr[0]=1; clr[1]=1; clr[2]=1; clr[3]=1;
	
	BTA2D_EmitQuadfv(
		pt+0, pt+2, pt+4, pt+6,
		st+0, st+2, st+4, st+6,
		clr, clr, clr, clr,
		tex);
}

BTA2D_API void BTA2D_EmitBasic2TexQuadf(
	float ox, float oy, float sx, float sy, int tex)
{
	BTA2D_EmitBasicTexQuadf(ox, oy, ox+sx, oy+sy, tex);
}

BTA2D_API void BTA2D_EmitBasicTexClrQuadf(
	float mx, float my, float nx, float ny, int tex,
	float cr, float cg, float cb, float ca)
{
	float pt[8];
	float st[8];
	float clr[4];
	
	pt[0]=mx; pt[1]=my; st[0]=0; st[1]=0;
	pt[2]=nx; pt[3]=my; st[2]=1; st[3]=0;
	pt[4]=nx; pt[5]=ny; st[4]=1; st[5]=1;
	pt[6]=mx; pt[7]=ny; st[6]=0; st[7]=1;
	clr[0]=cr; clr[1]=cg; clr[2]=cb; clr[3]=ca;
	
	BTA2D_EmitQuadfv(
		pt+0, pt+2, pt+4, pt+6,
		st+0, st+2, st+4, st+6,
		clr, clr, clr, clr,
		tex);
}

BTA2D_API void BTA2D_EmitBasic2TexClrQuadf(
	float ox, float oy, float sx, float sy, int tex,
	float cr, float cg, float cb, float ca)
{
	BTA2D_EmitBasicTexClrQuadf(ox, oy, ox+sx, oy+sy, tex,
		cr, cg, cb, ca);
}

BTA2D_API BTA2D_EditLayer *BTA2D_AllocEditLayer(int xs, int ys)
{
	BTA2D_EditLayer *tmp;
	int i, n;
	
	tmp=gctalloc("bta2d_editlayer_t", sizeof(BTA2D_EditLayer));
	tmp->buf=gcatomic(xs*ys*4*sizeof(s16));
	tmp->xs=xs;
	tmp->ys=ys;
	
	n=xs*ys;
	for(i=0; i<n; i++)
	{
		tmp->buf[i*4+0]=4096;
		tmp->buf[i*4+1]=4096;
		tmp->buf[i*4+2]=4096;
		tmp->buf[i*4+3]=4096;
//		tmp->buf[i*4+3]=1024;
	}
	
	return(tmp);
}

BTA2D_API void BTA2D_RedrawCanvas()
{
	BTA2D_EditLayer *layer;
	int ra, ga, ba, aa;
	int rb, gb, bb, ab;
	int x, y;
	int i, j, k, l;
	
	for(i=0; i<bta2d_canvas_ys; i++)
		for(j=0; j<bta2d_canvas_xs; j++)
	{
		if((i^j)&1)
			{ k=192; }
		else { k=96; }

		if((i^j)&8)
			{ k+=32; }

		bta2d_canvas_buf[(i*bta2d_canvas_xs+j)*4+0]=k;
		bta2d_canvas_buf[(i*bta2d_canvas_xs+j)*4+1]=k;
		bta2d_canvas_buf[(i*bta2d_canvas_xs+j)*4+2]=k;
		bta2d_canvas_buf[(i*bta2d_canvas_xs+j)*4+3]=255;

		ra=k*16; ga=k*16; ba=k*16; aa=0;
		for(k=0; k<bta2d_n_layers; k++)
		{
			layer=bta2d_layers[k];
			x=j-layer->ox; y=i-layer->oy;
			if((x<0) || (y<0))continue;
			if((x>=layer->xs) || (y>=layer->ys))
				continue;
			
			rb=layer->buf[(y*layer->xs+x)*4+0];
			gb=layer->buf[(y*layer->xs+x)*4+1];
			bb=layer->buf[(y*layer->xs+x)*4+2];
			ab=layer->buf[(y*layer->xs+x)*4+3];
			if(ab<=0)continue;
			
			ra=(ra*(4096-ab)+(rb*ab)+2048)>>12;
			ga=(ga*(4096-ab)+(gb*ab)+2048)>>12;
			ba=(ba*(4096-ab)+(bb*ab)+2048)>>12;
			aa=(aa*(4096-ab)+(ab*ab)+2048)>>12;
		}

		ra=(ra+8)>>4;
		ga=(ga+8)>>4;
		ba=(ba+8)>>4;
		ra=(ra>0)?((ra<255)?ra:255):0;
		ga=(ga>0)?((ga<255)?ga:255):0;
		ba=(ba>0)?((ba<255)?ba:255):0;

		bta2d_canvas_buf[(i*bta2d_canvas_xs+j)*4+0]=ra;
		bta2d_canvas_buf[(i*bta2d_canvas_xs+j)*4+1]=ga;
		bta2d_canvas_buf[(i*bta2d_canvas_xs+j)*4+2]=ba;
		bta2d_canvas_buf[(i*bta2d_canvas_xs+j)*4+3]=255;
	}

#if 0
	for(i=0; i<bta2d_n_layers; i++)
	{
		layer=bta2d_layers[i];
		
		for(j=0; j<layer->ys; j++)
			for(k=0; k<layer->xs; k++)
		{
			l=layer->buf[(j*layer->xs+k)*4];
		}
	}
#endif
}

BTA2D_API void BTA2D_DrawEmitToolbar()
{
	char tb[256];
	BTA2D_EditTool *tool;
	float ox, oy, xs, ys;
	int i, j, k;

	ox=-(bta2d_state->xs)/2;
	oy=(bta2d_state->ys)/2;

	for(i=0; i<16; i++)
	{
		for(j=0; j<2; j++)
		{
			tool=bta2d_tools[i*2+j];
			if(!tool)continue;

			k=Tex_LoadFile(tool->img, NULL, NULL);	
			BTA2D_EmitBasic2TexQuadf(ox+j*32, oy-(i+1)*32, 32, 32, k);
			
			if(tool->id==bta2d_tool_id)
			{
				k=Tex_LoadFile("anim2d/images/toolselect", NULL, NULL);	
				BTA2D_EmitBasic2TexQuadf(ox+j*32, oy-(i+1)*32, 32, 32, k);
			}
		}
	}

	i=8; j=0;

	k=Tex_LoadFile("anim2d/images/white", NULL, NULL);	
	BTA2D_EmitBasic2TexClrQuadf(ox+j*32+16, oy-(i+1)*32-16, 32, 32, k,
		bta2d_tool_color2[0]/255.0, bta2d_tool_color2[1]/255.0,
		bta2d_tool_color2[2]/255.0, bta2d_tool_color2[3]/255.0);
	BTA2D_EmitBasic2TexClrQuadf(ox+j*32, oy-(i+1)*32, 32, 32, k,
		bta2d_tool_color1[0]/255.0, bta2d_tool_color1[1]/255.0,
		bta2d_tool_color1[2]/255.0, bta2d_tool_color1[3]/255.0);

	ox=-(bta2d_state->xs)/2;
	oy=0;

	for(i=0; i<16; i++)
	{
		for(j=0; j<4; j++)
		{
			BTA2D_EmitBasic2TexClrQuadf(ox+j*16, oy-(i+1)*16, 16, 16, k,
				bta2d_color_palette[(i*4+j)*4+0]/255.0,
				bta2d_color_palette[(i*4+j)*4+1]/255.0,
				bta2d_color_palette[(i*4+j)*4+2]/255.0,
				bta2d_color_palette[(i*4+j)*4+3]/255.0);
		}
	}
	
#if 0
	i=Tex_LoadFile("anim2d/images/select", NULL, NULL);	
	BTA2D_EmitBasic2TexQuadf(ox+ 0, oy-32, 32, 32, i);

	i=Tex_LoadFile("anim2d/images/movepixels", NULL, NULL);	
	BTA2D_EmitBasic2TexQuadf(ox+32, oy-32, 32, 32, i);

	i=Tex_LoadFile("anim2d/images/moveselect", NULL, NULL);	
	BTA2D_EmitBasic2TexQuadf(ox+ 0, oy-64, 32, 32, i);

	i=Tex_LoadFile("anim2d/images/pencil", NULL, NULL);	
	BTA2D_EmitBasic2TexQuadf(ox+ 0, oy-96, 32, 32, i);
	i=Tex_LoadFile("anim2d/images/paintbrush", NULL, NULL);	
	BTA2D_EmitBasic2TexQuadf(ox+32, oy-96, 32, 32, i);
	i=Tex_LoadFile("anim2d/images/eraser", NULL, NULL);	
	BTA2D_EmitBasic2TexQuadf(ox+ 0, oy-128, 32, 32, i);
	i=Tex_LoadFile("anim2d/images/bucket", NULL, NULL);	
	BTA2D_EmitBasic2TexQuadf(ox+32, oy-128, 32, 32, i);
#endif

	if(bta2d_canvas_dirty)
	{
		BTA2D_RedrawCanvas();
		bta2d_canvas_tex=Tex_LoadTexture2(
			bta2d_canvas_xs, bta2d_canvas_ys,
			bta2d_canvas_buf, 2, bta2d_canvas_tex, 0);
		bta2d_canvas_dirty=0;
	}

	ox=-(bta2d_canvas_xs/2)*bta2d_canvas_zoom;
	oy=-(bta2d_canvas_ys/2)*bta2d_canvas_zoom;
	xs=bta2d_canvas_xs*bta2d_canvas_zoom;
	ys=bta2d_canvas_ys*bta2d_canvas_zoom;
	BTA2D_EmitBasic2TexQuadf(
		ox, oy, xs, ys,
		bta2d_canvas_tex);
}

BTA2D_API void BTA2D_Draw2D()
{
	BTA2D_BeginDrawing();
	
	BTA2D_DrawEmitToolbar();

	BTA2D_EndDrawing();
}

BTA2D_API int BTA2D_HandleTool()
{
	int clr[4], clr2[4];
	BTA2D_EditLayer *layer;
	int mx, my;
	int x, y;
	int i, j, k, l;

	if(bta2d_canvas_zoom<=0)
		return(0);

	mx=bta2d_state->mx;
	my=-bta2d_state->my;
	
	mx/=bta2d_canvas_zoom;
	my/=bta2d_canvas_zoom;
	mx+=(bta2d_canvas_xs/2);
	my+=(bta2d_canvas_ys/2);
	
	if((mx<0) || (my<0))
		return(0);
	if((mx>=bta2d_canvas_xs) || (my>=bta2d_canvas_ys))
		return(0);
	
	layer=bta2d_layers[0];
	x=mx-layer->ox;
	y=my-layer->oy;

	if((x<0) || (y<0))
		return(0);
	if((x>=layer->xs) || (y>=layer->ys))
		return(0);
	
	switch(bta2d_tool_id)
	{
	case BTA2D_TOOL_PENCIL:
		if(bta2d_state->mb&1)
		{
			layer->buf[(y*layer->xs+x)*4+0]=bta2d_tool_color1[0]*16;
			layer->buf[(y*layer->xs+x)*4+1]=bta2d_tool_color1[1]*16;
			layer->buf[(y*layer->xs+x)*4+2]=bta2d_tool_color1[2]*16;
			layer->buf[(y*layer->xs+x)*4+3]=bta2d_tool_color1[3]*16;
			bta2d_canvas_dirty=1;
		}
		if(bta2d_state->mb&4)
		{
			layer->buf[(y*layer->xs+x)*4+0]=bta2d_tool_color2[0]*16;
			layer->buf[(y*layer->xs+x)*4+1]=bta2d_tool_color2[1]*16;
			layer->buf[(y*layer->xs+x)*4+2]=bta2d_tool_color2[2]*16;
			layer->buf[(y*layer->xs+x)*4+3]=bta2d_tool_color2[3]*16;
			bta2d_canvas_dirty=1;
		}
		break;
	case BTA2D_TOOL_PAINTBRUSH:
		if(bta2d_state->mb&1)
		{
			clr[0]=bta2d_tool_color1[0]*16;
			clr[1]=bta2d_tool_color1[1]*16;
			clr[2]=bta2d_tool_color1[2]*16;
			clr[3]=bta2d_tool_color1[3]*16;
		}else if(bta2d_state->mb&4)
		{
			clr[0]=bta2d_tool_color2[0]*16;
			clr[1]=bta2d_tool_color2[1]*16;
			clr[2]=bta2d_tool_color2[2]*16;
			clr[3]=bta2d_tool_color2[3]*16;
		}else
			{ break; }

		for(i=-4; i<=4; i++)
			for(j=-4; j<=4; j++)
		{
			k=i*i+j*j;
			if(k>=16)continue;
			
			if(((x+j)<0) || ((x+j)>=layer->xs))
				continue;
			if(((y+i)<0) || ((y+i)>=layer->ys))
				continue;

			k=sqrt(k);
			
			k=clr[3]/(k+1);
		
			clr2[0]=layer->buf[((y+i)*layer->xs+x+j)*4+0];
			clr2[1]=layer->buf[((y+i)*layer->xs+x+j)*4+1];
			clr2[2]=layer->buf[((y+i)*layer->xs+x+j)*4+2];
			clr2[3]=layer->buf[((y+i)*layer->xs+x+j)*4+3];

			clr2[0]=(clr2[0]*(4096-k)+(clr[0]*k)+2048)>>12;
			clr2[1]=(clr2[1]*(4096-k)+(clr[1]*k)+2048)>>12;
			clr2[2]=(clr2[2]*(4096-k)+(clr[2]*k)+2048)>>12;
			clr2[3]=(clr2[3]*(4096-k)+(clr[3]*k)+2048)>>12;

			layer->buf[((y+i)*layer->xs+x+j)*4+0]=clr2[0];
			layer->buf[((y+i)*layer->xs+x+j)*4+1]=clr2[1];
			layer->buf[((y+i)*layer->xs+x+j)*4+2]=clr2[2];
			layer->buf[((y+i)*layer->xs+x+j)*4+3]=clr2[3];
		}
		bta2d_canvas_dirty=1;
		break;
	case BTA2D_TOOL_ERASER:
		if(!(bta2d_state->mb&1))
			break;

		for(i=-4; i<=4; i++)
			for(j=-4; j<=4; j++)
		{
			k=i*i+j*j;
			if(k>=16)continue;
			
			if(((x+j)<0) || ((x+j)>=layer->xs))
				continue;
			if(((y+i)<0) || ((y+i)>=layer->ys))
				continue;
			
			k=sqrt(k);
			
			k=4096/(k+1);
		
			clr2[0]=layer->buf[((y+i)*layer->xs+x+j)*4+0];
			clr2[1]=layer->buf[((y+i)*layer->xs+x+j)*4+1];
			clr2[2]=layer->buf[((y+i)*layer->xs+x+j)*4+2];
			clr2[3]=layer->buf[((y+i)*layer->xs+x+j)*4+3];

			clr2[0]=(clr2[0]*(4096-k)+(0*k)+2048)>>12;
			clr2[1]=(clr2[1]*(4096-k)+(0*k)+2048)>>12;
			clr2[2]=(clr2[2]*(4096-k)+(0*k)+2048)>>12;
			clr2[3]=(clr2[3]*(4096-k)+(0*k)+2048)>>12;

			layer->buf[((y+i)*layer->xs+x+j)*4+0]=clr2[0];
			layer->buf[((y+i)*layer->xs+x+j)*4+1]=clr2[1];
			layer->buf[((y+i)*layer->xs+x+j)*4+2]=clr2[2];
			layer->buf[((y+i)*layer->xs+x+j)*4+3]=clr2[3];
		}
		bta2d_canvas_dirty=1;
		break;
	}
	
	return(0);
}

BTA2D_API int BTA2D_HandleInput()
{
	BTA2D_EditTool *tool;
	float ox, oy, xs, ys;
	int mx, my;
	int i, j, k;

	ox=-(bta2d_state->xs)/2;
	oy=(bta2d_state->ys)/2;

	mx=bta2d_state->mx;
	my=-bta2d_state->my;

	if((bta2d_state->mb&1) && !(bta2d_state->lmb&1))
	{
//		printf("Click %d %d %d\n",
//			mx, my, bta2d_state->mb);
	}

	for(i=0; i<16; i++)
	{
		for(j=0; j<2; j++)
		{
			tool=bta2d_tools[i*2+j];
			if(!tool)continue;

			if(	(mx>=(ox+j*32+ 0)) &&
				(mx<=(ox+j*32+32)) &&
				(my>=(oy-(i+1)*32+ 0)) &&
				(my<=(oy-(i+1)*32+32)))
			{
				if(bta2d_state->mb&1)
				{
					bta2d_tool_id=tool->id;
//					printf("Tool Select %d\n", bta2d_tool_id);
				}
			}

//			k=Tex_LoadFile(tool->img, NULL, NULL);	
//			BTA2D_EmitBasic2TexQuadf(ox+j*32, oy-(i+1)*32, 32, 32, k);
			
//			if(tool->id==bta2d_tool_id)
//			{
//				k=Tex_LoadFile("anim2d/images/toolselect", NULL, NULL);	
//				BTA2D_EmitBasic2TexQuadf(ox+j*32, oy-(i+1)*32, 32, 32, k);
//			}
		}
	}

	ox=-(bta2d_state->xs)/2;
	oy=0;

	if((mx>=ox) && (mx<=(ox+4*16)) &&
		(my<=oy) && (my>=(oy-16*16)))
	{
		j=(mx-ox)/16;
		i=(oy-my)/16;
		
		if((bta2d_state->mb&1) && !(bta2d_state->lmb&1))
		{
			bta2d_tool_color1[0]=bta2d_color_palette[(i*4+j)*4+0];
			bta2d_tool_color1[1]=bta2d_color_palette[(i*4+j)*4+1];
			bta2d_tool_color1[2]=bta2d_color_palette[(i*4+j)*4+2];
			bta2d_tool_color1[3]=bta2d_color_palette[(i*4+j)*4+3];
		}

		if((bta2d_state->mb&4) && !(bta2d_state->lmb&4))
		{
			bta2d_tool_color2[0]=bta2d_color_palette[(i*4+j)*4+0];
			bta2d_tool_color2[1]=bta2d_color_palette[(i*4+j)*4+1];
			bta2d_tool_color2[2]=bta2d_color_palette[(i*4+j)*4+2];
			bta2d_tool_color2[3]=bta2d_color_palette[(i*4+j)*4+3];
		}
	}

	BTA2D_HandleTool();

	return(0);
}
