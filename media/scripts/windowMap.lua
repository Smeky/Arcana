
local mapOpen 	= false
	  objectID	= nil


function openMapWindow()
	myGui:setWindowShown( Window_Map_ID, true )
	
	mapOpen = true
end

function closeMapWindow() 
	myGui:setWindowShown( Window_Map_ID, false )

	mapOpen = false
end

function showOrHideMapWindow()
	myGui:reverseWindowShown( Window_Map_ID )
	
	mapOpen = myGui:isWindowShown( Window_Map_ID )
end

-- Function that closes the map window in case player went too far
local function update()
	if mapOpen == true then
		if objectID ~= nil then
		
			local playerX, playerY = myWorld:getPlayerCenter()
			local objectX, objectY = myWorld:getEntityCenter( objectID )

			local distance = distanceBetweenVectors( playerX, playerY, objectX, objectY )
			
			if distance > 200 then closeMapWindow() end
		end
	end
end

function newLevel_start()
	createLevel( "ZONE_START" )
end

function newLevel_zone_1_level_1()
	createLevel( "ZONE_1_LEVEL_1" )
end

register( "UpdateWorld",		update );
register( "GuiButtonMap1", 		newLevel_start )
register( "GuiButtonMap2", 		newLevel_zone_1_level_1 )














