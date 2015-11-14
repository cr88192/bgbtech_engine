using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace BGBTechBTJ
{
    public class BTJBlock
    {
        public int DecodeDiffDC(PDJPG_Context ctx, int tab)
        {
            int i, j;

            i = ctx.huff.DecodeSymbol(ctx, tab);
            j = ctx.huff.ReadNBits(ctx, i);

            if ((j & (1 << (i - 1))) == 0)
                j = (-1 << i) + j + 1;
            return (j);
        }

        public void QuantBlock(
            PDJPG_Context ctx, ShortPtr ibuf, ShortPtr obuf, int qid, int tag)
        {
            int i;

            if ((tag & 255) != 0)
            {
                for (i = 0; i < 64; i++)
                    obuf[i] = ibuf[i];
                return;
            }

            //	for(i=0; i<64; i++)
            //		obuf[i]=ibuf[i]/ctx.jpg_qt[qid][i];

            for (i = 0; i < 64; i++)
                obuf[i] = ((ibuf[i] * ctx.jpg_qtfp[qid * 64 + i]) + 2048) >> 12;
        }

        public void DequantBlock(
            PDJPG_Context ctx, ShortPtr ibuf, ShortPtr obuf, int qid, int tag)
        {
            int i;

            if ((tag & 255) != 0)
            {
                for (i = 0; i < 64; i++)
                    obuf[i] = ibuf[i];
                return;
            }

            for (i = 0; i < 64; i++)
                obuf[i] = ibuf[i] * ctx.jpg_qt[qid * 64 + i];
        }

#if true
        public void QuantMegaBlock(
            PDJPG_Context ctx, ShortPtr ibuf, ShortPtr obuf, int qid)
        {
            int i, j, k, l;

            //	for(i=0; i<64; i++)
            //		obuf[i]=ibuf[i]/ctx.jpg_qt[qid][i];

            //for (i = 0; i < 128; i++)
            // obuf[i] = (ibuf[i] * ctx.jpg_qtfp[qid * 256 + i]) >> 12;

            for (i = 0; i < 64; i++)
            {
                k = (ibuf[i * 64] * ctx.jpg_qtfp[qid * 256 + 64 + i]) >> 12;
                obuf[i * 64] = k;
            }

            for (i = 1; i < 64; i++)
            {
                for (j = 0; j < 64; j++)
                {
                    k = (ibuf[j * 64 + i] * ctx.jpg_qtfp[qid * 256 + i]) >> 12;
                    obuf[j * 64 + i] = k;
                }
            }

#if true
            l = 0;
            for (i = 0; i < 4096; i++)
            {
                j = obuf[PDJPG.pdjpg_doublezigzag2[i]];
                obuf[PDJPG.pdjpg_doublezigzag2[i]] = j - l;
                l = j;
            }
#endif
        }

        public void DequantMegaBlock(
            PDJPG_Context ctx, ShortPtr ibuf, ShortPtr obuf, int qid)
        {
            int i, j, k;

#if true
            k = 0;
            for (i = 0; i < 4096; i++)
            {
                j = ibuf[PDJPG.pdjpg_doublezigzag2[i]];
                k = k + j;
                obuf[PDJPG.pdjpg_doublezigzag2[i]] = k;
            }
#endif

            for (i = 0; i < 64; i++)
                obuf[i * 64] = obuf[i * 64] * ctx.jpg_qt[qid * 256 + 64 + i];

            for (i = 1; i < 64; i++)
            {
                for (j = 0; j < 64; j++)
                {
                    obuf[j * 64 + i] = obuf[j * 64 + i] * ctx.jpg_qt[qid * 256 + i];
                }
            }
        }
#endif

        public void EncodeVal(PDJPG_Context ctx, int tab, int z, int v)
        {
            int i, j, k;

            if (v == 0) { ctx.huff.EncodeSymbol(ctx, tab, z << 4); return; }

            if (v > 0)
            {
                i = 1; while (v >= (1 << i)) i++;
                if (i > 15)
                {
                    Console.Write(" EncodeVal: Overflow A\n");
                    i = 15;
                }

                ctx.huff.EncodeSymbol(ctx, tab, (z << 4) | i);
                ctx.huff.WriteNBits(ctx, v, i);
                return;
            }

            i = 1; j = -v; while (j >= (1 << i)) i++;
            if (i > 15)
            {
                Console.Write(" EncodeVal: Overflow B\n");
                i = 15;
            }

            ctx.huff.EncodeSymbol(ctx, tab, (z << 4) | i);

            k = (1 << i) - (j + 1);
            ctx.huff.WriteNBits(ctx, k, i);
            return;
        }

        public void StatVal(IntPtr stat, int z, int v)
        {
            int i, j;

            if (v == 0) { stat[z << 4]++; return; }

            if (v > 0)
            {
                i = 1; while (v >= (1 << i)) i++;
                if (i > 15) i = 15;
                stat[(z << 4) | i]++;
                return;
            }

            i = 1; j = -v; while (j >= (1 << i)) i++;
            if (i > 15) i = 15;
            stat[(z << 4) | i]++;
        }

#if true
        public int EncodeBlock(PDJPG_Context ctx,
            ShortPtr buf, int dctab, int actab, int cn, int ni, int nn)
        {
            int i, j;

            if (ctx.altvlc)
            {
                i = EncodeBlockAltVLC(ctx, buf, dctab, actab, cn, ni, nn);
                return i;
            }

            EncodeVal(ctx, dctab, 0, buf[0]);

            for (i = 1; i < 64; i++)
            {
                if (buf[PDJPG.pdjpg_zigzag2[i]] != 0)
                {
                    EncodeVal(ctx, actab, 0, buf[PDJPG.pdjpg_zigzag2[i]]);
                    continue;
                }
                for (j = i; j < 64; j++)
                    if (buf[PDJPG.pdjpg_zigzag2[j]] != 0) break;
                if (j >= 64)
                {
                    ctx.huff.EncodeSymbol(ctx, actab, 0);
                    break;
                }

                j -= i; if (j > 15) j = 15;
                EncodeVal(ctx, actab, j, buf[PDJPG.pdjpg_zigzag2[i + j]]);
                i += j;
            }
            return (0);
        }


        public int StatBlock(PDJPG_Context ctx, ShortPtr buf, IntPtr dcstat, IntPtr acstat, int cn, int ni, int nn)
        {
            int i, j;

            if (ctx.altvlc)
            {
                i = StatBlockAltVLC(ctx, buf, dcstat, acstat, cn, ni, nn);
                return i;
            }

            StatVal(dcstat, 0, buf[0]);
            for (i = 1; i < 64; i++)
            {
                if (buf[PDJPG.pdjpg_zigzag2[i]] != 0)
                {
                    StatVal(acstat, 0, buf[PDJPG.pdjpg_zigzag2[i]]);
                    continue;
                }
                for (j = i; j < 64; j++)
                    if (buf[PDJPG.pdjpg_zigzag2[j]] != 0) break;
                if (j >= 64)
                {
                    acstat[0]++;
                    break;
                }

                j -= i; if (j > 15) j = 15;
                StatVal(acstat, j, buf[PDJPG.pdjpg_zigzag2[i + j]]);
                i += j;
            }
            return (0);
        }

        public int DecodeBlock(PDJPG_Context ctx,
            ShortPtr buf, int dctab, int actab,
            int cn, int ni, int nn)
        {
            int i, j, k;

            if (ctx.altvlc)
            {
                i = DecodeBlockAltVLC(ctx, buf, dctab, actab, cn, ni, nn);
                return i;
            }

            for (i = 1; i < 64; i++) buf[i] = 0;
            //(buf+1).memset(0, 63 * sizeof(short));

            buf[0] += DecodeDiffDC(ctx, dctab);
            for (i = 1; i < 64; i++)
            {
                j = ctx.huff.DecodeSymbol(ctx, actab);
                if (j <= 0)
                {
                    if (j < 0)
                    {
                        //PDJPG.printf("bad dct block1 %02X(%d) %d/%d\n", j, j, ni, nn);
                        return (-1);
                    }
                    break;
                }

                i += (j >> 4) & 15;	//preceding 0's
                if ((j & 15) == 0) continue;
                if (i >= 64)
                {
                    //			printf("bad dct block3 %02X(%d) len=%d %d/%d\n",
                    //				j, j, i, ni, nn);
                    break;
                }

                j &= 15;
                k = ctx.huff.ReadNBits(ctx, j);
                if ((k & (1 << (j - 1))) == 0)
                    k = (-1 << j) + k + 1;

                j = PDJPG.pdjpg_zigzag2[i];
                buf[j] = k;
            }

            if (i > 64)
            {
                //printf("bad dct block2 %02X(%d) len=%d %d/%d\n", j, j, i, ni, nn);
                return (-1);
            }

            return (0);
        }
#endif


#if true
        private int EncodeBlockAltVLC_GetTag(PDJPG_Context ctx, int cn, int ni, int nn)
        {
            int tag;
            switch (cn)
            {
                case 0: tag = ctx.ydtb[ni]; break;
                case 1: tag = ctx.udtb[ni]; break;
                case 2: tag = ctx.vdtb[ni]; break;
                case 3: tag = ctx.wdtb[ni]; break;
                default: tag = 0; break;
            }
            return tag;
        }

        private void EncodeBlockAltVLC_Tag(PDJPG_Context ctx, int dctab, int cn, int ni, int nn)
        {
            int tag, ltag;

            ltag = ctx.cstb[cn];
            //ltag = 0;

            tag = EncodeBlockAltVLC_GetTag(ctx, cn, ni, nn);
            if (true)
            {
                if ((tag & 255) != (ltag & 255))
                    EncodeMegaTag(ctx, dctab, 1, tag & 255);
                if (((tag >> 8) & 255) != ((ltag >> 8) & 255))
                    EncodeMegaTag(ctx, dctab, 2, (tag >> 8) & 255);
                if (((tag >> 16) & 255) != ((ltag >> 16) & 255))
                    EncodeMegaTag(ctx, dctab, 3, (tag >> 16) & 255);
                if (((tag >> 24) & 255) != ((ltag >> 24) & 255))
                    EncodeMegaTag(ctx, dctab, 4, (tag >> 24) & 255);
                ctx.cstb[cn] = tag;
            }
        }

        public void StatBlockAltVLC_Tag(PDJPG_Context ctx, IntPtr dcstat, int cn, int ni, int nn)
        {
            int tag, ltag;

            ltag = ctx.cstb[cn];
            //ltag = 0;

            tag = EncodeBlockAltVLC_GetTag(ctx, cn, ni, nn);
            if (true)
            {
                if ((tag & 255) != (ltag & 255))
                    StatMegaTag(dcstat, 1, tag & 255);
                if (((tag >> 8) & 255) != ((ltag >> 8) & 255))
                    StatMegaTag(dcstat, 2, (tag >> 8) & 255);
                if (((tag >> 16) & 255) != ((ltag >> 16) & 255))
                    StatMegaTag(dcstat, 3, (tag >> 16) & 255);
                if (((tag >> 24) & 255) != ((ltag >> 24) & 255))
                    StatMegaTag(dcstat, 4, (tag >> 24) & 255);
                ctx.cstb[cn] = tag;
            }
        }

        public int EncodeBlockAltVLC(PDJPG_Context ctx,
            ShortPtr buf, int dctab, int actab, int cn, int ni, int nn)
        {
            int i, j;

            EncodeBlockAltVLC_Tag(ctx, dctab, cn, ni, nn);
            EncodeMegaVal(ctx, dctab, 0, buf[0]);

            for (i = 1; i < 64; i++)
            {
                if (buf[PDJPG.pdjpg_zigzag2[i]] != 0)
                {
                    EncodeMegaVal(ctx, actab, 0, buf[PDJPG.pdjpg_zigzag2[i]]);
                    continue;
                }
                for (j = i; j < 64; j++)
                    if (buf[PDJPG.pdjpg_zigzag2[j]] != 0) break;
                if (j >= 64)
                {
                    EncodeMegaVal(ctx, actab, 0, 0);
                    break;
                }

                j -= i; if (j > MEGA_MAXRLE) j = MEGA_MAXRLE;
                EncodeMegaVal(ctx, actab, j, buf[PDJPG.pdjpg_zigzag2[i + j]]);
                i += j;
            }
            return (0);
        }

        public int StatBlockAltVLC(PDJPG_Context ctx, ShortPtr buf, IntPtr dcstat, IntPtr acstat, int cn, int ni, int nn)
        {
            int i, j;

            StatBlockAltVLC_Tag(ctx, dcstat, cn, ni, nn);
            StatMegaVal(dcstat, 0, buf[0]);

            for (i = 1; i < 64; i++)
            {
                if (buf[PDJPG.pdjpg_zigzag2[i]] != 0)
                {
                    StatMegaVal(acstat, 0, buf[PDJPG.pdjpg_zigzag2[i]]);
                    continue;
                }
                for (j = i; j < 64; j++)
                    if (buf[PDJPG.pdjpg_zigzag2[j]] != 0) break;
                if (j >= 64)
                {
                    StatMegaVal(acstat, 0, 0);
                    break;
                }

                j -= i; if (j > MEGA_MAXRLE) j = MEGA_MAXRLE;
                StatMegaVal(acstat, j, buf[PDJPG.pdjpg_zigzag2[i + j]]);
                i += j;
            }
            return (0);
        }

        public int DecodeBlockAltVLC(PDJPG_Context ctx,
            ShortPtr buf, int dctab, int actab,
            int cn, int ni, int nn)
        {
            int i, j, k;

            for (i = 1; i < 64; i++) buf[i] = 0;
            //(buf+1).memset(0, 63 * sizeof(short));

            DecodeMegaValDC(ctx, dctab, out j, out k, cn, ni, nn);
            buf[0] += k;

            //buf[0] +=  DecodeDiffDC(ctx, dctab);
            for (i = 1; i < 64; i++)
            {
                DecodeMegaVal(ctx, actab, out j, out k);

                if ((j == 0) && (k == 0))
                    break;

                i += j;	//preceding 0's
                if ((i >= 64) || (j < 0))
                {
                    Console.Write("bad dct block3 {0:X2}({1}) len={2} {3}/{4}\n",
                        j, j, i, ni, nn);
                    return (-1);
                    break;
                }

                j = PDJPG.pdjpg_zigzag2[i];
                buf[j] = k;
            }

#if false
            buf[0] +=  DecodeDiffDC(ctx, dctab);
            for (i = 1; i < 64; i++)
            {
                j =  DecodeSymbol(ctx, actab);
                if (j <= 0)
                {
                    if (j < 0)
                    {
                        //PDJPG.printf("bad dct block1 %02X(%d) %d/%d\n", j, j, ni, nn);
                        return (-1);
                    }
                    break;
                }

                i += (j >> 4) & 15;	//preceding 0's
                if ((j & 15) == 0) continue;
                if (i >= 64)
                {
                    //			printf("bad dct block3 %02X(%d) len=%d %d/%d\n",
                    //				j, j, i, ni, nn);
                    break;
                }

                j &= 15;
                k =  ReadNBits(ctx, j);
                if ((k & (1 << (j - 1))) == 0)
                    k = (-1 << j) + k + 1;

                j = PDJPG.pdjpg_zigzag2[i];
                buf[j] = k;
            }
#endif

            if (i > 64)
            {
                Console.Write("bad dct block2 {0:X2}({1}) len={2} {3}/{4}\n", j, j, i, ni, nn);
                return (-1);
            }

            return (0);
        }
#endif

#if false
        static ulong[] bsxrp_ibase=new ulong[] {
        	//16 bit range
	        0x0000, 0x0001, 0x0002, 0x0003, 0x0004, 0x0005, 0x0006, 0x0007,
	        0x0008, 0x000A, 0x000C, 0x000E,
	
	        0x0010, 0x0014, 0x0018, 0x001C, 0x0020, 0x0028, 0x0030, 0x0038,
	        0x0040, 0x0050, 0x0060, 0x0070, 0x0080, 0x00A0, 0x00C0, 0x00E0,
	        0x0100, 0x0140, 0x0180, 0x01C0, 0x0200, 0x0280, 0x0300, 0x0380,
	        0x0400, 0x0500, 0x0600, 0x0700, 0x0800, 0x0A00, 0x0C00, 0x0E00,
	        0x1000, 0x1400, 0x1800, 0x1C00, 0x2000, 0x2800, 0x3000, 0x3800,
	        0x4000, 0x5000, 0x6000, 0x7000, 0x8000, 0xA000, 0xC000, 0xE000,
	
	        //32-bit range
	        0x00010000, 0x00014000, 0x00018000, 0x0001C000,
	        0x00020000, 0x00028000, 0x00030000, 0x00038000,
	        0x00040000, 0x00050000, 0x00060000, 0x00070000,
	        0x00080000, 0x000A0000, 0x000C0000, 0x000E0000,

	        0x00100000, 0x00140000, 0x00180000, 0x001C0000,
	        0x00200000, 0x00280000, 0x00300000, 0x00380000,
	        0x00400000, 0x00500000, 0x00600000, 0x00700000,
	        0x00800000, 0x00A00000, 0x00C00000, 0x00E00000,

	        0x01000000, 0x01400000, 0x01800000, 0x01C00000,
	        0x02000000, 0x02800000, 0x03000000, 0x03800000,
	        0x04000000, 0x05000000, 0x06000000, 0x07000000,
	        0x08000000, 0x0A000000, 0x0C000000, 0x0E000000,

	        0x10000000, 0x14000000, 0x18000000, 0x1C000000,
	        0x20000000, 0x28000000, 0x30000000, 0x38000000,
	        0x40000000, 0x50000000, 0x60000000, 0x70000000,
	        0x80000000, 0xA0000000, 0xC0000000, 0xE0000000,

	        //64 bit range
	        0x0000000100000000, 0x0000000140000000,
	        0x0000000180000000, 0x00000001C0000000,
	        0x0000000200000000, 0x0000000280000000,
	        0x0000000300000000, 0x0000000380000000,
	        0x0000000400000000, 0x0000000500000000,
	        0x0000000600000000, 0x0000000700000000,
	        0x0000000800000000, 0x0000000A00000000,
	        0x0000000C00000000, 0x0000000E00000000,

        	0x0000001000000000, 0x0000001400000000,
	        0x0000001800000000, 0x0000001C00000000,
	        0x0000002000000000, 0x0000002800000000,
	        0x0000003000000000, 0x0000003800000000,
	        0x0000004000000000, 0x0000005000000000,
	        0x0000006000000000, 0x0000007000000000,
	        0x0000008000000000, 0x000000A000000000,
	        0x000000C000000000, 0x000000E000000000,

	        0x0000010000000000, 0x0000014000000000,
	        0x0000018000000000, 0x000001C000000000,
	        0x0000020000000000, 0x0000028000000000,
	        0x0000030000000000, 0x0000038000000000,
	        0x0000040000000000, 0x0000050000000000,
	        0x0000060000000000, 0x0000070000000000,
	        0x0000080000000000, 0x00000A0000000000,
	        0x00000C0000000000, 0x00000E0000000000,

	        0x0000100000000000, 0x0000140000000000,
	        0x0000180000000000, 0x00001C0000000000,
	        0x0000200000000000, 0x0000280000000000,
	        0x0000300000000000, 0x0000380000000000,
	        0x0000400000000000, 0x0000500000000000,
	        0x0000600000000000, 0x0000700000000000,
	        0x0000800000000000, 0x0000A00000000000,
	        0x0000C00000000000, 0x0000E00000000000,

	        0x0001000000000000, 0x0001400000000000,
	        0x0001800000000000, 0x0001C00000000000,
	        0x0002000000000000, 0x0002800000000000,
	        0x0003000000000000, 0x0003800000000000,
	        0x0004000000000000, 0x0005000000000000,
	        0x0006000000000000, 0x0007000000000000,
	        0x0008000000000000, 0x000A000000000000,
	        0x000C000000000000, 0x000E000000000000,

	        0x0010000000000000, 0x0014000000000000,
	        0x0018000000000000, 0x001C000000000000,
	        0x0020000000000000, 0x0028000000000000,
	        0x0030000000000000, 0x0038000000000000,
	        0x0040000000000000, 0x0050000000000000,
	        0x0060000000000000, 0x0070000000000000,
	        0x0080000000000000, 0x00A0000000000000,
	        0x00C0000000000000, 0x00E0000000000000,

	        0x0100000000000000, 0x0140000000000000,
	        0x0180000000000000, 0x01C0000000000000,
	        0x0200000000000000, 0x0280000000000000,
	        0x0300000000000000, 0x0380000000000000,
	        0x0400000000000000, 0x0500000000000000,
	        0x0600000000000000, 0x0700000000000000,
	        0x0800000000000000, 0x0A00000000000000,
	        0x0C00000000000000, 0x0E00000000000000,

	        0x1000000000000000, 0x1400000000000000,
	        0x1800000000000000, 0x1C00000000000000,
	        0x2000000000000000, 0x2800000000000000,
	        0x3000000000000000, 0x3800000000000000,
	        0x4000000000000000, 0x5000000000000000,
	        0x6000000000000000, 0x7000000000000000,
	        0x8000000000000000, 0xA000000000000000,
	        0xC000000000000000, 0xE000000000000000,
    	};

        static int[] bsxrp_iextra = new int[] {
    		 0,  0,  0,  0,   0,  0,  0,  0,
		    1,  1,  1,  1,
		    2,  2,  2,  2,   3,  3,  3,  3,
		    4,  4,  4,  4,   5,  5,  5,  5,
		    6,  6,  6,  6,   7,  7,  7,  7,
		    8,  8,  8,  8,   9,  9,  9,  9,
		    10, 10, 10, 10,  11, 11, 11, 11,
		    12, 12, 12, 12,  13, 13, 13, 13,
		
		    //32 bit range
		    14, 14, 14, 14,  15, 15, 15, 15,
		    16, 16, 16, 16,  17, 17, 17, 17,
   
    		18, 18, 18, 18,  19, 19, 19, 19,
		    20, 20, 20, 20,  21, 21, 21, 21,

		    22, 22, 22, 22,  23, 23, 23, 23,
		    24, 24, 24, 24,  25, 25, 25, 25,

		    26, 26, 26, 26,  27, 27, 27, 27,
		    28, 28, 28, 28,  29, 29, 29, 29,
		
    		//64 bit range
		    30, 30, 30, 30,  31, 31, 31, 31,
		    32, 32, 32, 32,  33, 33, 33, 33,

		    34, 34, 34, 34,  35, 35, 35, 35,
		    36, 36, 36, 36,  37, 37, 37, 37,

    		38, 38, 38, 38,  39, 39, 39, 39,
	    	40, 40, 40, 40,  41, 41, 41, 41,

    		42, 42, 42, 42,  43, 43, 43, 43,
	    	44, 44, 44, 44,  45, 45, 45, 45,

    		46, 46, 46, 46,  47, 47, 47, 47,
	    	48, 48, 48, 48,  49, 49, 49, 49,

    		50, 50, 50, 50,  51, 51, 51, 51,
	    	52, 52, 52, 52,  53, 53, 53, 53,

    		54, 54, 54, 54,  55, 55, 55, 55,
	    	56, 56, 56, 56,  57, 57, 57, 57,

    		58, 58, 58, 58,  59, 59, 59, 59,
	    	60, 60, 60, 60,  61, 61, 61, 61,
	    };
#endif

#if true
        static ulong[] bsxrp_ibase = new ulong[] {
        	//16 bit range
	        0x0000, 0x0001, 0x0002, 0x0003,
            0x0004, 0x0006, 0x0008, 0x000C,
	        0x0010, 0x0018, 0x0020, 0x0030,
            0x0040, 0x0060, 0x0080, 0x00C0,
            0x0100, 0x0180, 0x0200, 0x0300,
            0x0400, 0x0600, 0x0800, 0x0C00,
            0x1000, 0x1800, 0x2000, 0x3000,
            0x4000, 0x6000, 0x8000, 0xC000,

            //32-bit range
            0x00010000, 0x00018000, 0x00020000, 0x00030000,
            0x00040000, 0x00060000, 0x00080000, 0x000C0000,
            0x00100000, 0x00180000, 0x00200000, 0x00300000,
            0x00400000, 0x00600000, 0x00800000, 0x00C00000,
            0x01000000, 0x01800000, 0x02000000, 0x03000000,
            0x04000000, 0x06000000, 0x08000000, 0x0C000000,
            0x10000000, 0x18000000, 0x20000000, 0x30000000,
            0x40000000, 0x60000000, 0x80000000, 0xC0000000,

            //64-bit range
            0x0000000100000000, 0x0000000180000000, 0x0000000200000000, 0x0000000300000000,
            0x0000000400000000, 0x0000000600000000, 0x0000000800000000, 0x0000000C00000000,
            0x0000001000000000, 0x0000001800000000, 0x0000002000000000, 0x0000003000000000,
            0x0000004000000000, 0x0000006000000000, 0x0000008000000000, 0x000000C000000000,
            0x0000010000000000, 0x0000018000000000, 0x0000020000000000, 0x0000030000000000,
            0x0000040000000000, 0x0000060000000000, 0x0000080000000000, 0x00000C0000000000,
            0x0000100000000000, 0x0000180000000000, 0x0000200000000000, 0x0000300000000000,
            0x0000400000000000, 0x0000600000000000, 0x0000800000000000, 0x0000C00000000000,
            0x0001000000000000, 0x0001800000000000, 0x0002000000000000, 0x0003000000000000,
            0x0004000000000000, 0x0006000000000000, 0x0008000000000000, 0x000C000000000000,
            0x0010000000000000, 0x0018000000000000, 0x0020000000000000, 0x0030000000000000,
            0x0040000000000000, 0x0060000000000000, 0x0080000000000000, 0x00C0000000000000,
            0x0100000000000000, 0x0180000000000000, 0x0200000000000000, 0x0300000000000000,
            0x0400000000000000, 0x0600000000000000, 0x0800000000000000, 0x0C00000000000000,
            0x1000000000000000, 0x1800000000000000, 0x2000000000000000, 0x3000000000000000,
            0x4000000000000000, 0x6000000000000000, 0x8000000000000000, 0xC000000000000000,

    	};

        static int[] bsxrp_iextra = new int[] {
    		 0,  0,  0,  0,
		     1,  1,  2,  2,
		     3,  3,  4,  4,
		     5,  5,  6,  6,
		     7,  7,  8,  8,
		     9,  9, 10, 10,
		    11, 11, 12, 12,
		    13, 13, 14, 14,
		
		    //32 bit range
		    15, 15, 16, 16,
		    17, 17, 18, 18,
    		19, 19, 20, 20,
		    21, 21, 22, 22,
		    23, 23, 24, 24,
		    25, 25, 26, 26,
		    27, 27, 28, 28,
		    29, 29, 30, 30,
		
    		//64 bit range
		    31, 31, 32, 32,
            
            33, 33, 34, 34,
            35, 35, 36, 36,
            37, 37, 38, 38,
            39, 39, 40, 40,
            41, 41, 42, 42,
            43, 43, 44, 44,
            45, 45, 46, 46,
            47, 47, 48, 48,
            49, 49, 50, 50,
            51, 51, 52, 52,
            53, 53, 54, 54,
            55, 55, 56, 56,
            57, 57, 58, 58,
            59, 59, 60, 60,
            61, 61, 62, 62,
	    };
#endif

        public void EncodeUVLC(PDJPG_Context ctx, int tab, int sbase, ulong val)
        {
            ulong lj, lk;
            int i;

            lj = val;
            for (i = 0; i < 252; i++)
            {
                lk = bsxrp_ibase[i] + (1UL << bsxrp_iextra[i]);
                if ((val >= bsxrp_ibase[i]) && (lj < lk))
                {
                    ctx.huff.EncodeSymbol(ctx, tab, sbase + i);
                    ctx.huff.WriteNBitsL(ctx, lj - bsxrp_ibase[i], bsxrp_iextra[i]);
                    break;
                }
            }
        }

        public void StatUVLC(IntPtr stat, int sbase, ulong val)
        {
            ulong lj, lk;
            int i;

            lj = val;
            for (i = 0; i < 252; i++)
            {
                lk = bsxrp_ibase[i] + (1UL << bsxrp_iextra[i]);
                if ((val >= bsxrp_ibase[i]) && (lj < lk))
                {
                    stat[sbase + i]++;
                    break;
                }
            }
        }

        public ulong DecodeUVLC(PDJPG_Context ctx, int tab)
        {
            ulong lk;
            int i;

            i = ctx.huff.DecodeSymbol(ctx, tab);
            lk = bsxrp_ibase[i] + ctx.huff.ReadNBitsL(ctx, bsxrp_iextra[i]);
            return (lk);
        }

        public void DecodePrefixUVLC(PDJPG_Context ctx, int pf, out ulong v)
        {
            v = bsxrp_ibase[pf] + ctx.huff.ReadNBitsL(ctx, bsxrp_iextra[pf]);
        }

        public int GetPrefixUVLC(int sbase, ulong val)
        {
            ulong lj, lk;
            int i;

            lj = val;
            for (i = 0; i < 252; i++)
            {
                lk = bsxrp_ibase[i] + (1UL << bsxrp_iextra[i]);
                if ((val >= bsxrp_ibase[i]) && (lj < lk))
                {
                    return (sbase + i);
                }
            }
            return (-1);
        }

        public int GetPrefixSVLC(int sbase, long v)
        {
            ulong lv;
            lv = (ulong)((v >= 0) ? (v << 1) : (((-v) << 1) - 1));
            return GetPrefixUVLC(sbase, lv);
        }

#if true

#if false
        public void EncodeMegaUVLC(PDJPG_Context ctx, int tab, int b, int v)
        {
            int i, j, k;

            if (v > 0)
            {
                i = 1; while (v >= (1 << i)) i++;
                if (i > 63)
                {
                    Console.Write(" EncodeVal: Overflow A\n");
                    i = 63;
                }

                 EncodeSymbol(ctx, tab, b + i);
                 WriteNBits(ctx, v, i);
                return;
            }

            if (v == 0)
            {
                 EncodeSymbol(ctx, tab, b);
            }

            i = 1; j = -v; while (j >= (1 << i)) i++;
            if (i > 63)
            {
                Console.Write(" EncodeVal: Overflow B\n");
                i = 63;
            }

             EncodeSymbol(ctx, tab, b + i);

            k = (1 << i) - (j + 1);
             WriteNBits(ctx, k, i);
            return;
        }


        public void StatMegaUVLC(IntPtr stat, int b, int v)
        {
            int i, j;

            if (v > 0)
            {
                i = 1; while (v >= (1 << i)) i++;
                if (i > 63) i = 63;
                stat[b + i]++;
                return;
            }

            if (v == 0)
            {
                stat[b]++;
                return;
            }

            i = 1; j = -v; while (j >= (1 << i)) i++;
            if (i > 15) i = 15;
            stat[b + i]++;
        }

        public void DecodeMegaUVLC(PDJPG_Context ctx, int pf, out int v)
        {
            int k;

            if (pf == 0)
            {
                v = 0;
                return;
            }

            k =  ReadNBits(ctx, pf);
            if ((k & (1 << (pf - 1))) == 0)
                k = (-1 << pf) + k + 1;
            v = k;
        }
#endif

        public void EncodeMegaVLC(PDJPG_Context ctx, int tab, int b, int v)
        {
            ulong lv;
            lv = (ulong)((v >= 0) ? (v << 1) : (((-v) << 1) - 1));
            //EncodeMegaUVLC(ctx, tab, b, i);
            EncodeUVLC(ctx, tab, b, lv);
        }

        public void StatMegaVLC(IntPtr stat, int b, int v)
        {
            ulong lv;
            lv = (ulong)((v >= 0) ? (v << 1) : (((-v) << 1) - 1));
            //StatMegaUVLC(stat, b, i);
            StatUVLC(stat, b, lv);
        }

        public void DecodeMegaVLC(PDJPG_Context ctx, int pf, out int v)
        {
            ulong lv;
            long li;
            DecodePrefixUVLC(ctx, pf, out lv);
            li = ((lv & 1) != 0) ? (-(long)((lv + 1) >> 1)) : (long)(lv >> 1);
            v = (int)li;
        }

        public void EncodeMegaTag(PDJPG_Context ctx, int tab, int z, int v)
        {
            EncodeMegaVLC(ctx, tab, 240, z);
            EncodeMegaVLC(ctx, tab, 0, v);
        }

        public void StatMegaTag(IntPtr stat, int z, int v)
        {
            StatMegaVLC(stat, 240, z);
            StatMegaVLC(stat, 0, v);
        }

        public void EncodeMegaVal(PDJPG_Context ctx, int tab, int z, int v)
        {
            int i, j, k;

#if false
            i = GetPrefixSVLC(0, -v);
            if ((z < 14) && (i<16))
            {
                EncodeMegaVLC(ctx, tab, (z<<4), -v);
                //EncodeVal(ctx, tab, z, v);
                return;
            }
            else
            {
                //EncodeMegaVLC(ctx, tab, 224, z);
                EncodeUVLC(ctx, tab, 224, (ulong)z);
                EncodeMegaVLC(ctx, tab, 0, -v);
                return;
            }
#endif

#if true
            if ((z < 14) && (v < 32767) && (v > -32767))
            {
                EncodeVal(ctx, tab, z, v);
                return;
            }
            else
            {
                //EncodeMegaVLC(ctx, tab, 224, z);
                EncodeUVLC(ctx, tab, 224, (ulong)z);
                EncodeMegaVLC(ctx, tab, 0, v);
                return;
            }
#endif

#if false
            EncodeVal(ctx, tab, z, v);
            return;
#endif

#if false
            if ((z == 0) && (v == 0))
            {
                ctx.huff.EncodeSymbol(ctx, tab, 128);
                return;
            }

            if (z > 0)
            {
                EncodeMegaVLC(ctx, tab, 128, z);
            }
            EncodeMegaVLC(ctx, tab, 0, v);

            //if (v == 0) {  EncodeSymbol(ctx, tab, z << 4); return; }
#endif

        }

        public void StatMegaVal(IntPtr stat, int z, int v)
        {
            int i, j;

#if false
            i = GetPrefixSVLC(0, -v);
            if ((z < 14) && (i < 16))
            {
                StatMegaVLC(stat, (z << 4), -v);
                return;
            }
            else
            {
                //EncodeMegaVLC(ctx, tab, 224, z);
                StatUVLC(stat, 224, (ulong)z);
                StatMegaVLC(stat, 0, -v);
                return;
            }
#endif

#if true
            if ((z < 14) && (v < 32767) && (v > -32767))
            {
                StatVal(stat, z, v);
                return;
            }
            else
            {
                //StatMegaVLC(stat, 224, z);
                StatUVLC(stat, 224, (ulong)z);
                StatMegaVLC(stat, 0, v);
                return;
            }
#endif

#if false
            StatVal(stat, z, v);
            return;
#endif

#if false
            if ((z == 0) && (v == 0))
            {
                stat[128]++;
                return;
            }

            //if (v == 0) { stat[z << 4]++; return; }
            if (z > 0)
            {
                StatMegaVLC(stat, 128, z);
            }
            StatMegaVLC(stat, 0, v);
#endif
        }


        public int DecodeMegaVal(PDJPG_Context ctx, int tab, out int z, out int v)
        {
            ulong lv;
            int i, j, k;

#if false
            j = ctx.huff.DecodeSymbol(ctx, tab);
            if (j < 224)
            {
                if (j <= 0)
                {
                    if (j < 0)
                    {
                        //PDJPG.printf("bad dct block1 %02X(%d) %d/%d\n", j, j, ni, nn);
                        z = 0; v = 0;
                        return -1;
                    }
                    z = 0; v = 0;
                    return 0;
                }

                z = (j >> 4) & 15;	//preceding 0's

                DecodeMegaVLC(ctx, j & 15, out k);
                v = -k;

                return 0;
            }
            else
            {
                //DecodeMegaVLC(ctx, j - 224, out k);
                DecodePrefixUVLC(ctx, j - 224, out lv);
                //z = k;
                z = (int)lv;

                j = ctx.huff.DecodeSymbol(ctx, tab);

                DecodeMegaVLC(ctx, j, out k);
                v = -k;
                return 0;
            }
#endif

#if true
            j = ctx.huff.DecodeSymbol(ctx, tab);
            if (j < 224)
            {
                if (j <= 0)
                {
                    if (j < 0)
                    {
                        //PDJPG.printf("bad dct block1 %02X(%d) %d/%d\n", j, j, ni, nn);
                        z = 0; v = 0;
                        return -1;
                    }
                    z = 0; v = 0;
                    return 0;
                }

                z = (j >> 4) & 15;	//preceding 0's

                j &= 15;
                k = ctx.huff.ReadNBits(ctx, j);
                if ((k & (1 << (j - 1))) == 0)
                    k = (-1 << j) + k + 1;

                v = k;

                return 0;
            }
            else
            {
                //DecodeMegaVLC(ctx, j - 224, out k);
                DecodePrefixUVLC(ctx, j - 224, out lv);
                //z = k;
                z = (int)lv;

                j = ctx.huff.DecodeSymbol(ctx, tab);

                DecodeMegaVLC(ctx, j, out k);
                v = k;
                return 0;
            }
#endif

#if false
            j =  DecodeSymbol(ctx, tab);
            if (j <= 0)
            {
                if (j < 0)
                {
                    //PDJPG.printf("bad dct block1 %02X(%d) %d/%d\n", j, j, ni, nn);
                    z = 0; v = 0;
                    return -1;
                }
                z = 0; v = 0;
                return 0;
            }

            z = (j >> 4) & 15;	//preceding 0's

            j &= 15;
            k =  ReadNBits(ctx, j);
            if ((k & (1 << (j - 1))) == 0)
                k = (-1 << j) + k + 1;

            v = k;

            return 0;
#endif

#if false
            j =  ctx.huff.DecodeSymbol(ctx, tab);
            if (j < 0)
            {
                //PDJPG.printf("bad dct block1 %02X(%d) %d/%d\n", j, j, ni, nn);
                z = 0; v = 0;
                return (-1);
            }

            if (j >= 128)
            {
                if (j == 128)
                {
                    z = 0; v = 0;
                    return 0;
                }

                DecodeMegaVLC(ctx, j - 128, out k);
                z = k;

                j =  ctx.huff.DecodeSymbol(ctx, tab);
            }
            else
            {
                z = 0;
            }

            DecodeMegaVLC(ctx, j, out k);
            v = k;
            return 0;
#endif
        }

        public int DecodeMegaValDC(PDJPG_Context ctx, int tab, out int z, out int v, int cn, int ni, int nn)
        {
            ulong lv;
            uint tag;
            int i, j, k;

#if true
            j = ctx.huff.DecodeSymbol(ctx, tab);

            tag = (uint)ctx.cstb[cn];
            //tag = 0;

            //handle any tags
            while (j >= 240)
            {
                DecodeMegaVLC(ctx, j - 240, out k);
                switch (k)
                {
                    case 1:
                        j = ctx.huff.DecodeSymbol(ctx, tab);
                        DecodeMegaVLC(ctx, j, out k);
                        tag = (tag & (uint)0xFFFFFF00) | (uint)k;
                        break;
                    case 2:
                        j = ctx.huff.DecodeSymbol(ctx, tab);
                        DecodeMegaVLC(ctx, j, out k);
                        tag = (tag & (uint)0xFFFF00FF) | (uint)(k << 8);
                        //ctx.jpg_stbuf[cn][ni] |= k << 8;
                        break;
                    case 3:
                        j = ctx.huff.DecodeSymbol(ctx, tab);
                        DecodeMegaVLC(ctx, j, out k);
                        tag = (tag & (uint)0xFF00FFFF) | (uint)(k << 16);
                        //ctx.jpg_stbuf[cn][ni] |= k << 16;
                        break;
                    case 4:
                        j = ctx.huff.DecodeSymbol(ctx, tab);
                        DecodeMegaVLC(ctx, j, out k);
                        tag = (tag & (uint)0x00FFFFFF) | (uint)(k << 24);
                        //ctx.jpg_stbuf[cn][ni] |= k << 24;
                        break;
                    default:
                        break;
                }
                j = ctx.huff.DecodeSymbol(ctx, tab);
            }

            ctx.jpg_stbuf[cn][ni] = (int)tag;
            ctx.cstb[cn] = (int)tag;

            if (j < 224)
            {
                if (j <= 0)
                {
                    if (j < 0)
                    {
                        //PDJPG.printf("bad dct block1 %02X(%d) %d/%d\n", j, j, ni, nn);
                        z = 0; v = 0;
                        return -1;
                    }
                    z = 0; v = 0;
                    return 0;
                }

                z = (j >> 4) & 15;	//preceding 0's

                j &= 15;
                k = ctx.huff.ReadNBits(ctx, j);
                if ((k & (1 << (j - 1))) == 0)
                    k = (-1 << j) + k + 1;

                v = k;

                return 0;
            }
            else
            {
                //DecodeMegaVLC(ctx, j - 224, out k);
                DecodePrefixUVLC(ctx, j - 224, out lv);
                //z = k;
                z = (int)lv;

                j = ctx.huff.DecodeSymbol(ctx, tab);

                DecodeMegaVLC(ctx, j, out k);
                v = k;
                return 0;
            }
#endif
        }

        //static int MEGA_MAXRLE = 15;
        static int MEGA_MAXRLE = 255;

        public int EncodeMegaBlock(PDJPG_Context ctx,
            ShortPtr buf, int dctab, int actab)
        {
            int i, j, k;

            EncodeMegaVal(ctx, dctab, 0, buf[0]);

            for (i = 1; i < 64; i++)
            {
                k = buf[PDJPG.pdjpg_doublezigzag2[i]];
                if (k != 0)
                {
                    EncodeMegaVal(ctx, dctab, 0, k);
                    continue;
                }
                for (j = i; j < 64; j++)
                    if (buf[PDJPG.pdjpg_doublezigzag2[j]] != 0) break;

                if (j >= 64)
                {
                    // EncodeSymbol(ctx, dctab, 0);
                    EncodeMegaVal(ctx, dctab, 0, 0);
                    break;
                }

                j -= i; if (j > MEGA_MAXRLE) j = MEGA_MAXRLE;
                EncodeMegaVal(ctx, dctab, j, buf[PDJPG.pdjpg_doublezigzag2[i + j]]);
                i += j;
            }

            for (i = 64; i < 4096; i++)
            {
                k = buf[PDJPG.pdjpg_doublezigzag2[i]];
                if (k != 0)
                {
                    EncodeMegaVal(ctx, actab, 0, k);
                    continue;
                }

                k = i + (MEGA_MAXRLE + 1); if (k > 4096) k = 4096;
                for (j = i; j < k; j++)
                    if (buf[PDJPG.pdjpg_doublezigzag2[j]] != 0) break;

                if (j >= 4096)
                {
                    // EncodeSymbol(ctx, actab, 0);
                    EncodeMegaVal(ctx, actab, 0, 0);
                    break;
                }

                j -= i; if (j > MEGA_MAXRLE) j = MEGA_MAXRLE;
                EncodeMegaVal(ctx, actab, j, buf[PDJPG.pdjpg_doublezigzag2[i + j]]);
                i += j;
            }

            //             EncodeSymbol(ctx, actab, 0);

            return (0);
        }

        public int StatMegaBlock(ShortPtr buf, IntPtr dcstat, IntPtr acstat)
        {
            int i, j, k;

            StatMegaVal(dcstat, 0, buf[0]);

            for (i = 1; i < 64; i++)
            {
                k = buf[PDJPG.pdjpg_doublezigzag2[i]];
                if (k != 0)
                {
                    StatMegaVal(dcstat, 0, k);
                    continue;
                }
                for (j = i; j < 64; j++)
                    if (buf[PDJPG.pdjpg_doublezigzag2[j]] != 0) break;
                if (j >= 64)
                {
                    //dcstat[0]++;
                    StatMegaVal(dcstat, 0, 0);
                    break;
                }

                j -= i; if (j > MEGA_MAXRLE) j = MEGA_MAXRLE;
                StatMegaVal(dcstat, j, buf[PDJPG.pdjpg_doublezigzag2[i + j]]);
                i += j;
            }

            for (i = 64; i < 4096; i++)
            {
                k = buf[PDJPG.pdjpg_doublezigzag2[i]];
                if (k != 0)
                {
                    StatMegaVal(acstat, 0, k);
                    continue;
                }

                k = i + (MEGA_MAXRLE + 1); if (k > 4096) k = 4096;
                for (j = i; j < k; j++)
                    if (buf[PDJPG.pdjpg_doublezigzag2[j]] != 0) break;
                if (j >= 4096)
                {
                    //acstat[0]++;
                    StatMegaVal(acstat, 0, 0);
                    break;
                }

                j -= i; if (j > MEGA_MAXRLE) j = MEGA_MAXRLE;
                StatMegaVal(acstat, j, buf[PDJPG.pdjpg_doublezigzag2[i + j]]);
                i += j;
            }

            //            acstat[0]++;

            return (0);
        }
#endif

#if true
        public int DecodeMegaBlock(PDJPG_Context ctx,
            ShortPtr buf, int dctab, int actab,
            int ni, int nn)
        {
            int i, j, k;

            for (i = 1; i < 4096; i++) buf[i] = 0;
            //(buf+1).memset(0, 63 * sizeof(short));
            j = 0;

            DecodeMegaVal(ctx, dctab, out j, out k);
            buf[0] += k;

            //buf[0] +=  DecodeDiffDC(ctx, dctab);
            for (i = 1; i < 64; i++)
            {
                DecodeMegaVal(ctx, dctab, out j, out k);

                if ((j == 0) && (k == 0))
                    break;

                i += j;	//preceding 0's
                if ((i >= 64) || (j < 0))
                {
                    Console.Write("bad dct block3 {0}({1}) len={2} {3}/{4}\n",
                        j, j, i, ni, nn);
                    break;
                }

                j = PDJPG.pdjpg_doublezigzag2[i];
                buf[j] = k;
            }

            for (i = 64; i < 4096; i++)
            {
                DecodeMegaVal(ctx, actab, out j, out k);

                if ((j == 0) && (k == 0))
                    break;

                i += j;	//preceding 0's
                if ((i >= 4096) || (j < 0))
                {
                    Console.Write("bad dct block3 {0}({1}) len={2} {3}/{4}\n",
                        j, j, i, ni, nn);
                    break;
                }

                j = PDJPG.pdjpg_doublezigzag2[i];
                buf[j] = k;
            }

#if true
            if (i > 4096)
            {
                Console.Write("bad dct block2 {0}({1}) len={2} {3}/{4}\n", j, j, i, ni, nn);
                return (-1);
            }
#endif

            return (0);
        }
#endif
    }
}
