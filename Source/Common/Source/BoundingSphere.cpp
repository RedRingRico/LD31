#include <BoundingSphere.hpp>
#include <Matrix4x4.hpp>
#include <Renderer.hpp>
#include <Memory.hpp>
#include <Maths.hpp>
#include <iostream>

namespace LD
{
	BoundingSphere::BoundingSphere( ) :
		m_PolygonCacheID( 0 ),
		m_Radius( 1.0f ),
		m_Position( Vector4( 0.0f, 0.0f, 0.0f, 0.0f ) ),
		m_VerticalDivisions( 4 ),
		m_HorizontalDivisions( 4 )
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
		// Ideally, the sphere would be geodesic, but this will do
		if( m_PolygonCacheID == 0 )
		{
			LD_MEMSIZE VertexCount = ( m_VerticalDivisions *
				( m_HorizontalDivisions - 1 ) ) + 2;

			// There should be 2 triangles per section (hence 2 * 3 edges)
			// Also, there's a fenceposting issue, which is why the vertical
			// divisions count is decremented by one
			LD_MEMSIZE IndexCount = ( m_VerticalDivisions *
				( m_HorizontalDivisions - 1 ) + 2 ) * 2 * 3;

			VERTEX_POSITION *pVertices = new VERTEX_POSITION[ VertexCount ];
			LD_UINT16 *pIndices = new LD_UINT16[ IndexCount ];

			pVertices[ 0 ].X = pVertices[ 0 ].Z = 0.0f;
			pVertices[ 0 ].Y = 1.0f;
			
			for( LD_MEMSIZE Latitude = 0; Latitude < m_HorizontalDivisions - 1;
				++Latitude )
			{
				LD_FLOAT32 LaSin, LaCos;

				SineCosine( LD_PI *
					( static_cast< LD_FLOAT32 >( Latitude + 1 ) /
					static_cast< LD_FLOAT32 >( m_HorizontalDivisions ) ),
					LaSin, LaCos );

				for( LD_MEMSIZE Longitude = 0; Longitude < m_VerticalDivisions;
					++Longitude )
				{
					LD_FLOAT32 LoSin, LoCos;
					SineCosine( LD_PI * (
						static_cast< LD_FLOAT32 >( Longitude ) /
						static_cast< LD_FLOAT32 >( m_VerticalDivisions ) ),
						LoSin, LoCos );

					LD_MEMSIZE Index = ( Latitude * m_HorizontalDivisions ) +
						Longitude;

					pVertices[ Index + 1 ].X = LaSin * LoCos;
					pVertices[ Index + 1 ].Y = LaCos;
					pVertices[ Index + 1 ].Z = LaSin * LoSin;

					std::cout << "Vertex " << Index + 1 << ": < " <<
						pVertices[ Index + 1 ].X << " " <<
						pVertices[ Index + 1 ].Y << " " <<
						pVertices[ Index + 1 ].Z << std::endl;
				}
			}

			pVertices[ VertexCount - 1 ].X = pVertices[ VertexCount - 1 ].Z =
				0.0f;
			pVertices[ VertexCount - 1 ].Y = -1.0f;
			
			// Top of the sphere
			std::cout << "Top indices:" << std::endl;
			for( LD_MEMSIZE i = 0; i < m_VerticalDivisions; ++i )
			{
				pIndices[ ( i * 3 ) ] = 0;
				pIndices[ ( i * 3 ) + 1 ] = i + 2;
				pIndices[ ( i * 3 ) + 2 ] = i + 1;

				std::cout << "Triangle " << i << ": " <<
					pIndices[ ( i * 3 ) ] << " " <<
					pIndices[ ( i * 3 ) + 1 ] << " " <<
					pIndices[ ( i * 3 ) + 2 ] << std::endl;
			}

			LD_MEMSIZE IndexStart = m_VerticalDivisions * 3;

			std::cout << "Middle starting index: " << IndexStart << std::endl;

			LD_MEMSIZE TriangleIndex = m_VerticalDivisions;
			
			// Middle of the sphere
			for( LD_MEMSIZE i = 0; i < m_VerticalDivisions - 2; ++i )
			{
				std::cout << "Circlet " << i << std::endl;
				for( LD_MEMSIZE j = 0; j < m_HorizontalDivisions; ++j )
				{
					pIndices[ IndexStart + ( m_VerticalDivisions * 6 ) ] =
						( m_VerticalDivisions * i ) + 1 + j;
					pIndices[ IndexStart + ( m_VerticalDivisions * 6 ) + 1 ] =
						( m_VerticalDivisions * i ) + 2 + j;
					pIndices[ IndexStart + ( m_VerticalDivisions * 6 ) + 2 ] =
						m_VerticalDivisions +
							( m_VerticalDivisions * i ) + 1 + j;

					std::cout << "Triangle " << TriangleIndex << ": " <<
						pIndices[ IndexStart + ( m_VerticalDivisions * 6 ) ] <<
						" " << pIndices[ IndexStart +
							( m_VerticalDivisions * 6 ) + 1 ] << " " <<
						pIndices[ IndexStart +
							( m_VerticalDivisions * 6 ) + 2 ] << std::endl;

					pIndices[ IndexStart + ( m_VerticalDivisions * 6 ) + 3 ] =
						( m_VerticalDivisions * i ) + 2 + j;
					pIndices[ IndexStart + ( m_VerticalDivisions * 6 ) + 4 ] =
						m_VerticalDivisions  +
							( m_VerticalDivisions * i ) + 2 + j;
					pIndices[ IndexStart + ( m_VerticalDivisions * 6 ) + 5 ] =
						m_VerticalDivisions +
							( m_VerticalDivisions * i ) + 1 + j;

					std::cout << "Triangle " << ++TriangleIndex << ": " <<
						pIndices[ IndexStart +
							( m_VerticalDivisions * 6 ) + 3 ] << " " <<
						pIndices[ IndexStart +
							( m_VerticalDivisions * 6 ) + 4 ] << " " <<
						pIndices[ IndexStart +
							( m_VerticalDivisions * 6 ) + 5 ] <<std::endl;

					++TriangleIndex;
				}
			}

			// Bottom of the sphere
			std::cout << "Bottom indices:" << std::endl;
			for( LD_MEMSIZE i = 0; i < m_VerticalDivisions; ++i )
			{
				pIndices[ ( i * 3 ) + ( TriangleIndex ) ] =
					VertexCount - 1;
				pIndices[ ( i * 3 ) + ( TriangleIndex ) + 1] =
					VertexCount - m_VerticalDivisions - 1;
				pIndices[ ( i * 3 ) + ( TriangleIndex ) + 2 ] =
					VertexCount - m_VerticalDivisions;

				std::cout << "Triangle " << TriangleIndex << ": " <<
					pIndices[ ( i * 3 ) + ( TriangleIndex ) ] << " " <<
					pIndices[ ( i * 3 ) + 1 + ( TriangleIndex ) ] << " " <<
					pIndices[ ( i * 3 ) + 2 + ( TriangleIndex ) ] << std::endl;

				++TriangleIndex;
			}

			std::cout << "Used " << TriangleIndex * 3 << " indices" <<
				std::endl;
			std::cout << "Indices: " << IndexCount << std::endl;

			p_Renderer.RegisterPolygons( VertexCount, IndexCount,
				reinterpret_cast< LD_BYTE * >( pVertices ), pIndices,
				GL_TRIANGLES, 0x6, m_PolygonCacheID );

			SafeDeleteArray< VERTEX_POSITION >( pVertices );
			SafeDeleteArray< LD_UINT16 >( pIndices );
		}

		p_Renderer.RenderPolygons( m_PolygonCacheID );

		return LD_OK;
	}
}

