
local myGui		= Gui

charSelectionIndex = 1

charTextureBarbarian 	= "CHAR_PLAYER_BARBARIAN"
charTextureKnight 		= "CHAR_PLAYER_KNIGHT"
charTextureRanger 		= "CHAR_PLAYER_RANGER"
charTextureAssassin 	= "CHAR_PLAYER_ASSASSIN"
charTextureSorcerer 	= "CHAR_PLAYER_SORCERER"
charTextureNecromancer	= "CHAR_PLAYER_NECROMANCER"

local charTex 	= {
	"CHAR_PLAYER_BARBARIAN";
	"CHAR_PLAYER_KNIGHT";
	"CHAR_PLAYER_RANGER";
	"CHAR_PLAYER_ASSASSIN";
	"CHAR_PLAYER_SORCERER";
	"CHAR_PLAYER_NECROMANCER";
}

local charName 	= {
	"Barbarian";
	"Knight";
	"Ranger";
	"Assassin";
	"Sorcerer";
	"Necromancer";
}

local sprite;
local text;

local charSprite_w, charSprite_h;

function initCharmenu()
	-- Reinitialize character selection
	charSelectionIndex	= 1
	
	charSprite_w, charSprite_h = myAssets:getTextureSize( "CHAR_PLAYER_BARBARIAN" );
	
	local charSpriteBox = { 
		( GameWindow_W - 80 ) / 2; 
		( GameWindow_H - 80 ) / 2; 
		80;
		140; 
	};
	
	local char_W	= 350
	local char_H	= 400

	--[[
	-- Window table
	local windowChar = {
		pos		= { ( GameWindow_W - char_W ) / 2, ( GameWindow_H - char_H ) / 2 };
		size	= { char_W, char_H };
		texture = "GUI_WINDOW_CHARMENU";
		widgets	= {
			{
				type	= "BUTTON";
				pos		= { 30, ( char_H - 30 ) / 2 };
				size	= { 40, 40 };
				texture = "GUI_BUTTON_CHARMENU_ARROWLEFT";
				msg		= "GuiButtonCharmenuPrev";
				hotkey	= { 0, 71 };
			};
			{
				type	= "BUTTON";
				pos		= { char_W - 60, ( char_H - 30 ) / 2 };
				size	= { 40, 40 };
				texture = "GUI_BUTTON_CHARMENU_ARROWRIGHT";
				msg		= "GuiButtonCharmenuNext";
				hotkey	= { 3, 72 };
			};
			{
				type	= "BUTTON";
				pos		= { ( char_W - 150 ) / 2, char_H - 40 };
				size	= { 150, 30 };
				texture = "GUI_BUTTON_MAINMENU_PLAY";
				msg		= "GuiButtonCharmenuPlay";
			};
		};
	}
	]]
	
	myGui:newElement( {
		pos		= { ( GameWindow_W - char_W ) / 2, ( GameWindow_H - char_H ) / 2 };
		size	= { char_W, char_H };
		texture = "GUI_WINDOW_CHARMENU";
		
		childs	= {
			{
				type	= "button";
				relPos		= { 30, ( char_H - 30 ) / 2 };
				size	= { 40, 40 };
				buttonTex	= "GUI_BUTTON_CHARMENU_ARROWLEFT";
				hotkeys	= { 0, 71 };
				
				functions = {
					onClick	= function( b ) 
						if charSelectionIndex == 1 then
							charSelectionIndex = 6
						else 
							charSelectionIndex = charSelectionIndex - 1
						end
						
						changeCharSprite()
					end;
				};
			};
			{
				type	= "button";
				relPos		= { char_W - 60, ( char_H - 30 ) / 2 };
				size	= { 40, 40 };
				buttonTex	= "GUI_BUTTON_CHARMENU_ARROWRIGHT";
				hotkeys	= { 3, 72 };
				
				functions = {
					onClick	= function( b ) 
						if charSelectionIndex == 6 then
							charSelectionIndex = 1
						else 
							charSelectionIndex = charSelectionIndex + 1
						end
						
						changeCharSprite()
					end;
				};
			};
			{
				type	= "button";
				relPos		= { ( char_W - 150 ) / 2, char_H - 40 };
				size	= { 150, 30 };
				buttonTex	= "GUI_BUTTON_MAINMENU_PLAY";
				msg		= "GuiButtonCharmenuPlay";
				hotkeys	= { 58 };
			};
		};
	} )
	
	sprite = myGui:newElement( {
		pos		= { ( GameWindow_W - 80 ) / 2, ( GameWindow_H - 80 ) / 2 };
		size	= { 80, 140 };
		clip	= {
			0; 
			( charSprite_h / 8 ) * 3; 
			charSprite_w / 4; 
			charSprite_h / 8;
		};
		texture	= charTextureBarbarian;
	} )
	
	text = myGui:newElement( {
		pos 	= { GameWindow_W / 2, GameWindow_H / 2 - 100 };
		text 	= {};
	} )
		
	changeCharSprite()
end	

function changeCharSprite()
	myGui:setTexture( sprite, { 
		texture	= charTex[ charSelectionIndex ];
		clip	= { 
			0; 
			( charSprite_h / 8 ) * 3; 
			charSprite_w / 4; 
			charSprite_h / 8;
		};
	}, 0 )

	myGui:setText( text, {
		string 	= charName[ charSelectionIndex ];
		font	= "FONT_ARIAL";
		size	= 17;
		style	= 1;
		color	= { 255, 255, 255 };
		origin	= 0;
	}, 0 )
end

