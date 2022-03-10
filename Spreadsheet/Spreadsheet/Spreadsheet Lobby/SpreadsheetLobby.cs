using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Forms;

namespace Spreadsheet_Lobby
{
    /// <summary>
    /// This form's purpose is to list all of the incoming spreadsheets for the user.
    /// </summary>
    public partial class SpreadsheetLobby : Form
    {
        List<string> spreadsheetNames;

        public SpreadsheetLobby(string connectAcceptedString)
        {
            InitializeComponent();
            spreadsheetListBox.Items.Add(connectAcceptedString.Replace(@"\3", ""));
        }

        private void connectButton_Click(object sender, EventArgs e)
        {
        }

        private void spreadsheetListBox_SelectedIndexChanged(object sender, EventArgs e)
        {
            if (spreadsheetListBox.SelectedIndex != -1)
                connectButton.Enabled = true;
            else
                connectButton.Enabled = false;
        }
    }
}
