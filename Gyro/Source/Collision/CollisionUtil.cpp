#include "CollisionUtil.h"
#include "Math/GMath.h"

// lots of inspiration from Joe's lecture and sample code.

namespace GCollision
{

	bool CheckIfAxisOverlap( float i_RangeBegin, float i_RangeEnd, float i_RayLength, float i_RayStart, float &o_enter, float &o_exit, float& normal )
	{
		normal = -1.0f;
		
		if( i_RangeBegin > i_RangeEnd )
		{
			float temp = i_RangeBegin;
			i_RangeBegin = i_RangeEnd;
			i_RangeEnd = i_RangeBegin;
		}
		
		if( i_RayLength == 0.0f )
		{
			if( i_RayStart < i_RangeBegin || i_RayStart > i_RangeEnd )
				return false;
			else
			{
				o_enter = -1.0f;
				o_exit = 1.0f;
				normal = 1.0f;

				return true;
			}
		}

		o_enter = ( i_RangeBegin - i_RayStart ) / i_RayLength;
		o_exit = ( i_RangeEnd - i_RayStart ) / i_RayLength;

		if( o_exit < o_enter )
		{
			normal = 1.0f;
			float temp = o_exit;
			o_exit = o_enter;
			o_enter = temp;
		}

		return !((o_enter >= 1.0f) || (o_exit <= 0.0f));
	}

bool SATAABB( const GAABB& i_boxA, const GVector3& i_boxAPos, const GVector3& i_boxAVel,
				const GAABB& i_boxB, const GVector3& i_boxBPos, const GVector3& i_boxBVel, float i_time, GContact& o_contact )
	{
		//box two new velocity
		GVector3 b2Velocity = i_boxBVel - i_boxAVel;

		//calculate pB2 -- point B 2 (end of frame location)
		GVector3 pB_2( i_boxB.m_Center.x() + b2Velocity.x() * i_time,
							i_boxB.m_Center.y() + b2Velocity.y() * i_time,
							i_boxB.m_Center.z() + b2Velocity.z() * i_time );

		float finalEnter = -1.0f;
		float finalExit = 1.0f;
		float normal = 1.0f;
		o_contact.m_firstNormal = GVector3::Up;

		float tempEnter, tempExit;
		//////TEST X PLANE/////
		//bloat A's x_max.  Check for swept collision with B2 - B1
		float rangeEnd = i_boxA.m_HalfX + i_boxA.m_Center.x() + i_boxAPos.x() + i_boxB.m_HalfX; // this is the bloated X value.
		float rangeBegin = i_boxA.m_Center.x() - i_boxA.m_HalfX + i_boxAPos.x() - i_boxB.m_HalfX;

		if( !CheckIfAxisOverlap( rangeBegin, rangeEnd, pB_2.x() - i_boxB.m_Center.x(), i_boxB.m_Center.x() + i_boxBPos.x(), tempEnter, tempExit, normal ) )
			return false;

		if( tempEnter > finalEnter )
		{
			GVector3 right( 1.0f, 0.0f, 0.0f );
			right.x( normal );
			o_contact.m_firstNormal = right;
			finalEnter = tempEnter;
		}


		//////TEST Y PLANE/////
		//bloat A's y_max.
		rangeEnd = i_boxA.m_HalfY + i_boxA.m_Center.y() + i_boxAPos.y() + i_boxB.m_HalfY;
		rangeBegin = i_boxA.m_Center.y() - i_boxA.m_HalfY + i_boxAPos.y() - i_boxB.m_HalfY;

		if( !CheckIfAxisOverlap( rangeBegin, rangeEnd, pB_2.y() - i_boxB.m_Center.y(), i_boxB.m_Center.y() + i_boxBPos.y(), tempEnter, tempExit, normal ) )
			return false;

		if( tempEnter > finalEnter )
		{
			GVector3 up( 0.0f, 1.0f, 0.0f );
			up.y( normal );
			o_contact.m_firstNormal = up;

			finalEnter = tempEnter;
		}
		if( tempExit < finalExit )
			finalExit = tempExit;

		//////TEST Z PLANE/////

		rangeEnd = i_boxA.m_HalfZ + i_boxA.m_Center.z() + i_boxAPos.z() + i_boxB.m_HalfZ;
		rangeBegin = i_boxA.m_Center.z() - i_boxA.m_HalfZ + i_boxAPos.z() - i_boxB.m_HalfZ;

		if( !CheckIfAxisOverlap( rangeBegin, rangeEnd, pB_2.z() - i_boxB.m_Center.z(), i_boxB.m_Center.z() + i_boxBPos.z(), tempEnter, tempExit, normal ) )
			return false;

		if( tempEnter > finalEnter )
		{
			// not sure.
			GVector3 forward( 0.0f, 0.0f, 1.0f );
			forward.z( normal );
			o_contact.m_firstNormal = forward;

			finalEnter = tempEnter;
		}
		if( tempExit < finalExit )
			finalExit = tempExit;

		o_contact.m_time = finalEnter;
		return finalEnter < finalExit ? true : false;
	}

bool SATOBBCheck( const GAABB& i_boxA, const GMatrix4& i_boxATransform, const GVector3& i_boxAVel,
		const GAABB& i_boxB, const GMatrix4& i_boxBTransform, const GVector3& i_boxBVel, float i_time, GContact& o_contact )
	{
		// Create matrices for converting from one box to another's coordinate spaces.
		GMatrix4 worldToA, worldToB;
		GMatrix4 objAtoB, objBtoA;

		objAtoB = i_boxBTransform;
		objAtoB.Invert();
		worldToB = objAtoB;
		objAtoB = i_boxATransform * worldToB;

		objBtoA = i_boxATransform;
		objBtoA.Invert();
		worldToA = objBtoA;
		objBtoA = i_boxBTransform * worldToA;

		float lastEnter = 0.0f;
		float firstExit = 1.0f;
		float normal = 1.0f;

		///////////////////// A INTO B /////////////////////////////
		GVector3 velAInB, velBInB;
		GMatrix4::OrientVector3( velAInB, i_boxAVel, worldToB );
		GMatrix4::OrientVector3( velBInB, i_boxBVel, worldToB );

		// new A velocity
		GVector3 velAInBAdjusted = velAInB - velBInB;

		// project A's extents onto B's axises

		// calculating the extents requires a large comment so that i don't forget why this is working later.
		/*
		i_boxA.m_HalfX * objAtoB( 0, 0 ):
		This follows the idea that N_x ( the normal representing positive "x-axis" in A ) times the length scalar dotted with the first column of our transformation matrix gives us the x value in B's coordinate space.
		We don't have to multiply the x and y values of N_x * scalar length, because we know the x and y of N_x are zero.

		A's x-axis is not the only one that affects A's x-length in B's coordinate system.  It's also (very) possible for the values in the Y and X to affect the length of the X:
		i_boxA.m_HalfY * objAtoB( 1, 0 )
		This follows the SAME idea as our X axis, only this time, we know that our x and z values are 0.

		i_boxA.m_HalfZ * objAtoB( 2, 0 )
		This follows the SAME idea as our X axis, only this time, we know that our x and y values are 0.

		The following then calculates the maximum extent length in B's x-axis:
		i_boxA.m_HalfX * objAtoB( 0, 0 ) + i_boxA.m_HalfY * objAtoB( 1, 0 ) + i_boxA.m_HalfZ * objAtoB( 2, 0 )

		We then do this same idea for each of B's other axises...
		*/

		float ExtentAx = fabs(i_boxA.m_HalfX * objAtoB( 0, 0 )) + fabs(i_boxA.m_HalfY * objAtoB( 1, 0 )) + fabs(i_boxA.m_HalfZ * objAtoB( 2, 0 ));
		float ExtentAy = fabs(i_boxA.m_HalfX * objAtoB( 0, 1 )) + fabs(i_boxA.m_HalfY * objAtoB( 1, 1 )) + fabs(i_boxA.m_HalfZ * objAtoB( 2, 1 ));
		float ExtentAz = fabs(i_boxA.m_HalfX * objAtoB( 0, 2 )) + fabs(i_boxA.m_HalfY * objAtoB( 1, 2 )) + fabs(i_boxA.m_HalfZ * objAtoB( 2, 2 ));

		// create a new bloated box B!
		GAABB bloatedB = i_boxB;
		bloatedB.m_Center = i_boxB.m_Center;
		bloatedB.m_HalfX += ExtentAx;
		bloatedB.m_HalfY += ExtentAy;
		bloatedB.m_HalfZ += ExtentAz;

		// get the start position of A. i think this should work?
		GVector3 centerAInB = objAtoB * i_boxA.m_Center;
			
		float enter = 0.0f;
		float exit = 1.0f;
		int axis = 0;
		float firstNormal = 0.0f;

		// check x-axis //
		if( !CheckIfAxisOverlap( bloatedB.m_Center.x() - bloatedB.m_HalfX, bloatedB.m_Center.x() + bloatedB.m_HalfX,
			velAInBAdjusted.x() * i_time, centerAInB.x(), lastEnter, firstExit, normal ) )
			return false;

		firstNormal = normal;

		// check y-axis //
		if( !CheckIfAxisOverlap( bloatedB.m_Center.y() - bloatedB.m_HalfY, bloatedB.m_Center.y() + bloatedB.m_HalfY,
			velAInBAdjusted.y() * i_time, centerAInB.y(), enter, exit, normal ) )
			return false;

		if( enter > lastEnter )
		{
			axis = 1;
			firstNormal = normal;
			lastEnter = enter;
		}

		if( exit < firstExit )
			firstExit = exit;

		// check z-axis //
		if( !CheckIfAxisOverlap( bloatedB.m_Center.z() - bloatedB.m_HalfZ, bloatedB.m_Center.z() + bloatedB.m_HalfZ,
			velAInBAdjusted.z() * i_time, centerAInB.z(), enter, exit, normal ) )
			return false;

		if( enter > lastEnter )
		{
			axis = 2;
			firstNormal = normal;
			lastEnter = enter;
		}

		if( exit < firstExit )
			firstExit = exit;

		//transform the collision normal.
		GVector3 srcNormal = GVector3::Zero;
		if( axis == 0 )
			srcNormal.x( firstNormal );
		else if( axis == 1 )
			srcNormal.y( firstNormal );
		else if( axis == 2 )
			srcNormal.z( firstNormal );

		GMatrix4::OrientVector3( o_contact.m_firstNormal, srcNormal, i_boxBTransform );

		//////////////// B INTO A ///////////////////////
		GVector3 velAInA, velBInA;
		GMatrix4::OrientVector3( velAInA, i_boxAVel, worldToA );
		GMatrix4::OrientVector3( velBInA, i_boxBVel, worldToA );

		// new B velocity
		GVector3 velBInAAdjusted = velBInA - velAInA;

		// project B's extents onto A's axises
		float ExtentBx = fabs(i_boxB.m_HalfX * objBtoA( 0, 0 )) + fabs(i_boxB.m_HalfY * objBtoA( 1, 0 )) + fabs(i_boxB.m_HalfZ * objBtoA( 2, 0 ));
		float ExtentBy = fabs(i_boxB.m_HalfX * objBtoA( 0, 1 )) + fabs(i_boxB.m_HalfY * objBtoA( 1, 1 )) + fabs(i_boxB.m_HalfZ * objBtoA( 2, 1 ));
		float ExtentBz = fabs(i_boxB.m_HalfX * objBtoA( 0, 2 )) + fabs(i_boxB.m_HalfY * objBtoA( 1, 2 )) + fabs(i_boxB.m_HalfZ * objBtoA( 2, 2 ));

		// create a new bloated box A!
		GAABB bloatedA = i_boxA;
		bloatedA.m_Center = i_boxA.m_Center;
		bloatedA.m_HalfX += ExtentBx;
		bloatedA.m_HalfY += ExtentBy;
		bloatedA.m_HalfZ += ExtentBz;

		// get the start position of B.
		GVector3 centerBInA = objBtoA * i_boxB.m_Center ;

		//reset values.
		normal = 1.0f;
		firstNormal = 1.0f;
		enter = 0.0f;
		exit = 1.0f;
		axis = -1;

		// check x-axis //
		if( !CheckIfAxisOverlap( bloatedA.m_Center.x() - bloatedA.m_HalfX, bloatedA.m_Center.x() + bloatedA.m_HalfX,
			velBInAAdjusted.x() * i_time, centerBInA.x(), enter, exit, normal ) )
			return false;

		if( enter > lastEnter )
		{
			firstNormal = normal;
			lastEnter = enter;
			axis = 0;
		}

		if( exit < firstExit )
			firstExit = exit;

		// check y-axis //
		if( !CheckIfAxisOverlap( bloatedA.m_Center.y() - bloatedA.m_HalfY, bloatedA.m_Center.y() + bloatedA.m_HalfY,
			velBInAAdjusted.y() * i_time, centerBInA.y(), enter, exit, normal ) )
			return false;

		if( enter > lastEnter )
		{
			axis = 1;
			firstNormal = normal;
			lastEnter = enter;
		}

		if( exit < firstExit )
			firstExit = exit;

		// check z-axis //
		if( !CheckIfAxisOverlap( bloatedA.m_Center.z() - bloatedA.m_HalfZ, bloatedA.m_Center.z() + bloatedA.m_HalfZ,
			velBInAAdjusted.z() * i_time, centerBInA.z(), enter, exit, normal ) )
			return false;

		if( enter > lastEnter )
		{
			axis = 2;
			firstNormal = normal;
			lastEnter = enter;
		}

		if( exit < firstExit )
			firstExit = exit;

		if( axis != - 1 )
		{
			GVector3 srcNormal = GVector3::Zero;
			if( axis == 0 )
				srcNormal.x( firstNormal );
			else if( axis == 1 )
				srcNormal.y( firstNormal );
			else if( axis == 2 )
				srcNormal.z( firstNormal );

			GMatrix4::OrientVector3( o_contact.m_firstNormal, srcNormal, i_boxATransform );
			// flip this so that it is the corresponing normal for "first"
			o_contact.m_firstNormal = -o_contact.m_firstNormal;
		}

		o_contact.m_time = lastEnter;
		return true;
	}

	// This isn't entirely worked out yet.  for now, just convert the AABB to an OBB and use that...
	bool SAT_OBB_AABB_Check( const GAABB& i_boxA, const GMatrix4& i_boxATransform, const GVector3& i_boxAVel,
		const GAABB& i_boxB, const GVector3 i_boxBPos, const GVector3& i_boxBVel, float i_time, GContact& o_contact )
	{
		/*
		GMatrix4 worldToB;
		worldToB.Identify();
		worldToB.PokeTranslation( -i_boxBPos.x(), -i_boxBPos.y(), -i_boxBPos.z() ); //instead of invert.

		GMatrix4 objAtoB = i_boxATransform * worldToB;
		
		GVector3 velAInBAdjusted = i_boxAVel - i_boxBVel;

		//compute the extents of B in A.
		float ExtentBx = fabs(i_boxA.m_HalfX * objAtoB( 0, 0 )) + fabs(i_boxA.m_HalfY * objAtoB( 1, 0 )) + fabs(i_boxA.m_HalfZ * objAtoB( 2, 0 ));
		float ExtentBy = fabs(i_boxA.m_HalfX * objAtoB( 0, 1 )) + fabs(i_boxA.m_HalfY * objAtoB( 1, 1 )) + fabs(i_boxA.m_HalfZ * objAtoB( 2, 1 ));
		float ExtentBz = fabs(i_boxA.m_HalfX * objAtoB( 0, 2 )) + fabs(i_boxA.m_HalfY * objAtoB( 1, 2 )) + fabs(i_boxA.m_HalfZ * objAtoB( 2, 2 ));

		// create a new bloated box A!
		GAABB bloatedB = i_boxB;
		bloatedB.m_Center = i_boxA.m_Center;
		bloatedB.m_HalfX += ExtentBx;
		bloatedB.m_HalfY += ExtentBy;
		bloatedB.m_HalfZ += ExtentBz;

		//get the center of A in B
		GVector3 centerAInB = objAtoB * i_boxA.m_Center ;

		float normal = 1.0f;

		float lastEnter = -1.0f;
		float firstExit = 1.0f;

		float enter = 0.0f;
		float exit = 1.0f;
		int axis = 0;
		float firstNormal = 0.0f;

		// check x-axis
		if( !CheckIfAxisOverlap( bloatedB.m_Center.x() - bloatedB.m_HalfX, bloatedB.m_Center.x() + bloatedB.m_HalfX,
			velAInBAdjusted.x() * i_time, centerAInB.x(), lastEnter, firstExit, normal ) )
			return false;

		firstNormal = normal;

		if( !CheckIfAxisOverlap( bloatedB.m_Center.y() - bloatedB.m_HalfY, bloatedB.m_Center.y() + bloatedB.m_HalfY,
			velAInBAdjusted.y() * i_time, centerAInB.y(), enter, exit, normal ) )
			return false;

		if( enter > lastEnter )
		{
			axis = 1;
			firstNormal = normal;
			lastEnter = enter;
		}

		if( exit < firstExit )
			firstExit = exit;

		if( !CheckIfAxisOverlap( bloatedB.m_Center.z() - bloatedB.m_HalfZ, bloatedB.m_Center.z() + bloatedB.m_HalfZ,
			velAInBAdjusted.z() * i_time, centerAInB.z(), enter, exit, normal ) )
			return false;

		if( enter > lastEnter )
		{
			axis = 2;
			firstNormal = normal;
			lastEnter = enter;
		}

		if( exit < firstExit )
			firstExit = exit;

		//transform the collision normal.
		GVector3 srcNormal = GVector3::Zero;
		if( axis == 0 )
			srcNormal.x( firstNormal );
		else if( axis == 1 )
			srcNormal.y( firstNormal );
		else if( axis == 2 )
			srcNormal.z( firstNormal );

		o_contact.m_firstNormal = srcNormal;
		o_contact.m_firstNormal = -o_contact.m_firstNormal;
		o_contact.m_time = lastEnter;
		*/

		return false;
	}


	bool SATAABBUnitTests( )
	{
		GContact contact;
		// AABB A is centered at 3,3.  this is just the local position.  It's world position is 0,0.  I want to test this.
		GAABB aabb_A( 4.0f, 4.0f, 2.0f, 2.0f, 2.0f, 2.0f );
		GVector3 aPos( 0.0f, 0.0f, 0.0f );
		GVector3 aVel1( 1.001f, 1.001f, 0.0f );
		GVector3 aVel3( 7.001f, 7.001f, 0.0f );

		// AABB B is centered at 0.5, 0.5.  this is just the local position.  It's world position is 0,0.  Let's test THIS.
		GAABB aabb_B( 1.0f, 1.0f, 2.0f, 0.0f, 0.0f, 2.0f );
		GVector3 bPos( 0.0f, 0.0f, 0.0f );
		GVector3 bVel1( 1.000f, 1.000f, 0.0f );
		GVector3 bVel2( 1.001f, 1.001f, 0.0f );
		GVector3 bVel3( 10.001f, 10.001f, 0.0f );

		// test...
		GAABB aabb_C( 4.0f, 4.0f, 2.0f, 2.0f, 2.0f, 2.0f );
		GVector3 cPos( -1.0f, 0.0f, 0.0f );
		GVector3 cVel( 1.0f, 0.0f, 0.0f );
		bool colliding = SATAABB( aabb_A, aPos, GVector3::Zero, aabb_C, cPos, cVel, 1.0f, contact );

		// let's make sure we're not already colliding.
		colliding = SATAABB( aabb_A, aPos, GVector3::Zero, aabb_B, bPos, GVector3::Zero, 1.0f, contact );
		if ( colliding )
			return false;

		// let's run for 1 second and see if we collide (we will not have crossed the mathematical plane yet, so we will not collide)
		colliding = SATAABB( aabb_A, aPos, GVector3::Zero, aabb_B, bPos, bVel1, 1.0f, contact );
		if( colliding )
			return false;

		// let's run for a little more than one second and see if we collide ( we should ).
		colliding = SATAABB( aabb_A, aPos, GVector3::Zero, aabb_B, bPos, bVel2, 1.0f, contact );
		if( !colliding )
			return false;

		// now let's pass all the way through and detect if we collide.
		colliding = SATAABB( aabb_A, aPos, GVector3::Zero, aabb_B, bPos, bVel3, 1.0f, contact );
		if( !colliding )
			return false;

		////// let's try making box A move away from B.//////

		// let's run for a little more than one second and see if we collide ( we shouldn't now ).
		colliding = SATAABB( aabb_A, aPos, aVel1, aabb_B, bPos, bVel2, 1.0f, contact );
		if( colliding )
			return false;

		// now let's pass all the way through and detect if we collide ( we still should, even though A is moving away faster now.  B will still pass
		// through A )
		colliding = SATAABB( aabb_A, aPos, aVel3, aabb_B, bPos, bVel3, 1.0f, contact );
		if( !colliding )
			return false;


		// OBB SAT CHECK
		GVector3 center( 0.0f, 0.0f, 0.0f );
		GAABB smallSquare( center,  0.5f, 0.5f, 0.0f );
		GAABB rectangle( center, 1.4142f, 0.7072f, 0.0f );

		GVector3 squareOne( 0.5f, 0.5f, 0.0f );
		GVector3 zero( 0.0f, 0.0f, 0.0f );
		GVector3 rect( 1.5f, 1.5f, 0.0f );


		// just before hitting.  at this point, the small square is sitting just under the left side of the rectangle rotated at 135 degrees.
		GMatrix4 squareOneMat, rectMat;
		rectMat.SetRotZ( GMath::Deg2Rad( 135.0f ) );
		rectMat.PokeTranslation( 1.5f, 1.5f, 0.0f );
		squareOneMat.Identify();
		squareOneMat.PokeTranslation( 0.48f, 0.5f, 0.0f );

		if( !SATOBBCheck( smallSquare, squareOneMat, zero, rectangle, rectMat, zero, 1.0f, contact ) )
			return false;

		// beginning intersection
		GVector3 movingRight( 200.0f, 0.0f, 0.0f );
		if( !SATOBBCheck( smallSquare, squareOneMat, movingRight, rectangle, rectMat, zero, 0.001f, contact ) )
			return false;

		// passing all the way through, by a whole lot...
		if( !SATOBBCheck( smallSquare, squareOneMat, movingRight, rectangle, rectMat, zero, 1.0f, contact ) )
			return false;

		return true;
	}


}