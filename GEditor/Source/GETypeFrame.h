#ifndef __GEDITORTYPEFRAME_H_
#define __GEDITORTYPEFRAME_H_

#include <wx/wx.h>
#include "wx/window.h"
#include "GEditorObject.h"
#include "GEWayPath.h"

class GETypeFrame : public wxFrame
{
public:
	GETypeFrame( wxWindow* parent, wxWindowID id, const wxString &title, const wxPoint &position, const wxSize& size, long style )
	: wxFrame( parent, id, title, position, size, style )
	{

	}

    virtual ~GETypeFrame()
	{
	}

	virtual void DisplayEditorObject( GEditorObject* i_object ) = 0;
	virtual void DisplayWayNode( u16 i_node ) { } // giant hack.
};

#endif