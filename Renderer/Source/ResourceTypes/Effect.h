#ifndef __EFFECT_H_
#define __EFFECT_H_

#include "../ResourceMaster.h"
#include "../ResourcePtr.h"
#include "../ResourceLoader.h"
#include <vector>
//#include "VertexShader.h"
//#include "PixelShader.h"
//#include "../Initialization.h"
#include "Technique.h"

//Not worried about memory....more performance.
class EffectData
{
public:
	EffectData( ) : m_depth( 0 ), m_opaque( 0 ), m_postOpaque( 0 ), m_postProcess( 0 ), m_alpha( 0 ), m_UI( 0 ), m_shadow( 0 ) {}
	~EffectData()
	{
		if( m_depth )
			delete m_depth;
		if( m_opaque )
			delete m_opaque;
		if( m_postOpaque )
			delete m_postOpaque;
		if( m_alpha )
			delete m_alpha;
		if( m_postProcess )
			delete m_postProcess;
		if( m_UI )
			delete m_UI;
		if( m_shadow )
			delete m_shadow;
	}

	Technique*		m_depth;
	Technique*		m_opaque;
	Technique*		m_postOpaque;
	Technique*		m_alpha;
	Technique*		m_postProcess;
	Technique*		m_UI;
	Technique*		m_shadow;
	bool			m_skinHack;
	unsigned __int16				m_effectId; // todo: turn back to u8
};

class EffectReference : public ResourceMaster<EffectData>
{
public:
	~EffectReference()
	{

	}
};

class EffectLoader : public ResourceLoader<EffectReference>
{
public:
	virtual EffectReference* Load( const char* i_filename );
};

#endif