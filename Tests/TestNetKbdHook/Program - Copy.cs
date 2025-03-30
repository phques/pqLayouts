using System;
using System.Diagnostics;
using System.Windows.Forms;
using System.Runtime.InteropServices;
//using static System.Net.Mime.MediaTypeNames;
using Application = System.Windows.Forms.Application;

// Structure returned by the hook whenever a key is pressed
internal struct KBDLLHOOKSTRUCT
{
    public int vkCode;
    int scanCode;
    public int flags;
    int time;
    int dwExtraInfo;
}

internal struct KEYBDINPUT
{
    /// <summary>
    /// Specifies a virtual-key code. The code must be a value in the range 1 to 254. The Winuser.h header file provides macro definitions (VK_*) for each value. If the dwFlags member specifies KEYEVENTF_UNICODE, wVk must be 0. 
    /// </summary>
    public UInt16 KeyCode;

    /// <summary>
    /// Specifies a hardware scan code for the key. If dwFlags specifies KEYEVENTF_UNICODE, wScan specifies a Unicode character which is to be sent to the foreground application. 
    /// </summary>
    public UInt16 Scan;

    /// <summary>
    /// Specifies various aspects of a keystroke. This member can be certain combinations of the following values.
    /// KEYEVENTF_EXTENDEDKEY - If specified, the scan code was preceded by a prefix byte that has the value 0xE0 (224).
    /// KEYEVENTF_KEYUP - If specified, the key is being released. If not specified, the key is being pressed.
    /// KEYEVENTF_SCANCODE - If specified, wScan identifies the key and wVk is ignored. 
    /// KEYEVENTF_UNICODE - Windows 2000/XP: If specified, the system synthesizes a VK_PACKET keystroke. The wVk parameter must be zero. This flag can only be combined with the KEYEVENTF_KEYUP flag. For more information, see the Remarks section. 
    /// </summary>
    public UInt32 Flags;

    /// <summary>
    /// Time stamp for the event, in milliseconds. If this parameter is zero, the system will provide its own time stamp. 
    /// </summary>
    public UInt32 Time;

    /// <summary>
    /// Specifies an additional value associated with the keystroke. Use the GetMessageExtraInfo function to obtain this information. 
    /// </summary>
    public IntPtr ExtraInfo;
}
internal struct INPUT
{
    /// <summary>
    /// Specifies the type of the input event. This member can be one of the following values. 
    /// <see cref="InputType.Mouse"/> - The event is a mouse event. Use the mi structure of the union.
    /// <see cref="InputType.Keyboard"/> - The event is a keyboard event. Use the ki structure of the union.
    /// <see cref="InputType.Hardware"/> - Windows 95/98/Me: The event is from input hardware other than a keyboard or mouse. Use the hi structure of the union.
    /// </summary>
    public UInt32 Type;

    /// <summary>
    /// The data structure that contains information about the simulated Mouse, Keyboard or Hardware event.
    /// </summary>
    public KEYBDINPUT Data;
}

internal enum InputType : uint // UInt32
{
    /// <summary>
    /// INPUT_MOUSE = 0x00 (The event is a mouse event. Use the mi structure of the union.)
    /// </summary>
    Mouse = 0,

    /// <summary>
    /// INPUT_KEYBOARD = 0x01 (The event is a keyboard event. Use the ki structure of the union.)
    /// </summary>
    Keyboard = 1,

    /// <summary>
    /// INPUT_HARDWARE = 0x02 (Windows 95/98/Me: The event is from input hardware other than a keyboard or mouse. Use the hi structure of the union.)
    /// </summary>
    Hardware = 2,
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
        if (nCode >= 0 && wParam == (IntPtr)WM_KEYDOWN)
        {
            //int vkCode = Marshal.ReadInt32(lParam);
            Console.WriteLine((Keys)lParam.vkCode);
            if (lParam.vkCode >= 'A' && lParam.vkCode < 'Z')
            {
                Console.WriteLine("A-Z");

                INPUT[] inputs = new INPUT[1];
                inputs[0].Type = (uint)InputType.Keyboard;
                inputs[0].Data.KeyCode = (ushort)(lParam.vkCode + 1);
                DispatchInput(inputs);

                return (IntPtr)1;
            }
        }
        return CallNextHookEx(_hookID, nCode, wParam, ref lParam);
    }

    public static void DispatchInput(INPUT[] inputs)
    {
        if (inputs == null) throw new ArgumentNullException("inputs");
        if (inputs.Length == 0) throw new ArgumentException("The input array was empty", "inputs");
        var successful = SendInput((UInt32)inputs.Length, inputs, Marshal.SizeOf(typeof(INPUT)));
        //if (successful != inputs.Length)
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

}