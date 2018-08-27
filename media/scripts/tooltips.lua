
--[[
	Main purpose of this file is to keep code clean and all tooltip releated stuff in a single place

	Currently supported tooltips:
		- Item
		
	Values:
		mouseBox		- Tooltip will be destroyed when mouse leaves this box
		width			- Width of the tooltip
		textureBg		- Background texture
		icon			- Single image
		texts			- Table of text wrappers
]]

function setupTooltip( type, data )
		if type == "item" 		then setupItemTp( data )
	elseif type == "upgrade"	then setupUpgradeTp( data )
	elseif type == "ability"	then setupAbilityTp( data )
	else print( "Unable to setup tooltip. Wrong type: " .. type )
	end
end

function setupItemTp( item )
	-- Contains every text wrapper that will be used in the tooltip
	local textCointainer = {}
	
	local titleColor
	local rarity 	= item.rarity
	
	-- Get correct color based on item rarity
		if rarity == 0 then titleColor = { 200, 200, 200 }	-- no rarity
	elseif rarity == 1 then titleColor = { 120, 215, 60 }	-- common
	elseif rarity == 2 then titleColor = { 60,  130, 215 }	-- rare
	elseif rarity == 3 then titleColor = { 170, 60,  215 }	-- epic
	elseif rarity == 4 then titleColor = { 250, 170, 50 }	-- legendary
	end
	
	-- Tooltip title by item name
	local textTitle = { 
		pos 	= { 110, 10 };
		font	= "FONT_ARIAL";
		size	= 16;
		style	= 0;
		color	= titleColor;
		origin	= 0;
		string 	= item.name;
	}
	
	-- Add tooltip title to the container
	table.insert( textCointainer, textTitle )
	
	-- Counts amount of stats in item ( used for correct item description position )
	local statCount = 0
	
	-- Loop that goes through all stats in item
	for i, v in ipairs( item.stats ) do
		-- Divide item stat value into rounded number and decimal part
		local num, dec = math.modf( v.value )
	
		-- Item stat value as string
		local valueString
		
		-- Check if there is decimal part and add it, otherwise use rounded number ( prevents .0 )
		if dec == 0.0 then 	valueString = num
		else valueString = roundNumber( v.value, 1 )
		end

		-- Add operand and stat value to string
		local statString
		statString = operandAsString( v.operand )
		statString = statString .. " " .. valueString
		
		-- In case operand is percent, add percent character after number
			if v.operand == 4 then statString = statString .. "%"
		elseif v.operand == 5 then statString = statString .. "%"
		end

		-- Add stat name as string
		statString = statString .. "  " .. charStatAsString( v.index )
	
	
		-- Set stat color based on its operand
		local valueColor
			if v.operand == 0	then valueColor = { 0, 255,   0 }
		elseif v.operand == 1	then valueColor = { 255, 0,   0 }
		elseif v.operand == 4	then valueColor = { 0, 	 255, 0 }
		elseif v.operand == 5	then valueColor = { 255, 0,   0 }
		end
	
		-- Text wrapper for stat value
		local textValue = { 
			pos 	= { 10, 25 + i * 16 };
			font	= "FONT_ARIAL";
			size	= 15;
			style	= 0;
			color	= valueColor;
			origin	= 1;
			string 	= statString;
		}
		
		-- Insert stat text to the container
		table.insert( textCointainer, textValue )
		
		-- Iterate stat counter
		statCount = statCount + 1
	end
	
	-- Text wrapper for item description 
	local textDescription = { 
		pos 	= { 15, 55 + statCount * 16 };
		font	= "FONT_ARIAL";
		size	= 15;
		style	= 0;
		color	= { 255, 255, 255 };
		origin	= 1;
		string 	= item.description;
	}
	
	-- Add item description text to the container
	table.insert( textCointainer, textDescription )
	
	-- Create tooltip table
	local tooltip = {
		width		= 220;
		mouseBox	= item.mouseBox;
		textureBg	= "GUI_TOOLTIP_BG";
		texts		= textCointainer;	
	}
	
	-- Create tooltip
	myGui:createTooltip( tooltip )
end

function setupUpgradeTp( upgrade )
	local textCointainer = {}

	-- Tooltip title of upgrade name
	local textTitle = { 
		pos 	= { 110, 10 };
		font	= "FONT_ARIAL";
		size	= 16;
		style	= 0;
		color	= { 255, 255, 255, 255 };
		origin	= 0;
		string 	= upgrade.name;
	}
	
	table.insert( textCointainer, textTitle )
	
	-- Text wrapper for upgrade description 
	local textDescription = { 
		pos 	= { 15, 55 };
		font	= "FONT_ARIAL";
		size	= 15;
		style	= 0;
		color	= { 255, 255, 255 };
		origin	= 1;
		string 	= upgrade.description;
	}
	
	table.insert( textCointainer, textDescription )

	-- Create tooltip table
	local tooltip = {
		width		= 220;
		mouseBox	= upgrade.mouseBox;
		textureBg	= "GUI_TOOLTIP_BG";
		texts		= textCointainer;	
	}
	
	myGui:createTooltip( tooltip )
end

function setupAbilityTp( ability )
	local textCointainer = {}
	
		-- Tooltip title if ability name
	local textTitle = { 
		pos 	= { 110, 10 };
		font	= "FONT_ARIAL";
		size	= 16;
		style	= 0;
		color	= { 255, 255, 255, 255 };
		origin	= 0;
		string 	= ability.name;
	}
	
	table.insert( textCointainer, textTitle )
	
	-- Ability cost
	local textMana = { 
		pos 	= { 15, 55 };
		font	= "FONT_ARIAL";
		size	= 15;
		style	= 0;
		color	= { 255, 255, 255, 255 };
		origin	= 1;
		string 	= "Cost: " .. ability.cost;
	}
	
	table.insert( textCointainer, textMana )
	
	-- Ability cast time
	local castString
	if ability.cast == 0.0 then castString = "Cast: instant"
	else castString = "Cast: " .. roundNumber( ability.cast, 1 ) .. "s"
	end
	
	local textCast = { 
		pos 	= { 15, 75 };
		font	= "FONT_ARIAL";
		size	= 15;
		style	= 0;
		color	= { 255, 255, 255, 255 };
		origin	= 1;
		string 	= castString;
	}
	
	table.insert( textCointainer, textCast )
		
	-- Ability cooldown
	local cdString
	if ability.maxCD == 0.0 then cdString = "CD: none"
	else cdString = "CD: " .. roundNumber( ability.maxCD, 1 ) .. "s"
	end
	
	local textCD = { 
		pos 	= { 15, 95 };
		font	= "FONT_ARIAL";
		size	= 15;
		style	= 0;
		color	= { 255, 255, 255, 255 };
		origin	= 1;
		string 	= cdString;
	}
	
	table.insert( textCointainer, textCD )
	
	-- Text wrapper for ability description 
	local textDescription = { 
		pos 	= { 15, 130 };
		font	= "FONT_ARIAL";
		size	= 15;
		style	= 0;
		color	= { 255, 255, 255 };
		origin	= 1;
		string 	= ability.description;
	}
	
	table.insert( textCointainer, textDescription )
	
	-- Create tooltip table
	local tooltip = {
		width		= 220;
		mouseBox	= ability.mouseBox;
		textureBg	= "GUI_TOOLTIP_BG";
		texts		= textCointainer;	
	}
	
	myGui:createTooltip( tooltip )
end







