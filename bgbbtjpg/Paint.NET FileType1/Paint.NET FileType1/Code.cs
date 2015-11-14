using System;
using System.Collections.Generic;
using System.Text;
using PaintDotNet;
using PaintDotNet.Data;
using System.IO;
using System.Drawing;
using System.Windows;
using System.Windows.Forms;
using System.Drawing.Imaging;

namespace BGBTechBTJ
{
    public class MyFileType : FileType
    {
        public MyFileType()
            : base("BGBTech JPEG Test",
                FileTypeFlags.SupportsLoading | FileTypeFlags.SupportsSaving | FileTypeFlags.SupportsLayers,
                new String[] { ".btj", ".jpg" })
        {
        }

        protected Bitmap BitmapFromByteBuffer(byte[] img, int xs, int ys)
        {
            Bitmap bmp;
            int cr, cg, cb, ca;
            int i, j, n;
            bmp = new Bitmap(xs, ys);
            n = xs * ys;
            for (i = 0; i < ys; i++)
                for (j = 0; j < xs; j++)
                {
                    cr = img[(i * xs + j) * 4 + 0];
                    cg = img[(i * xs + j) * 4 + 1];
                    cb = img[(i * xs + j) * 4 + 2];
                    ca = img[(i * xs + j) * 4 + 3];
                    //                        ca = 128;
                    bmp.SetPixel(j, ys - i - 1, Color.FromArgb(ca, cr, cg, cb));
                }
            return bmp;
        }

        protected Bitmap BitmapFromByteBufferRGB(byte[] img, int xs, int ys)
        {
            Bitmap bmp;
            int cr, cg, cb, ca;
            int i, j, n;
            bmp = new Bitmap(xs, ys);
            n = xs * ys;
            for (i = 0; i < ys; i++)
                for (j = 0; j < xs; j++)
                {
                    cr = img[(i * xs + j) * 4 + 0];
                    cg = img[(i * xs + j) * 4 + 1];
                    cb = img[(i * xs + j) * 4 + 2];
                    //ca = img[(i * xs + j) * 4 + 3];
                    ca = 255;
                    bmp.SetPixel(j, ys - i - 1, Color.FromArgb(ca, cr, cg, cb));
                }
            return bmp;
        }

        protected Bitmap BitmapFromByteBufferYI(byte[] img, int xs, int ys, int yi)
        {
            Bitmap bmp;
//            int cr, cg, cb, ca;
            int i, j, k, n;
            bmp = new Bitmap(xs, ys);
            n = xs * ys;
            for (i = 0; i < ys; i++)
                for (j = 0; j < xs; j++)
                {
                    k = img[(i * xs + j) * 4 + yi];
                    bmp.SetPixel(j, ys - i - 1, Color.FromArgb(255, k, k, k));
                }
            return bmp;
        }

        protected Bitmap BitmapFromByteBuffer(byte[] img, int xs, int ys,
            int basexs, int baseys, int orgx, int orgy)
        {
            Bitmap bmp;
            int cr, cg, cb, ca;
            int i, j, n;
            bmp = new Bitmap(basexs, baseys);
            n = xs * ys;
            for (i = 0; i < ys; i++)
            {
                for (j = 0; j < xs; j++)
                {
                    cr = img[(i * xs + j) * 4 + 0];
                    cg = img[(i * xs + j) * 4 + 1];
                    cb = img[(i * xs + j) * 4 + 2];
                    ca = img[(i * xs + j) * 4 + 3];
                    //                        ca = 128;
                    bmp.SetPixel(orgx + j, baseys - (orgy + i) - 1, Color.FromArgb(ca, cr, cg, cb));
                }
            }
            return bmp;
        }

        protected Bitmap BitmapFromByteBufferRGB(byte[] img, int xs, int ys,
            int basexs, int baseys, int orgx, int orgy)
        {
            Bitmap bmp;
            int cr, cg, cb, ca;
            int i, j, n;
            bmp = new Bitmap(basexs, baseys);
            n = xs * ys;
            for (i = 0; i < ys; i++)
            {
                for (j = 0; j < xs; j++)
                {
                    cr = img[(i * xs + j) * 4 + 0];
                    cg = img[(i * xs + j) * 4 + 1];
                    cb = img[(i * xs + j) * 4 + 2];
                    //ca = img[(i * xs + j) * 4 + 3];
                    ca = 255;
                    bmp.SetPixel(orgx + j, ys - (orgy + i) - 1, Color.FromArgb(ca, cr, cg, cb));
                }
            }
            return bmp;
        }

        protected void LoadDocumentLayer(Document doc, BytePtr ibuf, int sz, string name)
        {
            BitmapLayer layer;
            Bitmap bmp;
            PDJPG_LayerInfo linf;
            byte[] rgba;
            byte[] norm;
            byte[] spec;
            byte[] luma;
            int xs, ys, ox, oy, ln, fl;
//            int i, j;

            rgba = new byte[4096 * 4096 * 4];
            norm = new byte[4096 * 4096 * 4];
            spec = new byte[4096 * 4096 * 4];
            luma = new byte[4096 * 4096 * 4];

            PDJPG.PDJPG_DecodeComponent(ibuf, sz, rgba, norm, spec, luma, out xs, out ys, out linf);
            fl = linf.flags;

            //PDJPG_DecodeHasComponent()

            ln = doc.Layers.Count;
            //doc.Layers.Insert(ln, null);

            if (true)
            {
                //layer = new BitmapLayer(xs, ys);
                layer = new BitmapLayer(doc.Width, doc.Height);
                layer.Name = name;
                layer.Opacity = linf.alpha;
                layer.Visible = true;

                if((fl&PDJPG.PDJPG_BCSFL_LAYER_HIDDEN)!=0)
                    layer.Visible = false;

                if ((fl & PDJPG.PDJPG_BCSFL_ALPHACYAN) != 0)
                {
                    BlendByteBufferInverseTransparentColor(rgba, xs, ys);
                }

                //bmp = BitmapFromByteBuffer(rgba, xs, ys);
                bmp = BitmapFromByteBuffer(rgba, xs, ys, doc.Width, doc.Height, linf.orgx, linf.orgy);
                layer.Surface.CopyFromGdipBitmap(bmp, false);
                doc.Layers.Insert(ln++, layer);
            }

            if (PDJPG.PDJPG_DecodeHasComponent(ibuf, sz, "XYZ"))
            {
                //layer = new BitmapLayer(xs, ys);
                layer = new BitmapLayer(doc.Width, doc.Height);
                layer.Name = name + "::Normal";
                layer.Opacity = linf.alpha;
                layer.Visible = false;

                //bmp = BitmapFromByteBufferRGB(norm, xs, ys);
                bmp = BitmapFromByteBufferRGB(norm, xs, ys, doc.Width, doc.Height, linf.orgx, linf.orgy);
                layer.Surface.CopyFromGdipBitmap(bmp, false);
                doc.Layers.Insert(ln++, layer);
            }

            if (PDJPG.PDJPG_DecodeHasComponent(ibuf, sz, "SpRGB"))
            {
                //layer = new BitmapLayer(xs, ys);
                layer = new BitmapLayer(doc.Width, doc.Height);
                layer.Name = name + "::Specular";
                layer.Opacity = linf.alpha;
                layer.Visible = false;

                //bmp = BitmapFromByteBufferRGB(spec, xs, ys);
                bmp = BitmapFromByteBufferRGB(spec, xs, ys, doc.Width, doc.Height, linf.orgx, linf.orgy);
                layer.Surface.CopyFromGdipBitmap(bmp, false);
                doc.Layers.Insert(ln++, layer);
            }

            if (PDJPG.PDJPG_DecodeHasComponent(ibuf, sz, "LuRGB"))
            {
                //layer = new BitmapLayer(xs, ys);
                layer = new BitmapLayer(doc.Width, doc.Height);
                layer.Name = name + "::Luma";
                layer.Opacity = linf.alpha;
                layer.Visible = false;

                //bmp = BitmapFromByteBufferRGB(luma, xs, ys);
                bmp = BitmapFromByteBufferRGB(luma, xs, ys, doc.Width, doc.Height, linf.orgx, linf.orgy);
                layer.Surface.CopyFromGdipBitmap(bmp, false);
                doc.Layers.Insert(ln++, layer);
            }

        }

        protected override Document OnLoad(Stream input)
        {
            //try
            //{

#if true
                //                Bitmap b = new Bitmap(500, 500);
//                b.
                Bitmap b;
//                BytePtr bptbuf;
                byte[] temp;
//                BytePtr img;
                BytePtr cs;
//                byte[] img;
                string cln;
                int i, j, n, sz, sz1, xs, ys;
                int cr, cg, cb, ca;

                temp = new byte[4096 * 4096 * 4];

                n = 4096 * 4096;
                for (i = 0; i < n; i++)
                {
                    j = input.ReadByte();
                    if (j < 0) break;
                    temp[i] = (byte)j;
                }
                sz = i;

#if true
                Document doc;
                PDJPG.PDJPG_DummyDecode(new BytePtr(temp), sz, out xs, out ys);
                doc = new Document(xs, ys);
//                LoadDocumentLayer(doc, new BytePtr(temp), sz, "Background");

                cs = PDJPG.PDJPG_DecodeScanNextTagLayer(new BytePtr(temp), sz, out cln);
                if (cs.isNull)
                {
                    LoadDocumentLayer(doc, new BytePtr(temp), sz, "Background");
                }
                else
                {
                    while (!cs.isNull)
                    {
                        sz1 = sz - cs.pos;
                        LoadDocumentLayer(doc, cs, sz1, cln);
                        cs = PDJPG.PDJPG_DecodeScanNextTagLayer(cs, sz1, out cln);
                    }
                }

                return doc;
#endif

#if false
                //                i = c.DecodeImage(temp, sz, ref xs, ref ys);
//                bptbuf = PDJPG.PDJPG_Decode(new BytePtr(temp), sz, out xs, out ys);
//                img = (BytePtr)bptbuf;

                img = new byte[4096 * 4096 * 4];
                PDJPG.PDJPG_DecodeComponent(new BytePtr(temp), sz, img, null, null, null, out xs, out ys);

                b = new Bitmap(xs, ys);
                n = xs * ys;
                for (i = 0; i < ys; i++)
                    for (j = 0; j < xs; j++)
                    {
                        cr = img[(i * xs + j) * 4 + 0];
                        cg = img[(i * xs + j) * 4 + 1];
                        cb = img[(i * xs + j) * 4 + 2];
                        ca = img[(i * xs + j) * 4 + 3];
//                        ca = 128;
                        b.SetPixel(j, i, Color.FromArgb(ca, cr, cg, cb));
                    }

                return Document.FromImage(b);
//                return Document.FromImage(b);
#endif
#endif
#if false
            }
            catch
            {
                MessageBox.Show("Problem Importing File");

                Bitmap b = new Bitmap(500, 500);
                return Document.FromImage(b);
            }
#endif
        }

        protected void BitmapToByteBuffer(Bitmap bmp, out byte[] img, out int xs, out int ys)
        {
            Color clr;
            int i, j;

            xs = bmp.Width;
            ys = bmp.Height;
            img = new byte[xs * ys * 4];

            for (i = 0; i < ys; i++)
            {
                for (j = 0; j < xs; j++)
                {
                    clr = bmp.GetPixel(j, ys - i - 1);
                    img[(i * xs + j) * 4 + 0] = clr.R;
                    img[(i * xs + j) * 4 + 1] = clr.G;
                    img[(i * xs + j) * 4 + 2] = clr.B;
                    img[(i * xs + j) * 4 + 3] = clr.A;

#if false
                    //if ((clr.A == 0) && (clr.R == 0) && (clr.G == 0) && (clr.B == 0))
                    if(clr.A<4)
                    {
                        img[(i * xs + j) * 4 + 0] = (byte)0;
                        img[(i * xs + j) * 4 + 1] = (byte)255;
                        img[(i * xs + j) * 4 + 2] = (byte)255;
                    }
#endif
                }
            }
        }

        protected BitmapLayer FetchDocumentLayer(Document input, string name)
        {
            BitmapLayer layer;
            int i;

            for (i = 0; i < input.Layers.Count; i++)
            {
                layer = (BitmapLayer)input.Layers[i];
                if (layer.Name == name)
                    return layer;
            }
            return null;
        }

        protected bool FetchDocumentLayerAsByteBuffer(Document input, string name,
            out byte[] img, out int xs, out int ys)
        {
            Bitmap bmp;
            BitmapLayer layer;
            //byte[] img;
            //int i, j;

            layer = FetchDocumentLayer(input, name);
            if (layer == null)
            {
                img = null; xs = 0; ys = 0;
                return false;
            }

            bmp = layer.Surface.CreateAliasedBitmap();
            BitmapToByteBuffer(bmp, out img, out xs, out ys);
            return true;
        }

        protected bool FetchDocumentLayerAsByteBuffer(Document input, string name, int layernum,
            out byte[] img, out int xs, out int ys)
        {
            Bitmap bmp;
            BitmapLayer layer;
            //byte[] img;
            //int i, j;

            //layer = FetchDocumentLayer(input, name);
            layer = (BitmapLayer)input.Layers[layernum];

            if (layer == null)
            {
                img = null; xs = 0; ys = 0;
                return false;
            }

            bmp = layer.Surface.CreateAliasedBitmap();
            BitmapToByteBuffer(bmp, out img, out xs, out ys);
            return true;
        }

        protected double SCurve(double a)
        {
            return (a * a * (3 - 2 * a));
        }
        protected double SCurveClamp(double a)
        {
            double b;
            b = SCurve(a);
            b = (b > 0.0) ? ((b < 1.0) ? b : 1.0) : 0.0;
            return b;
        }

        protected double TCurve(double a)
        {
            //return (1 - ((1 - a) * (1 - a)));
            return (2 * a - a * a);
        }
        protected double TCurveClamp(double a)
        {
            double b;
            b = TCurve(a);
            b = (b > 0.0) ? ((b < 1.0) ? b : 1.0) : 0.0;
            return b;
        }

        protected double TCurveClamp2(double a)
        {
            double b;
            b = TCurve(a);
            b = (b > 0.02) ? ((b < 1.0) ? b : 1.0) : 0.02;
            return b;
        }

        protected void BlendByteBufferTransparentColor(byte[] rgba, int xs, int ys)
        {
            double f, g;
            int i, n;

            n = xs * ys;
            for (i = 0; i < n; i++)
            {
                //if (rgba[i * 4 + 3] > 240)
                    //continue;
                if (rgba[i * 4 + 3] >= 255)
                    continue;

                f=rgba[i * 4 + 3]/255.0;
                f = TCurveClamp2(f); g=1.0-f;
                rgba[i * 4 + 0] = (byte)((rgba[i * 4 + 0] * f) + (0 * g));
                rgba[i * 4 + 1] = (byte)((rgba[i * 4 + 1] * f) + (255 * g));
                rgba[i * 4 + 2] = (byte)((rgba[i * 4 + 2] * f) + (255 * g));
            }
        }

        protected void BlendByteBufferInverseTransparentColor(byte[] rgba, int xs, int ys)
        {
            double f, g;
            int i, n;

            n = xs * ys;
            for (i = 0; i < n; i++)
            {
                //if (rgba[i * 4 + 3] > 240)
                //continue;
                if (rgba[i * 4 + 3] >= 255)
                    continue;

                f = rgba[i * 4 + 3] / 255.0;
                f = TCurveClamp2(f); g = 1.0 - f;
                if (f > 0)
                {
                    rgba[i * 4 + 0] = (byte)((rgba[i * 4 + 0] - (0 * g)) * (1.0 / f));
                    rgba[i * 4 + 1] = (byte)((rgba[i * 4 + 1] - (255 * g)) * (1.0 / f));
                    rgba[i * 4 + 2] = (byte)((rgba[i * 4 + 2] - (255 * g)) * (1.0 / f));
                }
                else
                {
                    rgba[i * 4 + 0] = 0;
                    rgba[i * 4 + 1] = 0;
                    rgba[i * 4 + 2] = 0;
                }
            }
        }

        protected bool CalcByteBufferBounds(byte[] rgba, int xs, int ys, ref int mx, ref int my, ref int nx, ref int ny)
        {
            int i, j;

            if (rgba == null)
                return false;

            //mx = xs; my = ys; nx = 0; ny = 0;
            for (i = 0; i < ys; i++)
            {
                for (j = 0; j < xs; j++)
                {
                    if (rgba[(i * xs + j) * 4 + 3] > 0)
                    {
                        if (j < mx) mx = j;
                        if (j >= nx) nx = j+1;
                        if (i < my) my = i;
                        if (i >= ny) ny = i+1;
                    }
                }
            }
            return true;
        }

        protected bool CropByteBufferBounds(byte[] rgba, int xs, int ys, int mx, int my, int nx, int ny)
        {
            int xs1, ys1;
            int i, j;

            if (rgba == null)
                return false;

            xs1=nx-mx; ys1=ny-my;
            for (i = 0; i < ys1; i++)
            {
                for (j = 0; j < xs1; j++)
                {
                    rgba[(i * xs1 + j) * 4 + 0] = rgba[((my + i) * xs + (mx + j)) * 4 + 0];
                    rgba[(i * xs1 + j) * 4 + 1] = rgba[((my + i) * xs + (mx + j)) * 4 + 1];
                    rgba[(i * xs1 + j) * 4 + 2] = rgba[((my + i) * xs + (mx + j)) * 4 + 2];
                    rgba[(i * xs1 + j) * 4 + 3] = rgba[((my + i) * xs + (mx + j)) * 4 + 3];
                }
            }
            return true;
        }

        protected void AdjustCropBounds1D(int xs, ref int mx, ref int nx)
        {
            int cx, xs1;
            int i;

            //try to find power-of-2 box relative to center
            cx = (mx + nx) / 2;
            i = 1;
            while (((cx - i) > mx) && ((cx + i) < nx) &&
                   ((cx - i) > 0) && ((cx + i) < xs))
                i = i << 1;
            if (((cx - i) <= mx)&& ((cx + i) >= nx) &&
                ((cx - i) >= 0) && ((cx + i) <= xs))
            {
                mx = cx - i;
                nx = cx + i;
                return;
            }

            //try to find power-of-2 box relative to corner
            xs1 = nx - mx;
            i = 1;
            while ((i < xs1) && ((mx + i) < xs))
                i = i << 1;

            if ((i >= xs1) && ((mx + i) <= xs))
            {
                nx = mx + i;
                return;
            }

            //try to find power-of-2 box relative to origin
            i = 1;
            while ((i < nx) && (i < xs))
                i = i << 1;

            if ((i >= nx) && (i <= xs))
            {
                mx = 0;
                nx = i;
                return;
            }

            //try to find power-of-2 box relative to side
            i = 1;
            while (((xs-i) > mx) && ((xs-i)>0))
                i = i << 1;

            if (((xs-i) <= mx) && ((xs-i) >= 0))
            {
                mx = xs-i;
                nx = xs;
                return;
            }

            //try to just align
            mx = (mx / 16) * 16;
            nx = ((nx + 15) / 16) * 16;
            if (nx > xs) nx = xs;
        }

        protected bool AdjustCropBounds(int xs, int ys, ref int mx, ref int my, ref int nx, ref int ny)
        {
            int xs1, ys1;
            int cx, cy;
            int i;

            //try to find power-of-2 box relative to center
            cx = (mx + nx) / 2;
            cy = (my + ny) / 2;
            i = 1;
            while (((cx - i) > mx) && ((cy - i) > my) && ((cx + i) < nx) && ((cy + i) < ny) &&
                   ((cx - i) > 0) && ((cy - i) > 0) && ((cx + i) < xs) && ((cy + i) < ys))
                i = i << 1;
            if (((cx - i) <= mx) && ((cy - i) <= my) && ((cx + i) >= nx) && ((cy + i) >= ny) &&
                ((cx - i) >= 0) && ((cy - i) >= 0) && ((cx + i) <= xs) && ((cy + i) <= ys))
            {
                mx = cx - i;
                my = cy - i;
                nx = cx + i;
                ny = cy + i;
                return true;
            }

            //try to find power-of-2 box relative to corner
            xs1 = nx - mx;
            ys1 = ny - my;
            i = 1;
            while ((i < xs1) && (i < ys1) && ((mx + i) < xs) && ((my + i) < ys))
                i = i << 1;

            if ((i >= xs1) && (i >= ys1) && ((mx + i) <= xs) && ((my + i) <= ys))
            {
                nx = mx + i;
                ny = my + i;
                return true;
            }

            AdjustCropBounds1D(xs, ref mx, ref nx);
            AdjustCropBounds1D(ys, ref my, ref ny);
            return false;

#if false
            //at least try to pad the corners to multiples of 16
                mx = (mx / 16) * 16;
                my = (my / 16) * 16;
                nx = ((nx + 15) / 16) * 16;
                ny = ((ny + 15) / 16) * 16;
                if (nx > xs) nx = xs;
                if (ny > ys) ny = ys;
#endif
        }

        public bool Pow2P(int i)
        {
            return ((i & (i >> 1)) == 0);
        }

        public bool Align16P(int i)
        {
            return ((i & 15) == 0);
        }

        public bool BetterP(int x1, int x2)
        {
            if (Pow2P(x1) && Pow2P(x2))
                return (x1 <= x2);
            if (Pow2P(x1))
                return true;
            if (Pow2P(x2))
                return false;
            if (Align16P(x1))
                return true;
            return false;
        }

        protected void AdjustCropBounds2(int xs, int ys, ref int mx, ref int my, ref int nx, ref int ny)
        {
            int mx1, my1, nx1, ny1, xs1, ys1;
            int mx2, my2, nx2, ny2, xs2, ys2;

            mx1 = mx; my1 = my; nx1 = nx; ny1 = ny;
            AdjustCropBounds(xs, ys, ref mx1, ref my1, ref nx1, ref ny1);

            mx2 = (mx / 16) * 16;
            my2 = (my / 16) * 16;
            nx2 = ((nx + 15) / 16) * 16;
            ny2 = ((ny + 15) / 16) * 16;
            if (nx2 > xs) nx2 = xs;
            if (ny2 > ys) ny2 = ys;

            AdjustCropBounds(xs, ys, ref mx2, ref my2, ref nx2, ref ny2);

            xs1 = nx1 - mx1; ys1 = ny1 - my1;
            xs2 = nx2 - mx2; ys2 = ny2 - my2;

            if (BetterP(xs2, xs1) && BetterP(ys2, ys1))
            {
                mx = mx2; my = my2; nx = nx2; ny = ny2;
                return;
            }

            if (BetterP(xs1, xs2) && BetterP(ys1, ys2))
            {
                mx = mx1; my = my1; nx = nx1; ny = ny1;
                return;
            }

            if (BetterP(xs2, xs1)) { mx = mx2; nx = nx2; }
            else { mx = mx1; nx = nx1; }

            if (BetterP(ys2, ys1)) { my = my2; ny = ny2; }
            else { my = my1; ny = ny1; }
        }

        protected BytePtr EmitDocumentTagLayer(Document input, PDJPG_Context ctx, BytePtr ct,
            string name, int layernum, int qf)
        {
            //Bitmap bmp;
            //BitmapLayer layer;
            PDJPG_LayerInfo linf;
            int xs, ys, xs1, ys1, sz;
            byte[] rgba, norm, spec, luma;
            int mx, my, nx, ny;
            int bmx, bmy, bnx, bny;
            int i, j;

            FetchDocumentLayerAsByteBuffer(input, name, layernum, out rgba, out xs, out ys);
            FetchDocumentLayerAsByteBuffer(input, name + "::Normal", out norm, out xs1, out ys1);
            FetchDocumentLayerAsByteBuffer(input, name + "::Specular", out spec, out xs1, out ys1);
            FetchDocumentLayerAsByteBuffer(input, name + "::Luma", out luma, out xs1, out ys1);

            mx = xs; my = ys; nx = 0; ny = 0;
            CalcByteBufferBounds(rgba, xs, ys, ref mx, ref my, ref nx, ref ny);
            CalcByteBufferBounds(norm, xs, ys, ref mx, ref my, ref nx, ref ny);
            CalcByteBufferBounds(spec, xs, ys, ref mx, ref my, ref nx, ref ny);
            CalcByteBufferBounds(luma, xs, ys, ref mx, ref my, ref nx, ref ny);

            //skip empty layers
            if ((mx >= nx) || (my >= ny))
                return ct;

            bmx = mx; bmy = my; bnx = nx; bny = ny;

#if false
            //crop to visible part of image
            mx =(mx/16)*16;
            my =(my/16)*16;
            nx = ((nx+15) / 16) * 16;
            ny = ((ny+15) / 16) * 16;
            if (nx > xs) nx = xs;
            if (ny > ys) ny = ys;
#endif

            if (Form1.useAutoCrop)
            {
                if (Form1.useCropUsePow2)
                {
                    //AdjustCropBounds(xs, ys, ref mx, ref my, ref nx, ref ny);
                    //AdjustCropBounds(xs, ys, ref mx, ref my, ref nx, ref ny);
                    AdjustCropBounds2(xs, ys, ref mx, ref my, ref nx, ref ny);
                }
                else
                {
                    //simply align to multiples of 16
                    mx = (mx / 16) * 16;
                    my = (my / 16) * 16;
                    nx = ((nx + 15) / 16) * 16;
                    ny = ((ny + 15) / 16) * 16;
                    if (nx > xs) nx = xs;
                    if (ny > ys) ny = ys;
                }

                //crop image
                CropByteBufferBounds(rgba, xs, ys, mx, my, nx, ny);
                CropByteBufferBounds(norm, xs, ys, mx, my, nx, ny);
                CropByteBufferBounds(spec, xs, ys, mx, my, nx, ny);
                CropByteBufferBounds(luma, xs, ys, mx, my, nx, ny);
                xs = nx - mx; ys = ny - my;
            }
            else
            {
                mx = 0; my = 0;
                nx = xs; ny = ys;
            }

            //bmp = layer.Surface.CreateAliasedBitmap();
            //BitmapToByteBuffer(bmp, out img, out xs, out ys);

            linf = new PDJPG_LayerInfo();
            linf.orgx = mx;
            linf.orgy = my;
            linf.alpha = 255;
            linf.blend = 0;
            linf.minx = bmx;
            linf.miny = bmy;
            linf.maxx = bnx;
            linf.maxy = bny;

            i = PDJPG.PDJPG_EncodeComponentCtxI2(ctx, rgba, norm, spec, luma, null, ct, xs, ys, qf, name, linf);
            ct += i;
            return ct;
        }

        protected BytePtr EmitDocumentBaseLayer(Document input, PDJPG_Context ctx, BytePtr ct, string name, int qf)
        {
            //Bitmap bmp;
            //BitmapLayer layer;
            PDJPG_LayerInfo linf;
            int xs, ys, xs1, ys1, sz;
            byte[] rgba, norm, spec, luma;
            int mx, my, nx, ny;
            int i, j;

            FetchDocumentLayerAsByteBuffer(input, name, out rgba, out xs, out ys);
            FetchDocumentLayerAsByteBuffer(input, name + "::Normal", out norm, out xs1, out ys1);
            FetchDocumentLayerAsByteBuffer(input, name + "::Specular", out spec, out xs1, out ys1);
            FetchDocumentLayerAsByteBuffer(input, name + "::Luma", out luma, out xs1, out ys1);

            if (Form1.useAlphaCyan)
            {
                BlendByteBufferTransparentColor(rgba, xs, ys);
                qf |= PDJPG.PDJPG_QFL_ALPHACYAN;
            }

            //bmp = layer.Surface.CreateAliasedBitmap();
            //BitmapToByteBuffer(bmp, out img, out xs, out ys);

#if false
            mx = xs; my = ys; nx = 0; ny = 0;
            CalcByteBufferBounds(rgba, xs, ys, ref mx, ref my, ref nx, ref ny);
            CalcByteBufferBounds(norm, xs, ys, ref mx, ref my, ref nx, ref ny);
            CalcByteBufferBounds(spec, xs, ys, ref mx, ref my, ref nx, ref ny);
            CalcByteBufferBounds(luma, xs, ys, ref mx, ref my, ref nx, ref ny);

            //skip empty layers
            if ((mx >= nx) || (my >= ny))
                return ct;

            CropByteBufferBounds(rgba, xs, ys, mx, my, nx, ny);
            CropByteBufferBounds(norm, xs, ys, mx, my, nx, ny);
            CropByteBufferBounds(spec, xs, ys, mx, my, nx, ny);
            CropByteBufferBounds(luma, xs, ys, mx, my, nx, ny);
            xs = nx - mx; ys = ny - ys;
#endif
            mx = 0; my = 0;

            //bmp = layer.Surface.CreateAliasedBitmap();
            //BitmapToByteBuffer(bmp, out img, out xs, out ys);

            linf = new PDJPG_LayerInfo();
            linf.orgx = mx;
            linf.orgy = my;
            linf.alpha = 255;
            linf.blend = 0;


            i = PDJPG.PDJPG_EncodeComponentCtxI2(ctx, rgba, norm, spec, luma, null, ct, xs, ys, qf, null, linf);
            ct += i;
            return ct;
        }

        public static bool lossless=true;
        public static int quality=95;

        protected override void OnSave(Document input, Stream output, SaveConfigToken token,
            Surface scratchSurface, ProgressEventHandler callback)
        {
            RenderArgs ra = new RenderArgs(new Surface(input.Size));
            PDJPG_Context ctx;
            BytePtr obuf, ct;
            Color clr;
            BitmapLayer layer;
            Form1 settings;
            //Bitmap bmp;
            string bln;
            int xs, ys, sz;
            byte[] img;
            int i, j, lc, qf, qf1;
            //bool lossless;

            input.Render(ra, false);

            settings=new Form1();
            settings.Visible = true;
            settings.SetQuality(quality);
            settings.SetLossless(lossless);

            Application.Run(settings);

            quality = settings.quality;
            lossless = settings.lossless;

            if (settings.canceled)
            {
                //better way to do this?...
                throw new Exception();
            }

            xs = ra.Bitmap.Width;
            ys = ra.Bitmap.Height;
            img = new byte[xs * ys * 4];

            for (i = 0; i < ys; i++)
                for (j = 0; j < xs; j++)
                {
                    clr = ra.Bitmap.GetPixel(j, ys - i - 1);
                    img[(i * xs + j) * 4 + 0] = clr.R;
                    img[(i * xs + j) * 4 + 1] = clr.G;
                    img[(i * xs + j) * 4 + 2] = clr.B;
                    img[(i * xs + j) * 4 + 3] = clr.A;
#if false
                    //if (clr.A == 0)
                    if (clr.A < 4)
                    {
                        img[(i * xs + j) * 4 + 0] = (byte)0;
                        img[(i * xs + j) * 4 + 1] = (byte)255;
                        img[(i * xs + j) * 4 + 2] = (byte)255;
                    }
#endif
                }

            //BlendByteBufferTransparentColor(img, xs, ys);
#if false
            for (i = 0; i < ys; i++)
                for (j = 0; j < xs; j++)
                {
                    img[(i * xs + j) * 4 + 0] = (byte)(((i & 3) == 0) ? 255 : 0);
                    img[(i * xs + j) * 4 + 1] = (byte)((((i + j) & 3) == 0) ? 255 : 0);
                    img[(i * xs + j) * 4 + 2] = (byte)(((j & 3) == 0) ? 255 : 0);
                    img[(i * xs + j) * 4 + 3] = 255;
                }
#endif
#if true
            lc = 0; bln = null;
            for (i = 0; i < input.Layers.Count; i++)
            {
                layer = (BitmapLayer)input.Layers[i];
                if (layer.Name.Contains("::"))
                    continue;
                bln = layer.Name;
                lc++;
            }
#endif

            //lossless = true;
            //qf=lossless?(100|PDJPG.PDJPG_QFL_LOSSLESS):95;
            qf=lossless?(100|PDJPG.PDJPG_QFL_LOSSLESS):quality;

            if (Form1.useRDCT)
                qf |= PDJPG.PDJPG_QFL_RDCT;
            if (Form1.use444)
                qf |= PDJPG.PDJPG_QFL_444;
            if (Form1.useRCT)
                qf |= PDJPG.PDJPG_QFL_ORCT;
            if (Form1.useRGB)
                qf |= PDJPG.PDJPG_QFL_RGB;
            if (Form1.useMegablock)
                qf |= PDJPG.PDJPG_QFL_MEGABLOCK;
            if (Form1.useAltVLC)
                qf |= PDJPG.PDJPG_QFL_ALTVLC;
            if (Form1.useRangeCoder)
                qf |= PDJPG.PDJPG_QFL_RANGECODER;
            if (Form1.useAutoDCT)
                qf |= PDJPG.PDJPG_QFL_AUTODCT;

            //obuf = new BytePtr(xs * ys * 4);
            obuf = new BytePtr(1 << 24);
            //            sz = PDJPG.PDJPG_Encode(img, obuf, xs, ys, 90);
//            sz = PDJPG.PDJPG_EncodeComponent(img, null, null, null, obuf, xs, ys, 95);

#if true
            ctx = PDJPG.PDJPG_AllocContext();
            ct = obuf;

            ct = PDJPG.PDJPG_EncodeBeginLayeredCtx(ctx, ct, xs, ys, qf);
            //j = (lc > 1) ? 75 : 95;
            //i = PDJPG.PDJPG_EncodeComponentCtxI2(ctx, img, null, null, null, null, ct, xs, ys, j, null);
            //i = PDJPG.PDJPG_EncodeComponentCtxI2(ctx, img, null, null, null, null, ct, xs, ys, 95, null);
            //ct += i;

#if true
            if (lc > 1)
            {
                //i = PDJPG_EncodeCtx(ctx, img, ct, xs, ys, 75, 0);
                //i = PDJPG.PDJPG_EncodeComponentCtxI2(ctx, img, null, null, null, null, ct, xs, ys, 85, null);

                qf1 = qf;

                if (lossless && Form1.useLossyBase)
                {
                    qf1 = quality;

                    if (Form1.useRDCT)
                        qf1 |= PDJPG.PDJPG_QFL_RDCT;
                    if (Form1.use444)
                        qf1 |= PDJPG.PDJPG_QFL_444;
                    if (Form1.useRCT)
                        qf1 |= PDJPG.PDJPG_QFL_ORCT;
                    if (Form1.useRGB)
                        qf1 |= PDJPG.PDJPG_QFL_RGB;
                }

                if (Form1.useAlphaCyan)
                {
                    BlendByteBufferTransparentColor(img, xs, ys);
                    qf1 |= PDJPG.PDJPG_QFL_ALPHACYAN;
                }

                i = PDJPG.PDJPG_EncodeComponentCtxI2(ctx, img, null, null, null, null, ct, xs, ys, qf1, null, null);
                ct += i;

                for (i = 0; i < input.Layers.Count; i++)
                {
                    layer = (BitmapLayer)input.Layers[i];
                    if (layer.Name.Contains("::"))
                        continue;
                    qf1 = qf;
                    if (!layer.Visible)
                        qf1 |= PDJPG.PDJPG_QFL_LAYER_HIDDEN;
                    ct = EmitDocumentTagLayer(input, ctx, ct, layer.Name, i, qf1);
                }
            }
            else
            {
                ct = EmitDocumentBaseLayer(input, ctx, ct, bln, qf);
                //i = PDJPG.PDJPG_EncodeComponentCtxI2(ctx, img, null, null, null, null, ct, xs, ys, 95, null);
                //ct += i;
            }
#endif

            ct = PDJPG.PDJPG_EncodeEndLayeredCtx(ctx, ct);

            PDJPG.PDJPG_FreeContext(ctx);
            //free(tbuf);
            sz = (ct - obuf);
#endif

            if (sz > 0)
            {
                output.Write(obuf.buf, 0, sz);
            }
            else
            {
                MessageBox.Show("Encode Failed");
            }

//            ra.Bitmap.Save(output, ImageFormat.Bmp);
          
        }
    }

    public class MyFileTypeFactory : IFileTypeFactory
    {
        public FileType[] GetFileTypeInstances()
        {
            return new FileType[] { new MyFileType() };
        }
    }
}
