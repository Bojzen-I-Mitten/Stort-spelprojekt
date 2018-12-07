using System;

using System.Runtime.InteropServices;
using System.Windows;
using System.Windows.Interop;
using ThomasEngine;
namespace ThomasEditor
{
    public enum WindowType
    {
        EditorWindow,
        GameWindow
    }

    public class ThomasWindow : HwndHost
    {
        public static readonly DependencyProperty IsEditorProperty =
            DependencyProperty.Register(
                "IsEditor", typeof(Boolean), typeof(ThomasWindow), new PropertyMetadata(false));

        public bool IsEditor
        {
            get { return (bool)GetValue(IsEditorProperty); }
            set
            {
                SetValue(IsEditorProperty, value);
            }
        }

        public WindowType Type
        {
            get; set;
        }

        public new IntPtr windowHandle { get; private set; }
        const int WM_PAINT = 0x000F;
        const int WM_SIZE = 0x0005;
        const int WM_LBUTTONDOWN = 0x0201;

        [StructLayout(LayoutKind.Sequential)]
        struct Rect
        {
            public int Left;
            public int Top;
            public int Right;
            public int Bottom;
        }

        [StructLayout(LayoutKind.Sequential)]
        struct Paint
        {
            public IntPtr Context;
            public bool Erase;
            public Rect Area;
            public bool Restore;
            public bool Update;
            [MarshalAs(UnmanagedType.ByValArray, SizeConst = 32)]
            public byte[] Reserved;
        }

        [DllImport("user32.dll")]
        static extern IntPtr LoadCursor
            (IntPtr instance,
            int name);

        [DllImport("user32.dll")]
        static extern IntPtr SetFocus(IntPtr hwnd);

        [DllImport("user32.dll")]
        static extern bool DestroyWindow(IntPtr handle);

        [DllImport("user32.dll")]
        static extern IntPtr BeginPaint(IntPtr handle,
            out Paint paint);

        [DllImport("user32.dll")]
        static extern bool EndPaint(IntPtr handle,
            [In] ref Paint paint);

        protected override HandleRef BuildWindowCore(HandleRef parent)
        {            
            var width = Convert.ToInt32(ActualWidth);
            var height = Convert.ToInt32(ActualHeight);

            windowHandle = ThomasWrapper.CreateThomasWindow(parent.Handle, width, height, IsEditor);
            GotFocus += ThomasWindow_GotFocus;

            switch (Type)
            {
                case WindowType.EditorWindow:
                    ThomasWrapper.ThomasEditorWindowLoaded();
                    break;
                case WindowType.GameWindow:
                    ThomasWrapper.ThomasGameWindowLoaded();
                    break;
                default:
                    throw new System.InvalidOperationException("Non-supported window type was added to the execution");
            }

            return new HandleRef(this, windowHandle);            
        }

        private void ThomasWindow_GotFocus(object sender, RoutedEventArgs e)
        {
            SetFocus(windowHandle);
        }

        protected override void DestroyWindowCore(HandleRef hwnd)
        {
            DestroyWindow(windowHandle);
        }

        /*protected override IntPtr WndProc(IntPtr hwnd, int message, IntPtr wparam, IntPtr lparam, ref bool handled)
        {
           try
            {
                if (message == WM_PAINT)
                {
                    Paint paint;
                    BeginPaint(hwnd, out paint);
                    EndPaint(hwnd, ref paint);
                    handled = true;
                }
                ThomasWrapper.eventHandler(windowHandle, message, wparam, lparam);
            }
            catch (Exception e)
            {
                MessageBox.Show(e.Message);
            }

            return base.WndProc(hwnd, message, wparam, lparam, ref handled);
        }*/
    }
}
