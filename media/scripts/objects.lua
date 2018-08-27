

--[[
	
	data: ( Table )
	 - size, collbox, solid, collideable, texture
	 
	ai types:
		LoopMovement		- Loops the movement based on two positions - parents initial pos and initial pos + offset pos. Can be used either smooth or direct movement
			data:	offsetPos | smoothVel 	= In case movement should be used. Otherwise objects speed is used

]]

local objects = {}

function getObject( ID )
	if objects[ ID ] == nil then print( "Error: Wrong object ID! ( ID: " .. ID .. " )" ) return end
	
	return objects[ ID ]
end

objects[ "LOOT_BAG" ] = {
	data	= {
		size	= { 24, 24 };
		solid	= false;
		texture	= "LOOT_BAG";
	}
}

objects[ "RUNESTONE" ] = {
	data	= {
		size	= { 24, 24 };
		solid	= false;
		texture	= "RUNESTONE";
	}
}

objects[ "TREE_GREENLEAF" ] = {
	data	= {
		size	= { 220, 300 };
		collbox	= { 80, 240, 75, 60 };
		solid	= true;
		texture	= "OBJECT_TREE_GREENLEAF";
	}
}

objects[ "ROCK_GREY_ONE" ] = {
	data	= {
		size	= { 56, 52 };
		collbox	= { 0, 0, 56, 52 };
		solid	= true;
		texture	= "OBJECT_ROCK_GREY_ONE";
	};
	
	
}

objects[ "STRUCTURE_GATE_PORTAL" ] = {
	data	= {
		size	= { 200, 200 };
		texture = "OBJECT_STRUCTURE_GATE_PORTAL";
	};

	listenTo 	= {
		{ name = "LevelStart"; action =
			function( ID )
				local thisX, thisY = myWorld:getEntityPos( ID )
				
				local portal = getObject( "PORTAL_BLUE" )
				
				portal.pos = { thisX + 65, thisY + 85 }
	
				myWorld:createObject( portal )
			end;				
		};
	}
}

objects[ "STRUCTURE_TEMPLE_CENTER" ] = {
	pos		= { 950, 600 };
	data	= {
		size	= { 320, 320 };
		collbox	= { 40, 200, 240, 120 };
		solid	= true;
		texture	= "OBJECT_STRUCTURE_TEMPLE_CENTER";
	};
}

objects[ "STRUCTURE_TEMPLE_UPGRADE" ] = {
	pos		= { 800, 100 };
	data	= {
		size	= { 500, 500 };
		texture	= "OBJECT_STRUCTURE_TEMPLE_UPGRADE";
	};
	
}
 
objects[ "HIDDEN_TELEPORTER" ] = {
	pos		= { 1170, 700 };
	data	= {
		size	= { 50, 165 };
	};

	functions = {
		onInteract = function( ID ) local posX, posY = myWorld:getEntityCenter( ID );  interactWithWindow( posX, posY, Window_Map ) end;
	}
}

objects[ "CHARACTER_UPGRADE" ] = {
	pos		= { 1200, 350 };
	data	= {
		size	= { 70, 120 };
		texture	= "OBJECT_CHARACTER_UPGRADE";
	};
	
	functions = {
		onInteract 	= function( ID ) local posX, posY = myWorld:getEntityCenter( ID ); interactWithWindow( posX, posY, Window_Upgrade ) end;
	}
}

objects[ "PILLAR_LEVELEND" ] = {
	data	= {
		size	= { 70, 120 };
		texture	= "OBJECT_CHARACTER_UPGRADE";
	};
	
	functions = {
		onInteract 	= function( ID ) nextLevel() end;
	}
}

objects[ "PORTAL_BLUE" ] = {
	data	= {
		size	= { 80, 120 };
		solid	= false;
		texture	= "OBJECT_PORTAL_BLUE";
	};
	
	listenTo = {
		{ name = "SpawnEnemy"; action = 
					function( ID )
						local thisX, thisY = myWorld:getEntityPos( ID )
						
						
						SpawnManager.spawnEnemy( thisX, thisY )
					end; }
	}
}

objects[ "ROCK_GREY_FLOAT_ONE" ] = {
	data	= {
		size	= { 61, 59 };
		texture	= "OBJECT_ROCK_GREY_FLOAT_ONE";
		zLayer	= 16;
	};

	ai 		= {
		ID		= "LoopMovement";
		data	= {
			offsetPos	= { 0, 300 };
			smoothVel	= 4;
		};
	};
}

objects[ "CRYSTAL_PURPLE_STARTZONE" ] = {
	pos		= { 1220, 295 };
	data	= {
		size	= { 30, 45 };
		texture	= "OBJECT_CRYSTAL_PURPLE_STARTZONE";
	};
	
	effect 	= {
		{
			emission	= 0.05;
			parent		= true;
			lifeType	= 1;		-- Parent based
			posType		= 1;		-- Parent based
			zLayerType	= 2;
			
			particle	= {
				size			= { 10, 10 };
				texture			= "PARTICLE_START_PURPLE_CYSTAL";
				moveVel			= 50;
				moveVelRange	= 20;
				distance		= 50;
				distanceRange	= 10;
				rotDirection	= 2;
				rotVel			= 50;
				rotVelRange		= 50;
				scalingType		= 1;
			};
		};
	};
	
	ai 		= {
		ID		= "LoopMovement";
		data	= {
			offsetPos	= { 0, 50 };
			smoothVel	= 4;
		};
	};
}














































