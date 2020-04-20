require "objectNew"

Mapping = { new = ObjectNew }

function Mapping:toto(msg)
	print(msg)
end


m = Mapping:new()
m:toto("allo")

function toto(msg)
	print(msg)
end
