using System;
using System.Runtime.InteropServices;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace TestInterop
{
    public struct KeyDef
    {
        public uint vk;       // VeeKee = DWORD = unsigned long
        public uint scancode;  // UINT
    }

    public struct KeyValuePacked
    {
        //public KeyDef keydef;
        public uint vk;       // VeeKee = DWORD = unsigned long
        public uint scancode;  // UINT

        public int shift;   // bool
        public int control; // bool
        public int alt;     // bool
    }

    internal static class PqLayoutsHookDll
    {
        [DllImport("PqLayoutsHook.dll", CallingConvention = CallingConvention.Cdecl)]
        internal static extern bool AddMappingP(KeyValuePacked from, KeyValuePacked to);

        [DllImport("PqLayoutsHook.dll", CallingConvention = CallingConvention.Cdecl)]
        internal static extern bool HookKbdLL();

        [DllImport("PqLayoutsHook.dll", CallingConvention = CallingConvention.Cdecl)]
        internal static extern bool UnhookKbdLL();
    }

    internal class Program
    {
        static void Main(string[] args)
        {
            KeyValuePacked from = default;
            KeyValuePacked to = default;
            from.vk = 'A';
            to.vk = 'B';
            to.shift = 1;
            //to.alt = 1;

            bool ret = PqLayoutsHookDll.AddMappingP(from, to);
            ret = PqLayoutsHookDll.HookKbdLL();

            Console.WriteLine("hook()=" + ret + ", waiting for key");
            Console.ReadLine();

            ret = PqLayoutsHookDll.UnhookKbdLL();
            Console.WriteLine("unhook=" + ret);

        }
    }
}
