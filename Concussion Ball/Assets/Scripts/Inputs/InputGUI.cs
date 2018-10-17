using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

using ThomasEngine;

namespace InputIP
{

    public class InputGUI
    {
        public static void AppendIPString(ref string IP, int length)
        {
            if (IP.Length < length)
            {
                if (Input.GetKeyUp(Input.Keys.OemPeriod))
                {
                    IP += ".";
                }
                else if (Input.GetKeyUp(Input.Keys.NumPad0) || Input.GetKeyUp(Input.Keys.D0))
                {
                    IP += "0";
                }
                else if (Input.GetKeyUp(Input.Keys.NumPad1) || Input.GetKeyUp(Input.Keys.D1))
                {
                    IP += "1";
                }
                else if (Input.GetKeyUp(Input.Keys.NumPad2) || Input.GetKeyUp(Input.Keys.D2))
                {
                    IP += "2";
                }
                else if (Input.GetKeyUp(Input.Keys.NumPad3) || Input.GetKeyUp(Input.Keys.D3))
                {
                    IP += "3";
                }
                else if (Input.GetKeyUp(Input.Keys.NumPad4) || Input.GetKeyUp(Input.Keys.D4))
                {
                    IP += "4";
                }
                else if (Input.GetKeyUp(Input.Keys.NumPad5) || Input.GetKeyUp(Input.Keys.D5))
                {
                    IP += "5";
                }
                else if (Input.GetKeyUp(Input.Keys.NumPad6) || Input.GetKeyUp(Input.Keys.D6))
                {
                    IP += "6";
                }
                else if (Input.GetKeyUp(Input.Keys.NumPad7) || Input.GetKeyUp(Input.Keys.D7))
                {
                    IP += "7";
                }
                else if (Input.GetKeyUp(Input.Keys.NumPad8) || Input.GetKeyUp(Input.Keys.D8))
                {
                    IP += "8";
                }
                else if (Input.GetKeyUp(Input.Keys.NumPad9) || Input.GetKeyUp(Input.Keys.D9))
                {
                    IP += "9";
                }
            }
            if (Input.GetKeyUp(Input.Keys.Back))
            {
                if (IP.Length > 0)
                    IP = IP.Remove(IP.Length - 1);
            }
        }
    }
}
