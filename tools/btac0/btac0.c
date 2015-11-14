#include <bgbtac.h>

int main(int argc, char *argv[])
{
	char tb[256];
	char *ifn, *ofn;

	BTAC_SampleHeader *head;
	byte *obuf;
	void *fd;
	short *buf;
	short *buf2;
	int ch, rt, bits, len, flags, cmd;
	double e, el, er;
	int ldb;
	int i, j, k, n;

	ifn=NULL; ofn=NULL; flags=0; cmd=0;
	for(i=1; i<argc; i++)
	{
		if(argv[i][0]=='-')
		{
			if(!strcmp(argv[i], "-o"))
			{
				ofn=argv[i+1];
				i++;
				continue;
			}

			if(!strcmp(argv[i], "--splitstereo"))
			{
				flags|=BGBTAC_EFL_HALFMONO;
				continue;
			}

			if(!strcmp(argv[i], "--centermono"))
			{
				flags|=BGBTAC_EFL_CENTERMONO;
				continue;
			}

			if(!strcmp(argv[i], "--encode"))
			{
				cmd=1;
				continue;
			}

			if(!strcmp(argv[i], "--decode"))
			{
				cmd=2;
				continue;
			}

			continue;
		}
		
		if(!ifn)
		{
			ifn=argv[i];
			continue;
		}
		
		printf("%s: unexpected argument %s\n", argv[0], argv[i]);
	}

	if(cmd==2)
	{
		if(!ofn)
		{
			printf("%s: no output file\n", argv[0]);
			return(-1);
		}
	
		head=BTAC_LoadSample(ifn);
		if(!head)
		{
			printf("%s: load failed '%s'\n", argv[0], ifn);
			return(-1);
		}
		
		ch=head->chan;
		rt=head->rate;
		len=head->length;
		flags=head->flags;

		obuf=head->data;

		buf2=malloc((len+256)*2*sizeof(short));
//		BGBTAC_DecodeStreamS16(obuf, buf2, len);
		BGBTAC_DecodeStreamS16(obuf, buf2, len, flags);
	
//		sprintf(tb, "%s_out.wav", argv[1]);
		BGBTAC_StoreWAV(ofn, (byte *)buf2, ch, rt, 16, len);
		
		free(buf2);
	}

	if(cmd==1)
	{
		if(!ofn)
		{
			printf("%s: no output file\n", argv[0]);
			return(-1);
		}
	
		buf=BGBTAC_LoadWAV16(ifn, &ch, &rt, &len);
		if(!buf)
		{
			printf("%s: load failed '%s'\n", argv[0], ifn);
			return(-1);
		}

		if(ch==2)
		{
			flags|=BGBTAC_EFL_STEREO;
		}

//		BGBTAC_LoadWAV(argv[1], )

		head=BTAC_CreateSample(ch, rt, 16, len, flags);

		obuf=head->data;

		BGBTAC_EncodeStreamS16(buf, obuf, len, flags);

//		sprintf(tb, "%s_dump0.wav", argv[1]);
//		BGBTAC_StoreWAV(tb, (byte *)buf, ch, rt, 16, len);
	
//		sprintf(tb, "%s_dump.btac", argv[1]);
		BTAC_StoreSample(ofn, head);
	}

	if(cmd==0)
	{
//		BGBTAC_LoadWAV(argv[1], )

//		sprintf(tb, "%s.wav", argv[1]);
		sprintf(tb, "%s.wav", ifn);
//		buf=BGBTAC_LoadWAV_44Mono16(tb, &len);
//		buf=BGBTAC_LoadWAV_44Stereo16(tb, &len);
//		buf=BGBTAC_LoadWAV_Stereo16(tb, &rt, &len);
//		flags|=BGBTAC_EFL_STEREO;

		buf=BGBTAC_LoadWAV16(tb, &ch, &rt, &len);
		if(!buf)
		{
			printf("%s: load failed '%s'\n", argv[0], ifn);
			return(-1);
		}

		if(ch==2)
		{
			flags|=BGBTAC_EFL_STEREO;
		}

//		BGBTAC_LoadWAV(argv[1], )

		head=BTAC_CreateSample(ch, rt, 16, len, flags);

#if 1
		n=(len+63)/64;
//		obuf=malloc(n*64+256);
		obuf=head->data;

//		flags=BGBTAC_EFL_STEREO;
//		flags=BGBTAC_EFL_STEREO|BGBTAC_EFL_HALFMONO;

//		BGBTAC_EncodeStreamS16(buf, obuf, len);
		BGBTAC_EncodeStreamS16(buf, obuf, len, flags);

		sprintf(tb, "%s_dump0.wav", ifn);
		BGBTAC_StoreWAV(tb, (byte *)buf, ch, rt, 16, len);
	
		sprintf(tb, "%s_dump.btac", ifn);
//		fd=bgbtac_fopen(tb, "wb");
//		bgbtac_fwrite(obuf, n, 32, fd);
//		bgbtac_fclose(fd);
	
		BTAC_StoreSample(tb, head);
	
		buf2=malloc((len+256)*2*sizeof(short));
//		BGBTAC_DecodeStreamS16(obuf, buf2, len);
		BGBTAC_DecodeStreamS16(obuf, buf2, len, flags);
	
		sprintf(tb, "%s_out.wav", ifn);
		BGBTAC_StoreWAV(tb, (byte *)buf2, ch, rt, 16, len);
#endif

#if 1
		if(flags&BGBTAC_EFL_STEREO)
		{
			e=0; el=0; er=0; ldb=-1;
			for(i=0; i<len; i++)
			{
				j=buf[i*2+0]-buf2[i*2+0];
				el+=j*j;

				j=buf[i*2+1]-buf2[i*2+1];
				er+=j*j;
		
				j=(buf[i*2+0]+buf[i*2+1])>>1;
				k=(buf2[i*2+0]+buf2[i*2+1])>>1;
		
				j=j-k;
				e+=j*j;

#if 0
				if(j>=8192)
				{						
					if(ldb!=(i/64))
					{
						printf("\n");
						
						ldb=i/64;

						k=obuf[(i/64)*32+0]&15;
						printf("%d:%d E=%d T=%d Bm,n=%d,%d\n",
							i/64, i%64, j, k,
							(s8)obuf[(i/64)*32+6], (s8)obuf[(i/64)*32+7]);


						j=(s16)(obuf[ldb*32+2] | (obuf[ldb*32+3]<<8));
						k=(s16)(obuf[ldb*32+4] | (obuf[ldb*32+5]<<8));
						printf("\tM=%d, N=%d\n", j, k);

#if 0
						for(j=0; j<64; j++)
						{
							k=(buf[(ldb*64+j)*2+0]+buf[(ldb*64+j)*2+1])>>1;
							printf("%04X ", k&65535);
						}
						printf("\n");
#endif

					}else
					{
						printf("%d:%d E=%d\n", i/64, i%64, j);
					}
				}
#endif
			}
			printf("RMSE: C=%f, L=%f, R=%f\n",
				sqrt(e/len),
				sqrt(el/len), sqrt(er/len));
		}else
		{
			e=0;
			for(i=0; i<len; i++)
			{
				j=buf[i]-buf2[i];
				e+=j*j;
			}
			printf("RMSE = %f\n", sqrt(e/len));
		}
#endif
	}

	return(0);
}
