local myGui = Gui

local buttonCount	= 3
local buttonManager = {}

buttonManager.button 	= {}
buttonManager.index		= 0

local function selectNext()
	if buttonManager.index ~= 0 then
		myGui:setSelected( buttonManager.button[ buttonManager.index ], false )
	end
	
	buttonManager.index = buttonManager.index + 1;
	
	if buttonManager.index > buttonCount then
		buttonManager.index = 1
	end
	
	myGui:setSelected( buttonManager.button[ buttonManager.index ], true )
end

local function selectPrev()
	if buttonManager.index ~= 0 then
		myGui:setSelected( buttonManager.button[ buttonManager.index ], false )
	end
	
	buttonManager.index = buttonManager.index - 1;
	
	if buttonManager.index < 1 then
		buttonManager.index = buttonCount
	end
	
	myGui:setSelected( buttonManager.button[ buttonManager.index ], true )
end

local function onKeyPress( key ) 
		if key == 73 then selectPrev()
	elseif key == 74 then selectNext()
	end
end

register( "InputKeyReleased",		onKeyPress )

function initMainmenu() 
	local window_W	= 250
	local window_H	= 350
	
	Window_Mainmenu = myGui:newElement( {
		pos		= { ( GameWindow_W - window_W ) / 2, ( GameWindow_H - window_H ) / 2 };
		size	= { window_W, window_H };

		texture = "GUI_WINDOW_MAINMENU";
	} )
	
	buttonManager.button[ 1 ] = myGui:newButton( Window_Mainmenu, {
		relPos 		= { ( window_W - 150 ) / 2, 80 };
		size		= { 150, 25 };
		buttonTex 	= "GUI_BUTTON_MAINMENU_PLAY";
		msg			= "GuiButtonMenuPlay";
		hotkeys		= { 27 };
		
		sounds		= {
			onClick		= "BUTTON_BASE_CLICK";
		};
	} )
	
	buttonManager.button[ 2 ] = myGui:newButton( Window_Mainmenu, {
		relPos 		= { ( window_W - 150 ) / 2, 110 };
		size		= { 150, 25 };
		buttonTex 	= "GUI_BUTTON_MAINMENU_OPTIONS";
		hotkeys		= { 28 };
		
		functions	= {
			onClick		= 
				function( b )
					myGui:setShown( Window_Mainmenu, false )
					openSettings()
				end
		};
		
		sounds		= {
			onClick		= "BUTTON_BASE_CLICK";
		};
	} )
	
	buttonManager.button[ 3 ] = myGui:newButton( Window_Mainmenu, {
		relPos 		= { ( window_W - 150 ) / 2, window_H - 80 };
		size		= { 150, 25 };
		buttonTex 	= "GUI_BUTTON_MAINMENU_EXIT";
		hotkeys		= { 36 };
		
		functions	= {
			onClick		= function( b ) myCore:closeGame() end
		};
		
		sounds		= {
			onClick		= "BUTTON_BASE_CLICK";
		};
	} )
	
	initOptions()
	
end

