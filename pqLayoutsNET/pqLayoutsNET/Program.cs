using System;
using LlKbdHookNet;
using SendInputNET;

namespace pqLayoutsNET
{
    internal class Program
    {
        private const int MyInjection = 0x130466;
        SendInput SendInput;

        public Program()
        {
            SendInput = new SendInput(MyInjection);
        }

        public bool KeyboardHookEventHandler(KbdLLHookStruct e)
        {
            if (e.IsInjectedBy(MyInjection))
            {
                return true;
            }

            if (e.k.vkCode >= 'A' && e.k.vkCode <= 'Z')
            {
                Console.WriteLine("A-Y --> B-Z");

                SendInput.SendVk((ushort)(e.k.vkCode + 1), e.IsKeyDown);

                return false;
            }

            return true;
        }


        static void Main(string[] args)
        {
            Program p = new Program();
            KbdLLHook.SetHook(p.KeyboardHookEventHandler);
            System.Windows.Forms.Application.Run();
            KbdLLHook.UnHook();
        }
    }
}
