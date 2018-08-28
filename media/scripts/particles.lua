
--[[
	Emitter lifetime type:
		0	- Timed
		1	- Parent based
		
	Emitter position type:
		0 	- Default 		( position specified inside emitter )
		1	- Parent based	( position based on parent center )
		
	Particle rotation direction:
		0 	- Random
		1	- Left
		2 	- Right
		
	Particle slow type:
		0 	- None
		1 	- Full time			( Slows during the whole life )
		2 	- Quarter of life
		3	- Half of life
		4	- Three quarters of life
]]

local myEntity = EntitySystem

-- Forward declaration
local emitters = {}

function getEmitter( ID )
	if emitters[ ID ] == nil then print( "Error: Wrong item ID! ( ID: " .. ID .. " )" ) return end
	
	return emitters[ ID ]
end

-- Empty for now, needs a good way of adding and removing effects from entities ( not just time based )
function addEffectToEntity( entity )
	
end

register( "AddParticlesToEntity",	addEffectToEntity )

emitters[ "EMITTER_TEST" ] = {
	emission	= 0.1;
	parent		= true;
	lifeType	= 1;		-- Parent based
	posType		= 1;		-- Parent based
	
	particle	= {
		size			= { 8, 8 };
		texture			= "PARTICLE_TEST";
		moveVel			= 200;
		moveVelRange	= 100;
		distance		= 300;
		distanceRange	= 100;
		rotDirection	= 2;
		rotVel			= 50;
		rotVelRange		= 50;
	};
	
}
