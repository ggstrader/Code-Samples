namespace SpreadsheetGUI
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
      this.ContentsBox = new System.Windows.Forms.TextBox();
      this.CellNameBox = new System.Windows.Forms.TextBox();
      this.CellValueBox = new System.Windows.Forms.TextBox();
      this.EnterButton = new System.Windows.Forms.Button();
      this.backgroundWorkerSetContents = new System.ComponentModel.BackgroundWorker();
      this.openFileDialog = new System.Windows.Forms.OpenFileDialog();
      this.saveFileDialog = new System.Windows.Forms.SaveFileDialog();
      this.backgroundWorker1 = new System.ComponentModel.BackgroundWorker();
      this.Help_Button = new System.Windows.Forms.Button();
      this.connectButton = new System.Windows.Forms.Button();
      this.ipTextBox = new System.Windows.Forms.TextBox();
      this.label1 = new System.Windows.Forms.Label();
      this.Undo_Button = new System.Windows.Forms.Button();
      this.RevertButton = new System.Windows.Forms.Button();
      this.FillSheetButton = new System.Windows.Forms.Button();
      this.spreadsheetPanel1 = new SpreadsheetTemplate.SpreadsheetPanel();
      this.SuspendLayout();
      // 
      // ContentsBox
      // 
      this.ContentsBox.Location = new System.Drawing.Point(2309, 219);
      this.ContentsBox.Margin = new System.Windows.Forms.Padding(3, 2, 3, 2);
      this.ContentsBox.Name = "ContentsBox";
      this.ContentsBox.Size = new System.Drawing.Size(0, 38);
      this.ContentsBox.TabIndex = 1;
      this.ContentsBox.Leave += new System.EventHandler(this.ContentsBox_LeaveBox);
      // 
      // CellNameBox
      // 
      this.CellNameBox.Location = new System.Drawing.Point(24, 24);
      this.CellNameBox.Margin = new System.Windows.Forms.Padding(3, 2, 3, 2);
      this.CellNameBox.Name = "CellNameBox";
      this.CellNameBox.ReadOnly = true;
      this.CellNameBox.Size = new System.Drawing.Size(191, 38);
      this.CellNameBox.TabIndex = 2;
      // 
      // CellValueBox
      // 
      this.CellValueBox.Location = new System.Drawing.Point(221, 24);
      this.CellValueBox.Margin = new System.Windows.Forms.Padding(3, 2, 3, 2);
      this.CellValueBox.Name = "CellValueBox";
      this.CellValueBox.ReadOnly = true;
      this.CellValueBox.Size = new System.Drawing.Size(236, 38);
      this.CellValueBox.TabIndex = 3;
      // 
      // EnterButton
      // 
      this.EnterButton.Location = new System.Drawing.Point(1957, 15);
      this.EnterButton.Margin = new System.Windows.Forms.Padding(3, 2, 3, 2);
      this.EnterButton.Name = "EnterButton";
      this.EnterButton.Size = new System.Drawing.Size(152, 57);
      this.EnterButton.TabIndex = 4;
      this.EnterButton.Text = "Update";
      this.EnterButton.UseVisualStyleBackColor = true;
      this.EnterButton.Click += new System.EventHandler(this.EnterButton_Click);
      // 
      // backgroundWorkerSetContents
      // 
      this.backgroundWorkerSetContents.DoWork += new System.ComponentModel.DoWorkEventHandler(this.backgroundWorkerSetContents_DoWork);
      // 
      // openFileDialog
      // 
      this.openFileDialog.FileName = "openFileDialog1";
      this.openFileDialog.Filter = "spreadsheet (.sprd) |*.sprd| all files |*.*";
      this.openFileDialog.FileOk += new System.ComponentModel.CancelEventHandler(this.openFileDialog_FileOk);
      // 
      // saveFileDialog
      // 
      this.saveFileDialog.Filter = "spreadsheet (.sprd) |*.sprd| all files |*.*";
      this.saveFileDialog.FileOk += new System.ComponentModel.CancelEventHandler(this.saveFileDialog_FileOk);
      // 
      // Help_Button
      // 
      this.Help_Button.Location = new System.Drawing.Point(2195, 95);
      this.Help_Button.Margin = new System.Windows.Forms.Padding(8, 7, 8, 7);
      this.Help_Button.Name = "Help_Button";
      this.Help_Button.Size = new System.Drawing.Size(213, 55);
      this.Help_Button.TabIndex = 13;
      this.Help_Button.Text = "Help";
      this.Help_Button.UseVisualStyleBackColor = true;
      this.Help_Button.Click += new System.EventHandler(this.Help_Button_Click);
      // 
      // connectButton
      // 
      this.connectButton.Location = new System.Drawing.Point(1203, 15);
      this.connectButton.Margin = new System.Windows.Forms.Padding(8, 7, 8, 7);
      this.connectButton.Name = "connectButton";
      this.connectButton.Size = new System.Drawing.Size(200, 47);
      this.connectButton.TabIndex = 14;
      this.connectButton.Text = "CONNECT";
      this.connectButton.UseVisualStyleBackColor = true;
      this.connectButton.Click += new System.EventHandler(this.connectButton_Click);
      // 
      // ipTextBox
      // 
      this.ipTextBox.Location = new System.Drawing.Point(1579, 24);
      this.ipTextBox.Margin = new System.Windows.Forms.Padding(8, 7, 8, 7);
      this.ipTextBox.Name = "ipTextBox";
      this.ipTextBox.Size = new System.Drawing.Size(308, 38);
      this.ipTextBox.TabIndex = 15;
      this.ipTextBox.Text = "155.98.111.59";
      // 
      // label1
      // 
      this.label1.AutoSize = true;
      this.label1.Location = new System.Drawing.Point(1419, 31);
      this.label1.Margin = new System.Windows.Forms.Padding(8, 0, 8, 0);
      this.label1.Name = "label1";
      this.label1.Size = new System.Drawing.Size(139, 32);
      this.label1.TabIndex = 16;
      this.label1.Text = "Server IP:";
      // 
      // Undo_Button
      // 
      this.Undo_Button.Location = new System.Drawing.Point(469, 24);
      this.Undo_Button.Margin = new System.Windows.Forms.Padding(3, 2, 3, 2);
      this.Undo_Button.Name = "Undo_Button";
      this.Undo_Button.Size = new System.Drawing.Size(144, 47);
      this.Undo_Button.TabIndex = 17;
      this.Undo_Button.Text = "Undo";
      this.Undo_Button.UseVisualStyleBackColor = true;
      this.Undo_Button.Click += new System.EventHandler(this.UndoButton_Click);
      // 
      // RevertButton
      // 
      this.RevertButton.Location = new System.Drawing.Point(619, 24);
      this.RevertButton.Margin = new System.Windows.Forms.Padding(3, 2, 3, 2);
      this.RevertButton.Name = "RevertButton";
      this.RevertButton.Size = new System.Drawing.Size(171, 47);
      this.RevertButton.TabIndex = 18;
      this.RevertButton.Text = "Revert";
      this.RevertButton.UseVisualStyleBackColor = true;
      this.RevertButton.Click += new System.EventHandler(this.RevertButton_Click);
      // 
      // FillSheetButton
      // 
      this.FillSheetButton.Location = new System.Drawing.Point(2181, 147);
      this.FillSheetButton.Margin = new System.Windows.Forms.Padding(8, 7, 8, 7);
      this.FillSheetButton.Name = "FillSheetButton";
      this.FillSheetButton.Size = new System.Drawing.Size(227, 55);
      this.FillSheetButton.TabIndex = 7;
      this.FillSheetButton.Text = "Fill Sheet";
      this.FillSheetButton.UseVisualStyleBackColor = true;
      this.FillSheetButton.Click += new System.EventHandler(this.FillSheetButton_Click);
      // 
      // spreadsheetPanel1
      // 
      this.spreadsheetPanel1.Location = new System.Drawing.Point(0, 95);
      this.spreadsheetPanel1.Margin = new System.Windows.Forms.Padding(3, 2, 3, 2);
      this.spreadsheetPanel1.Name = "spreadsheetPanel1";
      this.spreadsheetPanel1.Size = new System.Drawing.Size(2128, 973);
      this.spreadsheetPanel1.TabIndex = 0;
      this.spreadsheetPanel1.Load += new System.EventHandler(this.spreadsheetPanel1_Load);
      // 
      // Form1
      // 
      this.AutoScaleDimensions = new System.Drawing.SizeF(16F, 31F);
      this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
      this.ClientSize = new System.Drawing.Size(2128, 1054);
      this.Controls.Add(this.RevertButton);
      this.Controls.Add(this.Undo_Button);
      this.Controls.Add(this.label1);
      this.Controls.Add(this.ipTextBox);
      this.Controls.Add(this.connectButton);
      this.Controls.Add(this.Help_Button);
      this.Controls.Add(this.FillSheetButton);
      this.Controls.Add(this.EnterButton);
      this.Controls.Add(this.CellValueBox);
      this.Controls.Add(this.CellNameBox);
      this.Controls.Add(this.ContentsBox);
      this.Controls.Add(this.spreadsheetPanel1);
      this.FormBorderStyle = System.Windows.Forms.FormBorderStyle.FixedDialog;
      this.Margin = new System.Windows.Forms.Padding(3, 2, 3, 2);
      this.Name = "Form1";
      this.Text = "Form1";
      this.Activated += new System.EventHandler(this.spreadsheetPanel1_Load);
      this.FormClosing += new System.Windows.Forms.FormClosingEventHandler(this.Form1_FormClosing);
      this.Load += new System.EventHandler(this.spreadsheetPanel1_Load);
      this.ResumeLayout(false);
      this.PerformLayout();

        }

        #endregion

        private SpreadsheetTemplate.SpreadsheetPanel spreadsheetPanel1;
        private System.Windows.Forms.TextBox ContentsBox;
        private System.Windows.Forms.TextBox CellNameBox;
        private System.Windows.Forms.TextBox CellValueBox;
        private System.Windows.Forms.Button EnterButton;
        private System.ComponentModel.BackgroundWorker backgroundWorkerSetContents;
        private System.Windows.Forms.OpenFileDialog openFileDialog;
        private System.Windows.Forms.SaveFileDialog saveFileDialog;
        private System.ComponentModel.BackgroundWorker backgroundWorker1;
        private System.Windows.Forms.Button Help_Button;
        private System.Windows.Forms.Button connectButton;
        private System.Windows.Forms.TextBox ipTextBox;
        private System.Windows.Forms.Label label1;
    private System.Windows.Forms.Button Undo_Button;
    private System.Windows.Forms.Button RevertButton;
    private System.Windows.Forms.Button FillSheetButton;
  }
}

