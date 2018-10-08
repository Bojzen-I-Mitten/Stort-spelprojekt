
using ThomasEngine;
namespace ThomasStandalone
{
    class Program
    {
        static void Main(string[] args)
        {
            Debug.OnDebugMessage += Debug_OnDebugMessage;

            ThomasWrapper.Start();
            Application.currentProject = Project.LoadProject(@"K:\Users\Morgan Cromell\Documents\Stort-spelprojekt\Concussion Ball\Concussion Ball.thomas");
            ThomasWrapper.Play();
            new Win32Window().create("test", 800, 600);
            ThomasWrapper.Stop();
        }

        private static void Debug_OnDebugMessage(DebugMessage newMessage)
        {
            switch(newMessage.Severity)
            {
                case MessageSeverity.Info:
                    System.Console.BackgroundColor = System.ConsoleColor.Black;
                    break;
                case MessageSeverity.Warning:
                    System.Console.BackgroundColor = System.ConsoleColor.DarkYellow;
                    break;
                case MessageSeverity.Error:
                    System.Console.BackgroundColor = System.ConsoleColor.Red;
                    break;
                case MessageSeverity.ThomasCore:
                    System.Console.BackgroundColor = System.ConsoleColor.Cyan;
                    break;
            }
            //System.Console.ForegroundColor = System.ConsoleColor.Black;
            System.Console.Out.WriteLine(newMessage.Message);
            System.Console.ResetColor();
        }
    }
}
