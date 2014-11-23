#include "NamedBitSet.h"

template<typename T>
u32 GNamedBitSet<T>::GetBitIndex( const GHashedString& i_hashString )
{
	for( u32 i = 0; i < m_strings.size() && i < NumBits ; i++ )
	{
		if( i_hashString == m_strings[i] )
			return i;
	}

	m_strings.push_back( i_hashString );
	return 0;
}