#ifndef __QUAT_TEST_COMP_H_
#define __QUAT_TEST_COMP_H_

//	======================================================================================================================
//	QuatTestComp.h
//	A component to test out quaternion math to ensure that it is all correct.
//	The reason it is a component is so that that component's actor can be used as the origin location for debug lines.
//========================================================================================================================

#include "Actor/GActor.h"
#include "Actor/IActorComponent.h"
#include "Math/GQuat.h"

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

	void								HandleInput(GActorHandle actor, unsigned int i_CharID, bool i_keyDown);
	void								HandleInputUpdate(GActorHandle i_Handle);
	void								ResetCalculation(GActorHandle i_Handle);
	void								NewRandomVector();

	GQuat								m_quat;
	GQuat								m_targetQuat;
	bool								m_keyDown;
};

#endif