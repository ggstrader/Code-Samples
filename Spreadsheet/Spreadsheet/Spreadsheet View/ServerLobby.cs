using NetworkController;
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
    SocketState state;
    NetworkAction cb;

    public SpreadsheetLobby(SocketState _state, NetworkAction callback)
    {
      InitializeComponent();
      state = _state;


      // TODO: Parse this correctly for multiple files
      char deliminator = Convert.ToChar(3);
      string names = state.sb.ToString().Replace(deliminator.ToString(), "");
      names = names.Substring(17);
      spreadsheetNames = new List<string>(names.Split('\n'));
      for (int i = 0; i < spreadsheetNames.Count; i++)
      {
//      if (spreadsheetNames[i].Substring(0, 16) != "connect_accepted")
//      {
          spreadsheetListBox.Items.Add(spreadsheetNames[i]);
      state.sb.Clear();
//      }
//      else
//      {
//          spreadsheetListBox.Items.Add(spreadsheetNames[i].Substring(17));
//      }
      }
    }

    private void connectButton_Click(object sender, EventArgs e)
    {
            string message = "load {0}\u0003";
      //if (spreadsheetListBox.SelectedIndex == -1)
      //{
      message = string.Format(message, new_spreadsheet_textbox.Text);
                Networking.Send_Data(state.theSocket, message);
            //}
            //else
            //{
            //    Networking.Send_Data(state.theSocket, string.Format(message, spreadsheetListBox.SelectedItem.ToString()));
            //}
      Networking.Get_Data(state);
      this.Close();
    }

    private void spreadsheetListBox_SelectedIndexChanged(object sender, EventArgs e)
    {
            if (spreadsheetListBox.SelectedIndex != -1)
            {
                connectButton.Enabled = true;
                new_spreadsheet_textbox.Text = spreadsheetListBox.SelectedItem.ToString();
            }
            //else
            //    connectButton.Enabled = false;
    }

        private void new_spreadsheet_textbox_TextChanged(object sender, EventArgs e)
        {
            connectButton.Enabled = true;
            spreadsheetListBox.SelectedIndex = -1;
        }
    }
}
