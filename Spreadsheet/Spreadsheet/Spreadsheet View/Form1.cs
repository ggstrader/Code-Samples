using SpreadsheetTemplate;
using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Threading;
using System.Threading.Tasks;
using SpreadsheetUtilities;
using System.Windows.Forms;
using NetworkController;
using Spreadsheet_Lobby;
using System.Diagnostics;
using System;
using System.Net;
using System.Net.Sockets;
using System.Text;

namespace SpreadsheetGUI
{
    public partial class Form1 : Form
    {
        private AbstractSpreadsheet spreadsheet;
        private System.Timers.Timer ping_timer;
        private int usedRow;
        private int usedColumn;
        private bool greater;
        private bool less;
        private int conditionalNumber;
        private SocketState theServer;
        //The key value pair is <client_id, cell_name>
        private Dictionary<string, string> focused_cells;

        public Form1()
        {
            spreadsheet = new Spreadsheet(s => true, s => s.ToUpper(), "ps6");
            InitializeComponent();
            this.spreadsheetPanel1.SelectionChanged += OnCellClicked;
            this.AcceptButton = EnterButton;
            greater = false;
            less = false;
            conditionalNumber = 0;
        }

        private void spreadsheetPanel1_Load(object sender, EventArgs e)
        {
            focused_cells = new Dictionary<string, string>();
            ContentsBox.Focus();
            usedColumn = 0;
            usedRow = 0;
        }

        private void openFileDialog_FileOk(object sender, CancelEventArgs e)
        {

            spreadsheetPanel1.Clear();
            ContentsBox.Clear();
            spreadsheet = new Spreadsheet(openFileDialog.FileName, s => true, s => s, "ps6");
            foreach (string name in spreadsheet.GetNamesOfAllNonemptyCells())
            {
                int row;
                int col;
                NameToCoords(name, out row, out col);
                object val = spreadsheet.GetCellValue(name);
                spreadsheetPanel1.SetValue(col, row, ValueToString(val));
            }
        }

        private void saveFileDialog_FileOk(object sender, CancelEventArgs e)
        {
            //spreadsheet.Save(saveFileDialog.FileName);
        }

        private void ChangeCellValue(string cell_name, string cell_contents)
        {
            IEnumerable<string> listOfNames;
            try
            {
                listOfNames = spreadsheet.SetContentsOfCell(cell_name, cell_contents);
            }
            catch
            {
                listOfNames = spreadsheet.SetContentsOfCell(cell_name, "CIRCULAR ERROR");
            }
            foreach (string name in listOfNames)
            {
                int row, col;
                NameToCoords(name, out row, out col);
                spreadsheetPanel1.SetValue(col, row, ValueToString(spreadsheet.GetCellValue(name)));
            }
        }

        private void backgroundWorkerSetContents_DoWork(object sender, DoWorkEventArgs e)
        {
            // Get the cell selection and contents of the text box
            int row, col;
            spreadsheetPanel1.GetSelection(out col, out row);
            string oldVal = ContentsBox.Text;
            IEnumerable<string> listOfNames;
            string cell_id = CoordsToName(row, col);
            try
            {
                listOfNames = spreadsheet.SetContentsOfCell(cell_id, oldVal);
            }
            catch
            {
                listOfNames = spreadsheet.SetContentsOfCell(cell_id, "CIRCULAR ERROR");
            }
            foreach (string name in listOfNames)
            {
                NameToCoords(name, out row, out col);
                spreadsheetPanel1.SetValue(col, row, ValueToString(spreadsheet.GetCellValue(name)));
            }
        }
        //private void backgroundWorkerSetContents_DoWork(object sender, DoWorkEventArgs e)
        //{
        //    string oldVal = ContentsBox.Text;
        //    string name = CoordsToName(usedRow, usedColumn);

        //    foreach (string cell in spreadsheet.SetContentsOfCell(name, oldVal))
        //    {
        //        NameToCoords(name, out usedRow, out usedColumn);
        //        spreadsheetPanel1.SetValue(usedColumn, usedRow, ValueToString(spreadsheet.GetCellValue(name)));
        //    }
        //    spreadsheetPanel1.SetValue(usedColumn, usedRow, oldVal);
        //}

        public void OnCellClicked(SpreadsheetPanel p)
        {
            int row;
            int col;
            spreadsheetPanel1.GetSelection(out col, out row);
            SelectCell(row, col);
        }

        private void SelectCell(int row, int col)
        {
            string name = CoordsToName(row, col);
            object value = spreadsheet.GetCellValue(name);
            object contents = spreadsheet.GetCellContents(name);
            ContentsBox.Left = 100;
            ContentsBox.Top = 100;

            int DATA_COL_WIDTH = 80;
            int DATA_ROW_HEIGHT = 20;
            int LABEL_COL_WIDTH = 30;
            int LABEL_ROW_HEIGHT = 30;
            int _firstCol;
            int _firstRow;
            spreadsheetPanel1.GetFirstCoords(out _firstCol, out _firstRow);

            ContentsBox.Left = LABEL_COL_WIDTH + (col - _firstCol) * DATA_COL_WIDTH + 1;// + 9;
            ContentsBox.Top = /*4 * */ LABEL_ROW_HEIGHT + (row + _firstRow) * DATA_ROW_HEIGHT + 40;// + 13;
            ContentsBox.Width = DATA_COL_WIDTH - 1;
            ContentsBox.Height = DATA_ROW_HEIGHT - 2;
            ContentsBox.Text = contents.ToString();
            ContentsBox.Focus();
            CellNameBox.Text = CoordsToName(row, col);
            CellValueBox.Text = ValueToString(value);
            usedRow = row;
            usedColumn = col;
            if (theServer != null)
            {
                Send_Data(theServer.theSocket, "unfocus " + (char)3 + "focus " + CoordsToName(row, col) + (char)3);
                //Networking.Send_Data(theServer.theSocket, "unfocus " + (char)3);
                //Thread.Sleep(10);
                //Networking.Send_Data(theServer.theSocket, "focus " + CoordsToName(row, col) + (char)3);
            }
        }

        private void EnterButton_Click(object sender, EventArgs e)
        {
            if (theServer != null)
            {
                string cell_id = CoordsToName(usedRow, usedColumn);
                string cell_contents = ContentsBox.Text;
                Send_Data(theServer.theSocket, "edit " + cell_id + ":" + cell_contents + (char)3);
            }
            backgroundWorkerSetContents.RunWorkerAsync();
            if (usedRow + 1 < 99)
                SelectCell(usedRow + 1, usedColumn);
        }

        private void OpenButton_Click(object sender, EventArgs e)
        {
            openFileDialog.ShowDialog();
        }

        private void SaveButton_Click(object sender, EventArgs e)
        {
            //saveFileDialog.ShowDialog();
        }

        private void Form1_FormClosing(object sender, FormClosingEventArgs e)
        {
            //if (spreadsheet.ChangedSinceSave)
            //{
            //  DialogResult dialogue = MessageBox.Show("Save changes before exiting?", "Question", MessageBoxButtons.YesNoCancel, MessageBoxIcon.Question);
            //  if (dialogue == DialogResult.No)
            //    return;
            //  if (dialogue == DialogResult.Cancel)
            //  {
            //    e.Cancel = true;
            //    return;
            //  }
            //  SaveButton.PerformClick();
            //}
        }

        #region Helper Methods

        private string ValueToString(object value)
        {
            string result = "error";
            if (value is String)
            {
                result = (string)value;
            }
            else if (value is double)
            {
                result = value.ToString();
            }
            if (value is FormulaError)
                result = "Invalid Formula";
            return result;
        }

        public void NameToCoords(string name, out int row, out int col)
        {
            col = (int)((char)name[0]) - 65;
            row = int.Parse(name.Substring(1)) - 1;
        }

        private string CoordsToName(int row, int col)
        {
            row += 1;
            col += 65;
            return ((char)col).ToString() + row.ToString();
        }

        private void FillSheet()
        {
            int row = 1;
            int col = 1;

            foreach (var form in FormGen(90 * 20))
            {
                string name = CoordsToName(row - 1, col - 1);
                //Invoke(OnCellClicked(this));
                if (row < 90)
                    row += 1;
                else { row = 1; col += 1; }
            }

        }

        private IEnumerable<string> FormGen(int num)
        {
            Random randy = new Random();
            for (int i = 0; i < num; i++)
            {
                yield return randomFormula(randy);
            }
        }

        private String randomName(Random rand)
        {
            return "ABCDEFGHIJKLMNOPQRSTUVWXYZ".Substring(rand.Next(26), 1) + (rand.Next(99) + 1);
        }

        /// <summary>
        /// Generates a random Formula
        /// </summary>
        /// <param name="rand"></param>
        /// <returns></returns>
        private String randomFormula(Random rand)
        {
            String f = randomName(rand);
            for (int i = 0; i < 10; i++)
            {
                switch (rand.Next(4))
                {
                    case 0:
                        f += "+";
                        break;
                    case 1:
                        f += "-";
                        break;
                    case 2:
                        f += "*";
                        break;
                    case 3:
                        f += "/";
                        break;
                }
                switch (rand.Next(2))
                {
                    case 0:
                        f += 7.2;
                        break;
                    case 1:
                        f += randomName(rand);
                        break;
                }
            }
            return f;
        }
        #endregion

        private void FillSheetButton_Click(object sender, EventArgs e)
        {
            FillSheet();
        }

        private void ContentsBox_LeaveBox(object sender, EventArgs e)
        {

        }
        private void NewButton_Click(object sender, EventArgs e)
        {
            SpreadsheetApplicationContext.getAppContext().RunForm(new Form1());
        }

        private void LessThen_CheckedChanged(object sender, EventArgs e)
        {

        }

        private void GreaterThenCheck_CheckedChanged(object sender, EventArgs e)
        {

        }

        private void ConditionalButton_Click(object sender, EventArgs e)
        {

        }

        private void Help_Button_Click(object sender, EventArgs e)
        {
            DialogResult dialogue = MessageBox.Show(
                "This spreadsheet mimics the behavior of google docs. " +
                "First begin by typing in the public IP address of the server that you would like to connect to. " +
                "Once you are connected you will be provided with a list of available spreadsheets or the option" +
                " to create your own \n\n" +
                "Once you are connected any changes you make will be propigated to all other clients using " +
                "the same spreadsheet as you and the spreadsheets are saved by the server", "Help File", MessageBoxButtons.OK, MessageBoxIcon.Information); ;
        }

        private void connectButton_Click(object sender, EventArgs e)
        {
            if (ipTextBox.Text == "")
            {
                MessageBox.Show("Please enter a valid address");
                return;
            }
            //this.Enabled = false;
            Networking.Connect_To_Server(InitialClientHandshake, ipTextBox.Text);
        }

        private void InitialClientHandshake(SocketState state)
        {
            Send_Data(state.theSocket, "register " + (char)3);
            state.messageProcessor = FinishingClientHandshake;
            Networking.Get_Data(state);
        }

        private void FinishingClientHandshake(SocketState state)
        {
            theServer = new SocketState(state.theSocket, state.sb.ToString().First());
            state.messageProcessor = UpdateCameFromServer;
            Networking.Get_Data(state);

            SpreadsheetLobby lobby = new SpreadsheetLobby(state, UpdateCameFromServer);
            lobby.ShowDialog();
            ping_timer = new System.Timers.Timer(10000);
            ping_timer.Elapsed += NoPingResponse;
        }

        private void NoPingResponse(Object source, System.Timers.ElapsedEventArgs e)
        {
            DisconnectFromServer();
        }
        private void DisconnectFromServer()
        {
            //TODO: Discnnect
            MessageBox.Show("The server has disconnected.");
        }

        private void UpdateCameFromServer(SocketState state)
        {
            char deliminator = Convert.ToChar(3);
            string[] messages = state.sb.ToString().Split(deliminator);
            for (int index = 0; index < messages.Count(); index++)
            {
                string message = messages[index];
                Debug.WriteLine(message);
                if (message.Count() > 0)
                    ParseServerMessage(message);
            }
            //string message = state.sb.ToString().Replace(deliminator.ToString(), "");
            theServer.sb.Clear();
            Networking.Get_Data(state);
        }

        private void ParseServerMessage(string message)
        {
            this.Enabled = true;

            string[] delimiterChars = { " "};
            string[] words = message.Split(delimiterChars, System.StringSplitOptions.RemoveEmptyEntries);

            if (words[0] == "connect_accepted")
            {
                MessageBox.Show("Connection accepted");
            }

            if (words[0] == "file_load_error")
            {
                MessageBox.Show("file load error");
            }
            if (words[0] == "disconnect")
            {
                DisconnectFromServer();
            }
            if (words[0] == "ping")
            {
                // Ping logic here
                Send_Data(theServer.theSocket, "ping_response \u0003");
            }
            if (words[0] == "ping_response")
            {
                // Ping response logic here
                Send_Data(theServer.theSocket, "ping \u0003");
                ping_timer = new System.Timers.Timer(10000);
                ping_timer.Elapsed += NoPingResponse;
            }

            if (words[0] == "full_state")
            {
            spreadsheet = new Spreadsheet(s => true, s => s.ToUpper(), "ps6");
            focused_cells = new Dictionary<string, string>();
                string[] stateDelimiter = { "\n" };
                if (words.Length > 1)
                {
                    string[] cell_states = words[1].Split(stateDelimiter, System.StringSplitOptions.RemoveEmptyEntries);
                    for (int j = 0; j < cell_states.Length; j++)
                    {
                        string cell_info = cell_states[j];
                        int colon_loc = cell_info.IndexOf(":");
                        string cell_id = cell_info.Substring(0, colon_loc);
                        string cell_contents = cell_info.Substring(colon_loc + 1);
                        int i = 0;
                        ChangeCellValue(cell_id, cell_contents);
                    }
                }
                else
                {
                    // empty spreadsheet
                }
            }

            if (words[0] == "change")
            {
                string cell_id = "";
                int i = 0;
                string temp = words[1];

                while (temp[i] != ':')
                {
                    cell_id += temp[i];
                    i++;
                }
                string cell_contents = temp.Split(':')[1];
                ChangeCellValue(cell_id, cell_contents);
            }

            if (words[0] == "focus")
            {
                string cell_id = "";
                int i = 0;
                string temp = words[1];

                while (temp[i] != ':')
                {
                    cell_id += temp[i];
                    i++;
                }
                string client_id = temp.Split(':')[1];
                if (!focused_cells.ContainsKey(client_id))
                {
                    focused_cells.Add(client_id, cell_id);
                    int row;
                    int col;
                    NameToCoords(cell_id, out row, out col);
                }
                    spreadsheetPanel1.SetColor(focused_cells);
            }

            if (words[0] == "unfocus")
            {
                string user_id = words[1];
                if (focused_cells.ContainsKey(user_id))
                {
                    string cell_id = focused_cells[user_id];
                    focused_cells.Remove(user_id);
                    int row;
                    int col;
                    NameToCoords(cell_id, out row, out col);
                }
                    spreadsheetPanel1.SetColor(focused_cells);
            }
        }
        private void Send_Data(Socket socket, String data)
        {
            bool connected = Networking.Send_Data(socket, data);
            if (!connected)
            {
                MessageBox.Show("Server has disconnected.");
            }
        }

        private void UndoButton_Click(object sender, EventArgs e)
        {
            if (theServer != null)
            {
                Send_Data(theServer.theSocket, "undo " + (Char)3);
            }
        }

        private void RevertButton_Click(object sender, EventArgs e)
        {
            if (theServer != null)
            {
                int row = (usedRow + 1);
                char column = (Char)(usedColumn + 65);
                Send_Data(theServer.theSocket, "revert " + column + row + (Char)3);
            }
        }

        private void test_button_Click(object sender, EventArgs e)
        {
            ParseServerMessage("full_state A6:3\nA9:=A6/2\n ");
            ParseServerMessage("focus A1:user_4 ");
            ParseServerMessage("unfocus user_4 ");
            ParseServerMessage("change A2:4 ");
            ParseServerMessage("change A3:5 ");
            ParseServerMessage("change A4:=A3 ");
            ParseServerMessage("change A3:=A4 ");
        }
    }
}
