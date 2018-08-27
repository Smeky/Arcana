
--[[

	Scene ID:
		0	- Main menu
		1	- Character selection menu
		2	- Play

]]

function initFirstScene()
	myCore:changeScene( 0 )
end

function changeSceneToCharmenu()
	myCore:changeScene( 1 )
end

function changeSceneToPlay()
	myCore:changeScene( 2 )
end

function initScene( sceneID )
	if sceneID == 0 then
		myGui:close()
		myWorld:close()
	
		initMainmenu()
	
	elseif sceneID == 1 then
		myGui:close()
		myWorld:close()
		
		initCharmenu()
	
	elseif sceneID == 2 then
		myGui:close()
		myWorld:close()
		
		initPlayscene()
	
	end
end

register( "GameInit",				initFirstScene )
register( "GuiButtonPauseMenu",		initFirstScene )
register( "NewScene",				initScene )
register( "GuiButtonMenuPlay",		changeSceneToCharmenu )
register( "GuiButtonCharmenuPlay",	changeSceneToPlay )




