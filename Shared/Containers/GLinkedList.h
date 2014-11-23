// Since a linked list is traversed linearly, it will probably be best to use a memory pool or some type of contiguous
// memory for nodes if the user specifies a set amount of nodes...this may not be safely possible, but i can figure something out later.


// use the linked nodes to iterate through.
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
