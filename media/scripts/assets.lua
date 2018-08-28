

local myAssets	= Assets
local myAudio	= Audio

function loadAssets() 
	loadTextures()
	loadFonts()
	loadSounds()
	loadMusic()
end

function loadTextures() 
	local textures = {
		{ ID = "GUI_WINDOW_MAINMENU", 					File = "window_mainmenu.png" };
		{ ID = "GUI_WINDOW_OPTIONS", 					File = "window_options.png" };
		{ ID = "GUI_WINDOW_CHARMENU", 					File = "window_charmenu.png" };
		{ ID = "GUI_WINDOW_PAUSESCREEN", 				File = "window_pause.png" };
		{ ID = "GUI_WINDOW_ABILITYBAR", 				File = "abilitybar_bg.png" };
		{ ID = "GUI_WINDOW_ABILITYBAR_FRONT", 			File = "abilitybar_front.png" };
		{ ID = "GUI_WINDOW_INVENTORY", 					File = "window_inventory.png" };
		{ ID = "GUI_WINDOW_CASTBAR_BORDER", 			File = "castbar_border.png" };
		{ ID = "GUI_WINDOW_CASTBAR_BG", 				File = "castbar_bg.png" };
		{ ID = "GUI_WINDOW_CASTBAR_PROG", 				File = "castbar_prog.png" };
		
		{ ID = "GUI_BUTTON_TEST", 						File = "button_test.png" };
		{ ID = "GUI_BUTTON_WINDOW_EXIT", 				File = "button_window_exit.png" };
		{ ID = "GUI_BUTTON_MAINMENU_PLAY", 				File = "button_main_play.png" };
		{ ID = "GUI_BUTTON_MAINMENU_OPTIONS", 			File = "button_main_options.png" };
		{ ID = "GUI_BUTTON_MAINMENU_EXIT", 				File = "button_main_exit.png" };
		{ ID = "GUI_BUTTON_OPTIONS_GAME", 				File = "button_options_game.png" };
		{ ID = "GUI_BUTTON_OPTIONS_VIDEO", 				File = "button_options_video.png" };
		{ ID = "GUI_BUTTON_OPTIONS_AUDIO", 				File = "button_options_audio.png" };
		{ ID = "GUI_BUTTON_OPTIONS_KEYS", 				File = "button_options_keys.png" };
		{ ID = "GUI_BUTTON_OPTIONS_ACCEPT", 			File = "button_options_accept.png" };
		{ ID = "GUI_BUTTON_OPTIONS_APPLY", 				File = "button_options_apply.png" };
		{ ID = "GUI_BUTTON_OPTIONS_CANCEL", 			File = "button_options_cancel.png" };
		{ ID = "GUI_BUTTON_CHARMENU_ARROWLEFT", 		File = "button_charselect_left.png" };
		{ ID = "GUI_BUTTON_CHARMENU_ARROWRIGHT", 		File = "button_charselect_right.png" };
		{ ID = "GUI_BUTTON_PAUSEMENU_RETURN", 			File = "button_pause_return.png" };
		{ ID = "GUI_BUTTON_PAUSEMENU_MENU", 			File = "button_pause_menu.png" };
		{ ID = "GUI_BUTTON_PAUSEMENU_EXIT", 			File = "button_pause_exit.png" };
		{ ID = "GUI_BUTTON_ABILITY", 					File = "button_ability.png" };
		{ ID = "GUI_BUTTON_UPGRADE_BG", 				File = "button_upgrade_bg.png" };
		{ ID = "GUI_BUTTON_UPGRADE", 					File = "button_upgrade.png" };
		
		{ ID = "GUI_CHECKBOX_TEST", 					File = "checkbox_test.png" };
		{ ID = "GUI_CHECKBOX_CHECK_TEST", 				File = "icon_checkbox_check.png" };
		
		{ ID = "GUI_SLIDER_OPTIONS", 						File = "slider_options.png" };
		{ ID = "GUI_SLIDER_BG_OPTIONS", 				File = "slider_options_bg.png" };
		
		{ ID = "GUI_ITEMSLOT_BG", 						File = "itemslot_bg.png" };
		{ ID = "GUI_ITEMSLOT_SHADOW_HELM", 				File = "itemslot_helm.png" };
		{ ID = "GUI_ITEMSLOT_SHADOW_AMULET", 			File = "itemslot_amulet.png" };
		{ ID = "GUI_ITEMSLOT_SHADOW_CHEST", 			File = "itemslot_chest.png" };
		{ ID = "GUI_ITEMSLOT_SHADOW_PANTS", 			File = "itemslot_pants.png" };
		{ ID = "GUI_ITEMSLOT_SHADOW_GLOVES", 			File = "itemslot_gloves.png" };
		{ ID = "GUI_ITEMSLOT_SHADOW_SHOES", 			File = "itemslot_shoes.png" };
		{ ID = "GUI_ITEMSLOT_SHADOW_MAINHAND", 			File = "itemslot_mainhand.png" };
		{ ID = "GUI_ITEMSLOT_SHADOW_OFFHAND", 			File = "itemslot_offhand.png" };
		{ ID = "GUI_ITEMSLOT_SHADOW_BAG", 				File = "itemslot_shadow_bag.png" };
		{ ID = "GUI_ITEMSLOT_BORDER", 					File = "itemslot_border.png" };
		{ ID = "GUI_ITEMSLOT_RARITY_COMMON", 			File = "itemslot_rarity_common.png" };
		{ ID = "GUI_ITEMSLOT_RARITY_RARE", 				File = "itemslot_rarity_rare.png" };
		{ ID = "GUI_ITEMSLOT_RARITY_EPIC", 				File = "itemslot_rarity_epic.png" };
		{ ID = "GUI_ITEMSLOT_RARITY_LEGENDARY", 		File = "itemslot_rarity_legendary.png" };
		
		{ ID = "GUI_TOOLTIP_BG", 						File = "tooltip_bg.png" };
		
		{ ID = "GUI_TRACK_FRONT", 						File = "tracker_front.png" };
		{ ID = "GUI_TRACK_HEALTH_BG", 					File = "tracker_hp_bg.png" };
		{ ID = "GUI_TRACK_HEALTH_PROG", 				File = "tracker_hp_prog.png" };
		{ ID = "GUI_TRACK_RESOURCE_RAGE_BG", 			File = "tracker_rp_bg_rage.png" };
		{ ID = "GUI_TRACK_RESOURCE_RAGE_PROG", 			File = "tracker_rp_prog_rage.png" };
		{ ID = "GUI_TRACK_RESOURCE_STAMINA_BG", 		File = "tracker_rp_bg_stamina.png" };
		{ ID = "GUI_TRACK_RESOURCE_STAMINA_PROG", 		File = "tracker_rp_prog_stamina.png" };
		{ ID = "GUI_TRACK_RESOURCE_FOCUS_BG", 			File = "tracker_rp_bg_focus.png" };
		{ ID = "GUI_TRACK_RESOURCE_FOCUS_PROG", 		File = "tracker_rp_prog_focus.png" };
		{ ID = "GUI_TRACK_RESOURCE_ENERGY_BG", 			File = "tracker_rp_bg_energy.png" };
		{ ID = "GUI_TRACK_RESOURCE_ENERGY_PROG", 		File = "tracker_rp_prog_energy.png" };
		{ ID = "GUI_TRACK_RESOURCE_MANA_BG", 			File = "tracker_rp_bg_mana.png" };
		{ ID = "GUI_TRACK_RESOURCE_MANA_PROG", 			File = "tracker_rp_prog_mana.png" };
		{ ID = "GUI_TRACK_RESOURCE_LIFEENERGY_BG", 		File = "tracker_rp_bg_lifeenergy.png" };
		{ ID = "GUI_TRACK_RESOURCE_LIFEENERGY_PROG", 	File = "tracker_rp_prog_lifeenergy.png" };
		
		-- Pause screen
		{ ID = "GUI_BG_PAUSESCREEN", 					File = "background_pausescreen.png" };
		
		-- Experience bar
		{ ID = "GUI_EXPBAR_BG", 						File = "expbar_bg.png" };
		{ ID = "GUI_EXPBAR_FRONT", 						File = "expbar_front.png" };
		
		-- Map
		{ ID = "GUI_MAP_BG", 							File = "map_bg.png" };
		{ ID = "GUI_BUTTON_GRASS", 						File = "button_map_grass.png" };
		
		-- Abilities
		{ ID = "GUI_ICON_ABILITY_BLINK", 				File = "icon_ability_blink.png" };
		{ ID = "GUI_ICON_ABILITY_FIREBALL", 			File = "icon_ability_fireball.png" };
		{ ID = "GUI_ICON_ABILITY_POISONBOMB", 			File = "icon_ability_poisonbomb.png" };
		{ ID = "GUI_ICON_ABILITY_FROSTSHIELD", 			File = "icon_ability_frostshield.png" };
		
		-- Upgrades
		{ ID = "GUI_ICON_UPG_HEALTH_MAX", 				File = "icon_upgrade_health_percent.png" };
		{ ID = "GUI_ICON_UPG_RESOURCE_MAX", 			File = "icon_upgrade_max_resource.png" };
		{ ID = "GUI_ICON_UPG_MOVESPD", 					File = "icon_upgrade_speed_attack.png" };
		{ ID = "GUI_ICON_UPG_HEALTH_REGEN", 			File = "icon_upgrade_stats_base.png" };
	}
	
	myAssets:loadTexture( "media/textures/gui/", textures )
	
	textures = {
		{ ID = "CHAR_PLAYER_BARBARIAN", 				File = "player_barbarian.png" };
		{ ID = "CHAR_PLAYER_KNIGHT", 					File = "player_knight.png" };
		{ ID = "CHAR_PLAYER_RANGER", 					File = "player_ranger.png" };
		{ ID = "CHAR_PLAYER_ASSASSIN", 					File = "player_assassin.png" };
		{ ID = "CHAR_PLAYER_SORCERER", 					File = "player_sorcerer.png" };
		{ ID = "CHAR_PLAYER_NECROMANCER", 				File = "player_necromancer.png" };
		
		{ ID = "CHAR_ENEMY_RANGER", 					File = "enemy_ranger.png" };
		{ ID = "CHAR_ENEMY_BOSS", 						File = "enemy_boss.png" };
	}
	
	myAssets:loadTexture( "media/textures/chars/", textures )
	
	textures = {
		{ ID = "LOOT_BAG", 								File = "loot_bag.png" };
		{ ID = "RUNESTONE", 							File = "runestone.png" };
		
		{ ID = "OBJECT_ROCK_GREY_ONE", 					File = "rock_grey_one.png" };
		{ ID = "OBJECT_ROCK_GREY_FLOAT_ONE", 			File = "rock_grey_float_one.png" };
		{ ID = "OBJECT_TREE_GREENLEAF", 				File = "tree_greenleaf.png" };
		
		{ ID = "OBJECT_TEST_ACTION", 					File = "testobject_action.png" };
		{ ID = "OBJECT_TEST_TOP", 						File = "testobject_top.png" };
		{ ID = "OBJECT_TEST_FINISH", 					File = "testobject_action_finish.png" };

		{ ID = "OBJECT_PORTAL_BLUE", 					File = "portal_blue.png" };
		{ ID = "OBJECT_PORTAL_RED", 					File = "portal_red.png" };
		{ ID = "OBJECT_PLATFORM_BOSS", 					File = "platform_level_boss.png" };
		{ ID = "OBJECT_PILLAR_BOSS", 					File = "pillar_level_boss.png" };
		{ ID = "OBJECT_ORB_SPAWNER_INIT", 				File = "orb_spawner_init.png" };
		{ ID = "OBJECT_ORB_SPAWNER", 					File = "orb_spawner.png" };
		{ ID = "OBJECT_STRUCTURE_GATE_PORTAL", 			File = "structure_gate_portal.png" };
		
		-- Starting zone 
		{ ID = "OBJECT_STRUCTURE_TEMPLE_CENTER", 		File = "structure_temple_center.png" };
		{ ID = "OBJECT_STRUCTURE_TEMPLE_UPGRADE", 		File = "structure_temple_upgrade.png" };
		{ ID = "OBJECT_CHARACTER_UPGRADE", 				File = "character_upgrade.png" };
		{ ID = "OBJECT_CRYSTAL_PURPLE_STARTZONE", 		File = "crystal_purple_startzone.png" };
	}
	
	myAssets:loadTexture( "media/textures/objects/", textures )
	
	textures = {
		{ ID = "PROJECTILE_ARROW", 						File = "arrow.png" };
		{ ID = "PROJECTILE_FIREBALL", 					File = "fireball.png" };
		{ ID = "PROJECTILE_BOMB_POISON", 				File = "bomb_poison.png" };
	}
	
	myAssets:loadTexture( "media/textures/projectiles/", textures )
	
	textures = {
		{ ID = "TILESET_TEST", 							File = "tileset_testing.png" };
		{ ID = "TILESET_START", 						File = "start.png" };
		{ ID = "TILESET_GRASS", 						File = "grass.png" };
		{ ID = "TILESET_WINTER", 						File = "tileset_winter.png" };
	}
	
	myAssets:loadTexture( "media/textures/tilesets/", textures )
	
	textures = {
		{ ID = "GUI_ITEM_MISSING", 						File = "missing.png" };
		{ ID = "GUI_ITEM_AMULET_AENOANET", 				File = "amulet_aenoanet.png" };
		-- IRON
		{ ID = "GUI_ITEM_HELM_IRON", 					File = "helm_iron.png" };
		{ ID = "GUI_ITEM_CHEST_IRON", 					File = "chest_iron.png" };
		{ ID = "GUI_ITEM_GLOVES_IRON", 					File = "gloves_iron.png" };
		{ ID = "GUI_ITEM_PANTS_IRON", 					File = "pants_iron.png" };
		{ ID = "GUI_ITEM_SHOES_IRON", 					File = "boots_iron.png" };
		-- LEATHER
		{ ID = "GUI_ITEM_HELM_LEATHER", 				File = "helm_leather.png" };
		{ ID = "GUI_ITEM_CHEST_LEATHER", 				File = "chest_leather.png" };
		{ ID = "GUI_ITEM_GLOVES_LEATHER", 				File = "gloves_leather.png" };
		{ ID = "GUI_ITEM_PANTS_LEATHER", 				File = "pants_leather.png" };
		{ ID = "GUI_ITEM_SHOES_LEATHER", 				File = "boots_leather.png" };
	}
	
	myAssets:loadTexture( "media/textures/items/", textures )
	
	textures = {
		{ ID = "PARTICLE_CROSS", 						File = "cross.png" };
		{ ID = "PARTICLE_START_PURPLE_CYSTAL", 			File = "circle_crystal_purple.png" };
		{ ID = "PARTICLE_EXPLOSION_POISON", 			File = "explosion_poison.png" };
		{ ID = "PARTICLE_SMOKE_POISON", 				File = "smoke_poison.png" };
	}
	
	myAssets:loadTexture( "media/textures/particles/", textures )
end

function loadFonts() 
	fonts = {
		{ ID = "FONT_ARIAL", 							File = "arial.ttf" };
	}
	
	myAssets:loadFont( "media/fonts/", fonts )
end

function loadSounds()
	sounds = {
		-- Gui
		{ ID = "BUTTON_BASE_CLICK", 					File = "button_base_click.ogg" };
		
		-- Projectiles
		{ ID = "PROJECTILE_BOMB_POISON_DESPAWN", 		File = "projectile_bomb_poison_despawn.ogg" };
		
		-- Objects
		-- { ID = "OBJECT_CRYSTAL_GLOW", 					File = "door.ogg" };
	}
	
	myAssets:loadSound( "media/sounds/", sounds )
end

function loadMusic()
	music	= {
		{ ID = "MAINMENU", 								File = "menu.ogg" };
	}
	
	myAudio:loadMusic( "media/music/", music )
end

register( "GameInit", loadAssets )








