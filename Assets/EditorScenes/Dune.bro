Dune = 
{
	DeathPlane = 
	{
		Fbx = "C:/Users/Jofu/Desktop/SomeGame/Assets/FBXScenes/10x10_cube.fbx",
		RotationY = 0,
		Scale = 
		{
			3000,
			10,
			3000,
		},
		Translation = 
		{
			-497.17486572266,
			-1993.7124023438,
			154.22290039063,
		},
		Trigger = 
		{
			AABB = 
			{
				Center = 
				{
					0,
					0,
					0,
				},
				HalfX = 30000,
				HalfY = 100,
				HalfZ = 30000,
			},
			Callback = 
			{
				Trigger = 
				{
				},
			},
			CollidesWith = 
			{
				"PLAYER","ENEMY"
			},
			CollisionBits = 
			{
				"TRIGGER",
			},
			IsTrigger = true,
			OnEnter = 
			{
				"Kill",
			},
			OnExit = 
			{
			},
		},
		Type = "trigger",
	},
	Dune = 
	{
		Fbx = "C:\\Users\\Jofu\\Desktop\\SomeGame\\Assets\\Meshes\\Raw\\FlatLand.fbx",
		RotationY = 0,
		Scale = 
		{
			1,
			1,
			1,
		},
		Translation = 
		{
			-100,
			107.73229980469,
			100,
		},
		Type = "static",
	},
	Enemy1 = 
	{
		Character = 
		{
			CharacterFile = "GeoEnemies/Follower",
		},
		Fbx = "C:\\Users\\Jofu\\Desktop\\SomeGame\\Assets\\Meshes\\Raw\\Follower.fbx",
		RotationY = 0,
		Scale = 
		{
			1,
			1,
			1,
		},
		Translation = 
		{
			-1560.7578125,
			-162.49227905273,
			2853.5444335938,
		},
		Type = "character",
	},
	Enemy2 = 
	{
		Character = 
		{
			CharacterFile = "GeoEnemies/Follower",
		},
		Fbx = "C:\\Users\\Jofu\\Desktop\\SomeGame\\Assets\\Meshes\\Raw\\Follower.fbx",
		RotationY = 0,
		Scale = 
		{
			1,
			1,
			1,
		},
		Translation = 
		{
			3426.8188476563,
			-316.16247558594,
			-3643.9467773438,
		},
		Type = "character",
	},
	JumpTrigger1 = 
	{
		Fbx = "C:/Users/Jofu/Desktop/SomeGame/Assets/FBXScenes/10x10_cube.fbx",
		RotationY = 0,
		Scale = 
		{
			25,
			50,
			25,
		},
		Translation = 
		{
			-2669.2648925781,
			540.35833740234,
			109.6471862793,
		},
		Trigger = 
		{
			AABB = 
			{
				Center = 
				{
					0,
					0,
					0,
				},
				HalfX = 250,
				HalfY = 500,
				HalfZ = 250,
			},
			Callback = 
			{
				Trigger = 
				{
				},
			},
			CollidesWith = 
			{
				"PLAYER", "ENEMY",
			},
			CollisionBits = 
			{
				"TRIGGER",
			},
			IsTrigger = true,
			OnEnter = 
			{
				"JumpTrigger",
			},
			OnExit = 
			{
			},
		},
		Type = "trigger",
	},
	PlayerOne = 
	{
		Character = 
		{
			CharacterFile = "Avatars/Player/QuakePlayer",
		},
		Fbx = "C:\\Users\\Jofu\\Desktop\\SomeGame\\Assets\\Models\\Player.fbx",
		RotationY = 0,
		Scale = 
		{
			1,
			1,
			1,
		},
		Translation = 
		{
			-1406.6433105469,
			115.42883300781,
			0,
		},
		Type = "character",
	},
}



