#ifndef __GCHUNK_MANAGER_H_
#define __GCHUNK_MANAGER_H_

#include "Utility/Singleton.h"
#include "GChunk.h"

#define GACTIVE_DIM 16;

class GChunkManager // : public Singleton<GChunkManager>
{
public:

	void NewSave( int i_seed );
	void LoadSave( );
	void Unload( );

	GChunk* ActiveChunk( int x, int y, int z );
	GChunk* ChunkFromCoord( float x, float y, float z );

	GChunk* m_activeChunks;
};

#define g_ChunkManager Singleton<GChunkManager>::Get()

#endif