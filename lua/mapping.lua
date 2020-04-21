require "objectNew"

Mapping = { new = ObjectNew }

-- function Mapping:toto(msg)
-- 	print(msg)
-- end


-- m = Mapping:new()
-- m:toto("allo")


-------------

-- only one of these
theKeyboard = {
	mappings = {}
}

function theKeyboard:AddMapping(vkFrom, vkTo)
	-- (#todo get real scancode / vk etc)
	-- convert string to byte 
	print(string.format("AddMapping from=%s, tp=%s", vkFrom, vkTo))
	vkFrom = string.byte(vkFrom)
	vkTo = string.byte(vkTo)
	print(string.format("AddMapping from=%x, tp=%x", vkFrom, vkTo))
	self.mappings[vkFrom] = vkTo
end

function theKeyboard:GetMapping(vkFrom)
	return self.mappings[vkFrom]
end

-- called from C++ for wm_keydown/up
function theKeyboard:OnKey(vk, down)
	print(string.format("theKeyboard:OnKey vk=%x", vk))
	return self.mappings[vk]
end

function OnKey(vk, down)
	print"onkey"
	return theKeyboard:OnKey(vk, down)
end

function OnKey2(thekbd, vk, down)
	print"onkey2"
	return thekbd:OnKey(vk, down)
end

theKeyboard:AddMapping('A','G')
theKeyboard:AddMapping('S','I')
theKeyboard:AddMapping('D','O')
theKeyboard:AddMapping('F',' ')

-- theKeyboard:AddMapping('','')
