#include "GAnimNode.h"
#include "GAnimClip.h"
#include "GAnimationUtil.h"
#include "GSkeleton.h"

GAnimNode* GAnimNode::Create(LuaPlus::LuaObject& i_obj)
{
	const char* clipName = i_obj["ClipName"].GetString();
	GAnimClip* clip = GAnimationUtil::LoadAnimClip(clipName);
	if (!clip)
		return NULL;

	GAnimNode* node = new GAnimNode();
	node->m_clip = clip;
	node->m_trimStart = i_obj["TrimStart"].GetFloat();
	node->m_trimEnd = i_obj["TrimEnd"].GetFloat();
	node->m_loop = i_obj["Loop"].GetBoolean();
	node->m_playRate = i_obj["PlayRate"].GetFloat();
	node->m_currTime = node->m_trimStart;
	return node;
}

bool GAnimNode::Update(GSkeletonInstance* i_skelly, float i_dt)
{
	m_currTime += (i_dt * m_playRate);

	if (m_currTime > m_trimEnd)
		m_currTime = m_trimEnd;

	m_clip->UpdateSkeletonInstace(i_skelly, m_currTime);

	if ((m_currTime >= m_clip->m_ClipLength || m_currTime >= m_trimEnd))
	{
		if (m_loop)
			m_currTime = m_trimStart;
		else
			return true;
	}

	return false;
}

void GAnimNode::Begin()
{
	m_currTime = m_trimStart;
}

void GAnimNode::End()
{

}