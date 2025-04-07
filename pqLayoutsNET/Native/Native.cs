using System;
using System.Runtime.InteropServices;

namespace Native
{
    // These can be used with both:
    // - the high byte (0xFF00) of VkKeyScanA
    // - the high word (0xFFFF0000) of a System.Windows.Forms.Keys
    [Flags]
    public enum KeyModifierFlags : UInt16
    {
        Shift = 0x0001,
        Control = 0x0002,
        Alt = 0x0004,
    }

    #region SendInput

    [StructLayout(LayoutKind.Sequential)]
    public struct INPUT
    {
        public SendInputType InputType;
        public InputUnion Union;

        public INPUT(SendInputType type)
        {
            InputType = type;
            Union = new InputUnion();
        }

        public static INPUT NewKbdInput(ushort vkCode, bool down, bool extended = false)
        {
            INPUT input = new INPUT(SendInputType.InputKeyboard);
            input.Union.Keyboard.vkCode = vkCode;
            input.Union.Keyboard.flags =
                (down ? KeyboardInputFlags.KeyDown : KeyboardInputFlags.KeyUp) |
                (extended ? KeyboardInputFlags.ExtendedKey : 0);

            return input;
        }
    }

    public enum SendInputType : uint
    {
        InputMouse = 0,
        InputKeyboard = 1,
        InputHardware = 2
    }

    [StructLayout(LayoutKind.Explicit)]
    public struct InputUnion
    {
        [FieldOffset(0)]
        public MOUSEINPUT Mouse;

        [FieldOffset(0)]
        public KEYBDINPUT Keyboard;

        [FieldOffset(0)]
        public HARDWAREINPUT Hardware;
    }

    [StructLayout(LayoutKind.Sequential)]
    public struct MOUSEINPUT
    {
        public int dx;
        public int dy;
        public uint mouseData;
        public MouseEventdwFlags dwFlags;
        public uint time;
        public IntPtr dwExtraInfo;
    }

    // https://learn.microsoft.com/en-us/windows/win32/api/winuser/ns-winuser-keybdinput
    [StructLayout(LayoutKind.Sequential)]
    public struct KEYBDINPUT
    {
        public ushort vkCode;
        public ushort scanCode;
        public KeyboardInputFlags flags;
        public uint time;
        public IntPtr dwExtraInfo;
    }

    [StructLayout(LayoutKind.Sequential)]
    public struct HARDWAREINPUT
    {
        public int uMsg;
        public short wParamL;
        public short wParamH;
    }

    [Flags]
    public enum MouseEventdwFlags : uint
    {
        MOUSEEVENTF_MOVE = 0x0001,
        MOUSEEVENTF_LEFTDOWN = 0x0002,
        MOUSEEVENTF_LEFTUP = 0x0004,
        MOUSEEVENTF_RIGHTDOWN = 0x0008,
        MOUSEEVENTF_RIGHTUP = 0x0010,
        MOUSEEVENTF_MIDDLEDOWN = 0x0020,
        MOUSEEVENTF_MIDDLEUP = 0x0040,
        MOUSEEVENTF_XDOWN = 0x0080,
        MOUSEEVENTF_XUP = 0x0100,
        MOUSEEVENTF_WHEEL = 0x0800,
        MOUSEEVENTF_VIRTUALDESK = 0x4000,
        MOUSEEVENTF_ABSOLUTE = 0x8000
    }

    [Flags]
    public enum KeyboardInputFlags : uint
    {
        KeyDown = 0x0,
        ExtendedKey = 0x0001,
        KeyUp = 0x0002,
        Scancode = 0x0008,
        Unicode = 0x0004
    }

    #endregion

    #region Windows Hook

    // Structure returned by the hook whenever a key is pressed
    public struct KBDLLHOOKSTRUCT
    {
        public int vkCode;
        public int scanCode;
        public int flags;
        public int time;
        public int dwExtraInfo;
    }

    // winuser.h, values for the flags field of KBDLLHOOKSTRUCT
    public enum KbdLLHookFlags : int
    {
        Extended = 0x01,
        Injected = 0x10,
        AltDown = 0x20,
        Up = 0x80,
        LowerIlInjected = 0x02
    }

    public delegate IntPtr LowLevelKeyboardProc(int nCode, IntPtr wParam, ref KBDLLHOOKSTRUCT lParam);

    #endregion


    public class Methods
    {
        [DllImport("kernel32.dll", CharSet = CharSet.Auto, SetLastError = true)]
        public static extern IntPtr GetModuleHandle(string lpModuleName);


        [DllImport("kernel32.dll", SetLastError = false)]
        public static extern uint GetLastError();


        [DllImport("user32.dll", SetLastError = true)]
        public static extern Int16 GetAsyncKeyState(UInt16 virtualKeyCode);


        [DllImport("user32.dll", SetLastError = true)]
        public static extern Int16 GetKeyState(UInt16 virtualKeyCode);


        [DllImport("user32.dll", SetLastError = true)]
        public static extern uint SendInput(uint nInputs, [In, MarshalAs(UnmanagedType.LPArray)] INPUT[] pInputs, int cbSize);


        [DllImport("user32.dll", CharSet = CharSet.Auto, SetLastError = true)]
        public static extern IntPtr SetWindowsHookEx(int idHook, LowLevelKeyboardProc lpfn, IntPtr hMod, uint dwThreadId);


        [DllImport("user32.dll", CharSet = CharSet.Auto, SetLastError = true)]
        [return: MarshalAs(UnmanagedType.Bool)]
        public static extern bool UnhookWindowsHookEx(IntPtr hhk);


        [DllImport("user32.dll", CharSet = CharSet.Auto, SetLastError = true)]
        public static extern IntPtr CallNextHookEx(IntPtr hhk, int nCode,
            IntPtr wParam, ref KBDLLHOOKSTRUCT lParam);

        // https://learn.microsoft.com/en-us/windows/win32/api/winuser/nf-winuser-vkkeyscana
        [DllImport("user32.dll", CharSet = CharSet.Auto, SetLastError = false)]
        public static extern Int16 VkKeyScanA(char ch);
    }


}