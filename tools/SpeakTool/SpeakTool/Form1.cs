using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Windows.Forms;
using System.Speech.Synthesis;
using System.Collections.ObjectModel;

namespace SpeakTool
{
    public partial class Form1 : Form
    {
        SpeechSynthesizer runSynth;
        string synthVoice;

        public Form1()
        {
            SpeechSynthesizer synth = new SpeechSynthesizer();
            ReadOnlyCollection<InstalledVoice> voices;
            InstalledVoice vinf;
            string str;
            int i;

            InitializeComponent();

            str = "";
            voices = synth.GetInstalledVoices();
            for (i = 0; i < voices.Count; i++)
            {
                vinf = voices[i];
                str = vinf.VoiceInfo.Name;
                comboBox1.Items.Add(str);
            }

            //synth.SpeakAsync(str);
        }

        private void btnSpeak_Click(object sender, EventArgs e)
        {
            SpeechSynthesizer synth = new SpeechSynthesizer();
            //string str;
            //int i;

            if (synthVoice != null)
                synth.SelectVoice(synthVoice);

            synth.SpeakAsync(textBox1.Text);
            runSynth = synth;

            //synth.SelectVoice("Sam");
            //VoiceInfo vinf = new VoiceInfo();
            //vinf.
        }

        private void textBox1_TextChanged(object sender, EventArgs e)
        {
        }

        private void button1_Click(object sender, EventArgs e)
        {
            saveFileDialog1.ShowDialog();
        }

        private void saveFileDialog1_FileOk(object sender, CancelEventArgs e)
        {
            textBox2.Text = saveFileDialog1.FileName;
        }

        private void textBox2_TextChanged(object sender, EventArgs e)
        {

        }

        private void button2_Click(object sender, EventArgs e)
        {
            openFileDialog1.ShowDialog();
        }

        private void openFileDialog1_FileOk(object sender, CancelEventArgs e)
        {
            textBox3.Text = openFileDialog1.FileName;
        }

        private void button3_Click(object sender, EventArgs e)
        {
            SpeechSynthesizer synth = new SpeechSynthesizer();
            synth.SetOutputToWaveFile(textBox2.Text);
            synth.Speak(textBox1.Text);
            synth.SetOutputToDefaultAudioDevice();
            MessageBox.Show("done");
        }

        private void btnStopSpeech_Click(object sender, EventArgs e)
        {
            runSynth.Pause();
        }

        private void comboBox1_SelectedIndexChanged(object sender, EventArgs e)
        {
            string str;
            str = (string)comboBox1.Items[comboBox1.SelectedIndex];
            synthVoice = str;
        }
    }
}
