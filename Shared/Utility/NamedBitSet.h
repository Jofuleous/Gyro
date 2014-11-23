#ifndef __NAMEDBITSET_H_
#define __NAMEDBITSET_H_

#include "GHashedString.h"
#include "Platform/Target.h"
#include <vector>

template<typename T>
class GNamedBitSet
{
public:

	static const u32 NumBits = sizeof( T ) * 4;

	GNamedBitSet(){}
	~GNamedBitSet(){}

	u32							GetBitIndex( const GHashedString& i_hashString );

	T							GetBitMask( const GHashedString& i_hashString ) 
								{ return 1 << GetBitIndex( i_hashString ); }

	// for now, let's just use a vector array.  i know, i know.  i'm really lazy...
	std::vector<GHashedString>	m_strings;
};

template<typename T>
u32 GNamedBitSet<T>::GetBitIndex( const GHashedString& i_hashString )
{
	for( u32 i = 0; i < m_strings.size() && i < NumBits ; i++ )
	{
		if( i_hashString == m_strings[i] )
			return i;
	}

	if( m_strings.size() < NumBits )
	{	
		m_strings.push_back( i_hashString );
		return m_strings.size()-1;
	}
	else
		assert( 0 ); // the number of collision bits has been exceeded.  increase to 64 bit?
	return 0;
}

#endif