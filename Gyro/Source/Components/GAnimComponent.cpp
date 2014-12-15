#include "GAnimComponent.h"
#include "Actor/ComponentManager.h"
#include "../../../Renderer/Source/ResourceTypes/Mesh.h"
#include "../../../Renderer/Source/RenderManager.h"
//#include "Animation/GAnimationUtil.h" this caused a linker error.  investigate later.
#include "Animation/GSkeleton.h"
#include "Animation/GAnimClip.h"
#include "GRenderComponent.h"
#include "Utility/Clock.h"
#include "Animation/GAnimNode.h"

const GHashedString GAnimComponent::m_typeName = "Anim";
u32 GAnimComponent::m_typeId = ComponentManager::GetNextTypeId();

IActorComponent* GAnimComponent::Create()
{
	return new GAnimComponent();
}

void GAnimComponent::Initialize(GActorHandle actor, LuaPlus::LuaObject& i_obj)
{
	m_skeletonInstance = NULL;

	LuaPlus::LuaObject& nodes = i_obj["AnimNodes"];
	for (LuaPlus::LuaTableIterator it(nodes); it; it.Next())
	{
		LuaPlus::LuaObject obj = it.GetValue();
		GAnimNode* node = GAnimNode::Create(obj);
		if (node)
			m_nodeMap.Set(GHashedString::Hash(node->m_clip->m_Name), node);
	}

	//m_clip = GAnimationUtil::LoadAnimClip("Avatar/Player/GoblinRun.banm");
	m_currentNode = m_nodeMap[GHashedString::Hash("Avatar/Player/GoblinIdle.banm")]->m_data;
	m_currentNode->Begin();
	m_clipTime = 0.0f;
}

void GAnimComponent::PlayAnim(const char* i_name)
{
	GHashNode<u32, GAnimNode*>* node = m_nodeMap[GHashedString::Hash(i_name)];
	if (node)
	{
		m_currentNode = node->m_data;
		m_currentNode->Begin();
	}
}

void GAnimComponent::Update(GActorHandle actor)
{
	if (m_skeletonInstance == NULL)
	{
		GRenderComponent* comp = GetComponent<GRenderComponent>(actor);
		if (!comp)
			return;

		Gyro::RendererObject* obj = comp->GetRendererObject();
		if (!obj)
			return;

		m_skeletonInstance = obj->m_entity->m_mesh->m_reference->rm_Reference.m_skeletonInstance;
		m_debugDraw = true;
	}
	else
	{

		if (m_currentNode )
		{
			m_currentNode->Update(m_skeletonInstance, g_Clock::Get().SecondsSinceLastFrame());
			//m_clip->UpdateSkeletonInstace(m_skeletonInstance, m_clipTime);
			//m_clipTime += 15.0f * g_Clock::Get().SecondsSinceLastFrame();
		}
		else if (!m_currentNode)
			m_skeletonInstance->BuildBindPose();
		else
			m_clipTime = 0.0f;

		m_skeletonInstance->EvaluateFullInstance();
		m_skeletonInstance->CreateBoneMatrices();

		if (m_debugDraw)
		{
			// loop through first 30 or so bones and draw...
			D3DXVECTOR3 first;
			D3DXVECTOR3 second;
			D3DXCOLOR	color(1.0f, 0.0f, 0.0f, 1.0f);

			GActor* a = GActor::FromHandle(actor);
			GVector3 worldPos = a->m_position;
			worldPos._x = worldPos._x + 50.0f;
			GMatrix4 rot = a->m_rotation;
			//rot.PokeScale( GVector3( 5.0f, 5.0f, 5.0f ) );


			for (int i = 1; i < m_skeletonInstance->m_Bones.Count(); i++)
			{
				GVector3 toWorldFirst;
				GVector3 toWorldSecond;
				GMatrix4::OrientVector3(toWorldFirst, m_skeletonInstance->m_Bones[i].m_AbsTranslation, rot);
				GMatrix4::OrientVector3(toWorldSecond, m_skeletonInstance->m_Bones[m_skeletonInstance->m_Bones[i].m_ParentId].m_AbsTranslation, rot);
				( toWorldFirst + worldPos ).ToD3DX(first);
				( toWorldSecond + worldPos ).ToD3DX(second);
				//DEBUG_PRINT("%d - %f %f %f\n", i, toWorldFirst.x(), toWorldFirst.y(), toWorldFirst.z());

				g_RenderManager.m_lines.AddLine(first, second, 0.002f, 0.002f, 1.5f, color);
			}
		}
	}
}