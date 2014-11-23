#ifndef __RENDER_COMPONENT_
#define __RENDER_COMPONENT_

#include "Actor/GActor.h"
#include "Actor/IActorComponent.h"
#include "Renderer/GRenderer.h"


//Interface to the physics system object...

class GRenderComponent : public IActorComponent
{
public:

	//Required
	static const GHashedString		m_typeName;
	static u32						m_typeId;

									GRenderComponent(): m_renderObject( NULL ) {}
	virtual							~GRenderComponent( void );

	virtual void					Update( GActorHandle actor );
	virtual void					EndUpdate( GActorHandle actor );
	virtual void					Initialize( GActorHandle actor, LuaPlus::LuaObject& i_obj );
	virtual IActorComponent*	Create();
	virtual GHashedString			Name( void ) const { return m_typeName; };
	virtual bool					HasUpdate( ) const { return false; };
	virtual bool					HasEndUpdate( ) const { return true; };
	Gyro::RendererObject*			GetRendererObject( ) { return m_renderObject; }

protected:
	Gyro::RendererObject*		m_renderObject;
	
};

#endif