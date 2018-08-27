local myGui = Gui

function initMainmenu() 
	local window_W	= 300
	local window_H	= 400
	
	myGui:newElement( {
		pos		= { ( GameWindow_W - window_W ) / 2, ( GameWindow_H - window_H ) / 2 };
		size	= { window_W, window_H };

		textures = {
			"GUI_WINDOW_MAINMENU";
		};
		
		childs	= {
			{
				type		= "button";
				relPos 		= { ( window_W - 150 ) / 2, 50 };
				size		= { 150, 30 };
				buttonTex 	= "GUI_BUTTON_MAINMENU_PLAY";
				msg			= "GuiButtonMenuPlay";
				hotkeys		= { 27 };
			};
			{
				type		= "button";
				relPos 		= { ( window_W - 150 ) / 2, window_H - 60 };
				size		= { 150, 30 };
				buttonTex 	= "GUI_BUTTON_MAINMENU_EXIT";
				msg			= "GuiButtonMenuExit";
			};
		};
	} )
	
end

