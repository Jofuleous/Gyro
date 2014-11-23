#include "Actor/GActor.h"
#include "Actor/IActorComponent.h"
#include "Core/GMessageManager.h"


class MonsterComponent : public IActorComponent
{
public:

	//Required
	static const GHashedString			m_typeName;
	static u32							m_typeId;

	MonsterComponent( )
	{
		m_collisionHandler = NULL; // another reason why I should have Rendererlemented creation functions instead of the inheritance factory way.
	}
	virtual								~MonsterComponent( void );

	virtual void						Update( GActorHandle actor );
	virtual void						EndUpdate( GActorHandle actor );
	virtual IActorComponent*		Create();
	virtual void						Initialize( GActorHandle actor, LuaPlus::LuaObject& i_obj );
	virtual GHashedString				Name( void ) const { return m_typeName; }

	bool								IsPissed() { return m_isPissed; };

	class MonsterCollisionHandler : public GMessageHandler
	{
	public:
		virtual void HandleMessage( GHashedString& i_string, void* i_data );
		virtual ~MonsterCollisionHandler() {}
	};

	MonsterCollisionHandler*			m_collisionHandler;

protected:

	GVector3			GetRandomPosition( float width, float height ) const ;
	bool					WithinBounds( float width, float height, GActorHandle i_actor ) const ;
	bool					m_isPissed;
	float					m_lastPissed;
	float					m_MaxSpeed;
};