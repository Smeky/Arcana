

local myEntity = EntitySystem

-- Forward declaration
local abilities = {}

function getAbility( ID )
	if abilities[ ID ] == nil then print( "Error: Wrong ability ID! ( ID: " .. ID .. " )" ) return end
	
	return abilities[ ID ]
end


abilities[ "BLINK" ] = {
	data	= { 
		name		= "Blink";
		description	= "Teleports caster";
		icon		= "GUI_ICON_ABILITY_BLINK";
		cost		= 20;
		cooldown	= 10.0;
	};
	
	onCast	= 	function( caster, destX, destY )
					local sizeX, sizeY 	= myEntity:getSize( caster )
					local centX, centY	= destX - sizeX / 2, destY - sizeY / 2
	
					if myEntity:isPosFree( centX, centY, sizeX, sizeY ) == true then
						myEntity:setCenter( caster, destX, destY )
					end
				end;
}

abilities[ "DEV_BLINK" ] = {
	data	= { 
		name		= "Blink ( DEV )";
		icon		= "GUI_ICON_ABILITY_BLINK";
		cost		= 1;
		cooldown	= 0.1;
		
	};
	
	onCast	= 	function( caster, destX, destY )
					myEntity:setCenter( caster, destX, destY )
				end;
}

abilities[ "FIREBALL" ] = {
	data	= { 
		name		= "Fireball";
		description	= "It's hot!";
		icon		= "GUI_ICON_ABILITY_FIREBALL";
		castTime	= 0.5;
		cooldown	= 3.0;
		cost		= 15;
	};
	
	onCast	= 	function( caster, destX, destY )
					local posX, posY 	= myEntity:getCenter( caster )
					local casterStats	= myEntity:getCharacterStats( caster )
					local angle 		= angleByVectors( posX, posY, destX, destY )
	
					casterStats.dmgMin	= casterStats.dmgMin * 2.5;
					casterStats.dmgMax	= casterStats.dmgMax * 2.5;
	
					myWorld:createProjectile( getProjectile( "FIREBALL", posX, posY, angle, casterStats.castDistance or 250, casterStats ) )
				end
}

abilities[ "FROSTSHIELD" ] = {
	data	= { 
		name		= "Frost Shield";
		description	= "Slows enemies that hits you";
		icon		= "GUI_ICON_ABILITY_FROSTSHIELD";
		cooldown	= 0.0;
		cost		= 0;
	};
	
	onCast	= 	function( caster, destX, destY )
					myEntity:addEffect( caster, getEffect( "EFFECT_FROSTSHILED" ) )
				end;
}

abilities[ "POISON_BOMB" ] = {
	data	= { 
		name		= "Poison bomb";
		description	= "Deals damage over time to enemies";
		icon		= "GUI_ICON_ABILITY_POISONBOMB";
		castTime	= 0;
		cost		= 15;
		cooldown	= 5.0;
	};
	
	onCast	= 	function( caster, destX, destY )
					local posX, posY 	= myEntity:getCenter( caster )
					local casterStats	= myEntity:getCharacterStats( caster )
					local angle 		= angleByVectors( posX, posY, destX, destY )
					local distance 		= distanceBetweenVectors( posX, posY, destX, destY )
					
					local maxDistance = 700
					if distance > maxDistance then distance = maxDistance end
					
					casterStats.dmgMin	= 0
					casterStats.dmgMax	= 0
	
					myWorld:createProjectile( getProjectile( "BOMB_POISON", posX, posY, angle, distance, casterStats ) )
				end
}







