using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Forms;
using System.IO.Ports;
namespace ComputerClient
{
    public partial class WerewolfMain : Form
    {
        public static SerialPort io;
        private static Encoding enc = new UTF8Encoding(true, true);

        private Label[] whiteLight;
        private Label[] redLight;

        public WerewolfMain()
        {
            InitializeComponent();

            whiteLight = new Label[12];
            redLight = new Label[12];

            initGame();
            refreshBtn_Click(null, null);
            Console.WriteLine("Hello World!");
        }

        private void init_serial(String port)
        {
            if (io != null && io.IsOpen)
            {
                io.Dispose();
                io = null;
            }
            try
            {
                this.debugOp.Text = "";
                this.connectBtn.Text = "Connecting...";
                io = new SerialPort(port, 115200);
                io.DtrEnable = true;
                io.DataReceived += new SerialDataReceivedEventHandler(port_reveiced);
                io.Open();
                while (!io.IsOpen) ;
                this.connectBtn.Text = "Close";
            }
            catch(Exception e)
            {
                Console.WriteLine(e.ToString());
                MessageBox.Show(e.ToString(), "Error");
            }
            
            
        }

        private void port_reveiced(object sender, SerialDataReceivedEventArgs e)
        {

            String data = io.ReadLine();
            data = enc.GetString(enc.GetBytes(data));

            Console.WriteLine(data);
            this.BeginInvoke(new MethodInvoker(delegate { this.log("RECEIVED", data); }));
            
            //this.log("RECEIVED", data);
            
        }


        private void connectBtn_Click(object sender, EventArgs e)
        {
            if (io != null && io.IsOpen)
            {
                io.Dispose();
                this.connectBtn.Text = "Connect";
                this.log("SYSTEM", "Port close");
            }
            else
            {
                this.init_serial(this.comList.SelectedItem.ToString());
                this.log("SYSTEM" , "Connecting " + this.comList.SelectedItem.ToString() + "\n");
            }
               
        }

        private void refreshBtn_Click(object sender, EventArgs e)
        {
            this.comList.Items.Clear();
            foreach(String s in SerialPort.GetPortNames())
            {
                this.comList.Items.Add(s);
            }
            this.comList.SelectedIndex = 0;
        }
        public void log(String sender,String data)
        {
            if (data.StartsWith("GLIGHT:"))
            {
                UInt16 g;
                UInt16.TryParse(data.Split(':')[1], out g);
                updateGreen(g);
            }
            else if (data.StartsWith("RLIGHT:"))
            {
                UInt16 r;
                UInt16.TryParse(data.Split(':')[1], out r);
                updateRed(r);
            }

            if (!data.EndsWith("\n"))
                data += "\n";
            data = sender + ":" + data;
            this.debugOp.AppendText(data);




        }
        private void initGame()
        {
            int centerx = 45;
            int centery = 100;
            int dx = 300;
            int dy = 150;
            for(int i = 1; i <= 12; i++)
            {

                int col_num = 0;
                int row_num = 0;

                if (i <= 4)
                {
                    col_num = i - 1;
                    row_num = 0;
                }else if(i >= 9)
                {
                    col_num = i - 9;
                    row_num = 3;
                }else
                {
                    col_num = ((i - 1) % 2) * 3;
                    if(i<=6)
                    {
                        row_num = 1;
                    }else
                    {
                        row_num = 2;
                    }
                }
                

                
                int px = centerx + col_num * dx;
                int py = centery + row_num * dy;

                // red label
                Label label1 = new Label();
                label1.BackColor = System.Drawing.Color.DarkRed;
                label1.BorderStyle = System.Windows.Forms.BorderStyle.FixedSingle;

                label1.Location = new System.Drawing.Point(px + 50, py - 50);

                label1.Size = new System.Drawing.Size(50, 50);
                label1.TabIndex = 0;
                label1.Text = "";
                this.gamePanel.Controls.Add(label1);
                redLight[i - 1] = label1;
                // white label
                label1 = new Label();
                label1.BackColor = System.Drawing.Color.White;
                label1.BorderStyle = System.Windows.Forms.BorderStyle.FixedSingle;

                label1.Location = new System.Drawing.Point(px + 120, py - 50);

                label1.Size = new System.Drawing.Size(50, 50);
                label1.TabIndex = 0;
                label1.Text = "";

                this.gamePanel.Controls.Add(label1);
                whiteLight[i - 1] = label1;

                for (int j = 1;j <= 5; j++)
                {
                    MyBtn b = new MyBtn(i,j,px + j * 40,py);
                    this.gamePanel.Controls.Add(b);
                    
                }
            }
        }
        
        private void updateGreen(UInt16 l)
        {
            for(int i = 1; i <= 12; i++)
            {
                if(contain(l,i))
                {
                    whiteLight[i - 1].Visible = true;
                }else
                {
                    whiteLight[i - 1].Visible = false;
                }
            }
        }
        private void updateRed(UInt16 l)
        {
            for (int i = 1; i <= 12; i++)
            {
                if (contain(l, i))
                {
                    redLight[i - 1].Visible = true;
                }
                else
                {
                    redLight[i - 1].Visible = false;
                }
            }
        }
        private bool contain(UInt16 l,int i)
        {
            if (i < 1)
            {
                return false;
            }
            return (l & (1 << (i - 1))) > 0;
        }

    }
}
