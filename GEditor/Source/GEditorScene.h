#ifndef _GEDITORSCENE_H_
#define _GEDITORSCENE_H_

#include <vector>
#include "Utility/Singleton.h"
#include "Containers/GHashMap.h"
#include "Utility/GHashedString.h"
#include "Partition/GOctree.h"
#include "Math/Utilities.h"
#include "Utility/GLock.h"
#include "GEditorCamera.h"
#include "FBXProcessor.h"
#include "GEAxisMarker.h"
#include "GEWayPath.h"


class GEditorObject;
class GEditorFrame;

class GEditorScene
{
public:

	enum EditMode 
	{
		OBJECTS,
		NAVIGATION,
	};

	friend Singleton<GEditorScene>;
	bool									Initialize( GEditorFrame* i_frame );
	void									Shutdown( );
	void									Update( );
	void									HandleMouseInput( );
	void									Render( );
	void									EndUpdate( );

	//keeping it basic for now
	void									AddObject( GEditorObject* i_object );
	void									RemoveObject( GEditorObject* i_object );
	void									MoveSelectedObjects( GVector3& i_delta );
	void									AttemptRaySelect( GVector3& i_direction, bool shiftDown );

	bool									LoadWorldFBX( const char* i_filename );
	bool									LoadLuaScene( const char* i_filename );
	bool									SaveMapFile( const char* i_filePath );
	bool									ExportMapFiles( const char* i_filePath );
	bool									ExportStaticMapData( const char* i_filePath );
	bool									WriteStaticMapData( const char* i_filePath, SceneExportData& i_data );
	bool									QuickExport( );
	bool									QuickSave( );
	bool									LoadBlankScene( );
	bool									UnloadScene( );
	GEditorObject*							GetObjectByName( const char* i_objectName ) const;
	GEditorObject*							GetSelectedObject() { return m_selectedObject; }

	GEditorObject*							CreateTrigger( );
	GEditorObject*							CreateStatic( );
	u16										CreateWaypoint( );

	void									SelectObject( GEditorObject* i_object, bool i_multiple );

	void									UnSelectObject( GEditorObject* i_object );

	// unselects everything.
	void									UnSelectAll( );

	const std::vector<GEditorObject*>&		GetObjects( ) const { return m_allObjects; }

	std::vector<GEditorObject*>				m_allObjects;
	std::vector<GEditorObject*>				m_selectedObjects;
	GEditorObject*							m_selectedObject;

	GEditorCamera							m_editorCamera;

	char*									m_sceneName;
	char*									m_filePath;


	// used for collision later on when we'll be placing AI nodes...maybe.
	GOctree< GMathUtil::ColTriangle >		m_colTree;

	GLock									m_renderLock;

	GEAxisMarker							m_axisMarker;

	// This is a hack so we can update the frame.  There should ever only be one editor frame...but i doubt I can make it a singleton for now. 
	GEditorFrame*							m_editorFrame;

	GEWayPath								m_path;	// for now, we're hackishly saying there is only one path...


	int		m_objectsCreated;


	// cleanup: draw raycast line
	D3DXVECTOR3			m_start;
	D3DXVECTOR3			m_end;
	bool				m_drawDebug;

protected:
	GEditorScene() : m_colTree( 25000.0f, 200, GVector3( 0.0f, 0.0f, 0.0f ), true ),
		m_drawDebug( false ),
		m_objectsCreated( 0 ),
		m_selectedObject( NULL ),
		m_filePath( 0 ),
		m_sceneName( 0 )
		{}

};

typedef Singleton<GEditorScene> g_EditorScene;

#endif