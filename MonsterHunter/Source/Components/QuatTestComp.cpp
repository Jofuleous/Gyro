#include "QuatTestComp.h"
#include "Actor/ComponentManager.h"
#include "Utility/Clock.h"
#include "../../Renderer/Source/RenderManager.h"

const GHashedString QuatTestComp::m_typeName = "QuatTest";
u32 QuatTestComp::m_typeId = ComponentManager::GetNextTypeId();

void QuatTestComp::Initialize( GActorHandle i_actor, LuaPlus::LuaObject& i_obj )
{
	m_Up = false;
	m_Left = false;
	m_Forward = false;
	m_quat.Identify();
}

QuatTestComp::~QuatTestComp()
{

}

void QuatTestComp::Update(GActorHandle i_actor)
{
	HandleInputUpdate();
}

void QuatTestComp::EndUpdate(GActorHandle i_actor)
{
	GActor* actor = GActor::FromHandle(i_actor);
	assert(actor);
	//m_quat.GetMatrix( actor->m_rotation, GVector3(0.0f, 0.0f, 0.0f) );
	GVector3 up = actor->m_position + (m_quat * GVector3::Right * 200.0f);
	D3DXVECTOR3 upD3DX;
	up.ToD3DX(upD3DX );
	D3DXVECTOR3 pos;
	actor->m_position.ToD3DX(pos);
	D3DXCOLOR	color(1.0f, 0.0f, 0.0f, 1.0f);
	g_RenderManager.m_lines.AddLine( pos, upD3DX, 0.001f, 0.001f, 1.5f, color);
}

IActorComponent* QuatTestComp::Create()
{
	return new QuatTestComp();
}

void QuatTestComp::HandleInputUpdate( )
{

	if ( m_Up )
	{
		GQuat input;
		input.Identify();
		//input.FromAngleAxis(1.f * g_Clock::Get().SecondsSinceLastFrame(), GVector3::Forward);
		//m_quat = m_quat * input;

		input.FromAngleAxis(1.f * g_Clock::Get().SecondsSinceLastFrame(), GVector3::Up);
		m_quat = m_quat * input;
	}
}

void QuatTestComp::HandleInput( GActorHandle actor, unsigned int i_CharID, bool i_keyDown )
{
	switch ( i_CharID )
	{
		case 0x49: // I
			m_Up = true;
		break;
	}
}