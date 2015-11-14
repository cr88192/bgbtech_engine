using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace BGBTechBTJ
{
    public partial class PDJPG
    {
        public static int PDJPG_MarkerSOS(PDJPG_Context ctx, BytePtr buf)
        {
            short[] dbuf = new short[4 * 64];
            byte[] ch = new byte[4];
            byte[] cv = new byte[4];
            byte[] qid = new byte[4];
            byte[] step = new byte[4];
            int[] xi = new int[4];
            int[] yi = new int[4];
            int[] wi = new int[4];
            int[] hi = new int[4];
            byte[] cdt = new byte[4];
            byte[] cat = new byte[4];
            int[] ccnt = new int[4];

            BytePtr s;
            int i, j, k, l, i1, j1, k1;
            int w, h, n, ns, rt;

            if (ctx.megablock)
            {
                i = PDJPG_MarkerMegablockSOS(ctx, buf);
                return i;
            }

            l = (buf[0] << 8) | buf[1];
            //	printf("ns %d\n", buf[2]);

            ns = buf[2]; rt = 0;
            s = buf + 3;
            for (i = 0; i < buf[2]; i++)
            {
                //		printf("%d %d %d\n", s[0], s[1]>>4, s[1]&15);

                cdt[i] = (byte)(s[1] >> 4);
                cat[i] = (byte)(s[1] & 15);

                for (j = 0; j < ctx.jpg_nc; j++)
                    if (ctx.jpg_cid[j] == s[0])
                    {
                        ch[i] = ctx.jpg_ch[j];
                        cv[i] = ctx.jpg_cv[j];
                        qid[i] = ctx.jpg_qid[j];
                        xi[i] = ctx.jpg_cxi[j];
                        yi[i] = ctx.jpg_cyi[j];

                        step[i] = (byte)(ch[i] * cv[i]);
                        break;
                    }

                w = (xi[i] + 7) / 8;
                h = (yi[i] + 7) / 8;
                //		w=(xi[i]+15)/8;
                //		h=(yi[i]+15)/8;
                wi[i] = w;
                hi[i] = h;

                n = (w + 1) * (h + 2);
                ctx.jpg_scid[i] = s[0];
                ctx.jpg_scn[i] = j;
                if (ctx.jpg_scbuf[i].isNull)
                    ctx.jpg_scbuf[i] = new ShortPtr(n * 64);
                if (ctx.jpg_sibuf[i].isNull)
                    ctx.jpg_sibuf[i] = new ShortPtr(n * 64);
                if (ctx.jpg_stbuf[i].isNull)
                    ctx.jpg_stbuf[i] = new IntPtr(n);

                //		memset(ctx.jpg_sibuf[i], 0, n*64);

                ccnt[i] = 0;

                s += 2;
            }
            s += 3;

            i = (ctx.xs + ctx.jpg_chm * 8 - 1) / (ctx.jpg_chm * 8);
            j = (ctx.ys + ctx.jpg_chn * 8 - 1) / (ctx.jpg_chn * 8);
            n = i * j;

            ctx.huff.InitStream(ctx, s);

            for (i = 0; i < (4 * 64); i++) dbuf[i] = 0;

            for (i = 0; i < 4; i++)
                ctx.cstb[i] = 0;

                rt = 0;
            for (i = 0; i < n; i++)
            {
                for (j = 0; j < ns; j++)
                {
                    for (k = 0; k < cv[j]; k++)
                    {
                        for (l = 0; l < ch[j]; l++)
                        {
                            if (rt < 0) break;

                            i1 = (i / (wi[j] / ch[j])) * ch[j];
                            j1 = (i % (wi[j] / ch[j])) * ch[j];
                            k1 = ((i1 + k) * wi[j]) + (j1 + l);

                            rt = ctx.block.DecodeBlock(ctx, new ShortPtr(dbuf, j * 64),
                                cdt[j] * 2 + 0, cat[j] * 2 + 1, j, k1, n);
                            if (rt < 0) break;
                            ctx.block.DequantBlock(ctx, new ShortPtr(dbuf, j * 64),
                                new ShortPtr(ctx.jpg_scbuf[j], k1 * 64), qid[j], ctx.jpg_stbuf[j][k1]);

                            if (ctx.auto_dct)
                            {
                                PDJPG_TransAuIDCT.TransBlock(new ShortPtr(ctx.jpg_scbuf[j], k1 * 64),
                                    new ShortPtr(ctx.jpg_sibuf[j], k1 * 64), 128, ctx.jpg_stbuf[j][k1]);
                            }else
                            if (ctx.jpg_rdct)
                            {
                                PDJPG_TransIRDCT.TransBlock(new ShortPtr(ctx.jpg_scbuf[j], k1 * 64),
                                    new ShortPtr(ctx.jpg_sibuf[j], k1 * 64), 128);
                            }
                            else
                            {
                                PDJPG_TransIDCT.TransBlock(new ShortPtr(ctx.jpg_scbuf[j], k1 * 64),
                                    new ShortPtr(ctx.jpg_sibuf[j], k1 * 64), 128);

                                //						PDJPG_TransIQDCT(ctx.jpg_scbuf[j]+k1*64,
                                //							ctx.jpg_sibuf[j]+k1*64, 128);
                            }
                        }
                        if (rt < 0) break;
                    }
                    if (rt < 0) break;
                }
                if (rt < 0) break;
            }

            if (rt < 0)
            {
                printf("PDJPG_MarkerSOS: Bad Image\n");
                return (-1);
            }

            i = ctx.huff.cs - buf;
            return (i);

            //	return(0);
        }

#if true
        public static int PDJPG_MarkerMegablockSOS(PDJPG_Context ctx, BytePtr buf)
        {
            short[] dbuf = new short[4 * 4096];
            byte[] ch = new byte[4];
            byte[] cv = new byte[4];
            byte[] qid = new byte[4];
            byte[] step = new byte[4];
            int[] xi = new int[4];
            int[] yi = new int[4];
            int[] wi = new int[4];
            int[] hi = new int[4];
            byte[] cdt = new byte[4];
            byte[] cat = new byte[4];
            int[] ccnt = new int[4];

            BytePtr s;
            int i, j, k, l, i1, j1, k1;
            int w, h, n, ns, rt;

            l = (buf[0] << 8) | buf[1];
            //	printf("ns %d\n", buf[2]);

            ns = buf[2]; rt = 0;
            s = buf + 3;
            for (i = 0; i < buf[2]; i++)
            {
                //		printf("%d %d %d\n", s[0], s[1]>>4, s[1]&15);

                cdt[i] = (byte)(s[1] >> 4);
                cat[i] = (byte)(s[1] & 15);

                for (j = 0; j < ctx.jpg_nc; j++)
                    if (ctx.jpg_cid[j] == s[0])
                    {
                        ch[i] = ctx.jpg_ch[j];
                        cv[i] = ctx.jpg_cv[j];
                        qid[i] = ctx.jpg_qid[j];
                        xi[i] = ctx.jpg_cxi[j];
                        yi[i] = ctx.jpg_cyi[j];

                        step[i] = (byte)(ch[i] * cv[i]);
                        break;
                    }

                w = (xi[i] + 63) / 64;
                h = (yi[i] + 63) / 64;
                //		w=(xi[i]+15)/8;
                //		h=(yi[i]+15)/8;
                wi[i] = w;
                hi[i] = h;

                n = (w + 1) * (h + 2);
                ctx.jpg_scid[i] = s[0];
                ctx.jpg_scn[i] = j;
                if (ctx.jpg_scbuf[i].isNull)
                    ctx.jpg_scbuf[i] = new ShortPtr(n * 4096);
                if (ctx.jpg_sibuf[i].isNull)
                    ctx.jpg_sibuf[i] = new ShortPtr(n * 4096);

                //		memset(ctx.jpg_sibuf[i], 0, n*64);

                ccnt[i] = 0;
                ctx.cstb[i] = 0;

                s += 2;
            }
            s += 3;

            i = (ctx.xs + ctx.jpg_chm * 64 - 1) / (ctx.jpg_chm * 64);
            j = (ctx.ys + ctx.jpg_chn * 64 - 1) / (ctx.jpg_chn * 64);
            n = i * j;

            ctx.huff.InitStream(ctx, s);

            for (i = 0; i < (4 * 4096); i++) dbuf[i] = 0;

            rt = 0;
            for (i = 0; i < n; i++)
            {
                for (j = 0; j < ns; j++)
                {
                    for (k = 0; k < cv[j]; k++)
                    {
                        for (l = 0; l < ch[j]; l++)
                        {
                            if (rt < 0) break;

                            i1 = (i / (wi[j] / ch[j])) * ch[j];
                            j1 = (i % (wi[j] / ch[j])) * ch[j];
                            k1 = ((i1 + k) * wi[j]) + (j1 + l);

                            rt = ctx.block.DecodeMegaBlock(ctx, new ShortPtr(dbuf, j * 4096),
                                cdt[j] * 2 + 0, cat[j] * 2 + 1, i, n);
                            if (rt < 0) break;
                            ctx.block.DequantMegaBlock(ctx, new ShortPtr(dbuf, j * 4096),
                                new ShortPtr(ctx.jpg_scbuf[j], k1 * 4096), qid[j]);

                            if (ctx.jpg_rdct)
                            {
                                PDJPG_TransIRDCT.TransMegaBlock(new ShortPtr(ctx.jpg_scbuf[j], k1 * 4096),
                                    new ShortPtr(ctx.jpg_sibuf[j], k1 * 4096), 128);
                            }
                            else
                            {
                                //PDJPG_TransIDCT.TransBlock(new ShortPtr(ctx.jpg_scbuf[j], k1 * 64),
                                    //new ShortPtr(ctx.jpg_sibuf[j], k1 * 64), 128);
                                PDJPG_TransIRDCT.TransMegaBlock(new ShortPtr(ctx.jpg_scbuf[j], k1 * 4096),
                                    new ShortPtr(ctx.jpg_sibuf[j], k1 * 4096), 128);
                            }
                        }
                        if (rt < 0) break;
                    }
                    if (rt < 0) break;
                }
                if (rt < 0) break;
            }

            if (rt < 0)
            {
                printf("PDJPG_MarkerSOS: Bad Image\n");
                return (-1);
            }

            i = ctx.huff.cs - buf;
            return (i);

            //	return(0);
        }
#endif

        public static int PDJPG_MarkerSOF0(PDJPG_Context ctx, BytePtr buf)
        {
            BytePtr s;
            int i, j, k, l, m, n;

            l = (buf[0] << 8) | buf[1];
            //	printf("p %d\n", buf[2]);

            i = (buf[3] << 8) | buf[4];
            j = (buf[5] << 8) | buf[6];

            ctx.xs = j;
            ctx.ys = i;
            //	printf("size %dx%d\n", j, i);

            m = 0; n = 0;

            k = buf[7];
            s = buf + 8;
            for (i = 0; i < k; i++)
            {
                ctx.jpg_cid[i] = (byte)(s[0]);
                ctx.jpg_ch[i] = (byte)(s[1] >> 4);
                ctx.jpg_cv[i] = (byte)(s[1] & 15);
                ctx.jpg_qid[i] = (byte)(s[2]);

                if (ctx.jpg_ch[i] > m) m = ctx.jpg_ch[i];
                if (ctx.jpg_cv[i] > n) n = ctx.jpg_cv[i];

                //		printf("CID %d: %d %d %d\n", ctx.jpg_cid[i],
                //			ctx.jpg_ch[i], ctx.jpg_cv[i], ctx.jpg_qid[i]);
                s += 3;
            }

            ctx.jpg_chm = m;
            ctx.jpg_chn = n;

            for (i = 0; i < k; i++)
            {
                ctx.jpg_cxi[i] = (ctx.jpg_ch[i] * ctx.xs) / m;
                ctx.jpg_cyi[i] = (ctx.jpg_cv[i] * ctx.ys) / n;
            }

            ctx.jpg_nc = k;

            return (0);
        }

        public static int PDJPG_MarkerDQT(PDJPG_Context ctx, BytePtr buf)
        {
            BytePtr s;
            BytePtr se;
            int i, j, l;

            l = (buf[0] << 8) | buf[1];

            s = buf + 2;
            se = buf + l;

            while (s < se)
            {
                i = (s[0] >> 4) & 15;
                if (i != 0)
                {
                    //			printf("bad bits %d\n", i);
                    return (-1);
                }
                i = s[0] & 15;
                //		printf("dest %d\n", i);
                if (ctx.megablock)
                {
                    for (j = 0; j < 64; j++)
                        ctx.jpg_qt[i * 256 + pdjpg_zigzag2[j]] = (byte)(s[j + 1]);
                    for (j = 0; j < 64; j++)
                        ctx.jpg_qt[i * 256 + 64 + pdjpg_zigzag2[j]] = (byte)(s[j + 64 + 1]);
                    s += 129;
                }
                else
                {
                    for (j = 0; j < 64; j++)
                        ctx.jpg_qt[i * 64 + pdjpg_zigzag2[j]] = (byte)(s[j + 1]);
                    s += 65;
                }
            }

            return (s - buf);
            //	return(0);
        }

        public static int PDJPG_MarkerDHT(PDJPG_Context ctx, BytePtr buf)
        {
            if (ctx.huff == null)
            {
                ctx.huff = new PDJHUFF(ctx);
            }

            return ctx.huff.MarkerDHT(ctx, buf);
        }

        public static int PDJPG_MarkerAPP0(PDJPG_Context ctx, BytePtr buf)
        {
            return (0);
        }

        public static int PDJPG_MarkerAPP9(PDJPG_Context ctx, BytePtr buf)
        {
            //	int i;
            buf += 2;

            if (!buf.memcmp("JPSERM", 6))
            {
                ctx.jpg_rdct = true;
                return (1);
            }

            return (0);
        }

        public static int PDJPG_MarkerAPP11(PDJPG_Context ctx, BytePtr buf)
        {
            //	int cr, cg, cb, ca;

            buf += 2;

            if (!buf.strcmp("AlphaColor"))
            {
                buf += buf.strlen() + 1;
                //		sscanf_s(buf, "%d %d %d %d", out cr, cg, &cb, &ca);
                //		PDJPG_SetContextAlphaColor(ctx, cr, cg, cb, ca);
                return (1);
            }

            return (0);
        }

        public static int PDJPG_MarkerDMLT_DQT(PDJPG_Context ctx, BytePtr buf, int tab)
        {
            int i, j, l, nb;

            ctx.huff.InitStream(ctx, buf);

            i = ctx.huff.ReadNBits(ctx, 4);
            nb = ctx.huff.ReadNBits(ctx, 4);

            if (i != 0)
            {
                return (-1);
            }

            l = 0;
            for (i = 0; i < 64; i++)
            {
                j = ((PDJHUFF)ctx.huff).ReadSRice(ctx, nb);
                l = l + j;
                ctx.jpg_qt[tab * 64 + pdjpg_zigzag2[i]] = (byte)l;
            }

            return 1;
        }

        public static int PDJPG_MarkerDMLT(PDJPG_Context ctx, BytePtr buf)
        {
            BytePtr cs, css;
            int i, j, k, sz, sz1, tn, tag;

            sz = (ushort)((buf[0] << 8) | buf[1]);

            buf += 2;

            i = (buf[0] << 24) | (buf[1] << 16) | (buf[2] << 8) | buf[3];
            //	printf("PDJPG_MarkerAPP12: %08X\n", i);

            if (buf.memcmp("DMLT", 4))
                return (-1);

            cs = buf + 4;

            tag = cs.next();
            sz1 = (int)cs.ReadUVLI();
            css = cs;
            while (tag != 0)
            {
                switch (tag)
                {
                    case PDJPG_DMLT_DVFH:
                        ctx.dvfh_lidx = (cs[2] << 8) + cs[3];
                        break;
                    case PDJPG_DMLT_DHT0: case PDJPG_DMLT_DHT1:
                    case PDJPG_DMLT_DHT2: case PDJPG_DMLT_DHT3:
                    case PDJPG_DMLT_DHT4: case PDJPG_DMLT_DHT5:
                    case PDJPG_DMLT_DHT6: case PDJPG_DMLT_DHT7:
                        i = tag - PDJPG_DMLT_DHT0;
                        ctx.huff.InitStream(ctx, cs);
                        ((PDJHUFF)ctx.huff).BSXRP_DecodeHuffTable(ctx, i);
                        break;
                    case PDJPG_DMLT_DQT0: case PDJPG_DMLT_DQT1:
                    case PDJPG_DMLT_DQT2: case PDJPG_DMLT_DQT3:
                    case PDJPG_DMLT_DQT4: case PDJPG_DMLT_DQT5:
                    case PDJPG_DMLT_DQT6: case PDJPG_DMLT_DQT7:
                        i = tag - PDJPG_DMLT_DQT0;
                        PDJPG_MarkerDMLT_DQT(ctx, cs, i);
                        break;
                    default:
                        break;
                }
                cs = css + sz1;
                tag = cs.next();
                sz1 = (int)cs.ReadUVLI();
                css = cs;
            }
            return 1;
        }

        public static int PDJPG_MarkerAPP12(PDJPG_Context ctx, BytePtr buf)
        {
            BytePtr ob, cs, css;
            int i, sz, sz1, tag;

            ob = buf;
            sz = (ushort)((buf[0] << 8) | buf[1]);

            buf += 2;

            i = (buf[0] << 24) | (buf[1] << 16) | (buf[2] << 8) | buf[3];
            //	printf("PDJPG_MarkerAPP12: %08X\n", i);

            if (!buf.memcmp("DMLT", 4))
            {
                i = PDJPG_MarkerDMLT(ctx, ob);
                return i;
            }

            if (!buf.memcmp("BCST", 4))
            {
                ctx.bcst_version = (ushort)((buf[4] << 8) | buf[5]);
                ctx.bcst_trans = (byte)buf[6];
                ctx.bcst_bpp = (byte)buf[7];
                ctx.bcst_flags =
                    (uint)((buf[8] << 24) | (buf[9] << 16) |
                    (buf[10] << 8) | buf[11]);

                if (sz > 14)
                {
                    ctx.bcst_alpha = (byte)buf[12];
                    ctx.bcst_blend = (byte)buf[13];
                    ctx.bcst_orgx = (ushort)((buf[14] << 8) | buf[15]);
                    ctx.bcst_orgy = (ushort)((buf[16] << 8) | buf[17]);

                    ctx.bcst_minx = (ushort)((buf[18] << 8) | buf[19]);
                    ctx.bcst_miny = (ushort)((buf[20] << 8) | buf[21]);
                    ctx.bcst_maxx = (ushort)((buf[22] << 8) | buf[23]);
                    ctx.bcst_maxy = (ushort)((buf[24] << 8) | buf[25]);
                }
                else
                {
                    ctx.bcst_alpha = 255;
                    ctx.bcst_blend = 0;
                    ctx.bcst_orgx = 0;
                    ctx.bcst_orgy = 0;

                    ctx.bcst_minx = 0;
                    ctx.bcst_miny = 0;
                    ctx.bcst_maxx = 0;
                    ctx.bcst_maxy = 0;
                }

                ctx.jpg_clrtrans = ctx.bcst_trans;

                if ((ctx.bcst_flags & PDJPG_BCSFL_RDCT) != 0)
                    ctx.jpg_rdct = true;
                if ((ctx.bcst_flags & PDJPG_BCSFL_LAYER_HIDDEN) != 0)
                    ctx.layer_hidden = true;
                if ((ctx.bcst_flags & PDJPG_BCSFL_MEGABLOCK) != 0)
                    ctx.megablock = true;
                if ((ctx.bcst_flags & PDJPG_BCSFL_ALTVLC) != 0)
                    ctx.altvlc = true;

                if ((ctx.bcst_flags & PDJPG_BCSFL_AUTODCT) != 0)
                    ctx.auto_dct = true;

                //		printf("PDJPG_MarkerAPP12: BCST %04X %02X %02X %08X\n",
                //			ctx.bcst_version, ctx.bcst_trans,
                //			ctx.bcst_bpp, ctx.bcst_flags);
                return (1);
            }

            if (!buf.memcmp("DRCT", 4))
            {
                if (ctx.huff == null)
                    ctx.huff = new BTJRange(ctx);
                i = ctx.huff.MarkerDHT(ctx, ob);
                return i;
            }

            return (0);
        }

        public static int PDJPG_MarkerAPP14(PDJPG_Context ctx, BytePtr buf)
        {
            buf += 2;

            return (0);
        }

        public static int PDJPG_MarkerAPPN(PDJPG_Context ctx, BytePtr buf)
        {
            return (0);
        }

#if false
public static int PDJPG_GetComponentPixel(PDJPG_Context ctx, int cn, int x, int y)
{
	int i, j, k, l;
	int id, w, h, n, v;

//	id=ctx.jpg_scn[cn];

	i=x*ctx.jpg_schsc[cn];
	j=y*ctx.jpg_scvsc[cn];
//	i=(ctx.jpg_ch[id]*x)/ctx.jpg_chm;
//	j=(ctx.jpg_cv[id]*y)/ctx.jpg_chn;
	k=i>>3;
	l=j>>3;

	w=ctx.jpg_schs[cn];
	h=ctx.jpg_scvs[cn];
//	w=(ctx.jpg_cxi[id]+7)/8;
//	h=(ctx.jpg_cyi[id]+7)/8;
//	w=(ctx.jpg_cxi[id]+15)/8;
//	h=(ctx.jpg_cyi[id]+15)/8;
	n=(l*w)+k;

	k=i&7;
	l=j&7;

	v=ctx.jpg_sibuf[cn][n*64+(l*8+k)];
	return(v);
}
#endif

#if true
        public static int PDJPG_GetComponentPixel(PDJPG_Context ctx, int cn, int x, int y)
        {
            int i, j, k, l;
            int w, n, n1, v;

            //	i=x*ctx.jpg_schsc[cn];
            //	j=y*ctx.jpg_scvsc[cn];

            i = (x * ctx.jpg_schsci[cn]) >> 16;
            j = (y * ctx.jpg_scvsci[cn]) >> 16;

            if (ctx.megablock)
            {
                k = i >> 6;
                l = j >> 6;

                w = ctx.jpg_schs[cn];
                //	h=ctx.jpg_scvs[cn];
                n = (l * w) + k;

                k = (i & 63)>>3;
                l = (j & 63)>>3;
                n1 = (l * 8) + k;

                k = i & 7;
                l = j & 7;

                v = ctx.jpg_sibuf[cn][n * 4096 + n1 * 64 + (l * 8 + k)];
                return (v);
            }
            else
            {
                k = i >> 3;
                l = j >> 3;

                w = ctx.jpg_schs[cn];
                //	h=ctx.jpg_scvs[cn];
                n = (l * w) + k;

                k = i & 7;
                l = j & 7;

                v = ctx.jpg_sibuf[cn][n * 64 + (l * 8 + k)];
                return (v);
            }
        }
#endif

#if true
        public static int PDJPG_GetComponentPixelS1(PDJPG_Context ctx, int cn, int x, int y)
        {
            int i, j, k, l;
            int w, n, v;

            i = x >> 3;
            j = y >> 3;
            w = ctx.jpg_schs[cn];
            //	h=ctx.jpg_scvs[cn];
            n = (j * w) + i;

            k = x & 7;
            l = y & 7;

            v = ctx.jpg_sibuf[cn][n * 64 + (l * 8 + k)];
            return (v);
        }

        public static int PDJPG_GetComponentPixelS2(PDJPG_Context ctx, int cn, int x, int y)
        {
            int i, j, k, l;
            int w, n, v;

            i = x >> 1;
            j = y >> 1;

            k = i >> 3;
            l = j >> 3;
            w = ctx.jpg_schs[cn];
            //	h=ctx.jpg_scvs[cn];
            n = (l * w) + k;

            k = i & 7;
            l = j & 7;

            v = ctx.jpg_sibuf[cn][n * 64 + (l * 8 + k)];
            return (v);
        }

        public static int PDJPG_GetComponentPixelS122(PDJPG_Context ctx, int x, int y,
            out int ry, out int ru, out int rv)
        {
            int i, j, k, l;
            int t0, t1, w0, w1, n0, n1, x1, y1;

            x1 = x >> 1; y1 = y >> 1;

            w0 = ctx.jpg_schs[0];
            w1 = ctx.jpg_schs[1];

            i = x >> 3; j = y >> 3;
            k = x1 >> 3; l = y1 >> 3;
            n0 = (j * w0) + i;
            n1 = (l * w1) + k;

            i = x & 7; j = y & 7;
            k = x1 & 7; l = y1 & 7;
            t0 = (n0 << 6) + ((j << 3) + i);
            t1 = (n1 << 6) + ((l << 3) + k);

            ry = ctx.jpg_sibuf[0][t0];
            ru = ctx.jpg_sibuf[1][t1];
            rv = ctx.jpg_sibuf[2][t1];

            return (0);
        }

        public static int PDJPG_GetComponentPixelS111(PDJPG_Context ctx, int x, int y,
            out int ry, out int ru, out int rv)
        {
            int i, j;
            int t0, w0, n0;

            w0 = ctx.jpg_schs[0];
            i = x >> 3; j = y >> 3;
            n0 = (j * w0) + i;

            i = x & 7; j = y & 7;
            t0 = (n0 << 6) + ((j << 3) + i);

            ry = ctx.jpg_sibuf[0][t0];
            ru = ctx.jpg_sibuf[1][t0];
            rv = ctx.jpg_sibuf[2][t0];

            return (0);
        }

        public static int PDJPG_GetComponentPixelS420(PDJPG_Context ctx, int x, int y,
            out int ry0, out int ry1, out int ry2, out int ry3,
            out int ru, out int rv)
        {
            int i, j, k, l;
            int t0, t1, w0, w1, n0, n1, x1, y1;

            x1 = x >> 1; y1 = y >> 1;

            w0 = ctx.jpg_schs[0];
            w1 = ctx.jpg_schs[1];

            i = x >> 3; j = y >> 3;
            k = x1 >> 3; l = y1 >> 3;
            n0 = (j * w0) + i;
            n1 = (l * w1) + k;

            i = x & 7; j = y & 7;
            k = x1 & 7; l = y1 & 7;
            t0 = (n0 << 6) + ((j << 3) + i);
            t1 = (n1 << 6) + ((l << 3) + k);

            ry0 = ctx.jpg_sibuf[0][t0];
            ry1 = ctx.jpg_sibuf[0][t0 + 1];
            ry2 = ctx.jpg_sibuf[0][t0 + 8];
            ry3 = ctx.jpg_sibuf[0][t0 + 9];

            ru = ctx.jpg_sibuf[1][t1];
            rv = ctx.jpg_sibuf[2][t1];

            return (0);
        }

#endif

        public static void PDJPG_DecodeColorTransformYCbCr420(PDJPG_Context ctx, BytePtr obuf)
        {
            int u, v, a, u1, v1;
            int y0, y1, y2, y3;
            int r0, g0, b0;
            int r1, g1, b1;
            int r2, g2, b2;
            int r3, g3, b3;
            int l0, l1, l2;
            int i, j, k, l;

#if true
            for (i = 0; i < ctx.ys; i += 2)
                for (j = 0; j < ctx.xs; j += 2)
                {
                    //			PDJPG_GetComponentPixelS122(ctx, j, i, &y0, &u, &v);
                    //			y1=PDJPG_GetComponentPixelS1(ctx, 0, j+1, i+0);
                    //			y2=PDJPG_GetComponentPixelS1(ctx, 0, j+0, i+1);
                    //			y3=PDJPG_GetComponentPixelS1(ctx, 0, j+1, i+1);

                    PDJPG_GetComponentPixelS420(ctx, j, i,
                        out y0, out y1, out y2, out y3, out u, out v);

                    if (ctx.alphaClr != 0)
                    {
                        a = 255;

                        k = y0 - ctx.alphaClrY; l = k * k;
                        k = y1 - ctx.alphaClrY; k = k * k; if (k < l) l = k;
                        k = y2 - ctx.alphaClrY; k = k * k; if (k < l) l = k;
                        k = y3 - ctx.alphaClrY; k = k * k; if (k < l) l = k;

                        //				k=y0-ctx.alphaClrY; l=k*k;
                        k = u - ctx.alphaClrU; l += k * k;
                        k = v - ctx.alphaClrV; l += k * k;

                        a = ctx.alphaClrA;
                        a = (l <= (a * a)) ? 0 : 255;
                    }
                    else { a = 255; }

                    u1 = u - 128; v1 = v - 128;
                    l0 = 91881 * v1;
                    l1 = -22554 * u1 - 46802 * v1;
                    l2 = 116130 * u1;

                    y0 <<= 16; y1 <<= 16; y2 <<= 16; y3 <<= 16;
                    r0 = y0 + l0; g0 = y0 + l1; b0 = y0 + l2;
                    r1 = y1 + l0; g1 = y1 + l1; b1 = y1 + l2;
                    r2 = y2 + l0; g2 = y2 + l1; b2 = y2 + l2;
                    r3 = y3 + l0; g3 = y3 + l1; b3 = y3 + l2;

                    //			r0=65536*y0+l0; g0=65536*y0+l1; b0=65536*y0+l2;
                    //			r1=65536*y1+l0; g1=65536*y1+l1; b1=65536*y1+l2;
                    //			r2=65536*y2+l0; g2=65536*y2+l1; b2=65536*y2+l2;
                    //			r3=65536*y3+l0; g3=65536*y3+l1; b3=65536*y3+l2;

                    r0 >>= 16; g0 >>= 16; b0 >>= 16;
                    r1 >>= 16; g1 >>= 16; b1 >>= 16;
                    r2 >>= 16; g2 >>= 16; b2 >>= 16;
                    r3 >>= 16; g3 >>= 16; b3 >>= 16;

#if false
			if((r0|r1|r2|r3)&(~255))
			{
				r0=(r0<0)?0:((r0>255)?255:r0);
				r1=(r1<0)?0:((r1>255)?255:r1);
				r2=(r2<0)?0:((r2>255)?255:r2);
				r3=(r3<0)?0:((r3>255)?255:r3);
			}

			if((g0|g1|g2|g3)&(~255))
			{
				g0=(g0<0)?0:((g0>255)?255:g0);
				g1=(g1<0)?0:((g1>255)?255:g1);
				g2=(g2<0)?0:((g2>255)?255:g2);
				g3=(g3<0)?0:((g3>255)?255:g3);
			}

			if((b0|b1|b2|b3)&(~255))
			{
				b0=(b0<0)?0:((b0>255)?255:b0);
				b1=(b1<0)?0:((b1>255)?255:b1);
				b2=(b2<0)?0:((b2>255)?255:b2);
				b3=(b3<0)?0:((b3>255)?255:b3);
			}
#endif

#if false
			if((r0|r1|r2|r3)&(~255))
			{
				if(r0&(~255))r0=(r0<0)?0:((r0>255)?255:r0);
				if(r1&(~255))r1=(r1<0)?0:((r1>255)?255:r1);
				if(r2&(~255))r2=(r2<0)?0:((r2>255)?255:r2);
				if(r3&(~255))r3=(r3<0)?0:((r3>255)?255:r3);
			}

			if((g0|g1|g2|g3)&(~255))
			{
				if(g0&(~255))g0=(g0<0)?0:((g0>255)?255:g0);
				if(g1&(~255))g1=(g1<0)?0:((g1>255)?255:g1);
				if(g2&(~255))g2=(g2<0)?0:((g2>255)?255:g2);
				if(g3&(~255))g3=(g3<0)?0:((g3>255)?255:g3);
			}

			if((b0|b1|b2|b3)&(~255))
			{
				if(b0&(~255))b0=(b0<0)?0:((b0>255)?255:b0);
				if(b1&(~255))b1=(b1<0)?0:((b1>255)?255:b1);
				if(b2&(~255))b2=(b2<0)?0:((b2>255)?255:b2);
				if(b3&(~255))b3=(b3<0)?0:((b3>255)?255:b3);
			}
#endif

#if false
			k=r0|r1|r2|r3 | g0|g1|g2|g3 | b0|b1|b2|b3;
			if(k&(~255))
			{
				if(r0&(~255))r0=(r0<0)?0:((r0>255)?255:r0);
				if(g0&(~255))g0=(g0<0)?0:((g0>255)?255:g0);
				if(b0&(~255))b0=(b0<0)?0:((b0>255)?255:b0);
				if(r1&(~255))r1=(r1<0)?0:((r1>255)?255:r1);
				if(g1&(~255))g1=(g1<0)?0:((g1>255)?255:g1);
				if(b1&(~255))b1=(b1<0)?0:((b1>255)?255:b1);
				if(r2&(~255))r2=(r2<0)?0:((r2>255)?255:r2);
				if(g2&(~255))g2=(g2<0)?0:((g2>255)?255:g2);
				if(b2&(~255))b2=(b2<0)?0:((b2>255)?255:b2);
				if(r3&(~255))r3=(r3<0)?0:((r3>255)?255:r3);
				if(g3&(~255))g3=(g3<0)?0:((g3>255)?255:g3);
				if(b3&(~255))b3=(b3<0)?0:((b3>255)?255:b3);
			}
#endif

#if false
			r0=(!(r0&(~255)))?r0:((r0<0)?0:((r0>255)?255:r0));
			g0=(!(g0&(~255)))?g0:((g0<0)?0:((g0>255)?255:g0));
			b0=(!(b0&(~255)))?b0:((b0<0)?0:((b0>255)?255:b0));
			r1=(!(r1&(~255)))?r1:((r1<0)?0:((r1>255)?255:r1));
			g1=(!(g1&(~255)))?g1:((g1<0)?0:((g1>255)?255:g1));
			b1=(!(b1&(~255)))?b1:((b1<0)?0:((b1>255)?255:b1));
			r2=(!(r2&(~255)))?r2:((r2<0)?0:((r2>255)?255:r2));
			g2=(!(g2&(~255)))?g2:((g2<0)?0:((g2>255)?255:g2));
			b2=(!(b2&(~255)))?b2:((b2<0)?0:((b2>255)?255:b2));
			r3=(!(r3&(~255)))?r3:((r3<0)?0:((r3>255)?255:r3));
			g3=(!(g3&(~255)))?g3:((g3<0)?0:((g3>255)?255:g3));
			b3=(!(b3&(~255)))?b3:((b3<0)?0:((b3>255)?255:b3));
#endif

#if true
                    r0 = (r0 < 0) ? 0 : ((r0 > 255) ? 255 : r0);
                    g0 = (g0 < 0) ? 0 : ((g0 > 255) ? 255 : g0);
                    b0 = (b0 < 0) ? 0 : ((b0 > 255) ? 255 : b0);
                    r1 = (r1 < 0) ? 0 : ((r1 > 255) ? 255 : r1);
                    g1 = (g1 < 0) ? 0 : ((g1 > 255) ? 255 : g1);
                    b1 = (b1 < 0) ? 0 : ((b1 > 255) ? 255 : b1);
                    r2 = (r2 < 0) ? 0 : ((r2 > 255) ? 255 : r2);
                    g2 = (g2 < 0) ? 0 : ((g2 > 255) ? 255 : g2);
                    b2 = (b2 < 0) ? 0 : ((b2 > 255) ? 255 : b2);
                    r3 = (r3 < 0) ? 0 : ((r3 > 255) ? 255 : r3);
                    g3 = (g3 < 0) ? 0 : ((g3 > 255) ? 255 : g3);
                    b3 = (b3 < 0) ? 0 : ((b3 > 255) ? 255 : b3);
#endif

                    k = ctx.ys - 1 - i;
                    l0 = ((k * ctx.xs) + j + 0) * 4;
                    //			l1=((k*ctx.xs)+j+1)*4;
                    k = ctx.ys - 2 - i;
                    l2 = ((k * ctx.xs) + j + 0) * 4;
                    //			l3=((k*ctx.xs)+j+1)*4;

                    //			ct=obuf+l0; ct.emit(r0; ct.emit(g0; ct.emit(b0; ct.emit(a;
                    //			ct=obuf+l1; ct.emit(r1; ct.emit(g1; ct.emit(b1; ct.emit(a;
                    //			ct=obuf+l2; ct.emit(r2; ct.emit(g2; ct.emit(b2; ct.emit(a;
                    //			ct=obuf+l3; ct.emit(r3; ct.emit(g3; ct.emit(b3; ct.emit(a;

                    obuf[l0 + 0] = r0; obuf[l0 + 1] = g0; obuf[l0 + 2] = b0; obuf[l0 + 3] = a;
                    obuf[l0 + 4] = r1; obuf[l0 + 5] = g1; obuf[l0 + 6] = b1; obuf[l0 + 7] = a;
                    obuf[l2 + 0] = r2; obuf[l2 + 1] = g2; obuf[l2 + 2] = b2; obuf[l2 + 3] = a;
                    obuf[l2 + 4] = r3; obuf[l2 + 5] = g3; obuf[l2 + 6] = b3; obuf[l2 + 7] = a;

                    //			obuf[l0+0]=r0; obuf[l0+1]=g0; obuf[l0+2]=b0; obuf[l0+3]=a;
                    //			obuf[l1+0]=r1; obuf[l1+1]=g1; obuf[l1+2]=b1; obuf[l1+3]=a;
                    //			obuf[l2+0]=r2; obuf[l2+1]=g2; obuf[l2+2]=b2; obuf[l2+3]=a;
                    //			obuf[l3+0]=r3; obuf[l3+1]=g3; obuf[l3+2]=b3; obuf[l3+3]=a;
                }
#endif

#if false
		for(i=0; i<ctx.ys; i++)
			for(j=0; j<ctx.xs; j++)
		{
			PDJPG_GetComponentPixelS122(ctx, j, i, &y, &u, &v);

			if(ctx.alphaClr)
			{
				k=y-ctx.alphaClrY; l=k*k;
				k=u-ctx.alphaClrU; l+=k*k;
				k=v-ctx.alphaClrV; l+=k*k;
				
				a=ctx.alphaClrA;
				a=(l<=(a*a))?0:255;
			}else { a=255; }

			u1=u-128; v1=v-128;
			r=65536*y          +91881*v1;
			g=65536*y- 22554*u1-46802*v1;
			b=65536*y+116130*u1;
			r>>=16; g>>=16; b>>=16;

			r=(r<0)?0:((r>255)?255:r);
			g=(g<0)?0:((g>255)?255:g);
			b=(b<0)?0:((b>255)?255:b);

			k=ctx.ys-1-i;
			l=((k*ctx.xs)+j)*4;

			obuf[l+0]=r;
			obuf[l+1]=g;
			obuf[l+2]=b;
			obuf[l+3]=a;
		}
#endif
        }

        /** Simulate basic decoding, but doesn't actually decode image.
          * Can be used to retrieve resolution or initialize Huffman tables
          * at a lower cost than a full decode. */
        public static int PDJPG_DummyDecodeCtx(PDJPG_Context ctx,
            BytePtr buf, int sz, out int xs, out int ys)
        {
            //	int r, g, b, u1;
            //	int l2;
            int i, j;

            if ((buf[0] != 0xFF) || (buf[1] != JPG.JPG_SOI))
            {
                xs = 0; ys = 0;
                return (-1);
            }

            ctx.jpg_is420 = false;
            ctx.jpg_rdct = false;
            ctx.jpg_clrtrans = 0;

            ctx.bcst_version = 0;
            ctx.bcst_flags = 0;
            ctx.bcst_trans = 0;
            ctx.bcst_bpp = 0;

            for (i = 0; i < 4; i++)
            {
                ctx.jpg_scbuf[i] = new ShortPtr(null);
                ctx.jpg_sibuf[i] = new ShortPtr(null);
            }

            for (i = 0; i < sz; i++)
            {
                //		if((buf[i]==0xFF) && (buf[i+1]!=0x00) && (buf[i+1]!=0xFF))
                if (buf[i] == 0xFF)
                {
                    if (buf[i + 1] == 0x00)
                    { i++; continue; }
                    if (buf[i + 1] == 0xFF)
                    { i++; continue; }

                    j = buf[i + 1] - 0xC0;
                    if (j < 0) j = 63;

                    //			printf("@ %d %s(%02X)\n", i,
                    //				pdjpg_marker[j], buf[i+1]);

                    if (buf[i + 1] == JPG.JPG_DQT) PDJPG_MarkerDQT(ctx, buf + i + 2);
                    if (buf[i + 1] == JPG.JPG_DHT) PDJPG_MarkerDHT(ctx, buf + i + 2);
                    if (buf[i + 1] == JPG.JPG_SOF0) PDJPG_MarkerSOF0(ctx, buf + i + 2);
                    if (buf[i + 1] == JPG.JPG_SOS)
                    {
                        //				j=PDJPG_MarkerSOS(ctx, buf+i+2);
                        //				if(j>0) { i+=j; }
                        continue;
                    }

                    if ((buf[i + 1] >= JPG.JPG_APP0) && (buf[i + 1] <= JPG.JPG_APP15))
                    {
                        if (buf[i + 1] == JPG.JPG_APP0) PDJPG_MarkerAPP0(ctx, buf + i + 2);
                        if (buf[i + 1] == JPG.JPG_APP9) PDJPG_MarkerAPP9(ctx, buf + i + 2);

                        if (buf[i + 1] == JPG.JPG_APP11) PDJPG_MarkerAPP11(ctx, buf + i + 2);
                        if (buf[i + 1] == JPG.JPG_APP12) PDJPG_MarkerAPP12(ctx, buf + i + 2);
                        if (buf[i + 1] == JPG.JPG_APP14) PDJPG_MarkerAPP14(ctx, buf + i + 2);

                        //skip over marker
                        j = (buf[i + 2] << 8) | buf[i + 3];
                        i += j;
                        continue;
                    }

                    if ((buf[i + 1] >= JPG.JPG_JPG0) && (buf[i + 1] <= JPG.JPG_JPG13))
                    {
                        if (buf[i + 1] == JPG.JPG_JPG11)
                        {
                            j = PDJPG_MarkerAPP11(ctx, buf + i + 2);
                            if (j < 1) { xs = 0; ys = 0; return (-1); }
                        }else if (buf[i + 1] == JPG.JPG_JPG12)
                        {
                            j = PDJPG_MarkerAPP12(ctx, buf + i + 2);
                            if (j < 1) { xs = 0; ys = 0; return (-1); }
                        }else
                        {
                            //unknown marker
                            xs = 0; ys = 0;
                            return (-1);
                        }

                        //skip over marker
                        j = (buf[i + 2] << 8) | buf[i + 3];
                        i += j;
                        continue;
                    }

                    if (buf[i + 1] == JPG.JPG_EOI) break;
                }
            }

            xs = ctx.xs;
            ys = ctx.ys;

            return (0);
        }

        public static int PDJPG_DummyDecode(BytePtr buf, int sz, out int xs, out int ys)
        {
            PDJPG_Context ctx;

            ctx = PDJPG_AllocContext();
            PDJPG_DummyDecodeCtx(ctx, buf, sz, out xs, out ys);
            //	ctx.jpg_imgbuf=null;
            return 0;
        }

        public static BytePtr PDJPG_DecodeCtx(PDJPG_Context ctx,
            BytePtr buf, int sz, out int xs, out int ys)
        {
            BytePtr obuf;
            BytePtr ct;
            int y, u, v, r, g, b, a, u1, v1;
            int i, j, k, l;

#if false
            if ((buf[0] != 0xFF) || (buf[1] != JPG.JPG_SOI))
            {
                xs = 0; ys = 0;
                return new BytePtr(null);
            }
#endif

            ctx.jpg_is420 = false;
            ctx.jpg_rdct = false;
            ctx.megablock = false;
            ctx.altvlc = false;
            ctx.auto_dct = false;

            ctx.jpg_clrtrans = 0;

            ctx.bcst_version = 0;
            ctx.bcst_flags = 0;
            ctx.bcst_trans = 0;
            ctx.bcst_bpp = 0;

            for (i = 0; i < 4; i++)
            {
                ctx.jpg_scbuf[i] = new ShortPtr(null);
                ctx.jpg_sibuf[i] = new ShortPtr(null);
            }

            for (i = 0; i < sz; i++)
            {
                //		if((buf[i]==0xFF) && (buf[i+1]!=0x00) && (buf[i+1]!=0xFF))
                if (buf[i] == 0xFF)
                {
                    if (buf[i + 1] == 0x00)
                    { i++; continue; }
                    if (buf[i + 1] == 0xFF)
                    { i++; continue; }

                    j = buf[i + 1] - 0xC0;
                    if (j < 0) j = 63;

                    //			printf("@ %d %s(%02X)\n", i,
                    //				pdjpg_marker[j], buf[i+1]);

                    if (buf[i + 1] == JPG.JPG_DQT) PDJPG_MarkerDQT(ctx, buf + i + 2);
                    if (buf[i + 1] == JPG.JPG_DHT) PDJPG_MarkerDHT(ctx, buf + i + 2);
                    if (buf[i + 1] == JPG.JPG_SOF0) PDJPG_MarkerSOF0(ctx, buf + i + 2);
                    if (buf[i + 1] == JPG.JPG_SOS)
                    {
                        j = PDJPG_MarkerSOS(ctx, buf + i + 2);
                        if (j > 0) { i += j; }
                        continue;
                    }

                    if ((buf[i + 1] >= JPG.JPG_APP0) && (buf[i + 1] <= JPG.JPG_APP15))
                    {
                        if (buf[i + 1] == JPG.JPG_APP0) PDJPG_MarkerAPP0(ctx, buf + i + 2);
                        if (buf[i + 1] == JPG.JPG_APP11) PDJPG_MarkerAPP11(ctx, buf + i + 2);
                        if (buf[i + 1] == JPG.JPG_APP12) PDJPG_MarkerAPP12(ctx, buf + i + 2);
                        if (buf[i + 1] == JPG.JPG_APP14) PDJPG_MarkerAPP14(ctx, buf + i + 2);

                        //skip over marker
                        j = (buf[i + 2] << 8) | buf[i + 3];
                        i += j;
                        continue;
                    }

                    if ((buf[i + 1] >= JPG.JPG_JPG0) && (buf[i + 1] <= JPG.JPG_JPG13))
                    {
                        if (buf[i + 1] == JPG.JPG_JPG11)
                        {
                            j = PDJPG_MarkerAPP11(ctx, buf + i + 2);
                            if (j < 1) { xs = 0; ys = 0; return (new BytePtr(null)); }
                        }
                        else if (buf[i + 1] == JPG.JPG_JPG12)
                        {
                            j = PDJPG_MarkerAPP12(ctx, buf + i + 2);
                            if (j < 1) { xs = 0; ys = 0; return (new BytePtr(null)); }
                        }
                        else
                        {
                            //unknown marker
                            xs = 0; ys = 0;
                            return (new BytePtr(null));
                        }

                        //skip over marker
                        j = (buf[i + 2] << 8) | buf[i + 3];
                        i += j;
                        continue;
                    }

                    if (buf[i + 1] == JPG.JPG_EOI) break;
                }
            }

            obuf = ctx.jpg_imgbuf;
            if ((obuf.isNull) || (ctx.xs != ctx.lxs) || (ctx.ys != ctx.lys))
            {
                //		if(ctx.jpg_imgbuf)
                //			free(ctx.jpg_imgbuf);

                obuf = new BytePtr((ctx.xs + 1) * (ctx.ys + 1) * 4);
                ((BytePtr)obuf).memset(0xFF, ctx.xs * ctx.ys * 4);

                ctx.jpg_imgbuf = (BytePtr)obuf;
                ctx.lxs = ctx.xs;
                ctx.lys = ctx.ys;
            }

            for (i = 0; i < ctx.jpg_nc; i++)
            {
                j = ctx.jpg_scn[i];
                //		ctx.jpg_schsc[i]=ctx.jpg_ch[j]/(ctx.jpg_chm*1.0);
                //		ctx.jpg_scvsc[i]=ctx.jpg_cv[j]/(ctx.jpg_chn*1.0);

                ctx.jpg_schsci[i] = (65536 * ctx.jpg_ch[j]) / ctx.jpg_chm;
                ctx.jpg_scvsci[i] = (65536 * ctx.jpg_cv[j]) / ctx.jpg_chn;

                if (ctx.megablock)
                {
                    ctx.jpg_schs[i] = (ctx.jpg_cxi[j] + 63) / 64;
                    ctx.jpg_scvs[i] = (ctx.jpg_cyi[j] + 63) / 64;
                }
                else
                {
                    ctx.jpg_schs[i] = (ctx.jpg_cxi[j] + 7) / 8;
                    ctx.jpg_scvs[i] = (ctx.jpg_cyi[j] + 7) / 8;
                }

            }

            if ((ctx.jpg_nc == 3) && !ctx.megablock)
            {
                i = 1;
                if (ctx.jpg_schsci[0] != 65536) i = 0;
                if (ctx.jpg_scvsci[0] != 65536) i = 0;
                if (ctx.jpg_schsci[1] != 32768) i = 0;
                if (ctx.jpg_scvsci[1] != 32768) i = 0;
                if (ctx.jpg_schsci[2] != 32768) i = 0;
                if (ctx.jpg_scvsci[2] != 32768) i = 0;
                ctx.jpg_is420 = (i != 0);

                i = 1;
                if (ctx.jpg_schsci[0] != 65536) i = 0;
                if (ctx.jpg_scvsci[0] != 65536) i = 0;
                if (ctx.jpg_schsci[1] != 65536) i = 0;
                if (ctx.jpg_scvsci[1] != 65536) i = 0;
                if (ctx.jpg_schsci[2] != 65536) i = 0;
                if (ctx.jpg_scvsci[2] != 65536) i = 0;
                ctx.jpg_is444 = (i != 0);
            }
            else
            {
                ctx.jpg_is420 = false;
                ctx.jpg_is444 = false;
            }

            if (ctx.jpg_is420 && (((ctx.xs | ctx.ys) & 1) == 0) &&
                (ctx.jpg_clrtrans == PDJPG_CLRS_YCBCR))
            {
                PDJPG_DecodeColorTransformYCbCr420(ctx, (BytePtr)obuf);
            }
            else
            {
                for (i = 0; i < ctx.ys; i++)
                    for (j = 0; j < ctx.xs; j++)
                    {
                        if (ctx.jpg_nc == 3)
                        {
                            if (ctx.jpg_is420)
                            {
                                //					y=PDJPG_GetComponentPixelS1(ctx, 0, j, i);
                                //					u=PDJPG_GetComponentPixelS2(ctx, 1, j, i);
                                //					v=PDJPG_GetComponentPixelS2(ctx, 2, j, i);
                                PDJPG_GetComponentPixelS122(ctx, j, i, out y, out u, out v);
                                a = 255;
                            }
                            else if (ctx.jpg_is444)
                            {
                                PDJPG_GetComponentPixelS111(ctx, j, i, out y, out u, out v);
                                a = 255;
                            }
                            else
                            {
                                y = PDJPG_GetComponentPixel(ctx, 0, j, i);
                                u = PDJPG_GetComponentPixel(ctx, 1, j, i);
                                v = PDJPG_GetComponentPixel(ctx, 2, j, i);
                                a = 255;
                            }

                            if (ctx.alphaClr != 0)
                            {
                                k = y - ctx.alphaClrY; l = k * k;
                                k = u - ctx.alphaClrU; l += k * k;
                                k = v - ctx.alphaClrV; l += k * k;

                                a = ctx.alphaClrA;
                                a = (l <= (a * a)) ? 0 : 255;

                                //					if(l<(16*16))
                                //					{
                                //						a=255*((16*sqrt(l))/ctx.alphaClrA);
                                //						a=(a<0)?0:(a>255)?255:a;
                                //					}
                            }

                            //				r=y+1.40200*(v-128);
                            //				g=y-0.34414*(u-128)-0.71414*(v-128);
                            //				b=y+1.77200*(u-128);

                            if (ctx.jpg_clrtrans == PDJPG_CLRS_YCBCR)
                            {
                                u1 = u - 128; v1 = v - 128;
                                r = 65536 * y + 91881 * v1;
                                g = 65536 * y - 22554 * u1 - 46802 * v1;
                                b = 65536 * y + 116130 * u1;
                                r >>= 16; g >>= 16; b >>= 16;
                            }
                            else if (ctx.jpg_clrtrans == PDJPG_CLRS_ORCT)
                            {
                                u1 = u - 128; v1 = v - 128;
                                g = y - (u1 + v1) / 4;
                                r = v1 + g;
                                b = u1 + g;
                            }
                            else if (ctx.jpg_clrtrans == PDJPG_CLRS_RGB)
                            {
                                r = y;
                                g = u;
                                b = v;
                            }
                            else
                            {
                                r = 0; g = 0; b = 0;
                            }

                            r = (r < 0) ? 0 : ((r > 255) ? 255 : r);
                            g = (g < 0) ? 0 : ((g > 255) ? 255 : g);
                            b = (b < 0) ? 0 : ((b > 255) ? 255 : b);

                            //				r=(r<0)?0:((r>16777215)?255:(r>>16));
                            //				g=(g<0)?0:((g>16777215)?255:(g>>16));
                            //				b=(b<0)?0:((b>16777215)?255:(b>>16));
                        }
                        else if (ctx.jpg_nc == 1)
                        {
                            y = PDJPG_GetComponentPixel(ctx, 0, j, i);
                            y = (y < 0) ? 0 : ((y > 255) ? 255 : y);
                            r = y; g = y; b = y; a = 255;
                        }
                        else
                        {
                            r = 0; g = 0; b = 0; a = 0;
                        }

                        k = ctx.ys - 1 - i;
                        l = ((k * ctx.xs) + j) * 4;

                        ct = (BytePtr)obuf;
                        ct[l + 0] = r;
                        ct[l + 1] = g;
                        ct[l + 2] = b;
                        ct[l + 3] = a;
                    }
            }

            for (i = 0; i < 4; i++)
            {
                //		if(ctx.jpg_scbuf[i])free(ctx.jpg_scbuf[i]);
                //		if(ctx.jpg_sibuf[i])free(ctx.jpg_sibuf[i]);
                ctx.jpg_scbuf[i] = new ShortPtr(null);
                ctx.jpg_sibuf[i] = new ShortPtr(null);
            }

            xs = ctx.xs;
            ys = ctx.ys;
            return (obuf);
        }

        public static void PDJPG_Free(BytePtr buf)
        {
            //	if(!buf)return;
            //	free(buf);
        }

        public static BytePtr PDJPG_DecodeScanForComponentLayer(BytePtr buf, int sz, string name)
        {
            BytePtr cs, cs2, cse;
            int i;

            Console.WriteLine("PDJPG_DecodeScanForComponentLayer: Sz={0} Name={1}", sz, name);

            cs = buf; cse = buf + sz;
            while (cs < cse)
            {
//                if (cs[0] == 0xFF)
//                    Console.WriteLine("FF,{0}", cs[1]);

                if ((cs[0] == 0xFF) && (cs[1] == JPG.JPG_APP11))
                {
                    i = (cs[2] << 8) | cs[3];
                    cs2 = cs + i + 2;

                    cs += 4;

                    Console.Write("APP11 {0}\n", cs.getString());

                    if (!cs.strcmp("TagLayer"))
                    {
                        //This does not cross tag-layer boundaries
                        break;
                    }

                    if (!cs.strcmp("CompLayer"))
                    {
                        cs += cs.strlen() + 1;

                        Console.Write("APP11 CompLayer={0}\n", cs.getString());

                        if (!cs.strcmp(name))
                        {
                            Console.Write("Found {0}\n", name);
                            cs += cs.strlen() + 1;
                            return (cs2);
                        }

                        cs += cs.strlen() + 1;
                        continue;
                    }
                    cs += i - 2;
                    continue;
                }
                if ((cs[0] == 0xFF) && (cs[1] >= JPG.JPG_APP0) && (cs[1] <= JPG.JPG_APP15))
                {
                    i = (cs[2] << 8) | cs[3];
                    cs += i + 2;
                    continue;
                }

                cs=cs+1;
            }

            Console.Write("Not Found {0}\n", name);
            return new BytePtr(null);
        }


        public static BytePtr PDJPG_DecodeScanNextTagLayer(BytePtr buf, int sz, out string name)
        {
            BytePtr cs, cs2, cse;
            int i;

//            Console.WriteLine("PDJPG_DecodeScanNextTagLayer: Sz={0} Name={1}", sz, name);

            cs = buf; cse = buf + sz;
            while (cs < cse)
            {
                //                if (cs[0] == 0xFF)
                //                    Console.WriteLine("FF,{0}", cs[1]);

                if ((cs[0] == 0xFF) && (cs[1] == JPG.JPG_APP11))
                {
                    i = (cs[2] << 8) | cs[3];
                    cs2 = cs + i + 2;

                    cs += 4;

                    Console.Write("APP11 {0}\n", cs.getString());

                    if (!cs.strcmp("TagLayer"))
                    {
                        cs += cs.strlen() + 1;

                        name = cs.getString();
                        Console.Write("APP11 TagLayer={0}\n", name);

                        cs += cs.strlen() + 1;
                        return (cs2);
                    }
                    cs += i - 2;
                    continue;
                }
                if ((cs[0] == 0xFF) && (cs[1] >= JPG.JPG_APP0) && (cs[1] <= JPG.JPG_APP15))
                {
                    i = (cs[2] << 8) | cs[3];
                    cs += i + 2;
                    continue;
                }

                cs = cs + 1;
            }

//            Console.Write("Not Found {0}\n", name);
            name = null;
            return new BytePtr(null);
        }

        public static BytePtr PDJPG_Decode(BytePtr buf, int sz, out int xs, out int ys)
        {
            PDJPG_Context ctx;
            BytePtr obuf;
            BytePtr otbuf, otbuf2;
            BytePtr csl;
            int i, n, sz1;

            ctx = PDJPG_AllocContext();
            obuf = PDJPG_DecodeCtx(ctx, buf, sz, out xs, out ys);
            //	ctx.jpg_imgbuf=null;

            csl = PDJPG_DecodeScanForComponentLayer(buf, sz, "Alpha");
            if (!csl.isNull)
            {
                sz1 = sz - ((BytePtr)csl - buf);
                otbuf = (BytePtr)PDJPG_DecodeLDatCtx(ctx, (BytePtr)csl, sz1, out xs, out ys);

                otbuf2 = (BytePtr)obuf;
                n = ctx.xs * ctx.ys;
                for (i = 0; i < n; i++)
                {
                    otbuf2[i * 4 + 3] = otbuf[i * 4 + 1];
                }
            }

            PDJPG_FreeContext(ctx);
            return (obuf);
        }

        public static int PDJPG_DecodeComponentType(BytePtr buf, int sz)
        {
            BytePtr csl;
            //	int sz1;

            //	sz1=(sz>1024)?1024:sz;
            //	csl=PDJPG_DecodeScanForComponentLayer(buf, sz1, "RGB");
            //	if(!csl)return(0);

            csl = PDJPG_DecodeScanForComponentLayer(buf, sz, "DASe");
            if (!csl.isNull)
            {
                csl = PDJPG_DecodeScanForComponentLayer(buf, sz, "LuRGB");
                if (!csl.isNull) return (16);
                csl = PDJPG_DecodeScanForComponentLayer(buf, sz, "SpRGB");
                if (!csl.isNull) return (12);
                return (8);
            }
            else
            {
                csl = PDJPG_DecodeScanForComponentLayer(buf, sz, "LuRGB");
                if (!csl.isNull) return (7);

                csl = PDJPG_DecodeScanForComponentLayer(buf, sz, "Alpha");
                if (!csl.isNull) return (4);

                return (3);
            }
        }

        public static bool PDJPG_DecodeHasComponent(BytePtr buf, int sz, string name)
        {
            BytePtr csl;
            csl = PDJPG_DecodeScanForComponentLayer(buf, sz, name);
            if (!csl.isNull) return true;
            return false;
        }

        public static BytePtr PDJPG_DecodeLDatCtx(PDJPG_Context ctx,
            BytePtr buf, int sz, out int xs, out int ys)
        {
            BytePtr tbuf, tbuf2;
            BytePtr cs, ct;
            int i;

            if ((buf[0] != 0xFF) || (buf[1] != JPG.JPG_APP12) ||
                (buf[4] != 'L') || (buf[5] != 'D') ||
                (buf[6] != 'A') || (buf[7] != 'T'))
            {
                tbuf = PDJPG_DecodeCtx(ctx, buf, sz, out xs, out ys);
                return (tbuf);
            }

            tbuf = new BytePtr(1 << 20);
            cs = buf; ct = (BytePtr)tbuf;

            while ((cs[0] == 0xFF) && (cs[1] == JPG.JPG_APP12) &&
                (cs[4] == 'L') && (cs[5] == 'D') &&
                (cs[6] == 'A') && (cs[7] == 'T'))
            {
                i = (cs[2] << 8) | cs[3];
                BytePtr.memcpy(ct, cs + 8, i - 6);
                cs += i + 2; ct += (i - 6);
            }

            i = ct - (BytePtr)tbuf;
            i = PDJPG_EscapeDecodeSingleBuffer((BytePtr)tbuf, i);

            tbuf2 = PDJPG_DecodeCtx(ctx, (BytePtr)tbuf, i, out xs, out ys);
            //	free(tbuf);
            return (tbuf2);
        }

        public static int PDJPG_DecodeComponentCtx(PDJPG_Context ctx,
            BytePtr buf, int sz,
            byte[] rgba, byte[] norm, byte[] spec, byte[] luma,
            out int xs, out int ys)
        {
            return (PDJPG_DecodeComponentCtxI(ctx, buf, sz,
                rgba, norm, spec, luma, null,
                out xs, out ys));
        }

        public static int PDJPG_DecodeFloatComponentCtx(PDJPG_Context ctx,
            BytePtr buf, int sz,
            float[] rgba, float[] norm,
            float[] spec, float[] luma,
            out int xs, out int ys)
        {
            BytePtr trgba, tnorm, tspec, tluma, tlebe;
            int i, n;
            double f, g;

            i = PDJPG_DummyDecodeCtx(ctx, buf, sz, out xs, out ys);
            if (i < 0) return (-1);

            n = ctx.xs * ctx.ys;
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

            i = PDJPG_DecodeComponentCtxI(ctx, buf, sz,
                ctx.imgbuf_rgba, ctx.imgbuf_norm,
                ctx.imgbuf_spec, ctx.imgbuf_luma,
                ctx.imgbuf_lebe,
                out xs, out ys);
            if (i < 0)
            {
                return (-1);
            }

            trgba = new BytePtr(ctx.imgbuf_rgba);
            tnorm = new BytePtr(ctx.imgbuf_norm);
            tspec = new BytePtr(ctx.imgbuf_spec);
            tluma = new BytePtr(ctx.imgbuf_luma);
            tlebe = new BytePtr(ctx.imgbuf_lebe);

            if (rgba != null)
            {
                for (i = 0; i < n; i++)
                {
                    f = (1.0 / 256) * pdjpg_scltab[tlebe[i * 4 + 0]];
                    rgba[i * 4 + 0] = (float)(trgba[i * 4 + 0] * f);
                    rgba[i * 4 + 1] = (float)(trgba[i * 4 + 1] * f);
                    rgba[i * 4 + 2] = (float)(trgba[i * 4 + 2] * f);
                    rgba[i * 4 + 3] = (float)(trgba[i * 4 + 3] * (1.0 / 256));
                }
            }

            if (norm != null)
            {
                for (i = 0; i < n; i++)
                {
                    //			f=(1.0/256)*pdjpg_scltab[tlebe[i*4+0]];
                    f = 1.0 / 256;
                    g = (1.0 / 256) * pdjpg_scltab[tlebe[i * 4 + 2]];
                    norm[i * 4 + 0] = (float)(tnorm[i * 4 + 0] * f);
                    norm[i * 4 + 1] = (float)(tnorm[i * 4 + 1] * f);
                    norm[i * 4 + 2] = (float)(tnorm[i * 4 + 2] * f);
                    norm[i * 4 + 3] = (float)(tnorm[i * 4 + 3] * g);
                }
            }

            if (spec != null)
            {
                for (i = 0; i < n; i++)
                {
                    //			f=(1.0/256)*pdjpg_scltab[tlebe[i*4+0]];
                    f = 1.0 / 256;
                    spec[i * 4 + 0] = (float)(tspec[i * 4 + 0] * f);
                    spec[i * 4 + 1] = (float)(tspec[i * 4 + 1] * f);
                    spec[i * 4 + 2] = (float)(tspec[i * 4 + 2] * f);
                    spec[i * 4 + 3] = (float)(tspec[i * 4 + 3] * f);
                }
            }

            if (luma != null)
            {
                for (i = 0; i < n; i++)
                {
                    f = (1.0 / 256) * pdjpg_scltab[tlebe[i * 4 + 1]];
                    luma[i * 4 + 0] = (float)(tluma[i * 4 + 0] * f);
                    luma[i * 4 + 1] = (float)(tluma[i * 4 + 1] * f);
                    luma[i * 4 + 2] = (float)(tluma[i * 4 + 2] * f);
                    luma[i * 4 + 3] = (float)(tluma[i * 4 + 3] * (1.0 / 256));
                }
            }
            return (0);
        }

        public static void PDJPG_ExtractComponentCtx(PDJPG_Context ctx,
            byte[] rgba, byte[] norm, byte[] spec, byte[] luma, byte[] lebe)
        {
            int n;

            n = ctx.xs * ctx.ys;
            if (rgba != null && ctx.imgbuf_rgba != null)
                BytePtr.memcpy(rgba, ctx.imgbuf_rgba, n * 4);
            if (norm != null && ctx.imgbuf_norm != null)
                BytePtr.memcpy(norm, ctx.imgbuf_norm, n * 4);
            if (spec != null && ctx.imgbuf_spec != null)
                BytePtr.memcpy(spec, ctx.imgbuf_spec, n * 4);
            if (luma != null && ctx.imgbuf_luma != null)
                BytePtr.memcpy(luma, ctx.imgbuf_luma, n * 4);
            if (lebe != null && ctx.imgbuf_lebe != null)
                BytePtr.memcpy(lebe, ctx.imgbuf_lebe, n * 4);
        }

        public static int PDJPG_DecodeComponentCtxI(PDJPG_Context ctx,
            BytePtr buf, int sz,
            byte[] rgba, byte[] norm, byte[] spec, byte[] luma, byte[] lebe,
            out int xs, out int ys)
        {
            BytePtr tbuf;
            BytePtr csl;
            int i, n, sz1;

            csl = PDJPG_DecodeScanForComponentLayer(buf, sz, "RGB");
            if (!csl.isNull)
//            if(false)
            {
                sz1 = sz - (csl - buf);
                tbuf = PDJPG_DecodeLDatCtx(ctx, csl, sz1, out xs, out ys);
                if (tbuf.isNull) return (-1);
            }
            else
            {
                tbuf = PDJPG_DecodeCtx(ctx, buf, sz, out xs, out ys);
                if (tbuf.isNull) return (-1);
            }

            n = ctx.xs * ctx.ys;

            for (i = 0; i < n; i++)
            {
                rgba[i * 4 + 0] = (byte)tbuf[i * 4 + 0];
                rgba[i * 4 + 1] = (byte)tbuf[i * 4 + 1];
                rgba[i * 4 + 2] = (byte)tbuf[i * 4 + 2];
                rgba[i * 4 + 3] = (byte)255;
                //		rgba[i*4+3]=(byte)tbuf[i*4+3];
            }

            if (norm != null)
            {
                csl = PDJPG_DecodeScanForComponentLayer(buf, sz, "XYZ");
                if (!csl.isNull)
                {
                    sz1 = sz - ((BytePtr)csl - buf);
                    tbuf = (BytePtr)PDJPG_DecodeLDatCtx(ctx, (BytePtr)csl, sz1, out xs, out ys);
                    for (i = 0; i < n; i++)
                    {
                        norm[i * 4 + 0] = (byte)tbuf[i * 4 + 0];
                        norm[i * 4 + 1] = (byte)tbuf[i * 4 + 2];
                        norm[i * 4 + 2] = (byte)tbuf[i * 4 + 1];
                        //				norm[i*4+3]=(byte)tbuf[i*4+3];
                    }
                }
            }

            if (spec != null)
            {
                csl = PDJPG_DecodeScanForComponentLayer(buf, sz, "SpRGB");
                if (!csl.isNull)
                {
                    sz1 = sz - ((BytePtr)csl - buf);
                    tbuf = (BytePtr)PDJPG_DecodeLDatCtx(ctx, (BytePtr)csl, sz1, out xs, out ys);
                    for (i = 0; i < n; i++)
                    {
                        spec[i * 4 + 0] = (byte)tbuf[i * 4 + 0];
                        spec[i * 4 + 1] = (byte)tbuf[i * 4 + 1];
                        spec[i * 4 + 2] = (byte)tbuf[i * 4 + 2];
                        //				spec[i*4+3]=(byte)tbuf[i*4+3];
                    }
                }
            }

            csl = PDJPG_DecodeScanForComponentLayer(buf, sz, "DASe");
            if (!csl.isNull)
            {
                sz1 = sz - ((BytePtr)csl - buf);
                tbuf = (BytePtr)PDJPG_DecodeLDatCtx(ctx, (BytePtr)csl, sz1, out xs, out ys);
                for (i = 0; i < n; i++)
                {
                    rgba[i * 4 + 3] = (byte)tbuf[i * 4 + 1];
                    if (norm != null) { norm[i * 4 + 3] = (byte)tbuf[i * 4 + 0]; }
                    if (spec != null) { spec[i * 4 + 3] = (byte)tbuf[i * 4 + 2]; }
                }
            }
            else
            {
                csl = PDJPG_DecodeScanForComponentLayer(buf, sz, "Alpha");
                if (!csl.isNull)
                {
                    sz1 = sz - ((BytePtr)csl - buf);
                    tbuf = (BytePtr)PDJPG_DecodeLDatCtx(ctx, (BytePtr)csl, sz1, out xs, out ys);
                    for (i = 0; i < n; i++)
                    { rgba[i * 4 + 3] = (byte)tbuf[i * 4 + 1]; }
                }

                if (norm != null)
                {
                    csl = PDJPG_DecodeScanForComponentLayer(buf, sz, "Depth");
                    if (!csl.isNull)
                    {
                        sz1 = sz - ((BytePtr)csl - buf);
                        tbuf = (BytePtr)PDJPG_DecodeLDatCtx(ctx, (BytePtr)csl, sz1, out xs, out ys);
                        for (i = 0; i < n; i++)
                        { norm[i * 4 + 3] = (byte)tbuf[i * 4 + 1]; }
                    }
                }
            }

            if (luma != null)
            {
                csl = PDJPG_DecodeScanForComponentLayer(buf, sz, "LuRGB");
                if (!csl.isNull)
                {
                    sz1 = sz - ((BytePtr)csl - buf);
                    tbuf = (BytePtr)PDJPG_DecodeLDatCtx(ctx, (BytePtr)csl, sz1, out xs, out ys);
                    for (i = 0; i < n; i++)
                    {
                        luma[i * 4 + 0] = (byte)tbuf[i * 4 + 0];
                        luma[i * 4 + 1] = (byte)tbuf[i * 4 + 1];
                        luma[i * 4 + 2] = (byte)tbuf[i * 4 + 2];
                        luma[i * 4 + 3] = 255;
                    }
                }
            }

            if (lebe != null)
            {
                csl = PDJPG_DecodeScanForComponentLayer(buf, sz, "LeBe");
                if (!csl.isNull)
                {
                    sz1 = sz - ((BytePtr)csl - buf);
                    tbuf = (BytePtr)PDJPG_DecodeLDatCtx(ctx, (BytePtr)csl, sz1, out xs, out ys);
                    for (i = 0; i < n; i++)
                    {
                        lebe[i * 4 + 0] = (byte)tbuf[i * 4 + 0];
                        lebe[i * 4 + 1] = (byte)tbuf[i * 4 + 1];
                        lebe[i * 4 + 2] = (byte)tbuf[i * 4 + 2];
                    }
                }
            }

            return (0);
        }

        public static int PDJPG_DecodeComponent(BytePtr buf, int sz,
            byte[] rgba, byte[] norm, byte[] spec, byte[] luma,
            out int xs, out int ys)
        {
            PDJPG_Context ctx;
            int i;

            ctx = PDJPG_AllocContext();
            i = PDJPG_DecodeComponentCtx(ctx, buf, sz,
                rgba, norm, spec, luma, out xs, out ys);
            //	ctx.jpg_imgbuf=null;
            PDJPG_FreeContext(ctx);
            return (i);
        }

        public static int PDJPG_DecodeComponent(BytePtr buf, int sz,
            byte[] rgba, byte[] norm, byte[] spec, byte[] luma,
            out int xs, out int ys, out int flags)
        {
            PDJPG_Context ctx;
            int i;

            ctx = PDJPG_AllocContext();
            i = PDJPG_DecodeComponentCtx(ctx, buf, sz,
                rgba, norm, spec, luma, out xs, out ys);
            //	ctx.jpg_imgbuf=null;
            flags = (int)ctx.bcst_flags;
            PDJPG_FreeContext(ctx);
            return (i);
        }

        public static int PDJPG_DecodeComponent(BytePtr buf, int sz,
        byte[] rgba, byte[] norm, byte[] spec, byte[] luma,
        out int xs, out int ys, out PDJPG_LayerInfo info)
        {
            PDJPG_Context ctx;
            int i;

            ctx = PDJPG_AllocContext();
            i = PDJPG_DecodeComponentCtx(ctx, buf, sz,
                rgba, norm, spec, luma, out xs, out ys);
            //	ctx.jpg_imgbuf=null;
            info = new PDJPG_LayerInfo();
            info.flags = (int)ctx.bcst_flags;
            info.alpha = ctx.bcst_alpha;
            info.blend = ctx.bcst_blend;
            info.orgx = ctx.bcst_orgx;
            info.orgy = ctx.bcst_orgy;

            PDJPG_FreeContext(ctx);
            return (i);
        }
    }
}
