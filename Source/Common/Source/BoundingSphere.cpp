#include <BoundingSphere.hpp>
#include <Matrix4x4.hpp>
#include <Renderer.hpp>
#include <Memory.hpp>
#include <Maths.hpp>

namespace LD
{
	BoundingSphere::BoundingSphere( ) :
		m_PolygonCacheID( 0 ),
		m_Radius( 1.0f ),
		m_Position( Vector4( 0.0f, 0.0f, 0.0f, 0.0f ) ),
		m_VerticalDivisions( 8 ),
		m_HorizontalDivisions( 8 )
	{
	}

	BoundingSphere::~BoundingSphere( )
	{
	}

	void BoundingSphere::SetPosition( const Vector4 &p_Position )
	{
		m_Position = p_Position;
	}

	void BoundingSphere::SetRadius( const LD_FLOAT32 p_Radius )
	{
		m_Radius = p_Radius;
	}

	void BoundingSphere::SetRenderDivisions( const LD_UINT32 p_Vertical,
		const LD_UINT32 p_Horizontal )
	{
		m_VerticalDivisions = p_Vertical;
		m_HorizontalDivisions = p_Horizontal;
	}

	LD_UINT32 BoundingSphere::GetWorldMatrix( Matrix4x4 &p_Matrix )
	{
		Matrix4x4 Scale;
		Matrix4x4 Position;

		Scale.Scale( m_Radius );
		Position.Translate( m_Position );

		p_Matrix = Scale * Position;

		return LD_OK;
	}

	LD_UINT32 BoundingSphere::Render( Renderer &p_Renderer )
	{
		if( m_PolygonCacheID == 0 )
		{
			LD_MEMSIZE VertexCount = m_VerticalDivisions *
				m_HorizontalDivisions;

			// There should be 2 triangles per section (hence 2 * 3 edges)
			// Also, there's a fenceposting issue, which is why the vertical
			// divisions count is decremented by one
			LD_MEMSIZE IndexCount = ( m_VerticalDivisions - 1 ) *
				m_HorizontalDivisions * 2 * 3;

			VERTEX_POSITION *pVertices = new VERTEX_POSITION[ VertexCount ];
			LD_UINT16 *pIndices = new LD_UINT16[ IndexCount ];

			for( LD_MEMSIZE Latitude = 0; Latitude < m_HorizontalDivisions;
				++Latitude )
			{
				LD_FLOAT32 LaSin, LaCos;

				SineCosine( LD_PI * static_cast< LD_FLOAT32 >( Latitude /
					m_HorizontalDivisions ), LaSin, LaCos );

				for( LD_MEMSIZE Longitude = 0; Longitude < m_VerticalDivisions;
					++Longitude )
				{
					LD_FLOAT32 LoSin, LoCos;
					SineCosine( LD_PI * static_cast< LD_FLOAT32 >(
						Longitude / m_VerticalDivisions ),
						LoSin, LoCos );

					pVertices[ Latitude + Longitude ].X = LaSin * LoCos;
					pVertices[ Latitude + Longitude ].Y = LaSin * LoSin;
					pVertices[ Latitude + Longitude ].Z = LaCos;
				}
			}

			p_Renderer.RegisterPolygons( VertexCount, IndexCount,
				reinterpret_cast< LD_BYTE * >( pVertices ), pIndices,
				GL_TRIANGLES, 0x6, m_PolygonCacheID );

			SafeDeleteArray< VERTEX_POSITION >( pVertices );
			SafeDeleteArray< LD_UINT16 >( pIndices );			
		}
		else
		{
			p_Renderer.RenderPolygons( m_PolygonCacheID );
		}

		return LD_OK;
	}
}

