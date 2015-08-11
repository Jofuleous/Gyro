#ifndef __MEMORY_BITFIELD_H_
#define __MEMORY_BITFIELD_H_

//===============================================================================================================
// GBitField.h
// Author: Jon Futch
// Created: Fall 2012
//
// A bit field is essentially a N sized list of bits that can be set to 1 and 0.
// This is good for things like memory pools, where we need to keep a mapping of 
// Whether an index into a memory pool is used or unused.
//===============================================================================================================

#include "Platform/Target.h"

class GBitField
{
public:
								~GBitField( );

	// Creates a bit field capable of holding "i_numItems".
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