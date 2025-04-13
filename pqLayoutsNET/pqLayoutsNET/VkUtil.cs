using Native;
using System;
using System.Windows.Forms;

namespace pqLayoutsNET
{
    public class VkUtil
    {
        static public bool CharToVk(char c, out ushort keyCode, out ushort modifiers)
        {
            keyCode = modifiers = 0;

            // Convert the character to a virtual key code
            Int16 vk = Methods.VkKeyScanA(c);

            // Check if the character is a valid key
            if (vk == -1)
            {
                return false;
            }

            // Extract the high byte (modifiers) and low byte (virtual key code)
            modifiers = (ushort)((vk & 0xFF00) >> 8);
            keyCode = (ushort)(vk & 0x00FF);

            return true;
        }

        static public Keys CharToKeys(char c)
        {
            if (!CharToVk(c, out ushort keyCode, out ushort modifiers))
            {
                return Keys.None;
            }

            return (System.Windows.Forms.Keys)(((uint)modifiers << 16) | keyCode);
        }

        static public bool Shift(uint modifiers)
        {
            return (modifiers & (uint)KeyModifierFlags.Shift) != 0;
        }

        static public bool Control(uint modifiers)
        {
            return (modifiers & (uint)KeyModifierFlags.Control) != 0;
        }

        static public bool Alt(uint modifiers)
        {
            return (modifiers & (uint)KeyModifierFlags.Alt) != 0;
        }

        static public ushort KeyCode(Keys key)
        {
            return (ushort)(key & Keys.KeyCode);
        }

        static public void VkToScanCode(uint vk, out uint scanCode, out uint scanCodePrefix)
        {
            uint code = Methods.MapVirtualKeyA(vk, MapVkFlags.VK_TO_VSC_EX);

            scanCode = code & 0xFF;
            scanCodePrefix = (code & 0xFF00) >> 8; // 0xE0, 0xE1 indicate extended key
        }

        static public bool IsKeyDown(ushort vkCode)
        {
            return (Methods.GetAsyncKeyState(vkCode) & (ushort)AsyncKeyStateFlags.Down) != 0;
        }

        static public bool IsKeyToggled(ushort vkCode)
        {
            return (Methods.GetAsyncKeyState(vkCode) & (ushort)AsyncKeyStateFlags.Toggled) != 0;
        }

        static public bool IsKeyDown(Keys key)
        {
            return IsKeyDown(KeyCode(key));
        }

        static public bool IsKeyToggled(Keys key)
        {
            return IsKeyToggled(KeyCode(key));
        }
    }
}
