using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace BGBTechBTJ
{
    public class BTJRange : BTJEntropyCoder
    {
        //public BytePtr cs;			//current pos in bitstream (input)
        //public BytePtr ct;			//current pos in bitstream (output)
        public int pos;			//bitstream offset
        public bool isend;			//bitstream has broken (decoder)

        public uint win;			//bitstream window
        public uint low;
        public uint high;

        public uint minSymRange;        //minimum allowed range

        public uint[] rtab_base;	    //range-table symbol base
        public uint[] rtab_range;   	//range-table symbol range
        
        public int[] rtab_count;	    //range-table symbol count
        public int[] rtab_len;	        //range-table symbol length (16.8)

        public BTJRange()
        {
            SetDefaults();
        }

        public BTJRange(PDJPG_Context ctx)
        {
            SetDefaults();
        }

        public override int SetDefaults()
        {
            rtab_base = new uint[8 * 256];
            rtab_range = new uint[8 * 256];
            rtab_count = new int[8 * 256];
            rtab_len = new int[8 * 256];

            minSymRange = 16;
            return 0;
        }

        public override int NextByte(PDJPG_Context ctx)
        {
            int i, j;

            //	if(isend)return(0xFF);
            if (isend) return (0x00);

            i = cs.next();
            if (i == 0xFF)
            {
                j = cs.next();
                if (j != 0)
                {
                    //			printf("PDJHUFF.NextByte: Bitstream Marker %02X\n", j);
                    isend = true;
                    return (0x00);
                }
            }
            return (i);
        }

        public void EmitByte(PDJPG_Context ctx, int val)
        {
            ct.emit(val & 0xFF);
            if ((val & 0xFF) == 0xFF)
                ct.emit(0x00);
        }

        public override int InitStream(PDJPG_Context ctx, BytePtr buf)
        {
            pos = 0;
            isend = false;

            cs = buf;
            win = (uint)NextByte(ctx);
            win = (uint)((win << 8) + NextByte(ctx));
            win = (uint)((win << 8) + NextByte(ctx));
            win = (uint)((win << 8) + NextByte(ctx));

            low = 0x00000000;
            high = 0xFFFFFFFF;
            return (0);
        }

        public override void InitOutputStream(PDJPG_Context ctx, BytePtr buf)
        {
            ct = buf;
            low = 0x00000000;
            high = 0xFFFFFFFF;
        }


        public void CheckAdjustRangeRead(PDJPG_Context ctx)
        {
            //if (range <= 0xFFFFFF)
            if ((low & 0xFF000000) == (high & 0xFF000000))
            {
                low = low << 8;
                high = (high << 8) + 0xFF;
                win = (uint)((win << 8) + NextByte(ctx));
                return;
            }
            if ((high - low) <= minSymRange)
            {
                //Console.WriteLine("Read Range Collapse");
                //FlushBits(ctx);

                win = (uint)NextByte(ctx);
                win = (uint)((win << 8) + NextByte(ctx));
                win = (uint)((win << 8) + NextByte(ctx));
                win = (uint)((win << 8) + NextByte(ctx));

                low = 0x00000000;
                high = 0xFFFFFFFF;
            }
        }

        public void CheckAdjustRangeWrite(PDJPG_Context ctx)
        {
            //if (range <= 0xFFFFFF)
            if ((low & 0xFF000000) == (high & 0xFF000000))
            {
                EmitByte(ctx, (int)(low >> 24));
                low = low << 8;
                high = (high << 8) + 0xFF;
                return;
            }
            if ((high - low) <= minSymRange)
            {
                //Console.WriteLine("Write Range Collapse {0:X8} {1:X8} {2:X8}<={3:X8}",
                    //low, high, high - low, minSymRange);
                FlushBits2(ctx);
            }
        }

        //Read bit, uniform probability
        public override int ReadBit(PDJPG_Context ctx)
        {
            uint mid, rng;
            int i;

            //rng = high >> 1;
            rng = (high - low) >> 1;
            mid = low + rng;
            if (win >= mid)
            {
                low = mid;
                CheckAdjustRangeRead(ctx);
                return 1;
            }
            else
            {
                high = mid - 1;
                CheckAdjustRangeRead(ctx);
                return 0;
            }
        }

        public override void WriteBit(PDJPG_Context ctx, int i)
        {
            uint mid, rng;

            //rng = high >> 1;
            rng = (high - low) >> 1;
            mid = low + rng;
            if ((i&1) != 0)
            {
                low = mid;
                CheckAdjustRangeWrite(ctx);
            }
            else
            {
                high = mid - 1;
                CheckAdjustRangeWrite(ctx);
            }
        }

        public override int ReadNBits(PDJPG_Context ctx, int n)
        {
            int i, v;

            v = 0;
            for (i = 0; i < n; i++)
                v = (v << 1) + ReadBit(ctx);
            return v;
        }

        public override ulong ReadNBitsL(PDJPG_Context ctx, int n)
        {
            ulong v;
            int i;

            v = 0;
            for (i = 0; i < n; i++)
                v = (v << 1) + (ulong)(ReadBit(ctx));
            return v;
        }

        public override void WriteNBits(PDJPG_Context ctx, int v, int n)
        {
            int i;

            for (i = n - 1; i >= 0; i--)
                WriteBit(ctx, (int)((v >> i)&1));
        }

        public override void WriteNBitsL(PDJPG_Context ctx, ulong val, int n)
        {
            int i;

            for (i = n - 1; i >= 0; i--)
                WriteBit(ctx, (int)((val >> i)&1));
        }

        public override void FlushBits(PDJPG_Context ctx)
        {
            while ((low != 0x00000000) || (high != 0xFFFFFFFF))
            {
                EmitByte(ctx, (int)(low >> 24));
                low = low << 8;
                high = (high << 8) + 0xFF;
            }
        }

        public void FlushBits2(PDJPG_Context ctx)
        {
            //low = low + ((high - low) >> 1);
            EmitByte(ctx, (int)(low >> 24));
            EmitByte(ctx, (int)(low >> 16));
            EmitByte(ctx, (int)(low >> 8));
            EmitByte(ctx, (int)(low >> 0));
            low = 0x00000000;
            high = 0xFFFFFFFF;
        }

        public void SetupRangeTable(int tab)
        {
            uint sb, sr, mr, nr;
            ulong cnt;
            int ltot;
            int i, j, k;

            ltot = 0;
            for (i = 0; i < 256; i++)
            {
                j = rtab_len[tab * 256 + i];
                ltot += j;
            }

            sb = 0x00000000; mr = 0xFFFFFFFF;
            for (i = 0; i < 256; i++)
            {
                j = rtab_len[tab * 256 + i];
                if (j == 0)
                {
                    rtab_base[tab * 256 + i] = 0;
                    rtab_range[tab * 256 + i] = 0;
                    continue;
                }

                // k = tot / Math.Pow(2, ((double)j - 0.5) / 16.0);
                cnt = (ulong)(((double)(1UL << 32) / Math.Pow(2, ((double)j) / 16.0)) + 0.5);
                sr = (uint)((cnt * 0xFFFFFFFFUL) / (1UL<<32));

                if (((ulong)sb + (ulong)sr) > 0xFFFFFFFFUL)
                {
                    sr = 0xFFFFFFFF - sb;
                }

                // rtab_count[tab * 256 + i] = (int)cnt;

                //sr = (uint)((((ulong)j) * 0xFFFFFFFFUL) / (ulong)ltot);
                rtab_base[tab * 256 + i] = sb;
                rtab_range[tab * 256 + i] = sr;
                //if (sr < mr) mr = sr;
                sb += sr;
            }

            //adjust for incomplete tables
            nr = sb;
            for (i = 0; i < 256; i++)
            {
                sb = rtab_base[tab * 256 + i];
                sr = rtab_range[tab * 256 + i];
                if (sr == 0) continue;

                sb = (uint)((sb * 0xFFFFFFFFUL) / nr);
                sr = (uint)((sr * 0xFFFFFFFFUL) / nr);
                rtab_base[tab * 256 + i] = sb;
                rtab_range[tab * 256 + i] = sr;
                if (sr < mr) mr = sr;
            }

            for (i = 0; i < 256; i++)
            {
                j = rtab_len[tab * 256 + i];
                if (j == 0)
                    continue;
                sb = rtab_base[tab * 256 + i];
                sr = rtab_range[tab * 256 + i];
                Console.WriteLine("{0}({1:X2}): L={2}({3:F4}) L={4:X8} H={5:X8} R={6:X8}",
                    tab, i, j, (double)j / 16.0, sb, sb + sr, sr);
            }

            mr = 0xFFFFFFFFU / mr;
            if (mr > minSymRange)
                minSymRange = mr;
            Console.WriteLine("MinRange={0} / {1}", mr, minSymRange);
        }

        public override void BuildTable(IntPtr stat, int tab)
        {
            int tot, ltot;
            int i, j, k;

            tot = 0;
            for (i = 0; i < 256; i++)
                tot += stat[i];

            ltot = 0;
            for (i = 0; i < 256; i++)
            {
                k = stat[i];
                if (k == 0)
                {
                    rtab_count[tab * 256 + i] = 0;
                    rtab_len[tab * 256 + i] = 0;
                    continue;
                }
                rtab_count[tab * 256 + i] = k;
                j = (int)((Math.Log((double)tot / (double)k) / Math.Log(2)) * 16 + 0.5);
                if ((k > 0) && (j < 1)) j = 1;
                if (j > 256) j = 256;
                rtab_len[tab * 256 + i] = j;
                ltot += j;
            }

            SetupRangeTable(tab);
        }

        public override int DecodeSymbol(PDJPG_Context ctx, int tab)
        {
            //	int i, j, k, l;
            uint sb, sr, rng;
            int i, j, k;

            rng = high - low;
            for (i = 0; i < 256; i++)
            {
                sb = (uint)(low + ((((ulong)rtab_base[tab * 256 + i]) * rng) >> 32));
                sr = (uint)((((ulong)rtab_range[tab * 256 + i]) * rng) >> 32);
                if ((win >= sb) && (win < (sb + sr)))
                {
                    low = sb;
                    //high = sb + sr - 1;
                    high = sb + sr;
                    CheckAdjustRangeRead(ctx);
                    return i;
                }
            }

            Console.WriteLine("BTJRang: DecodeSymbol: No Symbol T={0} L={1:X8} H={2:X8} W={3:X8}", tab, low, high, win);
#if true
            for (i = 0; i < 256; i++)
            {
                if (rtab_len[tab * 256 + i] == 0)
                    continue;
                sb = (uint)(low + ((((ulong)rtab_base[tab * 256 + i]) * rng) >> 32));
                sr = (uint)((((ulong)rtab_range[tab * 256 + i]) * rng) >> 32);
                Console.WriteLine("    {0} {1:X8} {2:X8} {3:X8}", i, sb, sb+sr, sr);
                if ((win >= sb) && (win < (sb + sr)))
                {
                    low = sb;
                    //high = sb + sr - 1;
                    high = sb + sr;
                    CheckAdjustRangeRead(ctx);
                    return i;
                }
            }
            throw new Exception();
#endif
            return (-1);
        }

        public override void EncodeSymbol(PDJPG_Context ctx, int tab, int v)
        {
            uint rng, sb, sr;
            rng = high - low;
            sb = (uint)(low + ((((ulong)rtab_base[tab * 256 + v]) * rng) >> 32));
            sr = (uint)((((ulong)rtab_range[tab * 256 + v]) * rng) >> 32);
            low = sb;
            //high = sb + sr - 1;
            high = sb + sr;
            CheckAdjustRangeWrite(ctx);
        }

        public override void EmitDHT(PDJPG_Context ctx, int tab)
        {
            BytePtr ctt;
            int i, j, k;

            ct.emit(0xFF);
            ct.emit(JPG.JPG_APP12);

            ctt = ct;
            ct.emit(0);	//Lf
            ct.emit(0);

            ct.EmitFourcc("DRCT");

            i = (tab / 2) | ((tab & 1) << 4);
            ct.emit(i); 	//Tc Th


            tab <<= 8;
            for (i = 0; i < 256; i++)
            {
                for(j=i; j<256; j++)
                {
                    if(rtab_len[tab + j] != 0)
                        break;
                }

                if (j >= 256)
                {
                    ct.EmitSVLI(0);
                    break;
                }

                if (j > i)
                {
                    k = j - i;
                    ct.EmitSVLI(-k);
                    i = j;
                }

                k = rtab_len[tab + j];
                if (k != 0)
                {
                    ct.EmitSVLI(k);
                }
            }

            //	if(k>=65536)
            //		printf("PDJPG_EmitDHT: Error %04X\n", k);

            //	printf("DHT %04X\n", k);

            i = ct - ctt;
            ctt[0] = i >> 8;	//Lf
            ctt[1] = i & 0xFF;
        }

        public override int MarkerDHT(PDJPG_Context ctx, BytePtr buf)
        {
            BytePtr cs;
            int i, j, k, sz, tn;

            sz = (ushort)((buf[0] << 8) | buf[1]);

            buf += 2;

            i = (buf[0] << 24) | (buf[1] << 16) | (buf[2] << 8) | buf[3];
            //	printf("PDJPG_MarkerAPP12: %08X\n", i);

            if (buf.memcmp("DRCT", 4))
                return (-1);

            cs = buf + 4;
            i = cs.next();

            k = (i >> 4) & 15;
            j = i & 15;
            tn = j * 2 + k;

            for (i = 0; i < 256; i++)
            {
                rtab_len[tn * 256 + i] = 0;
            }

            for (i = 0; i < 256; i++)
            {
                j = (int)cs.ReadSVLI();
                if (j <= 0)
                {
                    if (j == 0)
                        break;
                    i += (-j) - 1;
                    continue;
                }

                rtab_len[tn * 256 + i] = j;
            }

            SetupRangeTable(tn);

            return sz;
        }

        public override BytePtr EmitMiniHufftabBuf(PDJPG_Context ctx, BytePtr buf, int tab)
        {
            return new BytePtr(null);
        }

    }
}
