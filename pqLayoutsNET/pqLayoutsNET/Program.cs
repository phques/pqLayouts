using System;
using LlKbdHookNet;
using SendInputNET;
using Native;
using System.Windows.Forms;
using System.Runtime.InteropServices;
using static System.Net.Mime.MediaTypeNames;

namespace pqLayoutsNET
{

    internal class Program
    {
        private const int MyInjection = 0x130466;
        SendInput SendInput;
        Layer Layer;

        public Program()
        {
            SendInput = new SendInput(MyInjection);
        }

        // Return true to pass-through the event to the next handler.
        public bool KeyboardHookEventHandler(KbdLLHookStruct e)
        {
            if (e.IsInjectedBy(MyInjection))
            {
                return true;
            }

            // get current state of shift
            ushort shiftKeyCode = VkUtil.KeyCode(Keys.ShiftKey);
            short shiftState = Methods.GetAsyncKeyState(shiftKeyCode);
            bool shift = (shiftState & 0x8000) != 0;

            // get mapped key
            VeeKee inKey = new VeeKee((ushort)e.k.vkCode, false, false, false);
            VeeKee outKey = Layer.Mapping(inKey, shift);

            // output mapped key if found
            if (outKey.Code != 0)
            {
                SendInput.SendVk(outKey.Code, e.IsKeyDown);
                return false;
            }

            return true;
        }

        static void Main(string[] args)
        {
            Program p = new Program();

            YamlLoader yamlLoader = new YamlLoader(args[0]);
            p.Layer = yamlLoader.Load();


            KbdLLHook.SetHook(p.KeyboardHookEventHandler);
            System.Windows.Forms.Application.Run();

            KbdLLHook.UnHook();

        }
    }
}
