#ifndef _IACTORCOMPONENT_H_
#define _IACTORCOMPONENT_H_

#include "Utility/GSharedPtr.h"
#include "Utility/GHashedString.h"
#include "LuaPlus/LuaPlus.h"
#include "Actor/GActor.h"

// this is going to be done away with eventually...
class IActorComponent
{

public:

	virtual								~IActorComponent( void ){};
	virtual void						Update( GActorHandle actor ) = 0;
	virtual void						EndUpdate( GActorHandle actor ) = 0;
	virtual void						Initialize( GActorHandle actor, LuaPlus::LuaObject& i_obj ) = 0;
	virtual IActorComponent*			Create( ) = 0;		
	virtual GHashedString				Name( void ) const = 0;

	//a tiny bit better than calling an update.
	//once i add registration, these will no longer be needed...
	virtual bool						HasUpdate() const { return true; };
	virtual bool						HasEndUpdate() const { return true; };									
};

#endif