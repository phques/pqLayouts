using Native;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Forms;

namespace pqLayoutsNET
{
    public class VkUtil
    {
        static public Keys CharToKeys(char c)
        {
            // Convert the character to a virtual key code
            Int16 vk = Methods.VkKeyScanA(c);

            // Check if the character is a valid key
            if (vk == -1)
            {
                return Keys.None;
            }

            // Extract the high byte (modifiers) and low byte (virtual key code)
            UInt32 modifiers = ((UInt32)vk & 0xFF00) << 8;
            UInt32 keyCode = ((UInt32)vk & 0x00FF);

            return (System.Windows.Forms.Keys)(modifiers | keyCode);
        }
    }
}
