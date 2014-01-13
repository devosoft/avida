using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Forms;

namespace avida_ed_csharp
{
    public partial class AvidaEdMainWindow : Form
    {
        private AvidaEDPopViewStatView PopStatView;
        public AvidaEdMainWindow()
        {
            InitializeComponent();
            PopStatView = new AvidaEDPopViewStatView();
            splitContainer1.Panel2.Controls.Add(PopStatView);
            PopStatView.Anchor = ((System.Windows.Forms.AnchorStyles)((((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Bottom)
            | System.Windows.Forms.AnchorStyles.Left)
            | System.Windows.Forms.AnchorStyles.Right)));
            PopStatView.Dock = System.Windows.Forms.DockStyle.Fill;
        }

        private void button1_Click(object sender, EventArgs e)
        {

        }
    }
}
