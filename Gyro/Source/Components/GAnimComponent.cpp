#include "GAnimComponent.h"
#include "Actor/ComponentManager.h"
#include "../../../Renderer/Source/ResourceTypes/Mesh.h"
#include "Animation/GAnimationUtil.h"
#include "Animation/GSkeleton.h"
#include "Animation/GAnimClip.h"
#include "GRenderComponent.h"
#include "Utility/Clock.h"
#include "Animation/GAnimNode.h"
#include "Renderer/GDebugDraw.h"

const GHashedString GAnimComponent::m_typeName = "Anim";
u32 GAnimComponent::m_typeId = ComponentManager::GetNextTypeId();

IActorComponent* GAnimComponent::Create()
{
	return new GAnimComponent();
}

void GAnimComponent::Initialize( GActorHandle actor, LuaPlus::LuaObject& i_obj )
{
	m_skeletonInstance = NULL;

	LuaPlus::LuaObject& nodes = i_obj["AnimNodes"];
	for( LuaPlus::LuaTableIterator it( nodes ); it; it.Next() )
	{
		LuaPlus::LuaObject obj = it.GetValue();
		GAnimNode* node = GAnimNode::Create(obj);
		if( node )
			m_nodeMap.Set( GHashedString::Hash(node->m_clip->m_Name), node );
	}

	//m_clip = GAnimationUtil::LoadAnimClip("Avatar/Player/GoblinRun.banm");
	m_currentNode = m_nodeMap[GHashedString::Hash("Avatar/Player/GoblinIdle.banm")]->m_data;
	m_currentNode->Begin();
	m_prevNode = NULL;
	m_clipTime = 0.0f;
	m_debugDraw = true;
}

void GAnimComponent::PlayAnim( const char* i_name )
{
	GHashNode<u32, GAnimNode*>* node = m_nodeMap[GHashedString::Hash(i_name)];
	if( node )
	{
		if( node->m_data->m_blendInEndFrame > node->m_data->m_trimStart && node->m_data != m_currentNode )
			m_prevNode = m_currentNode;

		m_currentNode = node->m_data;
		m_currentNode->Begin();
	}
}

void GAnimComponent::Update( GActorHandle actor )
{
	if( m_skeletonInstance == NULL )
	{
		GRenderComponent* comp = GetComponent<GRenderComponent>( actor );
		if( !comp )
			return;

		Gyro::RendererObject* obj = comp->GetRendererObject();
		if( !obj )
			return;

		m_skeletonInstance = obj->m_entity->m_mesh->m_reference->rm_Reference.m_skeletonInstance;
	}
	else
	{
		if( m_currentNode )
			m_currentNode->Update( m_skeletonInstance, g_Clock::Get().SecondsSinceLastFrame() );

		if( m_prevNode )
			m_prevNode->Update( m_skeletonInstance, g_Clock::Get().SecondsSinceLastFrame() );

		//else
		//	m_skeletonInstance->BuildBindPose();

		if( m_debugDraw )
		{
			// loop through first 30 or so bones and draw...
			GActor* a = GActor::FromHandle(actor);
			GVector3 worldPos = a->m_position;
			worldPos._x = worldPos._x + 2.0f;
			GMatrix4 rot = a->m_rotation;

			for( int i = 1; i < m_skeletonInstance->m_Bones.Count(); i++ )
			{
				GVector3 toWorldFirst;
				GVector3 toWorldSecond;
				GMatrix4::OrientVector3( toWorldFirst, m_skeletonInstance->m_Bones[i].m_AbsTranslation, rot );
				GMatrix4::OrientVector3( toWorldSecond, m_skeletonInstance->m_Bones[m_skeletonInstance->m_Bones[i].m_ParentId].m_AbsTranslation, rot );

				GDebugDraw::DrawLine( toWorldFirst + worldPos, ( toWorldSecond + worldPos ), 0.002f, 0.002f, GDebugDraw::RED );
			}
		}
	}

	Evaluate();
}

void GAnimComponent::Evaluate()
{
	if( m_currentNode )
	{
		if( m_prevNode )
		{
			float into = m_currentNode->CalculateBlendInto();
			if( into < 1.0f )
				GAnimationUtil::Evaluate_AnimClipBlend( m_skeletonInstance, m_currentNode->m_clip, m_prevNode->m_clip, m_currentNode->m_currTime, m_prevNode->m_currTime, into );
			else
			{
				GAnimationUtil::Evaluate_AnimClip( m_skeletonInstance, m_currentNode->m_clip, m_currentNode->m_currTime );
				m_prevNode = NULL;
			}
		}
		else
			GAnimationUtil::Evaluate_AnimClip( m_skeletonInstance, m_currentNode->m_clip, m_currentNode->m_currTime );
	}

	m_skeletonInstance->EvaluateFullInstance();
	m_skeletonInstance->CreateBoneMatrices();
}