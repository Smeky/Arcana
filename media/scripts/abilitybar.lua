
--[[
	Description:
		Every ability bar related logic is in this file to keep things clean
]]


local myGui			= Gui
local myAssets		= Assets
local myEntity		= EntitySystem

local manager 		= {}

manager.abilityCD	= { false, false, false, false }
manager.isCD		= false
manager.updateCD	=
	function()
		local cdCounter = 0
		
		for i = 1, 4, 1 do
			if manager.abilityCD[ i ] then
				local ability 	= myEntity:getAbility( manager.player, i - 1 )
				local cooldown	= ability.maxCD - ability.curCD
				
				if cooldown == 0 then 
					manager.abilityCD[ i ] = false
					
					myGui:setText( manager.abButtons[ i ], {
						pos		= { 23, 20 };
						font	= "FONT_ARIAL";
						size	= 17;
						origin	= 0;
						string	= "";
					} )
				else
					myGui:setText( manager.abButtons[ i ], {
						pos		= { 23, 20 };
						font	= "FONT_ARIAL";
						size	= 17;
						origin	= 0;
						string	= roundNumber( cooldown, 1 );
					} )
				
					cdCounter = cdCounter + 1
				end 
			end -- if ability is on cd
		end

		if cdCounter == 0 then
			manager.isCD = false
		end
	end

function initAbilitybar()
	local window_W	= 700
	local window_H	= 128
	
	local bgTexture	= nil
	
		if charSelectionIndex == 1	then bgTexture = "GUI_TRACK_RESOURCE_RAGE_BG"; 			manager.texture = "GUI_TRACK_RESOURCE_RAGE_PROG";
	elseif charSelectionIndex == 2	then bgTexture = "GUI_TRACK_RESOURCE_STAMINA_BG"; 		manager.texture = "GUI_TRACK_RESOURCE_STAMINA_PROG";
	elseif charSelectionIndex == 3	then bgTexture = "GUI_TRACK_RESOURCE_FOCUS_BG"; 		manager.texture = "GUI_TRACK_RESOURCE_FOCUS_PROG";
	elseif charSelectionIndex == 4	then bgTexture = "GUI_TRACK_RESOURCE_ENERGY_BG"; 		manager.texture = "GUI_TRACK_RESOURCE_ENERGY_PROG";
	elseif charSelectionIndex == 5	then bgTexture = "GUI_TRACK_RESOURCE_MANA_BG"; 			manager.texture = "GUI_TRACK_RESOURCE_MANA_PROG";
	elseif charSelectionIndex == 6	then bgTexture = "GUI_TRACK_RESOURCE_LIFEENERGY_BG"; 	manager.texture = "GUI_TRACK_RESOURCE_LIFEENERGY_PROG";
	end
	
	manager.expTexture	= "GUI_EXPBAR_FRONT"
	
	Window_Abilitybar = myGui:newElement( {
		pos			= { ( GameWindow_W - window_W ) / 2, GameWindow_H - window_H };
		size		= { window_W, window_H };
		textures 	= { 
			"GUI_WINDOW_ABILITYBAR";
			"GUI_WINDOW_ABILITYBAR_FRONT";
		};
	} )
	
	manager.hpTrack = myGui:newElement( Window_Abilitybar, {
		size		= { 108, 108 };
		relPos		= { 8, 16 };
		
		textures	= { 
			"GUI_TRACK_HEALTH_BG";
			"GUI_TRACK_HEALTH_PROG";
		};
		
		texts		= {
			{
				pos		= { 54, 54 };
				font	= "FONT_ARIAL";
				size	= 16;
				origin	= 0;
				string	= "";
			};
		}
	} )
	
	manager.rpTrack = myGui:newElement( Window_Abilitybar, {
		size		= { 108, 108 };
		relPos		= { 583, 16 };
		
		textures	= { 
			bgTexture;
			manager.texture;
		};
		
		texts		= {
			{
				pos		= { 54, 54 };
				font	= "FONT_ARIAL";
				size	= 16;
				origin	= 0;
				string	= "";
			};
		}
	} )
	
	myGui:newElement( Window_Abilitybar, {
		size		= { 116, 116 };
		relPos		= { 4, 12 };
		
		textures	= {
			"GUI_TRACK_FRONT";
		};
	} )
	
	myGui:newElement( Window_Abilitybar, {
		size		= { 116, 116 };
		relPos		= { 579, 12 };
		
		textures	= {
			"GUI_TRACK_FRONT";
		};
	} )
	
	-- Experience bar
	manager.expBar = myGui:newElement( Window_Abilitybar, {
		size		= { 449, 14 };
		relPos		= { 125, 31 };
		
		textures	= {
			"GUI_EXPBAR_BG";
			manager.expTexture;
		};
	} )

	for i = 1, 4, 1 do
		manager.abButtons[ i ] = myGui:newButton( Window_Abilitybar, {
			size		= { 46, 46 };
			relPos		= { 73 + i * ( 50 + 8 ), 53 };
			offset		= { 1, 1 };
			hotkeys		= { 26 + i };
			
			active		= false;
			
			functions 	= {
				onMouseenter	= 
					function( b )
						local ability = manager.abilities[ i ]
						
						local x, y = myGui:getPos( b )
						
						ability.mouseBox = { x, y, myGui:getSize( b ) }
					
					
						setupTooltip( "ability", ability )
					end;
			}
		} )
		
		manager.abTextures[ i ]		= myGui:addTexture( 		manager.abButtons[ i ], "GUI_TOOLTIP_BG" )
		myGui:addButtonTexture( manager.abButtons[ i ], "GUI_BUTTON_ABILITY" )
	end
end

function abilitybarSetPlayer( ID )
	manager.player		= myEntity:getLightUserData( ID )
end

function resetAbilitybar()
	manager.player		= nil
	manager.hpTrack		= nil
	manager.rpTrack		= nil
	manager.texture		= nil
	manager.hpPrevMax	= nil
	manager.hpPrevCur	= nil
	manager.rpPrevMax	= nil
	manager.rpPrevCur	= nil
	manager.expBar		= nil
	manager.expTexture	= nil
	manager.expPrevMax	= nil
	manager.expPrevCur	= nil
	manager.abButtons	= {}
	manager.abTextures	= {}
	manager.abilities	= {}
	manager.buffer		= nil		-- Used to store new abilities before usage of player is possible
end

local function newAbility( index )
	if index ~= nil then
		if manager.player == nil then
			if isTableEmpty( manager.buffer ) then 
				manager.buffer = {}
			end
		
			manager.buffer[ index + 1 ] = index
		else
			manager.abilities[ index + 1 ] = myEntity:getAbility( manager.player, index )
			

			myGui:setTexture( manager.abButtons[ index + 1 ], manager.abilities[ index + 1 ].texture, manager.abTextures[ index + 1 ] )
			
			myGui:setActive( manager.abButtons[ index + 1 ], true )			
		end
	else
		print( "Error: unable to setup ability button. Wrong index: " .. type( index ) )
	end
end

local function update()
	-- Make sure player can be used
	if manager.player ~= nil then
		-- Setup abilities from buffer ( on initialization )
		if manager.buffer ~= nil then		
			for _, v in ipairs( manager.buffer ) do
				
				newAbility( v )
			end
			
			manager.buffer = nil
		end
		
		if manager.isCD then 
			manager.updateCD()
		end
	
		-- Store players hp and rp
		local hpNewMax, hpNewCur = myEntity:getHealth( 		manager.player )
		local rpNewMax, rpNewCur = myEntity:getResource( 	manager.player )
		
		-- In case health has changed, update interface
		if hpNewMax ~= manager.hpPrevMax or hpNewCur ~= manager.hpPrevCur then
			-- Calculate factor of current health
			local factor 	= hpNewCur / hpNewMax
			-- Store proggress bar textures size
			local tW, tH 	= myAssets:getTextureSize( "GUI_TRACK_HEALTH_PROG" )
			
			-- Set progress bar texture with correct clip, size and position
			myGui:setTexture( manager.hpTrack, {
				pos			= { 0, tH - tH * factor };
				size		= { tW, tH * factor };
				texture		= "GUI_TRACK_HEALTH_PROG";
				clip		= { 0, tH - tH * factor, tW, tH * factor };
			}, 0 )
			
			-- Set updated text
			myGui:setText( manager.hpTrack, {
				pos		= { 54, 54 };
				font	= "FONT_ARIAL";
				size	= 16;
				origin	= 0;
				string	= hpNewCur .. "/" .. hpNewMax;
			}, 0 )
			
			-- Store new health
			manager.hpPrevMax = hpNewMax
			manager.hpPrevCur = hpNewCur
		end
		-- In case resource has changed, update interface
		if rpNewMax ~= manager.rpPrevMax or rpNewCur ~= manager.rpPrevCur then
			-- Calculate factor of current resource
			local factor 	= rpNewCur / rpNewMax
			-- Store proggress bar textures size
			local tW, tH 	= myAssets:getTextureSize( manager.texture )
			
			-- Set progress bar texture with correct clip, size and position
			myGui:setTexture( manager.rpTrack, {
				pos			= { 0, tH - tH * factor };
				size		= { tW, tH * factor };
				texture		= manager.texture;
				clip		= { 0, tH - tH * factor, tW, tH * factor };
			}, 0 )
			
			-- Set updated text
			myGui:setText( manager.rpTrack, {
				pos		= { 54, 54 };
				font	= "FONT_ARIAL";
				size	= 16;
				origin	= 0;
				string	= rpNewCur .. "/" .. rpNewMax;
			}, 0 )
			
			-- Store new resource
			manager.rpPrevMax = rpNewMax
			manager.rpPrevCur = rpNewCur
		end
		
		-- Store players experience
		local expMax, expCur	= myEntity:getExperience( manager.player )
		
		-- Update experience bar
		if expMax ~= manager.expPrevMax or expCur ~= manager.expPrevCur then
			-- Calculate factor of current experience
			local factor 	= expCur / expMax
			-- Store proggress bar textures size
			local tW, tH 	= myAssets:getTextureSize( manager.expTexture )
		
			-- Set progress bar texture with correct clip, size and position
			myGui:setTexture( manager.expBar, {
				pos			= { 0, 0 };
				size		= { tW * factor, tH };
				texture		= manager.expTexture;
				clip		= { 0, 0, tW * factor, tH };
			}, 0 )
			
			-- Store new experience
			manager.expPrevMax = expMax
			manager.expPrevCur = expCur
		end
	end
end

local function abilityCasted( index )
	manager.isCD = true

	manager.abilityCD[ index + 1 ] = true
end

register( "UpdateWorld", 		update )
register( "PlayerNewAbility",	newAbility )
register( "PlayerAbilityCasted",abilityCasted )