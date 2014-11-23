#include "GRenderComponent.h"
#include "Actor/ComponentManager.h"
#include "Utility/Clock.h"

#include "../../Renderer/Source/ResourceManager.h"
#include "../../Renderer/Source/ResourceTypes/Entity.h"

#include <iostream>

const GHashedString GRenderComponent::m_typeName = "Renderer";
u32 GRenderComponent::m_typeId =ComponentManager::GetNextTypeId();

GRenderComponent::~GRenderComponent()
{
	/*
	if( m_renderObject )
	{
		g_Renderer::Get().RemoveRendererObject( m_renderObject );
		delete m_renderObject;
	}
	*/
}


void GRenderComponent::Initialize( GActorHandle i_actor, LuaPlus::LuaObject& i_obj )
{
	m_renderObject = new Gyro::RendererObject( i_actor );
	GActor* actor = GActor::FromHandle( i_actor );
	assert( actor );
	const char* entityName = i_obj["Entity"].GetString();

	Entity* entity = Entity::LoadEntity( entityName );
	m_renderObject->m_entity = entity;
	entity->DefaulInit( );
	actor->m_position.ToD3DX( entity->m_position );
	// need orientation..

	g_Renderer::Get().AddRendererObject( m_renderObject );
}

IActorComponent* GRenderComponent::Create()
{
	return new GRenderComponent();
}

void GRenderComponent::Update( GActorHandle i_actor )
{
	float dt = g_Clock::Get().SecondsSinceLastFrame();
	m_renderObject->m_entity->Update( dt );
}

void GRenderComponent::EndUpdate( GActorHandle i_actor)
{
	GActor* actor = GActor::FromHandle( i_actor );
	assert( actor );
	actor->m_position.ToD3DX( m_renderObject->m_entity->m_position );
}