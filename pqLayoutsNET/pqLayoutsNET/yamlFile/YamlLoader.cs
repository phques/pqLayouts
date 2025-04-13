using Native;
using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using YamlDotNet.Serialization;
using YamlDotNet.Serialization.NamingConventions;

namespace pqLayoutsNET
{
    public class YamlLoader
    {
        public YamlLoader(string filename)
        {
            Filename = filename;
        }

        public Layer Load()
        {
            LayoutIn = LoadYaml();
            if (LayoutIn == null)
            {
                return null;
            }

            Layer layer = GetLayer("main");
            return layer;
        }

        private List<string[]> SplitKeyboardChars(string lines)
        {
            List<string[]> rows = new List<string[]>();

            // split lines
            string[] linesArray = lines.Split(new[] { '\n' }, StringSplitOptions.RemoveEmptyEntries);

            foreach (var line in linesArray)
            {
                // split keys
                var keys = line.Split(new[] {' ', '\t'}, StringSplitOptions.RemoveEmptyEntries);
                rows.Add(keys.ToArray());
            }

            return rows;
        }

        private bool Compare(List<string[]> rowsTemplate, List<string[]> rows, string layerName)
        {
            if (rowsTemplate.Count != rows.Count)
            {
                Console.WriteLine($"layer {layerName} has {rows.Count} rows, expecting {rowsTemplate.Count}");
                return false;
            }

            for (int i = 0; i < rowsTemplate.Count; i++)
            {
                if (rowsTemplate[i].Length != rows[i].Length)
                {
                    Console.WriteLine($"layer {layerName}'s row {i + 1} has {rows[i].Length} chars, expecting {rowsTemplate[i].Length}");
                    return false;
                }
            }

            return true;
        }

        public Layer GetLayer(string layerName)
        {
            LayerIn layerIn = LayoutIn.Layers.Find(x => x.Name == layerName);
            if (layerIn == null)
            {
                Console.WriteLine($"Layer '{layerName}' not found in file '{Filename}'");
                return null;
            }

            Layer layer = new Layer(layerIn.Name);

            List<string[]> linesFrom = SplitKeyboardChars(layerIn.From);
            List<string[]> linesTo = SplitKeyboardChars(layerIn.To);
            List<string[]> linesShift = SplitKeyboardChars(layerIn.Shift);

            if (!Compare(linesFrom, linesTo, layerName))
                return null;

            if (!Compare(linesFrom, linesShift, layerName + " Shift"))
                return null;

            for (int i = 0; i < linesFrom.Count; i++)
            {
                for (int j = 0; j < linesFrom[i].Length; j++)
                {
                    var fromKey = linesFrom[i][j];
                    var toKey = linesTo[i][j];
                    var shiftKey = linesShift[i][j];

                    var fromVk = new VeeKee(fromKey[0]);
                    var toVk = new VeeKee(toKey[0]);
                    var shiftVk = new VeeKee(shiftKey[0]);

                    layer.AddMapping(fromVk, toVk);
                    layer.AddMappingShifted(fromVk, shiftVk);

                    //##pq debug
                    Console.WriteLine($"{fromKey} -> {toKey} {shiftKey}");
                }
            }

            return layer;
        }

        private LayoutIn LoadYaml()
        {
            var deserializer = new DeserializerBuilder()
                .WithNamingConvention(CamelCaseNamingConvention.Instance)
                .Build();
            try
            {
                using (var reader = new StreamReader(Filename))
                {
                    return deserializer.Deserialize<LayoutIn>(reader);
                }
            }
            catch (Exception e)
            {
                string inner = e.InnerException?.Message ?? "";
                Console.WriteLine($"Error reading file '{Filename}': {e.Message}\n{inner}");
            }

            // failed
            return null;
        }

        private readonly string Filename;
        private LayoutIn LayoutIn;
    } 

}