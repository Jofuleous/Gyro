#ifndef __GANIM_COMPONENT_H_
#define __GANIM_COMPONENT_H_

#include "Actor/GActor.h"
#include "Actor/IActorComponent.h"
#include "Containers/GHashMap.h"

class GSkeletonInstance;
class GAnimClip;
class GAnimNode;

class GAnimComponent : public IActorComponent
{
public:
	//Required
	static const GHashedString			m_typeName;
	static u32							m_typeId;

	GAnimComponent()
	{

	}
	virtual								~GAnimComponent(void) { }

	virtual void						Update(GActorHandle actor);
	virtual void						EndUpdate(GActorHandle actor) {}
	virtual bool						HasUpdate() const { return true; }
	virtual bool						HasEndUpdate() const { return false; }
	virtual IActorComponent*			Create();
	virtual void						Initialize(GActorHandle actor, LuaPlus::LuaObject& i_obj);
	virtual GHashedString				Name(void) const { return m_typeName; }
	void								PlayAnim( const char* i_name );
	void								Evaluate();

	void					DebugDrawBones();

	float					m_clipTime;
	//GAnimClip*				m_clip;


	bool						m_debugDraw;
	GSkeletonInstance*			m_skeletonInstance;
	GHashMap<u32, GAnimNode*>	m_nodeMap;
	GAnimNode*					m_currentNode;
	GAnimNode*					m_prevNode;

};

#endif