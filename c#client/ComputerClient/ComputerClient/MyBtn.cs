using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace ComputerClient
{
    public class MyBtn : System.Windows.Forms.Button
    {
        private int id, btnid;
        public MyBtn(int id, int btnid,int px, int py)
        {
            String text = "";
            switch (btnid)
            {
                case 1:
                    text = "<";
                    break;
                case 2:
                    text = ">";
                    break;
                case 3:
                    text = "YES";
                    break;
                case 4:
                    text = "NO";
                    break;
                case 5:
                    text = "CLK";
                    break;
                default:
                    text = "NA";
                    break;
            }
            this.Location = new System.Drawing.Point(px, py);
            
            this.Name = "button1";
            this.Size = new System.Drawing.Size(40, 23);
            this.TabIndex = 0;
            this.Text = text;
            this.UseVisualStyleBackColor = true;
            this.Click += new System.EventHandler(this.click);
        }
        private void click(object sender, EventArgs e)
        {
            String d = "";
            d += 'a' + this.id - 1;
            d += ':';
            d += 'a' + this.btnid - 1;
            d += '$';
            if(WerewolfMain.io != null && WerewolfMain.io.IsOpen)
                WerewolfMain.io.Write(d);
        }
    }

}
