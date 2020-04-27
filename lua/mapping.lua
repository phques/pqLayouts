require "objectNew"

Mapping = { new = ObjectNew }

-------------

-- only one of these
theKeyboard = {
	mappings = {}
}

function theKeyboard:addMapping(fromChar, toChar)
	-- (#todo get real scancode / vk etc)
	-- convert string to byte 
	print(string.format("addMapping from=%s, tp=%s", fromChar, toChar))

	vkFrom = pqLayout.GetVk(fromChar)
	vkTo = pqLayout.GetVk(toChar)

	-- self.mappings[vkFrom] = vkTo
end

function theKeyboard:getMapping(vkFrom)
	return self.mappings[vkFrom]
end

-- called from C++ for wm_keydown/up
function theKeyboard:onKey(vk, down)
	print(string.format("theKeyboard:OnKey vk=%x", vk))
	return self:getMapping(vk)
end

-------------------

theKeyboard:addMapping('A','G')
theKeyboard:addMapping('S','I')
theKeyboard:addMapping('D','O')
theKeyboard:addMapping('F',' ')

-- theKeyboard:addMapping('','')

function testGetVk(char)
	vk = pqLayout.GetVk(char)
	print(string.format("vk=%d, shft %s, ctrl %s, alt %s", vk.vk, vk.shiftOn, vk.ctrlOn, vk.altOn))
end

testGetVk('A')
testGetVk('a')
testGetVk('@')
