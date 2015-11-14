using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace BGBTechBTJ
{
    public abstract class BTJEntropyCoder
    {
        public BytePtr cs;			//current pos in bitstream (input)
        public BytePtr ct;			//current pos in bitstream (output)

        public abstract int SetDefaults();

        public abstract int NextByte(PDJPG_Context ctx);
        public abstract int InitStream(PDJPG_Context ctx, BytePtr buf);
        public abstract int ReadBit(PDJPG_Context ctx);
        public abstract int ReadNBits(PDJPG_Context ctx, int n);
        public abstract ulong ReadNBitsL(PDJPG_Context ctx, int n);

        public abstract void InitOutputStream(PDJPG_Context ctx, BytePtr buf);
        public abstract void WriteBit(PDJPG_Context ctx, int i);
        public abstract void WriteNBits(PDJPG_Context ctx, int v, int n);
        public abstract void WriteNBitsL(PDJPG_Context ctx, ulong val, int n);
        public abstract void FlushBits(PDJPG_Context ctx);

        public abstract void BuildTable(IntPtr stat, int tab);

        public abstract int DecodeSymbol(PDJPG_Context ctx, int tab);
        public abstract void EncodeSymbol(PDJPG_Context ctx, int tab, int v);

        public abstract void EmitDHT(PDJPG_Context ctx, int tab);
        public abstract int MarkerDHT(PDJPG_Context ctx, BytePtr buf);

        public abstract BytePtr EmitMiniHufftabBuf(PDJPG_Context ctx, BytePtr buf, int tab);
    }

    public class PDJHUFF:BTJEntropyCoder
    {
        public ushort[] code;	//huffman code bits
        public ushort[] mask;	//huffman code masks
        public short[] next;	//huffman code next
        public byte[] len;	    //huffman code length
        public short[] idx;	//huffman code masks

        //public BytePtr cs;			//current pos in bitstream (input)
        //public BytePtr ct;			//current pos in bitstream (output)
        public uint win;			//bitstream window
        public int pos;			//bitstream offset
        public bool isend;			//bitstream has broken (decoder)

        //Decoder

        public PDJHUFF()
        {
            SetDefaults();
        }

        public PDJHUFF(PDJPG_Context ctx)
        {
            SetDefaults();
        }

        public override int SetDefaults()
        {
            int i;

            code = new ushort[8 * 256];
            mask = new ushort[8 * 256];
            next = new short[8 * 256];
            len = new byte[8 * 256];
            idx = new short[8 * 256];

            //dummy huffman tables
            for (i = 0; i < 256; i++) len[0 * 256 + i] = 0;
            for (i = 0; i < 256; i++) len[1 * 256 + i] = 0;
            for (i = 0; i < 256; i++) len[2 * 256 + i] = 0;
            for (i = 0; i < 256; i++) len[3 * 256 + i] = 0;

            for (i = 0; i < 16; i++) len[0 * 256 + i] = 4;
            for (i = 0; i < 16; i++) len[2 * 256 + i] = 4;
            for (i = 0; i < 255; i++) len[1 * 256 + i] = 8;
            for (i = 0; i < 255; i++) len[3 * 256 + i] = 8;

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
                if (j!=0)
                {
                    //			printf(" NextByte: Bitstream Marker %02X\n", j);
                    isend = true;
                    return (0x00);
                }
            }
            return (i);
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
            return (0);
        }

        public override int ReadBit(PDJPG_Context ctx)
        {
            int i;

            i = (int)(win >> (31 - pos)) & 1;
            pos++;
            if (pos >= 8)
            {
                win = (uint)((win << 8) + NextByte(ctx));
                pos -= 8;
            }
            return (i);
        }

        public override int ReadNBits(PDJPG_Context ctx, int n)
        {
            int i;

            if (n <= 0) return (0);

            i = (int)((win >> (32 - n - pos)) & ((1 << n) - 1));
            pos += n;
            while (pos >= 8)
            {
                win = (uint)((win << 8) + NextByte(ctx));
                pos -= 8;
            }
            return (i);
        }

        public override ulong ReadNBitsL(PDJPG_Context ctx, int n)
        {
            ulong v;
            int i, j;

            i = n; v = 0;
            while (i > 16)
            {
                j=ReadNBits(ctx, 16);
                v += ((ulong)j) << (i-16);
                i -= 16;
            }
            j = ReadNBits(ctx, i);
            v += (ulong)j;
            
            return v;
        }

        public void SkipNBits(PDJPG_Context ctx, int n)
        {
            pos += n;
            while (pos >= 8)
            {
                win = (uint)((win << 8) + NextByte(ctx));
                pos -= 8;
            }
        }

        public int PeekWord(PDJPG_Context ctx)
        {
            int i;
            i = (int)((win >> (16 - pos)) & 65535);
            return (i);
        }

        public override void InitOutputStream(PDJPG_Context ctx, BytePtr buf)
        {
            ct = buf;
            win = 0;
            pos = 0;
        }

        public override void WriteBit(PDJPG_Context ctx, int i)
        {
            win |= (uint)(i << (31 - pos));
            pos++;
            if (pos >= 8)
            {
                i = (int)((win >> 24) & 0xFF);
                ct.emit(i);
                if (i == 0xFF) ct.emit(0x00);
                win <<= 8;
                pos -= 8;
            }
        }

        public override void WriteNBits(PDJPG_Context ctx, int v, int n)
        {
            int i, j, k;

            v &= (1 << n) - 1;

            j = pos + n;
            k = (int)(win + (v << (32 - j)));
            while (j >= 8)
            {
                i = (k >> 24) & 0xFF;
                ct.emit(i);
                if (i == 0xFF) ct.emit(0x00);
                k <<= 8;
                j -= 8;
            }
            pos = j;
            win = (uint)k;

            //	win|=i<<((32-n)-pos);
            //	pos+=n;
            //	while(pos>=8)
            //	{
            //		i=(win>>24)&0xFF;
            //		*ct++=i;
            //		if(i==0xFF)*ct++=0x00;
            //		win<<=8;
            //		pos-=8;
            //	}
        }

        public override void WriteNBitsL(PDJPG_Context ctx, ulong val, int n)
        {
            ulong l;
            int i;

            l = val; i = n;
            while (i > 16)
            {
                WriteNBits(ctx, (int)((l >> (i - 16)) & 65535), 16);
                i -= 16;
            }
            WriteNBits(ctx, (int)(l & ((1UL << i) - 1)), i);
        }

        public override void FlushBits(PDJPG_Context ctx)
        {
            int i;
            while (pos > 0)
            {
                i = (int)((win >> 24) & 0xFF);
                ct.emit(i);
                if (i == 0xFF) ct.emit(0x00);
                win <<= 8;
                pos -= 8;
            }
        }

        public void WriteRice(PDJPG_Context ctx, int v, int n)
        {
            int i;

            i = v >> n;
            while ((i--) != 0)
                WriteBit(ctx, 1);
            WriteBit(ctx, 0);
            WriteNBits(ctx, v & ((1 << n) - 1), n);
        }

        public int ReadRice(PDJPG_Context ctx, int n)
        {
            int i, j;

            i = 0;
            while (ReadBit(ctx)!=0)
                i++;
            j = ReadNBits(ctx, n);
            return ((i << n) | j);
        }

        public void WriteSRice(PDJPG_Context ctx, int v, int n)
        {
            int v1;
            v1 = ((v >= 0) ? (v << 1) : (((-v) << 1) - 1));
            WriteRice(ctx, v1, n);
        }

        public int ReadSRice(PDJPG_Context ctx, int n)
        {
            int v, v1;
            v1 = ReadRice(ctx, n);
            v = ((v1 & 1) != 0) ? (-((v1 + 1) >> 1)) : (v1 >> 1);
            return v;
        }

#if false
        public int DecodeSymbol(PDJPG_Context ctx, int tab)
        {
            int i, j, k, l;

            //	i=PeekWord(ctx);
            //	i=(win>>(16-pos))&65535;
            //	i=(u16)(win>>(16-pos));

#if true
            i = (byte)(win >> (24 - pos));

            tab <<= 8;
            //	j=idx[tab|k];
            //	j=idx[tab|(i>>8)];
            j = idx[tab | i];

#if false
            //	l=tab|j;
            //	k=len[l];
            k = len[tab | j];
            //	if(0)
            if (k <= 8)
            //	if(k<8)
            {
                pos += k;
                //		while(pos>=8)
                if (pos >= 8)
                {
                    win = (uint)((win << 8) + PDJHUFF.NextByte(ctx));
                    pos -= 8;
                }

                //		PDJHUFF.SkipNBits(ctx, k);
                return (j);
            }
#endif

            i = (ushort)(win >> (16 - pos));
            j = idx[tab | (i >> 8)];

            while (j>=0)
            {
                l = tab | j;
                k = len[l];
                if (k==0)
                {
                    //			printf(" DecodeSymbol: Trap A\n");
                    break;
                }

#if true
                if ((i >> (16 - k)) != code[l])
                //		if((k>=8) && (i>>(16-k))!=code[l])
                {
                    j = next[l];
                    //			if(!j)
                    if (j < 0)
                    {
                        //				printf(" DecodeSymbol: Trap B\n");
                        break;
                    }
                    continue;
                }
#endif

                pos += k;
                while (pos >= 8)
                //		if(pos>=8)
                {
                    win = (uint)((win << 8) +  NextByte(ctx));
                    pos -= 8;
                }

                //		 SkipNBits(ctx, k);
                return (j);
            }

#endif

#if false
	i= PeekWord(ctx);
	for(j=0; j<256; j++)
	{
		k=len[tab|j];
		if(!k)continue;

		if((i>>(16-k))!=code[tab|j])
			continue;
		 SkipNBits(ctx, k);
		return(j);
	}
#endif

            return (-1);
        }
#endif


#if true
        public override int DecodeSymbol(PDJPG_Context ctx, int tab)
        {
            //	int i, j, k, l;
            int i, j, k;

            tab <<= 8;

#if false
            i =  PeekWord(ctx);
            j = idx[tab | (i >> 8)];
            while (1)
            {
                l = tab | j;
                k = len[l];
                //		if(!k)break;

                if ((i >> (16 - k)) != code[l])
                {
                    j = next[l];
                    if (!j) break;
                    continue;
                }
                 SkipNBits(ctx, k);
                return (j);
            }
#endif

#if true
            i =  PeekWord(ctx);
            for (j = 0; j < 256; j++)
            {
                k = len[tab | j];
                if (k <= 0) continue;

                if ((i >> (16 - k)) != code[tab | j])
                    continue;
                 SkipNBits(ctx, k);
                return (j);
            }
#endif

            return (-1);
        }
#endif


        private int BalanceTree_r(ShortPtr nodes, ShortPtr nlen, int root, int h, int ml)
        {
            int h0, h1, h2, h3;
            int l0, l1, l2;

            if (root < 0) return (0);

            h1 =  BalanceTree_r(nodes, nlen, nodes[root * 2 + 0], h + 1, ml);
            h2 =  BalanceTree_r(nodes, nlen, nodes[root * 2 + 1], h + 1, ml);
            h0 = ((h1 > h2) ? h1 : h2) + 1;
            nlen[root] = (short)h0;

            if ((h + h0) <= ml)	//depth limit not exceeded
                return (h0);

            //ok, so part of the tree is too deep
            if ((h1 + 1) < h2)
            {
                l0 = nodes[root * 2 + 1];
                l1 = nodes[l0 * 2 + 1];
                nodes[l0 * 2 + 1] = nodes[l0 * 2 + 0];
                nodes[l0 * 2 + 0] = nodes[root * 2 + 0];
                nodes[root * 2 + 0] = (short)l0;
                nodes[root * 2 + 1] = (short)l1;
            }
            else if ((h2 + 1) < h1)
            {
                l0 = nodes[root * 2 + 0];
                l1 = nodes[l0 * 2 + 0];
                nodes[l0 * 2 + 0] = nodes[l0 * 2 + 1];
                nodes[l0 * 2 + 1] = nodes[root * 2 + 1];
                nodes[root * 2 + 0] = (short)l1;
                nodes[root * 2 + 1] = (short)l0;
            }
            else
            {
                //rotating would be ineffective or would make things worse...
                return (h0);
            }

            //recalc depth of modified sub-tree
            l1 = nodes[l0 * 2 + 0];
            l2 = nodes[l0 * 2 + 1];
            h1 = (l1 < 0) ? 0 : nlen[l1];
            h2 = (l2 < 0) ? 0 : nlen[l2];
            h3 = ((h1 > h2) ? h1 : h2) + 1;
            nlen[l0] = (short)h3;

            //recalc height of root node
            l1 = nodes[root * 2 + 0];
            l2 = nodes[root * 2 + 1];
            h1 = (l1 < 0) ? 0 : nlen[l1];
            h2 = (l2 < 0) ? 0 : nlen[l2];
            h0 = ((h1 > h2) ? h1 : h2) + 1;
            nlen[root] = (short)h0;

            return (h0);
        }

        private void CalcLengths_r(ShortPtr nodes, BytePtr cl, int root, int h)
        {
            if (root < 0)
            {
                if (root == -513) return;
                cl[(-root) - 1] = (byte)h;
                return;
            }

             CalcLengths_r(nodes, cl, nodes[root * 2 + 0], h + 1);
             CalcLengths_r(nodes, cl, nodes[root * 2 + 1], h + 1);
        }

        private int BuildLengths(IntPtr stat, int nc, BytePtr cl, int ml)
        {
            //	short nodes[1024], nlen[512];
            //	short roots[512], clen[512];
            //	int cnts[512];

            short[] nodes = new short[1024];
            short[] nlen = new short[512];
            short[] roots = new short[512];
            short[] clen = new short[512];
            int[] cnts = new int[512];
            int nr, nn;
            int i, j, k, l;

            nr = 0; nn = 0;
            for (i = 0; i < nc; i++)
            {
                if (stat[i]==0) continue;
                roots[nr] = (short)(-(i + 1));
                cnts[nr] = stat[i];
                clen[nr] = 0;
                nr++;
            }

            roots[nr] = -513;
            cnts[nr] = 0;
            clen[nr] = 0;
            nr++;


            for (i = 0; i < nc; i++) cl[i] = 0;
            if (nr==0) return (-1);


            while (nr > 1)
            {
                if (cnts[0] >= cnts[1]) { j = 0; k = 1; }
                else { j = 1; k = 0; }
                for (i = 2; i < nr; i++)
                {
                    if (cnts[i] <= cnts[k])
                    {
                        j = k; k = i;
                        continue;
                    }
                    if (cnts[i] <= cnts[j])
                    {
                        j = i;
                        continue;
                    }
                }

                nlen[nn] = (short)(((clen[j] > clen[k]) ? clen[j] : clen[k]) + 1);
                nodes[nn * 2 + 0] = roots[j];
                nodes[nn * 2 + 1] = roots[k];

                roots[nr] = (short)nn;
                cnts[nr] = cnts[j] + cnts[k];
                clen[nr] = nlen[nn];

                //		printf("%d %d %d\n", cnts[j], cnts[k], cnts[nr]);

                nn++; nr++;

                l = 0;
                for (i = 0; i < nr; i++)
                {
                    if ((i == j) || (i == k)) continue;
                    roots[l] = roots[i];
                    cnts[l] = cnts[i];
                    clen[l] = clen[i];
                    l++;
                }
                nr = l;
            }

            l = roots[0];
            j = clen[0];
            k = j;

            i = 4;
            while (((i--)>0) && (k > ml))
                k =  BalanceTree_r(new ShortPtr(nodes), new ShortPtr(nlen), l, 0, ml);
            if (k > ml) return (-2);	//failed to balance tree

             CalcLengths_r(new ShortPtr(nodes), cl, l, 0);
            return (0);
        }

        private int BuildLengthsAdjust(IntPtr stat, int nc, BytePtr cl, int ml)
        {
            int i, j;

            while (true)
            {
                j =  BuildLengths(stat, nc, cl, ml);
                if (j < 0)
                {
                    Console.Write(" BuildLengthsAdjust: Huff Fail %d\n");
                }

                for (i = 0; i < nc; i++)
                    if ((stat[i]!=0) && (cl[i]==0))
                        break;
                if (i >= nc) break;

                Console.Write(" BuildLengthsAdjust: Fiddle Adjust\n");
                for (i = 0; i < nc; i++)
                    stat[i]++;
                continue;
            }
            return (0);
        }

        public override void BuildTable(IntPtr stat, int tab)
        {
            BuildLengthsAdjust(stat, 256, new BytePtr(len, tab * 256), 16);
        }

        public override void EncodeSymbol(PDJPG_Context ctx, int tab, int v)
        {
            tab <<= 8;

            if (len[tab | v] <= 0)
            {
                Console.Write("EncodeSymbol: Bad {0} {1}\n", tab, v);
            }

             WriteNBits(ctx, code[tab | v], len[tab | v]);
        }

        public override void EmitDHT(PDJPG_Context ctx, int tab)
        {
            BytePtr p;
            int i, j, k;

            ct.emit(0xFF);
            ct.emit(JPG.JPG_DHT);

            i = 8 + 3 * 1;	//temp value
            p = ct;
            ct.emit(i >> 8);	//Lf
            ct.emit(i & 0xFF);

            i = (tab / 2) | ((tab & 1) << 4);
            ct.emit(i); 	//Tc Th

            tab <<= 8;
            for (i = 1; i <= 16; i++)
            {
                k = 0;
                for (j = 0; j < 256; j++)
                    if (len[tab | j] == i)
                        k++;
                ct.emit(k); 	//Li
            }

            k = 0;
            for (i = 1; i <= 16; i++)
            {
                k <<= 1;
                for (j = 0; j < 256; j++)
                    if (len[tab | j] == i)
                    {
                        ct.emit(j); 	//Vi
                        code[tab | j] = (ushort)(k++);
                    }
            }

            //	if(k>=65536)
            //		printf("PDJPG_EmitDHT: Error %04X\n", k);

            //	printf("DHT %04X\n", k);

            i = ct - p;
            p[0] = i >> 8;	//Lf
            p[1] = i & 0xFF;
        }

        public override int MarkerDHT(PDJPG_Context ctx, BytePtr buf)
        {
            BytePtr cnt;
            BytePtr s;
            int i, j, k, l, tn;

            l = (buf[0] << 8) | buf[1];

            j = (buf[2] >> 4) & 15;
            i = buf[2] & 15;
            //	printf("dest %d, type %d\n", i, j);

            tn = i * 2 + j;

            for (i = 0; i < 256; i++)
            {
                len[tn * 256 + i] = 0;
                next[tn * 256 + i] = -1;
                idx[tn * 256 + i] = -1;
            }

            cnt = buf + 3;
            s = cnt + 16;
            k = 0; l = 0;
            for (i = 0; i < 16; i++)
            {
                k <<= 1;
                for (j = 0; j < cnt[i]; j++)
                {
                    code[tn * 256 + (s[0])] = (ushort)(k++);
                    len[tn * 256 + (s[0])] = (byte)(i + 1);
                    mask[tn * 256 + (s[0])] = (ushort)((1 << (i + 1)) - 1);
                    l = i + 1;
                    s++;
                }
            }
            //	printf("%04X %d\n", k, l);

            //build lookup indices / chains

            for (i = 0; i < 256; i++)
            {
                //		next[tn*256+i]=0;
                //		idx[tn*256+i]=0;
                next[tn * 256 + i] = -1;
                idx[tn * 256 + i] = -1;
            }

            for (i = 0; i < 256; i++)
            {
                l = len[tn * 256 + i];
                if (l == 0) continue;
                j = code[tn * 256 + i];

                if (l <= 8)
                {
                    j = j << (8 - l);
                    k = 1 << (8 - l);
                    while (((k--) > 0) && (j < 256))
                    {
                        idx[tn * 256 + j] = (short)i;
                        j++;
                    }
                }
                else
                {
                    j = j >> (l - 8);
                    next[tn * 256 + i] =
                        idx[tn * 256 + j];
                    idx[tn * 256 + j] = (short)i;
                }
            }

            return (s - buf);
            //	return(0);
        }


        int BSXRP_SetupTable(BytePtr cl, int ncl,
            TypePtr<ushort> tc, TypePtr<ushort> tm, BytePtr tl,
            TypePtr<short> ti, TypePtr<short> tn)
        {
            int[] cnt = new int[16];
            int[] nc = new int[16];
            int[] nn = new int[16];
            int[] nf = new int[16];
            int i, j, k, k2, l, ff;

            for (i = 0; i < 16; i++) { cnt[i] = 0; nc[i] = 0; nn[i] = -1; nf[i] = -1; }
            for (i = 0; i < 256; i++) ti[i] = -1;

            for (i = 0; i < ncl; i++) cnt[cl[i]]++;
            cnt[0] = 0;

#if true
            j = cl[0];
            for (i = 15; i >= 1; i--)
                if (cnt[i]!=0) break;
            if (j > i) j = i;
            if (i==0) return (-12);	//no codes

            j = 1;
            for (i = 1; i < 16; i++)
            {
                j <<= 1;
                j -= cnt[i];

                //		dyPrintf("%d %d %d\n", i, j, cnt[i]);
                if (j < 0) return (-10);	//over subscribed
            }
            if ((j > 0) && ((ncl - cnt[0]) != 1))
                return (-11);	//incomplete set
#endif

            j = 0;
            for (i = 1; i < 16; i++)
            {
                j = (j + cnt[i - 1]) << 1;
                nc[i] = j;
            }

            for (i = 0; i < ncl; i++)
            {
                l = cl[i];
                if (l==0) continue;

                tl[i] = cl[i];
                tm[i] = (ushort)((1 << tl[i]) - 1);
                tc[i] = (ushort)(nc[l]++);

                if (nn[l] >= 0)
                {
                    tn[nn[l]] = (short)i;
                    nn[l] = i;
                }
                else
                {
                    nf[l] = i;
                    nn[l] = i;
                }
            }

            j = -1; ff = -1;
            for (i = 1; i < 16; i++)
            {
                if (nf[i] >= 0)
                {
                    if (ff < 0) ff = nf[i];
                    if (j >= 0) tn[j] = (short)nf[i];
                    j = nn[i];
                }
            }
            if (j >= 0) tn[j] = -1;

            for (i = ff; i != 0xFFFF; i = tn[i])
            {
                l = cl[i];
                if (l <= 8)
                {
                    k2 = tc[i] << (8 - l);
                    j = 1 << (8 - l);
                    while ((j--)!=0)
                    {
                        //k = BSXRP_TransposeByte(k2++);
                        k = k2++;
                        if (ti[k] == -1) ti[k] = (short)i;
                    }
                }
                else
                {
                    k2 = tc[i] >> (l - 8);
                    //k = BSXRP_TransposeByte(k2);
                    k = k2;
                    if (ti[k] == -1) ti[k] = (short)i;
                }

                k2 = (tc[i]) << (16 - l);
                //k = BSXRP_TransposeWord(k2++);
                k = k2++;
                tc[i] = (ushort)k;
            }

            j = 0;
            for (i = 0; i < 256; i++) if (ti[i] == -1)
                {
                    Console.Write("table bad index {0}\n", i);
                    j = -9;
                }
            return (j);
        }

        int BSXRP_DecodeHuffTableFixed(PDJPG_Context ctx,
            int tab, int tn)
        {
            byte[] lcl = new byte[288];
            int hl;
            int i, j;

            switch (tn)
            {
                case 0:
                    hl = 256;
                    for (i = 0; i < 256; i++) lcl[i] = 8;
                    break;
                case 1:
                    hl = 288;
                    for (i = 0; i < 144; i++) lcl[i] = 8;
                    for (i = 144; i < 256; i++) lcl[i] = 9;
                    for (i = 256; i < 280; i++) lcl[i] = 7;
                    for (i = 280; i < 288; i++) lcl[i] = 8;
                    break;
                case 2:
                    hl = 256;
                    for (i = 0; i < 16; i++) lcl[i] = 5;
                    for (i = 16; i < 32; i++) lcl[i] = 6;
                    for (i = 32; i < 64; i++) lcl[i] = 8;
                    for (i = 64; i < 128; i++) lcl[i] = 9;
                    for (i = 128; i < 256; i++) lcl[i] = 11;
                    break;
                default:
                    hl = 256;
                    for (i = 0; i < 256; i++) lcl[i] = 8;
                    break;
            }

            j = BSXRP_SetupTable(new BytePtr(lcl), hl,
                new TypePtr<ushort>(code, tab * 256), new TypePtr<ushort>(mask, tab * 256), new BytePtr(len, tab * 256),
                new TypePtr<short>(idx, tab * 256), new TypePtr<short>(next, tab * 256));
            if (j < 0)
            {
                //dyPrintf("Static Literal/Length Table Problem "
                //"Tab=%d, Tn=%d, Code=%d\n", tab, tn, j);
                return (j);
            }

            return (0);
        }

        static int[] bsxrp_lorder= new int[] {
	        16, 17, 18, 0, 8,7, 9,6, 10,5, 11,4, 12,3, 13,2, 14,1, 15, 19};
        static int[] bsxrp_lorder2 = new int[] {
	        3, 17, 15, 13, 11,9, 7,5, 4,6, 8,10, 12,14, 16,18, 0,1, 2, 19};

        public int BSXRP_DecodeHufftabSymbol(PDJPG_Context ctx)
        {
            int i;
            i = ReadRice(ctx, 2);
            return (bsxrp_lorder[i]);
        }

        public void BSXRP_EncodeHufftabSymbol(PDJPG_Context ctx, int v)
        {
            WriteRice(ctx, bsxrp_lorder2[v], 2);
        }

        public void BSXRP_StatLengths(PDJPG_Context ctx,
            BytePtr cl, int nc, IntPtr stat)
        {
            int i, j, l;

            l = -1;
            for (i = 0; i < nc; )
            {
                for (j = 0; ((i + j) < nc) && (cl[i + j] == 0); j++) ;
                if (j > 138) j = 138;
                if (j > 10) { stat[18]++; i += j; l = 0; continue; }
                if (j > 2) { stat[17]++; i += j; l = 0; continue; }

                for (j = 0; ((i + j) < nc) && (cl[i + j] == l); j++) ;
                if (j > 6) j = 6;
                if (j > 2) { stat[16]++; i += j; continue; }

                l = cl[i++];
                stat[l]++;
            }
        }

        public int BSXRP_DecodeCodeLengths(PDJPG_Context ctx, BytePtr cl, int ncl)
        {
            int i, j, k;

            for (i = 0; i < ncl; i++) cl[i] = 0;

            i = 0; k = 0;
            while (i < ncl)
            {
                j = BSXRP_DecodeHufftabSymbol(ctx);
                if (j < 0) return (j);

                if (j < 17)
                { k = j; cl[i++] = j; continue; }

                if (j == 17)
                {
                    j = ReadNBits(ctx, 3) + 3;
                    while ((j--)!=0) cl[i++] = k;
                    continue;
                }
                if (j == 18)
                {
                    j = ReadNBits(ctx, 7) + 11;
                    while ((j--)!=0) cl[i++] = k;
                    continue;
                }
                if (j == 19) break;

                return (-7);
            }
            return (0);
        }

        public void BSXRP_EncodeCodeLengths(PDJPG_Context ctx, BytePtr cl, int nc)
        {
            int i, j, l;

            i = 0; l = -1;
            while (i < nc)
            {
                for (j = 0; ((i + j) < nc) && (cl[i + j] == l); j++) ;
                if (j > 10)
                {
                    //check for end of table
                    if (((i + j) >= nc) && (l == 0))
                        break;

                    if (j > 138) j = 138;
                    BSXRP_EncodeHufftabSymbol(ctx, 18);
                    WriteNBits(ctx, j - 11, 7);
                    i += j;
                    continue;
                }
                if (j > 2)
                {
                    BSXRP_EncodeHufftabSymbol(ctx, 17);
                    WriteNBits(ctx, j - 3, 3);
                    i += j;
                    continue;
                }

                l = cl[i++];
                BSXRP_EncodeHufftabSymbol(ctx, l);

                //		dyPrintf("L%d ", l);

                if ((l < 0) || (l > 16)) Console.Write("problem: bad code length noted\n");
            }
            BSXRP_EncodeHufftabSymbol(ctx, 19);
            //	dyPrintf("\n");
        }

        public int BSXRP_DecodeHuffTable(PDJPG_Context ctx, int tab)
        {
            byte[] lcl = new byte[512];
            int ht, hk, hl;
            int i, j, k;

            ht = ReadNBits(ctx, 4);
            hk = ReadNBits(ctx, 4);

            if (ht == 0)
            {
                i = -1;
                //i = BSXRP_DecodeHuffTableFixed(ctx, tab, hk);
                return (i);
            }

            if (ht != 1)
            {
                Console.Write("Bad Huffman Table Type {0}\n", ht);
                return (-1);
            }

            BSXRP_DecodeCodeLengths(ctx, new BytePtr(lcl), 256);
            //j = BSXRP_SetupTable(lcl, 256,
                //code[tab], mask[tab], len[tab],
                //idx[tab], next[tab]);
            j = BSXRP_SetupTable(new BytePtr(lcl), 256,
                new TypePtr<ushort>(code, tab * 256), new TypePtr<ushort>(mask, tab * 256), new BytePtr(len, tab * 256),
                new TypePtr<short>(idx, tab * 256), new TypePtr<short>(next, tab * 256));
            if (j < 0)
            {
                Console.Write("Decode Dynamic Literal/Length Table Problem Tab={0}, Code={1}\n", tab, j);
                return (j);
            }

            return (0);
        }

        public int BSXRP_EncodeHuffTable(PDJPG_Context ctx,
            int tab, BytePtr cl, int nc)
        {
            int i, j;

            WriteNBits(ctx, 1, 4);
            WriteNBits(ctx, 2, 4);
            BSXRP_EncodeCodeLengths(ctx, cl, nc);

            //j = BSXRP_SetupTable(cl, nc,
                //code[tab], mask[tab], len[tab],
                //idx[tab], next[tab]);
            j = BSXRP_SetupTable(cl, nc,
                new TypePtr<ushort>(code, tab * 256), new TypePtr<ushort>(mask, tab * 256), new BytePtr(len, tab * 256),
                new TypePtr<short>(idx, tab * 256), new TypePtr<short>(next, tab * 256));
            if (j < 0)
            {
                Console.Write("Encode Dynamic Literal/Length Table Problem Tab={0}, Code={1}\n", tab, j);
                return (j);
            }

            return (0);
        }

        public override BytePtr EmitMiniHufftabBuf(PDJPG_Context ctx, BytePtr buf, int tab)
        {
            BytePtr ct1, ct2;

            ct1 = ct;
            InitOutputStream(ctx, buf);

            WriteNBits(ctx, 1, 4);
            WriteNBits(ctx, 2, 4);
            BSXRP_EncodeCodeLengths(ctx, new BytePtr(len, tab * 256), 256);
            FlushBits(ctx);
            ct2 = ct;
            ct = ct1;

            return ct2;
        }
    }
}
