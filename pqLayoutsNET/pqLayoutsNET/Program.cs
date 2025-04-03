using System;
using LlKbdHookNet;
using SendInputNET;
using Native;
using System.Windows.Forms;


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
            //KbdLLHook.SetHook(p.KeyboardHookEventHandler);
            //System.Windows.Forms.Application.Run();
            //KbdLLHook.UnHook();

            Keys keyCode = 0;
            keyCode = VkUtil.CharToKeys('t');
            keyCode = VkUtil.CharToKeys('T');
            keyCode = VkUtil.CharToKeys(';');
            keyCode = VkUtil.CharToKeys('*');
            keyCode = VkUtil.CharToKeys('\n');
            keyCode = VkUtil.CharToKeys('\b');
            keyCode = VkUtil.CharToKeys('\a');
            keyCode = 0;
        }
    }
}
