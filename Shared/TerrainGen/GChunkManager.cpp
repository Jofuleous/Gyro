#include "GChunkManager.h"

void GChunkManager::NewSave( int i_seed )
{
	// allocate big ol' chunk o' memory.
	//m_activeChunks = new GChunk[ GACTIVE_DIM * GACTIVE_DIM * GACTIVE_DIM ];

}

void GChunkManager::LoadSave( )
{

}

void GChunkManager::Unload( )
{

}

GChunk* GChunkManager::ActiveChunk( int x, int y, int z )
{
	return 0;
}

GChunk* GChunkManager::ChunkFromCoord( float x, float y, float z )
{
	return 0;
}
