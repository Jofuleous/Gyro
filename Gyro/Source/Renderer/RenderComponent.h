#ifndef __RENDER_COMPONENT_
#define __RENDER_COMPONENT_

#include "Actor/Actor.h"
#include "Actor/IActorComponent.h"
#include "Helper/SharedPtr.h"
#include "Renderer.h"


//Interface to the physics system object...

class RenderComponent : public Gyro::IActorComponent
{
public:

	//Required
	static const GHashedString		m_typeName;
	static u32						m_typeId;

									RenderComponent(): m_renderObject( NULL ) {}
	virtual							~RenderComponent( void );

	virtual void					Update( Gyro::SharedPtr<Gyro::Actor> actor );
	virtual void					EndUpdate( Gyro::SharedPtr<Gyro::Actor> actor );
	virtual void					Initialize( Gyro::SharedPtr<Gyro::Actor> actor, libconfig::Setting& i_setting );
	virtual Gyro::IActorComponent*	Create();
	virtual GHashedString			Name( void ) const { return m_typeName; };
	virtual bool					HasUpdate( ) const { return false; };
	virtual bool					HasEndUpdate( ) const { return false; };
	Gyro::RendererObject*			GetRendererObject( ) { return m_renderObject; }

protected:
	Gyro::RendererObject*		m_renderObject;
	
};

#endif