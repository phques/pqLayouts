using System;
using YamlDotNet.Serialization;
using YamlDotNet.Serialization.NamingConventions;

public class LoadLayout
{
	static public void Main()
    {
        var deserializer = new DeserializerBuilder()
            .WithNamingConvention(CamelCaseNamingConvention.Instance) 
            .Build();


        try
        {
            var input = new StreamReader("layoutHdNeuC.yaml");

            //var p = deserializer.Deserialize<object>(input);
            var p = deserializer.Deserialize<LayoutIn>(input);
            Console.WriteLine(p.ToString());

        }
        catch (Exception e)
        {
            string inner = e.InnerException?.Message ?? "";
            Console.WriteLine($"Error: {e.Message}\n{inner}");
        }
    }
}
