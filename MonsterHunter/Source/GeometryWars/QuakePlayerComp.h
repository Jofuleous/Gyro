#include "Actor/GActor.h"
#include "Actor/IActorComponent.h"
#include "Utility/GSharedPtr.h"
#include "Math/GVector3.h"
#include "Misc/GMouse.h"
#include "Misc/RBuffer.h"
#include "PlayerWeaponInv.h"
#include "Core/GMessageManager.h"

// I'll need to see how camera's are *supposed* to be done later...
//#include "../../../Renderer/Source/

class PlayerWeaponInv;

class QuakePlayerComp : public IActorComponent
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

	virtual								~QuakePlayerComp( void );

	virtual void						Update( GActorHandle actor );
	virtual void						EndUpdate( GActorHandle actor );
	virtual IActorComponent*		Create();
	virtual void						Initialize( GActorHandle actor, LuaPlus::LuaObject& i_obj );
	virtual GHashedString				Name( void ) const { return m_typeName; }

	void								HandleInput( GActorHandle actor, unsigned int i_CharID, bool i_keyDown );
	void								HandleInputUpdate( GActorHandle actor );
	void								HandleMouseButtonInput( u32 i_message );

	void								ReturnNormalSpeed( void )
										{ 
											m_maxSpeed = m_originalMaxSpeed;
											m_acceleration = m_originalAcceleration;
										}
	void								SetSpeedMultiplier( float i_multiplier )
										{
											m_maxSpeed *= i_multiplier;
											m_acceleration *= i_multiplier;
										}

	class EnemyCollisionHandler : public GMessageHandler
	{
	public:
		virtual void HandleMessage( GHashedString& i_string, void* i_data );
		virtual ~EnemyCollisionHandler() {}		
	};

	GMouse						m_mouse;
	std::vector<_keyboardInput>	m_inputs;
	bool						m_inputPending;
	static bool					m_freeCam;
	float						m_maxSpeed; // vector magnitude...
	float						m_originalMaxSpeed;
	float						m_originalAcceleration;
	float						m_acceleration;
	float						m_decelerate;
	char						m_fpsBuffer[32];
	char						m_levelBuffer[32];
	float						m_lastPosted;
	float						m_camTargetOffsetY;
	float						m_cameraPosY;
	float						m_cameraDistance;

	char						m_livesBuffer[16];
	EnemyCollisionHandler		m_enemyHandler;
	PlayerWeaponInv				m_weaponInv;
	int							m_lives;

	GVector3				m_inputVector;
	GVector3				m_lastInputVector;
	GVector3				m_lookingDirection;


};