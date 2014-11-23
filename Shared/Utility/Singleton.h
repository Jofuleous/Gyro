#ifndef __SINGLETON_
#define __SINGLETON_
#include <assert.h>

template<class T>
class Singleton
{
public:
	static inline T& Get()
	{

		if( !m_instance )
			m_instance = new T();

		assert( m_instance );
		return *m_instance;
	}

	static inline void Release()
	{
		if( m_instance )
		{
			delete m_instance;

			m_instance = 0;
		}
	}

private:
	static T* m_instance;
};

template<class T> T* Singleton<T>::m_instance = 0;


#endif