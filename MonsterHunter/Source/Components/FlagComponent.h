#include "Actor/GActor.h"
#include "Actor/IActorComponent.h"
#include "Core/GMessageManager.h"


class FlagComponent : public IActorComponent
{
public:

	//Required
	static const GHashedString			m_typeName;
	static u32							m_typeId;

	FlagComponent( )
	{
		m_collisionHandler = NULL;
	}
	virtual								~FlagComponent( void );

	virtual void						Update( GActorHandle actor );
	virtual void						EndUpdate( GActorHandle actor );
	virtual IActorComponent*		Create();
	virtual void						Initialize( GActorHandle actor, LuaPlus::LuaObject& i_obj );
	virtual GHashedString				Name( void ) const { return m_typeName; }
	void								ResetFlag( );

	class FlagCollisionHandler : public GMessageHandler
	{
	public:
		virtual void HandleMessage( GHashedString& i_string, void* i_data );
		virtual ~FlagCollisionHandler() {}
	};

	class FlagReturnHandler : public GMessageHandler
	{
	public:
		virtual void HandleMessage( GHashedString& i_string, void* i_data );
		virtual ~FlagReturnHandler() {}
	};

	// message handlers
	FlagCollisionHandler*			m_collisionHandler;
	FlagReturnHandler*				m_returnHandler;

	GVector3						m_startLocation;
	int								m_teamNumber;
	GActorHandle	m_captor;
	bool							m_immune;
	float							m_stopImmune;

protected:

};