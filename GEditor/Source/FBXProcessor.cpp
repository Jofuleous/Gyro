#include "FBXProcessor.h"
#include "Utility/GHashedString.h"

static int meshesProcessed = 0;
static const int MAX_BATCH_SIZE = 10000;

namespace FBXProcessor
{
	void ProcessCollisionNodes( FbxNode* i_node, GOctree< GMathUtil::ColTriangle >& o_colTree )
	{
		int count = i_node->GetChildCount();
		for( int i = 0; i < count; i++ )
		{
			FbxNode* child = i_node->GetChild( i );

			ProcessCollisionNodes( child, o_colTree );
		}

		if( !i_node || !i_node->GetNodeAttribute() )
			return;

		FbxNodeAttribute::EType AttributeType = i_node->GetNodeAttribute()->GetAttributeType();

		if(AttributeType == FbxNodeAttribute::eMesh)
		{
			FbxMesh* pMesh = (FbxMesh*) i_node->GetNodeAttribute();

			FbxGeometryConverter geoConverter( i_node->GetFbxManager() );
			pMesh = (FbxMesh*) geoConverter.Triangulate(pMesh, true);
			FbxVector4* pVertices = pMesh->GetControlPoints();

			int polyCount = pMesh->GetPolygonCount();
			for( int p = 0; p < polyCount; p++ )
			{
				int size = pMesh->GetPolygonSize( p );
				GMathUtil::ColTriangle triangle;
				GVector3 triangleCenter;
				float maxX, maxY, maxZ;
				float minX, minY, minZ;
				maxX = maxY = maxZ = -1000000.0f;
				minX = minY = minZ = 1000000.0f;

				// there's probably a faster way of doing this than creating an AABB...
				for( int v = 0; v < size; v++ )
				{
					int vertexIndex = pMesh->GetPolygonVertex( p, 2-v );
					GVector3 vertex( (float)pVertices[vertexIndex][0], (float)pVertices[vertexIndex][1], (float)-pVertices[vertexIndex][2] );
					switch ( 2-v )
					{
					case 2:
						triangle.m_zero = vertex;
					break;
					case 1:
						triangle.m_one = vertex;
					break;
					case 0:
						triangle.m_two = vertex;
					break;
					}
				}

				// i feel really dirty now....
				o_colTree.AddObject( triangle ); 
			}
		}
	}


	bool ProcessGeometryAndCollision( FbxNode* i_node, GHashMap<u32, BatchedMesh*>& i_meshes, GOctree< GMathUtil::ColTriangle >& o_colTree )
	{
		int count = i_node->GetChildCount();
		for( int i = 0; i < count; i++ )
		{
			FbxNode* child = i_node->GetChild( i );
			if( strcmp( child->GetName(), "Collision" ) == 0 )
			{
				ProcessCollisionNodes( child, o_colTree );
				continue;
			}
			else
				ProcessGeometryAndCollision( child, i_meshes, o_colTree );
		}

		if( !i_node || !i_node->GetNodeAttribute() )
			return false;

		FbxNodeAttribute::EType AttributeType = i_node->GetNodeAttribute()->GetAttributeType();

		if(AttributeType == FbxNodeAttribute::eMesh)
		{
			FbxMesh* pMesh = (FbxMesh*) i_node->GetNodeAttribute();

			FbxGeometryConverter geoConverter( i_node->GetFbxManager() );
			pMesh = (FbxMesh*)geoConverter.Triangulate(pMesh, true);

			FbxVector4* pVertices = pMesh->GetControlPoints();
			int vertexCount = pMesh->GetControlPointsCount();
			meshesProcessed++;

			if( pMesh )
			{

				// add the vertex to the batched mesh...lol.
				// eventually, we'll want a way of importing things that we know are not just scene geometry...
				// and intelligent partitioning of material meshes...
				int l;
				BatchedMesh* bMesh;
				const char* matName;

				// should be 1.
				int materialCount = i_node->GetMaterialCount();
				FbxSurfaceMaterial* mat = i_node->GetMaterial( 0 );
				assert( mat ); // No material associated with the node...something really wrong?
				if( mat )
					matName = mat->GetName();

				u32 hash = GHashedString::Hash( matName, strlen( matName ) );
				GHashNode<u32, BatchedMesh*>* node = i_meshes[ hash ];

				// for creating a unique entity id that will be used on the game side to identify this mesh.
				int deep = 0;

				if( node )
				{
					bMesh = node->m_data;
					while( bMesh->m_vertices.size() > MAX_BATCH_SIZE )
					{

						BatchedMesh* prev = bMesh;
						bMesh = bMesh->m_next;
						deep++;

						if( !bMesh )
						{
							bMesh = new BatchedMesh();
							strcpy_s( bMesh->m_materialName, 64, matName );
							sprintf( bMesh->m_meshName, "%s%d", matName, meshesProcessed );
							prev->m_next = bMesh;
							break;
						}
					}
				}
				else
				{
					bMesh = new BatchedMesh();
					strcpy_s( bMesh->m_materialName, 64, matName );
					sprintf( bMesh->m_meshName, "%s%d", matName, meshesProcessed );

					i_meshes.Set( hash, bMesh );
					node = i_meshes[ hash ];
				}

				int vertexId = 0; // this is being used, but i'm not too sure about it...
				int polyCount = pMesh->GetPolygonCount();

				//fill up the vertex buffer...
				int vertexCount = pMesh->GetControlPointsCount();
				int indexOffset = bMesh->m_vertices.size();

				//node->m_data.m_vertices.reserve( vertexCount );
				for( int q = 0; q < vertexCount; q++ )
				{
					FbxVector4 vertex = pMesh->GetControlPointAt( q );
					GVertices::s_vertexTNTB theVertex;
					// Assuming that there is no transformation/deformation data...
					theVertex.x = (float)vertex[0];
					theVertex.y = (float)vertex[1];
					theVertex.z = (float)-vertex[2]; // flip?
					bMesh->m_vertices.push_back( theVertex );
				}

				for( int i = 0; i < polyCount; i++ )
				{
					int size = pMesh->GetPolygonSize( i );
					for( int j = 0; j < size; j++ )
					{
						int vertexIndex = pMesh->GetPolygonVertex( i, 2-j );
						FbxVector4 vertexPosition = pVertices[vertexIndex];

						GVertices::s_vertexTNTB& theVertex = bMesh->m_vertices[vertexIndex + indexOffset];

						//now get the UV coords.
						for (l = 0; l < pMesh->GetElementUVCount(); l++)
						{
							int count2 = pMesh->GetElementUVCount();
							FbxGeometryElementUV* leUV = pMesh->GetElementUV( l); // LE UV!  ...

							switch (leUV->GetMappingMode())
							{
							case FbxGeometryElement::eByControlPoint:
								switch ( leUV->GetReferenceMode() )
								{

									case FbxGeometryElement::eDirect:
									{
										FbxVector2 v2 = leUV->GetDirectArray().GetAt( vertexIndex );
										theVertex.u = (float)v2[0];
										theVertex.v = 1.0f - (float)v2[1];
										break;
									}

									case FbxGeometryElement::eIndexToDirect:
									{
										int id = leUV->GetIndexArray().GetAt( vertexIndex );
										FbxVector2 v2 = leUV->GetDirectArray().GetAt(id);
										theVertex.u = (float)v2[0];
										theVertex.v = 1.0f - (float)v2[1];
										break;
									}

									default:
										break; // other reference modes not shown here!
								}
								break;

								case FbxGeometryElement::eByPolygonVertex:
								{
									int lTextureUVIndex = pMesh->GetTextureUVIndex(i, 2-j);
									switch (leUV->GetReferenceMode())
									{
										case FbxGeometryElement::eDirect:
										case FbxGeometryElement::eIndexToDirect:
										{
											FbxVector2 v2 = leUV->GetDirectArray().GetAt(lTextureUVIndex);
											theVertex.u = (float)v2[1];
											theVertex.v = (float)v2[0];
											break;
										}
										default:
											break; // other reference modes not shown here!
									}
									break;
								}

								case FbxGeometryElement::eByPolygon: // doesn't make much sense for UVs
								case FbxGeometryElement::eAllSame:   // doesn't make much sense for UVs
								case FbxGeometryElement::eNone:       // doesn't make much sense for UVs
								break;
							}
						}

						// now get the normals...
						for( l = 0; l < pMesh->GetElementNormalCount(); l++ )
						{
							FbxGeometryElementNormal* leNormal = pMesh->GetElementNormal( l); // LE NORMAL.  LE SIGH...
							int count2 = pMesh->GetElementNormalCount();

							//if(leNormal->GetMappingMode() == FbxGeometryElement::eByControlPoint)
							{
								FbxVector4 v4;
								switch( leNormal->GetReferenceMode() )
								{
									case FbxGeometryElement::eDirect:
									{
										v4 = leNormal->GetDirectArray().GetAt( vertexIndex );
										break;
									}
									case FbxGeometryElement::eIndexToDirect:
									{
										int id = leNormal->GetIndexArray().GetAt( vertexIndex );
										v4 = leNormal->GetDirectArray().GetAt(id);
										break;
									}
									default:
										break; // other reference modes not shown here!
								}

								theVertex.nx = (float)v4[0];
								theVertex.ny = (float)v4[1];
								theVertex.nz = (float)-v4[2];
							}
						}

						// get the tangents...
						for( l = 0; l < pMesh->GetElementTangentCount(); l++)
						{
							FbxGeometryElementTangent* leTangent = pMesh->GetElementTangent( l);
							int count2 = pMesh->GetElementTangentCount();

							//if(leTangent->GetMappingMode() == FbxGeometryElement::eByControlPoint)
							{
								FbxVector4 v4;
								switch (leTangent->GetReferenceMode())
								{
									case FbxGeometryElement::eDirect:
									{
										v4 = leTangent->GetDirectArray().GetAt(vertexIndex);
										break;
									}
									case FbxGeometryElement::eIndexToDirect:
									{
										int id = leTangent->GetIndexArray().GetAt(vertexIndex);
										v4 = leTangent->GetDirectArray().GetAt(id);
									}
									default:
										break; // other reference modes not shown here!
								}

								theVertex.tx = (float)v4[0];
								theVertex.ty = (float)v4[1];
								theVertex.tz = (float)-v4[2];
							}
						}

						// get the binormals...
						for( l = 0; l < pMesh->GetElementBinormalCount(); l++)
						{
							FbxGeometryElementBinormal* leBinormal = pMesh->GetElementBinormal( l);
							int count2 = pMesh->GetElementBinormalCount();

							//if( leBinormal->GetMappingMode() == FbxGeometryElement::eByControlPoint )
							{
								FbxVector4 v4;
								switch (leBinormal->GetReferenceMode())
								{
									case FbxGeometryElement::eDirect:
									{
										v4 = leBinormal->GetDirectArray().GetAt( vertexIndex );
										break;
									}
									case FbxGeometryElement::eIndexToDirect:
									{
										int id = leBinormal->GetIndexArray().GetAt( vertexIndex );
										v4 = leBinormal->GetDirectArray().GetAt( id );
										break;
									}
									default:
										break; // other reference modes not shown here!
								}

								theVertex.bx = (float)-v4[0];
								theVertex.by = (float)-v4[1];
								theVertex.bz = (float)v4[2];
							}
						}

						// increment vertexId... still not entirely sure what this is doing...
						vertexId++;

						bMesh->m_indices.push_back( vertexIndex + indexOffset );
					}
				}
			}
		}

		return true;
	}

	bool ProcessGeometry( FbxNode* i_node, GHashMap<u32, BatchedMesh*>& i_meshes )
	{
		int count = i_node->GetChildCount();
		for( int i = 0; i < count; i++ )
		{
			FbxNode* child = i_node->GetChild( i );
			if( strcmp( child->GetName(), "Collision" ) == 0 )
				continue;
			else
				ProcessGeometry( child, i_meshes );
		}

		if( !i_node || !i_node->GetNodeAttribute() )
			return false;

		FbxNodeAttribute::EType AttributeType = i_node->GetNodeAttribute()->GetAttributeType();

		if(AttributeType == FbxNodeAttribute::eMesh)
		{
			FbxMesh* pMesh = (FbxMesh*) i_node->GetNodeAttribute();

			FbxGeometryConverter geoConverter( i_node->GetFbxManager() );
			pMesh = (FbxMesh*)geoConverter.Triangulate(pMesh, true);

			FbxVector4* pVertices = pMesh->GetControlPoints();
			int vertexCount = pMesh->GetControlPointsCount();
			meshesProcessed++;

			if( pMesh )
			{

				// add the vertex to the batched mesh...lol.
				// eventually, we'll want a way of importing things that we know are not just scene geometry...
				// and intelligent partitioning of material meshes...
				int l;
				BatchedMesh* bMesh;
				const char* matName;

				// should be 1.
				int materialCount = i_node->GetMaterialCount();
				FbxSurfaceMaterial* mat = i_node->GetMaterial( 0 );
				assert( mat ); // No material associated with the node...something really wrong?
				if( mat )
					matName = mat->GetName();

				u32 hash = GHashedString::Hash( matName, strlen( matName ) );
				GHashNode<u32, BatchedMesh*>* node = i_meshes[ hash ];

				// for creating a unique entity id that will be used on the game side to identify this mesh.
				int deep = 0;

				if( node )
				{
					bMesh = node->m_data;
					while( bMesh->m_vertices.size() > MAX_BATCH_SIZE )
					{

						BatchedMesh* prev = bMesh;
						bMesh = bMesh->m_next;
						deep++;

						if( !bMesh )
						{

							bMesh = new BatchedMesh();
							strcpy_s( bMesh->m_materialName, 64, matName );
							sprintf( bMesh->m_meshName, "%s%d", matName, meshesProcessed );
							prev->m_next = bMesh;
							break;
						}
					}
				}
				else
				{
					bMesh = new BatchedMesh();
					strcpy_s( bMesh->m_materialName, 64, matName );
					sprintf( bMesh->m_meshName, "%s%d", matName, meshesProcessed );

					i_meshes.Set( hash, bMesh );
					node = i_meshes[ hash ];
				}

				int vertexId = 0; // this is being used, but i'm not too sure about it...
				int polyCount = pMesh->GetPolygonCount();

				//fill up the vertex buffer...
				int vertexCount = pMesh->GetControlPointsCount();
				int indexOffset = bMesh->m_vertices.size();

				//node->m_data.m_vertices.reserve( vertexCount );
				for( int q = 0; q < vertexCount; q++ )
				{

					FbxVector4 vertex = pMesh->GetControlPointAt( q );
					GVertices::s_vertexTNTB theVertex;
					// Assuming that there is no transformation/deformation data...
					theVertex.x = (float)vertex[0];
					theVertex.y = (float)vertex[1];
					theVertex.z = (float)-vertex[2]; // flip?

					bMesh->m_vertices.push_back( theVertex );
				}

				for( int i = 0; i < polyCount; i++ )
				{
					int size = pMesh->GetPolygonSize( i );
					for( int j = 0; j < size; j++ )
					{
						int vertexIndex = pMesh->GetPolygonVertex( i, 2-j );
						FbxVector4 vertexPosition = pVertices[vertexIndex];

						GVertices::s_vertexTNTB& theVertex = bMesh->m_vertices[vertexIndex + indexOffset];

						//now get the UV coords.
						for (l = 0; l < pMesh->GetElementUVCount(); l++)
						{
							int count2 = pMesh->GetElementUVCount();
							FbxGeometryElementUV* leUV = pMesh->GetElementUV( l ); // LE UV!  ...  Right now, we only use the first UV set. 
							// these UV sets can be found in the UV texture editor of Maya.  i'm not sure what they are right now...
							const char* uvName = leUV->GetName();

							switch (leUV->GetMappingMode())
							{
							case FbxGeometryElement::eByControlPoint:
								switch ( leUV->GetReferenceMode() )
								{

									case FbxGeometryElement::eDirect:
									{
										FbxVector2 v2 = leUV->GetDirectArray().GetAt( vertexIndex );
										theVertex.u = (float)v2[0];
										theVertex.v = 1.0f - (float)v2[1];
										break;
									}

									case FbxGeometryElement::eIndexToDirect:
									{
										int id = leUV->GetIndexArray().GetAt( vertexIndex );
										FbxVector2 v2 = leUV->GetDirectArray().GetAt(id);
										theVertex.u = (float)v2[0];
										theVertex.v = 1.0f - (float)v2[1];
										break;
									}

									default:
										break; // other reference modes not shown here!
								}
								break;

								case FbxGeometryElement::eByPolygonVertex:
								{
									//int lTextureUVIndex = pMesh->GetTextureUVIndex(i, 2-j);
									int lTextureUVIndex = pMesh->GetTextureUVIndex(i, 2-j);
									switch (leUV->GetReferenceMode())
									{
										case FbxGeometryElement::eDirect:
										{
											FbxVector2 v2 = leUV->GetDirectArray().GetAt( vertexIndex );
											theVertex.u = (float)v2[0];
											theVertex.v = (float)v2[1];
										}
										break;
										case FbxGeometryElement::eIndexToDirect:
										{
											FbxVector2 v2 = leUV->GetDirectArray().GetAt(lTextureUVIndex);
											theVertex.u = (float)v2[0];
											theVertex.v = 1.0f - (float)v2[1];
											//DEBUG_PRINT( "u = %f, v = %f", theVertex.u, theVertex.v );
										}
										break;
										default:
											break; // other reference modes not shown here!
									}
									break;
								}

								case FbxGeometryElement::eByPolygon: // doesn't make much sense for UVs
								case FbxGeometryElement::eAllSame:   // doesn't make much sense for UVs
								case FbxGeometryElement::eNone:       // doesn't make much sense for UVs
								break;
							}
						}

						// now get the normals...
						for( l = 0; l < pMesh->GetElementNormalCount(); l++ )
						{
							FbxGeometryElementNormal* leNormal = pMesh->GetElementNormal( l); // LE NORMAL.  LE SIGH...
							int count2 = pMesh->GetElementNormalCount();

							//if(leNormal->GetMappingMode() == FbxGeometryElement::eByControlPoint)
							{
								FbxVector4 v4;
								switch( leNormal->GetReferenceMode() )
								{
									case FbxGeometryElement::eDirect:
									{
										v4 = leNormal->GetDirectArray().GetAt( vertexIndex );
										break;
									}
									case FbxGeometryElement::eIndexToDirect:
									{
										int id = leNormal->GetIndexArray().GetAt( vertexIndex );
										v4 = leNormal->GetDirectArray().GetAt(id);
										break;
									}
									default:
										break; // other reference modes not shown here!
								}

								theVertex.nx = (float)v4[0];
								theVertex.ny = (float)v4[1];
								theVertex.nz = (float)-v4[2];
							}
						}

						// get the tangents...
						for( l = 0; l < pMesh->GetElementTangentCount(); l++)
						{
							FbxGeometryElementTangent* leTangent = pMesh->GetElementTangent( l);
							int count2 = pMesh->GetElementTangentCount();

							//if(leTangent->GetMappingMode() == FbxGeometryElement::eByControlPoint)
							{
								FbxVector4 v4;
								switch (leTangent->GetReferenceMode())
								{
									case FbxGeometryElement::eDirect:
									{
										v4 = leTangent->GetDirectArray().GetAt(vertexIndex);
										break;
									}
									case FbxGeometryElement::eIndexToDirect:
									{
										int id = leTangent->GetIndexArray().GetAt(vertexIndex);
										v4 = leTangent->GetDirectArray().GetAt(id);
									}
									default:
										break; // other reference modes not shown here!
								}

								theVertex.tx = (float)v4[0];
								theVertex.ty = (float)v4[1];
								theVertex.tz = (float)-v4[2];
							}
						}

						// get the binormals...
						for( l = 0; l < pMesh->GetElementBinormalCount(); l++)
						{
							FbxGeometryElementBinormal* leBinormal = pMesh->GetElementBinormal( l);
							int count2 = pMesh->GetElementBinormalCount();

							//if( leBinormal->GetMappingMode() == FbxGeometryElement::eByControlPoint )
							{
								FbxVector4 v4;
								switch (leBinormal->GetReferenceMode())
								{
									case FbxGeometryElement::eDirect:
									{
										v4 = leBinormal->GetDirectArray().GetAt( vertexIndex );
										break;
									}
									case FbxGeometryElement::eIndexToDirect:
									{
										int id = leBinormal->GetIndexArray().GetAt( vertexIndex );
										v4 = leBinormal->GetDirectArray().GetAt( id );
										break;
									}
									default:
										break; // other reference modes not shown here!
								}

								theVertex.bx = (float)-v4[0];
								theVertex.by = (float)-v4[1];
								theVertex.bz = (float)v4[2];
							}
						}

						// increment vertexId... still not entirely sure what this is doing...
						vertexId++;

						bMesh->m_indices.push_back( vertexIndex + indexOffset );
					}
				}
			}
		}

		return true;
	}

	bool ProcessGeometryAndAABB( FbxNode* i_node, GHashMap<u32, BatchedMesh*>& i_meshes, AABBData& o_aabb )
	{
		int count = i_node->GetChildCount();
		for( int i = 0; i < count; i++ )
		{
			FbxNode* child = i_node->GetChild( i );
			const char* name = child->GetName();
			if( strcmp( name, "Collision" ) == 0 )
				continue;
			else
				ProcessGeometryAndAABB( child, i_meshes, o_aabb );
		}

		if( !i_node || !i_node->GetNodeAttribute() )
			return false;

		FbxNodeAttribute::EType AttributeType = i_node->GetNodeAttribute()->GetAttributeType();

		if(AttributeType == FbxNodeAttribute::eMesh)
		{
			FbxMesh* pMesh = (FbxMesh*) i_node->GetNodeAttribute();

			FbxGeometryConverter geoConverter( i_node->GetFbxManager() );
			pMesh = (FbxMesh*)geoConverter.Triangulate(pMesh, true);

			FbxVector4* pVertices = pMesh->GetControlPoints();
			int vertexCount = pMesh->GetControlPointsCount();
			meshesProcessed++;

			if( pMesh )
			{

				// add the vertex to the batched mesh...lol.
				// eventually, we'll want a way of importing things that we know are not just scene geometry...
				// and intelligent partitioning of material meshes...
				int l;
				BatchedMesh* bMesh;
				const char* matName;

				// should be 1.
				int materialCount = i_node->GetMaterialCount();
				FbxSurfaceMaterial* mat = i_node->GetMaterial( 0 );
				assert( mat ); // No material associated with the node...something really wrong?
				if( mat )
					matName = mat->GetName();

				u32 hash = GHashedString::Hash( matName, strlen( matName ) );
				GHashNode<u32, BatchedMesh*>* node = i_meshes[ hash ];

				// for creating a unique entity id that will be used on the game side to identify this mesh.
				int deep = 0;

				if( node )
				{
					bMesh = node->m_data;
					while( bMesh->m_vertices.size() > MAX_BATCH_SIZE )
					{

						BatchedMesh* prev = bMesh;
						bMesh = bMesh->m_next;
						deep++;

						if( !bMesh )
						{

							bMesh = new BatchedMesh();
							strcpy_s( bMesh->m_materialName, 64, matName );
							sprintf( bMesh->m_meshName, "%s%d", matName, meshesProcessed );
							prev->m_next = bMesh;
							break;
						}
					}
				}
				else
				{
					bMesh = new BatchedMesh();
					strcpy_s( bMesh->m_materialName, 64, matName );
					sprintf( bMesh->m_meshName, "%s%d", matName, meshesProcessed );

					i_meshes.Set( hash, bMesh );
					node = i_meshes[ hash ];
				}

				int vertexId = 0; // this is being used, but i'm not too sure about it...
				int polyCount = pMesh->GetPolygonCount();

				//fill up the vertex buffer...
				int vertexCount = pMesh->GetControlPointsCount();
				int indexOffset = bMesh->m_vertices.size();

				//node->m_data.m_vertices.reserve( vertexCount );
				for( int q = 0; q < vertexCount; q++ )
				{

					FbxVector4 vertex = pMesh->GetControlPointAt( q );
					GVertices::s_vertexTNTB theVertex;
					// Assuming that there is no transformation/deformation data...
					theVertex.x = (float)vertex[0];
					theVertex.y = (float)vertex[1];
					theVertex.z = (float)-vertex[2]; // flip?

					// blah.  should just read in collision data from collision.  do this later.
					// this is fucking up branch prediction something firece.
					if( theVertex.x > o_aabb.m_maxX )
						o_aabb.m_maxX = theVertex.x;
					else if( theVertex.x < o_aabb.m_minX )
						o_aabb.m_minX = theVertex.x;
					if( theVertex.y > o_aabb.m_maxY )
						o_aabb.m_maxY = theVertex.y;
					else if( theVertex.y < o_aabb.m_minY )
						o_aabb.m_minY = theVertex.y;
					if( theVertex.z > o_aabb.m_maxZ )
						o_aabb.m_maxZ = theVertex.z;
					else if( theVertex.z < o_aabb.m_minZ )
						o_aabb.m_minZ = theVertex.z;

					bMesh->m_vertices.push_back( theVertex );
				}

				for( int i = 0; i < polyCount; i++ )
				{
					int size = pMesh->GetPolygonSize( i );
					for( int j = 0; j < size; j++ )
					{
						int vertexIndex = pMesh->GetPolygonVertex( i, 2-j );
						FbxVector4 vertexPosition = pVertices[vertexIndex];

						GVertices::s_vertexTNTB& theVertex = bMesh->m_vertices[vertexIndex + indexOffset];

						//now get the UV coords.
						for (l = 0; l < pMesh->GetElementUVCount(); l++)
						{
							int count2 = pMesh->GetElementUVCount();
							FbxGeometryElementUV* leUV = pMesh->GetElementUV( l); // LE UV!  ...

							switch (leUV->GetMappingMode())
							{
							case FbxGeometryElement::eByControlPoint:
								switch ( leUV->GetReferenceMode() )
								{

									case FbxGeometryElement::eDirect:
									{
										FbxVector2 v2 = leUV->GetDirectArray().GetAt( vertexIndex );
										theVertex.u = (float)v2[0];
										theVertex.v = 1.0f - (float)v2[1];
										break;
									}

									case FbxGeometryElement::eIndexToDirect:
									{
										int id = leUV->GetIndexArray().GetAt( vertexIndex );
										FbxVector2 v2 = leUV->GetDirectArray().GetAt(id);
										theVertex.u = (float)v2[0];
										theVertex.v = 1.0f - (float)v2[1];
										break;
									}

									default:
										break; // other reference modes not shown here!
								}
								break;

								case FbxGeometryElement::eByPolygonVertex:
								{
									int lTextureUVIndex = pMesh->GetTextureUVIndex(i, 2-j);
									switch (leUV->GetReferenceMode())
									{
										case FbxGeometryElement::eDirect:
										case FbxGeometryElement::eIndexToDirect:
										{
											FbxVector2 v2 = leUV->GetDirectArray().GetAt(lTextureUVIndex);
											theVertex.u = (float)v2[1];
											theVertex.v = (float)v2[0];
											break;
										}
										default:
											break; // other reference modes not shown here!
									}
									break;
								}

								case FbxGeometryElement::eByPolygon: // doesn't make much sense for UVs
								case FbxGeometryElement::eAllSame:   // doesn't make much sense for UVs
								case FbxGeometryElement::eNone:       // doesn't make much sense for UVs
								break;
							}
						}

						// now get the normals...
						for( l = 0; l < pMesh->GetElementNormalCount(); l++ )
						{
							FbxGeometryElementNormal* leNormal = pMesh->GetElementNormal( l); // LE NORMAL.  LE SIGH...
							int count2 = pMesh->GetElementNormalCount();

							//if(leNormal->GetMappingMode() == FbxGeometryElement::eByControlPoint)
							{
								FbxVector4 v4;
								switch( leNormal->GetReferenceMode() )
								{
									case FbxGeometryElement::eDirect:
									{
										v4 = leNormal->GetDirectArray().GetAt( vertexIndex );
										break;
									}
									case FbxGeometryElement::eIndexToDirect:
									{
										int id = leNormal->GetIndexArray().GetAt( vertexIndex );
										v4 = leNormal->GetDirectArray().GetAt(id);
										break;
									}
									default:
										break; // other reference modes not shown here!
								}

								theVertex.nx = (float)v4[0];
								theVertex.ny = (float)v4[1];
								theVertex.nz = (float)-v4[2];
							}
						}

						// get the tangents...
						for( l = 0; l < pMesh->GetElementTangentCount(); l++)
						{
							FbxGeometryElementTangent* leTangent = pMesh->GetElementTangent( l);
							int count2 = pMesh->GetElementTangentCount();

							//if(leTangent->GetMappingMode() == FbxGeometryElement::eByControlPoint)
							{
								FbxVector4 v4;
								switch (leTangent->GetReferenceMode())
								{
									case FbxGeometryElement::eDirect:
									{
										v4 = leTangent->GetDirectArray().GetAt(vertexIndex);
										break;
									}
									case FbxGeometryElement::eIndexToDirect:
									{
										int id = leTangent->GetIndexArray().GetAt(vertexIndex);
										v4 = leTangent->GetDirectArray().GetAt(id);
									}
									default:
										break; // other reference modes not shown here!
								}

								theVertex.tx = (float)v4[0];
								theVertex.ty = (float)v4[1];
								theVertex.tz = (float)-v4[2];
							}
						}

						// get the binormals...
						for( l = 0; l < pMesh->GetElementBinormalCount(); l++)
						{
							FbxGeometryElementBinormal* leBinormal = pMesh->GetElementBinormal( l);
							int count2 = pMesh->GetElementBinormalCount();

							//if( leBinormal->GetMappingMode() == FbxGeometryElement::eByControlPoint )
							{
								FbxVector4 v4;
								switch (leBinormal->GetReferenceMode())
								{
									case FbxGeometryElement::eDirect:
									{
										v4 = leBinormal->GetDirectArray().GetAt( vertexIndex );
										break;
									}
									case FbxGeometryElement::eIndexToDirect:
									{
										int id = leBinormal->GetIndexArray().GetAt( vertexIndex );
										v4 = leBinormal->GetDirectArray().GetAt( id );
										break;
									}
									default:
										break; // other reference modes not shown here!
								}

								theVertex.bx = (float)-v4[0];
								theVertex.by = (float)-v4[1];
								theVertex.bz = (float)v4[2];
							}
						}

						// increment vertexId... still not entirely sure what this is doing...
						vertexId++;

						bMesh->m_indices.push_back( vertexIndex + indexOffset );
					}
				}
			}
		}

		return true;
	}
}