#include "Utility/Clock.h"
#include "Components/GCollisionComp.h"
#include "Core/World.h"
#include "Components/HealthComponent.h"
#include "PlayerWeaponInv.h"

static int cmpr3( const void* a, const void* b )
{
	PlayerWeaponInv::Blaster* ad = ((PlayerWeaponInv::Blaster*)a);
	PlayerWeaponInv::Blaster* bd = ((PlayerWeaponInv::Blaster*)b);

	//SpawnData* aad = (*ad);
	//SpawnData* bbd = (*bd);

	return (int)( (*ad).m_scoreToUnlock - (*bd).m_scoreToUnlock );
}

void PlayerWeaponInv::Initialize( LuaPlus::LuaObject& i_obj )
{
	for ( LuaPlus::LuaTableIterator it( i_obj["Blasters"] ); it; it.Next() )
	{
		Blaster blaster;
		blaster.m_fireRate = it.GetValue()["FireRate"].GetFloat();
		blaster.m_projectileCount = it.GetValue()["Projectiles"].GetInteger();
		blaster.m_spread = GMath::Deg2Rad(  it.GetValue()["Spread"].GetFloat() );
		blaster.m_speed = it.GetValue()["Speed"].GetFloat();
		strcpy( blaster.m_characterName, it.GetValue()["Character"].GetString() );
		blaster.m_scoreToUnlock = it.GetValue()["Unlock"].GetInteger();

		// time for the really nasty ass hack to pre-load lua data...  not even sure what best practice is for dividing this data up.......
		const char* rootName = strrchr( blaster.m_characterName, '/' ) + 1;
		assert( rootName - 1 );

		if( g_SpawnManager::Get().m_luaState )
		{
			if( !g_SpawnManager::Get().m_luaState->GetGlobal( rootName + 1 ).IsTable() )
			{
				char path[128];
				sprintf( path, "data/Characters/%s.carx", blaster.m_characterName );
				if( g_SpawnManager::Get().m_luaState->DoFile( path ) )
				{
					if( g_SpawnManager::Get().m_luaState->GetTop() == 1 )
						assert( 0 ); // an error occured.
				}
			}
		}

		m_blasters.push_back( blaster );
	}

	qsort( &m_blasters[0], m_blasters.size(), sizeof( Blaster ), cmpr3 );

	m_totalKilled = 0;
	m_currentBlaster = 0;
	m_totalShotCount = 0;

	// note: this is really awkward.  destruction of the bullet handler should automatically be taken care of in the destructor
	// of this object as well...
	m_bulletHandler.m_messageOwner = this;
	g_MessageManager::Get().RegisterHandler( "BulletCollided", &m_bulletHandler );
		 
	m_lastBlasterShot = g_Clock::Get().SecondsSinceStart();
	m_lastBombThrow = g_Clock::Get().SecondsSinceStart();
}

PlayerWeaponInv::~PlayerWeaponInv( )
{
	g_MessageManager::Get().RemoveHandler( "BulletCollided", &m_bulletHandler );
}

void PlayerWeaponInv::FireBlaster( GActorHandle i_actor, const GVector3& i_origin, const GVector3& i_dir, const GVector3& i_parentVel )
{
	float seconds = g_Clock::Get().SecondsSinceStart();
	GActor* actor = GActor::FromHandle( i_actor );
	assert( actor );
	if( seconds - m_lastBlasterShot > m_blasters[m_currentBlaster].m_fireRate )
	{
		// this is a really shitty way to do it.  leaves us with potential tunneling...but it's the fastest method.
		// potentially move this into an actual bullet pool if performance proves to be really poor.

		float spreadTotal = m_blasters[m_currentBlaster].m_spread;
		float spreadIncrement = 0;
		if( m_blasters[m_currentBlaster].m_projectileCount > 1 )
			spreadIncrement = spreadTotal / (m_blasters[m_currentBlaster].m_projectileCount - 1 );
		float spreadStart = -(spreadTotal / 2.0f);
		float currentSpread = spreadStart;

		for( int i = 0; i < m_blasters[m_currentBlaster].m_projectileCount; i++ )
		{
			GMatrix4 rot;
			rot.SetRotY( currentSpread );
			GVector3 dir = i_dir * rot;
			GVector3 velocity( dir * m_blasters[m_currentBlaster].m_speed );
			// dirty hack.
			GActorHandle actorHandle = GActor::CreateActor( m_blasters[m_currentBlaster].m_characterName, i_origin, g_SpawnManager::Get().m_luaState );
			GActor* actor = GActor::FromHandle( actorHandle );
			GCollisionComp* col = GetComponent<GCollisionComp>( actorHandle );
			GRigidBody* body = col->GetRigidBody();
			body->m_velocity = velocity + i_parentVel;
			body->m_actor = actorHandle;
			body->m_acceleration = GVector3( 0.0f, 0.0f, 0.0f );
			body->m_markedForDelete = false;
			body->m_lastPosition = actor->m_position;
			g_World::Get().AddActor( actorHandle );

			BulletRecord rec;
			rec.m_bullet = actorHandle;
			rec.m_deathTime = seconds + 3.0f;
			m_hackyBulletTable.push_back( rec );
			currentSpread += spreadIncrement;

			m_totalShotCount++;
		}

		m_lastBlasterShot = g_Clock::Get().SecondsSinceStart();
	}
}

void PlayerWeaponInv::Update( )
{
	float time = g_Clock::Get().SecondsSinceStart();
	int size = m_hackyBulletTable.size();
	for( int i = 0; i < m_hackyBulletTable.size(); i++ )
	{
		GActor* actor = GActor::FromHandle( m_hackyBulletTable[i].m_bullet );

		if( !actor || time > m_hackyBulletTable[i].m_deathTime || actor->m_markedForDelete )
		{
			actor->Kill();
			m_hackyBulletTable[i] = m_hackyBulletTable.back();
			m_hackyBulletTable.pop_back();
		}
	}
}

void PlayerWeaponInv::FireBomb( )
{
	if( g_Clock::Get().SecondsSinceStart() - m_lastBombThrow > 0.1f )
	{

	}
}

void PlayerWeaponInv::BulletCollisionHandler::HandleMessage( GHashedString& i_string, void* i_data )
{
	// i_data is a collision contact...
	GContact* contact = (GContact*) i_data;

	assert( contact->m_first->m_owner ); // pointer is null
	assert( contact->m_second->m_owner );

	PlayerWeaponInv* weaponInv = (PlayerWeaponInv*) m_messageOwner;

	GActorHandle bulletHandle;
	GActorHandle hitActorHandle;
	GActor* first = GActor::FromHandle( contact->m_first->m_owner );
	if( first->IsA( "BasicBullet" )) // hack.  need filter types.
	{
		bulletHandle = contact->m_first->m_owner;
		hitActorHandle = contact->m_second->m_owner;
	}
	else
	{
		bulletHandle = contact->m_second->m_owner;
		hitActorHandle = contact->m_first->m_owner;
	}
	GActor* hitActor = GActor::FromHandle( hitActorHandle );
	GActor* bullet = GActor::FromHandle( bulletHandle );

	if( !hitActor || !bullet )
		return;

	if( !hitActor->m_markedForDelete && !bullet->m_markedForDelete )
	{
		// send damage to the enemy.  
		HealthComponent* health = GetComponent<HealthComponent>( hitActorHandle );
		if( health && health->m_CurrentHealth > 0 )
			health->DoDamage( 10.0f, hitActorHandle, bulletHandle );

		weaponInv->m_totalKilled++;

		if( weaponInv->m_currentBlaster + 1 < weaponInv->m_blasters.size() )
		{
			if( weaponInv->m_totalKilled > weaponInv->m_blasters[weaponInv->m_currentBlaster].m_scoreToUnlock )
			{
				weaponInv->m_currentBlaster++;
			}
		}

		bullet->Kill();
	}
}