#include <lbxgl2.h>

void BSP_ReadPad(VFILE *fd, int i)
{
	byte buf[1024];

	while(i>=1024)
	{
		vfread(buf, 1, 1024, fd);
		i-=1024;
	}
	if(i)vfread(buf, 1, i, fd);
}

void BSP_WritePad(VFILE *fd, int i)
{
	byte buf[1024];

	memset(buf, 0, 1024);
	while(i>=1024)
	{
		vfwrite(buf, 1, 1024, fd);
		i-=1024;
	}
	if(i)vfwrite(buf, 1, i, fd);
}


int BSP_ReadInt16(VFILE *fd)
{
	short i;
	i=vfgetc(fd);
	i|=vfgetc(fd)<<8;
	return(i);
}

void BSP_WriteInt16(VFILE *fd, int i)
{
	vfputc(i&0xFF, fd); vfputc((i>>8)&0xFF, fd);
}


int BSP_ReadInt32(VFILE *fd)
{
	int i;
	i=vfgetc(fd);
	i|=vfgetc(fd)<<8;
	i|=vfgetc(fd)<<16;
	i|=vfgetc(fd)<<24;
	return(i);
}

void BSP_WriteInt32(VFILE *fd, int i)
{
	vfputc(i&0xFF, fd); vfputc((i>>8)&0xFF, fd);
	vfputc((i>>16)&0xFF, fd); vfputc((i>>24)&0xFF, fd);
}


int BSP_ReadFourcc(VFILE *fd)
{
	int i;
	i=vfgetc(fd);
	i|=vfgetc(fd)<<8;
	i|=vfgetc(fd)<<16;
	i|=vfgetc(fd)<<24;
	return(i);
}

void BSP_WriteFourcc(VFILE *fd, int i)
{
	vfputc(i&0xFF, fd); vfputc((i>>8)&0xFF, fd);
	vfputc((i>>16)&0xFF, fd); vfputc((i>>24)&0xFF, fd);
}


float BSP_ReadFixed16(VFILE *fd, int s)
{
	short i;

	i=vfgetc(fd);
	i|=vfgetc(fd)<<8;

	return(((float)i)/s);
}

void BSP_WriteFixed16(VFILE *fd, float f, int s)
{
	int i;

	i=f*s;
	vfputc(i&0xFF, fd); vfputc((i>>8)&0xFF, fd);
}

float BSP_ReadFixed(VFILE *fd, int s)
{
	int i;

	i=vfgetc(fd);
	i|=vfgetc(fd)<<8;
	i|=vfgetc(fd)<<16;
	i|=vfgetc(fd)<<24;

	return(((float)i)/s);
}

void BSP_WriteFixed(VFILE *fd, float f, int s)
{
	int i;

	i=f*s;
	vfputc(i&0xFF, fd); vfputc((i>>8)&0xFF, fd);
	vfputc((i>>16)&0xFF, fd); vfputc((i>>24)&0xFF, fd);
}


float BSP_ReadHFloat(VFILE *fd)
{
	int i, e;
	float f;

	i=BSP_ReadInt16(fd)&0xFFFF;
	e=(i>>10)&31;
	if(e<15)
	{
		f=((i&1023)/1024.0)/(float)(1<<(15-e));
	}else
	{
		f=((i&1023)/1024.0)*(float)(1<<(e-15));
	}
	if(i&0x8000)f=-f;

	return(f);
}

void BSP_WriteHFloat(VFILE *fd, float f)
{
	int i, e, s;

	//underflow to 0
	if((f==0) || (fabs(f)<0.00003049))
	{
		BSP_WriteInt16(fd, 0);
		return;
	}

	if(f<0)
	{
		f=-f;
		s=0x8000;
	}else s=0;

	//overflow to max value (aka: +/- INF)
	if(f>=65472)
	{
		BSP_WriteInt16(fd, s|0x7FFF);
		return;
	}

	if(f<1)
	{
		e=15;
		while(((f*2)<=0.999457) && (e>0))
		{
			f*=2;
			e--;
		}

		i=s|(e<<10)|((int)(f*1024+0.5));
		BSP_WriteInt16(fd, i);
		return;
	}

	e=15;
	while((f>0.999457) && (e<31))
	{
		f*=0.5;
		e++;
	}
	if(f>0.999457)f=0.999457;
	i=s|(e<<10)|((int)(f*1024+0.5));
	BSP_WriteInt16(fd, i);
}

void BSP_ReadVector(VFILE *fd, float *v)
{
	v[0]=BSP_ReadFixed(fd, 65536);
	v[1]=BSP_ReadFixed(fd, 65536);
	v[2]=BSP_ReadFixed(fd, 65536);
}

void BSP_WriteVector(VFILE *fd, float *v)
{
	BSP_WriteFixed(fd, v[0], 65536);
	BSP_WriteFixed(fd, v[1], 65536);
	BSP_WriteFixed(fd, v[2], 65536);
}


void BSP_ReadVectorI(VFILE *fd, float *v)
{
	v[0]=BSP_ReadInt16(fd);
	v[1]=BSP_ReadInt16(fd);
	v[2]=BSP_ReadInt16(fd);
}

void BSP_WriteVectorFloor(VFILE *fd, float *v)
{
	BSP_WriteInt16(fd, (int)v[0]);
	BSP_WriteInt16(fd, (int)v[1]);
	BSP_WriteInt16(fd, (int)v[2]);
}

void BSP_WriteVectorCeil(VFILE *fd, float *v)
{
	BSP_WriteInt16(fd, (int)ceil(v[0]));
	BSP_WriteInt16(fd, (int)ceil(v[1]));
	BSP_WriteInt16(fd, (int)ceil(v[2]));
}

void BSP_WriteVectorRound(VFILE *fd, float *v)
{
	BSP_WriteInt16(fd, (int)(v[0]+0.5));
	BSP_WriteInt16(fd, (int)(v[1]+0.5));
	BSP_WriteInt16(fd, (int)(v[2]+0.5));
}


void BSP_ReadVector2_14(VFILE *fd, float *v)
{
	v[0]=BSP_ReadFixed16(fd, 16384);
	v[1]=BSP_ReadFixed16(fd, 16384);
	v[2]=BSP_ReadFixed16(fd, 16384);
}

void BSP_WriteVector2_14(VFILE *fd, float *v)
{
	BSP_WriteFixed16(fd, v[0], 16384);
	BSP_WriteFixed16(fd, v[1], 16384);
	BSP_WriteFixed16(fd, v[2], 16384);
}


void BSP_ReadVector2D(VFILE *fd, float *v)
{
	v[0]=BSP_ReadFixed(fd, 65536);
	v[1]=BSP_ReadFixed(fd, 65536);
}

void BSP_WriteVector2D(VFILE *fd, float *v)
{
	BSP_WriteFixed(fd, v[0], 65536);
	BSP_WriteFixed(fd, v[1], 65536);
}


void BSP_ReadNormal(VFILE *fd, float *v)
{
	v[0]=BSP_ReadFixed(fd, 16777216);
	v[1]=BSP_ReadFixed(fd, 16777216);
	v[2]=BSP_ReadFixed(fd, 16777216);
	v[3]=BSP_ReadFixed(fd, 65536);
}

void BSP_WriteNormal(VFILE *fd, float *v)
{
	BSP_WriteFixed(fd, v[0], 16777216);
	BSP_WriteFixed(fd, v[1], 16777216);
	BSP_WriteFixed(fd, v[2], 16777216);
	BSP_WriteFixed(fd, v[3], 65536);
}


void BSP_WriteImg(VFILE *fd, byte *buf, int xs, int ys)
{
	int i, j, k;
	int li, lj;

	li=0; lj=0;
	for(i=0; i<ys; i++)
		for(j=0; j<xs; j++)
	{
		if(i && j)
		{
			k=(buf[i*xs+j-1]+buf[(i-1)*xs+j]-
				buf[(i-1)*xs+j-1])&0xFF;
			k=(buf[i*xs+j]-k)&0xFF;
		}else k=buf[i*xs+j];

		if((k!=li) || (li && (lj>65534)))
		{
			if(!li)
			{
				if(lj>=65536)
				{
					vfputc(0x41, fd);
					vfputc(lj&0xFF, fd);
					vfputc((lj>>8)&0xFF, fd);
					vfputc((lj>>16)&0xFF, fd);
				}else
				if(lj>=96)
				{
					vfputc(0x40, fd);
					vfputc(lj&0xFF, fd);
					vfputc((lj>>8)&0xFF, fd);
				}else if(lj>1)
				{
					vfputc(0x40+lj, fd);
				}else if(lj)vfputc(li, fd);
			}else
			{
				if(lj && ((lj>1) || ((li>=0x40) && (li<0xC0))))
				{
					if(lj>=32)
					{
						vfputc(0xA0, fd);
						vfputc(lj&0xFF, fd);
						vfputc((lj>>8)&0xFF, fd);
					}else vfputc(0xA0+lj, fd);
				}
				if(lj)vfputc(li, fd);
			}
			li=k; lj=1;
		}else lj++;
	}

	if(!li)
	{
		if(lj>=65536)
		{
			vfputc(0x41, fd);
			vfputc(lj&0xFF, fd);
			vfputc((lj>>8)&0xFF, fd);
			vfputc((lj>>16)&0xFF, fd);
		}else
		if(lj>=96)
		{
			vfputc(0x40, fd);
			vfputc(lj&0xFF, fd);
			vfputc((lj>>8)&0xFF, fd);
		}else if(lj>1)
		{
			vfputc(0x40+lj, fd);
		}else if(lj)vfputc(li, fd);
	}else
	{
		if(lj && ((lj>1) || ((li>=0x40) && (li<0xC0))))
		{
			if(lj>=32)
			{
				vfputc(0xA0, fd);
				vfputc(lj&0xFF, fd);
				vfputc((lj>>8)&0xFF, fd);
			}else vfputc(0xA0+lj, fd);
		}
		if(lj)vfputc(li, fd);
	}
}


void BSP_ReadRLE(VFILE *fd, byte *buf, int sz)
{
	byte *ct, *ce;
	int i, j, k, l;

//	vfread(buf, 1, sz, fd);
//	return;

	ct=buf;
	ce=buf+sz;
	l=0;
	while(ct<ce)
	{
		i=vfgetc(fd);
		if(i>=0xC0)
		{
			if(i==0xC0)
			{
				l=vfgetc(fd);
				*ct++=l;
				continue;
			}
			if(i==0xC1)
			{
				i=vfgetc(fd);
				if(i>=0x80)
				{
					i=((i&0x7F)<<8)|vfgetc(fd);
					i+=192;
				}else i+=64;

				j=i;
				while(i--)*ct++=l;

				if(ct>ce)*(int *)-1=-1;

				continue;
			}

			i-=0xC0;
			j=i;
			while(i--)*ct++=l;

			if(ct>ce)*(int *)-1=-1;

			continue;
		}

		l=i-0x60;
		*ct++=l;
	}
}

void BSP_UnfiltBuf(byte *buf, int xs, int ys)
{
	int i, j, k;

	for(i=0; i<ys; i++)
		for(j=0; j<xs; j++)
	{
		if(i && j)
		{
			k=buf[(i-1)*xs+j]+buf[i*xs+(j-1)]-buf[(i-1)*xs+(j-1)];
			buf[i*xs+j]+=k;
		}else if(i)
		{
			buf[i*xs+j]+=buf[(i-1)*xs+j];
		}else if(j)
		{
			buf[i*xs+j]+=buf[i*xs+(j-1)];
		}
	}
}

void BSP_ReadImg(VFILE *fd, byte *buf, int xs, int ys)
{
	byte *tbuf;
	int i, j, k, l, y, u, v;

	tbuf=malloc(xs*ys*4);
	BSP_ReadRLE(fd, tbuf, xs*ys*3);

	i=xs*ys;
	BSP_UnfiltBuf(tbuf+i*0, xs, ys);
	BSP_UnfiltBuf(tbuf+i*1, xs, ys);
	BSP_UnfiltBuf(tbuf+i*2, xs, ys);

	l=xs*ys;
	for(i=0; i<ys; i++)
		for(j=0; j<xs; j++)
	{
		k=i*xs+j;

		y=tbuf[k];
		u=tbuf[l+k];
		v=tbuf[2*l+k];

		buf[k*4+0]=v+y;
		buf[k*4+1]=y;
		buf[k*4+2]=u+y;
		buf[k*4+3]=255;
	}

	free(tbuf);
}

void BSP_ReadImgMono(VFILE *fd, byte *buf, int xs, int ys)
{
	BSP_ReadRLE(fd, buf, xs*ys);
	BSP_UnfiltBuf(buf, xs, ys);
}

#if 0
void BSP_ReadImg(VFILE *fd, byte *buf, int xs, int ys)
{
	int i, j, k;
	int li, lj;

	vfread(buf, 1, xs*ys*4, fd);
	return;

	li=0; lj=0;
	for(i=0; i<ys; i++)
		for(j=0; j<xs; j++)
	{
		if(!lj)
		{
			k=vfgetc(fd);
			if((k>0x41) && (k<0xA0))
			{
				li=0;
				lj=k-0x41;
			}else if((k>0xA0) && (k<0xC0))
			{
				li=vfgetc(fd);
				lj=k-0xA1;
			}else if(k==0x40)
			{
				li=0;
				lj=vfgetc(fd);
				lj|=vfgetc(fd)<<8;
			}else if(k==0x41)
			{
				li=0;
				lj=vfgetc(fd);
				lj|=vfgetc(fd)<<8;
				lj|=vfgetc(fd)<<16;
			}else if(k==0xA0)
			{
				lj=vfgetc(fd);
				lj|=vfgetc(fd)<<8;
				li=vfgetc(fd);
			}else li=k;
		}else lj--;

		if(i && j)
		{
			k=(buf[i*xs+j-1]+buf[(i-1)*xs+j]-
				buf[(i-1)*xs+j-1])&0xFF;
			buf[i*xs+j]=(li+k)&0xFF;
		}else buf[i*xs+j]=li;
	}
}
#endif
