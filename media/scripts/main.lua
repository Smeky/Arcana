myCore 		= Core
myInput		= Input
myAssets	= Assets
myGui		= Gui
myWorld		= World
myInventory	= Inventory

GameWindow_W, GameWindow_H = myCore:getWindowSize()


--[[
	Timer:
		- startTime			( game time )
		- endTime			( delay after which timer should call its function )
		- action			( function called on end time )
]]


-- Testing

local Timers	= {}

function registerTimer( Timer ) 
	table.insert( Timers, Timer )
end

function update()
	if next( Timers ) ~= nil then
		local gameTime = myCore:getGameTime()
	
		for i, v in ipairs( Timers ) do 
			local relTime = gameTime - v.startTime
			
			if relTime >= v.endTime then 
				v.action()
				
				table.remove( Timers, i )
			end
		end	
	end
end

function DebugPrintError( statement, errorMsg )
	if statement == true then
		print( errorMsg )
	end
end

function getTableSize( T )
	local counter = 0
	
	for _ in pairs( T ) do counter = counter + 1 end
	
	return counter
end

function isTableEmpty( T )
	if T == nil then return true end
	
	return next( T ) == nil
end

function roundNumber( num, n )
	local mult = 10 ^ ( n or 0 )
	return math.floor( num * mult + 0.5 ) / mult
end

function distanceBetweenVectors( firstX, firstY, secondX, secondY )
	return math.sqrt( math.pow( firstX - secondX, 2 ) + math.pow( firstY - secondY, 2 ) )
end

function angleByVectors( firstX, firstY, secondX, secondY ) 
	return math.atan2( secondY - firstY, secondX - firstX )
end

function copyTable( orig )
    local orig_type = type( orig )
    local copy
	
    if orig_type == 'table' then
        copy = {}
		
        for orig_key, orig_value in next, orig, nil do
            copy[ deepcopy( orig_key ) ] = deepcopy( orig_value )
        end
		
        setmetatable( copy, deepcopy( getmetatable( orig ) ) )
    else -- number, string, boolean, etc
        copy = orig
    end
	
    return copy
end

register( "Update",				update )