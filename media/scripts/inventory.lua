


Itemslot_ID = {}

function initInventoryWindow()
	local itemslots = {}

	local window_W 	= 506
	local window_H 	= 266

	local slot_W	= 50
	local slot_H	= 50

	local offset	= 2

	local allSlots_W 	= ( slot_W + offset ) * 7
	local allSlots_H 	= ( slot_H + offset ) * 3

	local allSlots_X	= 234
	local allSlots_Y	= 30

    local equipCenter_X = 85
	local equipBase_Y	= 23
    local slotOffset    = 4

    -- Helm
	table.insert( itemslots, {
		type 		= "ITEMSLOT";
		pos			= { equipCenter_X, equipBase_Y };
		size		= { slot_W, slot_H };
	} )

	-- Amulet
	table.insert( itemslots, {
		type 		= "ITEMSLOT";
		pos			= { equipCenter_X + slot_W + slotOffset, equipBase_Y + slot_H / 2 };
		size		= { slot_W, slot_H };
	} )

	-- Chest
	table.insert( itemslots, {
		type 		= "ITEMSLOT";
		pos			= { equipCenter_X, equipBase_Y + slot_H + slotOffset };
		size		= { slot_W, slot_H };
	} )

	-- Pants
	table.insert( itemslots, {
		type 		= "ITEMSLOT";
		pos			= { equipCenter_X, equipBase_Y + ( slot_H + slotOffset ) * 2 };
		size		= { slot_W, slot_H };
	} )

	-- Gloves
	table.insert( itemslots, {
		type 		= "ITEMSLOT";
		pos			= { equipCenter_X - slot_W - slotOffset, equipBase_Y + ( slot_H + slotOffset ) * 1 + slot_H / 2 };
		size		= { slot_W, slot_H };
	} )

	-- Shoes
	table.insert( itemslots, {
		type 		= "ITEMSLOT";
		pos			= { equipCenter_X, equipBase_Y + ( slot_H + slotOffset ) * 3 };
		size		= { slot_W, slot_H };
	} )

	-- MainHand
	table.insert( itemslots, {
		type 		= "ITEMSLOT";
		pos			= { equipCenter_X - slot_W - slotOffset, equipBase_Y + ( slot_H + slotOffset ) * 2 + slot_H / 2 };
		size		= { slot_W, slot_H };
	} )
	
	-- OffHand
	table.insert( itemslots, {
		type 		= "ITEMSLOT";
		pos			= { equipCenter_X + slot_W + slotOffset, equipBase_Y + ( slot_H + slotOffset ) * 2 + slot_H / 2 };
		size		= { slot_W, slot_H };
	} )

	for yPos = 0, 3, 1 do
		for xPos = 0, 4, 1 do
			table.insert( itemslots, {
				type 		= "ITEMSLOT";
				pos			= { allSlots_X + xPos * ( slot_W + offset ), allSlots_Y + yPos * ( slot_H + offset ) };
				size		= { slot_W, slot_H };
			} )

		end
	end

	local windowInv = {
		pos			= { GameWindow_W - window_W - 50, ( GameWindow_H - window_H ) / 2 };
		size		= { window_W, window_H };
		texture		= "GUI_WINDOW_INVENTORY";

		widgets		= itemslots;
	}

	Window_Inventory_ID = myGui:createInventory( windowInv )

	Itemslot_ID		= myGui:getWindowWidgetsID( Window_Inventory_ID )
	
	-- Remove stat display ID accessing correct item slots is more clear
	table.remove( Itemslot_ID, 1 )

	myGui:reverseWindowShown( Window_Inventory_ID )
end

function initInventory()
	initInventoryWindow()
end

function isItemSlot( ID )
	for i, v in ipairs( Itemslot_ID ) do 
		if v == ID then return true end
	end
	
	return false
end

function getItemSlotIndex( ID )
	for i, v in ipairs( Itemslot_ID ) do
		if ID == v then
			return i
		end
	end
	
	return nil
end

function inventoryMouseEnter( WidgetID )
	if isItemSlot( WidgetID ) == true then 
		
		local index			= getItemSlotIndex( WidgetID )

		if myInventory:hasItem( index ) == true then
			local itemslotBox 	= myGui:getWidgetBox( Window_Inventory_ID, WidgetID )
			
			local itemData  	= myInventory:getItemData( index )
			
			itemData[ "mouseBox" ] = itemslotBox
			
			setupTooltip( "item", itemData )
		end
	end
end

function inventoryItemMove( itemIndex )
	if myInventory:hasItem( itemIndex ) == true then
		local itemslotBox 	= myGui:getWidgetBox( Window_Inventory_ID, Itemslot_ID[ itemIndex + 1 ] )
		
		local itemData		= myInventory:getItemData( itemIndex )
		
		itemData[ "mouseBox" ] = itemslotBox
		
		setupTooltip( "item", itemData )
	end
end

local itemToDestroyIndex

function inventoryShowDestroyWindow( itemIndex ) 
	myGui:reverseWindowShown( Window_ItemDestroy_ID )
	
	itemToDestroyIndex = itemIndex
end

function inventoryDestroyItem()
	if itemToDestroyIndex ~= nil then
		myInventory:delItem( itemToDestroyIndex )
	end
	
	myGui:reverseWindowShown( Window_ItemDestroy_ID )
end

function inventoryKeepItem()
	
	myGui:reverseWindowShown( Window_ItemDestroy_ID )
	
	itemToDestroyIndex = nil
end

register( "ButtonMouseEnter", 		inventoryMouseEnter )
register( "PlayerItemMove",			inventoryItemMove )
register( "InventoryDestroyItem",	inventoryShowDestroyWindow )
register( "ItemDestroyAccept",		inventoryDestroyItem )
register( "ItemDestroyDecline",		inventoryKeepItem )

-- Return stat name as string based on its index
function charStatAsString( index )
		if index == 0	then return "Health"
	elseif index == 1	then return "Resource"
	elseif index == 2	then return "Strength"
	elseif index == 3	then return "Dexterity"
	elseif index == 4	then return "Intelligence"
	elseif index == 5	then return "Vitality"
	elseif index == 6	then return "Armor"
	elseif index == 7	then return "HP regen"
	elseif index == 8	then return "RP regen"
	elseif index == 9	then return "Move spd."
	elseif index == 10	then return "Att spd."
	elseif index == 11	then return "Cast spd."
	elseif index == 12	then return "Min damage"
	elseif index == 13	then return "Max damage"
	elseif index == 14	then return "Crit Chance"
	elseif index == 15	then return "Crit Multip"
	elseif index == 16	then return "Exp Multip"
	end
end

-- Return stat operand as string
function operandAsString( operand ) 
		if operand == 0	then return "+"
	elseif operand == 1 then return "-"
	elseif operand == 2 then return "*"
	elseif operand == 3 then return "/"
	elseif operand == 4 then return "+"
	elseif operand == 5 then return "-"
	end
end

















