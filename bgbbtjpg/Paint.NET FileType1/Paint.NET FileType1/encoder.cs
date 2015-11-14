using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace BGBTechBTJ
{
    public partial class PDJPG
    {
        //Encoder


        public static void PDJPG_WriteString(PDJPG_Context ctx, string str)
        {
            //            char* s;
            int i;

            ctx.huff.FlushBits(ctx);
            //            s = str;
            //            while (*s)
            //                *ctx.huff.ct++ = *s++;
            for (i = 0; i < str.Length; i++)
                ctx.huff.ct.emit(str[i]);
            ctx.huff.ct.emit(0);
        }

#if false
public static int PDJPG_GetImgBlk(char *blk, int xo, int yo, BytePtr img, int xs, int ys)
{
	BytePtr cs;
	char *ct;
	int i, j, k;

	cs=img+(yo*xs+xo); ct=blk;
	memcpy8(ct, cs); ct+=8; cs+=xs;
	memcpy8(ct, cs); ct+=8; cs+=xs;
	memcpy8(ct, cs); ct+=8; cs+=xs;
	memcpy8(ct, cs); ct+=8; cs+=xs;
	memcpy8(ct, cs); ct+=8; cs+=xs;
	memcpy8(ct, cs); ct+=8; cs+=xs;
	memcpy8(ct, cs); ct+=8; cs+=xs;
	memcpy8(ct, cs); // ct+=8; cs+=xs;
	return(0);
}
#endif

#if true
        public static int PDJPG_GetImgBlk16(
            ShortPtr blk, int xo, int yo,
            ShortPtr img, int xs, int ys)
        {
            ShortPtr cs;
            ShortPtr ct;

            cs = img + (yo * xs + xo); ct = blk;
            ShortPtr.memcpy16(ct, cs); ct += 8; cs += xs;
            ShortPtr.memcpy16(ct, cs); ct += 8; cs += xs;
            ShortPtr.memcpy16(ct, cs); ct += 8; cs += xs;
            ShortPtr.memcpy16(ct, cs); ct += 8; cs += xs;
            ShortPtr.memcpy16(ct, cs); ct += 8; cs += xs;
            ShortPtr.memcpy16(ct, cs); ct += 8; cs += xs;
            ShortPtr.memcpy16(ct, cs); ct += 8; cs += xs;
            ShortPtr.memcpy16(ct, cs);
            return (0);
        }
#endif

        // int PDJPG_FilterImageDCT(BytePtr ibuf, short *obuf, int xs, int ys)
        public static int PDJPG_FilterImageDCT(ShortPtr ibuf, ShortPtr obuf,
            int xs, int ys, int dcbias)
        {
            //	static short tblk[DCTSZ2], tblk2[DCTSZ2];
            //	static char blk[DCTSZ2];
            //	short tblk[DCTSZ2], tblk2[DCTSZ2];
            //	char blk[DCTSZ2];
            ShortPtr blk = new ShortPtr(DCTSZ2);
            int i, j, k;

            k = 0;
            for (i = 0; i < (ys / DCTSZ); i++)
                for (j = 0; j < (xs / DCTSZ); j++)
                {
                    //		PDJPG_GetImgBlk(blk, j*DCTSZ, i*DCTSZ, ibuf, xs, ys);
                    PDJPG_GetImgBlk16(blk, j * DCTSZ, i * DCTSZ, ibuf, xs, ys);
                    PDJPG_TransDCT.TransBlock(blk, obuf + k * DCTSZ2, dcbias);
                    k++;
                }
            return (0);
        }

        //int PDJPG_FilterImageRDCT(BytePtr ibuf, short *obuf, int xs, int ys)
        public static int PDJPG_FilterImageRDCT(ShortPtr ibuf, ShortPtr obuf,
            int xs, int ys, int dcbias)
        {
            //	static short tblk[DCTSZ2], tblk2[DCTSZ2];
            //	static char blk[DCTSZ2];
            //	short tblk[DCTSZ2], tblk2[DCTSZ2];
            //	char blk[DCTSZ2];
            ShortPtr blk = new ShortPtr(DCTSZ2);
            int i, j, k;

            k = 0;
            for (i = 0; i < (ys / DCTSZ); i++)
                for (j = 0; j < (xs / DCTSZ); j++)
                {
                    //		PDJPG_GetImgBlk(blk, j*DCTSZ, i*DCTSZ, ibuf, xs, ys);
                    PDJPG_GetImgBlk16(blk, j * DCTSZ, i * DCTSZ, ibuf, xs, ys);
                    PDJPG_TransRDCT.TransBlock(blk, obuf + k * DCTSZ2, dcbias);
                    k++;
                }
            return (0);
        }

        //int PDJPG_FilterImageRDCT(BytePtr ibuf, short *obuf, int xs, int ys)
        public static int PDJPG_FilterImageAuDCT(PDJPG_Context ctx, ShortPtr ibuf, ShortPtr obuf, IntPtr tagbuf,
            int xs, int ys, int dcbias, int cn)
        {
            //	static short tblk[DCTSZ2], tblk2[DCTSZ2];
            //	static char blk[DCTSZ2];
            //	short tblk[DCTSZ2], tblk2[DCTSZ2];
            //	char blk[DCTSZ2];
            ShortPtr blk = new ShortPtr(DCTSZ2);
            int i, j, k, tag;

            k = 0;
            for (i = 0; i < (ys / DCTSZ); i++)
                for (j = 0; j < (xs / DCTSZ); j++)
                {
                    //		PDJPG_GetImgBlk(blk, j*DCTSZ, i*DCTSZ, ibuf, xs, ys);
                    PDJPG_GetImgBlk16(blk, j * DCTSZ, i * DCTSZ, ibuf, xs, ys);
                    PDJPG_TransAuDCT.TransBlock(ctx, blk, obuf + k * DCTSZ2, dcbias, (cn == 0) ? 0 : 1, out tag);
                    tagbuf[k] = tag;
                    k++;
                }
            return (0);
        }

#if true
        public static void PDJPG_GetImgMegaBlk16(
            ShortPtr blk, int xo, int yo,
            ShortPtr img, int xs, int ys)
        {
            int i, j;

            for (i = 0; i < 8; i++)
            {
                for (j = 0; j < 8; j++)
                {
                    PDJPG_GetImgBlk16(blk + ((i * 8 + j) * 64), xo + j * 8, yo + i * 8, img, xs, ys);
                }
            }
        }
#endif

#if true
        //int PDJPG_FilterImageRDCT(BytePtr ibuf, short *obuf, int xs, int ys)
        public static int PDJPG_FilterImageMegablockRDCT(ShortPtr ibuf, ShortPtr obuf,
            int xs, int ys, int dcbias)
        {
            //	static short tblk[DCTSZ2], tblk2[DCTSZ2];
            //	static char blk[DCTSZ2];
            //	short tblk[DCTSZ2], tblk2[DCTSZ2];
            //	char blk[DCTSZ2];
            ShortPtr blk = new ShortPtr(DCTSZ4);
            int i, j, k;

            k = 0;
            for (i = 0; i < (ys / DCTSZ2); i++)
                for (j = 0; j < (xs / DCTSZ2); j++)
                {
                    //		PDJPG_GetImgBlk(blk, j*DCTSZ, i*DCTSZ, ibuf, xs, ys);
                    PDJPG_GetImgMegaBlk16(blk, j * DCTSZ2, i * DCTSZ2, ibuf, xs, ys);
                    PDJPG_TransRDCT.TransMegaBlock(blk, obuf + k * DCTSZ4, dcbias);
                    k++;
                }
            return (0);
        }
#endif

        public static void PDJPG_EmitDQT(PDJPG_Context ctx, int n)
        {
            int i;

            if (ctx.megablock)
            {
                ctx.huff.ct.emit(0xFF);
                ctx.huff.ct.emit(JPG.JPG_DQT);

                i = 128 + 3;
                ctx.huff.ct.emit(i >> 8);
                ctx.huff.ct.emit(i & 0xFF);

                ctx.huff.ct.emit(n);
                for (i = 0; i < 64; i++)
                    ctx.huff.ct.emit(ctx.jpg_qt[n * 256 + pdjpg_zigzag2[i]]);
                for (i = 0; i < 64; i++)
                    ctx.huff.ct.emit(ctx.jpg_qt[n * 256 + 64 + pdjpg_zigzag2[i]]);
                return;
            }

            ctx.huff.ct.emit(0xFF);
            ctx.huff.ct.emit(JPG.JPG_DQT);

            i = 64 + 3;
            ctx.huff.ct.emit(i >> 8);
            ctx.huff.ct.emit(i & 0xFF);

            ctx.huff.ct.emit(n);
            for (i = 0; i < 64; i++)
                ctx.huff.ct.emit(ctx.jpg_qt[n * 64 + pdjpg_zigzag2[i]]);
        }

        public static void PDJPG_EmitSOF(PDJPG_Context ctx, int xs, int ys)
        {
            BytePtr p;
            int i;

            ctx.huff.ct.emit(0xFF);
            ctx.huff.ct.emit(JPG.JPG_SOF0);

            i = 8 + (ctx.jpg_mono ? 1 : 3) * 3;
            p = ctx.huff.ct;
            ctx.huff.ct.emit(i >> 8);	//Lf
            ctx.huff.ct.emit(i & 0xFF);

            ctx.huff.ct.emit(8); 	//P

            ctx.huff.ct.emit(ys >> 8);	//Y
            ctx.huff.ct.emit(ys & 0xFF);	//Y
            ctx.huff.ct.emit(xs >> 8);	//X
            ctx.huff.ct.emit(xs & 0xFF);	//X

            if (ctx.jpg_mono)
            {
                ctx.huff.ct.emit(1);	//Nf

                ctx.huff.ct.emit(1);	//Ci
                ctx.huff.ct.emit(0x11);	//Hi Vi
                ctx.huff.ct.emit(0);	//Tqi
            }
            else if (ctx.jpg_is420)
            {
                if (!ctx.jpg_yuvw)
                {
                    //YUV 4:2:0

                    ctx.huff.ct.emit(3);	//Nf

                    ctx.huff.ct.emit(1);	//Ci
                    ctx.huff.ct.emit(0x22);	//Hi Vi
                    ctx.huff.ct.emit(0);	//Tqi
                    ctx.huff.ct.emit(2);	//Ci
                    ctx.huff.ct.emit(0x11);	//Hi Vi
                    ctx.huff.ct.emit(1);	//Tqi
                    ctx.huff.ct.emit(3);	//Ci
                    ctx.huff.ct.emit(0x11);	//Hi Vi
                    ctx.huff.ct.emit(1);	//Tqi
                }
                else
                {
                    //YUVW 4:2:0

                    ctx.huff.ct.emit(4);	//Nf

                    ctx.huff.ct.emit(1);	//Ci
                    ctx.huff.ct.emit(0x22);	//Hi Vi
                    ctx.huff.ct.emit(0);	//Tqi
                    ctx.huff.ct.emit(2);	//Ci
                    ctx.huff.ct.emit(0x11);	//Hi Vi
                    ctx.huff.ct.emit(1);	//Tqi
                    ctx.huff.ct.emit(3);	//Ci
                    ctx.huff.ct.emit(0x11);	//Hi Vi
                    ctx.huff.ct.emit(1);	//Tqi
                    ctx.huff.ct.emit(4);	//Ci
                    ctx.huff.ct.emit(0x11);	//Hi Vi
                    ctx.huff.ct.emit(1);	//Tqi
                }
            }
            else
            {
                if (!ctx.jpg_yuvw)
                {
                    //YUV 4:4:4

                    ctx.huff.ct.emit(3);	//Nf

                    ctx.huff.ct.emit(1);	//Ci
                    ctx.huff.ct.emit(0x11);	//Hi Vi
                    ctx.huff.ct.emit(0);	//Tqi
                    ctx.huff.ct.emit(2);	//Ci
                    ctx.huff.ct.emit(0x11);	//Hi Vi
                    ctx.huff.ct.emit(1);	//Tqi
                    ctx.huff.ct.emit(3);	//Ci
                    ctx.huff.ct.emit(0x11);	//Hi Vi
                    ctx.huff.ct.emit(1);	//Tqi
                }
                else
                {
                    //YUVW 4:4:4

                    ctx.huff.ct.emit(4);	//Nf

                    ctx.huff.ct.emit(1);	//Ci
                    ctx.huff.ct.emit(0x11);	//Hi Vi
                    ctx.huff.ct.emit(0);	//Tqi
                    ctx.huff.ct.emit(2);	//Ci
                    ctx.huff.ct.emit(0x11);	//Hi Vi
                    ctx.huff.ct.emit(1);	//Tqi
                    ctx.huff.ct.emit(3);	//Ci
                    ctx.huff.ct.emit(0x11);	//Hi Vi
                    ctx.huff.ct.emit(1);	//Tqi
                    ctx.huff.ct.emit(4);	//Ci
                    ctx.huff.ct.emit(0x11);	//Hi Vi
                    ctx.huff.ct.emit(1);	//Tqi
                }
            }

            i = ctx.huff.ct - p;
            p[0] = i >> 8;	//Lf
            p[1] = i & 0xFF;
        }

        public static void PDJPG_EmitSOS(PDJPG_Context ctx)
        {
            BytePtr p;
            int i;

            ctx.huff.ct.emit(0xFF);
            ctx.huff.ct.emit(JPG.JPG_SOS);

            i = 6 + (ctx.jpg_mono ? 1 : 3) * 2;
            p = ctx.huff.ct;
            ctx.huff.ct.emit(i >> 8);	//Lf
            ctx.huff.ct.emit(i & 0xFF);

            if (ctx.jpg_yuvw)
            {
                ctx.huff.ct.emit(4); 	//Ns

                ctx.huff.ct.emit(1);	//Csi
                ctx.huff.ct.emit(0x00);	//Tdi Tai
                ctx.huff.ct.emit(2);	//Csi
                ctx.huff.ct.emit(0x11);	//Tdi Tai
                ctx.huff.ct.emit(3);	//Csi
                ctx.huff.ct.emit(0x11);	//Tdi Tai
                ctx.huff.ct.emit(4);	//Csi
                ctx.huff.ct.emit(0x11);	//Tdi Tai
            }
            else if (!ctx.jpg_mono)
            {
                ctx.huff.ct.emit(3); 	//Ns

                ctx.huff.ct.emit(1);	//Csi
                ctx.huff.ct.emit(0x00);	//Tdi Tai
                ctx.huff.ct.emit(2);	//Csi
                ctx.huff.ct.emit(0x11);	//Tdi Tai
                ctx.huff.ct.emit(3);	//Csi
                ctx.huff.ct.emit(0x11);	//Tdi Tai
            }
            else
            {
                ctx.huff.ct.emit(1); 	//Ns

                ctx.huff.ct.emit(1);	//Csi
                ctx.huff.ct.emit(0x00);	//Tdi Tai
            }

            ctx.huff.ct.emit(0); 	//Ss
            ctx.huff.ct.emit(63); 	//Se
            ctx.huff.ct.emit(0x00); 	//Ah Al

            i = ctx.huff.ct - p;
            p[0] = i >> 8;	//Lf
            p[1] = i & 0xFF;
        }


        static int[] pdjpg_ijg_qtab_y = new int[64] {
                16, 11, 10, 16,  24,  40,  51,  61,
                12, 12, 14, 19,  26,  58,  60,  55,
                14, 13, 16, 24,  40,  57,  69,  56,
                14, 17, 22, 29,  51,  87,  80,  62,
                18, 22, 37, 56,  68, 109, 103,  77,
                24, 35, 55, 64,  81, 104, 113,  92,
                49, 64, 78, 87, 103, 121, 120, 101,
                72, 92, 95, 98, 112, 100, 103,  99};

        static int[] pdjpg_ijg_qtab_uv = new int[64] {
               99, 99, 99, 99, 99, 99, 99, 99,
               99, 99, 99, 99, 99, 99, 99, 99,
               99, 99, 99, 99, 99, 99, 99, 99,
               99, 99, 99, 99, 99, 99, 99, 99,
               47, 66, 99, 99, 99, 99, 99, 99,
               24, 26, 56, 99, 99, 99, 99, 99,
               18, 21, 26, 66, 99, 99, 99, 99,
               17, 18, 24, 47, 99, 99, 99, 99};

        public static void PDJPG_MakeQuantTabFastIJG_Y(
            short[] inv, int cnt, BytePtr tab, double qf)
        {
            double s, q;
            int i, j;

            q = (qf * 100);
            s = (q < 50) ? 5000 / q : (200 - 2 * q);
            for (i = 0; i < 64; i++)
            {
                j = (int)((s * pdjpg_ijg_qtab_y[i] + 50) / 100);
                j = (j < 1) ? 1 : ((j < 256) ? j : 255);
                tab[i] = j;
            }
        }

        public static void PDJPG_MakeQuantTabFastIJG_UV(
            short[] inv, int cnt, BytePtr tab, double qf)
        {
            double s, q;
            int i, j;

            q = (qf * 100);
            s = (q < 50) ? 5000 / q : (200 - 2 * q);
            for (i = 0; i < 64; i++)
            {
                j = (int)((s * pdjpg_ijg_qtab_y[i] + 50) / 100);
                j = (j < 1) ? 1 : ((j < 256) ? j : 255);
                tab[i] = j;
            }
        }

        public static void PDJPG_MakeQuantTabInput(short[] inv, int cnt, BytePtr tab, double q)
        {
            PDJPG_MakeQuantTabInput_AA(inv, cnt, tab, q);
        }

        public static void PDJPG_MakeQuantTabInputY(short[] inv, int cnt, BytePtr tab, double q)
        {
            BytePtr taba, tabb;
            int i;

            //PDJPG_MakeQuantTabInput_AA(inv, cnt, tab, q);
            //PDJPG_MakeQuantTabFastIJG_Y(inv, cnt, tab, q);

            taba = new BytePtr(64);
            tabb = new BytePtr(64);
            PDJPG_MakeQuantTabInput_AA(inv, cnt, taba, q);
            PDJPG_MakeQuantTabFastIJG_Y(inv, cnt, tabb, q);
            for (i = 0; i < 64; i++)
                tab[i] = (taba[i] + tabb[i]) / 2;
        }

        public static void PDJPG_MakeQuantTabInputUV(short[] inv, int cnt, BytePtr tab, double q)
        {
            BytePtr taba, tabb;
            int i;

            //PDJPG_MakeQuantTabInput_AA(inv, cnt, tab, q);
            //PDJPG_MakeQuantTabFastIJG_UV(inv, cnt, tab, q);

            taba = new BytePtr(64);
            tabb = new BytePtr(64);
            PDJPG_MakeQuantTabInput_AA(inv, cnt, taba, q);
            PDJPG_MakeQuantTabFastIJG_UV(inv, cnt, tabb, q);
            for (i = 0; i < 64; i++)
                tab[i] = (taba[i] + tabb[i]) / 2;
        }

#if true
        public static void PDJPG_MakeMegaQuantTabInputY(short[] inv, int cnt, BytePtr tab, double q)
        {
            BytePtr taba, tabb;
            int i;

            //PDJPG_MakeQuantTabInput_AA(inv, cnt, tab, q);
            //PDJPG_MakeQuantTabFastIJG_Y(inv, cnt, tab, q);

            taba = new BytePtr(64);
            tabb = new BytePtr(64);
            PDJPG_MakeQuantTabInput_AA(inv, cnt*64, taba, q);
            PDJPG_MakeQuantTabFastIJG_Y(inv, cnt*64, tabb, q);
            for (i = 0; i < 64; i++)
                tab[i] = (taba[i] + tabb[i]) / 2;
            for (i = 0; i < 64; i++)
                tab[64+i] = (taba[i] + tabb[i]) / 2;
        }

        public static void PDJPG_MakeMegaQuantTabInputUV(short[] inv, int cnt, BytePtr tab, double q)
        {
            BytePtr taba, tabb;
            int i;

            //PDJPG_MakeQuantTabInput_AA(inv, cnt, tab, q);
            //PDJPG_MakeQuantTabFastIJG_UV(inv, cnt, tab, q);

            taba = new BytePtr(64);
            tabb = new BytePtr(64);
            PDJPG_MakeQuantTabInput_AA(inv, cnt*64, taba, q);
            PDJPG_MakeQuantTabFastIJG_UV(inv, cnt*64, tabb, q);
            for (i = 0; i < 64; i++)
                tab[i] = (taba[i] + tabb[i]) / 2;
            for (i = 0; i < 64; i++)
                tab[64 + i] = (taba[i] + tabb[i]) / 2;
        }
#endif

        public static void PDJPG_MakeQuantTabInput_AA(short[] inv, int cnt, BytePtr tab, double q)
        {
            //	static double deltas[DCTSZ2];
            double[] deltas = new double[DCTSZ2];
            double f, g, te;
            int i, j, k;

            if (q >= 1)
            {
                for (i = 0; i < DCTSZ2; i++)
                    tab[i] = 1;
                return;
            }

            for (j = 0; j < DCTSZ2; j++)
                deltas[j] = 0;
#if false
            for (i = 0; i < cnt; i++)
                for (j = 0; j < DCTSZ2; j++)
                    deltas[j] += Math.Abs(inv[i * DCTSZ2 + j] * 1.0);
            for (j = 0; j < DCTSZ2; j++)
                deltas[j] /= cnt;
#endif

#if true
            for (i = 0; i < cnt; i++)
                for (j = 0; j < DCTSZ2; j++)
                {
                    k = inv[i * DCTSZ2 + j];
                    deltas[j] += k*k;
                }
            for (j = 0; j < DCTSZ2; j++)
            {
                deltas[j] = Math.Sqrt(deltas[j] / cnt);
            }
#endif

            te = 0;
            for (j = 0; j < DCTSZ2; j++)
                te += deltas[j];

            for (i = 0; i < DCTSZ; i++)
            {
                for (j = 0; j < DCTSZ; j++)
                {
                    f = deltas[i * DCTSZ + j];

                    f = f / te;
                    if (f < 0.00001) f = 0.00001;
                    g = (1.0 - q);
                    f = 10 * g * g * g / f;
                    k = (int)f;
                    if (k < 1) k = 1;
                    if (k > 255) k = 255;

                    //			printf("%f/%f %d\n", deltas[i*DCTSZ+j], te, k);

                    //			if(f<1)f=1;
                    //			if(f>255)f=255;
                    tab[i * DCTSZ + j] = (byte)k;
                }
            }

#if false
            if (tab[DCTSZ2 - 3] > 1)
                tab[DCTSZ2 - 3] = (byte)(tab[DCTSZ2 - 3] * 0.75);
            if (tab[DCTSZ2 - 2] > 1)
                tab[DCTSZ2 - 2] = (byte)(tab[DCTSZ2 - 2] * 0.75);
            if (tab[DCTSZ2 - 1] > 1)
                tab[DCTSZ2 - 1] = (byte)(tab[DCTSZ2 - 1] * 0.5);
#endif
        }

        public static void PDJPG_MakeQuantTabInputFast(
            short[] inv, int cnt, BytePtr tab, double q)
        {
            int i;
            //	PDJPG_MakeQuantTabInput(in, cnt, tab, q);

            i = cnt / 16;
            if (i < 1) i = 1;
            PDJPG_MakeQuantTabInput(inv, i, tab, q);
        }

        public static void PDJPG_EmitMarkerBCST(PDJPG_Context ctx)
        {
            BytePtr lfp;
            int i;

            if (ctx.jpg_rdct)
            {
                //Emit libjpeg SERM marker
                ctx.huff.ct.emit(0xFF);
                ctx.huff.ct.emit(JPG.JPG_APP9);

                lfp = ctx.huff.ct;
                ctx.huff.ct.emit(0x00);	//length
                ctx.huff.ct.emit(0x00);
                ctx.huff.ct.emit('J');
                ctx.huff.ct.emit('P');
                ctx.huff.ct.emit('S');
                ctx.huff.ct.emit('E');
                ctx.huff.ct.emit('R');
                ctx.huff.ct.emit('M');
                ctx.huff.ct.emit(0);

                i = ctx.huff.ct - lfp;
                lfp[0] = i >> 8;	//length
                lfp[1] = i & 0xFF;
            }

            if (ctx.jpg_clrtrans == PDJPG_CLRS_RGB)
            {
                ctx.huff.ct.emit(0xFF);
                ctx.huff.ct.emit(JPG.JPG_APP14);

                lfp = ctx.huff.ct;
                ctx.huff.ct.emit(0x00);	//length
                ctx.huff.ct.emit(0x00);

                ctx.huff.ct.emit('A');
                ctx.huff.ct.emit('d');
                ctx.huff.ct.emit('o');
                ctx.huff.ct.emit('b');
                ctx.huff.ct.emit('e');

                i = 100;
                ctx.huff.ct.emit((i >> 8) & 0xFF);
                ctx.huff.ct.emit(i & 0xFF);

                i = 0;
                ctx.huff.ct.emit((i >> 24) & 0xFF);
                ctx.huff.ct.emit((i >> 16) & 0xFF);
                ctx.huff.ct.emit((i >> 8) & 0xFF);
                ctx.huff.ct.emit(i & 0xFF);

                ctx.huff.ct.emit(0);	//RGB
                //		ctx.huff.ct.emit(ctx.bcst_bpp);

                i = ctx.huff.ct - lfp;
                lfp[0] = i >> 8;	//length
                lfp[1] = i & 0xFF;
            }

            ctx.bcst_version = 0x0101;
            ctx.bcst_flags = 0;
            ctx.bcst_trans = ctx.jpg_clrtrans;
            ctx.bcst_bpp = 8;

            if (ctx.jpg_rdct)
                ctx.bcst_flags |= PDJPG_BCSFL_RDCT;

            if (ctx.layer_hidden)
                ctx.bcst_flags |= PDJPG_BCSFL_LAYER_HIDDEN;

            if (ctx.megablock)
                ctx.bcst_flags |= PDJPG_BCSFL_MEGABLOCK;
            if (ctx.altvlc)
                ctx.bcst_flags |= PDJPG_BCSFL_ALTVLC;

            if((ctx.jpg_qfl&PDJPG_QFL_ALPHACYAN)!=0)
                ctx.bcst_flags |= PDJPG_BCSFL_ALPHACYAN;

            if (ctx.rangecoder)
                ctx.bcst_flags |= PDJPG_BCSFL_RANGECODER;
            if (ctx.auto_dct)
                ctx.bcst_flags |= PDJPG_BCSFL_AUTODCT;

            //check if not to bother with marker...
            if ((ctx.bcst_flags == 0) && (ctx.bcst_trans == 0))
            {
                //		printf("PDJPG_EmitMarkerBCST: No Emit\n");
                return;
            }

            //	printf("PDJPG_EmitMarkerBCST: Emit\n");

            ctx.huff.ct.emit(0xFF);
            ctx.huff.ct.emit(JPG.JPG_APP12);

            lfp = ctx.huff.ct;
            ctx.huff.ct.emit(0x00);	//length
            ctx.huff.ct.emit(0x00);

            ctx.huff.ct.emit('B');
            ctx.huff.ct.emit('C');
            ctx.huff.ct.emit('S');
            ctx.huff.ct.emit('T');

            ctx.huff.ct.emit((ctx.bcst_version >> 8) & 0xFF);
            ctx.huff.ct.emit(ctx.bcst_version & 0xFF);

            ctx.huff.ct.emit(ctx.bcst_trans);
            ctx.huff.ct.emit(ctx.bcst_bpp);

            ctx.huff.ct.emit((byte)(ctx.bcst_flags >> 24) & 0xFF);
            ctx.huff.ct.emit((byte)(ctx.bcst_flags >> 16) & 0xFF);
            ctx.huff.ct.emit((byte)(ctx.bcst_flags >> 8) & 0xFF);
            ctx.huff.ct.emit((byte)ctx.bcst_flags & 0xFF);

            ctx.huff.ct.emit(ctx.bcst_alpha);
            ctx.huff.ct.emit(ctx.bcst_blend);

            ctx.huff.ct.emit((ctx.bcst_orgx >> 8) & 0xFF);
            ctx.huff.ct.emit(ctx.bcst_orgx & 0xFF);
            ctx.huff.ct.emit((ctx.bcst_orgy >> 8) & 0xFF);
            ctx.huff.ct.emit(ctx.bcst_orgy & 0xFF);

            ctx.huff.ct.EmitWord16BE(ctx.bcst_minx);
            ctx.huff.ct.EmitWord16BE(ctx.bcst_miny);
            ctx.huff.ct.EmitWord16BE(ctx.bcst_maxx);
            ctx.huff.ct.EmitWord16BE(ctx.bcst_maxy);

            i = ctx.huff.ct - lfp;
            lfp[0] = i >> 8;	//length
            lfp[1] = i & 0xFF;
        }

        public static void PDJPG_EmitMarkerDMLT_DQT(PDJPG_Context ctx, int n)
        {
            BytePtr tbuf, ct1, ct2;
            int i, j, k, l, nb;

#if false
            nb = 1;
            for (i = 0; i < 64; i++)
            {
                j=ctx.jpg_qt[n * 64 + i];
                while (j >= (1 << nb)) nb++;
            }
#endif

            nb = 4;

            tbuf = new BytePtr(65536);

            ct2 = ctx.huff.ct;
            ctx.huff.InitOutputStream(ctx, tbuf);

            ctx.huff.WriteNBits(ctx, 0, 4);      //reserved
            ctx.huff.WriteNBits(ctx, nb, 4);     //DQT bits

            l = 0;
            for (i = 0; i < 64; i++)
            {
                //ctx.huff.WriteNBits(ctx, ctx.jpg_qt[n * 64 + pdjpg_zigzag2[i]], nb);
                k = ctx.jpg_qt[n * 64 + pdjpg_zigzag2[i]];
                ((PDJHUFF)ctx.huff).WriteSRice(ctx, k - l, nb);
                l = k;
            }

//            WriteNBits(ctx, 1, 4);
//            WriteNBits(ctx, 2, 4);
//            BSXRP_EncodeCodeLengths(ctx, new BytePtr(len, tab * 256), 256);

            ctx.huff.FlushBits(ctx);
            ct1 = ctx.huff.ct;
            ctx.huff.ct = ct2;

            i = ct1 - tbuf;
            ctx.huff.ct.emit(PDJPG_DMLT_DQT0 + n);
            ctx.huff.ct.EmitUVLI((ulong)i);
            BytePtr.memcpy(ctx.huff.ct, tbuf, i);
            ctx.huff.ct += i;
        }

        public static void PDJPG_EmitMarkerDMLT_DHT(PDJPG_Context ctx, int tab)
        {
            BytePtr tbuf, ct1;
            int i;

            tbuf = new BytePtr(65536);
            ct1 = ctx.huff.EmitMiniHufftabBuf(ctx, tbuf, tab);
            i = ct1 - tbuf;
            ctx.huff.ct.emit(PDJPG_DMLT_DHT0 + tab);
            ctx.huff.ct.EmitUVLI((ulong)i);
            BytePtr.memcpy(ctx.huff.ct, tbuf, i);
            ctx.huff.ct += i;
        }

        public static void PDJPG_EmitMarkerDMLT(PDJPG_Context ctx)
        {
            BytePtr lfp;
            int i;

            ctx.huff.ct.emit(0xFF);
            ctx.huff.ct.emit(JPG.JPG_JPG12);

            lfp = ctx.huff.ct;
            ctx.huff.ct.emit(0x00);	//length
            ctx.huff.ct.emit(0x00);

            ctx.huff.ct.emit('D');
            ctx.huff.ct.emit('M');
            ctx.huff.ct.emit('L');
            ctx.huff.ct.emit('T');

            PDJPG_EmitMarkerDMLT_DQT(ctx, 0);
            if (!ctx.jpg_mono) PDJPG_EmitMarkerDMLT_DQT(ctx, 1);

            PDJPG_EmitMarkerDMLT_DHT(ctx, 0);
            PDJPG_EmitMarkerDMLT_DHT(ctx, 1);
            if (!ctx.jpg_mono)
            {
                PDJPG_EmitMarkerDMLT_DHT(ctx, 2);
                PDJPG_EmitMarkerDMLT_DHT(ctx, 3);
            }

            i = ctx.huff.ct - lfp;
            lfp[0] = i >> 8;	//length
            lfp[1] = i & 0xFF;

            //PDJPG_EmitDQT(ctx, 0);
            //if (!ctx.jpg_mono) PDJPG_EmitDQT(ctx, 1);

            //PDJPG_EmitSOF(ctx, xs, ys);

            //ctx.huff.EmitDHT(ctx, 0);
            //ctx.huff.EmitDHT(ctx, 1);
            //if (!ctx.jpg_mono)
            //{
            //ctx.huff.EmitDHT(ctx, 2);
            //ctx.huff.EmitDHT(ctx, 3);
            //}

            PDJPG_EmitSOF(ctx, ctx.xs, ctx.ys);
        }

        public static void PDJPG_EncodeCtx_Prepare(
            PDJPG_Context ctx, int xs, int ys, int qfl, int pf)
        {
            int xs2, ys2, xs3, ys3;
            //	int cr, cg, cb, cy, cu, cv;
            int i, qf;

            qf = qfl & 255;

            ctx.jpg_qfl = qfl;

            ctx.layer_hidden = false;
            if ((qfl & PDJPG_QFL_LAYER_HIDDEN) != 0)
                ctx.layer_hidden = true;

            ctx.megablock = false;
            if ((qfl & PDJPG_QFL_MEGABLOCK) != 0)
                ctx.megablock = true;

            ctx.altvlc = false;
            if ((qfl & PDJPG_QFL_ALTVLC) != 0)
                ctx.altvlc = true;

            ctx.rangecoder = false;
            if ((qfl & PDJPG_QFL_RANGECODER) != 0)
                ctx.rangecoder = true;

            ctx.auto_dct = false;
            if ((qfl & PDJPG_QFL_AUTODCT) != 0)
                ctx.auto_dct = true;

            if (ctx.megablock)
                ctx.altvlc = true;
            if (ctx.auto_dct)
                ctx.altvlc = true;

            ctx.jpg_is420 = true;

            if ((qf == 110) || (qf == 111) || ((qfl & PDJPG_QFL_444) != 0) ||
                    (qf == 102) || (qf == 103))
                ctx.jpg_is420 = false;

            ctx.jpg_noquant = false;
            if (qf >= 100)
                ctx.jpg_noquant = true;

            //combination not allowed
            //if (ctx.auto_dct && (qf < 100))
                //ctx.auto_dct = false;

            //	ctx.jpg_is444=ctx.huff.;
            if (ctx.megablock)
            {
                if (ctx.jpg_is420)
                {
                    xs2 = ((xs + 63) / 64) * 64;
                    ys2 = ((ys + 63) / 64) * 64;
                    //		xs3=(xs2+1)/2;
                    //		ys3=(ys2+1)/2;

                    xs3 = ((xs + 127) / 64) * 64;
                    ys3 = ((ys + 127) / 64) * 64;
                    ctx.jpg_is444 = false;
                }
                else
                {
                    xs2 = ((xs + 63) / 64) * 64;
                    ys2 = ((ys + 63) / 64) * 64;
                    xs3 = xs2;
                    ys3 = ys2;
                    ctx.jpg_is444 = true;
                }
            }
            else
            {
                if (ctx.jpg_is420)
                {
                    xs2 = ((xs + 7) / 8) * 8;
                    ys2 = ((ys + 7) / 8) * 8;
                    //		xs3=(xs2+1)/2;
                    //		ys3=(ys2+1)/2;

                    xs3 = ((xs + 15) / 16) * 8;
                    ys3 = ((ys + 15) / 16) * 8;
                    ctx.jpg_is444 = false;
                }
                else
                {
                    xs2 = ((xs + 7) / 8) * 8;
                    ys2 = ((ys + 7) / 8) * 8;
                    xs3 = xs2;
                    ys3 = ys2;
                    ctx.jpg_is444 = true;
                }
            }

            ctx.xs = xs;
            ctx.ys = ys;
            ctx.xs2 = xs2;
            ctx.ys2 = ys2;
            ctx.xs3 = xs3;
            ctx.ys3 = ys3;

            ctx.jpg_mono = false;
            ctx.jpg_yuvw = false;
            ctx.jpg_tabcacheframe = 0;
            ctx.xs = xs;
            ctx.ys = ys;

            if (pf == PDJPG_YYYA)
                ctx.jpg_mono = true;

            ctx.jpg_rdct = false;
            if ((qf == 110) || (qf == 111) || ((qfl & PDJPG_QFL_RDCT) != 0))
                ctx.jpg_rdct = true;

            ctx.jpg_clrtrans = 0;
            if ((qf == 110) || ((qfl & PDJPG_QFL_ORCT) != 0) || (qf == 101) || (qf == 103))
                ctx.jpg_clrtrans = PDJPG_CLRS_ORCT;
            if ((qf == 111) || ((qfl & PDJPG_QFL_RGB) != 0) || (qf == 104))
                ctx.jpg_clrtrans = PDJPG_CLRS_RGB;

            //full quality
            for (i = 0; i < 64; i++) ctx.jpg_qt[0 * 64 + i] = 1;
            for (i = 0; i < 64; i++) ctx.jpg_qt[1 * 64 + i] = 1;

            if (ctx.rangecoder)
            {
                ctx.huff = new BTJRange(ctx);
                ctx.huff.SetDefaults();
            }else
            {
                ctx.huff = new PDJHUFF(ctx);
                ctx.huff.SetDefaults();
            }


            //	for(i=0; i<255; i++)ctx.huff.len[0*256+i]=8;
            //	for(i=0; i<255; i++)ctx.huff.len[2*256+i]=8;


            //	ctx.huff.len[1*256+0]=7;

            //	printf("M0\n");

            //make sure intermediate buffers are present and the correct size
            if ((ctx.yb == null) || (xs != ctx.lxs) || (ys != ctx.lys))
            {
#if false
		if(ctx.yb)
		{
			free(ctx.yb);
			free(ctx.ub);
			free(ctx.vb);
			free(ctx.ydb);
			free(ctx.udb);
			free(ctx.vdb);
		}

		//W channel is rare, and so is handled specially
		if(ctx.wb)
		{
			free(ctx.wb);
			free(ctx.wdb);
		}
#endif

                //		ctx.yb=malloc(xs2*ys2);
                //		ctx.ub=malloc(xs2*ys2);
                //		ctx.vb=malloc(xs2*ys2);

                ctx.yb = new short[(xs2 + 16) * (ys2 + 16)];
                ctx.ub = new short[(xs2 + 16) * (ys2 + 16)];
                ctx.vb = new short[(xs2 + 16) * (ys2 + 16)];

                ctx.ydb = new short[(xs2 + 16) * (ys2 + 16)];
                ctx.udb = new short[(xs2 + 16) * (ys2 + 16)];
                ctx.vdb = new short[(xs2 + 16) * (ys2 + 16)];

                ctx.ydtb = new int[((xs2 + 16) * (ys2 + 16))/64];
                ctx.udtb = new int[((xs2 + 16) * (ys2 + 16))/64];
                ctx.vdtb = new int[((xs2 + 16) * (ys2 + 16))/64];

                if (ctx.jpg_yuvw)
                {
                    ctx.wb = new short[(xs2 + 16) * (ys2 + 16)];
                    ctx.wdb = new short[(xs2 + 16) * (ys2 + 16)];
                    ctx.wdtb = new int[((xs2 + 16) * (ys2 + 16)) / 64];
                }

                ctx.lxs = xs;
                ctx.lys = ys;
            }

            //	memset(ctx.yb, 128, xs2*ys2);
            //	memset(ctx.ub, 128, xs2*ys2);
            //	memset(ctx.vb, 128, xs2*ys2);

            ShortPtr.memset(ctx.yb, 128, xs2 * ys2);
            ShortPtr.memset(ctx.ub, 128, xs2 * ys2);
            ShortPtr.memset(ctx.vb, 128, xs2 * ys2);

            ShortPtr.memset(ctx.ydb, 0, xs2 * (ys2 + 8));
            ShortPtr.memset(ctx.udb, 0, xs3 * (ys3 + 8));
            ShortPtr.memset(ctx.vdb, 0, xs3 * (ys3 + 8));

            if (ctx.jpg_yuvw)
            {
                ShortPtr.memset(ctx.wb, 128, xs2 * ys2);
                ShortPtr.memset(ctx.wdb, 0, xs2 * (ys2 + 8));
            }
        }

        public static void PDJPG_ConvertImageYUV(PDJPG_Context ctx,
            BytePtr ibuf, int xs, int ys, int pf,
            int xs2, int ys2, int xs3, int ys3)
        {
            //	void (*getPixel2)(BytePtr rgb,
            //		int *ra, int *ga, int *ba,
            //		int *rb, int *gb, int *bb);
            BytePtr cse;
            ShortPtr ctu, ctv;
            BytePtr cs1, cs2;
            ShortPtr cty1, cty2;

            int cr, cg, cb, ca, cy, cu, cv;

            int cra, cga, cba, caa, cya, cua, cva;
            int crb, cgb, cbb, cab, cyb, cub, cvb;
            int crc, cgc, cbc, cac, cyc, cuc, cvc;
            int crd, cgd, cbd, cad, cyd, cud, cvd;
            int psz, psz2;

            int i, k, l, n;

            if ((pf == PDJPG_RGBA) && (ctx.alphaClr == 0))
            {
                n = (ys + 1) / 2;
                for (i = 0; i < n; i++)
                {
                    k = ((ys - (2 * i + 1)) * xs) * 4;
                    cs1 = ibuf + k; cse = cs1 + xs * 4;
                    l = ((ys - (2 * i + 2)) * xs) * 4;
                    cs2 = ibuf + l;

                    cty1 = new ShortPtr(ctx.yb, (2 * i + 0) * xs2);
                    cty2 = new ShortPtr(ctx.yb, (2 * i + 1) * xs2);

                    l = i * xs3;
                    ctu = new ShortPtr(ctx.ub, l);
                    ctv = new ShortPtr(ctx.vb, l);

                    while (cs1 < cse)
                    {
                        cra = cs1[0]; cga = cs1[1]; cba = cs1[2];
                        crb = cs1[4]; cgb = cs1[5]; cbb = cs1[6];
                        crc = cs2[0]; cgc = cs2[1]; cbc = cs2[2];
                        crd = cs2[4]; cgd = cs2[5]; cbd = cs2[6];

                        cya = 19595 * cra + 38470 * cga + 7471 * cba;
                        cyb = 19595 * crb + 38470 * cgb + 7471 * cbb;
                        cyc = 19595 * crc + 38470 * cgc + 7471 * cbc;
                        cyd = 19595 * crd + 38470 * cgd + 7471 * cbd;
                        cty1.emit(cya >> 16);
                        cty1.emit(cyb >> 16);
                        cty2.emit(cyc >> 16);
                        cty2.emit(cyd >> 16);

                        cr = (cra + crb + crc + crd) >> 2;
                        cg = (cga + cgb + cgc + cgd) >> 2;
                        cb = (cba + cbb + cbc + cbd) >> 2;

                        cu = -11056 * cr - 21712 * cg + 32768 * cb;
                        cv = 32768 * cr - 27440 * cg - 5328 * cb;
                        ctu.emit((cu >> 16) + 128);
                        ctv.emit((cv >> 16) + 128);

                        cs1 += 8; cs2 += 8;
                    }
                }
            }
            else
                if ((pf == PDJPG_RGBA) || (pf == PDJPG_RGB) ||
                    (pf == PDJPG_BGRA) || (pf == PDJPG_BGR))
                {
                    switch (pf)
                    {
                        case PDJPG_RGBA:
                        case PDJPG_BGRA:
                            psz = 4; break;
                        case PDJPG_RGB:
                        case PDJPG_BGR:
                            psz = 3; break;
                        default: psz = 4; break;
                    }
                    psz2 = psz * 2;

                    n = (ys + 1) / 2;
                    for (i = 0; i < n; i++)
                    {
                        k = ((ys - (2 * i + 1)) * xs) * psz;
                        cs1 = ibuf + k; cse = cs1 + xs * psz;
                        l = ((ys - (2 * i + 2)) * xs) * psz;
                        cs2 = ibuf + l;

                        cty1 = new ShortPtr(ctx.yb, (2 * i + 0) * xs2);
                        cty2 = new ShortPtr(ctx.yb, (2 * i + 1) * xs2);

                        l = i * xs3;
                        ctu = new ShortPtr(ctx.ub, l);
                        ctv = new ShortPtr(ctx.vb, l);

                        while (cs1 < cse)
                        {
                            //				getPixel2(cs1, &cra, &cga, &cba, &crb, &cgb, &cbb);
                            //				getPixel2(cs2, &crc, &cgc, &cbc, &crd, &cgd, &cbd);

                            switch (pf)
                            {
                                case PDJPG_RGBA:
                                    cra = cs1[0]; cga = cs1[1]; cba = cs1[2]; caa = cs1[3];
                                    crb = cs1[4]; cgb = cs1[5]; cbb = cs1[6]; cab = cs1[7];
                                    crc = cs2[0]; cgc = cs2[1]; cbc = cs2[2]; cac = cs2[3];
                                    crd = cs2[4]; cgd = cs2[5]; cbd = cs2[6]; cad = cs2[7];
                                    psz = 8; break;
                                case PDJPG_BGRA:
                                    cra = cs1[2]; cga = cs1[1]; cba = cs1[0]; caa = cs1[3];
                                    crb = cs1[6]; cgb = cs1[5]; cbb = cs1[4]; cab = cs1[7];
                                    crc = cs2[2]; cgc = cs2[1]; cbc = cs2[0]; cac = cs2[3];
                                    crd = cs2[6]; cgd = cs2[5]; cbd = cs2[4]; cad = cs2[7];
                                    psz = 8; break;
                                case PDJPG_RGB:
                                    cra = cs1[0]; cga = cs1[1]; cba = cs1[2]; caa = 255;
                                    crb = cs1[3]; cgb = cs1[4]; cbb = cs1[5]; cab = 255;
                                    crc = cs2[0]; cgc = cs2[1]; cbc = cs2[2]; cac = 255;
                                    crd = cs2[3]; cgd = cs2[4]; cbd = cs2[5]; cad = 255;
                                    psz = 6; break;
                                case PDJPG_BGR:
                                    cra = cs1[2]; cga = cs1[1]; cba = cs1[0]; caa = 255;
                                    crb = cs1[5]; cgb = cs1[4]; cbb = cs1[3]; cab = 255;
                                    crc = cs2[2]; cgc = cs2[1]; cbc = cs2[0]; cac = 255;
                                    crd = cs2[5]; cgd = cs2[4]; cbd = cs2[3]; cad = 255;
                                    psz = 6; break;
                                default:
                                    cra = 0; crb = 0; crc = 0; crd = 0;
                                    cga = 0; cgb = 0; cgc = 0; cgd = 0;
                                    cba = 0; cbb = 0; cbc = 0; cbd = 0;
                                    caa = 0; cab = 0; cac = 0; cad = 0;
                                    break;
                            }

                            if (ctx.alphaClr != 0)
                            {
                                ca = (caa + cab + cac + cad) >> 2;
                                if (ca < ctx.alphaClrA)
                                {
                                    cy = ctx.alphaClrY;
                                    cu = ctx.alphaClrU;
                                    cv = ctx.alphaClrV;
                                    cty1.emit(cy); cty1.emit(cy);
                                    cty2.emit(cy); cty2.emit(cy);
                                    ctu.emit(cu); ctv.emit(cv);
                                    continue;
                                }
                            }

                            cya = 19595 * cra + 38470 * cga + 7471 * cba;
                            cyb = 19595 * crb + 38470 * cgb + 7471 * cbb;
                            cyc = 19595 * crc + 38470 * cgc + 7471 * cbc;
                            cyd = 19595 * crd + 38470 * cgd + 7471 * cbd;
                            cty1.emit(cya >> 16);
                            cty1.emit(cyb >> 16);
                            cty2.emit(cyc >> 16);
                            cty2.emit(cyd >> 16);

                            cr = (cra + crb + crc + crd) >> 2;
                            cg = (cga + cgb + cgc + cgd) >> 2;
                            cb = (cba + cbb + cbc + cbd) >> 2;

                            cu = -11056 * cr - 21712 * cg + 32768 * cb;
                            cv = 32768 * cr - 27440 * cg - 5328 * cb;
                            ctu.emit((cu >> 16) + 128);
                            ctv.emit((cv >> 16) + 128);

                            cs1 += psz2; cs2 += psz2;
                        }
                    }
                }
                else
                {
                    switch (pf)
                    {
                        case PDJPG_YUVA: psz = 4; psz2 = 8; break;
                        case PDJPG_YUV: psz = 3; psz2 = 6; break;
                        case PDJPG_YUV422: psz = 2; psz2 = 4; break;
                        case PDJPG_YUV420: psz = 3; psz2 = 3; break;
                        case PDJPG_YA: psz = 2; psz2 = 4; break;
                        case PDJPG_Y: psz = 1; psz2 = 2; break;
                        case PDJPG_YYYA: psz = 4; psz2 = 8; break;
                        default: psz = 4; psz2 = 8; break;
                    }

                    n = (ys + 1) / 2;
                    for (i = 0; i < n; i++)
                    {
                        if (pf == PDJPG_YUV420)
                        {
                            k = (((ys - (2 * i + 1)) * xs2) * psz2) >> 1;
                            cs1 = ibuf + k; cse = cs1 + ((xs * psz2) >> 1);
                            l = (((ys - (2 * i + 2)) * xs2) * psz2) >> 1;
                            cs2 = ibuf + l;
                        }
                        else
                        {
                            k = ((ys - (2 * i + 1)) * xs) * psz;
                            cs1 = ibuf + k; cse = cs1 + xs * psz;
                            l = ((ys - (2 * i + 2)) * xs) * psz;
                            cs2 = ibuf + l;
                        }

                        cty1 = new ShortPtr(ctx.yb, (2 * i + 0) * xs2);
                        cty2 = new ShortPtr(ctx.yb, (2 * i + 1) * xs2);

                        l = i * xs3;
                        ctu = new ShortPtr(ctx.ub, l);
                        ctv = new ShortPtr(ctx.vb, l);

                        while (cs1 < cse)
                        {
                            //				getPixel2(cs1, &cra, &cga, &cba, &crb, &cgb, &cbb);
                            //				getPixel2(cs2, &crc, &cgc, &cbc, &crd, &cgd, &cbd);

                            switch (pf)
                            {
                                case PDJPG_YUVA:
                                    cya = cs1[0]; cua = cs1[1]; cva = cs1[2];
                                    cyb = cs1[4]; cub = cs1[5]; cvb = cs1[6];
                                    cyc = cs2[0]; cuc = cs2[1]; cvc = cs2[2];
                                    cyd = cs2[4]; cud = cs2[5]; cvd = cs2[6];
                                    cu = (cua + cub + cuc + cud) >> 2;
                                    cv = (cva + cvb + cvc + cvd) >> 2;
                                    break;
                                case PDJPG_YUV:
                                    cya = cs1[0]; cua = cs1[1]; cva = cs1[2];
                                    cyb = cs1[3]; cub = cs1[4]; cvb = cs1[5];
                                    cyc = cs2[0]; cuc = cs2[1]; cvc = cs2[2];
                                    cyd = cs2[3]; cud = cs2[4]; cvd = cs2[5];
                                    cu = (cua + cub + cuc + cud) >> 2;
                                    cv = (cva + cvb + cvc + cvd) >> 2;
                                    break;
                                case PDJPG_YUV422:
                                    cya = cs1[0]; cua = cs1[1]; cyb = cs1[2]; cva = cs1[3];
                                    cyc = cs2[0]; cuc = cs2[1]; cyd = cs2[2]; cvc = cs2[3];
                                    cu = (cua + cuc) >> 1; cv = (cva + cvc) >> 1;
                                    break;
                                case PDJPG_YUV420:
                                    cya = cs1[0]; cyb = cs1[1]; cu = cs1[2];
                                    cyc = cs2[0]; cyd = cs2[1]; cv = cs2[2];
                                    break;
                                case PDJPG_YA:
                                    cya = cs1[0]; cyb = cs1[2];
                                    cyc = cs2[0]; cyd = cs2[2];
                                    cu = 0; cv = 0;
                                    break;
                                case PDJPG_Y:
                                    cya = cs1[0]; cyb = cs1[1];
                                    cyc = cs2[0]; cyd = cs2[1];
                                    cu = 0; cv = 0;
                                    break;

                                case PDJPG_YYYA:
                                    cya = cs1[1]; cyb = cs1[5];
                                    cyc = cs2[1]; cyd = cs2[5];
                                    cu = 0; cv = 0;
                                    break;
                                default:
                                    cya = 0; cyb = 0; cyc = 0; cyd = 0;
                                    cua = 0; cub = 0; cuc = 0; cud = 0;
                                    cva = 0; cvb = 0; cvc = 0; cvd = 0;
                                    caa = 0; cab = 0; cac = 0; cad = 0;
                                    cu = 0; cv = 0;
                                    break;
                            }

                            cty1.emit(cya); cty1.emit(cyb);
                            cty2.emit(cyc); cty2.emit(cyd);
                            ctu.emit(cu); ctv.emit(cv);

                            cs1 += psz2; cs2 += psz2;
                        }
                    }
                }
        }

        public static void PDJPG_EncodeCtx_ColorTransform(
            PDJPG_Context ctx, BytePtr ibuf, int xs, int ys, int pf)
        {
            int xs2, ys2, xs3, ys3;
            int cr, cg, cb, ca, cy, cu, cv, cw;
            int i, j, k;

            xs2 = ctx.xs2;
            ys2 = ctx.ys2;
            xs3 = ctx.xs3;
            ys3 = ctx.ys3;

            for (i = 0; i < ys; i++)
                for (j = 0; j < xs; j++)
                {
                    k = ys - (i + 1);
                    cr = ibuf[(k * xs + j) * 4 + 0];
                    cg = ibuf[(k * xs + j) * 4 + 1];
                    cb = ibuf[(k * xs + j) * 4 + 2];
                    ca = ibuf[(k * xs + j) * 4 + 3];

                    //		if(qf==110)
                    if (ctx.jpg_clrtrans == PDJPG_CLRS_YCBCR)
                    {
                        cy = (int)(0.299 * cr + 0.587 * cg + 0.114 * cb);
                        cu = (int)(-0.1687 * cr - 0.3313 * cg + 0.5 * cb + 128);
                        cv = (int)(0.5 * cr - 0.4187 * cg - 0.0813 * cb + 128);
                        cw = ca;

                        cy = (cy < 0) ? 0 : ((cy > 255) ? 255 : cy);
                        cu = (cu < 0) ? 0 : ((cu > 255) ? 255 : cu);
                        cv = (cv < 0) ? 0 : ((cv > 255) ? 255 : cv);
                        cw = (cw < 0) ? 0 : ((cw > 255) ? 255 : cw);
                    }
                    else if (ctx.jpg_clrtrans == PDJPG_CLRS_ORCT)
                    {
                        //			cy=(cr+2*cg+cb)/4;
                        cy = cg + (cb + cr - 2 * cg) / 4;
                        cu = (cb - cg) + 128;
                        cv = (cr - cg) + 128;
                        cw = ca;
                        //			k=cy-(cu+cv-256)/4;
                        //			cy+=cg-k;
                        //			if(k!=cg)cy++;

                        //			cu=cb-cr;
                        //			cv=cg-cr-0.5*cu;
                        //			cy=cg-0.5*cv;
                        //			cu+=128; cv+=128;
                    }
                    else if (ctx.jpg_clrtrans == PDJPG_CLRS_RGB)
                    {
                        cy = cr;
                        cu = cg;
                        cv = cb;
                        cw = ca;
                    }
                    else
                    {
                        cy = (int)(0.299 * cr + 0.587 * cg + 0.114 * cb);
                        cu = (int)(-0.1687 * cr - 0.3313 * cg + 0.5 * cb + 128);
                        cv = (int)(0.5 * cr - 0.4187 * cg - 0.0813 * cb + 128);
                        cw = ca;
                    }

                    //		cu=128; cv=128;


                    ctx.yb[i * xs2 + j] = (short)cy;
                    ctx.ub[i * xs2 + j] = (short)cu;
                    ctx.vb[i * xs2 + j] = (short)cv;
                    if (ctx.jpg_yuvw)
                        ctx.wb[i * xs2 + j] = (short)cw;
                }

        }

        public static void PDJPG_EncodeCtx_ColorTransform16(
            PDJPG_Context ctx, ShortPtr ibuf, int xs, int ys, int pf)
        {
            int xs2, ys2, xs3, ys3;
            int cr, cg, cb, ca, cy, cu, cv, cw;
            int i, j, k;

            xs2 = ctx.xs2;
            ys2 = ctx.ys2;
            xs3 = ctx.xs3;
            ys3 = ctx.ys3;

            for (i = 0; i < ys; i++)
                for (j = 0; j < xs; j++)
                {
                    k = ys - (i + 1);
                    cr = ibuf[(k * xs + j) * 4 + 0];
                    cg = ibuf[(k * xs + j) * 4 + 1];
                    cb = ibuf[(k * xs + j) * 4 + 2];
                    ca = ibuf[(k * xs + j) * 4 + 3];

                    //		if(qf==110)
                    if (ctx.jpg_clrtrans == PDJPG_CLRS_YCBCR)
                    {
                        cy = (int)(0.299 * cr + 0.587 * cg + 0.114 * cb);
                        cu = (int)(-0.1687 * cr - 0.3313 * cg + 0.5 * cb + 128);
                        cv = (int)(0.5 * cr - 0.4187 * cg - 0.0813 * cb + 128);
                        cw = ca;
                    }
                    else if (ctx.jpg_clrtrans == PDJPG_CLRS_ORCT)
                    {
                        //			cy=(cr+2*cg+cb)/4;
                        cy = cg + (cb + cr - 2 * cg) / 4;
                        cu = (cb - cg) + 128;
                        cv = (cr - cg) + 128;
                        cw = ca;
                    }
                    else if (ctx.jpg_clrtrans == PDJPG_CLRS_RGB)
                    {
                        cy = cr; cu = cg; cv = cb; cw = ca;
                    }
                    else
                    {
                        cy = (int)(0.299 * cr + 0.587 * cg + 0.114 * cb);
                        cu = (int)(-0.1687 * cr - 0.3313 * cg + 0.5 * cb + 128);
                        cv = (int)(0.5 * cr - 0.4187 * cg - 0.0813 * cb + 128);
                        cw = ca;
                    }

                    //		cu=128; cv=128;

                    //		cy=(cy<0)?0:((cy>255)?255:cy);
                    //		cu=(cu<0)?0:((cu>255)?255:cu);
                    //		cv=(cv<0)?0:((cv>255)?255:cv);

                    ctx.yb[i * xs2 + j] = (short)cy;
                    ctx.ub[i * xs2 + j] = (short)cu;
                    ctx.vb[i * xs2 + j] = (short)cv;
                    if (ctx.jpg_yuvw)
                        ctx.wb[i * xs2 + j] = (short)cw;
                }

        }

        public static void PDJPG_EncodeCtx_DownSample(PDJPG_Context ctx)
        {
            int xs2, ys2, xs3, ys3;
            int i, j, k;

            if (!ctx.jpg_is420)
                return;

            xs2 = ctx.xs2;
            ys2 = ctx.ys2;
            xs3 = ctx.xs3;
            ys3 = ctx.ys3;

            //	if((xs3!=xs2) || (ys3!=ys2))
            for (i = 0; i < ys3; i++)
                for (j = 0; j < xs3; j++)
                {
                    k = ctx.ub[(i * 2 + 0) * xs2 + j * 2 + 0] +
                        ctx.ub[(i * 2 + 0) * xs2 + j * 2 + 1] +
                        ctx.ub[(i * 2 + 1) * xs2 + j * 2 + 0] +
                        ctx.ub[(i * 2 + 1) * xs2 + j * 2 + 1];
                    ctx.ub[i * xs3 + j] = (short)(k / 4);

                    k = ctx.vb[(i * 2 + 0) * xs2 + j * 2 + 0] +
                        ctx.vb[(i * 2 + 0) * xs2 + j * 2 + 1] +
                        ctx.vb[(i * 2 + 1) * xs2 + j * 2 + 0] +
                        ctx.vb[(i * 2 + 1) * xs2 + j * 2 + 1];
                    ctx.vb[i * xs3 + j] = (short)(k / 4);

                    if (ctx.jpg_yuvw)
                    {
                        k = ctx.wb[(i * 2 + 0) * xs2 + j * 2 + 0] +
                            ctx.wb[(i * 2 + 0) * xs2 + j * 2 + 1] +
                            ctx.wb[(i * 2 + 1) * xs2 + j * 2 + 0] +
                            ctx.wb[(i * 2 + 1) * xs2 + j * 2 + 1];
                        ctx.wb[i * xs3 + j] = (short)(k / 4);
                    }
                }
        }

        public static int PDJPG_EncodeCtx(
            PDJPG_Context ctx, byte[] ibuf, BytePtr obuf, int xs, int ys,
            int qfl, int pf)
        {
            int i;
            PDJPG_EncodeCtx_Prepare(ctx, xs, ys, qfl, pf);
            PDJPG_EncodeCtx_ColorTransform(ctx, new BytePtr(ibuf), xs, ys, pf);
            PDJPG_EncodeCtx_DownSample(ctx);
            i = PDJPG_EncodeCtx_Inner(ctx, obuf, xs, ys, qfl);
            return (i);
        }

        public static int PDJPG_EncodeCtxSS(
            PDJPG_Context ctx, short[] ibuf, BytePtr obuf, int xs, int ys,
            int qfl, int pf)
        {
            int i;
            PDJPG_EncodeCtx_Prepare(ctx, xs, ys, qfl, pf);
            PDJPG_EncodeCtx_ColorTransform16(ctx, new ShortPtr(ibuf), xs, ys, pf);
            PDJPG_EncodeCtx_DownSample(ctx);
            i = PDJPG_EncodeCtx_Inner(ctx, obuf, xs, ys, qfl);
            return (i);
        }

        public static int PDJPG_EncodeCtx_Inner(
            PDJPG_Context ctx, BytePtr obuf, int xs, int ys, int qfl)
        {
            //	static BytePtr yb=null, *ub, *vb;
            //	static short *ydb=null, *udb, *vdb;
            //	static int lxs=0, lys=0;

            IntPtr dcs = new IntPtr(256);
            IntPtr acs = new IntPtr(256);
            IntPtr dcsuv = new IntPtr(256);
            IntPtr acsuv = new IntPtr(256);
            ShortPtr tp;
            int xs2, ys2, xs3, ys3, qf;
            //	int cr, cg, cb, cy, cu, cv;
            int i, j, k, l, n;

            if (ctx.megablock)
            {
                i = PDJPG_EncodeCtx_MegablockInner(ctx, obuf, xs, ys, qfl);
                return i;
            }

            qf = qfl & 255;

            xs2 = ctx.xs2;
            ys2 = ctx.ys2;
            xs3 = ctx.xs3;
            ys3 = ctx.ys3;

            //	printf("M1\n");
            if (ctx.auto_dct)
            {
                //guestimate tables...

                PDJPG_MakeQuantTabFastIJG_Y(null, 0, new BytePtr(ctx.jpg_qt, 0 * 64), qf / 100.0);
                PDJPG_MakeQuantTabFastIJG_UV(null, 0, new BytePtr(ctx.jpg_qt, 1 * 64), qf / 100.0);

                PDJPG_SetupQuantTabDivFP(ctx, 0);
                PDJPG_SetupQuantTabDivFP(ctx, 1);

                PDJPG_FilterImageAuDCT(ctx, new ShortPtr(ctx.yb), new ShortPtr(ctx.ydb), new IntPtr(ctx.ydtb), xs2, ys2, 128, 0);
                PDJPG_FilterImageAuDCT(ctx, new ShortPtr(ctx.ub), new ShortPtr(ctx.udb), new IntPtr(ctx.udtb), xs3, ys3, 128, 1);
                PDJPG_FilterImageAuDCT(ctx, new ShortPtr(ctx.vb), new ShortPtr(ctx.vdb), new IntPtr(ctx.vdtb), xs3, ys3, 128, 2);
            }else
            //	if(qf==110)
            if (ctx.jpg_rdct)
            {
                //		printf("PDJPG_EncodeCtx: Lossless Detect\n");

                PDJPG_FilterImageRDCT(new ShortPtr(ctx.yb), new ShortPtr(ctx.ydb), xs2, ys2, 128);
                PDJPG_FilterImageRDCT(new ShortPtr(ctx.ub), new ShortPtr(ctx.udb), xs3, ys3, 128);
                PDJPG_FilterImageRDCT(new ShortPtr(ctx.vb), new ShortPtr(ctx.vdb), xs3, ys3, 128);
            }
            else
            {
                PDJPG_FilterImageDCT(new ShortPtr(ctx.yb), new ShortPtr(ctx.ydb), xs2, ys2, 128);
                PDJPG_FilterImageDCT(new ShortPtr(ctx.ub), new ShortPtr(ctx.udb), xs3, ys3, 128);
                PDJPG_FilterImageDCT(new ShortPtr(ctx.vb), new ShortPtr(ctx.vdb), xs3, ys3, 128);
            }

            j = (xs2 / 8) * (ys2 / 8);
            PDJPG_MakeQuantTabInputY(ctx.ydb, j, new BytePtr(ctx.jpg_qt, 0 * 64), qf / 100.0);

            j = (xs3 / 8) * (ys3 / 8);
            PDJPG_MakeQuantTabInputUV(ctx.udb, j, new BytePtr(ctx.jpg_qt, 1 * 64), qf / 100.0);
            PDJPG_MakeQuantTabInputUV(ctx.vdb, j, new BytePtr(ctx.jpg_qt, 2 * 64), qf / 100.0);
            for (i = 0; i < 64; i++) ctx.jpg_qt[1 * 64 + i] = (byte)((ctx.jpg_qt[1 * 64 + i] + ctx.jpg_qt[2 * 64 + i]) / 2);

            PDJPG_SetupQuantTabDivFP(ctx, 0);
            PDJPG_SetupQuantTabDivFP(ctx, 1);

            //	free(yb);
            //	free(ub);
            //	free(vb);


            if (ctx.jpg_mono || !ctx.jpg_is420)
            {
                j = (xs2 / 8) * (ys2 / 8); k = 0;
                for (i = 0; i < j; i++)
                {
                    ctx.block.QuantBlock(ctx, new ShortPtr(ctx.ydb, i * 64), new ShortPtr(ctx.ydb, i * 64), 0, ctx.ydtb[i]);
                    ctx.ydb[i * 64 + 0] -= (short)k; k = ctx.ydb[i * 64 + 0] + k;
                }
            }

            if (!ctx.jpg_mono && ctx.jpg_is420)
            {
                l = 0;
                for (i = 0; i <= (ys3 / 8); i++)
                    for (j = 0; j < (xs3 / 8); j++)
                    {
                        k = ((i * 2 + 0) * (xs2 / 8) + j * 2 + 0);
                        tp = new ShortPtr(ctx.ydb, k * 64);
                        ctx.block.QuantBlock(ctx, tp, tp, 0, ctx.ydtb[k]);
                        tp[0] -= l; l = tp[0] + l;

                        k = ((i * 2 + 0) * (xs2 / 8) + j * 2 + 1);
                        tp = new ShortPtr(ctx.ydb, k * 64);
                        ctx.block.QuantBlock(ctx, tp, tp, 0, ctx.ydtb[k]);
                        tp[0] -= l; l = tp[0] + l;

                        k = ((i * 2 + 1) * (xs2 / 8) + j * 2 + 0);
                        tp = new ShortPtr(ctx.ydb, k * 64);
                        ctx.block.QuantBlock(ctx, tp, tp, 0, ctx.ydtb[k]);
                        tp[0] -= l; l = tp[0] + l;

                        k = ((i * 2 + 1) * (xs2 / 8) + j * 2 + 1);
                        tp = new ShortPtr(ctx.ydb, k * 64);
                        ctx.block.QuantBlock(ctx, tp, tp, 0, ctx.ydtb[k]);
                        tp[0] -= l; l = tp[0] + l;
                    }
            }

            j = (xs3 / 8) * (ys3 / 8); k = 0; l = 0;
            for (i = 0; i < j; i++)
            {
                ctx.block.QuantBlock(ctx, new ShortPtr(ctx.udb, i * 64), new ShortPtr(ctx.udb, i * 64), 1, ctx.udtb[i]);
                ctx.block.QuantBlock(ctx, new ShortPtr(ctx.vdb, i * 64), new ShortPtr(ctx.vdb, i * 64), 1, ctx.vdtb[i]);
                ctx.udb[i * 64 + 0] -= (short)k; k = ctx.udb[i * 64 + 0] + k;
                ctx.vdb[i * 64 + 0] -= (short)l; l = ctx.vdb[i * 64 + 0] + l;
            }

            if (ctx.jpg_yuvw)
            {
                j = (xs3 / 8) * (ys3 / 8); k = 0;
                for (i = 0; i < j; i++)
                {
                    ctx.block.QuantBlock(ctx, new ShortPtr(ctx.wdb, i * 64), new ShortPtr(ctx.wdb, i * 64), 1, ctx.wdtb[i]);
                    ctx.wdb[i * 64 + 0] -= (short)k; k = ctx.wdb[i * 64 + 0] + k;
                }
            }

            //	printf("M2\n");

            for (i = 0; i < 256; i++) dcs[i] = 0;
            for (i = 0; i < 256; i++) acs[i] = 0;
            for (i = 0; i < 256; i++) dcsuv[i] = 0;
            for (i = 0; i < 256; i++) acsuv[i] = 0;

            for (i = 0; i < 4; i++)
                ctx.cstb[i] = 0;

            j = (xs2 / 8) * (ys2 / 8);
            k = (xs3 / 8) * (ys3 / 8);
            //for (i = 0; i < j; i++) ctx.block.StatBlock(ctx, new ShortPtr(ctx.ydb, i * 64), dcs, acs, 0, i, j);
            //for (i = 0; i < k; i++) ctx.block.StatBlock(ctx, new ShortPtr(ctx.udb, i * 64), dcsuv, acsuv, 1, i, k);
            //for (i = 0; i < k; i++) ctx.block.StatBlock(ctx, new ShortPtr(ctx.vdb, i * 64), dcsuv, acsuv, 2, i, k);

            if (ctx.jpg_is420)
            {
                n = (xs3 / 8) * (ys3 / 8);

                for (i = 0; i <= (ys3 / 8); i++)
                {
                    for (j = 0; j < (xs3 / 8); j++)
                    {
                        k = ((i * 2 + 0) * (xs2 / 8) + j * 2 + 0);
                        ctx.block.StatBlock(ctx, new ShortPtr(ctx.ydb, k * 64), dcs, acs, 0, k, n * 4);
                        k = ((i * 2 + 0) * (xs2 / 8) + j * 2 + 1);
                        ctx.block.StatBlock(ctx, new ShortPtr(ctx.ydb, k * 64), dcs, acs, 0, k, n * 4);
                        k = ((i * 2 + 1) * (xs2 / 8) + j * 2 + 0);
                        ctx.block.StatBlock(ctx, new ShortPtr(ctx.ydb, k * 64), dcs, acs, 0, k, n * 4);
                        k = ((i * 2 + 1) * (xs2 / 8) + j * 2 + 1);
                        ctx.block.StatBlock(ctx, new ShortPtr(ctx.ydb, k * 64), dcs, acs, 0, k, n * 4);
                    }
                }
            }
            else
            {
                n = (xs2 / 8) * (ys2 / 8);
                for (i = 0; i < n; i++) ctx.block.StatBlock(ctx, new ShortPtr(ctx.ydb, i * 64), dcs, acs, 0, i, n);
            }

            n = (xs3 / 8) * (ys3 / 8);
            for (i = 0; i < n; i++) ctx.block.StatBlock(ctx, new ShortPtr(ctx.udb, i * 64), dcsuv, acsuv, 1, i, n);
            for (i = 0; i < n; i++) ctx.block.StatBlock(ctx, new ShortPtr(ctx.vdb, i * 64), dcsuv, acsuv, 2, i, n);

            if (ctx.jpg_yuvw)
            {
                for (i = 0; i < n; i++) ctx.block.StatBlock(ctx, new ShortPtr(ctx.wdb, i * 64), dcsuv, acsuv, 3, i, n);
            }

            //	ctx.huff.BuildLengths(dcs, 256, ctx.huff.len+0*256, 16);
            //	ctx.huff.BuildLengths(acs, 256, ctx.huff.len+1*256, 16);
            //	ctx.huff.BuildLengths(dcsuv, 256, ctx.huff.len+2*256, 16);
            //	ctx.huff.BuildLengths(acsuv, 256, ctx.huff.len+3*256, 16);

            //ctx.huff.BuildLengthsAdjust(dcs, 256, new BytePtr(ctx.huff.len, 0 * 256), 16);
            //ctx.huff.BuildLengthsAdjust(acs, 256, new BytePtr(ctx.huff.len, 1 * 256), 16);
            //ctx.huff.BuildLengthsAdjust(dcsuv, 256, new BytePtr(ctx.huff.len, 2 * 256), 16);
            //ctx.huff.BuildLengthsAdjust(acsuv, 256, new BytePtr(ctx.huff.len, 3 * 256), 16);

            ctx.huff.BuildTable(dcs, 0);
            ctx.huff.BuildTable(acs, 1);
            ctx.huff.BuildTable(dcsuv, 2);
            ctx.huff.BuildTable(acsuv, 3);

            ctx.huff.InitOutputStream(ctx, obuf);

            //ctx.huff.ct = obuf;
            //ctx.huff.win = 0;
            //ctx.huff.pos = 0;

            if ((qfl & PDJPG_QFL_NOSOI) == 0)
            {
                ctx.huff.ct.emit(0xFF);
                ctx.huff.ct.emit(JPG.JPG_SOI);
            }

            PDJPG_EmitMarkerBCST(ctx);

            if (!ctx.jpg_dmlt)
            {

                PDJPG_EmitDQT(ctx, 0);
                if (!ctx.jpg_mono) PDJPG_EmitDQT(ctx, 1);

                PDJPG_EmitSOF(ctx, xs, ys);

                ctx.huff.EmitDHT(ctx, 0);
                ctx.huff.EmitDHT(ctx, 1);
                if (!ctx.jpg_mono)
                {
                    ctx.huff.EmitDHT(ctx, 2);
                    ctx.huff.EmitDHT(ctx, 3);
                }
            }
            else
            {
                PDJPG_EmitMarkerDMLT(ctx);
                //PDJPG_EmitSOF(ctx, xs, ys);
            }

            PDJPG_EmitSOS(ctx);

            ctx.huff.InitOutputStream(ctx, ctx.huff.ct);

            //ctx.huff.win = 0;
            //ctx.huff.pos = 0;

            for (i = 0; i < 4; i++ )
                ctx.cstb[i] = 0;

            if (ctx.jpg_mono)
            {
                j = (xs2 / 8) * (ys2 / 8);
                for (i = 0; i < j; i++) ctx.block.EncodeBlock(ctx, new ShortPtr(ctx.ydb, i * 64), 0, 1, 0, i, j);
            }
            else if (ctx.jpg_is420)
            {
                n = (xs3 / 8) * (ys3 / 8);

                for (i = 0; i <= (ys3 / 8); i++)
                    for (j = 0; j < (xs3 / 8); j++)
                    {
                        //ctx.block.EncodeBlock(ctx,
                            //new ShortPtr(ctx.ydb, ((i * 2 + 0) * (xs2 / 8) + j * 2 + 0) * 64), 0, 1);
                        //ctx.block.EncodeBlock(ctx,
                            //new ShortPtr(ctx.ydb, ((i * 2 + 0) * (xs2 / 8) + j * 2 + 1) * 64), 0, 1);
                        //ctx.block.EncodeBlock(ctx,
                            //new ShortPtr(ctx.ydb, ((i * 2 + 1) * (xs2 / 8) + j * 2 + 0) * 64), 0, 1);
                        //ctx.block.EncodeBlock(ctx,
                            //new ShortPtr(ctx.ydb, ((i * 2 + 1) * (xs2 / 8) + j * 2 + 1) * 64), 0, 1);

                        k = ((i * 2 + 0) * (xs2 / 8) + j * 2 + 0);
                        ctx.block.EncodeBlock(ctx, new ShortPtr(ctx.ydb, k * 64), 0, 1, 0, k, n*4);
                        k = ((i * 2 + 0) * (xs2 / 8) + j * 2 + 1);
                        ctx.block.EncodeBlock(ctx, new ShortPtr(ctx.ydb, k * 64), 0, 1, 0, k, n*4);
                        k = ((i * 2 + 1) * (xs2 / 8) + j * 2 + 0);
                        ctx.block.EncodeBlock(ctx, new ShortPtr(ctx.ydb, k * 64), 0, 1, 0, k, n*4);
                        k = ((i * 2 + 1) * (xs2 / 8) + j * 2 + 1);
                        ctx.block.EncodeBlock(ctx, new ShortPtr(ctx.ydb, k * 64), 0, 1, 0, k, n*4);

                        k = i * (xs3 / 8) + j;
                        ctx.block.EncodeBlock(ctx, new ShortPtr(ctx.udb, k * 64), 2, 3, 1, k, n);
                        ctx.block.EncodeBlock(ctx, new ShortPtr(ctx.vdb, k * 64), 2, 3, 2, k, n);
                        if (ctx.jpg_yuvw)
                        {
                            ctx.block.EncodeBlock(ctx, new ShortPtr(ctx.wdb, k * 64), 2, 3, 3, k, n);
                        }
                    }
            }
            else
            {
                j = (xs2 / 8) * (ys2 / 8);
                for (i = 0; i < j; i++)
                {
                    ctx.block.EncodeBlock(ctx, new ShortPtr(ctx.ydb, i * 64), 0, 1, 0, i, j);
                    ctx.block.EncodeBlock(ctx, new ShortPtr(ctx.udb, i * 64), 2, 3, 1, i, j);
                    ctx.block.EncodeBlock(ctx, new ShortPtr(ctx.vdb, i * 64), 2, 3, 2, i, j);
                    if (ctx.jpg_yuvw)
                    {
                        ctx.block.EncodeBlock(ctx, new ShortPtr(ctx.wdb, i * 64), 2, 3, 3, i, j);
                    }
                }
            }

            ctx.huff.FlushBits(ctx);

            if ((qfl & PDJPG_QFL_NOSOI) == 0)
            {
                ctx.huff.ct.emit(0xFF);
                ctx.huff.ct.emit(JPG.JPG_EOI);
            }

            //	printf("M3\n");

            i = ctx.huff.ct - obuf;

            return (i);
        }

#if true
        public static int PDJPG_EncodeCtx_MegablockInner(
            PDJPG_Context ctx, BytePtr obuf, int xs, int ys, int qfl)
        {
            //	static BytePtr yb=null, *ub, *vb;
            //	static short *ydb=null, *udb, *vdb;
            //	static int lxs=0, lys=0;

            IntPtr dcs = new IntPtr(256);
            IntPtr acs = new IntPtr(256);
            IntPtr dcsuv = new IntPtr(256);
            IntPtr acsuv = new IntPtr(256);
            ShortPtr tp;
            int xs2, ys2, xs3, ys3, qf;
            //	int cr, cg, cb, cy, cu, cv;
            int i, j, k, l;

            qf = qfl & 255;

            xs2 = ctx.xs2;
            ys2 = ctx.ys2;
            xs3 = ctx.xs3;
            ys3 = ctx.ys3;

            //	printf("M1\n");

            //	if(qf==110)
            if (ctx.jpg_rdct)
            {
                //		printf("PDJPG_EncodeCtx: Lossless Detect\n");

                PDJPG_FilterImageMegablockRDCT(new ShortPtr(ctx.yb), new ShortPtr(ctx.ydb), xs2, ys2, 128);
                PDJPG_FilterImageMegablockRDCT(new ShortPtr(ctx.ub), new ShortPtr(ctx.udb), xs3, ys3, 128);
                PDJPG_FilterImageMegablockRDCT(new ShortPtr(ctx.vb), new ShortPtr(ctx.vdb), xs3, ys3, 128);
            }
            else
            {
                PDJPG_FilterImageMegablockRDCT(new ShortPtr(ctx.yb), new ShortPtr(ctx.ydb), xs2, ys2, 128);
                PDJPG_FilterImageMegablockRDCT(new ShortPtr(ctx.ub), new ShortPtr(ctx.udb), xs3, ys3, 128);
                PDJPG_FilterImageMegablockRDCT(new ShortPtr(ctx.vb), new ShortPtr(ctx.vdb), xs3, ys3, 128);
                
                //PDJPG_FilterImageDCT(new ShortPtr(ctx.yb), new ShortPtr(ctx.ydb), xs2, ys2, 128);
                //PDJPG_FilterImageDCT(new ShortPtr(ctx.ub), new ShortPtr(ctx.udb), xs3, ys3, 128);
                //PDJPG_FilterImageDCT(new ShortPtr(ctx.vb), new ShortPtr(ctx.vdb), xs3, ys3, 128);
            }

            j = (xs2 / 64) * (ys2 / 64);
            PDJPG_MakeMegaQuantTabInputY(ctx.ydb, j, new BytePtr(ctx.jpg_qt, 0 * 256), qf / 100.0);

            j = (xs3 / 64) * (ys3 / 64);
            PDJPG_MakeMegaQuantTabInputUV(ctx.udb, j, new BytePtr(ctx.jpg_qt, 1 * 256), qf / 100.0);
            PDJPG_MakeMegaQuantTabInputUV(ctx.vdb, j, new BytePtr(ctx.jpg_qt, 2 * 256), qf / 100.0);
            for (i = 0; i < 128; i++) ctx.jpg_qt[1 * 256 + i] = (byte)((ctx.jpg_qt[1 * 256 + i] + ctx.jpg_qt[2 * 256 + i]) / 2);

            PDJPG_SetupQuantTabDivFP(ctx, 0);
            PDJPG_SetupQuantTabDivFP(ctx, 1);

            //	free(yb);
            //	free(ub);
            //	free(vb);


            if (ctx.jpg_mono || !ctx.jpg_is420)
            {
                j = (xs2 / 64) * (ys2 / 64); k = 0;
                for (i = 0; i < j; i++)
                {
                    ctx.block.QuantMegaBlock(ctx, new ShortPtr(ctx.ydb, i * 4096), new ShortPtr(ctx.ydb, i * 4096), 0);
                    ctx.ydb[i * 4096 + 0] -= (short)k; k = ctx.ydb[i * 4096 + 0] + k;
                }
            }

            if (!ctx.jpg_mono && ctx.jpg_is420)
            {
                l = 0;
                for (i = 0; i <= (ys3 / 64); i++)
                    for (j = 0; j < (xs3 / 64); j++)
                    {
                        tp = new ShortPtr(ctx.ydb, ((i * 2 + 0) * (xs2 / 64) + j * 2 + 0) * 4096);
                        ctx.block.QuantMegaBlock(ctx, tp, tp, 0);
                        tp[0] -= l; l = tp[0] + l;

                        tp = new ShortPtr(ctx.ydb, ((i * 2 + 0) * (xs2 / 64) + j * 2 + 1) * 4096);
                        ctx.block.QuantMegaBlock(ctx, tp, tp, 0);
                        tp[0] -= l; l = tp[0] + l;

                        tp = new ShortPtr(ctx.ydb, ((i * 2 + 1) * (xs2 / 64) + j * 2 + 0) * 4096);
                        ctx.block.QuantMegaBlock(ctx, tp, tp, 0);
                        tp[0] -= l; l = tp[0] + l;

                        tp = new ShortPtr(ctx.ydb, ((i * 2 + 1) * (xs2 / 64) + j * 2 + 1) * 4096);
                        ctx.block.QuantMegaBlock(ctx, tp, tp, 0);
                        tp[0] -= l; l = tp[0] + l;
                    }
            }

            j = (xs3 / 64) * (ys3 / 64); k = 0; l = 0;
            for (i = 0; i < j; i++)
            {
                ctx.block.QuantMegaBlock(ctx, new ShortPtr(ctx.udb, i * 4096), new ShortPtr(ctx.udb, i * 4096), 1);
                ctx.block.QuantMegaBlock(ctx, new ShortPtr(ctx.vdb, i * 4096), new ShortPtr(ctx.vdb, i * 4096), 1);
                ctx.udb[i * 4096 + 0] -= (short)k; k = ctx.udb[i * 4096 + 0] + k;
                ctx.vdb[i * 4096 + 0] -= (short)l; l = ctx.vdb[i * 4096 + 0] + l;
            }

            if (ctx.jpg_yuvw)
            {
                j = (xs3 / 64) * (ys3 / 64); k = 0;
                for (i = 0; i < j; i++)
                {
                    ctx.block.QuantMegaBlock(ctx, new ShortPtr(ctx.wdb, i * 4096), new ShortPtr(ctx.wdb, i * 4096), 1);
                    ctx.wdb[i * 4096 + 0] -= (short)k; k = ctx.wdb[i * 4096 + 0] + k;
                }
            }

            //	printf("M2\n");

            for (i = 0; i < 256; i++) dcs[i] = 0;
            for (i = 0; i < 256; i++) acs[i] = 0;
            for (i = 0; i < 256; i++) dcsuv[i] = 0;
            for (i = 0; i < 256; i++) acsuv[i] = 0;

            j = (xs2 / 64) * (ys2 / 64);
            k = (xs3 / 64) * (ys3 / 64);
            for (i = 0; i < j; i++) ctx.block.StatMegaBlock(new ShortPtr(ctx.ydb, i * 4096), dcs, acs);
            for (i = 0; i < k; i++) ctx.block.StatMegaBlock(new ShortPtr(ctx.udb, i * 4096), dcsuv, acsuv);
            for (i = 0; i < k; i++) ctx.block.StatMegaBlock(new ShortPtr(ctx.vdb, i * 4096), dcsuv, acsuv);

            if (ctx.jpg_yuvw)
            { for (i = 0; i < k; i++)ctx.block.StatMegaBlock(new ShortPtr(ctx.wdb, i * 4096), dcsuv, acsuv); }

            //	ctx.huff.BuildLengths(dcs, 256, ctx.huff.len+0*256, 16);
            //	ctx.huff.BuildLengths(acs, 256, ctx.huff.len+1*256, 16);
            //	ctx.huff.BuildLengths(dcsuv, 256, ctx.huff.len+2*256, 16);
            //	ctx.huff.BuildLengths(acsuv, 256, ctx.huff.len+3*256, 16);

            //ctx.huff.BuildLengthsAdjust(dcs, 256, new BytePtr(ctx.huff.len, 0 * 256), 16);
            //ctx.huff.BuildLengthsAdjust(acs, 256, new BytePtr(ctx.huff.len, 1 * 256), 16);
            //ctx.huff.BuildLengthsAdjust(dcsuv, 256, new BytePtr(ctx.huff.len, 2 * 256), 16);
            //ctx.huff.BuildLengthsAdjust(acsuv, 256, new BytePtr(ctx.huff.len, 3 * 256), 16);

            ctx.huff.BuildTable(dcs, 0);
            ctx.huff.BuildTable(acs, 1);
            ctx.huff.BuildTable(dcsuv, 2);
            ctx.huff.BuildTable(acsuv, 3);

            //ctx.huff.ct = obuf;
            //ctx.huff.win = 0;
            //ctx.huff.pos = 0;
            ctx.huff.InitOutputStream(ctx, obuf);

            if ((qfl & PDJPG_QFL_NOSOI) == 0)
            {
                ctx.huff.ct.emit(0xFF);
                ctx.huff.ct.emit(JPG.JPG_SOI);
            }

            PDJPG_EmitMarkerBCST(ctx);

            PDJPG_EmitDQT(ctx, 0);
            if (!ctx.jpg_mono) PDJPG_EmitDQT(ctx, 1);

            PDJPG_EmitSOF(ctx, xs, ys);

            ctx.huff.EmitDHT(ctx, 0);
            ctx.huff.EmitDHT(ctx, 1);
            if (!ctx.jpg_mono)
            {
                ctx.huff.EmitDHT(ctx, 2);
                ctx.huff.EmitDHT(ctx, 3);
            }

            PDJPG_EmitSOS(ctx);
            
            ctx.huff.InitOutputStream(ctx, ctx.huff.ct);

            //ctx.huff.win = 0;
            //ctx.huff.pos = 0;

            if (ctx.jpg_mono)
            {
                j = (xs2 / 64) * (ys2 / 64);
                for (i = 0; i < j; i++) ctx.block.EncodeMegaBlock(ctx, new ShortPtr(ctx.ydb, i * 4096), 0, 1);
            }
            else if (ctx.jpg_is420)
            {
                for (i = 0; i <= (ys3 / 64); i++)
                    for (j = 0; j < (xs3 / 64); j++)
                    {
                        ctx.block.EncodeMegaBlock(ctx,
                            new ShortPtr(ctx.ydb, ((i * 2 + 0) * (xs2 / 64) + j * 2 + 0) * 4096), 0, 1);
                        ctx.block.EncodeMegaBlock(ctx,
                            new ShortPtr(ctx.ydb, ((i * 2 + 0) * (xs2 / 64) + j * 2 + 1) * 4096), 0, 1);
                        ctx.block.EncodeMegaBlock(ctx,
                            new ShortPtr(ctx.ydb, ((i * 2 + 1) * (xs2 / 64) + j * 2 + 0) * 4096), 0, 1);
                        ctx.block.EncodeMegaBlock(ctx,
                            new ShortPtr(ctx.ydb, ((i * 2 + 1) * (xs2 / 64) + j * 2 + 1) * 4096), 0, 1);

                        k = i * (xs3 / 64) + j;
                        ctx.block.EncodeMegaBlock(ctx, new ShortPtr(ctx.udb, k * 4096), 2, 3);
                        ctx.block.EncodeMegaBlock(ctx, new ShortPtr(ctx.vdb, k * 4096), 2, 3);
                        if (ctx.jpg_yuvw)
                        { ctx.block.EncodeMegaBlock(ctx, new ShortPtr(ctx.wdb, k * 4096), 2, 3); }
                    }
            }
            else
            {
                j = (xs2 / 64) * (ys2 / 64);
                for (i = 0; i < j; i++)
                {
                    ctx.block.EncodeMegaBlock(ctx, new ShortPtr(ctx.ydb, i * 4096), 0, 1);
                    ctx.block.EncodeMegaBlock(ctx, new ShortPtr(ctx.udb, i * 4096), 2, 3);
                    ctx.block.EncodeMegaBlock(ctx, new ShortPtr(ctx.vdb, i * 4096), 2, 3);
                    if (ctx.jpg_yuvw)
                    { ctx.block.EncodeMegaBlock(ctx, new ShortPtr(ctx.wdb, i * 4096), 2, 3); }
                }
            }

            ctx.huff.FlushBits(ctx);

            if ((qfl & PDJPG_QFL_NOSOI) == 0)
            {
                ctx.huff.ct.emit(0xFF);
                ctx.huff.ct.emit(JPG.JPG_EOI);
            }

            //	printf("M3\n");

            i = ctx.huff.ct - obuf;

            return (i);
        }
#endif

        public static void PDJPG_FreeContext(PDJPG_Context ctx)
        {
#if false
	if(ctx.yb)
	{
		free(ctx.yb);
		free(ctx.ub);
		free(ctx.vb);
		free(ctx.ydb);
		free(ctx.udb);
		free(ctx.vdb);
	}

	if(ctx.jpg_imgbuf)
		free(ctx.jpg_imgbuf);

	if(ctx.imgbuf_rgba)
		free(ctx.imgbuf_rgba);
	if(ctx.imgbuf_norm)
		free(ctx.imgbuf_norm);
	if(ctx.imgbuf_spec)
		free(ctx.imgbuf_spec);
	if(ctx.imgbuf_luma)
		free(ctx.imgbuf_luma);
	if(ctx.imgbuf_lebe)
		free(ctx.imgbuf_lebe);

	free(ctx);
#endif
        }

        public static int PDJPG_Encode(
            byte[] ibuf, BytePtr obuf, int xs, int ys, int qf)
        {
            PDJPG_Context ctx;
            int sz;

            ctx = PDJPG_AllocContext();
            sz = PDJPG_EncodeCtx(ctx, ibuf, obuf, xs, ys, qf, 0);
            PDJPG_FreeContext(ctx);
            return (sz);
        }

        public static void PDJPG_FlushEncodeFast(PDJPG_Context ctx)
        {
            //	if(!ctx)return;
            ctx.jpg_tabcacheframe = 0;	//force rebuild
            ctx.oldAlphaClr = 0;
        }

        public static void PDJPG_SetContextAlphaColor(PDJPG_Context ctx,
            int cr, int cg, int cb, int ca)
        {
            int cy, cu, cv;
            //	if(!ctx)return;

            if (ca < 1)
            {
                ctx.alphaClr = 0;
                return;
            }

            ctx.alphaClr = cr + (cg << 8) + (cb << 16) + (ca << 24);

            cy = 19595 * cr + 38470 * cg + 7471 * cb;
            cu = -11056 * cr - 21712 * cg + 32768 * cb;
            cv = 32768 * cr - 27440 * cg - 5328 * cb;
            ctx.alphaClrY = (byte)(cy >> 16);
            ctx.alphaClrU = (byte)((cu >> 16) + 128);
            ctx.alphaClrV = (byte)((cv >> 16) + 128);
            ctx.alphaClrA = (byte)ca;
        }

#if false
//static void pdjpg_getPixel2_RGBA(BytePtr rgb,
//		int *ra, int *ga, int *ba,
//		int *rb, int *gb, int *bb)
//{
//	*ra=rgb[0]; *ga=rgb[1]; *ba=rgb[2];
//	*ra=rgb[4]; *ga=rgb[5]; *ba=rgb[6];
//}
#endif

#if false
        public static int PDJPG_EncodeFastCtx(PDJPG_Context ctx,
            BytePtr ibuf, BytePtr obuf, int xs, int ys, int qf, int pf)
        {
            char[] tb = new char[256];
            ShortPtr tp;
            int xs2, ys2, xs3, ys3;
            BytePtr ctt;

            int i, j, k, l;

            xs2 = ((xs + 7) / 8) * 8;
            ys2 = ((ys + 7) / 8) * 8;
            xs3 = ((xs + 15) / 16) * 8;
            ys3 = ((ys + 15) / 16) * 8;

            ctx.jpg_mono = ctx.huff.;

            if (ctx.jpg_tabcacheframe == 0)
            {
                //full quality
                for (i = 0; i < 64; i++) ctx.jpg_qt[0 * 64 + i] = 1;
                for (i = 0; i < 64; i++) ctx.jpg_qt[1 * 64 + i] = 1;
            }

            if ((ctx.yb == null) || (xs != ctx.lxs) || (ys != ctx.lys))
            {
#if false
		if(ctx.yb)
		{
			free(ctx.yb);
			free(ctx.ub);
			free(ctx.vb);
			free(ctx.ydb);
			free(ctx.udb);
			free(ctx.vdb);
		}
#endif

                ctx.yb = new short[xs2 * ys2];
                ctx.ub = new short[xs2 * ys2];
                ctx.vb = new short[xs2 * ys2];

                ctx.ydb = new short[(xs2 + 8) * (ys2 + 16)];
                ctx.udb = new short[(xs3 + 8) * (ys3 + 8)];
                ctx.vdb = new short[(xs3 + 8) * (ys3 + 8)];

                ctx.lxs = xs;
                ctx.lys = ys;

                ShortPtr.memset(ctx.yb, 128, xs2 * ys2);
                ShortPtr.memset(ctx.ub, 128, xs2 * ys2);
                ShortPtr.memset(ctx.vb, 128, xs2 * ys2);

                ShortPtr.memset(ctx.ydb, 0, xs2 * (ys2 + 8));
                ShortPtr.memset(ctx.udb, 0, xs3 * (ys3 + 8));
                ShortPtr.memset(ctx.vdb, 0, xs3 * (ys3 + 8));

                ctx.jpg_tabcacheframe = 0;	//force rebuild
            }

            PDJPG_ConvertImageYUV(ctx,
                ibuf, xs, ys, pf,
                xs2, ys2, xs3, ys3);

            PDJPG_FilterImageDCT(new ShortPtr(ctx.yb), new ShortPtr(ctx.ydb), xs2, ys2, 128);
            PDJPG_FilterImageDCT(new ShortPtr(ctx.ub), new ShortPtr(ctx.udb), xs3, ys3, 128);
            PDJPG_FilterImageDCT(new ShortPtr(ctx.vb), new ShortPtr(ctx.vdb), xs3, ys3, 128);

            if (ctx.jpg_tabcacheframe <= 0)
            {
                j = (xs2 / 8) * (ys2 / 8);
                PDJPG_MakeQuantTabInputFast(ctx.ydb, j, new BytePtr(ctx.jpg_qt, 0 * 64), qf / 100.0);

                j = (xs3 / 8) * (ys3 / 8);
                PDJPG_MakeQuantTabInputFast(ctx.udb, j, new BytePtr(ctx.jpg_qt, 1 * 64), qf / 100.0);
                PDJPG_MakeQuantTabInputFast(ctx.vdb, j, new BytePtr(ctx.jpg_qt, 2 * 64), qf / 100.0);
                for (i = 0; i < 64; i++)
                    ctx.jpg_qt[1 * 64 + i] = (byte)((ctx.jpg_qt[1 * 64 + i] + ctx.jpg_qt[2 * 64 + i]) / 2);

                PDJPG_SetupQuantTabDivFP(ctx, 0);
                PDJPG_SetupQuantTabDivFP(ctx, 1);
            }

            if (ctx.jpg_mono)
            {
                j = (xs2 / 8) * (ys2 / 8); k = 0;
                for (i = 0; i < j; i++)
                {
                    ctx.huff.QuantBlock(ctx, new ShortPtr(ctx.ydb, i * 64), new ShortPtr(ctx.ydb, i * 64), 0);
                    ctx.ydb[i * 64 + 0] -= (short)k;
                    k = ctx.ydb[i * 64 + 0] + k;
                }
            }

            if (!ctx.jpg_mono)
            {
                l = 0;
                for (i = 0; i <= (ys3 / 8); i++)
                    for (j = 0; j < (xs3 / 8); j++)
                    {
                        tp = new ShortPtr(ctx.ydb, ((i * 2 + 0) * (xs2 / 8) + j * 2 + 0) * 64);
                        ctx.huff.QuantBlock(ctx, tp, tp, 0);
                        tp[0] -= l; l = tp[0] + l;

                        tp = new ShortPtr(ctx.ydb, ((i * 2 + 0) * (xs2 / 8) + j * 2 + 1) * 64);
                        ctx.huff.QuantBlock(ctx, tp, tp, 0);
                        tp[0] -= l; l = tp[0] + l;

                        tp = new ShortPtr(ctx.ydb, ((i * 2 + 1) * (xs2 / 8) + j * 2 + 0) * 64);
                        ctx.huff.QuantBlock(ctx, tp, tp, 0);
                        tp[0] -= l; l = tp[0] + l;

                        tp = new ShortPtr(ctx.ydb, ((i * 2 + 1) * (xs2 / 8) + j * 2 + 1) * 64);
                        ctx.huff.QuantBlock(ctx, tp, tp, 0);
                        tp[0] -= l; l = tp[0] + l;
                    }
            }

            j = (xs3 / 8) * (ys3 / 8); k = 0; l = 0;
            for (i = 0; i < j; i++)
            {
                ctx.huff.QuantBlock(ctx, new ShortPtr(ctx.udb, i * 64), new ShortPtr(ctx.udb, i * 64), 1);
                ctx.huff.QuantBlock(ctx, new ShortPtr(ctx.vdb, i * 64), new ShortPtr(ctx.vdb, i * 64), 1);
                ctx.udb[i * 64 + 0] -= (short)k; k = ctx.udb[i * 64 + 0] + k;
                ctx.vdb[i * 64 + 0] -= (short)l; l = ctx.vdb[i * 64 + 0] + l;
            }

            //	printf("M2\n");

            if (ctx.jpg_tabcacheframe <= 0)
            {
                for (i = 0; i < 256; i++) ctx.dcs[i] = 1;
                for (i = 0; i < 256; i++) ctx.acs[i] = 1;
                for (i = 0; i < 256; i++) ctx.dcsuv[i] = 1;
                for (i = 0; i < 256; i++) ctx.acsuv[i] = 1;

                j = (xs2 / 8) * (ys2 / 8);
                k = (xs3 / 8) * (ys3 / 8);
                for (i = 0; i < j; i++)
                    ctx.huff.StatBlock(new ShortPtr(ctx.ydb, i * 64), new IntPtr(ctx.dcs), new IntPtr(ctx.acs));
                for (i = 0; i < k; i++)
                    ctx.huff.StatBlock(new ShortPtr(ctx.udb, i * 64), new IntPtr(ctx.dcsuv), new IntPtr(ctx.acsuv));
                for (i = 0; i < k; i++)
                    ctx.huff.StatBlock(new ShortPtr(ctx.vdb, i * 64), new IntPtr(ctx.dcsuv), new IntPtr(ctx.acsuv));

                //		ctx.huff.BuildLengths(ctx.dcs, 256, ctx.huff.len+0*256, 16);
                //		ctx.huff.BuildLengths(ctx.acs, 256, ctx.huff.len+1*256, 16);
                //		ctx.huff.BuildLengths(ctx.dcsuv, 256, ctx.huff.len+2*256, 16);
                //		ctx.huff.BuildLengths(ctx.acsuv, 256, ctx.huff.len+3*256, 16);

                ctx.huff.BuildLengthsAdjust(
                    new IntPtr(ctx.dcs), 256, new BytePtr(ctx.huff.len, 0 * 256), 16);
                ctx.huff.BuildLengthsAdjust(
                    new IntPtr(ctx.acs), 256, new BytePtr(ctx.huff.len, 1 * 256), 16);
                ctx.huff.BuildLengthsAdjust(
                    new IntPtr(ctx.dcsuv), 256, new BytePtr(ctx.huff.len, 2 * 256), 16);
                ctx.huff.BuildLengthsAdjust(
                    new IntPtr(ctx.acsuv), 256, new BytePtr(ctx.huff.len, 3 * 256), 16);
            }

            ctx.huff.ct = obuf;
            ctx.huff.win = 0;
            ctx.huff.pos = 0;

            ctx.huff.ct.emit(0xFF);
            ctx.huff.ct.emit(JPG.JPG_SOI);

            if ((ctx.alphaClr != 0) && (ctx.alphaClr != ctx.oldAlphaClr))
            {
                //		sprintf_s(tb, "%d %d %d %d",
                //			(ctx.alphaClr&0xFF),
                //			((ctx.alphaClr>>8)&0xFF),
                //			((ctx.alphaClr>>16)&0xFF),
                //			((ctx.alphaClr>>24)&0xFF));

                ctx.huff.ct.emit(0xFF);
                ctx.huff.ct.emit(JPG.JPG_APP11);
                ctt = ctx.huff.ct;
                ctx.huff.ct.emit(0x00);
                ctx.huff.ct.emit(0x00);
                ctx.huff.WriteString(ctx, "AlphaColor");
                //		ctx.huff.WriteString(ctx, tb);
                ctx.huff.WriteString(ctx, "");
                i = (ctx.huff.ct - ctt);
                ctt[0] = (i >> 8) & 0xFF; ctt[1] = i & 0xFF;
            }


            if (ctx.jpg_tabcacheframe <= 0)
            {
                PDJPG_EmitDQT(ctx, 0);
                if (!ctx.jpg_mono) PDJPG_EmitDQT(ctx, 1);
            }

            PDJPG_EmitSOF(ctx, xs, ys);

            //	if(ctx.jpg_tabcacheframe<=0)
            if (true)
            {
                ctx.huff.EmitDHT(ctx, 0);
                ctx.huff.EmitDHT(ctx, 1);
                if (!ctx.jpg_mono)
                {
                    ctx.huff.EmitDHT(ctx, 2);
                    ctx.huff.EmitDHT(ctx, 3);
                }
            }

            PDJPG_EmitSOS(ctx);

            ctx.huff.win = 0;
            ctx.huff.pos = 0;

            if (ctx.jpg_mono)
            {
                j = (xs2 / 8) * (ys2 / 8);
                for (i = 0; i < j; i++) ctx.huff.EncodeBlock(ctx, new ShortPtr(ctx.ydb, i * 64), 0, 1);
            }
            else
            {
                for (i = 0; i <= ((ys3) / 8); i++)
                    for (j = 0; j < (xs3 / 8); j++)
                    {
                        ctx.huff.EncodeBlock(ctx,
                            new ShortPtr(ctx.ydb, ((i * 2 + 0) * (xs2 / 8) + j * 2 + 0) * 64), 0, 1);
                        ctx.huff.EncodeBlock(ctx,
                            new ShortPtr(ctx.ydb, ((i * 2 + 0) * (xs2 / 8) + j * 2 + 1) * 64), 0, 1);
                        ctx.huff.EncodeBlock(ctx,
                            new ShortPtr(ctx.ydb, ((i * 2 + 1) * (xs2 / 8) + j * 2 + 0) * 64), 0, 1);
                        ctx.huff.EncodeBlock(ctx,
                            new ShortPtr(ctx.ydb, ((i * 2 + 1) * (xs2 / 8) + j * 2 + 1) * 64), 0, 1);

                        k = i * (xs3 / 8) + j;
                        ctx.huff.EncodeBlock(ctx, new ShortPtr(ctx.udb, k * 64), 2, 3);
                        ctx.huff.EncodeBlock(ctx, new ShortPtr(ctx.vdb, k * 64), 2, 3);
                    }
            }

            ctx.huff.FlushBits(ctx);

            ctx.huff.ct.emit(0xFF);
            ctx.huff.ct.emit(JPG.JPG_EOI);

            if (ctx.jpg_tabcacheframe <= 0)
            {
                //		ctx.jpg_tabcacheframe=16;
                ctx.jpg_tabcacheframe = 8;
            }
            else
            {
                ctx.jpg_tabcacheframe--;
            }

            //	printf("M3\n");

            i = ctx.huff.ct - obuf;

            return (i);
        }

        static PDJPG_Context st_ctx = null;

        public static int PDJPG_EncodeFast(
            BytePtr ibuf, BytePtr obuf, int xs, int ys, int qf)
        {
            int sz;

            if (st_ctx == null) st_ctx = PDJPG_AllocContext();
            sz = PDJPG_EncodeFastCtx(st_ctx, ibuf, obuf, xs, ys, qf, 0);
            //	PDJPG_FreeContext(ctx);
            return (sz);
        }
#endif

        public static BytePtr PDJPG_EmitComponentLayer(PDJPG_Context ctx,
            BytePtr ct, string name)
        {
            BytePtr ctt;
            int i;

            //ctx.huff.ct = ct; ctx.huff.win = 0; ctx.huff.pos = 0;
            //ctx.huff.InitOutputStream(ctx, ct);

            ct.emit(0xFF);
            ct.emit(JPG.JPG_APP11);
            ctt = ct;
            ct.emit(0x00);
            ct.emit(0x00);
            ct.EmitString("CompLayer");
            ct.EmitString(name);
            i = ct - ctt;
            ctt[0] = (i >> 8) & 0xFF; ctt[1] = i & 0xFF;
            return (ct);
        }

        public static BytePtr PDJPG_EmitTagLayer(PDJPG_Context ctx, 
            BytePtr ct, string name)
        {
            BytePtr ctt;
            int i;

            //ctx.huff.ct = ct; ctx.huff.win = 0; ctx.huff.pos = 0;
            //ctx.huff.InitOutputStream(ctx, ct);

            ct.emit(0xFF);
            ct.emit(JPG.JPG_APP11);
            ctt = ct;
            ct.emit(0x00);
            ct.emit(0x00);
            ct.EmitString("TagLayer");
            ct.EmitString(name);
            i = ct - ctt;
            ctt[0] = (i >> 8) & 0xFF; ctt[1] = i & 0xFF;
            return (ct);
        }

        public static BytePtr PDJPG_EmitMarkerJFIF(PDJPG_Context ctx, BytePtr ct)
        {
            BytePtr ctt;
            int i;

            if (ctx.jpg_clrtrans != PDJPG_CLRS_YCBCR)
                return (ct);

            //ctx.huff.ct = ct; ctx.huff.win = 0; ctx.huff.pos = 0;
            //ctx.huff.InitOutputStream(ctx, ct);

            ct.emit(0xFF);
            ct.emit(JPG.JPG_APP0);
            ctt = ct;
            ct.emit(0x00);
            ct.emit(0x00);
            ct.EmitString("JFIF");
            //PDJPG_WriteString(ctx, "JFIF");

            ct.emit(0x01);		//version high
            ct.emit(0x02);		//version low

            ct.emit(0x00);		//no units

            ct.emit(0x00);		//X density
            ct.emit(0x01);

            ct.emit(0x00);		//Y density
            ct.emit(0x01);

            ct.emit(0x00);		//thumbnail
            ct.emit(0x00);

            //	ctx.huff.WriteString(ctx, name);
            i = ct - ctt;
            ctt[0] = (i >> 8) & 0xFF; ctt[1] = i & 0xFF;
            return (ct);
        }

        public static int PDJPG_EscapeEncodeBuffer(BytePtr ibuf, int isz,
            BytePtr obuf, int osz)
        {
            BytePtr cs, ct, cse, cte;

            cs = ibuf; cse = ibuf + isz;
            ct = obuf; cte = obuf + osz;

            while ((cs < cse) && (ct < cte))
            {
                if (cs[0] == 0xFF)
                {
                    cs++;
                    ct.emit(0xFF);
                    ct.emit(0x00);
                    continue;
                }

                ct.emit(cs.next());
            }

            if (ct >= cte) return (-1);
            return (ct - obuf);
        }

        public static int PDJPG_EscapeEncodeSingleBuffer(BytePtr buf, int sz)
        {
            BytePtr tbuf;
            int i;

            tbuf = new BytePtr(sz * 2);
            i = PDJPG_EscapeEncodeBuffer(buf, sz, tbuf, sz * 2);
            if (i < 0) return (i);
            BytePtr.memcpy(buf, tbuf, i);
            //	free(tbuf);
            return (i);
        }

        public static int PDJPG_EscapeDecodeBuffer(BytePtr ibuf, int isz,
            BytePtr obuf, int osz)
        {
            BytePtr cs, ct, cse, cte;

            cs = ibuf; cse = ibuf + isz;
            ct = obuf; cte = obuf + osz;

            while ((cs < cse) && (ct < cte))
            {
                if ((cs[0] == 0xFF) && (cs[1] == 0x00))
                { cs += 2; ct.emit(0xFF); continue; }
                if ((cs[0] == 0xFF) && (cs[1] <= 0x0F))
                { ct.emit(0xFF); ct.emit(cs[1] - 1); cs += 2; continue; }
                ct.emit(cs.next());
            }
            if (ct >= cte) return (-1);
            return (ct - obuf);
        }

        public static int PDJPG_EscapeDecodeSingleBuffer(BytePtr buf, int sz)
        {
            BytePtr cs, ct, cse, cte;

            cs = buf; cse = buf + sz;
            ct = buf; cte = buf + sz;

            while ((cs < cse) && (ct < cte))
            {
                if ((cs[0] == 0xFF) && (cs[1] == 0x00))
                { cs += 2; ct.emit(0xFF); continue; }
                if ((cs[0] == 0xFF) && (cs[1] <= 0x0F))
                { ct.emit(0xFF); ct.emit(cs[1] - 1); cs += 2; continue; }
                ct.emit(cs.next());
            }
            if (ct >= cte) return (-1);
            return (ct - buf);
        }

        public static int PDJPG_EncodeLDatCtx(
            PDJPG_Context ctx, byte[] ibuf, BytePtr obuf,
            int xs, int ys, int qf, int pf)
        {
            BytePtr tbuf, cts;
            int i, j, sz, tsz;

            //	if(!ctx)ctx=PDJPG_AllocContext();

            tbuf = new BytePtr(1 << 20);
            sz = PDJPG_EncodeCtx(ctx, ibuf, tbuf, xs, ys, qf, pf);

            sz = PDJPG_EscapeEncodeSingleBuffer(tbuf, sz);

            cts = tbuf; tsz = sz;
            //ctx.huff.ct = obuf; ctx.huff.win = 0; ctx.huff.pos = 0;
            ctx.huff.InitOutputStream(ctx, obuf);

            while (tsz >= 65528)
            {
                i = 65529;
                if (cts[i - 1] == 0xFF) i--;
                j = i + 6;
                ctx.huff.ct.emit(0xFF);
                ctx.huff.ct.emit(JPG.JPG_APP12);
                ctx.huff.ct.emit((j >> 8) & 0xFF);
                ctx.huff.ct.emit(j & 0xFF);

                ctx.huff.ct.emit('L');
                ctx.huff.ct.emit('D');
                ctx.huff.ct.emit('A');
                ctx.huff.ct.emit('T');

                BytePtr.memcpy(ctx.huff.ct, cts, i);
                cts += i; ctx.huff.ct += i;
                tsz -= i;
            }

            if ((tsz > 0) && (tsz < 65528))
            {
                i = tsz + 6;
                ctx.huff.ct.emit(0xFF);
                ctx.huff.ct.emit(JPG.JPG_APP12);
                ctx.huff.ct.emit((i >> 8) & 0xFF);
                ctx.huff.ct.emit(i & 0xFF);

                ctx.huff.ct.emit('L');
                ctx.huff.ct.emit('D');
                ctx.huff.ct.emit('A');
                ctx.huff.ct.emit('T');

                BytePtr.memcpy(ctx.huff.ct, cts, tsz);
                ctx.huff.ct += tsz;
            }

            sz = ctx.huff.ct - obuf;
            //free(tbuf);

            return (sz);
        }

        public static int PDJPG_EncodeBaseCtx(
            PDJPG_Context ctx, byte[] ibuf, BytePtr obuf,
            int xs, int ys, int qf, int pf)
        {
            BytePtr tbuf, cs, cse, ct;
            int sz;

            tbuf = new BytePtr(1 << 24);
            sz = PDJPG_EncodeCtx(ctx, ibuf, tbuf, xs, ys, qf, pf);

            cs = tbuf; cse = tbuf + sz; ct = obuf;
            if ((cs[0] == 0xFF) && (cs[1] == JPG.JPG_SOI))
                cs += 2;
            while (cs < cse)
            {
                if ((cs[0] == 0xFF) && (cs[1] == JPG.JPG_EOI))
                    break;
                ct.emit(cs.next());
            }

            //free(tbuf);
            return (ct - obuf);
        }

        public static int PDJPG_EncodeComponentCtx(
            PDJPG_Context ctx,
            byte[] rgba, byte[] norm, byte[] spec, byte[] luma,
            BytePtr obuf, int xs, int ys, int qf)
        {
            return (PDJPG_EncodeComponentCtxI(ctx,
                rgba, norm, spec, luma, null,
                obuf, xs, ys, qf));
        }

        public static BytePtr PDJPG_EncodeBeginLayeredCtx(
            PDJPG_Context ctx, BytePtr ct, int xs, int ys, int qf)
        {
            ct.emit(0xFF);
            ct.emit(JPG.JPG_SOI);

            ct = PDJPG_EmitMarkerJFIF(ctx, ct);
            return ct;
        }

        public static BytePtr PDJPG_EncodeEndLayeredCtx(
            PDJPG_Context ctx, BytePtr ct)
        {
            ct.emit(0xFF);
            ct.emit(JPG.JPG_EOI);
            return ct;
        }

        public static int PDJPG_EncodeComponentCtxI(PDJPG_Context ctx,
            byte[] rgba, byte[] norm, byte[] spec, byte[] luma, byte[] lebe,
            BytePtr obuf, int xs, int ys, int qf)
        {
            int i;
            BytePtr ct;

            ct = obuf;

            ct = PDJPG_EncodeBeginLayeredCtx(ctx, ct, xs, ys, qf);
            i = PDJPG_EncodeComponentCtxI2(ctx, rgba, norm, spec, luma, lebe, ct, xs, ys, qf, null, null);
            ct += i;

            ct = PDJPG_EncodeEndLayeredCtx(ctx, ct);

            //free(tbuf);
            return (ct - obuf);
        }

        public static int PDJPG_EncodeComponentCtxI2(
            PDJPG_Context ctx,
            byte[] rgba, byte[] norm, byte[] spec, byte[] luma, byte[] lebe,
            BytePtr obuf, int xs, int ys, int qf, string tlname, PDJPG_LayerInfo linf)
        {
            byte[] tbuf;
            BytePtr ct;
            int i, j, k, n;

            if (rgba == null) return (-1);

            if (linf != null)
            {
                ctx.bcst_orgx = linf.orgx;
                ctx.bcst_orgy = linf.orgy;
                ctx.bcst_alpha = linf.alpha;
                ctx.bcst_blend = linf.blend;
                ctx.bcst_flags = (uint)linf.flags;

                ctx.bcst_minx = linf.minx;
                ctx.bcst_miny = linf.miny;
                ctx.bcst_maxx = linf.maxx;
                ctx.bcst_maxy = linf.maxy;
            }
            else
            {
                ctx.bcst_orgx = 0;
                ctx.bcst_orgy = 0;
                ctx.bcst_alpha = 255;
                ctx.bcst_blend = 0;
                ctx.bcst_flags = 0;

                ctx.bcst_minx = 0;
                ctx.bcst_miny = 0;
                ctx.bcst_maxx = 0;
                ctx.bcst_maxy = 0;
            }

            n = xs * ys;
            tbuf = new byte[xs * ys * 4];

            ct = obuf;

            //ct.emit(0xFF);
            //ct.emit(JPG.JPG_SOI);

            //ct = PDJPG_EmitMarkerJFIF(ctx, ct);

            if (tlname != null)
            {
                ct = PDJPG_EmitTagLayer(ctx, ct, tlname);
                ct = PDJPG_EmitComponentLayer(ctx, ct, "RGB");
                i = PDJPG_EncodeLDatCtx(ctx, rgba, ct, xs, ys, qf, 0);
                if (i < 0) { return (i); }
                ct += i;
            }
            else
            {
                ct = PDJPG_EmitComponentLayer(ctx, ct, "RGB");
                //	i=PDJPG_EncodeCtx(ctx, rgba, ct, xs, ys, qf, 0);
                i = PDJPG_EncodeBaseCtx(ctx, rgba, ct, xs, ys, qf, 0);
                if (i < 0) { return (i); }
                ct += i;
            }
            //	if((ct[-2]==0xFF) && (ct[-2]==JPG_EOI))
            //		{ ct-=2; }

            if (norm != null)
            {
                for (i = 0; i < n; i++)
                {
                    tbuf[i * 4 + 0] = norm[i * 4 + 0];
                    tbuf[i * 4 + 1] = norm[i * 4 + 2];
                    tbuf[i * 4 + 2] = norm[i * 4 + 1];
                    tbuf[i * 4 + 3] = 255;
                }

                ct = PDJPG_EmitComponentLayer(ctx, ct, "XYZ");
                i = PDJPG_EncodeLDatCtx(ctx, tbuf, ct, xs, ys, qf, 0);
                if (i < 0) { return (i); }
                ct += i;
            }

            if (spec != null)
            {
                ct = PDJPG_EmitComponentLayer(ctx, ct, "SpRGB");
                i = PDJPG_EncodeLDatCtx(ctx, spec, ct, xs, ys, qf, 0);
                if (i < 0) { return (i); }
                ct += i;
            }

            if ((norm != null) || (spec != null))
            {
                for (i = 0; i < n; i++)
                {
                    j = rgba[i * 4 + 3];
                    tbuf[i * 4 + 0] = (byte)((norm != null) ? norm[i * 4 + 3] : j);
                    tbuf[i * 4 + 1] = rgba[i * 4 + 3];
                    tbuf[i * 4 + 2] = (byte)((spec != null) ? spec[i * 4 + 3] : j);
                    tbuf[i * 4 + 3] = 255;
                }

                ct = PDJPG_EmitComponentLayer(ctx, ct, "DASe");
                i = PDJPG_EncodeLDatCtx(ctx, tbuf, ct, xs, ys, qf, 0);
                if (i < 0) { return (i); }
                ct += i;
            }
            else
            {
                k = 0;
                for (i = 0; i < n; i++)
                {
                    j = rgba[i * 4 + 3];
                    if (j != 255) k = 1;
                    tbuf[i * 4 + 0] = (byte)j;
                    tbuf[i * 4 + 1] = (byte)j;
                    tbuf[i * 4 + 2] = (byte)j;
                    tbuf[i * 4 + 3] = 255;
                }

                if (k != 0)
                {
                    ct = PDJPG_EmitComponentLayer(ctx, ct, "Alpha");
                    i = PDJPG_EncodeLDatCtx(ctx, tbuf, ct, xs, ys, qf, PDJPG_YYYA);
                    if (i < 0) { return (i); }
                    ct += i;
                }
            }

            if (luma != null)
            {
                ct = PDJPG_EmitComponentLayer(ctx, ct, "LuRGB");
                i = PDJPG_EncodeLDatCtx(ctx, luma, ct, xs, ys, qf, 0);
                if (i < 0) { return (i); }
                ct += i;
            }

            if (lebe != null)
            {
                ct = PDJPG_EmitComponentLayer(ctx, ct, "LeBe");
                i = PDJPG_EncodeLDatCtx(ctx, lebe, ct, xs, ys, qf, 0);
                if (i < 0) { return (i); }
                ct += i;
            }

            //ct.emit(0xFF);
            //ct.emit(JPG.JPG_EOI);

            //free(tbuf);
            return (ct - obuf);
        }

        public static int PDJPG_EncodeComponent(
            byte[] rgba, byte[] norm, byte[] spec, byte[] luma,
            BytePtr obuf, int xs, int ys, int qf)
        {
            PDJPG_Context ctx;
            int sz;

            ctx = PDJPG_AllocContext();
            sz = PDJPG_EncodeComponentCtx(ctx,
                rgba, norm, spec, luma,
                obuf, xs, ys, qf);
            PDJPG_FreeContext(ctx);
            return (sz);
        }

        public static double pdjpg_ssqr(double f)
        { return ((f >= 0) ? (f * f) : (-f * f)); }
        public static double pdjpg_ssqrt(double f)
        { return ((f >= 0) ? Math.Sqrt(f) : (-Math.Sqrt(-f))); }

        public static int PDJPG_CalcComponentValueExponent(double val)
        {
            double f;
            int i, j;

            if (val >= 0)
            {
                f = Math.Log(val) / Math.Log(2.0);
                i = (int)(pdjpg_ssqrt(f * 240.0) + 0.5);
                if (i > 63) i = 63;
                if (i < (-63)) i = -63;
                j = 192 + i;
                return (j);
            }
            else
            {
                f = Math.Log(-val) / Math.Log(2.0);
                i = (int)(pdjpg_ssqrt(f * 240.0) + 0.5);
                if (i > 63) i = 63;
                if (i < (-63)) i = -63;
                j = 63 - i;
                return (j);
            }
        }

        public static void PDJPG_EncodeFloatPixel(FloatPtr ipix, BytePtr opix, BytePtr oexp)
        {
            double f, g;
            int i, j, k;

            if ((ipix[0] >= 0) && (ipix[0] <= 1) &&
                (ipix[1] >= 0) && (ipix[1] <= 1) &&
                (ipix[2] >= 0) && (ipix[2] <= 1))
            {
                oexp[0] = 192;
                opix[0] = (byte)(ipix[0] * 255);
                opix[1] = (byte)(ipix[1] * 255);
                opix[2] = (byte)(ipix[2] * 255);
                return;
            }

            f = ipix[1];
            if (Math.Abs(ipix[0]) > Math.Abs(f)) f = ipix[0];
            if (Math.Abs(ipix[2]) > Math.Abs(f)) f = ipix[2];

            if (f >= 0)
            {
                f = Math.Log(f) / Math.Log(2.0);
                i = (int)(pdjpg_ssqrt(f * 240) + 0.5);
                if (i > 63) i = 63;
                if (i < (-63)) i = -63;
                j = 192 + i;
                g = pdjpg_scltab[j];

                oexp[0] = (byte)j;
                f = 255 / g;
                k = (int)(ipix[0] * f); k = (k < 0) ? 0 : ((k <= 255) ? k : 255); opix[0] = (byte)k;
                k = (int)(ipix[1] * f); k = (k < 0) ? 0 : ((k <= 255) ? k : 255); opix[1] = (byte)k;
                k = (int)(ipix[2] * f); k = (k < 0) ? 0 : ((k <= 255) ? k : 255); opix[2] = (byte)k;
                return;
            }
            else
            {
                f = Math.Log(-f) / Math.Log(2.0);
                i = (int)(pdjpg_ssqrt(f * 240) + 0.5);
                if (i > 63) i = 63;
                if (i < (-63)) i = -63;
                j = 63 - i;
                g = pdjpg_scltab[j];

                oexp[0] = (byte)j;
                f = 255 / g;
                k = (int)(ipix[0] * f); k = (k < 0) ? 0 : ((k <= 255) ? k : 255); opix[0] = (byte)k;
                k = (int)(ipix[1] * f); k = (k < 0) ? 0 : ((k <= 255) ? k : 255); opix[1] = (byte)k;
                k = (int)(ipix[2] * f); k = (k < 0) ? 0 : ((k <= 255) ? k : 255); opix[2] = (byte)k;
                return;
            }
        }

        public static int PDJPG_EncodeFloatComponentCtx(PDJPG_Context ctx,
            float[] rgba, float[] norm, float[] spec, float[] luma,
            BytePtr obuf, int xs, int ys, int qf)
        {
            byte[] trgba, tnorm, tspec, tluma, tlebe;
            int i, j, n;
            double f;

            n = xs * ys;
            if (ctx.imgbuf_rgba == null)
                ctx.imgbuf_rgba = new byte[4 * n];
            if (ctx.imgbuf_norm == null)
                ctx.imgbuf_norm = new byte[4 * n];
            if (ctx.imgbuf_spec == null)
                ctx.imgbuf_spec = new byte[4 * n];
            if (ctx.imgbuf_luma == null)
                ctx.imgbuf_luma = new byte[4 * n];

            if (ctx.imgbuf_lebe == null)
            {
                ctx.imgbuf_lebe = new byte[4 * n];
                BytePtr.memset(ctx.imgbuf_lebe, 192, 4 * n);
            }

            trgba = ctx.imgbuf_rgba;
            tnorm = ctx.imgbuf_norm;
            tspec = ctx.imgbuf_spec;
            tluma = ctx.imgbuf_luma;
            tlebe = ctx.imgbuf_lebe;

            if (rgba != null)
            {
                for (i = 0; i < n; i++)
                {
                    PDJPG_EncodeFloatPixel(new FloatPtr(rgba, i * 4), new BytePtr(trgba, i * 4), new BytePtr(tlebe, i * 4 + 0));
                }
            }

            if (norm != null)
            {
                for (i = 0; i < n; i++)
                {
                    //			f=(1.0/256)*pdjpg_scltab[tlebe[i*4+0]];
                    j = PDJPG_CalcComponentValueExponent(norm[i * 4 + 3]);
                    f = 255 / pdjpg_scltab[j];
                    tnorm[i * 4 + 0] = (byte)(norm[i * 4 + 0] * 255);
                    tnorm[i * 4 + 1] = (byte)(norm[i * 4 + 1] * 255);
                    tnorm[i * 4 + 2] = (byte)(norm[i * 4 + 2] * 255);
                    tnorm[i * 4 + 3] = (byte)(norm[i * 4 + 3] * f);
                    tlebe[i * 4 + 3] = (byte)j;
                }
            }

            if (spec != null)
            {
                for (i = 0; i < n; i++)
                {
                    tspec[i * 4 + 0] = (byte)(spec[i * 4 + 0] * 255);
                    tspec[i * 4 + 1] = (byte)(spec[i * 4 + 1] * 255);
                    tspec[i * 4 + 2] = (byte)(spec[i * 4 + 2] * 255);
                    tspec[i * 4 + 3] = (byte)(spec[i * 4 + 3] * 255);
                }
            }

            if (luma != null)
            {
                for (i = 0; i < n; i++)
                {
                    PDJPG_EncodeFloatPixel(new FloatPtr(luma, i * 4), new BytePtr(tluma, i * 4), new BytePtr(tlebe, i * 4 + 1));
                }
            }

            i = PDJPG_EncodeComponentCtxI(ctx,
                trgba, tnorm, tspec, tluma, tlebe,
                obuf, xs, ys, qf);
            return (i);
        }
    }
}
