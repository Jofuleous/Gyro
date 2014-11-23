#ifndef __SCENE_H_
#define __SCENE_H_

#include <vector>
#include "ResourceTypes/PointLight.h"
#include "ResourceTypes/Entity.h"
#include "ResourceTypes/EnviroMap.h"
#include "ResourceTypes/Mesh.h"


class Camera;

class Scene
{
public:
										Scene();
										~Scene();

	bool								Load( const char* i_filename );
	bool								LoadBinaryScene( const char* i_filename );
	bool								LoadBlankScene( );
	void								Update( float i_dt );
	void								Draw( float i_dt );
	void								Unload( );
	static Camera*						GetCamera();
	void								GetLightViewMatrix( D3DXMATRIX& o_viewMatrix );
	void								GetLightViewToProjection( D3DXMATRIX& o_projection );
	
	void								MoveSelectedEntity( float i_dt );

	//saying that we only have one scene camera for simplicity sake.
	static Camera*						m_camera;

	//bit hacky, but lets allow selection of a mesh for now.  maybe triangle picking later.
	const char*							m_sceneName;
	PointLight*							m_pointLight;
	D3DXVECTOR4							m_ambientLight;
	ResourcePtr<EnviroMapReference>		m_enviroMap;
	static bool							m_postProcessing;
	static bool							m_shadowMap;
	D3DXVECTOR3							m_directionalLightDir;
	D3DXVECTOR3							m_dirLightColor;
	float								m_dirLightIntensity;
	D3DXVECTOR3							m_dirEyePos;
	std::vector<Entity*>				m_entities;


	//let's get a 25%...don't want to put in that much effort.
	static Entity*						m_postProcessEntity;


	// for fbx scene loading


	class SceneMeshLoader : public MeshLoader
	{
	public:
		virtual MeshReference* Load( const char* i_entityName );
	};

	bool								m_loadingScene;
	FILE*								m_currentSceneFile;		

};

#endif