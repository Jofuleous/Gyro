--[[
	This file defines all of the assets for the renderer and how they're built

	There is a single list of assets, but within that list assets are grouped according to their type.
	The string used for each asset type determines the folder that those assets will be in;
	for example, using the string "Textures" means that the asset build process will:
		1) Look in Assets/Textures/ for the source texture files
		2) Build the target texture files into data/Textures/

	Within each asset type, the asset build process will look for the following:
		"tool" -
			This should be the name of the executable tool that is used to build the asset type.
			For example, "TextureBuilder" means that tools/TextureBuilder.exe will be used
		"extensions" -
			This defines a "source" and a "target" file extension.
			The source extension tells the asset build process what file type to look for in Assets/Textures,
			and the target extension tells what file type to create in data/Textures/
			(these are often different, but can be the same if the tool just copies files)
		"names" -
			This is a list of the actual assets that should be built.
			Each name should be the file name of the asset (without the directory or the file extension).
			For example, if there were a texture named "example1":
				* The source asset would be Assets/Textures/example1.png
				* The target asset would be data/Textures/example1.png
		"shouldAssetsAlwaysBeBuilt" -
			This is an optional boolean value, and if not found will default to false.
			Usually assets should only be built if they need to be
			(e.g. the built data file doesn't exist or the source asset has changed);
			when you are changing code for an asset-building executable, however,
			you will want to change this default behavior so that assets always get built
			regardless of dependencies.
]]--

-- The following does the same thing that is documented above,
-- but it applies to _all_ asset types.
-- In other words, if it is set to true then _every_ asset will be built
-- _every_ time the solution is built.
-- (you should never change this except as a sanity check)
shouldAssetsAlwaysBeBuilt = false

-- The following is the list of assets to be built
assets =
{
	VertexShaders =
	{
		tool = "VertexShaderBuilder",
		extensions =
		{
			source = "hlsl",
			target = "vp",
		},
		names =
		{
			"basicvertex",
			"depthvertex",
			"postprocess",
			"uivertexshader",
			"debugLinesVS",
			"hudVS",
		},
		shouldAssetsAlwaysBeBuilt = false,
	},
	FragmentShaders =
	{
		tool = "FragmentShaderBuilder",
		extensions =
		{
			source = "hlsl",
			target = "fp",
		},
		names =
		{
			"depthshade",
			"diffuse",
			"diffuseBasic",
			"specular",
			"diffuseNormalMap",
			"specularNormalMap",
			"specularUVScroll",
			"specularEnvironment",
			"specularEnvironmentNoNormal",
			"specularUVSCrollRefraction",
			"distortion",
			"uipixelshader",
			"toonShader",
			"alphablend",
			"debugLinesPS",
			"hudBoxPS",
		},
		shouldAssetsAlwaysBeBuilt = false,
	},
	Textures =
	{
		tool = "TextureBuilder",
		extensions =
		{
			source = "png",
			target = "png",
		},
		names =
		{
			"marble",
			"bark",
			"blueice",
			"companioncube",
			"ice",
			"obnoxious",
			"sand",
			"steel",
			"wood",
			"example1",
			"grass",
			"mossystonepath",
			"mossystonepathnormalmap",
			"PurplePink",
			"flame",
			"badgradesnoxbox",
			"raisedsunken",
			"raisedsunkennormalmap",
			"water",
			"waternormalmap",
			"Reticle",
			"outUV4",
			"entrance",
			"treetop",
			"ground",
			"brown",
			"observatory2",
			"steelgrey",
			"wall",
			"spiralground",
			"turret",
			"door",
			"skymask",
			"cloud",
			"wall_D",
			"railing_D",
			"metal_brace_D",
			"floor_D",
			"cement_wall_D",
			"cement_wall_NRM",
			"floor_NRM",
			"wall_NRM",
			"metal_brace_NRM",
			"railing_NRM",
			"trigger_orange",
			"rgb",
			"follower",
			"cyan",
			"red",
			"blue",
		},
		shouldAssetsAlwaysBeBuilt = false,
	},
	-- This is not a real asset;
	-- it is just an example of using Lua for a file format if you are interested or curious...
	-- Feel free to delete it or comment it out
	--[[LuaFileFormatExamples =
	{
		tool = "ExampleLuaBuilder",
		extensions =
		{
			source = "lua",
			target = "lua",
		},
		names =
		{
			"example",
		},
		shouldAssetsAlwaysBeBuilt = false,
	},]]--

	EnvironmentMaps =
	{
		tool = "EnvironmentMapBuilder",
		extensions =
		{
			source = "dds",
			target = "dds",
		},
		names =
		{
			"skybox02",
		},
		shouldAssetsAlwaysBeBuilt = false,
	},
	Entities =
	{
		tool = "EntityBuilder",
		extensions =
		{
			source = "entity",
			target = "entityb",
		},
		names =
		{
			"KarateCube",
			"Floor",
			"BarkCube",
			"CompanionCube",
			"SandCube",
			"WoodCube",
			"BlueIceCube",
			"IceCube",
			"ObnoxiousCube",
			"SteelCube",
			"SteelSphere",
			"IceSphere",
			"Babe",
			"ShineyWoodTorus",
			"SandTorus",
			"StonePlatform",
			"TerrainOne",
			"TerrainTwo",
			"OrbBase",
			"PowerOrb",
			"FlameQuad",
			"UnderAchiever",
			"RaisedSunken",
			"Water",
			"EnviroSphere",
			"EnviroPlane",
			"PostDistortionEntity",
			"ReticleEntity",
			"Tree",
			"leaf0",
			"leaf1",
			"leaf2",
			"leaf3",
			"leaf4",
			"leaf5",
			"trunk0",
			"Entrance",
			"TreeTop",
			"Ground",
			"SomeRock",
			"spiralground",
			"observatory",
			"Turret1",
			"Wall1",
			"Wall2",
			"Wall3",
			"Wall4",
			"Telescope",
			"Door",
			"SkyMask",
			"Cloud1",
			"Cloud2",
			"Cloud3",
			"Cloud4",
			"Guy",
			"FlagRed",
			"FlagBlue",
			"Follower",
			"Bullet",
			"Hopper",
			"Rectangle",
			"OrangePyramid",
			"Axis",
		},
		shouldAssetsAlwaysBeBuilt = false,
	},	
	Meshes =
	{
		tool = "MeshBuilder",
		extensions =
		{
			source = "mod",
			target = "bmod",
		},
		names =
		{
			"wreckt",
			"Floor",
			"sphere",
			"torus",
			"babe",
			"terrain1",
			"terrain2",
			"platform",
			"Base",
			"UnitQuad",
			"water",
			"PostQuad",
			"leaf0",
			"leaf1",
			"leaf2",
			"leaf3",
			"leaf4",
			"leaf5",
			"trunk0",
			"entrance",
			"ground",
			"treetop",
			"somerock",
			"spiralground",
			"observatory",
			"turret1",
			"wall1",
			"wall2",
			"wall3",
			"wall4",
			"telescope",
			"door",
			"skymask",
			"cloud1",
			"cloud2",
			"cloud3",
			"cloud4",
			"guy",
			"Flag",
			"Follower",
			"projectile",
			"bigSphere",
			"rectangle",
			"pyramid",
			"Axis",
			"Dude",
		},
		shouldAssetsAlwaysBeBuilt = false,
	},
	Materials =
	{
		tool = "MaterialBuilder",
		extensions =
		{
			source = "mat",
			target = "matb",
		},
		names =
		{
			"DiffuseBark",
			"DiffuseCompanion",
			"DiffuseSand",
			"DiffuseWood",
			"DistortionPostProcess",
			"SpecBlueIce",
			"SpecIce",
			"SpecObnoxious",
			"SpecSteel",
			"ShineyFloor",
			"ShineyWood",
			"KarateMaterial",
			"MossyStone",
			"GreenGrass",
			"AlphaOrbPurple",
			"AdditiveFlame",
			"AlphaTestTest",
			"RaisedSunkenNormal",
			"SpecularWater",
			"SpecEnvSphere",
			"SpecEnviroStone",
			"EnviroMirror",
			"SpecularWaterRefaction",
			"Reticle",
			"ToonTree",
			"Entrance",
			"GrassMat",
			"TreeTop",
			"SomeRock",
			"ObservatoryMat",
			"TelescopeMat",
			"WallMat",
			"SpiralGround",
			"TurretMat",
			"DoorMat",
			"SkyMaskMat",
			"CloudMat",
			"metal_m",
			"railing_m",
			"floor_m",
			"ceiling_m",
			"cement_m",
			"lambert1",
			"lambert2",
			"walls_m",
			"GuyMaterial",
			"TriggerOrange",
			"RGB",
			"Follower",
			"DiffusePurple",
			"CyanTransparent",
			"Red",
			"RedFull",
			"Orange",
			"FlagBlue",
		},
		shouldAssetsAlwaysBeBuilt = false,
	},
	Effects =
	{
		tool = "EffectsBuilder",
		extensions =
		{
			source = "efx",
			target = "efxb",
		},
		names =
		{
			"Diffuse",
			"BasicDiffuse",
			"Specular",
			"AlphaAdditive",
			"AlphaPartial",
			"AlphaTest",
			"DiffuseNormal",
			"SpecularNormal",
			"SpecularUVScroll",
			"SpecularEnvironmentMap",
			"SpecularEnvironmentNoNormal",
			"SpecularUVScrollRefraction",
			"DistortionPostProcess",
			"HUDEffect",
			"toonEffect",
			"AlphaBlendBasic",
		},
		shouldAssetsAlwaysBeBuilt = false,
	},	

	Characters =
	{
		tool = "CharacterBuilder",
		extensions =
		{
			source = "car",
			target = "carx",
		},
		names =
		{
			"Avatars/Player/PlayerOne",
			"Avatars/Player/QuakePlayer",
			"Flag/TeamOneFlag",
			"Flag/TeamTwoFlag",
			"GeoEnemies/Follower",
			"Projectiles/BasicBullet",
			"GeoEnemies/HopperEnemy",
			"GeoEnemies/SpinnerEnemy",
			"GeoEnemies/EnemySpawner",
			"Enemies/CtfAiPlayer",
			"Prototype/QuaternTest",
		},
		shouldAssetsAlwaysBeBuilt = true,		
	},
	Audio =
	{
		tool = "AudioBuilder",
		extensions =
		{
			source = "mp3",
			target = "mp3",
		},
		names =
		{
			"Music/allidoiswin",
			"Dialog/flag_captured",
			"Dialog/flag_returned",
			"SFX/airhorn",
			"SFX/walkingonground",
			"SFX/walkingonmetal",
			"Dialog/heavy-breathing-off-mic-loop",
		},
		shouldAssetsAlwaysBeBuilt = false,		
	},
}
