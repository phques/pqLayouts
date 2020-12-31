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

	--pqLayout.GetVk :
	-- table["vk"] = vk;
	-- table["shiftOn"] = (flags & 1) != 0;
	-- table["ctrlOn"] = (flags & 2) != 0;
	-- table["altOn"] = (flags & 4) != 0;

	vkFromTbl = pqLayout.GetVk(fromChar)
	vkToTbl = pqLayout.GetVk(toChar)

	print(string.format("addMapping from=%s, tp=%s", fromChar, toChar))
	print(string.format("           from=%x, tp=%x", vkFromTbl["vk"], vkToTbl["vk"]))

	self.mappings[vkFromTbl["vk"]] = vkToTbl
end

function theKeyboard:getMapping(vkFrom)
	vkToTbl = self.mappings[vkFrom]
	if vkToTbl == nil then
		return nil
	end
	
	vkTo = vkToTbl["vk"]
	if vkToTbl["shiftOn"] then
		vkTo = vkTo | 0x1000
	end
	return vkTo
end

-- called from C++ for wm_keydown/up
function theKeyboard:onKey(vk, down)
	print(string.format("theKeyboard:OnKey vk=%x, down=%s", vk, down))
	return self:getMapping(vk)
end

-------------------

theKeyboard:addMapping('A','Y')
theKeyboard:addMapping('S','E')
theKeyboard:addMapping('D',' ')
theKeyboard:addMapping('F','A')
theKeyboard:addMapping('G','G')

theKeyboard:addMapping('Z','.')
theKeyboard:addMapping('C',':')

-- theKeyboard:addMapping('','')

function testGetVk(char)
	vk = pqLayout.GetVk(char)
	print(string.format("vk=%d, shft %s, ctrl %s, alt %s", vk.vk, vk.shiftOn, vk.ctrlOn, vk.altOn))
end

testGetVk('A')
testGetVk('a')
testGetVk('@')
