#include "GBitField.h"

GBitField::GBitField( u32* i_start, u32 i_numElements, u32 i_numItems ): m_bitArray( i_start ),
	m_numElements( i_numElements), m_numItems( i_numItems )
{
}

GBitField::~GBitField( void )
{
	_aligned_free( m_bitArray );
}

GBitField* GBitField::Create( u32 i_numItems )
{
	u32 num_elements = i_numItems / sizeof( u32 ) + 1; //+1 because of truncation
	void* array_start = _aligned_malloc( sizeof( u32 ) * num_elements, CACHE_LINE );
	assert( array_start );

	if( array_start )
	{
		GBitField* ret = new GBitField( (u32*)array_start, num_elements, i_numItems);
		assert( ret ); // make sure this tiny allocation didn't fail...
		ret->Clear();

		return ret;
	}
	return NULL;
}

#ifdef _DEBUG
void GBitField::BitField_UnitTest( void )
{
	GBitField* field = GBitField::Create( 1000 );
	field->SetBitInUse( 10 );
	field->SetBitInUse( 1 );
	assert(field->IsBitSet( 10 ));
	assert(field->IsBitSet( 1 ));
	u32 next = field->GetNextFree();

	assert( (u32)field->Address() % CACHE_LINE == 0 ); 
	assert( next == 0 );
	field->SetBitInUse( next );
	next = field->GetNextFree();
	assert( next == 2 );
	field->SetBitInUse(1000);
	assert( field->IsBitSet( 1000 ) );
	for(u32 i = 0; i < 200; i++)
	{
		field->SetBitInUse( i );
	}

	for(u32 i = 0; i < 200; i++)
	{
		assert( field->IsBitSet( i ) );
	}

	assert( field->GetNextFree() == 200 );
	field->SetBitInUse( 200 );

	for(u32 i = 0; i < 200; i++)
	{
		field->SetBitFree( i );
		if( i != 199)
			assert( field->GetNextSet() == i+1);
	}

	for(u32 i = 0; i < 200; i++ )
	{
		assert( !field->IsBitSet( i ) );
	}

	assert( field->IsBitSet( 200 ) );
	field->Clear();
	for(u32 i = 0; i < field->ItemCount(); i++)
	{
		assert( !field->IsBitSet( i ) );
	}

	delete field;
}

#else
void GBitField::BitField_UnitTest( void )
{
	return;
}
#endif