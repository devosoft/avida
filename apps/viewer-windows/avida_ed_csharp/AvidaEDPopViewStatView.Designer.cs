namespace avida_ed_csharp
{
    partial class AvidaEDPopViewStatView
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

        #region Component Designer generated code

        /// <summary> 
        /// Required method for Designer support - do not modify 
        /// the contents of this method with the code editor.
        /// </summary>
        private void InitializeComponent()
        {
            System.ComponentModel.ComponentResourceManager resources = new System.ComponentModel.ComponentResourceManager(typeof(AvidaEDPopViewStatView));
            this.MapStatsViewContainer = new System.Windows.Forms.SplitContainer();
            this.MapPanel = new System.Windows.Forms.Panel();
            this.DishIcon = new System.Windows.Forms.PictureBox();
            this.DishLabel = new System.Windows.Forms.Label();
            this.MapViewBackgroundPanel = new System.Windows.Forms.Panel();
            this.TimeLabel = new System.Windows.Forms.Label();
            this.TimeValueLabel = new System.Windows.Forms.Label();
            this.ZoomTrackBar = new System.Windows.Forms.TrackBar();
            this.ModePanel = new System.Windows.Forms.Panel();
            this.FreezeButton = new System.Windows.Forms.Button();
            this.RunButton = new System.Windows.Forms.Button();
            this.NewButton = new System.Windows.Forms.Button();
            this.ModeComboBox = new System.Windows.Forms.ComboBox();
            this.ModeLabel = new System.Windows.Forms.Label();
            this.StatsButton = new System.Windows.Forms.CheckBox();
            this.SetupButton = new System.Windows.Forms.CheckBox();
            this.SelectedOrganismGenotypeBox = new System.Windows.Forms.GroupBox();
            this.PopulationStatisticsBox = new System.Windows.Forms.GroupBox();
            this.StatsGraphPanel = new System.Windows.Forms.Panel();
            this.SelectedAndPopStatsPanel = new System.Windows.Forms.SplitContainer();
            ((System.ComponentModel.ISupportInitialize)(this.MapStatsViewContainer)).BeginInit();
            this.MapStatsViewContainer.Panel1.SuspendLayout();
            this.MapStatsViewContainer.Panel2.SuspendLayout();
            this.MapStatsViewContainer.SuspendLayout();
            this.MapPanel.SuspendLayout();
            ((System.ComponentModel.ISupportInitialize)(this.DishIcon)).BeginInit();
            ((System.ComponentModel.ISupportInitialize)(this.ZoomTrackBar)).BeginInit();
            this.ModePanel.SuspendLayout();
            ((System.ComponentModel.ISupportInitialize)(this.SelectedAndPopStatsPanel)).BeginInit();
            this.SelectedAndPopStatsPanel.Panel1.SuspendLayout();
            this.SelectedAndPopStatsPanel.Panel2.SuspendLayout();
            this.SelectedAndPopStatsPanel.SuspendLayout();
            this.SuspendLayout();
            // 
            // MapStatsViewContainer
            // 
            this.MapStatsViewContainer.Dock = System.Windows.Forms.DockStyle.Fill;
            this.MapStatsViewContainer.Location = new System.Drawing.Point(0, 0);
            this.MapStatsViewContainer.Name = "MapStatsViewContainer";
            // 
            // MapStatsViewContainer.Panel1
            // 
            this.MapStatsViewContainer.Panel1.Controls.Add(this.SetupButton);
            this.MapStatsViewContainer.Panel1.Controls.Add(this.StatsButton);
            this.MapStatsViewContainer.Panel1.Controls.Add(this.DishLabel);
            this.MapStatsViewContainer.Panel1.Controls.Add(this.DishIcon);
            this.MapStatsViewContainer.Panel1.Controls.Add(this.MapPanel);
            // 
            // MapStatsViewContainer.Panel2
            // 
            this.MapStatsViewContainer.Panel2.Controls.Add(this.SelectedAndPopStatsPanel);
            this.MapStatsViewContainer.Panel2.Controls.Add(this.StatsGraphPanel);
            this.MapStatsViewContainer.Size = new System.Drawing.Size(777, 499);
            this.MapStatsViewContainer.SplitterDistance = 441;
            this.MapStatsViewContainer.TabIndex = 0;
            // 
            // MapPanel
            // 
            this.MapPanel.Anchor = ((System.Windows.Forms.AnchorStyles)((((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Bottom) 
            | System.Windows.Forms.AnchorStyles.Left) 
            | System.Windows.Forms.AnchorStyles.Right)));
            this.MapPanel.Controls.Add(this.ModePanel);
            this.MapPanel.Controls.Add(this.ZoomTrackBar);
            this.MapPanel.Controls.Add(this.TimeValueLabel);
            this.MapPanel.Controls.Add(this.TimeLabel);
            this.MapPanel.Controls.Add(this.MapViewBackgroundPanel);
            this.MapPanel.Location = new System.Drawing.Point(0, 44);
            this.MapPanel.Name = "MapPanel";
            this.MapPanel.Size = new System.Drawing.Size(438, 455);
            this.MapPanel.TabIndex = 0;
            // 
            // DishIcon
            // 
            this.DishIcon.Image = ((System.Drawing.Image)(resources.GetObject("DishIcon.Image")));
            this.DishIcon.Location = new System.Drawing.Point(4, 4);
            this.DishIcon.Name = "DishIcon";
            this.DishIcon.Size = new System.Drawing.Size(46, 34);
            this.DishIcon.SizeMode = System.Windows.Forms.PictureBoxSizeMode.Zoom;
            this.DishIcon.TabIndex = 1;
            this.DishIcon.TabStop = false;
            // 
            // DishLabel
            // 
            this.DishLabel.AutoSize = true;
            this.DishLabel.Location = new System.Drawing.Point(56, 14);
            this.DishLabel.Name = "DishLabel";
            this.DishLabel.Size = new System.Drawing.Size(50, 13);
            this.DishLabel.TabIndex = 2;
            this.DishLabel.Text = "@default";
            // 
            // MapViewBackgroundPanel
            // 
            this.MapViewBackgroundPanel.Anchor = ((System.Windows.Forms.AnchorStyles)((((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Bottom) 
            | System.Windows.Forms.AnchorStyles.Left) 
            | System.Windows.Forms.AnchorStyles.Right)));
            this.MapViewBackgroundPanel.BackColor = System.Drawing.SystemColors.ControlDark;
            this.MapViewBackgroundPanel.Location = new System.Drawing.Point(2, 0);
            this.MapViewBackgroundPanel.Name = "MapViewBackgroundPanel";
            this.MapViewBackgroundPanel.Size = new System.Drawing.Size(436, 400);
            this.MapViewBackgroundPanel.TabIndex = 0;
            // 
            // TimeLabel
            // 
            this.TimeLabel.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Bottom | System.Windows.Forms.AnchorStyles.Left)));
            this.TimeLabel.AutoSize = true;
            this.TimeLabel.Location = new System.Drawing.Point(4, 407);
            this.TimeLabel.Name = "TimeLabel";
            this.TimeLabel.Size = new System.Drawing.Size(33, 13);
            this.TimeLabel.TabIndex = 1;
            this.TimeLabel.Text = "Time:";
            // 
            // TimeValueLabel
            // 
            this.TimeValueLabel.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Bottom | System.Windows.Forms.AnchorStyles.Left)));
            this.TimeValueLabel.AutoSize = true;
            this.TimeValueLabel.ForeColor = System.Drawing.SystemColors.ControlDark;
            this.TimeValueLabel.Location = new System.Drawing.Point(43, 407);
            this.TimeValueLabel.Name = "TimeValueLabel";
            this.TimeValueLabel.Size = new System.Drawing.Size(63, 13);
            this.TimeValueLabel.TabIndex = 2;
            this.TimeValueLabel.Text = "(not started)";
            // 
            // ZoomTrackBar
            // 
            this.ZoomTrackBar.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Bottom | System.Windows.Forms.AnchorStyles.Right)));
            this.ZoomTrackBar.Location = new System.Drawing.Point(331, 404);
            this.ZoomTrackBar.Name = "ZoomTrackBar";
            this.ZoomTrackBar.Size = new System.Drawing.Size(104, 45);
            this.ZoomTrackBar.TabIndex = 9;
            // 
            // ModePanel
            // 
            this.ModePanel.Anchor = System.Windows.Forms.AnchorStyles.Bottom;
            this.ModePanel.Controls.Add(this.FreezeButton);
            this.ModePanel.Controls.Add(this.ModeLabel);
            this.ModePanel.Controls.Add(this.RunButton);
            this.ModePanel.Controls.Add(this.ModeComboBox);
            this.ModePanel.Controls.Add(this.NewButton);
            this.ModePanel.Location = new System.Drawing.Point(131, 401);
            this.ModePanel.Name = "ModePanel";
            this.ModePanel.Size = new System.Drawing.Size(158, 51);
            this.ModePanel.TabIndex = 10;
            // 
            // FreezeButton
            // 
            this.FreezeButton.Anchor = System.Windows.Forms.AnchorStyles.Bottom;
            this.FreezeButton.Location = new System.Drawing.Point(104, 25);
            this.FreezeButton.Name = "FreezeButton";
            this.FreezeButton.Size = new System.Drawing.Size(51, 23);
            this.FreezeButton.TabIndex = 15;
            this.FreezeButton.Text = "Freeze";
            this.FreezeButton.UseVisualStyleBackColor = true;
            // 
            // RunButton
            // 
            this.RunButton.Anchor = System.Windows.Forms.AnchorStyles.Bottom;
            this.RunButton.Location = new System.Drawing.Point(57, 25);
            this.RunButton.Name = "RunButton";
            this.RunButton.Size = new System.Drawing.Size(43, 23);
            this.RunButton.TabIndex = 14;
            this.RunButton.Text = "Run";
            this.RunButton.UseVisualStyleBackColor = true;
            // 
            // NewButton
            // 
            this.NewButton.Anchor = System.Windows.Forms.AnchorStyles.Bottom;
            this.NewButton.Location = new System.Drawing.Point(7, 25);
            this.NewButton.Name = "NewButton";
            this.NewButton.Size = new System.Drawing.Size(46, 23);
            this.NewButton.TabIndex = 13;
            this.NewButton.Text = "New";
            this.NewButton.UseVisualStyleBackColor = true;
            // 
            // ModeComboBox
            // 
            this.ModeComboBox.Anchor = System.Windows.Forms.AnchorStyles.Bottom;
            this.ModeComboBox.FormattingEnabled = true;
            this.ModeComboBox.Location = new System.Drawing.Point(47, 0);
            this.ModeComboBox.Name = "ModeComboBox";
            this.ModeComboBox.Size = new System.Drawing.Size(108, 21);
            this.ModeComboBox.TabIndex = 12;
            // 
            // ModeLabel
            // 
            this.ModeLabel.Anchor = System.Windows.Forms.AnchorStyles.Bottom;
            this.ModeLabel.AutoSize = true;
            this.ModeLabel.Location = new System.Drawing.Point(4, 3);
            this.ModeLabel.Name = "ModeLabel";
            this.ModeLabel.Size = new System.Drawing.Size(37, 13);
            this.ModeLabel.TabIndex = 11;
            this.ModeLabel.Text = "Mode:";
            // 
            // StatsButton
            // 
            this.StatsButton.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Right)));
            this.StatsButton.Appearance = System.Windows.Forms.Appearance.Button;
            this.StatsButton.AutoSize = true;
            this.StatsButton.Checked = true;
            this.StatsButton.CheckState = System.Windows.Forms.CheckState.Checked;
            this.StatsButton.Location = new System.Drawing.Point(394, 9);
            this.StatsButton.Name = "StatsButton";
            this.StatsButton.Size = new System.Drawing.Size(41, 23);
            this.StatsButton.TabIndex = 5;
            this.StatsButton.Text = "Stats";
            this.StatsButton.UseVisualStyleBackColor = true;
            // 
            // SetupButton
            // 
            this.SetupButton.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Right)));
            this.SetupButton.Appearance = System.Windows.Forms.Appearance.Button;
            this.SetupButton.AutoSize = true;
            this.SetupButton.Location = new System.Drawing.Point(347, 9);
            this.SetupButton.Name = "SetupButton";
            this.SetupButton.Size = new System.Drawing.Size(45, 23);
            this.SetupButton.TabIndex = 6;
            this.SetupButton.Text = "Setup";
            this.SetupButton.UseVisualStyleBackColor = true;
            // 
            // SelectedOrganismGenotypeBox
            // 
            this.SelectedOrganismGenotypeBox.Dock = System.Windows.Forms.DockStyle.Fill;
            this.SelectedOrganismGenotypeBox.Location = new System.Drawing.Point(0, 0);
            this.SelectedOrganismGenotypeBox.Name = "SelectedOrganismGenotypeBox";
            this.SelectedOrganismGenotypeBox.Size = new System.Drawing.Size(163, 252);
            this.SelectedOrganismGenotypeBox.TabIndex = 0;
            this.SelectedOrganismGenotypeBox.TabStop = false;
            this.SelectedOrganismGenotypeBox.Text = "Selected Organism Genotype";
            // 
            // PopulationStatisticsBox
            // 
            this.PopulationStatisticsBox.Dock = System.Windows.Forms.DockStyle.Fill;
            this.PopulationStatisticsBox.Location = new System.Drawing.Point(0, 0);
            this.PopulationStatisticsBox.Name = "PopulationStatisticsBox";
            this.PopulationStatisticsBox.Size = new System.Drawing.Size(162, 252);
            this.PopulationStatisticsBox.TabIndex = 1;
            this.PopulationStatisticsBox.TabStop = false;
            this.PopulationStatisticsBox.Text = "Population Statistics";
            // 
            // StatsGraphPanel
            // 
            this.StatsGraphPanel.Anchor = ((System.Windows.Forms.AnchorStyles)((((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Bottom) 
            | System.Windows.Forms.AnchorStyles.Left) 
            | System.Windows.Forms.AnchorStyles.Right)));
            this.StatsGraphPanel.BackColor = System.Drawing.SystemColors.ControlDark;
            this.StatsGraphPanel.Location = new System.Drawing.Point(4, 258);
            this.StatsGraphPanel.Name = "StatsGraphPanel";
            this.StatsGraphPanel.Size = new System.Drawing.Size(325, 241);
            this.StatsGraphPanel.TabIndex = 2;
            // 
            // SelectedAndPopStatsPanel
            // 
            this.SelectedAndPopStatsPanel.Anchor = ((System.Windows.Forms.AnchorStyles)(((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Left) 
            | System.Windows.Forms.AnchorStyles.Right)));
            this.SelectedAndPopStatsPanel.IsSplitterFixed = true;
            this.SelectedAndPopStatsPanel.Location = new System.Drawing.Point(3, 0);
            this.SelectedAndPopStatsPanel.Name = "SelectedAndPopStatsPanel";
            // 
            // SelectedAndPopStatsPanel.Panel1
            // 
            this.SelectedAndPopStatsPanel.Panel1.Controls.Add(this.SelectedOrganismGenotypeBox);
            // 
            // SelectedAndPopStatsPanel.Panel2
            // 
            this.SelectedAndPopStatsPanel.Panel2.Controls.Add(this.PopulationStatisticsBox);
            this.SelectedAndPopStatsPanel.Size = new System.Drawing.Size(329, 252);
            this.SelectedAndPopStatsPanel.SplitterDistance = 163;
            this.SelectedAndPopStatsPanel.TabIndex = 17;
            // 
            // AvidaEDPopViewStatView
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.Controls.Add(this.MapStatsViewContainer);
            this.Name = "AvidaEDPopViewStatView";
            this.Size = new System.Drawing.Size(777, 499);
            this.MapStatsViewContainer.Panel1.ResumeLayout(false);
            this.MapStatsViewContainer.Panel1.PerformLayout();
            this.MapStatsViewContainer.Panel2.ResumeLayout(false);
            ((System.ComponentModel.ISupportInitialize)(this.MapStatsViewContainer)).EndInit();
            this.MapStatsViewContainer.ResumeLayout(false);
            this.MapPanel.ResumeLayout(false);
            this.MapPanel.PerformLayout();
            ((System.ComponentModel.ISupportInitialize)(this.DishIcon)).EndInit();
            ((System.ComponentModel.ISupportInitialize)(this.ZoomTrackBar)).EndInit();
            this.ModePanel.ResumeLayout(false);
            this.ModePanel.PerformLayout();
            this.SelectedAndPopStatsPanel.Panel1.ResumeLayout(false);
            this.SelectedAndPopStatsPanel.Panel2.ResumeLayout(false);
            ((System.ComponentModel.ISupportInitialize)(this.SelectedAndPopStatsPanel)).EndInit();
            this.SelectedAndPopStatsPanel.ResumeLayout(false);
            this.ResumeLayout(false);

        }

        #endregion

        private System.Windows.Forms.SplitContainer MapStatsViewContainer;
        private System.Windows.Forms.Panel MapPanel;
        private System.Windows.Forms.Label DishLabel;
        private System.Windows.Forms.PictureBox DishIcon;
        private System.Windows.Forms.Label TimeValueLabel;
        private System.Windows.Forms.Label TimeLabel;
        private System.Windows.Forms.Panel MapViewBackgroundPanel;
        private System.Windows.Forms.TrackBar ZoomTrackBar;
        private System.Windows.Forms.Button FreezeButton;
        private System.Windows.Forms.Button RunButton;
        private System.Windows.Forms.Button NewButton;
        private System.Windows.Forms.ComboBox ModeComboBox;
        private System.Windows.Forms.Label ModeLabel;
        private System.Windows.Forms.Panel ModePanel;
        private System.Windows.Forms.CheckBox SetupButton;
        private System.Windows.Forms.CheckBox StatsButton;
        private System.Windows.Forms.Panel StatsGraphPanel;
        private System.Windows.Forms.GroupBox PopulationStatisticsBox;
        private System.Windows.Forms.GroupBox SelectedOrganismGenotypeBox;
        private System.Windows.Forms.SplitContainer SelectedAndPopStatsPanel;
    }
}
