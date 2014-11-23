#include <assert.h>

inline GHashedString::GHashedString( ) :
	m_Hash( Hash( "" ) )
{
}

inline GHashedString::GHashedString( const char * i_string ) :
	m_Hash( Hash( i_string ) )
#ifdef DEBUG_KEEP_STRING
	, m_String( i_string )
#endif
{
}

inline GHashedString::GHashedString( const GHashedString & i_other ) :
m_Hash( i_other.m_Hash )
#ifdef DEBUG_KEEP_STRING
	, m_String( i_other.m_String )
#endif
{

}

inline GHashedString & GHashedString::operator=( const GHashedString & i_other )
{
	m_Hash = i_other.m_Hash;

#ifdef DEBUG_KEEP_STRING
	m_String = i_other.m_String;
#endif

	return *this;
}

inline unsigned int GHashedString::Get( void ) const
{
	return m_Hash;
}

inline bool GHashedString::operator==( const GHashedString & i_other ) const
{
	return m_Hash == i_other.m_Hash;
}