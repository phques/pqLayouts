using Native;
using System;
using System.Collections.Generic;
using System.Runtime.InteropServices;

namespace SendInputNET
{
    public class SendInput
    {
        public SendInput(int injection)
        {
            Injection = injection;
        }

        public uint Send(List<INPUT> inputs, bool setInjection = false)
        {
            var array = inputs.ToArray();
            if (setInjection)
            {
                for (int i = 0; i < array.Length; i++)
                {
                    array[i].Union.Keyboard.dwExtraInfo = (IntPtr)Injection;
                }
            }
            return Methods.SendInput((uint)inputs.Count, array, Marshal.SizeOf(typeof(INPUT)));
        }

        public void SendVk(ushort vkCode, bool down, bool extended = false)
        {
            INPUT input = INPUT.NewKbdInput(vkCode, down, extended);
            input.Union.Keyboard.dwExtraInfo = (IntPtr)Injection;

            Send(new List<INPUT> { input });
        }

        private readonly int Injection;
    }
}
