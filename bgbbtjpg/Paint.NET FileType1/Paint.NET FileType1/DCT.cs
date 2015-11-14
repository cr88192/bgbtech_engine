using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace BGBTechBTJ
{

    class PDJPG_TransDCT
    {
#if true
        // void TransDCT_Horiz(byte *iblk, int *oblk)
        public static void TransDCT_Horiz(ShortPtr iblk, IntPtr oblk, int dcbias)
        {
            int[] ib = new int[8];

            //	ib[0]=iblk[0]-128;	ib[1]=iblk[1]-128;
            //	ib[2]=iblk[2]-128;	ib[3]=iblk[3]-128;
            //	ib[4]=iblk[4]-128;	ib[5]=iblk[5]-128;
            //	ib[6]=iblk[6]-128;	ib[7]=iblk[7]-128;

            ib[0] = iblk[0] - dcbias; ib[1] = iblk[1] - dcbias;
            ib[2] = iblk[2] - dcbias; ib[3] = iblk[3] - dcbias;
            ib[4] = iblk[4] - dcbias; ib[5] = iblk[5] - dcbias;
            ib[6] = iblk[6] - dcbias; ib[7] = iblk[7] - dcbias;

            oblk[0] = ib[0] * 91 + ib[1] * 91 + ib[2] * 91 + ib[3] * 91
                    + ib[4] * 91 + ib[5] * 91 + ib[6] * 91 + ib[7] * 91;
            oblk[1] = ib[0] * 126 + ib[1] * 106 + ib[2] * 71 + ib[3] * 25
                    - ib[4] * 25 - ib[5] * 71 - ib[6] * 106 - ib[7] * 126;
            oblk[2] = ib[0] * 118 + ib[1] * 49 - ib[2] * 49 - ib[3] * 118
                    - ib[4] * 118 - ib[5] * 49 + ib[6] * 49 + ib[7] * 118;
            oblk[3] = ib[0] * 106 - ib[1] * 25 - ib[2] * 126 - ib[3] * 71
                    + ib[4] * 71 + ib[5] * 126 + ib[6] * 25 - ib[7] * 106;
            oblk[4] = ib[0] * 91 - ib[1] * 91 - ib[2] * 91 + ib[3] * 91
                    + ib[4] * 91 - ib[5] * 91 - ib[6] * 91 + ib[7] * 91;
            oblk[5] = ib[0] * 71 - ib[1] * 126 + ib[2] * 25 + ib[3] * 106
                    - ib[4] * 106 - ib[5] * 25 + ib[6] * 126 - ib[7] * 71;
            oblk[6] = ib[0] * 49 - ib[1] * 118 + ib[2] * 118 - ib[3] * 49
                    - ib[4] * 49 + ib[5] * 118 - ib[6] * 118 + ib[7] * 49;
            oblk[7] = ib[0] * 25 - ib[1] * 71 + ib[2] * 106 - ib[3] * 126
                    + ib[4] * 126 - ib[5] * 106 + ib[6] * 71 - ib[7] * 25;
        }

        public static void TransDCT_Vert(IntPtr iblk, IntPtr oblk)
        {
            oblk[0] = iblk[0] * 91 + iblk[8] * 91
                    + iblk[16] * 91 + iblk[24] * 91
                    + iblk[32] * 91 + iblk[40] * 91
                    + iblk[48] * 91 + iblk[56] * 91;
            oblk[8] = iblk[0] * 126 + iblk[8] * 106
                    + iblk[16] * 71 + iblk[24] * 25
                    - iblk[32] * 25 - iblk[40] * 71
                    - iblk[48] * 106 - iblk[56] * 126;
            oblk[16] = iblk[0] * 118 + iblk[8] * 49
                    - iblk[16] * 49 - iblk[24] * 118
                    - iblk[32] * 118 - iblk[40] * 49
                    + iblk[48] * 49 + iblk[56] * 118;
            oblk[24] = iblk[0] * 106 - iblk[8] * 25
                    - iblk[16] * 126 - iblk[24] * 71
                    + iblk[32] * 71 + iblk[40] * 126
                    + iblk[48] * 25 - iblk[56] * 106;
            oblk[32] = iblk[0] * 91 - iblk[8] * 91
                    - iblk[16] * 91 + iblk[24] * 91
                    + iblk[32] * 91 - iblk[40] * 91
                    - iblk[48] * 91 + iblk[56] * 91;
            oblk[40] = iblk[0] * 71 - iblk[8] * 126
                    + iblk[16] * 25 + iblk[24] * 106
                    - iblk[32] * 106 - iblk[40] * 25
                    + iblk[48] * 126 - iblk[56] * 71;
            oblk[48] = iblk[0] * 49 - iblk[8] * 118
                    + iblk[16] * 118 - iblk[24] * 49
                    - iblk[32] * 49 + iblk[40] * 118
                    - iblk[48] * 118 + iblk[56] * 49;
            oblk[56] = iblk[0] * 25 - iblk[8] * 71
                    + iblk[16] * 106 - iblk[24] * 126
                    + iblk[32] * 126 - iblk[40] * 106
                    + iblk[48] * 71 - iblk[56] * 25;
        }

        // void TransDCT(byte *iblk, short *oblk)
        public static void TransBlock(ShortPtr iblk, ShortPtr oblk, int dcbias)
        {
            IntPtr s = new IntPtr(PDJPG.DCTSZ2);
            IntPtr t = new IntPtr(PDJPG.DCTSZ2);
            int i, j;

            TransDCT_Horiz(iblk + 0, s + 0, dcbias);
            TransDCT_Horiz(iblk + 8, s + 8, dcbias);
            TransDCT_Horiz(iblk + 16, s + 16, dcbias);
            TransDCT_Horiz(iblk + 24, s + 24, dcbias);
            TransDCT_Horiz(iblk + 32, s + 32, dcbias);
            TransDCT_Horiz(iblk + 40, s + 40, dcbias);
            TransDCT_Horiz(iblk + 48, s + 48, dcbias);
            TransDCT_Horiz(iblk + 56, s + 56, dcbias);

            TransDCT_Vert(s + 0, t + 0);
            TransDCT_Vert(s + 1, t + 1);
            TransDCT_Vert(s + 2, t + 2);
            TransDCT_Vert(s + 3, t + 3);
            TransDCT_Vert(s + 4, t + 4);
            TransDCT_Vert(s + 5, t + 5);
            TransDCT_Vert(s + 6, t + 6);
            TransDCT_Vert(s + 7, t + 7);

            for (i = 0; i < 64; i++)
            {
                j = t[i] >> 16;
                if (((short)j)!=j)
                    throw new Exception();
                oblk[i] = j;
                //oblk[i] = t[i] >> 16;
            }
        }

#endif
    }

    class PDJPG_TransIDCT
    {
#if true
        public static void TransIDCT_Horiz(ShortPtr iblk, IntPtr oblk)
        {
            int a, b, c, d, e, f;
            int i, j, k, l;
            int m, n, o, p;

            a = iblk[0] * 91; b = iblk[4] * 91;
            c = iblk[2] * 118; d = iblk[2] * 49;
            e = iblk[6] * 118; f = iblk[6] * 49;
            i = a + c + b + f; j = a + d - b - e; k = a - d - b + e; l = a - c + b - f;
            m = iblk[1] * 126 + iblk[3] * 106 + iblk[5] * 71 + iblk[7] * 25;
            n = iblk[1] * 106 - iblk[3] * 25 - iblk[5] * 126 - iblk[7] * 71;
            o = iblk[1] * 71 - iblk[3] * 126 + iblk[5] * 25 + iblk[7] * 106;
            p = iblk[1] * 25 - iblk[3] * 71 + iblk[5] * 106 - iblk[7] * 126;
            oblk[0] = i + m; oblk[1] = j + n; oblk[2] = k + o; oblk[3] = l + p;
            oblk[4] = l - p; oblk[5] = k - o; oblk[6] = j - n; oblk[7] = i - m;
        }

        public static void TransIDCT_Vert(IntPtr iblk, IntPtr oblk)
        {
            int a, b, c, d, e, f;
            int i, j, k, l;
            int m, n, o, p;

            a = iblk[0] * 91; b = iblk[32] * 91;
            c = iblk[16] * 118; d = iblk[16] * 49;
            e = iblk[48] * 118; f = iblk[48] * 49;
            i = a + c + b + f; j = a + d - b - e; k = a - d - b + e; l = a - c + b - f;
            m = iblk[8] * 126 + iblk[24] * 106 + iblk[40] * 71 + iblk[56] * 25;
            n = iblk[8] * 106 - iblk[24] * 25 - iblk[40] * 126 - iblk[56] * 71;
            o = iblk[8] * 71 - iblk[24] * 126 + iblk[40] * 25 + iblk[56] * 106;
            p = iblk[8] * 25 - iblk[24] * 71 + iblk[40] * 106 - iblk[56] * 126;
            oblk[0] = i + m; oblk[8] = j + n; oblk[16] = k + o; oblk[24] = l + p;
            oblk[32] = l - p; oblk[40] = k - o; oblk[48] = j - n; oblk[56] = i - m;
        }

        // void TransIDCT(short *iblk, byte *oblk)
        public static void TransBlock(ShortPtr iblk, ShortPtr oblk, int dcbias)
        {
            IntPtr s = new IntPtr(PDJPG.DCTSZ2);
            IntPtr t = new IntPtr(PDJPG.DCTSZ2);
            int i, j;

            TransIDCT_Horiz(iblk + 0, s + 0);
            TransIDCT_Horiz(iblk + 8, s + 8);
            TransIDCT_Horiz(iblk + 16, s + 16);
            TransIDCT_Horiz(iblk + 24, s + 24);
            TransIDCT_Horiz(iblk + 32, s + 32);
            TransIDCT_Horiz(iblk + 40, s + 40);
            TransIDCT_Horiz(iblk + 48, s + 48);
            TransIDCT_Horiz(iblk + 56, s + 56);

            TransIDCT_Vert(s + 0, t + 0);
            TransIDCT_Vert(s + 1, t + 1);
            TransIDCT_Vert(s + 2, t + 2);
            TransIDCT_Vert(s + 3, t + 3);
            TransIDCT_Vert(s + 4, t + 4);
            TransIDCT_Vert(s + 5, t + 5);
            TransIDCT_Vert(s + 6, t + 6);
            TransIDCT_Vert(s + 7, t + 7);

            for (i = 0; i < 64; i++)
            {
                j = (t[i] >> 16) + dcbias;
                //		if(j&(~255))j=(j<0)?0:((j>255)?255:j);
                oblk[i] = j;
                //		oblk[i]=(j<0)?0:((j>255)?255:j);
            }
        }

        public static void TransIDCT16(ShortPtr iblk, ShortPtr oblk, int dcbias)
        {
            IntPtr s = new IntPtr(PDJPG.DCTSZ2);
            IntPtr t = new IntPtr(PDJPG.DCTSZ2);
            int i, j;

            TransIDCT_Horiz(iblk + 0, s + 0);
            TransIDCT_Horiz(iblk + 8, s + 8);
            TransIDCT_Horiz(iblk + 16, s + 16);
            TransIDCT_Horiz(iblk + 24, s + 24);
            TransIDCT_Horiz(iblk + 32, s + 32);
            TransIDCT_Horiz(iblk + 40, s + 40);
            TransIDCT_Horiz(iblk + 48, s + 48);
            TransIDCT_Horiz(iblk + 56, s + 56);

            for (i = 0; i < PDJPG.DCTSZ2; i++)
                s[i] >>= 4;

            TransIDCT_Vert(s + 0, t + 0);
            TransIDCT_Vert(s + 1, t + 1);
            TransIDCT_Vert(s + 2, t + 2);
            TransIDCT_Vert(s + 3, t + 3);
            TransIDCT_Vert(s + 4, t + 4);
            TransIDCT_Vert(s + 5, t + 5);
            TransIDCT_Vert(s + 6, t + 6);
            TransIDCT_Vert(s + 7, t + 7);

            for (i = 0; i < 64; i++)
            {
                j = (t[i] >> 12) + dcbias;
                //		if(j&(~255))j=(j<0)?0:((j>255)?255:j);
                oblk[i] = j;
            }
        }

#endif
    }

    class PDJPG_TransRDCT
    {
        //RDCT: Integer Reversible DCT (Lossless)

#if true
        public static void TransRDCT_Ref(
            int iblk0, int iblk1, int iblk2, int iblk3,
            int iblk4, int iblk5, int iblk6, int iblk7,
            out int oblk0, out int oblk1, out int oblk2, out int oblk3,
            out int oblk4, out int oblk5, out int oblk6, out int oblk7)
        {
            int ib0, ib1, ib2, ib3, ib4, ib5, ib6, ib7;

            ib0 = iblk2; ib1 = iblk7;
            ib2 = iblk4; ib3 = iblk3;
            ib4 = iblk6; ib5 = iblk0;
            ib6 = iblk1; ib7 = iblk5;

            //9542 -23129 4403 -4963 10017 -3117 236
            //-9117 467 -3860 843 128 -3675 -3784
            //-561 2218 -2218 -1831 2104 -2626 3147
            //-298 -14012 -8192 2512 5465 -4877 1670
            //6518 7917 3636 5057 -1165 8502 -1393
            //3761 3365 -1698 -8867 5793 7269 -2062
            //-5385 4760 -2401 -4347 -7152 -1306 -2916
            //8212 -5882 -760 -261 3416 9556 4017
            //9028 -16635 -3179 5375 10162 13580 -9760

#if true
            ib7 = (-ib7) + (((ib0 * 9542 + ib1 * -23129 + ib2 * 4403 + ib3 * -4963 + ib4 * 10017 + ib5 * -3117 + ib6 * 236) + 4096) >> 13);
            ib0 = ib0 + (((ib1 * -9117 + ib2 * 467 + ib3 * -3860 + ib4 * 843 + ib5 * 128 + ib6 * -3675 + ib7 * -3784) + 4096) >> 13);
            ib1 = ib1 + (((ib0 * -561 + ib2 * 2218 + ib3 * -2218 + ib4 * -1831 + ib5 * 2104 + ib6 * -2626 + ib7 * 3147) + 4096) >> 13);
            ib2 = ib2 + (((ib0 * -298 + ib1 * -14012 + ib3 * -8192 + ib4 * 2512 + ib5 * 5465 + ib6 * -4877 + ib7 * 1670) + 4096) >> 13);
            ib3 = ib3 + (((ib0 * 6518 + ib1 * 7917 + ib2 * 3636 + ib4 * 5057 + ib5 * -1165 + ib6 * 8502 + ib7 * -1393) + 4096) >> 13);
            ib4 = ib4 + (((ib0 * 3761 + ib1 * 3365 + ib2 * -1698 + ib3 * -8867 + ib5 * 5793 + ib6 * 7269 + ib7 * -2062) + 4096) >> 13);
            ib5 = ib5 + (((ib0 * -5385 + ib1 * 4760 + ib2 * -2401 + ib3 * -4347 + ib4 * -7152 + ib6 * -1306 + ib7 * -2916) + 4096) >> 13);
            ib6 = ib6 + (((ib0 * 8212 + ib1 * -5882 + ib2 * -760 + ib3 * -261 + ib4 * 3416 + ib5 * 9556 + ib7 * 4017) + 4096) >> 13);
            ib7 = ib7 + (((ib0 * 9028 + ib1 * -16635 + ib2 * -3179 + ib3 * 5375 + ib4 * 10162 + ib5 * 13580 + ib6 * -9760) + 4096) >> 13);
#endif

            oblk0 = ib3; oblk1 = ib6;
            oblk2 = ib4; oblk3 = ib2;
            oblk4 = ib5; oblk5 = ib7;
            oblk6 = ib0; oblk7 = ib1;
        }
#endif

#if false
        public static void TransRDCT_Horiz(ShortPtr iblk, IntPtr oblk)
        {
			PDJPG_TransRDCT_Ref(
				iblk[0], iblk[1], iblk[2], iblk[3],
				iblk[4], iblk[5], iblk[6], iblk[7],
				out oblk[0], out oblk[1], out oblk[2], out oblk[3],
				out oblk[4], out oblk[5], out oblk[6], out oblk[7]);
        }

        public static void TransRDCT_Vert(IntPtr iblk, IntPtr oblk)
        {
			PDJPG_TransRDCT_Ref(
				iblk[0], iblk[8], iblk[16], iblk[24],
				iblk[32], iblk[40], iblk[48], iblk[56],
				out oblk[0], out oblk[8], out oblk[16], out oblk[24],
				out oblk[32], out oblk[40], out oblk[48], out oblk[56]);
        }
#endif

#if true
        // void TransRDCT(byte *iblk, short *oblk)
        public static void TransBlock(ShortPtr iblk, ShortPtr oblk, int dcbias)
        {
            int s00, s01, s02, s03, s04, s05, s06, s07;
            int s10, s11, s12, s13, s14, s15, s16, s17;
            int s20, s21, s22, s23, s24, s25, s26, s27;
            int s30, s31, s32, s33, s34, s35, s36, s37;
            int s40, s41, s42, s43, s44, s45, s46, s47;
            int s50, s51, s52, s53, s54, s55, s56, s57;
            int s60, s61, s62, s63, s64, s65, s66, s67;
            int s70, s71, s72, s73, s74, s75, s76, s77;
            int t00, t01, t02, t03, t04, t05, t06, t07;
            int t10, t11, t12, t13, t14, t15, t16, t17;
            int t20, t21, t22, t23, t24, t25, t26, t27;
            int t30, t31, t32, t33, t34, t35, t36, t37;
            int t40, t41, t42, t43, t44, t45, t46, t47;
            int t50, t51, t52, t53, t54, t55, t56, t57;
            int t60, t61, t62, t63, t64, t65, t66, t67;
            int t70, t71, t72, t73, t74, t75, t76, t77;
            //int i;

            TransRDCT_Ref(
                iblk[0] - dcbias, iblk[1] - dcbias,
                iblk[2] - dcbias, iblk[3] - dcbias,
                iblk[4] - dcbias, iblk[5] - dcbias,
                iblk[6] - dcbias, iblk[7] - dcbias,
                out s00, out s01, out s02, out s03,
                out s04, out s05, out s06, out s07);
            TransRDCT_Ref(
                iblk[8] - dcbias, iblk[9] - dcbias,
                iblk[10] - dcbias, iblk[11] - dcbias,
                iblk[12] - dcbias, iblk[13] - dcbias,
                iblk[14] - dcbias, iblk[15] - dcbias,
                out s10, out s11, out s12, out s13,
                out s14, out s15, out s16, out s17);
            TransRDCT_Ref(
                iblk[16] - dcbias, iblk[17] - dcbias,
                iblk[18] - dcbias, iblk[19] - dcbias,
                iblk[20] - dcbias, iblk[21] - dcbias,
                iblk[22] - dcbias, iblk[23] - dcbias,
                out s20, out s21, out s22, out s23,
                out s24, out s25, out s26, out s27);
            TransRDCT_Ref(
                iblk[24] - dcbias, iblk[25] - dcbias,
                iblk[26] - dcbias, iblk[27] - dcbias,
                iblk[28] - dcbias, iblk[29] - dcbias,
                iblk[30] - dcbias, iblk[31] - dcbias,
                out s30, out s31, out s32, out s33, 
                out s34, out s35, out s36, out s37);
            TransRDCT_Ref(
                iblk[32] - dcbias, iblk[33] - dcbias,
                iblk[34] - dcbias, iblk[35] - dcbias,
                iblk[36] - dcbias, iblk[37] - dcbias,
                iblk[38] - dcbias, iblk[39] - dcbias,
                out s40, out s41, out s42, out s43,
                out s44, out s45, out s46, out s47);
            TransRDCT_Ref(
                iblk[40] - dcbias, iblk[41] - dcbias,
                iblk[42] - dcbias, iblk[43] - dcbias,
                iblk[44] - dcbias, iblk[45] - dcbias,
                iblk[46] - dcbias, iblk[47] - dcbias,
                out s50, out s51, out s52, out s53,
                out s54, out s55, out s56, out s57);
            TransRDCT_Ref(
                iblk[48] - dcbias, iblk[49] - dcbias,
                iblk[50] - dcbias, iblk[51] - dcbias,
                iblk[52] - dcbias, iblk[53] - dcbias,
                iblk[54] - dcbias, iblk[55] - dcbias,
                out s60, out s61, out s62, out s63,
                out s64, out s65, out s66, out s67);
            TransRDCT_Ref(
                iblk[56] - dcbias, iblk[57] - dcbias,
                iblk[58] - dcbias, iblk[59] - dcbias,
                iblk[60] - dcbias, iblk[61] - dcbias,
                iblk[62] - dcbias, iblk[63] - dcbias,
                out s70, out s71, out s72, out s73,
                out s74, out s75, out s76, out s77);

            TransRDCT_Ref(s00, s10, s20, s30, s40, s50, s60, s70,
                out t00, out t10, out t20, out t30,
                out t40, out t50, out t60, out t70);
            TransRDCT_Ref(s01, s11, s21, s31, s41, s51, s61, s71,
                out t01, out t11, out t21, out t31,
                out t41, out t51, out t61, out t71);
            TransRDCT_Ref(s02, s12, s22, s32, s42, s52, s62, s72,
                out t02, out t12, out t22, out t32,
                out t42, out t52, out t62, out t72);
            TransRDCT_Ref(s03, s13, s23, s33, s43, s53, s63, s73,
                out t03, out t13, out t23, out t33,
                out t43, out t53, out t63, out t73);
            TransRDCT_Ref(s04, s14, s24, s34, s44, s54, s64, s74,
                out t04, out t14, out t24, out t34,
                out t44, out t54, out t64, out t74);
            TransRDCT_Ref(s05, s15, s25, s35, s45, s55, s65, s75,
                out t05, out t15, out t25, out t35,
                out t45, out t55, out t65, out t75);
            TransRDCT_Ref(s06, s16, s26, s36, s46, s56, s66, s76,
                out t06, out t16, out t26, out t36,
                out t46, out t56, out t66, out t76);
            TransRDCT_Ref(s07, s17, s27, s37, s47, s57, s67, s77,
                out t07, out t17, out t27, out t37,
                out t47, out t57, out t67, out t77);

            oblk[0] = t00; oblk[1] = t01; oblk[2] = t02; oblk[3] = t03;
            oblk[4] = t04; oblk[5] = t05; oblk[6] = t06; oblk[7] = t07;
            oblk[8] = t10; oblk[9] = t11; oblk[10] = t12; oblk[11] = t13;
            oblk[12] = t14; oblk[13] = t15; oblk[14] = t16; oblk[15] = t17;
            oblk[16] = t20; oblk[17] = t21; oblk[18] = t22; oblk[19] = t23;
            oblk[20] = t24; oblk[21] = t25; oblk[22] = t26; oblk[23] = t27;
            oblk[24] = t30; oblk[25] = t31; oblk[26] = t32; oblk[27] = t33;
            oblk[28] = t34; oblk[29] = t35; oblk[30] = t36; oblk[31] = t37;
            oblk[32] = t40; oblk[33] = t41; oblk[34] = t42; oblk[35] = t43;
            oblk[36] = t44; oblk[37] = t45; oblk[38] = t46; oblk[39] = t47;
            oblk[40] = t50; oblk[41] = t51; oblk[42] = t52; oblk[43] = t53;
            oblk[44] = t54; oblk[45] = t55; oblk[46] = t56; oblk[47] = t57;
            oblk[48] = t60; oblk[49] = t61; oblk[50] = t62; oblk[51] = t63;
            oblk[52] = t64; oblk[53] = t65; oblk[54] = t66; oblk[55] = t67;
            oblk[56] = t70; oblk[57] = t71; oblk[58] = t72; oblk[59] = t73;
            oblk[60] = t74; oblk[61] = t75; oblk[62] = t76; oblk[63] = t77;
        }
#endif

#if true
        public static void TransRDCT_Horiz(ShortPtr iblk, IntPtr oblk, int dcbias)
        {
            int ob0, ob1, ob2, ob3, ob4, ob5, ob6, ob7;
            TransRDCT_Ref(
                iblk[0] - dcbias, iblk[1] - dcbias, iblk[2] - dcbias, iblk[3] - dcbias,
                iblk[4] - dcbias, iblk[5] - dcbias, iblk[6] - dcbias, iblk[7] - dcbias,
                out ob0, out ob1, out ob2, out ob3,
                out ob4, out ob5, out ob6, out ob7);
            oblk[0] = ob0; oblk[1] = ob1;
            oblk[2] = ob2; oblk[3] = ob3;
            oblk[4] = ob4; oblk[5] = ob5;
            oblk[6] = ob6; oblk[7] = ob7;
        }

        public static void TransRDCT_Vert(IntPtr iblk, IntPtr oblk)
        {
            int ob0, ob1, ob2, ob3, ob4, ob5, ob6, ob7;
            TransRDCT_Ref(
                iblk[0], iblk[8], iblk[16], iblk[24],
                iblk[32], iblk[40], iblk[48], iblk[56],
                out ob0, out ob1, out ob2, out ob3,
                out ob4, out ob5, out ob6, out ob7);
            oblk[0] = ob0; oblk[8] = ob1;
            oblk[16] = ob2; oblk[24] = ob3;
            oblk[32] = ob4; oblk[40] = ob5;
            oblk[48] = ob6; oblk[56] = ob7;
        }

        public static void TransRDCT_Step(IntPtr iblk, IntPtr oblk, int step)
        {
            int ob0, ob1, ob2, ob3, ob4, ob5, ob6, ob7;
            TransRDCT_Ref(
                iblk[0 * step], iblk[1 * step],
                iblk[2 * step], iblk[3 * step],
                iblk[4 * step], iblk[5 * step],
                iblk[6 * step], iblk[7 * step],
                out ob0, out ob1, out ob2, out ob3,
                out ob4, out ob5, out ob6, out ob7);
            oblk[0 * step] = ob0; oblk[1 * step] = ob1;
            oblk[2 * step] = ob2; oblk[3 * step] = ob3;
            oblk[4 * step] = ob4; oblk[5 * step] = ob5;
            oblk[6 * step] = ob6; oblk[7 * step] = ob7;
        }
#endif

#if true
        public static void TransBasicBlock(ShortPtr iblk, IntPtr oblk, int dcbias)
        {
            IntPtr s = new IntPtr(PDJPG.DCTSZ2);
            IntPtr t = new IntPtr(PDJPG.DCTSZ2);
            int i;

            TransRDCT_Horiz(iblk + 0, s + 0, dcbias);
            TransRDCT_Horiz(iblk + 8, s + 8, dcbias);
            TransRDCT_Horiz(iblk + 16, s + 16, dcbias);
            TransRDCT_Horiz(iblk + 24, s + 24, dcbias);
            TransRDCT_Horiz(iblk + 32, s + 32, dcbias);
            TransRDCT_Horiz(iblk + 40, s + 40, dcbias);
            TransRDCT_Horiz(iblk + 48, s + 48, dcbias);
            TransRDCT_Horiz(iblk + 56, s + 56, dcbias);

            TransRDCT_Vert(s + 0, t + 0);
            TransRDCT_Vert(s + 1, t + 1);
            TransRDCT_Vert(s + 2, t + 2);
            TransRDCT_Vert(s + 3, t + 3);
            TransRDCT_Vert(s + 4, t + 4);
            TransRDCT_Vert(s + 5, t + 5);
            TransRDCT_Vert(s + 6, t + 6);
            TransRDCT_Vert(s + 7, t + 7);

            for (i = 0; i < 64; i++)
            {
                oblk[i] = t[i];
            }
        }

        public static void TransMegaBlock(ShortPtr iblk, ShortPtr oblk, int dcbias)
        {
            IntPtr s = new IntPtr(PDJPG.DCTSZ2 * PDJPG.DCTSZ2);
            IntPtr t = new IntPtr(PDJPG.DCTSZ2 * PDJPG.DCTSZ2);
            int i;

            for (i = 0; i < 64; i++)
            {
                TransBasicBlock(iblk + (i * 64), s + (i * 64), dcbias);
            }

#if true
            TransRDCT_Step(s + 0 * 512, s + 0 * 512, 64);
            TransRDCT_Step(s + 1 * 512, s + 1 * 512, 64);
            TransRDCT_Step(s + 2 * 512, s + 2 * 512, 64);
            TransRDCT_Step(s + 3 * 512, s + 3 * 512, 64);
            TransRDCT_Step(s + 4 * 512, s + 4 * 512, 64);
            TransRDCT_Step(s + 5 * 512, s + 5 * 512, 64);
            TransRDCT_Step(s + 6 * 512, s + 6 * 512, 64);
            TransRDCT_Step(s + 7 * 512, s + 7 * 512, 64);

            TransRDCT_Step(s + 0 * 64, s + 0 * 64, 512);
            TransRDCT_Step(s + 1 * 64, s + 1 * 64, 512);
            TransRDCT_Step(s + 2 * 64, s + 2 * 64, 512);
            TransRDCT_Step(s + 3 * 64, s + 3 * 64, 512);
            TransRDCT_Step(s + 4 * 64, s + 4 * 64, 512);
            TransRDCT_Step(s + 5 * 64, s + 5 * 64, 512);
            TransRDCT_Step(s + 6 * 64, s + 6 * 64, 512);
            TransRDCT_Step(s + 7 * 64, s + 7 * 64, 512);
#endif

#if false
            for (i = 0; i < 64; i++)
            {
                TransRDCT_Step(s + 0 * 512 + i, s + 0 * 512 + i, 64);
                TransRDCT_Step(s + 1 * 512 + i, s + 1 * 512 + i, 64);
                TransRDCT_Step(s + 2 * 512 + i, s + 2 * 512 + i, 64);
                TransRDCT_Step(s + 3 * 512 + i, s + 3 * 512 + i, 64);
                TransRDCT_Step(s + 4 * 512 + i, s + 4 * 512 + i, 64);
                TransRDCT_Step(s + 5 * 512 + i, s + 5 * 512 + i, 64);
                TransRDCT_Step(s + 6 * 512 + i, s + 6 * 512 + i, 64);
                TransRDCT_Step(s + 7 * 512 + i, s + 7 * 512 + i, 64);

                TransRDCT_Step(s + 0 * 64 + i, s + 0 * 64 + i, 512);
                TransRDCT_Step(s + 1 * 64 + i, s + 1 * 64 + i, 512);
                TransRDCT_Step(s + 2 * 64 + i, s + 2 * 64 + i, 512);
                TransRDCT_Step(s + 3 * 64 + i, s + 3 * 64 + i, 512);
                TransRDCT_Step(s + 4 * 64 + i, s + 4 * 64 + i, 512);
                TransRDCT_Step(s + 5 * 64 + i, s + 5 * 64 + i, 512);
                TransRDCT_Step(s + 6 * 64 + i, s + 6 * 64 + i, 512);
                TransRDCT_Step(s + 7 * 64 + i, s + 7 * 64 + i, 512);
            }
#endif

            for (i = 0; i < 4096; i++)
            {
                oblk[i] = s[i];
            }
        }
#endif


#if false
        private static int FLTOFIX14(double x)
        { return ((int)((x) * 8192 + 0.5)); }
        private static int FIX14TOI(int x)
        { return (((x) + 4096) >> 13); }

        // void TransRDCT_Horiz(byte *iblk, int *oblk)
        public static void TransRDCT_Horiz(ShortPtr iblk, IntPtr oblk, int dcbias)
        {
            int[] ib = new int[8];

            ib[0] = iblk[2] - dcbias; ib[1] = iblk[7] - dcbias;
            ib[2] = iblk[4] - dcbias; ib[3] = iblk[3] - dcbias;
            ib[4] = iblk[6] - dcbias; ib[5] = iblk[0] - dcbias;
            ib[6] = iblk[1] - dcbias; ib[7] = iblk[5] - dcbias;

            ib[7] = (-ib[7]) + FIX14TOI(
                ib[0] * FLTOFIX14(1.1648) + ib[1] * FLTOFIX14(-2.8234) +
                ib[2] * FLTOFIX14(0.5375) + ib[3] * FLTOFIX14(-0.6058) +
                ib[4] * FLTOFIX14(1.2228) + ib[5] * FLTOFIX14(-0.3805) +
                ib[6] * FLTOFIX14(0.0288));
            ib[0] = ib[0] + FIX14TOI(
                ib[1] * FLTOFIX14(-1.1129) + ib[2] * FLTOFIX14(0.0570) +
                ib[3] * FLTOFIX14(-0.4712) + ib[4] * FLTOFIX14(0.1029) +
                ib[5] * FLTOFIX14(0.0156) + ib[6] * FLTOFIX14(-0.4486) +
                ib[7] * FLTOFIX14(-0.4619));
            ib[1] = ib[1] + FIX14TOI(
                ib[0] * FLTOFIX14(-0.0685) + ib[2] * FLTOFIX14(0.2708) +
                ib[3] * FLTOFIX14(-0.2708) + ib[4] * FLTOFIX14(-0.2235) +
                ib[5] * FLTOFIX14(0.2568) + ib[6] * FLTOFIX14(-0.3205) +
                ib[7] * FLTOFIX14(0.3841));
            ib[2] = ib[2] + FIX14TOI(
                ib[0] * FLTOFIX14(-0.0364) + ib[1] * FLTOFIX14(-1.7104) +
                ib[3] * FLTOFIX14(-1.0000) + ib[4] * FLTOFIX14(0.3066) +
                ib[5] * FLTOFIX14(0.6671) + ib[6] * FLTOFIX14(-0.5953) +
                ib[7] * FLTOFIX14(0.2039));
            ib[3] = ib[3] + FIX14TOI(
                ib[0] * FLTOFIX14(0.7957) + ib[1] * FLTOFIX14(0.9664) +
                ib[2] * FLTOFIX14(0.4439) + ib[4] * FLTOFIX14(0.6173) +
                ib[5] * FLTOFIX14(-0.1422) + ib[6] * FLTOFIX14(1.0378) +
                ib[7] * FLTOFIX14(-0.1700));
            ib[4] = ib[4] + FIX14TOI(
                ib[0] * FLTOFIX14(0.4591) + ib[1] * FLTOFIX14(0.4108) +
                ib[2] * FLTOFIX14(-0.2073) + ib[3] * FLTOFIX14(-1.0824) +
                ib[5] * FLTOFIX14(0.7071) + ib[6] * FLTOFIX14(0.8873) +
                ib[7] * FLTOFIX14(-0.2517));
            ib[5] = ib[5] + FIX14TOI(
                ib[0] * FLTOFIX14(-0.6573) + ib[1] * FLTOFIX14(0.5810) +
                ib[2] * FLTOFIX14(-0.2931) + ib[3] * FLTOFIX14(-0.5307) +
                ib[4] * FLTOFIX14(-0.8730) + ib[6] * FLTOFIX14(-0.1594) +
                ib[7] * FLTOFIX14(-0.3560));
            ib[6] = ib[6] + FIX14TOI(
                ib[0] * FLTOFIX14(1.0024) + ib[1] * FLTOFIX14(-0.7180) +
                ib[2] * FLTOFIX14(-0.0928) + ib[3] * FLTOFIX14(-0.0318) +
                ib[4] * FLTOFIX14(0.4170) + ib[5] * FLTOFIX14(1.1665) +
                ib[7] * FLTOFIX14(0.4904));
            ib[7] = ib[7] + FIX14TOI(
                ib[0] * FLTOFIX14(1.1020) + ib[1] * FLTOFIX14(-2.0306) +
                ib[2] * FLTOFIX14(-0.3881) + ib[3] * FLTOFIX14(0.6561) +
                ib[4] * FLTOFIX14(1.2405) + ib[5] * FLTOFIX14(1.6577) +
                ib[6] * FLTOFIX14(-1.1914));
            oblk[0] = ib[3]; oblk[1] = ib[6];
            oblk[2] = ib[4]; oblk[3] = ib[2];
            oblk[4] = ib[5]; oblk[5] = ib[7];
            oblk[6] = ib[0]; oblk[7] = ib[1];
        }

        public static void TransRDCT_Vert(IntPtr iblk, IntPtr oblk)
        {
            int[] ib = new int[8];

            ib[0] = iblk[16]; ib[1] = iblk[56];
            ib[2] = iblk[32]; ib[3] = iblk[24];
            ib[4] = iblk[48]; ib[5] = iblk[0];
            ib[6] = iblk[8]; ib[7] = iblk[40];

            ib[7] = (-ib[7]) + FIX14TOI(
                ib[0] * FLTOFIX14(1.1648) + ib[1] * FLTOFIX14(-2.8234) +
                ib[2] * FLTOFIX14(0.5375) + ib[3] * FLTOFIX14(-0.6058) +
                ib[4] * FLTOFIX14(1.2228) + ib[5] * FLTOFIX14(-0.3805) +
                ib[6] * FLTOFIX14(0.0288));
            ib[0] = ib[0] + FIX14TOI(
                ib[1] * FLTOFIX14(-1.1129) + ib[2] * FLTOFIX14(0.0570) +
                ib[3] * FLTOFIX14(-0.4712) + ib[4] * FLTOFIX14(0.1029) +
                ib[5] * FLTOFIX14(0.0156) + ib[6] * FLTOFIX14(-0.4486) +
                ib[7] * FLTOFIX14(-0.4619));
            ib[1] = ib[1] + FIX14TOI(
                ib[0] * FLTOFIX14(-0.0685) + ib[2] * FLTOFIX14(0.2708) +
                ib[3] * FLTOFIX14(-0.2708) + ib[4] * FLTOFIX14(-0.2235) +
                ib[5] * FLTOFIX14(0.2568) + ib[6] * FLTOFIX14(-0.3205) +
                ib[7] * FLTOFIX14(0.3841));
            ib[2] = ib[2] + FIX14TOI(
                ib[0] * FLTOFIX14(-0.0364) + ib[1] * FLTOFIX14(-1.7104) +
                ib[3] * FLTOFIX14(-1.0000) + ib[4] * FLTOFIX14(0.3066) +
                ib[5] * FLTOFIX14(0.6671) + ib[6] * FLTOFIX14(-0.5953) +
                ib[7] * FLTOFIX14(0.2039));
            ib[3] = ib[3] + FIX14TOI(
                ib[0] * FLTOFIX14(0.7957) + ib[1] * FLTOFIX14(0.9664) +
                ib[2] * FLTOFIX14(0.4439) + ib[4] * FLTOFIX14(0.6173) +
                ib[5] * FLTOFIX14(-0.1422) + ib[6] * FLTOFIX14(1.0378) +
                ib[7] * FLTOFIX14(-0.1700));
            ib[4] = ib[4] + FIX14TOI(
                ib[0] * FLTOFIX14(0.4591) + ib[1] * FLTOFIX14(0.4108) +
                ib[2] * FLTOFIX14(-0.2073) + ib[3] * FLTOFIX14(-1.0824) +
                ib[5] * FLTOFIX14(0.7071) + ib[6] * FLTOFIX14(0.8873) +
                ib[7] * FLTOFIX14(-0.2517));
            ib[5] = ib[5] + FIX14TOI(
                ib[0] * FLTOFIX14(-0.6573) + ib[1] * FLTOFIX14(0.5810) +
                ib[2] * FLTOFIX14(-0.2931) + ib[3] * FLTOFIX14(-0.5307) +
                ib[4] * FLTOFIX14(-0.8730) + ib[6] * FLTOFIX14(-0.1594) +
                ib[7] * FLTOFIX14(-0.3560));
            ib[6] = ib[6] + FIX14TOI(
                ib[0] * FLTOFIX14(1.0024) + ib[1] * FLTOFIX14(-0.7180) +
                ib[2] * FLTOFIX14(-0.0928) + ib[3] * FLTOFIX14(-0.0318) +
                ib[4] * FLTOFIX14(0.4170) + ib[5] * FLTOFIX14(1.1665) +
                ib[7] * FLTOFIX14(0.4904));
            ib[7] = ib[7] + FIX14TOI(
                ib[0] * FLTOFIX14(1.1020) + ib[1] * FLTOFIX14(-2.0306) +
                ib[2] * FLTOFIX14(-0.3881) + ib[3] * FLTOFIX14(0.6561) +
                ib[4] * FLTOFIX14(1.2405) + ib[5] * FLTOFIX14(1.6577) +
                ib[6] * FLTOFIX14(-1.1914));
            oblk[0] = ib[3]; oblk[8] = ib[6];
            oblk[16] = ib[4]; oblk[24] = ib[2];
            oblk[32] = ib[5]; oblk[40] = ib[7];
            oblk[48] = ib[0]; oblk[56] = ib[1];
        }
#endif

#if false
        // void TransRDCT(byte *iblk, short *oblk)
        public static void TransBlock(ShortPtr iblk, ShortPtr oblk, int dcbias)
        {
            IntPtr s = new IntPtr(PDJPG.DCTSZ2);
            IntPtr t = new IntPtr(PDJPG.DCTSZ2);
            int i;

            TransRDCT_Horiz(iblk + 0, s + 0, dcbias);
            TransRDCT_Horiz(iblk + 8, s + 8, dcbias);
            TransRDCT_Horiz(iblk + 16, s + 16, dcbias);
            TransRDCT_Horiz(iblk + 24, s + 24, dcbias);
            TransRDCT_Horiz(iblk + 32, s + 32, dcbias);
            TransRDCT_Horiz(iblk + 40, s + 40, dcbias);
            TransRDCT_Horiz(iblk + 48, s + 48, dcbias);
            TransRDCT_Horiz(iblk + 56, s + 56, dcbias);

            TransRDCT_Vert(s + 0, t + 0);
            TransRDCT_Vert(s + 1, t + 1);
            TransRDCT_Vert(s + 2, t + 2);
            TransRDCT_Vert(s + 3, t + 3);
            TransRDCT_Vert(s + 4, t + 4);
            TransRDCT_Vert(s + 5, t + 5);
            TransRDCT_Vert(s + 6, t + 6);
            TransRDCT_Vert(s + 7, t + 7);

            for (i = 0; i < 64; i++)
                oblk[i] = t[i];
        }
#endif
    }

    class PDJPG_TransIRDCT
    {
        private static int FLTOFIX14(double x)
        { return ((int)((x) * 8192 + 0.5)); }
        private static int FIX14TOI(int x)
        { return (((x) + 4096) >> 13); }


#if true
        public static void TransIRDCT_Ref(
            int iblk0, int iblk1, int iblk2, int iblk3,
            int iblk4, int iblk5, int iblk6, int iblk7,
            out int oblk0, out int oblk1, out int oblk2, out int oblk3,
            out int oblk4, out int oblk5, out int oblk6, out int oblk7)
        {
            int ib0, ib1, ib2, ib3, ib4, ib5, ib6, ib7;

            ib0 = iblk6; ib1 = iblk7;
            ib2 = iblk3; ib3 = iblk0;
            ib4 = iblk2; ib5 = iblk4;
            ib6 = iblk1; ib7 = iblk5;

            //9028 -16635 -3179 5375 10162 13580 -9760
            //8212 -5882 -760 -261 3416 9556 4017
            //-5385 4760 -2401 -4347 -7152 -1306 -2916
            //3761 3365 -1698 -8867 5793 7269 -2062
            //6518 7917 3636 5057 -1165 8502 -1393
            //-298 -14012 -8192 2512 5465 -4877 1670
            //-561 2218 -2218 -1831 2104 -2626 3147
            //-9117 467 -3860 843 128 -3675 -3784
            //9542 -23129 4403 -4963 10017 -3117 236

#if true
            ib7 = ib7 - (((ib0 * 9028 + ib1 * -16635 + ib2 * -3179 + ib3 * 5375 + ib4 * 10162 + ib5 * 13580 + ib6 * -9760) + 4096) >> 13);
            ib6 = ib6 - (((ib0 * 8212 + ib1 * -5882 + ib2 * -760 + ib3 * -261 + ib4 * 3416 + ib5 * 9556 + ib7 * 4017) + 4096) >> 13);
            ib5 = ib5 - (((ib0 * -5385 + ib1 * 4760 + ib2 * -2401 + ib3 * -4347 + ib4 * -7152 + ib6 * -1306 + ib7 * -2916) + 4096) >> 13);
            ib4 = ib4 - (((ib0 * 3761 + ib1 * 3365 + ib2 * -1698 + ib3 * -8867 + ib5 * 5793 + ib6 * 7269 + ib7 * -2062) + 4096) >> 13);
            ib3 = ib3 - (((ib0 * 6518 + ib1 * 7917 + ib2 * 3636 + ib4 * 5057 + ib5 * -1165 + ib6 * 8502 + ib7 * -1393) + 4096) >> 13);
            ib2 = ib2 - (((ib0 * -298 + ib1 * -14012 + ib3 * -8192 + ib4 * 2512 + ib5 * 5465 + ib6 * -4877 + ib7 * 1670) + 4096) >> 13);
            ib1 = ib1 - (((ib0 * -561 + ib2 * 2218 + ib3 * -2218 + ib4 * -1831 + ib5 * 2104 + ib6 * -2626 + ib7 * 3147) + 4096) >> 13);
            ib0 = ib0 - (((ib1 * -9117 + ib2 * 467 + ib3 * -3860 + ib4 * 843 + ib5 * 128 + ib6 * -3675 + ib7 * -3784) + 4096) >> 13);
            ib7 = -ib7 + (((ib0 * 9542 + ib1 * -23129 + ib2 * 4403 + ib3 * -4963 + ib4 * 10017 + ib5 * -3117 + ib6 * 236) + 4096) >> 13);
#endif

            oblk0 = ib5; oblk1 = ib6;
            oblk2 = ib0; oblk3 = ib3;
            oblk4 = ib2; oblk5 = ib7;
            oblk6 = ib4; oblk7 = ib1;
        }
#endif

#if true
        public static void TransIRDCT_Horiz(IntPtr iblk, IntPtr oblk)
        {
            int ob0, ob1, ob2, ob3, ob4, ob5, ob6, ob7;
			TransIRDCT_Ref(
				iblk[0], iblk[1], iblk[2], iblk[3],
				iblk[4], iblk[5], iblk[6], iblk[7],
				out ob0, out ob1, out ob2, out ob3,
				out ob4, out ob5, out ob6, out ob7);
            oblk[0] = ob0; oblk[1] = ob1;
            oblk[2] = ob2; oblk[3] = ob3;
            oblk[4] = ob4; oblk[5] = ob5;
            oblk[6] = ob6; oblk[7] = ob7;
        }

        public static void TransIRDCT_Vert(IntPtr iblk, IntPtr oblk)
        {
            int ob0, ob1, ob2, ob3, ob4, ob5, ob6, ob7;
            TransIRDCT_Ref(
				iblk[0], iblk[8], iblk[16], iblk[24],
				iblk[32], iblk[40], iblk[48], iblk[56],
                out ob0, out ob1, out ob2, out ob3,
                out ob4, out ob5, out ob6, out ob7);
            oblk[0] = ob0; oblk[8] = ob1;
            oblk[16] = ob2; oblk[24] = ob3;
            oblk[32] = ob4; oblk[40] = ob5;
            oblk[48] = ob6; oblk[56] = ob7;
        }

        public static void TransIRDCT_Step(IntPtr iblk, IntPtr oblk, int step)
        {
            int ob0, ob1, ob2, ob3, ob4, ob5, ob6, ob7;
            TransIRDCT_Ref(
                iblk[0 * step], iblk[1 * step],
                iblk[2 * step], iblk[3 * step],
                iblk[4 * step], iblk[5 * step],
                iblk[6 * step], iblk[7 * step],
                out ob0, out ob1, out ob2, out ob3,
                out ob4, out ob5, out ob6, out ob7);
            oblk[0 * step] = ob0; oblk[1 * step] = ob1;
            oblk[2 * step] = ob2; oblk[3 * step] = ob3;
            oblk[4 * step] = ob4; oblk[5 * step] = ob5;
            oblk[6 * step] = ob6; oblk[7 * step] = ob7;
        }
#endif

#if true
        public static void TransBlock(ShortPtr iblk, ShortPtr oblk, int dcbias)
        {
            int s00, s01, s02, s03, s04, s05, s06, s07;
            int s10, s11, s12, s13, s14, s15, s16, s17;
            int s20, s21, s22, s23, s24, s25, s26, s27;
            int s30, s31, s32, s33, s34, s35, s36, s37;
            int s40, s41, s42, s43, s44, s45, s46, s47;
            int s50, s51, s52, s53, s54, s55, s56, s57;
            int s60, s61, s62, s63, s64, s65, s66, s67;
            int s70, s71, s72, s73, s74, s75, s76, s77;
            int t00, t01, t02, t03, t04, t05, t06, t07;
            int t10, t11, t12, t13, t14, t15, t16, t17;
            int t20, t21, t22, t23, t24, t25, t26, t27;
            int t30, t31, t32, t33, t34, t35, t36, t37;
            int t40, t41, t42, t43, t44, t45, t46, t47;
            int t50, t51, t52, t53, t54, t55, t56, t57;
            int t60, t61, t62, t63, t64, t65, t66, t67;
            int t70, t71, t72, t73, t74, t75, t76, t77;

            TransIRDCT_Ref(
                iblk[0], iblk[8], iblk[16], iblk[24],
                iblk[32], iblk[40], iblk[48], iblk[56],
                out s00, out s10, out s20, out s30,
                out s40, out s50, out s60, out s70);
            TransIRDCT_Ref(
                iblk[1], iblk[9], iblk[17], iblk[25],
                iblk[33], iblk[41], iblk[49], iblk[57],
                out s01, out s11, out s21, out s31,
                out s41, out s51, out s61, out s71);
            TransIRDCT_Ref(
                iblk[2], iblk[10], iblk[18], iblk[26],
                iblk[34], iblk[42], iblk[50], iblk[58],
                out s02, out s12, out s22, out s32,
                out s42, out s52, out s62, out s72);
            TransIRDCT_Ref(
                iblk[3], iblk[11], iblk[19], iblk[27],
                iblk[35], iblk[43], iblk[51], iblk[59],
                out s03, out s13, out s23, out s33,
                out s43, out s53, out s63, out s73);
            TransIRDCT_Ref(
                iblk[4], iblk[12], iblk[20], iblk[28],
                iblk[36], iblk[44], iblk[52], iblk[60],
                out s04, out s14, out s24, out s34,
                out s44, out s54, out s64, out s74);
            TransIRDCT_Ref(
                iblk[5], iblk[13], iblk[21], iblk[29],
                iblk[37], iblk[45], iblk[53], iblk[61],
                out s05, out s15, out s25, out s35,
                out s45, out s55, out s65, out s75);
            TransIRDCT_Ref(
                iblk[6], iblk[14], iblk[22], iblk[30],
                iblk[38], iblk[46], iblk[54], iblk[62],
                out s06, out s16, out s26, out s36,
                out s46, out s56, out s66, out s76);
            TransIRDCT_Ref(
                iblk[7], iblk[15], iblk[23], iblk[31],
                iblk[39], iblk[47], iblk[55], iblk[63],
                out s07, out s17, out s27, out s37,
                out s47, out s57, out s67, out s77);

            TransIRDCT_Ref(s00, s01, s02, s03, s04, s05, s06, s07,
                out t00, out t01, out t02, out t03,
                out t04, out t05, out t06, out t07);
            TransIRDCT_Ref(s10, s11, s12, s13, s14, s15, s16, s17,
                out t10, out t11, out t12, out t13,
                out t14, out t15, out t16, out t17);
            TransIRDCT_Ref(s20, s21, s22, s23, s24, s25, s26, s27,
                out t20, out t21, out t22, out t23,
                out t24, out t25, out t26, out t27);
            TransIRDCT_Ref(s30, s31, s32, s33, s34, s35, s36, s37,
                out t30, out t31, out t32, out t33,
                out t34, out t35, out t36, out t37);
            TransIRDCT_Ref(s40, s41, s42, s43, s44, s45, s46, s47,
                out t40, out t41, out t42, out t43,
                out t44, out t45, out t46, out t47);
            TransIRDCT_Ref(s50, s51, s52, s53, s54, s55, s56, s57,
                out t50, out t51, out t52, out t53,
                out t54, out t55, out t56, out t57);
            TransIRDCT_Ref(s60, s61, s62, s63, s64, s65, s66, s67,
                out t60, out t61, out t62, out t63,
                out t64, out t65, out t66, out t67);
            TransIRDCT_Ref(s70, s71, s72, s73, s74, s75, s76, s77,
                out t70, out t71, out t72, out t73,
                out t74, out t75, out t76, out t77);

            oblk[0] = t00 + dcbias; oblk[1] = t01 + dcbias;
            oblk[2] = t02 + dcbias; oblk[3] = t03 + dcbias;
            oblk[4] = t04 + dcbias; oblk[5] = t05 + dcbias;
            oblk[6] = t06 + dcbias; oblk[7] = t07 + dcbias;
            oblk[8] = t10 + dcbias; oblk[9] = t11 + dcbias;
            oblk[10] = t12 + dcbias; oblk[11] = t13 + dcbias;
            oblk[12] = t14 + dcbias; oblk[13] = t15 + dcbias;
            oblk[14] = t16 + dcbias; oblk[15] = t17 + dcbias;
            oblk[16] = t20 + dcbias; oblk[17] = t21 + dcbias;
            oblk[18] = t22 + dcbias; oblk[19] = t23 + dcbias;
            oblk[20] = t24 + dcbias; oblk[21] = t25 + dcbias;
            oblk[22] = t26 + dcbias; oblk[23] = t27 + dcbias;
            oblk[24] = t30 + dcbias; oblk[25] = t31 + dcbias;
            oblk[26] = t32 + dcbias; oblk[27] = t33 + dcbias;
            oblk[28] = t34 + dcbias; oblk[29] = t35 + dcbias;
            oblk[30] = t36 + dcbias; oblk[31] = t37 + dcbias;
            oblk[32] = t40 + dcbias; oblk[33] = t41 + dcbias;
            oblk[34] = t42 + dcbias; oblk[35] = t43 + dcbias;
            oblk[36] = t44 + dcbias; oblk[37] = t45 + dcbias;
            oblk[38] = t46 + dcbias; oblk[39] = t47 + dcbias;
            oblk[40] = t50 + dcbias; oblk[41] = t51 + dcbias;
            oblk[42] = t52 + dcbias; oblk[43] = t53 + dcbias;
            oblk[44] = t54 + dcbias; oblk[45] = t55 + dcbias;
            oblk[46] = t56 + dcbias; oblk[47] = t57 + dcbias;
            oblk[48] = t60 + dcbias; oblk[49] = t61 + dcbias;
            oblk[50] = t62 + dcbias; oblk[51] = t63 + dcbias;
            oblk[52] = t64 + dcbias; oblk[53] = t65 + dcbias;
            oblk[54] = t66 + dcbias; oblk[55] = t67 + dcbias;
            oblk[56] = t70 + dcbias; oblk[57] = t71 + dcbias;
            oblk[58] = t72 + dcbias; oblk[59] = t73 + dcbias;
            oblk[60] = t74 + dcbias; oblk[61] = t75 + dcbias;
            oblk[62] = t76 + dcbias; oblk[63] = t77 + dcbias;
        }
#endif

#if true
        public static void TransBasicBlock(IntPtr iblk, IntPtr oblk)
        {
            IntPtr s = new IntPtr(PDJPG.DCTSZ2);
            IntPtr t = new IntPtr(PDJPG.DCTSZ2);
            int i;

            TransIRDCT_Vert(iblk + 0, s + 0);
            TransIRDCT_Vert(iblk + 1, s + 1);
            TransIRDCT_Vert(iblk + 2, s + 2);
            TransIRDCT_Vert(iblk + 3, s + 3);
            TransIRDCT_Vert(iblk + 4, s + 4);
            TransIRDCT_Vert(iblk + 5, s + 5);
            TransIRDCT_Vert(iblk + 6, s + 6);
            TransIRDCT_Vert(iblk + 7, s + 7);

            TransIRDCT_Horiz(s + 0, t + 0);
            TransIRDCT_Horiz(s + 8, t + 8);
            TransIRDCT_Horiz(s + 16, t + 16);
            TransIRDCT_Horiz(s + 24, t + 24);
            TransIRDCT_Horiz(s + 32, t + 32);
            TransIRDCT_Horiz(s + 40, t + 40);
            TransIRDCT_Horiz(s + 48, t + 48);
            TransIRDCT_Horiz(s + 56, t + 56);

            for (i = 0; i < 64; i++)
            {
                oblk[i] = t[i];
            }
        }

        public static void TransMegaBlock(ShortPtr iblk, ShortPtr oblk, int dcbias)
        {
            IntPtr s = new IntPtr(PDJPG.DCTSZ2 * PDJPG.DCTSZ2);
            IntPtr t = new IntPtr(PDJPG.DCTSZ2 * PDJPG.DCTSZ2);
            int i, j;

            for (i = 0; i < 4096; i++)
            {
                s[i] = iblk[i];
            }

#if true
            TransIRDCT_Step(s + 0 * 64, s + 0 * 64, 512);
            TransIRDCT_Step(s + 1 * 64, s + 1 * 64, 512);
            TransIRDCT_Step(s + 2 * 64, s + 2 * 64, 512);
            TransIRDCT_Step(s + 3 * 64, s + 3 * 64, 512);
            TransIRDCT_Step(s + 4 * 64, s + 4 * 64, 512);
            TransIRDCT_Step(s + 5 * 64, s + 5 * 64, 512);
            TransIRDCT_Step(s + 6 * 64, s + 6 * 64, 512);
            TransIRDCT_Step(s + 7 * 64, s + 7 * 64, 512);

            TransIRDCT_Step(s + 0 * 512, s + 0 * 512, 64);
            TransIRDCT_Step(s + 1 * 512, s + 1 * 512, 64);
            TransIRDCT_Step(s + 2 * 512, s + 2 * 512, 64);
            TransIRDCT_Step(s + 3 * 512, s + 3 * 512, 64);
            TransIRDCT_Step(s + 4 * 512, s + 4 * 512, 64);
            TransIRDCT_Step(s + 5 * 512, s + 5 * 512, 64);
            TransIRDCT_Step(s + 6 * 512, s + 6 * 512, 64);
            TransIRDCT_Step(s + 7 * 512, s + 7 * 512, 64);
#endif

#if true
            for (i = 0; i < 64; i++)
            {
                TransBasicBlock(s + (i * 64), s + (i * 64));
            }
#endif

#if false
            for (i = 0; i < 64; i++)
            {
                TransIRDCT_Step(s + 0 * 64 + i, s + 0 * 64 + i, 512);
                TransIRDCT_Step(s + 1 * 64 + i, s + 1 * 64 + i, 512);
                TransIRDCT_Step(s + 2 * 64 + i, s + 2 * 64 + i, 512);
                TransIRDCT_Step(s + 3 * 64 + i, s + 3 * 64 + i, 512);
                TransIRDCT_Step(s + 4 * 64 + i, s + 4 * 64 + i, 512);
                TransIRDCT_Step(s + 5 * 64 + i, s + 5 * 64 + i, 512);
                TransIRDCT_Step(s + 6 * 64 + i, s + 6 * 64 + i, 512);
                TransIRDCT_Step(s + 7 * 64 + i, s + 7 * 64 + i, 512);

                TransIRDCT_Step(s + 0 * 512 + i, s + 0 * 512 + i, 64);
                TransIRDCT_Step(s + 1 * 512 + i, s + 1 * 512 + i, 64);
                TransIRDCT_Step(s + 2 * 512 + i, s + 2 * 512 + i, 64);
                TransIRDCT_Step(s + 3 * 512 + i, s + 3 * 512 + i, 64);
                TransIRDCT_Step(s + 4 * 512 + i, s + 4 * 512 + i, 64);
                TransIRDCT_Step(s + 5 * 512 + i, s + 5 * 512 + i, 64);
                TransIRDCT_Step(s + 6 * 512 + i, s + 6 * 512 + i, 64);
                TransIRDCT_Step(s + 7 * 512 + i, s + 7 * 512 + i, 64);
            }
#endif

            for (i = 0; i < 4096; i++)
            {
                j = (s[i]) + dcbias;
                //		if(j&(~255))j=(j<0)?0:((j>255)?255:j);
                oblk[i] = j;
            }
        }
#endif

#if false
        public static void TransIRDCT_Horiz(IntPtr iblk, IntPtr oblk)
        {
            int[] ib = new int[8];

            ib[0] = iblk[6]; ib[1] = iblk[7];
            ib[2] = iblk[3]; ib[3] = iblk[0];
            ib[4] = iblk[2]; ib[5] = iblk[4];
            ib[6] = iblk[1]; ib[7] = iblk[5];

            ib[7] = ib[7] - FIX14TOI(
                ib[0] * FLTOFIX14(1.1020) + ib[1] * FLTOFIX14(-2.0306) +
                ib[2] * FLTOFIX14(-0.3881) + ib[3] * FLTOFIX14(0.6561) +
                ib[4] * FLTOFIX14(1.2405) + ib[5] * FLTOFIX14(1.6577) +
                ib[6] * FLTOFIX14(-1.1914));
            ib[6] = ib[6] - FIX14TOI(
                ib[0] * FLTOFIX14(1.0024) + ib[1] * FLTOFIX14(-0.7180) +
                ib[2] * FLTOFIX14(-0.0928) + ib[3] * FLTOFIX14(-0.0318) +
                ib[4] * FLTOFIX14(0.4170) + ib[5] * FLTOFIX14(1.1665) +
                ib[7] * FLTOFIX14(0.4904));
            ib[5] = ib[5] - FIX14TOI(
                ib[0] * FLTOFIX14(-0.6573) + ib[1] * FLTOFIX14(0.5810) +
                ib[2] * FLTOFIX14(-0.2931) + ib[3] * FLTOFIX14(-0.5307) +
                ib[4] * FLTOFIX14(-0.8730) + ib[6] * FLTOFIX14(-0.1594) +
                ib[7] * FLTOFIX14(-0.3560));
            ib[4] = ib[4] - FIX14TOI(
                ib[0] * FLTOFIX14(0.4591) + ib[1] * FLTOFIX14(0.4108) +
                ib[2] * FLTOFIX14(-0.2073) + ib[3] * FLTOFIX14(-1.0824) +
                ib[5] * FLTOFIX14(0.7071) + ib[6] * FLTOFIX14(0.8873) +
                ib[7] * FLTOFIX14(-0.2517));
            ib[3] = ib[3] - FIX14TOI(
                ib[0] * FLTOFIX14(0.7957) + ib[1] * FLTOFIX14(0.9664) +
                ib[2] * FLTOFIX14(0.4439) + ib[4] * FLTOFIX14(0.6173) +
                ib[5] * FLTOFIX14(-0.1422) + ib[6] * FLTOFIX14(1.0378) +
                ib[7] * FLTOFIX14(-0.1700));
            ib[2] = ib[2] - FIX14TOI(
                ib[0] * FLTOFIX14(-0.0364) + ib[1] * FLTOFIX14(-1.7104) +
                ib[3] * FLTOFIX14(-1.0000) + ib[4] * FLTOFIX14(0.3066) +
                ib[5] * FLTOFIX14(0.6671) + ib[6] * FLTOFIX14(-0.5953) +
                ib[7] * FLTOFIX14(0.2039));
            ib[1] = ib[1] - FIX14TOI(
                ib[0] * FLTOFIX14(-0.0685) + ib[2] * FLTOFIX14(0.2708) +
                ib[3] * FLTOFIX14(-0.2708) + ib[4] * FLTOFIX14(-0.2235) +
                ib[5] * FLTOFIX14(0.2568) + ib[6] * FLTOFIX14(-0.3205) +
                ib[7] * FLTOFIX14(0.3841));
            ib[0] = ib[0] - FIX14TOI(
                ib[1] * FLTOFIX14(-1.1129) + ib[2] * FLTOFIX14(0.0570) +
                ib[3] * FLTOFIX14(-0.4712) + ib[4] * FLTOFIX14(0.1029) +
                ib[5] * FLTOFIX14(0.0156) + ib[6] * FLTOFIX14(-0.4486) +
                ib[7] * FLTOFIX14(-0.4619));
            ib[7] = -ib[7] + FIX14TOI(
                ib[0] * FLTOFIX14(1.1648) + ib[1] * FLTOFIX14(-2.8234) +
                ib[2] * FLTOFIX14(0.5375) + ib[3] * FLTOFIX14(-0.6058) +
                ib[4] * FLTOFIX14(1.2228) + ib[5] * FLTOFIX14(-0.3805) +
                ib[6] * FLTOFIX14(0.0288));

            oblk[0] = ib[5]; oblk[1] = ib[6];
            oblk[2] = ib[0]; oblk[3] = ib[3];
            oblk[4] = ib[2]; oblk[5] = ib[7];
            oblk[6] = ib[4]; oblk[7] = ib[1];
        }

        public static void TransIRDCT_Vert(ShortPtr iblk, IntPtr oblk)
        {
            int[] ib = new int[8];

            ib[0] = iblk[48]; ib[1] = iblk[56];
            ib[2] = iblk[24]; ib[3] = iblk[0];
            ib[4] = iblk[16]; ib[5] = iblk[32];
            ib[6] = iblk[8]; ib[7] = iblk[40];

            ib[7] = ib[7] - FIX14TOI(
                ib[0] * FLTOFIX14(1.1020) + ib[1] * FLTOFIX14(-2.0306) +
                ib[2] * FLTOFIX14(-0.3881) + ib[3] * FLTOFIX14(0.6561) +
                ib[4] * FLTOFIX14(1.2405) + ib[5] * FLTOFIX14(1.6577) +
                ib[6] * FLTOFIX14(-1.1914));
            ib[6] = ib[6] - FIX14TOI(
                ib[0] * FLTOFIX14(1.0024) + ib[1] * FLTOFIX14(-0.7180) +
                ib[2] * FLTOFIX14(-0.0928) + ib[3] * FLTOFIX14(-0.0318) +
                ib[4] * FLTOFIX14(0.4170) + ib[5] * FLTOFIX14(1.1665) +
                ib[7] * FLTOFIX14(0.4904));
            ib[5] = ib[5] - FIX14TOI(
                ib[0] * FLTOFIX14(-0.6573) + ib[1] * FLTOFIX14(0.5810) +
                ib[2] * FLTOFIX14(-0.2931) + ib[3] * FLTOFIX14(-0.5307) +
                ib[4] * FLTOFIX14(-0.8730) + ib[6] * FLTOFIX14(-0.1594) +
                ib[7] * FLTOFIX14(-0.3560));
            ib[4] = ib[4] - FIX14TOI(
                ib[0] * FLTOFIX14(0.4591) + ib[1] * FLTOFIX14(0.4108) +
                ib[2] * FLTOFIX14(-0.2073) + ib[3] * FLTOFIX14(-1.0824) +
                ib[5] * FLTOFIX14(0.7071) + ib[6] * FLTOFIX14(0.8873) +
                ib[7] * FLTOFIX14(-0.2517));
            ib[3] = ib[3] - FIX14TOI(
                ib[0] * FLTOFIX14(0.7957) + ib[1] * FLTOFIX14(0.9664) +
                ib[2] * FLTOFIX14(0.4439) + ib[4] * FLTOFIX14(0.6173) +
                ib[5] * FLTOFIX14(-0.1422) + ib[6] * FLTOFIX14(1.0378) +
                ib[7] * FLTOFIX14(-0.1700));
            ib[2] = ib[2] - FIX14TOI(
                ib[0] * FLTOFIX14(-0.0364) + ib[1] * FLTOFIX14(-1.7104) +
                ib[3] * FLTOFIX14(-1.0000) + ib[4] * FLTOFIX14(0.3066) +
                ib[5] * FLTOFIX14(0.6671) + ib[6] * FLTOFIX14(-0.5953) +
                ib[7] * FLTOFIX14(0.2039));
            ib[1] = ib[1] - FIX14TOI(
                ib[0] * FLTOFIX14(-0.0685) + ib[2] * FLTOFIX14(0.2708) +
                ib[3] * FLTOFIX14(-0.2708) + ib[4] * FLTOFIX14(-0.2235) +
                ib[5] * FLTOFIX14(0.2568) + ib[6] * FLTOFIX14(-0.3205) +
                ib[7] * FLTOFIX14(0.3841));
            ib[0] = ib[0] - FIX14TOI(
                ib[1] * FLTOFIX14(-1.1129) + ib[2] * FLTOFIX14(0.0570) +
                ib[3] * FLTOFIX14(-0.4712) + ib[4] * FLTOFIX14(0.1029) +
                ib[5] * FLTOFIX14(0.0156) + ib[6] * FLTOFIX14(-0.4486) +
                ib[7] * FLTOFIX14(-0.4619));
            ib[7] = -ib[7] + FIX14TOI(
                ib[0] * FLTOFIX14(1.1648) + ib[1] * FLTOFIX14(-2.8234) +
                ib[2] * FLTOFIX14(0.5375) + ib[3] * FLTOFIX14(-0.6058) +
                ib[4] * FLTOFIX14(1.2228) + ib[5] * FLTOFIX14(-0.3805) +
                ib[6] * FLTOFIX14(0.0288));

            oblk[0] = ib[5]; oblk[8] = ib[6];
            oblk[16] = ib[0]; oblk[24] = ib[3];
            oblk[32] = ib[2]; oblk[40] = ib[7];
            oblk[48] = ib[4]; oblk[56] = ib[1];
        }
#endif

#if false
        // void TransIRDCT(short *iblk, byte *oblk)
        public static void TransBlock(ShortPtr iblk, ShortPtr oblk, int dcbias)
        {
            IntPtr s = new IntPtr(PDJPG.DCTSZ2);
            IntPtr t = new IntPtr(PDJPG.DCTSZ2);
            int i, j;

            TransIRDCT_Vert(iblk + 0, s + 0);
            TransIRDCT_Vert(iblk + 1, s + 1);
            TransIRDCT_Vert(iblk + 2, s + 2);
            TransIRDCT_Vert(iblk + 3, s + 3);
            TransIRDCT_Vert(iblk + 4, s + 4);
            TransIRDCT_Vert(iblk + 5, s + 5);
            TransIRDCT_Vert(iblk + 6, s + 6);
            TransIRDCT_Vert(iblk + 7, s + 7);

            TransIRDCT_Horiz(s + 0, t + 0);
            TransIRDCT_Horiz(s + 8, t + 8);
            TransIRDCT_Horiz(s + 16, t + 16);
            TransIRDCT_Horiz(s + 24, t + 24);
            TransIRDCT_Horiz(s + 32, t + 32);
            TransIRDCT_Horiz(s + 40, t + 40);
            TransIRDCT_Horiz(s + 48, t + 48);
            TransIRDCT_Horiz(s + 56, t + 56);

            for (i = 0; i < 64; i++)
            {
                j = (t[i]) + dcbias;
                //		if(j&(~255))j=(j<0)?0:((j>255)?255:j);
                oblk[i] = j;
            }
        }
#endif

    }

    public class PDJPG_TransAuDCT
    {
        public static void TransBlock_None(ShortPtr iblk, ShortPtr oblk, int dcbias, int quant)
        {
            int i, j, k, l;

            for (i = 0; i < 8; i++)
            {
                for (j = 0; j < 8; j++)
                {
                    k = iblk[i * 8 + j];
                    oblk[i * 8 + j] = (k - dcbias + (quant / 2)) / quant;
                }
            }
        }

        public static void TransBlock_Left(ShortPtr iblk, ShortPtr oblk, int dcbias, int quant)
        {
            int i, j, k, l;

            for (i = 0; i < 8; i++)
            {
                l = dcbias;
                for (j = 0; j < 8; j++)
                {
                    k = iblk[i * 8 + j];
                    k = (k - l + (quant / 2)) / quant;
                    oblk[i * 8 + j] = k;
                    l = l + (k * quant);
                }
            }
        }

        public static void TransBlock_Up(ShortPtr iblk, ShortPtr oblk, int dcbias, int quant)
        {
            int i, j, k, l;

            for (i = 0; i < 8; i++)
            {
                l = dcbias;
                for (j = 0; j < 8; j++)
                {
                    k = iblk[j * 8 + i];
                    k = (k - l + (quant / 2)) / quant;
                    oblk[i * 8 + j] = k;
                    l = l + (k * quant);
                }
            }
        }

        public static void TransBlock_Avg(ShortPtr iblk, ShortPtr oblk, int dcbias, int quant)
        {
            short[] tblk = new short[64];
            int a, b, c;
            int i, j, k, l;

            for (i = 0; i < 8; i++)
            {
                for (j = 0; j < 8; j++)
                {
                    //a = (j > 0) ? iblk[(i - 0) * 8 + (j - 1)] : dcbias;
                    //b = (i > 0) ? iblk[(i - 1) * 8 + (j - 0)] : dcbias;
                    // c = ((i > 0) && (j > 0)) ? iblk[(i - 1) * 8 + (j - 1)] : dcbias;

                    a = (j > 0) ? tblk[(i - 0) * 8 + (j - 1)] : dcbias;
                    b = (i > 0) ? tblk[(i - 1) * 8 + (j - 0)] : dcbias;

                    l = (a + b) / 2;

                    k = iblk[i * 8 + j];
                    k = (k - l + (quant / 2)) / quant;
                    oblk[i * 8 + j] = k;
                    l = l + (k * quant);
                    tblk[i * 8 + j] = (short)l;
                }
            }
        }

        public static int Paeth(int a, int b, int c)
        {
            int p, pa, pb, pc;

            p = a + b - c;
            pa = (p > a) ? (p - a) : (a - p);
            pb = (p > b) ? (p - b) : (b - p);
            pc = (p > c) ? (p - c) : (c - p);

            p = (pa <= pb) ? ((pa <= pc) ? a : c) : ((pb <= pc) ? b : c);
            return (p);
        }

        public static void TransBlock_Paeth(ShortPtr iblk, ShortPtr oblk, int dcbias, int quant)
        {
            short[] tblk = new short[64];
            int a, b, c;
            int i, j, k, l;

            for (i = 0; i < 8; i++)
            {
                for (j = 0; j < 8; j++)
                {
                    //a = (j > 0) ? iblk[(i - 0) * 8 + (j - 1)] : dcbias;
                    //b = (i > 0) ? iblk[(i - 1) * 8 + (j - 0)] : dcbias;
                    //c = ((i > 0) && (j > 0)) ? iblk[(i - 1) * 8 + (j - 1)] : dcbias;

                    a = (j > 0) ? tblk[(i - 0) * 8 + (j - 1)] : dcbias;
                    b = (i > 0) ? tblk[(i - 1) * 8 + (j - 0)] : dcbias;
                    c = ((i > 0) && (j > 0)) ? tblk[(i - 1) * 8 + (j - 1)] : dcbias;

                    l = Paeth(a, b, c);

                    k = iblk[i * 8 + j];
                    k = (k - l + (quant / 2)) / quant;
                    oblk[i * 8 + j] = k;
                    l = l + (k * quant);
                    tblk[i * 8 + j] = (short)l;

                    //k = iblk[i * 8 + j];
                    //oblk[i * 8 + j] = k - l;
                }
            }
        }

        public static void TransBlock_Linear(ShortPtr iblk, ShortPtr oblk, int dcbias, int quant)
        {
            short[] tblk = new short[64];
            int a, b, c;
            int i, j, k, l;

            for (i = 0; i < 8; i++)
            {
                for (j = 0; j < 8; j++)
                {
                    //a = (j > 0) ? iblk[(i - 0) * 8 + (j - 1)] : dcbias;
                    //b = (i > 0) ? iblk[(i - 1) * 8 + (j - 0)] : dcbias;
                    //c = ((i > 0) && (j > 0)) ? iblk[(i - 1) * 8 + (j - 1)] : dcbias;

                    a = (j > 0) ? tblk[(i - 0) * 8 + (j - 1)] : dcbias;
                    b = (i > 0) ? tblk[(i - 1) * 8 + (j - 0)] : dcbias;
                    c = ((i > 0) && (j > 0)) ? tblk[(i - 1) * 8 + (j - 1)] : dcbias;

                    l = a + b - c;

                    k = iblk[i * 8 + j];
                    k = (k - l + (quant / 2)) / quant;
                    oblk[i * 8 + j] = k;
                    l = l + (k * quant);
                    tblk[i * 8 + j] = (short)l;

                    //k = iblk[i * 8 + j];
                    //oblk[i * 8 + j] = k - l;
                }
            }
        }

        public static void TransBlock2(ShortPtr iblk, ShortPtr oblk, int dcbias, int tag, int quant)
        {
            switch (tag&255)
            {
                case 0:
                    PDJPG_TransRDCT.TransBlock(iblk, oblk, dcbias);
                    break;
                case 1:
                    PDJPG_TransAuDCT.TransBlock_Left(iblk, oblk, dcbias, quant);
                    break;
                case 2:
                    PDJPG_TransAuDCT.TransBlock_Up(iblk, oblk, dcbias, quant);
                    break;
                case 3:
                    PDJPG_TransAuDCT.TransBlock_Avg(iblk, oblk, dcbias, quant);
                    break;
                case 4:
                    PDJPG_TransAuDCT.TransBlock_Paeth(iblk, oblk, dcbias, quant);
                    break;
                case 5:
                    PDJPG_TransAuDCT.TransBlock_Linear(iblk, oblk, dcbias, quant);
                    break;
                case 6:
                    PDJPG_TransAuDCT.TransBlock_None(iblk, oblk, dcbias, quant);
                    break;
                default:
                    break;
            }
        }

        public static void TransBlock(PDJPG_Context ctx, ShortPtr iblk, ShortPtr oblk, int dcbias, int qid, out int tag)
        {
            ShortPtr tblk = new ShortPtr(64);
            double err, berr;
            int i, j, k, btag, bqt;

            if (false)
            {
                tag = 0;
                TransBlock2(iblk, oblk, dcbias, tag, 1);
                return;
            }

            k = (100 - (ctx.jpg_qfl&255)) / 10;
            bqt = k;
            //bqt = k * k;
            //bqt = 0;

            btag = 0; berr = 999999999.0;
            for (i = 0; i < 7; i++)
            {
                TransBlock2(iblk, tblk, dcbias, i, bqt + 1);
                ctx.block.QuantBlock(ctx, tblk, tblk, qid, i);

                err = 0;
                for (j = 0; j < 64; j++)
                {
                    k = tblk[j];
                    if (k < 0) k = -k;
                    err += k;
                }
                if (err < berr)
                    { btag = i; berr = err; }
            }

            //if (btag != 0)
               //Console.Write("P={0}Q{1} ", btag, bqt);

            tag = btag;
            if (btag != 0) tag |= (bqt << 8);

            TransBlock2(iblk, oblk, dcbias, tag, bqt + 1);
        }
    }

    public class PDJPG_TransAuIDCT
    {
        public static void TransBlock_None(ShortPtr iblk, ShortPtr oblk, int dcbias, int quant)
        {
            int i, j, k, l;

            for (i = 0; i < 8; i++)
            {
                for (j = 0; j < 8; j++)
                {
                    k = iblk[i * 8 + j] * quant;
                    oblk[i * 8 + j] = k + dcbias;
                }
            }
        }

        public static void TransBlock_Left(ShortPtr iblk, ShortPtr oblk, int dcbias, int quant)
        {
            int i, j, k, l;

            for (i = 0; i < 8; i++)
            {
                l = dcbias;
                for (j = 0; j < 8; j++)
                {
                    k = iblk[i * 8 + j] * quant;
                    oblk[i * 8 + j] = k + l;
                    l += k;
                }
            }
        }

        public static void TransBlock_Up(ShortPtr iblk, ShortPtr oblk, int dcbias, int quant)
        {
            int i, j, k, l;

            for (i = 0; i < 8; i++)
            {
                l = dcbias;
                for (j = 0; j < 8; j++)
                {
                    k = iblk[j * 8 + i] * quant;
                    oblk[j * 8 + i] = k + l;
                    l += k;
                }
            }
        }

        public static void TransBlock_Avg(ShortPtr iblk, ShortPtr oblk, int dcbias, int quant)
        {
            int a, b, c;
            int i, j, k, l;

            for (i = 0; i < 8; i++)
            {
                for (j = 0; j < 8; j++)
                {
                    a = (j > 0) ? oblk[(i - 0) * 8 + (j - 1)] : dcbias;
                    b = (i > 0) ? oblk[(i - 1) * 8 + (j - 0)] : dcbias;
                    // c = ((i > 0) && (j > 0)) ? oblk[(i - 1) * 8 + (j - 1)] : dcbias;
                    l = (a + b) / 2;

                    k = iblk[i * 8 + j] * quant;
                    oblk[i * 8 + j] = k + l;
                }
            }
        }

        public static int Paeth(int a, int b, int c)
        {
            int p, pa, pb, pc;

            p = a + b - c;
            pa = (p > a) ? (p - a) : (a - p);
            pb = (p > b) ? (p - b) : (b - p);
            pc = (p > c) ? (p - c) : (c - p);

            p = (pa <= pb) ? ((pa <= pc) ? a : c) : ((pb <= pc) ? b : c);
            return (p);
        }

        public static void TransBlock_Paeth(ShortPtr iblk, ShortPtr oblk, int dcbias, int quant)
        {
            int a, b, c;
            int i, j, k, l;

            for (i = 0; i < 8; i++)
            {
                for (j = 0; j < 8; j++)
                {
                    a = (j > 0) ? oblk[(i - 0) * 8 + (j - 1)] : dcbias;
                    b = (i > 0) ? oblk[(i - 1) * 8 + (j - 0)] : dcbias;
                    c = ((i > 0) && (j > 0)) ? oblk[(i - 1) * 8 + (j - 1)] : dcbias;
                    l = Paeth(a, b, c);

                    k = iblk[i * 8 + j];
                    oblk[i * 8 + j] = (k * quant) + l;
                }
            }
        }

        public static void TransBlock_Linear(ShortPtr iblk, ShortPtr oblk, int dcbias, int quant)
        {
            int a, b, c;
            int i, j, k, l;

            for (i = 0; i < 8; i++)
            {
                for (j = 0; j < 8; j++)
                {
                    a = (j > 0) ? oblk[(i - 0) * 8 + (j - 1)] : dcbias;
                    b = (i > 0) ? oblk[(i - 1) * 8 + (j - 0)] : dcbias;
                    c = ((i > 0) && (j > 0)) ? oblk[(i - 1) * 8 + (j - 1)] : dcbias;
                    l = a + b - c;

                    k = iblk[i * 8 + j] * quant;
                    oblk[i * 8 + j] = k + l;
                }
            }
        }

        public static void TransBlock(ShortPtr iblk, ShortPtr oblk, int dcbias, int tag)
        {
            int quant;
            //if (tag != 0)
                //Console.Write("{0} ", tag);

            quant = ((tag >> 8) & 255) + 1;
            //quant = 1;
            switch (tag & 255)
            {
                case 0:
                    PDJPG_TransIRDCT.TransBlock(iblk, oblk, dcbias);
                    break;
                case 1:
                    PDJPG_TransAuIDCT.TransBlock_Left(iblk, oblk, dcbias, quant);
                    break;
                case 2:
                    PDJPG_TransAuIDCT.TransBlock_Up(iblk, oblk, dcbias, quant);
                    break;
                case 3:
                    PDJPG_TransAuIDCT.TransBlock_Avg(iblk, oblk, dcbias, quant);
                    break;
                case 4:
                    PDJPG_TransAuIDCT.TransBlock_Paeth(iblk, oblk, dcbias, quant);
                    break;
                case 5:
                    PDJPG_TransAuIDCT.TransBlock_Linear(iblk, oblk, dcbias, quant);
                    break;
                case 6:
                    PDJPG_TransAuIDCT.TransBlock_None(iblk, oblk, dcbias, quant);
                    break;
                default:
                    break;
            }
        }
    }
}
