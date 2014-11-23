#ifndef __PLAYER_WEAPON_H_
#define __PLAYER_WEAPON_H_

#include "SpawnManager.h" // bad, but let's not fail the class.
#include <vector>
#include "Actor/GActor.h"
#include "Core/GMessageManager.h"

class PlayerWeaponInv
{
public:

	class Blaster
	{
	public:
		float	m_fireRate;
		float	m_spread;
		float	m_speed;
		int		m_projectileCount;
		char	m_characterName[32];
		int		m_scoreToUnlock;
	};

	void		Initialize( LuaPlus::LuaObject& i_obj );
	void		FireBlaster( GActorHandle i_actor, const GVector3& i_origin, const GVector3& i_direction, const GVector3& i_parentVel );
	void		FireBomb( );
	void		Update( );

	float					m_lastBlasterShot;
	float					m_lastBombThrow;
	int						m_currentBlaster;
	int						m_bombCount;
	int						m_totalShotCount;
	int						m_totalKilled;


	class BulletCollisionHandler : public GMessageHandler
	{
	public:
		virtual void HandleMessage( GHashedString& i_string, void* i_data );
		virtual ~BulletCollisionHandler() {}		
	};


	struct BulletRecord
	{
		float							m_deathTime;
		GActorHandle	m_bullet;
	};

	std::vector<BulletRecord>	m_hackyBulletTable;
	std::vector<Blaster>		m_blasters;
	
	BulletCollisionHandler		m_bulletHandler;
	PlayerWeaponInv() {}
	~PlayerWeaponInv();

};

#endif