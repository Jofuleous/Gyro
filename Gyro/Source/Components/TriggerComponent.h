#include "Actor/IActorComponent.h"
#include "Helper/SharedPtr.h"
#include "Collision/CollisionManager.h"
#include <vector>

class GTriggerVolume;

class TriggerComponent : public Gyro::IActorComponent
{

public:
	//Required
	static const char* const		m_typeName;
	static u32						m_typeId;

	virtual void					Update( Gyro::SharedPtr<Gyro::Actor> actor );
	virtual void					EndUpdate( Gyro::SharedPtr<Gyro::Actor> actor );
	virtual void					Initialize( Gyro::SharedPtr<Gyro::Actor> actor, libconfig::Setting& i_setting );
	virtual IActorComponent*		Create( );
	virtual const char*				Name( void ) const;

	virtual bool					HasUpdate() const { return true; };
	virtual bool					HasEndUpdate() const { return true; };

	// when an object collides with an object of collision type "Trigger", this gets called.
	// i_thisObject = the collider of this specific trigger volume.
	void							OnCollide( const GContact& i_contact, GColliderData* i_thisObject );

private:
	std::vector<GTriggerVolume*>	m_callbacks;

};