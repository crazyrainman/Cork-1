// +-------------------------------------------------------------------------
// | StatsImpl.h
// | 
// | Author: Stephan Friedl
// +-------------------------------------------------------------------------
// | COPYRIGHT:
// |    Copyright Stephan Friedl 2015
// |    See the included COPYRIGHT file for further details.
// |    
// |    This file is part of the Cork library.
// |
// |    Cork is free software: you can redistribute it and/or modify
// |    it under the terms of the GNU Lesser General Public License as
// |    published by the Free Software Foundation, either version 3 of
// |    the License, or (at your option) any later version.
// |
// |    Cork is distributed in the hope that it will be useful,
// |    but WITHOUT ANY WARRANTY; without even the implied warranty of
// |    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// |    GNU Lesser General Public License for more details.
// |
// |    You should have received a copy 
// |    of the GNU Lesser General Public License
// |    along with Cork.  If not, see <http://www.gnu.org/licenses/>.
// +-------------------------------------------------------------------------

#pragma once


#include <array>
#include <list>
#include <unordered_set>
#include <unordered_map>
#include <map>
#include <deque>

#include <boost\container\small_vector.hpp>

#include "..\CorkDefs.h"

#include "..\Statistics.h"



namespace Cork
{
	namespace Statistics
	{


		class GeometricStatisticsEngine
		{
		public :

			enum PropertiesToCompute
			{
				ALL = 0xFFFF,
				AREA_AND_VOLUME = 2,
				EDGE_LENGTHS = 4,
				BOUNDING_BOX = 8
			};


			explicit
			GeometricStatisticsEngine( PropertiesToCompute		propertiesToCompute = ALL )
				: m_propertiesToCompute( propertiesToCompute ),
				  m_numTriangles( 0 ),
				  m_area( 0.0 ),
				  m_volume( 0.0 ),
				  m_minEdgeLength( DBL_MAX ),
				  m_maxEdgeLength( DBL_MIN )
			{}


			void			GeometricStatisticsEngine::AddTriangle( const Cork::Math::TriangleByVerticesBase&		nextTriangle   )
			{
				//	Get the edges

				if(( m_propertiesToCompute & ( PropertiesToCompute::AREA_AND_VOLUME |  PropertiesToCompute::EDGE_LENGTHS )) != 0 )
				{
					Cork::Math::Vector3D		edgeAB = nextTriangle.edgeAB();
					Cork::Math::Vector3D		edgeAC = nextTriangle.edgeAC();
					Cork::Math::Vector3D		edgeBC = nextTriangle.edgeBC();
				
					if(( m_propertiesToCompute & PropertiesToCompute::AREA_AND_VOLUME ) != 0 )
					{
						//	Add the incremental area of this triangle
					
						Cork::Math::Vector3D		ABcrossAC = cross( edgeAB, edgeAC );

						m_area += len( ABcrossAC ) / 2;

						//	Do the same for volume

						double		temp = dot( nextTriangle.vertexA(), ABcrossAC );

						m_volume += temp / 6;
					}

					//	Update the min/max edge lengths

					if(( m_propertiesToCompute & PropertiesToCompute::EDGE_LENGTHS ) != 0 )
					{
						m_minEdgeLength = std::min( m_minEdgeLength, (double)std::min( len(  edgeAB ), std::min( len( edgeAC ), len( edgeBC ) )));
						m_maxEdgeLength = std::max( m_maxEdgeLength, (double)std::max( len(  edgeAB ), std::max( len( edgeAC ), len( edgeBC ) )));
					}
				}

				if(( m_propertiesToCompute & PropertiesToCompute::BOUNDING_BOX ) != 0 )
				{
					m_boundingBox.convex( Cork::Math::BBox3D( min( nextTriangle[0], nextTriangle[1], nextTriangle[2] ), max( nextTriangle[0], nextTriangle[1], nextTriangle[2] ) ));
				}

				//	Increment the triangle count

				m_numTriangles++;
			};


			size_t				numTriangles() const
			{
				return( m_numTriangles );
			}

			const Cork::Math::BBox3D& 		boundingBox() const
			{
				return( m_boundingBox );
			}

			double				area() const
			{
				return( m_area );
			}

			double				volume() const
			{
				return( m_volume );
			}

			double				minEdgeLength() const
			{
				return( m_minEdgeLength );
			}

			double				maxEdgeLength() const
			{
				return( m_maxEdgeLength );
			}



		private :
		
			PropertiesToCompute		m_propertiesToCompute;

			size_t					m_numTriangles;

			Cork::Math::BBox3D		m_boundingBox;

			double					m_area;
			double					m_volume;

			double					m_minEdgeLength;
			double					m_maxEdgeLength;
		};





		class TopologicalStatisticsEngine
		{
		public :

			TopologicalStatisticsEngine( size_t		numTriangles )
				: m_numBodys( 0 ),
				  m_numNon2ManifoldEdges( 0 )
			{
				m_edges.reserve( numTriangles * 6 );
				m_vertexAssociations.reserve( numTriangles * 12 );
			}



			void			TopologicalStatisticsEngine::AddTriangle( const Cork::Math::TriangleByIndicesBase&		nextTriangle   )
			{
				EdgeSet::iterator	itrEdgeAB = m_edges.emplace( nextTriangle.a(), nextTriangle.b() ).first;
				EdgeSet::iterator	itrEdgeAC = m_edges.emplace( nextTriangle.a(), nextTriangle.c() ).first;
				EdgeSet::iterator	itrEdgeBC = m_edges.emplace( nextTriangle.b(), nextTriangle.c() ).first;
				
				int			abIncidences = const_cast<EdgeAndIncidence&>(*itrEdgeAB).AddIncidence();
				int			acIncidences = const_cast<EdgeAndIncidence&>(*itrEdgeAC).AddIncidence();
				int			bcIncidences = const_cast<EdgeAndIncidence&>(*itrEdgeBC).AddIncidence();

				m_vertexAssociations[nextTriangle.a()].push_back( nextTriangle.b() );
				m_vertexAssociations[nextTriangle.a()].push_back( nextTriangle.c() );
				m_vertexAssociations[nextTriangle.b()].push_back( nextTriangle.a() );
				m_vertexAssociations[nextTriangle.b()].push_back( nextTriangle.c() );
				m_vertexAssociations[nextTriangle.c()].push_back( nextTriangle.a() );
				m_vertexAssociations[nextTriangle.c()].push_back( nextTriangle.b() );
			}



			TopologicalStatistics			Analyze()
			{
				for( auto& edge : m_edges )
				{
					if( edge.numIncidences() != 2 )
					{
						m_numNon2ManifoldEdges++;
					}
				}

				std::vector<Cork::Math::VertexIndex>	vertexStack;
				std::vector<int>						vertexBodyMembership( m_vertexAssociations.size(), -1 );

				vertexStack.reserve( m_vertexAssociations.size() );

				int							numBodies = 0;

				while( !m_vertexAssociations.empty() )
				{
					vertexStack.push_back( m_vertexAssociations.begin()->first );

					size_t		currentStackTop = 0;

					while( currentStackTop < vertexStack.size() )
					{
						Cork::Math::VertexIndex					currentVertex = vertexStack[currentStackTop++];

						if( vertexBodyMembership[currentVertex] >= 0 )
						{
							continue;
						}

						AssociatedVertexVector&		associatedVertices = m_vertexAssociations[currentVertex];

						for( Cork::Math::VertexIndex connectedVertex : associatedVertices )
						{
							if( vertexBodyMembership[currentVertex] < 0 )
							{
								vertexStack.push_back( connectedVertex );
							}
						}

						vertexBodyMembership[currentVertex] = numBodies;
						m_vertexAssociations.erase( currentVertex );
					}

					numBodies++;
				}

				std::unique_ptr<Statistics::TopologicalStatistics::EdgeVector>		nonTwoManifoldEdges( new Statistics::TopologicalStatistics::EdgeVector() );

				return( TopologicalStatistics( m_edges.size(), numBodies, nonTwoManifoldEdges ) );
			}


		private :

			class EdgeAndIncidence : public Cork::Math::EdgeBase
			{
			public :

				EdgeAndIncidence( const Cork::Math::VertexIndex		a,
								  const Cork::Math::VertexIndex		b )
					: Cork::Math::EdgeBase( a, b ),
					  m_numIncidences( 0 )
				{}

				virtual ~EdgeAndIncidence()
				{}



				int			AddIncidence()
				{
					return( ++m_numIncidences );
				}


				int				numIncidences() const
				{
					return( m_numIncidences );
				}


				struct HashFunction
				{
					std::size_t operator()(const Cork::Math::EdgeBase& k) const
					{
					return ( k.vertexA() * 10000019 ^ k.vertexB() );
					}
				};


			private :

				int			m_numIncidences;
			};



			typedef boost::container::small_vector<Cork::Math::VertexIndex,100>					AssociatedVertexVector;

			typedef std::unordered_set<EdgeAndIncidence, EdgeAndIncidence::HashFunction>		EdgeSet;
			typedef std::unordered_map<Cork::Math::VertexIndex,AssociatedVertexVector>			VertexAssociations;


			int						m_numBodys;
			int						m_numNon2ManifoldEdges;

			EdgeSet					m_edges;

			VertexAssociations		m_vertexAssociations;
		};

	}

}



