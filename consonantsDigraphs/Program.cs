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
using System.Linq;

// small program to find highest frequency bigraphs of consonants
// combines two sets of consonants (from cmdline), creating digraphs from all permutations

namespace consonantsDigraphs
{
    public class DigraphValue
    {
        public DigraphValue(string di, int val)
        {
            digraph = di;
            value = val;
        }

        public string digraph;
        public int value;
    }

    public class TwoDigraphs
    {
        public TwoDigraphs(DigraphValue di1, DigraphValue di2)
        {
            digraph1 = di1;
            digraph2 = di2;
            TotalValue = (di1?.value ?? 0) + (di2?.value ?? 0);
        }
        public int TotalValue;
        public DigraphValue digraph1;
        public DigraphValue digraph2;
    }

    public class Doit
    {
        Dictionary<string, DigraphValue> digraphs;

        public Doit()
        {
            digraphs = new Dictionary<string, DigraphValue>();
            ReadDigraphs();
        }

        public void ReadDigraphs()
        {
            var lines = System.IO.File.ReadAllLines("consonantsDigraphs.txt");
            foreach (var line in lines)
            {
                // create digraph value from "ng 1234:
                var parts = line.Split(' ');
                var digraphValue = new DigraphValue(parts[0], int.Parse(parts[1]));
                // add to dictio
                digraphs.Add(parts[0], digraphValue);
            }
        }

        internal void Go(string mainLayerConsonants, string altLayerConsonants)
        {
            var twoDigraphList = new List<TwoDigraphs>();

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

                    var twoDigraphs = new TwoDigraphs(digraphValue1, digraphValue2);
                    twoDigraphList.Add(twoDigraphs);
                }
            }

            // sort list descending

            var sorted = from element in twoDigraphList
                         orderby element.TotalValue descending
                         select element;

            // output results
            foreach (var twoDigraphs in sorted)
            {
                string digraph1 = twoDigraphs.digraph1?.digraph ?? "";
                string digraph2 = twoDigraphs.digraph2?.digraph ?? "";
                int digraphValue1 = twoDigraphs.digraph1?.value ?? 0;
                int digraphValue2 = twoDigraphs.digraph2?.value ?? 0;

                Console.WriteLine("[{0}]/[{1}] {2} ({3}/{4})",
                    digraph1, digraph2, twoDigraphs.TotalValue, digraphValue1, digraphValue2);
            }
        }
    }

    class Program
    {

        static void Main(string[] args)
        {
            if (args.Length != 2)
            {
                Console.WriteLine("arguments: mainLayerConsonants altLayerConsonants");
                return;
            }

            var mainLayerConsonants = args[0]; //"dnrjftshc";
            var altLayerConsonants = args[1]; //"glbqzvpmwk";

            var doit = new Doit();
            doit.Go(mainLayerConsonants, altLayerConsonants);
        }
    }
}
