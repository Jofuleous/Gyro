#include <malloc.h>
#include <assert.h>
#include <intrin.h>

u32 GBitField::GetNextFree( void ) const
{
	for( u32 i = 0; i < m_numElements; i++ )
	{
		if( m_bitArray[i] < MAX_U32 /*0xFFFFFFFF */ )
		{
			u32 val = ~m_bitArray[i];
			unsigned long freebit;
			_BitScanForward(&freebit, val);
			assert( freebit < sizeof(u32) * 8 );
			return i * sizeof(u32)*8 + freebit;
		}
	}

	assert( false );
	return 0;
}

u32 GBitField::GetNextSet( void ) const
{
	for(u32 i = 0; i < m_numElements; i++)
	{
		if(m_bitArray[i] > 0)
		{
			u32 val = m_bitArray[i];
			unsigned long freebit;
			_BitScanForward(&freebit, val);
			return i * sizeof(u32) * 8 + freebit;
		}
	}
	return ~0;
}

void GBitField::SetBitInUse( u32 i_bit )
{
	u32 element = i_bit / (sizeof(u32) * 8);
	u32 bit = i_bit % (sizeof(u32) * 8);
	m_bitArray[element] |= ( 0x1 << bit );
}


void GBitField::SetBitFree( u32 i_bit )
{
	u32 element = i_bit / (sizeof(u32) * 8);
	u32 bit = i_bit % (sizeof(u32) * 8);
	//this may be a larger op than other masking methods...condense later.
	u32 val = m_bitArray[element];
	m_bitArray[element] = val - (0x1 << bit);
}

bool GBitField::IsBitSet( u32 i_bit ) const
{
	//condense later.
	u32 element = i_bit / (sizeof( u32) * 8);
	u32 bit = i_bit % (sizeof( u32 ) * 8);
	u32 val = m_bitArray[element];
	if( ((0x1 << bit) & val) == 0 )
		return false;
	return true;
}


void GBitField::Clear( void )
{
	for(u32 i = 0; i < m_numElements; i++)
	{
		m_bitArray[i] = 0;
	}
}


