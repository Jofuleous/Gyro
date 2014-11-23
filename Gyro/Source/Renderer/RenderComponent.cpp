#include "RenderComponent.h"
#include "Renderer/Renderer.h"
#include "Actor/ComponentManager.h"
#include "../../../External/Cheesy/Exports/Cheesy/Cheesy.h"
#include "../../../External/Cheesy/Exports/Cheesy/Sprite.h"

const GHashedString RenderComponent::m_typeName = "Renderer";
u32 RenderComponent::m_typeId = Gyro::ComponentManager::GetNextTypeId();

RenderComponent::~RenderComponent()
{
	/*
	if( m_renderObject )
	{
		g_Renderer::Get().RemoveRendererObject( m_renderObject );
		delete m_renderObject;
	}
	*/
}

//This will be reading from a config file eventually...for now, just expose a way of assigning a renderer object...
void RenderComponent::Initialize( Gyro::SharedPtr<Gyro::Actor> i_actor, libconfig::Setting& i_setting )
{
	m_renderObject = new Gyro::RendererObject( i_actor );


	libconfig::Setting& attributes = i_setting["Attributes"];

	libconfig::Setting& color = attributes["Color"];

	//color
	unsigned col;
	color.lookupValue( "r", col );
	m_renderObject->m_color.r = (char)col;
	color.lookupValue( "g", col );
	m_renderObject->m_color.g = (char)col;
	color.lookupValue( "b", col );
	m_renderObject->m_color.b = (char)col;
	m_renderObject->m_color.a = 255;

	if( attributes.exists( "Width" ) && attributes.exists( "Height" ) && attributes.exists( "Depth" ))
	{
		float width, height, depth;
		attributes.lookupValue( "Width", width );
		attributes.lookupValue( "Height", height );
		attributes.lookupValue( "Depth", depth );
		m_renderObject->m_sprite = Cheesy::Sprite::Create( Cheesy::Point2D( 0.0f,0.0f), depth, Cheesy::Point2D( width, height ), m_renderObject->m_color );
	}
	else
		m_renderObject->m_sprite = Cheesy::Sprite::Create( Cheesy::Point2D( 0.0f,0.0f), 1.0f, Cheesy::Point2D( 40.0f, 40.0f ), m_renderObject->m_color );

	m_renderObject->m_transform.Identify();

	g_Renderer::Get().AddRendererObject( m_renderObject );
}

Gyro::IActorComponent* RenderComponent::Create()
{
	return new RenderComponent();
}

void RenderComponent::Update( Gyro::SharedPtr<Gyro::Actor> i_actor )
{

}

void RenderComponent::EndUpdate( Gyro::SharedPtr<Gyro::Actor> i_actor)
{

}