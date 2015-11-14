namespace BGBTechBTJ
{
    partial class Form1
    {
        /// <summary>
        /// Required designer variable.
        /// </summary>
        private System.ComponentModel.IContainer components = null;

        /// <summary>
        /// Clean up any resources being used.
        /// </summary>
        /// <param name="disposing">true if managed resources should be disposed; otherwise, false.</param>
        protected override void Dispose(bool disposing)
        {
            if (disposing && (components != null))
            {
                components.Dispose();
            }
            base.Dispose(disposing);
        }

        #region Windows Form Designer generated code

        /// <summary>
        /// Required method for Designer support - do not modify
        /// the contents of this method with the code editor.
        /// </summary>
        private void InitializeComponent()
        {
            this.hScrollQuality = new System.Windows.Forms.HScrollBar();
            this.textQuality = new System.Windows.Forms.TextBox();
            this.checkLossless = new System.Windows.Forms.CheckBox();
            this.label1 = new System.Windows.Forms.Label();
            this.btnOK = new System.Windows.Forms.Button();
            this.btnCancel = new System.Windows.Forms.Button();
            this.checkLossyBase = new System.Windows.Forms.CheckBox();
            this.checkUse444 = new System.Windows.Forms.CheckBox();
            this.checkUseRDCT = new System.Windows.Forms.CheckBox();
            this.checkUseRCT = new System.Windows.Forms.CheckBox();
            this.checkUseRGB = new System.Windows.Forms.CheckBox();
            this.checkMegablock = new System.Windows.Forms.CheckBox();
            this.checkUseAltVLC = new System.Windows.Forms.CheckBox();
            this.label2 = new System.Windows.Forms.Label();
            this.checkAutoCrop = new System.Windows.Forms.CheckBox();
            this.checkCropUsePow2 = new System.Windows.Forms.CheckBox();
            this.checkUseAlphaCyan = new System.Windows.Forms.CheckBox();
            this.checkUseRangeCoder = new System.Windows.Forms.CheckBox();
            this.checkUseAutoDCT = new System.Windows.Forms.CheckBox();
            this.SuspendLayout();
            // 
            // hScrollQuality
            // 
            this.hScrollQuality.Location = new System.Drawing.Point(12, 377);
            this.hScrollQuality.Name = "hScrollQuality";
            this.hScrollQuality.Size = new System.Drawing.Size(80, 16);
            this.hScrollQuality.TabIndex = 0;
            this.hScrollQuality.Value = 100;
            this.hScrollQuality.Scroll += new System.Windows.Forms.ScrollEventHandler(this.hScrollQuality_Scroll);
            // 
            // textQuality
            // 
            this.textQuality.Location = new System.Drawing.Point(95, 373);
            this.textQuality.Name = "textQuality";
            this.textQuality.Size = new System.Drawing.Size(75, 20);
            this.textQuality.TabIndex = 1;
            this.textQuality.TextChanged += new System.EventHandler(this.textQuality_TextChanged);
            // 
            // checkLossless
            // 
            this.checkLossless.AutoSize = true;
            this.checkLossless.Location = new System.Drawing.Point(31, 12);
            this.checkLossless.Name = "checkLossless";
            this.checkLossless.Size = new System.Drawing.Size(136, 17);
            this.checkLossless.TabIndex = 2;
            this.checkLossless.Text = "Use Lossless Encoding";
            this.checkLossless.UseVisualStyleBackColor = true;
            this.checkLossless.CheckedChanged += new System.EventHandler(this.checkLossless_CheckedChanged);
            // 
            // label1
            // 
            this.label1.AutoSize = true;
            this.label1.Location = new System.Drawing.Point(9, 364);
            this.label1.Name = "label1";
            this.label1.Size = new System.Drawing.Size(71, 13);
            this.label1.TabIndex = 3;
            this.label1.Text = "Quality Level:";
            // 
            // btnOK
            // 
            this.btnOK.Location = new System.Drawing.Point(12, 412);
            this.btnOK.Name = "btnOK";
            this.btnOK.Size = new System.Drawing.Size(75, 23);
            this.btnOK.TabIndex = 4;
            this.btnOK.Text = "OK";
            this.btnOK.UseVisualStyleBackColor = true;
            this.btnOK.Click += new System.EventHandler(this.btnOK_Click);
            // 
            // btnCancel
            // 
            this.btnCancel.Location = new System.Drawing.Point(95, 412);
            this.btnCancel.Name = "btnCancel";
            this.btnCancel.Size = new System.Drawing.Size(75, 23);
            this.btnCancel.TabIndex = 5;
            this.btnCancel.Text = "Cancel";
            this.btnCancel.UseVisualStyleBackColor = true;
            this.btnCancel.Click += new System.EventHandler(this.btnCancel_Click);
            // 
            // checkLossyBase
            // 
            this.checkLossyBase.AutoSize = true;
            this.checkLossyBase.Location = new System.Drawing.Point(31, 36);
            this.checkLossyBase.Name = "checkLossyBase";
            this.checkLossyBase.Size = new System.Drawing.Size(187, 17);
            this.checkLossyBase.TabIndex = 6;
            this.checkLossyBase.Text = "Use Lossy Base Image (if lossless)";
            this.checkLossyBase.UseVisualStyleBackColor = true;
            this.checkLossyBase.CheckedChanged += new System.EventHandler(this.checkLossyBase_CheckedChanged);
            // 
            // checkUse444
            // 
            this.checkUse444.AutoSize = true;
            this.checkUse444.Location = new System.Drawing.Point(31, 83);
            this.checkUse444.Name = "checkUse444";
            this.checkUse444.Size = new System.Drawing.Size(180, 17);
            this.checkUse444.TabIndex = 7;
            this.checkUse444.Text = "Use 4:4:4 (if lossy, default=4:2:0)";
            this.checkUse444.UseVisualStyleBackColor = true;
            this.checkUse444.CheckedChanged += new System.EventHandler(this.checkUse444_CheckedChanged);
            // 
            // checkUseRDCT
            // 
            this.checkUseRDCT.AutoSize = true;
            this.checkUseRDCT.Location = new System.Drawing.Point(31, 60);
            this.checkUseRDCT.Name = "checkUseRDCT";
            this.checkUseRDCT.Size = new System.Drawing.Size(221, 17);
            this.checkUseRDCT.TabIndex = 8;
            this.checkUseRDCT.Text = "Use RDCT (if lossy, replaces use of DCT)";
            this.checkUseRDCT.UseVisualStyleBackColor = true;
            this.checkUseRDCT.CheckedChanged += new System.EventHandler(this.checkUseRDCT_CheckedChanged);
            // 
            // checkUseRCT
            // 
            this.checkUseRCT.AutoSize = true;
            this.checkUseRCT.Location = new System.Drawing.Point(31, 106);
            this.checkUseRCT.Name = "checkUseRCT";
            this.checkUseRCT.Size = new System.Drawing.Size(254, 17);
            this.checkUseRCT.TabIndex = 9;
            this.checkUseRCT.Text = "Use (O)RCT Colorspace (if lossy, default=YCbCr)";
            this.checkUseRCT.UseVisualStyleBackColor = true;
            this.checkUseRCT.CheckedChanged += new System.EventHandler(this.checkUseRCT_CheckedChanged);
            // 
            // checkUseRGB
            // 
            this.checkUseRGB.AutoSize = true;
            this.checkUseRGB.Location = new System.Drawing.Point(31, 130);
            this.checkUseRGB.Name = "checkUseRGB";
            this.checkUseRGB.Size = new System.Drawing.Size(258, 17);
            this.checkUseRGB.TabIndex = 10;
            this.checkUseRGB.Text = "Use RGB Colorspace (overrides RCT and YCbCr)";
            this.checkUseRGB.UseVisualStyleBackColor = true;
            this.checkUseRGB.CheckedChanged += new System.EventHandler(this.checkUseRGB_CheckedChanged);
            // 
            // checkMegablock
            // 
            this.checkMegablock.AutoSize = true;
            this.checkMegablock.Location = new System.Drawing.Point(31, 260);
            this.checkMegablock.Name = "checkMegablock";
            this.checkMegablock.Size = new System.Drawing.Size(137, 17);
            this.checkMegablock.TabIndex = 11;
            this.checkMegablock.Text = "Use Megablock Coding";
            this.checkMegablock.UseVisualStyleBackColor = true;
            this.checkMegablock.CheckedChanged += new System.EventHandler(this.checkMegablock_CheckedChanged);
            // 
            // checkUseAltVLC
            // 
            this.checkUseAltVLC.AutoSize = true;
            this.checkUseAltVLC.Location = new System.Drawing.Point(31, 283);
            this.checkUseAltVLC.Name = "checkUseAltVLC";
            this.checkUseAltVLC.Size = new System.Drawing.Size(113, 17);
            this.checkUseAltVLC.TabIndex = 12;
            this.checkUseAltVLC.Text = "Use Alternate VLC";
            this.checkUseAltVLC.UseVisualStyleBackColor = true;
            this.checkUseAltVLC.CheckedChanged += new System.EventHandler(this.checkUseAltVLC_CheckedChanged);
            // 
            // label2
            // 
            this.label2.AutoSize = true;
            this.label2.Location = new System.Drawing.Point(28, 244);
            this.label2.Name = "label2";
            this.label2.Size = new System.Drawing.Size(207, 13);
            this.label2.TabIndex = 13;
            this.label2.Text = "Experimental / Non-Compatible Extensions";
            // 
            // checkAutoCrop
            // 
            this.checkAutoCrop.AutoSize = true;
            this.checkAutoCrop.Location = new System.Drawing.Point(31, 154);
            this.checkAutoCrop.Name = "checkAutoCrop";
            this.checkAutoCrop.Size = new System.Drawing.Size(136, 17);
            this.checkAutoCrop.TabIndex = 14;
            this.checkAutoCrop.Text = "Auto Crop Visible Pixels";
            this.checkAutoCrop.UseVisualStyleBackColor = true;
            this.checkAutoCrop.CheckedChanged += new System.EventHandler(this.checkAutoCrop_CheckedChanged);
            // 
            // checkCropUsePow2
            // 
            this.checkCropUsePow2.AutoSize = true;
            this.checkCropUsePow2.Location = new System.Drawing.Point(31, 178);
            this.checkCropUsePow2.Name = "checkCropUsePow2";
            this.checkCropUsePow2.Size = new System.Drawing.Size(220, 17);
            this.checkCropUsePow2.TabIndex = 15;
            this.checkCropUsePow2.Text = "(AutoCrop) Try For Power-Of-2 Layer Size";
            this.checkCropUsePow2.UseVisualStyleBackColor = true;
            this.checkCropUsePow2.CheckedChanged += new System.EventHandler(this.checkCropUsePow2_CheckedChanged);
            // 
            // checkUseAlphaCyan
            // 
            this.checkUseAlphaCyan.AutoSize = true;
            this.checkUseAlphaCyan.Location = new System.Drawing.Point(31, 202);
            this.checkUseAlphaCyan.Name = "checkUseAlphaCyan";
            this.checkUseAlphaCyan.Size = new System.Drawing.Size(198, 17);
            this.checkUseAlphaCyan.TabIndex = 16;
            this.checkUseAlphaCyan.Text = "Use Cyan Color Blending With Alpha";
            this.checkUseAlphaCyan.UseVisualStyleBackColor = true;
            this.checkUseAlphaCyan.CheckedChanged += new System.EventHandler(this.checkUseAlphaCyan_CheckedChanged);
            // 
            // checkUseRangeCoder
            // 
            this.checkUseRangeCoder.AutoSize = true;
            this.checkUseRangeCoder.Location = new System.Drawing.Point(31, 307);
            this.checkUseRangeCoder.Name = "checkUseRangeCoder";
            this.checkUseRangeCoder.Size = new System.Drawing.Size(116, 17);
            this.checkUseRangeCoder.TabIndex = 17;
            this.checkUseRangeCoder.Text = "Use Range Coding";
            this.checkUseRangeCoder.UseVisualStyleBackColor = true;
            this.checkUseRangeCoder.CheckedChanged += new System.EventHandler(this.checkUseRangeCoder_CheckedChanged);
            // 
            // checkUseAutoDCT
            // 
            this.checkUseAutoDCT.AutoSize = true;
            this.checkUseAutoDCT.Location = new System.Drawing.Point(31, 331);
            this.checkUseAutoDCT.Name = "checkUseAutoDCT";
            this.checkUseAutoDCT.Size = new System.Drawing.Size(228, 17);
            this.checkUseAutoDCT.TabIndex = 18;
            this.checkUseAutoDCT.Text = "Use Alternate Block Transforms (AutoDCT)";
            this.checkUseAutoDCT.UseVisualStyleBackColor = true;
            this.checkUseAutoDCT.CheckedChanged += new System.EventHandler(this.checkUseAutoDCT_CheckedChanged);
            // 
            // Form1
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.ClientSize = new System.Drawing.Size(351, 447);
            this.Controls.Add(this.checkUseAutoDCT);
            this.Controls.Add(this.checkUseRangeCoder);
            this.Controls.Add(this.checkUseAlphaCyan);
            this.Controls.Add(this.checkCropUsePow2);
            this.Controls.Add(this.checkAutoCrop);
            this.Controls.Add(this.label2);
            this.Controls.Add(this.checkUseAltVLC);
            this.Controls.Add(this.checkMegablock);
            this.Controls.Add(this.checkUseRGB);
            this.Controls.Add(this.checkUseRCT);
            this.Controls.Add(this.checkUseRDCT);
            this.Controls.Add(this.checkUse444);
            this.Controls.Add(this.checkLossyBase);
            this.Controls.Add(this.btnCancel);
            this.Controls.Add(this.btnOK);
            this.Controls.Add(this.label1);
            this.Controls.Add(this.checkLossless);
            this.Controls.Add(this.textQuality);
            this.Controls.Add(this.hScrollQuality);
            this.Name = "Form1";
            this.Text = "Form1";
            this.ResumeLayout(false);
            this.PerformLayout();

        }

        #endregion

        private System.Windows.Forms.HScrollBar hScrollQuality;
        private System.Windows.Forms.TextBox textQuality;
        private System.Windows.Forms.CheckBox checkLossless;
        private System.Windows.Forms.Label label1;
        private System.Windows.Forms.Button btnOK;
        private System.Windows.Forms.Button btnCancel;
        private System.Windows.Forms.CheckBox checkLossyBase;
        private System.Windows.Forms.CheckBox checkUse444;
        private System.Windows.Forms.CheckBox checkUseRDCT;
        private System.Windows.Forms.CheckBox checkUseRCT;
        private System.Windows.Forms.CheckBox checkUseRGB;
        private System.Windows.Forms.CheckBox checkMegablock;
        private System.Windows.Forms.CheckBox checkUseAltVLC;
        private System.Windows.Forms.Label label2;
        private System.Windows.Forms.CheckBox checkAutoCrop;
        private System.Windows.Forms.CheckBox checkCropUsePow2;
        private System.Windows.Forms.CheckBox checkUseAlphaCyan;
        private System.Windows.Forms.CheckBox checkUseRangeCoder;
        private System.Windows.Forms.CheckBox checkUseAutoDCT;
    }
}