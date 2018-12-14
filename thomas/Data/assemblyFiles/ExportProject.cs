using System;
using System.Runtime.InteropServices;
using System.Windows.Interop;
using ThomasEngine;
using System.Diagnostics;
namespace ThomasStandalone
{
    
    class Program
    {
        static public UInt32 run_time;

        static void Main(string[] args)
        {

            ThomasEngine.Debug.OnDebugMessage += Debug_OnDebugMessage;
            if (args.Length == 1)
                run_time = UInt32.Parse(args[0]);
            else
                run_time = 0;                
           
            ThomasWrapper.Start(false);

            float startTime = Time.ElapsedTime;

            Win32Window window = new Win32Window();
            window.create(System.AppDomain.CurrentDomain.FriendlyName, 800, 600);

            Application.currentProject = Project.LoadProject("..\\Data\\project.thomas");

            ThomasWrapper.IssuePlay();

            ThomasEngine.Debug.Log("Game loaded. It took " + (Time.ElapsedTime - startTime) + " ms");

            window.loop();

            ThomasEngine.Debug.Log("Shutting down");
            ThomasWrapper.Exit();
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
            if (newMessage.Severity == MessageSeverity.Error)
            {
                foreach (string line in newMessage.StackTrace)
                {
                    System.Console.Out.WriteLine(line);
                }
            }

            System.Console.ResetColor();
        }
    }

    class Win32Window
    {
        const UInt32 WS_OVERLAPPEDWINDOW = 0xcf0000;
        const UInt32 WS_VISIBLE = 0x10000000;
        const UInt32 CS_USEDEFAULT = 0x80000000;
        const UInt32 CS_DBLCLKS = 8;
        const UInt32 CS_VREDRAW = 1;
        const UInt32 CS_HREDRAW = 2;
        const UInt32 COLOR_WINDOW = 5;
        const UInt32 COLOR_BACKGROUND = 1;
        const UInt32 IDC_ARROW = 32512;
        const UInt32 IDC_CROSS = 32515;
        const UInt32 WM_DESTROY = 2;
        const UInt32 WM_PAINT =  0x000F;
        const UInt32 WM_LBUTTONUP = 0x0202;
        const UInt32 WM_LBUTTONDBLCLK = 0x0203;
        const UInt32 WM_CHAR = 0x0102;
        public static new IntPtr windowHandle;

        [DllImport("user32.dll")]
        static extern bool UpdateWindow(IntPtr hWnd);

        [DllImport("user32.dll")]
        [return: MarshalAs(UnmanagedType.Bool)]
        static extern bool ShowWindow(IntPtr hWnd, int nCmdShow);

        [System.Runtime.InteropServices.DllImport("user32.dll", SetLastError = true)]
        static extern bool DestroyWindow(IntPtr hWnd);

        [DllImport("kernel32.dll")]
        static extern uint GetLastError();

        [DllImport("user32.dll")]
        static extern IntPtr DefWindowProc(IntPtr hWnd, uint uMsg, IntPtr wParam, IntPtr lParam);

        [DllImport("user32.dll")]
        static extern void PostQuitMessage(int nExitCode);

        [DllImport("user32.dll")]
        static extern sbyte GetMessage(out MSG lpMsg, IntPtr hWnd, uint wMsgFilterMin, uint wMsgFilterMax);

        [DllImport("user32.dll")]
        static extern IntPtr LoadCursor(IntPtr hInstance, int lpCursorName);

        [DllImport("user32.dll")]
        static extern bool TranslateMessage([In] ref MSG lpMsg);

        [DllImport("user32.dll")]
        static extern IntPtr DispatchMessage([In] ref MSG lpmsg);

        internal bool create(string name, int width, int height)
        {
            windowHandle = ThomasWrapper.CreateThomasWindow(name, width, height);

            ShowWindow(windowHandle, 1);

            UpdateWindow(windowHandle);

            return true;
        }

        internal void loop()
        {
            MSG msg;
            if (ThomasStandalone.Program.run_time == 0)
            {
                while (GetMessage(out msg, IntPtr.Zero, 0, 0) != 0)
                {
                    TranslateMessage(ref msg);
                    DispatchMessage(ref msg);
                }
            }
            else
            {
                Stopwatch stopwatch = new Stopwatch();
                stopwatch.Start();
                while (GetMessage(out msg, IntPtr.Zero, 0, 0) != 0 && stopwatch.ElapsedMilliseconds < ThomasStandalone.Program.run_time)
                {
                    TranslateMessage(ref msg);
                    DispatchMessage(ref msg);
                }
            }

            ThomasWrapper.eventHandler(windowHandle, msg.message, msg.wParam, msg.lParam);
        }
    }
}
