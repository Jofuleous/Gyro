#ifndef __SINGLETON_
#define __SINGLETON_

//============================================================================================
// Singleton.h
// Author:	Jon Futch
// Created: Unknown 2012-2014
//
// We all want to hate on singletons, but...they are nice for a lot of things ;X
//============================================================================================

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