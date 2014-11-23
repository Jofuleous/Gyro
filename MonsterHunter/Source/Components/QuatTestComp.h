#ifndef __QUAT_TEST_COMP_H_
#define __QUAT_TEST_COMP_H_

#include "Actor/GActor.h"
#include "Actor/IActorComponent.h"
#include "Utility/GSharedPtr.h"
#include "Math/GVector3.h"
#include "Misc/GMouse.h"
#include "Misc/RBuffer.h"
#include "Core/GMessageManager.h"
#include "Navigation/GWayPointManager.h"
#include "Audio/GAudioManager.h"
#include "Math/GTransform.h"
#include "Containers/GArray.h"

// I'll need to see how camera's are *supposed* to be done later...
//#include "../../../Renderer/Source/

class QuatTestComp : public IActorComponent
{
public:

	//Required
	static const GHashedString			m_typeName;
	static u32							m_typeId;

	struct _keyboardInput
	{
		unsigned int					m_CharID;
		bool							m_keyDown;
	};

	virtual								~QuatTestComp(void);

	virtual void						Update(GActorHandle actor);
	virtual void						EndUpdate(GActorHandle actor);
	virtual IActorComponent*			Create();
	virtual void						Initialize(GActorHandle actor, LuaPlus::LuaObject& i_obj);
	virtual GHashedString				Name(void) const { return m_typeName; }
	//virtual bool						HasUpdate() const { return false; };
	//virtual bool						HasEndUpdate() const { return false; };

	void								HandleInput(GActorHandle actor, unsigned int i_CharID, bool i_keyDown);
	void								HandleInputUpdate();

	GQuat								m_quat;
	bool								m_Up;
	bool								m_Left;
	bool								m_Forward;
};

#endif