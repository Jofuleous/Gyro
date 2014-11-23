#ifndef __GAUDIOMANAGER_H_
#define __GAUDIOMANAGER_H_

#include "..\..\External\FMOD\Windows\api\inc\fmod.hpp"
#include "..\..\External\FMOD\Windows\api\inc\fmod_errors.h"
#include <iostream>
#include <Windows.h>
#include "Utility/Singleton.h"
#include "LuaPlus/LuaObject.h"
#include "Math/GVector3.h"
#include <vector>
#include "Utility/GHashedString.h"

// keeping this really hacky so we can get the grade.  Ogre has a good sound manager:
// http://www.ogre3d.org/tikiwiki/tiki-index.php?page=FMOD+SoundManager
// then there's another Ogre one called like "another ogre sound manager"

#define MAX_SOUND_CHANNELS       200

namespace GAudio
{

class GAudioManager
{
public:

	friend class Singleton<GAudioManager>;

	GAudioManager( )
	{

	}

	bool					LoadWorldAudio( LuaPlus::LuaObject& i_obj );
	bool					Initialize( );
	bool					Shutdown( ) {}
	bool					Cleanse( ) {} 
	bool					PlayMusic( const char* i_filePath );
	FMOD::Channel*			PlayEffect( const char* i_effectPath, bool i_loop, bool i_3d, FMOD::Sound** o_sound = NULL, FMOD::Channel* o_channel = NULL );
	void					UpdateChannelPosition( FMOD::Channel* i_channel, const GVector3& i_pos );
	void					UpdateListenerPosition( const GVector3& i_listenerPos, const GVector3& i_forward, const GVector3& i_up ); 
	void					Update( );
	void					SetMusicVolume( float i_volume );
	void					SetEffectVolume( float i_volume );

// really bad.

	float				m_musicVolume;
	float				m_sfxVolume;
	FMOD::Channel*		m_musicChannel;
	FMOD::System*		m_fmodSystem;
	FMOD::ChannelGroup*	m_musicChannelGroup;
	FMOD::ChannelGroup*	m_effectChannelGroup;
};

}

#define g_AudioManager Singleton<GAudio::GAudioManager>::Get()
#endif