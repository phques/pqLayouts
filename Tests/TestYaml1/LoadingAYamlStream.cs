using System.IO;
using YamlDotNet.RepresentationModel;

public class LoadingAYamlStream
{
    //static 
        public void Main()
    {
        // Setup the input
        var input = new StreamReader("layoutHdNeuC.yaml");

        // Load the stream
        var yaml = new YamlStream();
        yaml.Load(input);

        // Examine the stream
        var mapping = (YamlMappingNode)yaml.Documents[0].RootNode;

        foreach (var entry in mapping.Children)
        {
            Console.WriteLine(((YamlScalarNode)entry.Key).Value);
        }
    }
}
