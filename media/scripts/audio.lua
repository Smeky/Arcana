
--[[
	Description
		- Manages all music based on scene or level
]]



local myAudio		= Audio
local currentScene	= 0			-- Main menu scene

local function onSceneChange( scene )
	
	-- First game load
	if scene == 0 then
		if currentScene ~= 1 then
			myAudio:stopMusic()
			myAudio:playMusic( "MAINMENU" )		
		end
	elseif scene == 2 then
		-- myAudio:stopMusic()
	end
	
	currentScene = scene
end

register( "NewScene", 		onSceneChange )

