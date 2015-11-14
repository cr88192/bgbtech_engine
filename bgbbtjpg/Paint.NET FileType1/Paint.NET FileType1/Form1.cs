using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Windows.Forms;

namespace BGBTechBTJ
{
    public partial class Form1 : Form
    {
        public int quality;
        public bool lossless;
        public bool canceled;

        public static bool useRDCT;
        public static bool use444;
        public static bool useRCT;
        public static bool useRGB;
        public static bool useLossyBase;
        public static bool useMegablock;
        public static bool useAltVLC;
        public static bool useAutoCrop = true;
        public static bool useCropUsePow2 = false;
        public static bool useAlphaCyan = true;
        public static bool useRangeCoder = false;
        public static bool useAutoDCT = false;

        public Form1()
        {
            InitializeComponent();
            SetQuality(90);
            lossless = false;
            canceled = false;
            this.StartPosition = FormStartPosition.CenterScreen;

            checkUseRDCT.Checked = useRDCT;
            checkUse444.Checked = use444;
            checkUseRCT.Checked = useRCT;
            checkUseRGB.Checked = useRGB;
            checkLossyBase.Checked = useLossyBase;
            checkMegablock.Checked = useMegablock;
            checkUseAltVLC.Checked = useAltVLC;
            checkAutoCrop.Checked = useAutoCrop;
            checkCropUsePow2.Checked = useCropUsePow2;
            checkUseAlphaCyan.Checked = useAlphaCyan;
            checkUseRangeCoder.Checked = useRangeCoder;
            checkUseAutoDCT.Checked = useAutoDCT;
        }

        public void SetLossless(bool value)
        {
            checkLossless.Checked=value;
            lossless = value;
        }

        public void SetQuality(int value)
        {
            if (value < 1) value = 1;
            if (value > 100) value = 100;
            quality = value;
            hScrollQuality.Value = value;
            textQuality.Text = value.ToString();
        }

        private void hScrollQuality_Scroll(object sender, ScrollEventArgs e)
        {
            textQuality.Text = hScrollQuality.Value.ToString();
        }

        private void textQuality_TextChanged(object sender, EventArgs e)
        {
            int value;
            bool b;
            b = System.Int32.TryParse(textQuality.Text, out value);
            if (b)
            {
                SetQuality(value);
            }
        }

        private void btnOK_Click(object sender, EventArgs e)
        {
            this.Close();
        }

        private void btnCancel_Click(object sender, EventArgs e)
        {
            canceled = true;
            this.Close();
        }

        private void checkLossless_CheckedChanged(object sender, EventArgs e)
        {
            lossless = checkLossless.Checked;
        }

        private void checkUseRDCT_CheckedChanged(object sender, EventArgs e)
        {
            useRDCT = checkUseRDCT.Checked;
        }

        private void checkUse444_CheckedChanged(object sender, EventArgs e)
        {
            use444 = checkUse444.Checked;
        }

        private void checkUseRCT_CheckedChanged(object sender, EventArgs e)
        {
            useRCT = checkUseRCT.Checked;
        }

        private void checkLossyBase_CheckedChanged(object sender, EventArgs e)
        {
            useLossyBase = checkLossyBase.Checked;
        }

        private void checkUseRGB_CheckedChanged(object sender, EventArgs e)
        {
            useRGB = checkUseRGB.Checked;
        }

        private void checkMegablock_CheckedChanged(object sender, EventArgs e)
        {
            useMegablock = checkMegablock.Checked;
        }

        private void checkUseAltVLC_CheckedChanged(object sender, EventArgs e)
        {
            useAltVLC = checkUseAltVLC.Checked;
        }

        private void checkAutoCrop_CheckedChanged(object sender, EventArgs e)
        {
            useAutoCrop = checkAutoCrop.Checked;
        }

        private void checkCropUsePow2_CheckedChanged(object sender, EventArgs e)
        {
            useCropUsePow2 = checkCropUsePow2.Checked;
        }

        private void checkUseAlphaCyan_CheckedChanged(object sender, EventArgs e)
        {
            useAlphaCyan = checkUseAlphaCyan.Checked;
        }

        private void checkUseRangeCoder_CheckedChanged(object sender, EventArgs e)
        {
            useRangeCoder = checkUseRangeCoder.Checked;
        }

        private void checkUseAutoDCT_CheckedChanged(object sender, EventArgs e)
        {
            useAutoDCT = checkUseAutoDCT.Checked;
        }
    }
}
