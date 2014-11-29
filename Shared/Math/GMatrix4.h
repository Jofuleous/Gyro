/*****************
  This could still use a lot of optimization + organization.
  todo: Get rid of for loops and the array later.  just use float members.
******************/

#ifndef __GMATRIX4_H_
#define __GMATRIX4_H_

//#define TEMP_MATRIX

#include "GVector3.h"


// MATRIX LAYOUT
// ROW - MAJOR
/*
--------------------------------------
Left_x | Left_y | Left_z
--------------------------------------
Up_x   | Up_y   | Up_z
--------------------------------------
For_x  | For_y  | For_z
--------------------------------------
Trans_x|Trans_y | Trans_z
--------------------------------------

Explanation on Row vs Column Major:
http://www.scratchapixel.com/lessons/3d-basic-lessons/lesson-4-geometry/conventions-again-row-major-vs-column-major-vector/

*/

class GMatrix4
{
public:
	GMatrix4( bool identityhack );
	GMatrix4(){}

	~GMatrix4(){}

	inline void					Concatenate( const GMatrix4& i_other );
	inline void					PokeTranslation( const GVector3& i_other );
	inline void					PokeTranslation( float i_x, float i_y, float i_z );
	inline void					SetTranslation( const GVector3& i_other );
	inline void					SetTranslation( float i_x, float i_y, float i_z );
	inline void					PokeScale( const GVector3& i_scale );
	inline void					PokeScale( float i_x, float i_y, float i_z );
	inline void					SetScale( const GVector3& i_scale );
	// inline void				SetScale( float i_x, float i_y, float i_z );
	inline GVector3				GetTranslation( void ) const ;
	inline void					SetRotX( float i_radians );
	inline void					SetRotY( float i_radians );
	inline void					SetRotZ( float i_radians );
	inline void					Identify( void );
	inline void					ToD3DX( void* o_d3dmatrix );
	inline GVector3				Left( ) const { return GVector3( m_elements[0][0], m_elements[0][1], m_elements[0][2] ); }
	inline GVector3				Dir( ) const { return GVector3( m_elements[2][0], m_elements[2][1], m_elements[2][2] ); }
	inline GVector3				Up( ) const { return GVector3( m_elements[1][0], m_elements[1][1], m_elements[1][2] ); }

	inline void					Invert( void );
	/*inline*/ void				Inverse( GMatrix4& o_dest ) const ;


	static void					OrientVector3( GVector3& o_dest, const GVector3& i_src, const GMatrix4& i_transform );

	inline float				operator() (unsigned i_row, unsigned i_col) const;
	inline float&				operator() (unsigned i_row, unsigned i_col);

#ifdef _DEBUG
	inline void		Debug_PrintMatrixToConsole( void );
#endif

#if defined(_DEBUG) && defined(_WIN32)
	static void		Debug_PrintD3DMatrixToConsole(void* i_matrix);
#endif 

	float m_elements[4][4];
	static const GMatrix4& Identity; 

//until i've 100% confirmed my matrix class is perfect...
#if defined( TEMP_MATRIX ) && defined( _DEBUG ) && defined( _WIN32 )
D3DXMATRIX			m_testMatrix;
inline bool			ConfirmEqual();
#endif

};

#include "GMatrix4.inl"

#endif