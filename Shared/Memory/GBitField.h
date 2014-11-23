#ifndef _BITFIELD_H_
#define _BITFIELD_H_

#include "Platform/Target.h"

//possibly align later
class GBitField
{
public:
								~GBitField( );
	static GBitField*			Create( u32 i_numItems );

	inline u32					GetNextFree( void ) const;
	inline u32					GetNextSet( void ) const;
	inline void					SetBitInUse( u32 i_bit );
	inline void					SetBitFree( u32 i_bit );
	inline bool					IsBitSet( u32 i_bit ) const;
	inline u32					ElementCount( void ) const {  return m_numElements; }
	inline u32					ItemCount( void ) const {  return m_numItems; }
	inline u32*					Address( void ) const { return m_bitArray; }
	inline void					Clear( void );
	static void					BitField_UnitTest( void );
	

private:
								GBitField( u32* i_start, u32 i_numElements, u32 i_numItems );
								GBitField(const GBitField& copy);


	u32* const					m_bitArray;
	const u32					m_numElements;
	const u32					m_numItems;

};


#include "GBitField.inl"

#endif