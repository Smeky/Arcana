

local myWorld		= World
local myCore		= Core
local myGui			= Gui
local myInput		= Input
local myAssets		= Assets
local myEntity		= EntitySystem

function initPausescreen() 
	local window_W	= 300
	local window_H	= 400
	
	Window_Pause = myGui:newElement( { 
		pos			= { ( GameWindow_W - window_W ) / 2, ( GameWindow_H - window_H ) / 2 };
		size		= { window_W, window_H };
		textures 	= { "GUI_WINDOW_PAUSESCREEN" };
		shown		= false;
		
		childs		= {
			{
				type		= "button";
				size		= { 150, 30 };
				relPos		= { ( window_W - 150 ) / 2, 50 };
				buttonTex	= "GUI_BUTTON_PAUSEMENU_MENU";
				msg			= "GuiButtonPauseMenu";
			};
			{
				type		= "button";
				size		= { 150, 30 };
				relPos		= { ( window_W - 150 ) / 2, window_H - 70 };
				buttonTex	= "GUI_BUTTON_MAINMENU_EXIT";
				msg			= "GuiButtonMenuExit";
				hotkeys 	= { 4 };
			};
		};
	} )
end

function initMapWindow()
	local window_W	= 850
	local window_H	= 560
	
	Window_Map	= myGui:newElement( { 
		pos			= { ( GameWindow_W - window_W ) / 2, ( GameWindow_H - window_H ) / 2 };
		size		= { window_W, window_H };
		textures 	= { 
			"GUI_WINDOW_PAUSESCREEN";
			"GUI_MAP_BG";
			};
		shown		= false;
		
		childs		= {
			{
				type		= "button";
				size		= { 35, 35 };
				relPos		= { window_W - 38, 3 };
				
				buttonTex	= "GUI_BUTTON_WINDOW_EXIT";
				
				functions	= {
					onClick		= function( b ) closeWindow( Window_Map ) end;
				};
			};
			{
				type 		= "button";
				relPos		= { 393, 273 };
				size		= { 265, 205 };
				buttonTex 	= "GUI_BUTTON_GRASS";
				
				functions	= {
					onClick	= function() createLevel( "ZONE_1_LEVEL_1" ) end;
				};
			};
		};
	} )
end



function initCastbar() 
	local window_W	= 250
	local window_H	= 30

	local windowCastBar = {
		pos				= { ( GameWindow_W - window_W ) / 2, GameWindow_H - 208 };
		size			= { window_W, window_H };
		text			= 	{
								pos 	= { 125, 13 };
								font	= "FONT_ARIAL";
								size	= 18;
								style	= 0;
								color	= { 255, 255, 255 };
								origin	= 0;
							};
		textureBorder 	= "GUI_WINDOW_CASTBAR_BORDER";
		textureBg		= "GUI_WINDOW_CASTBAR_BG";
		textureProg		= "GUI_WINDOW_CASTBAR_PROG";
	}
	
	Window_Castbar_ID = myGui:createCastbar( windowCastBar )
end

function initItemDestroyWindow()
	local window_W	= 450
	local window_H	= 120

	local windowItemDestroy = {
		pos			= { ( GameWindow_W - window_W ) / 2, 300 };
		size		= { window_W, window_H };
		texture 	= "GUI_TOOLTIP_BG";
		
		widgets		= {
			{
				type		= "BUTTON";
				pos			= { 20, 80 };
				size		= { 150, 30 };
				texture		= "GUI_BUTTON_MAINMENU_PLAY";
				msg			= "ItemDestroyAccept";
				hotkey		= 58;
			};
			{
				type		= "BUTTON";
				pos			= { 280, 80 };
				size		= { 150, 30 };
				texture		= "GUI_BUTTON_MAINMENU_EXIT";
				msg			= "ItemDestroyDecline";
			};
		};
	}
	
	Window_ItemDestroy_ID = myGui:createWindow( windowItemDestroy )
	myGui:reverseWindowShown( Window_ItemDestroy_ID )
end

local upgradeManager 	= {}
upgradeManager.buttons 	= {}
upgradeManager.IDs		= {}
upgradeManager.getID 	= function( Button )
	return upgradeManager.IDs[ Button ]
end
upgradeManager.getButton =  function( ID )
	return upgradeManager.buttons[ ID ]
end

function initUpgradeWindow()
	local window_W	= 700
	local window_H	= 500
	
	Window_Upgrade = myGui:newElement( {
		pos			= { ( GameWindow_W - window_W ) / 2, ( GameWindow_H - window_H ) / 2 };
		size		= { window_W, window_H };
		textures 	= { "GUI_WINDOW_PAUSESCREEN" };
		shown		= false;
		
		childs		= {
			{
				type		= "button";
				size		= { 35, 35 };
				relPos		= { window_W - 38, 3 };
				
				buttonTex	= "GUI_BUTTON_WINDOW_EXIT";
				
				functions	= {
					onClick		= function( b ) closeWindow( Window_Upgrade ) end;
				};
			};
		}
	} )
	
	for k, v in pairs( myWorld:getUpgrades() ) do
		local button = {}
		
		button.type 		= "button";
		button.relPos		= v.pos;
		button.size			= { 50, 50 };
		button.locked		= v.locked;
		button.textures		= {
			"GUI_BUTTON_UPGRADE_BG";
			v.texture
		};
		button.buttonTex	= "GUI_BUTTON_UPGRADE";
		
		if v.locked ~= true then
			button.texts		= {
				{
					font	= "FONT_ARIAL";
					size	= 15;
					origin	= 2;
					pos		= { 47, 30 };
					string	= v.level .. "/" .. v.levelMax;
				};
			};
		end
		
		button.functions	= {
			onClick	= 	
				function( b )
					local ID = upgradeManager.getID( b )
					
					if myInventory:buyUpgrade( ID ) then
						local upgrade = myWorld:getUpgrade( ID )

						myGui:setText( b, {
							font	= "FONT_ARIAL";
							size	= 15;
							origin	= 2;
							pos		= { 47, 30 };
							string	= upgrade.level .. "/" .. upgrade.levelMax;
						}, 0 )
						
						if upgrade.level == upgrade.levelMax then
							for i, v in ipairs( upgrade.unlocks ) do
								local childButton 	= upgradeManager.getButton( v )
								local childUpgrade 	= myWorld:getUpgrade( v )
								
								if childButton ~= nil and childUpgrade ~= nil then
									myGui:setLocked( childButton, false )
									myGui:addText( childButton, {
										font	= "FONT_ARIAL";
										size	= 15;
										origin	= 2;
										pos		= { 47, 30 };
										string	= 0 .. "/" .. childUpgrade.levelMax;
									} )
								end
							end
						end
					end
				end; -- onClick
			onMouseenter = 
				function( b )
					local ID 		= upgradeManager.getID( b )
					local upgrade	= myWorld:getUpgrade( ID )
					
					local x, y, w, h = myGui:getBox( b )
					upgrade.mouseBox = { x, y, w, h }
					
					setupTooltip( "upgrade", upgrade )
				end; -- onMouseenter
			
		};
		
		local temp = myGui:newButton( Window_Upgrade, button )
		
		upgradeManager.IDs[ temp ] 		= v.ID
		upgradeManager.buttons[ v.ID ] 	= temp
	end	-- For loop
end

function initPlayer()
	local playerData = {}
	
		if charSelectionIndex == 1 then playerData = getCharacter( "PLAYER_BARBARIAN" );
	elseif charSelectionIndex == 2 then playerData = getCharacter( "PLAYER_KNIGHT" );
	elseif charSelectionIndex == 3 then playerData = getCharacter( "PLAYER_RANGER" );
	elseif charSelectionIndex == 4 then playerData = getCharacter( "PLAYER_ASSASSIN" );
	elseif charSelectionIndex == 5 then playerData = getCharacter( "PLAYER_SORCERER" );
	elseif charSelectionIndex == 6 then playerData = getCharacter( "PLAYER_NECROMANCER" );
	end

	playerData.pos = { 1000, 950 }
	
	playerID = myWorld:createPlayer( playerData )
	
	local playerLUD = myEntity:getLightUserData( playerID )
	
	myEntity:addAbility( playerLUD, getAbility( "DEV_BLINK" ) );
	myEntity:addAbility( playerLUD, getAbility( "FIREBALL" ) );
	myEntity:addAbility( playerLUD, getAbility( "POISON_BOMB" ) );
	
	abilitybarSetPlayer( playerID )
end

function initPlayscene()
	local cameraSize = { GameWindow_W, GameWindow_H }
	
	math.randomseed( os.time() )
	
	resetAbilitybar()
	
	initPlayer()
	initPausescreen()
	initAbilitybar()
	initCastbar()
	initInventory()
	initItemDestroyWindow()
	initMapWindow()
	setupUpgrades()
	initUpgradeWindow()
	
	local temp = getLootTable( "Equipment" )
	
	createLevel( "ZONE_START" )
	-- createLevel( "ZONE_1_LEVEL_1" )
	
	myWorld:setCameraSize( cameraSize )
	
	myInventory:addItem( getItem( "ITEM_AMULET_AENOANET" ) )
	myInventory:addItem( getItem( "ITEM_HELM_IRON" ) )
	myInventory:addItem( getItem( "ITEM_CHEST_IRON" ) )
	myInventory:addItem( getItem( "ITEM_PANTS_IRON" ) )
	myInventory:addItem( getItem( "ITEM_GLOVES_IRON" ) )
	myInventory:addItem( getItem( "ITEM_SHOES_IRON" ) )	
	myInventory:addItem( getItem( "ITEM_HELM_LEATHER" ) )
	myInventory:addItem( getItem( "ITEM_CHEST_LEATHER" ) )
	myInventory:addItem( getItem( "ITEM_PANTS_LEATHER" ) )
	myInventory:addItem( getItem( "ITEM_GLOVES_LEATHER" ) )
	myInventory:addItem( getItem( "ITEM_SHOES_LEATHER" ) )
	myInventory:addItem( getItem( "ITEM_SWORD_DODYBRIO" ) )
	myInventory:addRunestones( 2000 )
end

local windowManager = {}
windowManager.window	= nil
windowManager.posX 		= nil
windowManager.posY		= nil
windowManager.update	= function()
	if windowManager.window ~= nil then
		local distance = distanceBetweenVectors( windowManager.posX, windowManager.posY, myWorld:getPlayerCenter() )
		
		if distance > 250 then
			myGui:reverseShown( windowManager.window )
			
			windowManager.window = nil
		end
	end
end

function interactWithWindow( PosX, PosY, Window )
	if Window == nil then return end
	
	if windowManager.window == Window then
		myGui:reverseShown( Window )
		
		windowManager.window = nil
	else
		windowManager.window 	= Window
		windowManager.posX 		= PosX
		windowManager.posY 		= PosY
		
		myGui:reverseShown( Window )
	end
end

function closeWindow( Window )
	myGui:setShown( Window, false )
	
	if windowManager.window == Window then
		windowManager.window = nil
	end
end

function closeWindows()
	if windowManager.window ~= nil then
		myGui:reverseShown( windowManager.window )
		
		windowManager.window = nil;
		
		return true
	end
	
	return false
end

function worldUpdate()
	if myInput:isKeyPressed( 0 ) 	then myWorld:movePlayerLeft()		end
	if myInput:isKeyPressed( 3 ) 	then myWorld:movePlayerRight()		end
	if myInput:isKeyPressed( 22 ) 	then myWorld:movePlayerUp()			end
	if myInput:isKeyPressed( 18 ) 	then myWorld:movePlayerDown()		end

	windowManager.update()
	SpawnManager.update()
	
	if myInput:isMousePressed( 0 ) 	then
		if myGui:hasMousefocus() == false and myGui:isActive( windowManager.window ) == false then
			-- Projectile 
			local pl_X, pl_Y 	= myWorld:getPlayerCenter()
			local o_X, o_Y 		= myWorld:getCameraOffset( { pl_X, pl_Y } )
			local m_X, m_Y 		= myInput:getMousePos()

			local angleToTarget = math.atan2( m_Y - o_Y, m_X - o_X )

			myWorld:playerAttack( angleToTarget )
		end
	end
	
	local player_X, player_Y = myWorld:getPlayerCenter()
	
	myWorld:moveCameraCenterTo( { player_X, player_Y }, 15 )
end

function spawnProjectile( SpecificData )
	myWorld:createProjectile( getProjectile( "ARROW", SpecificData.x, SpecificData.y, SpecificData.angle, SpecificData.attDistance or 400, SpecificData ) )
end

-- Return mouseposition relative to game camera
function getRelativeMousePos()
	local m_X, m_Y 		= myInput:getMousePos()
	local o_X, o_Y 		= myWorld:getCameraPos()
	
	return o_X + m_X, o_Y + m_Y
end

register( "UpdateWorld",			worldUpdate ) 
register( "SpawnProjectile",		spawnProjectile )
register( "PlayerHealthChange", 	onPlayerHealthChange )
register( "PlayerResourceChange", 	onPlayerResourceChange )











