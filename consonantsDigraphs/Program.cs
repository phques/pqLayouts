// Copyright 2021 Philippe Quesnel  
//
// This file is part of pqLayouts.
//
// pqLayouts is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
// 
// pqLayouts is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
// 
// You should have received a copy of the GNU General Public License
// along with pqLayouts.  If not, see <http://www.gnu.org/licenses/>.

using System;
using System.Collections.Generic;
using System.Diagnostics;
using System.Linq;
using System.Reflection.Metadata;

// small program to find highest frequency digraphs of consonants
// combines two sets of consonants (from cmdline), creating digraphs from all permutations

namespace consonantsDigraphs
{
    public class DigraphValue
    {
        public DigraphValue(string di, int val)
        {
            Digraph = di;
            Value = val;
        }

        public string Digraph;
        public int Value;
    }

    public class TwoDigraphs
    {
        public TwoDigraphs(DigraphValue di1, DigraphValue di2)
        {
            Digraph1 = di1;
            Digraph2 = di2;
            TotalValue = (di1?.Value ?? 0) + (di2?.Value ?? 0);
        }
        public int TotalValue;
        public DigraphValue Digraph1;
        public DigraphValue Digraph2;
    }

    public class Doit
    {
        readonly Dictionary<string, DigraphValue> digraphs;

        public Doit()
        {
            digraphs = new Dictionary<string, DigraphValue>();
            ReadDigraphs();
        }

        public void ReadDigraphs()
        {
            var lines = System.IO.File.ReadAllLines("data/consonantsDigraphs.txt");
            foreach (var line in lines)
            {
                // create digraph value from "ng 1234"
                var parts = line.Split(' ');
                var digraphValue = new DigraphValue(parts[0], int.Parse(parts[1]));

                // add to dictio
                digraphs.Add(parts[0], digraphValue);
            }
        }

        internal void SortByFrequecy(string mainLayerConsonants, string altLayerConsonants)
        {
            var digraphList = new List<DigraphValue>();

            foreach (var charMain in mainLayerConsonants)
            {
                foreach (var charAlt in altLayerConsonants)
                {
                    // create & lookup 1st digraph from combination of the two consonants
                    var digraph = charMain.ToString() + charAlt.ToString();
                    digraphs.TryGetValue(digraph, out DigraphValue digraphValue1);
                    
                    // create dummy digraph value if !found
                    digraphValue1 ??= new DigraphValue(digraph, 0);

                    // create & lookup 2nd digraph from combination of the two consonants
                    digraph = charAlt.ToString() + charMain.ToString();
                    digraphs.TryGetValue(digraph, out var digraphValue2);

                    // create dummy digraph value if !found
                    digraphValue2 ??= new DigraphValue(digraph, 0);

                    // actually, try to output both separately
                    //var twoDigraphs = new TwoDigraphs(digraphValue1, digraphValue2);
                    //digraphList.Add(twoDigraphs);
                    digraphList.Add(digraphValue1);
                    digraphList.Add(digraphValue2);
                }
            }

            // sort list descending

            var sorted = from element in digraphList
                         orderby element.Value descending
                         select element;

            // output results
            foreach (var digraphValue in sorted)
            {
                string digraph1 = digraphValue?.Digraph ?? "";
                int digraphValue1 = digraphValue?.Value ?? 0;

                Console.WriteLine("[{0}] {1}", digraph1, digraphValue1);
            }
        }

        /*
         * this expects a consonants cluster of 8 keys, 4x2 (expected to be on right hand side!)
         * First 3 params are the same length
         */
        internal void GenerateOneSetDigraphChords(
            string consonantsLayer1, string consonantsLayer2, string rightStenoKeys, 
            string leftStenoKey1, string leftStenoKey2, string leftStenoKeyVert)
        {
            for (int i = 0; i < consonantsLayer1.Length; i++)
            {
                for (int j = 0; j < consonantsLayer2.Length; j++)
                {
                    int layer1X = i % 4;
                    int layer1Y = i / 4;
                    int layer2X = j % 4;
                    int layer2Y = j / 4;
                    // same finger, use both left hand chord keys with single right hand key
                    if (i == j)
                    {
                        Console.Write(" {0}{1}-{2}", leftStenoKey1, leftStenoKey2, rightStenoKeys[i]);
                        Console.Write(" {0}{1}  ", consonantsLayer1[i], consonantsLayer2[j]);
                        Console.WriteLine("");
                    }
                    else
                    {
                        // layer1 to layer2
                        string leftStenoKey;
                        // vertical, use left 1st/2nd + leftVert + single right hand key
                        if (layer1X == layer2X)
                        {
                            leftStenoKey = layer1Y < layer2Y ? leftStenoKey1 : leftStenoKey2;

                            Console.Write(" {0}{1}-{2}", leftStenoKey, leftStenoKeyVert, rightStenoKeys[i]);
                            Console.Write(" {0}{1}  ", consonantsLayer1[i], consonantsLayer2[j]);
                            Console.WriteLine("");
                        }
                        else
                        {
                            leftStenoKey = layer1X < layer2X ? leftStenoKey1 : leftStenoKey2;

                            Console.Write(" {0}-{1}{2}", leftStenoKey, rightStenoKeys[i], rightStenoKeys[j]);
                            Console.Write(" {0}{1}  ", consonantsLayer1[i], consonantsLayer2[j]);
                            Console.WriteLine("");

                        }

                    }
                }
            }
        }

        internal void GenerateDigraphChords(string mainLayerConsonants, string altLayerConsonants, string rightStenoKeys, 
            string leftStenoKey1, string leftStenoKey2, string leftStenoKeyVert,
            string leftStenoKeyAlt1, string leftStenoKeyAlt2, string leftStenoKeyVertAlt)
        {
            Console.WriteLine("! main layer consonants digraphs");
            Console.WriteLine("steaks");
            GenerateOneSetDigraphChords(mainLayerConsonants, altLayerConsonants,
                rightStenoKeys, leftStenoKey1, leftStenoKey2, leftStenoKeyVert);
            Console.WriteLine("endsteaks");

            Console.WriteLine("\n");
            Console.WriteLine("! alt layer consonants digraphs");
            Console.WriteLine("steaks");
            GenerateOneSetDigraphChords(altLayerConsonants, mainLayerConsonants,
                rightStenoKeys, leftStenoKeyAlt1, leftStenoKeyAlt2, leftStenoKeyVertAlt);
            Console.WriteLine("endsteaks");
        }

        class Program
        {

            static void Main(string[] args)
            {
                if (args.Length == 2)
                {
                    var mainLayerConsonants = args[0];
                    var altLayerConsonants = args[1]; 

                    var doit = new Doit();
                    doit.SortByFrequecy(mainLayerConsonants, altLayerConsonants);
                }
                else if (args.Length == 9)
                {
                    var mainLayerConsonants = args[0];
                    var altLayerConsonants = args[1]; 
                    var rightStenoKeys = args[2]; 
                    var leftStenoKey1 = args[3]; 
                    var leftStenoKey2 = args[4]; 
                    var leftStenoKeyVert = args[5]; 
                    var leftStenoKey1Alt = args[6]; 
                    var leftStenoKey2Alt = args[7]; 
                    var leftStenoKeyVertAlt = args[8]; 

                    var doit = new Doit();
                    doit.GenerateDigraphChords(mainLayerConsonants, altLayerConsonants, rightStenoKeys, 
                        leftStenoKey1, leftStenoKey2, leftStenoKeyVert,
                        leftStenoKey1Alt, leftStenoKey2Alt, leftStenoKeyVertAlt);
                }
                else
                {
                    Console.WriteLine("arguments:");
                    Console.WriteLine(" mainLayerConsonants altLayerConsonants");
                    Console.WriteLine("or ");
                    Console.WriteLine(" mainLayerConsonants altLayerConsonants rightStenoKeys leftStenoKey1st leftStenoKey2nd leftStenoKeyVert altLeftStenoKey1st altLeftStenoKey2nd altLeftStenoKeyVert");
                }

            }
        }
    }
}
