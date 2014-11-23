#ifndef _RBUFFER_H_
#define _RBUFFER_H_
#include <assert.h>

namespace Gyro
{
template<typename T>
class RBuffer
{
	T*				_list;
	unsigned int	_size;
	unsigned int	_count;
	int				_current;

public:
	RBuffer():
	  _list( new T[50] ), _size( 50 ), _current( 0 ), _count( 0 )
	{
		assert(_list);
	}

	RBuffer( const unsigned int S ):
	  _list( new T[S] ), _size( S ), _current( 0 ), _count( 0 )
	{
		assert(_list);
	}
	~RBuffer(){ delete _list; }

	void						Push( const T& value );
	const T&					Pop( void );
	const T&					operator[]( const int index );
	const unsigned int			GetCount( ) const { return _count; }	
	const unsigned int			GetSize( ) const { return _size; }
	const unsigned int			GetValidCount( ) const { return _count > _size ? _size : _count; }
	static void					RBuffer_UnitTest( void );
	

};


template< typename T >
void RBuffer<T>::Push( const T& value )
{
	_list[_current] = value;
	_current = (_current + 1) % _size;
	_count++;
}

template< typename T >
const T& RBuffer<T>::Pop( void )
{
	_count--;
	_current -= 1;
	if( _current < 0 )
		_current = _size - 1;
	return _list[_current];
}

template< typename T >
const T& RBuffer<T>::operator[]( const int index )
{
	int back = index % _size;
	unsigned int i;
	if( back > (_current - 1) )
	{
		i = _size + (_current - 1 - back);
		return _list[i];
	}
	i = _current - 1 - back;
	return _list[i];
}


#ifdef _DEBUG
template<typename T>
void RBuffer<T>::RBuffer_UnitTest( void )
{
	//Query 10 elements back when adding 15 elements to the rbuffer.
	int numElements = 10;
	int numAdded = 15;
	Gyro::RBuffer<int> buffer( numElements );
	int i;
	for(i = 0; i < numAdded; i++)
	{
		buffer.Push( i );
		for(int j = 0; (j <= i) && j < numElements; j++)
			assert( buffer[j] == i - j);
	}

	//Assure count is correct...
	assert( buffer.GetCount() == numAdded );

	//Pop off all of the elements of the buffer.
	int numRemoved = 0;
	for(int j = 1; j <= numElements && j <= numAdded; j++)
	{
		int val = buffer.Pop();
		assert( val == i - j );
		numRemoved++;
	}

	//Assure all removed...
	assert( buffer.GetCount() == numAdded - numRemoved);
	return;
}
#else

template<typename T>
void RBuffer<T>::RBuffer_UnitTest( void )
{
	return;
}

#endif


}





#endif