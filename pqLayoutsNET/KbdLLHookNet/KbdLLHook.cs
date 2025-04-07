using System;
using System.Diagnostics;
using Native;

namespace LlKbdHookNet
{
    public class KbdLLHook
    {
        /// <summary>
        /// Delegate for KeyboardHook event handling called from this hook.
        /// </summary>
        /// <param name="e">the KBDLLHOOKSTRUCT from the keyboard hook</param>
        /// <returns>true to pass the event to the next handler</returns>
        public delegate bool KeyboardHookEventHandler(KbdLLHookStruct kbdHookStruct);

        public static bool SetHook(KeyboardHookEventHandler eventHandler)
        {
            KeyIntercepted = eventHandler;

            _hookID = SetHook(_proc);
            return _hookID != IntPtr.Zero;
        }

        public static void UnHook()
        {
            Methods.UnhookWindowsHookEx(_hookID);
        }

        private static IntPtr SetHook(LowLevelKeyboardProc proc)
        {
            using (Process curProcess = Process.GetCurrentProcess())
            {
                using (ProcessModule curModule = curProcess.MainModule)
                {
                    IntPtr moduleHandle = Native.Methods.GetModuleHandle(curModule.ModuleName);

                    return Methods.SetWindowsHookEx(WH_KEYBOARD_LL, proc, moduleHandle, 0);
                }
            }
        }

        private static IntPtr HookCallback(int nCode, IntPtr wParam, ref KBDLLHOOKSTRUCT lParam)
        {
            if (nCode >= 0 && KeyIntercepted != null)
            {
                if (!KeyIntercepted(new KbdLLHookStruct(ref lParam)))
                {
                    return (IntPtr)1;
                }
            }

            return Methods.CallNextHookEx(_hookID, nCode, wParam, ref lParam);
        }

        /// <summary>
        /// Event triggered when a keystroke is intercepted by the 
        /// low-level hook.
        /// Return true to pass-through the event to the next handler.
        /// </summary>
        private static event KeyboardHookEventHandler KeyIntercepted;

        private const int WH_KEYBOARD_LL = 13;
        private const int WM_KEYDOWN = 0x0100;
        private static readonly LowLevelKeyboardProc _proc = HookCallback;
        private static IntPtr _hookID = IntPtr.Zero;
    }

    // utility class wrapping a KBDLLHOOKSTRUCT
    public class KbdLLHookStruct
    {
        public KbdLLHookStruct(ref KBDLLHOOKSTRUCT lParam)
        {
            k = lParam;
        }

        public bool IsKeyDown => (k.flags & (int)KbdLLHookFlags.Up) == 0;
        public bool IsKeyUp => (k.flags & (int)KbdLLHookFlags.Up) != 0;
        public bool IsExtented => (k.flags & (int)KbdLLHookFlags.Extended) != 0;
        public bool IsInjected => (k.flags & (int)KbdLLHookFlags.Injected) != 0;
        public bool IsInjectedBy(int injection) => IsInjected && k.dwExtraInfo == injection;
        public bool IsAltDown => (k.flags & (int)KbdLLHookFlags.AltDown) != 0;
        public bool IsLowerIlInjected => (k.flags & (int)KbdLLHookFlags.LowerIlInjected) != 0;

        public KBDLLHOOKSTRUCT k;
    }
}
