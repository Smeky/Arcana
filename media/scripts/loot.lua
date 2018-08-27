

--[[
	Item category hierarchy:
	
	anyitem
		equipment
			armor
				helm
				chest
				..
			weapon
				sword
				bow
				wand
		consumable
			potion
			useitem
			crystalcore
		junk
]]


-- Forward decalaration
mainTable	= {}

mainTable.equipment = {}
mainTable.equipment.armor = {}
mainTable.equipment.armor.helm = {}
mainTable.equipment.weapon = {}
mainTable.equipment.weapon.sword = {}
mainTable.consumable = {}
mainTable.consumable.potion = {}
mainTable.consumable.useitem = {}
mainTable.consumable.crystalcore = {}
mainTable.junk = {}


function getLootTable( category )
		if category == "Anyitem" 			then return mainTable
	elseif category == "Equipment"			then return mainTable.equipment
	elseif category	== "Consumable"			then return mainTable.consumable
	elseif category == "Junk"				then return mainTable.junk
	else print( "Warning: Wrong category name: " .. category ); return nil
	end
end

function createLootBag( itemTable )
	local lootBag	= getObject( "LOOT_BAG" )
	
	lootBag.pos = { itemTable.posX, itemTable.posY }
	
	lootBag.functions = {}
	lootBag.functions.onInteract = 
		function( ID ) 
			if myInventory:isInventoryFull() == false then
				myInventory:addItem( getItem( itemTable.ID ) );  
			
				myWorld:despawnObject( ID )
			end
		end
	
	myWorld:createObject( lootBag )
end

function createRunestone( runestoneTable )
	local runestone = getObject( "RUNESTONE" )
	
	runestone.pos = { runestoneTable.posX, runestoneTable.posY }
	
	runestone.functions = {}
	runestone.functions.onInteract = 
		function( ID ) 
			myInventory:addRunestones( runestoneTable.amount );
			
			myWorld:despawnObject( ID )
		end
	
	myWorld:createObject( runestone )
end

register( "CreateLootBag",		createLootBag )
register( "CreateRunestones",	createRunestone )

-- Equipment
-- Armor
-- Helm
mainTable.equipment.armor.helm = {
	items = {
		{ ID = "ITEM_HELM_LEATHER", 	level = 3,	chance = 30 };
		{ ID = "ITEM_HELM_IRON", 		level =	6, 	chance = 30 };
	};
}

mainTable.equipment.armor.amulet = {
		{ ID = "ITEM_AMULET_AENOANET", 	level = 8,	chance = 3 };
}


mainTable.equipment.armor.chest = {
		{ ID = "ITEM_CHEST_LEATHER", 	level = 3,	chance = 30 };
		{ ID = "ITEM_CHEST_IRON", 		level =	6, 	chance = 30 };
}


mainTable.equipment.armor.pants = {
		{ ID = "ITEM_PANTS_LEATHER", 	level = 3,	chance = 30 };
		{ ID = "ITEM_PANTS_IRON", 		level =	6, 	chance = 30 };
}


mainTable.equipment.armor.gloves = {
		{ ID = "ITEM_GLOVES_LEATHER", 	level = 3,	chance = 30 };
		{ ID = "ITEM_GLOVES_IRON", 		level =	6, 	chance = 30 };
}


mainTable.equipment.armor.shoes = {
		{ ID = "ITEM_SHOES_LEATHER", 	level = 3,	chance = 30 };
		{ ID = "ITEM_SHOES_IRON", 		level =	6, 	chance = 30 };
}

-- Weapon
mainTable.equipment.weapon.sword = {

}

-- Consumable
mainTable.consumeable.potion = {

}

mainTable.consumeable.useitem = {

}

mainTable.consumeable.crystalcore = {

}

mainTable.junk = {

}