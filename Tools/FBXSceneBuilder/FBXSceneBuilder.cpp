#include "FBXSceneBuilder.h"
#include "SceneData.h"

#include <vector>
#include <stdio.h>   /* required for file operations */
#include <conio.h>  /* for clrscr */
#include <dos.h>  /* for delay */
#include <iostream>
#include <fstream>

#include "Containers/GHashMap.h"
#include "Utility/GHashedString.h"
#include "../../Gyro/Source/Collision/GCollisionMesh.h"

FbxManager* FBXSceneBuilder::g_FbxSdkManager = NULL;

bool WriteToFile( const char* i_fileName_target, SceneData& m_sceneData )
{
	FILE* targetFile = fopen( i_fileName_target, "w+b" );
	
	// write out the number of meshes...
	int numberOfMeshes = 0;
	// this is weird, but since materials can be broken up into multiple meshes to get past
	// the maximum primitive count, we need to go through all the broken up material meshes.
	for( u32 i = 0; i < m_sceneData.m_batchedMeshes.Size(); i++ )
	{
		GHashNode< u32, BatchedMesh* >* node = m_sceneData.m_batchedMeshes.Index( i );
		if( !node )
			continue;

		while( node )
		{
			BatchedMesh* current = node->m_data;

			while( current != NULL )
			{
				numberOfMeshes += 1;
				current = current->m_next;
			}
			node = node->m_next;
		}
	}

	fwrite( &numberOfMeshes, sizeof( u32 ), 1, targetFile );
	u64 bleh = 0;
	fwrite( &bleh, sizeof( u64 ), 1, targetFile );	// we're going to come back and overwrite this...


	u64 dataOffsetCheck = 0;

	for( int i = 0; i < (int)m_sceneData.m_batchedMeshes.Size(); i++ )
	{
		GHashNode< u32, BatchedMesh* >* node = m_sceneData.m_batchedMeshes.Index( i );
		if( !node )
			continue;

		while( node )
		{

			BatchedMesh* mesh = node->m_data;
			while( mesh != NULL )
			{

				// add .matb to the end of the material name..
				u32 eNameLength = strlen( mesh->m_meshName )+1;
				u32 mNameLength = strlen( mesh->m_materialName )+ 1 + 5;

				char* materialName =(char*) malloc( mNameLength );
				sprintf( materialName, "%s.matb",  mesh->m_materialName );

				fwrite( &eNameLength, sizeof( u32 ), 1, targetFile );
				dataOffsetCheck += sizeof( u32 );

				fwrite( mesh->m_meshName, sizeof( char ), eNameLength, targetFile );
				dataOffsetCheck += eNameLength;

				fwrite( &mNameLength, sizeof( u32 ), 1, targetFile );
				dataOffsetCheck += sizeof( u32 );

				fwrite( materialName, sizeof( char ), mNameLength, targetFile );
				dataOffsetCheck += mNameLength;

				// later, if we wind up going by entity, we'll want to write out matrix transformation data...
				u32 polyCount = mesh->m_indices.size() / 3;
				u32 vertexCount = mesh->m_vertices.size();

				//wow
				//http://stackoverflow.com/questions/2923272/how-to-convert-vector-to-array-c
				fwrite( &vertexCount, sizeof( u32 ), 1, targetFile );
				dataOffsetCheck += sizeof( u32 );

				fwrite( &polyCount, sizeof( u32 ), 1, targetFile );
				dataOffsetCheck += sizeof( u32 );

				fwrite(&mesh->m_vertices[0], sizeof( s_vertexTNTB ), mesh->m_vertices.size(), targetFile );
				dataOffsetCheck += mesh->m_vertices.size() * sizeof( s_vertexTNTB );

				fwrite(&mesh->m_indices[0], sizeof( UINT16 ), mesh->m_indices.size(), targetFile );
				dataOffsetCheck += mesh->m_indices.size() * sizeof( u16 );

				mesh = mesh->m_next;
			}

			node = node->m_next;
		}
	}

	u32 collisionPos = ftell( targetFile );

	fseek( targetFile, 4, SEEK_SET );
	u64 test = ftell( targetFile );
	fwrite( &collisionPos, sizeof( u64 ), 1, targetFile );
	fseek( targetFile, collisionPos, SEEK_SET );
	test = ftell( targetFile );
	
	/*
	u32 numCollisionMeshes = m_sceneData.m_collisionMeshes.size();
	fwrite( &numCollisionMeshes, sizeof( u32 ), 1, targetFile );
	for( int m = 0; m < numCollisionMeshes; m++ )
	{
		u32 polyCount = m_sceneData.m_collisionMeshes[m].m_indices.size() / 3;
		u32 vertexCount = m_sceneData.m_collisionMeshes[m].m_vertexCount;
		fwrite( &vertexCount, sizeof( u32 ), 1, targetFile );
		fwrite( &polyCount, sizeof( u32 ), 1, targetFile );
		fwrite( m_sceneData.m_collisionMeshes[m].m_vertices, sizeof( GVector3 ), m_sceneData.m_collisionMeshes[m].m_vertexCount, targetFile );
		fwrite( &m_sceneData.m_collisionMeshes[m].m_indices[0], sizeof( u16 ), m_sceneData.m_collisionMeshes[m].m_indices.size(), targetFile );
		//delete m_sceneData.m_collisionMeshes[m].m_vertices; // may be able to get away with not doing this, lol...no real point here.
	}
	*/

	// write out the octree data.
	fwrite( &m_sceneData.m_colTree.m_center, sizeof( GVector3 ), 1, targetFile );
	fwrite( &m_sceneData.m_colTree.m_dimension, sizeof( float ), 1, targetFile );
	fwrite( &m_sceneData.m_colTree.m_maxDepth, sizeof( int ), 1, targetFile );

	m_sceneData.m_colTree.SerializeNodes( targetFile, m_sceneData.m_colTree.m_root );

	fclose( targetFile );

	return true;
}

void ProcessCollisionMesh( FbxNode* i_node, FbxMesh* pMesh, SceneData& i_sceneData )
{

}

/*
// eventually, we'll want support for types of non-mesh...
void ProcessCollisionNodes( FbxNode* i_node, SceneData& i_sceneData )
{
	int count = i_node->GetChildCount();
	for( int i = 0; i < count; i++ )
	{
		FbxNode* child = i_node->GetChild( i );

		ProcessCollisionNodes( child, i_sceneData );
	}

	if( !i_node || !i_node->GetNodeAttribute() )
		return;

	FbxNodeAttribute::EType AttributeType = i_node->GetNodeAttribute()->GetAttributeType();

	if(AttributeType == FbxNodeAttribute::eMesh)
	{
		FbxMesh* pMesh = (FbxMesh*) i_node->GetNodeAttribute();

		FbxGeometryConverter geoConverter( i_node->GetFbxManager() );
		pMesh = geoConverter.TriangulateMesh( pMesh );
		FbxVector4* pVertices = pMesh->GetControlPoints();
		ColMesh colMesh;
		colMesh.m_vertices = new GVector3[pMesh->GetControlPointsCount()];
		colMesh.m_vertexCount = pMesh->GetControlPointsCount();


		for( int q = 0; q < colMesh.m_vertexCount; q++ )
		{

			FbxVector4 vertex = pMesh->GetControlPointAt( q );
			colMesh.m_vertices[q] = GVector3( vertex [0], vertex[1], -vertex[2] );
		}

		int vertexId = 0; // this is being used, but i'm not too sure about it...
		int polyCount = pMesh->GetPolygonCount();
		for( int p = 0; p < polyCount; p++ )
		{
			int size = pMesh->GetPolygonSize( p );
			for( int v = 0; v < size; v++ )
			{
				int vertexIndex = pMesh->GetPolygonVertex( p, 2-v );
				colMesh.m_indices.push_back( vertexIndex );
			}
		}

		i_sceneData.m_collisionMeshes.push_back( colMesh );
	}

}
*/

void ProcessCollisionNodes( FbxNode* i_node, SceneData& i_sceneData )
{
	int count = i_node->GetChildCount();
	for( int i = 0; i < count; i++ )
	{
		FbxNode* child = i_node->GetChild( i );

		ProcessCollisionNodes( child, i_sceneData );
	}

	if( !i_node || !i_node->GetNodeAttribute() )
		return;

	FbxNodeAttribute::EType AttributeType = i_node->GetNodeAttribute()->GetAttributeType();

	if(AttributeType == FbxNodeAttribute::eMesh)
	{
		FbxMesh* pMesh = (FbxMesh*) i_node->GetNodeAttribute();

		FbxGeometryConverter geoConverter( i_node->GetFbxManager() );
		pMesh = geoConverter.TriangulateMesh( pMesh );
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
			i_sceneData.m_colTree.AddObject( triangle ); 
		}
	}
}

bool ProcessNodes( FbxNode* i_node, SceneData& i_sceneData )
{
	int count = i_node->GetChildCount();
	for( int i = 0; i < count; i++ )
	{
		FbxNode* child = i_node->GetChild( i );
		if( strcmp( child->GetName(), "Collision" ) == 0 )
		{
			ProcessCollisionNodes( child, i_sceneData );
			continue;
		}
		else
			ProcessNodes( child, i_sceneData );
	}

	if( !i_node || !i_node->GetNodeAttribute() )
		return false;

	FbxNodeAttribute::EType AttributeType = i_node->GetNodeAttribute()->GetAttributeType();

	if(AttributeType == FbxNodeAttribute::eMesh)
	{
		FbxMesh* pMesh = (FbxMesh*) i_node->GetNodeAttribute();

		FbxGeometryConverter geoConverter( i_node->GetFbxManager() );
		pMesh = geoConverter.TriangulateMesh( pMesh );

		FbxVector4* pVertices = pMesh->GetControlPoints();
		int vertexCount = pMesh->GetControlPointsCount();

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
			GHashNode<u32, BatchedMesh*>* node = i_sceneData.m_batchedMeshes[ hash ];

			// for creating a unique entity id that will be used on the game side to identify this mesh.
			int deep = 0;

			if( node )
			{
				bMesh = node->m_data;
				while( bMesh->m_vertices.size() > 10000 )
				{

					BatchedMesh* prev = bMesh;
					bMesh = bMesh->m_next;
					deep++;

					if( !bMesh )
					{

						bMesh = new BatchedMesh();
						bMesh->m_materialName = matName;

						// create new mesh name.
						int len = strlen( matName ) + 4 ; // 4 = probably wont have 9999 meshes for a material.
						bMesh->m_meshName = (char*) malloc( len );
						sprintf( bMesh->m_meshName, "%s%d", matName, deep );

						prev->m_next = bMesh;
						break;
					}
				}
			}
			else
			{
				bMesh = new BatchedMesh();
				bMesh->m_materialName = matName;

				// create new mesh name.
				int len = strlen( matName ) + 4 ; // 4 = probably wont have 9999 meshes for a material.
				bMesh->m_meshName = (char*) malloc( len );
				sprintf( bMesh->m_meshName, "%s%d", matName, deep );

				i_sceneData.m_batchedMeshes.Set( hash, bMesh );
				node = i_sceneData.m_batchedMeshes[ hash ];
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
				s_vertexTNTB theVertex;
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

					s_vertexTNTB& theVertex = bMesh->m_vertices[vertexIndex + indexOffset];

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

						if(leNormal->GetMappingMode() == FbxGeometryElement::eByControlPoint)
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

						if(leTangent->GetMappingMode() == FbxGeometryElement::eByControlPoint)
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

						if( leBinormal->GetMappingMode() == FbxGeometryElement::eByControlPoint )
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


bool FBXSceneBuilder::Build_derived( const char* i_fileName_source, const char* i_fileName_target ) const
{
	if( g_FbxSdkManager == NULL )
	{
		g_FbxSdkManager = FbxManager::Create();
		FbxIOSettings* pIOsettings = FbxIOSettings::Create(g_FbxSdkManager, IOSROOT ); // IOSRoot? wat
		g_FbxSdkManager->SetIOSettings(pIOsettings);
	}
	using namespace std;

	FbxImporter* pImporter = FbxImporter::Create(g_FbxSdkManager,"The BroShow Importer");
	FbxScene* fbxScene = FbxScene::Create(g_FbxSdkManager,"Scene of a Magnificent Bro");

	bool succeeded = pImporter->Initialize( i_fileName_source, -1, g_FbxSdkManager->GetIOSettings() );
	if( !succeeded )
	{
		std::cerr << "Could not initialize file " << i_fileName_source << ".  Possibly could not find the file?";
		return false;
	}

	succeeded = pImporter->Import( fbxScene );
	if( !succeeded )
	{
		std::cerr << "Could not import file " << i_fileName_source << ".  Possibly could not find the file?";
		return false;
	}

	pImporter->Destroy();

	FbxNode* rootNode = fbxScene->GetRootNode();

	SceneData sceneData;

	ProcessNodes( rootNode, sceneData );

	// go through all the nodes of the map and write those to our scene file...
	WriteToFile( i_fileName_target, sceneData );
	return true;
}