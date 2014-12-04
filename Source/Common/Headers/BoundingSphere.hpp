#ifndef __LD_BOUNDINGSPHERE_HPP__
#define __LD_BOUNDINGSPHERE_HPP__

#include <DataTypes.hpp>
#include <Vector4.hpp>

namespace LD
{
	class Matrix4x4;
	class Renderer;

	class BoundingSphere
	{
	public:
		BoundingSphere( );
		~BoundingSphere( );

		void SetPosition( const Vector4 &p_Position );
		void SetRadius( const LD_FLOAT32 p_Radius );
		void SetRenderDivisions( const LD_UINT32 p_Vertical,
			const LD_UINT32 p_Horizontal );

		LD_UINT32 GetWorldMatrix( Matrix4x4 &p_Matrix );

		LD_UINT32 Render( Renderer &p_PolygonCacheManager );

	private:
		LD_UINT32	m_PolygonCacheID;
		LD_FLOAT32	m_Radius;
		Vector4		m_Position;
		LD_UINT32	m_VerticalDivisions;
		LD_UINT32	m_HorizontalDivisions;
	};
}

#endif // __LD_BOUNDINGSPHERE_HPP__

