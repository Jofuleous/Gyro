Test5 = 
{
	Flag1 = 
	{
		Character = 
		{
			CharacterFile = "Flag/TeamOneFlag",
		},
		Fbx = "../Assets/Models/Flag.fbx",
		RotationY = 0,
		Scale = 
		{
			1,
			1,
			1,
		},
		Translation = 
		{
			1500,
			50,
			1500,
		},
		Type = "character",
	},
	Flag2 = 
	{
		Character = 
		{
			CharacterFile = "Flag/TeamTwoFlag",
		},
		Fbx = "../Assets/Models/Flag.fbx",
		RotationY = 0,
		Scale = 
		{
			1,
			1,
			1,
		},
		Translation = 
		{
			-1500,
			50,
			-1500,
		},
		Type = "character",
	},
	Player1 = 
	{
		Character = 
		{
			CharacterFile = "Avatars/Player/PlayerOne",
		},
		Fbx = "../Assets/Models/Player.fbx",
		RotationY = 0,
		Scale = 
		{
			1,
			1,
			1,
		},
		Translation = 
		{
			0,
			-200,
			0,
		},
		Type = "character",
	},
	ScoreTeamOne = 
	{
		Fbx = "../Assets/FBXScenes/10x10_cube.fbx",
		RotationY = 0,
		Scale = 
		{
			10,
			12,
			10,
		},
		Translation = 
		{
			1500,
			170.07522583008,
			1500,
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
				HalfX = 100,
				HalfY = 120,
				HalfZ = 100,
			},
			Callback = 
			{
				Trigger = 
				{
				},
			},
			CollidesWith = 
			{
				"PLAYER",
			},
			CollisionBits = 
			{
				"TRIGGER",
			},
			IsTrigger = true,
			OnEnter = 
			{
				"ScoreTeam1",
			},
			OnExit = 
			{
			},
		},
		Type = "trigger",
	},
	ScoreTeamTwo = 
	{
		Fbx = "../Assets/FBXScenes/10x10_cube.fbx",
		RotationY = 0,
		Scale = 
		{
			10,
			12,
			10,
		},
		Translation = 
		{
			-1500,
			176.0905456543,
			-1500,
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
				HalfX = 100,
				HalfY = 120,
				HalfZ = 100,
			},
			Callback = 
			{
				Trigger = 
				{
				},
			},
			CollidesWith = 
			{
				"PLAYER",
			},
			CollisionBits = 
			{
				"TRIGGER",
			},
			IsTrigger = true,
			OnEnter = 
			{
				"ScoreTeam2",
			},
			OnExit = 
			{
			},
		},
		Type = "trigger",
	},
	Static = 
	{
		Fbx = "../Assets/FBXScenes/ctf_v12-MeshOnly-NoEmbed.fbx",
		RotationY = 0,
		Scale = 
		{
			1,
			1,
			1,
		},
		Translation = 
		{
			0,
			0,
			0,
		},
		Type = "static",
	},
}




