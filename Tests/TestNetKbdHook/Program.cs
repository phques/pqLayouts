using System;
using System.Diagnostics;
using System.Windows.Forms;
using System.Runtime.InteropServices;
//using static System.Net.Mime.MediaTypeNames;
using Application = System.Windows.Forms.Application;
using System.Collections.Generic;

// Structure returned by the hook whenever a key is pressed
public struct KBDLLHOOKSTRUCT
{
    public int vkCode;
    public int scanCode;
    public int flags;
    public int time;
    public int dwExtraInfo;
}

// winuser.h
public enum KbdHookFlags : int
{
    Extended = 0x01,
    Injected = 0x10,
    AltDown = 0x20,
    Up = 0x00000080,
    LowerIlInjected = 0x02
}


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
    public ushort VirtKeys;
    public ushort wScan;
    public KeyboardInputFlags Flags;
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

class InterceptKeys
{
    private const int WH_KEYBOARD_LL = 13;
    private const int WM_KEYDOWN = 0x0100;
    private static LowLevelKeyboardProc _proc = HookCallback;
    private static IntPtr _hookID = IntPtr.Zero;

    public static void Main()
    {
        _hookID = SetHook(_proc);
        Application.Run();
        Console.WriteLine("unhooking");
        UnhookWindowsHookEx(_hookID);
    }

    private static IntPtr SetHook(LowLevelKeyboardProc proc)
    {
        using (Process curProcess = Process.GetCurrentProcess())
        using (ProcessModule curModule = curProcess.MainModule)
        {
            return SetWindowsHookEx(WH_KEYBOARD_LL, proc,
                GetModuleHandle(curModule.ModuleName), 0);
        }
    }

    private delegate IntPtr LowLevelKeyboardProc(
        int nCode, IntPtr wParam, ref KBDLLHOOKSTRUCT lParam);

    private static IntPtr HookCallback(
        int nCode, IntPtr wParam, ref KBDLLHOOKSTRUCT lParam)
    {
        if (nCode >= 0 && lParam.dwExtraInfo != 0x130466)// && wParam == (IntPtr)WM_KEYDOWN)
        {
            //int vkCode = Marshal.ReadInt32(lParam);
            Console.WriteLine((Keys)lParam.vkCode);
            if (lParam.vkCode >= 'A' && lParam.vkCode < 'Z')
            {
                bool up = (lParam.flags & (int)KbdHookFlags.Up) != 0;

                Console.WriteLine("A-Z");

                var inputs = new List<INPUT>();
                var input = new INPUT(SendInputType.InputKeyboard);
                input.Union.Keyboard.VirtKeys = (ushort)(lParam.vkCode + 1);
                input.Union.Keyboard.Flags |= up ? KeyboardInputFlags.KeyUp : KeyboardInputFlags.KeyDown;
                input.Union.Keyboard.dwExtraInfo = (IntPtr)0x130466;

                inputs.Add(input);
                DispatchInput(inputs);

                return (IntPtr)1;
            }
        }
        return CallNextHookEx(_hookID, nCode, wParam, ref lParam);
    }

    public static void DispatchInput(List<INPUT> inputs)
    {
        if (inputs == null) throw new ArgumentNullException("inputs");
        if (inputs.Count == 0) throw new ArgumentException("The input array was empty", "inputs");
        var successful = SendInput((uint)inputs.Count, inputs.ToArray(), Marshal.SizeOf(typeof(INPUT)));
        if (successful != inputs.Count)
        {
            Console.WriteLine("last error = " + GetLastError());
        }
        //    throw new Exception("Some simulated input commands were not sent successfully. The most common reason for this happening are the security features of Windows including User Interface Privacy Isolation (UIPI). Your application can only send commands to applications of the same or lower elevation. Similarly certain commands are restricted to Accessibility/UIAutomation applications. Refer to the project home page and the code samples for more information.");
    }


    [DllImport("user32.dll", CharSet = CharSet.Auto, SetLastError = true)]
    private static extern IntPtr SetWindowsHookEx(int idHook,
        LowLevelKeyboardProc lpfn, IntPtr hMod, uint dwThreadId);

    [DllImport("user32.dll", CharSet = CharSet.Auto, SetLastError = true)]
    [return: MarshalAs(UnmanagedType.Bool)]
    private static extern bool UnhookWindowsHookEx(IntPtr hhk);

    [DllImport("user32.dll", CharSet = CharSet.Auto, SetLastError = true)]
    private static extern IntPtr CallNextHookEx(IntPtr hhk, int nCode,
        IntPtr wParam, ref KBDLLHOOKSTRUCT lParam);

    [DllImport("kernel32.dll", CharSet = CharSet.Auto, SetLastError = true)]
    private static extern IntPtr GetModuleHandle(string lpModuleName);

    [DllImport("user32.dll", SetLastError = true)]
    public static extern Int16 GetAsyncKeyState(UInt16 virtualKeyCode);

    [DllImport("user32.dll", SetLastError = true)]
    public static extern Int16 GetKeyState(UInt16 virtualKeyCode);

    //[DllImport("user32.dll", SetLastError = true)]
    //public static extern UInt32 SendInput(UInt32 numberOfInputs, INPUT[] inputs, Int32 sizeOfInputStructure);
    [DllImport("user32.dll", SetLastError = true)]
    internal static extern uint SendInput(uint nInputs, [In, MarshalAs(UnmanagedType.LPArray)] INPUT[] pInputs, int cbSize);

    [DllImport("kernel32.dll", SetLastError = false)]
    internal static extern uint GetLastError();
}