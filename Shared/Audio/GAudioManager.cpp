#include "GAudioManager.h"

#include <stdio.h>
#include <string.h>

namespace GAudio
{

bool GAudioManager::Initialize( )
{
	FMOD_RESULT result;
	unsigned int version;
	int numDrivers;
	FMOD_SPEAKERMODE speakerMode;
	FMOD_CAPS caps;
	char name[256];
 
	// Create FMOD interface object
	result = FMOD::System_Create(&m_fmodSystem);
	if( result != FMOD_OK )
		return false;

	// Check version
	result = m_fmodSystem->getVersion( &version );
	if( result != FMOD_OK )
		return false;
 
	//"Error! You are using an old version of FMOD " << version << ".
	if( version < FMOD_VERSION )
		return false;

	result = m_fmodSystem->getNumDrivers( &numDrivers );
	if( result != FMOD_OK )
		return false;

	// audio will not work.
	if( numDrivers == 0 )
	{
		result = m_fmodSystem->setOutput(FMOD_OUTPUTTYPE_NOSOUND);
		if( result != FMOD_OK )
			return false;
	}
	// At least one sound card
	else
	{
		// Get the capabilities of the default (0) sound card
		result = m_fmodSystem->getDriverCaps(0, &caps, 0, &speakerMode);
		if( result != FMOD_OK )
			return false;
 
        // Set the speaker mode to match that in Control Panel
        result = m_fmodSystem->setSpeakerMode(speakerMode);
		if( result != FMOD_OK )
			return false;
 
        // Increase buffer size if user has Acceleration slider set to off
        if (caps & FMOD_CAPS_HARDWARE_EMULATED)
        {
            result = m_fmodSystem->setDSPBufferSize(1024, 10);
			if( result != FMOD_OK )
				return false;
        }
 
        // Get name of driver
        result = m_fmodSystem->getDriverInfo(0, name, 256, 0);
		if( result != FMOD_OK )
			return false;
 
        // SigmaTel sound devices crackle for some reason if the format is PCM 16-bit.
        // PCM floating point output seems to solve it.
        if ( strstr(name, "SigmaTel") )
        {
            result = m_fmodSystem->setSoftwareFormat(48000, FMOD_SOUND_FORMAT_PCMFLOAT, 0, 0, FMOD_DSP_RESAMPLER_LINEAR);
            if( result != FMOD_OK )
        		return false;
		}
    }

	// Initialise FMOD
    result = m_fmodSystem->init(100, FMOD_INIT_NORMAL, 0);
 
    // If the selected speaker mode isn't supported by this sound card, switch it back to stereo
    if (result == FMOD_ERR_OUTPUT_CREATEBUFFER)
    {
        result = m_fmodSystem->setSpeakerMode(FMOD_SPEAKERMODE_STEREO);
        if( result != FMOD_OK )
 			return false;

        result = m_fmodSystem->init(100, FMOD_INIT_NORMAL, 0);
    }

    if( result != FMOD_OK )
		return false;

	// 3d settings
	m_fmodSystem->set3DSettings(1.0f, 1.0f, 0.5f);

	// create the music and effects channel groups.
	m_fmodSystem->createChannelGroup( NULL, &m_musicChannelGroup );
	m_fmodSystem->createChannelGroup( NULL, &m_effectChannelGroup );
	m_effectChannelGroup->setVolume( 1.0f );
	m_musicChannelGroup->setVolume( 1.0f );

	m_musicChannel = 0;
	m_musicVolume = 1.0f;
	m_sfxVolume = 1.0f;

	return true;
}

void GAudioManager::UpdateListenerPosition( const GVector3& i_listenerPos, const GVector3& i_forward, const GVector3& i_up )
{
	FMOD_VECTOR vel = {0.0f, 0.0f, 0.0f }; 
	m_fmodSystem->set3DListenerAttributes( 0, (FMOD_VECTOR*)&i_listenerPos, &vel, (FMOD_VECTOR*)(&i_forward), (FMOD_VECTOR*)(&i_up));
}

void GAudioManager::Update( )
{
	m_fmodSystem->update();
}

bool GAudioManager::LoadWorldAudio( LuaPlus::LuaObject& i_obj )
{
	return true;
}

FMOD::Channel* GAudioManager::PlayEffect( const char* i_effectPath, bool i_loop, bool i_3d, FMOD::Sound** o_sound, FMOD::Channel* i_channel)
{
	FMOD::Sound* effect;
	u32 bits = 0;
	if( i_3d )
	{
		bits |= FMOD_3D;
		bits |= FMOD_3D_WORLDRELATIVE;
		bits |= FMOD_3D_LINEARROLLOFF;
	}
	else
		bits |= FMOD_2D;
	if( i_loop )
		bits |= FMOD_LOOP_NORMAL;
	else
		bits |= FMOD_LOOP_OFF;

	FMOD_RESULT result =  m_fmodSystem->createStream( i_effectPath, bits, 0, &effect); // this is probably creating shit over and over.  fuck.
	if( result != FMOD_OK )
		return NULL;

	if( o_sound )
		*o_sound = effect;

	if( i_channel )
	{
		result = m_fmodSystem->playSound(FMOD_CHANNEL_REUSE, effect, false, &i_channel );
		i_channel->setChannelGroup( m_effectChannelGroup );
		if( i_loop )
			i_channel->setLoopCount( 9000 );
		return i_channel;
	}

	FMOD::Channel* newChannel;
	result = m_fmodSystem->playSound(FMOD_CHANNEL_FREE, effect, false, &newChannel );
	newChannel->setChannelGroup( m_effectChannelGroup );
	if( i_loop )
		newChannel->setLoopCount( 9000 );
	return newChannel;
}

bool GAudioManager::PlayMusic( const char* i_filepath )
{
	FMOD::Sound *song1;
	FMOD_RESULT result =  m_fmodSystem->createStream( i_filepath, FMOD_DEFAULT, 0, &song1);
	if( result != FMOD_OK )
		return false;

	// Assign each song to a channel and start them paused
    FMOD::Channel* channel1;
    result = m_fmodSystem->playSound(FMOD_CHANNEL_REUSE, song1, false, &channel1);
	channel1->setChannelGroup( m_musicChannelGroup );
	// Songs should repeat forever
	channel1->setLoopCount(9000);

	return true;
}

void GAudioManager::UpdateChannelPosition( FMOD::Channel* i_channel, const GVector3& i_pos )
{
	if( i_channel )
	{
		FMOD_VECTOR vel;
		vel.x =0.0f;
		vel.y = 0.0f;
		vel.z = 0.0f;
		i_channel->set3DAttributes( (FMOD_VECTOR*) &i_pos, 0 );
	}
}

void GAudioManager::SetEffectVolume( float i_volume )
{
	m_effectChannelGroup->setVolume( i_volume );
}

void GAudioManager::SetMusicVolume( float i_volume )
{
	m_musicChannelGroup->setVolume( i_volume );
}

}