#include "QuatTestComp.h"

#include "Actor/ComponentManager.h"
#include "Utility/Clock.h"
#include "Math/GMath.h"
#include "Math/GVector3.h"

// If we want to draw lines, this is required...  The interface will be different with the renderer overhaul.
#include "Renderer/GDebugDraw.h"

const GHashedString QuatTestComp::m_typeName = "QuatTest";
u32 QuatTestComp::m_typeId = ComponentManager::GetNextTypeId();

void QuatTestComp::Initialize( GActorHandle i_actor, LuaPlus::LuaObject& i_obj )
{
	m_keyDown = false;
	m_quat.Identify();
}

QuatTestComp::~QuatTestComp( )
{

}

void QuatTestComp::Update( GActorHandle i_actor )
{
	HandleInputUpdate( i_actor );
}

void QuatTestComp::EndUpdate( GActorHandle i_actor )
{
	if ( m_keyDown )
	{
		// Attempt 1.  Works, but weird...
		/*
		float totalRot = fabs(acosf(m_quat.DotProduct(m_targetQuat)));
		// time / (rot speed / totalRot).
		if (totalRot > 0.0f)
			m_quat.Slerp(m_quat, m_targetQuat, g_Clock::Get().SecondsSinceLastFrame() * (GMath::Deg2Rad(30.0f) / totalRot));

		m_quat.Normalize();
		*/

		// Attempt 2.
		GVector3 target = m_targetQuat * GVector3::Forward;
		target.Normalize();
		GVector3 current = m_quat * GVector3::Forward;
		current.Normalize();
		float dot = target.Dot( current );
		if (dot < 0.99999f)
		{
			GVector3 cross = current.Cross( target );
			float sign = GMath::Sign( cross.y() );
			cross.Normalize();
			GQuat delta;
			delta.FromAngleAxis( GMath::Deg2Rad( 30.0f ) * g_Clock::Get().SecondsSinceLastFrame(), cross );
			m_quat = m_quat * delta;
			if( GMath::Sign( ( m_quat * GVector3::Forward ).Cross( m_targetQuat * GVector3::Forward ).y() ) != sign )
				m_quat = m_targetQuat;
		}

	}

	GActor* actor = GActor::FromHandle( i_actor );
	assert( actor );

	GVector3 vStartPoint = actor->m_position;
	GVector3 vEndPoint = actor->m_position + ( m_quat * GVector3::Forward * 100.0f );
	GDebugDraw::DrawLine( vStartPoint, vEndPoint, 0.002f, 0.002f, GDebugDraw::RED );

	GVector3 vEndTargetPoint = actor->m_position + ( m_targetQuat * GVector3( 0.0f, 0.0f, 100.0f ) );
	GDebugDraw::DrawLine( vStartPoint, vEndTargetPoint, 0.002f, 0.002f, GDebugDraw::BLUE );
}

IActorComponent* QuatTestComp::Create()
{
	return new QuatTestComp();
}

void QuatTestComp::HandleInputUpdate( GActorHandle i_Handle )
{

	if ( m_keyDown )
	{

	}
	else
	{
		ResetCalculation( i_Handle );
	}
}

void QuatTestComp::ResetCalculation( GActorHandle i_Handle )
{
	GActor* actor = GActor::FromHandle( i_Handle );
	GVector3 facing = -actor->m_rotation.Dir(); // TODO: Fix the z-hack soon..
	GVector3 worldZ( 0.0f, 0.0f, 1.0f );
	float angle = acosf( worldZ.Dot( facing ) );
	GVector3 ortho = worldZ.Cross( facing );
	if( ortho.LengthSquared() > 0.0f )
	{
		ortho.Normalize();
		m_quat.FromAngleAxis( angle, ortho );
	}
}

void QuatTestComp::NewRandomVector( )
{
	m_targetQuat.Identify();
	GVector3 randomVector( 0.0f, 0.5f, 0.0f );
	randomVector.x( static_cast<float>( rand() % 100) / 100.0f - 0.5f );
	randomVector.z( static_cast<float>( rand() % 100 ) / 100.0f - 0.5f );
	randomVector.Normalize();

	float angle = acosf( randomVector.Dot( GVector3::Forward ) );
	GVector3 cross = GVector3::Forward.Cross( randomVector );
	cross.Normalize();
	m_targetQuat.FromAngleAxis( angle, cross );
}

void QuatTestComp::HandleInput( GActorHandle actor, unsigned int i_CharID, bool i_keyDown )
{
	switch ( i_CharID )
	{
	case 0x49: // I
		if( m_keyDown != i_keyDown )  // If we are now key down, but were not before...
		{
			ResetCalculation( actor );
			if( m_keyDown )
				NewRandomVector();
		}
		m_keyDown = i_keyDown;
		break;
	}
}