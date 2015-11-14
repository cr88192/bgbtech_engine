using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace BGBTechBTJ
{
    public class PDJPG_LayerInfo
    {
        public int flags;
        public byte alpha;
        public byte blend;
        public int orgx;
        public int orgy;

        public int minx;
        public int miny;
        public int maxx;
        public int maxy;
    }

    public class PDJPG_Context
    {
        public BTJEntropyCoder huff;
        public BTJBlock block;

        //Huffman
        // u16 huff_code[8*256];	//huffman code bits
        // u16 huff_mask[8*256];	//huffman code masks
        // s16 huff_next[8*256];	//huffman code next
        // byte huff_len[8*256];	//huffman code length
        // s16 huff_idx[8*256];	    //huffman code masks

        //public ushort[] huff_code;	//huffman code bits
        //public ushort[] huff_mask;	//huffman code masks
        //public short[] huff_next;	//huffman code next
        //public byte[] huff_len;	    //huffman code length
        //public short[] huff_idx;	//huffman code masks

        //public BytePtr huff_cs;			//current pos in bitstream (input)
        //public BytePtr huff_ct;			//current pos in bitstream (output)
        //public uint huff_win;			//bitstream window
        //public int huff_pos;			//bitstream offset
        //public bool huff_isend;			//bitstream has broken (decoder)

        //range coder
        //public uint range_win;			//bitstream range-coder window
        //public uint range_low;		    //bitstream range-coder base
        //public uint range_high;		//bitstream range-coder range

        //public uint[] rtab_base;	    //range-table symbol base
        //public uint[] rtab_range;   	//range-table symbol range

        //Common

        public byte[] jpg_qt;		//quantization tables
        public int[] jpg_qtfp;	//quantization tables (fixed point)

        public int alphaClr;			//color used for alpha (RGBA)
        public byte alphaClrY;
        public byte alphaClrU;
        public byte alphaClrV;
        public byte alphaClrA;
        public int oldAlphaClr;		//used to detect change

        public ushort bcst_version;
        public uint bcst_flags;
        public byte bcst_trans;
        public byte bcst_bpp;
        public byte bcst_alpha;
        public byte bcst_blend;
        public int bcst_orgx;
        public int bcst_orgy;

        public int bcst_minx;
        public int bcst_miny;
        public int bcst_maxx;
        public int bcst_maxy;

        public int dvfh_lidx;

        //Decoder Specific

        public int xs, ys;		//image size (raw image)
        public int xs2, ys2;	//image size (Y plane)
        public int xs3, ys3;	//image size (UV planes)


        public byte[] jpg_cid;
        public byte[] jpg_ch;
        public byte[] jpg_cv;
        public byte[] jpg_qid;
        public int[] jpg_cxi;
        public int[] jpg_cyi;
        public int jpg_nc;
        public int jpg_chm;
        public int jpg_chn;

        public int[] jpg_scid;
        public int[] jpg_scn;
        public ShortPtr[] jpg_scbuf;    //scan buffer
        // BytePtr jpg_sibuf[4];
        public ShortPtr[] jpg_sibuf;    //scan image buffer
        public IntPtr[] jpg_stbuf;      //scan tag buffer

        public int[] jpg_schs;	    	//scan horizontal size
        public int[] jpg_scvs;		    //scan vertical size

        public int[] jpg_schsci;		//scan horizontal scale
        public int[] jpg_scvsci;		//scan vertical scale

        public BytePtr jpg_imgbuf;		//image buffer

        public bool jpg_is420;			//image is 420
        public bool jpg_is444;			//image is 444
        public bool jpg_rdct;			//image uses RDCT
        public byte jpg_clrtrans;		//color space transform
        public bool jpg_yuvw;			//image uses YUVW
        public bool layer_hidden;       //currennt layer is hidden
        public bool megablock;          //using megablock encoding
        public bool altvlc;             //alternate VLC coding
        public bool rangecoder;         //uses range coding
        public bool auto_dct;           //auto-DCT
        public bool jpg_noquant;        //no quantization used
        public bool jpg_dmlt;           //use DMLT ("Define Mini-Lump Tables") headers


        public byte[] imgbuf_rgba;		//RGBA image buffer
        public byte[] imgbuf_norm;		//XYZD image buffer
        public byte[] imgbuf_spec;		//Spec image buffer
        public byte[] imgbuf_luma;		//Luma image buffer
        public byte[] imgbuf_lebe;		//LeBe image buffer

        //Encoder Specific
        public bool jpg_mono;
        public int jpg_tabcacheframe;	//cache table frame count.
        public int jpg_qfl;

        public short[] yb;		//Y pixel buffer
        public short[] ub;		//U pixel buffer
        public short[] vb;		//V pixel buffer
        public short[] wb;		//W pixel buffer

        public short[] ydb;		//Y DCT block buffer
        public short[] udb;		//U DCT block buffer
        public short[] vdb;		//V DCT block buffer
        public short[] wdb;		//W DCT block buffer

        public int[] ydtb;	//Y DCT block tag buffer
        public int[] udtb;	//U DCT block tag buffer
        public int[] vdtb;	//V DCT block tag buffer
        public int[] wdtb;	//W DCT block tag buffer
        public int[] cstb;  //current scan-tag buffer

        public int lxs;
        public int lys;

        public int[] dcs;
        public int[] acs;
        public int[] dcsuv;
        public int[] acsuv;
    }

    public class JPG
    {
        public const int JPG_SOF0 = 0xC0;
        public const int JPG_SOF1 = 0xC1;
        public const int JPG_SOF2 = 0xC2;
        public const int JPG_SOF3 = 0xC3;
        public const int JPG_DHT = 0xC4;
        public const int JPG_SOF5 = 0xC5;
        public const int JPG_SOF6 = 0xC6;
        public const int JPG_SOF7 = 0xC7;
        public const int JPG_JPG = 0xC8;
        public const int JPG_SOF9 = 0xC9;
        public const int JPG_SOF10 = 0xCA;
        public const int JPG_SOF11 = 0xCB;
        public const int JPG_DAC = 0xCC;
        public const int JPG_SOF13 = 0xCD;
        public const int JPG_SOF14 = 0xCE;
        public const int JPG_SOF15 = 0xCF;

        public const int JPG_RST0 = 0xD0;
        public const int JPG_RST1 = 0xD1;
        public const int JPG_RST2 = 0xD2;
        public const int JPG_RST3 = 0xD3;
        public const int JPG_RST4 = 0xD4;
        public const int JPG_RST5 = 0xD5;
        public const int JPG_RST6 = 0xD6;
        public const int JPG_RST7 = 0xD7;

        public const int JPG_SOI = 0xD8;
        public const int JPG_EOI = 0xD9;
        public const int JPG_SOS = 0xDA;
        public const int JPG_DQT = 0xDB;
        public const int JPG_DNL = 0xDC;
        public const int JPG_DRI = 0xDD;
        public const int JPG_DHP = 0xDE;
        public const int JPG_EXP = 0xDF;

        public const int JPG_APP0 = 0xE0;
        public const int JPG_APP1 = 0xE1;
        public const int JPG_APP2 = 0xE2;
        public const int JPG_APP3 = 0xE3;
        public const int JPG_APP4 = 0xE4;
        public const int JPG_APP5 = 0xE5;
        public const int JPG_APP6 = 0xE6;
        public const int JPG_APP7 = 0xE7;
        public const int JPG_APP8 = 0xE8;
        public const int JPG_APP9 = 0xE9;
        public const int JPG_APP10 = 0xEA;
        public const int JPG_APP11 = 0xEB;
        public const int JPG_APP12 = 0xEC;
        public const int JPG_APP13 = 0xED;
        public const int JPG_APP14 = 0xEE;
        public const int JPG_APP15 = 0xEF;

        public const int JPG_JPG0 = 0xF0;
        public const int JPG_JPG1 = 0xF1;
        public const int JPG_JPG2 = 0xF2;
        public const int JPG_JPG3 = 0xF3;
        public const int JPG_JPG4 = 0xF4;
        public const int JPG_JPG5 = 0xF5;
        public const int JPG_JPG6 = 0xF6;
        public const int JPG_JPG7 = 0xF7;
        public const int JPG_JPG8 = 0xF8;
        public const int JPG_JPG9 = 0xF9;
        public const int JPG_JPG10 = 0xFA;
        public const int JPG_JPG11 = 0xFB;
        public const int JPG_JPG12 = 0xFC;
        public const int JPG_JPG13 = 0xFD;
        public const int JPG_COM = 0xFE;
    }

    public partial class PDJPG
    {
        public const int PDJPG_RGBA = 0;
        public const int PDJPG_RGB = 1;
        public const int PDJPG_BGRA = 3;
        public const int PDJPG_BGR = 4;
        public const int PDJPG_YUVA = 5;
        public const int PDJPG_YUV = 6;
        public const int PDJPG_Y = 7;
        public const int PDJPG_YA = 8;
        public const int PDJPG_YUV422 = 9;	//YYUV
        public const int PDJPG_YUV420 = 10;	//YYU/YYV

        public const int PDJPG_YYYA = 11;

        public const int PDJPG_CLRS_YCBCR = 0;
        public const int PDJPG_CLRS_ORCT = 1;
        public const int PDJPG_CLRS_RGB = 2;
        public const int PDJPG_CLRS_YCCK = 3;

        public const int PDJPG_QFL_RDCT = 256;
        public const int PDJPG_QFL_444 = 512;
        public const int PDJPG_QFL_ORCT = 1024;
        public const int PDJPG_QFL_RGB = 2048;
        public const int PDJPG_QFL_NOSOI = 4096;
        public const int PDJPG_QFL_LAYER_HIDDEN = 8192;
        public const int PDJPG_QFL_MEGABLOCK = 16384;
        public const int PDJPG_QFL_ALTVLC = 32768;
        public const int PDJPG_QFL_ALPHACYAN = 65536;
        public const int PDJPG_QFL_RANGECODER = 131072;
        public const int PDJPG_QFL_AUTODCT = 262144;

        public const int PDJPG_QFL_LOSSLESS = (PDJPG_QFL_RDCT | PDJPG_QFL_444 | PDJPG_QFL_ORCT);

        public const int PDJPG_BCSFL_RDCT = 1;
        public const int PDJPG_BCSFL_LAYER_HIDDEN = 2;
        public const int PDJPG_BCSFL_MEGABLOCK = 4;
        public const int PDJPG_BCSFL_ALTVLC = 8;
        public const int PDJPG_BCSFL_ALPHACYAN = 16;
        public const int PDJPG_BCSFL_RANGECODER = 32;
        public const int PDJPG_BCSFL_AUTODCT = 64;

        public const int PDJPG_CTY_NONE = 0;
        public const int PDJPG_CTY_RGB = 3;
        public const int PDJPG_CTY_RGBA = 4;
        public const int PDJPG_CTY_RGBA_LUMA = 7;
        public const int PDJPG_CTY_RGBA_NORM = 8;
        public const int PDJPG_CTY_RGBA_NVSP = 12;
        public const int PDJPG_CTY_RGBA_NVSPLU = 16;

        public const int PDJPG_DMLT_EOT = 0x00;
        public const int PDJPG_DMLT_SOS = 0x01;
        public const int PDJPG_DMLT_DVFH = 0x02;
        public const int PDJPG_DMLT_SOMV = 0x03;
        public const int PDJPG_DMLT_SOF0 = 0x04;
        public const int PDJPG_DMLT_SOF1 = 0x05;
        public const int PDJPG_DMLT_SOF2 = 0x06;
        public const int PDJPG_DMLT_SOF3 = 0x07;

        public const int PDJPG_DMLT_DHT0 = 0x10;
        public const int PDJPG_DMLT_DHT1 = 0x11;
        public const int PDJPG_DMLT_DHT2 = 0x12;
        public const int PDJPG_DMLT_DHT3 = 0x13;
        public const int PDJPG_DMLT_DHT4 = 0x14;
        public const int PDJPG_DMLT_DHT5 = 0x15;
        public const int PDJPG_DMLT_DHT6 = 0x16;
        public const int PDJPG_DMLT_DHT7 = 0x17;

        public const int PDJPG_DMLT_DQT0 = 0x20;
        public const int PDJPG_DMLT_DQT1 = 0x21;
        public const int PDJPG_DMLT_DQT2 = 0x22;
        public const int PDJPG_DMLT_DQT3 = 0x23;
        public const int PDJPG_DMLT_DQT4 = 0x24;
        public const int PDJPG_DMLT_DQT5 = 0x25;
        public const int PDJPG_DMLT_DQT6 = 0x26;
        public const int PDJPG_DMLT_DQT7 = 0x27;

        public const int DCTSZ = 8;
        public const int DCTSZ2 = 64;
        public const int DCTSZ3 = 512;
        public const int DCTSZ4 = 4096;

        //typedef unsigned char byte;
        //typedef unsigned short ushort;
        //typedef unsigned int uint;


        public static string[] pdjpg_marker = new string[] {
            "SOF0", "SOF1", "SOF2", "SOF3", "DHT", "SOF5", "SOF6", "SOF7",
            "JPG", "SOF9", "SOF10", "SOF11", "DAC", "SOF13", "SOF14", "SOF15",
            "RST0", "RST1", "RST2", "RST3", "RST4", "RST5", "RST6", "RST7",
            "SOI", "EOI", "SOS", "DQT", "DNL", "DRI", "DHP", "EXP",
            "APP0", "APP1", "APP2", "APP3", "APP4", "APP5", "APP6", "APP7", 
            "APP8", "APP9", "APP10", "APP11", "APP12", "APP13", "APP14", "APP15", 
            "JPG0", "JPG1", "JPG2", "JPG3", "JPG4", "JPG5", "JPG6", "JPG7", 
            "JPG8", "JPG9", "JPG10", "JPG11", "JPG12", "JPG13", "COM", ""
        };

        public static int[] pdjpg_zigzag = new int[] {
             0,  1,  5,  6, 14, 15, 27, 28,
             2,  4,  7, 13, 16, 26, 29, 42,
             3,  8, 12, 17, 25, 30, 41, 43,
             9, 11, 18, 24, 31, 40, 44, 53,
            10, 19, 23, 32, 39, 45, 52, 54,
            20, 22, 33, 38, 46, 51, 55, 60,
            21, 34, 37, 47, 50, 56, 59, 61,
            35, 36, 48, 49, 57, 58, 62, 63
        };

        public static int[] pdjpg_zigzag2 = new int[] {
             0,  1,  8, 16,  9,  2,  3, 10,
            17, 24, 32, 25, 18, 11,  4,  5,
            12, 19, 26, 33, 40, 48, 41, 34,
            27, 20, 13,  6,  7, 14, 21, 28,
            35, 42, 49, 56, 57, 50, 43, 36,
            29, 22, 15, 23, 30, 37, 44, 51,
            58, 59, 52, 45, 38, 31, 39, 46,
            53, 60, 61, 54, 47, 55, 62, 63
        };

        public static int[] pdjpg_doublezigzag;
        public static int[] pdjpg_doublezigzag2;

        public static double[] pdjpg_exptab = new double[256];
        public static double[] pdjpg_scltab = new double[256];

        //Common
        static int init = 0;

        public static void printf(string msg)
        {
        }

        public static PDJPG_Context PDJPG_AllocContext()
        {
            PDJPG_Context ctx;

            PDJPG_Init();
            ctx = new PDJPG_Context();

            //ctx.huff = new PDJHUFF(ctx);
            ctx.block = new BTJBlock();


            ctx.jpg_qt = new byte[4 * 256];		//quantization tables
            ctx.jpg_qtfp = new int[4 * 256];	//quantization tables (fixed point)

            ctx.jpg_cid = new byte[16];
            ctx.jpg_ch = new byte[16];
            ctx.jpg_cv = new byte[16];
            ctx.jpg_qid = new byte[16];
            ctx.jpg_cxi = new int[16];
            ctx.jpg_cyi = new int[16];

            ctx.jpg_scid = new int[4];
            ctx.jpg_scn = new int[4];
            ctx.jpg_scbuf = new ShortPtr[4];
            ctx.jpg_sibuf = new ShortPtr[4];
            ctx.jpg_stbuf = new IntPtr[4];

            ctx.cstb = new int[4];

            ctx.jpg_schs = new int[4];		//scan horizontal size
            ctx.jpg_scvs = new int[4];		//scan vertical size

            ctx.jpg_schsci = new int[4];		//scan horizontal scale
            ctx.jpg_scvsci = new int[4];		//scan vertical scale

            ctx.dcs = new int[256];
            ctx.acs = new int[256];
            ctx.dcsuv = new int[256];
            ctx.acsuv = new int[256];

            return (ctx);
        }

        public static void PDJPG_Init()
        {
            int i, j;
            double f, g;

            if (init != 0) return;
            init = 1;

            pdjpg_doublezigzag = new int[4096];
            pdjpg_doublezigzag2 = new int[4096];
            for (i = 0; i < 64; i++)
            {
                for (j = 0; j < 64; j++)
                {
                    pdjpg_doublezigzag[i * 64 + j] =
                        pdjpg_zigzag[i] * 64 + pdjpg_zigzag[j];
                    pdjpg_doublezigzag2[i * 64 + j] =
                        pdjpg_zigzag2[j] * 64 + pdjpg_zigzag2[i];
                    //pdjpg_doublezigzag2[i * 64 + j] =
                        //pdjpg_zigzag2[i] * 64 + pdjpg_zigzag2[j];
                }
            }

            for (i = 0; i < 64; i++)
            {
                f = (i * i) / 240.0;
                g = Math.Pow(2.0, f);
                //		printf("%d %f %f %f\n", i, f, g, 1/g);

                pdjpg_exptab[192 + i] = f;
                pdjpg_exptab[192 - i] = -f;
                pdjpg_exptab[63 + i] = -f;
                pdjpg_exptab[63 - i] = f;

                pdjpg_scltab[192 + i] = g;
                pdjpg_scltab[192 - i] = 1.0 / g;
                pdjpg_scltab[63 + i] = -1.0 / g;
                pdjpg_scltab[63 - i] = -g;
            }

            for (i = 0; i < 18; i++)
            {
                j = (int)(Math.Sqrt(i * 240.0) + 0.5);
                //		printf("%d %d %d %d %d %d\n", i, j,
                //			192+j, 192-j, 63-j, 63+j);

                if (j > 63) continue;

                g = Math.Pow(2.0, i);

                pdjpg_exptab[192 + j] = i;
                pdjpg_exptab[192 - j] = -i;
                pdjpg_exptab[63 + j] = -i;
                pdjpg_exptab[63 - j] = i;

                pdjpg_scltab[192 + j] = g;
                pdjpg_scltab[192 - j] = 1.0 / g;
                pdjpg_scltab[63 + j] = -1.0 / g;
                pdjpg_scltab[63 - j] = -g;
            }

            //	exptab[128]=0;

        }

        public static void PDJPG_SetupQuantTabDivFP(PDJPG_Context ctx, int qid)
        {
            int i;

            if (ctx.megablock)
            {
                for (i = 0; i < 128; i++)
                    ctx.jpg_qtfp[qid * 256 + i] = (int)(4096.0 / ctx.jpg_qt[qid * 256 + i]);
            }
            else
            {
                for (i = 0; i < 64; i++)
                    ctx.jpg_qtfp[qid * 64 + i] = (int)(4096.0 / ctx.jpg_qt[qid * 64 + i]);
            }
        }

    }
}
