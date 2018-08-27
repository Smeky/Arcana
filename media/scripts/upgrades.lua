
--[[
	Upgrade data:
		
		ID
		name
		description
		texture
		locked
		level			-- Base level ( leave it, so base level is 0 )
		levelMax
		unlocks = { 1 = "", 2 = "", 3 = "" }	-- ID of upgrades that should be unlocked once this upgrade hits maximum level
		prices	= { 1 = , 2 = , .. }			-- Prices for each level ( amount of values must be equal to level max )
		pos		= {  }							-- Position used in upgrade window setup
]]


-- Forwrad declaration
local upgrades = {}

function initUpgrades()
	myWorld:loadUpgrades( upgrades )
end

register( "GameInit", initUpgrades )

function setupUpgrades()
	myWorld:setupUpgrades();
end

function getUpgrade( ID ) 
	if upgrades[ ID ] == nil then print( "Error: Wrong upgrade ID! ( ID: " .. ID .. " )" ) return end
	
	return upgrades[ ID ]
end

function getAllUpgrades() 
	return upgrades
end

upgrades[ "UPG_HEALTH_MAX" ] = {
	ID				= "UPG_HEALTH_MAX";
	name			= "Increase maximum health";
	description		= "Increase maximum health by 2% per level";
	texture			= "GUI_ICON_UPG_HEALTH_MAX";
	locked			= false;
	levelMax		= 5;
	
	unlocks			= {
		"UPG_RESOURCE_MAX";
		"UPG_MOVESPD";
	};
	
	prices			= {
		10;
		15;
		20;
		25;
		30;
	};
	
	pos				= { 100, 50 };
}

upgrades[ "UPG_RESOURCE_MAX" ] = {
	ID				= "UPG_RESOURCE_MAX";
	name			= "Increase maximum resource";
	description		= "Increase maximum resource by 1% per level";
	texture			= "GUI_ICON_UPG_RESOURCE_MAX";
	levelMax		= 5;
	
	prices			= {
		10;
		15;
		20;
		25;
		30;
	};
	
	pos				= { 70, 110 };
}
 
upgrades[ "UPG_MOVESPD" ] = {
	ID				= "UPG_MOVESPD";
	name			= "Increase movement speed";
	description		= "increase speed";
	texture			= "GUI_ICON_UPG_MOVESPD";
	levelMax		= 3;
	
	unlocks			= {
		"UPG_HEALTH_REGEN";
	};
	
	prices			= {
		30;
		40;
		50;
	};
	
	pos				= { 130, 110 };
}

upgrades[ "UPG_HEALTH_REGEN" ] = {
	ID				= "UPG_HEALTH_REGEN";
	name			= "Increase health regen";
	description		= "Increase health regen";
	texture			= "GUI_ICON_UPG_HEALTH_REGEN";
	levelMax		= 5;
	
	prices			= {
		10;
		15;
		20;
		25;
		30;
	};
	
	pos				= { 130, 170 };
}