using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using System.Linq.Expressions;

namespace TextToKbdlo
{
    internal class KbdLayer
    {
        internal readonly string Name;
        internal List<string[]> Rows = new List<string[]>();

        internal KbdLayer(string name)
        {
            Name = name;
        }

        internal void Read()
        {
            // skip initial empty lines (they are required !)
            string line;
            do
            {
                line = Console.ReadLine()?.Trim();
                if (!string.IsNullOrEmpty(line))
                    break;

            }
            while (true);

            while (!string.IsNullOrEmpty(line))
            {
                // split keys
                var keys = line.Split(' ').Where(k => !string.IsNullOrEmpty(k));

                Rows.Add(keys.ToArray());


                // get next row
                line = Console.ReadLine();
                line = line?.Trim();
            }
        }

        internal bool Compare(KbdLayer layer)
        {
            if (layer.Rows.Count != Rows.Count)
            {
                Console.WriteLine($"layer {layer.Name} has {layer.Rows.Count} rows, expecting {Rows.Count}");
                return false;
            }

            for (int i = 0; i < Rows.Count; i++)
            {
                if (layer.Rows[i].Length != Rows[i].Length)
                {
                    Console.WriteLine($"layer {layer.Name}'s row {i+1} has {layer.Rows[i].Length} chars, expecting {Rows[i].Length}");
                    return false;
                }
            }

            return true;
        }

        internal void Output(KbdLayer main, KbdLayer shift)
        {
            for (int i = 0; i < Rows.Count; i++)
            {
                for (int j = 0; j < Rows[i].Length; j++)
                {
                    var templateKey = Rows[i][j];
                    var mainKey = main.Rows[i][j];
                    var shiftKey = shift.Rows[i][j];
                    Console.WriteLine($"K2K {templateKey}  {mainKey} {shiftKey}");
                }

                Console.WriteLine("");
            }
        }
    }

    class Program
    {
        static void Main(string[] args)
        {
            if (args.Length != 1)
            {
                Console.WriteLine("expected parameter: filename of text file to convert");
                return;
            }
            //using (var file = File.OpenRead(args[0]))
            Console.SetIn(File.OpenText(args[0]));

            KbdLayer template = new KbdLayer("template");
            template.Read();

            KbdLayer main = new KbdLayer("main");
            main.Read();

            KbdLayer shift = new KbdLayer("shift");
            shift.Read();

            if (!template.Compare(main))
                return;

            if (!template.Compare(shift))
                return;

            Console.WriteLine($"! Generated from [{args[0]}]");
            template.Output(main, shift);
        }
    }
}
