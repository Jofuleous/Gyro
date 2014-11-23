#include "Actor/GActor.h"
#include "Actor/IActorComponent.h"
#include "Core/GMessageManager.h"

// this is hacky, but works for the assignment requirements for this week...

class TeamComponent : public IActorComponent
{
public:

	//Required
	static const GHashedString			m_typeName;
	static u32							m_typeId;

	TeamComponent( )
	{

	}
	virtual								~TeamComponent( void );

	virtual void						Update( GActorHandle actor ) {}
	virtual void						EndUpdate( GActorHandle actor ) { }
	virtual IActorComponent*		Create();
	virtual void						Initialize( GActorHandle actor, LuaPlus::LuaObject& i_obj );
	virtual GHashedString				Name( void ) const { return m_typeName; }
	virtual bool						HasUpdate() const { return false; };
	virtual bool						HasEndUpdate() const { return false; };		
	void								SetTeamNumber( int i_number ) { m_teamNumber = i_number; }
	void								SetFlag( bool i_bool );
	void								RemoveFlag();

	class TeamCollisionHandler : public GMessageHandler
	{
	public:
		virtual void HandleMessage( GHashedString& i_string, void* i_data );
		virtual ~TeamCollisionHandler() {}
	};

	TeamCollisionHandler*				m_collisionHandler;

	int									m_teamNumber;
	bool								m_carryingFlag;

protected:

};