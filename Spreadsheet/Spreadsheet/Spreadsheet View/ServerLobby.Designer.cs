namespace Spreadsheet_Lobby
{
    partial class SpreadsheetLobby
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
            this.spreadsheetListBox = new System.Windows.Forms.ListBox();
            this.connectButton = new System.Windows.Forms.Button();
            this.new_spreadsheet_textbox = new System.Windows.Forms.TextBox();
            this.SuspendLayout();
            // 
            // spreadsheetListBox
            // 
            this.spreadsheetListBox.FormattingEnabled = true;
            this.spreadsheetListBox.Location = new System.Drawing.Point(13, 13);
            this.spreadsheetListBox.Name = "spreadsheetListBox";
            this.spreadsheetListBox.Size = new System.Drawing.Size(259, 225);
            this.spreadsheetListBox.TabIndex = 0;
            this.spreadsheetListBox.SelectedIndexChanged += new System.EventHandler(this.spreadsheetListBox_SelectedIndexChanged);
            // 
            // connectButton
            // 
            this.connectButton.Enabled = false;
            this.connectButton.Location = new System.Drawing.Point(12, 270);
            this.connectButton.Name = "connectButton";
            this.connectButton.Size = new System.Drawing.Size(260, 23);
            this.connectButton.TabIndex = 1;
            this.connectButton.Text = "Connect!";
            this.connectButton.UseVisualStyleBackColor = true;
            this.connectButton.Click += new System.EventHandler(this.connectButton_Click);
            // 
            // new_spreadsheet_textbox
            // 
            this.new_spreadsheet_textbox.Location = new System.Drawing.Point(13, 244);
            this.new_spreadsheet_textbox.Name = "new_spreadsheet_textbox";
            this.new_spreadsheet_textbox.Size = new System.Drawing.Size(259, 20);
            this.new_spreadsheet_textbox.TabIndex = 2;
            this.new_spreadsheet_textbox.Text = "New Spreadsheet Name";
            this.new_spreadsheet_textbox.TextChanged += new System.EventHandler(this.new_spreadsheet_textbox_TextChanged);
            // 
            // SpreadsheetLobby
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.ClientSize = new System.Drawing.Size(284, 299);
            this.Controls.Add(this.new_spreadsheet_textbox);
            this.Controls.Add(this.connectButton);
            this.Controls.Add(this.spreadsheetListBox);
            this.FormBorderStyle = System.Windows.Forms.FormBorderStyle.FixedDialog;
            this.Name = "SpreadsheetLobby";
            this.Text = "ServerLobby";
            this.ResumeLayout(false);
            this.PerformLayout();

        }

        #endregion

        private System.Windows.Forms.ListBox spreadsheetListBox;
        private System.Windows.Forms.Button connectButton;
        private System.Windows.Forms.TextBox new_spreadsheet_textbox;
    }
}