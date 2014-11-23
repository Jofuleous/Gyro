#ifndef __MATERIAL_H_
#define __MATERIAL_H_

#include "../ResourcePtr.h"
#include "../ResourceManager.h"
#include "Effect.h"
#include "Texture.h"
#include <vector>
#include <map>

class Entity;

class MaterialData
{
public:
	~MaterialData()
	{
		for( unsigned i = 0; i < m_floatNames.size(); i++ )
			free( m_floatNames[i] );

		for( unsigned q = 0; q < m_textureNames.size(); q++ )
			free( m_textureNames[q] );

		m_textures.clear();
		m_floats.clear();
	}

	ResourcePtr<EffectReference>				m_effect;
	std::vector<char*>							m_textureNames;
	std::vector<ResourcePtr<TextureReference>>	m_textures;
	std::vector<float>							m_floats;
	std::vector<char*>							m_floatNames;
	bool										m_requiresCamera;
	bool										m_environmentMap;
	unsigned __int16							m_materialId; // todo: turn back to u16
	float										m_alpha;
};

class MaterialReference : public ResourceMaster<MaterialData>
{
public:
	~MaterialReference()
	{
	}
};

class MaterialLoader : public ResourceLoader<MaterialReference>
{
public:
	virtual MaterialReference* Load( const char* i_filename );
};

#endif