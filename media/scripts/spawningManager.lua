
-- Global object to handle spawning enemies ( to keep things clean )
SpawnManager		= {}

-- forward declaration
local waveAmount	= 0
local waveCounter	= 1
local waveEnded		= false
local levelStarted	= false
local waves			= {}

local bosses		= {}
local bossCounter	= 0

local timerLevelStart	= {}
timerLevelStart.start	= nil
timerLevelStart.limit	= 5.0

local timerEnemySpawn	= {}
timerEnemySpawn.start	= nil
timerEnemySpawn.limit	= 2.0

local timerWaveDelay	= {}
timerWaveDelay.start	= nil
timerWaveDelay.limit	= 5.0

local timerLevelEnd		= {}
timerLevelEnd.start		= nil
timerLevelEnd.limit		= 5.0

local timerBossSpawn	= {}
timerBossSpawn.start	= nil
timerBossSpawn.limit	= 3.0

--[[

				REWRITE THIS FILE !!!

]]

local function setLevelStart()
	levelStarted	= true
	waveEnded		= false
	
	timerEnemySpawn.start	= myWorld:getWorldTime()
	
	myCore:sendMessage( "LevelStart" )
end

local function spawnEnemy()
	if levelStarted == true and waveEnded == false then
		myCore:sendMessage( "SpawnEnemy" )

		timerEnemySpawn.start	= myWorld:getWorldTime()
	end
end

local function spawnBossObject()
	myCore:sendMessage( "WavesEnded" )
	
	timerBossSpawn.start = myWorld:getWorldTime()
end

SpawnManager.reset	= 
	function()
		waveAmount		= 0
		waveCounter		= 1
		waveEnded		= false
		levelStarted	= false
		
		timerLevelStart.start	= nil
		timerEnemySpawn.start	= nil
		timerWaveDelay.start	= nil
		timerLevelEnd.start		= nil
		timerBossSpawn.start	= nil
		
		waves			= {}
		bosses			= {}
		bossCounter		= 0
	end

SpawnManager.addWave = 
	function( wave )
		waveAmount = waveAmount + 1
		
		table.insert( waves, wave )
	end

SpawnManager.addBoss = 
	function( boss )
		bossCounter	= bossCounter + 1
	
		table.insert( bosses, boss )
	end
	
SpawnManager.update	= 
	function()
		if timerLevelStart.start ~= nil then
	
			local worldTime = myWorld:getWorldTime()
			
			if timerBossSpawn.start ~= nil then
				if worldTime - timerBossSpawn.start > timerBossSpawn.limit then
					timerBossSpawn.start = nil
				
					myCore:sendMessage( "SpawnBoss" )
				end
			end
			
			if levelStarted == false then 
				if worldTime - timerLevelStart.start > timerLevelStart.limit then
					setLevelStart()
				end

				return
			end
			
			if timerWaveDelay.start ~= nil then
				if worldTime - timerWaveDelay.start > timerWaveDelay.limit then
					timerWaveDelay.start	= nil
					
					myCore:sendMessage( "NextWave" )
				end
			end
			
			if timerLevelEnd.start ~= nil then
				if worldTime - timerLevelEnd.start > timerLevelEnd.limit then
					timerLevelEnd.start	= nil
					
					spawnBossObject()
				end
			end
			
			
			
			if worldTime - timerEnemySpawn.start > timerEnemySpawn.limit then
				spawnEnemy()
			end
		
			if waveEnded == false then
				if checkPricepool() == false then
					waveEnded = true
					
					if waveCounter < waveAmount then
						timerWaveDelay.start	= worldTime
						
						-- myCore:sendDelayedMessage( "NextWave", 	1.0 )
					else
						timerLevelEnd.start		= worldTime
						
						-- myCore:sendDelayedMessage( "WavesEnd", 	1.0 )
					end
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
		if levelStarted == false then return end
		if checkPricepool() == false then return end

		local enemy, price = rollForEnemy()
		
		waves[ waveCounter ].pricepool = waves[ waveCounter ].pricepool - price
		
		enemy.pos = { posX, posY }
	
		myWorld:createEnemy( enemy ) 
	end
	
SpawnManager.spawnBoss =
	function( posX, posY )
		for _, v in pairs( bosses ) do
			v.pos = { posX, posY }
			
			myWorld:createEnemy( v )
		end
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
	
	spawnEnemy()
end

-- Whenever new map is created, check if there are any enemy 
-- waves and send delayed message about level start
local function onNewLevel()
	if waveAmount > 0 then
		timerLevelStart.start		= myWorld:getWorldTime()
	end
end

local function bossDefeated() 
	bossCounter = bossCounter - 1
	
	if bossCounter == 0 then
		myCore:sendMessage( "LevelEnd" )
	end
end

register( "NewLevel", 		onNewLevel )
register( "LevelEnd",		SpawnManager.endLevel )
register( "NextWave", 		nextWave )
register( "BossDefeated", 	bossDefeated )