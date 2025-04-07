using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using YamlDotNet.Serialization;

namespace pqLayoutsNET
{
    public class LayoutIn
    {
        public string Name { get; set; }
        public string ImageFile { get; set; }
        public List<LayerIn> Layers { get; set; }
    }

    public class LayerIn
    {
        public string Name { get; set; }
        public string From { get; set; }
        public string To { get; set; }
        public string Shift { get; set; }
        [YamlMember(Alias = "key")]
        public string AccessKey { get; set; }   // not valid for main layer
        public ImageViewIn ImageView { get; set; }
        public bool ToggleOnTap { get; set; }   // mutually exclusive w KeyOnTap
        public string KeyOnTap { get; set; }    // mutually exclusive w ToggleOnTap
        public List<DualModIn> DualMods { get; set; }
        public List<AdaptiveIn> Adaptives { get; set; }
    }

    public class DualModIn
    {
        public string Key { get; set; }
        [YamlMember(Alias = "mod")]
        public string Modifier { get; set; }
        [YamlMember(Alias = "tap")]
        public string TapKey { get; set; }
    }

    public class AdaptiveIn
    {
        public string Keys { get; set; }
        public string Send { get; set; }
    }

    public class ImageViewIn
    {
        public ImageViewCoordsIn Normal { get; set; }
        public ImageViewCoordsIn Shift { get; set; }
    }

    public class ImageViewCoordsIn
    {
        public int Top { get; set; }
        public int Height { get; set; }
    }
}
