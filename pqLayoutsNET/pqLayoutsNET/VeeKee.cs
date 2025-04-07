using System;
using System.Windows.Forms;

namespace pqLayoutsNET
{
    public struct VeeKee
    {
        public VeeKee(ushort code, bool shift, bool control, bool alt)
        {
            Code = code;
            Shift = shift;
            Control = control;
            Alt = alt;
        }

        public VeeKee(char c)
        {
            this = CharToVeeKee(c);
        }

        public VeeKee(Keys key)
        {
            Code = (ushort)(key & Keys.KeyCode);
            Shift = (key & Keys.Shift) != 0;
            Control = (key & Keys.Control) != 0;
            Alt = (key & Keys.Alt) != 0;
        }

        static public VeeKee CharToVeeKee(char c)
        {
            VeeKee vk = new VeeKee();

            if (VkUtil.CharToVk(c, out ushort keyCode, out ushort modifiers))
            {
                vk.Code = keyCode;
                vk.Shift = VkUtil.Shift(modifiers);
                vk.Control = VkUtil.Control(modifiers);
                vk.Alt = VkUtil.Alt(modifiers);
            }

            return vk;
        }

        public bool Shift;
        public bool Control;
        public bool Alt;
        public ushort Code;   // the "code" part in a Keys

    }
}
