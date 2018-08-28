
--[[
	SFML Key codes:
		Enter		- 58
		0			- 26
		...
		9			- 35
		
		Arrow UP	- 73
		Arrow DOWN	- 74
		
		U       	- 20
		I       	- 8
]]

local myCore 	= Core
local myAudio	= Audio
local myGui		= Gui
local myEntity	= EntitySystem


function onKeyPress( keyID ) 
	-- print( "KeyID: " .. keyID )
end

function pauseGame()
	myGui:destroyTooltip()
	
	if myGui:isWindowShown( Window_ItemDestroy_ID ) then
		inventoryKeepItem()
	
	elseif closeWindows() then
	
	elseif myGui:isWindowShown( Window_Inventory_ID ) then
		myGui:reverseWindowShown( Window_Inventory_ID )
	
	else	
		myGui:reverseShown( Window_Pause )
		myGui:reverseWindowActive( Window_Abilitybar_ID )
		myWorld:pauseGame()
	end
	
	if myWorld:isGamePaused() then
		myGui:setActive( Window_Abilitybar, false )
	else 
		myGui:setActive( Window_Abilitybar, true )
	end
end

function onKeyRelease( keyID )
	local sceneID = myCore:getSceneID()
	
	if sceneID == 1 then
			if keyID == 27 then charSelectionIndex = 1; changeSceneToPlay( 2 ) -- Num1
		elseif keyID == 28 then charSelectionIndex = 2; changeSceneToPlay( 2 ) -- Num2
		elseif keyID == 29 then charSelectionIndex = 3; changeSceneToPlay( 2 ) -- Num3
		elseif keyID == 30 then charSelectionIndex = 4; changeSceneToPlay( 2 ) -- Num4
		elseif keyID == 31 then charSelectionIndex = 5; changeSceneToPlay( 2 ) -- Num5
		elseif keyID == 32 then charSelectionIndex = 6; changeSceneToPlay( 2 ) -- Num6
		end
		
	elseif sceneID == 2 then
		if myWorld:isGamePaused() == false then
				if keyID == 1 	then myGui:reverseWindowShown( Window_Inventory_ID )
			elseif keyID == 27 	then myWorld:playerCastAbility( 0, { getRelativeMousePos() } )
			elseif keyID == 28 	then myWorld:playerCastAbility( 1, { getRelativeMousePos() } )
			elseif keyID == 29 	then myWorld:playerCastAbility( 2, { getRelativeMousePos() } )
			elseif keyID == 30 	then myWorld:playerCastAbility( 3, { getRelativeMousePos() } )
			end
		end	
	end
	
	if keyID == 5 then			-- F
		if myWorld:isGamePaused() ~= true then
			myWorld:playerInteract()
		end
	
	elseif keyID == 36 then			-- Escape	
		if sceneID == 1 then
			myCore:changeScene( 0 )
			
		elseif sceneID == 2 then
			pauseGame()
			
		end
		
	elseif keyID == 20 then
		myCore:setupWindow( 1000, 600, 4 )
		GameWindow_W, GameWindow_H = myCore:getWindowSize()
		myGui:close()
		myCore:changeScene( myCore:getSceneID() )
	
	elseif keyID == 8 then
		myCore:setupWindow( 1280, 754, 8 )
		GameWindow_W, GameWindow_H = myCore:getWindowSize()
		myGui:close()
		myCore:changeScene( myCore:getSceneID() )
		
		
	elseif keyID == 85 then		-- F1		- Display map tile grid
		myWorld:displayMapGrid()
		
	elseif keyID == 87 then		-- F3		- Testing deleting sprite by ID
		myGui:deleteSprite( spriteID )
		
	elseif keyID == 88 then		-- F4		- Reload current scene
		myCore:changeScene( myCore:getSceneID() )
		
	elseif keyID == 89 then		-- F5		- Reload Lua scripts
		reload()
	
	elseif keyID == 90 then		-- F6		- Display entity box
		myWorld:displayEntityBox()
	
	elseif keyID == 91 then 	-- F7		- Display spatial system
		myWorld:displaySS()
	
	elseif keyID == 92 then 	-- F8		- Display entity stats
		myWorld:displayEntityStat()
		
	end
	
	
end

register( "InputKeyPressed", 	onKeyPress )
register( "InputKeyReleased", 	onKeyRelease )






