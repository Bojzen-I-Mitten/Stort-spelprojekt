using System.Windows;
using System.Threading;
using ThomasEngine;

class GUIInput
{
    public static void AppendString(ref string _string, int length)
    {
        char c = Input.GetLastKeyChar();
        if (char.IsPunctuation(c) || char.IsLetterOrDigit(c) || char.IsSymbol(c) || char.IsSeparator(c) || char.IsWhiteSpace(c))
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