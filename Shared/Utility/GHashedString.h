#ifndef __GHASHED_STRING_H
#define __GHASHED_STRING_H


/*
 I completely ripped this off of Joe =/
*/
#include <string>

class GHashedString
{
#ifdef DEBUG_KEEP_STRING
	std::string  m_String;
#endif
	unsigned int m_Hash;

public:
	GHashedString();
	GHashedString( const char * i_string );
	GHashedString( const GHashedString & i_other );
	GHashedString & operator=( const GHashedString & i_other );

	unsigned int Get() const;

	bool operator==( const GHashedString & i_other ) const;
	
	static unsigned int Hash( const char * i_string );
	static unsigned int Hash( const void * i_bytes, unsigned int i_count );

} ;

#include "GHashedString.inl"

#endif // __GHASHED_STRING_H