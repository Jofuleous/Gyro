#ifndef __ICOLLISIONCALLBACK_H_
#define __ICOLLISIONCALLBACK_H_
#include "LuaPlus/LuaPlus.h"

// Collision callbacks must be registered with the collision manager.
class GContact;
class GColliderData;


class ICollisionCallback
{
public:
	// If we're to be purely data driven, we can't do like what Joe did with his collision callbacks and define the parameters in code...
	// What I'm going to do is define a creator function (with the definition above) that registers with the collision manager that returns an ICollisionCallback.
	// Basically, it will behave as a factory without actually creating one of those objects first...which I think is better, but the
	// only way to enforce it is by a templated function.

	virtual void OnCollide( const GContact* i_contact, GColliderData* i_us, GColliderData* i_them ) = 0; 
	virtual ~ICollisionCallback( ){};
};

// Callback creation defition
typedef ICollisionCallback* (*CollisionCreatorFunc) ( LuaPlus::LuaObject& i_data );

#endif