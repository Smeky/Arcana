



--[[
	Item slots:
	   -1 	- Unknown
		0	- Helm
		1	- Amulet
		2	- Chest
		3	- Pants
		4	- Gloves
		5	- Shoes
		6	- Main hand
		7	- Off hand
]]

--[[
	Item rarity: 
		0 	- No rarity
		1	- Commons
		2	- Rare
		3	- Epic
		4	- Legendary
]]

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
]]

--[[
	Stat calculation operation type:
		0	- Add
		1	- Remove
		2 	- Multiply
		3 	- Divide
		4 	- Percent Add
		5	- Percent Remove
]]

-- Forward declaration
local item = {}

function getItem( ID )
	if item[ ID ] == nil then print( "Error: Wrong item ID! ( ID: " .. ID .. " )" ) return end

	return item[ ID ]
end

-- Amulets 

item[ "ITEM_AMULET_AENOANET" ] = {
	ID			= "ITEM_AMULET_AENOANET";
	name		= "Aenoa's Amulet";
	description	= "Amulet of the first supporter, AenoaNET";
	textureID	= "GUI_ITEM_AMULET_AENOANET";
	rarity		= 4;
	slot		= 1;
	stats		= {
		{ stat = 2, 	value = 20, 	operand = 0 };
		{ stat = 3, 	value = 200, 	operand = 0 };
		{ stat = 4, 	value = 6, 		operand = 0 };
		{ stat = 5, 	value = 5, 		operand = 0 };
		{ stat = 8, 	value = 3, 		operand = 0 };
		{ stat = 9, 	value = 200, 	operand = 0 };
		{ stat = 12, 	value = 20, 	operand = 0 };
	};
	effectID	= "EFFECT_SLOW_FROSTSHIELD"
}

-- Leather set

item[ "ITEM_HELM_LEATHER" ] = {
	ID			= "ITEM_HELM_LEATHER";
	name		= "Leather helmet";
	description	= "Made out of tasty cow";
	textureID	= "GUI_ITEM_HELM_LEATHER";
	rarity		= 1;
	slot		= 0;
	stats		= {
		{ stat = 2, 	value = 2, 	operand = 0 };
		{ stat = 3, 	value = 2, 	operand = 0 };
		{ stat = 4, 	value = 2, 	operand = 0 };
		{ stat = 5, 	value = 1, 	operand = 0 };
	};
}

item[ "ITEM_CHEST_LEATHER" ] = {
	ID			= "ITEM_CHEST_LEATHER";
	name		= "Leather chest";
	description	= "";
	textureID	= "GUI_ITEM_CHEST_LEATHER";
	rarity		= 1;
	slot		= 2;
	stats		= {
		{ stat = 2, 	value = 3, 	operand = 0 };
		{ stat = 3, 	value = 3, 	operand = 0 };
		{ stat = 4, 	value = 3, 	operand = 0 };
		{ stat = 5, 	value = 4, 	operand = 0 };
	};
}

item[ "ITEM_PANTS_LEATHER" ] = {
	ID			= "ITEM_PANTS_LEATHER";
	name		= "Leather pants";
	description	= "";
	textureID	= "GUI_ITEM_PANTS_LEATHER";
	rarity		= 1;
	slot		= 3;
	stats		= {
		{ stat = 2, 	value = 3, 	operand = 0 };
		{ stat = 3, 	value = 3, 	operand = 0 };
		{ stat = 4, 	value = 3, 	operand = 0 };
		{ stat = 5, 	value = 2, 	operand = 0 };
	};
}

item[ "ITEM_GLOVES_LEATHER" ] = {
	ID			= "ITEM_GLOVES_LEATHER";
	name		= "Gloves of the LEATHERwolf";
	description	= "Made out of wild steel";
	textureID	= "GUI_ITEM_GLOVES_LEATHER";
	rarity		= 1;
	slot		= 4;
	stats		= {
		{ stat = 2, 	value = 2, 	operand = 0 };
		{ stat = 3, 	value = 2, 	operand = 0 };
		{ stat = 4, 	value = 2, 	operand = 0 };
		{ stat = 5, 	value = 1, 	operand = 0 };
	};
}

item[ "ITEM_SHOES_LEATHER" ] = {
	ID			= "ITEM_SHOES_LEATHER";
	name		= "Leather boots";
	description	= "Made out of wild steel";
	textureID	= "GUI_ITEM_SHOES_LEATHER";
	rarity		= 1;
	slot		= 5;
	stats		= {
		{ stat = 2, 	value = 1, 	operand = 0 };
		{ stat = 3, 	value = 1, 	operand = 0 };
		{ stat = 4, 	value = 1, 	operand = 0 };
		{ stat = 5, 	value = 20, operand = 0 };
		{ stat = 6, 	value = 20, operand = 0 };
		{ stat = 7, 	value = 20, operand = 0 };
		{ stat = 8, 	value = 20, operand = 0 };
		{ stat = 9, 	value = 20, operand = 0 };
	};
}

-- Iron set

item[ "ITEM_HELM_IRON" ] = {
	ID			= "ITEM_HELM_IRON";
	name		= "Helm of the iron wolf";
	description	= "Made out of wild steel";
	textureID	= "GUI_ITEM_HELM_IRON";
	rarity		= 2;
	slot		= 0;
	stats		= {
		{ stat = 2, 	value = 6, 	operand = 0 };
		{ stat = 3, 	value = 6, 	operand = 0 };
		{ stat = 4, 	value = 6, 	operand = 0 };
		{ stat = 5, 	value = 5, 	operand = 0 };
	};
}

item[ "ITEM_CHEST_IRON" ] = {
	ID			= "ITEM_CHEST_IRON";
	name		= "Helm of the iron wolf";
	description	= "Made out of wild steel";
	textureID	= "GUI_ITEM_CHEST_IRON";
	rarity		= 2;
	slot		= 2;
	stats		= {
		{ stat = 2, 	value = 6, 	operand = 0 };
		{ stat = 3, 	value = 6, 	operand = 0 };
		{ stat = 4, 	value = 6, 	operand = 0 };
		{ stat = 5, 	value = 5, 	operand = 0 };
	};
}

item[ "ITEM_PANTS_IRON" ] = {
	ID			= "ITEM_PANTS_IRON";
	name		= "Pants of the iron wolf";
	description	= "Made out of wild steel";
	textureID	= "GUI_ITEM_PANTS_IRON";
	rarity		= 2;
	slot		= 3;
	stats		= {
		{ stat = 2, 	value = 6, 	operand = 0 };
		{ stat = 3, 	value = 6, 	operand = 0 };
		{ stat = 4, 	value = 6, 	operand = 0 };
		{ stat = 5, 	value = 5, 	operand = 0 };
	};
}

item[ "ITEM_GLOVES_IRON" ] = {
	ID			= "ITEM_GLOVES_IRON";
	name		= "Gloves of the iron wolf";
	description	= "Made out of wild steel";
	textureID	= "GUI_ITEM_GLOVES_IRON";
	rarity		= 2;
	slot		= 4;
	stats		= {
		{ stat = 2, 	value = 6, 	operand = 0 };
		{ stat = 3, 	value = 6, 	operand = 0 };
		{ stat = 4, 	value = 6, 	operand = 0 };
		{ stat = 5, 	value = 5, 	operand = 0 };
	};
}

item[ "ITEM_SHOES_IRON" ] = {
	ID			= "ITEM_SHOES_IRON";
	name		= "Boots of the iron wolf";
	description	= "Made out of wild steel";
	textureID	= "GUI_ITEM_SHOES_IRON";
	rarity		= 2;
	slot		= 5;
	stats		= {
		{ stat = 2, 	value = 6, 	operand = 0 };
		{ stat = 3, 	value = 6, 	operand = 0 };
		{ stat = 4, 	value = 6, 	operand = 0 };
		{ stat = 5, 	value = 5, 	operand = 0 };
		{ stat = 9, 	value = 40, operand = 0 };
	};
}	

-- Weapons 

item[ "ITEM_SWORD_DODYBRIO" ] = {
	ID			= "ITEM_SWORD_DODYBRIO";
	name		= "DodyBrio's Sword";
	description	= "";
	rarity		= 3;
	slot		= 6;
	stats		= {
		{ stat = 2, 	value = 2, 	operand = 0 };
		{ stat = 3, 	value = 2, 	operand = 0 };
		{ stat = 4, 	value = 2, 	operand = 0 };
		{ stat = 5, 	value = 1, 	operand = 0 };
	};
}

