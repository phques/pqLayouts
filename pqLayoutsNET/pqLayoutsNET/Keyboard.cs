using LlKbdHookNet;
using Native;
using SendInputNET;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Forms;

namespace pqLayoutsNET
{
    public class Keyboard
    {
        internal class KbdState
        {
            internal bool ShiftDown;
            internal bool CapsToggled;
        }

        private const int MyInjection = 0x130466;
        private readonly SendInput SendInput;
        private readonly Layer Layer;
        private readonly INPUT ShiftDownInput;
        private readonly INPUT ShiftUpInput;
        private KbdState State = new KbdState();

        public Keyboard(Layer layer)
        {
            SendInput = new SendInput(MyInjection);
            Layer = layer;

            CreateInputsForKey(Keys.ShiftKey, out ShiftDownInput, out ShiftUpInput);
        }

        private void CreateInputsForKey(Keys key, out INPUT down, out INPUT up)
        {
            VeeKee vk = new VeeKee(key);
            down = INPUT.NewKbdInput(vk.Code, true, vk.IsExtended);
            up = INPUT.NewKbdInput(vk.Code, false, vk.IsExtended);

            down.Union.Keyboard.dwExtraInfo = (IntPtr)MyInjection;
            up.Union.Keyboard.dwExtraInfo = (IntPtr)MyInjection;
        }

        public void Hook()
        {
            KbdLLHook.SetHook(KeyboardHookEventHandler);
        }

        public void UnHook()
        {
            KbdLLHook.UnHook();
        }

        private bool HandleMappedKey(VeeKee inKey, VeeKee outKey, KbdLLHookStruct e)
        {
            List<INPUT> prefixes = new List<INPUT>();
            List<INPUT> suffixes = new List<INPUT>();

            // does this key need shift?
            if (outKey.Shift && !State.ShiftDown && !(outKey.IsAlpha && State.CapsToggled))
            {
                prefixes.Add(ShiftDownInput);
                suffixes.Add(ShiftUpInput);
            }

            // do we need to *remove* shift?
            if (!outKey.Shift && State.ShiftDown && !outKey.IsAlpha)
            {
                prefixes.Add(ShiftUpInput);
                suffixes.Add(ShiftDownInput);
            }

            INPUT outKeyInput = INPUT.NewKbdInput(outKey.Code, e.IsKeyDown, outKey.IsExtended);

            // concat prefixes + outkey + suffixes
            var outInputs = new List<INPUT>();
            outInputs.AddRange(prefixes);
            outInputs.Add(outKeyInput);
            outInputs.AddRange(suffixes);

            SendInput.Send(outInputs, true);
            return false;
        }


        // Return true to pass-through the event to the next handler.
        public bool KeyboardHookEventHandler(KbdLLHookStruct e)
        {
            if (e.IsInjectedBy(MyInjection))
            {
                // pass-through to the next handler
                return true;
            }

            // get current state of shift, caps
            
            State.ShiftDown = VkUtil.IsKeyDown(Keys.ShiftKey);
            State.CapsToggled = VkUtil.IsKeyToggled(Keys.CapsLock);

            // get mapped key
            VeeKee inKey = new VeeKee((ushort)e.k.vkCode, false, false, false);
            VeeKee outKey = Layer.Mapping(inKey, State.ShiftDown);

            // output mapped key if found
            if (outKey.Code != 0)
            {
                return HandleMappedKey(inKey, outKey, e);
            }

            // pass-through to the next handler
            return true;
        }

    }
}
