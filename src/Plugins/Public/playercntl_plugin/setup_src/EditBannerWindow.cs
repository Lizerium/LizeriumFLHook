/*
 * Author: Nikolay Dvurechensky
 * Site: https://dvurechensky.pro/
 * Gmail: dvurechenskysoft@gmail.com
 * Last Updated: 09 мая 2026 07:54:16
 * Version: 1.0.487
 */

using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Windows.Forms;

namespace PlayerCntlSetup
{
    public partial class EditBannerWindow : Form
    {
        UIDataSet.StandardBannerListRow item;

        public EditBannerWindow(UIDataSet.StandardBannerListRow item)
        {
            InitializeComponent();
            this.item = item;
            richTextBox1.Text = item.BannerText;
        }

        private void buttonOK_Click(object sender, EventArgs e)
        {
            item.BannerText = richTextBox1.Text;
            this.Close();
        }

        private void buttonCancel_Click(object sender, EventArgs e)
        {
            this.Close();
        }
    }
}
