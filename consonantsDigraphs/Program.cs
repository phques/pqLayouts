﻿// Copyright 2021 Philippe Quesnel  
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
            var lines = System.IO.File.ReadAllLines("consonantsDigraphs.txt");
            foreach (var line in lines)
            {
                // create digraph value from "ng 1234"
                var parts = line.Split(' ');
                var digraphValue = new DigraphValue(parts[0], int.Parse(parts[1]));

                // add to dictio
                digraphs.Add(parts[0], digraphValue);
            }
        }

        internal void Go(string mainLayerConsonants, string altLayerConsonants)
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