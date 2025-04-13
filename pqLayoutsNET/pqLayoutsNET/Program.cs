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
        public Program()
        {
        }

        static void Main(string[] args)
        {
            TestKbHook(args);
        }

        private static void TestKbHook(string[] args)
        {
            Program p = new Program();

            YamlLoader yamlLoader = new YamlLoader(args[0]);
            Layer layer = yamlLoader.Load();

            Keyboard keyboard = new Keyboard(layer);
            keyboard.Hook();

            System.Windows.Forms.Application.Run();

            keyboard.UnHook();
        }
    }
}
