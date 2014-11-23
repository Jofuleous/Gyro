#ifndef __GTRANSFORM_H_
#define __GTRANSFORM_H_

#include "GVector3.h"
#include "GQuat.h"
#include "GMatrix4.h"

class GTransform
{
public:
	GQuat		m_Rotation;
	GVector3	m_Translation;
	GVector3	m_Scale;

	inline GTransform()
		:	m_Rotation( 0.f, 0.f, 0.f, 1.f ),
			m_Translation(1.f, 1.f, 1.f),
			m_Scale( 1.f, 1.f, 1.f )
	{
	}

	inline GTransform(const GVector3& i_Translation)
		:	m_Rotation(GQuat::Identity),
			m_Translation(i_Translation),
			m_Scale(1.f)
	{
	}

	inline GTransform( const GQuat& i_Rot )
		:	m_Rotation( i_Rot ),
			m_Translation( 0.f ),
			m_Scale( 1.f )
	{
	}

	inline void GTransform::SetIdentity( )
	{
		m_Rotation = GQuat::Identity;
		m_Translation = GVector3::Zero;
		m_Scale = GVector3::One;
	}

	GQuat GTransform::GetRotation( )
	{
		return m_Rotation;
	}

	inline void GTransform::SetRotation( const GQuat& i_rot )
	{
		m_Rotation = i_rot;
	}

	inline void GTransform::ConcatRotation( const GQuat& i_rot )
	{
		m_Rotation *= i_rot;
	}

	GVector3 GTransform::GetTranslation()
	{
		return m_Translation;
	}

	inline void GTransform::SetTranslation( const GVector3& i_trans )
	{
		m_Translation = i_trans;
	}

	inline void GTransform::AddToTranslation( const GVector3& i_trans )
	{
		m_Translation += i_trans;
	}

	inline void GTransform::ScaleTranslation( float i_scale )
	{
		m_Translation *= i_scale;
	}

	GVector3 GTransform::GetScale()
	{
		return m_Scale;
	}

	inline void GTransform::SetScale( const GVector3& i_scale )
	{
		m_Scale= i_scale;
	}

	inline void GTransform::MultiplyScale3D( const GVector3& i_scale )
	{
		m_Scale *= i_scale;
	}

	inline void GTransform::MultiplyScale( float i_scale )
	{
		m_Scale *= i_scale;
	}

	inline void GTransform::Multiply( GTransform* o_transform, const GTransform* A, const GTransform* B )
	{
		o_transform->m_Rotation = B->m_Rotation * A->m_Rotation; // make sure order is correct...
		o_transform->m_Scale = A->m_Scale * B->m_Scale;
		o_transform->m_Translation = A->m_Translation + B->m_Translation;
	}

	inline GTransform GTransform::operator*( const GTransform& i_transform )
	{
		GTransform output;
		Multiply(&output, this, &i_transform);
		return output;
	}

	void GTransform::operator*=( const GTransform& i_transform )
	{
		Multiply(this, this, &i_transform);
	}

	GVector3 GTransform::TransformVector( const GVector3& i_vector )
	{
		return m_Rotation * ( m_Scale * i_vector );
	}

	GVector3 GTransform::TransformVectorNoScale(const GVector3& i_vector)
	{
		return m_Rotation * i_vector;
	}

	GVector3 GTransform::TransformPosition(const GVector3& i_vector)
	{
		GVector3 transformed = m_Rotation * (m_Scale * i_vector);
		transformed += i_vector;
		return transformed;
	}

	GVector3 GTransform::InverseTransformPosition(const GVector3& i_vector)
	{
		return m_Rotation.Inverse() * (i_vector - m_Translation) * GVector3::SafeReciprocal( i_vector );
	}

	GVector3 GTransform::InverseTransformPositionNoScale(const GVector3& i_vector)
	{
		return m_Rotation.Inverse() * (i_vector - m_Translation);
	}

	GVector3 GTransform::InverseTransformVector(const GVector3& i_vector)
	{
		return (m_Rotation.Inverse() * i_vector) * m_Scale;
	}

	GVector3 GTransform::InverseTransformVectorNoScale(const GVector3& i_vector )
	{
		return m_Rotation.Inverse() * i_vector;
	}
};

#endif