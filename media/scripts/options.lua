

--[[
	Description:
		- Handles in-game options menu and all its functionality
]]


local myGui		= Gui
local myAudio	= Audio
local myCore	= Core

local windows	= {}
local buttons	= {}
local curPage	= 1

local checkboxes = {}

checkboxes.audio 	= {}
checkboxes.audio.enable		= {}

local sliders	= {}

sliders.audio	= {}

local settings 	= {}

function openSettings()
	myGui:setShown( Window_Options, true )
	
	myGui:setChecked( checkboxes.audio.enable.master, 	settings.audio.enable.master )
	myGui:setChecked( checkboxes.audio.enable.music, 	settings.audio.enable.music )
	myGui:setChecked( checkboxes.audio.enable.effect, 	settings.audio.enable.effect )
	myGui:setChecked( checkboxes.audio.enable.voice, 	settings.audio.enable.voice )
	myGui:setChecked( checkboxes.audio.enable.ambient, 	settings.audio.enable.ambient )
	myGui:setChecked( checkboxes.audio.enable.interface,settings.audio.enable.interface )
	
	myGui:setSliderValue( sliders.audio.master, 	settings.audio.volume.master )
	myGui:setSliderValue( sliders.audio.music, 		settings.audio.volume.music )
	myGui:setSliderValue( sliders.audio.effect, 	settings.audio.volume.effect )
	myGui:setSliderValue( sliders.audio.voice, 		settings.audio.volume.voice )
	myGui:setSliderValue( sliders.audio.ambient, 	settings.audio.volume.ambient )
	myGui:setSliderValue( sliders.audio.interface, 	settings.audio.volume.interface )
end

local function loadSettings()
	settings	= myCore:getSettings()
end

local function getSettings() 
	local resultSettings	= {}
	
	resultSettings.audio = {}
	resultSettings.audio.enable = {}
	resultSettings.audio.enable.master 		= myGui:isChecked( checkboxes.audio.enable.master )
	resultSettings.audio.enable.music	 	= myGui:isChecked( checkboxes.audio.enable.music )
	resultSettings.audio.enable.effect 		= myGui:isChecked( checkboxes.audio.enable.effect )
	resultSettings.audio.enable.voice 		= myGui:isChecked( checkboxes.audio.enable.voice )
	resultSettings.audio.enable.ambient 	= myGui:isChecked( checkboxes.audio.enable.ambient )
	resultSettings.audio.enable.interface 	= myGui:isChecked( checkboxes.audio.enable.interface )
	
	resultSettings.audio.volume = {}
	resultSettings.audio.volume.master		= myGui:getSliderValue( sliders.audio.master )
	resultSettings.audio.volume.music		= myGui:getSliderValue( sliders.audio.music )
	resultSettings.audio.volume.effect		= myGui:getSliderValue( sliders.audio.effect )
	resultSettings.audio.volume.voice		= myGui:getSliderValue( sliders.audio.voice )
	resultSettings.audio.volume.ambient		= myGui:getSliderValue( sliders.audio.ambient )
	resultSettings.audio.volume.interface	= myGui:getSliderValue( sliders.audio.interface )
	
	return resultSettings
end

local function closeSettings() 
	myGui:setShown( Window_Options, false );

	local scene = myCore:getSceneID()
	
		if scene == 0 then myGui:setShown( Window_Mainmenu, true )
	elseif scene == 2 then myGui:setShown( Window_Pause, true )
	end
end

local function acceptSettings()
	settings = getSettings()
	
	myCore:updateSettings( settings )
	
	closeSettings()
end

local function applySettings()
	myCore:updateSettings( getSettings() )
end

local function cancelSettings()
	myCore:updateSettings( settings )
	
	closeSettings()
end

local function changePage( page )
	myGui:setShown( windows[ curPage ], false )
	myGui:setSelected( buttons[ curPage], false )
	
	myGui:setShown( windows[ page ], true )
	myGui:setSelected( buttons[ page], true )
	
	curPage = page
end

function initOptions()
	loadSettings()

	local window_W	= 700
	local window_H	= 550
	
	local window_X	= ( GameWindow_W - window_W ) / 2
	local window_Y  = ( GameWindow_H - window_H ) / 2
	
	Window_Options = myGui:newElement( {
		pos		= { window_X, window_Y };
		size	= { window_W, window_H };

		texture = "GUI_WINDOW_OPTIONS";
		
		shown	= false;
		
		childs	= {
			{
				type		= "button";
				relPos 		= { 150, window_H - 30 };
				size		= { 130, 30 };
				buttonTex 	= "GUI_BUTTON_OPTIONS_ACCEPT";
				hotkeys		= { 58 };
				
				functions	= {
					onClick		= function( b ) 
						acceptSettings()
					end;
				};
				
				sounds		= {
					onClick		= "BUTTON_BASE_CLICK";
				};
			};
			{
				type		= "button";
				relPos 		= { 285, window_H - 30 };
				size		= { 130, 30 };
				buttonTex 	= "GUI_BUTTON_OPTIONS_APPLY";
				
				functions	= {
					onClick		= function( b )
						applySettings()
					end;
				};
				
				sounds		= {
					onClick		= "BUTTON_BASE_CLICK";
				};
			};
			{
				type		= "button";
				relPos 		= { 420, window_H - 30 };
				size		= { 130, 30 };
				buttonTex 	= "GUI_BUTTON_OPTIONS_CANCEL";
				hotkeys		= { 36 };
				
				functions	= {
					onClick		= function( b )
						cancelSettings()
					end;
				};
				
				sounds		= {
					onClick		= "BUTTON_BASE_CLICK";
				};
			};
		};
	} )
	
	buttons[ 1 ]	= myGui:newButton( Window_Options, {
		relPos 		= { 190, 0 };
		size		= { 80, 60 };
		buttonTex 	= "GUI_BUTTON_OPTIONS_GAME";
		
		selected	= true;
		
		functions	= { 
			onClick		= function( b ) changePage( 1 ) end;
		};
	} )
	
	buttons[ 2 ]	= myGui:newButton( Window_Options, {
		relPos 		= { 270, 0 };
		size		= { 80, 60 };
		buttonTex 	= "GUI_BUTTON_OPTIONS_VIDEO";
		
		functions	= { 
			onClick		= function( b ) changePage( 2 ) end;
		};
	} )
	
	buttons[ 3 ]	= myGui:newButton( Window_Options, {
		relPos 		= { 350, 0 };
		size		= { 80, 60 };
		buttonTex 	= "GUI_BUTTON_OPTIONS_AUDIO";
		
		functions	= { 
			onClick		= function( b ) changePage( 3 ) end;
		};
	} )
	
	buttons[ 4 ]	= myGui:newButton( Window_Options, {
		relPos 		= { 430, 0 };
		size		= { 80, 60 };
		buttonTex 	= "GUI_BUTTON_OPTIONS_KEYS";
		
		functions	= { 
			onClick		= function( b ) changePage( 4 ) end;
		};
	} )
	
	windows[ 1 ]	= myGui:newElement( Window_Options, {
		pos		= { ( GameWindow_W - window_W ) / 2, ( GameWindow_H - window_H ) / 2 };
		size	= { window_W, window_H };
		
		shown	= true;
	} )
	
	windows[ 2 ]	= myGui:newElement( Window_Options, {
		pos		= { ( GameWindow_W - window_W ) / 2, ( GameWindow_H - window_H ) / 2 };
		size	= { window_W, window_H };
		
		shown	= false;
	} )
	
	windows[ 3 ]	= myGui:newElement( Window_Options, {
		pos		= { ( GameWindow_W - window_W ) / 2, ( GameWindow_H - window_H ) / 2 };
		size	= { window_W, window_H };
		
		shown	= false;
	} )
	
	windows[ 4 ]	= myGui:newElement( Window_Options, {
		pos		= { ( GameWindow_W - window_W ) / 2, ( GameWindow_H - window_H ) / 2 };
		size	= { window_W, window_H };
		
		shown	= false;
	} )
	
	checkboxes.audio.enable.master = myGui:newCheckbox( windows[ 3 ], {
		type		= "checkbox";
		relPos 		= { 280, 90 };
		size		= { 20, 20 };
		
		checkboxTex	= "GUI_CHECKBOX_TEST";
		checkTex	= "GUI_CHECKBOX_CHECK_TEST";
		
		checked		= settings.audio.enable.master;
		
		text		= {
			font	= "FONT_ARIAL";
			size	= 15;
			origin	= 1;
			pos		= { 35, 1 };
			string	= "Master";
		};
		
		sounds		= {
			onClick		= "BUTTON_BASE_CLICK";
		};
	} )
	
	
	checkboxes.audio.enable.music = myGui:newCheckbox( windows[ 3 ], {
		type		= "checkbox";
		relPos 		= { 120, 180 };
		size		= { 20, 20 };
		
		checkboxTex	= "GUI_CHECKBOX_TEST";
		checkTex	= "GUI_CHECKBOX_CHECK_TEST";
		
		checked		= settings.audio.enable.music;
		
		text		= {
			font	= "FONT_ARIAL";
			size	= 15;
			origin	= 1;
			pos		= { 35, 1 };
			string	= "Music";
		};
		
		sounds		= {
			onClick		= "BUTTON_BASE_CLICK";
		};
	} )
	
	checkboxes.audio.enable.effect = myGui:newCheckbox( windows[ 3 ], {
		type		= "checkbox";
		relPos 		= { 120, 250 };
		size		= { 20, 20 };
		
		checkboxTex	= "GUI_CHECKBOX_TEST";
		checkTex	= "GUI_CHECKBOX_CHECK_TEST";
		
		checked		= settings.audio.enable.effect;
		
		text		= {
			font	= "FONT_ARIAL";
			size	= 15;
			origin	= 1;
			pos		= { 35, 1 };
			string	= "Sound Effects";
		};
		
		sounds		= {
			onClick		= "BUTTON_BASE_CLICK";
		};
	} )
	
	checkboxes.audio.enable.voice = myGui:newCheckbox( windows[ 3 ], {
		type		= "checkbox";
		relPos 		= { 400, 180 };
		size		= { 20, 20 };
		
		checkboxTex	= "GUI_CHECKBOX_TEST";
		checkTex	= "GUI_CHECKBOX_CHECK_TEST";
		
		checked		= settings.audio.enable.voice;
		
		text		= {
			font	= "FONT_ARIAL";
			size	= 15;
			origin	= 1;
			pos		= { 35, 1 };
			string	= "Voice";
		};
		
		sounds		= {
			onClick		= "BUTTON_BASE_CLICK";
		};
	} )
	
	checkboxes.audio.enable.ambient = myGui:newCheckbox( windows[ 3 ], {
		type		= "checkbox";
		relPos 		= { 120, 320 };
		size		= { 20, 20 };
		
		checkboxTex	= "GUI_CHECKBOX_TEST";
		checkTex	= "GUI_CHECKBOX_CHECK_TEST";
		
		checked		= settings.audio.enable.ambient;
		
		text		= {
			font	= "FONT_ARIAL";
			size	= 15;
			origin	= 1;
			pos		= { 35, 1 };
			string	= "Ambient";
		};
		
		sounds		= {
			onClick		= "BUTTON_BASE_CLICK";
		};
	} )
	
	checkboxes.audio.enable.interface = myGui:newCheckbox( windows[ 3 ], {
		type		= "checkbox";
		relPos 		= { 400, 250 };
		size		= { 20, 20 };
		
		checkboxTex	= "GUI_CHECKBOX_TEST";
		checkTex	= "GUI_CHECKBOX_CHECK_TEST";
		
		checked		= settings.audio.enable.interface;
		
		text		= {
			font	= "FONT_ARIAL";
			size	= 15;
			origin	= 1;
			pos		= { 35, 1 };
			string	= "Interface";
		};
		
		sounds		= {
			onClick		= "BUTTON_BASE_CLICK";
		};
	} )
	
	
	sliders.audio.master = myGui:newSlider( windows[ 3 ], {
		relPos 		= { 240, 125 };
		size		= { 150, 4 };
		texture		= "GUI_SLIDER_BG_OPTIONS";
		sliderTex	= "GUI_SLIDER_OPTIONS";
		sliderSize	= { 12, 20 };
		
		value		= settings.audio.volume.master;
	
		functions	= {
			onMove	= function( b ) myAudio:setVolume( myGui:getSliderValue( b ) * 100, 0 ) end;
		};
	} )
	
	sliders.audio.music = myGui:newSlider( windows[ 3 ], {
		relPos 		= { 120, 215 };
		size		= { 150, 4 };
		texture		= "GUI_SLIDER_BG_OPTIONS";
		sliderTex	= "GUI_SLIDER_OPTIONS";
		sliderSize	= { 12, 20 };
		
		value		= settings.audio.volume.music;
	
		functions	= {
			onMove	= function( b ) myAudio:setVolume( myGui:getSliderValue( b ) * 100, 1 ) end;
		};
	} )
	
	sliders.audio.effect = myGui:newSlider( windows[ 3 ], {
		relPos 		= { 120, 285 };
		size		= { 150, 4 };
		texture		= "GUI_SLIDER_BG_OPTIONS";
		sliderTex	= "GUI_SLIDER_OPTIONS";
		sliderSize	= { 12, 20 };
		
		value		= settings.audio.volume.effect;
	
		functions	= {
			onMove	= function( b ) myAudio:setVolume( myGui:getSliderValue( b ) * 100, 2 ) end;
		};
	} )
	
	sliders.audio.voice = myGui:newSlider( windows[ 3 ], {
		relPos 		= { 400, 215 };
		size		= { 150, 4 };
		texture		= "GUI_SLIDER_BG_OPTIONS";
		sliderTex	= "GUI_SLIDER_OPTIONS";
		sliderSize	= { 12, 20 };
		
		value		= settings.audio.volume.voice;
	
		functions	= {
			onMove	= function( b ) myAudio:setVolume( myGui:getSliderValue( b ) * 100, 3 ) end;
		};
	} )
	
	sliders.audio.ambient = myGui:newSlider( windows[ 3 ], {
		relPos 		= { 120, 355 };
		size		= { 150, 4 };
		texture		= "GUI_SLIDER_BG_OPTIONS";
		sliderTex	= "GUI_SLIDER_OPTIONS";
		sliderSize	= { 12, 20 };
		
		value		= settings.audio.volume.ambient;
	
		functions	= {
			onMove	= function( b ) myAudio:setVolume( myGui:getSliderValue( b ) * 100, 4 ) end;
		};
	} )
	
	sliders.audio.interface = myGui:newSlider( windows[ 3 ], {
		relPos 		= { 400, 285 };
		size		= { 150, 4 };
		texture		= "GUI_SLIDER_BG_OPTIONS";
		sliderTex	= "GUI_SLIDER_OPTIONS";
		sliderSize	= { 12, 20 };
		
		value		= settings.audio.volume.interface;
	
		functions	= {
			onMove	= function( b ) myAudio:setVolume( myGui:getSliderValue( b ) * 100, 5 ) end;
		};
	} )
end