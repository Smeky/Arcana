
local myEntity = EntitySystem

-- Forward declaration
local projectiles = {}

function getProjectile( ID, posX, posY, angle, distance, stats )
	local projectile = projectiles[ ID ]
	
	if projectile == nil then print( "Error: Wrong projectile ID! ( ID: " .. ID .. " )" ) return end
	
	projectile.pos		= { posX or 0, posY or 0 }
	projectile.angle	= angle or 0
	projectile.distance = distance or 0
	
	if stats ~= nil then
		projectile.stats = { 
			dmgMin		= stats.dmgMin or 0;
			dmgMax		= stats.dmgMax or 0;
			critChance	= stats.critChance or 0;
			critMultip	= stats.critMultip or 0;
			faction		= stats.faction or 0;
		}
	end
	
	return projectile
end

projectiles[ "ARROW" ] = {
	collrad		= { 0, 0, 10 / 2 };	-- X, Y, R
	data		= {
		size 		= { 10, 20 };
		speed		= 300;
		texture 	= "PROJECTILE_ARROW";
	};
}

projectiles[ "FIREBALL" ] = {
	collrad		= { 0, 0, 14 / 2 };	-- X, Y, R
	data		= {
		size 		= { 14, 14 };
		speed		= 300;
		texture 	= "PROJECTILE_FIREBALL";
	};
}

projectiles[ "BOMB_POISON" ] = {
	collrad		= { 0, 0, 12 / 2 };	-- X, Y, R
	data		= {
		size 		= { 12, 12 };
		speed		= 220;
		texture 	= "PROJECTILE_BOMB_POISON";
	};
	
	functions	= {
		onDespawn	= 
			function( ID )
				local self = myEntity:getLightUserData( ID )
				
				local nearby = myEntity:getNearbyChars( self, 80 )
				
				local faction = myEntity:getFaction( self )
				
				for _, v in pairs( nearby ) do 
					if faction ~= myEntity:getFaction( v ) then
						myEntity:addEffect( v, {  
							ID 			= "EFFECT_POISON";
							livetime	= 5;
							ticktime	= 0.5;
							color		= { 90, 120, 50, 255 };
							type		= 1;
							sub_type	= 0;
							
							functions 	= {
								onAction		= 
									function( parent ) 
										myEntity:dealDamage( parent, 2 );
									end;
							};
						} )
					end
				end
				
				local x, y = myEntity:getCenter( self )

				myWorld:createParticle( {
						
						angleType	= 0;
						
						particle = {
							size			= { 6, 6 };
							texture			= "PARTICLE_EXPLOSION_POISON";
							moveVel			= 100;
							moveVelRange	= 40;
							distance		= 60;
							distanceRange	= 10;
							scalingType		= 1;								
						}
				}, x, y, 20 )
				
				myWorld:createParticle( {
						
						angleType	= 0;
						
						particle = {
							size			= { 50, 50 };
							texture			= "PARTICLE_SMOKE_POISON";
							moveVel			= 30;
							moveVelRange	= 10;
							distance		= 35;
							distanceRange	= 10;
							-- scalingType		= 1;
							rotDirection	= 0;
							rotVel			= 100;
							rotVelRange		= 20;
							opacityType		= 1;
							opacityTime		= 0;
						}
				}, x, y, 3 )
			end
	};
}









































