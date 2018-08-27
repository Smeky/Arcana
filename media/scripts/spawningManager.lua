
-- Global object to handle spawning enemies ( to keep things clean )
SpawnManager		= {}

-- forward declaration
local waveAmount	= 0
local waveCounter	= 1
local waveEnded		= false
local levelStarted	= false
local waves			= {}

--[[
	waves = {	
		{
			pricepool
			enemies = {
				{ enemy, price };
				{ enemy, price };
				{ enemy, price };
				{ enemy, price };
				{ enemy, price };
				{ enemy, price };
				{ enemy, price };
			}
		};
	}
]]

--[[
-- Basic initialization
SpawnManager.canSpawn		= function()				return checkPricepool() end
SpawnManager.setPricepool 	= function( pool ) 			pricepool = pool end
SpawnManager.addEnemy		= function( enemy, cost ) 	enemy.cost = cost; table.insert( enemies, enemy ) end
SpawnManager.setEnemies		= function( allEnemies ) 	enemies = allEnemies end
SpawnManager.spawnEnemy		= 
	function( posX, posY )	
		if checkPricepool() == false then return end

		local enemy = rollForEnemy() 
		
		pricepool = pricepool - enemy.cost
		
		enemy.pos = { posX, posY }
	
		myWorld:createEnemy( enemy ) 
	end
]]

SpawnManager.reset	= 
	function()
		waveAmount		= 0
		waveCounter		= 1
		waveEnded		= false
		levelStarted	= false
		
		waves			= {}
	end

SpawnManager.addWave = 
	function( wave )
		waveAmount = waveAmount + 1
		
		table.insert( waves, wave )
	end
	
SpawnManager.update	= 
	function()
		if levelStarted == false then return end
	
		if waveEnded == false then
			if checkPricepool() == false then
				waveEnded = true
				
				if waveCounter < waveAmount then
					myCore:sendDelayedMessage( "NextWave", 	1.0 )
				else
					myCore:sendDelayedMessage( "WavesEnd", 	1.0 )
				end
			end
		end
	end
	
SpawnManager.canSpawn = 
	function()
		return waveEnded == false
	end

SpawnManager.spawnEnemy	= 
	function( posX, posY )
		if checkPricepool() == false then return end

		local enemy, price = rollForEnemy()
		
		waves[ waveCounter ].pricepool = waves[ waveCounter ].pricepool - price
		
		enemy.pos = { posX, posY }
	
		myWorld:createEnemy( enemy ) 
	end
	
SpawnManager.endLevel = 
	function()
		levelStarted 	= false
		waveEnded		= true
	end
	
-- Returns random enemy from enemies table
function rollForEnemy()
	-- Roll for random enemy out of wave table
	local index = math.random( 1, getTableSize( waves[ waveCounter ].enemies ) )
	
	local enemy 	= waves[ waveCounter ].enemies[ index ].enemy
	local price 	= waves[ waveCounter ].enemies[ index ].price
	
	
	-- Check if enemy can be spawned based on its price
	while price > waves[ waveCounter ].pricepool do 
		index = math.random( 1, getTableSize( waves[ waveCounter ].enemies ) )
	
		enemy 	= waves[ waveCounter ].enemies[ index ].enemy
		price 	= waves[ waveCounter ].enemies[ index ].price		
	end

	-- Return enemy to spawn
	return enemy, price
end

-- Returns if there are still some enemies to spawn based on credits in pricepool
function checkPricepool()
	if waves[ waveCounter ].pricepool <= 0 then return false end

	for k, v in pairs( waves[ waveCounter ].enemies ) do
		if v.price <= waves[ waveCounter ].pricepool then return true end
	end

	-- If there is no enemy left to spawn ( cuz pricepool is too low ) reset pricepool
	waves[ waveCounter ].pricepool = 0
	
	return false
end

local function nextWave()
	waveCounter	= waveCounter + 1;
	
	waveEnded	= false
	
	myCore:sendMessage( "SpawnEnemy" )
end

local function setLevelStart()
	levelStarted	= true
	waveEnded		= false
	
	myCore:sendMessage( "SpawnEnemy" )
end

local function spawnEnemy()
	if levelStarted == true and waveEnded == false then
		myCore:sendDelayedMessage( "SpawnEnemy", 1.0 )
	end
end

local function spawnBossObject()
	local object = getObject( "PILLAR_LEVELEND" )
	
	object.pos 	= { 1100, 1100 };
	
	myWorld:createObject( object );
end

register( "LevelStart", 	setLevelStart )
register( "LevelEnd",		SpawnManager.endLevel )
register( "NextWave", 		nextWave )
register( "SpawnEnemy", 	spawnEnemy )
register( "WavesEnd",		spawnBossObject )