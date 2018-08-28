

local myWorld		= World
local myGui 		= Gui
local myEntity		= EntitySystem

local level = {}

local mapManager = {}
mapManager.entrance = {}

local curZone	= 1
local curLevel	= 1
local maxLevel 	= 2

function createLevel( ID ) 
	SpawnManager.reset()
	
	if type( ID ) == "string" then
			if ID == "ZONE_START" 		then createLevel_start()	return
		elseif ID == "ZONE_1_LEVEL_1" 	then zone_1_level_1()
		else print( "Unable to create level. Wrong level ID ( " .. ID .. " )" ) return
		end
	else
			if ID == 1		then zone_1_level_1()
		elseif ID == 2		then zone_2_level_1()
		end
	end
	
	closeWindow( Window_Map )

	myWorld:createMap( level )
	
	if ID ~= "ZONE_START" then 
		myEntity:setPosRandom( playerLUD )
		
		local playerX, playerY = myEntity:getCenter( playerLUD )
		
		myWorld:setCameraCenter( playerX, playerY )
	else
		myEntity:setCenter( playerLUD, 1300, 750 )
		
		myWorld:setCameraCenter( { myEntity:getCenter( playerLUD ) } )
	end
	
	myCore:sendMessage( "NewLevel" )
end

function setupMap( mapID, objectID ) 
	local mapTable = nil
	
	if mapID == "MAP_HEAVEN" then 
		mapTable = {
			tileset		= "TILESET_GRASS";
			size		= { 24, 24 };
			
			objects 	= {
				{ table = getObject( "TREE_GREENLEAF"), 	min = 5, 	max = 7 };
			};
		}
	else print( "Error: Wrong map ID: " .. mapID )
	end
		
	if mapTable ~= nil then
		mapManager.entrance[ tostring( objectID ) ] = mapTable
	end
end

function nextLevel()
	if curLevel < maxLevel then
		curLevel = curLevel + 1

		createLevel( curLevel )
	else
		curLevel = 1
		
		createLevel( "ZONE_START" )
	end
end

function enterMap( objectID )
	if mapManager.entrance[ tostring( objectID ) ] ~= nil then
		local map = mapManager.entrance[ tostring( objectID ) ]
		
		if type( map ) == "table" then
			mapManager.entrance[ tostring( objectID ) ] = myWorld:createMap( map )
		else 
			myWorld:enterMap( map )
		end
	end
end

function zone_1_level_1()
	local enemyRanger = getCharacter( "ENEMY_RANGER" )
	
	enemyRanger.charData.loot = {
		minItems		= 0;
		maxItems		= 2;
		minRunestones	= 1;
		maxRunestones 	= 10;
		runestoneChance	= 100;

		table       	= {
			getLootTable( "Equipment" );
		}
	}
	
	SpawnManager.addWave( { 
		pricepool 	= 250;
		enemies		= {
			{ enemy = enemyRanger, price = 50 };
		};
	} )
	
	SpawnManager.addBoss( getCharacter( "ENEMY_BOSS" ) )

	local data = {
		map			= {
			tileset		= "TILESET_TEST";
			size		= { 42, 42 };

			layers 		= {
				{
					chance		= 0.55;
					smoothing	= 3;
				};
			};
		};
		
		objects 	= {
			{ table = getObject( "TREE_GREENLEAF"), 		min = 2, 	max = 4 };
			{ table = getObject( "ROCK_GREY_ONE"), 			min = 5, 	max = 15 };
			{ table = getObject( "PLATFORM_BOSS"),			min = 1 };
			{ table = getObject( "STRUCTURE_GATE_PORTAL"),	min = 1 };
		};
	}
	
	level = data 
end

function zone_2_level_1()
	local enemyRanger = getCharacter( "ENEMY_RANGER" )
	
	enemyRanger.charData.loot = {
		minItems		= 0;
		maxItems		= 2;
		minRunestones	= 1;
		maxRunestones 	= 10;
		runestoneChance	= 100;

		table       	= {
			getLootTable( "Equipment" );
		}
	}
	
	SpawnManager.addWave( { 
		pricepool 	= 500;
		enemies		= {
			{ enemy = enemyRanger, price = 50 };
		};
	} )
	
	SpawnManager.addBoss( getCharacter( "ENEMY_BOSS" ) )

	local data = {
		map			= {
			tileset		= "TILESET_WINTER";
			size		= { 68, 68 };

			layers 		= {
				{
					chance		= 0.53;
					smoothing	= 4;
				};
			};
		};
		
		objects 	= {
			{ table = getObject( "TREE_GREENLEAF"), 		min = 2, 	max = 4 };
			{ table = getObject( "ROCK_GREY_ONE"), 			min = 5, 	max = 15 };
			{ table = getObject( "PLATFORM_BOSS"),			min = 1 };
			{ table = getObject( "STRUCTURE_GATE_PORTAL"),	min = 3 };
		};
	}
	
	level = data 
end

function zone_1_level_2()
	SpawnManager.addWave( { 
		pricepool 	= 500;
		enemies		= {
			{ enemy = getCharacter( "ENEMY_SORCERER" ), price = 50 };
		};
	} )

	local data = {
		map			= {
			tileset		= "TILESET_GRASS";
			size		= { 36, 36 };		
		};
		
		objects 	= {
			{ table = getObject( "TREE_GREENLEAF"), 		min = 5, 	max = 10 };
			{ table = getObject( "STRUCTURE_GATE_PORTAL"),	min = 2 };
		};
	}
	
	level = data 
end