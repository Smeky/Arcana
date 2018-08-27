
local Faction_neutral	= 0
local Faction_friendly	= 1
local Faction_enemy		= 2

local Class_noclass		= 0
local Class_barbarian	= 1
local Class_knight		= 2
local Class_ranger		= 3
local Class_assassin	= 4
local Class_sorcerer	= 5
local Class_necromancer	= 6

-- Forward declarations
local characters = {}

function getCharacter( ID ) 
	if characters[ ID ] == nil then print( "Error: Wrong character ID! ( ID: " .. ID .. " )" ) return end
	
	return characters[ ID ]
end

characters[ "PLAYER_BARBARIAN" ] = {
	data		= {
		size 		= { 34, 42 };
		collbox		= { 0, 42 - ( 42 / 2.5 ), 34, 42 / 2.5 };
		texture 	= {
			texture 	= "CHAR_PLAYER_BARBARIAN";
			clip		= { 0; 0; 17; 21; };
			frameDelay	= 0.25;
			directions	= true
		}		
	};
	
	
	
	charData 	= {
		class		= Class_barbarian;
		faction		= Faction_friendly;
		health		= 120;
		resource	= 40;
		stats		= {
			str			= 10;
			regHealth	= 0;
			moveSpd		= 100;
			attSpd		= 250;
			dmgMin		= 3;
			dmgMax		= 6;
		};
	};
}

characters[ "PLAYER_KNIGHT" ] = {
	data		= {
		size 		= { 34, 42 };
		collbox		= { 0, 42 - ( 42 / 2.5 ), 34, 42 / 2.5 };
		texture 	= {
			texture 	= "CHAR_PLAYER_KNIGHT";
			clip		= { 0; 0; 17; 21; };
			frameDelay	= 0.25;
			directions	= true
		}		
	};
	
	charData 	= {
		class		= Class_knight;
		faction		= Faction_friendly;
		health		= 150;
		resource	= 40;
		stats		= {
			str			= 5;
			vit			= 6;
			regHealth	= 1;
			moveSpd		= 80;
			attSpd		= 180;
			dmgMin		= 2;
			dmgMax		= 5;
		};
	};
}

characters[ "PLAYER_RANGER" ] = {
	data		= {
		size 		= { 34, 42 };
		collbox		= { 0, 42 - ( 42 / 2.5 ), 34, 42 / 2.5 };
		texture 	= {
			texture 	= "CHAR_PLAYER_RANGER";
			clip		= { 0; 0; 17; 21; };
			frameDelay	= 0.25;
			directions	= true
		}		
	};
	
	charData 	= {
		class		= Class_ranger;
		faction		= Faction_friendly;
		health		= 80;
		resource	= 60;
		stats		= {
			dex			= 10;
			moveSpd		= 110;
			attSpd		= 300;
			dmgMin		= 3;
			dmgMax		= 5;
		};
	};
}

characters[ "PLAYER_ASSASSIN" ] = {
	data		= {
		size 		= { 34, 42 };
		collbox		= { 0, 42 - ( 42 / 2.5 ), 34, 42 / 2.5 };
		texture 	= {
			texture 	= "CHAR_PLAYER_ASSASSIN";
			clip		= { 0; 0; 17; 21; };
			frameDelay	= 0.25;
			directions	= true
		}		
	};
	
	
	
	charData 	= {
		class		= Class_assassin;
		faction		= Faction_friendly;
		health		= 90;
		resource	= 50;
		stats		= {
			dex			= 10;
			moveSpd		= 120;
			attSpd		= 300;
			dmgMin		= 4;
			dmgMax		= 8;
		};
	};
}

characters[ "PLAYER_SORCERER" ] = {
	data		= {
		size 		= { 34, 42 };
		collbox		= { 0, 42 - ( 42 / 2.5 ), 34, 42 / 2.5 };
		texture 	= {
			texture 	= "CHAR_PLAYER_SORCERER";
			clip		= { 0; 0; 17; 21; };
			frameDelay	= 0.25;
			directions	= true
		}		
	};
	
	charData 	= {
		class		= Class_sorcerer;
		faction		= Faction_friendly;
		health		= 80;
		resource	= 80;
		stats		= {
			int			= 8;
			regResource	= 1;
			moveSpd		= 90;
			attSpd		= 250;
			dmgMin		= 2;
			dmgMax		= 4;
		};	
	};
}

characters[ "PLAYER_NECROMANCER" ] = {
	data		= {
		size 		= { 34, 42 };
		collbox		= { 0, 42 - ( 42 / 2.5 ), 34, 42 / 2.5 };
		texture 	= {
			texture 	= "CHAR_PLAYER_NECROMANCER";
			clip		= { 0; 0; 17; 21; };
			frameDelay	= 0.25;
			directions	= true
		}		
	};
	
	charData 	= {
		class		= Class_necromancer;
		faction		= Faction_friendly;
		health		= 90;
		resource	= 40;
		stats		= {
			int			= 8;
			regHealth	= 1;
			regResource	= 1;
			moveSpd		= 90;
			attSpd		= 250;
			dmgMin		= 2;
			dmgMax		= 4;
		};
	};
}

characters[ "ENEMY_RANGER" ] = {
	data 		= {
		size 		= { 56, 80 };
		collbox		= { 0, 80 - 80 / 2.5, 56, 80 / 2.5 };
		texture 	= {
			texture 	= "CHAR_ENEMY_RANGER";
			clip		= { 0; 0; 17; 21; };
			frameDelay	= 0.25;
			directions	= true;
		};

	};
		
	charData 	= {
		class		= 3;
		faction		= 2;
		health		= 30;
		resource	= 40;
		experience	= 10;
		stats		= {
			str			= 10;
			vit			= 5;
			regHealth	= 1;
			moveSpd		= 100;
			attSpd		= 200;
			dmgMin		= 3;
			dmgMax		= 6;
		};
	};										
}

characters[ "ENEMY_SORCERER" ] = {
	data 		= {
		size 		= { 56, 80 };
		collbox		= { 0, 80 - 80 / 2.5, 56, 80 / 2.5 };
		texture 	= {
			texture 	= "CHAR_PLAYER_SORCERER";
			clip		= { 0; 0; 17; 21; };
			frameDelay	= 0.25;
			directions	= true;
		};

	};
		
	charData 	= {
		class		= 3;
		faction		= 2;
		health		= 30;
		resource	= 40;
		experience	= 10;
		stats		= {
			str			= 10;
			vit			= 5;
			regHealth	= 1;
			moveSpd		= 100;
			attSpd		= 200;
			dmgMin		= 3;
			dmgMax		= 6;
		};
	};										
}










