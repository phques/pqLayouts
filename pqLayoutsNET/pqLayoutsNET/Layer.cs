using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace pqLayoutsNET
{
    public class Layer
    {
        public Layer(string name)
        {
            Name = name;
        }

        public void AddMapping(VeeKee from, VeeKee to)
        {
            Mappings.Add(from, to);
        }
        public void AddMappingShifted(VeeKee from, VeeKee to)
        {
            MappingsShifted.Add(from, to);
        }

        public VeeKee Mapping(VeeKee key, bool shift)
        {
            var map = shift ? MappingsShifted : Mappings;
            if (map.TryGetValue(key, out var mappedKey))
            {
                return mappedKey;
            }

            return new VeeKee();
        }

        public string Name { get; }

        public Dictionary<VeeKee, VeeKee> Mappings { get; } = new Dictionary<VeeKee, VeeKee>();
        public Dictionary<VeeKee, VeeKee> MappingsShifted { get; } = new Dictionary<VeeKee, VeeKee>();
    }
}
