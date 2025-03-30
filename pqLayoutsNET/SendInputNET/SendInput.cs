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

        public uint Send(List<INPUT> inputs)
        {
            return Methods.SendInput((uint)inputs.Count, inputs.ToArray(), Marshal.SizeOf(typeof(INPUT)));
        }

        public void SendVk(ushort vkCode, bool down, bool extended = false)
        {
            INPUT input = INPUT.NewKbdInput(vkCode, down, extended);
            if (Injection != 0)
            {
                input.Union.Keyboard.dwExtraInfo = (IntPtr)Injection;
            }
            Send(new List<INPUT> { input });
        }

        private readonly int Injection;
    }
}
