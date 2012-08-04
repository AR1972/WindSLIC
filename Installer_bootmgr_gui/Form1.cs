using System;
using System.Collections.Generic;
using System.Data;
using System.Drawing;
using System.Text;
using System.Windows.Forms;
using System.IO;
using System.Diagnostics;

namespace WindSLIC_bootmgr
{
    public partial class Form1 : Form
    {
        List<Utils.VolumeInfo> volumes;

        public Form1()
        {
            InitializeComponent();
        }

        private void Form1_Load(object sender, EventArgs e)
        {
            Cursor = Cursors.WaitCursor;
            AddSLIC();
            byte[] patch = Utils.Decompress(Properties.Resources.bootmgr);
            byte version = patch[patch.Length - 1];
            decimal ver = Decimal.Round(version / (decimal)1.27) / 100;
            ver += 1;
            string s = ver.ToString();
            if (s.Length == 1)
            {
                s += ".00";
            }
            if (s.Length == 3)
            {
                s += "0";
            }
            this.Text += " - " + s;
            Utils.patchOEMID = patch[patch.Length - 2];
            Utils.BIOSRW = patch[patch.Length - 3];
            Utils.EBDAIgnored = patch[patch.Length - 4];
            patch = null;
            checkBox1.Checked = Utils.patchOEMID == 1 ? true : false;
            checkBox2.Checked = Utils.BIOSRW == 1 ? true : false;
            checkBox3.Checked = Utils.EBDAIgnored == 1 ? true : false;
            volumes = Utils.findBootmgr();
            Cursor = Cursors.Default;
        }

        private void button1_Click(object sender, EventArgs e)
        {
            try
            {
                Cursor = Cursors.WaitCursor;
                update();
                Utils.PatchBootmgr(ref volumes, "SLIC\\" + comboBox1.SelectedItem + "2.1.BIN");
                Cursor = Cursors.Default;
                MessageBox.Show("bootmgr patched install key and certificate then reboot.");
            }
            catch (Exception ex)
            {
                MessageBox.Show(ex.Message);
                Application.Exit();
            }
        }

        private void button2_Click(object sender, EventArgs e)
        {
            try
            {
                Cursor = Cursors.WaitCursor;
                Utils.UnpatchBootmgr(ref volumes);
                Cursor = Cursors.Default;
                MessageBox.Show("bootmgr unpatched");
            }
            catch (Exception ex)
            {
                MessageBox.Show(ex.Message);
                Application.Exit();
            }
        }

        private void AddSLIC()
        {
            try
            {
                if (!Directory.Exists("SLIC"))
                {
                    Directory.CreateDirectory("SLIC");
                }
                string[] SLICs = Directory.GetFiles("SLIC\\", "*.bin", SearchOption.AllDirectories);
                foreach (string SLIC in SLICs)
                {
                    string file = Path.GetFileName(SLIC);
                    this.comboBox1.Items.Add(file.Replace("2.1.BIN", "").Replace("2.1.bin", ""));
                }
                comboBox1.Sorted = true;
                if (comboBox1.Items.Count > 0)
                {
                    comboBox1.SelectedIndex = 0;
                }
                if (comboBox1.Items.Count <= 0)
                {
                    button1.Enabled = false;
                    button2.Enabled = false;
                    comboBox1.Enabled = false;
                    checkBox1.Enabled = false;
                    checkBox2.Enabled = false;
                    checkBox3.Enabled = false;
                }
            }
            catch (Exception ex)
            {
                MessageBox.Show(ex.Message);
                Application.Exit();
            }
        }

        private void update()
        {
            if (checkBox1.Checked == true)
            {
                Utils.patchOEMID = 0x01;
            }
            else
            {
                Utils.patchOEMID = 0x00;
            }

            if (checkBox2.Checked == true)
            {
                Utils.BIOSRW = 0x01;
            }
            else
            {
                Utils.BIOSRW = 0x00;
            }
            if (checkBox3.Checked == true)
            {
                Utils.EBDAIgnored = 0x01;
            }
            else
            {
                Utils.EBDAIgnored = 0x00;
            }
        }

        private void checkBox1_CheckedChanged(object sender, EventArgs e)
        {
            update();
        }

        private void checkBox2_CheckedChanged(object sender, EventArgs e)
        {
            update();
        }

        private void checkBox3_CheckedChanged(object sender, EventArgs e)
        {
            update();
        }
    }
}