using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using BGBTechBTJ;

namespace ConsoleApplication1
{
    class Program
    {
        static void PrintFixedRDCT()
        {
            double[] vals = new double[] {
                1.1648, -2.8234, 0.5375, -0.6058, 1.2228, -0.3805, 0.0288, 
                -1.1129, 0.0570, -0.4712, 0.1029, 0.0156, -0.4486, -0.4619,
                -0.0685, 0.2708, -0.2708, -0.2235, 0.2568, -0.3205, 0.3841,
                -0.0364, -1.7104, -1.0000, 0.3066, 0.6671, -0.5953, 0.2039,
                0.7957, 0.9664, 0.4439, 0.6173, -0.1422, 1.0378, -0.1700,
                0.4591, 0.4108, -0.2073, -1.0824, 0.7071, 0.8873, -0.2517,
                -0.6573, 0.5810, -0.2931, -0.5307, -0.8730, -0.1594, -0.3560,
                1.0024, -0.7180, -0.0928, -0.0318, 0.4170, 1.1665, 0.4904,
                1.1020, -2.0306, -0.3881, 0.6561, 1.2405, 1.6577, -1.1914};
            int i, j, k;

            for (i = 0; i < 9; i++)
            {
                for (j = 0; j < 7; j++)
                {
                    k = (int)Math.Round(vals[i * 7 + j] * 8192);
                    Console.Write("{0} ", k);
                }
                Console.WriteLine();
            }
        }

        static void PrintFixedIRDCT()
        {
            double[] vals = new double[] {
                1.1020, -2.0306, -0.3881, 0.6561, 1.2405, 1.6577, -1.1914,
                1.0024, -0.7180, -0.0928, -0.0318, 0.4170, 1.1665, 0.4904,
                -0.6573, 0.5810, -0.2931, -0.5307, -0.8730, -0.1594, -0.3560,
                0.4591, 0.4108, -0.2073, -1.0824, 0.7071, 0.8873, -0.2517,
                0.7957, 0.9664, 0.4439, 0.6173, -0.1422, 1.0378, -0.1700,
                -0.0364, -1.7104, -1.0000, 0.3066, 0.6671, -0.5953, 0.2039,
                -0.0685, 0.2708, -0.2708, -0.2235, 0.2568, -0.3205, 0.3841,
                -1.1129, 0.0570, -0.4712, 0.1029, 0.0156, -0.4486, -0.4619,
                1.1648, -2.8234, 0.5375, -0.6058, 1.2228, -0.3805, 0.0288};
            int i, j, k;

            for (i = 0; i < 9; i++)
            {
                for (j = 0; j < 7; j++)
                {
                    k = (int)Math.Round(vals[i * 7 + j] * 8192);
                    Console.Write("{0} ", k);
                }
                Console.WriteLine();
            }
        }

        static double SCurve(double a)
        {
            return (a * a * (3 - 2 * a));
        }
        static double SCurveClamp(double a)
        {
            double b;
            b = SCurve(a);
            b = (b > 0.0) ? ((b < 1.0) ? b : 1.0) : 0.0;
            return b;
        }

        static double TCurve(double a)
        {
            //return (1 - ((1 - a) * (1 - a)));
            //return (1 - (1 - 2 * a + a * a));
            return (2 * a - a * a);
        }

        static double TCurveClamp(double a)
        {
            double b;
            b = TCurve(a);
            b = (b > 0.0) ? ((b < 1.0) ? b : 1.0) : 0.0;
            return b;
        }

        static void PrintSCurve()
        {
            double f, g;
            int i;

            for (i = 0; i < 21; i++)
            {
                f = i / 20.0;
                g = TCurveClamp(f);
                Console.WriteLine("{0} {1} {2}", i, f, g);
            }
        }

        static void Main(string[] args)
        {
            byte[] encBuf;
            byte[] img;
            byte[] imgb;
            BytePtr obuf;
            int xs, ys, xs1, ys1;
            long er, eg, eb, ea;
            int i, j, k, sz, qf, n;

//            xs = 256; ys = 256;
            xs = 800; ys = 600;
            img = new byte[xs * ys * 4];

            PrintFixedIRDCT();

            PrintSCurve();
#if true
            //test pattern 0
            for (i = 0; i < ys; i++)
                for (j = 0; j < xs; j++)
                {
                    img[(i * xs + j) * 4 + 0] = (byte)(((i & 3) == 0) ? 255 : 0);
                    img[(i * xs + j) * 4 + 1] = (byte)((((i + j) & 3) == 0) ? 255 : 0);
                    img[(i * xs + j) * 4 + 2] = (byte)(((j & 3) == 0) ? 255 : 0);
                    //img[(i * xs + j) * 4 + 3] = (byte)(((((i>>2)+(j>>2)) & 1) == 0) ? 255 : 0);
                    img[(i * xs + j) * 4 + 3] = (byte)64;
                }
#endif

            //qf = 90 | PDJPG.PDJPG_QFL_444;
            //qf = 100 | PDJPG.PDJPG_QFL_LOSSLESS;
            //qf = 100 | PDJPG.PDJPG_QFL_LOSSLESS | PDJPG.PDJPG_QFL_MEGABLOCK;
            //qf = 100 | PDJPG.PDJPG_QFL_LOSSLESS | PDJPG.PDJPG_QFL_ALTVLC;
            //qf = 100 | PDJPG.PDJPG_QFL_LOSSLESS | PDJPG.PDJPG_QFL_RANGECODER;
            //qf = 100 | PDJPG.PDJPG_QFL_LOSSLESS | PDJPG.PDJPG_QFL_RANGECODER | PDJPG.PDJPG_QFL_ALTVLC;
            qf = 100 | PDJPG.PDJPG_QFL_LOSSLESS | PDJPG.PDJPG_QFL_AUTODCT;
            //qf = 95 | PDJPG.PDJPG_QFL_AUTODCT;
            //qf = 95 | PDJPG.PDJPG_QFL_AUTODCT | PDJPG.PDJPG_QFL_444;
            //qf = 95;

            encBuf = new byte[1 << 24];

            obuf = new BytePtr(encBuf);
            sz = PDJPG.PDJPG_EncodeComponent(img, null, null, null, obuf, xs, ys, qf);

            Console.WriteLine("Test Encoded Sz={0}", sz);

            imgb = new byte[4096 * 4096 * 4];
            PDJPG.PDJPG_DecodeComponent(new BytePtr(encBuf,0,true), sz, imgb, null, null, null, out xs1, out ys1);

            Console.WriteLine("Test Decoded Xs={0} Ys={1}", xs1, ys1);

            er = 0; eg = 0; eb = 0; ea = 0;
            for (i = 0; i < ys; i++)
                for (j = 0; j < xs; j++)
                {
                    k = imgb[(i * xs + j) * 4 + 0] - img[(i * xs + j) * 4 + 0];
                    er += k * k;
                    k = imgb[(i * xs + j) * 4 + 1] - img[(i * xs + j) * 4 + 1];
                    eg += k * k;
                    k = imgb[(i * xs + j) * 4 + 2] - img[(i * xs + j) * 4 + 2];
                    eb += k * k;
                    k = imgb[(i * xs + j) * 4 + 3] - img[(i * xs + j) * 4 + 3];
                    ea += k * k;
                }

            n = xs * ys;
            Console.WriteLine("RMSE: Er={0} Eg={1} Eb={2} Ea={3} N={4}",
                Math.Sqrt(er/n),
                Math.Sqrt(eg/n),
                Math.Sqrt(eb/n),
                Math.Sqrt(ea/n),
                n);

            Console.Read();
        }
    }
}
