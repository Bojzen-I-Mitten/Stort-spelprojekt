using System.Windows;
using System.Threading;
using System.Text.RegularExpressions;
using ThomasEngine;

class GUIInput
{
    public static void AppendString(ref string _string, int length)
    {
        char c = Input.GetLastKeyChar();
        string regexString = "";
        regexString += c;

        // No whitespace allowed
        if (Regex.IsMatch(regexString, "^[a-zA-Z0-9-.]*$"))
        {
            if (_string.Length < length)
            {
                _string += Input.GetLastKeyChar();
            }
        }
        else if (c == (char)Input.Keys.Back)
        {
            if (_string.Length > 0)
            {
                _string = _string.Remove(_string.Length - 1);
            }
        }
        else if ((Input.GetKey(Input.Keys.LeftControl) || Input.GetKey(Input.Keys.RightControl)) && Input.GetKeyDown(Input.Keys.V))
        {
            string pastedText = TryPaste();
            if (pastedText.Length < length)
            {
                _string = pastedText;
            }
        }

        Input.SetLastKey(0);
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