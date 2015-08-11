#ifndef __CONTAINERS_GLINKEDLIST_H_
#define __CONTAINERS_GLINKEDLIST_H_

//============================================================
// GLinkedList.h
// Author:	Jon Futch
// Created:	Unknown 2012-2014
//
// A singlely linked list container.
//============================================================

template<typename T>
class GLinkedNode
{
public:
	GLinkedNode( T i_data ) : m_data( i_data ), m_next( NULL ){}
	GLinkedNode( T i_data, GLinkedNode<T>* i_next ) : m_data( i_data ), m_next( i_next ){}
	T				m_data;
	GLinkedNode<T>*	m_next;
};

template<typename T>
class GLinkedList
{
public:
	GLinkedList() : m_head( NULL ) {}
	GLinkedList( T i_data ): m_head( new GLinkedNode<T>( i_data ) ) {}
	~GLinkedList() {}

	inline void					Insert( T i_data );

	// removes the first instance of this data.
	inline void					Remove( T i_data );

	// use the "Next" function in conjunction with head to traverse.
	inline GLinkedNode<T>*		Head( void );

	inline void					Next( GLinkedNode<T>*& o_next );

	GLinkedNode<T>*				m_head;
};

template<typename T>
inline GLinkedNode<T>* GLinkedList<T>::Head( void )
{
	return m_head;
}

template<typename T>
inline void GLinkedList<T>::Insert( T i_data )
{
	m_head = new GLinkedNode<T>( i_data, m_head );
}

template<typename T>
inline void GLinkedList<T>::Next( GLinkedNode<T>*& io_current )
{
	io_current = io_current->m_next;
}


// if you're using a singlely linked list, you're probably not worried about removal time...
template<typename T>
inline void GLinkedList<T>::Remove( T i_data )
{
	GLinkedNode<T>* current = m_head;
	GLinkedNode<T>* prev = NULL;

	while( current != NULL )
	{
		if( current->m_data == i_data )
		{
			// if we're the head...
			if( prev == NULL )
			{
				m_head = current->m_next;
				T data = current->m_data;
				delete current;
				return;
			}
			else
			{
				prev->m_next = current->m_next;
				delete current;
			}
		}
		current = current->m_next;
	}
}

#endif
