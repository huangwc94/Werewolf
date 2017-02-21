namespace ComputerClient
{
    partial class WerewolfMain
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
            System.ComponentModel.ComponentResourceManager resources = new System.ComponentModel.ComponentResourceManager(typeof(WerewolfMain));
            this.gamePanel = new System.Windows.Forms.Panel();
            this.connectBtn = new System.Windows.Forms.Button();
            this.refreshBtn = new System.Windows.Forms.Button();
            this.comList = new System.Windows.Forms.ComboBox();
            this.debugOp = new System.Windows.Forms.TextBox();
            this.SuspendLayout();
            // 
            // gamePanel
            // 
            this.gamePanel.BorderStyle = System.Windows.Forms.BorderStyle.Fixed3D;
            this.gamePanel.Location = new System.Drawing.Point(12, 67);
            this.gamePanel.Name = "gamePanel";
            this.gamePanel.Size = new System.Drawing.Size(1240, 631);
            this.gamePanel.TabIndex = 0;
            // 
            // connectBtn
            // 
            this.connectBtn.Location = new System.Drawing.Point(351, 12);
            this.connectBtn.Name = "connectBtn";
            this.connectBtn.Size = new System.Drawing.Size(158, 49);
            this.connectBtn.TabIndex = 1;
            this.connectBtn.Text = "Connect";
            this.connectBtn.UseVisualStyleBackColor = true;
            this.connectBtn.Click += new System.EventHandler(this.connectBtn_Click);
            // 
            // refreshBtn
            // 
            this.refreshBtn.Location = new System.Drawing.Point(187, 12);
            this.refreshBtn.Name = "refreshBtn";
            this.refreshBtn.Size = new System.Drawing.Size(158, 49);
            this.refreshBtn.TabIndex = 2;
            this.refreshBtn.Text = "Refresh";
            this.refreshBtn.UseVisualStyleBackColor = true;
            this.refreshBtn.Click += new System.EventHandler(this.refreshBtn_Click);
            // 
            // comList
            // 
            this.comList.DropDownStyle = System.Windows.Forms.ComboBoxStyle.DropDownList;
            this.comList.Font = new System.Drawing.Font("SimSun", 14F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(134)));
            this.comList.FormattingEnabled = true;
            this.comList.ImeMode = System.Windows.Forms.ImeMode.Disable;
            this.comList.Location = new System.Drawing.Point(12, 27);
            this.comList.Name = "comList";
            this.comList.Size = new System.Drawing.Size(169, 27);
            this.comList.TabIndex = 3;
            // 
            // debugOp
            // 
            this.debugOp.BackColor = System.Drawing.SystemColors.InactiveCaptionText;
            this.debugOp.ForeColor = System.Drawing.SystemColors.Info;
            this.debugOp.Location = new System.Drawing.Point(13, 705);
            this.debugOp.Multiline = true;
            this.debugOp.Name = "debugOp";
            this.debugOp.ReadOnly = true;
            this.debugOp.ScrollBars = System.Windows.Forms.ScrollBars.Vertical;
            this.debugOp.Size = new System.Drawing.Size(1239, 204);
            this.debugOp.TabIndex = 4;
            // 
            // WerewolfMain
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 12F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.ClientSize = new System.Drawing.Size(1264, 921);
            this.Controls.Add(this.debugOp);
            this.Controls.Add(this.comList);
            this.Controls.Add(this.refreshBtn);
            this.Controls.Add(this.connectBtn);
            this.Controls.Add(this.gamePanel);
            this.Icon = ((System.Drawing.Icon)(resources.GetObject("$this.Icon")));
            this.MaximizeBox = false;
            this.MinimizeBox = false;
            this.Name = "WerewolfMain";
            this.StartPosition = System.Windows.Forms.FormStartPosition.CenterScreen;
            this.Text = "Werewolf Computer Client v0.1";
            this.ResumeLayout(false);
            this.PerformLayout();

        }

        #endregion

        private System.Windows.Forms.Panel gamePanel;
        private System.Windows.Forms.Button connectBtn;
        private System.Windows.Forms.Button refreshBtn;
        private System.Windows.Forms.ComboBox comList;
        private System.Windows.Forms.TextBox debugOp;
    }
}

