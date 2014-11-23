#include <assert.h>
#include <math.h>

namespace GMath
{

	bool IsNAN( float i_val )
	{
		volatile float val = i_val;

		return val != val;
	}

	bool IsExactlyZero( float i_val )
	{
		if( i_val == 0.0f || i_val == -0.0f )
			return true;
		return false;
	}

	//Float compare.
	//http://www.cygnus-software.com/papers/comparingfloats/comparingfloats.htm
	/*float			HEX				DEC
	+1.99999976		0x3FFFFFFE		1073741822
	+1.99999988		0x3FFFFFFF		1073741823
	+2.00000000		0x40000000		1073741824
	2.00000024		0x40000001		1073741825
	2.00000048		0x40000002		1073741826
	*/
	bool AlmostEqual( float A, float B, int maxUlps = 10)
	{
		// Make sure maxUlps is non-negative and small enough that the
		// default NAN won't compare as equal to anything.
		assert(maxUlps > 0 && maxUlps < 4 * 1024 * 1024);
		int aInt = *(int*)&A;
		// Make aInt lexicographically ordered as a twos-complement int
		if (aInt < 0)
			aInt = 0x80000000 - aInt;
		// Make bInt lexicographically ordered as a twos-complement int
		int bInt = *(int*)&B;
		if (bInt < 0)
			bInt = 0x80000000 - bInt;
		int intDiff = abs(aInt - bInt);
		if (intDiff <= maxUlps)
			return true;
		return false;
	}

	bool GoodEnough( float A, float B )
	{
		if( (A >= 0 && B >= 0) || (A <= 0 && B <= 0))
		{
			float result = abs(A) - abs(B);
			if( result < 0.0000001 )
				return true;
		}
		return false;
	}

#ifdef _DEBUG
	bool AlmostEqual_UnitTest( void )
	{
		float a = 1.1f;
		float c = a * 0.2f;
		float d = a / 5.0f;

		//1 ULP gives maximum precision using this method.
		assert(GMath::AlmostEqual(c, d, 1));
		assert(GMath::AlmostEqual(c, d));

		//Since this is engine uses meters as distance/velocity unit, i doubt we'll go over 1 million meters in any direction.
		//...for now!
		float large = 1000000;
		float larger = 1000001;
		assert(!GMath::AlmostEqual(large, larger));

		//this number is "equal", but the one above is not...
		large = 10000000;
		larger = 10000001;
		assert(GMath::AlmostEqual(large, larger));

		a = (float)0.000000000001;
		c = (float)0.00000000000001;
		assert(!GMath::AlmostEqual(a, c));

		a = 0.0001f;
		c = 0.0002f;
		assert(!GMath::AlmostEqual(a, c, 1));

		return true;
	}
#endif
}