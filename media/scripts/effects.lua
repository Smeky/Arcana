
--[[
	Stat type:
		0	- STAT_HEALTH
		1	- STAT_RESOURCE
		2	- STAT_STR
		3	- STAT_DEX
		4	- STAT_INT
		5	- STAT_VIT
		6	- STAT_ARMOR
		7	- STAT_REGENHEALTH
		8	- STAT_REGENRESOURCE
		9	- STAT_MOVEMENTSPD
		10	- STAT_ATTACKSPD
		11	- STAT_CASTINGSPD
		12	- STAT_DAMAGEMIN
		13	- STAT_DAMAGEMAX
		14	- STAT_CRITCHANCE
		15	- STAT_CRITMULTIP
		16	- STAT_EXPBONUS
		
	Stat format:
		index, value, operand
		
	Stat calculation operation type:
		0	- Add
		1	- Remove
		2 	- Multiply
		3 	- Divide
		4 	- Percent Add
		5	- Percent Remove
		
	Effect type;
		0 	- Buff
		1 	- Debuff
		
	Effect sub-type:
		0 	- Default
		1	- Stat
		2 	- Event
]]

local effects = {}

function getEffect( ID ) 
	if effects[ ID ] == nil then print( "Error: Wrong effect ID! ( ID: " .. ID .. " )" ) return end
	
	return effects[ ID ]
end

function addEffectToPlayer( ID )
	if effects[ ID ] ~= nil then myWorld:addEffectToPlayer( effects[ ID ] ) end
end

register( "AddEffectToPlayer",	addEffectToPlayer )

effects[ "EFFECT_SLOW_FROSTSHIELD" ] = {
	ID 			= "EFFECT_SLOW_FROSTSHIELD";
	type		= 1;
	sub_type	= 1;
	
	stat		= {
		index		= 9;
		value		= 200;
		operation	= 1;
	};
	
	functions 	= {
		onAplication	= function( parent ) print( "Effect is up!" ) end;
		onRemove		= function( parent ) print( "Effect is down!" ) end;
	};
}
