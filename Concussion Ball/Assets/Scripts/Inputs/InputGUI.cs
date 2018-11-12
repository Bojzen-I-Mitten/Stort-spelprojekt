using System.Windows;
using System.Net;
using System.Threading;
using ThomasEngine;

class InputGUI
{
    public static void AppendIPString(ref string IP, int length)
    {
        if (char.IsNumber(Input.GetLastKeyChar()) || char.IsPunctuation(Input.GetLastKeyChar()))
        {
            if (IP.Length < length)
                IP += Input.GetLastKeyChar();
        }
        else if (Input.GetKey(Input.Keys.Back))
        {
            if (IP.Length > 0)
                IP = IP.Remove(IP.Length - 1);
        }
        else if ((Input.GetKey(Input.Keys.LeftControl) || Input.GetKey(Input.Keys.RightControl)) && Input.GetKeyDown(Input.Keys.V))
        {
            string pastedText = TryPaste();
            if (pastedText.Length < length)
            {
                IPAddress address;

                if (IPAddress.TryParse(pastedText, out address))
                {
                    IP = address.ToString();
                }
            }
        }
    }

    public static string TryPaste()
    {
        string pastedText = "";
        Thread t = new Thread(() =>
        {
            pastedText = Clipboard.GetText();
        });
        t.SetApartmentState(ApartmentState.STA);
        t.Start();
        t.Join();
        return pastedText;
    }
}